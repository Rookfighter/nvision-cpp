add_executable(simple_gauss "simple_gauss.cpp")
target_link_libraries(simple_gauss ${PNG_LIBRARIES})

add_executable(simple_box "simple_box.cpp")
target_link_libraries(simple_box ${PNG_LIBRARIES})

add_executable(simple_canny "simple_canny.cpp")
target_link_libraries(simple_canny ${PNG_LIBRARIES})

add_executable(simple_sobel "simple_sobel.cpp")
target_link_libraries(simple_sobel ${PNG_LIBRARIES})

add_executable(simple_scharr "simple_scharr.cpp")
target_link_libraries(simple_scharr ${PNG_LIBRARIES})

add_executable(simple_gradient "simple_gradient.cpp")
target_link_libraries(simple_gradient ${PNG_LIBRARIES})

add_executable(simple_laplace "simple_laplace.cpp")
target_link_libraries(simple_laplace ${PNG_LIBRARIES})

add_executable(simple_diffusion "simple_diffusion.cpp")
target_link_libraries(simple_diffusion ${PNG_LIBRARIES})