# Microsoft Visual C++ Generated NMAKE File, Format Version 40000
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

!IF "$(CFG)" == ""
CFG=XList - Win32 Debug
!MESSAGE No configuration specified.  Defaulting to XList - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "XList - Win32 Release" && "$(CFG)" != "XList - Win32 Debug" &&\
 "$(CFG)" != "XList - Win32 Unicode Debug" && "$(CFG)" !=\
 "XList - Win32 Unicode Release"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "xlist.mak" CFG="XList - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "XList - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "XList - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "XList - Win32 Unicode Debug" (based on\
 "Win32 (x86) Dynamic-Link Library")
!MESSAGE "XList - Win32 Unicode Release" (based on\
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
# PROP Target_Last_Scanned "XList - Win32 Unicode Release"
CPP=cl.exe
RSC=rc.exe
MTL=mktyplib.exe

!IF  "$(CFG)" == "XList - Win32 Release"

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

ALL : "$(OUTDIR)\xlist.ocx" "$(OUTDIR)\regsvr32.trg"

CLEAN : 
	-@erase ".\Release\xlist.lib"
	-@erase ".\Release\xlist.obj"
	-@erase ".\Release\xlist.pch"
	-@erase ".\Release\xlistctl.obj"
	-@erase ".\Release\stdafx.obj"
	-@erase ".\Release\xlistppg.obj"
	-@erase ".\Release\xlist.res"
	-@erase ".\Release\xlist.tlb"
	-@erase ".\Release\xlist.exp"
	-@erase ".\Release\regsvr32.trg"
	-@erase ".\Release\xlist.ocx"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
CPP_PROJ=/nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/xlist.pch" /Yu"stdafx.h" /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\Release/
CPP_SBRS=
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/xlist.res" /d "NDEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/xlist.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 /nologo /subsystem:windows /dll /machine:I386
LINK32_FLAGS=/nologo /subsystem:windows /dll /incremental:no\
 /pdb:"$(OUTDIR)/xlist.pdb" /machine:I386 /def:".\xlist.def"\
 /out:"$(OUTDIR)/xlist.ocx" /implib:"$(OUTDIR)/xlist.lib" 
DEF_FILE= \
	".\xlist.def"
LINK32_OBJS= \
	"$(INTDIR)/xlist.obj" \
	"$(INTDIR)/xlistctl.obj" \
	"$(INTDIR)/stdafx.obj" \
	"$(INTDIR)/xlistppg.obj" \
	"$(INTDIR)/xlist.res"

"$(OUTDIR)\xlist.ocx" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

# Begin Custom Build - Registering OLE control...
OutDir=.\Release
TargetPath=.\Release\xlist.ocx
InputPath=.\Release\xlist.ocx
SOURCE=$(InputPath)

"$(OutDir)\regsvr32.trg" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   regsvr32 /s /c "$(TargetPath)"
   echo regsvr32 exec. time > "$(OutDir)\regsvr32.trg"

# End Custom Build

!ELSEIF  "$(CFG)" == "XList - Win32 Debug"

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

ALL : "$(OUTDIR)\xlist.ocx" "$(OUTDIR)\regsvr32.trg"

CLEAN : 
	-@erase ".\Debug\vc40.pdb"
	-@erase ".\Debug\xlist.pch"
	-@erase ".\Debug\xlist.ilk"
	-@erase ".\Debug\xlist.obj"
	-@erase ".\Debug\xlistctl.obj"
	-@erase ".\Debug\stdafx.obj"
	-@erase ".\Debug\xlistppg.obj"
	-@erase ".\Debug\xlist.res"
	-@erase ".\Debug\xlist.tlb"
	-@erase ".\Debug\xlist.lib"
	-@erase ".\Debug\xlist.exp"
	-@erase ".\Debug\xlist.pdb"
	-@erase ".\Debug\regsvr32.trg"
	-@erase ".\Debug\xlist.ocx"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
CPP_PROJ=/nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/xlist.pch" /Yu"stdafx.h" /Fo"$(INTDIR)/"\
 /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\Debug/
CPP_SBRS=
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/xlist.res" /d "_DEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/xlist.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /debug /machine:I386
# ADD LINK32 /nologo /subsystem:windows /dll /debug /machine:I386
LINK32_FLAGS=/nologo /subsystem:windows /dll /incremental:yes\
 /pdb:"$(OUTDIR)/xlist.pdb" /debug /machine:I386 /def:".\xlist.def"\
 /out:"$(OUTDIR)/xlist.ocx" /implib:"$(OUTDIR)/xlist.lib" 
DEF_FILE= \
	".\xlist.def"
LINK32_OBJS= \
	"$(INTDIR)/xlist.obj" \
	"$(INTDIR)/xlistctl.obj" \
	"$(INTDIR)/stdafx.obj" \
	"$(INTDIR)/xlistppg.obj" \
	"$(INTDIR)/xlist.res"

"$(OUTDIR)\xlist.ocx" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

# Begin Custom Build - Registering OLE control...
OutDir=.\Debug
TargetPath=.\Debug\xlist.ocx
InputPath=.\Debug\xlist.ocx
SOURCE=$(InputPath)

"$(OutDir)\regsvr32.trg" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   regsvr32 /s /c "$(TargetPath)"
   echo regsvr32 exec. time > "$(OutDir)\regsvr32.trg"

# End Custom Build

!ELSEIF  "$(CFG)" == "XList - Win32 Unicode Debug"

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

ALL : "$(OUTDIR)\xlist.ocx" "$(OUTDIR)\regsvr32.trg"

CLEAN : 
	-@erase ".\UniDebug\vc40.pdb"
	-@erase ".\UniDebug\xlist.pch"
	-@erase ".\UniDebug\xlist.ilk"
	-@erase ".\UniDebug\stdafx.obj"
	-@erase ".\UniDebug\xlistppg.obj"
	-@erase ".\UniDebug\xlistctl.obj"
	-@erase ".\UniDebug\xlist.obj"
	-@erase ".\UniDebug\xlist.res"
	-@erase ".\UniDebug\xlist.tlb"
	-@erase ".\UniDebug\xlist.lib"
	-@erase ".\UniDebug\xlist.exp"
	-@erase ".\UniDebug\xlist.pdb"
	-@erase ".\UniDebug\regsvr32.trg"
	-@erase ".\UniDebug\xlist.ocx"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_UNICODE" /Yu"stdafx.h" /c
CPP_PROJ=/nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /D "_AFXDLL" /D "_UNICODE" /Fp"$(INTDIR)/xlist.pch" /Yu"stdafx.h"\
 /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\UniDebug/
CPP_SBRS=
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/xlist.res" /d "_DEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/xlist.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /debug /machine:I386
# ADD LINK32 /nologo /subsystem:windows /dll /debug /machine:I386
LINK32_FLAGS=/nologo /subsystem:windows /dll /incremental:yes\
 /pdb:"$(OUTDIR)/xlist.pdb" /debug /machine:I386 /def:".\xlist.def"\
 /out:"$(OUTDIR)/xlist.ocx" /implib:"$(OUTDIR)/xlist.lib" 
DEF_FILE= \
	".\xlist.def"
LINK32_OBJS= \
	"$(INTDIR)/stdafx.obj" \
	"$(INTDIR)/xlistppg.obj" \
	"$(INTDIR)/xlistctl.obj" \
	"$(INTDIR)/xlist.obj" \
	"$(INTDIR)/xlist.res"

"$(OUTDIR)\xlist.ocx" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

# Begin Custom Build - Registering OLE control...
OutDir=.\UniDebug
TargetPath=.\UniDebug\xlist.ocx
InputPath=.\UniDebug\xlist.ocx
SOURCE=$(InputPath)

"$(OutDir)\regsvr32.trg" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   regsvr32 /s /c "$(TargetPath)"
   echo regsvr32 exec. time > "$(OutDir)\regsvr32.trg"

# End Custom Build

!ELSEIF  "$(CFG)" == "XList - Win32 Unicode Release"

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

ALL : "$(OUTDIR)\xlist.ocx" "$(OUTDIR)\regsvr32.trg"

CLEAN : 
	-@erase ".\UniRelease\xlist.lib"
	-@erase ".\UniRelease\xlistppg.obj"
	-@erase ".\UniRelease\xlist.pch"
	-@erase ".\UniRelease\stdafx.obj"
	-@erase ".\UniRelease\xlistctl.obj"
	-@erase ".\UniRelease\xlist.obj"
	-@erase ".\UniRelease\xlist.res"
	-@erase ".\UniRelease\xlist.tlb"
	-@erase ".\UniRelease\xlist.exp"
	-@erase ".\UniRelease\regsvr32.trg"
	-@erase ".\UniRelease\xlist.ocx"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_UNICODE" /Yu"stdafx.h" /c
CPP_PROJ=/nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_UNICODE" /Fp"$(INTDIR)/xlist.pch" /Yu"stdafx.h" /Fo"$(INTDIR)/"\
 /c 
CPP_OBJS=.\UniRelease/
CPP_SBRS=
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/xlist.res" /d "NDEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/xlist.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 /nologo /subsystem:windows /dll /machine:I386
LINK32_FLAGS=/nologo /subsystem:windows /dll /incremental:no\
 /pdb:"$(OUTDIR)/xlist.pdb" /machine:I386 /def:".\xlist.def"\
 /out:"$(OUTDIR)/xlist.ocx" /implib:"$(OUTDIR)/xlist.lib" 
DEF_FILE= \
	".\xlist.def"
LINK32_OBJS= \
	"$(INTDIR)/xlistppg.obj" \
	"$(INTDIR)/stdafx.obj" \
	"$(INTDIR)/xlistctl.obj" \
	"$(INTDIR)/xlist.obj" \
	"$(INTDIR)/xlist.res"

"$(OUTDIR)\xlist.ocx" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

# Begin Custom Build - Registering OLE control...
OutDir=.\UniRelease
TargetPath=.\UniRelease\xlist.ocx
InputPath=.\UniRelease\xlist.ocx
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

# Name "XList - Win32 Release"
# Name "XList - Win32 Debug"
# Name "XList - Win32 Unicode Debug"
# Name "XList - Win32 Unicode Release"

!IF  "$(CFG)" == "XList - Win32 Release"

!ELSEIF  "$(CFG)" == "XList - Win32 Debug"

!ELSEIF  "$(CFG)" == "XList - Win32 Unicode Debug"

!ELSEIF  "$(CFG)" == "XList - Win32 Unicode Release"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\ReadMe.txt

!IF  "$(CFG)" == "XList - Win32 Release"

!ELSEIF  "$(CFG)" == "XList - Win32 Debug"

!ELSEIF  "$(CFG)" == "XList - Win32 Unicode Debug"

!ELSEIF  "$(CFG)" == "XList - Win32 Unicode Release"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\stdafx.cpp
DEP_CPP_STDAF=\
	".\stdafx.h"\
	

!IF  "$(CFG)" == "XList - Win32 Release"

# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/xlist.pch" /Yc"stdafx.h" /Fo"$(INTDIR)/" /c\
 $(SOURCE) \
	

"$(INTDIR)\stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\xlist.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "XList - Win32 Debug"

# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/xlist.pch" /Yc"stdafx.h" /Fo"$(INTDIR)/"\
 /Fd"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\xlist.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "XList - Win32 Unicode Debug"

# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /D "_AFXDLL" /D "_UNICODE" /Fp"$(INTDIR)/xlist.pch" /Yc"stdafx.h"\
 /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\xlist.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "XList - Win32 Unicode Release"

# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_UNICODE" /Fp"$(INTDIR)/xlist.pch" /Yc"stdafx.h" /Fo"$(INTDIR)/"\
 /c $(SOURCE) \
	

"$(INTDIR)\stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\xlist.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\xlist.cpp
DEP_CPP_XLIST=\
	".\stdafx.h"\
	".\xlist.h"\
	".\resource.h"\
	

!IF  "$(CFG)" == "XList - Win32 Release"


"$(INTDIR)\xlist.obj" : $(SOURCE) $(DEP_CPP_XLIST) "$(INTDIR)"\
 "$(INTDIR)\xlist.pch"


!ELSEIF  "$(CFG)" == "XList - Win32 Debug"


"$(INTDIR)\xlist.obj" : $(SOURCE) $(DEP_CPP_XLIST) "$(INTDIR)"\
 "$(INTDIR)\xlist.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_XLIST) "$(INTDIR)"\
 "$(INTDIR)\xlist.pch"


!ELSEIF  "$(CFG)" == "XList - Win32 Unicode Debug"


"$(INTDIR)\xlist.obj" : $(SOURCE) $(DEP_CPP_XLIST) "$(INTDIR)"\
 "$(INTDIR)\xlist.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_XLIST) "$(INTDIR)"\
 "$(INTDIR)\xlist.pch"


!ELSEIF  "$(CFG)" == "XList - Win32 Unicode Release"


"$(INTDIR)\xlist.obj" : $(SOURCE) $(DEP_CPP_XLIST) "$(INTDIR)"\
 "$(INTDIR)\xlist.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\xlist.def

!IF  "$(CFG)" == "XList - Win32 Release"

!ELSEIF  "$(CFG)" == "XList - Win32 Debug"

!ELSEIF  "$(CFG)" == "XList - Win32 Unicode Debug"

!ELSEIF  "$(CFG)" == "XList - Win32 Unicode Release"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\xlist.rc

!IF  "$(CFG)" == "XList - Win32 Release"

DEP_RSC_XLIST_=\
	".\xlist.ico"\
	".\xlistctl.bmp"\
	".\resource.h"\
	".\xlist.rc2"\
	
NODEP_RSC_XLIST_=\
	".\Release\xlist.tlb"\
	

"$(INTDIR)\xlist.res" : $(SOURCE) $(DEP_RSC_XLIST_) "$(INTDIR)"\
 "$(INTDIR)\XList.tlb"
   $(RSC) /l 0x409 /fo"$(INTDIR)/xlist.res" /i "Release" /d "NDEBUG" /d\
 "_AFXDLL" $(SOURCE)


!ELSEIF  "$(CFG)" == "XList - Win32 Debug"

DEP_RSC_XLIST_=\
	".\xlist.ico"\
	".\xlistctl.bmp"\
	".\resource.h"\
	".\xlist.rc2"\
	".\Debug\xlist.tlb"\
	

"$(INTDIR)\xlist.res" : $(SOURCE) $(DEP_RSC_XLIST_) "$(INTDIR)"\
 "$(INTDIR)\Xlist.tlb"
   $(RSC) /l 0x409 /fo"$(INTDIR)/xlist.res" /i "Debug" /d "_DEBUG" /d "_AFXDLL"\
 $(SOURCE)


!ELSEIF  "$(CFG)" == "XList - Win32 Unicode Debug"

DEP_RSC_XLIST_=\
	".\xlist.ico"\
	".\xlistctl.bmp"\
	".\resource.h"\
	".\xlist.rc2"\
	
NODEP_RSC_XLIST_=\
	".\UniDebug\xlist.tlb"\
	

"$(INTDIR)\xlist.res" : $(SOURCE) $(DEP_RSC_XLIST_) "$(INTDIR)"\
 "$(INTDIR)\XList.tlb"
   $(RSC) /l 0x409 /fo"$(INTDIR)/xlist.res" /i "UniDebug" /d "_DEBUG" /d\
 "_AFXDLL" $(SOURCE)


!ELSEIF  "$(CFG)" == "XList - Win32 Unicode Release"

DEP_RSC_XLIST_=\
	".\xlist.ico"\
	".\xlistctl.bmp"\
	".\resource.h"\
	".\xlist.rc2"\
	
NODEP_RSC_XLIST_=\
	".\UniRelease\xlist.tlb"\
	

"$(INTDIR)\xlist.res" : $(SOURCE) $(DEP_RSC_XLIST_) "$(INTDIR)"\
 "$(INTDIR)\XList.tlb"
   $(RSC) /l 0x409 /fo"$(INTDIR)/xlist.res" /i "UniRelease" /d "NDEBUG" /d\
 "_AFXDLL" $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\xlist.odl

!IF  "$(CFG)" == "XList - Win32 Release"


"$(OUTDIR)\XList.tlb" : $(SOURCE) "$(OUTDIR)"
   $(MTL) /nologo /D "NDEBUG" /tlb "$(OUTDIR)/xlist.tlb" /win32 $(SOURCE)


!ELSEIF  "$(CFG)" == "XList - Win32 Debug"


"$(OUTDIR)\Xlist.tlb" : $(SOURCE) "$(OUTDIR)"
   $(MTL) /nologo /D "_DEBUG" /tlb "$(OUTDIR)/xlist.tlb" /win32 $(SOURCE)


!ELSEIF  "$(CFG)" == "XList - Win32 Unicode Debug"


"$(OUTDIR)\XList.tlb" : $(SOURCE) "$(OUTDIR)"
   $(MTL) /nologo /D "_DEBUG" /tlb "$(OUTDIR)/xlist.tlb" /win32 $(SOURCE)


!ELSEIF  "$(CFG)" == "XList - Win32 Unicode Release"


"$(OUTDIR)\XList.tlb" : $(SOURCE) "$(OUTDIR)"
   $(MTL) /nologo /D "NDEBUG" /tlb "$(OUTDIR)/xlist.tlb" /win32 $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\xlistctl.cpp
DEP_CPP_XLISTC=\
	".\stdafx.h"\
	".\xlist.h"\
	".\xlistctl.h"\
	".\xlistppg.h"\
	".\resource.h"\
	

!IF  "$(CFG)" == "XList - Win32 Release"


"$(INTDIR)\xlistctl.obj" : $(SOURCE) $(DEP_CPP_XLISTC) "$(INTDIR)"\
 "$(INTDIR)\xlist.pch"


!ELSEIF  "$(CFG)" == "XList - Win32 Debug"


"$(INTDIR)\xlistctl.obj" : $(SOURCE) $(DEP_CPP_XLISTC) "$(INTDIR)"\
 "$(INTDIR)\xlist.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_XLISTC) "$(INTDIR)"\
 "$(INTDIR)\xlist.pch"


!ELSEIF  "$(CFG)" == "XList - Win32 Unicode Debug"


"$(INTDIR)\xlistctl.obj" : $(SOURCE) $(DEP_CPP_XLISTC) "$(INTDIR)"\
 "$(INTDIR)\xlist.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_XLISTC) "$(INTDIR)"\
 "$(INTDIR)\xlist.pch"


!ELSEIF  "$(CFG)" == "XList - Win32 Unicode Release"


"$(INTDIR)\xlistctl.obj" : $(SOURCE) $(DEP_CPP_XLISTC) "$(INTDIR)"\
 "$(INTDIR)\xlist.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\xlistppg.cpp
DEP_CPP_XLISTP=\
	".\stdafx.h"\
	".\xlist.h"\
	".\xlistppg.h"\
	".\resource.h"\
	

!IF  "$(CFG)" == "XList - Win32 Release"


"$(INTDIR)\xlistppg.obj" : $(SOURCE) $(DEP_CPP_XLISTP) "$(INTDIR)"\
 "$(INTDIR)\xlist.pch"


!ELSEIF  "$(CFG)" == "XList - Win32 Debug"


"$(INTDIR)\xlistppg.obj" : $(SOURCE) $(DEP_CPP_XLISTP) "$(INTDIR)"\
 "$(INTDIR)\xlist.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_XLISTP) "$(INTDIR)"\
 "$(INTDIR)\xlist.pch"


!ELSEIF  "$(CFG)" == "XList - Win32 Unicode Debug"


"$(INTDIR)\xlistppg.obj" : $(SOURCE) $(DEP_CPP_XLISTP) "$(INTDIR)"\
 "$(INTDIR)\xlist.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_XLISTP) "$(INTDIR)"\
 "$(INTDIR)\xlist.pch"


!ELSEIF  "$(CFG)" == "XList - Win32 Unicode Release"


"$(INTDIR)\xlistppg.obj" : $(SOURCE) $(DEP_CPP_XLISTP) "$(INTDIR)"\
 "$(INTDIR)\xlist.pch"


!ENDIF 

# End Source File
# End Target
# End Project
################################################################################
