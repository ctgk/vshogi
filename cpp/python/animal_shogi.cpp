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
    py::enum_<as::Squares::SquareEnum>(m, "Square")
        .value("A1", as::Squares::SQ_A1)
        .value("B1", as::Squares::SQ_B1)
        .value("C1", as::Squares::SQ_C1)
        .value("A2", as::Squares::SQ_A2)
        .value("B2", as::Squares::SQ_B2)
        .value("C2", as::Squares::SQ_C2)
        .value("A3", as::Squares::SQ_A3)
        .value("B3", as::Squares::SQ_B3)
        .value("C3", as::Squares::SQ_C3)
        .value("A4", as::Squares::SQ_A4)
        .value("B4", as::Squares::SQ_B4)
        .value("C4", as::Squares::SQ_C4);
}

void export_pieces(py::module& m)
{
    py::enum_<as::Pieces::PieceTypeEnum>(m, "Piece")
        .value("CH", as::Pieces::CH)
        .value("EL", as::Pieces::EL)
        .value("GI", as::Pieces::GI)
        .value("LI", as::Pieces::LI)
        .value("HE", as::Pieces::HE)
        .value("NA", as::Pieces::NA);

    py::enum_<as::Pieces::BoardPieceTypeEnum>(m, "BoardPiece")
        .value("B_CH", as::Pieces::B_CH)
        .value("B_EL", as::Pieces::B_EL)
        .value("B_GI", as::Pieces::B_GI)
        .value("B_LI", as::Pieces::B_LI)
        .value("B_HE", as::Pieces::B_HE)
        .value("W_CH", as::Pieces::W_CH)
        .value("W_EL", as::Pieces::W_EL)
        .value("W_GI", as::Pieces::W_GI)
        .value("W_LI", as::Pieces::W_LI)
        .value("W_HE", as::Pieces::W_HE)
        .value("VOID", as::Pieces::VOID);
}

} // namespace

void export_animal_shogi(py::module& m)
{
    export_pieces(m);
    export_square_enum(m);

    pyvshogi::export_board<as::Board, as::Squares::SquareEnum>(m);
    pyvshogi::export_piece_stand<as::Stand, as::Pieces>(m);
    pyvshogi::export_move<as::Move>(m);
    pyvshogi::export_game<as::Game>(m);
    pyvshogi::export_node<as::Game, as::Move>(m);
}
