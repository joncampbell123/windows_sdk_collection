# Microsoft Visual C++ Generated NMAKE File, Format Version 40000
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

!IF "$(CFG)" == ""
CFG=Saver - Win32 Debug
!MESSAGE No configuration specified.  Defaulting to Saver - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "Saver - Win32 Release" && "$(CFG)" != "Saver - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "Saver.mak" CFG="Saver - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Saver - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "Saver - Win32 Debug" (based on "Win32 (x86) Application")
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
MTL=mktyplib.exe
RSC=rc.exe
CPP=cl.exe

!IF  "$(CFG)" == "Saver - Win32 Release"

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

ALL : "$(OUTDIR)\Saver.scr"

CLEAN : 
	-@erase ".\Release\Saver.idb"
	-@erase ".\Release\Saver.pch"
	-@erase ".\Release\Saver.scr"
	-@erase ".\Release\Saver.obj"
	-@erase ".\Release\Drawwnd.obj"
	-@erase ".\Release\Saverdlg.obj"
	-@erase ".\Release\Saverwnd.obj"
	-@erase ".\Release\Stdafx.obj"
	-@erase ".\Release\Saver.res"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"Stdafx.h" /c
CPP_PROJ=/nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/Saver.pch" /Yu"Stdafx.h" /Fo"$(INTDIR)/"\
 /Fd"$(OUTDIR)/Saver.pdb" /c 
CPP_OBJS=.\Release/
CPP_SBRS=
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/Saver.res" /d "NDEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/Saver.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 /nologo /subsystem:windows /machine:I386 /out:"Release/Saver.scr"
LINK32_FLAGS=/nologo /subsystem:windows /incremental:no\
 /pdb:"$(OUTDIR)/Saver.pdb" /machine:I386 /out:"$(OUTDIR)/Saver.scr" 
LINK32_OBJS= \
	"$(INTDIR)/Saver.obj" \
	"$(INTDIR)/Drawwnd.obj" \
	"$(INTDIR)/Saverdlg.obj" \
	"$(INTDIR)/Saverwnd.obj" \
	"$(INTDIR)/Stdafx.obj" \
	"$(INTDIR)/Saver.res"

"$(OUTDIR)\Saver.scr" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "Saver - Win32 Debug"

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

ALL : "$(OUTDIR)\Saver.scr"

CLEAN : 
	-@erase ".\Debug\Saver.ilk"
	-@erase ".\Debug\Saver.res"
	-@erase ".\Debug\Saver.obj"
	-@erase ".\Debug\Saver.pch"
	-@erase ".\Debug\Drawwnd.obj"
	-@erase ".\Debug\Saverdlg.obj"
	-@erase ".\Debug\Saverwnd.obj"
	-@erase ".\Debug\Stdafx.obj"
	-@erase ".\Debug\Saver.pdb"
	-@erase ".\Debug\Saver.idb"
	-@erase ".\Debug\Saver.scr"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"Stdafx.h" /c
CPP_PROJ=/nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/Saver.pch" /Yu"Stdafx.h" /Fo"$(INTDIR)/"\
 /Fd"$(OUTDIR)/Saver.pdb" /c 
CPP_OBJS=.\Debug/
CPP_SBRS=
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/Saver.res" /d "_DEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/Saver.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386
# ADD LINK32 /nologo /subsystem:windows /debug /machine:I386 /out:"Debug/Saver.scr"
LINK32_FLAGS=/nologo /subsystem:windows /incremental:yes\
 /pdb:"$(OUTDIR)/Saver.pdb" /debug /machine:I386 /out:"$(OUTDIR)/Saver.scr" 
LINK32_OBJS= \
	"$(INTDIR)/Saver.obj" \
	"$(INTDIR)/Drawwnd.obj" \
	"$(INTDIR)/Saverdlg.obj" \
	"$(INTDIR)/Saverwnd.obj" \
	"$(INTDIR)/Stdafx.obj" \
	"$(INTDIR)/Saver.res"

"$(OUTDIR)\Saver.scr" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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

# Name "Saver - Win32 Release"
# Name "Saver - Win32 Debug"

!IF  "$(CFG)" == "Saver - Win32 Release"

!ELSEIF  "$(CFG)" == "Saver - Win32 Debug"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\Saver.rc
DEP_RSC_SAVER=\
	".\res\Saver.ico"\
	".\res\cursor1.cur"\
	".\res\Saver.rc2"\
	

"$(INTDIR)\Saver.res" : $(SOURCE) $(DEP_RSC_SAVER) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\Saver.cpp
DEP_CPP_SAVER_=\
	".\Saver.h"\
	".\Saverdlg.h"\
	".\drawwnd.h"\
	".\saverwnd.h"\
	".\stdafx.h"\
	

!IF  "$(CFG)" == "Saver - Win32 Release"


"$(INTDIR)\Saver.obj" : $(SOURCE) $(DEP_CPP_SAVER_) "$(INTDIR)"\
 "$(INTDIR)\Saver.pch"

"$(INTDIR)\Saver.idb" : $(SOURCE) $(DEP_CPP_SAVER_) "$(INTDIR)"\
 "$(INTDIR)\Saver.pch"


!ELSEIF  "$(CFG)" == "Saver - Win32 Debug"


"$(INTDIR)\Saver.obj" : $(SOURCE) $(DEP_CPP_SAVER_) "$(INTDIR)"\
 "$(INTDIR)\Saver.pch"

"$(INTDIR)\Saver.pdb" : $(SOURCE) $(DEP_CPP_SAVER_) "$(INTDIR)"\
 "$(INTDIR)\Saver.pch"

"$(INTDIR)\Saver.idb" : $(SOURCE) $(DEP_CPP_SAVER_) "$(INTDIR)"\
 "$(INTDIR)\Saver.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Drawwnd.cpp
DEP_CPP_DRAWW=\
	".\Saver.h"\
	".\drawwnd.h"\
	".\stdafx.h"\
	

!IF  "$(CFG)" == "Saver - Win32 Release"


"$(INTDIR)\Drawwnd.obj" : $(SOURCE) $(DEP_CPP_DRAWW) "$(INTDIR)"\
 "$(INTDIR)\Saver.pch"

"$(INTDIR)\Saver.idb" : $(SOURCE) $(DEP_CPP_DRAWW) "$(INTDIR)"\
 "$(INTDIR)\Saver.pch"


!ELSEIF  "$(CFG)" == "Saver - Win32 Debug"


"$(INTDIR)\Drawwnd.obj" : $(SOURCE) $(DEP_CPP_DRAWW) "$(INTDIR)"\
 "$(INTDIR)\Saver.pch"

"$(INTDIR)\Saver.pdb" : $(SOURCE) $(DEP_CPP_DRAWW) "$(INTDIR)"\
 "$(INTDIR)\Saver.pch"

"$(INTDIR)\Saver.idb" : $(SOURCE) $(DEP_CPP_DRAWW) "$(INTDIR)"\
 "$(INTDIR)\Saver.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Saverdlg.cpp
DEP_CPP_SAVERD=\
	".\Saver.h"\
	".\Saverdlg.h"\
	".\drawwnd.h"\
	".\stdafx.h"\
	

!IF  "$(CFG)" == "Saver - Win32 Release"


"$(INTDIR)\Saverdlg.obj" : $(SOURCE) $(DEP_CPP_SAVERD) "$(INTDIR)"\
 "$(INTDIR)\Saver.pch"

"$(INTDIR)\Saver.idb" : $(SOURCE) $(DEP_CPP_SAVERD) "$(INTDIR)"\
 "$(INTDIR)\Saver.pch"


!ELSEIF  "$(CFG)" == "Saver - Win32 Debug"


"$(INTDIR)\Saverdlg.obj" : $(SOURCE) $(DEP_CPP_SAVERD) "$(INTDIR)"\
 "$(INTDIR)\Saver.pch"

"$(INTDIR)\Saver.pdb" : $(SOURCE) $(DEP_CPP_SAVERD) "$(INTDIR)"\
 "$(INTDIR)\Saver.pch"

"$(INTDIR)\Saver.idb" : $(SOURCE) $(DEP_CPP_SAVERD) "$(INTDIR)"\
 "$(INTDIR)\Saver.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Saverwnd.cpp
DEP_CPP_SAVERW=\
	".\Saver.h"\
	".\drawwnd.h"\
	".\saverwnd.h"\
	".\stdafx.h"\
	

!IF  "$(CFG)" == "Saver - Win32 Release"


"$(INTDIR)\Saverwnd.obj" : $(SOURCE) $(DEP_CPP_SAVERW) "$(INTDIR)"\
 "$(INTDIR)\Saver.pch"

"$(INTDIR)\Saver.idb" : $(SOURCE) $(DEP_CPP_SAVERW) "$(INTDIR)"\
 "$(INTDIR)\Saver.pch"


!ELSEIF  "$(CFG)" == "Saver - Win32 Debug"


"$(INTDIR)\Saverwnd.obj" : $(SOURCE) $(DEP_CPP_SAVERW) "$(INTDIR)"\
 "$(INTDIR)\Saver.pch"

"$(INTDIR)\Saver.pdb" : $(SOURCE) $(DEP_CPP_SAVERW) "$(INTDIR)"\
 "$(INTDIR)\Saver.pch"

"$(INTDIR)\Saver.idb" : $(SOURCE) $(DEP_CPP_SAVERW) "$(INTDIR)"\
 "$(INTDIR)\Saver.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Stdafx.cpp
DEP_CPP_STDAF=\
	".\stdafx.h"\
	

!IF  "$(CFG)" == "Saver - Win32 Release"

# ADD CPP /Yc"Stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/Saver.pch" /Yc"Stdafx.h" /Fo"$(INTDIR)/"\
 /Fd"$(OUTDIR)/Saver.pdb" /c $(SOURCE) \
	

"$(INTDIR)\Stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\Saver.idb" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\Saver.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "Saver - Win32 Debug"

# ADD CPP /Yc"Stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/Saver.pch" /Yc"Stdafx.h" /Fo"$(INTDIR)/"\
 /Fd"$(OUTDIR)/Saver.pdb" /c $(SOURCE) \
	

"$(INTDIR)\Stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\Saver.pdb" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\Saver.idb" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\Saver.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
# End Target
# End Project
################################################################################
