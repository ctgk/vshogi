#ifndef VSHOGI_COMMON_GENERATOR_HPP
#define VSHOGI_COMMON_GENERATOR_HPP

#include "vshogi/common/bitboard.hpp"
#include "vshogi/common/board.hpp"
#include "vshogi/common/color.hpp"
#include "vshogi/common/move.hpp"
#include "vshogi/common/squares.hpp"
#include "vshogi/common/state.hpp"
#include "vshogi/common/utils.hpp"

namespace vshogi
{

template <class Config>
class KingMoveGenerator
{
private:
    using BitBoardType = BitBoard<Config>;
    using BoardType = Board<Config>;
    using MoveType = Move<Config>;
    using StateType = State<Config>;
    using Square = typename Config::Square;
    using SHelper = Squares<Config>;
    static constexpr auto SQ_NA = SHelper::SQ_NA; // NOLINT

private:
    const StateType* const m_state;
    const ColorEnum m_turn;
    const BoardType* const m_board;
    const Square m_src; //!< King square
    typename BitBoardType::SquareIterator m_iter;

public:
    KingMoveGenerator(const StateType* const state)
        : m_state(state), m_turn(state->get_turn()),
          m_board(&state->get_board()),
          m_src(m_board->get_king_location(m_turn)),
          m_iter((BitBoardType::get_attacks_by(m_board->get(m_src), m_src)
                  & (~m_board->get_occupied(m_turn)))
                     .square_iterator())
    {
        increment_iterator_while_square_is_attacked();
    }
    KingMoveGenerator& operator++()
    {
        ++m_iter;
        increment_iterator_while_square_is_attacked();
        return *this;
    }
    MoveType operator*() const
    {
        return MoveType(*m_iter, m_src, false);
    }
    KingMoveGenerator begin() const
    {
        return *this;
    }
    KingMoveGenerator end() const
    {
        static const auto end_iter = KingMoveGenerator();
        return end_iter;
    }
    bool operator!=(const KingMoveGenerator& other) const
    {
        return m_iter != other.m_iter;
    }

private:
    KingMoveGenerator()
        : m_state(nullptr), m_turn(), m_board(nullptr), m_src(SQ_NA),
          m_iter(BitBoardType().square_iterator())
    {
    }
    void increment_iterator_while_square_is_attacked()
    {
        while (!m_iter.is_end()) {
            if (m_board->is_square_attacked(~m_turn, *m_iter, m_src))
                ++m_iter;
            else
                break;
        }
    }
};

} // namespace vshogi

#endif // VSHOGI_COMMON_GENERATOR_HPP
