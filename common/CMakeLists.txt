cmake_minimum_required(VERSION 3.10)

aux_source_directory(. DIR_COMMON_SRCS)

add_library(common STATIC ${DIR_COMMON_SRCS})

set_target_properties(common PROPERTIES DEFINE_SYMBOL COMMON_DLL)

install(TARGETS common LIBRARY DESTINATION lib RUNTIME DESTINATION lib ARCHIVE DESTINATION lib)

install(FILES cconf.h Constants.h Errors.h DESTINATION include/fz)