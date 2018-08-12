cmake_minimum_required(VERSION 3.11)
project(maptool)

set(CMAKE_CXX_STANDARD 17)

find_package(Boost 1.67.0 COMPONENTS system filesystem)

add_executable(maptool main.cpp LocationBundleReader.cpp LocationBundleReader.hpp structs.hpp utils.hpp utils.cpp)

if (Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
    target_link_libraries(maptool LINK_PUBLIC ${Boost_LIBRARIES})
endif ()