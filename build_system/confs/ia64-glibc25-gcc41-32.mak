############################################################
## [ CONFIGURATION INFORMATION ]
############################################################
## Architecture     : Intel/AMD (x86)
## Operation System : Linux
## Compiler Version : glibc 2.3 / gcc 3.2
## Compile          : 32bit
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
    CC_FLAGS = -c -g -Wall -m32 -DLINUX -DDEBUG
else
    CC_FLAGS = -c -O2 -Wall -m32 -fno-strict-aliasing -DLINUX
endif # DEBUG
CC_LIB_FLAGS =
CC_CON_FLAGS =
CC_DLL_FLAGS = -fPIC

############################################################
## [ LINKER ]
############################################################
EXE_PROG      = gcc
EXE_FLAGS     = -m32 -lstdc++ -lpthread
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
