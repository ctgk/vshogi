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

template <class Stand, class PieceType, const PieceType array[], int Size>
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

} // namespace pyvshogi

#endif // PYTHON_VSHOGI_HPP
