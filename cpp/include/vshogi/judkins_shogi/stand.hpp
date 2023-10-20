#ifndef VSHOGI_JUDKINS_SHOGI_STAND_HPP
#define VSHOGI_JUDKINS_SHOGI_STAND_HPP

#include <cstdint>

#include "vshogi/judkins_shogi/piece.hpp"

namespace vshogi::judkins_shogi
{

constexpr int stand_shift_bits[] = {0, 2, 4, 6, 8, 10};
constexpr std::uint16_t stand_masks[] = {
    // clang-format off
    0x0003, // FU
    0x000c, // KE
    0x0030, // GI
    0x00c0, // KA
    0x0300, // HI
    0x0c00, // KI
    // clang-format on
};
constexpr std::uint16_t stand_deltas[] = {
    // clang-format off
    0x0001, // FU
    0x0004, // KE
    0x0010, // GI
    0x0040, // KA
    0x0100, // HI
    0x0400, // KI
    // clang-format on
};
constexpr std::uint16_t stand_setter_mask = 0x0fff;

class Stand
{
private:
    /**
     * @brief 16-bit integer representing pieces on a stand.
     * @details
     * ________ ______**  FU (2 pieces)
     * ________ ____**__  KE (2 pieces)
     * ________ __**____  GI (2 pieces)
     * ________ **______  KA (2 pieces)
     * ______** ________  HI (2 pieces)
     * ____**__ ________  KI (2 pieces)
     */
    std::uint16_t m_value;

public:
    Stand() : m_value(0U)
    {
    }
    Stand(const std::uint16_t v) : m_value(v & stand_setter_mask)
    {
    }
    Stand(
        const int num_fu,
        const int num_ke,
        const int num_gi,
        const int num_ka,
        const int num_hi,
        const int num_ki)
        : Stand(static_cast<std::uint16_t>(
            (num_fu << stand_shift_bits[Pieces::FU])
            + (num_ke << stand_shift_bits[Pieces::KE])
            + (num_gi << stand_shift_bits[Pieces::GI])
            + (num_ka << stand_shift_bits[Pieces::KA])
            + (num_hi << stand_shift_bits[Pieces::HI])
            + (num_ki << stand_shift_bits[Pieces::KI])))
    {
    }
    std::uint32_t get_value() const
    {
        return m_value;
    }
    int count(const Pieces::PieceTypeEnum p) const
    {
        return static_cast<int>(
            (m_value & stand_masks[p]) >> stand_shift_bits[p]);
    }
    bool exist(const Pieces::PieceTypeEnum p) const
    {
        return (m_value & stand_masks[p]) > 0u;
    }
    bool any() const
    {
        return m_value > 0u;
    }
    Stand& add(const Pieces::PieceTypeEnum p, const int num = 1)
    {
        for (int ii = num; ii--;)
            m_value = static_cast<std::uint16_t>(
                m_value + stand_deltas[Pieces::demote(p)]);
        return *this;
    }
    Stand& subtract(const Pieces::PieceTypeEnum p)
    {
        m_value = static_cast<std::uint16_t>(
            m_value - stand_deltas[Pieces::demote(p)]);
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
};

class BlackWhiteStands
{
private:
    Stand m_black;
    Stand m_white;

public:
    using StandType = vshogi::judkins_shogi::Stand;
    BlackWhiteStands() : m_black(), m_white()
    {
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
        constexpr int max_length = 13; // "RBGSNPrbgsnp "
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
            case '2':
                num = 2;
                continue;
            case 'P':
                m_black.add(Pieces::FU, num);
                break;
            case 'N':
                m_black.add(Pieces::KE, num);
                break;
            case 'S':
                m_black.add(Pieces::GI, num);
                break;
            case 'B':
                m_black.add(Pieces::KA, num);
                break;
            case 'R':
                m_black.add(Pieces::HI, num);
                break;
            case 'G':
                m_black.add(Pieces::KI, num);
                break;
            case 'p':
                m_white.add(Pieces::FU, num);
                break;
            case 'n':
                m_white.add(Pieces::KE, num);
                break;
            case 's':
                m_white.add(Pieces::GI, num);
                break;
            case 'b':
                m_white.add(Pieces::KA, num);
                break;
            case 'r':
                m_white.add(Pieces::HI, num);
                break;
            case 'g':
                m_white.add(Pieces::KI, num);
                break;
            default:
                break;
            }
            num = 1;
        }
    OUT_OF_LOOP:
        return ptr;
    }
    void append_sfen(std::string& out) const
    {
        if (!(m_black.any() || m_white.any())) {
            out += '-';
            return;
        }
        constexpr Pieces::PieceTypeEnum pieces[]
            = {Pieces::HI,
               Pieces::KA,
               Pieces::KI,
               Pieces::GI,
               Pieces::KE,
               Pieces::FU};
        for (auto c : color_array) {
            for (auto piece : pieces) {
                const auto num = operator[](c).count(piece);
                if (num == 0)
                    continue;
                if (num > 1)
                    out += static_cast<char>('0' + num);
                Pieces::append_sfen(Pieces::to_board_piece(c, piece), out);
            }
        }
    }
};

} // namespace vshogi::judkins_shogi

#endif // VSHOGI_JUDKINS_SHOGI_STAND_HPP
