# Microsoft Developer Studio Generated NMAKE File, Format Version 4.20
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

!IF "$(CFG)" == ""
CFG=Snap - Win32 Unicode Debug
!MESSAGE No configuration specified.  Defaulting to Snap - Win32 Unicode Debug.
!ENDIF 

!IF "$(CFG)" != "Snap - Win32 Release" && "$(CFG)" != "Snap - Win32 Debug" &&\
 "$(CFG)" != "Snap - Win32 Unicode Release" && "$(CFG)" !=\
 "Snap - Win32 Unicode Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "snapvw.mak" CFG="Snap - Win32 Unicode Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Snap - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "Snap - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE "Snap - Win32 Unicode Release" (based on "Win32 (x86) Application")
!MESSAGE "Snap - Win32 Unicode Debug" (based on "Win32 (x86) Application")
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
# PROP Target_Last_Scanned "Snap - Win32 Unicode Debug"
CPP=cl.exe
RSC=rc.exe
MTL=mktyplib.exe

!IF  "$(CFG)" == "Snap - Win32 Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
OUTDIR=.\Release
INTDIR=.\Release

ALL : "$(OUTDIR)\snapvw.exe"

CLEAN : 
	-@erase "$(INTDIR)\BkfstPg.obj"
	-@erase "$(INTDIR)\ChildFrm.obj"
	-@erase "$(INTDIR)\DinnerPg.obj"
	-@erase "$(INTDIR)\LunchPg.obj"
	-@erase "$(INTDIR)\MainFrm.obj"
	-@erase "$(INTDIR)\SnapDoc.obj"
	-@erase "$(INTDIR)\SnapView.obj"
	-@erase "$(INTDIR)\snapvw.obj"
	-@erase "$(INTDIR)\snapvw.pch"
	-@erase "$(INTDIR)\snapvw.res"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(OUTDIR)\snapvw.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
CPP_PROJ=/nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/snapvw.pch" /Yu"stdafx.h" /Fo"$(INTDIR)/" /c\
 
CPP_OBJS=.\Release/
CPP_SBRS=.\.
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/snapvw.res" /d "NDEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/snapvw.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 /nologo /subsystem:windows /machine:I386
LINK32_FLAGS=/nologo /subsystem:windows /incremental:no\
 /pdb:"$(OUTDIR)/snapvw.pdb" /machine:I386 /out:"$(OUTDIR)/snapvw.exe" 
LINK32_OBJS= \
	"$(INTDIR)\BkfstPg.obj" \
	"$(INTDIR)\ChildFrm.obj" \
	"$(INTDIR)\DinnerPg.obj" \
	"$(INTDIR)\LunchPg.obj" \
	"$(INTDIR)\MainFrm.obj" \
	"$(INTDIR)\SnapDoc.obj" \
	"$(INTDIR)\SnapView.obj" \
	"$(INTDIR)\snapvw.obj" \
	"$(INTDIR)\snapvw.res" \
	"$(INTDIR)\StdAfx.obj"

"$(OUTDIR)\snapvw.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "Snap - Win32 Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
OUTDIR=.\Debug
INTDIR=.\Debug

ALL : "$(OUTDIR)\snapvw.exe"

CLEAN : 
	-@erase "$(INTDIR)\BkfstPg.obj"
	-@erase "$(INTDIR)\ChildFrm.obj"
	-@erase "$(INTDIR)\DinnerPg.obj"
	-@erase "$(INTDIR)\LunchPg.obj"
	-@erase "$(INTDIR)\MainFrm.obj"
	-@erase "$(INTDIR)\SnapDoc.obj"
	-@erase "$(INTDIR)\SnapView.obj"
	-@erase "$(INTDIR)\snapvw.obj"
	-@erase "$(INTDIR)\snapvw.pch"
	-@erase "$(INTDIR)\snapvw.res"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\vc40.idb"
	-@erase "$(INTDIR)\vc40.pdb"
	-@erase "$(OUTDIR)\snapvw.exe"
	-@erase "$(OUTDIR)\snapvw.ilk"
	-@erase "$(OUTDIR)\snapvw.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
CPP_PROJ=/nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/snapvw.pch" /Yu"stdafx.h" /Fo"$(INTDIR)/"\
 /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\Debug/
CPP_SBRS=.\.
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/snapvw.res" /d "_DEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/snapvw.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386
# ADD LINK32 /nologo /subsystem:windows /debug /machine:I386
LINK32_FLAGS=/nologo /subsystem:windows /incremental:yes\
 /pdb:"$(OUTDIR)/snapvw.pdb" /debug /machine:I386 /out:"$(OUTDIR)/snapvw.exe" 
LINK32_OBJS= \
	"$(INTDIR)\BkfstPg.obj" \
	"$(INTDIR)\ChildFrm.obj" \
	"$(INTDIR)\DinnerPg.obj" \
	"$(INTDIR)\LunchPg.obj" \
	"$(INTDIR)\MainFrm.obj" \
	"$(INTDIR)\SnapDoc.obj" \
	"$(INTDIR)\SnapView.obj" \
	"$(INTDIR)\snapvw.obj" \
	"$(INTDIR)\snapvw.res" \
	"$(INTDIR)\StdAfx.obj"

"$(OUTDIR)\snapvw.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "Snap - Win32 Unicode Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Snap___W"
# PROP BASE Intermediate_Dir "Snap___W"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Snap___W"
# PROP Intermediate_Dir "Snap___W"
# PROP Target_Dir ""
OUTDIR=.\Snap___W
INTDIR=.\Snap___W

ALL : ".\UniRel\SnapVw.exe"

CLEAN : 
	-@erase "$(INTDIR)\BkfstPg.obj"
	-@erase "$(INTDIR)\ChildFrm.obj"
	-@erase "$(INTDIR)\DinnerPg.obj"
	-@erase "$(INTDIR)\LunchPg.obj"
	-@erase "$(INTDIR)\MainFrm.obj"
	-@erase "$(INTDIR)\SnapDoc.obj"
	-@erase "$(INTDIR)\SnapView.obj"
	-@erase "$(INTDIR)\snapvw.obj"
	-@erase "$(INTDIR)\snapvw.pch"
	-@erase "$(INTDIR)\snapvw.res"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase ".\UniRel\SnapVw.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /D "_UNICODE" /Yu"stdafx.h" /c
CPP_PROJ=/nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /D "_UNICODE" /Fp"$(INTDIR)/snapvw.pch" /Yu"stdafx.h"\
 /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\Snap___W/
CPP_SBRS=.\.
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/snapvw.res" /d "NDEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/snapvw.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 /nologo /entry:"wWinMainCRTStartup" /subsystem:windows /pdb:"UniRel/SnapVw.pdb" /machine:I386 /out:"UniRel/SnapVw.exe"
# SUBTRACT LINK32 /pdb:none
LINK32_FLAGS=/nologo /entry:"wWinMainCRTStartup" /subsystem:windows\
 /incremental:no /pdb:"UniRel/SnapVw.pdb" /machine:I386 /out:"UniRel/SnapVw.exe"\
 
LINK32_OBJS= \
	"$(INTDIR)\BkfstPg.obj" \
	"$(INTDIR)\ChildFrm.obj" \
	"$(INTDIR)\DinnerPg.obj" \
	"$(INTDIR)\LunchPg.obj" \
	"$(INTDIR)\MainFrm.obj" \
	"$(INTDIR)\SnapDoc.obj" \
	"$(INTDIR)\SnapView.obj" \
	"$(INTDIR)\snapvw.obj" \
	"$(INTDIR)\snapvw.res" \
	"$(INTDIR)\StdAfx.obj"

".\UniRel\SnapVw.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "Snap - Win32 Unicode Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Snap___0"
# PROP BASE Intermediate_Dir "Snap___0"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Snap___0"
# PROP Intermediate_Dir "Snap___0"
# PROP Target_Dir ""
OUTDIR=.\Snap___0
INTDIR=.\Snap___0

ALL : ".\UniDebug\SnapVw.exe"

CLEAN : 
	-@erase "$(INTDIR)\BkfstPg.obj"
	-@erase "$(INTDIR)\ChildFrm.obj"
	-@erase "$(INTDIR)\DinnerPg.obj"
	-@erase "$(INTDIR)\LunchPg.obj"
	-@erase "$(INTDIR)\MainFrm.obj"
	-@erase "$(INTDIR)\SnapDoc.obj"
	-@erase "$(INTDIR)\SnapView.obj"
	-@erase "$(INTDIR)\snapvw.obj"
	-@erase "$(INTDIR)\snapvw.pch"
	-@erase "$(INTDIR)\snapvw.res"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\vc40.idb"
	-@erase "$(INTDIR)\vc40.pdb"
	-@erase ".\UniDebug\SnapVw.exe"
	-@erase ".\UniDebug\SnapVw.ilk"
	-@erase ".\UniDebug\SnapVw.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /D "_UNICODE" /Yu"stdafx.h" /c
CPP_PROJ=/nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /D "_AFXDLL" /D "_MBCS" /D "_UNICODE" /Fp"$(INTDIR)/snapvw.pch" /Yu"stdafx.h"\
 /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\Snap___0/
CPP_SBRS=.\.
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/snapvw.res" /d "_DEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/snapvw.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386
# ADD LINK32 /nologo /entry:"wWinMainCRTStartup" /subsystem:windows /pdb:"UniDebug/SnapVw.pdb" /debug /machine:I386 /out:"UniDebug/SnapVw.exe"
# SUBTRACT LINK32 /pdb:none
LINK32_FLAGS=/nologo /entry:"wWinMainCRTStartup" /subsystem:windows\
 /incremental:yes /pdb:"UniDebug/SnapVw.pdb" /debug /machine:I386\
 /out:"UniDebug/SnapVw.exe" 
LINK32_OBJS= \
	"$(INTDIR)\BkfstPg.obj" \
	"$(INTDIR)\ChildFrm.obj" \
	"$(INTDIR)\DinnerPg.obj" \
	"$(INTDIR)\LunchPg.obj" \
	"$(INTDIR)\MainFrm.obj" \
	"$(INTDIR)\SnapDoc.obj" \
	"$(INTDIR)\SnapView.obj" \
	"$(INTDIR)\snapvw.obj" \
	"$(INTDIR)\snapvw.res" \
	"$(INTDIR)\StdAfx.obj"

".\UniDebug\SnapVw.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

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

# Name "Snap - Win32 Release"
# Name "Snap - Win32 Debug"
# Name "Snap - Win32 Unicode Release"
# Name "Snap - Win32 Unicode Debug"

!IF  "$(CFG)" == "Snap - Win32 Release"

!ELSEIF  "$(CFG)" == "Snap - Win32 Debug"

!ELSEIF  "$(CFG)" == "Snap - Win32 Unicode Release"

!ELSEIF  "$(CFG)" == "Snap - Win32 Unicode Debug"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\StdAfx.cpp
DEP_CPP_STDAF=\
	".\StdAfx.h"\
	

!IF  "$(CFG)" == "Snap - Win32 Release"

# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/snapvw.pch" /Yc"stdafx.h" /Fo"$(INTDIR)/" /c\
 $(SOURCE) \
	

"$(INTDIR)\StdAfx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\snapvw.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "Snap - Win32 Debug"

# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/snapvw.pch" /Yc"stdafx.h" /Fo"$(INTDIR)/"\
 /Fd"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\StdAfx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\snapvw.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "Snap - Win32 Unicode Release"

# ADD BASE CPP /Yc"stdafx.h"
# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /D "_UNICODE" /Fp"$(INTDIR)/snapvw.pch" /Yc"stdafx.h"\
 /Fo"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\StdAfx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\snapvw.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "Snap - Win32 Unicode Debug"

# ADD BASE CPP /Yc"stdafx.h"
# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /D "_AFXDLL" /D "_MBCS" /D "_UNICODE" /Fp"$(INTDIR)/snapvw.pch" /Yc"stdafx.h"\
 /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\StdAfx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\snapvw.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\MainFrm.cpp
DEP_CPP_MAINF=\
	".\mainfrm.h"\
	".\snapvw.h"\
	".\StdAfx.h"\
	

!IF  "$(CFG)" == "Snap - Win32 Release"


"$(INTDIR)\MainFrm.obj" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\snapvw.pch"


!ELSEIF  "$(CFG)" == "Snap - Win32 Debug"


"$(INTDIR)\MainFrm.obj" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\snapvw.pch"


!ELSEIF  "$(CFG)" == "Snap - Win32 Unicode Release"


"$(INTDIR)\MainFrm.obj" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\snapvw.pch"


!ELSEIF  "$(CFG)" == "Snap - Win32 Unicode Debug"


"$(INTDIR)\MainFrm.obj" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\snapvw.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\ChildFrm.cpp
DEP_CPP_CHILD=\
	".\childfrm.h"\
	".\snapvw.h"\
	".\StdAfx.h"\
	

!IF  "$(CFG)" == "Snap - Win32 Release"


"$(INTDIR)\ChildFrm.obj" : $(SOURCE) $(DEP_CPP_CHILD) "$(INTDIR)"\
 "$(INTDIR)\snapvw.pch"


!ELSEIF  "$(CFG)" == "Snap - Win32 Debug"


"$(INTDIR)\ChildFrm.obj" : $(SOURCE) $(DEP_CPP_CHILD) "$(INTDIR)"\
 "$(INTDIR)\snapvw.pch"


!ELSEIF  "$(CFG)" == "Snap - Win32 Unicode Release"


"$(INTDIR)\ChildFrm.obj" : $(SOURCE) $(DEP_CPP_CHILD) "$(INTDIR)"\
 "$(INTDIR)\snapvw.pch"


!ELSEIF  "$(CFG)" == "Snap - Win32 Unicode Debug"


"$(INTDIR)\ChildFrm.obj" : $(SOURCE) $(DEP_CPP_CHILD) "$(INTDIR)"\
 "$(INTDIR)\snapvw.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\SnapDoc.cpp
DEP_CPP_SNAPD=\
	".\snapdoc.h"\
	".\snapvw.h"\
	".\StdAfx.h"\
	

!IF  "$(CFG)" == "Snap - Win32 Release"


"$(INTDIR)\SnapDoc.obj" : $(SOURCE) $(DEP_CPP_SNAPD) "$(INTDIR)"\
 "$(INTDIR)\snapvw.pch"


!ELSEIF  "$(CFG)" == "Snap - Win32 Debug"


"$(INTDIR)\SnapDoc.obj" : $(SOURCE) $(DEP_CPP_SNAPD) "$(INTDIR)"\
 "$(INTDIR)\snapvw.pch"


!ELSEIF  "$(CFG)" == "Snap - Win32 Unicode Release"


"$(INTDIR)\SnapDoc.obj" : $(SOURCE) $(DEP_CPP_SNAPD) "$(INTDIR)"\
 "$(INTDIR)\snapvw.pch"


!ELSEIF  "$(CFG)" == "Snap - Win32 Unicode Debug"


"$(INTDIR)\SnapDoc.obj" : $(SOURCE) $(DEP_CPP_SNAPD) "$(INTDIR)"\
 "$(INTDIR)\snapvw.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\SnapView.cpp
DEP_CPP_SNAPV=\
	".\BkfstPg.h"\
	".\DinnerPg.h"\
	".\LunchPg.h"\
	".\snapdoc.h"\
	".\snapview.h"\
	".\snapvw.h"\
	".\StdAfx.h"\
	

!IF  "$(CFG)" == "Snap - Win32 Release"


"$(INTDIR)\SnapView.obj" : $(SOURCE) $(DEP_CPP_SNAPV) "$(INTDIR)"\
 "$(INTDIR)\snapvw.pch"


!ELSEIF  "$(CFG)" == "Snap - Win32 Debug"


"$(INTDIR)\SnapView.obj" : $(SOURCE) $(DEP_CPP_SNAPV) "$(INTDIR)"\
 "$(INTDIR)\snapvw.pch"


!ELSEIF  "$(CFG)" == "Snap - Win32 Unicode Release"


"$(INTDIR)\SnapView.obj" : $(SOURCE) $(DEP_CPP_SNAPV) "$(INTDIR)"\
 "$(INTDIR)\snapvw.pch"


!ELSEIF  "$(CFG)" == "Snap - Win32 Unicode Debug"


"$(INTDIR)\SnapView.obj" : $(SOURCE) $(DEP_CPP_SNAPV) "$(INTDIR)"\
 "$(INTDIR)\snapvw.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\snapvw.cpp
DEP_CPP_SNAPVW=\
	".\childfrm.h"\
	".\mainfrm.h"\
	".\snapdoc.h"\
	".\snapview.h"\
	".\snapvw.h"\
	".\StdAfx.h"\
	

!IF  "$(CFG)" == "Snap - Win32 Release"


"$(INTDIR)\snapvw.obj" : $(SOURCE) $(DEP_CPP_SNAPVW) "$(INTDIR)"\
 "$(INTDIR)\snapvw.pch"


!ELSEIF  "$(CFG)" == "Snap - Win32 Debug"


"$(INTDIR)\snapvw.obj" : $(SOURCE) $(DEP_CPP_SNAPVW) "$(INTDIR)"\
 "$(INTDIR)\snapvw.pch"


!ELSEIF  "$(CFG)" == "Snap - Win32 Unicode Release"


"$(INTDIR)\snapvw.obj" : $(SOURCE) $(DEP_CPP_SNAPVW) "$(INTDIR)"\
 "$(INTDIR)\snapvw.pch"


!ELSEIF  "$(CFG)" == "Snap - Win32 Unicode Debug"


"$(INTDIR)\snapvw.obj" : $(SOURCE) $(DEP_CPP_SNAPVW) "$(INTDIR)"\
 "$(INTDIR)\snapvw.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\snapvw.rc
DEP_RSC_SNAPVW_=\
	".\RES\SNAP.ICO"\
	".\RES\SNAP.RC2"\
	".\RES\SNAPDOC.ICO"\
	".\RES\Toolbar.bmp"\
	

!IF  "$(CFG)" == "Snap - Win32 Release"


"$(INTDIR)\snapvw.res" : $(SOURCE) $(DEP_RSC_SNAPVW_) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Snap - Win32 Debug"


"$(INTDIR)\snapvw.res" : $(SOURCE) $(DEP_RSC_SNAPVW_) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Snap - Win32 Unicode Release"


"$(INTDIR)\snapvw.res" : $(SOURCE) $(DEP_RSC_SNAPVW_) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Snap - Win32 Unicode Debug"


"$(INTDIR)\snapvw.res" : $(SOURCE) $(DEP_RSC_SNAPVW_) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\BkfstPg.cpp
DEP_CPP_BKFST=\
	".\BkfstPg.h"\
	".\snapvw.h"\
	".\StdAfx.h"\
	

!IF  "$(CFG)" == "Snap - Win32 Release"


"$(INTDIR)\BkfstPg.obj" : $(SOURCE) $(DEP_CPP_BKFST) "$(INTDIR)"\
 "$(INTDIR)\snapvw.pch"


!ELSEIF  "$(CFG)" == "Snap - Win32 Debug"


"$(INTDIR)\BkfstPg.obj" : $(SOURCE) $(DEP_CPP_BKFST) "$(INTDIR)"\
 "$(INTDIR)\snapvw.pch"


!ELSEIF  "$(CFG)" == "Snap - Win32 Unicode Release"


"$(INTDIR)\BkfstPg.obj" : $(SOURCE) $(DEP_CPP_BKFST) "$(INTDIR)"\
 "$(INTDIR)\snapvw.pch"


!ELSEIF  "$(CFG)" == "Snap - Win32 Unicode Debug"


"$(INTDIR)\BkfstPg.obj" : $(SOURCE) $(DEP_CPP_BKFST) "$(INTDIR)"\
 "$(INTDIR)\snapvw.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\LunchPg.cpp
DEP_CPP_LUNCH=\
	".\LunchPg.h"\
	".\snapvw.h"\
	".\StdAfx.h"\
	

!IF  "$(CFG)" == "Snap - Win32 Release"


"$(INTDIR)\LunchPg.obj" : $(SOURCE) $(DEP_CPP_LUNCH) "$(INTDIR)"\
 "$(INTDIR)\snapvw.pch"


!ELSEIF  "$(CFG)" == "Snap - Win32 Debug"


"$(INTDIR)\LunchPg.obj" : $(SOURCE) $(DEP_CPP_LUNCH) "$(INTDIR)"\
 "$(INTDIR)\snapvw.pch"


!ELSEIF  "$(CFG)" == "Snap - Win32 Unicode Release"


"$(INTDIR)\LunchPg.obj" : $(SOURCE) $(DEP_CPP_LUNCH) "$(INTDIR)"\
 "$(INTDIR)\snapvw.pch"


!ELSEIF  "$(CFG)" == "Snap - Win32 Unicode Debug"


"$(INTDIR)\LunchPg.obj" : $(SOURCE) $(DEP_CPP_LUNCH) "$(INTDIR)"\
 "$(INTDIR)\snapvw.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\DinnerPg.cpp
DEP_CPP_DINNE=\
	".\DinnerPg.h"\
	".\snapvw.h"\
	".\StdAfx.h"\
	

!IF  "$(CFG)" == "Snap - Win32 Release"


"$(INTDIR)\DinnerPg.obj" : $(SOURCE) $(DEP_CPP_DINNE) "$(INTDIR)"\
 "$(INTDIR)\snapvw.pch"


!ELSEIF  "$(CFG)" == "Snap - Win32 Debug"


"$(INTDIR)\DinnerPg.obj" : $(SOURCE) $(DEP_CPP_DINNE) "$(INTDIR)"\
 "$(INTDIR)\snapvw.pch"


!ELSEIF  "$(CFG)" == "Snap - Win32 Unicode Release"


"$(INTDIR)\DinnerPg.obj" : $(SOURCE) $(DEP_CPP_DINNE) "$(INTDIR)"\
 "$(INTDIR)\snapvw.pch"


!ELSEIF  "$(CFG)" == "Snap - Win32 Unicode Debug"


"$(INTDIR)\DinnerPg.obj" : $(SOURCE) $(DEP_CPP_DINNE) "$(INTDIR)"\
 "$(INTDIR)\snapvw.pch"


!ENDIF 

# End Source File
# End Target
# End Project
################################################################################
