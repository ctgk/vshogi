#ifndef VSHOGI_COMMON_ITERATOR_HPP
#define VSHOGI_COMMON_ITERATOR_HPP

#include <cassert>

#include "vshogi/common/bitboard.hpp"
#include "vshogi/common/board.hpp"
#include "vshogi/common/color.hpp"
#include "vshogi/common/iterator/chained_iterator.hpp"
#include "vshogi/common/iterator/drop.hpp"
#include "vshogi/common/iterator/iterator.hpp"
#include "vshogi/common/iterator/king.hpp"
#include "vshogi/common/iterator/soldier.hpp"
#include "vshogi/common/magic.hpp"
#include "vshogi/common/move.hpp"
#include "vshogi/common/square_traits.hpp"
#include "vshogi/common/state.hpp"
#include "vshogi/common/utils.hpp"

namespace vshogi
{

template <class P, IterEnum IterType = IterEnum::LEGAL>
using BoardMoveIterator = ChainedIterator<
    P,
    KingMoveIterator<P, IterType>,
    SoldierMoveIterator<P, IterType>>;

template <class P, IterEnum IterType = IterEnum::LEGAL>
using MoveGenerator = ChainedIterator<
    P,
    KingMoveIterator<P, IterType>,
    SoldierMoveIterator<P, IterType>,
    DropMoveIterator<P, IterType>>;

} // namespace vshogi

#endif // VSHOGI_COMMON_ITERATOR_HPP
