#pragma once

#include "./sg.hpp"

namespace sg {
extern auto fibonacci_sequence(auto n) -> Generator<decltype(n)>;

extern void testCoroutine();
} // namespace sg