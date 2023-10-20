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
static constexpr auto get_color = vshogi::judkins_shogi::Pieces::get_color;
static constexpr auto to_piece_type
    = vshogi::judkins_shogi::Pieces::to_piece_type;
static constexpr auto to_board_piece
    = vshogi::judkins_shogi::Pieces::to_board_piece;
static constexpr auto append_sfen = vshogi::judkins_shogi::Pieces::append_sfen;
static constexpr auto FU = vshogi::judkins_shogi::Pieces::FU; // NOLINT
static constexpr auto KE = vshogi::judkins_shogi::Pieces::KE; // NOLINT
static constexpr auto GI = vshogi::judkins_shogi::Pieces::GI; // NOLINT
static constexpr auto KI = vshogi::judkins_shogi::Pieces::KI; // NOLINT
static constexpr auto KA = vshogi::judkins_shogi::Pieces::KA; // NOLINT
static constexpr auto HI = vshogi::judkins_shogi::Pieces::HI; // NOLINT
static constexpr auto OU = vshogi::judkins_shogi::Pieces::OU; // NOLINT
static constexpr auto TO = vshogi::judkins_shogi::Pieces::TO; // NOLINT
static constexpr auto NK = vshogi::judkins_shogi::Pieces::NK; // NOLINT
static constexpr auto NG = vshogi::judkins_shogi::Pieces::NG; // NOLINT
static constexpr auto UM = vshogi::judkins_shogi::Pieces::UM; // NOLINT
static constexpr auto RY = vshogi::judkins_shogi::Pieces::RY; // NOLINT
static constexpr auto NA = vshogi::judkins_shogi::Pieces::NA; // NOLINT
static constexpr auto B_FU = vshogi::judkins_shogi::Pieces::B_FU; // NOLINT
static constexpr auto B_KE = vshogi::judkins_shogi::Pieces::B_KE; // NOLINT
static constexpr auto B_GI = vshogi::judkins_shogi::Pieces::B_GI; // NOLINT
static constexpr auto B_KI = vshogi::judkins_shogi::Pieces::B_KI; // NOLINT
static constexpr auto B_KA = vshogi::judkins_shogi::Pieces::B_KA; // NOLINT
static constexpr auto B_HI = vshogi::judkins_shogi::Pieces::B_HI; // NOLINT
static constexpr auto B_OU = vshogi::judkins_shogi::Pieces::B_OU; // NOLINT
static constexpr auto B_TO = vshogi::judkins_shogi::Pieces::B_TO; // NOLINT
static constexpr auto B_NK = vshogi::judkins_shogi::Pieces::B_NK; // NOLINT
static constexpr auto B_NG = vshogi::judkins_shogi::Pieces::B_NG; // NOLINT
static constexpr auto B_UM = vshogi::judkins_shogi::Pieces::B_UM; // NOLINT
static constexpr auto B_RY = vshogi::judkins_shogi::Pieces::B_RY; // NOLINT
static constexpr auto W_FU = vshogi::judkins_shogi::Pieces::W_FU; // NOLINT
static constexpr auto W_KE = vshogi::judkins_shogi::Pieces::W_KE; // NOLINT
static constexpr auto W_GI = vshogi::judkins_shogi::Pieces::W_GI; // NOLINT
static constexpr auto W_KI = vshogi::judkins_shogi::Pieces::W_KI; // NOLINT
static constexpr auto W_KA = vshogi::judkins_shogi::Pieces::W_KA; // NOLINT
static constexpr auto W_HI = vshogi::judkins_shogi::Pieces::W_HI; // NOLINT
static constexpr auto W_OU = vshogi::judkins_shogi::Pieces::W_OU; // NOLINT
static constexpr auto W_TO = vshogi::judkins_shogi::Pieces::W_TO; // NOLINT
static constexpr auto W_NK = vshogi::judkins_shogi::Pieces::W_NK; // NOLINT
static constexpr auto W_NG = vshogi::judkins_shogi::Pieces::W_NG; // NOLINT
static constexpr auto W_UM = vshogi::judkins_shogi::Pieces::W_UM; // NOLINT
static constexpr auto W_RY = vshogi::judkins_shogi::Pieces::W_RY; // NOLINT
static constexpr auto VOID = vshogi::judkins_shogi::Pieces::VOID; // NOLINT

static constexpr auto shift = vshogi::judkins_shogi::Squares::shift;
static constexpr auto to_rank = vshogi::judkins_shogi::Squares::to_rank;
static constexpr auto to_file = vshogi::judkins_shogi::Squares::to_file;
template <class T>
static auto is_edge(const T a)
{
    return vshogi::judkins_shogi::Squares::is_edge(a);
}
static constexpr auto get_direction
    = vshogi::judkins_shogi::Squares::get_direction;
static constexpr auto num_squares = vshogi::judkins_shogi::Squares::num_squares;
static constexpr auto RANK1 = vshogi::judkins_shogi::Squares::RANK1; // NOLINT
static constexpr auto RANK2 = vshogi::judkins_shogi::Squares::RANK2; // NOLINT
static constexpr auto RANK3 = vshogi::judkins_shogi::Squares::RANK3; // NOLINT
static constexpr auto RANK4 = vshogi::judkins_shogi::Squares::RANK4; // NOLINT
static constexpr auto RANK5 = vshogi::judkins_shogi::Squares::RANK5; // NOLINT
static constexpr auto RANK6 = vshogi::judkins_shogi::Squares::RANK6; // NOLINT
static constexpr auto FILE1 = vshogi::judkins_shogi::Squares::FILE1; // NOLINT
static constexpr auto FILE2 = vshogi::judkins_shogi::Squares::FILE2; // NOLINT
static constexpr auto FILE3 = vshogi::judkins_shogi::Squares::FILE3; // NOLINT
static constexpr auto FILE4 = vshogi::judkins_shogi::Squares::FILE4; // NOLINT
static constexpr auto FILE5 = vshogi::judkins_shogi::Squares::FILE5; // NOLINT
static constexpr auto FILE6 = vshogi::judkins_shogi::Squares::FILE6; // NOLINT
static constexpr auto SQ_6A = vshogi::judkins_shogi::Squares::SQ_6A; // NOLINT
static constexpr auto SQ_5A = vshogi::judkins_shogi::Squares::SQ_5A; // NOLINT
static constexpr auto SQ_4A = vshogi::judkins_shogi::Squares::SQ_4A; // NOLINT
static constexpr auto SQ_3A = vshogi::judkins_shogi::Squares::SQ_3A; // NOLINT
static constexpr auto SQ_2A = vshogi::judkins_shogi::Squares::SQ_2A; // NOLINT
static constexpr auto SQ_1A = vshogi::judkins_shogi::Squares::SQ_1A; // NOLINT
static constexpr auto SQ_6B = vshogi::judkins_shogi::Squares::SQ_6B; // NOLINT
static constexpr auto SQ_5B = vshogi::judkins_shogi::Squares::SQ_5B; // NOLINT
static constexpr auto SQ_4B = vshogi::judkins_shogi::Squares::SQ_4B; // NOLINT
static constexpr auto SQ_3B = vshogi::judkins_shogi::Squares::SQ_3B; // NOLINT
static constexpr auto SQ_2B = vshogi::judkins_shogi::Squares::SQ_2B; // NOLINT
static constexpr auto SQ_1B = vshogi::judkins_shogi::Squares::SQ_1B; // NOLINT
static constexpr auto SQ_6C = vshogi::judkins_shogi::Squares::SQ_6C; // NOLINT
static constexpr auto SQ_5C = vshogi::judkins_shogi::Squares::SQ_5C; // NOLINT
static constexpr auto SQ_4C = vshogi::judkins_shogi::Squares::SQ_4C; // NOLINT
static constexpr auto SQ_3C = vshogi::judkins_shogi::Squares::SQ_3C; // NOLINT
static constexpr auto SQ_2C = vshogi::judkins_shogi::Squares::SQ_2C; // NOLINT
static constexpr auto SQ_1C = vshogi::judkins_shogi::Squares::SQ_1C; // NOLINT
static constexpr auto SQ_6D = vshogi::judkins_shogi::Squares::SQ_6D; // NOLINT
static constexpr auto SQ_5D = vshogi::judkins_shogi::Squares::SQ_5D; // NOLINT
static constexpr auto SQ_4D = vshogi::judkins_shogi::Squares::SQ_4D; // NOLINT
static constexpr auto SQ_3D = vshogi::judkins_shogi::Squares::SQ_3D; // NOLINT
static constexpr auto SQ_2D = vshogi::judkins_shogi::Squares::SQ_2D; // NOLINT
static constexpr auto SQ_1D = vshogi::judkins_shogi::Squares::SQ_1D; // NOLINT
static constexpr auto SQ_6E = vshogi::judkins_shogi::Squares::SQ_6E; // NOLINT
static constexpr auto SQ_5E = vshogi::judkins_shogi::Squares::SQ_5E; // NOLINT
static constexpr auto SQ_4E = vshogi::judkins_shogi::Squares::SQ_4E; // NOLINT
static constexpr auto SQ_3E = vshogi::judkins_shogi::Squares::SQ_3E; // NOLINT
static constexpr auto SQ_2E = vshogi::judkins_shogi::Squares::SQ_2E; // NOLINT
static constexpr auto SQ_1E = vshogi::judkins_shogi::Squares::SQ_1E; // NOLINT
static constexpr auto SQ_6F = vshogi::judkins_shogi::Squares::SQ_6F; // NOLINT
static constexpr auto SQ_5F = vshogi::judkins_shogi::Squares::SQ_5F; // NOLINT
static constexpr auto SQ_4F = vshogi::judkins_shogi::Squares::SQ_4F; // NOLINT
static constexpr auto SQ_3F = vshogi::judkins_shogi::Squares::SQ_3F; // NOLINT
static constexpr auto SQ_2F = vshogi::judkins_shogi::Squares::SQ_2F; // NOLINT
static constexpr auto SQ_1F = vshogi::judkins_shogi::Squares::SQ_1F; // NOLINT
static constexpr auto SQ_NA = vshogi::judkins_shogi::Squares::SQ_NA; // NOLINT

} // namespace test_vshogi::test_judkins_shogi

#endif // TEST_VSHOGI_TEST_JUDKINS_SHOGI_TEST_JUDKINS_SHOGI_HPP
