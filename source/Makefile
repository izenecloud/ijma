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
DIRS = src/libiconv src/libmecab src/libz src test_src
TEST_DIRS = test_src

PWD = $(shell pwd)

##################################################
# all
##################################################
all:
	@for dir in $(DIRS); do \
		echo "===> $(PWD)/$$dir"; \
		(cd $$dir && $(MAKE) -f $(MAKE_FILE) $@) || exit 1; \
		echo "<=== $(PWD)/$$dir"; \
	done

##################################################
# clean
##################################################
clean:
	@for dir in $(DIRS); do \
		echo "===> $(PWD)/$$dir"; \
		(cd $$dir && $(MAKE) -f $(MAKE_FILE) $@) || exit 1; \
		echo "<=== $(PWD)/$$dir"; \
	done

##################################################
# bin
##################################################
test: all
	@for dir in $(TEST_DIRS); do \
		echo "===> $(PWD)/$$dir"; \
		(cd $$dir && $(MAKE) -f $(MAKE_FILE) $@) || exit 1; \
		echo "<=== $(PWD)/$$dir"; \
	done
