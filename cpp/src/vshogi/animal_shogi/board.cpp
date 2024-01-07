#include "vshogi/animal_shogi/board.hpp"
#include "vshogi/animal_shogi/piece.hpp"

namespace vshogi
{

static constexpr auto B_CH = vshogi::animal_shogi::Pieces::B_CH; // NOLINT
static constexpr auto B_EL = vshogi::animal_shogi::Pieces::B_EL; // NOLINT
static constexpr auto B_GI = vshogi::animal_shogi::Pieces::B_GI; // NOLINT
static constexpr auto B_LI = vshogi::animal_shogi::Pieces::B_LI; // NOLINT
static constexpr auto B_HE = vshogi::animal_shogi::Pieces::B_HE; // NOLINT
static constexpr auto W_CH = vshogi::animal_shogi::Pieces::W_CH; // NOLINT
static constexpr auto W_EL = vshogi::animal_shogi::Pieces::W_EL; // NOLINT
static constexpr auto W_GI = vshogi::animal_shogi::Pieces::W_GI; // NOLINT
static constexpr auto W_LI = vshogi::animal_shogi::Pieces::W_LI; // NOLINT
static constexpr auto W_HE = vshogi::animal_shogi::Pieces::W_HE; // NOLINT
static constexpr auto VOID = vshogi::animal_shogi::Pieces::VOID; // NOLINT

template <>
animal_shogi::Board::Board()
    : m_pieces{
        // clang-format off
        W_GI, W_LI, W_EL,
        VOID, W_CH, VOID,
        VOID, B_CH, VOID,
        B_EL, B_LI, B_GI,
        // clang-format on
    }
{
}

template <>
const char* animal_shogi::Board::set_sfen_rank(
    const char* const sfen_rank, const RankEnum rank)
{
    using namespace vshogi::animal_shogi;
    static_assert(static_cast<int>(RANK1) == 0);
    constexpr int max_length = 4; // "ELG ", "1c1/", ...
    auto piece_ptr = m_pieces + 3 * static_cast<int>(rank);
    const char* sfen_ptr;
    for (sfen_ptr = sfen_rank; sfen_ptr < sfen_rank + max_length; ++sfen_ptr) {
        switch (*sfen_ptr) {
        case '/': // fall-through
        case ' ': // fall-through
            ++sfen_ptr;
        case '\0':
            goto OUT_OF_LOOP;
        case '3':
            (*piece_ptr++) = VOID; // fall-through
        case '2':
            (*piece_ptr++) = VOID; // fall-through
        case '1':
            (*piece_ptr++) = VOID;
            break;
        case 'C':
            (*piece_ptr++) = B_CH;
            break;
        case 'E':
            (*piece_ptr++) = B_EL;
            break;
        case 'G':
            (*piece_ptr++) = B_GI;
            break;
        case 'H':
            (*piece_ptr++) = B_HE;
            break;
        case 'L':
            (*piece_ptr++) = B_LI;
            break;
        case 'c':
            (*piece_ptr++) = W_CH;
            break;
        case 'e':
            (*piece_ptr++) = W_EL;
            break;
        case 'g':
            (*piece_ptr++) = W_GI;
            break;
        case 'h':
            (*piece_ptr++) = W_HE;
            break;
        case 'l':
            (*piece_ptr++) = W_LI;
            break;
        default:
            break;
        }
    }
OUT_OF_LOOP:
    return sfen_ptr;
}

} // namespace vshogi
