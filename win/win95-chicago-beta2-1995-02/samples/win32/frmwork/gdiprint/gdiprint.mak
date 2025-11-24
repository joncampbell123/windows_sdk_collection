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
!MESSAGE NMAKE /f "gdiprint.mak" CFG="Win32 Debug"
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

ALL : .\WinRel\gdiprint.exe .\WinRel\gdiprint.bsc

$(OUTDIR) : 
    if not exist $(OUTDIR)/nul mkdir $(OUTDIR)

# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
# ADD BASE CPP /nologo /W3 /GX /YX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /FR /c
# ADD CPP /nologo /W3 /GX /YX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /FR /c
CPP_PROJ=/nologo /W3 /GX /YX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS"\
 /FR$(INTDIR)/ /Fp$(OUTDIR)/"gdiprint.pch" /Fo$(INTDIR)/ /c 
CPP_OBJS=.\WinRel/
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
RSC_PROJ=/l 0x409 /fo$(INTDIR)/"GDIPRINT.res" /d "NDEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o$(OUTDIR)/"gdiprint.bsc" 
BSC32_SBRS= \
	.\WinRel\INIT.SBR \
	.\WinRel\PALCTRL.SBR \
	.\WinRel\DIBUTIL.SBR \
	.\WinRel\CLIENT.SBR \
	.\WinRel\BRUSHDLG.SBR \
	.\WinRel\INFODLG.SBR \
	.\WinRel\FILEIO.SBR \
	.\WinRel\FILEDLG.SBR \
	.\WinRel\DISPATCH.SBR \
	.\WinRel\WINMAIN.SBR \
	.\WinRel\STATBAR.SBR \
	.\WinRel\FILENEW.SBR \
	.\WinRel\COLORDLG.SBR \
	.\WinRel\ABOUT.SBR \
	.\WinRel\TOOLBAR.SBR \
	.\WinRel\MISC.SBR \
	.\WinRel\GDIPRINT.SBR \
	.\WinRel\PALETTE.SBR \
	.\WinRel\PRINT.SBR \
	.\WinRel\PENDLG.SBR

.\WinRel\gdiprint.bsc : $(OUTDIR)  $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /NOLOGO /SUBSYSTEM:windows /MACHINE:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib comctl32.lib version.lib /NOLOGO /MACHINE:I386 /SUBSYSTEM:windows,4.0
# SUBTRACT LINK32 /PDB:none
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib comctl32.lib\
 version.lib /NOLOGO /INCREMENTAL:no /PDB:$(OUTDIR)/"gdiprint.pdb" /MACHINE:I386\
 /OUT:$(OUTDIR)/"gdiprint.exe" /SUBSYSTEM:windows,4.0  
DEF_FILE=
LINK32_OBJS= \
	.\WinRel\INIT.OBJ \
	.\WinRel\PALCTRL.OBJ \
	.\WinRel\DIBUTIL.OBJ \
	.\WinRel\CLIENT.OBJ \
	.\WinRel\BRUSHDLG.OBJ \
	.\WinRel\INFODLG.OBJ \
	.\WinRel\FILEIO.OBJ \
	.\WinRel\FILEDLG.OBJ \
	.\WinRel\DISPATCH.OBJ \
	.\WinRel\WINMAIN.OBJ \
	.\WinRel\STATBAR.OBJ \
	.\WinRel\FILENEW.OBJ \
	.\WinRel\COLORDLG.OBJ \
	.\WinRel\ABOUT.OBJ \
	.\WinRel\GDIPRINT.res \
	.\WinRel\TOOLBAR.OBJ \
	.\WinRel\MISC.OBJ \
	.\WinRel\GDIPRINT.OBJ \
	.\WinRel\PALETTE.OBJ \
	.\WinRel\PRINT.OBJ \
	.\WinRel\PENDLG.OBJ

.\WinRel\gdiprint.exe : $(OUTDIR)  $(DEF_FILE) $(LINK32_OBJS)
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

ALL : .\WinDebug\gdiprint.exe .\WinDebug\gdiprint.bsc

$(OUTDIR) : 
    if not exist $(OUTDIR)/nul mkdir $(OUTDIR)

# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
# ADD BASE CPP /nologo /W3 /GX /Zi /YX /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /FR /c
# ADD CPP /nologo /W3 /GX /Zi /YX /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /FR /c
CPP_PROJ=/nologo /W3 /GX /Zi /YX /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /FR$(INTDIR)/ /Fp$(OUTDIR)/"gdiprint.pch" /Fo$(INTDIR)/\
 /Fd$(OUTDIR)/"gdiprint.pdb" /c 
CPP_OBJS=.\WinDebug/
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
RSC_PROJ=/l 0x409 /fo$(INTDIR)/"GDIPRINT.res" /d "_DEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o$(OUTDIR)/"gdiprint.bsc" 
BSC32_SBRS= \
	.\WinDebug\INIT.SBR \
	.\WinDebug\PALCTRL.SBR \
	.\WinDebug\DIBUTIL.SBR \
	.\WinDebug\CLIENT.SBR \
	.\WinDebug\BRUSHDLG.SBR \
	.\WinDebug\INFODLG.SBR \
	.\WinDebug\FILEIO.SBR \
	.\WinDebug\FILEDLG.SBR \
	.\WinDebug\DISPATCH.SBR \
	.\WinDebug\WINMAIN.SBR \
	.\WinDebug\STATBAR.SBR \
	.\WinDebug\FILENEW.SBR \
	.\WinDebug\COLORDLG.SBR \
	.\WinDebug\ABOUT.SBR \
	.\WinDebug\TOOLBAR.SBR \
	.\WinDebug\MISC.SBR \
	.\WinDebug\GDIPRINT.SBR \
	.\WinDebug\PALETTE.SBR \
	.\WinDebug\PRINT.SBR \
	.\WinDebug\PENDLG.SBR

.\WinDebug\gdiprint.bsc : $(OUTDIR)  $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /NOLOGO /SUBSYSTEM:windows /DEBUG /MACHINE:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib comctl32.lib version.lib /NOLOGO /DEBUG /MACHINE:I386 /SUBSYSTEM:windows,4.0
# SUBTRACT LINK32 /PDB:none
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib comctl32.lib\
 version.lib /NOLOGO /INCREMENTAL:yes /PDB:$(OUTDIR)/"gdiprint.pdb" /DEBUG\
 /MACHINE:I386 /OUT:$(OUTDIR)/"gdiprint.exe" /SUBSYSTEM:windows,4.0  
DEF_FILE=
LINK32_OBJS= \
	.\WinDebug\INIT.OBJ \
	.\WinDebug\PALCTRL.OBJ \
	.\WinDebug\DIBUTIL.OBJ \
	.\WinDebug\CLIENT.OBJ \
	.\WinDebug\BRUSHDLG.OBJ \
	.\WinDebug\INFODLG.OBJ \
	.\WinDebug\FILEIO.OBJ \
	.\WinDebug\FILEDLG.OBJ \
	.\WinDebug\DISPATCH.OBJ \
	.\WinDebug\WINMAIN.OBJ \
	.\WinDebug\STATBAR.OBJ \
	.\WinDebug\FILENEW.OBJ \
	.\WinDebug\COLORDLG.OBJ \
	.\WinDebug\ABOUT.OBJ \
	.\WinDebug\GDIPRINT.res \
	.\WinDebug\TOOLBAR.OBJ \
	.\WinDebug\MISC.OBJ \
	.\WinDebug\GDIPRINT.OBJ \
	.\WinDebug\PALETTE.OBJ \
	.\WinDebug\PRINT.OBJ \
	.\WinDebug\PENDLG.OBJ

.\WinDebug\gdiprint.exe : $(OUTDIR)  $(DEF_FILE) $(LINK32_OBJS)
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

SOURCE=.\INIT.C
DEP_INIT_=\
	.\GLOBALS.H\
	.\PALCTRL.H

!IF  "$(CFG)" == "Win32 Release"

.\WinRel\INIT.OBJ :  $(SOURCE)  $(DEP_INIT_) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 Debug"

.\WinDebug\INIT.OBJ :  $(SOURCE)  $(DEP_INIT_) $(INTDIR)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\PALCTRL.C
DEP_PALCT=\
	.\GLOBALS.H\
	.\PALETTE.H\
	.\PALCTRL.H

!IF  "$(CFG)" == "Win32 Release"

.\WinRel\PALCTRL.OBJ :  $(SOURCE)  $(DEP_PALCT) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 Debug"

.\WinDebug\PALCTRL.OBJ :  $(SOURCE)  $(DEP_PALCT) $(INTDIR)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\DIBUTIL.C
DEP_DIBUT=\
	.\GLOBALS.H\
	.\PALETTE.H\
	.\DIBUTIL.H

!IF  "$(CFG)" == "Win32 Release"

.\WinRel\DIBUTIL.OBJ :  $(SOURCE)  $(DEP_DIBUT) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 Debug"

.\WinDebug\DIBUTIL.OBJ :  $(SOURCE)  $(DEP_DIBUT) $(INTDIR)

!ENDIF 

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
	.\DIBUTIL.H

!IF  "$(CFG)" == "Win32 Release"

.\WinRel\CLIENT.OBJ :  $(SOURCE)  $(DEP_CLIEN) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 Debug"

.\WinDebug\CLIENT.OBJ :  $(SOURCE)  $(DEP_CLIEN) $(INTDIR)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\BRUSHDLG.C
DEP_BRUSH=\
	.\GLOBALS.H\
	.\BRUSHDLG.H\
	.\PALETTE.H

!IF  "$(CFG)" == "Win32 Release"

.\WinRel\BRUSHDLG.OBJ :  $(SOURCE)  $(DEP_BRUSH) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 Debug"

.\WinDebug\BRUSHDLG.OBJ :  $(SOURCE)  $(DEP_BRUSH) $(INTDIR)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\INFODLG.C
DEP_INFOD=\
	.\GLOBALS.H\
	.\INFODLG.H

!IF  "$(CFG)" == "Win32 Release"

.\WinRel\INFODLG.OBJ :  $(SOURCE)  $(DEP_INFOD) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 Debug"

.\WinDebug\INFODLG.OBJ :  $(SOURCE)  $(DEP_INFOD) $(INTDIR)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\FILEIO.C
DEP_FILEI=\
	.\GLOBALS.H\
	.\TOOLBAR.H\
	.\PALETTE.H\
	.\DIBUTIL.H

!IF  "$(CFG)" == "Win32 Release"

.\WinRel\FILEIO.OBJ :  $(SOURCE)  $(DEP_FILEI) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 Debug"

.\WinDebug\FILEIO.OBJ :  $(SOURCE)  $(DEP_FILEI) $(INTDIR)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\FILEDLG.C
DEP_FILED=\
	.\GLOBALS.H\
	.\STATBAR.H\
	.\TOOLBAR.H\
	.\PALETTE.H\
	.\DIBUTIL.H

!IF  "$(CFG)" == "Win32 Release"

.\WinRel\FILEDLG.OBJ :  $(SOURCE)  $(DEP_FILED) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 Debug"

.\WinDebug\FILEDLG.OBJ :  $(SOURCE)  $(DEP_FILED) $(INTDIR)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\DISPATCH.C
DEP_DISPA=\
	.\GLOBALS.H

!IF  "$(CFG)" == "Win32 Release"

.\WinRel\DISPATCH.OBJ :  $(SOURCE)  $(DEP_DISPA) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 Debug"

.\WinDebug\DISPATCH.OBJ :  $(SOURCE)  $(DEP_DISPA) $(INTDIR)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\WINMAIN.C
DEP_WINMA=\
	.\GLOBALS.H

!IF  "$(CFG)" == "Win32 Release"

.\WinRel\WINMAIN.OBJ :  $(SOURCE)  $(DEP_WINMA) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 Debug"

.\WinDebug\WINMAIN.OBJ :  $(SOURCE)  $(DEP_WINMA) $(INTDIR)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\STATBAR.C
DEP_STATB=\
	.\GLOBALS.H\
	.\STATBAR.H

!IF  "$(CFG)" == "Win32 Release"

.\WinRel\STATBAR.OBJ :  $(SOURCE)  $(DEP_STATB) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 Debug"

.\WinDebug\STATBAR.OBJ :  $(SOURCE)  $(DEP_STATB) $(INTDIR)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\FILENEW.C
DEP_FILEN=\
	.\GLOBALS.H\
	.\TOOLBAR.H\
	.\STATBAR.H\
	.\PALETTE.H\
	.\DIBUTIL.H

!IF  "$(CFG)" == "Win32 Release"

.\WinRel\FILENEW.OBJ :  $(SOURCE)  $(DEP_FILEN) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 Debug"

.\WinDebug\FILENEW.OBJ :  $(SOURCE)  $(DEP_FILEN) $(INTDIR)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\COLORDLG.C
DEP_COLOR=\
	.\GLOBALS.H

!IF  "$(CFG)" == "Win32 Release"

.\WinRel\COLORDLG.OBJ :  $(SOURCE)  $(DEP_COLOR) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 Debug"

.\WinDebug\COLORDLG.OBJ :  $(SOURCE)  $(DEP_COLOR) $(INTDIR)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\ABOUT.C
DEP_ABOUT=\
	.\GLOBALS.H

!IF  "$(CFG)" == "Win32 Release"

.\WinRel\ABOUT.OBJ :  $(SOURCE)  $(DEP_ABOUT) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 Debug"

.\WinDebug\ABOUT.OBJ :  $(SOURCE)  $(DEP_ABOUT) $(INTDIR)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\GDIPRINT.RC
DEP_GDIPR=\
	.\GDIPRINT.ICO\
	.\TOOLBAR.BMP\
	.\GLOBALS.H\
	.\TOOLBAR.H\
	.\STATBAR.H\
	.\PENDLG.H\
	.\BRUSHDLG.H\
	.\INFODLG.H\
	.\PRINT.H

!IF  "$(CFG)" == "Win32 Release"

.\WinRel\GDIPRINT.res :  $(SOURCE)  $(DEP_GDIPR) $(INTDIR)
   $(RSC) $(RSC_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 Debug"

.\WinDebug\GDIPRINT.res :  $(SOURCE)  $(DEP_GDIPR) $(INTDIR)
   $(RSC) $(RSC_PROJ)  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\TOOLBAR.C
DEP_TOOLB=\
	.\GLOBALS.H\
	.\TOOLBAR.H

!IF  "$(CFG)" == "Win32 Release"

.\WinRel\TOOLBAR.OBJ :  $(SOURCE)  $(DEP_TOOLB) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 Debug"

.\WinDebug\TOOLBAR.OBJ :  $(SOURCE)  $(DEP_TOOLB) $(INTDIR)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\MISC.C
DEP_MISC_=\
	.\GLOBALS.H\
	.\TOOLBAR.H

!IF  "$(CFG)" == "Win32 Release"

.\WinRel\MISC.OBJ :  $(SOURCE)  $(DEP_MISC_) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 Debug"

.\WinDebug\MISC.OBJ :  $(SOURCE)  $(DEP_MISC_) $(INTDIR)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\GDIPRINT.C
DEP_GDIPRI=\
	.\GLOBALS.H\
	.\TOOLBAR.H\
	.\STATBAR.H\
	.\PALETTE.H

!IF  "$(CFG)" == "Win32 Release"

.\WinRel\GDIPRINT.OBJ :  $(SOURCE)  $(DEP_GDIPRI) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 Debug"

.\WinDebug\GDIPRINT.OBJ :  $(SOURCE)  $(DEP_GDIPRI) $(INTDIR)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\PALETTE.C
DEP_PALET=\
	.\GLOBALS.H\
	.\PALETTE.H

!IF  "$(CFG)" == "Win32 Release"

.\WinRel\PALETTE.OBJ :  $(SOURCE)  $(DEP_PALET) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 Debug"

.\WinDebug\PALETTE.OBJ :  $(SOURCE)  $(DEP_PALET) $(INTDIR)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\PRINT.C
DEP_PRINT=\
	.\GLOBALS.H\
	.\DIBUTIL.H\
	.\STATBAR.H\
	.\PRINT.H

!IF  "$(CFG)" == "Win32 Release"

.\WinRel\PRINT.OBJ :  $(SOURCE)  $(DEP_PRINT) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 Debug"

.\WinDebug\PRINT.OBJ :  $(SOURCE)  $(DEP_PRINT) $(INTDIR)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\PENDLG.C
DEP_PENDL=\
	.\GLOBALS.H\
	.\PENDLG.H

!IF  "$(CFG)" == "Win32 Release"

.\WinRel\PENDLG.OBJ :  $(SOURCE)  $(DEP_PENDL) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 Debug"

.\WinDebug\PENDLG.OBJ :  $(SOURCE)  $(DEP_PENDL) $(INTDIR)

!ENDIF 

# End Source File
# End Group
# End Project
################################################################################
