cmake_minimum_required(VERSION 3.10)
SET(EXECUTABLE_OUTPUT_PATH bin)

file(GLOB SRC_FILES *.h *.cpp)
project(server)

if(NOT MSVC)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -O2 -Wall -Werror -z defs -mcmodel=medium")
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
get_filename_component(hw_proto "../protos/fileutil.proto" ABSOLUTE)
get_filename_component(hw_proto_path "${hw_proto}" PATH)

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

add_executable(server ${SRC_FILES} ${hw_proto_srcs} ${hw_grpc_srcs})

if(WIN32)
	target_link_libraries(server wsock32 ws2_32)
	target_link_libraries(server 
	${Boost_LIBRARIES}/libboost_system-mgw81-mt-s-x64-1_70.a 
	${Boost_LIBRARIES}/libboost_filesystem-mgw81-mt-s-x64-1_70.a 
	${Boost_LIBRARIES}/libboost_thread-mgw81-mt-s-x64-1_70.a 
	${Boost_LIBRARIES}/libboost_program_options-mgw81-mt-s-x64-1_70.a
	${ZLIB_LIBRARY}/libzlibstatic.a)
else ()
	target_link_libraries(server 
						  ${ZLIB_LIBRARY} 
						  ${Boost_LIBRARIES}
						  ${_GRPC_GRPCPP_UNSECURE}
						  ${_PROTOBUF_LIBPROTOBUF})
endif(WIN32)