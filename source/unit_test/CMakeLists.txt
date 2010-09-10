include_directories(../libgtest/include)

FILE(GLOB_RECURSE unit_test_SRC "*.cpp")

MESSAGE(STATUS "unit_test_SRC: " ${unit_test_SRC})

link_directories(${PROJECT_SOURCE_DIR}/../lib)
add_executable(unit_test_jma ${unit_test_SRC})
target_link_libraries(unit_test_jma ${LIBS_JMA} gtest_main)
