#ifndef VSHOGI_ENGINE_DFPN_HPP
#define VSHOGI_ENGINE_DFPN_HPP

#include <cstdint>
#include <limits>
#include <memory>
#include <vector>

#include "vshogi/common/color.hpp"
#include "vshogi/common/result.hpp"
#include "vshogi/common/utils.hpp"
#include "vshogi/variants/animal_shogi.hpp"

namespace vshogi::engine::dfpn
{

template <class Game, class Move, bool Attacker = true>
class Node
{
    static_assert(!std::is_same<Game, animal_shogi::Game>::value);

private:
    using NodeAlly = Node<Game, Move, Attacker>;
    using NodeEnemy = Node<Game, Move, !Attacker>;

    std::unique_ptr<Game> m_game;

    NodeEnemy* m_parent;

    /**
     * @brief Pointer to sibling node.
     * @ref https://blog.mozilla.org/nnethercote/2012/03/07/n-ary-trees-in-c/
     */
    std::unique_ptr<NodeAlly> m_sibling;

    /**
     * @brief Pointer to first child node.
     * @ref https://blog.mozilla.org/nnethercote/2012/03/07/n-ary-trees-in-c/
     */
    std::unique_ptr<NodeEnemy> m_child;

    Move m_action;
    uint m_pn;
    uint m_dn;

    friend class Node<Game, Move, !Attacker>;

public:
    static constexpr uint zero = 0u;
    static constexpr uint unit = 100u;
    static constexpr uint cent = 1u;
    static constexpr uint max_number = std::numeric_limits<uint>::max();

    Node(const Game& g)
        : m_game(std::make_unique<Game>(Game(g))), m_parent(nullptr),
          m_sibling(nullptr), m_child(nullptr), m_action(), m_pn(unit),
          m_dn(unit)
    {
    }
    Node(const Move& action, const bool& is_dst_attacked_by_non_king = false)
        : m_game(nullptr), m_parent(nullptr), m_sibling(nullptr),
          m_child(nullptr), m_action(action), m_pn(unit), m_dn(unit)
    {
        if constexpr (!Attacker) {
            if (is_dst_attacked_by_non_king)
                m_pn += cent;
        }
    }

    // Rules of 5
    ~Node() = default; // 1/5 destructor
    Node(const Node<Game, Move, Attacker>& other)
        = default; // 2/5 copy constructor
    Node<Game, Move, Attacker>&
    operator=(const Node<Game, Move, Attacker>& other)
        = default; // 3/5 copy assignment
    Node(Node<Game, Move, Attacker>&& other) = default; // 4/5 move constructor
    Node<Game, Move, Attacker>& operator=(Node<Game, Move, Attacker>&& other)
        = default; // 5/5 move assignment

    uint get_pn() const
    {
        return m_pn;
    }
    uint get_dn() const
    {
        return m_dn;
    }
    uint get_num_child() const
    {
        const NodeEnemy* ch = m_child.get();
        uint out = 0U;
        while (true) {
            if (ch == nullptr)
                break;
            ++out;
            ch = ch->m_sibling.get();
        }
        return out;
    }
    std::vector<Move> get_mate_moves() const
    {
        std::vector<Move> out;
        if (!found_mate())
            return out;

        const NodeAlly* node = this;
        while (true) {
            if (node->m_child == nullptr) {
                // Length of mate moves can be even number
                // by king entering declaration.
                return out;
            }
            const NodeEnemy* node_enemy = node->select();
            out.emplace_back(node_enemy->m_action); // Attack move
            if (node_enemy->m_child == nullptr) {
                return out;
            }
            node = node_enemy->select();
            out.emplace_back(node->m_action); // Defence move
        }
    }
    bool found_mate() const
    {
        return (m_pn == zero) && (m_dn == max_number);
    }
    bool found_no_mate() const
    {
        return (m_pn == max_number) && (m_dn == zero);
    }
    bool found_conclusion() const
    {
        return (found_mate() || found_no_mate());
    }

    void select_simulate_expand_backprop()
    {
        if (m_child == nullptr) {
            // pn and dn should not be infinity when there are no children.
            simulate_expand_backprop();
        } else {
            NodeEnemy* child = select();
            child->m_parent = this;
            if (child->m_game == nullptr) {
                if (Attacker)
                    child->m_game = std::make_unique<Game>(
                        m_game->copy_and_apply_nocheck(child->m_action));
                else
                    child->m_game = std::make_unique<Game>(
                        m_game->copy_and_apply_dfpn_defence(child->m_action));
            }
            child->select_simulate_expand_backprop();
        }
    }

private:
    NodeEnemy* select()
    {
        if (Attacker) {
            uint min_pn = max_number;
            NodeEnemy* out = m_child.get();
            for (NodeEnemy* ch = out; ch != nullptr; ch = ch->m_sibling.get()) {
                if (min_pn > ch->m_pn) {
                    min_pn = ch->m_pn;
                    out = ch;
                }
            }
            return out;
        } else {
            uint min_dn = max_number;
            NodeEnemy* out = m_child.get();
            for (NodeEnemy* ch = out; ch != nullptr; ch = ch->m_sibling.get()) {
                if (min_dn > ch->m_dn) {
                    min_dn = ch->m_dn;
                    out = ch;
                }
            }
            return out;
        }
    }
    const NodeEnemy* select() const
    {
        if (Attacker) {
            uint min_pn = max_number;
            const NodeEnemy* out = m_child.get();
            for (auto ch = out; ch != nullptr; ch = ch->m_sibling.get()) {
                if (min_pn > ch->m_pn) {
                    min_pn = ch->m_pn;
                    out = ch;
                }
            }
            return out;
        } else {
            uint min_dn = max_number;
            const NodeEnemy* out = m_child.get();
            for (auto ch = out; ch != nullptr; ch = ch->m_sibling.get()) {
                if (min_dn > ch->m_dn) {
                    min_dn = ch->m_dn;
                    out = ch;
                }
            }
            return out;
        }
    }
    void simulate_expand_backprop()
    {
        const uint pn_original = m_pn;
        const uint dn_original = m_dn;
        const Game& game = *m_game;
        const auto r = game.get_result();
        if (r == ONGOING) {
            expand(game);
        } else {
            simulate(game);
        }
        if (m_parent != nullptr)
            m_parent->backprop(m_pn, m_dn, pn_original, dn_original);
    }
    void simulate(const Game& game)
    {
        const auto r = game.get_result();
        if (r == DRAW) {
            set_pndn_no_mate();
        } else {
            const auto winner = (r == BLACK_WIN) ? BLACK : WHITE;
            const auto turn = game.get_turn();
            if ((winner == turn) == Attacker)
                set_pndn_mate();
            else
                set_pndn_no_mate();
        }
    }
    void expand(const Game& game)
    {
        const std::vector<Move>& legal_moves = game.get_legal_moves();
        std::unique_ptr<NodeEnemy>* ch = &m_child;
        uint num_child = 0u;
        if constexpr (Attacker) {
            m_pn = max_number;
            m_dn = zero;
            int cache[Game::num_squares()] = {0}; //!< 0:?, 1:false, 2:true
            for (auto&& m : legal_moves) {
                if ((m_parent == nullptr)
                    && (!game.template is_check_move<false>(m)))
                    continue;
                const auto dst = m.destination();
                if (cache[dst] == 0) {
                    const auto defence = ~game.get_turn();
                    const auto& enemy_king_sq = game.get_king_location(defence);
                    const auto is_attacked
                        = game.get_board().is_square_attacked(
                            defence, dst, enemy_king_sq);
                    *ch = std::make_unique<NodeEnemy>(m, is_attacked);
                    cache[dst] = static_cast<int>(is_attacked) + 1;
                } else {
                    *ch = std::make_unique<NodeEnemy>(
                        m, static_cast<bool>(cache[dst] - 1));
                }
                m_pn = std::min(m_pn, ch->get()->m_pn);
                m_dn += ch->get()->m_dn;
                ch = &ch->get()->m_sibling;
                ++num_child;
            }
        } else {
            for (auto&& m : legal_moves) {
                *ch = std::make_unique<NodeEnemy>(m);
                ch = &ch->get()->m_sibling;
                ++num_child;
            }
            // defence node have at least one child unless checkmate
            m_pn = unit * num_child;
            m_dn = unit;
        }
    }
    void backprop(
        const uint& pn,
        const uint& dn,
        const uint& pn_prev,
        const uint& dn_prev)
    {
        const uint pn_original = m_pn;
        const uint dn_original = m_dn;
        if (Attacker) {
            m_pn = max_number;
            const NodeEnemy* ch = m_child.get();
            for (; ch != nullptr; ch = ch->m_sibling.get()) {
                if (m_pn > ch->m_pn)
                    m_pn = ch->m_pn;
            }
            if ((m_dn == max_number) || (dn == max_number)) {
                m_dn = max_number;
            } else {
                m_dn = m_dn + dn - dn_prev;
            }
        } else {
            if ((m_pn == max_number) || (pn == max_number)) {
                m_pn = max_number;
            } else {
                m_pn = m_pn + pn - pn_prev;
            }
            m_dn = max_number;
            const NodeEnemy* ch = m_child.get();
            for (; ch != nullptr; ch = ch->m_sibling.get()) {
                if (m_dn > ch->m_dn)
                    m_dn = ch->m_dn;
            }
        }
        if (found_no_mate()) {
            m_child.reset();
        }
        if (m_parent != nullptr)
            m_parent->backprop(m_pn, m_dn, pn_original, dn_original);
    }
    void set_pndn_mate()
    {
        m_pn = zero;
        m_dn = max_number;
    }
    void set_pndn_no_mate()
    {
        m_pn = max_number;
        m_dn = zero;
    }
};

template <class Game, class Move>
class Searcher
{
private:
    Node<Game, Move, true> m_root;
    const uint m_default_num_nodes;

public:
    Searcher(const Game& g, const uint& default_num_nodes = 1000)
        : m_root(g), m_default_num_nodes(default_num_nodes)
    {
    }

    /**
     * @brief Explore mate moves at given game state.
     *
     * @return true Found mate moves.
     * @return false No mate moves found which may be found by further explorations.
     */
    bool explore()
    {
        return explore(m_default_num_nodes);
    }

    /**
     * @brief Explore mate moves at given game state.
     *
     * @param n Number of nodes to explore.
     * @return true Found mate moves.
     * @return false No mate moves found which may be found by further explorations.
     */
    bool explore(uint n)
    {
        for (; n--;) {
            if (m_root.found_conclusion())
                break;
            m_root.select_simulate_expand_backprop();
        }
        return m_root.found_mate();
    }
    uint get_num_child() const
    {
        return m_root.get_num_child();
    }
    bool found_mate() const
    {
        return m_root.found_mate();
    }
    bool found_no_mate() const
    {
        return m_root.found_no_mate();
    }
    bool found_conclusion() const
    {
        return m_root.found_conclusion();
    }
    std::vector<Move> get_mate_moves()
    {
        return m_root.get_mate_moves();
    }
};

} // namespace vshogi::engine::dfpn

#endif // VSHOGI_ENGINE_DFPN_HPP
