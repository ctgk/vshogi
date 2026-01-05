#ifndef VSHOGI_ENGINE_GAZ_SEARCHER_HPP
#define VSHOGI_ENGINE_GAZ_SEARCHER_HPP

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
#include "vshogi/engine/gaz/node.hpp"
#include "vshogi/engine/tree/searcher.hpp"

namespace vshogi::engine::gaz
{

namespace dfpn = vshogi::engine::dfpn;

template <class P>
class Searcher : public tree::Searcher<Node>
{
private:
    dfpn::Searcher<P> m_dfpn;
    const uint m_dfpn_search_root;
    const uint m_dfpn_search_leaf;

    const Node* m_child_nodes[max_legal_moves];
    float m_gumbel_noises[max_legal_moves];

public:
    Searcher(
        const uint tree_size = 1000000u,
        const uint dfpn_search_root = 0u,
        const uint dfpn_search_leaf = 0u);
    ~Searcher() = default; // 1/5 destructor
    Searcher(const Searcher& other) = delete; // 2/5 copy constructor
    Searcher& operator=(const Searcher& other) = delete; // 3/5 copy assignment
    Searcher(Searcher&& other) = default; // 4/5 move constructor
    Searcher& operator=(Searcher&& other) = default; // 5/5 move assignment

    void init();
    Node* search(Game<P>& game);
    void simulate_expand_backprop(
        Node* const leaf,
        Game<P>& game,
        const float value,
        const float* const policy_logits = nullptr);
    void keep_top_n_actions(const uint num_actions);
    uint count_active_childs() const;
    move_t select_action() const;
    move_t select_action(const float& temperature) const;
    Searcher& apply(Game<P>& game, const move_t& action);
    // clang-format off
    uint get_search_count() const { return m_nodes[0].get_visit_count(); }
    bool proved_mate() const { return m_nodes[0].is_mate(); }
    // clang-format on

private:
    Node* select_a_leaf_node(Game<P>& game);
    bool dfpn_proved_mate(Game<P>& game, Node* const node);
    uint set_child_nodes_and_gumbel_noises();
    float score_of(const Node& child, const uint& max_visits) const;
    float score_of(
        const Node& child,
        const uint& max_visits,
        const float& gumbel_noise) const;
    uint max_child_visits() const;
};

template <class P>
Searcher<P>::Searcher(
    const uint tree_size,
    const uint dfpn_search_root,
    const uint dfpn_search_leaf)
    : tree::Searcher<Node>(tree_size),
      m_dfpn{std::max(dfpn_search_root, dfpn_search_leaf) * 10u},
      m_dfpn_search_root(dfpn_search_root),
      m_dfpn_search_leaf(dfpn_search_leaf), m_child_nodes{}, m_gumbel_noises{}
{
    init();
}

template <class P>
void Searcher<P>::init()
{
    tree::Searcher<Node>::init();
    m_child_nodes[0] = nullptr;
}

template <class P>
Node* Searcher<P>::search(Game<P>& game)
{
    Node* const leaf = select_a_leaf_node(game);
    assert(leaf != nullptr);
    assert(!leaf->has_child());
    if (game.get_result() != ONGOING) {
        leaf->simulate(game);
        backprop_to_root(game, leaf);
        return nullptr;
    }
    if (leaf->is_mate_to_lose()) {
        leaf->expand(m_next, game, nullptr);
        backprop_to_root(game, leaf);
        return nullptr;
    }
    assert(!leaf->is_mate_to_win());
    if (dfpn_proved_mate(game, leaf))
        return nullptr;
    return leaf;
}

template <class P>
void Searcher<P>::simulate_expand_backprop(
    Node* const leaf,
    Game<P>& game,
    const float value,
    const float* const policy_logits)
{
    if (leaf == nullptr)
        return;
    leaf->simulate_ongoing_and_expand(m_next, game, value, policy_logits);
    backprop_to_root(game, leaf);
}

template <class P>
Node* Searcher<P>::select_a_leaf_node(Game<P>& game)
{
    if (!m_nodes[0].has_child())
        return &m_nodes[0];
    Node* n = nullptr;
    if (m_child_nodes[0] == nullptr)
        n = &m_nodes[0];
    else {
        n = m_nodes[0].select_from(m_child_nodes);
        assert(n->get_parent() == &m_nodes[0]);
        game.apply_nocheck(n->get_action());
    }
    while (n->has_child()) {
        Node* const child = n->select();
        assert(child != nullptr);
        game.apply_nocheck(child->get_action());
        assert(child->get_parent() == n);
        n = child;
    }
    return n;
}

template <class P>
bool Searcher<P>::dfpn_proved_mate(Game<P>& game, Node* const node)
{
    const uint search_count
        = (node == &m_nodes[0]) ? m_dfpn_search_root : m_dfpn_search_leaf;
    if (search_count == 0u)
        return false;
    m_dfpn.init();
    m_dfpn.search(game, search_count);
    if (m_dfpn.proved_mate()) {
        const move_t m = m_dfpn.select_action();
        if (m != 0u) {
            node->simulate_mate_and_expand(m_next, m);
            backprop_to_root(game, node);
            if (node == &m_nodes[0]) {
                m_child_nodes[0] = m_nodes[0].get_child_1st();
                m_child_nodes[1] = nullptr;
            }
            return true;
        }
    }
    return false;
}

template <class P>
void Searcher<P>::keep_top_n_actions(const uint num_actions)
{
    assert(num_actions > 1u);
    const uint num_child = set_child_nodes_and_gumbel_noises();
    const uint max_visits = max_child_visits();
    std::tuple<const Node*, float, float> data[max_legal_moves] = {};
    for (uint ii = 0u; ii < num_child; ++ii) {
        const Node* const c = m_child_nodes[ii];
        const float q = sigma(-c->get_q_value(), max_visits);
        const float score = m_gumbel_noises[ii] + c->get_logit() + q;
        data[ii] = std::make_tuple(c, m_gumbel_noises[ii], score);
    }
    std::sort(
        data,
        data + num_child,
        [](const std::tuple<const Node*, float, float>& a,
           const std::tuple<const Node*, float, float>& b) {
            assert(std::get<0>(a));
            assert(std::get<0>(b));
            return std::get<2>(a) > std::get<2>(b);
        });
    uint ii = std::min(num_actions, num_child);
    m_child_nodes[ii] = nullptr;
    for (; ii--;) {
        m_child_nodes[ii] = std::get<0>(data[ii]);
        m_gumbel_noises[ii] = std::get<1>(data[ii]);
    }
}

template <class P>
uint Searcher<P>::set_child_nodes_and_gumbel_noises()
{
    static std::extreme_value_distribution<float> gumbel_dist(0.f, 1.f);

    if (m_child_nodes[0] == nullptr) {
        uint index = 0u;
        for (const Node* c = m_nodes[0].get_child(); c; c = c->get_sibling()) {
            m_child_nodes[index] = c;
            m_gumbel_noises[index] = gumbel_dist(random_engine);
            ++index;
        }
        m_child_nodes[index] = nullptr;
        return index;
    }
    for (uint ii = 0; ii < max_legal_moves; ++ii) {
        if (m_child_nodes[ii] == nullptr)
            return ii;
    }
    assert(false);
    return max_legal_moves;
}

template <class P>
uint Searcher<P>::count_active_childs() const
{
    for (uint ii = 0u; ii < max_legal_moves; ++ii) {
        if (m_child_nodes[ii] == nullptr)
            return ii;
    }
    return 0u;
}

template <class P>
move_t Searcher<P>::select_action() const
{
    float max_score = -std::numeric_limits<float>::infinity();
    move_t out{};

    const uint max_visits = max_child_visits();
    if (m_child_nodes[0] == nullptr) {
        for (const Node* c = &m_nodes[1]; &m_nodes[0] == c->get_parent(); ++c) {
            const float score = score_of(*c, max_visits);
            if (score > max_score) {
                max_score = score;
                out = c->get_action();
            }
        }
    } else {
        const float* noise = m_gumbel_noises;
        for (auto c = m_child_nodes; *c; ++c) {
            const float score = score_of(**c, max_visits, *noise++);
            if (score > max_score) {
                max_score = score;
                out = (*c)->get_action();
            }
        }
    }
    return out;
}

template <class P>
move_t Searcher<P>::select_action(const float& temperature) const
{
    std::vector<float> probas{};
    probas.reserve(m_nodes[0].count_childs());
    const uint max_visits = max_child_visits();
    for (auto c = &m_nodes[1]; &m_nodes[0] == c->get_parent(); ++c) {
        const float score = score_of(*c, max_visits) / temperature;
        probas.emplace_back(score);
    }
    softmax(probas);
    const float p_uniform = 1.f / static_cast<float>(probas.size());
    float sample = dist01(random_engine);
    for (auto c = &m_nodes[1]; &m_nodes[0] == c->get_parent(); ++c) {
        if (sample < p_uniform)
            return c->get_action();
        sample -= p_uniform;
    }
    assert(false);
    return m_nodes[1].get_action(); // just in case for numerical instability
}

template <class P>
float Searcher<P>::score_of(const Node& child, const uint& max_visits) const
{
    return child.get_logit() + sigma(-child.get_q_value(), max_visits);
}

template <class P>
float Searcher<P>::score_of(
    const Node& child, const uint& max_visits, const float& gumbel_noise) const
{
    return score_of(child, max_visits) + gumbel_noise;
}

template <class P>
uint Searcher<P>::max_child_visits() const
{
    const Node* c1 = get_root().get_child_1st();
    return c1 ? c1->get_visit_count() : 0u;
}

template <class P>
Searcher<P>& Searcher<P>::apply(Game<P>& game, const move_t& action)
{
    tree::Searcher<Node>::apply(action);
    m_child_nodes[0] = nullptr;
    game.apply(action);
    dfpn_proved_mate(game, &m_nodes[0]);
    return *this;
}

} // namespace vshogi::engine::gaz

#endif // VSHOGI_ENGINE_GAZ_SEARCHER_HPP
