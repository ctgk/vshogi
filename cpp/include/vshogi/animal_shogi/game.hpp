#ifndef VSHOGI_ANIMAL_SHOGI_GAME_HPP
#define VSHOGI_ANIMAL_SHOGI_GAME_HPP

#include <cstdint>
#include <string>
#include <vector>

#include "vshogi/animal_shogi/piece.hpp"
#include "vshogi/animal_shogi/squares.hpp"
#include "vshogi/animal_shogi/state.hpp"
#include "vshogi/game.hpp"
#include "vshogi/result.hpp"

namespace vshogi::animal_shogi
{

using Game = vshogi::Game<State, 2>;

namespace internal
{

inline ResultEnum move_result(
    const Move move,
    const Pieces::BoardPieceTypeEnum moving,
    const Pieces::BoardPieceTypeEnum captured)
{
    if (captured == Pieces::B_LI)
        return WHITE_WIN;
    if (captured == Pieces::W_LI)
        return BLACK_WIN;
    const auto r_dst = Squares::to_rank(move.destination());
    if ((moving == Pieces::B_LI) && (r_dst == Squares::RANK1))
        return BLACK_WIN;
    if ((moving == Pieces::W_LI) && (r_dst == Squares::RANK4))
        return WHITE_WIN;
    return ONGOING;
}

} // namespace internal

} // namespace vshogi::animal_shogi

namespace vshogi
{

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
    const auto turn = get_turn();
    const auto& board = get_board();
    const auto& stand = get_stand(turn);
    m_legal_moves.clear();
    for (auto src : Squares::square_array) {
        const auto p = board[src];
        if ((p == Pieces::VOID) || (Pieces::get_color(p) != turn))
            continue;
        for (auto dir : Squares::direction_array) {
            const auto dst = Squares::shift(src, dir);
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
    update_result();
}

template <>
inline animal_shogi::Game&
animal_shogi::Game::apply(const animal_shogi::Move move)
{
    const auto illegal = !is_legal(move);
    m_record.emplace_back(std::make_pair(m_zobrist_hash, move));
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
inline animal_shogi::Game::Game()
    : m_current_state(), m_record(), m_legal_moves(), m_result(ONGOING),
      m_zobrist_hash(m_current_state.zobrist_hash()),
      m_initial_sfen_without_ply(m_current_state.to_sfen()),
      m_half_num_pieces{0, 0}, m_initial_points{0, 0}
{
    m_record.reserve(128);
    update_internals();
}

template <>
inline animal_shogi::Game::Game(const std::string& sfen)
    : m_current_state(sfen), m_record(), m_legal_moves(), m_result(ONGOING),
      m_zobrist_hash(m_current_state.zobrist_hash()),
      m_initial_sfen_without_ply(m_current_state.to_sfen()),
      m_half_num_pieces{0, 0}, m_initial_points{0, 0}
{
    m_record.reserve(128);
    update_internals();
}

} // namespace vshogi

#endif // VSHOGI_ANIMAL_SHOGI_GAME_HPP
