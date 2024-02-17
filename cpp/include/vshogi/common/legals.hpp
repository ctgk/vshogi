#ifndef VSHOGI_COMMON_LEGALS_HPP
#define VSHOGI_COMMON_LEGALS_HPP

#include <vector>

namespace vshogi
{

template <class Game>
void append_legal_moves_by_king(
    std::vector<typename Game::Move>& out, const Game& game)
{
    using Move = typename Game::Move;
    using Squares = typename Game::Squares;

    const auto ac = game.get_turn(); //!< ally color
    const auto ec = ~ac; //!< enemy color
    const auto src = game.get_king_location(ac);
    if (src == Squares::SQ_NA)
        return;
    const auto& board = game.get_board();
    const auto moving = board[src];
    auto ptr_dst = Squares::get_non_ranging_attacks_by(moving, src);
    const auto& ally_mask = game.get_occupied(ac);
    for (; *ptr_dst != Squares::SQ_NA; ++ptr_dst) {
        if (ally_mask.is_one(*ptr_dst))
            continue;
        if (board.is_square_attacked(ec, *ptr_dst, src))
            continue;
        out.emplace_back(Move(*ptr_dst, src, false));
    }
}

template <class Game>
void append_legal_moves_by_drop(
    std::vector<typename Game::Move>& out, const Game& game)
{
    using BitBoard = typename Game::BitBoard;
    using Move = typename Game::Move;
    using Pieces = typename Game::Pieces;
    using Squares = typename Game::Squares;

    const auto turn = game.get_turn();
    const auto& stand = game.get_stand(turn);
    const auto& occupied = game.get_occupied();
    const auto& board = game.get_board();
    for (auto&& pt : Pieces::stand_piece_array) {
        if (!stand.exist(pt))
            continue;
        for (auto&& sq : Squares::square_array) {
            if (occupied.is_one(sq))
                continue;
            const auto p = Pieces::to_board_piece(turn, pt);
            const auto attacks = BitBoard::get_attacks_by(p, sq);
            if (!attacks.any())
                continue;
            if ((pt == Pieces::FU)
                && (board.has_pawn_in_file(turn, Squares::to_file(sq))
                    || game.is_drop_pawn_mate(sq)))
                continue;
            out.emplace_back(Move(sq, pt));
        }
    }
}

template <class Game>
std::vector<typename Game::Move> get_legal_moves(const Game& game)
{
    using Move = typename Game::Move;
    using Squares = typename Game::Squares;
    std::vector<Move> out;
    append_legal_moves_by_king(out, game);
    if (!game.in_check()) {
        append_legal_moves_by_drop(out, game);
        append_legal_moves_by_non_king(out, game);
    } else if (game.get_checker_locations[1] == Squares::SQ_NA) {
        append_legal_moves_to_defend_king(out, game);
    }
    return out;
}

} // namespace vshogi

#endif // VSHOGI_COMMON_LEGALS_HPP
