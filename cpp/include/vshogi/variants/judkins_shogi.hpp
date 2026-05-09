#ifndef VSHOGI_VARIANTS_JUDKINS_SHOGI_HPP
#define VSHOGI_VARIANTS_JUDKINS_SHOGI_HPP

#include <cassert>
#include <cstdint>
#include <type_traits>

#include "vshogi/common/bitboard_traits.hpp"
#include "vshogi/common/board.hpp"
#include "vshogi/common/color.hpp"
#include "vshogi/common/config.hpp"
#include "vshogi/common/game.hpp"
#include "vshogi/common/generator.hpp"
#include "vshogi/common/magic.hpp"
#include "vshogi/common/move.hpp"
#include "vshogi/common/notation.hpp"
#include "vshogi/common/piece_traits.hpp"
#include "vshogi/common/square_traits.hpp"
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

enum PieceEnum : std::uint8_t
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
*/
enum SquareEnum : uint
{
    // clang-format off
    SQ_6A = 30, SQ_5A = 24, SQ_4A = 18, SQ_3A = 12, SQ_2A =  6, SQ_1A = 0,
    SQ_6B = 31, SQ_5B = 25, SQ_4B = 19, SQ_3B = 13, SQ_2B =  7, SQ_1B = 1,
    SQ_6C = 32, SQ_5C = 26, SQ_4C = 20, SQ_3C = 14, SQ_2C =  8, SQ_1C = 2,
    SQ_6D = 33, SQ_5D = 27, SQ_4D = 21, SQ_3D = 15, SQ_2D =  9, SQ_1D = 3,
    SQ_6E = 34, SQ_5E = 28, SQ_4E = 22, SQ_3E = 16, SQ_2E = 10, SQ_1E = 4,
    SQ_6F = 35, SQ_5F = 29, SQ_4F = 23, SQ_3F = 17, SQ_2F = 11, SQ_1F = 5,
    // clang-format on
    SQ_NA = 36,
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
    FILE1 = 0,
    FILE2,
    FILE3,
    FILE4,
    FILE5,
    FILE6,
};

struct Parameters
{
    // clang-format off
    static constexpr std::array<FullPieceTypes, 13u> piece_types = {PT_FU, PT_KE, PT_GI, PT_KA, PT_HI, PT_KI, PT_OU, PT_TO, PT_NK, PT_NG, PT_UM, PT_RY, PT_NA};
    static constexpr uint num_stand_piece_types = 6; // FU, KE, GI, KA, HI, KI
    static constexpr uint num_files = 6; // 1, 2, 3, 4, 5, 6
    static constexpr uint num_ranks = 6; // A, B, C, D, E, F
    static constexpr uint num_promotion_ranks = 2;
    static constexpr uint num_dir = 12; //!< NW, N, NE, W, E, SW, S, SE, SSW, SSE, NNW, NNE
    static constexpr uint max_stand_piece_count = 2;
    static constexpr uint max_stand_sfen_length = 13; // "RBGSNPrbgsnp "
    static constexpr uint nfold_repetitions = 4;
    static constexpr std::array<uint, 13u> initial_piece_count = {2, 2, 2, 2, 2, 2, 2, 0, 0, 0, 0, 0, 0};
    static constexpr std::array<PieceEnum, 36> initial_position = {
        W_OU, W_FU, VOID, VOID, VOID, B_HI,
        W_KI, VOID, VOID, VOID, VOID, B_KA,
        W_GI, VOID, VOID, VOID, VOID, B_KE,
        W_KE, VOID, VOID, VOID, VOID, B_GI,
        W_KA, VOID, VOID, VOID, VOID, B_KI,
        W_HI, VOID, VOID, VOID, B_FU, B_OU,
    };
    using BaseTypeBitBoard = std::uint64_t;
    using BaseTypeStand = std::uint32_t; // _______* *_**_**_ **_**_** (KI, HI, KA, GI, KE, FU)
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
static_assert(Config::initial_points == 14u);
static_assert(Config::sum_piece_value == 950u);

} // namespace vshogi::judkins_shogi

namespace vshogi
{

template <>
inline std::uint64_t
    judkins_shogi::BitboardTraits::table_attacks[judkins_shogi::VOID + 1u]
                                                [judkins_shogi::SQ_NA + 1u]
    = {};
template <>
inline std::uint64_t judkins_shogi::BitboardTraits::table_pre_reverse_attack
    [judkins_shogi::SQ_NA + 1u][judkins_shogi::VOID + 1u]
    = {};
template <>
inline std::uint64_t
    judkins_shogi::BitboardTraits::table_ray[judkins_shogi::SQ_NA + 1u]
                                            [DIR_NA + 1u]
    = {};
template <>
inline std::uint64_t
    judkins_shogi::BitboardTraits::table_mask_between[judkins_shogi::SQ_NA + 1u]
                                                     [judkins_shogi::SQ_NA + 1u]
    = {};

template <>
inline const uint judkins_shogi::Stand::shift_bits[] = {0, 3, 6, 9, 12, 15};

template <>
inline const std::uint32_t judkins_shogi::Stand::masks[] = {
    // clang-format off
    0x00003, // FU
    0x00018, // KE
    0x000c0, // GI
    0x00600, // KA
    0x03000, // HI
    0x18000, // KI
    // clang-format on
};

template <>
inline const std::uint32_t judkins_shogi::Stand::deltas[] = {
    // clang-format off
    1 << 0, // FU
    1 << 3, // KE
    1 << 6, // GI
    1 << 9, // KA
    1 << 12, // HI
    1 << 15, // KI
    // clang-format on
};

template <>
inline const std::uint32_t judkins_shogi::Stand::mask
    = 0x00003 | 0x00018 | 0x000c0 | 0x00600 | 0x03000 | 0x18000;

template <>
inline const judkins_shogi::PieceTypeEnum
    Notation<judkins_shogi::Parameters>::pieces_in_sfen_order[]
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
    [judkins_shogi::SQ_NA]
    [num_colors * judkins_shogi::Config::num_piece_types + 1]
    = {};

template <>
inline const std::uint64_t
    judkins_shogi::Magic::premask_adjacent[judkins_shogi::SQ_NA]
    = {
        // clang-format off
0x000000000104105e, 0x000000000208209c, 0x000000000410411a, 0x0000000008208216, 0x000000001041040e, 0x000000002082081e,
0x0000000001041780, 0x0000000002082700, 0x0000000004104680, 0x0000000008208580, 0x0000000010410380, 0x0000000020820780,
0x000000000105e040, 0x000000000209c080, 0x000000000411a100, 0x0000000008216200, 0x000000001040e400, 0x000000002081e800,
0x0000000001781040, 0x0000000002702080, 0x0000000004684100, 0x0000000008588200, 0x0000000010390400, 0x00000000207a0800,
0x000000001e041040, 0x000000001c082080, 0x000000001a104100, 0x0000000016208200, 0x000000000e410400, 0x000000001e820800,
0x0000000781041040, 0x0000000702082080, 0x0000000684104100, 0x0000000588208200, 0x0000000390410400, 0x00000007a0820800,
        // clang-format on
};
template <>
inline const std::uint64_t
    judkins_shogi::Magic::premask_diagonal[judkins_shogi::SQ_NA]
    = {
        // clang-format off
0x0000000010204080, 0x0000000000408100, 0x0000000000010280, 0x0000000000002500, 0x0000000000084200, 0x0000000002108400,
0x0000000008102000, 0x0000000010204000, 0x000000000040a000, 0x0000000000094000, 0x0000000002108000, 0x0000000004210000,
0x0000000004080080, 0x0000000008100100, 0x0000000010280280, 0x0000000002500500, 0x0000000004200200, 0x0000000008400400,
0x0000000002002100, 0x0000000004004200, 0x000000000a00a400, 0x0000000014014080, 0x0000000008008100, 0x0000000010010200,
0x0000000000084200, 0x0000000000108400, 0x0000000000290000, 0x0000000000502000, 0x0000000000204080, 0x0000000000408100,
0x0000000002108400, 0x0000000004210000, 0x000000000a400000, 0x0000000014080000, 0x0000000008102000, 0x0000000010204080,
        // clang-format on
};
template <>
inline const std::uint32_t
    judkins_shogi::Magic::magic_number_adjacent[judkins_shogi::SQ_NA]
    = {
        0x02002040, 0x20840c03, 0x0c020814, 0x02010202, 0x08680163, 0x0200a024,
        0x05280840, 0x38040840, 0x00442010, 0x00100830, 0x51084204, 0x13084002,
        0x80282040, 0x80121020, 0x00010804, 0x00010488, 0x00051814, 0x02008104,
        0x848800a0, 0x45405040, 0x02010260, 0x00080310, 0x10040422, 0x08044204,
        0x6202004d, 0x01000c51, 0x040480c8, 0x23068101, 0x00842005, 0x002a4098,
        0x04084042, 0x81804440, 0x15084021, 0xa040100a, 0x21204021, 0x00881813,
};
template <>
inline const std::uint32_t
    judkins_shogi::Magic::magic_number_diagonal[judkins_shogi::SQ_NA]
    = {
        0x88b08098, 0x04101080, 0x02280440, 0x02140cc4, 0x00045080, 0x03082402,
        0x00410401, 0x010900a5, 0x00108888, 0x0004a4c0, 0x80084440, 0x08000408,
        0x02594811, 0x80114501, 0x00840082, 0x01c80822, 0x40320b0a, 0x00245224,
        0x03042418, 0x80288001, 0x48202004, 0x00108011, 0x00201010, 0x04100508,
        0x40391420, 0xa0811803, 0x80100400, 0x0b03a110, 0x01108a00, 0x40828238,
        0x54044818, 0x81190520, 0x080108a6, 0x00008401, 0x51904108, 0x1a104482,
};
template <>
inline judkins_shogi::bitboard_t judkins_shogi::Magic::attack_table_adjacent
    [judkins_shogi::SQ_NA][judkins_shogi::Magic::table_size_adjacent]
    = {};
template <>
inline judkins_shogi::bitboard_t judkins_shogi::Magic::attack_table_diagonal
    [judkins_shogi::SQ_NA][judkins_shogi::Magic::table_size_diagonal]
    = {};

} // namespace vshogi

#endif // VSHOGI_VARIANTS_JUDKINS_SHOGI_HPP
