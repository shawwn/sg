cmake_minimum_required(VERSION 3.20)
project(sg)

set(CMAKE_CXX_STANDARD 20)

add_executable(sg main.cpp sg/sg.cpp sg/sg.hpp sg/sg.cpp sg/sg.hpp sg/sg.cpp sg/sg.hpp sg/util/type_name.hpp sg/util/string.cpp sg/util/string.hpp sg/util/type_name.cpp sg/util/to_array.cpp sg/util/to_array.hpp sg/util/coroutine.cpp sg/util/coroutine.hpp sg/util/coroutine_magic.hpp sg/util/coroutine_unmagic.hpp sg/util/maybe.cpp sg/util/maybe.hpp sg/util/traits.cpp sg/util/traits.hpp)
