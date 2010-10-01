############################################################
## [ CONFIGURATION INFORMATION ]
############################################################
## Architecture     : SPARC
## Operation System : Solaris 9
## Compiler Version : CC 5.6
## Compile          : 64bit
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
    CC_FLAGS = -c -mt -g -w -xarch=generic64 -DARCH64 -DSOLARIS -DSUNCC -DSOLARIS_NATIVE -DDEBUG
else
    CC_FLAGS = -c -mt -xO2 -w -xarch=generic64 -DARCH64 -DSOLARIS -DSUNCC -DSOLARIS_NATIVE
endif # DEBUG
CC_LIB_FLAGS =
CC_CON_FLAGS =
CC_DLL_FLAGS = -kPIC

############################################################
## [ LINKER ]
############################################################
EXE_PROG      = CC
EXE_FLAGS     = -lCstd -xarch=generic64
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
