#include "vshogi/minishogi/game.hpp"

#include <CppUTest/TestHarness.h>

namespace test_vshogi::test_game
{

TEST_GROUP(minishogi_king_entering){};

TEST(minishogi_king_entering, from_default_start_position)
{
    using namespace vshogi::minishogi;
    auto game = Game();
    game.apply(Move(SQ_1B, SQ_1E));
    game.apply(Move(SQ_1B, SQ_1A));
    game.apply(Move(SQ_5B, SQ_2E));
    game.apply(Move(SQ_5B, SQ_4A));
    game.apply(Move(SQ_5C, SQ_5D));
    game.apply(Move(SQ_2E, SQ_5B, true));
    game.apply(Move(SQ_5B, SQ_5C));
    game.apply(Move(SQ_1C, SQ_1B));
    game.apply(Move(SQ_5A, SQ_5B, true));
    game.apply(Move(SQ_1D, SQ_1C));
    game.apply(Move(SQ_5D, SQ_5E));
    game.apply(Move(SQ_1E, SQ_1D));
    game.apply(Move(SQ_4D, SQ_3E));
    game.apply(Move(SQ_3E, KA));
    game.apply(Move(SQ_5C, SQ_5D));
    game.apply(Move(SQ_5E, HI));
    CHECK_EQUAL(vshogi::ONGOING, game.get_result());
    game.apply(Move(SQ_5D, SQ_4E));

    // Turn: WHITE
    // White: -
    //     5   4   3   2   1
    //   *---*---*---*---*---*
    // A |+TO|   |-GI|-KI|   |
    //   *---*---*---*---*---*
    // B |   |   |   |   |   |
    //   *---*---*---*---*---*
    // C |+OU|   |   |   |   |
    //   *---*---*---*---*---*
    // D |+KI|+GI|   |   |   |
    //   *---*---*---*---*---*
    // E |-HI|   |-KA|-UM|-OU|
    //   *---*---*---*---*---*
    // Black: FU,HI
    CHECK_EQUAL(vshogi::WHITE_WIN, game.get_result());
}

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

TEST_GROUP(minishogi_is_check_move){};

TEST(minishogi_is_check_move, check_by_moving_piece)
{
    using namespace vshogi::minishogi;
    // Turn: BLACK
    // White: -
    //     5   4   3   2   1
    //   *---*---*---*---*---*
    // A |-HI|-KA|-GI|-KI|-OU|
    //   *---*---*---*---*---*
    // B |   |   |   |   |-FU|
    //   *---*---*---*---*---*
    // C |   |   |   |   |   |
    //   *---*---*---*---*---*
    // D |+FU|   |   |   |   |
    //   *---*---*---*---*---*
    // E |+OU|+KI|+GI|+KA|+HI|
    //   *---*---*---*---*---*
    // Black: -
    auto game = Game();
    CHECK_TRUE(game.is_check_move(Move(SQ_1B, SQ_1E)));
    CHECK_FALSE(game.is_check_move(Move(SQ_1C, SQ_1E)));
}

TEST(minishogi_is_check_move, check_by_discovered_piece)
{
    using namespace vshogi::minishogi;
    // Turn: WHITE
    // White: -
    //     5   4   3   2   1
    //   *---*---*---*---*---*
    // A |   |   |   |   |-OU|
    //   *---*---*---*---*---*
    // B |   |   |   |   |   |
    //   *---*---*---*---*---*
    // C |   |   |   |   |+KI|
    //   *---*---*---*---*---*
    // D |   |   |   |   |   |
    //   *---*---*---*---*---*
    // E |+OU|   |   |   |+HI|
    //   *---*---*---*---*---*
    // Black: -
    auto game = Game("4k/5/4G/5/K3R b -");
    CHECK_TRUE(game.is_check_move(Move(SQ_2C, SQ_1C)));
    CHECK_FALSE(game.is_check_move(Move(SQ_1D, SQ_1C)));
}

} // namespace test_vshogi::test_game
