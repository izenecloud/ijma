############################################################
## [ CONFIGURATION INFORMATION ]
############################################################
## Architecture     : PA-RISC
## Operation System : HP-UX 11
## Compiler Version : aCC 3.30
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
CC_PROG      = aCC
ifdef DEBUG
    CC_FLAGS = -c -g -AA -mt +W829,921 -DHPUX -DDEBUG
else
    CC_FLAGS = -c +O2 -AA -mt +W829,921 -DHPUX
endif # DEBUG
CC_LIB_FLAGS =
CC_CON_FLAGS =
CC_DLL_FLAGS = +Z

############################################################
## [ LINKER ]
############################################################
EXE_PROG      = aCC
EXE_FLAGS     = -AA -Wl,+s,+b,../lib -lm
EXE_CON_FLAGS = 
EXE_DLL_FLAGS = -b
EXE_DLL_EXT   = sl

###########################################################
## [ JAVA ]
############################################################
JNI_INCLUDED = hp-ux
JNI_EXE_LIBS = -lstd_v2 -lCsup_v2
JAVA_PROG    = "$(JAVA_HOME)/bin/java" -Djava.library.path=.
JAVAC_PROG   = "$(JAVA_HOME)/bin/javac"
JAVAH_PROG   = "$(JAVA_HOME)/bin/javah"
JAVA_FLAGS   = -classpath . -d .
JAR_PROG     = "$(JAVA_HOME)/bin/jar"
JAR_FLAGS    = cvf
