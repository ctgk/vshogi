#ifndef VSHOGI_GAME_HPP
#define VSHOGI_GAME_HPP

#include <algorithm>
#include <string>
#include <vector>

#include "vshogi/common/bitboard_traits.hpp"
#include "vshogi/common/board.hpp"
#include "vshogi/common/color.hpp"
#include "vshogi/common/direction.hpp"
#include "vshogi/common/generator.hpp"
#include "vshogi/common/move.hpp"
#include "vshogi/common/piece_traits.hpp"
#include "vshogi/common/result.hpp"
#include "vshogi/common/square_traits.hpp"
#include "vshogi/common/stand.hpp"
#include "vshogi/common/state.hpp"

namespace vshogi
{

template <class P>
class Game
{
private:
    using C = Configuration<P>;
    using Square = typename C::Square;
    using File = typename C::File;
    using PieceType = typename C::PieceType;
    using Piece = typename C::Piece;
    using BT = BitboardTraits<P>;
    using PT = PieceTraits<P>;
    using ST = SquareTraits<P>;
    using MT = MoveTraits<P>;
    using bitboard_t = typename C::bitboard_t;

public:
    static constexpr uint num_ranks = C::num_ranks;
    static constexpr uint num_files = C::num_files;
    static constexpr uint num_squares = C::num_squares;

private:
    static constexpr uint num_piece_types = C::num_piece_types;
    static constexpr uint num_stand_piece_types = C::num_stand_piece_types;
    static constexpr uint num_dir = C::num_dir;

private:
    State<P> m_state;
    ResultEnum m_result;
    ZobristHashType m_hash;
    std::vector<ZobristHashType> m_hash_list;

    /**
     * @brief List of captured pieces, move, and check-square.
     *       fedcba98 76543210 fedcba98 76543210
     *       ________ ________ ******** ******** move
     *       ________ ******** ________ ________ captured
     *       ____**** ________ ________ ________ checker dir 0
     *       ****____ ________ ________ ________ checker dir 1
     * (MSB) xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx (LSB)
     *
     * @note
     */
    std::vector<std::uint32_t> m_captured_move_list;

public:
    Game() : Game(State<P>())
    {
    }
    Game(const std::string& sfen) : Game(State<P>(sfen))
    {
        const auto t = get_turn();
        const Board<P>& b = get_board();
        const auto enemy_king_sq = b.get_king_square(~t);
        if (enemy_king_sq == C::SQ_NA)
            return;
        for (auto src : BT::iterator(b.get_occupied(t))) {
            if (BT::is_one(b.get_attack_at(src), enemy_king_sq)) {
                m_result = (t == BLACK) ? BLACK_WIN : WHITE_WIN;
                return;
            }
        }
    }
    static constexpr uint feature_channels()
    {
        // 2-player * (piece-types + stand-piece-types)
        return State<P>::feature_channels();
    }
    static constexpr uint num_dlshogi_policy()
    {
        return State<P>::num_dlshogi_policy();
    }
    ColorEnum get_turn() const
    {
        return m_state.get_turn();
    }
    const State<P>& get_state() const
    {
        return m_state;
    }
    const Board<P>& get_board() const
    {
        return m_state.get_board();
    }
    const Stand<P>& get_stand() const
    {
        return m_state.get_stand();
    }
    const Stand<P>& get_stand(const ColorEnum c) const
    {
        return m_state.get_stand(c);
    }
    std::vector<move_t> get_legal_moves() const
    {
        std::vector<move_t> out{};
        if (m_result != ONGOING)
            return out;
        if (in_check()) {
            for (auto g = MoveGenerator<P, GenEnum::EVADE>(m_state); g; ++g)
                out.emplace_back(*g);
        } else {
            for (auto g = MoveGenerator<P>(m_state); g; ++g)
                out.emplace_back(*g);
        }
        return out;
    }
    std::vector<move_t> get_check_moves() const
    {
        std::vector<move_t> out{};
        if (m_result != ONGOING)
            return out;
        for (auto g = MoveGenerator<P, GenEnum::CHECK>(m_state); g; ++g)
            out.emplace_back(*g);
        return out;
    }
    ResultEnum get_result() const
    {
        return m_result;
    }

    /**
     * @brief Count repetitions of the current game position.
     * @note The current position itself counts as one occurrence.
     * @return uint Repetitions of the current game position.
     */
    uint count_repetitions() const
    {
        uint count = 1u;
        const int n = static_cast<int>(m_hash_list.size());
        for (int ii = n - 4; ii >= 0; ii -= 2) {
            const uint index = static_cast<uint>(ii);
            count += (m_hash == m_hash_list[index]);
        }
        return count;
    }

    ZobristHashType get_zobrist_hash() const
    {
        return m_hash;
    }
    std::uint64_t get_board_turn_hash() const
    {
        return get_zobrist_hash() ^ m_state.hash_stands();
    }

    /**
     * @brief Return the number of moves since the start of the game.
     * @note In conformance with `ply()` in
     * https://github.com/niklasf/python-chess
     * @return uint Number of moves since the start of the game.
     */
    uint ply() const
    {
        assert(m_captured_move_list.size() == m_hash_list.size());
        return static_cast<uint>(m_captured_move_list.size());
    }

    /**
     * @brief Return new game with horizontally flipped current positions.
     * @note Returned game does not have game records of the original.
     * @return Game New game with horizontally flipped current positions.
     */
    Game hflip() const
    {
        return Game(m_state.hflip());
    }
    Game rotate() const
    {
        return Game(m_state.rotate());
    }
    Game& resign()
    {
        m_result = (get_turn() == BLACK) ? WHITE_WIN : BLACK_WIN;
        return *this;
    }
    Game& declare_draw()
    {
        m_result = DRAW;
        return *this;
    }
    Game& apply(const move_t& move)
    {
        if ((m_result == ONGOING) && (!is_legal(move))) {
            add_record_and_update_state(move);
            m_result = (get_turn() == BLACK) ? BLACK_WIN : WHITE_WIN;
            return *this;
        }
        return apply_nocheck(move);
    }
    Game& apply_nocheck(const move_t& move)
    {
        add_record_and_update_state(move);
        update_result(C::nfold_repetitions);
        return *this;
    }
    Game& apply_dfpn(const move_t& move)
    {
        add_record_and_update_state(move);
        return *this;
    }
    Game& undo()
    {
        assert(ply() > 0u);
        const auto n = ply() - 1u;
        std::uint32_t v = m_captured_move_list[n];
        const auto move = move_t(static_cast<std::uint16_t>(v & 0x0ffffu));
        const auto captured = static_cast<Piece>((v >> 16u) & 0x0ffu);
        const auto checker_0 = static_cast<DirectionEnum>((v >> 24u) & 0x0fu);
        const auto checker_1 = static_cast<DirectionEnum>((v >> 28u) & 0x0fu);
        m_state.undo(move, captured, checker_0, checker_1);
        m_result = ONGOING;
        m_hash = m_hash_list[n];
        m_hash_list.pop_back();
        m_captured_move_list.pop_back();
        return *this;
    }
    Game& apply_discard(const move_t& move);
    Game& undo_discard();
    bool is_legal(const move_t move) const
    {
        if (MT::is_drop(move)) {
            auto g = DropMoveGenerator<P>(
                m_state, MT::get_src_pt(move), MT::get_dst(move));
            return move == *g;
        } else if (MT::get_src_sq(move) == m_state.get_king_square()) {
            for (auto g = KingMoveGenerator<P>(m_state); g; ++g) {
                if (*g == move)
                    return true;
            }
        } else {
            auto g = SoldierMoveGenerator<P>(m_state, move);
            return move == *g;
        }
        return false;
    }
    bool is_aigoma(const move_t move) const;
    void swap_log(
        std::vector<ZobristHashType>& hash_list,
        std::vector<std::uint32_t>& captured_move_list)
    {
        m_hash_list.swap(hash_list);
        m_captured_move_list.swap(captured_move_list);
    }
    bool had_two_consecutive_sacrifice_drops() const
    {
        const uint n = ply();
        if (n < 4u)
            return false;

        // first sacrifice drop
        const move_t drop1st = get_record_action(n - 4u);
        if (!MT::is_drop(drop1st))
            return false;

        // capture first sacrifice drop
        const move_t capt1st = get_record_action(n - 3u);
        if (MT::get_dst(drop1st) != MT::get_dst(capt1st))
            return false;

        // second sacrifice drop
        const move_t drop2nd = get_record_action(n - 2u);
        if (!MT::is_drop(drop2nd))
            return false;

        // capture second sacrifice drop
        const move_t capt2nd = get_record_action(n - 1u);
        return (MT::get_dst(drop2nd) == MT::get_dst(capt2nd))
               && (MT::get_dst(capt1st) == MT::get_src_sq(capt2nd));
    }

    /**
     * @brief Whether current turn player's king is in check or not.
     *
     * @return true Current turn player's king is in check.
     * @return false Current turn player's king is not in check.
     */
    bool in_check() const
    {
        return m_state.in_check();
    }
    bool is_valid_piece_count(const PieceType& except = C::NA) const
    {
        const Board<P>& b = get_board();
        const Stand<P>& black_stand = get_stand(BLACK);
        const Stand<P>& white_stand = get_stand(WHITE);
        uint piece_count[C::num_stand_piece_types + 1u] = {};
        for (auto sq : C::square_iterator()) {
            if (b.is_empty(sq))
                continue;
            piece_count[PT::demote(PT::to_piece_type(b[sq]))] += 1u;
        }
        for (auto pt : C::stand_piece_type_iterator()) {
            piece_count[pt] += black_stand.count(pt) + white_stand.count(pt);
        }
        for (auto pt :
             EnumIterator<PieceType, C::num_stand_piece_types + 1u>()) {
            if (pt == except)
                continue;
            if (piece_count[pt] != C::initial_piece_count[pt])
                return false;
        }
        return true;
    }
    move_t get_record_action(const uint index) const
    {
        if (index >= m_captured_move_list.size())
            return MT::make_move(C::SQ_NA, C::SQ_NA);
        return static_cast<move_t>(m_captured_move_list[index]);
    }
    void to_feature_map(float* const data) const
    {
        m_state.to_feature_map(data);
    }
    static void attention_matrix(float* const data)
    {
        std::fill_n(data, num_squares * num_squares, 0.f);
        for (auto sq : C::square_iterator()) {
            const uint ii = static_cast<uint>(sq);
            for (auto dir : C::direction_iterator()) {
                auto ptr_sq = ST::ray_from(sq, dir);
                for (; *ptr_sq != C::SQ_NA; ++ptr_sq) {
                    const uint jj = static_cast<uint>(*ptr_sq);
                    data[ii * num_squares + jj] = 1.f;
                }
            }
        }
    }
    static void attention_matrix(
        float* const data,
        const std::vector<DirectionEnum>& directions,
        const bool& local = false)
    {
        std::fill_n(data, num_squares * num_squares, 0.f);
        for (auto sq : C::square_iterator()) {
            const uint ii = static_cast<uint>(sq);
            for (auto dir : directions) {
                auto ptr_sq = ST::ray_from(sq, dir);
                for (; *ptr_sq != C::SQ_NA; ++ptr_sq) {
                    const uint jj = static_cast<uint>(*ptr_sq);
                    data[ii * num_squares + jj] = 1.f;
                    if (local)
                        break;
                }
            }
        }
    }

protected:
    Game(const State<P>& s)
        : m_state(s), m_result(ONGOING), m_hash(m_state.zobrist_hash()),
          m_hash_list{}, m_captured_move_list{}
    {
        m_hash_list.reserve(256);
        m_captured_move_list.reserve(256);
        update_result(C::nfold_repetitions);
    }
    static uint num_pieces(const State<P>& s, const ColorEnum& c)
    {
        const Board<P>& board = s.get_board();
        const auto& stand = s.get_stand(c);
        uint out = board.get_occupied(c).hamming_weight();
        for (auto pt : C::stand_piece_type_iterator())
            out += stand.count(pt);
        return out;
    }
    static uint total_point(const State<P>& s, const ColorEnum& c)
    {
        uint out = 0u;
        const Board<P>& board = s.get_board();
        const auto& stand = s.get_stand(c);
        for (auto sq : board.get_occupied(c).iterator())
            out += PT::get_point(board[sq]);
        for (auto pt : C::stand_piece_type_iterator())
            out += stand.count(pt) * PT::get_point(pt);
        return out;
    }

protected:
    void add_record_and_update_state(const move_t& move)
    {
        const auto captured = m_state.get_board()[MT::get_dst(move)];
        const auto checker_dir_0 = m_state.get_checker_dir(0u);
        const auto checker_dir_1 = m_state.get_checker_dir(1u);
        m_hash_list.emplace_back(m_hash);
        static_assert(sizeof(move_t) == sizeof(std::uint16_t));
        static_assert(sizeof(captured) == sizeof(std::uint8_t));
        m_captured_move_list.emplace_back(
            static_cast<std::uint32_t>(move)
            ^ (static_cast<std::uint32_t>(captured) << 16)
            ^ (static_cast<std::uint32_t>(checker_dir_0) << 24)
            ^ (static_cast<std::uint32_t>(checker_dir_1) << 28));
        m_state.apply(move, &m_hash);
    }
    void add_record_and_update_state_banish(const move_t& move)
    {
        const auto captured = m_state.get_board()[MT::get_dst(move)];
        const auto checker_dir_0 = m_state.get_checker_dir(0u);
        const auto checker_dir_1 = m_state.get_checker_dir(1u);
        m_hash_list.emplace_back(m_hash);
        static_assert(sizeof(move_t) == sizeof(std::uint16_t));
        static_assert(sizeof(captured) == sizeof(std::uint8_t));
        m_captured_move_list.emplace_back(
            static_cast<std::uint32_t>(move)
            ^ (static_cast<std::uint32_t>(captured) << 16)
            ^ (static_cast<std::uint32_t>(checker_dir_0) << 24)
            ^ (static_cast<std::uint32_t>(checker_dir_1) << 28));
        m_state.apply_discard(move, &m_hash);
    }

protected:
    void update_result(const uint nfold_repetitions)
    {
        m_result = ONGOING;
        const auto turn = get_turn();
        if (!DropMoveGenerator<P>(m_state) && !KingMoveGenerator<P>(m_state)
            && !SoldierMoveGenerator<P>(m_state))
            m_result = (turn == BLACK) ? WHITE_WIN : BLACK_WIN;
        if (is_repetitions(nfold_repetitions)) {
            if (m_state.in_check())
                m_result = (turn == BLACK) ? BLACK_WIN : WHITE_WIN;
            else
                m_result = DRAW;
        }
        if (can_declare_win_by_king_enter())
            m_result = (turn == BLACK) ? BLACK_WIN : WHITE_WIN;
    }

public:
    bool is_repetitions(const uint nfold_repetitions) const
    {
        uint num_fold = 1u;
        const int n = static_cast<int>(m_hash_list.size());
        for (int ii = n - 4; ii >= 0; ii -= 2) {
            const uint index = static_cast<uint>(ii);
            num_fold += (m_hash == m_hash_list[index]);
            if (num_fold >= nfold_repetitions)
                return true;
        }
        return false;
    }
    bool can_declare_win_by_king_enter() const
    {
        // http://www2.computer-shogi.org/wcsc17/rule_e.html
        // (4) There is no check on the King of the declaring side.
        if (m_state.in_check())
            return false;

        const auto turn = get_turn();
        const Board<P>& board = get_board();
        // (1) The King of the declaring side is in the third rank or beyond.
        if (!ST::in_promotion_zone(board.get_king_square(turn), turn))
            return false;

        const auto promo_zone_mask = BT::promotion_zone(turn);
        const auto piece_mask = (promo_zone_mask & board.get_occupied(turn));
        const uint num_pieces_in_zone = hamming_weight(piece_mask);

        // (3) The declaring side has 10 or more pieces other than the King in
        // the third rank or beyond.
        if (num_pieces_in_zone <= C::half_num_init_piece_each)
            return false;

        // (2) The declaring side has 28 (the first player (sente, black)) or
        // 27 (the second player (gote, white)) piece points or more.
        if (turn == BLACK)
            return count_point_of(turn, piece_mask) > C::initial_points;
        else
            return count_point_of(turn, piece_mask) >= C::initial_points;
    }

private:
    uint count_point_of(const ColorEnum& c, const bitboard_t& mask) const
    {
        uint out = 0;
        const Board<P>& board = get_board();
        for (auto sq : BT::iterator(mask))
            out += PT::get_point(board[sq]);
        const auto& stand = get_stand(c);
        for (auto pt : C::stand_piece_type_iterator()) {
            out += stand.count(pt) * PT::get_point(pt);
        }
        return out;
    }
};

template <class P>
bool Game<P>::is_aigoma(const move_t move) const
{
    if (not is_legal(move))
        return false;
    const Square checker_sq = m_state.find_checker_square();
    if (checker_sq == C::SQ_NA)
        return false;
    if (MT::get_dst(move) == checker_sq)
        return false;
    if (MT::is_drop(move))
        return true;
    return (MT::get_src_sq(move) != m_state.get_king_square());
}

template <class P>
Game<P>& Game<P>::apply_discard(const move_t& move)
{
    if ((m_result == ONGOING) && (!is_legal(move))) {
        add_record_and_update_state_banish(move);
        m_result = (get_turn() == BLACK) ? BLACK_WIN : WHITE_WIN;
        return *this;
    }
    add_record_and_update_state_banish(move);
    update_result(C::nfold_repetitions);
    return *this;
}

template <class P>
Game<P>& Game<P>::undo_discard()
{
    assert(ply() > 0u);
    const auto n = ply() - 1u;
    std::uint32_t v = m_captured_move_list[n];
    const auto move = move_t(static_cast<std::uint16_t>(v & 0x0ffffu));
    const auto captured = static_cast<Piece>((v >> 16u) & 0x0ffu);
    const auto checker_0 = static_cast<DirectionEnum>((v >> 24u) & 0x0fu);
    const auto checker_1 = static_cast<DirectionEnum>((v >> 28u) & 0x0fu);
    m_state.undo_discard(move, captured, checker_0, checker_1);
    m_result = ONGOING;
    m_hash = m_hash_list[n];
    m_hash_list.pop_back();
    m_captured_move_list.pop_back();
    return *this;
}

} // namespace vshogi

#endif // VSHOGI_GAME_HPP
