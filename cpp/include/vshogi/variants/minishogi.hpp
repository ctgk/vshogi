#ifndef VSHOGI_VARIANTS_MINISHOGI_HPP
#define VSHOGI_VARIANTS_MINISHOGI_HPP

#include <array>
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
    NA = 0b01111, //!< Not available
};

enum BoardPieceTypeEnum : std::uint8_t
{
    B_FU = FU, //!< Black Fu (Pawn)
    B_GI = GI, //!< Black Gin (Silver)
    B_KA = KA, //!< Black Kaku (Bishop)
    B_HI = HI, //!< Black Hisha (Rook)
    B_KI = KI, //!< Black Kin (Gold)
    B_OU = OU, //!< Black Ou, Gyoku (King)
    B_TO = TO, //!< Black Tokin (Promoted Pawn)
    B_NG = NG, //!< Black Nari-Gin (Promoted Silver)
    B_UM = UM, //!< Black Uma (Promoted Bishop)
    B_RY = RY, //!< Black Ryu (Promoted Rook)

    W_FU = 0b10000 + FU, //!< White Fu (Pawn)
    W_GI = 0b10000 + GI, //!< White Gin (Silver)
    W_KA = 0b10000 + KA, //!< White Kaku (Bishop)
    W_HI = 0b10000 + HI, //!< White Hisha (Rook)
    W_KI = 0b10000 + KI, //!< White Kin (Gold)
    W_OU = 0b10000 + OU, //!< White Ou, Gyoku (King)
    W_TO = 0b10000 + TO, //!< White Tokin (Promoted Pawn)
    W_NG = 0b10000 + NG, //!< White Nari-Gin (Promoted Silver)
    W_UM = 0b10000 + UM, //!< White Uma (Promoted Bishop)
    W_RY = 0b10000 + RY, //!< White Ryu (Promoted Rook)

    VOID = 0b11111, //!< Empty Square
};

/**
 * @brief Enumeration of board squares following SFEN.
 * @details
 *   5   4   3   2   1
 * +---+---+---+---+---+
 * | 0 | 1 | 2 | 3 | 4 | a
 * +---+---+---+---+---+
 * | 5 | 6 | 7 | 8 | 9 | b
 * +---+---+---+---+---+
 * | 10| 11| 12| 13| 14| c
 * +---+---+---+---+---+
 * | 15| 16| 17| 18| 19| d
 * +---+---+---+---+---+
 * | 20| 21| 22| 23| 24| e
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
    FILE1,
    FILE2,
    FILE3,
    FILE4,
    FILE5,
};
enum RankEnum : uint
{
    RANK1,
    RANK2,
    RANK3,
    RANK4,
    RANK5,
};

struct Config
{
    // clang-format off
    static constexpr uint num_piece_types = 10; // FU, GI, KI, KA, HI, OU, TO, NG, UM, RY
    static constexpr uint num_stand_piece_types = 5; // FU, GI, KI, KA, HI
    static constexpr uint color_bit = 4; // ___* ____
    static constexpr uint num_files = 5; // 1, 2, 3, 4, 5
    static constexpr uint num_ranks = 5; // A, B, C, D, E
    static constexpr uint num_promotion_ranks = 1;
    static constexpr uint num_dir = 8; //!< NW, N, NE, W, E, SW, S, SE
    static constexpr uint num_dir_dl = 8; //!< NW, N, NE, W, E, SW, S, SE
    static constexpr uint num_non_ranging_attacks = 7; // B_FU, W_FU, B_GI, W_GI, B_KI, W_KI, OU
    static constexpr uint num_attacks = 11; // B_FU, W_FU, B_GI, W_GI, B_KI, W_KI, OU, KA, HI, UM, RY
    static constexpr uint max_stand_piece_count = 2;
    static constexpr uint max_acceptable_repetitions = 3;
    using BaseTypeBitBoard = std::uint32_t;
    // clang-format on

    /**
     * @brief 16-bit integer representing pieces on a stand.
     * @details
     * ________ ______11  FU (2-bit for 0, 1, or 2 pieces)
     * ________ ___11___  GI (2-bit)
     * ________ 11______  KA (2-bit)
     * _____11_ ________  HI (2-bit)
     * __11____ ________  KI (2-bit)
     */
    using BaseTypeStand = std::uint16_t;

    Config() = delete;
    using PieceType = PieceTypeEnum;
    using BoardPieceType = BoardPieceTypeEnum;
    using Square = SquareEnum;
    using File = FileEnum;
    using Rank = RankEnum;
    static constexpr uint num_squares = num_files * num_ranks;
};

using Pieces = vshogi::Pieces<Config>;
using Squares = vshogi::Squares<Config>;
using Move = vshogi::Move<Config>;
using BitBoard = vshogi::BitBoard<Config>;
using Board = vshogi::Board<Config>;
using Stand = vshogi::Stand<Config>;
using BlackWhiteStands = vshogi::BlackWhiteStands<Config>;
using State = vshogi::State<Config>;
using Game = vshogi::Game<Config>;
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

inline SquareEnum operator--(SquareEnum& self, int)
{
    const auto out = self;
    self = static_cast<SquareEnum>(static_cast<int>(self) - 1);
    return out;
}

} // namespace vshogi::minishogi

namespace vshogi
{

template <>
inline const DirectionEnum
    minishogi::Pieces::attack_directions_table[2 * num_piece_types + 1][9]
    = {
        // clang-format off
{DIR_N,                                                      DIR_NA}, // B_FU
{DIR_NW, DIR_N, DIR_NE, DIR_SW, DIR_SE,                      DIR_NA}, // B_GI
{DIR_NW, DIR_NE, DIR_SW, DIR_SE,                             DIR_NA}, // B_KA
{DIR_N, DIR_W, DIR_E, DIR_S,                                 DIR_NA}, // B_HI
{DIR_NW, DIR_N, DIR_NE, DIR_W, DIR_E, DIR_S,                 DIR_NA}, // B_KI
{DIR_NW, DIR_N, DIR_NE, DIR_W, DIR_E, DIR_SW, DIR_S, DIR_SE, DIR_NA}, // B_OU
{DIR_NW, DIR_N, DIR_NE, DIR_W, DIR_E, DIR_S,                 DIR_NA}, // B_TO
{DIR_NW, DIR_N, DIR_NE, DIR_W, DIR_E, DIR_S,                 DIR_NA}, // B_NG
{DIR_NW, DIR_N, DIR_NE, DIR_W, DIR_E, DIR_SW, DIR_S, DIR_SE, DIR_NA}, // B_UM
{DIR_NW, DIR_N, DIR_NE, DIR_W, DIR_E, DIR_SW, DIR_S, DIR_SE, DIR_NA}, // B_RY
{DIR_S,                                                      DIR_NA}, // W_FU
{DIR_SE, DIR_S, DIR_SW, DIR_NE, DIR_NW,                      DIR_NA}, // W_GI
{DIR_SE, DIR_SW, DIR_NE, DIR_NW,                             DIR_NA}, // W_KA
{DIR_S, DIR_E, DIR_W, DIR_N,                                 DIR_NA}, // W_HI
{DIR_SE, DIR_S, DIR_SW, DIR_E, DIR_W, DIR_N,                 DIR_NA}, // W_KI
{DIR_SE, DIR_S, DIR_SW, DIR_E, DIR_W, DIR_NE, DIR_N, DIR_NW, DIR_NA}, // W_OU
{DIR_SE, DIR_S, DIR_SW, DIR_E, DIR_W, DIR_N,                 DIR_NA}, // W_TO
{DIR_SE, DIR_S, DIR_SW, DIR_E, DIR_W, DIR_N,                 DIR_NA}, // W_NG
{DIR_SE, DIR_S, DIR_SW, DIR_E, DIR_W, DIR_NE, DIR_N, DIR_NW, DIR_NA}, // W_UM
{DIR_SE, DIR_S, DIR_SW, DIR_E, DIR_W, DIR_NE, DIR_N, DIR_NW, DIR_NA}, // W_RY
        // clang-format on
};

template <>
inline const vshogi::minishogi::PieceTypeEnum
    vshogi::minishogi::Pieces::piece_array[]
    = {vshogi::minishogi::FU,
       vshogi::minishogi::GI,
       vshogi::minishogi::KA,
       vshogi::minishogi::HI,
       vshogi::minishogi::KI,
       vshogi::minishogi::OU,
       vshogi::minishogi::TO,
       vshogi::minishogi::NG,
       vshogi::minishogi::UM,
       vshogi::minishogi::RY};

template <>
inline const vshogi::minishogi::PieceTypeEnum
    vshogi::minishogi::Pieces::stand_piece_array[]
    = {vshogi::minishogi::FU,
       vshogi::minishogi::GI,
       vshogi::minishogi::KA,
       vshogi::minishogi::HI,
       vshogi::minishogi::KI};

template <>
inline vshogi::minishogi::PieceTypeEnum
vshogi::minishogi::Pieces::to_piece_type(const char c)
{
    using namespace vshogi::minishogi;
    switch (std::tolower(c)) {
    case 'p':
        return FU;
    case 's':
        return GI;
    case 'b':
        return KA;
    case 'r':
        return HI;
    case 'g':
        return KI;
    case 'k':
        return OU;
    default:
        return NA;
    }
}

template <>
inline char
vshogi::minishogi::Pieces::to_char(const vshogi::minishogi::PieceTypeEnum& p)
{
    constexpr char table[] = {'p', 's', 'b', 'r', 'g', 'k'};
    static_assert(0 == vshogi::minishogi::FU);
    static_assert(1 == vshogi::minishogi::GI);
    static_assert(2 == vshogi::minishogi::KA);
    static_assert(3 == vshogi::minishogi::HI);
    static_assert(4 == vshogi::minishogi::KI);
    static_assert(5 == vshogi::minishogi::OU);
    return table[p];
}

template <>
inline constexpr bool vshogi::minishogi::Pieces::is_promotable(
    const vshogi::minishogi::PieceTypeEnum& p)
{
    return (p < vshogi::minishogi::KI);
}

template <>
inline bool vshogi::minishogi::Pieces::is_ranging_to(
    const vshogi::minishogi::BoardPieceTypeEnum& p, const DirectionEnum& d)
{
    using namespace vshogi::minishogi;
    const auto base = demote(to_piece_type(p));
    if (base == HI)
        return (d == DIR_N) || (d == DIR_W) || (d == DIR_E) || (d == DIR_S);
    if (base == KA)
        return (d == DIR_NW) || (d == DIR_NE) || (d == DIR_SW) || (d == DIR_SE);
    return false;
}

template <>
inline uint
vshogi::minishogi::Pieces::get_point(const vshogi::minishogi::PieceTypeEnum& p)
{
    using namespace vshogi::minishogi;
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
inline bool
minishogi::Pieces::is_ranging_piece(const minishogi::PieceTypeEnum& pt)
{
    using namespace minishogi;
    return ((pt == KA) || (pt == HI) || (pt == UM) || (pt == RY));
}

template <>
inline bool vshogi::minishogi::Squares::in_promotion_zone(
    const vshogi::minishogi::RankEnum& r, const ColorEnum& c)
{
    return (c == BLACK) ? (r == vshogi::minishogi::RANK1)
                        : (r == vshogi::minishogi::RANK5);
}

template <>
inline DirectionEnum
vshogi::minishogi::Squares::get_direction_for_diagonal_or_knight(
    const vshogi::minishogi::SquareEnum& dst,
    const vshogi::minishogi::SquareEnum& src)
{
    using namespace vshogi::minishogi;
    switch (static_cast<int>(src - dst)) {
    case 6:
        return (to_file(src) > FILE4) ? DIR_NA : DIR_NW;
    // case 12:
    case 18:
        return (to_file(src) > FILE2) ? DIR_NA : DIR_NW;
    case 24:
        return DIR_NW;
    case 4:
        return DIR_NE;
    case 8:
        return (to_file(src) < FILE3) ? DIR_NA : DIR_NE;
    case 16:
        return (to_file(src) < FILE4) ? DIR_NA : DIR_NE;
    case 12:
        return (to_file(src) < vshogi::minishogi::FILE4) ? DIR_NW : DIR_NE;
    case -4:
        return DIR_SW;
    case -8:
        return (to_file(dst) < FILE3) ? DIR_NA : DIR_SW;
    case -16:
        return (to_file(dst) < FILE4) ? DIR_NA : DIR_SW;
    case -6:
        return (to_file(dst) > FILE4) ? DIR_NA : DIR_SE;
    case -18:
        return (to_file(dst) > FILE2) ? DIR_NA : DIR_SE;
    case -24:
        return DIR_SE;
    case -12:
        return (to_file(src) < vshogi::minishogi::FILE3) ? DIR_SW : DIR_SE;
    default:
        return DIR_NA;
    }
}

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
inline const int minishogi::BlackWhiteStands::max_sfen_length
    = 11; // "2p2s2g2b2r "

template <>
inline const minishogi::BitBoard minishogi::BitBoard::square_to_bitboard_array
    [minishogi::Config::num_squares + 1]
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
        BitBoard(),
};

template <>
inline minishogi::BitBoard
    minishogi::BitBoard::attacks_table[minishogi::Config::num_attacks]
                                      [minishogi::Config::num_squares]
    = {};

template <>
inline uint minishogi::BitBoard::hamming_weight() const
{
    // https://en.wikipedia.org/wiki/Hamming_weight
    constexpr std::uint32_t m1 = 0x55555555;
    constexpr std::uint32_t m2 = 0x33333333;
    constexpr std::uint32_t m4 = 0x0f0f0f0f;
    auto x = m_value;
    x -= (x >> 1U) & m1;
    x = (x & m2) + ((x >> 2) & m2);
    x = (x + (x >> 4)) & m4;
    x += x >> 8;
    x += x >> 16;
    return x & 0x7f;
}

template <>
inline minishogi::BitBoard minishogi::BitBoard::get_attacks_by(
    const vshogi::minishogi::BoardPieceTypeEnum& p,
    const vshogi::minishogi::SquareEnum& sq)
{
    using namespace vshogi::minishogi;
    switch (p) {
    case vshogi::minishogi::B_FU:
        return attacks_table[0][sq];
    case vshogi::minishogi::B_GI:
        return attacks_table[1][sq];
    case vshogi::minishogi::B_KI:
    case vshogi::minishogi::B_TO:
    case vshogi::minishogi::B_NG:
        return attacks_table[2][sq];
    case vshogi::minishogi::W_FU:
        return attacks_table[3][sq];
    case vshogi::minishogi::W_GI:
        return attacks_table[4][sq];
    case vshogi::minishogi::W_KI:
    case vshogi::minishogi::W_TO:
    case vshogi::minishogi::W_NG:
        return attacks_table[5][sq];
    case vshogi::minishogi::B_OU:
    case vshogi::minishogi::W_OU:
        return attacks_table[6][sq];
    case vshogi::minishogi::B_KA:
    case vshogi::minishogi::W_KA:
        return attacks_table[7][sq];
    case vshogi::minishogi::B_HI:
    case vshogi::minishogi::W_HI:
        return attacks_table[8][sq];
    case vshogi::minishogi::B_UM:
    case vshogi::minishogi::W_UM:
        return attacks_table[9][sq];
    case vshogi::minishogi::B_RY:
    case vshogi::minishogi::W_RY:
        return attacks_table[10][sq];
    default:
        return BitBoard();
    }
}

template <>
inline minishogi::BitBoard minishogi::BitBoard::get_attacks_by(
    const vshogi::minishogi::BoardPieceTypeEnum& p,
    const vshogi::minishogi::SquareEnum& sq,
    const vshogi::minishogi::BitBoard& occupied)
{
    using namespace vshogi::minishogi;
    switch (p) {
    case vshogi::minishogi::B_FU:
        return attacks_table[0][sq];
    case vshogi::minishogi::B_GI:
        return attacks_table[1][sq];
    case vshogi::minishogi::B_KI:
    case vshogi::minishogi::B_TO:
    case vshogi::minishogi::B_NG:
        return attacks_table[2][sq];
    case vshogi::minishogi::W_FU:
        return attacks_table[3][sq];
    case vshogi::minishogi::W_GI:
        return attacks_table[4][sq];
    case vshogi::minishogi::W_KI:
    case vshogi::minishogi::W_TO:
    case vshogi::minishogi::W_NG:
        return attacks_table[5][sq];
    case vshogi::minishogi::B_OU:
    case vshogi::minishogi::W_OU:
        return attacks_table[6][sq];
    case vshogi::minishogi::B_KA:
    case vshogi::minishogi::W_KA:
        return BitBoard::ranging_attacks_to_diagonal(sq, occupied);
    case vshogi::minishogi::B_HI:
    case vshogi::minishogi::W_HI:
        return BitBoard::ranging_attacks_to_adjacent(sq, occupied);
    case vshogi::minishogi::B_UM:
    case vshogi::minishogi::W_UM:
        return BitBoard::ranging_attacks_to_diagonal(sq, occupied)
               | attacks_table[6][sq];
    case vshogi::minishogi::B_RY:
    case vshogi::minishogi::W_RY:
        return BitBoard::ranging_attacks_to_adjacent(sq, occupied)
               | attacks_table[6][sq];
    default:
        return BitBoard();
    }
}

template <>
inline void minishogi::BitBoard::init_tables()
{
    for (auto sq = static_cast<Square>(num_squares); sq--;) {
        const auto b = from_square(sq);
        // clang-format off
        attacks_table[0][sq] = b.shift(DIR_N); // B_FU
        attacks_table[1][sq] = b.shift(DIR_NW) | b.shift(DIR_N) | b.shift(DIR_NE) | b.shift(DIR_SW) | b.shift(DIR_SE); // B_GI
        attacks_table[2][sq] = b.shift(DIR_NW) | b.shift(DIR_N) | b.shift(DIR_NE) | b.shift(DIR_W) | b.shift(DIR_E) | b.shift(DIR_S); // B_KI
        attacks_table[3][sq] = b.shift(DIR_S); // W_FU
        attacks_table[4][sq] = b.shift(DIR_NW) | b.shift(DIR_NE) | b.shift(DIR_SW) | b.shift(DIR_S) | b.shift(DIR_SE); // W_GI
        attacks_table[5][sq] = b.shift(DIR_N) | b.shift(DIR_W) | b.shift(DIR_E) | b.shift(DIR_SW) | b.shift(DIR_S) | b.shift(DIR_SE); // W_KI
        attacks_table[6][sq] = attacks_table[1][sq] | attacks_table[2][sq]; // OU
        attacks_table[7][sq] = BitBoard::ranging_attacks_to_diagonal(sq); // KA
        attacks_table[8][sq] = BitBoard::ranging_attacks_to_adjacent(sq); // HI
        attacks_table[9][sq] = attacks_table[7][sq] | attacks_table[6][sq]; // UM
        attacks_table[10][sq] = attacks_table[8][sq] | attacks_table[6][sq]; // RY
        // clang-format on
    }
}

template <>
inline std::uint64_t minishogi::State::zobrist_board
    [minishogi::Config::num_squares]
    [num_colors * minishogi::Config::num_piece_types + 1]
    = {};

template <>
inline std::uint64_t
    minishogi::State::zobrist_stand[num_colors]
                                   [minishogi::Config::num_stand_piece_types][3]
    = {};

} // namespace vshogi

#endif // VSHOGI_VARIANTS_MINISHOGI_HPP
