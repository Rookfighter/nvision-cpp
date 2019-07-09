
include_directories(
    ${CMAKE_CURRENT_LIST_DIR}
    ${CATCH2_INCLUDE_DIR}
)

set(TEST_SRC
    "main.cpp"

    "filter/backward_differences_filter.cpp"
    "filter/central_differences_filter.cpp"
    "filter/forward_differences_filter.cpp"
    "filter/gauss_filter.cpp"
    "filter/scharr_filter.cpp"
    "filter/sobel_filter.cpp"
)

add_executable(run_tests ${TEST_SRC})
