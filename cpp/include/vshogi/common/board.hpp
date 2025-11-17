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

template <class P>
class Board
{
private:
    using C = Configuration<P>;
    using SHelper = Squares<P>;
    using PHelper = Pieces<P>;
    using PieceType = typename C::PieceType;
    using ColoredPiece = typename C::ColoredPiece;
    using Square = typename C::Square;
    using File = typename C::File;
    using Rank = typename C::Rank;
    static constexpr uint num_square_states = C::num_colored_piece_types + 1u;
    static std::uint64_t zobrist_table[C::num_squares][num_square_states];

private:
    std::array<ColoredPiece, C::num_squares> m_pieces;
    Square m_kings[num_colors];
    BitBoard<P> m_bb_color[num_colors];
    BitBoard<P> m_bb_piece[C::num_piece_types];
    BitBoard<P> m_bb_slider[num_colors];

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
    Square get_king_square(const ColorEnum& c) const
    {
        return m_kings[c];
    }
    BitBoard<P> get_occupied() const
    {
        return m_bb_color[BLACK] ^ m_bb_color[WHITE];
    }
    BitBoard<P> get_occupied(const ColorEnum& c) const
    {
        return m_bb_color[c];
    }
    /**
     * @brief Get occupation by the given piece type.
     *
     * @param pt Piece type. Note that `NA` is not allowed here.
     * @return BitBoard<P> Occupation by the given piece type.
     */
    const BitBoard<P>& get_occupied(const PieceType& pt) const
    {
        assert(pt != C::NA);
        return m_bb_piece[pt];
    }
    /**
     * @brief Get occupation by the given colored piece.
     *
     * @param p Colored piece. Note that `C::VOID` is not allowed here.
     * @return BitBoard<P> Occupation by the given colored piece.
     */
    BitBoard<P> get_occupied(const ColoredPiece& p) const
    {
        assert(p != C::VOID);
        const auto c = PHelper::get_color(p);
        const auto pt = PHelper::to_piece_type(p);
        return m_bb_color[c] & m_bb_piece[pt];
    }
    template <PieceType PT>
    BitBoard<P> get_occupied(const ColorEnum& c) const
    {
        static_assert(PT != C::NA);
        return m_bb_color[c] & m_bb_piece[PT];
    }
    template <PieceType PT1, PieceType PT2, PieceType... Args>
    BitBoard<P> get_occupied(const ColorEnum& c) const
    {
        return get_occupied<PT1>(c) | get_occupied<PT2, Args...>(c);
    }
    const BitBoard<P>& get_occupied_by_slider(const ColorEnum& c) const
    {
        return m_bb_slider[c];
    }
    ColoredPiece pop_from(const Square& sq, std::uint64_t* const hash = nullptr)
    {
        const auto popped = m_pieces[sq];
        m_pieces[sq] = C::VOID;
        if (hash != nullptr)
            *hash ^= zobrist_table[sq][C::VOID];
        update_internals_by_popped(popped, sq, hash);
        return popped;
    }
    ColoredPiece place_at(
        const Square& sq,
        const ColoredPiece& p,
        std::uint64_t* const hash = nullptr)
    {
        assert(sq < C::SQ_NA);
        const auto popped = m_pieces[sq];
        m_pieces[sq] = p;
        update_internals_by_placed(p, sq, hash);
        update_internals_by_popped(popped, sq, hash);
        return popped;
    }
    void append_sfen(std::string& out) const
    {
        append_sfen_rank(static_cast<Rank>(0), out);
        for (uint ir = 1; ir < C::num_ranks; ++ir) {
            out += '/';
            append_sfen_rank(static_cast<Rank>(ir), out);
        }
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
    BitBoard<P> get_attacks_by_nocheck(const Square& sq) const
    {
        assert(sq < C::SQ_NA);
        const auto& p = m_pieces[sq];
        if (PHelper::is_slider(p))
            return BitBoard<P>::get_attacks_by(p, sq, get_occupied());
        return BitBoard<P>::get_attacks_by(p, sq);
    }
    Square find_attacker(
        const ColorEnum& attacker_color,
        const Square& attacked,
        const DirectionEnum& dir,
        const Square& skip = C::SQ_NA) const
    {
        auto ptr_sq = SHelper::ray_from(attacked, dir);
        if (ptr_sq == nullptr)
            return C::SQ_NA;
        const auto dir_rotated = rotate(dir);
        {
            const ColoredPiece& p = m_pieces[*ptr_sq];
            if ((p != C::VOID) && (*ptr_sq != skip)) {
                if ((PHelper::get_color(p) == attacker_color)
                    && PHelper::is_attacking_to(p, dir_rotated))
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
            if ((PHelper::get_color(p) == attacker_color)
                && PHelper::slidable_to(p, dir_rotated))
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
        const auto dir_from_attacker = rotate(dir);
        const auto ray = BitBoard<P>::get_ray_to(attacked, dir);
        if (!(ray & m_bb_color[attacker_color]).any())
            return C::SQ_NA;
        auto psq = SHelper::ray_from(attacked, dir);
        if (psq == nullptr)
            return C::SQ_NA;
        for (; *psq != C::SQ_NA; ++psq) {
            const auto& p = m_pieces[*psq];
            if ((p == C::VOID) || (*psq == skip))
                continue;
            if ((PHelper::get_color(p) == attacker_color)
                && PHelper::slidable_to(p, dir_from_attacker))
                return *psq;
            return C::SQ_NA;
        }
        return C::SQ_NA;
    }
    BitBoard<P> find_pinned(const ColorEnum& c) const
    {
        const Square& ally_king = m_kings[c];
        if (ally_king == C::SQ_NA)
            return BitBoard<P>();
        return find_sliding_attack_blockers(~c, c, ally_king);
    }
    BitBoard<P> find_cover(const ColorEnum& c) const
    {
        const Square& enemy_king = m_kings[~c];
        if (enemy_king == C::SQ_NA)
            return BitBoard<P>();
        return find_sliding_attack_blockers(c, c, enemy_king);
    }
    BitBoard<P> compute_king_movable(
        const ColorEnum& by_side, const BitBoard<P>& movable) const
    {
        const auto src = m_kings[by_side];
        if (src == C::SQ_NA)
            return BitBoard<P>();
        assert(m_pieces[src] == PHelper::to_board_piece(by_side, C::OU));
        BitBoard<P> out
            = BitBoard<P>::get_attacks_by(m_pieces[src], src) & movable;
        const BitBoard<P> occ_full_but_king = get_occupied().clear(src);
        out &= ~m_bb_color[by_side];
        clear_mask_where_attacked(out, ~by_side, occ_full_but_king);
        return out;
    }
    BitBoard<P> compute_movable_to(
        const Square& dst,
        const ColorEnum& by_side,
        const BitBoard<P>& src_mask) const
    {
        auto mask_slider = get_occupied_by_slider(by_side);
        auto mask_melee = m_bb_color[by_side] ^ mask_slider;
        const auto occ = get_occupied();
        mask_slider
            &= (Magic<P>::get_adjacent_attack(dst, occ)
                | Magic<P>::get_diagonal_attack(dst, occ));
        mask_melee &= BitBoard<P>::get_neighbor_at(dst, by_side);
        const auto candidates = src_mask & (mask_slider ^ mask_melee);
        BitBoard<P> out{};
        for (auto src : candidates.iterator()) {
            if (BitBoard<P>::get_attacks_by(m_pieces[src], src).is_one(dst))
                out.toggle(src);
        }
        return out;
    }
    template <bool Check>
    BitBoard<P> compute_droppable(const ColoredPiece& p) const
    {
        const auto occ_full = get_occupied();
        auto droppable = ~occ_full;
        update_droppable<Check>(droppable, p, occ_full);
        return droppable;
    }
    template <bool Check>
    BitBoard<P>
    compute_droppable(const ColoredPiece& p, BitBoard<P> droppable) const
    {
        const auto occ_full = get_occupied();
        droppable &= ~occ_full;
        update_droppable<Check>(droppable, p, occ_full);
        return droppable;
    }
    bool has_pawn_in_file(const File& f, const ColorEnum& by_side) const
    {
        const BitBoard<P> occ = get_occupied<C::FU>(by_side);
        return (BitBoard<P>::from_file(f) & occ).any();
    }
    bool
    is_drop_pawn_mate_square(const Square dst, const ColorEnum by_side) const
    {
        const Square sq = SHelper::shift(
            m_kings[~by_side], (by_side == BLACK) ? DIR_S : DIR_N);
        if ((sq == C::SQ_NA) || (sq != dst)
            || king_can_avoid_a_pawn_attack(~by_side)
            || enemy_can_capture_the_drop_pawn(sq, by_side))
            return false;
        return true;
    }
    Board hflip() const
    {
        Board out;
        for (auto sq : C::square_iterator()) {
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
    void update_internals_based_on_pieces()
    {
        m_kings[BLACK] = C::SQ_NA;
        m_kings[WHITE] = C::SQ_NA;
        std::fill_n(m_bb_color, num_colors, BitBoard<P>());
        std::fill_n(m_bb_piece, C::num_piece_types, BitBoard<P>());
        std::fill_n(m_bb_slider, num_colors, BitBoard<P>());
        for (auto sq : C::square_iterator()) {
            const auto& p = m_pieces[sq];
            const auto c = PHelper::get_color(p);
            const auto pt = PHelper::to_piece_type(p);
            if (pt == C::OU)
                m_kings[c] = sq;
            if (p != C::VOID) {
                m_bb_color[c].toggle(sq);
                m_bb_piece[pt].toggle(sq);
            }
            if (PHelper::is_slider(pt))
                m_bb_slider[c].toggle(sq);
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
            m_kings[c] = sq;
        m_bb_color[c].toggle(sq);
        m_bb_piece[pt].toggle(sq);
        if (PHelper::is_slider(pt))
            m_bb_slider[c].toggle(sq);
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
            m_kings[c] = C::SQ_NA;
        m_bb_color[c].toggle(sq);
        m_bb_piece[pt].toggle(sq);
        if (PHelper::is_slider(pt))
            m_bb_slider[c].toggle(sq);
    }
    BitBoard<P> find_sliding_attack_blockers(
        const ColorEnum& attack_by,
        const ColorEnum& block_by,
        const Square& target) const
    {
        BitBoard<P> out{};
        auto attackers = get_occupied_by_slider(attack_by);
        attackers
            &= (Magic<P>::get_adjacent_attack(target)
                | Magic<P>::get_diagonal_attack(target));
        for (auto atk : attackers.iterator()) {
            const auto target_dir = SHelper::direction(atk, target);
            if (!PHelper::slidable_to(m_pieces[atk], target_dir))
                continue;
            auto blockers = BitBoard<P>::get_line_segment(atk, target);
            blockers &= (m_bb_color[BLACK] | m_bb_color[WHITE]);
            if (blockers.hamming_weight() != 1u)
                continue;
            const auto blocker = *blockers.iterator();
            if (PHelper::get_color(m_pieces[blocker]) == block_by)
                out.set(blocker);
        }
        return out;
    }
    void clear_mask_where_attacked(
        BitBoard<P>& mask,
        const ColorEnum& by_side,
        const BitBoard<P>& occ_full_but_king) const
    {
        BitBoard<P> occ_atks = get_occupied_by_slider(by_side);
        BitBoard<P> occ_melee = m_bb_color[by_side] ^ occ_atks;
        const BitBoard<P> melee_mask = BitBoard<P>::get_neighbor_2nd(
            m_kings[~by_side], m_pieces[m_kings[~by_side]]);
        if constexpr (C::num_dir == 12u) {
            occ_melee
                &= (melee_mask
                    | melee_mask.shift(by_side == BLACK ? DIR_S : DIR_N));
        } else {
            occ_melee &= melee_mask;
        }
        occ_atks ^= occ_melee;
        for (auto sq : occ_atks.iterator()) {
            const auto& p = m_pieces[sq];
            if (PHelper::is_slider(p)
                && (BitBoard<P>::get_attacks_by(p, sq) & mask).any()) {
                mask &= ~BitBoard<P>::get_attacks_by(p, sq, occ_full_but_king);
            } else {
                mask &= ~BitBoard<P>::get_attacks_by(p, sq);
            }
            if (mask.empty())
                return;
        }
    }
    template <bool Check>
    void update_droppable(
        BitBoard<P>& droppable,
        const ColoredPiece& p,
        const BitBoard<P>& occ_full) const
    {
        if constexpr (Check) {
            const auto pt = PHelper::to_piece_type(p);
            const auto c = PHelper::get_color(p);
            const Square& target = m_kings[~c];
            droppable &= BitBoard<P>::get_attacks_by(
                PHelper::to_board_piece(~c, pt), target, occ_full);
            if (pt == C::FU) {
                if (!droppable.any())
                    return;
                if (has_pawn_in_file(SHelper::to_file(target), c)
                    || (can_drop_pawn_mate(c)))
                    droppable &= BitBoard<P>();
            }
        } else {
            droppable &= BitBoard<P>::compute_droppable(p);
            if (PHelper::to_piece_type(p) == C::FU) {
                const auto c = PHelper::get_color(p);
                exclude_two_pawns_in_a_file(droppable, c);
                exclude_drop_pawn_mate(droppable, c);
            }
        }
    }
    void exclude_two_pawns_in_a_file(
        BitBoard<P>& occ, const ColorEnum& by_side) const
    {
        for (auto f : EnumIterator<File, C::num_files>()) {
            if (has_pawn_in_file(f, by_side))
                occ &= ~BitBoard<P>::from_file(f);
        }
    }
    bool can_drop_pawn_mate(const ColorEnum& by_side) const
    {
        const Square dst = SHelper::shift(
            m_kings[~by_side], (by_side == BLACK) ? DIR_S : DIR_N);
        if (dst == C::SQ_NA)
            return false;
        if (king_can_avoid_a_pawn_attack(~by_side))
            return false;
        if (enemy_can_capture_the_drop_pawn(dst, by_side))
            return false;
        return true;
    }
    void
    exclude_drop_pawn_mate(BitBoard<P>& occ, const ColorEnum& by_side) const
    {
        const Square dst = SHelper::shift(
            m_kings[~by_side], (by_side == BLACK) ? DIR_S : DIR_N);
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
        const auto enemy_king_sq = m_kings[~by_side];
        if (enemy_king_sq == C::SQ_NA)
            return false;
        return enemy_king_sq
               == SHelper::shift(sq, (by_side == BLACK) ? DIR_N : DIR_S);
    }
    bool king_can_avoid_a_pawn_attack(const ColorEnum& king_color) const
    {
        return compute_king_movable(king_color, ~BitBoard<P>()).any();
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
                    = SHelper::direction(enemy_king_sq, src_next);
                const auto discovered_attacker_sq = find_sliding_attacker(
                    by_side, enemy_king_sq, discovered_dir, src_next);
                const auto is_pinned = (discovered_attacker_sq != C::SQ_NA);
                if (!is_pinned)
                    return true;
            }
        }
        return false;
    }

public:
    std::string
    unique_identifier_jpn(const Move<P>& move, const ColorEnum& by_side) const
    {
        const auto dst = move.destination();
        const auto dr = SHelper::to_rank(dst);
        const auto df = SHelper::to_file(dst);
        const auto p
            = (move.is_drop())
                  ? PHelper::to_board_piece(by_side, move.source_piece())
                  : m_pieces[move.source_square()];
        const auto src_candidates = get_src_candidates(dst, p, move.promote());
        const auto num_cands = src_candidates.hamming_weight();
        if (move.is_drop() && static_cast<bool>(num_cands))
            return u8"\u6253";
        if (num_cands < 2u)
            return u8""; // no unique identifier required

        const uint num_cands_vertical[3] = {
            (src_candidates & BitBoard<P>::from_rank_below(dr, by_side))
                .hamming_weight(),
            (src_candidates & BitBoard<P>::from_rank(dr)).hamming_weight(),
            (src_candidates & BitBoard<P>::from_rank_above(dr, by_side))
                .hamming_weight(),
        };
        const uint num_cands_horizontal[3] = {
            (src_candidates & BitBoard<P>::from_file_right(df, by_side))
                .hamming_weight(),
            (src_candidates & BitBoard<P>::from_file(df)).hamming_weight(),
            (src_candidates & BitBoard<P>::from_file_left(df, by_side))
                .hamming_weight(),
        };
        const auto src = move.source_square();
        return get_unique_identifier_jpn(
            compute_index(dr, SHelper::to_rank(src), by_side),
            compute_index(SHelper::to_file(src), df, by_side),
            num_cands_vertical,
            num_cands_horizontal,
            p);
    }
    std::string origin_eng(const Move<P>& move) const
    {
        const auto src = move.source_square();
        const auto p = m_pieces[src];
        const BitBoard<P> src_candidates
            = get_src_candidates(move.destination(), p, move.promote());
        if (src_candidates.hamming_weight() < 2u)
            return "";
        return std::string(1, '1' + SHelper::to_file(src))
               + std::string(1, '1' + SHelper::to_rank(src));
    }

private:
    BitBoard<P> get_src_candidates(
        const Square dst, const ColoredPiece p, const bool promote) const
    {
        const auto t = PHelper::get_color(p);
        const auto inverse_atk = BitBoard<P>::get_attacks_by(
            PHelper::to_board_piece(~t, PHelper::to_piece_type(p)),
            dst,
            get_occupied());
        BitBoard<P> src_candidates = inverse_atk & get_occupied(p);
        if (promote && (!SHelper::in_promotion_zone(dst, t)))
            src_candidates &= BitBoard<P>::get_promotion_zone(t);
        return src_candidates;
    }
    template <class T>
    static uint compute_index(const T d, const T s, const ColorEnum c)
    {
        return static_cast<uint>(
            sign(
                (static_cast<int>(c) * 2 - 1)
                * (static_cast<int>(s) - static_cast<int>(d)))
            + 1);
    }
    static std::string get_unique_identifier_jpn(
        const uint vertical_index,
        const uint horizontal_index,
        const uint num_candidates_vertical[3],
        const uint num_candidates_horizontal[3],
        const ColoredPiece& p)
    {
        // https://www.shogi.or.jp/faq/kihuhyouki.html
        static const std::string table_vertical[]
            = {u8"\u4e0a", u8"\u5bc4", u8"\u5f15"};
        static const std::string table_horizontal[]
            = {u8"\u53f3", u8"\u76f4", u8"\u5de6"};
        if (num_candidates_vertical[vertical_index] == 1u)
            return table_vertical[vertical_index];
        if (num_candidates_horizontal[horizontal_index] == 1u) {
            if ((horizontal_index == 1u)
                && (PHelper::slidable_to(p, DIR_E)
                    || PHelper::slidable_to(p, DIR_NW)))
                return (num_candidates_horizontal[0]) ? table_horizontal[2]
                                                      : table_horizontal[0];
            return table_horizontal[horizontal_index];
        }
        if ((vertical_index == 0u) && (horizontal_index == 1u))
            return table_horizontal[horizontal_index];
        if ((vertical_index == 2u) && (horizontal_index == 1u))
            return table_vertical[vertical_index];
        return table_horizontal[horizontal_index]
               + table_vertical[vertical_index];
    }
};

} // namespace vshogi

#endif // VSHOGI_BOARD_HPP
