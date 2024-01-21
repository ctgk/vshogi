#ifndef TEST_VSHOGI_TEST_MINISHOGI_TEST_MINISHOGI_HPP
#define TEST_VSHOGI_TEST_MINISHOGI_TEST_MINISHOGI_HPP

#include "vshogi/variants/minishogi.hpp"

namespace test_vshogi::test_minishogi
{

using PieceTypeEnum = vshogi::minishogi::Pieces::PieceTypeEnum;
template <class T>
static auto is_promotable(const T p)
{
    return vshogi::minishogi::Pieces::is_promotable(p);
}
template <class T>
static auto is_promoted(const T p)
{
    return vshogi::minishogi::Pieces::is_promoted(p);
}
template <class T>
static auto promote(const T p)
{
    return vshogi::minishogi::Pieces::promote(p);
}
template <class T>
static auto demote(const T p)
{
    return vshogi::minishogi::Pieces::demote(p);
}
template <class T>
static auto to_piece_type(const T a)
{
    return vshogi::minishogi::Pieces::to_piece_type(a);
}
static constexpr auto get_color = vshogi::minishogi::Pieces::get_color;
static constexpr auto to_board_piece
    = vshogi::minishogi::Pieces::to_board_piece;
static constexpr auto append_sfen = vshogi::minishogi::Pieces::append_sfen;

using SquareEnum = vshogi::minishogi::Squares::SquareEnum;
static constexpr auto to_rank = vshogi::minishogi::Squares::to_rank;
static constexpr auto to_file = vshogi::minishogi::Squares::to_file;
static constexpr auto shift = vshogi::minishogi::Squares::shift;
static constexpr auto num_squares = vshogi::minishogi::Squares::num_squares;

} // namespace test_vshogi::test_minishogi

#endif // TEST_VSHOGI_TEST_MINISHOGI_TEST_MINISHOGI_HPP
