#include "vshogi/shogi/game.hpp"

#include <pybind11/pybind11.h>

#include "vshogi.hpp"

namespace py = pybind11;
namespace sg = vshogi::shogi;

namespace
{

void export_square_enum(py::module& m)
{
    py::enum_<sg::SquareEnum>(m, "Square")
        .value("SQ_9A", sg::SQ_9A)
        .value("SQ_8A", sg::SQ_8A)
        .value("SQ_7A", sg::SQ_7A)
        .value("SQ_6A", sg::SQ_6A)
        .value("SQ_5A", sg::SQ_5A)
        .value("SQ_4A", sg::SQ_4A)
        .value("SQ_3A", sg::SQ_3A)
        .value("SQ_2A", sg::SQ_2A)
        .value("SQ_1A", sg::SQ_1A)
        .value("SQ_9B", sg::SQ_9B)
        .value("SQ_8B", sg::SQ_8B)
        .value("SQ_7B", sg::SQ_7B)
        .value("SQ_6B", sg::SQ_6B)
        .value("SQ_5B", sg::SQ_5B)
        .value("SQ_4B", sg::SQ_4B)
        .value("SQ_3B", sg::SQ_3B)
        .value("SQ_2B", sg::SQ_2B)
        .value("SQ_1B", sg::SQ_1B)
        .value("SQ_9C", sg::SQ_9C)
        .value("SQ_8C", sg::SQ_8C)
        .value("SQ_7C", sg::SQ_7C)
        .value("SQ_6C", sg::SQ_6C)
        .value("SQ_5C", sg::SQ_5C)
        .value("SQ_4C", sg::SQ_4C)
        .value("SQ_3C", sg::SQ_3C)
        .value("SQ_2C", sg::SQ_2C)
        .value("SQ_1C", sg::SQ_1C)
        .value("SQ_9D", sg::SQ_9D)
        .value("SQ_8D", sg::SQ_8D)
        .value("SQ_7D", sg::SQ_7D)
        .value("SQ_6D", sg::SQ_6D)
        .value("SQ_5D", sg::SQ_5D)
        .value("SQ_4D", sg::SQ_4D)
        .value("SQ_3D", sg::SQ_3D)
        .value("SQ_2D", sg::SQ_2D)
        .value("SQ_1D", sg::SQ_1D)
        .value("SQ_9E", sg::SQ_9E)
        .value("SQ_8E", sg::SQ_8E)
        .value("SQ_7E", sg::SQ_7E)
        .value("SQ_6E", sg::SQ_6E)
        .value("SQ_5E", sg::SQ_5E)
        .value("SQ_4E", sg::SQ_4E)
        .value("SQ_3E", sg::SQ_3E)
        .value("SQ_2E", sg::SQ_2E)
        .value("SQ_1E", sg::SQ_1E)
        .value("SQ_9F", sg::SQ_9F)
        .value("SQ_8F", sg::SQ_8F)
        .value("SQ_7F", sg::SQ_7F)
        .value("SQ_6F", sg::SQ_6F)
        .value("SQ_5F", sg::SQ_5F)
        .value("SQ_4F", sg::SQ_4F)
        .value("SQ_3F", sg::SQ_3F)
        .value("SQ_2F", sg::SQ_2F)
        .value("SQ_1F", sg::SQ_1F)
        .value("SQ_9G", sg::SQ_9G)
        .value("SQ_8G", sg::SQ_8G)
        .value("SQ_7G", sg::SQ_7G)
        .value("SQ_6G", sg::SQ_6G)
        .value("SQ_5G", sg::SQ_5G)
        .value("SQ_4G", sg::SQ_4G)
        .value("SQ_3G", sg::SQ_3G)
        .value("SQ_2G", sg::SQ_2G)
        .value("SQ_1G", sg::SQ_1G)
        .value("SQ_9H", sg::SQ_9H)
        .value("SQ_8H", sg::SQ_8H)
        .value("SQ_7H", sg::SQ_7H)
        .value("SQ_6H", sg::SQ_6H)
        .value("SQ_5H", sg::SQ_5H)
        .value("SQ_4H", sg::SQ_4H)
        .value("SQ_3H", sg::SQ_3H)
        .value("SQ_2H", sg::SQ_2H)
        .value("SQ_1H", sg::SQ_1H)
        .value("SQ_9I", sg::SQ_9I)
        .value("SQ_8I", sg::SQ_8I)
        .value("SQ_7I", sg::SQ_7I)
        .value("SQ_6I", sg::SQ_6I)
        .value("SQ_5I", sg::SQ_5I)
        .value("SQ_4I", sg::SQ_4I)
        .value("SQ_3I", sg::SQ_3I)
        .value("SQ_2I", sg::SQ_2I)
        .value("SQ_1I", sg::SQ_1I)
        .value("A9", sg::SQ_9A)
        .value("A8", sg::SQ_8A)
        .value("A7", sg::SQ_7A)
        .value("A6", sg::SQ_6A)
        .value("A5", sg::SQ_5A)
        .value("A4", sg::SQ_4A)
        .value("A3", sg::SQ_3A)
        .value("A2", sg::SQ_2A)
        .value("A1", sg::SQ_1A)
        .value("B9", sg::SQ_9B)
        .value("B8", sg::SQ_8B)
        .value("B7", sg::SQ_7B)
        .value("B6", sg::SQ_6B)
        .value("B5", sg::SQ_5B)
        .value("B4", sg::SQ_4B)
        .value("B3", sg::SQ_3B)
        .value("B2", sg::SQ_2B)
        .value("B1", sg::SQ_1B)
        .value("C9", sg::SQ_9C)
        .value("C8", sg::SQ_8C)
        .value("C7", sg::SQ_7C)
        .value("C6", sg::SQ_6C)
        .value("C5", sg::SQ_5C)
        .value("C4", sg::SQ_4C)
        .value("C3", sg::SQ_3C)
        .value("C2", sg::SQ_2C)
        .value("C1", sg::SQ_1C)
        .value("D9", sg::SQ_9D)
        .value("D8", sg::SQ_8D)
        .value("D7", sg::SQ_7D)
        .value("D6", sg::SQ_6D)
        .value("D5", sg::SQ_5D)
        .value("D4", sg::SQ_4D)
        .value("D3", sg::SQ_3D)
        .value("D2", sg::SQ_2D)
        .value("D1", sg::SQ_1D)
        .value("E9", sg::SQ_9E)
        .value("E8", sg::SQ_8E)
        .value("E7", sg::SQ_7E)
        .value("E6", sg::SQ_6E)
        .value("E5", sg::SQ_5E)
        .value("E4", sg::SQ_4E)
        .value("E3", sg::SQ_3E)
        .value("E2", sg::SQ_2E)
        .value("E1", sg::SQ_1E)
        .value("F9", sg::SQ_9F)
        .value("F8", sg::SQ_8F)
        .value("F7", sg::SQ_7F)
        .value("F6", sg::SQ_6F)
        .value("F5", sg::SQ_5F)
        .value("F4", sg::SQ_4F)
        .value("F3", sg::SQ_3F)
        .value("F2", sg::SQ_2F)
        .value("F1", sg::SQ_1F)
        .value("G9", sg::SQ_9G)
        .value("G8", sg::SQ_8G)
        .value("G7", sg::SQ_7G)
        .value("G6", sg::SQ_6G)
        .value("G5", sg::SQ_5G)
        .value("G4", sg::SQ_4G)
        .value("G3", sg::SQ_3G)
        .value("G2", sg::SQ_2G)
        .value("G1", sg::SQ_1G)
        .value("H9", sg::SQ_9H)
        .value("H8", sg::SQ_8H)
        .value("H7", sg::SQ_7H)
        .value("H6", sg::SQ_6H)
        .value("H5", sg::SQ_5H)
        .value("H4", sg::SQ_4H)
        .value("H3", sg::SQ_3H)
        .value("H2", sg::SQ_2H)
        .value("H1", sg::SQ_1H)
        .value("I9", sg::SQ_9I)
        .value("I8", sg::SQ_8I)
        .value("I7", sg::SQ_7I)
        .value("I6", sg::SQ_6I)
        .value("I5", sg::SQ_5I)
        .value("I4", sg::SQ_4I)
        .value("I3", sg::SQ_3I)
        .value("I2", sg::SQ_2I)
        .value("I1", sg::SQ_1I);
}

void export_pieces(py::module& m)
{
    py::enum_<sg::PieceTypeEnum>(m, "Piece")
        .value("FU", sg::FU)
        .value("KY", sg::KY)
        .value("KE", sg::KE)
        .value("GI", sg::GI)
        .value("KA", sg::KA)
        .value("HI", sg::HI)
        .value("KI", sg::KI)
        .value("OU", sg::OU)
        .value("TO", sg::TO)
        .value("NY", sg::NY)
        .value("NK", sg::NK)
        .value("NG", sg::NG)
        .value("UM", sg::UM)
        .value("RY", sg::RY)
        .value("NA", sg::NA);

    py::enum_<sg::BoardPieceTypeEnum>(m, "BoardPiece")
        .value("B_FU", sg::B_FU)
        .value("B_KY", sg::B_KY)
        .value("B_KE", sg::B_KE)
        .value("B_GI", sg::B_GI)
        .value("B_KA", sg::B_KA)
        .value("B_HI", sg::B_HI)
        .value("B_KI", sg::B_KI)
        .value("B_OU", sg::B_OU)
        .value("B_TO", sg::B_TO)
        .value("B_NY", sg::B_NY)
        .value("B_NK", sg::B_NK)
        .value("B_NG", sg::B_NG)
        .value("B_UM", sg::B_UM)
        .value("B_RY", sg::B_RY)
        .value("W_FU", sg::W_FU)
        .value("W_KY", sg::W_KY)
        .value("W_KE", sg::W_KE)
        .value("W_GI", sg::W_GI)
        .value("W_KA", sg::W_KA)
        .value("W_HI", sg::W_HI)
        .value("W_KI", sg::W_KI)
        .value("W_OU", sg::W_OU)
        .value("W_TO", sg::W_TO)
        .value("W_NY", sg::W_NY)
        .value("W_NK", sg::W_NK)
        .value("W_NG", sg::W_NG)
        .value("W_UM", sg::W_UM)
        .value("W_RY", sg::W_RY)
        .value("VOID", sg::VOID);
}

} // namespace

void export_shogi(py::module& m)
{
    export_square_enum(m);
    export_pieces(m);

    pyvshogi::export_board<sg::Board, sg::SquareEnum>(m);
    pyvshogi::export_piece_stand<
        sg::Stand,
        sg::PieceTypeEnum,
        sg::stand_piece_array,
        7>(m);
    pyvshogi::export_move<sg::Move, sg::SquareEnum, sg::PieceTypeEnum>(m);
    pyvshogi::export_game<sg::Game, sg::Move>(m);
    pyvshogi::export_node<sg::Game, sg::Move>(m);
}
