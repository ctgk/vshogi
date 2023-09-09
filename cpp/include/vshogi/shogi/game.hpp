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
    static constexpr int feature_channels()
    {
        return 2 * (14 + 7); // 2-player * (14-board-piece + 7-stand-piece)
    }
    static constexpr int num_dlshogi_policy()
    {
        return ranks() * files() * Move::num_policy_per_square();
    }
    Game& apply(const Move move)
    {
        m_record.emplace_back(std::make_pair(m_current_state.to_sfen(), move));
        m_current_state.apply(move);
        update_result();
        return *this;
    }
    void to_feature_map(float* const data) const
    {
        constexpr int stand_piece_types = 7;
        constexpr int board_piece_types = 14;
        constexpr int unpromoted_piece_types = stand_piece_types + 1; // + OU
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
};

} // namespace vshogi::shogi

#endif // VSHOGI_SHOGI_GAME_HPP
