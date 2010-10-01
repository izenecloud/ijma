##################################################
# include c-compiler environment makefile
##################################################
ifeq ($(origin JMA_SRC), undefined)
$(error no environment variable JMA_SRC))
else
include $(JMA_SRC)/build_system/common.mak
endif

##################################################
# definition for target
##################################################
OBJS =

INCS =

LIB_OUT =

INC_DIRS = -I"$(JMA_SRC)/include" -I"$(JMA_SRC)/source/include" -I"$(JMA_SRC)/source/src" -I"$(JMA_SRC)/source/src/libiconv" -I"$(JMA_SRC)/source/src/libmecab" -I"$(JMA_SRC)/source/src/libz"

LIB_DIRS = -L"$(JMA_SRC)/source/src" -L"$(JMA_SRC)/source/src/libiconv" -L"$(JMA_SRC)/source/src/libmecab" -L"$(JMA_SRC)/source/src/libz"

EXE_LIBS = -Xlinker --start-group -liconv -lmecab -lz -ljma --end-group

##################################################
# definition for test
##################################################
TEST_SOURCE = $(shell ls test_*.cpp)
TEST_OBJS   = $(TEST_SOURCE:.cpp=.o)
TEST_EXE    = $(TEST_SOURCE:.cpp=)

##################################################
# suffixes rules
##################################################
.SUFFIXES: .cpp .c .o .d

##################################################
# default
##################################################
all: clean-out $(LIB_OUT)

##################################################
# library output
##################################################
$(LIB_OUT): $(OBJS)
	$(LIB_PROG) $(LIB_FLAGS) $@ $?

##################################################
# test
##################################################
test: clean-test $(TEST_EXE)

test_%: test_%.o $(LIB_OUT)
	$(EXE_PROG) $(EXE_FLAGS) $(EXE_CON_FLAGS) $(LP_FLAGS) -o $@ $? $(LIB_DIRS) $(EXE_LIBS)
	@$(CPY_PROG) $@ $(JMA_SRC)/bin/

##################################################
# automatic dependency
##################################################
ifneq ($(MAKECMDGOALS),clean)
include $(OBJS:.o=.d)
include $(TEST_OBJS:.o=.d)
endif

##################################################
# compile
##################################################
%.o: %.cpp
	$(CC_PROG) -DHAVE_CONFIG_H $(CC_FLAGS) $(CC_DLL_FLAGS) $(INC_DIRS) -o $@ $<

##################################################
# generate dependency > *.d
##################################################
%.d: %.cpp
	@gcc -MM -DHAVE_CONFIG_H $(INC_DIRS) $< > $@

##################################################
# clean
##################################################
clean: clean-out clean-test
	$(DEL_PROG) $(OBJS) $(OBJS:.o=.d)
	$(DEL_PROG) $(TEST_OBJS) $(TEST_OBJS:.o=.d)
clean-out:
	$(DEL_PROG) $(LIB_OUT)
clean-test:
	$(DEL_PROG) $(TEST_EXE)
	cd $(JMA_SRC)/bin && $(DEL_PROG) $(TEST_EXE)
