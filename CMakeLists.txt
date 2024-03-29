cmake_minimum_required(VERSION 3.10)

file(GLOB SRC_FILES src/*.cpp src/*.h)
file(GLOB COMMON_FILES common/*.h common/*.cpp)
project(fileutil/src)

if(NOT MSVC)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -fPIC -O2 -Wall -Werror -z defs -mcmodel=medium")
else()
  add_definitions(-D_WIN32_WINNT=0x600)
endif()

set(protobuf_MODULE_COMPATIBLE TRUE)
find_package(Protobuf CONFIG REQUIRED)
message(STATUS "Using protobuf ${protobuf_VERSION}")

set(_PROTOBUF_LIBPROTOBUF protobuf::libprotobuf)
set(_PROTOBUF_PROTOC $<TARGET_FILE:protobuf::protoc>)

# Find gRPC installation
# Looks for gRPCConfig.cmake file installed by gRPC's cmake installation.
find_package(gRPC CONFIG REQUIRED)
message(STATUS "Using gRPC ${gRPC_VERSION}")

set(_GRPC_GRPCPP_UNSECURE gRPC::grpc++_unsecure)
set(_GRPC_CPP_PLUGIN_EXECUTABLE $<TARGET_FILE:gRPC::grpc_cpp_plugin>)


# Proto file
get_filename_component(hw_proto "./protos/fileutil.proto" ABSOLUTE)
get_filename_component(hw_proto_path "${hw_proto}" PATH)

find_package(ZLIB REQUIRED)

set(Boost_USE_STATIC_LIBS ON)

find_package(Boost COMPONENTS system filesystem thread date_time chrono REQUIRED)

if(WIN32)
include_directories(common ${Boost_INCLUDE_DIRS} ${ZLIB_INCLUDE_DIR} ${gRPC_ROOT}/include ${PROTOBUF_INCLUDE_DIRS})
else()
include_directories(common ${Boost_INCLUDE_DIRS} ${ZLIB_INCLUDE_DIR})
endif(WIN32)

# Generated sources
set(hw_proto_srcs "${CMAKE_CURRENT_BINARY_DIR}/fileutil.pb.cc")
set(hw_proto_hdrs "${CMAKE_CURRENT_BINARY_DIR}/fileutil.pb.h")
set(hw_grpc_srcs "${CMAKE_CURRENT_BINARY_DIR}/fileutil.grpc.pb.cc")
set(hw_grpc_hdrs "${CMAKE_CURRENT_BINARY_DIR}/fileutil.grpc.pb.h")
add_custom_command(
      OUTPUT "${hw_proto_srcs}" "${hw_proto_hdrs}" "${hw_grpc_srcs}" "${hw_grpc_hdrs}"
      COMMAND ${_PROTOBUF_PROTOC}
      ARGS --grpc_out "${CMAKE_CURRENT_BINARY_DIR}"
        --cpp_out "${CMAKE_CURRENT_BINARY_DIR}"
        -I "${hw_proto_path}"
        --plugin=protoc-gen-grpc="${_GRPC_CPP_PLUGIN_EXECUTABLE}"
        "${hw_proto}"
      DEPENDS "${hw_proto}")

# Include generated *.pb.h files
include_directories("${CMAKE_CURRENT_BINARY_DIR}")

add_subdirectory(common)

add_library(futil SHARED ${SRC_FILES} ${hw_proto_srcs} ${hw_grpc_srcs})

#add_library(futil STATIC ${SRC_FILES} ${hw_proto_srcs} ${hw_grpc_srcs})


if(WIN32)
	target_link_libraries(futil 
	wsock32
	ws2_32
	common
	${Boost_LIBRARIES}
	${ZLIB_ROOT}/lib/zlibstatic.lib
	${_GRPC_GRPCPP_UNSECURE}
	${_PROTOBUF_LIBPROTOBUF}
	)
else ()
	target_link_libraries(futil 
						  common
						  ${ZLIB_LIBRARY}
						  libboost_chrono.a
						  libboost_date_time.a
						  libboost_filesystem.a
						  libboost_thread.a
						  libboost_system.a
						  ${_GRPC_GRPCPP_UNSECURE}
						  ${_PROTOBUF_LIBPROTOBUF}
						  )
endif(WIN32)

set_target_properties(futil PROPERTIES DEFINE_SYMBOL FILEUTIL_DLL)

set_target_properties(futil PROPERTIES VERSION 1.2 SOVERSION 1)

install(TARGETS futil LIBRARY DESTINATION lib RUNTIME DESTINATION lib ARCHIVE DESTINATION lib)

#you can uncomment if you need
#install the runtime lib on the execute application directory for testing
if(WIN32)
install(TARGETS futil RUNTIME DESTINATION D:/fileutil/test/bld/Release)
endif(WIN32)

install(FILES src/FileUtilSdk.h
			  src/fconf.h 
			  src/FileUtilGeneratorBase.h 
			  src/FileUtilBase.h 
			  src/Uncopyable.h
			  src/FileUtilGeneratorInterface.h
			  src/FileUtil.h 
			  DESTINATION include/futil)