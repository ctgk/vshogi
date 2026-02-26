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
#include "vshogi/common/notation.hpp"
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
public:
    Searcher(
        const uint tree_size = 1000000u,
        const uint dfpn_budget_root = 0u,
        const uint dfpn_budget_leaf = 0u);
    Node* search(
        Game<P>& game, const float c_puct = 4.f, const float p_random = 0.25f);
    void simulate_expand_backprop(
        Node* const leaf,
        Game<P>& game,
        const float value,
        const float* const policy_logits = nullptr);
    move_t select_action() const;
    move_t select_action(const float temperature) const;
    // clang-format off
    uint get_search_count() const { return m_nodes[0].get_visit_count(); }
    // clang-format on

private:
    using dfpn::DfpnAugmentedSearcher<P, Node>::m_nodes;
    using dfpn::DfpnAugmentedSearcher<P, Node>::m_next;
    using dfpn::DfpnAugmentedSearcher<P, Node>::backprop_to_root;
    using dfpn::DfpnAugmentedSearcher<P, Node>::simulate_backprop_if_possible;
    Node*
    select_a_leaf_node(Game<P>& game, const float c_puct, const float p_random);
};

template <class P>
Searcher<P>::Searcher(
    const uint tree_size,
    const uint dfpn_budget_root,
    const uint dfpn_budget_leaf)
    : dfpn::DfpnAugmentedSearcher<P, Node>(
          tree_size, dfpn_budget_root, dfpn_budget_leaf)
{
}

template <class P>
Node* Searcher<P>::search(
    Game<P>& game, const float c_puct, const float p_random)
{
    if (m_next->is_end())
        return nullptr;
    Node* const leaf = select_a_leaf_node(game, c_puct, p_random);
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
    move_t out = tree::Searcher<Node>::select_action();
    if (out)
        return out;

    const Node& root = m_nodes.front();
    if (not root.has_child())
        return static_cast<move_t>(0);

    float max_score = -std::numeric_limits<float>::infinity();
    for (const Node* c = root.get_child(); c; c = c->get_sibling()) {
        const float score = -c->get_q_value() + c->get_proba();
        if (score > max_score) {
            max_score = score;
            out = c->get_action();
        }
    }
    return out;
}

template <class P>
move_t Searcher<P>::select_action(const float temperature) const
{
    constexpr float eps = 1.f;
    const auto& root = m_nodes.front();
    const uint n = root.count_childs();
    if (n == 0u)
        return static_cast<move_t>(0);
    std::vector<float> probas(n);
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
    return root.get_child()->get_action(); // For numerical instability.
}

template <class P>
Node* Searcher<P>::select_a_leaf_node(
    Game<P>& game, const float c_puct, const float p_random)
{
    Node* n = &m_nodes[0];
    while (n->has_child()) {
        Node* const child
            = n->select(c_puct, (n == &m_nodes[0]) ? p_random : 0.f);
        assert(child != nullptr);
        game.apply_nocheck(child->get_action());
        assert(child->get_parent() == n);
        n = child;
    }
    return n;
}

} // namespace vshogi::engine::az

#endif // VSHOGI_ENGINE_AZ_SEARCHER_HPP
