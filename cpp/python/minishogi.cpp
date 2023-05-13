#include "vshogi/minishogi/game.hpp"

#include <pybind11/numpy.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;
namespace ms = vshogi::minishogi;

namespace
{

void export_board(py::module& m)
{
    py::class_<ms::Board>(m, "Board")
        .def(
            "__getitem__",
            py::overload_cast<const ms::SquareEnum>(
                &ms::Board::operator[], py::const_));
}

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
        .value("SQ_1E", ms::SQ_1E);
}

void export_move(py::module& m)
{
    py::class_<ms::Move>(m, "Move")
        .def(
            py::init<const ms::SquareEnum, const ms::SquareEnum, const bool>(),
            py::arg("dst"),
            py::arg("src"),
            py::arg("promote") = false)
        .def(
            py::init<const ms::SquareEnum, const ms::PieceTypeEnum>(),
            py::arg("dst"),
            py::arg("src"))
        .def_property_readonly("destination", &ms::Move::destination)
        .def_property_readonly("promote", &ms::Move::promote)
        .def_property_readonly(
            "source",
            [](const ms::Move& self) -> py::object {
                if (self.is_drop())
                    return py::cast(ms::to_piece_type(self.source()));
                return py::cast(ms::to_square(self.source()));
            })
        .def("is_drop", &ms::Move::is_drop)
        .def("rotate", &ms::Move::rotate)
        .def("__hash__", &ms::Move::hash)
        .def("__eq__", &ms::Move::operator==)
        .def("__ne__", &ms::Move::operator!=);
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

void export_piece_stand(py::module& m)
{
    py::class_<ms::Stand>(m, "Stand")
        .def(
            "count",
            py::overload_cast<const ms::PieceTypeEnum>(
                &ms::Stand::count, py::const_))
        .def("any", &ms::Stand::any)
        .def("to_dict", [](const ms::Stand& self) -> py::dict {
            py::dict out;
            for (auto p : ms::stand_piece_array) {
                out[py::cast(p)] = self.count(p);
            }
            return out;
        });
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
            "get_legal_move_to",
            py::overload_cast<
                const ms::SquareEnum,
                const vshogi::DirectionEnum,
                const bool>(&ms::Game::get_legal_move_to, py::const_))
        .def(
            "get_legal_move_to",
            py::overload_cast<const ms::SquareEnum, const ms::PieceTypeEnum>(
                &ms::Game::get_legal_move_to, py::const_))
        .def(
            "__array__",
            [](const ms::Game& self) -> py::array_t<float> {
                constexpr int num_ch = 10 + 10 + 5 + 5;
                const auto shape = std::vector<py::ssize_t>({1, 5, 5, num_ch});
                auto out = py::array_t<float>(shape);
                out[py::make_tuple(py::ellipsis())] = 0.f;
                const auto turn = self.get_turn();
                const auto stand_turn = self.get_stand(turn);
                const auto stand_oppo = self.get_stand(~turn);
                for (int k = 5; k--;) {
                    const auto num_turn = static_cast<float>(
                        stand_turn.count(ms::stand_piece_array[k]));
                    const auto num_oppo = static_cast<float>(
                        stand_oppo.count(ms::stand_piece_array[k]));
                    for (int i = 5; i--;) {
                        for (int j = 5; j--;) {
                            *out.mutable_data(0, i, j, k) = num_turn;
                            *out.mutable_data(0, i, j, k + 15) = num_oppo;
                        }
                    }
                }

                const auto board = self.get_board();
                for (int i = 5; i--;) {
                    for (int j = 5; j--;) {
                        const auto sq = ms::square_array
                            [(turn == vshogi::BLACK)
                                 ? (i * 5 + j)
                                 : (ms::num_squares - 1 - i * 5 - j)];
                        const auto board_piece = board[sq];
                        if (board_piece == ms::VOID)
                            continue;
                        const auto color = ms::get_color(board_piece);
                        const auto piece_type = ms::to_piece_type(board_piece);
                        auto k = static_cast<int>(ms::demote(piece_type));
                        k += (ms::is_promoted(piece_type)) ? 6 : 0;
                        k += (turn == color) ? 0 : 15;
                        *out.mutable_data(0, i, j, k + 5) = 1.f;
                    }
                }
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
    export_board(m);
    export_square_enum(m);
    export_pieces(m);
    export_piece_stand(m);
    export_move(m);
    export_game(m);
}
