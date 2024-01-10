#ifndef VSHOGI_ENGINE_MCTS_HPP
#define VSHOGI_ENGINE_MCTS_HPP

#include <algorithm>
#include <cmath>
#include <memory>
#include <random>
#include <stdexcept>
#include <vector>

#include "vshogi/color.hpp"
#include "vshogi/engine/dfpn.hpp"
#include "vshogi/result.hpp"

namespace vshogi::engine::mcts
{

static std::random_device seed_gen;
static std::default_random_engine engine(seed_gen());
static std::uniform_real_distribution<float> dist(0.f, 1.f);

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
     * @brief Pointer to sibling node.
     * @ref https://blog.mozilla.org/nnethercote/2012/03/07/n-ary-trees-in-c/
     */
    std::unique_ptr<NodeGM> m_sibling;

    /**
     * @brief Pointer to first child node.
     * @ref https://blog.mozilla.org/nnethercote/2012/03/07/n-ary-trees-in-c/
     */
    std::unique_ptr<NodeGM> m_child;

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

public:
    Node()
        : m_parent(nullptr), m_sibling(nullptr), m_child(nullptr), m_action(),
          m_proba(0.f), m_visit_count(0), m_sqrt_visit_count(0.f), m_value(0.f),
          m_q_value(0.f), m_is_mate(false)
    {
    }
    Node(const Game& game, const float value, const float* const policy_logits)
        : Node()
    {
        simulate_expand_and_backprop(game, value, policy_logits);
    }
    Node(const Move action, const float proba) noexcept
        : m_parent(nullptr), m_sibling(nullptr), m_child(nullptr),
          m_action(action), m_proba(proba), m_visit_count(0),
          m_sqrt_visit_count(0.f), m_value(0.f), m_q_value(0.f),
          m_is_mate(false)
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
    Move get_action() const
    {
        return m_action;
    }
    std::size_t get_num_child() const
    {
        const NodeGM* ch = m_child.get();
        std::size_t out = 0UL;
        while (true) {
            if (ch == nullptr)
                break;
            ++out;
            ch = ch->m_sibling.get();
        }
        return out;
    }
    const Node<Game, Move>* get_child(std::size_t index = 0UL) const
    {
        const Node<Game, Move>* node = m_child.get();
        for (; index--;) {
            if (node->m_sibling == nullptr)
                break;
            node = node->m_sibling.get();
        }
        return node;
    }
    const Node<Game, Move>* get_child(const Move& action) const
    {
        const NodeGM* ch = m_child.get();
        for (; ch != nullptr; ch = ch->m_sibling.get()) {
            if (ch->m_action == action)
                return ch;
        }
        return nullptr;
    }
    const Node<Game, Move>* get_sibling() const
    {
        return m_sibling.get();
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
     * @param [in] num_dfpn_nodes Number of nodes to explore at leaf node using
     * DFPN algorithm.
     * @return Node<Game, Move> Leaf node selected by PUCT algorithm.
     * If it is game end, then output is null pointer.
     */
    Node<Game, Move>* select(
        Game& game,
        const float coeff_puct,
        const int non_random_ratio,
        int random_depth,
        const std::size_t num_dfpn_nodes = 0)
    {
        NodeGM* node = this;
        while (true) {
            if (node->m_child == nullptr)
                return node->select_at_leaf(game, num_dfpn_nodes);
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
        NodeGM* ch = m_child.get();
        for (; ch != nullptr; ch = ch->m_sibling.get()) {
            if (ch->m_action == action) {
                m_action = ch->m_action;
                m_proba = ch->m_proba;
                m_visit_count = ch->m_visit_count;
                m_sqrt_visit_count = ch->m_sqrt_visit_count;
                m_value = ch->m_value;
                m_q_value = ch->m_q_value;
                m_is_mate = ch->m_is_mate;
                m_child = std::move(ch->m_child);
                return *this;
            }
        }

        m_child = nullptr;
        m_visit_count = 0;
        m_sqrt_visit_count = 0.f;
        m_value = 0.f;
        m_q_value = 0.f;
        m_is_mate = false;
        return *this;
    }

    Move get_best_action() const
    {
        Move out = Move();
        int max_visit_count = -1;

        const NodeGM* ch = m_child.get();
        for (; ch != nullptr; ch = ch->m_sibling.get()) {
            if (ch->m_visit_count > max_visit_count) {
                out = ch->m_action;
                max_visit_count = ch->m_visit_count;
            }
        }
        return out;
    }
    Move get_best_move(const float temperature) const
    {
        constexpr float eps = 1e-3f;

        std::vector<float> probas(get_num_child());
        const NodeGM* ch = m_child.get();
        for (std::size_t ii = 0; ch != nullptr; ch = ch->m_sibling.get()) {
            const auto v = static_cast<float>(ch->m_visit_count);
            probas[ii++] = std::log((v + eps) / temperature);
        }
        softmax(probas);

        float s = dist(engine);
        Move out = Move(0);
        ch = m_child.get();
        for (std::size_t ii = 0; ch != nullptr; ch = ch->m_sibling.get()) {
            const auto p = probas[ii++];
            if (s < p)
                return ch->m_action;
            s -= p;
            out = ch->m_action; // For numerical instability.
        }
        return out;
    }

private:
    NodeGM* select_at_leaf(const Game& game, const std::size_t num_dfpn_nodes)
    {
        if (game.get_result() == ResultEnum::ONGOING) {
            if constexpr (!std::is_same<Game, animal_shogi::Game>::value) {
                if ((num_dfpn_nodes > 0)
                    && (!std::is_same<Game, animal_shogi::Game>::value)) {
                    auto searcher
                        = dfpn::Searcher<Game, Move>(game, num_dfpn_nodes);
                    if (searcher.explore()) {
                        m_value = 1.f;
                        m_q_value = 1.f;
                        m_is_mate = true;
                        backprop_leaf(); // Increment `m_visit_count`.
                        return nullptr;
                    }
                }
            }
            return this;
        }

        if (m_visit_count == 0)
            simulate_end_game(game);
        backprop_leaf();
        return nullptr;
    }
    NodeGM* get_child(std::size_t index)
    {
        Node<Game, Move>* node = m_child.get();
        for (; index--;) {
            if (node->m_sibling == nullptr)
                break;
            node = node->m_sibling.get();
        }
        return node;
    }
    NodeGM* select_at_internal_vertex(
        Game& game,
        const float coeff_puct,
        const int non_random_ratio,
        const int random_depth)
    {
        NodeGM* ch = select_child(coeff_puct, non_random_ratio, random_depth);
        ch->m_parent = this; // In order to cope with move operations.
        game.apply(ch->m_action);
        return ch;
    }
    NodeGM* select_child(
        const float coeff_puct,
        const int non_random_ratio,
        const int random_depth)
    {
        if (use_random(non_random_ratio, random_depth))
            return select_random();
        return select_max_puct(coeff_puct);
    }
    bool use_random(const int non_random_ratio, const int random_depth) const
    {
        if (random_depth <= 0)
            return false;
        const float u = dist(engine);
        const float p_random = 1.f / static_cast<float>(1 + non_random_ratio);
        if (u > p_random)
            return false;
        if (has_mate_to_win())
            return false;
        return true;
    }
    bool has_mate_to_win() const
    {
        const NodeGM* ch = m_child.get();
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
        const NodeGM* const child = get_child(index);
        return child->m_is_mate && (child->m_q_value > 0);
    }
    NodeGM* select_random()
    {
        constexpr std::size_t num_max_try = 3;
        const std::size_t num = get_num_child();
        const float p = 1.f / static_cast<float>(num);
        Node* ch = nullptr;
        for (std::size_t ii = num_max_try; ii--;) {
            float s = dist(engine);
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
    NodeGM* select_max_puct(const float coeff_puct) const
    {
        NodeGM* ch = m_child.get();
        NodeGM* out = ch;
        float max_puct_score
            = ch->puct_score_from_parent_view(coeff_puct, m_sqrt_visit_count);

        ch = ch->m_sibling.get();
        for (; ch != nullptr;) {
            const float score = ch->puct_score_from_parent_view(
                coeff_puct, m_sqrt_visit_count);
            if (score > max_puct_score) {
                max_puct_score = score;
                out = ch;
            }
            ch = ch->m_sibling.get();
        }
        return out;
    }
    float puct_score_from_parent_view(
        const float coeff_puct, const float sqrt_visit_count_of_parent) const
    {
        const float q = -m_q_value;
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
        if (num == 0)
            return;
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

        m_child = std::make_unique<NodeGM>(moves[0], probas[0]);
        NodeGM* child = m_child.get();
        for (std::size_t ii = 1; ii < num; ++ii) {
            child->m_sibling = std::make_unique<NodeGM>(moves[ii], probas[ii]);
            child = child->m_sibling.get();
        }
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
        const NodeGM* ch = m_child.get();
        for (; ch != nullptr; ch = ch->m_sibling.get()) {
            if (!ch->m_is_mate)
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
