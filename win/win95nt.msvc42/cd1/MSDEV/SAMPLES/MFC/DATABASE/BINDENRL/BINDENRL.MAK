# Microsoft Developer Studio Generated NMAKE File, Format Version 4.20
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

!IF "$(CFG)" == ""
CFG=BindEnrl - Win32 Debug
!MESSAGE No configuration specified.  Defaulting to BindEnrl - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "BindEnrl - Win32 Release" && "$(CFG)" !=\
 "BindEnrl - Win32 Debug" && "$(CFG)" != "BindEnrl - Win32 Unicode Debug" &&\
 "$(CFG)" != "BindEnrl - Win32 Unicode Release"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "BindEnrl.mak" CFG="BindEnrl - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "BindEnrl - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "BindEnrl - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE "BindEnrl - Win32 Unicode Debug" (based on "Win32 (x86) Application")
!MESSAGE "BindEnrl - Win32 Unicode Release" (based on\
 "Win32 (x86) Application")
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
# PROP Target_Last_Scanned "BindEnrl - Win32 Unicode Release"
MTL=mktyplib.exe
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "BindEnrl - Win32 Release"

# PROP BASE Use_MFC 5
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

ALL : "$(OUTDIR)\BindEnrl.exe"

CLEAN : 
	-@erase "$(INTDIR)\BindEnrl.obj"
	-@erase "$(INTDIR)\BindEnrl.pch"
	-@erase "$(INTDIR)\BindEnrl.res"
	-@erase "$(INTDIR)\cboxctrl.obj"
	-@erase "$(INTDIR)\CoursDlg.obj"
	-@erase "$(INTDIR)\EnrolDlg.obj"
	-@erase "$(INTDIR)\InstrDlg.obj"
	-@erase "$(INTDIR)\lboxctrl.obj"
	-@erase "$(INTDIR)\mcsdsc.obj"
	-@erase "$(INTDIR)\msmask.obj"
	-@erase "$(INTDIR)\propsht.obj"
	-@erase "$(INTDIR)\SectnDlg.obj"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\StudDlg.obj"
	-@erase "$(OUTDIR)\BindEnrl.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

F90=fl32.exe
# ADD BASE F90 /I "Release/"
# ADD F90 /I "Release/"
F90_OBJS=.\Release/
F90_PROJ=/I "Release/" /Fo"Release/" 

.for{$(F90_OBJS)}.obj:
   $(F90) $(F90_PROJ) $<  

.f{$(F90_OBJS)}.obj:
   $(F90) $(F90_PROJ) $<  

.f90{$(F90_OBJS)}.obj:
   $(F90) $(F90_PROJ) $<  

# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Yu"stdafx.h" /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
CPP_PROJ=/nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/BindEnrl.pch" /Yu"stdafx.h" /Fo"$(INTDIR)/"\
 /c 
CPP_OBJS=.\Release/
CPP_SBRS=.\.
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/BindEnrl.res" /d "NDEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/BindEnrl.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 /nologo /subsystem:windows /machine:I386
# SUBTRACT LINK32 /force
LINK32_FLAGS=/nologo /subsystem:windows /incremental:no\
 /pdb:"$(OUTDIR)/BindEnrl.pdb" /machine:I386 /out:"$(OUTDIR)/BindEnrl.exe" 
LINK32_OBJS= \
	"$(INTDIR)\BindEnrl.obj" \
	"$(INTDIR)\BindEnrl.res" \
	"$(INTDIR)\cboxctrl.obj" \
	"$(INTDIR)\CoursDlg.obj" \
	"$(INTDIR)\EnrolDlg.obj" \
	"$(INTDIR)\InstrDlg.obj" \
	"$(INTDIR)\lboxctrl.obj" \
	"$(INTDIR)\mcsdsc.obj" \
	"$(INTDIR)\msmask.obj" \
	"$(INTDIR)\propsht.obj" \
	"$(INTDIR)\SectnDlg.obj" \
	"$(INTDIR)\StdAfx.obj" \
	"$(INTDIR)\StudDlg.obj"

"$(OUTDIR)\BindEnrl.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "BindEnrl - Win32 Debug"

# PROP BASE Use_MFC 5
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

ALL : "$(OUTDIR)\BindEnrl.exe"

CLEAN : 
	-@erase "$(INTDIR)\BindEnrl.obj"
	-@erase "$(INTDIR)\BindEnrl.pch"
	-@erase "$(INTDIR)\BindEnrl.res"
	-@erase "$(INTDIR)\cboxctrl.obj"
	-@erase "$(INTDIR)\CoursDlg.obj"
	-@erase "$(INTDIR)\EnrolDlg.obj"
	-@erase "$(INTDIR)\InstrDlg.obj"
	-@erase "$(INTDIR)\lboxctrl.obj"
	-@erase "$(INTDIR)\mcsdsc.obj"
	-@erase "$(INTDIR)\msmask.obj"
	-@erase "$(INTDIR)\propsht.obj"
	-@erase "$(INTDIR)\SectnDlg.obj"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\StudDlg.obj"
	-@erase "$(INTDIR)\vc40.idb"
	-@erase "$(INTDIR)\vc40.pdb"
	-@erase "$(OUTDIR)\BindEnrl.exe"
	-@erase "$(OUTDIR)\BindEnrl.ilk"
	-@erase "$(OUTDIR)\BindEnrl.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

F90=fl32.exe
# ADD BASE F90 /I "Debug/"
# ADD F90 /I "Debug/"
F90_OBJS=.\Debug/
F90_PROJ=/I "Release/" /Fo"Release/" 

.for{$(F90_OBJS)}.obj:
   $(F90) $(F90_PROJ) $<  

.f{$(F90_OBJS)}.obj:
   $(F90) $(F90_PROJ) $<  

.f90{$(F90_OBJS)}.obj:
   $(F90) $(F90_PROJ) $<  

# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /Yu"stdafx.h" /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
CPP_PROJ=/nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/BindEnrl.pch" /Yu"stdafx.h"\
 /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\Debug/
CPP_SBRS=.\.
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/BindEnrl.res" /d "_DEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/BindEnrl.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386
# ADD LINK32 /nologo /subsystem:windows /debug /machine:I386
# SUBTRACT LINK32 /force
LINK32_FLAGS=/nologo /subsystem:windows /incremental:yes\
 /pdb:"$(OUTDIR)/BindEnrl.pdb" /debug /machine:I386\
 /out:"$(OUTDIR)/BindEnrl.exe" 
LINK32_OBJS= \
	"$(INTDIR)\BindEnrl.obj" \
	"$(INTDIR)\BindEnrl.res" \
	"$(INTDIR)\cboxctrl.obj" \
	"$(INTDIR)\CoursDlg.obj" \
	"$(INTDIR)\EnrolDlg.obj" \
	"$(INTDIR)\InstrDlg.obj" \
	"$(INTDIR)\lboxctrl.obj" \
	"$(INTDIR)\mcsdsc.obj" \
	"$(INTDIR)\msmask.obj" \
	"$(INTDIR)\propsht.obj" \
	"$(INTDIR)\SectnDlg.obj" \
	"$(INTDIR)\StdAfx.obj" \
	"$(INTDIR)\StudDlg.obj"

"$(OUTDIR)\BindEnrl.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "BindEnrl - Win32 Unicode Debug"

# PROP BASE Use_MFC 5
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "BindEnrl"
# PROP BASE Intermediate_Dir "BindEnrl"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "UniDebug"
# PROP Intermediate_Dir "UniDebug"
# PROP Target_Dir ""
OUTDIR=.\UniDebug
INTDIR=.\UniDebug

ALL : "$(OUTDIR)\BindEnrl.exe"

CLEAN : 
	-@erase "$(INTDIR)\BindEnrl.obj"
	-@erase "$(INTDIR)\BindEnrl.pch"
	-@erase "$(INTDIR)\BindEnrl.res"
	-@erase "$(INTDIR)\cboxctrl.obj"
	-@erase "$(INTDIR)\CoursDlg.obj"
	-@erase "$(INTDIR)\EnrolDlg.obj"
	-@erase "$(INTDIR)\InstrDlg.obj"
	-@erase "$(INTDIR)\lboxctrl.obj"
	-@erase "$(INTDIR)\mcsdsc.obj"
	-@erase "$(INTDIR)\msmask.obj"
	-@erase "$(INTDIR)\propsht.obj"
	-@erase "$(INTDIR)\SectnDlg.obj"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\StudDlg.obj"
	-@erase "$(INTDIR)\vc40.idb"
	-@erase "$(INTDIR)\vc40.pdb"
	-@erase "$(OUTDIR)\BindEnrl.exe"
	-@erase "$(OUTDIR)\BindEnrl.ilk"
	-@erase "$(OUTDIR)\BindEnrl.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

F90=fl32.exe
# ADD BASE F90 /I "BindEnrl/"
# ADD F90 /I "UniDebug/"
F90_OBJS=.\UniDebug/
F90_PROJ=/I "Release/" /Fo"Release/" 

.for{$(F90_OBJS)}.obj:
   $(F90) $(F90_PROJ) $<  

.f{$(F90_OBJS)}.obj:
   $(F90) $(F90_PROJ) $<  

.f90{$(F90_OBJS)}.obj:
   $(F90) $(F90_PROJ) $<  

# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /Yu"stdafx.h" /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_UNICODE" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
CPP_PROJ=/nologo /MDd /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS"\
 /D "_UNICODE" /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/BindEnrl.pch" /Yu"stdafx.h"\
 /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\UniDebug/
CPP_SBRS=.\.
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/BindEnrl.res" /d "_DEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/BindEnrl.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386
# ADD LINK32 /nologo /subsystem:windows /debug /machine:I386
# SUBTRACT LINK32 /force
LINK32_FLAGS=/nologo /subsystem:windows /incremental:yes\
 /pdb:"$(OUTDIR)/BindEnrl.pdb" /debug /machine:I386\
 /out:"$(OUTDIR)/BindEnrl.exe" 
LINK32_OBJS= \
	"$(INTDIR)\BindEnrl.obj" \
	"$(INTDIR)\BindEnrl.res" \
	"$(INTDIR)\cboxctrl.obj" \
	"$(INTDIR)\CoursDlg.obj" \
	"$(INTDIR)\EnrolDlg.obj" \
	"$(INTDIR)\InstrDlg.obj" \
	"$(INTDIR)\lboxctrl.obj" \
	"$(INTDIR)\mcsdsc.obj" \
	"$(INTDIR)\msmask.obj" \
	"$(INTDIR)\propsht.obj" \
	"$(INTDIR)\SectnDlg.obj" \
	"$(INTDIR)\StdAfx.obj" \
	"$(INTDIR)\StudDlg.obj"

"$(OUTDIR)\BindEnrl.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "BindEnrl - Win32 Unicode Release"

# PROP BASE Use_MFC 5
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "BindEnr0"
# PROP BASE Intermediate_Dir "BindEnr0"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "UniRel"
# PROP Intermediate_Dir "UniRel"
# PROP Target_Dir ""
OUTDIR=.\UniRel
INTDIR=.\UniRel

ALL : "$(OUTDIR)\BindEnrl.exe"

CLEAN : 
	-@erase "$(INTDIR)\BindEnrl.obj"
	-@erase "$(INTDIR)\BindEnrl.pch"
	-@erase "$(INTDIR)\BindEnrl.res"
	-@erase "$(INTDIR)\cboxctrl.obj"
	-@erase "$(INTDIR)\CoursDlg.obj"
	-@erase "$(INTDIR)\EnrolDlg.obj"
	-@erase "$(INTDIR)\InstrDlg.obj"
	-@erase "$(INTDIR)\lboxctrl.obj"
	-@erase "$(INTDIR)\mcsdsc.obj"
	-@erase "$(INTDIR)\msmask.obj"
	-@erase "$(INTDIR)\propsht.obj"
	-@erase "$(INTDIR)\SectnDlg.obj"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\StudDlg.obj"
	-@erase "$(OUTDIR)\BindEnrl.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

F90=fl32.exe
# ADD BASE F90 /I "BindEnr0/"
# ADD F90 /I "UniRel/"
F90_OBJS=.\UniRel/
F90_PROJ=/I "Release/" /Fo"Release/" 

.for{$(F90_OBJS)}.obj:
   $(F90) $(F90_PROJ) $<  

.f{$(F90_OBJS)}.obj:
   $(F90) $(F90_PROJ) $<  

.f90{$(F90_OBJS)}.obj:
   $(F90) $(F90_PROJ) $<  

# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Yu"stdafx.h" /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_UNICODE" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
CPP_PROJ=/nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D\
 "_UNICODE" /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/BindEnrl.pch" /Yu"stdafx.h"\
 /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\UniRel/
CPP_SBRS=.\.
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/BindEnrl.res" /d "NDEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/BindEnrl.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 /nologo /subsystem:windows /machine:I386
# SUBTRACT LINK32 /force
LINK32_FLAGS=/nologo /subsystem:windows /incremental:no\
 /pdb:"$(OUTDIR)/BindEnrl.pdb" /machine:I386 /out:"$(OUTDIR)/BindEnrl.exe" 
LINK32_OBJS= \
	"$(INTDIR)\BindEnrl.obj" \
	"$(INTDIR)\BindEnrl.res" \
	"$(INTDIR)\cboxctrl.obj" \
	"$(INTDIR)\CoursDlg.obj" \
	"$(INTDIR)\EnrolDlg.obj" \
	"$(INTDIR)\InstrDlg.obj" \
	"$(INTDIR)\lboxctrl.obj" \
	"$(INTDIR)\mcsdsc.obj" \
	"$(INTDIR)\msmask.obj" \
	"$(INTDIR)\propsht.obj" \
	"$(INTDIR)\SectnDlg.obj" \
	"$(INTDIR)\StdAfx.obj" \
	"$(INTDIR)\StudDlg.obj"

"$(OUTDIR)\BindEnrl.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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

# Name "BindEnrl - Win32 Release"
# Name "BindEnrl - Win32 Debug"
# Name "BindEnrl - Win32 Unicode Debug"
# Name "BindEnrl - Win32 Unicode Release"

!IF  "$(CFG)" == "BindEnrl - Win32 Release"

!ELSEIF  "$(CFG)" == "BindEnrl - Win32 Debug"

!ELSEIF  "$(CFG)" == "BindEnrl - Win32 Unicode Debug"

!ELSEIF  "$(CFG)" == "BindEnrl - Win32 Unicode Release"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\BindEnrl.cpp
DEP_CPP_BINDE=\
	".\BindEnrl.h"\
	".\CoursDlg.h"\
	".\InstrDlg.h"\
	".\mcsdsc.h"\
	".\propsht.h"\
	".\SectnDlg.h"\
	".\StdAfx.h"\
	".\StudDlg.h"\
	

!IF  "$(CFG)" == "BindEnrl - Win32 Release"


"$(INTDIR)\BindEnrl.obj" : $(SOURCE) $(DEP_CPP_BINDE) "$(INTDIR)"\
 "$(INTDIR)\BindEnrl.pch"


!ELSEIF  "$(CFG)" == "BindEnrl - Win32 Debug"


"$(INTDIR)\BindEnrl.obj" : $(SOURCE) $(DEP_CPP_BINDE) "$(INTDIR)"\
 "$(INTDIR)\BindEnrl.pch"


!ELSEIF  "$(CFG)" == "BindEnrl - Win32 Unicode Debug"


"$(INTDIR)\BindEnrl.obj" : $(SOURCE) $(DEP_CPP_BINDE) "$(INTDIR)"\
 "$(INTDIR)\BindEnrl.pch"


!ELSEIF  "$(CFG)" == "BindEnrl - Win32 Unicode Release"


"$(INTDIR)\BindEnrl.obj" : $(SOURCE) $(DEP_CPP_BINDE) "$(INTDIR)"\
 "$(INTDIR)\BindEnrl.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\StdAfx.cpp
DEP_CPP_STDAF=\
	".\StdAfx.h"\
	

!IF  "$(CFG)" == "BindEnrl - Win32 Release"

# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/BindEnrl.pch" /Yc"stdafx.h" /Fo"$(INTDIR)/"\
 /c $(SOURCE) \
	

"$(INTDIR)\StdAfx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\BindEnrl.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "BindEnrl - Win32 Debug"

# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/BindEnrl.pch" /Yc"stdafx.h"\
 /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\StdAfx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\BindEnrl.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "BindEnrl - Win32 Unicode Debug"

# ADD BASE CPP /Yc"stdafx.h"
# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MDd /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS"\
 /D "_UNICODE" /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/BindEnrl.pch" /Yc"stdafx.h"\
 /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\StdAfx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\BindEnrl.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "BindEnrl - Win32 Unicode Release"

# ADD BASE CPP /Yc"stdafx.h"
# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D\
 "_UNICODE" /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/BindEnrl.pch" /Yc"stdafx.h"\
 /Fo"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\StdAfx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\BindEnrl.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\BindEnrl.rc
DEP_RSC_BINDEN=\
	".\res\BindEnrl.ico"\
	".\res\BindEnrl.rc2"\
	

!IF  "$(CFG)" == "BindEnrl - Win32 Release"


"$(INTDIR)\BindEnrl.res" : $(SOURCE) $(DEP_RSC_BINDEN) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "BindEnrl - Win32 Debug"


"$(INTDIR)\BindEnrl.res" : $(SOURCE) $(DEP_RSC_BINDEN) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "BindEnrl - Win32 Unicode Debug"


"$(INTDIR)\BindEnrl.res" : $(SOURCE) $(DEP_RSC_BINDEN) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "BindEnrl - Win32 Unicode Release"


"$(INTDIR)\BindEnrl.res" : $(SOURCE) $(DEP_RSC_BINDEN) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\propsht.cpp
DEP_CPP_PROPS=\
	".\CoursDlg.h"\
	".\EnrolDlg.h"\
	".\InstrDlg.h"\
	".\mcsdsc.h"\
	".\propsht.h"\
	".\SectnDlg.h"\
	".\StdAfx.h"\
	".\StudDlg.h"\
	

!IF  "$(CFG)" == "BindEnrl - Win32 Release"


"$(INTDIR)\propsht.obj" : $(SOURCE) $(DEP_CPP_PROPS) "$(INTDIR)"\
 "$(INTDIR)\BindEnrl.pch"


!ELSEIF  "$(CFG)" == "BindEnrl - Win32 Debug"


"$(INTDIR)\propsht.obj" : $(SOURCE) $(DEP_CPP_PROPS) "$(INTDIR)"\
 "$(INTDIR)\BindEnrl.pch"


!ELSEIF  "$(CFG)" == "BindEnrl - Win32 Unicode Debug"


"$(INTDIR)\propsht.obj" : $(SOURCE) $(DEP_CPP_PROPS) "$(INTDIR)"\
 "$(INTDIR)\BindEnrl.pch"


!ELSEIF  "$(CFG)" == "BindEnrl - Win32 Unicode Release"


"$(INTDIR)\propsht.obj" : $(SOURCE) $(DEP_CPP_PROPS) "$(INTDIR)"\
 "$(INTDIR)\BindEnrl.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\mcsdsc.cpp
DEP_CPP_MCSDS=\
	".\mcsdsc.h"\
	".\StdAfx.h"\
	

!IF  "$(CFG)" == "BindEnrl - Win32 Release"


"$(INTDIR)\mcsdsc.obj" : $(SOURCE) $(DEP_CPP_MCSDS) "$(INTDIR)"\
 "$(INTDIR)\BindEnrl.pch"


!ELSEIF  "$(CFG)" == "BindEnrl - Win32 Debug"


"$(INTDIR)\mcsdsc.obj" : $(SOURCE) $(DEP_CPP_MCSDS) "$(INTDIR)"\
 "$(INTDIR)\BindEnrl.pch"


!ELSEIF  "$(CFG)" == "BindEnrl - Win32 Unicode Debug"


"$(INTDIR)\mcsdsc.obj" : $(SOURCE) $(DEP_CPP_MCSDS) "$(INTDIR)"\
 "$(INTDIR)\BindEnrl.pch"


!ELSEIF  "$(CFG)" == "BindEnrl - Win32 Unicode Release"


"$(INTDIR)\mcsdsc.obj" : $(SOURCE) $(DEP_CPP_MCSDS) "$(INTDIR)"\
 "$(INTDIR)\BindEnrl.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\msmask.cpp
DEP_CPP_MSMAS=\
	".\msmask.h"\
	".\StdAfx.h"\
	

!IF  "$(CFG)" == "BindEnrl - Win32 Release"


"$(INTDIR)\msmask.obj" : $(SOURCE) $(DEP_CPP_MSMAS) "$(INTDIR)"\
 "$(INTDIR)\BindEnrl.pch"


!ELSEIF  "$(CFG)" == "BindEnrl - Win32 Debug"


"$(INTDIR)\msmask.obj" : $(SOURCE) $(DEP_CPP_MSMAS) "$(INTDIR)"\
 "$(INTDIR)\BindEnrl.pch"


!ELSEIF  "$(CFG)" == "BindEnrl - Win32 Unicode Debug"


"$(INTDIR)\msmask.obj" : $(SOURCE) $(DEP_CPP_MSMAS) "$(INTDIR)"\
 "$(INTDIR)\BindEnrl.pch"


!ELSEIF  "$(CFG)" == "BindEnrl - Win32 Unicode Release"


"$(INTDIR)\msmask.obj" : $(SOURCE) $(DEP_CPP_MSMAS) "$(INTDIR)"\
 "$(INTDIR)\BindEnrl.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\SectnDlg.cpp
DEP_CPP_SECTN=\
	".\cboxctrl.h"\
	".\mcsdsc.h"\
	".\msmask.h"\
	".\SectnDlg.h"\
	".\StdAfx.h"\
	

!IF  "$(CFG)" == "BindEnrl - Win32 Release"


"$(INTDIR)\SectnDlg.obj" : $(SOURCE) $(DEP_CPP_SECTN) "$(INTDIR)"\
 "$(INTDIR)\BindEnrl.pch"


!ELSEIF  "$(CFG)" == "BindEnrl - Win32 Debug"


"$(INTDIR)\SectnDlg.obj" : $(SOURCE) $(DEP_CPP_SECTN) "$(INTDIR)"\
 "$(INTDIR)\BindEnrl.pch"


!ELSEIF  "$(CFG)" == "BindEnrl - Win32 Unicode Debug"


"$(INTDIR)\SectnDlg.obj" : $(SOURCE) $(DEP_CPP_SECTN) "$(INTDIR)"\
 "$(INTDIR)\BindEnrl.pch"


!ELSEIF  "$(CFG)" == "BindEnrl - Win32 Unicode Release"


"$(INTDIR)\SectnDlg.obj" : $(SOURCE) $(DEP_CPP_SECTN) "$(INTDIR)"\
 "$(INTDIR)\BindEnrl.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\CoursDlg.cpp
DEP_CPP_COURS=\
	".\CoursDlg.h"\
	".\lboxctrl.h"\
	".\mcsdsc.h"\
	".\msmask.h"\
	".\StdAfx.h"\
	

!IF  "$(CFG)" == "BindEnrl - Win32 Release"


"$(INTDIR)\CoursDlg.obj" : $(SOURCE) $(DEP_CPP_COURS) "$(INTDIR)"\
 "$(INTDIR)\BindEnrl.pch"


!ELSEIF  "$(CFG)" == "BindEnrl - Win32 Debug"


"$(INTDIR)\CoursDlg.obj" : $(SOURCE) $(DEP_CPP_COURS) "$(INTDIR)"\
 "$(INTDIR)\BindEnrl.pch"


!ELSEIF  "$(CFG)" == "BindEnrl - Win32 Unicode Debug"


"$(INTDIR)\CoursDlg.obj" : $(SOURCE) $(DEP_CPP_COURS) "$(INTDIR)"\
 "$(INTDIR)\BindEnrl.pch"


!ELSEIF  "$(CFG)" == "BindEnrl - Win32 Unicode Release"


"$(INTDIR)\CoursDlg.obj" : $(SOURCE) $(DEP_CPP_COURS) "$(INTDIR)"\
 "$(INTDIR)\BindEnrl.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\EnrolDlg.cpp
DEP_CPP_ENROL=\
	".\BindEnrl.h"\
	".\EnrolDlg.h"\
	".\StdAfx.h"\
	

!IF  "$(CFG)" == "BindEnrl - Win32 Release"


"$(INTDIR)\EnrolDlg.obj" : $(SOURCE) $(DEP_CPP_ENROL) "$(INTDIR)"\
 "$(INTDIR)\BindEnrl.pch"


!ELSEIF  "$(CFG)" == "BindEnrl - Win32 Debug"


"$(INTDIR)\EnrolDlg.obj" : $(SOURCE) $(DEP_CPP_ENROL) "$(INTDIR)"\
 "$(INTDIR)\BindEnrl.pch"


!ELSEIF  "$(CFG)" == "BindEnrl - Win32 Unicode Debug"


"$(INTDIR)\EnrolDlg.obj" : $(SOURCE) $(DEP_CPP_ENROL) "$(INTDIR)"\
 "$(INTDIR)\BindEnrl.pch"


!ELSEIF  "$(CFG)" == "BindEnrl - Win32 Unicode Release"


"$(INTDIR)\EnrolDlg.obj" : $(SOURCE) $(DEP_CPP_ENROL) "$(INTDIR)"\
 "$(INTDIR)\BindEnrl.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\StudDlg.cpp
DEP_CPP_STUDD=\
	".\mcsdsc.h"\
	".\StdAfx.h"\
	".\StudDlg.h"\
	

!IF  "$(CFG)" == "BindEnrl - Win32 Release"


"$(INTDIR)\StudDlg.obj" : $(SOURCE) $(DEP_CPP_STUDD) "$(INTDIR)"\
 "$(INTDIR)\BindEnrl.pch"


!ELSEIF  "$(CFG)" == "BindEnrl - Win32 Debug"


"$(INTDIR)\StudDlg.obj" : $(SOURCE) $(DEP_CPP_STUDD) "$(INTDIR)"\
 "$(INTDIR)\BindEnrl.pch"


!ELSEIF  "$(CFG)" == "BindEnrl - Win32 Unicode Debug"


"$(INTDIR)\StudDlg.obj" : $(SOURCE) $(DEP_CPP_STUDD) "$(INTDIR)"\
 "$(INTDIR)\BindEnrl.pch"


!ELSEIF  "$(CFG)" == "BindEnrl - Win32 Unicode Release"


"$(INTDIR)\StudDlg.obj" : $(SOURCE) $(DEP_CPP_STUDD) "$(INTDIR)"\
 "$(INTDIR)\BindEnrl.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\InstrDlg.cpp
DEP_CPP_INSTR=\
	".\InstrDlg.h"\
	".\mcsdsc.h"\
	".\StdAfx.h"\
	

!IF  "$(CFG)" == "BindEnrl - Win32 Release"


"$(INTDIR)\InstrDlg.obj" : $(SOURCE) $(DEP_CPP_INSTR) "$(INTDIR)"\
 "$(INTDIR)\BindEnrl.pch"


!ELSEIF  "$(CFG)" == "BindEnrl - Win32 Debug"


"$(INTDIR)\InstrDlg.obj" : $(SOURCE) $(DEP_CPP_INSTR) "$(INTDIR)"\
 "$(INTDIR)\BindEnrl.pch"


!ELSEIF  "$(CFG)" == "BindEnrl - Win32 Unicode Debug"


"$(INTDIR)\InstrDlg.obj" : $(SOURCE) $(DEP_CPP_INSTR) "$(INTDIR)"\
 "$(INTDIR)\BindEnrl.pch"


!ELSEIF  "$(CFG)" == "BindEnrl - Win32 Unicode Release"


"$(INTDIR)\InstrDlg.obj" : $(SOURCE) $(DEP_CPP_INSTR) "$(INTDIR)"\
 "$(INTDIR)\BindEnrl.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\cboxctrl.cpp
DEP_CPP_CBOXC=\
	".\cboxctrl.h"\
	".\StdAfx.h"\
	

!IF  "$(CFG)" == "BindEnrl - Win32 Release"


"$(INTDIR)\cboxctrl.obj" : $(SOURCE) $(DEP_CPP_CBOXC) "$(INTDIR)"\
 "$(INTDIR)\BindEnrl.pch"


!ELSEIF  "$(CFG)" == "BindEnrl - Win32 Debug"


"$(INTDIR)\cboxctrl.obj" : $(SOURCE) $(DEP_CPP_CBOXC) "$(INTDIR)"\
 "$(INTDIR)\BindEnrl.pch"


!ELSEIF  "$(CFG)" == "BindEnrl - Win32 Unicode Debug"


"$(INTDIR)\cboxctrl.obj" : $(SOURCE) $(DEP_CPP_CBOXC) "$(INTDIR)"\
 "$(INTDIR)\BindEnrl.pch"


!ELSEIF  "$(CFG)" == "BindEnrl - Win32 Unicode Release"


"$(INTDIR)\cboxctrl.obj" : $(SOURCE) $(DEP_CPP_CBOXC) "$(INTDIR)"\
 "$(INTDIR)\BindEnrl.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\lboxctrl.cpp
DEP_CPP_LBOXC=\
	".\lboxctrl.h"\
	".\StdAfx.h"\
	

!IF  "$(CFG)" == "BindEnrl - Win32 Release"


"$(INTDIR)\lboxctrl.obj" : $(SOURCE) $(DEP_CPP_LBOXC) "$(INTDIR)"\
 "$(INTDIR)\BindEnrl.pch"


!ELSEIF  "$(CFG)" == "BindEnrl - Win32 Debug"


"$(INTDIR)\lboxctrl.obj" : $(SOURCE) $(DEP_CPP_LBOXC) "$(INTDIR)"\
 "$(INTDIR)\BindEnrl.pch"


!ELSEIF  "$(CFG)" == "BindEnrl - Win32 Unicode Debug"


"$(INTDIR)\lboxctrl.obj" : $(SOURCE) $(DEP_CPP_LBOXC) "$(INTDIR)"\
 "$(INTDIR)\BindEnrl.pch"


!ELSEIF  "$(CFG)" == "BindEnrl - Win32 Unicode Release"


"$(INTDIR)\lboxctrl.obj" : $(SOURCE) $(DEP_CPP_LBOXC) "$(INTDIR)"\
 "$(INTDIR)\BindEnrl.pch"


!ENDIF 

# End Source File
# End Target
# End Project
################################################################################
################################################################################
# Section BindEnrl : {FAEEE761-117E-101B-8933-08002B2F4F5A}
# 	2:5:Class:CCbox1Ctrl
# 	2:10:HeaderFile:cboxctrl.h
# 	2:8:ImplFile:cboxctrl.cpp
# End Section
################################################################################
################################################################################
# Section BindEnrl : {FAEEE760-117E-101B-8933-08002B2F4F5A}
# 	0:8:Font.cpp:C:\mfc42\nonship\BindEnrl\Font.cpp
# 	0:9:Picture.h:C:\mfc42\nonship\BindEnrl\Picture.h
# 	0:6:Font.h:C:\mfc42\nonship\BindEnrl\Font.h
# 	0:11:Cbox1Ctrl.h:C:\mfc42\nonship\BindEnrl\Cbox1Ctrl.h
# 	0:11:Picture.cpp:C:\mfc42\nonship\BindEnrl\Picture.cpp
# 	0:13:Cbox1Ctrl.cpp:C:\mfc42\nonship\BindEnrl\Cbox1Ctrl.cpp
# 	2:21:DefaultSinkHeaderFile:cboxctrl.h
# 	2:16:DefaultSinkClass:CCbox1Ctrl
# End Section
################################################################################
################################################################################
# Section BindEnrl : {4F5F826A-8AAF-11CE-A77F-08002B2F4E98}
# 	0:8:Font.cpp:C:\mfc42\nonship\BindEnrl\Font.cpp
# 	0:25:rdoPreparedStatements.cpp:C:\mfc42\nonship\BindEnrl\rdoPreparedStatements.cpp
# 	0:12:rdoColumns.h:C:\mfc42\nonship\BindEnrl\rdoColumns.h
# 	0:13:rdoColumn.cpp:C:\mfc42\nonship\BindEnrl\rdoColumn.cpp
# 	0:14:rdoResultset.h:C:\mfc42\nonship\BindEnrl\rdoResultset.h
# 	0:6:Font.h:C:\mfc42\nonship\BindEnrl\Font.h
# 	0:15:rdoParameters.h:C:\mfc42\nonship\BindEnrl\rdoParameters.h
# 	0:11:rdoColumn.h:C:\mfc42\nonship\BindEnrl\rdoColumn.h
# 	0:10:rdoTable.h:C:\mfc42\nonship\BindEnrl\rdoTable.h
# 	0:8:McsDsc.h:C:\mfc42\nonship\BindEnrl\McsDsc.h
# 	0:23:rdoPreparedStatements.h:C:\mfc42\nonship\BindEnrl\rdoPreparedStatements.h
# 	0:17:rdoConnection.cpp:C:\mfc42\nonship\BindEnrl\rdoConnection.cpp
# 	0:18:rdoEnvironment.cpp:C:\mfc42\nonship\BindEnrl\rdoEnvironment.cpp
# 	0:15:rdoConnection.h:C:\mfc42\nonship\BindEnrl\rdoConnection.h
# 	0:17:rdoParameters.cpp:C:\mfc42\nonship\BindEnrl\rdoParameters.cpp
# 	0:11:rdoTables.h:C:\mfc42\nonship\BindEnrl\rdoTables.h
# 	0:14:rdoParameter.h:C:\mfc42\nonship\BindEnrl\rdoParameter.h
# 	0:14:rdoColumns.cpp:C:\mfc42\nonship\BindEnrl\rdoColumns.cpp
# 	0:10:McsDsc.cpp:C:\mfc42\nonship\BindEnrl\McsDsc.cpp
# 	0:18:rdoConnections.cpp:C:\mfc42\nonship\BindEnrl\rdoConnections.cpp
# 	0:12:rdoTable.cpp:C:\mfc42\nonship\BindEnrl\rdoTable.cpp
# 	0:16:rdoConnections.h:C:\mfc42\nonship\BindEnrl\rdoConnections.h
# 	0:16:rdoEnvironment.h:C:\mfc42\nonship\BindEnrl\rdoEnvironment.h
# 	0:13:rdoTables.cpp:C:\mfc42\nonship\BindEnrl\rdoTables.cpp
# 	0:24:rdoPreparedStatement.cpp:C:\mfc42\nonship\BindEnrl\rdoPreparedStatement.cpp
# 	0:17:rdoResultsets.cpp:C:\mfc42\nonship\BindEnrl\rdoResultsets.cpp
# 	0:16:rdoResultset.cpp:C:\mfc42\nonship\BindEnrl\rdoResultset.cpp
# 	0:15:rdoResultsets.h:C:\mfc42\nonship\BindEnrl\rdoResultsets.h
# 	0:16:rdoParameter.cpp:C:\mfc42\nonship\BindEnrl\rdoParameter.cpp
# 	0:22:rdoPreparedStatement.h:C:\mfc42\nonship\BindEnrl\rdoPreparedStatement.h
# 	2:21:DefaultSinkHeaderFile:mcsdsc.h
# 	2:16:DefaultSinkClass:CMcsDsc
# End Section
################################################################################
################################################################################
# Section BindEnrl : {02A69B01-081B-101B-8933-08002B2F4F5A}
# 	2:5:Class:CLbox1Ctrl
# 	2:10:HeaderFile:lboxctrl.h
# 	2:8:ImplFile:lboxctrl.cpp
# End Section
################################################################################
################################################################################
# Section BindEnrl : {B541C03A-63BC-11CE-920C-08002B369A33}
# 	2:5:Class:CrdoColumn
# 	2:10:HeaderFile:rdocol.h
# 	2:8:ImplFile:rdocol.cpp
# End Section
################################################################################
################################################################################
# Section BindEnrl : {2E746492-6ED1-11CE-9223-08002B369A33}
# 	2:5:Class:CrdoColumns
# 	2:10:HeaderFile:rdocols.h
# 	2:8:ImplFile:rdocols.cpp
# End Section
################################################################################
################################################################################
# Section OLE Controls
# 	{4F5F826A-8AAF-11CE-A77F-08002B2F4E98}
# 	{C932BA85-4374-101B-A56C-00AA003668DC}
# 	{FAEEE760-117E-101B-8933-08002B2F4F5A}
# 	{02A69B00-081B-101B-8933-08002B2F4F5A}
# End Section
################################################################################
################################################################################
# Section BindEnrl : {02A69B00-081B-101B-8933-08002B2F4F5A}
# 	0:8:Font.cpp:C:\mfc42\nonship\BindEnrl\Font.cpp
# 	0:9:Picture.h:C:\mfc42\nonship\BindEnrl\Picture.h
# 	0:6:Font.h:C:\mfc42\nonship\BindEnrl\Font.h
# 	0:11:Picture.cpp:C:\mfc42\nonship\BindEnrl\Picture.cpp
# 	0:11:Lbox1Ctrl.h:C:\mfc42\nonship\BindEnrl\Lbox1Ctrl.h
# 	0:13:Lbox1Ctrl.cpp:C:\mfc42\nonship\BindEnrl\Lbox1Ctrl.cpp
# 	2:21:DefaultSinkHeaderFile:lboxctrl.h
# 	2:16:DefaultSinkClass:CLbox1Ctrl
# End Section
################################################################################
################################################################################
# Section BindEnrl : {C932BA86-4374-101B-A56C-00AA003668DC}
# 	2:5:Class:CMsmask
# 	2:10:HeaderFile:msmask.h
# 	2:8:ImplFile:msmask.cpp
# End Section
################################################################################
################################################################################
# Section BindEnrl : {B541C03D-63BC-11CE-920C-08002B369A33}
# 	2:5:Class:CrdoTable
# 	2:10:HeaderFile:rdotable.h
# 	2:8:ImplFile:rdotable.cpp
# End Section
################################################################################
################################################################################
# Section BindEnrl : {4F5F8265-8AAF-11CE-A77F-08002B2F4E98}
# 	2:5:Class:CMcsDsc
# 	2:10:HeaderFile:mcsdsc.h
# 	2:8:ImplFile:mcsdsc.cpp
# End Section
################################################################################
################################################################################
# Section BindEnrl : {C932BA85-4374-101B-A56C-00AA003668DC}
# 	0:9:Picture.h:C:\mfc42\nonship\BindEnrl\Picture.h
# 	0:8:Font.cpp:C:\mfc42\nonship\BindEnrl\Font.cpp
# 	0:6:Font.h:C:\mfc42\nonship\BindEnrl\Font.h
# 	0:11:Picture.cpp:C:\mfc42\nonship\BindEnrl\Picture.cpp
# 	0:8:Msmask.h:C:\mfc42\nonship\BindEnrl\Msmask.h
# 	0:10:Msmask.cpp:C:\mfc42\nonship\BindEnrl\Msmask.cpp
# 	2:21:DefaultSinkHeaderFile:msmask.h
# 	2:16:DefaultSinkClass:CMsmask
# End Section
################################################################################
