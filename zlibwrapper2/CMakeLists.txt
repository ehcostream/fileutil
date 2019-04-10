aux_source_directory(. DIR_ZLIBWRAPPER_SRCS)

include_directories(${ZLIB_INCLUDE_DIR})

add_library(zlibwrapper2 ${DIR_ZLIBWRAPPER_SRCS})