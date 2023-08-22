#include "vshogi/animal_shogi/game.hpp"

#include <pybind11/numpy.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include "vshogi.hpp"

namespace py = pybind11;
namespace as = vshogi::animal_shogi;

namespace
{

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

} // namespace

void export_animal_shogi(py::module& m)
{
    export_pieces(m);
    export_square_enum(m);

    pyvshogi::export_board<as::Board, as::SquareEnum>(m);
    pyvshogi::export_piece_stand<
        as::Stand,
        as::PieceTypeEnum,
        as::stand_piece_array,
        3>(m);
    pyvshogi::export_move<as::Move, as::SquareEnum, as::PieceTypeEnum>(m);
    pyvshogi::export_game<as::Game, as::Move>(m);
    pyvshogi::export_node<as::Game, as::Move>(m);
}
