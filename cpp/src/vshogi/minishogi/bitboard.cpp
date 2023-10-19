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
constexpr BitBoard attacks_by_hi[Squares::num_squares] = {
    (bb_f5 | bb_ra) & (~bb_5a), (bb_f4 | bb_ra) & (~bb_4a),
    (bb_f3 | bb_ra) & (~bb_3a), (bb_f2 | bb_ra) & (~bb_2a),
    (bb_f1 | bb_ra) & (~bb_1a), (bb_f5 | bb_rb) & (~bb_5b),
    (bb_f4 | bb_rb) & (~bb_4b), (bb_f3 | bb_rb) & (~bb_3b),
    (bb_f2 | bb_rb) & (~bb_2b), (bb_f1 | bb_rb) & (~bb_1b),
    (bb_f5 | bb_rc) & (~bb_5c), (bb_f4 | bb_rc) & (~bb_4c),
    (bb_f3 | bb_rc) & (~bb_3c), (bb_f2 | bb_rc) & (~bb_2c),
    (bb_f1 | bb_rc) & (~bb_1c), (bb_f5 | bb_rd) & (~bb_5d),
    (bb_f4 | bb_rd) & (~bb_4d), (bb_f3 | bb_rd) & (~bb_3d),
    (bb_f2 | bb_rd) & (~bb_2d), (bb_f1 | bb_rd) & (~bb_1d),
    (bb_f5 | bb_re) & (~bb_5e), (bb_f4 | bb_re) & (~bb_4e),
    (bb_f3 | bb_re) & (~bb_3e), (bb_f2 | bb_re) & (~bb_2e),
    (bb_f1 | bb_re) & (~bb_1e),
};

namespace internal
{

constexpr BitBoard diagonal_5a_to_se = bb_5a | bb_4b | bb_3c | bb_2d | bb_1e;
constexpr BitBoard diagonal_4a_to_se = diagonal_5a_to_se.shift<DIR_N>();
constexpr BitBoard diagonal_3a_to_se = diagonal_4a_to_se.shift<DIR_N>();
constexpr BitBoard diagonal_2a_to_se = diagonal_3a_to_se.shift<DIR_N>();
constexpr BitBoard diagonal_1a_to_se = bb_1a;
constexpr BitBoard diagonal_5b_to_se = diagonal_5a_to_se.shift<DIR_S>();
constexpr BitBoard diagonal_5c_to_se = diagonal_5b_to_se.shift<DIR_S>();
constexpr BitBoard diagonal_5d_to_se = diagonal_5c_to_se.shift<DIR_S>();
constexpr BitBoard diagonal_5e_to_se = bb_5e;

constexpr BitBoard diagonal_1a_to_sw = bb_1a | bb_2b | bb_3c | bb_4d | bb_5e;
constexpr BitBoard diagonal_2a_to_sw = diagonal_1a_to_sw.shift<DIR_N>();
constexpr BitBoard diagonal_3a_to_sw = diagonal_2a_to_sw.shift<DIR_N>();
constexpr BitBoard diagonal_4a_to_sw = diagonal_3a_to_sw.shift<DIR_N>();
constexpr BitBoard diagonal_5a_to_sw = bb_5a;
constexpr BitBoard diagonal_1b_to_sw = diagonal_1a_to_sw.shift<DIR_S>();
constexpr BitBoard diagonal_1c_to_sw = diagonal_1b_to_sw.shift<DIR_S>();
constexpr BitBoard diagonal_1d_to_sw = diagonal_1c_to_sw.shift<DIR_S>();
constexpr BitBoard diagonal_1e_to_sw = bb_1e;

} // namespace internal

constexpr BitBoard attacks_by_ka[Squares::num_squares] = {
    (internal::diagonal_5a_to_se | internal::diagonal_5a_to_sw) & (~bb_5a),
    (internal::diagonal_4a_to_se | internal::diagonal_4a_to_sw) & (~bb_4a),
    (internal::diagonal_3a_to_se | internal::diagonal_3a_to_sw) & (~bb_3a),
    (internal::diagonal_2a_to_se | internal::diagonal_2a_to_sw) & (~bb_2a),
    (internal::diagonal_1a_to_se | internal::diagonal_1a_to_sw) & (~bb_1a),
    (internal::diagonal_5b_to_se | internal::diagonal_4a_to_sw) & (~bb_5b),
    (internal::diagonal_5a_to_se | internal::diagonal_3a_to_sw) & (~bb_4b),
    (internal::diagonal_4a_to_se | internal::diagonal_2a_to_sw) & (~bb_3b),
    (internal::diagonal_3a_to_se | internal::diagonal_1a_to_sw) & (~bb_2b),
    (internal::diagonal_2a_to_se | internal::diagonal_1b_to_sw) & (~bb_1b),
    (internal::diagonal_5c_to_se | internal::diagonal_3a_to_sw) & (~bb_5c),
    (internal::diagonal_5b_to_se | internal::diagonal_2a_to_sw) & (~bb_4c),
    (internal::diagonal_5a_to_se | internal::diagonal_1a_to_sw) & (~bb_3c),
    (internal::diagonal_4a_to_se | internal::diagonal_1b_to_sw) & (~bb_2c),
    (internal::diagonal_3a_to_se | internal::diagonal_1c_to_sw) & (~bb_1c),
    (internal::diagonal_5d_to_se | internal::diagonal_2a_to_sw) & (~bb_5d),
    (internal::diagonal_5c_to_se | internal::diagonal_1a_to_sw) & (~bb_4d),
    (internal::diagonal_5b_to_se | internal::diagonal_1b_to_sw) & (~bb_3d),
    (internal::diagonal_5a_to_se | internal::diagonal_1c_to_sw) & (~bb_2d),
    (internal::diagonal_4a_to_se | internal::diagonal_1d_to_sw) & (~bb_1d),
    (internal::diagonal_5e_to_se | internal::diagonal_1a_to_sw) & (~bb_5e),
    (internal::diagonal_5d_to_se | internal::diagonal_1b_to_sw) & (~bb_4e),
    (internal::diagonal_5c_to_se | internal::diagonal_1c_to_sw) & (~bb_3e),
    (internal::diagonal_5b_to_se | internal::diagonal_1d_to_sw) & (~bb_2e),
    (internal::diagonal_5a_to_se | internal::diagonal_1e_to_sw) & (~bb_1e),
};

BitBoard BitBoard::get_attacks_by(
    const Pieces::BoardPieceTypeEnum& piece,
    const Squares::SquareEnum& location)
{
    const auto piece_type = Pieces::to_piece_type(piece);
    const auto color = Pieces::get_color(piece);
    switch (piece_type) {
    case Pieces::FU:
        return attacks_by_fu[location][color];
    case Pieces::GI:
        return attacks_by_gi[location][color];
    case Pieces::KA:
        return attacks_by_ka[location];
    case Pieces::HI:
        return attacks_by_hi[location];
    case Pieces::KI:
    case Pieces::TO:
    case Pieces::NG:
        return attacks_by_ki[location][color];
    case Pieces::UM:
        return attacks_by_ou[location] | attacks_by_ka[location];
    case Pieces::RY:
        return attacks_by_ou[location] | attacks_by_hi[location];
    case Pieces::OU:
        return attacks_by_ou[location];
    default:
        break;
    }
    return BitBoard(0);
}

BitBoard BitBoard::get_attacks_by(
    const Pieces::BoardPieceTypeEnum& piece,
    const Squares::SquareEnum& location,
    const BitBoard& occupied)
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

/**
 * @brief Table of attacks by non-ranging pieces:
 * B_FU, B_GI, B_KI, W_FU, W_GI, W_KI, and OU.
 */
static Squares::SquareEnum non_ranging_attacks[7][Squares::num_squares][8];

static Squares::SquareEnum ranging_squares_to[Squares::num_squares]
                                             [Squares::num_directions][5];

void init_non_ranging_attacks_table()
{
    constexpr int size
        = sizeof(non_ranging_attacks) / sizeof(non_ranging_attacks[0][0][0]);
    std::fill_n(&non_ranging_attacks[0][0][0], size, Squares::SQ_NA);

    constexpr Pieces::BoardPieceTypeEnum non_ranging_pieces[]
        = {Pieces::B_FU,
           Pieces::B_GI,
           Pieces::B_KI,
           Pieces::W_FU,
           Pieces::W_GI,
           Pieces::W_KI,
           Pieces::B_OU};
    constexpr int num
        = sizeof(non_ranging_pieces) / sizeof(non_ranging_pieces[0]);
    for (int ii = 0; ii < num; ++ii) {
        const auto p = non_ranging_pieces[ii];
        for (auto& src : Squares::square_array) {
            const auto attacks = BitBoard::get_attacks_by(p, src);
            int index = 0;
            for (auto& dst : Squares::square_array) {
                if (attacks.is_one(dst))
                    non_ranging_attacks[ii][src][index++] = dst;
            }
        }
    }
}

void init_ranging_squares_table()
{
    constexpr int size
        = sizeof(ranging_squares_to) / sizeof(ranging_squares_to[0][0][0]);
    std::fill_n(&ranging_squares_to[0][0][0], size, Squares::SQ_NA);

    for (auto& src : Squares::square_array) {
        for (auto& dir : Squares::direction_array) {
            auto dst = src;
            int index = 0;
            while (true) {
                dst = Squares::shift(dst, dir);
                if (dst == Squares::SQ_NA)
                    break;
                ranging_squares_to[src][dir][index++] = dst;
            }
        }
    }
}

const Squares::SquareEnum* BitBoard::get_attacks_by_non_ranging(
    const Pieces::BoardPieceTypeEnum& piece,
    const Squares::SquareEnum& location)
{
    switch (piece) {
    case Pieces::B_FU:
        return non_ranging_attacks[0][location];
    case Pieces::B_GI:
        return non_ranging_attacks[1][location];
    case Pieces::B_KI:
    case Pieces::B_TO:
    case Pieces::B_NG:
        return non_ranging_attacks[2][location];
    case Pieces::W_FU:
        return non_ranging_attacks[3][location];
    case Pieces::W_GI:
        return non_ranging_attacks[4][location];
    case Pieces::W_KI:
    case Pieces::W_TO:
    case Pieces::W_NG:
        return non_ranging_attacks[5][location];
    case Pieces::B_OU:
    case Pieces::W_OU:
        return non_ranging_attacks[6][location];
    default:
        return nullptr;
    }
}

const Squares::SquareEnum* BitBoard::get_squares_along(
    const DirectionEnum& direction, const Squares::SquareEnum& location)
{
    if ((direction == DIR_NA) || (location == Squares::SQ_NA))
        return nullptr;
    return ranging_squares_to[location][direction];
}

} // namespace vshogi::minishogi
