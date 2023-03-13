#ifndef VSHOGI_ANIMAL_SHOGI_GAME_HPP
#define VSHOGI_ANIMAL_SHOGI_GAME_HPP

#include <cstdint>
#include <string>
#include <vector>

#include "vshogi/animal_shogi/move.hpp"
#include "vshogi/animal_shogi/state.hpp"

namespace vshogi::animal_shogi
{

enum ResultEnum : std::uint8_t
{
    ONGOING,
    DRAW,
    BLACK_WIN,
    WHITE_WIN,
};

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

    static constexpr std::uint64_t state_mask = 0x0ffffffffffffff;

public:
    Game() : m_history(), m_current_state(), m_result(ONGOING)
    {
        m_history.reserve(128);
    }
    Game(const std::string& sfen)
        : m_history(), m_current_state(sfen), m_result(ONGOING)
    {
        m_history.reserve(128);
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
    bool is_applicable(const Move move) const
    {
        return m_current_state.is_applicable(move);
    }

    /**
     * @brief Apply an applicable move to the current state.
     *
     * @param move Applicable move.
     * @return Game&
     */
    Game& apply(const Move move)
    {
        m_history.emplace_back(
            (static_cast<std::uint64_t>(move.hash()) << 56)
            + m_current_state.hash());
        const auto turn = get_turn();
        PieceTypeEnum moved_piece, captured_piece;
        m_current_state.apply(move, &moved_piece, &captured_piece);
        update_result(
            turn, moved_piece, captured_piece, to_rank(move.destination()));
        return *this;
    }
    std::size_t nth_move() const
    {
        return m_history.size() + 1;
    }

private:
    bool is_fourfold_repetitions() const
    {
        constexpr int num_acceptable_repetitions = 3;
        int num = 0;
        const auto current_hash = m_current_state.hash();
        for (auto&& previous_hash : m_history) {
            num += (current_hash == (previous_hash & state_mask));
            if (num > num_acceptable_repetitions) {
                return true;
            }
        }
        return false;
    }
    void update_result(
        const ColorEnum turn,
        const PieceTypeEnum moving,
        const PieceTypeEnum capturing,
        const RankEnum destination)
    {
        if (moving == LI) {
            if (turn == BLACK && destination == RANK1)
                m_result = BLACK_WIN;
            if (turn == WHITE && destination == RANK4)
                m_result = WHITE_WIN;
        }
        if (capturing == LI) {
            m_result = (turn == BLACK) ? BLACK_WIN : WHITE_WIN;
        }
        if (is_fourfold_repetitions()) {
            m_result = DRAW;
        }
    }
};

} // namespace vshogi::animal_shogi

#endif // VSHOGI_ANIMAL_SHOGI_GAME_HPP
