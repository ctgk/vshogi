#ifndef VSHOGI_BOARD_HPP
#define VSHOGI_BOARD_HPP

#include <cassert>
#include <random>
#include <string>

#include "vshogi/common/bitboard_traits.hpp"
#include "vshogi/common/color.hpp"
#include "vshogi/common/config.hpp"
#include "vshogi/common/direction.hpp"
#include "vshogi/common/magic.hpp"
#include "vshogi/common/move.hpp"
#include "vshogi/common/piece_traits.hpp"
#include "vshogi/common/square_traits.hpp"

namespace vshogi
{

template <class P>
class Board
{
private:
    using C = Configuration<P>;
    using BT = BitboardTraits<P>;
    using ST = SquareTraits<P>;
    using PT = PieceTraits<P>;
    using PieceType = typename C::PieceType;
    using Piece = typename C::Piece;
    using Square = typename C::Square;
    using File = typename C::File;
    using Rank = typename C::Rank;
    using bitboard_t = typename C::bitboard_t;
    static constexpr uint num_square_states = C::num_colored_piece_types + 1u;
    static std::uint64_t zobrist_table[C::num_squares][num_square_states];

private:
    std::array<Piece, C::num_squares> m_pieces;
    Square m_kings[num_colors];
    bitboard_t m_bb_color[num_colors];
    bitboard_t m_bb_piece[C::num_piece_types];
    bitboard_t m_bb_slider[num_colors];

public:
    Board()
        : m_pieces(C::initial_position), m_kings{}, m_bb_color{}, m_bb_piece{},
          m_bb_slider{}
    {
        update_internals_based_on_pieces();
    }
    Board(const char* const sfen)
        : m_pieces{}, m_kings{}, m_bb_color{}, m_bb_piece{}, m_bb_slider{}
    {
        set_sfen(sfen);
    }
    Piece get(const Square& sq) const
    {
        assert(sq != C::SQ_NA);
        return m_pieces[sq];
    }
    Piece operator[](const Square& sq) const
    {
        assert(sq != C::SQ_NA);
        return m_pieces[sq];
    }
    bool is_empty(const Square& sq) const
    {
        assert(sq != C::SQ_NA);
        return (m_pieces[sq] == C::VOID);
    }
    Square get_king_square(const ColorEnum& c) const
    {
        return m_kings[c];
    }
    bitboard_t get_occupied() const
    {
        return m_bb_color[BLACK] ^ m_bb_color[WHITE];
    }
    const bitboard_t& get_occupied(const ColorEnum& c) const
    {
        return m_bb_color[c];
    }
    /**
     * @brief Get occupation by the given piece type.
     *
     * @param pt Piece type. Note that `NA` is not allowed here.
     * @return bitboard_t Occupation by the given piece type.
     */
    const bitboard_t& get_occupied(const PieceType& pt) const
    {
        assert(pt != C::NA);
        return m_bb_piece[pt];
    }
    /**
     * @brief Get occupation by the given colored piece.
     *
     * @param p Colored piece. Note that `C::VOID` is not allowed here.
     * @return bitboard_t Occupation by the given colored piece.
     */
    bitboard_t get_occupied(const Piece& p) const
    {
        assert(p != C::VOID);
        const auto c = PT::get_color(p);
        const auto pt = PT::to_piece_type(p);
        return m_bb_color[c] & m_bb_piece[pt];
    }
    template <PieceType PT>
    bitboard_t get_occupied(const ColorEnum& c) const
    {
        static_assert(PT != C::NA);
        return m_bb_color[c] & m_bb_piece[PT];
    }
    template <PieceType PT1, PieceType PT2, PieceType... Args>
    bitboard_t get_occupied(const ColorEnum& c) const
    {
        return get_occupied<PT1>(c) | get_occupied<PT2, Args...>(c);
    }
    const bitboard_t& get_occupied_by_slider(const ColorEnum& c) const
    {
        return m_bb_slider[c];
    }
    Piece pop_from(const Square& sq, std::uint64_t* const hash = nullptr)
    {
        const auto popped = m_pieces[sq];
        m_pieces[sq] = C::VOID;
        if (hash != nullptr)
            *hash ^= zobrist_table[sq][C::VOID];
        update_internals_by_popped(popped, sq, hash);
        return popped;
    }
    Piece place_at(
        const Square& sq, const Piece& p, std::uint64_t* const hash = nullptr)
    {
        assert(sq < C::SQ_NA);
        const auto popped = m_pieces[sq];
        m_pieces[sq] = p;
        update_internals_by_placed(p, sq, hash);
        update_internals_by_popped(popped, sq, hash);
        return popped;
    }
    const char* set_sfen(const char* sfen)
    {
        for (uint ir = 0U; ir < C::num_ranks; ++ir) {
            assert(sfen[0] != ' ');
            sfen = set_sfen_rank(sfen, static_cast<Rank>(ir));
        }
        assert((sfen[0] == ' ') || (sfen[0] == '\0'));
        if (sfen[0] == ' ')
            ++sfen;
        update_internals_based_on_pieces();
        return sfen;
    }
    /**
     * @brief Get the attack map by the piece at the given square.
     * @note Do NOT pass `SQ_NA`.
     * @param sq Attacker piece location.
     * @return bitboard_t Attack map by the piece at the given square.
     */
    bitboard_t get_attack_at(const Square& sq) const
    {
        assert(sq < C::SQ_NA);
        const auto& p = m_pieces[sq];
        if (PT::is_slider(p))
            return BT::get_attack_by(p, sq, get_occupied());
        return BT::get_attack_by(p, sq);
    }
    Square find_attacker(
        const ColorEnum& attacker_color,
        const Square& attacked,
        const DirectionEnum& dir,
        const Square& skip = C::SQ_NA) const
    {
        auto ptr_sq = ST::ray_from(attacked, dir);
        const auto dir_rotated = vshogi::rotate(dir);
        {
            const Piece& p = m_pieces[*ptr_sq];
            if ((p != C::VOID) && (*ptr_sq != skip)) {
                if ((PT::get_color(p) == attacker_color)
                    && PT::is_attacking_to(p, dir_rotated))
                    return *ptr_sq;
                else
                    return C::SQ_NA;
            }
        }
        ++ptr_sq;
        for (; *ptr_sq != C::SQ_NA; ++ptr_sq) {
            const auto& sq = *ptr_sq;
            assert(sq != C::SQ_NA);
            const auto& p = m_pieces[sq];
            if ((p == C::VOID) || (sq == skip))
                continue;
            if ((PT::get_color(p) == attacker_color)
                && PT::slidable_to(p, dir_rotated))
                return sq;
            return C::SQ_NA;
        }
        return C::SQ_NA;
    }
    Square find_sliding_attacker(
        const ColorEnum& attacker_color,
        const Square& attacked,
        const DirectionEnum& dir,
        const Square& skip = C::SQ_NA) const
    {
        if ((dir == DIR_NA) || (attacked == C::SQ_NA))
            return C::SQ_NA;
        const auto dir_from_attacker = vshogi::rotate(dir);
        const auto ray = BT::get_ray_to(attacked, dir);
        if (!(ray & m_bb_color[attacker_color]))
            return C::SQ_NA;
        auto psq = ST::ray_from(attacked, dir);
        for (; *psq != C::SQ_NA; ++psq) {
            const auto& p = m_pieces[*psq];
            if ((p == C::VOID) || (*psq == skip))
                continue;
            if ((PT::get_color(p) == attacker_color)
                && PT::slidable_to(p, dir_from_attacker))
                return *psq;
            return C::SQ_NA;
        }
        return C::SQ_NA;
    }
    bitboard_t find_pinned(const ColorEnum& c) const
    {
        const Square& ally_king = m_kings[c];
        if (ally_king == C::SQ_NA)
            return static_cast<bitboard_t>(0);
        return find_sliding_attack_blockers(~c, c, ally_king);
    }
    bitboard_t find_cover(const ColorEnum& c) const
    {
        const Square& enemy_king = m_kings[~c];
        if (enemy_king == C::SQ_NA)
            return static_cast<bitboard_t>(0);
        return find_sliding_attack_blockers(c, c, enemy_king);
    }
    bitboard_t compute_king_movable(
        const ColorEnum& by_side,
        const bitboard_t& candidate = BT::full()) const;
    bitboard_t compute_movable_to(
        const Square& dst,
        const ColorEnum& by_side,
        const bitboard_t& src_mask) const
    {
        auto mask_slider = get_occupied_by_slider(by_side);
        auto mask_melee = m_bb_color[by_side] ^ mask_slider;
        const auto occ = get_occupied();
        mask_slider &= Magic<P>::get_adjacent_attack(dst, occ)
                       | Magic<P>::get_diagonal_attack(dst, occ);
        const auto neighbor
            = BT::get_attack_by(PT::make_piece(by_side, C::OU), dst);
        if constexpr (C::num_dir == 12u) {
            mask_melee
                &= neighbor | BT::shift(neighbor, by_side ? DIR_N : DIR_S);
        } else {
            mask_melee &= neighbor;
        }
        const auto candidates = src_mask & (mask_slider ^ mask_melee);
        bitboard_t out{};
        for (auto src : BT::iterator(candidates)) {
            if (BT::is_one(BT::get_attack_by(m_pieces[src], src), dst))
                out ^= BT::from_square(src);
        }
        return out;
    }
    Square drop_pawn_mate_square(
        const ColorEnum& by_side,
        const bitboard_t& candidate = BT::full()) const;
    Board hflip() const
    {
        Board out;
        for (auto sq : C::square_iterator()) {
            const auto sq_hflipped = ST::hflip(sq);
            assert(sq_hflipped != C::SQ_NA);
            out.m_pieces[sq_hflipped] = m_pieces[sq];
        }
        out.update_internals_based_on_pieces();
        return out;
    }
    Board rotate() const;
    static void init_tables()
    {
        std::random_device dev;
        std::mt19937_64 rng(dev());
        std::uniform_int_distribution<std::uint64_t> dist;
        for (auto sq : C::square_iterator()) {
            for (uint ii = 0u; ii < num_square_states; ++ii) {
                zobrist_table[sq][ii] = dist(rng);
            }
        }
    }
    std::uint64_t zobrist_hash() const
    {
        std::uint64_t out = static_cast<std::uint64_t>(0);
        for (auto sq = C::num_squares; sq--;) {
            out ^= zobrist_table[sq][m_pieces[sq]];
        }
        return out;
    }

private:
    const char* set_sfen_rank(const char* const sfen_rank, const Rank rank)
    {
        constexpr int max_length = 19; // e.g. "+p+p+p+p+p+p+p+p+p/"
        auto piece_ptr
            = m_pieces.data() + rank + (C::num_files - 1u) * C::num_ranks;
        const char* sfen_ptr = sfen_rank;
        bool promotion_flag = false;
        for (; sfen_ptr < sfen_rank + max_length; ++sfen_ptr) {
            switch (*sfen_ptr) {
            case '/':
                ++sfen_ptr;
            case ' ':
            case '\0':
                goto OUT_OF_LOOP;
            case '9':
                *piece_ptr = C::VOID;
                piece_ptr -= C::num_ranks; // fall-through
            case '8':
                *piece_ptr = C::VOID;
                piece_ptr -= C::num_ranks; // fall-through
            case '7':
                *piece_ptr = C::VOID;
                piece_ptr -= C::num_ranks; // fall-through
            case '6':
                *piece_ptr = C::VOID;
                piece_ptr -= C::num_ranks; // fall-through
            case '5':
                *piece_ptr = C::VOID;
                piece_ptr -= C::num_ranks; // fall-through
            case '4':
                *piece_ptr = C::VOID;
                piece_ptr -= C::num_ranks; // fall-through
            case '3':
                *piece_ptr = C::VOID;
                piece_ptr -= C::num_ranks; // fall-through
            case '2':
                *piece_ptr = C::VOID;
                piece_ptr -= C::num_ranks; // fall-through
            case '1':
                *piece_ptr = C::VOID;
                piece_ptr -= C::num_ranks;
                break;
            case '+':
                promotion_flag = true;
                continue;
            default:
                *piece_ptr
                    = promotion_flag
                          ? PT::promote_nocheck(PT::make_piece(*sfen_ptr))
                          : PT::make_piece(*sfen_ptr);
                piece_ptr -= C::num_ranks;
                break;
            }
            promotion_flag = false;
        }
    OUT_OF_LOOP:
        assert(piece_ptr + C::num_ranks == m_pieces.data() + rank);
        return sfen_ptr;
    }
    void update_internals_based_on_pieces()
    {
        m_kings[BLACK] = C::SQ_NA;
        m_kings[WHITE] = C::SQ_NA;
        std::fill_n(m_bb_color, num_colors, static_cast<bitboard_t>(0));
        std::fill_n(m_bb_piece, C::num_piece_types, static_cast<bitboard_t>(0));
        std::fill_n(m_bb_slider, num_colors, static_cast<bitboard_t>(0));
        for (auto sq : C::square_iterator()) {
            const auto& p = m_pieces[sq];
            const auto c = PT::get_color(p);
            const auto pt = PT::to_piece_type(p);
            if (pt == C::OU)
                m_kings[c] = sq;
            const auto m = BT::from_square(sq);
            if (p != C::VOID) {
                m_bb_color[c] ^= m;
                m_bb_piece[pt] ^= m;
            }
            if (PT::is_slider(pt))
                m_bb_slider[c] ^= m;
        }
    }
    void update_internals_by_placed(
        const Piece& p, const Square& sq, std::uint64_t* const hash = nullptr)
    {
        assert(sq < C::SQ_NA);
        assert(p < num_square_states);
        if (hash != nullptr)
            *hash ^= zobrist_table[sq][p];
        if (p == C::VOID)
            return;
        const auto c = PT::get_color(p);
        const auto pt = PT::to_piece_type(p);
        if (pt == C::OU)
            m_kings[c] = sq;
        const auto m = BT::from_square(sq);
        m_bb_color[c] ^= m;
        m_bb_piece[pt] ^= m;
        if (PT::is_slider(pt))
            m_bb_slider[c] ^= m;
    }
    void update_internals_by_popped(
        const Piece& p, const Square& sq, std::uint64_t* const hash = nullptr)
    {
        assert(sq < C::SQ_NA);
        if (hash != nullptr)
            *hash ^= zobrist_table[sq][p];
        if (p == C::VOID)
            return;
        const auto c = PT::get_color(p);
        const auto pt = PT::to_piece_type(p);
        if (pt == C::OU)
            m_kings[c] = C::SQ_NA;
        const auto m = BT::from_square(sq);
        m_bb_color[c] ^= m;
        m_bb_piece[pt] ^= m;
        if (PT::is_slider(pt))
            m_bb_slider[c] ^= m;
    }
    bitboard_t find_sliding_attack_blockers(
        const ColorEnum& attack_by,
        const ColorEnum& block_by,
        const Square& target) const
    {
        bitboard_t out{};
        auto attackers = get_occupied_by_slider(attack_by);
        attackers &= Magic<P>::get_adjacent_attack(target)
                     | Magic<P>::get_diagonal_attack(target);
        for (auto atk : BT::iterator(attackers)) {
            const auto target_dir = ST::direction(atk, target);
            if (!PT::slidable_to(m_pieces[atk], target_dir))
                continue;
            auto blockers = BT::get_mask_between(atk, target);
            blockers &= (m_bb_color[BLACK] | m_bb_color[WHITE]);
            if (hamming_weight(blockers) != 1u)
                continue;
            const auto blocker = *BT::iterator(blockers);
            if (PT::get_color(m_pieces[blocker]) == block_by)
                out ^= BT::from_square(blocker);
        }
        return out;
    }
    bool enemy_can_capture_the_drop_pawn(
        const Square& dst, const ColorEnum& by_side) const
    {
        const auto enemy_king_sq = m_kings[~by_side];
        const auto enemy_king_dir = (by_side == BLACK) ? DIR_N : DIR_S;
        for (auto dir : C::direction_iterator()) {
            if (dir == enemy_king_dir)
                continue;
            const auto src_next = find_attacker(~by_side, dst, dir);
            const bool is_attacking_the_pawn = (src_next != C::SQ_NA);
            if (is_attacking_the_pawn) {
                const auto discovered_dir
                    = ST::direction(enemy_king_sq, src_next);
                const auto discovered_attacker_sq = find_sliding_attacker(
                    by_side, enemy_king_sq, discovered_dir, src_next);
                const auto is_pinned = (discovered_attacker_sq != C::SQ_NA);
                if (!is_pinned)
                    return true;
            }
        }
        return false;
    }
};

template <class P>
Board<P> Board<P>::rotate() const
{
    Board out{};
    for (auto sq : C::square_iterator()) {
        const auto sq_rotated = ST::rotate(sq);
        assert(sq_rotated != C::SQ_NA);
        const auto pt = PT::to_piece_type(m_pieces[sq]);
        const auto c = PT::get_color(m_pieces[sq]);
        out.m_pieces[sq_rotated] = PT::make_piece(~c, pt);
    }
    out.update_internals_based_on_pieces();
    return out;
}

template <class P>
typename Configuration<P>::bitboard_t Board<P>::compute_king_movable(
    const ColorEnum& by_side, const bitboard_t& candidate) const
{
    const auto src = m_kings[by_side];
    if (src == C::SQ_NA)
        return static_cast<bitboard_t>(0);
    assert(m_pieces[src] == PT::make_piece(by_side, C::OU));

    auto out = BT::get_attack_by(m_pieces[src], src) & candidate;
    out &= ~m_bb_color[by_side]; // leading bits are 0s for sure.
    if (out == 0u)
        return out;

    const auto occ_full_but_king = get_occupied() ^ BT::from_square(src);
    auto atks = get_occupied_by_slider(~by_side);
    const auto melee = m_bb_color[~by_side] ^ atks;
    auto neighbor = BT::get_pre_reverse_attack(src, m_pieces[src]);
    if constexpr (C::num_dir == 12u)
        neighbor |= BT::shift(neighbor, by_side ? DIR_S : DIR_N);
    atks ^= melee & neighbor;
    for (auto sq : BT::iterator(atks)) {
        out &= ~BT::get_attack_by(m_pieces[sq], sq, occ_full_but_king);
        if (out == 0u)
            return out;
    }
    return out;
}

template <class P>
typename Configuration<P>::Square Board<P>::drop_pawn_mate_square(
    const ColorEnum& by_side, const bitboard_t& candidate) const
{
    const auto sq = ST::shift(m_kings[~by_side], by_side ? DIR_N : DIR_S);
    if ((sq == C::SQ_NA) || !BT::is_one(candidate, sq)
        || compute_king_movable(~by_side)
        || enemy_can_capture_the_drop_pawn(sq, by_side))
        return C::SQ_NA;
    return sq;
}

} // namespace vshogi

#endif // VSHOGI_BOARD_HPP
