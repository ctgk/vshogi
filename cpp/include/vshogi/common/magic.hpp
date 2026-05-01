#ifndef VSHOGI_COMMON_MAGIC_HPP
#define VSHOGI_COMMON_MAGIC_HPP

#include "vshogi/common/bitboard_traits.hpp"
#include "vshogi/common/config.hpp"
#include "vshogi/common/square_traits.hpp"
#include "vshogi/common/utils.hpp"

extern "C" std::uint32_t rust_to_magic_table_index_u32(
    std::uint32_t relevant_occ, std::uint32_t magic, std::uint32_t shift);
extern "C" std::uint32_t rust_to_magic_table_index_u64(
    std::uint64_t relevant_occ, std::uint32_t magic, std::uint32_t shift);
extern "C" std::uint32_t rust_to_magic_table_index_u128(
    std::uint64_t high,
    std::uint64_t low,
    std::uint32_t magic,
    std::uint32_t shift);

namespace vshogi
{

template <class P>
struct Magic
{
private:
    using C = Configuration<P>;
    using BT = BitboardTraits<P>;
    using ST = SquareTraits<P>;
    using Square = typename C::Square;
    using bitboard_t = typename C::bitboard_t;

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
    static const bitboard_t premask_north[C::num_squares];
    static const bitboard_t premask_south[C::num_squares];

    /**
     * @brief Mask to get all relevant occupancies along adjacent directions.
     * @details See below for detail:
     * https://www.chessprogramming.org/Best_Magics_so_far#Legende
     */
    static const bitboard_t premask_adjacent[C::num_squares];

    /**
     * @brief Mask to get all relevant occupancies along diagonal directions.
     * @details See below for detail:
     * https://www.chessprogramming.org/Best_Magics_so_far#Legende
     */
    static const bitboard_t premask_diagonal[C::num_squares];

    static const std::uint32_t magic_number_adjacent[C::num_squares];
    static const std::uint32_t magic_number_diagonal[C::num_squares];
    static const std::uint32_t magic_number_north[C::num_squares];
    static const std::uint32_t magic_number_south[C::num_squares];

    static bitboard_t attack_table_adjacent[C::num_squares]
                                           [table_size_adjacent];
    static bitboard_t attack_table_diagonal[C::num_squares]
                                           [table_size_diagonal];
    static bitboard_t attack_table_north[C::num_squares][table_size_lance];
    static bitboard_t attack_table_south[C::num_squares][table_size_lance];

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
    static bitboard_t compute_ray_to(
        const Square& src, const DirectionEnum& dir, const bitboard_t& occ)
    {
        bitboard_t out{};
        for (auto s = ST::ray_from(src, dir); *s != C::SQ_NA; ++s) {
            out ^= BT::from_square(*s);
            if (BT::is_one(occ, *s))
                return out;
        }
        return out;
    }
    static bitboard_t
    get_north_attack(const Square& sq, const bitboard_t& occupied)
    {
        const std::uint32_t magic = magic_number_north[sq];
        const auto index
            = Magic::template to_magic_table_index<log2_table_size_lance>(
                occupied & premask_north[sq], magic);
        assert(index < table_size_lance);
        return attack_table_north[sq][index];
    }
    static bitboard_t
    get_south_attack(const Square& sq, const bitboard_t& occupied)
    {
        const std::uint32_t magic = magic_number_south[sq];
        const auto index
            = Magic::template to_magic_table_index<log2_table_size_lance>(
                occupied & premask_south[sq], magic);
        assert(index < table_size_lance);
        return attack_table_south[sq][index];
    }
    static bitboard_t get_adjacent_attack(const Square& sq)
    {
        return attack_table_adjacent[sq][0u];
    }
    static bitboard_t
    get_adjacent_attack(const Square& sq, const bitboard_t& occupied)
    {
        const std::uint32_t magic = magic_number_adjacent[sq];
        const auto index
            = Magic::template to_magic_table_index<log2_table_size_adjacent>(
                occupied & premask_adjacent[sq], magic);
        assert(index < table_size_adjacent);
        return attack_table_adjacent[sq][index];
    }
    static bitboard_t get_diagonal_attack(const Square& sq)
    {
        return attack_table_diagonal[sq][0u];
    }
    static bitboard_t
    get_diagonal_attack(const Square& sq, const bitboard_t& occupied)
    {
        const std::uint32_t magic = magic_number_diagonal[sq];
        const auto index
            = Magic::template to_magic_table_index<log2_table_size_diagonal>(
                occupied & premask_diagonal[sq], magic);
        assert(index < table_size_diagonal);
        return attack_table_diagonal[sq][index];
    }
    static bitboard_t get_occupancy(
        const uint index, // 0 ~ (1<<num_relevant_squares - 1)
        const uint num_relevant_squares,
        const uint* const relevant_square_locations)
    {
        bitboard_t out{};
        for (uint ii = 0u; ii < num_relevant_squares; ++ii) {
            if (index & (1u << ii))
                out |= BT::from_square(
                    static_cast<Square>(relevant_square_locations[ii]));
        }
        return out;
    }
    template <uint Shift>
    static uint
    to_magic_table_index(bitboard_t relevant_occ, std::uint32_t magic)
    {
        if constexpr (sizeof(bitboard_t) == sizeof(std::uint32_t)) {
            return static_cast<uint>(rust_to_magic_table_index_u32(
                static_cast<std::uint32_t>(relevant_occ), magic, Shift));
        } else if constexpr (sizeof(bitboard_t) == sizeof(std::uint64_t)) {
            return static_cast<uint>(rust_to_magic_table_index_u64(
                static_cast<std::uint64_t>(relevant_occ), magic, Shift));
        } else if constexpr (sizeof(bitboard_t) == sizeof(uint128)) {
            return static_cast<uint>(rust_to_magic_table_index_u128(
                static_cast<std::uint64_t>(relevant_occ >> 64),
                static_cast<std::uint64_t>(relevant_occ),
                magic,
                Shift));
        } else {
            std::uint32_t product = 0u;
            while (relevant_occ) {
                product ^= static_cast<std::uint32_t>(relevant_occ) * magic;
                relevant_occ >>= 31u;
            }
            return product >> (32u - Shift);
        }
    }

private:
    static void init_attack_table_north()
    {
        for (auto sq : C::square_iterator()) {
            const bitboard_t& premask = premask_north[sq];
            const uint num_relevant_squares = hamming_weight(premask);
            const std::uint32_t magic = magic_number_north[sq];

            uint relevant_square_locations[C::num_squares] = {0};
            for (uint ii = 0, jj = 0; ii < C::num_squares; ++ii) {
                if (BT::is_one(premask, static_cast<Square>(ii)))
                    relevant_square_locations[jj++] = ii;
            }
            for (uint ii = (1u << num_relevant_squares); ii--;) {
                const bitboard_t occ = get_occupancy(
                    ii, num_relevant_squares, relevant_square_locations);
                const bitboard_t attack = compute_ray_to(sq, DIR_N, occ);
                const auto index = Magic::template to_magic_table_index<
                    log2_table_size_lance>(occ, magic);
                assert(index < table_size_lance);
                attack_table_north[sq][index] = attack;
            }
        }
    }
    static void init_attack_table_south()
    {
        for (auto sq : C::square_iterator()) {
            const bitboard_t& premask = premask_south[sq];
            const uint num_relevant_squares = hamming_weight(premask);
            const std::uint32_t magic = magic_number_south[sq];

            uint relevant_square_locations[C::num_squares] = {0};
            for (uint ii = 0, jj = 0; ii < C::num_squares; ++ii) {
                if (BT::is_one(premask, static_cast<Square>(ii)))
                    relevant_square_locations[jj++] = ii;
            }
            for (uint ii = (1u << num_relevant_squares); ii--;) {
                const bitboard_t occ = get_occupancy(
                    ii, num_relevant_squares, relevant_square_locations);
                const bitboard_t attack = compute_ray_to(sq, DIR_S, occ);
                const auto index = Magic::template to_magic_table_index<
                    log2_table_size_lance>(occ, magic);
                assert(index < table_size_lance);
                attack_table_south[sq][index] = attack;
            }
        }
    }
    static void init_attack_table_adjacent()
    {
        for (auto sq : C::square_iterator()) {
            const bitboard_t& premask = premask_adjacent[sq];
            const uint num_relevant_squares = hamming_weight(premask);
            const std::uint32_t magic = magic_number_adjacent[sq];

            uint relevant_square_locations[C::num_squares] = {0};
            for (uint ii = 0, jj = 0; ii < C::num_squares; ++ii) {
                if (BT::is_one(premask, static_cast<Square>(ii)))
                    relevant_square_locations[jj++] = ii;
            }
            for (uint ii = (1u << num_relevant_squares); ii--;) {
                const bitboard_t occ = get_occupancy(
                    ii, num_relevant_squares, relevant_square_locations);
                const bitboard_t attack = compute_ray_to(sq, DIR_N, occ)
                                          | compute_ray_to(sq, DIR_W, occ)
                                          | compute_ray_to(sq, DIR_E, occ)
                                          | compute_ray_to(sq, DIR_S, occ);
                const auto index = Magic::template to_magic_table_index<
                    log2_table_size_adjacent>(occ, magic);
                assert(index < table_size_adjacent);
                attack_table_adjacent[sq][index] = attack;
            }
        }
    }
    static void init_attack_table_diagonal()
    {
        for (auto sq : C::square_iterator()) {
            const bitboard_t& premask = premask_diagonal[sq];
            const uint num_relevant_squares = hamming_weight(premask);
            const std::uint32_t magic = magic_number_diagonal[sq];

            uint relevant_square_locations[C::num_squares] = {0};
            for (uint ii = 0, jj = 0; ii < C::num_squares; ++ii) {
                if (BT::is_one(premask, static_cast<Square>(ii)))
                    relevant_square_locations[jj++] = ii;
            }
            for (uint ii = (1u << num_relevant_squares); ii--;) {
                const bitboard_t occ = get_occupancy(
                    ii, num_relevant_squares, relevant_square_locations);
                const bitboard_t attack = compute_ray_to(sq, DIR_NW, occ)
                                          | compute_ray_to(sq, DIR_NE, occ)
                                          | compute_ray_to(sq, DIR_SW, occ)
                                          | compute_ray_to(sq, DIR_SE, occ);
                const auto index = Magic::template to_magic_table_index<
                    log2_table_size_diagonal>(occ, magic);
                assert(index < table_size_diagonal);
                attack_table_diagonal[sq][index] = attack;
            }
        }
    }
};

} // namespace vshogi

#endif // VSHOGI_COMMON_MAGIC_HPP
