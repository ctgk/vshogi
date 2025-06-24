#ifndef VSHOGI_ENGINE_DFPN_TABLE_HPP
#define VSHOGI_ENGINE_DFPN_TABLE_HPP

#include <map>
#include <vector>

#include "vshogi/common/config.hpp"
#include "vshogi/common/utils.hpp"
#include "vshogi/engine/dfpn/node.hpp"

namespace vshogi::engine::dfpn
{
template <class Parameters>
class Table
{
private:
    using C = Configuration<Parameters>;
    using BaseTypeStand = typename C::BaseTypeStand;
    using NodeType = Node<Parameters>;
    using GameType = Game<Parameters>;

    /**
     * Using `std::list` instead of `std::vector` to avoid pointer-related bugs
     * that can be caused by `std::vector`’s internal reallocation.
     */
    using StandNodePairs = std::list<std::pair<BaseTypeStand, NodeType>>;

    std::map<std::uint64_t, StandNodePairs> m_table;
    ColorEnum m_offence;

public:
    Node<Parameters>& get_or_expand(const GameType& g)
    {
        const std::uint64_t bt_hash = g.get_board_turn_hash();
        const auto t = g.get_turn();
        const auto s = g.get_stand(t).value();
        auto it = m_table.find(bt_hash);
        if (it == m_table.end()) {
            it = m_table.emplace(bt_hash, StandNodePairs()).first;
        } else {
            for (auto&& pair : it->second) {
                if (pair.first == s)
                    return pair.second;
            }
        }
        it->second.emplace_back(
            std::piecewise_construct, std::make_tuple(s), std::make_tuple());
        return it->second.back().second;
    }
    void lookup_lgstand(
        const GameType& g,
        const Node<Parameters>** const node_l,
        const Node<Parameters>** const node_g) const
    {
        const std::uint64_t bt_hash = g.get_board_turn_hash();
        auto it = m_table.find(bt_hash);
        *node_l = nullptr;
        *node_g = nullptr;
        if (it == m_table.end())
            return;
        lookup_lgstand(g, it->second, node_l, node_g);
    }

private:
    void lookup_lgstand(
        const GameType& g,
        const StandNodePairs& table,
        const NodeType** const node_l,
        const NodeType** const node_g) const
    {
        const bool is_atk = (g.get_turn() == m_offence);
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
            = Stand<Parameters>(~static_cast<typename C::BaseTypeStand>(0));
        bool found_best_l = false;
        bool found_best_g = false;
        for (auto&& it : table) {
            const auto s_iter = Stand<Parameters>(it.first);
            const NodeType* n_iter = &it.second;
            const bool is_mate = n_iter->found_mate(is_atk);
            const bool is_no_mate = n_iter->found_no_mate(is_atk);
            if (s_iter < s) {
                if (is_atk ? is_mate : is_no_mate) {
                    *node_l = n_iter;
                    found_best_l = true;
                } else if ((!found_best_l) && (s_l <= s_iter)) {
                    s_l = s_iter;
                    *node_l = n_iter;
                }
            } else if (s < s_iter) {
                if (is_atk ? is_no_mate : is_mate) {
                    *node_g = n_iter;
                    found_best_g = true;
                } else if ((!found_best_g) && (s_iter <= s_g)) {
                    s_g = s_iter;
                    *node_g = n_iter;
                }
            }
        }
    }

public: // utility
    Table(const ColorEnum& offence) : m_table{}, m_offence(offence)
    {
    }
    ColorEnum offence() const
    {
        return m_offence;
    }
    void set_offence(const ColorEnum& offence)
    {
        m_offence = offence;
    }
    void clear()
    {
        m_table.clear();
    }
};

} // namespace vshogi::engine::dfpn

#endif // VSHOGI_ENGINE_DFPN_TABLE_HPP
