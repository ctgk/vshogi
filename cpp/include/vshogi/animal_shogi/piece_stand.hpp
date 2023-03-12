#ifndef VSHOGI_ANIMAL_SHOGI_PIECE_STAND_HPP
#define VSHOGI_ANIMAL_SHOGI_PIECE_STAND_HPP

#include <cstdint>
#include <string>

#include "vshogi/animal_shogi/color.hpp"
#include "vshogi/animal_shogi/piece.hpp"

namespace vshogi::animal_shogi
{

namespace internal
{

constexpr std::uint8_t shift_bits_for_piece_stand[] = {
    0, // CH
    3, // EL
    6, // GI
    0, // LI
    0, // HE
    0, // NA
};
constexpr std::uint8_t mask_for_piece_stand[] = {
    0b00000011, // CH
    0b00011000, // EL
    0b11000000, // GI
    0, // LI
    0, // HE
    0, // NA
};
constexpr std::uint8_t delta_for_piece_stand[] = {
    0b00000001, // CH
    0b00001000, // EL
    0b01000000, // GI
    0, // LI
    0, // HE
    0, // NA
};

} // namespace internal

class PieceStand
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
    PieceStand() : m_value(0U)
    {
    }
    PieceStand(const std::uint8_t v) : m_value(v & 0b11011011)
    {
    }
    PieceStand(const int num_ch, const int num_el, const int num_gi)
        : PieceStand(
            static_cast<std::uint8_t>((num_gi << 6) + (num_el << 3) + num_ch))
    {
    }
    std::uint8_t get_value() const
    {
        return m_value;
    }
    std::uint8_t count(const PieceTypeEnum p) const
    {
        return static_cast<uint8_t>(
            (m_value & internal::mask_for_piece_stand[p])
            >> internal::shift_bits_for_piece_stand[p]);
    }
    bool exist(const PieceTypeEnum p) const
    {
        return static_cast<bool>(m_value & internal::mask_for_piece_stand[p]);
    }
    PieceStand& add(const PieceTypeEnum p, const std::uint8_t num = 1U)
    {
        m_value = static_cast<std::uint8_t>(
            m_value + internal::delta_for_piece_stand[p & 0x3] * num);
        return *this;
    }
    PieceStand& subtract(const PieceTypeEnum p)
    {
        m_value = static_cast<std::uint8_t>(
            m_value - internal::delta_for_piece_stand[p]);
        return *this;
    }
    bool operator==(const PieceStand& other) const
    {
        return m_value == other.m_value;
    }
    bool operator!=(const PieceStand& other) const
    {
        return m_value != other.m_value;
    }
};

class TwoPieceStands
{
private:
    PieceStand m_black;
    PieceStand m_white;

public:
    TwoPieceStands() : m_black(), m_white()
    {
    }
    PieceStand& black()
    {
        return m_black;
    }
    const PieceStand& black() const
    {
        return m_black;
    }
    PieceStand& white()
    {
        return m_white;
    }
    const PieceStand& white() const
    {
        return m_white;
    }
    PieceStand& operator[](const ColorEnum c)
    {
        return (c == BLACK) ? m_black : m_white;
    }
    const PieceStand& operator[](const ColorEnum c) const
    {
        return (c == BLACK) ? m_black : m_white;
    }
    const char* set_sfen_holdings(const char* const sfen_holdings);
};

} // namespace vshogi::animal_shogi

#endif // VSHOGI_ANIMAL_SHOGI_PIECE_STAND_HPP
