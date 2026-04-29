#pragma once

#include "vshogi/common/color.hpp"
#include "vshogi/common/result.hpp"
#include "vshogi/common/notation.hpp"
#include "vshogi/common/game.hpp"
#include "vshogi/engine/az/searcher.hpp"
#include "vshogi/engine/dfpn/searcher.hpp"
#include "vshogi/engine/gaz/searcher.hpp"

namespace vshogi {

// ColorEnum
using ColorEnum = vshogi::ColorEnum;
constexpr ColorEnum BLACK = vshogi::BLACK;
constexpr ColorEnum WHITE = vshogi::WHITE;

// ResultEnum
using ResultEnum = vshogi::ResultEnum;
constexpr ResultEnum ONGOING = vshogi::ONGOING;
constexpr ResultEnum DRAW = vshogi::DRAW;
constexpr ResultEnum BLACK_WIN = vshogi::BLACK_WIN;
constexpr ResultEnum WHITE_WIN = vshogi::WHITE_WIN;

} // namespace vshogi