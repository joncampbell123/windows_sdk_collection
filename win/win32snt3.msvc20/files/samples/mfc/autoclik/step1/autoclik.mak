# Microsoft Visual C++ Generated NMAKE File, Format Version 2.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101
# TARGTYPE "Win32 (MIPS) Application" 0x0501

!IF "$(CFG)" == ""
CFG=Win32 (80x86) Debug
!MESSAGE No configuration specified.  Defaulting to Win32 (80x86) Debug.
!ENDIF 

!IF "$(CFG)" != "Win32 (80x86) Debug" && "$(CFG)" != "Win32 (80x86) Release" &&\
 "$(CFG)" != "Win32 (80x86) Unicode Debug" && "$(CFG)" !=\
 "Win32 (80x86) Unicode Release" && "$(CFG)" != "Win32 (MIPS) Release" &&\
 "$(CFG)" != "Win32 (MIPS) Debug" && "$(CFG)" != "Win32 (MIPS) Unicode Release"\
 && "$(CFG)" != "Win32 (MIPS) Unicode Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "autoclik.mak" CFG="Win32 (80x86) Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Win32 (80x86) Debug" (based on "Win32 (x86) Application")
!MESSAGE "Win32 (80x86) Release" (based on "Win32 (x86) Application")
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
# PROP Target_Last_Scanned "Win32 (80x86) Debug"

!IF  "$(CFG)" == "Win32 (80x86) Debug"

# PROP BASE Use_MFC 5
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "WinDebug"
# PROP BASE Intermediate_Dir "WinDebug"
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "WinDebug"
# PROP Intermediate_Dir "WinDebug"
OUTDIR=.\WinDebug
INTDIR=.\WinDebug

ALL : MTL_TLBS $(OUTDIR)/autoclik.exe $(OUTDIR)/autoclik.bsc

$(OUTDIR) : 
    if not exist $(OUTDIR)/nul mkdir $(OUTDIR)

MTL=MkTypLib.exe
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 

MTL_TLBS : $(OUTDIR)/autoclik.tlb
CPP=cl.exe
# ADD BASE CPP /nologo /MT /W3 /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /FR /Yu"stdafx.h" /c
# ADD CPP /nologo /MD /W3 /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /Yu"stdafx.h" /c
CPP_PROJ=/nologo /MD /W3 /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"autoclik.pch" /Yu"stdafx.h"\
 /Fo$(INTDIR)/ /Fd$(OUTDIR)/"autoclik.pdb" /c 
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
RSC_PROJ=/l 0x409 /fo$(INTDIR)/"autoclik.res" /d "_DEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# SUBTRACT BASE BSC32 /Iu
# ADD BSC32 /nologo
# SUBTRACT BSC32 /Iu
BSC32_FLAGS=/nologo /o$(OUTDIR)/"autoclik.bsc" 
BSC32_SBRS= \
	$(INTDIR)/stdafx.sbr \
	$(INTDIR)/autoclik.sbr \
	$(INTDIR)/mainfrm.sbr \
	$(INTDIR)/autocdoc.sbr \
	$(INTDIR)/autocvw.sbr \
	$(INTDIR)/dialogs.sbr

$(OUTDIR)/autoclik.bsc : $(OUTDIR)  $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 /NOLOGO /SUBSYSTEM:windows /DEBUG /MACHINE:I386
# ADD LINK32 /NOLOGO /SUBSYSTEM:windows /DEBUG /MACHINE:I386
LINK32_FLAGS=/NOLOGO /SUBSYSTEM:windows /INCREMENTAL:yes\
 /PDB:$(OUTDIR)/"autoclik.pdb" /DEBUG /MACHINE:I386\
 /OUT:$(OUTDIR)/"autoclik.exe" 
DEF_FILE=
LINK32_OBJS= \
	$(INTDIR)/stdafx.obj \
	$(INTDIR)/autoclik.obj \
	$(INTDIR)/mainfrm.obj \
	$(INTDIR)/autocdoc.obj \
	$(INTDIR)/autocvw.obj \
	$(INTDIR)/autoclik.res \
	$(INTDIR)/dialogs.obj

$(OUTDIR)/autoclik.exe : $(OUTDIR)  $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "Win32 (80x86) Release"

# PROP BASE Use_MFC 5
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "WinRel"
# PROP BASE Intermediate_Dir "WinRel"
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "WinRel"
# PROP Intermediate_Dir "WinRel"
OUTDIR=.\WinRel
INTDIR=.\WinRel

ALL : MTL_TLBS $(OUTDIR)/autoclik.exe $(OUTDIR)/autoclik.bsc

$(OUTDIR) : 
    if not exist $(OUTDIR)/nul mkdir $(OUTDIR)

MTL=MkTypLib.exe
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 

MTL_TLBS : $(OUTDIR)/autoclik.tlb
CPP=cl.exe
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /FR /Yu"stdafx.h" /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /Yu"stdafx.h" /c
CPP_PROJ=/nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"autoclik.pch" /Yu"stdafx.h"\
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
RSC_PROJ=/l 0x409 /fo$(INTDIR)/"autoclik.res" /d "NDEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# SUBTRACT BASE BSC32 /Iu
# ADD BSC32 /nologo
# SUBTRACT BSC32 /Iu
BSC32_FLAGS=/nologo /o$(OUTDIR)/"autoclik.bsc" 
BSC32_SBRS= \
	$(INTDIR)/stdafx.sbr \
	$(INTDIR)/autoclik.sbr \
	$(INTDIR)/mainfrm.sbr \
	$(INTDIR)/autocdoc.sbr \
	$(INTDIR)/autocvw.sbr \
	$(INTDIR)/dialogs.sbr

$(OUTDIR)/autoclik.bsc : $(OUTDIR)  $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 /NOLOGO /SUBSYSTEM:windows /MACHINE:I386
# ADD LINK32 /NOLOGO /SUBSYSTEM:windows /MACHINE:I386
LINK32_FLAGS=/NOLOGO /SUBSYSTEM:windows /INCREMENTAL:no\
 /PDB:$(OUTDIR)/"autoclik.pdb" /MACHINE:I386 /OUT:$(OUTDIR)/"autoclik.exe" 
DEF_FILE=
LINK32_OBJS= \
	$(INTDIR)/stdafx.obj \
	$(INTDIR)/autoclik.obj \
	$(INTDIR)/mainfrm.obj \
	$(INTDIR)/autocdoc.obj \
	$(INTDIR)/autocvw.obj \
	$(INTDIR)/autoclik.res \
	$(INTDIR)/dialogs.obj

$(OUTDIR)/autoclik.exe : $(OUTDIR)  $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "Win32 (80x86) Unicode Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Win32__8"
# PROP BASE Intermediate_Dir "Win32__8"
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "UniDebug"
# PROP Intermediate_Dir "UniDebug"
OUTDIR=.\UniDebug
INTDIR=.\UniDebug

ALL : MTL_TLBS $(OUTDIR)/autoclik.exe $(OUTDIR)/autoclik.bsc

$(OUTDIR) : 
    if not exist $(OUTDIR)/nul mkdir $(OUTDIR)

MTL=MkTypLib.exe
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 

MTL_TLBS : $(OUTDIR)/autoclik.tlb
CPP=cl.exe
# ADD BASE CPP /nologo /MD /W3 /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /Yu"stdafx.h" /c
# ADD CPP /nologo /MD /W3 /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /D "_UNICODE" /FR /Yu"stdafx.h" /c
CPP_PROJ=/nologo /MD /W3 /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /D "_UNICODE" /FR$(INTDIR)/ /Fp$(OUTDIR)/"autoclik.pch"\
 /Yu"stdafx.h" /Fo$(INTDIR)/ /Fd$(OUTDIR)/"autoclik.pdb" /c 
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
RSC_PROJ=/l 0x409 /fo$(INTDIR)/"autoclik.res" /d "_DEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# SUBTRACT BASE BSC32 /Iu
# ADD BSC32 /nologo
# SUBTRACT BSC32 /Iu
BSC32_FLAGS=/nologo /o$(OUTDIR)/"autoclik.bsc" 
BSC32_SBRS= \
	$(INTDIR)/stdafx.sbr \
	$(INTDIR)/autoclik.sbr \
	$(INTDIR)/mainfrm.sbr \
	$(INTDIR)/autocdoc.sbr \
	$(INTDIR)/autocvw.sbr \
	$(INTDIR)/dialogs.sbr

$(OUTDIR)/autoclik.bsc : $(OUTDIR)  $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 /NOLOGO /SUBSYSTEM:windows /DEBUG /MACHINE:I386
# ADD LINK32 /NOLOGO /ENTRY:"wWinMainCRTStartup" /SUBSYSTEM:windows /DEBUG /MACHINE:I386
LINK32_FLAGS=/NOLOGO /ENTRY:"wWinMainCRTStartup" /SUBSYSTEM:windows\
 /INCREMENTAL:yes /PDB:$(OUTDIR)/"autoclik.pdb" /DEBUG /MACHINE:I386\
 /OUT:$(OUTDIR)/"autoclik.exe" 
DEF_FILE=
LINK32_OBJS= \
	$(INTDIR)/stdafx.obj \
	$(INTDIR)/autoclik.obj \
	$(INTDIR)/mainfrm.obj \
	$(INTDIR)/autocdoc.obj \
	$(INTDIR)/autocvw.obj \
	$(INTDIR)/autoclik.res \
	$(INTDIR)/dialogs.obj

$(OUTDIR)/autoclik.exe : $(OUTDIR)  $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "Win32 (80x86) Unicode Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Win32__0"
# PROP BASE Intermediate_Dir "Win32__0"
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "UniRel"
# PROP Intermediate_Dir "UniRel"
OUTDIR=.\UniRel
INTDIR=.\UniRel

ALL : MTL_TLBS $(OUTDIR)/autoclik.exe $(OUTDIR)/autoclik.bsc

$(OUTDIR) : 
    if not exist $(OUTDIR)/nul mkdir $(OUTDIR)

MTL=MkTypLib.exe
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 

MTL_TLBS : $(OUTDIR)/autoclik.tlb
CPP=cl.exe
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /Yu"stdafx.h" /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /D "_UNICODE" /FR /Yu"stdafx.h" /c
CPP_PROJ=/nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /D "_UNICODE" /FR$(INTDIR)/ /Fp$(OUTDIR)/"autoclik.pch"\
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
RSC_PROJ=/l 0x409 /fo$(INTDIR)/"autoclik.res" /d "NDEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# SUBTRACT BASE BSC32 /Iu
# ADD BSC32 /nologo
# SUBTRACT BSC32 /Iu
BSC32_FLAGS=/nologo /o$(OUTDIR)/"autoclik.bsc" 
BSC32_SBRS= \
	$(INTDIR)/stdafx.sbr \
	$(INTDIR)/autoclik.sbr \
	$(INTDIR)/mainfrm.sbr \
	$(INTDIR)/autocdoc.sbr \
	$(INTDIR)/autocvw.sbr \
	$(INTDIR)/dialogs.sbr

$(OUTDIR)/autoclik.bsc : $(OUTDIR)  $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 /NOLOGO /SUBSYSTEM:windows /MACHINE:I386
# ADD LINK32 /NOLOGO /ENTRY:"wWinMainCRTStartup" /SUBSYSTEM:windows /MACHINE:I386
LINK32_FLAGS=/NOLOGO /ENTRY:"wWinMainCRTStartup" /SUBSYSTEM:windows\
 /INCREMENTAL:no /PDB:$(OUTDIR)/"autoclik.pdb" /MACHINE:I386\
 /OUT:$(OUTDIR)/"autoclik.exe" 
DEF_FILE=
LINK32_OBJS= \
	$(INTDIR)/stdafx.obj \
	$(INTDIR)/autoclik.obj \
	$(INTDIR)/mainfrm.obj \
	$(INTDIR)/autocdoc.obj \
	$(INTDIR)/autocvw.obj \
	$(INTDIR)/autoclik.res \
	$(INTDIR)/dialogs.obj

$(OUTDIR)/autoclik.exe : $(OUTDIR)  $(DEF_FILE) $(LINK32_OBJS)
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

ALL : MTL_TLBS $(OUTDIR)/autoclik.exe $(OUTDIR)/autoclik.bsc

$(OUTDIR) : 
    if not exist $(OUTDIR)/nul mkdir $(OUTDIR)

MTL=MkTypLib.exe
# ADD BASE MTL /nologo /D "NDEBUG" /mips
# ADD MTL /nologo /D "NDEBUG" /mips
MTL_PROJ=/nologo /D "NDEBUG" /mips 

MTL_TLBS : $(OUTDIR)/autoclik.tlb
CPP=cl.exe
# ADD BASE CPP /nologo /MD /Gt0 /QMOb2000 /W3 /GX /YX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /c
# ADD CPP /nologo /MD /Gt0 /QMOb2000 /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /Yu"stdafx.h" /c
CPP_PROJ=/nologo /MD /Gt0 /QMOb2000 /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D\
 "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"autoclik.pch"\
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
RSC_PROJ=/l 0x409 /fo$(INTDIR)/"autoclik.res" /d "NDEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o$(OUTDIR)/"autoclik.bsc" 
BSC32_SBRS= \
	$(INTDIR)/stdafx.sbr \
	$(INTDIR)/autoclik.sbr \
	$(INTDIR)/mainfrm.sbr \
	$(INTDIR)/autocdoc.sbr \
	$(INTDIR)/autocvw.sbr \
	$(INTDIR)/dialogs.sbr

$(OUTDIR)/autoclik.bsc : $(OUTDIR)  $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 /NOLOGO /SUBSYSTEM:windows /MACHINE:MIPS
# ADD LINK32 /NOLOGO /SUBSYSTEM:windows /MACHINE:MIPS
LINK32_FLAGS=/NOLOGO /SUBSYSTEM:windows /PDB:$(OUTDIR)/"autoclik.pdb"\
 /MACHINE:MIPS /OUT:$(OUTDIR)/"autoclik.exe" 
DEF_FILE=
LINK32_OBJS= \
	$(INTDIR)/stdafx.obj \
	$(INTDIR)/autoclik.obj \
	$(INTDIR)/mainfrm.obj \
	$(INTDIR)/autocdoc.obj \
	$(INTDIR)/autocvw.obj \
	$(INTDIR)/autoclik.res \
	$(INTDIR)/dialogs.obj

$(OUTDIR)/autoclik.exe : $(OUTDIR)  $(DEF_FILE) $(LINK32_OBJS)
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

ALL : MTL_TLBS $(OUTDIR)/autoclik.exe $(OUTDIR)/autoclik.bsc

$(OUTDIR) : 
    if not exist $(OUTDIR)/nul mkdir $(OUTDIR)

MTL=MkTypLib.exe
# ADD BASE MTL /nologo /D "_DEBUG" /mips
# ADD MTL /nologo /D "_DEBUG" /mips
MTL_PROJ=/nologo /D "_DEBUG" /mips 

MTL_TLBS : $(OUTDIR)/autoclik.tlb
CPP=cl.exe
# ADD BASE CPP /nologo /MD /Gt0 /QMOb2000 /W3 /GX /Zi /YX /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /c
# ADD CPP /nologo /MD /Gt0 /QMOb2000 /W3 /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /Yu"stdafx.h" /c
CPP_PROJ=/nologo /MD /Gt0 /QMOb2000 /W3 /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D\
 "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"autoclik.pch"\
 /Yu"stdafx.h" /Fo$(INTDIR)/ /Fd$(OUTDIR)/"autoclik.pdb" /c 
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
RSC_PROJ=/l 0x409 /fo$(INTDIR)/"autoclik.res" /d "_DEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o$(OUTDIR)/"autoclik.bsc" 
BSC32_SBRS= \
	$(INTDIR)/stdafx.sbr \
	$(INTDIR)/autoclik.sbr \
	$(INTDIR)/mainfrm.sbr \
	$(INTDIR)/autocdoc.sbr \
	$(INTDIR)/autocvw.sbr \
	$(INTDIR)/dialogs.sbr

$(OUTDIR)/autoclik.bsc : $(OUTDIR)  $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 /NOLOGO /SUBSYSTEM:windows /DEBUG /MACHINE:MIPS
# ADD LINK32 /NOLOGO /SUBSYSTEM:windows /DEBUG /MACHINE:MIPS
LINK32_FLAGS=/NOLOGO /SUBSYSTEM:windows /PDB:$(OUTDIR)/"autoclik.pdb" /DEBUG\
 /MACHINE:MIPS /OUT:$(OUTDIR)/"autoclik.exe" 
DEF_FILE=
LINK32_OBJS= \
	$(INTDIR)/stdafx.obj \
	$(INTDIR)/autoclik.obj \
	$(INTDIR)/mainfrm.obj \
	$(INTDIR)/autocdoc.obj \
	$(INTDIR)/autocvw.obj \
	$(INTDIR)/autoclik.res \
	$(INTDIR)/dialogs.obj

$(OUTDIR)/autoclik.exe : $(OUTDIR)  $(DEF_FILE) $(LINK32_OBJS)
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

ALL : MTL_TLBS $(OUTDIR)/autoclik.exe $(OUTDIR)/autoclik.bsc

$(OUTDIR) : 
    if not exist $(OUTDIR)/nul mkdir $(OUTDIR)

MTL=MkTypLib.exe
# ADD BASE MTL /nologo /D "NDEBUG" /mips
# ADD MTL /nologo /D "NDEBUG" /mips
MTL_PROJ=/nologo /D "NDEBUG" /mips 

MTL_TLBS : $(OUTDIR)/autoclik.tlb
CPP=cl.exe
# ADD BASE CPP /nologo /MD /Gt0 /QMOb2000 /W3 /GX /YX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /c
# ADD CPP /nologo /MD /Gt0 /QMOb2000 /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /D "_UNICODE" /FR /Yu"stdafx.h" /c
CPP_PROJ=/nologo /MD /Gt0 /QMOb2000 /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D\
 "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /D "_UNICODE" /FR$(INTDIR)/\
 /Fp$(OUTDIR)/"autoclik.pch" /Yu"stdafx.h" /Fo$(INTDIR)/ /c 
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
RSC_PROJ=/l 0x409 /fo$(INTDIR)/"autoclik.res" /d "NDEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o$(OUTDIR)/"autoclik.bsc" 
BSC32_SBRS= \
	$(INTDIR)/stdafx.sbr \
	$(INTDIR)/autoclik.sbr \
	$(INTDIR)/mainfrm.sbr \
	$(INTDIR)/autocdoc.sbr \
	$(INTDIR)/autocvw.sbr \
	$(INTDIR)/dialogs.sbr

$(OUTDIR)/autoclik.bsc : $(OUTDIR)  $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 /NOLOGO /SUBSYSTEM:windows /MACHINE:MIPS
# ADD LINK32 /NOLOGO /ENTRY:"wWinMainCRTStartup" /SUBSYSTEM:windows /MACHINE:MIPS
LINK32_FLAGS=/NOLOGO /ENTRY:"wWinMainCRTStartup" /SUBSYSTEM:windows\
 /PDB:$(OUTDIR)/"autoclik.pdb" /MACHINE:MIPS /OUT:$(OUTDIR)/"autoclik.exe" 
DEF_FILE=
LINK32_OBJS= \
	$(INTDIR)/stdafx.obj \
	$(INTDIR)/autoclik.obj \
	$(INTDIR)/mainfrm.obj \
	$(INTDIR)/autocdoc.obj \
	$(INTDIR)/autocvw.obj \
	$(INTDIR)/autoclik.res \
	$(INTDIR)/dialogs.obj

$(OUTDIR)/autoclik.exe : $(OUTDIR)  $(DEF_FILE) $(LINK32_OBJS)
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

ALL : MTL_TLBS $(OUTDIR)/autoclik.exe $(OUTDIR)/autoclik.bsc

$(OUTDIR) : 
    if not exist $(OUTDIR)/nul mkdir $(OUTDIR)

MTL=MkTypLib.exe
# ADD BASE MTL /nologo /D "_DEBUG" /mips
# ADD MTL /nologo /D "_DEBUG" /mips
MTL_PROJ=/nologo /D "_DEBUG" /mips  

MTL_TLBS : $(OUTDIR)/autoclik.tlb
CPP=cl.exe
# ADD BASE CPP /nologo /MD /Gt0 /QMOb2000 /W3 /GX /Zi /YX /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /c
# ADD CPP /nologo /MD /Gt0 /QMOb2000 /W3 /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /D "_UNICODE" /FR /Yu"stdafx.h" /c
CPP_PROJ=/nologo /MD /Gt0 /QMOb2000 /W3 /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D\
 "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /D "_UNICODE" /FR$(INTDIR)/\
 /Fp$(OUTDIR)/"autoclik.pch" /Yu"stdafx.h" /Fo$(INTDIR)/\
 /Fd$(OUTDIR)/"autoclik.pdb" /c 
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
RSC_PROJ=/l 0x409 /fo$(INTDIR)/"autoclik.res" /d "_DEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o$(OUTDIR)/"autoclik.bsc" 
BSC32_SBRS= \
	$(INTDIR)/stdafx.sbr \
	$(INTDIR)/autoclik.sbr \
	$(INTDIR)/mainfrm.sbr \
	$(INTDIR)/autocdoc.sbr \
	$(INTDIR)/autocvw.sbr \
	$(INTDIR)/dialogs.sbr

$(OUTDIR)/autoclik.bsc : $(OUTDIR)  $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 /NOLOGO /SUBSYSTEM:windows /DEBUG /MACHINE:MIPS
# ADD LINK32 /NOLOGO /ENTRY:"wWinMainCRTStartup" /SUBSYSTEM:windows /DEBUG /MACHINE:MIPS
LINK32_FLAGS=/NOLOGO /ENTRY:"wWinMainCRTStartup" /SUBSYSTEM:windows\
 /PDB:$(OUTDIR)/"autoclik.pdb" /DEBUG /MACHINE:MIPS\
 /OUT:$(OUTDIR)/"autoclik.exe" 
DEF_FILE=
LINK32_OBJS= \
	$(INTDIR)/stdafx.obj \
	$(INTDIR)/autoclik.obj \
	$(INTDIR)/mainfrm.obj \
	$(INTDIR)/autocdoc.obj \
	$(INTDIR)/autocvw.obj \
	$(INTDIR)/autoclik.res \
	$(INTDIR)/dialogs.obj

$(OUTDIR)/autoclik.exe : $(OUTDIR)  $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 

################################################################################
# Begin Group "Source Files"

################################################################################
# Begin Source File

SOURCE=.\stdafx.cpp
DEP_STDAF=\
	.\stdafx.h

!IF  "$(CFG)" == "Win32 (80x86) Debug"

# ADD BASE CPP /Yc"stdafx.h"
# ADD CPP /Yc"stdafx.h"

$(INTDIR)/stdafx.obj :  $(SOURCE)  $(DEP_STDAF) $(INTDIR)
   $(CPP) /nologo /MD /W3 /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"autoclik.pch" /Yc"stdafx.h"\
 /Fo$(INTDIR)/ /Fd$(OUTDIR)/"autoclik.pdb" /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (80x86) Release"

# ADD BASE CPP /Yc"stdafx.h"
# ADD CPP /Yc"stdafx.h"

$(INTDIR)/stdafx.obj :  $(SOURCE)  $(DEP_STDAF) $(INTDIR)
   $(CPP) /nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"autoclik.pch" /Yc"stdafx.h"\
 /Fo$(INTDIR)/ /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (80x86) Unicode Debug"

# ADD BASE CPP /Yc"stdafx.h"
# ADD CPP /Yc"stdafx.h"

$(INTDIR)/stdafx.obj :  $(SOURCE)  $(DEP_STDAF) $(INTDIR)
   $(CPP) /nologo /MD /W3 /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /D "_UNICODE" /FR$(INTDIR)/ /Fp$(OUTDIR)/"autoclik.pch"\
 /Yc"stdafx.h" /Fo$(INTDIR)/ /Fd$(OUTDIR)/"autoclik.pdb" /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (80x86) Unicode Release"

# ADD BASE CPP /Yc"stdafx.h"
# ADD CPP /Yc"stdafx.h"

$(INTDIR)/stdafx.obj :  $(SOURCE)  $(DEP_STDAF) $(INTDIR)
   $(CPP) /nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /D "_UNICODE" /FR$(INTDIR)/ /Fp$(OUTDIR)/"autoclik.pch"\
 /Yc"stdafx.h" /Fo$(INTDIR)/ /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

# ADD BASE CPP /Gt0 /QMOb2000
# ADD CPP /Gt0 /QMOb2000 /Yc"stdafx.h"

$(INTDIR)/stdafx.obj :  $(SOURCE)  $(DEP_STDAF) $(INTDIR)
   $(CPP) /nologo /MD /Gt0 /QMOb2000 /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D\
 "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"autoclik.pch"\
 /Yc"stdafx.h" /Fo$(INTDIR)/ /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

# ADD BASE CPP /Gt0 /QMOb2000
# ADD CPP /Gt0 /QMOb2000 /Yc"stdafx.h"

$(INTDIR)/stdafx.obj :  $(SOURCE)  $(DEP_STDAF) $(INTDIR)
   $(CPP) /nologo /MD /Gt0 /QMOb2000 /W3 /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D\
 "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"autoclik.pch"\
 /Yc"stdafx.h" /Fo$(INTDIR)/ /Fd$(OUTDIR)/"autoclik.pdb" /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Unicode Release"

# ADD BASE CPP /Gt0 /QMOb2000
# ADD CPP /Gt0 /QMOb2000 /Yc"stdafx.h"

$(INTDIR)/stdafx.obj :  $(SOURCE)  $(DEP_STDAF) $(INTDIR)
   $(CPP) /nologo /MD /Gt0 /QMOb2000 /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D\
 "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /D "_UNICODE" /FR$(INTDIR)/\
 /Fp$(OUTDIR)/"autoclik.pch" /Yc"stdafx.h" /Fo$(INTDIR)/ /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Unicode Debug"

# ADD BASE CPP /Gt0 /QMOb2000
# ADD CPP /Gt0 /QMOb2000 /Yc"stdafx.h"

$(INTDIR)/stdafx.obj :  $(SOURCE)  $(DEP_STDAF) $(INTDIR)
   $(CPP) /nologo /MD /Gt0 /QMOb2000 /W3 /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D\
 "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /D "_UNICODE" /FR$(INTDIR)/\
 /Fp$(OUTDIR)/"autoclik.pch" /Yc"stdafx.h" /Fo$(INTDIR)/\
 /Fd$(OUTDIR)/"autoclik.pdb" /c  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\autoclik.cpp
DEP_AUTOC=\
	.\mainfrm.h\
	.\autocdoc.h\
	.\autocvw.h\
	.\autoclik.h

!IF  "$(CFG)" == "Win32 (80x86) Debug"

$(INTDIR)/autoclik.obj :  $(SOURCE)  $(DEP_AUTOC) $(INTDIR)\
 $(INTDIR)/stdafx.obj
   $(CPP) /nologo /MD /W3 /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"autoclik.pch" /Yu"stdafx.h"\
 /Fo$(INTDIR)/ /Fd$(OUTDIR)/"autoclik.pdb" /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (80x86) Release"

$(INTDIR)/autoclik.obj :  $(SOURCE)  $(DEP_AUTOC) $(INTDIR)\
 $(INTDIR)/stdafx.obj
   $(CPP) /nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"autoclik.pch" /Yu"stdafx.h"\
 /Fo$(INTDIR)/ /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (80x86) Unicode Debug"

$(INTDIR)/autoclik.obj :  $(SOURCE)  $(DEP_AUTOC) $(INTDIR)\
 $(INTDIR)/stdafx.obj
   $(CPP) /nologo /MD /W3 /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /D "_UNICODE" /FR$(INTDIR)/ /Fp$(OUTDIR)/"autoclik.pch"\
 /Yu"stdafx.h" /Fo$(INTDIR)/ /Fd$(OUTDIR)/"autoclik.pdb" /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (80x86) Unicode Release"

$(INTDIR)/autoclik.obj :  $(SOURCE)  $(DEP_AUTOC) $(INTDIR)\
 $(INTDIR)/stdafx.obj
   $(CPP) /nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /D "_UNICODE" /FR$(INTDIR)/ /Fp$(OUTDIR)/"autoclik.pch"\
 /Yu"stdafx.h" /Fo$(INTDIR)/ /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

# ADD BASE CPP /Gt0 /QMOb2000
# ADD CPP /Gt0 /QMOb2000 /Yu"stdafx.h"

$(INTDIR)/autoclik.obj :  $(SOURCE)  $(DEP_AUTOC) $(INTDIR)\
 $(INTDIR)/stdafx.obj
   $(CPP) /nologo /MD /Gt0 /QMOb2000 /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D\
 "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"autoclik.pch"\
 /Yu"stdafx.h" /Fo$(INTDIR)/ /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

# ADD BASE CPP /Gt0 /QMOb2000
# ADD CPP /Gt0 /QMOb2000 /Yu"stdafx.h"

$(INTDIR)/autoclik.obj :  $(SOURCE)  $(DEP_AUTOC) $(INTDIR)\
 $(INTDIR)/stdafx.obj
   $(CPP) /nologo /MD /Gt0 /QMOb2000 /W3 /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D\
 "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"autoclik.pch"\
 /Yu"stdafx.h" /Fo$(INTDIR)/ /Fd$(OUTDIR)/"autoclik.pdb" /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Unicode Release"

# ADD BASE CPP /Gt0 /QMOb2000
# ADD CPP /Gt0 /QMOb2000 /Yu"stdafx.h"

$(INTDIR)/autoclik.obj :  $(SOURCE)  $(DEP_AUTOC) $(INTDIR)\
 $(INTDIR)/stdafx.obj
   $(CPP) /nologo /MD /Gt0 /QMOb2000 /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D\
 "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /D "_UNICODE" /FR$(INTDIR)/\
 /Fp$(OUTDIR)/"autoclik.pch" /Yu"stdafx.h" /Fo$(INTDIR)/ /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Unicode Debug"

# ADD BASE CPP /Gt0 /QMOb2000
# ADD CPP /Gt0 /QMOb2000 /Yu"stdafx.h"

$(INTDIR)/autoclik.obj :  $(SOURCE)  $(DEP_AUTOC) $(INTDIR)\
 $(INTDIR)/stdafx.obj
   $(CPP) /nologo /MD /Gt0 /QMOb2000 /W3 /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D\
 "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /D "_UNICODE" /FR$(INTDIR)/\
 /Fp$(OUTDIR)/"autoclik.pch" /Yu"stdafx.h" /Fo$(INTDIR)/\
 /Fd$(OUTDIR)/"autoclik.pdb" /c  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\mainfrm.cpp
DEP_MAINF=\
	.\autoclik.h\
	.\mainfrm.h

!IF  "$(CFG)" == "Win32 (80x86) Debug"

$(INTDIR)/mainfrm.obj :  $(SOURCE)  $(DEP_MAINF) $(INTDIR) $(INTDIR)/stdafx.obj
   $(CPP) /nologo /MD /W3 /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"autoclik.pch" /Yu"stdafx.h"\
 /Fo$(INTDIR)/ /Fd$(OUTDIR)/"autoclik.pdb" /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (80x86) Release"

$(INTDIR)/mainfrm.obj :  $(SOURCE)  $(DEP_MAINF) $(INTDIR) $(INTDIR)/stdafx.obj
   $(CPP) /nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"autoclik.pch" /Yu"stdafx.h"\
 /Fo$(INTDIR)/ /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (80x86) Unicode Debug"

$(INTDIR)/mainfrm.obj :  $(SOURCE)  $(DEP_MAINF) $(INTDIR) $(INTDIR)/stdafx.obj
   $(CPP) /nologo /MD /W3 /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /D "_UNICODE" /FR$(INTDIR)/ /Fp$(OUTDIR)/"autoclik.pch"\
 /Yu"stdafx.h" /Fo$(INTDIR)/ /Fd$(OUTDIR)/"autoclik.pdb" /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (80x86) Unicode Release"

$(INTDIR)/mainfrm.obj :  $(SOURCE)  $(DEP_MAINF) $(INTDIR) $(INTDIR)/stdafx.obj
   $(CPP) /nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /D "_UNICODE" /FR$(INTDIR)/ /Fp$(OUTDIR)/"autoclik.pch"\
 /Yu"stdafx.h" /Fo$(INTDIR)/ /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

# ADD BASE CPP /Gt0 /QMOb2000
# ADD CPP /Gt0 /QMOb2000 /Yu"stdafx.h"

$(INTDIR)/mainfrm.obj :  $(SOURCE)  $(DEP_MAINF) $(INTDIR) $(INTDIR)/stdafx.obj
   $(CPP) /nologo /MD /Gt0 /QMOb2000 /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D\
 "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"autoclik.pch"\
 /Yu"stdafx.h" /Fo$(INTDIR)/ /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

# ADD BASE CPP /Gt0 /QMOb2000
# ADD CPP /Gt0 /QMOb2000 /Yu"stdafx.h"

$(INTDIR)/mainfrm.obj :  $(SOURCE)  $(DEP_MAINF) $(INTDIR) $(INTDIR)/stdafx.obj
   $(CPP) /nologo /MD /Gt0 /QMOb2000 /W3 /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D\
 "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"autoclik.pch"\
 /Yu"stdafx.h" /Fo$(INTDIR)/ /Fd$(OUTDIR)/"autoclik.pdb" /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Unicode Release"

# ADD BASE CPP /Gt0 /QMOb2000
# ADD CPP /Gt0 /QMOb2000 /Yu"stdafx.h"

$(INTDIR)/mainfrm.obj :  $(SOURCE)  $(DEP_MAINF) $(INTDIR) $(INTDIR)/stdafx.obj
   $(CPP) /nologo /MD /Gt0 /QMOb2000 /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D\
 "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /D "_UNICODE" /FR$(INTDIR)/\
 /Fp$(OUTDIR)/"autoclik.pch" /Yu"stdafx.h" /Fo$(INTDIR)/ /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Unicode Debug"

# ADD BASE CPP /Gt0 /QMOb2000
# ADD CPP /Gt0 /QMOb2000 /Yu"stdafx.h"

$(INTDIR)/mainfrm.obj :  $(SOURCE)  $(DEP_MAINF) $(INTDIR) $(INTDIR)/stdafx.obj
   $(CPP) /nologo /MD /Gt0 /QMOb2000 /W3 /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D\
 "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /D "_UNICODE" /FR$(INTDIR)/\
 /Fp$(OUTDIR)/"autoclik.pch" /Yu"stdafx.h" /Fo$(INTDIR)/\
 /Fd$(OUTDIR)/"autoclik.pdb" /c  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\autocdoc.cpp
DEP_AUTOCD=\
	.\autoclik.h\
	.\autocdoc.h

!IF  "$(CFG)" == "Win32 (80x86) Debug"

$(INTDIR)/autocdoc.obj :  $(SOURCE)  $(DEP_AUTOCD) $(INTDIR)\
 $(INTDIR)/stdafx.obj
   $(CPP) /nologo /MD /W3 /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"autoclik.pch" /Yu"stdafx.h"\
 /Fo$(INTDIR)/ /Fd$(OUTDIR)/"autoclik.pdb" /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (80x86) Release"

$(INTDIR)/autocdoc.obj :  $(SOURCE)  $(DEP_AUTOCD) $(INTDIR)\
 $(INTDIR)/stdafx.obj
   $(CPP) /nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"autoclik.pch" /Yu"stdafx.h"\
 /Fo$(INTDIR)/ /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (80x86) Unicode Debug"

$(INTDIR)/autocdoc.obj :  $(SOURCE)  $(DEP_AUTOCD) $(INTDIR)\
 $(INTDIR)/stdafx.obj
   $(CPP) /nologo /MD /W3 /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /D "_UNICODE" /FR$(INTDIR)/ /Fp$(OUTDIR)/"autoclik.pch"\
 /Yu"stdafx.h" /Fo$(INTDIR)/ /Fd$(OUTDIR)/"autoclik.pdb" /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (80x86) Unicode Release"

$(INTDIR)/autocdoc.obj :  $(SOURCE)  $(DEP_AUTOCD) $(INTDIR)\
 $(INTDIR)/stdafx.obj
   $(CPP) /nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /D "_UNICODE" /FR$(INTDIR)/ /Fp$(OUTDIR)/"autoclik.pch"\
 /Yu"stdafx.h" /Fo$(INTDIR)/ /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

# ADD BASE CPP /Gt0 /QMOb2000
# ADD CPP /Gt0 /QMOb2000 /Yu"stdafx.h"

$(INTDIR)/autocdoc.obj :  $(SOURCE)  $(DEP_AUTOCD) $(INTDIR)\
 $(INTDIR)/stdafx.obj
   $(CPP) /nologo /MD /Gt0 /QMOb2000 /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D\
 "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"autoclik.pch"\
 /Yu"stdafx.h" /Fo$(INTDIR)/ /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

# ADD BASE CPP /Gt0 /QMOb2000
# ADD CPP /Gt0 /QMOb2000 /Yu"stdafx.h"

$(INTDIR)/autocdoc.obj :  $(SOURCE)  $(DEP_AUTOCD) $(INTDIR)\
 $(INTDIR)/stdafx.obj
   $(CPP) /nologo /MD /Gt0 /QMOb2000 /W3 /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D\
 "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"autoclik.pch"\
 /Yu"stdafx.h" /Fo$(INTDIR)/ /Fd$(OUTDIR)/"autoclik.pdb" /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Unicode Release"

# ADD BASE CPP /Gt0 /QMOb2000
# ADD CPP /Gt0 /QMOb2000 /Yu"stdafx.h"

$(INTDIR)/autocdoc.obj :  $(SOURCE)  $(DEP_AUTOCD) $(INTDIR)\
 $(INTDIR)/stdafx.obj
   $(CPP) /nologo /MD /Gt0 /QMOb2000 /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D\
 "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /D "_UNICODE" /FR$(INTDIR)/\
 /Fp$(OUTDIR)/"autoclik.pch" /Yu"stdafx.h" /Fo$(INTDIR)/ /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Unicode Debug"

# ADD BASE CPP /Gt0 /QMOb2000
# ADD CPP /Gt0 /QMOb2000 /Yu"stdafx.h"

$(INTDIR)/autocdoc.obj :  $(SOURCE)  $(DEP_AUTOCD) $(INTDIR)\
 $(INTDIR)/stdafx.obj
   $(CPP) /nologo /MD /Gt0 /QMOb2000 /W3 /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D\
 "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /D "_UNICODE" /FR$(INTDIR)/\
 /Fp$(OUTDIR)/"autoclik.pch" /Yu"stdafx.h" /Fo$(INTDIR)/\
 /Fd$(OUTDIR)/"autoclik.pdb" /c  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\autocvw.cpp
DEP_AUTOCV=\
	.\autoclik.h\
	.\autocdoc.h\
	.\autocvw.h

!IF  "$(CFG)" == "Win32 (80x86) Debug"

$(INTDIR)/autocvw.obj :  $(SOURCE)  $(DEP_AUTOCV) $(INTDIR)\
 $(INTDIR)/stdafx.obj
   $(CPP) /nologo /MD /W3 /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"autoclik.pch" /Yu"stdafx.h"\
 /Fo$(INTDIR)/ /Fd$(OUTDIR)/"autoclik.pdb" /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (80x86) Release"

$(INTDIR)/autocvw.obj :  $(SOURCE)  $(DEP_AUTOCV) $(INTDIR)\
 $(INTDIR)/stdafx.obj
   $(CPP) /nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"autoclik.pch" /Yu"stdafx.h"\
 /Fo$(INTDIR)/ /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (80x86) Unicode Debug"

$(INTDIR)/autocvw.obj :  $(SOURCE)  $(DEP_AUTOCV) $(INTDIR)\
 $(INTDIR)/stdafx.obj
   $(CPP) /nologo /MD /W3 /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /D "_UNICODE" /FR$(INTDIR)/ /Fp$(OUTDIR)/"autoclik.pch"\
 /Yu"stdafx.h" /Fo$(INTDIR)/ /Fd$(OUTDIR)/"autoclik.pdb" /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (80x86) Unicode Release"

$(INTDIR)/autocvw.obj :  $(SOURCE)  $(DEP_AUTOCV) $(INTDIR)\
 $(INTDIR)/stdafx.obj
   $(CPP) /nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /D "_UNICODE" /FR$(INTDIR)/ /Fp$(OUTDIR)/"autoclik.pch"\
 /Yu"stdafx.h" /Fo$(INTDIR)/ /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

# ADD BASE CPP /Gt0 /QMOb2000
# ADD CPP /Gt0 /QMOb2000 /Yu"stdafx.h"

$(INTDIR)/autocvw.obj :  $(SOURCE)  $(DEP_AUTOCV) $(INTDIR)\
 $(INTDIR)/stdafx.obj
   $(CPP) /nologo /MD /Gt0 /QMOb2000 /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D\
 "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"autoclik.pch"\
 /Yu"stdafx.h" /Fo$(INTDIR)/ /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

# ADD BASE CPP /Gt0 /QMOb2000
# ADD CPP /Gt0 /QMOb2000 /Yu"stdafx.h"

$(INTDIR)/autocvw.obj :  $(SOURCE)  $(DEP_AUTOCV) $(INTDIR)\
 $(INTDIR)/stdafx.obj
   $(CPP) /nologo /MD /Gt0 /QMOb2000 /W3 /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D\
 "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"autoclik.pch"\
 /Yu"stdafx.h" /Fo$(INTDIR)/ /Fd$(OUTDIR)/"autoclik.pdb" /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Unicode Release"

# ADD BASE CPP /Gt0 /QMOb2000
# ADD CPP /Gt0 /QMOb2000 /Yu"stdafx.h"

$(INTDIR)/autocvw.obj :  $(SOURCE)  $(DEP_AUTOCV) $(INTDIR)\
 $(INTDIR)/stdafx.obj
   $(CPP) /nologo /MD /Gt0 /QMOb2000 /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D\
 "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /D "_UNICODE" /FR$(INTDIR)/\
 /Fp$(OUTDIR)/"autoclik.pch" /Yu"stdafx.h" /Fo$(INTDIR)/ /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Unicode Debug"

# ADD BASE CPP /Gt0 /QMOb2000
# ADD CPP /Gt0 /QMOb2000 /Yu"stdafx.h"

$(INTDIR)/autocvw.obj :  $(SOURCE)  $(DEP_AUTOCV) $(INTDIR)\
 $(INTDIR)/stdafx.obj
   $(CPP) /nologo /MD /Gt0 /QMOb2000 /W3 /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D\
 "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /D "_UNICODE" /FR$(INTDIR)/\
 /Fp$(OUTDIR)/"autoclik.pch" /Yu"stdafx.h" /Fo$(INTDIR)/\
 /Fd$(OUTDIR)/"autoclik.pdb" /c  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\autoclik.rc
DEP_AUTOCL=\
	.\res\autoclik.ico\
	.\res\autocdoc.ico\
	.\res\toolbar.bmp\
	.\res\autoclik.rc2

!IF  "$(CFG)" == "Win32 (80x86) Debug"

$(INTDIR)/autoclik.res :  $(SOURCE)  $(DEP_AUTOCL) $(INTDIR)
   $(RSC) /l 0x409 /fo$(INTDIR)/"autoclik.res" /i "$(OUTDIR)" /d "_DEBUG" /d\
 "_AFXDLL"  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (80x86) Release"

$(INTDIR)/autoclik.res :  $(SOURCE)  $(DEP_AUTOCL) $(INTDIR)
   $(RSC) /l 0x409 /fo$(INTDIR)/"autoclik.res" /i "$(OUTDIR)" /d "NDEBUG" /d\
 "_AFXDLL"  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (80x86) Unicode Debug"

$(INTDIR)/autoclik.res :  $(SOURCE)  $(DEP_AUTOCL) $(INTDIR)
   $(RSC) /l 0x409 /fo$(INTDIR)/"autoclik.res" /i "$(OUTDIR)" /d "_DEBUG" /d\
 "_AFXDLL"  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (80x86) Unicode Release"

$(INTDIR)/autoclik.res :  $(SOURCE)  $(DEP_AUTOCL) $(INTDIR)
   $(RSC) /l 0x409 /fo$(INTDIR)/"autoclik.res" /i "$(OUTDIR)" /d "NDEBUG" /d\
 "_AFXDLL"  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

$(INTDIR)/autoclik.res :  $(SOURCE)  $(DEP_AUTOCL) $(INTDIR)
   $(RSC) /l 0x409 /fo$(INTDIR)/"autoclik.res" /i "$(OUTDIR)" /d "NDEBUG" /d\
 "_AFXDLL"  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

$(INTDIR)/autoclik.res :  $(SOURCE)  $(DEP_AUTOCL) $(INTDIR)
   $(RSC) /l 0x409 /fo$(INTDIR)/"autoclik.res" /i "$(OUTDIR)" /d "_DEBUG" /d\
 "_AFXDLL"  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Unicode Release"

$(INTDIR)/autoclik.res :  $(SOURCE)  $(DEP_AUTOCL) $(INTDIR)
   $(RSC) /l 0x409 /fo$(INTDIR)/"autoclik.res" /i "$(OUTDIR)" /d "NDEBUG" /d\
 "_AFXDLL"  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Unicode Debug"

$(INTDIR)/autoclik.res :  $(SOURCE)  $(DEP_AUTOCL) $(INTDIR)
   $(RSC) /l 0x409 /fo$(INTDIR)/"autoclik.res" /i "$(OUTDIR)" /d "_DEBUG" /d\
 "_AFXDLL"  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\readme.txt
# End Source File
################################################################################
# Begin Source File

SOURCE=.\autoclik.odl

!IF  "$(CFG)" == "Win32 (80x86) Debug"

$(OUTDIR)/autoclik.tlb :  $(SOURCE)  $(OUTDIR)
   $(MTL) /nologo /D "_DEBUG" /tlb $(OUTDIR)/"autoclik.tlb" /win32  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (80x86) Release"

$(OUTDIR)/autoclik.tlb :  $(SOURCE)  $(OUTDIR)
   $(MTL) /nologo /D "NDEBUG" /tlb $(OUTDIR)/"autoclik.tlb" /win32  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (80x86) Unicode Debug"

$(OUTDIR)/autoclik.tlb :  $(SOURCE)  $(OUTDIR)
   $(MTL) /nologo /D "_DEBUG" /tlb $(OUTDIR)/"autoclik.tlb" /win32  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (80x86) Unicode Release"

$(OUTDIR)/autoclik.tlb :  $(SOURCE)  $(OUTDIR)
   $(MTL) /nologo /D "NDEBUG" /tlb $(OUTDIR)/"autoclik.tlb" /win32  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

$(OUTDIR)/autoclik.tlb :  $(SOURCE)  $(OUTDIR)
   $(MTL) /nologo /D "NDEBUG" /tlb $(OUTDIR)/"autoclik.tlb" /mips  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

$(OUTDIR)/autoclik.tlb :  $(SOURCE)  $(OUTDIR)
   $(MTL) /nologo /D "_DEBUG" /tlb $(OUTDIR)/"autoclik.tlb" /mips  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Unicode Release"

$(OUTDIR)/autoclik.tlb :  $(SOURCE)  $(OUTDIR)
   $(MTL) /nologo /D "NDEBUG" /tlb $(OUTDIR)/"autoclik.tlb" /mips  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Unicode Debug"

$(OUTDIR)/autoclik.tlb :  $(SOURCE)  $(OUTDIR)
   $(MTL) /nologo /D "_DEBUG" /tlb $(OUTDIR)/"autoclik.tlb" /mips  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\dialogs.cpp
DEP_DIALO=\
	.\stdafx.h\
	.\autoclik.h\
	.\dialogs.h

!IF  "$(CFG)" == "Win32 (80x86) Debug"

$(INTDIR)/dialogs.obj :  $(SOURCE)  $(DEP_DIALO) $(INTDIR) $(INTDIR)/stdafx.obj
   $(CPP) /nologo /MD /W3 /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"autoclik.pch" /Yu"stdafx.h"\
 /Fo$(INTDIR)/ /Fd$(OUTDIR)/"autoclik.pdb" /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (80x86) Release"

$(INTDIR)/dialogs.obj :  $(SOURCE)  $(DEP_DIALO) $(INTDIR) $(INTDIR)/stdafx.obj
   $(CPP) /nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"autoclik.pch" /Yu"stdafx.h"\
 /Fo$(INTDIR)/ /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (80x86) Unicode Debug"

$(INTDIR)/dialogs.obj :  $(SOURCE)  $(DEP_DIALO) $(INTDIR) $(INTDIR)/stdafx.obj
   $(CPP) /nologo /MD /W3 /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /D "_UNICODE" /FR$(INTDIR)/ /Fp$(OUTDIR)/"autoclik.pch"\
 /Yu"stdafx.h" /Fo$(INTDIR)/ /Fd$(OUTDIR)/"autoclik.pdb" /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (80x86) Unicode Release"

$(INTDIR)/dialogs.obj :  $(SOURCE)  $(DEP_DIALO) $(INTDIR) $(INTDIR)/stdafx.obj
   $(CPP) /nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /D "_UNICODE" /FR$(INTDIR)/ /Fp$(OUTDIR)/"autoclik.pch"\
 /Yu"stdafx.h" /Fo$(INTDIR)/ /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

# ADD BASE CPP /Gt0 /QMOb2000
# ADD CPP /Gt0 /QMOb2000 /Yu"stdafx.h"

$(INTDIR)/dialogs.obj :  $(SOURCE)  $(DEP_DIALO) $(INTDIR) $(INTDIR)/stdafx.obj
   $(CPP) /nologo /MD /Gt0 /QMOb2000 /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D\
 "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"autoclik.pch"\
 /Yu"stdafx.h" /Fo$(INTDIR)/ /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

# ADD BASE CPP /Gt0 /QMOb2000
# ADD CPP /Gt0 /QMOb2000 /Yu"stdafx.h"

$(INTDIR)/dialogs.obj :  $(SOURCE)  $(DEP_DIALO) $(INTDIR) $(INTDIR)/stdafx.obj
   $(CPP) /nologo /MD /Gt0 /QMOb2000 /W3 /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D\
 "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"autoclik.pch"\
 /Yu"stdafx.h" /Fo$(INTDIR)/ /Fd$(OUTDIR)/"autoclik.pdb" /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Unicode Release"

# ADD BASE CPP /Gt0 /QMOb2000
# ADD CPP /Gt0 /QMOb2000 /Yu"stdafx.h"

$(INTDIR)/dialogs.obj :  $(SOURCE)  $(DEP_DIALO) $(INTDIR) $(INTDIR)/stdafx.obj
   $(CPP) /nologo /MD /Gt0 /QMOb2000 /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D\
 "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /D "_UNICODE" /FR$(INTDIR)/\
 /Fp$(OUTDIR)/"autoclik.pch" /Yu"stdafx.h" /Fo$(INTDIR)/ /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Unicode Debug"

# ADD BASE CPP /Gt0 /QMOb2000
# ADD CPP /Gt0 /QMOb2000 /Yu"stdafx.h"

$(INTDIR)/dialogs.obj :  $(SOURCE)  $(DEP_DIALO) $(INTDIR) $(INTDIR)/stdafx.obj
   $(CPP) /nologo /MD /Gt0 /QMOb2000 /W3 /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D\
 "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /D "_UNICODE" /FR$(INTDIR)/\
 /Fp$(OUTDIR)/"autoclik.pch" /Yu"stdafx.h" /Fo$(INTDIR)/\
 /Fd$(OUTDIR)/"autoclik.pdb" /c  $(SOURCE) 

!ENDIF 

# End Source File
# End Group
# End Project
################################################################################
