#ifndef VSHOGI_ENGINE_DFPN_HPP
#define VSHOGI_ENGINE_DFPN_HPP

#include <cstdint>
#include <limits>
#include <memory>
#include <unordered_map>
#include <vector>

#include "vshogi/common/color.hpp"
#include "vshogi/common/result.hpp"
#include "vshogi/common/utils.hpp"
#include "vshogi/variants/animal_shogi.hpp"

/**
 * @brief Namespace for DFPN algorithm.
 *
 * Depth-First Proof-Number algorithm.
 *
 * 1. Select a leaf node.
 *      - Offence: argmin(#P of children).
 *      - Defence: argmin(#D of children).
 * 2. Simulate the game position.
 *      - If checkmate proved, #P=zero,#D=inf
 *      - If no mate proved, #P=inf,#D=zero
 *      - otherwise, none
 * 3. Expand its children.
 *      - Initialize all children by #P=unit,#D=unit
 * 4. Back-propagation
 *      - Offence: #P = min(#P of children), #D = sum(#D of children)
 *      - Defence: #P = sum(#P of children), #D = min(#D of children)
 */
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

    /**
     * @brief Number of searches required to prove there is a mate.
     *
     */
    uint m_pn;

    /**
     * @brief Number of searches required to prove there is no mate.
     *
     */
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
        m_game->clear_records_for_dfpn();
        simulate_expand_backprop();
    }
    Node(const Game& g, std::unordered_map<std::uint64_t, bool>& mate_cache)
        : m_game(std::make_unique<Game>(Game(g))), m_parent(nullptr),
          m_sibling(nullptr), m_child(nullptr), m_action(), m_pn(unit),
          m_dn(unit)
    {
        m_game->clear_records_for_dfpn();
        simulate_expand_backprop(mate_cache);
    }
    Node(const Move& action)
        : m_game(nullptr), m_parent(nullptr), m_sibling(nullptr),
          m_child(nullptr), m_action(action), m_pn(unit), m_dn(unit)
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

    ColorEnum get_turn() const
    {
        return m_game->get_turn();
    }
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
            NodeEnemy* const child = select();
            child->select_simulate_expand_backprop();
        }
    }
    void select_simulate_expand_backprop(
        std::unordered_map<std::uint64_t, bool>& mate_cache)
    {
        if (m_child == nullptr) {
            simulate_expand_backprop(mate_cache);
        } else {
            NodeEnemy* const child = select();
            child->select_simulate_expand_backprop(mate_cache);
        }
    }

private:
    void simulate_expand_backprop()
    {
        const Game& game = *m_game;
        const auto r = game.get_result();
        if (r == ONGOING) {
            expand(game);
        } else {
            simulate(game);
        }
        backprop();
    }
    void simulate_expand_backprop(
        std::unordered_map<std::uint64_t, bool>& mate_cache)
    {
        const Game& game = *m_game;
        const auto r = game.get_result();
        if (r == ONGOING) {
            expand(game, mate_cache);
        } else {
            simulate(game);
        }
        backprop(mate_cache);
    }

    /**
     * @brief Select a child node.
     *
     * - Attacker: select a node with smallest proof number.
     * - Defence: select a node with smallest dis-proof number.
     *
     * @return NodeEnemy*
     */
    NodeEnemy* select()
    {
        NodeEnemy* out = m_child.get();
        if (Attacker) {
            uint min_pn = max_number;
            for (NodeEnemy* ch = out; ch != nullptr; ch = ch->m_sibling.get()) {
                if (min_pn > ch->m_pn) {
                    min_pn = ch->m_pn;
                    out = ch;
                }
            }
        } else {
            uint min_dn = max_number;
            for (NodeEnemy* ch = out; ch != nullptr; ch = ch->m_sibling.get()) {
                if (min_dn > ch->m_dn) {
                    min_dn = ch->m_dn;
                    out = ch;
                }
            }
        }

        out->m_parent = this;
        if (out->m_game == nullptr) {
            if (Attacker)
                out->m_game = std::make_unique<Game>(
                    m_game->copy_and_apply_dfpn_offence(out->m_action));
            else
                out->m_game = std::make_unique<Game>(
                    m_game->copy_and_apply_dfpn_defence(out->m_action));
        }
        return out;
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

    /**
     * @brief Simulate the current game position.
     *
     * - {winner: BLACK, turn: BLACK, node: Attacker}
     * - Typical checkmate (winner!=turn && node!=Attacker): `set_pndn_mate`
     * - King entering by opponent (winner==turn && node!=Attacker) `set_pndn_no_mate`
     *
     * @param game
     */
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

    /**
     * @brief Expand child nodes and edit #P(Proof) and #D(Disproof).
     *
     * - Attacker: #P = min(#P of children), #D = sum(#D of children)
     * - Defence: #P = sum(#P of children), #D = min(#D of children)
     *
     * @param game
     */
    void expand(const Game& game)
    {
        const std::vector<Move>& legal_moves = game.get_legal_moves();
        std::unique_ptr<NodeEnemy>* ch = &m_child;
        if constexpr (Attacker) {
            /**
             * @brief 0:?, 1:false, 2:true
             */
            uint is_attacked_cache[Game::num_squares()] = {0};

            for (auto&& m : legal_moves) {
                if ((m_parent == nullptr)
                    && (!game.template is_check_move<false>(m)))
                    continue;
                *ch = std::make_unique<NodeEnemy>(m);
                modify_pndn_by_attacks(ch->get(), game, m, is_attacked_cache);
                ch = &ch->get()->m_sibling;
            }
        } else {
            for (auto&& m : legal_moves) {
                *ch = std::make_unique<NodeEnemy>(m);
                modify_pndn_by_defence(ch->get(), game, m);
                ch = &ch->get()->m_sibling;
            }
        }
    }
    void modify_pndn_by_attacks(
        NodeEnemy* const ch,
        const Game& g,
        const Move m,
        uint* const is_attacked_cache)
    {
        const auto dst = m.destination();
        if (is_attacked_cache[dst] == 0) {
            const auto defence = ~g.get_turn();
            const auto& enemy_king_sq = g.get_king_location(defence);
            const auto is_attacked
                = g.get_board().is_square_attacked(defence, dst, enemy_king_sq);
            is_attacked_cache[dst] = static_cast<uint>(is_attacked) + 1u;
        }
        ch->m_pn += cent * (is_attacked_cache[dst] - 1);
    }
    void
    modify_pndn_by_defence(NodeEnemy* const ch, const Game& g, const Move& m)
    {
        const auto& checker_sq = g.get_checker_location();
        if (m.destination() == checker_sq)
            ch->m_dn -= cent; // defence prefers capturing checker piece.
    }
    void modify_pndn_by_mate(
        NodeEnemy* const ch,
        const Game& g,
        const Move m,
        std::unordered_map<std::uint64_t, bool>& mate_cache)
    {
        const std::uint64_t zobrist_hash = g.get_zobrist_hash();
        const std::uint64_t hash
            = zobrist_hash ^ static_cast<std::uint64_t>(m.hash());
        if (auto it = mate_cache.find(hash); it != mate_cache.end()) {
            if (it->second) {
                ch->m_pn = cent;
                ch->m_dn = 10000 * unit;
            } else {
                ch->m_pn = 10000 * unit;
                ch->m_dn = cent;
            }
        }
    }
    void modify_pndn_if_parent_is_almost_mate(NodeEnemy* const ch)
    {
        if (m_dn > (1000 * unit)) { // if the self node is almost-mate
            ch->m_pn = cent;
            ch->m_dn = 10000 * unit;
        }
    }

    void expand(
        const Game& game, std::unordered_map<std::uint64_t, bool>& mate_cache)
    {
        const std::vector<Move>& legal_moves = game.get_legal_moves();
        std::unique_ptr<NodeEnemy>* ch = &m_child;
        if constexpr (Attacker) {
            /**
             * @brief 0:?, 1:false, 2:true
             */
            uint is_attacked_cache[Game::num_squares()] = {0};

            for (auto&& m : legal_moves) {
                if ((m_parent == nullptr)
                    && (!game.template is_check_move<false>(m)))
                    continue;
                *ch = std::make_unique<NodeEnemy>(m);
                modify_pndn_by_attacks(ch->get(), game, m, is_attacked_cache);
                modify_pndn_by_mate(ch->get(), game, m, mate_cache);
                ch = &ch->get()->m_sibling;
            }
        } else {
            for (auto&& m : legal_moves) {
                *ch = std::make_unique<NodeEnemy>(m);
                modify_pndn_by_defence(ch->get(), game, m);
                modify_pndn_if_parent_is_almost_mate(ch->get());
                ch = &ch->get()->m_sibling;
            }
        }
    }

    /**
     * @brief Backprop #P and #D from child.
     *
     * - Attacker: #P = min(#P of children), #D = sum(#D of children)
     * - Defence: #P = sum(#P of children), #D = min(#D of children)
     *
     */
    void backprop()
    {
        if (!found_conclusion()) {
            if (Attacker) {
                m_pn = max_number;
                m_dn = zero;
                const NodeEnemy* ch = m_child.get();
                for (; ch != nullptr; ch = ch->m_sibling.get()) {
                    if (m_pn > ch->m_pn)
                        m_pn = ch->m_pn;

                    if ((m_dn == max_number) || (ch->m_dn == max_number))
                        m_dn = max_number;
                    else
                        m_dn += ch->m_dn;
                }
            } else {
                m_dn = max_number;
                m_pn = zero;
                const NodeEnemy* ch = m_child.get();
                for (; ch != nullptr; ch = ch->m_sibling.get()) {
                    if (m_dn > ch->m_dn)
                        m_dn = ch->m_dn;

                    if ((m_pn == max_number) || (ch->m_pn == max_number))
                        m_pn = max_number;
                    else
                        m_pn += ch->m_pn;
                }
            }
        }
        if (found_no_mate()) {
            m_child.reset();
        }
        if (m_parent != nullptr)
            m_parent->backprop();
    }
    void backprop(std::unordered_map<std::uint64_t, bool>& mate_cache)
    {
        if (!found_conclusion()) {
            if (Attacker) {
                m_pn = max_number;
                m_dn = zero;
                const NodeEnemy* ch = m_child.get();
                for (; ch != nullptr; ch = ch->m_sibling.get()) {
                    if (m_pn > ch->m_pn)
                        m_pn = ch->m_pn;

                    if ((m_dn == max_number) || (ch->m_dn == max_number))
                        m_dn = max_number;
                    else
                        m_dn += ch->m_dn;
                }
            } else {
                m_dn = max_number;
                m_pn = zero;
                const NodeEnemy* ch = m_child.get();
                for (; ch != nullptr; ch = ch->m_sibling.get()) {
                    if (m_dn > ch->m_dn)
                        m_dn = ch->m_dn;

                    if ((m_pn == max_number) || (ch->m_pn == max_number))
                        m_pn = max_number;
                    else
                        m_pn += ch->m_pn;
                }
            }
        }
        if (found_conclusion()) {
            if ((!Attacker) && (m_parent != nullptr)) { // m_parent is attacker
                const std::uint64_t hash
                    = m_parent->m_game->get_zobrist_hash()
                      ^ static_cast<std::uint64_t>(m_action.hash());
                mate_cache[hash] = found_mate();
            }
            if (found_no_mate())
                m_child.reset();
        }
        if (m_parent != nullptr)
            m_parent->backprop(mate_cache);
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
    std::unique_ptr<Node<Game, Move, true>> m_root;

    /**
     * @brief True if there is a mate, false if there is no mate for sure.
     *
     */
    std::unordered_map<std::uint64_t, bool> m_mate_cache_for_black;

    /**
     * @brief True if there is a mate, false if there is no mate for sure.
     *
     */
    std::unordered_map<std::uint64_t, bool> m_mate_cache_for_white;

public:
    Searcher()
        : m_root(nullptr), m_mate_cache_for_black{}, m_mate_cache_for_white{}
    {
    }

    bool is_ready() const
    {
        return (m_root != nullptr);
    }
    void set_game(const Game& g)
    {
        auto& cache = (g.get_turn() == vshogi::BLACK) ? m_mate_cache_for_black
                                                      : m_mate_cache_for_white;
        m_root = std::make_unique<Node<Game, Move, true>>(g, cache);
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
        Node<Game, Move, true>* const root = m_root.get();
        auto& cache = (root->get_turn() == vshogi::BLACK)
                          ? m_mate_cache_for_black
                          : m_mate_cache_for_white;
        for (; n--;) {
            if (root->found_conclusion())
                break;
            root->select_simulate_expand_backprop(cache);
        }
        return root->found_mate();
    }
    uint get_num_child() const
    {
        return m_root->get_num_child();
    }
    bool found_mate() const
    {
        return m_root->found_mate();
    }
    bool found_no_mate() const
    {
        return m_root->found_no_mate();
    }
    bool found_conclusion() const
    {
        return m_root->found_conclusion();
    }
    std::vector<Move> get_mate_moves()
    {
        return m_root->get_mate_moves();
    }
};

} // namespace vshogi::engine::dfpn

#endif // VSHOGI_ENGINE_DFPN_HPP
