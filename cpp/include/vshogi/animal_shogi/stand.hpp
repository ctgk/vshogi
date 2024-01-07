#ifndef VSHOGI_ANIMAL_SHOGI_STAND_HPP
#define VSHOGI_ANIMAL_SHOGI_STAND_HPP

#include <cstdint>
#include <string>

#include "vshogi/animal_shogi/piece.hpp"
#include "vshogi/color.hpp"
#include "vshogi/stand.hpp"

namespace vshogi::animal_shogi
{

/**
 * @brief 8bit integer representing what is on a piece stand.
 * @details
 * ______11  Chick (2bits for 0, 1, or 2 pieces)
 * ___11___  Elephant (2 bits for 0, 1, or 2 pieces)
 * 11______  Giraffe (2 bits for 0, 1, or 2 pieces)
 */
using Stand = vshogi::Stand<Pieces, std::uint8_t>;

using BlackWhiteStands = vshogi::BlackWhiteStands<Stand, Pieces>;

} // namespace vshogi::animal_shogi

namespace vshogi
{

template <>
inline const int animal_shogi::Stand::shift_bits[] = {
    0, // CH
    3, // EL
    6, // GI
};
template <>
inline const std::uint8_t animal_shogi::Stand::masks[] = {
    0b00000011, // CH
    0b00011000, // EL
    0b11000000, // GI
};
template <>
inline const std::uint8_t animal_shogi::Stand::deltas[] = {
    0b00000001, // CH
    0b00001000, // EL
    0b01000000, // GI
};
template <>
inline const std::uint8_t animal_shogi::Stand::mask = 0b11011011;

template <>
template <>
inline animal_shogi::Stand::Stand(
    const int num_ch, const int num_el, const int num_gi)
    : Stand(static_cast<std::uint8_t>(
        (num_ch << shift_bits[animal_shogi::CH])
        + (num_el << shift_bits[animal_shogi::EL])
        + (num_gi << shift_bits[animal_shogi::GI])))
{
}

template <>
inline const animal_shogi::PieceTypeEnum
    animal_shogi::BlackWhiteStands::stand_pieces_in_sfen_order[]
    = {animal_shogi::GI, animal_shogi::EL, animal_shogi::CH};

template <>
inline const int animal_shogi::BlackWhiteStands::max_sfen_length
    = 13; // "2C2E2G2c2e2g "

} // namespace vshogi

#endif // VSHOGI_ANIMAL_SHOGI_STAND_HPP
