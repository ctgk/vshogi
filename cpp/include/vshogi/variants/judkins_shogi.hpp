#ifndef VSHOGI_VARIANTS_JUDKINS_SHOGI_HPP
#define VSHOGI_VARIANTS_JUDKINS_SHOGI_HPP

#include <cstdint>
#include <type_traits>

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

enum PieceTypeEnum : std::uint8_t
{
    FU, //!< Fu (Pawn)
    KE, //!< Kei (Knight)
    GI, //!< Gin (Silver)
    KA, //!< Kaku (Bishop)
    HI, //!< Hisha (Rook)
    KI, //!< Kin (Gold)
    OU, //!< Ou, Gyoku (King)
    TO, //!< Tokin (Promoted Pawn)
    NK, //!< Nari-Kei (Promoted Knight)
    NG, //!< Nari-Gin (Promoted Silver)
    UM, //!< Uma (Promoted Bishop)
    RY, //!< Ryu (Promoted Rook)
    NA, //!< NA
};

enum ColoredPieceEnum : std::uint8_t
{
    B_FU, //!< Black Fu (Pawn)
    B_KE, //!< Black Kei (Knight)
    B_GI, //!< Black Gin (Silver)
    B_KA, //!< Black Kaku (Bishop)
    B_HI, //!< Black Hisha (Rook)
    B_KI, //!< Black Kin (Gold)
    B_OU, //!< Black Ou, Gyoku (King)
    B_TO, //!< Black Tokin (Promoted Pawn)
    B_NK, //!< Black Nari-Kei (Promoted Knight)
    B_NG, //!< Black Nari-Gin (Promoted Silver)
    B_UM, //!< Black Uma (Promoted Bishop)
    B_RY, //!< Black Ryu (Promoted Rook)

    W_FU, //!< White Fu (Pawn)
    W_KE, //!< White Kei (Knight)
    W_GI, //!< White Gin (Silver)
    W_KA, //!< White Kaku (Bishop)
    W_HI, //!< White Hisha (Rook)
    W_KI, //!< White Kin (Gold)
    W_OU, //!< White Ou, Gyoku (King)
    W_TO, //!< White Tokin (Promoted Pawn)
    W_NK, //!< White Nari-Kei (Promoted Knight)
    W_NG, //!< White Nari-Gin (Promoted Silver)
    W_UM, //!< White Uma (Promoted Bishop)
    W_RY, //!< White Ryu (Promoted Rook)

    VOID, //!< Empty Square
};

/**
* @brief Enumeration of board squares following SFEN.
* @details
*   6   5   4   3   2   1
* +---+---+---+---+---+---+
* | 0 | 1 | 2 | 3 | 4 | 5 | a (1)
* +---+---+---+---+---+---+
* | 6 | 7 | 8 | 9 | 10| 11| b (2)
* +---+---+---+---+---+---+
* | 12| 13| 14| 15| 16| 17| c (3)
* +---+---+---+---+---+---+
* | 18| 19| 20| 21| 22| 23| d (4)
* +---+---+---+---+---+---+
* | 24| 25| 26| 27| 28| 29| e (5)
* +---+---+---+---+---+---+
* | 30| 31| 32| 33| 34| 35| f (6)
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
    RANK1 = 0,
    RANK2,
    RANK3,
    RANK4,
    RANK5,
    RANK6,
};
enum FileEnum : uint
{
    FILE6 = 0,
    FILE5,
    FILE4,
    FILE3,
    FILE2,
    FILE1,
};

struct Config
{
    // clang-format off
    static constexpr uint num_piece_types = 12; // FU, KE, GI, KI, KA, HI, OU, TO, NK, NG, UM, RY
    static constexpr uint num_stand_piece_types = 6; // FU, KE, GI, KI, KA, HI
    static constexpr uint num_files = 6; // 1, 2, 3, 4, 5, 6
    static constexpr uint num_ranks = 6; // A, B, C, D, E, F
    static constexpr uint num_promotion_ranks = 2;
    static constexpr uint num_dir = 12; //!< NW, N, NE, W, E, SW, S, SE, SSW, SSE, NNW, NNE
    static constexpr uint num_dir_dl = 10; //!< NW, N, NE, W, E, SW, S, SE, SSW, SSE
    static constexpr uint num_non_ranging_attacks = 9; // B_FU, W_FU, B_KE, W_KE, B_GI, W_GI, B_KI, W_KI, OU
    static constexpr uint num_attacks = 13; // B_FU, W_FU, B_KE, W_KE, B_GI, W_GI, B_KI, W_KI, OU, KA, HI, UM, RY
    static constexpr uint max_stand_piece_count = 2;
    static constexpr uint max_stand_sfen_length = 13; // "RBGSNPrbgsnp "
    static constexpr uint max_acceptable_repetitions = 3;
    using BaseTypeBitBoard = std::uint64_t;
    // clang-format on

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
    using BaseTypeStand = std::uint16_t;

    Config() = delete;
    using PieceType = PieceTypeEnum;
    using ColoredPiece = ColoredPieceEnum;
    using Square = SquareEnum;
    using File = FileEnum;
    using Rank = RankEnum;
    static constexpr uint num_squares = num_files * num_ranks;
    static constexpr uint num_colored_piece_types = 2 * num_piece_types;
};

using Pieces = vshogi::Pieces<Config>;
using Squares = vshogi::Squares<Config>;
using Move = vshogi::Move<Config>;
using BitBoard = vshogi::BitBoard<Config>;
using Board = vshogi::Board<Config>;
using Stand = vshogi::Stand<Config>;
using BlackWhiteStands = vshogi::BlackWhiteStands<Config>;
using State = vshogi::State<Config>;
using Game = vshogi::Game<Config>;
static_assert(FU == Pieces::FU);
static_assert(OU == Pieces::OU);
static_assert(NA == Pieces::NA);
static_assert(B_OU == Pieces::B_OU);
static_assert(W_OU == Pieces::W_OU);
static_assert(VOID == Pieces::VOID);

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
inline const DirectionEnum
    judkins_shogi::Pieces::attack_directions_table[2 * num_piece_types + 1][9]
    = {
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
{DIR_S,                                                      DIR_NA}, // W_FU
{DIR_SSE, DIR_SSW,                                           DIR_NA}, // W_KE
{DIR_SE, DIR_S, DIR_SW, DIR_NE, DIR_NW,                      DIR_NA}, // W_GI
{DIR_SE, DIR_SW, DIR_NE, DIR_NW,                             DIR_NA}, // W_KA
{DIR_S, DIR_E, DIR_W, DIR_N,                                 DIR_NA}, // W_HI
{DIR_SE, DIR_S, DIR_SW, DIR_E, DIR_W, DIR_N,                 DIR_NA}, // W_KI
{DIR_SE, DIR_S, DIR_SW, DIR_E, DIR_W, DIR_NE, DIR_N, DIR_NW, DIR_NA}, // W_OU
{DIR_SE, DIR_S, DIR_SW, DIR_E, DIR_W, DIR_N,                 DIR_NA}, // W_TO
{DIR_SE, DIR_S, DIR_SW, DIR_E, DIR_W, DIR_N,                 DIR_NA}, // W_NK
{DIR_SE, DIR_S, DIR_SW, DIR_E, DIR_W, DIR_N,                 DIR_NA}, // W_NG
{DIR_SE, DIR_S, DIR_SW, DIR_E, DIR_W, DIR_NE, DIR_N, DIR_NW, DIR_NA}, // W_UM
{DIR_SE, DIR_S, DIR_SW, DIR_E, DIR_W, DIR_NE, DIR_N, DIR_NW, DIR_NA}, // W_RY
        // clang-format on
};

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
    constexpr char table[] = {'p', 'n', 's', 'b', 'r', 'g', 'k'};
    static_assert(0 == vshogi::judkins_shogi::FU);
    static_assert(1 == vshogi::judkins_shogi::KE);
    static_assert(2 == vshogi::judkins_shogi::GI);
    static_assert(3 == vshogi::judkins_shogi::KA);
    static_assert(4 == vshogi::judkins_shogi::HI);
    static_assert(5 == vshogi::judkins_shogi::KI);
    static_assert(6 == vshogi::judkins_shogi::OU);
    return table[p];
}

template <>
inline constexpr bool vshogi::judkins_shogi::Pieces::is_promotable(
    const vshogi::judkins_shogi::PieceTypeEnum& p)
{
    return (p < vshogi::judkins_shogi::KI);
}

template <>
inline bool vshogi::judkins_shogi::Pieces::is_ranging_to(
    const vshogi::judkins_shogi::ColoredPieceEnum& p, const DirectionEnum& d)
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
inline bool vshogi::judkins_shogi::Squares::in_promotion_zone(
    const Rank& r, const ColorEnum& c)
{
    return (c == BLACK) ? (r <= vshogi::judkins_shogi::RANK2)
                        : (r >= vshogi::judkins_shogi::RANK5);
}

template <>
inline DirectionEnum
vshogi::judkins_shogi::Squares::get_direction_for_diagonal_or_knight(
    const Square& dst, const Square& src)
{
    using namespace vshogi::judkins_shogi;
    switch (static_cast<int>(src - dst)) {
    case 7:
        return (to_file(src) < FILE5) ? DIR_NA : DIR_NW;
    case 14:
        return (to_file(src) < FILE4) ? DIR_NA : DIR_NW;
    case 21:
        return (to_file(src) < FILE3) ? DIR_NA : DIR_NW;
    case 28:
        return (to_file(src) < FILE2) ? DIR_NA : DIR_NW;
    case 35:
        return DIR_NW;
    case 5:
        return DIR_NE;
    case 10:
        return (to_file(src) > FILE3) ? DIR_NA : DIR_NE;
    case 15:
        return (to_file(src) > FILE4) ? DIR_NA : DIR_NE;
    case 20:
        return (to_file(src) > FILE5) ? DIR_NA : DIR_NE;
    case 25:
        return (to_file(src) > FILE6) ? DIR_NA : DIR_NE;
    case 11:
        return (to_file(src) == FILE1) ? DIR_NA : DIR_NNE;
    case 13:
        return (to_file(src) == FILE6) ? DIR_NA : DIR_NNW;
    case -5:
        return DIR_SW;
    case -10:
        return (to_file(dst) > FILE3) ? DIR_NA : DIR_SW;
    case -15:
        return (to_file(dst) > FILE4) ? DIR_NA : DIR_SW;
    case -20:
        return (to_file(dst) > FILE5) ? DIR_NA : DIR_SW;
    case -25:
        return (to_file(dst) > FILE6) ? DIR_NA : DIR_SW;
    case -7:
        return (to_file(dst) < FILE5) ? DIR_NA : DIR_SE;
    case -14:
        return (to_file(dst) < FILE4) ? DIR_NA : DIR_SE;
    case -21:
        return (to_file(dst) < FILE3) ? DIR_NA : DIR_SE;
    case -28:
        return (to_file(dst) < FILE2) ? DIR_NA : DIR_SE;
    case -35:
        return DIR_SE;
    case -11:
        return (to_file(dst) == FILE1) ? DIR_NA : DIR_SSW;
    case -13:
        return (to_file(dst) == FILE6) ? DIR_NA : DIR_SSE;
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
inline std::uint64_t judkins_shogi::BlackWhiteStands::zobrist_table
    [num_colors][judkins_shogi::Config::num_stand_piece_types]
    [judkins_shogi::Config::max_stand_piece_count + 1]
    = {};

template <>
inline std::uint64_t judkins_shogi::Board::zobrist_table
    [judkins_shogi::Config::num_squares]
    [num_colors * judkins_shogi::Config::num_piece_types + 1]
    = {};

template <>
inline const judkins_shogi::BitBoard judkins_shogi::BitBoard::
    square_to_bitboard_array[judkins_shogi::Config::num_squares + 1]
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
    judkins_shogi::BitBoard::attacks_table[judkins_shogi::Config::num_attacks]
                                          [judkins_shogi::Config::num_squares]
    = {};

template <>
inline judkins_shogi::BitBoard judkins_shogi::BitBoard::get_attacks_by(
    const vshogi::judkins_shogi::ColoredPieceEnum& p,
    const vshogi::judkins_shogi::SquareEnum& sq)
{
    using namespace vshogi::judkins_shogi;
    switch (p) {
    case vshogi::judkins_shogi::B_FU:
        return attacks_table[0][sq];
    case vshogi::judkins_shogi::B_KE:
        return attacks_table[1][sq];
    case vshogi::judkins_shogi::B_GI:
        return attacks_table[2][sq];
    case vshogi::judkins_shogi::B_KI:
    case vshogi::judkins_shogi::B_TO:
    case vshogi::judkins_shogi::B_NK:
    case vshogi::judkins_shogi::B_NG:
        return attacks_table[3][sq];
    case vshogi::judkins_shogi::W_FU:
        return attacks_table[4][sq];
    case vshogi::judkins_shogi::W_KE:
        return attacks_table[5][sq];
    case vshogi::judkins_shogi::W_GI:
        return attacks_table[6][sq];
    case vshogi::judkins_shogi::W_KI:
    case vshogi::judkins_shogi::W_TO:
    case vshogi::judkins_shogi::W_NK:
    case vshogi::judkins_shogi::W_NG:
        return attacks_table[7][sq];
    case vshogi::judkins_shogi::B_OU:
    case vshogi::judkins_shogi::W_OU:
        return attacks_table[8][sq];
    case vshogi::judkins_shogi::B_KA:
    case vshogi::judkins_shogi::W_KA:
        return attacks_table[9][sq];
    case vshogi::judkins_shogi::B_HI:
    case vshogi::judkins_shogi::W_HI:
        return attacks_table[10][sq];
    case vshogi::judkins_shogi::B_UM:
    case vshogi::judkins_shogi::W_UM:
        return attacks_table[11][sq];
    case vshogi::judkins_shogi::B_RY:
    case vshogi::judkins_shogi::W_RY:
        return attacks_table[12][sq];
    default:
        return BitBoard();
    }
}

template <>
inline judkins_shogi::BitBoard judkins_shogi::BitBoard::get_attacks_by(
    const vshogi::judkins_shogi::ColoredPieceEnum& p,
    const vshogi::judkins_shogi::SquareEnum& sq,
    const vshogi::judkins_shogi::BitBoard& occupied)
{
    using namespace vshogi::judkins_shogi;
    switch (p) {
    case vshogi::judkins_shogi::B_FU:
        return attacks_table[0][sq];
    case vshogi::judkins_shogi::B_KE:
        return attacks_table[1][sq];
    case vshogi::judkins_shogi::B_GI:
        return attacks_table[2][sq];
    case vshogi::judkins_shogi::B_KI:
    case vshogi::judkins_shogi::B_TO:
    case vshogi::judkins_shogi::B_NK:
    case vshogi::judkins_shogi::B_NG:
        return attacks_table[3][sq];
    case vshogi::judkins_shogi::W_FU:
        return attacks_table[4][sq];
    case vshogi::judkins_shogi::W_KE:
        return attacks_table[5][sq];
    case vshogi::judkins_shogi::W_GI:
        return attacks_table[6][sq];
    case vshogi::judkins_shogi::W_KI:
    case vshogi::judkins_shogi::W_TO:
    case vshogi::judkins_shogi::W_NK:
    case vshogi::judkins_shogi::W_NG:
        return attacks_table[7][sq];
    case vshogi::judkins_shogi::B_OU:
    case vshogi::judkins_shogi::W_OU:
        return attacks_table[8][sq];
    case vshogi::judkins_shogi::B_KA:
    case vshogi::judkins_shogi::W_KA:
        return BitBoard::ranging_attacks_to_diagonal(sq, occupied);
    case vshogi::judkins_shogi::B_HI:
    case vshogi::judkins_shogi::W_HI:
        return BitBoard::ranging_attacks_to_adjacent(sq, occupied);
    case vshogi::judkins_shogi::B_UM:
    case vshogi::judkins_shogi::W_UM:
        return BitBoard::ranging_attacks_to_diagonal(sq, occupied)
               | attacks_table[8][sq];
    case vshogi::judkins_shogi::B_RY:
    case vshogi::judkins_shogi::W_RY:
        return BitBoard::ranging_attacks_to_adjacent(sq, occupied)
               | attacks_table[8][sq];
    default:
        return BitBoard();
    }
}

template <>
inline void judkins_shogi::BitBoard::init_tables()
{
    for (auto sq : EnumIterator<Square, num_squares>()) {
        const auto b = from_square(sq);
        // clang-format off
        attacks_table[0][sq] = b.shift(DIR_N); // B_FU
        attacks_table[1][sq] = b.shift(DIR_NNW) | b.shift(DIR_NNE); // B_KE
        attacks_table[2][sq] = b.shift(DIR_NW) | b.shift(DIR_N) | b.shift(DIR_NE) | b.shift(DIR_SW) | b.shift(DIR_SE); // B_GI
        attacks_table[3][sq] = b.shift(DIR_NW) | b.shift(DIR_N) | b.shift(DIR_NE) | b.shift(DIR_W) | b.shift(DIR_E) | b.shift(DIR_S); // B_KI
        attacks_table[4][sq] = b.shift(DIR_S); // W_FU
        attacks_table[5][sq] = b.shift(DIR_SSW) | b.shift(DIR_SSE); // W_KE
        attacks_table[6][sq] = b.shift(DIR_NW) | b.shift(DIR_NE) | b.shift(DIR_SW) | b.shift(DIR_S) | b.shift(DIR_SE); // W_GI
        attacks_table[7][sq] = b.shift(DIR_N) | b.shift(DIR_W) | b.shift(DIR_E) | b.shift(DIR_SW) | b.shift(DIR_S) | b.shift(DIR_SE); // W_KI
        attacks_table[8][sq] = attacks_table[2][sq] | attacks_table[3][sq]; // OU
        attacks_table[9][sq] = BitBoard::ranging_attacks_to_diagonal(sq); // KA
        attacks_table[10][sq] = BitBoard::ranging_attacks_to_adjacent(sq); // HI
        attacks_table[11][sq] = attacks_table[9][sq] | attacks_table[8][sq]; // UM
        attacks_table[12][sq] = attacks_table[10][sq] | attacks_table[8][sq]; // RY
        // clang-format on
    }
}

} // namespace vshogi

#endif // VSHOGI_VARIANTS_JUDKINS_SHOGI_HPP
