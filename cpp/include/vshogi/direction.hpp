#ifndef VSHOGI_DIRECTION_HPP
#define VSHOGI_DIRECTION_HPP

namespace vshogi
{

enum DirectionEnum
{
    // clang-format off
    DIR_NNW = 8,             DIR_NNE = 9,
     DIR_NW = 0,  DIR_N = 1,  DIR_NE = 2,
      DIR_W = 3,               DIR_E = 4,
     DIR_SW = 5,  DIR_S = 6,  DIR_SE = 7,
    DIR_SSW =10,             DIR_SSE = 11,
    // clang-format on
};

} // namespace vshogi

#endif // VSHOGI_DIRECTION_HPP
