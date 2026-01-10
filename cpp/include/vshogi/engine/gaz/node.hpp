#ifndef VSHOGI_ENGINE_GAZ_NODE_HPP
#define VSHOGI_ENGINE_GAZ_NODE_HPP

#include <memory>
#include <stdexcept>

#include "vshogi/common/game.hpp"
#include "vshogi/common/move.hpp"
#include "vshogi/common/utils.hpp"
#include "vshogi/engine/tree/node.hpp"

/**
 * @brief Gumbel Alpha Zero
 *
 * https://openreview.net/pdf?id=bERaNdoegnO
 */
namespace vshogi::engine::gaz
{

inline float sigma(
    const float q_hat,
    const uint max_visit_count,
    const float c_visit = 50.f,
    const float c_scale = 1.f)
{
    return (c_visit + static_cast<float>(max_visit_count)) * c_scale * q_hat;
}

/**
 * @note Maximum number of legal moves possible is 593 in standard shogi.
 * https://lfics81.techblog.jp/archives/2041940.html
 */
constexpr uint max_legal_moves = 600u;

class Node : public tree::Node<Node>
{
private:
    float m_logit;
    uint m_visit_count;
    float m_q_value;
    bool m_is_mate;

public:
    Node();
    // clang-format off
    float get_logit() const { return m_logit; }
    uint get_visit_count() const { return m_visit_count; }
    float get_q_value() const { return m_q_value; }
    bool is_mate() const { return m_is_mate; }
    bool is_mate_to_win() const { return m_is_mate && (m_q_value > 0.f); }
    bool is_mate_to_lose() const { return m_is_mate && (m_q_value < 0.f); }
    // clang-format on
    void init();
    void init(Node* const parent, const move_t& action, const float logit);
    float
    get_q_value(const uint greedy_depth, const uint min_visits = 10u) const;
    const Node* get_child_of(const move_t& action) const;

    /**
     * @note Users must check if the node has at least one child before
     * calling this method.
     */
    Node* select();
    Node* select_from(const Node** const child_nodes);
    template <class P>
    void simulate(const Game<P>& g);
    template <class P>
    void
    expand(Node*& next, const Game<P>& g, const float* const policy_logits);
    void simulate_mate_and_expand(Node*& next, const move_t& action);
    template <class P>
    void simulate_ongoing_and_expand(
        Node*& next,
        const Game<P>& g,
        const float value,
        const float* const policy_logits);
    Node* backprop(const float v, Node* const child);

private:
    float compute_v_pi() const;
    // select
    /**
     * @brief softmax(logits + sigma(completedQ))
     * @details eq. 11 in https://openreview.net/pdf?id=bERaNdoegnO
     */
    void improved_policy(float* const out) const;
    float completed_q_value_of(const Node* const child) const;

    template <class G, class P>
    void expand_by_generator(
        Node*& next,
        const State<P>& s,
        const ColorEnum& turn,
        const float* const policy_logits);
    template <class P>
    void make_node_at(
        Node*& next,
        const move_t& action,
        const ColorEnum& turn,
        const float* const policy_logits);

    // backprop
    void update_most_visited_child(Node* const candidate);
    bool all_childs_are_mate_to_win() const;
};

inline Node* Node::select()
{
    assert(has_child());
    float pi_prime[max_legal_moves] = {};
    improved_policy(pi_prime);

    // assert(m_visit_count == (sum(c->get_visit_count()) + 1));
    // eq. 14 in https://openreview.net/pdf?id=bERaNdoegnO
    uint ii = 0u;
    Node* out = m_child;
    float max_diff = pi_prime[ii++]
                     - static_cast<float>(out->get_visit_count())
                           / static_cast<float>(m_visit_count);
    for (Node* c = m_child + 1; c->m_parent == this; ++c) {
        const float d = pi_prime[ii++]
                        - static_cast<float>(c->get_visit_count())
                              / static_cast<float>(m_visit_count);
        if (d > max_diff)
            out = c;
    }
    out->m_parent = this;
    return out;
}

inline Node* Node::select_from(const Node** const child_nodes)
{
    assert(has_child());
    const Node* least_visited = child_nodes[0];
    for (const Node** p = child_nodes; *p; ++p) {
        if ((*p)->get_visit_count() < least_visited->get_visit_count())
            least_visited = *p;
    }
    for (Node* c = m_child; c->m_parent == this; ++c) {
        if (c == least_visited) {
            c->m_parent = this;
            return c;
        }
    }
    assert(false);
    return nullptr;
}

template <class P>
void Node::simulate(const Game<P>& g)
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
void Node::expand(
    Node*& next, const Game<P>& game, const float* const policy_logits)
{
    const auto turn = game.get_turn();
    m_child = next;
    if (game.in_check())
        expand_by_generator<MoveGenerator<P, GenEnum::EVADE>>(
            next, game.get_state(), turn, policy_logits);
    else
        expand_by_generator<MoveGenerator<P, GenEnum::LEGAL>>(
            next, game.get_state(), turn, policy_logits);
    if (m_child == next)
        m_child = nullptr;
    next->init_if_not_end();
}

template <class G, class P>
void Node::expand_by_generator(
    Node*& next,
    const State<P>& s,
    const ColorEnum& turn,
    const float* const policy_logits)
{
    for (auto g = G(s); g; ++g) {
        if (next->is_end())
            break;
        make_node_at<P>(next, *g, turn, policy_logits);
    }
}

template <class P>
void Node::make_node_at(
    Node*& next,
    const move_t& action,
    const ColorEnum& turn,
    const float* const policy_logits)
{
    const auto index = MoveTraits<P>::to_policy_index(action, turn);
    const auto logit = policy_logits ? policy_logits[index] : 0.f;
    next->init(this, action, logit);
    ++next;
}

template <class P>
void Node::simulate_ongoing_and_expand(
    Node*& next,
    const Game<P>& game,
    const float value,
    const float* const policy_logits)
{
    m_q_value = value;
    expand(next, game, policy_logits);
}

} // namespace vshogi::engine::gaz

#endif // VSHOGI_ENGINE_GAZ_NODE_HPP
