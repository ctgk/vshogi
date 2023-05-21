#ifndef PYTHON_VSHOGI_HPP
#define PYTHON_VSHOGI_HPP

#include <pybind11/pybind11.h>

namespace pyvshogi
{

template <class Board, class Square>
inline void export_board(pybind11::module& m)
{
    pybind11::class_<Board>(m, "Board")
        .def(
            "__getitem__",
            pybind11::overload_cast<Square>(
                &Board::operator[], pybind11::const_));
}

template <class Stand, class Piece, const Piece array[], int Size>
inline void export_piece_stand(pybind11::module& m)
{
    pybind11::class_<Stand>(m, "Stand")
        .def("count", &Stand::count)
        .def("any", &Stand::any)
        .def("to_dict", [](const Stand& self) -> pybind11::dict {
            pybind11::dict out;
            for (int i = 0; i < Size; ++i) {
                const auto p = array[i];
                out[pybind11::cast(p)] = self.count(p);
            }
            return out;
        });
}

template <class Move, class Square, class Piece>
inline void export_move(pybind11::module& m)
{
    pybind11::class_<Move>(m, "Move")
        .def(
            pybind11::init<const Square, const Square, const bool>(),
            pybind11::arg("dst"),
            pybind11::arg("src"),
            pybind11::arg("promote") = false)
        .def(
            pybind11::init<const Square, const Piece>(),
            pybind11::arg("dst"),
            pybind11::arg("src"))
        .def_property_readonly("destination", &Move::destination)
        .def_property_readonly("promote", &Move::promote)
        .def_property_readonly(
            "source",
            [](const Move& self) -> pybind11::object {
                if (self.is_drop())
                    return pybind11::cast(static_cast<Piece>(
                        static_cast<int>(self.source())
                        - static_cast<int>(Square::NUM_SQ)));
                return pybind11::cast(static_cast<Square>(self.source()));
            })
        .def("is_drop", &Move::is_drop)
        .def("rotate", &Move::rotate)
        .def("_to_dlshogi_policy_index", &Move::to_dlshogi_policy_index)
        .def_static("_num_policy_per_square", &Move::num_policy_per_square)
        .def("__hash__", &Move::hash)
        .def("__eq__", &Move::operator==)
        .def("__ne__", &Move::operator!=);
}

template <class Game>
inline void export_game(pybind11::module& m)
{
    pybind11::class_<Game>(m, "_Game")
        .def(pybind11::init<>())
        .def(pybind11::init<const std::string&>())
        .def("get_turn", &Game::get_turn)
        .def("get_board", &Game::get_board)
        .def("get_stand", &Game::get_stand)
        .def("get_result", &Game::get_result)
        .def("record_length", &Game::record_length)
        .def("get_legal_moves", &Game::get_legal_moves)
        .def("to_sfen", &Game::to_sfen)
        .def("is_legal", &Game::is_legal)
        .def("apply", &Game::apply)
        .def("get_move_at", &Game::get_move_at)
        .def("get_sfen_at", &Game::get_sfen_at)
        .def_static("ranks", &Game::ranks)
        .def_static("files", &Game::files)
        .def_static("feature_channels", &Game::feature_channels)
        .def(
            "__array__",
            [](const Game& self) -> pybind11::array_t<float> {
                const auto shape = std::vector<pybind11::ssize_t>(
                    {1,
                     Game::ranks(),
                     Game::files(),
                     Game::feature_channels()});
                auto out = pybind11::array_t<float>(shape);
                self.to_feature_map(out.mutable_data());
                return out;
            })
        .def("copy", [](const Game& self) { return Game(self); })
        .def(
            "__deepcopy__",
            [](const Game& self, pybind11::dict) { return Game(self); },
            pybind11::arg("memo"));
}

} // namespace pyvshogi

#endif // PYTHON_VSHOGI_HPP
