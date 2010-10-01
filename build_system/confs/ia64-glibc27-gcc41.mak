############################################################
## [ CONFIGURATION INFORMATION ]
############################################################
## Architecture     : Intel/AMD (x86)
## Operation System : Linux
## Compiler Version : glibc 2.7 / gcc 4.1.0
## Compile          : 64bit
############################################################

############################################################
## [ MAKEFILE ]
############################################################
MAKE_FILE = Makefile

############################################################
## [ LIBRARY ]
############################################################
LIB_PROG  = ar
LIB_FLAGS = rcv

############################################################
## [ COMPILER ]
############################################################
CC_PROG      = gcc
ifdef DEBUG
    CC_FLAGS = -c -g -Wall -DARCH64 -DLINUX -DDEBUG
else
    CC_FLAGS = -c -O2 -Wall -fno-strict-aliasing -DARCH64 -DLINUX
endif # DEBUG
CC_LIB_FLAGS =
CC_CON_FLAGS =
CC_DLL_FLAGS = -fPIC

############################################################
## [ LINKER ]
############################################################
EXE_PROG      = gcc
EXE_FLAGS     = -lstdc++ -lpthread
EXE_CON_FLAGS = 
EXE_DLL_FLAGS = -shared
EXE_DLL_EXT   = so

###########################################################
## [ JAVA ]
############################################################
JNI_INCLUDED = linux
JNI_EXE_LIBS =
JAVA_PROG    = "$(JAVA_HOME)/bin/java" -Djava.library.path=.
JAVAC_PROG   = "$(JAVA_HOME)/bin/javac"
JAVAH_PROG   = "$(JAVA_HOME)/bin/javah"
JAVA_FLAGS   = -classpath . -d .
JAR_PROG     = "$(JAVA_HOME)/bin/jar"
JAR_FLAGS    = cvf
