#pragma once

#include <optional>

#include "traits.hpp"

namespace sg {
template <typename T>
requires(not util::is_specialization_of_v<std::optional, T>)
using maybe = std::optional<T>;

template <typename T>
constexpr bool is_maybe_v = util::is_specialization_of_v<std::optional, T>;

constexpr std::nullopt_t nil = std::nullopt;

namespace util {
template <typename T, typename U>
requires std::convertible_to<U, T>
T&& either(maybe<T>&& x, U&& y) {
  return std::forward<decltype(x)::value_type>(x.value_or(static_cast<T>(y)));
}
} // namespace util
} // namespace sg