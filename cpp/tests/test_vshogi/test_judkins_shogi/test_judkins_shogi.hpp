#ifndef TEST_VSHOGI_TEST_JUDKINS_SHOGI_TEST_JUDKINS_SHOGI_HPP
#define TEST_VSHOGI_TEST_JUDKINS_SHOGI_TEST_JUDKINS_SHOGI_HPP

#include "vshogi/judkins_shogi/piece.hpp"
#include "vshogi/judkins_shogi/squares.hpp"

namespace test_vshogi::test_judkins_shogi
{

template <class T>
static auto is_promotable(const T p)
{
    return vshogi::judkins_shogi::Pieces::is_promotable(p);
}
template <class T>
static auto is_promoted(const T p)
{
    return vshogi::judkins_shogi::Pieces::is_promoted(p);
}
template <class T>
static auto promote(const T p)
{
    return vshogi::judkins_shogi::Pieces::promote(p);
}
template <class T>
static auto demote(const T p)
{
    return vshogi::judkins_shogi::Pieces::demote(p);
}
template <class T>
static auto to_piece_type(const T p)
{
    return vshogi::judkins_shogi::Pieces::to_piece_type(p);
}
static constexpr auto get_color = vshogi::judkins_shogi::Pieces::get_color;
static constexpr auto to_board_piece
    = vshogi::judkins_shogi::Pieces::to_board_piece;
static constexpr auto append_sfen = vshogi::judkins_shogi::Pieces::append_sfen;

static constexpr auto shift = vshogi::judkins_shogi::Squares::shift;
static constexpr auto to_rank = vshogi::judkins_shogi::Squares::to_rank;
static constexpr auto to_file = vshogi::judkins_shogi::Squares::to_file;
static constexpr auto get_direction
    = vshogi::judkins_shogi::Squares::get_direction;
static constexpr auto num_squares = vshogi::judkins_shogi::Squares::num_squares;

} // namespace test_vshogi::test_judkins_shogi

#endif // TEST_VSHOGI_TEST_JUDKINS_SHOGI_TEST_JUDKINS_SHOGI_HPP
