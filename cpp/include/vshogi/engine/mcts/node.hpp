#ifndef VSHOGI_ENGINE_MCTS_NODE_HPP
#define VSHOGI_ENGINE_MCTS_NODE_HPP

#include <memory>
#include <stdexcept>

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

class Node
{
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
    Node(const Node& other) = delete; // Rule 2/5 copy constructor
    Node& operator=(const Node& other) = delete; // Rule 3/5 copy assignment
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
    const auto actions = game.get_legal_moves();
    const auto turn = game.get_turn();
    const auto num = actions.size();
    if (num == 0)
        return;
    auto probas = std::vector<float>(num);
    for (std::size_t ii = num; ii--;) {
        const auto index = MoveTraits<P>::to_policy_index(actions[ii], turn);
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
void Node::simulate_ongoing_and_expand(
    const Game<P>& game, const float value, const float* const policy_logits)
{
    m_q_value = value;
    expand(game, policy_logits);
}

} // namespace vshogi::engine::mcts

#endif // VSHOGI_ENGINE_MCTS_NODE_HPP
