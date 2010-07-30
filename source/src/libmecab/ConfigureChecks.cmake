# include modules
INCLUDE (CheckIncludeFiles)
# usage: CHECK_INCLUDE_FILES (<header> <RESULT_VARIABLE> )
INCLUDE (CheckFunctionExists)
# usage: CHECK_FUNCTION_EXISTS (<function> <RESULT_VARIABLE> )
INCLUDE (CheckLibraryExists)
# usage: CHECK_LIBRARY_EXISTS (<library> <function> <location> <RESULT_VARIABLE> )

# check variables
CHECK_INCLUDE_FILES (fcntl.h HAVE_FCNTL_H)
CHECK_INCLUDE_FILES (ctype.h HAVE_CTYPE_H)
CHECK_INCLUDE_FILES (dirent.h HAVE_DIRENT_H)
CHECK_INCLUDE_FILES (dlfcn.h HAVE_DLFCN_H)
CHECK_INCLUDE_FILES (iconv.h HAVE_ICONV)
CHECK_FUNCTION_EXISTS (getenv HAVE_GETENV)
CHECK_FUNCTION_EXISTS (getpagesize HAVE_GETPAGESIZE)
CHECK_INCLUDE_FILES (inttypes.h HAVE_INTTYPES_H)
CHECK_INCLUDE_FILES (io.h HAVE_IO_H)
CHECK_LIBRARY_EXISTS (iconv iconv_open "" HAVE_LIBICONV)
CHECK_LIBRARY_EXISTS (pthread "pthread_create;pthread_join;pthread_mutex_lock" "" HAVE_LIBPTHREAD)
CHECK_INCLUDE_FILES (memory.h HAVE_MEMORY_H)
CHECK_FUNCTION_EXISTS (mmap HAVE_MMAP)
CHECK_FUNCTION_EXISTS (opendir HAVE_OPENDIR)
CHECK_INCLUDE_FILES (pthread.h HAVE_PTHREAD_H)
CHECK_FUNCTION_EXISTS (setjmp HAVE_SETJMP)
CHECK_INCLUDE_FILES (setjmp.h HAVE_SETJMP_H)
CHECK_INCLUDE_FILES (stdint.h HAVE_STDINT_H)
CHECK_INCLUDE_FILES (stdlib.h HAVE_STDLIB_H)
CHECK_INCLUDE_FILES (strings.h HAVE_STRINGS_H)
CHECK_INCLUDE_FILES (string.h HAVE_STRING_H)
CHECK_INCLUDE_FILES (sys/mman.h HAVE_SYS_MMAN_H)
CHECK_INCLUDE_FILES (sys/stat.h HAVE_SYS_STAT_H)
CHECK_INCLUDE_FILES (sys/times.h HAVE_SYS_TIMES_H)
CHECK_INCLUDE_FILES (sys/types.h HAVE_SYS_TYPES_H)
CHECK_INCLUDE_FILES (unistd.h HAVE_UNISTD_H)
CHECK_INCLUDE_FILES (windows.h HAVE_WINDOWS_H)

# generate config.h
CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/libmecab/config.h.in ${CMAKE_CURRENT_BINARY_DIR}/config.h)

# include config.h
include_directories(${CMAKE_CURRENT_BINARY_DIR})
add_definitions(-DHAVE_CONFIG_H)

# preprocessor definitions
add_definitions(-DDIC_VERSION=102)
add_definitions(-DICONV_CONST=)
add_definitions(-DPACKAGE="mecab")
add_definitions(-DVERSION="0.98pre3")

#IF (UNIX)
    #MESSAGE(STATUS "libmecab in UNIX")
    #add_definitions(-DMECAB_DEFAULT_RC="/dev/null")
#ELSE (WIN32)
    #MESSAGE(STATUS "libmecab in WIN32")
    #add_definitions(-DDLL_EXPORT)
    #add_definitions(-DMECAB_DEFAULT_RC="NUL")
#ENDIF (UNIX)
