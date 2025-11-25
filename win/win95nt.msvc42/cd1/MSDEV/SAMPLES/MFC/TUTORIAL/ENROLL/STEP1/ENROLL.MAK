# Microsoft Visual C++ Generated NMAKE File, Format Version 40000
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

!IF "$(CFG)" == ""
CFG=Enroll - Win32 Debug
!MESSAGE No configuration specified.  Defaulting to Enroll - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "Enroll - Win32 Release" && "$(CFG)" != "Enroll - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "enroll.mak" CFG="Enroll - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Enroll - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "Enroll - Win32 Debug" (based on "Win32 (x86) Application")
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
# PROP Target_Last_Scanned "Enroll - Win32 Debug"
RSC=rc.exe
MTL=mktyplib.exe
CPP=cl.exe

!IF  "$(CFG)" == "Enroll - Win32 Release"

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

ALL : ".\Release\enroll.exe"

CLEAN : 
	-@erase ".\Release\enroll.exe"
	-@erase ".\Release\mainfrm.obj"
	-@erase ".\Release\enroll.pch"
	-@erase ".\Release\enroll.obj"
	-@erase ".\Release\enroldoc.obj"
	-@erase ".\Release\sectset.obj"
	-@erase ".\Release\sectform.obj"
	-@erase ".\Release\stdafx.obj"
	-@erase ".\Release\enroll.res"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
CPP_PROJ=/nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/enroll.pch" /Yu"stdafx.h" /Fo"$(INTDIR)/" /c\
 
CPP_OBJS=.\Release/
CPP_SBRS=
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/enroll.res" /d "NDEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/enroll.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 /nologo /subsystem:windows /machine:I386
LINK32_FLAGS=/nologo /subsystem:windows /incremental:no\
 /pdb:"$(OUTDIR)/enroll.pdb" /machine:I386 /out:"$(OUTDIR)/enroll.exe" 
LINK32_OBJS= \
	".\Release\mainfrm.obj" \
	".\Release\enroll.obj" \
	".\Release\enroldoc.obj" \
	".\Release\sectset.obj" \
	".\Release\sectform.obj" \
	".\Release\stdafx.obj" \
	".\Release\enroll.res"

".\Release\enroll.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "Enroll - Win32 Debug"

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

ALL : ".\Debug\enroll.exe"

CLEAN : 
	-@erase ".\Debug\vc40.pdb"
	-@erase ".\Debug\enroll.pch"
	-@erase ".\Debug\enroll.exe"
	-@erase ".\Debug\sectset.obj"
	-@erase ".\Debug\enroldoc.obj"
	-@erase ".\Debug\enroll.obj"
	-@erase ".\Debug\sectform.obj"
	-@erase ".\Debug\stdafx.obj"
	-@erase ".\Debug\mainfrm.obj"
	-@erase ".\Debug\enroll.res"
	-@erase ".\Debug\enroll.ilk"
	-@erase ".\Debug\enroll.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
CPP_PROJ=/nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/enroll.pch" /Yu"stdafx.h" /Fo"$(INTDIR)/"\
 /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\Debug/
CPP_SBRS=
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/enroll.res" /d "_DEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/enroll.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386
# ADD LINK32 /nologo /subsystem:windows /debug /machine:I386
LINK32_FLAGS=/nologo /subsystem:windows /incremental:yes\
 /pdb:"$(OUTDIR)/enroll.pdb" /debug /machine:I386 /out:"$(OUTDIR)/enroll.exe" 
LINK32_OBJS= \
	".\Debug\sectset.obj" \
	".\Debug\enroldoc.obj" \
	".\Debug\enroll.obj" \
	".\Debug\sectform.obj" \
	".\Debug\stdafx.obj" \
	".\Debug\mainfrm.obj" \
	".\Debug\enroll.res"

".\Debug\enroll.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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

# Name "Enroll - Win32 Release"
# Name "Enroll - Win32 Debug"

!IF  "$(CFG)" == "Enroll - Win32 Release"

!ELSEIF  "$(CFG)" == "Enroll - Win32 Debug"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\readme.txt

!IF  "$(CFG)" == "Enroll - Win32 Release"

!ELSEIF  "$(CFG)" == "Enroll - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\enroll.cpp

!IF  "$(CFG)" == "Enroll - Win32 Release"

DEP_CPP_ENROL=\
	".\enroll.h"\
	".\enroldoc.h"\
	".\mainfrm.h"\
	".\sectform.h"\
	".\sectset.h"\
	".\stdafx.h"\
	

".\Release\enroll.obj" : $(SOURCE) $(DEP_CPP_ENROL) "$(INTDIR)"\
 ".\Release\enroll.pch"


!ELSEIF  "$(CFG)" == "Enroll - Win32 Debug"

DEP_CPP_ENROL=\
	".\enroll.h"\
	".\mainfrm.h"\
	".\sectform.h"\
	".\sectset.h"\
	".\enroldoc.h"\
	".\stdafx.h"\
	

".\Debug\enroll.obj" : $(SOURCE) $(DEP_CPP_ENROL) "$(INTDIR)"\
 ".\Debug\enroll.pch"

".\Debug\vc40.pdb" : $(SOURCE) $(DEP_CPP_ENROL) "$(INTDIR)"\
 ".\Debug\enroll.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\stdafx.cpp
DEP_CPP_STDAF=\
	".\stdafx.h"\
	

!IF  "$(CFG)" == "Enroll - Win32 Release"

# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/enroll.pch" /Yc"stdafx.h" /Fo"$(INTDIR)/" /c\
 $(SOURCE) \
	

".\Release\stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

".\Release\enroll.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "Enroll - Win32 Debug"

# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/enroll.pch" /Yc"stdafx.h" /Fo"$(INTDIR)/"\
 /Fd"$(INTDIR)/" /c $(SOURCE) \
	

".\Debug\stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

".\Debug\vc40.pdb" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

".\Debug\enroll.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\mainfrm.cpp
DEP_CPP_MAINF=\
	".\enroll.h"\
	".\mainfrm.h"\
	".\stdafx.h"\
	

!IF  "$(CFG)" == "Enroll - Win32 Release"


".\Release\mainfrm.obj" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 ".\Release\enroll.pch"


!ELSEIF  "$(CFG)" == "Enroll - Win32 Debug"


".\Debug\mainfrm.obj" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 ".\Debug\enroll.pch"

".\Debug\vc40.pdb" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 ".\Debug\enroll.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\enroldoc.cpp
DEP_CPP_ENROLD=\
	".\enroldoc.h"\
	".\enroll.h"\
	".\sectset.h"\
	".\stdafx.h"\
	

!IF  "$(CFG)" == "Enroll - Win32 Release"


".\Release\enroldoc.obj" : $(SOURCE) $(DEP_CPP_ENROLD) "$(INTDIR)"\
 ".\Release\enroll.pch"


!ELSEIF  "$(CFG)" == "Enroll - Win32 Debug"


".\Debug\enroldoc.obj" : $(SOURCE) $(DEP_CPP_ENROLD) "$(INTDIR)"\
 ".\Debug\enroll.pch"

".\Debug\vc40.pdb" : $(SOURCE) $(DEP_CPP_ENROLD) "$(INTDIR)"\
 ".\Debug\enroll.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\sectform.cpp
DEP_CPP_SECTF=\
	".\enroll.h"\
	".\enroldoc.h"\
	".\sectform.h"\
	".\sectset.h"\
	".\stdafx.h"\
	

!IF  "$(CFG)" == "Enroll - Win32 Release"


".\Release\sectform.obj" : $(SOURCE) $(DEP_CPP_SECTF) "$(INTDIR)"\
 ".\Release\enroll.pch"


!ELSEIF  "$(CFG)" == "Enroll - Win32 Debug"


".\Debug\sectform.obj" : $(SOURCE) $(DEP_CPP_SECTF) "$(INTDIR)"\
 ".\Debug\enroll.pch"

".\Debug\vc40.pdb" : $(SOURCE) $(DEP_CPP_SECTF) "$(INTDIR)"\
 ".\Debug\enroll.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\sectset.cpp
DEP_CPP_SECTS=\
	".\enroll.h"\
	".\sectset.h"\
	".\stdafx.h"\
	

!IF  "$(CFG)" == "Enroll - Win32 Release"


".\Release\sectset.obj" : $(SOURCE) $(DEP_CPP_SECTS) "$(INTDIR)"\
 ".\Release\enroll.pch"


!ELSEIF  "$(CFG)" == "Enroll - Win32 Debug"


".\Debug\sectset.obj" : $(SOURCE) $(DEP_CPP_SECTS) "$(INTDIR)"\
 ".\Debug\enroll.pch"

".\Debug\vc40.pdb" : $(SOURCE) $(DEP_CPP_SECTS) "$(INTDIR)"\
 ".\Debug\enroll.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\enroll.rc
DEP_RSC_ENROLL=\
	".\res\enroll.ico"\
	".\res\toolbar.bmp"\
	".\res\enroll.rc2"\
	

!IF  "$(CFG)" == "Enroll - Win32 Release"


".\Release\enroll.res" : $(SOURCE) $(DEP_RSC_ENROLL) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Enroll - Win32 Debug"


".\Debug\enroll.res" : $(SOURCE) $(DEP_RSC_ENROLL) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ENDIF 

# End Source File
# End Target
# End Project
################################################################################
