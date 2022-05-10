#pragma once

#include <optional>

#include "traits.hpp"

namespace sg {
    template<typename T>
    using maybe = std::optional<T>;

    template<typename T>
    constexpr bool is_maybe_v = util::is_specialization_of_v<std::optional, T>;

    constexpr std::nullopt_t nil = std::nullopt;

    namespace util {
        template<typename T>
        T either(maybe<T> x, T y) {
          return x ? *x : y;
        }
    }
}