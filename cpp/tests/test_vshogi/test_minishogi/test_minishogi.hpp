#ifndef TEST_VSHOGI_TEST_MINISHOGI_TEST_MINISHOGI_HPP
#define TEST_VSHOGI_TEST_MINISHOGI_TEST_MINISHOGI_HPP

#include "vshogi/minishogi/move.hpp"
#include "vshogi/minishogi/piece.hpp"
#include "vshogi/minishogi/squares.hpp"

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
static constexpr auto FU = vshogi::minishogi::Pieces::FU; // NOLINT
static constexpr auto GI = vshogi::minishogi::Pieces::GI; // NOLINT
static constexpr auto KI = vshogi::minishogi::Pieces::KI; // NOLINT
static constexpr auto KA = vshogi::minishogi::Pieces::KA; // NOLINT
static constexpr auto HI = vshogi::minishogi::Pieces::HI; // NOLINT
static constexpr auto OU = vshogi::minishogi::Pieces::OU; // NOLINT
static constexpr auto TO = vshogi::minishogi::Pieces::TO; // NOLINT
static constexpr auto NG = vshogi::minishogi::Pieces::NG; // NOLINT
static constexpr auto UM = vshogi::minishogi::Pieces::UM; // NOLINT
static constexpr auto RY = vshogi::minishogi::Pieces::RY; // NOLINT
static constexpr auto NA = vshogi::minishogi::Pieces::NA; // NOLINT
static constexpr auto B_FU = vshogi::minishogi::Pieces::B_FU; // NOLINT
static constexpr auto B_GI = vshogi::minishogi::Pieces::B_GI; // NOLINT
static constexpr auto B_KI = vshogi::minishogi::Pieces::B_KI; // NOLINT
static constexpr auto B_KA = vshogi::minishogi::Pieces::B_KA; // NOLINT
static constexpr auto B_HI = vshogi::minishogi::Pieces::B_HI; // NOLINT
static constexpr auto B_OU = vshogi::minishogi::Pieces::B_OU; // NOLINT
static constexpr auto B_TO = vshogi::minishogi::Pieces::B_TO; // NOLINT
static constexpr auto B_NG = vshogi::minishogi::Pieces::B_NG; // NOLINT
static constexpr auto B_UM = vshogi::minishogi::Pieces::B_UM; // NOLINT
static constexpr auto B_RY = vshogi::minishogi::Pieces::B_RY; // NOLINT
static constexpr auto W_FU = vshogi::minishogi::Pieces::W_FU; // NOLINT
static constexpr auto W_GI = vshogi::minishogi::Pieces::W_GI; // NOLINT
static constexpr auto W_KI = vshogi::minishogi::Pieces::W_KI; // NOLINT
static constexpr auto W_KA = vshogi::minishogi::Pieces::W_KA; // NOLINT
static constexpr auto W_HI = vshogi::minishogi::Pieces::W_HI; // NOLINT
static constexpr auto W_OU = vshogi::minishogi::Pieces::W_OU; // NOLINT
static constexpr auto W_TO = vshogi::minishogi::Pieces::W_TO; // NOLINT
static constexpr auto W_NG = vshogi::minishogi::Pieces::W_NG; // NOLINT
static constexpr auto W_UM = vshogi::minishogi::Pieces::W_UM; // NOLINT
static constexpr auto W_RY = vshogi::minishogi::Pieces::W_RY; // NOLINT
static constexpr auto VOID = vshogi::minishogi::Pieces::VOID; // NOLINT

using SquareEnum = vshogi::minishogi::Squares::SquareEnum;
static constexpr auto to_rank = vshogi::minishogi::Squares::to_rank;
static constexpr auto to_file = vshogi::minishogi::Squares::to_file;
static constexpr auto shift = vshogi::minishogi::Squares::shift;
static constexpr auto num_squares = vshogi::minishogi::Squares::num_squares;

} // namespace test_vshogi::test_minishogi

#endif // TEST_VSHOGI_TEST_MINISHOGI_TEST_MINISHOGI_HPP
