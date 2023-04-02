#ifndef VSHOGI_MINISHOGI_BOARD_HPP
#define VSHOGI_MINISHOGI_BOARD_HPP

#include "vshogi/minishogi/bitboard.hpp"
#include "vshogi/minishogi/piece.hpp"
#include "vshogi/minishogi/squares.hpp"

namespace vshogi::minishogi
{

class Board
{
private:
    BoardPieceTypeEnum m_pieces[num_squares];
    constexpr static int hash_shift_width = 5;

    const char* set_sfen_rank(const char* const sfen_rank, const RankEnum rank)
    {
        constexpr int max_length = 10; // e.g. "+r+b+s+gk/"
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

public:
    Board()
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
    BoardPieceTypeEnum operator[](const SquareEnum sq) const
    {
        return m_pieces[sq];
    }
    BoardPieceTypeEnum& operator[](const SquareEnum sq)
    {
        return m_pieces[sq];
    }
    BitBoard to_piece_mask() const
    {
        BitBoard out = BitBoard(0U);
        for (auto sq : square_array) {
            if (m_pieces[sq] != VOID) {
                out |= BitBoard::from_square(sq);
            }
        }
        return out;
    }

    BitBoard to_piece_mask(const ColorEnum c) const
    {
        BitBoard out = BitBoard();
        for (auto sq : square_array) {
            const auto p = m_pieces[sq];
            if (p == VOID)
                continue;
            if (get_color(p) == c)
                out |= BitBoard::from_square(sq);
        }
        return out;
    }
    BitBoard to_attack_mask(const ColorEnum c) const
    {
        const auto occupied = to_piece_mask();
        BitBoard out = BitBoard(0U);
        for (auto sq : square_array) {
            const auto p = m_pieces[sq];
            if (get_color(p) == c)
                out |= get_attacks_by(p, sq, occupied);
        }
        return out;
    }

    /**
     * @brief Set pieces on the board given SFEN string.
     *
     * @param sfen SFEN string. e.g. "rbsgk/4p/5/P4/KGSBR b - 1"
     * @return const char* Remaining SFEN string e.g. "b - 1"
     */
    const char* set_sfen(const char* sfen)
    {
        sfen = set_sfen_rank(sfen, RANK1);
        sfen = set_sfen_rank(sfen, RANK2);
        sfen = set_sfen_rank(sfen, RANK3);
        sfen = set_sfen_rank(sfen, RANK4);
        sfen = set_sfen_rank(sfen, RANK5);
        return sfen;
    }
};

} // namespace vshogi::minishogi

#endif // VSHOGI_MINISHOGI_BOARD_HPP
