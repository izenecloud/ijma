# objs
set(objs_jma
./analyzer
./knowledge
./sentence
./jma_factory
./jma_knowledge
./jma_analyzer
./jma_ctype
./jma_ctype_eucjp
./jma_ctype_sjis
./jma_ctype_utf8
./tokenizer
./pos_table
./iconv_utils
)

# include modules
# usage: CHECK_INCLUDE_FILES (<header> <RESULT_VARIABLE> )
INCLUDE (CheckIncludeFiles)

# check variables
CHECK_INCLUDE_FILES (iconv.h HAVE_ICONV)

add_library(jma STATIC ${objs_jma})

IF (CMAKE_COMPILER_IS_XLC)
    # link libjma.a with libiconv.a
    target_link_libraries(jma ${LIBS_MECAB} iconv)
ELSE (CMAKE_COMPILER_IS_XLC)
    target_link_libraries(jma ${LIBS_MECAB})
ENDIF (CMAKE_COMPILER_IS_XLC)

# generate jma_config.h
CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/../include/jma_config.h.in ${CMAKE_CURRENT_BINARY_DIR}/jma_config.h)

# include jma_config.h
include_directories(${CMAKE_CURRENT_BINARY_DIR})
add_definitions(-DHAVE_JMA_CONFIG_H)
