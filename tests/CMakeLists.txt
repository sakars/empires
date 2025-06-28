

add_executable(test_empires 
    test_game_logic.c
    test_runner.c)

set(TEST_EXECUTABLES test_empires)
target_link_libraries(test_empires empires_lib unity ${LIBEVENT_LIBRARIES})
target_include_directories(test_empires PRIVATE ../src ${LIBEVENT_INCLUDE_DIRS})
add_test(NAME EmpiresTests COMMAND test_empires)


add_subdirectory(hex)

