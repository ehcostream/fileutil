file(GLOB SRC_FILES *.cpp *.h)
project(test)


if(NOT MSVC)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -O2 -Wall -Werror -z defs -mcmodel=medium")
else()
  add_definitions(-D_WIN32_WINNT=0x600)
endif()

include_directories(common)


if(WIN32)
	INCLUDE_DIRECTORIES(C:/futil/include)

else ()
	INCLUDE_DIRECTORIES(/usr/local/include)
endif(WIN32)
add_executable(test test.cpp)

if(WIN32)
	target_link_libraries(test 
	wsock32
	ws2_32
	C:/futil/lib/futil.lib
	)
else ()
	target_link_libraries(test /usr/local/lib/libfutil.so /usr/local/lib/libcommon.a)
endif(WIN32)