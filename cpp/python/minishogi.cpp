#include "vshogi/minishogi/game.hpp"

#include <pybind11/numpy.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include "vshogi.hpp"

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
    py::enum_<ms::PieceTypeEnum>(m, "Piece")
        .value("FU", ms::FU)
        .value("GI", ms::GI)
        .value("KA", ms::KA)
        .value("HI", ms::HI)
        .value("KI", ms::KI)
        .value("OU", ms::OU)
        .value("TO", ms::TO)
        .value("NG", ms::NG)
        .value("UM", ms::UM)
        .value("RY", ms::RY)
        .value("NA", ms::NA);

    py::enum_<ms::BoardPieceTypeEnum>(m, "BoardPiece")
        .value("B_FU", ms::B_FU)
        .value("B_GI", ms::B_GI)
        .value("B_KA", ms::B_KA)
        .value("B_HI", ms::B_HI)
        .value("B_KI", ms::B_KI)
        .value("B_OU", ms::B_OU)
        .value("B_TO", ms::B_TO)
        .value("B_NG", ms::B_NG)
        .value("B_UM", ms::B_UM)
        .value("B_RY", ms::B_RY)
        .value("W_FU", ms::W_FU)
        .value("W_GI", ms::W_GI)
        .value("W_KA", ms::W_KA)
        .value("W_HI", ms::W_HI)
        .value("W_KI", ms::W_KI)
        .value("W_OU", ms::W_OU)
        .value("W_TO", ms::W_TO)
        .value("W_NG", ms::W_NG)
        .value("W_UM", ms::W_UM)
        .value("W_RY", ms::W_RY)
        .value("VOID", ms::VOID);
}

void export_game(py::module& m)
{
    py::class_<ms::Game>(m, "_Game")
        .def(py::init<>())
        .def(py::init<const std::string&>())
        .def("get_turn", &ms::Game::get_turn)
        .def("get_board", &ms::Game::get_board)
        .def("get_stand", &ms::Game::get_stand)
        .def("get_result", &ms::Game::get_result)
        .def("record_length", &ms::Game::record_length)
        .def("get_legal_moves", &ms::Game::get_legal_moves)
        .def(
            "to_sfen", &ms::Game::to_sfen, py::arg("include_move_count") = true)
        .def("is_legal", &ms::Game::is_legal)
        .def("apply", &ms::Game::apply)
        .def("get_move_at", &ms::Game::get_move_at)
        .def("get_sfen_at", &ms::Game::get_sfen_at)
        .def(
            "__array__",
            [](const ms::Game& self) -> py::array_t<float> {
                constexpr int num_ch = 10 + 10 + 5 + 5;
                const auto shape = std::vector<py::ssize_t>({1, 5, 5, num_ch});
                auto out = py::array_t<float>(shape);
                self.to_feature_map(out.mutable_data());
                return out;
            })
        .def("copy", [](const ms::Game& self) { return ms::Game(self); })
        .def(
            "__deepcopy__",
            [](const ms::Game& self, py::dict) { return ms::Game(self); },
            py::arg("memo"));
}

} // namespace

void export_minishogi(py::module& m)
{
    export_square_enum(m);
    export_pieces(m);

    pyvshogi::export_board<ms::Board, ms::SquareEnum>(m);
    pyvshogi::export_piece_stand<
        ms::Stand,
        ms::PieceTypeEnum,
        ms::stand_piece_array,
        5>(m);
    pyvshogi::export_move<ms::Move, ms::SquareEnum, ms::PieceTypeEnum>(m);
    export_game(m);
}
