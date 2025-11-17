#ifndef VSHOGI_COMMON_CONFIG_HPP
#define VSHOGI_COMMON_CONFIG_HPP

#include <array>

#include "vshogi/common/direction.hpp"
#include "vshogi/common/utils.hpp"

namespace vshogi
{

template <class ParamS>
struct Configuration
{
    Configuration() = delete;
    using Param = ParamS;
    using PieceType = typename Param::PieceType;
    using Piece = typename Param::Piece;
    using Square = typename Param::Square;
    using File = typename Param::File;
    using Rank = typename Param::Rank;
    using BaseTypeBitBoard = typename ParamS::BaseTypeBitBoard;
    using BaseTypeStand = typename ParamS::BaseTypeStand;
    static_assert(sizeof(Piece) == sizeof(std::uint8_t));

public: // pieces
    /**
     * @brief Number of piece types.
     * E.g. 10 (p, s, b, r, g, k, +p, +s, +b, +r) in minishogi.
     * @note Differentiate promotions but not colored.
     */
    static constexpr uint num_piece_types = Param::piece_types.size() - 1u;

    /**
     * @brief Array of piece types appears in the game.
     * E.g. {FU, GI, KA, HI, KI, OU, TO, NG, UM, RY, NA} in minishogi.
     */
    static constexpr std::array<FullPieceTypes, Param::piece_types.size()>
        piece_types = Param::piece_types;
    static_assert(piece_types[num_piece_types] == PT_NA);

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

    static constexpr PieceType FU = static_cast<PieceType>(0); // NOLINT
    static_assert(piece_types[0u] == PT_FU);
    static constexpr PieceType OU // NOLINT
        = static_cast<PieceType>(num_stand_piece_types);
    static_assert(piece_types[num_stand_piece_types] == PT_OU);
    static constexpr PieceType NA // NOLINT
        = static_cast<PieceType>(num_piece_types);
    static_assert(piece_types[num_piece_types] == PT_NA);
    static constexpr Piece VOID // NOLINT
        = static_cast<Piece>(num_colored_piece_types);

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
     * @brief Longer length of the board.
     * E.g. 5 in minishogi.
     */
    static constexpr uint board_length = std::max(num_files, num_ranks);

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
    static constexpr uint num_dir_dl = num_dir - (num_dir > 8) * 2u;

    static constexpr Square SQ_NA = static_cast<Square>(num_squares); // NOLINT
    static constexpr Rank RANK_A = static_cast<Rank>(0); // NOLINT
    static constexpr Rank RANK_B = static_cast<Rank>(1); // NOLINT
    static constexpr Rank RANK_Y = static_cast<Rank>(num_ranks - 2u); // NOLINT
    static constexpr Rank RANK_Z = static_cast<Rank>(num_ranks - 1u); // NOLINT

public: // stand
    static constexpr uint max_stand_piece_count = Param::max_stand_piece_count;
    static constexpr uint max_stand_sfen_length = Param::max_stand_sfen_length;

public: // game rules
    /**
     * @brief Maximum # of repetitions accepted in the game rule.
     * E.g. 3 in minishogi.
     */
    static constexpr uint max_acceptable_repetitions
        = Param::max_acceptable_repetitions;

    /**
     * @brief Array of initial piece count in the game.
     * E.g. {2(FU), 2(GI), 2(KA), 2(HI), 2(KI), 2(OU), 0(TO), 0(NG), 0(UM), 0(RY), 0(NA)}
     */
    static constexpr std::array<uint, Param::piece_types.size()>
        initial_piece_count = Param::initial_piece_count;

    static constexpr uint num_init_piece_each = Param::num_init_piece_each;
    static constexpr uint half_num_init_piece_each = num_init_piece_each / 2u;
    static constexpr uint initial_points = Param::initial_points;
    static constexpr uint sum_piece_value = Param::sum_piece_value;
    static constexpr std::array<Piece, num_squares> initial_position
        = Param::initial_position;

public:
    template <class ContiguousEnum, uint End>
    class EnumIteratorContainer
    {
    public:
        EnumIteratorContainer()
        {
        }
        EnumIterator<ContiguousEnum, End> begin()
        {
            return EnumIterator<ContiguousEnum, End>();
        }
        EnumIterator<ContiguousEnum, End> end()
        {
            return EnumIterator<ContiguousEnum, End>(End);
        }
    };
    static auto direction_iterator()
    {
        return EnumIteratorContainer<DirectionEnum, num_dir>();
    }
    static auto square_iterator()
    {
        return EnumIteratorContainer<Square, num_squares>();
    }
    static auto piece_type_iterator()
    {
        return EnumIteratorContainer<PieceType, num_piece_types>();
    }
    static auto stand_piece_type_iterator()
    {
        return EnumIteratorContainer<PieceType, num_stand_piece_types>();
    }
};

} // namespace vshogi

#endif // VSHOGI_COMMON_CONFIG_HPP
