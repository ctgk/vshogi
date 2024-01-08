#ifndef VSHOGI_ANIMAL_SHOGI_BITBOARD_HPP
#define VSHOGI_ANIMAL_SHOGI_BITBOARD_HPP

#include <cstdint>

#include "vshogi/animal_shogi/piece.hpp"
#include "vshogi/animal_shogi/squares.hpp"
#include "vshogi/bitboard.hpp"
#include "vshogi/color.hpp"

namespace vshogi::animal_shogi
{

constexpr unsigned int num_attack_types = 7;
using BitBoard = vshogi::
    BitBoard<std::uint16_t, Squares, BoardPieceTypeEnum, num_attack_types>;

constexpr BitBoard bb_a1 = (BitBoard(1) << static_cast<unsigned int>(SQ_A1));
constexpr BitBoard bb_b1 = (BitBoard(1) << static_cast<unsigned int>(SQ_B1));
constexpr BitBoard bb_c1 = (BitBoard(1) << static_cast<unsigned int>(SQ_C1));
constexpr BitBoard bb_a2 = (BitBoard(1) << static_cast<unsigned int>(SQ_A2));
constexpr BitBoard bb_b2 = (BitBoard(1) << static_cast<unsigned int>(SQ_B2));
constexpr BitBoard bb_c2 = (BitBoard(1) << static_cast<unsigned int>(SQ_C2));
constexpr BitBoard bb_a3 = (BitBoard(1) << static_cast<unsigned int>(SQ_A3));
constexpr BitBoard bb_b3 = (BitBoard(1) << static_cast<unsigned int>(SQ_B3));
constexpr BitBoard bb_c3 = (BitBoard(1) << static_cast<unsigned int>(SQ_C3));
constexpr BitBoard bb_a4 = (BitBoard(1) << static_cast<unsigned int>(SQ_A4));
constexpr BitBoard bb_b4 = (BitBoard(1) << static_cast<unsigned int>(SQ_B4));
constexpr BitBoard bb_c4 = (BitBoard(1) << static_cast<unsigned int>(SQ_C4));

constexpr BitBoard bb_rank1 = BitBoard(0b000000000111);
constexpr BitBoard bb_rank2 = BitBoard(0b000000111000);
constexpr BitBoard bb_rank3 = BitBoard(0b000111000000);
constexpr BitBoard bb_rank4 = BitBoard(0b111000000000);

constexpr BitBoard bb_filea = BitBoard(0b001001001001);
constexpr BitBoard bb_fileb = BitBoard(0b010010010010);
constexpr BitBoard bb_filec = BitBoard(0b100100100100);

} // namespace vshogi::animal_shogi

namespace vshogi
{

template <>
inline const animal_shogi::BitBoard animal_shogi::BitBoard::
    square_to_bitboard_array[animal_shogi::Squares::num_squares + 1]
    = {
        BitBoard(1) << static_cast<unsigned int>(0),
        BitBoard(1) << static_cast<unsigned int>(1),
        BitBoard(1) << static_cast<unsigned int>(2),
        BitBoard(1) << static_cast<unsigned int>(3),
        BitBoard(1) << static_cast<unsigned int>(4),
        BitBoard(1) << static_cast<unsigned int>(5),
        BitBoard(1) << static_cast<unsigned int>(6),
        BitBoard(1) << static_cast<unsigned int>(7),
        BitBoard(1) << static_cast<unsigned int>(8),
        BitBoard(1) << static_cast<unsigned int>(9),
        BitBoard(1) << static_cast<unsigned int>(10),
        BitBoard(1) << static_cast<unsigned int>(11),
        BitBoard(),
};

template <>
inline animal_shogi::BitBoard
    animal_shogi::BitBoard::attacks_table[animal_shogi::num_attack_types]
                                         [animal_shogi::Squares::num_squares]
    = {};

template <>
constexpr animal_shogi::BitBoard
animal_shogi::BitBoard::get_promotion_zone(const ColorEnum& c)
{
    return (c == BLACK) ? animal_shogi::bb_rank1 : animal_shogi::bb_rank4;
}

template <>
template <DirectionEnum Dir>
constexpr animal_shogi::BitBoard animal_shogi::BitBoard::shift() const
{
    constexpr auto bb_fab = animal_shogi::bb_filea | animal_shogi::bb_fileb;
    constexpr auto bb_fbc = animal_shogi::bb_fileb | animal_shogi::bb_filec;
    constexpr BitBoard filemask[] = {
        // clang-format off
        bb_fbc, ~BitBoard(0), bb_fab,
        bb_fbc,               bb_fab,
        bb_fbc, ~BitBoard(0), bb_fab,
        bb_fbc,               bb_fab,
        bb_fbc,               bb_fab,
        // clang-format on
    };
    constexpr auto delta = animal_shogi::Squares::direction_to_delta(Dir);
    if constexpr (delta > 0)
        return (*this & filemask[Dir]) << static_cast<unsigned int>(delta);
    else
        return (*this & filemask[Dir]) >> static_cast<unsigned int>(-delta);
}

template <>
inline animal_shogi::BitBoard animal_shogi::BitBoard::get_attacks_by(
    const animal_shogi::BoardPieceTypeEnum& p,
    const animal_shogi::SquareEnum& sq)
{
    using namespace vshogi::animal_shogi;
    switch (p) {
    case B_CH:
        return attacks_table[0][sq];
    case B_HE:
        return attacks_table[1][sq];
    case W_CH:
        return attacks_table[2][sq];
    case W_HE:
        return attacks_table[3][sq];
    case B_EL:
    case W_EL:
        return attacks_table[4][sq];
    case B_GI:
    case W_GI:
        return attacks_table[5][sq];
    case B_LI:
    case W_LI:
        return attacks_table[6][sq];
    default:
        return BitBoard();
    }
}

template <>
inline void animal_shogi::BitBoard::init_tables()
{
    for (auto&& sq : animal_shogi::Squares::square_array) {
        const auto b = from_square(sq);
        // clang-format off
        attacks_table[0][sq] = b.shift<DIR_N>(); // B_CH
        attacks_table[1][sq] = b.shift<DIR_NW>() | b.shift<DIR_N>() | b.shift<DIR_NE>() | b.shift<DIR_W>() | b.shift<DIR_E>() | b.shift<DIR_S>(); // B_KI
        attacks_table[2][sq] = b.shift<DIR_S>(); // W_CH
        attacks_table[3][sq] = b.shift<DIR_N>() | b.shift<DIR_W>() | b.shift<DIR_E>() | b.shift<DIR_SW>() | b.shift<DIR_S>() | b.shift<DIR_SE>(); // W_KI
        attacks_table[4][sq] = b.shift<DIR_NW>() | b.shift<DIR_NE>() | b.shift<DIR_SW>() | b.shift<DIR_SE>(); // EL
        attacks_table[5][sq] = b.shift<DIR_N>() | b.shift<DIR_W>() | b.shift<DIR_E>() | b.shift<DIR_S>(); // GI
        attacks_table[6][sq] = attacks_table[4][sq] | attacks_table[5][sq]; // LI
        // clang-format on
    }
}

} // namespace vshogi

#endif // VSHOGI_ANIMAL_SHOGI_BITBOARD_HPP
