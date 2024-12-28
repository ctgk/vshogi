#include "vshogi/variants/judkins_shogi.hpp"

namespace vshogi
{

static constexpr auto B_FU = vshogi::judkins_shogi::B_FU; // NOLINT
static constexpr auto B_KE = vshogi::judkins_shogi::B_KE; // NOLINT
static constexpr auto B_GI = vshogi::judkins_shogi::B_GI; // NOLINT
static constexpr auto B_KI = vshogi::judkins_shogi::B_KI; // NOLINT
static constexpr auto B_KA = vshogi::judkins_shogi::B_KA; // NOLINT
static constexpr auto B_HI = vshogi::judkins_shogi::B_HI; // NOLINT
static constexpr auto B_OU = vshogi::judkins_shogi::B_OU; // NOLINT
static constexpr auto B_TO = vshogi::judkins_shogi::B_TO; // NOLINT
static constexpr auto B_NK = vshogi::judkins_shogi::B_NK; // NOLINT
static constexpr auto B_NG = vshogi::judkins_shogi::B_NG; // NOLINT
static constexpr auto B_UM = vshogi::judkins_shogi::B_UM; // NOLINT
static constexpr auto B_RY = vshogi::judkins_shogi::B_RY; // NOLINT
static constexpr auto W_FU = vshogi::judkins_shogi::W_FU; // NOLINT
static constexpr auto W_KE = vshogi::judkins_shogi::W_KE; // NOLINT
static constexpr auto W_GI = vshogi::judkins_shogi::W_GI; // NOLINT
static constexpr auto W_KI = vshogi::judkins_shogi::W_KI; // NOLINT
static constexpr auto W_KA = vshogi::judkins_shogi::W_KA; // NOLINT
static constexpr auto W_HI = vshogi::judkins_shogi::W_HI; // NOLINT
static constexpr auto W_OU = vshogi::judkins_shogi::W_OU; // NOLINT
static constexpr auto W_TO = vshogi::judkins_shogi::W_TO; // NOLINT
static constexpr auto W_NK = vshogi::judkins_shogi::W_NK; // NOLINT
static constexpr auto W_NG = vshogi::judkins_shogi::W_NG; // NOLINT
static constexpr auto W_UM = vshogi::judkins_shogi::W_UM; // NOLINT
static constexpr auto W_RY = vshogi::judkins_shogi::W_RY; // NOLINT
static constexpr auto VOID = vshogi::judkins_shogi::VOID; // NOLINT

template <>
judkins_shogi::Board::Board()
    : m_pieces{
        // clang-format off
        W_OU, W_FU, VOID, VOID, VOID, B_HI,
        W_KI, VOID, VOID, VOID, VOID, B_KA,
        W_GI, VOID, VOID, VOID, VOID, B_KE,
        W_KE, VOID, VOID, VOID, VOID, B_GI,
        W_KA, VOID, VOID, VOID, VOID, B_KI,
        W_HI, VOID, VOID, VOID, B_FU, B_OU,
        // clang-format on
    }, m_king_locations{}, m_bb_color{}, m_bb_piece{}
{
    update_internals_based_on_pieces();
}

} // namespace vshogi
