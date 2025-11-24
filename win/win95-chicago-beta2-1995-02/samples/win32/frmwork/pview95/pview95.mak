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
!MESSAGE NMAKE /f "PVIEW95.MAK" CFG="Win32 Debug"
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

ALL : $(OUTDIR)/PVIEW95.exe $(OUTDIR)/PVIEW95.bsc

$(OUTDIR) : 
    if not exist $(OUTDIR)/nul mkdir $(OUTDIR)

# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
# ADD BASE CPP /nologo /W3 /GX /YX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /FR /c
# ADD CPP /nologo /W3 /GX /YX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /FR /c
# SUBTRACT CPP /X
CPP_PROJ=/nologo /W3 /GX /YX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS"\
 /FR$(INTDIR)/ /Fp$(OUTDIR)/"PVIEW95.pch" /Fo$(INTDIR)/ /c 
CPP_OBJS=.\WinRel/
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
# SUBTRACT RSC /x
RSC_PROJ=/l 0x409 /fo$(INTDIR)/"pview95.res" /d "NDEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o$(OUTDIR)/"PVIEW95.bsc" 
BSC32_SBRS= \
	$(INTDIR)/DISPATCH.SBR \
	$(INTDIR)/ABOUT.SBR \
	$(INTDIR)/MISC.SBR \
	$(INTDIR)/WINMAIN.SBR \
	$(INTDIR)/INIT.SBR \
	$(INTDIR)/PVIEW95.SBR \
	$(INTDIR)/LISTVIEW.SBR \
	$(INTDIR)/TOOLBAR.SBR \
	$(INTDIR)/PROCTHRD.SBR

$(OUTDIR)/PVIEW95.bsc : $(OUTDIR)  $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /NOLOGO /SUBSYSTEM:windows /MACHINE:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib comctl32.lib version.lib /NOLOGO /MACHINE:I386 /SUBSYSTEM:windows,4.0
# SUBTRACT LINK32 /PDB:none
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib comctl32.lib version.lib /NOLOGO\
 /INCREMENTAL:no /PDB:$(OUTDIR)/"PVIEW95.pdb" /MACHINE:I386\
 /OUT:$(OUTDIR)/"PVIEW95.exe" /SUBSYSTEM:windows,4.0  
DEF_FILE=
LINK32_OBJS= \
	$(INTDIR)/DISPATCH.OBJ \
	$(INTDIR)/ABOUT.OBJ \
	$(INTDIR)/MISC.OBJ \
	$(INTDIR)/WINMAIN.OBJ \
	$(INTDIR)/INIT.OBJ \
	$(INTDIR)/PVIEW95.OBJ \
	$(INTDIR)/pview95.res \
	$(INTDIR)/LISTVIEW.OBJ \
	$(INTDIR)/TOOLBAR.OBJ \
	$(INTDIR)/PROCTHRD.OBJ

$(OUTDIR)/PVIEW95.exe : $(OUTDIR)  $(DEF_FILE) $(LINK32_OBJS)
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

ALL : $(OUTDIR)/PVIEW95.exe $(OUTDIR)/PVIEW95.bsc

$(OUTDIR) : 
    if not exist $(OUTDIR)/nul mkdir $(OUTDIR)

# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
# ADD BASE CPP /nologo /W3 /GX /Zi /YX /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /FR /c
# ADD CPP /nologo /W3 /GX /Zi /YX /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /FR /c
# SUBTRACT CPP /X
CPP_PROJ=/nologo /W3 /GX /Zi /YX /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /FR$(INTDIR)/ /Fp$(OUTDIR)/"PVIEW95.pch" /Fo$(INTDIR)/\
 /Fd$(OUTDIR)/"PVIEW95.pdb" /c 
CPP_OBJS=.\WinDebug/
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
# SUBTRACT RSC /x
RSC_PROJ=/l 0x409 /fo$(INTDIR)/"pview95.res" /d "_DEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o$(OUTDIR)/"PVIEW95.bsc" 
BSC32_SBRS= \
	$(INTDIR)/DISPATCH.SBR \
	$(INTDIR)/ABOUT.SBR \
	$(INTDIR)/MISC.SBR \
	$(INTDIR)/WINMAIN.SBR \
	$(INTDIR)/INIT.SBR \
	$(INTDIR)/PVIEW95.SBR \
	$(INTDIR)/LISTVIEW.SBR \
	$(INTDIR)/TOOLBAR.SBR \
	$(INTDIR)/PROCTHRD.SBR

$(OUTDIR)/PVIEW95.bsc : $(OUTDIR)  $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /NOLOGO /SUBSYSTEM:windows /DEBUG /MACHINE:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib comctl32.lib version.lib /NOLOGO /DEBUG /MACHINE:I386 /SUBSYSTEM:windows,4.0
# SUBTRACT LINK32 /PDB:none
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib comctl32.lib version.lib /NOLOGO\
 /INCREMENTAL:yes /PDB:$(OUTDIR)/"PVIEW95.pdb" /DEBUG /MACHINE:I386\
 /OUT:$(OUTDIR)/"PVIEW95.exe" /SUBSYSTEM:windows,4.0  
DEF_FILE=
LINK32_OBJS= \
	$(INTDIR)/DISPATCH.OBJ \
	$(INTDIR)/ABOUT.OBJ \
	$(INTDIR)/MISC.OBJ \
	$(INTDIR)/WINMAIN.OBJ \
	$(INTDIR)/INIT.OBJ \
	$(INTDIR)/PVIEW95.OBJ \
	$(INTDIR)/pview95.res \
	$(INTDIR)/LISTVIEW.OBJ \
	$(INTDIR)/TOOLBAR.OBJ \
	$(INTDIR)/PROCTHRD.OBJ

$(OUTDIR)/PVIEW95.exe : $(OUTDIR)  $(DEF_FILE) $(LINK32_OBJS)
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

SOURCE=.\DISPATCH.C
DEP_DISPA=\
	.\globals.h

$(INTDIR)/DISPATCH.OBJ :  $(SOURCE)  $(DEP_DISPA) $(INTDIR)

# End Source File
################################################################################
# Begin Source File

SOURCE=.\ABOUT.C
DEP_ABOUT=\
	.\globals.h

$(INTDIR)/ABOUT.OBJ :  $(SOURCE)  $(DEP_ABOUT) $(INTDIR)

# End Source File
################################################################################
# Begin Source File

SOURCE=.\MISC.C
DEP_MISC_=\
	.\globals.h

$(INTDIR)/MISC.OBJ :  $(SOURCE)  $(DEP_MISC_) $(INTDIR)

# End Source File
################################################################################
# Begin Source File

SOURCE=.\WINMAIN.C
DEP_WINMA=\
	.\globals.h

$(INTDIR)/WINMAIN.OBJ :  $(SOURCE)  $(DEP_WINMA) $(INTDIR)

# End Source File
################################################################################
# Begin Source File

SOURCE=.\INIT.C
DEP_INIT_=\
	.\globals.h\
	.\PROCTHRD.H

$(INTDIR)/INIT.OBJ :  $(SOURCE)  $(DEP_INIT_) $(INTDIR)

# End Source File
################################################################################
# Begin Source File

SOURCE=.\PVIEW95.C
DEP_PVIEW=\
	.\globals.h\
	.\LISTVIEW.H\
	.\TOOLBAR.H\
	.\PROCTHRD.H

$(INTDIR)/PVIEW95.OBJ :  $(SOURCE)  $(DEP_PVIEW) $(INTDIR)

# End Source File
################################################################################
# Begin Source File

SOURCE=.\pview95.rc
DEP_PVIEW9=\
	.\PVIEW95.ICO\
	.\TOOLBAR.BMP\
	.\RESIZE.CUR\
	.\globals.h

$(INTDIR)/pview95.res :  $(SOURCE)  $(DEP_PVIEW9) $(INTDIR)
   $(RSC) $(RSC_PROJ)  $(SOURCE) 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\LISTVIEW.C
DEP_LISTV=\
	.\globals.h\
	.\LISTVIEW.H\
	.\PROCTHRD.H

$(INTDIR)/LISTVIEW.OBJ :  $(SOURCE)  $(DEP_LISTV) $(INTDIR)

# End Source File
################################################################################
# Begin Source File

SOURCE=.\TOOLBAR.C
DEP_TOOLB=\
	.\globals.h

$(INTDIR)/TOOLBAR.OBJ :  $(SOURCE)  $(DEP_TOOLB) $(INTDIR)

# End Source File
################################################################################
# Begin Source File

SOURCE=.\PROCTHRD.C
DEP_PROCT=\
	.\PROCTHRD.H\
	.\LISTVIEW.H\
	.\globals.h

$(INTDIR)/PROCTHRD.OBJ :  $(SOURCE)  $(DEP_PROCT) $(INTDIR)

# End Source File
# End Group
# End Project
################################################################################
