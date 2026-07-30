#include "vshogi/variants/leshogi.hpp"

#include <nanobind/nanobind.h>
#include <nanobind/ndarray.h>
#include <nanobind/stl/string.h>
#include <nanobind/stl/vector.h>

#include "vshogi_wrapper.hpp"

namespace nb = nanobind;
namespace ls = vshogi::leshogi;

void export_leshogi(nb::module_& m);

namespace
{

void export_square_enum(nb::module_& m)
{
    nb::enum_<ls::SquareEnum>(m, "Square", nb::is_arithmetic())
        .value("SQ_4A", ls::SQ_4A)
        .value("SQ_3A", ls::SQ_3A)
        .value("SQ_2A", ls::SQ_2A)
        .value("SQ_1A", ls::SQ_1A)
        .value("SQ_4B", ls::SQ_4B)
        .value("SQ_3B", ls::SQ_3B)
        .value("SQ_2B", ls::SQ_2B)
        .value("SQ_1B", ls::SQ_1B)
        .value("SQ_4C", ls::SQ_4C)
        .value("SQ_3C", ls::SQ_3C)
        .value("SQ_2C", ls::SQ_2C)
        .value("SQ_1C", ls::SQ_1C)
        .value("SQ_4D", ls::SQ_4D)
        .value("SQ_3D", ls::SQ_3D)
        .value("SQ_2D", ls::SQ_2D)
        .value("SQ_1D", ls::SQ_1D)
        .value("SQ_4E", ls::SQ_4E)
        .value("SQ_3E", ls::SQ_3E)
        .value("SQ_2E", ls::SQ_2E)
        .value("SQ_1E", ls::SQ_1E)
        .value("SQ_4F", ls::SQ_4F)
        .value("SQ_3F", ls::SQ_3F)
        .value("SQ_2F", ls::SQ_2F)
        .value("SQ_1F", ls::SQ_1F)
        .value("A4", ls::SQ_4A)
        .value("A3", ls::SQ_3A)
        .value("A2", ls::SQ_2A)
        .value("A1", ls::SQ_1A)
        .value("B4", ls::SQ_4B)
        .value("B3", ls::SQ_3B)
        .value("B2", ls::SQ_2B)
        .value("B1", ls::SQ_1B)
        .value("C4", ls::SQ_4C)
        .value("C3", ls::SQ_3C)
        .value("C2", ls::SQ_2C)
        .value("C1", ls::SQ_1C)
        .value("D4", ls::SQ_4D)
        .value("D3", ls::SQ_3D)
        .value("D2", ls::SQ_2D)
        .value("D1", ls::SQ_1D)
        .value("E4", ls::SQ_4E)
        .value("E3", ls::SQ_3E)
        .value("E2", ls::SQ_2E)
        .value("E1", ls::SQ_1E)
        .value("F4", ls::SQ_4F)
        .value("F3", ls::SQ_3F)
        .value("F2", ls::SQ_2F)
        .value("F1", ls::SQ_1F);
}

void export_pieces(nb::module_& m)
{
    nb::enum_<ls::PieceTypeEnum>(m, "Piece", nb::is_arithmetic())
        .value("FU", ls::FU)
        .value("KY", ls::KY)
        .value("KE", ls::KE)
        .value("GI", ls::GI)
        .value("KA", ls::KA)
        .value("HI", ls::HI)
        .value("KI", ls::KI)
        .value("OU", ls::OU)
        .value("TO", ls::TO)
        .value("NY", ls::NY)
        .value("NK", ls::NK)
        .value("NG", ls::NG)
        .value("UM", ls::UM)
        .value("RY", ls::RY)
        .value("NA", ls::NA);

    nb::enum_<ls::PieceEnum>(m, "BoardPiece", nb::is_arithmetic())
        .value("B_FU", ls::B_FU)
        .value("B_KY", ls::B_KY)
        .value("B_KE", ls::B_KE)
        .value("B_GI", ls::B_GI)
        .value("B_KA", ls::B_KA)
        .value("B_HI", ls::B_HI)
        .value("B_KI", ls::B_KI)
        .value("B_OU", ls::B_OU)
        .value("B_TO", ls::B_TO)
        .value("B_NY", ls::B_NY)
        .value("B_NK", ls::B_NK)
        .value("B_NG", ls::B_NG)
        .value("B_UM", ls::B_UM)
        .value("B_RY", ls::B_RY)
        .value("W_FU", ls::W_FU)
        .value("W_KY", ls::W_KY)
        .value("W_KE", ls::W_KE)
        .value("W_GI", ls::W_GI)
        .value("W_KA", ls::W_KA)
        .value("W_HI", ls::W_HI)
        .value("W_KI", ls::W_KI)
        .value("W_OU", ls::W_OU)
        .value("W_TO", ls::W_TO)
        .value("W_NY", ls::W_NY)
        .value("W_NK", ls::W_NK)
        .value("W_NG", ls::W_NG)
        .value("W_UM", ls::W_UM)
        .value("W_RY", ls::W_RY)
        .value("VOID", ls::VOID);
}

NB_MODULE(_shogi, m)
{
    export_leshogi(m);
}

void export_leshogi(nb::module_& m)
{
    assert(0 == 1);
    ls::SquareTraits::init_tables();
    ls::BlackWhiteStands::init_tables();
    ls::Board::init_tables();
    ls::BitboardTraits::init_tables();
    ls::Magic::init_tables();

    export_square_enum(m);
    export_pieces(m);

    pyvshogi::export_classes<ls::Parameters>(m);
}

} // namespace
