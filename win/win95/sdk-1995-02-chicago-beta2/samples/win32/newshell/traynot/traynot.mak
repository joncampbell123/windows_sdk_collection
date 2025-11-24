# Microsoft Visual C++ generated build script - Do not modify

PROJ = TRAYNOT
DEBUG = 1
PROGTYPE = 0
CALLER = 
ARGS = 
DLLS = 
ORIGIN = MSVCNT
ORIGIN_VER = 1.00
USEMFC = 0
CC = cl
CPP = cl
CXX = cl
CCREATEPCHFLAG = 
CPPCREATEPCHFLAG = 
CUSEPCHFLAG = 
CPPUSEPCHFLAG = 
FIRSTC = APP32.C
FIRSTCPP = 
RC = rc
CFLAGS_D_WEXE32 = /W3 /Zi /YX /D "_DEBUG" /D "_X86_" /D "_WINDOWS" /D "WIN32" /FR /ML /Fd"TRAYNOT.PDB"  /Fp"TRAYNOT.PCH"
CFLAGS_R_WEXE32 = /W3 /YX /O2 /D "NDEBUG" /D "_X86_" /D "_WINDOWS" /D "WIN32" /FR /ML /Fp"TRAYNOT.PCH"
LFLAGS_D_WEXE32 = /NOLOGO /DEBUG /DEBUGTYPE:cv /SUBSYSTEM:windows user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib olecli32.lib olesvr32.lib shell32.lib comctl32.lib
LFLAGS_R_WEXE32 = /NOLOGO /SUBSYSTEM:windows user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib olecli32.lib olesvr32.lib shell32.lib comctl32.lib
LFLAGS_D_LIB32 = /NOLOGO
LFLAGS_R_LIB32 = /NOLOGO
LIBS_D_WEXE32 = 
LIBS_R_WEXE32 = 
RCFLAGS32 = /dWIN32 
D_RCDEFINES32 = /d_DEBUG 
R_RCDEFINES32 = /dNDEBUG 
OBJS_EXT = 
LIBS_EXT = 
!if "$(DEBUG)" == "1"
CFLAGS = $(CFLAGS_D_WEXE32)
LFLAGS = $(LFLAGS_D_WEXE32)
LIBS = $(LIBS_D_WEXE32)
LFLAGS_LIB=$(LFLAGS_D_LIB32)
MAPFILE_OPTION = 
RCDEFINES = $(D_RCDEFINES32)
!else
CFLAGS = $(CFLAGS_R_WEXE32)
LFLAGS = $(LFLAGS_R_WEXE32)
LIBS = $(LIBS_R_WEXE32)
MAPFILE_OPTION = 
LFLAGS_LIB=$(LFLAGS_R_LIB32)
RCDEFINES = $(R_RCDEFINES32)
!endif
SBRS = APP32.SBR


APP32_DEP =  \
	app32.h


APP32_RCDEP =  \
	app32.h \
	app32.ico \
	state11.ico \
	state12.ico \
	state21.ico \
	state22.ico \
	state31.ico \
        state32.ico \
	app32.rcv


all:	$(PROJ).EXE $(PROJ).BSC

APP32.OBJ:	APP32.C $(APP32_DEP)
	$(CC) $(CFLAGS) $(CCREATEPCHFLAG) /c APP32.C

APP32.RES:	APP32.RC $(APP32_RCDEP)
	$(RC) $(RCFLAGS32) $(RCDEFINES) -r APP32.RC


$(PROJ).EXE:	APP32.RES

$(PROJ).EXE:	APP32.OBJ $(OBJS_EXT) $(LIBS_EXT)
	echo >NUL @<<$(PROJ).CRF
$(LFLAGS)
APP32.OBJ
$(OBJS_EXT)
-OUT:$(PROJ).EXE
$(MAPFILE_OPTION)
APP32.RES
$(LIBS)
$(LIBS_EXT)
$(DEFFILE_OPTION) -implib:$(PROJ).lib
<<
	link @$(PROJ).CRF

run: $(PROJ).EXE
	$(PROJ) $(RUNFLAGS)


$(PROJ).BSC: $(SBRS)
	bscmake @<<
/o$@ $(SBRS)
<<
