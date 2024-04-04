#ifndef VSHOGI_VARIANTS_SHOGI_HPP
#define VSHOGI_VARIANTS_SHOGI_HPP

#include <cstdint>

#include "vshogi/common/bitboard.hpp"
#include "vshogi/common/board.hpp"
#include "vshogi/common/color.hpp"
#include "vshogi/common/game.hpp"
#include "vshogi/common/move.hpp"
#include "vshogi/common/pieces.hpp"
#include "vshogi/common/squares.hpp"
#include "vshogi/common/stand.hpp"
#include "vshogi/common/state.hpp"

namespace vshogi::shogi
{

// clang-format off
constexpr uint num_piece_types = 14; // FU, KY, KE, GI, KI, KA, HI, OU, TO, NY, NK, NG, UM, RY
constexpr uint num_stand_piece_types = 7; // FU, KY, KE, GI, KI, KA, HI
constexpr uint promotion_bit = 3; // ____ _*___
constexpr uint num_files = 9; // 1, 2, 3, 4, 5, 6, 7, 8, 9
constexpr uint num_ranks = 9; // A, B, C, D, E, F, G, H, I
constexpr uint num_directions = 12; // NNW, NNE, NW, N, NE, W, E, SW, S, SE, SSW, SSE
constexpr uint num_directions_dlshogi = 10; // NW, N, NE, W, E, SW, S, SE, SSW, SSE
constexpr uint num_non_ranging_attacks = 9; // B_FU, W_FU, B_KE, W_KE, B_GI, W_GI, B_KI, W_KI, OU
constexpr uint num_attacks = 15; // B_FU, W_FU, B_KE, W_KE, B_GI, W_GI, B_KI, W_KI, OU, B_KY, W_KY, KA, HI, UM, RY
constexpr uint max_stand_piece_count = 18;
constexpr uint max_acceptable_repetition = 3;
// clang-format on

enum PieceTypeEnum : std::uint8_t
{
    FU = 0b0000, //!< Fu (Pawn)
    KY = 0b0001, //!< Kyo (Lance)
    KE = 0b0010, //!< Kei (Knight)
    GI = 0b0011, //!< Gin (Silver)
    KA = 0b0100, //!< Kaku (Bishop)
    HI = 0b0101, //!< Hisha (Rook)
    KI = 0b0110, //!< Kin (Gold)
    OU = 0b0111, //!< Ou, Gyoku (King)

    TO = 0b1000, //!< Tokin (Promoted Pawn)
    NY = 0b1001, //!< Nari-Kyo (Promoted Lance)
    NK = 0b1010, //!< Nari-Kei (Promoted Knight)
    NG = 0b1011, //!< Nari-Gin (Promoted Silver)
    UM = 0b1100, //!< Uma (Promoted Bishop)
    RY = 0b1101, //!< Ryu (Promoted Rook)

    NA = 0b1111, //!< NA
};

enum BoardPieceTypeEnum : std::uint8_t
{
    B_FU = 0b00000, //!< Black Fu (Pawn)
    B_KY = 0b00001, //!< Black Kyo (Lance)
    B_KE = 0b00010, //!< Black Kei (Knight)
    B_GI = 0b00011, //!< Black Gin (Silver)
    B_KA = 0b00100, //!< Black Kaku (Bishop)
    B_HI = 0b00101, //!< Black Hisha (Rook)
    B_KI = 0b00110, //!< Black Kin (Gold)
    B_OU = 0b00111, //!< Black Ou, Gyoku (King)
    B_TO = 0b01000, //!< Black Tokin (Promoted Pawn)
    B_NY = 0b01001, //!< Black Nari-Kyo (Promoted Lance)
    B_NK = 0b01010, //!< Black Nari-Kei (Promoted Knight)
    B_NG = 0b01011, //!< Black Nari-Gin (Promoted Silver)
    B_UM = 0b01100, //!< Black Uma (Promoted Bishop)
    B_RY = 0b01101, //!< Black Ryu (Promoted Rook)

    W_FU = 0b10000, //!< White Fu (Pawn)
    W_KY = 0b10001, //!< White Kyo (Lance)
    W_KE = 0b10010, //!< White Kei (Knight)
    W_GI = 0b10011, //!< White Gin (Silver)
    W_KA = 0b10100, //!< White Kaku (Bishop)
    W_HI = 0b10101, //!< White Hisha (Rook)
    W_KI = 0b10110, //!< White Kin (Gold)
    W_OU = 0b10111, //!< White Ou, Gyoku (King)
    W_TO = 0b11000, //!< White Tokin (Promoted Pawn)
    W_NY = 0b11001, //!< White Nari-Kyo (Promoted Lance)
    W_NK = 0b11010, //!< White Nari-Kei (Promoted Knight)
    W_NG = 0b11011, //!< White Nari-Gin (Promoted Silver)
    W_UM = 0b11100, //!< White Uma (Promoted Bishop)
    W_RY = 0b11101, //!< White Ryu (Promoted Rook)

    VOID = 0b11111, //!< Empty Square
};

/**
 * @brief Enumeration of board squares following SFEN.
 * @details
 *   9   8   7   6   5   4   3   2   1
 * +---+---+---+---+---+---+---+---+---+
 * | 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | a
 * +---+---+---+---+---+---+---+---+---+
 * | 9 | 10| 11| 12| 13| 14| 15| 16| 17| b
 * +---+---+---+---+---+---+---+---+---+
 * | 18| 19| 20| 21| 22| 23| 24| 25| 26| c
 * +---+---+---+---+---+---+---+---+---+
 * | 27| 28| 29| 30| 31| 32| 33| 34| 35| d
 * +---+---+---+---+---+---+---+---+---+
 * | 36| 37| 38| 39| 40| 41| 42| 43| 44| e
 * +---+---+---+---+---+---+---+---+---+
 * | 45| 46| 47| 48| 49| 50| 51| 52| 53| f
 * +---+---+---+---+---+---+---+---+---+
 * | 54| 55| 56| 57| 58| 59| 60| 61| 62| g
 * +---+---+---+---+---+---+---+---+---+
 * | 63| 64| 65| 66| 67| 68| 69| 70| 71| h
 * +---+---+---+---+---+---+---+---+---+
 * | 72| 73| 74| 75| 76| 77| 78| 79| 80| i
 * +---+---+---+---+---+---+---+---+---+
 */
enum SquareEnum : uint
{
    // clang-format off
    SQ_9A, SQ_8A, SQ_7A, SQ_6A, SQ_5A, SQ_4A, SQ_3A, SQ_2A, SQ_1A,
    SQ_9B, SQ_8B, SQ_7B, SQ_6B, SQ_5B, SQ_4B, SQ_3B, SQ_2B, SQ_1B,
    SQ_9C, SQ_8C, SQ_7C, SQ_6C, SQ_5C, SQ_4C, SQ_3C, SQ_2C, SQ_1C,
    SQ_9D, SQ_8D, SQ_7D, SQ_6D, SQ_5D, SQ_4D, SQ_3D, SQ_2D, SQ_1D,
    SQ_9E, SQ_8E, SQ_7E, SQ_6E, SQ_5E, SQ_4E, SQ_3E, SQ_2E, SQ_1E,
    SQ_9F, SQ_8F, SQ_7F, SQ_6F, SQ_5F, SQ_4F, SQ_3F, SQ_2F, SQ_1F,
    SQ_9G, SQ_8G, SQ_7G, SQ_6G, SQ_5G, SQ_4G, SQ_3G, SQ_2G, SQ_1G,
    SQ_9H, SQ_8H, SQ_7H, SQ_6H, SQ_5H, SQ_4H, SQ_3H, SQ_2H, SQ_1H,
    SQ_9I, SQ_8I, SQ_7I, SQ_6I, SQ_5I, SQ_4I, SQ_3I, SQ_2I, SQ_1I,
    // clang-format on
    SQ_NA,
    NUM_SQ = 81,

    // clang-format off
    A9 = SQ_9A, A8 = SQ_8A, A7 = SQ_7A, A6 = SQ_6A, A5 = SQ_5A, A4 = SQ_4A, A3 = SQ_3A, A2 = SQ_2A, A1 = SQ_1A,
    B9 = SQ_9B, B8 = SQ_8B, B7 = SQ_7B, B6 = SQ_6B, B5 = SQ_5B, B4 = SQ_4B, B3 = SQ_3B, B2 = SQ_2B, B1 = SQ_1B,
    C9 = SQ_9C, C8 = SQ_8C, C7 = SQ_7C, C6 = SQ_6C, C5 = SQ_5C, C4 = SQ_4C, C3 = SQ_3C, C2 = SQ_2C, C1 = SQ_1C,
    D9 = SQ_9D, D8 = SQ_8D, D7 = SQ_7D, D6 = SQ_6D, D5 = SQ_5D, D4 = SQ_4D, D3 = SQ_3D, D2 = SQ_2D, D1 = SQ_1D,
    E9 = SQ_9E, E8 = SQ_8E, E7 = SQ_7E, E6 = SQ_6E, E5 = SQ_5E, E4 = SQ_4E, E3 = SQ_3E, E2 = SQ_2E, E1 = SQ_1E,
    F9 = SQ_9F, F8 = SQ_8F, F7 = SQ_7F, F6 = SQ_6F, F5 = SQ_5F, F4 = SQ_4F, F3 = SQ_3F, F2 = SQ_2F, F1 = SQ_1F,
    G9 = SQ_9G, G8 = SQ_8G, G7 = SQ_7G, G6 = SQ_6G, G5 = SQ_5G, G4 = SQ_4G, G3 = SQ_3G, G2 = SQ_2G, G1 = SQ_1G,
    H9 = SQ_9H, H8 = SQ_8H, H7 = SQ_7H, H6 = SQ_6H, H5 = SQ_5H, H4 = SQ_4H, H3 = SQ_3H, H2 = SQ_2H, H1 = SQ_1H,
    I9 = SQ_9I, I8 = SQ_8I, I7 = SQ_7I, I6 = SQ_6I, I5 = SQ_5I, I4 = SQ_4I, I3 = SQ_3I, I2 = SQ_2I, I1 = SQ_1I,
    // clang-format on
};
enum RankEnum : uint
{
    RANK1,
    RANK2,
    RANK3,
    RANK4,
    RANK5,
    RANK6,
    RANK7,
    RANK8,
    RANK9,
};
enum FileEnum : uint
{
    FILE1,
    FILE2,
    FILE3,
    FILE4,
    FILE5,
    FILE6,
    FILE7,
    FILE8,
    FILE9,
};

using Pieces = vshogi::Pieces<
    PieceTypeEnum,
    BoardPieceTypeEnum,
    num_piece_types,
    num_stand_piece_types,
    promotion_bit>;
static_assert(FU == Pieces::FU);
static_assert(OU == Pieces::OU);
static_assert(NA == Pieces::NA);
static_assert(B_OU == Pieces::B_OU);
static_assert(W_OU == Pieces::W_OU);
static_assert(VOID == Pieces::VOID);

using Squares = vshogi::Squares<
    SquareEnum,
    FileEnum,
    RankEnum,
    Pieces,
    num_files,
    num_ranks,
    num_directions,
    num_directions_dlshogi>;

/**
 * @brief 32-bit integer representing pieces on a stand.
 * @details
 * ________ ________ ________ ___*****  FU (18 pieces)
 * ________ ________ _______* **______  KY (4 pieces)
 * ________ ________ ___***__ ________  KE (4 pieces)
 * ________ _______* **______ ________  GI (4 pieces)
 * ________ ____**__ ________ ________  KA (2 pieces)
 * ________ _**_____ ________ ________  HI (2 pieces)
 * _____*** ________ ________ ________  KI (4 pieces)
 */
using Stand = vshogi::Stand<std::uint32_t, Pieces>;

using BlackWhiteStands = vshogi::BlackWhiteStands<Stand>;

/**
 * @brief 16-bit integer representing a move in shogi game.
 * @details
 *       ________ _*******       Destination square (81 possibilities)
 *       ________ *_______       Promotion flag (2 possibilities)
 *       _******* ________       Source square or piece (88 possibilities = 81 + 7)
 * (MSB) xxxxxxxx xxxxxxxx (LSB)
 */
using Move = vshogi::Move<std::uint16_t, Squares, Pieces, 14, 7, 6>;

#ifdef __SIZEOF_INT128__
using BitBoard = vshogi::
    BitBoard<__uint128_t, Squares, BoardPieceTypeEnum, num_attacks, 3>;
#else
class UInt128
{
private:
    std::uint64_t m_value[2];

    constexpr UInt128(const std::uint64_t& higher, const std::uint64_t& lower)
        : m_value{higher, lower}
    {
    }

public:
    constexpr UInt128() : m_value{}
    {
    }
    constexpr UInt128(const std::uint64_t& a) : m_value{0UL, a}
    {
    }

    constexpr operator bool() const
    {
        return static_cast<bool>(m_value[1]) || static_cast<bool>(m_value[0]);
    }
    constexpr operator std::uint64_t() const
    {
        return m_value[0];
    }

    constexpr UInt128 operator~() const
    {
        return UInt128(~m_value[1], ~m_value[0]);
    }

    constexpr UInt128 operator|(const UInt128& other) const
    {
        return UInt128(
            m_value[1] | other.m_value[1], m_value[0] | other.m_value[0]);
    }

    constexpr UInt128 operator&(const UInt128& other) const
    {
        return UInt128(
            m_value[1] & other.m_value[1], m_value[0] & other.m_value[0]);
    }

    constexpr UInt128 operator^(const UInt128& other) const
    {
        return UInt128(
            m_value[1] ^ other.m_value[1], m_value[0] ^ other.m_value[0]);
    }

    constexpr UInt128 operator<<(const uint shift_width) const
    {
        UInt128(lshift_higher(shift_width), lshift_lower(shift_width));
    }

    constexpr UInt128 operator>>(const uint shift_width) const
    {
        UInt128(rshift_higher(shift_width), rshift_lower(shift_width));
    }

    UInt128& operator|=(const UInt128& other)
    {
        m_value[0] |= other.m_value[0];
        m_value[1] |= other.m_value[1];
        return *this;
    }
    UInt128& operator&=(const UInt128& other)
    {
        m_value[0] &= other.m_value[0];
        m_value[1] &= other.m_value[1];
        return *this;
    }
    UInt128& operator^=(const UInt128& other)
    {
        m_value[0] ^= other.m_value[0];
        m_value[1] ^= other.m_value[1];
        return *this;
    }

private:
    constexpr std::uint64_t lshift_lower(const uint shift_width) const
    {
        return (shift_width == 0)   ? m_value[0]
               : (shift_width > 63) ? 0
                                    : (m_value[0] << shift_width);
    }
    constexpr std::uint64_t lshift_carry(const uint shift_width) const
    {
        return static_cast<std::uint64_t>(
            (shift_width == 0)    ? 0
            : (shift_width < 64)  ? (m_value[0] >> (64 - shift_width))
            : (shift_width < 128) ? (m_value[0] << (shift_width - 64))
                                  : 0);
    }
    constexpr std::uint64_t lshift_higher(const uint shift_width) const
    {
        const auto c = lshift_carry(shift_width);
        const auto v = (shift_width < 64) ? m_value[1] << shift_width : 0;
        return v | c;
    }
    constexpr std::uint64_t rshift_lower(const uint shift_width) const
    {
        const auto v = (shift_width == 0)   ? m_value[0]
                       : (shift_width > 63) ? 0
                                            : (m_value[0] >> shift_width);
        const auto c = rshift_carry(shift_width);
        return v | c;
    }
    constexpr std::uint64_t rshift_carry(const uint shift_width) const
    {
        const auto v = static_cast<std::uint64_t>(m_value[1]);
        return (shift_width == 0)     ? 0
               : (shift_width <= 64)  ? (v << (64 - shift_width))
               : (shift_width <= 128) ? (v >> (shift_width - 64))
                                      : 0;
    }
    constexpr std::uint64_t rshift_higher(const uint shift_width) const
    {
        return (shift_width <= 64) ? ((m_value[1]) >> shift_width) : 0;
    }
};

using BitBoard
    = vshogi::BitBoard<UInt128, Squares, BoardPieceTypeEnum, num_attacks, 3>;
#endif

using Board = vshogi::Board<Squares, Pieces, BitBoard>;

using State
    = vshogi::State<Board, BlackWhiteStands, Move, max_stand_piece_count>;

using Game = vshogi::Game<State, max_acceptable_repetition>;

// clang-format off
constexpr BitBoard bb_1a = (BitBoard(1) << static_cast<uint>(SQ_1A));
constexpr BitBoard bb_1b = (BitBoard(1) << static_cast<uint>(SQ_1B));
constexpr BitBoard bb_1c = (BitBoard(1) << static_cast<uint>(SQ_1C));
constexpr BitBoard bb_1d = (BitBoard(1) << static_cast<uint>(SQ_1D));
constexpr BitBoard bb_1e = (BitBoard(1) << static_cast<uint>(SQ_1E));
constexpr BitBoard bb_1f = (BitBoard(1) << static_cast<uint>(SQ_1F));
constexpr BitBoard bb_1g = (BitBoard(1) << static_cast<uint>(SQ_1G));
constexpr BitBoard bb_1h = (BitBoard(1) << static_cast<uint>(SQ_1H));
constexpr BitBoard bb_1i = (BitBoard(1) << static_cast<uint>(SQ_1I));
constexpr BitBoard bb_2a = (BitBoard(1) << static_cast<uint>(SQ_2A));
constexpr BitBoard bb_2b = (BitBoard(1) << static_cast<uint>(SQ_2B));
constexpr BitBoard bb_2c = (BitBoard(1) << static_cast<uint>(SQ_2C));
constexpr BitBoard bb_2d = (BitBoard(1) << static_cast<uint>(SQ_2D));
constexpr BitBoard bb_2e = (BitBoard(1) << static_cast<uint>(SQ_2E));
constexpr BitBoard bb_2f = (BitBoard(1) << static_cast<uint>(SQ_2F));
constexpr BitBoard bb_2g = (BitBoard(1) << static_cast<uint>(SQ_2G));
constexpr BitBoard bb_2h = (BitBoard(1) << static_cast<uint>(SQ_2H));
constexpr BitBoard bb_2i = (BitBoard(1) << static_cast<uint>(SQ_2I));
constexpr BitBoard bb_3a = (BitBoard(1) << static_cast<uint>(SQ_3A));
constexpr BitBoard bb_3b = (BitBoard(1) << static_cast<uint>(SQ_3B));
constexpr BitBoard bb_3c = (BitBoard(1) << static_cast<uint>(SQ_3C));
constexpr BitBoard bb_3d = (BitBoard(1) << static_cast<uint>(SQ_3D));
constexpr BitBoard bb_3e = (BitBoard(1) << static_cast<uint>(SQ_3E));
constexpr BitBoard bb_3f = (BitBoard(1) << static_cast<uint>(SQ_3F));
constexpr BitBoard bb_3g = (BitBoard(1) << static_cast<uint>(SQ_3G));
constexpr BitBoard bb_3h = (BitBoard(1) << static_cast<uint>(SQ_3H));
constexpr BitBoard bb_3i = (BitBoard(1) << static_cast<uint>(SQ_3I));
constexpr BitBoard bb_4a = (BitBoard(1) << static_cast<uint>(SQ_4A));
constexpr BitBoard bb_4b = (BitBoard(1) << static_cast<uint>(SQ_4B));
constexpr BitBoard bb_4c = (BitBoard(1) << static_cast<uint>(SQ_4C));
constexpr BitBoard bb_4d = (BitBoard(1) << static_cast<uint>(SQ_4D));
constexpr BitBoard bb_4e = (BitBoard(1) << static_cast<uint>(SQ_4E));
constexpr BitBoard bb_4f = (BitBoard(1) << static_cast<uint>(SQ_4F));
constexpr BitBoard bb_4g = (BitBoard(1) << static_cast<uint>(SQ_4G));
constexpr BitBoard bb_4h = (BitBoard(1) << static_cast<uint>(SQ_4H));
constexpr BitBoard bb_4i = (BitBoard(1) << static_cast<uint>(SQ_4I));
constexpr BitBoard bb_5a = (BitBoard(1) << static_cast<uint>(SQ_5A));
constexpr BitBoard bb_5b = (BitBoard(1) << static_cast<uint>(SQ_5B));
constexpr BitBoard bb_5c = (BitBoard(1) << static_cast<uint>(SQ_5C));
constexpr BitBoard bb_5d = (BitBoard(1) << static_cast<uint>(SQ_5D));
constexpr BitBoard bb_5e = (BitBoard(1) << static_cast<uint>(SQ_5E));
constexpr BitBoard bb_5f = (BitBoard(1) << static_cast<uint>(SQ_5F));
constexpr BitBoard bb_5g = (BitBoard(1) << static_cast<uint>(SQ_5G));
constexpr BitBoard bb_5h = (BitBoard(1) << static_cast<uint>(SQ_5H));
constexpr BitBoard bb_5i = (BitBoard(1) << static_cast<uint>(SQ_5I));
constexpr BitBoard bb_6a = (BitBoard(1) << static_cast<uint>(SQ_6A));
constexpr BitBoard bb_6b = (BitBoard(1) << static_cast<uint>(SQ_6B));
constexpr BitBoard bb_6c = (BitBoard(1) << static_cast<uint>(SQ_6C));
constexpr BitBoard bb_6d = (BitBoard(1) << static_cast<uint>(SQ_6D));
constexpr BitBoard bb_6e = (BitBoard(1) << static_cast<uint>(SQ_6E));
constexpr BitBoard bb_6f = (BitBoard(1) << static_cast<uint>(SQ_6F));
constexpr BitBoard bb_6g = (BitBoard(1) << static_cast<uint>(SQ_6G));
constexpr BitBoard bb_6h = (BitBoard(1) << static_cast<uint>(SQ_6H));
constexpr BitBoard bb_6i = (BitBoard(1) << static_cast<uint>(SQ_6I));
constexpr BitBoard bb_7a = (BitBoard(1) << static_cast<uint>(SQ_7A));
constexpr BitBoard bb_7b = (BitBoard(1) << static_cast<uint>(SQ_7B));
constexpr BitBoard bb_7c = (BitBoard(1) << static_cast<uint>(SQ_7C));
constexpr BitBoard bb_7d = (BitBoard(1) << static_cast<uint>(SQ_7D));
constexpr BitBoard bb_7e = (BitBoard(1) << static_cast<uint>(SQ_7E));
constexpr BitBoard bb_7f = (BitBoard(1) << static_cast<uint>(SQ_7F));
constexpr BitBoard bb_7g = (BitBoard(1) << static_cast<uint>(SQ_7G));
constexpr BitBoard bb_7h = (BitBoard(1) << static_cast<uint>(SQ_7H));
constexpr BitBoard bb_7i = (BitBoard(1) << static_cast<uint>(SQ_7I));
constexpr BitBoard bb_8a = (BitBoard(1) << static_cast<uint>(SQ_8A));
constexpr BitBoard bb_8b = (BitBoard(1) << static_cast<uint>(SQ_8B));
constexpr BitBoard bb_8c = (BitBoard(1) << static_cast<uint>(SQ_8C));
constexpr BitBoard bb_8d = (BitBoard(1) << static_cast<uint>(SQ_8D));
constexpr BitBoard bb_8e = (BitBoard(1) << static_cast<uint>(SQ_8E));
constexpr BitBoard bb_8f = (BitBoard(1) << static_cast<uint>(SQ_8F));
constexpr BitBoard bb_8g = (BitBoard(1) << static_cast<uint>(SQ_8G));
constexpr BitBoard bb_8h = (BitBoard(1) << static_cast<uint>(SQ_8H));
constexpr BitBoard bb_8i = (BitBoard(1) << static_cast<uint>(SQ_8I));
constexpr BitBoard bb_9a = (BitBoard(1) << static_cast<uint>(SQ_9A));
constexpr BitBoard bb_9b = (BitBoard(1) << static_cast<uint>(SQ_9B));
constexpr BitBoard bb_9c = (BitBoard(1) << static_cast<uint>(SQ_9C));
constexpr BitBoard bb_9d = (BitBoard(1) << static_cast<uint>(SQ_9D));
constexpr BitBoard bb_9e = (BitBoard(1) << static_cast<uint>(SQ_9E));
constexpr BitBoard bb_9f = (BitBoard(1) << static_cast<uint>(SQ_9F));
constexpr BitBoard bb_9g = (BitBoard(1) << static_cast<uint>(SQ_9G));
constexpr BitBoard bb_9h = (BitBoard(1) << static_cast<uint>(SQ_9H));
constexpr BitBoard bb_9i = (BitBoard(1) << static_cast<uint>(SQ_9I));
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
inline const DirectionEnum
    shogi::Pieces::attack_directions_table[2 * num_piece_types + 1][9]
    = {
        // clang-format off
{DIR_N,                                                      DIR_NA}, // B_FU
{DIR_N,                                                      DIR_NA}, // B_KY
{DIR_NNW, DIR_NNE,                                           DIR_NA}, // B_KE
{DIR_NW, DIR_N, DIR_NE, DIR_SW, DIR_SE,                      DIR_NA}, // B_GI
{DIR_NW, DIR_NE, DIR_SW, DIR_SE,                             DIR_NA}, // B_KA
{DIR_N, DIR_W, DIR_E, DIR_S,                                 DIR_NA}, // B_HI
{DIR_NW, DIR_N, DIR_NE, DIR_W, DIR_E, DIR_S,                 DIR_NA}, // B_KI
{DIR_NW, DIR_N, DIR_NE, DIR_W, DIR_E, DIR_SW, DIR_S, DIR_SE, DIR_NA}, // B_OU
{DIR_NW, DIR_N, DIR_NE, DIR_W, DIR_E, DIR_S,                 DIR_NA}, // B_TO
{DIR_NW, DIR_N, DIR_NE, DIR_W, DIR_E, DIR_S,                 DIR_NA}, // B_NY
{DIR_NW, DIR_N, DIR_NE, DIR_W, DIR_E, DIR_S,                 DIR_NA}, // B_NK
{DIR_NW, DIR_N, DIR_NE, DIR_W, DIR_E, DIR_S,                 DIR_NA}, // B_NG
{DIR_NW, DIR_N, DIR_NE, DIR_W, DIR_E, DIR_SW, DIR_S, DIR_SE, DIR_NA}, // B_UM
{DIR_NW, DIR_N, DIR_NE, DIR_W, DIR_E, DIR_SW, DIR_S, DIR_SE, DIR_NA}, // B_RY
{DIR_S,                                                      DIR_NA}, // W_FU
{DIR_S,                                                      DIR_NA}, // W_KY
{DIR_SSE, DIR_SSW,                                           DIR_NA}, // W_KE
{DIR_SE, DIR_S, DIR_SW, DIR_NE, DIR_NW,                      DIR_NA}, // W_GI
{DIR_SE, DIR_SW, DIR_NE, DIR_NW,                             DIR_NA}, // W_KA
{DIR_S, DIR_E, DIR_W, DIR_N,                                 DIR_NA}, // W_HI
{DIR_SE, DIR_S, DIR_SW, DIR_E, DIR_W, DIR_N,                 DIR_NA}, // W_KI
{DIR_SE, DIR_S, DIR_SW, DIR_E, DIR_W, DIR_NE, DIR_N, DIR_NW, DIR_NA}, // W_OU
{DIR_SE, DIR_S, DIR_SW, DIR_E, DIR_W, DIR_N,                 DIR_NA}, // W_TO
{DIR_SE, DIR_S, DIR_SW, DIR_E, DIR_W, DIR_N,                 DIR_NA}, // W_NY
{DIR_SE, DIR_S, DIR_SW, DIR_E, DIR_W, DIR_N,                 DIR_NA}, // W_NK
{DIR_SE, DIR_S, DIR_SW, DIR_E, DIR_W, DIR_N,                 DIR_NA}, // W_NG
{DIR_SE, DIR_S, DIR_SW, DIR_E, DIR_W, DIR_NE, DIR_N, DIR_NW, DIR_NA}, // W_UM
{DIR_SE, DIR_S, DIR_SW, DIR_E, DIR_W, DIR_NE, DIR_N, DIR_NW, DIR_NA}, // W_RY
        // clang-format on
};

template <>
inline const vshogi::shogi::PieceTypeEnum vshogi::shogi::Pieces::piece_array[]
    = {vshogi::shogi::FU,
       vshogi::shogi::KY,
       vshogi::shogi::KE,
       vshogi::shogi::GI,
       vshogi::shogi::KA,
       vshogi::shogi::HI,
       vshogi::shogi::KI,
       vshogi::shogi::OU,
       vshogi::shogi::TO,
       vshogi::shogi::NY,
       vshogi::shogi::NK,
       vshogi::shogi::NG,
       vshogi::shogi::UM,
       vshogi::shogi::RY};

template <>
inline const vshogi::shogi::PieceTypeEnum
    vshogi::shogi::Pieces::stand_piece_array[]
    = {vshogi::shogi::FU,
       vshogi::shogi::KY,
       vshogi::shogi::KE,
       vshogi::shogi::GI,
       vshogi::shogi::KA,
       vshogi::shogi::HI,
       vshogi::shogi::KI};

template <>
inline vshogi::shogi::PieceTypeEnum
vshogi::shogi::Pieces::to_piece_type(const char c)
{
    switch (std::tolower(c)) {
    case 'p':
        return vshogi::shogi::FU;
    case 'l':
        return vshogi::shogi::KY;
    case 'n':
        return vshogi::shogi::KE;
    case 's':
        return vshogi::shogi::GI;
    case 'b':
        return vshogi::shogi::KA;
    case 'r':
        return vshogi::shogi::HI;
    case 'g':
        return vshogi::shogi::KI;
    case 'k':
        return vshogi::shogi::OU;
    default:
        return vshogi::shogi::NA;
    }
}

template <>
inline char shogi::Pieces::to_char(const shogi::PieceTypeEnum& p)
{
    constexpr char table[] = {'p', 'l', 'n', 's', 'b', 'r', 'g', 'k'};
    static_assert(0 == shogi::FU);
    static_assert(1 == shogi::KY);
    static_assert(2 == shogi::KE);
    static_assert(3 == shogi::GI);
    static_assert(4 == shogi::KA);
    static_assert(5 == shogi::HI);
    static_assert(6 == shogi::KI);
    static_assert(7 == shogi::OU);
    return table[p];
}

template <>
inline constexpr bool
vshogi::shogi::Pieces::is_promotable(const vshogi::shogi::PieceTypeEnum& p)
{
    return (p < vshogi::shogi::KI);
}

template <>
inline bool vshogi::shogi::Pieces::is_ranging_to(
    const vshogi::shogi::BoardPieceTypeEnum& p, const DirectionEnum& d)
{
    using namespace vshogi::shogi;
    if (p == B_KY)
        return (d == DIR_N);
    if (p == W_KY)
        return (d == DIR_S);
    const auto base = demote(to_piece_type(p));
    if (base == HI)
        return (d == DIR_N) || (d == DIR_W) || (d == DIR_E) || (d == DIR_S);
    if (base == KA)
        return (d == DIR_NW) || (d == DIR_NE) || (d == DIR_SW) || (d == DIR_SE);
    return false;
}

template <>
inline bool shogi::Pieces::is_ranging_piece(const shogi::PieceTypeEnum& pt)
{
    using namespace shogi;
    return ((pt == KY) || (pt == KA) || (pt == HI) || (pt == UM) || (pt == RY));
}

template <>
inline uint
vshogi::shogi::Pieces::get_point(const vshogi::shogi::PieceTypeEnum& p)
{
    using namespace vshogi::shogi;
    switch (demote(p)) {
    case OU:
        return 0;
    case KA:
    case HI:
        return 5;
    default:
        return 1;
    }
}

template <>
inline bool vshogi::shogi::Squares::in_promotion_zone(
    const vshogi::shogi::RankEnum& r, const ColorEnum& c)
{
    return (c == BLACK) ? (r <= vshogi::shogi::RANK3)
                        : (r >= vshogi::shogi::RANK7);
}

template <>
inline DirectionEnum
vshogi::shogi::Squares::get_direction_for_diagonal_or_knight(
    const vshogi::shogi::SquareEnum& dst, const vshogi::shogi::SquareEnum& src)
{
    switch (static_cast<int>(src - dst)) {
    case 10:
    case 20:
    case 30:
    // case 40:
    case 50:
    case 60:
    case 70:
    case 80:
        return DIR_NW;
    case 8:
    case 16:
    case 24:
    case 32:
    // case 40:
    case 48:
    case 56:
    case 64:
        return DIR_NE;
    case 40:
        return (to_file(src) < vshogi::shogi::FILE6) ? DIR_NW : DIR_NE;
    case 17:
        return DIR_NNE;
    case 19:
        return DIR_NNW;
    case -8:
    case -16:
    case -24:
    case -32:
    // case -40:
    case -48:
    case -56:
    case -64:
        return DIR_SW;
    case -10:
    case -20:
    case -30:
    // case -40:
    case -50:
    case -60:
    case -70:
    case -80:
        return DIR_SE;
    case -40:
        return (to_file(src) < vshogi::shogi::FILE5) ? DIR_SW : DIR_SE;
    case -17:
        return DIR_SSW;
    case -19:
        return DIR_SSE;
    default:
        return DIR_NA;
    }
}

template <>
inline const uint shogi::Stand::shift_bits[] = {0, 6, 10, 14, 18, 21, 24};

template <>
inline const std::uint32_t shogi::Stand::masks[] = {
    // clang-format off
    0x0000001f, // FU
    0x000001c0, // KY
    0x00001c00, // KE
    0x0001c000, // GI
    0x000c0000, // KA
    0x00600000, // HI
    0x07000000, // KI
    // clang-format on
};

template <>
inline const std::uint32_t shogi::Stand::deltas[] = {
    // clang-format off
    0x00000001, // FU
    0x00000040, // KY
    0x00000400, // KE
    0x00004000, // GI
    0x00040000, // KA
    0x00200000, // HI
    0x01000000, // KI
    // clang-format on
};

template <>
inline const std::uint32_t shogi::Stand::mask = 0x076ddddf;

template <>
template <>
inline shogi::Stand::Stand(
    const int num_fu,
    const int num_ky,
    const int num_ke,
    const int num_gi,
    const int num_ka,
    const int num_hi,
    const int num_ki)
    : Stand(static_cast<std::uint32_t>(
        (num_fu << shift_bits[shogi::FU]) + (num_ky << shift_bits[shogi::KY])
        + (num_ke << shift_bits[shogi::KE]) + (num_gi << shift_bits[shogi::GI])
        + (num_ka << shift_bits[shogi::KA]) + (num_hi << shift_bits[shogi::HI])
        + (num_ki << shift_bits[shogi::KI])))
{
}

template <>
inline const shogi::Pieces::PieceTypeEnum
    shogi::BlackWhiteStands::stand_pieces_in_sfen_order[]
    = {shogi::HI,
       shogi::KA,
       shogi::KI,
       shogi::GI,
       shogi::KE,
       shogi::KY,
       shogi::FU};
template <>
inline const int shogi::BlackWhiteStands::max_sfen_length
    = 26; // "10p2l2n2sbr2g2P2L2N2SBR2G "

template <>
inline const shogi::BitBoard
    shogi::BitBoard::square_to_bitboard_array[shogi::Squares::num_squares + 1]
    = {
        BitBoard(1) << static_cast<uint>(0),
        BitBoard(1) << static_cast<uint>(1),
        BitBoard(1) << static_cast<uint>(2),
        BitBoard(1) << static_cast<uint>(3),
        BitBoard(1) << static_cast<uint>(4),
        BitBoard(1) << static_cast<uint>(5),
        BitBoard(1) << static_cast<uint>(6),
        BitBoard(1) << static_cast<uint>(7),
        BitBoard(1) << static_cast<uint>(8),
        BitBoard(1) << static_cast<uint>(9),
        BitBoard(1) << static_cast<uint>(10),
        BitBoard(1) << static_cast<uint>(11),
        BitBoard(1) << static_cast<uint>(12),
        BitBoard(1) << static_cast<uint>(13),
        BitBoard(1) << static_cast<uint>(14),
        BitBoard(1) << static_cast<uint>(15),
        BitBoard(1) << static_cast<uint>(16),
        BitBoard(1) << static_cast<uint>(17),
        BitBoard(1) << static_cast<uint>(18),
        BitBoard(1) << static_cast<uint>(19),
        BitBoard(1) << static_cast<uint>(20),
        BitBoard(1) << static_cast<uint>(21),
        BitBoard(1) << static_cast<uint>(22),
        BitBoard(1) << static_cast<uint>(23),
        BitBoard(1) << static_cast<uint>(24),
        BitBoard(1) << static_cast<uint>(25),
        BitBoard(1) << static_cast<uint>(26),
        BitBoard(1) << static_cast<uint>(27),
        BitBoard(1) << static_cast<uint>(28),
        BitBoard(1) << static_cast<uint>(29),
        BitBoard(1) << static_cast<uint>(30),
        BitBoard(1) << static_cast<uint>(31),
        BitBoard(1) << static_cast<uint>(32),
        BitBoard(1) << static_cast<uint>(33),
        BitBoard(1) << static_cast<uint>(34),
        BitBoard(1) << static_cast<uint>(35),
        BitBoard(1) << static_cast<uint>(36),
        BitBoard(1) << static_cast<uint>(37),
        BitBoard(1) << static_cast<uint>(38),
        BitBoard(1) << static_cast<uint>(39),
        BitBoard(1) << static_cast<uint>(40),
        BitBoard(1) << static_cast<uint>(41),
        BitBoard(1) << static_cast<uint>(42),
        BitBoard(1) << static_cast<uint>(43),
        BitBoard(1) << static_cast<uint>(44),
        BitBoard(1) << static_cast<uint>(45),
        BitBoard(1) << static_cast<uint>(46),
        BitBoard(1) << static_cast<uint>(47),
        BitBoard(1) << static_cast<uint>(48),
        BitBoard(1) << static_cast<uint>(49),
        BitBoard(1) << static_cast<uint>(50),
        BitBoard(1) << static_cast<uint>(51),
        BitBoard(1) << static_cast<uint>(52),
        BitBoard(1) << static_cast<uint>(53),
        BitBoard(1) << static_cast<uint>(54),
        BitBoard(1) << static_cast<uint>(55),
        BitBoard(1) << static_cast<uint>(56),
        BitBoard(1) << static_cast<uint>(57),
        BitBoard(1) << static_cast<uint>(58),
        BitBoard(1) << static_cast<uint>(59),
        BitBoard(1) << static_cast<uint>(60),
        BitBoard(1) << static_cast<uint>(61),
        BitBoard(1) << static_cast<uint>(62),
        BitBoard(1) << static_cast<uint>(63),
        BitBoard(1) << static_cast<uint>(64),
        BitBoard(1) << static_cast<uint>(65),
        BitBoard(1) << static_cast<uint>(66),
        BitBoard(1) << static_cast<uint>(67),
        BitBoard(1) << static_cast<uint>(68),
        BitBoard(1) << static_cast<uint>(69),
        BitBoard(1) << static_cast<uint>(70),
        BitBoard(1) << static_cast<uint>(71),
        BitBoard(1) << static_cast<uint>(72),
        BitBoard(1) << static_cast<uint>(73),
        BitBoard(1) << static_cast<uint>(74),
        BitBoard(1) << static_cast<uint>(75),
        BitBoard(1) << static_cast<uint>(76),
        BitBoard(1) << static_cast<uint>(77),
        BitBoard(1) << static_cast<uint>(78),
        BitBoard(1) << static_cast<uint>(79),
        BitBoard(1) << static_cast<uint>(80),
        BitBoard(), // SQ_NA
};

template <>
inline shogi::BitBoard
    shogi::BitBoard::attacks_table[shogi::num_attacks]
                                  [shogi::Squares::num_squares]
    = {};

template <>
inline uint shogi::BitBoard::hamming_weight() const
{
    return hamming_weight_64bit(static_cast<std::uint64_t>(m_value))
           + hamming_weight_64bit(static_cast<std::uint64_t>(m_value >> 64));
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
    case B_KE:
        return attacks_table[1][sq];
    case B_GI:
        return attacks_table[2][sq];
    case B_KI:
    case B_TO:
    case B_NY:
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
    case W_NY:
    case W_NK:
    case W_NG:
        return attacks_table[7][sq];
    case B_OU:
    case W_OU:
        return attacks_table[8][sq];
    case B_KY:
        return attacks_table[9][sq];
    case W_KY:
        return attacks_table[10][sq];
    case B_KA:
    case W_KA:
        return attacks_table[11][sq];
    case B_HI:
    case W_HI:
        return attacks_table[12][sq];
    case B_UM:
    case W_UM:
        return attacks_table[13][sq];
    case B_RY:
    case W_RY:
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
    case B_KE:
        return attacks_table[1][sq];
    case B_GI:
        return attacks_table[2][sq];
    case B_KI:
    case B_TO:
    case B_NY:
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
    case W_NY:
    case W_NK:
    case W_NG:
        return attacks_table[7][sq];
    case B_OU:
    case W_OU:
        return attacks_table[8][sq];
    case B_KY:
        return BitBoard::ranging_attacks_to<DIR_N>(sq, occupied);
    case W_KY:
        return BitBoard::ranging_attacks_to<DIR_S>(sq, occupied);
    case B_KA:
    case W_KA:
        return BitBoard::ranging_attacks_to_diagonal(sq, occupied);
    case B_HI:
    case W_HI:
        return BitBoard::ranging_attacks_to_adjacent(sq, occupied);
    case B_UM:
    case W_UM:
        return BitBoard::ranging_attacks_to_diagonal(sq, occupied)
               | attacks_table[8][sq];
    case B_RY:
    case W_RY:
        return BitBoard::ranging_attacks_to_adjacent(sq, occupied)
               | attacks_table[8][sq];
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
        attacks_table[1][sq] = b.shift<DIR_NNW>() | b.shift<DIR_NNE>(); // B_KE
        attacks_table[2][sq] = b.shift<DIR_NW>() | b.shift<DIR_N>() | b.shift<DIR_NE>() | b.shift<DIR_SW>() | b.shift<DIR_SE>(); // B_GI
        attacks_table[3][sq] = b.shift<DIR_NW>() | b.shift<DIR_N>() | b.shift<DIR_NE>() | b.shift<DIR_W>() | b.shift<DIR_E>() | b.shift<DIR_S>(); // B_KI
        attacks_table[4][sq] = b.shift<DIR_S>(); // W_FU
        attacks_table[5][sq] = b.shift<DIR_SSW>() | b.shift<DIR_SSE>(); // W_KE
        attacks_table[6][sq] = b.shift<DIR_NW>() | b.shift<DIR_NE>() | b.shift<DIR_SW>() | b.shift<DIR_S>() | b.shift<DIR_SE>(); // W_GI
        attacks_table[7][sq] = b.shift<DIR_N>() | b.shift<DIR_W>() | b.shift<DIR_E>() | b.shift<DIR_SW>() | b.shift<DIR_S>() | b.shift<DIR_SE>(); // W_KI
        attacks_table[8][sq] = attacks_table[2][sq] | attacks_table[3][sq]; // OU
        attacks_table[9][sq] = BitBoard::ranging_attacks_to<DIR_N>(sq); // B_KY
        attacks_table[10][sq] = BitBoard::ranging_attacks_to<DIR_S>(sq); // W_KY
        attacks_table[11][sq] = BitBoard::ranging_attacks_to_diagonal(sq); // KA
        attacks_table[12][sq] = BitBoard::ranging_attacks_to_adjacent(sq); // HI
        attacks_table[13][sq] = attacks_table[11][sq] | attacks_table[8][sq]; // UM
        attacks_table[14][sq] = attacks_table[12][sq] | attacks_table[8][sq]; // RY
        // clang-format on
    }
}

template <>
inline std::uint64_t vshogi::shogi::State::zobrist_board
    [shogi::Squares::num_squares]
    [num_colors * shogi::Pieces::num_piece_types + 1]
    = {};

template <>
inline std::uint64_t vshogi::shogi::State::zobrist_stand
    [num_colors][shogi::Pieces::num_stand_piece_types][19]
    = {};

} // namespace vshogi

#endif // VSHOGI_VARIANTS_SHOGI_HPP
