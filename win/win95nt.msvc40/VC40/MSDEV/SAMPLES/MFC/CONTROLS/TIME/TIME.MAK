# Microsoft Visual C++ Generated NMAKE File, Format Version 40000
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

!IF "$(CFG)" == ""
CFG=Time - Win32 Debug
!MESSAGE No configuration specified.  Defaulting to Time - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "Time - Win32 Release" && "$(CFG)" != "Time - Win32 Debug" &&\
 "$(CFG)" != "Time - Win32 Unicode Debug" && "$(CFG)" !=\
 "Time - Win32 Unicode Release"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "time.mak" CFG="Time - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Time - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "Time - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "Time - Win32 Unicode Debug" (based on\
 "Win32 (x86) Dynamic-Link Library")
!MESSAGE "Time - Win32 Unicode Release" (based on\
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
# PROP Target_Last_Scanned "Time - Win32 Unicode Release"
CPP=cl.exe
RSC=rc.exe
MTL=mktyplib.exe

!IF  "$(CFG)" == "Time - Win32 Release"

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

ALL : "$(OUTDIR)\time.ocx" "$(OUTDIR)\regsvr32.trg"

CLEAN : 
	-@erase ".\Release\time.lib"
	-@erase ".\Release\timeppg.obj"
	-@erase ".\Release\time.pch"
	-@erase ".\Release\time.obj"
	-@erase ".\Release\timectl.obj"
	-@erase ".\Release\stdafx.obj"
	-@erase ".\Release\time.res"
	-@erase ".\Release\time.tlb"
	-@erase ".\Release\time.exp"
	-@erase ".\Release\regsvr32.trg"
	-@erase ".\Release\time.ocx"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
CPP_PROJ=/nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/time.pch" /Yu"stdafx.h" /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\Release/
CPP_SBRS=
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/time.res" /d "NDEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/time.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 /nologo /subsystem:windows /dll /machine:I386
LINK32_FLAGS=/nologo /subsystem:windows /dll /incremental:no\
 /pdb:"$(OUTDIR)/time.pdb" /machine:I386 /def:".\time.def"\
 /out:"$(OUTDIR)/time.ocx" /implib:"$(OUTDIR)/time.lib" 
DEF_FILE= \
	".\time.def"
LINK32_OBJS= \
	"$(INTDIR)/timeppg.obj" \
	"$(INTDIR)/time.obj" \
	"$(INTDIR)/timectl.obj" \
	"$(INTDIR)/stdafx.obj" \
	"$(INTDIR)/time.res"

"$(OUTDIR)\time.ocx" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

# Begin Custom Build - Registering OLE control...
OutDir=.\Release
TargetPath=.\Release\time.ocx
InputPath=.\Release\time.ocx
SOURCE=$(InputPath)

"$(OutDir)\regsvr32.trg" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   regsvr32 /s /c "$(TargetPath)"
   echo regsvr32 exec. time > "$(OutDir)\regsvr32.trg"

# End Custom Build

!ELSEIF  "$(CFG)" == "Time - Win32 Debug"

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

ALL : "$(OUTDIR)\time.ocx" "$(OUTDIR)\regsvr32.trg"

CLEAN : 
	-@erase ".\Debug\vc40.pdb"
	-@erase ".\Debug\time.pch"
	-@erase ".\Debug\time.ilk"
	-@erase ".\Debug\timectl.obj"
	-@erase ".\Debug\stdafx.obj"
	-@erase ".\Debug\time.obj"
	-@erase ".\Debug\timeppg.obj"
	-@erase ".\Debug\time.res"
	-@erase ".\Debug\time.tlb"
	-@erase ".\Debug\time.lib"
	-@erase ".\Debug\time.exp"
	-@erase ".\Debug\time.pdb"
	-@erase ".\Debug\regsvr32.trg"
	-@erase ".\Debug\time.ocx"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
CPP_PROJ=/nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/time.pch" /Yu"stdafx.h" /Fo"$(INTDIR)/"\
 /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\Debug/
CPP_SBRS=
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/time.res" /d "_DEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/time.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /debug /machine:I386
# ADD LINK32 /nologo /subsystem:windows /dll /debug /machine:I386
LINK32_FLAGS=/nologo /subsystem:windows /dll /incremental:yes\
 /pdb:"$(OUTDIR)/time.pdb" /debug /machine:I386 /def:".\time.def"\
 /out:"$(OUTDIR)/time.ocx" /implib:"$(OUTDIR)/time.lib" 
DEF_FILE= \
	".\time.def"
LINK32_OBJS= \
	"$(INTDIR)/timectl.obj" \
	"$(INTDIR)/stdafx.obj" \
	"$(INTDIR)/time.obj" \
	"$(INTDIR)/timeppg.obj" \
	"$(INTDIR)/time.res"

"$(OUTDIR)\time.ocx" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

# Begin Custom Build - Registering OLE control...
OutDir=.\Debug
TargetPath=.\Debug\time.ocx
InputPath=.\Debug\time.ocx
SOURCE=$(InputPath)

"$(OutDir)\regsvr32.trg" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   regsvr32 /s /c "$(TargetPath)"
   echo regsvr32 exec. time > "$(OutDir)\regsvr32.trg"

# End Custom Build

!ELSEIF  "$(CFG)" == "Time - Win32 Unicode Debug"

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

ALL : "$(OUTDIR)\time.ocx" "$(OUTDIR)\regsvr32.trg"

CLEAN : 
	-@erase ".\UniDebug\vc40.pdb"
	-@erase ".\UniDebug\time.pch"
	-@erase ".\UniDebug\time.ilk"
	-@erase ".\UniDebug\timeppg.obj"
	-@erase ".\UniDebug\stdafx.obj"
	-@erase ".\UniDebug\time.obj"
	-@erase ".\UniDebug\timectl.obj"
	-@erase ".\UniDebug\time.res"
	-@erase ".\UniDebug\time.tlb"
	-@erase ".\UniDebug\time.lib"
	-@erase ".\UniDebug\time.exp"
	-@erase ".\UniDebug\time.pdb"
	-@erase ".\UniDebug\regsvr32.trg"
	-@erase ".\UniDebug\time.ocx"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_UNICODE" /Yu"stdafx.h" /c
CPP_PROJ=/nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /D "_AFXDLL" /D "_UNICODE" /Fp"$(INTDIR)/time.pch" /Yu"stdafx.h"\
 /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\UniDebug/
CPP_SBRS=
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/time.res" /d "_DEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/time.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /debug /machine:I386
# ADD LINK32 /nologo /subsystem:windows /dll /debug /machine:I386
LINK32_FLAGS=/nologo /subsystem:windows /dll /incremental:yes\
 /pdb:"$(OUTDIR)/time.pdb" /debug /machine:I386 /def:".\time.def"\
 /out:"$(OUTDIR)/time.ocx" /implib:"$(OUTDIR)/time.lib" 
DEF_FILE= \
	".\time.def"
LINK32_OBJS= \
	"$(INTDIR)/timeppg.obj" \
	"$(INTDIR)/stdafx.obj" \
	"$(INTDIR)/time.obj" \
	"$(INTDIR)/timectl.obj" \
	"$(INTDIR)/time.res"

"$(OUTDIR)\time.ocx" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

# Begin Custom Build - Registering OLE control...
OutDir=.\UniDebug
TargetPath=.\UniDebug\time.ocx
InputPath=.\UniDebug\time.ocx
SOURCE=$(InputPath)

"$(OutDir)\regsvr32.trg" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   regsvr32 /s /c "$(TargetPath)"
   echo regsvr32 exec. time > "$(OutDir)\regsvr32.trg"

# End Custom Build

!ELSEIF  "$(CFG)" == "Time - Win32 Unicode Release"

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

ALL : "$(OUTDIR)\time.ocx" "$(OUTDIR)\regsvr32.trg"

CLEAN : 
	-@erase ".\UniRelease\time.lib"
	-@erase ".\UniRelease\timeppg.obj"
	-@erase ".\UniRelease\time.pch"
	-@erase ".\UniRelease\stdafx.obj"
	-@erase ".\UniRelease\timectl.obj"
	-@erase ".\UniRelease\time.obj"
	-@erase ".\UniRelease\time.res"
	-@erase ".\UniRelease\time.tlb"
	-@erase ".\UniRelease\time.exp"
	-@erase ".\UniRelease\regsvr32.trg"
	-@erase ".\UniRelease\time.ocx"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_UNICODE" /Yu"stdafx.h" /c
CPP_PROJ=/nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_UNICODE" /Fp"$(INTDIR)/time.pch" /Yu"stdafx.h" /Fo"$(INTDIR)/"\
 /c 
CPP_OBJS=.\UniRelease/
CPP_SBRS=
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/time.res" /d "NDEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/time.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 /nologo /subsystem:windows /dll /machine:I386
LINK32_FLAGS=/nologo /subsystem:windows /dll /incremental:no\
 /pdb:"$(OUTDIR)/time.pdb" /machine:I386 /def:".\time.def"\
 /out:"$(OUTDIR)/time.ocx" /implib:"$(OUTDIR)/time.lib" 
DEF_FILE= \
	".\time.def"
LINK32_OBJS= \
	"$(INTDIR)/timeppg.obj" \
	"$(INTDIR)/stdafx.obj" \
	"$(INTDIR)/timectl.obj" \
	"$(INTDIR)/time.obj" \
	"$(INTDIR)/time.res"

"$(OUTDIR)\time.ocx" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

# Begin Custom Build - Registering OLE control...
OutDir=.\UniRelease
TargetPath=.\UniRelease\time.ocx
InputPath=.\UniRelease\time.ocx
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

# Name "Time - Win32 Release"
# Name "Time - Win32 Debug"
# Name "Time - Win32 Unicode Debug"
# Name "Time - Win32 Unicode Release"

!IF  "$(CFG)" == "Time - Win32 Release"

!ELSEIF  "$(CFG)" == "Time - Win32 Debug"

!ELSEIF  "$(CFG)" == "Time - Win32 Unicode Debug"

!ELSEIF  "$(CFG)" == "Time - Win32 Unicode Release"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\ReadMe.txt

!IF  "$(CFG)" == "Time - Win32 Release"

!ELSEIF  "$(CFG)" == "Time - Win32 Debug"

!ELSEIF  "$(CFG)" == "Time - Win32 Unicode Debug"

!ELSEIF  "$(CFG)" == "Time - Win32 Unicode Release"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\stdafx.cpp
DEP_CPP_STDAF=\
	".\stdafx.h"\
	

!IF  "$(CFG)" == "Time - Win32 Release"

# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/time.pch" /Yc"stdafx.h" /Fo"$(INTDIR)/" /c\
 $(SOURCE) \
	

"$(INTDIR)\stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\time.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "Time - Win32 Debug"

# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/time.pch" /Yc"stdafx.h" /Fo"$(INTDIR)/"\
 /Fd"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\time.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "Time - Win32 Unicode Debug"

# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /D "_AFXDLL" /D "_UNICODE" /Fp"$(INTDIR)/time.pch" /Yc"stdafx.h"\
 /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\time.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "Time - Win32 Unicode Release"

# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_UNICODE" /Fp"$(INTDIR)/time.pch" /Yc"stdafx.h" /Fo"$(INTDIR)/"\
 /c $(SOURCE) \
	

"$(INTDIR)\stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\time.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\time.cpp
DEP_CPP_TIME_=\
	".\stdafx.h"\
	".\time.h"\
	".\resource.h"\
	

!IF  "$(CFG)" == "Time - Win32 Release"


"$(INTDIR)\time.obj" : $(SOURCE) $(DEP_CPP_TIME_) "$(INTDIR)"\
 "$(INTDIR)\time.pch"


!ELSEIF  "$(CFG)" == "Time - Win32 Debug"


"$(INTDIR)\time.obj" : $(SOURCE) $(DEP_CPP_TIME_) "$(INTDIR)"\
 "$(INTDIR)\time.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_TIME_) "$(INTDIR)"\
 "$(INTDIR)\time.pch"


!ELSEIF  "$(CFG)" == "Time - Win32 Unicode Debug"


"$(INTDIR)\time.obj" : $(SOURCE) $(DEP_CPP_TIME_) "$(INTDIR)"\
 "$(INTDIR)\time.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_TIME_) "$(INTDIR)"\
 "$(INTDIR)\time.pch"


!ELSEIF  "$(CFG)" == "Time - Win32 Unicode Release"


"$(INTDIR)\time.obj" : $(SOURCE) $(DEP_CPP_TIME_) "$(INTDIR)"\
 "$(INTDIR)\time.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\time.def

!IF  "$(CFG)" == "Time - Win32 Release"

!ELSEIF  "$(CFG)" == "Time - Win32 Debug"

!ELSEIF  "$(CFG)" == "Time - Win32 Unicode Debug"

!ELSEIF  "$(CFG)" == "Time - Win32 Unicode Release"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\time.rc

!IF  "$(CFG)" == "Time - Win32 Release"

DEP_RSC_TIME_R=\
	".\time.ico"\
	".\timectl.bmp"\
	".\CLOCKPIC.BMP"\
	".\resource.h"\
	".\time.rc2"\
	
NODEP_RSC_TIME_R=\
	".\Release\time.tlb"\
	

"$(INTDIR)\time.res" : $(SOURCE) $(DEP_RSC_TIME_R) "$(INTDIR)"\
 "$(INTDIR)\Time.tlb"
   $(RSC) /l 0x409 /fo"$(INTDIR)/time.res" /i "Release" /d "NDEBUG" /d\
 "_AFXDLL" $(SOURCE)


!ELSEIF  "$(CFG)" == "Time - Win32 Debug"

DEP_RSC_TIME_R=\
	".\time.ico"\
	".\timectl.bmp"\
	".\CLOCKPIC.BMP"\
	".\resource.h"\
	".\time.rc2"\
	".\Debug\time.tlb"\
	

"$(INTDIR)\time.res" : $(SOURCE) $(DEP_RSC_TIME_R) "$(INTDIR)"\
 "$(INTDIR)\Time.tlb"
   $(RSC) /l 0x409 /fo"$(INTDIR)/time.res" /i "Debug" /d "_DEBUG" /d "_AFXDLL"\
 $(SOURCE)


!ELSEIF  "$(CFG)" == "Time - Win32 Unicode Debug"

DEP_RSC_TIME_R=\
	".\time.ico"\
	".\timectl.bmp"\
	".\CLOCKPIC.BMP"\
	".\resource.h"\
	".\time.rc2"\
	".\UniDebug\time.tlb"\
	

"$(INTDIR)\time.res" : $(SOURCE) $(DEP_RSC_TIME_R) "$(INTDIR)"\
 "$(INTDIR)\Time.tlb"
   $(RSC) /l 0x409 /fo"$(INTDIR)/time.res" /i "UniDebug" /d "_DEBUG" /d\
 "_AFXDLL" $(SOURCE)


!ELSEIF  "$(CFG)" == "Time - Win32 Unicode Release"

DEP_RSC_TIME_R=\
	".\time.ico"\
	".\timectl.bmp"\
	".\CLOCKPIC.BMP"\
	".\resource.h"\
	".\time.rc2"\
	
NODEP_RSC_TIME_R=\
	".\UniRelease\time.tlb"\
	

"$(INTDIR)\time.res" : $(SOURCE) $(DEP_RSC_TIME_R) "$(INTDIR)"\
 "$(INTDIR)\Time.tlb"
   $(RSC) /l 0x409 /fo"$(INTDIR)/time.res" /i "UniRelease" /d "NDEBUG" /d\
 "_AFXDLL" $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\time.odl

!IF  "$(CFG)" == "Time - Win32 Release"


"$(OUTDIR)\Time.tlb" : $(SOURCE) "$(OUTDIR)"
   $(MTL) /nologo /D "NDEBUG" /tlb "$(OUTDIR)/time.tlb" /win32 $(SOURCE)


!ELSEIF  "$(CFG)" == "Time - Win32 Debug"


"$(OUTDIR)\Time.tlb" : $(SOURCE) "$(OUTDIR)"
   $(MTL) /nologo /D "_DEBUG" /tlb "$(OUTDIR)/time.tlb" /win32 $(SOURCE)


!ELSEIF  "$(CFG)" == "Time - Win32 Unicode Debug"


"$(OUTDIR)\Time.tlb" : $(SOURCE) "$(OUTDIR)"
   $(MTL) /nologo /D "_DEBUG" /tlb "$(OUTDIR)/time.tlb" /win32 $(SOURCE)


!ELSEIF  "$(CFG)" == "Time - Win32 Unicode Release"


"$(OUTDIR)\Time.tlb" : $(SOURCE) "$(OUTDIR)"
   $(MTL) /nologo /D "NDEBUG" /tlb "$(OUTDIR)/time.tlb" /win32 $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\timectl.cpp
DEP_CPP_TIMEC=\
	".\stdafx.h"\
	".\time.h"\
	".\timectl.h"\
	".\timeppg.h"\
	".\resource.h"\
	

!IF  "$(CFG)" == "Time - Win32 Release"


"$(INTDIR)\timectl.obj" : $(SOURCE) $(DEP_CPP_TIMEC) "$(INTDIR)"\
 "$(INTDIR)\time.pch"


!ELSEIF  "$(CFG)" == "Time - Win32 Debug"


"$(INTDIR)\timectl.obj" : $(SOURCE) $(DEP_CPP_TIMEC) "$(INTDIR)"\
 "$(INTDIR)\time.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_TIMEC) "$(INTDIR)"\
 "$(INTDIR)\time.pch"


!ELSEIF  "$(CFG)" == "Time - Win32 Unicode Debug"


"$(INTDIR)\timectl.obj" : $(SOURCE) $(DEP_CPP_TIMEC) "$(INTDIR)"\
 "$(INTDIR)\time.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_TIMEC) "$(INTDIR)"\
 "$(INTDIR)\time.pch"


!ELSEIF  "$(CFG)" == "Time - Win32 Unicode Release"


"$(INTDIR)\timectl.obj" : $(SOURCE) $(DEP_CPP_TIMEC) "$(INTDIR)"\
 "$(INTDIR)\time.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\timeppg.cpp
DEP_CPP_TIMEP=\
	".\stdafx.h"\
	".\time.h"\
	".\timeppg.h"\
	".\resource.h"\
	

!IF  "$(CFG)" == "Time - Win32 Release"


"$(INTDIR)\timeppg.obj" : $(SOURCE) $(DEP_CPP_TIMEP) "$(INTDIR)"\
 "$(INTDIR)\time.pch"


!ELSEIF  "$(CFG)" == "Time - Win32 Debug"


"$(INTDIR)\timeppg.obj" : $(SOURCE) $(DEP_CPP_TIMEP) "$(INTDIR)"\
 "$(INTDIR)\time.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_TIMEP) "$(INTDIR)"\
 "$(INTDIR)\time.pch"


!ELSEIF  "$(CFG)" == "Time - Win32 Unicode Debug"


"$(INTDIR)\timeppg.obj" : $(SOURCE) $(DEP_CPP_TIMEP) "$(INTDIR)"\
 "$(INTDIR)\time.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_TIMEP) "$(INTDIR)"\
 "$(INTDIR)\time.pch"


!ELSEIF  "$(CFG)" == "Time - Win32 Unicode Release"


"$(INTDIR)\timeppg.obj" : $(SOURCE) $(DEP_CPP_TIMEP) "$(INTDIR)"\
 "$(INTDIR)\time.pch"


!ENDIF 

# End Source File
# End Target
# End Project
################################################################################
