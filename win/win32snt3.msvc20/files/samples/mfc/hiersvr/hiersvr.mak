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
!MESSAGE NMAKE /f "hiersvr.mak" CFG="Win32 (80x86) Debug"
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

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "WinRel"
# PROP BASE Intermediate_Dir "WinRel"
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "WinRel"
# PROP Intermediate_Dir "WinRel"
OUTDIR=.\WinRel
INTDIR=.\WinRel

ALL : $(OUTDIR)/hiersvr.exe $(OUTDIR)/hiersvr.bsc

$(OUTDIR) : 
    if not exist $(OUTDIR)/nul mkdir $(OUTDIR)

MTL=MkTypLib.exe
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
CPP=cl.exe
# ADD BASE CPP /nologo /MD /W3 /GX /YX /O2 /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /Yu"stdafx.h" /c
CPP_PROJ=/nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D\
 "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"hiersvr.pch" /Yu"stdafx.h" /Fo$(INTDIR)/ /c\
 
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
RSC_PROJ=/l 0x409 /fo$(INTDIR)/"hiersvr.res" /d "NDEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# SUBTRACT BASE BSC32 /Iu
# ADD BSC32 /nologo
# SUBTRACT BSC32 /Iu
BSC32_FLAGS=/nologo /o$(OUTDIR)/"hiersvr.bsc" 
BSC32_SBRS= \
	$(INTDIR)/svrview.sbr \
	$(INTDIR)/hiersvr.sbr \
	$(INTDIR)/svrdoc.sbr \
	$(INTDIR)/svritem.sbr \
	$(INTDIR)/stdafx.sbr \
	$(INTDIR)/mainfrm.sbr \
	$(INTDIR)/zoomdlg.sbr \
	$(INTDIR)/ipframe.sbr

$(OUTDIR)/hiersvr.bsc : $(OUTDIR)  $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib mfc30.lib mfco30.lib mfcd30.lib mfcuia32.lib mfcans32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib /NOLOGO /SUBSYSTEM:windows /MACHINE:I386
# ADD LINK32 /NOLOGO /SUBSYSTEM:windows /MACHINE:I386
LINK32_FLAGS=/NOLOGO /SUBSYSTEM:windows /INCREMENTAL:no\
 /PDB:$(OUTDIR)/"hiersvr.pdb" /MACHINE:I386 /OUT:$(OUTDIR)/"hiersvr.exe" 
DEF_FILE=
LINK32_OBJS= \
	$(INTDIR)/svrview.obj \
	$(INTDIR)/hiersvr.obj \
	$(INTDIR)/svrdoc.obj \
	$(INTDIR)/hiersvr.res \
	$(INTDIR)/svritem.obj \
	$(INTDIR)/stdafx.obj \
	$(INTDIR)/mainfrm.obj \
	$(INTDIR)/zoomdlg.obj \
	$(INTDIR)/ipframe.obj

$(OUTDIR)/hiersvr.exe : $(OUTDIR)  $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "WinDebug"
# PROP BASE Intermediate_Dir "WinDebug"
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "WinDebug"
# PROP Intermediate_Dir "WinDebug"
OUTDIR=.\WinDebug
INTDIR=.\WinDebug

ALL : $(OUTDIR)/hiersvr.exe $(OUTDIR)/hiersvr.bsc

$(OUTDIR) : 
    if not exist $(OUTDIR)/nul mkdir $(OUTDIR)

MTL=MkTypLib.exe
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
CPP=cl.exe
# ADD BASE CPP /nologo /MD /W3 /GX /Zi /YX /Od /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /c
# ADD CPP /nologo /MD /W3 /GX /Zi /Od /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /Yu"stdafx.h" /c
CPP_PROJ=/nologo /MD /W3 /GX /Zi /Od /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D\
 "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"hiersvr.pch" /Yu"stdafx.h" /Fo$(INTDIR)/\
 /Fd$(OUTDIR)/"hiersvr.pdb" /c 
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
RSC_PROJ=/l 0x409 /fo$(INTDIR)/"hiersvr.res" /d "_DEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# SUBTRACT BASE BSC32 /Iu
# ADD BSC32 /nologo
# SUBTRACT BSC32 /Iu
BSC32_FLAGS=/nologo /o$(OUTDIR)/"hiersvr.bsc" 
BSC32_SBRS= \
	$(INTDIR)/svrview.sbr \
	$(INTDIR)/hiersvr.sbr \
	$(INTDIR)/svrdoc.sbr \
	$(INTDIR)/svritem.sbr \
	$(INTDIR)/stdafx.sbr \
	$(INTDIR)/mainfrm.sbr \
	$(INTDIR)/zoomdlg.sbr \
	$(INTDIR)/ipframe.sbr

$(OUTDIR)/hiersvr.bsc : $(OUTDIR)  $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib mfc30d.lib mfco30d.lib mfcd30d.lib mfcuia32.lib mfcans32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib /NOLOGO /SUBSYSTEM:windows /DEBUG /MACHINE:I386
# ADD LINK32 /NOLOGO /SUBSYSTEM:windows /DEBUG /MACHINE:I386
LINK32_FLAGS=/NOLOGO /SUBSYSTEM:windows /INCREMENTAL:yes\
 /PDB:$(OUTDIR)/"hiersvr.pdb" /DEBUG /MACHINE:I386 /OUT:$(OUTDIR)/"hiersvr.exe" 
DEF_FILE=
LINK32_OBJS= \
	$(INTDIR)/svrview.obj \
	$(INTDIR)/hiersvr.obj \
	$(INTDIR)/svrdoc.obj \
	$(INTDIR)/hiersvr.res \
	$(INTDIR)/svritem.obj \
	$(INTDIR)/stdafx.obj \
	$(INTDIR)/mainfrm.obj \
	$(INTDIR)/zoomdlg.obj \
	$(INTDIR)/ipframe.obj

$(OUTDIR)/hiersvr.exe : $(OUTDIR)  $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "Win32 (80x86) Unicode Debug"

# PROP BASE Use_MFC 1
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Win32_Un"
# PROP BASE Intermediate_Dir "Win32_Un"
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "UniDebug"
# PROP Intermediate_Dir "UniDebug"
OUTDIR=.\UniDebug
INTDIR=.\UniDebug

ALL : $(OUTDIR)/hiersvr.exe $(OUTDIR)/hiersvr.bsc

$(OUTDIR) : 
    if not exist $(OUTDIR)/nul mkdir $(OUTDIR)

MTL=MkTypLib.exe
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
CPP=cl.exe
# ADD BASE CPP /nologo /MT /W3 /GX /Zi /YX /Od /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /FR /c
# ADD CPP /nologo /MD /W3 /GX /Zi /Od /D "_DEBUG" /D "_WINDOWS" /D "_UNICODE" /D "_AFXDLL" /D "_MBCS" /FR /Yu"stdafx.h" /c
CPP_PROJ=/nologo /MD /W3 /GX /Zi /Od /D "_DEBUG" /D "_WINDOWS" /D "_UNICODE" /D\
 "_AFXDLL" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"hiersvr.pch" /Yu"stdafx.h"\
 /Fo$(INTDIR)/ /Fd$(OUTDIR)/"hiersvr.pdb" /c 
CPP_OBJS=.\UniDebug/

.c{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

RSC=rc.exe
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /fo$(INTDIR)/"hiersvr.res" /d "_DEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# SUBTRACT BASE BSC32 /Iu
# ADD BSC32 /nologo
# SUBTRACT BSC32 /Iu
BSC32_FLAGS=/nologo /o$(OUTDIR)/"hiersvr.bsc" 
BSC32_SBRS= \
	$(INTDIR)/svrview.sbr \
	$(INTDIR)/hiersvr.sbr \
	$(INTDIR)/svrdoc.sbr \
	$(INTDIR)/svritem.sbr \
	$(INTDIR)/stdafx.sbr \
	$(INTDIR)/mainfrm.sbr \
	$(INTDIR)/zoomdlg.sbr \
	$(INTDIR)/ipframe.sbr

$(OUTDIR)/hiersvr.bsc : $(OUTDIR)  $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib nafxcwd.lib mfcuia32.lib mfcans32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib /NOLOGO /SUBSYSTEM:windows /DEBUG /MACHINE:I386
# ADD LINK32 /NOLOGO /ENTRY:"wWinMainCRTStartup" /SUBSYSTEM:windows /DEBUG /MACHINE:I386
LINK32_FLAGS=/NOLOGO /ENTRY:"wWinMainCRTStartup" /SUBSYSTEM:windows\
 /INCREMENTAL:yes /PDB:$(OUTDIR)/"hiersvr.pdb" /DEBUG /MACHINE:I386\
 /OUT:$(OUTDIR)/"hiersvr.exe" 
DEF_FILE=
LINK32_OBJS= \
	$(INTDIR)/svrview.obj \
	$(INTDIR)/hiersvr.obj \
	$(INTDIR)/svrdoc.obj \
	$(INTDIR)/hiersvr.res \
	$(INTDIR)/svritem.obj \
	$(INTDIR)/stdafx.obj \
	$(INTDIR)/mainfrm.obj \
	$(INTDIR)/zoomdlg.obj \
	$(INTDIR)/ipframe.obj

$(OUTDIR)/hiersvr.exe : $(OUTDIR)  $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "Win32 (80x86) Unicode Release"

# PROP BASE Use_MFC 1
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Win32_Un"
# PROP BASE Intermediate_Dir "Win32_Un"
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "UniRel"
# PROP Intermediate_Dir "UniRel"
OUTDIR=.\UniRel
INTDIR=.\UniRel

ALL : $(OUTDIR)/hiersvr.exe $(OUTDIR)/hiersvr.bsc

$(OUTDIR) : 
    if not exist $(OUTDIR)/nul mkdir $(OUTDIR)

MTL=MkTypLib.exe
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
CPP=cl.exe
# ADD BASE CPP /nologo /MT /W3 /GX /YX /O2 /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /FR /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "_WINDOWS" /D "_UNICODE" /D "_AFXDLL" /D "_MBCS" /FR /Yu"stdafx.h" /c
CPP_PROJ=/nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "_WINDOWS" /D "_UNICODE" /D\
 "_AFXDLL" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"hiersvr.pch" /Yu"stdafx.h"\
 /Fo$(INTDIR)/ /c 
CPP_OBJS=.\UniRel/

.c{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

RSC=rc.exe
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /fo$(INTDIR)/"hiersvr.res" /d "NDEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# SUBTRACT BASE BSC32 /Iu
# ADD BSC32 /nologo
# SUBTRACT BSC32 /Iu
BSC32_FLAGS=/nologo /o$(OUTDIR)/"hiersvr.bsc" 
BSC32_SBRS= \
	$(INTDIR)/svrview.sbr \
	$(INTDIR)/hiersvr.sbr \
	$(INTDIR)/svrdoc.sbr \
	$(INTDIR)/svritem.sbr \
	$(INTDIR)/stdafx.sbr \
	$(INTDIR)/mainfrm.sbr \
	$(INTDIR)/zoomdlg.sbr \
	$(INTDIR)/ipframe.sbr

$(OUTDIR)/hiersvr.bsc : $(OUTDIR)  $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib nafxcw.lib mfcuia32.lib mfcans32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib /NOLOGO /SUBSYSTEM:windows /MACHINE:I386
# ADD LINK32 /NOLOGO /ENTRY:"wWinMainCRTStartup" /SUBSYSTEM:windows /MACHINE:I386
LINK32_FLAGS=/NOLOGO /ENTRY:"wWinMainCRTStartup" /SUBSYSTEM:windows\
 /INCREMENTAL:no /PDB:$(OUTDIR)/"hiersvr.pdb" /MACHINE:I386\
 /OUT:$(OUTDIR)/"hiersvr.exe" 
DEF_FILE=
LINK32_OBJS= \
	$(INTDIR)/svrview.obj \
	$(INTDIR)/hiersvr.obj \
	$(INTDIR)/svrdoc.obj \
	$(INTDIR)/hiersvr.res \
	$(INTDIR)/svritem.obj \
	$(INTDIR)/stdafx.obj \
	$(INTDIR)/mainfrm.obj \
	$(INTDIR)/zoomdlg.obj \
	$(INTDIR)/ipframe.obj

$(OUTDIR)/hiersvr.exe : $(OUTDIR)  $(DEF_FILE) $(LINK32_OBJS)
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

ALL : $(OUTDIR)/hiersvr.exe $(OUTDIR)/hiersvr.bsc

$(OUTDIR) : 
    if not exist $(OUTDIR)/nul mkdir $(OUTDIR)

MTL=MkTypLib.exe
# ADD BASE MTL /nologo /D "NDEBUG" /mips
# ADD MTL /nologo /D "NDEBUG" /mips
MTL_PROJ=/nologo /D "NDEBUG" /mips 
CPP=cl.exe
# ADD BASE CPP /nologo /MD /Gt0 /QMOb2000 /W3 /GX /YX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /c
# ADD CPP /nologo /MD /Gt0 /QMOb2000 /W3 /GX /O2 /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /Yu"stdafx.h" /c
CPP_PROJ=/nologo /MD /Gt0 /QMOb2000 /W3 /GX /O2 /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"hiersvr.pch" /Yu"stdafx.h"\
 /Fo$(INTDIR)/ /c 
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
RSC_PROJ=/l 0x409 /fo$(INTDIR)/"hiersvr.res" /d "NDEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o$(OUTDIR)/"hiersvr.bsc" 
BSC32_SBRS= \
	$(INTDIR)/svrview.sbr \
	$(INTDIR)/hiersvr.sbr \
	$(INTDIR)/svrdoc.sbr \
	$(INTDIR)/svritem.sbr \
	$(INTDIR)/stdafx.sbr \
	$(INTDIR)/mainfrm.sbr \
	$(INTDIR)/zoomdlg.sbr \
	$(INTDIR)/ipframe.sbr

$(OUTDIR)/hiersvr.bsc : $(OUTDIR)  $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 /NOLOGO /SUBSYSTEM:windows /MACHINE:MIPS
# ADD LINK32 /NOLOGO /SUBSYSTEM:windows /MACHINE:MIPS
LINK32_FLAGS=/NOLOGO /SUBSYSTEM:windows /PDB:$(OUTDIR)/"hiersvr.pdb"\
 /MACHINE:MIPS /OUT:$(OUTDIR)/"hiersvr.exe" 
DEF_FILE=
LINK32_OBJS= \
	$(INTDIR)/svrview.obj \
	$(INTDIR)/hiersvr.obj \
	$(INTDIR)/svrdoc.obj \
	$(INTDIR)/hiersvr.res \
	$(INTDIR)/svritem.obj \
	$(INTDIR)/stdafx.obj \
	$(INTDIR)/mainfrm.obj \
	$(INTDIR)/zoomdlg.obj \
	$(INTDIR)/ipframe.obj

$(OUTDIR)/hiersvr.exe : $(OUTDIR)  $(DEF_FILE) $(LINK32_OBJS)
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

ALL : $(OUTDIR)/hiersvr.exe $(OUTDIR)/hiersvr.bsc

$(OUTDIR) : 
    if not exist $(OUTDIR)/nul mkdir $(OUTDIR)

MTL=MkTypLib.exe
# ADD BASE MTL /nologo /D "_DEBUG" /mips
# ADD MTL /nologo /D "_DEBUG" /mips
MTL_PROJ=/nologo /D "_DEBUG" /mips 
CPP=cl.exe
# ADD BASE CPP /nologo /MD /Gt0 /QMOb2000 /W3 /GX /Zi /YX /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /c
# ADD CPP /nologo /MD /Gt0 /QMOb2000 /W3 /GX /Zi /Od /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /Yu"stdafx.h" /c
CPP_PROJ=/nologo /MD /Gt0 /QMOb2000 /W3 /GX /Zi /Od /D "_DEBUG" /D "_WINDOWS"\
 /D "_AFXDLL" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"hiersvr.pch" /Yu"stdafx.h"\
 /Fo$(INTDIR)/ /Fd$(OUTDIR)/"hiersvr.pdb" /c 
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
RSC_PROJ=/l 0x409 /fo$(INTDIR)/"hiersvr.res" /d "_DEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o$(OUTDIR)/"hiersvr.bsc" 
BSC32_SBRS= \
	$(INTDIR)/svrview.sbr \
	$(INTDIR)/hiersvr.sbr \
	$(INTDIR)/svrdoc.sbr \
	$(INTDIR)/svritem.sbr \
	$(INTDIR)/stdafx.sbr \
	$(INTDIR)/mainfrm.sbr \
	$(INTDIR)/zoomdlg.sbr \
	$(INTDIR)/ipframe.sbr

$(OUTDIR)/hiersvr.bsc : $(OUTDIR)  $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 /NOLOGO /SUBSYSTEM:windows /DEBUG /MACHINE:MIPS
# ADD LINK32 /NOLOGO /SUBSYSTEM:windows /DEBUG /MACHINE:MIPS
LINK32_FLAGS=/NOLOGO /SUBSYSTEM:windows /PDB:$(OUTDIR)/"hiersvr.pdb" /DEBUG\
 /MACHINE:MIPS /OUT:$(OUTDIR)/"hiersvr.exe" 
DEF_FILE=
LINK32_OBJS= \
	$(INTDIR)/svrview.obj \
	$(INTDIR)/hiersvr.obj \
	$(INTDIR)/svrdoc.obj \
	$(INTDIR)/hiersvr.res \
	$(INTDIR)/svritem.obj \
	$(INTDIR)/stdafx.obj \
	$(INTDIR)/mainfrm.obj \
	$(INTDIR)/zoomdlg.obj \
	$(INTDIR)/ipframe.obj

$(OUTDIR)/hiersvr.exe : $(OUTDIR)  $(DEF_FILE) $(LINK32_OBJS)
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

ALL : $(OUTDIR)/hiersvr.exe $(OUTDIR)/hiersvr.bsc

$(OUTDIR) : 
    if not exist $(OUTDIR)/nul mkdir $(OUTDIR)

MTL=MkTypLib.exe
# ADD BASE MTL /nologo /D "NDEBUG" /mips
# ADD MTL /nologo /D "NDEBUG" /mips
MTL_PROJ=/nologo /D "NDEBUG" /mips 
CPP=cl.exe
# ADD BASE CPP /nologo /MD /Gt0 /QMOb2000 /W3 /GX /YX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /c
# ADD CPP /nologo /MD /Gt0 /QMOb2000 /W3 /GX /O2 /D "NDEBUG" /D "_WINDOWS" /D "_UNICODE" /D "_AFXDLL" /D "_MBCS" /FR /Yu"stdafx.h" /c
CPP_PROJ=/nologo /MD /Gt0 /QMOb2000 /W3 /GX /O2 /D "NDEBUG" /D "_WINDOWS" /D\
 "_UNICODE" /D "_AFXDLL" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"hiersvr.pch"\
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
RSC_PROJ=/l 0x409 /fo$(INTDIR)/"hiersvr.res" /d "NDEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o$(OUTDIR)/"hiersvr.bsc" 
BSC32_SBRS= \
	$(INTDIR)/svrview.sbr \
	$(INTDIR)/hiersvr.sbr \
	$(INTDIR)/svrdoc.sbr \
	$(INTDIR)/svritem.sbr \
	$(INTDIR)/stdafx.sbr \
	$(INTDIR)/mainfrm.sbr \
	$(INTDIR)/zoomdlg.sbr \
	$(INTDIR)/ipframe.sbr

$(OUTDIR)/hiersvr.bsc : $(OUTDIR)  $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 /NOLOGO /SUBSYSTEM:windows /MACHINE:MIPS
# ADD LINK32 /NOLOGO /ENTRY:"wWinMainCRTStartup" /SUBSYSTEM:windows /MACHINE:MIPS
LINK32_FLAGS=/NOLOGO /ENTRY:"wWinMainCRTStartup" /SUBSYSTEM:windows\
 /PDB:$(OUTDIR)/"hiersvr.pdb" /MACHINE:MIPS /OUT:$(OUTDIR)/"hiersvr.exe" 
DEF_FILE=
LINK32_OBJS= \
	$(INTDIR)/svrview.obj \
	$(INTDIR)/hiersvr.obj \
	$(INTDIR)/svrdoc.obj \
	$(INTDIR)/hiersvr.res \
	$(INTDIR)/svritem.obj \
	$(INTDIR)/stdafx.obj \
	$(INTDIR)/mainfrm.obj \
	$(INTDIR)/zoomdlg.obj \
	$(INTDIR)/ipframe.obj

$(OUTDIR)/hiersvr.exe : $(OUTDIR)  $(DEF_FILE) $(LINK32_OBJS)
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

ALL : $(OUTDIR)/hiersvr.exe $(OUTDIR)/hiersvr.bsc

$(OUTDIR) : 
    if not exist $(OUTDIR)/nul mkdir $(OUTDIR)

MTL=MkTypLib.exe
# ADD BASE MTL /nologo /D "_DEBUG" /mips
# ADD MTL /nologo /D "_DEBUG" /mips
MTL_PROJ=/nologo /D "_DEBUG" /mips 
CPP=cl.exe
# ADD BASE CPP /nologo /MD /Gt0 /QMOb2000 /W3 /GX /Zi /YX /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /c
# ADD CPP /nologo /MD /Gt0 /QMOb2000 /W3 /GX /Zi /Od /D "_DEBUG" /D "_WINDOWS" /D "_UNICODE" /D "_AFXDLL" /D "_MBCS" /FR /Yu"stdafx.h" /c
CPP_PROJ=/nologo /MD /Gt0 /QMOb2000 /W3 /GX /Zi /Od /D "_DEBUG" /D "_WINDOWS"\
 /D "_UNICODE" /D "_AFXDLL" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"hiersvr.pch"\
 /Yu"stdafx.h" /Fo$(INTDIR)/ /Fd$(OUTDIR)/"hiersvr.pdb" /c 
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
RSC_PROJ=/l 0x409 /fo$(INTDIR)/"hiersvr.res" /d "_DEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o$(OUTDIR)/"hiersvr.bsc" 
BSC32_SBRS= \
	$(INTDIR)/svrview.sbr \
	$(INTDIR)/hiersvr.sbr \
	$(INTDIR)/svrdoc.sbr \
	$(INTDIR)/svritem.sbr \
	$(INTDIR)/stdafx.sbr \
	$(INTDIR)/mainfrm.sbr \
	$(INTDIR)/zoomdlg.sbr \
	$(INTDIR)/ipframe.sbr

$(OUTDIR)/hiersvr.bsc : $(OUTDIR)  $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 /NOLOGO /SUBSYSTEM:windows /DEBUG /MACHINE:MIPS
# ADD LINK32 /NOLOGO /ENTRY:"wWinMainCRTStartup" /SUBSYSTEM:windows /DEBUG /MACHINE:MIPS
LINK32_FLAGS=/NOLOGO /ENTRY:"wWinMainCRTStartup" /SUBSYSTEM:windows\
 /PDB:$(OUTDIR)/"hiersvr.pdb" /DEBUG /MACHINE:MIPS /OUT:$(OUTDIR)/"hiersvr.exe" 
DEF_FILE=
LINK32_OBJS= \
	$(INTDIR)/svrview.obj \
	$(INTDIR)/hiersvr.obj \
	$(INTDIR)/svrdoc.obj \
	$(INTDIR)/hiersvr.res \
	$(INTDIR)/svritem.obj \
	$(INTDIR)/stdafx.obj \
	$(INTDIR)/mainfrm.obj \
	$(INTDIR)/zoomdlg.obj \
	$(INTDIR)/ipframe.obj

$(OUTDIR)/hiersvr.exe : $(OUTDIR)  $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 

################################################################################
# Begin Group "Source Files"

################################################################################
# Begin Source File

SOURCE=.\svrview.cpp
DEP_SVRVI=\
	.\stdafx.h\
	.\hiersvr.h\
	.\svrdoc.h\
	.\svrview.h\
	.\svritem.h\
	.\zoomdlg.h

!IF  "$(CFG)" == "Win32 (80x86) Release"

# ADD CPP /Yu"stdafx.h"

$(INTDIR)/svrview.obj :  $(SOURCE)  $(DEP_SVRVI) $(INTDIR) $(INTDIR)/stdafx.obj
   $(CPP) /nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D\
 "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"hiersvr.pch" /Yu"stdafx.h" /Fo$(INTDIR)/ /c\
  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

# ADD CPP /Yu"stdafx.h"

$(INTDIR)/svrview.obj :  $(SOURCE)  $(DEP_SVRVI) $(INTDIR) $(INTDIR)/stdafx.obj
   $(CPP) /nologo /MD /W3 /GX /Zi /Od /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D\
 "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"hiersvr.pch" /Yu"stdafx.h" /Fo$(INTDIR)/\
 /Fd$(OUTDIR)/"hiersvr.pdb" /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (80x86) Unicode Debug"

# ADD CPP /Yu"stdafx.h"

$(INTDIR)/svrview.obj :  $(SOURCE)  $(DEP_SVRVI) $(INTDIR) $(INTDIR)/stdafx.obj
   $(CPP) /nologo /MD /W3 /GX /Zi /Od /D "_DEBUG" /D "_WINDOWS" /D "_UNICODE"\
 /D "_AFXDLL" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"hiersvr.pch" /Yu"stdafx.h"\
 /Fo$(INTDIR)/ /Fd$(OUTDIR)/"hiersvr.pdb" /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (80x86) Unicode Release"

# ADD CPP /Yu"stdafx.h"

$(INTDIR)/svrview.obj :  $(SOURCE)  $(DEP_SVRVI) $(INTDIR) $(INTDIR)/stdafx.obj
   $(CPP) /nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "_WINDOWS" /D "_UNICODE" /D\
 "_AFXDLL" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"hiersvr.pch" /Yu"stdafx.h"\
 /Fo$(INTDIR)/ /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

# ADD BASE CPP /Gt0 /QMOb2000
# ADD CPP /Gt0 /QMOb2000 /Yu"stdafx.h"

$(INTDIR)/svrview.obj :  $(SOURCE)  $(DEP_SVRVI) $(INTDIR) $(INTDIR)/stdafx.obj
   $(CPP) /nologo /MD /Gt0 /QMOb2000 /W3 /GX /O2 /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"hiersvr.pch" /Yu"stdafx.h"\
 /Fo$(INTDIR)/ /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

# ADD BASE CPP /Gt0 /QMOb2000
# ADD CPP /Gt0 /QMOb2000 /Yu"stdafx.h"

$(INTDIR)/svrview.obj :  $(SOURCE)  $(DEP_SVRVI) $(INTDIR) $(INTDIR)/stdafx.obj
   $(CPP) /nologo /MD /Gt0 /QMOb2000 /W3 /GX /Zi /Od /D "_DEBUG" /D "_WINDOWS"\
 /D "_AFXDLL" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"hiersvr.pch" /Yu"stdafx.h"\
 /Fo$(INTDIR)/ /Fd$(OUTDIR)/"hiersvr.pdb" /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Unicode Release"

# ADD BASE CPP /Gt0 /QMOb2000
# ADD CPP /Gt0 /QMOb2000 /Yu"stdafx.h"

$(INTDIR)/svrview.obj :  $(SOURCE)  $(DEP_SVRVI) $(INTDIR) $(INTDIR)/stdafx.obj
   $(CPP) /nologo /MD /Gt0 /QMOb2000 /W3 /GX /O2 /D "NDEBUG" /D "_WINDOWS" /D\
 "_UNICODE" /D "_AFXDLL" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"hiersvr.pch"\
 /Yu"stdafx.h" /Fo$(INTDIR)/ /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Unicode Debug"

# ADD BASE CPP /Gt0 /QMOb2000
# ADD CPP /Gt0 /QMOb2000 /Yu"stdafx.h"

$(INTDIR)/svrview.obj :  $(SOURCE)  $(DEP_SVRVI) $(INTDIR) $(INTDIR)/stdafx.obj
   $(CPP) /nologo /MD /Gt0 /QMOb2000 /W3 /GX /Zi /Od /D "_DEBUG" /D "_WINDOWS"\
 /D "_UNICODE" /D "_AFXDLL" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"hiersvr.pch"\
 /Yu"stdafx.h" /Fo$(INTDIR)/ /Fd$(OUTDIR)/"hiersvr.pdb" /c  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\hiersvr.cpp
DEP_HIERS=\
	.\stdafx.h\
	.\hiersvr.h\
	.\mainfrm.h\
	.\ipframe.h\
	.\svrdoc.h\
	.\svrview.h\
	.\svritem.h

!IF  "$(CFG)" == "Win32 (80x86) Release"

# ADD CPP /Yu"stdafx.h"

$(INTDIR)/hiersvr.obj :  $(SOURCE)  $(DEP_HIERS) $(INTDIR) $(INTDIR)/stdafx.obj
   $(CPP) /nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D\
 "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"hiersvr.pch" /Yu"stdafx.h" /Fo$(INTDIR)/ /c\
  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

# ADD CPP /Yu"stdafx.h"

$(INTDIR)/hiersvr.obj :  $(SOURCE)  $(DEP_HIERS) $(INTDIR) $(INTDIR)/stdafx.obj
   $(CPP) /nologo /MD /W3 /GX /Zi /Od /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D\
 "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"hiersvr.pch" /Yu"stdafx.h" /Fo$(INTDIR)/\
 /Fd$(OUTDIR)/"hiersvr.pdb" /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (80x86) Unicode Debug"

# ADD CPP /Yu"stdafx.h"

$(INTDIR)/hiersvr.obj :  $(SOURCE)  $(DEP_HIERS) $(INTDIR) $(INTDIR)/stdafx.obj
   $(CPP) /nologo /MD /W3 /GX /Zi /Od /D "_DEBUG" /D "_WINDOWS" /D "_UNICODE"\
 /D "_AFXDLL" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"hiersvr.pch" /Yu"stdafx.h"\
 /Fo$(INTDIR)/ /Fd$(OUTDIR)/"hiersvr.pdb" /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (80x86) Unicode Release"

# ADD CPP /Yu"stdafx.h"

$(INTDIR)/hiersvr.obj :  $(SOURCE)  $(DEP_HIERS) $(INTDIR) $(INTDIR)/stdafx.obj
   $(CPP) /nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "_WINDOWS" /D "_UNICODE" /D\
 "_AFXDLL" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"hiersvr.pch" /Yu"stdafx.h"\
 /Fo$(INTDIR)/ /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

# ADD BASE CPP /Gt0 /QMOb2000
# ADD CPP /Gt0 /QMOb2000 /Yu"stdafx.h"

$(INTDIR)/hiersvr.obj :  $(SOURCE)  $(DEP_HIERS) $(INTDIR) $(INTDIR)/stdafx.obj
   $(CPP) /nologo /MD /Gt0 /QMOb2000 /W3 /GX /O2 /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"hiersvr.pch" /Yu"stdafx.h"\
 /Fo$(INTDIR)/ /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

# ADD BASE CPP /Gt0 /QMOb2000
# ADD CPP /Gt0 /QMOb2000 /Yu"stdafx.h"

$(INTDIR)/hiersvr.obj :  $(SOURCE)  $(DEP_HIERS) $(INTDIR) $(INTDIR)/stdafx.obj
   $(CPP) /nologo /MD /Gt0 /QMOb2000 /W3 /GX /Zi /Od /D "_DEBUG" /D "_WINDOWS"\
 /D "_AFXDLL" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"hiersvr.pch" /Yu"stdafx.h"\
 /Fo$(INTDIR)/ /Fd$(OUTDIR)/"hiersvr.pdb" /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Unicode Release"

# ADD BASE CPP /Gt0 /QMOb2000
# ADD CPP /Gt0 /QMOb2000 /Yu"stdafx.h"

$(INTDIR)/hiersvr.obj :  $(SOURCE)  $(DEP_HIERS) $(INTDIR) $(INTDIR)/stdafx.obj
   $(CPP) /nologo /MD /Gt0 /QMOb2000 /W3 /GX /O2 /D "NDEBUG" /D "_WINDOWS" /D\
 "_UNICODE" /D "_AFXDLL" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"hiersvr.pch"\
 /Yu"stdafx.h" /Fo$(INTDIR)/ /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Unicode Debug"

# ADD BASE CPP /Gt0 /QMOb2000
# ADD CPP /Gt0 /QMOb2000 /Yu"stdafx.h"

$(INTDIR)/hiersvr.obj :  $(SOURCE)  $(DEP_HIERS) $(INTDIR) $(INTDIR)/stdafx.obj
   $(CPP) /nologo /MD /Gt0 /QMOb2000 /W3 /GX /Zi /Od /D "_DEBUG" /D "_WINDOWS"\
 /D "_UNICODE" /D "_AFXDLL" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"hiersvr.pch"\
 /Yu"stdafx.h" /Fo$(INTDIR)/ /Fd$(OUTDIR)/"hiersvr.pdb" /c  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\svrdoc.cpp
DEP_SVRDO=\
	.\stdafx.h\
	.\hiersvr.h\
	.\svrdoc.h\
	.\svritem.h\
	.\svrview.h

!IF  "$(CFG)" == "Win32 (80x86) Release"

# ADD CPP /Yu"stdafx.h"

$(INTDIR)/svrdoc.obj :  $(SOURCE)  $(DEP_SVRDO) $(INTDIR) $(INTDIR)/stdafx.obj
   $(CPP) /nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D\
 "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"hiersvr.pch" /Yu"stdafx.h" /Fo$(INTDIR)/ /c\
  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

# ADD CPP /Yu"stdafx.h"

$(INTDIR)/svrdoc.obj :  $(SOURCE)  $(DEP_SVRDO) $(INTDIR) $(INTDIR)/stdafx.obj
   $(CPP) /nologo /MD /W3 /GX /Zi /Od /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D\
 "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"hiersvr.pch" /Yu"stdafx.h" /Fo$(INTDIR)/\
 /Fd$(OUTDIR)/"hiersvr.pdb" /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (80x86) Unicode Debug"

# ADD CPP /Yu"stdafx.h"

$(INTDIR)/svrdoc.obj :  $(SOURCE)  $(DEP_SVRDO) $(INTDIR) $(INTDIR)/stdafx.obj
   $(CPP) /nologo /MD /W3 /GX /Zi /Od /D "_DEBUG" /D "_WINDOWS" /D "_UNICODE"\
 /D "_AFXDLL" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"hiersvr.pch" /Yu"stdafx.h"\
 /Fo$(INTDIR)/ /Fd$(OUTDIR)/"hiersvr.pdb" /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (80x86) Unicode Release"

# ADD CPP /Yu"stdafx.h"

$(INTDIR)/svrdoc.obj :  $(SOURCE)  $(DEP_SVRDO) $(INTDIR) $(INTDIR)/stdafx.obj
   $(CPP) /nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "_WINDOWS" /D "_UNICODE" /D\
 "_AFXDLL" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"hiersvr.pch" /Yu"stdafx.h"\
 /Fo$(INTDIR)/ /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

# ADD BASE CPP /Gt0 /QMOb2000
# ADD CPP /Gt0 /QMOb2000 /Yu"stdafx.h"

$(INTDIR)/svrdoc.obj :  $(SOURCE)  $(DEP_SVRDO) $(INTDIR) $(INTDIR)/stdafx.obj
   $(CPP) /nologo /MD /Gt0 /QMOb2000 /W3 /GX /O2 /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"hiersvr.pch" /Yu"stdafx.h"\
 /Fo$(INTDIR)/ /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

# ADD BASE CPP /Gt0 /QMOb2000
# ADD CPP /Gt0 /QMOb2000 /Yu"stdafx.h"

$(INTDIR)/svrdoc.obj :  $(SOURCE)  $(DEP_SVRDO) $(INTDIR) $(INTDIR)/stdafx.obj
   $(CPP) /nologo /MD /Gt0 /QMOb2000 /W3 /GX /Zi /Od /D "_DEBUG" /D "_WINDOWS"\
 /D "_AFXDLL" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"hiersvr.pch" /Yu"stdafx.h"\
 /Fo$(INTDIR)/ /Fd$(OUTDIR)/"hiersvr.pdb" /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Unicode Release"

# ADD BASE CPP /Gt0 /QMOb2000
# ADD CPP /Gt0 /QMOb2000 /Yu"stdafx.h"

$(INTDIR)/svrdoc.obj :  $(SOURCE)  $(DEP_SVRDO) $(INTDIR) $(INTDIR)/stdafx.obj
   $(CPP) /nologo /MD /Gt0 /QMOb2000 /W3 /GX /O2 /D "NDEBUG" /D "_WINDOWS" /D\
 "_UNICODE" /D "_AFXDLL" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"hiersvr.pch"\
 /Yu"stdafx.h" /Fo$(INTDIR)/ /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Unicode Debug"

# ADD BASE CPP /Gt0 /QMOb2000
# ADD CPP /Gt0 /QMOb2000 /Yu"stdafx.h"

$(INTDIR)/svrdoc.obj :  $(SOURCE)  $(DEP_SVRDO) $(INTDIR) $(INTDIR)/stdafx.obj
   $(CPP) /nologo /MD /Gt0 /QMOb2000 /W3 /GX /Zi /Od /D "_DEBUG" /D "_WINDOWS"\
 /D "_UNICODE" /D "_AFXDLL" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"hiersvr.pch"\
 /Yu"stdafx.h" /Fo$(INTDIR)/ /Fd$(OUTDIR)/"hiersvr.pdb" /c  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\hiersvr.rc
DEP_HIERSV=\
	.\res\hiersvr.ico\
	.\res\svrdoc.ico\
	.\res\toolbar.bmp\
	.\res\itoolbar.bmp

!IF  "$(CFG)" == "Win32 (80x86) Release"

$(INTDIR)/hiersvr.res :  $(SOURCE)  $(DEP_HIERSV) $(INTDIR)
   $(RSC) $(RSC_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

$(INTDIR)/hiersvr.res :  $(SOURCE)  $(DEP_HIERSV) $(INTDIR)
   $(RSC) $(RSC_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (80x86) Unicode Debug"

$(INTDIR)/hiersvr.res :  $(SOURCE)  $(DEP_HIERSV) $(INTDIR)
   $(RSC) $(RSC_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (80x86) Unicode Release"

$(INTDIR)/hiersvr.res :  $(SOURCE)  $(DEP_HIERSV) $(INTDIR)
   $(RSC) $(RSC_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

$(INTDIR)/hiersvr.res :  $(SOURCE)  $(DEP_HIERSV) $(INTDIR)
   $(RSC) $(RSC_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

$(INTDIR)/hiersvr.res :  $(SOURCE)  $(DEP_HIERSV) $(INTDIR)
   $(RSC) $(RSC_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Unicode Release"

$(INTDIR)/hiersvr.res :  $(SOURCE)  $(DEP_HIERSV) $(INTDIR)
   $(RSC) $(RSC_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Unicode Debug"

$(INTDIR)/hiersvr.res :  $(SOURCE)  $(DEP_HIERSV) $(INTDIR)
   $(RSC) $(RSC_PROJ)  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\svritem.cpp
DEP_SVRIT=\
	.\stdafx.h\
	.\hiersvr.h\
	.\svritem.h\
	.\svrdoc.h\
	.\svrview.h

!IF  "$(CFG)" == "Win32 (80x86) Release"

# ADD CPP /Yu"stdafx.h"

$(INTDIR)/svritem.obj :  $(SOURCE)  $(DEP_SVRIT) $(INTDIR) $(INTDIR)/stdafx.obj
   $(CPP) /nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D\
 "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"hiersvr.pch" /Yu"stdafx.h" /Fo$(INTDIR)/ /c\
  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

# ADD CPP /Yu"stdafx.h"

$(INTDIR)/svritem.obj :  $(SOURCE)  $(DEP_SVRIT) $(INTDIR) $(INTDIR)/stdafx.obj
   $(CPP) /nologo /MD /W3 /GX /Zi /Od /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D\
 "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"hiersvr.pch" /Yu"stdafx.h" /Fo$(INTDIR)/\
 /Fd$(OUTDIR)/"hiersvr.pdb" /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (80x86) Unicode Debug"

# ADD CPP /Yu"stdafx.h"

$(INTDIR)/svritem.obj :  $(SOURCE)  $(DEP_SVRIT) $(INTDIR) $(INTDIR)/stdafx.obj
   $(CPP) /nologo /MD /W3 /GX /Zi /Od /D "_DEBUG" /D "_WINDOWS" /D "_UNICODE"\
 /D "_AFXDLL" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"hiersvr.pch" /Yu"stdafx.h"\
 /Fo$(INTDIR)/ /Fd$(OUTDIR)/"hiersvr.pdb" /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (80x86) Unicode Release"

# ADD CPP /Yu"stdafx.h"

$(INTDIR)/svritem.obj :  $(SOURCE)  $(DEP_SVRIT) $(INTDIR) $(INTDIR)/stdafx.obj
   $(CPP) /nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "_WINDOWS" /D "_UNICODE" /D\
 "_AFXDLL" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"hiersvr.pch" /Yu"stdafx.h"\
 /Fo$(INTDIR)/ /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

# ADD BASE CPP /Gt0 /QMOb2000
# ADD CPP /Gt0 /QMOb2000 /Yu"stdafx.h"

$(INTDIR)/svritem.obj :  $(SOURCE)  $(DEP_SVRIT) $(INTDIR) $(INTDIR)/stdafx.obj
   $(CPP) /nologo /MD /Gt0 /QMOb2000 /W3 /GX /O2 /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"hiersvr.pch" /Yu"stdafx.h"\
 /Fo$(INTDIR)/ /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

# ADD BASE CPP /Gt0 /QMOb2000
# ADD CPP /Gt0 /QMOb2000 /Yu"stdafx.h"

$(INTDIR)/svritem.obj :  $(SOURCE)  $(DEP_SVRIT) $(INTDIR) $(INTDIR)/stdafx.obj
   $(CPP) /nologo /MD /Gt0 /QMOb2000 /W3 /GX /Zi /Od /D "_DEBUG" /D "_WINDOWS"\
 /D "_AFXDLL" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"hiersvr.pch" /Yu"stdafx.h"\
 /Fo$(INTDIR)/ /Fd$(OUTDIR)/"hiersvr.pdb" /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Unicode Release"

# ADD BASE CPP /Gt0 /QMOb2000
# ADD CPP /Gt0 /QMOb2000 /Yu"stdafx.h"

$(INTDIR)/svritem.obj :  $(SOURCE)  $(DEP_SVRIT) $(INTDIR) $(INTDIR)/stdafx.obj
   $(CPP) /nologo /MD /Gt0 /QMOb2000 /W3 /GX /O2 /D "NDEBUG" /D "_WINDOWS" /D\
 "_UNICODE" /D "_AFXDLL" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"hiersvr.pch"\
 /Yu"stdafx.h" /Fo$(INTDIR)/ /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Unicode Debug"

# ADD BASE CPP /Gt0 /QMOb2000
# ADD CPP /Gt0 /QMOb2000 /Yu"stdafx.h"

$(INTDIR)/svritem.obj :  $(SOURCE)  $(DEP_SVRIT) $(INTDIR) $(INTDIR)/stdafx.obj
   $(CPP) /nologo /MD /Gt0 /QMOb2000 /W3 /GX /Zi /Od /D "_DEBUG" /D "_WINDOWS"\
 /D "_UNICODE" /D "_AFXDLL" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"hiersvr.pch"\
 /Yu"stdafx.h" /Fo$(INTDIR)/ /Fd$(OUTDIR)/"hiersvr.pdb" /c  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\stdafx.cpp
DEP_STDAF=\
	.\stdafx.h

!IF  "$(CFG)" == "Win32 (80x86) Release"

# ADD CPP /Yc"stdafx.h"

$(INTDIR)/stdafx.obj :  $(SOURCE)  $(DEP_STDAF) $(INTDIR)
   $(CPP) /nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D\
 "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"hiersvr.pch" /Yc"stdafx.h" /Fo$(INTDIR)/ /c\
  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

# ADD CPP /Yc"stdafx.h"

$(INTDIR)/stdafx.obj :  $(SOURCE)  $(DEP_STDAF) $(INTDIR)
   $(CPP) /nologo /MD /W3 /GX /Zi /Od /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D\
 "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"hiersvr.pch" /Yc"stdafx.h" /Fo$(INTDIR)/\
 /Fd$(OUTDIR)/"hiersvr.pdb" /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (80x86) Unicode Debug"

# ADD CPP /Yc"stdafx.h"

$(INTDIR)/stdafx.obj :  $(SOURCE)  $(DEP_STDAF) $(INTDIR)
   $(CPP) /nologo /MD /W3 /GX /Zi /Od /D "_DEBUG" /D "_WINDOWS" /D "_UNICODE"\
 /D "_AFXDLL" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"hiersvr.pch" /Yc"stdafx.h"\
 /Fo$(INTDIR)/ /Fd$(OUTDIR)/"hiersvr.pdb" /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (80x86) Unicode Release"

# ADD CPP /Yc"stdafx.h"

$(INTDIR)/stdafx.obj :  $(SOURCE)  $(DEP_STDAF) $(INTDIR)
   $(CPP) /nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "_WINDOWS" /D "_UNICODE" /D\
 "_AFXDLL" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"hiersvr.pch" /Yc"stdafx.h"\
 /Fo$(INTDIR)/ /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

# ADD BASE CPP /Gt0 /QMOb2000
# ADD CPP /Gt0 /QMOb2000 /Yc"stdafx.h"

$(INTDIR)/stdafx.obj :  $(SOURCE)  $(DEP_STDAF) $(INTDIR)
   $(CPP) /nologo /MD /Gt0 /QMOb2000 /W3 /GX /O2 /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"hiersvr.pch" /Yc"stdafx.h"\
 /Fo$(INTDIR)/ /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

# ADD BASE CPP /Gt0 /QMOb2000
# ADD CPP /Gt0 /QMOb2000 /Yc"stdafx.h"

$(INTDIR)/stdafx.obj :  $(SOURCE)  $(DEP_STDAF) $(INTDIR)
   $(CPP) /nologo /MD /Gt0 /QMOb2000 /W3 /GX /Zi /Od /D "_DEBUG" /D "_WINDOWS"\
 /D "_AFXDLL" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"hiersvr.pch" /Yc"stdafx.h"\
 /Fo$(INTDIR)/ /Fd$(OUTDIR)/"hiersvr.pdb" /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Unicode Release"

# ADD BASE CPP /Gt0 /QMOb2000
# ADD CPP /Gt0 /QMOb2000 /Yc"stdafx.h"

$(INTDIR)/stdafx.obj :  $(SOURCE)  $(DEP_STDAF) $(INTDIR)
   $(CPP) /nologo /MD /Gt0 /QMOb2000 /W3 /GX /O2 /D "NDEBUG" /D "_WINDOWS" /D\
 "_UNICODE" /D "_AFXDLL" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"hiersvr.pch"\
 /Yc"stdafx.h" /Fo$(INTDIR)/ /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Unicode Debug"

# ADD BASE CPP /Gt0 /QMOb2000
# ADD CPP /Gt0 /QMOb2000 /Yc"stdafx.h"

$(INTDIR)/stdafx.obj :  $(SOURCE)  $(DEP_STDAF) $(INTDIR)
   $(CPP) /nologo /MD /Gt0 /QMOb2000 /W3 /GX /Zi /Od /D "_DEBUG" /D "_WINDOWS"\
 /D "_UNICODE" /D "_AFXDLL" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"hiersvr.pch"\
 /Yc"stdafx.h" /Fo$(INTDIR)/ /Fd$(OUTDIR)/"hiersvr.pdb" /c  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\mainfrm.cpp
DEP_MAINF=\
	.\stdafx.h\
	.\hiersvr.h\
	.\mainfrm.h

!IF  "$(CFG)" == "Win32 (80x86) Release"

# ADD CPP /Yu"stdafx.h"

$(INTDIR)/mainfrm.obj :  $(SOURCE)  $(DEP_MAINF) $(INTDIR) $(INTDIR)/stdafx.obj
   $(CPP) /nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D\
 "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"hiersvr.pch" /Yu"stdafx.h" /Fo$(INTDIR)/ /c\
  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

# ADD CPP /Yu"stdafx.h"

$(INTDIR)/mainfrm.obj :  $(SOURCE)  $(DEP_MAINF) $(INTDIR) $(INTDIR)/stdafx.obj
   $(CPP) /nologo /MD /W3 /GX /Zi /Od /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D\
 "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"hiersvr.pch" /Yu"stdafx.h" /Fo$(INTDIR)/\
 /Fd$(OUTDIR)/"hiersvr.pdb" /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (80x86) Unicode Debug"

# ADD CPP /Yu"stdafx.h"

$(INTDIR)/mainfrm.obj :  $(SOURCE)  $(DEP_MAINF) $(INTDIR) $(INTDIR)/stdafx.obj
   $(CPP) /nologo /MD /W3 /GX /Zi /Od /D "_DEBUG" /D "_WINDOWS" /D "_UNICODE"\
 /D "_AFXDLL" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"hiersvr.pch" /Yu"stdafx.h"\
 /Fo$(INTDIR)/ /Fd$(OUTDIR)/"hiersvr.pdb" /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (80x86) Unicode Release"

# ADD CPP /Yu"stdafx.h"

$(INTDIR)/mainfrm.obj :  $(SOURCE)  $(DEP_MAINF) $(INTDIR) $(INTDIR)/stdafx.obj
   $(CPP) /nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "_WINDOWS" /D "_UNICODE" /D\
 "_AFXDLL" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"hiersvr.pch" /Yu"stdafx.h"\
 /Fo$(INTDIR)/ /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

# ADD BASE CPP /Gt0 /QMOb2000
# ADD CPP /Gt0 /QMOb2000 /Yu"stdafx.h"

$(INTDIR)/mainfrm.obj :  $(SOURCE)  $(DEP_MAINF) $(INTDIR) $(INTDIR)/stdafx.obj
   $(CPP) /nologo /MD /Gt0 /QMOb2000 /W3 /GX /O2 /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"hiersvr.pch" /Yu"stdafx.h"\
 /Fo$(INTDIR)/ /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

# ADD BASE CPP /Gt0 /QMOb2000
# ADD CPP /Gt0 /QMOb2000 /Yu"stdafx.h"

$(INTDIR)/mainfrm.obj :  $(SOURCE)  $(DEP_MAINF) $(INTDIR) $(INTDIR)/stdafx.obj
   $(CPP) /nologo /MD /Gt0 /QMOb2000 /W3 /GX /Zi /Od /D "_DEBUG" /D "_WINDOWS"\
 /D "_AFXDLL" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"hiersvr.pch" /Yu"stdafx.h"\
 /Fo$(INTDIR)/ /Fd$(OUTDIR)/"hiersvr.pdb" /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Unicode Release"

# ADD BASE CPP /Gt0 /QMOb2000
# ADD CPP /Gt0 /QMOb2000 /Yu"stdafx.h"

$(INTDIR)/mainfrm.obj :  $(SOURCE)  $(DEP_MAINF) $(INTDIR) $(INTDIR)/stdafx.obj
   $(CPP) /nologo /MD /Gt0 /QMOb2000 /W3 /GX /O2 /D "NDEBUG" /D "_WINDOWS" /D\
 "_UNICODE" /D "_AFXDLL" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"hiersvr.pch"\
 /Yu"stdafx.h" /Fo$(INTDIR)/ /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Unicode Debug"

# ADD BASE CPP /Gt0 /QMOb2000
# ADD CPP /Gt0 /QMOb2000 /Yu"stdafx.h"

$(INTDIR)/mainfrm.obj :  $(SOURCE)  $(DEP_MAINF) $(INTDIR) $(INTDIR)/stdafx.obj
   $(CPP) /nologo /MD /Gt0 /QMOb2000 /W3 /GX /Zi /Od /D "_DEBUG" /D "_WINDOWS"\
 /D "_UNICODE" /D "_AFXDLL" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"hiersvr.pch"\
 /Yu"stdafx.h" /Fo$(INTDIR)/ /Fd$(OUTDIR)/"hiersvr.pdb" /c  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\zoomdlg.cpp
DEP_ZOOMD=\
	.\stdafx.h\
	.\hiersvr.h\
	.\zoomdlg.h

!IF  "$(CFG)" == "Win32 (80x86) Release"

# ADD CPP /Yu"stdafx.h"

$(INTDIR)/zoomdlg.obj :  $(SOURCE)  $(DEP_ZOOMD) $(INTDIR) $(INTDIR)/stdafx.obj
   $(CPP) /nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D\
 "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"hiersvr.pch" /Yu"stdafx.h" /Fo$(INTDIR)/ /c\
  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

# ADD CPP /Yu"stdafx.h"

$(INTDIR)/zoomdlg.obj :  $(SOURCE)  $(DEP_ZOOMD) $(INTDIR) $(INTDIR)/stdafx.obj
   $(CPP) /nologo /MD /W3 /GX /Zi /Od /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D\
 "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"hiersvr.pch" /Yu"stdafx.h" /Fo$(INTDIR)/\
 /Fd$(OUTDIR)/"hiersvr.pdb" /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (80x86) Unicode Debug"

# ADD CPP /Yu"stdafx.h"

$(INTDIR)/zoomdlg.obj :  $(SOURCE)  $(DEP_ZOOMD) $(INTDIR) $(INTDIR)/stdafx.obj
   $(CPP) /nologo /MD /W3 /GX /Zi /Od /D "_DEBUG" /D "_WINDOWS" /D "_UNICODE"\
 /D "_AFXDLL" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"hiersvr.pch" /Yu"stdafx.h"\
 /Fo$(INTDIR)/ /Fd$(OUTDIR)/"hiersvr.pdb" /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (80x86) Unicode Release"

# ADD CPP /Yu"stdafx.h"

$(INTDIR)/zoomdlg.obj :  $(SOURCE)  $(DEP_ZOOMD) $(INTDIR) $(INTDIR)/stdafx.obj
   $(CPP) /nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "_WINDOWS" /D "_UNICODE" /D\
 "_AFXDLL" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"hiersvr.pch" /Yu"stdafx.h"\
 /Fo$(INTDIR)/ /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

# ADD BASE CPP /Gt0 /QMOb2000
# ADD CPP /Gt0 /QMOb2000 /Yu"stdafx.h"

$(INTDIR)/zoomdlg.obj :  $(SOURCE)  $(DEP_ZOOMD) $(INTDIR) $(INTDIR)/stdafx.obj
   $(CPP) /nologo /MD /Gt0 /QMOb2000 /W3 /GX /O2 /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"hiersvr.pch" /Yu"stdafx.h"\
 /Fo$(INTDIR)/ /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

# ADD BASE CPP /Gt0 /QMOb2000
# ADD CPP /Gt0 /QMOb2000 /Yu"stdafx.h"

$(INTDIR)/zoomdlg.obj :  $(SOURCE)  $(DEP_ZOOMD) $(INTDIR) $(INTDIR)/stdafx.obj
   $(CPP) /nologo /MD /Gt0 /QMOb2000 /W3 /GX /Zi /Od /D "_DEBUG" /D "_WINDOWS"\
 /D "_AFXDLL" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"hiersvr.pch" /Yu"stdafx.h"\
 /Fo$(INTDIR)/ /Fd$(OUTDIR)/"hiersvr.pdb" /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Unicode Release"

# ADD BASE CPP /Gt0 /QMOb2000
# ADD CPP /Gt0 /QMOb2000 /Yu"stdafx.h"

$(INTDIR)/zoomdlg.obj :  $(SOURCE)  $(DEP_ZOOMD) $(INTDIR) $(INTDIR)/stdafx.obj
   $(CPP) /nologo /MD /Gt0 /QMOb2000 /W3 /GX /O2 /D "NDEBUG" /D "_WINDOWS" /D\
 "_UNICODE" /D "_AFXDLL" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"hiersvr.pch"\
 /Yu"stdafx.h" /Fo$(INTDIR)/ /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Unicode Debug"

# ADD BASE CPP /Gt0 /QMOb2000
# ADD CPP /Gt0 /QMOb2000 /Yu"stdafx.h"

$(INTDIR)/zoomdlg.obj :  $(SOURCE)  $(DEP_ZOOMD) $(INTDIR) $(INTDIR)/stdafx.obj
   $(CPP) /nologo /MD /Gt0 /QMOb2000 /W3 /GX /Zi /Od /D "_DEBUG" /D "_WINDOWS"\
 /D "_UNICODE" /D "_AFXDLL" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"hiersvr.pch"\
 /Yu"stdafx.h" /Fo$(INTDIR)/ /Fd$(OUTDIR)/"hiersvr.pdb" /c  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\ipframe.cpp
DEP_IPFRA=\
	.\stdafx.h\
	.\hiersvr.h\
	.\ipframe.h

!IF  "$(CFG)" == "Win32 (80x86) Release"

# ADD CPP /Yu"stdafx.h"

$(INTDIR)/ipframe.obj :  $(SOURCE)  $(DEP_IPFRA) $(INTDIR) $(INTDIR)/stdafx.obj
   $(CPP) /nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D\
 "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"hiersvr.pch" /Yu"stdafx.h" /Fo$(INTDIR)/ /c\
  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

# ADD CPP /Yu"stdafx.h"

$(INTDIR)/ipframe.obj :  $(SOURCE)  $(DEP_IPFRA) $(INTDIR) $(INTDIR)/stdafx.obj
   $(CPP) /nologo /MD /W3 /GX /Zi /Od /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D\
 "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"hiersvr.pch" /Yu"stdafx.h" /Fo$(INTDIR)/\
 /Fd$(OUTDIR)/"hiersvr.pdb" /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (80x86) Unicode Debug"

# ADD CPP /Yu"stdafx.h"

$(INTDIR)/ipframe.obj :  $(SOURCE)  $(DEP_IPFRA) $(INTDIR) $(INTDIR)/stdafx.obj
   $(CPP) /nologo /MD /W3 /GX /Zi /Od /D "_DEBUG" /D "_WINDOWS" /D "_UNICODE"\
 /D "_AFXDLL" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"hiersvr.pch" /Yu"stdafx.h"\
 /Fo$(INTDIR)/ /Fd$(OUTDIR)/"hiersvr.pdb" /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (80x86) Unicode Release"

# ADD CPP /Yu"stdafx.h"

$(INTDIR)/ipframe.obj :  $(SOURCE)  $(DEP_IPFRA) $(INTDIR) $(INTDIR)/stdafx.obj
   $(CPP) /nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "_WINDOWS" /D "_UNICODE" /D\
 "_AFXDLL" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"hiersvr.pch" /Yu"stdafx.h"\
 /Fo$(INTDIR)/ /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

# ADD BASE CPP /Gt0 /QMOb2000
# ADD CPP /Gt0 /QMOb2000 /Yu"stdafx.h"

$(INTDIR)/ipframe.obj :  $(SOURCE)  $(DEP_IPFRA) $(INTDIR) $(INTDIR)/stdafx.obj
   $(CPP) /nologo /MD /Gt0 /QMOb2000 /W3 /GX /O2 /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"hiersvr.pch" /Yu"stdafx.h"\
 /Fo$(INTDIR)/ /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

# ADD BASE CPP /Gt0 /QMOb2000
# ADD CPP /Gt0 /QMOb2000 /Yu"stdafx.h"

$(INTDIR)/ipframe.obj :  $(SOURCE)  $(DEP_IPFRA) $(INTDIR) $(INTDIR)/stdafx.obj
   $(CPP) /nologo /MD /Gt0 /QMOb2000 /W3 /GX /Zi /Od /D "_DEBUG" /D "_WINDOWS"\
 /D "_AFXDLL" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"hiersvr.pch" /Yu"stdafx.h"\
 /Fo$(INTDIR)/ /Fd$(OUTDIR)/"hiersvr.pdb" /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Unicode Release"

# ADD BASE CPP /Gt0 /QMOb2000
# ADD CPP /Gt0 /QMOb2000 /Yu"stdafx.h"

$(INTDIR)/ipframe.obj :  $(SOURCE)  $(DEP_IPFRA) $(INTDIR) $(INTDIR)/stdafx.obj
   $(CPP) /nologo /MD /Gt0 /QMOb2000 /W3 /GX /O2 /D "NDEBUG" /D "_WINDOWS" /D\
 "_UNICODE" /D "_AFXDLL" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"hiersvr.pch"\
 /Yu"stdafx.h" /Fo$(INTDIR)/ /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Unicode Debug"

# ADD BASE CPP /Gt0 /QMOb2000
# ADD CPP /Gt0 /QMOb2000 /Yu"stdafx.h"

$(INTDIR)/ipframe.obj :  $(SOURCE)  $(DEP_IPFRA) $(INTDIR) $(INTDIR)/stdafx.obj
   $(CPP) /nologo /MD /Gt0 /QMOb2000 /W3 /GX /Zi /Od /D "_DEBUG" /D "_WINDOWS"\
 /D "_UNICODE" /D "_AFXDLL" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"hiersvr.pch"\
 /Yu"stdafx.h" /Fo$(INTDIR)/ /Fd$(OUTDIR)/"hiersvr.pdb" /c  $(SOURCE) 

!ENDIF 

# End Source File
# End Group
# End Project
################################################################################
