# Microsoft Visual C++ Generated NMAKE File, Format Version 40000
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

!IF "$(CFG)" == ""
CFG=Button - Win32 Debug
!MESSAGE No configuration specified.  Defaulting to Button - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "Button - Win32 Release" && "$(CFG)" != "Button - Win32 Debug"\
 && "$(CFG)" != "Button - Win32 Unicode Debug" && "$(CFG)" !=\
 "Button - Win32 Unicode Release"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "button.mak" CFG="Button - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Button - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "Button - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "Button - Win32 Unicode Debug" (based on\
 "Win32 (x86) Dynamic-Link Library")
!MESSAGE "Button - Win32 Unicode Release" (based on\
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
# PROP Target_Last_Scanned "Button - Win32 Unicode Release"
RSC=rc.exe
MTL=mktyplib.exe
CPP=cl.exe

!IF  "$(CFG)" == "Button - Win32 Release"

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

ALL : "$(OUTDIR)\button.ocx" "$(OUTDIR)\regsvr32.trg"

CLEAN : 
	-@erase ".\Release\button.lib"
	-@erase ".\Release\stdafx.obj"
	-@erase ".\Release\btnctl.obj"
	-@erase ".\Release\button.pch"
	-@erase ".\Release\button.obj"
	-@erase ".\Release\btnppg.obj"
	-@erase ".\Release\button.res"
	-@erase ".\Release\button.tlb"
	-@erase ".\Release\button.exp"
	-@erase ".\Release\regsvr32.trg"
	-@erase ".\Release\button.ocx"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
CPP_PROJ=/nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/button.pch" /Yu"stdafx.h" /Fo"$(INTDIR)/" /c\
 
CPP_OBJS=.\Release/
CPP_SBRS=
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/button.res" /d "NDEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/button.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 /nologo /subsystem:windows /dll /machine:I386
LINK32_FLAGS=/nologo /subsystem:windows /dll /incremental:no\
 /pdb:"$(OUTDIR)/button.pdb" /machine:I386 /def:".\button.def"\
 /out:"$(OUTDIR)/button.ocx" /implib:"$(OUTDIR)/button.lib" 
DEF_FILE= \
	".\button.def"
LINK32_OBJS= \
	"$(INTDIR)/stdafx.obj" \
	"$(INTDIR)/btnctl.obj" \
	"$(INTDIR)/button.obj" \
	"$(INTDIR)/btnppg.obj" \
	"$(INTDIR)/button.res"

"$(OUTDIR)\button.ocx" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

# Begin Custom Build - Registering OLE control...
OutDir=.\Release
TargetPath=.\Release\button.ocx
InputPath=.\Release\button.ocx
SOURCE=$(InputPath)

"$(OutDir)\regsvr32.trg" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   regsvr32 /s /c "$(TargetPath)"
   echo regsvr32 exec. time > "$(OutDir)\regsvr32.trg"

# End Custom Build

!ELSEIF  "$(CFG)" == "Button - Win32 Debug"

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

ALL : "$(OUTDIR)\button.ocx" "$(OUTDIR)\regsvr32.trg"

CLEAN : 
	-@erase ".\Debug\vc40.pdb"
	-@erase ".\Debug\button.pch"
	-@erase ".\Debug\button.ilk"
	-@erase ".\Debug\stdafx.obj"
	-@erase ".\Debug\btnctl.obj"
	-@erase ".\Debug\button.obj"
	-@erase ".\Debug\btnppg.obj"
	-@erase ".\Debug\button.res"
	-@erase ".\Debug\button.tlb"
	-@erase ".\Debug\button.lib"
	-@erase ".\Debug\button.exp"
	-@erase ".\Debug\button.pdb"
	-@erase ".\Debug\regsvr32.trg"
	-@erase ".\Debug\button.ocx"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
CPP_PROJ=/nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/button.pch" /Yu"stdafx.h" /Fo"$(INTDIR)/"\
 /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\Debug/
CPP_SBRS=
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/button.res" /d "_DEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/button.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /debug /machine:I386
# ADD LINK32 /nologo /subsystem:windows /dll /debug /machine:I386
LINK32_FLAGS=/nologo /subsystem:windows /dll /incremental:yes\
 /pdb:"$(OUTDIR)/button.pdb" /debug /machine:I386 /def:".\button.def"\
 /out:"$(OUTDIR)/button.ocx" /implib:"$(OUTDIR)/button.lib" 
DEF_FILE= \
	".\button.def"
LINK32_OBJS= \
	"$(INTDIR)/stdafx.obj" \
	"$(INTDIR)/btnctl.obj" \
	"$(INTDIR)/button.obj" \
	"$(INTDIR)/btnppg.obj" \
	"$(INTDIR)/button.res"

"$(OUTDIR)\button.ocx" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

# Begin Custom Build - Registering OLE control...
OutDir=.\Debug
TargetPath=.\Debug\button.ocx
InputPath=.\Debug\button.ocx
SOURCE=$(InputPath)

"$(OutDir)\regsvr32.trg" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   regsvr32 /s /c "$(TargetPath)"
   echo regsvr32 exec. time > "$(OutDir)\regsvr32.trg"

# End Custom Build

!ELSEIF  "$(CFG)" == "Button - Win32 Unicode Debug"

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

ALL : "$(OUTDIR)\button.ocx" "$(OUTDIR)\regsvr32.trg"

CLEAN : 
	-@erase ".\UniDebug\vc40.pdb"
	-@erase ".\UniDebug\button.pch"
	-@erase ".\UniDebug\button.ilk"
	-@erase ".\UniDebug\btnppg.obj"
	-@erase ".\UniDebug\stdafx.obj"
	-@erase ".\UniDebug\btnctl.obj"
	-@erase ".\UniDebug\button.obj"
	-@erase ".\UniDebug\button.res"
	-@erase ".\UniDebug\button.tlb"
	-@erase ".\UniDebug\button.lib"
	-@erase ".\UniDebug\button.exp"
	-@erase ".\UniDebug\button.pdb"
	-@erase ".\UniDebug\regsvr32.trg"
	-@erase ".\UniDebug\button.ocx"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_UNICODE" /Yu"stdafx.h" /c
CPP_PROJ=/nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /D "_AFXDLL" /D "_UNICODE" /Fp"$(INTDIR)/button.pch" /Yu"stdafx.h"\
 /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\UniDebug/
CPP_SBRS=
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/button.res" /d "_DEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/button.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /debug /machine:I386
# ADD LINK32 /nologo /subsystem:windows /dll /debug /machine:I386
LINK32_FLAGS=/nologo /subsystem:windows /dll /incremental:yes\
 /pdb:"$(OUTDIR)/button.pdb" /debug /machine:I386 /def:".\button.def"\
 /out:"$(OUTDIR)/button.ocx" /implib:"$(OUTDIR)/button.lib" 
DEF_FILE= \
	".\button.def"
LINK32_OBJS= \
	"$(INTDIR)/btnppg.obj" \
	"$(INTDIR)/stdafx.obj" \
	"$(INTDIR)/btnctl.obj" \
	"$(INTDIR)/button.obj" \
	"$(INTDIR)/button.res"

"$(OUTDIR)\button.ocx" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

# Begin Custom Build - Registering OLE control...
OutDir=.\UniDebug
TargetPath=.\UniDebug\button.ocx
InputPath=.\UniDebug\button.ocx
SOURCE=$(InputPath)

"$(OutDir)\regsvr32.trg" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   regsvr32 /s /c "$(TargetPath)"
   echo regsvr32 exec. time > "$(OutDir)\regsvr32.trg"

# End Custom Build

!ELSEIF  "$(CFG)" == "Button - Win32 Unicode Release"

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

ALL : "$(OUTDIR)\button.ocx" "$(OUTDIR)\regsvr32.trg"

CLEAN : 
	-@erase ".\UniRelease\button.lib"
	-@erase ".\UniRelease\button.obj"
	-@erase ".\UniRelease\button.pch"
	-@erase ".\UniRelease\btnppg.obj"
	-@erase ".\UniRelease\stdafx.obj"
	-@erase ".\UniRelease\btnctl.obj"
	-@erase ".\UniRelease\button.res"
	-@erase ".\UniRelease\button.tlb"
	-@erase ".\UniRelease\button.exp"
	-@erase ".\UniRelease\regsvr32.trg"
	-@erase ".\UniRelease\button.ocx"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_UNICODE" /Yu"stdafx.h" /c
CPP_PROJ=/nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_UNICODE" /Fp"$(INTDIR)/button.pch" /Yu"stdafx.h" /Fo"$(INTDIR)/"\
 /c 
CPP_OBJS=.\UniRelease/
CPP_SBRS=
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/button.res" /d "NDEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/button.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 /nologo /subsystem:windows /dll /machine:I386
# SUBTRACT LINK32 /incremental:yes /debug
LINK32_FLAGS=/nologo /subsystem:windows /dll /incremental:no\
 /pdb:"$(OUTDIR)/button.pdb" /machine:I386 /def:".\button.def"\
 /out:"$(OUTDIR)/button.ocx" /implib:"$(OUTDIR)/button.lib" 
DEF_FILE= \
	".\button.def"
LINK32_OBJS= \
	"$(INTDIR)/button.obj" \
	"$(INTDIR)/btnppg.obj" \
	"$(INTDIR)/stdafx.obj" \
	"$(INTDIR)/btnctl.obj" \
	"$(INTDIR)/button.res"

"$(OUTDIR)\button.ocx" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

# Begin Custom Build - Registering OLE control...
OutDir=.\UniRelease
TargetPath=.\UniRelease\button.ocx
InputPath=.\UniRelease\button.ocx
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

# Name "Button - Win32 Release"
# Name "Button - Win32 Debug"
# Name "Button - Win32 Unicode Debug"
# Name "Button - Win32 Unicode Release"

!IF  "$(CFG)" == "Button - Win32 Release"

!ELSEIF  "$(CFG)" == "Button - Win32 Debug"

!ELSEIF  "$(CFG)" == "Button - Win32 Unicode Debug"

!ELSEIF  "$(CFG)" == "Button - Win32 Unicode Release"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\ReadMe.txt

!IF  "$(CFG)" == "Button - Win32 Release"

!ELSEIF  "$(CFG)" == "Button - Win32 Debug"

!ELSEIF  "$(CFG)" == "Button - Win32 Unicode Debug"

!ELSEIF  "$(CFG)" == "Button - Win32 Unicode Release"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\stdafx.cpp
DEP_CPP_STDAF=\
	".\stdafx.h"\
	

!IF  "$(CFG)" == "Button - Win32 Release"

# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/button.pch" /Yc"stdafx.h" /Fo"$(INTDIR)/" /c\
 $(SOURCE) \
	

"$(INTDIR)\stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\button.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "Button - Win32 Debug"

# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/button.pch" /Yc"stdafx.h" /Fo"$(INTDIR)/"\
 /Fd"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\button.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "Button - Win32 Unicode Debug"

# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /D "_AFXDLL" /D "_UNICODE" /Fp"$(INTDIR)/button.pch" /Yc"stdafx.h"\
 /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\button.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "Button - Win32 Unicode Release"

# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_UNICODE" /Fp"$(INTDIR)/button.pch" /Yc"stdafx.h" /Fo"$(INTDIR)/"\
 /c $(SOURCE) \
	

"$(INTDIR)\stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\button.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\button.cpp

!IF  "$(CFG)" == "Button - Win32 Release"

DEP_CPP_BUTTO=\
	".\button.h"\
	".\stdafx.h"\
	

"$(INTDIR)\button.obj" : $(SOURCE) $(DEP_CPP_BUTTO) "$(INTDIR)"\
 "$(INTDIR)\button.pch"


!ELSEIF  "$(CFG)" == "Button - Win32 Debug"

DEP_CPP_BUTTO=\
	".\button.h"\
	".\stdafx.h"\
	

"$(INTDIR)\button.obj" : $(SOURCE) $(DEP_CPP_BUTTO) "$(INTDIR)"\
 "$(INTDIR)\button.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_BUTTO) "$(INTDIR)"\
 "$(INTDIR)\button.pch"


!ELSEIF  "$(CFG)" == "Button - Win32 Unicode Debug"

DEP_CPP_BUTTO=\
	".\stdafx.h"\
	".\button.h"\
	".\resource.h"\
	

"$(INTDIR)\button.obj" : $(SOURCE) $(DEP_CPP_BUTTO) "$(INTDIR)"\
 "$(INTDIR)\button.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_BUTTO) "$(INTDIR)"\
 "$(INTDIR)\button.pch"


!ELSEIF  "$(CFG)" == "Button - Win32 Unicode Release"

DEP_CPP_BUTTO=\
	".\stdafx.h"\
	".\button.h"\
	".\resource.h"\
	

"$(INTDIR)\button.obj" : $(SOURCE) $(DEP_CPP_BUTTO) "$(INTDIR)"\
 "$(INTDIR)\button.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\button.def

!IF  "$(CFG)" == "Button - Win32 Release"

!ELSEIF  "$(CFG)" == "Button - Win32 Debug"

!ELSEIF  "$(CFG)" == "Button - Win32 Unicode Debug"

!ELSEIF  "$(CFG)" == "Button - Win32 Unicode Release"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\button.rc

!IF  "$(CFG)" == "Button - Win32 Release"

DEP_RSC_BUTTON=\
	".\button.ico"\
	".\btnctl.bmp"\
	".\resource.h"\
	".\button.rc2"\
	".\Release\button.tlb"\
	

"$(INTDIR)\button.res" : $(SOURCE) $(DEP_RSC_BUTTON) "$(INTDIR)"\
 "$(INTDIR)\button.tlb"
   $(RSC) /l 0x409 /fo"$(INTDIR)/button.res" /i "Release" /d "NDEBUG" /d\
 "_AFXDLL" $(SOURCE)


!ELSEIF  "$(CFG)" == "Button - Win32 Debug"

DEP_RSC_BUTTON=\
	".\button.ico"\
	".\btnctl.bmp"\
	".\resource.h"\
	".\button.rc2"\
	".\Debug\button.tlb"\
	

"$(INTDIR)\button.res" : $(SOURCE) $(DEP_RSC_BUTTON) "$(INTDIR)"\
 "$(INTDIR)\button.tlb"
   $(RSC) /l 0x409 /fo"$(INTDIR)/button.res" /i "Debug" /d "_DEBUG" /d\
 "_AFXDLL" $(SOURCE)


!ELSEIF  "$(CFG)" == "Button - Win32 Unicode Debug"

DEP_RSC_BUTTON=\
	".\UniDebug\button.tlb"\
	".\button.ico"\
	".\btnctl.bmp"\
	".\resource.h"\
	".\button.rc2"\
	

"$(INTDIR)\button.res" : $(SOURCE) $(DEP_RSC_BUTTON) "$(INTDIR)"\
 "$(INTDIR)\button.tlb"
   $(RSC) /l 0x409 /fo"$(INTDIR)/button.res" /i "UniDebug" /d "_DEBUG" /d\
 "_AFXDLL" $(SOURCE)


!ELSEIF  "$(CFG)" == "Button - Win32 Unicode Release"

DEP_RSC_BUTTON=\
	".\button.ico"\
	".\btnctl.bmp"\
	".\resource.h"\
	".\button.rc2"\
	".\UniRelease\button.tlb"\
	

"$(INTDIR)\button.res" : $(SOURCE) $(DEP_RSC_BUTTON) "$(INTDIR)"\
 "$(INTDIR)\button.tlb"
   $(RSC) /l 0x409 /fo"$(INTDIR)/button.res" /i "UniRelease" /d "NDEBUG" /d\
 "_AFXDLL" $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\button.odl

!IF  "$(CFG)" == "Button - Win32 Release"


"$(OUTDIR)\button.tlb" : $(SOURCE) "$(OUTDIR)"
   $(MTL) /nologo /D "NDEBUG" /tlb "$(OUTDIR)/button.tlb" /win32 $(SOURCE)


!ELSEIF  "$(CFG)" == "Button - Win32 Debug"


"$(OUTDIR)\button.tlb" : $(SOURCE) "$(OUTDIR)"
   $(MTL) /nologo /D "_DEBUG" /tlb "$(OUTDIR)/button.tlb" /win32 $(SOURCE)


!ELSEIF  "$(CFG)" == "Button - Win32 Unicode Debug"


"$(OUTDIR)\button.tlb" : $(SOURCE) "$(OUTDIR)"
   $(MTL) /nologo /D "_DEBUG" /tlb "$(OUTDIR)/button.tlb" /win32 $(SOURCE)


!ELSEIF  "$(CFG)" == "Button - Win32 Unicode Release"


"$(OUTDIR)\button.tlb" : $(SOURCE) "$(OUTDIR)"
   $(MTL) /nologo /D "NDEBUG" /tlb "$(OUTDIR)/button.tlb" /win32 $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\btnctl.cpp

!IF  "$(CFG)" == "Button - Win32 Release"

DEP_CPP_BTNCT=\
	".\btnctl.h"\
	".\btnppg.h"\
	".\button.h"\
	".\stdafx.h"\
	

"$(INTDIR)\btnctl.obj" : $(SOURCE) $(DEP_CPP_BTNCT) "$(INTDIR)"\
 "$(INTDIR)\button.pch"


!ELSEIF  "$(CFG)" == "Button - Win32 Debug"

DEP_CPP_BTNCT=\
	".\stdafx.h"\
	".\button.h"\
	".\btnctl.h"\
	".\btnppg.h"\
	".\resource.h"\
	

"$(INTDIR)\btnctl.obj" : $(SOURCE) $(DEP_CPP_BTNCT) "$(INTDIR)"\
 "$(INTDIR)\button.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_BTNCT) "$(INTDIR)"\
 "$(INTDIR)\button.pch"


!ELSEIF  "$(CFG)" == "Button - Win32 Unicode Debug"

DEP_CPP_BTNCT=\
	".\stdafx.h"\
	".\button.h"\
	".\btnctl.h"\
	".\btnppg.h"\
	".\resource.h"\
	

"$(INTDIR)\btnctl.obj" : $(SOURCE) $(DEP_CPP_BTNCT) "$(INTDIR)"\
 "$(INTDIR)\button.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_BTNCT) "$(INTDIR)"\
 "$(INTDIR)\button.pch"


!ELSEIF  "$(CFG)" == "Button - Win32 Unicode Release"

DEP_CPP_BTNCT=\
	".\stdafx.h"\
	".\button.h"\
	".\btnctl.h"\
	".\btnppg.h"\
	".\resource.h"\
	

"$(INTDIR)\btnctl.obj" : $(SOURCE) $(DEP_CPP_BTNCT) "$(INTDIR)"\
 "$(INTDIR)\button.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\btnppg.cpp

!IF  "$(CFG)" == "Button - Win32 Release"

DEP_CPP_BTNPP=\
	".\btnppg.h"\
	".\button.h"\
	".\stdafx.h"\
	

"$(INTDIR)\btnppg.obj" : $(SOURCE) $(DEP_CPP_BTNPP) "$(INTDIR)"\
 "$(INTDIR)\button.pch"


!ELSEIF  "$(CFG)" == "Button - Win32 Debug"

DEP_CPP_BTNPP=\
	".\stdafx.h"\
	".\button.h"\
	".\btnppg.h"\
	".\resource.h"\
	

"$(INTDIR)\btnppg.obj" : $(SOURCE) $(DEP_CPP_BTNPP) "$(INTDIR)"\
 "$(INTDIR)\button.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_BTNPP) "$(INTDIR)"\
 "$(INTDIR)\button.pch"


!ELSEIF  "$(CFG)" == "Button - Win32 Unicode Debug"

DEP_CPP_BTNPP=\
	".\stdafx.h"\
	".\button.h"\
	".\btnppg.h"\
	".\resource.h"\
	

"$(INTDIR)\btnppg.obj" : $(SOURCE) $(DEP_CPP_BTNPP) "$(INTDIR)"\
 "$(INTDIR)\button.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_BTNPP) "$(INTDIR)"\
 "$(INTDIR)\button.pch"


!ELSEIF  "$(CFG)" == "Button - Win32 Unicode Release"

DEP_CPP_BTNPP=\
	".\stdafx.h"\
	".\button.h"\
	".\btnppg.h"\
	".\resource.h"\
	

"$(INTDIR)\btnppg.obj" : $(SOURCE) $(DEP_CPP_BTNPP) "$(INTDIR)"\
 "$(INTDIR)\button.pch"


!ENDIF 

# End Source File
# End Target
# End Project
################################################################################
