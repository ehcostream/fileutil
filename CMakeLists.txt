cmake_minimum_required(VERSION 3.10)
SET(EXECUTABLE_OUTPUT_PATH bin)

file(GLOB SRC_FILES *.cpp *.h)
project(zlibutil)

if(WIN32)
#you can set your zlib full path
SET (ZLIB_INCLUDE_DIR "d:/zlib/include")
SET (ZLIB_LIBRARY "d:/zlib/lib")
#you can set your boost full path
SET (Boost_INCLUDE_DIRS "d:/boost/include/boost-1_70")
SET (Boost_LIBRARIES "d:/boost/lib")

set(Boost_USE_STATIC_LIBS ON) 
set(Boost_USE_STATIC_RUNTIME ON) 
endif(WIN32)

set(CMAKE_CXX_FLAGS_DEBUG "-std=c++11 -g -O2 -Wall -Werror -z defs -mcmodel=medium")

set(CMAKE_CXX_FLAGS_RELEASE "-std=c++11 -O2 -Wall -Werror -z defs -mcmodel=medium")

find_package(ZLIB REQUIRED)

find_package(Boost COMPONENTS system filesystem thread program_options REQUIRED)

include_directories(./zlibwrapper ${Boost_INCLUDE_DIRS} ${ZLIB_INCLUDE_DIR})

add_subdirectory(zlibwrapper)

add_library(fz SHARED ${SRC_FILES})

if(WIN32)
	target_link_libraries(fz wsock32 ws2_32)
	target_link_libraries(fz 
	zlibwrapper 
	${Boost_LIBRARIES}/libboost_system-mgw81-mt-s-x64-1_70.a 
	${Boost_LIBRARIES}/libboost_filesystem-mgw81-mt-s-x64-1_70.a 
	${Boost_LIBRARIES}/libboost_thread-mgw81-mt-s-x64-1_70.a 
	${Boost_LIBRARIES}/libboost_program_options-mgw81-mt-s-x64-1_70.a
	${ZLIB_LIBRARY}/libzlibstatic.a)
else ()
	target_link_libraries(fz zlibwrapper ${ZLIB_LIBRARY} ${Boost_LIBRARIES})
endif(WIN32)
