#ifndef VSHOGI_STAND_HPP
#define VSHOGI_STAND_HPP

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

    static const uint shift_bits[Config::num_stand_piece_types];
    static const Int masks[Config::num_stand_piece_types];
    static const Int deltas[Config::num_stand_piece_types];
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
    static constexpr uint num_stand_piece_types = Config::num_stand_piece_types;

    static const PieceType stand_pieces_in_sfen_order[num_stand_piece_types];
    static const int max_sfen_length;

    StandType m_black;
    StandType m_white;

public:
    BlackWhiteStands() : m_black(), m_white()
    {
    }
    bool operator==(const BlackWhiteStands& other) const
    {
        return (m_black == other.m_black) && (m_white == other.m_white);
    }
    bool operator!=(const BlackWhiteStands& other) const
    {
        return (m_black != other.m_black) || (m_white != other.m_white);
    }
    const StandType& black() const
    {
        return m_black;
    }
    const StandType& white() const
    {
        return m_white;
    }
    StandType& operator[](const ColorEnum& c)
    {
        return (c == BLACK) ? m_black : m_white;
    }
    const StandType& operator[](const ColorEnum& c) const
    {
        return (c == BLACK) ? m_black : m_white;
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
                m_black.add(PHelper::to_piece_type(*ptr), num ? num : 1);
            else if (('a' <= *ptr) && (*ptr <= 'z'))
                m_white.add(PHelper::to_piece_type(*ptr), num ? num : 1);
            num = 0;
        }
    END:
        return ptr;
    }
    void append_sfen(std::string& out) const
    {
        if (!(m_black.any() || m_white.any())) {
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
};

} // namespace vshogi

#endif // VSHOGI_STAND_HPP
