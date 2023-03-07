#ifndef VSHOGI_ANIMAL_SHOGI_BOARD_HPP
#define VSHOGI_ANIMAL_SHOGI_BOARD_HPP

#include <cstdint>

#include "vshogi/animal_shogi/piece.hpp"
#include "vshogi/animal_shogi/squares.hpp"

namespace vshogi::animal_shogi
{

class Board
{
private:
    BoardPieceTypeEnum m_pieces[NUM_SQUARES];

    const char* set_sfen_rank(const char* const sfen_rank, const RankEnum rank);

public:
    Board()
        : m_pieces{
            // clang-format off
            W_GI, W_LI, W_EL,
            VOID, W_CH, VOID,
            VOID, B_CH, VOID,
            B_EL, B_LI, B_GI,
            // clang-format on
        }
    {
    }

    /**
     * @brief Set pieces on the board according to given SFEN string.
     *
     * @param sfen SFEN string. e.g. "gle/1c1/1C1/ELG b - 1"
     * @return const char* Remaining SFEN string. e.g. "b - 1"
     */
    const char* set_sfen(const char* const sfen);
    BoardPieceTypeEnum get_piece_at(const SquareEnum sq) const
    {
        return m_pieces[sq];
    }
    PieceTypeEnum pop_piece_at(const SquareEnum sq)
    {
        const auto out = to_piece_type(m_pieces[sq]);
        m_pieces[sq] = VOID;
        return out;
    }

    /**
     * @brief Place piece by overwriting the square.
     *
     * @param sq Location to place piece.
     * @param piece Piece to place.
     * @return Board& board with the piece placed on the square.
     */
    Board& place_piece_at(const SquareEnum sq, const BoardPieceTypeEnum p)
    {
        m_pieces[sq] = p;
        return *this;
    }
};

} // namespace vshogi::animal_shogi

#endif // VSHOGI_ANIMAL_SHOGI_BOARD_HPP
