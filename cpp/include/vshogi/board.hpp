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
    static constexpr auto RANK1 = Squares::RANK1; // NOLINT
    static constexpr auto SQ_NA = Squares::SQ_NA; // NOLINT

private:
    BoardPieceTypeEnum m_pieces[num_squares];

public:
    Board();
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
    std::string to_sfen() const
    {
        auto out = std::string();
        out += to_sfen_rank(RANK1);
        for (int ir = 1; ir < num_ranks; ++ir) {
            out += '/';
            out += to_sfen_rank(static_cast<RankEnum>(ir));
        }
        return out;
    }
    SquareEnum find_attacker(
        const ColorEnum& attacker_color,
        const SquareEnum& attacked,
        const DirectionEnum& dir,
        const SquareEnum& skip = SQ_NA) const
    {
        auto ptr_sq = BitBoard::get_squares_along(dir, attacked);
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

private:
    const char* set_sfen_rank(const char* const sfen_rank, const RankEnum rank);
    std::string to_sfen_rank(const RankEnum rank) const
    {
        auto begin = m_pieces + num_files * static_cast<int>(rank);
        const auto end = begin + num_files;
        auto out = std::string();
        int num_void = 0;
        for (; begin < end; ++begin) {
            if (*begin == VOID) {
                ++num_void;
                continue;
            }
            if (num_void > 0) {
                out += static_cast<char>('0' + num_void);
                num_void = 0;
            }
            out += Pieces::to_sfen(*begin);
        }
        if (num_void > 0)
            out += static_cast<char>('0' + num_void);
        return out;
    }
};

} // namespace vshogi

#endif // VSHOGI_BOARD_HPP
