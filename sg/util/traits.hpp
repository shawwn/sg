#pragma once

#include <tuple>

// #include "maybe.hpp"
#include <optional>

// template<typename T>
// requires requires(T t) { { *t.begin() }; }
// constexpr bool is_container_type_v() { return true; }
// template<typename T>
// constexpr bool is_container_type_v() { return false; }

#define sg_predicate(name)                                                     \
  template <typename T>                                                        \
  constexpr bool name = requires(T it)

namespace sg::util {
#if 1
sg_predicate(is_iterable) { std::begin(it); };
sg_predicate(is_tuple) { std::get<0>(it); };
sg_predicate(is_pair) {
  it.first;
  it.second;
};

//    template < template <typename...> class Template, typename T >
//    struct is_instantiation_of : std::false_type {};
//
//    template < template <typename...> class Template, typename... Args >
//    struct is_instantiation_of< Template, Template<Args...> > : std::true_type
//    {};

template <template <typename...> class Template, typename T>
struct is_specialization_of : std::false_type {};

template <template <typename...> class Template, typename... Args>
struct is_specialization_of<Template, Template<Args...>> : std::true_type {};

template <template <typename...> class Template, typename... Args>
constexpr bool is_specialization_of_v =
    is_specialization_of<Template, Args...>::value;

//    sg_predicate(has_value_type) { decltype(it)::value_type; };

//    template<typename T, typename = std::enable_if<has_value_type<T>>>
//    auto value_type_f() -> typename T::value_type {};
//    template<typename T, typename = std::enable_if<!has_value_type<T>>>
//    auto value_type_f() -> T {};
//    template<typename T>
//    requires requires { typename T::value_type; }
//    using value_type_of = typename T::value_type;
//    template<typename T>
//    using value_type_of = T;

//    template<typename T>
//    requires requires { typename T::value_type; }
//    typename T::value_type value_type_f() { };
//    template<typename T>
//    T value_type_f() {};

//    template<typename T>
//    struct value_type {
//        template<typename V, typename = std::enable_if<has_value_type<V>>>
//        using type = typename V::value_type;
//        template<typename V, typename>
//        using type = V;
//    };
//    template<typename T>
//    using value_type_v = decltype(value_type_f<T>());
//    using value_type_v = typename value_type<T>::type;

//    using value_type_v = constexpr has_value_type<T> ? T;
//    template<typename T>
//    requires requires(T it) { T::value_type; }
//    using value_type_v = T::value_type;

//  template<typename T, typename = std::enable_if<has_value_type<T>>>
//    requires has_value_type<T>
//  struct as_value_type {
//      using type = typename T::value_type;
//  };
//
//  template<typename T> struct as_value_type<T,
//  std::enable_if<!has_value_type<T>>> {
//      using type = T;
//  };

//  template<typename T>
//  using as_value_type_v = typename as_value_type<T>::type;

template <bool, class Tp = void>
struct enable_if {};
template <class Tp>
struct enable_if<true, Tp> {
  typedef Tp type;
};

//    template <bool, typename T> struct _LIBCPP_TEMPLATE_VIS val_type_v { };
//    template <typename V> struct _LIBCPP_TEMPLATE_VIS
//    val_type_v<has_value_type<V>, void> { typedef T type; }; template <bool,
//    typename T, class Tp = typename T::value_type> struct _LIBCPP_TEMPLATE_VIS
//    val_type_v { typedef Tp type; }; template <class Tp> struct
//    _LIBCPP_TEMPLATE_VIS enable_if<true, Tp> {typedef Tp type;};

// https://stackoverflow.com/questions/22486386/partially-specializing-a-non-type-template-parameter-of-dependent-type
template <typename T, T N, typename = void>
struct X {
  static const bool isZero = false;
};

template <typename T, T N>
struct X<T, N, typename std::enable_if<N == 0>::type> {
  static const bool isZero = true;
};

/*
int main(int argc, char* argv[]) {
std::cout << X <int, 0>::isZero << std::endl;
std::cout << X <int, 1>::isZero << std::endl;
return 0;
}
 */

// https://devblogs.microsoft.com/oldnewthing/20190710-00/?p=102678

template <typename, typename = void>
constexpr bool is_type_complete_v = false;

template <typename T>
constexpr bool is_type_complete_v<T, std::void_t<decltype(sizeof(T))>> = true;

template <typename T>
using enable_if_exists = typename std::enable_if<is_type_complete_v<T>>::type;

template <typename T, typename = void>
struct atom_type {
  using type = T;
};
template <typename T>
struct atom_type<T, enable_if_exists<typename T::value_type>> {
  using type = typename T::value_type;
};

template <typename T>
using as_value_type_v = typename atom_type<T>::type;

//    template<typename T, typename = std::enable_if_t<has_value_type<T>>>
//  using as_value_type_v = typename T::value_type;
//  template<typename T, typename std::enable_if_t<!has_value_type<T>>>
//  using as_value_type_v = typename T::value_type;

//  template<typename T, decltype(std::enable_if<!has_value_type<T>>::value)>
//  using as_value_type_v = typename T::value_type;

template <template <typename...> class Template, typename T>
constexpr bool is_template_of_v() {
  return false;
}

template <template <typename...> class Template, typename... Args>
//    requires requires (Template<Args...> it) { {}; }
constexpr bool is_template_of_v() {
  static_assert(std::declval<Template<Args...>>());
  return true;
}

template <typename T>
constexpr bool is_optional_v = is_specialization_of_v<std::optional, T>;
//    template<typename T>
//    constexpr bool is_maybe_v() { return false; }
//    template<typename T>
//    constexpr bool is_maybe_v() { return false; }
//
//    template<typename T>
//    requires requires(T t) { { *t.begin() }; }
//    constexpr bool is_maybe_v() { return true; }
//    template<typename T>
//    constexpr bool is_maybe_v() { return false; }

//    optional(const optional<_Up>& __v)

//    template<typename T>
//    constexpr auto maybe_t(T it) -> decltype(maybe<)
#elif 1
template <typename T>
constexpr bool is_iterable = requires(T t) { std::begin(t); };

template <typename T>
constexpr bool is_tuple = requires(T t) { std::get<0>(t); };
template <typename T>
constexpr bool is_pair = requires(T t) {
                           t.first;
                           t.second;
                         };
#endif

// https://en.cppreference.com/w/cpp/concepts/derived_from
template <class Derived, class Base>
concept derived_from =
    std::is_base_of_v<Base, Derived> &&
    std::is_convertible_v<const volatile Derived*, const volatile Base*>;

} // namespace sg::util