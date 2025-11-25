# Microsoft Developer Studio Generated NMAKE File, Format Version 42001
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

!IF "$(CFG)" == ""
CFG=vcterm - Win32 Unicode Debug
!MESSAGE No configuration specified.  Defaulting to vcterm - Win32 Unicode\
 Debug.
!ENDIF 

!IF "$(CFG)" != "vcterm - Win32 Release" && "$(CFG)" != "vcterm - Win32 Debug"\
 && "$(CFG)" != "vcterm - Win32 Unicode Debug" && "$(CFG)" !=\
 "vcterm - Win32 Unicode Release"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "Vcterm.mak" CFG="vcterm - Win32 Unicode Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "vcterm - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "vcterm - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE "vcterm - Win32 Unicode Debug" (based on "Win32 (x86) Application")
!MESSAGE "vcterm - Win32 Unicode Release" (based on "Win32 (x86) Application")
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
# PROP Target_Last_Scanned "vcterm - Win32 Debug"
RSC=rc.exe
MTL=mktyplib.exe
CPP=cl.exe

!IF  "$(CFG)" == "vcterm - Win32 Release"

# PROP BASE Use_MFC 6
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

ALL : "$(OUTDIR)\Vcterm.exe"

CLEAN : 
	-@erase "$(INTDIR)\CanDlg.obj"
	-@erase "$(INTDIR)\commctrl.obj"
	-@erase "$(INTDIR)\InputDlg.obj"
	-@erase "$(INTDIR)\mainfrm.obj"
	-@erase "$(INTDIR)\setdlg.obj"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\termedit.obj"
	-@erase "$(INTDIR)\vcterm.obj"
	-@erase "$(INTDIR)\Vcterm.pch"
	-@erase "$(INTDIR)\vcterm.res"
	-@erase "$(OUTDIR)\Vcterm.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
CPP_PROJ=/nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/Vcterm.pch" /Yu"stdafx.h" /Fo"$(INTDIR)/" /c\
 
CPP_OBJS=.\Release/
CPP_SBRS=.\.
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/vcterm.res" /d "NDEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/Vcterm.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 /nologo /subsystem:windows /machine:I386
LINK32_FLAGS=/nologo /subsystem:windows /incremental:no\
 /pdb:"$(OUTDIR)/Vcterm.pdb" /machine:I386 /out:"$(OUTDIR)/Vcterm.exe" 
LINK32_OBJS= \
	"$(INTDIR)\CanDlg.obj" \
	"$(INTDIR)\commctrl.obj" \
	"$(INTDIR)\InputDlg.obj" \
	"$(INTDIR)\mainfrm.obj" \
	"$(INTDIR)\setdlg.obj" \
	"$(INTDIR)\StdAfx.obj" \
	"$(INTDIR)\termedit.obj" \
	"$(INTDIR)\vcterm.obj" \
	"$(INTDIR)\vcterm.res"

"$(OUTDIR)\Vcterm.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "vcterm - Win32 Debug"

# PROP BASE Use_MFC 6
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

ALL : "$(OUTDIR)\Vcterm.exe"

CLEAN : 
	-@erase "$(INTDIR)\CanDlg.obj"
	-@erase "$(INTDIR)\commctrl.obj"
	-@erase "$(INTDIR)\InputDlg.obj"
	-@erase "$(INTDIR)\mainfrm.obj"
	-@erase "$(INTDIR)\setdlg.obj"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\termedit.obj"
	-@erase "$(INTDIR)\vc40.idb"
	-@erase "$(INTDIR)\vc40.pdb"
	-@erase "$(INTDIR)\vcterm.obj"
	-@erase "$(INTDIR)\Vcterm.pch"
	-@erase "$(INTDIR)\vcterm.res"
	-@erase "$(OUTDIR)\Vcterm.exe"
	-@erase "$(OUTDIR)\Vcterm.ilk"
	-@erase "$(OUTDIR)\Vcterm.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
CPP_PROJ=/nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/Vcterm.pch" /Yu"stdafx.h" /Fo"$(INTDIR)/"\
 /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\Debug/
CPP_SBRS=.\.
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/vcterm.res" /d "_DEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/Vcterm.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386
# ADD LINK32 /nologo /subsystem:windows /debug /machine:I386
LINK32_FLAGS=/nologo /subsystem:windows /incremental:yes\
 /pdb:"$(OUTDIR)/Vcterm.pdb" /debug /machine:I386 /out:"$(OUTDIR)/Vcterm.exe" 
LINK32_OBJS= \
	"$(INTDIR)\CanDlg.obj" \
	"$(INTDIR)\commctrl.obj" \
	"$(INTDIR)\InputDlg.obj" \
	"$(INTDIR)\mainfrm.obj" \
	"$(INTDIR)\setdlg.obj" \
	"$(INTDIR)\StdAfx.obj" \
	"$(INTDIR)\termedit.obj" \
	"$(INTDIR)\vcterm.obj" \
	"$(INTDIR)\vcterm.res"

"$(OUTDIR)\Vcterm.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "vcterm - Win32 Unicode Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "vcterm__"
# PROP BASE Intermediate_Dir "vcterm__"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "DebugU"
# PROP Intermediate_Dir "DebugU"
# PROP Target_Dir ""
OUTDIR=.\DebugU
INTDIR=.\DebugU

ALL : "$(OUTDIR)\Vcterm.exe"

CLEAN : 
	-@erase "$(INTDIR)\CanDlg.obj"
	-@erase "$(INTDIR)\commctrl.obj"
	-@erase "$(INTDIR)\InputDlg.obj"
	-@erase "$(INTDIR)\mainfrm.obj"
	-@erase "$(INTDIR)\setdlg.obj"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\termedit.obj"
	-@erase "$(INTDIR)\vc40.idb"
	-@erase "$(INTDIR)\vc40.pdb"
	-@erase "$(INTDIR)\vcterm.obj"
	-@erase "$(INTDIR)\Vcterm.pch"
	-@erase "$(INTDIR)\vcterm.res"
	-@erase "$(OUTDIR)\Vcterm.exe"
	-@erase "$(OUTDIR)\Vcterm.ilk"
	-@erase "$(OUTDIR)\Vcterm.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_UNICODE" /Yu"stdafx.h" /c
CPP_PROJ=/nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /D "_AFXDLL" /D "_UNICODE" /Fp"$(INTDIR)/Vcterm.pch" /Yu"stdafx.h"\
 /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\DebugU/
CPP_SBRS=.\.
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/vcterm.res" /d "_DEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/Vcterm.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386
# ADD LINK32 /nologo /entry:"wWinMainCRTStartup" /subsystem:windows /debug /machine:I386
LINK32_FLAGS=/nologo /entry:"wWinMainCRTStartup" /subsystem:windows\
 /incremental:yes /pdb:"$(OUTDIR)/Vcterm.pdb" /debug /machine:I386\
 /out:"$(OUTDIR)/Vcterm.exe" 
LINK32_OBJS= \
	"$(INTDIR)\CanDlg.obj" \
	"$(INTDIR)\commctrl.obj" \
	"$(INTDIR)\InputDlg.obj" \
	"$(INTDIR)\mainfrm.obj" \
	"$(INTDIR)\setdlg.obj" \
	"$(INTDIR)\StdAfx.obj" \
	"$(INTDIR)\termedit.obj" \
	"$(INTDIR)\vcterm.obj" \
	"$(INTDIR)\vcterm.res"

"$(OUTDIR)\Vcterm.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "vcterm - Win32 Unicode Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "vcterm_0"
# PROP BASE Intermediate_Dir "vcterm_0"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "ReleaseU"
# PROP Intermediate_Dir "ReleaseU"
# PROP Target_Dir ""
OUTDIR=.\ReleaseU
INTDIR=.\ReleaseU

ALL : "$(OUTDIR)\Vcterm.exe"

CLEAN : 
	-@erase "$(INTDIR)\CanDlg.obj"
	-@erase "$(INTDIR)\commctrl.obj"
	-@erase "$(INTDIR)\InputDlg.obj"
	-@erase "$(INTDIR)\mainfrm.obj"
	-@erase "$(INTDIR)\setdlg.obj"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\termedit.obj"
	-@erase "$(INTDIR)\vcterm.obj"
	-@erase "$(INTDIR)\Vcterm.pch"
	-@erase "$(INTDIR)\vcterm.res"
	-@erase "$(OUTDIR)\Vcterm.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_UNICODE" /Yu"stdafx.h" /c
CPP_PROJ=/nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_UNICODE" /Fp"$(INTDIR)/Vcterm.pch" /Yu"stdafx.h" /Fo"$(INTDIR)/"\
 /c 
CPP_OBJS=.\ReleaseU/
CPP_SBRS=.\.
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/vcterm.res" /d "NDEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/Vcterm.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 /nologo /entry:"wWinMainCRTStartup" /subsystem:windows /machine:I386
LINK32_FLAGS=/nologo /entry:"wWinMainCRTStartup" /subsystem:windows\
 /incremental:no /pdb:"$(OUTDIR)/Vcterm.pdb" /machine:I386\
 /out:"$(OUTDIR)/Vcterm.exe" 
LINK32_OBJS= \
	"$(INTDIR)\CanDlg.obj" \
	"$(INTDIR)\commctrl.obj" \
	"$(INTDIR)\InputDlg.obj" \
	"$(INTDIR)\mainfrm.obj" \
	"$(INTDIR)\setdlg.obj" \
	"$(INTDIR)\StdAfx.obj" \
	"$(INTDIR)\termedit.obj" \
	"$(INTDIR)\vcterm.obj" \
	"$(INTDIR)\vcterm.res"

"$(OUTDIR)\Vcterm.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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

# Name "vcterm - Win32 Release"
# Name "vcterm - Win32 Debug"
# Name "vcterm - Win32 Unicode Debug"
# Name "vcterm - Win32 Unicode Release"

!IF  "$(CFG)" == "vcterm - Win32 Release"

!ELSEIF  "$(CFG)" == "vcterm - Win32 Debug"

!ELSEIF  "$(CFG)" == "vcterm - Win32 Unicode Debug"

!ELSEIF  "$(CFG)" == "vcterm - Win32 Unicode Release"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\vcterm.cpp
DEP_CPP_VCTER=\
	".\Commctrl.h"\
	".\Mainfrm.h"\
	".\StdAfx.h"\
	".\Termedit.h"\
	".\Vcterm.h"\
	

!IF  "$(CFG)" == "vcterm - Win32 Release"


"$(INTDIR)\vcterm.obj" : $(SOURCE) $(DEP_CPP_VCTER) "$(INTDIR)"\
 "$(INTDIR)\Vcterm.pch"


!ELSEIF  "$(CFG)" == "vcterm - Win32 Debug"


"$(INTDIR)\vcterm.obj" : $(SOURCE) $(DEP_CPP_VCTER) "$(INTDIR)"\
 "$(INTDIR)\Vcterm.pch"


!ELSEIF  "$(CFG)" == "vcterm - Win32 Unicode Debug"


"$(INTDIR)\vcterm.obj" : $(SOURCE) $(DEP_CPP_VCTER) "$(INTDIR)"\
 "$(INTDIR)\Vcterm.pch"


!ELSEIF  "$(CFG)" == "vcterm - Win32 Unicode Release"


"$(INTDIR)\vcterm.obj" : $(SOURCE) $(DEP_CPP_VCTER) "$(INTDIR)"\
 "$(INTDIR)\Vcterm.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\StdAfx.cpp
DEP_CPP_STDAF=\
	".\StdAfx.h"\
	

!IF  "$(CFG)" == "vcterm - Win32 Release"

# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/Vcterm.pch" /Yc"stdafx.h" /Fo"$(INTDIR)/" /c\
 $(SOURCE) \
	

"$(INTDIR)\StdAfx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\Vcterm.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "vcterm - Win32 Debug"

# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/Vcterm.pch" /Yc"stdafx.h" /Fo"$(INTDIR)/"\
 /Fd"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\StdAfx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\Vcterm.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "vcterm - Win32 Unicode Debug"

# ADD BASE CPP /Yc"stdafx.h"
# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /D "_AFXDLL" /D "_UNICODE" /Fp"$(INTDIR)/Vcterm.pch" /Yc"stdafx.h"\
 /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\StdAfx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\Vcterm.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "vcterm - Win32 Unicode Release"

# ADD BASE CPP /Yc"stdafx.h"
# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_UNICODE" /Fp"$(INTDIR)/Vcterm.pch" /Yc"stdafx.h" /Fo"$(INTDIR)/"\
 /c $(SOURCE) \
	

"$(INTDIR)\StdAfx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\Vcterm.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\mainfrm.cpp
DEP_CPP_MAINF=\
	".\CanDlg.h"\
	".\Commctrl.h"\
	".\InputDlg.h"\
	".\Mainfrm.h"\
	".\Setdlg.h"\
	".\StdAfx.h"\
	".\Termedit.h"\
	".\Vcterm.h"\
	

!IF  "$(CFG)" == "vcterm - Win32 Release"


"$(INTDIR)\mainfrm.obj" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\Vcterm.pch"


!ELSEIF  "$(CFG)" == "vcterm - Win32 Debug"


"$(INTDIR)\mainfrm.obj" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\Vcterm.pch"


!ELSEIF  "$(CFG)" == "vcterm - Win32 Unicode Debug"


"$(INTDIR)\mainfrm.obj" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\Vcterm.pch"


!ELSEIF  "$(CFG)" == "vcterm - Win32 Unicode Release"


"$(INTDIR)\mainfrm.obj" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\Vcterm.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\vcterm.rc
DEP_RSC_VCTERM=\
	".\res\Toolbar.bmp"\
	".\res\vcterm.ico"\
	".\res\vcterm.rc2"\
	

!IF  "$(CFG)" == "vcterm - Win32 Release"


"$(INTDIR)\vcterm.res" : $(SOURCE) $(DEP_RSC_VCTERM) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "vcterm - Win32 Debug"


"$(INTDIR)\vcterm.res" : $(SOURCE) $(DEP_RSC_VCTERM) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "vcterm - Win32 Unicode Debug"


"$(INTDIR)\vcterm.res" : $(SOURCE) $(DEP_RSC_VCTERM) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "vcterm - Win32 Unicode Release"


"$(INTDIR)\vcterm.res" : $(SOURCE) $(DEP_RSC_VCTERM) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\commctrl.cpp
DEP_CPP_COMMC=\
	".\Commctrl.h"\
	".\StdAfx.h"\
	

!IF  "$(CFG)" == "vcterm - Win32 Release"


"$(INTDIR)\commctrl.obj" : $(SOURCE) $(DEP_CPP_COMMC) "$(INTDIR)"\
 "$(INTDIR)\Vcterm.pch"


!ELSEIF  "$(CFG)" == "vcterm - Win32 Debug"


"$(INTDIR)\commctrl.obj" : $(SOURCE) $(DEP_CPP_COMMC) "$(INTDIR)"\
 "$(INTDIR)\Vcterm.pch"


!ELSEIF  "$(CFG)" == "vcterm - Win32 Unicode Debug"


"$(INTDIR)\commctrl.obj" : $(SOURCE) $(DEP_CPP_COMMC) "$(INTDIR)"\
 "$(INTDIR)\Vcterm.pch"


!ELSEIF  "$(CFG)" == "vcterm - Win32 Unicode Release"


"$(INTDIR)\commctrl.obj" : $(SOURCE) $(DEP_CPP_COMMC) "$(INTDIR)"\
 "$(INTDIR)\Vcterm.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\termedit.cpp
DEP_CPP_TERME=\
	".\Commctrl.h"\
	".\Mainfrm.h"\
	".\StdAfx.h"\
	".\Termedit.h"\
	".\Vcterm.h"\
	

!IF  "$(CFG)" == "vcterm - Win32 Release"


"$(INTDIR)\termedit.obj" : $(SOURCE) $(DEP_CPP_TERME) "$(INTDIR)"\
 "$(INTDIR)\Vcterm.pch"


!ELSEIF  "$(CFG)" == "vcterm - Win32 Debug"


"$(INTDIR)\termedit.obj" : $(SOURCE) $(DEP_CPP_TERME) "$(INTDIR)"\
 "$(INTDIR)\Vcterm.pch"


!ELSEIF  "$(CFG)" == "vcterm - Win32 Unicode Debug"


"$(INTDIR)\termedit.obj" : $(SOURCE) $(DEP_CPP_TERME) "$(INTDIR)"\
 "$(INTDIR)\Vcterm.pch"


!ELSEIF  "$(CFG)" == "vcterm - Win32 Unicode Release"


"$(INTDIR)\termedit.obj" : $(SOURCE) $(DEP_CPP_TERME) "$(INTDIR)"\
 "$(INTDIR)\Vcterm.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\setdlg.cpp
DEP_CPP_SETDL=\
	".\Commctrl.h"\
	".\Mainfrm.h"\
	".\Setdlg.h"\
	".\StdAfx.h"\
	".\Termedit.h"\
	".\Vcterm.h"\
	

!IF  "$(CFG)" == "vcterm - Win32 Release"


"$(INTDIR)\setdlg.obj" : $(SOURCE) $(DEP_CPP_SETDL) "$(INTDIR)"\
 "$(INTDIR)\Vcterm.pch"


!ELSEIF  "$(CFG)" == "vcterm - Win32 Debug"


"$(INTDIR)\setdlg.obj" : $(SOURCE) $(DEP_CPP_SETDL) "$(INTDIR)"\
 "$(INTDIR)\Vcterm.pch"


!ELSEIF  "$(CFG)" == "vcterm - Win32 Unicode Debug"


"$(INTDIR)\setdlg.obj" : $(SOURCE) $(DEP_CPP_SETDL) "$(INTDIR)"\
 "$(INTDIR)\Vcterm.pch"


!ELSEIF  "$(CFG)" == "vcterm - Win32 Unicode Release"


"$(INTDIR)\setdlg.obj" : $(SOURCE) $(DEP_CPP_SETDL) "$(INTDIR)"\
 "$(INTDIR)\Vcterm.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\InputDlg.cpp
DEP_CPP_INPUT=\
	".\InputDlg.h"\
	".\StdAfx.h"\
	".\Vcterm.h"\
	

!IF  "$(CFG)" == "vcterm - Win32 Release"


"$(INTDIR)\InputDlg.obj" : $(SOURCE) $(DEP_CPP_INPUT) "$(INTDIR)"\
 "$(INTDIR)\Vcterm.pch"


!ELSEIF  "$(CFG)" == "vcterm - Win32 Debug"


"$(INTDIR)\InputDlg.obj" : $(SOURCE) $(DEP_CPP_INPUT) "$(INTDIR)"\
 "$(INTDIR)\Vcterm.pch"


!ELSEIF  "$(CFG)" == "vcterm - Win32 Unicode Debug"


"$(INTDIR)\InputDlg.obj" : $(SOURCE) $(DEP_CPP_INPUT) "$(INTDIR)"\
 "$(INTDIR)\Vcterm.pch"


!ELSEIF  "$(CFG)" == "vcterm - Win32 Unicode Release"


"$(INTDIR)\InputDlg.obj" : $(SOURCE) $(DEP_CPP_INPUT) "$(INTDIR)"\
 "$(INTDIR)\Vcterm.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\CanDlg.cpp
DEP_CPP_CANDL=\
	".\CanDlg.h"\
	".\Commctrl.h"\
	".\Mainfrm.h"\
	".\StdAfx.h"\
	".\Termedit.h"\
	".\Vcterm.h"\
	

!IF  "$(CFG)" == "vcterm - Win32 Release"


"$(INTDIR)\CanDlg.obj" : $(SOURCE) $(DEP_CPP_CANDL) "$(INTDIR)"\
 "$(INTDIR)\Vcterm.pch"


!ELSEIF  "$(CFG)" == "vcterm - Win32 Debug"


"$(INTDIR)\CanDlg.obj" : $(SOURCE) $(DEP_CPP_CANDL) "$(INTDIR)"\
 "$(INTDIR)\Vcterm.pch"


!ELSEIF  "$(CFG)" == "vcterm - Win32 Unicode Debug"


"$(INTDIR)\CanDlg.obj" : $(SOURCE) $(DEP_CPP_CANDL) "$(INTDIR)"\
 "$(INTDIR)\Vcterm.pch"


!ELSEIF  "$(CFG)" == "vcterm - Win32 Unicode Release"


"$(INTDIR)\CanDlg.obj" : $(SOURCE) $(DEP_CPP_CANDL) "$(INTDIR)"\
 "$(INTDIR)\Vcterm.pch"


!ENDIF 

# End Source File
# End Target
# End Project
################################################################################
################################################################################
# Section OLE Controls
# 	{648A5600-2C6E-101B-82B6-000000000014}
# End Section
################################################################################
################################################################################
# Section vcterm : {648A5601-2C6E-101B-82B6-000000000014}
# 	2:5:Class:CCommCtrl
# 	2:10:HeaderFile:commctrl.h
# 	2:8:ImplFile:commctrl.cpp
# End Section
################################################################################
################################################################################
# Section vcterm : {648A5600-2C6E-101B-82B6-000000000014}
# 	0:15:CMSCommCtrl.cpp:D:\MSDEV\Projects\vcterm\CMSCommCtrl.cpp
# 	0:13:CMSCommCtrl.h:D:\MSDEV\Projects\vcterm\CMSCommCtrl.h
# 	2:21:DefaultSinkHeaderFile:commctrl.h
# 	2:16:DefaultSinkClass:CCommCtrl
# End Section
################################################################################
