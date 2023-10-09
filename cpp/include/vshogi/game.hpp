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
        const auto turn = get_turn();
        m_current_state.get_legal_moves(m_legal_moves);
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

        BitBoard occupied[3] = {BitBoard(), BitBoard(), BitBoard()};
        const auto& board = get_board();
        const auto king_sq = find_turn_king_and_get_occupied_masks(occupied);
        const auto dst = move.destination();
        const auto moved = board[dst];
        const bool check_by_moved
            = BitBoard::get_attacks_by(moved, dst, occupied[2]).is_one(king_sq);
        const auto discovered_checker_location
            = find_discovered_checker_location(king_sq, move, occupied);
        const bool check_by_discovered
            = (discovered_checker_location != Squares::SQ_NA);

        if (check_by_moved && check_by_discovered) {
            get_legal_moves_by_king_at(king_sq, occupied);
        } else if (check_by_moved || check_by_discovered) {
            get_legal_moves_by_king_at(king_sq, occupied);
            append_legal_moves_to_defend_king_from_check_by(
                king_sq,
                check_by_moved ? dst : discovered_checker_location,
                occupied);
        } else {
            m_current_state.get_legal_moves(m_legal_moves);
        }
        update_result();
    }
    void update_result()
    {
        const auto turn = get_turn();
        if (m_legal_moves.empty())
            m_result = (turn == BLACK) ? WHITE_WIN : BLACK_WIN;
        if (is_fourfold_repetitions()) {
            if (get_board().in_check(turn))
                m_result = (turn == BLACK) ? BLACK_WIN : WHITE_WIN;
            else
                m_result = DRAW;
        }
        if (m_result != ONGOING)
            m_legal_moves.clear();
    }
    bool is_fourfold_repetitions() const
    {
        const int num_acceptable_repetitions = 3;
        int num = 1;
        const auto current_sfen = m_current_state.to_sfen();
        for (auto&& previous_record : m_record) {
            num += (current_sfen == previous_record.first);
            if (num > num_acceptable_repetitions)
                return true;
        }
        return false;
    }
    SquareEnum find_turn_king_and_get_occupied_masks(BitBoard occupied[3]) const
    {
        const auto turn = get_turn();
        auto king = Pieces::to_board_piece(turn, Pieces::OU);
        const auto& board = get_board();
        SquareEnum out = Squares::SQ_NA;
        for (auto sq : Squares::square_array) {
            const auto p = board[sq];
            if (p == Pieces::VOID)
                continue;
            if (p == king)
                out = sq;
            const auto c = Pieces::get_color(p);
            occupied[c] |= BitBoard::from_square(sq);
        }
        occupied[2] = occupied[BLACK] | occupied[WHITE];
        return out;
    }
    SquareEnum find_discovered_checker_location(
        const SquareEnum king_location,
        const Move move,
        const BitBoard occupied[3]) const
    {
        if (move.is_drop())
            return Squares::SQ_NA;
        const auto dir
            = Squares::get_direction(move.source_square(), king_location);
        if ((dir != DIR_NA)
            && (Squares::get_direction(move.destination(), king_location)
                == dir))
            return Squares::SQ_NA;
        const auto& board = get_board();
        const auto turn = get_turn();
        auto sq = king_location;
        while (true) {
            sq = Squares::shift(sq, dir);
            if ((sq == Squares::SQ_NA) || occupied[turn].is_one(sq))
                return Squares::SQ_NA;
            if (occupied[~turn].is_one(sq))
                return BitBoard::get_attacks_by(board[sq], sq, occupied[2])
                               .is_one(king_location)
                           ? sq
                           : Squares::SQ_NA;
        }
    }
    void get_legal_moves_by_king_at(
        const SquareEnum king_location, const BitBoard occupied[3])
    {
        m_legal_moves.clear();
        const auto turn = get_turn();
        const auto& board = get_board();
        const auto moving = board[king_location];
        const auto deletion_mask = (~BitBoard::from_square(king_location));
        const auto movable = BitBoard::get_attacks_by(moving, king_location);
        for (auto dir : Squares::direction_array) {
            const auto dst = Squares::shift(king_location, dir);
            if ((!movable.is_one(dst)) || occupied[turn].is_one(dst))
                continue;
            BitBoard occupied_after_move[3]
                = {occupied[0], occupied[1], occupied[2]};
            Board board_after_move = board;
            const auto addition_mask = BitBoard::from_square(dst);
            occupied_after_move[turn] |= addition_mask;
            occupied_after_move[turn] &= deletion_mask;
            occupied_after_move[2] |= addition_mask;
            occupied_after_move[2] &= deletion_mask;
            board_after_move[king_location] = Pieces::VOID;
            board_after_move[dst] = moving;
            if (!is_square_attacked(
                    board_after_move, dst, ~turn, occupied_after_move))
                m_legal_moves.emplace_back(dst, king_location, false);
        }
    }
    void append_legal_moves_to_defend_king_from_check_by(
        const SquareEnum king_location,
        const SquareEnum checker_location,
        const BitBoard occupied[3])
    {
        append_legal_moves_by_non_king_moving_to(
            checker_location, king_location, occupied);
        if (!is_neighbor(king_location, checker_location)) {
            const auto dir
                = Squares::get_direction(checker_location, king_location);
            SquareEnum dst = king_location;
            while (true) {
                dst = Squares::shift(dst, dir);
                if ((dst == checker_location) || (dst == Squares::SQ_NA))
                    break;
                append_legal_moves_by_non_king_moving_to(
                    dst, king_location, occupied);
                append_legal_moves_dropping_to(dst);
            }
        }
    }
    void append_legal_moves_by_non_king_moving_to(
        const SquareEnum dst,
        const SquareEnum king_location,
        const BitBoard occupied[3])
    {
        const auto turn = get_turn();
        const auto& board = get_board();
        const auto addition_mask = BitBoard::from_square(dst);
        const auto target_in_promotion_zone
            = Squares::is_promotion_zone(dst, turn);
        for (auto src : Squares::square_array) {
            if ((src == king_location) || (!occupied[turn].is_one(src))
                || (!BitBoard::get_attacks_by(board[src], src, occupied[2])
                         .is_one(dst)))
                continue;
            BitBoard occupied_after_move[3]
                = {occupied[0], occupied[1], occupied[2]};
            const auto deletion_mask = (~BitBoard::from_square(src));
            Board board_after_move = board;
            occupied_after_move[turn] |= addition_mask;
            occupied_after_move[turn] &= deletion_mask;
            occupied_after_move[2] |= addition_mask;
            occupied_after_move[2] &= deletion_mask;
            board_after_move[src] = Pieces::VOID;
            board_after_move[dst] = board[src];
            if (is_square_attacked(
                    board_after_move,
                    king_location,
                    ~turn,
                    occupied_after_move))
                continue;

            if (!Pieces::is_promotable(board[src])) {
                if (BitBoard::get_attacks_by(board[src], dst).any())
                    m_legal_moves.emplace_back(dst, src, false);
                if (target_in_promotion_zone
                    || Squares::is_promotion_zone(src, turn))
                    m_legal_moves.emplace_back(dst, src, false);
            } else {
                m_legal_moves.emplace_back(dst, src, false);
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
            if (BitBoard::get_attacks_by(Pieces::to_board_piece(turn, pt), dst)
                    .any())
                m_legal_moves.emplace_back(Move(dst, pt));
        }
    }
    static bool is_square_attacked(
        const Board& board,
        const SquareEnum sq,
        const ColorEnum attacker_color,
        const BitBoard occupied[3])
    {
        for (auto dir : Squares::direction_array) {
            SquareEnum src = sq;
            while (true) {
                src = Squares::shift(src, dir);
                if ((src == Squares::SQ_NA)
                    || occupied[~attacker_color].is_one(src))
                    break;
                if (occupied[attacker_color].is_one(src)) {
                    if (BitBoard::get_attacks_by(board[src], src).is_one(sq))
                        return true;
                }
            }
        }
        return false;
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
