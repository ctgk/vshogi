#ifndef VSHOGI_STAND_HPP
#define VSHOGI_STAND_HPP

#include <string>

#include "vshogi/color.hpp"

namespace vshogi
{

template <class Pieces, class Int>
class Stand
{
private:
    static const int shift_bits[Pieces::num_stand_piece_types];
    static const Int masks[Pieces::num_stand_piece_types];
    static const Int deltas[Pieces::num_stand_piece_types];
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

    int count(const typename Pieces::PieceTypeEnum& p) const
    {
        return static_cast<int>((m_value & masks[p]) >> shift_bits[p]);
    }
    bool exist(const typename Pieces::PieceTypeEnum& p) const
    {
        return (m_value & masks[p]) > 0;
    }
    bool any() const
    {
        return m_value > 0;
    }
    Stand& add(const typename Pieces::PieceTypeEnum& p, const int num = 1)
    {
        for (int ii = num; ii--;) {
            m_value = static_cast<Int>(m_value + deltas[Pieces::demote(p)]);
        }
        return *this;
    }
    Stand& subtract(const typename Pieces::PieceTypeEnum& p)
    {
        m_value = static_cast<Int>(m_value - deltas[Pieces::demote(p)]);
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

template <class Stand, class Pieces>
class BlackWhiteStands
{
private:
    static const typename Pieces::PieceTypeEnum
        stand_pieces_in_sfen_order[Pieces::num_stand_piece_types];
    static const int max_sfen_length;

    Stand m_black;
    Stand m_white;

public:
    using StandType = Stand;
    BlackWhiteStands() : m_black(), m_white()
    {
    }
    Stand& black()
    {
        return m_black;
    }
    const Stand& black() const
    {
        return m_black;
    }
    Stand& white()
    {
        return m_white;
    }
    const Stand& white() const
    {
        return m_white;
    }
    Stand& operator[](const ColorEnum& c)
    {
        return (c == BLACK) ? m_black : m_white;
    }
    const Stand& operator[](const ColorEnum& c) const
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
                m_black.add(Pieces::to_piece_type(*ptr), num ? num : 1);
            else if (('a' <= *ptr) && (*ptr <= 'z'))
                m_white.add(Pieces::to_piece_type(*ptr), num ? num : 1);
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
                Pieces::append_sfen(Pieces::to_board_piece(c, p), out);
            }
        }
    }
};

} // namespace vshogi

#endif // VSHOGI_STAND_HPP
