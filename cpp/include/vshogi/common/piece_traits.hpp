#ifndef VSHOGI_PIECE_TRAITS_HPP
#define VSHOGI_PIECE_TRAITS_HPP

#include <cassert>
#include <cctype>
#include <string>

#include "vshogi/common/color.hpp"
#include "vshogi/common/config.hpp"
#include "vshogi/common/direction.hpp"
#include "vshogi/common/utils.hpp"

namespace vshogi
{

template <class P>
struct PieceTraits
{
private:
    using C = Configuration<P>;
    using PieceType = typename C::PieceType;
    using Piece = typename C::Piece;

public:
    PieceTraits() = delete;

public: // basic
    /**
     * @brief Get the color of a board piece.
     * @note Passing VOID returns WHITE.
     *
     * @param p Board piece.
     * @return constexpr ColorEnum Color of the board piece.
     */
    static constexpr ColorEnum get_color(const Piece& p)
    {
        return static_cast<ColorEnum>(p >= C::num_piece_types);
    }
    static constexpr PieceType to_piece_type(const Piece& p)
    {
        return (p < C::num_piece_types)
                   ? static_cast<PieceType>(p)
                   : static_cast<PieceType>(p - C::num_piece_types);
    }
    static constexpr Piece make_piece(const ColorEnum& c, const PieceType& pt)
    {
        if (pt == C::NA)
            return C::VOID;
        return static_cast<Piece>(c * C::num_piece_types + pt);
    }

public: // promotion
    static constexpr bool is_promotable(const PieceType& p)
    {
        return (p + 1u < C::num_stand_piece_types);
    }
    static constexpr bool is_promotable(const Piece& p)
    {
        return is_promotable(to_piece_type(p));
    }

    static constexpr bool is_promoted(const PieceType& pt)
    {
        return pt > C::num_stand_piece_types;
    }
    static constexpr bool is_promoted(const Piece& p)
    {
        return is_promoted(to_piece_type(p));
    }
    static constexpr bool is_promotion_fully_superior(const PieceType& pt)
    {
        return FPTHelper::is_promotion_fully_superior(C::piece_types[pt]);
    }
    static constexpr bool is_promotion_fully_superior(const Piece& p)
    {
        return is_promotion_fully_superior(to_piece_type(p));
    }

    /**
     * @brief Promote a promotable piece.
     * @note If the piece is not promotable, the return may not be safe.
     *
     * @tparam T
     * @param p Piece to promote. Should be promotable.
     * @return constexpr T Promoted piece.
     */
    template <class T>
    static constexpr T promote_nocheck(const T& p)
    {
        assert(is_promotable(p));
        return static_cast<T>(p + C::num_stand_piece_types + 1);
    }

    template <class T>
    static constexpr T demote(const T& p)
    {
        if (is_promoted(p))
            return demote_nocheck(p);
        return p;
    }
    template <class T>
    static constexpr T demote_nocheck(const T& p)
    {
        return static_cast<T>(p - C::num_stand_piece_types - 1);
    }

public: // attack directions
    static bool slidable_to(const Piece& p, const DirectionEnum& d)
    {
        return FPTHelper::slidable_to(
            C::piece_types[to_piece_type(p)],
            (get_color(p) == BLACK) ? d : rotate(d));
    }
    static bool is_slider(const PieceType& pt)
    {
        return FPTHelper::is_slider(C::piece_types[pt]);
    }
    static bool is_slider(const Piece& p)
    {
        return is_slider(to_piece_type(p));
    }
    static bool is_attacking_to(const Piece& p, const DirectionEnum& d)
    {
        return FPTHelper::is_attacking_to(
            C::piece_types[to_piece_type(p)],
            (get_color(p) == BLACK) ? d : rotate(d));
    }
    static const DirectionEnum* get_attack_directions(const Piece& p)
    {
        assert(p != C::VOID);
        return FPTHelper::get_attack_directions(
            C::piece_types[to_piece_type(p)], get_color(p));
    }

public: // point, value
    static uint get_point(const PieceType& p)
    {
        return FPTHelper::to_point(C::piece_types[p]);
    }
    static uint get_point(const Piece& p)
    {
        return get_point(to_piece_type(p));
    }
    static uint get_value(const PieceType& pt)
    {
        return FPTHelper::to_value(C::piece_types[pt]);
    }

public: // char, str
    static PieceType to_piece_type(char c)
    {
        const FullPieceTypes fpt = FPTHelper::to_fpt(c);
        c = static_cast<char>(std::tolower(static_cast<int>(c)));
        const FullPieceTypes* ptr = C::piece_types.data();
        for (; *ptr != PT_NA; ++ptr) {
            if (*ptr == fpt)
                return static_cast<PieceType>(ptr - C::piece_types.data());
        }
        return C::NA;
    }
    static constexpr Piece make_piece(const char c)
    {
        return make_piece(
            std::islower(static_cast<int>(c)) ? WHITE : BLACK,
            to_piece_type(c));
    }
};

} // namespace vshogi

#endif // VSHOGI_PIECE_TRAITS_HPP
