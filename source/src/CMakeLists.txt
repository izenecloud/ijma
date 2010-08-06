FILE(GLOB_RECURSE jma_SRC "*.cpp")

# include modules
# usage: CHECK_INCLUDE_FILES (<header> <RESULT_VARIABLE> )
INCLUDE (CheckIncludeFiles)

# check variables
CHECK_INCLUDE_FILES (iconv.h HAVE_ICONV)

# check libmecab configurations
INCLUDE(libmecab/ConfigureChecks.cmake)

add_library(jma STATIC ${jma_SRC})

IF (CMAKE_COMPILER_IS_XLC)
    # link libjma.a with libiconv.a
    target_link_libraries(jma iconv)
ENDIF (CMAKE_COMPILER_IS_XLC)

# generate jma_config.h
CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/../include/jma_config.h.in ${CMAKE_CURRENT_BINARY_DIR}/jma_config.h)
