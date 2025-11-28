# Microsoft Developer Studio Generated NMAKE File, Format Version 4.10
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

!IF "$(CFG)" == ""
CFG=foxtlib - Win32 Debug
!MESSAGE No configuration specified.  Defaulting to foxtlib - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "foxtlib - Win32 Release" && "$(CFG)" !=\
 "foxtlib - Win32 Debug" && "$(CFG)" != "foxtlib - Win32 Unicode Debug" &&\
 "$(CFG)" != "foxtlib - Win32 Unicode Release"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "FoxTLib.mak" CFG="foxtlib - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "foxtlib - Win32 Release" (based on\
 "Win32 (x86) Dynamic-Link Library")
!MESSAGE "foxtlib - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "foxtlib - Win32 Unicode Debug" (based on\
 "Win32 (x86) Dynamic-Link Library")
!MESSAGE "foxtlib - Win32 Unicode Release" (based on\
 "Win32 (x86) Dynamic-Link Library")
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
# PROP Target_Last_Scanned "foxtlib - Win32 Debug"
RSC=rc.exe
MTL=mktyplib.exe
CPP=cl.exe

!IF  "$(CFG)" == "foxtlib - Win32 Release"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP BASE Target_Ext "ocx"
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# PROP Target_Ext "ocx"
OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

ALL : "$(OUTDIR)\FoxTLib.ocx" "$(OUTDIR)\FoxTLib.bsc" "$(OUTDIR)\regsvr32.trg"

CLEAN : 
	-@erase "$(INTDIR)\foxtlib.obj"
	-@erase "$(INTDIR)\FoxTLib.pch"
	-@erase "$(INTDIR)\foxtlib.res"
	-@erase "$(INTDIR)\foxtlib.sbr"
	-@erase "$(INTDIR)\foxtlib.tlb"
	-@erase "$(INTDIR)\FoxtlibCtl.obj"
	-@erase "$(INTDIR)\FoxtlibCtl.sbr"
	-@erase "$(INTDIR)\FoxtlibPpg.obj"
	-@erase "$(INTDIR)\FoxtlibPpg.sbr"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\StdAfx.sbr"
	-@erase "$(OUTDIR)\FoxTLib.bsc"
	-@erase "$(OUTDIR)\FoxTLib.exp"
	-@erase "$(OUTDIR)\FoxTLib.lib"
	-@erase "$(OUTDIR)\FoxTLib.ocx"
	-@erase "$(OUTDIR)\regsvr32.trg"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
# ADD CPP /nologo /Gr /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_USRDLL" /FR /Yu"stdafx.h" /c
CPP_PROJ=/nologo /Gr /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_USRDLL" /FR"$(INTDIR)/"\
 /Fp"$(INTDIR)/FoxTLib.pch" /Yu"stdafx.h" /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\Release/
CPP_SBRS=.\Release/
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/foxtlib.res" /d "NDEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/FoxTLib.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\foxtlib.sbr" \
	"$(INTDIR)\FoxtlibCtl.sbr" \
	"$(INTDIR)\FoxtlibPpg.sbr" \
	"$(INTDIR)\StdAfx.sbr"

"$(OUTDIR)\FoxTLib.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 /nologo /subsystem:windows /dll /machine:I386
LINK32_FLAGS=/nologo /subsystem:windows /dll /incremental:no\
 /pdb:"$(OUTDIR)/FoxTLib.pdb" /machine:I386 /def:".\foxtlib.def"\
 /out:"$(OUTDIR)/FoxTLib.ocx" /implib:"$(OUTDIR)/FoxTLib.lib" 
DEF_FILE= \
	".\foxtlib.def"
LINK32_OBJS= \
	"$(INTDIR)\foxtlib.obj" \
	"$(INTDIR)\foxtlib.res" \
	"$(INTDIR)\FoxtlibCtl.obj" \
	"$(INTDIR)\FoxtlibPpg.obj" \
	"$(INTDIR)\StdAfx.obj" \
	".\ocxapi.lib"

"$(OUTDIR)\FoxTLib.ocx" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

# Begin Custom Build - Registering OLE control...
OutDir=.\Release
TargetPath=.\Release\FoxTLib.ocx
InputPath=.\Release\FoxTLib.ocx
SOURCE=$(InputPath)

"$(OutDir)\regsvr32.trg" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   regsvr32 /s /c "$(TargetPath)"
   echo regsvr32 exec. time > "$(OutDir)\regsvr32.trg"

# End Custom Build

!ELSEIF  "$(CFG)" == "foxtlib - Win32 Debug"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP BASE Target_Ext "ocx"
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# PROP Target_Ext "ocx"
OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

ALL : "$(OUTDIR)\FoxTLib.ocx" "$(OUTDIR)\regsvr32.trg"

CLEAN : 
	-@erase "$(INTDIR)\foxtlib.obj"
	-@erase "$(INTDIR)\FoxTLib.pch"
	-@erase "$(INTDIR)\foxtlib.res"
	-@erase "$(INTDIR)\foxtlib.tlb"
	-@erase "$(INTDIR)\FoxtlibCtl.obj"
	-@erase "$(INTDIR)\FoxtlibPpg.obj"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\vc40.idb"
	-@erase "$(INTDIR)\vc40.pdb"
	-@erase "$(OUTDIR)\FoxTLib.exp"
	-@erase "$(OUTDIR)\FoxTLib.ilk"
	-@erase "$(OUTDIR)\FoxTLib.lib"
	-@erase "$(OUTDIR)\FoxTLib.ocx"
	-@erase "$(OUTDIR)\FoxTLib.pdb"
	-@erase "$(OUTDIR)\regsvr32.trg"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
# ADD CPP /nologo /Gr /MD /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_USRDLL" /Yu"stdafx.h" /c
CPP_PROJ=/nologo /Gr /MD /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D\
 "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_USRDLL"\
 /Fp"$(INTDIR)/FoxTLib.pch" /Yu"stdafx.h" /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\Debug/
CPP_SBRS=.\.
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/foxtlib.res" /d "_DEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/FoxTLib.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /debug /machine:I386
# ADD LINK32 /nologo /subsystem:windows /dll /debug /machine:I386 /nodefaultlib:"msvcrt"
LINK32_FLAGS=/nologo /subsystem:windows /dll /incremental:yes\
 /pdb:"$(OUTDIR)/FoxTLib.pdb" /debug /machine:I386 /nodefaultlib:"msvcrt"\
 /def:".\foxtlib.def" /out:"$(OUTDIR)/FoxTLib.ocx"\
 /implib:"$(OUTDIR)/FoxTLib.lib" 
DEF_FILE= \
	".\foxtlib.def"
LINK32_OBJS= \
	"$(INTDIR)\foxtlib.obj" \
	"$(INTDIR)\foxtlib.res" \
	"$(INTDIR)\FoxtlibCtl.obj" \
	"$(INTDIR)\FoxtlibPpg.obj" \
	"$(INTDIR)\StdAfx.obj" \
	".\ocxapi.lib"

"$(OUTDIR)\FoxTLib.ocx" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

# Begin Custom Build - Registering OLE control...
OutDir=.\Debug
TargetPath=.\Debug\FoxTLib.ocx
InputPath=.\Debug\FoxTLib.ocx
SOURCE=$(InputPath)

"$(OutDir)\regsvr32.trg" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   regsvr32 /s /c "$(TargetPath)"
   echo regsvr32 exec. time > "$(OutDir)\regsvr32.trg"

# End Custom Build

!ELSEIF  "$(CFG)" == "foxtlib - Win32 Unicode Debug"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "DebugU"
# PROP BASE Intermediate_Dir "DebugU"
# PROP BASE Target_Dir ""
# PROP BASE Target_Ext "ocx"
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "DebugU"
# PROP Intermediate_Dir "DebugU"
# PROP Target_Dir ""
# PROP Target_Ext "ocx"
OUTDIR=.\DebugU
INTDIR=.\DebugU
# Begin Custom Macros
OutDir=.\DebugU
# End Custom Macros

ALL : "$(OUTDIR)\FoxTLib.ocx" "$(OUTDIR)\regsvr32.trg"

CLEAN : 
	-@erase "$(INTDIR)\foxtlib.obj"
	-@erase "$(INTDIR)\FoxTLib.pch"
	-@erase "$(INTDIR)\foxtlib.res"
	-@erase "$(INTDIR)\foxtlib.tlb"
	-@erase "$(INTDIR)\FoxtlibCtl.obj"
	-@erase "$(INTDIR)\FoxtlibPpg.obj"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\vc40.idb"
	-@erase "$(INTDIR)\vc40.pdb"
	-@erase "$(OUTDIR)\FoxTLib.exp"
	-@erase "$(OUTDIR)\FoxTLib.ilk"
	-@erase "$(OUTDIR)\FoxTLib.lib"
	-@erase "$(OUTDIR)\FoxTLib.ocx"
	-@erase "$(OUTDIR)\FoxTLib.pdb"
	-@erase "$(OUTDIR)\regsvr32.trg"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_USRDLL" /Yu"stdafx.h" /c
# ADD CPP /nologo /Gr /MD /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_USRDLL" /D "_UNICODE" /Yu"stdafx.h" /c
CPP_PROJ=/nologo /Gr /MD /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D\
 "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_USRDLL" /D "_UNICODE"\
 /Fp"$(INTDIR)/FoxTLib.pch" /Yu"stdafx.h" /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\DebugU/
CPP_SBRS=.\.
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/foxtlib.res" /d "_DEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/FoxTLib.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /debug /machine:I386
# ADD LINK32 /nologo /subsystem:windows /dll /debug /machine:I386 /nodefaultlib:"msvcrt"
LINK32_FLAGS=/nologo /subsystem:windows /dll /incremental:yes\
 /pdb:"$(OUTDIR)/FoxTLib.pdb" /debug /machine:I386 /nodefaultlib:"msvcrt"\
 /def:".\foxtlib.def" /out:"$(OUTDIR)/FoxTLib.ocx"\
 /implib:"$(OUTDIR)/FoxTLib.lib" 
DEF_FILE= \
	".\foxtlib.def"
LINK32_OBJS= \
	"$(INTDIR)\foxtlib.obj" \
	"$(INTDIR)\foxtlib.res" \
	"$(INTDIR)\FoxtlibCtl.obj" \
	"$(INTDIR)\FoxtlibPpg.obj" \
	"$(INTDIR)\StdAfx.obj" \
	".\ocxapi.lib"

"$(OUTDIR)\FoxTLib.ocx" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

# Begin Custom Build - Registering OLE control...
OutDir=.\DebugU
TargetPath=.\DebugU\FoxTLib.ocx
InputPath=.\DebugU\FoxTLib.ocx
SOURCE=$(InputPath)

"$(OutDir)\regsvr32.trg" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   regsvr32 /s /c "$(TargetPath)"
   echo regsvr32 exec. time > "$(OutDir)\regsvr32.trg"

# End Custom Build

!ELSEIF  "$(CFG)" == "foxtlib - Win32 Unicode Release"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "ReleaseU"
# PROP BASE Intermediate_Dir "ReleaseU"
# PROP BASE Target_Dir ""
# PROP BASE Target_Ext "ocx"
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "ReleaseU"
# PROP Intermediate_Dir "ReleaseU"
# PROP Target_Dir ""
# PROP Target_Ext "ocx"
OUTDIR=.\ReleaseU
INTDIR=.\ReleaseU
# Begin Custom Macros
OutDir=.\ReleaseU
# End Custom Macros

ALL : "$(OUTDIR)\FoxTLib.ocx" "$(OUTDIR)\regsvr32.trg"

CLEAN : 
	-@erase "$(INTDIR)\foxtlib.obj"
	-@erase "$(INTDIR)\FoxTLib.pch"
	-@erase "$(INTDIR)\foxtlib.res"
	-@erase "$(INTDIR)\foxtlib.tlb"
	-@erase "$(INTDIR)\FoxtlibCtl.obj"
	-@erase "$(INTDIR)\FoxtlibPpg.obj"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(OUTDIR)\FoxTLib.exp"
	-@erase "$(OUTDIR)\FoxTLib.lib"
	-@erase "$(OUTDIR)\FoxTLib.ocx"
	-@erase "$(OUTDIR)\regsvr32.trg"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_USRDLL" /Yu"stdafx.h" /c
# ADD CPP /nologo /Gr /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_USRDLL" /D "_UNICODE" /Yu"stdafx.h" /c
CPP_PROJ=/nologo /Gr /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_WINDLL" /D "_AFXDLL" /D "_USRDLL" /D "_UNICODE" /Fp"$(INTDIR)/FoxTLib.pch"\
 /Yu"stdafx.h" /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\ReleaseU/
CPP_SBRS=.\.
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/foxtlib.res" /d "NDEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/FoxTLib.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 /nologo /subsystem:windows /dll /machine:I386 /nodefaultlib:"msvcrt"
LINK32_FLAGS=/nologo /subsystem:windows /dll /incremental:no\
 /pdb:"$(OUTDIR)/FoxTLib.pdb" /machine:I386 /nodefaultlib:"msvcrt"\
 /def:".\foxtlib.def" /out:"$(OUTDIR)/FoxTLib.ocx"\
 /implib:"$(OUTDIR)/FoxTLib.lib" 
DEF_FILE= \
	".\foxtlib.def"
LINK32_OBJS= \
	"$(INTDIR)\foxtlib.obj" \
	"$(INTDIR)\foxtlib.res" \
	"$(INTDIR)\FoxtlibCtl.obj" \
	"$(INTDIR)\FoxtlibPpg.obj" \
	"$(INTDIR)\StdAfx.obj" \
	".\ocxapi.lib"

"$(OUTDIR)\FoxTLib.ocx" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

# Begin Custom Build - Registering OLE control...
OutDir=.\ReleaseU
TargetPath=.\ReleaseU\FoxTLib.ocx
InputPath=.\ReleaseU\FoxTLib.ocx
SOURCE=$(InputPath)

"$(OutDir)\regsvr32.trg" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   regsvr32 /s /c "$(TargetPath)"
   echo regsvr32 exec. time > "$(OutDir)\regsvr32.trg"

# End Custom Build

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

# Name "foxtlib - Win32 Release"
# Name "foxtlib - Win32 Debug"
# Name "foxtlib - Win32 Unicode Debug"
# Name "foxtlib - Win32 Unicode Release"

!IF  "$(CFG)" == "foxtlib - Win32 Release"

!ELSEIF  "$(CFG)" == "foxtlib - Win32 Debug"

!ELSEIF  "$(CFG)" == "foxtlib - Win32 Unicode Debug"

!ELSEIF  "$(CFG)" == "foxtlib - Win32 Unicode Release"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\ReadMe.txt

!IF  "$(CFG)" == "foxtlib - Win32 Release"

!ELSEIF  "$(CFG)" == "foxtlib - Win32 Debug"

!ELSEIF  "$(CFG)" == "foxtlib - Win32 Unicode Debug"

!ELSEIF  "$(CFG)" == "foxtlib - Win32 Unicode Release"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\StdAfx.cpp
DEP_CPP_STDAF=\
	".\StdAfx.h"\
	

!IF  "$(CFG)" == "foxtlib - Win32 Release"

# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /Gr /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_USRDLL" /FR"$(INTDIR)/"\
 /Fp"$(INTDIR)/FoxTLib.pch" /Yc"stdafx.h" /Fo"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\StdAfx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\StdAfx.sbr" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\FoxTLib.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "foxtlib - Win32 Debug"

# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /Gr /MD /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D\
 "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_USRDLL"\
 /Fp"$(INTDIR)/FoxTLib.pch" /Yc"stdafx.h" /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c\
 $(SOURCE) \
	

"$(INTDIR)\StdAfx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\FoxTLib.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "foxtlib - Win32 Unicode Debug"

# ADD BASE CPP /Yc"stdafx.h"
# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /Gr /MD /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D\
 "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_USRDLL" /D "_UNICODE"\
 /Fp"$(INTDIR)/FoxTLib.pch" /Yc"stdafx.h" /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c\
 $(SOURCE) \
	

"$(INTDIR)\StdAfx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\FoxTLib.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "foxtlib - Win32 Unicode Release"

# ADD BASE CPP /Yc"stdafx.h"
# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /Gr /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_WINDLL" /D "_AFXDLL" /D "_USRDLL" /D "_UNICODE" /Fp"$(INTDIR)/FoxTLib.pch"\
 /Yc"stdafx.h" /Fo"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\StdAfx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\FoxTLib.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\foxtlib.cpp
DEP_CPP_FOXTL=\
	".\foxtlib.h"\
	".\StdAfx.h"\
	

!IF  "$(CFG)" == "foxtlib - Win32 Release"


"$(INTDIR)\foxtlib.obj" : $(SOURCE) $(DEP_CPP_FOXTL) "$(INTDIR)"\
 "$(INTDIR)\FoxTLib.pch"

"$(INTDIR)\foxtlib.sbr" : $(SOURCE) $(DEP_CPP_FOXTL) "$(INTDIR)"\
 "$(INTDIR)\FoxTLib.pch"


!ELSEIF  "$(CFG)" == "foxtlib - Win32 Debug"


"$(INTDIR)\foxtlib.obj" : $(SOURCE) $(DEP_CPP_FOXTL) "$(INTDIR)"\
 "$(INTDIR)\FoxTLib.pch"


!ELSEIF  "$(CFG)" == "foxtlib - Win32 Unicode Debug"


"$(INTDIR)\foxtlib.obj" : $(SOURCE) $(DEP_CPP_FOXTL) "$(INTDIR)"\
 "$(INTDIR)\FoxTLib.pch"


!ELSEIF  "$(CFG)" == "foxtlib - Win32 Unicode Release"


"$(INTDIR)\foxtlib.obj" : $(SOURCE) $(DEP_CPP_FOXTL) "$(INTDIR)"\
 "$(INTDIR)\FoxTLib.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\foxtlib.def

!IF  "$(CFG)" == "foxtlib - Win32 Release"

!ELSEIF  "$(CFG)" == "foxtlib - Win32 Debug"

!ELSEIF  "$(CFG)" == "foxtlib - Win32 Unicode Debug"

!ELSEIF  "$(CFG)" == "foxtlib - Win32 Unicode Release"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\foxtlib.rc

!IF  "$(CFG)" == "foxtlib - Win32 Release"

DEP_RSC_FOXTLI=\
	".\FoxTLibCtl.bmp"\
	
NODEP_RSC_FOXTLI=\
	".\Release\foxtlib.tlb"\
	

"$(INTDIR)\foxtlib.res" : $(SOURCE) $(DEP_RSC_FOXTLI) "$(INTDIR)"\
 "$(INTDIR)\foxtlib.tlb"
   $(RSC) /l 0x409 /fo"$(INTDIR)/foxtlib.res" /i "Release" /d "NDEBUG" /d\
 "_AFXDLL" $(SOURCE)


!ELSEIF  "$(CFG)" == "foxtlib - Win32 Debug"

DEP_RSC_FOXTLI=\
	".\Debug\foxtlib.tlb"\
	".\FoxTLibCtl.bmp"\
	

"$(INTDIR)\foxtlib.res" : $(SOURCE) $(DEP_RSC_FOXTLI) "$(INTDIR)"\
 "$(INTDIR)\foxtlib.tlb"
   $(RSC) /l 0x409 /fo"$(INTDIR)/foxtlib.res" /i "Debug" /d "_DEBUG" /d\
 "_AFXDLL" $(SOURCE)


!ELSEIF  "$(CFG)" == "foxtlib - Win32 Unicode Debug"

DEP_RSC_FOXTLI=\
	".\FoxTLibCtl.bmp"\
	
NODEP_RSC_FOXTLI=\
	".\DebugU\foxtlib.tlb"\
	

"$(INTDIR)\foxtlib.res" : $(SOURCE) $(DEP_RSC_FOXTLI) "$(INTDIR)"\
 "$(INTDIR)\foxtlib.tlb"
   $(RSC) /l 0x409 /fo"$(INTDIR)/foxtlib.res" /i "DebugU" /d "_DEBUG" /d\
 "_AFXDLL" $(SOURCE)


!ELSEIF  "$(CFG)" == "foxtlib - Win32 Unicode Release"

DEP_RSC_FOXTLI=\
	".\FoxTLibCtl.bmp"\
	
NODEP_RSC_FOXTLI=\
	".\ReleaseU\foxtlib.tlb"\
	

"$(INTDIR)\foxtlib.res" : $(SOURCE) $(DEP_RSC_FOXTLI) "$(INTDIR)"\
 "$(INTDIR)\foxtlib.tlb"
   $(RSC) /l 0x409 /fo"$(INTDIR)/foxtlib.res" /i "ReleaseU" /d "NDEBUG" /d\
 "_AFXDLL" $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\foxtlib.odl

!IF  "$(CFG)" == "foxtlib - Win32 Release"


"$(OUTDIR)\foxtlib.tlb" : $(SOURCE) "$(OUTDIR)"
   $(MTL) /nologo /D "NDEBUG" /tlb "$(OUTDIR)/foxtlib.tlb" /win32 $(SOURCE)


!ELSEIF  "$(CFG)" == "foxtlib - Win32 Debug"


"$(OUTDIR)\foxtlib.tlb" : $(SOURCE) "$(OUTDIR)"
   $(MTL) /nologo /D "_DEBUG" /tlb "$(OUTDIR)/foxtlib.tlb" /win32 $(SOURCE)


!ELSEIF  "$(CFG)" == "foxtlib - Win32 Unicode Debug"


"$(OUTDIR)\foxtlib.tlb" : $(SOURCE) "$(OUTDIR)"
   $(MTL) /nologo /D "_DEBUG" /tlb "$(OUTDIR)/foxtlib.tlb" /win32 $(SOURCE)


!ELSEIF  "$(CFG)" == "foxtlib - Win32 Unicode Release"


"$(OUTDIR)\foxtlib.tlb" : $(SOURCE) "$(OUTDIR)"
   $(MTL) /nologo /D "NDEBUG" /tlb "$(OUTDIR)/foxtlib.tlb" /win32 $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\FoxtlibCtl.cpp
DEP_CPP_FOXTLIB=\
	".\foxtlib.h"\
	".\FoxtlibCtl.h"\
	".\FoxtlibPpg.h"\
	".\pro_ext.h"\
	".\StdAfx.h"\
	

!IF  "$(CFG)" == "foxtlib - Win32 Release"


"$(INTDIR)\FoxtlibCtl.obj" : $(SOURCE) $(DEP_CPP_FOXTLIB) "$(INTDIR)"\
 "$(INTDIR)\FoxTLib.pch"

"$(INTDIR)\FoxtlibCtl.sbr" : $(SOURCE) $(DEP_CPP_FOXTLIB) "$(INTDIR)"\
 "$(INTDIR)\FoxTLib.pch"


!ELSEIF  "$(CFG)" == "foxtlib - Win32 Debug"


"$(INTDIR)\FoxtlibCtl.obj" : $(SOURCE) $(DEP_CPP_FOXTLIB) "$(INTDIR)"\
 "$(INTDIR)\FoxTLib.pch"


!ELSEIF  "$(CFG)" == "foxtlib - Win32 Unicode Debug"


"$(INTDIR)\FoxtlibCtl.obj" : $(SOURCE) $(DEP_CPP_FOXTLIB) "$(INTDIR)"\
 "$(INTDIR)\FoxTLib.pch"


!ELSEIF  "$(CFG)" == "foxtlib - Win32 Unicode Release"


"$(INTDIR)\FoxtlibCtl.obj" : $(SOURCE) $(DEP_CPP_FOXTLIB) "$(INTDIR)"\
 "$(INTDIR)\FoxTLib.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\FoxtlibPpg.cpp
DEP_CPP_FOXTLIBP=\
	".\foxtlib.h"\
	".\FoxtlibPpg.h"\
	".\StdAfx.h"\
	

!IF  "$(CFG)" == "foxtlib - Win32 Release"


"$(INTDIR)\FoxtlibPpg.obj" : $(SOURCE) $(DEP_CPP_FOXTLIBP) "$(INTDIR)"\
 "$(INTDIR)\FoxTLib.pch"

"$(INTDIR)\FoxtlibPpg.sbr" : $(SOURCE) $(DEP_CPP_FOXTLIBP) "$(INTDIR)"\
 "$(INTDIR)\FoxTLib.pch"


!ELSEIF  "$(CFG)" == "foxtlib - Win32 Debug"


"$(INTDIR)\FoxtlibPpg.obj" : $(SOURCE) $(DEP_CPP_FOXTLIBP) "$(INTDIR)"\
 "$(INTDIR)\FoxTLib.pch"


!ELSEIF  "$(CFG)" == "foxtlib - Win32 Unicode Debug"


"$(INTDIR)\FoxtlibPpg.obj" : $(SOURCE) $(DEP_CPP_FOXTLIBP) "$(INTDIR)"\
 "$(INTDIR)\FoxTLib.pch"


!ELSEIF  "$(CFG)" == "foxtlib - Win32 Unicode Release"


"$(INTDIR)\FoxtlibPpg.obj" : $(SOURCE) $(DEP_CPP_FOXTLIBP) "$(INTDIR)"\
 "$(INTDIR)\FoxTLib.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\ocxapi.lib

!IF  "$(CFG)" == "foxtlib - Win32 Release"

!ELSEIF  "$(CFG)" == "foxtlib - Win32 Debug"

!ELSEIF  "$(CFG)" == "foxtlib - Win32 Unicode Debug"

!ELSEIF  "$(CFG)" == "foxtlib - Win32 Unicode Release"

!ENDIF 

# End Source File
# End Target
# End Project
################################################################################
