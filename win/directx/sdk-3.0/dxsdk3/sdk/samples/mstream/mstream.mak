# Microsoft Visual C++ Generated NMAKE File, Format Version 2.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

!IF "$(CFG)" == ""
CFG=Win32 Debug
!MESSAGE No configuration specified.  Defaulting to Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "Win32 Release" && "$(CFG)" != "Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "MSTREAM.MAK" CFG="Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

################################################################################
# Begin Project
# PROP Target_Last_Scanned "Win32 Debug"
MTL=MkTypLib.exe
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "WinRel"
# PROP BASE Intermediate_Dir "WinRel"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "retail"
# PROP Intermediate_Dir "retail"
OUTDIR=.\retail
INTDIR=.\retail

ALL : $(OUTDIR)/MSTREAM.exe $(OUTDIR)/MSTREAM.bsc

$(OUTDIR) : 
    if not exist $(OUTDIR)/nul mkdir $(OUTDIR)

# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
# ADD BASE CPP /nologo /W3 /GX /YX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /FR /c
# ADD CPP /nologo /W3 /GX /YX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /FR /c
CPP_PROJ=/nologo /W3 /GX /YX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS"\
 /FR$(INTDIR)/ /Fp$(OUTDIR)/"MSTREAM.pch" /Fo$(INTDIR)/ /c 
CPP_OBJS=.\retail/
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
RSC_PROJ=/l 0x409 /fo$(INTDIR)/"MSTREAM.res" /d "NDEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o$(OUTDIR)/"MSTREAM.bsc" 
BSC32_SBRS= \
    $(INTDIR)/MSTRCONV.SBR \
    $(INTDIR)/MSTREAM.SBR \
    $(INTDIR)/DEBUG.SBR \
    $(INTDIR)/MSTRHELP.SBR

$(OUTDIR)/MSTREAM.bsc : $(OUTDIR)  $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /NOLOGO /SUBSYSTEM:windows /MACHINE:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib comdlg32.lib shell32.lib uuid.lib comctl32.lib winmm.lib /NOLOGO /SUBSYSTEM:windows /MACHINE:I386
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib comdlg32.lib shell32.lib\
 uuid.lib comctl32.lib winmm.lib /NOLOGO /SUBSYSTEM:windows /INCREMENTAL:no\
 /PDB:$(OUTDIR)/"MSTREAM.pdb" /MACHINE:I386 /DEF:".\MSTREAM.DEF"\
 /OUT:$(OUTDIR)/"MSTREAM.exe" 
DEF_FILE=.\MSTREAM.DEF
LINK32_OBJS= \
    $(INTDIR)/MSTRCONV.OBJ \
    $(INTDIR)/MSTREAM.OBJ \
    $(INTDIR)/DEBUG.OBJ \
    $(INTDIR)/MSTREAM.res \
    $(INTDIR)/MSTRHELP.OBJ

$(OUTDIR)/MSTREAM.exe : $(OUTDIR)  $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "WinDebug"
# PROP BASE Intermediate_Dir "WinDebug"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "debug"
# PROP Intermediate_Dir "debug"
OUTDIR=.\debug
INTDIR=.\debug

ALL : $(OUTDIR)/MSTREAM.exe $(OUTDIR)/MSTREAM.bsc

$(OUTDIR) : 
    if not exist $(OUTDIR)/nul mkdir $(OUTDIR)

# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /D "DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /D "DEBUG" /win32 
# ADD BASE CPP /nologo /W3 /GX /Zi /YX /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /FR /c
# ADD CPP /nologo /W3 /GX /Zi /YX /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "DEBUG" /FR /c
CPP_PROJ=/nologo /W3 /GX /Zi /YX /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D\
 "DEBUG" /FR$(INTDIR)/ /Fp$(OUTDIR)/"MSTREAM.pch" /Fo$(INTDIR)/\
 /Fd$(OUTDIR)/"MSTREAM.pdb" /c 
CPP_OBJS=.\debug/
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG" /d "DEBUG"
RSC_PROJ=/l 0x409 /fo$(INTDIR)/"MSTREAM.res" /d "_DEBUG" /d "DEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o$(OUTDIR)/"MSTREAM.bsc" 
BSC32_SBRS= \
    $(INTDIR)/MSTRCONV.SBR \
    $(INTDIR)/MSTREAM.SBR \
    $(INTDIR)/DEBUG.SBR \
    $(INTDIR)/MSTRHELP.SBR

$(OUTDIR)/MSTREAM.bsc : $(OUTDIR)  $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /NOLOGO /SUBSYSTEM:windows /DEBUG /MACHINE:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib comdlg32.lib shell32.lib uuid.lib comctl32.lib winmm.lib /NOLOGO /SUBSYSTEM:windows /INCREMENTAL:no /DEBUG /MACHINE:I386
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib comdlg32.lib shell32.lib\
 uuid.lib comctl32.lib winmm.lib /NOLOGO /SUBSYSTEM:windows /INCREMENTAL:no\
 /PDB:$(OUTDIR)/"MSTREAM.pdb" /DEBUG /MACHINE:I386 /DEF:".\MSTREAM.DEF"\
 /OUT:$(OUTDIR)/"MSTREAM.exe" 
DEF_FILE=.\MSTREAM.DEF
LINK32_OBJS= \
    $(INTDIR)/MSTRCONV.OBJ \
    $(INTDIR)/MSTREAM.OBJ \
    $(INTDIR)/DEBUG.OBJ \
    $(INTDIR)/MSTREAM.res \
    $(INTDIR)/MSTRHELP.OBJ

$(OUTDIR)/MSTREAM.exe : $(OUTDIR)  $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
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

SOURCE=.\MSTRCONV.C
DEP_MSTRC=\
    .\DEBUG.H\
    .\MSTREAM.H\
    .\MIDSTUFF.H

$(INTDIR)/MSTRCONV.OBJ :  $(SOURCE)  $(DEP_MSTRC) $(INTDIR)

# End Source File
################################################################################
# Begin Source File

SOURCE=.\MSTREAM.C
DEP_MSTRE=\
    \dev\inc\mmreg.h\
    .\DEBUG.H\
    .\MIDSTUFF.H\
    .\MSTREAM.H

$(INTDIR)/MSTREAM.OBJ :  $(SOURCE)  $(DEP_MSTRE) $(INTDIR)

# End Source File
################################################################################
# Begin Source File

SOURCE=.\MSTREAM.DEF
# End Source File
################################################################################
# Begin Source File

SOURCE=.\DEBUG.C
DEP_DEBUG=\
    .\DEBUG.H

$(INTDIR)/DEBUG.OBJ :  $(SOURCE)  $(DEP_DEBUG) $(INTDIR)

# End Source File
################################################################################
# Begin Source File

SOURCE=.\MSTREAM.RC
DEP_MSTREA=\
    .\ico00001.ico

$(INTDIR)/MSTREAM.res :  $(SOURCE)  $(DEP_MSTREA) $(INTDIR)
   $(RSC) $(RSC_PROJ)  $(SOURCE) 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\MSTRHELP.C
DEP_MSTRH=\
    \dev\inc\mmreg.h\
    .\DEBUG.H\
    .\MIDSTUFF.H\
    .\MSTREAM.H

$(INTDIR)/MSTRHELP.OBJ :  $(SOURCE)  $(DEP_MSTRH) $(INTDIR)

# End Source File
# End Group
# End Project
################################################################################
