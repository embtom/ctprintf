#pragma once

#include "ctprintf/types.hpp"

#include <climits>
#include <cstddef>
#include <type_traits>

namespace ctprintf::detail {

constexpr conversion parse_conversion(char character)
{
    switch (character) {
    case 'c':
        return conversion::character;
    case 's':
        return conversion::string;
    case 'd':
    case 'i':
        return conversion::signed_decimal;
    case 'u':
        return conversion::unsigned_decimal;
    case 'o':
        return conversion::octal;
    case 'x':
        return conversion::hexadecimal;
    case 'X':
        return conversion::hexadecimal_upper;
    case 'p':
        return conversion::pointer;
    default:
        return conversion::character;
    }
}

constexpr bool is_conversion(char character)
{
    switch (character) {
    case 'c':
    case 's':
    case 'd':
    case 'i':
    case 'u':
    case 'o':
    case 'x':
    case 'X':
    case 'p':
        return true;
    default:
        return false;
    }
}

struct parsed_spec {
    format_spec spec{};
    bool valid = true;
};

constexpr parsed_spec parse_spec(const char *&cursor)
{
    parsed_spec parsed{};
    format_spec &result = parsed.spec;

    bool parsing_flags = true;
    while (parsing_flags) {
        switch (*cursor) {
        case '-':
            result.left = true;
            ++cursor;
            break;
        case '+':
            result.plus = true;
            ++cursor;
            break;
        case ' ':
            result.space = true;
            ++cursor;
            break;
        case '#':
            result.alternate = true;
            ++cursor;
            break;
        case '0':
            result.zero = true;
            ++cursor;
            break;
        default:
            parsing_flags = false;
            break;
        }
    }

    if (*cursor >= '0' && *cursor <= '9') {
        result.width = 0;
        while (*cursor >= '0' && *cursor <= '9') {
            if (const int digit = *cursor - '0'; result.width > (INT_MAX - digit) / 10) {
                parsed.valid = false;
                result.width = INT_MAX;
            } else if (result.width != INT_MAX) {
                result.width = result.width * 10 + digit;
            }
            ++cursor;
        }
    }

    if (*cursor == '\0') {
        parsed.valid = false;
        return parsed;
    }

    if (!is_conversion(*cursor))
        parsed.valid = false;

    result.type = parse_conversion(*cursor);
    ++cursor;
    return parsed;
}

void invalid_format();

template <typename T>
constexpr bool argument_matches(conversion type)
{
    using value_type = std::remove_cvref_t<T>;

    switch (type) {
    case conversion::signed_decimal:
        return std::is_integral_v<value_type> && std::is_signed_v<value_type>;
    case conversion::unsigned_decimal:
    case conversion::octal:
    case conversion::hexadecimal:
    case conversion::hexadecimal_upper:
        return std::is_integral_v<value_type> && !std::is_same_v<value_type, bool> &&
               !std::is_signed_v<value_type>;
    case conversion::character:
        return std::is_integral_v<value_type>;
    case conversion::string:
        return std::is_convertible_v<value_type, const char *>;
    case conversion::pointer:
        return (std::is_pointer_v<value_type> &&
                (std::is_object_v<std::remove_pointer_t<value_type>> ||
                 std::is_void_v<std::remove_pointer_t<value_type>>)) ||
               std::is_same_v<value_type, std::nullptr_t>;
    case conversion::percent:
        return false;
    }

    return false;
}

template <typename... Args>
consteval void validate_format(const char *format)
{
    constexpr std::size_t argument_count = sizeof...(Args);
    std::size_t argument_index = 0;

    while (*format != '\0') {
        if (*format++ != '%')
            continue;

        if (*format == '%') {
            ++format;
            continue;
        }

        const parsed_spec parsed = parse_spec(format);
        if (!parsed.valid)
            invalid_format();
        const format_spec &spec = parsed.spec;
        if constexpr (argument_count == 0) {
            invalid_format();
        } else {
            constexpr std::array<bool (*)(conversion), argument_count> matches{
                &argument_matches<Args>...};
            if (argument_index >= argument_count)
                invalid_format();
            if (!matches[argument_index](spec.type))
                invalid_format();
        }
        ++argument_index;
    }

    if (argument_index != argument_count)
        invalid_format();
}

} // namespace ctprintf::detail

namespace ctprintf {

template <typename... Args>
class format_string {
  public:
    template <std::size_t N>
    consteval format_string(const char (&text)[N])
     : text_(text)
    {
        detail::validate_format<Args...>(text);
    }

    [[nodiscard]] constexpr const char *data() const { return text_; }

  private:
    const char *text_;
};

} // namespace ctprintf
