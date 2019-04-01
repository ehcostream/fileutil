cmake_minimum_required(VERSION 3.5)

file(GLOB SRC_FILES *.cpp *.h)

project(zlibutil)

find_package(ZLIB REQUIRED)

set(CMAKE_CXX_FLAGS "-std=c++14 -g -Wall -Werror -z defs -mcmodel=medium")

add_executable(main ${SRC_FILES})

include_directories(${ZLIB_INCLUDE_DIR})

target_link_libraries(main ${ZLIB_LIBRARY})