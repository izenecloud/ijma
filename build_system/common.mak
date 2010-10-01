############################################################
## [ COMPILE MODE ] 
############################################################
#DEBUG    = 1
ifndef DEBUG
    RELEASE = 1
endif

############################################################
## [ OTHERS PROGRAMS ]
############################################################
DEL_PROG = /bin/rm -rf
RMD_PROG = /bin/rmdir 
CPY_PROG = /bin/cp
MKD_PROG = /bin/mkdir -p

############################################################
## [ INCLUDE ENVIRONMENT FOR PLATFORM ]
############################################################
include $(JMA_SRC)/build_system/confs/$(JMA_BUILD).mak
