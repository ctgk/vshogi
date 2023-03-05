/*
  Apery, a USI shogi playing engine derived from Stockfish, a UCI chess playing engine.
  Copyright (C) 2004-2008 Tord Romstad (Glaurung author)
  Copyright (C) 2008-2015 Marco Costalba, Joona Kiiski, Tord Romstad
  Copyright (C) 2015-2018 Marco Costalba, Joona Kiiski, Gary Linscott, Tord Romstad
  Copyright (C) 2011-2018 Hiraoka Takuya

  Apery is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  Apery is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef APERY_COLOR_HPP
#define APERY_COLOR_HPP

#include <cstdint>

#include "overloadEnumOperators.hpp"

namespace color
{

enum ColorEnum : std::uint8_t
{
    BLACK,
    WHITE,
    NUM_COLORS,
};
OverloadEnumOperators(ColorEnum);

inline constexpr ColorEnum opposite(const ColorEnum c)
{
    return static_cast<ColorEnum>(static_cast<std::uint8_t>(c) ^ 1);
}

inline constexpr ColorEnum operator~(const ColorEnum c)
{
    return opposite(c);
}

} // namespace color

#endif // #ifndef APERY_COLOR_HPP
