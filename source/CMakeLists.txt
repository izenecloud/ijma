cmake_minimum_required(VERSION 2.6)

#set(CMAKE_VERBOSE_MAKEFILE on)

# In CMake, compiler flags can be grouped in Configurations
#

# - Set default configurations definition, also add configuration Debugfull and Profile.
#
# Debugfull adds more debug information, and Profile adds profiling information as well.
#

IF (NOT CMAKE_BUILD_TYPE)
  #SET(CMAKE_BUILD_TYPE RelWithDebInfo)
  SET(CMAKE_BUILD_TYPE Release)
ENDIF (NOT CMAKE_BUILD_TYPE)

MESSAGE(STATUS "CMAKE_COMPILER_IS_GNUCXX: " ${CMAKE_COMPILER_IS_GNUCXX})
IF (CMAKE_COMPILER_IS_GNUCXX)
  SET(CMAKE_CONFIGURATION_TYPES ${CMAKE_CONFIGURATION_TYPES} "Debugfull" "Profile")

  # Select flags.
  SET(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O2 -g" CACHE STRING "Flags used by the compiler during Release with Debug Info builds" FORCE)
  # Disable assertion macros in RELEASE
  SET(CMAKE_CXX_FLAGS_RELEASE        "-O2 -DNDEBUG" CACHE STRING "Flags used by the compiler during release builds" FORCE)
  SET(CMAKE_CXX_FLAGS_DEBUG          "-g3 -gdwarf-2 -O0 -fno-reorder-blocks -fno-schedule-insns -fno-inline" CACHE STRING "Flags used by the compiler during debug builds" FORCE)
  SET(CMAKE_CXX_FLAGS_DEBUGFULL      "-g3 -fno-inline -DIZS_DEBUG" CACHE STRING "Debugfull build type" FORCE)
  SET(CMAKE_CXX_FLAGS_PROFILE        "-pg -O2 -DNDEBUG" CACHE STRING "Profile build type" FORCE)
  #SET(CMAKE_CXX_FLAGS_PROFILE        "-g3 -fno-inline -ftest-coverage -fprofile-arcs" CACHE STRING "Profile build type")

  MARK_AS_ADVANCED(CMAKE_CXX_FLAGS_DEBUGFULL CMAKE_CXX_FLAGS_PROFILE)
ENDIF (CMAKE_COMPILER_IS_GNUCXX)

# Select compile flags for MSVC
IF (CMAKE_COMPILER_IS_MSVC)
MESSAGE(STATUS "CMAKE_COMPILER_IS_MSVC: " ${CMAKE_COMPILER_IS_MSVC})

  SET(CMAKE_CXX_FLAGS_RELEASE "/EHsc /O2 /GL /GA /Ob2 /nologo /W3 /MT /Zi" CACHE STRING "Flags used by the compiler during release builds" FORCE)
  SET(CMAKE_CXX_FLAGS_DEBUG "/EHsc /Od /nologo /W3 /MTd /Zi" CACHE STRING "Flags used by the compiler during debug builds" FORCE)
ENDIF (CMAKE_COMPILER_IS_MSVC)

# Select compile flags for XLC
IF (CMAKE_COMPILER_IS_XLC)
MESSAGE(STATUS "CMAKE_COMPILER_IS_XLC: " ${CMAKE_COMPILER_IS_XLC})

  SET(CMAKE_CXX_FLAGS_RELEASE "-O2 -q64 -qrtti -DARCH64 -DAIX -DXLC -DNDEBUG")
  SET(CMAKE_CXX_FLAGS_DEBUG "-g -q64 -qrtti -DARCH64 -DAIX -DXLC -DDEBUG")

  SET(CMAKE_EXE_LINKER_FLAGS ${CMAKE_EXE_LINKER_FLAGS} "-brtl -lm -q64")
  SET(CMAKE_SHARED_LINKER_FLAGS ${CMAKE_SHARED_LINKER_FLAGS} "-bdynamic -G")
MESSAGE(STATUS "CMAKE_EXE_LINKER_FLAGS: " ${CMAKE_EXE_LINKER_FLAGS})
MESSAGE(STATUS "CMAKE_SHARED_LINKER_FLAGS: " ${CMAKE_SHARED_LINKER_FLAGS})
ENDIF (CMAKE_COMPILER_IS_XLC)

SET(CMAKE_C_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO}" CACHE STRING "Flags used by the compiler during Release with Debug Info builds." FORCE)
SET(CMAKE_C_FLAGS_RELEASE        "${CMAKE_CXX_FLAGS_RELEASE}" CACHE STRING "Flags used by the compiler during release builds" FORCE)
SET(CMAKE_C_FLAGS_DEBUG          "${CMAKE_CXX_FLAGS_DEBUG}" CACHE STRING "Flags used by the compiler during debug builds." FORCE)
SET(CMAKE_C_FLAGS_PROFILE        "${CMAKE_CXX_FLAGS_PROFILE}" CACHE STRING "Profile build type" FORCE)

project(JMA)

IF (CMAKE_COMPILER_IS_GNUCXX)
add_definitions("-Wall -Wno-deprecated")
ENDIF (CMAKE_COMPILER_IS_GNUCXX)

include_directories(../include)
include_directories(./include)
include_directories(./src/libmecab)

set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/../lib)
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/../bin)

IF (UNIX)
    MESSAGE(STATUS "UNIX: " ${UNIX})
    set(LIBS_JMA jma pthread)
ELSE (WIN32)
    MESSAGE(STATUS "WIN32: " ${WIN32})
    set(LIBS_JMA jma) # pthread library is not used under Win32 platform.
ENDIF (UNIX)

# set compile flag for whether to print debug information
add_definitions(-DJMA_DEBUG_PRINT=${CMAKE_JMA_DEBUG_PRINT})

# set compile flag for AIX platform
IF (CMAKE_PLATFORM_IS_AIX)
    add_definitions(-DPLATFORM_IS_AIX=${CMAKE_PLATFORM_IS_AIX})
ENDIF (CMAKE_PLATFORM_IS_AIX)

# include config.h, jma_config.h
add_definitions(-DHAVE_CONFIG_H)
add_definitions(-DHAVE_JMA_CONFIG_H)
include_directories(${CMAKE_CURRENT_BINARY_DIR}/src)

add_subdirectory(src)
add_subdirectory(test)
