# Microsoft Developer Studio Generated NMAKE File, Format Version 40001
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

!IF "$(CFG)" == ""
CFG=catalog2 - Win32 Debug
!MESSAGE No configuration specified.  Defaulting to catalog2 - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "catalog2 - Win32 Release" && "$(CFG)" !=\
 "catalog2 - Win32 Debug" && "$(CFG)" != "catalog2 - Win32 UNICODE Debug" &&\
 "$(CFG)" != "catalog2 - Win32 UNICODE Release" && "$(CFG)" !=\
 "catalog2 - Win32 DLL Debug" && "$(CFG)" != "catalog2 - Win32 DLL Release" &&\
 "$(CFG)" != "catalog2 - Win32 UNICODE DLL Debug" && "$(CFG)" !=\
 "catalog2 - Win32 UNICODE DLL Release"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "catalog2.mak" CFG="catalog2 - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "catalog2 - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "catalog2 - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE "catalog2 - Win32 UNICODE Debug" (based on "Win32 (x86) Application")
!MESSAGE "catalog2 - Win32 UNICODE Release" (based on\
 "Win32 (x86) Application")
!MESSAGE "catalog2 - Win32 DLL Debug" (based on "Win32 (x86) Application")
!MESSAGE "catalog2 - Win32 DLL Release" (based on "Win32 (x86) Application")
!MESSAGE "catalog2 - Win32 UNICODE DLL Debug" (based on\
 "Win32 (x86) Application")
!MESSAGE "catalog2 - Win32 UNICODE DLL Release" (based on\
 "Win32 (x86) Application")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 
################################################################################
# Begin Project
# PROP Target_Last_Scanned "catalog2 - Win32 Debug"
RSC=rc.exe
MTL=mktyplib.exe
CPP=cl.exe

!IF  "$(CFG)" == "catalog2 - Win32 Release"

# PROP BASE Use_MFC 5
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 5
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "StubRel"
# PROP Intermediate_Dir "StubRel"
# PROP Target_Dir ""
OUTDIR=.\StubRel
INTDIR=.\StubRel

ALL : "$(OUTDIR)\catalog2.exe"

CLEAN : 
	-@erase ".\StubRel\catalog2.exe"
	-@erase ".\StubRel\catalog2.obj"
	-@erase ".\StubRel\catalog2.pch"
	-@erase ".\StubRel\stdafx.obj"
	-@erase ".\StubRel\mainfrm.obj"
	-@erase ".\StubRel\catalog2.res"
	-@erase ".\StubRel\catsets.obj"
	-@erase ".\StubRel\cat2view.obj"
	-@erase ".\StubRel\tabpage.obj"
	-@erase ".\StubRel\settings.obj"
	-@erase ".\StubRel\colpage.obj"
	-@erase ".\StubRel\cat2doc.obj"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Yu"stdafx.h" /c
# ADD CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Yu"stdafx.h" /c
CPP_PROJ=/nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_MBCS" /Fp"$(INTDIR)/catalog2.pch" /Yu"stdafx.h" /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\StubRel/
CPP_SBRS=
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/catalog2.res" /d "NDEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/catalog2.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 /nologo /subsystem:windows /machine:I386
LINK32_FLAGS=/nologo /subsystem:windows /incremental:no\
 /pdb:"$(OUTDIR)/catalog2.pdb" /machine:I386 /out:"$(OUTDIR)/catalog2.exe" 
LINK32_OBJS= \
	"$(INTDIR)/catalog2.obj" \
	"$(INTDIR)/stdafx.obj" \
	"$(INTDIR)/mainfrm.obj" \
	"$(INTDIR)/catsets.obj" \
	"$(INTDIR)/cat2view.obj" \
	"$(INTDIR)/tabpage.obj" \
	"$(INTDIR)/settings.obj" \
	"$(INTDIR)/colpage.obj" \
	"$(INTDIR)/cat2doc.obj" \
	"$(INTDIR)/catalog2.res"

"$(OUTDIR)\catalog2.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "catalog2 - Win32 Debug"

# PROP BASE Use_MFC 5
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 5
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "StubDebug"
# PROP Intermediate_Dir "StubDebug"
# PROP Target_Dir ""
OUTDIR=.\StubDebug
INTDIR=.\StubDebug

ALL : "$(OUTDIR)\catalog2.exe"

CLEAN : 
	-@erase ".\StubDebug\catalog2.exe"
	-@erase ".\StubDebug\catalog2.obj"
	-@erase ".\StubDebug\catalog2.pch"
	-@erase ".\StubDebug\stdafx.obj"
	-@erase ".\StubDebug\mainfrm.obj"
	-@erase ".\StubDebug\catalog2.res"
	-@erase ".\StubDebug\catsets.obj"
	-@erase ".\StubDebug\cat2view.obj"
	-@erase ".\StubDebug\tabpage.obj"
	-@erase ".\StubDebug\settings.obj"
	-@erase ".\StubDebug\colpage.obj"
	-@erase ".\StubDebug\cat2doc.obj"
	-@erase ".\StubDebug\catalog2.ilk"
	-@erase ".\StubDebug\catalog2.pdb"
	-@erase ".\StubDebug\vc40.pdb"
	-@erase ".\StubDebug\vc40.idb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /Yu"stdafx.h" /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /Yu"stdafx.h" /c
CPP_PROJ=/nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /D "_MBCS" /Fp"$(INTDIR)/catalog2.pch" /Yu"stdafx.h" /Fo"$(INTDIR)/"\
 /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\StubDebug/
CPP_SBRS=
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/catalog2.res" /d "_DEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/catalog2.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386
# ADD LINK32 /nologo /subsystem:windows /debug /machine:I386
LINK32_FLAGS=/nologo /subsystem:windows /incremental:yes\
 /pdb:"$(OUTDIR)/catalog2.pdb" /debug /machine:I386\
 /out:"$(OUTDIR)/catalog2.exe" 
LINK32_OBJS= \
	"$(INTDIR)/catalog2.obj" \
	"$(INTDIR)/stdafx.obj" \
	"$(INTDIR)/mainfrm.obj" \
	"$(INTDIR)/catsets.obj" \
	"$(INTDIR)/cat2view.obj" \
	"$(INTDIR)/tabpage.obj" \
	"$(INTDIR)/settings.obj" \
	"$(INTDIR)/colpage.obj" \
	"$(INTDIR)/cat2doc.obj" \
	"$(INTDIR)/catalog2.res"

"$(OUTDIR)\catalog2.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "catalog2 - Win32 UNICODE Debug"

# PROP BASE Use_MFC 5
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "catalog0"
# PROP BASE Intermediate_Dir "catalog0"
# PROP BASE Target_Dir ""
# PROP Use_MFC 5
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "StubUniDbg"
# PROP Intermediate_Dir "StubUniDbg"
# PROP Target_Dir ""
OUTDIR=.\StubUniDbg
INTDIR=.\StubUniDbg

ALL : "$(OUTDIR)\catalog2.exe"

CLEAN : 
	-@erase ".\StubUniDbg\catalog2.exe"
	-@erase ".\StubUniDbg\catalog2.obj"
	-@erase ".\StubUniDbg\catalog2.pch"
	-@erase ".\StubUniDbg\stdafx.obj"
	-@erase ".\StubUniDbg\mainfrm.obj"
	-@erase ".\StubUniDbg\catalog2.res"
	-@erase ".\StubUniDbg\catsets.obj"
	-@erase ".\StubUniDbg\cat2view.obj"
	-@erase ".\StubUniDbg\tabpage.obj"
	-@erase ".\StubUniDbg\settings.obj"
	-@erase ".\StubUniDbg\colpage.obj"
	-@erase ".\StubUniDbg\cat2doc.obj"
	-@erase ".\StubUniDbg\catalog2.ilk"
	-@erase ".\StubUniDbg\catalog2.pdb"
	-@erase ".\StubUniDbg\vc40.pdb"
	-@erase ".\StubUniDbg\vc40.idb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /Yu"stdafx.h" /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_UNICODE" /Yu"stdafx.h" /c
CPP_PROJ=/nologo /MTd /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS"\
 /D "_UNICODE" /Fp"$(INTDIR)/catalog2.pch" /Yu"stdafx.h" /Fo"$(INTDIR)/"\
 /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\StubUniDbg/
CPP_SBRS=
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/catalog2.res" /d "_DEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/catalog2.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386
# ADD LINK32 /nologo /entry:"wWinMainCRTStartup" /subsystem:windows /debug /machine:I386
LINK32_FLAGS=/nologo /entry:"wWinMainCRTStartup" /subsystem:windows\
 /incremental:yes /pdb:"$(OUTDIR)/catalog2.pdb" /debug /machine:I386\
 /out:"$(OUTDIR)/catalog2.exe" 
LINK32_OBJS= \
	"$(INTDIR)/catalog2.obj" \
	"$(INTDIR)/stdafx.obj" \
	"$(INTDIR)/mainfrm.obj" \
	"$(INTDIR)/catsets.obj" \
	"$(INTDIR)/cat2view.obj" \
	"$(INTDIR)/tabpage.obj" \
	"$(INTDIR)/settings.obj" \
	"$(INTDIR)/colpage.obj" \
	"$(INTDIR)/cat2doc.obj" \
	"$(INTDIR)/catalog2.res"

"$(OUTDIR)\catalog2.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "catalog2 - Win32 UNICODE Release"

# PROP BASE Use_MFC 5
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "catalog1"
# PROP BASE Intermediate_Dir "catalog1"
# PROP BASE Target_Dir ""
# PROP Use_MFC 5
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "StubUniRel"
# PROP Intermediate_Dir "StubUniRel"
# PROP Target_Dir ""
OUTDIR=.\StubUniRel
INTDIR=.\StubUniRel

ALL : "$(OUTDIR)\catalog2.exe"

CLEAN : 
	-@erase ".\StubUniRel\catalog2.exe"
	-@erase ".\StubUniRel\catalog2.obj"
	-@erase ".\StubUniRel\catalog2.pch"
	-@erase ".\StubUniRel\stdafx.obj"
	-@erase ".\StubUniRel\mainfrm.obj"
	-@erase ".\StubUniRel\catalog2.res"
	-@erase ".\StubUniRel\catsets.obj"
	-@erase ".\StubUniRel\cat2view.obj"
	-@erase ".\StubUniRel\tabpage.obj"
	-@erase ".\StubUniRel\settings.obj"
	-@erase ".\StubUniRel\colpage.obj"
	-@erase ".\StubUniRel\cat2doc.obj"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Yu"stdafx.h" /c
# ADD CPP /nologo /MT /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_UNICODE" /Yu"stdafx.h" /c
CPP_PROJ=/nologo /MT /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D\
 "_UNICODE" /Fp"$(INTDIR)/catalog2.pch" /Yu"stdafx.h" /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\StubUniRel/
CPP_SBRS=
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/catalog2.res" /d "NDEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/catalog2.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 /nologo /entry:"wWinMainCRTStartup" /subsystem:windows /machine:I386
LINK32_FLAGS=/nologo /entry:"wWinMainCRTStartup" /subsystem:windows\
 /incremental:no /pdb:"$(OUTDIR)/catalog2.pdb" /machine:I386\
 /out:"$(OUTDIR)/catalog2.exe" 
LINK32_OBJS= \
	"$(INTDIR)/catalog2.obj" \
	"$(INTDIR)/stdafx.obj" \
	"$(INTDIR)/mainfrm.obj" \
	"$(INTDIR)/catsets.obj" \
	"$(INTDIR)/cat2view.obj" \
	"$(INTDIR)/tabpage.obj" \
	"$(INTDIR)/settings.obj" \
	"$(INTDIR)/colpage.obj" \
	"$(INTDIR)/cat2doc.obj" \
	"$(INTDIR)/catalog2.res"

"$(OUTDIR)\catalog2.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "catalog2 - Win32 DLL Debug"

# PROP BASE Use_MFC 5
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "catalog3"
# PROP BASE Intermediate_Dir "catalog3"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
OUTDIR=.\Debug
INTDIR=.\Debug

ALL : "$(OUTDIR)\catalog2.exe"

CLEAN : 
	-@erase ".\Debug\catalog2.exe"
	-@erase ".\Debug\catalog2.obj"
	-@erase ".\Debug\catalog2.pch"
	-@erase ".\Debug\stdafx.obj"
	-@erase ".\Debug\mainfrm.obj"
	-@erase ".\Debug\catalog2.res"
	-@erase ".\Debug\catsets.obj"
	-@erase ".\Debug\cat2view.obj"
	-@erase ".\Debug\tabpage.obj"
	-@erase ".\Debug\settings.obj"
	-@erase ".\Debug\colpage.obj"
	-@erase ".\Debug\cat2doc.obj"
	-@erase ".\Debug\catalog2.ilk"
	-@erase ".\Debug\catalog2.pdb"
	-@erase ".\Debug\vc40.pdb"
	-@erase ".\Debug\vc40.idb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /Yu"stdafx.h" /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
CPP_PROJ=/nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/catalog2.pch" /Yu"stdafx.h"\
 /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\Debug/
CPP_SBRS=
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/catalog2.res" /d "_DEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/catalog2.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386
# ADD LINK32 /nologo /subsystem:windows /debug /machine:I386
LINK32_FLAGS=/nologo /subsystem:windows /incremental:yes\
 /pdb:"$(OUTDIR)/catalog2.pdb" /debug /machine:I386\
 /out:"$(OUTDIR)/catalog2.exe" 
LINK32_OBJS= \
	"$(INTDIR)/catalog2.obj" \
	"$(INTDIR)/stdafx.obj" \
	"$(INTDIR)/mainfrm.obj" \
	"$(INTDIR)/catsets.obj" \
	"$(INTDIR)/cat2view.obj" \
	"$(INTDIR)/tabpage.obj" \
	"$(INTDIR)/settings.obj" \
	"$(INTDIR)/colpage.obj" \
	"$(INTDIR)/cat2doc.obj" \
	"$(INTDIR)/catalog2.res"

"$(OUTDIR)\catalog2.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "catalog2 - Win32 DLL Release"

# PROP BASE Use_MFC 5
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "catalog4"
# PROP BASE Intermediate_Dir "catalog4"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
OUTDIR=.\Release
INTDIR=.\Release

ALL : "$(OUTDIR)\catalog2.exe"

CLEAN : 
	-@erase ".\Release\catalog2.exe"
	-@erase ".\Release\catalog2.obj"
	-@erase ".\Release\catalog2.pch"
	-@erase ".\Release\stdafx.obj"
	-@erase ".\Release\mainfrm.obj"
	-@erase ".\Release\catalog2.res"
	-@erase ".\Release\catsets.obj"
	-@erase ".\Release\cat2view.obj"
	-@erase ".\Release\tabpage.obj"
	-@erase ".\Release\settings.obj"
	-@erase ".\Release\colpage.obj"
	-@erase ".\Release\cat2doc.obj"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Yu"stdafx.h" /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
CPP_PROJ=/nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/catalog2.pch" /Yu"stdafx.h" /Fo"$(INTDIR)/"\
 /c 
CPP_OBJS=.\Release/
CPP_SBRS=
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/catalog2.res" /d "NDEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/catalog2.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 /nologo /subsystem:windows /machine:I386
LINK32_FLAGS=/nologo /subsystem:windows /incremental:no\
 /pdb:"$(OUTDIR)/catalog2.pdb" /machine:I386 /out:"$(OUTDIR)/catalog2.exe" 
LINK32_OBJS= \
	"$(INTDIR)/catalog2.obj" \
	"$(INTDIR)/stdafx.obj" \
	"$(INTDIR)/mainfrm.obj" \
	"$(INTDIR)/catsets.obj" \
	"$(INTDIR)/cat2view.obj" \
	"$(INTDIR)/tabpage.obj" \
	"$(INTDIR)/settings.obj" \
	"$(INTDIR)/colpage.obj" \
	"$(INTDIR)/cat2doc.obj" \
	"$(INTDIR)/catalog2.res"

"$(OUTDIR)\catalog2.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "catalog2 - Win32 UNICODE DLL Debug"

# PROP BASE Use_MFC 5
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "catalog5"
# PROP BASE Intermediate_Dir "catalog5"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "UniDebug"
# PROP Intermediate_Dir "UniDebug"
# PROP Target_Dir ""
OUTDIR=.\UniDebug
INTDIR=.\UniDebug

ALL : "$(OUTDIR)\catalog2.exe"

CLEAN : 
	-@erase ".\UniDebug\catalog2.exe"
	-@erase ".\UniDebug\catalog2.obj"
	-@erase ".\UniDebug\catalog2.pch"
	-@erase ".\UniDebug\stdafx.obj"
	-@erase ".\UniDebug\mainfrm.obj"
	-@erase ".\UniDebug\catalog2.res"
	-@erase ".\UniDebug\catsets.obj"
	-@erase ".\UniDebug\cat2view.obj"
	-@erase ".\UniDebug\tabpage.obj"
	-@erase ".\UniDebug\settings.obj"
	-@erase ".\UniDebug\colpage.obj"
	-@erase ".\UniDebug\cat2doc.obj"
	-@erase ".\UniDebug\catalog2.ilk"
	-@erase ".\UniDebug\catalog2.pdb"
	-@erase ".\UniDebug\vc40.pdb"
	-@erase ".\UniDebug\vc40.idb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_UNICODE" /Yu"stdafx.h" /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_UNICODE" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
CPP_PROJ=/nologo /MDd /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS"\
 /D "_UNICODE" /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/catalog2.pch" /Yu"stdafx.h"\
 /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\UniDebug/
CPP_SBRS=
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/catalog2.res" /d "_DEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/catalog2.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 /nologo /entry:"wWinMainCRTStartup" /subsystem:windows /debug /machine:I386
# ADD LINK32 /nologo /entry:"wWinMainCRTStartup" /subsystem:windows /debug /machine:I386
LINK32_FLAGS=/nologo /entry:"wWinMainCRTStartup" /subsystem:windows\
 /incremental:yes /pdb:"$(OUTDIR)/catalog2.pdb" /debug /machine:I386\
 /out:"$(OUTDIR)/catalog2.exe" 
LINK32_OBJS= \
	"$(INTDIR)/catalog2.obj" \
	"$(INTDIR)/stdafx.obj" \
	"$(INTDIR)/mainfrm.obj" \
	"$(INTDIR)/catsets.obj" \
	"$(INTDIR)/cat2view.obj" \
	"$(INTDIR)/tabpage.obj" \
	"$(INTDIR)/settings.obj" \
	"$(INTDIR)/colpage.obj" \
	"$(INTDIR)/cat2doc.obj" \
	"$(INTDIR)/catalog2.res"

"$(OUTDIR)\catalog2.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "catalog2 - Win32 UNICODE DLL Release"

# PROP BASE Use_MFC 5
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "catalog6"
# PROP BASE Intermediate_Dir "catalog6"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "UniRelease"
# PROP Intermediate_Dir "UniRelease"
# PROP Target_Dir ""
OUTDIR=.\UniRelease
INTDIR=.\UniRelease

ALL : "$(OUTDIR)\catalog2.exe"

CLEAN : 
	-@erase ".\UniRelease\catalog2.exe"
	-@erase ".\UniRelease\catalog2.obj"
	-@erase ".\UniRelease\catalog2.pch"
	-@erase ".\UniRelease\stdafx.obj"
	-@erase ".\UniRelease\mainfrm.obj"
	-@erase ".\UniRelease\catalog2.res"
	-@erase ".\UniRelease\catsets.obj"
	-@erase ".\UniRelease\cat2view.obj"
	-@erase ".\UniRelease\tabpage.obj"
	-@erase ".\UniRelease\settings.obj"
	-@erase ".\UniRelease\colpage.obj"
	-@erase ".\UniRelease\cat2doc.obj"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_UNICODE" /Yu"stdafx.h" /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_UNICODE" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
CPP_PROJ=/nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D\
 "_UNICODE" /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/catalog2.pch" /Yu"stdafx.h"\
 /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\UniRelease/
CPP_SBRS=
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/catalog2.res" /d "NDEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/catalog2.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 /nologo /entry:"wWinMainCRTStartup" /subsystem:windows /machine:I386
# ADD LINK32 /nologo /entry:"wWinMainCRTStartup" /subsystem:windows /machine:I386
LINK32_FLAGS=/nologo /entry:"wWinMainCRTStartup" /subsystem:windows\
 /incremental:no /pdb:"$(OUTDIR)/catalog2.pdb" /machine:I386\
 /out:"$(OUTDIR)/catalog2.exe" 
LINK32_OBJS= \
	"$(INTDIR)/catalog2.obj" \
	"$(INTDIR)/stdafx.obj" \
	"$(INTDIR)/mainfrm.obj" \
	"$(INTDIR)/catsets.obj" \
	"$(INTDIR)/cat2view.obj" \
	"$(INTDIR)/tabpage.obj" \
	"$(INTDIR)/settings.obj" \
	"$(INTDIR)/colpage.obj" \
	"$(INTDIR)/cat2doc.obj" \
	"$(INTDIR)/catalog2.res"

"$(OUTDIR)\catalog2.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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

.c{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

################################################################################
# Begin Target

# Name "catalog2 - Win32 Release"
# Name "catalog2 - Win32 Debug"
# Name "catalog2 - Win32 UNICODE Debug"
# Name "catalog2 - Win32 UNICODE Release"
# Name "catalog2 - Win32 DLL Debug"
# Name "catalog2 - Win32 DLL Release"
# Name "catalog2 - Win32 UNICODE DLL Debug"
# Name "catalog2 - Win32 UNICODE DLL Release"

!IF  "$(CFG)" == "catalog2 - Win32 Release"

!ELSEIF  "$(CFG)" == "catalog2 - Win32 Debug"

!ELSEIF  "$(CFG)" == "catalog2 - Win32 UNICODE Debug"

!ELSEIF  "$(CFG)" == "catalog2 - Win32 UNICODE Release"

!ELSEIF  "$(CFG)" == "catalog2 - Win32 DLL Debug"

!ELSEIF  "$(CFG)" == "catalog2 - Win32 DLL Release"

!ELSEIF  "$(CFG)" == "catalog2 - Win32 UNICODE DLL Debug"

!ELSEIF  "$(CFG)" == "catalog2 - Win32 UNICODE DLL Release"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\readme.txt

!IF  "$(CFG)" == "catalog2 - Win32 Release"

!ELSEIF  "$(CFG)" == "catalog2 - Win32 Debug"

!ELSEIF  "$(CFG)" == "catalog2 - Win32 UNICODE Debug"

!ELSEIF  "$(CFG)" == "catalog2 - Win32 UNICODE Release"

!ELSEIF  "$(CFG)" == "catalog2 - Win32 DLL Debug"

!ELSEIF  "$(CFG)" == "catalog2 - Win32 DLL Release"

!ELSEIF  "$(CFG)" == "catalog2 - Win32 UNICODE DLL Debug"

!ELSEIF  "$(CFG)" == "catalog2 - Win32 UNICODE DLL Release"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\catalog2.cpp
DEP_CPP_CATAL=\
	".\mainfrm.h"\
	".\cat2doc.h"\
	".\cat2view.h"\
	".\catalog2.h"\
	".\catsets.h"\
	".\stdafx.h"\
	

!IF  "$(CFG)" == "catalog2 - Win32 Release"


"$(INTDIR)\catalog2.obj" : $(SOURCE) $(DEP_CPP_CATAL) "$(INTDIR)"\
 "$(INTDIR)\catalog2.pch"


!ELSEIF  "$(CFG)" == "catalog2 - Win32 Debug"


"$(INTDIR)\catalog2.obj" : $(SOURCE) $(DEP_CPP_CATAL) "$(INTDIR)"\
 "$(INTDIR)\catalog2.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_CATAL) "$(INTDIR)"\
 "$(INTDIR)\catalog2.pch"

"$(INTDIR)\vc40.idb" : $(SOURCE) $(DEP_CPP_CATAL) "$(INTDIR)"\
 "$(INTDIR)\catalog2.pch"


!ELSEIF  "$(CFG)" == "catalog2 - Win32 UNICODE Debug"


"$(INTDIR)\catalog2.obj" : $(SOURCE) $(DEP_CPP_CATAL) "$(INTDIR)"\
 "$(INTDIR)\catalog2.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_CATAL) "$(INTDIR)"\
 "$(INTDIR)\catalog2.pch"

"$(INTDIR)\vc40.idb" : $(SOURCE) $(DEP_CPP_CATAL) "$(INTDIR)"\
 "$(INTDIR)\catalog2.pch"


!ELSEIF  "$(CFG)" == "catalog2 - Win32 UNICODE Release"


"$(INTDIR)\catalog2.obj" : $(SOURCE) $(DEP_CPP_CATAL) "$(INTDIR)"\
 "$(INTDIR)\catalog2.pch"


!ELSEIF  "$(CFG)" == "catalog2 - Win32 DLL Debug"


"$(INTDIR)\catalog2.obj" : $(SOURCE) $(DEP_CPP_CATAL) "$(INTDIR)"\
 "$(INTDIR)\catalog2.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_CATAL) "$(INTDIR)"\
 "$(INTDIR)\catalog2.pch"

"$(INTDIR)\vc40.idb" : $(SOURCE) $(DEP_CPP_CATAL) "$(INTDIR)"\
 "$(INTDIR)\catalog2.pch"


!ELSEIF  "$(CFG)" == "catalog2 - Win32 DLL Release"


"$(INTDIR)\catalog2.obj" : $(SOURCE) $(DEP_CPP_CATAL) "$(INTDIR)"\
 "$(INTDIR)\catalog2.pch"


!ELSEIF  "$(CFG)" == "catalog2 - Win32 UNICODE DLL Debug"


"$(INTDIR)\catalog2.obj" : $(SOURCE) $(DEP_CPP_CATAL) "$(INTDIR)"\
 "$(INTDIR)\catalog2.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_CATAL) "$(INTDIR)"\
 "$(INTDIR)\catalog2.pch"

"$(INTDIR)\vc40.idb" : $(SOURCE) $(DEP_CPP_CATAL) "$(INTDIR)"\
 "$(INTDIR)\catalog2.pch"


!ELSEIF  "$(CFG)" == "catalog2 - Win32 UNICODE DLL Release"


"$(INTDIR)\catalog2.obj" : $(SOURCE) $(DEP_CPP_CATAL) "$(INTDIR)"\
 "$(INTDIR)\catalog2.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\stdafx.cpp
DEP_CPP_STDAF=\
	".\stdafx.h"\
	

!IF  "$(CFG)" == "catalog2 - Win32 Release"

# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS"\
 /Fp"$(INTDIR)/catalog2.pch" /Yc"stdafx.h" /Fo"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\catalog2.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "catalog2 - Win32 Debug"

# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /D "_MBCS" /Fp"$(INTDIR)/catalog2.pch" /Yc"stdafx.h" /Fo"$(INTDIR)/"\
 /Fd"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\vc40.idb" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\catalog2.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "catalog2 - Win32 UNICODE Debug"

# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MTd /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS"\
 /D "_UNICODE" /Fp"$(INTDIR)/catalog2.pch" /Yc"stdafx.h" /Fo"$(INTDIR)/"\
 /Fd"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\vc40.idb" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\catalog2.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "catalog2 - Win32 UNICODE Release"

# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MT /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D\
 "_UNICODE" /Fp"$(INTDIR)/catalog2.pch" /Yc"stdafx.h" /Fo"$(INTDIR)/" /c\
 $(SOURCE) \
	

"$(INTDIR)\stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\catalog2.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "catalog2 - Win32 DLL Debug"

# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/catalog2.pch" /Yc"stdafx.h"\
 /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\vc40.idb" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\catalog2.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "catalog2 - Win32 DLL Release"

# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/catalog2.pch" /Yc"stdafx.h" /Fo"$(INTDIR)/"\
 /c $(SOURCE) \
	

"$(INTDIR)\stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\catalog2.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "catalog2 - Win32 UNICODE DLL Debug"

# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MDd /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS"\
 /D "_UNICODE" /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/catalog2.pch" /Yc"stdafx.h"\
 /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\vc40.idb" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\catalog2.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "catalog2 - Win32 UNICODE DLL Release"

# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D\
 "_UNICODE" /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/catalog2.pch" /Yc"stdafx.h"\
 /Fo"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\catalog2.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\mainfrm.cpp
DEP_CPP_MAINF=\
	".\mainfrm.h"\
	".\catalog2.h"\
	".\stdafx.h"\
	

!IF  "$(CFG)" == "catalog2 - Win32 Release"


"$(INTDIR)\mainfrm.obj" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\catalog2.pch"


!ELSEIF  "$(CFG)" == "catalog2 - Win32 Debug"


"$(INTDIR)\mainfrm.obj" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\catalog2.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\catalog2.pch"

"$(INTDIR)\vc40.idb" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\catalog2.pch"


!ELSEIF  "$(CFG)" == "catalog2 - Win32 UNICODE Debug"


"$(INTDIR)\mainfrm.obj" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\catalog2.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\catalog2.pch"

"$(INTDIR)\vc40.idb" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\catalog2.pch"


!ELSEIF  "$(CFG)" == "catalog2 - Win32 UNICODE Release"


"$(INTDIR)\mainfrm.obj" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\catalog2.pch"


!ELSEIF  "$(CFG)" == "catalog2 - Win32 DLL Debug"


"$(INTDIR)\mainfrm.obj" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\catalog2.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\catalog2.pch"

"$(INTDIR)\vc40.idb" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\catalog2.pch"


!ELSEIF  "$(CFG)" == "catalog2 - Win32 DLL Release"


"$(INTDIR)\mainfrm.obj" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\catalog2.pch"


!ELSEIF  "$(CFG)" == "catalog2 - Win32 UNICODE DLL Debug"


"$(INTDIR)\mainfrm.obj" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\catalog2.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\catalog2.pch"

"$(INTDIR)\vc40.idb" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\catalog2.pch"


!ELSEIF  "$(CFG)" == "catalog2 - Win32 UNICODE DLL Release"


"$(INTDIR)\mainfrm.obj" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\catalog2.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\catalog2.rc
DEP_RSC_CATALO=\
	".\res\catalog2.ico"\
	".\res\cat2doc.ico"\
	".\res\toolbar1.bmp"\
	".\res\catalog2.rc2"\
	

!IF  "$(CFG)" == "catalog2 - Win32 Release"


"$(INTDIR)\catalog2.res" : $(SOURCE) $(DEP_RSC_CATALO) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "catalog2 - Win32 Debug"


"$(INTDIR)\catalog2.res" : $(SOURCE) $(DEP_RSC_CATALO) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "catalog2 - Win32 UNICODE Debug"


"$(INTDIR)\catalog2.res" : $(SOURCE) $(DEP_RSC_CATALO) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "catalog2 - Win32 UNICODE Release"


"$(INTDIR)\catalog2.res" : $(SOURCE) $(DEP_RSC_CATALO) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "catalog2 - Win32 DLL Debug"


"$(INTDIR)\catalog2.res" : $(SOURCE) $(DEP_RSC_CATALO) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "catalog2 - Win32 DLL Release"


"$(INTDIR)\catalog2.res" : $(SOURCE) $(DEP_RSC_CATALO) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "catalog2 - Win32 UNICODE DLL Debug"


"$(INTDIR)\catalog2.res" : $(SOURCE) $(DEP_RSC_CATALO) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "catalog2 - Win32 UNICODE DLL Release"


"$(INTDIR)\catalog2.res" : $(SOURCE) $(DEP_RSC_CATALO) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\catsets.cpp
DEP_CPP_CATSE=\
	".\catsets.h"\
	".\stdafx.h"\
	

!IF  "$(CFG)" == "catalog2 - Win32 Release"


"$(INTDIR)\catsets.obj" : $(SOURCE) $(DEP_CPP_CATSE) "$(INTDIR)"\
 "$(INTDIR)\catalog2.pch"


!ELSEIF  "$(CFG)" == "catalog2 - Win32 Debug"


"$(INTDIR)\catsets.obj" : $(SOURCE) $(DEP_CPP_CATSE) "$(INTDIR)"\
 "$(INTDIR)\catalog2.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_CATSE) "$(INTDIR)"\
 "$(INTDIR)\catalog2.pch"

"$(INTDIR)\vc40.idb" : $(SOURCE) $(DEP_CPP_CATSE) "$(INTDIR)"\
 "$(INTDIR)\catalog2.pch"


!ELSEIF  "$(CFG)" == "catalog2 - Win32 UNICODE Debug"


"$(INTDIR)\catsets.obj" : $(SOURCE) $(DEP_CPP_CATSE) "$(INTDIR)"\
 "$(INTDIR)\catalog2.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_CATSE) "$(INTDIR)"\
 "$(INTDIR)\catalog2.pch"

"$(INTDIR)\vc40.idb" : $(SOURCE) $(DEP_CPP_CATSE) "$(INTDIR)"\
 "$(INTDIR)\catalog2.pch"


!ELSEIF  "$(CFG)" == "catalog2 - Win32 UNICODE Release"


"$(INTDIR)\catsets.obj" : $(SOURCE) $(DEP_CPP_CATSE) "$(INTDIR)"\
 "$(INTDIR)\catalog2.pch"


!ELSEIF  "$(CFG)" == "catalog2 - Win32 DLL Debug"


"$(INTDIR)\catsets.obj" : $(SOURCE) $(DEP_CPP_CATSE) "$(INTDIR)"\
 "$(INTDIR)\catalog2.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_CATSE) "$(INTDIR)"\
 "$(INTDIR)\catalog2.pch"

"$(INTDIR)\vc40.idb" : $(SOURCE) $(DEP_CPP_CATSE) "$(INTDIR)"\
 "$(INTDIR)\catalog2.pch"


!ELSEIF  "$(CFG)" == "catalog2 - Win32 DLL Release"


"$(INTDIR)\catsets.obj" : $(SOURCE) $(DEP_CPP_CATSE) "$(INTDIR)"\
 "$(INTDIR)\catalog2.pch"


!ELSEIF  "$(CFG)" == "catalog2 - Win32 UNICODE DLL Debug"


"$(INTDIR)\catsets.obj" : $(SOURCE) $(DEP_CPP_CATSE) "$(INTDIR)"\
 "$(INTDIR)\catalog2.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_CATSE) "$(INTDIR)"\
 "$(INTDIR)\catalog2.pch"

"$(INTDIR)\vc40.idb" : $(SOURCE) $(DEP_CPP_CATSE) "$(INTDIR)"\
 "$(INTDIR)\catalog2.pch"


!ELSEIF  "$(CFG)" == "catalog2 - Win32 UNICODE DLL Release"


"$(INTDIR)\catsets.obj" : $(SOURCE) $(DEP_CPP_CATSE) "$(INTDIR)"\
 "$(INTDIR)\catalog2.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\cat2view.cpp
DEP_CPP_CAT2V=\
	".\cat2doc.h"\
	".\cat2view.h"\
	".\catalog2.h"\
	".\catsets.h"\
	".\stdafx.h"\
	

!IF  "$(CFG)" == "catalog2 - Win32 Release"


"$(INTDIR)\cat2view.obj" : $(SOURCE) $(DEP_CPP_CAT2V) "$(INTDIR)"\
 "$(INTDIR)\catalog2.pch"


!ELSEIF  "$(CFG)" == "catalog2 - Win32 Debug"


"$(INTDIR)\cat2view.obj" : $(SOURCE) $(DEP_CPP_CAT2V) "$(INTDIR)"\
 "$(INTDIR)\catalog2.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_CAT2V) "$(INTDIR)"\
 "$(INTDIR)\catalog2.pch"

"$(INTDIR)\vc40.idb" : $(SOURCE) $(DEP_CPP_CAT2V) "$(INTDIR)"\
 "$(INTDIR)\catalog2.pch"


!ELSEIF  "$(CFG)" == "catalog2 - Win32 UNICODE Debug"


"$(INTDIR)\cat2view.obj" : $(SOURCE) $(DEP_CPP_CAT2V) "$(INTDIR)"\
 "$(INTDIR)\catalog2.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_CAT2V) "$(INTDIR)"\
 "$(INTDIR)\catalog2.pch"

"$(INTDIR)\vc40.idb" : $(SOURCE) $(DEP_CPP_CAT2V) "$(INTDIR)"\
 "$(INTDIR)\catalog2.pch"


!ELSEIF  "$(CFG)" == "catalog2 - Win32 UNICODE Release"


"$(INTDIR)\cat2view.obj" : $(SOURCE) $(DEP_CPP_CAT2V) "$(INTDIR)"\
 "$(INTDIR)\catalog2.pch"


!ELSEIF  "$(CFG)" == "catalog2 - Win32 DLL Debug"


"$(INTDIR)\cat2view.obj" : $(SOURCE) $(DEP_CPP_CAT2V) "$(INTDIR)"\
 "$(INTDIR)\catalog2.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_CAT2V) "$(INTDIR)"\
 "$(INTDIR)\catalog2.pch"

"$(INTDIR)\vc40.idb" : $(SOURCE) $(DEP_CPP_CAT2V) "$(INTDIR)"\
 "$(INTDIR)\catalog2.pch"


!ELSEIF  "$(CFG)" == "catalog2 - Win32 DLL Release"


"$(INTDIR)\cat2view.obj" : $(SOURCE) $(DEP_CPP_CAT2V) "$(INTDIR)"\
 "$(INTDIR)\catalog2.pch"


!ELSEIF  "$(CFG)" == "catalog2 - Win32 UNICODE DLL Debug"


"$(INTDIR)\cat2view.obj" : $(SOURCE) $(DEP_CPP_CAT2V) "$(INTDIR)"\
 "$(INTDIR)\catalog2.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_CAT2V) "$(INTDIR)"\
 "$(INTDIR)\catalog2.pch"

"$(INTDIR)\vc40.idb" : $(SOURCE) $(DEP_CPP_CAT2V) "$(INTDIR)"\
 "$(INTDIR)\catalog2.pch"


!ELSEIF  "$(CFG)" == "catalog2 - Win32 UNICODE DLL Release"


"$(INTDIR)\cat2view.obj" : $(SOURCE) $(DEP_CPP_CAT2V) "$(INTDIR)"\
 "$(INTDIR)\catalog2.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\tabpage.cpp
DEP_CPP_TABPA=\
	".\tabpage.h"\
	".\catalog2.h"\
	".\stdafx.h"\
	

!IF  "$(CFG)" == "catalog2 - Win32 Release"


"$(INTDIR)\tabpage.obj" : $(SOURCE) $(DEP_CPP_TABPA) "$(INTDIR)"\
 "$(INTDIR)\catalog2.pch"


!ELSEIF  "$(CFG)" == "catalog2 - Win32 Debug"


"$(INTDIR)\tabpage.obj" : $(SOURCE) $(DEP_CPP_TABPA) "$(INTDIR)"\
 "$(INTDIR)\catalog2.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_TABPA) "$(INTDIR)"\
 "$(INTDIR)\catalog2.pch"

"$(INTDIR)\vc40.idb" : $(SOURCE) $(DEP_CPP_TABPA) "$(INTDIR)"\
 "$(INTDIR)\catalog2.pch"


!ELSEIF  "$(CFG)" == "catalog2 - Win32 UNICODE Debug"


"$(INTDIR)\tabpage.obj" : $(SOURCE) $(DEP_CPP_TABPA) "$(INTDIR)"\
 "$(INTDIR)\catalog2.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_TABPA) "$(INTDIR)"\
 "$(INTDIR)\catalog2.pch"

"$(INTDIR)\vc40.idb" : $(SOURCE) $(DEP_CPP_TABPA) "$(INTDIR)"\
 "$(INTDIR)\catalog2.pch"


!ELSEIF  "$(CFG)" == "catalog2 - Win32 UNICODE Release"


"$(INTDIR)\tabpage.obj" : $(SOURCE) $(DEP_CPP_TABPA) "$(INTDIR)"\
 "$(INTDIR)\catalog2.pch"


!ELSEIF  "$(CFG)" == "catalog2 - Win32 DLL Debug"


"$(INTDIR)\tabpage.obj" : $(SOURCE) $(DEP_CPP_TABPA) "$(INTDIR)"\
 "$(INTDIR)\catalog2.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_TABPA) "$(INTDIR)"\
 "$(INTDIR)\catalog2.pch"

"$(INTDIR)\vc40.idb" : $(SOURCE) $(DEP_CPP_TABPA) "$(INTDIR)"\
 "$(INTDIR)\catalog2.pch"


!ELSEIF  "$(CFG)" == "catalog2 - Win32 DLL Release"


"$(INTDIR)\tabpage.obj" : $(SOURCE) $(DEP_CPP_TABPA) "$(INTDIR)"\
 "$(INTDIR)\catalog2.pch"


!ELSEIF  "$(CFG)" == "catalog2 - Win32 UNICODE DLL Debug"


"$(INTDIR)\tabpage.obj" : $(SOURCE) $(DEP_CPP_TABPA) "$(INTDIR)"\
 "$(INTDIR)\catalog2.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_TABPA) "$(INTDIR)"\
 "$(INTDIR)\catalog2.pch"

"$(INTDIR)\vc40.idb" : $(SOURCE) $(DEP_CPP_TABPA) "$(INTDIR)"\
 "$(INTDIR)\catalog2.pch"


!ELSEIF  "$(CFG)" == "catalog2 - Win32 UNICODE DLL Release"


"$(INTDIR)\tabpage.obj" : $(SOURCE) $(DEP_CPP_TABPA) "$(INTDIR)"\
 "$(INTDIR)\catalog2.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\settings.cpp
DEP_CPP_SETTI=\
	".\settings.h"\
	".\catalog2.h"\
	".\stdafx.h"\
	

!IF  "$(CFG)" == "catalog2 - Win32 Release"


"$(INTDIR)\settings.obj" : $(SOURCE) $(DEP_CPP_SETTI) "$(INTDIR)"\
 "$(INTDIR)\catalog2.pch"


!ELSEIF  "$(CFG)" == "catalog2 - Win32 Debug"


"$(INTDIR)\settings.obj" : $(SOURCE) $(DEP_CPP_SETTI) "$(INTDIR)"\
 "$(INTDIR)\catalog2.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_SETTI) "$(INTDIR)"\
 "$(INTDIR)\catalog2.pch"

"$(INTDIR)\vc40.idb" : $(SOURCE) $(DEP_CPP_SETTI) "$(INTDIR)"\
 "$(INTDIR)\catalog2.pch"


!ELSEIF  "$(CFG)" == "catalog2 - Win32 UNICODE Debug"


"$(INTDIR)\settings.obj" : $(SOURCE) $(DEP_CPP_SETTI) "$(INTDIR)"\
 "$(INTDIR)\catalog2.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_SETTI) "$(INTDIR)"\
 "$(INTDIR)\catalog2.pch"

"$(INTDIR)\vc40.idb" : $(SOURCE) $(DEP_CPP_SETTI) "$(INTDIR)"\
 "$(INTDIR)\catalog2.pch"


!ELSEIF  "$(CFG)" == "catalog2 - Win32 UNICODE Release"


"$(INTDIR)\settings.obj" : $(SOURCE) $(DEP_CPP_SETTI) "$(INTDIR)"\
 "$(INTDIR)\catalog2.pch"


!ELSEIF  "$(CFG)" == "catalog2 - Win32 DLL Debug"


"$(INTDIR)\settings.obj" : $(SOURCE) $(DEP_CPP_SETTI) "$(INTDIR)"\
 "$(INTDIR)\catalog2.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_SETTI) "$(INTDIR)"\
 "$(INTDIR)\catalog2.pch"

"$(INTDIR)\vc40.idb" : $(SOURCE) $(DEP_CPP_SETTI) "$(INTDIR)"\
 "$(INTDIR)\catalog2.pch"


!ELSEIF  "$(CFG)" == "catalog2 - Win32 DLL Release"


"$(INTDIR)\settings.obj" : $(SOURCE) $(DEP_CPP_SETTI) "$(INTDIR)"\
 "$(INTDIR)\catalog2.pch"


!ELSEIF  "$(CFG)" == "catalog2 - Win32 UNICODE DLL Debug"


"$(INTDIR)\settings.obj" : $(SOURCE) $(DEP_CPP_SETTI) "$(INTDIR)"\
 "$(INTDIR)\catalog2.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_SETTI) "$(INTDIR)"\
 "$(INTDIR)\catalog2.pch"

"$(INTDIR)\vc40.idb" : $(SOURCE) $(DEP_CPP_SETTI) "$(INTDIR)"\
 "$(INTDIR)\catalog2.pch"


!ELSEIF  "$(CFG)" == "catalog2 - Win32 UNICODE DLL Release"


"$(INTDIR)\settings.obj" : $(SOURCE) $(DEP_CPP_SETTI) "$(INTDIR)"\
 "$(INTDIR)\catalog2.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\colpage.cpp
DEP_CPP_COLPA=\
	".\colpage.h"\
	".\catalog2.h"\
	".\stdafx.h"\
	

!IF  "$(CFG)" == "catalog2 - Win32 Release"


"$(INTDIR)\colpage.obj" : $(SOURCE) $(DEP_CPP_COLPA) "$(INTDIR)"\
 "$(INTDIR)\catalog2.pch"


!ELSEIF  "$(CFG)" == "catalog2 - Win32 Debug"


"$(INTDIR)\colpage.obj" : $(SOURCE) $(DEP_CPP_COLPA) "$(INTDIR)"\
 "$(INTDIR)\catalog2.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_COLPA) "$(INTDIR)"\
 "$(INTDIR)\catalog2.pch"

"$(INTDIR)\vc40.idb" : $(SOURCE) $(DEP_CPP_COLPA) "$(INTDIR)"\
 "$(INTDIR)\catalog2.pch"


!ELSEIF  "$(CFG)" == "catalog2 - Win32 UNICODE Debug"


"$(INTDIR)\colpage.obj" : $(SOURCE) $(DEP_CPP_COLPA) "$(INTDIR)"\
 "$(INTDIR)\catalog2.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_COLPA) "$(INTDIR)"\
 "$(INTDIR)\catalog2.pch"

"$(INTDIR)\vc40.idb" : $(SOURCE) $(DEP_CPP_COLPA) "$(INTDIR)"\
 "$(INTDIR)\catalog2.pch"


!ELSEIF  "$(CFG)" == "catalog2 - Win32 UNICODE Release"


"$(INTDIR)\colpage.obj" : $(SOURCE) $(DEP_CPP_COLPA) "$(INTDIR)"\
 "$(INTDIR)\catalog2.pch"


!ELSEIF  "$(CFG)" == "catalog2 - Win32 DLL Debug"


"$(INTDIR)\colpage.obj" : $(SOURCE) $(DEP_CPP_COLPA) "$(INTDIR)"\
 "$(INTDIR)\catalog2.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_COLPA) "$(INTDIR)"\
 "$(INTDIR)\catalog2.pch"

"$(INTDIR)\vc40.idb" : $(SOURCE) $(DEP_CPP_COLPA) "$(INTDIR)"\
 "$(INTDIR)\catalog2.pch"


!ELSEIF  "$(CFG)" == "catalog2 - Win32 DLL Release"


"$(INTDIR)\colpage.obj" : $(SOURCE) $(DEP_CPP_COLPA) "$(INTDIR)"\
 "$(INTDIR)\catalog2.pch"


!ELSEIF  "$(CFG)" == "catalog2 - Win32 UNICODE DLL Debug"


"$(INTDIR)\colpage.obj" : $(SOURCE) $(DEP_CPP_COLPA) "$(INTDIR)"\
 "$(INTDIR)\catalog2.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_COLPA) "$(INTDIR)"\
 "$(INTDIR)\catalog2.pch"

"$(INTDIR)\vc40.idb" : $(SOURCE) $(DEP_CPP_COLPA) "$(INTDIR)"\
 "$(INTDIR)\catalog2.pch"


!ELSEIF  "$(CFG)" == "catalog2 - Win32 UNICODE DLL Release"


"$(INTDIR)\colpage.obj" : $(SOURCE) $(DEP_CPP_COLPA) "$(INTDIR)"\
 "$(INTDIR)\catalog2.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\cat2doc.cpp
DEP_CPP_CAT2D=\
	".\colpage.h"\
	".\tabpage.h"\
	".\cat2doc.h"\
	".\catalog2.h"\
	".\catsets.h"\
	".\stdafx.h"\
	

!IF  "$(CFG)" == "catalog2 - Win32 Release"


"$(INTDIR)\cat2doc.obj" : $(SOURCE) $(DEP_CPP_CAT2D) "$(INTDIR)"\
 "$(INTDIR)\catalog2.pch"


!ELSEIF  "$(CFG)" == "catalog2 - Win32 Debug"


"$(INTDIR)\cat2doc.obj" : $(SOURCE) $(DEP_CPP_CAT2D) "$(INTDIR)"\
 "$(INTDIR)\catalog2.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_CAT2D) "$(INTDIR)"\
 "$(INTDIR)\catalog2.pch"

"$(INTDIR)\vc40.idb" : $(SOURCE) $(DEP_CPP_CAT2D) "$(INTDIR)"\
 "$(INTDIR)\catalog2.pch"


!ELSEIF  "$(CFG)" == "catalog2 - Win32 UNICODE Debug"


"$(INTDIR)\cat2doc.obj" : $(SOURCE) $(DEP_CPP_CAT2D) "$(INTDIR)"\
 "$(INTDIR)\catalog2.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_CAT2D) "$(INTDIR)"\
 "$(INTDIR)\catalog2.pch"

"$(INTDIR)\vc40.idb" : $(SOURCE) $(DEP_CPP_CAT2D) "$(INTDIR)"\
 "$(INTDIR)\catalog2.pch"


!ELSEIF  "$(CFG)" == "catalog2 - Win32 UNICODE Release"


"$(INTDIR)\cat2doc.obj" : $(SOURCE) $(DEP_CPP_CAT2D) "$(INTDIR)"\
 "$(INTDIR)\catalog2.pch"


!ELSEIF  "$(CFG)" == "catalog2 - Win32 DLL Debug"


"$(INTDIR)\cat2doc.obj" : $(SOURCE) $(DEP_CPP_CAT2D) "$(INTDIR)"\
 "$(INTDIR)\catalog2.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_CAT2D) "$(INTDIR)"\
 "$(INTDIR)\catalog2.pch"

"$(INTDIR)\vc40.idb" : $(SOURCE) $(DEP_CPP_CAT2D) "$(INTDIR)"\
 "$(INTDIR)\catalog2.pch"


!ELSEIF  "$(CFG)" == "catalog2 - Win32 DLL Release"


"$(INTDIR)\cat2doc.obj" : $(SOURCE) $(DEP_CPP_CAT2D) "$(INTDIR)"\
 "$(INTDIR)\catalog2.pch"


!ELSEIF  "$(CFG)" == "catalog2 - Win32 UNICODE DLL Debug"


"$(INTDIR)\cat2doc.obj" : $(SOURCE) $(DEP_CPP_CAT2D) "$(INTDIR)"\
 "$(INTDIR)\catalog2.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_CAT2D) "$(INTDIR)"\
 "$(INTDIR)\catalog2.pch"

"$(INTDIR)\vc40.idb" : $(SOURCE) $(DEP_CPP_CAT2D) "$(INTDIR)"\
 "$(INTDIR)\catalog2.pch"


!ELSEIF  "$(CFG)" == "catalog2 - Win32 UNICODE DLL Release"


"$(INTDIR)\cat2doc.obj" : $(SOURCE) $(DEP_CPP_CAT2D) "$(INTDIR)"\
 "$(INTDIR)\catalog2.pch"


!ENDIF 

# End Source File
# End Target
# End Project
################################################################################
