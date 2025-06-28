

add_subdirectory(hex)

add_library(empires_lib game_logic.c)

target_link_libraries(empires_lib PUBLIC hexgrid)


# target_link_libraries(empires ${LIBEVENT_LIBRARIES} hex)
# target_include_directories(empires PRIVATE ${LIBEVENT_INCLUDE_DIRS})
