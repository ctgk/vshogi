#ifndef VSHOGI_MINISHOGI_STAND_HPP
#define VSHOGI_MINISHOGI_STAND_HPP

#include <cstdint>

#include "vshogi/minishogi/piece.hpp"

namespace vshogi::minishogi
{

constexpr std::uint16_t stand_setter_mask = 0b0011011011011011;
constexpr int stand_shift_bits[] = {0, 3, 6, 9, 12};
constexpr std::uint16_t stand_masks[] = {
    // clang-format off
    0b0000000000000011,
    0b0000000000011000,
    0b0000000011000000,
    0b0000011000000000,
    0b0011000000000000,
    // clang-format on
};
constexpr std::uint16_t stand_deltas[] = {
    // clang-format off
    0b0000000000000001,
    0b0000000000001000,
    0b0000000001000000,
    0b0000001000000000,
    0b0001000000000000,
    // clang-format on
};

class Stand
{
private:
    /**
     * @brief 16-bit integer representing pieces on a stand.
     * @details
     * ________ ______11  FU (2-bit for 0, 1, or 2 pieces)
     * ________ ___11___  GI (2-bit)
     * ________ 11______  KA (2-bit)
     * _____11_ ________  HI (2-bit)
     * __11____ ________  KI (2-bit)
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
        const int num_gi,
        const int num_ka,
        const int num_hi,
        const int num_ki)
        : Stand(static_cast<std::uint16_t>(
            (num_ki << stand_shift_bits[Pieces::KI])
            + (num_hi << stand_shift_bits[Pieces::HI])
            + (num_ka << stand_shift_bits[Pieces::KA])
            + (num_gi << stand_shift_bits[Pieces::GI])
            + (num_fu << stand_shift_bits[Pieces::FU])))
    {
    }
    std::uint16_t get_value() const
    {
        return m_value;
    }
    void set_value(const std::uint16_t v)
    {
        m_value = v & stand_setter_mask;
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
        m_value = static_cast<std::uint16_t>(
            m_value + stand_deltas[Pieces::demote(p)] * num);
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
    using StandType = Stand;
    BlackWhiteStands() : m_black(), m_white()
    {
    }
    Stand& black()
    {
        return m_black;
    }
    const Stand& black() const
    {
        return m_black;
    }
    Stand& white()
    {
        return m_white;
    }
    const Stand& white() const
    {
        return m_white;
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
        constexpr int max_length = 11; // "2p2s2g2b2r "
        int preceding_number = 1;
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
                preceding_number = 2;
                continue;
            case 'P':
                m_black.add(Pieces::FU, preceding_number);
                break;
            case 'S':
                m_black.add(Pieces::GI, preceding_number);
                break;
            case 'G':
                m_black.add(Pieces::KI, preceding_number);
                break;
            case 'B':
                m_black.add(Pieces::KA, preceding_number);
                break;
            case 'R':
                m_black.add(Pieces::HI, preceding_number);
                break;
            case 'p':
                m_white.add(Pieces::FU, preceding_number);
                break;
            case 's':
                m_white.add(Pieces::GI, preceding_number);
                break;
            case 'g':
                m_white.add(Pieces::KI, preceding_number);
                break;
            case 'b':
                m_white.add(Pieces::KA, preceding_number);
                break;
            case 'r':
                m_white.add(Pieces::HI, preceding_number);
                break;
            default:
                break;
            }
            preceding_number = 1;
        }
    OUT_OF_LOOP:
        return ptr;
    }
    std::string to_sfen_holdings() const
    {
        constexpr Pieces::PieceTypeEnum stand_pieces[]
            = {Pieces::HI, Pieces::KA, Pieces::KI, Pieces::GI, Pieces::FU};
        auto out = std::string();
        for (auto c : color_array) {
            for (auto piece : stand_pieces) {
                const auto num = operator[](c).count(piece);
                if (num == 0)
                    continue;
                if (num == 1)
                    out += Pieces::to_sfen(Pieces::to_board_piece(c, piece));
                else
                    out += static_cast<char>('0' + num)
                           + Pieces::to_sfen(Pieces::to_board_piece(c, piece));
            }
        }
        if (out.empty())
            out += '-';
        return out;
    }
};

} // namespace vshogi::minishogi

#endif // VSHOGI_MINISHOGI_STAND_HPP
