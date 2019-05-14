file(GLOB SRC_FILES *.cpp *.h)
project(test)
find_package(ZLIB REQUIRED)
find_package(Boost COMPONENTS system filesystem thread program_options REQUIRED)

INCLUDE_DIRECTORIES(/usr/local/include/fz)
add_executable(test test.cpp)
target_link_libraries(test /usr/local/lib/libfz.so ${ZLIB_LIBRARY} ${Boost_LIBRARIES})
