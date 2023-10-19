#include "vshogi/shogi/bitboard.hpp"

namespace vshogi::shogi
{

constexpr BitBoard attacks_by_fu[Squares::num_squares][num_colors] = {
    {bb_9a.shift<DIR_N>(), bb_9a.shift<DIR_S>()},
    {bb_8a.shift<DIR_N>(), bb_8a.shift<DIR_S>()},
    {bb_7a.shift<DIR_N>(), bb_7a.shift<DIR_S>()},
    {bb_6a.shift<DIR_N>(), bb_6a.shift<DIR_S>()},
    {bb_5a.shift<DIR_N>(), bb_5a.shift<DIR_S>()},
    {bb_4a.shift<DIR_N>(), bb_4a.shift<DIR_S>()},
    {bb_3a.shift<DIR_N>(), bb_3a.shift<DIR_S>()},
    {bb_2a.shift<DIR_N>(), bb_2a.shift<DIR_S>()},
    {bb_1a.shift<DIR_N>(), bb_1a.shift<DIR_S>()},
    {bb_9b.shift<DIR_N>(), bb_9b.shift<DIR_S>()},
    {bb_8b.shift<DIR_N>(), bb_8b.shift<DIR_S>()},
    {bb_7b.shift<DIR_N>(), bb_7b.shift<DIR_S>()},
    {bb_6b.shift<DIR_N>(), bb_6b.shift<DIR_S>()},
    {bb_5b.shift<DIR_N>(), bb_5b.shift<DIR_S>()},
    {bb_4b.shift<DIR_N>(), bb_4b.shift<DIR_S>()},
    {bb_3b.shift<DIR_N>(), bb_3b.shift<DIR_S>()},
    {bb_2b.shift<DIR_N>(), bb_2b.shift<DIR_S>()},
    {bb_1b.shift<DIR_N>(), bb_1b.shift<DIR_S>()},
    {bb_9c.shift<DIR_N>(), bb_9c.shift<DIR_S>()},
    {bb_8c.shift<DIR_N>(), bb_8c.shift<DIR_S>()},
    {bb_7c.shift<DIR_N>(), bb_7c.shift<DIR_S>()},
    {bb_6c.shift<DIR_N>(), bb_6c.shift<DIR_S>()},
    {bb_5c.shift<DIR_N>(), bb_5c.shift<DIR_S>()},
    {bb_4c.shift<DIR_N>(), bb_4c.shift<DIR_S>()},
    {bb_3c.shift<DIR_N>(), bb_3c.shift<DIR_S>()},
    {bb_2c.shift<DIR_N>(), bb_2c.shift<DIR_S>()},
    {bb_1c.shift<DIR_N>(), bb_1c.shift<DIR_S>()},
    {bb_9d.shift<DIR_N>(), bb_9d.shift<DIR_S>()},
    {bb_8d.shift<DIR_N>(), bb_8d.shift<DIR_S>()},
    {bb_7d.shift<DIR_N>(), bb_7d.shift<DIR_S>()},
    {bb_6d.shift<DIR_N>(), bb_6d.shift<DIR_S>()},
    {bb_5d.shift<DIR_N>(), bb_5d.shift<DIR_S>()},
    {bb_4d.shift<DIR_N>(), bb_4d.shift<DIR_S>()},
    {bb_3d.shift<DIR_N>(), bb_3d.shift<DIR_S>()},
    {bb_2d.shift<DIR_N>(), bb_2d.shift<DIR_S>()},
    {bb_1d.shift<DIR_N>(), bb_1d.shift<DIR_S>()},
    {bb_9e.shift<DIR_N>(), bb_9e.shift<DIR_S>()},
    {bb_8e.shift<DIR_N>(), bb_8e.shift<DIR_S>()},
    {bb_7e.shift<DIR_N>(), bb_7e.shift<DIR_S>()},
    {bb_6e.shift<DIR_N>(), bb_6e.shift<DIR_S>()},
    {bb_5e.shift<DIR_N>(), bb_5e.shift<DIR_S>()},
    {bb_4e.shift<DIR_N>(), bb_4e.shift<DIR_S>()},
    {bb_3e.shift<DIR_N>(), bb_3e.shift<DIR_S>()},
    {bb_2e.shift<DIR_N>(), bb_2e.shift<DIR_S>()},
    {bb_1e.shift<DIR_N>(), bb_1e.shift<DIR_S>()},
    {bb_9f.shift<DIR_N>(), bb_9f.shift<DIR_S>()},
    {bb_8f.shift<DIR_N>(), bb_8f.shift<DIR_S>()},
    {bb_7f.shift<DIR_N>(), bb_7f.shift<DIR_S>()},
    {bb_6f.shift<DIR_N>(), bb_6f.shift<DIR_S>()},
    {bb_5f.shift<DIR_N>(), bb_5f.shift<DIR_S>()},
    {bb_4f.shift<DIR_N>(), bb_4f.shift<DIR_S>()},
    {bb_3f.shift<DIR_N>(), bb_3f.shift<DIR_S>()},
    {bb_2f.shift<DIR_N>(), bb_2f.shift<DIR_S>()},
    {bb_1f.shift<DIR_N>(), bb_1f.shift<DIR_S>()},
    {bb_9g.shift<DIR_N>(), bb_9g.shift<DIR_S>()},
    {bb_8g.shift<DIR_N>(), bb_8g.shift<DIR_S>()},
    {bb_7g.shift<DIR_N>(), bb_7g.shift<DIR_S>()},
    {bb_6g.shift<DIR_N>(), bb_6g.shift<DIR_S>()},
    {bb_5g.shift<DIR_N>(), bb_5g.shift<DIR_S>()},
    {bb_4g.shift<DIR_N>(), bb_4g.shift<DIR_S>()},
    {bb_3g.shift<DIR_N>(), bb_3g.shift<DIR_S>()},
    {bb_2g.shift<DIR_N>(), bb_2g.shift<DIR_S>()},
    {bb_1g.shift<DIR_N>(), bb_1g.shift<DIR_S>()},
    {bb_9h.shift<DIR_N>(), bb_9h.shift<DIR_S>()},
    {bb_8h.shift<DIR_N>(), bb_8h.shift<DIR_S>()},
    {bb_7h.shift<DIR_N>(), bb_7h.shift<DIR_S>()},
    {bb_6h.shift<DIR_N>(), bb_6h.shift<DIR_S>()},
    {bb_5h.shift<DIR_N>(), bb_5h.shift<DIR_S>()},
    {bb_4h.shift<DIR_N>(), bb_4h.shift<DIR_S>()},
    {bb_3h.shift<DIR_N>(), bb_3h.shift<DIR_S>()},
    {bb_2h.shift<DIR_N>(), bb_2h.shift<DIR_S>()},
    {bb_1h.shift<DIR_N>(), bb_1h.shift<DIR_S>()},
    {bb_9i.shift<DIR_N>(), bb_9i.shift<DIR_S>()},
    {bb_8i.shift<DIR_N>(), bb_8i.shift<DIR_S>()},
    {bb_7i.shift<DIR_N>(), bb_7i.shift<DIR_S>()},
    {bb_6i.shift<DIR_N>(), bb_6i.shift<DIR_S>()},
    {bb_5i.shift<DIR_N>(), bb_5i.shift<DIR_S>()},
    {bb_4i.shift<DIR_N>(), bb_4i.shift<DIR_S>()},
    {bb_3i.shift<DIR_N>(), bb_3i.shift<DIR_S>()},
    {bb_2i.shift<DIR_N>(), bb_2i.shift<DIR_S>()},
    {bb_1i.shift<DIR_N>(), bb_1i.shift<DIR_S>()},
};

constexpr BitBoard attacks_by_ke[Squares::num_squares][num_colors] = {
    {bb_9a.shift<DIR_NNW>() | bb_9a.shift<DIR_NNE>(),
     bb_9a.shift<DIR_SSW>() | bb_9a.shift<DIR_SSE>()},
    {bb_8a.shift<DIR_NNW>() | bb_8a.shift<DIR_NNE>(),
     bb_8a.shift<DIR_SSW>() | bb_8a.shift<DIR_SSE>()},
    {bb_7a.shift<DIR_NNW>() | bb_7a.shift<DIR_NNE>(),
     bb_7a.shift<DIR_SSW>() | bb_7a.shift<DIR_SSE>()},
    {bb_6a.shift<DIR_NNW>() | bb_6a.shift<DIR_NNE>(),
     bb_6a.shift<DIR_SSW>() | bb_6a.shift<DIR_SSE>()},
    {bb_5a.shift<DIR_NNW>() | bb_5a.shift<DIR_NNE>(),
     bb_5a.shift<DIR_SSW>() | bb_5a.shift<DIR_SSE>()},
    {bb_4a.shift<DIR_NNW>() | bb_4a.shift<DIR_NNE>(),
     bb_4a.shift<DIR_SSW>() | bb_4a.shift<DIR_SSE>()},
    {bb_3a.shift<DIR_NNW>() | bb_3a.shift<DIR_NNE>(),
     bb_3a.shift<DIR_SSW>() | bb_3a.shift<DIR_SSE>()},
    {bb_2a.shift<DIR_NNW>() | bb_2a.shift<DIR_NNE>(),
     bb_2a.shift<DIR_SSW>() | bb_2a.shift<DIR_SSE>()},
    {bb_1a.shift<DIR_NNW>() | bb_1a.shift<DIR_NNE>(),
     bb_1a.shift<DIR_SSW>() | bb_1a.shift<DIR_SSE>()},
    {bb_9b.shift<DIR_NNW>() | bb_9b.shift<DIR_NNE>(),
     bb_9b.shift<DIR_SSW>() | bb_9b.shift<DIR_SSE>()},
    {bb_8b.shift<DIR_NNW>() | bb_8b.shift<DIR_NNE>(),
     bb_8b.shift<DIR_SSW>() | bb_8b.shift<DIR_SSE>()},
    {bb_7b.shift<DIR_NNW>() | bb_7b.shift<DIR_NNE>(),
     bb_7b.shift<DIR_SSW>() | bb_7b.shift<DIR_SSE>()},
    {bb_6b.shift<DIR_NNW>() | bb_6b.shift<DIR_NNE>(),
     bb_6b.shift<DIR_SSW>() | bb_6b.shift<DIR_SSE>()},
    {bb_5b.shift<DIR_NNW>() | bb_5b.shift<DIR_NNE>(),
     bb_5b.shift<DIR_SSW>() | bb_5b.shift<DIR_SSE>()},
    {bb_4b.shift<DIR_NNW>() | bb_4b.shift<DIR_NNE>(),
     bb_4b.shift<DIR_SSW>() | bb_4b.shift<DIR_SSE>()},
    {bb_3b.shift<DIR_NNW>() | bb_3b.shift<DIR_NNE>(),
     bb_3b.shift<DIR_SSW>() | bb_3b.shift<DIR_SSE>()},
    {bb_2b.shift<DIR_NNW>() | bb_2b.shift<DIR_NNE>(),
     bb_2b.shift<DIR_SSW>() | bb_2b.shift<DIR_SSE>()},
    {bb_1b.shift<DIR_NNW>() | bb_1b.shift<DIR_NNE>(),
     bb_1b.shift<DIR_SSW>() | bb_1b.shift<DIR_SSE>()},
    {bb_9c.shift<DIR_NNW>() | bb_9c.shift<DIR_NNE>(),
     bb_9c.shift<DIR_SSW>() | bb_9c.shift<DIR_SSE>()},
    {bb_8c.shift<DIR_NNW>() | bb_8c.shift<DIR_NNE>(),
     bb_8c.shift<DIR_SSW>() | bb_8c.shift<DIR_SSE>()},
    {bb_7c.shift<DIR_NNW>() | bb_7c.shift<DIR_NNE>(),
     bb_7c.shift<DIR_SSW>() | bb_7c.shift<DIR_SSE>()},
    {bb_6c.shift<DIR_NNW>() | bb_6c.shift<DIR_NNE>(),
     bb_6c.shift<DIR_SSW>() | bb_6c.shift<DIR_SSE>()},
    {bb_5c.shift<DIR_NNW>() | bb_5c.shift<DIR_NNE>(),
     bb_5c.shift<DIR_SSW>() | bb_5c.shift<DIR_SSE>()},
    {bb_4c.shift<DIR_NNW>() | bb_4c.shift<DIR_NNE>(),
     bb_4c.shift<DIR_SSW>() | bb_4c.shift<DIR_SSE>()},
    {bb_3c.shift<DIR_NNW>() | bb_3c.shift<DIR_NNE>(),
     bb_3c.shift<DIR_SSW>() | bb_3c.shift<DIR_SSE>()},
    {bb_2c.shift<DIR_NNW>() | bb_2c.shift<DIR_NNE>(),
     bb_2c.shift<DIR_SSW>() | bb_2c.shift<DIR_SSE>()},
    {bb_1c.shift<DIR_NNW>() | bb_1c.shift<DIR_NNE>(),
     bb_1c.shift<DIR_SSW>() | bb_1c.shift<DIR_SSE>()},
    {bb_9d.shift<DIR_NNW>() | bb_9d.shift<DIR_NNE>(),
     bb_9d.shift<DIR_SSW>() | bb_9d.shift<DIR_SSE>()},
    {bb_8d.shift<DIR_NNW>() | bb_8d.shift<DIR_NNE>(),
     bb_8d.shift<DIR_SSW>() | bb_8d.shift<DIR_SSE>()},
    {bb_7d.shift<DIR_NNW>() | bb_7d.shift<DIR_NNE>(),
     bb_7d.shift<DIR_SSW>() | bb_7d.shift<DIR_SSE>()},
    {bb_6d.shift<DIR_NNW>() | bb_6d.shift<DIR_NNE>(),
     bb_6d.shift<DIR_SSW>() | bb_6d.shift<DIR_SSE>()},
    {bb_5d.shift<DIR_NNW>() | bb_5d.shift<DIR_NNE>(),
     bb_5d.shift<DIR_SSW>() | bb_5d.shift<DIR_SSE>()},
    {bb_4d.shift<DIR_NNW>() | bb_4d.shift<DIR_NNE>(),
     bb_4d.shift<DIR_SSW>() | bb_4d.shift<DIR_SSE>()},
    {bb_3d.shift<DIR_NNW>() | bb_3d.shift<DIR_NNE>(),
     bb_3d.shift<DIR_SSW>() | bb_3d.shift<DIR_SSE>()},
    {bb_2d.shift<DIR_NNW>() | bb_2d.shift<DIR_NNE>(),
     bb_2d.shift<DIR_SSW>() | bb_2d.shift<DIR_SSE>()},
    {bb_1d.shift<DIR_NNW>() | bb_1d.shift<DIR_NNE>(),
     bb_1d.shift<DIR_SSW>() | bb_1d.shift<DIR_SSE>()},
    {bb_9e.shift<DIR_NNW>() | bb_9e.shift<DIR_NNE>(),
     bb_9e.shift<DIR_SSW>() | bb_9e.shift<DIR_SSE>()},
    {bb_8e.shift<DIR_NNW>() | bb_8e.shift<DIR_NNE>(),
     bb_8e.shift<DIR_SSW>() | bb_8e.shift<DIR_SSE>()},
    {bb_7e.shift<DIR_NNW>() | bb_7e.shift<DIR_NNE>(),
     bb_7e.shift<DIR_SSW>() | bb_7e.shift<DIR_SSE>()},
    {bb_6e.shift<DIR_NNW>() | bb_6e.shift<DIR_NNE>(),
     bb_6e.shift<DIR_SSW>() | bb_6e.shift<DIR_SSE>()},
    {bb_5e.shift<DIR_NNW>() | bb_5e.shift<DIR_NNE>(),
     bb_5e.shift<DIR_SSW>() | bb_5e.shift<DIR_SSE>()},
    {bb_4e.shift<DIR_NNW>() | bb_4e.shift<DIR_NNE>(),
     bb_4e.shift<DIR_SSW>() | bb_4e.shift<DIR_SSE>()},
    {bb_3e.shift<DIR_NNW>() | bb_3e.shift<DIR_NNE>(),
     bb_3e.shift<DIR_SSW>() | bb_3e.shift<DIR_SSE>()},
    {bb_2e.shift<DIR_NNW>() | bb_2e.shift<DIR_NNE>(),
     bb_2e.shift<DIR_SSW>() | bb_2e.shift<DIR_SSE>()},
    {bb_1e.shift<DIR_NNW>() | bb_1e.shift<DIR_NNE>(),
     bb_1e.shift<DIR_SSW>() | bb_1e.shift<DIR_SSE>()},
    {bb_9f.shift<DIR_NNW>() | bb_9f.shift<DIR_NNE>(),
     bb_9f.shift<DIR_SSW>() | bb_9f.shift<DIR_SSE>()},
    {bb_8f.shift<DIR_NNW>() | bb_8f.shift<DIR_NNE>(),
     bb_8f.shift<DIR_SSW>() | bb_8f.shift<DIR_SSE>()},
    {bb_7f.shift<DIR_NNW>() | bb_7f.shift<DIR_NNE>(),
     bb_7f.shift<DIR_SSW>() | bb_7f.shift<DIR_SSE>()},
    {bb_6f.shift<DIR_NNW>() | bb_6f.shift<DIR_NNE>(),
     bb_6f.shift<DIR_SSW>() | bb_6f.shift<DIR_SSE>()},
    {bb_5f.shift<DIR_NNW>() | bb_5f.shift<DIR_NNE>(),
     bb_5f.shift<DIR_SSW>() | bb_5f.shift<DIR_SSE>()},
    {bb_4f.shift<DIR_NNW>() | bb_4f.shift<DIR_NNE>(),
     bb_4f.shift<DIR_SSW>() | bb_4f.shift<DIR_SSE>()},
    {bb_3f.shift<DIR_NNW>() | bb_3f.shift<DIR_NNE>(),
     bb_3f.shift<DIR_SSW>() | bb_3f.shift<DIR_SSE>()},
    {bb_2f.shift<DIR_NNW>() | bb_2f.shift<DIR_NNE>(),
     bb_2f.shift<DIR_SSW>() | bb_2f.shift<DIR_SSE>()},
    {bb_1f.shift<DIR_NNW>() | bb_1f.shift<DIR_NNE>(),
     bb_1f.shift<DIR_SSW>() | bb_1f.shift<DIR_SSE>()},
    {bb_9g.shift<DIR_NNW>() | bb_9g.shift<DIR_NNE>(),
     bb_9g.shift<DIR_SSW>() | bb_9g.shift<DIR_SSE>()},
    {bb_8g.shift<DIR_NNW>() | bb_8g.shift<DIR_NNE>(),
     bb_8g.shift<DIR_SSW>() | bb_8g.shift<DIR_SSE>()},
    {bb_7g.shift<DIR_NNW>() | bb_7g.shift<DIR_NNE>(),
     bb_7g.shift<DIR_SSW>() | bb_7g.shift<DIR_SSE>()},
    {bb_6g.shift<DIR_NNW>() | bb_6g.shift<DIR_NNE>(),
     bb_6g.shift<DIR_SSW>() | bb_6g.shift<DIR_SSE>()},
    {bb_5g.shift<DIR_NNW>() | bb_5g.shift<DIR_NNE>(),
     bb_5g.shift<DIR_SSW>() | bb_5g.shift<DIR_SSE>()},
    {bb_4g.shift<DIR_NNW>() | bb_4g.shift<DIR_NNE>(),
     bb_4g.shift<DIR_SSW>() | bb_4g.shift<DIR_SSE>()},
    {bb_3g.shift<DIR_NNW>() | bb_3g.shift<DIR_NNE>(),
     bb_3g.shift<DIR_SSW>() | bb_3g.shift<DIR_SSE>()},
    {bb_2g.shift<DIR_NNW>() | bb_2g.shift<DIR_NNE>(),
     bb_2g.shift<DIR_SSW>() | bb_2g.shift<DIR_SSE>()},
    {bb_1g.shift<DIR_NNW>() | bb_1g.shift<DIR_NNE>(),
     bb_1g.shift<DIR_SSW>() | bb_1g.shift<DIR_SSE>()},
    {bb_9h.shift<DIR_NNW>() | bb_9h.shift<DIR_NNE>(),
     bb_9h.shift<DIR_SSW>() | bb_9h.shift<DIR_SSE>()},
    {bb_8h.shift<DIR_NNW>() | bb_8h.shift<DIR_NNE>(),
     bb_8h.shift<DIR_SSW>() | bb_8h.shift<DIR_SSE>()},
    {bb_7h.shift<DIR_NNW>() | bb_7h.shift<DIR_NNE>(),
     bb_7h.shift<DIR_SSW>() | bb_7h.shift<DIR_SSE>()},
    {bb_6h.shift<DIR_NNW>() | bb_6h.shift<DIR_NNE>(),
     bb_6h.shift<DIR_SSW>() | bb_6h.shift<DIR_SSE>()},
    {bb_5h.shift<DIR_NNW>() | bb_5h.shift<DIR_NNE>(),
     bb_5h.shift<DIR_SSW>() | bb_5h.shift<DIR_SSE>()},
    {bb_4h.shift<DIR_NNW>() | bb_4h.shift<DIR_NNE>(),
     bb_4h.shift<DIR_SSW>() | bb_4h.shift<DIR_SSE>()},
    {bb_3h.shift<DIR_NNW>() | bb_3h.shift<DIR_NNE>(),
     bb_3h.shift<DIR_SSW>() | bb_3h.shift<DIR_SSE>()},
    {bb_2h.shift<DIR_NNW>() | bb_2h.shift<DIR_NNE>(),
     bb_2h.shift<DIR_SSW>() | bb_2h.shift<DIR_SSE>()},
    {bb_1h.shift<DIR_NNW>() | bb_1h.shift<DIR_NNE>(),
     bb_1h.shift<DIR_SSW>() | bb_1h.shift<DIR_SSE>()},
    {bb_9i.shift<DIR_NNW>() | bb_9i.shift<DIR_NNE>(),
     bb_9i.shift<DIR_SSW>() | bb_9i.shift<DIR_SSE>()},
    {bb_8i.shift<DIR_NNW>() | bb_8i.shift<DIR_NNE>(),
     bb_8i.shift<DIR_SSW>() | bb_8i.shift<DIR_SSE>()},
    {bb_7i.shift<DIR_NNW>() | bb_7i.shift<DIR_NNE>(),
     bb_7i.shift<DIR_SSW>() | bb_7i.shift<DIR_SSE>()},
    {bb_6i.shift<DIR_NNW>() | bb_6i.shift<DIR_NNE>(),
     bb_6i.shift<DIR_SSW>() | bb_6i.shift<DIR_SSE>()},
    {bb_5i.shift<DIR_NNW>() | bb_5i.shift<DIR_NNE>(),
     bb_5i.shift<DIR_SSW>() | bb_5i.shift<DIR_SSE>()},
    {bb_4i.shift<DIR_NNW>() | bb_4i.shift<DIR_NNE>(),
     bb_4i.shift<DIR_SSW>() | bb_4i.shift<DIR_SSE>()},
    {bb_3i.shift<DIR_NNW>() | bb_3i.shift<DIR_NNE>(),
     bb_3i.shift<DIR_SSW>() | bb_3i.shift<DIR_SSE>()},
    {bb_2i.shift<DIR_NNW>() | bb_2i.shift<DIR_NNE>(),
     bb_2i.shift<DIR_SSW>() | bb_2i.shift<DIR_SSE>()},
    {bb_1i.shift<DIR_NNW>() | bb_1i.shift<DIR_NNE>(),
     bb_1i.shift<DIR_SSW>() | bb_1i.shift<DIR_SSE>()},
};

namespace internal
{

constexpr BitBoard attacks_by_bky_sq9i
    = bb_9a | bb_9b | bb_9c | bb_9d | bb_9e | bb_9f | bb_9g | bb_9h;
constexpr BitBoard attacks_by_bky_sq8i
    = bb_8a | bb_8b | bb_8c | bb_8d | bb_8e | bb_8f | bb_8g | bb_8h;
constexpr BitBoard attacks_by_bky_sq7i
    = bb_7a | bb_7b | bb_7c | bb_7d | bb_7e | bb_7f | bb_7g | bb_7h;
constexpr BitBoard attacks_by_bky_sq6i
    = bb_6a | bb_6b | bb_6c | bb_6d | bb_6e | bb_6f | bb_6g | bb_6h;
constexpr BitBoard attacks_by_bky_sq5i
    = bb_5a | bb_5b | bb_5c | bb_5d | bb_5e | bb_5f | bb_5g | bb_5h;
constexpr BitBoard attacks_by_bky_sq4i
    = bb_4a | bb_4b | bb_4c | bb_4d | bb_4e | bb_4f | bb_4g | bb_4h;
constexpr BitBoard attacks_by_bky_sq3i
    = bb_3a | bb_3b | bb_3c | bb_3d | bb_3e | bb_3f | bb_3g | bb_3h;
constexpr BitBoard attacks_by_bky_sq2i
    = bb_2a | bb_2b | bb_2c | bb_2d | bb_2e | bb_2f | bb_2g | bb_2h;
constexpr BitBoard attacks_by_bky_sq1i
    = bb_1a | bb_1b | bb_1c | bb_1d | bb_1e | bb_1f | bb_1g | bb_1h;
constexpr BitBoard attacks_by_bky_sq9h = attacks_by_bky_sq9i.shift<DIR_N>();
constexpr BitBoard attacks_by_bky_sq8h = attacks_by_bky_sq8i.shift<DIR_N>();
constexpr BitBoard attacks_by_bky_sq7h = attacks_by_bky_sq7i.shift<DIR_N>();
constexpr BitBoard attacks_by_bky_sq6h = attacks_by_bky_sq6i.shift<DIR_N>();
constexpr BitBoard attacks_by_bky_sq5h = attacks_by_bky_sq5i.shift<DIR_N>();
constexpr BitBoard attacks_by_bky_sq4h = attacks_by_bky_sq4i.shift<DIR_N>();
constexpr BitBoard attacks_by_bky_sq3h = attacks_by_bky_sq3i.shift<DIR_N>();
constexpr BitBoard attacks_by_bky_sq2h = attacks_by_bky_sq2i.shift<DIR_N>();
constexpr BitBoard attacks_by_bky_sq1h = attacks_by_bky_sq1i.shift<DIR_N>();
constexpr BitBoard attacks_by_bky_sq9g = attacks_by_bky_sq9h.shift<DIR_N>();
constexpr BitBoard attacks_by_bky_sq8g = attacks_by_bky_sq8h.shift<DIR_N>();
constexpr BitBoard attacks_by_bky_sq7g = attacks_by_bky_sq7h.shift<DIR_N>();
constexpr BitBoard attacks_by_bky_sq6g = attacks_by_bky_sq6h.shift<DIR_N>();
constexpr BitBoard attacks_by_bky_sq5g = attacks_by_bky_sq5h.shift<DIR_N>();
constexpr BitBoard attacks_by_bky_sq4g = attacks_by_bky_sq4h.shift<DIR_N>();
constexpr BitBoard attacks_by_bky_sq3g = attacks_by_bky_sq3h.shift<DIR_N>();
constexpr BitBoard attacks_by_bky_sq2g = attacks_by_bky_sq2h.shift<DIR_N>();
constexpr BitBoard attacks_by_bky_sq1g = attacks_by_bky_sq1h.shift<DIR_N>();
constexpr BitBoard attacks_by_bky_sq9f = attacks_by_bky_sq9g.shift<DIR_N>();
constexpr BitBoard attacks_by_bky_sq8f = attacks_by_bky_sq8g.shift<DIR_N>();
constexpr BitBoard attacks_by_bky_sq7f = attacks_by_bky_sq7g.shift<DIR_N>();
constexpr BitBoard attacks_by_bky_sq6f = attacks_by_bky_sq6g.shift<DIR_N>();
constexpr BitBoard attacks_by_bky_sq5f = attacks_by_bky_sq5g.shift<DIR_N>();
constexpr BitBoard attacks_by_bky_sq4f = attacks_by_bky_sq4g.shift<DIR_N>();
constexpr BitBoard attacks_by_bky_sq3f = attacks_by_bky_sq3g.shift<DIR_N>();
constexpr BitBoard attacks_by_bky_sq2f = attacks_by_bky_sq2g.shift<DIR_N>();
constexpr BitBoard attacks_by_bky_sq1f = attacks_by_bky_sq1g.shift<DIR_N>();
constexpr BitBoard attacks_by_bky_sq9e = attacks_by_bky_sq9f.shift<DIR_N>();
constexpr BitBoard attacks_by_bky_sq8e = attacks_by_bky_sq8f.shift<DIR_N>();
constexpr BitBoard attacks_by_bky_sq7e = attacks_by_bky_sq7f.shift<DIR_N>();
constexpr BitBoard attacks_by_bky_sq6e = attacks_by_bky_sq6f.shift<DIR_N>();
constexpr BitBoard attacks_by_bky_sq5e = attacks_by_bky_sq5f.shift<DIR_N>();
constexpr BitBoard attacks_by_bky_sq4e = attacks_by_bky_sq4f.shift<DIR_N>();
constexpr BitBoard attacks_by_bky_sq3e = attacks_by_bky_sq3f.shift<DIR_N>();
constexpr BitBoard attacks_by_bky_sq2e = attacks_by_bky_sq2f.shift<DIR_N>();
constexpr BitBoard attacks_by_bky_sq1e = attacks_by_bky_sq1f.shift<DIR_N>();
constexpr BitBoard attacks_by_bky_sq9d = attacks_by_bky_sq9e.shift<DIR_N>();
constexpr BitBoard attacks_by_bky_sq8d = attacks_by_bky_sq8e.shift<DIR_N>();
constexpr BitBoard attacks_by_bky_sq7d = attacks_by_bky_sq7e.shift<DIR_N>();
constexpr BitBoard attacks_by_bky_sq6d = attacks_by_bky_sq6e.shift<DIR_N>();
constexpr BitBoard attacks_by_bky_sq5d = attacks_by_bky_sq5e.shift<DIR_N>();
constexpr BitBoard attacks_by_bky_sq4d = attacks_by_bky_sq4e.shift<DIR_N>();
constexpr BitBoard attacks_by_bky_sq3d = attacks_by_bky_sq3e.shift<DIR_N>();
constexpr BitBoard attacks_by_bky_sq2d = attacks_by_bky_sq2e.shift<DIR_N>();
constexpr BitBoard attacks_by_bky_sq1d = attacks_by_bky_sq1e.shift<DIR_N>();
constexpr BitBoard attacks_by_bky_sq9c = attacks_by_bky_sq9d.shift<DIR_N>();
constexpr BitBoard attacks_by_bky_sq8c = attacks_by_bky_sq8d.shift<DIR_N>();
constexpr BitBoard attacks_by_bky_sq7c = attacks_by_bky_sq7d.shift<DIR_N>();
constexpr BitBoard attacks_by_bky_sq6c = attacks_by_bky_sq6d.shift<DIR_N>();
constexpr BitBoard attacks_by_bky_sq5c = attacks_by_bky_sq5d.shift<DIR_N>();
constexpr BitBoard attacks_by_bky_sq4c = attacks_by_bky_sq4d.shift<DIR_N>();
constexpr BitBoard attacks_by_bky_sq3c = attacks_by_bky_sq3d.shift<DIR_N>();
constexpr BitBoard attacks_by_bky_sq2c = attacks_by_bky_sq2d.shift<DIR_N>();
constexpr BitBoard attacks_by_bky_sq1c = attacks_by_bky_sq1d.shift<DIR_N>();
constexpr BitBoard attacks_by_bky_sq9b = attacks_by_bky_sq9c.shift<DIR_N>();
constexpr BitBoard attacks_by_bky_sq8b = attacks_by_bky_sq8c.shift<DIR_N>();
constexpr BitBoard attacks_by_bky_sq7b = attacks_by_bky_sq7c.shift<DIR_N>();
constexpr BitBoard attacks_by_bky_sq6b = attacks_by_bky_sq6c.shift<DIR_N>();
constexpr BitBoard attacks_by_bky_sq5b = attacks_by_bky_sq5c.shift<DIR_N>();
constexpr BitBoard attacks_by_bky_sq4b = attacks_by_bky_sq4c.shift<DIR_N>();
constexpr BitBoard attacks_by_bky_sq3b = attacks_by_bky_sq3c.shift<DIR_N>();
constexpr BitBoard attacks_by_bky_sq2b = attacks_by_bky_sq2c.shift<DIR_N>();
constexpr BitBoard attacks_by_bky_sq1b = attacks_by_bky_sq1c.shift<DIR_N>();
constexpr BitBoard attacks_by_bky_sq9a = attacks_by_bky_sq9b.shift<DIR_N>();
constexpr BitBoard attacks_by_bky_sq8a = attacks_by_bky_sq8b.shift<DIR_N>();
constexpr BitBoard attacks_by_bky_sq7a = attacks_by_bky_sq7b.shift<DIR_N>();
constexpr BitBoard attacks_by_bky_sq6a = attacks_by_bky_sq6b.shift<DIR_N>();
constexpr BitBoard attacks_by_bky_sq5a = attacks_by_bky_sq5b.shift<DIR_N>();
constexpr BitBoard attacks_by_bky_sq4a = attacks_by_bky_sq4b.shift<DIR_N>();
constexpr BitBoard attacks_by_bky_sq3a = attacks_by_bky_sq3b.shift<DIR_N>();
constexpr BitBoard attacks_by_bky_sq2a = attacks_by_bky_sq2b.shift<DIR_N>();
constexpr BitBoard attacks_by_bky_sq1a = attacks_by_bky_sq1b.shift<DIR_N>();

constexpr BitBoard attacks_by_wky_sq9a
    = bb_9b | bb_9c | bb_9d | bb_9e | bb_9f | bb_9g | bb_9h | bb_9i;
constexpr BitBoard attacks_by_wky_sq8a
    = bb_8b | bb_8c | bb_8d | bb_8e | bb_8f | bb_8g | bb_8h | bb_8i;
constexpr BitBoard attacks_by_wky_sq7a
    = bb_7b | bb_7c | bb_7d | bb_7e | bb_7f | bb_7g | bb_7h | bb_7i;
constexpr BitBoard attacks_by_wky_sq6a
    = bb_6b | bb_6c | bb_6d | bb_6e | bb_6f | bb_6g | bb_6h | bb_6i;
constexpr BitBoard attacks_by_wky_sq5a
    = bb_5b | bb_5c | bb_5d | bb_5e | bb_5f | bb_5g | bb_5h | bb_5i;
constexpr BitBoard attacks_by_wky_sq4a
    = bb_4b | bb_4c | bb_4d | bb_4e | bb_4f | bb_4g | bb_4h | bb_4i;
constexpr BitBoard attacks_by_wky_sq3a
    = bb_3b | bb_3c | bb_3d | bb_3e | bb_3f | bb_3g | bb_3h | bb_3i;
constexpr BitBoard attacks_by_wky_sq2a
    = bb_2b | bb_2c | bb_2d | bb_2e | bb_2f | bb_2g | bb_2h | bb_2i;
constexpr BitBoard attacks_by_wky_sq1a
    = bb_1b | bb_1c | bb_1d | bb_1e | bb_1f | bb_1g | bb_1h | bb_1i;
constexpr BitBoard attacks_by_wky_sq9b = attacks_by_wky_sq9a.shift<DIR_S>();
constexpr BitBoard attacks_by_wky_sq8b = attacks_by_wky_sq8a.shift<DIR_S>();
constexpr BitBoard attacks_by_wky_sq7b = attacks_by_wky_sq7a.shift<DIR_S>();
constexpr BitBoard attacks_by_wky_sq6b = attacks_by_wky_sq6a.shift<DIR_S>();
constexpr BitBoard attacks_by_wky_sq5b = attacks_by_wky_sq5a.shift<DIR_S>();
constexpr BitBoard attacks_by_wky_sq4b = attacks_by_wky_sq4a.shift<DIR_S>();
constexpr BitBoard attacks_by_wky_sq3b = attacks_by_wky_sq3a.shift<DIR_S>();
constexpr BitBoard attacks_by_wky_sq2b = attacks_by_wky_sq2a.shift<DIR_S>();
constexpr BitBoard attacks_by_wky_sq1b = attacks_by_wky_sq1a.shift<DIR_S>();
constexpr BitBoard attacks_by_wky_sq9c = attacks_by_wky_sq9b.shift<DIR_S>();
constexpr BitBoard attacks_by_wky_sq8c = attacks_by_wky_sq8b.shift<DIR_S>();
constexpr BitBoard attacks_by_wky_sq7c = attacks_by_wky_sq7b.shift<DIR_S>();
constexpr BitBoard attacks_by_wky_sq6c = attacks_by_wky_sq6b.shift<DIR_S>();
constexpr BitBoard attacks_by_wky_sq5c = attacks_by_wky_sq5b.shift<DIR_S>();
constexpr BitBoard attacks_by_wky_sq4c = attacks_by_wky_sq4b.shift<DIR_S>();
constexpr BitBoard attacks_by_wky_sq3c = attacks_by_wky_sq3b.shift<DIR_S>();
constexpr BitBoard attacks_by_wky_sq2c = attacks_by_wky_sq2b.shift<DIR_S>();
constexpr BitBoard attacks_by_wky_sq1c = attacks_by_wky_sq1b.shift<DIR_S>();
constexpr BitBoard attacks_by_wky_sq9d = attacks_by_wky_sq9c.shift<DIR_S>();
constexpr BitBoard attacks_by_wky_sq8d = attacks_by_wky_sq8c.shift<DIR_S>();
constexpr BitBoard attacks_by_wky_sq7d = attacks_by_wky_sq7c.shift<DIR_S>();
constexpr BitBoard attacks_by_wky_sq6d = attacks_by_wky_sq6c.shift<DIR_S>();
constexpr BitBoard attacks_by_wky_sq5d = attacks_by_wky_sq5c.shift<DIR_S>();
constexpr BitBoard attacks_by_wky_sq4d = attacks_by_wky_sq4c.shift<DIR_S>();
constexpr BitBoard attacks_by_wky_sq3d = attacks_by_wky_sq3c.shift<DIR_S>();
constexpr BitBoard attacks_by_wky_sq2d = attacks_by_wky_sq2c.shift<DIR_S>();
constexpr BitBoard attacks_by_wky_sq1d = attacks_by_wky_sq1c.shift<DIR_S>();
constexpr BitBoard attacks_by_wky_sq9e = attacks_by_wky_sq9d.shift<DIR_S>();
constexpr BitBoard attacks_by_wky_sq8e = attacks_by_wky_sq8d.shift<DIR_S>();
constexpr BitBoard attacks_by_wky_sq7e = attacks_by_wky_sq7d.shift<DIR_S>();
constexpr BitBoard attacks_by_wky_sq6e = attacks_by_wky_sq6d.shift<DIR_S>();
constexpr BitBoard attacks_by_wky_sq5e = attacks_by_wky_sq5d.shift<DIR_S>();
constexpr BitBoard attacks_by_wky_sq4e = attacks_by_wky_sq4d.shift<DIR_S>();
constexpr BitBoard attacks_by_wky_sq3e = attacks_by_wky_sq3d.shift<DIR_S>();
constexpr BitBoard attacks_by_wky_sq2e = attacks_by_wky_sq2d.shift<DIR_S>();
constexpr BitBoard attacks_by_wky_sq1e = attacks_by_wky_sq1d.shift<DIR_S>();
constexpr BitBoard attacks_by_wky_sq9f = attacks_by_wky_sq9e.shift<DIR_S>();
constexpr BitBoard attacks_by_wky_sq8f = attacks_by_wky_sq8e.shift<DIR_S>();
constexpr BitBoard attacks_by_wky_sq7f = attacks_by_wky_sq7e.shift<DIR_S>();
constexpr BitBoard attacks_by_wky_sq6f = attacks_by_wky_sq6e.shift<DIR_S>();
constexpr BitBoard attacks_by_wky_sq5f = attacks_by_wky_sq5e.shift<DIR_S>();
constexpr BitBoard attacks_by_wky_sq4f = attacks_by_wky_sq4e.shift<DIR_S>();
constexpr BitBoard attacks_by_wky_sq3f = attacks_by_wky_sq3e.shift<DIR_S>();
constexpr BitBoard attacks_by_wky_sq2f = attacks_by_wky_sq2e.shift<DIR_S>();
constexpr BitBoard attacks_by_wky_sq1f = attacks_by_wky_sq1e.shift<DIR_S>();
constexpr BitBoard attacks_by_wky_sq9g = attacks_by_wky_sq9f.shift<DIR_S>();
constexpr BitBoard attacks_by_wky_sq8g = attacks_by_wky_sq8f.shift<DIR_S>();
constexpr BitBoard attacks_by_wky_sq7g = attacks_by_wky_sq7f.shift<DIR_S>();
constexpr BitBoard attacks_by_wky_sq6g = attacks_by_wky_sq6f.shift<DIR_S>();
constexpr BitBoard attacks_by_wky_sq5g = attacks_by_wky_sq5f.shift<DIR_S>();
constexpr BitBoard attacks_by_wky_sq4g = attacks_by_wky_sq4f.shift<DIR_S>();
constexpr BitBoard attacks_by_wky_sq3g = attacks_by_wky_sq3f.shift<DIR_S>();
constexpr BitBoard attacks_by_wky_sq2g = attacks_by_wky_sq2f.shift<DIR_S>();
constexpr BitBoard attacks_by_wky_sq1g = attacks_by_wky_sq1f.shift<DIR_S>();
constexpr BitBoard attacks_by_wky_sq9h = attacks_by_wky_sq9g.shift<DIR_S>();
constexpr BitBoard attacks_by_wky_sq8h = attacks_by_wky_sq8g.shift<DIR_S>();
constexpr BitBoard attacks_by_wky_sq7h = attacks_by_wky_sq7g.shift<DIR_S>();
constexpr BitBoard attacks_by_wky_sq6h = attacks_by_wky_sq6g.shift<DIR_S>();
constexpr BitBoard attacks_by_wky_sq5h = attacks_by_wky_sq5g.shift<DIR_S>();
constexpr BitBoard attacks_by_wky_sq4h = attacks_by_wky_sq4g.shift<DIR_S>();
constexpr BitBoard attacks_by_wky_sq3h = attacks_by_wky_sq3g.shift<DIR_S>();
constexpr BitBoard attacks_by_wky_sq2h = attacks_by_wky_sq2g.shift<DIR_S>();
constexpr BitBoard attacks_by_wky_sq1h = attacks_by_wky_sq1g.shift<DIR_S>();
constexpr BitBoard attacks_by_wky_sq9i = attacks_by_wky_sq9h.shift<DIR_S>();
constexpr BitBoard attacks_by_wky_sq8i = attacks_by_wky_sq8h.shift<DIR_S>();
constexpr BitBoard attacks_by_wky_sq7i = attacks_by_wky_sq7h.shift<DIR_S>();
constexpr BitBoard attacks_by_wky_sq6i = attacks_by_wky_sq6h.shift<DIR_S>();
constexpr BitBoard attacks_by_wky_sq5i = attacks_by_wky_sq5h.shift<DIR_S>();
constexpr BitBoard attacks_by_wky_sq4i = attacks_by_wky_sq4h.shift<DIR_S>();
constexpr BitBoard attacks_by_wky_sq3i = attacks_by_wky_sq3h.shift<DIR_S>();
constexpr BitBoard attacks_by_wky_sq2i = attacks_by_wky_sq2h.shift<DIR_S>();
constexpr BitBoard attacks_by_wky_sq1i = attacks_by_wky_sq1h.shift<DIR_S>();

constexpr BitBoard attacks_by_bgi_sq5e = bb_4d | bb_5d | bb_6d | bb_4f | bb_6f;
constexpr BitBoard attacks_by_bgi_sq5d = attacks_by_bgi_sq5e.shift<DIR_N>();
constexpr BitBoard attacks_by_bgi_sq5c = attacks_by_bgi_sq5d.shift<DIR_N>();
constexpr BitBoard attacks_by_bgi_sq5b = attacks_by_bgi_sq5c.shift<DIR_N>();
constexpr BitBoard attacks_by_bgi_sq5a = attacks_by_bgi_sq5b.shift<DIR_N>();
constexpr BitBoard attacks_by_bgi_sq5f = attacks_by_bgi_sq5e.shift<DIR_S>();
constexpr BitBoard attacks_by_bgi_sq5g = attacks_by_bgi_sq5f.shift<DIR_S>();
constexpr BitBoard attacks_by_bgi_sq5h = attacks_by_bgi_sq5g.shift<DIR_S>();
constexpr BitBoard attacks_by_bgi_sq5i = attacks_by_bgi_sq5h.shift<DIR_S>();
constexpr BitBoard attacks_by_bgi_sq4a = attacks_by_bgi_sq5a.shift<DIR_E>();
constexpr BitBoard attacks_by_bgi_sq4b = attacks_by_bgi_sq5b.shift<DIR_E>();
constexpr BitBoard attacks_by_bgi_sq4c = attacks_by_bgi_sq5c.shift<DIR_E>();
constexpr BitBoard attacks_by_bgi_sq4d = attacks_by_bgi_sq5d.shift<DIR_E>();
constexpr BitBoard attacks_by_bgi_sq4e = attacks_by_bgi_sq5e.shift<DIR_E>();
constexpr BitBoard attacks_by_bgi_sq4f = attacks_by_bgi_sq5f.shift<DIR_E>();
constexpr BitBoard attacks_by_bgi_sq4g = attacks_by_bgi_sq5g.shift<DIR_E>();
constexpr BitBoard attacks_by_bgi_sq4h = attacks_by_bgi_sq5h.shift<DIR_E>();
constexpr BitBoard attacks_by_bgi_sq4i = attacks_by_bgi_sq5i.shift<DIR_E>();
constexpr BitBoard attacks_by_bgi_sq3a = attacks_by_bgi_sq4a.shift<DIR_E>();
constexpr BitBoard attacks_by_bgi_sq3b = attacks_by_bgi_sq4b.shift<DIR_E>();
constexpr BitBoard attacks_by_bgi_sq3c = attacks_by_bgi_sq4c.shift<DIR_E>();
constexpr BitBoard attacks_by_bgi_sq3d = attacks_by_bgi_sq4d.shift<DIR_E>();
constexpr BitBoard attacks_by_bgi_sq3e = attacks_by_bgi_sq4e.shift<DIR_E>();
constexpr BitBoard attacks_by_bgi_sq3f = attacks_by_bgi_sq4f.shift<DIR_E>();
constexpr BitBoard attacks_by_bgi_sq3g = attacks_by_bgi_sq4g.shift<DIR_E>();
constexpr BitBoard attacks_by_bgi_sq3h = attacks_by_bgi_sq4h.shift<DIR_E>();
constexpr BitBoard attacks_by_bgi_sq3i = attacks_by_bgi_sq4i.shift<DIR_E>();
constexpr BitBoard attacks_by_bgi_sq2a = attacks_by_bgi_sq3a.shift<DIR_E>();
constexpr BitBoard attacks_by_bgi_sq2b = attacks_by_bgi_sq3b.shift<DIR_E>();
constexpr BitBoard attacks_by_bgi_sq2c = attacks_by_bgi_sq3c.shift<DIR_E>();
constexpr BitBoard attacks_by_bgi_sq2d = attacks_by_bgi_sq3d.shift<DIR_E>();
constexpr BitBoard attacks_by_bgi_sq2e = attacks_by_bgi_sq3e.shift<DIR_E>();
constexpr BitBoard attacks_by_bgi_sq2f = attacks_by_bgi_sq3f.shift<DIR_E>();
constexpr BitBoard attacks_by_bgi_sq2g = attacks_by_bgi_sq3g.shift<DIR_E>();
constexpr BitBoard attacks_by_bgi_sq2h = attacks_by_bgi_sq3h.shift<DIR_E>();
constexpr BitBoard attacks_by_bgi_sq2i = attacks_by_bgi_sq3i.shift<DIR_E>();
constexpr BitBoard attacks_by_bgi_sq1a = attacks_by_bgi_sq2a.shift<DIR_E>();
constexpr BitBoard attacks_by_bgi_sq1b = attacks_by_bgi_sq2b.shift<DIR_E>();
constexpr BitBoard attacks_by_bgi_sq1c = attacks_by_bgi_sq2c.shift<DIR_E>();
constexpr BitBoard attacks_by_bgi_sq1d = attacks_by_bgi_sq2d.shift<DIR_E>();
constexpr BitBoard attacks_by_bgi_sq1e = attacks_by_bgi_sq2e.shift<DIR_E>();
constexpr BitBoard attacks_by_bgi_sq1f = attacks_by_bgi_sq2f.shift<DIR_E>();
constexpr BitBoard attacks_by_bgi_sq1g = attacks_by_bgi_sq2g.shift<DIR_E>();
constexpr BitBoard attacks_by_bgi_sq1h = attacks_by_bgi_sq2h.shift<DIR_E>();
constexpr BitBoard attacks_by_bgi_sq1i = attacks_by_bgi_sq2i.shift<DIR_E>();
constexpr BitBoard attacks_by_bgi_sq6a = attacks_by_bgi_sq5a.shift<DIR_W>();
constexpr BitBoard attacks_by_bgi_sq6b = attacks_by_bgi_sq5b.shift<DIR_W>();
constexpr BitBoard attacks_by_bgi_sq6c = attacks_by_bgi_sq5c.shift<DIR_W>();
constexpr BitBoard attacks_by_bgi_sq6d = attacks_by_bgi_sq5d.shift<DIR_W>();
constexpr BitBoard attacks_by_bgi_sq6e = attacks_by_bgi_sq5e.shift<DIR_W>();
constexpr BitBoard attacks_by_bgi_sq6f = attacks_by_bgi_sq5f.shift<DIR_W>();
constexpr BitBoard attacks_by_bgi_sq6g = attacks_by_bgi_sq5g.shift<DIR_W>();
constexpr BitBoard attacks_by_bgi_sq6h = attacks_by_bgi_sq5h.shift<DIR_W>();
constexpr BitBoard attacks_by_bgi_sq6i = attacks_by_bgi_sq5i.shift<DIR_W>();
constexpr BitBoard attacks_by_bgi_sq7a = attacks_by_bgi_sq6a.shift<DIR_W>();
constexpr BitBoard attacks_by_bgi_sq7b = attacks_by_bgi_sq6b.shift<DIR_W>();
constexpr BitBoard attacks_by_bgi_sq7c = attacks_by_bgi_sq6c.shift<DIR_W>();
constexpr BitBoard attacks_by_bgi_sq7d = attacks_by_bgi_sq6d.shift<DIR_W>();
constexpr BitBoard attacks_by_bgi_sq7e = attacks_by_bgi_sq6e.shift<DIR_W>();
constexpr BitBoard attacks_by_bgi_sq7f = attacks_by_bgi_sq6f.shift<DIR_W>();
constexpr BitBoard attacks_by_bgi_sq7g = attacks_by_bgi_sq6g.shift<DIR_W>();
constexpr BitBoard attacks_by_bgi_sq7h = attacks_by_bgi_sq6h.shift<DIR_W>();
constexpr BitBoard attacks_by_bgi_sq7i = attacks_by_bgi_sq6i.shift<DIR_W>();
constexpr BitBoard attacks_by_bgi_sq8a = attacks_by_bgi_sq7a.shift<DIR_W>();
constexpr BitBoard attacks_by_bgi_sq8b = attacks_by_bgi_sq7b.shift<DIR_W>();
constexpr BitBoard attacks_by_bgi_sq8c = attacks_by_bgi_sq7c.shift<DIR_W>();
constexpr BitBoard attacks_by_bgi_sq8d = attacks_by_bgi_sq7d.shift<DIR_W>();
constexpr BitBoard attacks_by_bgi_sq8e = attacks_by_bgi_sq7e.shift<DIR_W>();
constexpr BitBoard attacks_by_bgi_sq8f = attacks_by_bgi_sq7f.shift<DIR_W>();
constexpr BitBoard attacks_by_bgi_sq8g = attacks_by_bgi_sq7g.shift<DIR_W>();
constexpr BitBoard attacks_by_bgi_sq8h = attacks_by_bgi_sq7h.shift<DIR_W>();
constexpr BitBoard attacks_by_bgi_sq8i = attacks_by_bgi_sq7i.shift<DIR_W>();
constexpr BitBoard attacks_by_bgi_sq9a = attacks_by_bgi_sq8a.shift<DIR_W>();
constexpr BitBoard attacks_by_bgi_sq9b = attacks_by_bgi_sq8b.shift<DIR_W>();
constexpr BitBoard attacks_by_bgi_sq9c = attacks_by_bgi_sq8c.shift<DIR_W>();
constexpr BitBoard attacks_by_bgi_sq9d = attacks_by_bgi_sq8d.shift<DIR_W>();
constexpr BitBoard attacks_by_bgi_sq9e = attacks_by_bgi_sq8e.shift<DIR_W>();
constexpr BitBoard attacks_by_bgi_sq9f = attacks_by_bgi_sq8f.shift<DIR_W>();
constexpr BitBoard attacks_by_bgi_sq9g = attacks_by_bgi_sq8g.shift<DIR_W>();
constexpr BitBoard attacks_by_bgi_sq9h = attacks_by_bgi_sq8h.shift<DIR_W>();
constexpr BitBoard attacks_by_bgi_sq9i = attacks_by_bgi_sq8i.shift<DIR_W>();

constexpr BitBoard attacks_by_wgi_sq5e = bb_4d | bb_6d | bb_4f | bb_5f | bb_6f;
constexpr BitBoard attacks_by_wgi_sq5d = attacks_by_wgi_sq5e.shift<DIR_N>();
constexpr BitBoard attacks_by_wgi_sq5c = attacks_by_wgi_sq5d.shift<DIR_N>();
constexpr BitBoard attacks_by_wgi_sq5b = attacks_by_wgi_sq5c.shift<DIR_N>();
constexpr BitBoard attacks_by_wgi_sq5a = attacks_by_wgi_sq5b.shift<DIR_N>();
constexpr BitBoard attacks_by_wgi_sq5f = attacks_by_wgi_sq5e.shift<DIR_S>();
constexpr BitBoard attacks_by_wgi_sq5g = attacks_by_wgi_sq5f.shift<DIR_S>();
constexpr BitBoard attacks_by_wgi_sq5h = attacks_by_wgi_sq5g.shift<DIR_S>();
constexpr BitBoard attacks_by_wgi_sq5i = attacks_by_wgi_sq5h.shift<DIR_S>();
constexpr BitBoard attacks_by_wgi_sq4a = attacks_by_wgi_sq5a.shift<DIR_E>();
constexpr BitBoard attacks_by_wgi_sq4b = attacks_by_wgi_sq5b.shift<DIR_E>();
constexpr BitBoard attacks_by_wgi_sq4c = attacks_by_wgi_sq5c.shift<DIR_E>();
constexpr BitBoard attacks_by_wgi_sq4d = attacks_by_wgi_sq5d.shift<DIR_E>();
constexpr BitBoard attacks_by_wgi_sq4e = attacks_by_wgi_sq5e.shift<DIR_E>();
constexpr BitBoard attacks_by_wgi_sq4f = attacks_by_wgi_sq5f.shift<DIR_E>();
constexpr BitBoard attacks_by_wgi_sq4g = attacks_by_wgi_sq5g.shift<DIR_E>();
constexpr BitBoard attacks_by_wgi_sq4h = attacks_by_wgi_sq5h.shift<DIR_E>();
constexpr BitBoard attacks_by_wgi_sq4i = attacks_by_wgi_sq5i.shift<DIR_E>();
constexpr BitBoard attacks_by_wgi_sq3a = attacks_by_wgi_sq4a.shift<DIR_E>();
constexpr BitBoard attacks_by_wgi_sq3b = attacks_by_wgi_sq4b.shift<DIR_E>();
constexpr BitBoard attacks_by_wgi_sq3c = attacks_by_wgi_sq4c.shift<DIR_E>();
constexpr BitBoard attacks_by_wgi_sq3d = attacks_by_wgi_sq4d.shift<DIR_E>();
constexpr BitBoard attacks_by_wgi_sq3e = attacks_by_wgi_sq4e.shift<DIR_E>();
constexpr BitBoard attacks_by_wgi_sq3f = attacks_by_wgi_sq4f.shift<DIR_E>();
constexpr BitBoard attacks_by_wgi_sq3g = attacks_by_wgi_sq4g.shift<DIR_E>();
constexpr BitBoard attacks_by_wgi_sq3h = attacks_by_wgi_sq4h.shift<DIR_E>();
constexpr BitBoard attacks_by_wgi_sq3i = attacks_by_wgi_sq4i.shift<DIR_E>();
constexpr BitBoard attacks_by_wgi_sq2a = attacks_by_wgi_sq3a.shift<DIR_E>();
constexpr BitBoard attacks_by_wgi_sq2b = attacks_by_wgi_sq3b.shift<DIR_E>();
constexpr BitBoard attacks_by_wgi_sq2c = attacks_by_wgi_sq3c.shift<DIR_E>();
constexpr BitBoard attacks_by_wgi_sq2d = attacks_by_wgi_sq3d.shift<DIR_E>();
constexpr BitBoard attacks_by_wgi_sq2e = attacks_by_wgi_sq3e.shift<DIR_E>();
constexpr BitBoard attacks_by_wgi_sq2f = attacks_by_wgi_sq3f.shift<DIR_E>();
constexpr BitBoard attacks_by_wgi_sq2g = attacks_by_wgi_sq3g.shift<DIR_E>();
constexpr BitBoard attacks_by_wgi_sq2h = attacks_by_wgi_sq3h.shift<DIR_E>();
constexpr BitBoard attacks_by_wgi_sq2i = attacks_by_wgi_sq3i.shift<DIR_E>();
constexpr BitBoard attacks_by_wgi_sq1a = attacks_by_wgi_sq2a.shift<DIR_E>();
constexpr BitBoard attacks_by_wgi_sq1b = attacks_by_wgi_sq2b.shift<DIR_E>();
constexpr BitBoard attacks_by_wgi_sq1c = attacks_by_wgi_sq2c.shift<DIR_E>();
constexpr BitBoard attacks_by_wgi_sq1d = attacks_by_wgi_sq2d.shift<DIR_E>();
constexpr BitBoard attacks_by_wgi_sq1e = attacks_by_wgi_sq2e.shift<DIR_E>();
constexpr BitBoard attacks_by_wgi_sq1f = attacks_by_wgi_sq2f.shift<DIR_E>();
constexpr BitBoard attacks_by_wgi_sq1g = attacks_by_wgi_sq2g.shift<DIR_E>();
constexpr BitBoard attacks_by_wgi_sq1h = attacks_by_wgi_sq2h.shift<DIR_E>();
constexpr BitBoard attacks_by_wgi_sq1i = attacks_by_wgi_sq2i.shift<DIR_E>();
constexpr BitBoard attacks_by_wgi_sq6a = attacks_by_wgi_sq5a.shift<DIR_W>();
constexpr BitBoard attacks_by_wgi_sq6b = attacks_by_wgi_sq5b.shift<DIR_W>();
constexpr BitBoard attacks_by_wgi_sq6c = attacks_by_wgi_sq5c.shift<DIR_W>();
constexpr BitBoard attacks_by_wgi_sq6d = attacks_by_wgi_sq5d.shift<DIR_W>();
constexpr BitBoard attacks_by_wgi_sq6e = attacks_by_wgi_sq5e.shift<DIR_W>();
constexpr BitBoard attacks_by_wgi_sq6f = attacks_by_wgi_sq5f.shift<DIR_W>();
constexpr BitBoard attacks_by_wgi_sq6g = attacks_by_wgi_sq5g.shift<DIR_W>();
constexpr BitBoard attacks_by_wgi_sq6h = attacks_by_wgi_sq5h.shift<DIR_W>();
constexpr BitBoard attacks_by_wgi_sq6i = attacks_by_wgi_sq5i.shift<DIR_W>();
constexpr BitBoard attacks_by_wgi_sq7a = attacks_by_wgi_sq6a.shift<DIR_W>();
constexpr BitBoard attacks_by_wgi_sq7b = attacks_by_wgi_sq6b.shift<DIR_W>();
constexpr BitBoard attacks_by_wgi_sq7c = attacks_by_wgi_sq6c.shift<DIR_W>();
constexpr BitBoard attacks_by_wgi_sq7d = attacks_by_wgi_sq6d.shift<DIR_W>();
constexpr BitBoard attacks_by_wgi_sq7e = attacks_by_wgi_sq6e.shift<DIR_W>();
constexpr BitBoard attacks_by_wgi_sq7f = attacks_by_wgi_sq6f.shift<DIR_W>();
constexpr BitBoard attacks_by_wgi_sq7g = attacks_by_wgi_sq6g.shift<DIR_W>();
constexpr BitBoard attacks_by_wgi_sq7h = attacks_by_wgi_sq6h.shift<DIR_W>();
constexpr BitBoard attacks_by_wgi_sq7i = attacks_by_wgi_sq6i.shift<DIR_W>();
constexpr BitBoard attacks_by_wgi_sq8a = attacks_by_wgi_sq7a.shift<DIR_W>();
constexpr BitBoard attacks_by_wgi_sq8b = attacks_by_wgi_sq7b.shift<DIR_W>();
constexpr BitBoard attacks_by_wgi_sq8c = attacks_by_wgi_sq7c.shift<DIR_W>();
constexpr BitBoard attacks_by_wgi_sq8d = attacks_by_wgi_sq7d.shift<DIR_W>();
constexpr BitBoard attacks_by_wgi_sq8e = attacks_by_wgi_sq7e.shift<DIR_W>();
constexpr BitBoard attacks_by_wgi_sq8f = attacks_by_wgi_sq7f.shift<DIR_W>();
constexpr BitBoard attacks_by_wgi_sq8g = attacks_by_wgi_sq7g.shift<DIR_W>();
constexpr BitBoard attacks_by_wgi_sq8h = attacks_by_wgi_sq7h.shift<DIR_W>();
constexpr BitBoard attacks_by_wgi_sq8i = attacks_by_wgi_sq7i.shift<DIR_W>();
constexpr BitBoard attacks_by_wgi_sq9a = attacks_by_wgi_sq8a.shift<DIR_W>();
constexpr BitBoard attacks_by_wgi_sq9b = attacks_by_wgi_sq8b.shift<DIR_W>();
constexpr BitBoard attacks_by_wgi_sq9c = attacks_by_wgi_sq8c.shift<DIR_W>();
constexpr BitBoard attacks_by_wgi_sq9d = attacks_by_wgi_sq8d.shift<DIR_W>();
constexpr BitBoard attacks_by_wgi_sq9e = attacks_by_wgi_sq8e.shift<DIR_W>();
constexpr BitBoard attacks_by_wgi_sq9f = attacks_by_wgi_sq8f.shift<DIR_W>();
constexpr BitBoard attacks_by_wgi_sq9g = attacks_by_wgi_sq8g.shift<DIR_W>();
constexpr BitBoard attacks_by_wgi_sq9h = attacks_by_wgi_sq8h.shift<DIR_W>();
constexpr BitBoard attacks_by_wgi_sq9i = attacks_by_wgi_sq8i.shift<DIR_W>();

constexpr BitBoard attacks_by_bki_sq5e
    = bb_4d | bb_5d | bb_6d | bb_4e | bb_6e | bb_5f;
constexpr BitBoard attacks_by_bki_sq5d = attacks_by_bki_sq5e.shift<DIR_N>();
constexpr BitBoard attacks_by_bki_sq5c = attacks_by_bki_sq5d.shift<DIR_N>();
constexpr BitBoard attacks_by_bki_sq5b = attacks_by_bki_sq5c.shift<DIR_N>();
constexpr BitBoard attacks_by_bki_sq5a = attacks_by_bki_sq5b.shift<DIR_N>();
constexpr BitBoard attacks_by_bki_sq5f = attacks_by_bki_sq5e.shift<DIR_S>();
constexpr BitBoard attacks_by_bki_sq5g = attacks_by_bki_sq5f.shift<DIR_S>();
constexpr BitBoard attacks_by_bki_sq5h = attacks_by_bki_sq5g.shift<DIR_S>();
constexpr BitBoard attacks_by_bki_sq5i = attacks_by_bki_sq5h.shift<DIR_S>();
constexpr BitBoard attacks_by_bki_sq4a = attacks_by_bki_sq5a.shift<DIR_E>();
constexpr BitBoard attacks_by_bki_sq4b = attacks_by_bki_sq5b.shift<DIR_E>();
constexpr BitBoard attacks_by_bki_sq4c = attacks_by_bki_sq5c.shift<DIR_E>();
constexpr BitBoard attacks_by_bki_sq4d = attacks_by_bki_sq5d.shift<DIR_E>();
constexpr BitBoard attacks_by_bki_sq4e = attacks_by_bki_sq5e.shift<DIR_E>();
constexpr BitBoard attacks_by_bki_sq4f = attacks_by_bki_sq5f.shift<DIR_E>();
constexpr BitBoard attacks_by_bki_sq4g = attacks_by_bki_sq5g.shift<DIR_E>();
constexpr BitBoard attacks_by_bki_sq4h = attacks_by_bki_sq5h.shift<DIR_E>();
constexpr BitBoard attacks_by_bki_sq4i = attacks_by_bki_sq5i.shift<DIR_E>();
constexpr BitBoard attacks_by_bki_sq3a = attacks_by_bki_sq4a.shift<DIR_E>();
constexpr BitBoard attacks_by_bki_sq3b = attacks_by_bki_sq4b.shift<DIR_E>();
constexpr BitBoard attacks_by_bki_sq3c = attacks_by_bki_sq4c.shift<DIR_E>();
constexpr BitBoard attacks_by_bki_sq3d = attacks_by_bki_sq4d.shift<DIR_E>();
constexpr BitBoard attacks_by_bki_sq3e = attacks_by_bki_sq4e.shift<DIR_E>();
constexpr BitBoard attacks_by_bki_sq3f = attacks_by_bki_sq4f.shift<DIR_E>();
constexpr BitBoard attacks_by_bki_sq3g = attacks_by_bki_sq4g.shift<DIR_E>();
constexpr BitBoard attacks_by_bki_sq3h = attacks_by_bki_sq4h.shift<DIR_E>();
constexpr BitBoard attacks_by_bki_sq3i = attacks_by_bki_sq4i.shift<DIR_E>();
constexpr BitBoard attacks_by_bki_sq2a = attacks_by_bki_sq3a.shift<DIR_E>();
constexpr BitBoard attacks_by_bki_sq2b = attacks_by_bki_sq3b.shift<DIR_E>();
constexpr BitBoard attacks_by_bki_sq2c = attacks_by_bki_sq3c.shift<DIR_E>();
constexpr BitBoard attacks_by_bki_sq2d = attacks_by_bki_sq3d.shift<DIR_E>();
constexpr BitBoard attacks_by_bki_sq2e = attacks_by_bki_sq3e.shift<DIR_E>();
constexpr BitBoard attacks_by_bki_sq2f = attacks_by_bki_sq3f.shift<DIR_E>();
constexpr BitBoard attacks_by_bki_sq2g = attacks_by_bki_sq3g.shift<DIR_E>();
constexpr BitBoard attacks_by_bki_sq2h = attacks_by_bki_sq3h.shift<DIR_E>();
constexpr BitBoard attacks_by_bki_sq2i = attacks_by_bki_sq3i.shift<DIR_E>();
constexpr BitBoard attacks_by_bki_sq1a = attacks_by_bki_sq2a.shift<DIR_E>();
constexpr BitBoard attacks_by_bki_sq1b = attacks_by_bki_sq2b.shift<DIR_E>();
constexpr BitBoard attacks_by_bki_sq1c = attacks_by_bki_sq2c.shift<DIR_E>();
constexpr BitBoard attacks_by_bki_sq1d = attacks_by_bki_sq2d.shift<DIR_E>();
constexpr BitBoard attacks_by_bki_sq1e = attacks_by_bki_sq2e.shift<DIR_E>();
constexpr BitBoard attacks_by_bki_sq1f = attacks_by_bki_sq2f.shift<DIR_E>();
constexpr BitBoard attacks_by_bki_sq1g = attacks_by_bki_sq2g.shift<DIR_E>();
constexpr BitBoard attacks_by_bki_sq1h = attacks_by_bki_sq2h.shift<DIR_E>();
constexpr BitBoard attacks_by_bki_sq1i = attacks_by_bki_sq2i.shift<DIR_E>();
constexpr BitBoard attacks_by_bki_sq6a = attacks_by_bki_sq5a.shift<DIR_W>();
constexpr BitBoard attacks_by_bki_sq6b = attacks_by_bki_sq5b.shift<DIR_W>();
constexpr BitBoard attacks_by_bki_sq6c = attacks_by_bki_sq5c.shift<DIR_W>();
constexpr BitBoard attacks_by_bki_sq6d = attacks_by_bki_sq5d.shift<DIR_W>();
constexpr BitBoard attacks_by_bki_sq6e = attacks_by_bki_sq5e.shift<DIR_W>();
constexpr BitBoard attacks_by_bki_sq6f = attacks_by_bki_sq5f.shift<DIR_W>();
constexpr BitBoard attacks_by_bki_sq6g = attacks_by_bki_sq5g.shift<DIR_W>();
constexpr BitBoard attacks_by_bki_sq6h = attacks_by_bki_sq5h.shift<DIR_W>();
constexpr BitBoard attacks_by_bki_sq6i = attacks_by_bki_sq5i.shift<DIR_W>();
constexpr BitBoard attacks_by_bki_sq7a = attacks_by_bki_sq6a.shift<DIR_W>();
constexpr BitBoard attacks_by_bki_sq7b = attacks_by_bki_sq6b.shift<DIR_W>();
constexpr BitBoard attacks_by_bki_sq7c = attacks_by_bki_sq6c.shift<DIR_W>();
constexpr BitBoard attacks_by_bki_sq7d = attacks_by_bki_sq6d.shift<DIR_W>();
constexpr BitBoard attacks_by_bki_sq7e = attacks_by_bki_sq6e.shift<DIR_W>();
constexpr BitBoard attacks_by_bki_sq7f = attacks_by_bki_sq6f.shift<DIR_W>();
constexpr BitBoard attacks_by_bki_sq7g = attacks_by_bki_sq6g.shift<DIR_W>();
constexpr BitBoard attacks_by_bki_sq7h = attacks_by_bki_sq6h.shift<DIR_W>();
constexpr BitBoard attacks_by_bki_sq7i = attacks_by_bki_sq6i.shift<DIR_W>();
constexpr BitBoard attacks_by_bki_sq8a = attacks_by_bki_sq7a.shift<DIR_W>();
constexpr BitBoard attacks_by_bki_sq8b = attacks_by_bki_sq7b.shift<DIR_W>();
constexpr BitBoard attacks_by_bki_sq8c = attacks_by_bki_sq7c.shift<DIR_W>();
constexpr BitBoard attacks_by_bki_sq8d = attacks_by_bki_sq7d.shift<DIR_W>();
constexpr BitBoard attacks_by_bki_sq8e = attacks_by_bki_sq7e.shift<DIR_W>();
constexpr BitBoard attacks_by_bki_sq8f = attacks_by_bki_sq7f.shift<DIR_W>();
constexpr BitBoard attacks_by_bki_sq8g = attacks_by_bki_sq7g.shift<DIR_W>();
constexpr BitBoard attacks_by_bki_sq8h = attacks_by_bki_sq7h.shift<DIR_W>();
constexpr BitBoard attacks_by_bki_sq8i = attacks_by_bki_sq7i.shift<DIR_W>();
constexpr BitBoard attacks_by_bki_sq9a = attacks_by_bki_sq8a.shift<DIR_W>();
constexpr BitBoard attacks_by_bki_sq9b = attacks_by_bki_sq8b.shift<DIR_W>();
constexpr BitBoard attacks_by_bki_sq9c = attacks_by_bki_sq8c.shift<DIR_W>();
constexpr BitBoard attacks_by_bki_sq9d = attacks_by_bki_sq8d.shift<DIR_W>();
constexpr BitBoard attacks_by_bki_sq9e = attacks_by_bki_sq8e.shift<DIR_W>();
constexpr BitBoard attacks_by_bki_sq9f = attacks_by_bki_sq8f.shift<DIR_W>();
constexpr BitBoard attacks_by_bki_sq9g = attacks_by_bki_sq8g.shift<DIR_W>();
constexpr BitBoard attacks_by_bki_sq9h = attacks_by_bki_sq8h.shift<DIR_W>();
constexpr BitBoard attacks_by_bki_sq9i = attacks_by_bki_sq8i.shift<DIR_W>();

constexpr BitBoard attacks_by_wki_sq5e
    = bb_5d | bb_4e | bb_6e | bb_4f | bb_5f | bb_6f;
constexpr BitBoard attacks_by_wki_sq5d = attacks_by_wki_sq5e.shift<DIR_N>();
constexpr BitBoard attacks_by_wki_sq5c = attacks_by_wki_sq5d.shift<DIR_N>();
constexpr BitBoard attacks_by_wki_sq5b = attacks_by_wki_sq5c.shift<DIR_N>();
constexpr BitBoard attacks_by_wki_sq5a = attacks_by_wki_sq5b.shift<DIR_N>();
constexpr BitBoard attacks_by_wki_sq5f = attacks_by_wki_sq5e.shift<DIR_S>();
constexpr BitBoard attacks_by_wki_sq5g = attacks_by_wki_sq5f.shift<DIR_S>();
constexpr BitBoard attacks_by_wki_sq5h = attacks_by_wki_sq5g.shift<DIR_S>();
constexpr BitBoard attacks_by_wki_sq5i = attacks_by_wki_sq5h.shift<DIR_S>();
constexpr BitBoard attacks_by_wki_sq4a = attacks_by_wki_sq5a.shift<DIR_E>();
constexpr BitBoard attacks_by_wki_sq4b = attacks_by_wki_sq5b.shift<DIR_E>();
constexpr BitBoard attacks_by_wki_sq4c = attacks_by_wki_sq5c.shift<DIR_E>();
constexpr BitBoard attacks_by_wki_sq4d = attacks_by_wki_sq5d.shift<DIR_E>();
constexpr BitBoard attacks_by_wki_sq4e = attacks_by_wki_sq5e.shift<DIR_E>();
constexpr BitBoard attacks_by_wki_sq4f = attacks_by_wki_sq5f.shift<DIR_E>();
constexpr BitBoard attacks_by_wki_sq4g = attacks_by_wki_sq5g.shift<DIR_E>();
constexpr BitBoard attacks_by_wki_sq4h = attacks_by_wki_sq5h.shift<DIR_E>();
constexpr BitBoard attacks_by_wki_sq4i = attacks_by_wki_sq5i.shift<DIR_E>();
constexpr BitBoard attacks_by_wki_sq3a = attacks_by_wki_sq4a.shift<DIR_E>();
constexpr BitBoard attacks_by_wki_sq3b = attacks_by_wki_sq4b.shift<DIR_E>();
constexpr BitBoard attacks_by_wki_sq3c = attacks_by_wki_sq4c.shift<DIR_E>();
constexpr BitBoard attacks_by_wki_sq3d = attacks_by_wki_sq4d.shift<DIR_E>();
constexpr BitBoard attacks_by_wki_sq3e = attacks_by_wki_sq4e.shift<DIR_E>();
constexpr BitBoard attacks_by_wki_sq3f = attacks_by_wki_sq4f.shift<DIR_E>();
constexpr BitBoard attacks_by_wki_sq3g = attacks_by_wki_sq4g.shift<DIR_E>();
constexpr BitBoard attacks_by_wki_sq3h = attacks_by_wki_sq4h.shift<DIR_E>();
constexpr BitBoard attacks_by_wki_sq3i = attacks_by_wki_sq4i.shift<DIR_E>();
constexpr BitBoard attacks_by_wki_sq2a = attacks_by_wki_sq3a.shift<DIR_E>();
constexpr BitBoard attacks_by_wki_sq2b = attacks_by_wki_sq3b.shift<DIR_E>();
constexpr BitBoard attacks_by_wki_sq2c = attacks_by_wki_sq3c.shift<DIR_E>();
constexpr BitBoard attacks_by_wki_sq2d = attacks_by_wki_sq3d.shift<DIR_E>();
constexpr BitBoard attacks_by_wki_sq2e = attacks_by_wki_sq3e.shift<DIR_E>();
constexpr BitBoard attacks_by_wki_sq2f = attacks_by_wki_sq3f.shift<DIR_E>();
constexpr BitBoard attacks_by_wki_sq2g = attacks_by_wki_sq3g.shift<DIR_E>();
constexpr BitBoard attacks_by_wki_sq2h = attacks_by_wki_sq3h.shift<DIR_E>();
constexpr BitBoard attacks_by_wki_sq2i = attacks_by_wki_sq3i.shift<DIR_E>();
constexpr BitBoard attacks_by_wki_sq1a = attacks_by_wki_sq2a.shift<DIR_E>();
constexpr BitBoard attacks_by_wki_sq1b = attacks_by_wki_sq2b.shift<DIR_E>();
constexpr BitBoard attacks_by_wki_sq1c = attacks_by_wki_sq2c.shift<DIR_E>();
constexpr BitBoard attacks_by_wki_sq1d = attacks_by_wki_sq2d.shift<DIR_E>();
constexpr BitBoard attacks_by_wki_sq1e = attacks_by_wki_sq2e.shift<DIR_E>();
constexpr BitBoard attacks_by_wki_sq1f = attacks_by_wki_sq2f.shift<DIR_E>();
constexpr BitBoard attacks_by_wki_sq1g = attacks_by_wki_sq2g.shift<DIR_E>();
constexpr BitBoard attacks_by_wki_sq1h = attacks_by_wki_sq2h.shift<DIR_E>();
constexpr BitBoard attacks_by_wki_sq1i = attacks_by_wki_sq2i.shift<DIR_E>();
constexpr BitBoard attacks_by_wki_sq6a = attacks_by_wki_sq5a.shift<DIR_W>();
constexpr BitBoard attacks_by_wki_sq6b = attacks_by_wki_sq5b.shift<DIR_W>();
constexpr BitBoard attacks_by_wki_sq6c = attacks_by_wki_sq5c.shift<DIR_W>();
constexpr BitBoard attacks_by_wki_sq6d = attacks_by_wki_sq5d.shift<DIR_W>();
constexpr BitBoard attacks_by_wki_sq6e = attacks_by_wki_sq5e.shift<DIR_W>();
constexpr BitBoard attacks_by_wki_sq6f = attacks_by_wki_sq5f.shift<DIR_W>();
constexpr BitBoard attacks_by_wki_sq6g = attacks_by_wki_sq5g.shift<DIR_W>();
constexpr BitBoard attacks_by_wki_sq6h = attacks_by_wki_sq5h.shift<DIR_W>();
constexpr BitBoard attacks_by_wki_sq6i = attacks_by_wki_sq5i.shift<DIR_W>();
constexpr BitBoard attacks_by_wki_sq7a = attacks_by_wki_sq6a.shift<DIR_W>();
constexpr BitBoard attacks_by_wki_sq7b = attacks_by_wki_sq6b.shift<DIR_W>();
constexpr BitBoard attacks_by_wki_sq7c = attacks_by_wki_sq6c.shift<DIR_W>();
constexpr BitBoard attacks_by_wki_sq7d = attacks_by_wki_sq6d.shift<DIR_W>();
constexpr BitBoard attacks_by_wki_sq7e = attacks_by_wki_sq6e.shift<DIR_W>();
constexpr BitBoard attacks_by_wki_sq7f = attacks_by_wki_sq6f.shift<DIR_W>();
constexpr BitBoard attacks_by_wki_sq7g = attacks_by_wki_sq6g.shift<DIR_W>();
constexpr BitBoard attacks_by_wki_sq7h = attacks_by_wki_sq6h.shift<DIR_W>();
constexpr BitBoard attacks_by_wki_sq7i = attacks_by_wki_sq6i.shift<DIR_W>();
constexpr BitBoard attacks_by_wki_sq8a = attacks_by_wki_sq7a.shift<DIR_W>();
constexpr BitBoard attacks_by_wki_sq8b = attacks_by_wki_sq7b.shift<DIR_W>();
constexpr BitBoard attacks_by_wki_sq8c = attacks_by_wki_sq7c.shift<DIR_W>();
constexpr BitBoard attacks_by_wki_sq8d = attacks_by_wki_sq7d.shift<DIR_W>();
constexpr BitBoard attacks_by_wki_sq8e = attacks_by_wki_sq7e.shift<DIR_W>();
constexpr BitBoard attacks_by_wki_sq8f = attacks_by_wki_sq7f.shift<DIR_W>();
constexpr BitBoard attacks_by_wki_sq8g = attacks_by_wki_sq7g.shift<DIR_W>();
constexpr BitBoard attacks_by_wki_sq8h = attacks_by_wki_sq7h.shift<DIR_W>();
constexpr BitBoard attacks_by_wki_sq8i = attacks_by_wki_sq7i.shift<DIR_W>();
constexpr BitBoard attacks_by_wki_sq9a = attacks_by_wki_sq8a.shift<DIR_W>();
constexpr BitBoard attacks_by_wki_sq9b = attacks_by_wki_sq8b.shift<DIR_W>();
constexpr BitBoard attacks_by_wki_sq9c = attacks_by_wki_sq8c.shift<DIR_W>();
constexpr BitBoard attacks_by_wki_sq9d = attacks_by_wki_sq8d.shift<DIR_W>();
constexpr BitBoard attacks_by_wki_sq9e = attacks_by_wki_sq8e.shift<DIR_W>();
constexpr BitBoard attacks_by_wki_sq9f = attacks_by_wki_sq8f.shift<DIR_W>();
constexpr BitBoard attacks_by_wki_sq9g = attacks_by_wki_sq8g.shift<DIR_W>();
constexpr BitBoard attacks_by_wki_sq9h = attacks_by_wki_sq8h.shift<DIR_W>();
constexpr BitBoard attacks_by_wki_sq9i = attacks_by_wki_sq8i.shift<DIR_W>();

constexpr BitBoard diagonal_9a_to_se
    = bb_9a | bb_8b | bb_7c | bb_6d | bb_5e | bb_4f | bb_3g | bb_2h | bb_1i;
constexpr BitBoard diagonal_8a_to_se = diagonal_9a_to_se.shift<DIR_N>();
constexpr BitBoard diagonal_7a_to_se = diagonal_8a_to_se.shift<DIR_N>();
constexpr BitBoard diagonal_6a_to_se = diagonal_7a_to_se.shift<DIR_N>();
constexpr BitBoard diagonal_5a_to_se = diagonal_6a_to_se.shift<DIR_N>();
constexpr BitBoard diagonal_4a_to_se = diagonal_5a_to_se.shift<DIR_N>();
constexpr BitBoard diagonal_3a_to_se = diagonal_4a_to_se.shift<DIR_N>();
constexpr BitBoard diagonal_2a_to_se = diagonal_3a_to_se.shift<DIR_N>();
constexpr BitBoard diagonal_1a_to_se = diagonal_2a_to_se.shift<DIR_N>();
constexpr BitBoard diagonal_9b_to_se = diagonal_9a_to_se.shift<DIR_S>();
constexpr BitBoard diagonal_9c_to_se = diagonal_9b_to_se.shift<DIR_S>();
constexpr BitBoard diagonal_9d_to_se = diagonal_9c_to_se.shift<DIR_S>();
constexpr BitBoard diagonal_9e_to_se = diagonal_9d_to_se.shift<DIR_S>();
constexpr BitBoard diagonal_9f_to_se = diagonal_9e_to_se.shift<DIR_S>();
constexpr BitBoard diagonal_9g_to_se = diagonal_9f_to_se.shift<DIR_S>();
constexpr BitBoard diagonal_9h_to_se = diagonal_9g_to_se.shift<DIR_S>();
constexpr BitBoard diagonal_9i_to_se = diagonal_9h_to_se.shift<DIR_S>();

constexpr BitBoard diagonal_1a_to_sw
    = bb_1a | bb_2b | bb_3c | bb_4d | bb_5e | bb_6f | bb_7g | bb_8h | bb_9i;
constexpr BitBoard diagonal_2a_to_sw = diagonal_1a_to_sw.shift<DIR_N>();
constexpr BitBoard diagonal_3a_to_sw = diagonal_2a_to_sw.shift<DIR_N>();
constexpr BitBoard diagonal_4a_to_sw = diagonal_3a_to_sw.shift<DIR_N>();
constexpr BitBoard diagonal_5a_to_sw = diagonal_4a_to_sw.shift<DIR_N>();
constexpr BitBoard diagonal_6a_to_sw = diagonal_5a_to_sw.shift<DIR_N>();
constexpr BitBoard diagonal_7a_to_sw = diagonal_6a_to_sw.shift<DIR_N>();
constexpr BitBoard diagonal_8a_to_sw = diagonal_7a_to_sw.shift<DIR_N>();
constexpr BitBoard diagonal_9a_to_sw = diagonal_8a_to_sw.shift<DIR_N>();
constexpr BitBoard diagonal_1b_to_sw = diagonal_1a_to_sw.shift<DIR_S>();
constexpr BitBoard diagonal_1c_to_sw = diagonal_1b_to_sw.shift<DIR_S>();
constexpr BitBoard diagonal_1d_to_sw = diagonal_1c_to_sw.shift<DIR_S>();
constexpr BitBoard diagonal_1e_to_sw = diagonal_1d_to_sw.shift<DIR_S>();
constexpr BitBoard diagonal_1f_to_sw = diagonal_1e_to_sw.shift<DIR_S>();
constexpr BitBoard diagonal_1g_to_sw = diagonal_1f_to_sw.shift<DIR_S>();
constexpr BitBoard diagonal_1h_to_sw = diagonal_1g_to_sw.shift<DIR_S>();
constexpr BitBoard diagonal_1i_to_sw = diagonal_1h_to_sw.shift<DIR_S>();

} // namespace internal

constexpr BitBoard attacks_by_ky[Squares::num_squares][num_colors] = {
    {internal::attacks_by_bky_sq9a, internal::attacks_by_wky_sq9a},
    {internal::attacks_by_bky_sq8a, internal::attacks_by_wky_sq8a},
    {internal::attacks_by_bky_sq7a, internal::attacks_by_wky_sq7a},
    {internal::attacks_by_bky_sq6a, internal::attacks_by_wky_sq6a},
    {internal::attacks_by_bky_sq5a, internal::attacks_by_wky_sq5a},
    {internal::attacks_by_bky_sq4a, internal::attacks_by_wky_sq4a},
    {internal::attacks_by_bky_sq3a, internal::attacks_by_wky_sq3a},
    {internal::attacks_by_bky_sq2a, internal::attacks_by_wky_sq2a},
    {internal::attacks_by_bky_sq1a, internal::attacks_by_wky_sq1a},
    {internal::attacks_by_bky_sq9b, internal::attacks_by_wky_sq9b},
    {internal::attacks_by_bky_sq8b, internal::attacks_by_wky_sq8b},
    {internal::attacks_by_bky_sq7b, internal::attacks_by_wky_sq7b},
    {internal::attacks_by_bky_sq6b, internal::attacks_by_wky_sq6b},
    {internal::attacks_by_bky_sq5b, internal::attacks_by_wky_sq5b},
    {internal::attacks_by_bky_sq4b, internal::attacks_by_wky_sq4b},
    {internal::attacks_by_bky_sq3b, internal::attacks_by_wky_sq3b},
    {internal::attacks_by_bky_sq2b, internal::attacks_by_wky_sq2b},
    {internal::attacks_by_bky_sq1b, internal::attacks_by_wky_sq1b},
    {internal::attacks_by_bky_sq9c, internal::attacks_by_wky_sq9c},
    {internal::attacks_by_bky_sq8c, internal::attacks_by_wky_sq8c},
    {internal::attacks_by_bky_sq7c, internal::attacks_by_wky_sq7c},
    {internal::attacks_by_bky_sq6c, internal::attacks_by_wky_sq6c},
    {internal::attacks_by_bky_sq5c, internal::attacks_by_wky_sq5c},
    {internal::attacks_by_bky_sq4c, internal::attacks_by_wky_sq4c},
    {internal::attacks_by_bky_sq3c, internal::attacks_by_wky_sq3c},
    {internal::attacks_by_bky_sq2c, internal::attacks_by_wky_sq2c},
    {internal::attacks_by_bky_sq1c, internal::attacks_by_wky_sq1c},
    {internal::attacks_by_bky_sq9d, internal::attacks_by_wky_sq9d},
    {internal::attacks_by_bky_sq8d, internal::attacks_by_wky_sq8d},
    {internal::attacks_by_bky_sq7d, internal::attacks_by_wky_sq7d},
    {internal::attacks_by_bky_sq6d, internal::attacks_by_wky_sq6d},
    {internal::attacks_by_bky_sq5d, internal::attacks_by_wky_sq5d},
    {internal::attacks_by_bky_sq4d, internal::attacks_by_wky_sq4d},
    {internal::attacks_by_bky_sq3d, internal::attacks_by_wky_sq3d},
    {internal::attacks_by_bky_sq2d, internal::attacks_by_wky_sq2d},
    {internal::attacks_by_bky_sq1d, internal::attacks_by_wky_sq1d},
    {internal::attacks_by_bky_sq9e, internal::attacks_by_wky_sq9e},
    {internal::attacks_by_bky_sq8e, internal::attacks_by_wky_sq8e},
    {internal::attacks_by_bky_sq7e, internal::attacks_by_wky_sq7e},
    {internal::attacks_by_bky_sq6e, internal::attacks_by_wky_sq6e},
    {internal::attacks_by_bky_sq5e, internal::attacks_by_wky_sq5e},
    {internal::attacks_by_bky_sq4e, internal::attacks_by_wky_sq4e},
    {internal::attacks_by_bky_sq3e, internal::attacks_by_wky_sq3e},
    {internal::attacks_by_bky_sq2e, internal::attacks_by_wky_sq2e},
    {internal::attacks_by_bky_sq1e, internal::attacks_by_wky_sq1e},
    {internal::attacks_by_bky_sq9f, internal::attacks_by_wky_sq9f},
    {internal::attacks_by_bky_sq8f, internal::attacks_by_wky_sq8f},
    {internal::attacks_by_bky_sq7f, internal::attacks_by_wky_sq7f},
    {internal::attacks_by_bky_sq6f, internal::attacks_by_wky_sq6f},
    {internal::attacks_by_bky_sq5f, internal::attacks_by_wky_sq5f},
    {internal::attacks_by_bky_sq4f, internal::attacks_by_wky_sq4f},
    {internal::attacks_by_bky_sq3f, internal::attacks_by_wky_sq3f},
    {internal::attacks_by_bky_sq2f, internal::attacks_by_wky_sq2f},
    {internal::attacks_by_bky_sq1f, internal::attacks_by_wky_sq1f},
    {internal::attacks_by_bky_sq9g, internal::attacks_by_wky_sq9g},
    {internal::attacks_by_bky_sq8g, internal::attacks_by_wky_sq8g},
    {internal::attacks_by_bky_sq7g, internal::attacks_by_wky_sq7g},
    {internal::attacks_by_bky_sq6g, internal::attacks_by_wky_sq6g},
    {internal::attacks_by_bky_sq5g, internal::attacks_by_wky_sq5g},
    {internal::attacks_by_bky_sq4g, internal::attacks_by_wky_sq4g},
    {internal::attacks_by_bky_sq3g, internal::attacks_by_wky_sq3g},
    {internal::attacks_by_bky_sq2g, internal::attacks_by_wky_sq2g},
    {internal::attacks_by_bky_sq1g, internal::attacks_by_wky_sq1g},
    {internal::attacks_by_bky_sq9h, internal::attacks_by_wky_sq9h},
    {internal::attacks_by_bky_sq8h, internal::attacks_by_wky_sq8h},
    {internal::attacks_by_bky_sq7h, internal::attacks_by_wky_sq7h},
    {internal::attacks_by_bky_sq6h, internal::attacks_by_wky_sq6h},
    {internal::attacks_by_bky_sq5h, internal::attacks_by_wky_sq5h},
    {internal::attacks_by_bky_sq4h, internal::attacks_by_wky_sq4h},
    {internal::attacks_by_bky_sq3h, internal::attacks_by_wky_sq3h},
    {internal::attacks_by_bky_sq2h, internal::attacks_by_wky_sq2h},
    {internal::attacks_by_bky_sq1h, internal::attacks_by_wky_sq1h},
    {internal::attacks_by_bky_sq9i, internal::attacks_by_wky_sq9i},
    {internal::attacks_by_bky_sq8i, internal::attacks_by_wky_sq8i},
    {internal::attacks_by_bky_sq7i, internal::attacks_by_wky_sq7i},
    {internal::attacks_by_bky_sq6i, internal::attacks_by_wky_sq6i},
    {internal::attacks_by_bky_sq5i, internal::attacks_by_wky_sq5i},
    {internal::attacks_by_bky_sq4i, internal::attacks_by_wky_sq4i},
    {internal::attacks_by_bky_sq3i, internal::attacks_by_wky_sq3i},
    {internal::attacks_by_bky_sq2i, internal::attacks_by_wky_sq2i},
    {internal::attacks_by_bky_sq1i, internal::attacks_by_wky_sq1i},
};

constexpr BitBoard attacks_by_gi[Squares::num_squares][num_colors] = {
    {internal::attacks_by_bgi_sq9a, internal::attacks_by_wgi_sq9a},
    {internal::attacks_by_bgi_sq8a, internal::attacks_by_wgi_sq8a},
    {internal::attacks_by_bgi_sq7a, internal::attacks_by_wgi_sq7a},
    {internal::attacks_by_bgi_sq6a, internal::attacks_by_wgi_sq6a},
    {internal::attacks_by_bgi_sq5a, internal::attacks_by_wgi_sq5a},
    {internal::attacks_by_bgi_sq4a, internal::attacks_by_wgi_sq4a},
    {internal::attacks_by_bgi_sq3a, internal::attacks_by_wgi_sq3a},
    {internal::attacks_by_bgi_sq2a, internal::attacks_by_wgi_sq2a},
    {internal::attacks_by_bgi_sq1a, internal::attacks_by_wgi_sq1a},
    {internal::attacks_by_bgi_sq9b, internal::attacks_by_wgi_sq9b},
    {internal::attacks_by_bgi_sq8b, internal::attacks_by_wgi_sq8b},
    {internal::attacks_by_bgi_sq7b, internal::attacks_by_wgi_sq7b},
    {internal::attacks_by_bgi_sq6b, internal::attacks_by_wgi_sq6b},
    {internal::attacks_by_bgi_sq5b, internal::attacks_by_wgi_sq5b},
    {internal::attacks_by_bgi_sq4b, internal::attacks_by_wgi_sq4b},
    {internal::attacks_by_bgi_sq3b, internal::attacks_by_wgi_sq3b},
    {internal::attacks_by_bgi_sq2b, internal::attacks_by_wgi_sq2b},
    {internal::attacks_by_bgi_sq1b, internal::attacks_by_wgi_sq1b},
    {internal::attacks_by_bgi_sq9c, internal::attacks_by_wgi_sq9c},
    {internal::attacks_by_bgi_sq8c, internal::attacks_by_wgi_sq8c},
    {internal::attacks_by_bgi_sq7c, internal::attacks_by_wgi_sq7c},
    {internal::attacks_by_bgi_sq6c, internal::attacks_by_wgi_sq6c},
    {internal::attacks_by_bgi_sq5c, internal::attacks_by_wgi_sq5c},
    {internal::attacks_by_bgi_sq4c, internal::attacks_by_wgi_sq4c},
    {internal::attacks_by_bgi_sq3c, internal::attacks_by_wgi_sq3c},
    {internal::attacks_by_bgi_sq2c, internal::attacks_by_wgi_sq2c},
    {internal::attacks_by_bgi_sq1c, internal::attacks_by_wgi_sq1c},
    {internal::attacks_by_bgi_sq9d, internal::attacks_by_wgi_sq9d},
    {internal::attacks_by_bgi_sq8d, internal::attacks_by_wgi_sq8d},
    {internal::attacks_by_bgi_sq7d, internal::attacks_by_wgi_sq7d},
    {internal::attacks_by_bgi_sq6d, internal::attacks_by_wgi_sq6d},
    {internal::attacks_by_bgi_sq5d, internal::attacks_by_wgi_sq5d},
    {internal::attacks_by_bgi_sq4d, internal::attacks_by_wgi_sq4d},
    {internal::attacks_by_bgi_sq3d, internal::attacks_by_wgi_sq3d},
    {internal::attacks_by_bgi_sq2d, internal::attacks_by_wgi_sq2d},
    {internal::attacks_by_bgi_sq1d, internal::attacks_by_wgi_sq1d},
    {internal::attacks_by_bgi_sq9e, internal::attacks_by_wgi_sq9e},
    {internal::attacks_by_bgi_sq8e, internal::attacks_by_wgi_sq8e},
    {internal::attacks_by_bgi_sq7e, internal::attacks_by_wgi_sq7e},
    {internal::attacks_by_bgi_sq6e, internal::attacks_by_wgi_sq6e},
    {internal::attacks_by_bgi_sq5e, internal::attacks_by_wgi_sq5e},
    {internal::attacks_by_bgi_sq4e, internal::attacks_by_wgi_sq4e},
    {internal::attacks_by_bgi_sq3e, internal::attacks_by_wgi_sq3e},
    {internal::attacks_by_bgi_sq2e, internal::attacks_by_wgi_sq2e},
    {internal::attacks_by_bgi_sq1e, internal::attacks_by_wgi_sq1e},
    {internal::attacks_by_bgi_sq9f, internal::attacks_by_wgi_sq9f},
    {internal::attacks_by_bgi_sq8f, internal::attacks_by_wgi_sq8f},
    {internal::attacks_by_bgi_sq7f, internal::attacks_by_wgi_sq7f},
    {internal::attacks_by_bgi_sq6f, internal::attacks_by_wgi_sq6f},
    {internal::attacks_by_bgi_sq5f, internal::attacks_by_wgi_sq5f},
    {internal::attacks_by_bgi_sq4f, internal::attacks_by_wgi_sq4f},
    {internal::attacks_by_bgi_sq3f, internal::attacks_by_wgi_sq3f},
    {internal::attacks_by_bgi_sq2f, internal::attacks_by_wgi_sq2f},
    {internal::attacks_by_bgi_sq1f, internal::attacks_by_wgi_sq1f},
    {internal::attacks_by_bgi_sq9g, internal::attacks_by_wgi_sq9g},
    {internal::attacks_by_bgi_sq8g, internal::attacks_by_wgi_sq8g},
    {internal::attacks_by_bgi_sq7g, internal::attacks_by_wgi_sq7g},
    {internal::attacks_by_bgi_sq6g, internal::attacks_by_wgi_sq6g},
    {internal::attacks_by_bgi_sq5g, internal::attacks_by_wgi_sq5g},
    {internal::attacks_by_bgi_sq4g, internal::attacks_by_wgi_sq4g},
    {internal::attacks_by_bgi_sq3g, internal::attacks_by_wgi_sq3g},
    {internal::attacks_by_bgi_sq2g, internal::attacks_by_wgi_sq2g},
    {internal::attacks_by_bgi_sq1g, internal::attacks_by_wgi_sq1g},
    {internal::attacks_by_bgi_sq9h, internal::attacks_by_wgi_sq9h},
    {internal::attacks_by_bgi_sq8h, internal::attacks_by_wgi_sq8h},
    {internal::attacks_by_bgi_sq7h, internal::attacks_by_wgi_sq7h},
    {internal::attacks_by_bgi_sq6h, internal::attacks_by_wgi_sq6h},
    {internal::attacks_by_bgi_sq5h, internal::attacks_by_wgi_sq5h},
    {internal::attacks_by_bgi_sq4h, internal::attacks_by_wgi_sq4h},
    {internal::attacks_by_bgi_sq3h, internal::attacks_by_wgi_sq3h},
    {internal::attacks_by_bgi_sq2h, internal::attacks_by_wgi_sq2h},
    {internal::attacks_by_bgi_sq1h, internal::attacks_by_wgi_sq1h},
    {internal::attacks_by_bgi_sq9i, internal::attacks_by_wgi_sq9i},
    {internal::attacks_by_bgi_sq8i, internal::attacks_by_wgi_sq8i},
    {internal::attacks_by_bgi_sq7i, internal::attacks_by_wgi_sq7i},
    {internal::attacks_by_bgi_sq6i, internal::attacks_by_wgi_sq6i},
    {internal::attacks_by_bgi_sq5i, internal::attacks_by_wgi_sq5i},
    {internal::attacks_by_bgi_sq4i, internal::attacks_by_wgi_sq4i},
    {internal::attacks_by_bgi_sq3i, internal::attacks_by_wgi_sq3i},
    {internal::attacks_by_bgi_sq2i, internal::attacks_by_wgi_sq2i},
    {internal::attacks_by_bgi_sq1i, internal::attacks_by_wgi_sq1i},
};

constexpr BitBoard attacks_by_ki[Squares::num_squares][num_colors] = {
    {internal::attacks_by_bki_sq9a, internal::attacks_by_wki_sq9a},
    {internal::attacks_by_bki_sq8a, internal::attacks_by_wki_sq8a},
    {internal::attacks_by_bki_sq7a, internal::attacks_by_wki_sq7a},
    {internal::attacks_by_bki_sq6a, internal::attacks_by_wki_sq6a},
    {internal::attacks_by_bki_sq5a, internal::attacks_by_wki_sq5a},
    {internal::attacks_by_bki_sq4a, internal::attacks_by_wki_sq4a},
    {internal::attacks_by_bki_sq3a, internal::attacks_by_wki_sq3a},
    {internal::attacks_by_bki_sq2a, internal::attacks_by_wki_sq2a},
    {internal::attacks_by_bki_sq1a, internal::attacks_by_wki_sq1a},
    {internal::attacks_by_bki_sq9b, internal::attacks_by_wki_sq9b},
    {internal::attacks_by_bki_sq8b, internal::attacks_by_wki_sq8b},
    {internal::attacks_by_bki_sq7b, internal::attacks_by_wki_sq7b},
    {internal::attacks_by_bki_sq6b, internal::attacks_by_wki_sq6b},
    {internal::attacks_by_bki_sq5b, internal::attacks_by_wki_sq5b},
    {internal::attacks_by_bki_sq4b, internal::attacks_by_wki_sq4b},
    {internal::attacks_by_bki_sq3b, internal::attacks_by_wki_sq3b},
    {internal::attacks_by_bki_sq2b, internal::attacks_by_wki_sq2b},
    {internal::attacks_by_bki_sq1b, internal::attacks_by_wki_sq1b},
    {internal::attacks_by_bki_sq9c, internal::attacks_by_wki_sq9c},
    {internal::attacks_by_bki_sq8c, internal::attacks_by_wki_sq8c},
    {internal::attacks_by_bki_sq7c, internal::attacks_by_wki_sq7c},
    {internal::attacks_by_bki_sq6c, internal::attacks_by_wki_sq6c},
    {internal::attacks_by_bki_sq5c, internal::attacks_by_wki_sq5c},
    {internal::attacks_by_bki_sq4c, internal::attacks_by_wki_sq4c},
    {internal::attacks_by_bki_sq3c, internal::attacks_by_wki_sq3c},
    {internal::attacks_by_bki_sq2c, internal::attacks_by_wki_sq2c},
    {internal::attacks_by_bki_sq1c, internal::attacks_by_wki_sq1c},
    {internal::attacks_by_bki_sq9d, internal::attacks_by_wki_sq9d},
    {internal::attacks_by_bki_sq8d, internal::attacks_by_wki_sq8d},
    {internal::attacks_by_bki_sq7d, internal::attacks_by_wki_sq7d},
    {internal::attacks_by_bki_sq6d, internal::attacks_by_wki_sq6d},
    {internal::attacks_by_bki_sq5d, internal::attacks_by_wki_sq5d},
    {internal::attacks_by_bki_sq4d, internal::attacks_by_wki_sq4d},
    {internal::attacks_by_bki_sq3d, internal::attacks_by_wki_sq3d},
    {internal::attacks_by_bki_sq2d, internal::attacks_by_wki_sq2d},
    {internal::attacks_by_bki_sq1d, internal::attacks_by_wki_sq1d},
    {internal::attacks_by_bki_sq9e, internal::attacks_by_wki_sq9e},
    {internal::attacks_by_bki_sq8e, internal::attacks_by_wki_sq8e},
    {internal::attacks_by_bki_sq7e, internal::attacks_by_wki_sq7e},
    {internal::attacks_by_bki_sq6e, internal::attacks_by_wki_sq6e},
    {internal::attacks_by_bki_sq5e, internal::attacks_by_wki_sq5e},
    {internal::attacks_by_bki_sq4e, internal::attacks_by_wki_sq4e},
    {internal::attacks_by_bki_sq3e, internal::attacks_by_wki_sq3e},
    {internal::attacks_by_bki_sq2e, internal::attacks_by_wki_sq2e},
    {internal::attacks_by_bki_sq1e, internal::attacks_by_wki_sq1e},
    {internal::attacks_by_bki_sq9f, internal::attacks_by_wki_sq9f},
    {internal::attacks_by_bki_sq8f, internal::attacks_by_wki_sq8f},
    {internal::attacks_by_bki_sq7f, internal::attacks_by_wki_sq7f},
    {internal::attacks_by_bki_sq6f, internal::attacks_by_wki_sq6f},
    {internal::attacks_by_bki_sq5f, internal::attacks_by_wki_sq5f},
    {internal::attacks_by_bki_sq4f, internal::attacks_by_wki_sq4f},
    {internal::attacks_by_bki_sq3f, internal::attacks_by_wki_sq3f},
    {internal::attacks_by_bki_sq2f, internal::attacks_by_wki_sq2f},
    {internal::attacks_by_bki_sq1f, internal::attacks_by_wki_sq1f},
    {internal::attacks_by_bki_sq9g, internal::attacks_by_wki_sq9g},
    {internal::attacks_by_bki_sq8g, internal::attacks_by_wki_sq8g},
    {internal::attacks_by_bki_sq7g, internal::attacks_by_wki_sq7g},
    {internal::attacks_by_bki_sq6g, internal::attacks_by_wki_sq6g},
    {internal::attacks_by_bki_sq5g, internal::attacks_by_wki_sq5g},
    {internal::attacks_by_bki_sq4g, internal::attacks_by_wki_sq4g},
    {internal::attacks_by_bki_sq3g, internal::attacks_by_wki_sq3g},
    {internal::attacks_by_bki_sq2g, internal::attacks_by_wki_sq2g},
    {internal::attacks_by_bki_sq1g, internal::attacks_by_wki_sq1g},
    {internal::attacks_by_bki_sq9h, internal::attacks_by_wki_sq9h},
    {internal::attacks_by_bki_sq8h, internal::attacks_by_wki_sq8h},
    {internal::attacks_by_bki_sq7h, internal::attacks_by_wki_sq7h},
    {internal::attacks_by_bki_sq6h, internal::attacks_by_wki_sq6h},
    {internal::attacks_by_bki_sq5h, internal::attacks_by_wki_sq5h},
    {internal::attacks_by_bki_sq4h, internal::attacks_by_wki_sq4h},
    {internal::attacks_by_bki_sq3h, internal::attacks_by_wki_sq3h},
    {internal::attacks_by_bki_sq2h, internal::attacks_by_wki_sq2h},
    {internal::attacks_by_bki_sq1h, internal::attacks_by_wki_sq1h},
    {internal::attacks_by_bki_sq9i, internal::attacks_by_wki_sq9i},
    {internal::attacks_by_bki_sq8i, internal::attacks_by_wki_sq8i},
    {internal::attacks_by_bki_sq7i, internal::attacks_by_wki_sq7i},
    {internal::attacks_by_bki_sq6i, internal::attacks_by_wki_sq6i},
    {internal::attacks_by_bki_sq5i, internal::attacks_by_wki_sq5i},
    {internal::attacks_by_bki_sq4i, internal::attacks_by_wki_sq4i},
    {internal::attacks_by_bki_sq3i, internal::attacks_by_wki_sq3i},
    {internal::attacks_by_bki_sq2i, internal::attacks_by_wki_sq2i},
    {internal::attacks_by_bki_sq1i, internal::attacks_by_wki_sq1i},
};

constexpr BitBoard attacks_by_ou[Squares::num_squares] = {
    bb_9a.expand_neighbor() & (~bb_9a), bb_8a.expand_neighbor() & (~bb_8a),
    bb_7a.expand_neighbor() & (~bb_7a), bb_6a.expand_neighbor() & (~bb_6a),
    bb_5a.expand_neighbor() & (~bb_5a), bb_4a.expand_neighbor() & (~bb_4a),
    bb_3a.expand_neighbor() & (~bb_3a), bb_2a.expand_neighbor() & (~bb_2a),
    bb_1a.expand_neighbor() & (~bb_1a), bb_9b.expand_neighbor() & (~bb_9b),
    bb_8b.expand_neighbor() & (~bb_8b), bb_7b.expand_neighbor() & (~bb_7b),
    bb_6b.expand_neighbor() & (~bb_6b), bb_5b.expand_neighbor() & (~bb_5b),
    bb_4b.expand_neighbor() & (~bb_4b), bb_3b.expand_neighbor() & (~bb_3b),
    bb_2b.expand_neighbor() & (~bb_2b), bb_1b.expand_neighbor() & (~bb_1b),
    bb_9c.expand_neighbor() & (~bb_9c), bb_8c.expand_neighbor() & (~bb_8c),
    bb_7c.expand_neighbor() & (~bb_7c), bb_6c.expand_neighbor() & (~bb_6c),
    bb_5c.expand_neighbor() & (~bb_5c), bb_4c.expand_neighbor() & (~bb_4c),
    bb_3c.expand_neighbor() & (~bb_3c), bb_2c.expand_neighbor() & (~bb_2c),
    bb_1c.expand_neighbor() & (~bb_1c), bb_9d.expand_neighbor() & (~bb_9d),
    bb_8d.expand_neighbor() & (~bb_8d), bb_7d.expand_neighbor() & (~bb_7d),
    bb_6d.expand_neighbor() & (~bb_6d), bb_5d.expand_neighbor() & (~bb_5d),
    bb_4d.expand_neighbor() & (~bb_4d), bb_3d.expand_neighbor() & (~bb_3d),
    bb_2d.expand_neighbor() & (~bb_2d), bb_1d.expand_neighbor() & (~bb_1d),
    bb_9e.expand_neighbor() & (~bb_9e), bb_8e.expand_neighbor() & (~bb_8e),
    bb_7e.expand_neighbor() & (~bb_7e), bb_6e.expand_neighbor() & (~bb_6e),
    bb_5e.expand_neighbor() & (~bb_5e), bb_4e.expand_neighbor() & (~bb_4e),
    bb_3e.expand_neighbor() & (~bb_3e), bb_2e.expand_neighbor() & (~bb_2e),
    bb_1e.expand_neighbor() & (~bb_1e), bb_9f.expand_neighbor() & (~bb_9f),
    bb_8f.expand_neighbor() & (~bb_8f), bb_7f.expand_neighbor() & (~bb_7f),
    bb_6f.expand_neighbor() & (~bb_6f), bb_5f.expand_neighbor() & (~bb_5f),
    bb_4f.expand_neighbor() & (~bb_4f), bb_3f.expand_neighbor() & (~bb_3f),
    bb_2f.expand_neighbor() & (~bb_2f), bb_1f.expand_neighbor() & (~bb_1f),
    bb_9g.expand_neighbor() & (~bb_9g), bb_8g.expand_neighbor() & (~bb_8g),
    bb_7g.expand_neighbor() & (~bb_7g), bb_6g.expand_neighbor() & (~bb_6g),
    bb_5g.expand_neighbor() & (~bb_5g), bb_4g.expand_neighbor() & (~bb_4g),
    bb_3g.expand_neighbor() & (~bb_3g), bb_2g.expand_neighbor() & (~bb_2g),
    bb_1g.expand_neighbor() & (~bb_1g), bb_9h.expand_neighbor() & (~bb_9h),
    bb_8h.expand_neighbor() & (~bb_8h), bb_7h.expand_neighbor() & (~bb_7h),
    bb_6h.expand_neighbor() & (~bb_6h), bb_5h.expand_neighbor() & (~bb_5h),
    bb_4h.expand_neighbor() & (~bb_4h), bb_3h.expand_neighbor() & (~bb_3h),
    bb_2h.expand_neighbor() & (~bb_2h), bb_1h.expand_neighbor() & (~bb_1h),
    bb_9i.expand_neighbor() & (~bb_9i), bb_8i.expand_neighbor() & (~bb_8i),
    bb_7i.expand_neighbor() & (~bb_7i), bb_6i.expand_neighbor() & (~bb_6i),
    bb_5i.expand_neighbor() & (~bb_5i), bb_4i.expand_neighbor() & (~bb_4i),
    bb_3i.expand_neighbor() & (~bb_3i), bb_2i.expand_neighbor() & (~bb_2i),
    bb_1i.expand_neighbor() & (~bb_1i),
};

constexpr BitBoard attacks_by_hi[Squares::num_squares] = {
    (bb_file9 | bb_ranka) & (~bb_9a), (bb_file8 | bb_ranka) & (~bb_8a),
    (bb_file7 | bb_ranka) & (~bb_7a), (bb_file6 | bb_ranka) & (~bb_6a),
    (bb_file5 | bb_ranka) & (~bb_5a), (bb_file4 | bb_ranka) & (~bb_4a),
    (bb_file3 | bb_ranka) & (~bb_3a), (bb_file2 | bb_ranka) & (~bb_2a),
    (bb_file1 | bb_ranka) & (~bb_1a), (bb_file9 | bb_rankb) & (~bb_9b),
    (bb_file8 | bb_rankb) & (~bb_8b), (bb_file7 | bb_rankb) & (~bb_7b),
    (bb_file6 | bb_rankb) & (~bb_6b), (bb_file5 | bb_rankb) & (~bb_5b),
    (bb_file4 | bb_rankb) & (~bb_4b), (bb_file3 | bb_rankb) & (~bb_3b),
    (bb_file2 | bb_rankb) & (~bb_2b), (bb_file1 | bb_rankb) & (~bb_1b),
    (bb_file9 | bb_rankc) & (~bb_9c), (bb_file8 | bb_rankc) & (~bb_8c),
    (bb_file7 | bb_rankc) & (~bb_7c), (bb_file6 | bb_rankc) & (~bb_6c),
    (bb_file5 | bb_rankc) & (~bb_5c), (bb_file4 | bb_rankc) & (~bb_4c),
    (bb_file3 | bb_rankc) & (~bb_3c), (bb_file2 | bb_rankc) & (~bb_2c),
    (bb_file1 | bb_rankc) & (~bb_1c), (bb_file9 | bb_rankd) & (~bb_9d),
    (bb_file8 | bb_rankd) & (~bb_8d), (bb_file7 | bb_rankd) & (~bb_7d),
    (bb_file6 | bb_rankd) & (~bb_6d), (bb_file5 | bb_rankd) & (~bb_5d),
    (bb_file4 | bb_rankd) & (~bb_4d), (bb_file3 | bb_rankd) & (~bb_3d),
    (bb_file2 | bb_rankd) & (~bb_2d), (bb_file1 | bb_rankd) & (~bb_1d),
    (bb_file9 | bb_ranke) & (~bb_9e), (bb_file8 | bb_ranke) & (~bb_8e),
    (bb_file7 | bb_ranke) & (~bb_7e), (bb_file6 | bb_ranke) & (~bb_6e),
    (bb_file5 | bb_ranke) & (~bb_5e), (bb_file4 | bb_ranke) & (~bb_4e),
    (bb_file3 | bb_ranke) & (~bb_3e), (bb_file2 | bb_ranke) & (~bb_2e),
    (bb_file1 | bb_ranke) & (~bb_1e), (bb_file9 | bb_rankf) & (~bb_9f),
    (bb_file8 | bb_rankf) & (~bb_8f), (bb_file7 | bb_rankf) & (~bb_7f),
    (bb_file6 | bb_rankf) & (~bb_6f), (bb_file5 | bb_rankf) & (~bb_5f),
    (bb_file4 | bb_rankf) & (~bb_4f), (bb_file3 | bb_rankf) & (~bb_3f),
    (bb_file2 | bb_rankf) & (~bb_2f), (bb_file1 | bb_rankf) & (~bb_1f),
    (bb_file9 | bb_rankg) & (~bb_9g), (bb_file8 | bb_rankg) & (~bb_8g),
    (bb_file7 | bb_rankg) & (~bb_7g), (bb_file6 | bb_rankg) & (~bb_6g),
    (bb_file5 | bb_rankg) & (~bb_5g), (bb_file4 | bb_rankg) & (~bb_4g),
    (bb_file3 | bb_rankg) & (~bb_3g), (bb_file2 | bb_rankg) & (~bb_2g),
    (bb_file1 | bb_rankg) & (~bb_1g), (bb_file9 | bb_rankh) & (~bb_9h),
    (bb_file8 | bb_rankh) & (~bb_8h), (bb_file7 | bb_rankh) & (~bb_7h),
    (bb_file6 | bb_rankh) & (~bb_6h), (bb_file5 | bb_rankh) & (~bb_5h),
    (bb_file4 | bb_rankh) & (~bb_4h), (bb_file3 | bb_rankh) & (~bb_3h),
    (bb_file2 | bb_rankh) & (~bb_2h), (bb_file1 | bb_rankh) & (~bb_1h),
    (bb_file9 | bb_ranki) & (~bb_9i), (bb_file8 | bb_ranki) & (~bb_8i),
    (bb_file7 | bb_ranki) & (~bb_7i), (bb_file6 | bb_ranki) & (~bb_6i),
    (bb_file5 | bb_ranki) & (~bb_5i), (bb_file4 | bb_ranki) & (~bb_4i),
    (bb_file3 | bb_ranki) & (~bb_3i), (bb_file2 | bb_ranki) & (~bb_2i),
    (bb_file1 | bb_ranki) & (~bb_1i),
};
constexpr BitBoard attacks_by_ka[Squares::num_squares] = {
    (internal::diagonal_9a_to_se | internal::diagonal_9a_to_sw) & (~bb_9a),
    (internal::diagonal_8a_to_se | internal::diagonal_8a_to_sw) & (~bb_8a),
    (internal::diagonal_7a_to_se | internal::diagonal_7a_to_sw) & (~bb_7a),
    (internal::diagonal_6a_to_se | internal::diagonal_6a_to_sw) & (~bb_6a),
    (internal::diagonal_5a_to_se | internal::diagonal_5a_to_sw) & (~bb_5a),
    (internal::diagonal_4a_to_se | internal::diagonal_4a_to_sw) & (~bb_4a),
    (internal::diagonal_3a_to_se | internal::diagonal_3a_to_sw) & (~bb_3a),
    (internal::diagonal_2a_to_se | internal::diagonal_2a_to_sw) & (~bb_2a),
    (internal::diagonal_1a_to_se | internal::diagonal_1a_to_sw) & (~bb_1a),

    (internal::diagonal_9b_to_se | internal::diagonal_8a_to_sw) & (~bb_9b),
    (internal::diagonal_9a_to_se | internal::diagonal_7a_to_sw) & (~bb_8b),
    (internal::diagonal_8a_to_se | internal::diagonal_6a_to_sw) & (~bb_7b),
    (internal::diagonal_7a_to_se | internal::diagonal_5a_to_sw) & (~bb_6b),
    (internal::diagonal_6a_to_se | internal::diagonal_4a_to_sw) & (~bb_5b),
    (internal::diagonal_5a_to_se | internal::diagonal_3a_to_sw) & (~bb_4b),
    (internal::diagonal_4a_to_se | internal::diagonal_2a_to_sw) & (~bb_3b),
    (internal::diagonal_3a_to_se | internal::diagonal_1a_to_sw) & (~bb_2b),
    (internal::diagonal_2a_to_se | internal::diagonal_1b_to_sw) & (~bb_1b),

    (internal::diagonal_9c_to_se | internal::diagonal_7a_to_sw) & (~bb_9c),
    (internal::diagonal_9b_to_se | internal::diagonal_6a_to_sw) & (~bb_8c),
    (internal::diagonal_9a_to_se | internal::diagonal_5a_to_sw) & (~bb_7c),
    (internal::diagonal_8a_to_se | internal::diagonal_4a_to_sw) & (~bb_6c),
    (internal::diagonal_7a_to_se | internal::diagonal_3a_to_sw) & (~bb_5c),
    (internal::diagonal_6a_to_se | internal::diagonal_2a_to_sw) & (~bb_4c),
    (internal::diagonal_5a_to_se | internal::diagonal_1a_to_sw) & (~bb_3c),
    (internal::diagonal_4a_to_se | internal::diagonal_1b_to_sw) & (~bb_2c),
    (internal::diagonal_3a_to_se | internal::diagonal_1c_to_sw) & (~bb_1c),

    (internal::diagonal_9d_to_se | internal::diagonal_6a_to_sw) & (~bb_9d),
    (internal::diagonal_9c_to_se | internal::diagonal_5a_to_sw) & (~bb_8d),
    (internal::diagonal_9b_to_se | internal::diagonal_4a_to_sw) & (~bb_7d),
    (internal::diagonal_9a_to_se | internal::diagonal_3a_to_sw) & (~bb_6d),
    (internal::diagonal_8a_to_se | internal::diagonal_2a_to_sw) & (~bb_5d),
    (internal::diagonal_7a_to_se | internal::diagonal_1a_to_sw) & (~bb_4d),
    (internal::diagonal_6a_to_se | internal::diagonal_1b_to_sw) & (~bb_3d),
    (internal::diagonal_5a_to_se | internal::diagonal_1c_to_sw) & (~bb_2d),
    (internal::diagonal_4a_to_se | internal::diagonal_1d_to_sw) & (~bb_1d),

    (internal::diagonal_9e_to_se | internal::diagonal_5a_to_sw) & (~bb_9e),
    (internal::diagonal_9d_to_se | internal::diagonal_4a_to_sw) & (~bb_8e),
    (internal::diagonal_9c_to_se | internal::diagonal_3a_to_sw) & (~bb_7e),
    (internal::diagonal_9b_to_se | internal::diagonal_2a_to_sw) & (~bb_6e),
    (internal::diagonal_9a_to_se | internal::diagonal_1a_to_sw) & (~bb_5e),
    (internal::diagonal_8a_to_se | internal::diagonal_1b_to_sw) & (~bb_4e),
    (internal::diagonal_7a_to_se | internal::diagonal_1c_to_sw) & (~bb_3e),
    (internal::diagonal_6a_to_se | internal::diagonal_1d_to_sw) & (~bb_2e),
    (internal::diagonal_5a_to_se | internal::diagonal_1e_to_sw) & (~bb_1e),

    (internal::diagonal_9f_to_se | internal::diagonal_4a_to_sw) & (~bb_9f),
    (internal::diagonal_9e_to_se | internal::diagonal_3a_to_sw) & (~bb_8f),
    (internal::diagonal_9d_to_se | internal::diagonal_2a_to_sw) & (~bb_7f),
    (internal::diagonal_9c_to_se | internal::diagonal_1a_to_sw) & (~bb_6f),
    (internal::diagonal_9b_to_se | internal::diagonal_1b_to_sw) & (~bb_5f),
    (internal::diagonal_9a_to_se | internal::diagonal_1c_to_sw) & (~bb_4f),
    (internal::diagonal_8a_to_se | internal::diagonal_1d_to_sw) & (~bb_3f),
    (internal::diagonal_7a_to_se | internal::diagonal_1e_to_sw) & (~bb_2f),
    (internal::diagonal_6a_to_se | internal::diagonal_1f_to_sw) & (~bb_1f),

    (internal::diagonal_9g_to_se | internal::diagonal_3a_to_sw) & (~bb_9g),
    (internal::diagonal_9f_to_se | internal::diagonal_2a_to_sw) & (~bb_8g),
    (internal::diagonal_9e_to_se | internal::diagonal_1a_to_sw) & (~bb_7g),
    (internal::diagonal_9d_to_se | internal::diagonal_1b_to_sw) & (~bb_6g),
    (internal::diagonal_9c_to_se | internal::diagonal_1c_to_sw) & (~bb_5g),
    (internal::diagonal_9b_to_se | internal::diagonal_1d_to_sw) & (~bb_4g),
    (internal::diagonal_9a_to_se | internal::diagonal_1e_to_sw) & (~bb_3g),
    (internal::diagonal_8a_to_se | internal::diagonal_1f_to_sw) & (~bb_2g),
    (internal::diagonal_7a_to_se | internal::diagonal_1g_to_sw) & (~bb_1g),

    (internal::diagonal_9h_to_se | internal::diagonal_2a_to_sw) & (~bb_9h),
    (internal::diagonal_9g_to_se | internal::diagonal_1a_to_sw) & (~bb_8h),
    (internal::diagonal_9f_to_se | internal::diagonal_1b_to_sw) & (~bb_7h),
    (internal::diagonal_9e_to_se | internal::diagonal_1c_to_sw) & (~bb_6h),
    (internal::diagonal_9d_to_se | internal::diagonal_1d_to_sw) & (~bb_5h),
    (internal::diagonal_9c_to_se | internal::diagonal_1e_to_sw) & (~bb_4h),
    (internal::diagonal_9b_to_se | internal::diagonal_1f_to_sw) & (~bb_3h),
    (internal::diagonal_9a_to_se | internal::diagonal_1g_to_sw) & (~bb_2h),
    (internal::diagonal_8a_to_se | internal::diagonal_1h_to_sw) & (~bb_1h),

    (internal::diagonal_9i_to_se | internal::diagonal_1a_to_sw) & (~bb_9i),
    (internal::diagonal_9h_to_se | internal::diagonal_1b_to_sw) & (~bb_8i),
    (internal::diagonal_9g_to_se | internal::diagonal_1c_to_sw) & (~bb_7i),
    (internal::diagonal_9f_to_se | internal::diagonal_1d_to_sw) & (~bb_6i),
    (internal::diagonal_9e_to_se | internal::diagonal_1e_to_sw) & (~bb_5i),
    (internal::diagonal_9d_to_se | internal::diagonal_1f_to_sw) & (~bb_4i),
    (internal::diagonal_9c_to_se | internal::diagonal_1g_to_sw) & (~bb_3i),
    (internal::diagonal_9b_to_se | internal::diagonal_1h_to_sw) & (~bb_2i),
    (internal::diagonal_9a_to_se | internal::diagonal_1i_to_sw) & (~bb_1i),
};

BitBoard BitBoard::get_attacks_by(
    const Pieces::BoardPieceTypeEnum piece, const Squares::SquareEnum location)
{
    const auto piece_type = Pieces::to_piece_type(piece);
    const auto color = Pieces::get_color(piece);
    switch (piece_type) {
    case Pieces::FU:
        return attacks_by_fu[location][color];
    case Pieces::KY:
        return attacks_by_ky[location][color];
    case Pieces::KE:
        return attacks_by_ke[location][color];
    case Pieces::GI:
        return attacks_by_gi[location][color];
    case Pieces::KA:
        return attacks_by_ka[location];
    case Pieces::KI:
    case Pieces::TO:
    case Pieces::NY:
    case Pieces::NK:
    case Pieces::NG:
        return attacks_by_ki[location][color];
    case Pieces::HI:
        return attacks_by_hi[location];
    case Pieces::UM:
        return attacks_by_ou[location] | attacks_by_ka[location];
    case Pieces::RY:
        return attacks_by_ou[location] | attacks_by_hi[location];
    case Pieces::OU:
        return attacks_by_ou[location];
    default:
        break;
    }
    return BitBoard(0, 0);
}

BitBoard BitBoard::get_attacks_by(
    const Pieces::BoardPieceTypeEnum piece,
    const Squares::SquareEnum location,
    const BitBoard occupied)
{
    const auto piece_type = Pieces::to_piece_type(piece);
    const auto color = Pieces::get_color(piece);
    switch (piece_type) {
    case Pieces::FU:
        return attacks_by_fu[location][color];
    case Pieces::KY:
        return (color == BLACK)
                   ? BitBoard::ranging_attacks_to<DIR_N>(location, occupied)
                   : BitBoard::ranging_attacks_to<DIR_S>(location, occupied);
    case Pieces::KE:
        return attacks_by_ke[location][color];
    case Pieces::GI:
        return attacks_by_gi[location][color];
    case Pieces::KA:
        return BitBoard::ranging_attacks_to_diagonal(location, occupied);
    case Pieces::KI:
    case Pieces::TO:
    case Pieces::NY:
    case Pieces::NK:
    case Pieces::NG:
        return attacks_by_ki[location][color];
    case Pieces::HI:
        return BitBoard::ranging_attacks_to_adjacent(location, occupied);
    case Pieces::UM:
        return attacks_by_ou[location]
               | BitBoard::ranging_attacks_to_diagonal(location, occupied);
    case Pieces::RY:
        return attacks_by_ou[location]
               | BitBoard::ranging_attacks_to_adjacent(location, occupied);
    case Pieces::OU:
        return attacks_by_ou[location];
    default:
        break;
    }
    return BitBoard(0, 0);
}

/**
 * @brief Table of attacks by non-ranging pieces:
 * B_FU, B_KE, B_GI, B_KI, W_FU, W_KE, W_GI, W_KI, and OU.
 */
static Squares::SquareEnum non_ranging_attacks[9][Squares::num_squares][8];

static Squares::SquareEnum ranging_squares_to[Squares::num_squares]
                                             [Squares::num_directions][9];

void init_non_ranging_attacks_table()
{
    constexpr int size
        = sizeof(non_ranging_attacks) / sizeof(non_ranging_attacks[0][0][0]);
    std::fill_n(&non_ranging_attacks[0][0][0], size, Squares::SQ_NA);

    constexpr Pieces::BoardPieceTypeEnum non_ranging_pieces[]
        = {Pieces::B_FU,
           Pieces::B_KE,
           Pieces::B_GI,
           Pieces::B_KI,
           Pieces::W_FU,
           Pieces::W_KE,
           Pieces::W_GI,
           Pieces::W_KI,
           Pieces::B_OU};
    constexpr int num
        = sizeof(non_ranging_pieces) / sizeof(non_ranging_pieces[0]);
    for (int ii = 0; ii < num; ++ii) {
        const auto p = non_ranging_pieces[ii];
        for (auto& src : Squares::square_array) {
            const auto attacks = BitBoard::get_attacks_by(p, src);
            int index = 0;
            for (auto& dst : Squares::square_array) {
                if (attacks.is_one(dst))
                    non_ranging_attacks[ii][src][index++] = dst;
            }
        }
    }
}

void init_ranging_squares_table()
{
    constexpr int size
        = sizeof(ranging_squares_to) / sizeof(ranging_squares_to[0][0][0]);
    std::fill_n(&ranging_squares_to[0][0][0], size, Squares::SQ_NA);

    for (auto& src : Squares::square_array) {
        for (auto& dir : Squares::direction_array) {
            auto dst = src;
            int index = 0;
            while (true) {
                dst = Squares::shift(dst, dir);
                if (dst == Squares::SQ_NA)
                    break;
                ranging_squares_to[src][dir][index++] = dst;
                if (dir > DIR_SE)
                    break;
            }
        }
    }
}

const Squares::SquareEnum* BitBoard::get_attacks_by_non_ranging(
    const Pieces::BoardPieceTypeEnum& piece,
    const Squares::SquareEnum& location)
{
    switch (piece) {
    case Pieces::B_FU:
        return non_ranging_attacks[0][location];
    case Pieces::B_KE:
        return non_ranging_attacks[1][location];
    case Pieces::B_GI:
        return non_ranging_attacks[2][location];
    case Pieces::B_KI:
    case Pieces::B_TO:
    case Pieces::B_NY:
    case Pieces::B_NK:
    case Pieces::B_NG:
        return non_ranging_attacks[3][location];
    case Pieces::W_FU:
        return non_ranging_attacks[4][location];
    case Pieces::W_KE:
        return non_ranging_attacks[5][location];
    case Pieces::W_GI:
        return non_ranging_attacks[6][location];
    case Pieces::W_KI:
    case Pieces::W_TO:
    case Pieces::W_NY:
    case Pieces::W_NK:
    case Pieces::W_NG:
        return non_ranging_attacks[7][location];
    case Pieces::B_OU:
    case Pieces::W_OU:
        return non_ranging_attacks[8][location];
    default:
        return nullptr;
    }
}

const Squares::SquareEnum* BitBoard::get_squares_along(
    const DirectionEnum& direction, const Squares::SquareEnum& location)
{
    if ((direction == DIR_NA) || (location == Squares::SQ_NA))
        return nullptr;
    return ranging_squares_to[location][direction];
}

} // namespace vshogi::shogi
