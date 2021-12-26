add_executable(simple_harris "simple_harris.cpp")
target_link_libraries(simple_harris ${PNG_LIBRARIES})

add_executable(simple_shi_tomasi "simple_shi_tomasi.cpp")
target_link_libraries(simple_shi_tomasi ${PNG_LIBRARIES})

add_executable(simple_fast "simple_fast.cpp")
target_link_libraries(simple_fast ${PNG_LIBRARIES})

add_executable(simple_orb "simple_orb.cpp")
target_link_libraries(simple_orb ${PNG_LIBRARIES})