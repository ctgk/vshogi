#include "vshogi/variants/animal_shogi.hpp"

#include <CppUTest/TestHarness.h>

namespace test_vshogi::test_animal_shogi
{

using namespace vshogi::animal_shogi;

TEST_GROUP(animal_shogi_board){};

TEST(animal_shogi_board, apply)
{
    {
        auto b = Board();
        CHECK_EQUAL(SQ_B4, b.get_king_location(vshogi::BLACK));
        b.apply(SQ_B4, VOID);
        CHECK_EQUAL(SQ_NA, b.get_king_location(vshogi::BLACK));
        b.apply(SQ_C3, B_LI);
        CHECK_EQUAL(SQ_C3, b.get_king_location(vshogi::BLACK));
        CHECK_EQUAL(B_LI, b[SQ_C3]);
        CHECK_EQUAL(VOID, b[SQ_B4]);
    }
    {
        auto b = Board();
        CHECK_EQUAL(SQ_B4, b.get_king_location(vshogi::BLACK));
        b.apply(SQ_C3, SQ_B4);
        CHECK_EQUAL(SQ_C3, b.get_king_location(vshogi::BLACK));
        CHECK_EQUAL(B_LI, b[SQ_C3]);
        CHECK_EQUAL(VOID, b[SQ_B4]);
    }
}

TEST(animal_shogi_board, hflip)
{
    auto b = Board();
    b.apply(SQ_A4, SQ_B4);

    const auto b_hflip = b.hflip();

    CHECK_EQUAL(W_EL, b_hflip[SQ_A1]);
    CHECK_EQUAL(SQ_C4, b_hflip.get_king_location(vshogi::BLACK));
    CHECK_EQUAL(SQ_B1, b_hflip.get_king_location(vshogi::WHITE));
}

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
