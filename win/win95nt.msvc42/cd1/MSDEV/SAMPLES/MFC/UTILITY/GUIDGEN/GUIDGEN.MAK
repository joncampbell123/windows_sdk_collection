# Microsoft Visual C++ Generated NMAKE File, Format Version 40000
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

!IF "$(CFG)" == ""
CFG=GuidGen - Win32 Debug
!MESSAGE No configuration specified.  Defaulting to GuidGen - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "GuidGen - Win32 Release" && "$(CFG)" !=\
 "GuidGen - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "GuidGen.mak" CFG="GuidGen - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "GuidGen - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "GuidGen - Win32 Debug" (based on "Win32 (x86) Application")
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
CPP=cl.exe
MTL=mktyplib.exe
RSC=rc.exe

!IF  "$(CFG)" == "GuidGen - Win32 Release"

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

ALL : "$(OUTDIR)\GuidGen.exe" "$(OUTDIR)\GuidGen.bsc"

CLEAN : 
	-@erase ".\Release\GuidGen.exe"
	-@erase ".\Release\Stdafx.obj"
	-@erase ".\Release\Guidgen.obj"
	-@erase ".\Release\GuidGen.pch"
	-@erase ".\Release\Guidgen.res"
	-@erase ".\Release\Guidgdlg.obj"
	-@erase ".\Release\GuidGen.idb"
	-@erase ".\Release\GuidGen.bsc"
	-@erase ".\Release\Stdafx.sbr"
	-@erase ".\Release\Guidgen.sbr"
	-@erase ".\Release\Guidgdlg.sbr"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /Yu"Stdafx.h" /c
CPP_PROJ=/nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /FR"$(INTDIR)/" /Fp"$(INTDIR)/GuidGen.pch" /Yu"Stdafx.h"\
 /Fo"$(INTDIR)/" /Fd"$(OUTDIR)/GuidGen.pdb" /c 
CPP_OBJS=.\Release/
CPP_SBRS=.\Release/
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/Guidgen.res" /d "NDEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/GuidGen.bsc" 
BSC32_SBRS= \
	"$(INTDIR)/Stdafx.sbr" \
	"$(INTDIR)/Guidgen.sbr" \
	"$(INTDIR)/Guidgdlg.sbr"

"$(OUTDIR)\GuidGen.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 /nologo /subsystem:windows /machine:I386
LINK32_FLAGS=/nologo /subsystem:windows /incremental:no\
 /pdb:"$(OUTDIR)/GuidGen.pdb" /machine:I386 /out:"$(OUTDIR)/GuidGen.exe" 
LINK32_OBJS= \
	"$(INTDIR)/Stdafx.obj" \
	"$(INTDIR)/Guidgen.obj" \
	"$(INTDIR)/Guidgdlg.obj" \
	"$(INTDIR)/Guidgen.res"

"$(OUTDIR)\GuidGen.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "GuidGen - Win32 Debug"

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

ALL : "$(OUTDIR)\GuidGen.exe" "$(OUTDIR)\GuidGen.bsc"

CLEAN : 
	-@erase ".\Debug\GuidGen.exe"
	-@erase ".\Debug\Stdafx.obj"
	-@erase ".\Debug\Guidgen.obj"
	-@erase ".\Debug\GuidGen.pch"
	-@erase ".\Debug\Guidgen.res"
	-@erase ".\Debug\Guidgdlg.obj"
	-@erase ".\Debug\GuidGen.ilk"
	-@erase ".\Debug\GuidGen.pdb"
	-@erase ".\Debug\GuidGen.idb"
	-@erase ".\Debug\GuidGen.bsc"
	-@erase ".\Debug\Stdafx.sbr"
	-@erase ".\Debug\Guidgen.sbr"
	-@erase ".\Debug\Guidgdlg.sbr"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /Yu"Stdafx.h" /c
CPP_PROJ=/nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /D "_AFXDLL" /D "_MBCS" /FR"$(INTDIR)/" /Fp"$(INTDIR)/GuidGen.pch"\
 /Yu"Stdafx.h" /Fo"$(INTDIR)/" /Fd"$(OUTDIR)/GuidGen.pdb" /c 
CPP_OBJS=.\Debug/
CPP_SBRS=.\Debug/
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/Guidgen.res" /d "_DEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/GuidGen.bsc" 
BSC32_SBRS= \
	"$(INTDIR)/Stdafx.sbr" \
	"$(INTDIR)/Guidgen.sbr" \
	"$(INTDIR)/Guidgdlg.sbr"

"$(OUTDIR)\GuidGen.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386
# ADD LINK32 /nologo /subsystem:windows /debug /machine:I386
LINK32_FLAGS=/nologo /subsystem:windows /incremental:yes\
 /pdb:"$(OUTDIR)/GuidGen.pdb" /debug /machine:I386 /out:"$(OUTDIR)/GuidGen.exe" 
LINK32_OBJS= \
	"$(INTDIR)/Stdafx.obj" \
	"$(INTDIR)/Guidgen.obj" \
	"$(INTDIR)/Guidgdlg.obj" \
	"$(INTDIR)/Guidgen.res"

"$(OUTDIR)\GuidGen.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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

# Name "GuidGen - Win32 Release"
# Name "GuidGen - Win32 Debug"

!IF  "$(CFG)" == "GuidGen - Win32 Release"

!ELSEIF  "$(CFG)" == "GuidGen - Win32 Debug"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\Stdafx.cpp
DEP_CPP_STDAF=\
	".\stdafx.h"\
	

!IF  "$(CFG)" == "GuidGen - Win32 Release"

# ADD CPP /Yc

BuildCmds= \
	$(CPP) /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /FR"$(INTDIR)/" /Fp"$(INTDIR)/GuidGen.pch" /Yc"Stdafx.h"\
 /Fo"$(INTDIR)/" /Fd"$(OUTDIR)/GuidGen.pdb" /c $(SOURCE) \
	

"$(INTDIR)\Stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\Stdafx.sbr" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\GuidGen.idb" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\GuidGen.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "GuidGen - Win32 Debug"

# ADD CPP /Yc

BuildCmds= \
	$(CPP) /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /D "_AFXDLL" /D "_MBCS" /FR"$(INTDIR)/" /Fp"$(INTDIR)/GuidGen.pch"\
 /Yc"Stdafx.h" /Fo"$(INTDIR)/" /Fd"$(OUTDIR)/GuidGen.pdb" /c $(SOURCE) \
	

"$(INTDIR)\Stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\Stdafx.sbr" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\GuidGen.pdb" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\GuidGen.idb" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\GuidGen.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Guidgen.cpp
DEP_CPP_GUIDG=\
	".\guidgdlg.h"\
	".\guidgen.h"\
	".\resource.h"\
	".\stdafx.h"\
	

!IF  "$(CFG)" == "GuidGen - Win32 Release"


"$(INTDIR)\Guidgen.obj" : $(SOURCE) $(DEP_CPP_GUIDG) "$(INTDIR)"\
 "$(INTDIR)\GuidGen.pch"

"$(INTDIR)\Guidgen.sbr" : $(SOURCE) $(DEP_CPP_GUIDG) "$(INTDIR)"\
 "$(INTDIR)\GuidGen.pch"

"$(INTDIR)\GuidGen.idb" : $(SOURCE) $(DEP_CPP_GUIDG) "$(INTDIR)"\
 "$(INTDIR)\GuidGen.pch"


!ELSEIF  "$(CFG)" == "GuidGen - Win32 Debug"


"$(INTDIR)\Guidgen.obj" : $(SOURCE) $(DEP_CPP_GUIDG) "$(INTDIR)"\
 "$(INTDIR)\GuidGen.pch"

"$(INTDIR)\Guidgen.sbr" : $(SOURCE) $(DEP_CPP_GUIDG) "$(INTDIR)"\
 "$(INTDIR)\GuidGen.pch"

"$(INTDIR)\GuidGen.pdb" : $(SOURCE) $(DEP_CPP_GUIDG) "$(INTDIR)"\
 "$(INTDIR)\GuidGen.pch"

"$(INTDIR)\GuidGen.idb" : $(SOURCE) $(DEP_CPP_GUIDG) "$(INTDIR)"\
 "$(INTDIR)\GuidGen.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Guidgen.rc
DEP_RSC_GUIDGE=\
	".\res\guidgen.ico"\
	".\res\guidgen.rc2"\
	

"$(INTDIR)\Guidgen.res" : $(SOURCE) $(DEP_RSC_GUIDGE) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\Guidgdlg.cpp
DEP_CPP_GUIDGD=\
	".\guidgdlg.h"\
	".\guidgen.h"\
	".\resource.h"\
	".\stdafx.h"\
	

!IF  "$(CFG)" == "GuidGen - Win32 Release"


"$(INTDIR)\Guidgdlg.obj" : $(SOURCE) $(DEP_CPP_GUIDGD) "$(INTDIR)"\
 "$(INTDIR)\GuidGen.pch"

"$(INTDIR)\Guidgdlg.sbr" : $(SOURCE) $(DEP_CPP_GUIDGD) "$(INTDIR)"\
 "$(INTDIR)\GuidGen.pch"

"$(INTDIR)\GuidGen.idb" : $(SOURCE) $(DEP_CPP_GUIDGD) "$(INTDIR)"\
 "$(INTDIR)\GuidGen.pch"


!ELSEIF  "$(CFG)" == "GuidGen - Win32 Debug"


"$(INTDIR)\Guidgdlg.obj" : $(SOURCE) $(DEP_CPP_GUIDGD) "$(INTDIR)"\
 "$(INTDIR)\GuidGen.pch"

"$(INTDIR)\Guidgdlg.sbr" : $(SOURCE) $(DEP_CPP_GUIDGD) "$(INTDIR)"\
 "$(INTDIR)\GuidGen.pch"

"$(INTDIR)\GuidGen.pdb" : $(SOURCE) $(DEP_CPP_GUIDGD) "$(INTDIR)"\
 "$(INTDIR)\GuidGen.pch"

"$(INTDIR)\GuidGen.idb" : $(SOURCE) $(DEP_CPP_GUIDGD) "$(INTDIR)"\
 "$(INTDIR)\GuidGen.pch"


!ENDIF 

# End Source File
# End Target
# End Project
################################################################################
