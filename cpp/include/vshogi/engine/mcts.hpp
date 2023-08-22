#ifndef VSHOGI_ENGINE_MCTS_HPP
#define VSHOGI_ENGINE_MCTS_HPP

#include <algorithm>
#include <cmath>
#include <memory>
#include <random>
#include <stdexcept>
#include <vector>

namespace vshogi::engine
{

static std::random_device dev;
static std::mt19937 rng(dev());
static std::uniform_real_distribution<float> uniform01(0, 1);

namespace internal
{

// arctanh(0.9999) = 4.951718775643098
constexpr float tanh_max = 0.9999f;
constexpr float atanh_max = 5.f;

inline float arctanh(const float x)
{
    if (x > tanh_max)
        return atanh_max;
    if (x < -tanh_max)
        return -atanh_max;
    return std::atanh(x);
}

inline float tanh(const float x)
{
    if (x > atanh_max)
        return 1.f;
    if (x < -atanh_max)
        return -1.f;
    return std::tanh(x);
}

} // namespace internal

template <class Game, class Move>
class Node
{
private:
    using NodeGM = Node<Game, Move>;

    /**
     * @brief `arctanh` of 1 ~ -1 scaled probability of the turn player winning
     * the game.
     * If the turn is black, then this value shows winning rate of black.
     * If the turn is white, then it shows the rate of white.
     * @details This is typically a raw estimate without `tanh` activation of a
     * machine learning model.
     */
    float m_value_arctanh;

    std::vector<Move> m_actions;
    std::vector<float> m_probas;
    NodeGM* m_parent;
    std::vector<NodeGM> m_children;

    /**
     * @brief Number of visit to this node in the course of explorations.
     */
    int m_visit_count;

    /**
     * @brief result of `std::sqrt(static_cast<float>(m_visit_count))`.
     * @details Computing this value beforehand to reduce the computation time
     * of `u_value_of_puct()` function.
     */
    float m_sqrt_visit_count;

    /**
     * @brief Average of `m_value_arctanh` of all the node below this and this
     * node weighted by their `m_visit_count`.
     */
    float m_q_arctanh;

    std::uniform_int_distribution<std::size_t> m_uniform_action_index;

public:
    Node()
        : m_value_arctanh(0.f), m_actions(), m_probas(), m_parent(nullptr),
          m_children(), m_visit_count(0), m_sqrt_visit_count(0.f),
          m_q_arctanh(0.f), m_uniform_action_index()
    {
    }

    /**
     * @brief Construct a new Node object
     *
     * @param value 1 ~ -1 scaled probability of turn player winning the game.
     * @param actions List of possible actions on the game.
     * @param probas 01-scaled probability of selecting corresponding actions.
     * It is users responsibility to make sure that the sum of them is 1.
     * @param parent Pointer to parent node if there is.
     */
    Node(
        const float value,
        const std::vector<Move>& actions,
        const std::vector<float>& probas,
        Node<Game, Move>* const parent = nullptr)
        : Node()
    {
        m_parent = parent;
        set_value_action_proba(value, actions, probas);
    }

    /**
     * @brief Set new value, actions, and probas.
     * @note This will initialize almost all the members except pointer to
     * parent node.
     *
     * @param value 1 ~ -1 scaled probability of turn player
     * winning the game.
     * @param actions List of possible actions on the game.
     * @param probas 01-scaled probability of selecting corresponding actions.
     * It is users responsibility to make sure that the sum of them is 1.
     */
    void set_value_action_proba(
        const float value,
        const std::vector<Move>& actions,
        const std::vector<float>& probas)
    {
        if (actions.size() != probas.size())
            throw std::invalid_argument(
                "# of actions (" + std::to_string(actions.size())
                + ") != # of probas (" + std::to_string(probas.size()) + ")");
        // m_parent = parent; Update everything except `m_parent`
        m_value_arctanh = internal::arctanh(value);
        m_actions = actions;
        m_probas = probas;
        m_children = std::vector<NodeGM>(actions.size());
        m_visit_count = 1;
        m_sqrt_visit_count = 1.f;
        m_q_arctanh = m_value_arctanh;
        m_uniform_action_index
            = std::uniform_int_distribution<std::size_t>(0, actions.size() - 1);

        if (m_parent != nullptr)
            m_parent->update_q_arctanh(-m_value_arctanh);
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
    /**
     * @brief Get 1 ~ -1 scaled probability of turn player winning the game.
     * @note If the node is not valid, it returns 0.
     *
     * @return float
     */
    float get_value() const
    {
        return internal::tanh(m_value_arctanh);
    }

    /**
     * @brief Get 1 ~ -1 scaled weighted average of turn player winning the
     * game.
     * @note If the node is not valid, it returns 0.
     *
     * @return float
     */
    float get_q_value() const
    {
        return internal::tanh(m_q_arctanh);
    }
    const std::vector<Move>& get_actions() const
    {
        return m_actions;
    }
    float get_proba(const Move& action) const
    {
        const auto index = get_index_of(action);
        return m_probas[index];
    }
    Node& get_child(const Move& action)
    {
        const auto index = get_index_of(action);
        return m_children[index];
    }

    /**
     * @brief Explore nodes starting from here using PUCT algorithm to a leaf
     * node.
     * @note Be sure to check that the node is valid (`is_valid()`) before
     * calling this method. Also please use the output and call `add_child()`
     * method after finishing this method (`(*leaf)->add_child(*action, ...)`).
     * @details PUCT algorithm explores nodes while taking
     * exploration-exploitation dilemma into consideration.
     *
     * @param [in,out] game The game position of the node. After the end, the
     * position corresponds to the leaf node.
     * @param [in] coeff_puct Coefficient of PUCT algorithm. Higher the value is,
     * stronger the exploration is.
     * @param [in] random_proba Probability of exploring child node at random.
     * e.g. If `random_proba == 0.25`, randomly explores child node 1 out of
     * 4 times, and explores the node with highest PUCT score 3 out of 4 times.
     * @param [in] random_depth Depth of nodes to explore in random manner.
     * e.g. If `random_depth == 2`, `random_proba` takes effect when exploring
     * child nodes from root node and nodes just below the root node, and
     * `random_proba` takes no effect as if it is 0 for the nodes further below.
     * @return Node<Game, Move> Leaf node reached by PUCT algorithm.
     * If it is game end, then output is null pointer.
     */
    Node<Game, Move>* explore(
        Game& game,
        const float coeff_puct,
        const float random_proba,
        const int random_depth)
    {
        if (!is_valid())
            return this;
        if (m_actions.empty()) { // game end
            increment_visit_count();
            update_q_arctanh(m_value_arctanh);
            return nullptr;
        }

        const auto index
            = get_action_index(coeff_puct, random_proba, random_depth);
        game.apply(m_actions[index]);
        increment_visit_count();
        NodeGM* child = &m_children[index];
        child->m_parent = this;
        return child->explore(game, coeff_puct, random_proba, random_depth - 1);
    }
    Node<Game, Move> pop_child(const Move action)
    {
        const auto index = get_index_of(action);
        auto out = std::move(m_children[index]);
        const auto visit_count_before = static_cast<float>(m_visit_count);
        m_children[index] = NodeGM();
        out.m_parent = nullptr;
        m_visit_count -= out.m_visit_count;
        m_sqrt_visit_count = std::sqrt(static_cast<float>(m_visit_count));
        m_q_arctanh
            = (m_q_arctanh * visit_count_before
               + out.m_q_arctanh * static_cast<float>(out.m_visit_count))
              / static_cast<float>(m_visit_count);
        return out;
    }

private:
    void increment_visit_count()
    {
        m_visit_count += 1;
        m_sqrt_visit_count = std::sqrt(static_cast<float>(m_visit_count));
    }
    std::size_t get_index_of(const Move action) const
    {
        const auto it = std::find(m_actions.cbegin(), m_actions.cend(), action);
        if (it == m_actions.cend())
            throw std::invalid_argument(
                "Given action ("
                + std::to_string(static_cast<int>(action.hash()))
                + ") not found in the node.");
        return static_cast<std::size_t>(std::distance(m_actions.cbegin(), it));
    }
    void update_q_arctanh(const float v_arctanh)
    {
        const auto count = static_cast<float>(m_visit_count);
        // The following two lines are equivalent with
        // `m_q_arctanh = (v_arctanh + (count - 1) * m_q_arctanh) / count;`.
        // But the following possibly prevents overflows.
        m_q_arctanh *= (count - 1.f) / count;
        m_q_arctanh += v_arctanh / count;
        if (m_parent != nullptr)
            m_parent->update_q_arctanh(-v_arctanh);
    }

    float puct_u(const std::size_t index) const
    {
        const auto p = m_probas[index];
        const auto visit_count_to_child = m_children[index].m_visit_count;
        return p * m_sqrt_visit_count
               / static_cast<float>(1 + visit_count_to_child);
    }
    float puct_q(const std::size_t index) const
    {
        return std::tanh(-m_children[index].m_q_arctanh);
    }
    float puct_score(const std::size_t index, const float coeff_puct) const
    {
        const auto score = puct_q(index) + puct_u(index) * coeff_puct;
        return score;
    }
    std::size_t action_index_with_max_puct_score(const float coeff_puct) const
    {
        std::size_t index = m_actions.size();
        float max_puct_score = -100.f;
        for (std::size_t ii = index; ii--;) {
            const auto score = puct_score(ii, coeff_puct);
            if (score > max_puct_score) {
                max_puct_score = score;
                index = ii;
            }
        }
        return index;
    }
    std::size_t random_action_index()
    {
        return m_uniform_action_index(rng);
    }
    std::size_t get_action_index(
        const float coeff_puct,
        const float random_proba,
        const int random_depth)
    {
        const auto use_random
            = (random_depth > 0) && (uniform01(rng) > random_proba);
        const auto index = use_random
                               ? random_action_index()
                               : action_index_with_max_puct_score(coeff_puct);
        return index;
    }
};

} // namespace vshogi::engine

#endif // VSHOGI_ENGINE_MCTS_HPP
