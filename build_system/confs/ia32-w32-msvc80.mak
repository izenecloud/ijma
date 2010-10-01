############################################################
## [ CONFIGURATION INFORMATION ]
############################################################
## Architecture     : Intel/AMD (x64)
## Operation System : Windows
## Compiler Version : MS Visual Studio 8.0 (2005)
## Compile          : 32bit
############################################################

############################################################
## [ ENVIRONMENT ]
############################################################
ifeq (,$(findstring /cygdrive/c/Program Files/Microsoft Visual Studio 8/Common7/IDE:/cygdrive/c/Program Files/Microsoft Visual Studio 8/Common7/Tools:/cygdrive/c/Program Files/Microsoft Visual Studio 8/Common7/Tools/bin:/cygdrive/c/Program Files/Microsoft Visual Studio 8/VC/BIN:/cygdrive/c/Program Files/Microsoft Visual Studio 8/VC/VCPackages:/cygdrive/c/Program Files/Microsoft Visual Studio 8/VC/PlatformSDK/bin:/cygdrive/c/Program Files/Microsoft Visual Studio 8/SDK/v2.0/bin:/cygdrive/c/WINDOWS/Microsoft.NET/Framework/v2.0.50727,$(PATH)))
ENVIRONMENT_ERROR=true
endif
ifeq (,$(findstring C:\Program Files\Microsoft Visual Studio 8\VC\ATLMFC\INCLUDE;C:\Program Files\Microsoft Visual Studio 8\VC\INCLUDE;C:\Program Files\Microsoft Visual Studio 8\VC\PlatformSDK\include;C:\Program Files\Microsoft Visual Studio 8\SDK\v2.0\include,$(INCLUDE)))
ENVIRONMENT_ERROR=true
endif
ifeq (,$(findstring C:\Program Files\Microsoft Visual Studio 8\VC\ATLMFC\LIB;C:\Program Files\Microsoft Visual Studio 8\VC\LIB;C:\Program Files\Microsoft Visual Studio 8\VC\PlatformSDK\lib;C:\Program Files\Microsoft Visual Studio 8\SDK\v2.0\lib,$(LIB)))
ENVIRONMENT_ERROR=true
endif
ifeq (,$(findstring C:\Program Files\Microsoft Visual Studio 8\VC\ATLMFC\LIB,$(LIBPATH)))
ENVIRONMENT_ERROR=true
endif

ifeq ($(ENVIRONMENT_ERROR),true)
$(error no environment variables related to visual studio 2005. please execute following command: source $(SF1_SRC)/build_system/confs/$(SF1_BUILD).env)
endif

############################################################
## [ MAKEFILE ]
############################################################
MAKE_FILE = Makefile.win

############################################################
## [ LIBRARY ]
############################################################
LIB_PROG  = lib.exe
ifdef DEBUG
    LIB_FLAGS = /nologo
else
    LIB_FLAGS = /nologo /LTCG
endif # DEBUG

############################################################
## [ COMPILER ]
############################################################
CC_PROG          = cl.exe
ifdef DEBUG
    CC_FLAGS     = /nologo /Od /Gm /EHsc /RTC1 /MTd /W2 /Wp64 /ZI /TP /errorReport:prompt /c \
                   /D "WIN32" /D "_DEBUG" /D "DEBUG" /D "_MBCS" /D "_CRT_SECURE_NO_DEPRECATE"
    CC_LIB_FLAGS = /D "_LIB"
    CC_CON_FLAGS = /D "_CONSOLE"
    CC_COM_FLAGS = /D "_WINDOWS" /D "_USRDLL" /D "_WINDLL" /D "_ATL_DLL"
	#CC_DLL_FLAGS = /D "_USRDLL"
else
    CC_FLAGS     = /nologo /O2 /GL /FD /EHsc /MT /W3 /Wp64 /Zi /TP /errorReport:prompt /c \
                   /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_CRT_SECURE_NO_DEPRECATE"
    CC_LIB_FLAGS = /D "_LIB"
    CC_CON_FLAGS = /D "_CONSOLE"
    CC_COM_FLAGS = /D "_WINDOWS" /D "_USRDLL" /D "_WINDLL" /D "_ATL_DLL"
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
    EXE_FLAGS     = /nologo /DEBUG
    EXE_CON_FLAGS = /SUBSYSTEM:CONSOLE /INCREMENTAL /MANIFEST /MACHINE:X86 /ERRORREPORT:PROMPT \
                    kernel32.lib user32.lib gdi32.lib winspool.lib   \
                    comdlg32.lib advapi32.lib shell32.lib ole32.lib  \
                    oleaut32.lib uuid.lib odbc32.lib odbccp32.lib
    EXE_COM_FLAGS = /SUBSYSTEM:WINDOWS /DLL /INCREMENTAL /MANIFEST /MACHINE:X86 /ERRORREPORT:PROMPT \
                    kernel32.lib user32.lib gdi32.lib winspool.lib        \
                    comdlg32.lib advapi32.lib shell32.lib ole32.lib       \
                    oleaut32.lib uuid.lib odbc32.lib odbccp32.lib
    EXE_DLL_FLAGS = /DLL /INCREMENTAL /MANIFEST /MACHINE:X86 /ERRORREPORT:PROMPT \
                    kernel32.lib user32.lib gdi32.lib winspool.lib  \
                    comdlg32.lib advapi32.lib shell32.lib ole32.lib \
                    oleaut32.lib uuid.lib odbc32.lib odbccp32.lib 
else
    EXE_FLAGS     = /nologo
    EXE_CON_FLAGS = /SUBSYSTEM:CONSOLE /INCREMENTAL:NO /MANIFEST /OPT:REF /OPT:ICF /LTCG /MACHINE:X86 /ERRORREPORT:PROMPT \
                    kernel32.lib user32.lib gdi32.lib winspool.lib   \
                    comdlg32.lib advapi32.lib shell32.lib ole32.lib  \
                    oleaut32.lib uuid.lib odbc32.lib odbccp32.lib
    EXE_COM_FLAGS = /SUBSYSTEM:WINDOWS /DLL /INCREMENTAL:NO /MANIFEST /OPT:REF /OPT:ICF /MACHINE:X86 /ERRORREPORT:PROMPT \
                    kernel32.lib user32.lib gdi32.lib winspool.lib        \
                    comdlg32.lib advapi32.lib shell32.lib ole32.lib       \
                    oleaut32.lib uuid.lib odbc32.lib odbccp32.lib
    EXE_DLL_FLAGS = /DLL /INCREMENTAL:NO /MANIFEST /OPT:REF /OPT:ICF /MACHINE:X86 /ERRORREPORT:PROMPT \
                    kernel32.lib user32.lib gdi32.lib winspool.lib  \
                    comdlg32.lib advapi32.lib shell32.lib ole32.lib \
                    oleaut32.lib uuid.lib odbc32.lib odbccp32.lib 
endif
EXE_DLL_EXT = dll

############################################################
## [ MANIFEST ]
############################################################
MT_PROG      = mt.exe
ifdef DEBUG
    MT_FLAGS = /nologo /notify_update
else
    MT_FLAGS = /nologo
endif

############################################################
## [ MIDL ]
############################################################
MTL_PROG      = midl.exe
ifdef DEBUG
    MTL_FLAGS = /nologo /char signed /env win32 /no_robust /D "_DEBUG" /D "DEBUG"
else
    MTL_FLAGS = /nologo /char signed /env win32 /no_robust /D "NDEBUG"
endif
MTL_COM_FLAGS = /Oicf

############################################################
## [ RC ]
############################################################
RSC_PROG      = rc.exe
ifdef DEBUG
    RSC_FLAGS = /D "_DEBUG"
else
    RSC_FLAGS = /D "NDEBUG"
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
