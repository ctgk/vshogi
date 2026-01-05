#ifndef VSHOGI_ENGINE_DFPN_TABLE_HPP
#define VSHOGI_ENGINE_DFPN_TABLE_HPP

#include <list>
#include <unordered_map>

#include "vshogi/common/game.hpp"
#include "vshogi/common/stand.hpp"
#include "vshogi/engine/dfpn/node.hpp"

namespace vshogi::engine::dfpn
{

template <class P>
class Table
{
    using C = Configuration<P>;
    using BaseTypeStand = typename C::BaseTypeStand;
    using StandNodePairs = std::list<std::pair<BaseTypeStand, Node*>>;

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
    void add(Node* const n, const Game<P>& g)
    {
        const std::uint64_t bt_hash = g.get_board_turn_hash();
        const auto s = g.get_stand().value();
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
        const Node** const node_ge,
        const Node** const node_e,
        const Node** const node_le) const
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
        const Node** const node_ge,
        const Node** const node_e,
        const Node** const node_le) const
    {
        const auto& s = g.get_stand();
        Stand<P> s_l{};
        Stand<P> s_g{static_cast<BaseTypeStand>(~0)};
        bool found_best_l = false;
        bool found_best_g = false;
        for (auto&& it : pairs) {
            const Stand<P> s_iter{it.first};
            const Node* const n_iter = it.second;
            if (s_iter == s) {
                *node_e = n_iter;
            }
            if (!found_best_l && (s_iter <= s)) {
                if ((n_iter->phi() == zero) && n_iter->fully_expanded()) {
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
                if ((n_iter->delta() == zero) && n_iter->fully_expanded()) {
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

} // namespace vshogi::engine::dfpn

#endif // VSHOGI_ENGINE_DFPN_TABLE_HPP
