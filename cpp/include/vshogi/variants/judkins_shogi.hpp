#ifndef VSHOGI_VARIANTS_JUDKINS_SHOGI_HPP
#define VSHOGI_VARIANTS_JUDKINS_SHOGI_HPP

#include <cstdint>
#include <type_traits>

#include "vshogi/common/bitboard.hpp"
#include "vshogi/common/board.hpp"
#include "vshogi/common/color.hpp"
#include "vshogi/common/game.hpp"
#include "vshogi/common/generator.hpp"
#include "vshogi/common/magic.hpp"
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
    static constexpr char piece_type_to_char[] = "pnsbrgk";
    static constexpr uint num_piece_types = 12; // FU, KE, GI, KA, HI, KI, OU, TO, NK, NG, UM, RY
    static constexpr uint num_stand_piece_types = 6; // FU, KE, GI, KA, HI, KI
    static constexpr uint piece_type_to_point[] = {1, 1, 1, 5, 5, 1, 0, 1, 1, 1, 5, 5, 0};
    static constexpr uint num_files = 6; // 1, 2, 3, 4, 5, 6
    static constexpr uint num_ranks = 6; // A, B, C, D, E, F
    static constexpr uint num_promotion_ranks = 2;
    static constexpr uint num_dir = 12; //!< NW, N, NE, W, E, SW, S, SE, SSW, SSE, NNW, NNE
    static constexpr uint num_dir_dl = 10; //!< NW, N, NE, W, E, SW, S, SE, SSW, SSE
    static constexpr uint log2_magic_table_size = 4;
    static constexpr uint max_stand_piece_count = 2;
    static constexpr uint max_stand_sfen_length = 13; // "RBGSNPrbgsnp "
    static constexpr uint max_acceptable_repetitions = 3;
    static constexpr uint half_num_initial_pieces = 3;
    static constexpr uint initial_points = 14;
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
    static constexpr uint magic_table_size = 1u << log2_magic_table_size;
};

using Pieces = vshogi::Pieces<Config>;
using Squares = vshogi::Squares<Config>;
using Move = vshogi::Move<Config>;
using BitBoard = vshogi::BitBoard<Config>;
using Magic = vshogi::Magic<Config>;
using Board = vshogi::Board<Config>;
using Stand = vshogi::Stand<Config>;
using BlackWhiteStands = vshogi::BlackWhiteStands<Config>;
using State = vshogi::State<Config>;
using DropMoveGenerator = vshogi::DropMoveGenerator<Config>;
using CheckDropMoveGenerator = vshogi::CheckDropMoveGenerator<Config>;
using NonKingBoardMoveGenerator = vshogi::NonKingBoardMoveGenerator<Config>;
using CheckNonKingBoardMoveGenerator
    = vshogi::CheckNonKingBoardMoveGenerator<Config>;
using KingMoveGenerator = vshogi::KingMoveGenerator<Config>;
using CheckKingMoveGenerator = vshogi::CheckKingMoveGenerator<Config>;
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
inline judkins_shogi::BitBoard judkins_shogi::BitBoard::attacks_table
    [judkins_shogi::Config::num_colored_piece_types]
    [judkins_shogi::Config::num_squares]
    = {};

template <>
inline judkins_shogi::BitBoard
    judkins_shogi::BitBoard::ray_table[judkins_shogi::Config::num_squares]
                                      [judkins_shogi::Config::num_dir]
    = {};
template <>
inline judkins_shogi::BitBoard judkins_shogi::BitBoard::line_segment_table
    [judkins_shogi::Config::num_squares][judkins_shogi::Config::num_squares]
    = {};

template <>
inline judkins_shogi::BitBoard judkins_shogi::BitBoard::get_attacks_by(
    const judkins_shogi::ColoredPieceEnum& p,
    const judkins_shogi::SquareEnum& sq,
    const judkins_shogi::BitBoard& occupied)
{
    switch (p) {
    case judkins_shogi::B_KA:
    case judkins_shogi::W_KA:
        return judkins_shogi::Magic::get_diagonal_attack(sq, occupied);
    case judkins_shogi::B_HI:
    case judkins_shogi::W_HI:
        return judkins_shogi::Magic::get_adjacent_attack(sq, occupied);
    case judkins_shogi::B_UM:
    case judkins_shogi::W_UM:
        return judkins_shogi::Magic::get_diagonal_attack(sq, occupied)
               | attacks_table[judkins_shogi::B_OU][sq];
    case judkins_shogi::B_RY:
    case judkins_shogi::W_RY:
        return judkins_shogi::Magic::get_adjacent_attack(sq, occupied)
               | attacks_table[judkins_shogi::B_OU][sq];
    default:
        return get_attacks_by(p, sq);
    }
}

template <>
inline const judkins_shogi::BitBoard
    judkins_shogi::Magic::premask_vertical[judkins_shogi::Config::num_squares]
    = {
        // clang-format off
        0x0000000001041040, 0x0000000002082080, 0x0000000004104100, 0x0000000008208200, 0x0000000010410400, 0x0000000020820800,
        0x0000000001041000, 0x0000000002082000, 0x0000000004104000, 0x0000000008208000, 0x0000000010410000, 0x0000000020820000,
        0x0000000001040040, 0x0000000002080080, 0x0000000004100100, 0x0000000008200200, 0x0000000010400400, 0x0000000020800800,
        0x0000000001001040, 0x0000000002002080, 0x0000000004004100, 0x0000000008008200, 0x0000000010010400, 0x0000000020020800,
        0x0000000000041040, 0x0000000000082080, 0x0000000000104100, 0x0000000000208200, 0x0000000000410400, 0x0000000000820800,
        0x0000000001041040, 0x0000000002082080, 0x0000000004104100, 0x0000000008208200, 0x0000000010410400, 0x0000000020820800,
        // clang-format on
};
template <>
inline const judkins_shogi::BitBoard
    judkins_shogi::Magic::premask_horizontal[judkins_shogi::Config::num_squares]
    = {
        // clang-format off
        0x000000000000001e, 0x000000000000001c, 0x000000000000001a, 0x0000000000000016, 0x000000000000000e, 0x000000000000001e,
        0x0000000000000780, 0x0000000000000700, 0x0000000000000680, 0x0000000000000580, 0x0000000000000380, 0x0000000000000780,
        0x000000000001e000, 0x000000000001c000, 0x000000000001a000, 0x0000000000016000, 0x000000000000e000, 0x000000000001e000,
        0x0000000000780000, 0x0000000000700000, 0x0000000000680000, 0x0000000000580000, 0x0000000000380000, 0x0000000000780000,
        0x000000001e000000, 0x000000001c000000, 0x000000001a000000, 0x0000000016000000, 0x000000000e000000, 0x000000001e000000,
        0x0000000780000000, 0x0000000700000000, 0x0000000680000000, 0x0000000580000000, 0x0000000380000000, 0x0000000780000000,
        // clang-format on
};
template <>
inline const judkins_shogi::BitBoard
    judkins_shogi::Magic::premask_nw_se[judkins_shogi::Config::num_squares]
    = {
        // clang-format off
        0x0000000010204080, 0x0000000000408100, 0x0000000000010200, 0x0000000000000400, 0x0000000000000000, 0x0000000000000000,
        0x0000000008102000, 0x0000000010204000, 0x0000000000408000, 0x0000000000010000, 0x0000000000000000, 0x0000000000000000,
        0x0000000004080000, 0x0000000008100000, 0x0000000010200080, 0x0000000000400100, 0x0000000000000200, 0x0000000000000400,
        0x0000000002000000, 0x0000000004000000, 0x0000000008002000, 0x0000000010004080, 0x0000000000008100, 0x0000000000010200,
        0x0000000000000000, 0x0000000000000000, 0x0000000000080000, 0x0000000000102000, 0x0000000000204080, 0x0000000000408100,
        0x0000000000000000, 0x0000000000000000, 0x0000000002000000, 0x0000000004080000, 0x0000000008102000, 0x0000000010204080,
        // clang-format on
};
template <>
inline const judkins_shogi::BitBoard
    judkins_shogi::Magic::premask_sw_ne[judkins_shogi::Config::num_squares]
    = {
        // clang-format off
        0x0000000000000000, 0x0000000000000000, 0x0000000000000080, 0x0000000000002100, 0x0000000000084200, 0x0000000002108400,
        0x0000000000000000, 0x0000000000000000, 0x0000000000002000, 0x0000000000084000, 0x0000000002108000, 0x0000000004210000,
        0x0000000000000080, 0x0000000000000100, 0x0000000000080200, 0x0000000002100400, 0x0000000004200000, 0x0000000008400000,
        0x0000000000002100, 0x0000000000004200, 0x0000000002008400, 0x0000000004010000, 0x0000000008000000, 0x0000000010000000,
        0x0000000000084200, 0x0000000000108400, 0x0000000000210000, 0x0000000000400000, 0x0000000000000000, 0x0000000000000000,
        0x0000000002108400, 0x0000000004210000, 0x0000000008400000, 0x0000000010000000, 0x0000000000000000, 0x0000000000000000,
        // clang-format on
};
template <>
inline const std::uint32_t judkins_shogi::Magic::magic_number_vertical
    [judkins_shogi::Config::num_squares]
    = {
        0x04883090, 0x00911008, 0x88221404, 0x22404222, 0x21844202, 0x00122105,
        0x80821850, 0x8c010220, 0x0000a810, 0x04022190, 0x81514201, 0x00000a54,
        0x21004570, 0x41020210, 0x48200824, 0xa2200102, 0x0008c101, 0x04100029,
        0x82810041, 0x18ac0431, 0x2380400c, 0x00988104, 0x10105086, 0x01040824,
        0x829508a0, 0x8940c840, 0x00204400, 0x10c288d0, 0x222010c2, 0x00906050,
        0x01012020, 0x05a21010, 0x0020c812, 0x00a44402, 0x01088164, 0x2604a364,
};
template <>
inline const std::uint32_t judkins_shogi::Magic::magic_number_horizontal
    [judkins_shogi::Config::num_squares]
    = {
        0x11108500, 0xf4044400, 0x44024000, 0x08800000, 0x08008888, 0x22120002,
        0x10204600, 0x1010d801, 0x20515001, 0x08a40000, 0x40200818, 0x0c200904,
        0x00088060, 0x04004000, 0x10008454, 0x0080a000, 0x40808140, 0x57011000,
        0x00200214, 0x01000102, 0x00202215, 0x11008282, 0x08900200, 0x00240610,
        0x09000008, 0x0001000a, 0x15905008, 0x00200221, 0x0040020c, 0x42000011,
        0x11020101, 0x11000100, 0xa80064a1, 0x64000481, 0x24040021, 0x10000201,
};
template <>
inline const std::uint32_t
    judkins_shogi::Magic::magic_number_nw_se[judkins_shogi::Config::num_squares]
    = {
        0x09138084, 0x00808894, 0x81104204, 0x00040420, 0xffffffff, 0xffffffff,
        0x00410806, 0x00006302, 0x00843200, 0x02e43801, 0xffffffff, 0xffffffff,
        0x06100404, 0x02000108, 0x85140a05, 0x10208460, 0x64a20010, 0x04040005,
        0x21014008, 0x04080120, 0x20020010, 0x80208127, 0xc0808c40, 0x01108108,
        0xffffffff, 0xffffffff, 0x200a8400, 0x04188500, 0x40c84100, 0x70130490,
        0xffffffff, 0xffffffff, 0x450200a2, 0x00401019, 0x04020802, 0x00826108,
};
template <>
inline const std::uint32_t
    judkins_shogi::Magic::magic_number_sw_ne[judkins_shogi::Config::num_squares]
    = {
        0xffffffff, 0xffffffff, 0x8c220201, 0x04244a00, 0x00121880, 0x04248640,
        0xffffffff, 0xffffffff, 0x22040219, 0x1000a310, 0x28049340, 0x824340a0,
        0x09200c02, 0x18240040, 0x00210a44, 0x010c092c, 0x00000521, 0x00001203,
        0x00c40400, 0x08204000, 0x01122044, 0x12028013, 0xc0410089, 0x48008004,
        0x2e448800, 0x90204400, 0x20014102, 0x105e88a0, 0xffffffff, 0xffffffff,
        0x11090120, 0x21088588, 0x44121570, 0x0110c008, 0xffffffff, 0xffffffff,
};
template <>
inline judkins_shogi::BitBoard judkins_shogi::Magic::attack_table_vertical
    [judkins_shogi::Config::num_squares]
    [judkins_shogi::Config::magic_table_size]
    = {};
template <>
inline judkins_shogi::BitBoard judkins_shogi::Magic::attack_table_horizontal
    [judkins_shogi::Config::num_squares]
    [judkins_shogi::Config::magic_table_size]
    = {};
template <>
inline judkins_shogi::BitBoard judkins_shogi::Magic::attack_table_nw_se
    [judkins_shogi::Config::num_squares]
    [judkins_shogi::Config::magic_table_size]
    = {};
template <>
inline judkins_shogi::BitBoard judkins_shogi::Magic::attack_table_sw_ne
    [judkins_shogi::Config::num_squares]
    [judkins_shogi::Config::magic_table_size]
    = {};

} // namespace vshogi

#endif // VSHOGI_VARIANTS_JUDKINS_SHOGI_HPP
