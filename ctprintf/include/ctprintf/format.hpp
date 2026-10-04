#pragma once

#include "ctprintf/detail/formatter.hpp"
#include "ctprintf/detail/parser.hpp"

#include <type_traits>
#include <utility>

namespace ctprintf {

template <typename... Args>
using format_text = format_string<std::type_identity_t<Args>...>;

template <Output O, typename... Args>
void format(O &output, format_text<Args...> format_text, Args &&...args)
{
    const char *cursor = format_text.data();
    if constexpr (sizeof...(Args) == 0)
        detail::write_formatted_arguments(output, cursor);
    else
        detail::write_formatted_arguments(output, cursor, std::forward<Args>(args)...);
}

} // namespace ctprintf
