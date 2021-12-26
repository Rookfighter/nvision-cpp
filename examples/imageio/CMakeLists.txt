add_executable(convert_image "convert_image.cpp")
target_link_libraries(convert_image ${PNG_LIBRARIES})

add_executable(resize_image "resize_image.cpp")
target_link_libraries(resize_image ${PNG_LIBRARIES})