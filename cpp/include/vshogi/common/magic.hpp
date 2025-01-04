#ifndef VSHOGI_COMMON_MAGIC_HPP
#define VSHOGI_COMMON_MAGIC_HPP

#include "vshogi/common/bitboard.hpp"
#include "vshogi/common/config.hpp"
#include "vshogi/common/squares.hpp"
#include "vshogi/common/utils.hpp"

namespace vshogi
{

template <class Parameters>
struct Magic
{
private:
    using C = Configuration<Parameters>;
    using BitBoardType = BitBoard<Parameters>;
    using SHelper = Squares<Parameters>;
    using Square = typename C::Square;
    using UInt = typename C::BaseTypeBitBoard;

public:
    static constexpr uint log2_table_size_lance = C::num_ranks - 2u;
    static constexpr uint log2_table_size_diagonal
        = (C::num_ranks + C::num_files - 6u);
    static constexpr uint log2_table_size_adjacent
        = (C::num_ranks + C::num_files - 4u);
    static constexpr uint table_size_lance = 1u << log2_table_size_lance;
    static constexpr uint table_size_diagonal = 1u << log2_table_size_diagonal;
    static constexpr uint table_size_adjacent = 1u << log2_table_size_adjacent;

private:
    static const UInt premask_north[C::num_squares];
    static const UInt premask_south[C::num_squares];

    /**
     * @brief Mask to get all relevant occupancies along adjacent directions.
     * @details See below for detail:
     * https://www.chessprogramming.org/Best_Magics_so_far#Legende
     */
    static const UInt premask_adjacent[C::num_squares];

    /**
     * @brief Mask to get all relevant occupancies along diagonal directions.
     * @details See below for detail:
     * https://www.chessprogramming.org/Best_Magics_so_far#Legende
     */
    static const UInt premask_diagonal[C::num_squares];

    static const std::uint32_t magic_number_adjacent[C::num_squares];
    static const std::uint32_t magic_number_diagonal[C::num_squares];
    static const std::uint32_t magic_number_north[C::num_squares];
    static const std::uint32_t magic_number_south[C::num_squares];

    static BitBoardType attack_table_adjacent[C::num_squares]
                                             [table_size_adjacent];
    static BitBoardType attack_table_diagonal[C::num_squares]
                                             [table_size_diagonal];
    static BitBoardType attack_table_north[C::num_squares][table_size_lance];
    static BitBoardType attack_table_south[C::num_squares][table_size_lance];

public:
    Magic() = delete;
    static void init_tables()
    {
        init_attack_table_adjacent();
        init_attack_table_diagonal();
        if constexpr (C::num_squares > 80) {
            init_attack_table_north();
            init_attack_table_south();
        }
    }
    static BitBoardType
    get_north_attack(const Square& sq, const BitBoardType& occupied)
    {
        const std::uint32_t magic = magic_number_north[sq];
        const auto index = to_magic_table_index<log2_table_size_lance>(
            occupied.value() & premask_north[sq], magic);
        assert(index < table_size_lance);
        return attack_table_north[sq][index];
    }
    static BitBoardType
    get_south_attack(const Square& sq, const BitBoardType& occupied)
    {
        const std::uint32_t magic = magic_number_south[sq];
        const auto index = to_magic_table_index<log2_table_size_lance>(
            occupied.value() & premask_south[sq], magic);
        assert(index < table_size_lance);
        return attack_table_south[sq][index];
    }
    static BitBoardType get_adjacent_attack(const Square& sq)
    {
        return attack_table_adjacent[sq][0u];
    }
    static BitBoardType
    get_adjacent_attack(const Square& sq, const BitBoardType& occupied)
    {
        const std::uint32_t magic = magic_number_adjacent[sq];
        const auto index = to_magic_table_index<log2_table_size_adjacent>(
            occupied.value() & premask_adjacent[sq], magic);
        assert(index < table_size_adjacent);
        return attack_table_adjacent[sq][index];
    }
    static BitBoardType get_diagonal_attack(const Square& sq)
    {
        return attack_table_diagonal[sq][0u];
    }
    static BitBoardType
    get_diagonal_attack(const Square& sq, const BitBoardType& occupied)
    {
        const std::uint32_t magic = magic_number_diagonal[sq];
        const auto index = to_magic_table_index<log2_table_size_diagonal>(
            occupied.value() & premask_diagonal[sq], magic);
        assert(index < table_size_diagonal);
        return attack_table_diagonal[sq][index];
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
    template <uint Shift>
    static uint to_magic_table_index(UInt relevant_occ, std::uint32_t magic)
    {
        if constexpr (sizeof(UInt) == sizeof(std::uint32_t)) {
            // shifting 32-bit of 32-bit integer results in undefined behavior.
            std::uint32_t product = relevant_occ * magic;
            return product >> (32u - Shift);
        } else if constexpr (sizeof(UInt) == sizeof(std::uint64_t)) {
            std::uint32_t product
                = static_cast<std::uint32_t>(relevant_occ) * magic;
            product ^= static_cast<std::uint32_t>(relevant_occ >> 31u) * magic;
            return product >> (32u - Shift);
        } else if constexpr (sizeof(UInt) == sizeof(uint128)) {
            std::uint32_t product
                = static_cast<std::uint32_t>(relevant_occ) * magic;
            magic *= magic;
            product ^= static_cast<std::uint32_t>(relevant_occ >> 29u) * magic;
            magic *= magic;
            product ^= static_cast<std::uint32_t>(relevant_occ >> 58u) * magic;
            return product >> (32u - Shift);
        } else {
            std::uint32_t product = 0u;
            while (relevant_occ) {
                product ^= static_cast<std::uint32_t>(relevant_occ) * magic;

                // Note: Shifting 32-bit fails to differentiate masks between
                // bb_9g(=54) and bb_5c(=22=54-32), which both of them lie
                // along a diagonal direction from SQ_3A.
                // Possibly the same in major column board representation.
                // relevant_occ >>= 32u;
                relevant_occ >>= 31u;
            }
            return product >> (32u - Shift);
        }
    }

private:
    static void init_attack_table_north()
    {
        for (auto sq : EnumIterator<Square, C::num_squares>()) {
            const BitBoardType premask
                = BitBoardType::from_value(premask_north[sq]);
            const uint num_relevant_squares = premask.hamming_weight();
            const std::uint32_t magic = magic_number_north[sq];

            uint relevant_square_locations[C::num_squares] = {0};
            for (uint ii = 0, jj = 0; ii < C::num_squares; ++ii) {
                if (premask.is_one(static_cast<Square>(ii)))
                    relevant_square_locations[jj++] = ii;
            }
            for (uint ii = (1u << num_relevant_squares); ii--;) {
                const BitBoardType occ = get_occupancy(
                    ii, num_relevant_squares, relevant_square_locations);
                const BitBoardType attack
                    = BitBoardType::compute_ray_to(sq, DIR_N, occ);
                const auto index = to_magic_table_index<log2_table_size_lance>(
                    occ.value(), magic);
                assert(index < table_size_lance);
                attack_table_north[sq][index] = attack;
            }
        }
    }
    static void init_attack_table_south()
    {
        for (auto sq : EnumIterator<Square, C::num_squares>()) {
            const BitBoardType premask
                = BitBoardType::from_value(premask_south[sq]);
            const uint num_relevant_squares = premask.hamming_weight();
            const std::uint32_t magic = magic_number_south[sq];

            uint relevant_square_locations[C::num_squares] = {0};
            for (uint ii = 0, jj = 0; ii < C::num_squares; ++ii) {
                if (premask.is_one(static_cast<Square>(ii)))
                    relevant_square_locations[jj++] = ii;
            }
            for (uint ii = (1u << num_relevant_squares); ii--;) {
                const BitBoardType occ = get_occupancy(
                    ii, num_relevant_squares, relevant_square_locations);
                const BitBoardType attack
                    = BitBoardType::compute_ray_to(sq, DIR_S, occ);
                const auto index = to_magic_table_index<log2_table_size_lance>(
                    occ.value(), magic);
                assert(index < table_size_lance);
                attack_table_south[sq][index] = attack;
            }
        }
    }
    static void init_attack_table_adjacent()
    {
        for (auto sq : EnumIterator<Square, C::num_squares>()) {
            const BitBoardType premask
                = BitBoardType::from_value(premask_adjacent[sq]);
            const uint num_relevant_squares = premask.hamming_weight();
            const std::uint32_t magic = magic_number_adjacent[sq];

            uint relevant_square_locations[C::num_squares] = {0};
            for (uint ii = 0, jj = 0; ii < C::num_squares; ++ii) {
                if (premask.is_one(static_cast<Square>(ii)))
                    relevant_square_locations[jj++] = ii;
            }
            for (uint ii = (1u << num_relevant_squares); ii--;) {
                const BitBoardType occ = get_occupancy(
                    ii, num_relevant_squares, relevant_square_locations);
                const BitBoardType attack
                    = BitBoardType::compute_ray_to(sq, DIR_N, occ)
                      | BitBoardType::compute_ray_to(sq, DIR_W, occ)
                      | BitBoardType::compute_ray_to(sq, DIR_E, occ)
                      | BitBoardType::compute_ray_to(sq, DIR_S, occ);
                const auto index
                    = to_magic_table_index<log2_table_size_adjacent>(
                        occ.value(), magic);
                assert(index < table_size_adjacent);
                attack_table_adjacent[sq][index] = attack;
            }
        }
    }
    static void init_attack_table_diagonal()
    {
        for (auto sq : EnumIterator<Square, C::num_squares>()) {
            const BitBoardType premask
                = BitBoardType::from_value(premask_diagonal[sq]);
            const uint num_relevant_squares = premask.hamming_weight();
            const std::uint32_t magic = magic_number_diagonal[sq];

            uint relevant_square_locations[C::num_squares] = {0};
            for (uint ii = 0, jj = 0; ii < C::num_squares; ++ii) {
                if (premask.is_one(static_cast<Square>(ii)))
                    relevant_square_locations[jj++] = ii;
            }
            for (uint ii = (1u << num_relevant_squares); ii--;) {
                const BitBoardType occ = get_occupancy(
                    ii, num_relevant_squares, relevant_square_locations);
                const BitBoardType attack
                    = BitBoardType::compute_ray_to(sq, DIR_NW, occ)
                      | BitBoardType::compute_ray_to(sq, DIR_NE, occ)
                      | BitBoardType::compute_ray_to(sq, DIR_SW, occ)
                      | BitBoardType::compute_ray_to(sq, DIR_SE, occ);
                const auto index
                    = to_magic_table_index<log2_table_size_diagonal>(
                        occ.value(), magic);
                assert(index < table_size_diagonal);
                attack_table_diagonal[sq][index] = attack;
            }
        }
    }
};

} // namespace vshogi

#endif // VSHOGI_COMMON_MAGIC_HPP
