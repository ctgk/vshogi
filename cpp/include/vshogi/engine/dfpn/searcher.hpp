#ifndef VSHOGI_ENGINE_DFPN_SEARCHER_HPP
#define VSHOGI_ENGINE_DFPN_SEARCHER_HPP

#include <memory>

#include "vshogi/common/game.hpp"
#include "vshogi/common/move.hpp"
#include "vshogi/common/pieces.hpp"
#include "vshogi/engine/dfpn/node.hpp"

namespace vshogi::engine::dfpn2
{

template <class Parameters, class Table>
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
    Table m_table;
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
        m_table.lookup_leg_stand_nodes(game, &node_l, &node_e, &node_g);

        if (n.is_first_arrival())
            game.update_result_dfpn(1u);
        if (n.simulate(game, node_l, node_e, node_g)) {
            --searches;
            return;
        }
        if (n.num_children() == 0u) {
            const bool fully_expanded
                = n.expand_children(game, node_l, node_e ? node_e : node_g);
            if (fully_expanded && (node_e == nullptr))
                m_table.add(n);
            --searches;
        }
        while (searches) {
            n.backprop(game);
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
    static uint compute_thpn_child(const uint th, const NodeType* const c2)
    {
        const uint n2 = c2 ? c2->pn() : inf;
        return std::min(th, (n2 != inf) ? n2 + 1u : inf);
    }

public: // utility
    Searcher() : m_table(), m_game(nullptr), m_num_searched(0u)
    {
    }
    bool is_ready() const
    {
        return static_cast<bool>(m_game);
    }
    bool found_mate() const
    {
        return m_table.get_root()->found_mate();
    }
    bool found_no_mate() const
    {
        return m_table.get_root()->found_no_mate();
    }
    bool found_conclusion() const
    {
        return m_table.get_root()->found_conclusion();
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
    void simulate_or_expand(
        Node<Parameters>& n, const GameType& game, uint& searches)
    {
        const Node<Parameters>* node_le = nullptr;
        const Node<Parameters>* node_ge = nullptr;
        m_table.look_up_le_ge_stand(game, &node_le, &node_ge);
        if (node_le && node_le->found_conclusion()) {
            n.m_pn = node_le->pn();
            n.m_dn = node_le->dn();
            --searches;
        } else if (node_ge && node_ge->found_conclusion()) {
            n.m_pn = node_ge->pn();
            n.m_dn = node_ge->dn();
            --searches;
        } else if (!n.has_child()) {
            if (!n.simulate(game) && n.expand(game, node_ge, node_le)
                && (node_le != &n))
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
        if ((ch1st != nullptr) && ch1st->found_mate()) {
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

        const bool is_atk = n->is_attacker();
        assert((!is_atk) || n->found_mate());
        for (n = n->get_child(); n; n = n->get_sibling()) {
            if (is_atk && (!n->found_mate()))
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

} // namespace vshogi::engine::dfpn2

#endif // VSHOGI_ENGINE_DFPN_SEARCHER_HPP
