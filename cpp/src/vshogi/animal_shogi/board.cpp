#include "vshogi/animal_shogi/board.hpp"

namespace vshogi::animal_shogi
{

const char*
Board::set_sfen_rank(const char* const sfen_rank, const RankEnum nth_rank)
{
    static_assert(static_cast<int>(RANK1) == 0);
    constexpr int max_length = 4; // "ELG ", "1c1/", ...
    auto piece_ptr = m_pieces + 3 * static_cast<int>(nth_rank);
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

const char* Board::set_sfen(const char* sfen)
{
    sfen = set_sfen_rank(sfen, RANK1);
    sfen = set_sfen_rank(sfen, RANK2);
    sfen = set_sfen_rank(sfen, RANK3);
    sfen = set_sfen_rank(sfen, RANK4);
    return sfen;
}

} // namespace vshogi::animal_shogi
