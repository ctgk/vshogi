#ifndef VSHOGI_COMMON_MAGIC_HPP
#define VSHOGI_COMMON_MAGIC_HPP

#include "vshogi/common/bitboard.hpp"
#include "vshogi/common/squares.hpp"
#include "vshogi/common/utils.hpp"

namespace vshogi
{

template <class Config>
struct Magic
{
private:
    using BitBoardType = BitBoard<Config>;
    using SHelper = Squares<Config>;
    using Square = typename Config::Square;
    using UInt = typename Config::BaseTypeBitBoard;
    static constexpr uint num_squares = Config::num_squares;
    static constexpr uint num_files = Config::num_files;
    static constexpr uint num_ranks = Config::num_ranks;
    static constexpr uint magic_table_size = Config::magic_table_size;
    static constexpr uint log2_magic_table_size = Config::log2_magic_table_size;

    /**
     * @brief Mask to get all relevant occupancies along vertical directions.
     * @details See below for detail:
     * https://www.chessprogramming.org/Best_Magics_so_far#Legende
     */
    static const BitBoardType premask_vertical[num_squares];
    /**
     * @brief Mask to get all relevant occupancies along horizontal directions.
     * @details See below for detail:
     * https://www.chessprogramming.org/Best_Magics_so_far#Legende
     */
    static const BitBoardType premask_horizontal[num_squares];

    /**
     * @brief Mask to get all relevant occupancies along SW-NE directions.
     * @details See below for detail:
     * https://www.chessprogramming.org/Best_Magics_so_far#Legende
     */
    static const BitBoardType premask_sw_ne[num_squares];
    /**
     * @brief Mask to get all relevant occupancies along NW-SE directions.
     * @details See below for detail:
     * https://www.chessprogramming.org/Best_Magics_so_far#Legende
     */
    static const BitBoardType premask_nw_se[num_squares];

    static const std::uint32_t magic_number_vertical[num_squares];
    static const std::uint32_t magic_number_horizontal[num_squares];
    static const std::uint32_t magic_number_sw_ne[num_squares];
    static const std::uint32_t magic_number_nw_se[num_squares];

    static BitBoardType attack_table_vertical[num_squares][magic_table_size];
    static BitBoardType attack_table_horizontal[num_squares][magic_table_size];
    static BitBoardType attack_table_sw_ne[num_squares][magic_table_size];
    static BitBoardType attack_table_nw_se[num_squares][magic_table_size];

public:
    Magic() = delete;
    static void init_tables()
    {
        init_attack_table_vertical();
        init_attack_table_horizontal();
        init_attack_table_nw_se();
        init_attack_table_sw_ne();
    }
    static BitBoardType
    get_adjacent_attack(const Square& sq, BitBoardType occupied)
    {
        return get_vertical_attack(sq, occupied)
               | get_horizontal_attack(sq, occupied);
    }
    static BitBoardType
    get_diagonal_attack(const Square& sq, BitBoardType occupied)
    {
        return get_nwse_attack(sq, occupied) | get_swne_attack(sq, occupied);
    }
    static BitBoardType get_occupancy(
        const uint index, // 0 ~ (1<<num_relevant_squares - 1)
        const uint num_relevant_squares,
        const uint* const relevant_square_locations)
    {
        BitBoardType out{};
        for (uint ii = 0u; ii < num_relevant_squares; ++ii) {
            if (index & (1u << ii))
                out |= BitBoardType::from_square(
                    static_cast<Square>(relevant_square_locations[ii]));
        }
        return out;
    }
    static uint to_magic_table_index(
        BitBoardType relevant_occupancies, const std::uint32_t magic)
    {
        std::uint32_t product = 0u;
        if constexpr (sizeof(BitBoardType) == sizeof(std::uint32_t)) {
            // shifting 32-bit of 32-bit integer results in undefined behavior.
            product = relevant_occupancies.value() * magic;
        } else {
            while (relevant_occupancies.any()) {
                product
                    ^= static_cast<std::uint32_t>(relevant_occupancies.value())
                       * magic;

                //// Note: Shifting 32-bit fails to differentiate masks between
                //// bb_9g(=54) and bb_5c(=22=54-32), which both of them lie
                //// along a diagonal direction from SQ_3A.
                // relevant_occupancies >>= 32u;

                relevant_occupancies >>= 31u;
            }
        }
        return product >> (32u - log2_magic_table_size);
    }

private:
    static void init_attack_table_vertical()
    {
        for (auto sq : EnumIterator<Square, num_squares>()) {
            const BitBoardType& premask = premask_vertical[sq];
            const uint num_relevant_squares = premask.hamming_weight();
            const std::uint32_t magic = magic_number_vertical[sq];

            uint relevant_square_locations[num_squares] = {0};
            for (uint ii = 0, jj = 0; ii < num_squares; ++ii) {
                if (premask.is_one(static_cast<Square>(ii)))
                    relevant_square_locations[jj++] = ii;
            }
            for (uint ii = (1u << num_relevant_squares); ii--;) {
                const BitBoardType occ = get_occupancy(
                    ii, num_relevant_squares, relevant_square_locations);
                const BitBoardType attack
                    = BitBoardType::compute_ray_to(sq, DIR_N, occ)
                      | BitBoardType::compute_ray_to(sq, DIR_S, occ);
                const auto index = to_magic_table_index(occ, magic);
                attack_table_vertical[sq][index] = attack;
            }
        }
    }
    static void init_attack_table_horizontal()
    {
        for (auto sq : EnumIterator<Square, num_squares>()) {
            const BitBoardType& premask = premask_horizontal[sq];
            const uint num_relevant_squares = premask.hamming_weight();
            const std::uint32_t magic = magic_number_horizontal[sq];

            uint relevant_square_locations[num_squares] = {0};
            for (uint ii = 0, jj = 0; ii < num_squares; ++ii) {
                if (premask.is_one(static_cast<Square>(ii)))
                    relevant_square_locations[jj++] = ii;
            }
            for (uint ii = (1u << num_relevant_squares); ii--;) {
                const BitBoardType occ = get_occupancy(
                    ii, num_relevant_squares, relevant_square_locations);
                const BitBoardType attack
                    = BitBoardType::compute_ray_to(sq, DIR_W, occ)
                      | BitBoardType::compute_ray_to(sq, DIR_E, occ);
                const auto index = to_magic_table_index(occ, magic);
                attack_table_horizontal[sq][index] = attack;
            }
        }
    }
    static void init_attack_table_nw_se()
    {
        for (auto sq : EnumIterator<Square, num_squares>()) {
            const BitBoardType& premask = premask_nw_se[sq];
            const uint num_relevant_squares = premask.hamming_weight();
            const std::uint32_t magic = magic_number_nw_se[sq];

            uint relevant_square_locations[num_squares] = {0};
            for (uint ii = 0, jj = 0; ii < num_squares; ++ii) {
                if (premask.is_one(static_cast<Square>(ii)))
                    relevant_square_locations[jj++] = ii;
            }
            for (uint ii = (1u << num_relevant_squares); ii--;) {
                const BitBoardType occ = get_occupancy(
                    ii, num_relevant_squares, relevant_square_locations);
                const BitBoardType attack
                    = BitBoardType::compute_ray_to(sq, DIR_NW, occ)
                      | BitBoardType::compute_ray_to(sq, DIR_SE, occ);
                const auto index = to_magic_table_index(occ, magic);
                attack_table_nw_se[sq][index] = attack;
            }
        }
    }
    static void init_attack_table_sw_ne()
    {
        for (auto sq : EnumIterator<Square, num_squares>()) {
            const BitBoardType& premask = premask_sw_ne[sq];
            const uint num_relevant_squares = premask.hamming_weight();
            const std::uint32_t magic = magic_number_sw_ne[sq];

            uint relevant_square_locations[num_squares] = {0};
            for (uint ii = 0, jj = 0; ii < num_squares; ++ii) {
                if (premask.is_one(static_cast<Square>(ii)))
                    relevant_square_locations[jj++] = ii;
            }
            for (uint ii = (1u << num_relevant_squares); ii--;) {
                const BitBoardType occ = get_occupancy(
                    ii, num_relevant_squares, relevant_square_locations);
                const BitBoardType attack
                    = BitBoardType::compute_ray_to(sq, DIR_SW, occ)
                      | BitBoardType::compute_ray_to(sq, DIR_NE, occ);
                const auto index = to_magic_table_index(occ, magic);
                attack_table_sw_ne[sq][index] = attack;
            }
        }
    }
    static BitBoardType
    get_vertical_attack(const Square& sq, BitBoardType occupied)
    {
        const std::uint32_t magic = magic_number_vertical[sq];
        occupied &= premask_vertical[sq];
        const auto index = to_magic_table_index(occupied, magic);
        return attack_table_vertical[sq][index];
    }
    static BitBoardType
    get_horizontal_attack(const Square& sq, BitBoardType occupied)
    {
        const std::uint32_t magic = magic_number_horizontal[sq];
        occupied &= premask_horizontal[sq];
        const auto index = to_magic_table_index(occupied, magic);
        return attack_table_horizontal[sq][index];
    }
    static BitBoardType get_swne_attack(const Square& sq, BitBoardType occupied)
    {
        const std::uint32_t magic = magic_number_sw_ne[sq];
        occupied &= premask_sw_ne[sq];
        const auto index = to_magic_table_index(occupied, magic);
        return attack_table_sw_ne[sq][index];
    }
    static BitBoardType get_nwse_attack(const Square& sq, BitBoardType occupied)
    {
        const std::uint32_t magic = magic_number_nw_se[sq];
        occupied &= premask_nw_se[sq];
        const auto index = to_magic_table_index(occupied, magic);
        return attack_table_nw_se[sq][index];
    }
};

} // namespace vshogi

#endif // VSHOGI_COMMON_MAGIC_HPP
