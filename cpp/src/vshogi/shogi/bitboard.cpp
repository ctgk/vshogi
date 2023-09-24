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

} // namespace internal

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
    bb_9a.expand_neighbor() & ~bb_9a, bb_8a.expand_neighbor() & ~bb_8a,
    bb_7a.expand_neighbor() & ~bb_7a, bb_6a.expand_neighbor() & ~bb_6a,
    bb_5a.expand_neighbor() & ~bb_5a, bb_4a.expand_neighbor() & ~bb_4a,
    bb_3a.expand_neighbor() & ~bb_3a, bb_2a.expand_neighbor() & ~bb_2a,
    bb_1a.expand_neighbor() & ~bb_1a, bb_9b.expand_neighbor() & ~bb_9b,
    bb_8b.expand_neighbor() & ~bb_8b, bb_7b.expand_neighbor() & ~bb_7b,
    bb_6b.expand_neighbor() & ~bb_6b, bb_5b.expand_neighbor() & ~bb_5b,
    bb_4b.expand_neighbor() & ~bb_4b, bb_3b.expand_neighbor() & ~bb_3b,
    bb_2b.expand_neighbor() & ~bb_2b, bb_1b.expand_neighbor() & ~bb_1b,
    bb_9c.expand_neighbor() & ~bb_9c, bb_8c.expand_neighbor() & ~bb_8c,
    bb_7c.expand_neighbor() & ~bb_7c, bb_6c.expand_neighbor() & ~bb_6c,
    bb_5c.expand_neighbor() & ~bb_5c, bb_4c.expand_neighbor() & ~bb_4c,
    bb_3c.expand_neighbor() & ~bb_3c, bb_2c.expand_neighbor() & ~bb_2c,
    bb_1c.expand_neighbor() & ~bb_1c, bb_9d.expand_neighbor() & ~bb_9d,
    bb_8d.expand_neighbor() & ~bb_8d, bb_7d.expand_neighbor() & ~bb_7d,
    bb_6d.expand_neighbor() & ~bb_6d, bb_5d.expand_neighbor() & ~bb_5d,
    bb_4d.expand_neighbor() & ~bb_4d, bb_3d.expand_neighbor() & ~bb_3d,
    bb_2d.expand_neighbor() & ~bb_2d, bb_1d.expand_neighbor() & ~bb_1d,
    bb_9e.expand_neighbor() & ~bb_9e, bb_8e.expand_neighbor() & ~bb_8e,
    bb_7e.expand_neighbor() & ~bb_7e, bb_6e.expand_neighbor() & ~bb_6e,
    bb_5e.expand_neighbor() & ~bb_5e, bb_4e.expand_neighbor() & ~bb_4e,
    bb_3e.expand_neighbor() & ~bb_3e, bb_2e.expand_neighbor() & ~bb_2e,
    bb_1e.expand_neighbor() & ~bb_1e, bb_9f.expand_neighbor() & ~bb_9f,
    bb_8f.expand_neighbor() & ~bb_8f, bb_7f.expand_neighbor() & ~bb_7f,
    bb_6f.expand_neighbor() & ~bb_6f, bb_5f.expand_neighbor() & ~bb_5f,
    bb_4f.expand_neighbor() & ~bb_4f, bb_3f.expand_neighbor() & ~bb_3f,
    bb_2f.expand_neighbor() & ~bb_2f, bb_1f.expand_neighbor() & ~bb_1f,
    bb_9g.expand_neighbor() & ~bb_9g, bb_8g.expand_neighbor() & ~bb_8g,
    bb_7g.expand_neighbor() & ~bb_7g, bb_6g.expand_neighbor() & ~bb_6g,
    bb_5g.expand_neighbor() & ~bb_5g, bb_4g.expand_neighbor() & ~bb_4g,
    bb_3g.expand_neighbor() & ~bb_3g, bb_2g.expand_neighbor() & ~bb_2g,
    bb_1g.expand_neighbor() & ~bb_1g, bb_9h.expand_neighbor() & ~bb_9h,
    bb_8h.expand_neighbor() & ~bb_8h, bb_7h.expand_neighbor() & ~bb_7h,
    bb_6h.expand_neighbor() & ~bb_6h, bb_5h.expand_neighbor() & ~bb_5h,
    bb_4h.expand_neighbor() & ~bb_4h, bb_3h.expand_neighbor() & ~bb_3h,
    bb_2h.expand_neighbor() & ~bb_2h, bb_1h.expand_neighbor() & ~bb_1h,
    bb_9i.expand_neighbor() & ~bb_9i, bb_8i.expand_neighbor() & ~bb_8i,
    bb_7i.expand_neighbor() & ~bb_7i, bb_6i.expand_neighbor() & ~bb_6i,
    bb_5i.expand_neighbor() & ~bb_5i, bb_4i.expand_neighbor() & ~bb_4i,
    bb_3i.expand_neighbor() & ~bb_3i, bb_2i.expand_neighbor() & ~bb_2i,
    bb_1i.expand_neighbor() & ~bb_1i,
};

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

} // namespace vshogi::shogi
