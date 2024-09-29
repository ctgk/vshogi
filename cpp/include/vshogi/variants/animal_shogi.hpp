#ifndef VSHOGI_VARIANTS_ANIMAL_SHOGI_HPP
#define VSHOGI_VARIANTS_ANIMAL_SHOGI_HPP

#include <cstdint>
#include <type_traits>

#include "vshogi/common/bitboard.hpp"
#include "vshogi/common/board.hpp"
#include "vshogi/common/color.hpp"
#include "vshogi/common/game.hpp"
#include "vshogi/common/generator.hpp"
#include "vshogi/common/move.hpp"
#include "vshogi/common/pieces.hpp"
#include "vshogi/common/squares.hpp"
#include "vshogi/common/stand.hpp"
#include "vshogi/common/state.hpp"
#include "vshogi/common/utils.hpp"

namespace vshogi::animal_shogi
{

enum PieceTypeEnum : std::uint8_t
{
    CH, //!< Chick (Pawn)
    EL, //!< Elephant (Limited Bishop)
    GI, //!< Giraffe (Limited Rook)
    LI, //!< Lion (King)
    HE, //!< Hen (Promoted Pawn)
    NA, // Not available.

    OU = LI,
};

enum ColoredPieceEnum : std::uint8_t
{
    B_CH, //!< Black Chick (Pawn)
    B_EL, //!< Black Elephant (Limited Bishop)
    B_GI, //!< Black Giraffe (Limited Rook)
    B_LI, //!< Black Lion (King)
    B_HE, //!< Black Hen (Promoted Pawn)
    W_CH, //!< White Chick (Pawn)
    W_EL, //!< White Elephant (Limited Bishop)
    W_GI, //!< White Giraffe (Limited Rook)
    W_LI, //!< White Lion (King)
    W_HE, //!< White Hen (Promoted Pawn)
    VOID, //!< Empty square.
};

/**
 * @brief Enumeration of board squares.
 * @details
 *    (1) (2) (3)
 *     A   B   C
 *   *---*---*---*
 * 1 | 0 | 1 | 2 |
 *   *---*---*---*
 * 2 | 3 | 4 | 5 |
 *   *---*---*---*
 * 3 | 6 | 7 | 8 |
 *   *---*---*---*
 * 4 | 9 | 10| 11|
 *   *---*---*---*
 */
enum SquareEnum : uint
{
    // clang-format off
    SQ_A1, SQ_B1, SQ_C1,
    SQ_A2, SQ_B2, SQ_C2,
    SQ_A3, SQ_B3, SQ_C3,
    SQ_A4, SQ_B4, SQ_C4,
    // clang-format on
    SQ_NA,
    NUM_SQ = 12,
};
enum FileEnum : uint
{
    FILE1 = 0,
    FILE2,
    FILE3,
};
enum RankEnum : uint
{
    RANK1 = 0,
    RANK2,
    RANK3,
    RANK4,
};

struct Config
{
    // clang-format off
    static constexpr char piece_type_to_char[] = "ceglh";
    static constexpr uint num_piece_types = 5; //!< CH, EL, GI, LI, HE
    static constexpr uint num_stand_piece_types = 3; //!< CH, EL, GI
    static constexpr uint piece_type_to_point[] = {0, 0, 0, 0, 0, 0};
    static constexpr uint num_files = 3; //!< A, B, C
    static constexpr uint num_ranks = 4; //!< 1, 2, 3, 4
    static constexpr uint num_promotion_ranks = 1;
    static constexpr uint num_dir = 8; //!< NW, N, NE, W, E, SW, S, SE
    static constexpr uint num_dir_dl = 8; //!< NW, N, NE, W, E, SW, S, SE
    static constexpr uint max_stand_piece_count = 2;
    static constexpr uint max_stand_sfen_length = 7; // "2C2E2G "
    static constexpr uint max_acceptable_repetitions = 2;
    static constexpr uint half_num_initial_pieces = 2;
    static constexpr uint initial_points = 3;
    using BaseTypeBitBoard = std::uint16_t;
    // clang-format on

    /**
     * @brief 8bit integer representing what is on a piece stand.
     * @details
     * ______11  Chick (2bits for 0, 1, or 2 pieces)
     * ___11___  Elephant (2 bits for 0, 1, or 2 pieces)
     * 11______  Giraffe (2 bits for 0, 1, or 2 pieces)
     */
    using BaseTypeStand = std::uint8_t;

    Config() = delete;
    using PieceType = PieceTypeEnum;
    using ColoredPiece = ColoredPieceEnum;
    using Square = SquareEnum;
    using File = FileEnum;
    using Rank = RankEnum;
    static constexpr uint num_squares = num_files * num_ranks;
    static constexpr uint num_colored_piece_types = 2 * num_piece_types;
};

using Pieces = vshogi::Pieces<Config>;
using Squares = vshogi::Squares<Config>;
using Move = vshogi::Move<Config>;
using BitBoard = vshogi::BitBoard<Config>;
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

constexpr BitBoard bb_a1 = (BitBoard(1) << static_cast<uint>(SQ_A1));
constexpr BitBoard bb_b1 = (BitBoard(1) << static_cast<uint>(SQ_B1));
constexpr BitBoard bb_c1 = (BitBoard(1) << static_cast<uint>(SQ_C1));
constexpr BitBoard bb_a2 = (BitBoard(1) << static_cast<uint>(SQ_A2));
constexpr BitBoard bb_b2 = (BitBoard(1) << static_cast<uint>(SQ_B2));
constexpr BitBoard bb_c2 = (BitBoard(1) << static_cast<uint>(SQ_C2));
constexpr BitBoard bb_a3 = (BitBoard(1) << static_cast<uint>(SQ_A3));
constexpr BitBoard bb_b3 = (BitBoard(1) << static_cast<uint>(SQ_B3));
constexpr BitBoard bb_c3 = (BitBoard(1) << static_cast<uint>(SQ_C3));
constexpr BitBoard bb_a4 = (BitBoard(1) << static_cast<uint>(SQ_A4));
constexpr BitBoard bb_b4 = (BitBoard(1) << static_cast<uint>(SQ_B4));
constexpr BitBoard bb_c4 = (BitBoard(1) << static_cast<uint>(SQ_C4));

constexpr BitBoard bb_rank1 = BitBoard(0b000000000111);
constexpr BitBoard bb_rank2 = BitBoard(0b000000111000);
constexpr BitBoard bb_rank3 = BitBoard(0b000111000000);
constexpr BitBoard bb_rank4 = BitBoard(0b111000000000);

constexpr BitBoard bb_filea = BitBoard(0b001001001001);
constexpr BitBoard bb_fileb = BitBoard(0b010010010010);
constexpr BitBoard bb_filec = BitBoard(0b100100100100);

namespace internal
{

inline ResultEnum move_result(
    const Move move,
    const ColoredPieceEnum moving,
    const ColoredPieceEnum captured)
{
    if (captured == B_LI)
        return WHITE_WIN;
    if (captured == W_LI)
        return BLACK_WIN;
    const auto r_dst = Squares::to_rank(move.destination());
    if ((moving == B_LI) && (r_dst == RANK1))
        return BLACK_WIN;
    if ((moving == W_LI) && (r_dst == RANK4))
        return WHITE_WIN;
    return ONGOING;
}

} // namespace internal

} // namespace vshogi::animal_shogi

namespace vshogi
{

template <>
inline const DirectionEnum
    animal_shogi::Pieces::attack_directions_table[2 * num_piece_types + 1][9]
    = {
        // clang-format off
{DIR_N,                                                      DIR_NA}, // B_CH
{DIR_NW, DIR_NE, DIR_SW, DIR_SE,                             DIR_NA}, // B_EL
{DIR_N, DIR_W, DIR_E, DIR_S,                                 DIR_NA}, // B_GI
{DIR_NW, DIR_N, DIR_NE, DIR_W, DIR_E, DIR_SW, DIR_S, DIR_SE, DIR_NA}, // B_LI
{DIR_NW, DIR_N, DIR_NE, DIR_W, DIR_E, DIR_S,                 DIR_NA}, // B_HE
{DIR_S,                                                      DIR_NA}, // W_CH
{DIR_SE, DIR_SW, DIR_NE, DIR_NW,                             DIR_NA}, // W_EL
{DIR_S, DIR_E, DIR_W, DIR_N,                                 DIR_NA}, // W_GI
{DIR_SE, DIR_S, DIR_SW, DIR_E, DIR_W, DIR_NE, DIR_N, DIR_NW, DIR_NA}, // W_LI
{DIR_SE, DIR_S, DIR_SW, DIR_E, DIR_W, DIR_N,                 DIR_NA}, // W_HE
        // clang-format on
};

template <>
constexpr bool
animal_shogi::Pieces::is_promotable(const animal_shogi::PieceTypeEnum& pt)
{
    return (pt == animal_shogi::CH);
}

template <>
inline bool animal_shogi::Pieces::is_ranging_to(
    const animal_shogi::ColoredPieceEnum&, const DirectionEnum&)
{
    return false;
}

template <>
inline bool
animal_shogi::Pieces::is_ranging_piece(const animal_shogi::PieceTypeEnum&)
{
    return false;
}

template <>
inline void animal_shogi::Pieces::append_sfen(
    const animal_shogi::ColoredPieceEnum& p, std::string& out)
{
    const auto color = get_color(p);
    const auto pt = to_piece_type(p);
    char c = to_char(pt);
    if (color == BLACK)
        c = static_cast<char>(std::toupper(static_cast<int>(c)));
    out += c;
}

template <>
inline animal_shogi::SquareEnum
animal_shogi::Squares::to_square(const char usi[2])
{
    return to_square(
        static_cast<animal_shogi::FileEnum>(usi[0] - 'a'),
        static_cast<animal_shogi::RankEnum>(usi[1] - '1'));
}

template <>
inline void
animal_shogi::Squares::to_usi(char usi[2], const animal_shogi::SquareEnum& sq)
{
    usi[0] = static_cast<char>(static_cast<int>(to_file(sq)) + 'a');
    usi[1] = static_cast<char>(static_cast<int>(to_rank(sq)) + '1');
}

template <>
inline const uint animal_shogi::Stand::shift_bits[] = {
    0u, // CH
    3u, // EL
    6u, // GI
};
template <>
inline const std::uint8_t animal_shogi::Stand::masks[] = {
    0b00000011, // CH
    0b00011000, // EL
    0b11000000, // GI
};
template <>
inline const std::uint8_t animal_shogi::Stand::deltas[] = {
    0b00000001, // CH
    0b00001000, // EL
    0b01000000, // GI
};
template <>
inline const std::uint8_t animal_shogi::Stand::mask = 0b11011011;

template <>
template <>
inline animal_shogi::Stand::Stand(
    const int num_ch, const int num_el, const int num_gi)
    : Stand(static_cast<std::uint8_t>(
        (num_ch << shift_bits[animal_shogi::CH])
        + (num_el << shift_bits[animal_shogi::EL])
        + (num_gi << shift_bits[animal_shogi::GI])))
{
}

template <>
inline const animal_shogi::PieceTypeEnum
    animal_shogi::BlackWhiteStands::stand_pieces_in_sfen_order[]
    = {animal_shogi::GI, animal_shogi::EL, animal_shogi::CH};

template <>
inline std::uint64_t animal_shogi::BlackWhiteStands::zobrist_table
    [num_colors][animal_shogi::Config::num_stand_piece_types]
    [animal_shogi::Config::max_stand_piece_count + 1]
    = {};

template <>
inline uint animal_shogi::Move::to_dlshogi_source_index() const
{
    if (is_drop())
        return 8 + static_cast<uint>(source_piece());
    constexpr int diff_plus_4_to_dir_index[] = {0, 1, 2, 3, -1, 4, 5, 6, 7};
    return static_cast<uint>(diff_plus_4_to_dir_index
                                 [static_cast<int>(source_square())
                                  - static_cast<int>(destination()) + 4]);
}

template <>
constexpr uint animal_shogi::Move::num_policy_per_square()
{
    return 8 + num_stand_piece_types;
}

template <>
inline animal_shogi::BitBoard animal_shogi::BitBoard::attacks_table
    [animal_shogi::Config::num_colored_piece_types]
    [animal_shogi::Config::num_squares]
    = {};

template <>
inline animal_shogi::BitBoard
    animal_shogi::BitBoard::ray_table[animal_shogi::Config::num_squares]
                                     [animal_shogi::Config::num_dir]
    = {};
template <>
inline animal_shogi::BitBoard animal_shogi::BitBoard::line_segment_table
    [animal_shogi::Config::num_squares][animal_shogi::Config::num_squares]
    = {};

template <>
inline animal_shogi::BitBoard animal_shogi::BitBoard::get_attacks_by(
    const animal_shogi::ColoredPieceEnum& p,
    const animal_shogi::SquareEnum& sq,
    const animal_shogi::BitBoard&)
{
    return get_attacks_by(p, sq);
}

template <>
inline std::uint64_t animal_shogi::Board::zobrist_table
    [animal_shogi::Config::num_squares]
    [num_colors * animal_shogi::Config::num_piece_types + 1]
    = {};

template <>
inline animal_shogi::ColoredPieceEnum animal_shogi::Board::apply(
    const animal_shogi::SquareEnum& dst,
    const animal_shogi::SquareEnum& src,
    const bool&,
    std::uint64_t* const hash)
{
    ColoredPiece moving_piece = place_piece_on(src, VOID);
    if (moving_piece != VOID) {
        m_bb_color[PHelper::get_color(moving_piece)].toggle(src);
    }
    if (hash != nullptr) {
        *hash ^= zobrist_table[src][VOID];
        *hash ^= zobrist_table[src][moving_piece];
    }
    if (!BitBoardType::get_attacks_by(moving_piece, dst).any())
        moving_piece = PHelper::promote_nocheck(moving_piece);
    return apply(dst, moving_piece, hash);
}

template <>
inline void
animal_shogi::KingMoveGenerator::increment_iterator_while_square_is_attacked()
{
}

template <>
inline void animal_shogi::DropMoveGenerator::init_sq_iter()
{
    m_sq_iter = (~m_board.get_occupied()).square_iterator();
}

template <>
inline void animal_shogi::DropMoveGenerator::increment_square()
{
    while (!m_sq_iter.is_end()) {
        if (m_board.is_empty(*m_sq_iter))
            break;
        ++m_sq_iter;
    }
}

template <>
inline void animal_shogi::DropMoveGenerator::increment_iterators_unless_legal()
{
    while (m_pt_iter < num_stand_piece_types) {
        increment_piece_type_unless_in_stand();
        if (m_pt_iter == num_stand_piece_types) {
            m_sq_iter = BitBoardType().square_iterator();
            break;
        }
        increment_square();
        if (m_sq_iter.is_end()) {
            m_pt_iter = static_cast<PieceType>(m_pt_iter + 1);
            if (m_pt_iter == num_stand_piece_types)
                break;
            init_sq_iter();
        } else {
            break;
        }
    }
}

template <>
inline void animal_shogi::NonKingBoardMoveGenerator::init_dst_iter()
{
    const auto src = *m_src_iter;
    const auto p = m_board[src];
    auto movable = BitBoardType::get_attacks_by(p, src);
    movable &= ~m_board.get_occupied(m_turn);
    m_dst_iter = movable.square_iterator();
}

template <>
inline animal_shogi::NonKingBoardMoveGenerator::NonKingBoardMoveGenerator(
    const animal_shogi::State& state)
    : m_state(state), m_turn(state.get_turn()), m_board(state.get_board()),
      m_pinned(), m_src_iter(), m_dst_iter(), m_promote(false)
{
    init_src_iter();
    while (!m_src_iter.is_end()) {
        init_dst_iter();
        if (m_dst_iter.is_end())
            ++m_src_iter;
        else
            break;
    }
}

template <>
inline animal_shogi::NonKingBoardMoveGenerator&
animal_shogi::NonKingBoardMoveGenerator::operator++()
{
    ++m_dst_iter;
    if (!m_dst_iter.is_end())
        return *this;

    ++m_src_iter;
    while (!m_src_iter.is_end()) {
        init_dst_iter();
        if (m_dst_iter.is_end())
            ++m_src_iter;
        else
            return *this;
    }
    m_promote = true;
    return *this;
}

template <>
inline animal_shogi::Move
animal_shogi::NonKingBoardMoveGenerator::operator*() const
{
    return animal_shogi::Move(*m_dst_iter, *m_src_iter);
}

template <>
inline bool animal_shogi::NonKingBoardMoveGenerator::operator!=(
    const NonKingBoardMoveGenerator& other) const
{

    return (m_src_iter != other.m_src_iter) || (m_dst_iter != other.m_dst_iter);
}

template <>
inline void animal_shogi::Game::update_result()
{
    const auto turn = get_turn();
    if (LegalMoveGenerator<animal_shogi::Config>(m_current_state).is_end())
        m_result = (turn == BLACK) ? WHITE_WIN : BLACK_WIN;
    if (is_repetitions())
        m_result = DRAW;
}

template <>
inline animal_shogi::Game&
animal_shogi::Game::apply(const animal_shogi::Move& move)
{
    const auto illegal = !is_legal(move);
    m_zobrist_hash_list.emplace_back(m_zobrist_hash);
    m_move_list.emplace_back(move);
    if (!move.is_drop()) {
        const auto moving = get_board()[move.source_square()];
        const auto captured = get_board()[move.destination()];
        m_result = animal_shogi::internal::move_result(move, moving, captured);
    }
    m_current_state.apply(move, &m_zobrist_hash);
    if (illegal)
        m_result = (get_turn() == BLACK) ? BLACK_WIN : WHITE_WIN;
    else if (m_result == ONGOING)
        update_result();
    return *this;
}

template <>
inline animal_shogi::Game&
animal_shogi::Game::apply_nocheck(const animal_shogi::Move& move)
{
    m_zobrist_hash_list.emplace_back(m_zobrist_hash);
    m_move_list.emplace_back(move);
    if (!move.is_drop()) {
        const auto moving = get_board()[move.source_square()];
        const auto captured = get_board()[move.destination()];
        m_result = animal_shogi::internal::move_result(move, moving, captured);
    }
    m_current_state.apply(move, &m_zobrist_hash);
    if (m_result == ONGOING) {
        update_result();
    }
    return *this;
}

template <>
inline animal_shogi::Game::Game(const animal_shogi::State& s)
    : m_current_state(s), m_zobrist_hash_list(), m_move_list(),
      m_result(ONGOING), m_zobrist_hash(m_current_state.zobrist_hash()),
      m_initial_sfen_without_ply(m_current_state.to_sfen())
{
    m_zobrist_hash_list.reserve(128);
    m_move_list.reserve(128);
    update_result();
}

template <>
inline void animal_shogi::Game::attention_matrix(float* const data)
{
    std::fill_n(data, num_squares * num_squares, 0.f);
    for (auto sq : EnumIterator<Square, num_squares>()) {
        const uint ii = static_cast<uint>(sq);
        for (auto dir : EnumIterator<DirectionEnum, num_dir>()) {
            const auto attention = SHelper::shift(sq, dir);
            if (attention != SHelper::SQ_NA) {
                const uint jj = static_cast<uint>(attention);
                data[ii * num_squares + jj] = 1.f;
            }
        }
    }
}

template <>
inline void animal_shogi::Game::attention_matrix(
    float* const data, const std::vector<DirectionEnum>& directions)
{
    std::fill_n(data, num_squares * num_squares, 0.f);
    for (auto sq : EnumIterator<Square, num_squares>()) {
        const uint ii = static_cast<uint>(sq);
        for (auto dir : directions) {
            const auto attention = SHelper::shift(sq, dir);
            if (attention != SHelper::SQ_NA) {
                const uint jj = static_cast<uint>(attention);
                data[ii * num_squares + jj] = 1.f;
            }
        }
    }
}

} // namespace vshogi

#endif // VSHOGI_VARIANTS_ANIMAL_SHOGI_HPP
