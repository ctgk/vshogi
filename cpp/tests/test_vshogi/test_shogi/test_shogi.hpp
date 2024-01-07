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
static constexpr auto FU = vshogi::shogi::Pieces::FU; // NOLINT
static constexpr auto KY = vshogi::shogi::Pieces::KY; // NOLINT
static constexpr auto KE = vshogi::shogi::Pieces::KE; // NOLINT
static constexpr auto GI = vshogi::shogi::Pieces::GI; // NOLINT
static constexpr auto KI = vshogi::shogi::Pieces::KI; // NOLINT
static constexpr auto KA = vshogi::shogi::Pieces::KA; // NOLINT
static constexpr auto HI = vshogi::shogi::Pieces::HI; // NOLINT
static constexpr auto OU = vshogi::shogi::Pieces::OU; // NOLINT
static constexpr auto TO = vshogi::shogi::Pieces::TO; // NOLINT
static constexpr auto NY = vshogi::shogi::Pieces::NY; // NOLINT
static constexpr auto NK = vshogi::shogi::Pieces::NK; // NOLINT
static constexpr auto NG = vshogi::shogi::Pieces::NG; // NOLINT
static constexpr auto UM = vshogi::shogi::Pieces::UM; // NOLINT
static constexpr auto RY = vshogi::shogi::Pieces::RY; // NOLINT
static constexpr auto NA = vshogi::shogi::Pieces::NA; // NOLINT
static constexpr auto B_FU = vshogi::shogi::Pieces::B_FU; // NOLINT
static constexpr auto B_KY = vshogi::shogi::Pieces::B_KY; // NOLINT
static constexpr auto B_KE = vshogi::shogi::Pieces::B_KE; // NOLINT
static constexpr auto B_GI = vshogi::shogi::Pieces::B_GI; // NOLINT
static constexpr auto B_KI = vshogi::shogi::Pieces::B_KI; // NOLINT
static constexpr auto B_KA = vshogi::shogi::Pieces::B_KA; // NOLINT
static constexpr auto B_HI = vshogi::shogi::Pieces::B_HI; // NOLINT
static constexpr auto B_OU = vshogi::shogi::Pieces::B_OU; // NOLINT
static constexpr auto B_TO = vshogi::shogi::Pieces::B_TO; // NOLINT
static constexpr auto B_NY = vshogi::shogi::Pieces::B_NY; // NOLINT
static constexpr auto B_NK = vshogi::shogi::Pieces::B_NK; // NOLINT
static constexpr auto B_NG = vshogi::shogi::Pieces::B_NG; // NOLINT
static constexpr auto B_UM = vshogi::shogi::Pieces::B_UM; // NOLINT
static constexpr auto B_RY = vshogi::shogi::Pieces::B_RY; // NOLINT
static constexpr auto W_FU = vshogi::shogi::Pieces::W_FU; // NOLINT
static constexpr auto W_KY = vshogi::shogi::Pieces::W_KY; // NOLINT
static constexpr auto W_KE = vshogi::shogi::Pieces::W_KE; // NOLINT
static constexpr auto W_GI = vshogi::shogi::Pieces::W_GI; // NOLINT
static constexpr auto W_KI = vshogi::shogi::Pieces::W_KI; // NOLINT
static constexpr auto W_KA = vshogi::shogi::Pieces::W_KA; // NOLINT
static constexpr auto W_HI = vshogi::shogi::Pieces::W_HI; // NOLINT
static constexpr auto W_OU = vshogi::shogi::Pieces::W_OU; // NOLINT
static constexpr auto W_TO = vshogi::shogi::Pieces::W_TO; // NOLINT
static constexpr auto W_NY = vshogi::shogi::Pieces::W_NY; // NOLINT
static constexpr auto W_NK = vshogi::shogi::Pieces::W_NK; // NOLINT
static constexpr auto W_NG = vshogi::shogi::Pieces::W_NG; // NOLINT
static constexpr auto W_UM = vshogi::shogi::Pieces::W_UM; // NOLINT
static constexpr auto W_RY = vshogi::shogi::Pieces::W_RY; // NOLINT
static constexpr auto VOID = vshogi::shogi::Pieces::VOID; // NOLINT

using SquareEnum = vshogi::shogi::Squares::SquareEnum;
static constexpr auto to_rank = vshogi::shogi::Squares::to_rank;
static constexpr auto to_file = vshogi::shogi::Squares::to_file;
static constexpr auto shift = vshogi::shogi::Squares::shift;
static constexpr auto get_direction = vshogi::shogi::Squares::get_direction;
static constexpr auto num_squares = vshogi::shogi::Squares::num_squares;

} // namespace test_vshogi::test_shogi

#endif // TEST_VSHOGI_TEST_SHOGI_TEST_SHOGI_HPP
