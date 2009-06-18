link_directories(${PROJECT_SOURCE_DIR}/../lib)

add_executable(test_jma_knowledge test_jma_knowledge.cpp)

target_link_libraries(test_jma_knowledge ${LIBS_JMA} ${LIBS_MECAB})
