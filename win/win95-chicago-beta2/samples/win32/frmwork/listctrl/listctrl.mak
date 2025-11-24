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
!MESSAGE NMAKE /f "LISTCTRL.MAK" CFG="Win32 Debug"
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

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "WinRel"
# PROP BASE Intermediate_Dir "WinRel"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "WinRel"
# PROP Intermediate_Dir "WinRel"
OUTDIR=.\WinRel
INTDIR=.\WinRel

ALL : $(OUTDIR)/LISTCTRL.exe $(OUTDIR)/LISTCTRL.bsc

$(OUTDIR) : 
    if not exist $(OUTDIR)/nul mkdir $(OUTDIR)

# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32  
# ADD BASE CPP /nologo /MD /W3 /GX /YX /O2 /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /c
# ADD CPP /nologo /W3 /GX /YX /O2 /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "WIN32" /FR /c
CPP_PROJ=/nologo /W3 /GX /YX /O2 /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D\
 "WIN32" /FR$(INTDIR)/ /Fp$(OUTDIR)/"LISTCTRL.pch" /Fo$(INTDIR)/ /c 
CPP_OBJS=.\WinRel/
# ADD BASE RSC /l 0x1 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x1 /d "NDEBUG"
RSC_PROJ=/l 0x1 /fo$(INTDIR)/"LISTCTRL.res" /d "NDEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# SUBTRACT BASE BSC32 /Iu
# ADD BSC32 /nologo
# SUBTRACT BSC32 /Iu
BSC32_FLAGS=/nologo /o$(OUTDIR)/"LISTCTRL.bsc" 
BSC32_SBRS= \
	$(INTDIR)/WINMAIN.SBR \
	$(INTDIR)/LISTVIEW.SBR \
	$(INTDIR)/ABOUT.SBR \
	$(INTDIR)/MISC.SBR \
	$(INTDIR)/LISTCTRL.SBR \
	$(INTDIR)/INIT.SBR \
	$(INTDIR)/DISPATCH.SBR

$(OUTDIR)/LISTCTRL.bsc : $(OUTDIR)  $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib mfc30.lib mfco30.lib mfcuia32.lib mfcans32.lib ole32.lib oleaut32.lib uuid.lib /NOLOGO /SUBSYSTEM:windows /MACHINE:I386
# ADD LINK32 user32.lib gdi32.lib winspool.lib version.lib comctl32.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /NOLOGO /MACHINE:I386 /SUBSYSTEM:windows,4.0
LINK32_FLAGS=user32.lib gdi32.lib winspool.lib version.lib comctl32.lib\
 comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /NOLOGO\
 /INCREMENTAL:no /PDB:$(OUTDIR)/"LISTCTRL.pdb" /MACHINE:I386\
 /OUT:$(OUTDIR)/"LISTCTRL.exe" /SUBSYSTEM:windows,4.0  
DEF_FILE=
LINK32_OBJS= \
	$(INTDIR)/WINMAIN.OBJ \
	$(INTDIR)/LISTVIEW.OBJ \
	$(INTDIR)/ABOUT.OBJ \
	$(INTDIR)/MISC.OBJ \
	$(INTDIR)/LISTCTRL.OBJ \
	$(INTDIR)/INIT.OBJ \
	$(INTDIR)/LISTCTRL.res \
	$(INTDIR)/DISPATCH.OBJ

$(OUTDIR)/LISTCTRL.exe : $(OUTDIR)  $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "Win32 Debug"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "WinDebug"
# PROP BASE Intermediate_Dir "WinDebug"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "WinDebug"
# PROP Intermediate_Dir "WinDebug"
OUTDIR=.\WinDebug
INTDIR=.\WinDebug

ALL : $(OUTDIR)/LISTCTRL.exe $(OUTDIR)/LISTCTRL.bsc

$(OUTDIR) : 
    if not exist $(OUTDIR)/nul mkdir $(OUTDIR)

# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
# ADD BASE CPP /nologo /MD /W3 /GX /Zi /YX /Od /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /c
# ADD CPP /nologo /W3 /GX /Zi /YX /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /FR /c
CPP_PROJ=/nologo /W3 /GX /Zi /YX /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D\
 "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"LISTCTRL.pch" /Fo$(INTDIR)/\
 /Fd$(OUTDIR)/"LISTCTRL.pdb" /c 
CPP_OBJS=.\WinDebug/
# ADD BASE RSC /l 0x1 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x1 /d "_DEBUG"
RSC_PROJ=/l 0x1 /fo$(INTDIR)/"LISTCTRL.res" /d "_DEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# SUBTRACT BASE BSC32 /Iu
# ADD BSC32 /nologo
# SUBTRACT BSC32 /Iu
BSC32_FLAGS=/nologo /o$(OUTDIR)/"LISTCTRL.bsc" 
BSC32_SBRS= \
	$(INTDIR)/WINMAIN.SBR \
	$(INTDIR)/LISTVIEW.SBR \
	$(INTDIR)/ABOUT.SBR \
	$(INTDIR)/MISC.SBR \
	$(INTDIR)/LISTCTRL.SBR \
	$(INTDIR)/INIT.SBR \
	$(INTDIR)/DISPATCH.SBR

$(OUTDIR)/LISTCTRL.bsc : $(OUTDIR)  $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib mfc30d.lib mfco30d.lib mfcuia32.lib mfcans32.lib ole32.lib oleaut32.lib uuid.lib /NOLOGO /SUBSYSTEM:windows /DEBUG /MACHINE:I386
# ADD LINK32 version.lib comctl32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /NOLOGO /DEBUG /MACHINE:I386 /SUBSYSTEM:windows,4.0
LINK32_FLAGS=version.lib comctl32.lib user32.lib gdi32.lib winspool.lib\
 comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /NOLOGO\
 /INCREMENTAL:yes /PDB:$(OUTDIR)/"LISTCTRL.pdb" /DEBUG /MACHINE:I386\
 /OUT:$(OUTDIR)/"LISTCTRL.exe" /SUBSYSTEM:windows,4.0  
DEF_FILE=
LINK32_OBJS= \
	$(INTDIR)/WINMAIN.OBJ \
	$(INTDIR)/LISTVIEW.OBJ \
	$(INTDIR)/ABOUT.OBJ \
	$(INTDIR)/MISC.OBJ \
	$(INTDIR)/LISTCTRL.OBJ \
	$(INTDIR)/INIT.OBJ \
	$(INTDIR)/LISTCTRL.res \
	$(INTDIR)/DISPATCH.OBJ

$(OUTDIR)/LISTCTRL.exe : $(OUTDIR)  $(DEF_FILE) $(LINK32_OBJS)
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

SOURCE=.\WINMAIN.C
DEP_WINMA=\
	.\GLOBALS.H

$(INTDIR)/WINMAIN.OBJ :  $(SOURCE)  $(DEP_WINMA) $(INTDIR)

# End Source File
################################################################################
# Begin Source File

SOURCE=.\LISTVIEW.C
DEP_LISTV=\
	.\GLOBALS.H\
	.\LISTVIEW.H

$(INTDIR)/LISTVIEW.OBJ :  $(SOURCE)  $(DEP_LISTV) $(INTDIR)

# End Source File
################################################################################
# Begin Source File

SOURCE=.\ABOUT.C
DEP_ABOUT=\
	.\GLOBALS.H

$(INTDIR)/ABOUT.OBJ :  $(SOURCE)  $(DEP_ABOUT) $(INTDIR)

# End Source File
################################################################################
# Begin Source File

SOURCE=.\MISC.C
DEP_MISC_=\
	.\GLOBALS.H

$(INTDIR)/MISC.OBJ :  $(SOURCE)  $(DEP_MISC_) $(INTDIR)

# End Source File
################################################################################
# Begin Source File

SOURCE=.\LISTCTRL.C
DEP_LISTC=\
	.\GLOBALS.H\
	.\LISTVIEW.H

$(INTDIR)/LISTCTRL.OBJ :  $(SOURCE)  $(DEP_LISTC) $(INTDIR)

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

SOURCE=.\LISTCTRL.RC
DEP_LISTCT=\
	.\WINDOWS.ICO\
	.\DOLLAR.ICO\
	.\DISK.ICO\
	.\CHIP.ICO\
	.\FILM.ICO\
	.\CHKBOXS.BMP\
	.\GLOBALS.H\
	.\LISTVIEW.H

$(INTDIR)/LISTCTRL.res :  $(SOURCE)  $(DEP_LISTCT) $(INTDIR)
   $(RSC) $(RSC_PROJ)  $(SOURCE) 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\DISPATCH.C
DEP_DISPA=\
	.\GLOBALS.H

$(INTDIR)/DISPATCH.OBJ :  $(SOURCE)  $(DEP_DISPA) $(INTDIR)

# End Source File
# End Group
# End Project
################################################################################
