#ifndef VSHOGI_ENGINE_AZ_SEARCHER_HPP
#define VSHOGI_ENGINE_AZ_SEARCHER_HPP

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
#include "vshogi/engine/az/node.hpp"
#include "vshogi/engine/dfpn/searcher.hpp"
#include "vshogi/engine/tree/searcher.hpp"

namespace vshogi::engine::az
{

namespace dfpn = vshogi::engine::dfpn;

template <class P>
class Searcher : public tree::Searcher<Node>
{
private:
    dfpn::Searcher<P> m_dfpn;
    const float m_coeff_puct;
    const float m_random_rate;
    const uint m_dfpn_search_root;
    const uint m_dfpn_search_leaf;

public:
    Searcher(
        const float c_puct = 4.f,
        const float p_random = 0.25f,
        const uint tree_size = 1000000u,
        const uint dfpn_search_root = 0u,
        const uint dfpn_search_leaf = 0u);
    Node* search(Game<P>& game);
    void simulate_expand_backprop(
        Node* const leaf,
        Game<P>& game,
        const float value,
        const float* const policy_logits = nullptr);
    Searcher<P>& apply(Game<P>& game, const move_t& action);
    move_t select_action() const;
    move_t select_action(const float temperature) const;
    // clang-format off
    uint get_search_count() const { return m_nodes[0].get_visit_count(); }
    bool proved_mate() const { return m_nodes[0].is_mate(); }
    // clang-format on

private:
    Node* select_a_leaf_node(Game<P>& game);
    bool dfpn_proved_mate(Game<P>& game, Node* const node);
};

template <class P>
Searcher<P>::Searcher(
    const float c_puct,
    const float p_random,
    const uint tree_size,
    const uint dfpn_search_root,
    const uint dfpn_search_leaf)
    : tree::Searcher<Node>(tree_size),
      m_dfpn{std::max(dfpn_search_root, dfpn_search_leaf) * 10u},
      m_coeff_puct(c_puct), m_random_rate(p_random),
      m_dfpn_search_root(dfpn_search_root), m_dfpn_search_leaf(dfpn_search_leaf)
{
    tree::Searcher<Node>::init();
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
Searcher<P>& Searcher<P>::apply(Game<P>& game, const move_t& action)
{
    remove_unselected_nodes(action);
    assert(&m_nodes.front() < m_next);
    assert(m_next < &m_nodes.back());
    m_nodes.front().init_as_begin();
    game.apply(action);
    dfpn_proved_mate(game, &m_nodes[0]);
    return *this;
}

template <class P>
move_t Searcher<P>::select_action() const
{
    return tree::Searcher<Node>::select_action();
}

template <class P>
move_t Searcher<P>::select_action(const float temperature) const
{
    constexpr float eps = 1.f;
    std::vector<float> probas(m_nodes[0].count_childs());
    const Node* ch = m_nodes[0].get_child();
    for (uint ii = 0u; ch; ch = ch->get_sibling()) {
        const auto v
            = static_cast<float>(ch->get_visit_count_excluding_random());
        probas[ii++] = std::log((v + eps)) / temperature;
    }
    softmax(probas);

    float s = dist01(random_engine);
    ch = m_nodes[0].get_child();
    for (uint ii = 0u; ch; ch = ch->get_sibling()) {
        const auto p = probas[ii++];
        if (s < p)
            return ch->get_action();
        s -= p;
    }
    return ch->get_action(); // For numerical instability.
}

template <class P>
Node* Searcher<P>::select_a_leaf_node(Game<P>& game)
{
    Node* n = &m_nodes[0];
    while (n->has_child()) {
        Node* const child
            = n->select(m_coeff_puct, (n == &m_nodes[0]) ? m_random_rate : 0.f);
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
    using C = Configuration<P>;
    const uint search_count
        = (node == &m_nodes[0]) ? m_dfpn_search_root : m_dfpn_search_leaf;
    if (search_count == 0u)
        return false;
    if (game.get_state().get_king_square(~game.get_turn()) == C::SQ_NA)
        return false;
    m_dfpn.init();
    m_dfpn.search(game, search_count);
    if (m_dfpn.proved_mate()) {
        const move_t m = m_dfpn.select_action();
        if (m != 0u) {
            node->simulate_mate_and_expand(m_next, m);
            backprop_to_root(game, node);
            return true;
        }
    }
    return false;
}

} // namespace vshogi::engine::az

#endif // VSHOGI_ENGINE_AZ_SEARCHER_HPP
