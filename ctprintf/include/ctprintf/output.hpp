#pragma once

namespace ctprintf {

// clang-format off
template <typename T>
concept Output = requires(T &output, char character) {
    output.put(character);
};
// clang-format on

} // namespace ctprintf
