#pragma once

#include "ctprintf/detail/parser.hpp"
#include "ctprintf/output.hpp"
#include "ctprintf/types.hpp"

#include <array>
#include <cstddef>
#include <cstdint>
#include <type_traits>
#include <utility>

namespace ctprintf::detail {

template <Output O>
void put_repeat(O &output, char character, int count)
{
    for (int index = 0; index < count; ++index)
        output.put(character);
}

template <Output O>
void write_string(O &output, const char *value, const format_spec &spec)
{
    if (value == nullptr)
        value = "(null)";

    std::size_t length = 0;
    while (value[length] != '\0')
        ++length;

    const int padding =
        spec.width > static_cast<int>(length) ? spec.width - static_cast<int>(length) : 0;
    if (!spec.left)
        put_repeat(output, ' ', padding);
    for (std::size_t index = 0; index < length; ++index)
        output.put(value[index]);
    if (spec.left)
        put_repeat(output, ' ', padding);
}

constexpr unsigned int base_of(conversion type)
{
    switch (type) {
    case conversion::octal:
        return 8;
    case conversion::hexadecimal:
    case conversion::hexadecimal_upper:
    case conversion::pointer:
        return 16;
    default:
        return 10;
    }
}

constexpr char sign_of(bool negative, const format_spec &spec)
{
    if (negative)
        return '-';
    if (spec.type != conversion::signed_decimal)
        return '\0';
    if (spec.plus)
        return '+';
    return spec.space ? ' ' : '\0';
}

struct number_prefix {
    std::array<char, 2> chars{};
    int length = 0;
};

constexpr number_prefix prefix_of(const format_spec &spec, bool nonzero, char first_digit)
{
    number_prefix prefix;
    const bool is_hexadecimal =
        spec.type == conversion::hexadecimal || spec.type == conversion::hexadecimal_upper;
    if (spec.alternate && spec.type == conversion::octal && first_digit != '0')
        prefix.chars[prefix.length++] = '0';
    if (spec.type == conversion::pointer || (spec.alternate && nonzero && is_hexadecimal)) {
        prefix.chars[prefix.length++] = '0';
        prefix.chars[prefix.length++] = spec.type == conversion::hexadecimal_upper ? 'X' : 'x';
    }
    return prefix;
}

template <Output O>
void write_character(O &output, char value, const format_spec &spec)
{
    const int padding = spec.width > 1 ? spec.width - 1 : 0;
    if (!spec.left)
        put_repeat(output, ' ', padding);
    output.put(value);
    if (spec.left)
        put_repeat(output, ' ', padding);
}

template <Output O, typename T>
void write_integer(O &output, T value, const format_spec &spec)
{
    using value_type = std::remove_cvref_t<T>;
    using unsigned_type = std::make_unsigned_t<value_type>;

    auto magnitude = static_cast<unsigned_type>(value);
    bool negative = false;
    if constexpr (std::is_signed_v<value_type>) {
        if (spec.type == conversion::signed_decimal && value < 0) {
            negative = true;
            magnitude = unsigned_type{0} - magnitude;
        }
    }

    const unsigned int base = base_of(spec.type);

    const char *digits =
        spec.type == conversion::hexadecimal_upper ? "0123456789ABCDEF" : "0123456789abcdef";
    const bool nonzero = magnitude != 0;
    std::array<char, sizeof(value_type) * 8 + 1> buffer{};
    int digit_count = 0;
    do {
        buffer[digit_count++] = digits[magnitude % base];
        magnitude /= base;
    } while (magnitude != 0);

    const char sign = sign_of(negative, spec);
    const number_prefix prefix = prefix_of(spec, nonzero, buffer[digit_count - 1]);

    const int sign_length = sign == '\0' ? 0 : 1;
    const int content_length = sign_length + prefix.length + digit_count;
    const int padding = spec.width > content_length ? spec.width - content_length : 0;
    const bool zero_padding = spec.zero && !spec.left;

    if (!spec.left && !zero_padding)
        put_repeat(output, ' ', padding);
    if (sign != '\0')
        output.put(sign);
    for (int index = 0; index < prefix.length; ++index)
        output.put(prefix.chars[index]);
    if (zero_padding)
        put_repeat(output, '0', padding);
    while (digit_count > 0)
        output.put(buffer[--digit_count]);
    if (spec.left)
        put_repeat(output, ' ', padding);
}

template <Output O, typename T>
void write_value(O &output, const format_spec &spec, const T &value)
{
    using value_type = std::remove_cvref_t<T>;

    if constexpr (std::is_integral_v<value_type>) {
        if constexpr (std::is_same_v<value_type, bool>) {
            write_character(output, static_cast<char>(value), spec);
        } else if (spec.type == conversion::character) {
            write_character(output, static_cast<char>(value), spec);
        } else {
            write_integer(output, value, spec);
        }
    }

    if constexpr (std::is_convertible_v<value_type, const char *>) {
        if (spec.type == conversion::string)
            write_string(output, value, spec);
    }

    if constexpr (std::is_pointer_v<value_type> || std::is_same_v<value_type, std::nullptr_t>) {
        if (spec.type == conversion::pointer) {
            std::uintptr_t address = 0;
            if constexpr (std::is_pointer_v<value_type>)
                address = reinterpret_cast<std::uintptr_t>(value);

            format_spec pointer_spec = spec;
            pointer_spec.type = conversion::pointer;
            pointer_spec.alternate = true;
            write_integer(output, address, pointer_spec);
        }
    }
}

template <Output O>
bool write_literals_until_specifier(O &output, const char *&cursor)
{
    while (*cursor != '\0') {
        if (*cursor != '%') {
            output.put(*cursor++);
            continue;
        }

        ++cursor;
        if (*cursor == '%') {
            output.put('%');
            ++cursor;
            continue;
        }

        return true;
    }

    return false;
}

template <Output O>
void write_formatted_arguments(O &output, const char *&cursor)
{
    while (write_literals_until_specifier(output, cursor))
        static_cast<void>(parse_spec(cursor));
}

template <Output O, typename First, typename... Rest>
void write_formatted_arguments(O &output, const char *&cursor, const First &first, Rest &&...rest)
{
    if (!write_literals_until_specifier(output, cursor))
        return;

    const parsed_spec parsed = parse_spec(cursor);
    write_value(output, parsed.spec, first);
    write_formatted_arguments(output, cursor, std::forward<Rest>(rest)...);
}

} // namespace ctprintf::detail
