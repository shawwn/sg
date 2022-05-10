#define yield(...) co_yield (__VA_ARGS__)
#define yield_from(f)                                                          \
  for (auto&& x : f) {                                                         \
    co_yield (std::forward<decltype(x)>(x));                                   \
  }
#define halt co_return
#define coro                                                                   \
  template <typename T>                                                        \
  sg::Generator<T>