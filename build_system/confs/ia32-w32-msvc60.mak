############################################################
## [ CONFIGURATION INFORMATION ]
############################################################
## Architecture     : Intel/AMD (x86)
## Operation System : Windows
## Compiler Version : MS Visual Studio 6.0
## Compile          : 32bit
############################################################

############################################################
## [ MAKEFILE ]
############################################################
MAKE_FILE = Makefile.win

############################################################
## [ LIBRARY ]
############################################################
LIB_PROG  = lib.exe
LIB_FLAGS = /nologo

############################################################
## [ COMPILER ]
############################################################
CC_PROG          = cl.exe
ifdef DEBUG
    CC_FLAGS     = /nologo /MTd /W3 /GX /Od /GZ /Zi /Zm500 /c \
                   /D "WIN32" /D "_DEBUG" /D "DEBUG" /D "_MBCS"
    CC_LIB_FLAGS = /D "_LIB"
    CC_CON_FLAGS = /D "_CONSOLE"
    CC_COM_FLAGS = /D "_USRDLL"
	#CC_DLL_FLAGS = /D "_USRDLL"
else
    CC_FLAGS     = /nologo /MT /W3 /GX /O2 /Zm500 /c \
                   /D "WIN32" /D "NDEBUG" /D "_MBCS"
    CC_LIB_FLAGS = /D "_LIB"
    CC_CON_FLAGS = /D "_CONSOLE"
    CC_COM_FLAGS = /D "_USRDLL" /D "_ATLDLL" /D "_ATL_MIN_CRT" /D "_WINDOWS"
	#CC_DLL_FLAGS = /D "_USRDLL"
endif # DEBUG
ifdef SF1_EXTEND
	CC_FLAGS += /D "EXTENDED_LA"
endif

############################################################
## [ LINKER ]
############################################################
EXE_PROG          = link.exe
ifdef DEBUG
    EXE_FLAGS     = /nologo /debug /pdbtype:sept
    EXE_CON_FLAGS = /subsystem:console /incremental:no /machine:I386 \
                    kernel32.lib user32.lib gdi32.lib winspool.lib   \
                    comdlg32.lib advapi32.lib shell32.lib ole32.lib  \
                    oleaut32.lib uuid.lib odbc32.lib odbccp32.lib
    EXE_COM_FLAGS = /subsystem:windows /dll /incremental:no /machine:I386 \
                    kernel32.lib user32.lib gdi32.lib winspool.lib        \
                    comdlg32.lib advapi32.lib shell32.lib ole32.lib       \
                    oleaut32.lib uuid.lib odbc32.lib odbccp32.lib
    EXE_DLL_FLAGS = /dll /incremental:no /machine:I386              \
                    kernel32.lib user32.lib gdi32.lib winspool.lib  \
                    comdlg32.lib advapi32.lib shell32.lib ole32.lib \
                    oleaut32.lib uuid.lib odbc32.lib odbccp32.lib 
else
    EXE_FLAGS     = /nologo
    EXE_CON_FLAGS = /subsystem:console /incremental:no /machine:I386 \
                    kernel32.lib user32.lib gdi32.lib winspool.lib   \
                    comdlg32.lib advapi32.lib shell32.lib ole32.lib  \
                    oleaut32.lib uuid.lib odbc32.lib odbccp32.lib
    EXE_COM_FLAGS = /subsystem:windows /dll /incremental:no /machine:I386 \
                    kernel32.lib user32.lib gdi32.lib winspool.lib        \
                    comdlg32.lib advapi32.lib shell32.lib ole32.lib       \
                    oleaut32.lib uuid.lib odbc32.lib odbccp32.lib
    EXE_DLL_FLAGS = /dll /incremental:no /machine:I386              \
                    kernel32.lib user32.lib gdi32.lib winspool.lib  \
                    comdlg32.lib advapi32.lib shell32.lib ole32.lib \
                    oleaut32.lib uuid.lib odbc32.lib odbccp32.lib 
endif
EXE_DLL_EXT = dll

############################################################
## [ MIDL ]
############################################################
MTL_PROG      = midl.exe
ifdef DEBUG
    MTL_FLAGS = /nologo /D "_DEBUG" /D "DEBUG"
else
    MTL_FLAGS = /nologo /D "NDEBUG"
endif
MTL_COM_FLAGS = /Oicf

############################################################
## [ RC ]
############################################################
RSC_PROG      = rc.exe
ifdef DEBUG
    RSC_FLAGS = /d "_DEBUG"
else
    RSC_FLAGS = /d "NDEBUG"
endif
RSC_COM_FLAGS = /l 0x412
RSC_DLL_FLAGS = -r

############################################################
## [ MC ]
############################################################
MC_PROG = mc.exe

###########################################################
## [ JAVA ]
############################################################
JNI_INCLUDED = win32
JNI_EXE_LIBS =
JAVA_PROG    = "$(JAVA_HOME)/bin/java" -Djava.library.path=.
JAVAC_PROG   = "$(JAVA_HOME)/bin/javac"
JAVAH_PROG   = "$(JAVA_HOME)/bin/javah"
JAVA_FLAGS   = -classpath . -d .
JAR_PROG     = "$(JAVA_HOME)/bin/jar"
JAR_FLAGS    = cvf
