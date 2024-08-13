#ifndef VSHOGI_BOARD_HPP
#define VSHOGI_BOARD_HPP

#include <string>

#include "vshogi/common/bitboard.hpp"
#include "vshogi/common/color.hpp"
#include "vshogi/common/direction.hpp"
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
    using BoardPieceType = typename Config::BoardPieceType;
    using Square = typename Config::Square;
    using Rank = typename Config::Rank;
    static constexpr auto num_dir = Config::num_dir;
    static constexpr auto num_files = Config::num_files;
    static constexpr auto num_ranks = Config::num_ranks;
    static constexpr auto num_squares = Config::num_squares;
    static constexpr auto VOID = PHelper::VOID; // NOLINT
    static constexpr auto SQ_NA = SHelper::SQ_NA; // NOLINT

private:
    BoardPieceType m_pieces[num_squares];

public:
    Board();
    Board(const char* const sfen)
    {
        set_sfen(sfen);
    }
    bool operator==(const Board& other) const
    {
        for (auto& sq : SHelper::square_array) {
            if (m_pieces[sq] != other.m_pieces[sq])
                return false;
        }
        return true;
    }
    bool operator!=(const Board& other) const
    {
        for (auto& sq : SHelper::square_array) {
            if (m_pieces[sq] != other.m_pieces[sq])
                return true;
        }
        return false;
    }
    BoardPieceType operator[](const Square& sq) const
    {
        return m_pieces[sq];
    }
    BoardPieceType& operator[](const Square& sq)
    {
        return m_pieces[sq];
    }
    bool is_empty(const Square& sq) const
    {
        return (m_pieces[sq] == VOID);
    }
    const char* set_sfen(const char* sfen)
    {
        for (uint ir = 0U; ir < num_ranks; ++ir) {
            sfen = set_sfen_rank(sfen, static_cast<Rank>(ir));
        }
        return sfen;
    }
    void append_sfen(std::string& out) const
    {
        append_sfen_rank(static_cast<Rank>(0), out);
        for (uint ir = 1; ir < num_ranks; ++ir) {
            out += '/';
            append_sfen_rank(static_cast<Rank>(ir), out);
        }
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
        for (auto&& sq : SHelper::square_array) {
            const auto sq_hflipped = SHelper::hflip(sq);
            out.m_pieces[sq_hflipped] = m_pieces[sq];
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
};

} // namespace vshogi

#endif // VSHOGI_BOARD_HPP
