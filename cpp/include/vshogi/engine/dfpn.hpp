#ifndef VSHOGI_ENGINE_DFPN_HPP
#define VSHOGI_ENGINE_DFPN_HPP

#include <cstdint>
#include <limits>
#include <memory>
#include <vector>

#include "vshogi/color.hpp"
#include "vshogi/result.hpp"

namespace vshogi::engine::dfpn
{

template <class Game, class Move, bool Attacker = true>
class Node
{
private:
    Node<Game, Move, !Attacker>* m_parent;
    Move m_action;
    std::uint32_t m_pn;
    std::uint32_t m_dn;
    std::vector<Node<Game, Move, !Attacker>> m_children;
    static constexpr std::uint32_t max_number
        = std::numeric_limits<std::uint32_t>::max();
    friend class Node<Game, Move, !Attacker>;

public:
    Node() : m_parent(nullptr), m_action(0), m_pn(1), m_dn(1), m_children()
    {
    }
    Node(const Move& action)
        : m_parent(nullptr), m_action(action), m_pn(1), m_dn(1), m_children()
    {
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

    std::uint32_t get_pn() const
    {
        return m_pn;
    }
    std::uint32_t get_dn() const
    {
        return m_dn;
    }
    std::vector<Move> get_mate_moves() const
    {
        std::vector<Move> out;
        if (!found_mate())
            return out;

        const Node<Game, Move, Attacker>* node = this;
        while (true) {
            const Node<Game, Move, !Attacker>* n1 = node->select();
            out.emplace_back(n1->m_action); // Attack move
            if (n1->m_children.empty()) {
                return out;
            }
            node = n1->select();
            out.emplace_back(node->m_action); // Defence move
        }
    }
    bool found_mate() const
    {
        return (m_pn == 0) && (m_dn == max_number);
    }
    bool found_no_mate() const
    {
        return (m_pn == max_number) && (m_dn == 0);
    }
    bool found_conclusion() const
    {
        return (found_mate() || found_no_mate());
    }

    void select_simulate_expand_backprop(Game& game)
    {
        if (m_children.empty()) {
            // pn and dn should not be infinity when there are no children.
            simulate_expand_backprop(game);
        } else {
            Node<Game, Move, !Attacker>* child = select();
            child->m_parent = this;
            child->select_simulate_expand_backprop(game.apply(child->m_action));
        }
    }

private:
    Node<Game, Move, !Attacker>* select()
    {
        if (Attacker) {
            std::uint32_t min_pn = max_number;
            Node<Game, Move, !Attacker>* out = &m_children[0];
            for (auto&& ch : m_children) {
                if (min_pn > ch.m_pn) {
                    min_pn = ch.m_pn;
                    out = &ch;
                }
            }
            return out;
        } else {
            std::uint32_t min_dn = max_number;
            Node<Game, Move, !Attacker>* out = &m_children[0];
            for (auto&& ch : m_children) {
                if (min_dn > ch.m_dn) {
                    min_dn = ch.m_dn;
                    out = &ch;
                }
            }
            return out;
        }
    }
    const Node<Game, Move, !Attacker>* select() const
    {
        if (Attacker) {
            std::uint32_t min_pn = max_number;
            const Node<Game, Move, !Attacker>* out = &m_children[0];
            for (auto&& ch : m_children) {
                if (min_pn > ch.m_pn) {
                    min_pn = ch.m_pn;
                    out = &ch;
                }
            }
            return out;
        } else {
            std::uint32_t min_dn = max_number;
            const Node<Game, Move, !Attacker>* out = &m_children[0];
            for (auto&& ch : m_children) {
                if (min_dn > ch.m_dn) {
                    min_dn = ch.m_dn;
                    out = &ch;
                }
            }
            return out;
        }
    }
    void simulate_expand_backprop(const Game& game)
    {
        const std::uint32_t pn_original = m_pn;
        const std::uint32_t dn_original = m_dn;
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
        for (auto&& m : legal_moves) {
            if ((!Attacker) || Game(game).apply(m).in_check()) {
                m_children.emplace_back(Node<Game, Move, !Attacker>(m));
            }
        }
        if (m_children.empty())
            set_pndn_no_mate();
        else {
            if (Attacker) {
                m_pn = 1;
                m_dn = static_cast<std::uint32_t>(m_children.size());
            } else {
                m_pn = static_cast<std::uint32_t>(m_children.size());
                m_dn = 1;
            }
        }
    }
    void backprop(
        const std::uint32_t& pn,
        const std::uint32_t& dn,
        const std::uint32_t& pn_prev,
        const std::uint32_t& dn_prev)
    {
        const std::uint32_t pn_original = m_pn;
        const std::uint32_t dn_original = m_dn;
        if (Attacker) {
            m_pn = max_number;
            for (auto&& child : m_children) {
                if (m_pn > child.m_pn)
                    m_pn = child.m_pn;
            }
            if ((m_dn == max_number) || (dn == max_number)) {
                m_dn = max_number;
            } else {
                m_dn = static_cast<std::uint32_t>(
                    static_cast<int>(m_dn) + static_cast<int>(dn)
                    - static_cast<int>(dn_prev));
            }
        } else {
            if ((m_pn == max_number) || (pn == max_number)) {
                m_pn = max_number;
            } else {
                m_pn = static_cast<std::uint32_t>(
                    static_cast<int>(m_pn) + static_cast<int>(pn)
                    - static_cast<int>(pn_prev));
            }
            m_dn = max_number;
            for (auto&& child : m_children) {
                if (m_dn > child.m_dn)
                    m_dn = child.m_dn;
            }
        }
        if (m_parent != nullptr)
            m_parent->backprop(m_pn, m_dn, pn_original, dn_original);
    }
    void set_pndn_mate()
    {
        m_pn = 0;
        m_dn = max_number;
    }
    void set_pndn_no_mate()
    {
        m_pn = max_number;
        m_dn = 0;
    }
};

template <class Game, class Move>
class Searcher
{
private:
    const int m_default_num_nodes;
    Node<Game, Move, true> m_root;
    std::unique_ptr<Game> m_game_ptr;

public:
    Searcher(const int default_num_nodes = 1000)
        : m_default_num_nodes(default_num_nodes), m_root(), m_game_ptr(nullptr)
    {
    }
    void set_root(const Game& g)
    {
        m_root = Node<Game, Move, true>();
        m_game_ptr = std::make_unique<Game>(Game(g));
    }
    bool explore()
    {
        return explore(m_default_num_nodes);
    }
    bool explore(std::size_t n)
    {
        const Game& g = *m_game_ptr;
        for (; n--;) {
            if (m_root.found_conclusion())
                break;
            auto g_tmp = Game(g);
            m_root.select_simulate_expand_backprop(g_tmp);
        }
        return m_root.found_mate();
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
