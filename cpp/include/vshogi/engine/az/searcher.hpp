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

namespace vshogi::engine::az
{

namespace dfpn = vshogi::engine::dfpn;

template <class P>
class Searcher
{
private:
    std::vector<Node> m_nodes;
    Node* m_next;
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
    ~Searcher() = default; // 1/5 destructor
    Searcher(const Searcher& other) = delete; // 2/5 copy constructor
    Searcher& operator=(const Searcher& other) = delete; // 3/5 copy assignment
    Searcher(Searcher&& other) = delete; // 4/5 move constructor
    Searcher& operator=(Searcher&& other) = delete; // 5/5 move assignment
    void init();
    Node* search(Game<P>& game);
    void simulate_expand_backprop(
        Node* const leaf,
        Game<P>& game,
        const float value,
        const float* const policy_logits = nullptr);
    Searcher<P>& apply(Game<P>& game, const move_t& action);
    move_t get_action_by_visit_max() const;
    move_t get_action_by_visit_distribution(const float temperature) const;
    move_t get_action_by_q_distribution(const float temperature) const;
    // clang-format off
    uint get_search_count() const { return m_nodes[0].get_visit_count(); }
    bool proved_mate() const { return m_nodes[0].is_mate(); }
    const Node& get_root() const { return m_nodes[0]; }
    uint count_nodes_remain() const { return static_cast<uint>(m_nodes.size()) - static_cast<uint>(m_next - m_nodes.data()) - 1u; }
    // clang-format on

private:
    Node* select_a_leaf_node(Game<P>& game);
    void backprop_to_root(Game<P>& game, Node* const leaf);
    bool dfpn_proved_mate(Game<P>& game, Node* const node);
    void eliminate_unselected_nodes(const move_t& selected);
    Node* compress_nodes();
};

template <class P>
Searcher<P>::Searcher(
    const float c_puct,
    const float p_random,
    const uint tree_size,
    const uint dfpn_search_root,
    const uint dfpn_search_leaf)
    : m_nodes(tree_size + 2u), m_next{},
      m_dfpn{std::max(dfpn_search_root, dfpn_search_leaf) * 10u},
      m_coeff_puct(c_puct), m_random_rate(p_random),
      m_dfpn_search_root(dfpn_search_root), m_dfpn_search_leaf(dfpn_search_leaf)
{
    init();
}

template <class P>
void Searcher<P>::init()
{
    m_nodes[0].init();
    m_next = std::next(m_nodes.data());
    m_next->init();
    m_nodes.back().init_as_end();
    m_dfpn.init();
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
    eliminate_unselected_nodes(action);
    m_next = compress_nodes();
    if (m_next == m_nodes.data()) { // all childs are eliminated
        m_next->init();
        ++m_next;
    }
    assert(&m_nodes.front() < m_next);
    assert(m_next < &m_nodes.back());
    m_nodes.front().init_as_begin();
    m_next->init();
    game.apply(action);
    dfpn_proved_mate(game, &m_nodes[0]);
    return *this;
}

template <class P>
void Searcher<P>::eliminate_unselected_nodes(const move_t& selected)
{
    for (Node* c = m_nodes.data() + 1; c->get_parent() == &m_nodes[0]; ++c) {
        if (c->get_action() != selected)
            c->destruct();
    }
}

template <class P>
Node* Searcher<P>::compress_nodes()
{
    Node* next = m_nodes.data();
    for (Node* n = m_nodes.data() + 1; n < m_next; ++n) {
        if (n->get_parent() == nullptr)
            continue; // destructed node
        if (n->get_parent() != next) {
            if (n->get_parent()->get_child() == n)
                n->update_child_of_parent(next);
            if (n->get_parent()->get_child_1st() == n)
                n->update_child_1st_of_parent(next);
        }
        *next = *n;
        next->update_parent_of_childs();
        ++next;
    }
    return next;
}

template <class P>
move_t Searcher<P>::get_action_by_visit_max() const
{
    const Node* c = m_nodes[0].get_child_1st();
    if (c == nullptr)
        c = m_nodes[0].get_child();
    return c ? c->get_action() : static_cast<move_t>(0);
}

template <class P>
move_t
Searcher<P>::get_action_by_visit_distribution(const float temperature) const
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
move_t Searcher<P>::get_action_by_q_distribution(const float temperature) const
{
    std::vector<float> probas(m_nodes[0].count_childs());
    const Node* c = m_nodes[0].get_child();
    for (uint ii = 0u; c; c = c->get_sibling()) {
        probas[ii++] = -c->get_q_value() / temperature;
    }
    softmax(probas);

    float s = dist01(random_engine);
    c = m_nodes[0].get_child();
    for (uint ii = 0u; c; c = c->get_sibling()) {
        const auto p = probas[ii++];
        if (s < p)
            return c->get_action();
        s -= p;
    }
    return c->get_action(); // For numerical instability.
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
void Searcher<P>::backprop_to_root(Game<P>& game, Node* const leaf)
{
    float v = leaf->get_q_value();
    for (Node *node = leaf, *child = nullptr;; v = -v) {
        Node* const parent = node->backprop(v, child);
        child = node;
        node = parent;
        if (parent == nullptr) // `node` was root node.
            break;
        game.undo();
    }
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
        const move_t m = m_dfpn.get_mate_move();
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
