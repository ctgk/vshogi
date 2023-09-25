#ifndef VSHOGI_ANIMAL_SHOGI_BOARD_HPP
#define VSHOGI_ANIMAL_SHOGI_BOARD_HPP

#include "vshogi/animal_shogi/bitboard.hpp"
#include "vshogi/animal_shogi/piece.hpp"
#include "vshogi/animal_shogi/squares.hpp"
#include "vshogi/board.hpp"

namespace vshogi::animal_shogi
{

using Board = vshogi::Board<Squares, Pieces, BitBoard>;

} // namespace vshogi::animal_shogi

namespace vshogi
{

template <>
inline animal_shogi::BitBoard
animal_shogi::Board::to_attack_mask(const ColorEnum c) const
{
    animal_shogi::BitBoard out = animal_shogi::BitBoard();
    for (auto sq : animal_shogi::Squares::square_array) {
        const auto p = m_pieces[sq];
        if (get_color(p) == c)
            out |= animal_shogi::BitBoard::get_attacks_by(p, sq);
    }
    return out;
}

template <>
inline animal_shogi::Squares::SquareEnum
animal_shogi::Board::king_location(const ColorEnum c) const
{
    const auto target = to_board_piece(c, animal_shogi::Pieces::LI);
    for (auto sq : animal_shogi::Squares::square_array) {
        if (m_pieces[sq] == target)
            return sq;
    }
    return SQ_NA;
}

} // namespace vshogi

#endif // VSHOGI_ANIMAL_SHOGI_BOARD_HPP
