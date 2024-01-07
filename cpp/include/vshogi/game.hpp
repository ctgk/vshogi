#ifndef VSHOGI_GAME_HPP
#define VSHOGI_GAME_HPP

#include <algorithm>
#include <string>
#include <vector>

#include "vshogi/color.hpp"
#include "vshogi/direction.hpp"
#include "vshogi/result.hpp"

namespace vshogi
{

namespace internal
{

template <class State>
int num_pieces(const State& s, const ColorEnum& c)
{
    using Squares = typename State::SquaresType;
    using Pieces = typename State::PiecesType;
    const auto& board = s.get_board();
    const auto& stand = s.get_stand(c);
    int out = 0;
    for (auto& sq : Squares::square_array) {
        const auto p = board[sq];
        if (p == Pieces::VOID)
            continue;
        if (Pieces::get_color(p) == c)
            out += 1;
    }
    for (auto& pt : Pieces::stand_piece_array) {
        out += stand.count(pt);
    }
    return out;
}

template <class State>
int total_point(const State& s, const ColorEnum& c)
{
    using Squares = typename State::SquaresType;
    using Pieces = typename State::PiecesType;
    int out = 0;
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

template <class State, int MaxAcceptableRepetition = 3>
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

    /**
     * @brief Pairs of zobrist hash and the move.
     */
    std::vector<std::pair<std::uint64_t, Move>> m_record;
    std::vector<Move> m_legal_moves;
    ResultEnum m_result;
    std::uint64_t m_zobrist_hash;
    const std::string m_initial_sfen_without_ply;
    const int m_half_num_pieces[2];
    const int m_initial_points[2];
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
    static constexpr int ranks()
    {
        return Squares::num_ranks;
    }
    static constexpr int files()
    {
        return Squares::num_files;
    }
    static constexpr int board_piece_types()
    {
        return sizeof(Pieces::piece_array) / sizeof(Pieces::piece_array[0]);
    }
    static constexpr int stand_piece_types()
    {
        return sizeof(Pieces::stand_piece_array)
               / sizeof(Pieces::stand_piece_array[0]);
    }
    static constexpr int feature_channels()
    {
        // 2-player * (board-piece-types + stand-piece-types)
        return 2 * (board_piece_types() + stand_piece_types());
    }
    static constexpr int num_dlshogi_policy()
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
    const std::vector<Move>& get_legal_moves() const
    {
        return m_legal_moves;
    }
    ResultEnum get_result() const
    {
        return m_result;
    }
    std::string to_sfen(const bool include_move_count = true) const
    {
        if (include_move_count)
            return m_current_state.to_sfen() + " "
                   + std::to_string(m_record.size() + 1);
        else
            return m_current_state.to_sfen();
    }
    std::size_t record_length() const
    {
        return m_record.size();
    }
    Move get_move_at(const std::size_t n) const
    {
        return m_record[n].second;
    }
    std::string
    get_sfen_at(const std::size_t n, const bool include_move_count = true) const
    {
        auto s = State(m_initial_sfen_without_ply);
        for (std::size_t ii = 0; ii < n; ++ii) {
            s.apply(m_record[ii].second);
        }
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
    Game& apply(const Move move)
    {
        m_record.emplace_back(std::make_pair(m_zobrist_hash, move));
        m_current_state.apply(move, &m_zobrist_hash);
        update_internals(move);
        return *this;
    }
    bool is_legal(const Move move) const
    {
        return (
            std::find(m_legal_moves.cbegin(), m_legal_moves.cend(), move)
            != m_legal_moves.cend());
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

    template <bool CheckLegality = true>
    bool is_check_move(const Move& move) const
    {
        if (CheckLegality && (!is_legal(move)))
            return false;
        const auto turn = get_turn();
        const auto dst = move.destination();
        const auto board_after_move
            = State(m_current_state).apply(move).get_board();
        const auto piece = board_after_move[dst];
        const auto enemy_king_sq = m_king_locations[~get_turn()];

        BitBoard occupied_after_move
            = m_occupied[2] | BitBoard::from_square(dst);
        auto discovered_checker_location = Squares::SQ_NA;
        if (!move.is_drop()) {
            const auto src = move.source_square();
            const auto dir = Squares::get_direction(src, enemy_king_sq);
            if ((dir != DIR_NA)
                && (dir != Squares::get_direction(dst, enemy_king_sq))
                && (board_after_move.find_attacker(turn, enemy_king_sq, dir)
                    != Squares::SQ_NA))
                return true; // discovered check
            occupied_after_move &= (~BitBoard::from_square(src));
        }
        return BitBoard::get_attacks_by(piece, dst, occupied_after_move)
            .is_one(enemy_king_sq);
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
        : m_current_state(s), m_record(), m_legal_moves(), m_result(ONGOING),
          m_zobrist_hash(m_current_state.zobrist_hash()),
          m_initial_sfen_without_ply(m_current_state.to_sfen()),
          m_half_num_pieces{
              internal::num_pieces(m_current_state, BLACK) / 2,
              internal::num_pieces(m_current_state, WHITE) / 2},
          m_initial_points{
              internal::total_point(m_current_state, BLACK),
              internal::total_point(m_current_state, WHITE)}
    {
        m_record.reserve(128);
        update_internals();
    }
    void update_internals()
    {
        update_king_occupied_checkers();
        update_legal_moves();
        update_result();
    }
    void update_internals(const Move& move)
    {
        if (!is_legal(move)) {
            m_result = (get_turn() == BLACK) ? BLACK_WIN : WHITE_WIN;
            m_legal_moves.clear();
            return;
        }
        update_king_occupied_checkers(move);
        update_legal_moves();
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
        int num = 1;
        auto s = State(m_initial_sfen_without_ply);
        const auto n = m_record.size();
        for (std::size_t ii = 0; ii < n; ++ii) {
            if (((n - ii) % 2 == 0) && (m_zobrist_hash == m_record[ii].first)) {
                num += (m_current_state == s);
                if (num > MaxAcceptableRepetition)
                    return true;
            }
            s.apply(m_record[ii].second);
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
        const int num_pieces_in_zone = piece_mask.hamming_weight();

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
    int count_point_in(const ColorEnum& c, const BitBoard& piece_mask) const
    {
        int out = 0;
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

protected:
    void update_legal_moves()
    {
        m_legal_moves.clear();
        m_legal_moves.reserve(128);
        append_legal_moves_by_king();
        if (m_checker_locations[0] == Squares::SQ_NA) {
            const auto turn = get_turn();
            const auto ally_mask = m_occupied[turn];
            const auto empty_mask = ~m_occupied[2];
            const auto& king_sq = m_king_locations[turn];
            for (auto sq : Squares::square_array) {
                if (empty_mask.is_one(sq))
                    append_legal_moves_dropping_to(sq);
                else if (ally_mask.is_one(sq) && (king_sq != sq))
                    append_legal_moves_by_non_king_at(sq);
            }
        } else if (m_checker_locations[1] == Squares::SQ_NA) {
            append_legal_moves_to_defend_king();
        }
    }
    void append_legal_moves_by_king()
    {
        const auto ac = get_turn(); //!< ally color
        const auto ec = ~ac; //!< enemy color
        const auto src = m_king_locations[ac];
        if (src == Squares::SQ_NA)
            return;
        const auto& board = get_board();
        const auto moving = board[src];
        auto ptr_dst = BitBoard::get_attacks_by_non_ranging(moving, src);
        const auto end = ptr_dst + 8;
        const auto& ally_mask = m_occupied[ac];
        for (; *ptr_dst != Squares::SQ_NA; ++ptr_dst) {
            if (ptr_dst >= end)
                break;
            if (ally_mask.is_one(*ptr_dst))
                continue;
            if (is_square_attacked(board, *ptr_dst, ec, src))
                continue;
            m_legal_moves.emplace_back(*ptr_dst, src, false);
        }
    }
    void append_legal_moves_by_non_king_at(const SquareEnum src)
    {
        const auto turn = get_turn();
        const auto& board = get_board();
        const auto king_sq = m_king_locations[turn];
        const auto moving = board[src];
        const auto promotable = Pieces::is_promotable(moving);
        const auto src_dir = Squares::get_direction(src, king_sq);
        const auto hidden_attacker_sq
            = board.find_attacker(~turn, king_sq, src_dir, src);
        const auto attacks = BitBoard::get_attacks_by(moving, src);
        const auto promotable_src = Squares::in_promotion_zone(src, turn);
        if (hidden_attacker_sq != Squares::SQ_NA) {
            auto ptr_dst = BitBoard::get_squares_along(src_dir, king_sq);
            for (;; ++ptr_dst) {
                const auto dst = *ptr_dst;
                if (attacks.is_one(dst)) {
                    const bool promote
                        = promotable
                          && (promotable_src
                              || Squares::in_promotion_zone(dst, turn));
                    append_legal_move_or_moves(moving, dst, src, promote);
                }
                if (dst == hidden_attacker_sq)
                    break;
            }
            return;
        }
        append_legal_moves_by_non_king_ignoring_discovered_check(
            moving,
            attacks & (~m_occupied[turn]),
            src,
            promotable,
            promotable_src,
            turn);
    }
    void append_legal_moves_by_non_king_ignoring_discovered_check(
        const typename Pieces::BoardPieceTypeEnum& p,
        const BitBoard& dst_mask,
        const SquareEnum& src,
        const bool& promotable,
        const bool& src_promote,
        const ColorEnum& turn)
    {
        auto ptr_dst = BitBoard::get_attacks_by_non_ranging(p, src);
        if (ptr_dst != nullptr) {
            for (; *ptr_dst != Squares::SQ_NA; ++ptr_dst) {
                if (!dst_mask.is_one(*ptr_dst))
                    continue;
                const bool promote
                    = promotable
                      && (src_promote
                          || Squares::in_promotion_zone(*ptr_dst, turn));
                append_legal_move_or_moves(p, *ptr_dst, src, promote);
            }
            return;
        }

        const auto& enemy_mask = m_occupied[~turn];
        for (auto& dir : Squares::direction_array) {
            ptr_dst = BitBoard::get_squares_along(dir, src);
            for (; *ptr_dst != Squares::SQ_NA; ++ptr_dst) {
                if (!dst_mask.is_one(*ptr_dst))
                    break;
                bool promote
                    = promotable
                      && (src_promote
                          || Squares::in_promotion_zone(*ptr_dst, turn));
                append_legal_move_or_moves(p, *ptr_dst, src, promote);
                if (enemy_mask.is_one(*ptr_dst))
                    break;
            }
        }
    }
    void append_legal_move_or_moves(
        const typename Pieces::BoardPieceTypeEnum& p,
        const SquareEnum& dst,
        const SquareEnum& src,
        const bool& promotable)
    {
        if (!BitBoard::get_attacks_by(p, dst).any())
            m_legal_moves.emplace_back(dst, src, true);
        else if (promotable) {
            m_legal_moves.emplace_back(dst, src, false);
            m_legal_moves.emplace_back(dst, src, true);
        } else
            m_legal_moves.emplace_back(dst, src, false);
    }
    static bool is_square_attacked(
        const Board& board,
        const SquareEnum sq,
        const ColorEnum enemy,
        const SquareEnum skip = Squares::SQ_NA)
    {
        for (auto dir : Squares::direction_array) {
            if (board.find_attacker(enemy, sq, dir, skip) != Squares::SQ_NA)
                return true;
        }
        return false;
    }
    void append_legal_moves_to_defend_king()
    {
        const auto turn = get_turn();
        const auto& board = get_board();
        const auto checker_location = m_checker_locations[0];
        const auto king_location = m_king_locations[turn];
        append_legal_moves_by_non_king_moving_to(checker_location);
        if (!is_neighbor(king_location, checker_location)) {
            const auto dir
                = Squares::get_direction(checker_location, king_location);
            auto ptr_dst = BitBoard::get_squares_along(dir, king_location);
            for (; *ptr_dst != Squares::SQ_NA; ++ptr_dst) {
                if (*ptr_dst == checker_location)
                    break;
                append_legal_moves_by_non_king_moving_to(*ptr_dst);
                append_legal_moves_dropping_to(*ptr_dst);
            }
        }
    }
    void append_legal_moves_by_non_king_moving_to(const SquareEnum dst)
    {
        const auto turn = get_turn();
        const auto& board = get_board();
        const auto king_location = m_king_locations[turn];
        const auto target_in_promotion_zone
            = Squares::in_promotion_zone(dst, turn);
        const auto empty_mask = ~m_occupied[2];
        const auto src_mask
            = m_occupied[turn] & (~BitBoard::from_square(king_location));
        for (auto dir : Squares::direction_array) {
            auto ptr_src = BitBoard::get_squares_along(dir, dst);
            for (; *ptr_src != Squares::SQ_NA; ++ptr_src) {
                const auto src = *ptr_src;
                if (empty_mask.is_one(src))
                    continue;
                if (!src_mask.is_one(src))
                    break;
                const auto p = board[src];
                if (!BitBoard::get_attacks_by(p, src).is_one(dst))
                    break;
                const auto src_dir = Squares::get_direction(src, king_location);
                if ((src_dir != DIR_NA)
                    && (src_dir != Squares::get_direction(dst, king_location))
                    && (board.find_attacker(~turn, king_location, src_dir, src)
                        != Squares::SQ_NA))
                    break;

                const auto promote = Pieces::is_promotable(p)
                                     && (Squares::in_promotion_zone(src, turn)
                                         || target_in_promotion_zone);
                append_legal_move_or_moves(p, dst, src, promote);
                break;
            }
        }
    }
    void append_legal_moves_dropping_to(const SquareEnum dst)
    {
        const auto turn = get_turn();
        const auto& stand = get_stand(turn);
        for (auto pt : Pieces::stand_piece_array) {
            if (!stand.exist(pt))
                continue;
            if ((pt == Pieces::FU)
                && (has_pawn_in_file(Squares::to_file(dst))
                    || is_drop_pawn_mate(dst)))
                continue;
            if (BitBoard::get_attacks_by(Pieces::to_board_piece(turn, pt), dst)
                    .any())
                m_legal_moves.emplace_back(Move(dst, pt));
        }
    }
    bool has_pawn_in_file(const typename Squares::FileEnum f) const
    {
        const auto turn = get_turn();
        const auto& board = get_board();
        const auto pawn = Pieces::to_board_piece(turn, Pieces::FU);
        for (auto sq : Squares::file_to_square_array[f]) {
            if (pawn == board[sq])
                return true;
        }
        return false;
    }
    bool is_drop_pawn_mate(const SquareEnum dst) const
    {
        const auto turn = get_turn();
        const auto& board = get_board();
        const auto pawn = Pieces::to_board_piece(turn, Pieces::FU);
        const auto enemy_king_sq = m_king_locations[~turn];
        if (!BitBoard::get_attacks_by(pawn, dst).is_one(enemy_king_sq))
            return false;

        // if opponent king can move away from the attack, then return false.
        const auto enemy_king_movable = BitBoard::get_attacks_by(
            Pieces::to_board_piece(~turn, Pieces::OU), enemy_king_sq);
        for (auto dir : Squares::direction_array) {
            const auto sq = Squares::shift(enemy_king_sq, dir);
            if (!enemy_king_movable.is_one(sq))
                continue;
            if (is_square_attacked(board, sq, turn, enemy_king_sq))
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
    static bool is_neighbor(const SquareEnum a, const SquareEnum b)
    {
        for (auto dir : Squares::direction_array) {
            if (b == Squares::shift(a, dir))
                return true;
        }
        return false;
    }
};

} // namespace vshogi

#endif // VSHOGI_GAME_HPP
