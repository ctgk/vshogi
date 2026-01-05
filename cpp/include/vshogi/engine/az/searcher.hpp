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
class Searcher : public dfpn::DfpnAugmentedSearcher<P, Node>
{
private:
    const float m_coeff_puct;
    const float m_random_rate;

public:
    Searcher(
        const float c_puct = 4.f,
        const float p_random = 0.25f,
        const uint tree_size = 1000000u,
        const uint dfpn_budget_root = 0u,
        const uint dfpn_budget_leaf = 0u);
    Node* search(Game<P>& game);
    void simulate_expand_backprop(
        Node* const leaf,
        Game<P>& game,
        const float value,
        const float* const policy_logits = nullptr);
    move_t select_action() const;
    move_t select_action(const float temperature) const;
    // clang-format off
    uint get_search_count() const { return m_nodes[0].get_visit_count(); }
    bool proved_mate() const { return m_nodes[0].is_mate(); }
    // clang-format on

private:
    using dfpn::DfpnAugmentedSearcher<P, Node>::m_nodes;
    using dfpn::DfpnAugmentedSearcher<P, Node>::m_next;
    using dfpn::DfpnAugmentedSearcher<P, Node>::backprop_to_root;
    using dfpn::DfpnAugmentedSearcher<P, Node>::simulate_backprop_if_possible;
    Node* select_a_leaf_node(Game<P>& game);
};

template <class P>
Searcher<P>::Searcher(
    const float c_puct,
    const float p_random,
    const uint tree_size,
    const uint dfpn_budget_root,
    const uint dfpn_budget_leaf)
    : dfpn::DfpnAugmentedSearcher<P, Node>(
          tree_size, dfpn_budget_root, dfpn_budget_leaf),
      m_coeff_puct(c_puct), m_random_rate(p_random)
{
}

template <class P>
Node* Searcher<P>::search(Game<P>& game)
{
    Node* const leaf = select_a_leaf_node(game);
    return simulate_backprop_if_possible(game, leaf);
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
move_t Searcher<P>::select_action() const
{
    return tree::Searcher<Node>::select_action();
}

template <class P>
move_t Searcher<P>::select_action(const float temperature) const
{
    constexpr float eps = 1.f;
    const auto& root = m_nodes.front();
    std::vector<float> probas(root.count_childs());
    const Node* ch = root.get_child();
    for (uint ii = 0u; ch; ch = ch->get_sibling()) {
        const auto v
            = static_cast<float>(ch->get_visit_count_excluding_random());
        probas[ii++] = std::log((v + eps)) / temperature;
    }
    softmax(probas);

    float s = dist01(random_engine);
    ch = root.get_child();
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

} // namespace vshogi::engine::az

#endif // VSHOGI_ENGINE_AZ_SEARCHER_HPP
