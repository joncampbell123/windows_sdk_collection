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
!MESSAGE NMAKE /f "gdipal.mak" CFG="Win32 Debug"
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

ALL : .\WinRel\gdipal.exe .\WinRel\gdipal.bsc

$(OUTDIR) : 
    if not exist $(OUTDIR)/nul mkdir $(OUTDIR)

# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
# ADD BASE CPP /nologo /W3 /GX /YX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /FR /c
# ADD CPP /nologo /W3 /GX /YX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /FR /c
CPP_PROJ=/nologo /W3 /GX /YX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS"\
 /FR$(INTDIR)/ /Fp$(OUTDIR)/"gdipal.pch" /Fo$(INTDIR)/ /c 
CPP_OBJS=.\WinRel/
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
RSC_PROJ=/l 0x409 /fo$(INTDIR)/"GDIPAL.res" /d "NDEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o$(OUTDIR)/"gdipal.bsc" 
BSC32_SBRS= \
	.\WinRel\TOOLBAR.SBR \
	.\WinRel\PALETTE.SBR \
	.\WinRel\MISC.SBR \
	.\WinRel\WINMAIN.SBR \
	.\WinRel\ABOUT.SBR \
	.\WinRel\PALCTRL.SBR \
	.\WinRel\INIT.SBR \
	.\WinRel\PENDLG.SBR \
	.\WinRel\CLIENT.SBR \
	.\WinRel\infodlg.sbr \
	.\WinRel\DISPATCH.SBR \
	.\WinRel\COLORDLG.SBR \
	.\WinRel\STATBAR.SBR \
	.\WinRel\GDIPAL.SBR \
	.\WinRel\BRUSHDLG.SBR

.\WinRel\gdipal.bsc : $(OUTDIR)  $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /NOLOGO /SUBSYSTEM:windows /MACHINE:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib comctl32.lib version.lib /NOLOGO /MACHINE:I386 /SUBSYSTEM:windows,4.0
# SUBTRACT LINK32 /PDB:none
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib comctl32.lib\
 version.lib /NOLOGO /INCREMENTAL:no /PDB:$(OUTDIR)/"gdipal.pdb" /MACHINE:I386\
 /OUT:$(OUTDIR)/"gdipal.exe" /SUBSYSTEM:windows,4.0  
DEF_FILE=
LINK32_OBJS= \
	.\WinRel\TOOLBAR.OBJ \
	.\WinRel\PALETTE.OBJ \
	.\WinRel\MISC.OBJ \
	.\WinRel\WINMAIN.OBJ \
	.\WinRel\ABOUT.OBJ \
	.\WinRel\PALCTRL.OBJ \
	.\WinRel\INIT.OBJ \
	.\WinRel\PENDLG.OBJ \
	.\WinRel\CLIENT.OBJ \
	.\WinRel\infodlg.obj \
	.\WinRel\DISPATCH.OBJ \
	.\WinRel\GDIPAL.res \
	.\WinRel\COLORDLG.OBJ \
	.\WinRel\STATBAR.OBJ \
	.\WinRel\GDIPAL.OBJ \
	.\WinRel\BRUSHDLG.OBJ

.\WinRel\gdipal.exe : $(OUTDIR)  $(DEF_FILE) $(LINK32_OBJS)
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

ALL : .\WinDebug\gdipal.exe .\WinDebug\gdipal.bsc

$(OUTDIR) : 
    if not exist $(OUTDIR)/nul mkdir $(OUTDIR)

# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
# ADD BASE CPP /nologo /W3 /GX /Zi /YX /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /FR /c
# ADD CPP /nologo /W3 /GX /Zi /YX /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /FR /c
CPP_PROJ=/nologo /W3 /GX /Zi /YX /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /FR$(INTDIR)/ /Fp$(OUTDIR)/"gdipal.pch" /Fo$(INTDIR)/ /Fd$(OUTDIR)/"gdipal.pdb"\
 /c 
CPP_OBJS=.\WinDebug/
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
RSC_PROJ=/l 0x409 /fo$(INTDIR)/"GDIPAL.res" /d "_DEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o$(OUTDIR)/"gdipal.bsc" 
BSC32_SBRS= \
	.\WinDebug\TOOLBAR.SBR \
	.\WinDebug\PALETTE.SBR \
	.\WinDebug\MISC.SBR \
	.\WinDebug\WINMAIN.SBR \
	.\WinDebug\ABOUT.SBR \
	.\WinDebug\PALCTRL.SBR \
	.\WinDebug\INIT.SBR \
	.\WinDebug\PENDLG.SBR \
	.\WinDebug\CLIENT.SBR \
	.\WinDebug\infodlg.sbr \
	.\WinDebug\DISPATCH.SBR \
	.\WinDebug\COLORDLG.SBR \
	.\WinDebug\STATBAR.SBR \
	.\WinDebug\GDIPAL.SBR \
	.\WinDebug\BRUSHDLG.SBR

.\WinDebug\gdipal.bsc : $(OUTDIR)  $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /NOLOGO /SUBSYSTEM:windows /DEBUG /MACHINE:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib comctl32.lib version.lib /NOLOGO /DEBUG /MACHINE:I386 /SUBSYSTEM:windows,4.0
# SUBTRACT LINK32 /PDB:none
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib comctl32.lib\
 version.lib /NOLOGO /INCREMENTAL:yes /PDB:$(OUTDIR)/"gdipal.pdb" /DEBUG\
 /MACHINE:I386 /OUT:$(OUTDIR)/"gdipal.exe" /SUBSYSTEM:windows,4.0  
DEF_FILE=
LINK32_OBJS= \
	.\WinDebug\TOOLBAR.OBJ \
	.\WinDebug\PALETTE.OBJ \
	.\WinDebug\MISC.OBJ \
	.\WinDebug\WINMAIN.OBJ \
	.\WinDebug\ABOUT.OBJ \
	.\WinDebug\PALCTRL.OBJ \
	.\WinDebug\INIT.OBJ \
	.\WinDebug\PENDLG.OBJ \
	.\WinDebug\CLIENT.OBJ \
	.\WinDebug\infodlg.obj \
	.\WinDebug\DISPATCH.OBJ \
	.\WinDebug\GDIPAL.res \
	.\WinDebug\COLORDLG.OBJ \
	.\WinDebug\STATBAR.OBJ \
	.\WinDebug\GDIPAL.OBJ \
	.\WinDebug\BRUSHDLG.OBJ

.\WinDebug\gdipal.exe : $(OUTDIR)  $(DEF_FILE) $(LINK32_OBJS)
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

SOURCE=.\MISC.C
DEP_MISC_=\
	.\GLOBALS.H

!IF  "$(CFG)" == "Win32 Release"

.\WinRel\MISC.OBJ :  $(SOURCE)  $(DEP_MISC_) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 Debug"

.\WinDebug\MISC.OBJ :  $(SOURCE)  $(DEP_MISC_) $(INTDIR)

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
################################################################################
# Begin Source File

SOURCE=.\CLIENT.C
DEP_CLIEN=\
	.\GLOBALS.H\
	.\TOOLBAR.H\
	.\STATBAR.H\
	.\PENDLG.H\
	.\BRUSHDLG.H\
	.\PALETTE.H

!IF  "$(CFG)" == "Win32 Release"

.\WinRel\CLIENT.OBJ :  $(SOURCE)  $(DEP_CLIEN) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 Debug"

.\WinDebug\CLIENT.OBJ :  $(SOURCE)  $(DEP_CLIEN) $(INTDIR)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\infodlg.c
DEP_INFOD=\
	.\GLOBALS.H\
	.\INFODLG.H

!IF  "$(CFG)" == "Win32 Release"

.\WinRel\infodlg.obj :  $(SOURCE)  $(DEP_INFOD) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 Debug"

.\WinDebug\infodlg.obj :  $(SOURCE)  $(DEP_INFOD) $(INTDIR)

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

SOURCE=.\GDIPAL.RC
DEP_GDIPA=\
	.\GDIPal.ICO\
	.\TOOLBAR.BMP\
	.\GLOBALS.H\
	.\TOOLBAR.H\
	.\STATBAR.H\
	.\PENDLG.H\
	.\BRUSHDLG.H\
	.\INFODLG.H

!IF  "$(CFG)" == "Win32 Release"

.\WinRel\GDIPAL.res :  $(SOURCE)  $(DEP_GDIPA) $(INTDIR)
   $(RSC) $(RSC_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 Debug"

.\WinDebug\GDIPAL.res :  $(SOURCE)  $(DEP_GDIPA) $(INTDIR)
   $(RSC) $(RSC_PROJ)  $(SOURCE) 

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

SOURCE=.\GDIPAL.C
DEP_GDIPAL=\
	.\GLOBALS.H\
	.\TOOLBAR.H\
	.\STATBAR.H\
	.\PALETTE.H

!IF  "$(CFG)" == "Win32 Release"

.\WinRel\GDIPAL.OBJ :  $(SOURCE)  $(DEP_GDIPAL) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 Debug"

.\WinDebug\GDIPAL.OBJ :  $(SOURCE)  $(DEP_GDIPAL) $(INTDIR)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\BRUSHDLG.C
DEP_BRUSH=\
	.\GLOBALS.H\
	.\BRUSHDLG.H

!IF  "$(CFG)" == "Win32 Release"

.\WinRel\BRUSHDLG.OBJ :  $(SOURCE)  $(DEP_BRUSH) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 Debug"

.\WinDebug\BRUSHDLG.OBJ :  $(SOURCE)  $(DEP_BRUSH) $(INTDIR)

!ENDIF 

# End Source File
# End Group
# End Project
################################################################################
