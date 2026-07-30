#ifndef VSHOGI_VARIANTS_LESHOGI_HPP
#define VSHOGI_VARIANTS_LESHOGI_HPP

#include <array>
#include <cstdint>

#include "vshogi/common/bitboard_traits.hpp"
#include "vshogi/common/board.hpp"
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
#include "vshogi/common/utils.hpp"

/**
 * @brief
 * https://www.logygames.com/leshogi/leshogi.html
 */
namespace vshogi::leshogi
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

enum SquareEnum : uint
{
    // clang-format off
    SQ_4A=18, SQ_3A=12, SQ_2A= 6, SQ_1A=0,
    SQ_4B=19, SQ_3B=13, SQ_2B= 7, SQ_1B=1,
    SQ_4C=20, SQ_3C=14, SQ_2C= 8, SQ_1C=2,
    SQ_4D=21, SQ_3D=15, SQ_2D= 9, SQ_1D=3,
    SQ_4E=22, SQ_3E=16, SQ_2E=10, SQ_1E=4,
    SQ_4F=23, SQ_3F=17, SQ_2F=11, SQ_1F=5,
    // clang-format on
    SQ_NA = 24,
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
};

struct Parameters
{
    // clang-format off
    static constexpr std::array<FullPieceTypes, 15u> piece_types = {PT_FU, PT_KY, PT_KE, PT_GI, PT_KA, PT_HI, PT_KI, PT_OU, PT_TO, PT_NY, PT_NK, PT_NG, PT_UM, PT_RY, PT_NA};
    static constexpr uint num_stand_piece_types = 7; // FU, KY, KE, GI, KA, HI, KI
    static constexpr uint num_files = 4; // 1, 2, 3, 4
    static constexpr uint num_ranks = 6; // A, B, C, D, E, F
    static constexpr uint num_promotion_ranks = 2;
    static constexpr uint max_stand_sfen_length = 14; // "plnsbrgPLNSBRG"
    static constexpr uint nfold_repetitions = 4;
    static constexpr std::array<PieceEnum, 24> initial_position = {
        W_KY, W_KI, VOID, VOID, B_FU, B_GI,
        W_KE, W_KA, VOID, VOID, B_HI, B_OU,
        W_OU, W_HI, VOID, VOID, B_KA, B_KE,
        W_GI, W_FU, VOID, VOID, B_KI, B_KY,
    };
    using BaseTypeBitBoard = uint32_t;
    using BaseTypeStand = std::uint32_t; // ________ ____**_* *_**_**_ **_**_** (KI, HI, KA, GI, KE, KY, FU)
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
static_assert(Config::initial_piece_count[0] == 2u); // FU
static_assert(Config::initial_piece_count[1] == 2u); // KY
static_assert(Config::initial_piece_count[2] == 2u); // KE
static_assert(Config::initial_piece_count[3] == 2u); // GI
static_assert(Config::initial_piece_count[4] == 2u); // KA
static_assert(Config::initial_piece_count[5] == 2u); // HI
static_assert(Config::initial_piece_count[6] == 2u); // KI
static_assert(Config::initial_piece_count[7] == 2u); // OU
static_assert(Config::max_stand_piece_count == 2u);
static_assert(Config::num_dir == 12u);
static_assert(Config::num_dir_dl == 10u);

} // namespace vshogi::leshogi

namespace vshogi
{

template <>
inline const uint leshogi::Stand::shift_bits[] = {0, 3, 6, 9, 12, 15, 18};
template <>
inline const std::uint32_t leshogi::Stand::masks[] = {
    // clang-format off
    0b000000000000000000000011, // FU
    0b000000000000000000011000, // KY
    0b000000000000000011000000, // KE
    0b000000000000011000000000, // GI
    0b000000000011000000000000, // KA
    0b000000011000000000000000, // HI
    0b000011000000000000000000, // KI
    // clang-format on
};
template <>
inline const std::uint32_t leshogi::Stand::deltas[] = {
    // clang-format off
    0b000000000000000000000001, // FU
    0b000000000000000000001000, // KY
    0b000000000000000001000000, // KE
    0b000000000000001000000000, // GI
    0b000000000001000000000000, // KA
    0b000000001000000000000000, // HI
    0b000001000000000000000000, // KI
    // clang-format on
};
template <>
inline const std::uint32_t leshogi::Stand::mask = 0b000011011011011011011011;

template <>
inline const leshogi::PieceTypeEnum
    Notation<leshogi::Parameters>::pieces_in_sfen_order[]
    = {leshogi::HI,
       leshogi::KA,
       leshogi::KI,
       leshogi::GI,
       leshogi::KE,
       leshogi::KY,
       leshogi::FU};

template <>
inline std::uint64_t leshogi::BlackWhiteStands::zobrist_table
    [num_colors][leshogi::Config::num_stand_piece_types]
    [leshogi::Config::max_stand_piece_count + 1]
    = {};

template <>
template <>
inline leshogi::Stand::Stand(
    const int num_fu,
    const int num_ky,
    const int num_ke,
    const int num_gi,
    const int num_ka,
    const int num_hi,
    const int num_ki)
    : Stand(
          static_cast<std::uint32_t>(
              (num_fu << shift_bits[leshogi::FU])
              + (num_ky << shift_bits[leshogi::KY])
              + (num_ke << shift_bits[leshogi::KE])
              + (num_gi << shift_bits[leshogi::GI])
              + (num_ka << shift_bits[leshogi::KA])
              + (num_hi << shift_bits[leshogi::HI])
              + (num_ki << shift_bits[leshogi::KI])))
{
}

template <>
inline const std::uint32_t leshogi::Magic::premask_north[leshogi::SQ_NA] = {
    0x00000000, 0x00000000, 0x00000002, 0x00000006, 0x0000000e, 0x0000001e,
    0x00000000, 0x00000000, 0x00000080, 0x00000180, 0x00000380, 0x00000780,
    0x00000000, 0x00000000, 0x00002000, 0x00006000, 0x0000e000, 0x0001e000,
    0x00000000, 0x00000000, 0x00080000, 0x00180000, 0x00380000, 0x00780000,
};
template <>
inline const std::uint32_t leshogi::Magic::premask_south[leshogi::SQ_NA] = {
    0x0000001e, 0x0000001c, 0x00000018, 0x00000010, 0x00000000, 0x00000000,
    0x00000780, 0x00000700, 0x00000600, 0x00000400, 0x00000000, 0x00000000,
    0x0001e000, 0x0001c000, 0x00018000, 0x00010000, 0x00000000, 0x00000000,
    0x00780000, 0x00700000, 0x00600000, 0x00400000, 0x00000000, 0x00000000,
};
template <>
inline const std::uint32_t
    leshogi::Magic::premask_adjacent[leshogi::Config::num_squares]
    = {
        // clang-format off
        0x0000105e, 0x0000209c, 0x0000411a, 0x00008216,
        0x0001040e, 0x0002081e, 0x00001780, 0x00002700,
        0x00004680, 0x00008580, 0x00010380, 0x00020780,
        0x0001e040, 0x0001c080, 0x0001a100, 0x00016200,
        0x0000e400, 0x0001e800, 0x00781040, 0x00702080,
        0x00684100, 0x00588200, 0x00390400, 0x007a0800,
        // clang-format on
};
template <>
inline const std::uint32_t
    leshogi::Magic::premask_diagonal[leshogi::Config::num_squares]
    = {
        // clang-format off
        0x00004080, 0x00008100, 0x00010280, 0x00002500,
        0x00004200, 0x00008400, 0x00002000, 0x00004000,
        0x0000a000, 0x00014000, 0x00008000, 0x00010000,
        0x00000080, 0x00000100, 0x00000280, 0x00000500,
        0x00000200, 0x00000400, 0x00002100, 0x00004200,
        0x0000a400, 0x00014080, 0x00008100, 0x00010200,
        // clang-format on
};
template <>
inline const std::uint32_t
    leshogi::Magic::magic_number_north[leshogi::Config::num_squares]
    = {
        0x00420041, 0x50881008, 0x90010000, 0x0a00802a, 0x10000001, 0x08008838,
        0x02430108, 0xd0002820, 0x01000110, 0x40800022, 0x80400c08, 0x28209001,
        0x00003200, 0x42040000, 0x00848400, 0x02020527, 0x01012010, 0x080a8080,
        0x4800200c, 0x00325820, 0x00102e08, 0x24100840, 0x20100260, 0x10000442,
};
template <>
inline const std::uint32_t
    leshogi::Magic::magic_number_south[leshogi::Config::num_squares]
    = {
        0x48020400, 0x46004000, 0x0200a50d, 0x0488c404, 0x10039100, 0x00108c90,
        0x24a10122, 0x80212008, 0x2008c040, 0x00528100, 0x00440000, 0x40100000,
        0x00118100, 0x4a10a004, 0x61009010, 0x00008480, 0x8118a020, 0x00020004,
        0x00200202, 0x20330c50, 0x02200200, 0x04200090, 0x29000400, 0x08000120,
};
template <>
inline const std::uint32_t
    leshogi::Magic::magic_number_adjacent[leshogi::Config::num_squares]
    = {
        // clang-format off
        0x42040000, 0x070c0200, 0x080a8080, 0x24100840,
        0x48020400, 0x08170250, 0x01041008, 0x00040000,
        0x48198000, 0x00110000, 0x40108204, 0x04883090,
        0x22082404, 0x420a8120, 0x40054000, 0x10182108,
        0x00016020, 0x01702800, 0x020c0180, 0x00883080,
        0x00401040, 0x00100902, 0x03088140, 0x08011200,
        // clang-format on
};
template <>
inline const std::uint32_t
    leshogi::Magic::magic_number_diagonal[leshogi::Config::num_squares]
    = {
        // clang-format off
        0x00288210, 0x00c03000, 0x5020601c, 0x80441080,
        0x52288008, 0x00488340, 0x08129600, 0x01120030,
        0x20210000, 0x00048000, 0x00102005, 0x4010800a,
        0x01201004, 0x00c00024, 0x50400104, 0x02200041,
        0x40422120, 0x00e04102, 0x01410004, 0x0008a241,
        0x00110014, 0x012440a0, 0x40402600, 0x02084020,
        // clang-format on
};
template <>
inline leshogi::bitboard_t
    leshogi::Magic::attack_table_north[leshogi::SQ_NA]
                                      [leshogi::Magic::table_size_lance]
    = {};
template <>
inline leshogi::bitboard_t
    leshogi::Magic::attack_table_south[leshogi::SQ_NA]
                                      [leshogi::Magic::table_size_lance]
    = {};
template <>
inline leshogi::bitboard_t
    leshogi::Magic::attack_table_adjacent[leshogi::SQ_NA]
                                         [leshogi::Magic::table_size_adjacent]
    = {};
template <>
inline leshogi::bitboard_t
    leshogi::Magic::attack_table_diagonal[leshogi::SQ_NA]
                                         [leshogi::Magic::table_size_diagonal]
    = {};

template <>
inline leshogi::bitboard_t
    leshogi::BitboardTraits::table_attacks[leshogi::VOID + 1u]
                                          [leshogi::SQ_NA + 1u]
    = {};
template <>
inline leshogi::bitboard_t
    leshogi::BitboardTraits::table_pre_reverse_attack[leshogi::SQ_NA + 1u]
                                                     [leshogi::VOID + 1u]
    = {};
template <>
inline leshogi::bitboard_t
    leshogi::BitboardTraits::table_ray[leshogi::SQ_NA + 1u][DIR_NA + 1u]
    = {};
template <>
inline leshogi::bitboard_t
    leshogi::BitboardTraits::table_mask_between[leshogi::SQ_NA + 1u]
                                               [leshogi::SQ_NA + 1u]
    = {};

template <>
inline leshogi::bitboard_t
leshogi::BitboardTraits::get_placeable(const leshogi::PieceEnum& p)
{
    constexpr bitboard_t table[C::num_colored_piece_types + 1u] = {
        mask & (~top_n_rank<1u>()), // B_FU
        mask & (~top_n_rank<1u>()), // B_KY
        mask & (~top_n_rank<2u>()), // B_KE
        mask, // B_GI
        mask, // B_KA
        mask, // B_HI
        mask, // B_KI
        mask, // B_OU
        mask, // B_TO
        mask, // B_NY
        mask, // B_NK
        mask, // B_NG
        mask, // B_UM
        mask, // B_RY
        mask & top_n_rank<6u - 1u>(), // W_FU
        mask & top_n_rank<6u - 1u>(), // W_KY
        mask & top_n_rank<6u - 2u>(), // W_KE
        mask, // W_GI
        mask, // W_KA
        mask, // W_HI
        mask, // W_KI
        mask, // W_OU
        mask, // W_TO
        mask, // W_NY
        mask, // W_NK
        mask, // W_NG
        mask, // W_UM
        mask, // W_RY
        mask, // VOID
    };
    return table[p];
}

template <>
inline leshogi::bitboard_t leshogi::BitboardTraits::get_attack_by(
    const leshogi::PieceEnum& p,
    const leshogi::SquareEnum& sq,
    const leshogi::bitboard_t& occupied)
{
    switch (p) {
    case leshogi::B_KY:
        return leshogi::Magic::get_north_attack(sq, occupied);
    case leshogi::W_KY:
        return leshogi::Magic::get_south_attack(sq, occupied);
    case leshogi::B_KA:
    case leshogi::W_KA:
        return leshogi::Magic::get_diagonal_attack(sq, occupied);
    case leshogi::B_HI:
    case leshogi::W_HI:
        return leshogi::Magic::get_adjacent_attack(sq, occupied);
    case leshogi::B_UM:
    case leshogi::W_UM:
        return leshogi::Magic::get_diagonal_attack(sq, occupied)
               | leshogi::BitboardTraits::get_attack_by(leshogi::B_OU, sq);
    case leshogi::B_RY:
    case leshogi::W_RY:
        return leshogi::Magic::get_adjacent_attack(sq, occupied)
               | leshogi::BitboardTraits::get_attack_by(leshogi::B_OU, sq);
    default:
        return get_attack_by(p, sq);
    }
}

template <>
inline std::uint64_t leshogi::Board::zobrist_table
    [leshogi::SQ_NA][num_colors * leshogi::Config::num_piece_types + 1]
    = {};

} // namespace vshogi

#endif // VSHOGI_VARIANTS_LESHOGI_HPP
