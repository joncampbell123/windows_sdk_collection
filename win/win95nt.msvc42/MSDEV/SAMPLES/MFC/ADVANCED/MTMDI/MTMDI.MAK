# Microsoft Visual C++ Generated NMAKE File, Format Version 40000
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

!IF "$(CFG)" == ""
CFG=MtMdi - Win32 Debug
!MESSAGE No configuration specified.  Defaulting to MtMdi - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "MtMdi - Win32 Release" && "$(CFG)" != "MtMdi - Win32 Debug" &&\
 "$(CFG)" != "MtMdi - Win32 Unicode Debug" && "$(CFG)" !=\
 "MtMdi - Win32 Unicode Release"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "MtMdi.mak" CFG="MtMdi - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "MtMdi - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "MtMdi - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE "MtMdi - Win32 Unicode Debug" (based on "Win32 (x86) Application")
!MESSAGE "MtMdi - Win32 Unicode Release" (based on "Win32 (x86) Application")
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
# PROP Target_Last_Scanned "MtMdi - Win32 Debug"
MTL=mktyplib.exe
RSC=rc.exe
CPP=cl.exe

!IF  "$(CFG)" == "MtMdi - Win32 Release"

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

ALL : "$(OUTDIR)\MtMdi.exe"

CLEAN : 
	-@erase ".\Release\MtMdi.exe"
	-@erase ".\Release\Stdafx.obj"
	-@erase ".\Release\Hello.obj"
	-@erase ".\Release\MtMdi.pch"
	-@erase ".\Release\Mainfrm.obj"
	-@erase ".\Release\Mdi.obj"
	-@erase ".\Release\Mtbounce.obj"
	-@erase ".\Release\Mtmdi.res"
	-@erase ".\Release\Bounce.obj"
	-@erase ".\Release\MtMdi.idb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"Stdafx.h" /c
CPP_PROJ=/nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/MtMdi.pch" /Yu"Stdafx.h" /Fo"$(INTDIR)/"\
 /Fd"$(OUTDIR)/MtMdi.pdb" /c 
CPP_OBJS=.\Release/
CPP_SBRS=
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/Mtmdi.res" /d "NDEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/MtMdi.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 /nologo /subsystem:windows /machine:I386
LINK32_FLAGS=/nologo /subsystem:windows /incremental:no\
 /pdb:"$(OUTDIR)/MtMdi.pdb" /machine:I386 /out:"$(OUTDIR)/MtMdi.exe" 
LINK32_OBJS= \
	"$(INTDIR)/Stdafx.obj" \
	"$(INTDIR)/Hello.obj" \
	"$(INTDIR)/Mainfrm.obj" \
	"$(INTDIR)/Mdi.obj" \
	"$(INTDIR)/Mtbounce.obj" \
	"$(INTDIR)/Bounce.obj" \
	"$(INTDIR)/Mtmdi.res"

"$(OUTDIR)\MtMdi.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "MtMdi - Win32 Debug"

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

ALL : "$(OUTDIR)\MtMdi.exe"

CLEAN : 
	-@erase ".\Debug\MtMdi.exe"
	-@erase ".\Debug\Stdafx.obj"
	-@erase ".\Debug\Hello.obj"
	-@erase ".\Debug\MtMdi.pch"
	-@erase ".\Debug\Mainfrm.obj"
	-@erase ".\Debug\Mdi.obj"
	-@erase ".\Debug\Mtbounce.obj"
	-@erase ".\Debug\Mtmdi.res"
	-@erase ".\Debug\Bounce.obj"
	-@erase ".\Debug\MtMdi.ilk"
	-@erase ".\Debug\MtMdi.pdb"
	-@erase ".\Debug\MtMdi.idb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"Stdafx.h" /c
CPP_PROJ=/nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/MtMdi.pch" /Yu"Stdafx.h" /Fo"$(INTDIR)/"\
 /Fd"$(OUTDIR)/MtMdi.pdb" /c 
CPP_OBJS=.\Debug/
CPP_SBRS=
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/Mtmdi.res" /d "_DEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/MtMdi.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386
# ADD LINK32 /nologo /subsystem:windows /debug /machine:I386
LINK32_FLAGS=/nologo /subsystem:windows /incremental:yes\
 /pdb:"$(OUTDIR)/MtMdi.pdb" /debug /machine:I386 /out:"$(OUTDIR)/MtMdi.exe" 
LINK32_OBJS= \
	"$(INTDIR)/Stdafx.obj" \
	"$(INTDIR)/Hello.obj" \
	"$(INTDIR)/Mainfrm.obj" \
	"$(INTDIR)/Mdi.obj" \
	"$(INTDIR)/Mtbounce.obj" \
	"$(INTDIR)/Bounce.obj" \
	"$(INTDIR)/Mtmdi.res"

"$(OUTDIR)\MtMdi.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "MtMdi - Win32 Unicode Debug"

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

ALL : "$(OUTDIR)\MtMdi.exe"

CLEAN : 
	-@erase ".\UniDebug\MtMdi.pdb"
	-@erase ".\UniDebug\MtMdi.pch"
	-@erase ".\UniDebug\MtMdi.idb"
	-@erase ".\UniDebug\MtMdi.exe"
	-@erase ".\UniDebug\Stdafx.obj"
	-@erase ".\UniDebug\Hello.obj"
	-@erase ".\UniDebug\Mainfrm.obj"
	-@erase ".\UniDebug\Mdi.obj"
	-@erase ".\UniDebug\Mtbounce.obj"
	-@erase ".\UniDebug\Bounce.obj"
	-@erase ".\UniDebug\Mtmdi.res"
	-@erase ".\UniDebug\MtMdi.ilk"
	-@erase ".\UniDebug\MtMdi.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_UNICODE" /D "_AFXDLL" /D "_MBCS" /Yu"Stdafx.h" /c
CPP_PROJ=/nologo /MDd /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS"\
 /D "_UNICODE" /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/MtMdi.pch" /Yu"Stdafx.h"\
 /Fo"$(INTDIR)/" /Fd"$(OUTDIR)/MtMdi.pdb" /c 
CPP_OBJS=.\UniDebug/
CPP_SBRS=
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/Mtmdi.res" /d "_DEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/MtMdi.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386
# ADD LINK32 /nologo /entry:"wWinMainCRTStartup" /subsystem:windows /debug /machine:I386
LINK32_FLAGS=/nologo /entry:"wWinMainCRTStartup" /subsystem:windows\
 /incremental:yes /pdb:"$(OUTDIR)/MtMdi.pdb" /debug /machine:I386\
 /out:"$(OUTDIR)/MtMdi.exe" 
LINK32_OBJS= \
	".\UniDebug\Stdafx.obj" \
	".\UniDebug\Hello.obj" \
	".\UniDebug\Mainfrm.obj" \
	".\UniDebug\Mdi.obj" \
	".\UniDebug\Mtbounce.obj" \
	".\UniDebug\Bounce.obj" \
	".\UniDebug\Mtmdi.res"

"$(OUTDIR)\MtMdi.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "MtMdi - Win32 Unicode Release"

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

ALL : "$(OUTDIR)\MtMdi.exe"

CLEAN : 
	-@erase ".\UniRelease\MtMdi.idb"
	-@erase ".\UniRelease\MtMdi.pch"
	-@erase ".\UniRelease\MtMdi.exe"
	-@erase ".\UniRelease\Stdafx.obj"
	-@erase ".\UniRelease\Hello.obj"
	-@erase ".\UniRelease\Mainfrm.obj"
	-@erase ".\UniRelease\Mdi.obj"
	-@erase ".\UniRelease\Mtbounce.obj"
	-@erase ".\UniRelease\Bounce.obj"
	-@erase ".\UniRelease\Mtmdi.res"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_UNICODE" /D "_AFXDLL" /D "_MBCS" /Yu"Stdafx.h" /c
CPP_PROJ=/nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D\
 "_UNICODE" /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/MtMdi.pch" /Yu"Stdafx.h"\
 /Fo"$(INTDIR)/" /Fd"$(OUTDIR)/MtMdi.pdb" /c 
CPP_OBJS=.\UniRelease/
CPP_SBRS=
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/Mtmdi.res" /d "NDEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/MtMdi.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 /nologo /entry:"wWinMainCRTStartup" /subsystem:windows /machine:I386
LINK32_FLAGS=/nologo /entry:"wWinMainCRTStartup" /subsystem:windows\
 /incremental:no /pdb:"$(OUTDIR)/MtMdi.pdb" /machine:I386\
 /out:"$(OUTDIR)/MtMdi.exe" 
LINK32_OBJS= \
	".\UniRelease\Stdafx.obj" \
	".\UniRelease\Hello.obj" \
	".\UniRelease\Mainfrm.obj" \
	".\UniRelease\Mdi.obj" \
	".\UniRelease\Mtbounce.obj" \
	".\UniRelease\Bounce.obj" \
	".\UniRelease\Mtmdi.res"

"$(OUTDIR)\MtMdi.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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

# Name "MtMdi - Win32 Release"
# Name "MtMdi - Win32 Debug"
# Name "MtMdi - Win32 Unicode Debug"
# Name "MtMdi - Win32 Unicode Release"

!IF  "$(CFG)" == "MtMdi - Win32 Release"

!ELSEIF  "$(CFG)" == "MtMdi - Win32 Debug"

!ELSEIF  "$(CFG)" == "MtMdi - Win32 Unicode Debug"

!ELSEIF  "$(CFG)" == "MtMdi - Win32 Unicode Release"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\Stdafx.cpp
DEP_CPP_STDAF=\
	".\stdafx.h"\
	

!IF  "$(CFG)" == "MtMdi - Win32 Release"

# ADD CPP /Yc"Stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/MtMdi.pch" /Yc"Stdafx.h" /Fo"$(INTDIR)/"\
 /Fd"$(OUTDIR)/MtMdi.pdb" /c $(SOURCE) \
	

"$(INTDIR)\Stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\MtMdi.idb" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\MtMdi.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "MtMdi - Win32 Debug"

# ADD CPP /Yc"Stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/MtMdi.pch" /Yc"Stdafx.h" /Fo"$(INTDIR)/"\
 /Fd"$(OUTDIR)/MtMdi.pdb" /c $(SOURCE) \
	

"$(INTDIR)\Stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\MtMdi.pdb" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\MtMdi.idb" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\MtMdi.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "MtMdi - Win32 Unicode Debug"

# ADD CPP /Yc"Stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MDd /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS"\
 /D "_UNICODE" /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/MtMdi.pch" /Yc"Stdafx.h"\
 /Fo"$(INTDIR)/" /Fd"$(OUTDIR)/MtMdi.pdb" /c $(SOURCE) \
	

"$(INTDIR)\Stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\MtMdi.pdb" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\MtMdi.idb" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\MtMdi.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "MtMdi - Win32 Unicode Release"

# ADD CPP /Yc"Stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D\
 "_UNICODE" /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/MtMdi.pch" /Yc"Stdafx.h"\
 /Fo"$(INTDIR)/" /Fd"$(OUTDIR)/MtMdi.pdb" /c $(SOURCE) \
	

"$(INTDIR)\Stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\MtMdi.idb" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\MtMdi.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Hello.cpp
DEP_CPP_HELLO=\
	".\hello.h"\
	".\mdi.h"\
	".\resource.h"\
	".\stdafx.h"\
	

!IF  "$(CFG)" == "MtMdi - Win32 Release"


"$(INTDIR)\Hello.obj" : $(SOURCE) $(DEP_CPP_HELLO) "$(INTDIR)"\
 "$(INTDIR)\MtMdi.pch"

"$(INTDIR)\MtMdi.idb" : $(SOURCE) $(DEP_CPP_HELLO) "$(INTDIR)"\
 "$(INTDIR)\MtMdi.pch"


!ELSEIF  "$(CFG)" == "MtMdi - Win32 Debug"


"$(INTDIR)\Hello.obj" : $(SOURCE) $(DEP_CPP_HELLO) "$(INTDIR)"\
 "$(INTDIR)\MtMdi.pch"

"$(INTDIR)\MtMdi.pdb" : $(SOURCE) $(DEP_CPP_HELLO) "$(INTDIR)"\
 "$(INTDIR)\MtMdi.pch"

"$(INTDIR)\MtMdi.idb" : $(SOURCE) $(DEP_CPP_HELLO) "$(INTDIR)"\
 "$(INTDIR)\MtMdi.pch"


!ELSEIF  "$(CFG)" == "MtMdi - Win32 Unicode Debug"


"$(INTDIR)\Hello.obj" : $(SOURCE) $(DEP_CPP_HELLO) "$(INTDIR)"\
 "$(INTDIR)\MtMdi.pch"

"$(INTDIR)\MtMdi.pdb" : $(SOURCE) $(DEP_CPP_HELLO) "$(INTDIR)"\
 "$(INTDIR)\MtMdi.pch"

"$(INTDIR)\MtMdi.idb" : $(SOURCE) $(DEP_CPP_HELLO) "$(INTDIR)"\
 "$(INTDIR)\MtMdi.pch"


!ELSEIF  "$(CFG)" == "MtMdi - Win32 Unicode Release"


"$(INTDIR)\Hello.obj" : $(SOURCE) $(DEP_CPP_HELLO) "$(INTDIR)"\
 "$(INTDIR)\MtMdi.pch"

"$(INTDIR)\MtMdi.idb" : $(SOURCE) $(DEP_CPP_HELLO) "$(INTDIR)"\
 "$(INTDIR)\MtMdi.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Mainfrm.cpp
DEP_CPP_MAINF=\
	".\bounce.h"\
	".\hello.h"\
	".\mainfrm.h"\
	".\mdi.h"\
	".\resource.h"\
	".\mtbounce.h"\
	".\stdafx.h"\
	

!IF  "$(CFG)" == "MtMdi - Win32 Release"


"$(INTDIR)\Mainfrm.obj" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\MtMdi.pch"

"$(INTDIR)\MtMdi.idb" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\MtMdi.pch"


!ELSEIF  "$(CFG)" == "MtMdi - Win32 Debug"


"$(INTDIR)\Mainfrm.obj" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\MtMdi.pch"

"$(INTDIR)\MtMdi.pdb" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\MtMdi.pch"

"$(INTDIR)\MtMdi.idb" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\MtMdi.pch"


!ELSEIF  "$(CFG)" == "MtMdi - Win32 Unicode Debug"


"$(INTDIR)\Mainfrm.obj" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\MtMdi.pch"

"$(INTDIR)\MtMdi.pdb" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\MtMdi.pch"

"$(INTDIR)\MtMdi.idb" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\MtMdi.pch"


!ELSEIF  "$(CFG)" == "MtMdi - Win32 Unicode Release"


"$(INTDIR)\Mainfrm.obj" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\MtMdi.pch"

"$(INTDIR)\MtMdi.idb" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\MtMdi.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Mdi.cpp
DEP_CPP_MDI_C=\
	".\bounce.h"\
	".\mainfrm.h"\
	".\mdi.h"\
	".\resource.h"\
	".\mtbounce.h"\
	".\stdafx.h"\
	

!IF  "$(CFG)" == "MtMdi - Win32 Release"


"$(INTDIR)\Mdi.obj" : $(SOURCE) $(DEP_CPP_MDI_C) "$(INTDIR)"\
 "$(INTDIR)\MtMdi.pch"

"$(INTDIR)\MtMdi.idb" : $(SOURCE) $(DEP_CPP_MDI_C) "$(INTDIR)"\
 "$(INTDIR)\MtMdi.pch"


!ELSEIF  "$(CFG)" == "MtMdi - Win32 Debug"


"$(INTDIR)\Mdi.obj" : $(SOURCE) $(DEP_CPP_MDI_C) "$(INTDIR)"\
 "$(INTDIR)\MtMdi.pch"

"$(INTDIR)\MtMdi.pdb" : $(SOURCE) $(DEP_CPP_MDI_C) "$(INTDIR)"\
 "$(INTDIR)\MtMdi.pch"

"$(INTDIR)\MtMdi.idb" : $(SOURCE) $(DEP_CPP_MDI_C) "$(INTDIR)"\
 "$(INTDIR)\MtMdi.pch"


!ELSEIF  "$(CFG)" == "MtMdi - Win32 Unicode Debug"


"$(INTDIR)\Mdi.obj" : $(SOURCE) $(DEP_CPP_MDI_C) "$(INTDIR)" "$(INTDIR)\MtMdi.pch"

"$(INTDIR)\MtMdi.pdb" : $(SOURCE) $(DEP_CPP_MDI_C) "$(INTDIR)"\
 "$(INTDIR)\MtMdi.pch"

"$(INTDIR)\MtMdi.idb" : $(SOURCE) $(DEP_CPP_MDI_C) "$(INTDIR)"\
 "$(INTDIR)\MtMdi.pch"


!ELSEIF  "$(CFG)" == "MtMdi - Win32 Unicode Release"


"$(INTDIR)\Mdi.obj" : $(SOURCE) $(DEP_CPP_MDI_C) "$(INTDIR)"\
 "$(INTDIR)\MtMdi.pch"

"$(INTDIR)\MtMdi.idb" : $(SOURCE) $(DEP_CPP_MDI_C) "$(INTDIR)"\
 "$(INTDIR)\MtMdi.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Mtbounce.cpp
DEP_CPP_MTBOU=\
	".\bounce.h"\
	".\mtbounce.h"\
	".\stdafx.h"\
	

!IF  "$(CFG)" == "MtMdi - Win32 Release"


"$(INTDIR)\Mtbounce.obj" : $(SOURCE) $(DEP_CPP_MTBOU) "$(INTDIR)"\
 "$(INTDIR)\MtMdi.pch"

"$(INTDIR)\MtMdi.idb" : $(SOURCE) $(DEP_CPP_MTBOU) "$(INTDIR)"\
 "$(INTDIR)\MtMdi.pch"


!ELSEIF  "$(CFG)" == "MtMdi - Win32 Debug"


"$(INTDIR)\Mtbounce.obj" : $(SOURCE) $(DEP_CPP_MTBOU) "$(INTDIR)"\
 "$(INTDIR)\MtMdi.pch"

"$(INTDIR)\MtMdi.pdb" : $(SOURCE) $(DEP_CPP_MTBOU) "$(INTDIR)"\
 "$(INTDIR)\MtMdi.pch"

"$(INTDIR)\MtMdi.idb" : $(SOURCE) $(DEP_CPP_MTBOU) "$(INTDIR)"\
 "$(INTDIR)\MtMdi.pch"


!ELSEIF  "$(CFG)" == "MtMdi - Win32 Unicode Debug"


"$(INTDIR)\Mtbounce.obj" : $(SOURCE) $(DEP_CPP_MTBOU) "$(INTDIR)"\
 "$(INTDIR)\MtMdi.pch"

"$(INTDIR)\MtMdi.pdb" : $(SOURCE) $(DEP_CPP_MTBOU) "$(INTDIR)"\
 "$(INTDIR)\MtMdi.pch"

"$(INTDIR)\MtMdi.idb" : $(SOURCE) $(DEP_CPP_MTBOU) "$(INTDIR)"\
 "$(INTDIR)\MtMdi.pch"


!ELSEIF  "$(CFG)" == "MtMdi - Win32 Unicode Release"


"$(INTDIR)\Mtbounce.obj" : $(SOURCE) $(DEP_CPP_MTBOU) "$(INTDIR)"\
 "$(INTDIR)\MtMdi.pch"

"$(INTDIR)\MtMdi.idb" : $(SOURCE) $(DEP_CPP_MTBOU) "$(INTDIR)"\
 "$(INTDIR)\MtMdi.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Mtmdi.rc
DEP_RSC_MTMDI=\
	".\MDI.ICO"\
	".\HELLO.ICO"\
	

!IF  "$(CFG)" == "MtMdi - Win32 Release"


"$(INTDIR)\Mtmdi.res" : $(SOURCE) $(DEP_RSC_MTMDI) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "MtMdi - Win32 Debug"


"$(INTDIR)\Mtmdi.res" : $(SOURCE) $(DEP_RSC_MTMDI) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "MtMdi - Win32 Unicode Debug"


"$(INTDIR)\Mtmdi.res" : $(SOURCE) $(DEP_RSC_MTMDI) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "MtMdi - Win32 Unicode Release"


"$(INTDIR)\Mtmdi.res" : $(SOURCE) $(DEP_RSC_MTMDI) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Bounce.cpp
DEP_CPP_BOUNC=\
	".\bounce.h"\
	".\mdi.h"\
	".\resource.h"\
	".\mtbounce.h"\
	".\stdafx.h"\
	

!IF  "$(CFG)" == "MtMdi - Win32 Release"


"$(INTDIR)\Bounce.obj" : $(SOURCE) $(DEP_CPP_BOUNC) "$(INTDIR)"\
 "$(INTDIR)\MtMdi.pch"

"$(INTDIR)\MtMdi.idb" : $(SOURCE) $(DEP_CPP_BOUNC) "$(INTDIR)"\
 "$(INTDIR)\MtMdi.pch"


!ELSEIF  "$(CFG)" == "MtMdi - Win32 Debug"


"$(INTDIR)\Bounce.obj" : $(SOURCE) $(DEP_CPP_BOUNC) "$(INTDIR)"\
 "$(INTDIR)\MtMdi.pch"

"$(INTDIR)\MtMdi.pdb" : $(SOURCE) $(DEP_CPP_BOUNC) "$(INTDIR)"\
 "$(INTDIR)\MtMdi.pch"

"$(INTDIR)\MtMdi.idb" : $(SOURCE) $(DEP_CPP_BOUNC) "$(INTDIR)"\
 "$(INTDIR)\MtMdi.pch"


!ELSEIF  "$(CFG)" == "MtMdi - Win32 Unicode Debug"


"$(INTDIR)\Bounce.obj" : $(SOURCE) $(DEP_CPP_BOUNC) "$(INTDIR)"\
 "$(INTDIR)\MtMdi.pch"

"$(INTDIR)\MtMdi.pdb" : $(SOURCE) $(DEP_CPP_BOUNC) "$(INTDIR)"\
 "$(INTDIR)\MtMdi.pch"

"$(INTDIR)\MtMdi.idb" : $(SOURCE) $(DEP_CPP_BOUNC) "$(INTDIR)"\
 "$(INTDIR)\MtMdi.pch"


!ELSEIF  "$(CFG)" == "MtMdi - Win32 Unicode Release"


"$(INTDIR)\Bounce.obj" : $(SOURCE) $(DEP_CPP_BOUNC) "$(INTDIR)"\
 "$(INTDIR)\MtMdi.pch"

"$(INTDIR)\MtMdi.idb" : $(SOURCE) $(DEP_CPP_BOUNC) "$(INTDIR)"\
 "$(INTDIR)\MtMdi.pch"


!ENDIF 

# End Source File
# End Target
# End Project
################################################################################
