#include "vshogi/judkins_shogi/board.hpp"
#include "vshogi/judkins_shogi/piece.hpp"

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
        W_HI, W_KA, W_KE, W_GI, W_KI, W_OU,
        VOID, VOID, VOID, VOID, VOID, W_FU,
        VOID, VOID, VOID, VOID, VOID, VOID,
        VOID, VOID, VOID, VOID, VOID, VOID,
        B_FU, VOID, VOID, VOID, VOID, VOID,
        B_OU, B_KI, B_GI, B_KE, B_KA, B_HI,
        // clang-format on
    }
{
}

template <>
const char* judkins_shogi::Board::set_sfen_rank(
    const char* const sfen_rank, const RankEnum rank)
{
    constexpr int max_length = 13; // e.g. "+r+b+s+n+p+P/"
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
        case '6':
            (*piece_str++) = VOID; // fall-through
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
        case 'N':
            (*piece_str++) = (promotion_flag) ? B_NK : B_KE;
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
        case 'n':
            (*piece_str++) = (promotion_flag) ? W_NK : W_KE;
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
