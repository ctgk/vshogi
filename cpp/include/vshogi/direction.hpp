#ifndef VSHOGI_DIRECTION_HPP
#define VSHOGI_DIRECTION_HPP

namespace vshogi
{

enum DirectionEnum
{
    // clang-format off
    DIR_NNW =10,             DIR_NNE =11,
     DIR_NW = 0,  DIR_N = 1,  DIR_NE = 2,
      DIR_W = 3,               DIR_E = 4,
     DIR_SW = 5,  DIR_S = 6,  DIR_SE = 7,
    DIR_SSW = 8,             DIR_SSE = 9, // knights come from SSW or SSE.
    // clang-format on
    DIR_NA = -1,
};

} // namespace vshogi

#endif // VSHOGI_DIRECTION_HPP
