#include "vshogi/minishogi/game.hpp"

#include <CppUTest/TestHarness.h>

namespace test_vshogi::test_game
{

TEST_GROUP(minishogi_king_entering){};

TEST(minishogi_king_entering, black_win)
{
    using namespace vshogi::minishogi;
    auto game = Game("KgBGS/1R3/5/5/k4 b Prbsp");
    CHECK_EQUAL(vshogi::ONGOING, game.get_result());
    game.apply(Move(SQ_4A, SQ_4B));
    CHECK_EQUAL(vshogi::ONGOING, game.get_result());

    CHECK_TRUE(game.is_legal(Move(SQ_5D, SQ_5E)));
    game.apply(Move(SQ_5D, SQ_5E));
    CHECK_EQUAL(vshogi::BLACK_WIN, game.get_result());
}

TEST(minishogi_king_entering, white_win)
{
    using namespace vshogi::minishogi;
    // Turn: BLACK
    // White: KI
    //     5   4   3   2   1
    //   *---*---*---*---*---*
    // A |+OU|   |+KA|+KI|+GI|
    //   *---*---*---*---*---*
    // B |   |+HI|   |   |   |
    //   *---*---*---*---*---*
    // C |   |   |   |   |   |
    //   *---*---*---*---*---*
    // D |   |   |   |   |   |
    //   *---*---*---*---*---*
    // E |-OU|-HI|-KA|-GI|-TO|
    //   *---*---*---*---*---*
    // Black: FU
    const auto sfen = "K1BGS/1R3/5/5/krbs+p b Pg";
    const auto move = Move(SQ_4A, SQ_4B);

    auto game = Game(sfen);
    CHECK_EQUAL(vshogi::ONGOING, game.get_result());
    CHECK_TRUE(game.is_legal(move));
    game.apply(move);
    CHECK_EQUAL(vshogi::WHITE_WIN, game.get_result());
}

TEST(minishogi_king_entering, ongoing_because_of_check)
{
    using namespace vshogi::minishogi;
    // Turn: BLACK
    // White: KI
    //     5   4   3   2   1
    //   *---*---*---*---*---*
    // A |+OU|   |+KA|+KI|+GI|
    //   *---*---*---*---*---*
    // B |   |+HI|   |   |   |
    //   *---*---*---*---*---*
    // C |   |   |   |   |   |
    //   *---*---*---*---*---*
    // D |   |   |   |   |   |
    //   *---*---*---*---*---*
    // E |-OU|-HI|-KA|-GI|-TO|
    //   *---*---*---*---*---*
    // Black: FU
    const auto sfen = "K1BGS/1R3/5/5/krbs+p b Pg";
    const auto move = Move(SQ_5B, SQ_4B);

    auto game = Game(sfen);
    CHECK_EQUAL(vshogi::ONGOING, game.get_result());
    CHECK_TRUE(game.is_legal(move));
    game.apply(move);
    CHECK_EQUAL(vshogi::ONGOING, game.get_result());
}

TEST(minishogi_king_entering, ongoing_because_of_king_location)
{
    using namespace vshogi::minishogi;
    // Turn: BLACK
    // White: KI
    //     5   4   3   2   1
    //   *---*---*---*---*---*
    // A |+OU|   |+KA|+KI|+GI|
    //   *---*---*---*---*---*
    // B |   |+HI|   |   |   |
    //   *---*---*---*---*---*
    // C |   |   |   |   |   |
    //   *---*---*---*---*---*
    // D |-OU|   |   |   |   |
    //   *---*---*---*---*---*
    // E |   |-HI|-KA|-GI|-TO|
    //   *---*---*---*---*---*
    // Black: FU
    const auto sfen = "K1BGS/1R3/5/k4/1rbs+p b Pg";
    const auto move = Move(SQ_4A, SQ_4B);

    auto game = Game(sfen);
    CHECK_EQUAL(vshogi::ONGOING, game.get_result());
    CHECK_TRUE(game.is_legal(move));
    game.apply(move);
    CHECK_EQUAL(vshogi::ONGOING, game.get_result());
}

TEST(minishogi_king_entering, ongoing_because_of_num_pieces_in_the_zone)
{
    using namespace vshogi::minishogi;
    // Turn: BLACK
    // White: KI, GI, FU
    //     5   4   3   2   1
    //   *---*---*---*---*---*
    // A |+OU|   |+KA|+KI|+GI|
    //   *---*---*---*---*---*
    // B |   |+HI|   |   |   |
    //   *---*---*---*---*---*
    // C |   |   |   |   |   |
    //   *---*---*---*---*---*
    // D |   |   |   |   |   |
    //   *---*---*---*---*---*
    // E |-OU|-HI|-KA|   |   |
    //   *---*---*---*---*---*
    // Black: FU
    const auto sfen = "K1BGS/1R3/5/5/krb2 b Pgsp";
    const auto move = Move(SQ_4A, SQ_4B);

    auto game = Game(sfen);
    CHECK_EQUAL(vshogi::ONGOING, game.get_result());
    CHECK_TRUE(game.is_legal(move));
    game.apply(move);
    CHECK_EQUAL(vshogi::ONGOING, game.get_result());
}

TEST(minishogi_king_entering, ongoing_because_of_point)
{
    using namespace vshogi::minishogi;
    // Turn: BLACK
    // White:
    //     5   4   3   2   1
    //   *---*---*---*---*---*
    // A |+OU|-KI|+KA|+KI|+GI|
    //   *---*---*---*---*---*
    // B |   |+HI|   |   |   |
    //   *---*---*---*---*---*
    // C |   |   |   |   |   |
    //   *---*---*---*---*---*
    // D |   |   |   |   |   |
    //   *---*---*---*---*---*
    // E |-OU|-HI|-KA|-GI|-TO|
    //   *---*---*---*---*---*
    // Black: FU
    const auto sfen = "KgBGS/1R3/5/5/krbs+p b P";
    const auto move = Move(SQ_4A, SQ_4B);

    auto game = Game(sfen);
    CHECK_EQUAL(vshogi::ONGOING, game.get_result());
    CHECK_TRUE(game.is_legal(move));
    game.apply(move);
    CHECK_EQUAL(vshogi::ONGOING, game.get_result());
}

} // namespace test_vshogi::test_game
