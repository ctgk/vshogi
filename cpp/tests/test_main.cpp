#include "vshogi/animal_shogi/game.hpp"
#include "vshogi/judkins_shogi/game.hpp"
#include "vshogi/minishogi/game.hpp"
#include "vshogi/shogi/game.hpp"

#include <CppUTest/CommandLineTestRunner.h>

int main(int argc, char* argv[])
{
    vshogi::animal_shogi::State::init_zobrist_table();

    vshogi::minishogi::init_non_ranging_attacks_table();
    vshogi::minishogi::init_ranging_squares_table();
    vshogi::minishogi::State::init_zobrist_table();

    vshogi::judkins_shogi::init_non_ranging_attacks_table();
    vshogi::judkins_shogi::init_ranging_squares_table();
    vshogi::judkins_shogi::State::init_zobrist_table();

    vshogi::shogi::BitBoard::init_tables();
    vshogi::shogi::State::init_zobrist_table();
    return CommandLineTestRunner::RunAllTests(argc, argv);
}
