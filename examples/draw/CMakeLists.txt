add_executable(simple_lines "simple_lines.cpp")
target_link_libraries(simple_lines ${PNG_LIBRARIES})

add_executable(filled_polygon "filled_polygon.cpp")
target_link_libraries(filled_polygon ${PNG_LIBRARIES})