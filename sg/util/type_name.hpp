#pragma once

// https://stackoverflow.com/questions/281818/unmangling-the-result-of-stdtype-infoname

#include <string>
#include <string_view>

namespace sg::type_name_detail {
static std::string replace(
    const std::string& str, const std::string& from, const std::string& to) {
  auto pos = str.find(from);
  if (pos == std::string::npos)
    return str;
  return str.substr(0, pos) + to +
         replace(str.substr(pos + from.size()), from, to);
}
static std::string replace(const std::string& str, const std::string& from,
    const std::string& to, auto... more) {
  return replace(replace(str, from, to), more...);
}
} // namespace sg::type_name_detail

namespace sg {

template <typename T>
constexpr auto get_type_name() -> std::string_view {
#if defined(__clang__)
  constexpr auto prefix = std::string_view{"[T = "};
  constexpr auto suffix = "]";
  constexpr auto function = std::string_view{__PRETTY_FUNCTION__};
#elif defined(__GNUC__)
  constexpr auto prefix = std::string_view{"with T = "};
  constexpr auto suffix = "; ";
  constexpr auto function = std::string_view{__PRETTY_FUNCTION__};
#elif defined(__MSC_VER)
  constexpr auto prefix = std::string_view{"get_type_name<"};
  constexpr auto suffix = ">(void)";
  constexpr auto function = std::string_view{__FUNCSIG__};
#else
#error Unsupported compiler
#endif

  const auto start = function.find(prefix) + prefix.size();
  const auto end = function.find(suffix);
  const auto size = end - start;

  return function.substr(start, size);
}

template <typename T>
std::string type_name() {
  auto out = std::string(get_type_name<T>());
  out = ::sg::type_name_detail::replace(out, "unsigned long long", "u64",

      "unsigned int", "u32",

      "unsigned short", "u16",

      "unsigned char", "byte",

      "long long", "i64",

      "int", "i32",

      "short", "i16",

      "std::", "",

      "double", "f64",

      "float", "f32");
  return out;
}

std::string type_name(auto&& x) {
  return type_name<std::decay_t<decltype(x)>>();
}

} // namespace sg
