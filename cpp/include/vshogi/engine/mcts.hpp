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
#include "vshogi/engine/dfpn.hpp"

namespace vshogi::engine::mcts
{

template <class Config>
class Node
{
private:
    using GameType = Game<Config>;
    using MoveType = Move<Config>;

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

    int m_visit_count_excluding_random;

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
    Node()
        : m_parent(nullptr), m_sibling(nullptr), m_child(nullptr), m_action(),
          m_proba(0.f), m_visit_count(0), m_visit_count_excluding_random(0),
          m_sqrt_visit_count(0.f), m_value(0.f), m_q_value(0.f),
          m_is_mate(false), m_most_visited_child(nullptr)
    {
    }
    Node(
        const std::vector<MoveType>& actions,
        const ColorEnum& turn,
        const float value,
        const float* const policy_logits)
        : Node()
    {
        m_visit_count = 1;
        m_visit_count_excluding_random = 1;
        simulate_expand_and_backprop(actions, turn, value, policy_logits);
    }
    Node(const MoveType action, const float proba) noexcept
        : m_parent(nullptr), m_sibling(nullptr), m_child(nullptr),
          m_action(action), m_proba(proba), m_visit_count(0),
          m_visit_count_excluding_random(0), m_sqrt_visit_count(0.f),
          m_value(0.f), m_q_value(0.f), m_is_mate(false),
          m_most_visited_child(nullptr)
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
        return m_visit_count_excluding_random;
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
    void increment_visit_counts()
    {
        ++m_visit_count;
        ++m_visit_count_excluding_random;
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
     * @return Node Leaf node selected by PUCT algorithm.
     * If it is game end, then output is null pointer.
     */
    Node* select(
        GameType& game,
        const float coeff_puct,
        const int non_random_ratio,
        int random_depth)
    {
        increment_visit_counts();
        Node* node = this;
        while (node->has_child()) {
            node = node->select_at_internal_vertex(
                game, coeff_puct, non_random_ratio, random_depth--);
        }
        return node->select_at_leaf(game);
    }
    Node* select_at_internal_vertex(
        GameType& game,
        const float coeff_puct,
        const int non_random_ratio,
        const int random_depth)
    {
        Node* ch = select_child(coeff_puct, non_random_ratio, random_depth);
        ch->m_parent = this; // In order to cope with move operations.
        game.apply_nocheck(ch->m_action);
        return ch;
    }
    Node* select_at_leaf(const GameType& game)
    {
        if (game.get_result() == ResultEnum::ONGOING) {
            return this;
        }
        if (m_visit_count == 1)
            simulate_end_game(game);
        backprop_leaf();
        return nullptr;
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
    void simulate_mate_and_backprop()
    {
        m_value = 1.f;
        m_q_value = 1.f;
        m_is_mate = true;
        backprop_leaf(); // Increment `m_visit_count`.
    }
    Node& apply(const MoveType& action)
    {
        Node* ch = m_child.get();
        for (; ch != nullptr; ch = ch->m_sibling.get()) {
            if (ch->m_action == action) {
                m_action = ch->m_action;
                m_proba = ch->m_proba;
                m_visit_count = ch->m_visit_count;
                m_visit_count_excluding_random
                    = ch->m_visit_count_excluding_random;
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
        m_visit_count_excluding_random = 0;
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
    Node* select_child(
        const float coeff_puct,
        const int non_random_ratio,
        const int random_depth)
    {
        Node* ch = nullptr;
        if (use_random(non_random_ratio, random_depth)) {
            ch = select_random();
        } else {
            ch = select_max_puct(coeff_puct);
            ++(ch->m_visit_count_excluding_random);
        }
        ++(ch->m_visit_count);
        return ch;
    }
    bool use_random(const int non_random_ratio, const int random_depth) const
    {
        if (random_depth <= 0)
            return false;
        const float u = dist01(random_engine);
        const float p_random = 1.f / static_cast<float>(1 + non_random_ratio);
        if (u > p_random)
            return false;
        if (has_mate_to_win())
            return false;
        return true;
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
    bool is_mate_to_win() const
    {
        return m_is_mate && (m_q_value > 0);
    }
    bool is_mate_to_lose() const
    {
        return m_is_mate && (m_q_value < 0);
    }
    bool is_mate_to_lose(const std::size_t& index) const
    {
        const Node* const child = get_child(index);
        return child->m_is_mate && (child->m_q_value > 0);
    }
    Node* select_random()
    {
        constexpr std::size_t num_max_try = 3;
        const std::size_t num = get_num_child();
        const float p = 1.f / static_cast<float>(num);
        Node* ch = nullptr;
        for (std::size_t ii = num_max_try; ii--;) {
            float s = dist01(random_engine);
            for (ch = m_child.get(); ch != nullptr; ch = ch->m_sibling.get()) {
                if (p > s) {
                    if (!ch->is_mate_to_win())
                        return ch;
                    else
                        break;
                }
                s -= p;
            }
        }
        return ch;
    }
    Node* select_max_puct(const float coeff_puct) const
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
    void simulate_end_game(const GameType& game)
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

private:
    void backprop_at_internal_vertex(const float v)
    {
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
            candidate->m_visit_count_excluding_random
            > m_most_visited_child->m_visit_count_excluding_random)
            m_most_visited_child = candidate;
        else if (
            (candidate->m_visit_count_excluding_random
             == m_most_visited_child->m_visit_count_excluding_random)
            && (candidate->m_q_value < m_most_visited_child->m_q_value))
            m_most_visited_child = candidate;
    }
};

template <class Config>
class Searcher
{
private:
    using GameType = Game<Config>;
    using MoveType = Move<Config>;

private:
    std::unique_ptr<Node<Config>> m_root;
    const float m_coeff_puct;
    const int m_non_random_ratio;
    const int m_random_depth;

public:
    Searcher(
        const float coeff_puct,
        const int non_random_ratio,
        const int random_depth)
        : m_root{}, m_coeff_puct(coeff_puct),
          m_non_random_ratio(non_random_ratio), m_random_depth(random_depth)
    {
    }
    void set_game(const GameType& g, const float v, const float* const p_logits)
    {
        m_root = std::make_unique<Node<Config>>(
            g.get_legal_moves(), g.get_turn(), v, p_logits);
    }
    int get_visit_count() const
    {
        return m_root->get_visit_count();
    }
    Node<Config>* select(GameType& game)
    {
        Node<Config>* node = m_root.get();
        int random_depth = m_random_depth;
        node->increment_visit_counts();
        while (node->has_child()) {
            node = node->select_at_internal_vertex(
                game, m_coeff_puct, m_non_random_ratio, random_depth--);
        }
        return node->select_at_leaf(game);
    }
    Searcher<Config>& apply(const MoveType& action)
    {
        m_root->apply(action);
        return *this;
    }
    const Node<Config>* get_root() const
    {
        return m_root.get();
    }
    MoveType get_action_by_visit_max() const
    {
        const Node<Config>* const ch = m_root->get_most_visited_child();
        if (ch == nullptr)
            return MoveType();
        else
            return ch->get_action();
    }
    MoveType get_action_by_visit_distribution(const float temperature) const
    {
        constexpr float eps = 1.f;

        std::vector<float> probas(m_root->get_num_child());
        const Node<Config>* ch = m_root->get_child();
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
    static float
    evaluate_by_random_playout(const GameType& g, const uint num_random_playout)
    {
        float v = 0.f;
        const auto turn = g.get_turn();
        for (uint ii = num_random_playout; ii--;)
            v += result_to_value(single_random_playout(g), turn);
        return v / static_cast<float>(num_random_playout);
    }

private:
    static ResultEnum single_random_playout(const GameType& g)
    {
        GameType g_copy = GameType(g);
        while (g_copy.get_result() == ONGOING) {
            const auto m = random_select_legal_action(g_copy);
            g_copy.apply_nocheck(m);
        }
        return g_copy.get_result();
    }
    static MoveType random_select_legal_action(const GameType& g)
    {
        const State<Config>& state = g.get_state();
        const auto turn = state.get_turn();
        auto iter_drop = DropMoveGenerator<Config>(state);
        auto iter_king = KingMoveGenerator<Config>(state);
        auto iter_board = NonKingBoardMoveGenerator<Config>(state);
        const auto num_droppable
            = iter_drop.is_end() ? 0u : state.get_stand(turn).unique_count();
        const auto num_non_king
            = iter_board.is_end()
                  ? 0u
                  : state.get_board().get_occupied(turn).hamming_weight() - 1u;
        const auto num_king = iter_king.is_end() ? 0u : 1u;
        const float num_source
            = static_cast<float>(num_droppable + num_non_king + num_king);
        const auto fraction_drop
            = static_cast<float>(num_droppable) / num_source;
        const auto fraction_non_king
            = static_cast<float>(num_non_king) / num_source;
        float s_iter = dist01(random_engine);
        if (s_iter < fraction_drop)
            return random_select_one_action(iter_drop);
        s_iter -= fraction_drop;
        if (s_iter < fraction_non_king)
            return random_select_one_action(iter_board);
        return random_select_one_action(iter_king);
    }
    template <class Iterable>
    static MoveType random_select_one_action(Iterable& iter)
    {
        MoveType out = *iter;
        ++iter;
        for (uint ii = 2u; !iter.is_end(); ++ii, ++iter) {
            const auto r = dist01(random_engine);
            if (static_cast<uint>(r * static_cast<float>(ii)) == 0u)
                out = *iter;
        }
        return out;
    }
    static float result_to_value(const ResultEnum r, const ColorEnum turn)
    {
        if (r == DRAW)
            return 0.f;
        if ((static_cast<int>(r) - static_cast<int>(BLACK_WIN))
            == static_cast<int>(turn))
            return 1.f;
        return -1.f;
    }
};

} // namespace vshogi::engine::mcts

#endif // VSHOGI_ENGINE_MCTS_HPP
