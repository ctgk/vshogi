#ifndef VSHOGI_ANIMAL_SHOGI_BOARD_HPP
#define VSHOGI_ANIMAL_SHOGI_BOARD_HPP

#include <cstdint>
#include <string>

#include "vshogi/animal_shogi/bitboard.hpp"
#include "vshogi/animal_shogi/piece.hpp"
#include "vshogi/animal_shogi/squares.hpp"

namespace vshogi::animal_shogi
{

class Board
{
private:
    BoardPieceTypeEnum m_pieces[num_squares];
    const char* set_sfen_rank(const char* const sfen_rank, const RankEnum rank);
    void append_rank_sfen(const RankEnum rank, std::string& sfen) const
    {
        auto begin = m_pieces + num_files * static_cast<int>(rank);
        const auto end = begin + num_files;
        int num_void = 0;
        for (; begin < end; ++begin) {
            if (*begin == VOID) {
                ++num_void;
                continue;
            }
            if (num_void > 0) {
                sfen += static_cast<char>('0' + num_void);
                num_void = 0;
            }
            sfen += to_sfen_piece(*begin);
        }
        if (num_void > 0)
            sfen += static_cast<char>('0' + num_void);
    }

public:
    Board()
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
    BoardPieceTypeEnum operator[](const SquareEnum sq) const
    {
        return m_pieces[sq];
    }
    BoardPieceTypeEnum& operator[](const SquareEnum sq)
    {
        return m_pieces[sq];
    }

    static Board from_hash(std::uint64_t value)
    {
        auto b = Board();
        b.set_hash(value);
        return b;
    }

    /**
     * @brief Set pieces on the board according to given SFEN string.
     *
     * @param sfen SFEN string. e.g. "gle/1c1/1C1/ELG b - 1"
     * @return const char* Remaining SFEN string. e.g. "b - 1"
     */
    const char* set_sfen(const char* const sfen);
    void set_hash(std::uint64_t value)
    {
        for (int i = num_squares; i--;) {
            m_pieces[i] = static_cast<BoardPieceTypeEnum>(value & 0xf);
            value >>= 4;
        }
    }
    BitBoard to_attack_mask(const ColorEnum c) const
    {
        BitBoard out = BitBoard();
        for (auto&& sq : square_array) {
            const auto p = m_pieces[sq];
            if (to_color(p) == c)
                out |= get_attacks_by(p, sq);
        }
        return out;
    }
    BitBoard to_piece_mask() const
    {
        BitBoard out = BitBoard();
        for (auto sq : square_array) {
            if (m_pieces[sq] != VOID)
                out |= square_masks[sq];
        }
        return out;
    }
    BitBoard to_piece_mask(const ColorEnum c) const
    {
        BitBoard out = BitBoard();
        for (auto&& sq : square_array) {
            const auto p = m_pieces[sq];
            if (p == VOID)
                continue;
            if (to_color(p) == c)
                out |= square_masks[sq];
        }
        return out;
    }
    std::uint64_t hash() const
    {
        std::uint64_t out = 0;
        for (auto&& piece : m_pieces) {
            out <<= 4;
            out |= piece;
        }
        return out;
    }
    int count(const PieceTypeEnum p) const
    {
        int num = 0;
        for (auto&& piece : m_pieces) {
            num += (to_piece_type(piece) == p);
        }
        return num;
    }
    std::string to_sfen() const
    {
        auto out = std::string();
        append_rank_sfen(RANK1, out);
        out += '/';
        append_rank_sfen(RANK2, out);
        out += '/';
        append_rank_sfen(RANK3, out);
        out += '/';
        append_rank_sfen(RANK4, out);
        return out;
    }
};

} // namespace vshogi::animal_shogi

#endif // VSHOGI_ANIMAL_SHOGI_BOARD_HPP
