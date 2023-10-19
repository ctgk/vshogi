#include <CppUTest/CommandLineTestRunner.h>

#include "vshogi/judkins_shogi/bitboard.hpp"
#include "vshogi/minishogi/bitboard.hpp"
#include "vshogi/shogi/bitboard.hpp"

int main(int argc, char* argv[])
{
    vshogi::minishogi::init_non_ranging_attacks_table();
    vshogi::minishogi::init_ranging_squares_table();
    vshogi::judkins_shogi::init_non_ranging_attacks_table();
    vshogi::judkins_shogi::init_ranging_squares_table();
    vshogi::shogi::init_non_ranging_attacks_table();
    vshogi::shogi::init_ranging_squares_table();
    return CommandLineTestRunner::RunAllTests(argc, argv);
}
