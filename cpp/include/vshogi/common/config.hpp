#ifndef VSHOGI_COMMON_CONFIG_HPP
#define VSHOGI_COMMON_CONFIG_HPP

#include <array>

#include "vshogi/common/color.hpp"
#include "vshogi/common/direction.hpp"
#include "vshogi/common/utils.hpp"

namespace vshogi
{

struct FPTHelper
{
private:
    static const DirectionEnum attack_directions_table[29][9];

public:
    FPTHelper() = delete;

    static constexpr FullPieceTypes to_fpt(char c)
    {
        c = static_cast<char>(std::tolower(static_cast<int>(c)));
        switch (c) {
        case 'p':
            return PT_FU;
        case 'l':
            return PT_KY;
        case 'n':
            return PT_KE;
        case 's':
            return PT_GI;
        case 'b':
            return PT_KA;
        case 'r':
            return PT_HI;
        case 'g':
            return PT_KI;
        case 'k':
            return PT_OU;
        default:
            break;
        }
        return PT_NA;
    }
    static constexpr bool is_promotable(const FullPieceTypes pt)
    {
        return pt < PT_KI;
    }
    static constexpr bool is_promoted(const FullPieceTypes pt)
    {
        return pt > PT_OU;
    }
    static constexpr bool is_promotion_fully_superior(const FullPieceTypes pt)
    {
        return (pt == PT_FU) || (pt == PT_KA) || (pt == PT_HI);
    }
    static constexpr uint to_point(const FullPieceTypes pt)
    {
        constexpr uint table[] = {
            1, //!< Fu (Pawn)
            1, //!< Kyo (Lance)
            1, //!< Kei (Knight)
            1, //!< Gin (Silver)
            5, //!< Kaku (Bishop)
            5, //!< Hisha (Rook)
            1, //!< Kin (Gold)
            0, //!< Ou, Gyoku (King)
            1, //!< Tokin (Promoted Pawn)
            1, //!< Nari-Kyo (Promoted Lance)
            1, //!< Nari-Kei (Promoted Knight)
            1, //!< Nari-Gin (Promoted Silver)
            5, //!< Uma (Promoted Bishop)
            5, //!< Ryu (Promoted Rook)
            0, //!< NA
        };
        return table[pt];
    }

    /**
     * @brief Get valuation of a piece type.
     * @ref https://shogi.zukeran.org/2018/06/12/piece-value-1/
     *
     * @param pt Input
     * @return constexpr uint valuation of the input
     */
    static constexpr uint to_value(const FullPieceTypes pt)
    {
        constexpr uint table[] = {
            5, //!< Fu (Pawn)
            30, //!< Kyo (Lance)
            35, //!< Kei (Knight)
            55, //!< Gin (Silver)
            95, //!< Kaku (Bishop)
            100, //!< Hisha (Rook)
            60, //!< Kin (Gold)
            0, //!< Ou, Gyoku (King)
            60, //!< Tokin (Promoted Pawn)
            60, //!< Nari-Kyo (Promoted Lance)
            60, //!< Nari-Kei (Promoted Knight)
            60, //!< Nari-Gin (Promoted Silver)
            115, //!< Uma (Promoted Bishop)
            120, //!< Ryu (Promoted Rook)
            0, //!< NA
        };
        return table[pt];
    }

    static const DirectionEnum*
    get_attack_directions(const FullPieceTypes pt, const ColorEnum c)
    {
        if (pt == PT_NA)
            return attack_directions_table[28];
        return attack_directions_table[pt + static_cast<uint>(c) * 14u];
    }
    static bool is_slider(const FullPieceTypes pt)
    {
        constexpr bool table[] = {
            false, //!< Fu (Pawn)
            true, //!< Kyo (Lance)
            false, //!< Kei (Knight)
            false, //!< Gin (Silver)
            true, //!< Kaku (Bishop)
            true, //!< Hisha (Rook)
            false, //!< Kin (Gold)
            false, //!< Ou, Gyoku (King)
            false, //!< Tokin (Promoted Pawn)
            false, //!< Nari-Kyo (Promoted Lance)
            false, //!< Nari-Kei (Promoted Knight)
            false, //!< Nari-Gin (Promoted Silver)
            true, //!< Uma (Promoted Bishop)
            true, //!< Ryu (Promoted Rook)
            false, //!< NA
        };
        return table[pt];
    }
    static bool slidable_to(const FullPieceTypes pt, const DirectionEnum d)
    {
        switch (pt) {
        case PT_KY:
            return (d == DIR_N);
        case PT_KA:
        case PT_UM:
            return (d == DIR_NW) || (d == DIR_NE) || (d == DIR_SW)
                   || (d == DIR_SE);
        case PT_HI:
        case PT_RY:
            return (d == DIR_N) || (d == DIR_W) || (d == DIR_E) || (d == DIR_S);
        default:
            break;
        }
        return false;
    }
    static bool is_attacking_to(const FullPieceTypes pt, const DirectionEnum d)
    {
        switch (d) {
        case DIR_N:
            return (pt != PT_KE) && (pt != PT_KA);
        case DIR_NW:
        case DIR_NE:
            return !((pt < PT_GI) || (pt == PT_HI)); // FU, KY, KE, HI
        case DIR_W:
        case DIR_E:
        case DIR_S:
            return (pt > PT_KA) && (pt != PT_NA); // fu, ky, ke, gi, ka
        case DIR_SW:
        case DIR_SE:
            return (pt == PT_GI) || (pt == PT_KA) || (pt == PT_OU)
                   || (pt == PT_UM) || (pt == PT_RY);
        case DIR_NNW:
        case DIR_NNE:
            return (pt == PT_KE);

        default:
            break;
        }
        return false;
    }
};

inline const DirectionEnum FPTHelper::attack_directions_table[29][9] = {
    // clang-format off
    {DIR_N,                                                      DIR_NA}, // B_FU
    {DIR_N,                                                      DIR_NA}, // B_KY
    {DIR_NNW, DIR_NNE,                                           DIR_NA}, // B_KE
    {DIR_NW, DIR_N, DIR_NE, DIR_SW, DIR_SE,                      DIR_NA}, // B_GI
    {DIR_NW, DIR_NE, DIR_SW, DIR_SE,                             DIR_NA}, // B_KA
    {DIR_N, DIR_W, DIR_E, DIR_S,                                 DIR_NA}, // B_HI
    {DIR_NW, DIR_N, DIR_NE, DIR_W, DIR_E, DIR_S,                 DIR_NA}, // B_KI
    {DIR_NW, DIR_N, DIR_NE, DIR_W, DIR_E, DIR_SW, DIR_S, DIR_SE, DIR_NA}, // B_OU
    {DIR_NW, DIR_N, DIR_NE, DIR_W, DIR_E, DIR_S,                 DIR_NA}, // B_TO
    {DIR_NW, DIR_N, DIR_NE, DIR_W, DIR_E, DIR_S,                 DIR_NA}, // B_NY
    {DIR_NW, DIR_N, DIR_NE, DIR_W, DIR_E, DIR_S,                 DIR_NA}, // B_NK
    {DIR_NW, DIR_N, DIR_NE, DIR_W, DIR_E, DIR_S,                 DIR_NA}, // B_NG
    {DIR_NW, DIR_N, DIR_NE, DIR_W, DIR_E, DIR_SW, DIR_S, DIR_SE, DIR_NA}, // B_UM
    {DIR_NW, DIR_N, DIR_NE, DIR_W, DIR_E, DIR_SW, DIR_S, DIR_SE, DIR_NA}, // B_RY
    {DIR_S,                                                      DIR_NA}, // W_FU
    {DIR_S,                                                      DIR_NA}, // W_KY
    {DIR_SSE, DIR_SSW,                                           DIR_NA}, // W_KE
    {DIR_SE, DIR_S, DIR_SW, DIR_NE, DIR_NW,                      DIR_NA}, // W_GI
    {DIR_SE, DIR_SW, DIR_NE, DIR_NW,                             DIR_NA}, // W_KA
    {DIR_S, DIR_E, DIR_W, DIR_N,                                 DIR_NA}, // W_HI
    {DIR_SE, DIR_S, DIR_SW, DIR_E, DIR_W, DIR_N,                 DIR_NA}, // W_KI
    {DIR_SE, DIR_S, DIR_SW, DIR_E, DIR_W, DIR_NE, DIR_N, DIR_NW, DIR_NA}, // W_OU
    {DIR_SE, DIR_S, DIR_SW, DIR_E, DIR_W, DIR_N,                 DIR_NA}, // W_TO
    {DIR_SE, DIR_S, DIR_SW, DIR_E, DIR_W, DIR_N,                 DIR_NA}, // W_NY
    {DIR_SE, DIR_S, DIR_SW, DIR_E, DIR_W, DIR_N,                 DIR_NA}, // W_NK
    {DIR_SE, DIR_S, DIR_SW, DIR_E, DIR_W, DIR_N,                 DIR_NA}, // W_NG
    {DIR_SE, DIR_S, DIR_SW, DIR_E, DIR_W, DIR_NE, DIR_N, DIR_NW, DIR_NA}, // W_UM
    {DIR_SE, DIR_S, DIR_SW, DIR_E, DIR_W, DIR_NE, DIR_N, DIR_NW, DIR_NA}, // W_RY
    {                                                            DIR_NA}, // NA
    // clang-format on
};

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
    using bitboard_t = typename ParamS::BaseTypeBitBoard;
    using BaseTypeStand = typename ParamS::BaseTypeStand;
    static_assert(sizeof(Piece) == sizeof(std::uint8_t));

private:
    static constexpr uint compute_num_init_piece_each();
    static constexpr uint compute_initial_points();
    static constexpr uint compute_sum_piece_value();
    static constexpr std::array<uint, Param::piece_types.size()>
    compute_initial_piece_count();
    static constexpr uint compute_max_stand_piece_count();
    static constexpr uint compute_num_dir();

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
    static constexpr uint num_dir = compute_num_dir();

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
    static constexpr uint max_stand_piece_count
        = compute_max_stand_piece_count();
    static constexpr uint max_stand_sfen_length = Param::max_stand_sfen_length;

public: // game rules
    /**
     * @brief Threshold for declaring a game end due to n-fold repetition.
     * E.g. 4 in minishogi.
     */
    static constexpr uint nfold_repetitions = Param::nfold_repetitions;

    /**
     * @brief Array of initial piece count in the game.
     * E.g. {2(FU), 2(GI), 2(KA), 2(HI), 2(KI), 2(OU), 0(TO), 0(NG), 0(UM), 0(RY), 0(NA)}
     */
    static constexpr std::array<uint, Param::piece_types.size()>
        initial_piece_count = compute_initial_piece_count();

    static constexpr uint num_init_piece_each = compute_num_init_piece_each();
    static constexpr uint half_num_init_piece_each = num_init_piece_each / 2u;
    static constexpr uint initial_points = compute_initial_points();
    static constexpr uint sum_piece_value = compute_sum_piece_value();
    static constexpr std::array<Piece, num_squares> initial_position
        = Param::initial_position;
    static_assert(2u * num_piece_types == static_cast<uint>(VOID));
    static_assert(num_init_piece_each > 0u);
    static_assert(initial_points > 0u);

    static constexpr uint dlshogi_policy_size
        = num_squares * (2 * num_dir_dl + num_stand_piece_types);

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

template <class Param>
constexpr uint Configuration<Param>::compute_num_init_piece_each()
{
    uint out = 0u;
    for (const auto piece : Param::initial_position) {
        const uint piece_index = static_cast<uint>(piece);
        if (piece_index < num_piece_types) {
            ++out;
        }
    }
    return out;
}

template <class Param>
constexpr uint Configuration<Param>::compute_initial_points()
{
    uint out = 0u;
    for (const auto piece : Param::initial_position) {
        const uint piece_index = static_cast<uint>(piece);
        if (piece_index < num_piece_types) {
            out += FPTHelper::to_point(Param::piece_types[piece_index]);
        }
    }
    return out;
}

template <class Param>
constexpr uint Configuration<Param>::compute_sum_piece_value()
{
    uint out = 0u;
    constexpr uint promoted_offset = static_cast<uint>(PT_TO);
    for (const auto piece : Param::initial_position) {
        const uint piece_index = static_cast<uint>(piece);
        if (piece_index < static_cast<uint>(VOID)) {
            const uint piece_type_index = piece_index % num_piece_types;
            const auto fpt = Param::piece_types[piece_type_index];
            if (FPTHelper::is_promotable(fpt)) {
                out += FPTHelper::to_value(
                    static_cast<FullPieceTypes>(
                        static_cast<uint>(fpt) + promoted_offset));
            } else {
                out += FPTHelper::to_value(fpt);
            }
        }
    }
    return out;
}

template <class Param>
constexpr std::array<uint, Param::piece_types.size()>
Configuration<Param>::compute_initial_piece_count()
{
    std::array<uint, Param::piece_types.size()> out = {};
    for (const auto piece : Param::initial_position) {
        const uint piece_index = static_cast<uint>(piece);
        if (piece_index != static_cast<uint>(VOID)) {
            const uint piece_type_index = piece_index % num_piece_types;
            if (piece_type_index < num_piece_types) {
                ++out[piece_type_index];
            }
        }
    }
    return out;
}

template <class Param>
constexpr uint Configuration<Param>::compute_max_stand_piece_count()
{
    const auto counts = compute_initial_piece_count();
    uint max_count = 0u;
    for (uint i = 0u; i < num_stand_piece_types; ++i) {
        if (counts[i] > max_count) {
            max_count = counts[i];
        }
    }
    return max_count;
}

template <class Param>
constexpr uint Configuration<Param>::compute_num_dir()
{
    // Check if knight (PT_KE) exists in piece_types
    for (const auto pt : Param::piece_types) {
        if (pt == PT_KE) {
            return 12u; // Knight requires 12 directions (includes NNW, NNE)
        }
    }
    return 8u; // Standard 8 directions without knight
}

} // namespace vshogi

#endif // VSHOGI_COMMON_CONFIG_HPP
