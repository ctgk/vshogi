#include "vshogi/variants/minishogi.hpp"

namespace vshogi
{

static constexpr auto B_FU = vshogi::minishogi::B_FU; // NOLINT
static constexpr auto B_GI = vshogi::minishogi::B_GI; // NOLINT
static constexpr auto B_KI = vshogi::minishogi::B_KI; // NOLINT
static constexpr auto B_KA = vshogi::minishogi::B_KA; // NOLINT
static constexpr auto B_HI = vshogi::minishogi::B_HI; // NOLINT
static constexpr auto B_OU = vshogi::minishogi::B_OU; // NOLINT
static constexpr auto B_TO = vshogi::minishogi::B_TO; // NOLINT
static constexpr auto B_NG = vshogi::minishogi::B_NG; // NOLINT
static constexpr auto B_UM = vshogi::minishogi::B_UM; // NOLINT
static constexpr auto B_RY = vshogi::minishogi::B_RY; // NOLINT
static constexpr auto W_FU = vshogi::minishogi::W_FU; // NOLINT
static constexpr auto W_GI = vshogi::minishogi::W_GI; // NOLINT
static constexpr auto W_KI = vshogi::minishogi::W_KI; // NOLINT
static constexpr auto W_KA = vshogi::minishogi::W_KA; // NOLINT
static constexpr auto W_HI = vshogi::minishogi::W_HI; // NOLINT
static constexpr auto W_OU = vshogi::minishogi::W_OU; // NOLINT
static constexpr auto W_TO = vshogi::minishogi::W_TO; // NOLINT
static constexpr auto W_NG = vshogi::minishogi::W_NG; // NOLINT
static constexpr auto W_UM = vshogi::minishogi::W_UM; // NOLINT
static constexpr auto W_RY = vshogi::minishogi::W_RY; // NOLINT
static constexpr auto VOID = vshogi::minishogi::VOID; // NOLINT

template <>
minishogi::Board::Board()
    : m_pieces{
        // clang-format off
        W_OU, W_FU, VOID, VOID, B_HI,
        W_KI, VOID, VOID, VOID, B_KA,
        W_GI, VOID, VOID, VOID, B_GI,
        W_KA, VOID, VOID, VOID, B_KI,
        W_HI, VOID, VOID, B_FU, B_OU,
        // clang-format on
    }, m_king_locations{}, m_bb_color{}, m_bb_piece{}
{
    update_internals_based_on_pieces();
}

} // namespace vshogi
