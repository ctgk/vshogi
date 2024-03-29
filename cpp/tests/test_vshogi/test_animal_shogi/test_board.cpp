#include "vshogi/animal_shogi/board.hpp"
#include "vshogi/animal_shogi/state.hpp"

#include <CppUTest/TestHarness.h>

namespace test_vshogi::test_animal_shogi
{

using namespace vshogi::animal_shogi;
static constexpr auto B_CH = Pieces::B_CH; // NOLINT
static constexpr auto B_EL = Pieces::B_EL; // NOLINT
static constexpr auto B_GI = Pieces::B_GI; // NOLINT
static constexpr auto B_LI = Pieces::B_LI; // NOLINT
static constexpr auto B_HE = Pieces::B_HE; // NOLINT
static constexpr auto W_CH = Pieces::W_CH; // NOLINT
static constexpr auto W_EL = Pieces::W_EL; // NOLINT
static constexpr auto W_GI = Pieces::W_GI; // NOLINT
static constexpr auto W_LI = Pieces::W_LI; // NOLINT
static constexpr auto W_HE = Pieces::W_HE; // NOLINT
static constexpr auto VOID = Pieces::VOID; // NOLINT
static constexpr auto SQ_A4 = Squares::SQ_A4; // NOLINT
static constexpr auto SQ_A3 = Squares::SQ_A3; // NOLINT
static constexpr auto SQ_A2 = Squares::SQ_A2; // NOLINT
static constexpr auto SQ_A1 = Squares::SQ_A1; // NOLINT
static constexpr auto SQ_B4 = Squares::SQ_B4; // NOLINT
static constexpr auto SQ_B3 = Squares::SQ_B3; // NOLINT
static constexpr auto SQ_B2 = Squares::SQ_B2; // NOLINT
static constexpr auto SQ_B1 = Squares::SQ_B1; // NOLINT
static constexpr auto SQ_C4 = Squares::SQ_C4; // NOLINT
static constexpr auto SQ_C3 = Squares::SQ_C3; // NOLINT
static constexpr auto SQ_C2 = Squares::SQ_C2; // NOLINT
static constexpr auto SQ_C1 = Squares::SQ_C1; // NOLINT

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

} // namespace test_vshogi::test_animal_shogi
