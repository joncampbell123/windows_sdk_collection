# Microsoft Visual C++ Generated NMAKE File, Format Version 2.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101
# TARGTYPE "Win32 (MIPS) Application" 0x0501

!IF "$(CFG)" == ""
CFG=Win32 (80x86) Debug
!MESSAGE No configuration specified.  Defaulting to Win32 (80x86) Debug.
!ENDIF 

!IF "$(CFG)" != "Win32 (80x86) Release" && "$(CFG)" != "Win32 (80x86) Debug" &&\
 "$(CFG)" != "Win32 (80x86) Unicode Debug" && "$(CFG)" !=\
 "Win32 (80x86) Unicode Release" && "$(CFG)" != "Win32 (MIPS) Release" &&\
 "$(CFG)" != "Win32 (MIPS) Debug" && "$(CFG)" != "Win32 (MIPS) Unicode Release"\
 && "$(CFG)" != "Win32 (MIPS) Unicode Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "mtmdi.mak" CFG="Win32 (80x86) Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Win32 (80x86) Release" (based on "Win32 (x86) Application")
!MESSAGE "Win32 (80x86) Debug" (based on "Win32 (x86) Application")
!MESSAGE "Win32 (80x86) Unicode Debug" (based on "Win32 (x86) Application")
!MESSAGE "Win32 (80x86) Unicode Release" (based on "Win32 (x86) Application")
!MESSAGE "Win32 (MIPS) Release" (based on "Win32 (MIPS) Application")
!MESSAGE "Win32 (MIPS) Debug" (based on "Win32 (MIPS) Application")
!MESSAGE "Win32 (MIPS) Unicode Release" (based on "Win32 (MIPS) Application")
!MESSAGE "Win32 (MIPS) Unicode Debug" (based on "Win32 (MIPS) Application")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

################################################################################
# Begin Project
# PROP Target_Last_Scanned "Win32 (MIPS) Release"

!IF  "$(CFG)" == "Win32 (80x86) Release"

# PROP BASE Use_MFC 1
# PROP BASE Use_Debug_Libraries 0
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "WinRel"
# PROP Intermediate_Dir "WinRel"
OUTDIR=.\WinRel
INTDIR=.\WinRel

ALL : .\WinRel\mtmdi.exe .\WinRel\mtmdi.bsc

$(OUTDIR) : 
    if not exist $(OUTDIR)/nul mkdir $(OUTDIR)

MTL=MkTypLib.exe
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
CPP=cl.exe
# ADD BASE CPP /nologo /MT /W3 /GX /YX /Ox /D "NDEBUG" /D "_WINDOWS" /D "_X86_" /D "_MBCS" /FR /c
# ADD CPP /nologo /MD /W3 /GX /Ox /D "NDEBUG" /D "_WINDOWS" /D "_X86_" /D "_AFXDLL" /D "_MBCS" /FR /Yu"stdafx.h" /c
CPP_PROJ=/nologo /MD /W3 /GX /Ox /D "NDEBUG" /D "_WINDOWS" /D "_X86_" /D\
 "_AFXDLL" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"mtmdi.pch" /Yu"stdafx.h"\
 /Fo$(INTDIR)/ /c 
CPP_OBJS=.\WinRel/

.c{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

RSC=rc.exe
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /fo$(INTDIR)/"mtmdi.res" /d "NDEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# SUBTRACT BASE BSC32 /Iu
# ADD BSC32 /nologo
# SUBTRACT BSC32 /Iu
BSC32_FLAGS=/nologo /o$(OUTDIR)/"mtmdi.bsc" 
BSC32_SBRS= \
	.\WinRel\hello.sbr \
	.\WinRel\mainfrm.sbr \
	.\WinRel\mdi.sbr \
	.\WinRel\bounce.sbr \
	.\WinRel\stdafx.sbr \
	.\WinRel\mtbounce.sbr

.\WinRel\mtmdi.bsc : $(OUTDIR)  $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib olecli32.lib olesvr32.lib nafxcw.lib mfcuia32.lib mfcans32.lib ole32.lib oleaut32.lib uuid.lib /NOLOGO /SUBSYSTEM:windows /MACHINE:IX86
# ADD LINK32 /NOLOGO /SUBSYSTEM:windows /MACHINE:IX86
LINK32_FLAGS=/NOLOGO /SUBSYSTEM:windows /INCREMENTAL:no\
 /PDB:$(OUTDIR)/"mtmdi.pdb" /MACHINE:IX86 /OUT:$(OUTDIR)/"mtmdi.exe" 
DEF_FILE=
LINK32_OBJS= \
	.\WinRel\hello.obj \
	.\WinRel\mainfrm.obj \
	.\WinRel\mdi.obj \
	.\WinRel\bounce.obj \
	.\WinRel\stdafx.obj \
	.\WinRel\mtmdi.res \
	.\WinRel\mtbounce.obj

.\WinRel\mtmdi.exe : $(OUTDIR)  $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

# PROP BASE Use_MFC 1
# PROP BASE Use_Debug_Libraries 1
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "WinDebug"
# PROP Intermediate_Dir "WinDebug"
OUTDIR=.\WinDebug
INTDIR=.\WinDebug

ALL : .\WinDebug\mtmdi.exe .\WinDebug\mtmdi.bsc

$(OUTDIR) : 
    if not exist $(OUTDIR)/nul mkdir $(OUTDIR)

MTL=MkTypLib.exe
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
CPP=cl.exe
# ADD BASE CPP /nologo /MT /W3 /GX /Zi /YX /Od /D "_DEBUG" /D "_WINDOWS" /D "_X86_" /D "_MBCS" /FR /c
# ADD CPP /nologo /MD /W3 /GX /Zi /Od /D "_DEBUG" /D "_WINDOWS" /D "_X86_" /D "_AFXDLL" /D "_MBCS" /FR /Yu"stdafx.h" /c
CPP_PROJ=/nologo /MD /W3 /GX /Zi /Od /D "_DEBUG" /D "_WINDOWS" /D "_X86_" /D\
 "_AFXDLL" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"mtmdi.pch" /Yu"stdafx.h"\
 /Fo$(INTDIR)/ /Fd$(OUTDIR)/"mtmdi.pdb" /c 
CPP_OBJS=.\WinDebug/

.c{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

RSC=rc.exe
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /fo$(INTDIR)/"mtmdi.res" /d "_DEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# SUBTRACT BASE BSC32 /Iu
# ADD BSC32 /nologo
# SUBTRACT BSC32 /Iu
BSC32_FLAGS=/nologo /o$(OUTDIR)/"mtmdi.bsc" 
BSC32_SBRS= \
	.\WinDebug\hello.sbr \
	.\WinDebug\mainfrm.sbr \
	.\WinDebug\mdi.sbr \
	.\WinDebug\bounce.sbr \
	.\WinDebug\stdafx.sbr \
	.\WinDebug\mtbounce.sbr

.\WinDebug\mtmdi.bsc : $(OUTDIR)  $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib olecli32.lib olesvr32.lib nafxcwd.lib mfcuia32.lib mfcans32.lib ole32.lib oleaut32.lib uuid.lib /NOLOGO /SUBSYSTEM:windows /DEBUG /MACHINE:IX86
# ADD LINK32 /NOLOGO /SUBSYSTEM:windows /DEBUG /MACHINE:IX86
LINK32_FLAGS=/NOLOGO /SUBSYSTEM:windows /INCREMENTAL:yes\
 /PDB:$(OUTDIR)/"mtmdi.pdb" /DEBUG /MACHINE:IX86 /OUT:$(OUTDIR)/"mtmdi.exe" 
DEF_FILE=
LINK32_OBJS= \
	.\WinDebug\hello.obj \
	.\WinDebug\mainfrm.obj \
	.\WinDebug\mdi.obj \
	.\WinDebug\bounce.obj \
	.\WinDebug\stdafx.obj \
	.\WinDebug\mtmdi.res \
	.\WinDebug\mtbounce.obj

.\WinDebug\mtmdi.exe : $(OUTDIR)  $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "Win32 (80x86) Unicode Debug"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Win32__8"
# PROP BASE Intermediate_Dir "Win32__8"
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "UniDebug"
# PROP Intermediate_Dir "UniDebug"
OUTDIR=.\UniDebug
INTDIR=.\UniDebug

ALL : .\UniDebug\mtmdi.exe .\UniDebug\mtmdi.bsc

$(OUTDIR) : 
    if not exist $(OUTDIR)/nul mkdir $(OUTDIR)

MTL=MkTypLib.exe
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
CPP=cl.exe
# ADD BASE CPP /nologo /MD /W3 /GX /Zi /YX /Od /D "_DEBUG" /D "_WINDOWS" /D "_X86_" /D "_AFXDLL" /D "_MBCS" /FR /c
# ADD CPP /nologo /MD /W3 /GX /Zi /Od /D "_DEBUG" /D "_WINDOWS" /D "_X86_" /D "_AFXDLL" /D "_MBCS" /D "_UNICODE" /FR /Yu"stdafx.h" /c
CPP_PROJ=/nologo /MD /W3 /GX /Zi /Od /D "_DEBUG" /D "_WINDOWS" /D "_X86_" /D\
 "_AFXDLL" /D "_MBCS" /D "_UNICODE" /FR$(INTDIR)/ /Fp$(OUTDIR)/"mtmdi.pch"\
 /Yu"stdafx.h" /Fo$(INTDIR)/ /Fd$(OUTDIR)/"mtmdi.pdb" /c 
CPP_OBJS=.\UniDebug/

.c{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

RSC=rc.exe
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /fo$(INTDIR)/"mtmdi.res" /d "_DEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# SUBTRACT BASE BSC32 /Iu
# ADD BSC32 /nologo
# SUBTRACT BSC32 /Iu
BSC32_FLAGS=/nologo /o$(OUTDIR)/"mtmdi.bsc" 
BSC32_SBRS= \
	.\UniDebug\hello.sbr \
	.\UniDebug\mainfrm.sbr \
	.\UniDebug\mdi.sbr \
	.\UniDebug\bounce.sbr \
	.\UniDebug\stdafx.sbr \
	.\UniDebug\mtbounce.sbr

.\UniDebug\mtmdi.bsc : $(OUTDIR)  $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 /NOLOGO /SUBSYSTEM:windows /DEBUG /MACHINE:IX86
# ADD LINK32 /NOLOGO /ENTRY:"wWinMainCRTStartup" /SUBSYSTEM:windows /DEBUG /MACHINE:IX86
LINK32_FLAGS=/NOLOGO /ENTRY:"wWinMainCRTStartup" /SUBSYSTEM:windows\
 /INCREMENTAL:yes /PDB:$(OUTDIR)/"mtmdi.pdb" /DEBUG /MACHINE:IX86\
 /OUT:$(OUTDIR)/"mtmdi.exe" 
DEF_FILE=
LINK32_OBJS= \
	.\UniDebug\hello.obj \
	.\UniDebug\mainfrm.obj \
	.\UniDebug\mdi.obj \
	.\UniDebug\bounce.obj \
	.\UniDebug\stdafx.obj \
	.\UniDebug\mtmdi.res \
	.\UniDebug\mtbounce.obj

.\UniDebug\mtmdi.exe : $(OUTDIR)  $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "Win32 (80x86) Unicode Release"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Win32__0"
# PROP BASE Intermediate_Dir "Win32__0"
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "UniRel"
# PROP Intermediate_Dir "UniRel"
OUTDIR=.\UniRel
INTDIR=.\UniRel

ALL : .\UniRel\mtmdi.exe .\UniRel\mtmdi.bsc

$(OUTDIR) : 
    if not exist $(OUTDIR)/nul mkdir $(OUTDIR)

MTL=MkTypLib.exe
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
CPP=cl.exe
# ADD BASE CPP /nologo /MD /W3 /GX /YX /Ox /D "NDEBUG" /D "_WINDOWS" /D "_X86_" /D "_AFXDLL" /D "_MBCS" /FR /c
# ADD CPP /nologo /MD /W3 /GX /Ox /D "NDEBUG" /D "_WINDOWS" /D "_X86_" /D "_AFXDLL" /D "_MBCS" /D "_UNICODE" /FR /Yu"stdafx.h" /c
CPP_PROJ=/nologo /MD /W3 /GX /Ox /D "NDEBUG" /D "_WINDOWS" /D "_X86_" /D\
 "_AFXDLL" /D "_MBCS" /D "_UNICODE" /FR$(INTDIR)/ /Fp$(OUTDIR)/"mtmdi.pch"\
 /Yu"stdafx.h" /Fo$(INTDIR)/ /c 
CPP_OBJS=.\UniRel/

.c{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

RSC=rc.exe
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /fo$(INTDIR)/"mtmdi.res" /d "NDEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# SUBTRACT BASE BSC32 /Iu
# ADD BSC32 /nologo
# SUBTRACT BSC32 /Iu
BSC32_FLAGS=/nologo /o$(OUTDIR)/"mtmdi.bsc" 
BSC32_SBRS= \
	.\UniRel\hello.sbr \
	.\UniRel\mainfrm.sbr \
	.\UniRel\mdi.sbr \
	.\UniRel\bounce.sbr \
	.\UniRel\stdafx.sbr \
	.\UniRel\mtbounce.sbr

.\UniRel\mtmdi.bsc : $(OUTDIR)  $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 /NOLOGO /SUBSYSTEM:windows /MACHINE:IX86
# ADD LINK32 /NOLOGO /ENTRY:"wWinMainCRTStartup" /SUBSYSTEM:windows /MACHINE:IX86
LINK32_FLAGS=/NOLOGO /ENTRY:"wWinMainCRTStartup" /SUBSYSTEM:windows\
 /INCREMENTAL:no /PDB:$(OUTDIR)/"mtmdi.pdb" /MACHINE:IX86\
 /OUT:$(OUTDIR)/"mtmdi.exe" 
DEF_FILE=
LINK32_OBJS= \
	.\UniRel\hello.obj \
	.\UniRel\mainfrm.obj \
	.\UniRel\mdi.obj \
	.\UniRel\bounce.obj \
	.\UniRel\stdafx.obj \
	.\UniRel\mtmdi.res \
	.\UniRel\mtbounce.obj

.\UniRel\mtmdi.exe : $(OUTDIR)  $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Win32__M"
# PROP BASE Intermediate_Dir "Win32__M"
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "WinRel"
# PROP Intermediate_Dir "WinRel"
OUTDIR=.\WinRel
INTDIR=.\WinRel

ALL : .\WinRel\mtmdi.exe .\WinRel\mtmdi.bsc

$(OUTDIR) : 
    if not exist $(OUTDIR)/nul mkdir $(OUTDIR)

MTL=MkTypLib.exe
# ADD BASE MTL /nologo /D "NDEBUG" /mips
# ADD MTL /nologo /D "NDEBUG" /mips
MTL_PROJ=/nologo /D "NDEBUG" /mips 
CPP=cl.exe
# ADD BASE CPP /nologo /MD /Gt0 /QMOb2000 /W3 /GX /YX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /c
# ADD CPP /nologo /MD /Gt0 /QMOb2000 /W3 /GX /O2 /D "NDEBUG" /D "_WINDOWS" /D "_MIPS_" /D "_AFXDLL" /D "_MBCS" /FR /Yu"stdafx.h" /c
CPP_PROJ=/nologo /MD /Gt0 /QMOb2000 /W3 /GX /O2 /D "NDEBUG" /D "_WINDOWS" /D\
 "_MIPS_" /D "_AFXDLL" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"mtmdi.pch"\
 /Yu"stdafx.h" /Fo$(INTDIR)/ /c 
CPP_OBJS=.\WinRel/

.c{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

RSC=rc.exe
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /fo$(INTDIR)/"mtmdi.res" /d "NDEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o$(OUTDIR)/"mtmdi.bsc" 
BSC32_SBRS= \
	.\WinRel\hello.sbr \
	.\WinRel\mainfrm.sbr \
	.\WinRel\mdi.sbr \
	.\WinRel\bounce.sbr \
	.\WinRel\stdafx.sbr \
	.\WinRel\mtbounce.sbr

.\WinRel\mtmdi.bsc : $(OUTDIR)  $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 /NOLOGO /SUBSYSTEM:windows /MACHINE:MIPS
# ADD LINK32 /NOLOGO /SUBSYSTEM:windows /MACHINE:MIPS
LINK32_FLAGS=/NOLOGO /SUBSYSTEM:windows /PDB:$(OUTDIR)/"mtmdi.pdb"\
 /MACHINE:MIPS /OUT:$(OUTDIR)/"mtmdi.exe" 
DEF_FILE=
LINK32_OBJS= \
	.\WinRel\hello.obj \
	.\WinRel\mainfrm.obj \
	.\WinRel\mdi.obj \
	.\WinRel\bounce.obj \
	.\WinRel\stdafx.obj \
	.\WinRel\mtmdi.res \
	.\WinRel\mtbounce.obj

.\WinRel\mtmdi.exe : $(OUTDIR)  $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Win32__0"
# PROP BASE Intermediate_Dir "Win32__0"
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "WinDebug"
# PROP Intermediate_Dir "WinDebug"
OUTDIR=.\WinDebug
INTDIR=.\WinDebug

ALL : .\WinDebug\mtmdi.exe .\WinDebug\mtmdi.bsc

$(OUTDIR) : 
    if not exist $(OUTDIR)/nul mkdir $(OUTDIR)

MTL=MkTypLib.exe
# ADD BASE MTL /nologo /D "_DEBUG" /mips
# ADD MTL /nologo /D "_DEBUG" /mips
MTL_PROJ=/nologo /D "_DEBUG" /mips 
CPP=cl.exe
# ADD BASE CPP /nologo /MD /Gt0 /QMOb2000 /W3 /GX /Zi /YX /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /c
# ADD CPP /nologo /MD /Gt0 /QMOb2000 /W3 /GX /Zi /Od /D "_DEBUG" /D "_WINDOWS" /D "_MIPS_" /D "_AFXDLL" /D "_MBCS" /FR /Yu"stdafx.h" /c
CPP_PROJ=/nologo /MD /Gt0 /QMOb2000 /W3 /GX /Zi /Od /D "_DEBUG" /D "_WINDOWS"\
 /D "_MIPS_" /D "_AFXDLL" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"mtmdi.pch"\
 /Yu"stdafx.h" /Fo$(INTDIR)/ /Fd$(OUTDIR)/"mtmdi.pdb" /c 
CPP_OBJS=.\WinDebug/

.c{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

RSC=rc.exe
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /fo$(INTDIR)/"mtmdi.res" /d "_DEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o$(OUTDIR)/"mtmdi.bsc" 
BSC32_SBRS= \
	.\WinDebug\hello.sbr \
	.\WinDebug\mainfrm.sbr \
	.\WinDebug\mdi.sbr \
	.\WinDebug\bounce.sbr \
	.\WinDebug\stdafx.sbr \
	.\WinDebug\mtbounce.sbr

.\WinDebug\mtmdi.bsc : $(OUTDIR)  $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 /NOLOGO /SUBSYSTEM:windows /DEBUG /MACHINE:MIPS
# ADD LINK32 /NOLOGO /SUBSYSTEM:windows /DEBUG /MACHINE:MIPS
LINK32_FLAGS=/NOLOGO /SUBSYSTEM:windows /PDB:$(OUTDIR)/"mtmdi.pdb" /DEBUG\
 /MACHINE:MIPS /OUT:$(OUTDIR)/"mtmdi.exe" 
DEF_FILE=
LINK32_OBJS= \
	.\WinDebug\hello.obj \
	.\WinDebug\mainfrm.obj \
	.\WinDebug\mdi.obj \
	.\WinDebug\bounce.obj \
	.\WinDebug\stdafx.obj \
	.\WinDebug\mtmdi.res \
	.\WinDebug\mtbounce.obj

.\WinDebug\mtmdi.exe : $(OUTDIR)  $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Unicode Release"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Win32__1"
# PROP BASE Intermediate_Dir "Win32__1"
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "UniRel"
# PROP Intermediate_Dir "UniRel"
OUTDIR=.\UniRel
INTDIR=.\UniRel

ALL : .\UniRel\mtmdi.exe .\UniRel\mtmdi.bsc

$(OUTDIR) : 
    if not exist $(OUTDIR)/nul mkdir $(OUTDIR)

MTL=MkTypLib.exe
# ADD BASE MTL /nologo /D "NDEBUG" /mips
# ADD MTL /nologo /D "NDEBUG" /mips
MTL_PROJ=/nologo /D "NDEBUG" /mips 
CPP=cl.exe
# ADD BASE CPP /nologo /MD /Gt0 /QMOb2000 /W3 /GX /YX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /c
# ADD CPP /nologo /MD /Gt0 /QMOb2000 /W3 /GX /O2 /D "NDEBUG" /D "_WINDOWS" /D "_MIPS_" /D "_AFXDLL" /D "_MBCS" /D "_UNICODE" /FR /Yu"stdafx.h" /c
CPP_PROJ=/nologo /MD /Gt0 /QMOb2000 /W3 /GX /O2 /D "NDEBUG" /D "_WINDOWS" /D\
 "_MIPS_" /D "_AFXDLL" /D "_MBCS" /D "_UNICODE" /FR$(INTDIR)/\
 /Fp$(OUTDIR)/"mtmdi.pch" /Yu"stdafx.h" /Fo$(INTDIR)/ /c 
CPP_OBJS=.\UniRel/

.c{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

RSC=rc.exe
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /fo$(INTDIR)/"mtmdi.res" /d "NDEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o$(OUTDIR)/"mtmdi.bsc" 
BSC32_SBRS= \
	.\UniRel\hello.sbr \
	.\UniRel\mainfrm.sbr \
	.\UniRel\mdi.sbr \
	.\UniRel\bounce.sbr \
	.\UniRel\stdafx.sbr \
	.\UniRel\mtbounce.sbr

.\UniRel\mtmdi.bsc : $(OUTDIR)  $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 /NOLOGO /SUBSYSTEM:windows /MACHINE:MIPS
# ADD LINK32 /NOLOGO /ENTRY:"wWinMainCRTStartup" /SUBSYSTEM:windows /MACHINE:MIPS
LINK32_FLAGS=/NOLOGO /ENTRY:"wWinMainCRTStartup" /SUBSYSTEM:windows\
 /PDB:$(OUTDIR)/"mtmdi.pdb" /MACHINE:MIPS /OUT:$(OUTDIR)/"mtmdi.exe" 
DEF_FILE=
LINK32_OBJS= \
	.\UniRel\hello.obj \
	.\UniRel\mainfrm.obj \
	.\UniRel\mdi.obj \
	.\UniRel\bounce.obj \
	.\UniRel\stdafx.obj \
	.\UniRel\mtmdi.res \
	.\UniRel\mtbounce.obj

.\UniRel\mtmdi.exe : $(OUTDIR)  $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Unicode Debug"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Win32__2"
# PROP BASE Intermediate_Dir "Win32__2"
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "UniDebug"
# PROP Intermediate_Dir "UniDebug"
OUTDIR=.\UniDebug
INTDIR=.\UniDebug

ALL : .\UniDebug\mtmdi.exe .\UniDebug\mtmdi.bsc

$(OUTDIR) : 
    if not exist $(OUTDIR)/nul mkdir $(OUTDIR)

MTL=MkTypLib.exe
# ADD BASE MTL /nologo /D "_DEBUG" /mips
# ADD MTL /nologo /D "_DEBUG" /mips
MTL_PROJ=/nologo /D "_DEBUG" /mips 
CPP=cl.exe
# ADD BASE CPP /nologo /MD /Gt0 /QMOb2000 /W3 /GX /Zi /YX /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /c
# ADD CPP /nologo /MD /Gt0 /QMOb2000 /W3 /GX /Zi /Od /D "_DEBUG" /D "_WINDOWS" /D "_MIPS_" /D "_AFXDLL" /D "_MBCS" /D "_UNICODE" /FR /Yu"stdafx.h" /c
CPP_PROJ=/nologo /MD /Gt0 /QMOb2000 /W3 /GX /Zi /Od /D "_DEBUG" /D "_WINDOWS"\
 /D "_MIPS_" /D "_AFXDLL" /D "_MBCS" /D "_UNICODE" /FR$(INTDIR)/\
 /Fp$(OUTDIR)/"mtmdi.pch" /Yu"stdafx.h" /Fo$(INTDIR)/ /Fd$(OUTDIR)/"mtmdi.pdb"\
 /c 
CPP_OBJS=.\UniDebug/

.c{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

RSC=rc.exe
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /fo$(INTDIR)/"mtmdi.res" /d "_DEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o$(OUTDIR)/"mtmdi.bsc" 
BSC32_SBRS= \
	.\UniDebug\hello.sbr \
	.\UniDebug\mainfrm.sbr \
	.\UniDebug\mdi.sbr \
	.\UniDebug\bounce.sbr \
	.\UniDebug\stdafx.sbr \
	.\UniDebug\mtbounce.sbr

.\UniDebug\mtmdi.bsc : $(OUTDIR)  $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 /NOLOGO /SUBSYSTEM:windows /DEBUG /MACHINE:MIPS
# ADD LINK32 /NOLOGO /ENTRY:"wWinMainCRTStartup" /SUBSYSTEM:windows /DEBUG /MACHINE:MIPS
LINK32_FLAGS=/NOLOGO /ENTRY:"wWinMainCRTStartup" /SUBSYSTEM:windows\
 /PDB:$(OUTDIR)/"mtmdi.pdb" /DEBUG /MACHINE:MIPS /OUT:$(OUTDIR)/"mtmdi.exe" 
DEF_FILE=
LINK32_OBJS= \
	.\UniDebug\hello.obj \
	.\UniDebug\mainfrm.obj \
	.\UniDebug\mdi.obj \
	.\UniDebug\bounce.obj \
	.\UniDebug\stdafx.obj \
	.\UniDebug\mtmdi.res \
	.\UniDebug\mtbounce.obj

.\UniDebug\mtmdi.exe : $(OUTDIR)  $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 

################################################################################
# Begin Group "Source Files"

################################################################################
# Begin Source File

SOURCE=.\hello.cpp
DEP_HELLO=\
	.\stdafx.h\
	.\hello.h\
	.\mdi.h

!IF  "$(CFG)" == "Win32 (80x86) Release"

# ADD BASE CPP /Yu"STDAFX.H"
# ADD CPP /Yu"stdafx.h"

.\WinRel\hello.obj :  $(SOURCE)  $(DEP_HELLO) $(INTDIR) .\WinRel\stdafx.obj
   $(CPP) /nologo /MD /W3 /GX /Ox /D "NDEBUG" /D "_WINDOWS" /D "_X86_" /D\
 "_AFXDLL" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"mtmdi.pch" /Yu"stdafx.h"\
 /Fo$(INTDIR)/ /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

# ADD BASE CPP /Yu"STDAFX.H"
# ADD CPP /Yu"stdafx.h"

.\WinDebug\hello.obj :  $(SOURCE)  $(DEP_HELLO) $(INTDIR) .\WinDebug\stdafx.obj
   $(CPP) /nologo /MD /W3 /GX /Zi /Od /D "_DEBUG" /D "_WINDOWS" /D "_X86_" /D\
 "_AFXDLL" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"mtmdi.pch" /Yu"stdafx.h"\
 /Fo$(INTDIR)/ /Fd$(OUTDIR)/"mtmdi.pdb" /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (80x86) Unicode Debug"

# ADD BASE CPP /Yu"STDAFX.H"
# ADD CPP /Yu"stdafx.h"

.\UniDebug\hello.obj :  $(SOURCE)  $(DEP_HELLO) $(INTDIR) .\UniDebug\stdafx.obj
   $(CPP) /nologo /MD /W3 /GX /Zi /Od /D "_DEBUG" /D "_WINDOWS" /D "_X86_" /D\
 "_AFXDLL" /D "_MBCS" /D "_UNICODE" /FR$(INTDIR)/ /Fp$(OUTDIR)/"mtmdi.pch"\
 /Yu"stdafx.h" /Fo$(INTDIR)/ /Fd$(OUTDIR)/"mtmdi.pdb" /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (80x86) Unicode Release"

# ADD BASE CPP /Yu"STDAFX.H"
# ADD CPP /Yu"stdafx.h"

.\UniRel\hello.obj :  $(SOURCE)  $(DEP_HELLO) $(INTDIR) .\UniRel\stdafx.obj
   $(CPP) /nologo /MD /W3 /GX /Ox /D "NDEBUG" /D "_WINDOWS" /D "_X86_" /D\
 "_AFXDLL" /D "_MBCS" /D "_UNICODE" /FR$(INTDIR)/ /Fp$(OUTDIR)/"mtmdi.pch"\
 /Yu"stdafx.h" /Fo$(INTDIR)/ /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

# ADD BASE CPP /Gt0 /QMOb2000
# ADD CPP /Gt0 /QMOb2000 /Yu"stdafx.h"

.\WinRel\hello.obj :  $(SOURCE)  $(DEP_HELLO) $(INTDIR) .\WinRel\stdafx.obj
   $(CPP) /nologo /MD /Gt0 /QMOb2000 /W3 /GX /O2 /D "NDEBUG" /D "_WINDOWS" /D\
 "_MIPS_" /D "_AFXDLL" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"mtmdi.pch"\
 /Yu"stdafx.h" /Fo$(INTDIR)/ /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

# ADD BASE CPP /Gt0 /QMOb2000
# ADD CPP /Gt0 /QMOb2000 /Yu"stdafx.h"

.\WinDebug\hello.obj :  $(SOURCE)  $(DEP_HELLO) $(INTDIR) .\WinDebug\stdafx.obj
   $(CPP) /nologo /MD /Gt0 /QMOb2000 /W3 /GX /Zi /Od /D "_DEBUG" /D "_WINDOWS"\
 /D "_MIPS_" /D "_AFXDLL" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"mtmdi.pch"\
 /Yu"stdafx.h" /Fo$(INTDIR)/ /Fd$(OUTDIR)/"mtmdi.pdb" /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Unicode Release"

# ADD BASE CPP /Gt0 /QMOb2000
# ADD CPP /Gt0 /QMOb2000 /Yu"stdafx.h"

.\UniRel\hello.obj :  $(SOURCE)  $(DEP_HELLO) $(INTDIR) .\UniRel\stdafx.obj
   $(CPP) /nologo /MD /Gt0 /QMOb2000 /W3 /GX /O2 /D "NDEBUG" /D "_WINDOWS" /D\
 "_MIPS_" /D "_AFXDLL" /D "_MBCS" /D "_UNICODE" /FR$(INTDIR)/\
 /Fp$(OUTDIR)/"mtmdi.pch" /Yu"stdafx.h" /Fo$(INTDIR)/ /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Unicode Debug"

# ADD BASE CPP /Gt0 /QMOb2000
# ADD CPP /Gt0 /QMOb2000 /Yu"stdafx.h"

.\UniDebug\hello.obj :  $(SOURCE)  $(DEP_HELLO) $(INTDIR) .\UniDebug\stdafx.obj
   $(CPP) /nologo /MD /Gt0 /QMOb2000 /W3 /GX /Zi /Od /D "_DEBUG" /D "_WINDOWS"\
 /D "_MIPS_" /D "_AFXDLL" /D "_MBCS" /D "_UNICODE" /FR$(INTDIR)/\
 /Fp$(OUTDIR)/"mtmdi.pch" /Yu"stdafx.h" /Fo$(INTDIR)/ /Fd$(OUTDIR)/"mtmdi.pdb"\
 /c  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\mainfrm.cpp
DEP_MAINF=\
	.\stdafx.h\
	.\bounce.h\
	.\hello.h\
	.\mdi.h\
	.\mainfrm.h\
	.\mtbounce.h

!IF  "$(CFG)" == "Win32 (80x86) Release"

# ADD BASE CPP /Yu"STDAFX.H"
# ADD CPP /Yu"stdafx.h"

.\WinRel\mainfrm.obj :  $(SOURCE)  $(DEP_MAINF) $(INTDIR) .\WinRel\stdafx.obj
   $(CPP) /nologo /MD /W3 /GX /Ox /D "NDEBUG" /D "_WINDOWS" /D "_X86_" /D\
 "_AFXDLL" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"mtmdi.pch" /Yu"stdafx.h"\
 /Fo$(INTDIR)/ /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

# ADD BASE CPP /Yu"STDAFX.H"
# ADD CPP /Yu"stdafx.h"

.\WinDebug\mainfrm.obj :  $(SOURCE)  $(DEP_MAINF) $(INTDIR)\
 .\WinDebug\stdafx.obj
   $(CPP) /nologo /MD /W3 /GX /Zi /Od /D "_DEBUG" /D "_WINDOWS" /D "_X86_" /D\
 "_AFXDLL" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"mtmdi.pch" /Yu"stdafx.h"\
 /Fo$(INTDIR)/ /Fd$(OUTDIR)/"mtmdi.pdb" /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (80x86) Unicode Debug"

# ADD BASE CPP /Yu"STDAFX.H"
# ADD CPP /Yu"stdafx.h"

.\UniDebug\mainfrm.obj :  $(SOURCE)  $(DEP_MAINF) $(INTDIR)\
 .\UniDebug\stdafx.obj
   $(CPP) /nologo /MD /W3 /GX /Zi /Od /D "_DEBUG" /D "_WINDOWS" /D "_X86_" /D\
 "_AFXDLL" /D "_MBCS" /D "_UNICODE" /FR$(INTDIR)/ /Fp$(OUTDIR)/"mtmdi.pch"\
 /Yu"stdafx.h" /Fo$(INTDIR)/ /Fd$(OUTDIR)/"mtmdi.pdb" /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (80x86) Unicode Release"

# ADD BASE CPP /Yu"STDAFX.H"
# ADD CPP /Yu"stdafx.h"

.\UniRel\mainfrm.obj :  $(SOURCE)  $(DEP_MAINF) $(INTDIR) .\UniRel\stdafx.obj
   $(CPP) /nologo /MD /W3 /GX /Ox /D "NDEBUG" /D "_WINDOWS" /D "_X86_" /D\
 "_AFXDLL" /D "_MBCS" /D "_UNICODE" /FR$(INTDIR)/ /Fp$(OUTDIR)/"mtmdi.pch"\
 /Yu"stdafx.h" /Fo$(INTDIR)/ /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

# ADD BASE CPP /Gt0 /QMOb2000
# ADD CPP /Gt0 /QMOb2000 /Yu"stdafx.h"

.\WinRel\mainfrm.obj :  $(SOURCE)  $(DEP_MAINF) $(INTDIR) .\WinRel\stdafx.obj
   $(CPP) /nologo /MD /Gt0 /QMOb2000 /W3 /GX /O2 /D "NDEBUG" /D "_WINDOWS" /D\
 "_MIPS_" /D "_AFXDLL" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"mtmdi.pch"\
 /Yu"stdafx.h" /Fo$(INTDIR)/ /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

# ADD BASE CPP /Gt0 /QMOb2000
# ADD CPP /Gt0 /QMOb2000 /Yu"stdafx.h"

.\WinDebug\mainfrm.obj :  $(SOURCE)  $(DEP_MAINF) $(INTDIR)\
 .\WinDebug\stdafx.obj
   $(CPP) /nologo /MD /Gt0 /QMOb2000 /W3 /GX /Zi /Od /D "_DEBUG" /D "_WINDOWS"\
 /D "_MIPS_" /D "_AFXDLL" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"mtmdi.pch"\
 /Yu"stdafx.h" /Fo$(INTDIR)/ /Fd$(OUTDIR)/"mtmdi.pdb" /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Unicode Release"

# ADD BASE CPP /Gt0 /QMOb2000
# ADD CPP /Gt0 /QMOb2000 /Yu"stdafx.h"

.\UniRel\mainfrm.obj :  $(SOURCE)  $(DEP_MAINF) $(INTDIR) .\UniRel\stdafx.obj
   $(CPP) /nologo /MD /Gt0 /QMOb2000 /W3 /GX /O2 /D "NDEBUG" /D "_WINDOWS" /D\
 "_MIPS_" /D "_AFXDLL" /D "_MBCS" /D "_UNICODE" /FR$(INTDIR)/\
 /Fp$(OUTDIR)/"mtmdi.pch" /Yu"stdafx.h" /Fo$(INTDIR)/ /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Unicode Debug"

# ADD BASE CPP /Gt0 /QMOb2000
# ADD CPP /Gt0 /QMOb2000 /Yu"stdafx.h"

.\UniDebug\mainfrm.obj :  $(SOURCE)  $(DEP_MAINF) $(INTDIR)\
 .\UniDebug\stdafx.obj
   $(CPP) /nologo /MD /Gt0 /QMOb2000 /W3 /GX /Zi /Od /D "_DEBUG" /D "_WINDOWS"\
 /D "_MIPS_" /D "_AFXDLL" /D "_MBCS" /D "_UNICODE" /FR$(INTDIR)/\
 /Fp$(OUTDIR)/"mtmdi.pch" /Yu"stdafx.h" /Fo$(INTDIR)/ /Fd$(OUTDIR)/"mtmdi.pdb"\
 /c  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\mdi.cpp
DEP_MDI_C=\
	.\stdafx.h\
	.\mdi.h\
	.\mainfrm.h\
	.\bounce.h\
	.\mtbounce.h

!IF  "$(CFG)" == "Win32 (80x86) Release"

# ADD BASE CPP /Yu"STDAFX.H"
# ADD CPP /Yu"stdafx.h"

.\WinRel\mdi.obj :  $(SOURCE)  $(DEP_MDI_C) $(INTDIR) .\WinRel\stdafx.obj
   $(CPP) /nologo /MD /W3 /GX /Ox /D "NDEBUG" /D "_WINDOWS" /D "_X86_" /D\
 "_AFXDLL" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"mtmdi.pch" /Yu"stdafx.h"\
 /Fo$(INTDIR)/ /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

# ADD BASE CPP /Yu"STDAFX.H"
# ADD CPP /Yu"stdafx.h"

.\WinDebug\mdi.obj :  $(SOURCE)  $(DEP_MDI_C) $(INTDIR) .\WinDebug\stdafx.obj
   $(CPP) /nologo /MD /W3 /GX /Zi /Od /D "_DEBUG" /D "_WINDOWS" /D "_X86_" /D\
 "_AFXDLL" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"mtmdi.pch" /Yu"stdafx.h"\
 /Fo$(INTDIR)/ /Fd$(OUTDIR)/"mtmdi.pdb" /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (80x86) Unicode Debug"

# ADD BASE CPP /Yu"STDAFX.H"
# ADD CPP /Yu"stdafx.h"

.\UniDebug\mdi.obj :  $(SOURCE)  $(DEP_MDI_C) $(INTDIR) .\UniDebug\stdafx.obj
   $(CPP) /nologo /MD /W3 /GX /Zi /Od /D "_DEBUG" /D "_WINDOWS" /D "_X86_" /D\
 "_AFXDLL" /D "_MBCS" /D "_UNICODE" /FR$(INTDIR)/ /Fp$(OUTDIR)/"mtmdi.pch"\
 /Yu"stdafx.h" /Fo$(INTDIR)/ /Fd$(OUTDIR)/"mtmdi.pdb" /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (80x86) Unicode Release"

# ADD BASE CPP /Yu"STDAFX.H"
# ADD CPP /Yu"stdafx.h"

.\UniRel\mdi.obj :  $(SOURCE)  $(DEP_MDI_C) $(INTDIR) .\UniRel\stdafx.obj
   $(CPP) /nologo /MD /W3 /GX /Ox /D "NDEBUG" /D "_WINDOWS" /D "_X86_" /D\
 "_AFXDLL" /D "_MBCS" /D "_UNICODE" /FR$(INTDIR)/ /Fp$(OUTDIR)/"mtmdi.pch"\
 /Yu"stdafx.h" /Fo$(INTDIR)/ /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

# ADD BASE CPP /Gt0 /QMOb2000
# ADD CPP /Gt0 /QMOb2000 /Yu"stdafx.h"

.\WinRel\mdi.obj :  $(SOURCE)  $(DEP_MDI_C) $(INTDIR) .\WinRel\stdafx.obj
   $(CPP) /nologo /MD /Gt0 /QMOb2000 /W3 /GX /O2 /D "NDEBUG" /D "_WINDOWS" /D\
 "_MIPS_" /D "_AFXDLL" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"mtmdi.pch"\
 /Yu"stdafx.h" /Fo$(INTDIR)/ /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

# ADD BASE CPP /Gt0 /QMOb2000
# ADD CPP /Gt0 /QMOb2000 /Yu"stdafx.h"

.\WinDebug\mdi.obj :  $(SOURCE)  $(DEP_MDI_C) $(INTDIR) .\WinDebug\stdafx.obj
   $(CPP) /nologo /MD /Gt0 /QMOb2000 /W3 /GX /Zi /Od /D "_DEBUG" /D "_WINDOWS"\
 /D "_MIPS_" /D "_AFXDLL" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"mtmdi.pch"\
 /Yu"stdafx.h" /Fo$(INTDIR)/ /Fd$(OUTDIR)/"mtmdi.pdb" /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Unicode Release"

# ADD BASE CPP /Gt0 /QMOb2000
# ADD CPP /Gt0 /QMOb2000 /Yu"stdafx.h"

.\UniRel\mdi.obj :  $(SOURCE)  $(DEP_MDI_C) $(INTDIR) .\UniRel\stdafx.obj
   $(CPP) /nologo /MD /Gt0 /QMOb2000 /W3 /GX /O2 /D "NDEBUG" /D "_WINDOWS" /D\
 "_MIPS_" /D "_AFXDLL" /D "_MBCS" /D "_UNICODE" /FR$(INTDIR)/\
 /Fp$(OUTDIR)/"mtmdi.pch" /Yu"stdafx.h" /Fo$(INTDIR)/ /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Unicode Debug"

# ADD BASE CPP /Gt0 /QMOb2000
# ADD CPP /Gt0 /QMOb2000 /Yu"stdafx.h"

.\UniDebug\mdi.obj :  $(SOURCE)  $(DEP_MDI_C) $(INTDIR) .\UniDebug\stdafx.obj
   $(CPP) /nologo /MD /Gt0 /QMOb2000 /W3 /GX /Zi /Od /D "_DEBUG" /D "_WINDOWS"\
 /D "_MIPS_" /D "_AFXDLL" /D "_MBCS" /D "_UNICODE" /FR$(INTDIR)/\
 /Fp$(OUTDIR)/"mtmdi.pch" /Yu"stdafx.h" /Fo$(INTDIR)/ /Fd$(OUTDIR)/"mtmdi.pdb"\
 /c  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\bounce.cpp
DEP_BOUNC=\
	.\stdafx.h\
	.\mdi.h\
	.\bounce.h\
	.\mtbounce.h

!IF  "$(CFG)" == "Win32 (80x86) Release"

# ADD BASE CPP /Yu"STDAFX.H"
# ADD CPP /Yu"stdafx.h"

.\WinRel\bounce.obj :  $(SOURCE)  $(DEP_BOUNC) $(INTDIR) .\WinRel\stdafx.obj
   $(CPP) /nologo /MD /W3 /GX /Ox /D "NDEBUG" /D "_WINDOWS" /D "_X86_" /D\
 "_AFXDLL" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"mtmdi.pch" /Yu"stdafx.h"\
 /Fo$(INTDIR)/ /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

# ADD BASE CPP /Yu"STDAFX.H"
# ADD CPP /Yu"stdafx.h"

.\WinDebug\bounce.obj :  $(SOURCE)  $(DEP_BOUNC) $(INTDIR)\
 .\WinDebug\stdafx.obj
   $(CPP) /nologo /MD /W3 /GX /Zi /Od /D "_DEBUG" /D "_WINDOWS" /D "_X86_" /D\
 "_AFXDLL" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"mtmdi.pch" /Yu"stdafx.h"\
 /Fo$(INTDIR)/ /Fd$(OUTDIR)/"mtmdi.pdb" /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (80x86) Unicode Debug"

# ADD BASE CPP /Yu"STDAFX.H"
# ADD CPP /Yu"stdafx.h"

.\UniDebug\bounce.obj :  $(SOURCE)  $(DEP_BOUNC) $(INTDIR)\
 .\UniDebug\stdafx.obj
   $(CPP) /nologo /MD /W3 /GX /Zi /Od /D "_DEBUG" /D "_WINDOWS" /D "_X86_" /D\
 "_AFXDLL" /D "_MBCS" /D "_UNICODE" /FR$(INTDIR)/ /Fp$(OUTDIR)/"mtmdi.pch"\
 /Yu"stdafx.h" /Fo$(INTDIR)/ /Fd$(OUTDIR)/"mtmdi.pdb" /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (80x86) Unicode Release"

# ADD BASE CPP /Yu"STDAFX.H"
# ADD CPP /Yu"stdafx.h"

.\UniRel\bounce.obj :  $(SOURCE)  $(DEP_BOUNC) $(INTDIR) .\UniRel\stdafx.obj
   $(CPP) /nologo /MD /W3 /GX /Ox /D "NDEBUG" /D "_WINDOWS" /D "_X86_" /D\
 "_AFXDLL" /D "_MBCS" /D "_UNICODE" /FR$(INTDIR)/ /Fp$(OUTDIR)/"mtmdi.pch"\
 /Yu"stdafx.h" /Fo$(INTDIR)/ /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

# ADD BASE CPP /Gt0 /QMOb2000
# ADD CPP /Gt0 /QMOb2000 /Yu"stdafx.h"

.\WinRel\bounce.obj :  $(SOURCE)  $(DEP_BOUNC) $(INTDIR) .\WinRel\stdafx.obj
   $(CPP) /nologo /MD /Gt0 /QMOb2000 /W3 /GX /O2 /D "NDEBUG" /D "_WINDOWS" /D\
 "_MIPS_" /D "_AFXDLL" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"mtmdi.pch"\
 /Yu"stdafx.h" /Fo$(INTDIR)/ /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

# ADD BASE CPP /Gt0 /QMOb2000
# ADD CPP /Gt0 /QMOb2000 /Yu"stdafx.h"

.\WinDebug\bounce.obj :  $(SOURCE)  $(DEP_BOUNC) $(INTDIR)\
 .\WinDebug\stdafx.obj
   $(CPP) /nologo /MD /Gt0 /QMOb2000 /W3 /GX /Zi /Od /D "_DEBUG" /D "_WINDOWS"\
 /D "_MIPS_" /D "_AFXDLL" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"mtmdi.pch"\
 /Yu"stdafx.h" /Fo$(INTDIR)/ /Fd$(OUTDIR)/"mtmdi.pdb" /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Unicode Release"

# ADD BASE CPP /Gt0 /QMOb2000
# ADD CPP /Gt0 /QMOb2000 /Yu"stdafx.h"

.\UniRel\bounce.obj :  $(SOURCE)  $(DEP_BOUNC) $(INTDIR) .\UniRel\stdafx.obj
   $(CPP) /nologo /MD /Gt0 /QMOb2000 /W3 /GX /O2 /D "NDEBUG" /D "_WINDOWS" /D\
 "_MIPS_" /D "_AFXDLL" /D "_MBCS" /D "_UNICODE" /FR$(INTDIR)/\
 /Fp$(OUTDIR)/"mtmdi.pch" /Yu"stdafx.h" /Fo$(INTDIR)/ /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Unicode Debug"

# ADD BASE CPP /Gt0 /QMOb2000
# ADD CPP /Gt0 /QMOb2000 /Yu"stdafx.h"

.\UniDebug\bounce.obj :  $(SOURCE)  $(DEP_BOUNC) $(INTDIR)\
 .\UniDebug\stdafx.obj
   $(CPP) /nologo /MD /Gt0 /QMOb2000 /W3 /GX /Zi /Od /D "_DEBUG" /D "_WINDOWS"\
 /D "_MIPS_" /D "_AFXDLL" /D "_MBCS" /D "_UNICODE" /FR$(INTDIR)/\
 /Fp$(OUTDIR)/"mtmdi.pch" /Yu"stdafx.h" /Fo$(INTDIR)/ /Fd$(OUTDIR)/"mtmdi.pdb"\
 /c  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\stdafx.cpp
DEP_STDAF=\
	.\stdafx.h

!IF  "$(CFG)" == "Win32 (80x86) Release"

# ADD CPP /Yc"stdafx.h"

.\WinRel\stdafx.obj :  $(SOURCE)  $(DEP_STDAF) $(INTDIR)
   $(CPP) /nologo /MD /W3 /GX /Ox /D "NDEBUG" /D "_WINDOWS" /D "_X86_" /D\
 "_AFXDLL" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"mtmdi.pch" /Yc"stdafx.h"\
 /Fo$(INTDIR)/ /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

# ADD CPP /Yc"stdafx.h"

.\WinDebug\stdafx.obj :  $(SOURCE)  $(DEP_STDAF) $(INTDIR)
   $(CPP) /nologo /MD /W3 /GX /Zi /Od /D "_DEBUG" /D "_WINDOWS" /D "_X86_" /D\
 "_AFXDLL" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"mtmdi.pch" /Yc"stdafx.h"\
 /Fo$(INTDIR)/ /Fd$(OUTDIR)/"mtmdi.pdb" /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (80x86) Unicode Debug"

# ADD BASE CPP /Yc"stdafx.h"
# ADD CPP /Yc"stdafx.h"

.\UniDebug\stdafx.obj :  $(SOURCE)  $(DEP_STDAF) $(INTDIR)
   $(CPP) /nologo /MD /W3 /GX /Zi /Od /D "_DEBUG" /D "_WINDOWS" /D "_X86_" /D\
 "_AFXDLL" /D "_MBCS" /D "_UNICODE" /FR$(INTDIR)/ /Fp$(OUTDIR)/"mtmdi.pch"\
 /Yc"stdafx.h" /Fo$(INTDIR)/ /Fd$(OUTDIR)/"mtmdi.pdb" /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (80x86) Unicode Release"

# ADD BASE CPP /Yc"stdafx.h"
# ADD CPP /Yc"stdafx.h"

.\UniRel\stdafx.obj :  $(SOURCE)  $(DEP_STDAF) $(INTDIR)
   $(CPP) /nologo /MD /W3 /GX /Ox /D "NDEBUG" /D "_WINDOWS" /D "_X86_" /D\
 "_AFXDLL" /D "_MBCS" /D "_UNICODE" /FR$(INTDIR)/ /Fp$(OUTDIR)/"mtmdi.pch"\
 /Yc"stdafx.h" /Fo$(INTDIR)/ /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

# ADD BASE CPP /Gt0 /QMOb2000
# ADD CPP /Gt0 /QMOb2000 /Yc"stdafx.h"

.\WinRel\stdafx.obj :  $(SOURCE)  $(DEP_STDAF) $(INTDIR)
   $(CPP) /nologo /MD /Gt0 /QMOb2000 /W3 /GX /O2 /D "NDEBUG" /D "_WINDOWS" /D\
 "_MIPS_" /D "_AFXDLL" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"mtmdi.pch"\
 /Yc"stdafx.h" /Fo$(INTDIR)/ /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

# ADD BASE CPP /Gt0 /QMOb2000
# ADD CPP /Gt0 /QMOb2000 /Yc"stdafx.h"

.\WinDebug\stdafx.obj :  $(SOURCE)  $(DEP_STDAF) $(INTDIR)
   $(CPP) /nologo /MD /Gt0 /QMOb2000 /W3 /GX /Zi /Od /D "_DEBUG" /D "_WINDOWS"\
 /D "_MIPS_" /D "_AFXDLL" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"mtmdi.pch"\
 /Yc"stdafx.h" /Fo$(INTDIR)/ /Fd$(OUTDIR)/"mtmdi.pdb" /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Unicode Release"

# ADD BASE CPP /Gt0 /QMOb2000
# ADD CPP /Gt0 /QMOb2000 /Yc"stdafx.h"

.\UniRel\stdafx.obj :  $(SOURCE)  $(DEP_STDAF) $(INTDIR)
   $(CPP) /nologo /MD /Gt0 /QMOb2000 /W3 /GX /O2 /D "NDEBUG" /D "_WINDOWS" /D\
 "_MIPS_" /D "_AFXDLL" /D "_MBCS" /D "_UNICODE" /FR$(INTDIR)/\
 /Fp$(OUTDIR)/"mtmdi.pch" /Yc"stdafx.h" /Fo$(INTDIR)/ /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Unicode Debug"

# ADD BASE CPP /Gt0 /QMOb2000
# ADD CPP /Gt0 /QMOb2000 /Yc"stdafx.h"

.\UniDebug\stdafx.obj :  $(SOURCE)  $(DEP_STDAF) $(INTDIR)
   $(CPP) /nologo /MD /Gt0 /QMOb2000 /W3 /GX /Zi /Od /D "_DEBUG" /D "_WINDOWS"\
 /D "_MIPS_" /D "_AFXDLL" /D "_MBCS" /D "_UNICODE" /FR$(INTDIR)/\
 /Fp$(OUTDIR)/"mtmdi.pch" /Yc"stdafx.h" /Fo$(INTDIR)/ /Fd$(OUTDIR)/"mtmdi.pdb"\
 /c  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\mtmdi.rc
DEP_MTMDI=\
	.\mdi.ico\
	.\hello.ico

!IF  "$(CFG)" == "Win32 (80x86) Release"

.\WinRel\mtmdi.res :  $(SOURCE)  $(DEP_MTMDI) $(INTDIR)
   $(RSC) $(RSC_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

.\WinDebug\mtmdi.res :  $(SOURCE)  $(DEP_MTMDI) $(INTDIR)
   $(RSC) $(RSC_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (80x86) Unicode Debug"

.\UniDebug\mtmdi.res :  $(SOURCE)  $(DEP_MTMDI) $(INTDIR)
   $(RSC) $(RSC_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (80x86) Unicode Release"

.\UniRel\mtmdi.res :  $(SOURCE)  $(DEP_MTMDI) $(INTDIR)
   $(RSC) $(RSC_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

.\WinRel\mtmdi.res :  $(SOURCE)  $(DEP_MTMDI) $(INTDIR)
   $(RSC) $(RSC_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

.\WinDebug\mtmdi.res :  $(SOURCE)  $(DEP_MTMDI) $(INTDIR)
   $(RSC) $(RSC_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Unicode Release"

.\UniRel\mtmdi.res :  $(SOURCE)  $(DEP_MTMDI) $(INTDIR)
   $(RSC) $(RSC_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Unicode Debug"

.\UniDebug\mtmdi.res :  $(SOURCE)  $(DEP_MTMDI) $(INTDIR)
   $(RSC) $(RSC_PROJ)  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\mtbounce.cpp
DEP_MTBOU=\
	.\stdafx.h\
	.\bounce.h\
	.\mtbounce.h

!IF  "$(CFG)" == "Win32 (80x86) Release"

# ADD CPP /Yu"stdafx.h"

.\WinRel\mtbounce.obj :  $(SOURCE)  $(DEP_MTBOU) $(INTDIR) .\WinRel\stdafx.obj
   $(CPP) /nologo /MD /W3 /GX /Ox /D "NDEBUG" /D "_WINDOWS" /D "_X86_" /D\
 "_AFXDLL" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"mtmdi.pch" /Yu"stdafx.h"\
 /Fo$(INTDIR)/ /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

# ADD CPP /Yu"stdafx.h"

.\WinDebug\mtbounce.obj :  $(SOURCE)  $(DEP_MTBOU) $(INTDIR)\
 .\WinDebug\stdafx.obj
   $(CPP) /nologo /MD /W3 /GX /Zi /Od /D "_DEBUG" /D "_WINDOWS" /D "_X86_" /D\
 "_AFXDLL" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"mtmdi.pch" /Yu"stdafx.h"\
 /Fo$(INTDIR)/ /Fd$(OUTDIR)/"mtmdi.pdb" /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (80x86) Unicode Debug"

# ADD BASE CPP /Yu"stdafx.h"
# ADD CPP /Yu"stdafx.h"

.\UniDebug\mtbounce.obj :  $(SOURCE)  $(DEP_MTBOU) $(INTDIR)\
 .\UniDebug\stdafx.obj
   $(CPP) /nologo /MD /W3 /GX /Zi /Od /D "_DEBUG" /D "_WINDOWS" /D "_X86_" /D\
 "_AFXDLL" /D "_MBCS" /D "_UNICODE" /FR$(INTDIR)/ /Fp$(OUTDIR)/"mtmdi.pch"\
 /Yu"stdafx.h" /Fo$(INTDIR)/ /Fd$(OUTDIR)/"mtmdi.pdb" /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (80x86) Unicode Release"

# ADD BASE CPP /Yu"stdafx.h"
# ADD CPP /Yu"stdafx.h"

.\UniRel\mtbounce.obj :  $(SOURCE)  $(DEP_MTBOU) $(INTDIR) .\UniRel\stdafx.obj
   $(CPP) /nologo /MD /W3 /GX /Ox /D "NDEBUG" /D "_WINDOWS" /D "_X86_" /D\
 "_AFXDLL" /D "_MBCS" /D "_UNICODE" /FR$(INTDIR)/ /Fp$(OUTDIR)/"mtmdi.pch"\
 /Yu"stdafx.h" /Fo$(INTDIR)/ /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

# ADD BASE CPP /Gt0 /QMOb2000
# ADD CPP /Gt0 /QMOb2000 /Yu"stdafx.h"

.\WinRel\mtbounce.obj :  $(SOURCE)  $(DEP_MTBOU) $(INTDIR) .\WinRel\stdafx.obj
   $(CPP) /nologo /MD /Gt0 /QMOb2000 /W3 /GX /O2 /D "NDEBUG" /D "_WINDOWS" /D\
 "_MIPS_" /D "_AFXDLL" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"mtmdi.pch"\
 /Yu"stdafx.h" /Fo$(INTDIR)/ /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

# ADD BASE CPP /Gt0 /QMOb2000
# ADD CPP /Gt0 /QMOb2000 /Yu"stdafx.h"

.\WinDebug\mtbounce.obj :  $(SOURCE)  $(DEP_MTBOU) $(INTDIR)\
 .\WinDebug\stdafx.obj
   $(CPP) /nologo /MD /Gt0 /QMOb2000 /W3 /GX /Zi /Od /D "_DEBUG" /D "_WINDOWS"\
 /D "_MIPS_" /D "_AFXDLL" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"mtmdi.pch"\
 /Yu"stdafx.h" /Fo$(INTDIR)/ /Fd$(OUTDIR)/"mtmdi.pdb" /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Unicode Release"

# ADD BASE CPP /Gt0 /QMOb2000
# ADD CPP /Gt0 /QMOb2000 /Yu"stdafx.h"

.\UniRel\mtbounce.obj :  $(SOURCE)  $(DEP_MTBOU) $(INTDIR) .\UniRel\stdafx.obj
   $(CPP) /nologo /MD /Gt0 /QMOb2000 /W3 /GX /O2 /D "NDEBUG" /D "_WINDOWS" /D\
 "_MIPS_" /D "_AFXDLL" /D "_MBCS" /D "_UNICODE" /FR$(INTDIR)/\
 /Fp$(OUTDIR)/"mtmdi.pch" /Yu"stdafx.h" /Fo$(INTDIR)/ /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Unicode Debug"

# ADD BASE CPP /Gt0 /QMOb2000
# ADD CPP /Gt0 /QMOb2000 /Yu"stdafx.h"

.\UniDebug\mtbounce.obj :  $(SOURCE)  $(DEP_MTBOU) $(INTDIR)\
 .\UniDebug\stdafx.obj
   $(CPP) /nologo /MD /Gt0 /QMOb2000 /W3 /GX /Zi /Od /D "_DEBUG" /D "_WINDOWS"\
 /D "_MIPS_" /D "_AFXDLL" /D "_MBCS" /D "_UNICODE" /FR$(INTDIR)/\
 /Fp$(OUTDIR)/"mtmdi.pch" /Yu"stdafx.h" /Fo$(INTDIR)/ /Fd$(OUTDIR)/"mtmdi.pdb"\
 /c  $(SOURCE) 

!ENDIF 

# End Source File
# End Group
# End Project
################################################################################
