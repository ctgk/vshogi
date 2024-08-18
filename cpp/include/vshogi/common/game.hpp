#ifndef VSHOGI_GAME_HPP
#define VSHOGI_GAME_HPP

#include <algorithm>
#include <string>
#include <vector>

#include "vshogi/common/bitboard.hpp"
#include "vshogi/common/board.hpp"
#include "vshogi/common/color.hpp"
#include "vshogi/common/direction.hpp"
#include "vshogi/common/move.hpp"
#include "vshogi/common/pieces.hpp"
#include "vshogi/common/result.hpp"
#include "vshogi/common/squares.hpp"
#include "vshogi/common/stand.hpp"
#include "vshogi/common/state.hpp"

namespace vshogi
{

namespace internal
{

template <class Config>
uint num_pieces(const State<Config>& s, const ColorEnum& c)
{
    using SHelper = Squares<Config>;
    using PHelper = Pieces<Config>;
    using Square = typename Config::Square;
    using PieceType = typename Config::PieceType;
    constexpr auto num_sp_types = Config::num_stand_piece_types;
    const auto& board = s.get_board();
    const auto& stand = s.get_stand(c);
    uint out = 0u;
    for (auto sq : EnumIterator<Square, Config::num_squares>()) {
        const auto p = board[sq];
        if (p == PHelper::VOID)
            continue;
        if (PHelper::get_color(p) == c)
            out += 1U;
    }

    for (auto pt : EnumIterator<PieceType, num_sp_types>())
        out += stand.count(pt);
    return out;
}

template <class Config>
uint total_point(const State<Config>& s, const ColorEnum& c)
{
    using SHelper = Squares<Config>;
    using PHelper = Pieces<Config>;
    using Square = typename Config::Square;
    using PieceType = typename Config::PieceType;
    constexpr auto num_sp_types = Config::num_stand_piece_types;
    uint out = 0u;
    const auto& board = s.get_board();
    const auto& stand = s.get_stand(c);
    for (auto sq : EnumIterator<Square, Config::num_squares>()) {
        const auto p = board[sq];
        if (PHelper::get_color(p) == c)
            out += PHelper::get_point(p);
    }
    for (auto pt : EnumIterator<PieceType, num_sp_types>()) {
        out += stand.count(pt) * PHelper::get_point(pt);
    }
    return out;
}

} // namespace internal

template <class Config>
class Game
{
private:
    using Square = typename Config::Square;
    using File = typename Config::File;
    using PieceType = typename Config::PieceType;
    using ColoredPiece = typename Config::ColoredPiece;
    using PHelper = Pieces<Config>;
    using SHelper = Squares<Config>;
    using BitBoardType = BitBoard<Config>;
    using BoardType = Board<Config>;
    using MoveType = Move<Config>;
    using StandType = Stand<Config>;
    using StateType = State<Config>;

    static constexpr uint num_piece_types = Config::num_piece_types;
    static constexpr uint num_stand_piece_types = Config::num_stand_piece_types;
    static constexpr uint num_dir = Config::num_dir;
    static constexpr uint num_ranks = Config::num_ranks;
    static constexpr uint num_files = Config::num_files;
    static constexpr uint num_squares = Config::num_squares;
    static constexpr uint max_acceptable_repetitions
        = Config::max_acceptable_repetitions;

private:
    StateType m_current_state;

    std::vector<std::uint64_t> m_zobrist_hash_list;
    std::vector<MoveType> m_move_list;
    std::vector<MoveType> m_legal_moves;
    ResultEnum m_result;
    std::uint64_t m_zobrist_hash;
    const std::string m_initial_sfen_without_ply;
    const uint m_half_num_pieces[2];
    const uint m_initial_points[2];

public:
    Game() : Game(StateType())
    {
    }
    Game(const std::string& sfen) : Game(StateType(sfen))
    {
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
        // 2-player * (piece-types + stand-piece-types)
        return 2 * (num_piece_types + num_stand_piece_types);
    }
    static constexpr uint num_dlshogi_policy()
    {
        return ranks() * files() * MoveType::num_policy_per_square();
    }
    ColorEnum get_turn() const
    {
        return m_current_state.get_turn();
    }
    const BoardType& get_board() const
    {
        return m_current_state.get_board();
    }
    const StandType& get_stand(const ColorEnum c) const
    {
        return m_current_state.get_stand(c);
    }
    const std::vector<MoveType>& get_legal_moves() const
    {
        return m_legal_moves;
    }
    Square get_checker_location(const uint index = 0u) const
    {
        return m_current_state.get_checker_location(index);
    }
    ResultEnum get_result() const
    {
        return m_result;
    }
    std::uint64_t get_zobrist_hash() const
    {
        return m_zobrist_hash;
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
    MoveType get_move_at(const std::size_t n) const
    {
        return m_move_list[n];
    }
    std::string
    get_sfen_at(const std::size_t n, const bool include_move_count = true) const
    {
        auto s = StateType(m_initial_sfen_without_ply);
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
    Game& resign()
    {
        m_result = (get_turn() == BLACK) ? WHITE_WIN : BLACK_WIN;
        return *this;
    }
    Game& apply(const MoveType& move)
    {
        if ((m_result == ONGOING) && (!is_legal(move))) {
            add_record_and_update_state(move);
            m_result = (get_turn() == BLACK) ? BLACK_WIN : WHITE_WIN;
            return *this;
        }
        return apply_nocheck(move);
    }
    Game& apply_nocheck(const MoveType& move)
    {
        add_record_and_update_state(move);
        update_internals();
        return *this;
    }
    Game& apply_mcts_internal_vertex(const MoveType& move)
    {
        add_record_and_update_state(move);
        update_internals_mcts_internal_vertex();
        return *this;
    }
    Game& apply_dfpn_offence(const MoveType& move)
    {
        add_record_and_update_state_for_dfpn(move);
        update_internals_dfpn_offence();
        return *this;
    }
    Game& apply_dfpn_defence(const MoveType& move)
    {
        add_record_and_update_state_for_dfpn(move);
        update_internals_dfpn_defence();
        return *this;
    }
    Game copy_and_apply_dfpn_offence(const MoveType& move)
    {
        auto out = Game(
            m_current_state,
            m_zobrist_hash_list,
            m_move_list,
            m_result,
            m_zobrist_hash,
            m_initial_sfen_without_ply,
            m_half_num_pieces[BLACK],
            m_half_num_pieces[WHITE],
            m_initial_points[BLACK],
            m_initial_points[WHITE]);
        out.apply_dfpn_offence(move);
        return out;
    }
    Game copy_and_apply_dfpn_defence(const MoveType& move)
    {
        auto out = Game(
            m_current_state,
            m_zobrist_hash_list,
            m_move_list,
            m_result,
            m_zobrist_hash,
            m_initial_sfen_without_ply,
            m_half_num_pieces[BLACK],
            m_half_num_pieces[WHITE],
            m_initial_points[BLACK],
            m_initial_points[WHITE]);
        out.apply_dfpn_defence(move);
        return out;
    }
    bool is_legal(const MoveType move) const
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
        return m_current_state.in_check();
    }

    template <bool CheckLegality = true>
    bool is_check_move(const MoveType& move) const
    {
        if (CheckLegality && (!is_legal(move)))
            return false;
        const auto turn = get_turn();
        const auto dst = move.destination();
        const auto board_after_move
            = StateType(m_current_state).apply(move).get_board();
        const auto piece = board_after_move[dst];
        const auto enemy_king_sq = get_board().get_king_location(~turn);

        BitBoard occupied_after_move
            = get_board().get_occupied() | BitBoardType::from_square(dst);
        if (!move.is_drop()) {
            const auto src = move.source_square();
            const auto dir = SHelper::get_direction(src, enemy_king_sq);
            if ((dir != DIR_NA)
                && (dir != SHelper::get_direction(dst, enemy_king_sq))
                && (board_after_move.find_attacker(turn, enemy_king_sq, dir)
                    != SHelper::SQ_NA))
                return true; // discovered check
            occupied_after_move &= (~BitBoardType::from_square(src));
        }
        return BitBoardType::get_attacks_by(piece, dst, occupied_after_move)
            .is_one(enemy_king_sq);
    }
    void clear_records_for_dfpn()
    {
        m_zobrist_hash_list.clear();
        m_move_list.clear();
        m_zobrist_hash_list.emplace_back(
            m_current_state.get_board().zobrist_hash());
    }
    void to_feature_map(float* const data) const
    {
        m_current_state.to_feature_map(data);
    }

protected:
    Game(const StateType& s)
        : m_current_state(s), m_zobrist_hash_list(), m_move_list(),
          m_legal_moves(), m_result(ONGOING),
          m_zobrist_hash(m_current_state.zobrist_hash()),
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
    Game(
        const StateType& s,
        const std::vector<uint64_t>& zobrist_hash_list,
        const std::vector<MoveType>& move_list,
        const ResultEnum& result,
        const uint64_t& zobrist_hash,
        const std::string& initial_sfen_without_ply,
        const uint& half_num_pieces_black,
        const uint& half_num_pieces_white,
        const uint& initial_points_black,
        const uint& initial_points_white)
        : m_current_state(s), m_zobrist_hash_list(zobrist_hash_list),
          m_move_list(move_list), m_legal_moves(), m_result(result),
          m_zobrist_hash(zobrist_hash),
          m_initial_sfen_without_ply(initial_sfen_without_ply),
          m_half_num_pieces{half_num_pieces_black, half_num_pieces_white},
          m_initial_points{initial_points_black, initial_points_white}
    {
    }
    void add_record_and_update_state(const MoveType& move)
    {
        m_zobrist_hash_list.emplace_back(m_zobrist_hash);
        m_move_list.emplace_back(move);
        m_current_state.apply(move, &m_zobrist_hash);
    }
    void add_record_and_update_state_for_dfpn(const MoveType& move)
    {
        m_move_list.emplace_back(move);
        m_current_state.apply(move, &m_zobrist_hash);
        m_zobrist_hash_list.emplace_back(
            m_current_state.get_board().zobrist_hash());
    }
    void update_internals()
    {
        update_legal_moves(false);
        update_result();
    }
    void update_internals_mcts_internal_vertex()
    {
        m_legal_moves.clear();
        m_result = UNKNOWN;
    }
    void update_internals_dfpn_offence()
    {
        update_legal_moves(false);
        update_result_for_dfpn();
    }
    void update_internals_dfpn_defence()
    {
        update_legal_moves(true);
        update_result_for_dfpn();
    }

protected:
    void update_result()
    {
        m_result = ONGOING;
        const auto turn = get_turn();
        if (m_legal_moves.empty())
            m_result = (turn == BLACK) ? WHITE_WIN : BLACK_WIN;
        if (is_repetitions()) {
            if (m_current_state.in_check())
                m_result = (turn == BLACK) ? BLACK_WIN : WHITE_WIN;
            else
                m_result = DRAW;
        }
        if (can_declare_win_by_king_enter())
            m_result = (turn == BLACK) ? BLACK_WIN : WHITE_WIN;
        if (m_result != ONGOING)
            m_legal_moves.clear();
    }
    void update_result_for_dfpn()
    {
        m_result = ONGOING;
        const auto turn = get_turn();
        if (m_legal_moves.empty())
            m_result = (turn == BLACK) ? WHITE_WIN : BLACK_WIN;
        if (is_duplicate_at_least_once())
            m_result = DRAW;
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
            if (num > max_acceptable_repetitions)
                return true;
        }
        return false;
    }
    bool is_duplicate_at_least_once() const
    {
        const int n = static_cast<int>(m_move_list.size());
        for (int ii = n - 4; ii >= 0; ii -= 2) {
            const uint index = static_cast<uint>(ii);
            if (m_zobrist_hash_list[static_cast<uint>(n)]
                == m_zobrist_hash_list[index])
                return true;
        }
        return false;
    }
    bool can_declare_win_by_king_enter() const
    {
        // http://www2.computer-shogi.org/wcsc17/rule_e.html
        // (4) There is no check on the King of the declaring side.
        if (m_current_state.in_check())
            return false;

        const auto turn = get_turn();
        const BoardType& board = get_board();
        // (1) The King of the declaring side is in the third rank or beyond.
        if (!SHelper::in_promotion_zone(board.get_king_location(turn), turn))
            return false;

        const auto promo_zone_mask = BitBoardType::get_promotion_zone(turn);
        const auto piece_mask = (promo_zone_mask & board.get_occupied(turn));
        const uint num_pieces_in_zone = piece_mask.hamming_weight();

        // (3) The declaring side has 10 or more pieces other than the King in
        // the third rank or beyond.
        if (num_pieces_in_zone <= m_half_num_pieces[turn])
            return false;

        // (2) The declaring side has 28 (the first player (sente, black)) or
        // 27 (the second player (gote, white)) piece points or more.
        if (turn == BLACK)
            return count_point_of(turn, piece_mask) > m_initial_points[turn];
        else
            return count_point_of(turn, piece_mask) >= m_initial_points[turn];
    }
    uint count_point_of(const ColorEnum& c, const BitBoardType& mask) const
    {
        uint out = 0;
        const BoardType& board = get_board();
        for (auto sq : EnumIterator<Square, num_squares>()) {
            if (mask.is_one(sq))
                out += PHelper::get_point(board[sq]);
        }
        const auto& stand = get_stand(c);
        for (auto pt : EnumIterator<PieceType, num_stand_piece_types>()) {
            out += stand.count(pt) * PHelper::get_point(pt);
        }
        return out;
    }

protected:
    void update_legal_moves(const bool& restrict_legal_to_check)
    {
        m_legal_moves.clear();
        m_legal_moves.reserve(128);
        if (restrict_legal_to_check) {
            append_check_moves_by_king(); // discovered check
            if (m_current_state.in_double_check()) {
            } else if (m_current_state.in_check()) {
                append_legal_moves_to_defend_king(true);
            } else {
                // no check to turn player's king
                append_check_drop_moves();
                const auto turn = get_turn();
                const BoardType& board = get_board();
                const auto& ally_mask = board.get_occupied(turn);
                const auto king_sq = board.get_king_location(turn);
                for (auto sq : EnumIterator<Square, num_squares>()) {
                    if (ally_mask.is_one(sq) && (king_sq != sq))
                        append_check_moves_by_non_king_at(sq);
                }
            }
        } else {
            append_legal_moves_by_king();
            if (!m_current_state.in_check()) {
                append_legal_drop_moves();
                const auto turn = get_turn();
                const BoardType& board = get_board();
                const auto& ally_mask = board.get_occupied(turn);
                const auto king_sq = board.get_king_location(turn);
                for (auto sq : EnumIterator<Square, num_squares>()) {
                    if (ally_mask.is_one(sq) && (king_sq != sq))
                        append_legal_moves_by_non_king_at(sq);
                }
            } else if (!m_current_state.in_double_check()) {
                append_legal_moves_to_defend_king(false);
            }
        }
    }
    void append_legal_moves_by_king()
    {
        const auto ac = get_turn(); //!< ally color
        const auto ec = ~ac; //!< enemy color
        const BoardType& board = get_board();
        const auto src = board.get_king_location(ac);
        if (src == SHelper::SQ_NA)
            return;
        const auto& moving = board[src];
        auto ptr_dst = SHelper::get_non_ranging_attacks_by(moving, src);
        const auto end = ptr_dst + 8;
        const auto& ally_mask = board.get_occupied(ac);
        for (; *ptr_dst != SHelper::SQ_NA; ++ptr_dst) {
            if (ptr_dst >= end)
                break;
            if (ally_mask.is_one(*ptr_dst))
                continue;
            if (board.is_square_attacked(ec, *ptr_dst, src))
                continue;
            m_legal_moves.emplace_back(*ptr_dst, src, false);
        }
    }
    void append_check_moves_by_king()
    {
        const auto ac = get_turn();
        const auto ec = ~ac;
        const BoardType& board = get_board();
        const auto src = board.get_king_location(ac);
        const auto enemy_king_sq = board.get_king_location(ec);
        if ((src == SHelper::SQ_NA) || (enemy_king_sq == SHelper::SQ_NA))
            return;
        const auto src_dir = SHelper::get_direction(src, enemy_king_sq);
        if (src_dir == DIR_NA)
            return;
        const auto hidden_attacker_sq
            = board.find_attacker(ac, enemy_king_sq, src_dir, src);
        if (hidden_attacker_sq == SHelper::SQ_NA)
            return;
        auto ptr_dst = SHelper::get_non_ranging_attacks_by(board[src], src);
        const auto end = ptr_dst + 8;
        const auto& ally_mask = board.get_occupied(ac);
        for (; *ptr_dst != SHelper::SQ_NA; ++ptr_dst) {
            if (ptr_dst >= end)
                break;
            if (ally_mask.is_one(*ptr_dst))
                continue;
            if (SHelper::get_direction(*ptr_dst, enemy_king_sq) == src_dir)
                continue;
            if (board.is_square_attacked(ec, *ptr_dst, src))
                continue;
            m_legal_moves.emplace_back(*ptr_dst, src, false);
        }
    }
    void append_legal_moves_by_non_king_at(const Square& src)
    {
        const auto turn = get_turn();
        const BoardType& board = get_board();
        const auto king_sq = board.get_king_location(turn);
        const auto& moving = board[src];
        const auto promotable = PHelper::is_promotable(moving);
        const auto src_dir = SHelper::get_direction(src, king_sq);
        const auto hidden_attacker_sq
            = board.find_attacker(~turn, king_sq, src_dir, src);
        const auto attacks = BitBoardType::get_attacks_by(moving, src);
        const auto promotable_src = SHelper::in_promotion_zone(src, turn);
        if (hidden_attacker_sq != SHelper::SQ_NA) {
            auto ptr_dst = SHelper::get_squares_along(src_dir, king_sq);
            for (;; ++ptr_dst) {
                const auto dst = *ptr_dst;
                if (attacks.is_one(dst)) {
                    const bool promote
                        = promotable
                          && (promotable_src
                              || SHelper::in_promotion_zone(dst, turn));
                    append_legal_move_or_moves(
                        moving, dst, src, promote, false);
                }
                if (dst == hidden_attacker_sq)
                    break;
            }
            return;
        }
        append_legal_moves_by_non_king_ignoring_discovered_check(
            moving,
            attacks & (~board.get_occupied(turn)),
            src,
            promotable,
            promotable_src,
            turn,
            false);
    }
    void append_check_moves_by_non_king_at(const Square& src)
    {
        const auto turn = get_turn();
        const BoardType& board = get_board();
        const auto king_sq = board.get_king_location(turn);
        const auto enemy_king_sq = board.get_king_location(~turn);
        const auto& moving = board[src];
        const auto promotable = PHelper::is_promotable(moving);
        const auto src_dir_from_my_king = SHelper::get_direction(src, king_sq);
        const auto src_dir_from_enemy_king
            = SHelper::get_direction(src, enemy_king_sq);
        const auto counter_attacker_sq
            = board.find_attacker(~turn, king_sq, src_dir_from_my_king, src);
        const auto discovered_checker_sq = board.find_attacker(
            turn, enemy_king_sq, src_dir_from_enemy_king, src);
        const auto attacks = BitBoardType::get_attacks_by(moving, src);
        const auto promotable_src = SHelper::in_promotion_zone(src, turn);
        if (counter_attacker_sq != SHelper::SQ_NA) {
            // Needs avoiding counter attack
            for (auto pd
                 = SHelper::get_squares_along(src_dir_from_my_king, king_sq);
                 ;) {
                if (attacks.is_one(*pd)) {
                    const bool promote
                        = promotable
                          && (promotable_src
                              || SHelper::in_promotion_zone(*pd, turn));
                    if (discovered_checker_sq != SHelper::SQ_NA)
                        append_legal_move_or_moves(
                            moving, *pd, src, promote, false);
                    else
                        append_legal_move_or_moves(
                            moving, *pd, src, promote, true);
                }
                if (*pd++ == counter_attacker_sq)
                    break;
            }
        } else {
            if (discovered_checker_sq != SHelper::SQ_NA) {
                auto check_way = BitBoardType();
                {
                    for (auto sq = enemy_king_sq;;) {
                        sq = SHelper::shift(sq, src_dir_from_enemy_king);
                        if (sq == discovered_checker_sq)
                            break;
                        else
                            check_way |= BitBoardType::from_square(sq);
                    }
                }
                append_legal_moves_by_non_king_ignoring_discovered_check(
                    moving,
                    attacks & (~board.get_occupied(turn)) & (~check_way),
                    src,
                    promotable,
                    promotable_src,
                    turn,
                    false);
                for (auto pd = SHelper::get_squares_along(
                         src_dir_from_enemy_king, enemy_king_sq);
                     *pd != discovered_checker_sq;) {
                    const auto dst = *pd++;
                    if (attacks.is_one(dst)) {
                        const bool promote
                            = promotable
                              && (promotable_src
                                  || SHelper::in_promotion_zone(dst, turn));
                        append_legal_move_or_moves(
                            moving, dst, src, promote, true);
                    }
                }
            } else
                append_legal_moves_by_non_king_ignoring_discovered_check(
                    moving,
                    attacks & (~board.get_occupied(turn)),
                    src,
                    promotable,
                    promotable_src,
                    turn,
                    true);
        }
    }
    void append_legal_moves_by_non_king_ignoring_discovered_check(
        const ColoredPiece& p,
        const BitBoardType& dst_mask,
        const Square& src,
        const bool& promotable,
        const bool& src_promote,
        const ColorEnum& turn,
        const bool& restrict_legal_to_check)
    {
        auto ptr_dst = SHelper::get_non_ranging_attacks_by(p, src);
        if (ptr_dst != nullptr) {
            for (; *ptr_dst != SHelper::SQ_NA; ++ptr_dst) {
                if (!dst_mask.is_one(*ptr_dst))
                    continue;
                const bool promote
                    = promotable
                      && (src_promote
                          || SHelper::in_promotion_zone(*ptr_dst, turn));
                append_legal_move_or_moves(
                    p, *ptr_dst, src, promote, restrict_legal_to_check);
            }
            return;
        }

        const auto& enemy_mask = get_board().get_occupied(~turn);
        for (auto dp = PHelper::get_attack_directions(p); *dp != DIR_NA;) {
            ptr_dst = SHelper::get_squares_along(*dp++, src);
            for (; *ptr_dst != SHelper::SQ_NA; ++ptr_dst) {
                if (!dst_mask.is_one(*ptr_dst))
                    break;
                bool promote
                    = promotable
                      && (src_promote
                          || SHelper::in_promotion_zone(*ptr_dst, turn));
                append_legal_move_or_moves(
                    p, *ptr_dst, src, promote, restrict_legal_to_check);
                if (enemy_mask.is_one(*ptr_dst))
                    break;
            }
        }
    }
    void append_legal_move_or_moves(
        const ColoredPiece& p,
        const Square& dst,
        const Square& src,
        const bool& promotable,
        const bool& restrict_legal_to_check)
    {
        const BoardType& board = get_board();
        if (restrict_legal_to_check) {
            const auto& occupied = board.get_occupied();
            const auto enemy_king_sq = board.get_king_location(~get_turn());
            auto attacks = BitBoardType::get_attacks_by(p, dst, occupied);
            if (attacks.is_one(enemy_king_sq))
                m_legal_moves.emplace_back(dst, src, false);
            if (promotable) {
                attacks = BitBoardType::get_attacks_by(
                    PHelper::promote_nocheck(p), dst, occupied);
                if (attacks.is_one(enemy_king_sq))
                    m_legal_moves.emplace_back(dst, src, true);
            }
        } else {
            const auto attacks = BitBoardType::get_attacks_by(p, dst);
            if (!attacks.any())
                m_legal_moves.emplace_back(dst, src, true);
            else if (promotable) {
                m_legal_moves.emplace_back(dst, src, true);
                m_legal_moves.emplace_back(dst, src, false);
            } else
                m_legal_moves.emplace_back(dst, src, false);
        }
    }
    void append_legal_moves_to_defend_king(const bool& restrict_legal_to_check)
    {
        const auto turn = get_turn();
        const BoardType& board = get_board();
        const auto checker_location = m_current_state.get_checker_location();
        const auto king_location = board.get_king_location(turn);
        append_legal_moves_by_non_king_moving_to(
            checker_location, restrict_legal_to_check);
        if (!is_neighbor(king_location, checker_location)) {
            const auto dir
                = SHelper::get_direction(checker_location, king_location);
            auto ptr_dst = SHelper::get_squares_along(dir, king_location);
            for (; *ptr_dst != SHelper::SQ_NA; ++ptr_dst) {
                if (*ptr_dst == checker_location)
                    break;
                append_legal_moves_by_non_king_moving_to(
                    *ptr_dst, restrict_legal_to_check);
                append_legal_moves_dropping_to(
                    *ptr_dst, restrict_legal_to_check);
            }
        }
    }
    void append_legal_moves_by_non_king_moving_to(
        const Square& dst, const bool& restrict_legal_to_check)
    {
        const auto turn = get_turn();
        const BoardType& board = get_board();
        const auto king_location = board.get_king_location(turn);
        const auto enemy_king_sq = board.get_king_location(~turn);
        const auto target_in_promotion_zone
            = SHelper::in_promotion_zone(dst, turn);
        const auto empty_mask = ~board.get_occupied();
        const auto src_mask = board.get_occupied(turn)
                              & (~BitBoardType::from_square(king_location));
        for (auto dir : EnumIterator<DirectionEnum, num_dir>()) {
            auto ptr_src = SHelper::get_squares_along(dir, dst);
            for (; *ptr_src != SHelper::SQ_NA; ++ptr_src) {
                const auto src = *ptr_src;
                if (empty_mask.is_one(src))
                    continue;
                if (!src_mask.is_one(src))
                    break;
                const auto p = board[src];
                if (!BitBoardType::get_attacks_by(p, src).is_one(dst))
                    break;
                if (restrict_legal_to_check
                    && (!BitBoardType::get_attacks_by(
                             p, dst, board.get_occupied())
                             .is_one(enemy_king_sq)))
                    break;
                const auto src_dir = SHelper::get_direction(src, king_location);
                if ((src_dir != DIR_NA)
                    && (src_dir != SHelper::get_direction(dst, king_location))
                    && (board.find_attacker(~turn, king_location, src_dir, src)
                        != SHelper::SQ_NA))
                    break;

                const auto promote = PHelper::is_promotable(p)
                                     && (SHelper::in_promotion_zone(src, turn)
                                         || target_in_promotion_zone);
                append_legal_move_or_moves(
                    p, dst, src, promote, restrict_legal_to_check);
                break;
            }
        }
    }
    void append_check_drop_moves()
    {
        const auto turn = get_turn();
        const BoardType& board = get_board();
        const auto& stand = get_stand(turn);
        const auto enemy_king_sq = board.get_king_location(~turn);
        const Square* sq_ptr = nullptr;
        for (auto pt : EnumIterator<PieceType, num_stand_piece_types>()) {
            if (!stand.exist(pt))
                continue;
            const auto p = PHelper::to_board_piece(turn, pt);
            for (auto dp = PHelper::get_attack_directions(p); *dp != DIR_NA;) {
                sq_ptr
                    = SHelper::get_squares_along(rotate(*dp++), enemy_king_sq);
                for (; *sq_ptr != SHelper::SQ_NA;) {
                    if (!board.is_empty(*sq_ptr))
                        break;
                    const auto attacks
                        = BitBoardType::get_attacks_by(p, *sq_ptr);
                    if (!attacks.is_one(enemy_king_sq))
                        break;
                    if ((pt == PHelper::FU)
                        && (has_pawn_in_file(SHelper::to_file(*sq_ptr))
                            || is_drop_pawn_mate(*sq_ptr)))
                        break;
                    m_legal_moves.emplace_back(MoveType(*sq_ptr++, pt));
                }
            }
        }
    }
    void append_legal_drop_moves()
    {
        const auto turn = get_turn();
        const auto& stand = get_stand(turn);
        for (auto pt : EnumIterator<PieceType, num_stand_piece_types>()) {
            if (!stand.exist(pt))
                continue;
            for (auto sq : EnumIterator<Square, num_squares>()) {
                if (!get_board().is_empty(sq))
                    continue;
                const auto p = PHelper::to_board_piece(turn, pt);
                const auto attacks = BitBoardType::get_attacks_by(p, sq);
                if (!attacks.any())
                    continue;
                if ((pt == PHelper::FU)
                    && (has_pawn_in_file(SHelper::to_file(sq))
                        || is_drop_pawn_mate(sq)))
                    continue;
                m_legal_moves.emplace_back(MoveType(sq, pt));
            }
        }
    }
    void append_legal_moves_dropping_to(
        const Square& dst, const bool& restrict_legal_to_check)
    {
        const auto turn = get_turn();
        const auto enemy_king_sq = get_board().get_king_location(~turn);
        const auto& stand = get_stand(turn);
        for (auto pt : EnumIterator<PieceType, num_stand_piece_types>()) {
            if (!stand.exist(pt))
                continue;
            const auto attacks = BitBoardType::get_attacks_by(
                PHelper::to_board_piece(turn, pt), dst);
            if (!attacks.any())
                continue;
            if (restrict_legal_to_check && (!attacks.is_one(enemy_king_sq)))
                continue;
            if ((pt == PHelper::FU)
                && (has_pawn_in_file(SHelper::to_file(dst))
                    || is_drop_pawn_mate(dst)))
                continue;
            m_legal_moves.emplace_back(MoveType(dst, pt));
        }
    }
    bool has_pawn_in_file(const File f) const
    {
        const auto turn = get_turn();
        const auto& board = get_board();
        const auto pawn = PHelper::to_board_piece(turn, PHelper::FU);
        for (auto sq : SHelper::file_to_square_array[f]) {
            if (pawn == board[sq])
                return true;
        }
        return false;
    }
    bool is_drop_pawn_mate(const Square dst) const
    {
        const auto turn = get_turn();
        const BoardType& board = get_board();
        const auto pawn = PHelper::to_board_piece(turn, PHelper::FU);
        const auto enemy_king_sq = board.get_king_location(~turn);
        if (SHelper::get_non_ranging_attacks_by(pawn, dst)[0] != enemy_king_sq)
            return false;

        // if opponent king can move away from the attack, then return false.
        const auto& enemy_mask = board.get_occupied(~turn);
        const Square* sq_ptr = SHelper::get_non_ranging_attacks_by(
            board[enemy_king_sq], enemy_king_sq);
        for (; *sq_ptr != SHelper::SQ_NA; ++sq_ptr) {
            if (enemy_mask.is_one(*sq_ptr))
                continue;
            if (board.is_square_attacked(turn, *sq_ptr, enemy_king_sq))
                continue;
            return false;
        }

        // if opponent can capture the dropped pawn, then return false.
        const auto enemy_king_direction = (turn == BLACK) ? DIR_N : DIR_S;
        for (auto dir : EnumIterator<DirectionEnum, num_dir>()) {
            if (dir == enemy_king_direction)
                continue;
            const auto src_next = board.find_attacker(~turn, dst, dir);
            const auto attacking_the_pawn = (src_next != SHelper::SQ_NA);
            if (attacking_the_pawn) {
                const auto discovered_dir
                    = SHelper::get_direction(src_next, enemy_king_sq);
                const auto discovered_attacker_sq = board.find_attacker(
                    turn, enemy_king_sq, discovered_dir, src_next);
                const auto pinned = (discovered_attacker_sq != SHelper::SQ_NA);
                if (!pinned)
                    return false;
            }
        }
        return true;
    }
    static bool is_neighbor(const Square a, const Square b)
    {
        for (auto dir : EnumIterator<DirectionEnum, num_dir>()) {
            if (b == SHelper::shift(a, static_cast<DirectionEnum>(dir)))
                return true;
        }
        return false;
    }
};

} // namespace vshogi

#endif // VSHOGI_GAME_HPP
