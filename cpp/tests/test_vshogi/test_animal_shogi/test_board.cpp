#include "vshogi/variants/animal_shogi.hpp"

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

TEST(animal_shogi_board, append_sfen)
{
    {
        auto b = Board();
        auto actual = std::string();
        b.append_sfen(actual);
        STRCMP_EQUAL("gle/1c1/1C1/ELG", actual.c_str());
    }
}

} // namespace test_vshogi::test_animal_shogi
