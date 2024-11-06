#include "vshogi/variants/shogi.hpp"

namespace vshogi
{

static constexpr auto B_FU = vshogi::shogi::B_FU; // NOLINT
static constexpr auto B_KY = vshogi::shogi::B_KY; // NOLINT
static constexpr auto B_KE = vshogi::shogi::B_KE; // NOLINT
static constexpr auto B_GI = vshogi::shogi::B_GI; // NOLINT
static constexpr auto B_KI = vshogi::shogi::B_KI; // NOLINT
static constexpr auto B_KA = vshogi::shogi::B_KA; // NOLINT
static constexpr auto B_HI = vshogi::shogi::B_HI; // NOLINT
static constexpr auto B_OU = vshogi::shogi::B_OU; // NOLINT
static constexpr auto B_TO = vshogi::shogi::B_TO; // NOLINT
static constexpr auto B_NY = vshogi::shogi::B_NY; // NOLINT
static constexpr auto B_NK = vshogi::shogi::B_NK; // NOLINT
static constexpr auto B_NG = vshogi::shogi::B_NG; // NOLINT
static constexpr auto B_UM = vshogi::shogi::B_UM; // NOLINT
static constexpr auto B_RY = vshogi::shogi::B_RY; // NOLINT
static constexpr auto W_FU = vshogi::shogi::W_FU; // NOLINT
static constexpr auto W_KY = vshogi::shogi::W_KY; // NOLINT
static constexpr auto W_KE = vshogi::shogi::W_KE; // NOLINT
static constexpr auto W_GI = vshogi::shogi::W_GI; // NOLINT
static constexpr auto W_KI = vshogi::shogi::W_KI; // NOLINT
static constexpr auto W_KA = vshogi::shogi::W_KA; // NOLINT
static constexpr auto W_HI = vshogi::shogi::W_HI; // NOLINT
static constexpr auto W_OU = vshogi::shogi::W_OU; // NOLINT
static constexpr auto W_TO = vshogi::shogi::W_TO; // NOLINT
static constexpr auto W_NY = vshogi::shogi::W_NY; // NOLINT
static constexpr auto W_NK = vshogi::shogi::W_NK; // NOLINT
static constexpr auto W_NG = vshogi::shogi::W_NG; // NOLINT
static constexpr auto W_UM = vshogi::shogi::W_UM; // NOLINT
static constexpr auto W_RY = vshogi::shogi::W_RY; // NOLINT
static constexpr auto VOID = vshogi::shogi::VOID; // NOLINT

template <>
shogi::Board::Board()
    : m_pieces{
        // clang-format off
        W_KY, W_KE, W_GI, W_KI, W_OU, W_KI, W_GI, W_KE, W_KY,
        VOID, W_HI, VOID, VOID, VOID, VOID, VOID, W_KA, VOID,
        W_FU, W_FU, W_FU, W_FU, W_FU, W_FU, W_FU, W_FU, W_FU,
        VOID, VOID, VOID, VOID, VOID, VOID, VOID, VOID, VOID,
        VOID, VOID, VOID, VOID, VOID, VOID, VOID, VOID, VOID,
        VOID, VOID, VOID, VOID, VOID, VOID, VOID, VOID, VOID,
        B_FU, B_FU, B_FU, B_FU, B_FU, B_FU, B_FU, B_FU, B_FU,
        VOID, B_KA, VOID, VOID, VOID, VOID, VOID, B_HI, VOID,
        B_KY, B_KE, B_GI, B_KI, B_OU, B_KI, B_GI, B_KE, B_KY,
        // clang-format on
    }, m_king_locations{}, m_bb_color{}, m_bb_piece{}
{
    update_internals_based_on_pieces();
}

} // namespace vshogi
