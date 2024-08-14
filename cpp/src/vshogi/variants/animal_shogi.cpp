#include "vshogi/variants/animal_shogi.hpp"

namespace vshogi
{

static constexpr auto B_CH = animal_shogi::B_CH; // NOLINT
static constexpr auto B_EL = animal_shogi::B_EL; // NOLINT
static constexpr auto B_GI = animal_shogi::B_GI; // NOLINT
static constexpr auto B_LI = animal_shogi::B_LI; // NOLINT
static constexpr auto B_HE = animal_shogi::B_HE; // NOLINT
static constexpr auto W_CH = animal_shogi::W_CH; // NOLINT
static constexpr auto W_EL = animal_shogi::W_EL; // NOLINT
static constexpr auto W_GI = animal_shogi::W_GI; // NOLINT
static constexpr auto W_LI = animal_shogi::W_LI; // NOLINT
static constexpr auto W_HE = animal_shogi::W_HE; // NOLINT
static constexpr auto VOID = animal_shogi::VOID; // NOLINT

template <>
animal_shogi::Board::Board()
    : m_pieces{
        // clang-format off
        W_GI, W_LI, W_EL,
        VOID, W_CH, VOID,
        VOID, B_CH, VOID,
        B_EL, B_LI, B_GI,
        // clang-format on
    }, m_king_locations{animal_shogi::SQ_B4, animal_shogi::SQ_B1}
{
}

template <>
const char*
animal_shogi::Board::set_sfen_rank(const char* const sfen_rank, const Rank rank)
{
    static_assert(static_cast<int>(vshogi::animal_shogi::RANK1) == 0);
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
            (*piece_ptr++) = vshogi::animal_shogi::VOID; // fall-through
        case '2':
            (*piece_ptr++) = vshogi::animal_shogi::VOID; // fall-through
        case '1':
            (*piece_ptr++) = vshogi::animal_shogi::VOID;
            break;
        case 'C':
            (*piece_ptr++) = vshogi::animal_shogi::B_CH;
            break;
        case 'E':
            (*piece_ptr++) = vshogi::animal_shogi::B_EL;
            break;
        case 'G':
            (*piece_ptr++) = vshogi::animal_shogi::B_GI;
            break;
        case 'H':
            (*piece_ptr++) = vshogi::animal_shogi::B_HE;
            break;
        case 'L':
            (*piece_ptr++) = vshogi::animal_shogi::B_LI;
            break;
        case 'c':
            (*piece_ptr++) = vshogi::animal_shogi::W_CH;
            break;
        case 'e':
            (*piece_ptr++) = vshogi::animal_shogi::W_EL;
            break;
        case 'g':
            (*piece_ptr++) = vshogi::animal_shogi::W_GI;
            break;
        case 'h':
            (*piece_ptr++) = vshogi::animal_shogi::W_HE;
            break;
        case 'l':
            (*piece_ptr++) = vshogi::animal_shogi::W_LI;
            break;
        default:
            break;
        }
    }
OUT_OF_LOOP:
    return sfen_ptr;
}

template <>
std::uint64_t animal_shogi::BlackWhiteStands::zobrist_table
    [num_colors][animal_shogi::Config::num_stand_piece_types]
    [animal_shogi::Config::max_stand_piece_count + 1]
    = {};

} // namespace vshogi
