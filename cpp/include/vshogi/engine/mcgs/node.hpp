#ifndef VSHOGI_ENGINE_MCGS_NODE_HPP
#define VSHOGI_ENGINE_MCGS_NODE_HPP

#include "vshogi/common/game.hpp"
#include "vshogi/engine/contiguous_buffer.hpp"
#include "vshogi/engine/mcgs/edge.hpp"

namespace vshogi::engine::mcgs
{

class Node
{
public:
    Node() = default;
    ~Node() = default; // Rule 1/5 destructor
    Node(const Node& other) = delete; // Rule 2/5 copy constructor
    Node& operator=(const Node& other) = delete; // Rule 3/5 copy assignment
    Node(Node&& other); // Rule 4/5 move constructor
    Node& operator=(Node&& other); // Rule 5/5 move assignment

    Edge* select();
    Edge* select_unexplored();
    template <class P>
    void simulate(const Game<P>& g);
    template <class P>
    void expand(
        ContiguousBuffer<Edge>& buffer,
        const Game<P>& g,
        const float policy_logits[Configuration<P>::dlshogi_policy_size]
        = nullptr);
    void backprop();

    template <class P>
    void simulate_ongoing_and_expand(
        ContiguousBuffer<Edge>& buffer,
        const Game<P>& game,
        const float value,
        const float* const policy_logits = nullptr);
    void simulate_mate_and_expand(
        ContiguousBuffer<Edge>& buffer, const move_t action);

    // clang-format off
    bool has_child() const { return static_cast<bool>(m_child); }
    bool is_leaf() const { return not has_child(); }
    const Edge* get_child() const { return m_child; }
    const Node* get_child_node() const { return m_child ? m_child->get_child() : nullptr; }
    const Node* get_child_node_of(const move_t action) const;
    float get_value() const { return m_value; }
    float get_prior_value() const { return m_prior_value; }
    uint get_visits() const { return m_visits; }
    uint count_childs() const { uint out = 0u; for (auto e = m_child; e && (e->m_parent == this); ++e) { ++out; } return out; }
    bool is_mate() const { return (m_value == 1.f) or (m_value == -1.f); }
    bool is_mate_to_win() const { return m_value == 1.f; }
    bool is_mate_to_lose() const { return m_value == -1.f; }
    Edge* child() { return m_child; }
    void init() { std::memset(reinterpret_cast<void*>(this), 0, sizeof(Node)); }
    void destruct() { for (auto e = m_child; e && (e->m_parent == this); ++e) e->destruct(); init(); }
    auto num_parents() const { return m_num_parents; }
    // clang-format on

    friend class Edge;

private:
    Edge* m_child;

    // ZobristHashType m_hash;
    float m_value;
    float m_prior_value;
    uint m_visits;
    uint m_num_parents;

    float first_play_urgency() const;
    template <class P, GenEnum GenType>
    void expand_by_generator(
        ContiguousBuffer<Edge>& buffer,
        const Game<P>& g,
        const float* const policy_logits);
};

inline Node::Node(Node&& other)
{
    m_child = other.m_child;
    m_value = other.m_value;
    m_prior_value = other.m_prior_value;
    m_visits = other.m_visits;
    m_num_parents = other.m_num_parents;

    const auto n = other.count_childs();
    for (auto e = m_child; e < m_child + n; ++e) {
        e->m_parent = this;
    }

    other.init();
}

inline Node& Node::operator=(Node&& other)
{
    if (this == &other)
        return *this;
    m_child = other.m_child;
    m_value = other.m_value;
    m_prior_value = other.m_prior_value;
    m_visits = other.m_visits;
    m_num_parents = other.m_num_parents;

    const auto n = other.count_childs();
    for (auto e = m_child; e < m_child + n; ++e) {
        e->m_parent = this;
    }

    other.init();
    return *this;
}

template <class P>
void Node::simulate(const Game<P>& g)
{
    if (m_visits)
        return;

    const auto result = g.get_result();
    if (result == ONGOING)
        return;

    const auto turn = g.get_turn();
    if (result == DRAW) {
        m_value = (turn == BLACK) ? -0.2f : 0.2f;
        m_prior_value = m_value;
    } else {
        const auto winner = (result == BLACK_WIN) ? BLACK : WHITE;
        m_value = (winner == turn) ? 1.f : -1.f;
        m_prior_value = m_value;
    }
}

template <class P>
void Node::expand(
    ContiguousBuffer<Edge>& buffer,
    const Game<P>& game,
    const float policy_logits[Configuration<P>::dlshogi_policy_size])
{
    if (game.in_check()) {
        expand_by_generator<P, GenEnum::EVADE>(buffer, game, policy_logits);
    } else {
        expand_by_generator<P, GenEnum::LEGAL>(buffer, game, policy_logits);
    }
}

template <class P>
void Node::simulate_ongoing_and_expand(
    ContiguousBuffer<Edge>& buffer,
    const Game<P>& game,
    const float value,
    const float* const policy_logits)
{
    m_prior_value = std::clamp(value, -0.99f, 0.99f);
    m_value = m_prior_value;
    expand(buffer, game, policy_logits);
}

template <class P, GenEnum GenType>
void Node::expand_by_generator(
    ContiguousBuffer<Edge>& buffer,
    const Game<P>& game,
    const float* const policy_logits)
{
    std::vector<std::pair<float, move_t>> lm_pairs{};
    lm_pairs.reserve(128u);
    const auto turn = game.get_turn();
    for (auto gen = MoveGenerator<P, GenType>(game.get_state()); gen; ++gen) {
        const move_t action = *gen;
        const auto index = MoveTraits<P>::to_policy_index(action, turn);
        const float p = policy_logits ? policy_logits[index] : 0.f;
        lm_pairs.emplace_back(p, action);
    }
    std::sort(lm_pairs.begin(), lm_pairs.end(), std::greater<>());

    m_child = buffer.next();
    float max_logit = -std::numeric_limits<float>::infinity();
    for (auto&& pair : lm_pairs) {
        if (buffer.is_full())
            break;
        buffer.emplace_next(this, pair.first, pair.second);
        max_logit = std::max(max_logit, pair.first);
    }

    if (m_child == buffer.next()) { // no child expanded.
        m_child = nullptr;
        return;
    }
    if (not buffer.is_full())
        buffer.next()->init();
    float sumexp = 0.f;
    for (Edge* e = m_child; e && (e->m_parent == this); ++e) {
        e->m_prior_proba -= max_logit;
        e->m_prior_proba = std::exp(e->m_prior_proba);
        sumexp += e->m_prior_proba;
    }
    for (Edge* e = m_child; e && (e->m_parent == this); ++e) {
        e->m_prior_proba /= sumexp;
    }
}

inline Edge::Edge(Edge&& other)
{
    m_parent = other.m_parent;
    m_child = other.m_child;
    m_prior_proba = other.m_prior_proba;
    m_visits = other.m_visits;
    m_action = other.m_action;
    m_mate = other.m_mate;
    other.init();

    assert(m_parent->m_child);
    if (m_parent->m_child == &other)
        m_parent->m_child = this;
}

inline Edge& Edge::operator=(Edge&& other)
{
    if (this == &other)
        return *this;
    m_parent = other.m_parent;
    m_child = other.m_child;
    m_prior_proba = other.m_prior_proba;
    m_visits = other.m_visits;
    m_action = other.m_action;
    m_mate = other.m_mate;
    other.init();

    assert(m_parent->m_child);
    if (m_parent->m_child == &other)
        m_parent->m_child = this;

    return *this;
}

inline float Edge::get_value() const
{
    if (is_mate_to_win())
        return 1.f;
    if (is_mate_to_lose())
        return -1.f;
    return m_child ? -m_child->get_value() : -1.f;
}

inline void Edge::backprop()
{
    if (m_child) {
        m_child->backprop();
        if (m_child->is_mate()) {
            m_mate = m_child->is_mate_to_win() ? -1 : 1; // flip win/loss
        }
    }
    ++m_visits;
}

inline void Edge::destruct()
{
    if (m_child) {
        assert(m_child->m_num_parents);
        --(m_child->m_num_parents);
        if (m_child->num_parents() == 0u)
            m_child->destruct();
    }
    init();
}

inline Node& Edge::set_child(Node& c)
{
    m_child = &c;
    ++(c.m_num_parents);
    if (is_mate_to_win()) {
        c.m_prior_value = -1.f;
        c.m_value = -1.f;
    }
    return c;
}

inline Node* Edge::pop_child()
{
    Node* c = m_child;
    if (c) {
        assert(c->m_num_parents == 1u); // other parents should be removed
        --(c->m_num_parents);
    }
    m_child = nullptr;
    return c;
}

} // namespace vshogi::engine::mcgs

#endif // VSHOGI_ENGINE_MCGS_NODE_HPP
