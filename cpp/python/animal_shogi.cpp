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

void export_move(py::module& m)
{
    py::class_<as::Move>(m, "Move")
        .def(
            py::init<const as::SquareEnum, const as::SquareEnum>(),
            py::arg("dst"),
            py::arg("src"))
        .def(
            py::init<const as::SquareEnum, const as::PieceTypeEnum>(),
            py::arg("dst"),
            py::arg("src"))
        .def("_to_dlshogi_policy_index", &as::Move::to_dlshogi_policy_index)
        .def_property_readonly("destination", &as::Move::destination)
        .def_property_readonly(
            "source",
            [](const as::Move& self) -> py::object {
                if (self.is_drop()) {
                    return py::cast(as::to_piece_type(self.source()));
                }
                return py::cast(as::to_square(self.source()));
            })
        .def("is_drop", &as::Move::is_drop)
        .def("rotate", &as::Move::rotate)
        .def("__hash__", &as::Move::hash)
        .def("__eq__", &as::Move::operator==)
        .def("__ne__", &as::Move::operator!=);
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

void export_game(py::module& m)
{
    py::class_<as::Game>(m, "_Game")
        .def(py::init<>())
        .def(py::init<const std::uint64_t>())
        .def(py::init<const std::string&>())
        .def(
            "__array__",
            [](const as::Game& self) -> py::array_t<float> {
                const auto shape = std::vector<py::ssize_t>({1, 4, 3, 16});
                auto out = py::array_t<float>(shape);
                for (int i = 4; i--;) {
                    for (int j = 3; j--;) {
                        const auto turn = self.get_turn();
                        {
                            const auto stand_turn = self.get_stand(turn);
                            const auto stand_oppo = self.get_stand(~turn);
                            for (int k = 3; k--;) {
                                *out.mutable_data(0, i, j, k)
                                    = static_cast<float>(stand_turn.count(
                                        as::stand_piece_array[k]));
                                *out.mutable_data(0, i, j, k + 8)
                                    = static_cast<float>(stand_oppo.count(
                                        as::stand_piece_array[k]));
                            }
                        }
                        {
                            const auto board = self.get_board();
                            const auto sq
                                = (turn == vshogi::BLACK)
                                      ? as::square_array[i * 3 + j]
                                      : as::square_array
                                          [as::num_squares - 1 - i * 3 - j];
                            for (int k = 5; k--;) {
                                const auto piece_type
                                    = static_cast<as::PieceTypeEnum>(k);
                                *out.mutable_data(0, i, j, k + 3)
                                    = static_cast<float>(
                                        board[sq]
                                        == as::to_board_piece(
                                            turn, piece_type));
                                *out.mutable_data(0, i, j, k + 8 + 3)
                                    = static_cast<float>(
                                        board[sq]
                                        == as::to_board_piece(
                                            ~turn, piece_type));
                            }
                        }
                    }
                }
                return out;
            })
        .def("get_turn", &as::Game::get_turn)
        .def("get_board", &as::Game::get_board)
        .def("get_stand", &as::Game::get_stand)
        .def("get_result", &as::Game::get_result)
        .def("record_length", &as::Game::record_length)
        .def("get_legal_moves", &as::Game::get_legal_moves)
        .def(
            "to_sfen", &as::Game::to_sfen, py::arg("include_move_count") = true)
        .def("hash_current_state", &as::Game::hash_current_state)
        .def("is_legal", &as::Game::is_legal)
        .def("apply", &as::Game::apply)
        .def("get_move_at", &as::Game::get_move_at)
        .def("get_sfen_at", &as::Game::get_sfen_at)
        .def("get_state_hash_at", &as::Game::get_state_hash_at)
        .def("copy", [](const as::Game& self) { return as::Game(self); })
        .def(
            "__deepcopy__",
            [](const as::Game& self, py::dict) { return as::Game(self); },
            py::arg("memo"));
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
    export_move(m);
    export_game(m);
}
