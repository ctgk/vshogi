#include "vshogi/judkins_shogi/game.hpp"

#include <pybind11/pybind11.h>

#include "vshogi_wrapper.hpp"

namespace py = pybind11;
namespace js = vshogi::judkins_shogi;

namespace
{

void export_square_enum(py::module& m)
{
    py::enum_<js::SquareEnum>(m, "Square")
        .value("SQ_6A", js::SQ_6A)
        .value("SQ_5A", js::SQ_5A)
        .value("SQ_4A", js::SQ_4A)
        .value("SQ_3A", js::SQ_3A)
        .value("SQ_2A", js::SQ_2A)
        .value("SQ_1A", js::SQ_1A)
        .value("SQ_6B", js::SQ_6B)
        .value("SQ_5B", js::SQ_5B)
        .value("SQ_4B", js::SQ_4B)
        .value("SQ_3B", js::SQ_3B)
        .value("SQ_2B", js::SQ_2B)
        .value("SQ_1B", js::SQ_1B)
        .value("SQ_6C", js::SQ_6C)
        .value("SQ_5C", js::SQ_5C)
        .value("SQ_4C", js::SQ_4C)
        .value("SQ_3C", js::SQ_3C)
        .value("SQ_2C", js::SQ_2C)
        .value("SQ_1C", js::SQ_1C)
        .value("SQ_6D", js::SQ_6D)
        .value("SQ_5D", js::SQ_5D)
        .value("SQ_4D", js::SQ_4D)
        .value("SQ_3D", js::SQ_3D)
        .value("SQ_2D", js::SQ_2D)
        .value("SQ_1D", js::SQ_1D)
        .value("SQ_6E", js::SQ_6E)
        .value("SQ_5E", js::SQ_5E)
        .value("SQ_4E", js::SQ_4E)
        .value("SQ_3E", js::SQ_3E)
        .value("SQ_2E", js::SQ_2E)
        .value("SQ_1E", js::SQ_1E)
        .value("SQ_6F", js::SQ_6F)
        .value("SQ_5F", js::SQ_5F)
        .value("SQ_4F", js::SQ_4F)
        .value("SQ_3F", js::SQ_3F)
        .value("SQ_2F", js::SQ_2F)
        .value("SQ_1F", js::SQ_1F)
        .value("A6", js::SQ_6A)
        .value("A5", js::SQ_5A)
        .value("A4", js::SQ_4A)
        .value("A3", js::SQ_3A)
        .value("A2", js::SQ_2A)
        .value("A1", js::SQ_1A)
        .value("B6", js::SQ_6B)
        .value("B5", js::SQ_5B)
        .value("B4", js::SQ_4B)
        .value("B3", js::SQ_3B)
        .value("B2", js::SQ_2B)
        .value("B1", js::SQ_1B)
        .value("C6", js::SQ_6C)
        .value("C5", js::SQ_5C)
        .value("C4", js::SQ_4C)
        .value("C3", js::SQ_3C)
        .value("C2", js::SQ_2C)
        .value("C1", js::SQ_1C)
        .value("D6", js::SQ_6D)
        .value("D5", js::SQ_5D)
        .value("D4", js::SQ_4D)
        .value("D3", js::SQ_3D)
        .value("D2", js::SQ_2D)
        .value("D1", js::SQ_1D)
        .value("E6", js::SQ_6E)
        .value("E5", js::SQ_5E)
        .value("E4", js::SQ_4E)
        .value("E3", js::SQ_3E)
        .value("E2", js::SQ_2E)
        .value("E1", js::SQ_1E)
        .value("F6", js::SQ_6F)
        .value("F5", js::SQ_5F)
        .value("F4", js::SQ_4F)
        .value("F3", js::SQ_3F)
        .value("F2", js::SQ_2F)
        .value("F1", js::SQ_1F);
}

void export_pieces(py::module& m)
{
    py::enum_<js::PieceTypeEnum>(m, "Piece")
        .value("FU", js::FU)
        .value("KE", js::KE)
        .value("GI", js::GI)
        .value("KA", js::KA)
        .value("HI", js::HI)
        .value("KI", js::KI)
        .value("OU", js::OU)
        .value("TO", js::TO)
        .value("NK", js::NK)
        .value("NG", js::NG)
        .value("UM", js::UM)
        .value("RY", js::RY)
        .value("NA", js::NA);
    py::enum_<js::BoardPieceTypeEnum>(m, "BoardPiece")
        .value("B_FU", js::B_FU)
        .value("B_KE", js::B_KE)
        .value("B_GI", js::B_GI)
        .value("B_KA", js::B_KA)
        .value("B_HI", js::B_HI)
        .value("B_KI", js::B_KI)
        .value("B_OU", js::B_OU)
        .value("B_TO", js::B_TO)
        .value("B_NK", js::B_NK)
        .value("B_NG", js::B_NG)
        .value("B_UM", js::B_UM)
        .value("B_RY", js::B_RY)
        .value("W_FU", js::W_FU)
        .value("W_KE", js::W_KE)
        .value("W_GI", js::W_GI)
        .value("W_KA", js::W_KA)
        .value("W_HI", js::W_HI)
        .value("W_KI", js::W_KI)
        .value("W_OU", js::W_OU)
        .value("W_TO", js::W_TO)
        .value("W_NK", js::W_NK)
        .value("W_NG", js::W_NG)
        .value("W_UM", js::W_UM)
        .value("W_RY", js::W_RY)
        .value("VOID", js::VOID);
}

} // namespace

void export_judkins_shogi(py::module& m)
{
    js::Squares::init_tables();
    js::State::init_zobrist_table();
    js::init_non_ranging_attacks_table();
    js::init_ranging_squares_table();

    export_square_enum(m);
    export_pieces(m);
    pyvshogi::export_classes<js::Game>(m);
}
