#include "vshogi/animal_shogi/game.hpp"

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;
namespace as = vshogi::animal_shogi;

void export_color_enum(py::module& m)
{
    py::enum_<as::ColorEnum>(m, "Color")
        .value("BLACK", as::BLACK)
        .value("WHITE", as::WHITE);
}

void export_square_enum(py::module& m)
{
    py::enum_<as::SquareEnum>(m, "Square")
        .value("A1", as::SQ_A1)
        .value("B1", as::SQ_B1)
        .value("C1", as::SQ_C1)
        .value("A2", as::SQ_A2)
        .value("B2", as::SQ_B2)
        .value("C2", as::SQ_C2)
        .value("A3", as::SQ_A3)
        .value("B3", as::SQ_B3)
        .value("C3", as::SQ_C3)
        .value("A4", as::SQ_A4)
        .value("B4", as::SQ_B4)
        .value("C4", as::SQ_C4);
}

void export_move(py::module& m)
{
    py::class_<as::Move>(m, "Move")
        .def(py::init<const as::SquareEnum, const as::SquareEnum>())
        .def(py::init<const as::SquareEnum, const as::PieceTypeEnum>())
        .def("destination", &as::Move::destination)
        .def(
            "source",
            [](const as::Move& self) -> py::object {
                if (self.is_drop()) {
                    return py::cast(as::to_piece_type(self.source()));
                }
                return py::cast(as::to_square(self.source()));
            })
        .def("is_drop", &as::Move::is_drop);
}

void export_board(py::module& m)
{
    py::class_<as::Board>(m, "Board")
        .def("__getitem__", &as::Board::get_piece_at);
}

void export_pieces(py::module& m)
{
    py::enum_<as::PieceTypeEnum>(m, "Piece")
        .value("CH", as::CH)
        .value("EL", as::EL)
        .value("GI", as::GI)
        .value("LI", as::LI)
        .value("HE", as::HE)
        .value("NA", as::NA);

    py::enum_<as::BoardPieceTypeEnum>(m, "BoardPiece")
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
        .value("VOID", as::VOID);
}

void export_piece_stand(py::module& m)
{
    py::class_<as::Stand>(m, "Stand")
        .def(
            "count",
            py::overload_cast<const as::PieceTypeEnum>(
                &as::Stand::count, py::const_));
}

void export_animal_shogi_game(py::module& m)
{
    py::enum_<as::ResultEnum>(m, "Result")
        .value("ONGOING", as::ONGOING)
        .value("DRAW", as::DRAW)
        .value("BLACK_WIN", as::BLACK_WIN)
        .value("WHITE_WIN", as::WHITE_WIN);
    py::class_<as::Game>(m, "_Game")
        .def(py::init<>())
        .def(py::init<const std::string&>())
        .def("get_turn", &as::Game::get_turn)
        .def("get_board", &as::Game::get_board)
        .def("get_stand", &as::Game::get_stand)
        .def("get_result", &as::Game::get_result)
        .def("is_applicable", &as::Game::is_applicable)
        .def("apply", &as::Game::apply);
}

PYBIND11_MODULE(_vshogi, m)
{
    auto animal_shogi_module = m.def_submodule("animal_shogi");
    export_board(animal_shogi_module);
    export_pieces(animal_shogi_module);
    export_color_enum(animal_shogi_module);
    export_square_enum(animal_shogi_module);
    export_move(animal_shogi_module);
    export_piece_stand(animal_shogi_module);
    export_animal_shogi_game(animal_shogi_module);
}
