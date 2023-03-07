#ifndef VSHOGI_ANIMAL_SHOGI_BITBOARD_HPP
#define VSHOGI_ANIMAL_SHOGI_BITBOARD_HPP

#include <cstdint>

#include "vshogi/animal_shogi/color.hpp"
#include "vshogi/animal_shogi/squares.hpp"

namespace vshogi::animal_shogi
{

/**
 * @brief Binary board representing ON and OFF of each squares.
 * @details
 *            *------------------- C4
 *            |           *------- B1
 *            |           |*------ A1
 *            |           ||
 *            v           vv
 * (MSB) xxxx xxxx xxxx xxxx (LSB)
 *
 */
class BitBoard
{
private:
    std::uint16_t m_value;

public:
    constexpr BitBoard() : m_value(0U)
    {
    }
    constexpr BitBoard(const std::uint16_t v) : m_value(v & 0x0fff)
    {
    }
    constexpr BitBoard operator|(const BitBoard other) const
    {
        return BitBoard(m_value | other.m_value);
    }
    bool operator==(const BitBoard other) const
    {
        return m_value == other.m_value;
    }
    bool operator!=(const BitBoard other) const
    {
        return m_value != other.m_value;
    }
    constexpr BitBoard one_rank_above() const
    {
        return BitBoard(static_cast<std::uint16_t>(m_value >> 3));
    }
    constexpr BitBoard one_rank_below() const
    {
        return BitBoard(static_cast<std::uint16_t>(m_value << 3));
    }
    constexpr BitBoard one_file_right() const;
    constexpr BitBoard one_file_left() const;
    bool is_one(const SquareEnum sq) const
    {
        return static_cast<bool>(
            (1 << static_cast<std::uint16_t>(sq)) & m_value);
    }
};

constexpr BitBoard rank_1_mask = BitBoard(0b000000000111);
constexpr BitBoard rank_2_mask = BitBoard(0b000000111000);
constexpr BitBoard rank_3_mask = BitBoard(0b000111000000);
constexpr BitBoard rank_4_mask = BitBoard(0b111000000000);
constexpr BitBoard rank_masks[]
    = {rank_1_mask, rank_2_mask, rank_3_mask, rank_4_mask};

constexpr BitBoard file_a_mask = BitBoard(0b001001001001);
constexpr BitBoard file_b_mask = BitBoard(0b010010010010);
constexpr BitBoard file_c_mask = BitBoard(0b100100100100);
constexpr BitBoard file_masks[] = {file_a_mask, file_b_mask, file_c_mask};

constexpr BitBoard square_a1_mask = BitBoard(0b000000000001);
constexpr BitBoard square_b1_mask = BitBoard(0b000000000010);
constexpr BitBoard square_c1_mask = BitBoard(0b000000000100);
constexpr BitBoard square_a2_mask = BitBoard(0b000000001000);
constexpr BitBoard square_b2_mask = BitBoard(0b000000010000);
constexpr BitBoard square_c2_mask = BitBoard(0b000000100000);
constexpr BitBoard square_a3_mask = BitBoard(0b000001000000);
constexpr BitBoard square_b3_mask = BitBoard(0b000010000000);
constexpr BitBoard square_c3_mask = BitBoard(0b000100000000);
constexpr BitBoard square_a4_mask = BitBoard(0b001000000000);
constexpr BitBoard square_b4_mask = BitBoard(0b010000000000);
constexpr BitBoard square_c4_mask = BitBoard(0b100000000000);
constexpr BitBoard square_masks[] = {
    square_a1_mask,
    square_b1_mask,
    square_c1_mask,
    square_a2_mask,
    square_b2_mask,
    square_c2_mask,
    square_a3_mask,
    square_b3_mask,
    square_c3_mask,
    square_a4_mask,
    square_b4_mask,
    square_c4_mask,
};

constexpr BitBoard BitBoard::one_file_right() const
{
    return BitBoard(
        static_cast<std::uint16_t>((m_value & (~file_c_mask.m_value)) << 1));
}

constexpr BitBoard BitBoard::one_file_left() const
{
    return BitBoard(
        static_cast<std::uint16_t>((m_value & (~file_a_mask.m_value)) >> 1));
}

constexpr BitBoard chick_attacks[NUM_SQUARES][NUM_COLORS] = {
    {square_a1_mask.one_rank_above(), square_a1_mask.one_rank_below()},
    {square_b1_mask.one_rank_above(), square_b1_mask.one_rank_below()},
    {square_c1_mask.one_rank_above(), square_c1_mask.one_rank_below()},
    {square_a2_mask.one_rank_above(), square_a2_mask.one_rank_below()},
    {square_b2_mask.one_rank_above(), square_b2_mask.one_rank_below()},
    {square_c2_mask.one_rank_above(), square_c2_mask.one_rank_below()},
    {square_a3_mask.one_rank_above(), square_a3_mask.one_rank_below()},
    {square_b3_mask.one_rank_above(), square_b3_mask.one_rank_below()},
    {square_c3_mask.one_rank_above(), square_c3_mask.one_rank_below()},
    {square_a4_mask.one_rank_above(), square_a4_mask.one_rank_below()},
    {square_b4_mask.one_rank_above(), square_b4_mask.one_rank_below()},
    {square_c4_mask.one_rank_above(), square_c4_mask.one_rank_below()},
};

namespace internal
{

constexpr BitBoard el_attack_sqb3
    = square_a2_mask | square_c2_mask | square_a4_mask | square_c4_mask;
constexpr BitBoard gi_attack_sqb3
    = square_a3_mask | square_b2_mask | square_c3_mask | square_b4_mask;

} // namespace internal

constexpr BitBoard elephant_attacks[NUM_SQUARES] = {
    internal::el_attack_sqb3.one_rank_above().one_rank_above().one_file_left(),
    internal::el_attack_sqb3.one_rank_above().one_rank_above(),
    internal::el_attack_sqb3.one_rank_above().one_rank_above().one_file_right(),
    internal::el_attack_sqb3.one_rank_above().one_file_left(),
    internal::el_attack_sqb3.one_rank_above(),
    internal::el_attack_sqb3.one_rank_above().one_file_right(),
    internal::el_attack_sqb3.one_file_left(),
    internal::el_attack_sqb3,
    internal::el_attack_sqb3.one_file_right(),
    internal::el_attack_sqb3.one_rank_below().one_file_left(),
    internal::el_attack_sqb3.one_rank_below(),
    internal::el_attack_sqb3.one_rank_below().one_file_right(),
};
constexpr BitBoard giraffe_attacks[NUM_SQUARES] = {
    internal::gi_attack_sqb3.one_rank_above().one_rank_above().one_file_left(),
    internal::gi_attack_sqb3.one_rank_above().one_rank_above(),
    internal::gi_attack_sqb3.one_rank_above().one_rank_above().one_file_right(),
    internal::gi_attack_sqb3.one_rank_above().one_file_left(),
    internal::gi_attack_sqb3.one_rank_above(),
    internal::gi_attack_sqb3.one_rank_above().one_file_right(),
    internal::gi_attack_sqb3.one_file_left(),
    internal::gi_attack_sqb3,
    internal::gi_attack_sqb3.one_file_right(),
    internal::gi_attack_sqb3.one_rank_below().one_file_left(),
    internal::gi_attack_sqb3.one_rank_below(),
    internal::gi_attack_sqb3.one_rank_below().one_file_right(),
};
constexpr BitBoard lion_attacks[NUM_SQUARES] = {
    elephant_attacks[SQ_A1] | giraffe_attacks[SQ_A1],
    elephant_attacks[SQ_B1] | giraffe_attacks[SQ_B1],
    elephant_attacks[SQ_C1] | giraffe_attacks[SQ_C1],
    elephant_attacks[SQ_A2] | giraffe_attacks[SQ_A2],
    elephant_attacks[SQ_B2] | giraffe_attacks[SQ_B2],
    elephant_attacks[SQ_C2] | giraffe_attacks[SQ_C2],
    elephant_attacks[SQ_A3] | giraffe_attacks[SQ_A3],
    elephant_attacks[SQ_B3] | giraffe_attacks[SQ_B3],
    elephant_attacks[SQ_C3] | giraffe_attacks[SQ_C3],
    elephant_attacks[SQ_A4] | giraffe_attacks[SQ_A4],
    elephant_attacks[SQ_B4] | giraffe_attacks[SQ_B4],
    elephant_attacks[SQ_C4] | giraffe_attacks[SQ_C4],
};

} // namespace vshogi::animal_shogi

#endif // VSHOGI_ANIMAL_SHOGI_BITBOARD_HPP
