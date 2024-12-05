#ifndef VSHOGI_VARIANTS_SHOGI_HPP
#define VSHOGI_VARIANTS_SHOGI_HPP

#include <cassert>
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

namespace vshogi::shogi
{

enum PieceTypeEnum : std::uint8_t
{
    FU, //!< Fu (Pawn)
    KY, //!< Kyo (Lance)
    KE, //!< Kei (Knight)
    GI, //!< Gin (Silver)
    KA, //!< Kaku (Bishop)
    HI, //!< Hisha (Rook)
    KI, //!< Kin (Gold)
    OU, //!< Ou, Gyoku (King)
    TO, //!< Tokin (Promoted Pawn)
    NY, //!< Nari-Kyo (Promoted Lance)
    NK, //!< Nari-Kei (Promoted Knight)
    NG, //!< Nari-Gin (Promoted Silver)
    UM, //!< Uma (Promoted Bishop)
    RY, //!< Ryu (Promoted Rook)
    NA, //!< NA
};

enum ColoredPieceEnum : std::uint8_t
{
    B_FU, //!< Black Fu (Pawn)
    B_KY, //!< Black Kyo (Lance)
    B_KE, //!< Black Kei (Knight)
    B_GI, //!< Black Gin (Silver)
    B_KA, //!< Black Kaku (Bishop)
    B_HI, //!< Black Hisha (Rook)
    B_KI, //!< Black Kin (Gold)
    B_OU, //!< Black Ou, Gyoku (King)
    B_TO, //!< Black Tokin (Promoted Pawn)
    B_NY, //!< Black Nari-Kyo (Promoted Lance)
    B_NK, //!< Black Nari-Kei (Promoted Knight)
    B_NG, //!< Black Nari-Gin (Promoted Silver)
    B_UM, //!< Black Uma (Promoted Bishop)
    B_RY, //!< Black Ryu (Promoted Rook)

    W_FU, //!< White Fu (Pawn)
    W_KY, //!< White Kyo (Lance)
    W_KE, //!< White Kei (Knight)
    W_GI, //!< White Gin (Silver)
    W_KA, //!< White Kaku (Bishop)
    W_HI, //!< White Hisha (Rook)
    W_KI, //!< White Kin (Gold)
    W_OU, //!< White Ou, Gyoku (King)
    W_TO, //!< White Tokin (Promoted Pawn)
    W_NY, //!< White Nari-Kyo (Promoted Lance)
    W_NK, //!< White Nari-Kei (Promoted Knight)
    W_NG, //!< White Nari-Gin (Promoted Silver)
    W_UM, //!< White Uma (Promoted Bishop)
    W_RY, //!< White Ryu (Promoted Rook)

    VOID, //!< Empty Square
};

/**
 * @brief Enumeration of board squares following SFEN.
 * @details
 *   9   8   7   6   5   4   3   2   1
 * +---+---+---+---+---+---+---+---+---+
 * | 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | a (1)
 * +---+---+---+---+---+---+---+---+---+
 * | 9 | 10| 11| 12| 13| 14| 15| 16| 17| b (2)
 * +---+---+---+---+---+---+---+---+---+
 * | 18| 19| 20| 21| 22| 23| 24| 25| 26| c (3)
 * +---+---+---+---+---+---+---+---+---+
 * | 27| 28| 29| 30| 31| 32| 33| 34| 35| d (4)
 * +---+---+---+---+---+---+---+---+---+
 * | 36| 37| 38| 39| 40| 41| 42| 43| 44| e (5)
 * +---+---+---+---+---+---+---+---+---+
 * | 45| 46| 47| 48| 49| 50| 51| 52| 53| f (6)
 * +---+---+---+---+---+---+---+---+---+
 * | 54| 55| 56| 57| 58| 59| 60| 61| 62| g (7)
 * +---+---+---+---+---+---+---+---+---+
 * | 63| 64| 65| 66| 67| 68| 69| 70| 71| h (8)
 * +---+---+---+---+---+---+---+---+---+
 * | 72| 73| 74| 75| 76| 77| 78| 79| 80| i (9)
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
    RANK1 = 0,
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
    FILE9 = 0,
    FILE8,
    FILE7,
    FILE6,
    FILE5,
    FILE4,
    FILE3,
    FILE2,
    FILE1,
};

struct Config
{
    // clang-format off
    static constexpr char piece_type_to_char[] = "plnsbrgk";
    static constexpr uint num_piece_types = 14; // FU, KY, KE, GI, KA, HI, KI, OU, TO, NY, NK, NG, UM, RY
    static constexpr uint num_stand_piece_types = 7; // FU, KY, KE, GI, KA, HI, KI
    static constexpr uint piece_type_to_point[] = {1, 1, 1, 1, 5, 5, 1, 0, 1, 1, 1, 1, 5, 5, 0};
    static constexpr uint piece_type_to_value[] = {5, 30, 35, 55, 95, 100, 60, 0, 60, 60, 60, 60, 115, 120, 0}; // https://shogi.zukeran.org/2018/06/12/piece-value-1/
    static constexpr uint sum_piece_value = 2 * (60 * 17 + 115 + 120);
    static constexpr uint num_files = 9; // 1, 2, 3, 4, 5, 6, 7, 8, 9
    static constexpr uint num_ranks = 9; // A, B, C, D, E, F, G, H, I
    static constexpr uint num_promotion_ranks = 3;
    static constexpr uint num_dir = 12; //!< NW, N, NE, W, E, SW, S, SE, SSW, SSE, NNW, NNE
    static constexpr uint num_dir_dl = 10; //!< NW, N, NE, W, E, SW, S, SE, SSW, SSE
    static constexpr uint log2_magic_table_size = 12;
    static constexpr uint max_stand_piece_count = 18;
    static constexpr uint max_stand_sfen_length = 26; // "10p2l2n2sbr2g2P2L2N2SBR2G "
    static constexpr uint max_acceptable_repetitions = 3;
    static constexpr uint half_num_initial_pieces = 10;
    static constexpr uint initial_points = 27;
    using BaseTypeBitBoard = uint128;
    // clang-format on

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
    using BaseTypeStand = std::uint32_t;

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
inline bool vshogi::shogi::Pieces::is_ranging_to(
    const vshogi::shogi::ColoredPieceEnum& p, const DirectionEnum& d)
{
    using namespace vshogi::shogi;
    if (p == vshogi::shogi::B_KY)
        return (d == DIR_N);
    if (p == vshogi::shogi::W_KY)
        return (d == DIR_S);
    const auto base = demote(to_piece_type(p));
    if (base == vshogi::shogi::HI)
        return (d == DIR_N) || (d == DIR_W) || (d == DIR_E) || (d == DIR_S);
    if (base == vshogi::shogi::KA)
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
inline const shogi::PieceTypeEnum
    shogi::BlackWhiteStands::stand_pieces_in_sfen_order[]
    = {shogi::HI,
       shogi::KA,
       shogi::KI,
       shogi::GI,
       shogi::KE,
       shogi::KY,
       shogi::FU};

template <>
inline std::uint64_t shogi::BlackWhiteStands::zobrist_table
    [num_colors][shogi::Config::num_stand_piece_types]
    [shogi::Config::max_stand_piece_count + 1]
    = {};

template <>
inline std::uint64_t vshogi::shogi::Board::zobrist_table
    [shogi::Config::num_squares]
    [num_colors * shogi::Config::num_piece_types + 1]
    = {};

template <>
inline shogi::BitBoard
    shogi::BitBoard::attacks_table[shogi::Config::num_colored_piece_types]
                                  [shogi::Config::num_squares]
    = {};

template <>
inline shogi::BitBoard shogi::BitBoard::ray_table[shogi::Config::num_squares]
                                                 [shogi::Config::num_dir]
    = {};
template <>
inline shogi::BitBoard
    shogi::BitBoard::line_segment_table[shogi::Config::num_squares]
                                       [shogi::Config::num_squares]
    = {};

template <>
inline shogi::BitBoard shogi::BitBoard::get_attacks_by(
    const shogi::ColoredPieceEnum& p,
    const shogi::SquareEnum& sq,
    const shogi::BitBoard& occupied)
{
    switch (p) {
    case shogi::B_KY:
        return shogi::Magic::get_north_attack(sq, occupied);
    case shogi::W_KY:
        return shogi::Magic::get_south_attack(sq, occupied);
    case shogi::B_KA:
    case shogi::W_KA:
        return shogi::Magic::get_diagonal_attack(sq, occupied);
    case shogi::B_HI:
    case shogi::W_HI:
        return shogi::Magic::get_adjacent_attack(sq, occupied);
    case shogi::B_UM:
    case shogi::W_UM:
        return shogi::Magic::get_diagonal_attack(sq, occupied)
               | attacks_table[shogi::B_OU][sq];
    case shogi::B_RY:
    case shogi::W_RY:
        return shogi::Magic::get_adjacent_attack(sq, occupied)
               | attacks_table[shogi::B_OU][sq];
    default:
        return get_attacks_by(p, sq);
    }
}

template <>
inline const shogi::BitBoard
    shogi::Magic::premask_north[shogi::Config::num_squares]
    = {
        // clang-format off
(static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x0000000000000000), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x0000000000000000), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x0000000000000000), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x0000000000000000), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x0000000000000000), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x0000000000000000), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x0000000000000000), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x0000000000000000), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x0000000000000000),
(static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x0000000000000000), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x0000000000000000), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x0000000000000000), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x0000000000000000), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x0000000000000000), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x0000000000000000), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x0000000000000000), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x0000000000000000), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x0000000000000000),
(static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x0000000000000200), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x0000000000000400), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x0000000000000800), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x0000000000001000), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x0000000000002000), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x0000000000004000), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x0000000000008000), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x0000000000010000), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x0000000000020000),
(static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x0000000000040200), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x0000000000080400), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x0000000000100800), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x0000000000201000), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x0000000000402000), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x0000000000804000), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x0000000001008000), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x0000000002010000), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x0000000004020000),
(static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x0000000008040200), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x0000000010080400), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x0000000020100800), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x0000000040201000), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x0000000080402000), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x0000000100804000), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x0000000201008000), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x0000000402010000), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x0000000804020000),
(static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x0000001008040200), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x0000002010080400), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x0000004020100800), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x0000008040201000), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x0000010080402000), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x0000020100804000), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x0000040201008000), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x0000080402010000), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x0000100804020000),
(static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x0000201008040200), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x0000402010080400), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x0000804020100800), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x0001008040201000), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x0002010080402000), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x0004020100804000), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x0008040201008000), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x0010080402010000), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x0020100804020000),
(static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x0040201008040200), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x0080402010080400), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x0100804020100800), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x0201008040201000), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x0402010080402000), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x0804020100804000), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x1008040201008000), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x2010080402010000), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x4020100804020000),
(static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x8040201008040200), (static_cast<uint128>(0x00001) << 64) | static_cast<uint128>(0x0080402010080400), (static_cast<uint128>(0x00002) << 64) | static_cast<uint128>(0x0100804020100800), (static_cast<uint128>(0x00004) << 64) | static_cast<uint128>(0x0201008040201000), (static_cast<uint128>(0x00008) << 64) | static_cast<uint128>(0x0402010080402000), (static_cast<uint128>(0x00010) << 64) | static_cast<uint128>(0x0804020100804000), (static_cast<uint128>(0x00020) << 64) | static_cast<uint128>(0x1008040201008000), (static_cast<uint128>(0x00040) << 64) | static_cast<uint128>(0x2010080402010000), (static_cast<uint128>(0x00080) << 64) | static_cast<uint128>(0x4020100804020000),
        // clang-format on
};
template <>
inline const shogi::BitBoard
    shogi::Magic::premask_south[shogi::Config::num_squares]
    = {
        // clang-format off
(static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x8040201008040200), (static_cast<uint128>(0x00001) << 64) | static_cast<uint128>(0x0080402010080400), (static_cast<uint128>(0x00002) << 64) | static_cast<uint128>(0x0100804020100800), (static_cast<uint128>(0x00004) << 64) | static_cast<uint128>(0x0201008040201000), (static_cast<uint128>(0x00008) << 64) | static_cast<uint128>(0x0402010080402000), (static_cast<uint128>(0x00010) << 64) | static_cast<uint128>(0x0804020100804000), (static_cast<uint128>(0x00020) << 64) | static_cast<uint128>(0x1008040201008000), (static_cast<uint128>(0x00040) << 64) | static_cast<uint128>(0x2010080402010000), (static_cast<uint128>(0x00080) << 64) | static_cast<uint128>(0x4020100804020000),
(static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x8040201008040000), (static_cast<uint128>(0x00001) << 64) | static_cast<uint128>(0x0080402010080000), (static_cast<uint128>(0x00002) << 64) | static_cast<uint128>(0x0100804020100000), (static_cast<uint128>(0x00004) << 64) | static_cast<uint128>(0x0201008040200000), (static_cast<uint128>(0x00008) << 64) | static_cast<uint128>(0x0402010080400000), (static_cast<uint128>(0x00010) << 64) | static_cast<uint128>(0x0804020100800000), (static_cast<uint128>(0x00020) << 64) | static_cast<uint128>(0x1008040201000000), (static_cast<uint128>(0x00040) << 64) | static_cast<uint128>(0x2010080402000000), (static_cast<uint128>(0x00080) << 64) | static_cast<uint128>(0x4020100804000000),
(static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x8040201008000000), (static_cast<uint128>(0x00001) << 64) | static_cast<uint128>(0x0080402010000000), (static_cast<uint128>(0x00002) << 64) | static_cast<uint128>(0x0100804020000000), (static_cast<uint128>(0x00004) << 64) | static_cast<uint128>(0x0201008040000000), (static_cast<uint128>(0x00008) << 64) | static_cast<uint128>(0x0402010080000000), (static_cast<uint128>(0x00010) << 64) | static_cast<uint128>(0x0804020100000000), (static_cast<uint128>(0x00020) << 64) | static_cast<uint128>(0x1008040200000000), (static_cast<uint128>(0x00040) << 64) | static_cast<uint128>(0x2010080400000000), (static_cast<uint128>(0x00080) << 64) | static_cast<uint128>(0x4020100800000000),
(static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x8040201000000000), (static_cast<uint128>(0x00001) << 64) | static_cast<uint128>(0x0080402000000000), (static_cast<uint128>(0x00002) << 64) | static_cast<uint128>(0x0100804000000000), (static_cast<uint128>(0x00004) << 64) | static_cast<uint128>(0x0201008000000000), (static_cast<uint128>(0x00008) << 64) | static_cast<uint128>(0x0402010000000000), (static_cast<uint128>(0x00010) << 64) | static_cast<uint128>(0x0804020000000000), (static_cast<uint128>(0x00020) << 64) | static_cast<uint128>(0x1008040000000000), (static_cast<uint128>(0x00040) << 64) | static_cast<uint128>(0x2010080000000000), (static_cast<uint128>(0x00080) << 64) | static_cast<uint128>(0x4020100000000000),
(static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x8040200000000000), (static_cast<uint128>(0x00001) << 64) | static_cast<uint128>(0x0080400000000000), (static_cast<uint128>(0x00002) << 64) | static_cast<uint128>(0x0100800000000000), (static_cast<uint128>(0x00004) << 64) | static_cast<uint128>(0x0201000000000000), (static_cast<uint128>(0x00008) << 64) | static_cast<uint128>(0x0402000000000000), (static_cast<uint128>(0x00010) << 64) | static_cast<uint128>(0x0804000000000000), (static_cast<uint128>(0x00020) << 64) | static_cast<uint128>(0x1008000000000000), (static_cast<uint128>(0x00040) << 64) | static_cast<uint128>(0x2010000000000000), (static_cast<uint128>(0x00080) << 64) | static_cast<uint128>(0x4020000000000000),
(static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x8040000000000000), (static_cast<uint128>(0x00001) << 64) | static_cast<uint128>(0x0080000000000000), (static_cast<uint128>(0x00002) << 64) | static_cast<uint128>(0x0100000000000000), (static_cast<uint128>(0x00004) << 64) | static_cast<uint128>(0x0200000000000000), (static_cast<uint128>(0x00008) << 64) | static_cast<uint128>(0x0400000000000000), (static_cast<uint128>(0x00010) << 64) | static_cast<uint128>(0x0800000000000000), (static_cast<uint128>(0x00020) << 64) | static_cast<uint128>(0x1000000000000000), (static_cast<uint128>(0x00040) << 64) | static_cast<uint128>(0x2000000000000000), (static_cast<uint128>(0x00080) << 64) | static_cast<uint128>(0x4000000000000000),
(static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x8000000000000000), (static_cast<uint128>(0x00001) << 64) | static_cast<uint128>(0x0000000000000000), (static_cast<uint128>(0x00002) << 64) | static_cast<uint128>(0x0000000000000000), (static_cast<uint128>(0x00004) << 64) | static_cast<uint128>(0x0000000000000000), (static_cast<uint128>(0x00008) << 64) | static_cast<uint128>(0x0000000000000000), (static_cast<uint128>(0x00010) << 64) | static_cast<uint128>(0x0000000000000000), (static_cast<uint128>(0x00020) << 64) | static_cast<uint128>(0x0000000000000000), (static_cast<uint128>(0x00040) << 64) | static_cast<uint128>(0x0000000000000000), (static_cast<uint128>(0x00080) << 64) | static_cast<uint128>(0x0000000000000000),
(static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x0000000000000000), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x0000000000000000), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x0000000000000000), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x0000000000000000), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x0000000000000000), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x0000000000000000), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x0000000000000000), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x0000000000000000), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x0000000000000000),
(static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x0000000000000000), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x0000000000000000), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x0000000000000000), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x0000000000000000), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x0000000000000000), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x0000000000000000), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x0000000000000000), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x0000000000000000), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x0000000000000000),
        // clang-format on
};
template <>
inline const shogi::BitBoard
    shogi::Magic::premask_vertical[shogi::Config::num_squares]
    = {
        // clang-format off
(static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x8040201008040200), (static_cast<uint128>(0x00001) << 64) | static_cast<uint128>(0x0080402010080400), (static_cast<uint128>(0x00002) << 64) | static_cast<uint128>(0x0100804020100800), (static_cast<uint128>(0x00004) << 64) | static_cast<uint128>(0x0201008040201000), (static_cast<uint128>(0x00008) << 64) | static_cast<uint128>(0x0402010080402000), (static_cast<uint128>(0x00010) << 64) | static_cast<uint128>(0x0804020100804000), (static_cast<uint128>(0x00020) << 64) | static_cast<uint128>(0x1008040201008000), (static_cast<uint128>(0x00040) << 64) | static_cast<uint128>(0x2010080402010000), (static_cast<uint128>(0x00080) << 64) | static_cast<uint128>(0x4020100804020000),
(static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x8040201008040000), (static_cast<uint128>(0x00001) << 64) | static_cast<uint128>(0x0080402010080000), (static_cast<uint128>(0x00002) << 64) | static_cast<uint128>(0x0100804020100000), (static_cast<uint128>(0x00004) << 64) | static_cast<uint128>(0x0201008040200000), (static_cast<uint128>(0x00008) << 64) | static_cast<uint128>(0x0402010080400000), (static_cast<uint128>(0x00010) << 64) | static_cast<uint128>(0x0804020100800000), (static_cast<uint128>(0x00020) << 64) | static_cast<uint128>(0x1008040201000000), (static_cast<uint128>(0x00040) << 64) | static_cast<uint128>(0x2010080402000000), (static_cast<uint128>(0x00080) << 64) | static_cast<uint128>(0x4020100804000000),
(static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x8040201008000200), (static_cast<uint128>(0x00001) << 64) | static_cast<uint128>(0x0080402010000400), (static_cast<uint128>(0x00002) << 64) | static_cast<uint128>(0x0100804020000800), (static_cast<uint128>(0x00004) << 64) | static_cast<uint128>(0x0201008040001000), (static_cast<uint128>(0x00008) << 64) | static_cast<uint128>(0x0402010080002000), (static_cast<uint128>(0x00010) << 64) | static_cast<uint128>(0x0804020100004000), (static_cast<uint128>(0x00020) << 64) | static_cast<uint128>(0x1008040200008000), (static_cast<uint128>(0x00040) << 64) | static_cast<uint128>(0x2010080400010000), (static_cast<uint128>(0x00080) << 64) | static_cast<uint128>(0x4020100800020000),
(static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x8040201000040200), (static_cast<uint128>(0x00001) << 64) | static_cast<uint128>(0x0080402000080400), (static_cast<uint128>(0x00002) << 64) | static_cast<uint128>(0x0100804000100800), (static_cast<uint128>(0x00004) << 64) | static_cast<uint128>(0x0201008000201000), (static_cast<uint128>(0x00008) << 64) | static_cast<uint128>(0x0402010000402000), (static_cast<uint128>(0x00010) << 64) | static_cast<uint128>(0x0804020000804000), (static_cast<uint128>(0x00020) << 64) | static_cast<uint128>(0x1008040001008000), (static_cast<uint128>(0x00040) << 64) | static_cast<uint128>(0x2010080002010000), (static_cast<uint128>(0x00080) << 64) | static_cast<uint128>(0x4020100004020000),
(static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x8040200008040200), (static_cast<uint128>(0x00001) << 64) | static_cast<uint128>(0x0080400010080400), (static_cast<uint128>(0x00002) << 64) | static_cast<uint128>(0x0100800020100800), (static_cast<uint128>(0x00004) << 64) | static_cast<uint128>(0x0201000040201000), (static_cast<uint128>(0x00008) << 64) | static_cast<uint128>(0x0402000080402000), (static_cast<uint128>(0x00010) << 64) | static_cast<uint128>(0x0804000100804000), (static_cast<uint128>(0x00020) << 64) | static_cast<uint128>(0x1008000201008000), (static_cast<uint128>(0x00040) << 64) | static_cast<uint128>(0x2010000402010000), (static_cast<uint128>(0x00080) << 64) | static_cast<uint128>(0x4020000804020000),
(static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x8040001008040200), (static_cast<uint128>(0x00001) << 64) | static_cast<uint128>(0x0080002010080400), (static_cast<uint128>(0x00002) << 64) | static_cast<uint128>(0x0100004020100800), (static_cast<uint128>(0x00004) << 64) | static_cast<uint128>(0x0200008040201000), (static_cast<uint128>(0x00008) << 64) | static_cast<uint128>(0x0400010080402000), (static_cast<uint128>(0x00010) << 64) | static_cast<uint128>(0x0800020100804000), (static_cast<uint128>(0x00020) << 64) | static_cast<uint128>(0x1000040201008000), (static_cast<uint128>(0x00040) << 64) | static_cast<uint128>(0x2000080402010000), (static_cast<uint128>(0x00080) << 64) | static_cast<uint128>(0x4000100804020000),
(static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x8000201008040200), (static_cast<uint128>(0x00001) << 64) | static_cast<uint128>(0x0000402010080400), (static_cast<uint128>(0x00002) << 64) | static_cast<uint128>(0x0000804020100800), (static_cast<uint128>(0x00004) << 64) | static_cast<uint128>(0x0001008040201000), (static_cast<uint128>(0x00008) << 64) | static_cast<uint128>(0x0002010080402000), (static_cast<uint128>(0x00010) << 64) | static_cast<uint128>(0x0004020100804000), (static_cast<uint128>(0x00020) << 64) | static_cast<uint128>(0x0008040201008000), (static_cast<uint128>(0x00040) << 64) | static_cast<uint128>(0x0010080402010000), (static_cast<uint128>(0x00080) << 64) | static_cast<uint128>(0x0020100804020000),
(static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x0040201008040200), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x0080402010080400), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x0100804020100800), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x0201008040201000), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x0402010080402000), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x0804020100804000), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x1008040201008000), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x2010080402010000), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x4020100804020000),
(static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x8040201008040200), (static_cast<uint128>(0x00001) << 64) | static_cast<uint128>(0x0080402010080400), (static_cast<uint128>(0x00002) << 64) | static_cast<uint128>(0x0100804020100800), (static_cast<uint128>(0x00004) << 64) | static_cast<uint128>(0x0201008040201000), (static_cast<uint128>(0x00008) << 64) | static_cast<uint128>(0x0402010080402000), (static_cast<uint128>(0x00010) << 64) | static_cast<uint128>(0x0804020100804000), (static_cast<uint128>(0x00020) << 64) | static_cast<uint128>(0x1008040201008000), (static_cast<uint128>(0x00040) << 64) | static_cast<uint128>(0x2010080402010000), (static_cast<uint128>(0x00080) << 64) | static_cast<uint128>(0x4020100804020000),
        // clang-format on
};
template <>
inline const shogi::BitBoard
    shogi::Magic::premask_horizontal[shogi::Config::num_squares]
    = {
        // clang-format off
(static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x00000000000000fe), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x00000000000000fc), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x00000000000000fa), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x00000000000000f6), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x00000000000000ee), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x00000000000000de), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x00000000000000be), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x000000000000007e), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x00000000000000fe),
(static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x000000000001fc00), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x000000000001f800), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x000000000001f400), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x000000000001ec00), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x000000000001dc00), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x000000000001bc00), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x0000000000017c00), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x000000000000fc00), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x000000000001fc00),
(static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x0000000003f80000), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x0000000003f00000), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x0000000003e80000), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x0000000003d80000), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x0000000003b80000), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x0000000003780000), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x0000000002f80000), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x0000000001f80000), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x0000000003f80000),
(static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x00000007f0000000), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x00000007e0000000), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x00000007d0000000), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x00000007b0000000), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x0000000770000000), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x00000006f0000000), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x00000005f0000000), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x00000003f0000000), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x00000007f0000000),
(static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x00000fe000000000), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x00000fc000000000), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x00000fa000000000), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x00000f6000000000), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x00000ee000000000), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x00000de000000000), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x00000be000000000), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x000007e000000000), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x00000fe000000000),
(static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x001fc00000000000), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x001f800000000000), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x001f400000000000), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x001ec00000000000), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x001dc00000000000), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x001bc00000000000), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x0017c00000000000), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x000fc00000000000), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x001fc00000000000),
(static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x3f80000000000000), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x3f00000000000000), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x3e80000000000000), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x3d80000000000000), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x3b80000000000000), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x3780000000000000), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x2f80000000000000), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x1f80000000000000), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x3f80000000000000),
(static_cast<uint128>(0x0007f) << 64) | static_cast<uint128>(0x0000000000000000), (static_cast<uint128>(0x0007e) << 64) | static_cast<uint128>(0x0000000000000000), (static_cast<uint128>(0x0007d) << 64) | static_cast<uint128>(0x0000000000000000), (static_cast<uint128>(0x0007b) << 64) | static_cast<uint128>(0x0000000000000000), (static_cast<uint128>(0x00077) << 64) | static_cast<uint128>(0x0000000000000000), (static_cast<uint128>(0x0006f) << 64) | static_cast<uint128>(0x0000000000000000), (static_cast<uint128>(0x0005f) << 64) | static_cast<uint128>(0x0000000000000000), (static_cast<uint128>(0x0003f) << 64) | static_cast<uint128>(0x0000000000000000), (static_cast<uint128>(0x0007f) << 64) | static_cast<uint128>(0x0000000000000000),
(static_cast<uint128>(0x0fe00) << 64) | static_cast<uint128>(0x0000000000000000), (static_cast<uint128>(0x0fc00) << 64) | static_cast<uint128>(0x0000000000000000), (static_cast<uint128>(0x0fa00) << 64) | static_cast<uint128>(0x0000000000000000), (static_cast<uint128>(0x0f600) << 64) | static_cast<uint128>(0x0000000000000000), (static_cast<uint128>(0x0ee00) << 64) | static_cast<uint128>(0x0000000000000000), (static_cast<uint128>(0x0de00) << 64) | static_cast<uint128>(0x0000000000000000), (static_cast<uint128>(0x0be00) << 64) | static_cast<uint128>(0x0000000000000000), (static_cast<uint128>(0x07e00) << 64) | static_cast<uint128>(0x0000000000000000), (static_cast<uint128>(0x0fe00) << 64) | static_cast<uint128>(0x0000000000000000),
        // clang-format on
};
template <>
inline const shogi::BitBoard
    shogi::Magic::premask_diagonal[shogi::Config::num_squares]
    = {
        // clang-format off
(static_cast<uint128>(0x00040) << 64) | static_cast<uint128>(0x1004010040100400), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x2008020080200800), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x0010040100401400), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x0000080200882800), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x0000000411105000), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x000000202220a000), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x0000404040414000), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x0080808080808000), (static_cast<uint128>(0x00001) << 64) | static_cast<uint128>(0x0101010101010000),
(static_cast<uint128>(0x00020) << 64) | static_cast<uint128>(0x0802008020080000), (static_cast<uint128>(0x00040) << 64) | static_cast<uint128>(0x1004010040100000), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x2008020080280000), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x0010040110500000), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x0000082220a00000), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x0000404441400000), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x0080808082800000), (static_cast<uint128>(0x00001) << 64) | static_cast<uint128>(0x0101010101000000), (static_cast<uint128>(0x00002) << 64) | static_cast<uint128>(0x0202020202000000),
(static_cast<uint128>(0x00010) << 64) | static_cast<uint128>(0x0401004010000400), (static_cast<uint128>(0x00020) << 64) | static_cast<uint128>(0x0802008020000800), (static_cast<uint128>(0x00040) << 64) | static_cast<uint128>(0x1004010050001400), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x20080220a0002800), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x0010444140005000), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x008088828000a000), (static_cast<uint128>(0x00001) << 64) | static_cast<uint128>(0x0101010500014000), (static_cast<uint128>(0x00002) << 64) | static_cast<uint128>(0x0202020200008000), (static_cast<uint128>(0x00004) << 64) | static_cast<uint128>(0x0404040400010000),
(static_cast<uint128>(0x00008) << 64) | static_cast<uint128>(0x0200802000080800), (static_cast<uint128>(0x00010) << 64) | static_cast<uint128>(0x0401004000101000), (static_cast<uint128>(0x00020) << 64) | static_cast<uint128>(0x080200a000282000), (static_cast<uint128>(0x00040) << 64) | static_cast<uint128>(0x1004414000504400), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x2088828000a08800), (static_cast<uint128>(0x00001) << 64) | static_cast<uint128>(0x0111050001411000), (static_cast<uint128>(0x00002) << 64) | static_cast<uint128>(0x02020a0002802000), (static_cast<uint128>(0x00004) << 64) | static_cast<uint128>(0x0404040001004000), (static_cast<uint128>(0x00008) << 64) | static_cast<uint128>(0x0808080002008000),
(static_cast<uint128>(0x00004) << 64) | static_cast<uint128>(0x0100400010101000), (static_cast<uint128>(0x00008) << 64) | static_cast<uint128>(0x0200800020202000), (static_cast<uint128>(0x00010) << 64) | static_cast<uint128>(0x0401400050404000), (static_cast<uint128>(0x00020) << 64) | static_cast<uint128>(0x08828000a0888000), (static_cast<uint128>(0x00041) << 64) | static_cast<uint128>(0x1105000141110400), (static_cast<uint128>(0x00002) << 64) | static_cast<uint128>(0x220a000282200800), (static_cast<uint128>(0x00004) << 64) | static_cast<uint128>(0x0414000500401000), (static_cast<uint128>(0x00008) << 64) | static_cast<uint128>(0x0808000200802000), (static_cast<uint128>(0x00010) << 64) | static_cast<uint128>(0x1010000401004000),
(static_cast<uint128>(0x00002) << 64) | static_cast<uint128>(0x0080002020202000), (static_cast<uint128>(0x00004) << 64) | static_cast<uint128>(0x0100004040404000), (static_cast<uint128>(0x00008) << 64) | static_cast<uint128>(0x028000a080808000), (static_cast<uint128>(0x00011) << 64) | static_cast<uint128>(0x0500014111010000), (static_cast<uint128>(0x00022) << 64) | static_cast<uint128>(0x0a00028222080000), (static_cast<uint128>(0x00044) << 64) | static_cast<uint128>(0x1400050440100400), (static_cast<uint128>(0x00008) << 64) | static_cast<uint128>(0x28000a0080200800), (static_cast<uint128>(0x00010) << 64) | static_cast<uint128>(0x1000040100401000), (static_cast<uint128>(0x00020) << 64) | static_cast<uint128>(0x2000080200802000),
(static_cast<uint128>(0x00001) << 64) | static_cast<uint128>(0x0000404040404000), (static_cast<uint128>(0x00002) << 64) | static_cast<uint128>(0x0000808080808000), (static_cast<uint128>(0x00005) << 64) | static_cast<uint128>(0x0001410101010000), (static_cast<uint128>(0x0000a) << 64) | static_cast<uint128>(0x0002822202000000), (static_cast<uint128>(0x00014) << 64) | static_cast<uint128>(0x0005044410000000), (static_cast<uint128>(0x00028) << 64) | static_cast<uint128>(0x000a088020080000), (static_cast<uint128>(0x00050) << 64) | static_cast<uint128>(0x0014010040100400), (static_cast<uint128>(0x00020) << 64) | static_cast<uint128>(0x0008020080200800), (static_cast<uint128>(0x00040) << 64) | static_cast<uint128>(0x0010040100401000),
(static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x0080808080808000), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x0101010101010000), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x0282020202000000), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x0504440400000000), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x0a08882000000000), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x1411004010000000), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x2802008020080000), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x1004010040100400), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x2008020080200800),
(static_cast<uint128>(0x00001) << 64) | static_cast<uint128>(0x0101010101010000), (static_cast<uint128>(0x00002) << 64) | static_cast<uint128>(0x0202020202000000), (static_cast<uint128>(0x00005) << 64) | static_cast<uint128>(0x0404040400000000), (static_cast<uint128>(0x0000a) << 64) | static_cast<uint128>(0x0888080000000000), (static_cast<uint128>(0x00014) << 64) | static_cast<uint128>(0x1110400000000000), (static_cast<uint128>(0x00028) << 64) | static_cast<uint128>(0x2200802000000000), (static_cast<uint128>(0x00050) << 64) | static_cast<uint128>(0x0401004010000000), (static_cast<uint128>(0x00020) << 64) | static_cast<uint128>(0x0802008020080000), (static_cast<uint128>(0x00040) << 64) | static_cast<uint128>(0x1004010040100400),
        // clang-format on
};
template <>
inline const std::uint32_t
    shogi::Magic::magic_number_north[shogi::Config::num_squares]
    = {
        // clang-format off
0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
0x00420061, 0x58ec9440, 0x54891828, 0x004201e2, 0x41880103, 0x20420926, 0x079ac001, 0x55325004, 0xc0583021,
0x81152105, 0x08c08058, 0x80829089, 0x86102410, 0x0400c004, 0xb0058500, 0xd4a10830, 0x20618040, 0x0a01823e,
0x4294916a, 0x0428500c, 0x08027802, 0x023a40aa, 0x252085d1, 0xc114b004, 0x08400534, 0x20b98c08, 0x0091b608,
0x10a344b1, 0x0aa18028, 0x1c20121e, 0x2010080b, 0xc5400e23, 0x01090422, 0x05912038, 0x30210a78, 0x02c84504,
0x1804ca39, 0x0257190c, 0x40841232, 0x40029101, 0x02019884, 0x44040e23, 0x64446724, 0x1c200c1c, 0x48048030,
0x68278c11, 0x00004194, 0x4b000b11, 0x12109099, 0x42a48489, 0x81420848, 0x0a805164, 0x76e0b805, 0x1280427c,
0x40619c0a, 0x0a280624, 0x2f0c0203, 0x930d402d, 0x00800a24, 0x0101a451, 0x01011094, 0x04022032, 0x00112f48,
        // clang-format on
};
template <>
inline const std::uint32_t
    shogi::Magic::magic_number_south[shogi::Config::num_squares]
    = {
        // clang-format off
0x3c9008c1, 0x0d812c41, 0xaa00c4c9, 0x84206045, 0x8c265245, 0x43e92c01, 0xa043119a, 0x5c0c88a9, 0x840c0132,
0x7168ab88, 0x20180184, 0x00e41002, 0x26108c19, 0x4a889294, 0x8641c402, 0x00a6f20c, 0x040e018b, 0x20900401,
0x85f294c4, 0x2216b545, 0x1221a00e, 0x04094102, 0x28042050, 0x18090112, 0x01034301, 0x0088200d, 0x30822ec2,
0x20100042, 0x04d16241, 0x0202e94c, 0x4ad654ac, 0x13201201, 0x82420d18, 0x0628401a, 0x580a4606, 0x4a920509,
0x3028061a, 0x914608b8, 0x98124086, 0x42007211, 0x10118605, 0x00209401, 0x0645e50d, 0x1488c406, 0x10439590,
0x01908c90, 0x03404050, 0x24000208, 0x25a309e2, 0x50024c10, 0xe64a4a46, 0xb2416634, 0x9025b00e, 0x08600480,
0x54906e18, 0x844d840a, 0x201cc051, 0x2dc0064a, 0xa8312008, 0x82020a8c, 0x88980402, 0x10528135, 0x4a54a002,
0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
        // clang-format on
};
template <>
inline const std::uint32_t
    shogi::Magic::magic_number_vertical[shogi::Config::num_squares]
    = {
        // clang-format off
0x0a001321, 0x09152211, 0x62a00262, 0x06009c02, 0x8c082802, 0x680d080c, 0x10068005, 0x500d0ea1, 0x0820098a,
0xa6204434, 0xa038246e, 0xcb849422, 0x641014a3, 0x14801c1a, 0x44096e41, 0x01814074, 0x84979211, 0x508c0108,
0x80d10229, 0x4028a051, 0x02011801, 0x21961f43, 0x5e022391, 0xc2282562, 0x40242201, 0x68809c02, 0xe240084f,
0x21cd2804, 0x4a10e804, 0xe8509494, 0x88264251, 0xc5101431, 0x61889892, 0x11008594, 0x90036015, 0x04a3f004,
0x40781341, 0x989e0901, 0xa10144aa, 0x0028122b, 0x04220716, 0x24722801, 0x40046c2a, 0x222c0381, 0x4283120a,
0x2692c805, 0x05580281, 0x08318202, 0x008a8d01, 0x80484050, 0x9114a021, 0x540892a9, 0x091040e1, 0xa0072111,
0x00ac009c, 0x10481803, 0x208488ca, 0x406dab0a, 0x64460404, 0x118a0011, 0x24811468, 0x0204b185, 0x4418200a,
0x222b0026, 0x10048e01, 0x421450e5, 0x04020501, 0x1464852b, 0x0c184803, 0x0c6080db, 0x00101001, 0x51210da6,
0x58488e23, 0xac121967, 0x98440b45, 0x16826001, 0x1310058c, 0x881a0344, 0x5e812702, 0x58086685, 0x410090b0,
        // clang-format on
};
template <>
inline const std::uint32_t
    shogi::Magic::magic_number_horizontal[shogi::Config::num_squares]
    = {
        // clang-format off
0x31197412, 0x96884e6c, 0x02821526, 0x81122150, 0x094502d0, 0x38901100, 0x82119520, 0x8415500b, 0x04589110,
0x903c4905, 0x0124228c, 0x118c604a, 0x000042b3, 0x22647882, 0x4290b0b0, 0x002020c2, 0x81944e0c, 0x181311d2,
0x00a10253, 0xc3215010, 0xb0100403, 0x004100c8, 0x78000081, 0x249c0321, 0x89028012, 0x502400d0, 0x82311008,
0x8114642d, 0x04881021, 0x14221011, 0x5148000b, 0x83805056, 0x03212356, 0xa481a509, 0x28ee0016, 0x08180801,
0x4a426104, 0x21d20028, 0xc402288c, 0x08b01090, 0x04040800, 0x223e6928, 0x884e8c2d, 0x880274e8, 0x0101c410,
0x84011544, 0xa41019a0, 0x870229e0, 0x14100801, 0xb042c064, 0x00890285, 0x821e0028, 0x851808c0, 0x40414061,
0x20240082, 0x04ce022d, 0x00060d0e, 0x48041202, 0x09080015, 0x02a40001, 0xc0104841, 0x63d0c404, 0x5944390a,
0x23610050, 0x01c000aa, 0x04225482, 0x03a1005c, 0x61012646, 0x088204b0, 0x64410464, 0x40470908, 0x30349220,
0xc4ae8392, 0x5026c080, 0x00112280, 0x8004b042, 0x480c9050, 0x39d22c0e, 0x080a8608, 0x14000278, 0x21a3460b,
        // clang-format on
};
template <>
inline const std::uint32_t
    shogi::Magic::magic_number_diagonal[shogi::Config::num_squares]
    = {
        // clang-format off
0x00010042, 0x3a2207c9, 0x19a23080, 0x080e3b55, 0x44402a04, 0x6041ca44, 0x44886016, 0x99430550, 0xc2320820,
0x26888041, 0xa9080191, 0x4aa00019, 0x6752a03a, 0x592a0c84, 0x025161d3, 0x4824c018, 0x01883083, 0x00840421,
0x04015091, 0x6c821891, 0x50948002, 0x090a9009, 0x11403003, 0x09188110, 0x11203040, 0x4280a1c2, 0xf450180c,
0x4a0d4c15, 0x4040814f, 0x09820901, 0x0204480a, 0x401b0201, 0x20a01003, 0x22a0310e, 0x10220052, 0x40852401,
0xc4800b28, 0x2100041c, 0x4022c103, 0x02c8900a, 0x506a1002, 0x0650c901, 0x04c8818d, 0x1b4900a1, 0xdc804403,
0x9d86a311, 0x35142005, 0x4040a022, 0x500d0e04, 0x42948084, 0x3420081a, 0x11100859, 0x88270249, 0x412440a1,
0x1480a215, 0x14001210, 0xa0010474, 0x003009d4, 0x43106a08, 0x22443403, 0xc81b82c6, 0x60183120, 0x30012003,
0x80548243, 0x71424220, 0x0062a210, 0x04640261, 0x2004020a, 0x4a280349, 0xc8028889, 0x8902094a, 0x12040cd6,
0x020a2403, 0x41390011, 0x1b82552c, 0x01792049, 0x20014052, 0xd0988a06, 0x01414228, 0x04c83099, 0x52359c02,
        // clang-format on
};
template <>
inline shogi::BitBoard
    shogi::Magic::attack_table_north[shogi::Config::num_squares]
                                    [shogi::Config::magic_table_size]
    = {};
template <>
inline shogi::BitBoard
    shogi::Magic::attack_table_south[shogi::Config::num_squares]
                                    [shogi::Config::magic_table_size]
    = {};
template <>
inline shogi::BitBoard
    shogi::Magic::attack_table_vertical[shogi::Config::num_squares]
                                       [shogi::Config::magic_table_size]
    = {};
template <>
inline shogi::BitBoard
    shogi::Magic::attack_table_horizontal[shogi::Config::num_squares]
                                         [shogi::Config::magic_table_size]
    = {};
template <>
inline shogi::BitBoard
    shogi::Magic::attack_table_diagonal[shogi::Config::num_squares]
                                       [shogi::Config::magic_table_size]
    = {};

template <>
inline bool shogi::Board::is_square_attacked_by_ranging_pieces(
    const ColorEnum& by_side, const Square& sq, const Square& skip) const
{
    using namespace shogi;
    const BitBoardType occ_full = get_occupied().clear(skip);
    {
        const auto attack_inverted
            = (by_side == BLACK) ? shogi::Magic::get_south_attack(sq, occ_full)
                                 : shogi::Magic::get_north_attack(sq, occ_full);
        const auto occ_offence = get_occupied<KY>(by_side);
        if ((attack_inverted & occ_offence).any())
            return true;
    }
    {
        const auto attack_inverted
            = shogi::Magic::get_diagonal_attack(sq, occ_full);
        const auto occ_offence = get_occupied<KA, UM>(by_side);
        if ((attack_inverted & occ_offence).any())
            return true;
    }
    {
        const auto attack_inverted
            = shogi::Magic::get_adjacent_attack(sq, occ_full);
        const auto occ_offence = get_occupied<HI, RY>(by_side);
        if ((attack_inverted & occ_offence).any())
            return true;
    }
    return false;
}

template <>
inline bool shogi::Board::is_square_attacked(
    const ColorEnum& by_side, const Square& sq, const Square& skip) const
{
    assert(sq != SQ_NA);
    using namespace shogi;
    return is_square_attacked_by<FU>(by_side, sq)
           || is_square_attacked_by<KE>(by_side, sq)
           || is_square_attacked_by<GI>(by_side, sq)
           || is_square_attacked_by<KI, TO, NY, NK, NG>(by_side, sq)
           || is_square_attacked_by<OU, UM, RY>(by_side, sq)
           || is_square_attacked_by_ranging_pieces(by_side, sq, skip);
}
template <>
inline shogi::BitBoard
shogi::Board::get_occupied_by_ranging(const ColorEnum& c) const
{
    using namespace shogi;
    return get_occupied<KY, KA, HI, UM, RY>(c);
}

template <>
inline shogi::Move shogi::NonKingBoardMoveGenerator::random_select()
{
    using namespace shogi;
    const auto src_fgke = m_board.get_occupied<FU, GI, KE>(m_turn);
    const auto src_kkhi = m_board.get_occupied<KY, KA, HI>(m_turn);
    const auto src_gold = m_board.get_occupied<KI, TO, NY, NK, NG>(m_turn);
    const auto src_umry = m_board.get_occupied<UM, RY>(m_turn);
    const auto num_fgke = static_cast<float>(src_fgke.hamming_weight());
    const auto num_kkhi = static_cast<float>(src_kkhi.hamming_weight());
    const auto num_gold = static_cast<float>(src_gold.hamming_weight());
    const auto num_umry = static_cast<float>(src_umry.hamming_weight());
    const auto num_src = num_fgke + num_kkhi + num_gold + num_umry;
    float r = dist01(random_engine);
    const auto fraction_fgke = num_fgke / num_src;
    if (r < fraction_fgke) {
        auto iter_fgke = NonKingBoardMoveGenerator(m_state, src_fgke, m_pinned);
        if (!iter_fgke.is_end())
            return iter_fgke.random_select_by_iterating_all();
        auto iter_kkhi = NonKingBoardMoveGenerator(m_state, src_kkhi, m_pinned);
        if (!iter_kkhi.is_end())
            return iter_kkhi.random_select_by_iterating_all();
        auto iter_gold
            = NoPromoMoveGenerator<Config>(m_state, src_gold, m_pinned);
        if (!iter_gold.is_end())
            return iter_gold.random_select();
        auto iter_umry
            = NoPromoMoveGenerator<Config>(m_state, src_umry, m_pinned);
        return iter_umry.random_select();
    }
    r -= fraction_fgke;
    const auto fraction_kkhi = num_kkhi / num_src;
    if (r < fraction_kkhi) {
        auto iter_kkhi = NonKingBoardMoveGenerator(m_state, src_kkhi, m_pinned);
        if (!iter_kkhi.is_end())
            return iter_kkhi.random_select_by_iterating_all();
        auto iter_gold
            = NoPromoMoveGenerator<Config>(m_state, src_gold, m_pinned);
        if (!iter_gold.is_end())
            return iter_gold.random_select();
        auto iter_umry
            = NoPromoMoveGenerator<Config>(m_state, src_umry, m_pinned);
        if (!iter_umry.is_end())
            return iter_umry.random_select();
        auto iter_fgke = NonKingBoardMoveGenerator(m_state, src_fgke, m_pinned);
        return iter_fgke.random_select_by_iterating_all();
    }
    r -= fraction_kkhi;
    const auto fraction_gold = num_gold / num_src;
    if (r < fraction_gold) {
        auto iter_gold
            = NoPromoMoveGenerator<Config>(m_state, src_gold, m_pinned);
        if (!iter_gold.is_end())
            return iter_gold.random_select();
        auto iter_umry
            = NoPromoMoveGenerator<Config>(m_state, src_umry, m_pinned);
        if (!iter_umry.is_end())
            return iter_umry.random_select();
        auto iter_fgke = NonKingBoardMoveGenerator(m_state, src_fgke, m_pinned);
        if (!iter_fgke.is_end())
            return iter_fgke.random_select_by_iterating_all();
        auto iter_kkhi = NonKingBoardMoveGenerator(m_state, src_kkhi, m_pinned);
        return iter_kkhi.random_select_by_iterating_all();
    }
    {
        auto iter_umry
            = NoPromoMoveGenerator<Config>(m_state, src_umry, m_pinned);
        if (!iter_umry.is_end())
            return iter_umry.random_select();
        auto iter_fgke = NonKingBoardMoveGenerator(m_state, src_fgke, m_pinned);
        if (!iter_fgke.is_end())
            return iter_fgke.random_select_by_iterating_all();
        auto iter_kkhi = NonKingBoardMoveGenerator(m_state, src_kkhi, m_pinned);
        if (!iter_kkhi.is_end())
            return iter_kkhi.random_select_by_iterating_all();
        auto iter_gold
            = NoPromoMoveGenerator<Config>(m_state, src_gold, m_pinned);
        return iter_gold.random_select();
    }
}

} // namespace vshogi

#endif // VSHOGI_VARIANTS_SHOGI_HPP
