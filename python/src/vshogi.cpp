#include "vshogi/animal_shogi/game.hpp"

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;
namespace as = vshogi::animal_shogi;

void export_animal_shogi_game(py::module& m)
{
    py::class_<as::Game>(m, "_Game")
        .def(py::init<>())
        .def(py::init<const std::string&>());
}

PYBIND11_MODULE(_vshogi, m)
{
    auto animal_shogi_module = m.def_submodule("animal_shogi");
    export_animal_shogi_game(animal_shogi_module);
}
