#ifndef VSHOGI_BOARD_HPP
#define VSHOGI_BOARD_HPP

#include <cassert>
#include <random>
#include <string>

#include "vshogi/common/bitboard.hpp"
#include "vshogi/common/color.hpp"
#include "vshogi/common/config.hpp"
#include "vshogi/common/direction.hpp"
#include "vshogi/common/magic.hpp"
#include "vshogi/common/move.hpp"
#include "vshogi/common/pieces.hpp"
#include "vshogi/common/squares.hpp"

namespace vshogi
{

template <class Parameters>
class Board
{
private:
    using C = Configuration<Parameters>;
    using SHelper = Squares<Parameters>;
    using PHelper = Pieces<Parameters>;
    using BitBoardType = BitBoard<Parameters>;
    using MoveType = Move<Parameters>;
    using PieceType = typename C::PieceType;
    using ColoredPiece = typename C::ColoredPiece;
    using Square = typename C::Square;
    using File = typename C::File;
    using Rank = typename C::Rank;
    static constexpr uint num_square_states = C::num_colored_piece_types + 1u;
    static std::uint64_t zobrist_table[C::num_squares][num_square_states];

private:
    std::array<ColoredPiece, C::num_squares> m_pieces;
    Square m_king_locations[num_colors];
    BitBoardType m_bb_color[num_colors];
    BitBoardType m_bb_piece[C::num_piece_types];

public:
    Board()
        : m_pieces(C::initial_position), m_king_locations{}, m_bb_color{},
          m_bb_piece{}
    {
        update_internals_based_on_pieces();
    }
    Board(const char* const sfen)
    {
        set_sfen(sfen);
    }
    ColoredPiece get(const Square& sq) const
    {
        assert(sq != C::SQ_NA);
        return m_pieces[sq];
    }
    ColoredPiece operator[](const Square& sq) const
    {
        assert(sq != C::SQ_NA);
        return m_pieces[sq];
    }
    bool is_empty(const Square& sq) const
    {
        assert(sq != C::SQ_NA);
        return (m_pieces[sq] == C::VOID);
    }
    Square get_king_location(const ColorEnum& c) const
    {
        return m_king_locations[c];
    }
    BitBoardType get_occupied() const
    {
        return m_bb_color[BLACK] ^ m_bb_color[WHITE];
    }
    BitBoardType get_occupied(const ColorEnum& c) const
    {
        return m_bb_color[c];
    }
    /**
     * @brief Get occupation by the given piece type.
     *
     * @param pt Piece type. Note that `NA` is not allowed here.
     * @return BitBoardType Occupation by the given piece type.
     */
    const BitBoardType& get_occupied(const PieceType& pt) const
    {
        assert(pt != C::NA);
        return m_bb_piece[pt];
    }
    /**
     * @brief Get occupation by the given colored piece.
     *
     * @param p Colored piece. Note that `C::VOID` is not allowed here.
     * @return BitBoardType Occupation by the given colored piece.
     */
    BitBoardType get_occupied(const ColoredPiece& p) const
    {
        assert(p != C::VOID);
        const auto c = PHelper::get_color(p);
        const auto pt = PHelper::to_piece_type(p);
        return m_bb_color[c] & m_bb_piece[pt];
    }
    template <PieceType PT>
    BitBoardType get_occupied(const ColorEnum& c) const
    {
        static_assert(PT != C::NA);
        return m_bb_color[c] & m_bb_piece[PT];
    }
    template <PieceType PT1, PieceType PT2, PieceType... Args>
    BitBoardType get_occupied(const ColorEnum& c) const
    {
        return get_occupied<PT1>(c) | get_occupied<PT2, Args...>(c);
    }
    BitBoardType get_occupied_by_ranging(const ColorEnum& c) const
    {
        BitBoardType out{};
        for (PieceType pt : EnumIterator<PieceType, C::num_piece_types>()) {
            if (FPTHelper::is_ranging(C::piece_types[pt]))
                out |= (m_bb_color[c] & m_bb_piece[pt]);
        }
        return out;
    }
    void append_sfen(std::string& out) const
    {
        append_sfen_rank(static_cast<Rank>(0), out);
        for (uint ir = 1; ir < C::num_ranks; ++ir) {
            out += '/';
            append_sfen_rank(static_cast<Rank>(ir), out);
        }
    }
    ColoredPiece apply(
        const Square& dst,
        const ColoredPiece& p,
        std::uint64_t* const hash = nullptr)
    {
        const ColoredPiece popped = place_piece_on(dst, p);
        update_internals_by_placed(p, dst, hash);
        update_internals_by_popped(popped, dst, hash);
        return popped;
    }
    ColoredPiece apply(
        const Square& dst,
        const Square& src,
        const bool& promote = false,
        std::uint64_t* const hash = nullptr)
    {
        ColoredPiece moving_piece = place_piece_on(src, C::VOID);
        update_internals_by_placed(C::VOID, src, hash);
        update_internals_by_popped(moving_piece, src, hash);
        if (promote)
            moving_piece = PHelper::promote_nocheck(moving_piece);
        return apply(dst, moving_piece, hash);
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
    BitBoardType get_attacks_by_nocheck(const Square& sq) const
    {
        assert(sq < C::SQ_NA);
        return BitBoardType::get_attacks_by(m_pieces[sq], sq, get_occupied());
    }
    Square find_attacker(
        const ColorEnum& attacker_color,
        const Square& attacked,
        const DirectionEnum& dir,
        const Square& skip = C::SQ_NA) const
    {
        auto ptr_sq = SHelper::get_squares_along(dir, attacked);
        if (ptr_sq == nullptr)
            return C::SQ_NA;
        if (!(BitBoardType::get_ray_to(attacked, dir)
              & m_bb_color[attacker_color])
                 .any())
            return C::SQ_NA;
        {
            const ColoredPiece& p = m_pieces[*ptr_sq];
            if ((p != C::VOID) && (*ptr_sq != skip)) {
                if ((PHelper::get_color(p) == attacker_color)
                    && BitBoardType::get_attacks_by(p, *ptr_sq)
                           .is_one(attacked))
                    return *ptr_sq;
                else
                    return C::SQ_NA;
            }
        }

        const auto dir_rotated = rotate(dir);
        ++ptr_sq;
        for (; *ptr_sq != C::SQ_NA; ++ptr_sq) {
            const auto& sq = *ptr_sq;
            assert(sq != C::SQ_NA);
            const auto& p = m_pieces[sq];
            if ((p == C::VOID) || (sq == skip))
                continue;
            if ((PHelper::get_color(p) == attacker_color)
                && PHelper::is_ranging_to(p, dir_rotated))
                return sq;
            return C::SQ_NA;
        }
        return C::SQ_NA;
    }
    Square find_ranging_attacker(
        const ColorEnum& attacker_color,
        const Square& attacked,
        const DirectionEnum& dir,
        const Square& skip = C::SQ_NA) const
    {
        if ((dir == DIR_NA) || (attacked == C::SQ_NA))
            return C::SQ_NA;
        const auto dir_from_attacker = rotate(dir);
        const auto ray = BitBoardType::get_ray_to(attacked, dir);
        if (!(ray & m_bb_color[attacker_color]).any())
            return C::SQ_NA;
        auto psq = SHelper::get_squares_along(dir, attacked);
        if (psq == nullptr)
            return C::SQ_NA;
        for (; *psq != C::SQ_NA; ++psq) {
            const auto& p = m_pieces[*psq];
            if ((p == C::VOID) || (*psq == skip))
                continue;
            if ((PHelper::get_color(p) == attacker_color)
                && PHelper::is_ranging_to(p, dir_from_attacker))
                return *psq;
            return C::SQ_NA;
        }
        return C::SQ_NA;
    }
    BitBoardType find_pinned(const ColorEnum& c) const
    {
        const Square& ally_king = m_king_locations[c];
        if (ally_king == C::SQ_NA)
            return BitBoardType();
        return find_ranging_attack_blockers(~c, c, ally_king);
    }
    BitBoardType find_cover(const ColorEnum& c) const
    {
        const Square& enemy_king = m_king_locations[~c];
        if (enemy_king == C::SQ_NA)
            return BitBoardType();
        return find_ranging_attack_blockers(c, c, enemy_king);
    }
    BitBoardType compute_king_movable(
        const ColorEnum& by_side, const BitBoardType& movable) const
    {
        const auto src = m_king_locations[by_side];
        if (src == C::SQ_NA)
            return BitBoardType();
        assert(m_pieces[src] == PHelper::to_board_piece(by_side, C::OU));
        BitBoardType out
            = BitBoardType::get_attacks_by(m_pieces[src], src) & movable;
        const BitBoardType occ_full_but_king = get_occupied().clear(src);
        out &= ~m_bb_color[by_side];
        clear_mask_where_attacked(out, ~by_side, occ_full_but_king);
        return out;
    }
    BitBoardType compute_movable_to(
        const Square& dst,
        const ColorEnum& by_side,
        const BitBoardType& src_mask) const
    {
        const auto occ = get_occupied();
        const auto mask_ranging = get_occupied_by_ranging(by_side);
        const auto mask_8dir
            = (Magic<Parameters>::get_adjacent_attack(dst, occ)
               | Magic<Parameters>::get_diagonal_attack(dst, occ));
        const auto mask_melee = (m_bb_color[by_side] ^ mask_ranging)
                                & BitBoardType::compute_neighbor5x5(dst);
        const auto candidates
            = src_mask & ((mask_ranging & mask_8dir) ^ mask_melee);
        BitBoardType out{};
        for (auto src : candidates.square_iterator()) {
            if (BitBoardType::get_attacks_by(m_pieces[src], src).is_one(dst))
                out.toggle(src);
        }
        return out;
    }
    template <bool Check>
    BitBoardType compute_droppable(const ColoredPiece& p) const
    {
        const auto occ_full = get_occupied();
        auto droppable = ~occ_full;
        update_droppable<Check>(droppable, p, occ_full);
        return droppable;
    }
    template <bool Check>
    BitBoardType
    compute_droppable(const ColoredPiece& p, BitBoardType droppable) const
    {
        const auto occ_full = get_occupied();
        droppable &= ~occ_full;
        update_droppable<Check>(droppable, p, occ_full);
        return droppable;
    }
    Board hflip() const
    {
        Board out;
        for (auto sq : EnumIterator<Square, C::num_squares>()) {
            const auto sq_hflipped = SHelper::hflip(sq);
            assert(sq_hflipped != C::SQ_NA);
            out.m_pieces[sq_hflipped] = m_pieces[sq];
        }
        out.update_internals_based_on_pieces();
        return out;
    }
    static void init_tables()
    {
        std::random_device dev;
        std::mt19937_64 rng(dev());
        std::uniform_int_distribution<std::uint64_t> dist;
        for (auto sq : EnumIterator<Square, C::num_squares>()) {
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
                *piece_ptr = promotion_flag
                                 ? PHelper::promote_nocheck(
                                     PHelper::to_board_piece(*sfen_ptr))
                                 : PHelper::to_board_piece(*sfen_ptr);
                piece_ptr -= C::num_ranks;
                break;
            }
            promotion_flag = false;
        }
    OUT_OF_LOOP:
        assert(piece_ptr + C::num_ranks == m_pieces.data() + rank);
        return sfen_ptr;
    }
    void append_sfen_rank(const Rank rank, std::string& out) const
    {
        auto ptr = m_pieces.data() + rank + (C::num_files - 1u) * C::num_ranks;
        int num_void = 0;
        for (; ptr >= m_pieces.data(); ptr -= C::num_ranks) {
            if (*ptr == C::VOID) {
                ++num_void;
                continue;
            }
            if (num_void > 0) {
                out += static_cast<char>('0' + num_void);
                num_void = 0;
            }
            PHelper::append_sfen(*ptr, out);
        }
        if (num_void > 0)
            out += static_cast<char>('0' + num_void);
    }
    ColoredPiece place_piece_on(const Square& sq, const ColoredPiece& p)
    {
        assert(sq < C::SQ_NA);
        const auto out = m_pieces[sq];
        m_pieces[sq] = p;
        return out;
    }
    void update_internals_based_on_pieces()
    {
        m_king_locations[BLACK] = C::SQ_NA;
        m_king_locations[WHITE] = C::SQ_NA;
        std::fill_n(m_bb_color, num_colors, BitBoardType());
        std::fill_n(m_bb_piece, C::num_piece_types, BitBoardType());
        for (auto sq : EnumIterator<Square, C::num_squares>()) {
            const auto& p = m_pieces[sq];
            const auto c = PHelper::get_color(p);
            const auto pt = PHelper::to_piece_type(p);
            if (pt == C::OU)
                m_king_locations[c] = sq;
            if (p != C::VOID) {
                m_bb_color[c].toggle(sq);
                m_bb_piece[pt].toggle(sq);
            }
        }
    }
    void update_internals_by_placed(
        const ColoredPiece& p,
        const Square& sq,
        std::uint64_t* const hash = nullptr)
    {
        assert(sq < C::SQ_NA);
        assert(p < num_square_states);
        if (hash != nullptr)
            *hash ^= zobrist_table[sq][p];
        if (p == C::VOID)
            return;
        const auto c = PHelper::get_color(p);
        const auto pt = PHelper::to_piece_type(p);
        if (pt == C::OU)
            m_king_locations[c] = sq;
        m_bb_color[c].toggle(sq);
        m_bb_piece[pt].toggle(sq);
    }
    void update_internals_by_popped(
        const ColoredPiece& p,
        const Square& sq,
        std::uint64_t* const hash = nullptr)
    {
        assert(sq < C::SQ_NA);
        if (hash != nullptr)
            *hash ^= zobrist_table[sq][p];
        if (p == C::VOID)
            return;
        const auto c = PHelper::get_color(p);
        const auto pt = PHelper::to_piece_type(p);
        if (pt == C::OU)
            m_king_locations[c] = C::SQ_NA;
        m_bb_color[c].toggle(sq);
        m_bb_piece[pt].toggle(sq);
    }
    BitBoardType find_ranging_attack_blockers(
        const ColorEnum& attack_by,
        const ColorEnum& block_by,
        const Square& target) const
    {
        BitBoardType out{};
        const BitBoardType occ_full = m_bb_color[BLACK] | m_bb_color[WHITE];
        const auto ranger = get_occupied_by_ranging(attack_by);
        const auto mask_8dir = Magic<Parameters>::get_adjacent_attack(target)
                               | Magic<Parameters>::get_diagonal_attack(target);
        const auto mask_attackers = ranger & mask_8dir;
        for (auto atk : mask_attackers.square_iterator()) {
            const auto king_dir = SHelper::get_direction(target, atk);
            if (!PHelper::is_ranging_to(m_pieces[atk], king_dir))
                continue;
            const auto segment = BitBoardType::get_line_segment(atk, target);
            const BitBoardType blockers = occ_full & segment;
            if (blockers.hamming_weight() != 1u)
                continue;
            const auto blocker = *blockers.square_iterator();
            if (PHelper::get_color(m_pieces[blocker]) == block_by)
                out.set(blocker);
        }
        return out;
    }
    void clear_mask_where_attacked(
        BitBoardType& mask,
        const ColorEnum& by_side,
        const BitBoardType& occ_full_but_king) const
    {
        const BitBoardType occ_ranging = get_occupied_by_ranging(by_side);
        const BitBoardType occ_melee = m_bb_color[by_side] ^ occ_ranging;
        const BitBoardType neighbor5x5
            = BitBoardType::compute_neighbor5x5(m_king_locations[~by_side]);
        const BitBoardType neighbor
            = neighbor5x5
              | neighbor5x5.shift((by_side == BLACK) ? DIR_S : DIR_N);
        const BitBoardType occ_atks = occ_ranging ^ (neighbor & occ_melee);
        for (auto sq : occ_atks.square_iterator()) {
            const auto& p = m_pieces[sq];
            if (PHelper::is_ranging_piece(p)
                && (BitBoardType::get_attacks_by(p, sq) & mask).any()) {
                mask &= ~BitBoardType::get_attacks_by(p, sq, occ_full_but_king);
            } else {
                mask &= ~BitBoardType::get_attacks_by(p, sq);
            }
            if (mask.empty())
                return;
        }
    }
    template <bool Check>
    void update_droppable(
        BitBoardType& droppable,
        const ColoredPiece& p,
        const BitBoardType& occ_full) const
    {
        if constexpr (Check) {
            const auto pt = PHelper::to_piece_type(p);
            const auto c = PHelper::get_color(p);
            const Square& target = m_king_locations[~c];
            droppable &= BitBoardType::get_attacks_by(
                PHelper::to_board_piece(~c, pt), target, occ_full);
            if (pt == C::FU) {
                if (!droppable.any())
                    return;
                if (has_pawn_in_file(SHelper::to_file(target), c)
                    || (can_drop_pawn_mate(c)))
                    droppable &= BitBoardType();
            }
        } else {
            droppable &= BitBoardType::compute_droppable(p);
            if (PHelper::to_piece_type(p) == C::FU) {
                const auto c = PHelper::get_color(p);
                exclude_two_pawns_in_a_file(droppable, c);
                exclude_drop_pawn_mate(droppable, c);
            }
        }
    }
    void exclude_two_pawns_in_a_file(
        BitBoardType& occ, const ColorEnum& by_side) const
    {
        for (auto f : EnumIterator<File, C::num_files>()) {
            if (has_pawn_in_file(f, by_side))
                occ &= ~BitBoardType::from_file(f);
        }
    }
    bool has_pawn_in_file(const File& f, const ColorEnum& by_side) const
    {
        const BitBoardType occ = get_occupied<C::FU>(by_side);
        return (BitBoardType::from_file(f) & occ).any();
    }
    bool can_drop_pawn_mate(const ColorEnum& by_side) const
    {
        const Square dst = SHelper::shift(
            m_king_locations[~by_side], (by_side == BLACK) ? DIR_S : DIR_N);
        if (dst == C::SQ_NA)
            return false;
        if (king_can_avoid_a_pawn_attack(~by_side))
            return false;
        if (enemy_can_capture_the_drop_pawn(dst, by_side))
            return false;
        return true;
    }
    void
    exclude_drop_pawn_mate(BitBoardType& occ, const ColorEnum& by_side) const
    {
        const Square dst = SHelper::shift(
            m_king_locations[~by_side], (by_side == BLACK) ? DIR_S : DIR_N);
        if (dst == C::SQ_NA)
            return;
        if (!occ.is_one(dst))
            return;
        if (king_can_avoid_a_pawn_attack(~by_side))
            return;
        if (enemy_can_capture_the_drop_pawn(dst, by_side))
            return;
        occ.clear(dst);
    }
    bool is_pawn_attacking_to_enemy_king(
        const Square& sq, const ColorEnum& by_side) const
    {
        const auto enemy_king_sq = m_king_locations[~by_side];
        if (enemy_king_sq == C::SQ_NA)
            return false;
        return enemy_king_sq
               == SHelper::shift(sq, (by_side == BLACK) ? DIR_N : DIR_S);
    }
    bool king_can_avoid_a_pawn_attack(const ColorEnum& king_color) const
    {
        return compute_king_movable(king_color, ~BitBoardType()).any();
    }
    bool enemy_can_capture_the_drop_pawn(
        const Square& dst, const ColorEnum& by_side) const
    {
        const auto enemy_king_sq = m_king_locations[~by_side];
        const auto enemy_king_dir = (by_side == BLACK) ? DIR_N : DIR_S;
        for (auto dir : EnumIterator<DirectionEnum, C::num_dir>()) {
            if (dir == enemy_king_dir)
                continue;
            const auto src_next = find_attacker(~by_side, dst, dir);
            const bool is_attacking_the_pawn = (src_next != C::SQ_NA);
            if (is_attacking_the_pawn) {
                const auto discovered_dir
                    = SHelper::get_direction(src_next, enemy_king_sq);
                const auto discovered_attacker_sq = find_ranging_attacker(
                    by_side, enemy_king_sq, discovered_dir, src_next);
                const auto is_pinned = (discovered_attacker_sq != C::SQ_NA);
                if (!is_pinned)
                    return true;
            }
        }
        return false;
    }
};

} // namespace vshogi

#endif // VSHOGI_BOARD_HPP
