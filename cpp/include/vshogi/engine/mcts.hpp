#ifndef VSHOGI_ENGINE_MCTS_HPP
#define VSHOGI_ENGINE_MCTS_HPP

#include <algorithm>
#include <cmath>
#include <memory>
#include <stdexcept>
#include <vector>

#include "vshogi/color.hpp"
#include "vshogi/result.hpp"

namespace vshogi::engine::mcts
{

static std::size_t integer = 0UL;

template <class Game, class Move>
class Node
{
private:
    using NodeGM = Node<Game, Move>;

    /**
     * @brief Pointer to parent node.
     *
     */
    NodeGM* m_parent;

    /**
     * @brief Action to perform to get to this node from parent node.
     *
     */
    Move m_action;

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

    /**
     * @brief Child nodes.
     */
    std::vector<NodeGM> m_children;

public:
    Node()
        : m_parent(nullptr), m_action(0), m_proba(0.f), m_visit_count(0),
          m_sqrt_visit_count(0.f), m_value(0.f), m_q_value(0.f),
          m_is_mate(false), m_children()
    {
    }
    Node(const Game& game, const float value, const float* const policy_logits)
        : Node()
    {
        simulate_expand_and_backprop(game, value, policy_logits);
    }
    Node(const Move action, const float proba) noexcept
        : m_parent(nullptr), m_action(action), m_proba(proba), m_visit_count(0),
          m_sqrt_visit_count(0.f), m_value(0.f), m_q_value(0.f),
          m_is_mate(false), m_children()
    {
    }

    // Rules of 5
    ~Node() = default; // 1/5 destructor
    Node(const Node<Game, Move>& other) = default; // 2/5 copy constructor
    Node<Game, Move>& operator=(const Node<Game, Move>& other)
        = default; // 3/5 copy assignment
    Node(Node<Game, Move>&& other) = default; // 4/5 move constructor
    Node<Game, Move>& operator=(Node<Game, Move>&& other)
        = default; // 5/5 move assignment

    /**
     * @brief Return true if the node has valid value and action probas.
     *
     * @return true The node has valid value and action probas.
     * @return false The node has invalid value and action probas.
     */
    bool is_valid() const
    {
        return m_visit_count > 0;
    }
    int get_visit_count() const
    {
        return m_visit_count;
    }
    float get_value() const
    {
        return m_value;
    }
    float get_q_value() const
    {
        return m_q_value;
    }
    float get_proba() const
    {
        return m_proba;
    }
    std::vector<Move> get_actions() const noexcept
    {
        std::vector<Move> out = std::vector<Move>();
        out.reserve(m_children.size());
        for (auto& child : m_children) {
            out.emplace_back(child.m_action);
        }
        return out;
    }
    const Node<Game, Move>* get_child(const Move& action) const noexcept
    {
        for (const NodeGM& child : m_children) {
            if (child.m_action == action)
                return &child;
        }
        return nullptr;
    }

    /**
     * @brief Select a leaf node using PUCT algorithm.
     * @note https://en.wikipedia.org/wiki/Monte_Carlo_tree_search#Principle_of_operation
     *
     * @param [in,out] game The game position of the node. After the end, the
     * position corresponds to the leaf node.
     * @param [in] coeff_puct Coefficient of PUCT algorithm. Higher the value is,
     * stronger the exploration is.
     * @param [in] non_random_ratio Ratio of selecting actions in non-random manner.
     * `proba_of_random : proba_of_non_random = 1 : non_random_ratio`.
     * If the value is negative, no random selection.
     * @param [in] random_depth Depth of nodes to explore in random manner.
     * e.g. If `random_depth == 2`, `non_random_ratio` takes effect when
     * selecting child nodes from root node and from nodes beneath the root
     * node. `non_random_ratio` takes no effect for the nodes further below.
     * @return Node<Game, Move> Leaf node selected by PUCT algorithm.
     * If it is game end, then output is null pointer.
     */
    Node<Game, Move>* select(
        Game& game,
        const float coeff_puct,
        const int non_random_ratio,
        int random_depth) noexcept
    {
        NodeGM* node = this;
        while (true) {
            if (node->m_children.empty())
                return node->select_at_leaf(game);
            node = node->select_at_internal_vertex(
                game, coeff_puct, non_random_ratio, random_depth--);
        }
    }

    /**
     * @note https://en.wikipedia.org/wiki/Monte_Carlo_tree_search#Principle_of_operation
     *
     * @param game
     * @param value
     * @param policy_logits
     */
    void simulate_expand_and_backprop(
        const Game& game, const float value, const float* const policy_logits)
    {
        simulate_ongoing_game(value);
        expand(game, policy_logits);
        backprop_leaf();
    }
    Node<Game, Move>& apply(const Move& action)
    {
        if (m_parent != nullptr)
            return *this;

        for (auto&& child : m_children) {
            if (child.m_action == action) {
                m_visit_count = child.m_visit_count;
                m_sqrt_visit_count = child.m_sqrt_visit_count;
                m_value = child.m_value;
                m_q_value = child.m_q_value;
                m_is_mate = child.m_is_mate;
                const std::vector<NodeGM> tmp = std::move(child.m_children);
                m_children = std::move(tmp);
                return *this;
            }
        }

        m_visit_count = 0;
        m_sqrt_visit_count = 0.f;
        m_value = 0.f;
        m_q_value = 0.f;
        m_is_mate = false;
        m_children = std::vector<NodeGM>();
        return *this;
    }

private:
    NodeGM* select_at_leaf(const Game& game)
    {
        if (game.get_result() == ResultEnum::ONGOING)
            return this; // Call `simulate_expand_and_backprop()` later on!
        if (m_visit_count == 0)
            simulate_end_game(game);
        backprop_leaf();
        return nullptr;
    }
    NodeGM* select_at_internal_vertex(
        Game& game,
        const float coeff_puct,
        const int non_random_ratio,
        const int random_depth)
    {
        const auto index
            = select_action_index(coeff_puct, non_random_ratio, random_depth);
        NodeGM* out = &m_children[index];
        out->m_parent = this; // In order to cope with move operations.
        game.apply(out->m_action);
        return out;
    }
    std::size_t select_action_index(
        const float coeff_puct,
        const int non_random_ratio,
        const int random_depth) const
    {
        if (use_random(non_random_ratio, random_depth))
            return select_random();
        return select_max_puct(coeff_puct);
    }
    bool use_random(const int non_random_ratio, const int random_depth) const
    {
        if (random_depth <= 0)
            return false;
        const auto denominator = static_cast<std::size_t>(1 + non_random_ratio);
        if ((integer++ % denominator) != 0)
            return false;
        if (has_mate_to_win())
            return false;
        return true;
    }
    bool has_mate_to_win() const
    {
        for (std::size_t ii = m_children.size(); ii--;) {
            if (is_mate_to_win(ii))
                return true;
        }
        return false;
    }
    bool is_mate_to_win(const std::size_t& index) const
    {
        const auto& child = m_children[index];
        return child.m_is_mate && (child.m_q_value < 0);
    }
    bool is_mate_to_lose(const std::size_t& index) const
    {
        const auto& child = m_children[index];
        return child.m_is_mate && (child.m_q_value > 0);
    }
    std::size_t select_random() const
    {
        constexpr std::size_t num_max_try = 3;
        const auto num = m_children.size();
        if (integer + num < num)
            integer %= num;
        const auto end = integer + std::min(num, num_max_try);
        auto index = integer % num;
        for (; integer < end; ++integer) {
            if (!is_mate_to_lose(index))
                return index;
            index++;
            index %= num;
        }
        return index;
    }
    std::size_t select_max_puct(const float coeff_puct) const
    {
        auto index = m_children.size() - 1UL;
        auto max_puct_score = puct_score(index, coeff_puct);
        for (auto ii = index; ii--;) {
            const auto score = puct_score(ii, coeff_puct);
            if (score > max_puct_score) {
                max_puct_score = score;
                index = ii;
            }
        }
        return index;
    }
    float
    puct_score(const std::size_t index, const float coeff_puct) const noexcept
    {
        const auto q = puct_q(index);
        if (is_mate_to_win(index)) {
            const auto p_plus_1 = m_children[index].m_proba + 1.f;
            return (q + 2.f) + p_plus_1 * m_sqrt_visit_count * coeff_puct;
        }
        return q + puct_u(index) * coeff_puct;
    }
    float puct_q(const std::size_t index) const noexcept
    {
        return -m_children[index].m_q_value;
    }
    float puct_u(const std::size_t index) const noexcept
    {
        const NodeGM& child = m_children[index];
        const auto p = child.m_proba;
        const auto count = child.m_visit_count;
        return p * m_sqrt_visit_count / static_cast<float>(1 + count);
    }

private:
    void simulate_ongoing_game(const float value)
    {
        m_value = value;
        m_q_value = value;
    }
    void simulate_end_game(const Game& game)
    {
        const auto result = game.get_result();
        if (result == ResultEnum::DRAW)
            return; // Skip simulation as no change from initial values.

        const auto turn = game.get_turn();
        const auto winner = (result == BLACK_WIN) ? BLACK : WHITE;
        const auto value = (winner == turn) ? 1.f : -1.f;
        m_value = value;
        m_q_value = value;
        m_is_mate = true;
    }

private:
    void expand(const Game& game, const float* const policy_logits)
    {
        const std::vector<Move>& moves = game.get_legal_moves();
        const auto num = moves.size();
        auto probas = std::vector<float>(num);
        const auto is_black_turn = (game.get_turn() == ColorEnum::BLACK);
        for (std::size_t ii = num; ii--;) {
            const auto index
                = (is_black_turn)
                      ? moves[ii].to_dlshogi_policy_index()
                      : moves[ii].rotate().to_dlshogi_policy_index();
            probas[ii] = policy_logits[index];
        }
        softmax(probas);

        m_children.reserve(num);
        for (std::size_t ii = 0; ii < num; ++ii)
            m_children.emplace_back(moves[ii], probas[ii]);
    }

private:
    void backprop_at_internal_vertex(const float v)
    {
        const auto count_before = static_cast<float>(m_visit_count);
        m_visit_count += 1;
        const auto count_after = static_cast<float>(m_visit_count);
        m_sqrt_visit_count = std::sqrt(count_after);

        m_q_value *= count_before / count_after;
        m_q_value += v / count_after;

        if (m_parent != nullptr)
            m_parent->backprop_at_internal_vertex(-v);
    }
    void backprop_mate_at_internal_vertex(const float v)
    {
        const auto count_before = static_cast<float>(m_visit_count);
        m_visit_count += 1;
        const auto count_after = static_cast<float>(m_visit_count);
        m_sqrt_visit_count = std::sqrt(count_after);

        if ((!m_is_mate) && (v < 0) && has_non_mate_child()) {
            m_is_mate = false;
            m_q_value *= count_before / count_after;
            m_q_value += v / count_after;
            if (m_parent != nullptr)
                m_parent->backprop_at_internal_vertex(-v);
        } else {
            m_is_mate = true;
            m_q_value = v;
            if (m_parent != nullptr)
                m_parent->backprop_mate_at_internal_vertex(-v);
        }
    }
    bool has_non_mate_child() const
    {
        for (auto&& child : m_children) {
            if (!child.m_is_mate)
                return true;
        }
        return false;
    }
    void backprop_leaf()
    {
        // skip updating `m_q_value` because there should be no value change.
        m_visit_count += 1;
        m_sqrt_visit_count = std::sqrt(static_cast<float>(m_visit_count));
        if (m_parent != nullptr) {
            if (m_is_mate)
                m_parent->backprop_mate_at_internal_vertex(-m_q_value);
            else
                m_parent->backprop_at_internal_vertex(-m_q_value);
        }
    }

private:
    static void softmax(std::vector<float>& v)
    {
        if (v.empty())
            return;
        const float maximum_value = *std::max_element(v.cbegin(), v.cend());
        float sum = 0.f;
        for (auto&& e : v) {
            e -= maximum_value;
            e = std::exp(e);
            sum += e;
        }
        for (auto&& e : v) {
            e /= sum;
        }
    }
};

} // namespace vshogi::engine::mcts

#endif // VSHOGI_ENGINE_MCTS_HPP
