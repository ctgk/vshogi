#ifndef VSHOGI_ANIMAL_SHOGI_GAME_HPP
#define VSHOGI_ANIMAL_SHOGI_GAME_HPP

#include <cstdint>
#include <string>
#include <vector>

#include "vshogi/animal_shogi/move.hpp"
#include "vshogi/animal_shogi/state.hpp"

namespace vshogi::animal_shogi
{

/**
 * @brief Animal Shogi game.
 *
 */
class Game
{
private:
    using StateActionHash = std::uint64_t;
    std::vector<StateActionHash> m_history;
    State m_current_state;
    ResultEnum m_result;

public:
    Game() : m_history(), m_current_state(), m_result(UNKNOWN)
    {
        m_history.reserve(128);
    }
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
    ResultEnum get_result() const
    {
        return m_result;
    }
    bool is_move_applicable(const Move move) const
    {
        return m_current_state.is_move_applicable(move);
    }

    /**
     * @brief Apply an applicable move to the current state.
     *
     * @param move Applicable move.
     * @return Game&
     */
    Game& apply_move(const Move move)
    {
        m_history.emplace_back(
            (static_cast<std::uint64_t>(move.hash()) << 56)
            + m_current_state.hash());
        const auto turn = get_turn();
        PieceTypeEnum moved_piece, captured_piece;
        m_current_state.apply_move(move, &moved_piece, &captured_piece);
        update_result(
            turn, moved_piece, captured_piece, to_rank(move.destination()));
        return *this;
    }
    std::size_t nth_move() const
    {
        return m_history.size() + 1;
    }

private:
    void update_result(
        const ColorEnum turn,
        const PieceTypeEnum moving,
        const PieceTypeEnum capturing,
        const RankEnum destination)
    {
        // TODO: Implement three fold repetitions.
        if (moving == LI) {
            if (turn == BLACK && destination == RANK1)
                m_result = BLACK_WIN;
            if (turn == WHITE && destination == RANK4)
                m_result = WHITE_WIN;
        }
        if (capturing == LI) {
            m_result = (turn == BLACK) ? BLACK_WIN : WHITE_WIN;
        }
    }
};

} // namespace vshogi::animal_shogi

#endif // VSHOGI_ANIMAL_SHOGI_GAME_HPP
