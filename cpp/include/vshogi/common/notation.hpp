#ifndef VSHOGI_COMMON_NOTATION_HPP
#define VSHOGI_COMMON_NOTATION_HPP

#include <string>

#include "vshogi/common/board.hpp"
#include "vshogi/common/color.hpp"
#include "vshogi/common/config.hpp"
#include "vshogi/common/game.hpp"
#include "vshogi/common/move.hpp"
#include "vshogi/common/piece_traits.hpp"
#include "vshogi/common/square_traits.hpp"
#include "vshogi/common/stand.hpp"
#include "vshogi/common/state.hpp"

namespace vshogi
{

template <class P>
class Notation
{
    using C = Configuration<P>;
    using BT = BitboardTraits<P>;
    using PT = PieceTraits<P>;
    using ST = SquareTraits<P>;
    using MT = MoveTraits<P>;
    using Piece = typename C::Piece;
    using PieceType = typename C::PieceType;
    using Square = typename C::Square;
    using bitboard_t = typename C::bitboard_t;

public:
    Notation() = delete;
    static constexpr char to_char(const PieceType& pt);
    static void to_sfen(const Square& sq, char sfen[2]);
    static void to_sfen(const move_t& m, char sfen[5]);
    template <class T>
    static std::string to_sfen(const T& a);
    static std::string to_sfen(const PieceType& pt);
    static std::string to_sfen(const Piece& p);
    static std::string to_sfen(const Board<P>& b);
    static std::string to_sfen(const BlackWhiteStands<P>& stands);
    static std::string to_sfen(const State<P>& s);
    static std::string
    to_sfen(const Game<P>& g, const bool include_move_count = true);

    static std::string to_eng(const PieceType& pt);
    static std::string to_eng(const Piece& p);
    static std::string to_eng(const Square& sq);
    static std::string to_eng(const move_t& m, const Game<P>& g);

    static const std::string&
    to_jpn(const PieceType& pt, const bool single_char = true);
    static std::string to_jpn(const Square& sq);
    static std::string to_jpn(const move_t& m, const Game<P>& g);

private:
    static const PieceType pieces_in_sfen_order[C::num_stand_piece_types];
    static constexpr char to_char(const FullPieceTypes& pt);
    static const std::string& to_jpn(FullPieceTypes pt, const bool single_char);
    static void to_sfen(
        const Board<P>& board, const typename C::Rank rank, std::string& out);
    static std::string unique_identifier_jpn(
        const move_t& m, const Board<P>& b, const ColorEnum& by_side);
    static uint compute_index(const int d, const int s, const ColorEnum c);
    static std::string get_unique_identifier_jpn(
        const uint vertical_index,
        const uint horizontal_index,
        const uint num_candidates_vertical[3],
        const uint num_candidates_horizontal[3],
        const Piece& p);
    static typename C::bitboard_t get_src_candidate(
        const Board<P>& b, const Square dst, const Piece p, const bool promote);
};

template <class P>
constexpr char Notation<P>::to_char(const PieceType& pt)
{
    return to_char(C::piece_types[pt]);
}

template <class P>
void Notation<P>::to_sfen(const Square& sq, char sfen[2])
{
    sfen[0] = static_cast<char>(static_cast<int>(ST::to_file(sq)) + '1');
    sfen[1] = static_cast<char>(static_cast<int>(ST::to_rank(sq)) + 'a');
}

template <class P>
void Notation<P>::to_sfen(const move_t& m, char sfen[5])
{
    if (MT::is_drop(m)) {
        sfen[0] = static_cast<char>(std::toupper(to_char(MT::get_src_pt(m))));
        sfen[1] = '*';
    } else {
        to_sfen(MT::get_src_sq(m), sfen);
    }
    to_sfen(MT::get_dst(m), sfen + 2);
    if (MT::get_promote(m))
        sfen[4] = '+';
}

template <class P>
template <class T>
std::string Notation<P>::to_sfen(const T& a)
{
    char sfen[10] = {};
    to_sfen(a, sfen);
    return std::string(sfen);
}

template <class P>
std::string Notation<P>::to_sfen(const PieceType& pt)
{
    if ((pt == C::NA) || !PT::is_promoted(pt))
        return std::string(1, to_char(pt));
    return std::string(1, '+') + to_char(pt);
}

template <class P>
std::string Notation<P>::to_sfen(const Piece& p)
{
    if (p == C::VOID)
        return std::string(1, '?');
    auto out = std::string();
    const auto color = PT::get_color(p);
    const auto promotion = PT::is_promoted(p);
    const auto pt = PT::demote(PT::to_piece_type(p));
    char c = to_char(pt);
    if (color == BLACK)
        c = static_cast<char>(std::toupper(static_cast<int>(c)));
    if (promotion)
        out += '+';
    out += c;
    return out;
}

template <class P>
std::string Notation<P>::to_sfen(const Board<P>& b)
{
    using Rank = typename C::Rank;
    auto out = std::string();
    out.reserve(C::num_squares);
    to_sfen(b, static_cast<Rank>(0), out);
    for (uint ir = 1; ir < C::num_ranks; ++ir) {
        out += '/';
        to_sfen(b, static_cast<Rank>(ir), out);
    }
    return out;
}

template <class P>
std::string Notation<P>::to_sfen(const BlackWhiteStands<P>& stands)
{
    auto out = std::string();
    if (!(stands.black().any() || stands.white().any())) {
        out += '-';
        return out;
    }
    for (auto& c : color_array) {
        for (auto& p : pieces_in_sfen_order) {
            const auto num = stands[c].count(p);
            if (num == 0)
                continue;
            if (num > 9)
                out += '1';
            if (num > 1)
                out += static_cast<char>('0' + num % 10);
            out += to_sfen(PT::make_piece(c, p));
        }
    }
    return out;
}

template <class P>
std::string Notation<P>::to_sfen(const State<P>& s)
{
    auto out = to_sfen(s.get_board());
    out += ' ';
    out += ((s.get_turn() == BLACK) ? 'b' : 'w');
    out += ' ';
    out += to_sfen(s.get_stands());
    return out;
}

template <class P>
std::string
Notation<P>::to_sfen(const Game<P>& g, const bool include_move_count)
{
    if (include_move_count)
        return to_sfen(g.get_state()) + " " + std::to_string(g.ply() + 1u);
    return to_sfen(g.get_state());
}

template <class P>
void Notation<P>::to_sfen(
    const Board<P>& board, const typename C::Rank rank, std::string& out)
{
    using File = typename C::File;
    int num_void = 0;
    for (int f = static_cast<int>(C::num_files) - 1; f >= 0; --f) {
        const auto sq = ST::to_square(static_cast<File>(f), rank);
        const auto piece = board[sq];
        if (piece == C::VOID) {
            ++num_void;
            continue;
        }
        if (num_void > 0) {
            out += static_cast<char>('0' + num_void);
            num_void = 0;
        }
        out += to_sfen(piece);
    }
    if (num_void > 0)
        out += static_cast<char>('0' + num_void);
}

template <class P>
std::string Notation<P>::to_eng(const PieceType& pt)
{
    if (PT::is_promoted(pt))
        return "+" + std::string(1, std::toupper(to_char(pt)));
    return std::string(1, std::toupper(to_char(pt)));
}

template <class P>
std::string Notation<P>::to_eng(const Piece& p)
{
    return to_eng(PT::to_piece_type(p));
}

template <class P>
std::string Notation<P>::to_eng(const Square& sq)
{
    const auto f = ST::to_file(sq);
    const auto r = ST::to_rank(sq);
    return std::string(1, '1' + f) + std::string(1, '1' + r);
}

template <class P>
std::string Notation<P>::to_eng(const move_t& m, const Game<P>& g)
{
    const Square dst = MT::get_dst(m);
    const Square src = MT::get_src_sq(m);
    const Board<P>& b = g.get_board();
    const auto p = b[src];
    const auto t = g.get_turn();

    const auto dst_eng = to_eng(dst);
    if (MT::is_drop(m))
        return to_eng(MT::get_src_pt(m)) + "*" + dst_eng;
    const auto pt_eng = to_eng(p);
    const auto src_candidate = get_src_candidate(b, dst, p, MT::get_promote(m));
    const auto origin_eng = (hamming_weight(src_candidate) < 2u)
                                ? ""
                                : std::string(1, '1' + ST::to_file(src))
                                      + std::string(1, '1' + ST::to_rank(src));
    const auto movement_eng = (b.is_empty(dst) ? "-" : "x");
    const char promotion_eng = MT::get_promote(m) ? '+'
                               : (PT::is_promotable(p)
                                  && (ST::in_promotion_zone(dst, t)
                                      || ST::in_promotion_zone(src, t)))
                                   ? '='
                                   : '\0';
    if (promotion_eng)
        return pt_eng + origin_eng + movement_eng + dst_eng + promotion_eng;
    return pt_eng + origin_eng + movement_eng + dst_eng;
}

template <class P>
const std::string&
Notation<P>::to_jpn(FullPieceTypes pt, const bool single_char)
{
    static const std::string single_char_table[] = {
        u8"\u6B69", //!< Fu (Pawn)
        u8"\u9999", //!< Kyo (Lance)
        u8"\u6842", //!< Kei (Knight)
        u8"\u9280", //!< Gin (Silver)
        u8"\u89D2", //!< Kaku (Bishop)
        u8"\u98DB", //!< Hisha (Rook)
        u8"\u91D1", //!< Kin (Gold)
        u8"\u7389", //!< Ou, Gyoku (King)
        u8"\u3068", //!< Tokin (Promoted Pawn)
        u8"\u674F", //!< Nari-Kyo (Promoted Lance)
        u8"\u572D", //!< Nari-Kei (Promoted Knight)
        u8"\u5168", //!< Nari-Gin (Promoted Silver)
        u8"\u99AC", //!< Uma (Promoted Bishop)
        u8"\u9F8D", //!< Ryu (Promoted Rook)
        u8"\uFF1F", //!< NA
    };
    static const std::string two_chars_table[] = {
        u8"\u6B69", //!< Fu (Pawn)
        u8"\u9999", //!< Kyo (Lance)
        u8"\u6842", //!< Kei (Knight)
        u8"\u9280", //!< Gin (Silver)
        u8"\u89D2", //!< Kaku (Bishop)
        u8"\u98DB", //!< Hisha (Rook)
        u8"\u91D1", //!< Kin (Gold)
        u8"\u7389", //!< Ou, Gyoku (King)
        u8"\u3068", //!< Tokin (Promoted Pawn)
        u8"\u6210\u9999", //!< Nari-Kyo (Promoted Lance)
        u8"\u6210\u6842", //!< Nari-Kei (Promoted Knight)
        u8"\u6210\u9280", //!< Nari-Gin (Promoted Silver)
        u8"\u99AC", //!< Uma (Promoted Bishop)
        u8"\u9F8D", //!< Ryu (Promoted Rook)
        u8"\uFF1F", //!< NA
    };
    if (single_char)
        return single_char_table[pt];
    return two_chars_table[pt];
}

template <class P>
const std::string&
Notation<P>::to_jpn(const PieceType& pt, const bool single_char)
{
    return to_jpn(C::piece_types[pt], single_char);
}

template <class P>
std::string Notation<P>::to_jpn(const Square& sq)
{
    const auto f = ST::to_file(sq);
    const auto r = ST::to_rank(sq);
    static const std::string arabic[] = {
        u8"\uFF11",
        u8"\uFF12",
        u8"\uFF13",
        u8"\uFF14",
        u8"\uFF15",
        u8"\uFF16",
        u8"\uFF17",
        u8"\uFF18",
        u8"\uFF19",
    };
    static const std::string kanji[] = {
        u8"\u4E00",
        u8"\u4E8C",
        u8"\u4E09",
        u8"\u56DB",
        u8"\u4E94",
        u8"\u516D",
        u8"\u4E03",
        u8"\u516B",
        u8"\u4E5D",
    };
    return arabic[f] + kanji[r];
}

template <class P>
constexpr char Notation<P>::to_char(const FullPieceTypes& pt)
{
    constexpr char table[] = {
        'p', //!< Fu (Pawn)
        'l', //!< Kyo (Lance)
        'n', //!< Kei (Knight)
        's', //!< Gin (Silver)
        'b', //!< Kaku (Bishop)
        'r', //!< Hisha (Rook)
        'g', //!< Kin (Gold)
        'k', //!< Ou, Gyoku (King)
        'p', //!< Tokin (Promoted Pawn)
        'l', //!< Nari-Kyo (Promoted Lance)
        'n', //!< Nari-Kei (Promoted Knight)
        's', //!< Nari-Gin (Promoted Silver)
        'b', //!< Uma (Promoted Bishop)
        'r', //!< Ryu (Promoted Rook)
        '?', //!< NA
    };
    return table[pt];
}

template <class P>
std::string Notation<P>::to_jpn(const move_t& m, const Game<P>& g)
{
    const Square dst = MT::get_dst(m);
    const Board<P>& b = g.get_board();
    const ColorEnum t = g.get_turn();
    auto dst_jpn = to_jpn(dst);
    const auto unique_identifier = unique_identifier_jpn(m, b, t);
    if (MT::is_drop(m))
        return dst_jpn + to_jpn(MT::get_src_pt(m), true) + unique_identifier;

    const Square src = MT::get_src_sq(m);
    const auto p = b[src];
    const PieceType pt = PT::to_piece_type(p);
    const auto pt_jpn = to_jpn(pt, false);
    const auto promotion_jpn = MT::get_promote(m) ? u8"\u6210"
                               : (PT::is_promotable(p)
                                  && (ST::in_promotion_zone(src, t)
                                      || ST::in_promotion_zone(dst, t)))
                                   ? u8"\u4e0d\u6210"
                                   : u8"";

    const auto dst_prev = MT::get_dst(g.get_record_action(g.ply() - 1u));
    if (dst == dst_prev)
        dst_jpn = u8"\u540c";
    return dst_jpn + pt_jpn + unique_identifier + promotion_jpn;
}

template <class P>
std::string Notation<P>::unique_identifier_jpn(
    const move_t& move, const Board<P>& b, const ColorEnum& by_side)
{
    const auto dst = MT::get_dst(move);
    const auto dr = ST::to_rank(dst);
    const auto df = ST::to_file(dst);
    const auto p = (MT::is_drop(move))
                       ? PT::make_piece(by_side, MT::get_src_pt(move))
                       : b[MT::get_src_sq(move)];
    const auto src_candidates
        = get_src_candidate(b, dst, p, MT::get_promote(move));
    const auto num_cands = hamming_weight(src_candidates);
    if (MT::is_drop(move) && static_cast<bool>(num_cands))
        return u8"\u6253";
    if (num_cands < 2u)
        return u8""; // no unique identifier required
    bitboard_t above{}, below{}, right{}, left{};
    for (auto r : EnumIterator<typename C::Rank, C::num_ranks>()) {
        if (r < dr)
            above ^= BT::from_rank(r);
        else if (dr < r)
            below ^= BT::from_rank(r);
    }
    right = BT::invert(BT::from_file(df)) & (BT::from_file(df) - 1u);
    left = BT::full() ^ (BT::from_file(df) ^ right);
    if (by_side == WHITE) {
        auto tmp = right;
        right = left;
        left = tmp;
        tmp = above;
        above = below;
        below = tmp;
    }
    const uint num_cands_vertical[3] = {
        hamming_weight(src_candidates & below),
        hamming_weight(src_candidates & BT::from_rank(dr)),
        hamming_weight(src_candidates & above),
    };
    const uint num_cands_horizontal[3] = {
        hamming_weight(src_candidates & right),
        hamming_weight(src_candidates & BT::from_file(df)),
        hamming_weight(src_candidates & left),
    };
    const auto src = MT::get_src_sq(move);
    return get_unique_identifier_jpn(
        compute_index(
            static_cast<int>(dr), static_cast<int>(ST::to_rank(src)), by_side),
        compute_index(
            static_cast<int>(ST::to_file(src)), static_cast<int>(df), by_side),
        num_cands_vertical,
        num_cands_horizontal,
        p);
}

template <class P>
uint Notation<P>::compute_index(const int d, const int s, const ColorEnum c)
{
    return static_cast<uint>(sign((static_cast<int>(c) * 2 - 1) * (s - d)) + 1);
}

template <class P>
std::string Notation<P>::get_unique_identifier_jpn(
    const uint vertical_index,
    const uint horizontal_index,
    const uint num_candidates_vertical[3],
    const uint num_candidates_horizontal[3],
    const Piece& p)
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
            && (PT::slidable_to(p, DIR_E) || PT::slidable_to(p, DIR_NW)))
            return (num_candidates_horizontal[0]) ? table_horizontal[2]
                                                  : table_horizontal[0];
        return table_horizontal[horizontal_index];
    }
    if ((vertical_index == 0u) && (horizontal_index == 1u))
        return table_horizontal[horizontal_index];
    if ((vertical_index == 2u) && (horizontal_index == 1u))
        return table_vertical[vertical_index];
    return table_horizontal[horizontal_index] + table_vertical[vertical_index];
}

template <class P>
typename Configuration<P>::bitboard_t Notation<P>::get_src_candidate(
    const Board<P>& b, const Square dst, const Piece p, const bool promote)
{
    const auto t = PT::get_color(p);
    const auto inverse_atk = BT::get_attack_by(
        PT::make_piece(~t, PT::to_piece_type(p)), dst, b.get_occupied());
    auto src_candidates = inverse_atk & b.get_occupied(p);
    if (promote && (!ST::in_promotion_zone(dst, t)))
        src_candidates &= BT::promotion_zone(t);
    return src_candidates;
}

} // namespace vshogi

#endif // VSHOGI_COMMON_NOTATION_HPP
