

add_subdirectory(hex)

add_library(empires_lib hexdrawer.c)

target_link_libraries(empires_lib PUBLIC hexgrid)

add_executable(empires main.c)
target_link_libraries(empires empires_lib ${GLUT_LIBRARIES} ${OPENGL_LIBRARIES} ${LIBEVENT_LIBRARIES})
target_include_directories(empires PRIVATE ${GLUT_INCLUDE_DIRS} ${OPENGL_INCLUDE_DIRS} ${LIBEVENT_INCLUDE_DIRS})

