cmake_minimum_required(VERSION 3.10)
project(TreeCacheMalloc)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_BUILD_TYPE Debug)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0")
set(CMAKE_LIBRARY_PATH /usr/local/lib/ ${CMAKE_LIBRARY_PATH})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address")
set(CMAKE_LINKER_FLAGS "${CMAKE_LINKER_FLAGS} -fsanitize=address")

include_directories(${CMAKE_SOURCE_DIR}/include)

set(SOURCES
    src/central_cache.cpp
    src/page_cache.cpp
    src/thread_cache.cpp
)

add_library(tiny_tcmalloc SHARED ${SOURCES})

target_link_libraries(tiny_tcmalloc PRIVATE pthread)

add_subdirectory(bench)

install(TARGETS tiny_tcmalloc
        DESTINATION lib)