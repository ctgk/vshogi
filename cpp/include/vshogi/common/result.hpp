#ifndef VSHOGI_RESULT_HPP
#define VSHOGI_RESULT_HPP

#include <cstdint>

namespace vshogi
{

enum ResultEnum : std::uint8_t
{
    ONGOING,
    DRAW,
    BLACK_WIN,
    WHITE_WIN,
    UNKNOWN,
};

} // namespace vshogi

#endif // VSHOGI_RESULT_HPP
