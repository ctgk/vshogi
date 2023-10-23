#ifndef VSHOGI_MINISHOGI_STAND_HPP
#define VSHOGI_MINISHOGI_STAND_HPP

#include <cstdint>

#include "vshogi/minishogi/piece.hpp"
#include "vshogi/stand.hpp"

namespace vshogi::minishogi
{

/**
 * @brief 16-bit integer representing pieces on a stand.
 * @details
 * ________ ______11  FU (2-bit for 0, 1, or 2 pieces)
 * ________ ___11___  GI (2-bit)
 * ________ 11______  KA (2-bit)
 * _____11_ ________  HI (2-bit)
 * __11____ ________  KI (2-bit)
 */
using Stand = vshogi::Stand<Pieces, std::uint16_t>;

using BlackWhiteStands = vshogi::BlackWhiteStands<Stand, Pieces>;

} // namespace vshogi::minishogi

namespace vshogi
{

template <>
inline const int minishogi::Stand::shift_bits[] = {0, 3, 6, 9, 12};

template <>
inline const std::uint16_t minishogi::Stand::masks[] = {
    // clang-format off
    0b0000000000000011,
    0b0000000000011000,
    0b0000000011000000,
    0b0000011000000000,
    0b0011000000000000,
    // clang-format on
};

template <>
inline const std::uint16_t minishogi::Stand::deltas[] = {
    // clang-format off
    0b0000000000000001,
    0b0000000000001000,
    0b0000000001000000,
    0b0000001000000000,
    0b0001000000000000,
    // clang-format on
};

template <>
inline const std::uint16_t minishogi::Stand::mask = 0b0011011011011011;

template <>
template <>
inline minishogi::Stand::Stand(
    const int num_fu,
    const int num_gi,
    const int num_ka,
    const int num_hi,
    const int num_ki)
    : Stand(static_cast<std::uint16_t>(
        (num_ki << shift_bits[minishogi::Pieces::KI])
        + (num_hi << shift_bits[minishogi::Pieces::HI])
        + (num_ka << shift_bits[minishogi::Pieces::KA])
        + (num_gi << shift_bits[minishogi::Pieces::GI])
        + (num_fu << shift_bits[minishogi::Pieces::FU])))
{
}

template <>
inline const minishogi::Pieces::PieceTypeEnum
    minishogi::BlackWhiteStands::stand_pieces_in_sfen_order[]
    = {minishogi::Pieces::HI,
       minishogi::Pieces::KA,
       minishogi::Pieces::KI,
       minishogi::Pieces::GI,
       minishogi::Pieces::FU};
template <>
inline const int minishogi::BlackWhiteStands::max_sfen_length
    = 11; // "2p2s2g2b2r "

} // namespace vshogi

#endif // VSHOGI_MINISHOGI_STAND_HPP
