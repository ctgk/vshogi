#ifndef VSHOGI_COMMON_ITERATOR_ITERATOR_HPP
#define VSHOGI_COMMON_ITERATOR_ITERATOR_HPP

#include <cstdint>

namespace vshogi
{

enum class IterEnum : std::uint8_t
{
    LEGAL = 0u, //!< iterate all moves.
    CHECK = 1u, //!< iterate moves that give check to the enemy king.
    EVADE = 2u, //!< iterate moves that evade check to my king.
};

} // namespace vshogi

#endif // VSHOGI_COMMON_ITERATOR_ITERATOR_HPP
