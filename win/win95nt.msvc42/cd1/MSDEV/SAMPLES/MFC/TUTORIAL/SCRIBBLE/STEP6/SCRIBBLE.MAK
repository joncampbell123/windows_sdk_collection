# Microsoft Visual C++ Generated NMAKE File, Format Version 40001
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

!IF "$(CFG)" == ""
CFG=Scribble - Win32 Debug
!MESSAGE No configuration specified.  Defaulting to Scribble - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "Scribble - Win32 Release" && "$(CFG)" !=\
 "Scribble - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "scribble.mak" CFG="Scribble - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Scribble - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "Scribble - Win32 Debug" (based on "Win32 (x86) Application")
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
# PROP Target_Last_Scanned "Scribble - Win32 Debug"
RSC=rc.exe
MTL=mktyplib.exe
CPP=cl.exe

!IF  "$(CFG)" == "Scribble - Win32 Release"

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

ALL : "$(OUTDIR)\scribble.exe" "$(OUTDIR)\scribble.hlp"

CLEAN : 
	-@erase ".\Release\scribble.exe"
	-@erase ".\Release\stdafx.obj"
	-@erase ".\Release\childfrm.obj"
	-@erase ".\Release\scribble.pch"
	-@erase ".\Release\mainfrm.obj"
	-@erase ".\Release\scribdoc.obj"
	-@erase ".\Release\pendlg.obj"
	-@erase ".\Release\scribvw.obj"
	-@erase ".\Release\scribble.obj"
	-@erase ".\Release\scribble.res"
	-@erase ".\Release\scribble.hlp"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
CPP_PROJ=/nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/scribble.pch" /Yu"stdafx.h" /Fo"$(INTDIR)/"\
 /c 
CPP_OBJS=.\Release/
CPP_SBRS=
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/scribble.res" /d "NDEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/scribble.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 /nologo /subsystem:windows /machine:I386
LINK32_FLAGS=/nologo /subsystem:windows /incremental:no\
 /pdb:"$(OUTDIR)/scribble.pdb" /machine:I386 /out:"$(OUTDIR)/scribble.exe" 
LINK32_OBJS= \
	"$(INTDIR)/stdafx.obj" \
	"$(INTDIR)/childfrm.obj" \
	"$(INTDIR)/mainfrm.obj" \
	"$(INTDIR)/scribdoc.obj" \
	"$(INTDIR)/pendlg.obj" \
	"$(INTDIR)/scribvw.obj" \
	"$(INTDIR)/scribble.obj" \
	"$(INTDIR)/scribble.res"

"$(OUTDIR)\scribble.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "Scribble - Win32 Debug"

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

ALL : "$(OUTDIR)\scribble.exe" "$(OUTDIR)\scribble.hlp"

CLEAN : 
	-@erase ".\Debug\vc40.pdb"
	-@erase ".\Debug\scribble.pch"
	-@erase ".\Debug\scribble.exe"
	-@erase ".\Debug\pendlg.obj"
	-@erase ".\Debug\childfrm.obj"
	-@erase ".\Debug\scribdoc.obj"
	-@erase ".\Debug\mainfrm.obj"
	-@erase ".\Debug\scribvw.obj"
	-@erase ".\Debug\stdafx.obj"
	-@erase ".\Debug\scribble.obj"
	-@erase ".\Debug\scribble.res"
	-@erase ".\Debug\scribble.ilk"
	-@erase ".\Debug\scribble.pdb"
	-@erase ".\Debug\scribble.hlp"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
CPP_PROJ=/nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/scribble.pch" /Yu"stdafx.h"\
 /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\Debug/
CPP_SBRS=
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/scribble.res" /d "_DEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/scribble.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386
# ADD LINK32 /nologo /subsystem:windows /debug /machine:I386
LINK32_FLAGS=/nologo /subsystem:windows /incremental:yes\
 /pdb:"$(OUTDIR)/scribble.pdb" /debug /machine:I386\
 /out:"$(OUTDIR)/scribble.exe" 
LINK32_OBJS= \
	"$(INTDIR)/pendlg.obj" \
	"$(INTDIR)/childfrm.obj" \
	"$(INTDIR)/scribdoc.obj" \
	"$(INTDIR)/mainfrm.obj" \
	"$(INTDIR)/scribvw.obj" \
	"$(INTDIR)/stdafx.obj" \
	"$(INTDIR)/scribble.obj" \
	"$(INTDIR)/scribble.res"

"$(OUTDIR)\scribble.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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

# Name "Scribble - Win32 Release"
# Name "Scribble - Win32 Debug"

!IF  "$(CFG)" == "Scribble - Win32 Release"

!ELSEIF  "$(CFG)" == "Scribble - Win32 Debug"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\readme.txt

!IF  "$(CFG)" == "Scribble - Win32 Release"

!ELSEIF  "$(CFG)" == "Scribble - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\scribble.cpp
DEP_CPP_SCRIB=\
	".\stdafx.h"\
	".\scribble.h"\
	".\mainfrm.h"\
	".\childfrm.h"\
	".\scribdoc.h"\
	".\scribvw.h"\
	

!IF  "$(CFG)" == "Scribble - Win32 Release"


"$(INTDIR)\scribble.obj" : $(SOURCE) $(DEP_CPP_SCRIB) "$(INTDIR)"\
 "$(INTDIR)\scribble.pch"


!ELSEIF  "$(CFG)" == "Scribble - Win32 Debug"


"$(INTDIR)\scribble.obj" : $(SOURCE) $(DEP_CPP_SCRIB) "$(INTDIR)"\
 "$(INTDIR)\scribble.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_SCRIB) "$(INTDIR)"\
 "$(INTDIR)\scribble.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\stdafx.cpp
DEP_CPP_STDAF=\
	".\stdafx.h"\
	

!IF  "$(CFG)" == "Scribble - Win32 Release"

# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/scribble.pch" /Yc"stdafx.h" /Fo"$(INTDIR)/"\
 /c $(SOURCE) \
	

"$(INTDIR)\stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\scribble.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "Scribble - Win32 Debug"

# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/scribble.pch" /Yc"stdafx.h"\
 /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\scribble.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\mainfrm.cpp
DEP_CPP_MAINF=\
	".\stdafx.h"\
	".\scribble.h"\
	".\mainfrm.h"\
	

!IF  "$(CFG)" == "Scribble - Win32 Release"


"$(INTDIR)\mainfrm.obj" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\scribble.pch"


!ELSEIF  "$(CFG)" == "Scribble - Win32 Debug"


"$(INTDIR)\mainfrm.obj" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\scribble.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\scribble.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\childfrm.cpp
DEP_CPP_CHILD=\
	".\stdafx.h"\
	".\scribble.h"\
	".\childfrm.h"\
	

!IF  "$(CFG)" == "Scribble - Win32 Release"


"$(INTDIR)\childfrm.obj" : $(SOURCE) $(DEP_CPP_CHILD) "$(INTDIR)"\
 "$(INTDIR)\scribble.pch"


!ELSEIF  "$(CFG)" == "Scribble - Win32 Debug"


"$(INTDIR)\childfrm.obj" : $(SOURCE) $(DEP_CPP_CHILD) "$(INTDIR)"\
 "$(INTDIR)\scribble.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_CHILD) "$(INTDIR)"\
 "$(INTDIR)\scribble.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\scribdoc.cpp
DEP_CPP_SCRIBD=\
	".\pendlg.h"\
	".\scribdoc.h"\
	".\scribble.h"\
	".\stdafx.h"\
	

!IF  "$(CFG)" == "Scribble - Win32 Release"


"$(INTDIR)\scribdoc.obj" : $(SOURCE) $(DEP_CPP_SCRIBD) "$(INTDIR)"\
 "$(INTDIR)\scribble.pch"


!ELSEIF  "$(CFG)" == "Scribble - Win32 Debug"


"$(INTDIR)\scribdoc.obj" : $(SOURCE) $(DEP_CPP_SCRIBD) "$(INTDIR)"\
 "$(INTDIR)\scribble.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_SCRIBD) "$(INTDIR)"\
 "$(INTDIR)\scribble.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\scribvw.cpp
DEP_CPP_SCRIBV=\
	".\stdafx.h"\
	".\scribble.h"\
	".\scribdoc.h"\
	".\scribvw.h"\
	

!IF  "$(CFG)" == "Scribble - Win32 Release"


"$(INTDIR)\scribvw.obj" : $(SOURCE) $(DEP_CPP_SCRIBV) "$(INTDIR)"\
 "$(INTDIR)\scribble.pch"


!ELSEIF  "$(CFG)" == "Scribble - Win32 Debug"


"$(INTDIR)\scribvw.obj" : $(SOURCE) $(DEP_CPP_SCRIBV) "$(INTDIR)"\
 "$(INTDIR)\scribble.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_SCRIBV) "$(INTDIR)"\
 "$(INTDIR)\scribble.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\scribble.rc
DEP_RSC_SCRIBB=\
	".\res\scribble.ico"\
	".\res\scribdoc.ico"\
	".\res\toolbar.bmp"\
	".\res\scribble.rc2"\
	

"$(INTDIR)\scribble.res" : $(SOURCE) $(DEP_RSC_SCRIBB) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\pendlg.cpp
DEP_CPP_PENDL=\
	".\stdafx.h"\
	".\scribble.h"\
	".\pendlg.h"\
	

!IF  "$(CFG)" == "Scribble - Win32 Release"


"$(INTDIR)\pendlg.obj" : $(SOURCE) $(DEP_CPP_PENDL) "$(INTDIR)"\
 "$(INTDIR)\scribble.pch"


!ELSEIF  "$(CFG)" == "Scribble - Win32 Debug"


"$(INTDIR)\pendlg.obj" : $(SOURCE) $(DEP_CPP_PENDL) "$(INTDIR)"\
 "$(INTDIR)\scribble.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_PENDL) "$(INTDIR)"\
 "$(INTDIR)\scribble.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\hlp\scribble.hpj

!IF  "$(CFG)" == "Scribble - Win32 Release"

# Begin Custom Build - Building help file...
OutDir=.\Release
TargetName=scribble
InputPath=.\hlp\scribble.hpj

"$(OutDir)\$(TargetName).hlp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   makehelp.bat

# End Custom Build

!ELSEIF  "$(CFG)" == "Scribble - Win32 Debug"

# Begin Custom Build - Building help file...
OutDir=.\Debug
TargetName=scribble
InputPath=.\hlp\scribble.hpj

"$(OutDir)\$(TargetName).hlp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   makehelp.bat

# End Custom Build

!ENDIF 

# End Source File
# End Target
# End Project
################################################################################
