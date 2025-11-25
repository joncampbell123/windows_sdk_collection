# Microsoft Visual C++ Generated NMAKE File, Format Version 2.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

!IF "$(CFG)" == ""
CFG=Win32 Debug
!MESSAGE No configuration specified.  Defaulting to Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "Win32 Release" && "$(CFG)" != "Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "CGUTIL.MAK" CFG="Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

################################################################################
# Begin Project
# PROP Target_Last_Scanned "Win32 Debug"
CPP=cl.exe

!IF  "$(CFG)" == "Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "WinRel"
# PROP BASE Intermediate_Dir "WinRel"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "WinRel"
# PROP Intermediate_Dir "WinRel"
OUTDIR=.\WinRel
INTDIR=.\WinRel

ALL : ..\lib\retail\cgutil.lib $(OUTDIR)/CGUTIL.bsc

$(OUTDIR) : 
    if not exist $(OUTDIR)/nul mkdir $(OUTDIR)

# ADD BASE CPP /nologo /W3 /GX /YX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /FR /c
# ADD CPP /nologo /G3 /W3 /WX /GX /YX /O2 /I "..\include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /FR /c
CPP_PROJ=/nologo /G3 /W3 /WX /GX /YX /O2 /I "..\include" /D "WIN32" /D "NDEBUG"\
 /D "_WINDOWS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"CGUTIL.pch" /Fo$(INTDIR)/ /c 
CPP_OBJS=.\WinRel/
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o$(OUTDIR)/"CGUTIL.bsc" 
BSC32_SBRS= \
    $(INTDIR)/CGEXCPT.SBR \
    $(INTDIR)/CGRES.SBR \
    $(INTDIR)/STRREC.SBR

$(OUTDIR)/CGUTIL.bsc : $(OUTDIR)  $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LIB32=lib.exe
# ADD BASE LIB32 /NOLOGO
# ADD LIB32 /NOLOGO /OUT:"..\lib\retail\cgutil.lib"
LIB32_FLAGS=/NOLOGO /OUT:"..\lib\retail\cgutil.lib" 
DEF_FLAGS=
DEF_FILE=
LIB32_OBJS= \
    $(INTDIR)/CGEXCPT.OBJ \
    $(INTDIR)/CGRES.OBJ \
    $(INTDIR)/STRREC.OBJ

..\lib\retail\cgutil.lib : $(OUTDIR)  $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ELSEIF  "$(CFG)" == "Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "WinDebug"
# PROP BASE Intermediate_Dir "WinDebug"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "WinDebug"
# PROP Intermediate_Dir "WinDebug"
OUTDIR=.\WinDebug
INTDIR=.\WinDebug

ALL : ..\LIB\DEBUG\CGUTIL.LIB $(OUTDIR)/CGUTIL.bsc

$(OUTDIR) : 
    if not exist $(OUTDIR)/nul mkdir $(OUTDIR)

# ADD BASE CPP /nologo /W3 /GX /Z7 /YX /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /FR /c
# ADD CPP /nologo /G3 /W3 /WX /GX /Z7 /YX /Od /I "..\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /FR /c
CPP_PROJ=/nologo /G3 /W3 /WX /GX /Z7 /YX /Od /I "..\include" /D "WIN32" /D\
 "_DEBUG" /D "_WINDOWS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"CGUTIL.pch" /Fo$(INTDIR)/ /c\
 
CPP_OBJS=.\WinDebug/
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o$(OUTDIR)/"CGUTIL.bsc" 
BSC32_SBRS= \
    $(INTDIR)/CGEXCPT.SBR \
    $(INTDIR)/CGRES.SBR \
    $(INTDIR)/STRREC.SBR

$(OUTDIR)/CGUTIL.bsc : $(OUTDIR)  $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LIB32=lib.exe
# ADD BASE LIB32 /NOLOGO
# ADD LIB32 /NOLOGO /OUT:"..\lib\debug\cgutil.lib"
LIB32_FLAGS=/NOLOGO /OUT:"..\lib\debug\cgutil.lib" 
DEF_FLAGS=
DEF_FILE=
LIB32_OBJS= \
    $(INTDIR)/CGEXCPT.OBJ \
    $(INTDIR)/CGRES.OBJ \
    $(INTDIR)/STRREC.OBJ

..\LIB\DEBUG\CGUTIL.LIB : $(OUTDIR)  $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ENDIF 

.c{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

################################################################################
# Begin Group "Source Files"

################################################################################
# Begin Source File

SOURCE=.\CGEXCPT.CPP
DEP_CGEXC=\
    ..\include\cgres.h\
    ..\include\cgexcpt.h

$(INTDIR)/CGEXCPT.OBJ :  $(SOURCE)  $(DEP_CGEXC) $(INTDIR)

# End Source File
################################################################################
# Begin Source File

SOURCE=.\CGRES.CPP
DEP_CGRES=\
    ..\include\cgexcpt.h\
    ..\include\cgres.h

$(INTDIR)/CGRES.OBJ :  $(SOURCE)  $(DEP_CGRES) $(INTDIR)

# End Source File
################################################################################
# Begin Source File

SOURCE=.\STRREC.CPP
DEP_STRRE=\
    ..\include\strrec.h

$(INTDIR)/STRREC.OBJ :  $(SOURCE)  $(DEP_STRRE) $(INTDIR)

# End Source File
# End Group
# End Project
################################################################################
