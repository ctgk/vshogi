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

} // namespace pyvshogi

#endif // PYTHON_VSHOGI_HPP
