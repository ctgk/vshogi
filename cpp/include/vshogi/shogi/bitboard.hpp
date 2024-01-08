#ifndef VSHOGI_SHOGI_BITBOARD_HPP
#define VSHOGI_SHOGI_BITBOARD_HPP

#include <algorithm>
#include <cstdint>

#include "vshogi/bitboard.hpp"
#include "vshogi/shogi/piece.hpp"
#include "vshogi/shogi/squares.hpp"

namespace vshogi::shogi
{

constexpr unsigned int num_attack_types = 15;
using BitBoard = vshogi::
    BitBoard<__uint128_t, Squares, BoardPieceTypeEnum, num_attack_types>;

// clang-format off
constexpr BitBoard bb_1a = (BitBoard(1) << static_cast<unsigned int>(SQ_1A));
constexpr BitBoard bb_1b = (BitBoard(1) << static_cast<unsigned int>(SQ_1B));
constexpr BitBoard bb_1c = (BitBoard(1) << static_cast<unsigned int>(SQ_1C));
constexpr BitBoard bb_1d = (BitBoard(1) << static_cast<unsigned int>(SQ_1D));
constexpr BitBoard bb_1e = (BitBoard(1) << static_cast<unsigned int>(SQ_1E));
constexpr BitBoard bb_1f = (BitBoard(1) << static_cast<unsigned int>(SQ_1F));
constexpr BitBoard bb_1g = (BitBoard(1) << static_cast<unsigned int>(SQ_1G));
constexpr BitBoard bb_1h = (BitBoard(1) << static_cast<unsigned int>(SQ_1H));
constexpr BitBoard bb_1i = (BitBoard(1) << static_cast<unsigned int>(SQ_1I));
constexpr BitBoard bb_2a = (BitBoard(1) << static_cast<unsigned int>(SQ_2A));
constexpr BitBoard bb_2b = (BitBoard(1) << static_cast<unsigned int>(SQ_2B));
constexpr BitBoard bb_2c = (BitBoard(1) << static_cast<unsigned int>(SQ_2C));
constexpr BitBoard bb_2d = (BitBoard(1) << static_cast<unsigned int>(SQ_2D));
constexpr BitBoard bb_2e = (BitBoard(1) << static_cast<unsigned int>(SQ_2E));
constexpr BitBoard bb_2f = (BitBoard(1) << static_cast<unsigned int>(SQ_2F));
constexpr BitBoard bb_2g = (BitBoard(1) << static_cast<unsigned int>(SQ_2G));
constexpr BitBoard bb_2h = (BitBoard(1) << static_cast<unsigned int>(SQ_2H));
constexpr BitBoard bb_2i = (BitBoard(1) << static_cast<unsigned int>(SQ_2I));
constexpr BitBoard bb_3a = (BitBoard(1) << static_cast<unsigned int>(SQ_3A));
constexpr BitBoard bb_3b = (BitBoard(1) << static_cast<unsigned int>(SQ_3B));
constexpr BitBoard bb_3c = (BitBoard(1) << static_cast<unsigned int>(SQ_3C));
constexpr BitBoard bb_3d = (BitBoard(1) << static_cast<unsigned int>(SQ_3D));
constexpr BitBoard bb_3e = (BitBoard(1) << static_cast<unsigned int>(SQ_3E));
constexpr BitBoard bb_3f = (BitBoard(1) << static_cast<unsigned int>(SQ_3F));
constexpr BitBoard bb_3g = (BitBoard(1) << static_cast<unsigned int>(SQ_3G));
constexpr BitBoard bb_3h = (BitBoard(1) << static_cast<unsigned int>(SQ_3H));
constexpr BitBoard bb_3i = (BitBoard(1) << static_cast<unsigned int>(SQ_3I));
constexpr BitBoard bb_4a = (BitBoard(1) << static_cast<unsigned int>(SQ_4A));
constexpr BitBoard bb_4b = (BitBoard(1) << static_cast<unsigned int>(SQ_4B));
constexpr BitBoard bb_4c = (BitBoard(1) << static_cast<unsigned int>(SQ_4C));
constexpr BitBoard bb_4d = (BitBoard(1) << static_cast<unsigned int>(SQ_4D));
constexpr BitBoard bb_4e = (BitBoard(1) << static_cast<unsigned int>(SQ_4E));
constexpr BitBoard bb_4f = (BitBoard(1) << static_cast<unsigned int>(SQ_4F));
constexpr BitBoard bb_4g = (BitBoard(1) << static_cast<unsigned int>(SQ_4G));
constexpr BitBoard bb_4h = (BitBoard(1) << static_cast<unsigned int>(SQ_4H));
constexpr BitBoard bb_4i = (BitBoard(1) << static_cast<unsigned int>(SQ_4I));
constexpr BitBoard bb_5a = (BitBoard(1) << static_cast<unsigned int>(SQ_5A));
constexpr BitBoard bb_5b = (BitBoard(1) << static_cast<unsigned int>(SQ_5B));
constexpr BitBoard bb_5c = (BitBoard(1) << static_cast<unsigned int>(SQ_5C));
constexpr BitBoard bb_5d = (BitBoard(1) << static_cast<unsigned int>(SQ_5D));
constexpr BitBoard bb_5e = (BitBoard(1) << static_cast<unsigned int>(SQ_5E));
constexpr BitBoard bb_5f = (BitBoard(1) << static_cast<unsigned int>(SQ_5F));
constexpr BitBoard bb_5g = (BitBoard(1) << static_cast<unsigned int>(SQ_5G));
constexpr BitBoard bb_5h = (BitBoard(1) << static_cast<unsigned int>(SQ_5H));
constexpr BitBoard bb_5i = (BitBoard(1) << static_cast<unsigned int>(SQ_5I));
constexpr BitBoard bb_6a = (BitBoard(1) << static_cast<unsigned int>(SQ_6A));
constexpr BitBoard bb_6b = (BitBoard(1) << static_cast<unsigned int>(SQ_6B));
constexpr BitBoard bb_6c = (BitBoard(1) << static_cast<unsigned int>(SQ_6C));
constexpr BitBoard bb_6d = (BitBoard(1) << static_cast<unsigned int>(SQ_6D));
constexpr BitBoard bb_6e = (BitBoard(1) << static_cast<unsigned int>(SQ_6E));
constexpr BitBoard bb_6f = (BitBoard(1) << static_cast<unsigned int>(SQ_6F));
constexpr BitBoard bb_6g = (BitBoard(1) << static_cast<unsigned int>(SQ_6G));
constexpr BitBoard bb_6h = (BitBoard(1) << static_cast<unsigned int>(SQ_6H));
constexpr BitBoard bb_6i = (BitBoard(1) << static_cast<unsigned int>(SQ_6I));
constexpr BitBoard bb_7a = (BitBoard(1) << static_cast<unsigned int>(SQ_7A));
constexpr BitBoard bb_7b = (BitBoard(1) << static_cast<unsigned int>(SQ_7B));
constexpr BitBoard bb_7c = (BitBoard(1) << static_cast<unsigned int>(SQ_7C));
constexpr BitBoard bb_7d = (BitBoard(1) << static_cast<unsigned int>(SQ_7D));
constexpr BitBoard bb_7e = (BitBoard(1) << static_cast<unsigned int>(SQ_7E));
constexpr BitBoard bb_7f = (BitBoard(1) << static_cast<unsigned int>(SQ_7F));
constexpr BitBoard bb_7g = (BitBoard(1) << static_cast<unsigned int>(SQ_7G));
constexpr BitBoard bb_7h = (BitBoard(1) << static_cast<unsigned int>(SQ_7H));
constexpr BitBoard bb_7i = (BitBoard(1) << static_cast<unsigned int>(SQ_7I));
constexpr BitBoard bb_8a = (BitBoard(1) << static_cast<unsigned int>(SQ_8A));
constexpr BitBoard bb_8b = (BitBoard(1) << static_cast<unsigned int>(SQ_8B));
constexpr BitBoard bb_8c = (BitBoard(1) << static_cast<unsigned int>(SQ_8C));
constexpr BitBoard bb_8d = (BitBoard(1) << static_cast<unsigned int>(SQ_8D));
constexpr BitBoard bb_8e = (BitBoard(1) << static_cast<unsigned int>(SQ_8E));
constexpr BitBoard bb_8f = (BitBoard(1) << static_cast<unsigned int>(SQ_8F));
constexpr BitBoard bb_8g = (BitBoard(1) << static_cast<unsigned int>(SQ_8G));
constexpr BitBoard bb_8h = (BitBoard(1) << static_cast<unsigned int>(SQ_8H));
constexpr BitBoard bb_8i = (BitBoard(1) << static_cast<unsigned int>(SQ_8I));
constexpr BitBoard bb_9a = (BitBoard(1) << static_cast<unsigned int>(SQ_9A));
constexpr BitBoard bb_9b = (BitBoard(1) << static_cast<unsigned int>(SQ_9B));
constexpr BitBoard bb_9c = (BitBoard(1) << static_cast<unsigned int>(SQ_9C));
constexpr BitBoard bb_9d = (BitBoard(1) << static_cast<unsigned int>(SQ_9D));
constexpr BitBoard bb_9e = (BitBoard(1) << static_cast<unsigned int>(SQ_9E));
constexpr BitBoard bb_9f = (BitBoard(1) << static_cast<unsigned int>(SQ_9F));
constexpr BitBoard bb_9g = (BitBoard(1) << static_cast<unsigned int>(SQ_9G));
constexpr BitBoard bb_9h = (BitBoard(1) << static_cast<unsigned int>(SQ_9H));
constexpr BitBoard bb_9i = (BitBoard(1) << static_cast<unsigned int>(SQ_9I));
constexpr BitBoard bb_file1 = bb_1a | bb_1b | bb_1c | bb_1d | bb_1e | bb_1f | bb_1g | bb_1h | bb_1i;
constexpr BitBoard bb_file2 = bb_2a | bb_2b | bb_2c | bb_2d | bb_2e | bb_2f | bb_2g | bb_2h | bb_2i;
constexpr BitBoard bb_file3 = bb_3a | bb_3b | bb_3c | bb_3d | bb_3e | bb_3f | bb_3g | bb_3h | bb_3i;
constexpr BitBoard bb_file4 = bb_4a | bb_4b | bb_4c | bb_4d | bb_4e | bb_4f | bb_4g | bb_4h | bb_4i;
constexpr BitBoard bb_file5 = bb_5a | bb_5b | bb_5c | bb_5d | bb_5e | bb_5f | bb_5g | bb_5h | bb_5i;
constexpr BitBoard bb_file6 = bb_6a | bb_6b | bb_6c | bb_6d | bb_6e | bb_6f | bb_6g | bb_6h | bb_6i;
constexpr BitBoard bb_file7 = bb_7a | bb_7b | bb_7c | bb_7d | bb_7e | bb_7f | bb_7g | bb_7h | bb_7i;
constexpr BitBoard bb_file8 = bb_8a | bb_8b | bb_8c | bb_8d | bb_8e | bb_8f | bb_8g | bb_8h | bb_8i;
constexpr BitBoard bb_file9 = bb_9a | bb_9b | bb_9c | bb_9d | bb_9e | bb_9f | bb_9g | bb_9h | bb_9i;
constexpr BitBoard bb_ranka = bb_1a | bb_2a | bb_3a | bb_4a | bb_5a | bb_6a | bb_7a | bb_8a | bb_9a;
constexpr BitBoard bb_rankb = bb_1b | bb_2b | bb_3b | bb_4b | bb_5b | bb_6b | bb_7b | bb_8b | bb_9b;
constexpr BitBoard bb_rankc = bb_1c | bb_2c | bb_3c | bb_4c | bb_5c | bb_6c | bb_7c | bb_8c | bb_9c;
constexpr BitBoard bb_rankd = bb_1d | bb_2d | bb_3d | bb_4d | bb_5d | bb_6d | bb_7d | bb_8d | bb_9d;
constexpr BitBoard bb_ranke = bb_1e | bb_2e | bb_3e | bb_4e | bb_5e | bb_6e | bb_7e | bb_8e | bb_9e;
constexpr BitBoard bb_rankf = bb_1f | bb_2f | bb_3f | bb_4f | bb_5f | bb_6f | bb_7f | bb_8f | bb_9f;
constexpr BitBoard bb_rankg = bb_1g | bb_2g | bb_3g | bb_4g | bb_5g | bb_6g | bb_7g | bb_8g | bb_9g;
constexpr BitBoard bb_rankh = bb_1h | bb_2h | bb_3h | bb_4h | bb_5h | bb_6h | bb_7h | bb_8h | bb_9h;
constexpr BitBoard bb_ranki = bb_1i | bb_2i | bb_3i | bb_4i | bb_5i | bb_6i | bb_7i | bb_8i | bb_9i;
// clang-format on

} // namespace vshogi::shogi

namespace vshogi
{

template <>
inline const shogi::BitBoard
    shogi::BitBoard::square_to_bitboard_array[shogi::Squares::num_squares + 1]
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
        BitBoard(1) << static_cast<unsigned int>(12),
        BitBoard(1) << static_cast<unsigned int>(13),
        BitBoard(1) << static_cast<unsigned int>(14),
        BitBoard(1) << static_cast<unsigned int>(15),
        BitBoard(1) << static_cast<unsigned int>(16),
        BitBoard(1) << static_cast<unsigned int>(17),
        BitBoard(1) << static_cast<unsigned int>(18),
        BitBoard(1) << static_cast<unsigned int>(19),
        BitBoard(1) << static_cast<unsigned int>(20),
        BitBoard(1) << static_cast<unsigned int>(21),
        BitBoard(1) << static_cast<unsigned int>(22),
        BitBoard(1) << static_cast<unsigned int>(23),
        BitBoard(1) << static_cast<unsigned int>(24),
        BitBoard(1) << static_cast<unsigned int>(25),
        BitBoard(1) << static_cast<unsigned int>(26),
        BitBoard(1) << static_cast<unsigned int>(27),
        BitBoard(1) << static_cast<unsigned int>(28),
        BitBoard(1) << static_cast<unsigned int>(29),
        BitBoard(1) << static_cast<unsigned int>(30),
        BitBoard(1) << static_cast<unsigned int>(31),
        BitBoard(1) << static_cast<unsigned int>(32),
        BitBoard(1) << static_cast<unsigned int>(33),
        BitBoard(1) << static_cast<unsigned int>(34),
        BitBoard(1) << static_cast<unsigned int>(35),
        BitBoard(1) << static_cast<unsigned int>(36),
        BitBoard(1) << static_cast<unsigned int>(37),
        BitBoard(1) << static_cast<unsigned int>(38),
        BitBoard(1) << static_cast<unsigned int>(39),
        BitBoard(1) << static_cast<unsigned int>(40),
        BitBoard(1) << static_cast<unsigned int>(41),
        BitBoard(1) << static_cast<unsigned int>(42),
        BitBoard(1) << static_cast<unsigned int>(43),
        BitBoard(1) << static_cast<unsigned int>(44),
        BitBoard(1) << static_cast<unsigned int>(45),
        BitBoard(1) << static_cast<unsigned int>(46),
        BitBoard(1) << static_cast<unsigned int>(47),
        BitBoard(1) << static_cast<unsigned int>(48),
        BitBoard(1) << static_cast<unsigned int>(49),
        BitBoard(1) << static_cast<unsigned int>(50),
        BitBoard(1) << static_cast<unsigned int>(51),
        BitBoard(1) << static_cast<unsigned int>(52),
        BitBoard(1) << static_cast<unsigned int>(53),
        BitBoard(1) << static_cast<unsigned int>(54),
        BitBoard(1) << static_cast<unsigned int>(55),
        BitBoard(1) << static_cast<unsigned int>(56),
        BitBoard(1) << static_cast<unsigned int>(57),
        BitBoard(1) << static_cast<unsigned int>(58),
        BitBoard(1) << static_cast<unsigned int>(59),
        BitBoard(1) << static_cast<unsigned int>(60),
        BitBoard(1) << static_cast<unsigned int>(61),
        BitBoard(1) << static_cast<unsigned int>(62),
        BitBoard(1) << static_cast<unsigned int>(63),
        BitBoard(1) << static_cast<unsigned int>(64),
        BitBoard(1) << static_cast<unsigned int>(65),
        BitBoard(1) << static_cast<unsigned int>(66),
        BitBoard(1) << static_cast<unsigned int>(67),
        BitBoard(1) << static_cast<unsigned int>(68),
        BitBoard(1) << static_cast<unsigned int>(69),
        BitBoard(1) << static_cast<unsigned int>(70),
        BitBoard(1) << static_cast<unsigned int>(71),
        BitBoard(1) << static_cast<unsigned int>(72),
        BitBoard(1) << static_cast<unsigned int>(73),
        BitBoard(1) << static_cast<unsigned int>(74),
        BitBoard(1) << static_cast<unsigned int>(75),
        BitBoard(1) << static_cast<unsigned int>(76),
        BitBoard(1) << static_cast<unsigned int>(77),
        BitBoard(1) << static_cast<unsigned int>(78),
        BitBoard(1) << static_cast<unsigned int>(79),
        BitBoard(1) << static_cast<unsigned int>(80),
        BitBoard(), // SQ_NA
};

template <>
inline shogi::BitBoard
    shogi::BitBoard::attacks_table[shogi::num_attack_types]
                                  [shogi::Squares::num_squares]
    = {};

template <>
inline int shogi::BitBoard::hamming_weight() const
{
    return hamming_weight_64bit(static_cast<std::uint64_t>(m_value))
           + hamming_weight_64bit(static_cast<std::uint64_t>(m_value >> 64));
}

template <>
constexpr shogi::BitBoard
shogi::BitBoard::get_promotion_zone(const ColorEnum& c)
{
    using namespace vshogi::shogi;
    return (c == BLACK) ? (bb_ranka | bb_rankb | bb_rankc)
                        : (bb_rankg | bb_rankh | bb_ranki);
}

template <>
template <DirectionEnum Dir>
constexpr shogi::BitBoard shogi::BitBoard::shift() const
{
    constexpr auto bb_f12345678 = ~shogi::bb_file9;
    constexpr auto bb_f23456789 = ~shogi::bb_file1;
    constexpr BitBoard filemask[] = {
        // clang-format off
        bb_f12345678, ~BitBoard(0), bb_f23456789,
        bb_f12345678,               bb_f23456789,
        bb_f12345678, ~BitBoard(0), bb_f23456789,
        bb_f12345678,               bb_f23456789,
        bb_f12345678,               bb_f23456789,
        // clang-format on
    };
    constexpr auto delta = shogi::Squares::direction_to_delta(Dir);
    if constexpr (delta > 0)
        return (*this & filemask[Dir]) << static_cast<unsigned int>(delta);
    else
        return (*this & filemask[Dir]) >> static_cast<unsigned int>(-delta);
}

template <>
inline shogi::BitBoard shogi::BitBoard::get_attacks_by(
    const vshogi::shogi::BoardPieceTypeEnum& p,
    const vshogi::shogi::SquareEnum& sq)
{
    using namespace vshogi::shogi;
    switch (p) {
    case B_FU:
        return attacks_table[0][sq];
    case B_KY:
        return attacks_table[1][sq];
    case B_KE:
        return attacks_table[2][sq];
    case B_GI:
        return attacks_table[3][sq];
    case B_KI:
    case B_TO:
    case B_NY:
    case B_NK:
    case B_NG:
        return attacks_table[4][sq];
    case W_FU:
        return attacks_table[5][sq];
    case W_KY:
        return attacks_table[6][sq];
    case W_KE:
        return attacks_table[7][sq];
    case W_GI:
        return attacks_table[8][sq];
    case W_KI:
    case W_TO:
    case W_NY:
    case W_NK:
    case W_NG:
        return attacks_table[9][sq];
    case B_KA:
    case W_KA:
        return attacks_table[10][sq];
    case B_HI:
    case W_HI:
        return attacks_table[11][sq];
    case B_UM:
    case W_UM:
        return attacks_table[12][sq];
    case B_RY:
    case W_RY:
        return attacks_table[13][sq];
    case B_OU:
    case W_OU:
        return attacks_table[14][sq];
    default:
        return BitBoard();
    }
}

template <>
inline shogi::BitBoard shogi::BitBoard::get_attacks_by(
    const vshogi::shogi::BoardPieceTypeEnum& p,
    const vshogi::shogi::SquareEnum& sq,
    const vshogi::shogi::BitBoard& occupied)
{
    using namespace vshogi::shogi;
    switch (p) {
    case B_FU:
        return attacks_table[0][sq];
    case B_KY:
        return BitBoard::ranging_attacks_to<DIR_N>(sq, occupied);
    case B_KE:
        return attacks_table[2][sq];
    case B_GI:
        return attacks_table[3][sq];
    case B_KI:
    case B_TO:
    case B_NY:
    case B_NK:
    case B_NG:
        return attacks_table[4][sq];
    case W_FU:
        return attacks_table[5][sq];
    case W_KY:
        return BitBoard::ranging_attacks_to<DIR_S>(sq, occupied);
    case W_KE:
        return attacks_table[7][sq];
    case W_GI:
        return attacks_table[8][sq];
    case W_KI:
    case W_TO:
    case W_NY:
    case W_NK:
    case W_NG:
        return attacks_table[9][sq];
    case B_KA:
    case W_KA:
        return BitBoard::ranging_attacks_to_diagonal(sq, occupied);
    case B_HI:
    case W_HI:
        return BitBoard::ranging_attacks_to_adjacent(sq, occupied);
    case B_UM:
    case W_UM:
        return BitBoard::ranging_attacks_to_diagonal(sq, occupied)
               | attacks_table[14][sq];
    case B_RY:
    case W_RY:
        return BitBoard::ranging_attacks_to_adjacent(sq, occupied)
               | attacks_table[14][sq];
    case B_OU:
    case W_OU:
        return attacks_table[14][sq];
    default:
        return BitBoard();
    }
}

template <>
inline void shogi::BitBoard::init_tables()
{
    for (auto&& sq : shogi::Squares::square_array) {
        const auto b = from_square(sq);
        // clang-format off
        attacks_table[0][sq] = b.shift<DIR_N>(); // B_FU
        attacks_table[1][sq] = BitBoard::ranging_attacks_to<DIR_N>(sq); // B_KY
        attacks_table[2][sq] = b.shift<DIR_NNW>() | b.shift<DIR_NNE>(); // B_KE
        attacks_table[3][sq] = b.shift<DIR_NW>() | b.shift<DIR_N>() | b.shift<DIR_NE>() | b.shift<DIR_SW>() | b.shift<DIR_SE>(); // B_GI
        attacks_table[4][sq] = b.shift<DIR_NW>() | b.shift<DIR_N>() | b.shift<DIR_NE>() | b.shift<DIR_W>() | b.shift<DIR_E>() | b.shift<DIR_S>(); // B_KI
        attacks_table[5][sq] = b.shift<DIR_S>(); // W_FU
        attacks_table[6][sq] = BitBoard::ranging_attacks_to<DIR_S>(sq); // W_KY
        attacks_table[7][sq] = b.shift<DIR_SSW>() | b.shift<DIR_SSE>(); // W_KE
        attacks_table[8][sq] = b.shift<DIR_NW>() | b.shift<DIR_NE>() | b.shift<DIR_SW>() | b.shift<DIR_S>() | b.shift<DIR_SE>(); // W_GI
        attacks_table[9][sq] = b.shift<DIR_N>() | b.shift<DIR_W>() | b.shift<DIR_E>() | b.shift<DIR_SW>() | b.shift<DIR_S>() | b.shift<DIR_SE>(); // W_KI
        attacks_table[10][sq] = BitBoard::ranging_attacks_to_diagonal(sq); // KA
        attacks_table[11][sq] = BitBoard::ranging_attacks_to_adjacent(sq); // HI
        attacks_table[12][sq] = attacks_table[10][sq] | attacks_table[4][sq]; // UM
        attacks_table[13][sq] = attacks_table[11][sq] | attacks_table[3][sq]; // RY
        attacks_table[14][sq] = attacks_table[3][sq] | attacks_table[4][sq]; // OU
        // clang-format on
    }
}

} // namespace vshogi

#endif // VSHOGI_SHOGI_BITBOARD_HPP
