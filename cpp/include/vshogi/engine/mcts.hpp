#ifndef VSHOGI_ENGINE_MCTS_HPP
#define VSHOGI_ENGINE_MCTS_HPP

#include <algorithm>
#include <cmath>
#include <memory>
#include <stdexcept>
#include <vector>

#include "vshogi/common/color.hpp"
#include "vshogi/common/game.hpp"
#include "vshogi/common/move.hpp"
#include "vshogi/common/result.hpp"
#include "vshogi/common/utils.hpp"
#include "vshogi/engine/dfpn3/searcher.hpp"

namespace vshogi::engine::mcts
{

namespace dfpn = vshogi::engine::dfpn3;

template <class Parameters>
class Node
{
private:
    using GameType = Game<Parameters>;
    using MoveType = Move<Parameters>;

    /**
     * @brief Pointer to parent node.
     *
     */
    Node* m_parent;

    /**
     * @brief Pointer to sibling node.
     * @ref https://blog.mozilla.org/nnethercote/2012/03/07/n-ary-trees-in-c/
     */
    std::unique_ptr<Node> m_sibling;

    /**
     * @brief Pointer to first child node.
     * @ref https://blog.mozilla.org/nnethercote/2012/03/07/n-ary-trees-in-c/
     */
    std::unique_ptr<Node> m_child;

    /**
     * @brief Action to perform to get to this node from parent node.
     *
     */
    MoveType m_action;

    /**
     * @brief Probability to perform the action at the parent node.
     *
     */
    float m_proba;

    /**
     * @brief Number of visit to this node in the course of explorations.
     * @note `backprop` step increments this value.
     */
    int m_visit_count;

    int m_visit_count_by_random;

    /**
     * @brief result of `std::sqrt(static_cast<float>(m_visit_count))`.
     * @details Computing this value beforehand to reduce the computation time
     * of `u_value_of_puct()` function.
     */
    float m_sqrt_visit_count;

    /**
     * @brief 1 ~ -1 scaled probability of the turn player winning the game.
     * @details This is typically a raw estimate of a machine learning model.
     * If the turn is black, then this value shows winning rate of black.
     * If the turn is white, then it shows the rate of white. If the value is
     * out of [-1, 1] range, then it means that there is a winner.
     */
    float m_value;

    /**
     * @brief Average of `m_value` of all the nodes below this including this
     * one weighted by their `m_visit_count`.
     * @note `m_q_arctanh = atanh(m_q_value)` does not necessarily hold.
     */
    float m_q_value;

    /**
     * @brief True if the node is in mate or leads to mate, otherwise false.
     */
    bool m_is_mate;

    Node* m_most_visited_child;

public:
    /**
     * @brief Select a best or random child node.
     * @note Users must check that the node has at least one child node.
     *
     * @param game Corresponding game object to apply the selected move on.
     * @param coeff_puct Coefficient of PUCT computation.
     * @param random_rate Probability of select a child in random manner.
     * @return Node* A best or random child node.
     */
    Node* select_nocheck(
        Game<Parameters>& game, const float coeff_puct, const float random_rate)
    {
        assert(has_child());
        Node* const ch = select_best_or_random_child(coeff_puct, random_rate);
        ch->m_parent = this;
        game.apply_nocheck(ch->m_action);
        return ch;
    }
    void simulate(const Game<Parameters>& game)
    {
        if (m_visit_count)
            return; // this node should have previous simulation result.
        const auto result = game.get_result();
        if ((result == ONGOING) || (result == DRAW))
            return; // skip, no value changes.

        const auto turn = game.get_turn();
        const auto winner = (result == BLACK_WIN) ? BLACK : WHITE;
        const auto value = (winner == turn) ? 1.f : -1.f;
        m_value = value;
        m_q_value = value;
        m_is_mate = true;
    }
    void expand(
        const std::vector<MoveType>& actions,
        const ColorEnum& turn,
        const float* const policy_logits)
    {
        const auto num = actions.size();
        if (num == 0)
            return;
        auto probas = std::vector<float>(num);
        const auto is_black_turn = (turn == ColorEnum::BLACK);
        for (std::size_t ii = num; ii--;) {
            const auto index
                = (is_black_turn)
                      ? actions[ii].to_dlshogi_policy_index()
                      : actions[ii].rotate().to_dlshogi_policy_index();
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
    void backprop_to_root()
    {
        backprop_leaf();
    }
    void simulate_mate_and_backprop(const Move<Parameters>& a)
    {
        m_value = 1.f;
        m_q_value = 1.f;
        m_is_mate = true;
        m_child = std::make_unique<Node<Parameters>>(a, 1.f);
        m_most_visited_child = m_child.get();
        m_most_visited_child->m_value = -1.f;
        m_most_visited_child->m_q_value = -1.f;
        m_most_visited_child->m_is_mate = true;
        backprop_leaf(); // Increment `m_visit_count`.
    }
    /**
     * @note https://en.wikipedia.org/wiki/Monte_Carlo_tree_search#Principle_of_operation
     *
     * @param actions
     * @param turn
     * @param value
     * @param policy_logits
     */
    void simulate_expand_and_backprop(
        const std::vector<MoveType>& actions,
        const ColorEnum& turn,
        const float value,
        const float* const policy_logits)
    {
        simulate_ongoing_game(value);
        expand(actions, turn, policy_logits);
        backprop_leaf();
    }

private: // select
    Node*
    select_best_or_random_child(const float coeff_puct, const float random_rate)
    {
        if (select_best_over_random(random_rate))
            return select_best_child(coeff_puct);
        return select_random_child();
    }
    bool select_best_over_random(const float random_rate)
    {
        if (m_is_mate || has_mate_to_win())
            return true;
        constexpr float eps = 1e-3f;
        if (random_rate < eps)
            return true;
        const float s = dist01(random_engine);
        return s > random_rate;
    }
    Node* select_best_child(const float coeff_puct)
    {
        Node* ch = m_child.get();
        Node* out = ch;
        float max_puct_score = ch->puct_score_from_parent_view(
            coeff_puct, m_sqrt_visit_count, m_q_value);

        ch = ch->m_sibling.get();
        for (; ch != nullptr;) {
            const float score = ch->puct_score_from_parent_view(
                coeff_puct, m_sqrt_visit_count, m_q_value);
            if (score > max_puct_score) {
                max_puct_score = score;
                out = ch;
            }
            ch = ch->m_sibling.get();
        }
        return out;
    }
    Node* select_random_child()
    {
        constexpr uint num_max_try = 3u;
        const uint num = get_num_child();
        const float p = 1.f / static_cast<float>(num);
        Node* ch = nullptr;
        for (uint ii = num_max_try; ii--;) {
            float s = dist01(random_engine);
            for (ch = m_child.get(); ch != nullptr; ch = ch->m_sibling.get()) {
                if (s < p) {
                    if (!ch->is_mate_to_win())
                        return ch;
                    else
                        break;
                }
                s -= p;
            }
        }
        assert(ch != nullptr);
        ++(ch->m_visit_count_by_random);
        return ch;
    }

public: // utility
    Node()
        : m_parent(nullptr), m_sibling(nullptr), m_child(nullptr), m_action(),
          m_proba(0.f), m_visit_count(0), m_visit_count_by_random(0),
          m_sqrt_visit_count(0.f), m_value(0.f), m_q_value(0.f),
          m_is_mate(false), m_most_visited_child(nullptr)
    {
    }
    Node(const MoveType action, const float proba) noexcept
        : m_parent(nullptr), m_sibling(nullptr), m_child(nullptr),
          m_action(action), m_proba(proba), m_visit_count(0),
          m_visit_count_by_random(0), m_sqrt_visit_count(0.f), m_value(0.f),
          m_q_value(0.f), m_is_mate(false), m_most_visited_child(nullptr)
    {
    }

    // Rules of 5
    ~Node() = default; // 1/5 destructor
    Node(const Node& other) = default; // 2/5 copy constructor
    Node& operator=(const Node& other) = default; // 3/5 copy assignment
    Node(Node&& other) = default; // 4/5 move constructor
    Node& operator=(Node&& other) = default; // 5/5 move assignment

    int get_visit_count() const
    {
        return m_visit_count;
    }
    int get_visit_count_excluding_random() const
    {
        return m_visit_count - m_visit_count_by_random;
    }
    float get_value() const
    {
        return m_value;
    }
    float get_q_value(const uint greedy_depth = 0u) const
    {
        if ((m_most_visited_child == nullptr) || (greedy_depth == 0u))
            return m_q_value;
        else
            return -m_most_visited_child->get_q_value(greedy_depth - 1u);
    }
    float get_proba() const
    {
        return m_proba;
    }
    MoveType get_action() const
    {
        return m_action;
    }
    bool has_child() const
    {
        return static_cast<bool>(m_child);
    }
    uint get_num_child() const
    {
        const Node* ch = m_child.get();
        uint out = 0u;
        while (true) {
            if (ch == nullptr)
                break;
            ++out;
            ch = ch->m_sibling.get();
        }
        return out;
    }
    const Node* get_child(uint index = 0U) const
    {
        const Node* node = m_child.get();
        for (; index--;) {
            if (node->m_sibling == nullptr)
                break;
            node = node->m_sibling.get();
        }
        return node;
    }
    const Node* get_child(const MoveType& action) const
    {
        const Node* ch = m_child.get();
        for (; ch != nullptr; ch = ch->m_sibling.get()) {
            if (ch->m_action == action)
                return ch;
        }
        return nullptr;
    }
    const Node* get_sibling() const
    {
        return m_sibling.get();
    }
    const Node* get_most_visited_child() const
    {
        return m_most_visited_child;
    }
    const Node* get_parent() const
    {
        return m_parent;
    }
    bool is_mate() const
    {
        return m_is_mate;
    }
    bool is_mate_to_win() const
    {
        return m_is_mate && (m_q_value > 0);
    }
    bool is_mate_to_lose() const
    {
        return m_is_mate && (m_q_value < 0);
    }
    Node& apply(const MoveType& action)
    {
        Node* ch = m_child.get();
        for (; ch != nullptr; ch = ch->m_sibling.get()) {
            if (ch->m_action == action) {
                m_action = ch->m_action;
                m_proba = ch->m_proba;
                m_visit_count = ch->m_visit_count;
                m_visit_count_by_random = ch->m_visit_count_by_random;
                m_sqrt_visit_count = ch->m_sqrt_visit_count;
                m_value = ch->m_value;
                m_q_value = ch->m_q_value;
                m_is_mate = ch->m_is_mate;
                m_most_visited_child = ch->m_most_visited_child;
                m_child = std::move(ch->m_child);
                return *this;
            }
        }

        m_child = nullptr;
        m_visit_count = 0;
        m_visit_count_by_random = 0;
        m_sqrt_visit_count = 0.f;
        m_value = 0.f;
        m_q_value = 0.f;
        m_is_mate = false;
        m_most_visited_child = nullptr;
        return *this;
    }

private:
    Node* get_child(std::size_t index)
    {
        Node* node = m_child.get();
        for (; index--;) {
            if (node->m_sibling == nullptr)
                break;
            node = node->m_sibling.get();
        }
        return node;
    }
    bool has_mate_to_win() const
    {
        const Node* ch = m_child.get();
        for (; ch != nullptr; ch = ch->m_sibling.get()) {
            if (ch->is_mate_to_win())
                return true;
        }
        return false;
    }
    bool is_mate_to_lose(const std::size_t& index) const
    {
        const Node* const child = get_child(index);
        return child->m_is_mate && (child->m_q_value > 0);
    }
    float puct_score_from_parent_view(
        const float coeff_puct,
        const float sqrt_visit_count_of_parent,
        const float q_of_parent) const
    {
        const float q = (m_visit_count == 0) ? q_of_parent : -m_q_value;
        if (is_mate_to_lose()) {
            const float p_plus_1 = m_proba + 1.f;
            return (q + 2.f)
                   + p_plus_1 * sqrt_visit_count_of_parent * coeff_puct;
        }
        const float u = m_proba * sqrt_visit_count_of_parent
                        / static_cast<float>(1 + m_visit_count);
        return q + u * coeff_puct;
    }

private:
    void simulate_ongoing_game(const float value)
    {
        m_value = value;
        m_q_value = value;
    }

private:
    void backprop_at_internal_vertex(const float v)
    {
        m_visit_count += 1;
        const auto count_before = static_cast<float>(m_visit_count - 1);
        const auto count_after = static_cast<float>(m_visit_count);
        m_sqrt_visit_count = std::sqrt(count_after);

        m_q_value *= count_before / count_after;
        m_q_value += v / count_after;

        if (m_parent != nullptr) {
            m_parent->update_most_visited_child(this);
            m_parent->backprop_at_internal_vertex(-v);
        }
    }
    void backprop_mate_at_internal_vertex(const float v)
    {
        m_visit_count += 1;
        const auto count_before = static_cast<float>(m_visit_count - 1);
        const auto count_after = static_cast<float>(m_visit_count);
        m_sqrt_visit_count = std::sqrt(count_after);

        if ((!m_is_mate) && (v < 0) && has_non_mate_child()) {
            m_is_mate = false;
            m_q_value *= count_before / count_after;
            m_q_value += v / count_after;
            if (m_parent != nullptr) {
                m_parent->update_most_visited_child(this);
                m_parent->backprop_at_internal_vertex(-v);
            }
        } else {
            m_is_mate = true;
            m_q_value = v;
            if (m_parent != nullptr) {
                m_parent->update_most_visited_child(this);
                m_parent->backprop_mate_at_internal_vertex(-v);
            }
        }
    }
    bool has_non_mate_child() const
    {
        const Node* ch = m_child.get();
        for (; ch != nullptr; ch = ch->m_sibling.get()) {
            if (!ch->m_is_mate)
                return true;
        }
        return false;
    }
    void backprop_leaf()
    {
        m_visit_count += 1;
        // skip updating `m_q_value` because there should be no value change.
        m_sqrt_visit_count = std::sqrt(static_cast<float>(m_visit_count));
        if (m_parent != nullptr) {
            m_parent->update_most_visited_child(this);
            if (m_is_mate)
                m_parent->backprop_mate_at_internal_vertex(-m_q_value);
            else
                m_parent->backprop_at_internal_vertex(-m_q_value);
        }
    }
    void update_most_visited_child(Node* const candidate)
    {
        if (m_most_visited_child == nullptr)
            m_most_visited_child = candidate;
        else if (
            candidate->get_visit_count_excluding_random()
            > m_most_visited_child->get_visit_count_excluding_random())
            m_most_visited_child = candidate;
        else if (
            (candidate->get_visit_count_excluding_random()
             == m_most_visited_child->get_visit_count_excluding_random())
            && (candidate->m_q_value < m_most_visited_child->m_q_value))
            m_most_visited_child = candidate;
    }
};

template <class Parameters>
class Searcher
{
private:
    using GameType = Game<Parameters>;
    using MoveType = Move<Parameters>;

private:
    std::unique_ptr<Node<Parameters>> m_root;
    dfpn::Searcher<Parameters> m_dfpn;
    const float m_coeff_puct;
    const float m_random_rate;
    const int m_random_depth;
    const uint m_dfpn_search_leaf;

public:
    Searcher(
        const float coeff_puct,
        const float random_rate,
        const int random_depth,
        const uint dfpn_search_leaf = 0u)
        : m_root(std::make_unique<Node<Parameters>>()),
          m_dfpn{dfpn_search_leaf * 10u}, m_coeff_puct(coeff_puct),
          m_random_rate(random_rate), m_random_depth(random_depth),
          m_dfpn_search_leaf(dfpn_search_leaf)
    {
    }
    void init_root()
    {
        m_root = std::make_unique<Node<Parameters>>();
    }
    int get_visit_count() const
    {
        return m_root->get_visit_count();
    }
    Node<Parameters>* search(Game<Parameters>& game)
    {
        Node<Parameters>* const leaf = select_a_leaf_node(game);
        assert(leaf != nullptr);
        assert(!leaf->has_child());
        if (game.get_result() != ONGOING) {
            leaf->simulate(game);
            leaf->backprop_to_root();
            return nullptr;
        }
        if (leaf->is_mate_to_lose()) {
            leaf->expand(game.get_legal_moves(), game.get_turn(), nullptr);
            leaf->backprop_to_root();
            return nullptr;
        }
        assert(!leaf->is_mate_to_win());
        if (m_dfpn_search_leaf) {
            m_dfpn.set_game(game);
            m_dfpn.search(m_dfpn_search_leaf);
            if (m_dfpn.proved_mate()) {
                leaf->simulate_mate_and_backprop(m_dfpn.get_mate_move());
                return nullptr;
            }
        }
        return leaf;
    }
    Searcher<Parameters>& apply(const MoveType& action)
    {
        m_root->apply(action);
        return *this;
    }
    const Node<Parameters>* get_root() const
    {
        return m_root.get();
    }
    MoveType get_action_by_visit_max() const
    {
        const Node<Parameters>* const ch = m_root->get_most_visited_child();
        if (ch == nullptr)
            return MoveType();
        else
            return ch->get_action();
    }
    MoveType get_action_by_visit_distribution(const float temperature) const
    {
        constexpr float eps = 1.f;

        std::vector<float> probas(m_root->get_num_child());
        const Node<Parameters>* ch = m_root->get_child();
        for (uint ii = 0u; ch != nullptr; ch = ch->get_sibling()) {
            const auto v
                = static_cast<float>(ch->get_visit_count_excluding_random());
            probas[ii++] = std::log((v + eps)) / temperature;
        }
        softmax(probas);

        float s = dist01(random_engine);
        ch = m_root->get_child();
        for (uint ii = 0u; ch != nullptr; ch = ch->get_sibling()) {
            const auto p = probas[ii++];
            if (s < p)
                return ch->get_action();
            s -= p;
        }
        return ch->get_action(); // For numerical instability.
    }
    MoveType get_action_by_q_distribution(const float temperature) const
    {
        std::vector<float> probas(m_root->get_num_child());
        const Node<Parameters>* ch = m_root->get_child();
        for (uint ii = 0u; ch != nullptr; ch = ch->get_sibling()) {
            probas[ii++] = -ch->get_q_value() / temperature;
        }
        softmax(probas);

        float s = dist01(random_engine);
        ch = m_root->get_child();
        for (uint ii = 0u; ch != nullptr; ch = ch->get_sibling()) {
            const auto p = probas[ii++];
            if (s < p)
                return ch->get_action();
            s -= p;
        }
        return ch->get_action(); // For numerical instability.
    }

private:
    Node<Parameters>* select_a_leaf_node(Game<Parameters>& game)
    {
        Node<Parameters>* n = m_root.get();
        for (int depth = 0; n->has_child(); ++depth) {
            Node<Parameters>* const child = n->select_nocheck(
                game,
                m_coeff_puct,
                (depth < m_random_depth) ? m_random_rate : 0.f);
            assert(child != nullptr);
            assert(child->get_parent() == n);
            n = child;
        }
        return n;
    }
};

} // namespace vshogi::engine::mcts

#endif // VSHOGI_ENGINE_MCTS_HPP
