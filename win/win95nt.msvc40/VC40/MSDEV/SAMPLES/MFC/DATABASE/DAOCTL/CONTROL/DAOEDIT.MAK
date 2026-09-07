# Microsoft Visual C++ Generated NMAKE File, Format Version 40000
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

!IF "$(CFG)" == ""
CFG=DaoEdit - Win32 Debug
!MESSAGE No configuration specified.  Defaulting to DaoEdit - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "DaoEdit - Win32 Release" && "$(CFG)" !=\
 "DaoEdit - Win32 Debug" && "$(CFG)" != "DaoEdit - Win32 Debug Unicode" &&\
 "$(CFG)" != "DaoEdit - Win32 Release Unicode"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "DaoEdit.mak" CFG="DaoEdit - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "DaoEdit - Win32 Release" (based on\
 "Win32 (x86) Dynamic-Link Library")
!MESSAGE "DaoEdit - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "DaoEdit - Win32 Debug Unicode" (based on\
 "Win32 (x86) Dynamic-Link Library")
!MESSAGE "DaoEdit - Win32 Release Unicode" (based on\
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
# PROP Target_Last_Scanned "DaoEdit - Win32 Release Unicode"
RSC=rc.exe
CPP=cl.exe
MTL=mktyplib.exe

!IF  "$(CFG)" == "DaoEdit - Win32 Release"

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

ALL : "$(OUTDIR)\DaoEdit.ocx" "$(OUTDIR)\regsvr32.trg"

CLEAN : 
	-@erase ".\Release\DaoEdit.lib"
	-@erase ".\Release\StdAfx.obj"
	-@erase ".\Release\DaoEdit.obj"
	-@erase ".\Release\DaoEdit.pch"
	-@erase ".\Release\DaoEdit.res"
	-@erase ".\Release\DaoEdit.tlb"
	-@erase ".\Release\daoppg.obj"
	-@erase ".\Release\daoctl.obj"
	-@erase ".\Release\DaoEdit.exp"
	-@erase ".\Release\regsvr32.trg"
	-@erase ".\Release\DaoEdit.ocx"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
CPP_PROJ=/nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/DaoEdit.pch" /Yu"stdafx.h" /Fo"$(INTDIR)/"\
 /c 
CPP_OBJS=.\Release/
CPP_SBRS=
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/DaoEdit.res" /d "NDEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/DaoEdit.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 /nologo /subsystem:windows /dll /machine:I386
LINK32_FLAGS=/nologo /subsystem:windows /dll /incremental:no\
 /pdb:"$(OUTDIR)/DaoEdit.pdb" /machine:I386 /def:".\DaoEdit.def"\
 /out:"$(OUTDIR)/DaoEdit.ocx" /implib:"$(OUTDIR)/DaoEdit.lib" 
DEF_FILE= \
	".\DaoEdit.def"
LINK32_OBJS= \
	"$(INTDIR)/StdAfx.obj" \
	"$(INTDIR)/DaoEdit.obj" \
	"$(INTDIR)/daoppg.obj" \
	"$(INTDIR)/daoctl.obj" \
	"$(INTDIR)/DaoEdit.res"

"$(OUTDIR)\DaoEdit.ocx" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

# Begin Custom Build - Registering OLE control...
OutDir=.\Release
TargetPath=.\Release\DaoEdit.ocx
InputPath=.\Release\DaoEdit.ocx
SOURCE=$(InputPath)

"$(OutDir)\regsvr32.trg" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   regsvr32 /s /c "$(TargetPath)"
   echo regsvr32 exec. time > "$(OutDir)\regsvr32.trg"

# End Custom Build

!ELSEIF  "$(CFG)" == "DaoEdit - Win32 Debug"

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

ALL : "$(OUTDIR)\DaoEdit.ocx" "$(OUTDIR)\regsvr32.trg"

CLEAN : 
	-@erase ".\Debug\vc40.pdb"
	-@erase ".\Debug\DaoEdit.pch"
	-@erase ".\Debug\DaoEdit.ilk"
	-@erase ".\Debug\StdAfx.obj"
	-@erase ".\Debug\DaoEdit.obj"
	-@erase ".\Debug\DaoEdit.res"
	-@erase ".\Debug\DaoEdit.tlb"
	-@erase ".\Debug\daoppg.obj"
	-@erase ".\Debug\daoctl.obj"
	-@erase ".\Debug\DaoEdit.lib"
	-@erase ".\Debug\DaoEdit.exp"
	-@erase ".\Debug\DaoEdit.pdb"
	-@erase ".\Debug\regsvr32.trg"
	-@erase ".\Debug\DaoEdit.ocx"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
CPP_PROJ=/nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/DaoEdit.pch" /Yu"stdafx.h"\
 /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\Debug/
CPP_SBRS=
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/DaoEdit.res" /d "_DEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/DaoEdit.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /debug /machine:I386
# ADD LINK32 /nologo /subsystem:windows /dll /debug /machine:I386
LINK32_FLAGS=/nologo /subsystem:windows /dll /incremental:yes\
 /pdb:"$(OUTDIR)/DaoEdit.pdb" /debug /machine:I386 /def:".\DaoEdit.def"\
 /out:"$(OUTDIR)/DaoEdit.ocx" /implib:"$(OUTDIR)/DaoEdit.lib" 
DEF_FILE= \
	".\DaoEdit.def"
LINK32_OBJS= \
	"$(INTDIR)/StdAfx.obj" \
	"$(INTDIR)/DaoEdit.obj" \
	"$(INTDIR)/daoppg.obj" \
	"$(INTDIR)/daoctl.obj" \
	"$(INTDIR)/DaoEdit.res"

"$(OUTDIR)\DaoEdit.ocx" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

# Begin Custom Build - Registering OLE control...
OutDir=.\Debug
TargetPath=.\Debug\DaoEdit.ocx
InputPath=.\Debug\DaoEdit.ocx
SOURCE=$(InputPath)

"$(OutDir)\regsvr32.trg" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   regsvr32 /s /c "$(TargetPath)"
   echo regsvr32 exec. time > "$(OutDir)\regsvr32.trg"

# End Custom Build

!ELSEIF  "$(CFG)" == "DaoEdit - Win32 Debug Unicode"

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

ALL : "$(OUTDIR)\DaoEdit.ocx" "$(OUTDIR)\regsvr32.trg"

CLEAN : 
	-@erase ".\DebugU\vc40.pdb"
	-@erase ".\DebugU\DaoEdit.pch"
	-@erase ".\DebugU\DaoEdit.ilk"
	-@erase ".\DebugU\DaoEdit.obj"
	-@erase ".\DebugU\StdAfx.obj"
	-@erase ".\DebugU\DaoEdit.res"
	-@erase ".\DebugU\DaoEdit.tlb"
	-@erase ".\DebugU\daoppg.obj"
	-@erase ".\DebugU\daoctl.obj"
	-@erase ".\DebugU\DaoEdit.lib"
	-@erase ".\DebugU\DaoEdit.exp"
	-@erase ".\DebugU\DaoEdit.pdb"
	-@erase ".\DebugU\regsvr32.trg"
	-@erase ".\DebugU\DaoEdit.ocx"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_UNICODE" /Yu"stdafx.h" /c
CPP_PROJ=/nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /D "_AFXDLL" /D "_UNICODE" /Fp"$(INTDIR)/DaoEdit.pch" /Yu"stdafx.h"\
 /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\DebugU/
CPP_SBRS=
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/DaoEdit.res" /d "_DEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/DaoEdit.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /debug /machine:I386
# ADD LINK32 /nologo /subsystem:windows /dll /debug /machine:I386
LINK32_FLAGS=/nologo /subsystem:windows /dll /incremental:yes\
 /pdb:"$(OUTDIR)/DaoEdit.pdb" /debug /machine:I386 /def:".\DaoEdit.def"\
 /out:"$(OUTDIR)/DaoEdit.ocx" /implib:"$(OUTDIR)/DaoEdit.lib" 
DEF_FILE= \
	".\DaoEdit.def"
LINK32_OBJS= \
	"$(INTDIR)/DaoEdit.obj" \
	"$(INTDIR)/StdAfx.obj" \
	"$(INTDIR)/daoppg.obj" \
	"$(INTDIR)/daoctl.obj" \
	"$(INTDIR)/DaoEdit.res"

"$(OUTDIR)\DaoEdit.ocx" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

# Begin Custom Build - Registering OLE control...
OutDir=.\DebugU
TargetPath=.\DebugU\DaoEdit.ocx
InputPath=.\DebugU\DaoEdit.ocx
SOURCE=$(InputPath)

"$(OutDir)\regsvr32.trg" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   regsvr32 /s /c "$(TargetPath)"
   echo regsvr32 exec. time > "$(OutDir)\regsvr32.trg"

# End Custom Build

!ELSEIF  "$(CFG)" == "DaoEdit - Win32 Release Unicode"

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

ALL : "$(OUTDIR)\DaoEdit.ocx" "$(OUTDIR)\regsvr32.trg"

CLEAN : 
	-@erase ".\ReleaseU\DaoEdit.lib"
	-@erase ".\ReleaseU\StdAfx.obj"
	-@erase ".\ReleaseU\DaoEdit.obj"
	-@erase ".\ReleaseU\DaoEdit.pch"
	-@erase ".\ReleaseU\DaoEdit.res"
	-@erase ".\ReleaseU\DaoEdit.tlb"
	-@erase ".\ReleaseU\daoppg.obj"
	-@erase ".\ReleaseU\daoctl.obj"
	-@erase ".\ReleaseU\DaoEdit.exp"
	-@erase ".\ReleaseU\regsvr32.trg"
	-@erase ".\ReleaseU\DaoEdit.ocx"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_UNICODE" /Yu"stdafx.h" /c
# SUBTRACT CPP /Fr
CPP_PROJ=/nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_UNICODE" /Fp"$(INTDIR)/DaoEdit.pch" /Yu"stdafx.h"\
 /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\ReleaseU/
CPP_SBRS=
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/DaoEdit.res" /d "NDEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/DaoEdit.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 /nologo /subsystem:windows /dll /machine:I386
LINK32_FLAGS=/nologo /subsystem:windows /dll /incremental:no\
 /pdb:"$(OUTDIR)/DaoEdit.pdb" /machine:I386 /def:".\DaoEdit.def"\
 /out:"$(OUTDIR)/DaoEdit.ocx" /implib:"$(OUTDIR)/DaoEdit.lib" 
DEF_FILE= \
	".\DaoEdit.def"
LINK32_OBJS= \
	"$(INTDIR)/StdAfx.obj" \
	"$(INTDIR)/DaoEdit.obj" \
	"$(INTDIR)/daoppg.obj" \
	"$(INTDIR)/daoctl.obj" \
	"$(INTDIR)/DaoEdit.res"

"$(OUTDIR)\DaoEdit.ocx" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

# Begin Custom Build - Registering OLE control...
OutDir=.\ReleaseU
TargetPath=.\ReleaseU\DaoEdit.ocx
InputPath=.\ReleaseU\DaoEdit.ocx
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

# Name "DaoEdit - Win32 Release"
# Name "DaoEdit - Win32 Debug"
# Name "DaoEdit - Win32 Debug Unicode"
# Name "DaoEdit - Win32 Release Unicode"

!IF  "$(CFG)" == "DaoEdit - Win32 Release"

!ELSEIF  "$(CFG)" == "DaoEdit - Win32 Debug"

!ELSEIF  "$(CFG)" == "DaoEdit - Win32 Debug Unicode"

!ELSEIF  "$(CFG)" == "DaoEdit - Win32 Release Unicode"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\ReadMe.txt

!IF  "$(CFG)" == "DaoEdit - Win32 Release"

!ELSEIF  "$(CFG)" == "DaoEdit - Win32 Debug"

!ELSEIF  "$(CFG)" == "DaoEdit - Win32 Debug Unicode"

!ELSEIF  "$(CFG)" == "DaoEdit - Win32 Release Unicode"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\StdAfx.cpp
DEP_CPP_STDAF=\
	".\StdAfx.h"\
	

!IF  "$(CFG)" == "DaoEdit - Win32 Release"

# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/DaoEdit.pch" /Yc"stdafx.h" /Fo"$(INTDIR)/"\
 /c $(SOURCE) \
	

"$(INTDIR)\StdAfx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\DaoEdit.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "DaoEdit - Win32 Debug"

# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/DaoEdit.pch" /Yc"stdafx.h"\
 /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\StdAfx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\DaoEdit.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "DaoEdit - Win32 Debug Unicode"

# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /D "_AFXDLL" /D "_UNICODE" /Fp"$(INTDIR)/DaoEdit.pch" /Yc"stdafx.h"\
 /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\StdAfx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\DaoEdit.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "DaoEdit - Win32 Release Unicode"

# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_UNICODE" /Fp"$(INTDIR)/DaoEdit.pch" /Yc"stdafx.h"\
 /Fo"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\StdAfx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\DaoEdit.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\DaoEdit.cpp
DEP_CPP_DAOED=\
	".\DaoEdit.h"\
	".\StdAfx.h"\
	

!IF  "$(CFG)" == "DaoEdit - Win32 Release"


"$(INTDIR)\DaoEdit.obj" : $(SOURCE) $(DEP_CPP_DAOED) "$(INTDIR)"\
 "$(INTDIR)\DaoEdit.pch"


!ELSEIF  "$(CFG)" == "DaoEdit - Win32 Debug"


"$(INTDIR)\DaoEdit.obj" : $(SOURCE) $(DEP_CPP_DAOED) "$(INTDIR)"\
 "$(INTDIR)\DaoEdit.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_DAOED) "$(INTDIR)"\
 "$(INTDIR)\DaoEdit.pch"


!ELSEIF  "$(CFG)" == "DaoEdit - Win32 Debug Unicode"


"$(INTDIR)\DaoEdit.obj" : $(SOURCE) $(DEP_CPP_DAOED) "$(INTDIR)"\
 "$(INTDIR)\DaoEdit.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_DAOED) "$(INTDIR)"\
 "$(INTDIR)\DaoEdit.pch"


!ELSEIF  "$(CFG)" == "DaoEdit - Win32 Release Unicode"


"$(INTDIR)\DaoEdit.obj" : $(SOURCE) $(DEP_CPP_DAOED) "$(INTDIR)"\
 "$(INTDIR)\DaoEdit.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\DaoEdit.def

!IF  "$(CFG)" == "DaoEdit - Win32 Release"

!ELSEIF  "$(CFG)" == "DaoEdit - Win32 Debug"

!ELSEIF  "$(CFG)" == "DaoEdit - Win32 Debug Unicode"

!ELSEIF  "$(CFG)" == "DaoEdit - Win32 Release Unicode"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\DaoEdit.rc

!IF  "$(CFG)" == "DaoEdit - Win32 Release"

DEP_RSC_DAOEDI=\
	".\DaoEdit.ico"\
	".\DaoCtl.bmp"\
	".\Release\DaoEdit.tlb"\
	

"$(INTDIR)\DaoEdit.res" : $(SOURCE) $(DEP_RSC_DAOEDI) "$(INTDIR)"\
 "$(INTDIR)\DaoEdit.tlb"
   $(RSC) /l 0x409 /fo"$(INTDIR)/DaoEdit.res" /i "Release" /d "NDEBUG" /d\
 "_AFXDLL" $(SOURCE)


!ELSEIF  "$(CFG)" == "DaoEdit - Win32 Debug"

DEP_RSC_DAOEDI=\
	".\DaoEdit.ico"\
	".\DaoCtl.bmp"\
	
NODEP_RSC_DAOEDI=\
	".\Debug\DaoEdit.tlb"\
	

"$(INTDIR)\DaoEdit.res" : $(SOURCE) $(DEP_RSC_DAOEDI) "$(INTDIR)"\
 "$(INTDIR)\DaoEdit.tlb"
   $(RSC) /l 0x409 /fo"$(INTDIR)/DaoEdit.res" /i "Debug" /d "_DEBUG" /d\
 "_AFXDLL" $(SOURCE)


!ELSEIF  "$(CFG)" == "DaoEdit - Win32 Debug Unicode"

DEP_RSC_DAOEDI=\
	".\DaoEdit.ico"\
	".\DaoCtl.bmp"\
	
NODEP_RSC_DAOEDI=\
	".\DebugU\DaoEdit.tlb"\
	

"$(INTDIR)\DaoEdit.res" : $(SOURCE) $(DEP_RSC_DAOEDI) "$(INTDIR)"\
 "$(INTDIR)\DaoEdit.tlb"
   $(RSC) /l 0x409 /fo"$(INTDIR)/DaoEdit.res" /i "DebugU" /d "_DEBUG" /d\
 "_AFXDLL" $(SOURCE)


!ELSEIF  "$(CFG)" == "DaoEdit - Win32 Release Unicode"

DEP_RSC_DAOEDI=\
	".\DaoEdit.ico"\
	".\DaoCtl.bmp"\
	".\ReleaseU\DaoEdit.tlb"\
	

"$(INTDIR)\DaoEdit.res" : $(SOURCE) $(DEP_RSC_DAOEDI) "$(INTDIR)"\
 "$(INTDIR)\DaoEdit.tlb"
   $(RSC) /l 0x409 /fo"$(INTDIR)/DaoEdit.res" /i "ReleaseU" /d "NDEBUG" /d\
 "_AFXDLL" $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\DaoEdit.odl

!IF  "$(CFG)" == "DaoEdit - Win32 Release"


"$(OUTDIR)\DaoEdit.tlb" : $(SOURCE) "$(OUTDIR)"
   $(MTL) /nologo /D "NDEBUG" /tlb "$(OUTDIR)/DaoEdit.tlb" /win32 $(SOURCE)


!ELSEIF  "$(CFG)" == "DaoEdit - Win32 Debug"


"$(OUTDIR)\DaoEdit.tlb" : $(SOURCE) "$(OUTDIR)"
   $(MTL) /nologo /D "_DEBUG" /tlb "$(OUTDIR)/DaoEdit.tlb" /win32 $(SOURCE)


!ELSEIF  "$(CFG)" == "DaoEdit - Win32 Debug Unicode"


"$(OUTDIR)\DaoEdit.tlb" : $(SOURCE) "$(OUTDIR)"
   $(MTL) /nologo /D "_DEBUG" /tlb "$(OUTDIR)/DaoEdit.tlb" /win32 $(SOURCE)


!ELSEIF  "$(CFG)" == "DaoEdit - Win32 Release Unicode"


"$(OUTDIR)\DaoEdit.tlb" : $(SOURCE) "$(OUTDIR)"
   $(MTL) /nologo /D "NDEBUG" /tlb "$(OUTDIR)/DaoEdit.tlb" /win32 $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\daoppg.cpp
DEP_CPP_DAOPP=\
	".\DaoEdit.h"\
	".\DaoPpg.h"\
	".\StdAfx.h"\
	

!IF  "$(CFG)" == "DaoEdit - Win32 Release"


"$(INTDIR)\daoppg.obj" : $(SOURCE) $(DEP_CPP_DAOPP) "$(INTDIR)"\
 "$(INTDIR)\DaoEdit.pch"


!ELSEIF  "$(CFG)" == "DaoEdit - Win32 Debug"


"$(INTDIR)\daoppg.obj" : $(SOURCE) $(DEP_CPP_DAOPP) "$(INTDIR)"\
 "$(INTDIR)\DaoEdit.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_DAOPP) "$(INTDIR)"\
 "$(INTDIR)\DaoEdit.pch"


!ELSEIF  "$(CFG)" == "DaoEdit - Win32 Debug Unicode"


"$(INTDIR)\daoppg.obj" : $(SOURCE) $(DEP_CPP_DAOPP) "$(INTDIR)"\
 "$(INTDIR)\DaoEdit.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_DAOPP) "$(INTDIR)"\
 "$(INTDIR)\DaoEdit.pch"


!ELSEIF  "$(CFG)" == "DaoEdit - Win32 Release Unicode"


"$(INTDIR)\daoppg.obj" : $(SOURCE) $(DEP_CPP_DAOPP) "$(INTDIR)"\
 "$(INTDIR)\DaoEdit.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\daoctl.cpp
DEP_CPP_DAOCT=\
	".\DaoCtl.h"\
	".\DaoEdit.h"\
	".\DaoPpg.h"\
	".\StdAfx.h"\
	

!IF  "$(CFG)" == "DaoEdit - Win32 Release"


"$(INTDIR)\daoctl.obj" : $(SOURCE) $(DEP_CPP_DAOCT) "$(INTDIR)"\
 "$(INTDIR)\DaoEdit.pch"


!ELSEIF  "$(CFG)" == "DaoEdit - Win32 Debug"


"$(INTDIR)\daoctl.obj" : $(SOURCE) $(DEP_CPP_DAOCT) "$(INTDIR)"\
 "$(INTDIR)\DaoEdit.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_DAOCT) "$(INTDIR)"\
 "$(INTDIR)\DaoEdit.pch"


!ELSEIF  "$(CFG)" == "DaoEdit - Win32 Debug Unicode"


"$(INTDIR)\daoctl.obj" : $(SOURCE) $(DEP_CPP_DAOCT) "$(INTDIR)"\
 "$(INTDIR)\DaoEdit.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_DAOCT) "$(INTDIR)"\
 "$(INTDIR)\DaoEdit.pch"


!ELSEIF  "$(CFG)" == "DaoEdit - Win32 Release Unicode"


"$(INTDIR)\daoctl.obj" : $(SOURCE) $(DEP_CPP_DAOCT) "$(INTDIR)"\
 "$(INTDIR)\DaoEdit.pch"


!ENDIF 

# End Source File
# End Target
# End Project
################################################################################
