#include <iostream>

#include "sg/coroutine_test.hpp"
#include "sg/sg.hpp"

#define stdMov(x) std::move(x)
#define stdFwd(x) std::forward<decltype(x)>(x)

template <class T>
using NoRef = typename std::remove_reference<T>::type;

auto& idfn(auto& x) { return stdFwd(x); }
auto&& idfn(auto&& x) { return stdFwd(x); }

// template <typename Arg, typename... Args>
// const Arg& print(const Arg& arg, const Args&... args) {
//   std::cout << arg;
//   (std::cout << ... << args) << "\n";
//   std::cout.flush();
//   return stdFwd(arg);
// }
//
// template <typename Arg, typename... Args>
// const Arg&& print(const Arg&& arg, const Args&&... args) {
//   std::cout << arg;
//   (std::cout << ... << args) << "\n";
//   std::cout.flush();
//   return stdFwd(arg);
// }

// template <typename Arg, typename... Args>
// NoRef<Arg>&& print(NoRef<Arg>&& arg, NoRef<Args>&&... args) {
//   std::cout << arg;
//   (std::cout << ... << args) << "\n";
//   std::cout.flush();
//   return stdFwd(arg);
// }

template <class T>
struct NoisyCopy;

// template<typename T>
// std::ostream& operator<<(std::ostream& stream, NoRef<NoisyCopy<T>>&& rv);
// template<typename T>
// std::ostream& operator<<(std::ostream& stream, const NoRef<NoisyCopy<T>>&
// rv);

auto& last(auto& arg) { return stdFwd(arg); }
auto& last(auto& arg, auto&... args) { return last(stdFwd(args)...); }
auto&& last(auto&& arg) { return stdFwd(arg); }
auto&& last(auto&& arg, auto&&... args) { return last(stdFwd(args)...); }

auto& display(auto& arg, auto&... args) {
  std::cout << arg;
  (void)(std::cout << ... << args);
  std::cout.flush();
  return stdFwd(arg);
}

auto&& display(auto&& arg, auto&&... args) {
  std::cout << arg;
  (void)(std::cout << ... << args);
  std::cout.flush();
  return stdFwd(arg);
}

void print() { display("\n"); }
auto& print(auto&... args) {
  display(stdFwd(args)..., "\n");
  return last(stdFwd(args)...);
}
auto&& print(auto&&... args) {
  display(stdFwd(args)..., "\n");
  return last(stdFwd(args)...);
}

// template <typename Arg, typename... Args>
// const Arg& print(const Arg& arg, const Args&... args);
// template <typename Arg, typename... Args>
// const Arg&& print(const Arg&& arg, const Args&&... args);

#include <concepts>

// template <template<typename...> typename Template, typename T>
// concept NonNoCopy = not sg::is_specialization_of_v<NoCopy, Template<T>>;

template <template <typename...> typename Template, typename T>
requires(not sg::is_specialization_of_v<Template, T>)
struct NotSpecialization_s {};

template <template <typename...> typename Template, typename T>
concept NotSpecialization = (not sg::is_specialization_of_v<Template, T>);

template <template <typename...> typename Template, typename T>
requires NotSpecialization<Template, T>
using NotSpecialization_t = Template<T>;

// template<typename T>
// struct NoCopy;
//
// template<typename T>
// struct NoCopy_detail {
//   using Foo = NoCopy<T>;
////  typedef NoCopy<T> Foo;
////  template<typename V>
////  using
//};
//
// struct NoCopy_detail2 {
//  template<typename T>
//  using Foo = NoCopy<T>;
////  typedef NoCopy<T> Foo;
////  template<typename V>
////  using
//};
//
// template <typename T>
// requires (not sg::is_specialization_of_v<NoCopy, T>)
// struct NotNoCopy {};

// template<typename T>
// struct NoCopy;

template <typename T>
// requires (not requires { NotSpecialization<T::template Self_t, T>; })
// requires (not requires { T::is_NoCopy; })
// requires ((sg::is_type_complete_v<typename T::invalid> and not T::invalid())
// or not T::invalid) requires (not requires { T::invalid == false; }) requires
// (not requires { sg::is_specialization_of_v<typename T::Self, T>; }) requires
// (not sg::is_specialization_of_v<NoCopy_detail2::Foo, T>) requires
// NonNoCopy<NoCopy, T> requires NonNoCopy<NoCopy, T> struct NoCopy :
// NotNoCopy<T>
struct NoCopy //: NotSpecialization_s<NoCopy, T>
{
  //  static constexpr bool invalid() { return false; }
  //  static constexpr bool invalid = sg::is_specialization_of_v<NoCopy, T>;
  //  static constexpr bool is_NoCopy = true;
  //  template<typename V = T>
  //  static constexpr bool is_NoCopy = sg::is_specialization_of_v<NoCopy, V>;
  using Self = NoCopy<T>;

  //  typedef NoCopy<T> Self_v;

#if CRASHES_LLVM
  template <typename...>
  requires(not sg::is_specialization_of_v<NoCopy, T>)
  using Self_t = NoCopy<T>;
  //  struct Self_t : NoCopy<T> { typedef NoCopy<T> Self; };
  using Self = Self_t<>;
#endif

  //  template<typename V = T>
  //  requires (not sg::is_specialization_of_v<NoCopy, V>)
  //  using value_type = V;
  typedef NotSpecialization_t<NoCopy, T> NotNoCopy;
  using value_type = T;

  //  template<typename V>
  //  requires NotSpecialization<NoCopy, V>
  //  using value_type_t = V;
  //  using value_type = value_type_t<T>;

  //  static constexpr bool verify = sg::is_specialization_of_v<NoCopy, V>;
  //  constexpr bool verify = true;
  //  using verify_t = verify<typename T>;
  //  using value_type_t = NoCopy<V>;
  //  template<typename V>
  //  requires NotSpecialization<NoCopy, V>
  //  using value_type_t = NoCopy<V>;
  //  using value_type = value_type_t<T>;
  //  static constexpr bool verify = true;
  //  using value_type = typename T::value_type;
  //  using value_type = T;
  //  static_assert(not sg::is_specialization_of_v<NoCopy, value_type>);
  T value;
  NoCopy(T&& value) : value(stdFwd(value)) {}
  NoCopy(const T& value) : value(stdFwd(value)) {}
  NoCopy(NoCopy&& rv) noexcept : NoCopy(stdFwd(rv.value)) {}
  NoCopy(const NoCopy& lv) = delete;
  void operator=(auto& rhs) = delete;
  void operator=(auto&& rhs) = delete;

  friend std::ostream& operator<<(std::ostream& stream, NoCopy&& rv) {
    stream << "NoCopy(.value=" << rv.value << ")";
    return stdFwd(stream);
  }
  friend std::ostream& operator<<(std::ostream& stream, const NoCopy& rv) {
    stream << "NoCopy(.value=" << rv.value << ")";
    return stdFwd(stream);
  }
};

#if 0
template<typename T>
requires (not sg::is_specialization_of_v<detail_NoCopy, T>)
using NoCopy = detail_NoCopy<T>;
#endif

#if 0
inline constexpr auto forward(auto& lv) noexcept -> decltype(std::forward<decltype(lv)>(lv)) {
  print("forward lvalue: ", static_cast<void*>(&lv));
  return std::forward<decltype(lv)>(lv);
}
inline constexpr auto forward(auto&& rv) noexcept -> decltype(std::forward<decltype(rv)>(rv)) {
  print("forward rvalue: ", static_cast<void*>(&rv));
  return std::forward<decltype(rv)>(rv);
}
#elif 0
template <typename T>
inline constexpr auto forward(std::remove_reference_t<T>& lv) noexcept
    -> decltype(std::forward<decltype(lv)>(lv)) {
  print("forward lvalue: ", static_cast<void*>(&lv));
  return std::forward<decltype(lv)>(lv);
}
template <typename T>
inline constexpr auto forward(std::remove_reference_t<T>&& rv) noexcept
    -> decltype(std::forward<decltype(rv)>(rv)) {
  print("forward rvalue: ", static_cast<void*>(&rv));
  return std::forward<decltype(rv)>(rv);
}
#elif 0
template <typename T>
inline constexpr T&& forward(std::remove_reference_t<T>& lv) noexcept {
  print("forward lvalue: ", static_cast<void*>(&lv));
  return std::forward<decltype(lv)>(lv);
}
template <typename T>
inline constexpr T&& forward(std::remove_reference_t<T>&& rv) noexcept {
  print("forward rvalue: ", static_cast<void*>(&rv));
  return std::forward<decltype(rv)>(rv);
}
#elif 1
template <typename T>
inline constexpr T&& forward(std::remove_reference_t<T>& lv) noexcept {
  print("forward lvalue: ", static_cast<void*>(&lv));
  return std::forward<decltype(lv)>(lv);
}
template <typename T>
inline constexpr T&& forward(std::remove_reference_t<T>&& rv) noexcept {
  print("forward rvalue: ", static_cast<void*>(&rv));
  return std::forward<decltype(rv)>(rv);
}

#else
template <class T>
inline _LIBCPP_INLINE_VISIBILITY constexpr T&&
forward(typename std::remove_reference<T>::type& t) noexcept {
  print("forward lvalue: ", static_cast<void*>(&t));
  return static_cast<T&&>(t);
}

template <class T>
inline _LIBCPP_INLINE_VISIBILITY constexpr T&&
forward(typename std::remove_reference<T>::type&& t) noexcept {
  print("forward rvalue: ", static_cast<void*>(&t));
  static_assert(!std::is_lvalue_reference<T>::value,
      "can not forward an rvalue as an lvalue");
  return static_cast<T&&>(t);
}
#endif

template <typename T>
struct NoisyCopy {
  T value;
  friend std::ostream& operator<<(std::ostream& stream, NoisyCopy&& rv) {
    stream << "NoisyCopy(.value=" << rv.value << ")";
    return stdFwd(stream);
  }
  friend std::ostream& operator<<(std::ostream& stream, const NoisyCopy& rv) {
    stream << "NoisyCopy(.value=" << rv.value << ")";
    return stdFwd(stream);
  }

  NoisyCopy() { print(*this); }
  NoisyCopy(T&& rv) : value(stdFwd(rv)) { print("(T&& rv); *this == ", *this); }
  NoisyCopy(const T& lv) : value(stdFwd(lv)) {
    print("(const T& lv); *this == ", *this);
  }
  NoisyCopy(const NoisyCopy<T>& lv) : value(stdMov(lv.value)) {
    print("(const NoisyCopy<T>& lv); *this == ", *this);
  }

  NoisyCopy& operator=(const NoisyCopy& lv) noexcept {
    print(*this, " operator=(const NoisyCopy& lv); lv == ", lv);
    value = stdMov(lv.value);
    return *this;
  }

  NoisyCopy& operator=(NoisyCopy<T>&& rv) noexcept {
    print(*this, " operator=(NoisyCopy<T>&& rv); rv == ", rv);
    value = stdMov(rv.value);
    return *this;
  }
};

int main() {
  std::cout << std::boolalpha;
  sg::testCoroutine();
  //  std::cout << "Hello, World!" << std::endl;
  sg::test_sg();

  // error: Cannot take the address of an rvalue of type 'int'
  // print("address: ", static_cast<void*>(&forward(1)));
  forward<int>(1);
  int x = 1;
//  forward<int&>(x);
//  forward<int&>(x);
#if 0
  forward(x);
  forward(42);
  print("address: ", static_cast<void*>(&forward<int&>(x)));
  print("address: ", static_cast<void*>(&forward<int&>(x)));
  // print("address: ", static_cast<void*>(&std::move(x)));
#endif
  print("foo", "bar");

  print();
  print("# NoisyCopy");
  {
    auto a0 = print(">>> auto a0 = NoisyCopy<int>(0); ", NoisyCopy<int>(0));
    print();
    int zero = 0;
    print(">>> int zero = 0;");
    auto b0 = print(">>> auto b0 = NoisyCopy{zero}; ", NoisyCopy{zero});
    print();
    auto a1 = print(">>> auto a1 = NoisyCopy<int>{1}; ", NoisyCopy<int>{1});
    print();
    auto a2 = print(">>> auto a2 = a1; ", a1);
    print();
    auto a3 = print(">>> auto a3 = decltype(a2); ", decltype(a2)(a2));
    print();
    auto a4 = print(">>> auto a4 = NoisyCopy<int>(a3); ", NoisyCopy<int>(a3));
    print();
    a4 = print(">>> a4 = a3; ", a3);
    print();
  }

  print();
  print("# NoCopy");
  {
    int zero = 0;
    auto a0 = print(">>> auto a0 = ", NoisyCopy{zero});
    print(sg::type_name(a0), " a0;");
    auto a1 = print(">>> auto a1 = ", NoCopy{zero});
    decltype(a1)::Self foo{zero};
    print(sg::type_name(a1), " a1;");
    auto b0 = print(">>> auto b0 = ", NoCopy{a0});
    print(sg::type_name(b0), " b0;");
    //    auto c0(print( ">>> auto c0 = ", NoCopy<decltype(b0)>{stdFwd(b0)}));
    //    print(sg::type_name(c0), " c0;");
    b0.value = 42;
    b0.value = 99;
    print("b0 == ", b0);
    //    print("c0 == ", c0);
    auto d0 = idfn(NoCopy<decltype(a0)>{a0});
    //    NoCopy<NoCopy<int>> fails;
  }
  print();
  return 0;
}
