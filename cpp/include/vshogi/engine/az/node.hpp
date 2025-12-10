#ifndef VSHOGI_ENGINE_AZ_NODE_HPP
#define VSHOGI_ENGINE_AZ_NODE_HPP

#include <memory>
#include <stdexcept>

#include "vshogi/common/game.hpp"
#include "vshogi/common/move.hpp"
#include "vshogi/common/utils.hpp"
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
    float m_proba;
    uint m_visit_count;
    uint m_visit_count_by_random;
    float m_sqrt_visit_count;
    float m_q_value;
    bool m_is_mate;

public:
    Node();
    Node(const move_t& action, const float proba);
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
    // clang-format on
    float get_q_value(const uint greedy_depth) const;
    uint count_childs() const;
    const Node* get_child_of(const move_t& action) const;
    Node& apply(const move_t& action);

    /**
     * @note Users must check if the node has at least one child before
     * calling this method.
     */
    Node* select(const float& c_puct, const float& p_random);
    template <class P>
    void simulate(const Game<P>& g);
    template <class P>
    void expand(const Game<P>& g, const float* const policy_logits);
    void simulate_mate_and_expand(const move_t& action);
    template <class P>
    void simulate_ongoing_and_expand(
        const Game<P>& g, const float value, const float* const policy_logits);
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

    template <class P>
    static float make_node_at(
        std::unique_ptr<Node>** ptr,
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
void Node::expand(const Game<P>& game, const float* const policy_logits)
{
    const auto turn = game.get_turn();
    std::unique_ptr<Node>* ptr = &m_child;
    float max_logit = -std::numeric_limits<float>::infinity();
    if (game.in_check()) {
        auto gen = MoveGenerator<P, GenEnum::EVADE>(game.get_state());
        for (; gen; ++gen)
            max_logit = std::max(
                max_logit, make_node_at<P>(&ptr, *gen, turn, policy_logits));
    } else {
        auto gen = MoveGenerator<P, GenEnum::LEGAL>(game.get_state());
        for (; gen; ++gen)
            max_logit = std::max(
                max_logit, make_node_at<P>(&ptr, *gen, turn, policy_logits));
    }

    float sumexp = 0.f;
    for (Node* c = m_child.get(); c; c = c->m_sibling.get()) {
        c->m_proba -= max_logit;
        c->m_proba = std::exp(c->m_proba);
        sumexp += c->m_proba;
    }
    for (Node* c = m_child.get(); c; c = c->m_sibling.get()) {
        c->m_proba /= sumexp;
    }
}

template <class P>
float Node::make_node_at(
    std::unique_ptr<Node>** ptr,
    const move_t& action,
    const ColorEnum& turn,
    const float* const policy_logits)
{
    const auto index = MoveTraits<P>::to_policy_index(action, turn);
    const auto logit = policy_logits ? policy_logits[index] : 0.f;
    **ptr = std::make_unique<Node>(action, logit);
    *ptr = &(**ptr)->m_sibling;
    return logit;
}

template <class P>
void Node::simulate_ongoing_and_expand(
    const Game<P>& game, const float value, const float* const policy_logits)
{
    m_q_value = value;
    expand(game, policy_logits);
}

} // namespace vshogi::engine::az

#endif // VSHOGI_ENGINE_AZ_NODE_HPP
