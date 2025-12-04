#ifndef VSHOGI_ENGINE_MCTS_SEARCHER_HPP
#define VSHOGI_ENGINE_MCTS_SEARCHER_HPP

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
#include "vshogi/engine/mcts/node.hpp"

namespace vshogi::engine::mcts
{

namespace dfpn = vshogi::engine::dfpn;

template <class P>
class Searcher
{
private:
    Node<P> m_root;
    dfpn::Searcher<P> m_dfpn;
    const float m_coeff_puct;
    const float m_random_rate;
    const uint m_dfpn_search_root;
    const uint m_dfpn_search_leaf;

public:
    Searcher(
        const float c_puct,
        const float p_random,
        const uint dfpn_search_root = 0u,
        const uint dfpn_search_leaf = 0u);

    // clang-format off
    void init_root() { m_root.init(); }
    uint get_visit_count() const { return m_root.get_visit_count(); }
    bool proved_mate() const { return m_root.is_mate(); }
    const Node<P>& get_root() const { return m_root; }
    // clang-format on

    Node<P>* search(Game<P>& game);
    void simulate_expand_backprop(
        Node<P>* const leaf,
        Game<P>& game,
        const float value,
        const float* const policy_logits = nullptr);
    Searcher<P>& apply(Game<P>& game, const move_t& action);
    move_t get_action_by_visit_max() const;
    move_t get_action_by_visit_distribution(const float temperature) const;
    move_t get_action_by_q_distribution(const float temperature) const;

private:
    Node<P>* select_a_leaf_node(Game<P>& game);
    void backprop_to_root(Game<P>& game, Node<P>* const leaf);
    bool dfpn_proved_mate(Game<P>& game, Node<P>* const node);
};

template <class P>
Searcher<P>::Searcher(
    const float c_puct,
    const float p_random,
    const uint dfpn_search_root,
    const uint dfpn_search_leaf)
    : m_root(), m_dfpn{std::max(dfpn_search_root, dfpn_search_leaf) * 10u},
      m_coeff_puct(c_puct), m_random_rate(p_random),
      m_dfpn_search_root(dfpn_search_root), m_dfpn_search_leaf(dfpn_search_leaf)
{
}

template <class P>
Node<P>* Searcher<P>::search(Game<P>& game)
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

template <class P>
void Searcher<P>::simulate_expand_backprop(
    Node<P>* const leaf,
    Game<P>& game,
    const float value,
    const float* const policy_logits)
{
    if (leaf == nullptr)
        return;
    leaf->simulate_ongoing_and_expand(
        game.get_legal_moves(), game.get_turn(), value, policy_logits);
    backprop_to_root(game, leaf);
}

template <class P>
Searcher<P>& Searcher<P>::apply(Game<P>& game, const move_t& action)
{
    m_root.apply(action);
    game.apply(action);
    dfpn_proved_mate(game, &m_root);
    return *this;
}

template <class P>
move_t Searcher<P>::get_action_by_visit_max() const
{
    const Node<P>* const ch = m_root.get_most_visited_child();
    if (ch == nullptr) {
        assert(!m_root->has_child());
        return move_t();
    } else
        return ch->get_action();
}

template <class P>
move_t
Searcher<P>::get_action_by_visit_distribution(const float temperature) const
{
    constexpr float eps = 1.f;
    std::vector<float> probas(m_root.get_num_child());
    const Node<P>* ch = m_root.get_child();
    for (uint ii = 0u; ch; ch = ch->get_sibling()) {
        const auto v
            = static_cast<float>(ch->get_visit_count_excluding_random());
        probas[ii++] = std::log((v + eps)) / temperature;
    }
    softmax(probas);

    float s = dist01(random_engine);
    ch = m_root.get_child();
    for (uint ii = 0u; ch; ch = ch->get_sibling()) {
        const auto p = probas[ii++];
        if (s < p)
            return ch->get_action();
        s -= p;
    }
    return ch->get_action(); // For numerical instability.
}

template <class P>
move_t Searcher<P>::get_action_by_q_distribution(const float temperature) const
{
    std::vector<float> probas(m_root.get_num_child());
    const Node<P>* c = m_root.get_child();
    for (uint ii = 0u; c; c = c->get_sibling()) {
        probas[ii++] = -c->get_q_value() / temperature;
    }
    softmax(probas);

    float s = dist01(random_engine);
    c = m_root.get_child();
    for (uint ii = 0u; c; c = c->get_sibling()) {
        const auto p = probas[ii++];
        if (s < p)
            return c->get_action();
        s -= p;
    }
    return c->get_action(); // For numerical instability.
}

template <class P>
Node<P>* Searcher<P>::select_a_leaf_node(Game<P>& game)
{
    Node<P>* n = &m_root;
    while (n->has_child()) {
        Node<P>* const child = n->select(
            game, m_coeff_puct, (n == &m_root) ? m_random_rate : 0.f);
        assert(child != nullptr);
        assert(child->get_parent() == n);
        n = child;
    }
    return n;
}

template <class P>
void Searcher<P>::backprop_to_root(Game<P>& game, Node<P>* const leaf)
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

template <class P>
bool Searcher<P>::dfpn_proved_mate(Game<P>& game, Node<P>* const node)
{
    const uint search_count
        = (node == &m_root) ? m_dfpn_search_root : m_dfpn_search_leaf;
    if (search_count == 0u)
        return false;
    m_dfpn.init();
    m_dfpn.search(game, search_count);
    if (m_dfpn.proved_mate()) {
        const move_t m = m_dfpn.get_mate_move();
        if (m != 0u) {
            node->simulate_mate_and_expand(m);
            backprop_to_root(game, node);
            return true;
        }
    }
    return false;
}

} // namespace vshogi::engine::mcts

#endif // VSHOGI_ENGINE_MCTS_SEARCHER_HPP
