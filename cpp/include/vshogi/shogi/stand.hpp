#ifndef VSHOGI_SHOGI_STAND_HPP
#define VSHOGI_SHOGI_STAND_HPP

#include <cstdint>

#include "vshogi/shogi/piece.hpp"
#include "vshogi/stand.hpp"

namespace vshogi::shogi
{

/**
 * @brief 32-bit integer representing pieces on a stand.
 * @details
 * ________ ________ ________ ___*****  FU (18 pieces)
 * ________ ________ _______* **______  KY (4 pieces)
 * ________ ________ ___***__ ________  KE (4 pieces)
 * ________ _______* **______ ________  GI (4 pieces)
 * ________ ____**__ ________ ________  KA (2 pieces)
 * ________ _**_____ ________ ________  HI (2 pieces)
 * _____*** ________ ________ ________  KI (4 pieces)
 */
using Stand = vshogi::Stand<Pieces, std::uint32_t>;

using BlackWhiteStands = vshogi::BlackWhiteStands<Stand, Pieces>;

} // namespace vshogi::shogi

namespace vshogi
{

template <>
inline const int shogi::Stand::shift_bits[] = {0, 6, 10, 14, 18, 21, 24};

template <>
inline const std::uint32_t shogi::Stand::masks[] = {
    // clang-format off
    0x0000001f, // FU
    0x000001c0, // KY
    0x00001c00, // KE
    0x0001c000, // GI
    0x000c0000, // KA
    0x00600000, // HI
    0x07000000, // KI
    // clang-format on
};

template <>
inline const std::uint32_t shogi::Stand::deltas[] = {
    // clang-format off
    0x00000001, // FU
    0x00000040, // KY
    0x00000400, // KE
    0x00004000, // GI
    0x00040000, // KA
    0x00200000, // HI
    0x01000000, // KI
    // clang-format on
};

template <>
inline const std::uint32_t shogi::Stand::mask = 0x076ddddf;

template <>
template <>
inline shogi::Stand::Stand(
    const int num_fu,
    const int num_ky,
    const int num_ke,
    const int num_gi,
    const int num_ka,
    const int num_hi,
    const int num_ki)
    : Stand(static_cast<std::uint32_t>(
        (num_fu << shift_bits[shogi::Pieces::FU])
        + (num_ky << shift_bits[shogi::Pieces::KY])
        + (num_ke << shift_bits[shogi::Pieces::KE])
        + (num_gi << shift_bits[shogi::Pieces::GI])
        + (num_ka << shift_bits[shogi::Pieces::KA])
        + (num_hi << shift_bits[shogi::Pieces::HI])
        + (num_ki << shift_bits[shogi::Pieces::KI])))
{
}

template <>
inline const shogi::Pieces::PieceTypeEnum
    shogi::BlackWhiteStands::stand_pieces_in_sfen_order[]
    = {shogi::Pieces::HI,
       shogi::Pieces::KA,
       shogi::Pieces::KI,
       shogi::Pieces::GI,
       shogi::Pieces::KE,
       shogi::Pieces::KY,
       shogi::Pieces::FU};
template <>
inline const int shogi::BlackWhiteStands::max_sfen_length
    = 26; // "10p2l2n2sbr2g2P2L2N2SBR2G "

} // namespace vshogi

#endif // VSHOGI_SHOGI_STAND_HPP
