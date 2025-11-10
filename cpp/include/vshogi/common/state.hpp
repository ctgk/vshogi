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

template <class P>
class State
{
private:
    using C = Configuration<P>;
    using PieceType = typename C::PieceType;
    using ColoredPiece = typename C::ColoredPiece;
    using Rank = typename C::Rank;
    using Square = typename C::Square;
    using PHelper = Pieces<P>;
    using SHelper = Squares<P>;
    using BitBoardType = BitBoard<P>;
    using Stands = BlackWhiteStands<P>;
    static constexpr uint max_stand_piece_count = C::max_stand_piece_count;
    static constexpr uint num_piece_types = C::num_piece_types;
    static constexpr uint num_stand_piece_types = C::num_stand_piece_types;
    static constexpr uint num_dir = C::num_dir;
    static constexpr ColoredPiece VOID = C::VOID; // NOLINT
    static constexpr std::uint64_t zobrist_hash_for_turn = 0x000000aaaaaaaaaau;

public:
    static constexpr uint num_ranks = C::num_ranks;
    static constexpr uint num_files = C::num_files;
    static constexpr uint num_squares = C::num_squares;

private:
    Board<P> m_board;
    Stands m_stands;
    ColorEnum m_turn; //!< Player to make a move in the current state.
    Square m_checkers[2]; //!< Checkers attacking turn player's king.

public:
    State()
        : m_board(), m_stands(), m_turn(BLACK), m_checkers{C::SQ_NA, C::SQ_NA}
    {
    }
    State(const std::string& sfen) : m_board(), m_stands(), m_turn()
    {
        set_sfen(sfen);
        update_checkers();
    }
    static constexpr uint feature_channels()
    {
        return 2 * (num_piece_types + num_stand_piece_types);
    }
    static constexpr uint num_dlshogi_policy()
    {
        return num_squares * Move<P>::num_policy_per_square();
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
    const Board<P>& get_board() const
    {
        return m_board;
    }
    const Stand<P>& get_stand() const
    {
        return get_stand(m_turn);
    }
    const Stand<P>& get_stand(const ColorEnum c) const
    {
        return m_stands[c];
    }
    ColorEnum get_turn() const
    {
        return m_turn;
    }
    Square get_king_square() const
    {
        return get_king_square(m_turn);
    }
    Square get_king_square(const ColorEnum c) const
    {
        return m_board.get_king_square(c);
    }
    Square get_checker_square(const uint i = 0u) const
    {
        return m_checkers[i];
    }
    bool in_check() const
    {
        return m_checkers[0] != C::SQ_NA;
    }
    bool in_double_check() const
    {
        return m_checkers[1] != C::SQ_NA;
    }
    bool can_apply_drop_move(const PieceType pt = C::NA) const
    {
        if (in_double_check())
            return false;
        if ((pt != C::NA) && (!m_stands[m_turn].exist(pt)))
            return false;
        if (in_check() && (!PHelper::is_ranging_piece(m_board[m_checkers[0]])))
            return false;
        return true;
    }
    bool in_promotion_zone(const Move<P>& m) const
    {
        if (SHelper::in_promotion_zone(m.destination(), m_turn))
            return true;
        if (m.is_drop())
            return false;
        return SHelper::in_promotion_zone(m.source_square(), m_turn);
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
    State& apply(const Move<P>& move, std::uint64_t* const hash = nullptr)
    {
        const Square dst = move.destination();
        if (move.is_drop()) {
            const PieceType src = move.source_piece();
            const ColoredPiece p = m_stands.pop_piece_from(m_turn, src, hash);
            m_board.apply(dst, p, hash);
            update_checkers_before_turn_update(dst);
        } else {
            const Square src = move.source_square();
            const auto captured = m_board.apply(dst, src, move.promote(), hash);
            m_stands.add_captured_piece(captured, hash);
            update_checkers_before_turn_update(dst, src);
        }
        m_turn = ~m_turn;
        if (hash != nullptr)
            *hash ^= zobrist_hash_for_turn;
        return *this;
    }
    State& undo(
        const Move<P>& move,
        const ColoredPiece& captured,
        const Square& checker_sq)
    {
        const Square dst = move.destination();
        if (captured != VOID)
            m_stands.remove_captured_piece(captured);
        if (move.is_drop()) {
            const auto dropped = m_board.apply(dst, captured);
            m_stands.return_dropped_piece(dropped);
        } else {
            const auto src = move.source_square();
            auto moved = m_board.apply(dst, captured);
            if (move.promote())
                moved = PHelper::demote_nocheck(moved);
            m_board.apply(src, moved);
        }
        m_turn = ~m_turn;
        m_checkers[0] = checker_sq;
        m_checkers[1] = C::SQ_NA;
        return *this;
    }
    void to_feature_map(float* const data) const
    {
        constexpr uint sp_types = num_stand_piece_types;
        constexpr uint ch_half = sp_types + num_piece_types;
        constexpr uint ch = ch_half * 2;

        const auto& stand_curr = m_stands[m_turn];
        const auto& stand_next = m_stands[~m_turn];

        float num_pieces_curr[sp_types] = {};
        float num_pieces_next[sp_types] = {};
        for (auto pt : C::stand_piece_type_iterator()) {
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
    std::uint64_t zobrist_hash(const bool& hash_stands = true) const
    {
        auto out = m_board.zobrist_hash();
        if (hash_stands)
            out ^= m_stands.get_zobrist_hash();
        if (m_turn == WHITE)
            out ^= zobrist_hash_for_turn;
        return out;
    }
    std::uint64_t hash_stands() const
    {
        return m_stands.get_zobrist_hash();
    }
    BitBoardType find_pinned() const
    {
        if (in_double_check())
            return BitBoardType();
        return m_board.find_pinned(m_turn);
    }
    BitBoardType compute_king_movable() const
    {
        return compute_king_movable(~BitBoardType());
    }
    BitBoardType compute_king_movable(BitBoardType movable) const
    {
        for (uint ii = 0u; ii < 2u; ++ii) {
            const Square& sq = m_checkers[ii];
            if (sq == C::SQ_NA)
                break;
            movable &= ~m_board.get_attacks_by_nocheck(sq);
        }
        return m_board.compute_king_movable(m_turn, movable);
    }
    bool is_declined_promotion(const Move<P>& move) const
    {
        if (move.is_drop())
            return false;
        if (move.promote())
            return false;
        if (!in_promotion_zone(move))
            return false;
        return PHelper::is_promotion_fully_superior(
            m_board[move.source_square()]);
    }

private:
    State(const Board<P>& b, const Stands& s, const ColorEnum& turn)
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
        std::fill_n(m_checkers, 2, C::SQ_NA);
        uint index = 0u;
        const auto king_sq = m_board.get_king_square(m_turn);
        for (auto dir : C::direction_iterator()) {
            const auto sq = m_board.find_attacker(~m_turn, king_sq, dir);
            if (sq != C::SQ_NA) {
                m_checkers[index++] = sq;
                if (index > 1)
                    break;
            }
        }
    }
    void update_checkers_before_turn_update(const Square& dst)
    {
        const auto enemy_king_sq = m_board.get_king_square(~m_turn);
        const bool check_by_moved = is_check_by_moved(enemy_king_sq, dst);

        m_checkers[0] = (check_by_moved) ? dst : C::SQ_NA;
        m_checkers[1] = C::SQ_NA;
    }
    void
    update_checkers_before_turn_update(const Square& dst, const Square& src)
    {
        const auto enemy_king_sq = m_board.get_king_square(~m_turn);
        if (enemy_king_sq == C::SQ_NA) {
            m_checkers[0] = C::SQ_NA;
            m_checkers[1] = C::SQ_NA;
            return;
        }
        const auto dst_dir = SHelper::direction(enemy_king_sq, dst);
        const auto discovered_checker_sq = find_discovered_checker_square(
            enemy_king_sq, dst_dir, SHelper::direction(enemy_king_sq, src));

        const bool check_by_discovered = (discovered_checker_sq != C::SQ_NA);
        const bool check_by_moved = is_check_by_moved(enemy_king_sq, dst);

        if (check_by_moved && check_by_discovered) {
            m_checkers[0] = dst;
            m_checkers[1] = discovered_checker_sq;
        } else if (check_by_moved) {
            m_checkers[0] = dst;
            m_checkers[1] = C::SQ_NA;
        } else if (check_by_discovered) {
            m_checkers[0] = discovered_checker_sq;
            m_checkers[1] = C::SQ_NA;
        } else {
            m_checkers[0] = C::SQ_NA;
            m_checkers[1] = C::SQ_NA;
        }
    }
    Square find_discovered_checker_square(
        const Square& enemy_king_sq,
        const DirectionEnum& dst_dir,
        const DirectionEnum& src_dir)
    {
        if ((src_dir == DIR_NA) || (src_dir == dst_dir))
            return C::SQ_NA;
        return m_board.find_attacker(m_turn, enemy_king_sq, src_dir);
    }
    bool is_check_by_moved(const Square& enemy_king_sq, const Square& dst)
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
    static void fill_ms24b_with(
        std::uint64_t* const hash, const ColoredPiece& p, const Move<P>& m)
    {
        if (hash == nullptr)
            return;
        *hash <<= 24u;
        *hash >>= 24u;

        static_assert(sizeof(ColoredPiece) == sizeof(std::uint8_t));
        static_assert(sizeof(Move<P>) == sizeof(std::uint16_t));
        *hash ^= static_cast<std::uint64_t>(p) << (64u - 8u);
        *hash ^= static_cast<std::uint64_t>(m.hash()) << (64u - 24u);
    }
};

} // namespace vshogi

#endif // VSHOGI_STATE_HPP
