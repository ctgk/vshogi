#ifndef VSHOGI_COMMON_CONFIG_HPP
#define VSHOGI_COMMON_CONFIG_HPP

#include <array>

#include "vshogi/common/utils.hpp"

namespace vshogi
{

template <class ParamS>
struct Configuration
{
    Configuration() = delete;
    using Param = ParamS;
    using PieceType = typename Param::PieceType;
    using ColoredPiece = typename Param::ColoredPiece;
    using Square = typename Param::Square;
    using File = typename Param::File;
    using Rank = typename Param::Rank;
    using BaseTypeBitBoard = typename ParamS::BaseTypeBitBoard;
    using BaseTypeStand = typename ParamS::BaseTypeStand;

public: // pieces
    /**
     * @brief Number of piece types.
     * E.g. 10 (p, s, b, r, g, k, +p, +s, +b, +r) in minishogi.
     * @note Differentiate promotions but not colored.
     */
    static constexpr uint num_piece_types = Param::num_piece_types;

    /**
     * @brief Number of colored piece types.
     * E.g. 20 (p, s, b, r, g, k, +p, +s, +b, +r, P, S, ...) in minishogi.
     * @note `num_colored_piece_types` == 2 * `num_piece_types`
     */
    static constexpr uint num_colored_piece_types = 2 * num_piece_types;

    /**
     * @brief Number of piece types that can exist in players' stand.
     * E.g. 5 (p, s, b, r, g) in minishogi.
     * @note `num_stand_piece_types` + #promotions + 1(OU) == `num_piece_types`
     */
    static constexpr uint num_stand_piece_types = Param::num_stand_piece_types;

public: // squares
    /**
     * @brief Number of files in the board.
     * E.g. 5 (1, 2, 3, 4, 5) in minishogi.
     */
    static constexpr uint num_files = Param::num_files;

    /**
     * @brief Number of ranks in the board.
     * E.g. 5 (A, B, C, D, E) in minishogi.
     */
    static constexpr uint num_ranks = Param::num_ranks;

    /**
     * @brief Number of squares in the board.
     * E.g. 25 (SQ_1A, SQ_1B, ..., SQ_5E) in minishogi.
     */
    static constexpr uint num_squares = num_files * num_ranks;

    /**
     * @brief Number of ranks where pieces may promote.
     * E.g. 1 (A for BLACK, E for WHITE) in minishogi
     */
    static constexpr uint num_promotion_ranks = Param::num_promotion_ranks;

    /**
     * @brief Number of directions of piece attacks by both players.
     * E.g. 12 (NNW, NNE, NW, N, NE, W, E, SW, S, SE, SSW, SSE) in shogi.
     */
    static constexpr uint num_dir = Param::num_dir;

    /**
     * @brief Number of direction of piece attacks by turn player.
     * E.g. 10 (NNW, NNE, NW, N, NE, W, E, SW, S, SE) in shogi.
     */
    static constexpr uint num_dir_dl = Param::num_dir_dl;

public: // stand
    static constexpr uint max_stand_piece_count = Param::max_stand_piece_count;
    static constexpr uint max_stand_sfen_length = Param::max_stand_sfen_length;

public: // magic
    static constexpr uint log2_magic_table_size = Param::log2_magic_table_size;
    static constexpr uint magic_table_size = 1u << log2_magic_table_size;

public: // game rules
    /**
     * @brief Maximum # of repetitions accepted in the game rule.
     * E.g. 3 in minishogi.
     */
    static constexpr uint max_acceptable_repetitions = 3;

    static constexpr uint num_init_piece_each = Param::num_init_piece_each;
    static constexpr uint half_num_init_piece_each = num_init_piece_each / 2u;
    static constexpr uint initial_points = Param::initial_points;
    static constexpr uint sum_piece_value = Param::sum_piece_value;
};

} // namespace vshogi

#endif // VSHOGI_COMMON_CONFIG_HPP
