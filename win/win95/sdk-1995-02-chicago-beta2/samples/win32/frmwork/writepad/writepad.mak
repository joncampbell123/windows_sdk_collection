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
!MESSAGE NMAKE /f "WRITEPAD.MAK" CFG="Win32 Debug"
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
# PROP Target_Last_Scanned "Win32 Release"
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

ALL : $(OUTDIR)/WRITEPAD.exe $(OUTDIR)/WRITEPAD.bsc

$(OUTDIR) : 
    if not exist $(OUTDIR)/nul mkdir $(OUTDIR)

# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
# ADD BASE CPP /nologo /W3 /GX /YX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /FR /c
# ADD CPP /nologo /W3 /GX /YX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /FR /c
CPP_PROJ=/nologo /W3 /GX /YX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS"\
 /FR$(INTDIR)/ /Fp$(OUTDIR)/"WRITEPAD.pch" /Fo$(INTDIR)/ /c 
CPP_OBJS=.\WinRel/
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
RSC_PROJ=/l 0x409 /fo$(INTDIR)/"WRITEPAD.res" /d "NDEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o$(OUTDIR)/"WRITEPAD.bsc" 
BSC32_SBRS= \
	$(INTDIR)/PRINT.SBR \
	$(INTDIR)/MISC.SBR \
	$(INTDIR)/FILEDLG.SBR \
	$(INTDIR)/STATBAR.SBR \
	$(INTDIR)/WRITEPAD.SBR \
	$(INTDIR)/RULER.SBR \
	$(INTDIR)/DISPATCH.SBR \
	$(INTDIR)/INIT.SBR \
	$(INTDIR)/TOOLBAR.SBR \
	$(INTDIR)/FILE.SBR \
	$(INTDIR)/SEARCH.SBR \
	$(INTDIR)/PRINTDLG.SBR \
	$(INTDIR)/WINMAIN.SBR \
	$(INTDIR)/FINDDLG.SBR \
	$(INTDIR)/RTF.SBR \
	$(INTDIR)/MDICHILD.SBR \
	$(INTDIR)/ABOUT.SBR

$(OUTDIR)/WRITEPAD.bsc : $(OUTDIR)  $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /NOLOGO /SUBSYSTEM:windows /MACHINE:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib comctl32.lib version.lib /NOLOGO /MACHINE:I386 /SUBSYSTEM:windows,4.0
# SUBTRACT LINK32 /PDB:none
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib comctl32.lib\
 version.lib /NOLOGO /INCREMENTAL:no /PDB:$(OUTDIR)/"WRITEPAD.pdb" /MACHINE:I386\
 /OUT:$(OUTDIR)/"WRITEPAD.exe" /SUBSYSTEM:windows,4.0  
DEF_FILE=
LINK32_OBJS= \
	$(INTDIR)/PRINT.OBJ \
	$(INTDIR)/MISC.OBJ \
	$(INTDIR)/FILEDLG.OBJ \
	$(INTDIR)/STATBAR.OBJ \
	$(INTDIR)/WRITEPAD.OBJ \
	$(INTDIR)/RULER.OBJ \
	$(INTDIR)/DISPATCH.OBJ \
	$(INTDIR)/INIT.OBJ \
	$(INTDIR)/TOOLBAR.OBJ \
	$(INTDIR)/FILE.OBJ \
	$(INTDIR)/SEARCH.OBJ \
	$(INTDIR)/PRINTDLG.OBJ \
	$(INTDIR)/WINMAIN.OBJ \
	$(INTDIR)/FINDDLG.OBJ \
	$(INTDIR)/RTF.OBJ \
	$(INTDIR)/WRITEPAD.res \
	$(INTDIR)/MDICHILD.OBJ \
	$(INTDIR)/ABOUT.OBJ

$(OUTDIR)/WRITEPAD.exe : $(OUTDIR)  $(DEF_FILE) $(LINK32_OBJS)
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

ALL : $(OUTDIR)/WRITEPAD.exe $(OUTDIR)/WRITEPAD.bsc

$(OUTDIR) : 
    if not exist $(OUTDIR)/nul mkdir $(OUTDIR)

# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
# ADD BASE CPP /nologo /W3 /GX /Zi /YX /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /FR /c
# ADD CPP /nologo /W3 /GX /Zi /YX /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /FR /c
CPP_PROJ=/nologo /W3 /GX /Zi /YX /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /FR$(INTDIR)/ /Fp$(OUTDIR)/"WRITEPAD.pch" /Fo$(INTDIR)/\
 /Fd$(OUTDIR)/"WRITEPAD.pdb" /c 
CPP_OBJS=.\WinDebug/
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
RSC_PROJ=/l 0x409 /fo$(INTDIR)/"WRITEPAD.res" /d "_DEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o$(OUTDIR)/"WRITEPAD.bsc" 
BSC32_SBRS= \
	$(INTDIR)/PRINT.SBR \
	$(INTDIR)/MISC.SBR \
	$(INTDIR)/FILEDLG.SBR \
	$(INTDIR)/STATBAR.SBR \
	$(INTDIR)/WRITEPAD.SBR \
	$(INTDIR)/RULER.SBR \
	$(INTDIR)/DISPATCH.SBR \
	$(INTDIR)/INIT.SBR \
	$(INTDIR)/TOOLBAR.SBR \
	$(INTDIR)/FILE.SBR \
	$(INTDIR)/SEARCH.SBR \
	$(INTDIR)/PRINTDLG.SBR \
	$(INTDIR)/WINMAIN.SBR \
	$(INTDIR)/FINDDLG.SBR \
	$(INTDIR)/RTF.SBR \
	$(INTDIR)/MDICHILD.SBR \
	$(INTDIR)/ABOUT.SBR

$(OUTDIR)/WRITEPAD.bsc : $(OUTDIR)  $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /NOLOGO /SUBSYSTEM:windows /DEBUG /MACHINE:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib comctl32.lib version.lib /NOLOGO /DEBUG /MACHINE:I386 /SUBSYSTEM:windows,4.0
# SUBTRACT LINK32 /PDB:none
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib comctl32.lib\
 version.lib /NOLOGO /INCREMENTAL:yes /PDB:$(OUTDIR)/"WRITEPAD.pdb" /DEBUG\
 /MACHINE:I386 /OUT:$(OUTDIR)/"WRITEPAD.exe" /SUBSYSTEM:windows,4.0  
DEF_FILE=
LINK32_OBJS= \
	$(INTDIR)/PRINT.OBJ \
	$(INTDIR)/MISC.OBJ \
	$(INTDIR)/FILEDLG.OBJ \
	$(INTDIR)/STATBAR.OBJ \
	$(INTDIR)/WRITEPAD.OBJ \
	$(INTDIR)/RULER.OBJ \
	$(INTDIR)/DISPATCH.OBJ \
	$(INTDIR)/INIT.OBJ \
	$(INTDIR)/TOOLBAR.OBJ \
	$(INTDIR)/FILE.OBJ \
	$(INTDIR)/SEARCH.OBJ \
	$(INTDIR)/PRINTDLG.OBJ \
	$(INTDIR)/WINMAIN.OBJ \
	$(INTDIR)/FINDDLG.OBJ \
	$(INTDIR)/RTF.OBJ \
	$(INTDIR)/WRITEPAD.res \
	$(INTDIR)/MDICHILD.OBJ \
	$(INTDIR)/ABOUT.OBJ

$(OUTDIR)/WRITEPAD.exe : $(OUTDIR)  $(DEF_FILE) $(LINK32_OBJS)
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

SOURCE=.\PRINT.C
DEP_PRINT=\
	.\GLOBALS.H\
	.\writepad.inl

$(INTDIR)/PRINT.OBJ :  $(SOURCE)  $(DEP_PRINT) $(INTDIR)

# End Source File
################################################################################
# Begin Source File

SOURCE=.\MISC.C
DEP_MISC_=\
	.\GLOBALS.H\
	.\TOOLBAR.H\
	.\writepad.inl

$(INTDIR)/MISC.OBJ :  $(SOURCE)  $(DEP_MISC_) $(INTDIR)

# End Source File
################################################################################
# Begin Source File

SOURCE=.\FILEDLG.C
DEP_FILED=\
	.\GLOBALS.H\
	.\writepad.inl

$(INTDIR)/FILEDLG.OBJ :  $(SOURCE)  $(DEP_FILED) $(INTDIR)

# End Source File
################################################################################
# Begin Source File

SOURCE=.\STATBAR.C
DEP_STATB=\
	.\GLOBALS.H\
	.\STATBAR.H\
	.\writepad.inl

$(INTDIR)/STATBAR.OBJ :  $(SOURCE)  $(DEP_STATB) $(INTDIR)

# End Source File
################################################################################
# Begin Source File

SOURCE=.\WRITEPAD.C
DEP_WRITE=\
	.\GLOBALS.H\
	.\MDICHILD.H\
	.\TOOLBAR.H\
	.\STATBAR.H\
	.\RTF.H\
	.\writepad.inl

$(INTDIR)/WRITEPAD.OBJ :  $(SOURCE)  $(DEP_WRITE) $(INTDIR)

# End Source File
################################################################################
# Begin Source File

SOURCE=.\RULER.C
DEP_RULER=\
	.\GLOBALS.H\
	.\RULER.H\
	.\writepad.inl

$(INTDIR)/RULER.OBJ :  $(SOURCE)  $(DEP_RULER) $(INTDIR)

# End Source File
################################################################################
# Begin Source File

SOURCE=.\DISPATCH.C
DEP_DISPA=\
	.\GLOBALS.H\
	.\writepad.inl

$(INTDIR)/DISPATCH.OBJ :  $(SOURCE)  $(DEP_DISPA) $(INTDIR)

# End Source File
################################################################################
# Begin Source File

SOURCE=.\INIT.C
DEP_INIT_=\
	.\GLOBALS.H\
	.\writepad.inl

$(INTDIR)/INIT.OBJ :  $(SOURCE)  $(DEP_INIT_) $(INTDIR)

# End Source File
################################################################################
# Begin Source File

SOURCE=.\TOOLBAR.C
DEP_TOOLB=\
	.\GLOBALS.H\
	.\TOOLBAR.H\
	.\writepad.inl

$(INTDIR)/TOOLBAR.OBJ :  $(SOURCE)  $(DEP_TOOLB) $(INTDIR)

# End Source File
################################################################################
# Begin Source File

SOURCE=.\FILE.C
DEP_FILE_=\
	.\GLOBALS.H\
	.\RTF.H\
	.\writepad.inl

$(INTDIR)/FILE.OBJ :  $(SOURCE)  $(DEP_FILE_) $(INTDIR)

# End Source File
################################################################################
# Begin Source File

SOURCE=.\SEARCH.C
DEP_SEARC=\
	.\GLOBALS.H\
	.\writepad.inl

$(INTDIR)/SEARCH.OBJ :  $(SOURCE)  $(DEP_SEARC) $(INTDIR)

# End Source File
################################################################################
# Begin Source File

SOURCE=.\PRINTDLG.C
DEP_PRINTD=\
	.\GLOBALS.H\
	.\writepad.inl

$(INTDIR)/PRINTDLG.OBJ :  $(SOURCE)  $(DEP_PRINTD) $(INTDIR)

# End Source File
################################################################################
# Begin Source File

SOURCE=.\WINMAIN.C
DEP_WINMA=\
	.\GLOBALS.H\
	.\writepad.inl

$(INTDIR)/WINMAIN.OBJ :  $(SOURCE)  $(DEP_WINMA) $(INTDIR)

# End Source File
################################################################################
# Begin Source File

SOURCE=.\FINDDLG.C
DEP_FINDD=\
	.\GLOBALS.H\
	.\writepad.inl

$(INTDIR)/FINDDLG.OBJ :  $(SOURCE)  $(DEP_FINDD) $(INTDIR)

# End Source File
################################################################################
# Begin Source File

SOURCE=.\RTF.C
DEP_RTF_C=\
	.\GLOBALS.H\
	.\RTF.H\
	.\RULER.H\
	.\TOOLBAR.H\
	.\writepad.inl

$(INTDIR)/RTF.OBJ :  $(SOURCE)  $(DEP_RTF_C) $(INTDIR)

# End Source File
################################################################################
# Begin Source File

SOURCE=.\WRITEPAD.RC
DEP_WRITEP=\
	.\WRITEPAD.ICO\
	.\CHILD.ICO\
	.\TOOLBAR.BMP\
	.\RULER.BMP\
	.\TOOLBAR.H\
	.\STATBAR.H\
	.\RULER.H

$(INTDIR)/WRITEPAD.res :  $(SOURCE)  $(DEP_WRITEP) $(INTDIR)
   $(RSC) $(RSC_PROJ)  $(SOURCE) 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\MDICHILD.C
DEP_MDICH=\
	.\GLOBALS.H\
	.\MDICHILD.H\
	.\TOOLBAR.H\
	.\STATBAR.H\
	.\RULER.H\
	.\RTF.H\
	.\writepad.inl

$(INTDIR)/MDICHILD.OBJ :  $(SOURCE)  $(DEP_MDICH) $(INTDIR)

# End Source File
################################################################################
# Begin Source File

SOURCE=.\ABOUT.C
DEP_ABOUT=\
	.\GLOBALS.H\
	.\writepad.inl

$(INTDIR)/ABOUT.OBJ :  $(SOURCE)  $(DEP_ABOUT) $(INTDIR)

# End Source File
# End Group
# End Project
################################################################################
