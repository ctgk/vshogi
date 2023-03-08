#ifndef VSHOGI_ANIMAL_SHOGI_GAME_HPP
#define VSHOGI_ANIMAL_SHOGI_GAME_HPP

#include <cstdint>
#include <deque>
#include <string>

#include "vshogi/animal_shogi/move.hpp"
#include "vshogi/animal_shogi/state.hpp"

namespace vshogi::animal_shogi
{

/**
 * @brief Pair of a state and action. The action turns the state to the next
 * one.
 *
 */
struct StateActionPair
{
    std::uint64_t hashed_state;
    Move action;

    StateActionPair(const std::uint64_t s, const Move a)
        : hashed_state(s), action(a)
    {
    }
};

/**
 * @brief Animal Shogi game.
 *
 */
class Game
{
private:
    std::deque<StateActionPair> m_history;
    State m_current_state;

public:
    Game();
    Game(const std::string& sfen);

    ColorEnum get_turn() const
    {
        return m_current_state.get_turn();
    }
    const Board& get_board() const
    {
        return m_current_state.get_board();
    }
    const PieceStand& get_piece_stand(const ColorEnum c) const
    {
        return m_current_state.get_piece_stand(c);
    }
    bool is_move_legal(const Move move) const
    {
        return m_current_state.is_move_legal(move);
    }
    Game& apply_move(const Move move)
    {
        m_history.emplace_back(StateActionPair(m_current_state.hash(), move));
        m_current_state.apply_move(move);
        return *this;
    }
    std::size_t nth_move() const
    {
        return m_history.size() + 1;
    }
};

} // namespace vshogi::animal_shogi

#endif // VSHOGI_ANIMAL_SHOGI_GAME_HPP
