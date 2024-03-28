#ifndef VSHOGI_GAME_HPP
#define VSHOGI_GAME_HPP

#include <algorithm>
#include <string>
#include <vector>

#include "vshogi/common/color.hpp"
#include "vshogi/common/direction.hpp"
#include "vshogi/common/result.hpp"

namespace vshogi
{

namespace internal
{

template <class State>
uint num_pieces(const State& s, const ColorEnum& c)
{
    using Squares = typename State::SquaresType;
    using Pieces = typename State::PiecesType;
    const auto& board = s.get_board();
    const auto& stand = s.get_stand(c);
    uint out = 0u;
    for (auto& sq : Squares::square_array) {
        const auto p = board[sq];
        if (p == Pieces::VOID)
            continue;
        if (Pieces::get_color(p) == c)
            out += 1U;
    }
    for (auto& pt : Pieces::stand_piece_array) {
        out += stand.count(pt);
    }
    return out;
}

template <class State>
uint total_point(const State& s, const ColorEnum& c)
{
    using Squares = typename State::SquaresType;
    using Pieces = typename State::PiecesType;
    uint out = 0u;
    const auto& board = s.get_board();
    const auto& stand = s.get_stand(c);
    for (auto& sq : Squares::square_array) {
        const auto p = board[sq];
        if (p == Pieces::VOID)
            continue;
        if (Pieces::get_color(p) == c)
            out += Pieces::get_point(p);
    }
    for (auto& pt : Pieces::stand_piece_array) {
        out += stand.count(pt) * Pieces::get_point(pt);
    }
    return out;
}

} // namespace internal

template <class State, uint MaxAcceptableRepetition = 3>
class Game
{
public:
    using Board = typename State::BoardType;
    using BitBoard = typename State::BitBoardType;
    using Stand = typename State::StandType;
    using Move = typename State::MoveType;
    using Squares = typename State::SquaresType;
    using SquareEnum = typename Squares::SquareEnum;
    using Pieces = typename State::PiecesType;

private:
    State m_current_state;

    std::vector<std::uint64_t> m_zobrist_hash_list;
    std::vector<Move> m_move_list;
    ResultEnum m_result;
    std::uint64_t m_zobrist_hash;
    const std::string m_initial_sfen_without_ply;
    const uint m_half_num_pieces[2];
    const uint m_initial_points[2];
    BitBoard m_occupied[num_colors + 1];
    SquareEnum m_king_locations[num_colors];
    SquareEnum m_checker_locations[2];

public:
    Game() : Game(State())
    {
    }
    Game(const std::string& sfen) : Game(State(sfen))
    {
    }
    static constexpr uint ranks()
    {
        return Squares::num_ranks;
    }
    static constexpr uint files()
    {
        return Squares::num_files;
    }
    static constexpr uint board_piece_types()
    {
        return sizeof(Pieces::piece_array) / sizeof(Pieces::piece_array[0]);
    }
    static constexpr uint stand_piece_types()
    {
        return sizeof(Pieces::stand_piece_array)
               / sizeof(Pieces::stand_piece_array[0]);
    }
    static constexpr uint feature_channels()
    {
        // 2-player * (board-piece-types + stand-piece-types)
        return 2 * (board_piece_types() + stand_piece_types());
    }
    static constexpr uint num_dlshogi_policy()
    {
        return ranks() * files() * Move::num_policy_per_square();
    }
    ColorEnum get_turn() const
    {
        return m_current_state.get_turn();
    }
    const Board& get_board() const
    {
        return m_current_state.get_board();
    }
    const Stand& get_stand(const ColorEnum c) const
    {
        return m_current_state.get_stand(c);
    }
    const SquareEnum& get_king_location(const ColorEnum& c) const
    {
        return m_king_locations[c];
    }
    const BitBoard& get_occupied(const ColorEnum& c) const
    {
        return m_occupied[c];
    }
    const BitBoard& get_occupied() const
    {
        return m_occupied[2];
    }
    ResultEnum get_result() const
    {
        return m_result;
    }
    std::string to_sfen(const bool include_move_count = true) const
    {
        if (include_move_count)
            return m_current_state.to_sfen() + " "
                   + std::to_string(m_move_list.size() + 1);
        else
            return m_current_state.to_sfen();
    }
    std::size_t record_length() const
    {
        return m_move_list.size();
    }
    Move get_move_at(const std::size_t n) const
    {
        return m_move_list[n];
    }
    std::string
    get_sfen_at(const std::size_t n, const bool include_move_count = true) const
    {
        auto s = State(m_initial_sfen_without_ply);
        for (std::size_t ii = 0; ii < n; ++ii)
            s.apply(m_move_list[ii]);
        if (include_move_count)
            return s.to_sfen() + ' ' + std::to_string(n + 1);
        return s.to_sfen();
    }

    /**
     * @brief Return new game with horizontally flipped current positions.
     * @note Returned game does not have game records of the original.
     * @return Game New game with horizontally flipped current positions.
     */
    Game hflip() const
    {
        return Game(m_current_state.hflip());
    }
    Game& apply(const Move& move, const bool& is_legal = true)
    {
        if ((m_result == ONGOING) && (!is_legal)) {
            add_record_and_update_state(move);
            m_result = (get_turn() == BLACK) ? BLACK_WIN : WHITE_WIN;
        } else {
            add_record_and_update_state(move);
            update_internals(move);
        }
        return *this;
    }

    /**
     * @brief Whether current turn player's king is in check or not.
     *
     * @return true Current turn player's king is in check.
     * @return false Current turn player's king is not in check.
     */
    bool in_check() const
    {
        return m_checker_locations[0] != Squares::SQ_NA;
    }

    bool in_double_check() const
    {
        return m_checker_locations[1] != Squares::SQ_NA;
    }

    const SquareEnum& get_checker_location(const uint index) const
    {
        return m_checker_locations[index];
    }

    void to_feature_map(float* const data) const
    {
        constexpr int num_squares = ranks() * files();
        constexpr int sp_types = stand_piece_types();
        constexpr int bp_types = board_piece_types();
        constexpr int unpromoted_piece_types = sp_types + 1; // + OU
        constexpr int ch_half = sp_types + bp_types;
        constexpr int ch = feature_channels();

        const auto turn = get_turn();
        const auto stand_curr = get_stand(turn);
        const auto stand_next = get_stand(~turn);
        const auto board = get_board();

        for (int k = sp_types; k--;) {
            const auto p = Pieces::stand_piece_array[k];
            const auto num_curr = static_cast<float>(stand_curr.count(p));
            const auto num_next = static_cast<float>(stand_next.count(p));
            for (int i = num_squares; i--;) {
                data[i * ch + k] = num_curr;
                data[i * ch + k + ch_half] = num_next;
            }
        }
        for (int i = num_squares; i--;) {
            const auto sq = static_cast<typename Squares::SquareEnum>(
                (turn == BLACK) ? i : (num_squares - 1 - i));
            const auto board_piece = board[sq];
            for (int k = bp_types; k--;) {
                data[i * ch + k + sp_types] = 0.f;
                data[i * ch + k + sp_types + ch_half] = 0.f;
            }
            if (board.is_empty(sq))
                continue;
            const auto color = Pieces::get_color(board_piece);
            const auto piece_type = Pieces::to_piece_type(board_piece);
            auto k = static_cast<int>(Pieces::demote(piece_type));
            k += (Pieces::is_promoted(piece_type)) ? unpromoted_piece_types : 0;
            k += (turn == color) ? 0 : ch_half;
            data[i * ch + k + sp_types] = 1.f;
        }
    }

protected:
    Game(const State& s)
        : m_current_state(s), m_zobrist_hash_list(), m_move_list(),
          m_result(ONGOING), m_zobrist_hash(m_current_state.zobrist_hash()),
          m_initial_sfen_without_ply(m_current_state.to_sfen()),
          m_half_num_pieces{
              internal::num_pieces(m_current_state, BLACK) / 2,
              internal::num_pieces(m_current_state, WHITE) / 2},
          m_initial_points{
              internal::total_point(m_current_state, BLACK),
              internal::total_point(m_current_state, WHITE)}
    {
        m_zobrist_hash_list.reserve(128);
        m_move_list.reserve(128);
        update_internals();
    }
    void add_record_and_update_state(const Move& move)
    {
        m_zobrist_hash_list.emplace_back(m_zobrist_hash);
        m_move_list.emplace_back(move);
        m_current_state.apply(move, &m_zobrist_hash);
    }
    void update_internals()
    {
        update_king_occupied_checkers();
        update_result();
    }
    void update_internals(const Move& move)
    {
        update_king_occupied_checkers(move);
        update_result();
    }

protected:
    void update_king_occupied_checkers()
    {
        m_king_locations[BLACK] = Squares::SQ_NA;
        m_king_locations[WHITE] = Squares::SQ_NA;
        m_occupied[BLACK] = BitBoard();
        m_occupied[WHITE] = BitBoard();
        const auto turn = get_turn();
        const auto& board = get_board();
        for (auto sq : Squares::square_array) {
            const auto p = board[sq];
            if (p == Pieces::VOID)
                continue;
            const auto c = Pieces::get_color(p);
            if (c == BLACK)
                m_occupied[BLACK] |= BitBoard::from_square(sq);
            else if (c == WHITE)
                m_occupied[WHITE] |= BitBoard::from_square(sq);
            if (p == Pieces::B_OU)
                m_king_locations[BLACK] = sq;
            else if (p == Pieces::W_OU)
                m_king_locations[WHITE] = sq;
        }
        m_occupied[2] = m_occupied[BLACK] | m_occupied[WHITE];
        m_checker_locations[0] = Squares::SQ_NA;
        m_checker_locations[1] = Squares::SQ_NA;
        int index = 0;
        for (auto dir : Squares::direction_array) {
            const auto checker_sq
                = board.find_attacker(~turn, m_king_locations[turn], dir);
            if (checker_sq != Squares::SQ_NA) {
                m_checker_locations[index++] = checker_sq;
                if (index > 1)
                    break;
            }
        }
    }
    void update_king_occupied_checkers(const Move& move)
    {
        const auto turn = get_turn();
        const auto& board = get_board();
        const auto dst = move.destination();
        const auto moved = board[dst];
        const auto king_sq = m_king_locations[turn];
        auto discovered_checker_location = Squares::SQ_NA;
        const auto addition = BitBoard::from_square(dst);
        m_occupied[turn] &= (~addition);
        m_occupied[~turn] |= addition;
        m_occupied[2] |= addition;
        if (!move.is_drop()) {
            const auto src = move.source_square();
            const auto dir = Squares::get_direction(src, king_sq);
            const auto deletion = ~BitBoard::from_square(src);
            m_occupied[~turn] &= deletion;
            m_occupied[2] &= deletion;
            if ((dir != DIR_NA)
                && (dir != Squares::get_direction(dst, king_sq)))
                discovered_checker_location
                    = board.find_attacker(~turn, king_sq, dir);
            if (Pieces::OU == Pieces::to_piece_type(moved))
                m_king_locations[~turn] = dst;
        }

        const bool check_by_moved
            = BitBoard::get_attacks_by(moved, dst, m_occupied[2])
                  .is_one(m_king_locations[turn]);
        const bool check_by_discovered
            = (discovered_checker_location != Squares::SQ_NA);
        if (check_by_moved && check_by_discovered) {
            m_checker_locations[0] = dst;
            m_checker_locations[1] = discovered_checker_location;
        } else if (check_by_moved) {
            m_checker_locations[0] = dst;
            m_checker_locations[1] = Squares::SQ_NA;
        } else if (check_by_discovered) {
            m_checker_locations[0] = discovered_checker_location;
            m_checker_locations[1] = Squares::SQ_NA;
        } else {
            m_checker_locations[0] = Squares::SQ_NA;
            m_checker_locations[1] = Squares::SQ_NA;
        }
    }

protected:
    void update_result()
    {
        m_result = ONGOING;
        const auto turn = get_turn();
        if (m_legal_moves.empty())
            m_result = (turn == BLACK) ? WHITE_WIN : BLACK_WIN;
        if (is_repetitions()) {
            if (m_checker_locations[0] != Squares::SQ_NA)
                m_result = (turn == BLACK) ? BLACK_WIN : WHITE_WIN;
            else
                m_result = DRAW;
        }
        if (can_declare_win_by_king_enter())
            m_result = (turn == BLACK) ? BLACK_WIN : WHITE_WIN;
        if (m_result != ONGOING)
            m_legal_moves.clear();
    }
    bool is_repetitions() const
    {
        uint num = 1u;
        const int n = static_cast<int>(m_move_list.size());
        for (int ii = n - 4; ii >= 0; ii -= 2) {
            const uint index = static_cast<uint>(ii);
            num += (m_zobrist_hash == m_zobrist_hash_list[index]);
            if (num > MaxAcceptableRepetition)
                return true;
        }
        return false;
    }
    bool can_declare_win_by_king_enter() const
    {
        const auto turn = get_turn();
        // http://www2.computer-shogi.org/wcsc17/rule_e.html
        // (1) The King of the declaring side is in the third rank or beyond.
        if (!Squares::in_promotion_zone(m_king_locations[turn], turn))
            return false;

        // (4) There is no check on the King of the declaring side.
        if (m_checker_locations[0] != Squares::SQ_NA)
            return false;

        const auto promo_zone_mask = BitBoard::get_promotion_zone(turn);
        const auto piece_mask = (promo_zone_mask & m_occupied[turn]);
        const uint num_pieces_in_zone = piece_mask.hamming_weight();

        // (3) The declaring side has 10 or more pieces other than the King in
        // the third rank or beyond.
        if (num_pieces_in_zone <= m_half_num_pieces[turn])
            return false;

        // (2) The declaring side has 28 (the first player (sente, black)) or
        // 27 (the second player (gote, white)) piece points or more.
        if (turn == BLACK)
            return count_point_in(turn, piece_mask) > m_initial_points[turn];
        else
            return count_point_in(turn, piece_mask) >= m_initial_points[turn];
    }
    uint count_point_in(const ColorEnum& c, const BitBoard& piece_mask) const
    {
        uint out = 0;
        const auto& board = get_board();
        for (auto& sq : Squares::square_array) {
            if (piece_mask.is_one(sq)) {
                out += Pieces::get_point(board[sq]);
            }
        }
        const auto& stand = get_stand(c);
        for (auto& pt : Pieces::stand_piece_array) {
            out += stand.count(pt) * Pieces::get_point(pt);
        }
        return out;
    }

public:
    bool is_drop_pawn_mate(const SquareEnum& dst) const
    {
        const auto turn = get_turn();
        const auto& board = get_board();
        const auto pawn = Pieces::to_board_piece(turn, Pieces::FU);
        const auto enemy_king_sq = m_king_locations[~turn];
        const auto& enemy_mask = m_occupied[~turn];
        if (Squares::get_non_ranging_attacks_by(pawn, dst)[0] != enemy_king_sq)
            return false;

        // if opponent king can move away from the attack, then return false.
        const SquareEnum* sq_ptr = Squares::get_non_ranging_attacks_by(
            board[enemy_king_sq], enemy_king_sq);
        for (; *sq_ptr != Squares::SQ_NA; ++sq_ptr) {
            if (enemy_mask.is_one(*sq_ptr))
                continue;
            if (board.is_square_attacked(turn, *sq_ptr, enemy_king_sq))
                continue;
            return false;
        }

        // if opponent can capture the dropped pawn, then return false.
        const auto enemy_king_direction = (turn == BLACK) ? DIR_N : DIR_S;
        for (auto dir : Squares::direction_array) {
            if (dir == enemy_king_direction)
                continue;
            const auto src_next = board.find_attacker(~turn, dst, dir);
            const auto attacking_the_pawn = (src_next != Squares::SQ_NA);
            if (attacking_the_pawn) {
                const auto discovered_dir
                    = Squares::get_direction(src_next, enemy_king_sq);
                const auto discovered_attacker_sq = board.find_attacker(
                    turn, enemy_king_sq, discovered_dir, src_next);
                const auto pinned = (discovered_attacker_sq != Squares::SQ_NA);
                if (!pinned)
                    return false;
            }
        }
        return true;
    }
};

} // namespace vshogi

#endif // VSHOGI_GAME_HPP
