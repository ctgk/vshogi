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
 */
enum SquareEnum : uint
{
    // clang-format off
    SQ_9A=72, SQ_8A=63, SQ_7A=54, SQ_6A=45, SQ_5A=36, SQ_4A=27, SQ_3A=18, SQ_2A= 9, SQ_1A=0,
    SQ_9B=73, SQ_8B=64, SQ_7B=55, SQ_6B=46, SQ_5B=37, SQ_4B=28, SQ_3B=19, SQ_2B=10, SQ_1B=1,
    SQ_9C=74, SQ_8C=65, SQ_7C=56, SQ_6C=47, SQ_5C=38, SQ_4C=29, SQ_3C=20, SQ_2C=11, SQ_1C=2,
    SQ_9D=75, SQ_8D=66, SQ_7D=57, SQ_6D=48, SQ_5D=39, SQ_4D=30, SQ_3D=21, SQ_2D=12, SQ_1D=3,
    SQ_9E=76, SQ_8E=67, SQ_7E=58, SQ_6E=49, SQ_5E=40, SQ_4E=31, SQ_3E=22, SQ_2E=13, SQ_1E=4,
    SQ_9F=77, SQ_8F=68, SQ_7F=59, SQ_6F=50, SQ_5F=41, SQ_4F=32, SQ_3F=23, SQ_2F=14, SQ_1F=5,
    SQ_9G=78, SQ_8G=69, SQ_7G=60, SQ_6G=51, SQ_5G=42, SQ_4G=33, SQ_3G=24, SQ_2G=15, SQ_1G=6,
    SQ_9H=79, SQ_8H=70, SQ_7H=61, SQ_6H=52, SQ_5H=43, SQ_4H=34, SQ_3H=25, SQ_2H=16, SQ_1H=7,
    SQ_9I=80, SQ_8I=71, SQ_7I=62, SQ_6I=53, SQ_5I=44, SQ_4I=35, SQ_3I=26, SQ_2I=17, SQ_1I=8,
    // clang-format on
    SQ_NA = 81,
    NUM_SQ = 81,

    // clang-format off
    A9=SQ_9A, A8=SQ_8A, A7=SQ_7A, A6=SQ_6A, A5=SQ_5A, A4=SQ_4A, A3=SQ_3A, A2=SQ_2A, A1=SQ_1A,
    B9=SQ_9B, B8=SQ_8B, B7=SQ_7B, B6=SQ_6B, B5=SQ_5B, B4=SQ_4B, B3=SQ_3B, B2=SQ_2B, B1=SQ_1B,
    C9=SQ_9C, C8=SQ_8C, C7=SQ_7C, C6=SQ_6C, C5=SQ_5C, C4=SQ_4C, C3=SQ_3C, C2=SQ_2C, C1=SQ_1C,
    D9=SQ_9D, D8=SQ_8D, D7=SQ_7D, D6=SQ_6D, D5=SQ_5D, D4=SQ_4D, D3=SQ_3D, D2=SQ_2D, D1=SQ_1D,
    E9=SQ_9E, E8=SQ_8E, E7=SQ_7E, E6=SQ_6E, E5=SQ_5E, E4=SQ_4E, E3=SQ_3E, E2=SQ_2E, E1=SQ_1E,
    F9=SQ_9F, F8=SQ_8F, F7=SQ_7F, F6=SQ_6F, F5=SQ_5F, F4=SQ_4F, F3=SQ_3F, F2=SQ_2F, F1=SQ_1F,
    G9=SQ_9G, G8=SQ_8G, G7=SQ_7G, G6=SQ_6G, G5=SQ_5G, G4=SQ_4G, G3=SQ_3G, G2=SQ_2G, G1=SQ_1G,
    H9=SQ_9H, H8=SQ_8H, H7=SQ_7H, H6=SQ_6H, H5=SQ_5H, H4=SQ_4H, H3=SQ_3H, H2=SQ_2H, H1=SQ_1H,
    I9=SQ_9I, I8=SQ_8I, I7=SQ_7I, I6=SQ_6I, I5=SQ_5I, I4=SQ_4I, I3=SQ_3I, I2=SQ_2I, I1=SQ_1I,
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
    FILE1 = 0,
    FILE2,
    FILE3,
    FILE4,
    FILE5,
    FILE6,
    FILE7,
    FILE8,
    FILE9,
};

struct Parameters
{
    // clang-format off
    static constexpr std::array<FullPieceTypes, 15u> piece_types = {PT_FU, PT_KY, PT_KE, PT_GI, PT_KA, PT_HI, PT_KI, PT_OU, PT_TO, PT_NY, PT_NK, PT_NG, PT_UM, PT_RY, PT_NA};
    static constexpr uint num_stand_piece_types = 7; // FU, KY, KE, GI, KA, HI, KI
    static constexpr uint sum_piece_value = 2 * (60 * 17 + 115 + 120);
    static constexpr uint num_files = 9; // 1, 2, 3, 4, 5, 6, 7, 8, 9
    static constexpr uint num_ranks = 9; // A, B, C, D, E, F, G, H, I
    static constexpr uint num_promotion_ranks = 3;
    static constexpr uint num_dir = 12; //!< NW, N, NE, W, E, SW, S, SE, SSW, SSE, NNW, NNE
    static constexpr uint log2_magic_table_size = 14u + 1u; // 1 for buffer
    static constexpr uint max_stand_piece_count = 18;
    static constexpr uint max_stand_sfen_length = 26; // "10p2l2n2sbr2g2P2L2N2SBR2G "
    static constexpr uint max_acceptable_repetitions = 3;
    static constexpr uint num_init_piece_each = 20;
    static constexpr uint initial_points = 27;
    using BaseTypeBitBoard = uint128;
    using BaseTypeStand = std::uint32_t; // _____*** _**_**_* **_***_* **_***** (KI, HI, KA, GI, KE, KY, FU)
    using PieceType = PieceTypeEnum;
    using ColoredPiece = ColoredPieceEnum;
    using Square = SquareEnum;
    using File = FileEnum;
    using Rank = RankEnum;
    Parameters() = delete;
    // clang-format on
};

using Config = vshogi::Configuration<Parameters>;
using Pieces = vshogi::Pieces<Parameters>;
using Squares = vshogi::Squares<Parameters>;
using Move = vshogi::Move<Parameters>;
using BitBoard = vshogi::BitBoard<Parameters>;
using Magic = vshogi::Magic<Parameters>;
using Board = vshogi::Board<Parameters>;
using Stand = vshogi::Stand<Parameters>;
using BlackWhiteStands = vshogi::BlackWhiteStands<Parameters>;
using State = vshogi::State<Parameters>;
using LegalMoveGenerator = vshogi::LegalMoveGenerator<Parameters>;
using Game = vshogi::Game<Parameters>;
static_assert(FU == Config::FU);
static_assert(OU == Config::OU);
static_assert(NA == Config::NA);
static_assert(VOID == Config::VOID);

// clang-format off
constexpr BitBoard bb_na = BitBoard();
constexpr BitBoard bb_1a = BitBoard::from_square<SQ_1A>();
constexpr BitBoard bb_1b = BitBoard::from_square<SQ_1B>();
constexpr BitBoard bb_1c = BitBoard::from_square<SQ_1C>();
constexpr BitBoard bb_1d = BitBoard::from_square<SQ_1D>();
constexpr BitBoard bb_1e = BitBoard::from_square<SQ_1E>();
constexpr BitBoard bb_1f = BitBoard::from_square<SQ_1F>();
constexpr BitBoard bb_1g = BitBoard::from_square<SQ_1G>();
constexpr BitBoard bb_1h = BitBoard::from_square<SQ_1H>();
constexpr BitBoard bb_1i = BitBoard::from_square<SQ_1I>();
constexpr BitBoard bb_2a = BitBoard::from_square<SQ_2A>();
constexpr BitBoard bb_2b = BitBoard::from_square<SQ_2B>();
constexpr BitBoard bb_2c = BitBoard::from_square<SQ_2C>();
constexpr BitBoard bb_2d = BitBoard::from_square<SQ_2D>();
constexpr BitBoard bb_2e = BitBoard::from_square<SQ_2E>();
constexpr BitBoard bb_2f = BitBoard::from_square<SQ_2F>();
constexpr BitBoard bb_2g = BitBoard::from_square<SQ_2G>();
constexpr BitBoard bb_2h = BitBoard::from_square<SQ_2H>();
constexpr BitBoard bb_2i = BitBoard::from_square<SQ_2I>();
constexpr BitBoard bb_3a = BitBoard::from_square<SQ_3A>();
constexpr BitBoard bb_3b = BitBoard::from_square<SQ_3B>();
constexpr BitBoard bb_3c = BitBoard::from_square<SQ_3C>();
constexpr BitBoard bb_3d = BitBoard::from_square<SQ_3D>();
constexpr BitBoard bb_3e = BitBoard::from_square<SQ_3E>();
constexpr BitBoard bb_3f = BitBoard::from_square<SQ_3F>();
constexpr BitBoard bb_3g = BitBoard::from_square<SQ_3G>();
constexpr BitBoard bb_3h = BitBoard::from_square<SQ_3H>();
constexpr BitBoard bb_3i = BitBoard::from_square<SQ_3I>();
constexpr BitBoard bb_4a = BitBoard::from_square<SQ_4A>();
constexpr BitBoard bb_4b = BitBoard::from_square<SQ_4B>();
constexpr BitBoard bb_4c = BitBoard::from_square<SQ_4C>();
constexpr BitBoard bb_4d = BitBoard::from_square<SQ_4D>();
constexpr BitBoard bb_4e = BitBoard::from_square<SQ_4E>();
constexpr BitBoard bb_4f = BitBoard::from_square<SQ_4F>();
constexpr BitBoard bb_4g = BitBoard::from_square<SQ_4G>();
constexpr BitBoard bb_4h = BitBoard::from_square<SQ_4H>();
constexpr BitBoard bb_4i = BitBoard::from_square<SQ_4I>();
constexpr BitBoard bb_5a = BitBoard::from_square<SQ_5A>();
constexpr BitBoard bb_5b = BitBoard::from_square<SQ_5B>();
constexpr BitBoard bb_5c = BitBoard::from_square<SQ_5C>();
constexpr BitBoard bb_5d = BitBoard::from_square<SQ_5D>();
constexpr BitBoard bb_5e = BitBoard::from_square<SQ_5E>();
constexpr BitBoard bb_5f = BitBoard::from_square<SQ_5F>();
constexpr BitBoard bb_5g = BitBoard::from_square<SQ_5G>();
constexpr BitBoard bb_5h = BitBoard::from_square<SQ_5H>();
constexpr BitBoard bb_5i = BitBoard::from_square<SQ_5I>();
constexpr BitBoard bb_6a = BitBoard::from_square<SQ_6A>();
constexpr BitBoard bb_6b = BitBoard::from_square<SQ_6B>();
constexpr BitBoard bb_6c = BitBoard::from_square<SQ_6C>();
constexpr BitBoard bb_6d = BitBoard::from_square<SQ_6D>();
constexpr BitBoard bb_6e = BitBoard::from_square<SQ_6E>();
constexpr BitBoard bb_6f = BitBoard::from_square<SQ_6F>();
constexpr BitBoard bb_6g = BitBoard::from_square<SQ_6G>();
constexpr BitBoard bb_6h = BitBoard::from_square<SQ_6H>();
constexpr BitBoard bb_6i = BitBoard::from_square<SQ_6I>();
constexpr BitBoard bb_7a = BitBoard::from_square<SQ_7A>();
constexpr BitBoard bb_7b = BitBoard::from_square<SQ_7B>();
constexpr BitBoard bb_7c = BitBoard::from_square<SQ_7C>();
constexpr BitBoard bb_7d = BitBoard::from_square<SQ_7D>();
constexpr BitBoard bb_7e = BitBoard::from_square<SQ_7E>();
constexpr BitBoard bb_7f = BitBoard::from_square<SQ_7F>();
constexpr BitBoard bb_7g = BitBoard::from_square<SQ_7G>();
constexpr BitBoard bb_7h = BitBoard::from_square<SQ_7H>();
constexpr BitBoard bb_7i = BitBoard::from_square<SQ_7I>();
constexpr BitBoard bb_8a = BitBoard::from_square<SQ_8A>();
constexpr BitBoard bb_8b = BitBoard::from_square<SQ_8B>();
constexpr BitBoard bb_8c = BitBoard::from_square<SQ_8C>();
constexpr BitBoard bb_8d = BitBoard::from_square<SQ_8D>();
constexpr BitBoard bb_8e = BitBoard::from_square<SQ_8E>();
constexpr BitBoard bb_8f = BitBoard::from_square<SQ_8F>();
constexpr BitBoard bb_8g = BitBoard::from_square<SQ_8G>();
constexpr BitBoard bb_8h = BitBoard::from_square<SQ_8H>();
constexpr BitBoard bb_8i = BitBoard::from_square<SQ_8I>();
constexpr BitBoard bb_9a = BitBoard::from_square<SQ_9A>();
constexpr BitBoard bb_9b = BitBoard::from_square<SQ_9B>();
constexpr BitBoard bb_9c = BitBoard::from_square<SQ_9C>();
constexpr BitBoard bb_9d = BitBoard::from_square<SQ_9D>();
constexpr BitBoard bb_9e = BitBoard::from_square<SQ_9E>();
constexpr BitBoard bb_9f = BitBoard::from_square<SQ_9F>();
constexpr BitBoard bb_9g = BitBoard::from_square<SQ_9G>();
constexpr BitBoard bb_9h = BitBoard::from_square<SQ_9H>();
constexpr BitBoard bb_9i = BitBoard::from_square<SQ_9I>();
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
    shogi::BlackWhiteStands::pieces_in_sfen_order[]
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
inline const uint128 shogi::Magic::premask_north[shogi::Config::num_squares] = {
    // clang-format off
(static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x0000000000000000), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x0000000000000000), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x0000000000000002), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x0000000000000006), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x000000000000000e), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x000000000000001e), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x000000000000003e), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x000000000000007e), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x00000000000000fe),
(static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x0000000000000000), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x0000000000000000), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x0000000000000400), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x0000000000000c00), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x0000000000001c00), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x0000000000003c00), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x0000000000007c00), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x000000000000fc00), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x000000000001fc00),
(static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x0000000000000000), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x0000000000000000), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x0000000000080000), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x0000000000180000), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x0000000000380000), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x0000000000780000), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x0000000000f80000), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x0000000001f80000), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x0000000003f80000),
(static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x0000000000000000), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x0000000000000000), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x0000000010000000), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x0000000030000000), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x0000000070000000), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x00000000f0000000), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x00000001f0000000), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x00000003f0000000), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x00000007f0000000),
(static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x0000000000000000), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x0000000000000000), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x0000002000000000), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x0000006000000000), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x000000e000000000), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x000001e000000000), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x000003e000000000), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x000007e000000000), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x00000fe000000000),
(static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x0000000000000000), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x0000000000000000), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x0000400000000000), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x0000c00000000000), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x0001c00000000000), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x0003c00000000000), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x0007c00000000000), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x000fc00000000000), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x001fc00000000000),
(static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x0000000000000000), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x0000000000000000), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x0080000000000000), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x0180000000000000), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x0380000000000000), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x0780000000000000), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x0f80000000000000), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x1f80000000000000), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x3f80000000000000),
(static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x0000000000000000), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x0000000000000000), (static_cast<uint128>(0x00001) << 64) | static_cast<uint128>(0x0000000000000000), (static_cast<uint128>(0x00003) << 64) | static_cast<uint128>(0x0000000000000000), (static_cast<uint128>(0x00007) << 64) | static_cast<uint128>(0x0000000000000000), (static_cast<uint128>(0x0000f) << 64) | static_cast<uint128>(0x0000000000000000), (static_cast<uint128>(0x0001f) << 64) | static_cast<uint128>(0x0000000000000000), (static_cast<uint128>(0x0003f) << 64) | static_cast<uint128>(0x0000000000000000), (static_cast<uint128>(0x0007f) << 64) | static_cast<uint128>(0x0000000000000000),
(static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x0000000000000000), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x0000000000000000), (static_cast<uint128>(0x00200) << 64) | static_cast<uint128>(0x0000000000000000), (static_cast<uint128>(0x00600) << 64) | static_cast<uint128>(0x0000000000000000), (static_cast<uint128>(0x00e00) << 64) | static_cast<uint128>(0x0000000000000000), (static_cast<uint128>(0x01e00) << 64) | static_cast<uint128>(0x0000000000000000), (static_cast<uint128>(0x03e00) << 64) | static_cast<uint128>(0x0000000000000000), (static_cast<uint128>(0x07e00) << 64) | static_cast<uint128>(0x0000000000000000), (static_cast<uint128>(0x0fe00) << 64) | static_cast<uint128>(0x0000000000000000),
    // clang-format on
};
template <>
inline const uint128 shogi::Magic::premask_south[shogi::Config::num_squares] = {
    // clang-format off
(static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x00000000000000fe), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x00000000000000fc), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x00000000000000f8), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x00000000000000f0), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x00000000000000e0), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x00000000000000c0), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x0000000000000080), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x0000000000000000), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x0000000000000000),
(static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x000000000001fc00), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x000000000001f800), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x000000000001f000), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x000000000001e000), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x000000000001c000), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x0000000000018000), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x0000000000010000), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x0000000000000000), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x0000000000000000),
(static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x0000000003f80000), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x0000000003f00000), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x0000000003e00000), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x0000000003c00000), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x0000000003800000), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x0000000003000000), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x0000000002000000), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x0000000000000000), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x0000000000000000),
(static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x00000007f0000000), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x00000007e0000000), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x00000007c0000000), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x0000000780000000), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x0000000700000000), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x0000000600000000), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x0000000400000000), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x0000000000000000), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x0000000000000000),
(static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x00000fe000000000), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x00000fc000000000), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x00000f8000000000), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x00000f0000000000), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x00000e0000000000), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x00000c0000000000), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x0000080000000000), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x0000000000000000), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x0000000000000000),
(static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x001fc00000000000), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x001f800000000000), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x001f000000000000), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x001e000000000000), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x001c000000000000), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x0018000000000000), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x0010000000000000), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x0000000000000000), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x0000000000000000),
(static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x3f80000000000000), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x3f00000000000000), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x3e00000000000000), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x3c00000000000000), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x3800000000000000), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x3000000000000000), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x2000000000000000), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x0000000000000000), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x0000000000000000),
(static_cast<uint128>(0x0007f) << 64) | static_cast<uint128>(0x0000000000000000), (static_cast<uint128>(0x0007e) << 64) | static_cast<uint128>(0x0000000000000000), (static_cast<uint128>(0x0007c) << 64) | static_cast<uint128>(0x0000000000000000), (static_cast<uint128>(0x00078) << 64) | static_cast<uint128>(0x0000000000000000), (static_cast<uint128>(0x00070) << 64) | static_cast<uint128>(0x0000000000000000), (static_cast<uint128>(0x00060) << 64) | static_cast<uint128>(0x0000000000000000), (static_cast<uint128>(0x00040) << 64) | static_cast<uint128>(0x0000000000000000), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x0000000000000000), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x0000000000000000),
(static_cast<uint128>(0x0fe00) << 64) | static_cast<uint128>(0x0000000000000000), (static_cast<uint128>(0x0fc00) << 64) | static_cast<uint128>(0x0000000000000000), (static_cast<uint128>(0x0f800) << 64) | static_cast<uint128>(0x0000000000000000), (static_cast<uint128>(0x0f000) << 64) | static_cast<uint128>(0x0000000000000000), (static_cast<uint128>(0x0e000) << 64) | static_cast<uint128>(0x0000000000000000), (static_cast<uint128>(0x0c000) << 64) | static_cast<uint128>(0x0000000000000000), (static_cast<uint128>(0x08000) << 64) | static_cast<uint128>(0x0000000000000000), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x0000000000000000), (static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x0000000000000000),
    // clang-format on
};
template <>
inline const uint128 shogi::Magic::premask_adjacent[shogi::Config::num_squares]
    = {
        // clang-format off
(static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x80402010080402fe), (static_cast<uint128>(0x00001) << 64) | static_cast<uint128>(0x00804020100804fc), (static_cast<uint128>(0x00002) << 64) | static_cast<uint128>(0x01008040201008fa), (static_cast<uint128>(0x00004) << 64) | static_cast<uint128>(0x02010080402010f6), (static_cast<uint128>(0x00008) << 64) | static_cast<uint128>(0x04020100804020ee), (static_cast<uint128>(0x00010) << 64) | static_cast<uint128>(0x08040201008040de), (static_cast<uint128>(0x00020) << 64) | static_cast<uint128>(0x10080402010080be), (static_cast<uint128>(0x00040) << 64) | static_cast<uint128>(0x201008040201007e), (static_cast<uint128>(0x00080) << 64) | static_cast<uint128>(0x40201008040200fe),
(static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x804020100805fc00), (static_cast<uint128>(0x00001) << 64) | static_cast<uint128>(0x008040201009f800), (static_cast<uint128>(0x00002) << 64) | static_cast<uint128>(0x010080402011f400), (static_cast<uint128>(0x00004) << 64) | static_cast<uint128>(0x020100804021ec00), (static_cast<uint128>(0x00008) << 64) | static_cast<uint128>(0x040201008041dc00), (static_cast<uint128>(0x00010) << 64) | static_cast<uint128>(0x080402010081bc00), (static_cast<uint128>(0x00020) << 64) | static_cast<uint128>(0x1008040201017c00), (static_cast<uint128>(0x00040) << 64) | static_cast<uint128>(0x201008040200fc00), (static_cast<uint128>(0x00080) << 64) | static_cast<uint128>(0x402010080401fc00),
(static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x804020100bf80200), (static_cast<uint128>(0x00001) << 64) | static_cast<uint128>(0x0080402013f00400), (static_cast<uint128>(0x00002) << 64) | static_cast<uint128>(0x0100804023e80800), (static_cast<uint128>(0x00004) << 64) | static_cast<uint128>(0x0201008043d81000), (static_cast<uint128>(0x00008) << 64) | static_cast<uint128>(0x0402010083b82000), (static_cast<uint128>(0x00010) << 64) | static_cast<uint128>(0x0804020103784000), (static_cast<uint128>(0x00020) << 64) | static_cast<uint128>(0x1008040202f88000), (static_cast<uint128>(0x00040) << 64) | static_cast<uint128>(0x2010080401f90000), (static_cast<uint128>(0x00080) << 64) | static_cast<uint128>(0x4020100803fa0000),
(static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x80402017f0040200), (static_cast<uint128>(0x00001) << 64) | static_cast<uint128>(0x00804027e0080400), (static_cast<uint128>(0x00002) << 64) | static_cast<uint128>(0x01008047d0100800), (static_cast<uint128>(0x00004) << 64) | static_cast<uint128>(0x02010087b0201000), (static_cast<uint128>(0x00008) << 64) | static_cast<uint128>(0x0402010770402000), (static_cast<uint128>(0x00010) << 64) | static_cast<uint128>(0x08040206f0804000), (static_cast<uint128>(0x00020) << 64) | static_cast<uint128>(0x10080405f1008000), (static_cast<uint128>(0x00040) << 64) | static_cast<uint128>(0x20100803f2010000), (static_cast<uint128>(0x00080) << 64) | static_cast<uint128>(0x40201007f4020000),
(static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x80402fe008040200), (static_cast<uint128>(0x00001) << 64) | static_cast<uint128>(0x00804fc010080400), (static_cast<uint128>(0x00002) << 64) | static_cast<uint128>(0x01008fa020100800), (static_cast<uint128>(0x00004) << 64) | static_cast<uint128>(0x02010f6040201000), (static_cast<uint128>(0x00008) << 64) | static_cast<uint128>(0x04020ee080402000), (static_cast<uint128>(0x00010) << 64) | static_cast<uint128>(0x08040de100804000), (static_cast<uint128>(0x00020) << 64) | static_cast<uint128>(0x10080be201008000), (static_cast<uint128>(0x00040) << 64) | static_cast<uint128>(0x201007e402010000), (static_cast<uint128>(0x00080) << 64) | static_cast<uint128>(0x40200fe804020000),
(static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0x805fc01008040200), (static_cast<uint128>(0x00001) << 64) | static_cast<uint128>(0x009f802010080400), (static_cast<uint128>(0x00002) << 64) | static_cast<uint128>(0x011f404020100800), (static_cast<uint128>(0x00004) << 64) | static_cast<uint128>(0x021ec08040201000), (static_cast<uint128>(0x00008) << 64) | static_cast<uint128>(0x041dc10080402000), (static_cast<uint128>(0x00010) << 64) | static_cast<uint128>(0x081bc20100804000), (static_cast<uint128>(0x00020) << 64) | static_cast<uint128>(0x1017c40201008000), (static_cast<uint128>(0x00040) << 64) | static_cast<uint128>(0x200fc80402010000), (static_cast<uint128>(0x00080) << 64) | static_cast<uint128>(0x401fd00804020000),
(static_cast<uint128>(0x00000) << 64) | static_cast<uint128>(0xbf80201008040200), (static_cast<uint128>(0x00001) << 64) | static_cast<uint128>(0x3f00402010080400), (static_cast<uint128>(0x00002) << 64) | static_cast<uint128>(0x3e80804020100800), (static_cast<uint128>(0x00004) << 64) | static_cast<uint128>(0x3d81008040201000), (static_cast<uint128>(0x00008) << 64) | static_cast<uint128>(0x3b82010080402000), (static_cast<uint128>(0x00010) << 64) | static_cast<uint128>(0x3784020100804000), (static_cast<uint128>(0x00020) << 64) | static_cast<uint128>(0x2f88040201008000), (static_cast<uint128>(0x00040) << 64) | static_cast<uint128>(0x1f90080402010000), (static_cast<uint128>(0x00080) << 64) | static_cast<uint128>(0x3fa0100804020000),
(static_cast<uint128>(0x0007f) << 64) | static_cast<uint128>(0x0040201008040200), (static_cast<uint128>(0x0007e) << 64) | static_cast<uint128>(0x0080402010080400), (static_cast<uint128>(0x0007d) << 64) | static_cast<uint128>(0x0100804020100800), (static_cast<uint128>(0x0007b) << 64) | static_cast<uint128>(0x0201008040201000), (static_cast<uint128>(0x00077) << 64) | static_cast<uint128>(0x0402010080402000), (static_cast<uint128>(0x0006f) << 64) | static_cast<uint128>(0x0804020100804000), (static_cast<uint128>(0x0005f) << 64) | static_cast<uint128>(0x1008040201008000), (static_cast<uint128>(0x0003f) << 64) | static_cast<uint128>(0x2010080402010000), (static_cast<uint128>(0x0007f) << 64) | static_cast<uint128>(0x4020100804020000),
(static_cast<uint128>(0x0fe00) << 64) | static_cast<uint128>(0x8040201008040200), (static_cast<uint128>(0x0fc01) << 64) | static_cast<uint128>(0x0080402010080400), (static_cast<uint128>(0x0fa02) << 64) | static_cast<uint128>(0x0100804020100800), (static_cast<uint128>(0x0f604) << 64) | static_cast<uint128>(0x0201008040201000), (static_cast<uint128>(0x0ee08) << 64) | static_cast<uint128>(0x0402010080402000), (static_cast<uint128>(0x0de10) << 64) | static_cast<uint128>(0x0804020100804000), (static_cast<uint128>(0x0be20) << 64) | static_cast<uint128>(0x1008040201008000), (static_cast<uint128>(0x07e40) << 64) | static_cast<uint128>(0x2010080402010000), (static_cast<uint128>(0x0fe80) << 64) | static_cast<uint128>(0x4020100804020000),
        // clang-format on
};
template <>
inline const uint128 shogi::Magic::premask_diagonal[shogi::Config::num_squares]
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
0xffffffff, 0xffffffff, 0x00420061, 0x58ec9440, 0x54891828, 0x004201e2, 0x22c00054, 0x41880103, 0x20420926,
0xffffffff, 0xffffffff, 0x079ac001, 0x55325004, 0xc0583021, 0x81152105, 0x08c08058, 0x80829089, 0x86102410,
0xffffffff, 0xffffffff, 0x0400c004, 0xb0058500, 0xd4a10830, 0x20618040, 0x0a01823e, 0x4294916a, 0x0428500c,
0xffffffff, 0xffffffff, 0x04b04028, 0x8c048830, 0x001809c8, 0x08027802, 0x023a40aa, 0x252085d1, 0xc114b004,
0xffffffff, 0xffffffff, 0x08400534, 0x20b98c08, 0x0091b608, 0x10a344b1, 0x1d000a05, 0x0aa18028, 0x70000aca,
0xffffffff, 0xffffffff, 0x02050140, 0x1c20121e, 0xec414504, 0x02412a10, 0x23439158, 0x509c0001, 0x11d0c0a8,
0xffffffff, 0xffffffff, 0x2010080b, 0xc5400e23, 0x01090422, 0x0000240a, 0x02c84504, 0x1804ca39, 0x0257190c,
0xffffffff, 0xffffffff, 0x49010068, 0xd0026a20, 0xe07001b4, 0x09001718, 0x40841232, 0x31000905, 0xb5800a4c,
0xffffffff, 0xffffffff, 0x08000698, 0xa0004108, 0x8093160c, 0x40029101, 0x02019884, 0x44040e23, 0x64446724,
        // clang-format on
};
template <>
inline const std::uint32_t
    shogi::Magic::magic_number_south[shogi::Config::num_squares]
    = {
        // clang-format off
0x1c200c1c, 0x82c40c00, 0x80202600, 0x28a80b00, 0xa8820a00, 0x48048030, 0x68278c11, 0xffffffff, 0xffffffff,
0x50400081, 0x42100c10, 0x94524fa0, 0x21008010, 0x00004194, 0x80402c38, 0x6e210040, 0xffffffff, 0xffffffff,
0x8c0851d0, 0x117e2148, 0x4b000b11, 0x12109099, 0x42a48489, 0x81420848, 0x88a24240, 0xffffffff, 0xffffffff,
0x500464d0, 0x341400c8, 0x0a805164, 0xaa209881, 0x28007240, 0x1020b600, 0xc2841044, 0xffffffff, 0xffffffff,
0x6000a581, 0x00848480, 0x76e0b805, 0x11002104, 0x1280427c, 0xbc644420, 0x40619c0a, 0xffffffff, 0xffffffff,
0x060241ac, 0x0a280624, 0x03060527, 0x2f0c0203, 0x930d402d, 0x02c00005, 0x00800a24, 0xffffffff, 0xffffffff,
0x0101a451, 0x01011094, 0x4800222c, 0x90122848, 0x00725aa4, 0x04022032, 0x000028cb, 0xffffffff, 0xffffffff,
0x00026028, 0x00112f48, 0x3c9008c1, 0x24000008, 0x81c01018, 0x0d812c41, 0x01c62001, 0xffffffff, 0xffffffff,
0xaa00c4c9, 0x0042c201, 0x80004105, 0x84206045, 0x8c265245, 0x43e92c01, 0xa043119a, 0xffffffff, 0xffffffff,
        // clang-format on
};
template <>
inline const std::uint32_t
    shogi::Magic::magic_number_adjacent[shogi::Config::num_squares]
    = {
        // clang-format off
0x2000409b, 0x10080011, 0x010000c1, 0x29800031, 0x80020841, 0x01410010, 0x80100810, 0x44008828, 0x00020318,
0x09200881, 0x400c0401, 0xa0982008, 0x4c440011, 0x00602003, 0x800c1208, 0x0c600084, 0x48000104, 0xa1246018,
0xaa20c092, 0x14110006, 0x2181a208, 0x40140084, 0x0a271002, 0x1030a302, 0x51302002, 0x2491d804, 0x00425802,
0x35802002, 0x14803002, 0x00182808, 0x00a80808, 0x00880410, 0x00002008, 0x0c050810, 0x54198008, 0x88206008,
0x10100101, 0x04488204, 0x41428010, 0x84d20010, 0xc0d00404, 0x92180014, 0x0880a8d0, 0x5a832208, 0x2410a208,
0xd14137b4, 0x08208944, 0x0000aca8, 0x036ac2a8, 0x14f02104, 0x02296024, 0x00010324, 0x02462002, 0x84363002,
0x4406a104, 0x10080401, 0x4008000a, 0x40800102, 0x0a000082, 0x020b8204, 0x43340204, 0x01c00104, 0x12020024,
0x82580101, 0xf2100044, 0x10020802, 0x01c85088, 0x92148d48, 0x00505104, 0x30972104, 0xa3008408, 0x83612c08,
0xc0280086, 0x225a0982, 0x961800d4, 0x3a442184, 0x12c0c04c, 0xe9ac0041, 0x0b81808e, 0x01080081, 0x88c0001a,
        // clang-format on
};
template <>
inline const std::uint32_t
    shogi::Magic::magic_number_diagonal[shogi::Config::num_squares]
    = {
        // clang-format off
0x010c8512, 0x4a950202, 0x0084ad56, 0x6004108a, 0x85800191, 0x13002920, 0x020c091a, 0x00601029, 0x989c04b4,
0x010208c2, 0x00540424, 0x20844322, 0x4d5ec174, 0x32019084, 0x1cac10cc, 0x08440d02, 0x180046a0, 0x31919450,
0xc0081016, 0x418c506c, 0xaa4021a4, 0x4d8310e4, 0x1ea08248, 0x60040059, 0x24b04815, 0x040084e0, 0x02904031,
0x00588962, 0xc0419021, 0x045598a9, 0xb815e301, 0x02002105, 0x0146890c, 0x00089092, 0x004024c8, 0x28149054,
0xa86218a0, 0xcc906ac8, 0x0601b041, 0x2d223006, 0x28944c02, 0x00880982, 0x4c408124, 0x8c6a349a, 0x7344a50e,
0x2500a20d, 0x01208016, 0x80822014, 0xa82e2002, 0x00017206, 0x11886101, 0x20042014, 0x96ca4048, 0x03463f88,
0xa00400b0, 0x45055004, 0x19005e2d, 0x41124e05, 0x00901842, 0x8ca40698, 0x40000869, 0xaa2a4014, 0x20008046,
0x4c002091, 0x81968025, 0x54f20182, 0x0070007a, 0x09031c24, 0x45088c14, 0xcc423284, 0x00860803, 0xeb104990,
0x40bc2007, 0x38440548, 0x000f0001, 0x22c40184, 0x20ecc011, 0xc041040a, 0x90239150, 0x03500323, 0x10081d41,
        // clang-format on
};
template <>
inline shogi::BitBoard
    shogi::Magic::attack_table_north[shogi::Config::num_squares]
                                    [shogi::Magic::table_size]
    = {};
template <>
inline shogi::BitBoard
    shogi::Magic::attack_table_south[shogi::Config::num_squares]
                                    [shogi::Magic::table_size]
    = {};
template <>
inline shogi::BitBoard
    shogi::Magic::attack_table_adjacent[shogi::Config::num_squares]
                                       [shogi::Magic::table_size]
    = {};
template <>
inline shogi::BitBoard
    shogi::Magic::attack_table_diagonal[shogi::Config::num_squares]
                                       [shogi::Magic::table_size]
    = {};

template <>
inline bool shogi::Board::is_square_attacked_by_ranging_pieces(
    const ColorEnum& by_side, const Square& sq, const Square& skip) const
{
    using namespace shogi;
    const BitBoardType occ_full = get_occupied().clear(skip);
    {
        const auto occ_offence = get_occupied<KY, HI, RY>(by_side)
                                 & BitBoardType::get_attacks_by(
                                     PHelper::to_board_piece(~by_side, KY), sq);
        if (occ_offence.any()) {
            const auto attack_inverted
                = (by_side == BLACK)
                      ? shogi::Magic::get_south_attack(sq, occ_full)
                      : shogi::Magic::get_north_attack(sq, occ_full);
            if ((attack_inverted & occ_offence).any())
                return true;
        }
    }
    {
        const auto occ_offence = get_occupied<KA, UM>(by_side)
                                 & shogi::Magic::get_diagonal_attack(sq);
        if (occ_offence.any()) {
            const auto attack_inverted
                = shogi::Magic::get_diagonal_attack(sq, occ_full);
            if ((attack_inverted & occ_offence).any())
                return true;
        }
    }
    {
        const auto occ_offence = get_occupied<HI, RY>(by_side)
                                 & BitBoardType::get_attacks_by(B_HI, sq);
        if (occ_offence.any()) {
            const auto attack_inverted
                = shogi::Magic::get_adjacent_attack(sq, occ_full);
            if ((attack_inverted & occ_offence).any())
                return true;
        }
    }
    return false;
}

template <>
inline bool shogi::Board::is_square_attacked(
    const ColorEnum& by_side, const Square& sq, const Square& skip) const
{
    assert(sq != C::SQ_NA);
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
inline shogi::Move NonKingBoardMoveGenerator<shogi::Parameters>::random_select()
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
        auto iter_fgke = NonKingBoardMoveGenerator<Parameters>(
            m_state, src_fgke, m_pinned);
        if (!iter_fgke.is_end())
            return iter_fgke.random_select_by_iterating_all();
        auto iter_kkhi = NonKingBoardMoveGenerator<Parameters>(
            m_state, src_kkhi, m_pinned);
        if (!iter_kkhi.is_end())
            return iter_kkhi.random_select_by_iterating_all();
        auto iter_gold
            = NoPromoMoveGenerator<Parameters>(m_state, src_gold, m_pinned);
        if (!iter_gold.is_end())
            return iter_gold.random_select();
        auto iter_umry
            = NoPromoMoveGenerator<Parameters>(m_state, src_umry, m_pinned);
        return iter_umry.random_select();
    }
    r -= fraction_fgke;
    const auto fraction_kkhi = num_kkhi / num_src;
    if (r < fraction_kkhi) {
        auto iter_kkhi = NonKingBoardMoveGenerator(m_state, src_kkhi, m_pinned);
        if (!iter_kkhi.is_end())
            return iter_kkhi.random_select_by_iterating_all();
        auto iter_gold
            = NoPromoMoveGenerator<Parameters>(m_state, src_gold, m_pinned);
        if (!iter_gold.is_end())
            return iter_gold.random_select();
        auto iter_umry
            = NoPromoMoveGenerator<Parameters>(m_state, src_umry, m_pinned);
        if (!iter_umry.is_end())
            return iter_umry.random_select();
        auto iter_fgke = NonKingBoardMoveGenerator(m_state, src_fgke, m_pinned);
        return iter_fgke.random_select_by_iterating_all();
    }
    r -= fraction_kkhi;
    const auto fraction_gold = num_gold / num_src;
    if (r < fraction_gold) {
        auto iter_gold
            = NoPromoMoveGenerator<Parameters>(m_state, src_gold, m_pinned);
        if (!iter_gold.is_end())
            return iter_gold.random_select();
        auto iter_umry
            = NoPromoMoveGenerator<Parameters>(m_state, src_umry, m_pinned);
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
            = NoPromoMoveGenerator<Parameters>(m_state, src_umry, m_pinned);
        if (!iter_umry.is_end())
            return iter_umry.random_select();
        auto iter_fgke = NonKingBoardMoveGenerator(m_state, src_fgke, m_pinned);
        if (!iter_fgke.is_end())
            return iter_fgke.random_select_by_iterating_all();
        auto iter_kkhi = NonKingBoardMoveGenerator(m_state, src_kkhi, m_pinned);
        if (!iter_kkhi.is_end())
            return iter_kkhi.random_select_by_iterating_all();
        auto iter_gold
            = NoPromoMoveGenerator<Parameters>(m_state, src_gold, m_pinned);
        return iter_gold.random_select();
    }
}

} // namespace vshogi

#endif // VSHOGI_VARIANTS_SHOGI_HPP
