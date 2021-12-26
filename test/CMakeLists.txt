
include_directories(${CMAKE_CURRENT_LIST_DIR}/include)

add_executable(cve_tests
    "src/main.cpp"
    "src/core/image_correlation_test.cpp"
    "src/core/image_interpolation_test.cpp"
    "src/core/image_resizing_test.cpp"
    "src/core/pixel_test.cpp"
    "src/core/pixel_color_space_test.cpp"

    "src/filter/backward_differences_filter_test.cpp"
    "src/filter/forward_differences_filter_test.cpp"
    "src/filter/central_differences_filter_test.cpp"
    "src/filter/box_filter_test.cpp"
    "src/filter/gauss_filter_test.cpp"
    "src/filter/sobel_filter_test.cpp"
    "src/filter/scharr_filter_test.cpp"
    "src/filter/laplace_filter_test.cpp"
    "src/filter/diffusion_filter_test.cpp"
    "src/filter/canny_edge_filter_test.cpp"

    "src/feature/harris_feature_test.cpp"
    "src/feature/shi_tomasi_feature_test.cpp"
    "src/feature/fast_feature_test.cpp"
    "src/feature/orb_feature_test.cpp"
    "src/feature/brief_descriptor_test.cpp"
    "src/feature/orb_descriptor_test.cpp"
)
