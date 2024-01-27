#ifndef VSHOGI_STATE_HPP
#define VSHOGI_STATE_HPP

#include <random>
#include <string>
#include <type_traits>
#include <vector>

#include "vshogi/common/color.hpp"
#include "vshogi/common/direction.hpp"

namespace vshogi
{

template <class Board, class Stands, class Move, uint MaxStandPieceCount>
class State
{
    static_assert(std::is_same<
                  typename Board::PiecesType,
                  typename Move::PiecesType>::value);
    static_assert(std::is_same<
                  typename Board::SquaresType,
                  typename Move::SquaresType>::value);

private:
    using BitBoard = typename Board::BitBoardType;
    using Squares = typename Board::SquaresType;
    using Pieces = typename Board::PiecesType;
    using Stand = typename Stands::StandType;

public:
    using BoardType = Board;
    using BitBoardType = typename Board::BitBoardType;
    using SquaresType = typename Board::SquaresType;
    using PiecesType = typename Board::PiecesType;
    using StandType = typename Stands::StandType;
    using MoveType = Move;
    using PieceTypeEnum = typename Pieces::PieceTypeEnum;
    using BoardPieceTypeEnum = typename Pieces::BoardPieceTypeEnum;
    using RankEnum = typename Squares::RankEnum;
    using SquareEnum = typename Squares::SquareEnum;

private:
    static std::uint64_t
        zobrist_board[Squares::num_squares]
                     [num_colors * Pieces::num_piece_types + 1];
    static std::uint64_t zobrist_stand[num_colors]
                                      [Pieces::num_stand_piece_types]
                                      [MaxStandPieceCount + 1];

    Board m_board;
    Stands m_stands;
    ColorEnum m_turn; //!< Player to make a move in the current state.

public:
    State() : m_board(), m_stands(), m_turn(BLACK)
    {
    }
    State(const std::string& sfen) : m_board(), m_stands(), m_turn()
    {
        set_sfen(sfen);
    }
    bool operator==(const State& other) const
    {
        return (m_board == other.m_board) && (m_stands == other.m_stands)
               && (m_turn == other.m_turn);
    }
    bool operator!=(const State& other) const
    {
        return (m_board != other.m_board) || (m_stands != other.m_stands)
               || (m_turn != other.m_turn);
    }
    const Board& get_board() const
    {
        return m_board;
    }
    const Stand& get_stand(const ColorEnum c) const
    {
        return m_stands[c];
    }
    ColorEnum get_turn() const
    {
        return m_turn;
    }
    void set_sfen(const std::string& sfen)
    {
        auto s = sfen.c_str();
        s = m_board.set_sfen(s);
        {
            (*s == 'b') ? m_turn = BLACK : m_turn = WHITE;
            s += 2;
        }
        m_stands.set_sfen(s);
    }
    std::string to_sfen() const
    {
        auto out = std::string();
        out.reserve(Squares::num_squares);
        m_board.append_sfen(out);
        out += ' ';
        append_sfen_turn(out);
        out += ' ';
        m_stands.append_sfen(out);
        return out;
    }
    State hflip() const
    {
        return State(m_board.hflip(), m_stands, m_turn);
    }
    State& apply(const Move& move, std::uint64_t* const hash = nullptr)
    {
        const auto dst = move.destination();
        auto moving = pop_piece_from_stand_or_board(move, hash);
        const auto captured = Pieces::to_piece_type(m_board[dst]);
        if (captured != Pieces::NA)
            add_captured_to_stand(Pieces::demote(captured), hash);
        if (move.promote())
            moving = Pieces::promote(moving);
        place_piece_at(dst, moving, hash);
        m_turn = ~m_turn;
        return *this;
    }
    static void init_zobrist_table()
    {
        std::random_device dev;
        std::mt19937_64 rng(dev());
        std::uniform_int_distribution<std::uint64_t> dist;
        for (auto& c : color_array) {
            for (auto& pt : Pieces::stand_piece_array) {
                for (uint num = 0; num < MaxStandPieceCount + 1; ++num) {
                    zobrist_stand[c][pt][num] = dist(rng);
                }
            }
        }
        constexpr int num_pieces = num_colors * Pieces::num_piece_types + 1;
        for (auto& sq : Squares::square_array) {
            for (int ii = 0; ii < num_pieces; ++ii) {
                zobrist_board[sq][ii] = dist(rng);
            }
        }
    }
    std::uint64_t zobrist_hash() const
    {
        std::uint64_t out = static_cast<std::uint64_t>(0);
        for (auto& c : color_array) {
            for (auto& pt : Pieces::stand_piece_array) {
                const auto num = m_stands[c].count(pt);
                out ^= zobrist_stand[c][pt][num];
            }
        }
        for (auto& sq : Squares::square_array) {
            out ^= zobrist_board[sq][to_index(m_board[sq])];
        }
        return out;
    }

private:
    State(const Board& b, const Stands& s, const ColorEnum& turn)
        : m_board(b), m_stands(s), m_turn(turn)
    {
    }
    void append_sfen_turn(std::string& out) const
    {
        out += ((m_turn == BLACK) ? 'b' : 'w');
    }
    BoardPieceTypeEnum
    pop_piece_from_stand_or_board(const Move& move, std::uint64_t* const hash)
    {
        if (move.is_drop()) {
            const auto src = move.source_piece();
            m_stands[m_turn].subtract(src);
            if (hash != nullptr) {
                const auto num_after = m_stands[m_turn].count(src);
                const auto num_before = num_after + 1;
                *hash ^= zobrist_stand[m_turn][src][num_before];
                *hash ^= zobrist_stand[m_turn][src][num_after];
            }
            return Pieces::to_board_piece(m_turn, src);
        } else {
            const auto src = move.source_square();
            const auto out = m_board[src];
            m_board[src] = Pieces::VOID;
            if (hash != nullptr) {
                *hash ^= zobrist_board[src][to_index(out)];
                *hash ^= zobrist_board[src][to_index(Pieces::VOID)];
            }
            return out;
        }
    }
    void add_captured_to_stand(
        const typename Pieces::PieceTypeEnum& p, std::uint64_t* const hash)
    {
        m_stands[m_turn].add(p);
        if (hash != nullptr) {
            const auto num_after = m_stands[m_turn].count(p);
            const auto num_before = num_after - 1;
            *hash ^= zobrist_stand[m_turn][p][num_before];
            *hash ^= zobrist_stand[m_turn][p][num_after];
        }
    }
    void place_piece_at(
        const SquareEnum& sq,
        const BoardPieceTypeEnum& p,
        std::uint64_t* const hash)
    {
        if (hash != nullptr) {
            *hash ^= zobrist_board[sq][to_index(m_board[sq])];
            *hash ^= zobrist_board[sq][to_index(p)];
        }
        m_board[sq] = p;
    }
    static uint to_index(const typename Pieces::BoardPieceTypeEnum& p)
    {
        if (p == Pieces::VOID)
            return 2u * Pieces::num_piece_types;
        uint out = Pieces::demote(Pieces::to_piece_type(p));
        out += (Pieces::get_color(p) == WHITE) * Pieces::num_piece_types;
        out += Pieces::is_promoted(p) * (Pieces::num_stand_piece_types + 1);
        return out;
    }
};

} // namespace vshogi

#endif // VSHOGI_STATE_HPP
