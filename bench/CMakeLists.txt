# add_executable(test_demo test_demo.cpp)
add_executable(bench_demo bench_demo.cpp)
# add_executable(tc_malloc UnitTest.cpp)
# set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
target_link_libraries(bench_demo tiny_tcmalloc pthread)
target_include_directories(bench_demo PRIVATE ${CMAKE_SOURCE_DIR}/include)

install(TARGETS bench_demo
        DESTINATION ${CMAKE_SOURCE_DIR}/build/bin)