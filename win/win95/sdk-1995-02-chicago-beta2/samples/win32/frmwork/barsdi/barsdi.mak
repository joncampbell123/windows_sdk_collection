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
!MESSAGE NMAKE /f "BARSDI.MAK" CFG="Win32 Debug"
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

ALL : $(OUTDIR)/BARSDI.exe $(OUTDIR)/BARSDI.bsc

$(OUTDIR) : 
    if not exist $(OUTDIR)/nul mkdir $(OUTDIR)

# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
# ADD BASE CPP /nologo /W3 /GX /YX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /FR /c
# ADD CPP /nologo /W3 /GX /YX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /FR /c
CPP_PROJ=/nologo /W3 /GX /YX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS"\
 /FR$(INTDIR)/ /Fp$(OUTDIR)/"BARSDI.pch" /Fo$(INTDIR)/ /c 
CPP_OBJS=.\WinRel/
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
RSC_PROJ=/l 0x409 /fo$(INTDIR)/"BARSDI.res" /d "NDEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o$(OUTDIR)/"BARSDI.bsc" 
BSC32_SBRS= \
	$(INTDIR)/TOOLBAR.SBR \
	$(INTDIR)/MISC.SBR \
	$(INTDIR)/BARSDI.SBR \
	$(INTDIR)/INIT.SBR \
	$(INTDIR)/DISPATCH.SBR \
	$(INTDIR)/WINMAIN.SBR \
	$(INTDIR)/STATBAR.SBR \
	$(INTDIR)/ABOUT.SBR

$(OUTDIR)/BARSDI.bsc : $(OUTDIR)  $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /NOLOGO /SUBSYSTEM:windows /MACHINE:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib comctl32.lib version.lib /NOLOGO /MACHINE:I386 /SUBSYSTEM:windows,4.0
# SUBTRACT LINK32 /PDB:none
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib comctl32.lib\
 version.lib /NOLOGO /INCREMENTAL:no /PDB:$(OUTDIR)/"BARSDI.pdb" /MACHINE:I386\
 /OUT:$(OUTDIR)/"BARSDI.exe" /SUBSYSTEM:windows,4.0  
DEF_FILE=
LINK32_OBJS= \
	$(INTDIR)/BARSDI.res \
	$(INTDIR)/TOOLBAR.OBJ \
	$(INTDIR)/MISC.OBJ \
	$(INTDIR)/BARSDI.OBJ \
	$(INTDIR)/INIT.OBJ \
	$(INTDIR)/DISPATCH.OBJ \
	$(INTDIR)/WINMAIN.OBJ \
	$(INTDIR)/STATBAR.OBJ \
	$(INTDIR)/ABOUT.OBJ

$(OUTDIR)/BARSDI.exe : $(OUTDIR)  $(DEF_FILE) $(LINK32_OBJS)
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
# PROP Output_Dir "WinDebug"
# PROP Intermediate_Dir "WinDebug"
OUTDIR=.\WinDebug
INTDIR=.\WinDebug

ALL : $(OUTDIR)/BARSDI.exe $(OUTDIR)/BARSDI.bsc

$(OUTDIR) : 
    if not exist $(OUTDIR)/nul mkdir $(OUTDIR)

# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
# ADD BASE CPP /nologo /W3 /GX /Zi /YX /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /FR /c
# ADD CPP /nologo /W3 /GX /Zi /YX /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /FR /c
CPP_PROJ=/nologo /W3 /GX /Zi /YX /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /FR$(INTDIR)/ /Fp$(OUTDIR)/"BARSDI.pch" /Fo$(INTDIR)/ /Fd$(OUTDIR)/"BARSDI.pdb"\
 /c 
CPP_OBJS=.\WinDebug/
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
RSC_PROJ=/l 0x409 /fo$(INTDIR)/"BARSDI.res" /d "_DEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o$(OUTDIR)/"BARSDI.bsc" 
BSC32_SBRS= \
	$(INTDIR)/TOOLBAR.SBR \
	$(INTDIR)/MISC.SBR \
	$(INTDIR)/BARSDI.SBR \
	$(INTDIR)/INIT.SBR \
	$(INTDIR)/DISPATCH.SBR \
	$(INTDIR)/WINMAIN.SBR \
	$(INTDIR)/STATBAR.SBR \
	$(INTDIR)/ABOUT.SBR

$(OUTDIR)/BARSDI.bsc : $(OUTDIR)  $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /NOLOGO /SUBSYSTEM:windows /DEBUG /MACHINE:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib comctl32.lib version.lib /NOLOGO /DEBUG /MACHINE:I386 /SUBSYSTEM:windows,4.0
# SUBTRACT LINK32 /PDB:none
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib comctl32.lib\
 version.lib /NOLOGO /INCREMENTAL:yes /PDB:$(OUTDIR)/"BARSDI.pdb" /DEBUG\
 /MACHINE:I386 /OUT:$(OUTDIR)/"BARSDI.exe" /SUBSYSTEM:windows,4.0  
DEF_FILE=
LINK32_OBJS= \
	$(INTDIR)/BARSDI.res \
	$(INTDIR)/TOOLBAR.OBJ \
	$(INTDIR)/MISC.OBJ \
	$(INTDIR)/BARSDI.OBJ \
	$(INTDIR)/INIT.OBJ \
	$(INTDIR)/DISPATCH.OBJ \
	$(INTDIR)/WINMAIN.OBJ \
	$(INTDIR)/STATBAR.OBJ \
	$(INTDIR)/ABOUT.OBJ

$(OUTDIR)/BARSDI.exe : $(OUTDIR)  $(DEF_FILE) $(LINK32_OBJS)
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

SOURCE=.\BARSDI.RC
DEP_BARSD=\
	.\BARSDI.ICO\
	.\TOOLBAR.BMP\
	.\GLOBALS.H\
	.\TOOLBAR.H\
	.\STATBAR.H

$(INTDIR)/BARSDI.res :  $(SOURCE)  $(DEP_BARSD) $(INTDIR)
   $(RSC) $(RSC_PROJ)  $(SOURCE) 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\TOOLBAR.C
DEP_TOOLB=\
	.\GLOBALS.H\
	.\TOOLBAR.H

$(INTDIR)/TOOLBAR.OBJ :  $(SOURCE)  $(DEP_TOOLB) $(INTDIR)

# End Source File
################################################################################
# Begin Source File

SOURCE=.\MISC.C
DEP_MISC_=\
	.\GLOBALS.H\
	.\STATBAR.H

$(INTDIR)/MISC.OBJ :  $(SOURCE)  $(DEP_MISC_) $(INTDIR)

# End Source File
################################################################################
# Begin Source File

SOURCE=.\BARSDI.C
DEP_BARSDI=\
	.\GLOBALS.H\
	.\STATBAR.H\
	.\TOOLBAR.H

$(INTDIR)/BARSDI.OBJ :  $(SOURCE)  $(DEP_BARSDI) $(INTDIR)

# End Source File
################################################################################
# Begin Source File

SOURCE=.\INIT.C
DEP_INIT_=\
	.\GLOBALS.H

$(INTDIR)/INIT.OBJ :  $(SOURCE)  $(DEP_INIT_) $(INTDIR)

# End Source File
################################################################################
# Begin Source File

SOURCE=.\DISPATCH.C
DEP_DISPA=\
	.\GLOBALS.H

$(INTDIR)/DISPATCH.OBJ :  $(SOURCE)  $(DEP_DISPA) $(INTDIR)

# End Source File
################################################################################
# Begin Source File

SOURCE=.\WINMAIN.C
DEP_WINMA=\
	.\GLOBALS.H

$(INTDIR)/WINMAIN.OBJ :  $(SOURCE)  $(DEP_WINMA) $(INTDIR)

# End Source File
################################################################################
# Begin Source File

SOURCE=.\STATBAR.C
DEP_STATB=\
	.\GLOBALS.H\
	.\STATBAR.H

$(INTDIR)/STATBAR.OBJ :  $(SOURCE)  $(DEP_STATB) $(INTDIR)

# End Source File
################################################################################
# Begin Source File

SOURCE=.\ABOUT.C
DEP_ABOUT=\
	.\GLOBALS.H

$(INTDIR)/ABOUT.OBJ :  $(SOURCE)  $(DEP_ABOUT) $(INTDIR)

# End Source File
# End Group
# End Project
################################################################################
