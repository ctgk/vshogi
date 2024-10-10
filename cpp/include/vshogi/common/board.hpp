#ifndef VSHOGI_BOARD_HPP
#define VSHOGI_BOARD_HPP

#include <random>
#include <string>

#include "vshogi/common/bitboard.hpp"
#include "vshogi/common/color.hpp"
#include "vshogi/common/direction.hpp"
#include "vshogi/common/move.hpp"
#include "vshogi/common/pieces.hpp"
#include "vshogi/common/squares.hpp"

namespace vshogi
{

template <class Config>
class Board
{
public:
    static constexpr auto num_files = Config::num_files;
    static constexpr auto num_ranks = Config::num_ranks;
    static constexpr auto num_squares = Config::num_squares;

private:
    using SHelper = Squares<Config>;
    using PHelper = Pieces<Config>;
    using BitBoardType = BitBoard<Config>;
    using MoveType = Move<Config>;
    using PieceType = typename Config::PieceType;
    using ColoredPiece = typename Config::ColoredPiece;
    using Square = typename Config::Square;
    using Rank = typename Config::Rank;
    static constexpr auto num_dir = Config::num_dir;
    static constexpr auto num_piece_types = Config::num_piece_types;
    static constexpr auto num_square_states = num_colors * num_piece_types + 1;
    static constexpr auto VOID = PHelper::VOID; // NOLINT
    static constexpr auto SQ_NA = SHelper::SQ_NA; // NOLINT

    static std::uint64_t zobrist_table[num_squares][num_square_states];

private:
    ColoredPiece m_pieces[num_squares];
    Square m_king_locations[num_colors];
    BitBoardType m_bb_color[num_colors];
    BitBoardType m_bb_piece[num_piece_types];

public:
    Board();
    Board(const char* const sfen)
    {
        set_sfen(sfen);
    }
    ColoredPiece get(const Square& sq) const
    {
        return m_pieces[sq];
    }
    ColoredPiece operator[](const Square& sq) const
    {
        return m_pieces[sq];
    }
    bool is_empty(const Square& sq) const
    {
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
        const auto c = PHelper::get_color(p);
        const auto pt = PHelper::to_piece_type(p);
        return m_bb_color[c] & m_bb_piece[pt];
    }
    template <PieceType PT>
    BitBoardType get_occupied(const ColorEnum& c) const
    {
        return m_bb_color[c] & m_bb_piece[PT];
    }
    template <PieceType PT1, PieceType PT2, PieceType... Args>
    BitBoardType get_occupied(const ColorEnum& c) const
    {
        return get_occupied<PT1>(c) | get_occupied<PT2, Args...>(c);
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
            sfen = set_sfen_rank(sfen, static_cast<Rank>(ir));
        }
        update_internals_based_on_pieces();
        return sfen;
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
        const Square& king = m_king_locations[c];
        BitBoardType out{};
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
        for (auto dir : EnumIterator<DirectionEnum, num_dir>()) {
            if (find_attacker(by_side, sq, dir, skip) != SQ_NA)
                return true;
        }
        return false;
    }
    Board hflip() const
    {
        Board out;
        for (auto sq : EnumIterator<Square, num_squares>()) {
            const auto sq_hflipped = SHelper::hflip(sq);
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
    const char* set_sfen_rank(const char* const sfen_rank, const Rank rank);
    void append_sfen_rank(const Rank rank, std::string& out) const
    {
        auto ptr = m_pieces + num_files * static_cast<uint>(rank);
        const auto end = ptr + num_files;
        int num_void = 0;
        for (; ptr < end; ++ptr) {
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
        const auto out = m_pieces[sq];
        m_pieces[sq] = p;
        return out;
    }
    void update_internals_based_on_pieces()
    {
        m_king_locations[BLACK] = SQ_NA;
        m_king_locations[WHITE] = SQ_NA;
        std::fill_n(m_bb_color, num_colors, BitBoardType());
        std::fill_n(m_bb_piece, num_piece_types, BitBoardType());
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
        const auto attack_inverted = BitBoardType::get_attacks_by(
            PHelper::to_board_piece(~by_side, PT), sq);
        const auto occ_offence
            = get_occupied(PHelper::to_board_piece(by_side, PT));
        return (attack_inverted & occ_offence).any();
    }
    template <PieceType Base, PieceType Alike, PieceType... Args>
    bool is_square_attacked_by(const ColorEnum& by_side, const Square& sq) const
    {
        const auto attack_inverted = BitBoardType::get_attacks_by(
            PHelper::to_board_piece(~by_side, Base), sq);
        const auto occ_offence = get_occupied<Base, Alike, Args...>(by_side);
        return (attack_inverted & occ_offence).any();
    }
    bool is_square_attacked_by_ranging_pieces(
        const ColorEnum& by_side, const Square& sq, const Square& skip) const;
    BitBoardType get_occupied_by_ranging(const ColorEnum&) const
    {
        return BitBoardType();
    }
};

} // namespace vshogi

#endif // VSHOGI_BOARD_HPP
