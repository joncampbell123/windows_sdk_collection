# Microsoft Visual C++ Generated NMAKE File, Format Version 40000
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

!IF "$(CFG)" == ""
CFG=autodriv - Win32 (80x86) Debug
!MESSAGE No configuration specified.  Defaulting to autodriv - Win32 (80x86)\
 Debug.
!ENDIF 

!IF "$(CFG)" != "autodriv - Win32 (80x86) Debug" && "$(CFG)" !=\
 "autodriv - Win32 (80x86) Release" && "$(CFG)" !=\
 "autodriv - Win32 (80x86) Unicode Debug" && "$(CFG)" !=\
 "autodriv - Win32 (80x86) Unicode Release"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "autodriv.mak" CFG="autodriv - Win32 (80x86) Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "autodriv - Win32 (80x86) Debug" (based on "Win32 (x86) Application")
!MESSAGE "autodriv - Win32 (80x86) Release" (based on\
 "Win32 (x86) Application")
!MESSAGE "autodriv - Win32 (80x86) Unicode Debug" (based on\
 "Win32 (x86) Application")
!MESSAGE "autodriv - Win32 (80x86) Unicode Release" (based on\
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
# PROP Target_Last_Scanned "autodriv - Win32 (80x86) Debug"
CPP=cl.exe
RSC=rc.exe
MTL=mktyplib.exe

!IF  "$(CFG)" == "autodriv - Win32 (80x86) Debug"

# PROP BASE Use_MFC 5
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir WinDebug
# PROP BASE Intermediate_Dir WinDebug
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir WinDebug
# PROP Intermediate_Dir WinDebug
OUTDIR=.\WinDebug
INTDIR=.\WinDebug

ALL : "$(OUTDIR)\autodriv.exe" "$(OUTDIR)\autodriv.bsc"

CLEAN : 
	-@erase ".\WinDebug\vc40.pdb"
	-@erase ".\WinDebug\autodriv.pch"
	-@erase ".\WinDebug\autodriv.bsc"
	-@erase ".\WinDebug\stdafx.sbr"
	-@erase ".\WinDebug\autoddlg.sbr"
	-@erase ".\WinDebug\autoclik.sbr"
	-@erase ".\WinDebug\autodriv.sbr"
	-@erase ".\WinDebug\autodriv.exe"
	-@erase ".\WinDebug\autodriv.obj"
	-@erase ".\WinDebug\stdafx.obj"
	-@erase ".\WinDebug\autoddlg.obj"
	-@erase ".\WinDebug\autoclik.obj"
	-@erase ".\WinDebug\autodriv.res"
	-@erase ".\WinDebug\autodriv.ilk"
	-@erase ".\WinDebug\autodriv.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MT /W3 /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /FR /Yu"stdafx.h" /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /Yu"stdafx.h" /c
CPP_PROJ=/nologo /MDd /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS"\
 /D "_AFXDLL" /D "_MBCS" /FR"$(INTDIR)/" /Fp"$(INTDIR)/autodriv.pch"\
 /Yu"stdafx.h" /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\WinDebug/
CPP_SBRS=.\WinDebug/
# ADD BASE RSC /l 0x1 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/autodriv.res" /d "_DEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# SUBTRACT BASE BSC32 /Iu
# ADD BSC32 /nologo
# SUBTRACT BSC32 /Iu
BSC32_FLAGS=/nologo /o"$(OUTDIR)/autodriv.bsc" 
BSC32_SBRS= \
	"$(INTDIR)/stdafx.sbr" \
	"$(INTDIR)/autoddlg.sbr" \
	"$(INTDIR)/autoclik.sbr" \
	"$(INTDIR)/autodriv.sbr"

"$(OUTDIR)\autodriv.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386
# ADD LINK32 /nologo /subsystem:windows /debug /machine:I386
# SUBTRACT LINK32 /pdb:none
LINK32_FLAGS=/nologo /subsystem:windows /incremental:yes\
 /pdb:"$(OUTDIR)/autodriv.pdb" /debug /machine:I386\
 /out:"$(OUTDIR)/autodriv.exe" 
LINK32_OBJS= \
	"$(INTDIR)/autodriv.obj" \
	"$(INTDIR)/stdafx.obj" \
	"$(INTDIR)/autoddlg.obj" \
	"$(INTDIR)/autoclik.obj" \
	"$(INTDIR)/autodriv.res"

"$(OUTDIR)\autodriv.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "autodriv - Win32 (80x86) Release"

# PROP BASE Use_MFC 5
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir WinRel
# PROP BASE Intermediate_Dir WinRel
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir WinRel
# PROP Intermediate_Dir WinRel
OUTDIR=.\WinRel
INTDIR=.\WinRel

ALL : "$(OUTDIR)\autodriv.exe" "$(OUTDIR)\autodriv.bsc"

CLEAN : 
	-@erase ".\WinRel\autodriv.bsc"
	-@erase ".\WinRel\autoclik.sbr"
	-@erase ".\WinRel\autodriv.pch"
	-@erase ".\WinRel\autodriv.sbr"
	-@erase ".\WinRel\autoddlg.sbr"
	-@erase ".\WinRel\stdafx.sbr"
	-@erase ".\WinRel\autodriv.exe"
	-@erase ".\WinRel\autoddlg.obj"
	-@erase ".\WinRel\stdafx.obj"
	-@erase ".\WinRel\autoclik.obj"
	-@erase ".\WinRel\autodriv.obj"
	-@erase ".\WinRel\autodriv.res"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /FR /Yu"stdafx.h" /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /Yu"stdafx.h" /c
CPP_PROJ=/nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /FR"$(INTDIR)/" /Fp"$(INTDIR)/autodriv.pch" /Yu"stdafx.h"\
 /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\WinRel/
CPP_SBRS=.\WinRel/
# ADD BASE RSC /l 0x1 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/autodriv.res" /d "NDEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# SUBTRACT BASE BSC32 /Iu
# ADD BSC32 /nologo
# SUBTRACT BSC32 /Iu
BSC32_FLAGS=/nologo /o"$(OUTDIR)/autodriv.bsc" 
BSC32_SBRS= \
	"$(INTDIR)/autoclik.sbr" \
	"$(INTDIR)/autodriv.sbr" \
	"$(INTDIR)/autoddlg.sbr" \
	"$(INTDIR)/stdafx.sbr"

"$(OUTDIR)\autodriv.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 /nologo /subsystem:windows /machine:I386
# SUBTRACT LINK32 /pdb:none
LINK32_FLAGS=/nologo /subsystem:windows /incremental:no\
 /pdb:"$(OUTDIR)/autodriv.pdb" /machine:I386 /out:"$(OUTDIR)/autodriv.exe" 
LINK32_OBJS= \
	"$(INTDIR)/autoddlg.obj" \
	"$(INTDIR)/stdafx.obj" \
	"$(INTDIR)/autoclik.obj" \
	"$(INTDIR)/autodriv.obj" \
	"$(INTDIR)/autodriv.res"

"$(OUTDIR)\autodriv.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "autodriv - Win32 (80x86) Unicode Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir Win32__8
# PROP BASE Intermediate_Dir Win32__8
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir UniDebug
# PROP Intermediate_Dir UniDebug
OUTDIR=.\UniDebug
INTDIR=.\UniDebug

ALL : "$(OUTDIR)\autodriv.exe" "$(OUTDIR)\autodriv.bsc"

CLEAN : 
	-@erase ".\UniDebug\vc40.pdb"
	-@erase ".\UniDebug\autodriv.pch"
	-@erase ".\UniDebug\autodriv.bsc"
	-@erase ".\UniDebug\autoddlg.sbr"
	-@erase ".\UniDebug\autoclik.sbr"
	-@erase ".\UniDebug\stdafx.sbr"
	-@erase ".\UniDebug\autodriv.sbr"
	-@erase ".\UniDebug\autodriv.exe"
	-@erase ".\UniDebug\autodriv.obj"
	-@erase ".\UniDebug\autoddlg.obj"
	-@erase ".\UniDebug\autoclik.obj"
	-@erase ".\UniDebug\stdafx.obj"
	-@erase ".\UniDebug\autodriv.res"
	-@erase ".\UniDebug\autodriv.ilk"
	-@erase ".\UniDebug\autodriv.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MD /W3 /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /Yu"stdafx.h" /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_AFXDLL" /D "_UNICODE" /FR /Yu"stdafx.h" /c
CPP_PROJ=/nologo /MDd /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS"\
 /D "_AFXDLL" /D "_UNICODE" /FR"$(INTDIR)/" /Fp"$(INTDIR)/autodriv.pch"\
 /Yu"stdafx.h" /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\UniDebug/
CPP_SBRS=.\UniDebug/
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/autodriv.res" /d "_DEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# SUBTRACT BASE BSC32 /Iu
# ADD BSC32 /nologo
# SUBTRACT BSC32 /Iu
BSC32_FLAGS=/nologo /o"$(OUTDIR)/autodriv.bsc" 
BSC32_SBRS= \
	"$(INTDIR)/autoddlg.sbr" \
	"$(INTDIR)/autoclik.sbr" \
	"$(INTDIR)/stdafx.sbr" \
	"$(INTDIR)/autodriv.sbr"

"$(OUTDIR)\autodriv.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386
# SUBTRACT BASE LINK32 /pdb:none
# ADD LINK32 /nologo /entry:"wWinMainCRTStartup" /subsystem:windows /debug /machine:I386
# SUBTRACT LINK32 /pdb:none
LINK32_FLAGS=/nologo /entry:"wWinMainCRTStartup" /subsystem:windows\
 /incremental:yes /pdb:"$(OUTDIR)/autodriv.pdb" /debug /machine:I386\
 /out:"$(OUTDIR)/autodriv.exe" 
LINK32_OBJS= \
	"$(INTDIR)/autodriv.obj" \
	"$(INTDIR)/autoddlg.obj" \
	"$(INTDIR)/autoclik.obj" \
	"$(INTDIR)/stdafx.obj" \
	"$(INTDIR)/autodriv.res"

"$(OUTDIR)\autodriv.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "autodriv - Win32 (80x86) Unicode Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir Win32__0
# PROP BASE Intermediate_Dir Win32__0
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir UniRel
# PROP Intermediate_Dir UniRel
OUTDIR=.\UniRel
INTDIR=.\UniRel

ALL : "$(OUTDIR)\autodriv.exe" "$(OUTDIR)\autodriv.bsc"

CLEAN : 
	-@erase ".\UniRel\autodriv.bsc"
	-@erase ".\UniRel\stdafx.sbr"
	-@erase ".\UniRel\autoddlg.sbr"
	-@erase ".\UniRel\autodriv.pch"
	-@erase ".\UniRel\autoclik.sbr"
	-@erase ".\UniRel\autodriv.sbr"
	-@erase ".\UniRel\autodriv.exe"
	-@erase ".\UniRel\autoclik.obj"
	-@erase ".\UniRel\autodriv.obj"
	-@erase ".\UniRel\stdafx.obj"
	-@erase ".\UniRel\autoddlg.obj"
	-@erase ".\UniRel\autodriv.res"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /Yu"stdafx.h" /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_AFXDLL" /D "_UNICODE" /FR /Yu"stdafx.h" /c
CPP_PROJ=/nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_UNICODE" /FR"$(INTDIR)/" /Fp"$(INTDIR)/autodriv.pch"\
 /Yu"stdafx.h" /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\UniRel/
CPP_SBRS=.\UniRel/
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/autodriv.res" /d "NDEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# SUBTRACT BASE BSC32 /Iu
# ADD BSC32 /nologo
# SUBTRACT BSC32 /Iu
BSC32_FLAGS=/nologo /o"$(OUTDIR)/autodriv.bsc" 
BSC32_SBRS= \
	"$(INTDIR)/stdafx.sbr" \
	"$(INTDIR)/autoddlg.sbr" \
	"$(INTDIR)/autoclik.sbr" \
	"$(INTDIR)/autodriv.sbr"

"$(OUTDIR)\autodriv.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# SUBTRACT BASE LINK32 /pdb:none
# ADD LINK32 /nologo /entry:"wWinMainCRTStartup" /subsystem:windows /machine:I386
# SUBTRACT LINK32 /pdb:none
LINK32_FLAGS=/nologo /entry:"wWinMainCRTStartup" /subsystem:windows\
 /incremental:no /pdb:"$(OUTDIR)/autodriv.pdb" /machine:I386\
 /out:"$(OUTDIR)/autodriv.exe" 
LINK32_OBJS= \
	"$(INTDIR)/autoclik.obj" \
	"$(INTDIR)/autodriv.obj" \
	"$(INTDIR)/stdafx.obj" \
	"$(INTDIR)/autoddlg.obj" \
	"$(INTDIR)/autodriv.res"

"$(OUTDIR)\autodriv.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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

MTL_PROJ=
################################################################################
# Begin Target

# Name "autodriv - Win32 (80x86) Debug"
# Name "autodriv - Win32 (80x86) Release"
# Name "autodriv - Win32 (80x86) Unicode Debug"
# Name "autodriv - Win32 (80x86) Unicode Release"

!IF  "$(CFG)" == "autodriv - Win32 (80x86) Debug"

!ELSEIF  "$(CFG)" == "autodriv - Win32 (80x86) Release"

!ELSEIF  "$(CFG)" == "autodriv - Win32 (80x86) Unicode Debug"

!ELSEIF  "$(CFG)" == "autodriv - Win32 (80x86) Unicode Release"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\stdafx.cpp
DEP_CPP_STDAF=\
	".\stdafx.h"\
	

!IF  "$(CFG)" == "autodriv - Win32 (80x86) Debug"

# ADD BASE CPP /Yc"stdafx.h"
# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MDd /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS"\
 /D "_AFXDLL" /D "_MBCS" /FR"$(INTDIR)/" /Fp"$(INTDIR)/autodriv.pch"\
 /Yc"stdafx.h" /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\stdafx.sbr" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\autodriv.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "autodriv - Win32 (80x86) Release"

# ADD BASE CPP /Yc"stdafx.h"
# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /FR"$(INTDIR)/" /Fp"$(INTDIR)/autodriv.pch" /Yc"stdafx.h"\
 /Fo"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\stdafx.sbr" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\autodriv.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "autodriv - Win32 (80x86) Unicode Debug"

# ADD BASE CPP /Yc"stdafx.h"
# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MDd /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS"\
 /D "_AFXDLL" /D "_UNICODE" /FR"$(INTDIR)/" /Fp"$(INTDIR)/autodriv.pch"\
 /Yc"stdafx.h" /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\stdafx.sbr" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\autodriv.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "autodriv - Win32 (80x86) Unicode Release"

# ADD BASE CPP /Yc"stdafx.h"
# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_UNICODE" /FR"$(INTDIR)/" /Fp"$(INTDIR)/autodriv.pch"\
 /Yc"stdafx.h" /Fo"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\stdafx.sbr" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\autodriv.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\autodriv.cpp
DEP_CPP_AUTOD=\
	".\autoclik.h"\
	".\autoddlg.h"\
	".\autodriv.h"\
	".\stdafx.h"\
	

!IF  "$(CFG)" == "autodriv - Win32 (80x86) Debug"


"$(INTDIR)\autodriv.obj" : $(SOURCE) $(DEP_CPP_AUTOD) "$(INTDIR)"\
 "$(INTDIR)\autodriv.pch"

"$(INTDIR)\autodriv.sbr" : $(SOURCE) $(DEP_CPP_AUTOD) "$(INTDIR)"\
 "$(INTDIR)\autodriv.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_AUTOD) "$(INTDIR)"\
 "$(INTDIR)\autodriv.pch"


!ELSEIF  "$(CFG)" == "autodriv - Win32 (80x86) Release"


"$(INTDIR)\autodriv.obj" : $(SOURCE) $(DEP_CPP_AUTOD) "$(INTDIR)"\
 "$(INTDIR)\autodriv.pch"

"$(INTDIR)\autodriv.sbr" : $(SOURCE) $(DEP_CPP_AUTOD) "$(INTDIR)"\
 "$(INTDIR)\autodriv.pch"


!ELSEIF  "$(CFG)" == "autodriv - Win32 (80x86) Unicode Debug"


"$(INTDIR)\autodriv.obj" : $(SOURCE) $(DEP_CPP_AUTOD) "$(INTDIR)"\
 "$(INTDIR)\autodriv.pch"

"$(INTDIR)\autodriv.sbr" : $(SOURCE) $(DEP_CPP_AUTOD) "$(INTDIR)"\
 "$(INTDIR)\autodriv.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_AUTOD) "$(INTDIR)"\
 "$(INTDIR)\autodriv.pch"


!ELSEIF  "$(CFG)" == "autodriv - Win32 (80x86) Unicode Release"


"$(INTDIR)\autodriv.obj" : $(SOURCE) $(DEP_CPP_AUTOD) "$(INTDIR)"\
 "$(INTDIR)\autodriv.pch"

"$(INTDIR)\autodriv.sbr" : $(SOURCE) $(DEP_CPP_AUTOD) "$(INTDIR)"\
 "$(INTDIR)\autodriv.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\autoddlg.cpp
DEP_CPP_AUTODD=\
	".\autoclik.h"\
	".\autoddlg.h"\
	".\autodriv.h"\
	".\stdafx.h"\
	

!IF  "$(CFG)" == "autodriv - Win32 (80x86) Debug"


"$(INTDIR)\autoddlg.obj" : $(SOURCE) $(DEP_CPP_AUTODD) "$(INTDIR)"\
 "$(INTDIR)\autodriv.pch"

"$(INTDIR)\autoddlg.sbr" : $(SOURCE) $(DEP_CPP_AUTODD) "$(INTDIR)"\
 "$(INTDIR)\autodriv.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_AUTODD) "$(INTDIR)"\
 "$(INTDIR)\autodriv.pch"


!ELSEIF  "$(CFG)" == "autodriv - Win32 (80x86) Release"


"$(INTDIR)\autoddlg.obj" : $(SOURCE) $(DEP_CPP_AUTODD) "$(INTDIR)"\
 "$(INTDIR)\autodriv.pch"

"$(INTDIR)\autoddlg.sbr" : $(SOURCE) $(DEP_CPP_AUTODD) "$(INTDIR)"\
 "$(INTDIR)\autodriv.pch"


!ELSEIF  "$(CFG)" == "autodriv - Win32 (80x86) Unicode Debug"


"$(INTDIR)\autoddlg.obj" : $(SOURCE) $(DEP_CPP_AUTODD) "$(INTDIR)"\
 "$(INTDIR)\autodriv.pch"

"$(INTDIR)\autoddlg.sbr" : $(SOURCE) $(DEP_CPP_AUTODD) "$(INTDIR)"\
 "$(INTDIR)\autodriv.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_AUTODD) "$(INTDIR)"\
 "$(INTDIR)\autodriv.pch"


!ELSEIF  "$(CFG)" == "autodriv - Win32 (80x86) Unicode Release"


"$(INTDIR)\autoddlg.obj" : $(SOURCE) $(DEP_CPP_AUTODD) "$(INTDIR)"\
 "$(INTDIR)\autodriv.pch"

"$(INTDIR)\autoddlg.sbr" : $(SOURCE) $(DEP_CPP_AUTODD) "$(INTDIR)"\
 "$(INTDIR)\autodriv.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\autodriv.rc
DEP_RSC_AUTODR=\
	".\res\autodriv.ico"\
	".\res\autodriv.rc2"\
	

!IF  "$(CFG)" == "autodriv - Win32 (80x86) Debug"


"$(INTDIR)\autodriv.res" : $(SOURCE) $(DEP_RSC_AUTODR) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "autodriv - Win32 (80x86) Release"


"$(INTDIR)\autodriv.res" : $(SOURCE) $(DEP_RSC_AUTODR) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "autodriv - Win32 (80x86) Unicode Debug"


"$(INTDIR)\autodriv.res" : $(SOURCE) $(DEP_RSC_AUTODR) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "autodriv - Win32 (80x86) Unicode Release"


"$(INTDIR)\autodriv.res" : $(SOURCE) $(DEP_RSC_AUTODR) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\autoclik.cpp
DEP_CPP_AUTOC=\
	".\autoclik.h"\
	".\stdafx.h"\
	

!IF  "$(CFG)" == "autodriv - Win32 (80x86) Debug"


"$(INTDIR)\autoclik.obj" : $(SOURCE) $(DEP_CPP_AUTOC) "$(INTDIR)"\
 "$(INTDIR)\autodriv.pch"

"$(INTDIR)\autoclik.sbr" : $(SOURCE) $(DEP_CPP_AUTOC) "$(INTDIR)"\
 "$(INTDIR)\autodriv.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_AUTOC) "$(INTDIR)"\
 "$(INTDIR)\autodriv.pch"


!ELSEIF  "$(CFG)" == "autodriv - Win32 (80x86) Release"


"$(INTDIR)\autoclik.obj" : $(SOURCE) $(DEP_CPP_AUTOC) "$(INTDIR)"\
 "$(INTDIR)\autodriv.pch"

"$(INTDIR)\autoclik.sbr" : $(SOURCE) $(DEP_CPP_AUTOC) "$(INTDIR)"\
 "$(INTDIR)\autodriv.pch"


!ELSEIF  "$(CFG)" == "autodriv - Win32 (80x86) Unicode Debug"


"$(INTDIR)\autoclik.obj" : $(SOURCE) $(DEP_CPP_AUTOC) "$(INTDIR)"\
 "$(INTDIR)\autodriv.pch"

"$(INTDIR)\autoclik.sbr" : $(SOURCE) $(DEP_CPP_AUTOC) "$(INTDIR)"\
 "$(INTDIR)\autodriv.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_AUTOC) "$(INTDIR)"\
 "$(INTDIR)\autodriv.pch"


!ELSEIF  "$(CFG)" == "autodriv - Win32 (80x86) Unicode Release"


"$(INTDIR)\autoclik.obj" : $(SOURCE) $(DEP_CPP_AUTOC) "$(INTDIR)"\
 "$(INTDIR)\autodriv.pch"

"$(INTDIR)\autoclik.sbr" : $(SOURCE) $(DEP_CPP_AUTOC) "$(INTDIR)"\
 "$(INTDIR)\autodriv.pch"


!ENDIF 

# End Source File
# End Target
# End Project
################################################################################
