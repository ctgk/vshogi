#ifndef VSHOGI_ENGINE_DFPN_SEARCHER_HPP
#define VSHOGI_ENGINE_DFPN_SEARCHER_HPP

#include <list>
#include <memory>
#include <unordered_map>

#include "vshogi/common/game.hpp"
#include "vshogi/common/move.hpp"
#include "vshogi/engine/dfpn/node.hpp"

namespace vshogi::engine::dfpn
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
                if (pair.first == s) {
                    pair.second = n;
                    return;
                }
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
            if (!found_best_l && (s_iter <= s)) {
                if ((n_iter->phi() == zero) && n_iter->fully_expanded()
                    && (!n_iter->proved_by_repetitions())) {
                    *node_le = n_iter;
                    found_best_l = true;
                } else if (
                    (*node_le == nullptr)
                    || ((*node_le)->fully_expanded() < n_iter->fully_expanded())
                    || (((*node_le)->fully_expanded()
                         == n_iter->fully_expanded())
                        && (s_l < s_iter))) {
                    s_l = s_iter;
                    *node_le = n_iter;
                }
            }
            if (!found_best_g && (s_iter >= s)) {
                if ((n_iter->delta() == zero) && n_iter->fully_expanded()
                    && (!n_iter->proved_by_repetitions())) {
                    *node_ge = n_iter;
                    found_best_g = true;
                } else if (
                    (*node_ge == nullptr)
                    || ((*node_ge)->fully_expanded() < n_iter->fully_expanded())
                    || (((*node_ge)->fully_expanded()
                         == n_iter->fully_expanded())
                        && (s_iter < s_g))) {
                    s_g = s_iter;
                    *node_ge = n_iter;
                }
            }
        }
    }
};

template <class P>
class ScopedGame
{
private:
    Game<P>& m_game;
    std::vector<ZobristHashType> m_hash_list{};
    std::vector<std::uint32_t> m_captured_move_list{};

public:
    ScopedGame(Game<P>& g) : m_game(g)
    {
        m_hash_list.reserve(32u);
        m_captured_move_list.reserve(32u);
        m_game.swap_log(m_hash_list, m_captured_move_list);
    }
    ~ScopedGame()
    {
        m_game.swap_log(m_hash_list, m_captured_move_list);
    }
};

template <class P>
class Searcher
{
    using C = Configuration<P>;

private:
    std::vector<Node<P>> m_nodes; //!< The first one is the root node.
    Node<P>* m_next;
    Table<P> m_table;
    uint m_search_count;
    uint m_remaining_searches;

public:
    Move<P> search(Game<P>& g, const uint n)
    {
        ScopedGame scope{g};
        if (m_search_count == 0u) {
            m_next = std::next(m_nodes.data());
            if (!m_nodes[0].simulate(g)) {
                m_nodes[0].expand(m_next, g);
                m_table.add(&m_nodes[0], g);
                m_nodes[0].backprop(C::SQ_NA);
            }
        }
        if (m_nodes[0].proved()) {
            return Move<P>();
        }
        m_remaining_searches = n;
        const auto out = multiple_iterative_deepening(m_nodes[0], g, inf, inf);
        m_search_count += n - m_remaining_searches;
        return out;
    }

private:
    Move<P> multiple_iterative_deepening(
        Node<P>& n, Game<P>& g, const uint th_p, const uint th_d)
    {
        const bool offence = (g.ply() % 2u == 0u);
        Move<P> out = n.get_action();
        assert(offence || g.in_check());
        const Node<P>*twin_ge{}, *twin_e{}, *twin_le{};
        m_table.look_up(g, &twin_ge, &twin_e, &twin_le);
        if (n.simulate(g, twin_ge, twin_le)) {
            --m_remaining_searches;
            return out;
        }
        const auto checker_sq = offence ? C::SQ_NA : g.get_checker_location();
        if (!n.has_child()) {
            n.expand(m_next, g, twin_ge, twin_le);
            if ((twin_e == nullptr) || twin_e->proved_by_repetitions()
                || (!twin_e->fully_expanded()))
                m_table.add(&n, g);
            --m_remaining_searches;
            n.backprop(checker_sq);
        }
        if (m_next == nullptr)
            return out;
        while (m_next && m_remaining_searches && (n.phi() < th_p)
               && (n.delta() < th_d)) {
            uint th_p_ch, th_d_ch;
            Node<P>* const child = n.select(th_p, th_d, th_p_ch, th_d_ch);
            g.apply_dfpn(child->get_action());
            out = multiple_iterative_deepening(*child, g, th_p_ch, th_d_ch);
            g.undo();
            n.backprop(checker_sq);
        }
        return out;
    }

public: // utility
    Searcher(const uint num_nodes = 100000u)
        : m_nodes(num_nodes + 1u), m_next(nullptr), m_table{},
          m_search_count(0u), m_remaining_searches(0u)
    {
        init();
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
        m_nodes[m_nodes.size() - 1u].init(Move<P>(C::SQ_NA, C::SQ_NA));
        m_next = nullptr;
        m_table.clear();
        m_search_count = 0u;
    }
    uint get_search_count() const
    {
        return m_search_count;
    }
    uint get_num_nodes_remain() const
    {
        if (m_next == nullptr)
            return 0u;
        return static_cast<uint>(m_nodes.size())
               - static_cast<uint>(m_next - m_nodes.data());
    }
    bool proved() const
    {
        return m_nodes[0].proved();
    }
    bool proved_mate() const
    {
        return m_nodes[0].proved_mate(true);
    }
    bool proved_no_mate() const
    {
        return m_nodes[0].proved_no_mate(true);
    }
    const Node<P>* get_root() const
    {
        return &m_nodes[0];
    }
    Move<P> get_mate_move() const
    {
        if (!m_nodes[0].proved_mate(true))
            return Move<P>();
        const auto c1 = m_nodes[0].get_child_1st();
        if (c1 == nullptr)
            return Move<P>();
        return c1->get_action();
    }
    std::vector<Move<P>> get_mate_moves(Game<P>& game) const
    {
        ScopedGame scope{game};
        follow_line(game, m_nodes[0].get_child_1st());
        std::vector<Move<P>> out{};
        if (game.ply() > 0u) {
            for (uint ii = 0u; ii < game.ply(); ++ii)
                out.emplace_back(game.get_record_action(ii));
            for (uint ii = game.ply(); ii--;)
                game.undo();
        }
        assert(game.ply() == 0u);
        return out;
    }

private: // utility
    bool follow_line(Game<P>& game, const Node<P>* node) const
    {
        if ((node == nullptr) || (!game.is_legal(node->get_action())))
            return false;
        game.apply_nocheck(node->get_action());
        if ((game.count_repetitions() > 1u) || (game.ply() > 31u)) {
            game.undo();
            return false;
        }
        const bool offence = (game.ply() % 2u == 0u);
        if (is_mate_end(game))
            return true;
        if (!node->proved_mate(offence))
            node = lookup_in_table(game);
        if (node) {
            const Node<P>* c = node->get_child_1st();
            if (follow_line(game, c))
                return true;
            for (c = node->get_child(); c; c = c->get_sibling()) {
                if (c == node->get_child_1st())
                    continue;
                if (follow_line(game, c))
                    return true;
            }
        }
        game.undo();
        return false;
    }
    static bool is_mate_end(const Game<P>& game)
    {
        const bool offence = (game.ply() % 2u == 0u);
        const auto r = game.get_result();
        if ((r == ONGOING) || (r == DRAW))
            return false;
        const auto turn = game.get_turn();
        const auto winner = (r == BLACK_WIN) ? BLACK : WHITE;
        return offence ? (turn == winner) : (turn != winner);
    }
    const Node<P>* lookup_in_table(Game<P>& game) const
    {
        const bool offence = (game.ply() % 2u == 0u);
        const Node<P>*node_ge{}, *node_e{}, *node_le{};
        m_table.look_up(game, &node_ge, &node_e, &node_le);
        const Node<P>* const node = (offence) ? node_le : node_ge;
        if (node && node->proved_mate(offence))
            return node;
        return nullptr;
    }
};

} // namespace vshogi::engine::dfpn

#endif // VSHOGI_ENGINE_DFPN_SEARCHER_HPP
