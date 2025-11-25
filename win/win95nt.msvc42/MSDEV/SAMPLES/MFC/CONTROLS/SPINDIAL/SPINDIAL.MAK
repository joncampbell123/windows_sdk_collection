# Microsoft Visual C++ Generated NMAKE File, Format Version 40000
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

!IF "$(CFG)" == ""
CFG=SpinDial - Win32 Debug
!MESSAGE No configuration specified.  Defaulting to SpinDial - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "SpinDial - Win32 Release" && "$(CFG)" !=\
 "SpinDial - Win32 Debug" && "$(CFG)" != "SpinDial - Win32 Unicode Debug" &&\
 "$(CFG)" != "SpinDial - Win32 Unicode Release"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "spindial.mak" CFG="SpinDial - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "SpinDial - Win32 Release" (based on\
 "Win32 (x86) Dynamic-Link Library")
!MESSAGE "SpinDial - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "SpinDial - Win32 Unicode Debug" (based on\
 "Win32 (x86) Dynamic-Link Library")
!MESSAGE "SpinDial - Win32 Unicode Release" (based on\
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
# PROP Target_Last_Scanned "SpinDial - Win32 Unicode Release"
RSC=rc.exe
MTL=mktyplib.exe
CPP=cl.exe

!IF  "$(CFG)" == "SpinDial - Win32 Release"

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

ALL : "$(OUTDIR)\spindial.ocx" "$(OUTDIR)\regsvr32.trg"

CLEAN : 
	-@erase ".\Release\spindial.lib"
	-@erase ".\Release\stdafx.obj"
	-@erase ".\Release\spinppg.obj"
	-@erase ".\Release\spindial.pch"
	-@erase ".\Release\spinctl.obj"
	-@erase ".\Release\spindial.obj"
	-@erase ".\Release\spindial.res"
	-@erase ".\Release\spindial.tlb"
	-@erase ".\Release\spindial.exp"
	-@erase ".\Release\regsvr32.trg"
	-@erase ".\Release\spindial.ocx"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
CPP_PROJ=/nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/spindial.pch" /Yu"stdafx.h" /Fo"$(INTDIR)/"\
 /c 
CPP_OBJS=.\Release/
CPP_SBRS=
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/spindial.res" /d "NDEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/spindial.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 /nologo /subsystem:windows /dll /machine:I386
LINK32_FLAGS=/nologo /subsystem:windows /dll /incremental:no\
 /pdb:"$(OUTDIR)/spindial.pdb" /machine:I386 /def:".\spindial.def"\
 /out:"$(OUTDIR)/spindial.ocx" /implib:"$(OUTDIR)/spindial.lib" 
DEF_FILE= \
	".\spindial.def"
LINK32_OBJS= \
	"$(INTDIR)/stdafx.obj" \
	"$(INTDIR)/spinppg.obj" \
	"$(INTDIR)/spinctl.obj" \
	"$(INTDIR)/spindial.obj" \
	"$(INTDIR)/spindial.res"

"$(OUTDIR)\spindial.ocx" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

# Begin Custom Build - Registering OLE control...
OutDir=.\Release
TargetPath=.\Release\spindial.ocx
InputPath=.\Release\spindial.ocx
SOURCE=$(InputPath)

"$(OutDir)\regsvr32.trg" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   regsvr32 /s /c "$(TargetPath)"
   echo regsvr32 exec. time > "$(OutDir)\regsvr32.trg"

# End Custom Build

!ELSEIF  "$(CFG)" == "SpinDial - Win32 Debug"

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

ALL : "$(OUTDIR)\spindial.ocx" "$(OUTDIR)\regsvr32.trg"

CLEAN : 
	-@erase ".\Debug\vc40.pdb"
	-@erase ".\Debug\spindial.pch"
	-@erase ".\Debug\spindial.ilk"
	-@erase ".\Debug\spinppg.obj"
	-@erase ".\Debug\stdafx.obj"
	-@erase ".\Debug\spindial.obj"
	-@erase ".\Debug\spinctl.obj"
	-@erase ".\Debug\spindial.res"
	-@erase ".\Debug\spindial.tlb"
	-@erase ".\Debug\spindial.lib"
	-@erase ".\Debug\spindial.exp"
	-@erase ".\Debug\spindial.pdb"
	-@erase ".\Debug\regsvr32.trg"
	-@erase ".\Debug\spindial.ocx"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
CPP_PROJ=/nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/spindial.pch" /Yu"stdafx.h"\
 /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\Debug/
CPP_SBRS=
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/spindial.res" /d "_DEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/spindial.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /debug /machine:I386
# ADD LINK32 /nologo /subsystem:windows /dll /debug /machine:I386
LINK32_FLAGS=/nologo /subsystem:windows /dll /incremental:yes\
 /pdb:"$(OUTDIR)/spindial.pdb" /debug /machine:I386 /def:".\spindial.def"\
 /out:"$(OUTDIR)/spindial.ocx" /implib:"$(OUTDIR)/spindial.lib" 
DEF_FILE= \
	".\spindial.def"
LINK32_OBJS= \
	"$(INTDIR)/spinppg.obj" \
	"$(INTDIR)/stdafx.obj" \
	"$(INTDIR)/spindial.obj" \
	"$(INTDIR)/spinctl.obj" \
	"$(INTDIR)/spindial.res"

"$(OUTDIR)\spindial.ocx" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

# Begin Custom Build - Registering OLE control...
OutDir=.\Debug
TargetPath=.\Debug\spindial.ocx
InputPath=.\Debug\spindial.ocx
SOURCE=$(InputPath)

"$(OutDir)\regsvr32.trg" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   regsvr32 /s /c "$(TargetPath)"
   echo regsvr32 exec. time > "$(OutDir)\regsvr32.trg"

# End Custom Build

!ELSEIF  "$(CFG)" == "SpinDial - Win32 Unicode Debug"

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

ALL : "$(OUTDIR)\spindial.ocx" "$(OUTDIR)\regsvr32.trg"

CLEAN : 
	-@erase ".\UniDebug\vc40.pdb"
	-@erase ".\UniDebug\spindial.pch"
	-@erase ".\UniDebug\spindial.ilk"
	-@erase ".\UniDebug\spindial.obj"
	-@erase ".\UniDebug\stdafx.obj"
	-@erase ".\UniDebug\spinctl.obj"
	-@erase ".\UniDebug\spinppg.obj"
	-@erase ".\UniDebug\spindial.res"
	-@erase ".\UniDebug\spindial.tlb"
	-@erase ".\UniDebug\spindial.lib"
	-@erase ".\UniDebug\spindial.exp"
	-@erase ".\UniDebug\spindial.pdb"
	-@erase ".\UniDebug\regsvr32.trg"
	-@erase ".\UniDebug\spindial.ocx"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_UNICODE" /Yu"stdafx.h" /c
CPP_PROJ=/nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /D "_AFXDLL" /D "_UNICODE" /Fp"$(INTDIR)/spindial.pch" /Yu"stdafx.h"\
 /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\UniDebug/
CPP_SBRS=
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/spindial.res" /d "_DEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/spindial.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /debug /machine:I386
# ADD LINK32 /nologo /subsystem:windows /dll /debug /machine:I386
LINK32_FLAGS=/nologo /subsystem:windows /dll /incremental:yes\
 /pdb:"$(OUTDIR)/spindial.pdb" /debug /machine:I386 /def:".\spindial.def"\
 /out:"$(OUTDIR)/spindial.ocx" /implib:"$(OUTDIR)/spindial.lib" 
DEF_FILE= \
	".\spindial.def"
LINK32_OBJS= \
	"$(INTDIR)/spindial.obj" \
	"$(INTDIR)/stdafx.obj" \
	"$(INTDIR)/spinctl.obj" \
	"$(INTDIR)/spinppg.obj" \
	"$(INTDIR)/spindial.res"

"$(OUTDIR)\spindial.ocx" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

# Begin Custom Build - Registering OLE control...
OutDir=.\UniDebug
TargetPath=.\UniDebug\spindial.ocx
InputPath=.\UniDebug\spindial.ocx
SOURCE=$(InputPath)

"$(OutDir)\regsvr32.trg" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   regsvr32 /s /c "$(TargetPath)"
   echo regsvr32 exec. time > "$(OutDir)\regsvr32.trg"

# End Custom Build

!ELSEIF  "$(CFG)" == "SpinDial - Win32 Unicode Release"

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

ALL : "$(OUTDIR)\spindial.ocx" "$(OUTDIR)\regsvr32.trg"

CLEAN : 
	-@erase ".\UniRelease\spindial.lib"
	-@erase ".\UniRelease\spinctl.obj"
	-@erase ".\UniRelease\spindial.pch"
	-@erase ".\UniRelease\spindial.obj"
	-@erase ".\UniRelease\spinppg.obj"
	-@erase ".\UniRelease\stdafx.obj"
	-@erase ".\UniRelease\spindial.res"
	-@erase ".\UniRelease\spindial.tlb"
	-@erase ".\UniRelease\spindial.exp"
	-@erase ".\UniRelease\regsvr32.trg"
	-@erase ".\UniRelease\spindial.ocx"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_UNICODE" /Yu"stdafx.h" /c
CPP_PROJ=/nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_UNICODE" /Fp"$(INTDIR)/spindial.pch" /Yu"stdafx.h"\
 /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\UniRelease/
CPP_SBRS=
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/spindial.res" /d "NDEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/spindial.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 /nologo /subsystem:windows /dll /machine:I386
LINK32_FLAGS=/nologo /subsystem:windows /dll /incremental:no\
 /pdb:"$(OUTDIR)/spindial.pdb" /machine:I386 /def:".\spindial.def"\
 /out:"$(OUTDIR)/spindial.ocx" /implib:"$(OUTDIR)/spindial.lib" 
DEF_FILE= \
	".\spindial.def"
LINK32_OBJS= \
	"$(INTDIR)/spinctl.obj" \
	"$(INTDIR)/spindial.obj" \
	"$(INTDIR)/spinppg.obj" \
	"$(INTDIR)/stdafx.obj" \
	"$(INTDIR)/spindial.res"

"$(OUTDIR)\spindial.ocx" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

# Begin Custom Build - Registering OLE control...
OutDir=.\UniRelease
TargetPath=.\UniRelease\spindial.ocx
InputPath=.\UniRelease\spindial.ocx
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

# Name "SpinDial - Win32 Release"
# Name "SpinDial - Win32 Debug"
# Name "SpinDial - Win32 Unicode Debug"
# Name "SpinDial - Win32 Unicode Release"

!IF  "$(CFG)" == "SpinDial - Win32 Release"

!ELSEIF  "$(CFG)" == "SpinDial - Win32 Debug"

!ELSEIF  "$(CFG)" == "SpinDial - Win32 Unicode Debug"

!ELSEIF  "$(CFG)" == "SpinDial - Win32 Unicode Release"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\ReadMe.txt

!IF  "$(CFG)" == "SpinDial - Win32 Release"

!ELSEIF  "$(CFG)" == "SpinDial - Win32 Debug"

!ELSEIF  "$(CFG)" == "SpinDial - Win32 Unicode Debug"

!ELSEIF  "$(CFG)" == "SpinDial - Win32 Unicode Release"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\stdafx.cpp
DEP_CPP_STDAF=\
	".\stdafx.h"\
	

!IF  "$(CFG)" == "SpinDial - Win32 Release"

# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/spindial.pch" /Yc"stdafx.h" /Fo"$(INTDIR)/"\
 /c $(SOURCE) \
	

"$(INTDIR)\stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\spindial.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "SpinDial - Win32 Debug"

# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/spindial.pch" /Yc"stdafx.h"\
 /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\spindial.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "SpinDial - Win32 Unicode Debug"

# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /D "_AFXDLL" /D "_UNICODE" /Fp"$(INTDIR)/spindial.pch" /Yc"stdafx.h"\
 /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\spindial.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "SpinDial - Win32 Unicode Release"

# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_UNICODE" /Fp"$(INTDIR)/spindial.pch" /Yc"stdafx.h"\
 /Fo"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\spindial.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\spindial.cpp
DEP_CPP_SPIND=\
	".\spindial.h"\
	".\stdafx.h"\
	

!IF  "$(CFG)" == "SpinDial - Win32 Release"


"$(INTDIR)\spindial.obj" : $(SOURCE) $(DEP_CPP_SPIND) "$(INTDIR)"\
 "$(INTDIR)\spindial.pch"


!ELSEIF  "$(CFG)" == "SpinDial - Win32 Debug"


"$(INTDIR)\spindial.obj" : $(SOURCE) $(DEP_CPP_SPIND) "$(INTDIR)"\
 "$(INTDIR)\spindial.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_SPIND) "$(INTDIR)"\
 "$(INTDIR)\spindial.pch"


!ELSEIF  "$(CFG)" == "SpinDial - Win32 Unicode Debug"


"$(INTDIR)\spindial.obj" : $(SOURCE) $(DEP_CPP_SPIND) "$(INTDIR)"\
 "$(INTDIR)\spindial.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_SPIND) "$(INTDIR)"\
 "$(INTDIR)\spindial.pch"


!ELSEIF  "$(CFG)" == "SpinDial - Win32 Unicode Release"


"$(INTDIR)\spindial.obj" : $(SOURCE) $(DEP_CPP_SPIND) "$(INTDIR)"\
 "$(INTDIR)\spindial.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\spindial.def

!IF  "$(CFG)" == "SpinDial - Win32 Release"

!ELSEIF  "$(CFG)" == "SpinDial - Win32 Debug"

!ELSEIF  "$(CFG)" == "SpinDial - Win32 Unicode Debug"

!ELSEIF  "$(CFG)" == "SpinDial - Win32 Unicode Release"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\spindial.rc

!IF  "$(CFG)" == "SpinDial - Win32 Release"

DEP_RSC_SPINDI=\
	".\spindial.ico"\
	".\spinctl.bmp"\
	".\resource.h"\
	".\spindial.rc2"\
	".\Release\spindial.tlb"\
	

"$(INTDIR)\spindial.res" : $(SOURCE) $(DEP_RSC_SPINDI) "$(INTDIR)"\
 "$(INTDIR)\spindial.tlb"
   $(RSC) /l 0x409 /fo"$(INTDIR)/spindial.res" /i "Release" /d "NDEBUG" /d\
 "_AFXDLL" $(SOURCE)


!ELSEIF  "$(CFG)" == "SpinDial - Win32 Debug"

DEP_RSC_SPINDI=\
	".\spindial.ico"\
	".\spinctl.bmp"\
	".\resource.h"\
	".\spindial.rc2"\
	".\Debug\spindial.tlb"\
	

"$(INTDIR)\spindial.res" : $(SOURCE) $(DEP_RSC_SPINDI) "$(INTDIR)"\
 "$(INTDIR)\spindial.tlb"
   $(RSC) /l 0x409 /fo"$(INTDIR)/spindial.res" /i "Debug" /d "_DEBUG" /d\
 "_AFXDLL" $(SOURCE)


!ELSEIF  "$(CFG)" == "SpinDial - Win32 Unicode Debug"

DEP_RSC_SPINDI=\
	".\UniDebug\spindial.tlb"\
	".\spindial.ico"\
	".\spinctl.bmp"\
	".\resource.h"\
	".\spindial.rc2"\
	

"$(INTDIR)\spindial.res" : $(SOURCE) $(DEP_RSC_SPINDI) "$(INTDIR)"\
 "$(INTDIR)\spindial.tlb"
   $(RSC) /l 0x409 /fo"$(INTDIR)/spindial.res" /i "UniDebug" /d "_DEBUG" /d\
 "_AFXDLL" $(SOURCE)


!ELSEIF  "$(CFG)" == "SpinDial - Win32 Unicode Release"

DEP_RSC_SPINDI=\
	".\spindial.ico"\
	".\spinctl.bmp"\
	".\resource.h"\
	".\spindial.rc2"\
	".\UniRelease\spindial.tlb"\
	

"$(INTDIR)\spindial.res" : $(SOURCE) $(DEP_RSC_SPINDI) "$(INTDIR)"\
 "$(INTDIR)\spindial.tlb"
   $(RSC) /l 0x409 /fo"$(INTDIR)/spindial.res" /i "UniRelease" /d "NDEBUG" /d\
 "_AFXDLL" $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\spindial.odl

!IF  "$(CFG)" == "SpinDial - Win32 Release"


"$(OUTDIR)\spindial.tlb" : $(SOURCE) "$(OUTDIR)"
   $(MTL) /nologo /D "NDEBUG" /tlb "$(OUTDIR)/spindial.tlb" /win32 $(SOURCE)


!ELSEIF  "$(CFG)" == "SpinDial - Win32 Debug"


"$(OUTDIR)\spindial.tlb" : $(SOURCE) "$(OUTDIR)"
   $(MTL) /nologo /D "_DEBUG" /tlb "$(OUTDIR)/spindial.tlb" /win32 $(SOURCE)


!ELSEIF  "$(CFG)" == "SpinDial - Win32 Unicode Debug"


"$(OUTDIR)\spindial.tlb" : $(SOURCE) "$(OUTDIR)"
   $(MTL) /nologo /D "_DEBUG" /tlb "$(OUTDIR)/spindial.tlb" /win32 $(SOURCE)


!ELSEIF  "$(CFG)" == "SpinDial - Win32 Unicode Release"


"$(OUTDIR)\spindial.tlb" : $(SOURCE) "$(OUTDIR)"
   $(MTL) /nologo /D "NDEBUG" /tlb "$(OUTDIR)/spindial.tlb" /win32 $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\spinctl.cpp
DEP_CPP_SPINC=\
	".\spinctl.h"\
	".\spindial.h"\
	".\spinppg.h"\
	".\stdafx.h"\
	

!IF  "$(CFG)" == "SpinDial - Win32 Release"


"$(INTDIR)\spinctl.obj" : $(SOURCE) $(DEP_CPP_SPINC) "$(INTDIR)"\
 "$(INTDIR)\spindial.pch"


!ELSEIF  "$(CFG)" == "SpinDial - Win32 Debug"


"$(INTDIR)\spinctl.obj" : $(SOURCE) $(DEP_CPP_SPINC) "$(INTDIR)"\
 "$(INTDIR)\spindial.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_SPINC) "$(INTDIR)"\
 "$(INTDIR)\spindial.pch"


!ELSEIF  "$(CFG)" == "SpinDial - Win32 Unicode Debug"


"$(INTDIR)\spinctl.obj" : $(SOURCE) $(DEP_CPP_SPINC) "$(INTDIR)"\
 "$(INTDIR)\spindial.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_SPINC) "$(INTDIR)"\
 "$(INTDIR)\spindial.pch"


!ELSEIF  "$(CFG)" == "SpinDial - Win32 Unicode Release"


"$(INTDIR)\spinctl.obj" : $(SOURCE) $(DEP_CPP_SPINC) "$(INTDIR)"\
 "$(INTDIR)\spindial.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\spinppg.cpp
DEP_CPP_SPINP=\
	".\spindial.h"\
	".\spinppg.h"\
	".\stdafx.h"\
	

!IF  "$(CFG)" == "SpinDial - Win32 Release"


"$(INTDIR)\spinppg.obj" : $(SOURCE) $(DEP_CPP_SPINP) "$(INTDIR)"\
 "$(INTDIR)\spindial.pch"


!ELSEIF  "$(CFG)" == "SpinDial - Win32 Debug"


"$(INTDIR)\spinppg.obj" : $(SOURCE) $(DEP_CPP_SPINP) "$(INTDIR)"\
 "$(INTDIR)\spindial.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_SPINP) "$(INTDIR)"\
 "$(INTDIR)\spindial.pch"


!ELSEIF  "$(CFG)" == "SpinDial - Win32 Unicode Debug"


"$(INTDIR)\spinppg.obj" : $(SOURCE) $(DEP_CPP_SPINP) "$(INTDIR)"\
 "$(INTDIR)\spindial.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_SPINP) "$(INTDIR)"\
 "$(INTDIR)\spindial.pch"


!ELSEIF  "$(CFG)" == "SpinDial - Win32 Unicode Release"


"$(INTDIR)\spinppg.obj" : $(SOURCE) $(DEP_CPP_SPINP) "$(INTDIR)"\
 "$(INTDIR)\spindial.pch"


!ENDIF 

# End Source File
# End Target
# End Project
################################################################################
