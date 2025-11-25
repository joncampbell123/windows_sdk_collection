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
!MESSAGE NMAKE /f "SPLASH.MAK" CFG="Win32 Debug"
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
# PROP Output_Dir "WinRel"
# PROP Intermediate_Dir "WinRel"
OUTDIR=.\WinRel
INTDIR=.\WinRel

ALL : $(OUTDIR)/SPLASH.exe $(OUTDIR)/SPLASH.bsc

$(OUTDIR) : 
    if not exist $(OUTDIR)/nul mkdir $(OUTDIR)

# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
# ADD BASE CPP /nologo /W3 /GX /YX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /FR /c
# ADD CPP /nologo /W3 /GX /YX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /FR /c
CPP_PROJ=/nologo /W3 /GX /YX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS"\
 /FR$(INTDIR)/ /Fp$(OUTDIR)/"SPLASH.pch" /Fo$(INTDIR)/ /c 
CPP_OBJS=.\WinRel/
# ADD BASE RSC  /d "NDEBUG"
# ADD RSC /d "NDEBUG"
RSC_PROJ= /l 0x409 /fo$(INTDIR)/"SPLASH.res" /d "NDEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o$(OUTDIR)/"SPLASH.bsc" 
BSC32_SBRS= \
    $(INTDIR)/SPLASH.SBR

$(OUTDIR)/SPLASH.bsc : $(OUTDIR)  $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /NOLOGO /SUBSYSTEM:windows,4.0 /MACHINE:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib uuid.lib winmm.lib /NOLOGO /SUBSYSTEM:windows,4.0 /MACHINE:I386
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib uuid.lib winmm.lib /NOLOGO /SUBSYSTEM:windows,4.0\
 /INCREMENTAL:no /PDB:$(OUTDIR)/"SPLASH.pdb" /MACHINE:I386\
 /OUT:$(OUTDIR)/"SPLASH.exe" 
DEF_FILE=
LINK32_OBJS= \
    $(INTDIR)/SPLASH.OBJ \
    $(INTDIR)/SPLASH.res

$(OUTDIR)/SPLASH.exe : $(OUTDIR)  $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<
    copy $(OUTDIR)\SPLASH.exe iklowns.exe

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

ALL : $(OUTDIR)/SPLASH.exe $(OUTDIR)/SPLASH.bsc

$(OUTDIR) : 
    if not exist $(OUTDIR)/nul mkdir $(OUTDIR)

# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
# ADD BASE CPP /nologo /W3 /GX /Zi /YX /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /FR /c
# ADD CPP /nologo /W3 /GX /Zi /YX /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /FR /c
CPP_PROJ=/nologo /W3 /GX /Zi /YX /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /FR$(INTDIR)/ /Fp$(OUTDIR)/"SPLASH.pch" /Fo$(INTDIR)/ /Fd$(OUTDIR)/"SPLASH.pdb"\
 /c 
CPP_OBJS=.\WinDebug/
# ADD BASE RSC /d "_DEBUG"
# ADD RSC /d "_DEBUG"
RSC_PROJ=/l 0x409 /fo$(INTDIR)/"SPLASH.res" /d "_DEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o$(OUTDIR)/"SPLASH.bsc" 
BSC32_SBRS= \
    $(INTDIR)/SPLASH.SBR

$(OUTDIR)/SPLASH.bsc : $(OUTDIR)  $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /NOLOGO /SUBSYSTEM:windows,4.0 /DEBUG /MACHINE:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib uuid.lib winmm.lib /NOLOGO /SUBSYSTEM:windows,4.0 /DEBUG /MACHINE:I386
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib uuid.lib winmm.lib /NOLOGO /SUBSYSTEM:windows,4.0\
 /INCREMENTAL:yes /PDB:$(OUTDIR)/"SPLASH.pdb" /DEBUG /MACHINE:I386\
 /OUT:$(OUTDIR)/"SPLASH.exe" 
DEF_FILE=
LINK32_OBJS= \
    $(INTDIR)/SPLASH.OBJ \
    $(INTDIR)/SPLASH.res

$(OUTDIR)/SPLASH.exe : $(OUTDIR)  $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<
    copy $(OUTDIR)\SPLASH.exe iklowns.exe

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

SOURCE=.\SPLASH.CPP

$(INTDIR)/SPLASH.OBJ :  $(SOURCE)  $(INTDIR)

# End Source File
################################################################################
# Begin Source File

SOURCE=.\SPLASH.RC
DEP_SPLAS=\
    .\data\SPLASH.BMP \
    .\SPLASH.WAV

$(INTDIR)/SPLASH.res :  $(SOURCE)  $(DEP_SPLAS) $(INTDIR)
#   which $(RSC)
   $(RSC) $(RSC_PROJ)  $(SOURCE) 

# End Source File
# End Group
# End Project
################################################################################
