#ifndef VSHOGI_ENGINE_AZ_NODE_HPP
#define VSHOGI_ENGINE_AZ_NODE_HPP

#include <memory>
#include <stdexcept>

#include "vshogi/common/game.hpp"
#include "vshogi/common/move.hpp"
#include "vshogi/common/utils.hpp"
#include "vshogi/engine/contiguous_buffer.hpp"
#include "vshogi/engine/tree/node.hpp"

/**
 * @brief Alpha Zero
 *
 */
namespace vshogi::engine::az
{

class Node : public tree::Node<Node>
{
private:
    move_t m_action;
    bool m_is_mate;
    float m_proba;
    uint m_visit_count;
    uint m_visit_count_by_random;
    float m_sqrt_visit_count;
    float m_q_value;

public:
    Node();
    // clang-format off
    move_t get_action() const { return m_action; }
    float get_proba() const { return m_proba; }
    uint get_visit_count() const { return m_visit_count; }
    uint get_visit_count_excluding_random() const { return m_visit_count - m_visit_count_by_random; }
    float get_q_value() const { return m_q_value; }
    bool is_mate() const { return m_is_mate; }
    bool is_mate_to_win() const { return m_is_mate && (m_q_value > 0.f); }
    bool is_mate_to_lose() const { return m_is_mate && (m_q_value < 0.f); }
    // clang-format on
    void init();
    void init(Node* const parent, const move_t& action, const float proba);
    void init_as_begin();
    float
    get_q_value(const uint greedy_depth, const uint min_visits = 10u) const;
    const Node* get_child_of(const move_t& action) const;

    /**
     * @note Users must check if the node has at least one child before
     * calling this method.
     */
    Node* select(const float& c_puct, const float& p_random);
    template <class P>
    void simulate(const Game<P>& g);
    template <class P>
    void expand(
        ContiguousBuffer<Node>& buffer,
        const Game<P>& g,
        const float* const policy_logits);
    void simulate_mate_and_expand(
        ContiguousBuffer<Node>& buffer, const move_t& action);
    template <class P>
    void simulate_ongoing_and_expand(
        ContiguousBuffer<Node>& buffer,
        const Game<P>& g,
        const float value,
        const float* const policy_logits);
    Node* backprop(const float v, Node* const child);

private:
    const Node* most_visited_child() const;
    float compute_v_pi(const uint min_visits) const;
    // select
    Node* select_best_or_random_child(const float c_puct, const float p_random);
    bool select_best_over_random(const float p_random);
    bool has_mate_to_win() const;
    Node* select_best_child(const float c_puct);
    Node* select_random_child();
    float first_play_urgency() const;
    float puct_score_of(
        const Node* const c, const float c_puct, const float q_fpu) const;

    // expand
    template <class P, GenEnum GenType>
    void expand_by_generator(
        ContiguousBuffer<Node>& buffer,
        const Game<P>& game,
        const float* const policy_logits);
    template <class P>
    float init_node_in(
        ContiguousBuffer<Node>& buffer,
        const move_t& action,
        const ColorEnum& turn,
        const float* const policy_logits);

    // backprop
    void update_most_visited_child(Node* const candidate);
    bool all_childs_are_mate_to_win() const;
};

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
    ContiguousBuffer<Node>& buffer,
    const Game<P>& game,
    const float* const policy_logits)
{
    if (game.in_check())
        expand_by_generator<P, GenEnum::EVADE>(buffer, game, policy_logits);
    else
        expand_by_generator<P, GenEnum::LEGAL>(buffer, game, policy_logits);
}

template <class P, GenEnum GenType>
void Node::expand_by_generator(
    ContiguousBuffer<Node>& buffer,
    const Game<P>& game,
    const float* const policy_logits)
{
    m_child = buffer.next();
    const auto turn = game.get_turn();
    float max_logit = -std::numeric_limits<float>::infinity();
    auto gen = MoveGenerator<P, GenType>(game.get_state());
    for (; gen; ++gen) {
        if (buffer.is_full())
            break;
        max_logit = std::max(
            max_logit, init_node_in<P>(buffer, *gen, turn, policy_logits));
    }
    if (m_child == buffer.next()) { // no child expanded
        m_child = nullptr;
        return;
    }
    if (not buffer.is_full())
        buffer.next()->init();
    float sumexp = 0.f;
    for (Node* c = m_child; c->m_parent == this; ++c) {
        c->m_proba -= max_logit;
        c->m_proba = std::exp(c->m_proba);
        sumexp += c->m_proba;
    }
    for (Node* c = m_child; c->m_parent == this; ++c) {
        c->m_proba /= sumexp;
    }
}

template <class P>
float Node::init_node_in(
    ContiguousBuffer<Node>& buffer,
    const move_t& action,
    const ColorEnum& turn,
    const float* const policy_logits)
{
    const auto index = MoveTraits<P>::to_policy_index(action, turn);
    const auto logit = policy_logits ? policy_logits[index] : 0.f;
    buffer.emplace_next(this, action, logit);
    return logit;
}

template <class P>
void Node::simulate_ongoing_and_expand(
    ContiguousBuffer<Node>& buffer,
    const Game<P>& game,
    const float value,
    const float* const policy_logits)
{
    m_q_value = value;
    expand(buffer, game, policy_logits);
}

} // namespace vshogi::engine::az

#endif // VSHOGI_ENGINE_AZ_NODE_HPP
