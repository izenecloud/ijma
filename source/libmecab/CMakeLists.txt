# objs
set(objs_mecab
char_property
connector
context_id
dictionary_compiler
dictionary_generator
dictionary
dictionary_rewriter
eval
feature_index
iconv_utils
lbfgs
learner
learner_tagger
libmecab
nbest_generator
param
string_buffer
tagger
tokenizer
utils
viterbi
writer
)

# preprocessor definitions
add_definitions(-DHAVE_FCNTL_H -DHAVE_GETENV -DHAVE_MMAP -DHAVE_STRING_H -DHAVE_SYS_MMAN_H -DHAVE_SYS_STAT_H -DHAVE_SYS_TYPES_H -DHAVE_UNISTD_H)
add_definitions(-DDIC_VERSION=102)
add_definitions(-DICONV_CONST=)
add_definitions(-DMECAB_DEFAULT_RC="/usr/local/etc/mecabrc")
add_definitions(-DPACKAGE="mecab")
add_definitions(-DVERSION="0.98pre3")

IF (UNIX)
    MESSAGE(STATUS "libmecab in UNIX")
    add_definitions(-DHAVE_PTHREAD_H)
    add_definitions(-DHAVE_ICONV)
    add_definitions(-DHAVE_DIRENT_H )
ELSE (WIN32)
    MESSAGE(STATUS "libmecab in WIN32")
    add_definitions(-DHAVE_WINDOWS_H)
    add_definitions(-DDLL_EXPORT)
ENDIF (UNIX)

# output definitions
add_library(mecab SHARED ${objs_mecab})

add_executable(mecab-exe mecab.cpp)
target_link_libraries(mecab-exe ${LIBS_MECAB})

add_executable(mecab-cost-train mecab-cost-train.cpp)
target_link_libraries(mecab-cost-train ${LIBS_MECAB})

add_executable(mecab-dict-gen mecab-dict-gen.cpp)
target_link_libraries(mecab-dict-gen ${LIBS_MECAB})

add_executable(mecab-dict-index mecab-dict-index.cpp)
target_link_libraries(mecab-dict-index ${LIBS_MECAB})

add_executable(mecab-system-eval mecab-system-eval.cpp)
target_link_libraries(mecab-system-eval ${LIBS_MECAB})

add_executable(mecab-test-gen mecab-test-gen.cpp)
target_link_libraries(mecab-test-gen ${LIBS_MECAB})
