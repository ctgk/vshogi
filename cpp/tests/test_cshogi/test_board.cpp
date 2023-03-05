#include "cshogi.h"
#include "init.hpp"

#include <CppUTest/TestHarness.h>

TEST_GROUP(Board){};

TEST(Board, sfen)
{
    auto board = __Board();
    CHECK_EQUAL(
        "lnsgkgsnl/1r5b1/ppppppppp/9/9/9/PPPPPPPPP/1B5R1/LNSGKGSNL b - 1",
        board.toSFEN());
}

TEST_GROUP(LegalMoveList){};
TEST(LegalMoveList, size)
{
    initTable();
    auto board = __Board();
    auto move_list = __LegalMoveList(board);
    CHECK_EQUAL(30, move_list.size());
}
