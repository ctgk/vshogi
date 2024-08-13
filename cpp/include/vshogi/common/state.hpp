#ifndef VSHOGI_STATE_HPP
#define VSHOGI_STATE_HPP

#include <random>
#include <string>
#include <type_traits>
#include <vector>

#include "vshogi/common/bitboard.hpp"
#include "vshogi/common/board.hpp"
#include "vshogi/common/color.hpp"
#include "vshogi/common/direction.hpp"
#include "vshogi/common/move.hpp"
#include "vshogi/common/pieces.hpp"
#include "vshogi/common/squares.hpp"
#include "vshogi/common/stand.hpp"

namespace vshogi
{

template <class Config>
class State
{
private:
    using PieceType = typename Config::PieceType;
    using BoardPieceType = typename Config::BoardPieceType;
    using Rank = typename Config::Rank;
    using Square = typename Config::Square;
    using PHelper = Pieces<Config>;
    using SHelper = Squares<Config>;
    using BitBoardType = BitBoard<Config>;
    using BoardType = Board<Config>;
    using MoveType = Move<Config>;
    using Stands = BlackWhiteStands<Config>;
    using StandType = Stand<Config>;

    static constexpr uint max_stand_piece_count = Config::max_stand_piece_count;
    static constexpr uint num_squares = Config::num_squares;
    static constexpr uint num_piece_types = Config::num_piece_types;
    static constexpr uint num_stand_piece_types = Config::num_stand_piece_types;
    static constexpr uint num_ranks = Config::num_ranks;
    static constexpr uint num_files = Config::num_files;

private:
    static std::uint64_t zobrist_board[num_squares]
                                      [num_colors * num_piece_types + 1];
    static std::uint64_t zobrist_stand[num_colors][num_stand_piece_types]
                                      [max_stand_piece_count + 1];

    BoardType m_board;
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
    static constexpr uint ranks()
    {
        return num_ranks;
    }
    static constexpr uint files()
    {
        return num_files;
    }
    static constexpr uint squares()
    {
        return num_squares;
    }
    static constexpr uint board_piece_types()
    {
        return num_piece_types;
    }
    static constexpr uint stand_piece_types()
    {
        return num_stand_piece_types;
    }
    static constexpr uint feature_channels()
    {
        return 2 * (board_piece_types() + stand_piece_types());
    }
    static constexpr uint num_dlshogi_policy()
    {
        return squares() * MoveType::num_policy_per_square();
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
    const BoardType& get_board() const
    {
        return m_board;
    }
    const StandType& get_stand(const ColorEnum c) const
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
        out.reserve(num_squares);
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
    State& apply(const MoveType& move, std::uint64_t* const hash = nullptr)
    {
        const auto dst = move.destination();
        auto moving = pop_piece_from_stand_or_board(move, hash);
        const auto captured = PHelper::to_piece_type(m_board[dst]);
        if (captured != PHelper::NA)
            add_captured_to_stand(PHelper::demote(captured), hash);
        if (move.promote())
            moving = PHelper::promote_nocheck(moving);
        place_piece_at(dst, moving, hash);
        m_turn = ~m_turn;
        return *this;
    }
    void to_feature_map(float* const data) const
    {
        constexpr uint sp_types = stand_piece_types();
        constexpr uint unpromoted_piece_types = sp_types + 1; // + OU
        constexpr uint ch_half = sp_types + board_piece_types();
        constexpr uint ch = ch_half * 2;

        const auto& stand_curr = m_stands[m_turn];
        const auto& stand_next = m_stands[~m_turn];

        float num_pieces_curr[sp_types] = {};
        float num_pieces_next[sp_types] = {};
        for (uint k = sp_types; k--;) {
            const auto p = PHelper::stand_piece_array[k];
            num_pieces_curr[k] = static_cast<float>(stand_curr.count(p));
            num_pieces_next[k] = static_cast<float>(stand_next.count(p));
        }

        std::fill_n(data, num_squares * ch, 0.f);
        for (uint i = num_squares; i--;) {
            float* const data_ch = data + i * ch;
            for (uint k = sp_types; k--;)
                data_ch[k + ch_half] = num_pieces_next[k];
            for (uint k = sp_types; k--;)
                data_ch[k] = num_pieces_curr[k];

            const auto sq = static_cast<Square>(
                (m_turn == BLACK) ? i : (num_squares - 1 - i));
            if (m_board.is_empty(sq))
                continue;
            const auto& p = m_board[sq];
            const auto pt = PHelper::to_piece_type(p);
            const auto k = static_cast<uint>(PHelper::demote(pt))
                           + PHelper::is_promoted(pt) * unpromoted_piece_types
                           + (m_turn != PHelper::get_color(p)) * ch_half;
            data_ch[k + sp_types] = 1.f;
        }
    }
    static void init_zobrist_table()
    {
        std::random_device dev;
        std::mt19937_64 rng(dev());
        std::uniform_int_distribution<std::uint64_t> dist;
        for (auto& c : color_array) {
            for (auto& pt : PHelper::stand_piece_array) {
                for (uint num = 0; num < max_stand_piece_count + 1; ++num) {
                    zobrist_stand[c][pt][num] = dist(rng);
                }
            }
        }
        constexpr int num_pieces = num_colors * num_piece_types + 1;
        for (auto& sq : SHelper::square_array) {
            for (int ii = 0; ii < num_pieces; ++ii) {
                zobrist_board[sq][ii] = dist(rng);
            }
        }
    }
    std::uint64_t zobrist_hash_board() const
    {
        std::uint64_t out = static_cast<std::uint64_t>(0);
        for (auto&& sq : SHelper::square_array) {
            out ^= zobrist_board[sq][to_index(m_board[sq])];
        }
        return out;
    }
    std::uint64_t zobrist_hash() const
    {
        std::uint64_t out = static_cast<std::uint64_t>(0);
        for (auto& c : color_array) {
            for (auto& pt : PHelper::stand_piece_array) {
                const auto num = m_stands[c].count(pt);
                out ^= zobrist_stand[c][pt][num];
            }
        }
        out ^= zobrist_hash_board();
        return out;
    }

private:
    State(const BoardType& b, const Stands& s, const ColorEnum& turn)
        : m_board(b), m_stands(s), m_turn(turn)
    {
    }
    void append_sfen_turn(std::string& out) const
    {
        out += ((m_turn == BLACK) ? 'b' : 'w');
    }
    BoardPieceType pop_piece_from_stand_or_board(
        const MoveType& move, std::uint64_t* const hash)
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
            return PHelper::to_board_piece(m_turn, src);
        } else {
            const auto src = move.source_square();
            const auto out = m_board.apply(src, PHelper::VOID);
            if (hash != nullptr) {
                *hash ^= zobrist_board[src][to_index(out)];
                *hash ^= zobrist_board[src][to_index(PHelper::VOID)];
            }
            return out;
        }
    }
    void add_captured_to_stand(const PieceType& p, std::uint64_t* const hash)
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
        const Square& sq, const BoardPieceType& p, std::uint64_t* const hash)
    {
        if (hash != nullptr) {
            *hash ^= zobrist_board[sq][to_index(m_board[sq])];
            *hash ^= zobrist_board[sq][to_index(p)];
        }
        m_board.apply(sq, p);
    }
    static uint to_index(const BoardPieceType& p)
    {
        if (p == PHelper::VOID)
            return 2u * num_piece_types;
        uint out = PHelper::demote(PHelper::to_piece_type(p));
        out += (PHelper::get_color(p) == WHITE) * num_piece_types;
        out += PHelper::is_promoted(p) * (num_stand_piece_types + 1);
        return out;
    }
};

} // namespace vshogi

#endif // VSHOGI_STATE_HPP
