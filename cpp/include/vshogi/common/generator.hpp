#ifndef VSHOGI_COMMON_GENERATOR_HPP
#define VSHOGI_COMMON_GENERATOR_HPP

#include "vshogi/common/generators/chain.hpp"
#include "vshogi/common/generators/drop.hpp"
#include "vshogi/common/generators/gentype.hpp"
#include "vshogi/common/generators/king.hpp"
#include "vshogi/common/generators/soldier.hpp"

namespace vshogi
{

template <class P, GenEnum GenType = GenEnum::LEGAL>
using BoardMoveGenerator = ChainGenerator<
    P,
    KingMoveGenerator<P, GenType>,
    SoldierMoveGenerator<P, GenType>>;

template <class P, GenEnum GenType = GenEnum::LEGAL>
using MoveGenerator = ChainGenerator<
    P,
    KingMoveGenerator<P, GenType>,
    SoldierMoveGenerator<P, GenType>,
    DropMoveGenerator<P, GenType>>;

} // namespace vshogi

#endif // VSHOGI_COMMON_GENERATOR_HPP
