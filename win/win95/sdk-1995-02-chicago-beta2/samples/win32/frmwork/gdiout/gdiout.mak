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
!MESSAGE NMAKE /f "gdiout.mak" CFG="Win32 Debug"
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

ALL : .\WinRel\gdiout.exe .\WinRel\gdiout.bsc

$(OUTDIR) : 
    if not exist $(OUTDIR)/nul mkdir $(OUTDIR)

# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
# ADD BASE CPP /nologo /W3 /GX /YX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /FR /c
# ADD CPP /nologo /W3 /GX /YX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /FR /c
CPP_PROJ=/nologo /W3 /GX /YX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS"\
 /FR$(INTDIR)/ /Fp$(OUTDIR)/"gdiout.pch" /Fo$(INTDIR)/ /c 
CPP_OBJS=.\WinRel/
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
RSC_PROJ=/l 0x409 /fo$(INTDIR)/"GDIOUT.res" /d "NDEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o$(OUTDIR)/"gdiout.bsc" 
BSC32_SBRS= \
	.\WinRel\INIT.SBR \
	.\WinRel\ABOUT.SBR \
	.\WinRel\WINMAIN.SBR \
	.\WinRel\MISC.SBR \
	.\WinRel\DISPATCH.SBR \
	.\WinRel\GDIOUT.SBR

.\WinRel\gdiout.bsc : $(OUTDIR)  $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /NOLOGO /SUBSYSTEM:windows /MACHINE:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib version.lib /NOLOGO /MACHINE:I386 /SUBSYSTEM:windows,4.0
# SUBTRACT LINK32 /PDB:none
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib version.lib /NOLOGO\
 /INCREMENTAL:no /PDB:$(OUTDIR)/"gdiout.pdb" /MACHINE:I386\
 /OUT:$(OUTDIR)/"gdiout.exe" /SUBSYSTEM:windows,4.0  
DEF_FILE=
LINK32_OBJS= \
	.\WinRel\INIT.OBJ \
	.\WinRel\ABOUT.OBJ \
	.\WinRel\GDIOUT.res \
	.\WinRel\WINMAIN.OBJ \
	.\WinRel\MISC.OBJ \
	.\WinRel\DISPATCH.OBJ \
	.\WinRel\GDIOUT.OBJ

.\WinRel\gdiout.exe : $(OUTDIR)  $(DEF_FILE) $(LINK32_OBJS)
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

ALL : .\WinDebug\gdiout.exe .\WinDebug\gdiout.bsc

$(OUTDIR) : 
    if not exist $(OUTDIR)/nul mkdir $(OUTDIR)

# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
# ADD BASE CPP /nologo /W3 /GX /Zi /YX /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /FR /c
# ADD CPP /nologo /W3 /GX /Zi /YX /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /FR /c
CPP_PROJ=/nologo /W3 /GX /Zi /YX /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /FR$(INTDIR)/ /Fp$(OUTDIR)/"gdiout.pch" /Fo$(INTDIR)/ /Fd$(OUTDIR)/"gdiout.pdb"\
 /c 
CPP_OBJS=.\WinDebug/
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
RSC_PROJ=/l 0x409 /fo$(INTDIR)/"GDIOUT.res" /d "_DEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o$(OUTDIR)/"gdiout.bsc" 
BSC32_SBRS= \
	.\WinDebug\INIT.SBR \
	.\WinDebug\ABOUT.SBR \
	.\WinDebug\WINMAIN.SBR \
	.\WinDebug\MISC.SBR \
	.\WinDebug\DISPATCH.SBR \
	.\WinDebug\GDIOUT.SBR

.\WinDebug\gdiout.bsc : $(OUTDIR)  $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /NOLOGO /SUBSYSTEM:windows /DEBUG /MACHINE:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib version.lib /NOLOGO /DEBUG /MACHINE:I386 /SUBSYSTEM:windows,4.0
# SUBTRACT LINK32 /PDB:none
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib version.lib /NOLOGO\
 /INCREMENTAL:yes /PDB:$(OUTDIR)/"gdiout.pdb" /DEBUG /MACHINE:I386\
 /OUT:$(OUTDIR)/"gdiout.exe" /SUBSYSTEM:windows,4.0  
DEF_FILE=
LINK32_OBJS= \
	.\WinDebug\INIT.OBJ \
	.\WinDebug\ABOUT.OBJ \
	.\WinDebug\GDIOUT.res \
	.\WinDebug\WINMAIN.OBJ \
	.\WinDebug\MISC.OBJ \
	.\WinDebug\DISPATCH.OBJ \
	.\WinDebug\GDIOUT.OBJ

.\WinDebug\gdiout.exe : $(OUTDIR)  $(DEF_FILE) $(LINK32_OBJS)
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
	.\WIN16EXT.H\
	.\GLOBALS.H

!IF  "$(CFG)" == "Win32 Release"

.\WinRel\INIT.OBJ :  $(SOURCE)  $(DEP_INIT_) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 Debug"

.\WinDebug\INIT.OBJ :  $(SOURCE)  $(DEP_INIT_) $(INTDIR)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\ABOUT.C
DEP_ABOUT=\
	.\WIN16EXT.H\
	.\GLOBALS.H

!IF  "$(CFG)" == "Win32 Release"

.\WinRel\ABOUT.OBJ :  $(SOURCE)  $(DEP_ABOUT) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 Debug"

.\WinDebug\ABOUT.OBJ :  $(SOURCE)  $(DEP_ABOUT) $(INTDIR)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\GDIOUT.RC
DEP_GDIOU=\
	.\GDIOut.ICO\
	.\GLOBALS.H\
	.\version.h

!IF  "$(CFG)" == "Win32 Release"

.\WinRel\GDIOUT.res :  $(SOURCE)  $(DEP_GDIOU) $(INTDIR)
   $(RSC) $(RSC_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 Debug"

.\WinDebug\GDIOUT.res :  $(SOURCE)  $(DEP_GDIOU) $(INTDIR)
   $(RSC) $(RSC_PROJ)  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\WINMAIN.C
DEP_WINMA=\
	.\WIN16EXT.H\
	.\GLOBALS.H

!IF  "$(CFG)" == "Win32 Release"

.\WinRel\WINMAIN.OBJ :  $(SOURCE)  $(DEP_WINMA) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 Debug"

.\WinDebug\WINMAIN.OBJ :  $(SOURCE)  $(DEP_WINMA) $(INTDIR)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\MISC.C
DEP_MISC_=\
	.\WIN16EXT.H\
	.\GLOBALS.H

!IF  "$(CFG)" == "Win32 Release"

.\WinRel\MISC.OBJ :  $(SOURCE)  $(DEP_MISC_) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 Debug"

.\WinDebug\MISC.OBJ :  $(SOURCE)  $(DEP_MISC_) $(INTDIR)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\DISPATCH.C
DEP_DISPA=\
	.\WIN16EXT.H\
	.\GLOBALS.H

!IF  "$(CFG)" == "Win32 Release"

.\WinRel\DISPATCH.OBJ :  $(SOURCE)  $(DEP_DISPA) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 Debug"

.\WinDebug\DISPATCH.OBJ :  $(SOURCE)  $(DEP_DISPA) $(INTDIR)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\GDIOUT.C
DEP_GDIOUT=\
	.\WIN16EXT.H\
	.\GLOBALS.H

!IF  "$(CFG)" == "Win32 Release"

.\WinRel\GDIOUT.OBJ :  $(SOURCE)  $(DEP_GDIOUT) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 Debug"

.\WinDebug\GDIOUT.OBJ :  $(SOURCE)  $(DEP_GDIOUT) $(INTDIR)

!ENDIF 

# End Source File
# End Group
# End Project
################################################################################
