#ifndef VSHOGI_STATE_HPP
#define VSHOGI_STATE_HPP

#include <random>
#include <string>
#include <vector>

#include "vshogi/common/bitboard_traits.hpp"
#include "vshogi/common/board.hpp"
#include "vshogi/common/color.hpp"
#include "vshogi/common/direction.hpp"
#include "vshogi/common/move.hpp"
#include "vshogi/common/piece_traits.hpp"
#include "vshogi/common/square_traits.hpp"
#include "vshogi/common/stand.hpp"

namespace vshogi
{

template <class P>
class State
{
private:
    using C = Configuration<P>;
    using PieceType = typename C::PieceType;
    using Piece = typename C::Piece;
    using Rank = typename C::Rank;
    using Square = typename C::Square;
    using BT = BitboardTraits<P>;
    using PT = PieceTraits<P>;
    using ST = SquareTraits<P>;
    using MT = MoveTraits<P>;
    using Stands = BlackWhiteStands<P>;
    using bitboard_t = typename C::bitboard_t;
    static constexpr uint num_piece_types = C::num_piece_types;
    static constexpr uint num_stand_piece_types = C::num_stand_piece_types;
    static constexpr uint num_dir = C::num_dir;
    static constexpr Piece VOID = C::VOID; // NOLINT
    static constexpr std::uint64_t zobrist_hash_for_turn = 0x000000aaaaaaaaaau;

public:
    static constexpr uint num_ranks = C::num_ranks;
    static constexpr uint num_files = C::num_files;
    static constexpr uint num_squares = C::num_squares;

private:
    Board<P> m_board;
    Stands m_stands;
    ColorEnum m_turn; //!< Player to make a move in the current state.

    /**
     * @brief Direction of checkers from turn player's king.
     */
    DirectionEnum m_checkers[2];

public:
    State() : m_board(), m_stands(), m_turn(BLACK), m_checkers{DIR_NA, DIR_NA}
    {
    }
    State(const std::string& sfen) : m_board(), m_stands(), m_turn()
    {
        set_sfen(sfen);
        update_checkers();
    }
    static constexpr uint feature_channels()
    {
        return 2 * (num_piece_types + num_stand_piece_types);
    }
    static constexpr uint num_dlshogi_policy()
    {
        return num_squares * (2 * C::num_dir_dl + C::num_stand_piece_types);
    }
    bool operator==(const State& other) const
    {
        return (m_board == other.m_board) && (m_stands == other.m_stands)
               && (m_turn == other.m_turn);
    }
    bool operator!=(const State& other) const
    {
        return (m_board != other.m_board) || (m_stands != other.m_stands)
               || (m_turn != other.m_turn);
    }
    const Board<P>& get_board() const
    {
        return m_board;
    }
    const Stand<P>& get_stand() const
    {
        return get_stand(m_turn);
    }
    const Stand<P>& get_stand(const ColorEnum c) const
    {
        return m_stands[c];
    }
    const Stands& get_stands() const
    {
        return m_stands;
    }
    ColorEnum get_turn() const
    {
        return m_turn;
    }
    Square get_king_square() const
    {
        return get_king_square(m_turn);
    }
    Square get_king_square(const ColorEnum c) const
    {
        return m_board.get_king_square(c);
    }
    DirectionEnum get_checker_dir(const uint index = 0u) const
    {
        return m_checkers[index];
    }
    Square find_checker_square(const uint index = 0u) const
    {
        auto ptr_sq = ST::ray_from(get_king_square(), m_checkers[index]);
        assert(ptr_sq != nullptr);
        for (; *ptr_sq != C::SQ_NA; ++ptr_sq) {
            if (!m_board.is_empty(*ptr_sq))
                break;
        }
        assert((m_checkers[index] == DIR_NA) || (*ptr_sq != C::SQ_NA));
        return *ptr_sq;
    }
    bool in_check() const
    {
        return m_checkers[0] != DIR_NA;
    }
    bool in_double_check() const
    {
        return m_checkers[1] != DIR_NA;
    }
    bool can_apply_drop_move(const PieceType pt = C::NA) const
    {
        if (in_double_check())
            return false;
        if ((pt != C::NA) && (!m_stands[m_turn].exist(pt)))
            return false;
        if (!in_check())
            return true;
        return m_board.is_empty(ST::shift(get_king_square(), m_checkers[0]));
    }
    bool in_promotion_zone(const move_t& m) const
    {
        if (ST::in_promotion_zone(MT::get_dst(m), m_turn))
            return true;
        if (MT::is_drop(m))
            return false;
        return ST::in_promotion_zone(MT::get_src_sq(m), m_turn);
    }
    void set_sfen(const std::string& sfen)
    {
        auto s = sfen.c_str();
        s = m_board.set_sfen(s);
        {
            (*s == 'b') ? m_turn = BLACK : m_turn = WHITE;
            s += 2;
        }
        m_stands.set_sfen(s);
    }
    State hflip() const
    {
        return State(m_board.hflip(), m_stands, m_turn);
    }
    State rotate() const
    {
        return State(m_board.rotate(), m_stands.rotate(), ~m_turn);
    }
    State& apply(const move_t& move, std::uint64_t* const hash = nullptr)
    {
        const Square dst = MT::get_dst(move);
        if (MT::is_drop(move)) {
            const PieceType src = MT::get_src_pt(move);
            const Piece p = m_stands.pop_piece_from(m_turn, src, hash);
            m_board.place_at(dst, p, hash);
            update_checkers_before_turn_update(dst);
        } else {
            const Square src = MT::get_src_sq(move);
            auto moving_piece = m_board.pop_from(src, hash);
            if (MT::get_promote(move))
                moving_piece = PT::promote_nocheck(moving_piece);
            const auto captured = m_board.place_at(dst, moving_piece, hash);
            m_stands.add_captured_piece(captured, hash);
            update_checkers_before_turn_update(dst, src);
        }
        m_turn = ~m_turn;
        if (hash != nullptr)
            *hash ^= zobrist_hash_for_turn;
        return *this;
    }
    State& undo(
        const move_t& move,
        const Piece& captured,
        const DirectionEnum& checker_dir_0,
        const DirectionEnum& checker_dir_1)
    {
        const Square dst = MT::get_dst(move);
        if (captured != VOID)
            m_stands.remove_captured_piece(captured);
        if (MT::is_drop(move)) {
            assert(captured == C::VOID);
            const auto dropped = m_board.pop_from(dst);
            m_stands.return_dropped_piece(dropped);
        } else {
            const auto src = MT::get_src_sq(move);
            auto moved = m_board.place_at(dst, captured);
            if (MT::get_promote(move))
                moved = PT::demote_nocheck(moved);
            m_board.place_at(src, moved);
        }
        m_turn = ~m_turn;
        m_checkers[0] = checker_dir_0;
        m_checkers[1] = checker_dir_1;
        return *this;
    }
    State&
    apply_discard(const move_t& move, std::uint64_t* const hash = nullptr);
    State& undo_discard(
        const move_t& move,
        const Piece& captured,
        const DirectionEnum& checker_dir_0,
        const DirectionEnum& checker_dir_1);
    void to_feature_map(float* const data) const
    {
        constexpr uint sp_types = num_stand_piece_types;
        constexpr uint ch_half = sp_types + num_piece_types;
        constexpr uint ch = ch_half * 2;

        const auto& stand_curr = m_stands[m_turn];
        const auto& stand_next = m_stands[~m_turn];

        float num_pieces_curr[sp_types] = {};
        float num_pieces_next[sp_types] = {};
        for (auto pt : C::stand_piece_type_iterator()) {
            num_pieces_curr[pt] = static_cast<float>(stand_curr.count(pt));
            num_pieces_next[pt] = static_cast<float>(stand_next.count(pt));
        }

        std::fill_n(data, num_squares * ch, 0.f);
        for (uint i = num_squares; i--;) {
            float* const data_ch = data + i * ch;
            for (uint k = sp_types; k--;)
                data_ch[k + ch_half] = num_pieces_next[k];
            for (uint k = sp_types; k--;)
                data_ch[k] = num_pieces_curr[k];

            const auto sq = static_cast<Square>(
                (m_turn == BLACK) ? i : (num_squares - 1 - i));
            if (m_board.is_empty(sq))
                continue;
            const auto& p = m_board[sq];
            const auto pt = PT::to_piece_type(p);
            const auto k = static_cast<uint>(pt)
                           + (m_turn != PT::get_color(p)) * ch_half;
            data_ch[k + sp_types] = 1.f;
        }
    }
    std::uint64_t zobrist_hash(const bool& hash_stands = true) const
    {
        auto out = m_board.zobrist_hash();
        if (hash_stands)
            out ^= m_stands.get_zobrist_hash();
        if (m_turn == WHITE)
            out ^= zobrist_hash_for_turn;
        return out;
    }
    std::uint64_t hash_stands() const
    {
        return m_stands.get_zobrist_hash();
    }
    bitboard_t find_pinned() const
    {
        if (in_double_check())
            return static_cast<bitboard_t>(0);
        return m_board.find_pinned(m_turn);
    }
    bool is_declined_promotion(const move_t& move) const
    {
        if (MT::is_drop(move))
            return false;
        if (MT::get_promote(move))
            return false;
        if (!in_promotion_zone(move))
            return false;
        return PT::is_promotion_fully_superior(m_board[MT::get_src_sq(move)]);
    }

private:
    State(const Board<P>& b, const Stands& s, const ColorEnum& turn)
        : m_board(b), m_stands(s), m_turn(turn)
    {
        update_checkers();
    }
    void update_checkers()
    {
        std::fill_n(m_checkers, 2, DIR_NA);
        uint index = 0u;
        const auto king_sq = m_board.get_king_square(m_turn);
        for (auto dir : C::direction_iterator()) {
            const auto sq = m_board.find_attacker(~m_turn, king_sq, dir);
            if (sq != C::SQ_NA) {
                m_checkers[index++] = dir;
                if (index > 1)
                    break;
            }
        }
    }
    void update_checkers_before_turn_update(const Square& dst)
    {
        const auto enemy_king_sq = m_board.get_king_square(~m_turn);
        const bool check_by_moved = is_check_by_moved(enemy_king_sq, dst);

        m_checkers[0]
            = check_by_moved ? ST::direction(enemy_king_sq, dst) : DIR_NA;
        m_checkers[1] = DIR_NA;
    }
    void
    update_checkers_before_turn_update(const Square& dst, const Square& src)
    {
        const auto enemy_king_sq = m_board.get_king_square(~m_turn);
        if (enemy_king_sq == C::SQ_NA) {
            m_checkers[0] = DIR_NA;
            m_checkers[1] = DIR_NA;
            return;
        }
        const auto dst_dir = ST::direction(enemy_king_sq, dst);
        const auto src_dir = ST::direction(enemy_king_sq, src);
        const auto discovered_checker_sq
            = find_discovered_checker_square(enemy_king_sq, dst_dir, src_dir);

        const bool check_by_discovered = (discovered_checker_sq != C::SQ_NA);
        const bool check_by_moved = is_check_by_moved(enemy_king_sq, dst);

        if (check_by_moved && check_by_discovered) {
            m_checkers[0] = dst_dir;
            m_checkers[1] = src_dir;
        } else if (check_by_moved) {
            m_checkers[0] = dst_dir;
            m_checkers[1] = DIR_NA;
        } else if (check_by_discovered) {
            m_checkers[0] = src_dir;
            m_checkers[1] = DIR_NA;
        } else {
            m_checkers[0] = DIR_NA;
            m_checkers[1] = DIR_NA;
        }
    }
    Square find_discovered_checker_square(
        const Square& enemy_king_sq,
        const DirectionEnum& dst_dir,
        const DirectionEnum& src_dir)
    {
        if ((src_dir == DIR_NA) || (src_dir == dst_dir))
            return C::SQ_NA;
        return m_board.find_attacker(m_turn, enemy_king_sq, src_dir);
    }
    bool is_check_by_moved(const Square& enemy_king_sq, const Square& dst)
    {
        const auto attack = BT::get_attack_by(m_board[dst], dst);
        if (!BT::is_one(attack, enemy_king_sq))
            return false;
        if (!PT::is_slider(m_board[dst]))
            return true;
        return (BT::get_mask_between(dst, enemy_king_sq)
                & m_board.get_occupied())
               == 0u;
    }
    static void
    fill_ms24b_with(std::uint64_t* const hash, const Piece& p, const move_t& m)
    {
        if (hash == nullptr)
            return;
        *hash <<= 24u;
        *hash >>= 24u;

        static_assert(sizeof(Piece) == sizeof(std::uint8_t));
        static_assert(sizeof(move_t) == sizeof(std::uint16_t));
        *hash ^= static_cast<std::uint64_t>(p) << (64u - 8u);
        *hash ^= static_cast<std::uint64_t>(m) << (64u - 24u);
    }
};

template <class P>
State<P>& State<P>::apply_discard(const move_t& move, std::uint64_t* const hash)
{
    const Square dst = MT::get_dst(move);
    if (MT::is_drop(move)) {
        const PieceType src = MT::get_src_pt(move);
        const Piece p = m_stands.pop_piece_from(m_turn, src, hash);
        m_board.place_at(dst, p, hash);
        update_checkers_before_turn_update(dst);
    } else {
        const Square src = MT::get_src_sq(move);
        auto moving_piece = m_board.pop_from(src, hash);
        if (MT::get_promote(move))
            moving_piece = PT::promote_nocheck(moving_piece);
        const auto captured = m_board.place_at(dst, moving_piece, hash);
        // m_stands.add_captured_piece(captured, hash);
        update_checkers_before_turn_update(dst, src);
    }
    m_turn = ~m_turn;
    if (hash != nullptr)
        *hash ^= zobrist_hash_for_turn;
    return *this;
}

template <class P>
State<P>& State<P>::undo_discard(
    const move_t& move,
    const Piece& captured,
    const DirectionEnum& checker_dir_0,
    const DirectionEnum& checker_dir_1)
{
    const Square dst = MT::get_dst(move);
    // if (captured != VOID)
    //     m_stands.remove_captured_piece(captured);
    if (MT::is_drop(move)) {
        assert(captured == C::VOID);
        const auto dropped = m_board.pop_from(dst);
        m_stands.return_dropped_piece(dropped);
    } else {
        const auto src = MT::get_src_sq(move);
        auto moved = m_board.place_at(dst, captured);
        if (MT::get_promote(move))
            moved = PT::demote_nocheck(moved);
        m_board.place_at(src, moved);
    }
    m_turn = ~m_turn;
    m_checkers[0] = checker_dir_0;
    m_checkers[1] = checker_dir_1;
    return *this;
}

} // namespace vshogi

#endif // VSHOGI_STATE_HPP
