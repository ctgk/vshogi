#ifndef VSHOGI_VARIANTS_JUDKINS_SHOGI_HPP
#define VSHOGI_VARIANTS_JUDKINS_SHOGI_HPP

#include <cassert>
#include <cstdint>
#include <type_traits>

#include "vshogi/common/bitboard.hpp"
#include "vshogi/common/board.hpp"
#include "vshogi/common/color.hpp"
#include "vshogi/common/config.hpp"
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
    static constexpr uint sum_piece_value = 2 * (60 * 4 + 115 + 120);
    static constexpr uint num_files = 6; // 1, 2, 3, 4, 5, 6
    static constexpr uint num_ranks = 6; // A, B, C, D, E, F
    static constexpr uint num_promotion_ranks = 2;
    static constexpr uint num_dir = 12; //!< NW, N, NE, W, E, SW, S, SE, SSW, SSE, NNW, NNE
    static constexpr uint log2_magic_table_size = 5;
    static constexpr uint max_stand_piece_count = 2;
    static constexpr uint max_stand_sfen_length = 13; // "RBGSNPrbgsnp "
    static constexpr uint max_acceptable_repetitions = 3;
    static constexpr uint num_init_piece_each = 6;
    static constexpr uint initial_points = 14;
    using BaseTypeBitBoard = std::uint64_t;
    using BaseTypeStand = std::uint32_t; // _______* *_**_**_ **_**_** (KI, HI, KA, GI, KE, FU)
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

constexpr BitBoard bb_na = BitBoard();
constexpr BitBoard bb_1a = BitBoard::from_square<SQ_1A>();
constexpr BitBoard bb_1b = BitBoard::from_square<SQ_1B>();
constexpr BitBoard bb_1c = BitBoard::from_square<SQ_1C>();
constexpr BitBoard bb_1d = BitBoard::from_square<SQ_1D>();
constexpr BitBoard bb_1e = BitBoard::from_square<SQ_1E>();
constexpr BitBoard bb_1f = BitBoard::from_square<SQ_1F>();
constexpr BitBoard bb_2a = BitBoard::from_square<SQ_2A>();
constexpr BitBoard bb_2b = BitBoard::from_square<SQ_2B>();
constexpr BitBoard bb_2c = BitBoard::from_square<SQ_2C>();
constexpr BitBoard bb_2d = BitBoard::from_square<SQ_2D>();
constexpr BitBoard bb_2e = BitBoard::from_square<SQ_2E>();
constexpr BitBoard bb_2f = BitBoard::from_square<SQ_2F>();
constexpr BitBoard bb_3a = BitBoard::from_square<SQ_3A>();
constexpr BitBoard bb_3b = BitBoard::from_square<SQ_3B>();
constexpr BitBoard bb_3c = BitBoard::from_square<SQ_3C>();
constexpr BitBoard bb_3d = BitBoard::from_square<SQ_3D>();
constexpr BitBoard bb_3e = BitBoard::from_square<SQ_3E>();
constexpr BitBoard bb_3f = BitBoard::from_square<SQ_3F>();
constexpr BitBoard bb_4a = BitBoard::from_square<SQ_4A>();
constexpr BitBoard bb_4b = BitBoard::from_square<SQ_4B>();
constexpr BitBoard bb_4c = BitBoard::from_square<SQ_4C>();
constexpr BitBoard bb_4d = BitBoard::from_square<SQ_4D>();
constexpr BitBoard bb_4e = BitBoard::from_square<SQ_4E>();
constexpr BitBoard bb_4f = BitBoard::from_square<SQ_4F>();
constexpr BitBoard bb_5a = BitBoard::from_square<SQ_5A>();
constexpr BitBoard bb_5b = BitBoard::from_square<SQ_5B>();
constexpr BitBoard bb_5c = BitBoard::from_square<SQ_5C>();
constexpr BitBoard bb_5d = BitBoard::from_square<SQ_5D>();
constexpr BitBoard bb_5e = BitBoard::from_square<SQ_5E>();
constexpr BitBoard bb_5f = BitBoard::from_square<SQ_5F>();
constexpr BitBoard bb_6a = BitBoard::from_square<SQ_6A>();
constexpr BitBoard bb_6b = BitBoard::from_square<SQ_6B>();
constexpr BitBoard bb_6c = BitBoard::from_square<SQ_6C>();
constexpr BitBoard bb_6d = BitBoard::from_square<SQ_6D>();
constexpr BitBoard bb_6e = BitBoard::from_square<SQ_6E>();
constexpr BitBoard bb_6f = BitBoard::from_square<SQ_6F>();

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
template <>
inline judkins_shogi::Stand::Stand(
    const int num_fu,
    const int num_ke,
    const int num_gi,
    const int num_ka,
    const int num_hi,
    const int num_ki)
    : Stand(static_cast<Int>(
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
inline const std::uint64_t
    judkins_shogi::Magic::premask_vertical[judkins_shogi::Config::num_squares]
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
inline const std::uint64_t
    judkins_shogi::Magic::premask_horizontal[judkins_shogi::Config::num_squares]
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
inline const std::uint64_t
    judkins_shogi::Magic::premask_diagonal[judkins_shogi::Config::num_squares]
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
inline const std::uint32_t judkins_shogi::Magic::magic_number_vertical
    [judkins_shogi::Config::num_squares]
    = {
        0x50881008, 0x05124000, 0x04000004, 0x0a00802a, 0x04005008, 0x04a00020,
        0x00100908, 0x01208480, 0x01102004, 0x20210408, 0x0c20021c, 0x01104008,
        0x00444200, 0x08008838, 0x12008400, 0x00021408, 0x00009084, 0x08008000,
        0x04000100, 0x00004094, 0x01240140, 0x00000100, 0x88c00200, 0x00003200,
        0x10002004, 0x020040a4, 0x02020527, 0x00800004, 0x01012010, 0x4800200c,
        0x88220244, 0x0c0c0028, 0x51288900, 0x06000801, 0x04080081, 0x08080012,
};
template <>
inline const std::uint32_t judkins_shogi::Magic::magic_number_horizontal
    [judkins_shogi::Config::num_squares]
    = {
        0xa028042c, 0x04961210, 0x002c6908, 0x03a10054, 0x080c3a14, 0x30010902,
        0x00088808, 0x02020140, 0x41486130, 0x00010045, 0x2000c048, 0x80000991,
        0x1a620224, 0x00102811, 0x10140810, 0x8104020c, 0x08020a06, 0x404400a4,
        0x4111a048, 0x08120240, 0x81c20008, 0x00425107, 0x000c4018, 0x08412004,
        0x02064400, 0x08170250, 0x04230800, 0x02043600, 0x20424220, 0x20014052,
        0x04883090, 0x00148840, 0x20210802, 0x80910048, 0x28031202, 0x0202204c,
};
template <>
inline const std::uint32_t judkins_shogi::Magic::magic_number_diagonal
    [judkins_shogi::Config::num_squares]
    = {
        0x04304222, 0x081012b8, 0x012440a0, 0x100a8001, 0x40402600, 0x02258240,
        0x84020086, 0x04a10508, 0x08014880, 0x85008128, 0x022102e2, 0x80100910,
        0x88100214, 0x08080604, 0x00201908, 0x80a05b04, 0x60402106, 0x00a03081,
        0x48130b84, 0x01061110, 0xa1a08084, 0x82108151, 0x00082008, 0x06128001,
        0x8641ed8a, 0x00048180, 0x40002204, 0x48088080, 0x10422040, 0x040a20a0,
        0x03128808, 0x000020e0, 0x24040822, 0x54018522, 0x00220950, 0x8a105108,
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
inline judkins_shogi::BitBoard judkins_shogi::Magic::attack_table_diagonal
    [judkins_shogi::Config::num_squares]
    [judkins_shogi::Config::magic_table_size]
    = {};

template <>
inline bool judkins_shogi::Board::is_square_attacked_by_ranging_pieces(
    const ColorEnum& by_side,
    const judkins_shogi::SquareEnum& sq,
    const judkins_shogi::SquareEnum& skip) const
{
    using namespace judkins_shogi;
    const BitBoardType occ_full = get_occupied().clear(skip);
    {
        const auto occ_offence = get_occupied<KA, UM>(by_side);
        if (occ_offence.any()) {
            const auto attack_inverted
                = judkins_shogi::Magic::get_diagonal_attack(sq, occ_full);
            if ((attack_inverted & occ_offence).any())
                return true;
        }
    }
    {
        const auto occ_offence = get_occupied<HI, RY>(by_side);
        if (occ_offence.any()) {
            const auto attack_inverted
                = judkins_shogi::Magic::get_adjacent_attack(sq, occ_full);
            if ((attack_inverted & occ_offence).any())
                return true;
        }
    }
    return false;
}

template <>
inline bool judkins_shogi::Board::is_square_attacked(
    const ColorEnum& by_side,
    const judkins_shogi::SquareEnum& sq,
    const judkins_shogi::SquareEnum& skip) const
{
    assert(sq != C::SQ_NA);
    using namespace judkins_shogi;
    return is_square_attacked_by<FU>(by_side, sq)
           || is_square_attacked_by<KE>(by_side, sq)
           || is_square_attacked_by<GI>(by_side, sq)
           || is_square_attacked_by<KI, TO, NK, NG>(by_side, sq)
           || is_square_attacked_by<OU, UM, RY>(by_side, sq)
           || is_square_attacked_by_ranging_pieces(by_side, sq, skip);
}
template <>
inline judkins_shogi::BitBoard
judkins_shogi::Board::get_occupied_by_ranging(const ColorEnum& c) const
{
    using namespace judkins_shogi;
    return get_occupied<KA, HI, UM, RY>(c);
}

template <>
inline judkins_shogi::Move
NonKingBoardMoveGenerator<judkins_shogi::Parameters>::random_select()
{
    using namespace judkins_shogi;
    const auto src_fgke = m_board.get_occupied<FU, GI, KE>(m_turn);
    const auto src_kahi = m_board.get_occupied<KA, HI>(m_turn);
    const auto src_gold = m_board.get_occupied<KI, TO, NK, NG>(m_turn);
    const auto src_umry = m_board.get_occupied<UM, RY>(m_turn);
    auto iter_fgke
        = NonKingBoardMoveGenerator<Parameters>(m_state, src_fgke, m_pinned);
    auto iter_kahi
        = NonKingBoardMoveGenerator<Parameters>(m_state, src_kahi, m_pinned);
    auto iter_gold
        = NoPromoMoveGenerator<Parameters>(m_state, src_gold, m_pinned);
    auto iter_umry
        = NoPromoMoveGenerator<Parameters>(m_state, src_umry, m_pinned);
    const auto num_fgke = iter_fgke.is_end()
                              ? 0.f
                              : static_cast<float>(src_fgke.hamming_weight());
    const auto num_kahi = iter_kahi.is_end()
                              ? 0.f
                              : static_cast<float>(src_kahi.hamming_weight());
    const auto num_gold = iter_gold.is_end()
                              ? 0.f
                              : static_cast<float>(src_gold.hamming_weight());
    const auto num_umry = iter_umry.is_end()
                              ? 0.f
                              : static_cast<float>(src_umry.hamming_weight());
    const auto num_src = num_fgke + num_kahi + num_gold + num_umry;
    float r = dist01(random_engine);
    const auto fraction_fgke = num_fgke / num_src;
    if (r < fraction_fgke)
        return iter_fgke.random_select_by_iterating_all();
    r -= fraction_fgke;
    const auto fraction_kahi = num_kahi / num_src;
    if (r < fraction_kahi)
        return iter_kahi.random_select_by_iterating_all();
    r -= fraction_kahi;
    const auto fraction_gold = num_gold / num_src;
    if (r < fraction_gold)
        return iter_gold.random_select();
    return iter_umry.random_select();
}

} // namespace vshogi

#endif // VSHOGI_VARIANTS_JUDKINS_SHOGI_HPP
