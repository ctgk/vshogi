#ifndef VSHOGI_COMMON_UTILS_HPP
#define VSHOGI_COMMON_UTILS_HPP

#include <algorithm>
#include <cmath>
#include <vector>

namespace vshogi
{

using uint = unsigned int;

inline void softmax(std::vector<float>& logits)
{
    if (logits.empty())
        return;
    const float maximum_value
        = *std::max_element(logits.cbegin(), logits.cend());
    float sum = 0.f;
    for (auto&& e : logits) {
        e -= maximum_value;
        e = std::exp(e);
        sum += e;
    }
    for (auto&& e : logits) {
        e /= sum;
    }
}

} // namespace vshogi

#endif // VSHOGI_COMMON_UTILS_HPP
