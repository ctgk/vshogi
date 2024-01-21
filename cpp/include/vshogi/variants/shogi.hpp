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
constexpr unsigned int num_piece_types = 14; // FU, KY, KE, GI, KI, KA, HI, OU, TO, NY, NK, NG, UM, RY
constexpr unsigned int num_stand_piece_types = 7; // FU, KY, KE, GI, KI, KA, HI
constexpr unsigned int promotion_bit = 3; // ____ _*___
constexpr unsigned int num_files = 9; // 1, 2, 3, 4, 5, 6, 7, 8, 9
constexpr unsigned int num_ranks = 9; // A, B, C, D, E, F, G, H, I
constexpr unsigned int num_directions = 12; // NNW, NNE, NW, N, NE, W, E, SW, S, SE, SSW, SSE
constexpr unsigned int num_directions_dlshogi = 10; // NW, N, NE, W, E, SW, S, SE, SSW, SSE
constexpr unsigned int num_non_ranging_board_piece = 9; // B_FU, W_FU, B_KE, W_KE, B_GI, W_GI, B_KI, W_KI, OU
constexpr unsigned int num_attack_types = 15; // B_FU, W_FU, B_KY, W_KY, B_KE, W_KE, B_GI, W_GI, B_KI, W_KI, KA, HI, OU, UM, RY
constexpr unsigned int max_stand_piece_count = 18;
constexpr unsigned int max_acceptable_repetition = 3;
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
enum SquareEnum
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
enum RankEnum
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
enum FileEnum
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
    num_files,
    num_ranks,
    SquareEnum,
    FileEnum,
    RankEnum,
    num_directions,
    num_directions_dlshogi,
    num_non_ranging_board_piece>;

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
using Stand = vshogi::Stand<Pieces, std::uint32_t>;

using BlackWhiteStands = vshogi::BlackWhiteStands<Stand>;

/**
 * @brief 16-bit integer representing a move in shogi game.
 * @details
 *       ________ _*******       Destination square (81 possibilities)
 *       ________ *_______       Promotion flag (2 possibilities)
 *       _******* ________       Source square or piece (88 possibilities = 81 + 7)
 * (MSB) xxxxxxxx xxxxxxxx (LSB)
 */
using Move = vshogi::Move<Squares, Pieces, std::uint16_t>;

using BitBoard = vshogi::
    BitBoard<__uint128_t, Squares, BoardPieceTypeEnum, num_attack_types>;

using Board = vshogi::Board<Squares, Pieces, BitBoard>;

using State
    = vshogi::State<Board, BlackWhiteStands, Move, max_stand_piece_count>;

using Game = vshogi::Game<State, max_acceptable_repetition>;

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
    switch (p) {
    case shogi::FU:
        return 'p';
    case shogi::KY:
        return 'l';
    case shogi::KE:
        return 'n';
    case shogi::GI:
        return 's';
    case shogi::KA:
        return 'b';
    case shogi::HI:
        return 'r';
    case shogi::KI:
        return 'g';
    case shogi::OU:
        return 'k';
    default:
        return '\0';
    }
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
inline int
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
template <>
inline const vshogi::shogi::SquareEnum*
vshogi::shogi::Squares::get_non_ranging_attacks_by(
    const vshogi::shogi::BoardPieceTypeEnum& p,
    const vshogi::shogi::SquareEnum& location)
{
    switch (p) {
    case vshogi::shogi::B_FU:
        return non_ranging_attacks_array[0][location];
    case vshogi::shogi::B_KE:
        return non_ranging_attacks_array[1][location];
    case vshogi::shogi::B_GI:
        return non_ranging_attacks_array[2][location];
    case vshogi::shogi::B_KI:
    case vshogi::shogi::B_TO:
    case vshogi::shogi::B_NY:
    case vshogi::shogi::B_NK:
    case vshogi::shogi::B_NG:
        return non_ranging_attacks_array[3][location];
    case vshogi::shogi::W_FU:
        return non_ranging_attacks_array[4][location];
    case vshogi::shogi::W_KE:
        return non_ranging_attacks_array[5][location];
    case vshogi::shogi::W_GI:
        return non_ranging_attacks_array[6][location];
    case vshogi::shogi::W_KI:
    case vshogi::shogi::W_TO:
    case vshogi::shogi::W_NY:
    case vshogi::shogi::W_NK:
    case vshogi::shogi::W_NG:
        return non_ranging_attacks_array[7][location];
    case vshogi::shogi::B_OU:
    case vshogi::shogi::W_OU:
        return non_ranging_attacks_array[8][location];
    default:
        return nullptr;
    }
}

template <>
inline void vshogi::shogi::Squares::init_non_ranging_attacks_array()
{
    constexpr int size = sizeof(non_ranging_attacks_array)
                         / sizeof(non_ranging_attacks_array[0][0][0]);
    std::fill_n(
        &non_ranging_attacks_array[0][0][0], size, vshogi::shogi::SQ_NA);

    // B_FU, B_KE, B_GI, B_KI, W_FU, W_KE, W_GI, W_KI, OU
    constexpr DirectionEnum piece_to_direction[9][9] = {
        // clang-format off
        {DIR_N, DIR_NA}, // B_FU
        {DIR_NNW, DIR_NNE, DIR_NA}, // B_KE
        {DIR_NW, DIR_N, DIR_NE, DIR_SW, DIR_SE, DIR_NA}, // B_GI
        {DIR_NW, DIR_N, DIR_NE, DIR_W, DIR_E, DIR_S, DIR_NA}, // B_KI
        {DIR_S, DIR_NA}, // W_FU
        {DIR_SSW, DIR_SSE, DIR_NA}, // W_KE
        {DIR_NW, DIR_NE, DIR_SW, DIR_S, DIR_SE, DIR_NA}, // W_GI
        {DIR_N, DIR_W, DIR_E, DIR_SW, DIR_S, DIR_SE, DIR_NA}, // W_KI
        {DIR_NW, DIR_N, DIR_NE, DIR_W, DIR_E, DIR_SW, DIR_S, DIR_SE, DIR_NA}, // OU
        // clang-format on
    };
    for (int i = 0; i < 9; ++i) {
        for (auto&& sq : square_array) {
            int index = 0;
            for (auto&& dir : piece_to_direction[i]) {
                if (dir == DIR_NA)
                    break;
                const auto dst = shift(sq, dir);
                if (dst == SQ_NA)
                    continue;
                non_ranging_attacks_array[i][sq][index++] = dst;
            }
        }
    }
}

template <>
inline const int shogi::Stand::shift_bits[] = {0, 6, 10, 14, 18, 21, 24};

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
constexpr int shogi::Move::source_shift()
{
    return 8;
}
template <>
constexpr int shogi::Move::promote_shift()
{
    return 7;
}
template <>
constexpr std::uint16_t shogi::Move::destination_mask()
{
    return 0x007f;
}
template <>
constexpr std::uint16_t shogi::Move::source_mask()
{
    return 0x7f00;
}
template <>
constexpr std::uint16_t shogi::Move::promote_mask()
{
    return 0x0080;
}

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
