# Microsoft Visual C++ Generated NMAKE File, Format Version 40000
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

!IF "$(CFG)" == ""
CFG=Circ1 - Win32 Debug
!MESSAGE No configuration specified.  Defaulting to Circ1 - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "Circ1 - Win32 Release" && "$(CFG)" != "Circ1 - Win32 Debug" &&\
 "$(CFG)" != "Circ1 - Win32 Unicode Debug" && "$(CFG)" !=\
 "Circ1 - Win32 Unicode Release"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "circ1.mak" CFG="Circ1 - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Circ1 - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "Circ1 - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "Circ1 - Win32 Unicode Debug" (based on\
 "Win32 (x86) Dynamic-Link Library")
!MESSAGE "Circ1 - Win32 Unicode Release" (based on\
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
# PROP Target_Last_Scanned "Circ1 - Win32 Unicode Release"
CPP=cl.exe
RSC=rc.exe
MTL=mktyplib.exe

!IF  "$(CFG)" == "Circ1 - Win32 Release"

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

ALL : "$(OUTDIR)\circ1.ocx" "$(OUTDIR)\regsvr32.trg"

CLEAN : 
	-@erase ".\Release\circ1.lib"
	-@erase ".\Release\circ1.obj"
	-@erase ".\Release\circ1.pch"
	-@erase ".\Release\circ1ppg.obj"
	-@erase ".\Release\circ1ctl.obj"
	-@erase ".\Release\stdafx.obj"
	-@erase ".\Release\circ1.res"
	-@erase ".\Release\circ1.tlb"
	-@erase ".\Release\circ1.exp"
	-@erase ".\Release\regsvr32.trg"
	-@erase ".\Release\circ1.ocx"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
# SUBTRACT CPP /WX /Fr
CPP_PROJ=/nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/circ1.pch" /Yu"stdafx.h" /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\Release/
CPP_SBRS=
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/circ1.res" /d "NDEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/circ1.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 /nologo /subsystem:windows /dll /machine:I386
LINK32_FLAGS=/nologo /subsystem:windows /dll /incremental:no\
 /pdb:"$(OUTDIR)/circ1.pdb" /machine:I386 /def:".\circ1.def"\
 /out:"$(OUTDIR)/circ1.ocx" /implib:"$(OUTDIR)/circ1.lib" 
DEF_FILE= \
	".\circ1.def"
LINK32_OBJS= \
	"$(INTDIR)/circ1.obj" \
	"$(INTDIR)/circ1ppg.obj" \
	"$(INTDIR)/circ1ctl.obj" \
	"$(INTDIR)/stdafx.obj" \
	"$(INTDIR)/circ1.res"

"$(OUTDIR)\circ1.ocx" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

# Begin Custom Build - Registering OLE control...
OutDir=.\Release
TargetPath=.\Release\circ1.ocx
InputPath=.\Release\circ1.ocx
SOURCE=$(InputPath)

"$(OutDir)\regsvr32.trg" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   regsvr32 /s /c "$(TargetPath)"
   echo regsvr32 exec. time > "$(OutDir)\regsvr32.trg"

# End Custom Build

!ELSEIF  "$(CFG)" == "Circ1 - Win32 Debug"

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

ALL : "$(OUTDIR)\circ1.ocx" "$(OUTDIR)\regsvr32.trg"

CLEAN : 
	-@erase ".\Debug\vc40.pdb"
	-@erase ".\Debug\circ1.pch"
	-@erase ".\Debug\circ1.ilk"
	-@erase ".\Debug\circ1.obj"
	-@erase ".\Debug\circ1ppg.obj"
	-@erase ".\Debug\stdafx.obj"
	-@erase ".\Debug\circ1ctl.obj"
	-@erase ".\Debug\circ1.res"
	-@erase ".\Debug\circ1.tlb"
	-@erase ".\Debug\circ1.lib"
	-@erase ".\Debug\circ1.exp"
	-@erase ".\Debug\circ1.pdb"
	-@erase ".\Debug\regsvr32.trg"
	-@erase ".\Debug\circ1.ocx"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
CPP_PROJ=/nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/circ1.pch" /Yu"stdafx.h" /Fo"$(INTDIR)/"\
 /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\Debug/
CPP_SBRS=
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/circ1.res" /d "_DEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/circ1.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /debug /machine:I386
# ADD LINK32 /nologo /subsystem:windows /dll /debug /machine:I386
LINK32_FLAGS=/nologo /subsystem:windows /dll /incremental:yes\
 /pdb:"$(OUTDIR)/circ1.pdb" /debug /machine:I386 /def:".\circ1.def"\
 /out:"$(OUTDIR)/circ1.ocx" /implib:"$(OUTDIR)/circ1.lib" 
DEF_FILE= \
	".\circ1.def"
LINK32_OBJS= \
	"$(INTDIR)/circ1.obj" \
	"$(INTDIR)/circ1ppg.obj" \
	"$(INTDIR)/stdafx.obj" \
	"$(INTDIR)/circ1ctl.obj" \
	"$(INTDIR)/circ1.res"

"$(OUTDIR)\circ1.ocx" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

# Begin Custom Build - Registering OLE control...
OutDir=.\Debug
TargetPath=.\Debug\circ1.ocx
InputPath=.\Debug\circ1.ocx
SOURCE=$(InputPath)

"$(OutDir)\regsvr32.trg" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   regsvr32 /s /c "$(TargetPath)"
   echo regsvr32 exec. time > "$(OutDir)\regsvr32.trg"

# End Custom Build

!ELSEIF  "$(CFG)" == "Circ1 - Win32 Unicode Debug"

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

ALL : "$(OUTDIR)\circ1.ocx" "$(OUTDIR)\regsvr32.trg"

CLEAN : 
	-@erase ".\UniDebug\vc40.pdb"
	-@erase ".\UniDebug\circ1.pch"
	-@erase ".\UniDebug\circ1.ilk"
	-@erase ".\UniDebug\circ1ppg.obj"
	-@erase ".\UniDebug\stdafx.obj"
	-@erase ".\UniDebug\circ1.obj"
	-@erase ".\UniDebug\circ1ctl.obj"
	-@erase ".\UniDebug\circ1.res"
	-@erase ".\UniDebug\circ1.tlb"
	-@erase ".\UniDebug\circ1.lib"
	-@erase ".\UniDebug\circ1.exp"
	-@erase ".\UniDebug\circ1.pdb"
	-@erase ".\UniDebug\regsvr32.trg"
	-@erase ".\UniDebug\circ1.ocx"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_UNICODE" /Yu"stdafx.h" /c
CPP_PROJ=/nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /D "_AFXDLL" /D "_UNICODE" /Fp"$(INTDIR)/circ1.pch" /Yu"stdafx.h"\
 /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\UniDebug/
CPP_SBRS=
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/circ1.res" /d "_DEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/circ1.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /debug /machine:I386
# ADD LINK32 /nologo /subsystem:windows /dll /debug /machine:I386
LINK32_FLAGS=/nologo /subsystem:windows /dll /incremental:yes\
 /pdb:"$(OUTDIR)/circ1.pdb" /debug /machine:I386 /def:".\circ1.def"\
 /out:"$(OUTDIR)/circ1.ocx" /implib:"$(OUTDIR)/circ1.lib" 
DEF_FILE= \
	".\circ1.def"
LINK32_OBJS= \
	"$(INTDIR)/circ1ppg.obj" \
	"$(INTDIR)/stdafx.obj" \
	"$(INTDIR)/circ1.obj" \
	"$(INTDIR)/circ1ctl.obj" \
	"$(INTDIR)/circ1.res"

"$(OUTDIR)\circ1.ocx" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

# Begin Custom Build - Registering OLE control...
OutDir=.\UniDebug
TargetPath=.\UniDebug\circ1.ocx
InputPath=.\UniDebug\circ1.ocx
SOURCE=$(InputPath)

"$(OutDir)\regsvr32.trg" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   regsvr32 /s /c "$(TargetPath)"
   echo regsvr32 exec. time > "$(OutDir)\regsvr32.trg"

# End Custom Build

!ELSEIF  "$(CFG)" == "Circ1 - Win32 Unicode Release"

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

ALL : "$(OUTDIR)\circ1.ocx" "$(OUTDIR)\regsvr32.trg"

CLEAN : 
	-@erase ".\UniRelease\circ1.lib"
	-@erase ".\UniRelease\stdafx.obj"
	-@erase ".\UniRelease\circ1ppg.obj"
	-@erase ".\UniRelease\circ1.pch"
	-@erase ".\UniRelease\circ1.obj"
	-@erase ".\UniRelease\circ1ctl.obj"
	-@erase ".\UniRelease\circ1.res"
	-@erase ".\UniRelease\circ1.tlb"
	-@erase ".\UniRelease\circ1.exp"
	-@erase ".\UniRelease\regsvr32.trg"
	-@erase ".\UniRelease\circ1.ocx"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_UNICODE" /Yu"stdafx.h" /c
CPP_PROJ=/nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_UNICODE" /Fp"$(INTDIR)/circ1.pch" /Yu"stdafx.h" /Fo"$(INTDIR)/"\
 /c 
CPP_OBJS=.\UniRelease/
CPP_SBRS=
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/circ1.res" /d "NDEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/circ1.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 /nologo /subsystem:windows /dll /machine:I386
LINK32_FLAGS=/nologo /subsystem:windows /dll /incremental:no\
 /pdb:"$(OUTDIR)/circ1.pdb" /machine:I386 /def:".\circ1.def"\
 /out:"$(OUTDIR)/circ1.ocx" /implib:"$(OUTDIR)/circ1.lib" 
DEF_FILE= \
	".\circ1.def"
LINK32_OBJS= \
	"$(INTDIR)/stdafx.obj" \
	"$(INTDIR)/circ1ppg.obj" \
	"$(INTDIR)/circ1.obj" \
	"$(INTDIR)/circ1ctl.obj" \
	"$(INTDIR)/circ1.res"

"$(OUTDIR)\circ1.ocx" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

# Begin Custom Build - Registering OLE control...
OutDir=.\UniRelease
TargetPath=.\UniRelease\circ1.ocx
InputPath=.\UniRelease\circ1.ocx
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

# Name "Circ1 - Win32 Release"
# Name "Circ1 - Win32 Debug"
# Name "Circ1 - Win32 Unicode Debug"
# Name "Circ1 - Win32 Unicode Release"

!IF  "$(CFG)" == "Circ1 - Win32 Release"

!ELSEIF  "$(CFG)" == "Circ1 - Win32 Debug"

!ELSEIF  "$(CFG)" == "Circ1 - Win32 Unicode Debug"

!ELSEIF  "$(CFG)" == "Circ1 - Win32 Unicode Release"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\readme.txt

!IF  "$(CFG)" == "Circ1 - Win32 Release"

!ELSEIF  "$(CFG)" == "Circ1 - Win32 Debug"

!ELSEIF  "$(CFG)" == "Circ1 - Win32 Unicode Debug"

!ELSEIF  "$(CFG)" == "Circ1 - Win32 Unicode Release"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\stdafx.cpp
DEP_CPP_STDAF=\
	".\stdafx.h"\
	

!IF  "$(CFG)" == "Circ1 - Win32 Release"

# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/circ1.pch" /Yc"stdafx.h" /Fo"$(INTDIR)/" /c\
 $(SOURCE) \
	

"$(INTDIR)\stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\circ1.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "Circ1 - Win32 Debug"

# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/circ1.pch" /Yc"stdafx.h" /Fo"$(INTDIR)/"\
 /Fd"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\circ1.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "Circ1 - Win32 Unicode Debug"

# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /D "_AFXDLL" /D "_UNICODE" /Fp"$(INTDIR)/circ1.pch" /Yc"stdafx.h"\
 /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\circ1.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "Circ1 - Win32 Unicode Release"

# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_UNICODE" /Fp"$(INTDIR)/circ1.pch" /Yc"stdafx.h" /Fo"$(INTDIR)/"\
 /c $(SOURCE) \
	

"$(INTDIR)\stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\circ1.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\circ1.cpp
DEP_CPP_CIRC1=\
	".\circ1.h"\
	".\stdafx.h"\
	

!IF  "$(CFG)" == "Circ1 - Win32 Release"


"$(INTDIR)\circ1.obj" : $(SOURCE) $(DEP_CPP_CIRC1) "$(INTDIR)"\
 "$(INTDIR)\circ1.pch"


!ELSEIF  "$(CFG)" == "Circ1 - Win32 Debug"


"$(INTDIR)\circ1.obj" : $(SOURCE) $(DEP_CPP_CIRC1) "$(INTDIR)"\
 "$(INTDIR)\circ1.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_CIRC1) "$(INTDIR)"\
 "$(INTDIR)\circ1.pch"


!ELSEIF  "$(CFG)" == "Circ1 - Win32 Unicode Debug"


"$(INTDIR)\circ1.obj" : $(SOURCE) $(DEP_CPP_CIRC1) "$(INTDIR)"\
 "$(INTDIR)\circ1.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_CIRC1) "$(INTDIR)"\
 "$(INTDIR)\circ1.pch"


!ELSEIF  "$(CFG)" == "Circ1 - Win32 Unicode Release"


"$(INTDIR)\circ1.obj" : $(SOURCE) $(DEP_CPP_CIRC1) "$(INTDIR)"\
 "$(INTDIR)\circ1.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\circ1.def

!IF  "$(CFG)" == "Circ1 - Win32 Release"

!ELSEIF  "$(CFG)" == "Circ1 - Win32 Debug"

!ELSEIF  "$(CFG)" == "Circ1 - Win32 Unicode Debug"

!ELSEIF  "$(CFG)" == "Circ1 - Win32 Unicode Release"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\circ1.rc

!IF  "$(CFG)" == "Circ1 - Win32 Release"

DEP_RSC_CIRC1_=\
	".\circ1.ico"\
	".\circ1ctl.bmp"\
	".\circ1.rc2"\
	
NODEP_RSC_CIRC1_=\
	".\Release\circ1.tlb"\
	

"$(INTDIR)\circ1.res" : $(SOURCE) $(DEP_RSC_CIRC1_) "$(INTDIR)"\
 "$(INTDIR)\Circ1.tlb"
   $(RSC) /l 0x409 /fo"$(INTDIR)/circ1.res" /i "Release" /d "NDEBUG" /d\
 "_AFXDLL" $(SOURCE)


!ELSEIF  "$(CFG)" == "Circ1 - Win32 Debug"

DEP_RSC_CIRC1_=\
	".\circ1.ico"\
	".\circ1ctl.bmp"\
	".\circ1.rc2"\
	".\Debug\circ1.tlb"\
	

"$(INTDIR)\circ1.res" : $(SOURCE) $(DEP_RSC_CIRC1_) "$(INTDIR)"\
 "$(INTDIR)\Circ1.tlb"
   $(RSC) /l 0x409 /fo"$(INTDIR)/circ1.res" /i "Debug" /d "_DEBUG" /d "_AFXDLL"\
 $(SOURCE)


!ELSEIF  "$(CFG)" == "Circ1 - Win32 Unicode Debug"

DEP_RSC_CIRC1_=\
	".\circ1.ico"\
	".\circ1ctl.bmp"\
	".\circ1.rc2"\
	
NODEP_RSC_CIRC1_=\
	".\UniDebug\circ1.tlb"\
	

"$(INTDIR)\circ1.res" : $(SOURCE) $(DEP_RSC_CIRC1_) "$(INTDIR)"\
 "$(INTDIR)\Circ1.tlb"
   $(RSC) /l 0x409 /fo"$(INTDIR)/circ1.res" /i "UniDebug" /d "_DEBUG" /d\
 "_AFXDLL" $(SOURCE)


!ELSEIF  "$(CFG)" == "Circ1 - Win32 Unicode Release"

DEP_RSC_CIRC1_=\
	".\circ1.ico"\
	".\circ1ctl.bmp"\
	".\circ1.rc2"\
	
NODEP_RSC_CIRC1_=\
	".\UniRelease\circ1.tlb"\
	

"$(INTDIR)\circ1.res" : $(SOURCE) $(DEP_RSC_CIRC1_) "$(INTDIR)"\
 "$(INTDIR)\Circ1.tlb"
   $(RSC) /l 0x409 /fo"$(INTDIR)/circ1.res" /i "UniRelease" /d "NDEBUG" /d\
 "_AFXDLL" $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\circ1.odl

!IF  "$(CFG)" == "Circ1 - Win32 Release"


"$(OUTDIR)\Circ1.tlb" : $(SOURCE) "$(OUTDIR)"
   $(MTL) /nologo /D "NDEBUG" /tlb "$(OUTDIR)/circ1.tlb" /win32 $(SOURCE)


!ELSEIF  "$(CFG)" == "Circ1 - Win32 Debug"


"$(OUTDIR)\Circ1.tlb" : $(SOURCE) "$(OUTDIR)"
   $(MTL) /nologo /D "_DEBUG" /tlb "$(OUTDIR)/circ1.tlb" /win32 $(SOURCE)


!ELSEIF  "$(CFG)" == "Circ1 - Win32 Unicode Debug"


"$(OUTDIR)\Circ1.tlb" : $(SOURCE) "$(OUTDIR)"
   $(MTL) /nologo /D "_DEBUG" /tlb "$(OUTDIR)/circ1.tlb" /win32 $(SOURCE)


!ELSEIF  "$(CFG)" == "Circ1 - Win32 Unicode Release"


"$(OUTDIR)\Circ1.tlb" : $(SOURCE) "$(OUTDIR)"
   $(MTL) /nologo /D "NDEBUG" /tlb "$(OUTDIR)/circ1.tlb" /win32 $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\circ1ctl.cpp
DEP_CPP_CIRC1C=\
	".\circ1.h"\
	".\circ1ctl.h"\
	".\circ1ppg.h"\
	".\stdafx.h"\
	

!IF  "$(CFG)" == "Circ1 - Win32 Release"


"$(INTDIR)\circ1ctl.obj" : $(SOURCE) $(DEP_CPP_CIRC1C) "$(INTDIR)"\
 "$(INTDIR)\circ1.pch"


!ELSEIF  "$(CFG)" == "Circ1 - Win32 Debug"


"$(INTDIR)\circ1ctl.obj" : $(SOURCE) $(DEP_CPP_CIRC1C) "$(INTDIR)"\
 "$(INTDIR)\circ1.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_CIRC1C) "$(INTDIR)"\
 "$(INTDIR)\circ1.pch"


!ELSEIF  "$(CFG)" == "Circ1 - Win32 Unicode Debug"


"$(INTDIR)\circ1ctl.obj" : $(SOURCE) $(DEP_CPP_CIRC1C) "$(INTDIR)"\
 "$(INTDIR)\circ1.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_CIRC1C) "$(INTDIR)"\
 "$(INTDIR)\circ1.pch"


!ELSEIF  "$(CFG)" == "Circ1 - Win32 Unicode Release"


"$(INTDIR)\circ1ctl.obj" : $(SOURCE) $(DEP_CPP_CIRC1C) "$(INTDIR)"\
 "$(INTDIR)\circ1.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\circ1ppg.cpp
DEP_CPP_CIRC1P=\
	".\circ1.h"\
	".\circ1ppg.h"\
	".\stdafx.h"\
	

!IF  "$(CFG)" == "Circ1 - Win32 Release"


"$(INTDIR)\circ1ppg.obj" : $(SOURCE) $(DEP_CPP_CIRC1P) "$(INTDIR)"\
 "$(INTDIR)\circ1.pch"


!ELSEIF  "$(CFG)" == "Circ1 - Win32 Debug"


"$(INTDIR)\circ1ppg.obj" : $(SOURCE) $(DEP_CPP_CIRC1P) "$(INTDIR)"\
 "$(INTDIR)\circ1.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_CIRC1P) "$(INTDIR)"\
 "$(INTDIR)\circ1.pch"


!ELSEIF  "$(CFG)" == "Circ1 - Win32 Unicode Debug"


"$(INTDIR)\circ1ppg.obj" : $(SOURCE) $(DEP_CPP_CIRC1P) "$(INTDIR)"\
 "$(INTDIR)\circ1.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_CIRC1P) "$(INTDIR)"\
 "$(INTDIR)\circ1.pch"


!ELSEIF  "$(CFG)" == "Circ1 - Win32 Unicode Release"


"$(INTDIR)\circ1ppg.obj" : $(SOURCE) $(DEP_CPP_CIRC1P) "$(INTDIR)"\
 "$(INTDIR)\circ1.pch"


!ENDIF 

# End Source File
# End Target
# End Project
################################################################################
