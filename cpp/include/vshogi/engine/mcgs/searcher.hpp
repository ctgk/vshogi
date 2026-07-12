#ifndef VSHOGI_ENGINE_MCGS_SEARCHER_HPP
#define VSHOGI_ENGINE_MCGS_SEARCHER_HPP

#include <unordered_map>
#ifndef NDEBUG
#include <set>
#endif

#include "vshogi/common/move.hpp"
#include "vshogi/common/utils.hpp"
#include "vshogi/engine/contiguous_buffer.hpp"
#include "vshogi/engine/dfpn/searcher.hpp"
#include "vshogi/engine/mcgs/edge.hpp"
#include "vshogi/engine/mcgs/node.hpp"

namespace vshogi::engine::mcgs
{

template <class P>
class Searcher
{
public:
    Searcher(
        const uint size = 100000u,
        const uint dfpn_budget_root = 0u,
        const uint dfpn_budget_leaf = 0u);
    Node* search(Game<P>& game, const float epsilon_greedy = 0.f);
    void simulate_expand_backprop(
        Node* const leaf,
        Game<P>& game,
        const float value,
        const float* const policy_logits = nullptr);
    move_t select_action() const;
    move_t select_action(const float temperature) const;
    std::vector<move_t> get_mate_moves(Game<P>& game);
    void apply(Game<P>& game, const move_t action);
    // clang-format off
    void init() { m_root.init(); m_node_table.clear(); m_edge_buffer.init(); m_trajectory.clear(); }
    const Node& get_root() const { return m_root; }
    uint get_search_count() const { return m_root.get_visits(); }
    bool proved_mate() const { return get_root().is_mate(); }
    const ContiguousBuffer<Edge>& edge_buffer() const { return m_edge_buffer; }
    auto num_nodes_in_table() const { return m_node_table.size(); }
    auto num_edges_in_buffer() const { return m_edge_buffer.size(); }
    // clang-format on

protected:
    dfpn::Searcher<P> m_dfpn;
    const uint m_dfpn_budget_root;
    const uint m_dfpn_budget_leaf;
    Node m_root;
    std::unordered_map<ZobristHashType, Node> m_node_table;
    ContiguousBuffer<Edge> m_edge_buffer;
    std::vector<Edge*> m_trajectory;

private:
    // select
    Node*
    select_leaf_or_transposition(Game<P>& game, const float epsilon_greedy);
    Node* find_child_node_of(Edge* const edge, const Game<P>& game);

    // backprop
    void backprop_to_root(Game<P>& game);

    bool dfpn_proved_mate(Game<P>& game, Node* const node);
    bool follow_line_or_node(Game<P>& game, const Node* const node);
    bool follow_line_and_node(Game<P>& game, const Node* const node);
    bool follow_dfpn(Game<P>& game);

    void remove_unselected_edges_nodes(const move_t selected);
};

template <class P>
Searcher<P>::Searcher(
    const uint size, const uint dfpn_budget_root, const uint dfpn_budget_leaf)
    : m_dfpn{std::max(dfpn_budget_root, dfpn_budget_leaf) * 10u},
      m_dfpn_budget_root{dfpn_budget_root},
      m_dfpn_budget_leaf{dfpn_budget_leaf}, m_root{}, m_node_table{},
      m_edge_buffer{size}, m_trajectory{}
{
    m_trajectory.reserve(32u);
}

template <class P>
Node* Searcher<P>::search(Game<P>& game, const float epsilon_greedy)
{
    Node* const n = select_leaf_or_transposition(game, epsilon_greedy);
    assert(n); // should be able to select at least root node.
    if (game.get_result() != ONGOING) {
        n->simulate(game);
        backprop_to_root(game);
        return nullptr;
    }
    if (n->get_visits() or n->has_child()) {
        backprop_to_root(game);
        return nullptr;
    }
    if (n->is_mate_to_lose()) {
        n->expand(m_edge_buffer, game, nullptr);
        backprop_to_root(game);
        return nullptr;
    }
    if (dfpn_proved_mate(game, n))
        return nullptr;
    return n;
}

template <class P>
std::vector<move_t> Searcher<P>::get_mate_moves(Game<P>& game)
{
    std::vector<move_t> out{};
    if (!proved_mate())
        return out;

    dfpn::ScopedGame scope{game};
    const Node& root = get_root();
    if (follow_line_or_node(game, &root)) {
        for (uint ii = 0u; ii < game.ply(); ++ii) {
            out.emplace_back(game.get_record_action(ii));
        }
    }
    for (uint ii = game.ply(); ii--;)
        game.undo();
    assert(game.ply() == 0u);
    return out;
}

template <class P>
bool Searcher<P>::follow_line_or_node(Game<P>& game, const Node* const node)
{
    assert(game.ply() % 2u == 0u);
    const ResultEnum expect = game.get_turn() ? WHITE_WIN : BLACK_WIN;
    if (game.ply() > 32u)
        return false; // to prevent falling into infinite loop
    if ((node == nullptr) or (node->is_leaf()))
        return follow_dfpn(game);
    for (auto c = node->get_child(); c->get_parent() == node; ++c) {
        if (c->is_mate_to_win()) {
            const move_t move_atk = c->get_action();
            if (!game.is_legal(move_atk))
                continue;
            game.apply_nocheck(move_atk);
            if (game.get_result() == expect)
                return true;
            if (follow_line_and_node(game, c->get_child()))
                return true;
            game.undo();
        }
    }
    return false;
}

template <class P>
bool Searcher<P>::follow_line_and_node(Game<P>& game, const Node* const node)
{
    assert(game.ply() % 2u == 1u);
    const ResultEnum expect = game.get_turn() ? BLACK_WIN : WHITE_WIN;
    if (game.ply() > 32u)
        return false; // to prevent falling into infinite loop
    if (node and node->has_child()) {
        for (auto e = node->get_child(); e->get_parent() == node; ++e) {
            if (e->is_mate_to_lose()) {
                const move_t move_blk = e->get_action();
                if (game.is_legal(move_blk)) {
                    game.apply_nocheck(move_blk);
                    if (game.get_result() == expect)
                        return true;
                    if (follow_line_or_node(game, e->get_child()))
                        return true;
                }
                return false;
            }
        }
    }
    const auto moves_blk = game.get_legal_moves();
    const uint index = static_cast<uint>(
        dist01(random_engine) * static_cast<float>(moves_blk.size()));
    const auto move_blk = moves_blk[index];
    game.apply_nocheck(move_blk);
    if (follow_dfpn(game))
        return true;
    game.undo();
    return false;
}

template <class P>
bool Searcher<P>::follow_dfpn(Game<P>& game)
{
    assert(game.ply() % 2u == 0u);
    m_dfpn.init();
    m_dfpn.search(game, std::max(m_dfpn_budget_root, m_dfpn_budget_leaf));
    if (!m_dfpn.proved_mate())
        return false;
    const auto mate_moves = m_dfpn.get_mate_moves(game);
    for (auto&& m : mate_moves)
        game.apply_nocheck(m);
    return true;
}

template <class P>
void Searcher<P>::apply(Game<P>& game, const move_t action)
{
    remove_unselected_edges_nodes(action);
    game.apply(action);
    dfpn_proved_mate(game, &m_root);
}

namespace internal
{

#ifndef NDEBUG
template <class P>
bool check_node_edges(const Node& node, const Game<P>& game)
{
    const auto moves = game.get_legal_moves();
    std::set<move_t> expected(moves.begin(), moves.end());
    if (node.is_mate_to_win() && (node.count_childs() == 1u)) {
        const auto a = node.get_child()->get_action();
        assert(expected.find(a) != expected.end());
        return true;
    }
    {
        std::set<move_t> actual{};
        for (const Edge* e = node.get_child(); e; e = e->get_sibling()) {
            actual.emplace(e->get_action());
        }
        assert(expected == actual);
    }
    {
        std::set<move_t> actual{};
        for (auto e = node.get_child(); e && (e->get_parent() == &node); ++e)
            actual.emplace(e->get_action());
        assert(expected == actual);
    }
    return true;
}
#endif

} // namespace internal

template <class P>
Node* Searcher<P>::select_leaf_or_transposition(
    Game<P>& game, const float epsilon_greedy)
{
    m_trajectory.clear();
    const bool explore = dist01(random_engine) <= epsilon_greedy;
    float depth = explore ? -std::log2f(1.f - dist01(random_engine)) - 1.f
                          : std::numeric_limits<float>::infinity();
    Node* n = &m_root;
    bool disable_backprop = false;
    while (n->has_child() && n->get_visits()) {
        assert(internal::check_node_edges(*n, game));

        Edge* e = nullptr;
        if (n->get_visits() == 1u) {
            e = n->select();
        } else if (depth < 0.f and !n->is_mate_to_win()) {
            e = n->select_unexplored();
            if (e)
                disable_backprop = true;
        }
        if (e == nullptr) {
            e = n->select();
        }
        assert(e && (e->get_parent() == n));

        m_trajectory.emplace_back(e);
        const move_t a = e->get_action();
        assert(game.is_legal(a));
        game.apply_nocheck(a);
        n = find_child_node_of(e, game);
        if ((n == nullptr) or (n->get_visits() > e->get_visits()))
            break;
        depth -= 1.f;
    }
    if (disable_backprop) {
        for (auto& e : m_trajectory)
            e = nullptr;
    }
    return n;
}

template <class P>
Node* Searcher<P>::find_child_node_of(Edge* const edge, const Game<P>& game)
{
    assert(edge);
    if (edge->has_child())
        return edge->child();
    const auto hash = game.hash_with_history();
    if (auto it = m_node_table.find(hash); it != m_node_table.end())
        return &(edge->set_child(it->second));
    if (auto [it, inserted] = m_node_table.try_emplace(hash); inserted)
        return &(edge->set_child(it->second));
    return nullptr;
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
    assert(
        m_trajectory.empty() || (m_trajectory[0] == nullptr)
        || ((*m_trajectory.rbegin())->get_child() == leaf
            and m_trajectory[0]->get_parent() == &m_root));
    leaf->simulate_ongoing_and_expand(
        m_edge_buffer, game, value, policy_logits);
    backprop_to_root(game);
}

template <class P>
void Searcher<P>::backprop_to_root(Game<P>& game)
{
    for (auto it = m_trajectory.rbegin(); it != m_trajectory.rend(); ++it) {
        if (*it) {
            assert((*it)->get_parent());
            (*it)->backprop();
        }
        game.undo();
    }
    m_root.backprop();
    m_trajectory.clear();
}

template <class P>
move_t Searcher<P>::select_action() const
{
    move_t out{};
    uint max_visits = 0u;
    const Node* const root = &get_root();
    if (root->is_leaf())
        return out;
    out = root->get_child()->get_action();
    for (const Edge* e = root->get_child(); e->get_parent() == root; ++e) {
        if (e->is_mate_to_win())
            return e->get_action();
        if (e->get_visits() > max_visits) {
            max_visits = e->get_visits();
            out = e->get_action();
        }
    }
    return out;
}

template <class P>
move_t Searcher<P>::select_action(const float temperature) const
{
    constexpr float eps = 1.f;
    const Node* const root = &get_root();
    const uint n = root->count_childs();
    if (n == 0u)
        return static_cast<move_t>(0);
    std::vector<float> probas(n);
    const Edge* e = root->get_child();
    for (uint ii = 0u; e->get_parent() == root; ++e) {
        const float v = static_cast<float>(e->get_visits());
        probas[ii++] = std::log(v + eps) / temperature;
    }
    softmax(probas);

    float s = dist01(random_engine);
    e = root->get_child();
    for (uint ii = 0u; e->get_parent() == root; ++e) {
        const auto p = probas[ii++];
        if (s < p)
            return e->get_action();
        s -= p;
    }
    return root->get_child()->get_action(); // for numerical instability.
}

template <class P>
bool Searcher<P>::dfpn_proved_mate(Game<P>& game, Node* const node)
{
    using C = Configuration<P>;
    const uint budget
        = (node == &get_root()) ? m_dfpn_budget_root : m_dfpn_budget_leaf;
    if (budget == 0u)
        return false;
    if (game.get_state().get_king_square(~game.get_turn()) == C::SQ_NA)
        return false;
    m_dfpn.init();
    m_dfpn.search(game, budget);
    if (m_dfpn.proved_mate()) {
        const move_t action = m_dfpn.select_action();
        if (action) {
            node->simulate_mate_and_expand(m_edge_buffer, action);
            backprop_to_root(game);
            return true;
        }
    }
    return false;
}

template <class P>
void Searcher<P>::remove_unselected_edges_nodes(const move_t selected)
{
    if (m_root.is_leaf())
        return;
    {
        // assert(m_root.is_leaf() or m_root.get_child() == m_edge_buffer.data());
        // Note that above expression may not be true after edge buffer compaction.
        Edge* selected_edge = nullptr;
        const auto end = m_root.get_child() + m_root.count_childs();
        for (Edge* e = m_root.child(); e != end; ++e) {
            if (e->get_action() == selected)
                selected_edge = e;
            else
                e->destruct();
        }
        if (selected_edge) {
            Node* const c = selected_edge->pop_child();
            if (c)
                m_root = std::move(*c);
            else
                init();
            selected_edge->destruct();
        } else {
            init();
        }
    }
    {
        Edge* const end = m_edge_buffer.next();
        Edge*& next = m_edge_buffer.next();
        next = m_edge_buffer.data();
        for (Edge* e = m_edge_buffer.data(); e < end; ++e) {
            if (e->get_parent() == nullptr)
                continue; // destructed edge
            *next++ = std::move(*e);
        }
        if (next == m_edge_buffer.data())
            next->init();

        for (auto it = m_node_table.begin(); it != m_node_table.end();) {
            if (it->second.num_parents() == 0u)
                it = m_node_table.erase(it);
            else
                ++it;
        }
    }
    assert(m_root.num_parents() == 0u);
    assert(
        m_root.get_child() == nullptr
        or m_root.get_child() == m_edge_buffer.data());
    if (m_root.get_child() != nullptr
        and m_root.get_child() != m_edge_buffer.data())
        init();
}

} // namespace vshogi::engine::mcgs

#endif // VSHOGI_ENGINE_MCGS_SEARCHER_HPP
