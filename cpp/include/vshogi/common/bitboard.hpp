#ifndef VSHOGI_BITBOARD_HPP
#define VSHOGI_BITBOARD_HPP

#include <cstdint>

#include "vshogi/common/color.hpp"
#include "vshogi/common/direction.hpp"
#include "vshogi/common/squares.hpp"
#include "vshogi/common/utils.hpp"

namespace vshogi
{

template <class Config>
class BitBoard
{
private:
    using ColoredPiece = typename Config::ColoredPiece;
    using Square = typename Config::Square;
    using UInt = typename Config::BaseTypeBitBoard;
    static constexpr uint num_squares = Config::num_squares;
    static constexpr uint num_files = Config::num_files;
    static constexpr uint num_ranks = Config::num_ranks;
    static constexpr uint num_dir = Config::num_dir;
    static constexpr uint num_promotion_ranks = Config::num_promotion_ranks;
    static constexpr uint num_colored_piece_types
        = Config::num_colored_piece_types;
    using SHelper = Squares<Config>;
    using PHelper = Pieces<Config>;

    UInt m_value;

    static constexpr UInt mask
        = (static_cast<UInt>(1) << num_squares) - static_cast<UInt>(1);
    static const BitBoard square_to_bitboard_array[num_squares + 1U];
    static BitBoard attacks_table[num_colored_piece_types][num_squares];
    static BitBoard ray_table[num_squares][num_dir];

public:
    constexpr BitBoard() : m_value()
    {
    }
    template <class Int>
    constexpr BitBoard(const Int& v) : m_value(UInt(v) & mask)
    {
    }
    constexpr UInt value() const
    {
        return m_value;
    }
    constexpr BitBoard operator~() const
    {
        return BitBoard(~m_value);
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
        return BitBoard(m_value << shift_width);
    }
    constexpr BitBoard operator>>(const uint& shift_width) const
    {
        return BitBoard(m_value >> shift_width);
    }
    constexpr bool operator==(const BitBoard& other) const
    {
        return m_value == other.m_value;
    }
    constexpr bool any() const
    {
        return static_cast<bool>(m_value);
    }
    bool is_one(const Square& sq) const
    {
        return static_cast<bool>(from_square(sq).m_value & m_value);
    }
    uint hamming_weight() const
    {
        return vshogi::hamming_weight(m_value);
    }

    static BitBoard from_square(const Square& sq)
    {
        return square_to_bitboard_array[sq];
    }
    static constexpr BitBoard get_promotion_zone(const ColorEnum& c)
    {
        constexpr uint s = num_squares - num_promotion_ranks * num_files;
        return (c == BLACK) ? ((~BitBoard(0)) << s) >> s
                            : ((~BitBoard(0)) >> s) << s;
    }

    constexpr BitBoard shift(const DirectionEnum& dir) const
    {
        constexpr auto bb_all = ~BitBoard(0);
        constexpr auto bb_all_but_lmost = ~file_mask_leftmost();
        constexpr auto bb_all_but_rmost = ~file_mask_rightmost();
        const auto delta = SHelper::direction_to_delta(dir);
        constexpr BitBoard filemask[] = {
            // clang-format off
            bb_all_but_lmost, bb_all, bb_all_but_rmost,
            bb_all_but_lmost,         bb_all_but_rmost,
            bb_all_but_lmost, bb_all, bb_all_but_rmost,
            bb_all_but_lmost,         bb_all_but_rmost,
            bb_all_but_lmost,         bb_all_but_rmost,
            // clang-format on
        };
        if (delta > 0)
            return (*this & filemask[dir]) << static_cast<uint>(delta);
        else
            return (*this & filemask[dir]) >> static_cast<uint>(-delta);
    }

    static BitBoard compute_ray_to(
        Square sq,
        const DirectionEnum dir,
        const BitBoard& occupied = BitBoard())
    {
        BitBoard out{};
        while (true) {
            sq = SHelper::shift(sq, dir);
            if (sq == SHelper::SQ_NA)
                break; // reached the end of the board
            else if (occupied.is_one(sq)) {
                out |= BitBoard::from_square(sq);
                break; // reached a piece
            } else {
                out |= BitBoard::from_square(sq); // continue
            }
        }
        return out;
    }
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

    static BitBoard get_attacks_by(const ColoredPiece& p, const Square& sq)
    {
        if (p == PHelper::VOID)
            return BitBoard();
        return attacks_table[p][sq];
    }
    static BitBoard get_attacks_by(
        const ColoredPiece& p, const Square& sq, const BitBoard& occupied);
    static BitBoard get_ray_to(const Square& sq, const DirectionEnum& dir)
    {
        return ray_table[sq][dir];
    }
    static void init_tables()
    {
        for (auto p : EnumIterator<ColoredPiece, num_colored_piece_types>()) {
            for (auto sq : EnumIterator<Square, num_squares>()) {
                attacks_table[p][sq] = compute_attack_by(p, sq);
            }
        }

        for (auto sq : EnumIterator<Square, num_squares>()) {
            for (auto dir : EnumIterator<DirectionEnum, num_dir>()) {
                ray_table[sq][dir] = compute_ray_to(sq, dir);
            }
        }
    }

private:
    template <uint NumSquaresFromTop = num_ranks>
    static constexpr BitBoard file_mask_leftmost()
    {
        if constexpr (NumSquaresFromTop == 0u)
            return BitBoard(0);
        else
            return (file_mask_leftmost<NumSquaresFromTop - 1>() << num_files)
                   | BitBoard(1);
    }
    template <uint NumSquaresFromTop = num_ranks>
    static constexpr BitBoard file_mask_rightmost()
    {
        if constexpr (NumSquaresFromTop == 0u)
            return BitBoard(0);
        else
            return (BitBoard(1) << (num_files - 1))
                   | (file_mask_rightmost<NumSquaresFromTop - 1>()
                      << num_files);
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
