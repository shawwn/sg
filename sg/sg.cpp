#include "sg.hpp"

#include <iostream>
#include <tuple>

#include <vector>
#include <tuple>
#include <utility>
#include <string>
#include <unordered_map>

using namespace sg::cpp;

// https://stackoverflow.com/questions/1872220/is-it-possible-to-iterate-over-arguments-in-variadic-macros

#define STRINGIZE(arg)  STRINGIZE1(arg)
#define STRINGIZE1(arg) STRINGIZE2(arg)
#define STRINGIZE2(arg) #arg

#define CONCATENATE(arg1, arg2)   CONCATENATE1(arg1, arg2)
#define CONCATENATE1(arg1, arg2)  CONCATENATE2(arg1, arg2)
#define CONCATENATE2(arg1, arg2)  arg1##arg2


#define FOR_EACH_0(what, ...) __VA_ARGS__

#define FOR_EACH_1(what, x, ...) what(x)
#define FOR_EACH_2(what, x, ...)\
  what(x),\
  FOR_EACH_1(what,  __VA_ARGS__)
#define FOR_EACH_3(what, x, ...)\
  what(x),\
  FOR_EACH_2(what, __VA_ARGS__)
#define FOR_EACH_4(what, x, ...)\
  what(x),\
  FOR_EACH_3(what,  __VA_ARGS__)
#define FOR_EACH_5(what, x, ...)\
  what(x),\
 FOR_EACH_4(what,  __VA_ARGS__)
#define FOR_EACH_6(what, x, ...)\
  what(x),\
  FOR_EACH_5(what,  __VA_ARGS__)
#define FOR_EACH_7(what, x, ...)\
  what(x),\
  FOR_EACH_6(what,  __VA_ARGS__)
#define FOR_EACH_8(what, x, ...)\
  what(x),\
  FOR_EACH_7(what,  __VA_ARGS__)

#define FOR_EACH_NARG(...) FOR_EACH_NARG_(__VA_ARGS__, FOR_EACH_RSEQ_N())
#define FOR_EACH_NARG_(...) FOR_EACH_ARG_N(__VA_ARGS__)
#define FOR_EACH_ARG_N(_1, _2, _3, _4, _5, _6, _7, _8, N, ...) N
#define FOR_EACH_RSEQ_N() 8, 7, 6, 5, 4, 3, 2, 1, 0

#define FOR_EACH_(N, what, ...) CONCATENATE(FOR_EACH_, N)(what, __VA_ARGS__)
#define FOR_EACH(what, ...) FOR_EACH_(FOR_EACH_NARG(__VA_ARGS__), what, __VA_ARGS__)

/*
#define PRN_STRUCT_OFFSETS_(structure, field) printf(STRINGIZE(structure)":"STRINGIZE(field)" - offset = %d\n", offsetof(structure, field));
#define PRN_STRUCT_OFFSETS(field) PRN_STRUCT_OFFSETS_(struct a, field)

int main(int argc, char *argv[])
{
  FOR_EACH(PRN_STRUCT_OFFSETS, a, b, c);
  printf("\n");

  return 0;
}
*/


// https://stackoverflow.com/questions/14365837/reverse-the-arguments-to-a-variadic-macro

#define EXPAND(x) x

#define REVERSE_1(a) a
#define REVERSE_2(a,b) b,a
#define REVERSE_3(a,...) EXPAND(REVERSE_2(__VA_ARGS__)),a
#define REVERSE_4(a,...) EXPAND(REVERSE_3(__VA_ARGS__)),a
#define REVERSE_5(a,...) EXPAND(REVERSE_4(__VA_ARGS__)),a
#define REVERSE_6(a,...) EXPAND(REVERSE_5(__VA_ARGS__)),a
#define REVERSE_7(a,...) EXPAND(REVERSE_6(__VA_ARGS__)),a
#define REVERSE_8(a,...) EXPAND(REVERSE_7(__VA_ARGS__)),a
#define REVERSE_9(a,...) EXPAND(REVERSE_8(__VA_ARGS__)),a
#define REVERSE_10(a,...) EXPAND(REVERSE_9(__VA_ARGS__)),a
#define REVERSE1(N,...) EXPAND(REVERSE_ ## N(__VA_ARGS__))
#define REVERSE_N(N, ...) REVERSE1(N, __VA_ARGS__)

#define _GET_NTH_ARG(_1, _2, _3, _4, _5, _6, _7, _9, _10, N, ...) N
#define NUM_ARGS(...) EXPAND(_GET_NTH_ARG(__VA_ARGS__,9,8,7,6,5,4,3,2,1,0))
#define REVERSE(...) REVERSE_N(NUM_ARGS(__VA_ARGS__), __VA_ARGS__)

#define HD(x, ...) x
#define TL(x, ...) __VA_ARGS__

#define LAST1(...) HD(__VA_ARGS__)
#define LAST(...) LAST1(REVERSE(__VA_ARGS__))

#define ALMOST2(...) REVERSE(__VA_ARGS__)
#define ALMOST1(...) ALMOST2(TL(__VA_ARGS__))
#define ALMOST(...) ALMOST1(REVERSE(__VA_ARGS__))

#define BODY_1(last, ...) __VA_ARGS__ ; return last;
#define BODY(...) BODY_1(LAST(__VA_ARGS__), ALMOST(__VA_ARGS__))

#define FN_ARG(x) auto x
// #define FN_ARGS(...) (__VA_ARGS__)
#define FN_ARGS(...) (FOR_EACH(FN_ARG, __VA_ARGS__))
#define ARGS(...) FOR_EACH(FN_ARG, __VA_ARGS__)

#define FN(args, ...) [=] FN_ARGS args { BODY(__VA_ARGS__); }
// #define FUNC(...) ([=](auto _) { BODY(__VA_ARGS__); })
#define FUNC(...) FN((_), __VA_ARGS__)
// #define WRAP(f) FUNC(f(_))
#define WRAP(f) ([] (auto _) { return f(_); })
#define FN1(args, body) ([=] args body)

#define LAMBDA_1(body, ...) FN((__VA_ARGS__), body)
#define LAMBDA(...) LAMBDA_1(LAST(__VA_ARGS__), ALMOST(__VA_ARGS__))

#define THUNK(...) ([=]() __VA_ARGS__)
#define DO(...) THUNK(__VA_ARGS__)()

// template<typename Arg, typename ...Args>
// Arg print(Arg arg, Args&&... args)
// {
//   std::cout << arg;
//   (std::cout << ... << args) << "\n";
//   std::cout.flush();
//   return std::move(arg);
// }

#include <iomanip>

template < template <typename...> class Template, typename T >
struct is_instantiation_of : std::false_type {};

template < template <typename...> class Template, typename... Args >
struct is_instantiation_of< Template, Template<Args...> > : std::true_type {};


void display(auto x);

auto literal(const char* x) {
  display(std::quoted(x));
}
auto literal(const std::string& x) {
  display(std::quoted(x));
}
auto literal(const std::string_view& x) {
  display(std::quoted(x));
}
auto literal(auto x) {
  display(x);
}

void display_seq(auto seq, auto sep = ", ") {
  size_t i = 0;
  for (auto v : seq) {
    if (i++)
      display(sep);
    literal(v);
  }
}

void display(const char* x) {
  std::cout << x;
}
void display(const std::string_view& x) {
  std::cout << x;
}
void display(const std::string& x) {
  display(x.c_str());
}
void display(decltype(nil)) {
  display("nil");
}
template<typename T>
//requires requires { !std::is_same_v<maybe<T>::in_place_t, T>; }
requires is_maybe_v<T>
auto display_maybe(T x) {
  if (x) {
    auto v = *x;
    display(v);
  } else {
    display(nil);
  }
}

void display(auto x) {
//  if constexpr(is_maybe<decltype(x)>) {
  using T = Bare<decltype(x)>;
//  maybe<T> v;
//  if constexpr (std::is_same_v<Bare<decltype(v)>, Bare<T>>) {
//  if constexpr (requires { display_maybe(x); })
//    display_maybe<T>(x);
//  if constexpr (!std::is_convertible_v<T, maybe<T>>)
  if constexpr (is_iterable<T>) {
    using iterator_type = decltype(std::begin(x));
    using value_type = decltype(*std::declval<iterator_type>());
    if constexpr (is_pair<value_type>) {
      display("{");
      display_seq(x, ", ");
      display("}");
    } else {
      display("[");
      display_seq(x, ", ");
      display("]");
    }
  }
  else if constexpr (is_pair<T>) {
    // display("[");
    literal(x.first);
    // display("]: ");
    display(": ");
    literal(x.second);
  }
  else if constexpr (is_tuple<decltype(x)>) {
    display("(");
    display_seq(to_array(x), ", ");
    display(")");
  } else {
    std::cout << x;
  }
}

void display(auto x, auto ... xs) {
  display(x);
  display(xs...);
}

auto print(auto x, auto ... xs) {
  display(x, xs..., "\n");
  return x;
}

// /**
//  * Literal class type that wraps a constant expression string.
//  *
//  * Uses implicit conversion to allow templates to *seemingly* accept constant strings.
//  */
// template<size_t N>
// struct StringLiteral {
//     constexpr StringLiteral(const char (&str)[N]) {
//       std::copy_n(str, N, value);
//     }

//     operator std::string() const { return value; }

//     char value[N];
// };

// template<StringLiteral open = "", StringLiteral sep = " ", StringLiteral close = "\n">
// auto print(auto arg, auto ... args)
// {
//   std::cout << open;
//   std::cout << arg;
//   for (auto x : sg::to_array(args))
//     std::cout << sep << x;
//   std::cout << close;
//   std::cout.flush();
//   return arg;
// }

// auto do1(auto x) { return x; }

// auto do1(auto x, auto ... ys) {
//   return x;
// }




auto list1(auto x, auto ... args) {
  std::vector<decltype(x)> out;
  out.emplace_back(x);
  for (auto arg : sg::to_array(args...)) {
    out.emplace_back(arg);
  }
  return out;
}



auto&& update(auto&& out, auto&& l) {
  if constexpr (is_iterable<decltype(l)>)
    std::copy(std::begin(l), std::end(l), std::back_inserter(out));
  else
    std::back_inserter(out) = l;
  return std::forward<decltype(out)>(out);
}


// auto&& update(auto&& out, auto&& l) {
//   std::copy(std::begin(l), std::end(l), std::back_inserter(out));
//   return std::forward<decltype(out)>(out);
// }

auto&& update(auto&& out, auto&& x, auto&& ... ys) {
  return update(update(out, x), ys...);
}

auto tuple(auto ... xs) {
  return std::make_tuple(xs...);
}

auto list(auto x) {
  return std::vector<decltype(x)>{x};
}

auto list(auto x, auto ... args) {
  return std::vector<decltype(x)>{x, args...};
}

auto append(auto l, auto ... ls) {
  return update(decltype(l){}, l, ls...);
}

auto map(auto f, auto l) {
  std::vector<decltype(f(*std::begin(l)))> out;
  for (auto x : l)
    out.emplace_back(f(x));
  return out;
}

#define MAP(f, l) map(FN((_), f(_)), l)

#define each(x, l, ...) for (auto x : l) { __VA_ARGS__; }

auto prn = WRAP(print);

void sg::test_sg() {
  print("Hello, world!");
  print(NUM_ARGS(foo,bar,baz));
  print(REVERSE(0,1,2));
  print("HD ", HD(0,1,2,3,4));
  print("TL ", TL(0,1,2,3,4));
  print(ALMOST(0,1,2,3,4));
  print("LAST ", LAST(0,1,2,3,4));
  print("DO", ([=]() { BODY((void)1,(void)2,(void)3,4); })());
  // print("FN", FN((), 1+2)());
  print("FN", FN((a, b), a+b)(1,2));
  // print("FN", FN((FN_ARG(a), FN_ARG(b)), a+b)(1,2));
  print("DO", DO({ print(1,2,3,4); return ""; }));
  // print("DO", DO( -> void { print(1,2,3,4); }));
  print("LAMBDA", LAMBDA(a, b, a+b)(1,2));
  // print("LAMBDA", ([=] (FOR_EACH(FN_ARG, a, b)) { return a+b; })(1,2));
  // print("FN", FN((), 1,2,3,{ print("stmt"); },4)());
  // FN((), print("hi"))();
  print("hi");
  FN1((),{ print("hi"); })();
  FN((a), print(a))(1);
  FN((a), a)(1);
  FN((a, b), a+b)(1,2);
  FN((x, y, z), print(x),print(y), print(z), x+y+z)(1,2,3);
  map(FN((_), print(_)), list(1,2,3));
  map(FUNC(print(_)), list(1,2,3));
  MAP(print, list(1,2,3));
  // map(FUNC(_), list(list(1,2)));
  map(FUNC(_), list(1,2,3));
  auto zz = list(list(1,2,3));
  update(zz[0], list(6,7,8));
  // auto prn = WRAP(print);
  prn("===");
  map(LAMBDA(_, map(prn, _)), zz);
  prn("===");
  map(FUNC(map(prn, _)), zz);
  prn("===");
  map(FUNC(map(prn, _)), zz);
  prn("===");
  map(FUNC(map(prn, _)), zz);
  // MAP(MAP(print, _), zz);
  prn("===");
  print(append(list(1,2,3), list(4,5,6)).size());
  prn("===");
  print(zz.size());
  prn("===");
  each(x, list(1,2,3), prn(x));
  prn("~0~");
// each([x, y], list(list(1,2)), (void)x);
  for (auto x : list(list(1,2))) {
    auto [a, b] = tuple(x[0], x[1]);
    print(a, b);
  }
  prn("~1~");
  for(auto x : list(1,2)) {
    auto&& [a] = tuple(x);
    prn(a);
  }
  prn("~2~");
  for(auto [a, b] : list(tuple(1, 2))) {
    print(a, b);
  }
  prn(list(list(1,2,3)));
  prn(tuple(1,2,3));
  {
    std::unordered_map<std::string, std::string> u = {
            {"RED","#FF0000"},
            {"GREEN","#00FF00"},
            {"BLUE","#0000FF"}
    };
    prn(u);
  }
  {
    std::unordered_map<int, std::string> u = {
            {1,"#FF0000"},
            {2,"#00FF00"},
            {3,"#0000FF"}
    };
    prn(u);
  }
  {
    std::unordered_map<int, decltype(list("foo"))> u = {
            {1,list("#FF0000")},
            {2,list("#00FF00")},
            {3,list("#0000FF")}
    };
    prn(u);
  }
  prn(nil);
//  auto m = maybe<int>(1);
  int not_maybe = 1;
  maybe<int> m(1);
//  display_maybe(m);
  prn("");
  auto m2 = std::make_optional(m);
  prn(sg::type_name(m2));
  prn(sg::get_type_name<std::decay_t<decltype(m2)>>());
//  if constexpr(is_instantiation_of<maybe, decltype(m)>) {
//  if constexpr(is_template_of_v<maybe, decltype(m)>()) {
//  if constexpr(is_maybe<decltype(m)>) {
  prn(sg::type_name(is_specialization_of<std::optional, decltype(m)>()));
  prn(is_specialization_of_v<std::optional, decltype(m)>);
  prn(is_specialization_of_v<std::optional, decltype(not_maybe)>);
  prn(is_specialization_of_v<maybe, decltype(m)>);
  prn(is_specialization_of_v<maybe, decltype(not_maybe)>);
  print("std::is_same_v<maybe<decltype(m)::value_type>, decltype(m)> = ",
        std::is_same_v<maybe<decltype(m)::value_type>, decltype(m)>);
  print("std::is_same_v<maybe<decltype(m2)::value_type>, decltype(m2)> = ",
        std::is_same_v<maybe<decltype(m2)::value_type>, decltype(m2)>);
//  print("std::is_same_v<maybe<decltype(not_maybe)::value_type>, decltype(not_maybe)> = ",
//        std::is_same_v<maybe<decltype(not_maybe)::value_type>, decltype(not_maybe)>);
  display("std::is_same_v<maybe<as_value_type_v<decltype(not_maybe)>>, decltype(not_maybe)> = ");
  prn(std::is_same_v<maybe<as_value_type_v<decltype(not_maybe)>>, decltype(not_maybe)>);
  display("std::is_same_v<maybe<as_value_type_v<decltype(m)>>, decltype(m)> = ");
  prn(std::is_same_v<maybe<as_value_type_v<decltype(m)>>, decltype(m)>);
//  prn(std::is_same_v<maybe<decltype(value_type_f<decltype(m)>())>, decltype(m)>);
//  prn(sg::type_name(value_type_f<decltype(m)>()));
  prn(sg::get_type_name<as_value_type_v<decltype(m)>>());
//  prn(sg::get_type_name<as_value_type<decltype(m)>::type>());
  prn(sg::get_type_name<decltype(m)::value_type>());

  prn(sg::get_type_name<atom_type<decltype(m)>::type>());// fails?!
  prn("fails?");
  prn(sg::get_type_name<as_value_type_v<decltype(m)>>());

  prn(is_maybe_v<decltype(m)>);
  prn(is_maybe_v<decltype(not_maybe)>);
  prn(sg::get_type_name<as_value_type_v<decltype(m)>>());
  prn(sg::get_type_name<as_value_type_v<decltype(not_maybe)>>());
//  prn(has_value_type<decltype(m)>);
//  prn(has_value_type<decltype(not_maybe)>);
//    prn("maybe");
//  }
//  if constexpr (is_maybe<decltype(m)>) {
//    prn("maybe");
//  }
//  auto lst = std::vector<maybe<int>>{1,2,3,nil,4};
//  prn(lst);
}