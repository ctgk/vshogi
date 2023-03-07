#include "vshogi/animal_shogi/board.hpp"
#include "vshogi/animal_shogi/state.hpp"

#include <CppUTest/TestHarness.h>

namespace test_vshogi::test_animal_shogi
{

using namespace vshogi::animal_shogi;

TEST_GROUP(animal_shogi_board){};

TEST(animal_shogi_board, set_sfen)
{
    const char sfen[] = "gle/1c1/1C1/ELG b - 1";
    auto b = Board();
    const auto actual = b.set_sfen(sfen);
    CHECK_EQUAL(W_GI, b.get_piece_at(SQ_A1));
    CHECK_EQUAL(W_CH, b.get_piece_at(SQ_B2));
    CHECK_EQUAL(VOID, b.get_piece_at(SQ_C3));
    CHECK_EQUAL(B_LI, b.get_piece_at(SQ_B4));
    CHECK_EQUAL('b', *actual);
}

} // namespace test_vshogi::test_animal_shogi
