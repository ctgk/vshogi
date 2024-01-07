#ifndef TEST_VSHOGI_TEST_SHOGI_TEST_SHOGI_HPP
#define TEST_VSHOGI_TEST_SHOGI_TEST_SHOGI_HPP

#include "vshogi/shogi/move.hpp"
#include "vshogi/shogi/piece.hpp"
#include "vshogi/shogi/squares.hpp"

namespace test_vshogi::test_shogi
{

using PieceTypeEnum = vshogi::shogi::Pieces::PieceTypeEnum;
template <class T>
static auto is_promotable(const T p)
{
    return vshogi::shogi::Pieces::is_promotable(p);
}
template <class T>
static auto is_promoted(const T p)
{
    return vshogi::shogi::Pieces::is_promoted(p);
}
template <class T>
static auto promote(const T p)
{
    return vshogi::shogi::Pieces::promote(p);
}
template <class T>
static auto demote(const T p)
{
    return vshogi::shogi::Pieces::demote(p);
}
template <class T>
static auto to_piece_type(const T a)
{
    return vshogi::shogi::Pieces::to_piece_type(a);
}
static constexpr auto get_color = vshogi::shogi::Pieces::get_color;
static constexpr auto to_board_piece = vshogi::shogi::Pieces::to_board_piece;
static constexpr auto append_sfen = vshogi::shogi::Pieces::append_sfen;

using SquareEnum = vshogi::shogi::Squares::SquareEnum;
static constexpr auto to_rank = vshogi::shogi::Squares::to_rank;
static constexpr auto to_file = vshogi::shogi::Squares::to_file;
static constexpr auto shift = vshogi::shogi::Squares::shift;
static constexpr auto get_direction = vshogi::shogi::Squares::get_direction;
static constexpr auto num_squares = vshogi::shogi::Squares::num_squares;

} // namespace test_vshogi::test_shogi

#endif // TEST_VSHOGI_TEST_SHOGI_TEST_SHOGI_HPP
