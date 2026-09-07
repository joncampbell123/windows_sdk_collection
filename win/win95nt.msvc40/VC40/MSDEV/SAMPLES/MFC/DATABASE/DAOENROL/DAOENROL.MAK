# Microsoft Visual C++ Generated NMAKE File, Format Version 40001
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

!IF "$(CFG)" == ""
CFG=DaoEnrol - Win32 Debug
!MESSAGE No configuration specified.  Defaulting to DaoEnrol - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "DaoEnrol - Win32 Release" && "$(CFG)" !=\
 "DaoEnrol - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "DaoEnrol.mak" CFG="DaoEnrol - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "DaoEnrol - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "DaoEnrol - Win32 Debug" (based on "Win32 (x86) Application")
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
# PROP Target_Last_Scanned "DaoEnrol - Win32 Debug"
MTL=mktyplib.exe
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "DaoEnrol - Win32 Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir Release
# PROP BASE Intermediate_Dir Release
# PROP BASE Target_Dir
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir Release
# PROP Intermediate_Dir Release
# PROP Target_Dir
OUTDIR=.\Release
INTDIR=.\Release

ALL : ".\Release\DaoEnrol.exe"

CLEAN : 
	-@erase ".\Release\DaoEnrol.exe"
	-@erase ".\Release\sectform.obj"
	-@erase ".\Release\DaoEnrol.pch"
	-@erase ".\Release\crsform.obj"
	-@erase ".\Release\StdAfx.obj"
	-@erase ".\Release\MainFrm.obj"
	-@erase ".\Release\coursese.obj"
	-@erase ".\Release\addform.obj"
	-@erase ".\Release\sectset.obj"
	-@erase ".\Release\denrldoc.obj"
	-@erase ".\Release\DaoEnrol.obj"
	-@erase ".\Release\DaoEnrol.res"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
CPP_PROJ=/nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/DaoEnrol.pch" /Yu"stdafx.h" /Fo"$(INTDIR)/"\
 /c 
CPP_OBJS=.\Release/
CPP_SBRS=
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/DaoEnrol.res" /d "NDEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/DaoEnrol.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 /nologo /subsystem:windows /machine:I386
LINK32_FLAGS=/nologo /subsystem:windows /incremental:no\
 /pdb:"$(OUTDIR)/DaoEnrol.pdb" /machine:I386 /out:"$(OUTDIR)/DaoEnrol.exe" 
LINK32_OBJS= \
	".\Release\sectform.obj" \
	".\Release\crsform.obj" \
	".\Release\StdAfx.obj" \
	".\Release\MainFrm.obj" \
	".\Release\coursese.obj" \
	".\Release\addform.obj" \
	".\Release\sectset.obj" \
	".\Release\denrldoc.obj" \
	".\Release\DaoEnrol.obj" \
	".\Release\DaoEnrol.res"

".\Release\DaoEnrol.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "DaoEnrol - Win32 Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir Debug
# PROP BASE Intermediate_Dir Debug
# PROP BASE Target_Dir
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir Debug
# PROP Intermediate_Dir Debug
# PROP Target_Dir
OUTDIR=.\Debug
INTDIR=.\Debug

ALL : ".\Debug\DaoEnrol.exe" ".\Debug\DaoEnrol.bsc"

CLEAN : 
	-@erase ".\Debug\vc40.pdb"
	-@erase ".\Debug\DaoEnrol.pch"
	-@erase ".\Debug\DaoEnrol.bsc"
	-@erase ".\Debug\addform.sbr"
	-@erase ".\Debug\sectset.sbr"
	-@erase ".\Debug\sectform.sbr"
	-@erase ".\Debug\StdAfx.sbr"
	-@erase ".\Debug\crsform.sbr"
	-@erase ".\Debug\coursese.sbr"
	-@erase ".\Debug\denrldoc.sbr"
	-@erase ".\Debug\MainFrm.sbr"
	-@erase ".\Debug\DaoEnrol.sbr"
	-@erase ".\Debug\DaoEnrol.exe"
	-@erase ".\Debug\coursese.obj"
	-@erase ".\Debug\denrldoc.obj"
	-@erase ".\Debug\MainFrm.obj"
	-@erase ".\Debug\DaoEnrol.obj"
	-@erase ".\Debug\addform.obj"
	-@erase ".\Debug\sectset.obj"
	-@erase ".\Debug\sectform.obj"
	-@erase ".\Debug\StdAfx.obj"
	-@erase ".\Debug\crsform.obj"
	-@erase ".\Debug\DaoEnrol.res"
	-@erase ".\Debug\DaoEnrol.ilk"
	-@erase ".\Debug\DaoEnrol.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /Yu"stdafx.h" /c
CPP_PROJ=/nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /D "_AFXDLL" /D "_MBCS" /FR"$(INTDIR)/" /Fp"$(INTDIR)/DaoEnrol.pch"\
 /Yu"stdafx.h" /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\Debug/
CPP_SBRS=.\Debug/
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/DaoEnrol.res" /d "_DEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/DaoEnrol.bsc" 
BSC32_SBRS= \
	".\Debug\addform.sbr" \
	".\Debug\sectset.sbr" \
	".\Debug\sectform.sbr" \
	".\Debug\StdAfx.sbr" \
	".\Debug\crsform.sbr" \
	".\Debug\coursese.sbr" \
	".\Debug\denrldoc.sbr" \
	".\Debug\MainFrm.sbr" \
	".\Debug\DaoEnrol.sbr"

".\Debug\DaoEnrol.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386
# ADD LINK32 /nologo /subsystem:windows /debug /machine:I386
LINK32_FLAGS=/nologo /subsystem:windows /incremental:yes\
 /pdb:"$(OUTDIR)/DaoEnrol.pdb" /debug /machine:I386\
 /out:"$(OUTDIR)/DaoEnrol.exe" 
LINK32_OBJS= \
	".\Debug\coursese.obj" \
	".\Debug\denrldoc.obj" \
	".\Debug\MainFrm.obj" \
	".\Debug\DaoEnrol.obj" \
	".\Debug\addform.obj" \
	".\Debug\sectset.obj" \
	".\Debug\sectform.obj" \
	".\Debug\StdAfx.obj" \
	".\Debug\crsform.obj" \
	".\Debug\DaoEnrol.res"

".\Debug\DaoEnrol.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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

# Name "DaoEnrol - Win32 Release"
# Name "DaoEnrol - Win32 Debug"

!IF  "$(CFG)" == "DaoEnrol - Win32 Release"

!ELSEIF  "$(CFG)" == "DaoEnrol - Win32 Debug"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\ReadMe.txt

!IF  "$(CFG)" == "DaoEnrol - Win32 Release"

!ELSEIF  "$(CFG)" == "DaoEnrol - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\DaoEnrol.cpp
DEP_CPP_DAOEN=\
	".\DaoEnrol.h"\
	".\MainFrm.h"\
	".\coursese.h"\
	".\denrldoc.h"\
	".\sectform.h"\
	".\sectset.h"\
	".\StdAfx.h"\
	

!IF  "$(CFG)" == "DaoEnrol - Win32 Release"


".\Release\DaoEnrol.obj" : $(SOURCE) $(DEP_CPP_DAOEN) "$(INTDIR)"\
 ".\Release\DaoEnrol.pch"


!ELSEIF  "$(CFG)" == "DaoEnrol - Win32 Debug"


".\Debug\DaoEnrol.obj" : $(SOURCE) $(DEP_CPP_DAOEN) "$(INTDIR)"\
 ".\Debug\DaoEnrol.pch"

".\Debug\DaoEnrol.sbr" : $(SOURCE) $(DEP_CPP_DAOEN) "$(INTDIR)"\
 ".\Debug\DaoEnrol.pch"

".\Debug\vc40.pdb" : $(SOURCE) $(DEP_CPP_DAOEN) "$(INTDIR)"\
 ".\Debug\DaoEnrol.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\StdAfx.cpp
DEP_CPP_STDAF=\
	".\StdAfx.h"\
	

!IF  "$(CFG)" == "DaoEnrol - Win32 Release"

# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/DaoEnrol.pch" /Yc"stdafx.h" /Fo"$(INTDIR)/"\
 /c $(SOURCE) \
	

".\Release\StdAfx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

".\Release\DaoEnrol.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "DaoEnrol - Win32 Debug"

# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /D "_AFXDLL" /D "_MBCS" /FR"$(INTDIR)/" /Fp"$(INTDIR)/DaoEnrol.pch"\
 /Yc"stdafx.h" /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c $(SOURCE) \
	

".\Debug\StdAfx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

".\Debug\StdAfx.sbr" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

".\Debug\vc40.pdb" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

".\Debug\DaoEnrol.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\MainFrm.cpp
DEP_CPP_MAINF=\
	".\DaoEnrol.h"\
	".\MainFrm.h"\
	".\addform.h"\
	".\coursese.h"\
	".\crsform.h"\
	".\denrldoc.h"\
	".\sectform.h"\
	".\sectset.h"\
	".\StdAfx.h"\
	

!IF  "$(CFG)" == "DaoEnrol - Win32 Release"


".\Release\MainFrm.obj" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 ".\Release\DaoEnrol.pch"


!ELSEIF  "$(CFG)" == "DaoEnrol - Win32 Debug"


".\Debug\MainFrm.obj" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 ".\Debug\DaoEnrol.pch"

".\Debug\MainFrm.sbr" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 ".\Debug\DaoEnrol.pch"

".\Debug\vc40.pdb" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 ".\Debug\DaoEnrol.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\denrldoc.cpp
DEP_CPP_DENRL=\
	".\DaoEnrol.h"\
	".\coursese.h"\
	".\denrldoc.h"\
	".\sectset.h"\
	".\StdAfx.h"\
	

!IF  "$(CFG)" == "DaoEnrol - Win32 Release"


".\Release\denrldoc.obj" : $(SOURCE) $(DEP_CPP_DENRL) "$(INTDIR)"\
 ".\Release\DaoEnrol.pch"


!ELSEIF  "$(CFG)" == "DaoEnrol - Win32 Debug"


".\Debug\denrldoc.obj" : $(SOURCE) $(DEP_CPP_DENRL) "$(INTDIR)"\
 ".\Debug\DaoEnrol.pch"

".\Debug\denrldoc.sbr" : $(SOURCE) $(DEP_CPP_DENRL) "$(INTDIR)"\
 ".\Debug\DaoEnrol.pch"

".\Debug\vc40.pdb" : $(SOURCE) $(DEP_CPP_DENRL) "$(INTDIR)"\
 ".\Debug\DaoEnrol.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\sectform.cpp
DEP_CPP_SECTF=\
	".\DaoEnrol.h"\
	".\coursese.h"\
	".\denrldoc.h"\
	".\sectform.h"\
	".\sectset.h"\
	".\StdAfx.h"\
	

!IF  "$(CFG)" == "DaoEnrol - Win32 Release"


".\Release\sectform.obj" : $(SOURCE) $(DEP_CPP_SECTF) "$(INTDIR)"\
 ".\Release\DaoEnrol.pch"


!ELSEIF  "$(CFG)" == "DaoEnrol - Win32 Debug"


".\Debug\sectform.obj" : $(SOURCE) $(DEP_CPP_SECTF) "$(INTDIR)"\
 ".\Debug\DaoEnrol.pch"

".\Debug\sectform.sbr" : $(SOURCE) $(DEP_CPP_SECTF) "$(INTDIR)"\
 ".\Debug\DaoEnrol.pch"

".\Debug\vc40.pdb" : $(SOURCE) $(DEP_CPP_SECTF) "$(INTDIR)"\
 ".\Debug\DaoEnrol.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\sectset.cpp
DEP_CPP_SECTS=\
	".\DaoEnrol.h"\
	".\sectset.h"\
	".\StdAfx.h"\
	

!IF  "$(CFG)" == "DaoEnrol - Win32 Release"


".\Release\sectset.obj" : $(SOURCE) $(DEP_CPP_SECTS) "$(INTDIR)"\
 ".\Release\DaoEnrol.pch"


!ELSEIF  "$(CFG)" == "DaoEnrol - Win32 Debug"


".\Debug\sectset.obj" : $(SOURCE) $(DEP_CPP_SECTS) "$(INTDIR)"\
 ".\Debug\DaoEnrol.pch"

".\Debug\sectset.sbr" : $(SOURCE) $(DEP_CPP_SECTS) "$(INTDIR)"\
 ".\Debug\DaoEnrol.pch"

".\Debug\vc40.pdb" : $(SOURCE) $(DEP_CPP_SECTS) "$(INTDIR)"\
 ".\Debug\DaoEnrol.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\DaoEnrol.rc
DEP_RSC_DAOENR=\
	".\res\DaoEnrol.ico"\
	".\res\denrldoc.ico"\
	".\res\Toolbar.bmp"\
	".\res\DaoEnrol.rc2"\
	

!IF  "$(CFG)" == "DaoEnrol - Win32 Release"


".\Release\DaoEnrol.res" : $(SOURCE) $(DEP_RSC_DAOENR) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "DaoEnrol - Win32 Debug"


".\Debug\DaoEnrol.res" : $(SOURCE) $(DEP_RSC_DAOENR) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\coursese.cpp
DEP_CPP_COURS=\
	".\DaoEnrol.h"\
	".\coursese.h"\
	".\StdAfx.h"\
	

!IF  "$(CFG)" == "DaoEnrol - Win32 Release"


".\Release\coursese.obj" : $(SOURCE) $(DEP_CPP_COURS) "$(INTDIR)"\
 ".\Release\DaoEnrol.pch"


!ELSEIF  "$(CFG)" == "DaoEnrol - Win32 Debug"


".\Debug\coursese.obj" : $(SOURCE) $(DEP_CPP_COURS) "$(INTDIR)"\
 ".\Debug\DaoEnrol.pch"

".\Debug\coursese.sbr" : $(SOURCE) $(DEP_CPP_COURS) "$(INTDIR)"\
 ".\Debug\DaoEnrol.pch"

".\Debug\vc40.pdb" : $(SOURCE) $(DEP_CPP_COURS) "$(INTDIR)"\
 ".\Debug\DaoEnrol.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\addform.cpp
DEP_CPP_ADDFO=\
	".\addform.h"\
	".\DaoEnrol.h"\
	".\StdAfx.h"\
	

!IF  "$(CFG)" == "DaoEnrol - Win32 Release"


".\Release\addform.obj" : $(SOURCE) $(DEP_CPP_ADDFO) "$(INTDIR)"\
 ".\Release\DaoEnrol.pch"


!ELSEIF  "$(CFG)" == "DaoEnrol - Win32 Debug"


".\Debug\addform.obj" : $(SOURCE) $(DEP_CPP_ADDFO) "$(INTDIR)"\
 ".\Debug\DaoEnrol.pch"

".\Debug\addform.sbr" : $(SOURCE) $(DEP_CPP_ADDFO) "$(INTDIR)"\
 ".\Debug\DaoEnrol.pch"

".\Debug\vc40.pdb" : $(SOURCE) $(DEP_CPP_ADDFO) "$(INTDIR)"\
 ".\Debug\DaoEnrol.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\crsform.cpp
DEP_CPP_CRSFO=\
	".\addform.h"\
	".\coursese.h"\
	".\crsform.h"\
	".\DaoEnrol.h"\
	".\denrldoc.h"\
	".\MainFrm.h"\
	".\sectset.h"\
	".\StdAfx.h"\
	

!IF  "$(CFG)" == "DaoEnrol - Win32 Release"


".\Release\crsform.obj" : $(SOURCE) $(DEP_CPP_CRSFO) "$(INTDIR)"\
 ".\Release\DaoEnrol.pch"


!ELSEIF  "$(CFG)" == "DaoEnrol - Win32 Debug"


".\Debug\crsform.obj" : $(SOURCE) $(DEP_CPP_CRSFO) "$(INTDIR)"\
 ".\Debug\DaoEnrol.pch"

".\Debug\crsform.sbr" : $(SOURCE) $(DEP_CPP_CRSFO) "$(INTDIR)"\
 ".\Debug\DaoEnrol.pch"

".\Debug\vc40.pdb" : $(SOURCE) $(DEP_CPP_CRSFO) "$(INTDIR)"\
 ".\Debug\DaoEnrol.pch"


!ENDIF 

# End Source File
# End Target
# End Project
################################################################################
