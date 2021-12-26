add_executable(colorwheel "colorwheel.cpp")
target_link_libraries(colorwheel ${PNG_LIBRARIES})

add_executable(simple_lucas_kanade "simple_lucas_kanade.cpp")
target_link_libraries(simple_lucas_kanade ${PNG_LIBRARIES})