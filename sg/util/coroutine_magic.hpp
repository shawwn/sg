#define yield co_yield
#define yield_from(f) for (auto x : f) { co_yield x; }
#define halt co_return
#define coro template<typename T> Generator<T>