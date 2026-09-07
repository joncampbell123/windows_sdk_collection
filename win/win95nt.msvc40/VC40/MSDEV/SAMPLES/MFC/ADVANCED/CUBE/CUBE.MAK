# Microsoft Visual C++ Generated NMAKE File, Format Version 40000
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

!IF "$(CFG)" == ""
CFG=Cube - Win32 Debug
!MESSAGE No configuration specified.  Defaulting to Cube - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "Cube - Win32 Release" && "$(CFG)" != "Cube - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "Cube.mak" CFG="Cube - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Cube - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "Cube - Win32 Debug" (based on "Win32 (x86) Application")
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
# PROP Target_Last_Scanned "Cube - Win32 Debug"
CPP=cl.exe
MTL=mktyplib.exe
RSC=rc.exe

!IF  "$(CFG)" == "Cube - Win32 Release"

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

ALL : "$(OUTDIR)\Cube.exe"

CLEAN : 
	-@erase ".\Release\Cube.idb"
	-@erase ".\Release\Cube.pch"
	-@erase ".\Release\Cube.exe"
	-@erase ".\Release\Cubeview.obj"
	-@erase ".\Release\Mainfrm.obj"
	-@erase ".\Release\Stdafx.obj"
	-@erase ".\Release\Cube.obj"
	-@erase ".\Release\Cubedoc.obj"
	-@erase ".\Release\Cube.res"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"Stdafx.h" /c
CPP_PROJ=/nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/Cube.pch" /Yu"Stdafx.h" /Fo"$(INTDIR)/"\
 /Fd"$(OUTDIR)/Cube.pdb" /c 
CPP_OBJS=.\Release/
CPP_SBRS=
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/Cube.res" /d "NDEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/Cube.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 opengl32.lib glu32.lib /nologo /subsystem:windows /machine:I386
LINK32_FLAGS=opengl32.lib glu32.lib /nologo /subsystem:windows /incremental:no\
 /pdb:"$(OUTDIR)/Cube.pdb" /machine:I386 /out:"$(OUTDIR)/Cube.exe" 
LINK32_OBJS= \
	"$(INTDIR)/Cubeview.obj" \
	"$(INTDIR)/Mainfrm.obj" \
	"$(INTDIR)/Stdafx.obj" \
	"$(INTDIR)/Cube.obj" \
	"$(INTDIR)/Cubedoc.obj" \
	"$(INTDIR)/Cube.res"

"$(OUTDIR)\Cube.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "Cube - Win32 Debug"

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

ALL : "$(OUTDIR)\Cube.exe"

CLEAN : 
	-@erase ".\Debug\Cube.pdb"
	-@erase ".\Debug\Cube.pch"
	-@erase ".\Debug\Mainfrm.obj"
	-@erase ".\Debug\Cube.obj"
	-@erase ".\Debug\Cubeview.obj"
	-@erase ".\Debug\Stdafx.obj"
	-@erase ".\Debug\Cubedoc.obj"
	-@erase ".\Debug\Cube.res"
	-@erase ".\Debug\Cube.idb"
	-@erase ".\Debug\Cube.exe"
	-@erase ".\Debug\Cube.ilk"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"Stdafx.h" /c
CPP_PROJ=/nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/Cube.pch" /Yu"Stdafx.h" /Fo"$(INTDIR)/"\
 /Fd"$(OUTDIR)/Cube.pdb" /c 
CPP_OBJS=.\Debug/
CPP_SBRS=
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/Cube.res" /d "_DEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/Cube.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386
# ADD LINK32 opengl32.lib glu32.lib /nologo /subsystem:windows /debug /machine:I386
LINK32_FLAGS=opengl32.lib glu32.lib /nologo /subsystem:windows /incremental:yes\
 /pdb:"$(OUTDIR)/Cube.pdb" /debug /machine:I386 /out:"$(OUTDIR)/Cube.exe" 
LINK32_OBJS= \
	"$(INTDIR)/Mainfrm.obj" \
	"$(INTDIR)/Cube.obj" \
	"$(INTDIR)/Cubeview.obj" \
	"$(INTDIR)/Stdafx.obj" \
	"$(INTDIR)/Cubedoc.obj" \
	"$(INTDIR)/Cube.res"

"$(OUTDIR)\Cube.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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

# Name "Cube - Win32 Release"
# Name "Cube - Win32 Debug"

!IF  "$(CFG)" == "Cube - Win32 Release"

!ELSEIF  "$(CFG)" == "Cube - Win32 Debug"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\Stdafx.cpp
DEP_CPP_STDAF=\
	".\Stdafx.h"\
	

!IF  "$(CFG)" == "Cube - Win32 Release"

# ADD CPP /Yc"Stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/Cube.pch" /Yc"Stdafx.h" /Fo"$(INTDIR)/"\
 /Fd"$(OUTDIR)/Cube.pdb" /c $(SOURCE) \
	

"$(INTDIR)\Stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\Cube.idb" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\Cube.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "Cube - Win32 Debug"

# ADD CPP /Yc"Stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/Cube.pch" /Yc"Stdafx.h" /Fo"$(INTDIR)/"\
 /Fd"$(OUTDIR)/Cube.pdb" /c $(SOURCE) \
	

"$(INTDIR)\Stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\Cube.pdb" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\Cube.idb" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\Cube.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Cube.rc
DEP_RSC_CUBE_=\
	".\Res\Cube.ico"\
	".\Res\Toolbar.bmp"\
	".\Res\Cube.rc2"\
	

"$(INTDIR)\Cube.res" : $(SOURCE) $(DEP_RSC_CUBE_) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\Cubedoc.cpp
DEP_CPP_CUBED=\
	".\Cube.h"\
	".\Resource.h"\
	".\Cubedoc.h"\
	".\Stdafx.h"\
	

!IF  "$(CFG)" == "Cube - Win32 Release"


"$(INTDIR)\Cubedoc.obj" : $(SOURCE) $(DEP_CPP_CUBED) "$(INTDIR)"\
 "$(INTDIR)\Cube.pch"

"$(INTDIR)\Cube.idb" : $(SOURCE) $(DEP_CPP_CUBED) "$(INTDIR)"\
 "$(INTDIR)\Cube.pch"


!ELSEIF  "$(CFG)" == "Cube - Win32 Debug"


"$(INTDIR)\Cubedoc.obj" : $(SOURCE) $(DEP_CPP_CUBED) "$(INTDIR)"\
 "$(INTDIR)\Cube.pch"

"$(INTDIR)\Cube.pdb" : $(SOURCE) $(DEP_CPP_CUBED) "$(INTDIR)"\
 "$(INTDIR)\Cube.pch"

"$(INTDIR)\Cube.idb" : $(SOURCE) $(DEP_CPP_CUBED) "$(INTDIR)"\
 "$(INTDIR)\Cube.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Cubeview.cpp
DEP_CPP_CUBEV=\
	".\Cube.h"\
	".\Resource.h"\
	".\Cubedoc.h"\
	".\Cubeview.h"\
	".\Stdafx.h"\
	

!IF  "$(CFG)" == "Cube - Win32 Release"


"$(INTDIR)\Cubeview.obj" : $(SOURCE) $(DEP_CPP_CUBEV) "$(INTDIR)"\
 "$(INTDIR)\Cube.pch"

"$(INTDIR)\Cube.idb" : $(SOURCE) $(DEP_CPP_CUBEV) "$(INTDIR)"\
 "$(INTDIR)\Cube.pch"


!ELSEIF  "$(CFG)" == "Cube - Win32 Debug"


"$(INTDIR)\Cubeview.obj" : $(SOURCE) $(DEP_CPP_CUBEV) "$(INTDIR)"\
 "$(INTDIR)\Cube.pch"

"$(INTDIR)\Cube.pdb" : $(SOURCE) $(DEP_CPP_CUBEV) "$(INTDIR)"\
 "$(INTDIR)\Cube.pch"

"$(INTDIR)\Cube.idb" : $(SOURCE) $(DEP_CPP_CUBEV) "$(INTDIR)"\
 "$(INTDIR)\Cube.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Mainfrm.cpp
DEP_CPP_MAINF=\
	".\Cube.h"\
	".\Resource.h"\
	".\Cubedoc.h"\
	".\Cubeview.h"\
	".\Mainfrm.h"\
	".\Stdafx.h"\
	

!IF  "$(CFG)" == "Cube - Win32 Release"


"$(INTDIR)\Mainfrm.obj" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\Cube.pch"

"$(INTDIR)\Cube.idb" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\Cube.pch"


!ELSEIF  "$(CFG)" == "Cube - Win32 Debug"


"$(INTDIR)\Mainfrm.obj" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\Cube.pch"

"$(INTDIR)\Cube.pdb" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\Cube.pch"

"$(INTDIR)\Cube.idb" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\Cube.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Cube.cpp
DEP_CPP_CUBE_C=\
	".\Cube.h"\
	".\Resource.h"\
	".\Cubedoc.h"\
	".\Cubeview.h"\
	".\Mainfrm.h"\
	".\Stdafx.h"\
	

!IF  "$(CFG)" == "Cube - Win32 Release"


"$(INTDIR)\Cube.obj" : $(SOURCE) $(DEP_CPP_CUBE_C) "$(INTDIR)"\
 "$(INTDIR)\Cube.pch"

"$(INTDIR)\Cube.idb" : $(SOURCE) $(DEP_CPP_CUBE_C) "$(INTDIR)"\
 "$(INTDIR)\Cube.pch"


!ELSEIF  "$(CFG)" == "Cube - Win32 Debug"


"$(INTDIR)\Cube.obj" : $(SOURCE) $(DEP_CPP_CUBE_C) "$(INTDIR)"\
 "$(INTDIR)\Cube.pch"

"$(INTDIR)\Cube.pdb" : $(SOURCE) $(DEP_CPP_CUBE_C) "$(INTDIR)"\
 "$(INTDIR)\Cube.pch"

"$(INTDIR)\Cube.idb" : $(SOURCE) $(DEP_CPP_CUBE_C) "$(INTDIR)"\
 "$(INTDIR)\Cube.pch"


!ENDIF 

# End Source File
# End Target
# End Project
################################################################################
