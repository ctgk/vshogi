#ifndef VSHOGI_JUDKINS_SHOGI_BITBOARD_HPP
#define VSHOGI_JUDKINS_SHOGI_BITBOARD_HPP

#include <cstdint>

#include "vshogi/bitboard.hpp"
#include "vshogi/judkins_shogi/piece.hpp"
#include "vshogi/judkins_shogi/squares.hpp"

namespace vshogi::judkins_shogi
{

constexpr unsigned int num_attack_types = 13;
using BitBoard = vshogi::
    BitBoard<std::uint64_t, Squares, BoardPieceTypeEnum, num_attack_types>;

constexpr BitBoard bb_1a = (BitBoard(1) << static_cast<unsigned int>(SQ_1A));
constexpr BitBoard bb_1b = (BitBoard(1) << static_cast<unsigned int>(SQ_1B));
constexpr BitBoard bb_1c = (BitBoard(1) << static_cast<unsigned int>(SQ_1C));
constexpr BitBoard bb_1d = (BitBoard(1) << static_cast<unsigned int>(SQ_1D));
constexpr BitBoard bb_1e = (BitBoard(1) << static_cast<unsigned int>(SQ_1E));
constexpr BitBoard bb_1f = (BitBoard(1) << static_cast<unsigned int>(SQ_1F));
constexpr BitBoard bb_2a = (BitBoard(1) << static_cast<unsigned int>(SQ_2A));
constexpr BitBoard bb_2b = (BitBoard(1) << static_cast<unsigned int>(SQ_2B));
constexpr BitBoard bb_2c = (BitBoard(1) << static_cast<unsigned int>(SQ_2C));
constexpr BitBoard bb_2d = (BitBoard(1) << static_cast<unsigned int>(SQ_2D));
constexpr BitBoard bb_2e = (BitBoard(1) << static_cast<unsigned int>(SQ_2E));
constexpr BitBoard bb_2f = (BitBoard(1) << static_cast<unsigned int>(SQ_2F));
constexpr BitBoard bb_3a = (BitBoard(1) << static_cast<unsigned int>(SQ_3A));
constexpr BitBoard bb_3b = (BitBoard(1) << static_cast<unsigned int>(SQ_3B));
constexpr BitBoard bb_3c = (BitBoard(1) << static_cast<unsigned int>(SQ_3C));
constexpr BitBoard bb_3d = (BitBoard(1) << static_cast<unsigned int>(SQ_3D));
constexpr BitBoard bb_3e = (BitBoard(1) << static_cast<unsigned int>(SQ_3E));
constexpr BitBoard bb_3f = (BitBoard(1) << static_cast<unsigned int>(SQ_3F));
constexpr BitBoard bb_4a = (BitBoard(1) << static_cast<unsigned int>(SQ_4A));
constexpr BitBoard bb_4b = (BitBoard(1) << static_cast<unsigned int>(SQ_4B));
constexpr BitBoard bb_4c = (BitBoard(1) << static_cast<unsigned int>(SQ_4C));
constexpr BitBoard bb_4d = (BitBoard(1) << static_cast<unsigned int>(SQ_4D));
constexpr BitBoard bb_4e = (BitBoard(1) << static_cast<unsigned int>(SQ_4E));
constexpr BitBoard bb_4f = (BitBoard(1) << static_cast<unsigned int>(SQ_4F));
constexpr BitBoard bb_5a = (BitBoard(1) << static_cast<unsigned int>(SQ_5A));
constexpr BitBoard bb_5b = (BitBoard(1) << static_cast<unsigned int>(SQ_5B));
constexpr BitBoard bb_5c = (BitBoard(1) << static_cast<unsigned int>(SQ_5C));
constexpr BitBoard bb_5d = (BitBoard(1) << static_cast<unsigned int>(SQ_5D));
constexpr BitBoard bb_5e = (BitBoard(1) << static_cast<unsigned int>(SQ_5E));
constexpr BitBoard bb_5f = (BitBoard(1) << static_cast<unsigned int>(SQ_5F));
constexpr BitBoard bb_6a = (BitBoard(1) << static_cast<unsigned int>(SQ_6A));
constexpr BitBoard bb_6b = (BitBoard(1) << static_cast<unsigned int>(SQ_6B));
constexpr BitBoard bb_6c = (BitBoard(1) << static_cast<unsigned int>(SQ_6C));
constexpr BitBoard bb_6d = (BitBoard(1) << static_cast<unsigned int>(SQ_6D));
constexpr BitBoard bb_6e = (BitBoard(1) << static_cast<unsigned int>(SQ_6E));
constexpr BitBoard bb_6f = (BitBoard(1) << static_cast<unsigned int>(SQ_6F));

constexpr BitBoard bb_file1 = bb_1a | bb_1b | bb_1c | bb_1d | bb_1e | bb_1f;
constexpr BitBoard bb_file2 = bb_2a | bb_2b | bb_2c | bb_2d | bb_2e | bb_2f;
constexpr BitBoard bb_file3 = bb_3a | bb_3b | bb_3c | bb_3d | bb_3e | bb_3f;
constexpr BitBoard bb_file4 = bb_4a | bb_4b | bb_4c | bb_4d | bb_4e | bb_4f;
constexpr BitBoard bb_file5 = bb_5a | bb_5b | bb_5c | bb_5d | bb_5e | bb_5f;
constexpr BitBoard bb_file6 = bb_6a | bb_6b | bb_6c | bb_6d | bb_6e | bb_6f;
constexpr BitBoard bb_ranka = bb_1a | bb_2a | bb_3a | bb_4a | bb_5a | bb_6a;
constexpr BitBoard bb_rankb = bb_1b | bb_2b | bb_3b | bb_4b | bb_5b | bb_6b;
constexpr BitBoard bb_rankc = bb_1c | bb_2c | bb_3c | bb_4c | bb_5c | bb_6c;
constexpr BitBoard bb_rankd = bb_1d | bb_2d | bb_3d | bb_4d | bb_5d | bb_6d;
constexpr BitBoard bb_ranke = bb_1e | bb_2e | bb_3e | bb_4e | bb_5e | bb_6e;
constexpr BitBoard bb_rankf = bb_1f | bb_2f | bb_3f | bb_4f | bb_5f | bb_6f;

} // namespace vshogi::judkins_shogi

namespace vshogi
{

template <>
inline const judkins_shogi::BitBoard judkins_shogi::BitBoard::
    square_to_bitboard_array[judkins_shogi::Squares::num_squares + 1]
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
        BitBoard(),
};

template <>
inline judkins_shogi::BitBoard
    judkins_shogi::BitBoard::attacks_table[judkins_shogi::num_attack_types]
                                          [judkins_shogi::Squares::num_squares]
    = {};

template <>
constexpr judkins_shogi::BitBoard
judkins_shogi::BitBoard::get_promotion_zone(const ColorEnum& c)
{
    using namespace vshogi::judkins_shogi;
    return (c == BLACK) ? (bb_ranka | bb_rankb) : (bb_ranke | bb_rankf);
}

template <>
template <DirectionEnum Dir>
constexpr judkins_shogi::BitBoard judkins_shogi::BitBoard::shift() const
{
    constexpr auto bb_f12345 = ~judkins_shogi::bb_file6;
    constexpr auto bb_f23456 = ~judkins_shogi::bb_file1;
    constexpr BitBoard filemask[] = {
        // clang-format off
        bb_f12345, ~BitBoard(0), bb_f23456,
        bb_f12345,               bb_f23456,
        bb_f12345, ~BitBoard(0), bb_f23456,
        bb_f12345,               bb_f23456,
        bb_f12345,               bb_f23456,
        // clang-format on
    };
    constexpr auto delta = judkins_shogi::Squares::direction_to_delta(Dir);
    if constexpr (delta > 0)
        return (*this & filemask[Dir]) << static_cast<unsigned int>(delta);
    else
        return (*this & filemask[Dir]) >> static_cast<unsigned int>(-delta);
}

template <>
inline judkins_shogi::BitBoard judkins_shogi::BitBoard::get_attacks_by(
    const vshogi::judkins_shogi::BoardPieceTypeEnum& p,
    const vshogi::judkins_shogi::SquareEnum& sq)
{
    using namespace vshogi::judkins_shogi;
    switch (p) {
    case B_FU:
        return attacks_table[0][sq];
    case B_KE:
        return attacks_table[1][sq];
    case B_GI:
        return attacks_table[2][sq];
    case B_KI:
    case B_TO:
    case B_NK:
    case B_NG:
        return attacks_table[3][sq];
    case W_FU:
        return attacks_table[4][sq];
    case W_KE:
        return attacks_table[5][sq];
    case W_GI:
        return attacks_table[6][sq];
    case W_KI:
    case W_TO:
    case W_NK:
    case W_NG:
        return attacks_table[7][sq];
    case B_KA:
    case W_KA:
        return attacks_table[8][sq];
    case B_HI:
    case W_HI:
        return attacks_table[9][sq];
    case B_UM:
    case W_UM:
        return attacks_table[10][sq];
    case B_RY:
    case W_RY:
        return attacks_table[11][sq];
    case B_OU:
    case W_OU:
        return attacks_table[12][sq];
    default:
        return BitBoard();
    }
}

template <>
inline judkins_shogi::BitBoard judkins_shogi::BitBoard::get_attacks_by(
    const vshogi::judkins_shogi::BoardPieceTypeEnum& p,
    const vshogi::judkins_shogi::SquareEnum& sq,
    const vshogi::judkins_shogi::BitBoard& occupied)
{
    using namespace vshogi::judkins_shogi;
    switch (p) {
    case B_FU:
        return attacks_table[0][sq];
    case B_KE:
        return attacks_table[1][sq];
    case B_GI:
        return attacks_table[2][sq];
    case B_KI:
    case B_TO:
    case B_NK:
    case B_NG:
        return attacks_table[3][sq];
    case W_FU:
        return attacks_table[4][sq];
    case W_KE:
        return attacks_table[5][sq];
    case W_GI:
        return attacks_table[6][sq];
    case W_KI:
    case W_TO:
    case W_NK:
    case W_NG:
        return attacks_table[7][sq];
    case B_KA:
    case W_KA:
        return BitBoard::ranging_attacks_to_diagonal(sq, occupied);
    case B_HI:
    case W_HI:
        return BitBoard::ranging_attacks_to_adjacent(sq, occupied);
    case B_UM:
    case W_UM:
        return BitBoard::ranging_attacks_to_diagonal(sq, occupied)
               | attacks_table[12][sq];
    case B_RY:
    case W_RY:
        return BitBoard::ranging_attacks_to_adjacent(sq, occupied)
               | attacks_table[12][sq];
    case B_OU:
    case W_OU:
        return attacks_table[12][sq];
    default:
        return BitBoard();
    }
}

template <>
inline void judkins_shogi::BitBoard::init_tables()
{
    for (auto&& sq : judkins_shogi::Squares::square_array) {
        const auto b = from_square(sq);
        // clang-format off
        attacks_table[0][sq] = b.shift<DIR_N>(); // B_FU
        attacks_table[1][sq] = b.shift<DIR_NNW>() | b.shift<DIR_NNE>(); // B_KE
        attacks_table[2][sq] = b.shift<DIR_NW>() | b.shift<DIR_N>() | b.shift<DIR_NE>() | b.shift<DIR_SW>() | b.shift<DIR_SE>(); // B_GI
        attacks_table[3][sq] = b.shift<DIR_NW>() | b.shift<DIR_N>() | b.shift<DIR_NE>() | b.shift<DIR_W>() | b.shift<DIR_E>() | b.shift<DIR_S>(); // B_KI
        attacks_table[4][sq] = b.shift<DIR_S>(); // W_FU
        attacks_table[5][sq] = b.shift<DIR_SSW>() | b.shift<DIR_SSE>(); // W_KE
        attacks_table[6][sq] = b.shift<DIR_NW>() | b.shift<DIR_NE>() | b.shift<DIR_SW>() | b.shift<DIR_S>() | b.shift<DIR_SE>(); // W_GI
        attacks_table[7][sq] = b.shift<DIR_N>() | b.shift<DIR_W>() | b.shift<DIR_E>() | b.shift<DIR_SW>() | b.shift<DIR_S>() | b.shift<DIR_SE>(); // W_KI
        attacks_table[8][sq] = BitBoard::ranging_attacks_to_diagonal(sq); // KA
        attacks_table[9][sq] = BitBoard::ranging_attacks_to_adjacent(sq); // HI
        attacks_table[10][sq] = attacks_table[8][sq] | attacks_table[3][sq]; // UM
        attacks_table[11][sq] = attacks_table[9][sq] | attacks_table[2][sq]; // RY
        attacks_table[12][sq] = attacks_table[2][sq] | attacks_table[3][sq]; // OU
        // clang-format on
    }
}

} // namespace vshogi

#endif // VSHOGI_JUDKINS_SHOGI_BITBOARD_HPP
