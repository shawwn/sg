cmake_minimum_required(VERSION 3.20)
project(sg)

set(CMAKE_CXX_STANDARD 20)
enable_testing()

find_package(GTest CONFIG REQUIRED)

add_library(sg sg/sg.cpp sg/sg.hpp sg/sg.cpp sg/sg.hpp sg/sg.cpp sg/sg.hpp sg/util/type_name.hpp sg/util/string.cpp sg/util/string.hpp sg/util/type_name.cpp sg/util/to_array.cpp sg/util/to_array.hpp sg/coroutine.hpp sg/coroutine_magic.hpp sg/coroutine_unmagic.hpp sg/util/maybe.cpp sg/util/maybe.hpp sg/util/traits.cpp sg/util/traits.hpp sg/coroutine_test.cpp sg/coroutine_test.hpp)

add_executable(sg_main main.cpp)
target_link_libraries(sg GTest::gmock GTest::gtest)
target_link_libraries(sg_main sg)

add_executable(sg_test sg/sg_test.cpp)

target_link_libraries(sg_test PRIVATE GTest::gmock GTest::gtest GTest::gmock_main GTest::gtest_main sg)
add_test(sg_gtests sg_test)

