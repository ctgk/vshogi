#ifndef VSHOGI_COMMON_GENERATOR_HPP
#define VSHOGI_COMMON_GENERATOR_HPP

#include <cassert>

#include "vshogi/common/bitboard.hpp"
#include "vshogi/common/board.hpp"
#include "vshogi/common/color.hpp"
#include "vshogi/common/generators/chain.hpp"
#include "vshogi/common/generators/drop.hpp"
#include "vshogi/common/generators/gentype.hpp"
#include "vshogi/common/generators/king.hpp"
#include "vshogi/common/generators/soldier.hpp"
#include "vshogi/common/magic.hpp"
#include "vshogi/common/move.hpp"
#include "vshogi/common/square_traits.hpp"
#include "vshogi/common/state.hpp"
#include "vshogi/common/utils.hpp"

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
