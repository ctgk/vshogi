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
    Move get_valid_move_to(
        const SquareEnum dst,
        const DirectionEnum dir,
        const bool promote = false) const
    {
        return m_current_state.get_valid_move_to(dst, dir, promote);
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
