#ifndef VSHOGI_ENGINE_MCTS_NODE_HPP
#define VSHOGI_ENGINE_MCTS_NODE_HPP

#include "vshogi/common/game.hpp"
#include "vshogi/common/move.hpp"
#include "vshogi/common/utils.hpp"

/**
 * @brief Monte Carlo Tree Search
 *
 * https://en.wikipedia.org/wiki/Monte_Carlo_tree_search#Principle_of_operation
 */
namespace vshogi::engine::mcts
{

template <class P>
class Node
{
    using MT = MoveTraits<P>;

private:
    move_t m_action;
    float m_proba;
    uint m_visit_count;
    uint m_visit_count_by_random;
    float m_sqrt_visit_count;
    float m_q_value;
    bool m_is_mate;
    Node* m_parent;
    std::unique_ptr<Node> m_sibling;
    std::unique_ptr<Node> m_child;
    Node* m_most_visited_child;

public:
    Node();
    Node(const move_t& action, const float proba);
    ~Node() = default; // Rule 1/5 destructor
    Node(const Node& other) = default; // Rule 2/5 copy constructor
    Node& operator=(const Node& other) = default; // Rule 3/5 copy assignment
    Node(Node&& other) = default; // Rule 4/5 move constructor
    Node& operator=(Node&& other) = default; // Rule 5/5 move assignment
    void init();
    // clang-format off
    move_t get_action() const { return m_action; }
    float get_proba() const { return m_proba; }
    uint get_visit_count() const { return m_visit_count; }
    uint get_visit_count_excluding_random() const { return m_visit_count - m_visit_count_by_random; }
    float get_q_value() const { return m_q_value; }
    bool is_mate() const { return m_is_mate; }
    bool is_mate_to_win() const { return m_is_mate && (m_q_value > 0.f); }
    bool is_mate_to_lose() const { return m_is_mate && (m_q_value < 0.f); }
    const Node* get_parent() const { return m_parent; }
    const Node* get_sibling() const { return m_sibling.get(); }
    const Node* get_child() const { return m_child.get(); }
    const Node* get_most_visited_child() const { return m_most_visited_child; }
    bool has_child() const { return static_cast<bool>(m_child); }
    // clang-format on
    float get_q_value(const uint greedy_depth) const;
    uint get_num_child() const;
    const Node* get_child(const move_t& action) const;
    Node& apply(const move_t& action);

    /**
     * @note Users must check if the node has at least one child before
     * calling this method.
     */
    Node* select(Game<P>& g, const float& c_puct, const float& p_random);
    void simulate(const Game<P>& g);
    void expand(
        const std::vector<move_t>& actions,
        const ColorEnum& turn,
        const float* const policy_logits);
    void simulate_mate_and_expand(const move_t& action);
    void simulate_ongoing_and_expand(
        const std::vector<move_t>& actions,
        const ColorEnum& turn,
        const float value,
        const float* const policy_logits);
    Node* backprop(const float v, Node* const child);

private:
    // select
    Node* select_best_or_random_child(const float c_puct, const float p_random);
    bool select_best_over_random(const float p_random);
    Node* select_best_child(const float c_puct);
    Node* select_random_child();
    float first_play_urgency() const;
    float puct_score_of(
        const Node* const c, const float c_puct, const float q_fpu) const;

    // backprop
    void update_most_visited_child(Node* const candidate);
    bool all_childs_are_mate_to_win() const;
};

template <class P>
Node<P>::Node()
    : m_action{}, m_proba(0.f), m_visit_count(0u), m_visit_count_by_random(0u),
      m_sqrt_visit_count(0.f), m_q_value(0.f), m_is_mate(false),
      m_parent(nullptr), m_sibling(nullptr), m_child(nullptr),
      m_most_visited_child(nullptr)
{
}

template <class P>
Node<P>::Node(const move_t& action, const float proba)
    : m_action(action), m_proba(proba), m_visit_count(0u),
      m_visit_count_by_random(0u), m_sqrt_visit_count(0.f), m_q_value(0.f),
      m_is_mate(false), m_parent(nullptr), m_sibling(nullptr), m_child(nullptr),
      m_most_visited_child(nullptr)
{
}

template <class P>
void Node<P>::init()
{
    m_action = static_cast<move_t>(0);
    m_proba = 0.f;
    m_visit_count = 0u;
    m_visit_count_by_random = 0u;
    m_sqrt_visit_count = 0.f;
    m_q_value = 0.f;
    m_is_mate = false;
    m_parent = nullptr;
    m_sibling.reset();
    m_child.reset();
    m_most_visited_child = nullptr;
}

template <class P>
float Node<P>::get_q_value(const uint greedy_depth) const
{
    if (m_is_mate || !m_most_visited_child || !greedy_depth)
        return m_q_value;
    return -m_most_visited_child->get_q_value(greedy_depth - 1u);
}

template <class P>
uint Node<P>::get_num_child() const
{
    uint out = 0u;
    for (const Node* c = get_child(); c; c = c->get_sibling())
        ++out;
    return out;
}

template <class P>
const Node<P>* Node<P>::get_child(const move_t& action) const
{
    for (const Node* c = get_child(); c; c = c->get_sibling()) {
        if (c->m_action == action)
            return c;
    }
    return nullptr;
}

template <class P>
Node<P>& Node<P>::apply(const move_t& action)
{
    assert(m_action == 0u);
    for (Node* c = m_child.get(); c; c = c->m_sibling.get()) {
        if (c->m_action == action) {
            // m_action = c->m_action;
            m_visit_count = c->m_visit_count;
            m_visit_count_by_random = c->m_visit_count_by_random;
            m_sqrt_visit_count = c->m_sqrt_visit_count;
            m_q_value = c->m_q_value;
            m_is_mate = c->m_is_mate;
            // m_parent = c->m_parent;
            // m_sibling = c->m_sibling;
            m_most_visited_child = c->m_most_visited_child;
            m_child = std::move(c->m_child);
            return *this;
        }
    }
    m_visit_count = 0u;
    m_visit_count_by_random = 0u;
    m_sqrt_visit_count = 0.f;
    m_q_value = 0.f;
    m_is_mate = false;
    m_most_visited_child = nullptr;
    m_child.reset();
    return *this;
}

template <class P>
Node<P>* Node<P>::select(Game<P>& g, const float& c_puct, const float& p_random)
{
    assert(has_child());
    Node* const c = select_best_or_random_child(c_puct, p_random);
    c->m_parent = this;
    g.apply_nocheck(c->m_action);
    return c;
}

template <class P>
Node<P>*
Node<P>::select_best_or_random_child(const float c_puct, const float p_random)
{
    if (select_best_over_random(p_random))
        return select_best_child(c_puct);
    return select_random_child();
}

template <class P>
bool Node<P>::select_best_over_random(const float p_random)
{
    if (m_is_mate)
        return true;
    constexpr float eps = 1e-3f;
    if (p_random < eps)
        return true;
    const float s = dist01(random_engine);
    return s > p_random;
}

template <class P>
Node<P>* Node<P>::select_best_child(const float c_puct)
{
    const float q_fpu = first_play_urgency();
    Node* out = nullptr;
    float max_puct_score = -100.f;

    for (Node* c = m_child.get(); c; c = c->m_sibling.get()) {
        const float score = puct_score_of(c, c_puct, q_fpu);
        if (score > max_puct_score) {
            max_puct_score = score;
            out = c;
        }
    }
    return out;
}

template <class P>
float Node<P>::first_play_urgency() const
{
    // https://lczero.org/dev/lc0/search/alphazero/#first-play-urgency-fpu
    float q = m_q_value;
    for (const Node* c = m_child.get(); c; c = c->get_sibling()) {
        if (c->m_visit_count)
            q -= c->m_proba;
    }
    return q;
}

template <class P>
float Node<P>::puct_score_of(
    const Node* const c, const float c_puct, const float q_fpu) const
{
    const float q = (m_visit_count) ? -c->m_q_value : q_fpu;
    if (c->is_mate_to_lose()) {
        const float p_plus_1 = c->m_proba + 1.f;
        return q + 2.f + p_plus_1 * m_sqrt_visit_count * c_puct;
    }
    const float u = c->m_proba * m_sqrt_visit_count
                    / static_cast<float>(1 + c->m_visit_count);
    return q + u * c_puct;
}

template <class P>
Node<P>* Node<P>::select_random_child()
{
    constexpr uint num_try_max = 3u;
    const uint num = get_num_child();
    const float p = 1.f / static_cast<float>(num);
    Node* c = nullptr;
    for (uint ii = num_try_max; ii--;) {
        float s = dist01(random_engine);
        for (c = m_child.get(); c; c = c->m_sibling.get()) {
            if (s < p) {
                if (c->is_mate_to_win())
                    break; // Do not select child that leads to LOSS
                return c;
            } else
                s -= p;
        }
    }
    assert(c != nullptr);
    ++(c->m_visit_count_by_random);
    return c;
}

template <class P>
void Node<P>::simulate(const Game<P>& g)
{
    if (m_visit_count)
        return;
    const auto result = g.get_result();
    if (result == ONGOING)
        return;
    const auto turn = g.get_turn();
    if (result == DRAW) {
        m_q_value = (turn == BLACK) ? -0.2f : 0.2f;
    } else {
        const auto winner = (result == BLACK_WIN) ? BLACK : WHITE;
        const auto value = (winner == turn) ? 1.f : -1.f;
        m_q_value = value;
        m_is_mate = true;
    }
}

template <class P>
void Node<P>::expand(
    const std::vector<move_t>& actions,
    const ColorEnum& turn,
    const float* const policy_logits)
{
    const auto num = actions.size();
    if (num == 0)
        return;
    auto probas = std::vector<float>(num);
    for (std::size_t ii = num; ii--;) {
        const auto index = MT::to_policy_index(actions[ii], turn);
        probas[ii] = (policy_logits) ? policy_logits[index] : 0.f;
    }
    softmax(probas);

    m_child = std::make_unique<Node>(actions[0], probas[0]);
    Node* child = m_child.get();
    for (std::size_t ii = 1; ii < num; ++ii) {
        child->m_sibling = std::make_unique<Node>(actions[ii], probas[ii]);
        child = child->m_sibling.get();
    }
}

template <class P>
void Node<P>::simulate_mate_and_expand(const move_t& action)
{
    m_q_value = 1.f;
    m_is_mate = true;
    if (has_child()) {
        Node* c = m_child.get();
        for (; c; c = c->m_sibling.get()) {
            if (c->get_action() == action) {
                m_most_visited_child = c;
                break;
            }
        }
        if (c == nullptr)
            throw std::invalid_argument("Given action not found.");
    } else {
        m_child = std::make_unique<Node>(action, 1.f);
        m_most_visited_child = m_child.get();
    }
    m_most_visited_child->m_q_value = -1.f;
    m_most_visited_child->m_is_mate = true;
}

template <class P>
void Node<P>::simulate_ongoing_and_expand(
    const std::vector<move_t>& actions,
    const ColorEnum& turn,
    const float value,
    const float* const policy_logits)
{
    m_q_value = value;
    expand(actions, turn, policy_logits);
}

template <class P>
Node<P>* Node<P>::backprop(const float v, Node* const child)
{
    const auto count_before = static_cast<float>(m_visit_count++);
    const auto count_after = static_cast<float>(m_visit_count);
    m_sqrt_visit_count = std::sqrt(static_cast<float>(m_visit_count));
    update_most_visited_child(child);
    if (m_is_mate) {
        // preserve `m_q_value` if it is already found to be mate.
    } else if (
        (m_most_visited_child && m_most_visited_child->is_mate_to_lose())
        || all_childs_are_mate_to_win()) {
        m_is_mate = true;
        assert((0.99f < std::abs(v)) && (std::abs(v) < 1.01f));
        m_q_value = v;
    } else if (!m_is_mate) {
        m_q_value = (v + count_before * m_q_value) / count_after;
    }
    return m_parent;
}

template <class P>
void Node<P>::update_most_visited_child(Node* const candidate)
{
    // |   candidate(c)  |                m_most_visited_child(m)             |
    // |                 | nullptr | is_mate_to_win | is_mate_to_lose | other |
    // |         nullptr |    #    |       (m)      |       (m)       |  (m)  |
    // |  is_mate_to_win |   (c)   |        #       |        x        |  (m)  |
    // | is_mate_to_lose |   (c)   |       (c)      |        #        |  (c)  |
    // |           other |   (c)   |       (c)      |        x        |   ?   |
    // #: don't care
    // x: impossible to happen.

    if (m_most_visited_child == nullptr) {
        m_most_visited_child = candidate;
        return;
    }
    if (candidate == nullptr) {
        return;
    }
    assert(
        candidate->is_mate_to_win() ? !m_most_visited_child->is_mate_to_lose()
                                    : true);
    if (candidate->is_mate_to_lose()
        || m_most_visited_child->is_mate_to_win()) {
        m_most_visited_child = candidate;
        return;
    }
    if (candidate->get_visit_count_excluding_random()
        > m_most_visited_child->get_visit_count_excluding_random())
        m_most_visited_child = candidate;
    else if (
        (candidate->get_visit_count_excluding_random()
         == m_most_visited_child->get_visit_count_excluding_random())
        && (candidate->m_q_value < m_most_visited_child->m_q_value))
        m_most_visited_child = candidate;
}

template <class P>
bool Node<P>::all_childs_are_mate_to_win() const
{
    for (const Node* c = get_child(); c; c = c->get_sibling()) {
        if (!c->is_mate_to_win())
            return false;
    }
    return true;
}

} // namespace vshogi::engine::mcts

#endif // VSHOGI_ENGINE_MCTS_NODE_HPP
