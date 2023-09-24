#ifndef VSHOGI_ANIMAL_SHOGI_STAND_HPP
#define VSHOGI_ANIMAL_SHOGI_STAND_HPP

#include <cstdint>
#include <string>

#include "vshogi/animal_shogi/piece.hpp"
#include "vshogi/color.hpp"

namespace vshogi::animal_shogi
{

constexpr std::uint8_t stand_shift_bits[] = {
    0, // CH
    3, // EL
    6, // GI
    0, // LI
    0, // HE
    0, // NA
};
constexpr std::uint8_t stand_masks[] = {
    0b00000011, // CH
    0b00011000, // EL
    0b11000000, // GI
    0, // LI
    0, // HE
    0, // NA
};
constexpr std::uint8_t stand_deltas[] = {
    0b00000001, // CH
    0b00001000, // EL
    0b01000000, // GI
    0, // LI
    0, // HE
    0, // NA
};

/**
 * @brief A piece stand class storing captured pieces counts in 8-bit integer.
 *
 */
class Stand
{
private:
    /**
     * @brief 8bit integer representing what is on a piece stand.
     * @details
     * ______11  Chick (2bits for 0, 1, or 2 pieces)
     * ___11___  Elephant (2 bits for 0, 1, or 2 pieces)
     * 11______  Giraffe (2 bits for 0, 1, or 2 pieces)
     */
    std::uint8_t m_value;

public:
    Stand() : m_value(0U)
    {
    }
    Stand(const std::uint8_t v) : m_value(v & 0b11011011)
    {
    }
    Stand(const int num_ch, const int num_el, const int num_gi)
        : Stand(
            static_cast<std::uint8_t>((num_gi << 6) + (num_el << 3) + num_ch))
    {
    }
    std::uint8_t get_value() const
    {
        return m_value;
    }
    void set_value(std::uint8_t v)
    {
        m_value = v & 0b11011011;
    }
    std::uint8_t count(const Pieces::PieceTypeEnum p) const
    {
        return static_cast<uint8_t>(
            (m_value & stand_masks[p]) >> stand_shift_bits[p]);
    }
    bool exist(const Pieces::PieceTypeEnum p) const
    {
        return static_cast<bool>(m_value & stand_masks[p]);
    }
    bool any() const
    {
        return static_cast<bool>(m_value);
    }
    bool empty() const
    {
        return !static_cast<bool>(m_value);
    }
    Stand& add(const Pieces::PieceTypeEnum p, const std::uint8_t num = 1U)
    {
        m_value
            = static_cast<std::uint8_t>(m_value + stand_deltas[p & 0x3] * num);
        return *this;
    }
    Stand& subtract(const Pieces::PieceTypeEnum p)
    {
        m_value = static_cast<std::uint8_t>(m_value - stand_deltas[p]);
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
    const char* set_sfen_holdings(const char* const sfen_holdings);
    std::string to_sfen_holdings() const
    {
        constexpr Pieces::PieceTypeEnum stand_pieces[]
            = {Pieces::GI, Pieces::EL, Pieces::CH};
        auto out = std::string();
        for (auto c : color_array) {
            for (auto piece : stand_pieces) {
                const auto num = operator[](c).count(piece);
                if (num == 0)
                    continue;
                if (num == 1)
                    out += Pieces::to_sfen(Pieces::to_board_piece(c, piece));
                else
                    out += std::to_string(static_cast<int>(num))
                           + Pieces::to_sfen(Pieces::to_board_piece(c, piece));
            }
        }
        if (out.empty())
            out += '-';
        return out;
    }
};

} // namespace vshogi::animal_shogi

#endif // VSHOGI_ANIMAL_SHOGI_STAND_HPP
