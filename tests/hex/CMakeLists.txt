

add_executable(test_hex test_hexcoords.c)
target_link_libraries(test_hex hexgrid unity)

add_test(NAME HexTests COMMAND test_hex)