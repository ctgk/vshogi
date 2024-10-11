#ifndef VSHOGI_GAME_HPP
#define VSHOGI_GAME_HPP

#include <algorithm>
#include <string>
#include <vector>

#include "vshogi/common/bitboard.hpp"
#include "vshogi/common/board.hpp"
#include "vshogi/common/color.hpp"
#include "vshogi/common/direction.hpp"
#include "vshogi/common/generator.hpp"
#include "vshogi/common/move.hpp"
#include "vshogi/common/pieces.hpp"
#include "vshogi/common/result.hpp"
#include "vshogi/common/squares.hpp"
#include "vshogi/common/stand.hpp"
#include "vshogi/common/state.hpp"

namespace vshogi
{

template <class Config>
class Game
{
public:
    static constexpr uint num_ranks = Config::num_ranks;
    static constexpr uint num_files = Config::num_files;
    static constexpr uint num_squares = Config::num_squares;

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
    static constexpr uint max_acceptable_repetitions
        = Config::max_acceptable_repetitions;
    static constexpr std::uint64_t lsb40bit = 0x000000ffffffffffu;

private:
    StateType m_current_state;
    ResultEnum m_result;
    std::uint64_t m_captured_move_hash;
    const std::string m_initial_sfen_without_ply;
    std::vector<ZobristHashType> m_hash_list;
    uint m_num_fold;

public:
    Game() : Game(StateType())
    {
    }
    Game(const std::string& sfen) : Game(StateType(sfen))
    {
    }
    static constexpr uint feature_channels()
    {
        // 2-player * (piece-types + stand-piece-types)
        return StateType::feature_channels();
    }
    static constexpr uint num_dlshogi_policy()
    {
        return StateType::num_dlshogi_policy();
    }
    ColorEnum get_turn() const
    {
        return m_current_state.get_turn();
    }
    const StateType& get_state() const
    {
        return m_current_state;
    }
    const BoardType& get_board() const
    {
        return m_current_state.get_board();
    }
    const StandType& get_stand(const ColorEnum c) const
    {
        return m_current_state.get_stand(c);
    }
    std::vector<MoveType> get_legal_moves() const
    {
        std::vector<MoveType> out{};
        if (m_result != ONGOING)
            return out;
        for (auto m : LegalMoveGenerator<Config>(m_current_state))
            out.emplace_back(m);
        return out;
    }
    Square get_checker_location(const uint index = 0u) const
    {
        return m_current_state.get_checker_location(index);
    }
    ResultEnum get_result() const
    {
        return m_result;
    }
    uint get_num_fold() const
    {
        return m_num_fold;
    }
    std::uint64_t get_zobrist_hash() const
    {
        return m_captured_move_hash & lsb40bit;
    }
    std::string to_sfen(const bool include_move_count = true) const
    {
        if (include_move_count)
            return m_current_state.to_sfen() + " "
                   + std::to_string(m_hash_list.size() + 1);
        else
            return m_current_state.to_sfen();
    }
    std::size_t record_length() const
    {
        return m_hash_list.size();
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
        update_result();
        return *this;
    }
    Game& apply_random_playout(const MoveType& move)
    {
        add_record_and_update_state(move);
        update_result_for_random_playout();
        return *this;
    }
    Game& apply_dfpn(const MoveType& move)
    {
        add_record_and_update_state_for_dfpn(move);
        update_result_for_dfpn();
        return *this;
    }
    Game copy_and_apply_dfpn(const MoveType& move)
    {
        auto out = Game(
            m_current_state,
            m_result,
            m_captured_move_hash,
            m_initial_sfen_without_ply,
            m_hash_list);
        out.apply_dfpn(move);
        return out;
    }
    bool is_legal(const MoveType move) const
    {
        if (move.is_drop()) {
            for (auto m : DropMoveGenerator<Config>(m_current_state)) {
                if (m == move)
                    return true;
            }
        } else if (
            move.source_square() == get_board().get_king_location(get_turn())) {
            for (auto m : KingMoveGenerator<Config>(m_current_state)) {
                if (m == move)
                    return true;
            }
        } else {
            for (auto m : NonKingBoardMoveGenerator<Config>(m_current_state)) {
                if (m == move)
                    return true;
            }
        }
        return false;
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
    void clear_records_for_dfpn()
    {
        m_hash_list.clear();
        m_hash_list.emplace_back(m_current_state.get_board().zobrist_hash());
    }
    void to_feature_map(float* const data) const
    {
        m_current_state.to_feature_map(data);
    }
    static void attention_matrix(float* const data)
    {
        std::fill_n(data, num_squares * num_squares, 0.f);
        for (auto sq : EnumIterator<Square, num_squares>()) {
            const uint ii = static_cast<uint>(sq);
            for (auto dir : EnumIterator<DirectionEnum, num_dir>()) {
                auto ptr_sq = SHelper::get_squares_along(dir, sq);
                for (; *ptr_sq != SHelper::SQ_NA; ++ptr_sq) {
                    const uint jj = static_cast<uint>(*ptr_sq);
                    data[ii * num_squares + jj] = 1.f;
                }
            }
        }
    }
    static void attention_matrix(
        float* const data, const std::vector<DirectionEnum>& directions)
    {
        std::fill_n(data, num_squares * num_squares, 0.f);
        for (auto sq : EnumIterator<Square, num_squares>()) {
            const uint ii = static_cast<uint>(sq);
            for (auto dir : directions) {
                auto ptr_sq = SHelper::get_squares_along(dir, sq);
                for (; *ptr_sq != SHelper::SQ_NA; ++ptr_sq) {
                    const uint jj = static_cast<uint>(*ptr_sq);
                    data[ii * num_squares + jj] = 1.f;
                }
            }
        }
    }

protected:
    Game(const StateType& s)
        : m_current_state(s), m_result(ONGOING),
          m_captured_move_hash(m_current_state.zobrist_hash() & lsb40bit),
          m_initial_sfen_without_ply(m_current_state.to_sfen()), m_hash_list{},
          m_num_fold(1u)
    {
        m_hash_list.reserve(256);
        update_result();
    }
    Game(
        const StateType& s,
        const ResultEnum& result,
        const uint64_t& zobrist_hash,
        const std::string& initial_sfen_without_ply,
        const std::vector<ZobristHashType>& hash_list)
        : m_current_state(s), m_result(result),
          m_captured_move_hash(zobrist_hash & lsb40bit),
          m_initial_sfen_without_ply(initial_sfen_without_ply),
          m_hash_list(hash_list), m_num_fold(1u)
    {
    }
    static uint num_pieces(const StateType& s, const ColorEnum& c)
    {
        const auto& board = s.get_board();
        const auto& stand = s.get_stand(c);
        uint out = 0u;
        for (auto sq : EnumIterator<Square, num_squares>()) {
            const auto p = board[sq];
            if (p == PHelper::VOID)
                continue;
            if (PHelper::get_color(p) == c)
                out += 1u;
        }
        for (auto pt : EnumIterator<PieceType, num_stand_piece_types>())
            out += stand.count(pt);
        return out;
    }
    static uint total_point(const StateType& s, const ColorEnum& c)
    {
        uint out = 0u;
        const auto& board = s.get_board();
        const auto& stand = s.get_stand(c);
        for (auto sq : EnumIterator<Square, num_squares>()) {
            const auto p = board[sq];
            if (PHelper::get_color(p) == c)
                out += PHelper::get_point(p);
        }
        for (auto pt : EnumIterator<PieceType, num_stand_piece_types>()) {
            out += stand.count(pt) * PHelper::get_point(pt);
        }
        return out;
    }

protected:
    void add_record_and_update_state(const MoveType& move)
    {
        m_hash_list.emplace_back(m_captured_move_hash);
        m_current_state.apply(move, &m_captured_move_hash);
    }
    void add_record_and_update_state_for_dfpn(const MoveType& move)
    {
        m_current_state.apply(move, &m_captured_move_hash);
        m_hash_list.emplace_back(m_current_state.get_board().zobrist_hash());
    }

protected:
    void update_result()
    {
        m_result = ONGOING;
        const auto turn = get_turn();
        if (LegalMoveGenerator<Config>(m_current_state).is_end())
            m_result = (turn == BLACK) ? WHITE_WIN : BLACK_WIN;
        if (is_repetitions()) {
            if (m_current_state.in_check())
                m_result = (turn == BLACK) ? BLACK_WIN : WHITE_WIN;
            else
                m_result = DRAW;
        }
        if (can_declare_win_by_king_enter())
            m_result = (turn == BLACK) ? BLACK_WIN : WHITE_WIN;
    }
    void update_result_for_dfpn()
    {
        m_result = ONGOING;
        const auto turn = get_turn();
        if (LegalMoveGenerator<Config>(m_current_state).is_end())
            m_result = (turn == BLACK) ? WHITE_WIN : BLACK_WIN;
        if (is_repetitions(1u))
            m_result = DRAW;
        if (can_declare_win_by_king_enter())
            m_result = (turn == BLACK) ? BLACK_WIN : WHITE_WIN;
    }
    void update_result_for_random_playout()
    {
        m_result = ONGOING;
        const auto turn = get_turn();
        if (LegalMoveGenerator<Config>(m_current_state).is_end())
            m_result = (turn == BLACK) ? WHITE_WIN : BLACK_WIN;
        if (is_repetitions(1u))
            m_result = DRAW;
        if (can_declare_win_by_king_enter())
            m_result = (turn == BLACK) ? BLACK_WIN : WHITE_WIN;
    }
    bool is_repetitions(
        const uint max_repetitions_inclusive = max_acceptable_repetitions)
    {
        m_num_fold = 1u;
        const auto hash = lsb40bit & m_captured_move_hash;
        const int n = static_cast<int>(m_hash_list.size());
        for (int ii = n - 4; ii >= 0; ii -= 2) {
            const uint index = static_cast<uint>(ii);
            m_num_fold += (hash == (m_hash_list[index] & lsb40bit));
            if (m_num_fold > max_repetitions_inclusive)
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
        if (num_pieces_in_zone <= Config::half_num_initial_pieces)
            return false;

        // (2) The declaring side has 28 (the first player (sente, black)) or
        // 27 (the second player (gote, white)) piece points or more.
        if (turn == BLACK)
            return count_point_of(turn, piece_mask) > Config::initial_points;
        else
            return count_point_of(turn, piece_mask) >= Config::initial_points;
    }
    uint count_point_of(const ColorEnum& c, const BitBoardType& mask) const
    {
        uint out = 0;
        const BoardType& board = get_board();
        for (auto sq : mask.square_iterator())
            out += PHelper::get_point(board[sq]);
        const auto& stand = get_stand(c);
        for (auto pt : EnumIterator<PieceType, num_stand_piece_types>()) {
            out += stand.count(pt) * PHelper::get_point(pt);
        }
        return out;
    }
};

} // namespace vshogi

#endif // VSHOGI_GAME_HPP
