#include "vshogi/animal_shogi/game.hpp"
#include "vshogi/judkins_shogi/game.hpp"
#include "vshogi/minishogi/game.hpp"
#include "vshogi/shogi/game.hpp"

#include <CppUTest/CommandLineTestRunner.h>

int main(int argc, char* argv[])
{
    vshogi::animal_shogi::Squares::init_tables();
    vshogi::animal_shogi::BitBoard::init_tables();
    vshogi::animal_shogi::State::init_zobrist_table();

    vshogi::minishogi::Squares::init_tables();
    vshogi::minishogi::BitBoard::init_tables();
    vshogi::minishogi::State::init_zobrist_table();

    vshogi::judkins_shogi::Squares::init_tables();
    vshogi::judkins_shogi::BitBoard::init_tables();
    vshogi::judkins_shogi::State::init_zobrist_table();

    vshogi::shogi::Squares::init_tables();
    vshogi::shogi::BitBoard::init_tables();
    vshogi::shogi::State::init_zobrist_table();
    return CommandLineTestRunner::RunAllTests(argc, argv);
}
