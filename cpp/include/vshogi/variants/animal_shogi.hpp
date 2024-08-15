#ifndef VSHOGI_VARIANTS_ANIMAL_SHOGI_HPP
#define VSHOGI_VARIANTS_ANIMAL_SHOGI_HPP

#include <cstdint>
#include <type_traits>

#include "vshogi/common/bitboard.hpp"
#include "vshogi/common/board.hpp"
#include "vshogi/common/color.hpp"
#include "vshogi/common/game.hpp"
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
    NA = 0b0111, // Not available.

    OU = LI,
};

enum BoardPieceTypeEnum : std::uint8_t
{
    B_CH = CH, //!< Black Chick (Pawn)
    B_EL = EL, //!< Black Elephant (Limited Bishop)
    B_GI = GI, //!< Black Giraffe (Limited Rook)
    B_LI = LI, //!< Black Lion (King)
    B_HE = HE, //!< Black Hen (Promoted Pawn)
    W_CH = 0b1000 + CH, //!< White Chick (Pawn)
    W_EL = 0b1000 + EL, //!< White Elephant (Limited Bishop)
    W_GI = 0b1000 + GI, //!< White Giraffe (Limited Rook)
    W_LI = 0b1000 + LI, //!< White Lion (King)
    W_HE = 0b1000 + HE, //!< White Hen (Promoted Pawn)
    VOID = 0b1111, //!< Empty square.
};

/**
 * @brief Enumeration of board squares.
 * @details
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
    FILE_A,
    FILE_B,
    FILE_C,
};
enum RankEnum : uint
{
    RANK1,
    RANK2,
    RANK3,
    RANK4,
};

struct Config
{
    // clang-format off
    static constexpr uint num_piece_types = 5; //!< CH, EL, GI, LI, HE
    static constexpr uint num_stand_piece_types = 3; //!< CH, EL, GI
    static constexpr uint color_bit = 3; //!< ____ *___
    static constexpr uint num_files = 3; //!< A, B, C
    static constexpr uint num_ranks = 4; //!< 1, 2, 3, 4
    static constexpr uint num_promotion_ranks = 1;
    static constexpr uint num_dir = 8; //!< NW, N, NE, W, E, SW, S, SE
    static constexpr uint num_dir_dl = 8; //!< NW, N, NE, W, E, SW, S, SE
    static constexpr uint num_non_ranging_board_piece = 7; //!< B_CH, W_CH, EL, GI, LI, B_HE, W_HE
    static constexpr uint num_attacks = 7; //!< B_CH, W_CH, EL, GI, LI, B_HE, W_HE
    static constexpr uint max_stand_piece_count = 2;
    static constexpr uint max_acceptable_repetitions = 2;
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

template <class E>
inline typename std::enable_if<std::is_enum<E>::value, E>::type
operator--(E& self, int)
{
    const auto out = self;
    self = static_cast<E>(static_cast<int>(self) - 1);
    return out;
}

namespace internal
{

inline ResultEnum move_result(
    const Move move,
    const BoardPieceTypeEnum moving,
    const BoardPieceTypeEnum captured)
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
inline animal_shogi::PieceTypeEnum
animal_shogi::Pieces::to_piece_type(const char c)
{
    using namespace animal_shogi;
    switch (std::tolower(c)) {
    case 'c':
        return CH;
    case 'e':
        return EL;
    case 'g':
        return GI;
    case 'h':
        return HE;
    case 'l':
        return LI;
    default:
        return NA;
    }
}

template <>
constexpr bool
animal_shogi::Pieces::is_promotable(const animal_shogi::PieceTypeEnum& pt)
{
    return (pt == animal_shogi::CH);
}

template <>
inline bool
animal_shogi::Pieces::is_ranging_piece(const animal_shogi::PieceTypeEnum&)
{
    return false;
}

template <>
inline char animal_shogi::Pieces::to_char(const animal_shogi::PieceTypeEnum& p)
{
    constexpr char table[] = {'c', 'e', 'g', 'l', 'h'};
    static_assert(0 == animal_shogi::CH);
    static_assert(1 == animal_shogi::EL);
    static_assert(2 == animal_shogi::GI);
    static_assert(3 == animal_shogi::LI);
    static_assert(4 == animal_shogi::HE);
    return table[p];
}

template <>
inline void animal_shogi::Pieces::append_sfen(
    const animal_shogi::BoardPieceTypeEnum& p, std::string& out)
{
    constexpr char table[] = {'c', 'e', 'g', 'l', 'h'};
    static_assert(0 == animal_shogi::CH);
    static_assert(1 == animal_shogi::EL);
    static_assert(2 == animal_shogi::GI);
    static_assert(3 == animal_shogi::LI);
    static_assert(4 == animal_shogi::HE);
    if (p != VOID)
        out += (get_color(p) == BLACK)
                   ? static_cast<char>(std::toupper(static_cast<int>(table[p])))
                   : table[to_piece_type(p)];
}

template <>
constexpr animal_shogi::FileEnum animal_shogi::Squares::file_right_most()
{
    return animal_shogi::FILE_C;
}
template <>
constexpr animal_shogi::FileEnum animal_shogi::Squares::file_left_most()
{
    return animal_shogi::FILE_A;
}

template <>
constexpr animal_shogi::FileEnum
animal_shogi::Squares::to_file(const animal_shogi::SquareEnum& sq)
{
    return static_cast<animal_shogi::FileEnum>(sq % num_files);
}

template <>
constexpr animal_shogi::SquareEnum animal_shogi::Squares::to_square(
    const animal_shogi::FileEnum& f, const animal_shogi::RankEnum& r)
{
    return static_cast<animal_shogi::SquareEnum>(r * num_files + f);
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
inline bool animal_shogi::Squares::in_promotion_zone(
    const animal_shogi::RankEnum& r, const ColorEnum& c)
{
    return (c == BLACK) ? (r == animal_shogi::RANK1)
                        : (r == animal_shogi::RANK4);
}

template <>
inline DirectionEnum
animal_shogi::Squares::get_direction_for_diagonal_or_knight(
    const animal_shogi::SquareEnum& dst, const animal_shogi::SquareEnum& src)
{
    using namespace vshogi::animal_shogi;
    switch (static_cast<int>(src - dst)) {
    case 8:
        return DIR_NW;
    case 2:
        return DIR_NE;
    case 4:
        return (Squares::to_file(src) == FILE_A) ? DIR_NE : DIR_NW;
    case -2:
        return DIR_SW;
    case -8:
        return DIR_SE;
    case -4:
        return (Squares::to_file(dst) == FILE_A) ? DIR_SW : DIR_SE;
    default:
        return DIR_NA;
    }
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
inline const int animal_shogi::BlackWhiteStands::max_sfen_length
    = 13; // "2C2E2G2c2e2g "

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
inline const animal_shogi::BitBoard animal_shogi::BitBoard::
    square_to_bitboard_array[animal_shogi::Config::num_squares + 1]
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
        BitBoard(),
};

template <>
inline animal_shogi::BitBoard
    animal_shogi::BitBoard::attacks_table[animal_shogi::Config::num_attacks]
                                         [animal_shogi::Config::num_squares]
    = {};

template <>
inline animal_shogi::BitBoard animal_shogi::BitBoard::get_attacks_by(
    const animal_shogi::BoardPieceTypeEnum& p,
    const animal_shogi::SquareEnum& sq)
{
    using namespace animal_shogi;
    switch (p) {
    case vshogi::animal_shogi::B_CH:
        return attacks_table[0][sq];
    case vshogi::animal_shogi::B_HE:
        return attacks_table[1][sq];
    case vshogi::animal_shogi::W_CH:
        return attacks_table[2][sq];
    case vshogi::animal_shogi::W_HE:
        return attacks_table[3][sq];
    case vshogi::animal_shogi::B_EL:
    case vshogi::animal_shogi::W_EL:
        return attacks_table[4][sq];
    case vshogi::animal_shogi::B_GI:
    case vshogi::animal_shogi::W_GI:
        return attacks_table[5][sq];
    case vshogi::animal_shogi::B_LI:
    case vshogi::animal_shogi::W_LI:
        return attacks_table[6][sq];
    default:
        return BitBoard();
    }
}

template <>
inline animal_shogi::BitBoard animal_shogi::BitBoard::get_attacks_by(
    const animal_shogi::BoardPieceTypeEnum& p,
    const animal_shogi::SquareEnum& sq,
    const animal_shogi::BitBoard&)
{
    return get_attacks_by(p, sq);
}

template <>
inline void animal_shogi::BitBoard::init_tables()
{
    for (auto sq = static_cast<Square>(num_squares); sq--;) {
        const auto b = from_square(sq);
        // clang-format off
        attacks_table[0][sq] = b.shift(DIR_N); // B_CH
        attacks_table[1][sq] = b.shift(DIR_NW) | b.shift(DIR_N) | b.shift(DIR_NE) | b.shift(DIR_W) | b.shift(DIR_E) | b.shift(DIR_S); // B_KI
        attacks_table[2][sq] = b.shift(DIR_S); // W_CH
        attacks_table[3][sq] = b.shift(DIR_N) | b.shift(DIR_W) | b.shift(DIR_E) | b.shift(DIR_SW) | b.shift(DIR_S) | b.shift(DIR_SE); // W_KI
        attacks_table[4][sq] = b.shift(DIR_NW) | b.shift(DIR_NE) | b.shift(DIR_SW) | b.shift(DIR_SE); // EL
        attacks_table[5][sq] = b.shift(DIR_N) | b.shift(DIR_W) | b.shift(DIR_E) | b.shift(DIR_S); // GI
        attacks_table[6][sq] = attacks_table[4][sq] | attacks_table[5][sq]; // LI
        // clang-format on
    }
}

template <>
inline std::uint64_t animal_shogi::Board::zobrist_table
    [animal_shogi::Config::num_squares]
    [num_colors * animal_shogi::Config::num_piece_types + 1]
    = {};

template <>
inline animal_shogi::BoardPieceTypeEnum animal_shogi::Board::apply(
    const animal_shogi::SquareEnum& dst,
    const animal_shogi::SquareEnum& src,
    const bool&,
    std::uint64_t* const hash)
{
    BoardPieceType moving_piece = place_piece_on(src, VOID);
    if (hash != nullptr) {
        *hash ^= zobrist_table[src][to_index(VOID)];
        *hash ^= zobrist_table[src][to_index(moving_piece)];
    }
    if (!BitBoardType::get_attacks_by(moving_piece, dst).any())
        moving_piece = PHelper::promote_nocheck(moving_piece);
    return apply(dst, moving_piece, hash);
}

template <>
inline void animal_shogi::Game::update_result()
{
    const auto turn = get_turn();
    if (m_legal_moves.empty())
        m_result = (turn == BLACK) ? WHITE_WIN : BLACK_WIN;
    if (is_repetitions())
        m_result = DRAW;
    if (m_result != ONGOING)
        m_legal_moves.clear();
}

template <>
inline void animal_shogi::Game::update_internals()
{
    update_king_occupied_checkers();
    {
        const auto turn = get_turn();
        const auto& board = get_board();
        const auto& stand = get_stand(turn);
        m_legal_moves.clear();
        for (auto src = static_cast<Square>(num_squares); src--;) {
            const auto p = board[src];
            if ((p == PHelper::VOID) || (PHelper::get_color(p) != turn))
                continue;
            for (auto dp = PHelper::get_attack_directions(p); *dp != DIR_NA;) {
                const auto dst = SHelper::shift(src, *dp++);
                if (dst == SHelper::SQ_NA)
                    continue;
                const auto t = board[dst];
                if (((t == PHelper::VOID) || (PHelper::get_color(t) == ~turn))
                    && BitBoardType::get_attacks_by(p, src).is_one(dst))
                    m_legal_moves.emplace_back(dst, src);
            }
        }
        for (auto dst = static_cast<Square>(num_squares); dst--;) {
            if (!board.is_empty(dst))
                continue;
            for (auto pt = static_cast<PieceType>(num_stand_piece_types);
                 pt--;) {
                if (stand.exist(pt))
                    m_legal_moves.emplace_back(MoveType(dst, pt));
            }
        }
    }
    update_result();
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
    if (illegal) {
        m_result = (get_turn() == BLACK) ? BLACK_WIN : WHITE_WIN;
        m_legal_moves.clear();
    } else if (m_result == ONGOING) {
        update_internals();
    } else {
        m_legal_moves.clear();
    }
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
        update_internals();
    } else {
        m_legal_moves.clear();
    }
    return *this;
}

template <>
inline animal_shogi::Game&
animal_shogi::Game::apply_mcts_internal_vertex(const animal_shogi::Move& move)
{
    return apply_nocheck(move);
}

template <>
inline animal_shogi::Game::Game(const animal_shogi::State& s)
    : m_current_state(s), m_zobrist_hash_list(), m_move_list(), m_legal_moves(),
      m_result(ONGOING), m_zobrist_hash(m_current_state.zobrist_hash()),
      m_initial_sfen_without_ply(m_current_state.to_sfen()),
      m_half_num_pieces{0, 0}, m_initial_points{0, 0}
{
    m_zobrist_hash_list.reserve(128);
    m_move_list.reserve(128);
    update_internals();
}

} // namespace vshogi

#endif // VSHOGI_VARIANTS_ANIMAL_SHOGI_HPP
