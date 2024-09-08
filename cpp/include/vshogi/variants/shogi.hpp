#ifndef VSHOGI_VARIANTS_SHOGI_HPP
#define VSHOGI_VARIANTS_SHOGI_HPP

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
    static constexpr uint num_files = 9; // 1, 2, 3, 4, 5, 6, 7, 8, 9
    static constexpr uint num_ranks = 9; // A, B, C, D, E, F, G, H, I
    static constexpr uint num_promotion_ranks = 3;
    static constexpr uint num_dir = 12; //!< NW, N, NE, W, E, SW, S, SE, SSW, SSE, NNW, NNE
    static constexpr uint num_dir_dl = 10; //!< NW, N, NE, W, E, SW, S, SE, SSW, SSE
    static constexpr uint log2_magic_table_size = 7;
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
inline const shogi::BitBoard
    shogi::BitBoard::square_to_bitboard_array[shogi::Config::num_squares + 1]
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
    shogi::Magic::premask_nw_se[shogi::Config::num_squares]
    = {
        // clang-format off
(static_cast<uint128>(0x00040) << 64) | static_cast<uint128>(0x1004010040100400), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x2008020080200800), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x0010040100401000), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x0000080200802000), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x0000000401004000), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x0000000002008000), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x0000000000010000), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x0000000000000000), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x0000000000000000),
(static_cast<uint128>(0x00020) << 64) | static_cast<uint128>(0x0802008020080000), (static_cast<uint128>(0x00040) << 64) | static_cast<uint128>(0x1004010040100000), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x2008020080200000), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x0010040100400000), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x0000080200800000), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x0000000401000000), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x0000000002000000), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x0000000000000000), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x0000000000000000),
(static_cast<uint128>(0x00010) << 64) | static_cast<uint128>(0x0401004010000000), (static_cast<uint128>(0x00020) << 64) | static_cast<uint128>(0x0802008020000000), (static_cast<uint128>(0x00040) << 64) | static_cast<uint128>(0x1004010040000400), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x2008020080000800), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x0010040100001000), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x0000080200002000), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x0000000400004000), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x0000000000008000), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x0000000000010000),
(static_cast<uint128>(0x00008) << 64) | static_cast<uint128>(0x0200802000000000), (static_cast<uint128>(0x00010) << 64) | static_cast<uint128>(0x0401004000000000), (static_cast<uint128>(0x00020) << 64) | static_cast<uint128>(0x0802008000080000), (static_cast<uint128>(0x00040) << 64) | static_cast<uint128>(0x1004010000100400), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x2008020000200800), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x0010040000401000), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x0000080000802000), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x0000000001004000), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x0000000002008000),
(static_cast<uint128>(0x00004) << 64) | static_cast<uint128>(0x0100400000000000), (static_cast<uint128>(0x00008) << 64) | static_cast<uint128>(0x0200800000000000), (static_cast<uint128>(0x00010) << 64) | static_cast<uint128>(0x0401000010000000), (static_cast<uint128>(0x00020) << 64) | static_cast<uint128>(0x0802000020080000), (static_cast<uint128>(0x00040) << 64) | static_cast<uint128>(0x1004000040100400), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x2008000080200800), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x0010000100401000), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x0000000200802000), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x0000000401004000),
(static_cast<uint128>(0x00002) << 64) | static_cast<uint128>(0x0080000000000000), (static_cast<uint128>(0x00004) << 64) | static_cast<uint128>(0x0100000000000000), (static_cast<uint128>(0x00008) << 64) | static_cast<uint128>(0x0200002000000000), (static_cast<uint128>(0x00010) << 64) | static_cast<uint128>(0x0400004010000000), (static_cast<uint128>(0x00020) << 64) | static_cast<uint128>(0x0800008020080000), (static_cast<uint128>(0x00040) << 64) | static_cast<uint128>(0x1000010040100400), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x2000020080200800), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x0000040100401000), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x0000080200802000),
(static_cast<uint128>(0x00001) << 64) | static_cast<uint128>(0x0000000000000000), (static_cast<uint128>(0x00002) << 64) | static_cast<uint128>(0x0000000000000000), (static_cast<uint128>(0x00004) << 64) | static_cast<uint128>(0x0000400000000000), (static_cast<uint128>(0x00008) << 64) | static_cast<uint128>(0x0000802000000000), (static_cast<uint128>(0x00010) << 64) | static_cast<uint128>(0x0001004010000000), (static_cast<uint128>(0x00020) << 64) | static_cast<uint128>(0x0002008020080000), (static_cast<uint128>(0x00040) << 64) | static_cast<uint128>(0x0004010040100400), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x0008020080200800), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x0010040100401000),
(static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x0000000000000000), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x0000000000000000), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x0080000000000000), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x0100400000000000), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x0200802000000000), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x0401004010000000), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x0802008020080000), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x1004010040100400), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x2008020080200800),
(static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x0000000000000000), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x0000000000000000), (static_cast<uint128>(0x00001) << 64) | static_cast<uint128>(0x0000000000000000), (static_cast<uint128>(0x00002) << 64) | static_cast<uint128>(0x0080000000000000), (static_cast<uint128>(0x00004) << 64) | static_cast<uint128>(0x0100400000000000), (static_cast<uint128>(0x00008) << 64) | static_cast<uint128>(0x0200802000000000), (static_cast<uint128>(0x00010) << 64) | static_cast<uint128>(0x0401004010000000), (static_cast<uint128>(0x00020) << 64) | static_cast<uint128>(0x0802008020080000), (static_cast<uint128>(0x00040) << 64) | static_cast<uint128>(0x1004010040100400),
        // clang-format on
};
template <>
inline const shogi::BitBoard
    shogi::Magic::premask_sw_ne[shogi::Config::num_squares]
    = {
        // clang-format off
(static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x0000000000000000), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x0000000000000000), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x0000000000000400), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x0000000000080800), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x0000000010101000), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x0000002020202000), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x0000404040404000), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x0080808080808000), (static_cast<uint128>(0x00001) << 64) | static_cast<uint128>(0x0101010101010000),
(static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x0000000000000000), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x0000000000000000), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x0000000000080000), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x0000000010100000), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x0000002020200000), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x0000404040400000), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x0080808080800000), (static_cast<uint128>(0x00001) << 64) | static_cast<uint128>(0x0101010101000000), (static_cast<uint128>(0x00002) << 64) | static_cast<uint128>(0x0202020202000000),
(static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x0000000000000400), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x0000000000000800), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x0000000010001000), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x0000002020002000), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x0000404040004000), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x0080808080008000), (static_cast<uint128>(0x00001) << 64) | static_cast<uint128>(0x0101010100010000), (static_cast<uint128>(0x00002) << 64) | static_cast<uint128>(0x0202020200000000), (static_cast<uint128>(0x00004) << 64) | static_cast<uint128>(0x0404040400000000),
(static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x0000000000080800), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x0000000000101000), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x0000002000202000), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x0000404000404000), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x0080808000808000), (static_cast<uint128>(0x00001) << 64) | static_cast<uint128>(0x0101010001010000), (static_cast<uint128>(0x00002) << 64) | static_cast<uint128>(0x0202020002000000), (static_cast<uint128>(0x00004) << 64) | static_cast<uint128>(0x0404040000000000), (static_cast<uint128>(0x00008) << 64) | static_cast<uint128>(0x0808080000000000),
(static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x0000000010101000), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x0000000020202000), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x0000400040404000), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x0080800080808000), (static_cast<uint128>(0x00001) << 64) | static_cast<uint128>(0x0101000101010000), (static_cast<uint128>(0x00002) << 64) | static_cast<uint128>(0x0202000202000000), (static_cast<uint128>(0x00004) << 64) | static_cast<uint128>(0x0404000400000000), (static_cast<uint128>(0x00008) << 64) | static_cast<uint128>(0x0808000000000000), (static_cast<uint128>(0x00010) << 64) | static_cast<uint128>(0x1010000000000000),
(static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x0000002020202000), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x0000004040404000), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x0080008080808000), (static_cast<uint128>(0x00001) << 64) | static_cast<uint128>(0x0100010101010000), (static_cast<uint128>(0x00002) << 64) | static_cast<uint128>(0x0200020202000000), (static_cast<uint128>(0x00004) << 64) | static_cast<uint128>(0x0400040400000000), (static_cast<uint128>(0x00008) << 64) | static_cast<uint128>(0x0800080000000000), (static_cast<uint128>(0x00010) << 64) | static_cast<uint128>(0x1000000000000000), (static_cast<uint128>(0x00020) << 64) | static_cast<uint128>(0x2000000000000000),
(static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x0000404040404000), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x0000808080808000), (static_cast<uint128>(0x00001) << 64) | static_cast<uint128>(0x0001010101010000), (static_cast<uint128>(0x00002) << 64) | static_cast<uint128>(0x0002020202000000), (static_cast<uint128>(0x00004) << 64) | static_cast<uint128>(0x0004040400000000), (static_cast<uint128>(0x00008) << 64) | static_cast<uint128>(0x0008080000000000), (static_cast<uint128>(0x00010) << 64) | static_cast<uint128>(0x0010000000000000), (static_cast<uint128>(0x00020) << 64) | static_cast<uint128>(0x0000000000000000), (static_cast<uint128>(0x00040) << 64) | static_cast<uint128>(0x0000000000000000),
(static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x0080808080808000), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x0101010101010000), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x0202020202000000), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x0404040400000000), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x0808080000000000), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x1010000000000000), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x2000000000000000), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x0000000000000000), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x0000000000000000),
(static_cast<uint128>(0x00001) << 64) | static_cast<uint128>(0x0101010101010000), (static_cast<uint128>(0x00002) << 64) | static_cast<uint128>(0x0202020202000000), (static_cast<uint128>(0x00004) << 64) | static_cast<uint128>(0x0404040400000000), (static_cast<uint128>(0x00008) << 64) | static_cast<uint128>(0x0808080000000000), (static_cast<uint128>(0x00010) << 64) | static_cast<uint128>(0x1010000000000000), (static_cast<uint128>(0x00020) << 64) | static_cast<uint128>(0x2000000000000000), (static_cast<uint128>(0x00040) << 64) | static_cast<uint128>(0x0000000000000000), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x0000000000000000), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x0000000000000000),
        // clang-format on
};
template <>
inline const std::uint32_t
    shogi::Magic::magic_number_north[shogi::Config::num_squares]
    = {
        // clang-format off
0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
0x00420061, 0x58ec9440, 0x54891828, 0x004201e2, 0x20420926, 0x079ac001, 0x55325004, 0xc0583021, 0x81152105,
0x80829089, 0x86102410, 0xb0058500, 0xd4a10830, 0x20618040, 0x0a01823e, 0x4294916a, 0x0428500c, 0x04b04028,
0x8c048830, 0x001809c8, 0x08027802, 0x023a40aa, 0x252085d1, 0xc114b004, 0x08400534, 0x20b98c08, 0x0091b608,
0x10a344b1, 0x1c20121e, 0xec414504, 0x01090422, 0x05912038, 0x30210a78, 0x02c84504, 0x1804ca39, 0x0257190c,
0x40841232, 0x8093160c, 0x68278c11, 0x12109099, 0x42a48489, 0x81420848, 0x88a24240, 0xaa209881, 0x1280427c,
0x84206045, 0x8c265245, 0x20900401, 0x18090112, 0x01034301, 0x0645e50d, 0xa6204434, 0x90036015, 0x0a166423,
0x02280810, 0x6c821891, 0x0204480a, 0x00820a65, 0x22c22208, 0x4010590f, 0x04901822, 0x02182302, 0x08021052,
        // clang-format on
};
template <>
inline const std::uint32_t
    shogi::Magic::magic_number_south[shogi::Config::num_squares]
    = {
        // clang-format off
0x12c21902, 0x850a41c8, 0x64226405, 0x09204405, 0x50109130, 0x982050c1, 0xbe144802, 0xa6d08409, 0x04401064,
0x04504149, 0x00802a08, 0x50c0351f, 0x11060415, 0x76102009, 0x40b638c1, 0x0826287c, 0x31915036, 0x8b0828a0,
0x0c46028a, 0x03000ca2, 0xc15d8101, 0x03240872, 0x28220690, 0x491c0848, 0x46a3a089, 0x0710912a, 0xd1067cc0,
0x24410142, 0x71c82241, 0x4e120401, 0x0c2af071, 0x4b695510, 0x5082d991, 0x000d8e02, 0xc0085281, 0x0d41b008,
0x6f4a51ac, 0x20854104, 0x508ce001, 0x244c1004, 0x28e02802, 0x011a380d, 0x32341114, 0xc9229449, 0x454404e4,
0x22214100, 0x0d200448, 0x41240648, 0x0020118c, 0x9041815a, 0x320700a6, 0x325408a2, 0x00084229, 0xc7231100,
0xeb004010, 0x20034008, 0x080244a0, 0x008189c2, 0xb4011254, 0x66447010, 0x80204d40, 0x40049060, 0x4e1c8000,
0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
        // clang-format on
};
template <>
inline const std::uint32_t
    shogi::Magic::magic_number_vertical[shogi::Config::num_squares]
    = {
        // clang-format off
0x42100c10, 0xc2841044, 0x84206045, 0x59402366, 0xc5101431, 0x8b08610a, 0x26042404, 0x9a248025, 0x22288113,
0x4c002085, 0x544940c8, 0x17041512, 0xc0200302, 0x00838111, 0x496402a4, 0x01020032, 0x12259a02, 0x04410524,
0x79204170, 0xa2040aac, 0x005022b2, 0x06931021, 0xc8c03886, 0x4010590f, 0xd2480c87, 0x0650c901, 0x04c8818d,
0x0a05a284, 0x42948084, 0x01041408, 0x7c08e30c, 0x412440a1, 0x8902094a, 0x12040cd6, 0xd0988a06, 0xc404d232,
0x20214123, 0x35a11232, 0x1385c184, 0xb8026483, 0x05831130, 0x894a810c, 0x90f004a4, 0x45948401, 0x06042a08,
0x140440a8, 0x08140268, 0x467095a3, 0x480b8185, 0x15401028, 0x0501303e, 0x110162a6, 0x900a0289, 0x140423c1,
0x24099006, 0x03151461, 0x0040950c, 0x89902422, 0xe0416230, 0x04308a90, 0x008914d9, 0x0c4408d0, 0x04c02268,
0xb8411230, 0x08e85184, 0x50501c05, 0xe02c2982, 0x20048090, 0x722420c5, 0x6404802d, 0x1488a081, 0x54545060,
0x0e9010b0, 0x08c0f006, 0x64226405, 0x09204405, 0x50109130, 0x982050c1, 0xbe144802, 0xa6d08409, 0x04401064,
        // clang-format on
};
template <>
inline const std::uint32_t
    shogi::Magic::magic_number_horizontal[shogi::Config::num_squares]
    = {
        // clang-format off
0x090180c4, 0x80901010, 0x909b6124, 0x22600780, 0x30131a90, 0x0b013478, 0x10808809, 0x02025470, 0x0500041b,
0x64868088, 0x9a4a9324, 0x92010814, 0xf8310800, 0x45088012, 0x21648008, 0x40381005, 0x0400b200, 0x46418005,
0x59e31081, 0x00a40042, 0x22244040, 0x2041d144, 0x2d88a58e, 0x18290049, 0x0ac12081, 0xa0b00902, 0x002b0204,
0x24410142, 0x71c82241, 0x4e120401, 0x5082d991, 0x508ce001, 0x28e02802, 0x011a380d, 0xc9229449, 0x320700a6,
0x0808100a, 0x55f4401b, 0x03280068, 0xe4081247, 0x01810132, 0x17104021, 0x60089208, 0x20c29840, 0x02040540,
0x00868100, 0x04620224, 0x98605e00, 0x00c51810, 0x01004088, 0x04829c02, 0x20090219, 0x8080205b, 0x84017c00,
0xa002c0d2, 0x8c41a441, 0x10a50133, 0x13040096, 0x0820c029, 0x04b00002, 0xc0242436, 0x90c3402a, 0x4684c54a,
0x0480003b, 0xa0992048, 0xc4130810, 0x4c882688, 0x48100226, 0x408c2200, 0x62020200, 0xaa054222, 0x08814a2c,
0x0a61c010, 0x008b5016, 0x20c04020, 0x003a0406, 0x00012024, 0xa20a401c, 0x18a04000, 0x40902345, 0x48c81020,
        // clang-format on
};
template <>
inline const std::uint32_t
    shogi::Magic::magic_number_nw_se[shogi::Config::num_squares]
    = {
        // clang-format off
0x0b2a8081, 0x3003011e, 0x018a4c10, 0x5a894262, 0xa2572805, 0x04953002, 0x84004064, 0xffffffff, 0xffffffff,
0xc8331506, 0x6b620241, 0x0c4b8acd, 0xc616d101, 0x34808020, 0x1000489e, 0xa1ebd4a6, 0xffffffff, 0xffffffff,
0xa2908608, 0x808a1181, 0x0b0622f2, 0xd2098219, 0xa4089310, 0x0800a078, 0x42d203e0, 0x01a08cb8, 0x18064362,
0xda330f22, 0x000a2069, 0xe0c21303, 0x0c08a04b, 0x30e51033, 0x00204204, 0xc0884021, 0x06e61508, 0x50a44415,
0x722288d0, 0x0a2c0420, 0x42402928, 0x02080a36, 0x24489e06, 0x21908222, 0x12090020, 0xa140e006, 0x0168228e,
0x8a0e2405, 0x430980eb, 0x24a048d2, 0xca188571, 0xc80da20c, 0x600398c3, 0xc6820016, 0x08105880, 0x4f32a091,
0x26900809, 0xe2041322, 0x01502022, 0x820832a0, 0x9041fcbe, 0xc645ef8b, 0x01a89ea1, 0x88940222, 0xc8006162,
0xffffffff, 0xffffffff, 0xc842790c, 0x702886c0, 0x1a340482, 0x11448588, 0x2425c881, 0x40040059, 0x06044029,
0xffffffff, 0xffffffff, 0x20424800, 0x44248819, 0xa17aa081, 0x06604002, 0x20840a01, 0x80240204, 0x6a823e4a,
        // clang-format on
};
template <>
inline const std::uint32_t
    shogi::Magic::magic_number_sw_ne[shogi::Config::num_squares]
    = {
        // clang-format off
0xffffffff, 0xffffffff, 0x81170204, 0x80018280, 0x30011a02, 0x401081d3, 0x18a30786, 0x00841231, 0x8d144010,
0xffffffff, 0xffffffff, 0x010c4480, 0x81004442, 0x50a08082, 0x91182489, 0x90ca2884, 0x32200490, 0x040908e0,
0x14bc1002, 0x00044908, 0x02808094, 0x0230b081, 0x4a824402, 0x81209830, 0x04866038, 0x3ba01a11, 0x2c661401,
0xb26521c9, 0xe7450108, 0x30211880, 0x202702c8, 0xd0423004, 0x15156081, 0x06882411, 0x06346041, 0x83790444,
0xc0c18354, 0x2b201157, 0xdaa0a08b, 0x53042088, 0x46a80840, 0x60b51e20, 0x08020502, 0x14480188, 0x286b010c,
0x52106e09, 0x41a032cd, 0x05300428, 0x8220428d, 0x0ac10059, 0x62901c92, 0x16845341, 0x2490a41a, 0x0c048212,
0x33214ea2, 0x46881d14, 0xa2564880, 0x22241120, 0x00850912, 0x42cc2020, 0x40803c11, 0x23380401, 0x2002a004,
0x12048058, 0x5c09a5e3, 0x100a5082, 0x087a008c, 0x40884047, 0x2546890a, 0x8701200a, 0xffffffff, 0xffffffff,
0x47886006, 0x029a280d, 0x0110014c, 0x22099078, 0x12223012, 0x0b400802, 0x08195601, 0xffffffff, 0xffffffff,
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
    shogi::Magic::attack_table_nw_se[shogi::Config::num_squares]
                                    [shogi::Config::magic_table_size]
    = {};
template <>
inline shogi::BitBoard
    shogi::Magic::attack_table_sw_ne[shogi::Config::num_squares]
                                    [shogi::Config::magic_table_size]
    = {};

} // namespace vshogi

#endif // VSHOGI_VARIANTS_SHOGI_HPP
