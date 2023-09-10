#ifndef VSHOGI_SHOGI_GAME_HPP
#define VSHOGI_SHOGI_GAME_HPP

#include <string>
#include <utility>
#include <vector>

#include "vshogi/game.hpp"
#include "vshogi/result.hpp"
#include "vshogi/shogi/move.hpp"
#include "vshogi/shogi/state.hpp"

namespace vshogi::shogi
{

class Game : public vshogi::Game<State, Board, Stand, Move>
{
public:
    Game() : vshogi::Game<State, Board, Stand, Move>()
    {
    }
    Game(const std::string& sfen)
        : vshogi::Game<State, Board, Stand, Move>(sfen)
    {
    }
    Game& apply(const Move move)
    {
        m_record.emplace_back(std::make_pair(m_current_state.to_sfen(), move));
        m_current_state.apply(move);
        update_result();
        return *this;
    }
};

} // namespace vshogi::shogi

#endif // VSHOGI_SHOGI_GAME_HPP
