#include "vshogi/shogi/game.hpp"

#include <pybind11/pybind11.h>

#include "vshogi.hpp"

namespace py = pybind11;
namespace sg = vshogi::shogi;

namespace
{

void export_square_enum(py::module& m)
{
    py::enum_<sg::Squares::SquareEnum>(m, "Square")
        .value("SQ_9A", sg::Squares::SQ_9A)
        .value("SQ_8A", sg::Squares::SQ_8A)
        .value("SQ_7A", sg::Squares::SQ_7A)
        .value("SQ_6A", sg::Squares::SQ_6A)
        .value("SQ_5A", sg::Squares::SQ_5A)
        .value("SQ_4A", sg::Squares::SQ_4A)
        .value("SQ_3A", sg::Squares::SQ_3A)
        .value("SQ_2A", sg::Squares::SQ_2A)
        .value("SQ_1A", sg::Squares::SQ_1A)
        .value("SQ_9B", sg::Squares::SQ_9B)
        .value("SQ_8B", sg::Squares::SQ_8B)
        .value("SQ_7B", sg::Squares::SQ_7B)
        .value("SQ_6B", sg::Squares::SQ_6B)
        .value("SQ_5B", sg::Squares::SQ_5B)
        .value("SQ_4B", sg::Squares::SQ_4B)
        .value("SQ_3B", sg::Squares::SQ_3B)
        .value("SQ_2B", sg::Squares::SQ_2B)
        .value("SQ_1B", sg::Squares::SQ_1B)
        .value("SQ_9C", sg::Squares::SQ_9C)
        .value("SQ_8C", sg::Squares::SQ_8C)
        .value("SQ_7C", sg::Squares::SQ_7C)
        .value("SQ_6C", sg::Squares::SQ_6C)
        .value("SQ_5C", sg::Squares::SQ_5C)
        .value("SQ_4C", sg::Squares::SQ_4C)
        .value("SQ_3C", sg::Squares::SQ_3C)
        .value("SQ_2C", sg::Squares::SQ_2C)
        .value("SQ_1C", sg::Squares::SQ_1C)
        .value("SQ_9D", sg::Squares::SQ_9D)
        .value("SQ_8D", sg::Squares::SQ_8D)
        .value("SQ_7D", sg::Squares::SQ_7D)
        .value("SQ_6D", sg::Squares::SQ_6D)
        .value("SQ_5D", sg::Squares::SQ_5D)
        .value("SQ_4D", sg::Squares::SQ_4D)
        .value("SQ_3D", sg::Squares::SQ_3D)
        .value("SQ_2D", sg::Squares::SQ_2D)
        .value("SQ_1D", sg::Squares::SQ_1D)
        .value("SQ_9E", sg::Squares::SQ_9E)
        .value("SQ_8E", sg::Squares::SQ_8E)
        .value("SQ_7E", sg::Squares::SQ_7E)
        .value("SQ_6E", sg::Squares::SQ_6E)
        .value("SQ_5E", sg::Squares::SQ_5E)
        .value("SQ_4E", sg::Squares::SQ_4E)
        .value("SQ_3E", sg::Squares::SQ_3E)
        .value("SQ_2E", sg::Squares::SQ_2E)
        .value("SQ_1E", sg::Squares::SQ_1E)
        .value("SQ_9F", sg::Squares::SQ_9F)
        .value("SQ_8F", sg::Squares::SQ_8F)
        .value("SQ_7F", sg::Squares::SQ_7F)
        .value("SQ_6F", sg::Squares::SQ_6F)
        .value("SQ_5F", sg::Squares::SQ_5F)
        .value("SQ_4F", sg::Squares::SQ_4F)
        .value("SQ_3F", sg::Squares::SQ_3F)
        .value("SQ_2F", sg::Squares::SQ_2F)
        .value("SQ_1F", sg::Squares::SQ_1F)
        .value("SQ_9G", sg::Squares::SQ_9G)
        .value("SQ_8G", sg::Squares::SQ_8G)
        .value("SQ_7G", sg::Squares::SQ_7G)
        .value("SQ_6G", sg::Squares::SQ_6G)
        .value("SQ_5G", sg::Squares::SQ_5G)
        .value("SQ_4G", sg::Squares::SQ_4G)
        .value("SQ_3G", sg::Squares::SQ_3G)
        .value("SQ_2G", sg::Squares::SQ_2G)
        .value("SQ_1G", sg::Squares::SQ_1G)
        .value("SQ_9H", sg::Squares::SQ_9H)
        .value("SQ_8H", sg::Squares::SQ_8H)
        .value("SQ_7H", sg::Squares::SQ_7H)
        .value("SQ_6H", sg::Squares::SQ_6H)
        .value("SQ_5H", sg::Squares::SQ_5H)
        .value("SQ_4H", sg::Squares::SQ_4H)
        .value("SQ_3H", sg::Squares::SQ_3H)
        .value("SQ_2H", sg::Squares::SQ_2H)
        .value("SQ_1H", sg::Squares::SQ_1H)
        .value("SQ_9I", sg::Squares::SQ_9I)
        .value("SQ_8I", sg::Squares::SQ_8I)
        .value("SQ_7I", sg::Squares::SQ_7I)
        .value("SQ_6I", sg::Squares::SQ_6I)
        .value("SQ_5I", sg::Squares::SQ_5I)
        .value("SQ_4I", sg::Squares::SQ_4I)
        .value("SQ_3I", sg::Squares::SQ_3I)
        .value("SQ_2I", sg::Squares::SQ_2I)
        .value("SQ_1I", sg::Squares::SQ_1I)
        .value("A9", sg::Squares::SQ_9A)
        .value("A8", sg::Squares::SQ_8A)
        .value("A7", sg::Squares::SQ_7A)
        .value("A6", sg::Squares::SQ_6A)
        .value("A5", sg::Squares::SQ_5A)
        .value("A4", sg::Squares::SQ_4A)
        .value("A3", sg::Squares::SQ_3A)
        .value("A2", sg::Squares::SQ_2A)
        .value("A1", sg::Squares::SQ_1A)
        .value("B9", sg::Squares::SQ_9B)
        .value("B8", sg::Squares::SQ_8B)
        .value("B7", sg::Squares::SQ_7B)
        .value("B6", sg::Squares::SQ_6B)
        .value("B5", sg::Squares::SQ_5B)
        .value("B4", sg::Squares::SQ_4B)
        .value("B3", sg::Squares::SQ_3B)
        .value("B2", sg::Squares::SQ_2B)
        .value("B1", sg::Squares::SQ_1B)
        .value("C9", sg::Squares::SQ_9C)
        .value("C8", sg::Squares::SQ_8C)
        .value("C7", sg::Squares::SQ_7C)
        .value("C6", sg::Squares::SQ_6C)
        .value("C5", sg::Squares::SQ_5C)
        .value("C4", sg::Squares::SQ_4C)
        .value("C3", sg::Squares::SQ_3C)
        .value("C2", sg::Squares::SQ_2C)
        .value("C1", sg::Squares::SQ_1C)
        .value("D9", sg::Squares::SQ_9D)
        .value("D8", sg::Squares::SQ_8D)
        .value("D7", sg::Squares::SQ_7D)
        .value("D6", sg::Squares::SQ_6D)
        .value("D5", sg::Squares::SQ_5D)
        .value("D4", sg::Squares::SQ_4D)
        .value("D3", sg::Squares::SQ_3D)
        .value("D2", sg::Squares::SQ_2D)
        .value("D1", sg::Squares::SQ_1D)
        .value("E9", sg::Squares::SQ_9E)
        .value("E8", sg::Squares::SQ_8E)
        .value("E7", sg::Squares::SQ_7E)
        .value("E6", sg::Squares::SQ_6E)
        .value("E5", sg::Squares::SQ_5E)
        .value("E4", sg::Squares::SQ_4E)
        .value("E3", sg::Squares::SQ_3E)
        .value("E2", sg::Squares::SQ_2E)
        .value("E1", sg::Squares::SQ_1E)
        .value("F9", sg::Squares::SQ_9F)
        .value("F8", sg::Squares::SQ_8F)
        .value("F7", sg::Squares::SQ_7F)
        .value("F6", sg::Squares::SQ_6F)
        .value("F5", sg::Squares::SQ_5F)
        .value("F4", sg::Squares::SQ_4F)
        .value("F3", sg::Squares::SQ_3F)
        .value("F2", sg::Squares::SQ_2F)
        .value("F1", sg::Squares::SQ_1F)
        .value("G9", sg::Squares::SQ_9G)
        .value("G8", sg::Squares::SQ_8G)
        .value("G7", sg::Squares::SQ_7G)
        .value("G6", sg::Squares::SQ_6G)
        .value("G5", sg::Squares::SQ_5G)
        .value("G4", sg::Squares::SQ_4G)
        .value("G3", sg::Squares::SQ_3G)
        .value("G2", sg::Squares::SQ_2G)
        .value("G1", sg::Squares::SQ_1G)
        .value("H9", sg::Squares::SQ_9H)
        .value("H8", sg::Squares::SQ_8H)
        .value("H7", sg::Squares::SQ_7H)
        .value("H6", sg::Squares::SQ_6H)
        .value("H5", sg::Squares::SQ_5H)
        .value("H4", sg::Squares::SQ_4H)
        .value("H3", sg::Squares::SQ_3H)
        .value("H2", sg::Squares::SQ_2H)
        .value("H1", sg::Squares::SQ_1H)
        .value("I9", sg::Squares::SQ_9I)
        .value("I8", sg::Squares::SQ_8I)
        .value("I7", sg::Squares::SQ_7I)
        .value("I6", sg::Squares::SQ_6I)
        .value("I5", sg::Squares::SQ_5I)
        .value("I4", sg::Squares::SQ_4I)
        .value("I3", sg::Squares::SQ_3I)
        .value("I2", sg::Squares::SQ_2I)
        .value("I1", sg::Squares::SQ_1I);
}

void export_pieces(py::module& m)
{
    py::enum_<sg::Pieces::PieceTypeEnum>(m, "Piece")
        .value("FU", sg::Pieces::FU)
        .value("KY", sg::Pieces::KY)
        .value("KE", sg::Pieces::KE)
        .value("GI", sg::Pieces::GI)
        .value("KA", sg::Pieces::KA)
        .value("HI", sg::Pieces::HI)
        .value("KI", sg::Pieces::KI)
        .value("OU", sg::Pieces::OU)
        .value("TO", sg::Pieces::TO)
        .value("NY", sg::Pieces::NY)
        .value("NK", sg::Pieces::NK)
        .value("NG", sg::Pieces::NG)
        .value("UM", sg::Pieces::UM)
        .value("RY", sg::Pieces::RY)
        .value("NA", sg::Pieces::NA);

    py::enum_<sg::Pieces::BoardPieceTypeEnum>(m, "BoardPiece")
        .value("B_FU", sg::Pieces::B_FU)
        .value("B_KY", sg::Pieces::B_KY)
        .value("B_KE", sg::Pieces::B_KE)
        .value("B_GI", sg::Pieces::B_GI)
        .value("B_KA", sg::Pieces::B_KA)
        .value("B_HI", sg::Pieces::B_HI)
        .value("B_KI", sg::Pieces::B_KI)
        .value("B_OU", sg::Pieces::B_OU)
        .value("B_TO", sg::Pieces::B_TO)
        .value("B_NY", sg::Pieces::B_NY)
        .value("B_NK", sg::Pieces::B_NK)
        .value("B_NG", sg::Pieces::B_NG)
        .value("B_UM", sg::Pieces::B_UM)
        .value("B_RY", sg::Pieces::B_RY)
        .value("W_FU", sg::Pieces::W_FU)
        .value("W_KY", sg::Pieces::W_KY)
        .value("W_KE", sg::Pieces::W_KE)
        .value("W_GI", sg::Pieces::W_GI)
        .value("W_KA", sg::Pieces::W_KA)
        .value("W_HI", sg::Pieces::W_HI)
        .value("W_KI", sg::Pieces::W_KI)
        .value("W_OU", sg::Pieces::W_OU)
        .value("W_TO", sg::Pieces::W_TO)
        .value("W_NY", sg::Pieces::W_NY)
        .value("W_NK", sg::Pieces::W_NK)
        .value("W_NG", sg::Pieces::W_NG)
        .value("W_UM", sg::Pieces::W_UM)
        .value("W_RY", sg::Pieces::W_RY)
        .value("VOID", sg::Pieces::VOID);
}

} // namespace

void export_shogi(py::module& m)
{
    export_square_enum(m);
    export_pieces(m);

    pyvshogi::export_board<sg::Board, sg::Squares::SquareEnum>(m);
    pyvshogi::export_piece_stand<sg::Stand, sg::Pieces>(m);
    pyvshogi::export_move<sg::Move>(m);
    pyvshogi::export_game<sg::Game>(m);
    pyvshogi::export_node<sg::Game, sg::Move>(m);
}
