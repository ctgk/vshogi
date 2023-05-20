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
    CHECK_EQUAL(W_GI, b[SQ_A1]);
    CHECK_EQUAL(W_CH, b[SQ_B2]);
    CHECK_EQUAL(VOID, b[SQ_C3]);
    CHECK_EQUAL(B_LI, b[SQ_B4]);
    CHECK_EQUAL('b', *actual);
}

TEST(animal_shogi_board, to_sfen)
{
    {
        auto b = Board();
        const auto actual = b.to_sfen();
        STRCMP_EQUAL("gle/1c1/1C1/ELG", actual.c_str());
    }
}

TEST(animal_shogi_board, init)
{
    //     A  B  C
    //   *--*--*--*
    // 1 |-G|-C|-E|
    //   *--*--*--*
    // 2 |+L|  |+C|
    //   *--*--*--*
    // 3 |  |-L|+G|
    //   *--*--*--*
    // 4 |+E|  |  |
    //   *--*--*--*
    auto b = Board::from_hash(185353356648959);
    CHECK_EQUAL(W_GI, b[SQ_A1]);
    CHECK_EQUAL(W_CH, b[SQ_B1]);
    CHECK_EQUAL(W_EL, b[SQ_C1]);
    CHECK_EQUAL(B_LI, b[SQ_A2]);
    CHECK_EQUAL(VOID, b[SQ_B2]);
    CHECK_EQUAL(B_CH, b[SQ_C2]);
    CHECK_EQUAL(VOID, b[SQ_A3]);
    CHECK_EQUAL(W_LI, b[SQ_B3]);
    CHECK_EQUAL(B_GI, b[SQ_C3]);
    CHECK_EQUAL(B_EL, b[SQ_A4]);
    CHECK_EQUAL(VOID, b[SQ_B4]);
    CHECK_EQUAL(VOID, b[SQ_C4]);
}

} // namespace test_vshogi::test_animal_shogi
