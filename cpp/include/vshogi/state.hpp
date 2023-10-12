#ifndef VSHOGI_STATE_HPP
#define VSHOGI_STATE_HPP

#include <string>
#include <type_traits>
#include <vector>

#include "vshogi/color.hpp"
#include "vshogi/direction.hpp"

namespace vshogi
{

template <class Board, class Stands, class Move>
class State
{
    static_assert(std::is_same<
                  typename Board::PiecesType,
                  typename Move::PiecesType>::value);
    static_assert(std::is_same<
                  typename Board::SquaresType,
                  typename Move::SquaresType>::value);

private:
    using BitBoard = typename Board::BitBoardType;
    using Squares = typename Board::SquaresType;
    using Pieces = typename Board::PiecesType;
    using Stand = typename Stands::StandType;

public:
    using BoardType = Board;
    using BitBoardType = typename Board::BitBoardType;
    using SquaresType = typename Board::SquaresType;
    using PiecesType = typename Board::PiecesType;
    using StandType = typename Stands::StandType;
    using MoveType = Move;
    using PieceTypeEnum = typename Pieces::PieceTypeEnum;
    using BoardPieceTypeEnum = typename Pieces::BoardPieceTypeEnum;
    using RankEnum = typename Squares::RankEnum;
    using SquareEnum = typename Squares::SquareEnum;

private:
    Board m_board;
    Stands m_stands;
    ColorEnum m_turn; //!< Player to make a move in the current state.

public:
    State() : m_board(), m_stands(), m_turn(BLACK)
    {
    }
    State(const std::string& sfen) : m_board(), m_stands(), m_turn()
    {
        set_sfen(sfen);
    }
    const Board& get_board() const
    {
        return m_board;
    }
    const Stand& get_stand(const ColorEnum c) const
    {
        return m_stands[c];
    }
    ColorEnum get_turn() const
    {
        return m_turn;
    }
    void set_sfen(const std::string& sfen)
    {
        auto s = sfen.c_str();
        s = m_board.set_sfen(s);
        {
            (*s == 'b') ? m_turn = BLACK : m_turn = WHITE;
            s += 2;
        }
        m_stands.set_sfen_holdings(s);
    }
    std::string to_sfen() const
    {
        return m_board.to_sfen() + " " + to_sfen_turn() + " "
               + m_stands.to_sfen_holdings();
    }
    State& apply(const Move move)
    {
        const auto dst = move.destination();
        auto moving = pop_piece_from_stand_or_board(move);
        const auto captured = Pieces::to_piece_type(m_board[dst]);
        if (captured != Pieces::NA)
            m_stands[m_turn].add(captured);
        if (move.promote())
            moving = Pieces::promote(moving);
        m_board[dst] = moving;
        m_turn = ~m_turn;
        return *this;
    }

private:
    std::string to_sfen_turn() const
    {
        return (m_turn == BLACK) ? "b" : "w";
    }
    BoardPieceTypeEnum pop_piece_from_stand_or_board(const Move move)
    {
        if (move.is_drop()) {
            const auto src = move.source_piece();
            const auto out = Pieces::to_board_piece(m_turn, src);
            m_stands[m_turn].subtract(src);
            return out;
        } else {
            const auto src = move.source_square();
            const auto out = m_board[src];
            m_board[src] = Pieces::VOID;
            return out;
        }
    }
};

} // namespace vshogi

#endif // VSHOGI_STATE_HPP
