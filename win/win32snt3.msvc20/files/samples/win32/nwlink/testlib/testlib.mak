# Microsoft Visual C++ Generated NMAKE File, Format Version 2.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (MIPS) Static Library" 0x0504
# TARGTYPE "Win32 (x86) Static Library" 0x0104

!IF "$(CFG)" == ""
CFG=Win32 (80x86) Debug
!MESSAGE No configuration specified.  Defaulting to Win32 (80x86) Debug.
!ENDIF 

!IF "$(CFG)" != "Win32 (80x86) Release" && "$(CFG)" != "Win32 (80x86) Debug" &&\
 "$(CFG)" != "Win32 (MIPS) Release" && "$(CFG)" != "Win32 (MIPS) Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "testlib.mak" CFG="Win32 (80x86) Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Win32 (80x86) Release" (based on "Win32 (x86) Static Library")
!MESSAGE "Win32 (80x86) Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "Win32 (MIPS) Release" (based on "Win32 (MIPS) Static Library")
!MESSAGE "Win32 (MIPS) Debug" (based on "Win32 (MIPS) Static Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

################################################################################
# Begin Project
# PROP Target_Last_Scanned "Win32 (MIPS) Release"

!IF  "$(CFG)" == "Win32 (80x86) Release"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "WinRel"
# PROP BASE Intermediate_Dir "WinRel"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "WinRel"
# PROP Intermediate_Dir "WinRel"
OUTDIR=.\WinRel
INTDIR=.\WinRel

ALL : .\testlib.lib $(OUTDIR)/testlib.bsc

$(OUTDIR) : 
    if not exist $(OUTDIR)/nul mkdir $(OUTDIR)

CPP=cl.exe
# ADD BASE CPP /nologo /MD /W3 /GX /YX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /c
# ADD CPP /nologo /W3 /GX /YX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /FR /c
CPP_PROJ=/nologo /W3 /GX /YX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D\
 "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"testlib.pch" /Fo$(INTDIR)/ /c 
CPP_OBJS=.\WinRel/

.c{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# SUBTRACT BASE BSC32 /Iu
# ADD BSC32 /nologo
# SUBTRACT BSC32 /Iu
BSC32_FLAGS=/nologo /o$(OUTDIR)/"testlib.bsc" 
BSC32_SBRS= \
	$(INTDIR)/errmsg.sbr \
	$(INTDIR)/geterror.sbr \
	$(INTDIR)/dstrerr.sbr \
	$(INTDIR)/netprint.sbr \
	$(INTDIR)/dperror.sbr \
	$(INTDIR)/wperror.sbr \
	$(INTDIR)/memory.sbr \
	$(INTDIR)/cmdline.sbr

$(OUTDIR)/testlib.bsc : $(OUTDIR)  $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LIB32=lib.exe
# ADD BASE LIB32 -NOLOGO
# ADD LIB32 -NOLOGO /OUT:"testlib.lib"
LIB32_FLAGS=-NOLOGO /OUT:"testlib.lib" 
DEF_FLAGS=
DEF_FILE=
LIB32_OBJS= \
	$(INTDIR)/errmsg.obj \
	$(INTDIR)/geterror.obj \
	$(INTDIR)/dstrerr.obj \
	$(INTDIR)/netprint.obj \
	$(INTDIR)/dperror.obj \
	$(INTDIR)/wperror.obj \
	$(INTDIR)/memory.obj \
	$(INTDIR)/cmdline.obj

.\testlib.lib : $(OUTDIR)  $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "WinDebug"
# PROP BASE Intermediate_Dir "WinDebug"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "WinDebug"
# PROP Intermediate_Dir "WinDebug"
OUTDIR=.\WinDebug
INTDIR=.\WinDebug

ALL : .\testlib.lib $(OUTDIR)/testlib.bsc

$(OUTDIR) : 
    if not exist $(OUTDIR)/nul mkdir $(OUTDIR)

CPP=cl.exe
# ADD BASE CPP /nologo /MD /W3 /GX /Z7 /YX /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /c
# ADD CPP /nologo /W3 /GX /Z7 /YX /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /FR /c
CPP_PROJ=/nologo /W3 /GX /Z7 /YX /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D\
 "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"testlib.pch" /Fo$(INTDIR)/ /c 
CPP_OBJS=.\WinDebug/

.c{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# SUBTRACT BASE BSC32 /Iu
# ADD BSC32 /nologo
# SUBTRACT BSC32 /Iu
BSC32_FLAGS=/nologo /o$(OUTDIR)/"testlib.bsc" 
BSC32_SBRS= \
	$(INTDIR)/errmsg.sbr \
	$(INTDIR)/geterror.sbr \
	$(INTDIR)/dstrerr.sbr \
	$(INTDIR)/netprint.sbr \
	$(INTDIR)/dperror.sbr \
	$(INTDIR)/wperror.sbr \
	$(INTDIR)/memory.sbr \
	$(INTDIR)/cmdline.sbr

$(OUTDIR)/testlib.bsc : $(OUTDIR)  $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LIB32=lib.exe
# ADD BASE LIB32 -NOLOGO
# ADD LIB32 -NOLOGO /OUT:"testlib.lib"
LIB32_FLAGS=-NOLOGO /OUT:"testlib.lib" 
DEF_FLAGS=
DEF_FILE=
LIB32_OBJS= \
	$(INTDIR)/errmsg.obj \
	$(INTDIR)/geterror.obj \
	$(INTDIR)/dstrerr.obj \
	$(INTDIR)/netprint.obj \
	$(INTDIR)/dperror.obj \
	$(INTDIR)/wperror.obj \
	$(INTDIR)/memory.obj \
	$(INTDIR)/cmdline.obj

.\testlib.lib : $(OUTDIR)  $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Win32__M"
# PROP BASE Intermediate_Dir "Win32__M"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "WinRel"
# PROP Intermediate_Dir "WinRel"
OUTDIR=.\WinRel
INTDIR=.\WinRel

ALL : .\testlib.lib $(OUTDIR)/testlib.bsc

$(OUTDIR) : 
    if not exist $(OUTDIR)/nul mkdir $(OUTDIR)

CPP=cl.exe
# ADD BASE CPP /nologo /MD /Gt0 /QMOb2000 /W3 /GX /YX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /c
# ADD CPP /nologo /Gt0 /QMOb2000 /W3 /GX /YX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /FR /c
CPP_PROJ=/nologo /Gt0 /QMOb2000 /W3 /GX /YX /O2 /D "NDEBUG" /D "WIN32" /D\
 "_WINDOWS" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"testlib.pch" /Fo$(INTDIR)/ /c\
 
CPP_OBJS=.\WinRel/

.c{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# SUBTRACT BASE BSC32 /Iu
# ADD BSC32 /nologo
# SUBTRACT BSC32 /Iu
BSC32_FLAGS=/nologo /o$(OUTDIR)/"testlib.bsc" 
BSC32_SBRS= \
	$(INTDIR)/errmsg.sbr \
	$(INTDIR)/geterror.sbr \
	$(INTDIR)/dstrerr.sbr \
	$(INTDIR)/netprint.sbr \
	$(INTDIR)/dperror.sbr \
	$(INTDIR)/wperror.sbr \
	$(INTDIR)/memory.sbr \
	$(INTDIR)/cmdline.sbr

$(OUTDIR)/testlib.bsc : $(OUTDIR)  $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LIB32=lib.exe
# ADD BASE LIB32 -NOLOGO
# ADD LIB32 -NOLOGO /OUT:"testlib.lib"
LIB32_FLAGS=-NOLOGO /OUT:"testlib.lib" 
DEF_FLAGS=
DEF_FILE=
LIB32_OBJS= \
	$(INTDIR)/errmsg.obj \
	$(INTDIR)/geterror.obj \
	$(INTDIR)/dstrerr.obj \
	$(INTDIR)/netprint.obj \
	$(INTDIR)/dperror.obj \
	$(INTDIR)/wperror.obj \
	$(INTDIR)/memory.obj \
	$(INTDIR)/cmdline.obj

.\testlib.lib : $(OUTDIR)  $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Win32__0"
# PROP BASE Intermediate_Dir "Win32__0"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "WinDebug"
# PROP Intermediate_Dir "WinDebug"
OUTDIR=.\WinDebug
INTDIR=.\WinDebug

ALL : .\testlib.lib $(OUTDIR)/testlib.bsc

$(OUTDIR) : 
    if not exist $(OUTDIR)/nul mkdir $(OUTDIR)

CPP=cl.exe
# ADD BASE CPP /nologo /MD /Gt0 /QMOb2000 /W3 /GX /Z7 /YX /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /c
# ADD CPP /nologo /Gt0 /QMOb2000 /W3 /GX /Z7 /YX /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /FR /c
CPP_PROJ=/nologo /Gt0 /QMOb2000 /W3 /GX /Z7 /YX /Od /D "_DEBUG" /D "WIN32" /D\
 "_WINDOWS" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"testlib.pch" /Fo$(INTDIR)/ /c\
 
CPP_OBJS=.\WinDebug/

.c{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# SUBTRACT BASE BSC32 /Iu
# ADD BSC32 /nologo
# SUBTRACT BSC32 /Iu
BSC32_FLAGS=/nologo /o$(OUTDIR)/"testlib.bsc" 
BSC32_SBRS= \
	$(INTDIR)/errmsg.sbr \
	$(INTDIR)/geterror.sbr \
	$(INTDIR)/dstrerr.sbr \
	$(INTDIR)/netprint.sbr \
	$(INTDIR)/dperror.sbr \
	$(INTDIR)/wperror.sbr \
	$(INTDIR)/memory.sbr \
	$(INTDIR)/cmdline.sbr

$(OUTDIR)/testlib.bsc : $(OUTDIR)  $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LIB32=lib.exe
# ADD BASE LIB32 -NOLOGO
# ADD LIB32 -NOLOGO /OUT:"testlib.lib"
LIB32_FLAGS=-NOLOGO /OUT:"testlib.lib" 
DEF_FLAGS=
DEF_FILE=
LIB32_OBJS= \
	$(INTDIR)/errmsg.obj \
	$(INTDIR)/geterror.obj \
	$(INTDIR)/dstrerr.obj \
	$(INTDIR)/netprint.obj \
	$(INTDIR)/dperror.obj \
	$(INTDIR)/wperror.obj \
	$(INTDIR)/memory.obj \
	$(INTDIR)/cmdline.obj

.\testlib.lib : $(OUTDIR)  $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ENDIF 

################################################################################
# Begin Group "Source Files"

################################################################################
# Begin Source File

SOURCE=.\errmsg.c
DEP_ERRMS=\
	.\externs.h\
	.\testlib.h

!IF  "$(CFG)" == "Win32 (80x86) Release"

$(INTDIR)/errmsg.obj :  $(SOURCE)  $(DEP_ERRMS) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

$(INTDIR)/errmsg.obj :  $(SOURCE)  $(DEP_ERRMS) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

$(INTDIR)/errmsg.obj :  $(SOURCE)  $(DEP_ERRMS) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

$(INTDIR)/errmsg.obj :  $(SOURCE)  $(DEP_ERRMS) $(INTDIR)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\geterror.c
DEP_GETER=\
	.\externs.h\
	.\testlib.h

!IF  "$(CFG)" == "Win32 (80x86) Release"

$(INTDIR)/geterror.obj :  $(SOURCE)  $(DEP_GETER) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

$(INTDIR)/geterror.obj :  $(SOURCE)  $(DEP_GETER) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

$(INTDIR)/geterror.obj :  $(SOURCE)  $(DEP_GETER) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

$(INTDIR)/geterror.obj :  $(SOURCE)  $(DEP_GETER) $(INTDIR)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\dstrerr.c
DEP_DSTRE=\
	.\externs.h\
	.\testlib.h

!IF  "$(CFG)" == "Win32 (80x86) Release"

$(INTDIR)/dstrerr.obj :  $(SOURCE)  $(DEP_DSTRE) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

$(INTDIR)/dstrerr.obj :  $(SOURCE)  $(DEP_DSTRE) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

$(INTDIR)/dstrerr.obj :  $(SOURCE)  $(DEP_DSTRE) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

$(INTDIR)/dstrerr.obj :  $(SOURCE)  $(DEP_DSTRE) $(INTDIR)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\netprint.c

!IF  "$(CFG)" == "Win32 (80x86) Release"

$(INTDIR)/netprint.obj :  $(SOURCE)  $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

$(INTDIR)/netprint.obj :  $(SOURCE)  $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

$(INTDIR)/netprint.obj :  $(SOURCE)  $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

$(INTDIR)/netprint.obj :  $(SOURCE)  $(INTDIR)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\dperror.c
DEP_DPERR=\
	.\externs.h\
	.\testlib.h

!IF  "$(CFG)" == "Win32 (80x86) Release"

$(INTDIR)/dperror.obj :  $(SOURCE)  $(DEP_DPERR) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

$(INTDIR)/dperror.obj :  $(SOURCE)  $(DEP_DPERR) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

$(INTDIR)/dperror.obj :  $(SOURCE)  $(DEP_DPERR) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

$(INTDIR)/dperror.obj :  $(SOURCE)  $(DEP_DPERR) $(INTDIR)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\wperror.c
DEP_WPERR=\
	.\externs.h\
	.\testlib.h

!IF  "$(CFG)" == "Win32 (80x86) Release"

$(INTDIR)/wperror.obj :  $(SOURCE)  $(DEP_WPERR) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

$(INTDIR)/wperror.obj :  $(SOURCE)  $(DEP_WPERR) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

$(INTDIR)/wperror.obj :  $(SOURCE)  $(DEP_WPERR) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

$(INTDIR)/wperror.obj :  $(SOURCE)  $(DEP_WPERR) $(INTDIR)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\memory.c
DEP_MEMOR=\
	.\externs.h\
	.\testlib.h

!IF  "$(CFG)" == "Win32 (80x86) Release"

$(INTDIR)/memory.obj :  $(SOURCE)  $(DEP_MEMOR) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

$(INTDIR)/memory.obj :  $(SOURCE)  $(DEP_MEMOR) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

$(INTDIR)/memory.obj :  $(SOURCE)  $(DEP_MEMOR) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

$(INTDIR)/memory.obj :  $(SOURCE)  $(DEP_MEMOR) $(INTDIR)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\cmdline.c
DEP_CMDLI=\
	.\externs.h\
	.\testlib.h

!IF  "$(CFG)" == "Win32 (80x86) Release"

$(INTDIR)/cmdline.obj :  $(SOURCE)  $(DEP_CMDLI) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

$(INTDIR)/cmdline.obj :  $(SOURCE)  $(DEP_CMDLI) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

$(INTDIR)/cmdline.obj :  $(SOURCE)  $(DEP_CMDLI) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

$(INTDIR)/cmdline.obj :  $(SOURCE)  $(DEP_CMDLI) $(INTDIR)

!ENDIF 

# End Source File
# End Group
# End Project
################################################################################
