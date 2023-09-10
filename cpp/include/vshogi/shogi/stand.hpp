#ifndef VSHOGI_SHOGI_STAND_HPP
#define VSHOGI_SHOGI_STAND_HPP

#include <cstdint>

#include "vshogi/shogi/piece.hpp"

namespace vshogi::shogi
{

constexpr int stand_shift_bits[] = {0, 6, 10, 14, 18, 21, 24};
constexpr std::uint32_t stand_masks[] = {
    // clang-format off
    0x0000001f, // FU
    0x000000c0, // KY
    0x00001c00, // KE
    0x0001c000, // GI
    0x000c0000, // KA
    0x00600000, // HI
    0x07000000, // KI
    // clang-format on
};
constexpr std::uint32_t stand_deltas[] = {
    // clang-format off
    0x00000001, // FU
    0x00000040, // KY
    0x00000400, // KE
    0x00004000, // GI
    0x00040000, // KA
    0x00200000, // HI
    0x01000000, // KI
    // clang-format on
};
constexpr std::uint32_t stand_setter_mask = 0x076ddddf;

class Stand
{
private:
    /**
     * @brief 32-bit integer representing pieces on a stand.
     * @details
     * ________ ________ ________ ___*****  FU (18 pieces)
     * ________ ________ _______* **______  KY (4 pieces)
     * ________ ________ ___***__ ________  KE (4 pieces)
     * ________ _______* **______ ________  GI (4 pieces)
     * ________ ____**__ ________ ________  KA (2 pieces)
     * ________ _**_____ ________ ________  HI (2 pieces)
     * _____*** ________ ________ ________  KI (4 pieces)
     */
    std::uint32_t m_value;

public:
    Stand() : m_value(0U)
    {
    }
    Stand(const std::uint32_t v) : m_value(v & stand_setter_mask)
    {
    }
    Stand(
        const int num_fu,
        const int num_ky,
        const int num_ke,
        const int num_gi,
        const int num_ka,
        const int num_hi,
        const int num_ki)
        : Stand(static_cast<std::uint32_t>(
            (num_fu << stand_shift_bits[FU]) + (num_ky << stand_shift_bits[KY])
            + (num_ke << stand_shift_bits[KE])
            + (num_gi << stand_shift_bits[GI])
            + (num_ka << stand_shift_bits[KA])
            + (num_hi << stand_shift_bits[HI])
            + (num_ki << stand_shift_bits[KI])))
    {
    }
    std::uint32_t get_value() const
    {
        return m_value;
    }
    int count(const PieceTypeEnum p) const
    {
        return static_cast<int>(
            (m_value & stand_masks[p]) >> stand_shift_bits[p]);
    }
    bool exist(const PieceTypeEnum p) const
    {
        return (m_value & stand_masks[p]) > 0u;
    }
    bool any() const
    {
        return m_value > 0u;
    }
    Stand& add(const PieceTypeEnum p, const int num = 1)
    {
        m_value += stand_deltas[demote(p)] * static_cast<std::uint32_t>(num);
        return *this;
    }
    Stand& subtract(const PieceTypeEnum p)
    {
        m_value = static_cast<std::uint32_t>(m_value - stand_deltas[demote(p)]);
        return *this;
    }
    bool operator==(const Stand& other) const
    {
        return m_value == other.m_value;
    }
    bool operator!=(const Stand& other) const
    {
        return m_value != other.m_value;
    }
    static constexpr int num_piece_types()
    {
        return sizeof(stand_piece_array) / sizeof(stand_piece_array[0]);
    }
    static constexpr PieceTypeEnum piece_types(const int i)
    {
        return stand_piece_array[i];
    }
};

class BlackWhiteStands
{
private:
    Stand m_black;
    Stand m_white;

public:
    BlackWhiteStands() : m_black(), m_white()
    {
    }
    static constexpr int num_piece_types()
    {
        return Stand::num_piece_types();
    }
    Stand& operator[](const ColorEnum c)
    {
        return (c == BLACK) ? m_black : m_white;
    }
    const Stand& operator[](const ColorEnum c) const
    {
        return (c == BLACK) ? m_black : m_white;
    }
    const char* set_sfen_holdings(const char* const sfen_holdings)
    {
        constexpr int max_length = 25; // "10p2l2n2sbr2g2P2L2N2SBR2G"
        int num = 1;
        const char* ptr = sfen_holdings;
        for (; ptr - sfen_holdings < max_length; ++ptr) {
            switch (*ptr) {
            case '-':
                ++ptr; // fall-through
            case ' ':
                ++ptr; // fall-through
            case '\0':
                goto OUT_OF_LOOP;
            case '1':
                num = 10;
                continue;
            case '0':
                num = (num == 1) ? 0 : (num + 0);
                continue;
            case '2':
                num = (num == 1) ? 2 : (num + 2);
                continue;
            case '3':
                num = (num == 1) ? 3 : (num + 3);
                continue;
            case '4':
                num = (num == 1) ? 4 : (num + 4);
                continue;
            case '5':
                num = (num == 1) ? 5 : (num + 5);
                continue;
            case '6':
                num = (num == 1) ? 6 : (num + 6);
                continue;
            case '7':
                num = (num == 1) ? 7 : (num + 7);
                continue;
            case '8':
                num = (num == 1) ? 8 : (num + 8);
                continue;
            case '9':
                num = (num == 1) ? 9 : (num + 9);
                continue;
            case 'P':
                m_black.add(FU, num);
                break;
            case 'L':
                m_black.add(KY, num);
                break;
            case 'N':
                m_black.add(KE, num);
                break;
            case 'S':
                m_black.add(GI, num);
                break;
            case 'B':
                m_black.add(KA, num);
                break;
            case 'R':
                m_black.add(HI, num);
                break;
            case 'G':
                m_black.add(KI, num);
                break;
            case 'p':
                m_white.add(FU, num);
                break;
            case 'l':
                m_white.add(KY, num);
                break;
            case 'n':
                m_white.add(KE, num);
                break;
            case 's':
                m_white.add(GI, num);
                break;
            case 'b':
                m_white.add(KA, num);
                break;
            case 'r':
                m_white.add(HI, num);
                break;
            case 'g':
                m_white.add(KI, num);
                break;
            default:
                break;
            }
            num = 1;
        }
    OUT_OF_LOOP:
        return ptr;
    }
    std::string to_sfen_holdings() const
    {
        auto out = std::string();
        constexpr PieceTypeEnum pieces[] = {HI, KA, KI, GI, KE, KY, FU};
        for (auto c : color_array) {
            for (auto piece : pieces) {
                const auto num = operator[](c).count(piece);
                if (num == 0)
                    continue;
                if (num == 1)
                    out += to_sfen_piece(to_board_piece(c, piece));
                else
                    out += std::to_string(num)
                           + to_sfen_piece(to_board_piece(c, piece));
            }
        }
        if (out.empty())
            out += '-';
        return out;
    }
};

} // namespace vshogi::shogi

#endif // VSHOGI_SHOGI_STAND_HPP
