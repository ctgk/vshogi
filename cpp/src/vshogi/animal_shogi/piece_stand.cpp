#include "vshogi/animal_shogi/piece_stand.hpp"

namespace vshogi::animal_shogi
{

const char* TwoPieceStands::set_sfen_holdings(const char* const sfen_holdings)
{
    static_assert(static_cast<int>(CH) == 0);
    static_assert(static_cast<int>(EL) == 1);
    static_assert(static_cast<int>(GI) == 2);
    std::uint8_t preceding_number = 1;
    constexpr int max_length = 13; // "2C2E2G2c2e2g "
    const char* ptr;
    for (ptr = sfen_holdings; ptr < sfen_holdings + max_length; ++ptr) {
        switch (*ptr) {
        case '-':
            ++ptr; // fall-through
        case ' ':
            ++ptr;
        case '\0': // fall-through
            goto OUT_OF_LOOP;
        case '2':
            preceding_number = 2;
            continue;
        default:
            break;
        }
        ('A' < *ptr && *ptr < 'Z')
            ? m_black.add(
                // 'C' -> CH(0), 'E' -> EL(1), 'G' -> GI(2)
                static_cast<PieceTypeEnum>((*ptr - 'C') >> 1),
                preceding_number)
            : m_white.add(
                static_cast<PieceTypeEnum>((*ptr - 'c') >> 1),
                preceding_number);
        preceding_number = 1;
    }
OUT_OF_LOOP:
    return ptr;
}

} // namespace vshogi::animal_shogi
