#ifndef VSHOGI_BOARD_HPP
#define VSHOGI_BOARD_HPP

#include <string>

#include "vshogi/color.hpp"
#include "vshogi/direction.hpp"

namespace vshogi
{

template <class Squares, class Pieces, class BitBoard>
class Board
{
public:
    using SquaresType = Squares;
    using PiecesType = Pieces;
    using BitBoardType = BitBoard;
    using BoardPieceTypeEnum = typename Pieces::BoardPieceTypeEnum;
    using SquareEnum = typename Squares::SquareEnum;
    using RankEnum = typename Squares::RankEnum;

private:
    static constexpr auto get_color = Pieces::get_color;
    static constexpr auto to_board_piece = Pieces::to_board_piece;
    static constexpr auto VOID = Pieces::VOID; // NOLINT
    static constexpr auto num_files = Squares::num_files;
    static constexpr auto num_ranks = Squares::num_ranks;
    static constexpr auto num_squares = Squares::num_squares;
    static constexpr auto SQ_NA = Squares::SQ_NA; // NOLINT

private:
    BoardPieceTypeEnum m_pieces[num_squares];

public:
    Board();
    bool operator==(const Board& other) const
    {
        for (auto& sq : Squares::square_array) {
            if (m_pieces[sq] != other.m_pieces[sq])
                return false;
        }
        return true;
    }
    bool operator!=(const Board& other) const
    {
        for (auto& sq : Squares::square_array) {
            if (m_pieces[sq] != other.m_pieces[sq])
                return true;
        }
        return false;
    }
    BoardPieceTypeEnum operator[](const SquareEnum sq) const
    {
        return m_pieces[sq];
    }
    BoardPieceTypeEnum& operator[](const SquareEnum sq)
    {
        return m_pieces[sq];
    }
    bool is_empty(const SquareEnum sq) const
    {
        return (m_pieces[sq] == VOID);
    }
    const char* set_sfen(const char* sfen)
    {
        for (int ir = 0; ir < num_ranks; ++ir) {
            sfen = set_sfen_rank(sfen, static_cast<RankEnum>(ir));
        }
        return sfen;
    }
    void append_sfen(std::string& out) const
    {
        append_sfen_rank(static_cast<RankEnum>(0), out);
        for (int ir = 1; ir < num_ranks; ++ir) {
            out += '/';
            append_sfen_rank(static_cast<RankEnum>(ir), out);
        }
    }
    SquareEnum find_attacker(
        const ColorEnum& attacker_color,
        const SquareEnum& attacked,
        const DirectionEnum& dir,
        const SquareEnum& skip = SQ_NA) const
    {
        auto ptr_sq = Squares::get_squares_along(dir, attacked);
        if (ptr_sq == nullptr)
            return SQ_NA;
        for (; *ptr_sq != SQ_NA; ++ptr_sq) {
            const auto sq = *ptr_sq;
            const auto p = m_pieces[sq];
            if ((p == VOID) || (sq == skip))
                continue;
            if ((Pieces::get_color(p) == attacker_color)
                && BitBoard::get_attacks_by(p, sq).is_one(attacked))
                return sq;
            return SQ_NA;
        }
        return SQ_NA;
    }
    Board hflip() const
    {
        Board out;
        for (auto&& sq : Squares::square_array) {
            const auto f = static_cast<int>(sq % num_files);
            const auto r = static_cast<int>(sq / num_files);
            const auto f_hflipped = (num_files - 1) - f;
            const auto sq_hflipped
                = static_cast<SquareEnum>(r * num_files + f_hflipped);
            out.m_pieces[sq_hflipped] = m_pieces[sq];
        }
        return out;
    }

private:
    const char* set_sfen_rank(const char* const sfen_rank, const RankEnum rank);
    void append_sfen_rank(const RankEnum rank, std::string& out) const
    {
        auto ptr = m_pieces + num_files * static_cast<int>(rank);
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
            Pieces::append_sfen(*ptr, out);
        }
        if (num_void > 0)
            out += static_cast<char>('0' + num_void);
    }
};

} // namespace vshogi

#endif // VSHOGI_BOARD_HPP
