#include "vshogi/shogi/board.hpp"
#include "vshogi/shogi/piece.hpp"

namespace vshogi
{

static constexpr auto B_FU = vshogi::shogi::Pieces::B_FU; // NOLINT
static constexpr auto B_KY = vshogi::shogi::Pieces::B_KY; // NOLINT
static constexpr auto B_KE = vshogi::shogi::Pieces::B_KE; // NOLINT
static constexpr auto B_GI = vshogi::shogi::Pieces::B_GI; // NOLINT
static constexpr auto B_KI = vshogi::shogi::Pieces::B_KI; // NOLINT
static constexpr auto B_KA = vshogi::shogi::Pieces::B_KA; // NOLINT
static constexpr auto B_HI = vshogi::shogi::Pieces::B_HI; // NOLINT
static constexpr auto B_OU = vshogi::shogi::Pieces::B_OU; // NOLINT
static constexpr auto B_TO = vshogi::shogi::Pieces::B_TO; // NOLINT
static constexpr auto B_NY = vshogi::shogi::Pieces::B_NY; // NOLINT
static constexpr auto B_NK = vshogi::shogi::Pieces::B_NK; // NOLINT
static constexpr auto B_NG = vshogi::shogi::Pieces::B_NG; // NOLINT
static constexpr auto B_UM = vshogi::shogi::Pieces::B_UM; // NOLINT
static constexpr auto B_RY = vshogi::shogi::Pieces::B_RY; // NOLINT
static constexpr auto W_FU = vshogi::shogi::Pieces::W_FU; // NOLINT
static constexpr auto W_KY = vshogi::shogi::Pieces::W_KY; // NOLINT
static constexpr auto W_KE = vshogi::shogi::Pieces::W_KE; // NOLINT
static constexpr auto W_GI = vshogi::shogi::Pieces::W_GI; // NOLINT
static constexpr auto W_KI = vshogi::shogi::Pieces::W_KI; // NOLINT
static constexpr auto W_KA = vshogi::shogi::Pieces::W_KA; // NOLINT
static constexpr auto W_HI = vshogi::shogi::Pieces::W_HI; // NOLINT
static constexpr auto W_OU = vshogi::shogi::Pieces::W_OU; // NOLINT
static constexpr auto W_TO = vshogi::shogi::Pieces::W_TO; // NOLINT
static constexpr auto W_NY = vshogi::shogi::Pieces::W_NY; // NOLINT
static constexpr auto W_NK = vshogi::shogi::Pieces::W_NK; // NOLINT
static constexpr auto W_NG = vshogi::shogi::Pieces::W_NG; // NOLINT
static constexpr auto W_UM = vshogi::shogi::Pieces::W_UM; // NOLINT
static constexpr auto W_RY = vshogi::shogi::Pieces::W_RY; // NOLINT
static constexpr auto VOID = vshogi::shogi::Pieces::VOID; // NOLINT

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
    }
{
}

template <>
const char*
shogi::Board::set_sfen_rank(const char* const sfen_rank, const RankEnum rank)
{
    constexpr int max_length = 19; // e.g. "+p+p+p+p+p+p+p+p+p/"
    auto piece_ptr = m_pieces + num_files * static_cast<int>(rank);
    const char* sfen_ptr = sfen_rank;
    bool promotion_flag = false;
    for (; sfen_ptr < sfen_rank + max_length; ++sfen_ptr) {
        switch (*sfen_ptr) {
        case '/':
        case ' ':
            ++sfen_ptr;
        case '\0':
            goto OUT_OF_LOOP;
        case '9':
            (*piece_ptr++) = VOID; // fall-through
        case '8':
            (*piece_ptr++) = VOID; // fall-through
        case '7':
            (*piece_ptr++) = VOID; // fall-through
        case '6':
            (*piece_ptr++) = VOID; // fall-through
        case '5':
            (*piece_ptr++) = VOID; // fall-through
        case '4':
            (*piece_ptr++) = VOID; // fall-through
        case '3':
            (*piece_ptr++) = VOID; // fall-through
        case '2':
            (*piece_ptr++) = VOID; // fall-through
        case '1':
            (*piece_ptr++) = VOID;
            break;
        case '+':
            promotion_flag = true;
            continue;
        case 'P':
            (*piece_ptr++) = promotion_flag ? B_TO : B_FU;
            break;
        case 'L':
            (*piece_ptr++) = promotion_flag ? B_NY : B_KY;
            break;
        case 'N':
            (*piece_ptr++) = promotion_flag ? B_NK : B_KE;
            break;
        case 'S':
            (*piece_ptr++) = promotion_flag ? B_NG : B_GI;
            break;
        case 'B':
            (*piece_ptr++) = promotion_flag ? B_UM : B_KA;
            break;
        case 'R':
            (*piece_ptr++) = promotion_flag ? B_RY : B_HI;
            break;
        case 'G':
            (*piece_ptr++) = B_KI;
            break;
        case 'K':
            (*piece_ptr++) = B_OU;
            break;
        case 'p':
            (*piece_ptr++) = promotion_flag ? W_TO : W_FU;
            break;
        case 'l':
            (*piece_ptr++) = promotion_flag ? W_NY : W_KY;
            break;
        case 'n':
            (*piece_ptr++) = promotion_flag ? W_NK : W_KE;
            break;
        case 's':
            (*piece_ptr++) = promotion_flag ? W_NG : W_GI;
            break;
        case 'b':
            (*piece_ptr++) = promotion_flag ? W_UM : W_KA;
            break;
        case 'r':
            (*piece_ptr++) = promotion_flag ? W_RY : W_HI;
            break;
        case 'g':
            (*piece_ptr++) = W_KI;
            break;
        case 'k':
            (*piece_ptr++) = W_OU;
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
