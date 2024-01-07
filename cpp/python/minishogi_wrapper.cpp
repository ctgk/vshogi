#include "vshogi/minishogi/game.hpp"

#include <pybind11/numpy.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include "vshogi_wrapper.hpp"

namespace py = pybind11;
namespace ms = vshogi::minishogi;

namespace
{

void export_square_enum(py::module& m)
{
    py::enum_<ms::SquareEnum>(m, "Square")
        .value("SQ_5A", ms::SQ_5A)
        .value("SQ_4A", ms::SQ_4A)
        .value("SQ_3A", ms::SQ_3A)
        .value("SQ_2A", ms::SQ_2A)
        .value("SQ_1A", ms::SQ_1A)
        .value("SQ_5B", ms::SQ_5B)
        .value("SQ_4B", ms::SQ_4B)
        .value("SQ_3B", ms::SQ_3B)
        .value("SQ_2B", ms::SQ_2B)
        .value("SQ_1B", ms::SQ_1B)
        .value("SQ_5C", ms::SQ_5C)
        .value("SQ_4C", ms::SQ_4C)
        .value("SQ_3C", ms::SQ_3C)
        .value("SQ_2C", ms::SQ_2C)
        .value("SQ_1C", ms::SQ_1C)
        .value("SQ_5D", ms::SQ_5D)
        .value("SQ_4D", ms::SQ_4D)
        .value("SQ_3D", ms::SQ_3D)
        .value("SQ_2D", ms::SQ_2D)
        .value("SQ_1D", ms::SQ_1D)
        .value("SQ_5E", ms::SQ_5E)
        .value("SQ_4E", ms::SQ_4E)
        .value("SQ_3E", ms::SQ_3E)
        .value("SQ_2E", ms::SQ_2E)
        .value("SQ_1E", ms::SQ_1E)
        .value("A5", ms::SQ_5A)
        .value("A4", ms::SQ_4A)
        .value("A3", ms::SQ_3A)
        .value("A2", ms::SQ_2A)
        .value("A1", ms::SQ_1A)
        .value("B5", ms::SQ_5B)
        .value("B4", ms::SQ_4B)
        .value("B3", ms::SQ_3B)
        .value("B2", ms::SQ_2B)
        .value("B1", ms::SQ_1B)
        .value("C5", ms::SQ_5C)
        .value("C4", ms::SQ_4C)
        .value("C3", ms::SQ_3C)
        .value("C2", ms::SQ_2C)
        .value("C1", ms::SQ_1C)
        .value("D5", ms::SQ_5D)
        .value("D4", ms::SQ_4D)
        .value("D3", ms::SQ_3D)
        .value("D2", ms::SQ_2D)
        .value("D1", ms::SQ_1D)
        .value("E5", ms::SQ_5E)
        .value("E4", ms::SQ_4E)
        .value("E3", ms::SQ_3E)
        .value("E2", ms::SQ_2E)
        .value("E1", ms::SQ_1E);
}

void export_pieces(py::module& m)
{
    py::enum_<ms::Pieces::PieceTypeEnum>(m, "Piece")
        .value("FU", ms::Pieces::FU)
        .value("GI", ms::Pieces::GI)
        .value("KA", ms::Pieces::KA)
        .value("HI", ms::Pieces::HI)
        .value("KI", ms::Pieces::KI)
        .value("OU", ms::Pieces::OU)
        .value("TO", ms::Pieces::TO)
        .value("NG", ms::Pieces::NG)
        .value("UM", ms::Pieces::UM)
        .value("RY", ms::Pieces::RY)
        .value("NA", ms::Pieces::NA);

    py::enum_<ms::Pieces::BoardPieceTypeEnum>(m, "BoardPiece")
        .value("B_FU", ms::Pieces::B_FU)
        .value("B_GI", ms::Pieces::B_GI)
        .value("B_KA", ms::Pieces::B_KA)
        .value("B_HI", ms::Pieces::B_HI)
        .value("B_KI", ms::Pieces::B_KI)
        .value("B_OU", ms::Pieces::B_OU)
        .value("B_TO", ms::Pieces::B_TO)
        .value("B_NG", ms::Pieces::B_NG)
        .value("B_UM", ms::Pieces::B_UM)
        .value("B_RY", ms::Pieces::B_RY)
        .value("W_FU", ms::Pieces::W_FU)
        .value("W_GI", ms::Pieces::W_GI)
        .value("W_KA", ms::Pieces::W_KA)
        .value("W_HI", ms::Pieces::W_HI)
        .value("W_KI", ms::Pieces::W_KI)
        .value("W_OU", ms::Pieces::W_OU)
        .value("W_TO", ms::Pieces::W_TO)
        .value("W_NG", ms::Pieces::W_NG)
        .value("W_UM", ms::Pieces::W_UM)
        .value("W_RY", ms::Pieces::W_RY)
        .value("VOID", ms::Pieces::VOID);
}

} // namespace

void export_minishogi(py::module& m)
{
    ms::Squares::init_tables();
    ms::State::init_zobrist_table();
    ms::init_non_ranging_attacks_table();
    ms::init_ranging_squares_table();

    export_square_enum(m);
    export_pieces(m);

    pyvshogi::export_classes<ms::Game>(m);
}
