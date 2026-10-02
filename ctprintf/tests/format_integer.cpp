#include <string>

#include <ctprintf/format.hpp>

#include <gtest/gtest.h>

#include <cstdint>

namespace {

struct StringOutput {
    std::string buffer;

    void put(char character) { buffer.push_back(character); }
};

} // namespace

TEST(FormatIntegerTest, FormatsZeroPaddedHexadecimal)
{
    StringOutput output;
    ctprintf::format(output, "PC=%08x", std::uint32_t{0x2a});

    EXPECT_EQ(output.buffer, "PC=0000002a");
}

TEST(FormatIntegerTest, FormatsAlternateAndLeftAlignedHexadecimal)
{
    StringOutput output;
    ctprintf::format(output, "%-#8X", std::uint32_t{0x2a});

    EXPECT_EQ(output.buffer, "0X2A    ");
}

TEST(FormatIntegerTest, FormatsOctalAndEscapedPercent)
{
    StringOutput output;
    ctprintf::format(output, "load=%% %o", 8U);

    EXPECT_EQ(output.buffer, "load=% 10");
}

TEST(FormatIntegerTest, FormatsSignedDecimalAndFlags)
{
    StringOutput output;
    ctprintf::format(output, "%+06d", -42);

    EXPECT_EQ(output.buffer, "-00042");
}

TEST(FormatIntegerTest, FormatsUnsignedDecimal)
{
    StringOutput output;
    ctprintf::format(output, "%u", std::uint32_t{4294967295});

    EXPECT_EQ(output.buffer, "4294967295");
}
