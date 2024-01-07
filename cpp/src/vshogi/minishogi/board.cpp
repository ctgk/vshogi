#include "vshogi/minishogi/board.hpp"
#include "vshogi/minishogi/piece.hpp"

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
        W_HI, W_KA, W_GI, W_KI, W_OU,
        VOID, VOID, VOID, VOID, W_FU,
        VOID, VOID, VOID, VOID, VOID,
        B_FU, VOID, VOID, VOID, VOID,
        B_OU, B_KI, B_GI, B_KA, B_HI,
        // clang-format on
    }
{
}

template <>
const char* minishogi::Board::set_sfen_rank(
    const char* const sfen_rank, const RankEnum rank)
{
    constexpr int max_length = 11; // e.g. "+r+b+s+p+P/"
    auto piece_str = m_pieces + num_files * static_cast<int>(rank);
    const char* sfen_ptr = sfen_rank;
    bool promotion_flag = false;
    for (; sfen_ptr < sfen_rank + max_length; ++sfen_ptr) {
        switch (*sfen_ptr) {
        case '/':
        case ' ':
            ++sfen_ptr;
        case '\0':
            goto OUT_OF_LOOP;
        case '5':
            (*piece_str++) = VOID; // fall-through
        case '4':
            (*piece_str++) = VOID; // fall-through
        case '3':
            (*piece_str++) = VOID; // fall-through
        case '2':
            (*piece_str++) = VOID; // fall-through
        case '1':
            (*piece_str++) = VOID;
            break;
        case '+':
            promotion_flag = true;
            continue;
        case 'P':
            (*piece_str++) = (promotion_flag) ? B_TO : B_FU;
            break;
        case 'S':
            (*piece_str++) = (promotion_flag) ? B_NG : B_GI;
            break;
        case 'G':
            (*piece_str++) = B_KI;
            break;
        case 'B':
            (*piece_str++) = (promotion_flag) ? B_UM : B_KA;
            break;
        case 'R':
            (*piece_str++) = (promotion_flag) ? B_RY : B_HI;
            break;
        case 'K':
            (*piece_str++) = B_OU;
            break;
        case 'p':
            (*piece_str++) = (promotion_flag) ? W_TO : W_FU;
            break;
        case 's':
            (*piece_str++) = (promotion_flag) ? W_NG : W_GI;
            break;
        case 'g':
            (*piece_str++) = W_KI;
            break;
        case 'b':
            (*piece_str++) = (promotion_flag) ? W_UM : W_KA;
            break;
        case 'r':
            (*piece_str++) = (promotion_flag) ? W_RY : W_HI;
            break;
        case 'k':
            (*piece_str++) = W_OU;
            break;
        default:
            break;
        }
        promotion_flag = false;
    }
OUT_OF_LOOP:
    return sfen_ptr;
}

} // namespace vshogi
