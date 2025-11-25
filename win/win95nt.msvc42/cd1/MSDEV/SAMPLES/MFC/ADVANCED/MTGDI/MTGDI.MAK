# Microsoft Visual C++ Generated NMAKE File, Format Version 40000
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

!IF "$(CFG)" == ""
CFG=MtGdi - Win32 Debug
!MESSAGE No configuration specified.  Defaulting to MtGdi - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "MtGdi - Win32 Release" && "$(CFG)" != "MtGdi - Win32 Debug" &&\
 "$(CFG)" != "MtGdi - Win32 Unicode Release" && "$(CFG)" !=\
 "MtGdi - Win32 Unicode Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "MtGdi.mak" CFG="MtGdi - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "MtGdi - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "MtGdi - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE "MtGdi - Win32 Unicode Release" (based on "Win32 (x86) Application")
!MESSAGE "MtGdi - Win32 Unicode Debug" (based on "Win32 (x86) Application")
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
# PROP Target_Last_Scanned "MtGdi - Win32 Debug"
CPP=cl.exe
MTL=mktyplib.exe
RSC=rc.exe

!IF  "$(CFG)" == "MtGdi - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir Release
# PROP BASE Intermediate_Dir Release
# PROP BASE Target_Dir
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 0
# PROP Output_Dir Release
# PROP Intermediate_Dir Release
# PROP Target_Dir
OUTDIR=.\Release
INTDIR=.\Release

ALL : ".\Release\MtGdi.exe"

CLEAN : 
	-@erase ".\Release\MtGdi.idb"
	-@erase ".\Release\MtGdi.pch"
	-@erase ".\Release\MtGdi.exe"
	-@erase ".\Release\Mtgdi.obj"
	-@erase ".\Release\Threads.obj"
	-@erase ".\Release\Mtgdivw.obj"
	-@erase ".\Release\Mtgdidoc.obj"
	-@erase ".\Release\Mainfrm.obj"
	-@erase ".\Release\Stdafx.obj"
	-@erase ".\Release\Mtgdi.res"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"Stdafx.h" /c
CPP_PROJ=/nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/MtGdi.pch" /Yu"Stdafx.h" /Fo"$(INTDIR)/"\
 /Fd"$(OUTDIR)/MtGdi.pdb" /c 
CPP_OBJS=.\Release/
CPP_SBRS=
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/Mtgdi.res" /d "NDEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/MtGdi.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 /nologo /subsystem:windows /machine:I386
LINK32_FLAGS=/nologo /subsystem:windows /incremental:no\
 /pdb:"$(OUTDIR)/MtGdi.pdb" /machine:I386 /out:"$(OUTDIR)/MtGdi.exe" 
LINK32_OBJS= \
	".\Release\Mtgdi.obj" \
	".\Release\Threads.obj" \
	".\Release\Mtgdivw.obj" \
	".\Release\Mtgdidoc.obj" \
	".\Release\Mainfrm.obj" \
	".\Release\Stdafx.obj" \
	".\Release\Mtgdi.res"

".\Release\MtGdi.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "MtGdi - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir Debug
# PROP BASE Intermediate_Dir Debug
# PROP BASE Target_Dir
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 1
# PROP Output_Dir Debug
# PROP Intermediate_Dir Debug
# PROP Target_Dir
OUTDIR=.\Debug
INTDIR=.\Debug

ALL : ".\Debug\MtGdi.exe"

CLEAN : 
	-@erase ".\Debug\MtGdi.pdb"
	-@erase ".\Debug\MtGdi.pch"
	-@erase ".\Debug\Mtgdidoc.obj"
	-@erase ".\Debug\Stdafx.obj"
	-@erase ".\Debug\Mtgdi.obj"
	-@erase ".\Debug\Threads.obj"
	-@erase ".\Debug\Mtgdivw.obj"
	-@erase ".\Debug\Mainfrm.obj"
	-@erase ".\Debug\Mtgdi.res"
	-@erase ".\Debug\MtGdi.idb"
	-@erase ".\Debug\MtGdi.exe"
	-@erase ".\Debug\MtGdi.ilk"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"Stdafx.h" /c
CPP_PROJ=/nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/MtGdi.pch" /Yu"Stdafx.h" /Fo"$(INTDIR)/"\
 /Fd"$(OUTDIR)/MtGdi.pdb" /c 
CPP_OBJS=.\Debug/
CPP_SBRS=
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/Mtgdi.res" /d "_DEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/MtGdi.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386
# ADD LINK32 /nologo /subsystem:windows /debug /machine:I386
LINK32_FLAGS=/nologo /subsystem:windows /incremental:yes\
 /pdb:"$(OUTDIR)/MtGdi.pdb" /debug /machine:I386 /out:"$(OUTDIR)/MtGdi.exe" 
LINK32_OBJS= \
	".\Debug\Mtgdidoc.obj" \
	".\Debug\Stdafx.obj" \
	".\Debug\Mtgdi.obj" \
	".\Debug\Threads.obj" \
	".\Debug\Mtgdivw.obj" \
	".\Debug\Mainfrm.obj" \
	".\Debug\Mtgdi.res"

".\Debug\MtGdi.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "MtGdi - Win32 Unicode Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir Release
# PROP BASE Intermediate_Dir Release
# PROP BASE Target_Dir
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 0
# PROP Output_Dir UniRelease
# PROP Intermediate_Dir UniRelease
# PROP Target_Dir
OUTDIR=.\UniRelease
INTDIR=.\UniRelease

ALL : ".\UniRelease\MtGdi.exe"

CLEAN : 
	-@erase ".\UniRelease\MtGdi.idb"
	-@erase ".\UniRelease\MtGdi.pch"
	-@erase ".\UniRelease\MtGdi.exe"
	-@erase ".\UniRelease\Mtgdivw.obj"
	-@erase ".\UniRelease\Stdafx.obj"
	-@erase ".\UniRelease\Mainfrm.obj"
	-@erase ".\UniRelease\Mtgdidoc.obj"
	-@erase ".\UniRelease\Mtgdi.obj"
	-@erase ".\UniRelease\Threads.obj"
	-@erase ".\UniRelease\Mtgdi.res"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_UNICODE" /D "_AFXDLL" /D "_MBCS" /Yu"Stdafx.h" /c
CPP_PROJ=/nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D\
 "_UNICODE" /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/MtGdi.pch" /Yu"Stdafx.h"\
 /Fo"$(INTDIR)/" /Fd"$(OUTDIR)/MtGdi.pdb" /c 
CPP_OBJS=.\UniRelease/
CPP_SBRS=
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/Mtgdi.res" /d "NDEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/MtGdi.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 /nologo /entry:"wWinMainCRTStartup" /subsystem:windows /machine:I386
LINK32_FLAGS=/nologo /entry:"wWinMainCRTStartup" /subsystem:windows\
 /incremental:no /pdb:"$(OUTDIR)/MtGdi.pdb" /machine:I386\
 /out:"$(OUTDIR)/MtGdi.exe" 
LINK32_OBJS= \
	".\UniRelease\Mtgdivw.obj" \
	".\UniRelease\Stdafx.obj" \
	".\UniRelease\Mainfrm.obj" \
	".\UniRelease\Mtgdidoc.obj" \
	".\UniRelease\Mtgdi.obj" \
	".\UniRelease\Threads.obj" \
	".\UniRelease\Mtgdi.res"

".\UniRelease\MtGdi.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "MtGdi - Win32 Unicode Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir Debug
# PROP BASE Intermediate_Dir Debug
# PROP BASE Target_Dir
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 1
# PROP Output_Dir UniDebug
# PROP Intermediate_Dir UniDebug
# PROP Target_Dir
OUTDIR=.\UniDebug
INTDIR=.\UniDebug

ALL : ".\UniDebug\MtGdi.exe"

CLEAN : 
	-@erase ".\UniDebug\MtGdi.pdb"
	-@erase ".\UniDebug\MtGdi.pch"
	-@erase ".\UniDebug\Threads.obj"
	-@erase ".\UniDebug\Mtgdidoc.obj"
	-@erase ".\UniDebug\Mtgdivw.obj"
	-@erase ".\UniDebug\Mainfrm.obj"
	-@erase ".\UniDebug\Stdafx.obj"
	-@erase ".\UniDebug\Mtgdi.obj"
	-@erase ".\UniDebug\Mtgdi.res"
	-@erase ".\UniDebug\MtGdi.idb"
	-@erase ".\UniDebug\MtGdi.exe"
	-@erase ".\UniDebug\MtGdi.ilk"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_UNICODE" /D "_AFXDLL" /D "_MBCS" /Yu"Stdafx.h" /c
CPP_PROJ=/nologo /MDd /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS"\
 /D "_UNICODE" /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/MtGdi.pch" /Yu"Stdafx.h"\
 /Fo"$(INTDIR)/" /Fd"$(OUTDIR)/MtGdi.pdb" /c 
CPP_OBJS=.\UniDebug/
CPP_SBRS=
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/Mtgdi.res" /d "_DEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/MtGdi.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386
# ADD LINK32 /nologo /entry:"wWinMainCRTStartup" /subsystem:windows /debug /machine:I386
LINK32_FLAGS=/nologo /entry:"wWinMainCRTStartup" /subsystem:windows\
 /incremental:yes /pdb:"$(OUTDIR)/MtGdi.pdb" /debug /machine:I386\
 /out:"$(OUTDIR)/MtGdi.exe" 
LINK32_OBJS= \
	".\UniDebug\Threads.obj" \
	".\UniDebug\Mtgdidoc.obj" \
	".\UniDebug\Mtgdivw.obj" \
	".\UniDebug\Mainfrm.obj" \
	".\UniDebug\Stdafx.obj" \
	".\UniDebug\Mtgdi.obj" \
	".\UniDebug\Mtgdi.res"

".\UniDebug\MtGdi.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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

# Name "MtGdi - Win32 Release"
# Name "MtGdi - Win32 Debug"
# Name "MtGdi - Win32 Unicode Release"
# Name "MtGdi - Win32 Unicode Debug"

!IF  "$(CFG)" == "MtGdi - Win32 Release"

!ELSEIF  "$(CFG)" == "MtGdi - Win32 Debug"

!ELSEIF  "$(CFG)" == "MtGdi - Win32 Unicode Release"

!ELSEIF  "$(CFG)" == "MtGdi - Win32 Unicode Debug"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\Threads.cpp
DEP_CPP_THREA=\
	".\Mtgdi.h"\
	".\Resource.h"\
	".\Stdafx.h"\
	".\Threads.h"\
	

!IF  "$(CFG)" == "MtGdi - Win32 Release"


".\Release\Threads.obj" : $(SOURCE) $(DEP_CPP_THREA) "$(INTDIR)"\
 ".\Release\MtGdi.pch"

".\Release\MtGdi.idb" : $(SOURCE) $(DEP_CPP_THREA) "$(INTDIR)"\
 ".\Release\MtGdi.pch"


!ELSEIF  "$(CFG)" == "MtGdi - Win32 Debug"


".\Debug\Threads.obj" : $(SOURCE) $(DEP_CPP_THREA) "$(INTDIR)"\
 ".\Debug\MtGdi.pch"

".\Debug\MtGdi.pdb" : $(SOURCE) $(DEP_CPP_THREA) "$(INTDIR)"\
 ".\Debug\MtGdi.pch"

".\Debug\MtGdi.idb" : $(SOURCE) $(DEP_CPP_THREA) "$(INTDIR)"\
 ".\Debug\MtGdi.pch"


!ELSEIF  "$(CFG)" == "MtGdi - Win32 Unicode Release"


".\UniRelease\Threads.obj" : $(SOURCE) $(DEP_CPP_THREA) "$(INTDIR)"\
 ".\UniRelease\MtGdi.pch"

".\UniRelease\MtGdi.idb" : $(SOURCE) $(DEP_CPP_THREA) "$(INTDIR)"\
 ".\UniRelease\MtGdi.pch"


!ELSEIF  "$(CFG)" == "MtGdi - Win32 Unicode Debug"


".\UniDebug\Threads.obj" : $(SOURCE) $(DEP_CPP_THREA) "$(INTDIR)"\
 ".\UniDebug\MtGdi.pch"

".\UniDebug\MtGdi.pdb" : $(SOURCE) $(DEP_CPP_THREA) "$(INTDIR)"\
 ".\UniDebug\MtGdi.pch"

".\UniDebug\MtGdi.idb" : $(SOURCE) $(DEP_CPP_THREA) "$(INTDIR)"\
 ".\UniDebug\MtGdi.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Mtgdi.cpp
DEP_CPP_MTGDI=\
	".\Mainfrm.h"\
	".\Mtgdi.h"\
	".\Resource.h"\
	".\Mtgdidoc.h"\
	".\Mtgdivw.h"\
	".\Threads.h"\
	".\Stdafx.h"\
	

!IF  "$(CFG)" == "MtGdi - Win32 Release"


".\Release\Mtgdi.obj" : $(SOURCE) $(DEP_CPP_MTGDI) "$(INTDIR)"\
 ".\Release\MtGdi.pch"

".\Release\MtGdi.idb" : $(SOURCE) $(DEP_CPP_MTGDI) "$(INTDIR)"\
 ".\Release\MtGdi.pch"


!ELSEIF  "$(CFG)" == "MtGdi - Win32 Debug"


".\Debug\Mtgdi.obj" : $(SOURCE) $(DEP_CPP_MTGDI) "$(INTDIR)"\
 ".\Debug\MtGdi.pch"

".\Debug\MtGdi.pdb" : $(SOURCE) $(DEP_CPP_MTGDI) "$(INTDIR)"\
 ".\Debug\MtGdi.pch"

".\Debug\MtGdi.idb" : $(SOURCE) $(DEP_CPP_MTGDI) "$(INTDIR)"\
 ".\Debug\MtGdi.pch"


!ELSEIF  "$(CFG)" == "MtGdi - Win32 Unicode Release"


".\UniRelease\Mtgdi.obj" : $(SOURCE) $(DEP_CPP_MTGDI) "$(INTDIR)"\
 ".\UniRelease\MtGdi.pch"

".\UniRelease\MtGdi.idb" : $(SOURCE) $(DEP_CPP_MTGDI) "$(INTDIR)"\
 ".\UniRelease\MtGdi.pch"


!ELSEIF  "$(CFG)" == "MtGdi - Win32 Unicode Debug"


".\UniDebug\Mtgdi.obj" : $(SOURCE) $(DEP_CPP_MTGDI) "$(INTDIR)"\
 ".\UniDebug\MtGdi.pch"

".\UniDebug\MtGdi.pdb" : $(SOURCE) $(DEP_CPP_MTGDI) "$(INTDIR)"\
 ".\UniDebug\MtGdi.pch"

".\UniDebug\MtGdi.idb" : $(SOURCE) $(DEP_CPP_MTGDI) "$(INTDIR)"\
 ".\UniDebug\MtGdi.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Mtgdi.rc
DEP_RSC_MTGDI_=\
	".\Res\Mtgdi.ico"\
	".\Res\Mtgdi.rc2"\
	

!IF  "$(CFG)" == "MtGdi - Win32 Release"


".\Release\Mtgdi.res" : $(SOURCE) $(DEP_RSC_MTGDI_) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "MtGdi - Win32 Debug"


".\Debug\Mtgdi.res" : $(SOURCE) $(DEP_RSC_MTGDI_) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "MtGdi - Win32 Unicode Release"


".\UniRelease\Mtgdi.res" : $(SOURCE) $(DEP_RSC_MTGDI_) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "MtGdi - Win32 Unicode Debug"


".\UniDebug\Mtgdi.res" : $(SOURCE) $(DEP_RSC_MTGDI_) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Mtgdidoc.cpp
DEP_CPP_MTGDID=\
	".\Mtgdi.h"\
	".\Resource.h"\
	".\Mtgdidoc.h"\
	".\Stdafx.h"\
	

!IF  "$(CFG)" == "MtGdi - Win32 Release"


".\Release\Mtgdidoc.obj" : $(SOURCE) $(DEP_CPP_MTGDID) "$(INTDIR)"\
 ".\Release\MtGdi.pch"

".\Release\MtGdi.idb" : $(SOURCE) $(DEP_CPP_MTGDID) "$(INTDIR)"\
 ".\Release\MtGdi.pch"


!ELSEIF  "$(CFG)" == "MtGdi - Win32 Debug"


".\Debug\Mtgdidoc.obj" : $(SOURCE) $(DEP_CPP_MTGDID) "$(INTDIR)"\
 ".\Debug\MtGdi.pch"

".\Debug\MtGdi.pdb" : $(SOURCE) $(DEP_CPP_MTGDID) "$(INTDIR)"\
 ".\Debug\MtGdi.pch"

".\Debug\MtGdi.idb" : $(SOURCE) $(DEP_CPP_MTGDID) "$(INTDIR)"\
 ".\Debug\MtGdi.pch"


!ELSEIF  "$(CFG)" == "MtGdi - Win32 Unicode Release"


".\UniRelease\Mtgdidoc.obj" : $(SOURCE) $(DEP_CPP_MTGDID) "$(INTDIR)"\
 ".\UniRelease\MtGdi.pch"

".\UniRelease\MtGdi.idb" : $(SOURCE) $(DEP_CPP_MTGDID) "$(INTDIR)"\
 ".\UniRelease\MtGdi.pch"


!ELSEIF  "$(CFG)" == "MtGdi - Win32 Unicode Debug"


".\UniDebug\Mtgdidoc.obj" : $(SOURCE) $(DEP_CPP_MTGDID) "$(INTDIR)"\
 ".\UniDebug\MtGdi.pch"

".\UniDebug\MtGdi.pdb" : $(SOURCE) $(DEP_CPP_MTGDID) "$(INTDIR)"\
 ".\UniDebug\MtGdi.pch"

".\UniDebug\MtGdi.idb" : $(SOURCE) $(DEP_CPP_MTGDID) "$(INTDIR)"\
 ".\UniDebug\MtGdi.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Mtgdivw.cpp
DEP_CPP_MTGDIV=\
	".\Mtgdi.h"\
	".\Resource.h"\
	".\Mtgdidoc.h"\
	".\Mtgdivw.h"\
	".\Threads.h"\
	".\Stdafx.h"\
	

!IF  "$(CFG)" == "MtGdi - Win32 Release"


".\Release\Mtgdivw.obj" : $(SOURCE) $(DEP_CPP_MTGDIV) "$(INTDIR)"\
 ".\Release\MtGdi.pch"

".\Release\MtGdi.idb" : $(SOURCE) $(DEP_CPP_MTGDIV) "$(INTDIR)"\
 ".\Release\MtGdi.pch"


!ELSEIF  "$(CFG)" == "MtGdi - Win32 Debug"


".\Debug\Mtgdivw.obj" : $(SOURCE) $(DEP_CPP_MTGDIV) "$(INTDIR)"\
 ".\Debug\MtGdi.pch"

".\Debug\MtGdi.pdb" : $(SOURCE) $(DEP_CPP_MTGDIV) "$(INTDIR)"\
 ".\Debug\MtGdi.pch"

".\Debug\MtGdi.idb" : $(SOURCE) $(DEP_CPP_MTGDIV) "$(INTDIR)"\
 ".\Debug\MtGdi.pch"


!ELSEIF  "$(CFG)" == "MtGdi - Win32 Unicode Release"


".\UniRelease\Mtgdivw.obj" : $(SOURCE) $(DEP_CPP_MTGDIV) "$(INTDIR)"\
 ".\UniRelease\MtGdi.pch"

".\UniRelease\MtGdi.idb" : $(SOURCE) $(DEP_CPP_MTGDIV) "$(INTDIR)"\
 ".\UniRelease\MtGdi.pch"


!ELSEIF  "$(CFG)" == "MtGdi - Win32 Unicode Debug"


".\UniDebug\Mtgdivw.obj" : $(SOURCE) $(DEP_CPP_MTGDIV) "$(INTDIR)"\
 ".\UniDebug\MtGdi.pch"

".\UniDebug\MtGdi.pdb" : $(SOURCE) $(DEP_CPP_MTGDIV) "$(INTDIR)"\
 ".\UniDebug\MtGdi.pch"

".\UniDebug\MtGdi.idb" : $(SOURCE) $(DEP_CPP_MTGDIV) "$(INTDIR)"\
 ".\UniDebug\MtGdi.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Stdafx.cpp
DEP_CPP_STDAF=\
	".\Stdafx.h"\
	

!IF  "$(CFG)" == "MtGdi - Win32 Release"

# ADD CPP /Yc"Stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/MtGdi.pch" /Yc"Stdafx.h" /Fo"$(INTDIR)/"\
 /Fd"$(OUTDIR)/MtGdi.pdb" /c $(SOURCE) \
	

".\Release\Stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

".\Release\MtGdi.idb" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

".\Release\MtGdi.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "MtGdi - Win32 Debug"

# ADD CPP /Yc"Stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/MtGdi.pch" /Yc"Stdafx.h" /Fo"$(INTDIR)/"\
 /Fd"$(OUTDIR)/MtGdi.pdb" /c $(SOURCE) \
	

".\Debug\Stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

".\Debug\MtGdi.pdb" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

".\Debug\MtGdi.idb" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

".\Debug\MtGdi.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "MtGdi - Win32 Unicode Release"

# ADD CPP /Yc"Stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D\
 "_UNICODE" /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/MtGdi.pch" /Yc"Stdafx.h"\
 /Fo"$(INTDIR)/" /Fd"$(OUTDIR)/MtGdi.pdb" /c $(SOURCE) \
	

".\UniRelease\Stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

".\UniRelease\MtGdi.idb" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

".\UniRelease\MtGdi.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "MtGdi - Win32 Unicode Debug"

# ADD CPP /Yc"Stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MDd /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS"\
 /D "_UNICODE" /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/MtGdi.pch" /Yc"Stdafx.h"\
 /Fo"$(INTDIR)/" /Fd"$(OUTDIR)/MtGdi.pdb" /c $(SOURCE) \
	

".\UniDebug\Stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

".\UniDebug\MtGdi.pdb" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

".\UniDebug\MtGdi.idb" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

".\UniDebug\MtGdi.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Mainfrm.cpp
DEP_CPP_MAINF=\
	".\Mainfrm.h"\
	".\Mtgdi.h"\
	".\Resource.h"\
	".\Stdafx.h"\
	

!IF  "$(CFG)" == "MtGdi - Win32 Release"


".\Release\Mainfrm.obj" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 ".\Release\MtGdi.pch"

".\Release\MtGdi.idb" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 ".\Release\MtGdi.pch"


!ELSEIF  "$(CFG)" == "MtGdi - Win32 Debug"


".\Debug\Mainfrm.obj" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 ".\Debug\MtGdi.pch"

".\Debug\MtGdi.pdb" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 ".\Debug\MtGdi.pch"

".\Debug\MtGdi.idb" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 ".\Debug\MtGdi.pch"


!ELSEIF  "$(CFG)" == "MtGdi - Win32 Unicode Release"


".\UniRelease\Mainfrm.obj" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 ".\UniRelease\MtGdi.pch"

".\UniRelease\MtGdi.idb" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 ".\UniRelease\MtGdi.pch"


!ELSEIF  "$(CFG)" == "MtGdi - Win32 Unicode Debug"


".\UniDebug\Mainfrm.obj" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 ".\UniDebug\MtGdi.pch"

".\UniDebug\MtGdi.pdb" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 ".\UniDebug\MtGdi.pch"

".\UniDebug\MtGdi.idb" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 ".\UniDebug\MtGdi.pch"


!ENDIF 

# End Source File
# End Target
# End Project
################################################################################
