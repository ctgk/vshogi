#ifndef VSHOGI_GAME_HPP
#define VSHOGI_GAME_HPP

#include <string>
#include <vector>

#include "vshogi/color.hpp"
#include "vshogi/result.hpp"

namespace vshogi
{

template <class State, class Board, class Stand, class Move>
class Game
{
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
        return Board::ranks();
    }
    static constexpr int files()
    {
        return Board::files();
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
    bool is_legal(const Move move) const
    {
        return m_current_state.is_legal(move);
    }

protected:
    void update_result()
    {
        const auto turn = get_turn();
        m_legal_moves = m_current_state.get_legal_moves();
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
