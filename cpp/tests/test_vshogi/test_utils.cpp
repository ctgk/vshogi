#include "vshogi/common/utils.hpp"

#include <CppUTest/TestHarness.h>

namespace test_vshogi
{

using namespace vshogi;

TEST_GROUP (test_clz) {
};

TEST(test_clz, test_uint32)
{
    CHECK_EQUAL(0u, clz(static_cast<std::uint32_t>(0xffffffff)));
    CHECK_EQUAL(4u, clz(static_cast<std::uint32_t>(0x0fffffff)));
}

TEST(test_clz, test_uint64)
{
    CHECK_EQUAL(32u, clz(static_cast<std::uint64_t>(0x00000000ffffffff)));
    CHECK_EQUAL(8u, clz(static_cast<std::uint64_t>(0x00ffffffffffffff)));
}

TEST(test_clz, test_uint128)
{
    CHECK_EQUAL(128u, clz(static_cast<uint128>(0)));
    CHECK_EQUAL(64u + 8u, clz(static_cast<uint128>(0x00ffffffffffffff)));
}

} // namespace test_vshogi
