

add_library(hexgrid hexgrid.c hexcoords.c interact.c)
target_include_directories(hexgrid PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})