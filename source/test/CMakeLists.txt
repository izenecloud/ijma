link_directories(${PROJECT_SOURCE_DIR}/../lib)

add_executable(test_jma_knowledge test_jma_knowledge.cpp)
add_executable(test_jma_factory test_jma_factory.cpp)

target_link_libraries(test_jma_knowledge ${LIBS_JMA} ${LIBS_MECAB})
target_link_libraries(test_jma_factory ${LIBS_JMA} ${LIBS_MECAB})
