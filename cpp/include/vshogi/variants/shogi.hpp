#ifndef VSHOGI_VARIANTS_SHOGI_HPP
#define VSHOGI_VARIANTS_SHOGI_HPP

#include <cassert>
#include <cstdint>
#include <type_traits>

#include "vshogi/common/bitboard_traits.hpp"
#include "vshogi/common/board.hpp"
#include "vshogi/common/color.hpp"
#include "vshogi/common/game.hpp"
#include "vshogi/common/generator.hpp"
#include "vshogi/common/magic.hpp"
#include "vshogi/common/move.hpp"
#include "vshogi/common/notation.hpp"
#include "vshogi/common/piece_traits.hpp"
#include "vshogi/common/square_traits.hpp"
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

enum PieceEnum : std::uint8_t
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
    static constexpr uint max_stand_piece_count = 18;
    static constexpr uint max_stand_sfen_length = 26; // "10p2l2n2sbr2g2P2L2N2SBR2G "
    static constexpr uint nfold_repetitions = 4;
    static constexpr std::array<uint, 15u> initial_piece_count = {18, 4, 4, 4, 2, 2, 4, 2, 0, 0, 0, 0, 0, 0, 0};
    static constexpr std::array<PieceEnum, 81> initial_position = {
        W_KY, VOID, W_FU, VOID, VOID, VOID, B_FU, VOID, B_KY,
        W_KE, W_KA, W_FU, VOID, VOID, VOID, B_FU, B_HI, B_KE,
        W_GI, VOID, W_FU, VOID, VOID, VOID, B_FU, VOID, B_GI,
        W_KI, VOID, W_FU, VOID, VOID, VOID, B_FU, VOID, B_KI,
        W_OU, VOID, W_FU, VOID, VOID, VOID, B_FU, VOID, B_OU,
        W_KI, VOID, W_FU, VOID, VOID, VOID, B_FU, VOID, B_KI,
        W_GI, VOID, W_FU, VOID, VOID, VOID, B_FU, VOID, B_GI,
        W_KE, W_HI, W_FU, VOID, VOID, VOID, B_FU, B_KA, B_KE,
        W_KY, VOID, W_FU, VOID, VOID, VOID, B_FU, VOID, B_KY,
    };
    using BaseTypeBitBoard = uint128;
    using BaseTypeStand = std::uint32_t; // _____*** _**_**_* **_***_* **_***** (KI, HI, KA, GI, KE, KY, FU)
    using PieceType = PieceTypeEnum;
    using Piece = PieceEnum;
    using Square = SquareEnum;
    using File = FileEnum;
    using Rank = RankEnum;
    Parameters() = delete;
    // clang-format on
};

using Config = vshogi::Configuration<Parameters>;
using PieceTraits = vshogi::PieceTraits<Parameters>;
using SquareTraits = vshogi::SquareTraits<Parameters>;
using BitboardTraits = vshogi::BitboardTraits<Parameters>;
using MoveTraits = vshogi::MoveTraits<Parameters>;
using Magic = vshogi::Magic<Parameters>;
using Board = vshogi::Board<Parameters>;
using Stand = vshogi::Stand<Parameters>;
using BlackWhiteStands = vshogi::BlackWhiteStands<Parameters>;
using State = vshogi::State<Parameters>;
using LegalMoveGenerator = vshogi::MoveGenerator<Parameters, GenEnum::LEGAL>;
using Game = vshogi::Game<Parameters>;
using bitboard_t = typename Config::bitboard_t;
static_assert(FU == Config::FU);
static_assert(OU == Config::OU);
static_assert(NA == Config::NA);
static_assert(VOID == Config::VOID);
static_assert(Config::initial_points == 27u);

} // namespace vshogi::shogi

namespace vshogi
{

template <>
inline uint128 shogi::BitboardTraits::table_attacks[shogi::VOID + 1u]
                                                   [shogi::SQ_NA + 1u]
    = {};
template <>
inline uint128
    shogi::BitboardTraits::table_pre_reverse_attack[shogi::SQ_NA + 1u]
                                                   [shogi::VOID + 1u]
    = {};
template <>
inline uint128 shogi::BitboardTraits::table_ray[shogi::SQ_NA + 1u][DIR_NA + 1u]
    = {};
template <>
inline uint128 shogi::BitboardTraits::table_mask_between[shogi::SQ_NA + 1u]
                                                        [shogi::SQ_NA + 1u]
    = {};

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
inline const shogi::PieceTypeEnum
    Notation<shogi::Parameters>::pieces_in_sfen_order[]
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
    [shogi::SQ_NA][num_colors * shogi::Config::num_piece_types + 1]
    = {};

template <>
inline const uint128 shogi::Magic::premask_north[shogi::SQ_NA] = {
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
inline const uint128 shogi::Magic::premask_south[shogi::SQ_NA] = {
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
inline const uint128 shogi::Magic::premask_adjacent[shogi::SQ_NA] = {
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
inline const uint128 shogi::Magic::premask_diagonal[shogi::SQ_NA] = {
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
inline const std::uint32_t shogi::Magic::magic_number_north[shogi::SQ_NA] = {
    // clang-format off
0xffffffff, 0xffffffff, 0x58ec9440, 0x54891828, 0x22c00054, 0x41880103, 0x20420926, 0x079ac001, 0x1d000a05,
0xffffffff, 0xffffffff, 0x0aa18028, 0x02050140, 0xec414504, 0x02412a10, 0x06208020, 0x11d0c0a8, 0x48048030,
0xffffffff, 0xffffffff, 0x68278c11, 0x50400081, 0x42100c10, 0x94524fa0, 0x00004194, 0x6e210040, 0x88a24240,
0xffffffff, 0xffffffff, 0x341400c8, 0x0a805164, 0xaa209881, 0xc2841044, 0x6000a581, 0x40619c0a, 0x85f294c4,
0xffffffff, 0xffffffff, 0x2216b545, 0x1221a00e, 0x64000186, 0x04094102, 0x28042050, 0x01034301, 0xa8312008,
0xffffffff, 0xffffffff, 0x82020a8c, 0x88980402, 0x10528135, 0x0480024d, 0x0a001321, 0x1c43c061, 0x1bb00010,
0xffffffff, 0xffffffff, 0x06009c02, 0x000162f4, 0x8c082802, 0x10068005, 0x500d0ea1, 0x14801c1a, 0x406dab0a,
0xffffffff, 0xffffffff, 0x118a0011, 0x24811468, 0x9619111a, 0x0204b185, 0xa0002844, 0x8e182d10, 0x40470908,
0xffffffff, 0xffffffff, 0xc4ae8392, 0x8004b042, 0x39d22c0e, 0x080a8608, 0x14000278, 0x00010042, 0x592a0c84,
    // clang-format on
};
template <>
inline const std::uint32_t shogi::Magic::magic_number_south[shogi::SQ_NA] = {
    // clang-format off
0x0d006c68, 0x5a620858, 0x281100c0, 0x41d4c242, 0x20821608, 0x44400010, 0x00800898, 0xffffffff, 0xffffffff,
0x401b0201, 0x061ad253, 0xa4524000, 0x97211100, 0x10128011, 0x22a0310e, 0x40852401, 0xffffffff, 0xffffffff,
0x4a520102, 0x010a0504, 0x0c800820, 0x401b0996, 0x32214902, 0x64b0c4a0, 0x50424104, 0xffffffff, 0xffffffff,
0x22210004, 0x04034008, 0x00042e00, 0x200841a2, 0x2c12c408, 0x600ea200, 0x811022c0, 0xffffffff, 0xffffffff,
0x01219010, 0x00018910, 0x04012b01, 0x0a802a80, 0xa2511040, 0x36511028, 0x51004248, 0xffffffff, 0xffffffff,
0x4860a610, 0x02103819, 0x40a91692, 0x60150884, 0xa1019011, 0x040a4110, 0x1070010c, 0xffffffff, 0xffffffff,
0x4c206252, 0x17041512, 0x0c14c9a1, 0x39844848, 0x92905180, 0x00850584, 0xc0200302, 0xffffffff, 0xffffffff,
0x02900b08, 0xac041521, 0x01a348e6, 0x01024a51, 0x60000985, 0x496402a4, 0xb6818202, 0xffffffff, 0xffffffff,
0x0d4823c2, 0x201d14e2, 0x08a9c101, 0x8200480e, 0x00062aa2, 0x04910003, 0x81459202, 0xffffffff, 0xffffffff,
    // clang-format on
};
template <>
inline const std::uint32_t shogi::Magic::magic_number_adjacent[shogi::SQ_NA] = {
    // clang-format off
0x10020208, 0x88002041, 0x11008204, 0x85400086, 0x12020020, 0x41000183, 0x9d200310, 0x00320010, 0x4010000c,
0x2a900052, 0x81900a01, 0x20020402, 0x86a10012, 0x03420408, 0xc5000104, 0xe1240108, 0x10028102, 0xb806c402,
0x40103002, 0x21180021, 0x94600a04, 0x804da801, 0x20448102, 0x14d0a004, 0x01501d10, 0x04d9a850, 0x99040208,
0x4908001a, 0x08401001, 0x82b00001, 0x082d4006, 0xc4408018, 0x05080208, 0x23890008, 0x04040018, 0x02086008,
0xa4448808, 0x90002401, 0x300e0402, 0xae000042, 0x28220420, 0x8a840011, 0x00000102, 0x92600090, 0x02438101,
0xe2000838, 0xa4020282, 0xe792d104, 0x01188102, 0x08304081, 0x0b080a04, 0x80006001, 0x002bf001, 0x02042001,
0x00c08006, 0x40102005, 0x04300009, 0x21020224, 0x6d008401, 0x05411022, 0x82812082, 0xc4082222, 0x82352102,
0x01100608, 0x08808042, 0x12241404, 0x20204102, 0x0cac6208, 0xa0e9a008, 0x20024008, 0x82008810, 0x007a0601,
0x64422102, 0x20a00202, 0x80008202, 0x01000102, 0x00604202, 0x45800082, 0x05410082, 0x80015024, 0x40512002,
    // clang-format on
};
template <>
inline const std::uint32_t shogi::Magic::magic_number_diagonal[shogi::SQ_NA] = {
    // clang-format off
0xc0011f84, 0x22290658, 0x54c10070, 0x84008244, 0x45828c42, 0x80500081, 0x03180280, 0x01b41004, 0x28900173,
0x3a055381, 0x22c840a2, 0xa102008a, 0x504226a6, 0x00404420, 0xa10c0c88, 0x44206485, 0x4c0c238c, 0x98998c0a,
0x00042508, 0x42101808, 0x44000a01, 0xa840800d, 0x01624008, 0xa420200a, 0xf154052a, 0x88060042, 0xa27109e0,
0x1219350e, 0x76018014, 0x04208c0e, 0x40a82041, 0x06008401, 0xf3400205, 0x10008841, 0x21210a02, 0x0c4473e8,
0x10134603, 0x5048a442, 0x00642084, 0x00040041, 0x00085804, 0xe9808202, 0x291608d4, 0xa28962d0, 0x0412203c,
0x8c00c022, 0x0b290811, 0x80c96009, 0xe00b826c, 0x82214102, 0x028e4008, 0x43047008, 0x90106446, 0x24c2ac59,
0x0004c491, 0x41908303, 0xd82d0028, 0x02200208, 0x60000805, 0x01180350, 0xb8600084, 0x4a04007c, 0x88238a16,
0xac103401, 0x2452a401, 0x00001252, 0x224002c3, 0x8f4005c6, 0x00080203, 0x808080a0, 0x12927082, 0x041e8382,
0x28408802, 0xd00c82e2, 0x70000401, 0x0410801c, 0x08302722, 0xc491350a, 0x24000088, 0x841e420c, 0x50c91c33,
    // clang-format on
};
template <>
inline shogi::bitboard_t
    shogi::Magic::attack_table_north[shogi::SQ_NA]
                                    [shogi::Magic::table_size_lance]
    = {};
template <>
inline shogi::bitboard_t
    shogi::Magic::attack_table_south[shogi::SQ_NA]
                                    [shogi::Magic::table_size_lance]
    = {};
template <>
inline shogi::bitboard_t
    shogi::Magic::attack_table_adjacent[shogi::SQ_NA]
                                       [shogi::Magic::table_size_adjacent]
    = {};
template <>
inline shogi::bitboard_t
    shogi::Magic::attack_table_diagonal[shogi::SQ_NA]
                                       [shogi::Magic::table_size_diagonal]
    = {};

} // namespace vshogi

#endif // VSHOGI_VARIANTS_SHOGI_HPP
