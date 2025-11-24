# Microsoft Visual C++ generated build script - Do not modify

PROJ = FONTVIEW
DEBUG = 1
PROGTYPE = 0
CALLER = 
ARGS = 
DLLS = 
ORIGIN = MSVCNT
ORIGIN_VER = 1.00
PROJPATH = UNKNOWN 
USEMFC = 0
CC = cl
CPP = cl
CXX = cl
CCREATEPCHFLAG = 
CPPCREATEPCHFLAG = 
CUSEPCHFLAG = 
CPPUSEPCHFLAG = 
FIRSTC = DIALOGS.C
FIRSTCPP = 
RC = rc
CFLAGS_D_WEXE32 = /nologo /W3 /Zi /YX /D "_X86_" /D "_DEBUG" /D "_WINDOWS" /D "WIN32" /FR /ML /Fd"FONTVIEW.PDB" /Fp"FONTVIEW.PCH"
CFLAGS_R_WEXE32 = /nologo /W3 /YX /O2 /D "_X86_" /D "NDEBUG" /D "_WINDOWS" /FR /ML /Fp"FONTVIEW.PCH"
LFLAGS_D_WEXE32 = /NOLOGO /DEBUG /DEBUGTYPE:cv /SUBSYSTEM:windows user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib olecli32.lib olesvr32.lib shell32.lib
LFLAGS_R_WEXE32 = /NOLOGO /SUBSYSTEM:windows user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib olecli32.lib olesvr32.lib shell32.lib
LFLAGS_D_LIB32 = /NOLOGO
LFLAGS_R_LIB32 = /NOLOGO
LIBS_D_WEXE32 = 
LIBS_R_WEXE32 = 
RCFLAGS32 = 
D_RCDEFINES32 = /d_DEBUG /d, /dWIN32
R_RCDEFINES32 = /dNDEBUG /d, /dWIN32
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
SBRS = DIALOGS.SBR \
		DISPLAY.SBR \
		FONTVIEW.SBR \
		STATUS.SBR \
		TOOLS.SBR


DIALOGS_DEP =  \
	                  .\fontview.h


DISPLAY_DEP = 

FONTVIEW_DEP =  \
	                  .\fontview.h


FONTVIEW_RCDEP =  \
	                  .\fontview.h \
	                  .\fontview.ico \
	                  .\fontview.bmp \
	                  .\zoom.ico \
	                  .\fontview.dlg


STATUS_DEP = 

TOOLS_DEP = 

all:	$(PROJ).EXE $(PROJ).BSC

DIALOGS.OBJ:	DIALOGS.C $(DIALOGS_DEP)
	$(CC) $(CFLAGS) $(CCREATEPCHFLAG) /c DIALOGS.C

DISPLAY.OBJ:	DISPLAY.C $(DISPLAY_DEP)
	$(CC) $(CFLAGS) $(CUSEPCHFLAG) /c DISPLAY.C

FONTVIEW.OBJ:	FONTVIEW.C $(FONTVIEW_DEP)
	$(CC) $(CFLAGS) $(CUSEPCHFLAG) /c FONTVIEW.C

FONTVIEW.RES:	FONTVIEW.RC $(FONTVIEW_RCDEP)
	$(RC) $(RCFLAGS32) $(RCDEFINES) -r FONTVIEW.RC

STATUS.OBJ:	STATUS.C $(STATUS_DEP)
	$(CC) $(CFLAGS) $(CUSEPCHFLAG) /c STATUS.C

TOOLS.OBJ:	TOOLS.C $(TOOLS_DEP)
	$(CC) $(CFLAGS) $(CUSEPCHFLAG) /c TOOLS.C


$(PROJ).EXE:	FONTVIEW.RES

$(PROJ).EXE:	DIALOGS.OBJ DISPLAY.OBJ FONTVIEW.OBJ STATUS.OBJ TOOLS.OBJ $(OBJS_EXT) $(LIBS_EXT)
	echo >NUL @<<$(PROJ).CRF
DIALOGS.OBJ 
DISPLAY.OBJ 
FONTVIEW.OBJ 
STATUS.OBJ 
TOOLS.OBJ 
$(OBJS_EXT)
-OUT:$(PROJ).EXE
$(MAPFILE_OPTION)
FONTVIEW.RES
$(LIBS)
$(LIBS_EXT)
$(DEFFILE_OPTION) -implib:$(PROJ).lib
<<
	link $(LFLAGS) @$(PROJ).CRF

run: $(PROJ).EXE
	$(PROJ) $(RUNFLAGS)


$(PROJ).BSC: $(SBRS)
	bscmake @<<
/o$@ $(SBRS)
<<
