file(GLOB SRC_FILES *.cpp *.h)
project(test)

if(WIN32)
set(Boost_USE_STATIC_LIBS ON) 
endif(WIN32)

if(NOT MSVC)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -O2 -Wall -Werror -z defs -mcmodel=medium")
else()
  add_definitions(-D_WIN32_WINNT=0x600)
endif()

find_package(ZLIB REQUIRED)
find_package(Boost COMPONENTS system filesystem thread program_options REQUIRED)
include_directories(common ${Boost_INCLUDE_DIRS} ${ZLIB_INCLUDE_DIR})


if(WIN32)
	INCLUDE_DIRECTORIES(C:/fz/include)

else ()
	INCLUDE_DIRECTORIES(/usr/local/include/fz)
endif(WIN32)
add_executable(test test.cpp)

if(WIN32)
	target_link_libraries(test 
	wsock32
	ws2_32
	${ZLIB_LIBRARY} 
	${Boost_LIBRARIES}
	C:/fz/lib/fz.lib
	)
else ()
	target_link_libraries(test /usr/local/lib/libfz.so /usr/local/lib/libcommon.so ${ZLIB_LIBRARY} ${Boost_LIBRARIES} )
endif(WIN32)