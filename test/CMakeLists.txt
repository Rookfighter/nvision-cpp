
include_directories(
    ${CMAKE_CURRENT_LIST_DIR}
    ${CATCH2_INCLUDE_DIR}
)

set(TEST_SRC
    "main.cpp"

    "feature/brief_descriptor.cpp"
    "feature/brute_force_knn.cpp"
    "feature/fast_detector.cpp"
    "feature/harris_detector.cpp"
    "feature/shi_tomasi_detector.cpp"

    "filter/backward_differences_filter.cpp"
    "filter/box_filter.cpp"
    "filter/central_differences_filter.cpp"
    "filter/forward_differences_filter.cpp"
    "filter/gauss_filter.cpp"
    "filter/scharr_filter.cpp"
    "filter/sobel_filter.cpp"

    "projective/camera_calibration.cpp"
    "projective/direct_linear_transformation.cpp"
    "projective/homography_decomposition.cpp"
)

add_executable(run_tests ${TEST_SRC})
