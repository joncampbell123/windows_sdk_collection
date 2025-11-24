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
!MESSAGE NMAKE /f "VIDCAP32.MAK" CFG="Win32 Debug"
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

ALL : $(OUTDIR)/VIDCAP32.exe $(OUTDIR)/VIDCAP32.bsc

$(OUTDIR) : 
    if not exist $(OUTDIR)/nul mkdir $(OUTDIR)

# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
# ADD BASE CPP /nologo /MD /W3 /GX /YX /O2 /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /c
# ADD CPP /nologo /W3 /GX /YX /O2 /I "c:\mm\internal\inc" /I "d:\sdk\inc32" /I "c:\msvc20\include" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "MTN" /D "_WIN32" /FR /c
# SUBTRACT CPP /X
CPP_PROJ=/nologo /W3 /GX /YX /O2 /I "c:\mm\internal\inc" /I "d:\sdk\inc32" /I\
 "c:\msvc20\include" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "MTN" /D "_WIN32"\
 /FR$(INTDIR)/ /Fp$(OUTDIR)/"VIDCAP32.pch" /Fo$(INTDIR)/ /c 
CPP_OBJS=.\WinRel/
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /i "..\..\verinfo" /d "NDEBUG" /d "MTN"
RSC_PROJ=/l 0x409 /fo$(INTDIR)/"VIDCAP.res" /i "..\..\verinfo" /d "NDEBUG" /d\
 "MTN" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o$(OUTDIR)/"VIDCAP32.bsc" 
BSC32_SBRS= \
	$(INTDIR)/VIDCAP.SBR \
	$(INTDIR)/DIALOGS.SBR \
	$(INTDIR)/ARROW.SBR \
	$(INTDIR)/PROFILE.SBR \
	$(INTDIR)/TOOLBAR.SBR \
	$(INTDIR)/VIDFRAME.SBR \
	$(INTDIR)/RLMETER.SBR \
	$(INTDIR)/STATUS.SBR \
	$(INTDIR)/HELP.SBR

$(OUTDIR)/VIDCAP32.bsc : $(OUTDIR)  $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 /NOLOGO /SUBSYSTEM:windows /MACHINE:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib vfw32.lib msacm32.lib winmm.lib /NOLOGO /SUBSYSTEM:windows /MACHINE:I386
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib vfw32.lib msacm32.lib\
 winmm.lib /NOLOGO /SUBSYSTEM:windows /INCREMENTAL:no\
 /PDB:$(OUTDIR)/"VIDCAP32.pdb" /MACHINE:I386 /OUT:$(OUTDIR)/"VIDCAP32.exe" 
DEF_FILE=
LINK32_OBJS= \
	$(INTDIR)/VIDCAP.OBJ \
	$(INTDIR)/DIALOGS.OBJ \
	$(INTDIR)/ARROW.OBJ \
	$(INTDIR)/PROFILE.OBJ \
	$(INTDIR)/TOOLBAR.OBJ \
	$(INTDIR)/VIDFRAME.OBJ \
	$(INTDIR)/RLMETER.OBJ \
	$(INTDIR)/VIDCAP.res \
	$(INTDIR)/STATUS.OBJ \
	$(INTDIR)/HELP.OBJ

$(OUTDIR)/VIDCAP32.exe : $(OUTDIR)  $(DEF_FILE) $(LINK32_OBJS)
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

ALL : $(OUTDIR)/VIDCAP32.exe $(OUTDIR)/VIDCAP32.bsc

$(OUTDIR) : 
    if not exist $(OUTDIR)/nul mkdir $(OUTDIR)

# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
# ADD BASE CPP /nologo /MD /W3 /GX /Zi /YX /Od /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /c
# ADD CPP /nologo /W3 /GX /Zi /YX /Od /I "c:\mm\internal\inc" /I "d:\sdk\inc32" /I "c:\msvc20\include" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "MTN" /D "_WIN32" /FR /c
# SUBTRACT CPP /X
CPP_PROJ=/nologo /W3 /GX /Zi /YX /Od /I "c:\mm\internal\inc" /I "d:\sdk\inc32"\
 /I "c:\msvc20\include" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "MTN" /D\
 "_WIN32" /FR$(INTDIR)/ /Fp$(OUTDIR)/"VIDCAP32.pch" /Fo$(INTDIR)/\
 /Fd$(OUTDIR)/"VIDCAP32.pdb" /c 
CPP_OBJS=.\WinDebug/
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /i "..\..\verinfo" /d "_DEBUG" /d "MTN"
RSC_PROJ=/l 0x409 /fo$(INTDIR)/"VIDCAP.res" /i "..\..\verinfo" /d "_DEBUG" /d\
 "MTN" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o$(OUTDIR)/"VIDCAP32.bsc" 
BSC32_SBRS= \
	$(INTDIR)/VIDCAP.SBR \
	$(INTDIR)/DIALOGS.SBR \
	$(INTDIR)/ARROW.SBR \
	$(INTDIR)/PROFILE.SBR \
	$(INTDIR)/TOOLBAR.SBR \
	$(INTDIR)/VIDFRAME.SBR \
	$(INTDIR)/RLMETER.SBR \
	$(INTDIR)/STATUS.SBR \
	$(INTDIR)/HELP.SBR

$(OUTDIR)/VIDCAP32.bsc : $(OUTDIR)  $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 /NOLOGO /SUBSYSTEM:windows /DEBUG /MACHINE:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib vfw32.lib msacm32.lib winmm.lib /NOLOGO /SUBSYSTEM:windows /DEBUG /MACHINE:I386
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib vfw32.lib msacm32.lib\
 winmm.lib /NOLOGO /SUBSYSTEM:windows /INCREMENTAL:yes\
 /PDB:$(OUTDIR)/"VIDCAP32.pdb" /DEBUG /MACHINE:I386\
 /OUT:$(OUTDIR)/"VIDCAP32.exe" 
DEF_FILE=
LINK32_OBJS= \
	$(INTDIR)/VIDCAP.OBJ \
	$(INTDIR)/DIALOGS.OBJ \
	$(INTDIR)/ARROW.OBJ \
	$(INTDIR)/PROFILE.OBJ \
	$(INTDIR)/TOOLBAR.OBJ \
	$(INTDIR)/VIDFRAME.OBJ \
	$(INTDIR)/RLMETER.OBJ \
	$(INTDIR)/VIDCAP.res \
	$(INTDIR)/STATUS.OBJ \
	$(INTDIR)/HELP.OBJ

$(OUTDIR)/VIDCAP32.exe : $(OUTDIR)  $(DEF_FILE) $(LINK32_OBJS)
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

SOURCE=.\VIDCAP.C
DEP_VIDCA=\
	D:\SDK\INC32\MSVIDEO.H\
	D:\SDK\INC32\AVICAP.H\
	.\VIDCAP.H\
	.\VIDFRAME.H\
	.\PROFILE.H\
	.\TOOLBAR.H\
	.\STATUS.H\
	.\ARROW.H\
	.\RLMETER.H\
	.\HELP.H\
	.\DIALOGS.H

$(INTDIR)/VIDCAP.OBJ :  $(SOURCE)  $(DEP_VIDCA) $(INTDIR)

# End Source File
################################################################################
# Begin Source File

SOURCE=.\DIALOGS.C
DEP_DIALO=\
	D:\SDK\INC32\MSVIDEO.H\
	.\ARROW.H\
	.\RLMETER.H\
	D:\SDK\INC32\AVICAP.H\
	.\VIDCAP.H\
	.\VIDFRAME.H\
	.\HELP.H\
	.\DIALOGS.H

$(INTDIR)/DIALOGS.OBJ :  $(SOURCE)  $(DEP_DIALO) $(INTDIR)

# End Source File
################################################################################
# Begin Source File

SOURCE=.\ARROW.C
DEP_ARROW=\
	.\ARROW.H

$(INTDIR)/ARROW.OBJ :  $(SOURCE)  $(DEP_ARROW) $(INTDIR)

# End Source File
################################################################################
# Begin Source File

SOURCE=.\PROFILE.C

$(INTDIR)/PROFILE.OBJ :  $(SOURCE)  $(INTDIR)

# End Source File
################################################################################
# Begin Source File

SOURCE=.\TOOLBAR.C
DEP_TOOLB=\
	.\TOOLBAR.H

$(INTDIR)/TOOLBAR.OBJ :  $(SOURCE)  $(DEP_TOOLB) $(INTDIR)

# End Source File
################################################################################
# Begin Source File

SOURCE=.\VIDFRAME.C
DEP_VIDFR=\
	D:\SDK\INC32\MSVIDEO.H\
	D:\SDK\INC32\AVICAP.H\
	.\VIDCAP.H\
	.\VIDFRAME.H\
	.\DIALOGS.H

$(INTDIR)/VIDFRAME.OBJ :  $(SOURCE)  $(DEP_VIDFR) $(INTDIR)

# End Source File
################################################################################
# Begin Source File

SOURCE=.\RLMETER.C
DEP_RLMET=\
	.\RLMETER.H

$(INTDIR)/RLMETER.OBJ :  $(SOURCE)  $(DEP_RLMET) $(INTDIR)

# End Source File
################################################################################
# Begin Source File

SOURCE=.\VIDCAP.RC
DEP_VIDCAP=\
	.\VIDCAP.ICO\
	.\BUTTONS.BMP\
	.\VIDCAP.H\
	.\vidcap.rcv\
	.\VIDCAP.DLG\
	.\DIALOGS.H\
	\MM\VERINFO\VERINFO.H\
	\MM\VERINFO\VERINFO.VER

$(INTDIR)/VIDCAP.res :  $(SOURCE)  $(DEP_VIDCAP) $(INTDIR)
   $(RSC) $(RSC_PROJ)  $(SOURCE) 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\STATUS.C
DEP_STATU=\
	.\STATUS.H

$(INTDIR)/STATUS.OBJ :  $(SOURCE)  $(DEP_STATU) $(INTDIR)

# End Source File
################################################################################
# Begin Source File

SOURCE=.\HELP.C
DEP_HELP_=\
	.\HELP.H

$(INTDIR)/HELP.OBJ :  $(SOURCE)  $(DEP_HELP_) $(INTDIR)

# End Source File
################################################################################
# Begin Source File

SOURCE=.\vidcap.rcv
# End Source File
# End Group
# End Project
################################################################################
