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
static constexpr auto get_color = vshogi::minishogi::Pieces::get_color;
static constexpr auto to_piece_type = vshogi::minishogi::Pieces::to_piece_type;
static constexpr auto to_board_piece
    = vshogi::minishogi::Pieces::to_board_piece;
static constexpr auto to_sfen = vshogi::minishogi::Pieces::to_sfen;
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
template <class T>
static auto is_edge(const T a)
{
    return vshogi::minishogi::Squares::is_edge(a);
}
static constexpr auto to_rank = vshogi::minishogi::Squares::to_rank;
static constexpr auto to_file = vshogi::minishogi::Squares::to_file;
static constexpr auto shift = vshogi::minishogi::Squares::shift;
static constexpr auto num_squares = vshogi::minishogi::Squares::num_squares;
static constexpr auto RANK1 = vshogi::minishogi::Squares::RANK1; // NOLINT
static constexpr auto RANK2 = vshogi::minishogi::Squares::RANK2; // NOLINT
static constexpr auto RANK3 = vshogi::minishogi::Squares::RANK3; // NOLINT
static constexpr auto RANK4 = vshogi::minishogi::Squares::RANK4; // NOLINT
static constexpr auto RANK5 = vshogi::minishogi::Squares::RANK5; // NOLINT
static constexpr auto FILE1 = vshogi::minishogi::Squares::FILE1; // NOLINT
static constexpr auto FILE2 = vshogi::minishogi::Squares::FILE2; // NOLINT
static constexpr auto FILE3 = vshogi::minishogi::Squares::FILE3; // NOLINT
static constexpr auto FILE4 = vshogi::minishogi::Squares::FILE4; // NOLINT
static constexpr auto FILE5 = vshogi::minishogi::Squares::FILE5; // NOLINT
static constexpr auto SQ_5A = vshogi::minishogi::Squares::SQ_5A; // NOLINT
static constexpr auto SQ_4A = vshogi::minishogi::Squares::SQ_4A; // NOLINT
static constexpr auto SQ_3A = vshogi::minishogi::Squares::SQ_3A; // NOLINT
static constexpr auto SQ_2A = vshogi::minishogi::Squares::SQ_2A; // NOLINT
static constexpr auto SQ_1A = vshogi::minishogi::Squares::SQ_1A; // NOLINT
static constexpr auto SQ_5B = vshogi::minishogi::Squares::SQ_5B; // NOLINT
static constexpr auto SQ_4B = vshogi::minishogi::Squares::SQ_4B; // NOLINT
static constexpr auto SQ_3B = vshogi::minishogi::Squares::SQ_3B; // NOLINT
static constexpr auto SQ_2B = vshogi::minishogi::Squares::SQ_2B; // NOLINT
static constexpr auto SQ_1B = vshogi::minishogi::Squares::SQ_1B; // NOLINT
static constexpr auto SQ_5C = vshogi::minishogi::Squares::SQ_5C; // NOLINT
static constexpr auto SQ_4C = vshogi::minishogi::Squares::SQ_4C; // NOLINT
static constexpr auto SQ_3C = vshogi::minishogi::Squares::SQ_3C; // NOLINT
static constexpr auto SQ_2C = vshogi::minishogi::Squares::SQ_2C; // NOLINT
static constexpr auto SQ_1C = vshogi::minishogi::Squares::SQ_1C; // NOLINT
static constexpr auto SQ_5D = vshogi::minishogi::Squares::SQ_5D; // NOLINT
static constexpr auto SQ_4D = vshogi::minishogi::Squares::SQ_4D; // NOLINT
static constexpr auto SQ_3D = vshogi::minishogi::Squares::SQ_3D; // NOLINT
static constexpr auto SQ_2D = vshogi::minishogi::Squares::SQ_2D; // NOLINT
static constexpr auto SQ_1D = vshogi::minishogi::Squares::SQ_1D; // NOLINT
static constexpr auto SQ_5E = vshogi::minishogi::Squares::SQ_5E; // NOLINT
static constexpr auto SQ_4E = vshogi::minishogi::Squares::SQ_4E; // NOLINT
static constexpr auto SQ_3E = vshogi::minishogi::Squares::SQ_3E; // NOLINT
static constexpr auto SQ_2E = vshogi::minishogi::Squares::SQ_2E; // NOLINT
static constexpr auto SQ_1E = vshogi::minishogi::Squares::SQ_1E; // NOLINT
static constexpr auto SQ_NA = vshogi::minishogi::Squares::SQ_NA; // NOLINT

} // namespace test_vshogi::test_minishogi

#endif // TEST_VSHOGI_TEST_MINISHOGI_TEST_MINISHOGI_HPP
