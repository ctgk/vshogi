#include "vshogi/minishogi/game.hpp"

#include <pybind11/numpy.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;
namespace ms = vshogi::minishogi;

namespace
{

void export_square_enum(py::module& m)
{
    py::enum_<ms::SquareEnum>(m, "Square")
        .value("SQ_51", ms::SQ_51)
        .value("SQ_41", ms::SQ_41)
        .value("SQ_31", ms::SQ_31)
        .value("SQ_21", ms::SQ_21)
        .value("SQ_11", ms::SQ_11)
        .value("SQ_52", ms::SQ_52)
        .value("SQ_42", ms::SQ_42)
        .value("SQ_32", ms::SQ_32)
        .value("SQ_22", ms::SQ_22)
        .value("SQ_12", ms::SQ_12)
        .value("SQ_53", ms::SQ_53)
        .value("SQ_43", ms::SQ_43)
        .value("SQ_33", ms::SQ_33)
        .value("SQ_23", ms::SQ_23)
        .value("SQ_13", ms::SQ_13)
        .value("SQ_54", ms::SQ_54)
        .value("SQ_44", ms::SQ_44)
        .value("SQ_34", ms::SQ_34)
        .value("SQ_24", ms::SQ_24)
        .value("SQ_14", ms::SQ_14)
        .value("SQ_55", ms::SQ_55)
        .value("SQ_45", ms::SQ_45)
        .value("SQ_35", ms::SQ_35)
        .value("SQ_25", ms::SQ_25)
        .value("SQ_15", ms::SQ_15);
}

void export_move(py::module& m)
{
    py::class_<ms::Move>(m, "Move")
        .def(
            py::init<const ms::SquareEnum, const ms::SquareEnum>(),
            py::arg("destination"),
            py::arg("source"))
        .def(
            py::init<const ms::SquareEnum, const ms::PieceTypeEnum>(),
            py::arg("destination"),
            py::arg("source"))
        .def_property_readonly("destination", &ms::Move::destination)
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

} // namespace

void export_minishogi(py::module& m)
{
    export_square_enum(m);
    export_move(m);
    export_pieces(m);
    export_piece_stand(m);
}
