# Microsoft Visual C++ Generated NMAKE File, Format Version 40000
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

!IF "$(CFG)" == ""
CFG=Pal - Win32 Debug
!MESSAGE No configuration specified.  Defaulting to Pal - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "Pal - Win32 Release" && "$(CFG)" != "Pal - Win32 Debug" &&\
 "$(CFG)" != "Pal - Win32 Unicode Debug" && "$(CFG)" !=\
 "Pal - Win32 Unicode Release"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "pal.mak" CFG="Pal - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Pal - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "Pal - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "Pal - Win32 Unicode Debug" (based on\
 "Win32 (x86) Dynamic-Link Library")
!MESSAGE "Pal - Win32 Unicode Release" (based on\
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
# PROP Target_Last_Scanned "Pal - Win32 Unicode Release"
MTL=mktyplib.exe
RSC=rc.exe
CPP=cl.exe

!IF  "$(CFG)" == "Pal - Win32 Release"

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

ALL : "$(OUTDIR)\pal.ocx" "$(OUTDIR)\regsvr32.trg"

CLEAN : 
	-@erase ".\Release\pal.lib"
	-@erase ".\Release\palctl.obj"
	-@erase ".\Release\pal.pch"
	-@erase ".\Release\palppg.obj"
	-@erase ".\Release\pal.obj"
	-@erase ".\Release\stdafx.obj"
	-@erase ".\Release\pal.res"
	-@erase ".\Release\pal.tlb"
	-@erase ".\Release\pal.exp"
	-@erase ".\Release\regsvr32.trg"
	-@erase ".\Release\pal.ocx"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
CPP_PROJ=/nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/pal.pch" /Yu"stdafx.h" /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\Release/
CPP_SBRS=
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/pal.res" /d "NDEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/pal.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 /nologo /subsystem:windows /dll /machine:I386
LINK32_FLAGS=/nologo /subsystem:windows /dll /incremental:no\
 /pdb:"$(OUTDIR)/pal.pdb" /machine:I386 /def:".\pal.def"\
 /out:"$(OUTDIR)/pal.ocx" /implib:"$(OUTDIR)/pal.lib" 
DEF_FILE= \
	".\pal.def"
LINK32_OBJS= \
	"$(INTDIR)/palctl.obj" \
	"$(INTDIR)/palppg.obj" \
	"$(INTDIR)/pal.obj" \
	"$(INTDIR)/stdafx.obj" \
	"$(INTDIR)/pal.res"

"$(OUTDIR)\pal.ocx" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

# Begin Custom Build - Registering OLE control...
OutDir=.\Release
TargetPath=.\Release\pal.ocx
InputPath=.\Release\pal.ocx
SOURCE=$(InputPath)

"$(OutDir)\regsvr32.trg" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   regsvr32 /s /c "$(TargetPath)"
   echo regsvr32 exec. time > "$(OutDir)\regsvr32.trg"

# End Custom Build

!ELSEIF  "$(CFG)" == "Pal - Win32 Debug"

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

ALL : "$(OUTDIR)\pal.ocx" "$(OUTDIR)\regsvr32.trg"

CLEAN : 
	-@erase ".\Debug\vc40.pdb"
	-@erase ".\Debug\pal.pch"
	-@erase ".\Debug\pal.ilk"
	-@erase ".\Debug\palppg.obj"
	-@erase ".\Debug\stdafx.obj"
	-@erase ".\Debug\palctl.obj"
	-@erase ".\Debug\pal.obj"
	-@erase ".\Debug\pal.res"
	-@erase ".\Debug\pal.tlb"
	-@erase ".\Debug\pal.lib"
	-@erase ".\Debug\pal.exp"
	-@erase ".\Debug\pal.pdb"
	-@erase ".\Debug\regsvr32.trg"
	-@erase ".\Debug\pal.ocx"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
CPP_PROJ=/nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/pal.pch" /Yu"stdafx.h" /Fo"$(INTDIR)/"\
 /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\Debug/
CPP_SBRS=
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/pal.res" /d "_DEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/pal.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /debug /machine:I386
# ADD LINK32 /nologo /subsystem:windows /dll /debug /machine:I386
LINK32_FLAGS=/nologo /subsystem:windows /dll /incremental:yes\
 /pdb:"$(OUTDIR)/pal.pdb" /debug /machine:I386 /def:".\pal.def"\
 /out:"$(OUTDIR)/pal.ocx" /implib:"$(OUTDIR)/pal.lib" 
DEF_FILE= \
	".\pal.def"
LINK32_OBJS= \
	"$(INTDIR)/palppg.obj" \
	"$(INTDIR)/stdafx.obj" \
	"$(INTDIR)/palctl.obj" \
	"$(INTDIR)/pal.obj" \
	"$(INTDIR)/pal.res"

"$(OUTDIR)\pal.ocx" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

# Begin Custom Build - Registering OLE control...
OutDir=.\Debug
TargetPath=.\Debug\pal.ocx
InputPath=.\Debug\pal.ocx
SOURCE=$(InputPath)

"$(OutDir)\regsvr32.trg" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   regsvr32 /s /c "$(TargetPath)"
   echo regsvr32 exec. time > "$(OutDir)\regsvr32.trg"

# End Custom Build

!ELSEIF  "$(CFG)" == "Pal - Win32 Unicode Debug"

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

ALL : "$(OUTDIR)\pal.ocx" "$(OUTDIR)\regsvr32.trg"

CLEAN : 
	-@erase ".\UniDebug\vc40.pdb"
	-@erase ".\UniDebug\pal.pch"
	-@erase ".\UniDebug\pal.ilk"
	-@erase ".\UniDebug\pal.obj"
	-@erase ".\UniDebug\palctl.obj"
	-@erase ".\UniDebug\palppg.obj"
	-@erase ".\UniDebug\stdafx.obj"
	-@erase ".\UniDebug\pal.res"
	-@erase ".\UniDebug\pal.tlb"
	-@erase ".\UniDebug\pal.lib"
	-@erase ".\UniDebug\pal.exp"
	-@erase ".\UniDebug\pal.pdb"
	-@erase ".\UniDebug\regsvr32.trg"
	-@erase ".\UniDebug\pal.ocx"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_UNICODE" /Yu"stdafx.h" /c
CPP_PROJ=/nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /D "_AFXDLL" /D "_UNICODE" /Fp"$(INTDIR)/pal.pch" /Yu"stdafx.h" /Fo"$(INTDIR)/"\
 /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\UniDebug/
CPP_SBRS=
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/pal.res" /d "_DEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/pal.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /debug /machine:I386
# ADD LINK32 /nologo /subsystem:windows /dll /debug /machine:I386
LINK32_FLAGS=/nologo /subsystem:windows /dll /incremental:yes\
 /pdb:"$(OUTDIR)/pal.pdb" /debug /machine:I386 /def:".\pal.def"\
 /out:"$(OUTDIR)/pal.ocx" /implib:"$(OUTDIR)/pal.lib" 
DEF_FILE= \
	".\pal.def"
LINK32_OBJS= \
	"$(INTDIR)/pal.obj" \
	"$(INTDIR)/palctl.obj" \
	"$(INTDIR)/palppg.obj" \
	"$(INTDIR)/stdafx.obj" \
	"$(INTDIR)/pal.res"

"$(OUTDIR)\pal.ocx" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

# Begin Custom Build - Registering OLE control...
OutDir=.\UniDebug
TargetPath=.\UniDebug\pal.ocx
InputPath=.\UniDebug\pal.ocx
SOURCE=$(InputPath)

"$(OutDir)\regsvr32.trg" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   regsvr32 /s /c "$(TargetPath)"
   echo regsvr32 exec. time > "$(OutDir)\regsvr32.trg"

# End Custom Build

!ELSEIF  "$(CFG)" == "Pal - Win32 Unicode Release"

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

ALL : "$(OUTDIR)\pal.ocx" "$(OUTDIR)\regsvr32.trg"

CLEAN : 
	-@erase ".\UniRelease\pal.lib"
	-@erase ".\UniRelease\palppg.obj"
	-@erase ".\UniRelease\pal.pch"
	-@erase ".\UniRelease\stdafx.obj"
	-@erase ".\UniRelease\pal.obj"
	-@erase ".\UniRelease\palctl.obj"
	-@erase ".\UniRelease\pal.res"
	-@erase ".\UniRelease\pal.tlb"
	-@erase ".\UniRelease\pal.exp"
	-@erase ".\UniRelease\regsvr32.trg"
	-@erase ".\UniRelease\pal.ocx"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_UNICODE" /Yu"stdafx.h" /c
CPP_PROJ=/nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_UNICODE" /Fp"$(INTDIR)/pal.pch" /Yu"stdafx.h" /Fo"$(INTDIR)/" /c\
 
CPP_OBJS=.\UniRelease/
CPP_SBRS=
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/pal.res" /d "NDEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/pal.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 /nologo /subsystem:windows /dll /machine:I386
LINK32_FLAGS=/nologo /subsystem:windows /dll /incremental:no\
 /pdb:"$(OUTDIR)/pal.pdb" /machine:I386 /def:".\pal.def"\
 /out:"$(OUTDIR)/pal.ocx" /implib:"$(OUTDIR)/pal.lib" 
DEF_FILE= \
	".\pal.def"
LINK32_OBJS= \
	"$(INTDIR)/palppg.obj" \
	"$(INTDIR)/stdafx.obj" \
	"$(INTDIR)/pal.obj" \
	"$(INTDIR)/palctl.obj" \
	"$(INTDIR)/pal.res"

"$(OUTDIR)\pal.ocx" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

# Begin Custom Build - Registering OLE control...
OutDir=.\UniRelease
TargetPath=.\UniRelease\pal.ocx
InputPath=.\UniRelease\pal.ocx
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

# Name "Pal - Win32 Release"
# Name "Pal - Win32 Debug"
# Name "Pal - Win32 Unicode Debug"
# Name "Pal - Win32 Unicode Release"

!IF  "$(CFG)" == "Pal - Win32 Release"

!ELSEIF  "$(CFG)" == "Pal - Win32 Debug"

!ELSEIF  "$(CFG)" == "Pal - Win32 Unicode Debug"

!ELSEIF  "$(CFG)" == "Pal - Win32 Unicode Release"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\ReadMe.txt

!IF  "$(CFG)" == "Pal - Win32 Release"

!ELSEIF  "$(CFG)" == "Pal - Win32 Debug"

!ELSEIF  "$(CFG)" == "Pal - Win32 Unicode Debug"

!ELSEIF  "$(CFG)" == "Pal - Win32 Unicode Release"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\stdafx.cpp
DEP_CPP_STDAF=\
	".\stdafx.h"\
	

!IF  "$(CFG)" == "Pal - Win32 Release"

# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/pal.pch" /Yc"stdafx.h" /Fo"$(INTDIR)/" /c\
 $(SOURCE) \
	

"$(INTDIR)\stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\pal.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "Pal - Win32 Debug"

# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/pal.pch" /Yc"stdafx.h" /Fo"$(INTDIR)/"\
 /Fd"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\pal.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "Pal - Win32 Unicode Debug"

# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /D "_AFXDLL" /D "_UNICODE" /Fp"$(INTDIR)/pal.pch" /Yc"stdafx.h" /Fo"$(INTDIR)/"\
 /Fd"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\pal.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "Pal - Win32 Unicode Release"

# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_UNICODE" /Fp"$(INTDIR)/pal.pch" /Yc"stdafx.h" /Fo"$(INTDIR)/" /c\
 $(SOURCE) \
	

"$(INTDIR)\stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\pal.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\pal.cpp
DEP_CPP_PAL_C=\
	".\pal.h"\
	".\stdafx.h"\
	

!IF  "$(CFG)" == "Pal - Win32 Release"


"$(INTDIR)\pal.obj" : $(SOURCE) $(DEP_CPP_PAL_C) "$(INTDIR)"\
 "$(INTDIR)\pal.pch"


!ELSEIF  "$(CFG)" == "Pal - Win32 Debug"


"$(INTDIR)\pal.obj" : $(SOURCE) $(DEP_CPP_PAL_C) "$(INTDIR)"\
 "$(INTDIR)\pal.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_PAL_C) "$(INTDIR)"\
 "$(INTDIR)\pal.pch"


!ELSEIF  "$(CFG)" == "Pal - Win32 Unicode Debug"


"$(INTDIR)\pal.obj" : $(SOURCE) $(DEP_CPP_PAL_C) "$(INTDIR)"\
 "$(INTDIR)\pal.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_PAL_C) "$(INTDIR)"\
 "$(INTDIR)\pal.pch"


!ELSEIF  "$(CFG)" == "Pal - Win32 Unicode Release"


"$(INTDIR)\pal.obj" : $(SOURCE) $(DEP_CPP_PAL_C) "$(INTDIR)"\
 "$(INTDIR)\pal.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\pal.def

!IF  "$(CFG)" == "Pal - Win32 Release"

!ELSEIF  "$(CFG)" == "Pal - Win32 Debug"

!ELSEIF  "$(CFG)" == "Pal - Win32 Unicode Debug"

!ELSEIF  "$(CFG)" == "Pal - Win32 Unicode Release"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\pal.rc

!IF  "$(CFG)" == "Pal - Win32 Release"

DEP_RSC_PAL_R=\
	".\pal.ico"\
	".\palctl.bmp"\
	".\resource.h"\
	".\pal.rc2"\
	".\Release\pal.tlb"\
	

"$(INTDIR)\pal.res" : $(SOURCE) $(DEP_RSC_PAL_R) "$(INTDIR)"\
 "$(INTDIR)\pal.tlb"
   $(RSC) /l 0x409 /fo"$(INTDIR)/pal.res" /i "Release" /d "NDEBUG" /d "_AFXDLL"\
 $(SOURCE)


!ELSEIF  "$(CFG)" == "Pal - Win32 Debug"

DEP_RSC_PAL_R=\
	".\pal.ico"\
	".\palctl.bmp"\
	".\resource.h"\
	".\pal.rc2"\
	".\Debug\pal.tlb"\
	

"$(INTDIR)\pal.res" : $(SOURCE) $(DEP_RSC_PAL_R) "$(INTDIR)"\
 "$(INTDIR)\pal.tlb"
   $(RSC) /l 0x409 /fo"$(INTDIR)/pal.res" /i "Debug" /d "_DEBUG" /d "_AFXDLL"\
 $(SOURCE)


!ELSEIF  "$(CFG)" == "Pal - Win32 Unicode Debug"

DEP_RSC_PAL_R=\
	".\pal.ico"\
	".\palctl.bmp"\
	".\resource.h"\
	".\pal.rc2"\
	".\UniDebug\pal.tlb"\
	

"$(INTDIR)\pal.res" : $(SOURCE) $(DEP_RSC_PAL_R) "$(INTDIR)"\
 "$(INTDIR)\pal.tlb"
   $(RSC) /l 0x409 /fo"$(INTDIR)/pal.res" /i "UniDebug" /d "_DEBUG" /d\
 "_AFXDLL" $(SOURCE)


!ELSEIF  "$(CFG)" == "Pal - Win32 Unicode Release"

DEP_RSC_PAL_R=\
	".\UniRelease\pal.tlb"\
	".\pal.ico"\
	".\palctl.bmp"\
	".\resource.h"\
	".\pal.rc2"\
	

"$(INTDIR)\pal.res" : $(SOURCE) $(DEP_RSC_PAL_R) "$(INTDIR)"\
 "$(INTDIR)\pal.tlb"
   $(RSC) /l 0x409 /fo"$(INTDIR)/pal.res" /i "UniRelease" /d "NDEBUG" /d\
 "_AFXDLL" $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\pal.odl

!IF  "$(CFG)" == "Pal - Win32 Release"


"$(OUTDIR)\pal.tlb" : $(SOURCE) "$(OUTDIR)"
   $(MTL) /nologo /D "NDEBUG" /tlb "$(OUTDIR)/pal.tlb" /win32 $(SOURCE)


!ELSEIF  "$(CFG)" == "Pal - Win32 Debug"


"$(OUTDIR)\pal.tlb" : $(SOURCE) "$(OUTDIR)"
   $(MTL) /nologo /D "_DEBUG" /tlb "$(OUTDIR)/pal.tlb" /win32 $(SOURCE)


!ELSEIF  "$(CFG)" == "Pal - Win32 Unicode Debug"


"$(OUTDIR)\pal.tlb" : $(SOURCE) "$(OUTDIR)"
   $(MTL) /nologo /D "_DEBUG" /tlb "$(OUTDIR)/pal.tlb" /win32 $(SOURCE)


!ELSEIF  "$(CFG)" == "Pal - Win32 Unicode Release"


"$(OUTDIR)\pal.tlb" : $(SOURCE) "$(OUTDIR)"
   $(MTL) /nologo /D "NDEBUG" /tlb "$(OUTDIR)/pal.tlb" /win32 $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\palctl.cpp
DEP_CPP_PALCT=\
	".\pal.h"\
	".\palctl.h"\
	".\palppg.h"\
	".\stdafx.h"\
	

!IF  "$(CFG)" == "Pal - Win32 Release"


"$(INTDIR)\palctl.obj" : $(SOURCE) $(DEP_CPP_PALCT) "$(INTDIR)"\
 "$(INTDIR)\pal.pch"


!ELSEIF  "$(CFG)" == "Pal - Win32 Debug"


"$(INTDIR)\palctl.obj" : $(SOURCE) $(DEP_CPP_PALCT) "$(INTDIR)"\
 "$(INTDIR)\pal.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_PALCT) "$(INTDIR)"\
 "$(INTDIR)\pal.pch"


!ELSEIF  "$(CFG)" == "Pal - Win32 Unicode Debug"


"$(INTDIR)\palctl.obj" : $(SOURCE) $(DEP_CPP_PALCT) "$(INTDIR)"\
 "$(INTDIR)\pal.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_PALCT) "$(INTDIR)"\
 "$(INTDIR)\pal.pch"


!ELSEIF  "$(CFG)" == "Pal - Win32 Unicode Release"


"$(INTDIR)\palctl.obj" : $(SOURCE) $(DEP_CPP_PALCT) "$(INTDIR)"\
 "$(INTDIR)\pal.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\palppg.cpp
DEP_CPP_PALPP=\
	".\pal.h"\
	".\palppg.h"\
	".\stdafx.h"\
	

!IF  "$(CFG)" == "Pal - Win32 Release"


"$(INTDIR)\palppg.obj" : $(SOURCE) $(DEP_CPP_PALPP) "$(INTDIR)"\
 "$(INTDIR)\pal.pch"


!ELSEIF  "$(CFG)" == "Pal - Win32 Debug"


"$(INTDIR)\palppg.obj" : $(SOURCE) $(DEP_CPP_PALPP) "$(INTDIR)"\
 "$(INTDIR)\pal.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_PALPP) "$(INTDIR)"\
 "$(INTDIR)\pal.pch"


!ELSEIF  "$(CFG)" == "Pal - Win32 Unicode Debug"


"$(INTDIR)\palppg.obj" : $(SOURCE) $(DEP_CPP_PALPP) "$(INTDIR)"\
 "$(INTDIR)\pal.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_PALPP) "$(INTDIR)"\
 "$(INTDIR)\pal.pch"


!ELSEIF  "$(CFG)" == "Pal - Win32 Unicode Release"


"$(INTDIR)\palppg.obj" : $(SOURCE) $(DEP_CPP_PALPP) "$(INTDIR)"\
 "$(INTDIR)\pal.pch"


!ENDIF 

# End Source File
# End Target
# End Project
################################################################################
