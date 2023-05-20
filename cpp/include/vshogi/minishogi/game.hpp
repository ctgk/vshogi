#ifndef VSHOGI_MINISHOGI_GAME_HPP
#define VSHOGI_MINISHOGI_GAME_HPP

#include <string>
#include <utility>
#include <vector>

#include "vshogi/minishogi/move.hpp"
#include "vshogi/minishogi/state.hpp"
#include "vshogi/result.hpp"

namespace vshogi::minishogi
{

class Game
{
private:
    /**
     * @brief Pairs of SFEN (without #ply) and the move.
     *
     */
    std::vector<std::pair<std::string, Move>> m_record;
    State m_current_state;
    ResultEnum m_result;

public:
    Game() : m_record(), m_current_state(), m_result(ONGOING)
    {
        m_record.reserve(128);
    }
    Game(const std::string sfen)
        : m_record(), m_current_state(sfen), m_result(ONGOING)
    {
        m_record.reserve(128);
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
    bool is_legal(const Move move) const
    {
        return m_current_state.is_legal(move);
    }
    auto get_legal_moves() const
    {
        if (m_result == ONGOING)
            return m_current_state.get_legal_moves();
        return std::vector<Move>();
    }
    Game& apply(const Move move)
    {
        m_record.emplace_back(std::make_pair(m_current_state.to_sfen(), move));
        m_current_state.apply(move);
        update_result();
        return *this;
    }
    std::size_t record_length() const
    {
        return m_record.size();
    }
    Move get_move_at(const std::size_t n) const
    {
        return m_record[n].second;
    }
    std::string get_sfen_at(const std::size_t n) const
    {
        return m_record[n].first;
    }
    static constexpr int feature_channels()
    {
        return 2 * (10 + 5); // 2-player * (10-board-piece + 5-stand-piece)
    }
    void to_feature_map(float* const data) const
    {
        constexpr int stand_piece_types = 5;
        constexpr int board_piece_types = 10;
        constexpr int unpromoted_piece_types = 6;
        constexpr int ch_half = stand_piece_types + board_piece_types;
        constexpr int ch = feature_channels();

        const auto turn = get_turn();
        const auto stand_curr = get_stand(turn);
        const auto stand_next = get_stand(~turn);
        const auto board = get_board();

        for (int k = stand_piece_types; k--;) {
            const auto p = stand_piece_array[k];
            const auto num_curr = static_cast<float>(stand_curr.count(p));
            const auto num_next = static_cast<float>(stand_next.count(p));
            for (int i = num_squares; i--;) {
                data[i * ch + k] = num_curr;
                data[i * ch + k + ch_half] = num_next;
            }
        }
        for (int i = num_squares; i--;) {
            const auto sq
                = square_array[(turn == BLACK) ? i : (num_squares - 1 - i)];
            const auto board_piece = board[sq];
            for (int k = board_piece_types; k--;) {
                data[i * ch + k + stand_piece_types] = 0.f;
                data[i * ch + k + stand_piece_types + ch_half] = 0.f;
            }
            if (board_piece == VOID)
                continue;
            const auto color = get_color(board_piece);
            const auto piece_type = to_piece_type(board_piece);
            auto k = static_cast<int>(demote(piece_type));
            k += (is_promoted(piece_type)) ? unpromoted_piece_types : 0;
            k += (turn == color) ? 0 : ch_half;
            data[i * ch + k + stand_piece_types] = 1.f;
        }
    }

private:
    void update_result()
    {
        const auto turn = get_turn();
        const auto legal_moves = get_legal_moves();
        if (legal_moves.empty())
            m_result = (turn == BLACK) ? WHITE_WIN : BLACK_WIN;
        if (is_fourfold_repetitions()) {
            if (get_board().in_check(turn))
                m_result = (turn == BLACK) ? BLACK_WIN : WHITE_WIN;
            else
                m_result = DRAW;
        }
    }
    bool is_fourfold_repetitions() const
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
};

} // namespace vshogi::minishogi

#endif // VSHOGI_MINISHOGI_GAME_HPP
