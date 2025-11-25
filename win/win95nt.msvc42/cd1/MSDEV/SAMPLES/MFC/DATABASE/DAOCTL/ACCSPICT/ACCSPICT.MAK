# Microsoft Visual C++ Generated NMAKE File, Format Version 40000
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

!IF "$(CFG)" == ""
CFG=AccsPict - Win32 Debug
!MESSAGE No configuration specified.  Defaulting to AccsPict - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "AccsPict - Win32 Release" && "$(CFG)" !=\
 "AccsPict - Win32 Debug" && "$(CFG)" != "AccsPict - Win32 Debug Unicode" &&\
 "$(CFG)" != "AccsPict - Win32 Release Unicode"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "AccsPict.mak" CFG="AccsPict - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "AccsPict - Win32 Release" (based on\
 "Win32 (x86) Dynamic-Link Library")
!MESSAGE "AccsPict - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "AccsPict - Win32 Debug Unicode" (based on\
 "Win32 (x86) Dynamic-Link Library")
!MESSAGE "AccsPict - Win32 Release Unicode" (based on\
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
# PROP Target_Last_Scanned "AccsPict - Win32 Release Unicode"
RSC=rc.exe
MTL=mktyplib.exe
CPP=cl.exe

!IF  "$(CFG)" == "AccsPict - Win32 Release"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir Release
# PROP BASE Intermediate_Dir Release
# PROP BASE Target_Dir
# PROP BASE Target_Ext ocx
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 0
# PROP Output_Dir Release
# PROP Intermediate_Dir Release
# PROP Target_Dir
# PROP Target_Ext ocx
OUTDIR=.\Release
INTDIR=.\Release

ALL : ".\Release\AccsPict.ocx" ".\Release\regsvr32.trg"

CLEAN : 
	-@erase ".\Release\AccsPict.lib"
	-@erase ".\Release\StdAfx.obj"
	-@erase ".\Release\AccsPpg.obj"
	-@erase ".\Release\AccsPict.pch"
	-@erase ".\Release\accspict.obj"
	-@erase ".\Release\AccsCtl.obj"
	-@erase ".\Release\strmdib.obj"
	-@erase ".\Release\accspict.res"
	-@erase ".\Release\AccsPict.tlb"
	-@erase ".\Release\AccsPict.exp"
	-@erase ".\Release\regsvr32.trg"
	-@erase ".\Release\AccsPict.ocx"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
CPP_PROJ=/nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/AccsPict.pch" /Yu"stdafx.h" /Fo"$(INTDIR)/"\
 /c 
CPP_OBJS=.\Release/
CPP_SBRS=
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/accspict.res" /d "NDEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/AccsPict.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 /nologo /subsystem:windows /dll /machine:I386
LINK32_FLAGS=/nologo /subsystem:windows /dll /incremental:no\
 /pdb:"$(OUTDIR)/AccsPict.pdb" /machine:I386 /def:".\AccsPict.def"\
 /out:"$(OUTDIR)/AccsPict.ocx" /implib:"$(OUTDIR)/AccsPict.lib" 
DEF_FILE= \
	".\AccsPict.def"
LINK32_OBJS= \
	".\Release\StdAfx.obj" \
	".\Release\AccsPpg.obj" \
	".\Release\accspict.obj" \
	".\Release\AccsCtl.obj" \
	".\Release\strmdib.obj" \
	".\Release\accspict.res"

".\Release\AccsPict.ocx" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

# Begin Custom Build - Registering OLE control...
OutDir=.\Release
TargetPath=.\Release\AccsPict.ocx
InputPath=.\Release\AccsPict.ocx
SOURCE=$(InputPath)

"$(OutDir)\regsvr32.trg" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   regsvr32 /s /c "$(TargetPath)"
   echo regsvr32 exec. time > "$(OutDir)\regsvr32.trg"

# End Custom Build

!ELSEIF  "$(CFG)" == "AccsPict - Win32 Debug"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir Debug
# PROP BASE Intermediate_Dir Debug
# PROP BASE Target_Dir
# PROP BASE Target_Ext ocx
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 1
# PROP Output_Dir Debug
# PROP Intermediate_Dir Debug
# PROP Target_Dir
# PROP Target_Ext ocx
OUTDIR=.\Debug
INTDIR=.\Debug

ALL : ".\Debug\AccsPict.ocx" ".\Debug\regsvr32.trg"

CLEAN : 
	-@erase ".\Debug\vc40.pdb"
	-@erase ".\Debug\AccsPict.pch"
	-@erase ".\Debug\AccsPict.ilk"
	-@erase ".\Debug\StdAfx.obj"
	-@erase ".\Debug\strmdib.obj"
	-@erase ".\Debug\AccsCtl.obj"
	-@erase ".\Debug\accspict.obj"
	-@erase ".\Debug\AccsPpg.obj"
	-@erase ".\Debug\accspict.res"
	-@erase ".\Debug\accspict.tlb"
	-@erase ".\Debug\AccsPict.lib"
	-@erase ".\Debug\AccsPict.exp"
	-@erase ".\Debug\AccsPict.pdb"
	-@erase ".\Debug\regsvr32.trg"
	-@erase ".\Debug\AccsPict.ocx"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
CPP_PROJ=/nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/AccsPict.pch" /Yu"stdafx.h"\
 /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\Debug/
CPP_SBRS=
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/accspict.res" /d "_DEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/AccsPict.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /debug /machine:I386
# ADD LINK32 /nologo /subsystem:windows /dll /debug /machine:I386
LINK32_FLAGS=/nologo /subsystem:windows /dll /incremental:yes\
 /pdb:"$(OUTDIR)/AccsPict.pdb" /debug /machine:I386 /def:".\AccsPict.def"\
 /out:"$(OUTDIR)/AccsPict.ocx" /implib:"$(OUTDIR)/AccsPict.lib" 
DEF_FILE= \
	".\AccsPict.def"
LINK32_OBJS= \
	".\Debug\StdAfx.obj" \
	".\Debug\strmdib.obj" \
	".\Debug\AccsCtl.obj" \
	".\Debug\accspict.obj" \
	".\Debug\AccsPpg.obj" \
	".\Debug\accspict.res"

".\Debug\AccsPict.ocx" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

# Begin Custom Build - Registering OLE control...
OutDir=.\Debug
TargetPath=.\Debug\AccsPict.ocx
InputPath=.\Debug\AccsPict.ocx
SOURCE=$(InputPath)

"$(OutDir)\regsvr32.trg" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   regsvr32 /s /c "$(TargetPath)"
   echo regsvr32 exec. time > "$(OutDir)\regsvr32.trg"

# End Custom Build

!ELSEIF  "$(CFG)" == "AccsPict - Win32 Debug Unicode"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir DebugU
# PROP BASE Intermediate_Dir DebugU
# PROP BASE Target_Dir
# PROP BASE Target_Ext ocx
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 1
# PROP Output_Dir DebugU
# PROP Intermediate_Dir DebugU
# PROP Target_Dir
# PROP Target_Ext ocx
OUTDIR=.\DebugU
INTDIR=.\DebugU

ALL : ".\DebugU\AccsPict.ocx" ".\DebugU\regsvr32.trg"

CLEAN : 
	-@erase ".\DebugU\vc40.pdb"
	-@erase ".\DebugU\AccsPict.pch"
	-@erase ".\DebugU\AccsPict.ilk"
	-@erase ".\DebugU\strmdib.obj"
	-@erase ".\DebugU\accspict.obj"
	-@erase ".\DebugU\AccsPpg.obj"
	-@erase ".\DebugU\StdAfx.obj"
	-@erase ".\DebugU\AccsCtl.obj"
	-@erase ".\DebugU\accspict.res"
	-@erase ".\DebugU\AccsPict.tlb"
	-@erase ".\DebugU\AccsPict.lib"
	-@erase ".\DebugU\AccsPict.exp"
	-@erase ".\DebugU\AccsPict.pdb"
	-@erase ".\DebugU\regsvr32.trg"
	-@erase ".\DebugU\AccsPict.ocx"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_UNICODE" /Yu"stdafx.h" /c
CPP_PROJ=/nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /D "_AFXDLL" /D "_UNICODE" /Fp"$(INTDIR)/AccsPict.pch" /Yu"stdafx.h"\
 /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\DebugU/
CPP_SBRS=
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/accspict.res" /d "_DEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/AccsPict.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /debug /machine:I386
# ADD LINK32 /nologo /subsystem:windows /dll /debug /machine:I386
LINK32_FLAGS=/nologo /subsystem:windows /dll /incremental:yes\
 /pdb:"$(OUTDIR)/AccsPict.pdb" /debug /machine:I386 /def:".\AccsPict.def"\
 /out:"$(OUTDIR)/AccsPict.ocx" /implib:"$(OUTDIR)/AccsPict.lib" 
DEF_FILE= \
	".\AccsPict.def"
LINK32_OBJS= \
	".\DebugU\strmdib.obj" \
	".\DebugU\accspict.obj" \
	".\DebugU\AccsPpg.obj" \
	".\DebugU\StdAfx.obj" \
	".\DebugU\AccsCtl.obj" \
	".\DebugU\accspict.res"

".\DebugU\AccsPict.ocx" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

# Begin Custom Build - Registering OLE control...
OutDir=.\DebugU
TargetPath=.\DebugU\AccsPict.ocx
InputPath=.\DebugU\AccsPict.ocx
SOURCE=$(InputPath)

"$(OutDir)\regsvr32.trg" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   regsvr32 /s /c "$(TargetPath)"
   echo regsvr32 exec. time > "$(OutDir)\regsvr32.trg"

# End Custom Build

!ELSEIF  "$(CFG)" == "AccsPict - Win32 Release Unicode"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir ReleaseU
# PROP BASE Intermediate_Dir ReleaseU
# PROP BASE Target_Dir
# PROP BASE Target_Ext ocx
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ReleaseU
# PROP Intermediate_Dir ReleaseU
# PROP Target_Dir
# PROP Target_Ext ocx
OUTDIR=.\ReleaseU
INTDIR=.\ReleaseU

ALL : ".\ReleaseU\AccsPict.ocx" ".\ReleaseU\regsvr32.trg"

CLEAN : 
	-@erase ".\ReleaseU\AccsPict.lib"
	-@erase ".\ReleaseU\AccsCtl.obj"
	-@erase ".\ReleaseU\AccsPict.pch"
	-@erase ".\ReleaseU\strmdib.obj"
	-@erase ".\ReleaseU\accspict.obj"
	-@erase ".\ReleaseU\StdAfx.obj"
	-@erase ".\ReleaseU\AccsPpg.obj"
	-@erase ".\ReleaseU\accspict.res"
	-@erase ".\ReleaseU\AccsPict.tlb"
	-@erase ".\ReleaseU\AccsPict.exp"
	-@erase ".\ReleaseU\regsvr32.trg"
	-@erase ".\ReleaseU\AccsPict.ocx"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_UNICODE" /Yu"stdafx.h" /c
CPP_PROJ=/nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_UNICODE" /Fp"$(INTDIR)/AccsPict.pch" /Yu"stdafx.h"\
 /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\ReleaseU/
CPP_SBRS=
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/accspict.res" /d "NDEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/AccsPict.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 /nologo /subsystem:windows /dll /machine:I386
LINK32_FLAGS=/nologo /subsystem:windows /dll /incremental:no\
 /pdb:"$(OUTDIR)/AccsPict.pdb" /machine:I386 /def:".\AccsPict.def"\
 /out:"$(OUTDIR)/AccsPict.ocx" /implib:"$(OUTDIR)/AccsPict.lib" 
DEF_FILE= \
	".\AccsPict.def"
LINK32_OBJS= \
	".\ReleaseU\AccsCtl.obj" \
	".\ReleaseU\strmdib.obj" \
	".\ReleaseU\accspict.obj" \
	".\ReleaseU\StdAfx.obj" \
	".\ReleaseU\AccsPpg.obj" \
	".\ReleaseU\accspict.res"

".\ReleaseU\AccsPict.ocx" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

# Begin Custom Build - Registering OLE control...
OutDir=.\ReleaseU
TargetPath=.\ReleaseU\AccsPict.ocx
InputPath=.\ReleaseU\AccsPict.ocx
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

# Name "AccsPict - Win32 Release"
# Name "AccsPict - Win32 Debug"
# Name "AccsPict - Win32 Debug Unicode"
# Name "AccsPict - Win32 Release Unicode"

!IF  "$(CFG)" == "AccsPict - Win32 Release"

!ELSEIF  "$(CFG)" == "AccsPict - Win32 Debug"

!ELSEIF  "$(CFG)" == "AccsPict - Win32 Debug Unicode"

!ELSEIF  "$(CFG)" == "AccsPict - Win32 Release Unicode"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\ReadMe.txt

!IF  "$(CFG)" == "AccsPict - Win32 Release"

!ELSEIF  "$(CFG)" == "AccsPict - Win32 Debug"

!ELSEIF  "$(CFG)" == "AccsPict - Win32 Debug Unicode"

!ELSEIF  "$(CFG)" == "AccsPict - Win32 Release Unicode"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\StdAfx.cpp
DEP_CPP_STDAF=\
	".\stdafx.h"\
	

!IF  "$(CFG)" == "AccsPict - Win32 Release"

# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/AccsPict.pch" /Yc"stdafx.h" /Fo"$(INTDIR)/"\
 /c $(SOURCE) \
	

".\Release\StdAfx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

".\Release\AccsPict.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "AccsPict - Win32 Debug"

# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/AccsPict.pch" /Yc"stdafx.h"\
 /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c $(SOURCE) \
	

".\Debug\StdAfx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

".\Debug\vc40.pdb" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

".\Debug\AccsPict.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "AccsPict - Win32 Debug Unicode"

# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /D "_AFXDLL" /D "_UNICODE" /Fp"$(INTDIR)/AccsPict.pch" /Yc"stdafx.h"\
 /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c $(SOURCE) \
	

".\DebugU\StdAfx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

".\DebugU\vc40.pdb" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

".\DebugU\AccsPict.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "AccsPict - Win32 Release Unicode"

# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_UNICODE" /Fp"$(INTDIR)/AccsPict.pch" /Yc"stdafx.h"\
 /Fo"$(INTDIR)/" /c $(SOURCE) \
	

".\ReleaseU\StdAfx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

".\ReleaseU\AccsPict.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\accspict.cpp
DEP_CPP_ACCSP=\
	".\accspict.h"\
	".\stdafx.h"\
	

!IF  "$(CFG)" == "AccsPict - Win32 Release"


".\Release\accspict.obj" : $(SOURCE) $(DEP_CPP_ACCSP) "$(INTDIR)"\
 ".\Release\AccsPict.pch"


!ELSEIF  "$(CFG)" == "AccsPict - Win32 Debug"


".\Debug\accspict.obj" : $(SOURCE) $(DEP_CPP_ACCSP) "$(INTDIR)"\
 ".\Debug\AccsPict.pch"

".\Debug\vc40.pdb" : $(SOURCE) $(DEP_CPP_ACCSP) "$(INTDIR)"\
 ".\Debug\AccsPict.pch"


!ELSEIF  "$(CFG)" == "AccsPict - Win32 Debug Unicode"


".\DebugU\accspict.obj" : $(SOURCE) $(DEP_CPP_ACCSP) "$(INTDIR)"\
 ".\DebugU\AccsPict.pch"

".\DebugU\vc40.pdb" : $(SOURCE) $(DEP_CPP_ACCSP) "$(INTDIR)"\
 ".\DebugU\AccsPict.pch"


!ELSEIF  "$(CFG)" == "AccsPict - Win32 Release Unicode"


".\ReleaseU\accspict.obj" : $(SOURCE) $(DEP_CPP_ACCSP) "$(INTDIR)"\
 ".\ReleaseU\AccsPict.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\AccsPict.def

!IF  "$(CFG)" == "AccsPict - Win32 Release"

!ELSEIF  "$(CFG)" == "AccsPict - Win32 Debug"

!ELSEIF  "$(CFG)" == "AccsPict - Win32 Debug Unicode"

!ELSEIF  "$(CFG)" == "AccsPict - Win32 Release Unicode"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\accspict.rc

!IF  "$(CFG)" == "AccsPict - Win32 Release"

DEP_RSC_ACCSPI=\
	".\AccsPict.ico"\
	".\AccsCtl.bmp"\
	".\Release\AccsPict.tlb"\
	

".\Release\accspict.res" : $(SOURCE) $(DEP_RSC_ACCSPI) "$(INTDIR)"\
 ".\Release\AccsPict.tlb"
   $(RSC) /l 0x409 /fo"$(INTDIR)/accspict.res" /i "Release" /d "NDEBUG" /d\
 "_AFXDLL" $(SOURCE)


!ELSEIF  "$(CFG)" == "AccsPict - Win32 Debug"

DEP_RSC_ACCSPI=\
	".\AccsPict.ico"\
	".\AccsCtl.bmp"\
	".\Debug\accspict.tlb"\
	

".\Debug\accspict.res" : $(SOURCE) $(DEP_RSC_ACCSPI) "$(INTDIR)"\
 ".\Debug\accspict.tlb"
   $(RSC) /l 0x409 /fo"$(INTDIR)/accspict.res" /i "Debug" /d "_DEBUG" /d\
 "_AFXDLL" $(SOURCE)


!ELSEIF  "$(CFG)" == "AccsPict - Win32 Debug Unicode"

DEP_RSC_ACCSPI=\
	".\AccsPict.ico"\
	".\AccsCtl.bmp"\
	".\DebugU\AccsPict.tlb"\
	

".\DebugU\accspict.res" : $(SOURCE) $(DEP_RSC_ACCSPI) "$(INTDIR)"\
 ".\DebugU\AccsPict.tlb"
   $(RSC) /l 0x409 /fo"$(INTDIR)/accspict.res" /i "DebugU" /d "_DEBUG" /d\
 "_AFXDLL" $(SOURCE)


!ELSEIF  "$(CFG)" == "AccsPict - Win32 Release Unicode"

DEP_RSC_ACCSPI=\
	".\AccsPict.ico"\
	".\AccsCtl.bmp"\
	".\ReleaseU\AccsPict.tlb"\
	

".\ReleaseU\accspict.res" : $(SOURCE) $(DEP_RSC_ACCSPI) "$(INTDIR)"\
 ".\ReleaseU\AccsPict.tlb"
   $(RSC) /l 0x409 /fo"$(INTDIR)/accspict.res" /i "ReleaseU" /d "NDEBUG" /d\
 "_AFXDLL" $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\accspict.odl

!IF  "$(CFG)" == "AccsPict - Win32 Release"


".\Release\AccsPict.tlb" : $(SOURCE) "$(OUTDIR)"
   $(MTL) /nologo /D "NDEBUG" /tlb "$(OUTDIR)/accspict.tlb" /win32 $(SOURCE)


!ELSEIF  "$(CFG)" == "AccsPict - Win32 Debug"


".\Debug\accspict.tlb" : $(SOURCE) "$(OUTDIR)"
   $(MTL) /nologo /D "_DEBUG" /tlb "$(OUTDIR)/accspict.tlb" /win32 $(SOURCE)


!ELSEIF  "$(CFG)" == "AccsPict - Win32 Debug Unicode"


".\DebugU\AccsPict.tlb" : $(SOURCE) "$(OUTDIR)"
   $(MTL) /nologo /D "_DEBUG" /tlb "$(OUTDIR)/accspict.tlb" /win32 $(SOURCE)


!ELSEIF  "$(CFG)" == "AccsPict - Win32 Release Unicode"


".\ReleaseU\AccsPict.tlb" : $(SOURCE) "$(OUTDIR)"
   $(MTL) /nologo /D "NDEBUG" /tlb "$(OUTDIR)/accspict.tlb" /win32 $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\AccsCtl.cpp
DEP_CPP_ACCSC=\
	".\AccsCtl.h"\
	".\accspict.h"\
	".\AccsPpg.h"\
	".\strmdib.h"\
	".\stdafx.h"\
	

!IF  "$(CFG)" == "AccsPict - Win32 Release"


".\Release\AccsCtl.obj" : $(SOURCE) $(DEP_CPP_ACCSC) "$(INTDIR)"\
 ".\Release\AccsPict.pch"


!ELSEIF  "$(CFG)" == "AccsPict - Win32 Debug"


".\Debug\AccsCtl.obj" : $(SOURCE) $(DEP_CPP_ACCSC) "$(INTDIR)"\
 ".\Debug\AccsPict.pch"

".\Debug\vc40.pdb" : $(SOURCE) $(DEP_CPP_ACCSC) "$(INTDIR)"\
 ".\Debug\AccsPict.pch"


!ELSEIF  "$(CFG)" == "AccsPict - Win32 Debug Unicode"


".\DebugU\AccsCtl.obj" : $(SOURCE) $(DEP_CPP_ACCSC) "$(INTDIR)"\
 ".\DebugU\AccsPict.pch"

".\DebugU\vc40.pdb" : $(SOURCE) $(DEP_CPP_ACCSC) "$(INTDIR)"\
 ".\DebugU\AccsPict.pch"


!ELSEIF  "$(CFG)" == "AccsPict - Win32 Release Unicode"


".\ReleaseU\AccsCtl.obj" : $(SOURCE) $(DEP_CPP_ACCSC) "$(INTDIR)"\
 ".\ReleaseU\AccsPict.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\AccsPpg.cpp
DEP_CPP_ACCSPP=\
	".\accspict.h"\
	".\AccsPpg.h"\
	".\stdafx.h"\
	

!IF  "$(CFG)" == "AccsPict - Win32 Release"


".\Release\AccsPpg.obj" : $(SOURCE) $(DEP_CPP_ACCSPP) "$(INTDIR)"\
 ".\Release\AccsPict.pch"


!ELSEIF  "$(CFG)" == "AccsPict - Win32 Debug"


".\Debug\AccsPpg.obj" : $(SOURCE) $(DEP_CPP_ACCSPP) "$(INTDIR)"\
 ".\Debug\AccsPict.pch"

".\Debug\vc40.pdb" : $(SOURCE) $(DEP_CPP_ACCSPP) "$(INTDIR)"\
 ".\Debug\AccsPict.pch"


!ELSEIF  "$(CFG)" == "AccsPict - Win32 Debug Unicode"


".\DebugU\AccsPpg.obj" : $(SOURCE) $(DEP_CPP_ACCSPP) "$(INTDIR)"\
 ".\DebugU\AccsPict.pch"

".\DebugU\vc40.pdb" : $(SOURCE) $(DEP_CPP_ACCSPP) "$(INTDIR)"\
 ".\DebugU\AccsPict.pch"


!ELSEIF  "$(CFG)" == "AccsPict - Win32 Release Unicode"


".\ReleaseU\AccsPpg.obj" : $(SOURCE) $(DEP_CPP_ACCSPP) "$(INTDIR)"\
 ".\ReleaseU\AccsPict.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\strmdib.cpp
DEP_CPP_STRMD=\
	".\accspict.h"\
	".\strmdib.h"\
	".\stdafx.h"\
	

!IF  "$(CFG)" == "AccsPict - Win32 Release"


".\Release\strmdib.obj" : $(SOURCE) $(DEP_CPP_STRMD) "$(INTDIR)"\
 ".\Release\AccsPict.pch"


!ELSEIF  "$(CFG)" == "AccsPict - Win32 Debug"


".\Debug\strmdib.obj" : $(SOURCE) $(DEP_CPP_STRMD) "$(INTDIR)"\
 ".\Debug\AccsPict.pch"

".\Debug\vc40.pdb" : $(SOURCE) $(DEP_CPP_STRMD) "$(INTDIR)"\
 ".\Debug\AccsPict.pch"


!ELSEIF  "$(CFG)" == "AccsPict - Win32 Debug Unicode"


".\DebugU\strmdib.obj" : $(SOURCE) $(DEP_CPP_STRMD) "$(INTDIR)"\
 ".\DebugU\AccsPict.pch"

".\DebugU\vc40.pdb" : $(SOURCE) $(DEP_CPP_STRMD) "$(INTDIR)"\
 ".\DebugU\AccsPict.pch"


!ELSEIF  "$(CFG)" == "AccsPict - Win32 Release Unicode"


".\ReleaseU\strmdib.obj" : $(SOURCE) $(DEP_CPP_STRMD) "$(INTDIR)"\
 ".\ReleaseU\AccsPict.pch"


!ENDIF 

# End Source File
# End Target
# End Project
################################################################################
