// #include "vshogi/judkins_shogi/board.hpp"

// #include <CppUTest/TestHarness.h>

// namespace test_vshogi::test_judkins_shogi
// {

// using namespace vshogi::judkins_shogi;

// TEST_GROUP(board){};

// TEST(board, get)
// {
//     auto b = Board();
//     CHECK_EQUAL(piece::W_HI, b[squares::SQ_6A]);
//     CHECK_EQUAL(piece::B_GI, b[squares::SQ_4F]);
//     CHECK_EQUAL(piece::VOID, b[squares::SQ_1D]);
// }

// TEST(board, set)
// {
//     auto b = Board();
//     CHECK_EQUAL(piece::VOID, b[squares::SQ_2D]);
//     b[squares::SQ_2D] = piece::W_GI;
//     CHECK_EQUAL(piece::W_GI, b[squares::SQ_2D]);
// }

// TEST(board, to_piece_mask)
// {
//     auto b = Board();
//     CHECK_TRUE(
//         (bb_6a | bb_5a | bb_4a | bb_3a | bb_2a | bb_1a | bb_1b | bb_6e | bb_6f
//          | bb_5f | bb_4f | bb_3f | bb_2f | bb_1f)
//         == b.to_piece_mask());
//     CHECK_TRUE(
//         (bb_6a | bb_5a | bb_4a | bb_3a | bb_2a | bb_1a | bb_1b)
//         == b.to_piece_mask(vshogi::WHITE));
//     CHECK_TRUE(
//         (bb_6f | bb_5f | bb_4f | bb_3f | bb_2f | bb_1f | bb_6e)
//         == b.to_piece_mask(vshogi::BLACK));
// }

// TEST(board, to_attack_mask)
// {
//     auto b = Board();
//     const auto actual = b.to_attack_mask(vshogi::BLACK);
//     const auto expected
//         = (bb_6b | bb_5c | bb_4d | bb_3e | bb_1e // KA
//            | bb_1b | bb_1c | bb_1d | bb_1e | bb_2f // HI
//            | bb_6d // FU
//            | bb_6e | bb_5e | bb_5f // OU
//            | bb_6e | bb_5e | bb_4e | bb_6f | bb_4f // KI
//            | bb_5e | bb_4e | bb_3e // GI
//            | bb_4d | bb_2d // KE
//         );
//     CHECK_EQUAL(expected.hamming_weight(), actual.hamming_weight());
//     CHECK_TRUE(expected == actual);
// }

// TEST(board, king_location)
// {
//     auto b = Board();
//     CHECK_EQUAL(squares::SQ_6F, b.king_location(vshogi::BLACK));
//     CHECK_EQUAL(squares::SQ_1A, b.king_location(vshogi::WHITE));
//     b[squares::SQ_6F] = piece::VOID;
//     b[squares::SQ_1A] = piece::VOID;
//     CHECK_EQUAL(squares::SQ_NA, b.king_location(vshogi::BLACK));
//     CHECK_EQUAL(squares::SQ_NA, b.king_location(vshogi::WHITE));
// }

// TEST(board, in_check)
// {
//     auto b = Board();
//     CHECK_FALSE(b.in_check(vshogi::BLACK));
//     CHECK_FALSE(b.in_check(vshogi::WHITE));
//     b[squares::SQ_1B] = piece::B_FU;
//     b[squares::SQ_6E] = piece::W_FU;
//     CHECK_TRUE(b.in_check(vshogi::BLACK));
//     CHECK_TRUE(b.in_check(vshogi::WHITE));
// }

// TEST(board, set_sfen)
// {
//     {
//         const char sfen[] = "+r+b+n+s+p+P/+S+N+B+RGg/6/5k/6/K5 b - 1";
//         auto b = Board();
//         const auto actual = b.set_sfen(sfen);
//         CHECK_EQUAL(piece::W_RY, b[squares::SQ_6A]);
//         CHECK_EQUAL(piece::W_UM, b[squares::SQ_5A]);
//         CHECK_EQUAL(piece::W_NK, b[squares::SQ_4A]);
//         CHECK_EQUAL(piece::W_NG, b[squares::SQ_3A]);
//         CHECK_EQUAL(piece::W_TO, b[squares::SQ_2A]);
//         CHECK_EQUAL(piece::B_TO, b[squares::SQ_1A]);
//         CHECK_EQUAL(piece::B_NG, b[squares::SQ_6B]);
//         CHECK_EQUAL(piece::B_NK, b[squares::SQ_5B]);
//         CHECK_EQUAL(piece::B_UM, b[squares::SQ_4B]);
//         CHECK_EQUAL(piece::B_RY, b[squares::SQ_3B]);
//         CHECK_EQUAL(piece::B_KI, b[squares::SQ_2B]);
//         CHECK_EQUAL(piece::W_KI, b[squares::SQ_1B]);
//         CHECK_EQUAL(piece::VOID, b[squares::SQ_6C]);
//         CHECK_EQUAL(piece::VOID, b[squares::SQ_5C]);
//         CHECK_EQUAL(piece::VOID, b[squares::SQ_4C]);
//         CHECK_EQUAL(piece::VOID, b[squares::SQ_3C]);
//         CHECK_EQUAL(piece::VOID, b[squares::SQ_2C]);
//         CHECK_EQUAL(piece::VOID, b[squares::SQ_1C]);
//         CHECK_EQUAL(piece::VOID, b[squares::SQ_6D]);
//         CHECK_EQUAL(piece::VOID, b[squares::SQ_5D]);
//         CHECK_EQUAL(piece::VOID, b[squares::SQ_4D]);
//         CHECK_EQUAL(piece::VOID, b[squares::SQ_3D]);
//         CHECK_EQUAL(piece::VOID, b[squares::SQ_2D]);
//         CHECK_EQUAL(piece::W_OU, b[squares::SQ_1D]);
//         CHECK_EQUAL(piece::VOID, b[squares::SQ_6E]);
//         CHECK_EQUAL(piece::VOID, b[squares::SQ_5E]);
//         CHECK_EQUAL(piece::VOID, b[squares::SQ_4E]);
//         CHECK_EQUAL(piece::VOID, b[squares::SQ_3E]);
//         CHECK_EQUAL(piece::VOID, b[squares::SQ_2E]);
//         CHECK_EQUAL(piece::VOID, b[squares::SQ_1E]);
//         CHECK_EQUAL(piece::B_OU, b[squares::SQ_6F]);
//         CHECK_EQUAL(piece::VOID, b[squares::SQ_5F]);
//         CHECK_EQUAL(piece::VOID, b[squares::SQ_4F]);
//         CHECK_EQUAL(piece::VOID, b[squares::SQ_3F]);
//         CHECK_EQUAL(piece::VOID, b[squares::SQ_2F]);
//         CHECK_EQUAL(piece::VOID, b[squares::SQ_1F]);
//         CHECK_EQUAL('b', actual[0]);
//         CHECK_EQUAL(' ', actual[1]);
//     }
//     {
//         const char sfen[] = "k4K/6/6/6/6/6 w 2R2B2G2S2N2P 1";
//         auto b = Board();
//         const auto actual = b.set_sfen(sfen);
//         CHECK_EQUAL(piece::W_OU, b[squares::SQ_6A]);
//         CHECK_EQUAL(piece::VOID, b[squares::SQ_5A]);
//         CHECK_EQUAL(piece::VOID, b[squares::SQ_4A]);
//         CHECK_EQUAL(piece::VOID, b[squares::SQ_3A]);
//         CHECK_EQUAL(piece::VOID, b[squares::SQ_2A]);
//         CHECK_EQUAL(piece::B_OU, b[squares::SQ_1A]);
//         CHECK_EQUAL(piece::VOID, b[squares::SQ_6B]);
//         CHECK_EQUAL(piece::VOID, b[squares::SQ_5B]);
//         CHECK_EQUAL(piece::VOID, b[squares::SQ_4B]);
//         CHECK_EQUAL(piece::VOID, b[squares::SQ_3B]);
//         CHECK_EQUAL(piece::VOID, b[squares::SQ_2B]);
//         CHECK_EQUAL(piece::VOID, b[squares::SQ_1B]);
//         CHECK_EQUAL(piece::VOID, b[squares::SQ_6C]);
//         CHECK_EQUAL(piece::VOID, b[squares::SQ_5C]);
//         CHECK_EQUAL(piece::VOID, b[squares::SQ_4C]);
//         CHECK_EQUAL(piece::VOID, b[squares::SQ_3C]);
//         CHECK_EQUAL(piece::VOID, b[squares::SQ_2C]);
//         CHECK_EQUAL(piece::VOID, b[squares::SQ_1C]);
//         CHECK_EQUAL(piece::VOID, b[squares::SQ_6D]);
//         CHECK_EQUAL(piece::VOID, b[squares::SQ_5D]);
//         CHECK_EQUAL(piece::VOID, b[squares::SQ_4D]);
//         CHECK_EQUAL(piece::VOID, b[squares::SQ_3D]);
//         CHECK_EQUAL(piece::VOID, b[squares::SQ_2D]);
//         CHECK_EQUAL(piece::VOID, b[squares::SQ_1D]);
//         CHECK_EQUAL(piece::VOID, b[squares::SQ_6E]);
//         CHECK_EQUAL(piece::VOID, b[squares::SQ_5E]);
//         CHECK_EQUAL(piece::VOID, b[squares::SQ_4E]);
//         CHECK_EQUAL(piece::VOID, b[squares::SQ_3E]);
//         CHECK_EQUAL(piece::VOID, b[squares::SQ_2E]);
//         CHECK_EQUAL(piece::VOID, b[squares::SQ_1E]);
//         CHECK_EQUAL(piece::VOID, b[squares::SQ_6F]);
//         CHECK_EQUAL(piece::VOID, b[squares::SQ_5F]);
//         CHECK_EQUAL(piece::VOID, b[squares::SQ_4F]);
//         CHECK_EQUAL(piece::VOID, b[squares::SQ_3F]);
//         CHECK_EQUAL(piece::VOID, b[squares::SQ_2F]);
//         CHECK_EQUAL(piece::VOID, b[squares::SQ_1F]);
//         CHECK_EQUAL('w', actual[0]);
//         CHECK_EQUAL(' ', actual[1]);
//     }
// }

// TEST(board, to_sfen)
// {
//     const char sfen[] = "+r+b+n+s+p+P/+S+N+B+RGg/6/5k/6/K5 b - 1";
//     auto b = Board();
//     b.set_sfen(sfen);

//     const char expected[] = "+r+b+n+s+p+P/+S+N+B+RGg/6/5k/6/K5";
//     STRCMP_EQUAL(expected, b.to_sfen().c_str());
// }

// } // namespace test_vshogi::test_judkins_shogi
