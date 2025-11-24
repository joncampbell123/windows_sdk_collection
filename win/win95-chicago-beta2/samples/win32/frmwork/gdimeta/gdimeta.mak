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
!MESSAGE NMAKE /f "GDIMETA.MAK" CFG="Win32 Debug"
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

ALL : $(OUTDIR)/GDIMETA.exe $(OUTDIR)/GDIMETA.bsc

$(OUTDIR) : 
    if not exist $(OUTDIR)/nul mkdir $(OUTDIR)

# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
# ADD BASE CPP /nologo /W3 /GX /YX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /FR /c
# ADD CPP /nologo /W3 /GX /YX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /FR /c
CPP_PROJ=/nologo /W3 /GX /YX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS"\
 /FR$(INTDIR)/ /Fp$(OUTDIR)/"GDIMETA.pch" /Fo$(INTDIR)/ /c 
CPP_OBJS=.\WinRel/
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
RSC_PROJ=/l 0x409 /fo$(INTDIR)/"GDIMETA.res" /d "NDEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o$(OUTDIR)/"GDIMETA.bsc" 
BSC32_SBRS= \
	$(INTDIR)/FILEDLG.SBR \
	$(INTDIR)/INIT.SBR \
	$(INTDIR)/DISPATCH.SBR \
	$(INTDIR)/WINMAIN.SBR \
	$(INTDIR)/STATBAR.SBR \
	$(INTDIR)/FILE.SBR \
	$(INTDIR)/COLORDLG.SBR \
	$(INTDIR)/TOOLBAR.SBR \
	$(INTDIR)/METAFILE.SBR \
	$(INTDIR)/PALETTE.SBR \
	$(INTDIR)/PENDLG.SBR \
	$(INTDIR)/MISC.SBR \
	$(INTDIR)/PALCTRL.SBR \
	$(INTDIR)/CLIENT.SBR \
	$(INTDIR)/INFODLG.SBR \
	$(INTDIR)/ABOUT.SBR \
	$(INTDIR)/GDIMETA.SBR \
	$(INTDIR)/BRUSHDLG.SBR

$(OUTDIR)/GDIMETA.bsc : $(OUTDIR)  $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /NOLOGO /SUBSYSTEM:windows /MACHINE:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib comctl32.lib version.lib /NOLOGO /MACHINE:I386 /SUBSYSTEM:windows,4.0
# SUBTRACT LINK32 /PDB:none
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib comctl32.lib\
 version.lib /NOLOGO /INCREMENTAL:no /PDB:$(OUTDIR)/"GDIMETA.pdb" /MACHINE:I386\
 /OUT:$(OUTDIR)/"GDIMETA.exe" /SUBSYSTEM:windows,4.0  
DEF_FILE=
LINK32_OBJS= \
	$(INTDIR)/FILEDLG.OBJ \
	$(INTDIR)/INIT.OBJ \
	$(INTDIR)/DISPATCH.OBJ \
	$(INTDIR)/WINMAIN.OBJ \
	$(INTDIR)/STATBAR.OBJ \
	$(INTDIR)/FILE.OBJ \
	$(INTDIR)/COLORDLG.OBJ \
	$(INTDIR)/TOOLBAR.OBJ \
	$(INTDIR)/METAFILE.OBJ \
	$(INTDIR)/PALETTE.OBJ \
	$(INTDIR)/GDIMETA.res \
	$(INTDIR)/PENDLG.OBJ \
	$(INTDIR)/MISC.OBJ \
	$(INTDIR)/PALCTRL.OBJ \
	$(INTDIR)/CLIENT.OBJ \
	$(INTDIR)/INFODLG.OBJ \
	$(INTDIR)/ABOUT.OBJ \
	$(INTDIR)/GDIMETA.OBJ \
	$(INTDIR)/BRUSHDLG.OBJ

$(OUTDIR)/GDIMETA.exe : $(OUTDIR)  $(DEF_FILE) $(LINK32_OBJS)
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

ALL : $(OUTDIR)/GDIMETA.exe $(OUTDIR)/GDIMETA.bsc

$(OUTDIR) : 
    if not exist $(OUTDIR)/nul mkdir $(OUTDIR)

# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
# ADD BASE CPP /nologo /W3 /GX /Zi /YX /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /FR /c
# ADD CPP /nologo /W3 /GX /Zi /YX /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /FR /c
CPP_PROJ=/nologo /W3 /GX /Zi /YX /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /FR$(INTDIR)/ /Fp$(OUTDIR)/"GDIMETA.pch" /Fo$(INTDIR)/\
 /Fd$(OUTDIR)/"GDIMETA.pdb" /c 
CPP_OBJS=.\WinDebug/
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
RSC_PROJ=/l 0x409 /fo$(INTDIR)/"GDIMETA.res" /d "_DEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o$(OUTDIR)/"GDIMETA.bsc" 
BSC32_SBRS= \
	$(INTDIR)/FILEDLG.SBR \
	$(INTDIR)/INIT.SBR \
	$(INTDIR)/DISPATCH.SBR \
	$(INTDIR)/WINMAIN.SBR \
	$(INTDIR)/STATBAR.SBR \
	$(INTDIR)/FILE.SBR \
	$(INTDIR)/COLORDLG.SBR \
	$(INTDIR)/TOOLBAR.SBR \
	$(INTDIR)/METAFILE.SBR \
	$(INTDIR)/PALETTE.SBR \
	$(INTDIR)/PENDLG.SBR \
	$(INTDIR)/MISC.SBR \
	$(INTDIR)/PALCTRL.SBR \
	$(INTDIR)/CLIENT.SBR \
	$(INTDIR)/INFODLG.SBR \
	$(INTDIR)/ABOUT.SBR \
	$(INTDIR)/GDIMETA.SBR \
	$(INTDIR)/BRUSHDLG.SBR

$(OUTDIR)/GDIMETA.bsc : $(OUTDIR)  $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /NOLOGO /SUBSYSTEM:windows /DEBUG /MACHINE:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib comctl32.lib version.lib /NOLOGO /DEBUG /MACHINE:I386 /SUBSYSTEM:windows,4.0
# SUBTRACT LINK32 /PDB:none
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib comctl32.lib\
 version.lib /NOLOGO /INCREMENTAL:yes /PDB:$(OUTDIR)/"GDIMETA.pdb" /DEBUG\
 /MACHINE:I386 /OUT:$(OUTDIR)/"GDIMETA.exe" /SUBSYSTEM:windows,4.0  
DEF_FILE=
LINK32_OBJS= \
	$(INTDIR)/FILEDLG.OBJ \
	$(INTDIR)/INIT.OBJ \
	$(INTDIR)/DISPATCH.OBJ \
	$(INTDIR)/WINMAIN.OBJ \
	$(INTDIR)/STATBAR.OBJ \
	$(INTDIR)/FILE.OBJ \
	$(INTDIR)/COLORDLG.OBJ \
	$(INTDIR)/TOOLBAR.OBJ \
	$(INTDIR)/METAFILE.OBJ \
	$(INTDIR)/PALETTE.OBJ \
	$(INTDIR)/GDIMETA.res \
	$(INTDIR)/PENDLG.OBJ \
	$(INTDIR)/MISC.OBJ \
	$(INTDIR)/PALCTRL.OBJ \
	$(INTDIR)/CLIENT.OBJ \
	$(INTDIR)/INFODLG.OBJ \
	$(INTDIR)/ABOUT.OBJ \
	$(INTDIR)/GDIMETA.OBJ \
	$(INTDIR)/BRUSHDLG.OBJ

$(OUTDIR)/GDIMETA.exe : $(OUTDIR)  $(DEF_FILE) $(LINK32_OBJS)
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

SOURCE=.\FILEDLG.C
DEP_FILED=\
	.\GLOBALS.H\
	.\gdimeta.inl

$(INTDIR)/FILEDLG.OBJ :  $(SOURCE)  $(DEP_FILED) $(INTDIR)

# End Source File
################################################################################
# Begin Source File

SOURCE=.\INIT.C
DEP_INIT_=\
	.\GLOBALS.H\
	.\PALCTRL.H\
	.\gdimeta.inl

$(INTDIR)/INIT.OBJ :  $(SOURCE)  $(DEP_INIT_) $(INTDIR)

# End Source File
################################################################################
# Begin Source File

SOURCE=.\DISPATCH.C
DEP_DISPA=\
	.\GLOBALS.H\
	.\gdimeta.inl

$(INTDIR)/DISPATCH.OBJ :  $(SOURCE)  $(DEP_DISPA) $(INTDIR)

# End Source File
################################################################################
# Begin Source File

SOURCE=.\WINMAIN.C
DEP_WINMA=\
	.\GLOBALS.H\
	.\gdimeta.inl

$(INTDIR)/WINMAIN.OBJ :  $(SOURCE)  $(DEP_WINMA) $(INTDIR)

# End Source File
################################################################################
# Begin Source File

SOURCE=.\STATBAR.C
DEP_STATB=\
	.\GLOBALS.H\
	.\STATBAR.H\
	.\gdimeta.inl

$(INTDIR)/STATBAR.OBJ :  $(SOURCE)  $(DEP_STATB) $(INTDIR)

# End Source File
################################################################################
# Begin Source File

SOURCE=.\FILE.C
DEP_FILE_=\
	.\GLOBALS.H\
	.\PALETTE.H\
	.\METAFILE.H\
	.\gdimeta.inl

$(INTDIR)/FILE.OBJ :  $(SOURCE)  $(DEP_FILE_) $(INTDIR)

# End Source File
################################################################################
# Begin Source File

SOURCE=.\COLORDLG.C
DEP_COLOR=\
	.\GLOBALS.H\
	.\gdimeta.inl

$(INTDIR)/COLORDLG.OBJ :  $(SOURCE)  $(DEP_COLOR) $(INTDIR)

# End Source File
################################################################################
# Begin Source File

SOURCE=.\TOOLBAR.C
DEP_TOOLB=\
	.\GLOBALS.H\
	.\TOOLBAR.H\
	.\gdimeta.inl

$(INTDIR)/TOOLBAR.OBJ :  $(SOURCE)  $(DEP_TOOLB) $(INTDIR)

# End Source File
################################################################################
# Begin Source File

SOURCE=.\METAFILE.C
DEP_METAF=\
	.\GLOBALS.H\
	.\METAFILE.H\
	.\PALETTE.H\
	.\gdimeta.inl

$(INTDIR)/METAFILE.OBJ :  $(SOURCE)  $(DEP_METAF) $(INTDIR)

# End Source File
################################################################################
# Begin Source File

SOURCE=.\PALETTE.C
DEP_PALET=\
	.\GLOBALS.H\
	.\PALETTE.H\
	.\gdimeta.inl

$(INTDIR)/PALETTE.OBJ :  $(SOURCE)  $(DEP_PALET) $(INTDIR)

# End Source File
################################################################################
# Begin Source File

SOURCE=.\GDIMETA.RC
DEP_GDIME=\
	.\GDIMETA.ICO\
	.\TOOLBAR.BMP\
	.\GLOBALS.H\
	.\TOOLBAR.H\
	.\STATBAR.H\
	.\PENDLG.H\
	.\BRUSHDLG.H\
	.\INFODLG.H\
	.\gdimeta.inl

$(INTDIR)/GDIMETA.res :  $(SOURCE)  $(DEP_GDIME) $(INTDIR)
   $(RSC) $(RSC_PROJ)  $(SOURCE) 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\PENDLG.C
DEP_PENDL=\
	.\GLOBALS.H\
	.\PENDLG.H\
	.\gdimeta.inl

$(INTDIR)/PENDLG.OBJ :  $(SOURCE)  $(DEP_PENDL) $(INTDIR)

# End Source File
################################################################################
# Begin Source File

SOURCE=.\MISC.C
DEP_MISC_=\
	.\GLOBALS.H\
	.\gdimeta.inl

$(INTDIR)/MISC.OBJ :  $(SOURCE)  $(DEP_MISC_) $(INTDIR)

# End Source File
################################################################################
# Begin Source File

SOURCE=.\PALCTRL.C
DEP_PALCT=\
	.\GLOBALS.H\
	.\PALETTE.H\
	.\PALCTRL.H\
	.\gdimeta.inl

$(INTDIR)/PALCTRL.OBJ :  $(SOURCE)  $(DEP_PALCT) $(INTDIR)

# End Source File
################################################################################
# Begin Source File

SOURCE=.\CLIENT.C
DEP_CLIEN=\
	.\GLOBALS.H\
	.\TOOLBAR.H\
	.\STATBAR.H\
	.\PENDLG.H\
	.\BRUSHDLG.H\
	.\PALETTE.H\
	.\METAFILE.H\
	.\gdimeta.inl

$(INTDIR)/CLIENT.OBJ :  $(SOURCE)  $(DEP_CLIEN) $(INTDIR)

# End Source File
################################################################################
# Begin Source File

SOURCE=.\INFODLG.C
DEP_INFOD=\
	.\GLOBALS.H\
	.\INFODLG.H\
	.\gdimeta.inl

$(INTDIR)/INFODLG.OBJ :  $(SOURCE)  $(DEP_INFOD) $(INTDIR)

# End Source File
################################################################################
# Begin Source File

SOURCE=.\ABOUT.C
DEP_ABOUT=\
	.\GLOBALS.H\
	.\gdimeta.inl

$(INTDIR)/ABOUT.OBJ :  $(SOURCE)  $(DEP_ABOUT) $(INTDIR)

# End Source File
################################################################################
# Begin Source File

SOURCE=.\GDIMETA.C
DEP_GDIMET=\
	.\GLOBALS.H\
	.\TOOLBAR.H\
	.\STATBAR.H\
	.\PALETTE.H\
	.\gdimeta.inl

$(INTDIR)/GDIMETA.OBJ :  $(SOURCE)  $(DEP_GDIMET) $(INTDIR)

# End Source File
################################################################################
# Begin Source File

SOURCE=.\BRUSHDLG.C
DEP_BRUSH=\
	.\GLOBALS.H\
	.\BRUSHDLG.H\
	.\gdimeta.inl

$(INTDIR)/BRUSHDLG.OBJ :  $(SOURCE)  $(DEP_BRUSH) $(INTDIR)

# End Source File
# End Group
# End Project
################################################################################
