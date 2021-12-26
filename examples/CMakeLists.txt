# CMakeLists.txt
#
#     Author: Fabian Meyer
# Created On: 19 Jul 2019

find_package(PNG REQUIRED)

include_directories(
    ${PNG_INCLUDE_DIRS}
)

add_subdirectory(draw)
add_subdirectory(filter)
add_subdirectory(imageio)
add_subdirectory(feature)
add_subdirectory(optflow)