file(GLOB SRC_FILES *.cpp *.h)
project(test)

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

find_package(ZLIB REQUIRED)
find_package(Boost COMPONENTS system filesystem thread program_options REQUIRED)

if(WIN32)
	INCLUDE_DIRECTORIES(C:/fz/include ${Boost_INCLUDE_DIRS} ${ZLIB_INCLUDE_DIR})
else ()
	INCLUDE_DIRECTORIES(/usr/local/include/fz ${Boost_INCLUDE_DIRS} ${ZLIB_INCLUDE_DIR})
endif(WIN32)
add_executable(test test.cpp)

if(WIN32)
	target_link_libraries(test 
	wsock32
	ws2_32
	${Boost_LIBRARIES}/libboost_system-mgw81-mt-s-x64-1_70.a 
	${Boost_LIBRARIES}/libboost_filesystem-mgw81-mt-s-x64-1_70.a 
	${Boost_LIBRARIES}/libboost_thread-mgw81-mt-s-x64-1_70.a 
	${Boost_LIBRARIES}/libboost_program_options-mgw81-mt-s-x64-1_70.a
	${ZLIB_LIBRARY}/libzlibstatic.a
	C:/fz/lib/libfz.dll.a
	)
else ()
	target_link_libraries(test /usr/local/lib/libfz.so ${ZLIB_LIBRARY} ${Boost_LIBRARIES})
endif(WIN32)