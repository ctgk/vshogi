#include "vshogi/engine/dfpn.hpp"
#include "vshogi/variants/judkins_shogi.hpp"
#include "vshogi/variants/minishogi.hpp"
#include "vshogi/variants/shogi.hpp"

#include <CppUTest/TestHarness.h>

namespace test_vshogi::test_engine
{

TEST_GROUP (dfpn) {
};

TEST(dfpn, no_mate_no_check)
{
    using namespace vshogi::minishogi;

    // Turn: BLACK
    // White: -
    //     5   4   3   2   1
    //   *---*---*---*---*---*
    // A |   |   |-OU|   |   |
    //   *---*---*---*---*---*
    // B |   |   |   |   |   |
    //   *---*---*---*---*---*
    // C |   |   |   |   |   |
    //   *---*---*---*---*---*
    // D |   |   |   |   |   |
    //   *---*---*---*---*---*
    // E |   |   |+OU|   |   |
    //   *---*---*---*---*---*
    // Black: -
    auto searcher = vshogi::engine::dfpn::Searcher<Config>();
    searcher.set_game(Game("2k2/5/5/5/2K2 b -"));
    CHECK_TRUE(searcher.found_conclusion());
    CHECK_FALSE(searcher.found_mate());
    CHECK_TRUE(searcher.found_no_mate());
    CHECK_FALSE(searcher.search(1u));
    CHECK_TRUE(searcher.found_conclusion());
    CHECK_FALSE(searcher.found_mate());
    CHECK_TRUE(searcher.found_no_mate());
}

TEST(dfpn, no_mate)
{
    using namespace vshogi::minishogi;
    using Searcher = vshogi::engine::dfpn::Searcher<Config>;

    // Turn: BLACK
    // White: -
    //     5   4   3   2   1
    //   *---*---*---*---*---*
    // A |   |   |-OU|   |   |
    //   *---*---*---*---*---*
    // B |   |   |   |   |   |
    //   *---*---*---*---*---*
    // C |   |+TO|   |   |   |
    //   *---*---*---*---*---*
    // D |   |   |   |   |   |
    //   *---*---*---*---*---*
    // E |   |   |   |   |   |
    //   *---*---*---*---*---*
    // Black: GIx2
    auto searcher = Searcher();
    searcher.set_game(Game("2k2/5/1+P3/5/5 b 2S"));
    searcher.search(800);
    CHECK_TRUE(searcher.found_conclusion());
    CHECK_FALSE(searcher.found_mate());
    CHECK_TRUE(searcher.found_no_mate());
}

TEST(dfpn, no_mate_1)
{
    using namespace vshogi::shogi;
    using Searcher = vshogi::engine::dfpn::Searcher<Config>;
    // Turn: WHITE
    // White: FUx3,KY,KE,GI
    //     9   8   7   6   5   4   3   2   1
    //   +---+---+---+---+---+---+---+---+---+
    // A |-KY|   |   |   |   |   |-KI|   |-KY|
    //   +---+---+---+---+---+---+---+---+---+
    // B |   |   |   |   |   |   |-KI|-OU|   |
    //   +---+---+---+---+---+---+---+---+---+
    // C |   |-FU|   |   |   |   |-KE|-FU|   |
    //   +---+---+---+---+---+---+---+---+---+
    // D |   |   |   |+RY|   |-FU|-FU|   |-FU|
    //   +---+---+---+---+---+---+---+---+---+
    // E |   |   |-FU|   |-FU|   |   |+FU|   |
    //   +---+---+---+---+---+---+---+---+---+
    // F |-FU|   |   |   |   |+KE|+FU|   |+FU|
    //   +---+---+---+---+---+---+---+---+---+
    // G |   |+FU|   |+GI|   |   |   |   |   |
    //   +---+---+---+---+---+---+---+---+---+
    // H |+FU|+KI|   |+GI|+KA|   |-TO|   |   |
    //   +---+---+---+---+---+---+---+---+---+
    // I |   |+KE|+OU|   |+KA|   |   |-RY|+KY|
    //   +---+---+---+---+---+---+---+---+---+
    // Black: FU,GI,KI
    auto g = Game("l5g1l/6gk1/1p4np1/3+R1pp1p/2p1p2P1/p4NP1P/1P1S5/PG1SB1+p2/"
                  "1NK1B2+rL w GSPsnl3p 134");
    auto searcher = Searcher();
    searcher.set_game(g);
    CHECK_FALSE(searcher.search(100));
    CHECK_TRUE(searcher.found_no_mate());
}

TEST(dfpn, mate_in_one_straight_forward)
{
    using namespace vshogi::minishogi;
    using Searcher = vshogi::engine::dfpn::Searcher<Config>;
    auto g = Game("3rk/3b1/4P/4R/4K b -");

    auto searcher = Searcher();
    CHECK_FALSE(searcher.is_ready());
    searcher.set_game(g);
    CHECK_TRUE(searcher.is_ready());

    CHECK_TRUE(searcher.search(1u));
    CHECK_EQUAL(Move(SQ_1B, SQ_1C).hash(), searcher.get_mate_move().hash());
}

TEST(dfpn, mate_in_one)
{
    using namespace vshogi::minishogi;
    using Searcher = vshogi::engine::dfpn::Searcher<Config>;
    auto searcher = Searcher();

    {
        // Turn: BLACK
        // White: -
        //     5   4   3   2   1
        //   *---*---*---*---*---*
        // A |   |   |-OU|   |   |
        //   *---*---*---*---*---*
        // B |   |   |   |   |   |
        //   *---*---*---*---*---*
        // C |   |   |+KI|+KA|   |
        //   *---*---*---*---*---*
        // D |   |   |   |   |   |
        //   *---*---*---*---*---*
        // E |   |   |+OU|   |   |
        //   *---*---*---*---*---*
        // Black: -
        searcher.set_game(Game("2k2/5/2GB1/5/2K2 b -"));
        CHECK_TRUE(searcher.search(10u));
        CHECK_TRUE(searcher.found_mate());
        CHECK_EQUAL(Move(SQ_3B, SQ_3C).hash(), searcher.get_mate_move().hash());
    }
    {
        // DISCOVERED CHECK

        // Turn: BLACK
        // White: -
        //     5   4   3   2   1
        //   *---*---*---*---*---*
        // A |   |   |   |-KA|   |
        //   *---*---*---*---*---*
        // B |   |   |-HI|   |-OU|
        //   *---*---*---*---*---*
        // C |   |   |   |-FU|+KA|
        //   *---*---*---*---*---*
        // D |   |   |   |-KI|+HI|
        //   *---*---*---*---*---*
        // E |   |   |   |   |   |
        //   *---*---*---*---*---*
        // Black: KI
        searcher.set_game(Game("3b1/2r1k/3pB/3gR/5 b G"));
        CHECK_TRUE(searcher.search(50));
    }
}

TEST(dfpn, mate_in_three_straight_forward)
{
    using namespace vshogi::minishogi;
    using Searcher = vshogi::engine::dfpn::Searcher<Config>;

    // Turn: BLACK
    // White: -
    //     5   4   3   2   1
    //   *---*---*---*---*---*
    // A |   |   |   |-KA|-OU|
    //   *---*---*---*---*---*
    // B |   |   |   |-GI|   |
    //   *---*---*---*---*---*
    // C |   |   |   |+KA|+FU|
    //   *---*---*---*---*---*
    // D |   |   |   |   |+RY|
    //   *---*---*---*---*---*
    // E |   |   |   |   |+OU|
    //   *---*---*---*---*---*
    // Black: HI
    auto g = Game("3bk/3s1/3BP/4+R/4K b -");
    auto searcher = vshogi::engine::dfpn::Searcher<Config>();
    searcher.set_game(g);
    CHECK_FALSE(searcher.search(1u));
    CHECK_FALSE(searcher.search(1u));
    CHECK_TRUE(searcher.search(1u));
    CHECK_EQUAL(Move(SQ_1B, SQ_1C).hash(), searcher.get_mate_move().hash());
}

TEST(dfpn, mate_in_three)
{
    using namespace vshogi::minishogi;
    auto searcher = vshogi::engine::dfpn::Searcher<Config>();
    {
        // Turn: White
        // White: KIx2
        //     5   4   3   2   1
        //   *---*---*---*---*---*
        // A |   |   |   |   |   |
        //   *---*---*---*---*---*
        // B |   |   |-FU|   |   |
        //   *---*---*---*---*---*
        // C |   |   |   |   |   |
        //   *---*---*---*---*---*
        // D |   |   |+OU|   |   |
        //   *---*---*---*---*---*
        // E |   |   |   |   |   |
        //   *---*---*---*---*---*
        // Black: -
        searcher.set_game(Game("5/2p2/5/2K2/5 w 2g"));
        CHECK_TRUE(searcher.search(20));
        CHECK_TRUE(searcher.found_mate());
        CHECK_EQUAL(Move(SQ_3C, KI).hash(), searcher.get_mate_move().hash());
    }
    {
        // Turn: BLACK
        // White: -
        //     5   4   3   2   1
        //   *---*---*---*---*---*
        // A |   |   |-GI|-KI|-OU|
        //   *---*---*---*---*---*
        // B |   |   |   |   |   |
        //   *---*---*---*---*---*
        // C |   |   |   |+HI|+KI|
        //   *---*---*---*---*---*
        // D |   |   |   |   |   |
        //   *---*---*---*---*---*
        // E |   |   |   |   |+OU|
        //   *---*---*---*---*---*
        // Black: HI
        searcher.set_game(Game("2sgk/5/3RG/5/4K b R"));
        CHECK_TRUE(searcher.search(300));
        CHECK_TRUE(searcher.found_conclusion());
        CHECK_TRUE(searcher.found_mate());
        CHECK_EQUAL(Move(SQ_1B, SQ_1C).hash(), searcher.get_mate_move().hash());
    }
}

TEST(dfpn, mate_in_three_by_king_move)
{
    using namespace vshogi::judkins_shogi;
    using Searcher = vshogi::engine::dfpn::Searcher<Config>;
    auto searcher = Searcher();
    {
        // Turn: BLACK
        // White: FU
        //     6   5   4   3   2   1
        //   +---+---+---+---+---+---+
        // A |   |   |   |+HI|-KE|-HI|
        //   +---+---+---+---+---+---+
        // B |   |   |   |   |   |-OU|
        //   +---+---+---+---+---+---+
        // C |   |-UM|   |   |   |-FU|
        //   +---+---+---+---+---+---+
        // D |   |   |   |+OU|   |   |
        //   +---+---+---+---+---+---+
        // E |   |   |+KA|   |   |-KI|
        //   +---+---+---+---+---+---+
        // F |   |   |   |-KI|   |   |
        //   +---+---+---+---+---+---+
        // Black: -
        searcher.set_game(Game("3Rnr/5k/1+b3p/3K2/2B2g/3g2 b p"));
        searcher.search(100);
        CHECK_TRUE(searcher.found_conclusion());
        CHECK_TRUE(searcher.found_mate());
        CHECK_EQUAL(Move(SQ_3B, SQ_3A).hash(), searcher.get_mate_move().hash());
    }
    {
        // COUNTER CHECK

        // Turn: BLACK
        // White: KE
        //     6   5   4   3   2   1
        //   +---+---+---+---+---+---+
        // A |   |   |   |+HI|-KE|-HI|
        //   +---+---+---+---+---+---+
        // B |   |   |   |   |   |-OU|
        //   +---+---+---+---+---+---+
        // C |   |-UM|   |   |   |-FU|
        //   +---+---+---+---+---+---+
        // D |   |   |   |+OU|   |   |
        //   +---+---+---+---+---+---+
        // E |   |   |+KA|   |   |   |
        //   +---+---+---+---+---+---+
        // F |   |   |   |   |   |   |
        //   +---+---+---+---+---+---+
        // Black: -
        searcher.set_game(Game("3Rnr/5k/1+b3p/3K2/2B3/6 b n"));
        CHECK_TRUE(searcher.search(100));
    }
}

TEST(dfpn, mate_in_five_straight_forward)
{
    using namespace vshogi::judkins_shogi;

    // Turn: BLACK
    // White: -
    //     6   5   4   3   2   1
    //   +---+---+---+---+---+---+
    // A |   |   |   |   |-FU|-FU|
    //   +---+---+---+---+---+---+
    // B |   |   |   |-FU|-KA|-OU|
    //   +---+---+---+---+---+---+
    // C |   |   |   |-KE|   |+KE|
    //   +---+---+---+---+---+---+
    // D |   |   |   |+FU|   |+FU|
    //   +---+---+---+---+---+---+
    // E |   |   |+GI|   |+GI|+HI|
    //   +---+---+---+---+---+---+
    // F |   |   |   |+KI|+HI|+KI|
    //   +---+---+---+---+---+---+
    // Black: KA
    auto searcher = vshogi::engine::dfpn::Searcher<Config>();
    searcher.set_game(Game("4pp/3pbk/3n1N/3P1P/2S1SR/3GRG b B"));
    CHECK_FALSE(searcher.search(4u));
    CHECK_TRUE(searcher.search(1u));
    CHECK_EQUAL(Move(SQ_2C, KA).hash(), searcher.get_mate_move().hash());
}

TEST(dfpn, mate_in_five)
{
    using namespace vshogi::minishogi;

    // Turn: BLACK
    // White: FU
    //     5   4   3   2   1
    //   *---*---*---*---*---*
    // A |   |   |-FU|-OU|-KA|
    //   *---*---*---*---*---*
    // B |   |   |   |   |+HI|
    //   *---*---*---*---*---*
    // C |   |   |-UM|+KI|   |
    //   *---*---*---*---*---*
    // D |   |   |   |   |   |
    //   *---*---*---*---*---*
    // E |   |   |   |   |   |
    //   *---*---*---*---*---*
    // Black: GIx2
    auto searcher = vshogi::engine::dfpn::Searcher<Config>();
    searcher.set_game(Game("2pkb/4R/2+bG1/5/5 b 2S"));
    CHECK_TRUE(searcher.search(800));
    CHECK_EQUAL(Move(SQ_2B, GI).hash(), searcher.get_mate_move().hash());
}

TEST(dfpn, king_entering_before_mate)
{
    using namespace vshogi::judkins_shogi;
    using Searcher = vshogi::engine::dfpn::Searcher<Config>;
    // Turn: WHITE
    // White: GI
    //     6   5   4   3   2   1
    //   +---+---+---+---+---+---+
    // A |   |   |   |   |   |   |
    //   +---+---+---+---+---+---+
    // B |   |   |   |   |   |   |
    //   +---+---+---+---+---+---+
    // C |   |   |   |-KE|   |   |
    //   +---+---+---+---+---+---+
    // D |   |   |   |   |   |   |
    //   +---+---+---+---+---+---+
    // E |+FU|   |   |   |-KI|-RY|
    //   +---+---+---+---+---+---+
    // F |   |+OU|   |-OU|-TO|-UM|
    //   +---+---+---+---+---+---+
    // Black: -
    auto searcher = Searcher();
    searcher.set_game(Game("6/6/3n2/6/P3g+r/1K1k+p+b w s"));
    CHECK_TRUE(searcher.search(100));
    CHECK_TRUE(searcher.found_mate());
    const auto actual = searcher.get_mate_moves();
    CHECK_EQUAL(2, actual.size());
    CHECK_TRUE(actual[0] == Move(SQ_4E, SQ_3C, true));
    CHECK_TRUE(actual[1] == Move(SQ_6F, SQ_5F));
}

TEST(dfpn, debug)
{
    using namespace vshogi::shogi;
    using Searcher = vshogi::engine::dfpn::Searcher<Config>;
    // Turn: BLACK
    // White: FUx3,KI
    //     9   8   7   6   5   4   3   2   1
    //   +---+---+---+---+---+---+---+---+---+
    // A |   |-KA|   |-KE|   |   |   |   |-OU|
    //   +---+---+---+---+---+---+---+---+---+
    // B |-KY|   |-GI|   |   |   |-FU|   |-GI|
    //   +---+---+---+---+---+---+---+---+---+
    // C |-FU|-HI|-FU|   |   |-FU|-KE|   |   |
    //   +---+---+---+---+---+---+---+---+---+
    // D |   |-FU|   |+UM|-FU|   |   |   |+HI|
    //   +---+---+---+---+---+---+---+---+---+
    // E |   |   |   |   |+FU|   |   |   |   |
    //   +---+---+---+---+---+---+---+---+---+
    // F |   |   |   |   |   |   |+KI|-FU|   |
    //   +---+---+---+---+---+---+---+---+---+
    // G |+FU|+FU|+FU|+FU|   |+FU|+KE|   |   |
    //   +---+---+---+---+---+---+---+---+---+
    // H |   |   |   |   |+OU|+KI|+GI|   |   |
    //   +---+---+---+---+---+---+---+---+---+
    // I |+KY|+KE|+GI|   |   |+KI|   |   |+KY|
    //   +---+---+---+---+---+---+---+---+---+
    // Black: FUx2,KY
    auto g = Game("1b1n4k/l1s3p1s/prp2pn2/1p1+Bp3R/4P4/6Gp1/PPPP1PN2/4KGS2/"
                  "LNS2G2L b L2Pg3p");
    auto searcher = Searcher();
    searcher.set_game(g);
    searcher.search(100);
    const auto actual = searcher.get_mate_moves();
    for (auto&& m : actual) {
        g.apply(m);
    }
    CHECK_TRUE(g.get_result() == vshogi::BLACK_WIN);
}

TEST(dfpn, debug2)
{
    using namespace vshogi::shogi;
    using Searcher = vshogi::engine::dfpn::Searcher<Config>;
    auto g = Game("ln5nl/3rgkgs1/1pp1pp1pp/p3s1p2/3P3Pb/P1P1P4/1PBSSPP1P/"
                  "2G1G2R1/LN2K2NL b P 29");
    auto searcher = Searcher();
    searcher.set_game(g);
    searcher.search(100);
    CHECK_TRUE(searcher.found_conclusion());
    CHECK_TRUE(searcher.found_no_mate());
}

// TEST(dfpn, mate_moves_without_waste_moves)
// {
//     using namespace vshogi::shogi;
//     using Searcher = vshogi::engine::dfpn::Searcher<Config>;
//     // Turn: BLACK
//     // White: -
//     //     9   8   7   6   5   4   3   2   1
//     //   +---+---+---+---+---+---+---+---+---+
//     // A |   |   |   |   |   |   |   |   |   |
//     //   +---+---+---+---+---+---+---+---+---+
//     // B |   |   |   |   |   |   |   |   |   |
//     //   +---+---+---+---+---+---+---+---+---+
//     // C |   |   |   |   |   |   |-KE|-FU|   |
//     //   +---+---+---+---+---+---+---+---+---+
//     // D |   |   |   |   |   |   |-FU|   |-FU|
//     //   +---+---+---+---+---+---+---+---+---+
//     // E |   |   |   |   |   |   |   |-OU|   |
//     //   +---+---+---+---+---+---+---+---+---+
//     // F |   |   |   |   |   |   |+FU|   |   |
//     //   +---+---+---+---+---+---+---+---+---+
//     // G |   |   |   |   |   |   |   |+FU|+FU|
//     //   +---+---+---+---+---+---+---+---+---+
//     // H |   |   |   |   |   |   |+HI|   |   |
//     //   +---+---+---+---+---+---+---+---+---+
//     // I |   |   |   |   |   |+OU|   |   |+KY|
//     //   +---+---+---+---+---+---+---+---+---+
//     // Black: KY,KA
//     auto g = Game("9/9/6np1/6p1p/7k1/6P2/7PP/6R2/5K2L b BL");
//     auto searcher = Searcher();
//     searcher.set_game(g);
//     searcher.search(10);
//     // Mate moves with waste moves: ['L*2f', '2e1e', 'B*2d', '2c2d', '1g1f']
//     // Mate moves w/o waste moves: ['L*2f', '2e1e', '1g1f']
//     const auto actual = searcher.get_mate_moves();
//     CHECK_EQUAL(3, actual.size());
// }

// TEST(dfpn, cache)
// {
//     using namespace vshogi::minishogi;
//     using Searcher = vshogi::engine::dfpn::Searcher<Config>;

//     // Turn: White
//     // White: KIx2
//     //     5   4   3   2   1
//     //   *---*---*---*---*---*
//     // A |   |   |   |   |   |
//     //   *---*---*---*---*---*
//     // B |   |   |-FU|   |   |
//     //   *---*---*---*---*---*
//     // C |   |   |   |   |   |
//     //   *---*---*---*---*---*
//     // D |   |   |+OU|   |   |
//     //   *---*---*---*---*---*
//     // E |   |   |   |   |   |
//     //   *---*---*---*---*---*
//     // Black: -
//     auto searcher = Searcher();
//     searcher.set_game(Game("5/2p2/5/2K2/5 w 2g"));
//     searcher.explore(21);
//     CHECK_FALSE(searcher.found_mate());
//     searcher.explore(100);
//     CHECK_TRUE(searcher.found_mate());

//     /**
//      * @brief It should only take 7 searches to prove checkmate.
//      *
//      * - W: G*3c
//      *      - B: 3d4e
//      *          - W: G*4d
//      *      - B: 3d3e
//      *          - W: G*3d
//      *      - B: 3d2e
//      *          - W: G*2d
//      */
//     searcher.set_game(Game("5/2p2/5/2K2/5 w 2g"));
//     CHECK_FALSE(searcher.found_mate());
//     searcher.explore(6);
//     CHECK_FALSE(searcher.found_mate());
//     searcher.explore(1);
//     CHECK_TRUE(searcher.found_mate());

//     const auto actual = searcher.get_mate_moves();
//     CHECK_EQUAL(3, actual.size());
//     CHECK_TRUE(Move(SQ_3C, KI) == actual[0]);
// }

} // namespace test_vshogi::test_engine
