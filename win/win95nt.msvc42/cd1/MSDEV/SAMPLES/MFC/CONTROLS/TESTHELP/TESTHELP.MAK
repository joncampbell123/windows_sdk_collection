# Microsoft Developer Studio Generated NMAKE File, Format Version 4.20
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

!IF "$(CFG)" == ""
CFG=testhelp - Win32 Debug
!MESSAGE No configuration specified.  Defaulting to testhelp - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "testhelp - Win32 Release" && "$(CFG)" !=\
 "testhelp - Win32 Debug" && "$(CFG)" != "testhelp - Win32 Unicode Debug" &&\
 "$(CFG)" != "testhelp - Win32 Unicode Release"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "testhelp.mak" CFG="testhelp - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "testhelp - Win32 Release" (based on\
 "Win32 (x86) Dynamic-Link Library")
!MESSAGE "testhelp - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "testhelp - Win32 Unicode Debug" (based on\
 "Win32 (x86) Dynamic-Link Library")
!MESSAGE "testhelp - Win32 Unicode Release" (based on\
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
# PROP Target_Last_Scanned "testhelp - Win32 Unicode Release"
MTL=mktyplib.exe
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "testhelp - Win32 Release"

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

ALL : "$(OUTDIR)\testhelp.ocx" "$(OUTDIR)\regsvr32.trg"

CLEAN : 
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\testhelp.obj"
	-@erase "$(INTDIR)\testhelp.pch"
	-@erase "$(INTDIR)\testhelp.res"
	-@erase "$(INTDIR)\testhelp.tlb"
	-@erase "$(INTDIR)\ThelpCtl.obj"
	-@erase "$(INTDIR)\ThelpPpg.obj"
	-@erase "$(OUTDIR)\regsvr32.trg"
	-@erase "$(OUTDIR)\testhelp.exp"
	-@erase "$(OUTDIR)\testhelp.lib"
	-@erase "$(OUTDIR)\testhelp.ocx"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_USRDLL" /Yu"stdafx.h" /c
CPP_PROJ=/nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_USRDLL" /Fp"$(INTDIR)/testhelp.pch"\
 /Yu"stdafx.h" /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\Release/
CPP_SBRS=.\.
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/testhelp.res" /d "NDEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/testhelp.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 /nologo /subsystem:windows /dll /machine:I386
LINK32_FLAGS=/nologo /subsystem:windows /dll /incremental:no\
 /pdb:"$(OUTDIR)/testhelp.pdb" /machine:I386 /def:".\testhelp.def"\
 /out:"$(OUTDIR)/testhelp.ocx" /implib:"$(OUTDIR)/testhelp.lib" 
DEF_FILE= \
	".\testhelp.def"
LINK32_OBJS= \
	"$(INTDIR)\StdAfx.obj" \
	"$(INTDIR)\testhelp.obj" \
	"$(INTDIR)\testhelp.res" \
	"$(INTDIR)\ThelpCtl.obj" \
	"$(INTDIR)\ThelpPpg.obj"

"$(OUTDIR)\testhelp.ocx" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

# Begin Custom Build - Registering OLE control...
OutDir=.\Release
TargetPath=.\Release\testhelp.ocx
InputPath=.\Release\testhelp.ocx
SOURCE=$(InputPath)

"$(OutDir)\regsvr32.trg" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   regsvr32 /s /c "$(TargetPath)"
   echo regsvr32 exec. time > "$(OutDir)\regsvr32.trg"

# End Custom Build

!ELSEIF  "$(CFG)" == "testhelp - Win32 Debug"

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

ALL : "$(OUTDIR)\testhelp.ocx" "$(OUTDIR)\regsvr32.trg"

CLEAN : 
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\testhelp.obj"
	-@erase "$(INTDIR)\testhelp.pch"
	-@erase "$(INTDIR)\testhelp.res"
	-@erase "$(INTDIR)\testhelp.tlb"
	-@erase "$(INTDIR)\ThelpCtl.obj"
	-@erase "$(INTDIR)\ThelpPpg.obj"
	-@erase "$(INTDIR)\vc40.idb"
	-@erase "$(INTDIR)\vc40.pdb"
	-@erase "$(OUTDIR)\regsvr32.trg"
	-@erase "$(OUTDIR)\testhelp.exp"
	-@erase "$(OUTDIR)\testhelp.ilk"
	-@erase "$(OUTDIR)\testhelp.lib"
	-@erase "$(OUTDIR)\testhelp.ocx"
	-@erase "$(OUTDIR)\testhelp.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_USRDLL" /Yu"stdafx.h" /c
CPP_PROJ=/nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_USRDLL" /Fp"$(INTDIR)/testhelp.pch"\
 /Yu"stdafx.h" /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\Debug/
CPP_SBRS=.\.
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/testhelp.res" /d "_DEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/testhelp.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /debug /machine:I386
# ADD LINK32 /nologo /subsystem:windows /dll /debug /machine:I386
LINK32_FLAGS=/nologo /subsystem:windows /dll /incremental:yes\
 /pdb:"$(OUTDIR)/testhelp.pdb" /debug /machine:I386 /def:".\testhelp.def"\
 /out:"$(OUTDIR)/testhelp.ocx" /implib:"$(OUTDIR)/testhelp.lib" 
DEF_FILE= \
	".\testhelp.def"
LINK32_OBJS= \
	"$(INTDIR)\StdAfx.obj" \
	"$(INTDIR)\testhelp.obj" \
	"$(INTDIR)\testhelp.res" \
	"$(INTDIR)\ThelpCtl.obj" \
	"$(INTDIR)\ThelpPpg.obj"

"$(OUTDIR)\testhelp.ocx" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

# Begin Custom Build - Registering OLE control...
OutDir=.\Debug
TargetPath=.\Debug\testhelp.ocx
InputPath=.\Debug\testhelp.ocx
SOURCE=$(InputPath)

"$(OutDir)\regsvr32.trg" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   regsvr32 /s /c "$(TargetPath)"
   echo regsvr32 exec. time > "$(OutDir)\regsvr32.trg"

# End Custom Build

!ELSEIF  "$(CFG)" == "testhelp - Win32 Unicode Debug"

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

ALL : "$(OUTDIR)\testhelp.ocx" "$(OUTDIR)\regsvr32.trg"

CLEAN : 
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\testhelp.obj"
	-@erase "$(INTDIR)\testhelp.pch"
	-@erase "$(INTDIR)\testhelp.res"
	-@erase "$(INTDIR)\testhelp.tlb"
	-@erase "$(INTDIR)\ThelpCtl.obj"
	-@erase "$(INTDIR)\ThelpPpg.obj"
	-@erase "$(INTDIR)\vc40.idb"
	-@erase "$(INTDIR)\vc40.pdb"
	-@erase "$(OUTDIR)\regsvr32.trg"
	-@erase "$(OUTDIR)\testhelp.exp"
	-@erase "$(OUTDIR)\testhelp.ilk"
	-@erase "$(OUTDIR)\testhelp.lib"
	-@erase "$(OUTDIR)\testhelp.ocx"
	-@erase "$(OUTDIR)\testhelp.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_USRDLL" /Yu"stdafx.h" /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_USRDLL" /D "_UNICODE" /Yu"stdafx.h" /c
CPP_PROJ=/nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /D "_WINDLL" /D "_AFXDLL" /D "_USRDLL" /D "_UNICODE"\
 /Fp"$(INTDIR)/testhelp.pch" /Yu"stdafx.h" /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\DebugU/
CPP_SBRS=.\.
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/testhelp.res" /d "_DEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/testhelp.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /debug /machine:I386
# ADD LINK32 /nologo /subsystem:windows /dll /debug /machine:I386
LINK32_FLAGS=/nologo /subsystem:windows /dll /incremental:yes\
 /pdb:"$(OUTDIR)/testhelp.pdb" /debug /machine:I386 /def:".\testhelp.def"\
 /out:"$(OUTDIR)/testhelp.ocx" /implib:"$(OUTDIR)/testhelp.lib" 
DEF_FILE= \
	".\testhelp.def"
LINK32_OBJS= \
	"$(INTDIR)\StdAfx.obj" \
	"$(INTDIR)\testhelp.obj" \
	"$(INTDIR)\testhelp.res" \
	"$(INTDIR)\ThelpCtl.obj" \
	"$(INTDIR)\ThelpPpg.obj"

"$(OUTDIR)\testhelp.ocx" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

# Begin Custom Build - Registering OLE control...
OutDir=.\DebugU
TargetPath=.\DebugU\testhelp.ocx
InputPath=.\DebugU\testhelp.ocx
SOURCE=$(InputPath)

"$(OutDir)\regsvr32.trg" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   regsvr32 /s /c "$(TargetPath)"
   echo regsvr32 exec. time > "$(OutDir)\regsvr32.trg"

# End Custom Build

!ELSEIF  "$(CFG)" == "testhelp - Win32 Unicode Release"

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

ALL : "$(OUTDIR)\testhelp.ocx" "$(OUTDIR)\regsvr32.trg"

CLEAN : 
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\testhelp.obj"
	-@erase "$(INTDIR)\testhelp.pch"
	-@erase "$(INTDIR)\testhelp.res"
	-@erase "$(INTDIR)\testhelp.tlb"
	-@erase "$(INTDIR)\ThelpCtl.obj"
	-@erase "$(INTDIR)\ThelpPpg.obj"
	-@erase "$(OUTDIR)\regsvr32.trg"
	-@erase "$(OUTDIR)\testhelp.exp"
	-@erase "$(OUTDIR)\testhelp.lib"
	-@erase "$(OUTDIR)\testhelp.ocx"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_USRDLL" /Yu"stdafx.h" /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_USRDLL" /D "_UNICODE" /Yu"stdafx.h" /c
CPP_PROJ=/nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_WINDLL" /D "_AFXDLL" /D "_USRDLL" /D "_UNICODE" /Fp"$(INTDIR)/testhelp.pch"\
 /Yu"stdafx.h" /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\ReleaseU/
CPP_SBRS=.\.
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/testhelp.res" /d "NDEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/testhelp.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 /nologo /subsystem:windows /dll /machine:I386
LINK32_FLAGS=/nologo /subsystem:windows /dll /incremental:no\
 /pdb:"$(OUTDIR)/testhelp.pdb" /machine:I386 /def:".\testhelp.def"\
 /out:"$(OUTDIR)/testhelp.ocx" /implib:"$(OUTDIR)/testhelp.lib" 
DEF_FILE= \
	".\testhelp.def"
LINK32_OBJS= \
	"$(INTDIR)\StdAfx.obj" \
	"$(INTDIR)\testhelp.obj" \
	"$(INTDIR)\testhelp.res" \
	"$(INTDIR)\ThelpCtl.obj" \
	"$(INTDIR)\ThelpPpg.obj"

"$(OUTDIR)\testhelp.ocx" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

# Begin Custom Build - Registering OLE control...
OutDir=.\ReleaseU
TargetPath=.\ReleaseU\testhelp.ocx
InputPath=.\ReleaseU\testhelp.ocx
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

# Name "testhelp - Win32 Release"
# Name "testhelp - Win32 Debug"
# Name "testhelp - Win32 Unicode Debug"
# Name "testhelp - Win32 Unicode Release"

!IF  "$(CFG)" == "testhelp - Win32 Release"

!ELSEIF  "$(CFG)" == "testhelp - Win32 Debug"

!ELSEIF  "$(CFG)" == "testhelp - Win32 Unicode Debug"

!ELSEIF  "$(CFG)" == "testhelp - Win32 Unicode Release"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\StdAfx.cpp
DEP_CPP_STDAF=\
	".\StdAfx.h"\
	

!IF  "$(CFG)" == "testhelp - Win32 Release"

# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_USRDLL" /Fp"$(INTDIR)/testhelp.pch"\
 /Yc"stdafx.h" /Fo"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\StdAfx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\testhelp.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "testhelp - Win32 Debug"

# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_USRDLL" /Fp"$(INTDIR)/testhelp.pch"\
 /Yc"stdafx.h" /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\StdAfx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\testhelp.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "testhelp - Win32 Unicode Debug"

# ADD BASE CPP /Yc"stdafx.h"
# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /D "_WINDLL" /D "_AFXDLL" /D "_USRDLL" /D "_UNICODE"\
 /Fp"$(INTDIR)/testhelp.pch" /Yc"stdafx.h" /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c\
 $(SOURCE) \
	

"$(INTDIR)\StdAfx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\testhelp.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "testhelp - Win32 Unicode Release"

# ADD BASE CPP /Yc"stdafx.h"
# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_WINDLL" /D "_AFXDLL" /D "_USRDLL" /D "_UNICODE" /Fp"$(INTDIR)/testhelp.pch"\
 /Yc"stdafx.h" /Fo"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\StdAfx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\testhelp.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\testhelp.cpp
DEP_CPP_TESTH=\
	".\StdAfx.h"\
	".\testhelp.h"\
	

!IF  "$(CFG)" == "testhelp - Win32 Release"


"$(INTDIR)\testhelp.obj" : $(SOURCE) $(DEP_CPP_TESTH) "$(INTDIR)"\
 "$(INTDIR)\testhelp.pch"


!ELSEIF  "$(CFG)" == "testhelp - Win32 Debug"


"$(INTDIR)\testhelp.obj" : $(SOURCE) $(DEP_CPP_TESTH) "$(INTDIR)"\
 "$(INTDIR)\testhelp.pch"


!ELSEIF  "$(CFG)" == "testhelp - Win32 Unicode Debug"


"$(INTDIR)\testhelp.obj" : $(SOURCE) $(DEP_CPP_TESTH) "$(INTDIR)"\
 "$(INTDIR)\testhelp.pch"


!ELSEIF  "$(CFG)" == "testhelp - Win32 Unicode Release"


"$(INTDIR)\testhelp.obj" : $(SOURCE) $(DEP_CPP_TESTH) "$(INTDIR)"\
 "$(INTDIR)\testhelp.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\testhelp.def

!IF  "$(CFG)" == "testhelp - Win32 Release"

!ELSEIF  "$(CFG)" == "testhelp - Win32 Debug"

!ELSEIF  "$(CFG)" == "testhelp - Win32 Unicode Debug"

!ELSEIF  "$(CFG)" == "testhelp - Win32 Unicode Release"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\testhelp.rc

!IF  "$(CFG)" == "testhelp - Win32 Release"

DEP_RSC_TESTHE=\
	".\testhelp.ico"\
	".\ThelpCtl.bmp"\
	
NODEP_RSC_TESTHE=\
	".\Release\testhelp.tlb"\
	

"$(INTDIR)\testhelp.res" : $(SOURCE) $(DEP_RSC_TESTHE) "$(INTDIR)"\
 "$(INTDIR)\testhelp.tlb"
   $(RSC) /l 0x409 /fo"$(INTDIR)/testhelp.res" /i "Release" /d "NDEBUG" /d\
 "_AFXDLL" $(SOURCE)


!ELSEIF  "$(CFG)" == "testhelp - Win32 Debug"

DEP_RSC_TESTHE=\
	".\Debug\testhelp.tlb"\
	".\testhelp.ico"\
	".\ThelpCtl.bmp"\
	

"$(INTDIR)\testhelp.res" : $(SOURCE) $(DEP_RSC_TESTHE) "$(INTDIR)"\
 "$(INTDIR)\testhelp.tlb"
   $(RSC) /l 0x409 /fo"$(INTDIR)/testhelp.res" /i "Debug" /d "_DEBUG" /d\
 "_AFXDLL" $(SOURCE)


!ELSEIF  "$(CFG)" == "testhelp - Win32 Unicode Debug"

DEP_RSC_TESTHE=\
	".\testhelp.ico"\
	".\ThelpCtl.bmp"\
	
NODEP_RSC_TESTHE=\
	".\DebugU\testhelp.tlb"\
	

"$(INTDIR)\testhelp.res" : $(SOURCE) $(DEP_RSC_TESTHE) "$(INTDIR)"\
 "$(INTDIR)\testhelp.tlb"
   $(RSC) /l 0x409 /fo"$(INTDIR)/testhelp.res" /i "DebugU" /d "_DEBUG" /d\
 "_AFXDLL" $(SOURCE)


!ELSEIF  "$(CFG)" == "testhelp - Win32 Unicode Release"

DEP_RSC_TESTHE=\
	".\testhelp.ico"\
	".\ThelpCtl.bmp"\
	
NODEP_RSC_TESTHE=\
	".\ReleaseU\testhelp.tlb"\
	

"$(INTDIR)\testhelp.res" : $(SOURCE) $(DEP_RSC_TESTHE) "$(INTDIR)"\
 "$(INTDIR)\testhelp.tlb"
   $(RSC) /l 0x409 /fo"$(INTDIR)/testhelp.res" /i "ReleaseU" /d "NDEBUG" /d\
 "_AFXDLL" $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\testhelp.odl

!IF  "$(CFG)" == "testhelp - Win32 Release"


"$(OUTDIR)\testhelp.tlb" : $(SOURCE) "$(OUTDIR)"
   $(MTL) /nologo /D "NDEBUG" /tlb "$(OUTDIR)/testhelp.tlb" /win32 $(SOURCE)


!ELSEIF  "$(CFG)" == "testhelp - Win32 Debug"


"$(OUTDIR)\testhelp.tlb" : $(SOURCE) "$(OUTDIR)"
   $(MTL) /nologo /D "_DEBUG" /tlb "$(OUTDIR)/testhelp.tlb" /win32 $(SOURCE)


!ELSEIF  "$(CFG)" == "testhelp - Win32 Unicode Debug"


"$(OUTDIR)\testhelp.tlb" : $(SOURCE) "$(OUTDIR)"
   $(MTL) /nologo /D "_DEBUG" /tlb "$(OUTDIR)/testhelp.tlb" /win32 $(SOURCE)


!ELSEIF  "$(CFG)" == "testhelp - Win32 Unicode Release"


"$(OUTDIR)\testhelp.tlb" : $(SOURCE) "$(OUTDIR)"
   $(MTL) /nologo /D "NDEBUG" /tlb "$(OUTDIR)/testhelp.tlb" /win32 $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\ThelpCtl.cpp
DEP_CPP_THELP=\
	".\StdAfx.h"\
	".\testhelp.h"\
	".\ThelpCtl.h"\
	".\ThelpPpg.h"\
	

!IF  "$(CFG)" == "testhelp - Win32 Release"


"$(INTDIR)\ThelpCtl.obj" : $(SOURCE) $(DEP_CPP_THELP) "$(INTDIR)"\
 "$(INTDIR)\testhelp.pch"


!ELSEIF  "$(CFG)" == "testhelp - Win32 Debug"


"$(INTDIR)\ThelpCtl.obj" : $(SOURCE) $(DEP_CPP_THELP) "$(INTDIR)"\
 "$(INTDIR)\testhelp.pch"


!ELSEIF  "$(CFG)" == "testhelp - Win32 Unicode Debug"


"$(INTDIR)\ThelpCtl.obj" : $(SOURCE) $(DEP_CPP_THELP) "$(INTDIR)"\
 "$(INTDIR)\testhelp.pch"


!ELSEIF  "$(CFG)" == "testhelp - Win32 Unicode Release"


"$(INTDIR)\ThelpCtl.obj" : $(SOURCE) $(DEP_CPP_THELP) "$(INTDIR)"\
 "$(INTDIR)\testhelp.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\ThelpPpg.cpp
DEP_CPP_THELPP=\
	".\StdAfx.h"\
	".\testhelp.h"\
	".\ThelpPpg.h"\
	

!IF  "$(CFG)" == "testhelp - Win32 Release"


"$(INTDIR)\ThelpPpg.obj" : $(SOURCE) $(DEP_CPP_THELPP) "$(INTDIR)"\
 "$(INTDIR)\testhelp.pch"


!ELSEIF  "$(CFG)" == "testhelp - Win32 Debug"


"$(INTDIR)\ThelpPpg.obj" : $(SOURCE) $(DEP_CPP_THELPP) "$(INTDIR)"\
 "$(INTDIR)\testhelp.pch"


!ELSEIF  "$(CFG)" == "testhelp - Win32 Unicode Debug"


"$(INTDIR)\ThelpPpg.obj" : $(SOURCE) $(DEP_CPP_THELPP) "$(INTDIR)"\
 "$(INTDIR)\testhelp.pch"


!ELSEIF  "$(CFG)" == "testhelp - Win32 Unicode Release"


"$(INTDIR)\ThelpPpg.obj" : $(SOURCE) $(DEP_CPP_THELPP) "$(INTDIR)"\
 "$(INTDIR)\testhelp.pch"


!ENDIF 

# End Source File
# End Target
# End Project
################################################################################
