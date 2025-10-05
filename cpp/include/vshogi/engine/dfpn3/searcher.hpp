#ifndef VSHOGI_ENGINE_DFPN3_SEARCHER_HPP
#define VSHOGI_ENGINE_DFPN3_SEARCHER_HPP

#include <list>
#include <memory>
#include <unordered_map>

#include "vshogi/common/game.hpp"
#include "vshogi/common/move.hpp"
#include "vshogi/engine/dfpn3/node.hpp"

namespace vshogi::engine::dfpn3
{

template <class P>
class Table
{
    using C = Configuration<P>;
    using BaseTypeStand = typename C::BaseTypeStand;
    using StandNodePairs = std::list<std::pair<BaseTypeStand, Node<P>*>>;

private:
    std::unordered_map<std::uint64_t, StandNodePairs> m_table;

public:
    Table() : m_table{}
    {
    }
    void clear()
    {
        m_table.clear();
    }
    void add(Node<P>* const n, const Game<P>& g)
    {
        const std::uint64_t bt_hash = g.get_board_turn_hash();
        const auto t = g.get_turn();
        const auto s = g.get_stand(t).value();
        auto it = m_table.find(bt_hash);
        if (it == m_table.end()) {
            m_table.emplace(bt_hash, StandNodePairs());
            m_table[bt_hash].emplace_back(s, n);
        } else {
            for (auto&& pair : it->second) {
                if (pair.first == s)
                    return; // already added
            }
            it->second.emplace_back(s, n);
        }
    }
    void look_up(
        const Game<P>& g,
        const Node<P>** const node_ge,
        const Node<P>** const node_e,
        const Node<P>** const node_le) const
    {
        const std::uint64_t bt_hash = g.get_board_turn_hash();
        auto it = m_table.find(bt_hash);
        *node_ge = nullptr;
        *node_e = nullptr;
        *node_le = nullptr;
        if (it == m_table.end())
            return;
        return look_up(g, it->second, node_ge, node_e, node_le);
    }

private:
    void look_up(
        const Game<P>& g,
        const StandNodePairs& pairs,
        const Node<P>** const node_ge,
        const Node<P>** const node_e,
        const Node<P>** const node_le) const
    {
        const auto s = g.get_stand(g.get_turn());
        Stand<P> s_l{};
        Stand<P> s_g{static_cast<BaseTypeStand>(~0)};
        bool found_best_l = false;
        bool found_best_g = false;
        for (auto&& it : pairs) {
            const Stand<P> s_iter{it.first};
            const Node<P>* const n_iter = it.second;
            if (s_iter == s) {
                *node_e = n_iter;
            }
            if (n_iter->proved_by_repetitions() || (!n_iter->fully_expanded()))
                continue;
            if (!found_best_l && (s_iter <= s)) {
                if (n_iter->phi() == zero) {
                    *node_le = n_iter;
                    found_best_l = true;
                } else if ((*node_le == nullptr) || (s_l < s_iter)) {
                    s_l = s_iter;
                    *node_le = n_iter;
                }
            }
            if (!found_best_g && (s_iter >= s)) {
                if (n_iter->delta() == zero) {
                    *node_ge = n_iter;
                    found_best_g = true;
                } else if ((*node_ge == nullptr) || (s_iter < s_g)) {
                    s_g = s_iter;
                    *node_ge = n_iter;
                }
            }
        }
    }
};

template <class P>
class Searcher
{
private:
    std::vector<Node<P>> m_nodes; //!< The first one is the root node.
    Node<P>* m_next;
    Table<P> m_table;
    std::unique_ptr<Game<P>> m_game;
    uint m_search_count;
    uint m_remaining_searches;

public:
    void set_game(const Game<P>& g)
    {
        init();
        m_game = std::make_unique<Game<P>>(g);
        Game<P>& game = *m_game;
        if (!m_nodes[0].simulate(game)) {
            m_nodes[0].expand(m_next, game);
            m_table.add(&m_nodes[0], game);
            m_nodes[0].backprop(game.get_king_location(~game.get_turn()));
        }
    }
    Move<P> search(const uint n)
    {
        if (m_nodes[0].proved())
            return Move<P>();
        Game<P>& g = *m_game;
        m_remaining_searches = n;
        const auto out = multiple_iterative_deepning(m_nodes[0], g, inf, inf);
        m_search_count += n - m_remaining_searches;
        return out;
    }

private:
    Move<P> multiple_iterative_deepning(
        Node<P>& n, Game<P>& g, const uint th_p, const uint th_d)
    {
        Move<P> out{};
        assert(n.offence() || g.in_check());
        const Node<P>* twin_ge = nullptr;
        const Node<P>* twin_e = nullptr;
        const Node<P>* twin_le = nullptr;
        m_table.look_up(g, &twin_ge, &twin_e, &twin_le);
        if (n.simulate(g, twin_ge, twin_le)) {
            --m_remaining_searches;
            return n.get_action();
        }
        const auto king_sq
            = g.get_king_location(n.offence() ? ~g.get_turn() : g.get_turn());
        if (!n.has_child()) {
            n.expand(m_next, g, twin_ge, twin_le);
            if (twin_e == nullptr)
                m_table.add(&n, g);
            --m_remaining_searches;
            out = n.get_action();
            n.backprop(king_sq);
        }
        if (m_next == nullptr)
            return n.get_action();
        uint th_p_ch, th_d_ch;
        while (m_next && m_remaining_searches && (n.phi() < th_p)
               && (n.delta() < th_d)) {
            Node<P>* const child = n.select(th_p, th_d, th_p_ch, th_d_ch);
            g.apply_dfpn(child->get_action());
            out = multiple_iterative_deepning(*child, g, th_p_ch, th_d_ch);
            g.undo();
            n.backprop(king_sq);
        }
        return out;
    }

public: // utility
    Searcher(const uint num_nodes = 100000u)
        : m_nodes(num_nodes + 1u), m_next(nullptr), m_table{}, m_game(nullptr),
          m_search_count(0u), m_remaining_searches(0u)
    {
    }

    // Rules of 5
    ~Searcher() = default; // 1/5 destructor
    Searcher(const Searcher& other) = delete; // 2/5 copy constructor
    Searcher& operator=(const Searcher& other) = delete; // 3/5 copy assignment
    Searcher(Searcher&& other) = delete; // 4/5 move constructor
    Searcher& operator=(Searcher&& other) = delete; // 5/5 move assignment

    void init()
    {
        m_nodes[0].init();
        m_nodes[m_nodes.size() - 1u].init(false, Move<P>());
        m_next = std::next(m_nodes.data());
        m_table.clear();
        m_game.reset();
        m_search_count = 0u;
    }
    bool is_ready() const
    {
        return static_cast<bool>(m_game);
    }
    uint get_search_count() const
    {
        return m_search_count;
    }
    uint get_num_nodes_remain() const
    {
        if (m_next == nullptr)
            return 0u;
        return static_cast<uint>(
            static_cast<int>(m_nodes.size())
            - static_cast<int>(m_next - m_nodes.data()));
    }
    bool proved() const
    {
        return m_nodes[0].proved();
    }
    bool proved_mate() const
    {
        return m_nodes[0].proved_mate();
    }
    bool proved_no_mate() const
    {
        return m_nodes[0].proved_no_mate();
    }
    const Node<P>* get_root() const
    {
        return &m_nodes[0];
    }
    Move<P> get_mate_move() const
    {
        if (!m_nodes[0].proved_mate())
            return Move<P>();
        const auto c1 = m_nodes[0].get_child_1st();
        if (c1 == nullptr)
            return Move<P>();
        return c1->get_action();
    }
    std::vector<Move<P>> get_mate_moves() const
    {
        std::vector<Move<P>> out{};
        const Node<P>* const c1 = m_nodes[0].get_child_1st();
        if (c1 && c1->proved_mate())
            append_mate_moves(out, *m_game, c1);
        return out;
    }

private: // utility
    void append_mate_moves(
        std::vector<Move<P>>& out,
        Game<P>& game,
        const Node<P>* const node) const
    {
        const Move<P> action = node->get_action();
        game.apply_nocheck(action);
        out.emplace_back(action);
        const Node<P>* const ch1st = node->get_child_1st();
        if ((ch1st != nullptr) && ch1st->proved_mate()) {
            // The 1st child may not have mate value because
            // `search_inner()` can assign mate value on a node having children
            // with arbitrary #P and #D values by `m_table.look_up_fuzzy()`.
            append_mate_moves(out, game, ch1st);
        } else if (game.get_result() == ONGOING) {
            if (!node->offence()) { // applied action was offence move.
                const auto m = game.get_legal_moves()[0];
                game.apply_nocheck(m); // defence move
                out.emplace_back(m);
            }
            Searcher<P> searcher{static_cast<uint>(m_nodes.size() - 1u)};
            searcher.set_game(game);
            searcher.search(m_search_count);
            if (searcher.proved_mate()) {
                const Node<P>* const c1 = searcher.m_nodes[0].get_child_1st();
                if (c1 && c1->proved_mate())
                    searcher.append_mate_moves(out, game, c1);
            }
            if (!node->offence())
                game.undo();
        }
        game.undo();
    }
};

} // namespace vshogi::engine::dfpn3

#endif // VSHOGI_ENGINE_DFPN3_SEARCHER_HPP
