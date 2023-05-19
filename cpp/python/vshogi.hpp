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

} // namespace pyvshogi

#endif // PYTHON_VSHOGI_HPP
