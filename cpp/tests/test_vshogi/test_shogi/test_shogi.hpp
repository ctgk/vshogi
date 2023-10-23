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
template <class T>
static auto is_edge(const T a)
{
    return vshogi::shogi::Squares::is_edge(a);
}
static constexpr auto to_rank = vshogi::shogi::Squares::to_rank;
static constexpr auto to_file = vshogi::shogi::Squares::to_file;
static constexpr auto shift = vshogi::shogi::Squares::shift;
static constexpr auto get_direction = vshogi::shogi::Squares::get_direction;
static constexpr auto num_squares = vshogi::shogi::Squares::num_squares;
static constexpr auto RANK1 = vshogi::shogi::Squares::RANK1; // NOLINT
static constexpr auto RANK2 = vshogi::shogi::Squares::RANK2; // NOLINT
static constexpr auto RANK3 = vshogi::shogi::Squares::RANK3; // NOLINT
static constexpr auto RANK4 = vshogi::shogi::Squares::RANK4; // NOLINT
static constexpr auto RANK5 = vshogi::shogi::Squares::RANK5; // NOLINT
static constexpr auto RANK6 = vshogi::shogi::Squares::RANK6; // NOLINT
static constexpr auto RANK7 = vshogi::shogi::Squares::RANK7; // NOLINT
static constexpr auto RANK8 = vshogi::shogi::Squares::RANK8; // NOLINT
static constexpr auto RANK9 = vshogi::shogi::Squares::RANK9; // NOLINT
static constexpr auto FILE1 = vshogi::shogi::Squares::FILE1; // NOLINT
static constexpr auto FILE2 = vshogi::shogi::Squares::FILE2; // NOLINT
static constexpr auto FILE3 = vshogi::shogi::Squares::FILE3; // NOLINT
static constexpr auto FILE4 = vshogi::shogi::Squares::FILE4; // NOLINT
static constexpr auto FILE5 = vshogi::shogi::Squares::FILE5; // NOLINT
static constexpr auto FILE6 = vshogi::shogi::Squares::FILE6; // NOLINT
static constexpr auto FILE7 = vshogi::shogi::Squares::FILE7; // NOLINT
static constexpr auto FILE8 = vshogi::shogi::Squares::FILE8; // NOLINT
static constexpr auto FILE9 = vshogi::shogi::Squares::FILE9; // NOLINT
static constexpr auto SQ_9A = vshogi::shogi::Squares::SQ_9A; // NOLINT
static constexpr auto SQ_8A = vshogi::shogi::Squares::SQ_8A; // NOLINT
static constexpr auto SQ_7A = vshogi::shogi::Squares::SQ_7A; // NOLINT
static constexpr auto SQ_6A = vshogi::shogi::Squares::SQ_6A; // NOLINT
static constexpr auto SQ_5A = vshogi::shogi::Squares::SQ_5A; // NOLINT
static constexpr auto SQ_4A = vshogi::shogi::Squares::SQ_4A; // NOLINT
static constexpr auto SQ_3A = vshogi::shogi::Squares::SQ_3A; // NOLINT
static constexpr auto SQ_2A = vshogi::shogi::Squares::SQ_2A; // NOLINT
static constexpr auto SQ_1A = vshogi::shogi::Squares::SQ_1A; // NOLINT
static constexpr auto SQ_9B = vshogi::shogi::Squares::SQ_9B; // NOLINT
static constexpr auto SQ_8B = vshogi::shogi::Squares::SQ_8B; // NOLINT
static constexpr auto SQ_7B = vshogi::shogi::Squares::SQ_7B; // NOLINT
static constexpr auto SQ_6B = vshogi::shogi::Squares::SQ_6B; // NOLINT
static constexpr auto SQ_5B = vshogi::shogi::Squares::SQ_5B; // NOLINT
static constexpr auto SQ_4B = vshogi::shogi::Squares::SQ_4B; // NOLINT
static constexpr auto SQ_3B = vshogi::shogi::Squares::SQ_3B; // NOLINT
static constexpr auto SQ_2B = vshogi::shogi::Squares::SQ_2B; // NOLINT
static constexpr auto SQ_1B = vshogi::shogi::Squares::SQ_1B; // NOLINT
static constexpr auto SQ_9C = vshogi::shogi::Squares::SQ_9C; // NOLINT
static constexpr auto SQ_8C = vshogi::shogi::Squares::SQ_8C; // NOLINT
static constexpr auto SQ_7C = vshogi::shogi::Squares::SQ_7C; // NOLINT
static constexpr auto SQ_6C = vshogi::shogi::Squares::SQ_6C; // NOLINT
static constexpr auto SQ_5C = vshogi::shogi::Squares::SQ_5C; // NOLINT
static constexpr auto SQ_4C = vshogi::shogi::Squares::SQ_4C; // NOLINT
static constexpr auto SQ_3C = vshogi::shogi::Squares::SQ_3C; // NOLINT
static constexpr auto SQ_2C = vshogi::shogi::Squares::SQ_2C; // NOLINT
static constexpr auto SQ_1C = vshogi::shogi::Squares::SQ_1C; // NOLINT
static constexpr auto SQ_9D = vshogi::shogi::Squares::SQ_9D; // NOLINT
static constexpr auto SQ_8D = vshogi::shogi::Squares::SQ_8D; // NOLINT
static constexpr auto SQ_7D = vshogi::shogi::Squares::SQ_7D; // NOLINT
static constexpr auto SQ_6D = vshogi::shogi::Squares::SQ_6D; // NOLINT
static constexpr auto SQ_5D = vshogi::shogi::Squares::SQ_5D; // NOLINT
static constexpr auto SQ_4D = vshogi::shogi::Squares::SQ_4D; // NOLINT
static constexpr auto SQ_3D = vshogi::shogi::Squares::SQ_3D; // NOLINT
static constexpr auto SQ_2D = vshogi::shogi::Squares::SQ_2D; // NOLINT
static constexpr auto SQ_1D = vshogi::shogi::Squares::SQ_1D; // NOLINT
static constexpr auto SQ_9E = vshogi::shogi::Squares::SQ_9E; // NOLINT
static constexpr auto SQ_8E = vshogi::shogi::Squares::SQ_8E; // NOLINT
static constexpr auto SQ_7E = vshogi::shogi::Squares::SQ_7E; // NOLINT
static constexpr auto SQ_6E = vshogi::shogi::Squares::SQ_6E; // NOLINT
static constexpr auto SQ_5E = vshogi::shogi::Squares::SQ_5E; // NOLINT
static constexpr auto SQ_4E = vshogi::shogi::Squares::SQ_4E; // NOLINT
static constexpr auto SQ_3E = vshogi::shogi::Squares::SQ_3E; // NOLINT
static constexpr auto SQ_2E = vshogi::shogi::Squares::SQ_2E; // NOLINT
static constexpr auto SQ_1E = vshogi::shogi::Squares::SQ_1E; // NOLINT
static constexpr auto SQ_9F = vshogi::shogi::Squares::SQ_9F; // NOLINT
static constexpr auto SQ_8F = vshogi::shogi::Squares::SQ_8F; // NOLINT
static constexpr auto SQ_7F = vshogi::shogi::Squares::SQ_7F; // NOLINT
static constexpr auto SQ_6F = vshogi::shogi::Squares::SQ_6F; // NOLINT
static constexpr auto SQ_5F = vshogi::shogi::Squares::SQ_5F; // NOLINT
static constexpr auto SQ_4F = vshogi::shogi::Squares::SQ_4F; // NOLINT
static constexpr auto SQ_3F = vshogi::shogi::Squares::SQ_3F; // NOLINT
static constexpr auto SQ_2F = vshogi::shogi::Squares::SQ_2F; // NOLINT
static constexpr auto SQ_1F = vshogi::shogi::Squares::SQ_1F; // NOLINT
static constexpr auto SQ_9G = vshogi::shogi::Squares::SQ_9G; // NOLINT
static constexpr auto SQ_8G = vshogi::shogi::Squares::SQ_8G; // NOLINT
static constexpr auto SQ_7G = vshogi::shogi::Squares::SQ_7G; // NOLINT
static constexpr auto SQ_6G = vshogi::shogi::Squares::SQ_6G; // NOLINT
static constexpr auto SQ_5G = vshogi::shogi::Squares::SQ_5G; // NOLINT
static constexpr auto SQ_4G = vshogi::shogi::Squares::SQ_4G; // NOLINT
static constexpr auto SQ_3G = vshogi::shogi::Squares::SQ_3G; // NOLINT
static constexpr auto SQ_2G = vshogi::shogi::Squares::SQ_2G; // NOLINT
static constexpr auto SQ_1G = vshogi::shogi::Squares::SQ_1G; // NOLINT
static constexpr auto SQ_9H = vshogi::shogi::Squares::SQ_9H; // NOLINT
static constexpr auto SQ_8H = vshogi::shogi::Squares::SQ_8H; // NOLINT
static constexpr auto SQ_7H = vshogi::shogi::Squares::SQ_7H; // NOLINT
static constexpr auto SQ_6H = vshogi::shogi::Squares::SQ_6H; // NOLINT
static constexpr auto SQ_5H = vshogi::shogi::Squares::SQ_5H; // NOLINT
static constexpr auto SQ_4H = vshogi::shogi::Squares::SQ_4H; // NOLINT
static constexpr auto SQ_3H = vshogi::shogi::Squares::SQ_3H; // NOLINT
static constexpr auto SQ_2H = vshogi::shogi::Squares::SQ_2H; // NOLINT
static constexpr auto SQ_1H = vshogi::shogi::Squares::SQ_1H; // NOLINT
static constexpr auto SQ_9I = vshogi::shogi::Squares::SQ_9I; // NOLINT
static constexpr auto SQ_8I = vshogi::shogi::Squares::SQ_8I; // NOLINT
static constexpr auto SQ_7I = vshogi::shogi::Squares::SQ_7I; // NOLINT
static constexpr auto SQ_6I = vshogi::shogi::Squares::SQ_6I; // NOLINT
static constexpr auto SQ_5I = vshogi::shogi::Squares::SQ_5I; // NOLINT
static constexpr auto SQ_4I = vshogi::shogi::Squares::SQ_4I; // NOLINT
static constexpr auto SQ_3I = vshogi::shogi::Squares::SQ_3I; // NOLINT
static constexpr auto SQ_2I = vshogi::shogi::Squares::SQ_2I; // NOLINT
static constexpr auto SQ_1I = vshogi::shogi::Squares::SQ_1I; // NOLINT
static constexpr auto SQ_NA = vshogi::shogi::Squares::SQ_NA; // NOLINT

} // namespace test_vshogi::test_shogi

#endif // TEST_VSHOGI_TEST_SHOGI_TEST_SHOGI_HPP
