# Microsoft Visual C++ Generated NMAKE File, Format Version 40000
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

!IF "$(CFG)" == ""
CFG=Push - Win32 Debug
!MESSAGE No configuration specified.  Defaulting to Push - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "Push - Win32 Release" && "$(CFG)" != "Push - Win32 Debug" &&\
 "$(CFG)" != "Push - Win32 Unicode Debug" && "$(CFG)" !=\
 "Push - Win32 Unicode Release"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "push.mak" CFG="Push - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Push - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "Push - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "Push - Win32 Unicode Debug" (based on\
 "Win32 (x86) Dynamic-Link Library")
!MESSAGE "Push - Win32 Unicode Release" (based on\
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
# PROP Target_Last_Scanned "Push - Win32 Unicode Release"
RSC=rc.exe
MTL=mktyplib.exe
CPP=cl.exe

!IF  "$(CFG)" == "Push - Win32 Release"

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

ALL : "$(OUTDIR)\push.ocx" "$(OUTDIR)\regsvr32.trg"

CLEAN : 
	-@erase ".\Release\push.lib"
	-@erase ".\Release\pushctl.obj"
	-@erase ".\Release\push.pch"
	-@erase ".\Release\push.obj"
	-@erase ".\Release\pushppg.obj"
	-@erase ".\Release\stdafx.obj"
	-@erase ".\Release\push.res"
	-@erase ".\Release\push.tlb"
	-@erase ".\Release\push.exp"
	-@erase ".\Release\regsvr32.trg"
	-@erase ".\Release\push.ocx"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
CPP_PROJ=/nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/push.pch" /Yu"stdafx.h" /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\Release/
CPP_SBRS=
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/push.res" /d "NDEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/push.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 /nologo /subsystem:windows /dll /machine:I386
LINK32_FLAGS=/nologo /subsystem:windows /dll /incremental:no\
 /pdb:"$(OUTDIR)/push.pdb" /machine:I386 /def:".\push.def"\
 /out:"$(OUTDIR)/push.ocx" /implib:"$(OUTDIR)/push.lib" 
DEF_FILE= \
	".\push.def"
LINK32_OBJS= \
	"$(INTDIR)/pushctl.obj" \
	"$(INTDIR)/push.obj" \
	"$(INTDIR)/pushppg.obj" \
	"$(INTDIR)/stdafx.obj" \
	"$(INTDIR)/push.res"

"$(OUTDIR)\push.ocx" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

# Begin Custom Build - Registering OLE control...
OutDir=.\Release
TargetPath=.\Release\push.ocx
InputPath=.\Release\push.ocx
SOURCE=$(InputPath)

"$(OutDir)\regsvr32.trg" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   regsvr32 /s /c "$(TargetPath)"
   echo regsvr32 exec. time > "$(OutDir)\regsvr32.trg"

# End Custom Build

!ELSEIF  "$(CFG)" == "Push - Win32 Debug"

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

ALL : "$(OUTDIR)\push.ocx" "$(OUTDIR)\regsvr32.trg"

CLEAN : 
	-@erase ".\Debug\vc40.pdb"
	-@erase ".\Debug\push.pch"
	-@erase ".\Debug\push.ilk"
	-@erase ".\Debug\push.obj"
	-@erase ".\Debug\stdafx.obj"
	-@erase ".\Debug\pushctl.obj"
	-@erase ".\Debug\pushppg.obj"
	-@erase ".\Debug\push.res"
	-@erase ".\Debug\push.tlb"
	-@erase ".\Debug\push.lib"
	-@erase ".\Debug\push.exp"
	-@erase ".\Debug\push.pdb"
	-@erase ".\Debug\regsvr32.trg"
	-@erase ".\Debug\push.ocx"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
CPP_PROJ=/nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/push.pch" /Yu"stdafx.h" /Fo"$(INTDIR)/"\
 /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\Debug/
CPP_SBRS=
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/push.res" /d "_DEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/push.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /debug /machine:I386
# ADD LINK32 /nologo /subsystem:windows /dll /debug /machine:I386
LINK32_FLAGS=/nologo /subsystem:windows /dll /incremental:yes\
 /pdb:"$(OUTDIR)/push.pdb" /debug /machine:I386 /def:".\push.def"\
 /out:"$(OUTDIR)/push.ocx" /implib:"$(OUTDIR)/push.lib" 
DEF_FILE= \
	".\push.def"
LINK32_OBJS= \
	"$(INTDIR)/push.obj" \
	"$(INTDIR)/stdafx.obj" \
	"$(INTDIR)/pushctl.obj" \
	"$(INTDIR)/pushppg.obj" \
	"$(INTDIR)/push.res"

"$(OUTDIR)\push.ocx" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

# Begin Custom Build - Registering OLE control...
OutDir=.\Debug
TargetPath=.\Debug\push.ocx
InputPath=.\Debug\push.ocx
SOURCE=$(InputPath)

"$(OutDir)\regsvr32.trg" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   regsvr32 /s /c "$(TargetPath)"
   echo regsvr32 exec. time > "$(OutDir)\regsvr32.trg"

# End Custom Build

!ELSEIF  "$(CFG)" == "Push - Win32 Unicode Debug"

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

ALL : "$(OUTDIR)\push.ocx" "$(OUTDIR)\regsvr32.trg"

CLEAN : 
	-@erase ".\UniDebug\vc40.pdb"
	-@erase ".\UniDebug\push.pch"
	-@erase ".\UniDebug\push.ilk"
	-@erase ".\UniDebug\pushppg.obj"
	-@erase ".\UniDebug\stdafx.obj"
	-@erase ".\UniDebug\push.obj"
	-@erase ".\UniDebug\pushctl.obj"
	-@erase ".\UniDebug\push.res"
	-@erase ".\UniDebug\push.tlb"
	-@erase ".\UniDebug\push.lib"
	-@erase ".\UniDebug\push.exp"
	-@erase ".\UniDebug\push.pdb"
	-@erase ".\UniDebug\regsvr32.trg"
	-@erase ".\UniDebug\push.ocx"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_UNICODE" /Yu"stdafx.h" /c
CPP_PROJ=/nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /D "_AFXDLL" /D "_UNICODE" /Fp"$(INTDIR)/push.pch" /Yu"stdafx.h"\
 /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\UniDebug/
CPP_SBRS=
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/push.res" /d "_DEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/push.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /debug /machine:I386
# ADD LINK32 /nologo /subsystem:windows /dll /debug /machine:I386
LINK32_FLAGS=/nologo /subsystem:windows /dll /incremental:yes\
 /pdb:"$(OUTDIR)/push.pdb" /debug /machine:I386 /def:".\push.def"\
 /out:"$(OUTDIR)/push.ocx" /implib:"$(OUTDIR)/push.lib" 
DEF_FILE= \
	".\push.def"
LINK32_OBJS= \
	"$(INTDIR)/pushppg.obj" \
	"$(INTDIR)/stdafx.obj" \
	"$(INTDIR)/push.obj" \
	"$(INTDIR)/pushctl.obj" \
	"$(INTDIR)/push.res"

"$(OUTDIR)\push.ocx" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

# Begin Custom Build - Registering OLE control...
OutDir=.\UniDebug
TargetPath=.\UniDebug\push.ocx
InputPath=.\UniDebug\push.ocx
SOURCE=$(InputPath)

"$(OutDir)\regsvr32.trg" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   regsvr32 /s /c "$(TargetPath)"
   echo regsvr32 exec. time > "$(OutDir)\regsvr32.trg"

# End Custom Build

!ELSEIF  "$(CFG)" == "Push - Win32 Unicode Release"

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

ALL : "$(OUTDIR)\push.ocx" "$(OUTDIR)\regsvr32.trg"

CLEAN : 
	-@erase ".\UniRelease\push.lib"
	-@erase ".\UniRelease\stdafx.obj"
	-@erase ".\UniRelease\pushppg.obj"
	-@erase ".\UniRelease\push.pch"
	-@erase ".\UniRelease\push.obj"
	-@erase ".\UniRelease\pushctl.obj"
	-@erase ".\UniRelease\push.res"
	-@erase ".\UniRelease\push.tlb"
	-@erase ".\UniRelease\push.exp"
	-@erase ".\UniRelease\regsvr32.trg"
	-@erase ".\UniRelease\push.ocx"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_UNICODE" /Yu"stdafx.h" /c
CPP_PROJ=/nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_UNICODE" /Fp"$(INTDIR)/push.pch" /Yu"stdafx.h" /Fo"$(INTDIR)/"\
 /c 
CPP_OBJS=.\UniRelease/
CPP_SBRS=
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/push.res" /d "NDEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/push.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 /nologo /subsystem:windows /dll /machine:I386
LINK32_FLAGS=/nologo /subsystem:windows /dll /incremental:no\
 /pdb:"$(OUTDIR)/push.pdb" /machine:I386 /def:".\push.def"\
 /out:"$(OUTDIR)/push.ocx" /implib:"$(OUTDIR)/push.lib" 
DEF_FILE= \
	".\push.def"
LINK32_OBJS= \
	"$(INTDIR)/stdafx.obj" \
	"$(INTDIR)/pushppg.obj" \
	"$(INTDIR)/push.obj" \
	"$(INTDIR)/pushctl.obj" \
	"$(INTDIR)/push.res"

"$(OUTDIR)\push.ocx" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

# Begin Custom Build - Registering OLE control...
OutDir=.\UniRelease
TargetPath=.\UniRelease\push.ocx
InputPath=.\UniRelease\push.ocx
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

# Name "Push - Win32 Release"
# Name "Push - Win32 Debug"
# Name "Push - Win32 Unicode Debug"
# Name "Push - Win32 Unicode Release"

!IF  "$(CFG)" == "Push - Win32 Release"

!ELSEIF  "$(CFG)" == "Push - Win32 Debug"

!ELSEIF  "$(CFG)" == "Push - Win32 Unicode Debug"

!ELSEIF  "$(CFG)" == "Push - Win32 Unicode Release"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\ReadMe.txt

!IF  "$(CFG)" == "Push - Win32 Release"

!ELSEIF  "$(CFG)" == "Push - Win32 Debug"

!ELSEIF  "$(CFG)" == "Push - Win32 Unicode Debug"

!ELSEIF  "$(CFG)" == "Push - Win32 Unicode Release"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\stdafx.cpp
DEP_CPP_STDAF=\
	".\stdafx.h"\
	

!IF  "$(CFG)" == "Push - Win32 Release"

# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/push.pch" /Yc"stdafx.h" /Fo"$(INTDIR)/" /c\
 $(SOURCE) \
	

"$(INTDIR)\stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\push.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "Push - Win32 Debug"

# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/push.pch" /Yc"stdafx.h" /Fo"$(INTDIR)/"\
 /Fd"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\push.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "Push - Win32 Unicode Debug"

# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /D "_AFXDLL" /D "_UNICODE" /Fp"$(INTDIR)/push.pch" /Yc"stdafx.h"\
 /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\push.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "Push - Win32 Unicode Release"

# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_UNICODE" /Fp"$(INTDIR)/push.pch" /Yc"stdafx.h" /Fo"$(INTDIR)/"\
 /c $(SOURCE) \
	

"$(INTDIR)\stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\push.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\push.cpp

!IF  "$(CFG)" == "Push - Win32 Release"

DEP_CPP_PUSH_=\
	".\push.h"\
	".\stdafx.h"\
	

"$(INTDIR)\push.obj" : $(SOURCE) $(DEP_CPP_PUSH_) "$(INTDIR)"\
 "$(INTDIR)\push.pch"


!ELSEIF  "$(CFG)" == "Push - Win32 Debug"

DEP_CPP_PUSH_=\
	".\stdafx.h"\
	".\push.h"\
	".\resource.h"\
	

"$(INTDIR)\push.obj" : $(SOURCE) $(DEP_CPP_PUSH_) "$(INTDIR)"\
 "$(INTDIR)\push.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_PUSH_) "$(INTDIR)"\
 "$(INTDIR)\push.pch"


!ELSEIF  "$(CFG)" == "Push - Win32 Unicode Debug"

DEP_CPP_PUSH_=\
	".\stdafx.h"\
	".\push.h"\
	".\resource.h"\
	

"$(INTDIR)\push.obj" : $(SOURCE) $(DEP_CPP_PUSH_) "$(INTDIR)"\
 "$(INTDIR)\push.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_PUSH_) "$(INTDIR)"\
 "$(INTDIR)\push.pch"


!ELSEIF  "$(CFG)" == "Push - Win32 Unicode Release"

DEP_CPP_PUSH_=\
	".\stdafx.h"\
	".\push.h"\
	".\resource.h"\
	

"$(INTDIR)\push.obj" : $(SOURCE) $(DEP_CPP_PUSH_) "$(INTDIR)"\
 "$(INTDIR)\push.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\push.def

!IF  "$(CFG)" == "Push - Win32 Release"

!ELSEIF  "$(CFG)" == "Push - Win32 Debug"

!ELSEIF  "$(CFG)" == "Push - Win32 Unicode Debug"

!ELSEIF  "$(CFG)" == "Push - Win32 Unicode Release"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\push.rc

!IF  "$(CFG)" == "Push - Win32 Release"

DEP_RSC_PUSH_R=\
	".\Release\push.tlb"\
	".\push.ico"\
	".\pushctl.bmp"\
	".\pushcd.bmp"\
	".\pushcu.bmp"\
	".\resource.h"\
	".\push.rc2"\
	

"$(INTDIR)\push.res" : $(SOURCE) $(DEP_RSC_PUSH_R) "$(INTDIR)"\
 "$(INTDIR)\push.tlb"
   $(RSC) /l 0x409 /fo"$(INTDIR)/push.res" /i "Release" /d "NDEBUG" /d\
 "_AFXDLL" $(SOURCE)


!ELSEIF  "$(CFG)" == "Push - Win32 Debug"

DEP_RSC_PUSH_R=\
	".\push.ico"\
	".\pushctl.bmp"\
	".\pushcd.bmp"\
	".\pushcu.bmp"\
	".\resource.h"\
	".\push.rc2"\
	".\Debug\push.tlb"\
	

"$(INTDIR)\push.res" : $(SOURCE) $(DEP_RSC_PUSH_R) "$(INTDIR)"\
 "$(INTDIR)\push.tlb"
   $(RSC) /l 0x409 /fo"$(INTDIR)/push.res" /i "Debug" /d "_DEBUG" /d "_AFXDLL"\
 $(SOURCE)


!ELSEIF  "$(CFG)" == "Push - Win32 Unicode Debug"

DEP_RSC_PUSH_R=\
	".\push.ico"\
	".\pushctl.bmp"\
	".\pushcd.bmp"\
	".\pushcu.bmp"\
	".\resource.h"\
	".\push.rc2"\
	".\UniDebug\push.tlb"\
	

"$(INTDIR)\push.res" : $(SOURCE) $(DEP_RSC_PUSH_R) "$(INTDIR)"\
 "$(INTDIR)\push.tlb"
   $(RSC) /l 0x409 /fo"$(INTDIR)/push.res" /i "UniDebug" /d "_DEBUG" /d\
 "_AFXDLL" $(SOURCE)


!ELSEIF  "$(CFG)" == "Push - Win32 Unicode Release"

DEP_RSC_PUSH_R=\
	".\UniRelease\push.tlb"\
	".\push.ico"\
	".\pushctl.bmp"\
	".\pushcd.bmp"\
	".\pushcu.bmp"\
	".\resource.h"\
	".\push.rc2"\
	

"$(INTDIR)\push.res" : $(SOURCE) $(DEP_RSC_PUSH_R) "$(INTDIR)"\
 "$(INTDIR)\push.tlb"
   $(RSC) /l 0x409 /fo"$(INTDIR)/push.res" /i "UniRelease" /d "NDEBUG" /d\
 "_AFXDLL" $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\push.odl

!IF  "$(CFG)" == "Push - Win32 Release"


"$(OUTDIR)\push.tlb" : $(SOURCE) "$(OUTDIR)"
   $(MTL) /nologo /D "NDEBUG" /tlb "$(OUTDIR)/push.tlb" /win32 $(SOURCE)


!ELSEIF  "$(CFG)" == "Push - Win32 Debug"


"$(OUTDIR)\push.tlb" : $(SOURCE) "$(OUTDIR)"
   $(MTL) /nologo /D "_DEBUG" /tlb "$(OUTDIR)/push.tlb" /win32 $(SOURCE)


!ELSEIF  "$(CFG)" == "Push - Win32 Unicode Debug"


"$(OUTDIR)\push.tlb" : $(SOURCE) "$(OUTDIR)"
   $(MTL) /nologo /D "_DEBUG" /tlb "$(OUTDIR)/push.tlb" /win32 $(SOURCE)


!ELSEIF  "$(CFG)" == "Push - Win32 Unicode Release"


"$(OUTDIR)\push.tlb" : $(SOURCE) "$(OUTDIR)"
   $(MTL) /nologo /D "NDEBUG" /tlb "$(OUTDIR)/push.tlb" /win32 $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\pushctl.cpp

!IF  "$(CFG)" == "Push - Win32 Release"

DEP_CPP_PUSHC=\
	".\push.h"\
	".\pushctl.h"\
	".\pushppg.h"\
	".\stdafx.h"\
	

"$(INTDIR)\pushctl.obj" : $(SOURCE) $(DEP_CPP_PUSHC) "$(INTDIR)"\
 "$(INTDIR)\push.pch"


!ELSEIF  "$(CFG)" == "Push - Win32 Debug"

DEP_CPP_PUSHC=\
	".\stdafx.h"\
	".\push.h"\
	".\pushctl.h"\
	".\pushppg.h"\
	".\resource.h"\
	

"$(INTDIR)\pushctl.obj" : $(SOURCE) $(DEP_CPP_PUSHC) "$(INTDIR)"\
 "$(INTDIR)\push.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_PUSHC) "$(INTDIR)"\
 "$(INTDIR)\push.pch"


!ELSEIF  "$(CFG)" == "Push - Win32 Unicode Debug"

DEP_CPP_PUSHC=\
	".\stdafx.h"\
	".\push.h"\
	".\pushctl.h"\
	".\pushppg.h"\
	".\resource.h"\
	

"$(INTDIR)\pushctl.obj" : $(SOURCE) $(DEP_CPP_PUSHC) "$(INTDIR)"\
 "$(INTDIR)\push.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_PUSHC) "$(INTDIR)"\
 "$(INTDIR)\push.pch"


!ELSEIF  "$(CFG)" == "Push - Win32 Unicode Release"

DEP_CPP_PUSHC=\
	".\stdafx.h"\
	".\push.h"\
	".\pushctl.h"\
	".\pushppg.h"\
	".\resource.h"\
	

"$(INTDIR)\pushctl.obj" : $(SOURCE) $(DEP_CPP_PUSHC) "$(INTDIR)"\
 "$(INTDIR)\push.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\pushppg.cpp
DEP_CPP_PUSHP=\
	".\stdafx.h"\
	".\push.h"\
	".\pushppg.h"\
	".\resource.h"\
	

!IF  "$(CFG)" == "Push - Win32 Release"


"$(INTDIR)\pushppg.obj" : $(SOURCE) $(DEP_CPP_PUSHP) "$(INTDIR)"\
 "$(INTDIR)\push.pch"


!ELSEIF  "$(CFG)" == "Push - Win32 Debug"


"$(INTDIR)\pushppg.obj" : $(SOURCE) $(DEP_CPP_PUSHP) "$(INTDIR)"\
 "$(INTDIR)\push.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_PUSHP) "$(INTDIR)"\
 "$(INTDIR)\push.pch"


!ELSEIF  "$(CFG)" == "Push - Win32 Unicode Debug"


"$(INTDIR)\pushppg.obj" : $(SOURCE) $(DEP_CPP_PUSHP) "$(INTDIR)"\
 "$(INTDIR)\push.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_PUSHP) "$(INTDIR)"\
 "$(INTDIR)\push.pch"


!ELSEIF  "$(CFG)" == "Push - Win32 Unicode Release"


"$(INTDIR)\pushppg.obj" : $(SOURCE) $(DEP_CPP_PUSHP) "$(INTDIR)"\
 "$(INTDIR)\push.pch"


!ENDIF 

# End Source File
# End Target
# End Project
################################################################################
