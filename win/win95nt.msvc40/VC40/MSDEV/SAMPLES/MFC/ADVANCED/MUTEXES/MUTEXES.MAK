# Microsoft Visual C++ Generated NMAKE File, Format Version 40000
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

!IF "$(CFG)" == ""
CFG=mutexes - Win32 Debug
!MESSAGE No configuration specified.  Defaulting to mutexes - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "mutexes - Win32 Release" && "$(CFG)" !=\
 "mutexes - Win32 Debug" && "$(CFG)" != "mutexes - Win32 Unicode Release" &&\
 "$(CFG)" != "mutexes - Win32 Unicode Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "MUTEXES.MAK" CFG="mutexes - Win32 Unicode Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "mutexes - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "mutexes - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE "mutexes - Win32 Unicode Release" (based on "Win32 (x86) Application")
!MESSAGE "mutexes - Win32 Unicode Debug" (based on "Win32 (x86) Application")
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
# PROP Target_Last_Scanned "mutexes - Win32 Debug"
CPP=cl.exe
RSC=rc.exe
MTL=mktyplib.exe

!IF  "$(CFG)" == "mutexes - Win32 Release"

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

ALL : "$(OUTDIR)\mutexes.exe" "$(OUTDIR)\mutexes.bsc"

CLEAN : 
	-@erase ".\Release\mutexes.bsc"
	-@erase ".\Release\MUTEXDLG.SBR"
	-@erase ".\Release\mutexes.pch"
	-@erase ".\Release\MUTEXES.SBR"
	-@erase ".\Release\THREADS.SBR"
	-@erase ".\Release\STDAFX.SBR"
	-@erase ".\Release\mutexes.exe"
	-@erase ".\Release\STDAFX.OBJ"
	-@erase ".\Release\MUTEXDLG.OBJ"
	-@erase ".\Release\MUTEXES.OBJ"
	-@erase ".\Release\THREADS.OBJ"
	-@erase ".\Release\mutexes.res"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /Yu"stdafx.h" /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /Yu"stdafx.h" /c
CPP_PROJ=/nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /FR"$(INTDIR)/" /Fp"$(INTDIR)/mutexes.pch"\
 /Yu"stdafx.h" /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\Release/
CPP_SBRS=.\Release/
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/mutexes.res" /d "NDEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/mutexes.bsc" 
BSC32_SBRS= \
	"$(INTDIR)/MUTEXDLG.SBR" \
	"$(INTDIR)/MUTEXES.SBR" \
	"$(INTDIR)/THREADS.SBR" \
	"$(INTDIR)/STDAFX.SBR"

"$(OUTDIR)\mutexes.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 /nologo /subsystem:windows /machine:I386
LINK32_FLAGS=/nologo /subsystem:windows /incremental:no\
 /pdb:"$(OUTDIR)/mutexes.pdb" /machine:I386\
 /out:"$(OUTDIR)/mutexes.exe" 
LINK32_OBJS= \
	"$(INTDIR)/STDAFX.OBJ" \
	"$(INTDIR)/MUTEXDLG.OBJ" \
	"$(INTDIR)/MUTEXES.OBJ" \
	"$(INTDIR)/THREADS.OBJ" \
	"$(INTDIR)/mutexes.res"

"$(OUTDIR)\mutexes.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "mutexes - Win32 Debug"

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

ALL : "$(OUTDIR)\mutexes.exe" "$(OUTDIR)\mutexes.bsc"

CLEAN : 
	-@erase ".\Debug\vc40.pdb"
	-@erase ".\Debug\mutexes.pch"
	-@erase ".\Debug\mutexes.bsc"
	-@erase ".\Debug\MUTEXES.SBR"
	-@erase ".\Debug\THREADS.SBR"
	-@erase ".\Debug\MUTEXDLG.SBR"
	-@erase ".\Debug\STDAFX.SBR"
	-@erase ".\Debug\mutexes.exe"
	-@erase ".\Debug\MUTEXDLG.OBJ"
	-@erase ".\Debug\STDAFX.OBJ"
	-@erase ".\Debug\MUTEXES.OBJ"
	-@erase ".\Debug\THREADS.OBJ"
	-@erase ".\Debug\mutexes.res"
	-@erase ".\Debug\mutexes.ilk"
	-@erase ".\Debug\mutexes.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /Yu"stdafx.h" /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /Yu"stdafx.h" /c
CPP_PROJ=/nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /D "_AFXDLL" /D "_MBCS" /FR"$(INTDIR)/" /Fp"$(INTDIR)/mutexes.pch"\
 /Yu"stdafx.h" /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\Debug/
CPP_SBRS=.\Debug/
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/mutexes.res" /d "_DEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/mutexes.bsc" 
BSC32_SBRS= \
	"$(INTDIR)/MUTEXES.SBR" \
	"$(INTDIR)/THREADS.SBR" \
	"$(INTDIR)/MUTEXDLG.SBR" \
	"$(INTDIR)/STDAFX.SBR"

"$(OUTDIR)\mutexes.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386
# ADD LINK32 /nologo /subsystem:windows /debug /machine:I386
LINK32_FLAGS=/nologo /subsystem:windows /incremental:yes\
 /pdb:"$(OUTDIR)/mutexes.pdb" /debug /machine:I386\
 /out:"$(OUTDIR)/mutexes.exe" 
LINK32_OBJS= \
	"$(INTDIR)/MUTEXDLG.OBJ" \
	"$(INTDIR)/STDAFX.OBJ" \
	"$(INTDIR)/MUTEXES.OBJ" \
	"$(INTDIR)/THREADS.OBJ" \
	"$(INTDIR)/mutexes.res"

"$(OUTDIR)\mutexes.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "mutexes - Win32 Unicode Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir Unicode Release\Release
# PROP BASE Intermediate_Dir Unicode Release\Release
# PROP BASE Target_Dir
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ReleaseU
# PROP Intermediate_Dir ReleaseU
# PROP Target_Dir
OUTDIR=.\ReleaseU
INTDIR=.\ReleaseU

ALL : "$(OUTDIR)\mutexes.exe" "$(OUTDIR)\mutexes.bsc"

CLEAN : 
	-@erase ".\ReleaseU\mutexes.bsc"
	-@erase ".\ReleaseU\MUTEXES.SBR"
	-@erase ".\ReleaseU\mutexes.pch"
	-@erase ".\ReleaseU\THREADS.SBR"
	-@erase ".\ReleaseU\STDAFX.SBR"
	-@erase ".\ReleaseU\MUTEXDLG.SBR"
	-@erase ".\ReleaseU\mutexes.exe"
	-@erase ".\ReleaseU\STDAFX.OBJ"
	-@erase ".\ReleaseU\MUTEXDLG.OBJ"
	-@erase ".\ReleaseU\MUTEXES.OBJ"
	-@erase ".\ReleaseU\THREADS.OBJ"
	-@erase ".\ReleaseU\mutexes.res"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /Yu"stdafx.h" /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_AFXDLL" /D "_UNICODE" /FR /Yu"stdafx.h" /c
CPP_PROJ=/nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_UNICODE" /FR"$(INTDIR)/" /Fp"$(INTDIR)/mutexes.pch"\
 /Yu"stdafx.h" /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\ReleaseU/
CPP_SBRS=.\ReleaseU/
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/mutexes.res" /d "NDEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/mutexes.bsc" 
BSC32_SBRS= \
	"$(INTDIR)/MUTEXES.SBR" \
	"$(INTDIR)/THREADS.SBR" \
	"$(INTDIR)/STDAFX.SBR" \
	"$(INTDIR)/MUTEXDLG.SBR"

"$(OUTDIR)\mutexes.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 /nologo /entry:"wWinMainCRTStartup" /subsystem:windows /machine:I386
LINK32_FLAGS=/nologo /entry:"wWinMainCRTStartup" /subsystem:windows\
 /incremental:no /pdb:"$(OUTDIR)/mutexes.pdb" /machine:I386\
 /out:"$(OUTDIR)/mutexes.exe" 
LINK32_OBJS= \
	"$(INTDIR)/STDAFX.OBJ" \
	"$(INTDIR)/MUTEXDLG.OBJ" \
	"$(INTDIR)/MUTEXES.OBJ" \
	"$(INTDIR)/THREADS.OBJ" \
	"$(INTDIR)/mutexes.res"

"$(OUTDIR)\mutexes.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "mutexes - Win32 Unicode Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir Unicode Debug
# PROP BASE Intermediate_Dir Unicode Debug
# PROP BASE Target_Dir
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir DebugU
# PROP Intermediate_Dir DebugU
# PROP Target_Dir
OUTDIR=.\DebugU
INTDIR=.\DebugU

ALL : "$(OUTDIR)\mutexes.exe" "$(OUTDIR)\mutexes.bsc"

CLEAN : 
	-@erase ".\DebugU\vc40.pdb"
	-@erase ".\DebugU\mutexes.pch"
	-@erase ".\DebugU\mutexes.bsc"
	-@erase ".\DebugU\MUTEXDLG.SBR"
	-@erase ".\DebugU\THREADS.SBR"
	-@erase ".\DebugU\STDAFX.SBR"
	-@erase ".\DebugU\MUTEXES.SBR"
	-@erase ".\DebugU\mutexes.exe"
	-@erase ".\DebugU\STDAFX.OBJ"
	-@erase ".\DebugU\MUTEXES.OBJ"
	-@erase ".\DebugU\MUTEXDLG.OBJ"
	-@erase ".\DebugU\THREADS.OBJ"
	-@erase ".\DebugU\mutexes.res"
	-@erase ".\DebugU\mutexes.ilk"
	-@erase ".\DebugU\mutexes.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /Yu"stdafx.h" /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_AFXDLL" /D "_UNICODE" /FR /Yu"stdafx.h" /c
CPP_PROJ=/nologo /MDd /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS"\
 /D "_AFXDLL" /D "_UNICODE" /FR"$(INTDIR)/" /Fp"$(INTDIR)/mutexes.pch"\
 /Yu"stdafx.h" /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\DebugU/
CPP_SBRS=.\DebugU/
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/mutexes.res" /d "_DEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/mutexes.bsc" 
BSC32_SBRS= \
	"$(INTDIR)/MUTEXDLG.SBR" \
	"$(INTDIR)/THREADS.SBR" \
	"$(INTDIR)/STDAFX.SBR" \
	"$(INTDIR)/MUTEXES.SBR"

"$(OUTDIR)\mutexes.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386
# ADD LINK32 /nologo /entry:"wWinMainCRTStartup" /subsystem:windows /debug /machine:I386
LINK32_FLAGS=/nologo /entry:"wWinMainCRTStartup" /subsystem:windows\
 /incremental:yes /pdb:"$(OUTDIR)/mutexes.pdb" /debug /machine:I386\
 /out:"$(OUTDIR)/mutexes.exe" 
LINK32_OBJS= \
	"$(INTDIR)/STDAFX.OBJ" \
	"$(INTDIR)/MUTEXES.OBJ" \
	"$(INTDIR)/MUTEXDLG.OBJ" \
	"$(INTDIR)/THREADS.OBJ" \
	"$(INTDIR)/mutexes.res"

"$(OUTDIR)\mutexes.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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

# Name "mutexes - Win32 Release"
# Name "mutexes - Win32 Debug"
# Name "mutexes - Win32 Unicode Release"
# Name "mutexes - Win32 Unicode Debug"

!IF  "$(CFG)" == "mutexes - Win32 Release"

!ELSEIF  "$(CFG)" == "mutexes - Win32 Debug"

!ELSEIF  "$(CFG)" == "mutexes - Win32 Unicode Release"

!ELSEIF  "$(CFG)" == "mutexes - Win32 Unicode Debug"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\readme.txt

!IF  "$(CFG)" == "mutexes - Win32 Release"

!ELSEIF  "$(CFG)" == "mutexes - Win32 Debug"

!ELSEIF  "$(CFG)" == "mutexes - Win32 Unicode Release"

!ELSEIF  "$(CFG)" == "mutexes - Win32 Unicode Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\MUTEXES.CPP
DEP_CPP_MUTEX=\
	".\STDAFX.H"\
	".\MUTEXES.H"\
	".\MUTEXDLG.H"\
	

!IF  "$(CFG)" == "mutexes - Win32 Release"


"$(INTDIR)\MUTEXES.OBJ" : $(SOURCE) $(DEP_CPP_MUTEX) "$(INTDIR)"\
 "$(INTDIR)\mutexes.pch"

"$(INTDIR)\MUTEXES.SBR" : $(SOURCE) $(DEP_CPP_MUTEX) "$(INTDIR)"\
 "$(INTDIR)\mutexes.pch"


!ELSEIF  "$(CFG)" == "mutexes - Win32 Debug"


"$(INTDIR)\MUTEXES.OBJ" : $(SOURCE) $(DEP_CPP_MUTEX) "$(INTDIR)"\
 "$(INTDIR)\mutexes.pch"

"$(INTDIR)\MUTEXES.SBR" : $(SOURCE) $(DEP_CPP_MUTEX) "$(INTDIR)"\
 "$(INTDIR)\mutexes.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_MUTEX) "$(INTDIR)"\
 "$(INTDIR)\mutexes.pch"


!ELSEIF  "$(CFG)" == "mutexes - Win32 Unicode Release"


"$(INTDIR)\MUTEXES.OBJ" : $(SOURCE) $(DEP_CPP_MUTEX) "$(INTDIR)"\
 "$(INTDIR)\mutexes.pch"

"$(INTDIR)\MUTEXES.SBR" : $(SOURCE) $(DEP_CPP_MUTEX) "$(INTDIR)"\
 "$(INTDIR)\mutexes.pch"


!ELSEIF  "$(CFG)" == "mutexes - Win32 Unicode Debug"


"$(INTDIR)\MUTEXES.OBJ" : $(SOURCE) $(DEP_CPP_MUTEX) "$(INTDIR)"\
 "$(INTDIR)\mutexes.pch"

"$(INTDIR)\MUTEXES.SBR" : $(SOURCE) $(DEP_CPP_MUTEX) "$(INTDIR)"\
 "$(INTDIR)\mutexes.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_MUTEX) "$(INTDIR)"\
 "$(INTDIR)\mutexes.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\STDAFX.CPP
DEP_CPP_STDAF=\
	".\STDAFX.H"\
	

!IF  "$(CFG)" == "mutexes - Win32 Release"

# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /FR"$(INTDIR)/" /Fp"$(INTDIR)/mutexes.pch"\
 /Yc"stdafx.h" /Fo"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\STDAFX.OBJ" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\STDAFX.SBR" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\mutexes.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "mutexes - Win32 Debug"

# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /D "_AFXDLL" /D "_MBCS" /FR"$(INTDIR)/" /Fp"$(INTDIR)/mutexes.pch"\
 /Yc"stdafx.h" /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\STDAFX.OBJ" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\STDAFX.SBR" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\mutexes.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "mutexes - Win32 Unicode Release"

# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_UNICODE" /FR"$(INTDIR)/" /Fp"$(INTDIR)/mutexes.pch"\
 /Yc"stdafx.h" /Fo"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\STDAFX.OBJ" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\STDAFX.SBR" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\mutexes.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "mutexes - Win32 Unicode Debug"

# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MDd /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS"\
 /D "_AFXDLL" /D "_UNICODE" /FR"$(INTDIR)/" /Fp"$(INTDIR)/mutexes.pch"\
 /Yc"stdafx.h" /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\STDAFX.OBJ" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\STDAFX.SBR" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\mutexes.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\mutexes.rc
DEP_RSC_MUTEXE=\
	".\res\mutexes.ico"\
	".\res\mutexes.rc2"\
	

!IF  "$(CFG)" == "mutexes - Win32 Release"


"$(INTDIR)\mutexes.res" : $(SOURCE) $(DEP_RSC_MUTEXE) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "mutexes - Win32 Debug"


"$(INTDIR)\mutexes.res" : $(SOURCE) $(DEP_RSC_MUTEXE) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "mutexes - Win32 Unicode Release"


"$(INTDIR)\mutexes.res" : $(SOURCE) $(DEP_RSC_MUTEXE) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "mutexes - Win32 Unicode Debug"


"$(INTDIR)\mutexes.res" : $(SOURCE) $(DEP_RSC_MUTEXE) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\THREADS.CPP
DEP_CPP_THREA=\
	".\STDAFX.H"\
	".\MUTEXES.H"\
	".\MUTEXDLG.H"\
	".\THREADS.H"\
	

!IF  "$(CFG)" == "mutexes - Win32 Release"


"$(INTDIR)\THREADS.OBJ" : $(SOURCE) $(DEP_CPP_THREA) "$(INTDIR)"\
 "$(INTDIR)\mutexes.pch"

"$(INTDIR)\THREADS.SBR" : $(SOURCE) $(DEP_CPP_THREA) "$(INTDIR)"\
 "$(INTDIR)\mutexes.pch"


!ELSEIF  "$(CFG)" == "mutexes - Win32 Debug"


"$(INTDIR)\THREADS.OBJ" : $(SOURCE) $(DEP_CPP_THREA) "$(INTDIR)"\
 "$(INTDIR)\mutexes.pch"

"$(INTDIR)\THREADS.SBR" : $(SOURCE) $(DEP_CPP_THREA) "$(INTDIR)"\
 "$(INTDIR)\mutexes.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_THREA) "$(INTDIR)"\
 "$(INTDIR)\mutexes.pch"


!ELSEIF  "$(CFG)" == "mutexes - Win32 Unicode Release"


"$(INTDIR)\THREADS.OBJ" : $(SOURCE) $(DEP_CPP_THREA) "$(INTDIR)"\
 "$(INTDIR)\mutexes.pch"

"$(INTDIR)\THREADS.SBR" : $(SOURCE) $(DEP_CPP_THREA) "$(INTDIR)"\
 "$(INTDIR)\mutexes.pch"


!ELSEIF  "$(CFG)" == "mutexes - Win32 Unicode Debug"


"$(INTDIR)\THREADS.OBJ" : $(SOURCE) $(DEP_CPP_THREA) "$(INTDIR)"\
 "$(INTDIR)\mutexes.pch"

"$(INTDIR)\THREADS.SBR" : $(SOURCE) $(DEP_CPP_THREA) "$(INTDIR)"\
 "$(INTDIR)\mutexes.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_THREA) "$(INTDIR)"\
 "$(INTDIR)\mutexes.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\MUTEXDLG.CPP
DEP_CPP_MUTEXD=\
	".\MUTEXDLG.H"\
	".\MUTEXES.H"\
	".\STDAFX.H"\
	".\THREADS.H"\
	

!IF  "$(CFG)" == "mutexes - Win32 Release"


"$(INTDIR)\MUTEXDLG.OBJ" : $(SOURCE) $(DEP_CPP_MUTEXD) "$(INTDIR)"\
 "$(INTDIR)\mutexes.pch"

"$(INTDIR)\MUTEXDLG.SBR" : $(SOURCE) $(DEP_CPP_MUTEXD) "$(INTDIR)"\
 "$(INTDIR)\mutexes.pch"


!ELSEIF  "$(CFG)" == "mutexes - Win32 Debug"


"$(INTDIR)\MUTEXDLG.OBJ" : $(SOURCE) $(DEP_CPP_MUTEXD) "$(INTDIR)"\
 "$(INTDIR)\mutexes.pch"

"$(INTDIR)\MUTEXDLG.SBR" : $(SOURCE) $(DEP_CPP_MUTEXD) "$(INTDIR)"\
 "$(INTDIR)\mutexes.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_MUTEXD) "$(INTDIR)"\
 "$(INTDIR)\mutexes.pch"


!ELSEIF  "$(CFG)" == "mutexes - Win32 Unicode Release"


"$(INTDIR)\MUTEXDLG.OBJ" : $(SOURCE) $(DEP_CPP_MUTEXD) "$(INTDIR)"\
 "$(INTDIR)\mutexes.pch"

"$(INTDIR)\MUTEXDLG.SBR" : $(SOURCE) $(DEP_CPP_MUTEXD) "$(INTDIR)"\
 "$(INTDIR)\mutexes.pch"


!ELSEIF  "$(CFG)" == "mutexes - Win32 Unicode Debug"


"$(INTDIR)\MUTEXDLG.OBJ" : $(SOURCE) $(DEP_CPP_MUTEXD) "$(INTDIR)"\
 "$(INTDIR)\mutexes.pch"

"$(INTDIR)\MUTEXDLG.SBR" : $(SOURCE) $(DEP_CPP_MUTEXD) "$(INTDIR)"\
 "$(INTDIR)\mutexes.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_MUTEXD) "$(INTDIR)"\
 "$(INTDIR)\mutexes.pch"


!ENDIF 

# End Source File
# End Target
# End Project
################################################################################
