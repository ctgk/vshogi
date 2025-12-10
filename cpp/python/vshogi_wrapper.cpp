#include "vshogi/common/color.hpp"
#include "vshogi/common/result.hpp"
#include "vshogi/engine/az/node.hpp"
#include "vshogi/engine/gaz/node.hpp"

#include <pybind11/numpy.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;

void export_judkins_shogi(py::module& m);
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

void export_az_node(py::module& m)
{
    using Node = vshogi::engine::az::Node;
    py::class_<Node>(m, "AzNode")
        .def("get_proba", &Node::get_proba)
        .def("get_visit_count", &Node::get_visit_count)
        .def(
            "get_visit_count_excluding_random",
            &Node::get_visit_count_excluding_random)
        .def(
            "get_q_value",
            [](const Node& self, const uint depth) {
                return self.get_q_value(depth);
            })
        .def(
            "get_actions",
            [](const Node& self) {
                std::vector<uint> out{};
                out.reserve(self.count_childs());
                for (const Node* c = self.get_child(); c; c = c->get_sibling())
                    out.emplace_back(c->get_action());
                return out;
            })
        .def(
            "get_child_of",
            [](const Node& self, const vshogi::move_t& action) -> py::object {
                const auto out = self.get_child_of(action);
                if (out)
                    return py::cast(*out, py::return_value_policy::reference);
                return py::none();
            });
}

void export_gaz_node(py::module& m)
{
    using Node = vshogi::engine::gaz::Node;
    py::class_<Node>(m, "GazNode")
        .def("get_visit_count", &Node::get_visit_count)
        .def(
            "get_q_value",
            py::overload_cast<const uint>(&Node::get_q_value, py::const_))
        .def(
            "get_actions",
            [](const Node& self) {
                std::vector<uint> out{};
                out.reserve(self.count_childs());
                for (const Node* c = self.get_child(); c; c = c->get_sibling())
                    out.emplace_back(c->get_action());
                return out;
            })
        .def(
            "get_probas",
            [](const Node& self) {
                std::vector<float> out{};
                out.reserve(self.count_childs());
                for (const Node* c = self.get_child(); c; c = c->get_sibling())
                    out.emplace_back(c->get_logit());
                vshogi::softmax(out);
                return out;
            })
        .def(
            "get_child_of",
            [](const Node& self, const vshogi::move_t& action) -> py::object {
                const auto out = self.get_child_of(action);
                if (out)
                    return py::cast(*out, py::return_value_policy::reference);
                return py::none();
            });
}

PYBIND11_MODULE(_vshogi, m)
{
    export_color_enum(m);
    export_result_enum(m);
    export_az_node(m);
    export_gaz_node(m);

    auto judkins_shogi_module = m.def_submodule("judkins_shogi");
    export_judkins_shogi(judkins_shogi_module);

    auto minishogi_module = m.def_submodule("minishogi");
    export_minishogi(minishogi_module);

    auto shogi_module = m.def_submodule("shogi");
    export_shogi(shogi_module);
}
