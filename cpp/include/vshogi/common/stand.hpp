#ifndef VSHOGI_STAND_HPP
#define VSHOGI_STAND_HPP

#include <random>
#include <string>

#include "vshogi/common/color.hpp"
#include "vshogi/common/pieces.hpp"

namespace vshogi
{

template <class Parameters>
class Stand
{
private:
    using C = Configuration<Parameters>;
    using Int = typename C::BaseTypeStand;
    using PieceType = typename C::PieceType;
    using PHelper = Pieces<Parameters>;
    static constexpr uint num_piece_types = C::num_stand_piece_types;

    static const uint shift_bits[num_piece_types];
    static const Int masks[num_piece_types];
    static const Int deltas[num_piece_types];
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

    Int value() const
    {
        return m_value;
    }
    uint count(const PieceType& p) const
    {
        assert(static_cast<uint>(p) < num_piece_types);
        return static_cast<uint>((m_value & masks[p]) >> shift_bits[p]);
    }
    uint unique_count() const
    {
        uint out = 0u;
        for (auto pt : C::stand_piece_type_iterator())
            out += static_cast<uint>(exist(pt));
        return out;
    }
    bool exist(const PieceType& p) const
    {
        assert(static_cast<uint>(p) < num_piece_types);
        return (m_value & masks[p]) > 0;
    }
    bool any() const
    {
        return m_value > 0;
    }
    Stand& add(const PieceType& p, const int num = 1)
    {
        assert(p != C::NA);
        const auto p_demoted = PHelper::demote(p);
        for (int ii = num; ii--;) {
            m_value = static_cast<Int>(m_value + deltas[p_demoted]);
        }
        return *this;
    }
    Stand& subtract(const PieceType& p)
    {
        assert(p != C::NA);
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
    bool operator>=(const Stand& other) const
    {
        return (((m_value - other.m_value) & ~mask) == 0u);
    }
    bool operator<=(const Stand& other) const
    {
        return other.operator>=(*this);
    }
    bool operator>(const Stand& other) const
    {
        const auto d = m_value - other.m_value;
        return (d != 0u) && ((d & ~mask) == 0u);
    }
    bool operator<(const Stand& other) const
    {
        return other.operator>(*this);
    }
};

template <class Parameters>
class BlackWhiteStands
{
private:
    using C = Configuration<Parameters>;
    using PHelper = Pieces<Parameters>;
    using PieceType = typename C::PieceType;
    using ColoredPiece = typename C::ColoredPiece;

public:
    using StandType = Stand<Parameters>;

private:
    static constexpr uint num_piece_types = C::num_stand_piece_types;
    static constexpr uint max_piece_count = C::max_stand_piece_count;
    static constexpr uint max_sfen_length = 100u;

    static const PieceType pieces_in_sfen_order[num_piece_types];
    static std::uint64_t zobrist_table[num_colors][num_piece_types]
                                      [max_piece_count + 1];

    StandType m_stands[num_colors];
    std::uint64_t m_hash;

    std::uint64_t compute_zobrist_hash() const
    {
        std::uint64_t out = static_cast<std::uint64_t>(0);
        for (auto& c : color_array) {
            for (auto pt : C::stand_piece_type_iterator()) {
                const auto num = m_stands[c].count(pt);
                assert(pt < num_piece_types);
                assert(num <= max_piece_count);
                out ^= zobrist_table[c][pt][num];
            }
        }
        return out;
    }

public:
    BlackWhiteStands() : m_stands{}, m_hash()
    {
        m_hash = compute_zobrist_hash();
    }
    BlackWhiteStands(const std::string& sfen) : m_stands{}, m_hash()
    {
        set_sfen(sfen.c_str());
        m_hash = compute_zobrist_hash();
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
        for (; ptr < sfen + max_sfen_length; ++ptr) {
            switch (*ptr) {
            case '-':
                ++ptr;
                goto END;
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
        if (ptr[0] == ' ')
            ++ptr;
        m_hash = compute_zobrist_hash();
        return ptr;
    }
    void append_sfen(std::string& out) const
    {
        if (!(m_stands[BLACK].any() || m_stands[WHITE].any())) {
            out += '-';
            return;
        }
        for (auto& c : color_array) {
            for (auto& p : pieces_in_sfen_order) {
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
    ColoredPiece pop_piece_from(
        const ColorEnum& c,
        const PieceType& pt,
        std::uint64_t* const hash = nullptr)
    {
        m_stands[c].subtract(pt);
        const auto num_after = m_stands[c].count(pt);
        const auto num_before = num_after + 1;
        assert(pt != C::NA);
        assert(num_before <= max_piece_count);
        assert(num_after <= max_piece_count);
        m_hash ^= zobrist_table[c][pt][num_before];
        m_hash ^= zobrist_table[c][pt][num_after];
        if (hash != nullptr) {
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
        const ColoredPiece& captured, std::uint64_t* const hash = nullptr)
    {
        if ((captured == C::VOID)
            || (PHelper::to_piece_type(captured) == C::OU))
            return;

        const auto c = ~PHelper::get_color(captured);
        const auto pt_demoted
            = PHelper::demote(PHelper::to_piece_type(captured));
        m_stands[c].add(pt_demoted);
        const auto num_after = m_stands[c].count(pt_demoted);
        const auto num_before = num_after - 1;
        assert(pt_demoted < num_piece_types);
        assert(num_before <= max_piece_count);
        assert(num_after <= max_piece_count);
        m_hash ^= zobrist_table[c][pt_demoted][num_before];
        m_hash ^= zobrist_table[c][pt_demoted][num_after];
        if (hash != nullptr) {
            *hash ^= zobrist_table[c][pt_demoted][num_before];
            *hash ^= zobrist_table[c][pt_demoted][num_after];
        }
    }
    void return_dropped_piece(const ColoredPiece& dropped)
    {
        const auto c = PHelper::get_color(dropped);
        const auto pt = PHelper::to_piece_type(dropped);
        m_stands[c].add(pt);
        const auto num_after = m_stands[c].count(pt);
        const auto num_before = num_after - 1;
        m_hash ^= zobrist_table[c][pt][num_before];
        m_hash ^= zobrist_table[c][pt][num_after];
    }
    void remove_captured_piece(const ColoredPiece& captured)
    {
        const auto pt = PHelper::to_piece_type(captured);
        if (pt == C::OU)
            return;
        const auto c = ~PHelper::get_color(captured);
        const auto pt_demoted = PHelper::demote(pt);
        m_stands[c].subtract(pt_demoted);
        const auto num_after = m_stands[c].count(pt_demoted);
        const auto num_before = num_after + 1;
        m_hash ^= zobrist_table[c][pt_demoted][num_before];
        m_hash ^= zobrist_table[c][pt_demoted][num_after];
    }
    std::uint64_t get_zobrist_hash() const
    {
        return m_hash;
    }
    static void init_tables()
    {
        std::random_device dev;
        std::mt19937_64 rng(dev());
        std::uniform_int_distribution<std::uint64_t> dist;
        for (auto&& c : color_array) {
            for (auto pt : C::stand_piece_type_iterator()) {
                for (uint num = 0; num < max_piece_count + 1; ++num) {
                    assert(pt < num_piece_types);
                    assert(num <= max_piece_count);
                    zobrist_table[c][pt][num] = dist(rng);
                }
            }
        }
    }
};

} // namespace vshogi

#endif // VSHOGI_STAND_HPP
