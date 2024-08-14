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
private:
    using SHelper = Squares<Config>;
    using PHelper = Pieces<Config>;
    using BitBoardType = BitBoard<Config>;
    using MoveType = Move<Config>;
    using PieceType = typename Config::PieceType;
    using BoardPieceType = typename Config::BoardPieceType;
    using Square = typename Config::Square;
    using Rank = typename Config::Rank;
    static constexpr auto num_dir = Config::num_dir;
    static constexpr auto num_files = Config::num_files;
    static constexpr auto num_ranks = Config::num_ranks;
    static constexpr auto num_squares = Config::num_squares;
    static constexpr auto num_piece_types = Config::num_piece_types;
    static constexpr auto num_square_states = num_colors * num_piece_types + 1;
    static constexpr auto VOID = PHelper::VOID; // NOLINT
    static constexpr auto SQ_NA = SHelper::SQ_NA; // NOLINT

    static std::uint64_t zobrist_table[num_squares][num_square_states];

private:
    BoardPieceType m_pieces[num_squares];
    Square m_king_locations[num_colors];

public:
    Board();
    Board(const char* const sfen)
    {
        set_sfen(sfen);
    }
    BoardPieceType operator[](const Square& sq) const
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
    void append_sfen(std::string& out) const
    {
        append_sfen_rank(static_cast<Rank>(0), out);
        for (uint ir = 1; ir < num_ranks; ++ir) {
            out += '/';
            append_sfen_rank(static_cast<Rank>(ir), out);
        }
    }
    BoardPieceType apply(
        const Square& dst,
        const BoardPieceType& p,
        std::uint64_t* const hash = nullptr)
    {
        if (PHelper::to_piece_type(p) == PHelper::OU)
            m_king_locations[PHelper::get_color(p)] = dst;
        const BoardPieceType popped = place_piece_on(dst, p);
        if (PHelper::to_piece_type(popped) == PHelper::OU)
            m_king_locations[PHelper::get_color(popped)] = SQ_NA;
        if (hash != nullptr) {
            *hash ^= zobrist_table[dst][to_index(popped)];
            *hash ^= zobrist_table[dst][to_index(p)];
        }
        return popped;
    }
    BoardPieceType apply(
        const Square& dst,
        const Square& src,
        const bool& promote = false,
        std::uint64_t* const hash = nullptr)
    {
        BoardPieceType moving_piece = place_piece_on(src, VOID);
        if (hash != nullptr) {
            *hash ^= zobrist_table[src][to_index(VOID)];
            *hash ^= zobrist_table[src][to_index(moving_piece)];
        }
        if (promote)
            moving_piece = PHelper::promote_nocheck(moving_piece);
        return apply(dst, moving_piece, hash);
    }
    const char* set_sfen(const char* sfen)
    {
        for (uint ir = 0U; ir < num_ranks; ++ir) {
            sfen = set_sfen_rank(sfen, static_cast<Rank>(ir));
        }
        update_king_position_based_on_pieces();
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
        for (; *ptr_sq != SQ_NA; ++ptr_sq) {
            const auto& sq = *ptr_sq;
            const auto& p = m_pieces[sq];
            if ((p == VOID) || (sq == skip))
                continue;
            if ((PHelper::get_color(p) == attacker_color)
                && BitBoardType::get_attacks_by(p, sq).is_one(attacked))
                return sq;
            return SQ_NA;
        }
        return SQ_NA;
    }
    bool is_square_attacked(
        const ColorEnum& attacker_color,
        const Square& sq,
        const Square& skip = SQ_NA) const
    {
        for (auto dir = static_cast<DirectionEnum>(num_dir); dir--;) {
            if (find_attacker(attacker_color, sq, dir, skip) != SQ_NA)
                return true;
        }
        return false;
    }
    Board hflip() const
    {
        Board out;
        for (auto sq = static_cast<Square>(num_squares); sq--;) {
            const auto sq_hflipped = SHelper::hflip(sq);
            out.m_pieces[sq_hflipped] = m_pieces[sq];
        }
        out.update_king_position_based_on_pieces();
        return out;
    }
    static void init_tables()
    {
        std::random_device dev;
        std::mt19937_64 rng(dev());
        std::uniform_int_distribution<std::uint64_t> dist;
        for (auto sq = static_cast<Square>(num_squares); sq--;) {
            for (int ii = 0; ii < num_square_states; ++ii) {
                zobrist_table[sq][ii] = dist(rng);
            }
        }
    }
    std::uint64_t zobrist_hash() const
    {
        std::uint64_t out = static_cast<std::uint64_t>(0);
        for (auto sq = static_cast<Square>(num_squares); sq--;) {
            out ^= zobrist_table[sq][to_index(m_pieces[sq])];
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
    BoardPieceType place_piece_on(const Square& sq, const BoardPieceType& p)
    {
        const auto out = m_pieces[sq];
        m_pieces[sq] = p;
        return out;
    }
    void update_king_position_based_on_pieces()
    {
        m_king_locations[BLACK] = SQ_NA;
        m_king_locations[WHITE] = SQ_NA;
        for (auto sq = static_cast<Square>(num_squares); sq--;) {
            const auto& p = m_pieces[sq];
            if (PHelper::to_piece_type(p) == PHelper::OU)
                m_king_locations[PHelper::get_color(p)] = sq;
        }
    }
    static uint to_index(const BoardPieceType& p)
    {
        if (p == PHelper::VOID)
            return 2u * num_piece_types;
        uint out = PHelper::to_piece_type(p);
        out += (PHelper::get_color(p) == WHITE) * num_piece_types;
        return out;
    }
};

} // namespace vshogi

#endif // VSHOGI_BOARD_HPP
