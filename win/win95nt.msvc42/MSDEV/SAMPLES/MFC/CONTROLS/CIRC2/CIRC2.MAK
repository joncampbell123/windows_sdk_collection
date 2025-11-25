# Microsoft Visual C++ Generated NMAKE File, Format Version 40000
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

!IF "$(CFG)" == ""
CFG=Circ2 - Win32 Debug
!MESSAGE No configuration specified.  Defaulting to Circ2 - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "Circ2 - Win32 Release" && "$(CFG)" != "Circ2 - Win32 Debug" &&\
 "$(CFG)" != "Circ2 - Win32 Unicode Debug" && "$(CFG)" !=\
 "Circ2 - Win32 Unicode Release"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "circ2.mak" CFG="Circ2 - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Circ2 - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "Circ2 - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "Circ2 - Win32 Unicode Debug" (based on\
 "Win32 (x86) Dynamic-Link Library")
!MESSAGE "Circ2 - Win32 Unicode Release" (based on\
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
# PROP Target_Last_Scanned "Circ2 - Win32 Unicode Release"
MTL=mktyplib.exe
RSC=rc.exe
CPP=cl.exe

!IF  "$(CFG)" == "Circ2 - Win32 Release"

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

ALL : "$(OUTDIR)\circ2.ocx" "$(OUTDIR)\regsvr32.trg"

CLEAN : 
	-@erase ".\Release\circ2.lib"
	-@erase ".\Release\circ2.obj"
	-@erase ".\Release\circ2.pch"
	-@erase ".\Release\circ2ppg.obj"
	-@erase ".\Release\stdafx.obj"
	-@erase ".\Release\circ2ctl.obj"
	-@erase ".\Release\circ2.res"
	-@erase ".\Release\circ2.tlb"
	-@erase ".\Release\circ2.exp"
	-@erase ".\Release\regsvr32.trg"
	-@erase ".\Release\circ2.ocx"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
CPP_PROJ=/nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/circ2.pch" /Yu"stdafx.h" /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\Release/
CPP_SBRS=
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/circ2.res" /d "NDEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/circ2.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 /nologo /subsystem:windows /dll /machine:I386
LINK32_FLAGS=/nologo /subsystem:windows /dll /incremental:no\
 /pdb:"$(OUTDIR)/circ2.pdb" /machine:I386 /def:".\circ2.def"\
 /out:"$(OUTDIR)/circ2.ocx" /implib:"$(OUTDIR)/circ2.lib" 
DEF_FILE= \
	".\circ2.def"
LINK32_OBJS= \
	"$(INTDIR)/circ2.obj" \
	"$(INTDIR)/circ2ppg.obj" \
	"$(INTDIR)/stdafx.obj" \
	"$(INTDIR)/circ2ctl.obj" \
	"$(INTDIR)/circ2.res"

"$(OUTDIR)\circ2.ocx" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

# Begin Custom Build - Registering OLE control...
OutDir=.\Release
TargetPath=.\Release\circ2.ocx
InputPath=.\Release\circ2.ocx
SOURCE=$(InputPath)

"$(OutDir)\regsvr32.trg" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   regsvr32 /s /c "$(TargetPath)"
   echo regsvr32 exec. time > "$(OutDir)\regsvr32.trg"

# End Custom Build

!ELSEIF  "$(CFG)" == "Circ2 - Win32 Debug"

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

ALL : "$(OUTDIR)\circ2.ocx" "$(OUTDIR)\regsvr32.trg"

CLEAN : 
	-@erase ".\Debug\vc40.pdb"
	-@erase ".\Debug\circ2.pch"
	-@erase ".\Debug\circ2.ilk"
	-@erase ".\Debug\circ2ppg.obj"
	-@erase ".\Debug\circ2ctl.obj"
	-@erase ".\Debug\circ2.obj"
	-@erase ".\Debug\stdafx.obj"
	-@erase ".\Debug\circ2.res"
	-@erase ".\Debug\circ2.tlb"
	-@erase ".\Debug\circ2.lib"
	-@erase ".\Debug\circ2.exp"
	-@erase ".\Debug\circ2.pdb"
	-@erase ".\Debug\regsvr32.trg"
	-@erase ".\Debug\circ2.ocx"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
CPP_PROJ=/nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/circ2.pch" /Yu"stdafx.h" /Fo"$(INTDIR)/"\
 /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\Debug/
CPP_SBRS=
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/circ2.res" /d "_DEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/circ2.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /debug /machine:I386
# ADD LINK32 /nologo /subsystem:windows /dll /debug /machine:I386
LINK32_FLAGS=/nologo /subsystem:windows /dll /incremental:yes\
 /pdb:"$(OUTDIR)/circ2.pdb" /debug /machine:I386 /def:".\circ2.def"\
 /out:"$(OUTDIR)/circ2.ocx" /implib:"$(OUTDIR)/circ2.lib" 
DEF_FILE= \
	".\circ2.def"
LINK32_OBJS= \
	"$(INTDIR)/circ2ppg.obj" \
	"$(INTDIR)/circ2ctl.obj" \
	"$(INTDIR)/circ2.obj" \
	"$(INTDIR)/stdafx.obj" \
	"$(INTDIR)/circ2.res"

"$(OUTDIR)\circ2.ocx" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

# Begin Custom Build - Registering OLE control...
OutDir=.\Debug
TargetPath=.\Debug\circ2.ocx
InputPath=.\Debug\circ2.ocx
SOURCE=$(InputPath)

"$(OutDir)\regsvr32.trg" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   regsvr32 /s /c "$(TargetPath)"
   echo regsvr32 exec. time > "$(OutDir)\regsvr32.trg"

# End Custom Build

!ELSEIF  "$(CFG)" == "Circ2 - Win32 Unicode Debug"

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

ALL : "$(OUTDIR)\circ2.ocx" "$(OUTDIR)\regsvr32.trg"

CLEAN : 
	-@erase ".\UniDebug\vc40.pdb"
	-@erase ".\UniDebug\circ2.pch"
	-@erase ".\UniDebug\circ2.ilk"
	-@erase ".\UniDebug\circ2.obj"
	-@erase ".\UniDebug\circ2ppg.obj"
	-@erase ".\UniDebug\stdafx.obj"
	-@erase ".\UniDebug\circ2ctl.obj"
	-@erase ".\UniDebug\circ2.res"
	-@erase ".\UniDebug\circ2.tlb"
	-@erase ".\UniDebug\circ2.lib"
	-@erase ".\UniDebug\circ2.exp"
	-@erase ".\UniDebug\circ2.pdb"
	-@erase ".\UniDebug\regsvr32.trg"
	-@erase ".\UniDebug\circ2.ocx"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_UNICODE" /Yu"stdafx.h" /c
CPP_PROJ=/nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /D "_AFXDLL" /D "_UNICODE" /Fp"$(INTDIR)/circ2.pch" /Yu"stdafx.h"\
 /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\UniDebug/
CPP_SBRS=
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/circ2.res" /d "_DEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/circ2.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /debug /machine:I386
# ADD LINK32 /nologo /subsystem:windows /dll /debug /machine:I386
LINK32_FLAGS=/nologo /subsystem:windows /dll /incremental:yes\
 /pdb:"$(OUTDIR)/circ2.pdb" /debug /machine:I386 /def:".\circ2.def"\
 /out:"$(OUTDIR)/circ2.ocx" /implib:"$(OUTDIR)/circ2.lib" 
DEF_FILE= \
	".\circ2.def"
LINK32_OBJS= \
	"$(INTDIR)/circ2.obj" \
	"$(INTDIR)/circ2ppg.obj" \
	"$(INTDIR)/stdafx.obj" \
	"$(INTDIR)/circ2ctl.obj" \
	"$(INTDIR)/circ2.res"

"$(OUTDIR)\circ2.ocx" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

# Begin Custom Build - Registering OLE control...
OutDir=.\UniDebug
TargetPath=.\UniDebug\circ2.ocx
InputPath=.\UniDebug\circ2.ocx
SOURCE=$(InputPath)

"$(OutDir)\regsvr32.trg" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   regsvr32 /s /c "$(TargetPath)"
   echo regsvr32 exec. time > "$(OutDir)\regsvr32.trg"

# End Custom Build

!ELSEIF  "$(CFG)" == "Circ2 - Win32 Unicode Release"

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

ALL : "$(OUTDIR)\circ2.ocx" "$(OUTDIR)\regsvr32.trg"

CLEAN : 
	-@erase ".\UniRelease\circ2.lib"
	-@erase ".\UniRelease\circ2.obj"
	-@erase ".\UniRelease\circ2.pch"
	-@erase ".\UniRelease\circ2ppg.obj"
	-@erase ".\UniRelease\stdafx.obj"
	-@erase ".\UniRelease\circ2ctl.obj"
	-@erase ".\UniRelease\circ2.res"
	-@erase ".\UniRelease\circ2.tlb"
	-@erase ".\UniRelease\circ2.exp"
	-@erase ".\UniRelease\regsvr32.trg"
	-@erase ".\UniRelease\circ2.ocx"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_UNICODE" /Yu"stdafx.h" /c
CPP_PROJ=/nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_UNICODE" /Fp"$(INTDIR)/circ2.pch" /Yu"stdafx.h" /Fo"$(INTDIR)/"\
 /c 
CPP_OBJS=.\UniRelease/
CPP_SBRS=
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/circ2.res" /d "NDEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/circ2.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 /nologo /subsystem:windows /dll /machine:I386
LINK32_FLAGS=/nologo /subsystem:windows /dll /incremental:no\
 /pdb:"$(OUTDIR)/circ2.pdb" /machine:I386 /def:".\circ2.def"\
 /out:"$(OUTDIR)/circ2.ocx" /implib:"$(OUTDIR)/circ2.lib" 
DEF_FILE= \
	".\circ2.def"
LINK32_OBJS= \
	"$(INTDIR)/circ2.obj" \
	"$(INTDIR)/circ2ppg.obj" \
	"$(INTDIR)/stdafx.obj" \
	"$(INTDIR)/circ2ctl.obj" \
	"$(INTDIR)/circ2.res"

"$(OUTDIR)\circ2.ocx" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

# Begin Custom Build - Registering OLE control...
OutDir=.\UniRelease
TargetPath=.\UniRelease\circ2.ocx
InputPath=.\UniRelease\circ2.ocx
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

# Name "Circ2 - Win32 Release"
# Name "Circ2 - Win32 Debug"
# Name "Circ2 - Win32 Unicode Debug"
# Name "Circ2 - Win32 Unicode Release"

!IF  "$(CFG)" == "Circ2 - Win32 Release"

!ELSEIF  "$(CFG)" == "Circ2 - Win32 Debug"

!ELSEIF  "$(CFG)" == "Circ2 - Win32 Unicode Debug"

!ELSEIF  "$(CFG)" == "Circ2 - Win32 Unicode Release"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\readme.txt

!IF  "$(CFG)" == "Circ2 - Win32 Release"

!ELSEIF  "$(CFG)" == "Circ2 - Win32 Debug"

!ELSEIF  "$(CFG)" == "Circ2 - Win32 Unicode Debug"

!ELSEIF  "$(CFG)" == "Circ2 - Win32 Unicode Release"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\stdafx.cpp
DEP_CPP_STDAF=\
	".\stdafx.h"\
	

!IF  "$(CFG)" == "Circ2 - Win32 Release"

# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/circ2.pch" /Yc"stdafx.h" /Fo"$(INTDIR)/" /c\
 $(SOURCE) \
	

"$(INTDIR)\stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\circ2.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "Circ2 - Win32 Debug"

# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/circ2.pch" /Yc"stdafx.h" /Fo"$(INTDIR)/"\
 /Fd"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\circ2.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "Circ2 - Win32 Unicode Debug"

# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /D "_AFXDLL" /D "_UNICODE" /Fp"$(INTDIR)/circ2.pch" /Yc"stdafx.h"\
 /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\circ2.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "Circ2 - Win32 Unicode Release"

# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_UNICODE" /Fp"$(INTDIR)/circ2.pch" /Yc"stdafx.h" /Fo"$(INTDIR)/"\
 /c $(SOURCE) \
	

"$(INTDIR)\stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\circ2.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\circ2.cpp
DEP_CPP_CIRC2=\
	".\circ2.h"\
	".\stdafx.h"\
	

!IF  "$(CFG)" == "Circ2 - Win32 Release"


"$(INTDIR)\circ2.obj" : $(SOURCE) $(DEP_CPP_CIRC2) "$(INTDIR)"\
 "$(INTDIR)\circ2.pch"


!ELSEIF  "$(CFG)" == "Circ2 - Win32 Debug"


"$(INTDIR)\circ2.obj" : $(SOURCE) $(DEP_CPP_CIRC2) "$(INTDIR)"\
 "$(INTDIR)\circ2.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_CIRC2) "$(INTDIR)"\
 "$(INTDIR)\circ2.pch"


!ELSEIF  "$(CFG)" == "Circ2 - Win32 Unicode Debug"


"$(INTDIR)\circ2.obj" : $(SOURCE) $(DEP_CPP_CIRC2) "$(INTDIR)"\
 "$(INTDIR)\circ2.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_CIRC2) "$(INTDIR)"\
 "$(INTDIR)\circ2.pch"


!ELSEIF  "$(CFG)" == "Circ2 - Win32 Unicode Release"


"$(INTDIR)\circ2.obj" : $(SOURCE) $(DEP_CPP_CIRC2) "$(INTDIR)"\
 "$(INTDIR)\circ2.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\circ2.def

!IF  "$(CFG)" == "Circ2 - Win32 Release"

!ELSEIF  "$(CFG)" == "Circ2 - Win32 Debug"

!ELSEIF  "$(CFG)" == "Circ2 - Win32 Unicode Debug"

!ELSEIF  "$(CFG)" == "Circ2 - Win32 Unicode Release"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\circ2.rc

!IF  "$(CFG)" == "Circ2 - Win32 Release"

DEP_RSC_CIRC2_=\
	".\Release\circ2.tlb"\
	".\circ2.ico"\
	".\circ2ctl.bmp"\
	".\resource.h"\
	".\circ2.rc2"\
	

"$(INTDIR)\circ2.res" : $(SOURCE) $(DEP_RSC_CIRC2_) "$(INTDIR)"\
 "$(INTDIR)\circ2.tlb"
   $(RSC) /l 0x409 /fo"$(INTDIR)/circ2.res" /i "Release" /d "NDEBUG" /d\
 "_AFXDLL" $(SOURCE)


!ELSEIF  "$(CFG)" == "Circ2 - Win32 Debug"

DEP_RSC_CIRC2_=\
	".\circ2.ico"\
	".\circ2ctl.bmp"\
	".\resource.h"\
	".\circ2.rc2"\
	".\Debug\circ2.tlb"\
	

"$(INTDIR)\circ2.res" : $(SOURCE) $(DEP_RSC_CIRC2_) "$(INTDIR)"\
 "$(INTDIR)\circ2.tlb"
   $(RSC) /l 0x409 /fo"$(INTDIR)/circ2.res" /i "Debug" /d "_DEBUG" /d "_AFXDLL"\
 $(SOURCE)


!ELSEIF  "$(CFG)" == "Circ2 - Win32 Unicode Debug"

DEP_RSC_CIRC2_=\
	".\circ2.ico"\
	".\circ2ctl.bmp"\
	".\resource.h"\
	".\circ2.rc2"\
	".\UniDebug\circ2.tlb"\
	

"$(INTDIR)\circ2.res" : $(SOURCE) $(DEP_RSC_CIRC2_) "$(INTDIR)"\
 "$(INTDIR)\circ2.tlb"
   $(RSC) /l 0x409 /fo"$(INTDIR)/circ2.res" /i "UniDebug" /d "_DEBUG" /d\
 "_AFXDLL" $(SOURCE)


!ELSEIF  "$(CFG)" == "Circ2 - Win32 Unicode Release"

DEP_RSC_CIRC2_=\
	".\circ2.ico"\
	".\circ2ctl.bmp"\
	".\resource.h"\
	".\circ2.rc2"\
	".\UniRelease\circ2.tlb"\
	

"$(INTDIR)\circ2.res" : $(SOURCE) $(DEP_RSC_CIRC2_) "$(INTDIR)"\
 "$(INTDIR)\circ2.tlb"
   $(RSC) /l 0x409 /fo"$(INTDIR)/circ2.res" /i "UniRelease" /d "NDEBUG" /d\
 "_AFXDLL" $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\circ2.odl

!IF  "$(CFG)" == "Circ2 - Win32 Release"


"$(OUTDIR)\circ2.tlb" : $(SOURCE) "$(OUTDIR)"
   $(MTL) /nologo /D "NDEBUG" /tlb "$(OUTDIR)/circ2.tlb" /win32 $(SOURCE)


!ELSEIF  "$(CFG)" == "Circ2 - Win32 Debug"


"$(OUTDIR)\circ2.tlb" : $(SOURCE) "$(OUTDIR)"
   $(MTL) /nologo /D "_DEBUG" /tlb "$(OUTDIR)/circ2.tlb" /win32 $(SOURCE)


!ELSEIF  "$(CFG)" == "Circ2 - Win32 Unicode Debug"


"$(OUTDIR)\circ2.tlb" : $(SOURCE) "$(OUTDIR)"
   $(MTL) /nologo /D "_DEBUG" /tlb "$(OUTDIR)/circ2.tlb" /win32 $(SOURCE)


!ELSEIF  "$(CFG)" == "Circ2 - Win32 Unicode Release"


"$(OUTDIR)\circ2.tlb" : $(SOURCE) "$(OUTDIR)"
   $(MTL) /nologo /D "NDEBUG" /tlb "$(OUTDIR)/circ2.tlb" /win32 $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\circ2ctl.cpp
DEP_CPP_CIRC2C=\
	".\circ2.h"\
	".\circ2ctl.h"\
	".\circ2ppg.h"\
	".\stdafx.h"\
	

!IF  "$(CFG)" == "Circ2 - Win32 Release"


"$(INTDIR)\circ2ctl.obj" : $(SOURCE) $(DEP_CPP_CIRC2C) "$(INTDIR)"\
 "$(INTDIR)\circ2.pch"


!ELSEIF  "$(CFG)" == "Circ2 - Win32 Debug"


"$(INTDIR)\circ2ctl.obj" : $(SOURCE) $(DEP_CPP_CIRC2C) "$(INTDIR)"\
 "$(INTDIR)\circ2.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_CIRC2C) "$(INTDIR)"\
 "$(INTDIR)\circ2.pch"


!ELSEIF  "$(CFG)" == "Circ2 - Win32 Unicode Debug"


"$(INTDIR)\circ2ctl.obj" : $(SOURCE) $(DEP_CPP_CIRC2C) "$(INTDIR)"\
 "$(INTDIR)\circ2.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_CIRC2C) "$(INTDIR)"\
 "$(INTDIR)\circ2.pch"


!ELSEIF  "$(CFG)" == "Circ2 - Win32 Unicode Release"


"$(INTDIR)\circ2ctl.obj" : $(SOURCE) $(DEP_CPP_CIRC2C) "$(INTDIR)"\
 "$(INTDIR)\circ2.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\circ2ppg.cpp
DEP_CPP_CIRC2P=\
	".\circ2.h"\
	".\circ2ppg.h"\
	".\stdafx.h"\
	

!IF  "$(CFG)" == "Circ2 - Win32 Release"


"$(INTDIR)\circ2ppg.obj" : $(SOURCE) $(DEP_CPP_CIRC2P) "$(INTDIR)"\
 "$(INTDIR)\circ2.pch"


!ELSEIF  "$(CFG)" == "Circ2 - Win32 Debug"


"$(INTDIR)\circ2ppg.obj" : $(SOURCE) $(DEP_CPP_CIRC2P) "$(INTDIR)"\
 "$(INTDIR)\circ2.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_CIRC2P) "$(INTDIR)"\
 "$(INTDIR)\circ2.pch"


!ELSEIF  "$(CFG)" == "Circ2 - Win32 Unicode Debug"


"$(INTDIR)\circ2ppg.obj" : $(SOURCE) $(DEP_CPP_CIRC2P) "$(INTDIR)"\
 "$(INTDIR)\circ2.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_CIRC2P) "$(INTDIR)"\
 "$(INTDIR)\circ2.pch"


!ELSEIF  "$(CFG)" == "Circ2 - Win32 Unicode Release"


"$(INTDIR)\circ2ppg.obj" : $(SOURCE) $(DEP_CPP_CIRC2P) "$(INTDIR)"\
 "$(INTDIR)\circ2.pch"


!ENDIF 

# End Source File
# End Target
# End Project
################################################################################
