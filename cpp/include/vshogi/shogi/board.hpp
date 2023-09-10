#ifndef VSHOGI_SHOGI_BOARD_HPP
#define VSHOGI_SHOGI_BOARD_HPP

#include <string>

#include "vshogi/shogi/bitboard.hpp"
#include "vshogi/shogi/piece.hpp"
#include "vshogi/shogi/squares.hpp"

namespace vshogi::shogi
{

class Board
{
private:
    BoardPieceTypeEnum m_pieces[num_squares];

public:
    Board()
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
    BoardPieceTypeEnum operator[](const SquareEnum sq) const
    {
        return m_pieces[sq];
    }
    BoardPieceTypeEnum& operator[](const SquareEnum sq)
    {
        return m_pieces[sq];
    }
    bool is_empty(const SquareEnum sq) const
    {
        return (m_pieces[sq] == VOID);
    }
    BitBoard to_piece_mask() const
    {
        BitBoard out = BitBoard();
        for (auto sq : square_array) {
            if (!is_empty(sq))
                out |= BitBoard::from_square(sq);
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
        BitBoard out = BitBoard();
        for (auto sq : square_array) {
            const auto p = m_pieces[sq];
            if (get_color(p) == c)
                out |= get_attacks_by(p, sq, occupied);
        }
        return out;
    }

    /**
     * @brief return a square where king locates.
     *
     * @param c Color of the king you want to locate.
     * @return SquareEnum Location of the king.
     */
    SquareEnum king_location(const ColorEnum c) const
    {
        const auto target = to_board_piece(c, OU);
        for (auto sq : square_array) {
            if (m_pieces[sq] == target)
                return sq;
        }
        return SQ_NA;
    }

    /**
     * @brief return true if the king is in check.
     *
     * @param c Color of the king.
     * @return true The king is in check.
     * @return false The king is not in check or not found on the board.
     */
    bool in_check(const ColorEnum c) const
    {
        const auto king_sq = king_location(c);
        if (king_sq == SQ_NA)
            return false;
        const auto opponent_attacks = to_attack_mask(~c);
        return opponent_attacks.is_one(king_sq);
    }

    /**
     * @brief Set pieces on the board given SFEN string.
     *
     * @param sfen SFEN string. e.g. "lnsgkgsnl/1r5b1/ppppppppp/9/9/9/PPPPPPPPP/1B5R1/LNSGKGSNL b - 1"
     * @return const char* Remaining SFEN string e.g. "b - 1"
     */
    const char* set_sfen(const char* sfen)
    {
        constexpr RankEnum rank_array[]
            = {RANK1, RANK2, RANK3, RANK4, RANK5, RANK6, RANK7, RANK8, RANK9};
        for (auto r : rank_array) {
            sfen = set_sfen_rank(sfen, r);
        }
        return sfen;
    }
    std::string to_sfen() const
    {
        auto out = std::string();
        constexpr RankEnum rank_array[]
            = {RANK1, RANK2, RANK3, RANK4, RANK5, RANK6, RANK7, RANK8};
        for (auto r : rank_array) {
            out += to_sfen_rank(r);
            out += '/';
        }
        out += to_sfen_rank(RANK9);
        return out;
    }
    static constexpr int ranks()
    {
        return num_ranks;
    }
    static constexpr int files()
    {
        return num_files;
    }
    static constexpr int num_piece_types()
    {
        return sizeof(piece_array) / sizeof(piece_array[0]);
    }
    static constexpr SquareEnum square(const int i)
    {
        return square_array[i];
    }

private:
    const char* set_sfen_rank(const char* const sfen_rank, const RankEnum rank)
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
    std::string to_sfen_rank(const RankEnum rank) const
    {
        auto begin = m_pieces + num_files * static_cast<int>(rank);
        const auto end = begin + num_files;
        auto out = std::string();
        int num_void = 0;
        for (; begin < end; ++begin) {
            if (*begin == VOID) {
                ++num_void;
                continue;
            }
            if (num_void > 0) {
                out += static_cast<char>('0' + num_void);
                num_void = 0;
            }
            out += to_sfen_piece(*begin);
        }
        if (num_void > 0)
            out += static_cast<char>('0' + num_void);
        return out;
    }
};

} // namespace vshogi::shogi

#endif // VSHOGI_SHOGI_BOARD_HPP
