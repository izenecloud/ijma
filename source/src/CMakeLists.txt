# objs
set(objs_jma
./analyzer
./knowledge
./sentence
./jma_factory
)

add_library(jma ${objs_jma})
target_link_libraries(jma ${LIBS_MECAB})
