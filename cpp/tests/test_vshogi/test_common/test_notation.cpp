#include "vshogi/common/notation.hpp"
#include "vshogi/variants/judkins_shogi.hpp"
#include "vshogi/variants/minishogi.hpp"
#include "vshogi/variants/shogi.hpp"

#include <CppUTest/TestHarness.h>

namespace test_vshogi
{

TEST_GROUP (test_to_sfen) {
    template <class P, class T>
    void check(const T& a, const char expect[])
    {
        using NT = vshogi::Notation<P>;
        {
            char actual[10] = {};
            NT::to_sfen(a, actual);
            STRCMP_EQUAL(expect, actual);
        }
        {
            const std::string actual = NT::to_sfen(a);
            STRCMP_EQUAL(expect, actual.c_str());
        }
    }
};

TEST(test_to_sfen, minishogi_move)
{
    using namespace vshogi::minishogi;
    using P = vshogi::minishogi::Parameters;
    CHECK_TRUE(Move(SQ_1A, SQ_1B, true) == Move("1a1b+"));
    check<P>(Move(FU, SQ_3C), "P*3c");
    check<P>(Move(SQ_1A, SQ_1B), "1a1b");
    check<P>(Move(SQ_1A, SQ_1B, true), "1a1b+");
}

TEST(test_to_sfen, judkins_shogi_move)
{
    using namespace vshogi::judkins_shogi;
    using P = vshogi::judkins_shogi::Parameters;
    CHECK_TRUE(Move(SQ_1A, SQ_1B, true) == Move("1a1b+"));
    check<P>(Move(FU, SQ_3C), "P*3c");
    check<P>(Move(SQ_1A, SQ_1B), "1a1b");
    check<P>(Move(SQ_1A, SQ_1B, true), "1a1b+");
}

TEST(test_to_sfen, standard_shogi_square)
{
    using namespace vshogi::shogi;
    using P = vshogi::shogi::Parameters;
    check<P>(SQ_1A, "1a");
    check<P>(SQ_1B, "1b");
    check<P>(SQ_1C, "1c");
    check<P>(SQ_1D, "1d");
    check<P>(SQ_1E, "1e");
    check<P>(SQ_1F, "1f");
    check<P>(SQ_1G, "1g");
    check<P>(SQ_1H, "1h");
    check<P>(SQ_1I, "1i");
    check<P>(SQ_2A, "2a");
    check<P>(SQ_2B, "2b");
    check<P>(SQ_2C, "2c");
    check<P>(SQ_2D, "2d");
    check<P>(SQ_2E, "2e");
    check<P>(SQ_2F, "2f");
    check<P>(SQ_2G, "2g");
    check<P>(SQ_2H, "2h");
    check<P>(SQ_2I, "2i");
    check<P>(SQ_3A, "3a");
    check<P>(SQ_3B, "3b");
    check<P>(SQ_3C, "3c");
    check<P>(SQ_3D, "3d");
    check<P>(SQ_3E, "3e");
    check<P>(SQ_3F, "3f");
    check<P>(SQ_3G, "3g");
    check<P>(SQ_3H, "3h");
    check<P>(SQ_3I, "3i");
    check<P>(SQ_4A, "4a");
    check<P>(SQ_4B, "4b");
    check<P>(SQ_4C, "4c");
    check<P>(SQ_4D, "4d");
    check<P>(SQ_4E, "4e");
    check<P>(SQ_4F, "4f");
    check<P>(SQ_4G, "4g");
    check<P>(SQ_4H, "4h");
    check<P>(SQ_4I, "4i");
    check<P>(SQ_5A, "5a");
    check<P>(SQ_5B, "5b");
    check<P>(SQ_5C, "5c");
    check<P>(SQ_5D, "5d");
    check<P>(SQ_5E, "5e");
    check<P>(SQ_5F, "5f");
    check<P>(SQ_5G, "5g");
    check<P>(SQ_5H, "5h");
    check<P>(SQ_5I, "5i");
    check<P>(SQ_6A, "6a");
    check<P>(SQ_6B, "6b");
    check<P>(SQ_6C, "6c");
    check<P>(SQ_6D, "6d");
    check<P>(SQ_6E, "6e");
    check<P>(SQ_6F, "6f");
    check<P>(SQ_6G, "6g");
    check<P>(SQ_6H, "6h");
    check<P>(SQ_6I, "6i");
    check<P>(SQ_7A, "7a");
    check<P>(SQ_7B, "7b");
    check<P>(SQ_7C, "7c");
    check<P>(SQ_7D, "7d");
    check<P>(SQ_7E, "7e");
    check<P>(SQ_7F, "7f");
    check<P>(SQ_7G, "7g");
    check<P>(SQ_7H, "7h");
    check<P>(SQ_7I, "7i");
    check<P>(SQ_8A, "8a");
    check<P>(SQ_8B, "8b");
    check<P>(SQ_8C, "8c");
    check<P>(SQ_8D, "8d");
    check<P>(SQ_8E, "8e");
    check<P>(SQ_8F, "8f");
    check<P>(SQ_8G, "8g");
    check<P>(SQ_8H, "8h");
    check<P>(SQ_8I, "8i");
    check<P>(SQ_9A, "9a");
    check<P>(SQ_9B, "9b");
    check<P>(SQ_9C, "9c");
    check<P>(SQ_9D, "9d");
    check<P>(SQ_9E, "9e");
    check<P>(SQ_9F, "9f");
    check<P>(SQ_9G, "9g");
    check<P>(SQ_9H, "9h");
    check<P>(SQ_9I, "9i");
}

TEST(test_to_sfen, standard_shogi_move)
{
    using namespace vshogi::shogi;
    using P = vshogi::shogi::Parameters;
    CHECK_TRUE(Move(SQ_1A, SQ_1B, true) == Move("1a1b+"));
    check<P>(Move(FU, SQ_3G), "P*3g");
    check<P>(Move(SQ_1A, SQ_1B), "1a1b");
    check<P>(Move(SQ_1A, SQ_1B, true), "1a1b+");
}

} // namespace test_vshogi
