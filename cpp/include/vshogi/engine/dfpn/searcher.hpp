#ifndef VSHOGI_ENGINE_DFPN_SEARCHER_HPP
#define VSHOGI_ENGINE_DFPN_SEARCHER_HPP

#include <cstdint>
#include <limits>
#include <list>
#include <memory>
#include <unordered_map>
#include <vector>

#include "vshogi/common/color.hpp"
#include "vshogi/common/game.hpp"
#include "vshogi/common/generator.hpp"
#include "vshogi/common/result.hpp"
#include "vshogi/common/utils.hpp"
#include "vshogi/engine/dfpn/node.hpp"

/**
 * @brief Namespace for DFPN algorithm.
 *
 * Depth-First Proof-Number algorithm.
 *
 * 1. Select a leaf node.
 *
 *      - Offence: argmin(#P of children).
 *
 *      - Defence: argmin(#D of children).
 *
 * 2. Simulate the game position.
 *
 *      - If checkmate proved, #P=zero,#D=inf
 *
 *      - If no mate proved, #P=inf,#D=zero
 *
 *      - otherwise, do nothing
 *
 * 3. Expand its children.
 *
 *      - Initialize all children by #P=unit,#D=unit
 *
 * 4. Back-propagation
 *
 *      - Offence: #P = min(#P of children), #D = sum(#D of children)
 *
 *      - Defence: #P = sum(#P of children), #D = min(#D of children)
 *
 * ref: https://qhapaq.hatenablog.com/entry/2020/07/18/201956
 */
namespace vshogi::engine::dfpn
{

template <class Parameters>
class TranspositionTable
{
private:
    using C = Configuration<Parameters>;
    using BaseTypeStand = typename C::BaseTypeStand;
    using StandNodeTable
        = std::list<std::pair<BaseTypeStand, const Node<Parameters>*>>;
    using StandType = Stand<Parameters>;
    using GameType = Game<Parameters>;
    using MoveType = Move<Parameters>;
    using NodeType = Node<Parameters>;

private:
    std::unordered_map<std::uint64_t, StandNodeTable> m_table;
    NodeType m_root;

public:
    TranspositionTable() : m_table{}, m_root{}
    {
    }
    void clear()
    {
        m_table.clear();
        m_root.init();
    }
    NodeType* get_root()
    {
        return &m_root;
    }
    const NodeType* get_root() const
    {
        return &m_root;
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
    void look_up_leg_stand_nodes(
        const GameType& g,
        const NodeType** const node_le,
        const NodeType** const node_e,
        const NodeType** const node_ge) const
    {
        const std::uint64_t bt_hash = g.get_board_turn_hash();
        auto it = m_table.find(bt_hash);
        *node_le = nullptr;
        *node_e = nullptr;
        *node_ge = nullptr;
        if (it == m_table.end())
            return;
        return look_up_leg_stand_nodes(g, it->second, node_le, node_e, node_ge);
    }

    void look_up_le_ge_stand(
        const GameType& g,
        const NodeType** const node_le_stand,
        const NodeType** const node_ge_stand) const
    {
        const std::uint64_t bt_hash = g.get_board_turn_hash();
        auto it = m_table.find(bt_hash);
        *node_le_stand = nullptr;
        *node_ge_stand = nullptr;
        if (it == m_table.end())
            return;
        return look_up_le_ge_stand(g, it->second, node_le_stand, node_ge_stand);
    }

    /**
     * @brief Prefer node with mate if offence turn else no-mate, and return it
     *
     * @param g
     * @return const NodeType*
     */
    const NodeType* look_up_le_stand(const GameType& g) const
    {
        const std::uint64_t bt_hash = g.get_board_turn_hash();
        auto it = m_table.find(bt_hash);
        if (it == m_table.end())
            return nullptr;
        return look_up_le_stand(g, it->second);
    }

private:
    void look_up_leg_stand_nodes(
        const GameType& g,
        const StandNodeTable& table,
        const NodeType** const node_le,
        const NodeType** const node_e,
        const NodeType** const node_ge) const
    {
        // - offence turn (`offence == true`)
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
            const bool offence = n_iter->offence();
            const bool is_mate = n_iter->proved_mate();
            const bool is_no_mate = n_iter->proved_no_mate();
            if (s_iter <= s) {
                if (offence ? is_mate : is_no_mate) {
                    // weaker offence stand, but mate
                    *node_le = n_iter;
                    found_best_l = true;
                } else if (
                    (!found_best_l) && (offence ? (!is_no_mate) : (!is_mate))) {
                    // exclude weaker offence stand, and no mate.
                    if ((*node_le == nullptr) || (s_l < s_iter)) {
                        s_l = s_iter;
                        *node_le = n_iter;
                    }
                }
            }
            if (s_iter == s) {
                *node_e = n_iter;
                if (n_iter->proved())
                    return;
            }
            if (s_iter >= s) {
                if (offence ? is_no_mate : is_mate) {
                    *node_ge = n_iter;
                    found_best_g = true;
                } else if (
                    (!found_best_g) && (offence ? (!is_mate) : (!is_no_mate))) {
                    // exclude greater offence stand, and mate.
                    if ((*node_ge == nullptr) || (s_iter < s_g)) {
                        s_g = s_iter;
                        *node_ge = n_iter;
                    }
                }
            }
        }
    }
    void look_up_le_ge_stand(
        const GameType& g,
        const StandNodeTable& table,
        const NodeType** const node_le_stand,
        const NodeType** const node_ge_stand) const
    {
        // - offence turn (`offence == true`)
        //     - Weaker offence stand, but mate (or #P <= #D)
        //     - Stronger offence stand, but no-mate (#P > #D).
        // - defence turn
        //     - Weaker defence stand, but no-mate.
        //     - Stronger defence stand, but mate.
        const auto t = g.get_turn();
        const auto s = g.get_stand(t);
        Stand<Parameters> s_le = Stand<Parameters>();
        Stand<Parameters> s_ge = Stand<Parameters>();
        bool found_best_le = false;
        bool found_best_ge = false;
        for (auto& it : table) {
            const auto s_iter = Stand<Parameters>(it.first);
            const NodeType* n_iter = it.second;
            const bool offence = n_iter->offence();
            const bool is_mate = n_iter->proved_mate();
            const bool is_no_mate = n_iter->proved_no_mate();
            if (s_iter <= s) {
                if (offence ? is_mate : is_no_mate) {
                    // weaker offence stand, but mate
                    *node_le_stand = n_iter;
                    found_best_le = true;
                } else if (
                    (!found_best_le)
                    && (offence ? (!is_no_mate) : (!is_mate))) {
                    // exclude weaker offence stand, and no mate.
                    if ((*node_le_stand == nullptr) || (s_le < s_iter)) {
                        s_le = s_iter;
                        *node_le_stand = n_iter;
                    }
                }
            }
            if (s <= s_iter) {
                if (offence ? is_no_mate : is_mate) {
                    *node_ge_stand = n_iter;
                    found_best_ge = true;
                } else if (
                    (!found_best_ge)
                    && (offence ? (!is_mate) : (!is_no_mate))) {
                    // exclude greater offence stand, and mate.
                    if ((*node_ge_stand == nullptr) || (s_iter < s_ge)) {
                        s_ge = s_iter;
                        *node_ge_stand = n_iter;
                    }
                }
            }
        }
    }
    const NodeType*
    look_up_le_stand(const GameType& g, const StandNodeTable& table) const
    {
        // - offence turn (`offence == true`)
        //     - Weaker offence stand, but mate (or #P <= #D)
        //     - Stronger offence stand, but no-mate (#P > #D).
        // - defence turn
        //     - Weaker defence stand, but no-mate.
        //     - Stronger defence stand, but mate.
        const auto t = g.get_turn();
        const auto s = g.get_stand(t);
        Stand<Parameters> s_out = Stand<Parameters>();
        const NodeType* n_out = nullptr;
        for (auto& it : table) {
            const auto s_iter = Stand<Parameters>(it.first);
            const NodeType* n_iter = it.second;
            const bool offence = n_iter->offence();
            const bool is_mate = n_iter->proved_mate();
            const bool is_no_mate = n_iter->proved_no_mate();
            if (s_iter <= s) {
                if (offence ? is_mate : is_no_mate) {
                    const NodeType* const ch1st = n_iter->get_child_1st();
                    if (ch1st && ch1st->proved())
                        return n_iter; // weaker offence stand, but mate
                    s_out = s_iter;
                    n_out = n_iter;
                } else if (offence ? (!is_no_mate) : (!is_mate)) {
                    // exclude weaker offence stand, and no mate.
                    if ((n_out == nullptr) || (!n_out->proved())
                        || (s_out < s_iter)) {
                        s_out = s_iter;
                        n_out = n_iter;
                    }
                }
            }
        }
        return n_out;
    }
};

template <class Parameters>
class Searcher
{
private:
    using GameType = Game<Parameters>;
    using MoveType = Move<Parameters>;
    using NodeType = Node<Parameters>;
    using PHelper = Pieces<Parameters>;

private:
    TranspositionTable<Parameters> m_table;
    std::unique_ptr<GameType> m_game;
    uint m_num_searched;

public:
    Searcher() : m_table(), m_game(nullptr), m_num_searched(0u)
    {
    }

    bool is_ready() const
    {
        return static_cast<bool>(m_game);
    }
    void set_game(const GameType& g)
    {
        m_table.clear();
        m_game = std::make_unique<GameType>(g);
        const GameType& game = *m_game;
        Node<Parameters>* const root = m_table.get_root();
        if (!root->simulate(game)) {
            root->expand(game, nullptr, nullptr);
            m_table.add(root, game);
        }
        m_num_searched = 0u;
    }

    /**
     * @brief Search for mate moves at given game state.
     *
     * @param n Number of nodes to explore.
     * @return true Proved checkmate.
     * @return false Checkmate not proved.
     */
    Move<Parameters> search(const uint n)
    {
        MoveType out{};
        Node<Parameters>* const root = m_table.get_root();
        GameType& game = *m_game;
        uint num = n;
        uint thpn_ch = 0u, thdn_ch = 0u;
        while (num) {
            if (root->proved())
                break;
            NodeType* const c1 = root->select(inf, inf, thpn_ch, thdn_ch);
            out = search_inner(*c1, game, num, thpn_ch, thdn_ch);
            root->backprop();
        }
        m_num_searched += n - num;
        return out;
    }
    bool proved_mate() const
    {
        return m_table.get_root()->proved_mate();
    }
    bool proved_no_mate() const
    {
        return m_table.get_root()->proved_no_mate();
    }
    bool proved() const
    {
        return m_table.get_root()->proved();
    }
    uint get_search_count() const
    {
        return m_num_searched;
    }
    MoveType get_mate_move() const
    {
        return m_table.get_root()->get_child_1st()->get_action();
    }
    std::vector<MoveType> get_mate_moves() const
    {
        std::vector<MoveType> out{};
        append_mate_moves(out, *m_game, m_table.get_root()->get_child_1st());
        return out;
    }
    const Node<Parameters>* get_root() const
    {
        return m_table.get_root();
    }

private:
    MoveType search_inner(
        Node<Parameters>& n,
        GameType& game,
        uint& searches,
        const uint thpn,
        const uint thdn)
    {
        MoveType out{};
        game.apply_dfpn(n.get_action());
        assert(n.offence() || game.in_check());
        simulate_or_expand(n, game, searches);
        if (searches == 0u)
            out = n.get_action();
        uint thpn_ch = 0u, thdn_ch = 0u;
        while (searches) {
            if ((n.pn() >= thpn) || (n.dn() >= thdn))
                break;
            NodeType* const c1 = n.select(thpn, thdn, thpn_ch, thdn_ch);
            out = search_inner(*c1, game, searches, thpn_ch, thdn_ch);
            n.backprop();
        }
        game.undo();
        return out;
    }
    void simulate_or_expand(Node<Parameters>& n, GameType& game, uint& searches)
    {
        const Node<Parameters>* node_le = nullptr;
        const Node<Parameters>* node_e = nullptr;
        const Node<Parameters>* node_ge = nullptr;
        m_table.look_up_leg_stand_nodes(game, &node_le, &node_e, &node_ge);
        if (!n.has_child())
            game.update_result_dfpn(1u, node_e != nullptr);
        if (n.simulate(game, node_le, node_e, node_ge)) {
            --searches;
        } else if (!n.has_child()) {
            const bool fully_expanded = n.expand(game, node_ge, node_le);
            if (fully_expanded && (node_e != &n))
                m_table.add(&n, game);
            --searches;
        }
    }
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
            // with arbitrary #P and #D values by `m_table.look_up_fuzzy()`.
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
        const Node<Parameters>* n = m_table.look_up_le_stand(game);
        if (n == nullptr) {
            assert(game.in_check()); // assert defence turn
            return *LegalMoveGenerator<Parameters>(game.get_state());
        }

        const bool offence = n->offence();
        assert((!offence) || n->proved_mate());
        for (n = n->get_child(); n; n = n->get_sibling()) {
            if (offence && (!n->proved_mate()))
                continue;
            const MoveType action = n->get_action();
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

} // namespace vshogi::engine::dfpn

#endif // VSHOGI_ENGINE_DFPN_SEARCHER_HPP
