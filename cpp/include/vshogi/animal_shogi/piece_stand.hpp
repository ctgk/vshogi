#ifndef VSHOGI_ANIMAL_SHOGI_PIECE_STAND_HPP
#define VSHOGI_ANIMAL_SHOGI_PIECE_STAND_HPP

#include <cstdint>

namespace vshogi::animal_shogi
{

namespace internal
{

constexpr std::uint32_t shift_bits_for_piece_stand[] = {
    0, // FU
    6, // KY
    10, // KE
    14, // GI
    18, // KI
    22, // KA
    25, // HI
};
constexpr std::uint32_t mask_for_piece_stand[] = {
    0x1f << 0, // FU
    0x7 << 6, // KY
    0x7 << 10, // KE
    0x7 << 14, // GI
    0x7 << 18, // KI
    0x3 << 22, // KA
    0x3 << 25, // HI
};
constexpr std::uint32_t delta_for_piece_stand[] = {
    1 << 0, // FU
    1 << 6, // KY
    1 << 10, // KE
    1 << 14, // GI
    1 << 18, // KI
    1 << 22, // KA
    1 << 25, // HI
};

} // namespace internal

class PieceStand
{
private:
    /**
     * @brief 32bit integer representing what is on a piece stand.
     * @details
     * xxxxxxxx xxxxxxxx xxxxxxxx xxx11111  Pawn
     * xxxxxxxx xxxxxxxx xxxxxxx1 11xxxxxx  Lance
     * xxxxxxxx xxxxxxxx xxx111xx xxxxxxxx  Knight
     * xxxxxxxx xxxxxxx1 11xxxxxx xxxxxxxx  Silver
     * xxxxxxxx xxx111xx xxxxxxxx xxxxxxxx  Gold
     * xxxxxxxx 11xxxxxx xxxxxxxx xxxxxxxx  Bishop
     * xxxxx11x xxxxxxxx xxxxxxxx xxxxxxxx  Rook
     */
    std::uint32_t m_value;

public:
    PieceStand() : m_value(0U)
    {
    }
    PieceStand(const std::uint32_t v) : m_value(v)
    {
    }

    enum PieceTypeEnum
    {
        FU,
        KY,
        KE,
        GI,
        KI,
        KA,
        HI,
    };

    std::uint32_t count(const PieceTypeEnum p) const
    {
        return (m_value & internal::mask_for_piece_stand[p])
               >> internal::shift_bits_for_piece_stand[p];
    }
    bool exist(const PieceTypeEnum p) const
    {
        return static_cast<bool>(m_value & internal::mask_for_piece_stand[p]);
    }
    PieceStand& add(const PieceTypeEnum p)
    {
        m_value += internal::delta_for_piece_stand[p];
        return *this;
    }
    PieceStand& subtract(const PieceTypeEnum p)
    {
        m_value -= internal::delta_for_piece_stand[p];
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
