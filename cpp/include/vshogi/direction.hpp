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

inline bool has_dir_n(const DirectionEnum& d)
{
    return (
        (d == DIR_NNW) || (d == DIR_NNE) || (d == DIR_NW) || (d == DIR_N)
        || (d == DIR_NE));
}

inline bool has_dir_s(const DirectionEnum& d)
{
    return (
        (d == DIR_SSW) || (d == DIR_SSE) || (d == DIR_SW) || (d == DIR_S)
        || (d == DIR_SE));
}

inline bool has_dir_w(const DirectionEnum& d)
{
    return (
        (d == DIR_NNW) || (d == DIR_SSW) || (d == DIR_NW) || (d == DIR_W)
        || (d == DIR_SW));
}

inline bool has_dir_e(const DirectionEnum& d)
{
    return (
        (d == DIR_NNE) || (d == DIR_SSE) || (d == DIR_NE) || (d == DIR_E)
        || (d == DIR_SE));
}

} // namespace vshogi

#endif // VSHOGI_DIRECTION_HPP
