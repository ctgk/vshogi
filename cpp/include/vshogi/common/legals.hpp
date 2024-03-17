#ifndef VSHOGI_COMMON_LEGALS_HPP
#define VSHOGI_COMMON_LEGALS_HPP

#include "vshogi/common/direction.hpp"

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

template <class Move, class Square>
void append_legal_moves_by_board_piece(
    std::vector<Move>& out,
    const Square& dst,
    const Square& src,
    const bool& promotable,
    const bool& force_promote)
{
    if (!force_promote)
        out.emplace_back(Move(dst, src, false));
    if (promotable)
        out.emplace_back(Move(dst, src, true));
}

template <class Game>
void append_legal_moves_by_non_king(
    std::vector<typename Game::Move>& out, const Game& game)
{
    using BitBoard = typename Game::BitBoard;
    using Move = typename Game::Move;
    using Pieces = typename Game::Pieces;
    using Squares = typename Game::Squares;

    const auto turn = game.get_turn();
    const auto& board = game.get_board();
    const auto& ally_mask = game.get_occupied(turn);
    const auto& king_sq = game.get_king_location(turn);
    const auto& occupied = game.get_occupied();
    for (auto&& src : Squares::square_array) {
        if ((!ally_mask.is_one(src)) || (src == king_sq))
            continue;

        const auto& moving = board[src];
        const auto src_dir_from_king = Squares::get_direction(src, king_sq);
        const auto promotable_piece = Pieces::is_promotable(moving);
        const auto promotable_src = Squares::in_promotion_zone(src, turn);
        const auto hidden_attacker_sq
            = board.find_attacker(~turn, king_sq, src_dir_from_king, src);
        if (hidden_attacker_sq != Squares::SQ_NA) {
            auto ptr_dst
                = Squares::get_squares_along(src_dir_from_king, king_sq);
            const auto dst_mask = BitBoard::get_attacks_by(moving, src);
            for (; *ptr_dst != Squares::SQ_NA; ++ptr_dst) {
                if (!dst_mask.is_one(*ptr_dst))
                    continue;
                append_legal_moves_by_board_piece(
                    out,
                    *ptr_dst,
                    src,
                    promotable_piece
                        && (promotable_src
                            || Squares::in_promotion_zone(*ptr_dst, turn)),
                    !BitBoard::get_attacks_by(moving, *ptr_dst).any());
                if (occupied.is_one(*ptr_dst))
                    break;
            }
        } else if (Pieces::is_ranging_piece(moving)) {
            const auto dst_mask
                = BitBoard::get_attacks_by(moving, src) & (~ally_mask);
            for (auto dp = Pieces::get_attack_directions(moving); *dp != DIR_NA;
                 ++dp) {
                auto ptr_dst = Squares::get_squares_along(*dp, src);
                for (; *ptr_dst != Squares::SQ_NA; ++ptr_dst) {
                    if (!dst_mask.is_one(*ptr_dst))
                        break;
                    append_legal_moves_by_board_piece(
                        out,
                        *ptr_dst,
                        src,
                        promotable_piece
                            && (promotable_src
                                || Squares::in_promotion_zone(*ptr_dst, turn)),
                        !BitBoard::get_attacks_by(moving, *ptr_dst).any());
                    if (occupied.is_one(*ptr_dst))
                        break;
                }
            }
        } else {
            const auto dst_mask
                = BitBoard::get_attacks_by(moving, src) & (~ally_mask);
            auto ptr_dst = Squares::get_non_ranging_attacks_by(moving, src);
            for (; *ptr_dst != Squares::SQ_NA; ++ptr_dst) {
                if (!dst_mask.is_one(*ptr_dst))
                    continue;
                append_legal_moves_by_board_piece(
                    out,
                    *ptr_dst,
                    src,
                    promotable_piece
                        && (promotable_src
                            || Squares::in_promotion_zone(*ptr_dst, turn)),
                    !BitBoard::get_attacks_by(moving, *ptr_dst).any());
            }
        }
    }
}

template <class Game>
void append_legal_moves_by_non_king_moving_to(
    std::vector<typename Game::Move>& out,
    const Game& game,
    const typename Game::SquareEnum& dst)
{
    using BitBoard = typename Game::BitBoard;
    using Pieces = typename Game::Pieces;
    using Squares = typename Game::Squares;

    const auto turn = game.get_turn();
    const auto& board = game.get_board();
    const auto& king_location = game.get_king_location(turn);
    const auto& enemy_mask = game.get_occupied(~turn);
    const auto& occupied = game.get_occupied();
    const auto promotable_dst = Squares::in_promotion_zone(dst, turn);

    for (auto&& dir : Squares::direction_array) {
        auto ptr_src = Squares::get_squares_along(dir, dst);
        for (; *ptr_src != Squares::SQ_NA; ++ptr_src) {
            const auto& src = *ptr_src;
            if (!occupied.is_one(src)) // vacant
                continue;
            if (enemy_mask.is_one(src) || (src == king_location))
                break;
            const auto& p = board[src];
            if (!BitBoard::get_attacks_by(p, src).is_one(dst))
                break;
            const auto src_dir = Squares::get_direction(src, king_location);
            if ((src_dir != DIR_NA)
                && (src_dir != Squares::get_direction(dst, king_location))
                && (board.find_attacker(~turn, king_location, src_dir, src)
                    != Squares::SQ_NA))
                break;
            append_legal_moves_by_board_piece(
                out,
                dst,
                src,
                Pieces::is_promotable(p)
                    && (promotable_dst
                        || Squares::in_promotion_zone(src, turn)),
                !BitBoard::get_attacks_by(p, dst).any());
            break;
        }
    }
}

template <class Game>
void append_legal_moves_defending_king_dropping_to(
    std::vector<typename Game::Move>& out,
    const Game& game,
    const typename Game::SquareEnum& dst)
{
    using BitBoard = typename Game::BitBoard;
    using Pieces = typename Game::Pieces;
    using Move = typename Game::Move;
    using Squares = typename Game::Squares;

    const auto turn = game.get_turn();
    const auto& board = game.get_board();
    const auto& stand = game.get_stand(turn);
    for (auto&& pt : Pieces::stand_piece_array) {
        if (!stand.exist(pt))
            continue;
        const auto p = Pieces::to_board_piece(turn, pt);
        const auto attacks = BitBoard::get_attacks_by(p, dst);
        if (!attacks.any())
            continue;
        if ((pt == Pieces::FU)
            && board.has_pawn_in_file(turn, Squares::to_file(dst)))
            continue;
        out.emplace_back(Move(dst, pt));
    }
}

template <class Game>
void append_legal_moves_to_defend_king(
    std::vector<typename Game::Move>& out, const Game& game)
{
    using BitBoard = typename Game::BitBoard;
    using Pieces = typename Game::Pieces;
    using Squares = typename Game::Squares;

    const auto turn = game.get_turn();
    const auto& king_sq = game.get_king_location(turn);
    const auto& checker_sq = game.get_checker_location(0);

    append_legal_moves_by_non_king_moving_to(out, game, checker_sq);
    if (Squares::is_neighbor(king_sq, checker_sq))
        return;

    const auto dir = Squares::get_direction(checker_sq, king_sq);
    auto ptr_dst = Squares::get_squares_along(dir, king_sq);
    for (; *ptr_dst != checker_sq; ++ptr_dst) {
        append_legal_moves_by_non_king_moving_to(out, game, *ptr_dst);
        append_legal_moves_defending_king_dropping_to(out, game, *ptr_dst);
    }
}

template <class Game>
std::vector<typename Game::Move> get_legal_moves(const Game& game)
{
    std::vector<typename Game::Move> out;
    append_legal_moves_by_king(out, game);
    if (!game.in_check()) {
        append_legal_moves_by_drop(out, game);
        append_legal_moves_by_non_king(out, game);
    } else if (!game.in_double_check()) {
        append_legal_moves_to_defend_king(out, game);
    }
    return out;
}

template <class Game>
void append_check_moves_moving_to(
    std::vector<typename Game::Move>& out,
    const Game& game,
    const typename Game::SquareEnum& dst)
{
    using BitBoard = typename Game::BitBoard;
    using Move = typename Game::Move;
    using Pieces = typename Game::Pieces;
    using Squares = typename Game::Squares;

    const auto turn = game.get_turn();
    const auto& board = game.get_board();
    const auto& king_location = game.get_king_location(turn);
    const auto& enemy_king_sq = game.get_king_location(~turn);
    const auto& enemy_mask = game.get_occupied(~turn);
    const auto& occupied = game.get_occupied();
    const auto promotable_dst = Squares::in_promotion_zone(dst, turn);

    for (auto&& dir : Squares::direction_array) {
        auto ptr_src = Squares::get_squares_along(dir, dst);
        for (; *ptr_src != Squares::SQ_NA; ++ptr_src) {
            const auto& src = *ptr_src;
            if (!occupied.is_one(src)) // vacant
                continue;
            if (enemy_mask.is_one(src) || (src == king_location))
                break;
            const auto& p = board[src];
            if (!BitBoard::get_attacks_by(p, src).is_one(dst))
                break;
            const auto src_dir = Squares::get_direction(src, king_location);
            if ((src_dir != DIR_NA)
                && (src_dir != Squares::get_direction(dst, king_location))
                && (board.find_attacker(~turn, king_location, src_dir, src)
                    != Squares::SQ_NA))
                break;
            if (BitBoard::get_attacks_by(p, dst).is_one(enemy_king_sq))
                out.emplace_back(Move(dst, src, false));
            if (Pieces::is_promotable(p)
                && (promotable_dst || Squares::in_promotion_zone(src, turn))
                && BitBoard::get_attacks_by(Pieces::promote(p), dst)
                       .is_one(enemy_king_sq))
                out.emplace_back(Move(dst, src, true));
            break;
        }
    }
}

template <class Game>
void append_counter_check_moves_dropping_to(
    std::vector<typename Game::Move>& out,
    const Game& game,
    const typename Game::SquareEnum& dst)
{
    using BitBoard = typename Game::BitBoard;
    using Pieces = typename Game::Pieces;
    using Move = typename Game::Move;
    using Squares = typename Game::Squares;

    const auto turn = game.get_turn();
    const auto& board = game.get_board();
    const auto& stand = game.get_stand(turn);
    const auto& enemy_king_sq = game.get_king_location(~turn);
    for (auto&& pt : Pieces::stand_piece_array) {
        if (!stand.exist(pt))
            continue;
        const auto p = Pieces::to_board_piece(turn, pt);
        const auto attacks = BitBoard::get_attacks_by(p, dst);
        if (!attacks.is_one(enemy_king_sq))
            continue;
        if ((pt == Pieces::FU)
            && board.has_pawn_in_file(turn, Squares::to_file(dst)))
            continue;
        out.emplace_back(Move(dst, pt));
    }
}

template <class Game>
std::vector<typename Game::Move> get_counter_check_moves(const Game& game)
{
    using Move = typename Game::Move;
    using Squares = typename Game::Squares;
    std::vector<Move> out;

    const auto turn = game.get_turn();
    const auto& king_sq = game.get_king_location(turn);
    const auto& checker_sq = game.get_checker_location(0);

    append_check_moves_moving_to(out, game, checker_sq);
    if (Squares::is_neighbor(king_sq, checker_sq))
        return out;

    const auto dir = Squares::get_direction(checker_sq, king_sq);
    auto ptr_dst = Squares::get_squares_along(dir, king_sq);
    for (; *ptr_dst != checker_sq; ++ptr_dst) {
        append_check_moves_moving_to(out, game, *ptr_dst);
        append_counter_check_moves_dropping_to(out, game, *ptr_dst);
    }
    return out;
}

template <class Game>
std::vector<typename Game::Move> get_check_moves(const Game& game)
{
    std::vector<typename Game::Move> out;
    if (game.in_double_check())
        return out;
    if (game.in_check())
        return get_counter_check_moves(game);

    append_check_moves_by_drop(out, game);
    append_check_moves_by_board(out, game);
    return out;
}

} // namespace vshogi

#endif // VSHOGI_COMMON_LEGALS_HPP
