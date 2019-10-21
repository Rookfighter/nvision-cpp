# CMakeLists.txt
#
#     Author: Fabian Meyer
# Created On: 19 Jul 2019

find_package(PNG REQUIRED)

include_directories(
    ${PNG_INCLUDE_DIRS}
)


add_executable(detect_corners "detect_corners.cpp")
add_executable(detect_edges "detect_edges.cpp")
add_executable(draw_shapes "draw_shapes.cpp")
add_executable(filter_smooth "filter_smooth.cpp")
add_executable(find_matches "find_matches.cpp")
add_executable(optical_flow "optical_flow.cpp")
add_executable(resize_image "resize_image.cpp")
add_executable(simple_gauss "simple_gauss.cpp")
add_executable(simple_fast "simple_fast.cpp")

target_link_libraries(detect_corners ${PNG_LIBRARIES})
target_link_libraries(detect_edges ${PNG_LIBRARIES})
target_link_libraries(draw_shapes ${PNG_LIBRARIES})
target_link_libraries(filter_smooth ${PNG_LIBRARIES})
target_link_libraries(find_matches ${PNG_LIBRARIES})
target_link_libraries(optical_flow ${PNG_LIBRARIES})
target_link_libraries(resize_image ${PNG_LIBRARIES})
target_link_libraries(simple_gauss ${PNG_LIBRARIES})
target_link_libraries(simple_fast ${PNG_LIBRARIES})
