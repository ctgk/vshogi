#include "vshogi/minishogi/bitboard.hpp"

namespace vshogi::minishogi
{

namespace internal
{

constexpr BitBoard attacks_by_bgi_sq3c = bb_2b | bb_2d | bb_3b | bb_4b | bb_4d;
constexpr BitBoard attacks_by_bgi_sq3d = attacks_by_bgi_sq3c.shift<DIR_S>();
constexpr BitBoard attacks_by_bgi_sq3e = attacks_by_bgi_sq3d.shift<DIR_S>();
constexpr BitBoard attacks_by_bgi_sq3b = attacks_by_bgi_sq3c.shift<DIR_N>();
constexpr BitBoard attacks_by_bgi_sq3a = attacks_by_bgi_sq3b.shift<DIR_N>();
constexpr BitBoard attacks_by_bgi_sq2a = attacks_by_bgi_sq3a.shift<DIR_E>();
constexpr BitBoard attacks_by_bgi_sq2b = attacks_by_bgi_sq3b.shift<DIR_E>();
constexpr BitBoard attacks_by_bgi_sq2c = attacks_by_bgi_sq3c.shift<DIR_E>();
constexpr BitBoard attacks_by_bgi_sq2d = attacks_by_bgi_sq3d.shift<DIR_E>();
constexpr BitBoard attacks_by_bgi_sq2e = attacks_by_bgi_sq3e.shift<DIR_E>();
constexpr BitBoard attacks_by_bgi_sq1a = attacks_by_bgi_sq2a.shift<DIR_E>();
constexpr BitBoard attacks_by_bgi_sq1b = attacks_by_bgi_sq2b.shift<DIR_E>();
constexpr BitBoard attacks_by_bgi_sq1c = attacks_by_bgi_sq2c.shift<DIR_E>();
constexpr BitBoard attacks_by_bgi_sq1d = attacks_by_bgi_sq2d.shift<DIR_E>();
constexpr BitBoard attacks_by_bgi_sq1e = attacks_by_bgi_sq2e.shift<DIR_E>();
constexpr BitBoard attacks_by_bgi_sq4a = attacks_by_bgi_sq3a.shift<DIR_W>();
constexpr BitBoard attacks_by_bgi_sq4b = attacks_by_bgi_sq3b.shift<DIR_W>();
constexpr BitBoard attacks_by_bgi_sq4c = attacks_by_bgi_sq3c.shift<DIR_W>();
constexpr BitBoard attacks_by_bgi_sq4d = attacks_by_bgi_sq3d.shift<DIR_W>();
constexpr BitBoard attacks_by_bgi_sq4e = attacks_by_bgi_sq3e.shift<DIR_W>();
constexpr BitBoard attacks_by_bgi_sq5a = attacks_by_bgi_sq4a.shift<DIR_W>();
constexpr BitBoard attacks_by_bgi_sq5b = attacks_by_bgi_sq4b.shift<DIR_W>();
constexpr BitBoard attacks_by_bgi_sq5c = attacks_by_bgi_sq4c.shift<DIR_W>();
constexpr BitBoard attacks_by_bgi_sq5d = attacks_by_bgi_sq4d.shift<DIR_W>();
constexpr BitBoard attacks_by_bgi_sq5e = attacks_by_bgi_sq4e.shift<DIR_W>();

constexpr BitBoard attacks_by_wgi_sq3c = bb_2b | bb_2d | bb_3d | bb_4b | bb_4d;
constexpr BitBoard attacks_by_wgi_sq3d = attacks_by_wgi_sq3c.shift<DIR_S>();
constexpr BitBoard attacks_by_wgi_sq3e = attacks_by_wgi_sq3d.shift<DIR_S>();
constexpr BitBoard attacks_by_wgi_sq3b = attacks_by_wgi_sq3c.shift<DIR_N>();
constexpr BitBoard attacks_by_wgi_sq3a = attacks_by_wgi_sq3b.shift<DIR_N>();
constexpr BitBoard attacks_by_wgi_sq2a = attacks_by_wgi_sq3a.shift<DIR_E>();
constexpr BitBoard attacks_by_wgi_sq2b = attacks_by_wgi_sq3b.shift<DIR_E>();
constexpr BitBoard attacks_by_wgi_sq2c = attacks_by_wgi_sq3c.shift<DIR_E>();
constexpr BitBoard attacks_by_wgi_sq2d = attacks_by_wgi_sq3d.shift<DIR_E>();
constexpr BitBoard attacks_by_wgi_sq2e = attacks_by_wgi_sq3e.shift<DIR_E>();
constexpr BitBoard attacks_by_wgi_sq1a = attacks_by_wgi_sq2a.shift<DIR_E>();
constexpr BitBoard attacks_by_wgi_sq1b = attacks_by_wgi_sq2b.shift<DIR_E>();
constexpr BitBoard attacks_by_wgi_sq1c = attacks_by_wgi_sq2c.shift<DIR_E>();
constexpr BitBoard attacks_by_wgi_sq1d = attacks_by_wgi_sq2d.shift<DIR_E>();
constexpr BitBoard attacks_by_wgi_sq1e = attacks_by_wgi_sq2e.shift<DIR_E>();
constexpr BitBoard attacks_by_wgi_sq4a = attacks_by_wgi_sq3a.shift<DIR_W>();
constexpr BitBoard attacks_by_wgi_sq4b = attacks_by_wgi_sq3b.shift<DIR_W>();
constexpr BitBoard attacks_by_wgi_sq4c = attacks_by_wgi_sq3c.shift<DIR_W>();
constexpr BitBoard attacks_by_wgi_sq4d = attacks_by_wgi_sq3d.shift<DIR_W>();
constexpr BitBoard attacks_by_wgi_sq4e = attacks_by_wgi_sq3e.shift<DIR_W>();
constexpr BitBoard attacks_by_wgi_sq5a = attacks_by_wgi_sq4a.shift<DIR_W>();
constexpr BitBoard attacks_by_wgi_sq5b = attacks_by_wgi_sq4b.shift<DIR_W>();
constexpr BitBoard attacks_by_wgi_sq5c = attacks_by_wgi_sq4c.shift<DIR_W>();
constexpr BitBoard attacks_by_wgi_sq5d = attacks_by_wgi_sq4d.shift<DIR_W>();
constexpr BitBoard attacks_by_wgi_sq5e = attacks_by_wgi_sq4e.shift<DIR_W>();

constexpr BitBoard attacks_by_bki_sq3c
    = bb_2b | bb_2c | bb_3b | bb_3d | bb_4b | bb_4c;
constexpr BitBoard attacks_by_bki_sq3b = attacks_by_bki_sq3c.shift<DIR_N>();
constexpr BitBoard attacks_by_bki_sq3a = attacks_by_bki_sq3b.shift<DIR_N>();
constexpr BitBoard attacks_by_bki_sq3d = attacks_by_bki_sq3c.shift<DIR_S>();
constexpr BitBoard attacks_by_bki_sq3e = attacks_by_bki_sq3d.shift<DIR_S>();
constexpr BitBoard attacks_by_bki_sq2a = attacks_by_bki_sq3a.shift<DIR_E>();
constexpr BitBoard attacks_by_bki_sq2b = attacks_by_bki_sq3b.shift<DIR_E>();
constexpr BitBoard attacks_by_bki_sq2c = attacks_by_bki_sq3c.shift<DIR_E>();
constexpr BitBoard attacks_by_bki_sq2d = attacks_by_bki_sq3d.shift<DIR_E>();
constexpr BitBoard attacks_by_bki_sq2e = attacks_by_bki_sq3e.shift<DIR_E>();
constexpr BitBoard attacks_by_bki_sq1a = attacks_by_bki_sq2a.shift<DIR_E>();
constexpr BitBoard attacks_by_bki_sq1b = attacks_by_bki_sq2b.shift<DIR_E>();
constexpr BitBoard attacks_by_bki_sq1c = attacks_by_bki_sq2c.shift<DIR_E>();
constexpr BitBoard attacks_by_bki_sq1d = attacks_by_bki_sq2d.shift<DIR_E>();
constexpr BitBoard attacks_by_bki_sq1e = attacks_by_bki_sq2e.shift<DIR_E>();
constexpr BitBoard attacks_by_bki_sq4a = attacks_by_bki_sq3a.shift<DIR_W>();
constexpr BitBoard attacks_by_bki_sq4b = attacks_by_bki_sq3b.shift<DIR_W>();
constexpr BitBoard attacks_by_bki_sq4c = attacks_by_bki_sq3c.shift<DIR_W>();
constexpr BitBoard attacks_by_bki_sq4d = attacks_by_bki_sq3d.shift<DIR_W>();
constexpr BitBoard attacks_by_bki_sq4e = attacks_by_bki_sq3e.shift<DIR_W>();
constexpr BitBoard attacks_by_bki_sq5a = attacks_by_bki_sq4a.shift<DIR_W>();
constexpr BitBoard attacks_by_bki_sq5b = attacks_by_bki_sq4b.shift<DIR_W>();
constexpr BitBoard attacks_by_bki_sq5c = attacks_by_bki_sq4c.shift<DIR_W>();
constexpr BitBoard attacks_by_bki_sq5d = attacks_by_bki_sq4d.shift<DIR_W>();
constexpr BitBoard attacks_by_bki_sq5e = attacks_by_bki_sq4e.shift<DIR_W>();

constexpr BitBoard attacks_by_wki_sq3c
    = bb_2c | bb_2d | bb_3b | bb_3d | bb_4c | bb_4d;
constexpr BitBoard attacks_by_wki_sq3b = attacks_by_wki_sq3c.shift<DIR_N>();
constexpr BitBoard attacks_by_wki_sq3a = attacks_by_wki_sq3b.shift<DIR_N>();
constexpr BitBoard attacks_by_wki_sq3d = attacks_by_wki_sq3c.shift<DIR_S>();
constexpr BitBoard attacks_by_wki_sq3e = attacks_by_wki_sq3d.shift<DIR_S>();
constexpr BitBoard attacks_by_wki_sq2a = attacks_by_wki_sq3a.shift<DIR_E>();
constexpr BitBoard attacks_by_wki_sq2b = attacks_by_wki_sq3b.shift<DIR_E>();
constexpr BitBoard attacks_by_wki_sq2c = attacks_by_wki_sq3c.shift<DIR_E>();
constexpr BitBoard attacks_by_wki_sq2d = attacks_by_wki_sq3d.shift<DIR_E>();
constexpr BitBoard attacks_by_wki_sq2e = attacks_by_wki_sq3e.shift<DIR_E>();
constexpr BitBoard attacks_by_wki_sq1a = attacks_by_wki_sq2a.shift<DIR_E>();
constexpr BitBoard attacks_by_wki_sq1b = attacks_by_wki_sq2b.shift<DIR_E>();
constexpr BitBoard attacks_by_wki_sq1c = attacks_by_wki_sq2c.shift<DIR_E>();
constexpr BitBoard attacks_by_wki_sq1d = attacks_by_wki_sq2d.shift<DIR_E>();
constexpr BitBoard attacks_by_wki_sq1e = attacks_by_wki_sq2e.shift<DIR_E>();
constexpr BitBoard attacks_by_wki_sq4a = attacks_by_wki_sq3a.shift<DIR_W>();
constexpr BitBoard attacks_by_wki_sq4b = attacks_by_wki_sq3b.shift<DIR_W>();
constexpr BitBoard attacks_by_wki_sq4c = attacks_by_wki_sq3c.shift<DIR_W>();
constexpr BitBoard attacks_by_wki_sq4d = attacks_by_wki_sq3d.shift<DIR_W>();
constexpr BitBoard attacks_by_wki_sq4e = attacks_by_wki_sq3e.shift<DIR_W>();
constexpr BitBoard attacks_by_wki_sq5a = attacks_by_wki_sq4a.shift<DIR_W>();
constexpr BitBoard attacks_by_wki_sq5b = attacks_by_wki_sq4b.shift<DIR_W>();
constexpr BitBoard attacks_by_wki_sq5c = attacks_by_wki_sq4c.shift<DIR_W>();
constexpr BitBoard attacks_by_wki_sq5d = attacks_by_wki_sq4d.shift<DIR_W>();
constexpr BitBoard attacks_by_wki_sq5e = attacks_by_wki_sq4e.shift<DIR_W>();

} // namespace internal

constexpr BitBoard attacks_by_fu[Squares::num_squares][num_colors] = {
    {bb_5a.shift<DIR_N>(), bb_5a.shift<DIR_S>()},
    {bb_4a.shift<DIR_N>(), bb_4a.shift<DIR_S>()},
    {bb_3a.shift<DIR_N>(), bb_3a.shift<DIR_S>()},
    {bb_2a.shift<DIR_N>(), bb_2a.shift<DIR_S>()},
    {bb_1a.shift<DIR_N>(), bb_1a.shift<DIR_S>()},
    {bb_5b.shift<DIR_N>(), bb_5b.shift<DIR_S>()},
    {bb_4b.shift<DIR_N>(), bb_4b.shift<DIR_S>()},
    {bb_3b.shift<DIR_N>(), bb_3b.shift<DIR_S>()},
    {bb_2b.shift<DIR_N>(), bb_2b.shift<DIR_S>()},
    {bb_1b.shift<DIR_N>(), bb_1b.shift<DIR_S>()},
    {bb_5c.shift<DIR_N>(), bb_5c.shift<DIR_S>()},
    {bb_4c.shift<DIR_N>(), bb_4c.shift<DIR_S>()},
    {bb_3c.shift<DIR_N>(), bb_3c.shift<DIR_S>()},
    {bb_2c.shift<DIR_N>(), bb_2c.shift<DIR_S>()},
    {bb_1c.shift<DIR_N>(), bb_1c.shift<DIR_S>()},
    {bb_5d.shift<DIR_N>(), bb_5d.shift<DIR_S>()},
    {bb_4d.shift<DIR_N>(), bb_4d.shift<DIR_S>()},
    {bb_3d.shift<DIR_N>(), bb_3d.shift<DIR_S>()},
    {bb_2d.shift<DIR_N>(), bb_2d.shift<DIR_S>()},
    {bb_1d.shift<DIR_N>(), bb_1d.shift<DIR_S>()},
    {bb_5e.shift<DIR_N>(), bb_5e.shift<DIR_S>()},
    {bb_4e.shift<DIR_N>(), bb_4e.shift<DIR_S>()},
    {bb_3e.shift<DIR_N>(), bb_3e.shift<DIR_S>()},
    {bb_2e.shift<DIR_N>(), bb_2e.shift<DIR_S>()},
    {bb_1e.shift<DIR_N>(), bb_1e.shift<DIR_S>()},
};
constexpr BitBoard attacks_by_gi[Squares::num_squares][num_colors] = {
    {internal::attacks_by_bgi_sq5a, internal::attacks_by_wgi_sq5a},
    {internal::attacks_by_bgi_sq4a, internal::attacks_by_wgi_sq4a},
    {internal::attacks_by_bgi_sq3a, internal::attacks_by_wgi_sq3a},
    {internal::attacks_by_bgi_sq2a, internal::attacks_by_wgi_sq2a},
    {internal::attacks_by_bgi_sq1a, internal::attacks_by_wgi_sq1a},
    {internal::attacks_by_bgi_sq5b, internal::attacks_by_wgi_sq5b},
    {internal::attacks_by_bgi_sq4b, internal::attacks_by_wgi_sq4b},
    {internal::attacks_by_bgi_sq3b, internal::attacks_by_wgi_sq3b},
    {internal::attacks_by_bgi_sq2b, internal::attacks_by_wgi_sq2b},
    {internal::attacks_by_bgi_sq1b, internal::attacks_by_wgi_sq1b},
    {internal::attacks_by_bgi_sq5c, internal::attacks_by_wgi_sq5c},
    {internal::attacks_by_bgi_sq4c, internal::attacks_by_wgi_sq4c},
    {internal::attacks_by_bgi_sq3c, internal::attacks_by_wgi_sq3c},
    {internal::attacks_by_bgi_sq2c, internal::attacks_by_wgi_sq2c},
    {internal::attacks_by_bgi_sq1c, internal::attacks_by_wgi_sq1c},
    {internal::attacks_by_bgi_sq5d, internal::attacks_by_wgi_sq5d},
    {internal::attacks_by_bgi_sq4d, internal::attacks_by_wgi_sq4d},
    {internal::attacks_by_bgi_sq3d, internal::attacks_by_wgi_sq3d},
    {internal::attacks_by_bgi_sq2d, internal::attacks_by_wgi_sq2d},
    {internal::attacks_by_bgi_sq1d, internal::attacks_by_wgi_sq1d},
    {internal::attacks_by_bgi_sq5e, internal::attacks_by_wgi_sq5e},
    {internal::attacks_by_bgi_sq4e, internal::attacks_by_wgi_sq4e},
    {internal::attacks_by_bgi_sq3e, internal::attacks_by_wgi_sq3e},
    {internal::attacks_by_bgi_sq2e, internal::attacks_by_wgi_sq2e},
    {internal::attacks_by_bgi_sq1e, internal::attacks_by_wgi_sq1e},
};
constexpr BitBoard attacks_by_ki[Squares::num_squares][num_colors] = {
    {internal::attacks_by_bki_sq5a, internal::attacks_by_wki_sq5a},
    {internal::attacks_by_bki_sq4a, internal::attacks_by_wki_sq4a},
    {internal::attacks_by_bki_sq3a, internal::attacks_by_wki_sq3a},
    {internal::attacks_by_bki_sq2a, internal::attacks_by_wki_sq2a},
    {internal::attacks_by_bki_sq1a, internal::attacks_by_wki_sq1a},
    {internal::attacks_by_bki_sq5b, internal::attacks_by_wki_sq5b},
    {internal::attacks_by_bki_sq4b, internal::attacks_by_wki_sq4b},
    {internal::attacks_by_bki_sq3b, internal::attacks_by_wki_sq3b},
    {internal::attacks_by_bki_sq2b, internal::attacks_by_wki_sq2b},
    {internal::attacks_by_bki_sq1b, internal::attacks_by_wki_sq1b},
    {internal::attacks_by_bki_sq5c, internal::attacks_by_wki_sq5c},
    {internal::attacks_by_bki_sq4c, internal::attacks_by_wki_sq4c},
    {internal::attacks_by_bki_sq3c, internal::attacks_by_wki_sq3c},
    {internal::attacks_by_bki_sq2c, internal::attacks_by_wki_sq2c},
    {internal::attacks_by_bki_sq1c, internal::attacks_by_wki_sq1c},
    {internal::attacks_by_bki_sq5d, internal::attacks_by_wki_sq5d},
    {internal::attacks_by_bki_sq4d, internal::attacks_by_wki_sq4d},
    {internal::attacks_by_bki_sq3d, internal::attacks_by_wki_sq3d},
    {internal::attacks_by_bki_sq2d, internal::attacks_by_wki_sq2d},
    {internal::attacks_by_bki_sq1d, internal::attacks_by_wki_sq1d},
    {internal::attacks_by_bki_sq5e, internal::attacks_by_wki_sq5e},
    {internal::attacks_by_bki_sq4e, internal::attacks_by_wki_sq4e},
    {internal::attacks_by_bki_sq3e, internal::attacks_by_wki_sq3e},
    {internal::attacks_by_bki_sq2e, internal::attacks_by_wki_sq2e},
    {internal::attacks_by_bki_sq1e, internal::attacks_by_wki_sq1e},
};
constexpr BitBoard attacks_by_ou[Squares::num_squares] = {
    bb_5a.expand() & (~bb_5a), bb_4a.expand() & (~bb_4a),
    bb_3a.expand() & (~bb_3a), bb_2a.expand() & (~bb_2a),
    bb_1a.expand() & (~bb_1a), bb_5b.expand() & (~bb_5b),
    bb_4b.expand() & (~bb_4b), bb_3b.expand() & (~bb_3b),
    bb_2b.expand() & (~bb_2b), bb_1b.expand() & (~bb_1b),
    bb_5c.expand() & (~bb_5c), bb_4c.expand() & (~bb_4c),
    bb_3c.expand() & (~bb_3c), bb_2c.expand() & (~bb_2c),
    bb_1c.expand() & (~bb_1c), bb_5d.expand() & (~bb_5d),
    bb_4d.expand() & (~bb_4d), bb_3d.expand() & (~bb_3d),
    bb_2d.expand() & (~bb_2d), bb_1d.expand() & (~bb_1d),
    bb_5e.expand() & (~bb_5e), bb_4e.expand() & (~bb_4e),
    bb_3e.expand() & (~bb_3e), bb_2e.expand() & (~bb_2e),
    bb_1e.expand() & (~bb_1e),
};

BitBoard BitBoard::get_attacks_by(
    const Pieces::BoardPieceTypeEnum piece,
    const Squares::SquareEnum location,
    const BitBoard occupied)
{
    const auto piece_type = Pieces::to_piece_type(piece);
    const auto color = Pieces::get_color(piece);
    switch (piece_type) {
    case Pieces::FU:
        return attacks_by_fu[location][color];
    case Pieces::GI:
        return attacks_by_gi[location][color];
    case Pieces::KA:
        return BitBoard::ranging_attacks_to_diagonal(location, occupied);
    case Pieces::HI:
        return BitBoard::ranging_attacks_to_adjacent(location, occupied);
    case Pieces::KI:
    case Pieces::TO:
    case Pieces::NG:
        return attacks_by_ki[location][color];
    case Pieces::UM:
        return attacks_by_ou[location]
               | BitBoard::ranging_attacks_to_diagonal(location, occupied);
    case Pieces::RY:
        return attacks_by_ou[location]
               | BitBoard::ranging_attacks_to_adjacent(location, occupied);
    case Pieces::OU:
        return attacks_by_ou[location];
    default:
        break;
    }
    return BitBoard(0);
}

} // namespace vshogi::minishogi
