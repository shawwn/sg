//
// Created by Shawn Presser on 4/21/22.
//

#include "coroutine_test.hpp"
#include <list>
#include <map>
#include <vector>

#include "coroutine_magic.hpp"

using namespace sg::cpp;

auto seq_value_type_v(auto&& seq) -> Bare<decltype(*std::begin(seq))> {}

template <template <typename...> typename Container>
auto container_of_v(auto&& seq) -> Container<decltype(seq_value_type_v(seq))> {}

auto vector_of_v(auto&& seq) -> decltype(container_of_v<std::vector>(seq)) {}

auto maybe_of_v(auto&& seq) -> decltype(container_of_v<maybe>(seq)) {}

template <typename T>
requires requires(T t) { maybe_of_v<decltype(seq_value_type_v(t))>; }
constexpr bool is_seq_of_maybe_v() {
  return true;
}
template <typename T>
constexpr bool is_seq_of_maybe_v() {
  return false;
}
constexpr bool is_seq_of_maybe_v(auto&& seq) {
  return is_seq_of_maybe_v<decltype(seq)>();
}

template <typename T>
requires requires(T t) { std::size(t); }
auto len(T&& seq) {
  return std::size(seq);
}

auto len(auto&& seq) {
  auto i = 0;
  for (auto x : seq) {
    i += 1;
  }
  return i;
}

auto len(const char* seq) { return std::strlen(seq); }

template <typename T>
requires requires(T t) {
           std::rbegin(t);
           std::rend(t);
         }
auto reversed(T seq) -> Generator<decltype(seq_value_type_v(seq))> {
  for (auto it = seq.rbegin(); it != seq.rend(); it++) {
    yield(*it);
  }
}

template <typename T>
Generator<T> reversed(Generator<T> seq) {
  using vector_t = decltype(vector_of_v(seq));
  vector_t buffered;
  std::copy(std::begin(seq), std::end(seq), std::back_inserter(buffered));
  yield_from(reversed(buffered));
}

auto maybes(auto&& seq) -> std::vector<decltype(maybe_of_v(seq))> {
  decltype(maybes(seq)) vec;
  std::copy(std::begin(seq), std::end(seq), std::back_inserter(vec));
  return vec;
}

template <typename T, template <typename...> typename V = maybe>
requires requires(T t) {
           std::is_same_v<decltype(seq_value_type_v(t)),
               V<decltype(seq_value_type_v(t))>>;
         }
auto maybes2(T seq) {
  return std::move(seq);
}

template <typename T>
requires requires(T t) { maybe_of_v<decltype(seq_value_type_v(t))>; }
auto maybes3(T seq) {
  return std::move(seq);
}

template <typename T>
requires requires(T t) { is_seq_of_maybe_v<T>(); }
auto maybes4(T seq) {
  return std::move(seq);
}

auto maybes5(auto seq) {
  if constexpr (is_seq_of_maybe_v<decltype(seq)>()) {
    return std::move(seq);
  } else {
    return maybes(seq);
  }
}

auto maybes6(auto seq) {
  if constexpr (is_seq_of_maybe_v(seq)) {
    return std::move(seq);
  } else {
    return maybes(seq);
  }
}

auto range(auto start, auto stop, auto step) -> Generator<decltype(stop)> {
  assert(step != 0);
  if (step > 0) {
    for (auto i = start; i < stop; i += step) {
      yield(i);
    }
  } else if (step < 0) {
    for (auto i = start; i > stop; i -= step) {
      yield(i);
    }
  }
}
auto range(auto stop) -> Generator<decltype(stop)> {
  yield_from(range(
      static_cast<decltype(stop)>(0), stop, static_cast<decltype(stop)>(1)));
}

auto range(auto start, auto stop) -> Generator<decltype(stop)> {
  yield_from(range(start, stop, static_cast<decltype(stop)>(1)));
}

auto enumerate(auto&& l, int start = 0)
    -> Generator<decltype(std::make_pair(start, *std::begin(l)))> {
  for (auto v : l)
    yield(std::make_pair(start++, v));
}

// coro fibonacci_sequence(unsigned n) {
auto sg::fibonacci_sequence(auto n) -> Generator<decltype(n)> {
  if (n == 0)
    halt;
  if (n > 94)
    throw std::runtime_error(
        "Too big Fibonacci sequence. Elements would overflow.");
  yield(0);
  if (n == 1)
    halt;
  yield(1);
  if (n == 2)
    halt;
  uint64_t a = 0;
  uint64_t b = 1;
  for (auto i = 2; i < n; i++) {
    uint64_t s = a + b;
    yield(s);
    a = b;
    b = s;
  }
}

////Generator<uint64_t> sg::fibonacci_sequence(unsigned n) {
// decltype(::fibonacci_sequence(0)) sg::fibonacci_sequence(unsigned n) {
////  return std::move(::fibonacci_sequence<uint64_t>(n));
//}

// auto values(auto&& l) -> Generator<decltype(*std::begin(l))> {
//   for (auto v : l)
//     yield(v);
// }

// auto value_type(auto)

// template<typename Container>
// concept ValueType = requires(Container l) {
//   using type = *std::begin(l);
// };

// template<typename Container>
// concept ValueType = decltype(std::begin())

constexpr auto value_type_t(auto&& l) -> std::decay_t<decltype(*std::begin(l))>;
template <template <class...> class C, typename V>
constexpr auto container_type_t(C<V> l) -> C<decltype(value_type_t(l))>;

#include <iterator> // std::back_inserter

template <template <class...> class C = std::vector>
auto make(auto l) -> C<decltype(value_type_t(l))> {
  return {};
}
auto&& append_(auto&& out, auto&& l) {
  if constexpr (sg::is_iterable<decltype(l)>)
    std::copy(std::begin(l), std::end(l), std::back_inserter(out));
  else
    std::back_inserter(out) = l;
  return std::forward<decltype(out)>(out);
}
auto&& append_(auto&& out, auto&& l, auto&&... ls) {
  return append_(append_(out, l), ls...);
}

constexpr bool all_same_2(auto x, auto y) {
  return std::is_same_v<decltype(x), decltype(y)>;
}
constexpr bool all_same(auto x, auto... ys) {
  return all_same_2(x, std::get<0>(ys...)) && all_same(ys...);
}

// template<typename T>
// requires all_same<T, Ts...>
// template<template<class> typename C, typename V>
// auto append(C<std::decay_t<V>> l, decltype(l) x) {
// auto append(auto&& l, decltype(container_type_t(l))&& x) {
#if 0
template<typename C>
auto append(const C& l, const C& x) {
//  static_assert(std::is_same_v<decltype(l), decltype(x)>);
//  static_assert(all_same(l, xs...));
  return append_(append_(make(l), l), x);
}

template<typename C, typename ... Cs>
auto append(const C& l, const C& x, const Cs& ... ys) {
  return append(append(l, x), ys...);
}
#else
// auto append(auto l, auto x) {
//   return append_(append_(make(l), l), x);
// }
auto append(auto&& l, auto&&... ys) { return append_(make(l), l, ys...); }
#endif

#if 0
auto&& add_(auto&& l, auto&& x) {
  auto it = std::back_inserter(l);
  it = x;
  return l;
}
#else
auto&& add_(auto&& l, auto&& x) { return append_(l, x); }
#endif
auto&& add_(auto&& l, auto&& x, auto&&... xs) {
  return add_(add_(l, x), xs...);
}

template <template <class...> class C = std::vector>
auto list(auto l) {
  using V = decltype(value_type_t(l));
//  using V = decltype(*std::begin(l));
#if 0
  using V = decltype(*std::begin(l));
  C<V> out;
  append_(out, l);
  return out;
#elif 0
  auto out(make<C>(l));
  append_(out, l);
  return out;
#elif 0
  return make<C>(l, l.begin(), l.end());
#elif 0
  return append_(make<C>(l), l);
#elif 1
  return append_(C<V>(), l);
#else
  return append_(make(l), l);
#endif
}

template <template <class...> class C, class V>
auto listlike(auto&& l, const C<V>& like) {
  return list<C>(std::forward<decltype(l)>(l));
}

// template<class T>
// auto list(auto&& l) {
//   auto out = make<T>();
//   append_(out, l);
//   return out;
// }

// auto list(auto&& l) -> decltype(auto) {
//   using T = decltype(value_type_t(l));
//   std::vector<T> out;
//   for (auto v : l)
//     out.push_back(v);
//   return out;
// }

// auto list(auto&& l, auto as = std::vector<decltype(value_type_t(l))>()) ->
// decltype(auto) {
//   decltype(as) out;
//   append_(out, l);
//   return out;
// }
template <typename T>
constexpr bool can_print() {
  return false;
}

template <typename T>
requires requires(T t) {
           { std::cout << t };
         }
constexpr bool can_print() {
  using V = std::decay_t<T>;
  return !std::is_pointer_v<V> || std::is_same_v<const char*, V>;
}

template <typename T>
requires requires(T t) {
           { std::begin(t) };
         }
constexpr bool is_list() {
  return true;
}
template <typename T>
constexpr bool is_list() {
  return false;
}

// void disp(const char* x) {
//   std::cout << std::string(x);
// }
// void disp(const std::string& x) {
//   std::cout << x;
// }
void disp_type(auto&& x) { std::cout << "#{" << type_name(x) << "}"; }
void disp(auto&& x) {
  if constexpr (can_print<decltype(x)>()) {
    std::cout << x;
  } else if constexpr (is_list<decltype(x)>()) {
    auto sep = "";
    disp_type(x);
    disp("[");
    for (auto&& v : x) {
      disp(sep);
      disp(v);
      sep = ", ";
    }
    disp("]");
  } else {
    disp_type(x);
  }
}
// template<typename T>
// requires requires(std::decay_t<T> t) { { std::cout << t }; }
// void disp(auto&& x) {
//   std::cout << x;
// }

auto&& pr(auto&& x) {
  disp(x);
  return x;
}
auto&& pr(auto&& x, auto&&... ys) {
  pr(x);
  pr(ys...);
  return x;
}
auto&& prn(auto&&... xs) { return pr(xs..., "\n"); }
auto&& prs(auto&& sep, auto&& x) {
  pr(x);
  return x;
}
auto&& prs(auto&& sep, auto&& x, auto&&... ys) {
  prs(sep, x);
  pr(sep);
  prs(sep, ys...);
  return x;
}
auto&& prc(auto&& x, auto&&... ys) {
  pr("(");
  prs(", ", x, ys...);
  pr(")");
  return x;
}

void sg::testCoroutine() {
#if 1
  try {
    auto gen = fibonacci_sequence(10); // max 94 before uint64_t overflows
    for (int j = 0; gen; j++)
      std::cout << "fib(" << j << ")=" << gen() << '\n';
  } catch (const std::exception& ex) {
    std::cerr << "Exception: " << ex.what() << '\n';
  } catch (...) {
    std::cerr << "Unknown exception.\n";
  }
#endif

  //  auto gen = fibonacci_sequence(10);
  //  for (int j=0;gen;j++) {
  //    auto x = gen();
  //  }
  {
    int j = 0;
    for (auto x : fibonacci_sequence(10)) {
      std::cout << "fib(" << j << ")=" << x << '\n';
      j++;
    }
  }
  {
    for (auto [i, x] : enumerate(fibonacci_sequence(10)))
      std::cout << "enumerate fib(" << i << ")=" << x << '\n';
  }
  //  {
  //    auto l = list_t<std::vector<int>>::make(fibonacci_sequence(5));
  //    std::cout << l.size() << " " << type_name(l) << std::endl;
  //  }
  //  {
  //    auto l = list_t<std::list<int>>::make(fibonacci_sequence(5));
  //    std::cout << l.size() << " " << type_name(l) << std::endl;
  //  }
  //  {
  //    auto l = list2_t<std::vector>::make(fibonacci_sequence(5));
  //    std::cout << l.size() << " " << type_name(l) << std::endl;
  //  }
  //  {
  //    auto l = list2_t<std::list>::make(fibonacci_sequence(5));
  //    std::cout << l.size() << " " << type_name(l) << std::endl;
  //  }
  //  {
  //    auto l = list2::make(fibonacci_sequence(5));
  //    std::cout << l.size() << " " << type_name(l) << std::endl;
  //  }
  {
    auto l = list<std::list>(fibonacci_sequence(5));
    std::cout << l.size() << " " << type_name(l) << std::endl;
  }
  {
    auto l = list(fibonacci_sequence(5));
    std::cout << l.size() << " " << type_name(l) << std::endl;
  }
  if (1) {
    auto l = list(fibonacci_sequence(5));
    auto l2 = listlike(fibonacci_sequence(6), std::list<float>());
    append_(l2, 5.0, 6.0, 7, 8, fibonacci_sequence(10));
    std::cout << "TKTK\n";
    std::cout << l.size() << " " << type_name(l) << std::endl;
    std::cout << l2.size() << " " << type_name(l2) << std::endl;
    for (auto [i, x] : enumerate(l2)) {
      std::cout << "  [" << i << "]: " << x << " (" << type_name(x) << ")\n";
    }
  }
  using V = std::vector<int>;
  {
    std::vector<double> r;
    //    auto l = append(r, decltype(r){1, 2, 3});
    //    using V = decltype(r);
    auto l = append(r, V{1, 2, 3}, V{4, 5, 6}, fibonacci_sequence(5));
    std::cout << l.size() << " " << type_name(l) << std::endl;
    //    for (auto x : l) {
    //      std::cout << " " << x;
    //    }
    //    std::cout << std::endl;
    prc("foo", 42, l, std::map<std::string, int>{{"foo", 42}},
        sg::fibonacci_sequence<decltype(3)>, fibonacci_sequence(3),
        ([=]() -> Generator<float> {
          yield(42.9);
          //        for (auto it : fibonacci_sequence(10))
          //          yield(it);
          yield_from(fibonacci_sequence(10));
        }()));
    prn();
  }
  {
    for (auto n : range(5))
      pr(n);
    prn();
    for (auto n : reversed(range(5)))
      pr(n);
    prn();
    auto l = std::vector<int>{1, 2, 3};
    for (auto n : reversed(l))
      pr(n);
    prn();
    prn(len(l));
    prn(len(reversed(l)));
    prn(len("foobar"));
    prn(len("foobar"s));
    for (auto x : maybes(l)) {
      prn(x);
    }
    //    for (auto x : maybes3(maybes2(maybes(l)))) {
    //    for (auto x : maybes3(maybes(l))) {
    //      prn(x);
    //    }
  }
}