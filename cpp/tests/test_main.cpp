#include "vshogi/variants/animal_shogi.hpp"
#include "vshogi/variants/judkins_shogi.hpp"
#include "vshogi/variants/minishogi.hpp"
#include "vshogi/variants/shogi.hpp"

#include <CppUTest/CommandLineTestRunner.h>

int main(int argc, char* argv[])
{
    vshogi::animal_shogi::Pieces::init_tables();
    vshogi::animal_shogi::Squares::init_tables();
    vshogi::animal_shogi::BlackWhiteStands::init_tables();
    vshogi::animal_shogi::BitBoard::init_tables();
    vshogi::animal_shogi::State::init_zobrist_table();

    vshogi::minishogi::Pieces::init_tables();
    vshogi::minishogi::Squares::init_tables();
    vshogi::minishogi::BlackWhiteStands::init_tables();
    vshogi::minishogi::BitBoard::init_tables();
    vshogi::minishogi::State::init_zobrist_table();

    vshogi::judkins_shogi::Pieces::init_tables();
    vshogi::judkins_shogi::Squares::init_tables();
    vshogi::judkins_shogi::BlackWhiteStands::init_tables();
    vshogi::judkins_shogi::BitBoard::init_tables();
    vshogi::judkins_shogi::State::init_zobrist_table();

    vshogi::shogi::Pieces::init_tables();
    vshogi::shogi::Squares::init_tables();
    vshogi::shogi::BlackWhiteStands::init_tables();
    vshogi::shogi::BitBoard::init_tables();
    vshogi::shogi::State::init_zobrist_table();
    return CommandLineTestRunner::RunAllTests(argc, argv);
}
