cmake_minimum_required(VERSION 3.5)

SET(EXECUTABLE_OUTPUT_PATH ./bin)

file(GLOB SRC_FILES *.cpp *.h)

project(zlibutil)

find_package(ZLIB REQUIRED)

find_package(Boost COMPONENTS filesystem system REQUIRED)

set(CMAKE_CXX_FLAGS_DEBUG "-std=c++11 -g -O2 -Wall -Werror -z defs -mcmodel=medium")

set(CMAKE_CXX_FLAGS_RELEASE "-std=c++11 -O2 -Wall -Werror -z defs -mcmodel=medium")

add_executable(fz ${SRC_FILES})

include_directories(./zlibwrapper ${ZLIB_INCLUDE_DIR} ${Boost_INCLUDE_DIRS})

add_subdirectory(zlibwrapper)

target_link_libraries(fz zlibwrapper ${ZLIB_LIBRARY} ${Boost_LIBRARIES})