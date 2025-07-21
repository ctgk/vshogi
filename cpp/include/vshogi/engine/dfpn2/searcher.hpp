#ifndef VSHOGI_ENGINE_DFPN2_SEARCHER_HPP
#define VSHOGI_ENGINE_DFPN2_SEARCHER_HPP

#include <map>
#include <memory>

#include "vshogi/common/game.hpp"
#include "vshogi/common/move.hpp"
#include "vshogi/common/pieces.hpp"
#include "vshogi/engine/dfpn2/node.hpp"

namespace vshogi::engine::dfpn2
{

template <class Parameters>
class Table
{
private:
    using C = Configuration<Parameters>;
    using BaseTypeStand = typename C::BaseTypeStand;
    using StandNodeTable
        = std::vector<std::pair<BaseTypeStand, const Node<Parameters>*>>;
    using StandType = Stand<Parameters>;
    using GameType = Game<Parameters>;
    using MoveType = Move<Parameters>;
    using NodeType = Node<Parameters>;

private:
    std::map<std::uint64_t, StandNodeTable> m_table;

public:
    Table() : m_table{}
    {
    }
    void clear()
    {
        m_table.clear();
    }

    void add(const NodeType* const n, const GameType& g)
    {
        const std::uint64_t bt_hash = g.get_board_turn_hash();
        const auto t = g.get_turn();
        const auto s = g.get_stand(t).value();
        auto it = m_table.find(bt_hash);
        if (it == m_table.end()) {
            m_table.emplace(bt_hash, StandNodeTable());
            m_table[bt_hash].emplace_back(s, n);
        } else {
            for (auto&& pair : it->second) {
                if (pair.first == s)
                    return;
            }
            it->second.emplace_back(s, n);
        }
    }
    const Node<Parameters>* look_up_le_stand_node(const GameType& g) const
    {
        const std::uint64_t bt_hash = g.get_board_turn_hash();
        auto it = m_table.find(bt_hash);
        if (it == m_table.end())
            return nullptr;
        return look_up_le_stand_node(g, it->second);
    }
    void look_up_leg_stand_nodes(
        const GameType& g,
        const NodeType** const node_l,
        const NodeType** const node_e,
        const NodeType** const node_g) const
    {
        const std::uint64_t bt_hash = g.get_board_turn_hash();
        auto it = m_table.find(bt_hash);
        *node_l = nullptr;
        *node_e = nullptr;
        *node_g = nullptr;
        if (it == m_table.end())
            return;
        return look_up_leg_stand_nodes(g, it->second, node_l, node_e, node_g);
    }

private:
    const NodeType*
    look_up_le_stand_node(const GameType& g, const StandNodeTable& table) const
    {
        // - offence turn (`is_attacker == true`)
        //     - Weaker offence stand, but mate (or #P <= #D)
        //     - Stronger offence stand, but no-mate (#P > #D).
        // - defence turn
        //     - Weaker defence stand, but no-mate.
        //     - Stronger defence stand, but mate.
        const auto t = g.get_turn();
        const auto s = g.get_stand(t);
        Stand<Parameters> s_le = Stand<Parameters>();
        const NodeType* out = nullptr;
        for (auto& it : table) {
            const auto s_iter = Stand<Parameters>(it.first);
            const NodeType* const n_iter = it.second;
            const bool is_atk = n_iter->offence();
            const bool is_mate = n_iter->proved_mate();
            const bool is_no_mate = n_iter->proved_no_mate();
            const NodeType* const c1 = n_iter->get_child_1st();
            if ((c1 == nullptr) || (!c1->proved()))
                continue;
            if (s_iter <= s) {
                if (is_atk ? is_mate : is_no_mate) {
                    // weaker offence stand, but mate
                    return n_iter;
                } else if (is_atk ? (!is_no_mate) : (!is_mate)) {
                    // exclude weaker offence stand, and no mate.
                    if ((out == nullptr) || (s_le < s_iter)) {
                        s_le = s_iter;
                        out = n_iter;
                    }
                }
            }
        }
        return out;
    }
    void look_up_leg_stand_nodes(
        const GameType& g,
        const StandNodeTable& table,
        const NodeType** const node_l,
        const NodeType** const node_e,
        const NodeType** const node_g) const
    {
        // - offence turn (`is_attacker == true`)
        //     - Weaker offence stand, but mate (or #P <= #D)
        //     - Stronger offence stand, but no-mate (#P > #D).
        // - defence turn
        //     - Weaker defence stand, but no-mate.
        //     - Stronger defence stand, but mate.
        const auto t = g.get_turn();
        const auto s = g.get_stand(t);
        Stand<Parameters> s_l = Stand<Parameters>();
        Stand<Parameters> s_g
            = Stand<Parameters>(static_cast<BaseTypeStand>(~0));
        bool found_best_l = false;
        bool found_best_g = false;
        for (auto& it : table) {
            const auto s_iter = Stand<Parameters>(it.first);
            const NodeType* const n_iter = it.second;
            const bool is_atk = n_iter->offence();
            const bool is_mate = n_iter->proved_mate();
            const bool is_no_mate = n_iter->proved_no_mate();
            if (s_iter < s) {
                if (is_atk ? is_mate : is_no_mate) {
                    // weaker offence stand, but mate
                    *node_l = n_iter;
                    found_best_l = true;
                } else if (
                    (!found_best_l) && (is_atk ? (!is_no_mate) : (!is_mate))) {
                    // exclude weaker offence stand, and no mate.
                    if ((*node_l == nullptr) || (s_l < s_iter)) {
                        s_l = s_iter;
                        *node_l = n_iter;
                    }
                }
            } else if (s_iter == s) {
                *node_e = n_iter;
                if (n_iter->proved())
                    return;
            } else if (s_iter > s) {
                if (is_atk ? is_no_mate : is_mate) {
                    *node_g = n_iter;
                    found_best_g = true;
                } else if (
                    (!found_best_g) && (is_atk ? (!is_mate) : (!is_no_mate))) {
                    // exclude greater offence stand, and mate.
                    if ((*node_g == nullptr) || (s_iter < s_g)) {
                        s_g = s_iter;
                        *node_g = n_iter;
                    }
                }
            }
        }
    }
};

template <class Parameters>
class Searcher
{
private:
    using C = Configuration<Parameters>;
    using GameType = Game<Parameters>;
    using MoveType = Move<Parameters>;
    using NodeType = Node<Parameters>;
    using PHelper = Pieces<Parameters>;

private:
    NodeType m_root;
    Table<Parameters> m_table;
    std::unique_ptr<GameType> m_game;
    uint m_num_searched;

public:
    void set_game(const GameType& g)
    {
        m_root.init();
        m_table.clear();
        m_game = std::make_unique<GameType>(g);
        // const GameType& game = *m_game;
        // NodeType* const root = m_table.get_root();
        // if (!root->simulate(game))
        //     root->expand_children(game);
        m_num_searched = 0u;
    }

    /**
     * @brief Search for mate moves at given game state.
     *
     * @param n Number of nodes to explore.
     * @return true Proved mate.
     * @return false No proved mate found.
     */
    bool search(const uint n)
    {
        GameType& game = *m_game;
        uint num = n;
        search_inner(game, m_root, num, inf, inf);
        m_num_searched += n - num;
        return m_root.proved_mate();
    }

private:
    void search_inner(
        GameType& game,
        NodeType& n,
        uint& searches,
        const uint thpn,
        const uint thdn)
    {
        assert(n.offence() || game.in_check());
        const NodeType* node_l = nullptr; // offence->mate, defence->no-mate
        const NodeType* node_e = nullptr;
        const NodeType* node_g = nullptr; // offence->no-mate, defence->mate
        m_table.look_up_leg_stand_nodes(game, &node_l, &node_e, &node_g);

        if (n.is_first_arrival())
            game.update_result_dfpn(1u);
        if (n.simulate(game, node_l, node_e, node_g)) {
            --searches;
            return;
        }
        if (n.num_children() == 0u) {
            const bool fully_expanded
                = n.expand(game, node_l, node_e ? node_e : node_g);
            if (fully_expanded && (node_e == nullptr))
                m_table.add(&n, game);
            --searches;
        }
        while (searches) {
            n.backprop(game);
            // std::cout << "#P=" << n.pn() << ", #D=" << n.dn() << " at "
            //           << game.to_sfen() << std::endl;
            if ((n.pn() >= thpn) || (n.dn() >= thdn))
                break;
            const uint thpn_ch = n.compute_child_thpn(thpn);
            const uint thdn_ch = n.compute_child_thdn(thdn);
            NodeType* const c1 = n.select();
            game.apply_dfpn(c1->get_action());
            search_inner(game, *c1, searches, thpn_ch, thdn_ch);
            game.undo();
        }
    }

public: // utility
    Searcher() : m_root(), m_table(), m_game(nullptr), m_num_searched(0u)
    {
    }
    bool is_ready() const
    {
        return static_cast<bool>(m_game);
    }
    bool proved_mate() const
    {
        return m_root.proved_mate();
    }
    bool proved_no_mate() const
    {
        return m_root.proved_no_mate();
    }
    bool proved() const
    {
        return m_root.proved();
    }
    uint get_search_count() const
    {
        return m_num_searched;
    }
    MoveType get_mate_move() const
    {
        if (!m_root.proved_mate())
            return MoveType();
        const auto c1 = m_root.get_child_1st();
        if (c1 == nullptr)
            return MoveType();
        return c1->get_action();
    }
    std::vector<MoveType> get_mate_moves() const
    {
        std::vector<MoveType> out{};
        const NodeType* const c1 = m_root.get_child_1st();
        if (c1 && c1->proved_mate())
            append_mate_moves(out, *m_game, c1);
        return out;
    }
    const Node<Parameters>* get_root() const
    {
        return &m_root;
    }

private:
    void append_mate_moves(
        std::vector<MoveType>& out,
        GameType& game,
        const Node<Parameters>* const node) const
    {
        const MoveType action = node->get_action();
        game.apply_nocheck(action);
        out.emplace_back(action);
        const Node<Parameters>* const ch1st = node->get_child_1st();
        if ((ch1st != nullptr) && ch1st->proved_mate()) {
            // The 1st child may not have mate value because
            // `search_inner()` can assign mate value on a node having children
            // with arbitrary #P and #D values by `m_table.look_up_...()`.
            append_mate_moves(out, game, ch1st);
        } else if (game.get_result() == ONGOING) {
            append_mate_moves(out, game);
        }
        game.undo();
    }
    void append_mate_moves(std::vector<MoveType>& out, GameType& game) const
    {
        const MoveType action = find_action_from_transposition_table(game);
        if (action.hash() == 0u)
            return;
        game.apply_nocheck(action);
        out.emplace_back(action);
        if (game.get_result() == ONGOING)
            append_mate_moves(out, game);
        game.undo();
    }
    MoveType find_action_from_transposition_table(const GameType& game) const
    {
        const ColorEnum t = game.get_turn();
        const Stand<Parameters>& stand = game.get_stand(t);
        const Node<Parameters>* n = m_table.look_up_le_stand_node(game);
        if (n == nullptr) {
            assert(game.in_check()); // assert defence turn
            return *LegalMoveGenerator<Parameters>(game.get_state());
        }

        const bool is_atk = n->offence();
        assert((!is_atk) || n->proved_mate());
        for (auto& ch : n->get_children()) {
            if (is_atk && (!ch.proved_mate()))
                continue;
            const MoveType action = ch.get_action();
            if (!action.is_drop()) // legal for sure
                return action;
            const auto pt = action.source_piece();
            if (stand.exist(pt))
                return action;
        }
        assert(false);
        return MoveType();
    }
};

} // namespace vshogi::engine::dfpn2

#endif // VSHOGI_ENGINE_DFPN2_SEARCHER_HPP
