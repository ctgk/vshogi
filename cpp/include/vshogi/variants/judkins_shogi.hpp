#ifndef VSHOGI_VARIANTS_JUDKINS_SHOGI_HPP
#define VSHOGI_VARIANTS_JUDKINS_SHOGI_HPP

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

namespace vshogi::judkins_shogi
{

// clang-format off
constexpr uint num_piece_types = 12; // FU, KE, GI, KI, KA, HI, OU, TO, NK, NG, UM, RY
constexpr uint num_stand_piece_types = 6; // FU, KE, GI, KI, KA, HI
constexpr uint promotion_bit = 3; // ____ _*___
constexpr uint num_files = 6; // 1, 2, 3, 4, 5, 6
constexpr uint num_ranks = 6; // A, B, C, D, E, F
constexpr uint num_directions = 12; // NNW, NNE, NW, N, NE, W, E, SW, S, SE, SSW, SSE
constexpr uint num_directions_dlshogi = 10; // NW, N, NE, W, E, SW, S, SE, SSW, SSE
constexpr uint num_non_ranging_attacks = 9; // B_FU, W_FU, B_KE, W_KE, B_GI, W_GI, B_KI, W_KI, OU
constexpr uint num_attacks = 13; // B_FU, W_FU, B_KE, W_KE, B_GI, W_GI, B_KI, W_KI, OU, KA, HI, UM, RY
constexpr uint max_stand_piece_count = 2;
constexpr uint max_acceptable_repetition = 3;
// clang-format on

enum PieceTypeEnum : std::uint8_t
{
    FU = 0b0000, //!< Fu (Pawn)
    KE = 0b0001, //!< Kei (Knight)
    GI = 0b0010, //!< Gin (Silver)
    KA = 0b0011, //!< Kaku (Bishop)
    HI = 0b0100, //!< Hisha (Rook)
    KI = 0b0101, //!< Kin (Gold)
    OU = 0b0111, //!< Ou, Gyoku (King)

    TO = 0b1000 + FU, //!< Tokin (Promoted Pawn)
    NK = 0b1000 + KE, //!< Nari-Kei (Promoted Knight)
    NG = 0b1000 + GI, //!< Nari-Gin (Promoted Silver)
    UM = 0b1000 + KA, //!< Uma (Promoted Bishop)
    RY = 0b1000 + HI, //!< Ryu (Promoted Rook)

    NA = 0b1111, //!< NA
};

enum BoardPieceTypeEnum : std::uint8_t
{
    B_FU = FU, //!< Black Fu (Pawn)
    B_KE = KE, //!< Black Kei (Knight)
    B_GI = GI, //!< Black Gin (Silver)
    B_KA = KA, //!< Black Kaku (Bishop)
    B_HI = HI, //!< Black Hisha (Rook)
    B_KI = KI, //!< Black Kin (Gold)
    B_OU = OU, //!< Black Ou, Gyoku (King)
    B_TO = TO, //!< Black Tokin (Promoted Pawn)
    B_NK = NK, //!< Black Nari-Kei (Promoted Knight)
    B_NG = NG, //!< Black Nari-Gin (Promoted Silver)
    B_UM = UM, //!< Black Uma (Promoted Bishop)
    B_RY = RY, //!< Black Ryu (Promoted Rook)

    W_FU = 0b10000 + B_FU, //!< White Fu (Pawn)
    W_KE = 0b10000 + B_KE, //!< White Kei (Knight)
    W_GI = 0b10000 + B_GI, //!< White Gin (Silver)
    W_KA = 0b10000 + B_KA, //!< White Kaku (Bishop)
    W_HI = 0b10000 + B_HI, //!< White Hisha (Rook)
    W_KI = 0b10000 + B_KI, //!< White Kin (Gold)
    W_OU = 0b10000 + B_OU, //!< White Ou, Gyoku (King)
    W_TO = 0b10000 + B_TO, //!< White Tokin (Promoted Pawn)
    W_NK = 0b10000 + B_NK, //!< White Nari-Kei (Promoted Knight)
    W_NG = 0b10000 + B_NG, //!< White Nari-Gin (Promoted Silver)
    W_UM = 0b10000 + B_UM, //!< White Uma (Promoted Bishop)
    W_RY = 0b10000 + B_RY, //!< White Ryu (Promoted Rook)

    VOID = 0b11111, //!< Empty Square
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

/**
* @brief Enumeration of board squares following SFEN.
* @details
*   6   5   4   3   2   1
* +---+---+---+---+---+---+
* | 0 | 1 | 2 | 3 | 4 | 5 | a
* +---+---+---+---+---+---+
* | 6 | 7 | 8 | 9 | 10| 11| b
* +---+---+---+---+---+---+
* | 12| 13| 14| 15| 16| 17| c
* +---+---+---+---+---+---+
* | 18| 19| 20| 21| 22| 23| d
* +---+---+---+---+---+---+
* | 24| 25| 26| 27| 28| 29| e
* +---+---+---+---+---+---+
* | 30| 31| 32| 33| 34| 35| f
* +---+---+---+---+---+---+
*/
enum SquareEnum : uint
{
    // clang-format off
    SQ_6A, SQ_5A, SQ_4A, SQ_3A, SQ_2A, SQ_1A,
    SQ_6B, SQ_5B, SQ_4B, SQ_3B, SQ_2B, SQ_1B,
    SQ_6C, SQ_5C, SQ_4C, SQ_3C, SQ_2C, SQ_1C,
    SQ_6D, SQ_5D, SQ_4D, SQ_3D, SQ_2D, SQ_1D,
    SQ_6E, SQ_5E, SQ_4E, SQ_3E, SQ_2E, SQ_1E,
    SQ_6F, SQ_5F, SQ_4F, SQ_3F, SQ_2F, SQ_1F,
    // clang-format on
    SQ_NA,
    NUM_SQ = 36,

    // clang-format off
    A6 = SQ_6A, A5 = SQ_5A, A4 = SQ_4A, A3 = SQ_3A, A2 = SQ_2A, A1 = SQ_1A,
    B6 = SQ_6B, B5 = SQ_5B, B4 = SQ_4B, B3 = SQ_3B, B2 = SQ_2B, B1 = SQ_1B,
    C6 = SQ_6C, C5 = SQ_5C, C4 = SQ_4C, C3 = SQ_3C, C2 = SQ_2C, C1 = SQ_1C,
    D6 = SQ_6D, D5 = SQ_5D, D4 = SQ_4D, D3 = SQ_3D, D2 = SQ_2D, D1 = SQ_1D,
    E6 = SQ_6E, E5 = SQ_5E, E4 = SQ_4E, E3 = SQ_3E, E2 = SQ_2E, E1 = SQ_1E,
    F6 = SQ_6F, F5 = SQ_5F, F4 = SQ_4F, F3 = SQ_3F, F2 = SQ_2F, F1 = SQ_1F,
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
};
enum FileEnum : uint
{
    FILE1,
    FILE2,
    FILE3,
    FILE4,
    FILE5,
    FILE6,
};

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
 * @brief 16-bit integer representing pieces on a stand.
 * @details
 * ________ ______**  FU (2 pieces)
 * ________ ____**__  KE (2 pieces)
 * ________ __**____  GI (2 pieces)
 * ________ **______  KA (2 pieces)
 * ______** ________  HI (2 pieces)
 * ____**__ ________  KI (2 pieces)
 */
using Stand = vshogi::Stand<std::uint16_t, Pieces>;

using BlackWhiteStands = vshogi::BlackWhiteStands<Stand>;

/**
 * @brief 16-bit integer representing a move in Judkins-Shogi game.
 * @details
 *       ________ __******       Destination square (36 possibilities)
 *       ________ _*______       Promotion flag (2 possibilities)
 *       ___***** *_______       Source square or piece (42 possibilities = 36 + 6)
 * (MSB) xxxxxxxx xxxxxxxx (LSB)
 */
using Move = vshogi::Move<std::uint16_t, Squares, Pieces, 12, 6, 5>;

using BitBoard
    = vshogi::BitBoard<std::uint64_t, Squares, BoardPieceTypeEnum, num_attacks>;

using Board = vshogi::Board<Squares, Pieces, BitBoard>;

using State
    = vshogi::State<Board, BlackWhiteStands, Move, max_stand_piece_count>;

using Game = vshogi::Game<State, max_acceptable_repetition>;

constexpr BitBoard bb_1a = (BitBoard(1) << static_cast<uint>(SQ_1A));
constexpr BitBoard bb_1b = (BitBoard(1) << static_cast<uint>(SQ_1B));
constexpr BitBoard bb_1c = (BitBoard(1) << static_cast<uint>(SQ_1C));
constexpr BitBoard bb_1d = (BitBoard(1) << static_cast<uint>(SQ_1D));
constexpr BitBoard bb_1e = (BitBoard(1) << static_cast<uint>(SQ_1E));
constexpr BitBoard bb_1f = (BitBoard(1) << static_cast<uint>(SQ_1F));
constexpr BitBoard bb_2a = (BitBoard(1) << static_cast<uint>(SQ_2A));
constexpr BitBoard bb_2b = (BitBoard(1) << static_cast<uint>(SQ_2B));
constexpr BitBoard bb_2c = (BitBoard(1) << static_cast<uint>(SQ_2C));
constexpr BitBoard bb_2d = (BitBoard(1) << static_cast<uint>(SQ_2D));
constexpr BitBoard bb_2e = (BitBoard(1) << static_cast<uint>(SQ_2E));
constexpr BitBoard bb_2f = (BitBoard(1) << static_cast<uint>(SQ_2F));
constexpr BitBoard bb_3a = (BitBoard(1) << static_cast<uint>(SQ_3A));
constexpr BitBoard bb_3b = (BitBoard(1) << static_cast<uint>(SQ_3B));
constexpr BitBoard bb_3c = (BitBoard(1) << static_cast<uint>(SQ_3C));
constexpr BitBoard bb_3d = (BitBoard(1) << static_cast<uint>(SQ_3D));
constexpr BitBoard bb_3e = (BitBoard(1) << static_cast<uint>(SQ_3E));
constexpr BitBoard bb_3f = (BitBoard(1) << static_cast<uint>(SQ_3F));
constexpr BitBoard bb_4a = (BitBoard(1) << static_cast<uint>(SQ_4A));
constexpr BitBoard bb_4b = (BitBoard(1) << static_cast<uint>(SQ_4B));
constexpr BitBoard bb_4c = (BitBoard(1) << static_cast<uint>(SQ_4C));
constexpr BitBoard bb_4d = (BitBoard(1) << static_cast<uint>(SQ_4D));
constexpr BitBoard bb_4e = (BitBoard(1) << static_cast<uint>(SQ_4E));
constexpr BitBoard bb_4f = (BitBoard(1) << static_cast<uint>(SQ_4F));
constexpr BitBoard bb_5a = (BitBoard(1) << static_cast<uint>(SQ_5A));
constexpr BitBoard bb_5b = (BitBoard(1) << static_cast<uint>(SQ_5B));
constexpr BitBoard bb_5c = (BitBoard(1) << static_cast<uint>(SQ_5C));
constexpr BitBoard bb_5d = (BitBoard(1) << static_cast<uint>(SQ_5D));
constexpr BitBoard bb_5e = (BitBoard(1) << static_cast<uint>(SQ_5E));
constexpr BitBoard bb_5f = (BitBoard(1) << static_cast<uint>(SQ_5F));
constexpr BitBoard bb_6a = (BitBoard(1) << static_cast<uint>(SQ_6A));
constexpr BitBoard bb_6b = (BitBoard(1) << static_cast<uint>(SQ_6B));
constexpr BitBoard bb_6c = (BitBoard(1) << static_cast<uint>(SQ_6C));
constexpr BitBoard bb_6d = (BitBoard(1) << static_cast<uint>(SQ_6D));
constexpr BitBoard bb_6e = (BitBoard(1) << static_cast<uint>(SQ_6E));
constexpr BitBoard bb_6f = (BitBoard(1) << static_cast<uint>(SQ_6F));

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
inline DirectionEnum
    judkins_shogi::Pieces::attack_directions_table[2 * num_piece_types + 1][9]
    = {};

template <>
inline const vshogi::judkins_shogi::PieceTypeEnum
    vshogi::judkins_shogi::Pieces::piece_array[]
    = {vshogi::judkins_shogi::FU,
       vshogi::judkins_shogi::KE,
       vshogi::judkins_shogi::GI,
       vshogi::judkins_shogi::KA,
       vshogi::judkins_shogi::HI,
       vshogi::judkins_shogi::KI,
       vshogi::judkins_shogi::OU,
       vshogi::judkins_shogi::TO,
       vshogi::judkins_shogi::NK,
       vshogi::judkins_shogi::NG,
       vshogi::judkins_shogi::UM,
       vshogi::judkins_shogi::RY};

template <>
inline const vshogi::judkins_shogi::PieceTypeEnum
    vshogi::judkins_shogi::Pieces::stand_piece_array[]
    = {vshogi::judkins_shogi::FU,
       vshogi::judkins_shogi::KE,
       vshogi::judkins_shogi::GI,
       vshogi::judkins_shogi::KA,
       vshogi::judkins_shogi::HI,
       vshogi::judkins_shogi::KI};

template <>
inline vshogi::judkins_shogi::PieceTypeEnum
vshogi::judkins_shogi::Pieces::to_piece_type(const char c)
{
    using namespace vshogi::judkins_shogi;
    switch (std::tolower(c)) {
    case 'p':
        return FU;
    case 'n':
        return KE;
    case 's':
        return GI;
    case 'b':
        return KA;
    case 'r':
        return HI;
    case 'g':
        return KI;
    case 'k':
        return OU;
    default:
        return NA;
    }
}

template <>
inline char vshogi::judkins_shogi::Pieces::to_char(
    const vshogi::judkins_shogi::PieceTypeEnum& p)
{
    using namespace vshogi::judkins_shogi;
    switch (p) {
    case FU:
        return 'p';
    case KE:
        return 'n';
    case GI:
        return 's';
    case KA:
        return 'b';
    case HI:
        return 'r';
    case KI:
        return 'g';
    case OU:
        return 'k';
    default:
        return '\0';
    }
}

template <>
inline constexpr bool vshogi::judkins_shogi::Pieces::is_promotable(
    const vshogi::judkins_shogi::PieceTypeEnum& p)
{
    return (p < vshogi::judkins_shogi::KI);
}

template <>
inline bool vshogi::judkins_shogi::Pieces::is_ranging_to(
    const vshogi::judkins_shogi::BoardPieceTypeEnum& p, const DirectionEnum& d)
{
    using namespace vshogi::judkins_shogi;
    const auto base = demote(to_piece_type(p));
    if (base == HI)
        return (d == DIR_N) || (d == DIR_W) || (d == DIR_E) || (d == DIR_S);
    if (base == KA)
        return (d == DIR_NW) || (d == DIR_NE) || (d == DIR_SW) || (d == DIR_SE);
    return false;
}

template <>
inline bool
judkins_shogi::Pieces::is_ranging_piece(const judkins_shogi::PieceTypeEnum& pt)
{
    using namespace judkins_shogi;
    return ((pt == KA) || (pt == HI) || (pt == UM) || (pt == RY));
}

template <>
inline uint vshogi::judkins_shogi::Pieces::get_point(
    const vshogi::judkins_shogi::PieceTypeEnum& p)
{
    using namespace vshogi::judkins_shogi;
    switch (demote(p)) {
    case OU:
        return 0u;
    case KA:
    case HI:
        return 5u;
    default:
        return 1u;
    }
}

template <>
inline void judkins_shogi::Pieces::init_attack_directions_of_black()
{
    constexpr DirectionEnum table[][9] = {
        // clang-format off
        {DIR_N,                                                      DIR_NA}, // B_FU
        {DIR_NNW, DIR_NNE,                                           DIR_NA}, // B_KE
        {DIR_NW, DIR_N, DIR_NE, DIR_SW, DIR_SE,                      DIR_NA}, // B_GI
        {DIR_NW, DIR_NE, DIR_SW, DIR_SE,                             DIR_NA}, // B_KA
        {DIR_N, DIR_W, DIR_E, DIR_S,                                 DIR_NA}, // B_HI
        {DIR_NW, DIR_N, DIR_NE, DIR_W, DIR_E, DIR_S,                 DIR_NA}, // B_KI
        {DIR_NW, DIR_N, DIR_NE, DIR_W, DIR_E, DIR_SW, DIR_S, DIR_SE, DIR_NA}, // B_OU
        {DIR_NW, DIR_N, DIR_NE, DIR_W, DIR_E, DIR_S,                 DIR_NA}, // B_TO
        {DIR_NW, DIR_N, DIR_NE, DIR_W, DIR_E, DIR_S,                 DIR_NA}, // B_NK
        {DIR_NW, DIR_N, DIR_NE, DIR_W, DIR_E, DIR_S,                 DIR_NA}, // B_NG
        {DIR_NW, DIR_N, DIR_NE, DIR_W, DIR_E, DIR_SW, DIR_S, DIR_SE, DIR_NA}, // B_UM
        {DIR_NW, DIR_N, DIR_NE, DIR_W, DIR_E, DIR_SW, DIR_S, DIR_SE, DIR_NA}, // B_RY
        // clang-format on
    };
    for (uint ii = num_piece_types; ii--;) {
        for (uint jj = 0; jj < 9; ++jj) {
            if (table[ii][jj] == DIR_NA)
                break;
            attack_directions_table[ii][jj] = table[ii][jj];
        }
    }
}

template <>
inline bool vshogi::judkins_shogi::Squares::in_promotion_zone(
    const vshogi::judkins_shogi::RankEnum& r, const ColorEnum& c)
{
    return (c == BLACK) ? (r <= vshogi::judkins_shogi::RANK2)
                        : (r >= vshogi::judkins_shogi::RANK5);
}

template <>
inline DirectionEnum
vshogi::judkins_shogi::Squares::get_direction_for_diagonal_or_knight(
    const SquareEnum& dst, const SquareEnum& src)
{
    switch (static_cast<int>(src - dst)) {
    case 7:
    case 14:
    case 21:
    case 28:
    case 35:
        return DIR_NW;
    case 5:
    case 10:
    case 15:
    case 20:
    case 25:
        return DIR_NE;
    case 11:
        return DIR_NNE;
    case 13:
        return DIR_NNW;
    case -5:
    case -10:
    case -15:
    case -20:
    case -25:
        return DIR_SW;
    case -7:
    case -14:
    case -21:
    case -28:
    case -35:
        return DIR_SE;
    case -11:
        return DIR_SSW;
    case -13:
        return DIR_SSE;
    default:
        return DIR_NA;
    }
}

template <>
inline const uint judkins_shogi::Stand::shift_bits[] = {0, 2, 4, 6, 8, 10};

template <>
inline const std::uint16_t judkins_shogi::Stand::masks[] = {
    // clang-format off
    0x0003, // FU
    0x000c, // KE
    0x0030, // GI
    0x00c0, // KA
    0x0300, // HI
    0x0c00, // KI
    // clang-format on
};

template <>
inline const std::uint16_t judkins_shogi::Stand::deltas[] = {
    // clang-format off
    0x0001, // FU
    0x0004, // KE
    0x0010, // GI
    0x0040, // KA
    0x0100, // HI
    0x0400, // KI
    // clang-format on
};

template <>
inline const std::uint16_t judkins_shogi::Stand::mask = 0x0fff;

template <>
template <>
inline judkins_shogi::Stand::Stand(
    const int num_fu,
    const int num_ke,
    const int num_gi,
    const int num_ka,
    const int num_hi,
    const int num_ki)
    : Stand(static_cast<std::uint16_t>(
        (num_fu << shift_bits[judkins_shogi::FU])
        + (num_ke << shift_bits[judkins_shogi::KE])
        + (num_gi << shift_bits[judkins_shogi::GI])
        + (num_ka << shift_bits[judkins_shogi::KA])
        + (num_hi << shift_bits[judkins_shogi::HI])
        + (num_ki << shift_bits[judkins_shogi::KI])))
{
}

template <>
inline const judkins_shogi::PieceTypeEnum
    judkins_shogi::BlackWhiteStands::stand_pieces_in_sfen_order[]
    = {judkins_shogi::HI,
       judkins_shogi::KA,
       judkins_shogi::KI,
       judkins_shogi::GI,
       judkins_shogi::KE,
       judkins_shogi::FU};

template <>
inline const int judkins_shogi::BlackWhiteStands::max_sfen_length
    = 13; // "RBGSNPrbgsnp "

template <>
inline const judkins_shogi::BitBoard judkins_shogi::BitBoard::
    square_to_bitboard_array[judkins_shogi::Squares::num_squares + 1]
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
        BitBoard(),
};

template <>
inline judkins_shogi::BitBoard
    judkins_shogi::BitBoard::attacks_table[judkins_shogi::num_attacks]
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
        return (*this & filemask[Dir]) << static_cast<uint>(delta);
    else
        return (*this & filemask[Dir]) >> static_cast<uint>(-delta);
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
    case B_OU:
    case W_OU:
        return attacks_table[8][sq];
    case B_KA:
    case W_KA:
        return attacks_table[9][sq];
    case B_HI:
    case W_HI:
        return attacks_table[10][sq];
    case B_UM:
    case W_UM:
        return attacks_table[11][sq];
    case B_RY:
    case W_RY:
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
    case B_OU:
    case W_OU:
        return attacks_table[8][sq];
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
        attacks_table[8][sq] = attacks_table[2][sq] | attacks_table[3][sq]; // OU
        attacks_table[9][sq] = BitBoard::ranging_attacks_to_diagonal(sq); // KA
        attacks_table[10][sq] = BitBoard::ranging_attacks_to_adjacent(sq); // HI
        attacks_table[11][sq] = attacks_table[9][sq] | attacks_table[8][sq]; // UM
        attacks_table[12][sq] = attacks_table[10][sq] | attacks_table[8][sq]; // RY
        // clang-format on
    }
}

template <>
inline std::uint64_t judkins_shogi::State::zobrist_board
    [judkins_shogi::Squares::num_squares]
    [num_colors * judkins_shogi::Pieces::num_piece_types + 1]
    = {};

template <>
inline std::uint64_t judkins_shogi::State::zobrist_stand
    [num_colors][judkins_shogi::Pieces::num_stand_piece_types][3]
    = {};

} // namespace vshogi

#endif // VSHOGI_VARIANTS_JUDKINS_SHOGI_HPP
