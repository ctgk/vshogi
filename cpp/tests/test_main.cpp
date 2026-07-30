#include "vshogi/variants/judkins_shogi.hpp"
#include "vshogi/variants/leshogi.hpp"
#include "vshogi/variants/minishogi.hpp"
#include "vshogi/variants/shogi.hpp"

#include <CppUTest/CommandLineTestRunner.h>

#if defined(__aarch64__) || defined(_M_ARM64)
SimpleString StringFrom(std::nullptr_t)
{
    return SimpleString("nullptr");
}
#endif

int main(int argc, char* argv[])
{
    vshogi::leshogi::SquareTraits::init_tables();
    vshogi::leshogi::BlackWhiteStands::init_tables();
    vshogi::leshogi::BitboardTraits::init_tables();
    vshogi::leshogi::Magic::init_tables();
    vshogi::leshogi::Board::init_tables();

    vshogi::minishogi::SquareTraits::init_tables();
    vshogi::minishogi::BlackWhiteStands::init_tables();
    vshogi::minishogi::BitboardTraits::init_tables();
    vshogi::minishogi::Magic::init_tables();
    vshogi::minishogi::Board::init_tables();

    vshogi::judkins_shogi::SquareTraits::init_tables();
    vshogi::judkins_shogi::BlackWhiteStands::init_tables();
    vshogi::judkins_shogi::BitboardTraits::init_tables();
    vshogi::judkins_shogi::Magic::init_tables();
    vshogi::judkins_shogi::Board::init_tables();

    vshogi::shogi::SquareTraits::init_tables();
    vshogi::shogi::BlackWhiteStands::init_tables();
    vshogi::shogi::BitboardTraits::init_tables();
    vshogi::shogi::Magic::init_tables();
    vshogi::shogi::Board::init_tables();
    return CommandLineTestRunner::RunAllTests(argc, argv);
}
