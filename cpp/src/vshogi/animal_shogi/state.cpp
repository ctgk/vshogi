#include "vshogi/animal_shogi/state.hpp"

namespace vshogi::animal_shogi
{

State::State() : m_board(), m_two_piece_stands(), m_turn(BLACK)
{
}

State::State(const std::string& sfen)
    : m_board(), m_two_piece_stands(), m_turn()
{
    auto s = sfen.c_str();

    s = m_board.set_sfen(s);
    {
        (*s == 'b') ? m_turn = BLACK : m_turn = WHITE;
        ++s;
        ++s;
    }
    m_two_piece_stands.set_sfen_holdings(s);
}

void State::apply_move(const Move move)
{
    const PieceTypeEnum captured_piece
        = pop_piece_from_board(move.destination());
    add_captured_piece_to_stand(captured_piece);

    const BoardPieceTypeEnum moving_piece
        = pop_piece_from_stand_or_board(move.source());
    place_piece_on_board(moving_piece);
}

} // namespace vshogi::animal_shogi
