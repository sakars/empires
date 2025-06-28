cmake_minimum_required(VERSION 3.10)
project(empires C)

set(CMAKE_C_STANDARD 99)

find_package(PkgConfig REQUIRED)
pkg_check_modules(LIBEVENT REQUIRED libevent)

add_subdirectory(external/Unity)
add_subdirectory(src)
enable_testing()
add_subdirectory(tests)
