# Microsoft Visual C++ Generated NMAKE File, Format Version 40000
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

!IF "$(CFG)" == ""
CFG=Smile - Win32 Debug
!MESSAGE No configuration specified.  Defaulting to Smile - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "Smile - Win32 Release" && "$(CFG)" != "Smile - Win32 Debug" &&\
 "$(CFG)" != "Smile - Win32 Unicode Debug" && "$(CFG)" !=\
 "Smile - Win32 Unicode Release"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "Smile.mak" CFG="Smile - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Smile - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "Smile - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "Smile - Win32 Unicode Debug" (based on\
 "Win32 (x86) Dynamic-Link Library")
!MESSAGE "Smile - Win32 Unicode Release" (based on\
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
# PROP Target_Last_Scanned "Smile - Win32 Debug"
MTL=mktyplib.exe
RSC=rc.exe
CPP=cl.exe

!IF  "$(CFG)" == "Smile - Win32 Release"

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

ALL : "$(OUTDIR)\Smile.ocx" "$(OUTDIR)\regsvr32.trg"

CLEAN : 
	-@erase ".\Release\Smile.lib"
	-@erase ".\Release\SmileCtl.obj"
	-@erase ".\Release\Smile.pch"
	-@erase ".\Release\StdAfx.obj"
	-@erase ".\Release\Smile.obj"
	-@erase ".\Release\SmilePpg.obj"
	-@erase ".\Release\Smile.res"
	-@erase ".\Release\Smile.tlb"
	-@erase ".\Release\Smile.exp"
	-@erase ".\Release\regsvr32.trg"
	-@erase ".\Release\Smile.ocx"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
CPP_PROJ=/nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/Smile.pch" /Yu"stdafx.h" /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\Release/
CPP_SBRS=
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/Smile.res" /d "NDEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/Smile.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 /nologo /subsystem:windows /dll /machine:I386
LINK32_FLAGS=/nologo /subsystem:windows /dll /incremental:no\
 /pdb:"$(OUTDIR)/Smile.pdb" /machine:I386 /def:".\Smile.def"\
 /out:"$(OUTDIR)/Smile.ocx" /implib:"$(OUTDIR)/Smile.lib" 
DEF_FILE= \
	".\Smile.def"
LINK32_OBJS= \
	"$(INTDIR)/SmileCtl.obj" \
	"$(INTDIR)/StdAfx.obj" \
	"$(INTDIR)/Smile.obj" \
	"$(INTDIR)/SmilePpg.obj" \
	"$(INTDIR)/Smile.res"

"$(OUTDIR)\Smile.ocx" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

# Begin Custom Build - Registering OLE control...
OutDir=.\Release
TargetPath=.\Release\Smile.ocx
InputPath=.\Release\Smile.ocx
SOURCE=$(InputPath)

"$(OutDir)\regsvr32.trg" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   regsvr32 /s /c "$(TargetPath)"
   echo regsvr32 exec. time > "$(OutDir)\regsvr32.trg"

# End Custom Build

!ELSEIF  "$(CFG)" == "Smile - Win32 Debug"

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

ALL : "$(OUTDIR)\Smile.ocx" "$(OUTDIR)\regsvr32.trg"

CLEAN : 
	-@erase ".\Debug\vc40.pdb"
	-@erase ".\Debug\Smile.pch"
	-@erase ".\Debug\Smile.ilk"
	-@erase ".\Debug\Smile.obj"
	-@erase ".\Debug\SmileCtl.obj"
	-@erase ".\Debug\SmilePpg.obj"
	-@erase ".\Debug\StdAfx.obj"
	-@erase ".\Debug\Smile.res"
	-@erase ".\Debug\Smile.tlb"
	-@erase ".\Debug\Smile.lib"
	-@erase ".\Debug\Smile.exp"
	-@erase ".\Debug\Smile.pdb"
	-@erase ".\Debug\regsvr32.trg"
	-@erase ".\Debug\Smile.ocx"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
CPP_PROJ=/nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/Smile.pch" /Yu"stdafx.h" /Fo"$(INTDIR)/"\
 /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\Debug/
CPP_SBRS=
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/Smile.res" /d "_DEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/Smile.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /debug /machine:I386
# ADD LINK32 /nologo /subsystem:windows /dll /debug /machine:I386
LINK32_FLAGS=/nologo /subsystem:windows /dll /incremental:yes\
 /pdb:"$(OUTDIR)/Smile.pdb" /debug /machine:I386 /def:".\Smile.def"\
 /out:"$(OUTDIR)/Smile.ocx" /implib:"$(OUTDIR)/Smile.lib" 
DEF_FILE= \
	".\Smile.def"
LINK32_OBJS= \
	"$(INTDIR)/Smile.obj" \
	"$(INTDIR)/SmileCtl.obj" \
	"$(INTDIR)/SmilePpg.obj" \
	"$(INTDIR)/StdAfx.obj" \
	"$(INTDIR)/Smile.res"

"$(OUTDIR)\Smile.ocx" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

# Begin Custom Build - Registering OLE control...
OutDir=.\Debug
TargetPath=.\Debug\Smile.ocx
InputPath=.\Debug\Smile.ocx
SOURCE=$(InputPath)

"$(OutDir)\regsvr32.trg" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   regsvr32 /s /c "$(TargetPath)"
   echo regsvr32 exec. time > "$(OutDir)\regsvr32.trg"

# End Custom Build

!ELSEIF  "$(CFG)" == "Smile - Win32 Unicode Debug"

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

ALL : "$(OUTDIR)\Smile.ocx" "$(OUTDIR)\regsvr32.trg"

CLEAN : 
	-@erase ".\DebugU\vc40.pdb"
	-@erase ".\DebugU\Smile.pch"
	-@erase ".\DebugU\Smile.ilk"
	-@erase ".\DebugU\Smile.obj"
	-@erase ".\DebugU\SmilePpg.obj"
	-@erase ".\DebugU\StdAfx.obj"
	-@erase ".\DebugU\SmileCtl.obj"
	-@erase ".\DebugU\Smile.res"
	-@erase ".\DebugU\Smile.tlb"
	-@erase ".\DebugU\Smile.lib"
	-@erase ".\DebugU\Smile.exp"
	-@erase ".\DebugU\Smile.pdb"
	-@erase ".\DebugU\regsvr32.trg"
	-@erase ".\DebugU\Smile.ocx"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_UNICODE" /Yu"stdafx.h" /c
CPP_PROJ=/nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /D "_AFXDLL" /D "_UNICODE" /Fp"$(INTDIR)/Smile.pch" /Yu"stdafx.h"\
 /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\DebugU/
CPP_SBRS=
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/Smile.res" /d "_DEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/Smile.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /debug /machine:I386
# ADD LINK32 /nologo /subsystem:windows /dll /debug /machine:I386
LINK32_FLAGS=/nologo /subsystem:windows /dll /incremental:yes\
 /pdb:"$(OUTDIR)/Smile.pdb" /debug /machine:I386 /def:".\Smile.def"\
 /out:"$(OUTDIR)/Smile.ocx" /implib:"$(OUTDIR)/Smile.lib" 
DEF_FILE= \
	".\Smile.def"
LINK32_OBJS= \
	"$(INTDIR)/Smile.obj" \
	"$(INTDIR)/SmilePpg.obj" \
	"$(INTDIR)/StdAfx.obj" \
	"$(INTDIR)/SmileCtl.obj" \
	"$(INTDIR)/Smile.res"

"$(OUTDIR)\Smile.ocx" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

# Begin Custom Build - Registering OLE control...
OutDir=.\DebugU
TargetPath=.\DebugU\Smile.ocx
InputPath=.\DebugU\Smile.ocx
SOURCE=$(InputPath)

"$(OutDir)\regsvr32.trg" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   regsvr32 /s /c "$(TargetPath)"
   echo regsvr32 exec. time > "$(OutDir)\regsvr32.trg"

# End Custom Build

!ELSEIF  "$(CFG)" == "Smile - Win32 Unicode Release"

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

ALL : "$(OUTDIR)\Smile.ocx" "$(OUTDIR)\regsvr32.trg"

CLEAN : 
	-@erase ".\ReleaseU\Smile.lib"
	-@erase ".\ReleaseU\SmileCtl.obj"
	-@erase ".\ReleaseU\Smile.pch"
	-@erase ".\ReleaseU\Smile.obj"
	-@erase ".\ReleaseU\SmilePpg.obj"
	-@erase ".\ReleaseU\StdAfx.obj"
	-@erase ".\ReleaseU\Smile.res"
	-@erase ".\ReleaseU\Smile.tlb"
	-@erase ".\ReleaseU\Smile.exp"
	-@erase ".\ReleaseU\regsvr32.trg"
	-@erase ".\ReleaseU\Smile.ocx"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_UNICODE" /Yu"stdafx.h" /c
CPP_PROJ=/nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_UNICODE" /Fp"$(INTDIR)/Smile.pch" /Yu"stdafx.h" /Fo"$(INTDIR)/"\
 /c 
CPP_OBJS=.\ReleaseU/
CPP_SBRS=
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/Smile.res" /d "NDEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/Smile.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 /nologo /subsystem:windows /dll /machine:I386
LINK32_FLAGS=/nologo /subsystem:windows /dll /incremental:no\
 /pdb:"$(OUTDIR)/Smile.pdb" /machine:I386 /def:".\Smile.def"\
 /out:"$(OUTDIR)/Smile.ocx" /implib:"$(OUTDIR)/Smile.lib" 
DEF_FILE= \
	".\Smile.def"
LINK32_OBJS= \
	"$(INTDIR)/SmileCtl.obj" \
	"$(INTDIR)/Smile.obj" \
	"$(INTDIR)/SmilePpg.obj" \
	"$(INTDIR)/StdAfx.obj" \
	"$(INTDIR)/Smile.res"

"$(OUTDIR)\Smile.ocx" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

# Begin Custom Build - Registering OLE control...
OutDir=.\ReleaseU
TargetPath=.\ReleaseU\Smile.ocx
InputPath=.\ReleaseU\Smile.ocx
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

# Name "Smile - Win32 Release"
# Name "Smile - Win32 Debug"
# Name "Smile - Win32 Unicode Debug"
# Name "Smile - Win32 Unicode Release"

!IF  "$(CFG)" == "Smile - Win32 Release"

!ELSEIF  "$(CFG)" == "Smile - Win32 Debug"

!ELSEIF  "$(CFG)" == "Smile - Win32 Unicode Debug"

!ELSEIF  "$(CFG)" == "Smile - Win32 Unicode Release"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\ReadMe.txt

!IF  "$(CFG)" == "Smile - Win32 Release"

!ELSEIF  "$(CFG)" == "Smile - Win32 Debug"

!ELSEIF  "$(CFG)" == "Smile - Win32 Unicode Debug"

!ELSEIF  "$(CFG)" == "Smile - Win32 Unicode Release"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\StdAfx.cpp
DEP_CPP_STDAF=\
	".\StdAfx.h"\
	

!IF  "$(CFG)" == "Smile - Win32 Release"

# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/Smile.pch" /Yc"stdafx.h" /Fo"$(INTDIR)/" /c\
 $(SOURCE) \
	

"$(INTDIR)\StdAfx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\Smile.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "Smile - Win32 Debug"

# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/Smile.pch" /Yc"stdafx.h" /Fo"$(INTDIR)/"\
 /Fd"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\StdAfx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\Smile.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "Smile - Win32 Unicode Debug"

# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /D "_AFXDLL" /D "_UNICODE" /Fp"$(INTDIR)/Smile.pch" /Yc"stdafx.h"\
 /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\StdAfx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\Smile.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "Smile - Win32 Unicode Release"

# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_UNICODE" /Fp"$(INTDIR)/Smile.pch" /Yc"stdafx.h" /Fo"$(INTDIR)/"\
 /c $(SOURCE) \
	

"$(INTDIR)\StdAfx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\Smile.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Smile.cpp
DEP_CPP_SMILE=\
	".\Smile.h"\
	".\StdAfx.h"\
	

!IF  "$(CFG)" == "Smile - Win32 Release"


"$(INTDIR)\Smile.obj" : $(SOURCE) $(DEP_CPP_SMILE) "$(INTDIR)"\
 "$(INTDIR)\Smile.pch"


!ELSEIF  "$(CFG)" == "Smile - Win32 Debug"


"$(INTDIR)\Smile.obj" : $(SOURCE) $(DEP_CPP_SMILE) "$(INTDIR)"\
 "$(INTDIR)\Smile.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_SMILE) "$(INTDIR)"\
 "$(INTDIR)\Smile.pch"


!ELSEIF  "$(CFG)" == "Smile - Win32 Unicode Debug"


"$(INTDIR)\Smile.obj" : $(SOURCE) $(DEP_CPP_SMILE) "$(INTDIR)"\
 "$(INTDIR)\Smile.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_SMILE) "$(INTDIR)"\
 "$(INTDIR)\Smile.pch"


!ELSEIF  "$(CFG)" == "Smile - Win32 Unicode Release"


"$(INTDIR)\Smile.obj" : $(SOURCE) $(DEP_CPP_SMILE) "$(INTDIR)"\
 "$(INTDIR)\Smile.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Smile.def

!IF  "$(CFG)" == "Smile - Win32 Release"

!ELSEIF  "$(CFG)" == "Smile - Win32 Debug"

!ELSEIF  "$(CFG)" == "Smile - Win32 Unicode Debug"

!ELSEIF  "$(CFG)" == "Smile - Win32 Unicode Release"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Smile.rc

!IF  "$(CFG)" == "Smile - Win32 Release"

DEP_RSC_SMILE_=\
	".\Smile.ico"\
	".\SmileCtl.bmp"\
	".\Release\Smile.tlb"\
	

"$(INTDIR)\Smile.res" : $(SOURCE) $(DEP_RSC_SMILE_) "$(INTDIR)"\
 "$(INTDIR)\Smile.tlb"
   $(RSC) /l 0x409 /fo"$(INTDIR)/Smile.res" /i "Release" /d "NDEBUG" /d\
 "_AFXDLL" $(SOURCE)


!ELSEIF  "$(CFG)" == "Smile - Win32 Debug"

DEP_RSC_SMILE_=\
	".\Debug\Smile.tlb"\
	".\Smile.ico"\
	".\SmileCtl.bmp"\
	

"$(INTDIR)\Smile.res" : $(SOURCE) $(DEP_RSC_SMILE_) "$(INTDIR)"\
 "$(INTDIR)\Smile.tlb"
   $(RSC) /l 0x409 /fo"$(INTDIR)/Smile.res" /i "Debug" /d "_DEBUG" /d\
 "_AFXDLL" $(SOURCE)


!ELSEIF  "$(CFG)" == "Smile - Win32 Unicode Debug"

DEP_RSC_SMILE_=\
	".\DebugU\Smile.tlb"\
	".\Smile.ico"\
	".\SmileCtl.bmp"\
	

"$(INTDIR)\Smile.res" : $(SOURCE) $(DEP_RSC_SMILE_) "$(INTDIR)"\
 "$(INTDIR)\Smile.tlb"
   $(RSC) /l 0x409 /fo"$(INTDIR)/Smile.res" /i "DebugU" /d "_DEBUG" /d\
 "_AFXDLL" $(SOURCE)


!ELSEIF  "$(CFG)" == "Smile - Win32 Unicode Release"

DEP_RSC_SMILE_=\
	".\Smile.ico"\
	".\SmileCtl.bmp"\
	".\ReleaseU\Smile.tlb"\
	

"$(INTDIR)\Smile.res" : $(SOURCE) $(DEP_RSC_SMILE_) "$(INTDIR)"\
 "$(INTDIR)\Smile.tlb"
   $(RSC) /l 0x409 /fo"$(INTDIR)/Smile.res" /i "ReleaseU" /d "NDEBUG" /d\
 "_AFXDLL" $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Smile.odl

!IF  "$(CFG)" == "Smile - Win32 Release"


"$(OUTDIR)\Smile.tlb" : $(SOURCE) "$(OUTDIR)"
   $(MTL) /nologo /D "NDEBUG" /tlb "$(OUTDIR)/Smile.tlb" /win32 $(SOURCE)


!ELSEIF  "$(CFG)" == "Smile - Win32 Debug"


"$(OUTDIR)\Smile.tlb" : $(SOURCE) "$(OUTDIR)"
   $(MTL) /nologo /D "_DEBUG" /tlb "$(OUTDIR)/Smile.tlb" /win32 $(SOURCE)


!ELSEIF  "$(CFG)" == "Smile - Win32 Unicode Debug"


"$(OUTDIR)\Smile.tlb" : $(SOURCE) "$(OUTDIR)"
   $(MTL) /nologo /D "_DEBUG" /tlb "$(OUTDIR)/Smile.tlb" /win32 $(SOURCE)


!ELSEIF  "$(CFG)" == "Smile - Win32 Unicode Release"


"$(OUTDIR)\Smile.tlb" : $(SOURCE) "$(OUTDIR)"
   $(MTL) /nologo /D "NDEBUG" /tlb "$(OUTDIR)/Smile.tlb" /win32 $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\SmileCtl.cpp
DEP_CPP_SMILEC=\
	".\Smile.h"\
	".\SmileCtl.h"\
	".\SmilePpg.h"\
	".\StdAfx.h"\
	

!IF  "$(CFG)" == "Smile - Win32 Release"


"$(INTDIR)\SmileCtl.obj" : $(SOURCE) $(DEP_CPP_SMILEC) "$(INTDIR)"\
 "$(INTDIR)\Smile.pch"


!ELSEIF  "$(CFG)" == "Smile - Win32 Debug"


"$(INTDIR)\SmileCtl.obj" : $(SOURCE) $(DEP_CPP_SMILEC) "$(INTDIR)"\
 "$(INTDIR)\Smile.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_SMILEC) "$(INTDIR)"\
 "$(INTDIR)\Smile.pch"


!ELSEIF  "$(CFG)" == "Smile - Win32 Unicode Debug"


"$(INTDIR)\SmileCtl.obj" : $(SOURCE) $(DEP_CPP_SMILEC) "$(INTDIR)"\
 "$(INTDIR)\Smile.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_SMILEC) "$(INTDIR)"\
 "$(INTDIR)\Smile.pch"


!ELSEIF  "$(CFG)" == "Smile - Win32 Unicode Release"


"$(INTDIR)\SmileCtl.obj" : $(SOURCE) $(DEP_CPP_SMILEC) "$(INTDIR)"\
 "$(INTDIR)\Smile.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\SmilePpg.cpp
DEP_CPP_SMILEP=\
	".\Smile.h"\
	".\SmilePpg.h"\
	".\StdAfx.h"\
	

!IF  "$(CFG)" == "Smile - Win32 Release"


"$(INTDIR)\SmilePpg.obj" : $(SOURCE) $(DEP_CPP_SMILEP) "$(INTDIR)"\
 "$(INTDIR)\Smile.pch"


!ELSEIF  "$(CFG)" == "Smile - Win32 Debug"


"$(INTDIR)\SmilePpg.obj" : $(SOURCE) $(DEP_CPP_SMILEP) "$(INTDIR)"\
 "$(INTDIR)\Smile.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_SMILEP) "$(INTDIR)"\
 "$(INTDIR)\Smile.pch"


!ELSEIF  "$(CFG)" == "Smile - Win32 Unicode Debug"


"$(INTDIR)\SmilePpg.obj" : $(SOURCE) $(DEP_CPP_SMILEP) "$(INTDIR)"\
 "$(INTDIR)\Smile.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_SMILEP) "$(INTDIR)"\
 "$(INTDIR)\Smile.pch"


!ELSEIF  "$(CFG)" == "Smile - Win32 Unicode Release"


"$(INTDIR)\SmilePpg.obj" : $(SOURCE) $(DEP_CPP_SMILEP) "$(INTDIR)"\
 "$(INTDIR)\Smile.pch"


!ENDIF 

# End Source File
# End Target
# End Project
################################################################################
