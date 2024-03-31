#ifndef VSHOGI_VARIANTS_ANIMAL_SHOGI_HPP
#define VSHOGI_VARIANTS_ANIMAL_SHOGI_HPP

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

namespace vshogi::animal_shogi
{

// clang-format off
constexpr uint num_piece_types = 5; // CH, EL, GI, LI, HE
constexpr uint num_stand_piece_types = 3; // CH, EL, GI
constexpr uint promotion_bit = 2; // ____ __*__
constexpr uint num_files = 3; // A, B, C
constexpr uint num_ranks = 4; // 1, 2, 3, 4
constexpr uint num_directions = 8; // NW, N, NE, W, E, SW, S, SE
constexpr uint num_directions_dlshogi = 8; // NW, N, NE, W, E, SW, S, SE
constexpr uint num_non_ranging_board_piece = 7; // B_CH, W_CH, EL, GI, LI, B_HE, W_HE
constexpr uint num_attack_types = 7; // B_CH, W_CH, EL, GI, LI, B_HE, W_HE
constexpr uint max_stand_piece_count = 2;
constexpr uint max_acceptable_repetition = 2;
// clang-format on

enum PieceTypeEnum : std::uint8_t
{
    CH = 0b0000, //!< Chick (Pawn)
    EL = 0b0001, //!< Elephant (Limited Bishop)
    GI = 0b0010, //!< Giraffe (Limited Rook)
    LI = 0b0011, //!< Lion (King)
    HE = 0b0100, //!< Hen (Promoted Pawn)
    NA = 0b0111, // Not available.

    OU = LI,
};

enum BoardPieceTypeEnum : std::uint8_t
{
    B_CH = 0b0000, //!< Black Chick (Pawn)
    B_EL = 0b0001, //!< Black Elephant (Limited Bishop)
    B_GI = 0b0010, //!< Black Giraffe (Limited Rook)
    B_LI = 0b0011, //!< Black Lion (King)
    B_HE = 0b0100, //!< Black Hen (Promoted Pawn)
    W_CH = 0b1000, //!< White Chick (Pawn)
    W_EL = 0b1001, //!< White Elephant (Limited Bishop)
    W_GI = 0b1010, //!< White Giraffe (Limited Rook)
    W_LI = 0b1011, //!< White Lion (King)
    W_HE = 0b1100, //!< White Hen (Promoted Pawn)
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

using Pieces = vshogi::Pieces<
    PieceTypeEnum,
    BoardPieceTypeEnum,
    num_piece_types,
    num_stand_piece_types,
    promotion_bit>;

using Squares = vshogi::Squares<
    SquareEnum,
    FileEnum,
    RankEnum,
    Pieces,
    num_files,
    num_ranks,
    num_directions,
    num_directions_dlshogi>;

/**
 * @brief 8bit integer representing what is on a piece stand.
 * @details
 * ______11  Chick (2bits for 0, 1, or 2 pieces)
 * ___11___  Elephant (2 bits for 0, 1, or 2 pieces)
 * 11______  Giraffe (2 bits for 0, 1, or 2 pieces)
 */
using Stand = vshogi::Stand<std::uint8_t, Pieces>;
using BlackWhiteStands = vshogi::BlackWhiteStands<Stand>;

/**
 * @brief 8bit integer representing an Animal Shogi move.
 * @details
 *       ____ ****       destination square (12 squares)
 *       **** ____       source square (15 = 12 squares + 3 pieces to drop)
 * (MSB) xxxx xxxx (LSB)
 */
using Move = vshogi::Move<std::uint8_t, Squares, Pieces, 7, 3, 3>;

using BitBoard = vshogi::
    BitBoard<std::uint16_t, Squares, BoardPieceTypeEnum, num_attack_types>;

using Board = vshogi::Board<Squares, Pieces, BitBoard>;

using State
    = vshogi::State<Board, BlackWhiteStands, Move, max_stand_piece_count>;

using Game = vshogi::Game<State, max_acceptable_repetition>;

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

inline Pieces::BoardPieceTypeEnum
promote_if_possible(const Pieces::BoardPieceTypeEnum p, const Move move)
{
    if (move.is_drop())
        return p;
    if (Pieces::to_piece_type(p) != CH)
        return p;
    const auto r_src = Squares::to_rank(move.source_square());
    const auto r_dst = Squares::to_rank(move.destination());
    const auto c = Pieces::get_color(p);
    if (c == vshogi::BLACK) {
        if ((r_src == RANK2) && (r_dst == RANK1))
            return Pieces::to_board_piece(c, HE);
        else
            return p;
    } else {
        if ((r_src == RANK3) && (r_dst == RANK4))
            return Pieces::to_board_piece(c, HE);
        else
            return p;
    }
}

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
inline DirectionEnum
    animal_shogi::Pieces::attack_directions_table[2 * num_piece_types + 1][9]
    = {};

template <>
inline const animal_shogi::PieceTypeEnum animal_shogi::Pieces::piece_array[]
    = {animal_shogi::CH,
       animal_shogi::EL,
       animal_shogi::GI,
       animal_shogi::LI,
       animal_shogi::HE};

template <>
inline const animal_shogi::PieceTypeEnum
    animal_shogi::Pieces::stand_piece_array[]
    = {animal_shogi::CH, animal_shogi::EL, animal_shogi::GI};

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
inline void animal_shogi::Pieces::init_attack_directions_of_black()
{
    constexpr DirectionEnum table[][9] = {
        // clang-format off
        {DIR_N,                                                      DIR_NA}, // B_CH
        {DIR_NW, DIR_NE, DIR_SW, DIR_SE,                             DIR_NA}, // B_EL
        {DIR_N, DIR_W, DIR_E, DIR_S,                                 DIR_NA}, // B_GI
        {DIR_NW, DIR_N, DIR_NE, DIR_W, DIR_E, DIR_SW, DIR_S, DIR_SE, DIR_NA}, // B_LI
        {DIR_NW, DIR_N, DIR_NE, DIR_W, DIR_E, DIR_S,                 DIR_NA}, // B_HE
        // clang-format on
    };
    for (uint ii = num_piece_types; ii--;) {
        for (uint jj = 0; jj < 9; ++jj) {
            if (table[ii][jj] == DIR_NA)
                break;
            attack_directions_table[ii][jj] = table[ii][jj];
        }
    }
}

template <>
inline char animal_shogi::Pieces::to_char(const animal_shogi::PieceTypeEnum& p)
{
    switch (p) {
    case animal_shogi::CH:
        return 'c';
    case animal_shogi::EL:
        return 'e';
    case animal_shogi::GI:
        return 'g';
    case animal_shogi::HE:
        return 'h';
    case animal_shogi::LI:
        return 'l';
    default:
        return '\0';
    }
}

template <>
inline void animal_shogi::Pieces::append_sfen(
    const animal_shogi::BoardPieceTypeEnum& p, std::string& out)
{
    switch (p) {
    case animal_shogi::B_CH:
        out += 'C';
        break;
    case animal_shogi::B_EL:
        out += 'E';
        break;
    case animal_shogi::B_GI:
        out += 'G';
        break;
    case animal_shogi::B_LI:
        out += 'L';
        break;
    case animal_shogi::B_HE:
        out += 'H';
        break;
    case animal_shogi::W_CH:
        out += 'c';
        break;
    case animal_shogi::W_EL:
        out += 'e';
        break;
    case animal_shogi::W_GI:
        out += 'g';
        break;
    case animal_shogi::W_LI:
        out += 'l';
        break;
    case animal_shogi::W_HE:
        out += 'h';
        break;
    default:
        break;
    }
}

template <>
constexpr animal_shogi::FileEnum animal_shogi::Squares::fe()
{
    return animal_shogi::FILE_C;
}
template <>
constexpr animal_shogi::FileEnum animal_shogi::Squares::fw()
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
    square_to_bitboard_array[animal_shogi::Squares::num_squares + 1]
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
    animal_shogi::BitBoard::attacks_table[animal_shogi::num_attack_types]
                                         [animal_shogi::Squares::num_squares]
    = {};

template <>
constexpr animal_shogi::BitBoard
animal_shogi::BitBoard::get_promotion_zone(const ColorEnum& c)
{
    return (c == BLACK) ? animal_shogi::bb_rank1 : animal_shogi::bb_rank4;
}

template <>
template <DirectionEnum Dir>
constexpr animal_shogi::BitBoard animal_shogi::BitBoard::shift() const
{
    constexpr auto bb_fab = animal_shogi::bb_filea | animal_shogi::bb_fileb;
    constexpr auto bb_fbc = animal_shogi::bb_fileb | animal_shogi::bb_filec;
    constexpr BitBoard filemask[] = {
        // clang-format off
        bb_fbc, ~BitBoard(0), bb_fab,
        bb_fbc,               bb_fab,
        bb_fbc, ~BitBoard(0), bb_fab,
        bb_fbc,               bb_fab,
        bb_fbc,               bb_fab,
        // clang-format on
    };
    constexpr auto delta = animal_shogi::Squares::direction_to_delta(Dir);
    if constexpr (delta > 0)
        return (*this & filemask[Dir]) << static_cast<uint>(delta);
    else
        return (*this & filemask[Dir]) >> static_cast<uint>(-delta);
}

template <>
inline animal_shogi::BitBoard animal_shogi::BitBoard::get_attacks_by(
    const animal_shogi::BoardPieceTypeEnum& p,
    const animal_shogi::SquareEnum& sq)
{
    using namespace animal_shogi;
    switch (p) {
    case B_CH:
        return attacks_table[0][sq];
    case B_HE:
        return attacks_table[1][sq];
    case W_CH:
        return attacks_table[2][sq];
    case W_HE:
        return attacks_table[3][sq];
    case B_EL:
    case W_EL:
        return attacks_table[4][sq];
    case B_GI:
    case W_GI:
        return attacks_table[5][sq];
    case B_LI:
    case W_LI:
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
    for (auto&& sq : animal_shogi::Squares::square_array) {
        const auto b = from_square(sq);
        // clang-format off
        attacks_table[0][sq] = b.shift<DIR_N>(); // B_CH
        attacks_table[1][sq] = b.shift<DIR_NW>() | b.shift<DIR_N>() | b.shift<DIR_NE>() | b.shift<DIR_W>() | b.shift<DIR_E>() | b.shift<DIR_S>(); // B_KI
        attacks_table[2][sq] = b.shift<DIR_S>(); // W_CH
        attacks_table[3][sq] = b.shift<DIR_N>() | b.shift<DIR_W>() | b.shift<DIR_E>() | b.shift<DIR_SW>() | b.shift<DIR_S>() | b.shift<DIR_SE>(); // W_KI
        attacks_table[4][sq] = b.shift<DIR_NW>() | b.shift<DIR_NE>() | b.shift<DIR_SW>() | b.shift<DIR_SE>(); // EL
        attacks_table[5][sq] = b.shift<DIR_N>() | b.shift<DIR_W>() | b.shift<DIR_E>() | b.shift<DIR_S>(); // GI
        attacks_table[6][sq] = attacks_table[4][sq] | attacks_table[5][sq]; // LI
        // clang-format on
    }
}

template <>
inline std::uint64_t animal_shogi::State::zobrist_board
    [animal_shogi::Squares::num_squares]
    [num_colors * animal_shogi::Pieces::num_piece_types + 1]
    = {};

template <>
inline std::uint64_t animal_shogi::State::zobrist_stand
    [num_colors][animal_shogi::Pieces::num_stand_piece_types][3]
    = {};

template <>
inline animal_shogi::State& animal_shogi::State::apply(
    const animal_shogi::Move& move, std::uint64_t* const hash)
{
    const auto dst = move.destination();
    auto moving = pop_piece_from_stand_or_board(move, hash);
    const auto captured = Pieces::to_piece_type(m_board[dst]);
    if ((captured != Pieces::NA) && (captured != animal_shogi::LI))
        add_captured_to_stand(Pieces::demote(captured), hash);
    moving = animal_shogi::internal::promote_if_possible(moving, move);
    place_piece_at(dst, moving, hash);
    m_turn = ~m_turn;
    return *this;
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
        for (auto src : Squares::square_array) {
            const auto p = board[src];
            if ((p == Pieces::VOID) || (Pieces::get_color(p) != turn))
                continue;
            for (auto dp = Pieces::get_attack_directions(p); *dp != DIR_NA;) {
                const auto dst = Squares::shift(src, *dp++);
                if (dst == Squares::SQ_NA)
                    continue;
                const auto t = board[dst];
                if (((t == Pieces::VOID) || (Pieces::get_color(t) == ~turn))
                    && BitBoard::get_attacks_by(p, src).is_one(dst))
                    m_legal_moves.emplace_back(dst, src);
            }
        }
        for (auto dst : Squares::square_array) {
            if (!board.is_empty(dst))
                continue;
            for (auto pt : Pieces::stand_piece_array) {
                if (stand.exist(pt))
                    m_legal_moves.emplace_back(Move(dst, pt));
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
inline void animal_shogi::Game::to_feature_map(float* const data) const
{
    constexpr int num_squares = ranks() * files();
    constexpr int sp_types = stand_piece_types();
    constexpr int bp_types = board_piece_types();
    constexpr int ch_half = sp_types + bp_types;
    constexpr int ch = feature_channels();

    const auto turn = get_turn();
    const auto stand_curr = get_stand(turn);
    const auto stand_next = get_stand(~turn);
    const auto board = get_board();

    for (int k = sp_types; k--;) {
        const auto p = animal_shogi::Pieces::stand_piece_array[k];
        const auto num_curr = static_cast<float>(stand_curr.count(p));
        const auto num_next = static_cast<float>(stand_next.count(p));
        for (int i = num_squares; i--;) {
            data[i * ch + k] = num_curr;
            data[i * ch + k + ch_half] = num_next;
        }
    }
    for (int i = num_squares; i--;) {
        const auto sq = static_cast<typename animal_shogi::Squares::SquareEnum>(
            (turn == BLACK) ? i : (num_squares - 1 - i));
        const auto board_piece = board[sq];
        for (int k = bp_types; k--;) {
            data[i * ch + k + sp_types] = 0.f;
            data[i * ch + k + sp_types + ch_half] = 0.f;
        }
        if (board.is_empty(sq))
            continue;
        const auto color = animal_shogi::Pieces::get_color(board_piece);
        const auto piece_type
            = animal_shogi::Pieces::to_piece_type(board_piece);
        auto k = static_cast<int>(piece_type);
        k += (turn == color) ? 0 : ch_half;
        data[i * ch + k + sp_types] = 1.f;
    }
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
