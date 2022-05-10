#pragma once

#include "./coroutine.hpp"

namespace sg {
extern auto fibonacci_sequence(auto n) -> sg::Generator<decltype(n)>;

extern void testCoroutine();
} // namespace sg