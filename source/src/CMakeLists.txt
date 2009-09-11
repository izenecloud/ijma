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
)

add_library(jma ${objs_jma})
target_link_libraries(jma ${LIBS_MECAB})
