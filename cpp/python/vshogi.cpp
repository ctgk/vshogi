#include "vshogi/color.hpp"
#include "vshogi/result.hpp"

#include <pybind11/numpy.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;

void export_animal_shogi(py::module& m);
void export_minishogi(py::module& m);
void export_shogi(py::module& m);

void export_color_enum(py::module& m)
{
    py::enum_<vshogi::ColorEnum>(m, "Color")
        .value("BLACK", vshogi::BLACK)
        .value("WHITE", vshogi::WHITE);
}
void export_result_enum(py::module& m)
{
    py::enum_<vshogi::ResultEnum>(m, "Result")
        .value("ONGOING", vshogi::ONGOING)
        .value("DRAW", vshogi::DRAW)
        .value("BLACK_WIN", vshogi::BLACK_WIN)
        .value("WHITE_WIN", vshogi::WHITE_WIN);
}

PYBIND11_MODULE(_vshogi, m)
{
    export_color_enum(m);
    export_result_enum(m);

    auto animal_shogi_module = m.def_submodule("animal_shogi");
    export_animal_shogi(animal_shogi_module);

    auto minishogi_module = m.def_submodule("minishogi");
    export_minishogi(minishogi_module);

    auto shogi_module = m.def_submodule("shogi");
    export_shogi(shogi_module);
}
