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

template <class State>
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
    /**
     * @brief Pairs of SFEN (without #ply) and the move.
     *
     */
    std::vector<std::pair<std::string, Move>> m_record;
    std::vector<Move> m_legal_moves;
    State m_current_state;
    ResultEnum m_result;
    BitBoard m_occupied[num_colors + 1];
    SquareEnum m_king_locations[num_colors];
    SquareEnum m_checker_locations[2];

public:
    Game() : m_record(), m_legal_moves(), m_current_state(), m_result(ONGOING)
    {
        m_record.reserve(128);
        update_internals();
    }
    Game(const std::string& sfen)
        : m_record(), m_legal_moves(), m_current_state(sfen), m_result(ONGOING)
    {
        m_record.reserve(128);
        update_internals();
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
        if (include_move_count)
            return m_record[n].first + ' ' + std::to_string(n + 1);
        return m_record[n].first;
    }
    Game& apply(const Move move)
    {
        m_record.emplace_back(std::make_pair(m_current_state.to_sfen(), move));
        m_current_state.apply(move);
        update_internals(move);
        return *this;
    }
    bool is_legal(const Move move) const
    {
        return (
            std::find(m_legal_moves.cbegin(), m_legal_moves.cend(), move)
            != m_legal_moves.cend());
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
    void update_internals()
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
        update_legal_moves();
        update_result();
    }
    void update_internals(const Move move)
    {
        if (!is_legal(move)) {
            const auto turn = get_turn();
            m_result = (turn == BLACK) ? BLACK_WIN : WHITE_WIN;
            m_legal_moves.clear();
            return;
        }

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

        update_legal_moves();
        update_result();
    }
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
        if (m_result != ONGOING)
            m_legal_moves.clear();
    }
    bool is_repetitions() const
    {
        constexpr int num_acceptable_repetitions = 3;
        int num = 1;
        const auto current_sfen = m_current_state.to_sfen();
        for (auto&& previous_record : m_record) {
            num += (current_sfen == previous_record.first);
            if (num > num_acceptable_repetitions)
                return true;
        }
        return false;
    }
    void update_legal_moves()
    {
        m_legal_moves.clear();
        append_legal_moves_by_king();
        if (m_checker_locations[0] == Squares::SQ_NA) {
            const auto turn = get_turn();
            for (auto sq : Squares::square_array) {
                if (!m_occupied[~turn].is_one(sq)) { // Empty or ally square
                    if (m_king_locations[turn] != sq)
                        append_legal_moves_by_non_king_at(sq);
                    if (!m_occupied[2].is_one(sq)) // empty
                        append_legal_moves_dropping_to(sq);
                }
            }
        } else if (m_checker_locations[1] == Squares::SQ_NA) {
            append_legal_moves_to_defend_king();
        }
    }
    void append_legal_moves_by_king()
    {
        m_legal_moves.clear();
        const auto turn = get_turn();
        const auto src = m_king_locations[turn];
        if (src == Squares::SQ_NA)
            return;
        const auto& board = get_board();
        const auto moving = board[src];
        const auto movable = BitBoard::get_attacks_by(moving, src);
        for (auto dir : Squares::direction_array) {
            const auto dst = Squares::shift(src, dir);
            if ((!movable.is_one(dst)) || m_occupied[turn].is_one(dst))
                continue;
            if (!is_square_attacked(dst, ~turn, src))
                m_legal_moves.emplace_back(dst, src, false);
        }
    }
    void append_legal_moves_by_non_king_at(const SquareEnum src)
    {
        const auto turn = get_turn();
        const auto& board = get_board();
        const auto king_sq = m_king_locations[turn];
        const auto moving = board[src];
        const auto promotable = Pieces::is_promotable(moving);
        const auto ranging = Pieces::is_ranging(moving);
        const auto src_dir = Squares::get_direction(src, king_sq);
        const auto hidden_attacker_sq
            = board.find_attacker(~turn, king_sq, src_dir, src);
        const auto attacks = BitBoard::get_attacks_by(moving, src);
        const auto src_in_promotion_zone
            = Squares::is_promotion_zone(src, turn);
        if (hidden_attacker_sq != Squares::SQ_NA) {
            SquareEnum dst = king_sq;
            while (true) {
                dst = Squares::shift(dst, src_dir);
                if (dst == src)
                    continue;
                if (attacks.is_one(dst)) {
                    if (!BitBoard::get_attacks_by(moving, dst).any())
                        m_legal_moves.emplace_back(dst, src, true);
                    else if (
                        promotable
                        && (src_in_promotion_zone
                            || Squares::is_promotion_zone(dst, turn))) {
                        m_legal_moves.emplace_back(dst, src, false);
                        m_legal_moves.emplace_back(dst, src, true);
                    } else {
                        m_legal_moves.emplace_back(dst, src, false);
                    }
                }
                if (dst == hidden_attacker_sq)
                    break;
            }
            return;
        }
        for (auto dir : Squares::direction_array) {
            SquareEnum dst = src;
            while (true) {
                dst = Squares::shift(dst, dir);
                if ((dst == Squares::SQ_NA) || m_occupied[turn].is_one(dst)
                    || (!attacks.is_one(dst)))
                    break;
                if (!BitBoard::get_attacks_by(moving, dst).any())
                    m_legal_moves.emplace_back(dst, src, true);
                else if (
                    promotable
                    && (src_in_promotion_zone
                        || Squares::is_promotion_zone(dst, turn))) {
                    m_legal_moves.emplace_back(dst, src, false);
                    m_legal_moves.emplace_back(dst, src, true);
                } else {
                    m_legal_moves.emplace_back(dst, src, false);
                }
                if ((!ranging) || m_occupied[~turn].is_one(dst))
                    break;
            }
        }
    }
    bool is_square_attacked(
        const SquareEnum sq,
        const ColorEnum attacker_color,
        const SquareEnum skip = Squares::SQ_NA) const
    {
        const auto& board = get_board();
        for (auto dir : Squares::direction_array) {
            const auto attacker_sq
                = board.find_attacker(attacker_color, sq, dir, skip);
            if (attacker_sq != Squares::SQ_NA)
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
            SquareEnum dst = king_location;
            while (true) {
                dst = Squares::shift(dst, dir);
                if ((dst == checker_location) || (dst == Squares::SQ_NA))
                    break;
                append_legal_moves_by_non_king_moving_to(dst);
                append_legal_moves_dropping_to(dst);
            }
        }
    }
    void append_legal_moves_by_non_king_moving_to(const SquareEnum dst)
    {
        const auto turn = get_turn();
        const auto& board = get_board();
        const auto king_location = m_king_locations[turn];
        const auto target_in_promotion_zone
            = Squares::is_promotion_zone(dst, turn);
        const auto empty_mask = ~m_occupied[2];
        const auto src_mask
            = m_occupied[turn] & (~BitBoard::from_square(king_location));
        for (auto dir : Squares::direction_array) {
            SquareEnum src = dst;
            while (true) {
                src = Squares::shift(src, dir);
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

                if (Pieces::is_promotable(p)) {
                    if (BitBoard::get_attacks_by(p, dst).any())
                        m_legal_moves.emplace_back(dst, src, false);
                    if (target_in_promotion_zone
                        || Squares::is_promotion_zone(src, turn))
                        m_legal_moves.emplace_back(dst, src, true);
                } else {
                    m_legal_moves.emplace_back(dst, src, false);
                }
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
        for (auto sq : Squares::squares_in_file[f]) {
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
            if (is_square_attacked(sq, turn, enemy_king_sq))
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
