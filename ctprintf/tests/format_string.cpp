#include <ctprintf/format.hpp>

#include <gtest/gtest.h>

#include <string>

namespace {

struct StringOutput {
    std::string buffer;

    void put(char character) { buffer.push_back(character); }
};

} // namespace

TEST(FormatStringTest, FormatsStringCharacterAndLiteralPercent)
{
    StringOutput output;
    ctprintf::format(output, "Hello, %-6s %c %%", "world", '!');

    EXPECT_EQ(output.buffer, "Hello, world  ! %");
}

TEST(FormatStringTest, FormatsBooleanAsCharacter)
{
    StringOutput output;
    ctprintf::format(output, "%c", true);

    EXPECT_EQ(output.buffer, std::string(1, '\x01'));
}

TEST(FormatStringTest, WritesLiteralWithoutArguments)
{
    StringOutput output;
    ctprintf::format(output, "ready %%");

    EXPECT_EQ(output.buffer, "ready %");
}

TEST(FormatStringTest, FormatsNullString)
{
    StringOutput output;
    const char *value = nullptr;
    ctprintf::format(output, "%s", value);

    EXPECT_EQ(output.buffer, "(null)");
}

TEST(FormatStringTest, FormatsObjectPointer)
{
    StringOutput output;
    int value = 0;
    ctprintf::format(output, "%p", &value);

    EXPECT_EQ(output.buffer.substr(0, 2), "0x");
}

TEST(FormatStringTest, FormatsNullPointer)
{
    StringOutput output;
    ctprintf::format(output, "%p", nullptr);

    EXPECT_EQ(output.buffer, "0x0");
}

TEST(FormatStringTest, FormatsMultipleArguments)
{
    StringOutput output;
    ctprintf::format(output, "%d:%s:%c:%u", -42, "answer", '!', 42U);

    EXPECT_EQ(output.buffer, "-42:answer:!:42");
}
