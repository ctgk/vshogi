#ifndef VSHOGI_VARIANTS_MINISHOGI_HPP
#define VSHOGI_VARIANTS_MINISHOGI_HPP

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

enum ColoredPieceEnum : std::uint8_t
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
 * @details
 *   5   4   3   2   1
 * +---+---+---+---+---+
 * | 0 | 1 | 2 | 3 | 4 | a (1)
 * +---+---+---+---+---+
 * | 5 | 6 | 7 | 8 | 9 | b (2)
 * +---+---+---+---+---+
 * | 10| 11| 12| 13| 14| c (3)
 * +---+---+---+---+---+
 * | 15| 16| 17| 18| 19| d (4)
 * +---+---+---+---+---+
 * | 20| 21| 22| 23| 24| e (5)
 * +---+---+---+---+---+
 */
enum SquareEnum : uint
{
    // clang-format off
    SQ_5A =  0, SQ_4A =  1, SQ_3A =  2, SQ_2A =  3, SQ_1A =  4,
    SQ_5B =  5, SQ_4B =  6, SQ_3B =  7, SQ_2B =  8, SQ_1B =  9,
    SQ_5C = 10, SQ_4C = 11, SQ_3C = 12, SQ_2C = 13, SQ_1C = 14,
    SQ_5D = 15, SQ_4D = 16, SQ_3D = 17, SQ_2D = 18, SQ_1D = 19,
    SQ_5E = 20, SQ_4E = 21, SQ_3E = 22, SQ_2E = 23, SQ_1E = 24,
    // clang-format on
    SQ_NA,
    NUM_SQ = 25,
};
enum FileEnum : uint
{
    FILE5 = 0,
    FILE4,
    FILE3,
    FILE2,
    FILE1,
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
    static constexpr uint num_dir_dl = 8; //!< NW, N, NE, W, E, SW, S, SE
    static constexpr uint log2_magic_table_size = 4;
    static constexpr uint max_stand_piece_count = 2;
    static constexpr uint max_stand_sfen_length = 11; // "2p2s2g2b2r "
    static constexpr uint max_acceptable_repetitions = 3;
    static constexpr uint num_init_piece_each = 6;
    static constexpr uint initial_points = 13;
    using BaseTypeBitBoard = std::uint32_t;
    using BaseTypeStand = std::uint16_t; // __11_11_ 11_11_11 (KI, HI, KA, GI, FU)
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
constexpr BitBoard bb_2a = (BitBoard(1) << static_cast<uint>(SQ_2A));
constexpr BitBoard bb_2b = (BitBoard(1) << static_cast<uint>(SQ_2B));
constexpr BitBoard bb_2c = (BitBoard(1) << static_cast<uint>(SQ_2C));
constexpr BitBoard bb_2d = (BitBoard(1) << static_cast<uint>(SQ_2D));
constexpr BitBoard bb_2e = (BitBoard(1) << static_cast<uint>(SQ_2E));
constexpr BitBoard bb_3a = (BitBoard(1) << static_cast<uint>(SQ_3A));
constexpr BitBoard bb_3b = (BitBoard(1) << static_cast<uint>(SQ_3B));
constexpr BitBoard bb_3c = (BitBoard(1) << static_cast<uint>(SQ_3C));
constexpr BitBoard bb_3d = (BitBoard(1) << static_cast<uint>(SQ_3D));
constexpr BitBoard bb_3e = (BitBoard(1) << static_cast<uint>(SQ_3E));
constexpr BitBoard bb_4a = (BitBoard(1) << static_cast<uint>(SQ_4A));
constexpr BitBoard bb_4b = (BitBoard(1) << static_cast<uint>(SQ_4B));
constexpr BitBoard bb_4c = (BitBoard(1) << static_cast<uint>(SQ_4C));
constexpr BitBoard bb_4d = (BitBoard(1) << static_cast<uint>(SQ_4D));
constexpr BitBoard bb_4e = (BitBoard(1) << static_cast<uint>(SQ_4E));
constexpr BitBoard bb_5a = (BitBoard(1) << static_cast<uint>(SQ_5A));
constexpr BitBoard bb_5b = (BitBoard(1) << static_cast<uint>(SQ_5B));
constexpr BitBoard bb_5c = (BitBoard(1) << static_cast<uint>(SQ_5C));
constexpr BitBoard bb_5d = (BitBoard(1) << static_cast<uint>(SQ_5D));
constexpr BitBoard bb_5e = (BitBoard(1) << static_cast<uint>(SQ_5E));

constexpr BitBoard bb_ranka = bb_1a | bb_2a | bb_3a | bb_4a | bb_5a;
constexpr BitBoard bb_rankb = bb_1b | bb_2b | bb_3b | bb_4b | bb_5b;
constexpr BitBoard bb_rankc = bb_1c | bb_2c | bb_3c | bb_4c | bb_5c;
constexpr BitBoard bb_rankd = bb_1d | bb_2d | bb_3d | bb_4d | bb_5d;
constexpr BitBoard bb_ranke = bb_1e | bb_2e | bb_3e | bb_4e | bb_5e;

constexpr BitBoard bb_file1 = bb_1a | bb_1b | bb_1c | bb_1d | bb_1e;
constexpr BitBoard bb_file2 = bb_2a | bb_2b | bb_2c | bb_2d | bb_2e;
constexpr BitBoard bb_file3 = bb_3a | bb_3b | bb_3c | bb_3d | bb_3e;
constexpr BitBoard bb_file4 = bb_4a | bb_4b | bb_4c | bb_4d | bb_4e;
constexpr BitBoard bb_file5 = bb_5a | bb_5b | bb_5c | bb_5d | bb_5e;

} // namespace vshogi::minishogi

namespace vshogi
{

template <>
inline const uint minishogi::Stand::shift_bits[] = {0, 3, 6, 9, 12};

template <>
inline const std::uint16_t minishogi::Stand::masks[] = {
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
template <>
inline minishogi::Stand::Stand(
    const int num_fu,
    const int num_gi,
    const int num_ka,
    const int num_hi,
    const int num_ki)
    : Stand(static_cast<std::uint16_t>(
        (num_ki << shift_bits[minishogi::KI])
        + (num_hi << shift_bits[minishogi::HI])
        + (num_ka << shift_bits[minishogi::KA])
        + (num_gi << shift_bits[minishogi::GI])
        + (num_fu << shift_bits[minishogi::FU])))
{
}

template <>
inline const minishogi::PieceTypeEnum
    minishogi::BlackWhiteStands::stand_pieces_in_sfen_order[]
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
    [minishogi::Config::num_squares]
    [num_colors * minishogi::Config::num_piece_types + 1]
    = {};

template <>
inline minishogi::BitBoard minishogi::BitBoard::attacks_table
    [minishogi::Config::num_colored_piece_types][minishogi::Config::num_squares]
    = {};
template <>
inline minishogi::BitBoard
    minishogi::BitBoard::ray_table[minishogi::Config::num_squares]
                                  [minishogi::Config::num_dir]
    = {};
template <>
inline minishogi::BitBoard
    minishogi::BitBoard::line_segment_table[minishogi::Config::num_squares]
                                           [minishogi::Config::num_squares]
    = {};

template <>
inline minishogi::BitBoard minishogi::BitBoard::get_attacks_by(
    const vshogi::minishogi::ColoredPieceEnum& p,
    const vshogi::minishogi::SquareEnum& sq,
    const vshogi::minishogi::BitBoard& occupied)
{
    switch (p) {
    case minishogi::B_KA:
    case minishogi::W_KA:
        return minishogi::Magic::get_diagonal_attack(sq, occupied);
    case minishogi::B_HI:
    case minishogi::W_HI:
        return minishogi::Magic::get_adjacent_attack(sq, occupied);
    case minishogi::B_UM:
    case minishogi::W_UM:
        return minishogi::Magic::get_diagonal_attack(sq, occupied)
               | attacks_table[minishogi::B_OU][sq];
    case minishogi::B_RY:
    case minishogi::W_RY:
        return minishogi::Magic::get_adjacent_attack(sq, occupied)
               | attacks_table[minishogi::B_OU][sq];
    default:
        return get_attacks_by(p, sq);
    }
}

template <>
inline const minishogi::BitBoard
    minishogi::Magic::premask_vertical[minishogi::Config::num_squares]
    = {
        0x00008420, 0x00010840, 0x00021080, 0x00042100, 0x00084200,
        0x00008400, 0x00010800, 0x00021000, 0x00042000, 0x00084000,
        0x00008020, 0x00010040, 0x00020080, 0x00040100, 0x00080200,
        0x00000420, 0x00000840, 0x00001080, 0x00002100, 0x00004200,
        0x00008420, 0x00010840, 0x00021080, 0x00042100, 0x00084200,
};
template <>
inline const minishogi::BitBoard
    minishogi::Magic::premask_horizontal[minishogi::Config::num_squares]
    = {
        0x0000000e, 0x0000000c, 0x0000000a, 0x00000006, 0x0000000e,
        0x000001c0, 0x00000180, 0x00000140, 0x000000c0, 0x000001c0,
        0x00003800, 0x00003000, 0x00002800, 0x00001800, 0x00003800,
        0x00070000, 0x00060000, 0x00050000, 0x00030000, 0x00070000,
        0x00e00000, 0x00c00000, 0x00a00000, 0x00600000, 0x00e00000,
};
template <>
inline const minishogi::BitBoard
    minishogi::Magic::premask_diagonal[minishogi::Config::num_squares]
    = {
        0x00041040, 0x00002080, 0x00000140, 0x00000880, 0x00011100,
        0x00020800, 0x00041000, 0x00002800, 0x00011000, 0x00022000,
        0x00010040, 0x00020080, 0x00050140, 0x00020080, 0x00040100,
        0x00000880, 0x00001100, 0x00002800, 0x00001040, 0x00002080,
        0x00011100, 0x00022000, 0x00050000, 0x00020800, 0x00041040,
};
template <>
inline const std::uint32_t
    minishogi::Magic::magic_number_vertical[minishogi::Config::num_squares]
    = {
        0x01208480, 0x01102004, 0x00811600, 0x28209001, 0x00325820,
        0x00102e08, 0x01822001, 0x51288900, 0x0080d000, 0x00d21400,
        0x0200a004, 0x10808000, 0x208028c2, 0x0488c404, 0x00108c90,
        0x24a10122, 0x42020002, 0x10414020, 0x00440000, 0x00410000,
        0x8118a020, 0x04828800, 0x00a24008, 0x82111400, 0x002c6908,
};
template <>
inline const std::uint32_t
    minishogi::Magic::magic_number_horizontal[minishogi::Config::num_squares]
    = {
        0x8842842c, 0x04100100, 0x10020004, 0xa0400000, 0x08900200,
        0x00c80800, 0x02240001, 0x22300a40, 0x5944080a, 0x03610010,
        0x00020420, 0x40410008, 0x00248220, 0x40268080, 0x00112080,
        0x40049040, 0x18a21080, 0x080c3a14, 0x40408a04, 0x00002000,
        0x08020080, 0x64801091, 0x08000428, 0x60010524, 0x00840240,
};
template <>
inline const std::uint32_t
    minishogi::Magic::magic_number_diagonal[minishogi::Config::num_squares]
    = {
        0x42c22000, 0x8084c482, 0x00824000, 0x88420080, 0x0412c050,
        0x4002c000, 0x00028402, 0x86040008, 0x0288900a, 0x20020888,
        0x00422000, 0x40201801, 0x00881000, 0x0128a400, 0x00102000,
        0x00291120, 0x04144900, 0x00040100, 0x00814400, 0x404400a4,
        0x10218802, 0x00020d48, 0x00022a02, 0x892c0802, 0x0064a200,
};
template <>
inline minishogi::BitBoard
    minishogi::Magic::attack_table_vertical[minishogi::Config::num_squares]
                                           [minishogi::Config::magic_table_size]
    = {};
template <>
inline minishogi::BitBoard minishogi::Magic::attack_table_horizontal
    [minishogi::Config::num_squares][minishogi::Config::magic_table_size]
    = {};
template <>
inline minishogi::BitBoard
    minishogi::Magic::attack_table_diagonal[minishogi::Config::num_squares]
                                           [minishogi::Config::magic_table_size]
    = {};

template <>
inline bool minishogi::Board::is_square_attacked_by_ranging_pieces(
    const ColorEnum& by_side, const Square& sq, const Square& skip) const
{
    using namespace minishogi;
    const BitBoardType occ_full = get_occupied().clear(skip);
    {
        const auto attack_inverted
            = minishogi::Magic::get_diagonal_attack(sq, occ_full);
        const auto occ_offence = get_occupied<KA, UM>(by_side);
        if ((attack_inverted & occ_offence).any())
            return true;
    }
    {
        const auto attack_inverted
            = minishogi::Magic::get_adjacent_attack(sq, occ_full);
        const auto occ_offence = get_occupied<HI, RY>(by_side);
        if ((attack_inverted & occ_offence).any())
            return true;
    }
    return false;
}

template <>
inline bool minishogi::Board::is_square_attacked(
    const ColorEnum& by_side, const Square& sq, const Square& skip) const
{
    using namespace minishogi;
    return is_square_attacked_by<FU>(by_side, sq)
           || is_square_attacked_by<GI>(by_side, sq)
           || is_square_attacked_by<KI, TO, NG>(by_side, sq)
           || is_square_attacked_by<OU, UM, RY>(by_side, sq)
           || is_square_attacked_by_ranging_pieces(by_side, sq, skip);
}
template <>
inline minishogi::BitBoard
minishogi::Board::get_occupied_by_ranging(const ColorEnum& c) const
{
    using namespace minishogi;
    return get_occupied<KA, HI, UM, RY>(c);
}

template <>
inline minishogi::Move
NonKingBoardMoveGenerator<minishogi::Parameters>::random_select()
{
    using namespace minishogi;
    const auto src_fugi = m_board.get_occupied<FU, GI>(m_turn);
    const auto src_kahi = m_board.get_occupied<KA, HI>(m_turn);
    const auto src_gold = m_board.get_occupied<KI, TO, NG>(m_turn);
    const auto src_umry = m_board.get_occupied<UM, RY>(m_turn);
    auto iter_fugi = NonKingBoardMoveGenerator(m_state, src_fugi, m_pinned);
    auto iter_kahi = NonKingBoardMoveGenerator(m_state, src_kahi, m_pinned);
    auto iter_gold
        = NoPromoMoveGenerator<Parameters>(m_state, src_gold, m_pinned);
    auto iter_umry
        = NoPromoMoveGenerator<Parameters>(m_state, src_umry, m_pinned);
    const auto num_fugi = iter_fugi.is_end()
                              ? 0.f
                              : static_cast<float>(src_fugi.hamming_weight());
    const auto num_kahi = iter_kahi.is_end()
                              ? 0.f
                              : static_cast<float>(src_kahi.hamming_weight());
    const auto num_gold = iter_gold.is_end()
                              ? 0.f
                              : static_cast<float>(src_gold.hamming_weight());
    const auto num_umry = iter_umry.is_end()
                              ? 0.f
                              : static_cast<float>(src_umry.hamming_weight());
    const auto num_src = num_fugi + num_kahi + num_gold + num_umry;
    float r = dist01(random_engine);
    const auto fraction_fugi = num_fugi / num_src;
    if (r < fraction_fugi)
        return iter_fugi.random_select_by_iterating_all();
    r -= fraction_fugi;
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

#endif // VSHOGI_VARIANTS_MINISHOGI_HPP
