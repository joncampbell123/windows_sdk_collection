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
!MESSAGE NMAKE /f "SUBCLS.MAK" CFG="Win32 Debug"
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

ALL : $(OUTDIR)/SUBCLS.exe $(OUTDIR)/SUBCLS.bsc

$(OUTDIR) : 
    if not exist $(OUTDIR)/nul mkdir $(OUTDIR)

# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
# ADD BASE CPP /nologo /W3 /GX /YX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /FR /c
# ADD CPP /nologo /W3 /GX /YX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /FR /c
CPP_PROJ=/nologo /W3 /GX /YX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS"\
 /FR$(INTDIR)/ /Fp$(OUTDIR)/"SUBCLS.pch" /Fo$(INTDIR)/ /c 
CPP_OBJS=.\WinRel/
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
RSC_PROJ=/l 0x409 /fo$(INTDIR)/"SUBCLS.res" /d "NDEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o$(OUTDIR)/"SUBCLS.bsc" 
BSC32_SBRS= \
	$(INTDIR)/ABOUT.SBR \
	$(INTDIR)/MODAL.SBR \
	$(INTDIR)/INIT.SBR \
	$(INTDIR)/DIALOG.SBR \
	$(INTDIR)/SUBCLS.SBR \
	$(INTDIR)/DISPATCH.SBR \
	$(INTDIR)/WINMAIN.SBR \
	$(INTDIR)/MISC.SBR

$(OUTDIR)/SUBCLS.bsc : $(OUTDIR)  $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /NOLOGO /SUBSYSTEM:windows /MACHINE:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib comctl32.lib version.lib /NOLOGO /MACHINE:I386 /SUBSYSTEM:windows,4.0
# SUBTRACT LINK32 /PDB:none
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib comctl32.lib\
 version.lib /NOLOGO /INCREMENTAL:no /PDB:$(OUTDIR)/"SUBCLS.pdb" /MACHINE:I386\
 /OUT:$(OUTDIR)/"SUBCLS.exe" /SUBSYSTEM:windows,4.0  
DEF_FILE=
LINK32_OBJS= \
	$(INTDIR)/ABOUT.OBJ \
	$(INTDIR)/MODAL.OBJ \
	$(INTDIR)/INIT.OBJ \
	$(INTDIR)/DIALOG.OBJ \
	$(INTDIR)/SUBCLS.OBJ \
	$(INTDIR)/DISPATCH.OBJ \
	$(INTDIR)/WINMAIN.OBJ \
	$(INTDIR)/SUBCLS.res \
	$(INTDIR)/MISC.OBJ

$(OUTDIR)/SUBCLS.exe : $(OUTDIR)  $(DEF_FILE) $(LINK32_OBJS)
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

ALL : $(OUTDIR)/SUBCLS.exe $(OUTDIR)/SUBCLS.bsc

$(OUTDIR) : 
    if not exist $(OUTDIR)/nul mkdir $(OUTDIR)

# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
# ADD BASE CPP /nologo /W3 /GX /Zi /YX /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /FR /c
# ADD CPP /nologo /W3 /GX /Zi /YX /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /FR /c
CPP_PROJ=/nologo /W3 /GX /Zi /YX /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /FR$(INTDIR)/ /Fp$(OUTDIR)/"SUBCLS.pch" /Fo$(INTDIR)/ /Fd$(OUTDIR)/"SUBCLS.pdb"\
 /c 
CPP_OBJS=.\WinDebug/
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
RSC_PROJ=/l 0x409 /fo$(INTDIR)/"SUBCLS.res" /d "_DEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o$(OUTDIR)/"SUBCLS.bsc" 
BSC32_SBRS= \
	$(INTDIR)/ABOUT.SBR \
	$(INTDIR)/MODAL.SBR \
	$(INTDIR)/INIT.SBR \
	$(INTDIR)/DIALOG.SBR \
	$(INTDIR)/SUBCLS.SBR \
	$(INTDIR)/DISPATCH.SBR \
	$(INTDIR)/WINMAIN.SBR \
	$(INTDIR)/MISC.SBR

$(OUTDIR)/SUBCLS.bsc : $(OUTDIR)  $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /NOLOGO /SUBSYSTEM:windows /DEBUG /MACHINE:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib comctl32.lib version.lib /NOLOGO /DEBUG /MACHINE:I386 /SUBSYSTEM:windows,4.0
# SUBTRACT LINK32 /PDB:none
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib comctl32.lib\
 version.lib /NOLOGO /INCREMENTAL:yes /PDB:$(OUTDIR)/"SUBCLS.pdb" /DEBUG\
 /MACHINE:I386 /OUT:$(OUTDIR)/"SUBCLS.exe" /SUBSYSTEM:windows,4.0  
DEF_FILE=
LINK32_OBJS= \
	$(INTDIR)/ABOUT.OBJ \
	$(INTDIR)/MODAL.OBJ \
	$(INTDIR)/INIT.OBJ \
	$(INTDIR)/DIALOG.OBJ \
	$(INTDIR)/SUBCLS.OBJ \
	$(INTDIR)/DISPATCH.OBJ \
	$(INTDIR)/WINMAIN.OBJ \
	$(INTDIR)/SUBCLS.res \
	$(INTDIR)/MISC.OBJ

$(OUTDIR)/SUBCLS.exe : $(OUTDIR)  $(DEF_FILE) $(LINK32_OBJS)
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

SOURCE=.\ABOUT.C
DEP_ABOUT=\
	.\WIN16EXT.H\
	.\GLOBALS.H

$(INTDIR)/ABOUT.OBJ :  $(SOURCE)  $(DEP_ABOUT) $(INTDIR)

# End Source File
################################################################################
# Begin Source File

SOURCE=.\MODAL.C
DEP_MODAL=\
	.\WIN16EXT.H\
	.\GLOBALS.H\
	.\MODAL.H\
	.\SUBCLS.H\
	.\LISTDLG.H

$(INTDIR)/MODAL.OBJ :  $(SOURCE)  $(DEP_MODAL) $(INTDIR)

# End Source File
################################################################################
# Begin Source File

SOURCE=.\INIT.C
DEP_INIT_=\
	.\WIN16EXT.H\
	.\GLOBALS.H

$(INTDIR)/INIT.OBJ :  $(SOURCE)  $(DEP_INIT_) $(INTDIR)

# End Source File
################################################################################
# Begin Source File

SOURCE=.\DIALOG.C
DEP_DIALO=\
	.\WIN16EXT.H\
	.\GLOBALS.H\
	.\MODAL.H\
	.\SUBCLS.H\
	.\LISTDLG.H

$(INTDIR)/DIALOG.OBJ :  $(SOURCE)  $(DEP_DIALO) $(INTDIR)

# End Source File
################################################################################
# Begin Source File

SOURCE=.\SUBCLS.C
DEP_SUBCL=\
	.\WIN16EXT.H\
	.\GLOBALS.H\
	.\MODAL.H\
	.\SUBCLS.H\
	.\LISTDLG.H

$(INTDIR)/SUBCLS.OBJ :  $(SOURCE)  $(DEP_SUBCL) $(INTDIR)

# End Source File
################################################################################
# Begin Source File

SOURCE=.\DISPATCH.C
DEP_DISPA=\
	.\WIN16EXT.H\
	.\GLOBALS.H

$(INTDIR)/DISPATCH.OBJ :  $(SOURCE)  $(DEP_DISPA) $(INTDIR)

# End Source File
################################################################################
# Begin Source File

SOURCE=.\WINMAIN.C
DEP_WINMA=\
	.\WIN16EXT.H\
	.\GLOBALS.H

$(INTDIR)/WINMAIN.OBJ :  $(SOURCE)  $(DEP_WINMA) $(INTDIR)

# End Source File
################################################################################
# Begin Source File

SOURCE=.\SUBCLS.RC
DEP_SUBCLS=\
	.\subcls.ICO\
	.\GLOBALS.H\
	.\MODAL.H\
	.\LISTDLG.H\
	.\version.h

$(INTDIR)/SUBCLS.res :  $(SOURCE)  $(DEP_SUBCLS) $(INTDIR)
   $(RSC) $(RSC_PROJ)  $(SOURCE) 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\MISC.C
DEP_MISC_=\
	.\WIN16EXT.H\
	.\GLOBALS.H

$(INTDIR)/MISC.OBJ :  $(SOURCE)  $(DEP_MISC_) $(INTDIR)

# End Source File
# End Group
# End Project
################################################################################
