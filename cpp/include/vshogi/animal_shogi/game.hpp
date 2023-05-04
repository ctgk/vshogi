#ifndef VSHOGI_ANIMAL_SHOGI_GAME_HPP
#define VSHOGI_ANIMAL_SHOGI_GAME_HPP

#include <cstdint>
#include <string>
#include <vector>

#include "vshogi/animal_shogi/move.hpp"
#include "vshogi/animal_shogi/state.hpp"
#include "vshogi/result.hpp"

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
    std::vector<StateActionHash> m_record;
    State m_current_state;
    ResultEnum m_result;

    static constexpr std::uint64_t state_mask = 0x0ffffffffffffff;

public:
    Game() : m_record(), m_current_state(), m_result(ONGOING)
    {
        m_record.reserve(128);
    }
    Game(const std::uint64_t hash_value)
        : m_record(), m_current_state(), m_result()
    {
        m_record.reserve(128);
        m_current_state.set_hash(hash_value);
    }
    Game(const std::string& sfen)
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
    std::uint64_t hash_current_state() const
    {
        return m_current_state.hash();
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

    /**
     * @brief Apply an legal move to the current state.
     *
     * @param move legal move.
     * @return Game&
     */
    Game& apply(const Move move)
    {
        m_record.emplace_back(
            (static_cast<std::uint64_t>(move.hash()) << 56)
            + m_current_state.hash());
        const auto turn = get_turn();
        PieceTypeEnum moved_piece, captured_piece;
        m_current_state.apply(move, &moved_piece, &captured_piece);
        update_result(
            turn, moved_piece, captured_piece, to_rank(move.destination()));
        return *this;
    }
    std::size_t record_length() const
    {
        return m_record.size();
    }
    Move get_move_at(const std::size_t n) const
    {
        const auto state_action_hash = m_record[n];
        const auto action_hash = state_action_hash >> 56;
        return Move(static_cast<std::uint8_t>(action_hash));
    }
    std::uint64_t get_state_hash_at(const std::size_t n) const
    {
        const auto state_action_hash = m_record[n];
        return state_action_hash & 0x0ffffffffffffffUL;
    }

private:
    bool is_fourfold_repetitions() const
    {
        constexpr int num_acceptable_repetitions = 3;
        int num = 1;
        const auto current_hash = m_current_state.hash();
        for (auto&& previous_hash : m_record) {
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
