#ifndef VSHOGI_ANIMAL_SHOGI_BITBOARD_HPP
#define VSHOGI_ANIMAL_SHOGI_BITBOARD_HPP

#include <cstdint>

#include "vshogi/animal_shogi/piece.hpp"
#include "vshogi/animal_shogi/squares.hpp"
#include "vshogi/color.hpp"

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
    using SquareEnum = Squares::SquareEnum;
    static constexpr auto direction_to_delta = Squares::direction_to_delta;
    static constexpr auto SQ_A4 = Squares::SQ_A4; // NOLINT
    static constexpr auto SQ_A3 = Squares::SQ_A3; // NOLINT
    static constexpr auto SQ_A2 = Squares::SQ_A2; // NOLINT
    static constexpr auto SQ_A1 = Squares::SQ_A1; // NOLINT
    static constexpr auto SQ_B4 = Squares::SQ_B4; // NOLINT
    static constexpr auto SQ_B3 = Squares::SQ_B3; // NOLINT
    static constexpr auto SQ_B2 = Squares::SQ_B2; // NOLINT
    static constexpr auto SQ_B1 = Squares::SQ_B1; // NOLINT
    static constexpr auto SQ_C4 = Squares::SQ_C4; // NOLINT
    static constexpr auto SQ_C3 = Squares::SQ_C3; // NOLINT
    static constexpr auto SQ_C2 = Squares::SQ_C2; // NOLINT
    static constexpr auto SQ_C1 = Squares::SQ_C1; // NOLINT
    std::uint16_t m_value;

public:
    constexpr BitBoard() : m_value(0U)
    {
    }
    constexpr BitBoard(const std::uint16_t v) : m_value(v & 0x0fff)
    {
    }
    constexpr static BitBoard from_square(const SquareEnum sq)
    {
        return BitBoard(static_cast<std::uint16_t>(1 << static_cast<int>(sq)));
    }
    constexpr std::uint16_t get_value() const
    {
        return m_value;
    }
    constexpr BitBoard operator|(const BitBoard other) const
    {
        return BitBoard(m_value | other.m_value);
    }
    BitBoard& operator|=(const BitBoard other)
    {
        m_value = static_cast<std::uint16_t>(m_value | other.m_value);
        return *this;
    }
    constexpr BitBoard operator&(const BitBoard other) const
    {
        return BitBoard(m_value & other.m_value);
    }
    BitBoard& operator&=(const BitBoard other)
    {
        m_value = static_cast<std::uint16_t>(m_value & other.m_value);
        return *this;
    }
    constexpr BitBoard operator~() const
    {
        return BitBoard(static_cast<std::uint16_t>(~m_value));
    }
    constexpr BitBoard operator<<(const int shift_width) const
    {
        return (shift_width > 0) ? BitBoard(
                   static_cast<std::uint16_t>(m_value << shift_width))
                                 : BitBoard(static_cast<std::uint16_t>(
                                     m_value >> -shift_width));
    }
    bool operator==(const BitBoard other) const
    {
        return m_value == other.m_value;
    }
    bool operator!=(const BitBoard other) const
    {
        return m_value != other.m_value;
    }
    template <DirectionEnum D>
    constexpr BitBoard shift() const
    {
        constexpr auto bb_fab
            = ~(BitBoard::from_square(SQ_C1) | BitBoard::from_square(SQ_C2)
                | BitBoard::from_square(SQ_C3) | BitBoard::from_square(SQ_C4));
        constexpr auto bb_fbc
            = ~(BitBoard::from_square(SQ_A1) | BitBoard::from_square(SQ_A2)
                | BitBoard::from_square(SQ_A3) | BitBoard::from_square(SQ_A4));
        constexpr BitBoard mask[] = {
            // clang-format off
            bb_fbc, ~BitBoard(), bb_fab,
            bb_fbc,              bb_fab,
            bb_fbc, ~BitBoard(), bb_fab,
            // clang-format on
        };
        return (*this & mask[D]) << direction_to_delta(D);
    }
    bool any() const
    {
        return m_value > 0;
    }
    bool is_one(const SquareEnum sq) const
    {
        return static_cast<bool>(
            (1 << static_cast<std::uint16_t>(sq)) & m_value);
    }
    static constexpr BitBoard get_attacks_by(
        const Pieces::BoardPieceTypeEnum pieces,
        const Squares::SquareEnum location);
    static constexpr BitBoard get_attacks_by(
        const Pieces::BoardPieceTypeEnum pieces,
        const Squares::SquareEnum location,
        const BitBoard)
    {
        return get_attacks_by(pieces, location);
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

constexpr BitBoard bb_a1 = BitBoard::from_square(Squares::SQ_A1);
constexpr BitBoard bb_b1 = BitBoard::from_square(Squares::SQ_B1);
constexpr BitBoard bb_c1 = BitBoard::from_square(Squares::SQ_C1);
constexpr BitBoard bb_a2 = BitBoard::from_square(Squares::SQ_A2);
constexpr BitBoard bb_b2 = BitBoard::from_square(Squares::SQ_B2);
constexpr BitBoard bb_c2 = BitBoard::from_square(Squares::SQ_C2);
constexpr BitBoard bb_a3 = BitBoard::from_square(Squares::SQ_A3);
constexpr BitBoard bb_b3 = BitBoard::from_square(Squares::SQ_B3);
constexpr BitBoard bb_c3 = BitBoard::from_square(Squares::SQ_C3);
constexpr BitBoard bb_a4 = BitBoard::from_square(Squares::SQ_A4);
constexpr BitBoard bb_b4 = BitBoard::from_square(Squares::SQ_B4);
constexpr BitBoard bb_c4 = BitBoard::from_square(Squares::SQ_C4);
constexpr BitBoard square_masks[] = {
    bb_a1,
    bb_b1,
    bb_c1,
    bb_a2,
    bb_b2,
    bb_c2,
    bb_a3,
    bb_b3,
    bb_c3,
    bb_a4,
    bb_b4,
    bb_c4,
};

namespace internal
{

constexpr BitBoard el_attack_sqb3 = bb_a2 | bb_c2 | bb_a4 | bb_c4;
constexpr BitBoard gi_attack_sqb3 = bb_a3 | bb_b2 | bb_c3 | bb_b4;
constexpr BitBoard he_attack_sqb3[]
    = {gi_attack_sqb3 | bb_a2 | bb_c2, gi_attack_sqb3 | bb_a4 | bb_c4};

} // namespace internal

constexpr BitBoard chick_attacks[Squares::num_squares][num_colors] = {
    {bb_a1.shift<DIR_N>(), bb_a1.shift<DIR_S>()},
    {bb_b1.shift<DIR_N>(), bb_b1.shift<DIR_S>()},
    {bb_c1.shift<DIR_N>(), bb_c1.shift<DIR_S>()},
    {bb_a2.shift<DIR_N>(), bb_a2.shift<DIR_S>()},
    {bb_b2.shift<DIR_N>(), bb_b2.shift<DIR_S>()},
    {bb_c2.shift<DIR_N>(), bb_c2.shift<DIR_S>()},
    {bb_a3.shift<DIR_N>(), bb_a3.shift<DIR_S>()},
    {bb_b3.shift<DIR_N>(), bb_b3.shift<DIR_S>()},
    {bb_c3.shift<DIR_N>(), bb_c3.shift<DIR_S>()},
    {bb_a4.shift<DIR_N>(), bb_a4.shift<DIR_S>()},
    {bb_b4.shift<DIR_N>(), bb_b4.shift<DIR_S>()},
    {bb_c4.shift<DIR_N>(), bb_c4.shift<DIR_S>()},
};

constexpr BitBoard elephant_attacks[Squares::num_squares] = {
    internal::el_attack_sqb3.shift<DIR_N>().shift<DIR_N>().shift<DIR_W>(),
    internal::el_attack_sqb3.shift<DIR_N>().shift<DIR_N>(),
    internal::el_attack_sqb3.shift<DIR_N>().shift<DIR_N>().shift<DIR_E>(),
    internal::el_attack_sqb3.shift<DIR_N>().shift<DIR_W>(),
    internal::el_attack_sqb3.shift<DIR_N>(),
    internal::el_attack_sqb3.shift<DIR_N>().shift<DIR_E>(),
    internal::el_attack_sqb3.shift<DIR_W>(),
    internal::el_attack_sqb3,
    internal::el_attack_sqb3.shift<DIR_E>(),
    internal::el_attack_sqb3.shift<DIR_S>().shift<DIR_W>(),
    internal::el_attack_sqb3.shift<DIR_S>(),
    internal::el_attack_sqb3.shift<DIR_S>().shift<DIR_E>(),
};
constexpr BitBoard giraffe_attacks[Squares::num_squares] = {
    internal::gi_attack_sqb3.shift<DIR_N>().shift<DIR_N>().shift<DIR_W>(),
    internal::gi_attack_sqb3.shift<DIR_N>().shift<DIR_N>(),
    internal::gi_attack_sqb3.shift<DIR_N>().shift<DIR_N>().shift<DIR_E>(),
    internal::gi_attack_sqb3.shift<DIR_N>().shift<DIR_W>(),
    internal::gi_attack_sqb3.shift<DIR_N>(),
    internal::gi_attack_sqb3.shift<DIR_N>().shift<DIR_E>(),
    internal::gi_attack_sqb3.shift<DIR_W>(),
    internal::gi_attack_sqb3,
    internal::gi_attack_sqb3.shift<DIR_E>(),
    internal::gi_attack_sqb3.shift<DIR_S>().shift<DIR_W>(),
    internal::gi_attack_sqb3.shift<DIR_S>(),
    internal::gi_attack_sqb3.shift<DIR_S>().shift<DIR_E>(),
};
constexpr BitBoard lion_attacks[Squares::num_squares] = {
    elephant_attacks[Squares::SQ_A1] | giraffe_attacks[Squares::SQ_A1],
    elephant_attacks[Squares::SQ_B1] | giraffe_attacks[Squares::SQ_B1],
    elephant_attacks[Squares::SQ_C1] | giraffe_attacks[Squares::SQ_C1],
    elephant_attacks[Squares::SQ_A2] | giraffe_attacks[Squares::SQ_A2],
    elephant_attacks[Squares::SQ_B2] | giraffe_attacks[Squares::SQ_B2],
    elephant_attacks[Squares::SQ_C2] | giraffe_attacks[Squares::SQ_C2],
    elephant_attacks[Squares::SQ_A3] | giraffe_attacks[Squares::SQ_A3],
    elephant_attacks[Squares::SQ_B3] | giraffe_attacks[Squares::SQ_B3],
    elephant_attacks[Squares::SQ_C3] | giraffe_attacks[Squares::SQ_C3],
    elephant_attacks[Squares::SQ_A4] | giraffe_attacks[Squares::SQ_A4],
    elephant_attacks[Squares::SQ_B4] | giraffe_attacks[Squares::SQ_B4],
    elephant_attacks[Squares::SQ_C4] | giraffe_attacks[Squares::SQ_C4],
};
constexpr BitBoard hen_attacks[Squares::num_squares][num_colors] = {
    {internal::he_attack_sqb3[BLACK]
         .shift<DIR_N>()
         .shift<DIR_N>()
         .shift<DIR_W>(),
     internal::he_attack_sqb3[WHITE]
         .shift<DIR_N>()
         .shift<DIR_N>()
         .shift<DIR_W>()},
    {internal::he_attack_sqb3[BLACK].shift<DIR_N>().shift<DIR_N>(),
     internal::he_attack_sqb3[WHITE].shift<DIR_N>().shift<DIR_N>()},
    {internal::he_attack_sqb3[BLACK]
         .shift<DIR_N>()
         .shift<DIR_N>()
         .shift<DIR_E>(),
     internal::he_attack_sqb3[WHITE]
         .shift<DIR_N>()
         .shift<DIR_N>()
         .shift<DIR_E>()},
    {internal::he_attack_sqb3[BLACK].shift<DIR_N>().shift<DIR_W>(),
     internal::he_attack_sqb3[WHITE].shift<DIR_N>().shift<DIR_W>()},
    {internal::he_attack_sqb3[BLACK].shift<DIR_N>(),
     internal::he_attack_sqb3[WHITE].shift<DIR_N>()},
    {internal::he_attack_sqb3[BLACK].shift<DIR_N>().shift<DIR_E>(),
     internal::he_attack_sqb3[WHITE].shift<DIR_N>().shift<DIR_E>()},
    {internal::he_attack_sqb3[BLACK].shift<DIR_W>(),
     internal::he_attack_sqb3[WHITE].shift<DIR_W>()},
    {internal::he_attack_sqb3[BLACK], internal::he_attack_sqb3[WHITE]},
    {internal::he_attack_sqb3[BLACK].shift<DIR_E>(),
     internal::he_attack_sqb3[WHITE].shift<DIR_E>()},
    {internal::he_attack_sqb3[BLACK].shift<DIR_S>().shift<DIR_W>(),
     internal::he_attack_sqb3[WHITE].shift<DIR_S>().shift<DIR_W>()},
    {internal::he_attack_sqb3[BLACK].shift<DIR_S>(),
     internal::he_attack_sqb3[WHITE].shift<DIR_S>()},
    {internal::he_attack_sqb3[BLACK].shift<DIR_S>().shift<DIR_E>(),
     internal::he_attack_sqb3[WHITE].shift<DIR_S>().shift<DIR_E>()},
};

constexpr BitBoard BitBoard::get_attacks_by(
    const Pieces::BoardPieceTypeEnum piece, const Squares::SquareEnum location)
{
    switch (piece) {
    case Pieces::B_CH:
        return chick_attacks[location][BLACK];
    case Pieces::W_CH:
        return chick_attacks[location][WHITE];
    case Pieces::B_EL: // fall-through
    case Pieces::W_EL:
        return elephant_attacks[location];
    case Pieces::B_GI: // fall-through
    case Pieces::W_GI:
        return giraffe_attacks[location];
    case Pieces::B_LI: // fall-through
    case Pieces::W_LI:
        return lion_attacks[location];
    case Pieces::B_HE:
        return hen_attacks[location][BLACK];
    case Pieces::W_HE:
        return hen_attacks[location][WHITE];
    default:
        break;
    }
    return BitBoard();
}

} // namespace vshogi::animal_shogi

#endif // VSHOGI_ANIMAL_SHOGI_BITBOARD_HPP
