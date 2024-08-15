#ifndef VSHOGI_STAND_HPP
#define VSHOGI_STAND_HPP

#include <random>
#include <string>

#include "vshogi/common/color.hpp"
#include "vshogi/common/pieces.hpp"

namespace vshogi
{

template <class Config>
class Stand
{
private:
    using Int = typename Config::BaseTypeStand;
    using PieceType = typename Config::PieceType;
    using PHelper = Pieces<Config>;
    static constexpr uint num_stand_piece_types = Config::num_stand_piece_types;

    static const uint shift_bits[num_stand_piece_types];
    static const Int masks[num_stand_piece_types];
    static const Int deltas[num_stand_piece_types];
    static const Int mask;

    Int m_value;

public:
    Stand() : m_value(static_cast<Int>(0))
    {
    }
    Stand(const Int& v) : m_value(static_cast<Int>(v & mask))
    {
    }
    template <typename... Args>
    Stand(const int, const int, Args...);

    uint count(const PieceType& p) const
    {
        return static_cast<uint>((m_value & masks[p]) >> shift_bits[p]);
    }
    bool exist(const PieceType& p) const
    {
        return (m_value & masks[p]) > 0;
    }
    bool any() const
    {
        return m_value > 0;
    }
    Stand& add(const PieceType& p, const int num = 1)
    {
        for (int ii = num; ii--;) {
            m_value = static_cast<Int>(m_value + deltas[PHelper::demote(p)]);
        }
        return *this;
    }
    Stand& subtract(const PieceType& p)
    {
        m_value = static_cast<Int>(m_value - deltas[PHelper::demote(p)]);
        return *this;
    }
    bool operator==(const Stand& other) const
    {
        return m_value == other.m_value;
    }
    bool operator!=(const Stand& other) const
    {
        return m_value != other.m_value;
    }
};

template <class Config>
class BlackWhiteStands
{
public:
    using StandType = Stand<Config>;

private:
    using PHelper = Pieces<Config>;
    using PieceType = typename Config::PieceType;
    using BoardPieceType = typename Config::BoardPieceType;
    static constexpr uint num_stand_piece_types = Config::num_stand_piece_types;
    static constexpr uint max_stand_piece_count = Config::max_stand_piece_count;
    static constexpr uint max_sfen_length = Config::max_stand_sfen_length;

    static const PieceType stand_pieces_in_sfen_order[num_stand_piece_types];
    static std::uint64_t zobrist_table[num_colors][num_stand_piece_types]
                                      [max_stand_piece_count + 1];

    StandType m_stands[num_colors];

public:
    BlackWhiteStands() : m_stands{}
    {
    }
    BlackWhiteStands(const std::string& sfen) : m_stands{}
    {
        set_sfen(sfen.c_str());
    }
    bool operator==(const BlackWhiteStands& other) const
    {
        return (
            (m_stands[BLACK] == other.m_stands[BLACK])
            && (m_stands[WHITE] == other.m_stands[WHITE]));
    }
    bool operator!=(const BlackWhiteStands& other) const
    {
        return (
            (m_stands[BLACK] != other.m_stands[BLACK])
            || (m_stands[WHITE] != other.m_stands[WHITE]));
    }
    const StandType& black() const
    {
        return m_stands[BLACK];
    }
    const StandType& white() const
    {
        return m_stands[WHITE];
    }
    const StandType& operator[](const ColorEnum& c) const
    {
        return (c == BLACK) ? m_stands[BLACK] : m_stands[WHITE];
    }
    const char* set_sfen(const char* const sfen)
    {
        int num = 0;
        const char* ptr = sfen;
        for (; ptr - sfen < max_sfen_length; ++ptr) {
            switch (*ptr) {
            case '-':
                ++ptr; // fall-through
            case ' ':
                ++ptr; // fall-through
            case '\0':
                goto END;
            default:
                break;
            }

            while (('0' <= *ptr) && (*ptr <= '9')) {
                num *= 10;
                num += (*ptr++ - '0');
            }

            if (('A' <= *ptr) && (*ptr <= 'Z'))
                m_stands[BLACK].add(
                    PHelper::to_piece_type(*ptr), num ? num : 1);
            else if (('a' <= *ptr) && (*ptr <= 'z'))
                m_stands[WHITE].add(
                    PHelper::to_piece_type(*ptr), num ? num : 1);
            num = 0;
        }
    END:
        return ptr;
    }
    void append_sfen(std::string& out) const
    {
        if (!(m_stands[BLACK].any() || m_stands[WHITE].any())) {
            out += '-';
            return;
        }
        for (auto& c : color_array) {
            for (auto& p : stand_pieces_in_sfen_order) {
                const auto num = operator[](c).count(p);
                if (num == 0)
                    continue;
                if (num > 9)
                    out += '1';
                if (num > 1)
                    out += static_cast<char>('0' + num % 10);
                PHelper::append_sfen(PHelper::to_board_piece(c, p), out);
            }
        }
    }
    BoardPieceType pop_piece_from(
        const ColorEnum& c, const PieceType& pt, std::uint64_t* const hash)
    {
        m_stands[c].subtract(pt);
        if (hash != nullptr) {
            const auto num_after = m_stands[c].count(pt);
            const auto num_before = num_after + 1;
            *hash ^= zobrist_table[c][pt][num_before];
            *hash ^= zobrist_table[c][pt][num_after];
        }
        return PHelper::to_board_piece(c, pt);
    }

    /**
     * @brief Add captured piece on a stand with opposite color of the piece.
     * @note Note the following:
     * - If the piece is promoted, add demoted piece on the stand.
     * - If the piece is `VOID` or king, do nothing.
     *
     * @param captured Captured piece.
     * @param hash Pointer to zobrist hash value.
     */
    void add_captured_piece(
        const BoardPieceType& captured, std::uint64_t* const hash = nullptr)
    {
        if ((captured == PHelper::VOID)
            || (PHelper::to_piece_type(captured) == PHelper::OU))
            return;

        const auto c = ~PHelper::get_color(captured);
        const auto pt_demoted
            = PHelper::demote(PHelper::to_piece_type(captured));
        m_stands[c].add(pt_demoted);
        if (hash != nullptr) {
            const auto num_after = m_stands[c].count(pt_demoted);
            const auto num_before = num_after - 1;
            *hash ^= zobrist_table[c][pt_demoted][num_before];
            *hash ^= zobrist_table[c][pt_demoted][num_after];
        }
    }

    std::uint64_t zobrist_hash() const
    {
        std::uint64_t out = static_cast<std::uint64_t>(0);
        for (auto& c : color_array) {
            for (auto pt : EnumIterator<PieceType, num_stand_piece_types>()) {
                const auto num = m_stands[c].count(pt);
                out ^= zobrist_table[c][pt][num];
            }
        }
        return out;
    }

    static void init_tables()
    {
        std::random_device dev;
        std::mt19937_64 rng(dev());
        std::uniform_int_distribution<std::uint64_t> dist;
        for (auto&& c : color_array) {
            for (auto pt : EnumIterator<PieceType, num_stand_piece_types>()) {
                for (uint num = 0; num < max_stand_piece_count + 1; ++num) {
                    zobrist_table[c][pt][num] = dist(rng);
                }
            }
        }
    }
};

} // namespace vshogi

#endif // VSHOGI_STAND_HPP
