#include "vshogi/animal_shogi/game.hpp"

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;
namespace as = vshogi::animal_shogi;

void export_color_enum(py::module& m)
{
    py::enum_<as::ColorEnum>(m, "ColorEnum")
        .value("BLACK", as::BLACK)
        .value("WHITE", as::WHITE)
        .export_values();
}

void export_square_enum(py::module& m)
{
    py::enum_<as::SquareEnum>(m, "SquareEnum")
        .value("SQ_A1", as::SQ_A1)
        .value("SQ_B1", as::SQ_B1)
        .value("SQ_C1", as::SQ_C1)
        .value("SQ_A2", as::SQ_A2)
        .value("SQ_B2", as::SQ_B2)
        .value("SQ_C2", as::SQ_C2)
        .value("SQ_A3", as::SQ_A3)
        .value("SQ_B3", as::SQ_B3)
        .value("SQ_C3", as::SQ_C3)
        .value("SQ_A4", as::SQ_A4)
        .value("SQ_B4", as::SQ_B4)
        .value("SQ_C4", as::SQ_C4)
        .export_values();
}

void export_move_source_enum(py::module& m)
{
    py::enum_<as::MoveSourceEnum>(m, "MoveSourceEnum")
        .value("MS_A1", as::MS_A1)
        .value("MS_B1", as::MS_B1)
        .value("MS_C1", as::MS_C1)
        .value("MS_A2", as::MS_A2)
        .value("MS_B2", as::MS_B2)
        .value("MS_C2", as::MS_C2)
        .value("MS_A3", as::MS_A3)
        .value("MS_B3", as::MS_B3)
        .value("MS_C3", as::MS_C3)
        .value("MS_A4", as::MS_A4)
        .value("MS_B4", as::MS_B4)
        .value("MS_C4", as::MS_C4)
        .value("MS_CH", as::MS_CH)
        .value("MS_EL", as::MS_EL)
        .value("MS_GI", as::MS_GI)
        .export_values();
}

void export_move(py::module& m)
{
    py::class_<as::Move>(m, "Move")
        .def(py::init<const as::SquareEnum, const as::MoveSourceEnum>())
        .def(py::init<const as::SquareEnum, const as::SquareEnum>())
        .def(py::init<const as::SquareEnum, const as::PieceTypeEnum>())
        .def("destination", &as::Move::destination)
        .def("source", &as::Move::source)
        .def("is_drop", &as::Move::is_drop);
}

void export_board(py::module& m)
{
    py::class_<as::Board>(m, "Board")
        .def(py::init<>())
        .def("get_piece_at", &as::Board::get_piece_at);
}

void export_pieces(py::module& m)
{
    py::enum_<as::PieceTypeEnum>(m, "PieceTypeEnum")
        .value("CH", as::CH)
        .value("EL", as::EL)
        .value("GI", as::GI)
        .value("LI", as::LI)
        .value("HE", as::HE)
        .value("NA", as::NA)
        .export_values();

    py::enum_<as::BoardPieceTypeEnum>(m, "BoardPieceTypeEnum")
        .value("B_CH", as::B_CH)
        .value("B_EL", as::B_EL)
        .value("B_GI", as::B_GI)
        .value("B_LI", as::B_LI)
        .value("B_HE", as::B_HE)
        .value("W_CH", as::W_CH)
        .value("W_EL", as::W_EL)
        .value("W_GI", as::W_GI)
        .value("W_LI", as::W_LI)
        .value("W_HE", as::W_HE)
        .value("VOID", as::VOID)
        .export_values();

    py::enum_<as::CapturedPieceTypeEnum>(m, "CapturedPieceTypeEnum")
        .value("C_CH", as::C_CH)
        .value("C_EL", as::C_EL)
        .value("C_GI", as::C_GI)
        .value("C_NA", as::C_NA)
        .export_values();
}

void export_animal_shogi_game(py::module& m)
{
    py::enum_<as::ResultEnum>(m, "ResultEnum")
        .value("ONGOING", as::ONGOING)
        .value("DRAW", as::DRAW)
        .value("BLACK_WIN", as::BLACK_WIN)
        .value("WHITE_WIN", as::WHITE_WIN)
        .export_values();
    py::class_<as::Game>(m, "_Game")
        .def(py::init<>())
        .def(py::init<const std::string&>())
        .def("get_turn", &as::Game::get_turn)
        .def("get_board", &as::Game::get_board)
        .def("get_piece_stand", &as::Game::get_piece_stand)
        .def("get_result", &as::Game::get_result)
        .def("is_applicable", &as::Game::is_applicable)
        .def("apply_move", &as::Game::apply_move);
}

PYBIND11_MODULE(_vshogi, m)
{
    auto animal_shogi_module = m.def_submodule("animal_shogi");
    export_board(animal_shogi_module);
    export_pieces(animal_shogi_module);
    export_color_enum(animal_shogi_module);
    export_square_enum(animal_shogi_module);
    export_move_source_enum(animal_shogi_module);
    export_move(animal_shogi_module);
    export_animal_shogi_game(animal_shogi_module);
}
