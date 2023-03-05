#include <string>

#include "cshogi.h"
#include "init.hpp"

#include <CppUTest/TestHarness.h>

// clang-format off
TEST_GROUP(Board){
    int push_usi_moves_and_return_isDraw(const std::vector<std::string>& moves)
    {
        auto board = __Board();
        for (auto&& usi : moves) {
            const auto move = board.move_from_usi(usi);
            board.push(move);
        }
        return board.isDraw(2147483647);
    }
};
// clang-format on

TEST(Board, sfen)
{
    auto board = __Board();
    CHECK_EQUAL(
        "lnsgkgsnl/1r5b1/ppppppppp/9/9/9/PPPPPPPPP/1B5R1/LNSGKGSNL b - 1",
        board.toSFEN());
}

TEST(Board, draw)
{
    {
        // repetition win
        std::vector<std::string> moves = {
            // clang-format off
            "7g7f", "8c8d", "6g6f", "8d8e", "8h7g", "3c3d", "2h6h", "7a6b",
            "7i7h", "5a4b", "1g1f", "1c1d", "3i3h", "4b3b", "4g4f", "6a5b",
            "3g3f", "5c5d", "7h6g", "3a4b", "5i4h", "9c9d", "4h3i", "7c7d",
            "6i5h", "4b5c", "3i2h", "4a4b", "5g5f", "6b7c", "9i9h", "7c8d",
            "9g9f", "7d7e", "6h7h", "8b7b", "7g5i", "6c6d", "5i4h", "6d6e",
            "5h5g", "6e6f", "5g6f", "8e8f", "8g8f", "7e7f", "5f5e", "5d5e",
            "6g7f", "5e5f", "P*5e", "7b6b", "P*6e", "6b7b", "6f5f", "P*7e",
            "7f7e", "8d7e", "7h7e", "7b7e", "4h7e", "R*7i", "R*7a", "7i8i+",
            "7e5c+", "5b5c", "7a8a+", "S*4h", "N*2f", "4h4i+", "2f3d", "B*3i",
            "2h3g", "3i4h+", "3g2h", "4h3i", "2h3g", "3i4h", "3g2h", "4h3i",
            "2h3g", "3i4h", "3g2h", "4h3i", "2h3g", "3i4h",
            // clang-format on
        };
        const auto actual = push_usi_moves_and_return_isDraw(moves);
        CHECK_EQUAL(RepetitionWin, actual);
    }
    {
        // repetition lose
        std::vector<std::string> moves = {
            // clang-format off
            "7g7f", "4a5b", "2g2f", "3c3d", "2f2e", "2b8h+", "7i8h", "3a2b",
            "8h7g", "2b3c", "3i3h", "5a6b", "B*3b", "3c2b", "3b2a+", "6b7b",
            "2a2b", "9c9d", "2b1a", "5c5d", "1a3c", "1c1d", "5i6h", "6c6d",
            "L*4e", "6d6e", "N*6d", "7b6c", "6d5b+", "6a5b", "4e4c+", "5b6b",
            "3c2c", "7a7b", "S*7a", "8b9b", "2c4a", "6b5b", "4a5b", "6c6d",
            "5b5c", "6d7d", "5c7e", "7d6c", "7e5c", "6c7d", "5c7e", "7d6c",
            "7e5c", "6c7d", "5c7e", "7d6c", "7e5c", "6c7d",
            // clang-format on
        };
        const auto actual = push_usi_moves_and_return_isDraw(moves);
        CHECK_EQUAL(RepetitionLose, actual);
    }
}

TEST_GROUP(LegalMoveList){};
TEST(LegalMoveList, size)
{
    initTable();
    auto board = __Board();
    auto move_list = __LegalMoveList(board);
    CHECK_EQUAL(30, move_list.size());
}
