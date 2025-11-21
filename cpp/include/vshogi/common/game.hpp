#ifndef VSHOGI_GAME_HPP
#define VSHOGI_GAME_HPP

#include <algorithm>
#include <string>
#include <vector>

#include "vshogi/common/bitboard.hpp"
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

template <class Parameters>
class Game
{
private:
    using C = Configuration<Parameters>;
    using Square = typename C::Square;
    using File = typename C::File;
    using PieceType = typename C::PieceType;
    using Piece = typename C::Piece;
    using PT = PieceTraits<Parameters>;
    using ST = SquareTraits<Parameters>;
    using BitBoardType = BitBoard<Parameters>;
    using BoardType = Board<Parameters>;
    using MoveType = Move<Parameters>;
    using StandType = Stand<Parameters>;
    using StateType = State<Parameters>;

public:
    static constexpr uint num_ranks = C::num_ranks;
    static constexpr uint num_files = C::num_files;
    static constexpr uint num_squares = C::num_squares;

private:
    static constexpr uint num_piece_types = C::num_piece_types;
    static constexpr uint num_stand_piece_types = C::num_stand_piece_types;
    static constexpr uint num_dir = C::num_dir;

private:
    StateType m_current_state;
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
    Game() : Game(StateType())
    {
    }
    Game(const std::string& sfen) : Game(StateType(sfen))
    {
        const auto t = get_turn();
        const BoardType& b = get_board();
        const auto enemy_king_sq = b.get_king_square(~t);
        if (enemy_king_sq == C::SQ_NA)
            return;
        for (auto src : b.get_occupied(t).iterator()) {
            if (b.get_attacks_by_nocheck(src).is_one(enemy_king_sq)) {
                m_result = (t == BLACK) ? BLACK_WIN : WHITE_WIN;
                return;
            }
        }
    }
    static constexpr uint feature_channels()
    {
        // 2-player * (piece-types + stand-piece-types)
        return StateType::feature_channels();
    }
    static constexpr uint num_dlshogi_policy()
    {
        return StateType::num_dlshogi_policy();
    }
    ColorEnum get_turn() const
    {
        return m_current_state.get_turn();
    }
    const StateType& get_state() const
    {
        return m_current_state;
    }
    const BoardType& get_board() const
    {
        return m_current_state.get_board();
    }
    const StandType& get_stand() const
    {
        return m_current_state.get_stand();
    }
    const StandType& get_stand(const ColorEnum c) const
    {
        return m_current_state.get_stand(c);
    }
    std::vector<MoveType> get_legal_moves() const
    {
        std::vector<MoveType> out{};
        if (m_result != ONGOING)
            return out;
        if (in_check()) {
            for (auto g
                 = MoveGenerator<Parameters, GenEnum::EVADE>(m_current_state);
                 g;
                 ++g)
                out.emplace_back(*g);
        } else {
            for (auto g = MoveGenerator<Parameters>(m_current_state); g; ++g)
                out.emplace_back(*g);
        }
        return out;
    }
    std::vector<MoveType> get_check_moves() const
    {
        std::vector<MoveType> out{};
        if (m_result != ONGOING)
            return out;
        for (auto g
             = MoveGenerator<Parameters, GenEnum::CHECK>(m_current_state);
             g;
             ++g)
            out.emplace_back(*g);
        return out;
    }
    Square get_king_square() const
    {
        return m_current_state.get_board().get_king_square(get_turn());
    }
    Square get_king_square(const ColorEnum c) const
    {
        return m_current_state.get_board().get_king_square(c);
    }
    Square find_checker_square(const uint index = 0u) const
    {
        return m_current_state.find_checker_square(index);
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
        return get_zobrist_hash() ^ m_current_state.hash_stands();
    }
    std::string to_sfen(const bool include_move_count = true) const
    {
        if (include_move_count)
            return m_current_state.to_sfen() + " "
                   + std::to_string(m_captured_move_list.size() + 1);
        else
            return m_current_state.to_sfen();
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
        return Game(m_current_state.hflip());
    }
    Game rotate() const
    {
        return Game(m_current_state.rotate());
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
    Game& apply(const MoveType& move)
    {
        if ((m_result == ONGOING) && (!is_legal(move))) {
            add_record_and_update_state(move);
            m_result = (get_turn() == BLACK) ? BLACK_WIN : WHITE_WIN;
            return *this;
        }
        return apply_nocheck(move);
    }
    Game& apply_nocheck(const MoveType& move)
    {
        add_record_and_update_state(move);
        update_result(C::max_acceptable_repetitions);
        return *this;
    }
    Game& apply_dfpn(const MoveType& move)
    {
        add_record_and_update_state(move);
        return *this;
    }
    void update_result_dfpn(
        const uint max_repetitions_inclusive,
        const bool check_repetition = true)
    {
        m_result = ONGOING;
        const auto turn = get_turn();
        if (check_repetition && is_repetitions(max_repetitions_inclusive)) {
            if (m_current_state.in_check())
                m_result = (turn == BLACK) ? BLACK_WIN : WHITE_WIN;
            else
                m_result = DRAW;
        }
        if (can_declare_win_by_king_enter())
            m_result = (turn == BLACK) ? BLACK_WIN : WHITE_WIN;
    }
    Game& undo()
    {
        assert(ply() > 0u);
        const auto n = ply() - 1u;
        std::uint32_t v = m_captured_move_list[n];
        const auto move = MoveType(static_cast<std::uint16_t>(v & 0x0ffffu));
        const auto captured = static_cast<Piece>((v >> 16u) & 0x0ffu);
        const auto checker_0 = static_cast<DirectionEnum>((v >> 24u) & 0x0fu);
        const auto checker_1 = static_cast<DirectionEnum>((v >> 28u) & 0x0fu);
        m_current_state.undo(move, captured, checker_0, checker_1);
        m_result = ONGOING;
        m_hash = m_hash_list[n];
        m_hash_list.pop_back();
        m_captured_move_list.pop_back();
        return *this;
    }
    bool is_legal(const MoveType move) const
    {
        if (move.is_drop()) {
            auto iter = DropMoveGenerator<Parameters>(
                m_current_state, move.source_piece(), move.destination());
            return move == *iter;
        } else if (
            move.source_square() == get_board().get_king_square(get_turn())) {
            for (auto it = KingMoveGenerator<Parameters>(m_current_state); it;
                 ++it) {
                if (*it == move)
                    return true;
            }
        } else {
            auto iter = SoldierMoveGenerator<Parameters>(m_current_state, move);
            return move == *iter;
        }
        return false;
    }
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
        const Move<Parameters> drop1st = get_record_action(n - 4u);
        if (!drop1st.is_drop())
            return false;

        // capture first sacrifice drop
        const Move<Parameters> capt1st = get_record_action(n - 3u);
        if (drop1st.destination() != capt1st.destination())
            return false;

        // second sacrifice drop
        const Move<Parameters> drop2nd = get_record_action(n - 2u);
        if (!drop2nd.is_drop())
            return false;

        // capture second sacrifice drop
        const Move<Parameters> capt2nd = get_record_action(n - 1u);
        return (drop2nd.destination() == capt2nd.destination())
               && (capt1st.destination() == capt2nd.source_square());
    }

    /**
     * @brief Whether current turn player's king is in check or not.
     *
     * @return true Current turn player's king is in check.
     * @return false Current turn player's king is not in check.
     */
    bool in_check() const
    {
        return m_current_state.in_check();
    }
    bool is_valid_piece_count(const PieceType& except = C::NA) const
    {
        const BoardType& b = get_board();
        const StandType& black_stand = get_stand(BLACK);
        const StandType& white_stand = get_stand(WHITE);
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
    Move<Parameters> get_record_action(const uint index) const
    {
        return Move<Parameters>(
            static_cast<std::uint16_t>(m_captured_move_list[index]));
    }
    std::string to_jpn(const Move<Parameters>& move) const
    {
        const Square dst = move.destination();
        const BoardType& b = get_board();
        const ColorEnum t = get_turn();
        if (move.is_drop()) {
            const auto dst_jpn = ST::to_jpn(dst);
            return dst_jpn + PT::to_jpn(move.source_piece())
                   + b.unique_identifier_jpn(move, t);
        } else {
            const Square src = move.source_square();
            const PieceType pt = PT::to_piece_type(b[src]);
            const auto pt_jpn = PT::to_jpn(pt, false);
            const auto unique_identifier_jpn = b.unique_identifier_jpn(move, t);
            const auto promotion_jpn = move.promotion_to_jpn(pt, t);
            const uint n = ply();
            const auto dst_jpn = move.destination_to_jpn(
                (n > 0u) ? get_record_action(n - 1u).destination() : C::SQ_NA);
            return dst_jpn + pt_jpn + unique_identifier_jpn + promotion_jpn;
        }
    }
    std::string to_eng(const Move<Parameters>& move) const
    {
        const Square dst = move.destination();
        const auto dst_eng = ST::to_eng(dst);
        if (move.is_drop())
            return PT::to_eng(move.source_piece()) + "*" + dst_eng;
        const Square src = move.source_square();
        const BoardType& b = get_board();
        const auto pt_eng = PT::to_eng(b[src]);
        const auto origin_eng = b.origin_eng(move);
        const auto movement_eng = (b.is_empty(dst) ? "-" : "x");
        const auto promotion_eng
            = move.promotion_to_eng(PT::to_piece_type(b[src]), get_turn());
        return pt_eng + origin_eng + movement_eng + dst_eng + promotion_eng;
    }
    void to_feature_map(float* const data) const
    {
        m_current_state.to_feature_map(data);
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
    Game(const StateType& s)
        : m_current_state(s), m_result(ONGOING),
          m_hash(m_current_state.zobrist_hash()), m_hash_list{},
          m_captured_move_list{}
    {
        m_hash_list.reserve(256);
        m_captured_move_list.reserve(256);
        update_result(C::max_acceptable_repetitions);
    }
    static uint num_pieces(const StateType& s, const ColorEnum& c)
    {
        const BoardType& board = s.get_board();
        const auto& stand = s.get_stand(c);
        uint out = board.get_occupied(c).hamming_weight();
        for (auto pt : C::stand_piece_type_iterator())
            out += stand.count(pt);
        return out;
    }
    static uint total_point(const StateType& s, const ColorEnum& c)
    {
        uint out = 0u;
        const BoardType& board = s.get_board();
        const auto& stand = s.get_stand(c);
        for (auto sq : board.get_occupied(c).iterator())
            out += PT::get_point(board[sq]);
        for (auto pt : C::stand_piece_type_iterator())
            out += stand.count(pt) * PT::get_point(pt);
        return out;
    }

protected:
    void add_record_and_update_state(const MoveType& move)
    {
        const auto captured = m_current_state.get_board()[move.destination()];
        const auto checker_dir_0 = m_current_state.get_checker_dir(0u);
        const auto checker_dir_1 = m_current_state.get_checker_dir(1u);
        m_hash_list.emplace_back(m_hash);
        static_assert(sizeof(MoveType) == sizeof(std::uint16_t));
        static_assert(sizeof(captured) == sizeof(std::uint8_t));
        m_captured_move_list.emplace_back(
            static_cast<std::uint32_t>(move.hash())
            ^ (static_cast<std::uint32_t>(captured) << 16)
            ^ (static_cast<std::uint32_t>(checker_dir_0) << 24)
            ^ (static_cast<std::uint32_t>(checker_dir_1) << 28));
        m_current_state.apply(move, &m_hash);
    }

protected:
    void update_result(const uint max_repetitions_inclusive)
    {
        m_result = ONGOING;
        const auto turn = get_turn();
        if (!DropMoveGenerator<Parameters>(m_current_state)
            && !KingMoveGenerator<Parameters>(m_current_state)
            && !SoldierMoveGenerator<Parameters>(m_current_state))
            m_result = (turn == BLACK) ? WHITE_WIN : BLACK_WIN;
        if (is_repetitions(max_repetitions_inclusive)) {
            if (m_current_state.in_check())
                m_result = (turn == BLACK) ? BLACK_WIN : WHITE_WIN;
            else
                m_result = DRAW;
        }
        if (can_declare_win_by_king_enter())
            m_result = (turn == BLACK) ? BLACK_WIN : WHITE_WIN;
    }

public:
    bool is_repetitions(const uint max_repetitions_inclusive) const
    {
        uint num_fold = 1u;
        const int n = static_cast<int>(m_hash_list.size());
        for (int ii = n - 4; ii >= 0; ii -= 2) {
            const uint index = static_cast<uint>(ii);
            num_fold += (m_hash == m_hash_list[index]);
            if (num_fold > max_repetitions_inclusive)
                return true;
        }
        return false;
    }
    bool can_declare_win_by_king_enter() const
    {
        // http://www2.computer-shogi.org/wcsc17/rule_e.html
        // (4) There is no check on the King of the declaring side.
        if (m_current_state.in_check())
            return false;

        const auto turn = get_turn();
        const BoardType& board = get_board();
        // (1) The King of the declaring side is in the third rank or beyond.
        if (!ST::in_promotion_zone(board.get_king_square(turn), turn))
            return false;

        const auto promo_zone_mask = BitBoardType::get_promotion_zone(turn);
        const auto piece_mask = (promo_zone_mask & board.get_occupied(turn));
        const uint num_pieces_in_zone = piece_mask.hamming_weight();

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
    uint count_point_of(const ColorEnum& c, const BitBoardType& mask) const
    {
        uint out = 0;
        const BoardType& board = get_board();
        for (auto sq : mask.iterator())
            out += PT::get_point(board[sq]);
        const auto& stand = get_stand(c);
        for (auto pt : C::stand_piece_type_iterator()) {
            out += stand.count(pt) * PT::get_point(pt);
        }
        return out;
    }
};

} // namespace vshogi

#endif // VSHOGI_GAME_HPP
