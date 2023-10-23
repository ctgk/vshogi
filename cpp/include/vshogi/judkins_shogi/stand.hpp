#ifndef VSHOGI_JUDKINS_SHOGI_STAND_HPP
#define VSHOGI_JUDKINS_SHOGI_STAND_HPP

#include <cstdint>

#include "vshogi/judkins_shogi/piece.hpp"
#include "vshogi/stand.hpp"

namespace vshogi::judkins_shogi
{

/**
 * @brief 16-bit integer representing pieces on a stand.
 * @details
 * ________ ______**  FU (2 pieces)
 * ________ ____**__  KE (2 pieces)
 * ________ __**____  GI (2 pieces)
 * ________ **______  KA (2 pieces)
 * ______** ________  HI (2 pieces)
 * ____**__ ________  KI (2 pieces)
 */
using Stand = vshogi::Stand<Pieces, std::uint16_t>;

using BlackWhiteStands = vshogi::BlackWhiteStands<Stand, Pieces>;

} // namespace vshogi::judkins_shogi

namespace vshogi
{

template <>
inline const int judkins_shogi::Stand::shift_bits[] = {0, 2, 4, 6, 8, 10};

template <>
inline const std::uint16_t judkins_shogi::Stand::masks[] = {
    // clang-format off
    0x0003, // FU
    0x000c, // KE
    0x0030, // GI
    0x00c0, // KA
    0x0300, // HI
    0x0c00, // KI
    // clang-format on
};

template <>
inline const std::uint16_t judkins_shogi::Stand::deltas[] = {
    // clang-format off
    0x0001, // FU
    0x0004, // KE
    0x0010, // GI
    0x0040, // KA
    0x0100, // HI
    0x0400, // KI
    // clang-format on
};

template <>
inline const std::uint16_t judkins_shogi::Stand::mask = 0x0fff;

template <>
template <>
inline judkins_shogi::Stand::Stand(
    const int num_fu,
    const int num_ke,
    const int num_gi,
    const int num_ka,
    const int num_hi,
    const int num_ki)
    : Stand(static_cast<std::uint16_t>(
        (num_fu << shift_bits[judkins_shogi::Pieces::FU])
        + (num_ke << shift_bits[judkins_shogi::Pieces::KE])
        + (num_gi << shift_bits[judkins_shogi::Pieces::GI])
        + (num_ka << shift_bits[judkins_shogi::Pieces::KA])
        + (num_hi << shift_bits[judkins_shogi::Pieces::HI])
        + (num_ki << shift_bits[judkins_shogi::Pieces::KI])))
{
}

template <>
inline const judkins_shogi::Pieces::PieceTypeEnum
    judkins_shogi::BlackWhiteStands::stand_pieces_in_sfen_order[]
    = {judkins_shogi::Pieces::HI,
       judkins_shogi::Pieces::KA,
       judkins_shogi::Pieces::KI,
       judkins_shogi::Pieces::GI,
       judkins_shogi::Pieces::KE,
       judkins_shogi::Pieces::FU};

template <>
inline const int judkins_shogi::BlackWhiteStands::max_sfen_length
    = 13; // "RBGSNPrbgsnp "

} // namespace vshogi

#endif // VSHOGI_JUDKINS_SHOGI_STAND_HPP
