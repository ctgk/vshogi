#ifndef VSHOGI_ANIMAL_SHOGI_PIECE_STAND_HPP
#define VSHOGI_ANIMAL_SHOGI_PIECE_STAND_HPP

#include <cstdint>

namespace vshogi::animal_shogi
{

namespace internal
{

constexpr std::uint8_t shift_bits_for_piece_stand[] = {
    0, // CH
    3, // EL
    6, // GI
};
constexpr std::uint8_t mask_for_piece_stand[] = {
    0b00000011, // CH
    0b00011000, // EL
    0b11000000, // GI
};
constexpr std::uint8_t delta_for_piece_stand[] = {
    0b00000001, // CH
    0b00001000, // EL
    0b01000000, // GI
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
    PieceStand(const std::uint8_t v) : m_value(v)
    {
    }

    enum PieceTypeEnum
    {
        CH, // Chick (Pawn)
        EL, // Elephant (limited Bishop)
        GI, // Giraffe (limited Rook)
    };

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
    PieceStand& add(const PieceTypeEnum p)
    {
        m_value = static_cast<std::uint8_t>(
            m_value + internal::delta_for_piece_stand[p]);
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

} // namespace vshogi::animal_shogi

#endif // VSHOGI_ANIMAL_SHOGI_PIECE_STAND_HPP
