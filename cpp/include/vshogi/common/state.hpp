#ifndef VSHOGI_STATE_HPP
#define VSHOGI_STATE_HPP

#include <random>
#include <string>
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
    using ColoredPiece = typename Config::ColoredPiece;
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
    static constexpr uint num_dir = Config::num_dir;
    static constexpr Square SQ_NA = SHelper::SQ_NA; // NOLINT

    static constexpr std::uint64_t zobrist_hash_for_turn = 0xaaaaaaaaaaaaaaaau;

private:
    BoardType m_board;
    Stands m_stands;
    ColorEnum m_turn; //!< Player to make a move in the current state.
    Square m_checker_locations[2]; //!< Checkers attacking turn player's king.

public:
    State()
        : m_board(), m_stands(),
          m_turn(BLACK), m_checker_locations{SQ_NA, SQ_NA}
    {
    }
    State(const std::string& sfen) : m_board(), m_stands(), m_turn()
    {
        set_sfen(sfen);
        update_checkers();
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
    static constexpr uint feature_channels()
    {
        return 2 * (num_piece_types + num_stand_piece_types);
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
    Square get_checker_location(const uint i = 0u) const
    {
        return m_checker_locations[i];
    }
    bool in_check() const
    {
        return m_checker_locations[0] != SQ_NA;
    }
    bool in_double_check() const
    {
        return m_checker_locations[1] != SQ_NA;
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
        if (move.is_drop()) {
            const PieceType src = move.source_piece();
            const Square dst = move.destination();
            const ColoredPiece p = m_stands.pop_piece_from(m_turn, src, hash);
            m_board.apply(dst, p, hash);
            update_checkers_before_turn_update(dst);
        } else {
            const Square src = move.source_square();
            const Square dst = move.destination();
            const auto captured = m_board.apply(dst, src, move.promote(), hash);
            m_stands.add_captured_piece(captured, hash);
            update_checkers_before_turn_update(dst, src);
        }
        m_turn = ~m_turn;
        if (hash != nullptr)
            *hash ^= zobrist_hash_for_turn;
        return *this;
    }
    void to_feature_map(float* const data) const
    {
        constexpr uint sp_types = num_stand_piece_types;
        constexpr uint unpromoted_piece_types = sp_types + 1; // + OU
        constexpr uint ch_half = sp_types + num_piece_types;
        constexpr uint ch = ch_half * 2;

        const auto& stand_curr = m_stands[m_turn];
        const auto& stand_next = m_stands[~m_turn];

        float num_pieces_curr[sp_types] = {};
        float num_pieces_next[sp_types] = {};
        for (auto pt : EnumIterator<PieceType, num_stand_piece_types>()) {
            num_pieces_curr[pt] = static_cast<float>(stand_curr.count(pt));
            num_pieces_next[pt] = static_cast<float>(stand_next.count(pt));
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
            const auto k = static_cast<uint>(pt)
                           + (m_turn != PHelper::get_color(p)) * ch_half;
            data_ch[k + sp_types] = 1.f;
        }
    }
    std::uint64_t zobrist_hash() const
    {
        auto out = m_stands.zobrist_hash() ^ m_board.zobrist_hash();
        if (m_turn == WHITE)
            out ^= zobrist_hash_for_turn;
        return out;
    }

private:
    State(const BoardType& b, const Stands& s, const ColorEnum& turn)
        : m_board(b), m_stands(s), m_turn(turn)
    {
        update_checkers();
    }
    void append_sfen_turn(std::string& out) const
    {
        out += ((m_turn == BLACK) ? 'b' : 'w');
    }
    void update_checkers()
    {
        std::fill_n(m_checker_locations, 2, SQ_NA);
        uint index = 0u;
        const auto king_sq = m_board.get_king_location(m_turn);
        for (auto dir : EnumIterator<DirectionEnum, num_dir>()) {
            const auto sq = m_board.find_attacker(~m_turn, king_sq, dir);
            if (sq != SQ_NA) {
                m_checker_locations[index++] = sq;
                if (index > 1)
                    break;
            }
        }
    }
    void update_checkers_before_turn_update(const Square& dst)
    {
        const auto enemy_king_sq = m_board.get_king_location(~m_turn);
        const auto dst_dir = SHelper::get_direction(dst, enemy_king_sq);
        const bool check_by_moved
            = is_check_by_moved(enemy_king_sq, dst, dst_dir);

        m_checker_locations[0] = (check_by_moved) ? dst : SQ_NA;
        m_checker_locations[1] = SQ_NA;
    }
    void
    update_checkers_before_turn_update(const Square& dst, const Square& src)
    {
        const auto enemy_king_sq = m_board.get_king_location(~m_turn);
        const auto dst_dir = SHelper::get_direction(dst, enemy_king_sq);
        const auto discovered_checker_sq = find_discovered_checker_location(
            enemy_king_sq, dst_dir, SHelper::get_direction(src, enemy_king_sq));

        const bool check_by_discovered = (discovered_checker_sq != SQ_NA);
        const bool check_by_moved
            = is_check_by_moved(enemy_king_sq, dst, dst_dir);

        if (check_by_moved && check_by_discovered) {
            m_checker_locations[0] = dst;
            m_checker_locations[1] = discovered_checker_sq;
        } else if (check_by_moved) {
            m_checker_locations[0] = dst;
            m_checker_locations[1] = SQ_NA;
        } else if (check_by_discovered) {
            m_checker_locations[0] = discovered_checker_sq;
            m_checker_locations[1] = SQ_NA;
        } else {
            m_checker_locations[0] = SQ_NA;
            m_checker_locations[1] = SQ_NA;
        }
    }
    Square find_discovered_checker_location(
        const Square& enemy_king_sq,
        const DirectionEnum& dst_dir,
        const DirectionEnum& src_dir)
    {
        if ((src_dir == DIR_NA) || (src_dir == dst_dir))
            return SQ_NA;
        return m_board.find_attacker(m_turn, enemy_king_sq, src_dir);
    }
    bool is_check_by_moved(
        const Square& enemy_king_sq,
        const Square& dst,
        const DirectionEnum& dst_dir)
    {
        const BitBoardType attacks
            = BitBoardType::get_attacks_by(m_board[dst], dst);
        if (!attacks.is_one(enemy_king_sq))
            return false;
        if (!PHelper::is_ranging_piece(m_board[dst]))
            return true;
        return !(BitBoardType::get_line_segment(dst, enemy_king_sq)
                 & m_board.get_occupied())
                    .any();
    }
};

} // namespace vshogi

#endif // VSHOGI_STATE_HPP
