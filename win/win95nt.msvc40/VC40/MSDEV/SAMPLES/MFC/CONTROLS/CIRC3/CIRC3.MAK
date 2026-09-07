# Microsoft Visual C++ Generated NMAKE File, Format Version 40000
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

!IF "$(CFG)" == ""
CFG=Circ3 - Win32 Debug
!MESSAGE No configuration specified.  Defaulting to Circ3 - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "Circ3 - Win32 Release" && "$(CFG)" != "Circ3 - Win32 Debug" &&\
 "$(CFG)" != "Circ3 - Win32 Unicode Debug" && "$(CFG)" !=\
 "Circ3 - Win32 Unicode Release"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "circ3.mak" CFG="Circ3 - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Circ3 - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "Circ3 - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "Circ3 - Win32 Unicode Debug" (based on\
 "Win32 (x86) Dynamic-Link Library")
!MESSAGE "Circ3 - Win32 Unicode Release" (based on\
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
# PROP Target_Last_Scanned "Circ3 - Win32 Unicode Release"
CPP=cl.exe
RSC=rc.exe
MTL=mktyplib.exe

!IF  "$(CFG)" == "Circ3 - Win32 Release"

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

ALL : "$(OUTDIR)\circ3.ocx" "$(OUTDIR)\regsvr32.trg"

CLEAN : 
	-@erase ".\Release\circ3.lib"
	-@erase ".\Release\circ3ctl.obj"
	-@erase ".\Release\circ3.pch"
	-@erase ".\Release\circ3.obj"
	-@erase ".\Release\stdafx.obj"
	-@erase ".\Release\circ3ppg.obj"
	-@erase ".\Release\circ3.res"
	-@erase ".\Release\circ3.tlb"
	-@erase ".\Release\circ3.exp"
	-@erase ".\Release\regsvr32.trg"
	-@erase ".\Release\circ3.ocx"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
CPP_PROJ=/nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/circ3.pch" /Yu"stdafx.h" /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\Release/
CPP_SBRS=
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/circ3.res" /d "NDEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/circ3.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 /nologo /subsystem:windows /dll /machine:I386
LINK32_FLAGS=/nologo /subsystem:windows /dll /incremental:no\
 /pdb:"$(OUTDIR)/circ3.pdb" /machine:I386 /def:".\circ3.def"\
 /out:"$(OUTDIR)/circ3.ocx" /implib:"$(OUTDIR)/circ3.lib" 
DEF_FILE= \
	".\circ3.def"
LINK32_OBJS= \
	"$(INTDIR)/circ3ctl.obj" \
	"$(INTDIR)/circ3.obj" \
	"$(INTDIR)/stdafx.obj" \
	"$(INTDIR)/circ3ppg.obj" \
	"$(INTDIR)/circ3.res"

"$(OUTDIR)\circ3.ocx" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

# Begin Custom Build - Registering OLE control...
OutDir=.\Release
TargetPath=.\Release\circ3.ocx
InputPath=.\Release\circ3.ocx
SOURCE=$(InputPath)

"$(OutDir)\regsvr32.trg" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   regsvr32 /s /c "$(TargetPath)"
   echo regsvr32 exec. time > "$(OutDir)\regsvr32.trg"

# End Custom Build

!ELSEIF  "$(CFG)" == "Circ3 - Win32 Debug"

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

ALL : "$(OUTDIR)\circ3.ocx" "$(OUTDIR)\regsvr32.trg"

CLEAN : 
	-@erase ".\Debug\vc40.pdb"
	-@erase ".\Debug\circ3.pch"
	-@erase ".\Debug\circ3.ilk"
	-@erase ".\Debug\circ3ctl.obj"
	-@erase ".\Debug\circ3.obj"
	-@erase ".\Debug\stdafx.obj"
	-@erase ".\Debug\circ3ppg.obj"
	-@erase ".\Debug\circ3.res"
	-@erase ".\Debug\circ3.tlb"
	-@erase ".\Debug\circ3.lib"
	-@erase ".\Debug\circ3.exp"
	-@erase ".\Debug\circ3.pdb"
	-@erase ".\Debug\regsvr32.trg"
	-@erase ".\Debug\circ3.ocx"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
CPP_PROJ=/nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/circ3.pch" /Yu"stdafx.h" /Fo"$(INTDIR)/"\
 /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\Debug/
CPP_SBRS=
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/circ3.res" /d "_DEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/circ3.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /debug /machine:I386
# ADD LINK32 /nologo /subsystem:windows /dll /debug /machine:I386
LINK32_FLAGS=/nologo /subsystem:windows /dll /incremental:yes\
 /pdb:"$(OUTDIR)/circ3.pdb" /debug /machine:I386 /def:".\circ3.def"\
 /out:"$(OUTDIR)/circ3.ocx" /implib:"$(OUTDIR)/circ3.lib" 
DEF_FILE= \
	".\circ3.def"
LINK32_OBJS= \
	"$(INTDIR)/circ3ctl.obj" \
	"$(INTDIR)/circ3.obj" \
	"$(INTDIR)/stdafx.obj" \
	"$(INTDIR)/circ3ppg.obj" \
	"$(INTDIR)/circ3.res"

"$(OUTDIR)\circ3.ocx" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

# Begin Custom Build - Registering OLE control...
OutDir=.\Debug
TargetPath=.\Debug\circ3.ocx
InputPath=.\Debug\circ3.ocx
SOURCE=$(InputPath)

"$(OutDir)\regsvr32.trg" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   regsvr32 /s /c "$(TargetPath)"
   echo regsvr32 exec. time > "$(OutDir)\regsvr32.trg"

# End Custom Build

!ELSEIF  "$(CFG)" == "Circ3 - Win32 Unicode Debug"

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

ALL : "$(OUTDIR)\circ3.ocx" "$(OUTDIR)\regsvr32.trg"

CLEAN : 
	-@erase ".\UniDebug\vc40.pdb"
	-@erase ".\UniDebug\circ3.pch"
	-@erase ".\UniDebug\circ3.ilk"
	-@erase ".\UniDebug\circ3ctl.obj"
	-@erase ".\UniDebug\circ3.obj"
	-@erase ".\UniDebug\stdafx.obj"
	-@erase ".\UniDebug\circ3ppg.obj"
	-@erase ".\UniDebug\circ3.res"
	-@erase ".\UniDebug\circ3.tlb"
	-@erase ".\UniDebug\circ3.lib"
	-@erase ".\UniDebug\circ3.exp"
	-@erase ".\UniDebug\circ3.pdb"
	-@erase ".\UniDebug\regsvr32.trg"
	-@erase ".\UniDebug\circ3.ocx"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_UNICODE" /Yu"stdafx.h" /c
CPP_PROJ=/nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /D "_AFXDLL" /D "_UNICODE" /Fp"$(INTDIR)/circ3.pch" /Yu"stdafx.h"\
 /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\UniDebug/
CPP_SBRS=
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/circ3.res" /d "_DEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/circ3.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /debug /machine:I386
# ADD LINK32 /nologo /subsystem:windows /dll /debug /machine:I386
LINK32_FLAGS=/nologo /subsystem:windows /dll /incremental:yes\
 /pdb:"$(OUTDIR)/circ3.pdb" /debug /machine:I386 /def:".\circ3.def"\
 /out:"$(OUTDIR)/circ3.ocx" /implib:"$(OUTDIR)/circ3.lib" 
DEF_FILE= \
	".\circ3.def"
LINK32_OBJS= \
	"$(INTDIR)/circ3ctl.obj" \
	"$(INTDIR)/circ3.obj" \
	"$(INTDIR)/stdafx.obj" \
	"$(INTDIR)/circ3ppg.obj" \
	"$(INTDIR)/circ3.res"

"$(OUTDIR)\circ3.ocx" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

# Begin Custom Build - Registering OLE control...
OutDir=.\UniDebug
TargetPath=.\UniDebug\circ3.ocx
InputPath=.\UniDebug\circ3.ocx
SOURCE=$(InputPath)

"$(OutDir)\regsvr32.trg" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   regsvr32 /s /c "$(TargetPath)"
   echo regsvr32 exec. time > "$(OutDir)\regsvr32.trg"

# End Custom Build

!ELSEIF  "$(CFG)" == "Circ3 - Win32 Unicode Release"

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

ALL : "$(OUTDIR)\circ3.ocx" "$(OUTDIR)\regsvr32.trg"

CLEAN : 
	-@erase ".\UniRelease\circ3.lib"
	-@erase ".\UniRelease\circ3.obj"
	-@erase ".\UniRelease\circ3.pch"
	-@erase ".\UniRelease\circ3ppg.obj"
	-@erase ".\UniRelease\stdafx.obj"
	-@erase ".\UniRelease\circ3ctl.obj"
	-@erase ".\UniRelease\circ3.res"
	-@erase ".\UniRelease\circ3.tlb"
	-@erase ".\UniRelease\circ3.exp"
	-@erase ".\UniRelease\regsvr32.trg"
	-@erase ".\UniRelease\circ3.ocx"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_UNICODE" /Yu"stdafx.h" /c
CPP_PROJ=/nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_UNICODE" /Fp"$(INTDIR)/circ3.pch" /Yu"stdafx.h" /Fo"$(INTDIR)/"\
 /c 
CPP_OBJS=.\UniRelease/
CPP_SBRS=
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/circ3.res" /d "NDEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/circ3.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 /nologo /subsystem:windows /dll /machine:I386
LINK32_FLAGS=/nologo /subsystem:windows /dll /incremental:no\
 /pdb:"$(OUTDIR)/circ3.pdb" /machine:I386 /def:".\circ3.def"\
 /out:"$(OUTDIR)/circ3.ocx" /implib:"$(OUTDIR)/circ3.lib" 
DEF_FILE= \
	".\circ3.def"
LINK32_OBJS= \
	"$(INTDIR)/circ3.obj" \
	"$(INTDIR)/circ3ppg.obj" \
	"$(INTDIR)/stdafx.obj" \
	"$(INTDIR)/circ3ctl.obj" \
	"$(INTDIR)/circ3.res"

"$(OUTDIR)\circ3.ocx" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

# Begin Custom Build - Registering OLE control...
OutDir=.\UniRelease
TargetPath=.\UniRelease\circ3.ocx
InputPath=.\UniRelease\circ3.ocx
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

# Name "Circ3 - Win32 Release"
# Name "Circ3 - Win32 Debug"
# Name "Circ3 - Win32 Unicode Debug"
# Name "Circ3 - Win32 Unicode Release"

!IF  "$(CFG)" == "Circ3 - Win32 Release"

!ELSEIF  "$(CFG)" == "Circ3 - Win32 Debug"

!ELSEIF  "$(CFG)" == "Circ3 - Win32 Unicode Debug"

!ELSEIF  "$(CFG)" == "Circ3 - Win32 Unicode Release"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\readme.txt

!IF  "$(CFG)" == "Circ3 - Win32 Release"

!ELSEIF  "$(CFG)" == "Circ3 - Win32 Debug"

!ELSEIF  "$(CFG)" == "Circ3 - Win32 Unicode Debug"

!ELSEIF  "$(CFG)" == "Circ3 - Win32 Unicode Release"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\stdafx.cpp
DEP_CPP_STDAF=\
	".\stdafx.h"\
	

!IF  "$(CFG)" == "Circ3 - Win32 Release"

# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/circ3.pch" /Yc"stdafx.h" /Fo"$(INTDIR)/" /c\
 $(SOURCE) \
	

"$(INTDIR)\stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\circ3.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "Circ3 - Win32 Debug"

# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/circ3.pch" /Yc"stdafx.h" /Fo"$(INTDIR)/"\
 /Fd"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\circ3.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "Circ3 - Win32 Unicode Debug"

# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /D "_AFXDLL" /D "_UNICODE" /Fp"$(INTDIR)/circ3.pch" /Yc"stdafx.h"\
 /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\circ3.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "Circ3 - Win32 Unicode Release"

# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_UNICODE" /Fp"$(INTDIR)/circ3.pch" /Yc"stdafx.h" /Fo"$(INTDIR)/"\
 /c $(SOURCE) \
	

"$(INTDIR)\stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\circ3.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\circ3.cpp
DEP_CPP_CIRC3=\
	".\circ3.h"\
	".\stdafx.h"\
	

!IF  "$(CFG)" == "Circ3 - Win32 Release"


"$(INTDIR)\circ3.obj" : $(SOURCE) $(DEP_CPP_CIRC3) "$(INTDIR)"\
 "$(INTDIR)\circ3.pch"


!ELSEIF  "$(CFG)" == "Circ3 - Win32 Debug"


"$(INTDIR)\circ3.obj" : $(SOURCE) $(DEP_CPP_CIRC3) "$(INTDIR)"\
 "$(INTDIR)\circ3.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_CIRC3) "$(INTDIR)"\
 "$(INTDIR)\circ3.pch"


!ELSEIF  "$(CFG)" == "Circ3 - Win32 Unicode Debug"


"$(INTDIR)\circ3.obj" : $(SOURCE) $(DEP_CPP_CIRC3) "$(INTDIR)"\
 "$(INTDIR)\circ3.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_CIRC3) "$(INTDIR)"\
 "$(INTDIR)\circ3.pch"


!ELSEIF  "$(CFG)" == "Circ3 - Win32 Unicode Release"


"$(INTDIR)\circ3.obj" : $(SOURCE) $(DEP_CPP_CIRC3) "$(INTDIR)"\
 "$(INTDIR)\circ3.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\circ3.def

!IF  "$(CFG)" == "Circ3 - Win32 Release"

!ELSEIF  "$(CFG)" == "Circ3 - Win32 Debug"

!ELSEIF  "$(CFG)" == "Circ3 - Win32 Unicode Debug"

!ELSEIF  "$(CFG)" == "Circ3 - Win32 Unicode Release"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\circ3.rc

!IF  "$(CFG)" == "Circ3 - Win32 Release"

DEP_RSC_CIRC3_=\
	".\circ3.ico"\
	".\circ3ctl.bmp"\
	".\resource.h"\
	".\circ3.rc2"\
	".\Release\circ3.tlb"\
	

"$(INTDIR)\circ3.res" : $(SOURCE) $(DEP_RSC_CIRC3_) "$(INTDIR)"\
 "$(INTDIR)\circ3.tlb"
   $(RSC) /l 0x409 /fo"$(INTDIR)/circ3.res" /i "Release" /d "NDEBUG" /d\
 "_AFXDLL" $(SOURCE)


!ELSEIF  "$(CFG)" == "Circ3 - Win32 Debug"

DEP_RSC_CIRC3_=\
	".\Debug\circ3.tlb"\
	".\circ3.ico"\
	".\circ3ctl.bmp"\
	".\resource.h"\
	".\circ3.rc2"\
	

"$(INTDIR)\circ3.res" : $(SOURCE) $(DEP_RSC_CIRC3_) "$(INTDIR)"\
 "$(INTDIR)\circ3.tlb"
   $(RSC) /l 0x409 /fo"$(INTDIR)/circ3.res" /i "Debug" /d "_DEBUG" /d "_AFXDLL"\
 $(SOURCE)


!ELSEIF  "$(CFG)" == "Circ3 - Win32 Unicode Debug"

DEP_RSC_CIRC3_=\
	".\UniDebug\circ3.tlb"\
	".\circ3.ico"\
	".\circ3ctl.bmp"\
	".\resource.h"\
	".\circ3.rc2"\
	

"$(INTDIR)\circ3.res" : $(SOURCE) $(DEP_RSC_CIRC3_) "$(INTDIR)"\
 "$(INTDIR)\circ3.tlb"
   $(RSC) /l 0x409 /fo"$(INTDIR)/circ3.res" /i "UniDebug" /d "_DEBUG" /d\
 "_AFXDLL" $(SOURCE)


!ELSEIF  "$(CFG)" == "Circ3 - Win32 Unicode Release"

DEP_RSC_CIRC3_=\
	".\circ3.ico"\
	".\circ3ctl.bmp"\
	".\resource.h"\
	".\circ3.rc2"\
	".\UniRelease\circ3.tlb"\
	

"$(INTDIR)\circ3.res" : $(SOURCE) $(DEP_RSC_CIRC3_) "$(INTDIR)"\
 "$(INTDIR)\circ3.tlb"
   $(RSC) /l 0x409 /fo"$(INTDIR)/circ3.res" /i "UniRelease" /d "NDEBUG" /d\
 "_AFXDLL" $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\circ3.odl

!IF  "$(CFG)" == "Circ3 - Win32 Release"


"$(OUTDIR)\circ3.tlb" : $(SOURCE) "$(OUTDIR)"
   $(MTL) /nologo /D "NDEBUG" /tlb "$(OUTDIR)/circ3.tlb" /win32 $(SOURCE)


!ELSEIF  "$(CFG)" == "Circ3 - Win32 Debug"


"$(OUTDIR)\circ3.tlb" : $(SOURCE) "$(OUTDIR)"
   $(MTL) /nologo /D "_DEBUG" /tlb "$(OUTDIR)/circ3.tlb" /win32 $(SOURCE)


!ELSEIF  "$(CFG)" == "Circ3 - Win32 Unicode Debug"


"$(OUTDIR)\circ3.tlb" : $(SOURCE) "$(OUTDIR)"
   $(MTL) /nologo /D "_DEBUG" /tlb "$(OUTDIR)/circ3.tlb" /win32 $(SOURCE)


!ELSEIF  "$(CFG)" == "Circ3 - Win32 Unicode Release"


"$(OUTDIR)\circ3.tlb" : $(SOURCE) "$(OUTDIR)"
   $(MTL) /nologo /D "NDEBUG" /tlb "$(OUTDIR)/circ3.tlb" /win32 $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\circ3ctl.cpp
DEP_CPP_CIRC3C=\
	".\circ3.h"\
	".\circ3ctl.h"\
	".\circ3ppg.h"\
	".\stdafx.h"\
	

!IF  "$(CFG)" == "Circ3 - Win32 Release"


"$(INTDIR)\circ3ctl.obj" : $(SOURCE) $(DEP_CPP_CIRC3C) "$(INTDIR)"\
 "$(INTDIR)\circ3.pch"


!ELSEIF  "$(CFG)" == "Circ3 - Win32 Debug"


"$(INTDIR)\circ3ctl.obj" : $(SOURCE) $(DEP_CPP_CIRC3C) "$(INTDIR)"\
 "$(INTDIR)\circ3.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_CIRC3C) "$(INTDIR)"\
 "$(INTDIR)\circ3.pch"


!ELSEIF  "$(CFG)" == "Circ3 - Win32 Unicode Debug"


"$(INTDIR)\circ3ctl.obj" : $(SOURCE) $(DEP_CPP_CIRC3C) "$(INTDIR)"\
 "$(INTDIR)\circ3.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_CIRC3C) "$(INTDIR)"\
 "$(INTDIR)\circ3.pch"


!ELSEIF  "$(CFG)" == "Circ3 - Win32 Unicode Release"


"$(INTDIR)\circ3ctl.obj" : $(SOURCE) $(DEP_CPP_CIRC3C) "$(INTDIR)"\
 "$(INTDIR)\circ3.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\circ3ppg.cpp
DEP_CPP_CIRC3P=\
	".\circ3.h"\
	".\circ3ppg.h"\
	".\stdafx.h"\
	

!IF  "$(CFG)" == "Circ3 - Win32 Release"


"$(INTDIR)\circ3ppg.obj" : $(SOURCE) $(DEP_CPP_CIRC3P) "$(INTDIR)"\
 "$(INTDIR)\circ3.pch"


!ELSEIF  "$(CFG)" == "Circ3 - Win32 Debug"


"$(INTDIR)\circ3ppg.obj" : $(SOURCE) $(DEP_CPP_CIRC3P) "$(INTDIR)"\
 "$(INTDIR)\circ3.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_CIRC3P) "$(INTDIR)"\
 "$(INTDIR)\circ3.pch"


!ELSEIF  "$(CFG)" == "Circ3 - Win32 Unicode Debug"


"$(INTDIR)\circ3ppg.obj" : $(SOURCE) $(DEP_CPP_CIRC3P) "$(INTDIR)"\
 "$(INTDIR)\circ3.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_CIRC3P) "$(INTDIR)"\
 "$(INTDIR)\circ3.pch"


!ELSEIF  "$(CFG)" == "Circ3 - Win32 Unicode Release"


"$(INTDIR)\circ3ppg.obj" : $(SOURCE) $(DEP_CPP_CIRC3P) "$(INTDIR)"\
 "$(INTDIR)\circ3.pch"


!ENDIF 

# End Source File
# End Target
# End Project
################################################################################
