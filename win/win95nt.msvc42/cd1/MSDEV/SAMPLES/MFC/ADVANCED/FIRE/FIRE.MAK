# Microsoft Visual C++ Generated NMAKE File, Format Version 40000
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

!IF "$(CFG)" == ""
CFG=Fire - Win32 Debug
!MESSAGE No configuration specified.  Defaulting to Fire - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "Fire - Win32 Release" && "$(CFG)" != "Fire - Win32 Debug" &&\
 "$(CFG)" != "Fire - Win32 Profile Release"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "fire.mak" CFG="Fire - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Fire - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "Fire - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE "Fire - Win32 Profile Release" (based on "Win32 (x86) Application")
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
# PROP Target_Last_Scanned "Fire - Win32 Debug"
RSC=rc.exe
MTL=mktyplib.exe
CPP=cl.exe

!IF  "$(CFG)" == "Fire - Win32 Release"

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

ALL : "$(OUTDIR)\fire.exe"

CLEAN : 
	-@erase ".\Release\fire.idb"
	-@erase ".\Release\fire.pch"
	-@erase ".\Release\fire.exe"
	-@erase ".\Release\firewnd.obj"
	-@erase ".\Release\fire.obj"
	-@erase ".\Release\firedlg.obj"
	-@erase ".\Release\stdafx.obj"
	-@erase ".\Release\fire.res"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"Stdafx.h" /c
# SUBTRACT CPP /Fr
CPP_PROJ=/nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/fire.pch" /Yu"Stdafx.h" /Fo"$(INTDIR)/"\
 /Fd"$(OUTDIR)/fire.pdb" /c 
CPP_OBJS=.\Release/
CPP_SBRS=
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/fire.res" /d "NDEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/fire.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 /nologo /subsystem:windows /machine:I386
LINK32_FLAGS=/nologo /subsystem:windows /incremental:no\
 /pdb:"$(OUTDIR)/fire.pdb" /machine:I386 /out:"$(OUTDIR)/fire.exe" 
LINK32_OBJS= \
	"$(INTDIR)/firewnd.obj" \
	"$(INTDIR)/fire.obj" \
	"$(INTDIR)/firedlg.obj" \
	"$(INTDIR)/stdafx.obj" \
	"$(INTDIR)/fire.res"

"$(OUTDIR)\fire.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "Fire - Win32 Debug"

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

ALL : "$(OUTDIR)\fire.exe"

CLEAN : 
	-@erase ".\Debug\fire.pdb"
	-@erase ".\Debug\fire.pch"
	-@erase ".\Debug\firedlg.obj"
	-@erase ".\Debug\stdafx.obj"
	-@erase ".\Debug\firewnd.obj"
	-@erase ".\Debug\fire.obj"
	-@erase ".\Debug\fire.res"
	-@erase ".\Debug\fire.idb"
	-@erase ".\Debug\fire.exe"
	-@erase ".\Debug\fire.ilk"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"Stdafx.h" /c
# SUBTRACT CPP /Fr
CPP_PROJ=/nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/fire.pch" /Yu"Stdafx.h" /Fo"$(INTDIR)/"\
 /Fd"$(OUTDIR)/fire.pdb" /c 
CPP_OBJS=.\Debug/
CPP_SBRS=
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/fire.res" /d "_DEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/fire.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386
# ADD LINK32 /nologo /subsystem:windows /debug /machine:I386
LINK32_FLAGS=/nologo /subsystem:windows /incremental:yes\
 /pdb:"$(OUTDIR)/fire.pdb" /debug /machine:I386 /out:"$(OUTDIR)/fire.exe" 
LINK32_OBJS= \
	"$(INTDIR)/firedlg.obj" \
	"$(INTDIR)/stdafx.obj" \
	"$(INTDIR)/firewnd.obj" \
	"$(INTDIR)/fire.obj" \
	"$(INTDIR)/fire.res"

"$(OUTDIR)\fire.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "Fire - Win32 Profile Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir Release
# PROP BASE Intermediate_Dir Release
# PROP BASE Target_Dir
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ProfRelease
# PROP Intermediate_Dir ProfRelease
# PROP Target_Dir
OUTDIR=.\ProfRelease
INTDIR=.\ProfRelease

ALL : "$(OUTDIR)\fire.exe"

CLEAN : 
	-@erase ".\ProfRelease\fire.idb"
	-@erase ".\ProfRelease\fire.pch"
	-@erase ".\ProfRelease\fire.exe"
	-@erase ".\ProfRelease\firedlg.obj"
	-@erase ".\ProfRelease\stdafx.obj"
	-@erase ".\ProfRelease\firewnd.obj"
	-@erase ".\ProfRelease\fire.obj"
	-@erase ".\ProfRelease\fire.res"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"Stdafx.h" /c
# SUBTRACT CPP /Fr
CPP_PROJ=/nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/fire.pch" /Yu"Stdafx.h" /Fo"$(INTDIR)/"\
 /Fd"$(OUTDIR)/fire.pdb" /c 
CPP_OBJS=.\ProfRelease/
CPP_SBRS=
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/fire.res" /d "NDEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/fire.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 /nologo /subsystem:windows /profile /machine:I386
LINK32_FLAGS=/nologo /subsystem:windows /profile /machine:I386\
 /out:"$(OUTDIR)/fire.exe" 
LINK32_OBJS= \
	"$(INTDIR)/firedlg.obj" \
	"$(INTDIR)/stdafx.obj" \
	"$(INTDIR)/firewnd.obj" \
	"$(INTDIR)/fire.obj" \
	"$(INTDIR)/fire.res"

"$(OUTDIR)\fire.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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

# Name "Fire - Win32 Release"
# Name "Fire - Win32 Debug"
# Name "Fire - Win32 Profile Release"

!IF  "$(CFG)" == "Fire - Win32 Release"

!ELSEIF  "$(CFG)" == "Fire - Win32 Debug"

!ELSEIF  "$(CFG)" == "Fire - Win32 Profile Release"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\fire.rc
DEP_RSC_FIRE_=\
	".\RES\fire.ico"\
	".\RES\fire.rc2"\
	

!IF  "$(CFG)" == "Fire - Win32 Release"


"$(INTDIR)\fire.res" : $(SOURCE) $(DEP_RSC_FIRE_) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Fire - Win32 Debug"


"$(INTDIR)\fire.res" : $(SOURCE) $(DEP_RSC_FIRE_) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Fire - Win32 Profile Release"


"$(INTDIR)\fire.res" : $(SOURCE) $(DEP_RSC_FIRE_) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\fire.cpp
DEP_CPP_FIRE_C=\
	".\fire.h"\
	".\firedlg.h"\
	".\firewnd.h"\
	".\stdafx.h"\
	

!IF  "$(CFG)" == "Fire - Win32 Release"


BuildCmds= \
	$(CPP) /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/fire.pch" /Yu"Stdafx.h" /Fo"$(INTDIR)/"\
 /Fd"$(OUTDIR)/fire.pdb" /c $(SOURCE) \
	

"$(INTDIR)\fire.obj" : $(SOURCE) $(DEP_CPP_FIRE_C) "$(INTDIR)"\
 "$(INTDIR)\fire.pch"
   $(BuildCmds)

"$(INTDIR)\fire.idb" : $(SOURCE) $(DEP_CPP_FIRE_C) "$(INTDIR)"\
 "$(INTDIR)\fire.pch"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "Fire - Win32 Debug"

# ADD CPP /Yu"Stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/fire.pch" /Yu"Stdafx.h" /Fo"$(INTDIR)/"\
 /Fd"$(OUTDIR)/fire.pdb" /c $(SOURCE) \
	

"$(INTDIR)\fire.obj" : $(SOURCE) $(DEP_CPP_FIRE_C) "$(INTDIR)"\
 "$(INTDIR)\fire.pch"
   $(BuildCmds)

"$(INTDIR)\fire.pdb" : $(SOURCE) $(DEP_CPP_FIRE_C) "$(INTDIR)"\
 "$(INTDIR)\fire.pch"
   $(BuildCmds)

"$(INTDIR)\fire.idb" : $(SOURCE) $(DEP_CPP_FIRE_C) "$(INTDIR)"\
 "$(INTDIR)\fire.pch"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "Fire - Win32 Profile Release"


BuildCmds= \
	$(CPP) /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/fire.pch" /Yu"Stdafx.h" /Fo"$(INTDIR)/"\
 /Fd"$(OUTDIR)/fire.pdb" /c $(SOURCE) \
	

"$(INTDIR)\fire.obj" : $(SOURCE) $(DEP_CPP_FIRE_C) "$(INTDIR)"\
 "$(INTDIR)\fire.pch"
   $(BuildCmds)

"$(INTDIR)\fire.idb" : $(SOURCE) $(DEP_CPP_FIRE_C) "$(INTDIR)"\
 "$(INTDIR)\fire.pch"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\firedlg.cpp
DEP_CPP_FIRED=\
	".\fire.h"\
	".\firedlg.h"\
	".\firewnd.h"\
	".\stdafx.h"\
	

!IF  "$(CFG)" == "Fire - Win32 Release"


BuildCmds= \
	$(CPP) /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/fire.pch" /Yu"Stdafx.h" /Fo"$(INTDIR)/"\
 /Fd"$(OUTDIR)/fire.pdb" /c $(SOURCE) \
	

"$(INTDIR)\firedlg.obj" : $(SOURCE) $(DEP_CPP_FIRED) "$(INTDIR)"\
 "$(INTDIR)\fire.pch"
   $(BuildCmds)

"$(INTDIR)\fire.idb" : $(SOURCE) $(DEP_CPP_FIRED) "$(INTDIR)"\
 "$(INTDIR)\fire.pch"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "Fire - Win32 Debug"

# ADD CPP /Yu

BuildCmds= \
	$(CPP) /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/fire.pch" /Yu"Stdafx.h" /Fo"$(INTDIR)/"\
 /Fd"$(OUTDIR)/fire.pdb" /c $(SOURCE) \
	

"$(INTDIR)\firedlg.obj" : $(SOURCE) $(DEP_CPP_FIRED) "$(INTDIR)"\
 "$(INTDIR)\fire.pch"
   $(BuildCmds)

"$(INTDIR)\fire.pdb" : $(SOURCE) $(DEP_CPP_FIRED) "$(INTDIR)"\
 "$(INTDIR)\fire.pch"
   $(BuildCmds)

"$(INTDIR)\fire.idb" : $(SOURCE) $(DEP_CPP_FIRED) "$(INTDIR)"\
 "$(INTDIR)\fire.pch"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "Fire - Win32 Profile Release"


BuildCmds= \
	$(CPP) /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/fire.pch" /Yu"Stdafx.h" /Fo"$(INTDIR)/"\
 /Fd"$(OUTDIR)/fire.pdb" /c $(SOURCE) \
	

"$(INTDIR)\firedlg.obj" : $(SOURCE) $(DEP_CPP_FIRED) "$(INTDIR)"\
 "$(INTDIR)\fire.pch"
   $(BuildCmds)

"$(INTDIR)\fire.idb" : $(SOURCE) $(DEP_CPP_FIRED) "$(INTDIR)"\
 "$(INTDIR)\fire.pch"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\firewnd.cpp
DEP_CPP_FIREW=\
	".\fire.h"\
	".\firewnd.h"\
	".\stdafx.h"\
	

!IF  "$(CFG)" == "Fire - Win32 Release"


"$(INTDIR)\firewnd.obj" : $(SOURCE) $(DEP_CPP_FIREW) "$(INTDIR)"\
 "$(INTDIR)\fire.pch"

"$(INTDIR)\fire.idb" : $(SOURCE) $(DEP_CPP_FIREW) "$(INTDIR)"\
 "$(INTDIR)\fire.pch"


!ELSEIF  "$(CFG)" == "Fire - Win32 Debug"


"$(INTDIR)\firewnd.obj" : $(SOURCE) $(DEP_CPP_FIREW) "$(INTDIR)"\
 "$(INTDIR)\fire.pch"

"$(INTDIR)\fire.pdb" : $(SOURCE) $(DEP_CPP_FIREW) "$(INTDIR)"\
 "$(INTDIR)\fire.pch"

"$(INTDIR)\fire.idb" : $(SOURCE) $(DEP_CPP_FIREW) "$(INTDIR)"\
 "$(INTDIR)\fire.pch"


!ELSEIF  "$(CFG)" == "Fire - Win32 Profile Release"


"$(INTDIR)\firewnd.obj" : $(SOURCE) $(DEP_CPP_FIREW) "$(INTDIR)"\
 "$(INTDIR)\fire.pch"

"$(INTDIR)\fire.idb" : $(SOURCE) $(DEP_CPP_FIREW) "$(INTDIR)"\
 "$(INTDIR)\fire.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\stdafx.cpp
DEP_CPP_STDAF=\
	".\stdafx.h"\
	

!IF  "$(CFG)" == "Fire - Win32 Release"

# ADD CPP /Yc"Stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/fire.pch" /Yc"Stdafx.h" /Fo"$(INTDIR)/"\
 /Fd"$(OUTDIR)/fire.pdb" /c $(SOURCE) \
	

"$(INTDIR)\stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\fire.idb" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\fire.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "Fire - Win32 Debug"

# ADD CPP /Yc

BuildCmds= \
	$(CPP) /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/fire.pch" /Yc /Fo"$(INTDIR)/"\
 /Fd"$(OUTDIR)/fire.pdb" /c $(SOURCE) \
	

"$(INTDIR)\stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\fire.pdb" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\fire.idb" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\fire.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "Fire - Win32 Profile Release"

# ADD CPP /Yc

BuildCmds= \
	$(CPP) /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/fire.pch" /Yc /Fo"$(INTDIR)/"\
 /Fd"$(OUTDIR)/fire.pdb" /c $(SOURCE) \
	

"$(INTDIR)\stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\fire.idb" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\fire.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
# End Target
# End Project
################################################################################
