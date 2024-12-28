#ifndef VSHOGI_BOARD_HPP
#define VSHOGI_BOARD_HPP

#include <cassert>
#include <random>
#include <string>

#include "vshogi/common/bitboard.hpp"
#include "vshogi/common/color.hpp"
#include "vshogi/common/config.hpp"
#include "vshogi/common/direction.hpp"
#include "vshogi/common/move.hpp"
#include "vshogi/common/pieces.hpp"
#include "vshogi/common/squares.hpp"

namespace vshogi
{

template <class Parameters>
class Board
{
private:
    using C = Configuration<Parameters>;

public:
    static constexpr auto num_files = C::num_files;
    static constexpr auto num_ranks = C::num_ranks;
    static constexpr auto num_squares = C::num_squares;

private:
    using SHelper = Squares<Parameters>;
    using PHelper = Pieces<Parameters>;
    using BitBoardType = BitBoard<Parameters>;
    using MoveType = Move<Parameters>;
    using PieceType = typename C::PieceType;
    using ColoredPiece = typename C::ColoredPiece;
    using Square = typename C::Square;
    using Rank = typename C::Rank;
    static constexpr auto num_square_states
        = num_colors * C::num_piece_types + 1;
    static constexpr auto VOID = PHelper::VOID; // NOLINT
    static constexpr auto SQ_NA = SHelper::SQ_NA; // NOLINT

    static std::uint64_t zobrist_table[num_squares][num_square_states];

private:
    ColoredPiece m_pieces[num_squares];
    Square m_king_locations[num_colors];
    BitBoardType m_bb_color[num_colors];
    BitBoardType m_bb_piece[C::num_piece_types];

public:
    Board();
    Board(const char* const sfen)
    {
        set_sfen(sfen);
    }
    ColoredPiece get(const Square& sq) const
    {
        assert(sq != SQ_NA);
        return m_pieces[sq];
    }
    ColoredPiece operator[](const Square& sq) const
    {
        assert(sq != SQ_NA);
        return m_pieces[sq];
    }
    bool is_empty(const Square& sq) const
    {
        assert(sq != SQ_NA);
        return (m_pieces[sq] == VOID);
    }
    Square get_king_location(const ColorEnum& c) const
    {
        return m_king_locations[c];
    }
    BitBoardType get_occupied() const
    {
        return m_bb_color[BLACK] ^ m_bb_color[WHITE];
    }
    BitBoardType get_occupied(const ColorEnum& c) const
    {
        return m_bb_color[c];
    }
    /**
     * @brief Get occupation by the given piece type.
     *
     * @param pt Piece type. Note that `NA` is not allowed here.
     * @return BitBoardType Occupation by the given piece type.
     */
    BitBoardType get_occupied(const PieceType& pt) const
    {
        assert(pt != PHelper::NA);
        return m_bb_piece[pt];
    }
    /**
     * @brief Get occupation by the given colored piece.
     *
     * @param p Colored piece. Note that `VOID` is not allowed here.
     * @return BitBoardType Occupation by the given colored piece.
     */
    BitBoardType get_occupied(const ColoredPiece& p) const
    {
        assert(p != VOID);
        const auto c = PHelper::get_color(p);
        const auto pt = PHelper::to_piece_type(p);
        return m_bb_color[c] & m_bb_piece[pt];
    }
    template <PieceType PT>
    BitBoardType get_occupied(const ColorEnum& c) const
    {
        static_assert(PT != PHelper::NA);
        return m_bb_color[c] & m_bb_piece[PT];
    }
    template <PieceType PT1, PieceType PT2, PieceType... Args>
    BitBoardType get_occupied(const ColorEnum& c) const
    {
        return get_occupied<PT1>(c) | get_occupied<PT2, Args...>(c);
    }
    BitBoardType get_occupied_by_ranging(const ColorEnum&) const
    {
        return BitBoardType();
    }
    void append_sfen(std::string& out) const
    {
        append_sfen_rank(static_cast<Rank>(0), out);
        for (uint ir = 1; ir < num_ranks; ++ir) {
            out += '/';
            append_sfen_rank(static_cast<Rank>(ir), out);
        }
    }
    ColoredPiece apply(
        const Square& dst,
        const ColoredPiece& p,
        std::uint64_t* const hash = nullptr)
    {
        const ColoredPiece popped = place_piece_on(dst, p);
        update_internals_by_placed(p, dst, hash);
        update_internals_by_popped(popped, dst, hash);
        return popped;
    }
    ColoredPiece apply(
        const Square& dst,
        const Square& src,
        const bool& promote = false,
        std::uint64_t* const hash = nullptr)
    {
        ColoredPiece moving_piece = place_piece_on(src, VOID);
        update_internals_by_placed(VOID, src, hash);
        update_internals_by_popped(moving_piece, src, hash);
        if (promote)
            moving_piece = PHelper::promote_nocheck(moving_piece);
        return apply(dst, moving_piece, hash);
    }
    const char* set_sfen(const char* sfen)
    {
        for (uint ir = 0U; ir < num_ranks; ++ir) {
            assert(sfen[0] != ' ');
            sfen = set_sfen_rank(sfen, static_cast<Rank>(ir));
        }
        assert((sfen[0] == ' ') || (sfen[0] == '\0'));
        if (sfen[0] == ' ')
            ++sfen;
        update_internals_based_on_pieces();
        return sfen;
    }
    BitBoardType get_attacks_by_nocheck(const Square& sq) const
    {
        assert(sq < SQ_NA);
        return BitBoardType::get_attacks_by(m_pieces[sq], sq, get_occupied());
    }
    Square find_attacker(
        const ColorEnum& attacker_color,
        const Square& attacked,
        const DirectionEnum& dir,
        const Square& skip = SQ_NA) const
    {
        auto ptr_sq = SHelper::get_squares_along(dir, attacked);
        if (ptr_sq == nullptr)
            return SQ_NA;
        if (!(BitBoardType::get_ray_to(attacked, dir)
              & m_bb_color[attacker_color])
                 .any())
            return SQ_NA;
        {
            const ColoredPiece& p = m_pieces[*ptr_sq];
            if ((p != VOID) && (*ptr_sq != skip)) {
                if ((PHelper::get_color(p) == attacker_color)
                    && BitBoardType::get_attacks_by(p, *ptr_sq)
                           .is_one(attacked))
                    return *ptr_sq;
                else
                    return SQ_NA;
            }
        }

        const auto dir_rotated = rotate(dir);
        ++ptr_sq;
        for (; *ptr_sq != SQ_NA; ++ptr_sq) {
            const auto& sq = *ptr_sq;
            assert(sq != SQ_NA);
            const auto& p = m_pieces[sq];
            if ((p == VOID) || (sq == skip))
                continue;
            if ((PHelper::get_color(p) == attacker_color)
                && PHelper::is_ranging_to(p, dir_rotated))
                return sq;
            return SQ_NA;
        }
        return SQ_NA;
    }
    Square find_ranging_attacker(
        const ColorEnum& attacker_color,
        const Square& attacked,
        const DirectionEnum& dir,
        const Square& skip = SQ_NA) const
    {
        if ((dir == DIR_NA) || (attacked == SQ_NA))
            return SQ_NA;
        const auto dir_from_attacker = rotate(dir);
        const auto ray = BitBoardType::get_ray_to(attacked, dir);
        if (!(ray & m_bb_color[attacker_color]).any())
            return SQ_NA;
        auto psq = SHelper::get_squares_along(dir, attacked);
        if (psq == nullptr)
            return SQ_NA;
        for (; *psq != SQ_NA; ++psq) {
            const auto& p = m_pieces[*psq];
            if ((p == VOID) || (*psq == skip))
                continue;
            if ((PHelper::get_color(p) == attacker_color)
                && PHelper::is_ranging_to(p, dir_from_attacker))
                return *psq;
            return SQ_NA;
        }
        return SQ_NA;
    }
    BitBoardType find_pinned(const ColorEnum& c) const
    {
        BitBoardType out{};
        const Square& king = m_king_locations[c];
        if (king == SQ_NA)
            return out;
        const BitBoardType occ_full = m_bb_color[BLACK] | m_bb_color[WHITE];
        for (auto atk : get_occupied_by_ranging(~c).square_iterator()) {
            const auto king_dir = SHelper::get_direction(king, atk);
            if (!PHelper::is_ranging_to(m_pieces[atk], king_dir))
                continue;
            const auto segment = BitBoardType::get_line_segment(atk, king);
            if ((occ_full & segment).hamming_weight() != 1u)
                continue;
            const BitBoardType pinned_mask = m_bb_color[c] & segment;
            if (pinned_mask.hamming_weight() == 1u)
                out.set(*pinned_mask.square_iterator());
        }
        return out;
    }
    bool is_square_attacked(
        const ColorEnum& by_side,
        const Square& sq,
        const Square& skip = SQ_NA) const
    {
        if (sq == SQ_NA)
            return false;
        for (auto dir : EnumIterator<DirectionEnum, C::num_dir>()) {
            if (find_attacker(by_side, sq, dir, skip) != SQ_NA)
                return true;
        }
        return false;
    }
    bool is_drop_pawn_mate(const Square& dst, const ColorEnum& by_side) const
    {
        if (!is_pawn_attacking_to_enemy_king(dst, by_side))
            return false;
        if (king_can_move_away_from_a_pawn_attack(~by_side))
            return false;
        if (enemy_can_capture_the_drop_pawn(dst, by_side))
            return false;
        return true;
    }
    Board hflip() const
    {
        Board out;
        for (auto sq : EnumIterator<Square, num_squares>()) {
            const auto sq_hflipped = SHelper::hflip(sq);
            assert(sq_hflipped != SQ_NA);
            out.m_pieces[sq_hflipped] = m_pieces[sq];
        }
        out.update_internals_based_on_pieces();
        return out;
    }
    static void init_tables()
    {
        std::random_device dev;
        std::mt19937_64 rng(dev());
        std::uniform_int_distribution<std::uint64_t> dist;
        for (auto sq : EnumIterator<Square, num_squares>()) {
            for (uint ii = 0u; ii < num_square_states; ++ii) {
                zobrist_table[sq][ii] = dist(rng);
            }
        }
    }
    std::uint64_t zobrist_hash() const
    {
        std::uint64_t out = static_cast<std::uint64_t>(0);
        for (auto sq = num_squares; sq--;) {
            out ^= zobrist_table[sq][m_pieces[sq]];
        }
        return out;
    }

private:
    const char* set_sfen_rank(const char* const sfen_rank, const Rank rank)
    {
        constexpr int max_length = 19; // e.g. "+p+p+p+p+p+p+p+p+p/"
        auto piece_ptr = m_pieces + rank + (num_files - 1u) * num_ranks;
        const char* sfen_ptr = sfen_rank;
        bool promotion_flag = false;
        for (; sfen_ptr < sfen_rank + max_length; ++sfen_ptr) {
            switch (*sfen_ptr) {
            case '/':
                ++sfen_ptr;
            case ' ':
            case '\0':
                goto OUT_OF_LOOP;
            case '9':
                *piece_ptr = VOID;
                piece_ptr -= num_ranks; // fall-through
            case '8':
                *piece_ptr = VOID;
                piece_ptr -= num_ranks; // fall-through
            case '7':
                *piece_ptr = VOID;
                piece_ptr -= num_ranks; // fall-through
            case '6':
                *piece_ptr = VOID;
                piece_ptr -= num_ranks; // fall-through
            case '5':
                *piece_ptr = VOID;
                piece_ptr -= num_ranks; // fall-through
            case '4':
                *piece_ptr = VOID;
                piece_ptr -= num_ranks; // fall-through
            case '3':
                *piece_ptr = VOID;
                piece_ptr -= num_ranks; // fall-through
            case '2':
                *piece_ptr = VOID;
                piece_ptr -= num_ranks; // fall-through
            case '1':
                *piece_ptr = VOID;
                piece_ptr -= num_ranks;
                break;
            case '+':
                promotion_flag = true;
                continue;
            default:
                *piece_ptr = promotion_flag
                                 ? PHelper::promote_nocheck(
                                     PHelper::to_board_piece(*sfen_ptr))
                                 : PHelper::to_board_piece(*sfen_ptr);
                piece_ptr -= num_ranks;
                break;
            }
            promotion_flag = false;
        }
    OUT_OF_LOOP:
        assert(piece_ptr + num_ranks == m_pieces + rank);
        return sfen_ptr;
    }
    void append_sfen_rank(const Rank rank, std::string& out) const
    {
        auto ptr = m_pieces + rank + (num_files - 1u) * num_ranks;
        int num_void = 0;
        for (; ptr >= m_pieces; ptr -= num_ranks) {
            if (*ptr == VOID) {
                ++num_void;
                continue;
            }
            if (num_void > 0) {
                out += static_cast<char>('0' + num_void);
                num_void = 0;
            }
            PHelper::append_sfen(*ptr, out);
        }
        if (num_void > 0)
            out += static_cast<char>('0' + num_void);
    }
    ColoredPiece place_piece_on(const Square& sq, const ColoredPiece& p)
    {
        assert(sq < SQ_NA);
        const auto out = m_pieces[sq];
        m_pieces[sq] = p;
        return out;
    }
    void update_internals_based_on_pieces()
    {
        m_king_locations[BLACK] = SQ_NA;
        m_king_locations[WHITE] = SQ_NA;
        std::fill_n(m_bb_color, num_colors, BitBoardType());
        std::fill_n(m_bb_piece, C::num_piece_types, BitBoardType());
        for (auto sq : EnumIterator<Square, num_squares>()) {
            const auto& p = m_pieces[sq];
            const auto c = PHelper::get_color(p);
            const auto pt = PHelper::to_piece_type(p);
            if (pt == PHelper::OU)
                m_king_locations[c] = sq;
            if (p != VOID) {
                m_bb_color[c].toggle(sq);
                m_bb_piece[pt].toggle(sq);
            }
        }
    }
    void update_internals_by_placed(
        const ColoredPiece& p,
        const Square& sq,
        std::uint64_t* const hash = nullptr)
    {
        assert(sq < SQ_NA);
        assert(p < num_square_states);
        if (hash != nullptr)
            *hash ^= zobrist_table[sq][p];
        if (p == VOID)
            return;
        const auto c = PHelper::get_color(p);
        const auto pt = PHelper::to_piece_type(p);
        if (pt == PHelper::OU)
            m_king_locations[c] = sq;
        m_bb_color[c].toggle(sq);
        m_bb_piece[pt].toggle(sq);
    }
    void update_internals_by_popped(
        const ColoredPiece& p,
        const Square& sq,
        std::uint64_t* const hash = nullptr)
    {
        assert(sq < SQ_NA);
        if (hash != nullptr)
            *hash ^= zobrist_table[sq][p];
        if (p == VOID)
            return;
        const auto c = PHelper::get_color(p);
        const auto pt = PHelper::to_piece_type(p);
        if (pt == PHelper::OU)
            m_king_locations[c] = SQ_NA;
        m_bb_color[c].toggle(sq);
        m_bb_piece[pt].toggle(sq);
    }
    template <PieceType PT>
    bool is_square_attacked_by(const ColorEnum& by_side, const Square& sq) const
    {
        static_assert(PT < PHelper::NA);
        assert(sq < SQ_NA);
        const auto attack_inverted = BitBoardType::get_attacks_by(
            PHelper::to_board_piece(~by_side, PT), sq);
        const auto occ_offence
            = get_occupied(PHelper::to_board_piece(by_side, PT));
        return (attack_inverted & occ_offence).any();
    }
    template <PieceType Base, PieceType Alike, PieceType... Args>
    bool is_square_attacked_by(const ColorEnum& by_side, const Square& sq) const
    {
        static_assert(Base < PHelper::NA);
        assert(sq < SQ_NA);
        const auto attack_inverted = BitBoardType::get_attacks_by(
            PHelper::to_board_piece(~by_side, Base), sq);
        const auto occ_offence = get_occupied<Base, Alike, Args...>(by_side);
        return (attack_inverted & occ_offence).any();
    }
    bool is_square_attacked_by_ranging_pieces(
        const ColorEnum& by_side, const Square& sq, const Square& skip) const;
    bool is_pawn_attacking_to_enemy_king(
        const Square& sq, const ColorEnum& by_side) const
    {
        const auto enemy_king_sq = m_king_locations[~by_side];
        if (enemy_king_sq == SQ_NA)
            return false;
        return enemy_king_sq
               == SHelper::shift(sq, (by_side == BLACK) ? DIR_N : DIR_S);
    }
    bool
    king_can_move_away_from_a_pawn_attack(const ColorEnum& king_color) const
    {
        const Square& king_sq = m_king_locations[king_color];
        const BitBoardType& ally_mask = m_bb_color[king_color];
        const BitBoardType king_dst_mask
            = get_attacks_by_nocheck(king_sq) & (~ally_mask);
        for (auto sq : king_dst_mask.square_iterator()) {
            if (is_square_attacked(~king_color, sq, king_sq))
                continue;
            return true;
        }
        return false;
    }
    bool enemy_can_capture_the_drop_pawn(
        const Square& dst, const ColorEnum& by_side) const
    {
        const auto enemy_king_sq = m_king_locations[~by_side];
        const auto enemy_king_dir = (by_side == BLACK) ? DIR_N : DIR_S;
        for (auto dir : EnumIterator<DirectionEnum, C::num_dir>()) {
            if (dir == enemy_king_dir)
                continue;
            const auto src_next = find_attacker(~by_side, dst, dir);
            const bool is_attacking_the_pawn = (src_next != SHelper::SQ_NA);
            if (is_attacking_the_pawn) {
                const auto discovered_dir
                    = SHelper::get_direction(src_next, enemy_king_sq);
                const auto discovered_attacker_sq = find_ranging_attacker(
                    by_side, enemy_king_sq, discovered_dir, src_next);
                const auto is_pinned
                    = (discovered_attacker_sq != SHelper::SQ_NA);
                if (!is_pinned)
                    return true;
            }
        }
        return false;
    }
};

} // namespace vshogi

#endif // VSHOGI_BOARD_HPP
