cmake_minimum_required(VERSION 3.10)

aux_source_directory(. DIR_COMMON_SRCS)

add_library(common SHARED ${DIR_COMMON_SRCS})

install(TARGETS common LIBRARY DESTINATION lib RUNTIME DESTINATION lib ARCHIVE DESTINATION lib)

install(FILES Constants.h DESTINATION include/fz)