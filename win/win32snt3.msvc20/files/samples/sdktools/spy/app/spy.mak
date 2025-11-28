# Microsoft Visual C++ Generated NMAKE File, Format Version 2.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101
# TARGTYPE "Win32 (MIPS) Application" 0x0501

!IF "$(CFG)" == ""
CFG=Win32 (80x86) Debug
!MESSAGE No configuration specified.  Defaulting to Win32 (80x86) Debug.
!ENDIF 

!IF "$(CFG)" != "Win32 (80x86) Release" && "$(CFG)" != "Win32 (80x86) Debug" &&\
 "$(CFG)" != "Win32 (MIPS) Release" && "$(CFG)" != "Win32 (MIPS) Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "spy.mak" CFG="Win32 (80x86) Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Win32 (80x86) Release" (based on "Win32 (x86) Application")
!MESSAGE "Win32 (80x86) Debug" (based on "Win32 (x86) Application")
!MESSAGE "Win32 (MIPS) Release" (based on "Win32 (MIPS) Application")
!MESSAGE "Win32 (MIPS) Debug" (based on "Win32 (MIPS) Application")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

################################################################################
# Begin Project
# PROP Target_Last_Scanned "Win32 (MIPS) Release"

!IF  "$(CFG)" == "Win32 (80x86) Release"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "WinRel"
# PROP BASE Intermediate_Dir "WinRel"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\WinRel"
# PROP Intermediate_Dir "WinRel"
OUTDIR=..\WinRel
INTDIR=.\WinRel

ALL : $(OUTDIR)/spy.exe $(OUTDIR)/spy.bsc

$(OUTDIR) : 
    if not exist $(OUTDIR)/nul mkdir $(OUTDIR)

$(INTDIR) : 
    if not exist $(INTDIR)/nul mkdir $(INTDIR)

MTL=MkTypLib.exe
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
CPP=cl.exe
# ADD BASE CPP /nologo /MD /W3 /GX /YX"spy.h" /O2 /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /c
# ADD CPP /nologo /W3 /GX /YX"spy.h" /O2 /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "WIN32" /FR /c
CPP_PROJ=/nologo /W3 /GX /YX"spy.h" /O2 /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D\
 "WIN32" /FR$(INTDIR)/ /Fp$(OUTDIR)/"spy.pch" /Fo$(INTDIR)/ /c 
CPP_OBJS=.\WinRel/

.c{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

RSC=rc.exe
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
RSC_PROJ=/l 0x409 /fo$(INTDIR)/"spy.res" /d "NDEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# SUBTRACT BASE BSC32 /Iu
# ADD BSC32 /nologo
# SUBTRACT BSC32 /Iu
BSC32_FLAGS=/nologo /o$(OUTDIR)/"spy.bsc" 
BSC32_SBRS= \
	.\WinRel\hook.sbr \
	.\WinRel\wm.sbr \
	.\WinRel\wprintf.sbr \
	.\WinRel\dialogs.sbr \
	.\WinRel\misc.sbr \
	.\WinRel\spy.sbr

$(OUTDIR)/spy.bsc : $(OUTDIR)  $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib odbc32.lib mfc30.lib mfco30.lib mfcd30.lib ole32.lib oleaut32.lib uuid.lib /NOLOGO /SUBSYSTEM:windows /MACHINE:I386
# ADD LINK32 user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib /NOLOGO /SUBSYSTEM:windows /MACHINE:I386
LINK32_FLAGS=user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib\
 /NOLOGO /SUBSYSTEM:windows /INCREMENTAL:no /PDB:$(OUTDIR)/"spy.pdb"\
 /MACHINE:I386 /OUT:$(OUTDIR)/"spy.exe" 
DEF_FILE=
LINK32_OBJS= \
	.\WinRel\spy.res \
	.\WinRel\hook.obj \
	.\WinRel\wm.obj \
	.\WinRel\wprintf.obj \
	.\WinRel\dialogs.obj \
	.\WinRel\misc.obj \
	.\WinRel\spy.obj

$(OUTDIR)/spy.exe : $(OUTDIR)  $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "WinDebug"
# PROP BASE Intermediate_Dir "WinDebug"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\WinDebug"
# PROP Intermediate_Dir "WinDebug"
OUTDIR=..\WinDebug
INTDIR=.\WinDebug

ALL : $(OUTDIR)/spy.exe $(OUTDIR)/spy.bsc

$(OUTDIR) : 
    if not exist $(OUTDIR)/nul mkdir $(OUTDIR)

$(INTDIR) : 
    if not exist $(INTDIR)/nul mkdir $(INTDIR)

MTL=MkTypLib.exe
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
CPP=cl.exe
# ADD BASE CPP /nologo /MD /W3 /GX /Zi /YX"spy.h" /Od /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /c
# ADD CPP /nologo /W3 /GX /Zi /YX"spy.h" /Od /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "WIN32" /FR /c
CPP_PROJ=/nologo /W3 /GX /Zi /YX"spy.h" /Od /D "_DEBUG" /D "_WINDOWS" /D\
 "_MBCS" /D "WIN32" /FR$(INTDIR)/ /Fp$(OUTDIR)/"spy.pch" /Fo$(INTDIR)/\
 /Fd$(OUTDIR)/"spy.pdb" /c 
CPP_OBJS=.\WinDebug/

.c{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

RSC=rc.exe
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
RSC_PROJ=/l 0x409 /fo$(INTDIR)/"spy.res" /d "_DEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# SUBTRACT BASE BSC32 /Iu
# ADD BSC32 /nologo
# SUBTRACT BSC32 /Iu
BSC32_FLAGS=/nologo /o$(OUTDIR)/"spy.bsc" 
BSC32_SBRS= \
	.\WinDebug\hook.sbr \
	.\WinDebug\wm.sbr \
	.\WinDebug\wprintf.sbr \
	.\WinDebug\dialogs.sbr \
	.\WinDebug\misc.sbr \
	.\WinDebug\spy.sbr

$(OUTDIR)/spy.bsc : $(OUTDIR)  $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib odbc32.lib mfc30d.lib mfco30d.lib mfcd30d.lib ole32.lib oleaut32.lib uuid.lib /NOLOGO /SUBSYSTEM:windows /DEBUG /MACHINE:I386
# ADD LINK32 user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib /NOLOGO /SUBSYSTEM:windows /DEBUG /MACHINE:I386
LINK32_FLAGS=user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib\
 /NOLOGO /SUBSYSTEM:windows /INCREMENTAL:yes /PDB:$(OUTDIR)/"spy.pdb" /DEBUG\
 /MACHINE:I386 /OUT:$(OUTDIR)/"spy.exe" 
DEF_FILE=
LINK32_OBJS= \
	.\WinDebug\spy.res \
	.\WinDebug\hook.obj \
	.\WinDebug\wm.obj \
	.\WinDebug\wprintf.obj \
	.\WinDebug\dialogs.obj \
	.\WinDebug\misc.obj \
	.\WinDebug\spy.obj

$(OUTDIR)/spy.exe : $(OUTDIR)  $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Win32__M"
# PROP BASE Intermediate_Dir "Win32__M"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\WinRel"
# PROP Intermediate_Dir "WinRel"
OUTDIR=..\WinRel
INTDIR=.\WinRel

ALL : $(OUTDIR)/spy.exe $(OUTDIR)/spy.bsc

$(OUTDIR) : 
    if not exist $(OUTDIR)/nul mkdir $(OUTDIR)

$(INTDIR) : 
    if not exist $(INTDIR)/nul mkdir $(INTDIR)

MTL=MkTypLib.exe
# ADD BASE MTL /nologo /D "NDEBUG" /mips
# ADD MTL /nologo /D "NDEBUG" /mips
MTL_PROJ=/nologo /D "NDEBUG" /mips 
CPP=cl.exe
# ADD BASE CPP /nologo /MD /Gt0 /QMOb2000 /W3 /GX /YX"spy.h" /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /c
# ADD CPP /nologo /Gt0 /QMOb2000 /W3 /GX /YX"spy.h" /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /FR /c
CPP_PROJ=/nologo /Gt0 /QMOb2000 /W3 /GX /YX"spy.h" /O2 /D "WIN32" /D "NDEBUG"\
 /D "_WINDOWS" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"spy.pch" /Fo$(INTDIR)/ /c 
CPP_OBJS=.\WinRel/

.c{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

RSC=rc.exe
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG"
RSC_PROJ=/l 0x409 /fo$(INTDIR)/"spy.res" /d "NDEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# SUBTRACT BASE BSC32 /Iu
# ADD BSC32 /nologo
# SUBTRACT BSC32 /Iu
BSC32_FLAGS=/nologo /o$(OUTDIR)/"spy.bsc" 
BSC32_SBRS= \
	.\WinRel\hook.sbr \
	.\WinRel\wm.sbr \
	.\WinRel\wprintf.sbr \
	.\WinRel\dialogs.sbr \
	.\WinRel\misc.sbr \
	.\WinRel\spy.sbr

$(OUTDIR)/spy.bsc : $(OUTDIR)  $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 /NOLOGO /SUBSYSTEM:windows /MACHINE:MIPS
# ADD LINK32 user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib /NOLOGO /SUBSYSTEM:windows /MACHINE:MIPS
LINK32_FLAGS=user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib\
 /NOLOGO /SUBSYSTEM:windows /PDB:$(OUTDIR)/"spy.pdb" /MACHINE:MIPS\
 /OUT:$(OUTDIR)/"spy.exe" 
DEF_FILE=
LINK32_OBJS= \
	.\WinRel\spy.res \
	.\WinRel\hook.obj \
	.\WinRel\wm.obj \
	.\WinRel\wprintf.obj \
	.\WinRel\dialogs.obj \
	.\WinRel\misc.obj \
	.\WinRel\spy.obj

$(OUTDIR)/spy.exe : $(OUTDIR)  $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Win32__0"
# PROP BASE Intermediate_Dir "Win32__0"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\WinDebug"
# PROP Intermediate_Dir "WinDebug"
OUTDIR=..\WinDebug
INTDIR=.\WinDebug

ALL : $(OUTDIR)/spy.exe $(OUTDIR)/spy.bsc

$(OUTDIR) : 
    if not exist $(OUTDIR)/nul mkdir $(OUTDIR)

$(INTDIR) : 
    if not exist $(INTDIR)/nul mkdir $(INTDIR)

MTL=MkTypLib.exe
# ADD BASE MTL /nologo /D "_DEBUG" /mips
# ADD MTL /nologo /D "_DEBUG" /mips
MTL_PROJ=/nologo /D "_DEBUG" /mips 
CPP=cl.exe
# ADD BASE CPP /nologo /MD /Gt0 /QMOb2000 /W3 /GX /Zi /YX"spy.h" /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /c
# ADD CPP /nologo /Gt0 /QMOb2000 /W3 /GX /Zi /YX"spy.h" /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /FR /c
CPP_PROJ=/nologo /Gt0 /QMOb2000 /W3 /GX /Zi /YX"spy.h" /Od /D "WIN32" /D\
 "_DEBUG" /D "_WINDOWS" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"spy.pch"\
 /Fo$(INTDIR)/ /Fd$(OUTDIR)/"spy.pdb" /c 
CPP_OBJS=.\WinDebug/

.c{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

RSC=rc.exe
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG"
RSC_PROJ=/l 0x409 /fo$(INTDIR)/"spy.res" /d "_DEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# SUBTRACT BASE BSC32 /Iu
# ADD BSC32 /nologo
# SUBTRACT BSC32 /Iu
BSC32_FLAGS=/nologo /o$(OUTDIR)/"spy.bsc" 
BSC32_SBRS= \
	.\WinDebug\hook.sbr \
	.\WinDebug\wm.sbr \
	.\WinDebug\wprintf.sbr \
	.\WinDebug\dialogs.sbr \
	.\WinDebug\misc.sbr \
	.\WinDebug\spy.sbr

$(OUTDIR)/spy.bsc : $(OUTDIR)  $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 /NOLOGO /SUBSYSTEM:windows /DEBUG /MACHINE:MIPS
# ADD LINK32 user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib /NOLOGO /SUBSYSTEM:windows /DEBUG /MACHINE:MIPS
LINK32_FLAGS=user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib\
 /NOLOGO /SUBSYSTEM:windows /PDB:$(OUTDIR)/"spy.pdb" /DEBUG /MACHINE:MIPS\
 /OUT:$(OUTDIR)/"spy.exe" 
DEF_FILE=
LINK32_OBJS= \
	.\WinDebug\spy.res \
	.\WinDebug\hook.obj \
	.\WinDebug\wm.obj \
	.\WinDebug\wprintf.obj \
	.\WinDebug\dialogs.obj \
	.\WinDebug\misc.obj \
	.\WinDebug\spy.obj

$(OUTDIR)/spy.exe : $(OUTDIR)  $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 

################################################################################
# Begin Group "Source Files"

################################################################################
# Begin Source File

SOURCE=.\spy.rc
DEP_SPY_R=\
	.\spy.ico\
	.\spy.h\
	.\dialogs.h\
	..\hook.h\
	.\spyfuncs.h\
	.\wprintf.h

!IF  "$(CFG)" == "Win32 (80x86) Release"

.\WinRel\spy.res :  $(SOURCE)  $(DEP_SPY_R) $(INTDIR)
   $(RSC) $(RSC_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

.\WinDebug\spy.res :  $(SOURCE)  $(DEP_SPY_R) $(INTDIR)
   $(RSC) $(RSC_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

.\WinRel\spy.res :  $(SOURCE)  $(DEP_SPY_R) $(INTDIR)
   $(RSC) $(RSC_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

.\WinDebug\spy.res :  $(SOURCE)  $(DEP_SPY_R) $(INTDIR)
   $(RSC) $(RSC_PROJ)  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\hook.c
DEP_HOOK_=\
	.\spy.h\
	.\dialogs.h\
	..\hook.h\
	.\spyfuncs.h\
	.\wprintf.h

!IF  "$(CFG)" == "Win32 (80x86) Release"

.\WinRel\hook.obj :  $(SOURCE)  $(DEP_HOOK_) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

.\WinDebug\hook.obj :  $(SOURCE)  $(DEP_HOOK_) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

.\WinRel\hook.obj :  $(SOURCE)  $(DEP_HOOK_) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

.\WinDebug\hook.obj :  $(SOURCE)  $(DEP_HOOK_) $(INTDIR)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\wm.c
DEP_WM_C4=\
	.\spy.h\
	.\dialogs.h\
	..\hook.h\
	.\spyfuncs.h\
	.\wprintf.h

!IF  "$(CFG)" == "Win32 (80x86) Release"

.\WinRel\wm.obj :  $(SOURCE)  $(DEP_WM_C4) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

.\WinDebug\wm.obj :  $(SOURCE)  $(DEP_WM_C4) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

.\WinRel\wm.obj :  $(SOURCE)  $(DEP_WM_C4) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

.\WinDebug\wm.obj :  $(SOURCE)  $(DEP_WM_C4) $(INTDIR)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\wprintf.c
DEP_WPRIN=\
	.\spy.h\
	.\dialogs.h\
	..\hook.h\
	.\spyfuncs.h\
	.\wprintf.h

!IF  "$(CFG)" == "Win32 (80x86) Release"

.\WinRel\wprintf.obj :  $(SOURCE)  $(DEP_WPRIN) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

.\WinDebug\wprintf.obj :  $(SOURCE)  $(DEP_WPRIN) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

.\WinRel\wprintf.obj :  $(SOURCE)  $(DEP_WPRIN) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

.\WinDebug\wprintf.obj :  $(SOURCE)  $(DEP_WPRIN) $(INTDIR)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\dialogs.c
DEP_DIALO=\
	.\spy.h\
	.\dialogs.h\
	..\hook.h\
	.\spyfuncs.h\
	.\wprintf.h

!IF  "$(CFG)" == "Win32 (80x86) Release"

.\WinRel\dialogs.obj :  $(SOURCE)  $(DEP_DIALO) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

.\WinDebug\dialogs.obj :  $(SOURCE)  $(DEP_DIALO) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

.\WinRel\dialogs.obj :  $(SOURCE)  $(DEP_DIALO) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

.\WinDebug\dialogs.obj :  $(SOURCE)  $(DEP_DIALO) $(INTDIR)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\misc.c
DEP_MISC_=\
	.\spy.h\
	.\dialogs.h\
	..\hook.h\
	.\spyfuncs.h\
	.\wprintf.h

!IF  "$(CFG)" == "Win32 (80x86) Release"

.\WinRel\misc.obj :  $(SOURCE)  $(DEP_MISC_) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

.\WinDebug\misc.obj :  $(SOURCE)  $(DEP_MISC_) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

.\WinRel\misc.obj :  $(SOURCE)  $(DEP_MISC_) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

.\WinDebug\misc.obj :  $(SOURCE)  $(DEP_MISC_) $(INTDIR)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\spy.c
DEP_SPY_C=\
	.\spy.h\
	.\dialogs.h\
	..\hook.h\
	.\spyfuncs.h\
	.\wprintf.h

!IF  "$(CFG)" == "Win32 (80x86) Release"

.\WinRel\spy.obj :  $(SOURCE)  $(DEP_SPY_C) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

.\WinDebug\spy.obj :  $(SOURCE)  $(DEP_SPY_C) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

.\WinRel\spy.obj :  $(SOURCE)  $(DEP_SPY_C) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

.\WinDebug\spy.obj :  $(SOURCE)  $(DEP_SPY_C) $(INTDIR)

!ENDIF 

# End Source File
# End Group
# End Project
################################################################################
