#ifndef VSHOGI_COMMON_GENERATORS_GENTYPE_HPP
#define VSHOGI_COMMON_GENERATORS_GENTYPE_HPP

#include <cstdint>

namespace vshogi
{

enum class GenEnum : std::uint8_t
{
    LEGAL = 0u, //!< generate all moves.
    CHECK = 1u, //!< generate moves that give check to the enemy king.
    EVADE = 2u, //!< generate moves that evade check to my king.
};

} // namespace vshogi

#endif // VSHOGI_COMMON_GENERATORS_GENTYPE_HPP
