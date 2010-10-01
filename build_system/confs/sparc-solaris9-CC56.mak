############################################################
## [ CONFIGURATION INFORMATION ]
############################################################
## Architecture     : SPARC
## Operation System : Solaris 9
## Compiler Version : CC 5.6
## Compile          : 32bit
############################################################

############################################################
## [ MAKEFILE ]
############################################################
MAKE_FILE = Makefile

############################################################
## [ LIBRARY ]
############################################################
LIB_PROG  = CC -xar
LIB_FLAGS = -o

############################################################
## [ COMPILER ]
############################################################
CC_PROG      = CC
ifdef DEBUG
    CC_FLAGS = -c -mt -g -w -DSOLARIS -DSUNCC -DSOLARIS_NATIVE -DDEBUG
else
    CC_FLAGS = -c -mt -xO2 -w -DSOLARIS -DSUNCC -DSOLARIS_NATIVE
endif # DEBUG
ifdef SF1_EXTEND
    CC_FLAGS += -library=stlport4 -DEXTENDED_LA -D_REENTRANT
endif
CC_LIB_FLAGS =
CC_CON_FLAGS =
CC_DLL_FLAGS = -kPIC

############################################################
## [ LINKER ]
############################################################
EXE_PROG      = CC
ifdef SF1_EXTEND
    EXE_FLAGS = -library=stlport4
else
    EXE_FLAGS = -lCstd
endif
EXE_CON_FLAGS = -lsocket -lnsl -lm -lkstat
EXE_DLL_FLAGS = -G
EXE_DLL_EXT   = so

###########################################################
## [ JAVA ]
############################################################
JNI_INCLUDED = solaris
JNI_EXE_LIBS =
JAVA_PROG    = "$(JAVA_HOME)/bin/java" -Djava.library.path=.
JAVAC_PROG   = "$(JAVA_HOME)/bin/javac"
JAVAH_PROG   = "$(JAVA_HOME)/bin/javah"
JAVA_FLAGS   = -classpath . -d .
JAR_PROG     = "$(JAVA_HOME)/bin/jar"
JAR_FLAGS    = cvf
