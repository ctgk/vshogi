#ifndef VSHOGI_VARIANTS_MINISHOGI_HPP
#define VSHOGI_VARIANTS_MINISHOGI_HPP

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

namespace vshogi::minishogi
{

enum PieceTypeEnum : std::uint8_t
{
    FU, //!< Fu (Pawn)
    GI, //!< Gin (Silver)
    KA, //!< Kaku (Bishop)
    HI, //!< Hisha (Rook)
    KI, //!< Kin (Gold)
    OU, //!< Ou, Gyoku (King)
    TO, //!< Tokin (Promoted Pawn)
    NG, //!< Nari-Gin (Promoted Silver)
    UM, //!< Uma (Promoted Bishop)
    RY, //!< Ryu (Promoted Rook)
    NA, //!< Not available
};

enum PieceEnum : std::uint8_t
{
    B_FU, //!< Black Fu (Pawn)
    B_GI, //!< Black Gin (Silver)
    B_KA, //!< Black Kaku (Bishop)
    B_HI, //!< Black Hisha (Rook)
    B_KI, //!< Black Kin (Gold)
    B_OU, //!< Black Ou, Gyoku (King)
    B_TO, //!< Black Tokin (Promoted Pawn)
    B_NG, //!< Black Nari-Gin (Promoted Silver)
    B_UM, //!< Black Uma (Promoted Bishop)
    B_RY, //!< Black Ryu (Promoted Rook)

    W_FU, //!< White Fu (Pawn)
    W_GI, //!< White Gin (Silver)
    W_KA, //!< White Kaku (Bishop)
    W_HI, //!< White Hisha (Rook)
    W_KI, //!< White Kin (Gold)
    W_OU, //!< White Ou, Gyoku (King)
    W_TO, //!< White Tokin (Promoted Pawn)
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
    SQ_5A = 20, SQ_4A = 15, SQ_3A = 10, SQ_2A = 5, SQ_1A = 0,
    SQ_5B = 21, SQ_4B = 16, SQ_3B = 11, SQ_2B = 6, SQ_1B = 1,
    SQ_5C = 22, SQ_4C = 17, SQ_3C = 12, SQ_2C = 7, SQ_1C = 2,
    SQ_5D = 23, SQ_4D = 18, SQ_3D = 13, SQ_2D = 8, SQ_1D = 3,
    SQ_5E = 24, SQ_4E = 19, SQ_3E = 14, SQ_2E = 9, SQ_1E = 4,
    // clang-format on
    SQ_NA = 25,
    NUM_SQ = 25,
};
enum FileEnum : uint
{
    FILE1 = 0,
    FILE2,
    FILE3,
    FILE4,
    FILE5,
};
enum RankEnum : uint
{
    RANK1 = 0,
    RANK2,
    RANK3,
    RANK4,
    RANK5,
};

struct Parameters
{
    // clang-format off
    static constexpr std::array<FullPieceTypes, 11u> piece_types = {PT_FU, PT_GI, PT_KA, PT_HI, PT_KI, PT_OU, PT_TO, PT_NG, PT_UM, PT_RY, PT_NA};
    static constexpr uint num_stand_piece_types = 5; // FU, GI, KA, HI, KI
    static constexpr uint sum_piece_value = 2 * (60 * 3 + 115 + 120);
    static constexpr uint num_files = 5; // 1, 2, 3, 4, 5
    static constexpr uint num_ranks = 5; // A, B, C, D, E
    static constexpr uint num_promotion_ranks = 1;
    static constexpr uint num_dir = 8; //!< NW, N, NE, W, E, SW, S, SE
    static constexpr uint max_stand_piece_count = 2;
    static constexpr uint max_stand_sfen_length = 11; // "2p2s2g2b2r "
    static constexpr uint nfold_repetitions = 4;
    static constexpr std::array<uint, 11u> initial_piece_count = {2, 2, 2, 2, 2, 2, 0, 0, 0, 0, 0};
    static constexpr std::array<PieceEnum, 25> initial_position = {
        W_OU, W_FU, VOID, VOID, B_HI,
        W_KI, VOID, VOID, VOID, B_KA,
        W_GI, VOID, VOID, VOID, B_GI,
        W_KA, VOID, VOID, VOID, B_KI,
        W_HI, VOID, VOID, B_FU, B_OU,
    };
    using BaseTypeBitBoard = std::uint32_t;
    using BaseTypeStand
        = std::uint16_t; // __11_11_ 11_11_11 (KI, HI, KA, GI, FU)
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
static_assert(Config::initial_points == 13u);

} // namespace vshogi::minishogi

namespace vshogi
{

template <>
inline std::uint32_t
    minishogi::BitboardTraits::table_attacks[minishogi::VOID + 1u]
                                            [minishogi::SQ_NA + 1u]
    = {};
template <>
inline std::uint32_t
    minishogi::BitboardTraits::table_pre_reverse_attack[minishogi::SQ_NA + 1u]
                                                       [minishogi::VOID + 1u]
    = {};
template <>
inline std::uint32_t minishogi::BitboardTraits::table_ray[minishogi::SQ_NA + 1u]
                                                         [DIR_NA + 1u]
    = {};
template <>
inline std::uint32_t
    minishogi::BitboardTraits::table_mask_between[minishogi::SQ_NA + 1u]
                                                 [minishogi::SQ_NA + 1u]
    = {};

template <>
inline const uint minishogi::Stand::shift_bits[] = {0, 3, 6, 9, 12};
template <>
inline const minishogi::Config::BaseTypeStand minishogi::Stand::masks[] = {
    // clang-format off
    0b0000000000000011,
    0b0000000000011000,
    0b0000000011000000,
    0b0000011000000000,
    0b0011000000000000,
    // clang-format on
};
template <>
inline const std::uint16_t minishogi::Stand::deltas[] = {
    // clang-format off
    0b0000000000000001,
    0b0000000000001000,
    0b0000000001000000,
    0b0000001000000000,
    0b0001000000000000,
    // clang-format on
};
template <>
inline const std::uint16_t minishogi::Stand::mask = 0b0011011011011011;

template <>
inline const minishogi::PieceTypeEnum
    Notation<minishogi::Parameters>::pieces_in_sfen_order[]
    = {minishogi::HI,
       minishogi::KA,
       minishogi::KI,
       minishogi::GI,
       minishogi::FU};

template <>
inline std::uint64_t minishogi::BlackWhiteStands::zobrist_table
    [num_colors][minishogi::Config::num_stand_piece_types]
    [minishogi::Config::max_stand_piece_count + 1]
    = {};

template <>
inline std::uint64_t minishogi::Board::zobrist_table
    [minishogi::SQ_NA][num_colors * minishogi::Config::num_piece_types + 1]
    = {};

template <>
inline const std::uint32_t minishogi::Magic::premask_adjacent[minishogi::SQ_NA]
    = {
        0x0000842e, 0x0001084c, 0x0002108a, 0x00042106, 0x0008420e,
        0x000085c0, 0x00010980, 0x00021140, 0x000420c0, 0x000841c0,
        0x0000b820, 0x00013040, 0x00022880, 0x00041900, 0x00083a00,
        0x00070420, 0x00060840, 0x00051080, 0x00032100, 0x00074200,
        0x00e08420, 0x00c10840, 0x00a21080, 0x00642100, 0x00e84200,
};
template <>
inline const std::uint32_t minishogi::Magic::premask_diagonal[minishogi::SQ_NA]
    = {
        0x00041040, 0x00002080, 0x00000140, 0x00000880, 0x00011100,
        0x00020800, 0x00041000, 0x00002800, 0x00011000, 0x00022000,
        0x00010040, 0x00020080, 0x00050140, 0x00020080, 0x00040100,
        0x00000880, 0x00001100, 0x00002800, 0x00001040, 0x00002080,
        0x00011100, 0x00022000, 0x00050000, 0x00020800, 0x00041040,
};
template <>
inline const std::uint32_t
    minishogi::Magic::magic_number_adjacent[minishogi::SQ_NA]
    = {
        0x24208005, 0x42841008, 0x62081008, 0x22040405, 0x10081700,
        0x20504014, 0x20208c04, 0x02182008, 0x02440140, 0x01219110,
        0x0d082040, 0x88161000, 0x10422020, 0x04910105, 0x00a40881,
        0x1022080c, 0x0c150c81, 0x10382400, 0x00502000, 0x02845048,
        0x04100941, 0x65160900, 0x03010240, 0x01044480, 0x42085100,
};
template <>
inline const std::uint32_t
    minishogi::Magic::magic_number_diagonal[minishogi::SQ_NA]
    = {
        0x45084810, 0x82810004, 0x04881240, 0x0c840001, 0x801290a0,
        0x09501000, 0x00040400, 0x0c040000, 0x20442088, 0x0040a000,
        0x00801090, 0x00402001, 0x30801082, 0x18a04000, 0x42100820,
        0x01088008, 0x04428062, 0x0822000c, 0x02010820, 0x40c0d0a0,
        0x08814108, 0x80090800, 0xa0084800, 0x00101402, 0xa8881000,
};
template <>
inline minishogi::bitboard_t minishogi::Magic::attack_table_adjacent
    [minishogi::SQ_NA][minishogi::Magic::table_size_adjacent]
    = {};
template <>
inline minishogi::bitboard_t minishogi::Magic::attack_table_diagonal
    [minishogi::SQ_NA][minishogi::Magic::table_size_diagonal]
    = {};

} // namespace vshogi

#endif // VSHOGI_VARIANTS_MINISHOGI_HPP
