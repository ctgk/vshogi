#ifndef VSHOGI_GAME_HPP
#define VSHOGI_GAME_HPP

#include <algorithm>
#include <string>
#include <vector>

#include "vshogi/color.hpp"
#include "vshogi/result.hpp"

namespace vshogi
{

template <class State>
class Game
{
public:
    using Board = typename State::BoardType;
    using Stand = typename State::StandType;
    using Move = typename State::MoveType;
    using Squares = typename State::SquaresType;
    using Pieces = typename State::PiecesType;

protected:
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
        update_result();
    }
    Game(const std::string& sfen)
        : m_record(), m_legal_moves(), m_current_state(sfen), m_result(ONGOING)
    {
        m_record.reserve(128);
        update_result();
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
        update_result();
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
    void update_result()
    {
        const auto turn = get_turn();
        m_current_state.get_legal_moves(m_legal_moves);
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
};

} // namespace vshogi

#endif // VSHOGI_GAME_HPP
