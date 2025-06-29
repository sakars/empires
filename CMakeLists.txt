cmake_minimum_required(VERSION 3.10)
project(empires C)

set(CMAKE_C_STANDARD 99)

find_package(PkgConfig REQUIRED)
pkg_check_modules(LIBEVENT REQUIRED libevent)
find_package(GLUT REQUIRED)
include_directories(${GLUT_INCLUDE_DIRS})
link_libraries(${GLUT_LIBRARIES})

find_package(OpenGL REQUIRED)
include_directories(${OPENGL_INCLUDE_DIRS})
link_libraries(${OPENGL_LIBRARIES} m)
add_definitions(${OPENGL_DEFINITIONS})

add_subdirectory(external/Unity)
add_subdirectory(src)
enable_testing()
add_subdirectory(tests)
