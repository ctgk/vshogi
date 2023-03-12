#include "vshogi/animal_shogi/state.hpp"

#include <CppUTest/TestHarness.h>

namespace test_vshogi::test_animal_shogi
{

using namespace vshogi::animal_shogi;

TEST_GROUP(animal_shogi_state){};

TEST(animal_shogi_state, get_turn)
{
    {
        const auto s = State();
        CHECK_EQUAL(BLACK, s.get_turn());
    }
    {
        const auto s = State(default_initial_sfen);
        CHECK_EQUAL(BLACK, s.get_turn());
    }
    {
        const auto s = State("gle/1c1/1C1/ELG w - 1");
        CHECK_EQUAL(WHITE, s.get_turn());
    }
}

TEST(animal_shogi_state, apply)
{
    {
        auto s = State();
        s.apply(Move(SQ_B2, MS_B3));

        CHECK_EQUAL(B_CH, s.get_board().get_piece_at(SQ_B2)); // destination
        CHECK_EQUAL(VOID, s.get_board().get_piece_at(SQ_B3)); // source
        CHECK_EQUAL(1, s.get_piece_stand(BLACK).count(CH));
        CHECK_EQUAL(0, s.get_piece_stand(BLACK).count(EL));
        CHECK_EQUAL(0, s.get_piece_stand(BLACK).count(GI));
        CHECK_EQUAL(0, s.get_piece_stand(WHITE).count(CH));
        CHECK_EQUAL(0, s.get_piece_stand(WHITE).count(EL));
        CHECK_EQUAL(0, s.get_piece_stand(WHITE).count(GI));
        CHECK_EQUAL(WHITE, s.get_turn());
    }
    {
        // test promotion (Chick -> Hen)
        auto s = State("lge/1C1/1c1/EGL b - 1");
        s.apply(Move(SQ_B1, MS_B2));

        CHECK_EQUAL(B_HE, s.get_board().get_piece_at(SQ_B1)); // destination
        CHECK_EQUAL(VOID, s.get_board().get_piece_at(SQ_B2)); // source
        CHECK_EQUAL(0, s.get_piece_stand(BLACK).count(CH));
        CHECK_EQUAL(0, s.get_piece_stand(BLACK).count(EL));
        CHECK_EQUAL(1, s.get_piece_stand(BLACK).count(GI));
        CHECK_EQUAL(0, s.get_piece_stand(WHITE).count(CH));
        CHECK_EQUAL(0, s.get_piece_stand(WHITE).count(EL));
        CHECK_EQUAL(0, s.get_piece_stand(WHITE).count(GI));
        CHECK_EQUAL(WHITE, s.get_turn());
    }
}

TEST(animal_shogi_state, is_applicable)
{
    {
        CHECK_TRUE(State().is_applicable(Move(SQ_B2, MS_B3)));
        CHECK_FALSE(State().is_applicable(Move(SQ_B3, MS_B2)));
    }
}

TEST(animal_shogi_state, hash)
{
    {
        const auto actual = State("gle/1c1/1C1/ELG b - 1").hash();
        CHECK_TRUE(((actual >> 50) & 0x1) == BLACK);
    }
    {
        const auto actual = State("gle/1c1/1C1/ELG w - 1").hash();
        CHECK_TRUE(((actual >> 50) & 0x1) == WHITE);
    }
    {
        const auto a = State("gle/1c1/1C1/ELG b - 1").hash();
        const auto b = State("gle/1c1/1C1/ELG w - 1").hash();
        CHECK_EQUAL(a & 0x0ffffffffffff, b & 0x0ffffffffffff);
    }
}

} // namespace test_vshogi::test_animal_shogi
