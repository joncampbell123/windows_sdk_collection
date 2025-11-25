# Microsoft Visual C++ Generated NMAKE File, Format Version 40000
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

!IF "$(CFG)" == ""
CFG=Licensed - Win32 Debug
!MESSAGE No configuration specified.  Defaulting to Licensed - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "Licensed - Win32 Release" && "$(CFG)" !=\
 "Licensed - Win32 Debug" && "$(CFG)" != "Licensed - Win32 Unicode Debug" &&\
 "$(CFG)" != "Licensed - Win32 Unicode Release"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "licensed.mak" CFG="Licensed - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Licensed - Win32 Release" (based on\
 "Win32 (x86) Dynamic-Link Library")
!MESSAGE "Licensed - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "Licensed - Win32 Unicode Debug" (based on\
 "Win32 (x86) Dynamic-Link Library")
!MESSAGE "Licensed - Win32 Unicode Release" (based on\
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
# PROP Target_Last_Scanned "Licensed - Win32 Unicode Release"
RSC=rc.exe
CPP=cl.exe
MTL=mktyplib.exe

!IF  "$(CFG)" == "Licensed - Win32 Release"

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

ALL : "$(OUTDIR)\licensed.ocx" "$(OUTDIR)\regsvr32.trg"

CLEAN : 
	-@erase ".\Release\licensed.lib"
	-@erase ".\Release\licenppg.obj"
	-@erase ".\Release\licensed.pch"
	-@erase ".\Release\stdafx.obj"
	-@erase ".\Release\licenctl.obj"
	-@erase ".\Release\licensed.obj"
	-@erase ".\Release\licensed.res"
	-@erase ".\Release\licensed.tlb"
	-@erase ".\Release\licensed.exp"
	-@erase ".\Release\regsvr32.trg"
	-@erase ".\Release\licensed.ocx"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
CPP_PROJ=/nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/licensed.pch" /Yu"stdafx.h" /Fo"$(INTDIR)/"\
 /c 
CPP_OBJS=.\Release/
CPP_SBRS=
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/licensed.res" /d "NDEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/licensed.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 /nologo /subsystem:windows /dll /machine:I386
LINK32_FLAGS=/nologo /subsystem:windows /dll /incremental:no\
 /pdb:"$(OUTDIR)/licensed.pdb" /machine:I386 /def:".\licensed.def"\
 /out:"$(OUTDIR)/licensed.ocx" /implib:"$(OUTDIR)/licensed.lib" 
DEF_FILE= \
	".\licensed.def"
LINK32_OBJS= \
	"$(INTDIR)/licenppg.obj" \
	"$(INTDIR)/stdafx.obj" \
	"$(INTDIR)/licenctl.obj" \
	"$(INTDIR)/licensed.obj" \
	"$(INTDIR)/licensed.res"

"$(OUTDIR)\licensed.ocx" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

# Begin Custom Build - Registering OLE control...
OutDir=.\Release
TargetPath=.\Release\licensed.ocx
InputPath=.\Release\licensed.ocx
SOURCE=$(InputPath)

"$(OutDir)\regsvr32.trg" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   regsvr32 /s /c "$(TargetPath)"
   echo regsvr32 exec. time > "$(OutDir)\regsvr32.trg"

# End Custom Build

!ELSEIF  "$(CFG)" == "Licensed - Win32 Debug"

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

ALL : "$(OUTDIR)\licensed.ocx" "$(OUTDIR)\regsvr32.trg"

CLEAN : 
	-@erase ".\Debug\vc40.pdb"
	-@erase ".\Debug\licensed.pch"
	-@erase ".\Debug\licensed.ilk"
	-@erase ".\Debug\stdafx.obj"
	-@erase ".\Debug\licenctl.obj"
	-@erase ".\Debug\licensed.obj"
	-@erase ".\Debug\licenppg.obj"
	-@erase ".\Debug\licensed.res"
	-@erase ".\Debug\licensed.tlb"
	-@erase ".\Debug\licensed.lib"
	-@erase ".\Debug\licensed.exp"
	-@erase ".\Debug\licensed.pdb"
	-@erase ".\Debug\regsvr32.trg"
	-@erase ".\Debug\licensed.ocx"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
CPP_PROJ=/nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/licensed.pch" /Yu"stdafx.h"\
 /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\Debug/
CPP_SBRS=
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/licensed.res" /d "_DEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/licensed.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /debug /machine:I386
# ADD LINK32 /nologo /subsystem:windows /dll /debug /machine:I386
LINK32_FLAGS=/nologo /subsystem:windows /dll /incremental:yes\
 /pdb:"$(OUTDIR)/licensed.pdb" /debug /machine:I386 /def:".\licensed.def"\
 /out:"$(OUTDIR)/licensed.ocx" /implib:"$(OUTDIR)/licensed.lib" 
DEF_FILE= \
	".\licensed.def"
LINK32_OBJS= \
	"$(INTDIR)/stdafx.obj" \
	"$(INTDIR)/licenctl.obj" \
	"$(INTDIR)/licensed.obj" \
	"$(INTDIR)/licenppg.obj" \
	"$(INTDIR)/licensed.res"

"$(OUTDIR)\licensed.ocx" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

# Begin Custom Build - Registering OLE control...
OutDir=.\Debug
TargetPath=.\Debug\licensed.ocx
InputPath=.\Debug\licensed.ocx
SOURCE=$(InputPath)

"$(OutDir)\regsvr32.trg" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   regsvr32 /s /c "$(TargetPath)"
   echo regsvr32 exec. time > "$(OutDir)\regsvr32.trg"

# End Custom Build

!ELSEIF  "$(CFG)" == "Licensed - Win32 Unicode Debug"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir DebugU
# PROP BASE Intermediate_Dir DebugU
# PROP BASE Target_Dir
# PROP BASE Target_Ext ocx
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 1
# PROP Output_Dir UniDebug
# PROP Intermediate_Dir UniDebug
# PROP Target_Dir
# PROP Target_Ext ocx
OUTDIR=.\UniDebug
INTDIR=.\UniDebug

ALL : "$(OUTDIR)\licensed.ocx" "$(OUTDIR)\regsvr32.trg"

CLEAN : 
	-@erase ".\UniDebug\vc40.pdb"
	-@erase ".\UniDebug\licensed.pch"
	-@erase ".\UniDebug\licensed.ilk"
	-@erase ".\UniDebug\licenppg.obj"
	-@erase ".\UniDebug\stdafx.obj"
	-@erase ".\UniDebug\licenctl.obj"
	-@erase ".\UniDebug\licensed.obj"
	-@erase ".\UniDebug\licensed.res"
	-@erase ".\UniDebug\licensed.tlb"
	-@erase ".\UniDebug\licensed.lib"
	-@erase ".\UniDebug\licensed.exp"
	-@erase ".\UniDebug\licensed.pdb"
	-@erase ".\UniDebug\regsvr32.trg"
	-@erase ".\UniDebug\licensed.ocx"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_UNICODE" /Yu"stdafx.h" /c
CPP_PROJ=/nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /D "_AFXDLL" /D "_UNICODE" /Fp"$(INTDIR)/licensed.pch" /Yu"stdafx.h"\
 /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\UniDebug/
CPP_SBRS=
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/licensed.res" /d "_DEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/licensed.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /debug /machine:I386
# ADD LINK32 /nologo /subsystem:windows /dll /debug /machine:I386
LINK32_FLAGS=/nologo /subsystem:windows /dll /incremental:yes\
 /pdb:"$(OUTDIR)/licensed.pdb" /debug /machine:I386 /def:".\licensed.def"\
 /out:"$(OUTDIR)/licensed.ocx" /implib:"$(OUTDIR)/licensed.lib" 
DEF_FILE= \
	".\licensed.def"
LINK32_OBJS= \
	"$(INTDIR)/licenppg.obj" \
	"$(INTDIR)/stdafx.obj" \
	"$(INTDIR)/licenctl.obj" \
	"$(INTDIR)/licensed.obj" \
	"$(INTDIR)/licensed.res"

"$(OUTDIR)\licensed.ocx" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

# Begin Custom Build - Registering OLE control...
OutDir=.\UniDebug
TargetPath=.\UniDebug\licensed.ocx
InputPath=.\UniDebug\licensed.ocx
SOURCE=$(InputPath)

"$(OutDir)\regsvr32.trg" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   regsvr32 /s /c "$(TargetPath)"
   echo regsvr32 exec. time > "$(OutDir)\regsvr32.trg"

# End Custom Build

!ELSEIF  "$(CFG)" == "Licensed - Win32 Unicode Release"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir ReleaseU
# PROP BASE Intermediate_Dir ReleaseU
# PROP BASE Target_Dir
# PROP BASE Target_Ext ocx
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 0
# PROP Output_Dir UniRelease
# PROP Intermediate_Dir UniRelease
# PROP Target_Dir
# PROP Target_Ext ocx
OUTDIR=.\UniRelease
INTDIR=.\UniRelease

ALL : "$(OUTDIR)\licensed.ocx" "$(OUTDIR)\regsvr32.trg"

CLEAN : 
	-@erase ".\UniRelease\licensed.lib"
	-@erase ".\UniRelease\licenctl.obj"
	-@erase ".\UniRelease\licensed.pch"
	-@erase ".\UniRelease\licensed.obj"
	-@erase ".\UniRelease\licenppg.obj"
	-@erase ".\UniRelease\stdafx.obj"
	-@erase ".\UniRelease\licensed.res"
	-@erase ".\UniRelease\licensed.tlb"
	-@erase ".\UniRelease\licensed.exp"
	-@erase ".\UniRelease\regsvr32.trg"
	-@erase ".\UniRelease\licensed.ocx"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_UNICODE" /Yu"stdafx.h" /c
CPP_PROJ=/nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_UNICODE" /Fp"$(INTDIR)/licensed.pch" /Yu"stdafx.h"\
 /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\UniRelease/
CPP_SBRS=
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/licensed.res" /d "NDEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/licensed.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 /nologo /subsystem:windows /dll /machine:I386
LINK32_FLAGS=/nologo /subsystem:windows /dll /incremental:no\
 /pdb:"$(OUTDIR)/licensed.pdb" /machine:I386 /def:".\licensed.def"\
 /out:"$(OUTDIR)/licensed.ocx" /implib:"$(OUTDIR)/licensed.lib" 
DEF_FILE= \
	".\licensed.def"
LINK32_OBJS= \
	"$(INTDIR)/licenctl.obj" \
	"$(INTDIR)/licensed.obj" \
	"$(INTDIR)/licenppg.obj" \
	"$(INTDIR)/stdafx.obj" \
	"$(INTDIR)/licensed.res"

"$(OUTDIR)\licensed.ocx" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

# Begin Custom Build - Registering OLE control...
OutDir=.\UniRelease
TargetPath=.\UniRelease\licensed.ocx
InputPath=.\UniRelease\licensed.ocx
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

# Name "Licensed - Win32 Release"
# Name "Licensed - Win32 Debug"
# Name "Licensed - Win32 Unicode Debug"
# Name "Licensed - Win32 Unicode Release"

!IF  "$(CFG)" == "Licensed - Win32 Release"

!ELSEIF  "$(CFG)" == "Licensed - Win32 Debug"

!ELSEIF  "$(CFG)" == "Licensed - Win32 Unicode Debug"

!ELSEIF  "$(CFG)" == "Licensed - Win32 Unicode Release"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\ReadMe.txt

!IF  "$(CFG)" == "Licensed - Win32 Release"

!ELSEIF  "$(CFG)" == "Licensed - Win32 Debug"

!ELSEIF  "$(CFG)" == "Licensed - Win32 Unicode Debug"

!ELSEIF  "$(CFG)" == "Licensed - Win32 Unicode Release"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\stdafx.cpp
DEP_CPP_STDAF=\
	".\stdafx.h"\
	

!IF  "$(CFG)" == "Licensed - Win32 Release"

# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/licensed.pch" /Yc"stdafx.h" /Fo"$(INTDIR)/"\
 /c $(SOURCE) \
	

"$(INTDIR)\stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\licensed.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "Licensed - Win32 Debug"

# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/licensed.pch" /Yc"stdafx.h"\
 /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\licensed.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "Licensed - Win32 Unicode Debug"

# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /D "_AFXDLL" /D "_UNICODE" /Fp"$(INTDIR)/licensed.pch" /Yc"stdafx.h"\
 /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\licensed.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "Licensed - Win32 Unicode Release"

# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_UNICODE" /Fp"$(INTDIR)/licensed.pch" /Yc"stdafx.h"\
 /Fo"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\licensed.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\licensed.cpp
DEP_CPP_LICEN=\
	".\licensed.h"\
	".\stdafx.h"\
	

!IF  "$(CFG)" == "Licensed - Win32 Release"


"$(INTDIR)\licensed.obj" : $(SOURCE) $(DEP_CPP_LICEN) "$(INTDIR)"\
 "$(INTDIR)\licensed.pch"


!ELSEIF  "$(CFG)" == "Licensed - Win32 Debug"


"$(INTDIR)\licensed.obj" : $(SOURCE) $(DEP_CPP_LICEN) "$(INTDIR)"\
 "$(INTDIR)\licensed.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_LICEN) "$(INTDIR)"\
 "$(INTDIR)\licensed.pch"


!ELSEIF  "$(CFG)" == "Licensed - Win32 Unicode Debug"


"$(INTDIR)\licensed.obj" : $(SOURCE) $(DEP_CPP_LICEN) "$(INTDIR)"\
 "$(INTDIR)\licensed.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_LICEN) "$(INTDIR)"\
 "$(INTDIR)\licensed.pch"


!ELSEIF  "$(CFG)" == "Licensed - Win32 Unicode Release"


"$(INTDIR)\licensed.obj" : $(SOURCE) $(DEP_CPP_LICEN) "$(INTDIR)"\
 "$(INTDIR)\licensed.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\licensed.def

!IF  "$(CFG)" == "Licensed - Win32 Release"

!ELSEIF  "$(CFG)" == "Licensed - Win32 Debug"

!ELSEIF  "$(CFG)" == "Licensed - Win32 Unicode Debug"

!ELSEIF  "$(CFG)" == "Licensed - Win32 Unicode Release"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\licensed.rc

!IF  "$(CFG)" == "Licensed - Win32 Release"

DEP_RSC_LICENS=\
	".\Release\licensed.tlb"\
	".\licensed.ico"\
	".\licenctl.bmp"\
	".\resource.h"\
	".\licensed.rc2"\
	

"$(INTDIR)\licensed.res" : $(SOURCE) $(DEP_RSC_LICENS) "$(INTDIR)"\
 "$(INTDIR)\licensed.tlb"
   $(RSC) /l 0x409 /fo"$(INTDIR)/licensed.res" /i "Release" /d "NDEBUG" /d\
 "_AFXDLL" $(SOURCE)


!ELSEIF  "$(CFG)" == "Licensed - Win32 Debug"

DEP_RSC_LICENS=\
	".\licensed.ico"\
	".\licenctl.bmp"\
	".\resource.h"\
	".\licensed.rc2"\
	".\Debug\licensed.tlb"\
	

"$(INTDIR)\licensed.res" : $(SOURCE) $(DEP_RSC_LICENS) "$(INTDIR)"\
 "$(INTDIR)\licensed.tlb"
   $(RSC) /l 0x409 /fo"$(INTDIR)/licensed.res" /i "Debug" /d "_DEBUG" /d\
 "_AFXDLL" $(SOURCE)


!ELSEIF  "$(CFG)" == "Licensed - Win32 Unicode Debug"

DEP_RSC_LICENS=\
	".\UniDebug\licensed.tlb"\
	".\licensed.ico"\
	".\licenctl.bmp"\
	".\resource.h"\
	".\licensed.rc2"\
	

"$(INTDIR)\licensed.res" : $(SOURCE) $(DEP_RSC_LICENS) "$(INTDIR)"\
 "$(INTDIR)\licensed.tlb"
   $(RSC) /l 0x409 /fo"$(INTDIR)/licensed.res" /i "UniDebug" /d "_DEBUG" /d\
 "_AFXDLL" $(SOURCE)


!ELSEIF  "$(CFG)" == "Licensed - Win32 Unicode Release"

DEP_RSC_LICENS=\
	".\licensed.ico"\
	".\licenctl.bmp"\
	".\resource.h"\
	".\licensed.rc2"\
	".\UniRelease\licensed.tlb"\
	

"$(INTDIR)\licensed.res" : $(SOURCE) $(DEP_RSC_LICENS) "$(INTDIR)"\
 "$(INTDIR)\licensed.tlb"
   $(RSC) /l 0x409 /fo"$(INTDIR)/licensed.res" /i "UniRelease" /d "NDEBUG" /d\
 "_AFXDLL" $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\licensed.odl

!IF  "$(CFG)" == "Licensed - Win32 Release"


"$(OUTDIR)\licensed.tlb" : $(SOURCE) "$(OUTDIR)"
   $(MTL) /nologo /D "NDEBUG" /tlb "$(OUTDIR)/licensed.tlb" /win32 $(SOURCE)


!ELSEIF  "$(CFG)" == "Licensed - Win32 Debug"


"$(OUTDIR)\licensed.tlb" : $(SOURCE) "$(OUTDIR)"
   $(MTL) /nologo /D "_DEBUG" /tlb "$(OUTDIR)/licensed.tlb" /win32 $(SOURCE)


!ELSEIF  "$(CFG)" == "Licensed - Win32 Unicode Debug"


"$(OUTDIR)\licensed.tlb" : $(SOURCE) "$(OUTDIR)"
   $(MTL) /nologo /D "_DEBUG" /tlb "$(OUTDIR)/licensed.tlb" /win32 $(SOURCE)


!ELSEIF  "$(CFG)" == "Licensed - Win32 Unicode Release"


"$(OUTDIR)\licensed.tlb" : $(SOURCE) "$(OUTDIR)"
   $(MTL) /nologo /D "NDEBUG" /tlb "$(OUTDIR)/licensed.tlb" /win32 $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\licenctl.cpp
DEP_CPP_LICENC=\
	".\licenctl.h"\
	".\licenppg.h"\
	".\licensed.h"\
	".\stdafx.h"\
	

!IF  "$(CFG)" == "Licensed - Win32 Release"


"$(INTDIR)\licenctl.obj" : $(SOURCE) $(DEP_CPP_LICENC) "$(INTDIR)"\
 "$(INTDIR)\licensed.pch"


!ELSEIF  "$(CFG)" == "Licensed - Win32 Debug"


"$(INTDIR)\licenctl.obj" : $(SOURCE) $(DEP_CPP_LICENC) "$(INTDIR)"\
 "$(INTDIR)\licensed.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_LICENC) "$(INTDIR)"\
 "$(INTDIR)\licensed.pch"


!ELSEIF  "$(CFG)" == "Licensed - Win32 Unicode Debug"


"$(INTDIR)\licenctl.obj" : $(SOURCE) $(DEP_CPP_LICENC) "$(INTDIR)"\
 "$(INTDIR)\licensed.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_LICENC) "$(INTDIR)"\
 "$(INTDIR)\licensed.pch"


!ELSEIF  "$(CFG)" == "Licensed - Win32 Unicode Release"


"$(INTDIR)\licenctl.obj" : $(SOURCE) $(DEP_CPP_LICENC) "$(INTDIR)"\
 "$(INTDIR)\licensed.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\licenppg.cpp
DEP_CPP_LICENP=\
	".\licenppg.h"\
	".\licensed.h"\
	".\stdafx.h"\
	

!IF  "$(CFG)" == "Licensed - Win32 Release"


"$(INTDIR)\licenppg.obj" : $(SOURCE) $(DEP_CPP_LICENP) "$(INTDIR)"\
 "$(INTDIR)\licensed.pch"


!ELSEIF  "$(CFG)" == "Licensed - Win32 Debug"


"$(INTDIR)\licenppg.obj" : $(SOURCE) $(DEP_CPP_LICENP) "$(INTDIR)"\
 "$(INTDIR)\licensed.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_LICENP) "$(INTDIR)"\
 "$(INTDIR)\licensed.pch"


!ELSEIF  "$(CFG)" == "Licensed - Win32 Unicode Debug"


"$(INTDIR)\licenppg.obj" : $(SOURCE) $(DEP_CPP_LICENP) "$(INTDIR)"\
 "$(INTDIR)\licensed.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_LICENP) "$(INTDIR)"\
 "$(INTDIR)\licensed.pch"


!ELSEIF  "$(CFG)" == "Licensed - Win32 Unicode Release"


"$(INTDIR)\licenppg.obj" : $(SOURCE) $(DEP_CPP_LICENP) "$(INTDIR)"\
 "$(INTDIR)\licensed.pch"


!ENDIF 

# End Source File
# End Target
# End Project
################################################################################
