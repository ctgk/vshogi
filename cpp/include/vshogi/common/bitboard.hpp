#ifndef VSHOGI_BITBOARD_HPP
#define VSHOGI_BITBOARD_HPP

#include <cassert>
#include <cstdint>

#include "vshogi/common/color.hpp"
#include "vshogi/common/config.hpp"
#include "vshogi/common/direction.hpp"
#include "vshogi/common/squares.hpp"
#include "vshogi/common/utils.hpp"

namespace vshogi
{

template <class Parameters>
class BitBoard
{
private:
    using C = Configuration<Parameters>;
    using ColoredPiece = typename C::ColoredPiece;
    using File = typename C::File;
    using Rank = typename C::Rank;
    using Square = typename C::Square;
    using UInt = typename C::BaseTypeBitBoard;
    using SHelper = Squares<Parameters>;
    using PHelper = Pieces<Parameters>;

    static constexpr UInt ones(const uint num_ones)
    {
        return ~(~UInt() << num_ones);
    }

    UInt m_value;

    static constexpr UInt mask = ones(C::num_squares);
    static BitBoard attacks_table[C::num_colored_piece_types][C::num_squares];
    static BitBoard ray_table[C::num_squares][C::num_dir];
    static BitBoard line_segment_table[C::num_squares][C::num_squares];
    static BitBoard neighbor_table[num_colors][C::num_squares];
    static BitBoard neighbor_2nd_table[num_colors][C::num_squares];

    constexpr BitBoard(const UInt v_masked) : m_value(v_masked)
    {
    }

public:
    constexpr BitBoard() : m_value()
    {
    }
    constexpr UInt value() const
    {
        return m_value;
    }
    constexpr BitBoard operator~() const
    {
        return BitBoard(m_value ^ mask);
    }
    constexpr BitBoard operator|(const BitBoard& other) const
    {
        return BitBoard(m_value | other.m_value);
    }
    BitBoard& operator|=(const BitBoard& other)
    {
        m_value |= other.m_value;
        return *this;
    }
    constexpr BitBoard operator&(const BitBoard& other) const
    {
        return BitBoard(m_value & other.m_value);
    }
    BitBoard& operator&=(const BitBoard& other)
    {
        m_value &= other.m_value;
        return *this;
    }
    constexpr BitBoard operator^(const BitBoard& other) const
    {
        return BitBoard(m_value ^ other.m_value);
    }
    BitBoard& operator^=(const BitBoard& other)
    {
        m_value ^= other.m_value;
        return *this;
    }
    constexpr BitBoard operator<<(const uint& shift_width) const
    {
        return BitBoard((m_value << shift_width) & mask);
    }
    constexpr BitBoard operator>>(const uint& shift_width) const
    {
        return BitBoard(m_value >> shift_width);
    }
    BitBoard& operator>>=(const uint& shift_width)
    {
        m_value >>= shift_width;
        return *this;
    }
    constexpr bool operator==(const BitBoard& other) const
    {
        return m_value == other.m_value;
    }
    constexpr bool operator!=(const BitBoard& other) const
    {
        return m_value != other.m_value;
    }
    constexpr bool any() const
    {
        return static_cast<bool>(m_value);
    }
    constexpr bool empty() const
    {
        return !static_cast<bool>(m_value);
    }
    bool is_one(const Square& sq) const
    {
        assert((m_value >> C::SQ_NA) == 0);
        return static_cast<bool>((m_value >> sq) & static_cast<UInt>(1));
    }

    /**
     * @brief Set a bit corresponding to the given square.
     *
     * @param sq Corresponding square bit to set. Do not pass `SQ_NA`.
     * @return BitBoard& Set bitboard.
     */
    BitBoard& set(const Square& sq)
    {
        assert(sq != C::SQ_NA);
        m_value |= static_cast<UInt>(static_cast<UInt>(1) << sq);
        return *this;
    }
    /**
     * @brief Toggle a bit corresponding to the given square.
     *
     * @param sq Corresponding square bit to toggle. Do not pass `SQ_NA`.
     * @return BitBoard& Toggled bitboard.
     */
    BitBoard& toggle(const Square& sq)
    {
        assert(sq != C::SQ_NA);
        m_value ^= static_cast<UInt>(static_cast<UInt>(1) << sq);
        return *this;
    }

    /**
     * @brief Clear a bit corresponding to the given square.
     *
     * @param sq Corresponding square bit to clear.
     * @return BitBoard& Cleared bitboard.
     */
    BitBoard& clear(const Square& sq)
    {
        m_value &= static_cast<UInt>(~(static_cast<UInt>(1) << sq));
        return *this;
    }

    uint hamming_weight() const
    {
        return vshogi::hamming_weight(m_value);
    }

    template <class Int>
    static BitBoard from_value(const Int& v)
    {
        return BitBoard(static_cast<UInt>(v) & mask);
    }
    static BitBoard from_square(const Square& sq)
    {
        return BitBoard((static_cast<UInt>(1) << sq) & mask);
    }
    template <Square SQ>
    static constexpr BitBoard from_square()
    {
        return BitBoard(static_cast<UInt>(1) << SQ);
    }
    static BitBoard from_rank(const Rank& r)
    {
        return from_rank1<>() << r;
    }
    template <Rank R>
    static constexpr BitBoard from_rank()
    {
        return from_rank1<>() << R;
    }
    template <Rank R1, Rank R2, Rank... Args>
    static constexpr BitBoard from_rank()
    {
        return from_rank<R1>() | from_rank<R2, Args...>();
    }
    static BitBoard from_rank_above(const Rank r)
    {
        BitBoard out{};
        for (uint i = 0u; i < r; ++i) {
            out |= from_rank(static_cast<Rank>(i));
        }
        return out;
    }
    static BitBoard from_rank_below(const Rank r)
    {
        BitBoard out{};
        for (uint i = static_cast<uint>(r) + 1; i < C::num_ranks; ++i) {
            out |= from_rank(static_cast<Rank>(i));
        }
        return out;
    }
    static BitBoard from_rank_above(const Rank r, const ColorEnum c)
    {
        if (c == BLACK)
            return from_rank_above(r);
        return from_rank_below(r);
    }
    static BitBoard from_rank_below(const Rank r, const ColorEnum c)
    {
        if (c == BLACK)
            return from_rank_below(r);
        return from_rank_above(r);
    }

    /**
     * @brief Return bit mask filled with 1s in the file.
     * @note Passing invalid file returns an empty mask.
     *
     * @param f Input file to fill with 1s.
     * @return constexpr BitBoard Bit mask filled with 1s in the file.
     */
    static constexpr BitBoard from_file(const File& f)
    {
        constexpr auto file_a = ones(C::num_ranks);
        return BitBoard(
            mask & (file_a << (static_cast<uint>(f) * C::num_ranks)));
    }
    static BitBoard from_file_right(const File f)
    {
        return BitBoard(ones(C::num_ranks * f));
    }
    static BitBoard from_file_left(const File f)
    {
        return ~(from_file(f) | from_file_right(f));
    }
    static BitBoard from_file_right(const File f, const ColorEnum c)
    {
        if (c == BLACK)
            return from_file_right(f);
        return from_file_left(f);
    }
    static BitBoard from_file_left(const File f, const ColorEnum c)
    {
        if (c == BLACK)
            return from_file_left(f);
        return from_file_right(f);
    }

    static BitBoard get_promotion_zone(const ColorEnum& c)
    {
        constexpr uint s = C::num_ranks - C::num_promotion_ranks;
        constexpr BitBoard br = from_ranks<C::num_promotion_ranks>();
        static const BitBoard promotion_zone_array[2] = {br, br << s};
        return promotion_zone_array[c];
    }

    constexpr BitBoard shift(const DirectionEnum& dir) const
    {
        const auto delta = SHelper::direction_to_delta(dir);
        constexpr BitBoard all = BitBoard(mask);
        constexpr BitBoard all_but_a = ~from_rank<C::RANK_A>();
        constexpr BitBoard all_but_z = ~from_rank<C::RANK_Z>();
        constexpr BitBoard all_but_ab = ~from_rank<C::RANK_A, C::RANK_B>();
        constexpr BitBoard all_but_yz = ~from_rank<C::RANK_Y, C::RANK_Z>();
        constexpr BitBoard rankmask[] = {
            // clang-format off
            all_but_a, all_but_a, all_but_a,
            all,                  all,
            all_but_z, all_but_z, all_but_z,
            all_but_yz,           all_but_yz,
            all_but_ab,           all_but_ab,
            // clang-format on
        };
        if (delta > 0)
            return (*this & rankmask[dir]) << static_cast<uint>(delta);
        else
            return (*this & rankmask[dir]) >> static_cast<uint>(-delta);
    }
    static BitBoard compute_droppable(const ColoredPiece& p)
    {
        constexpr BitBoard all = BitBoard(mask);
        constexpr BitBoard all_but_a = ~from_rank<C::RANK_A>();
        constexpr BitBoard all_but_z = ~from_rank<C::RANK_Z>();
        constexpr BitBoard all_but_ab = ~from_rank<C::RANK_A, C::RANK_B>();
        constexpr BitBoard all_but_yz = ~from_rank<C::RANK_Y, C::RANK_Z>();
        const auto dirs = PHelper::get_attack_directions(p);
        if (dirs[1] == DIR_NA) {
            return (dirs[0] == DIR_N) ? all_but_a : all_but_z;
        } else if (dirs[2] == DIR_NA) {
            return (dirs[0] > DIR_SSE) ? all_but_ab : all_but_yz;
        } else {
            return all;
        }
    }

    static BitBoard get_attacks_by(const ColoredPiece& p, const Square& sq)
    {
        if ((p == C::VOID) || (sq == C::SQ_NA))
            return BitBoard();
        return attacks_table[p][sq];
    }
    static BitBoard get_attacks_by(
        const ColoredPiece& p, const Square& sq, const BitBoard& occupied);

    /**
     * Get ray mask along a direction from a square.
     *
     * @note Ray is infinite even if the direction is a knight-direction.
     * @param sq Beginning square of a ray. (exclusive)
     * @param dir Direction of a ray.
     * @return BitBoard ray mask.
     */
    static BitBoard get_ray_to(const Square& sq, const DirectionEnum& dir)
    {
        if ((sq == C::SQ_NA) || (dir == DIR_NA))
            return BitBoard();
        return ray_table[sq][dir];
    }

    /**
     * @brief Get the line segment mask that lies in between given squares.
     *
     * @param a First square (non-inclusive)
     * @param b Second square (non-inclusive)
     * @return BitBoard Line segment mask that lines in between given squares.
     */
    static BitBoard get_line_segment(const Square& a, const Square& b)
    {
        if ((a == C::SQ_NA) || (b == C::SQ_NA))
            return BitBoard();
        return line_segment_table[a][b];
    }
    static BitBoard get_neighbor_at(const Square& sq, const ColorEnum& c)
    {
        if (sq == C::SQ_NA)
            return BitBoard();
        return neighbor_table[c][sq];
    }
    static BitBoard get_neighbor_2nd_at(const Square& sq, const ColorEnum& c)
    {
        if (sq == C::SQ_NA)
            return BitBoard();
        return neighbor_2nd_table[c][sq];
    }
    static BitBoard compute_ray_to(
        Square sq,
        const DirectionEnum dir,
        const BitBoard& occupied = BitBoard())
    {
        BitBoard out{};
        while (true) {
            sq = SHelper::shift(sq, dir);
            if (sq == C::SQ_NA)
                break; // reached the end of the board
            else if (occupied.is_one(sq)) {
                out.set(sq);
                break; // reached a piece
            } else {
                out.set(sq); // continue
            }
        }
        return out;
    }

    static void init_tables()
    {
        for (auto p :
             EnumIterator<ColoredPiece, C::num_colored_piece_types>()) {
            for (auto sq : EnumIterator<Square, C::num_squares>()) {
                attacks_table[p][sq] = compute_attack_by(p, sq);
            }
        }

        for (auto sq : EnumIterator<Square, C::num_squares>()) {
            for (auto dir : EnumIterator<DirectionEnum, C::num_dir>()) {
                ray_table[sq][dir] = compute_ray_to(sq, dir);
            }
        }

        for (auto sq1 : EnumIterator<Square, C::num_squares>()) {
            for (auto sq2 : EnumIterator<Square, C::num_squares>()) {
                line_segment_table[sq1][sq2] = compute_line_segment(sq1, sq2);
            }
        }

        for (auto c : {BLACK, WHITE}) {
            for (auto sq : EnumIterator<Square, C::num_squares>()) {
                neighbor_table[c][sq] = compute_neighbor_at(sq, c);
                neighbor_2nd_table[c][sq] = compute_2nd_neighbor_of(sq, c);
            }
        }
    }

public:
    class SquareIterator
    {
    private:
        UInt m_mask;
        uint m_curr;

    public:
        SquareIterator() : m_mask(), m_curr(C::num_squares)
        {
        }
        SquareIterator(const UInt& bb_value) : m_mask(bb_value), m_curr()
        {
            if (!static_cast<bool>(m_mask)) {
                m_curr = C::num_squares;
                return;
            }
            const auto shift = std::min(ntz(m_mask), C::num_squares);
            m_curr = shift;
            m_mask = static_cast<UInt>(m_mask >> shift);
        }
        SquareIterator& operator++()
        {

            ++m_curr;
            m_mask = static_cast<UInt>(m_mask >> 1u);
            if (!static_cast<bool>(m_mask)) {
                m_curr = C::num_squares;
                return *this;
            }
            const auto shift = ntz(m_mask);
            m_curr += shift;
            m_mask = static_cast<UInt>(m_mask >> shift);
            return *this;
        }
        Square operator*() const
        {
            return static_cast<Square>(m_curr);
        }
        SquareIterator begin()
        {
            return *this;
        }
        SquareIterator end() const
        {
            static const auto end_iter = SquareIterator();
            return end_iter;
        }
        bool operator!=(const SquareIterator& other) const
        {
            return m_curr != other.m_curr;
        }
        bool is_end() const
        {
            return m_curr >= C::num_squares;
        }
    };
    SquareIterator square_iterator() const
    {
        return SquareIterator(m_value);
    }

private:
    static BitBoard compute_ray_to_adjacent(
        const Square& sq, const BitBoard& occupied = BitBoard())
    {
        return compute_ray_to(sq, DIR_N, occupied)
               | compute_ray_to(sq, DIR_E, occupied)
               | compute_ray_to(sq, DIR_W, occupied)
               | compute_ray_to(sq, DIR_S, occupied);
    }
    static BitBoard compute_ray_to_diagonal(
        const Square& sq, const BitBoard& occupied = BitBoard())
    {
        return compute_ray_to(sq, DIR_NW, occupied)
               | compute_ray_to(sq, DIR_NE, occupied)
               | compute_ray_to(sq, DIR_SW, occupied)
               | compute_ray_to(sq, DIR_SE, occupied);
    }

private:
    static BitBoard compute_line_segment(Square a, const Square b)
    {
        const auto dir_to_b = SHelper::get_direction(b, a);
        if (dir_to_b == DIR_NA)
            return BitBoard();
        BitBoard out{};
        while (true) {
            a = SHelper::shift(a, dir_to_b);
            if (a == b)
                break;
            out.set(a);
        }
        return out;
    }

private:
    static BitBoard
    compute_neighbor_at(const Square& sq, const ColorEnum& by_side)
    {
        auto out = BitBoard::from_square(sq);
        out |= get_attacks_by(PHelper::to_board_piece(BLACK, C::OU), sq);
        if constexpr (C::num_dir > 8) {
            for (auto d : {DIR_SSE, DIR_SSW}) {
                if (by_side == WHITE)
                    d = rotate(d);
                out |= BitBoard::from_square(sq).shift(d);
            }
        }
        return out;
    }
    static BitBoard compute_neighbor5x5(const Square& sq)
    {
        const auto base = BitBoard::from_square(sq);
        auto out = base;
        out |= get_attacks_by(PHelper::to_board_piece(BLACK, C::OU), sq);
        out |= out.shift(DIR_W) | out.shift(DIR_E);
        out |= out.shift(DIR_N) | out.shift(DIR_S);
        return out;
    }
    static BitBoard
    compute_2nd_neighbor_of(const Square& sq, const ColorEnum& c)
    {
        auto out = compute_neighbor5x5(sq);
        if constexpr (C::num_dir > 8) {
            const auto d = (c == BLACK) ? DIR_S : DIR_N;
            out |= out.shift(d);

            BitBoard k = from_square(sq).shift(d).shift(d).shift(d).shift(d);
            k |= k.shift(DIR_E).shift(DIR_E);
            k |= k.shift(DIR_W).shift(DIR_W);
            out |= k;
        }
        return out;
    }

private:
    template <uint NumSqsFromSQ1A = C::num_files>
    static constexpr BitBoard from_rank1()
    {
        if constexpr (NumSqsFromSQ1A == 0u)
            return BitBoard(0);
        else
            return (from_rank1<NumSqsFromSQ1A - 1>() << C::num_ranks)
                   | BitBoard(1);
    }
    template <uint NumRanks>
    static constexpr BitBoard from_ranks()
    {
        if constexpr (NumRanks == 0u)
            return BitBoard(0);
        else
            return (from_ranks<NumRanks - 1>() << 1u) | from_rank1<>();
    }
    static BitBoard compute_attack_by(const ColoredPiece& p, const Square& sq)
    {
        auto a = BitBoard();
        if (PHelper::is_ranging_piece(p)) {
            for (auto pd = PHelper::get_attack_directions(p); *pd != DIR_NA;
                 ++pd) {
                if (PHelper::is_ranging_to(p, *pd))
                    a |= compute_ray_to(sq, *pd);
            }
        }
        for (auto pd = PHelper::get_attack_directions(p); *pd != DIR_NA;) {
            a |= from_square(sq).shift(*pd++);
        }
        return a;
    }
};

} // namespace vshogi

#endif // VSHOGI_BITBOARD_HPP
