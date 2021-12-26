# CMakeLists.txt
#
#     Author: Fabian Meyer
# Created On: 08 Nov 2018

cmake_minimum_required(VERSION 3.10)
project(cv-eigen)

option(BUILD_TESTS "Enables if tests should be built" OFF)
option(BUILD_EXAMPLES "Enables if examples should be built" OFF)

SET(CMAKE_CXX_STANDARD 17)
SET(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if (MSVC)
    # warning level 4 and all warnings as errors
    add_compile_options(/W4 /WX)
else()
    # lots of warnings and all warnings as errors
    add_compile_options(-Wall -Wextra -pedantic -Werror)
endif()

add_subdirectory(dep)

include_directories(
    include
    ${EIGEN3_INCLUDE_DIR}
)

install(DIRECTORY "include/cve"
        DESTINATION include)

if(${BUILD_TESTS})
    # Compile tests
    include_directories(${CATCH2_INCLUDE_DIR})
    add_subdirectory(test)
endif(${BUILD_TESTS})

if(${BUILD_EXAMPLES})
    # Compile examples
    add_subdirectory(examples)
endif(${BUILD_EXAMPLES})
