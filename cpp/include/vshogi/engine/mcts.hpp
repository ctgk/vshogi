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
#include "vshogi/engine/dfpn/searcher.hpp"

namespace vshogi::engine::mcts
{

namespace dfpn = vshogi::engine::dfpn;

template <class P>
class Node
{
private:
    using MT = MoveTraits<P>;

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
    move_t m_action;

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
     * @brief Average of raw values of all the nodes below this including this
     * one weighted by their `m_visit_count`.
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
        Game<P>& game, const float coeff_puct, const float random_rate)
    {
        assert(has_child());
        Node* const ch = select_best_or_random_child(coeff_puct, random_rate);
        ch->m_parent = this;
        game.apply_nocheck(ch->m_action);
        return ch;
    }
    void simulate(const Game<P>& game)
    {
        if (m_visit_count)
            return; // this node should have previous simulation result.
        const auto result = game.get_result();
        if (result == ONGOING)
            return;

        const auto turn = game.get_turn();
        if (result == DRAW) {
            m_q_value = (turn == BLACK) ? -0.2f : 0.2f;
            return;
        }

        const auto winner = (result == BLACK_WIN) ? BLACK : WHITE;
        const auto value = (winner == turn) ? 1.f : -1.f;
        m_q_value = value;
        m_is_mate = true;
    }
    void expand(
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
    void simulate_mate_and_expand(const move_t& a)
    {
        m_q_value = 1.f;
        m_is_mate = true;
        if (has_child()) {
            Node* c = m_child.get();
            for (; c; c = c->m_sibling.get()) {
                if (c->get_action() == a) {
                    m_most_visited_child = c;
                    break;
                }
            }
            if (c == nullptr)
                throw std::invalid_argument("Given action not found.");
        } else {
            m_child = std::make_unique<Node<P>>(a, 1.f);
            m_most_visited_child = m_child.get();
        }
        m_most_visited_child->m_q_value = -1.f;
        m_most_visited_child->m_is_mate = true;
    }
    /**
     * @note https://en.wikipedia.org/wiki/Monte_Carlo_tree_search#Principle_of_operation
     *
     * @param actions
     * @param turn
     * @param value
     * @param policy_logits
     */
    void simulate_ongoing_and_expand(
        const std::vector<move_t>& actions,
        const ColorEnum& turn,
        const float value,
        const float* const policy_logits)
    {
        simulate_ongoing_game(value);
        expand(actions, turn, policy_logits);
    }
    Node* backprop(const float v, Node* const child)
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
        const float q_fpu = first_play_urgency();
        Node* ch = m_child.get();
        Node* out = ch;
        float max_puct_score = ch->puct_score_from_parent_view(
            coeff_puct, m_sqrt_visit_count, q_fpu);

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
    float first_play_urgency() const;
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

private: // simulate
    void simulate_ongoing_game(const float value)
    {
        m_q_value = value;
    }

private: // backprop
    void update_most_visited_child(Node* const candidate)
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
            candidate->is_mate_to_win()
                ? !m_most_visited_child->is_mate_to_lose()
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
    /**
     * @brief `all(c->is_mate_to_win() for c in children)`
     *
     * @return true All childs are mate to win, which means mate to lose for
     * this node.
     * @return false There exists at least one child that is not mate to win,
     * which means this node is not mate to lose.
     */
    bool all_childs_are_mate_to_win() const
    {
        for (const Node* c = m_child.get(); c != nullptr;
             c = c->get_sibling()) {
            if (!c->is_mate_to_win())
                return false;
        }
        return true;
    }

public: // utility
    Node()
        : m_parent(nullptr), m_sibling(nullptr), m_child(nullptr), m_action(),
          m_proba(0.f), m_visit_count(0), m_visit_count_by_random(0),
          m_sqrt_visit_count(0.f), m_q_value(0.f), m_is_mate(false),
          m_most_visited_child(nullptr)
    {
    }
    Node(const move_t action, const float proba)
        : m_parent(nullptr), m_sibling(nullptr), m_child(nullptr),
          m_action(action), m_proba(proba), m_visit_count(0),
          m_visit_count_by_random(0), m_sqrt_visit_count(0.f), m_q_value(0.f),
          m_is_mate(false), m_most_visited_child(nullptr)
    {
        assert(action != 0u);
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
    float get_q_value(const uint greedy_depth = 0u) const
    {
        if (m_is_mate || (m_most_visited_child == nullptr)
            || (greedy_depth == 0u))
            return m_q_value;
        else
            return -m_most_visited_child->get_q_value(greedy_depth - 1u);
    }
    float get_proba() const
    {
        return m_proba;
    }
    move_t get_action() const
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
    const Node* get_child() const
    {
        return m_child.get();
    }
    const Node* get_child(const move_t& action) const
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
    Node& apply(const move_t& action)
    {
        Node* ch = m_child.get();
        for (; ch != nullptr; ch = ch->m_sibling.get()) {
            if (ch->m_action == action) {
                m_action = ch->m_action;
                m_proba = ch->m_proba;
                m_visit_count = ch->m_visit_count;
                m_visit_count_by_random = ch->m_visit_count_by_random;
                m_sqrt_visit_count = ch->m_sqrt_visit_count;
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
        m_q_value = 0.f;
        m_is_mate = false;
        m_most_visited_child = nullptr;
        return *this;
    }

private:
    bool has_mate_to_win() const
    {
        const Node* ch = m_child.get();
        for (; ch != nullptr; ch = ch->m_sibling.get()) {
            if (ch->is_mate_to_win())
                return true;
        }
        return false;
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
};

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
class Searcher
{
private:
    using MT = MoveTraits<P>;

private:
    std::unique_ptr<Node<P>> m_root;
    dfpn::Searcher<P> m_dfpn;
    const float m_coeff_puct;
    const float m_random_rate;
    const uint m_dfpn_search_root;
    const uint m_dfpn_search_leaf;

public:
    Searcher(
        const float coeff_puct,
        const float random_rate,
        const uint dfpn_search_root = 0u,
        const uint dfpn_search_leaf = 0u)
        : m_root(std::make_unique<Node<P>>()),
          m_dfpn{std::max(dfpn_search_root, dfpn_search_leaf) * 10u},
          m_coeff_puct(coeff_puct), m_random_rate(random_rate),
          m_dfpn_search_root(dfpn_search_root),
          m_dfpn_search_leaf(dfpn_search_leaf)
    {
    }
    void init_root()
    {
        m_root = std::make_unique<Node<P>>();
    }
    int get_visit_count() const
    {
        return m_root->get_visit_count();
    }
    bool proved_mate() const
    {
        return m_root->is_mate();
    }
    Node<P>* search(Game<P>& game)
    {
        Node<P>* const leaf = select_a_leaf_node(game);
        assert(leaf != nullptr);
        assert(!leaf->has_child());
        if (game.get_result() != ONGOING) {
            leaf->simulate(game);
            backprop_to_root(game, leaf);
            return nullptr;
        }
        if (leaf->is_mate_to_lose()) {
            leaf->expand(game.get_legal_moves(), game.get_turn(), nullptr);
            backprop_to_root(game, leaf);
            return nullptr;
        }
        assert(!leaf->is_mate_to_win());
        if (dfpn_proved_mate(game, leaf))
            return nullptr;
        return leaf;
    }
    void simulate_expand_backprop(
        Node<P>* const leaf,
        Game<P>& game,
        const float value,
        const float* const policy_logits = nullptr)
    {
        if (leaf == nullptr)
            return;
        leaf->simulate_ongoing_and_expand(
            game.get_legal_moves(), game.get_turn(), value, policy_logits);
        backprop_to_root(game, leaf);
    }
    Searcher<P>& apply(Game<P>& game, const move_t& action)
    {
        m_root->apply(action);
        game.apply(action);
        dfpn_proved_mate(game, m_root.get());
        return *this;
    }
    const Node<P>* get_root() const
    {
        return m_root.get();
    }
    move_t get_action_by_visit_max() const
    {
        const Node<P>* const ch = m_root->get_most_visited_child();
        if (ch == nullptr) {
            assert(!m_root->has_child());
            return move_t();
        } else
            return ch->get_action();
    }
    move_t get_action_by_visit_distribution(const float temperature) const
    {
        constexpr float eps = 1.f;

        std::vector<float> probas(m_root->get_num_child());
        const Node<P>* ch = m_root->get_child();
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
    move_t get_action_by_q_distribution(const float temperature) const
    {
        std::vector<float> probas(m_root->get_num_child());
        const Node<P>* ch = m_root->get_child();
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
    Node<P>* select_a_leaf_node(Game<P>& game)
    {
        Node<P>* n = m_root.get();
        while (n->has_child()) {
            Node<P>* const child = n->select_nocheck(
                game, m_coeff_puct, (n == m_root.get()) ? m_random_rate : 0.f);
            assert(child != nullptr);
            assert(child->get_parent() == n);
            n = child;
        }
        return n;
    }
    void backprop_to_root(Game<P>& game, Node<P>* const leaf)
    {
        float v = leaf->get_q_value();
        for (Node<P>*n = leaf, *prev = nullptr;; v = -v) {
            Node<P>* const p = n->backprop(v, prev);
            prev = n;
            n = p;
            if (n == nullptr) // `n` was root node.
                break;
            game.undo();
        }
    }
    bool dfpn_proved_mate(Game<P>& game, Node<P>* const node)
    {
        const uint search_count
            = (node == m_root.get()) ? m_dfpn_search_root : m_dfpn_search_leaf;
        if (search_count == 0u)
            return false;
        m_dfpn.init();
        m_dfpn.search(game, search_count);
        if (m_dfpn.proved_mate()) {
            const move_t m = m_dfpn.get_mate_move();
            if (m != 0u) {
                node->simulate_mate_and_expand(m_dfpn.get_mate_move());
                backprop_to_root(game, node);
                return true;
            }
        }
        return false;
    }
};

} // namespace vshogi::engine::mcts

#endif // VSHOGI_ENGINE_MCTS_HPP
