#include "vshogi/common/color.hpp"
#include "vshogi/common/result.hpp"
#include "vshogi/engine/az/node.hpp"
#include "vshogi/engine/dfpn/node.hpp"
#include "vshogi/engine/gaz/node.hpp"

#include <nanobind/nanobind.h>
#include <nanobind/ndarray.h>
#include <nanobind/stl/vector.h>

namespace nb = nanobind;

void export_color_enum(nb::module_& m)
{
    nb::enum_<vshogi::ColorEnum>(m, "Color", nb::is_arithmetic())
        .value("BLACK", vshogi::BLACK)
        .value("WHITE", vshogi::WHITE);
}
void export_result_enum(nb::module_& m)
{
    nb::enum_<vshogi::ResultEnum>(m, "Result", nb::is_arithmetic())
        .value("ONGOING", vshogi::ONGOING)
        .value("DRAW", vshogi::DRAW)
        .value("BLACK_WIN", vshogi::BLACK_WIN)
        .value("WHITE_WIN", vshogi::WHITE_WIN);
}

void export_dfpn_node(nb::module_& m)
{
    using Node = vshogi::engine::dfpn::Node;
    constexpr float unit = static_cast<float>(vshogi::engine::dfpn::unit);
    constexpr uint inf = vshogi::engine::dfpn::inf;
    nb::class_<Node>(m, "DfpnNode")
        .def(
            "pn",
            [](const Node& self, const bool offence) {
                const auto n = self.pn(offence);
                if (n == inf)
                    return std::numeric_limits<float>::infinity();
                return static_cast<float>(n) / unit;
            })
        .def(
            "dn",
            [](const Node& self, const bool offence) {
                const auto n = self.dn(offence);
                if (n == inf)
                    return std::numeric_limits<float>::infinity();
                return static_cast<float>(n) / unit;
            })
        .def("get_action", &Node::get_action)
        .def("has_child", &Node::has_child)
        .def("get_children", [](const Node& self) -> nb::object {
            std::vector<const Node*> out;
            if (self.has_child()) {
                for (auto ch = self.get_child(); ch; ch = ch->get_sibling())
                    out.emplace_back(ch);
            }
            return nb::cast(out, nb::rv_policy::reference);
        });
}

void export_az_node(nb::module_& m)
{
    using Node = vshogi::engine::az::Node;
    nb::class_<Node>(m, "AzNode")
        .def("get_proba", &Node::get_proba)
        .def("get_visit_count", &Node::get_visit_count)
        .def(
            "get_visit_count_excluding_random",
            &Node::get_visit_count_excluding_random)
        .def(
            "get_q_value",
            [](const Node& self, const uint depth, const uint min_visits) {
                return self.get_q_value(depth, min_visits);
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
            [](const Node& self, const vshogi::move_t& action) -> nb::object {
                const auto out = self.get_child_of(action);
                if (out)
                    return nb::cast(*out, nb::rv_policy::reference);
                return nb::none();
            });
}

void export_gaz_node(nb::module_& m)
{
    using Node = vshogi::engine::gaz::Node;
    nb::class_<Node>(m, "GazNode")
        .def("get_visit_count", &Node::get_visit_count)
        .def(
            "get_q_value",
            nb::overload_cast<const uint, const uint>(
                &Node::get_q_value, nb::const_))
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
            [](const Node& self, const vshogi::move_t& action) -> nb::object {
                const auto out = self.get_child_of(action);
                if (out)
                    return nb::cast(*out, nb::rv_policy::reference);
                return nb::none();
            });
}

NB_MODULE(_vshogi, m)
{
    export_color_enum(m);
    export_result_enum(m);
    export_dfpn_node(m);
    export_az_node(m);
    export_gaz_node(m);
}
