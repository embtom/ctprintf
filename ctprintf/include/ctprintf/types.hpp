#pragma once

#include <array>
#include <cstddef>

namespace ctprintf {

template <std::size_t N>
struct fixed_string {
    std::array<char, N> data{};

    constexpr explicit fixed_string(const char (&text)[N])
    {
        for (std::size_t index = 0; index < N; ++index)
            data[index] = text[index];
    }

    [[nodiscard]] constexpr std::size_t size() const { return N - 1; }

    [[nodiscard]] constexpr char operator[](std::size_t index) const { return data[index]; }
};

enum class conversion {
    character,
    string,
    signed_decimal,
    unsigned_decimal,
    octal,
    hexadecimal,
    hexadecimal_upper,
    pointer,
    percent
};

struct format_spec {
    conversion type{};
    bool left = false;
    bool plus = false;
    bool space = false;
    bool alternate = false;
    bool zero = false;
    int width = -1;
    int precision = -1;
};

} // namespace ctprintf
