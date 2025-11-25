# Microsoft Visual C++ Generated NMAKE File, Format Version 40000
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

!IF "$(CFG)" == ""
CFG=IpDrive - Win32 Debug
!MESSAGE No configuration specified.  Defaulting to IpDrive - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "IpDrive - Win32 Release" && "$(CFG)" !=\
 "IpDrive - Win32 Debug" && "$(CFG)" != "IpDrive - Win32 Unicode Release" &&\
 "$(CFG)" != "IpDrive - Win32 Unicode Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "IpDrive.mak" CFG="IpDrive - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "IpDrive - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "IpDrive - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE "IpDrive - Win32 Unicode Release" (based on "Win32 (x86) Application")
!MESSAGE "IpDrive - Win32 Unicode Debug" (based on "Win32 (x86) Application")
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
# PROP Target_Last_Scanned "IpDrive - Win32 Debug"
MTL=mktyplib.exe
RSC=rc.exe
CPP=cl.exe

!IF  "$(CFG)" == "IpDrive - Win32 Release"

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

ALL : "$(OUTDIR)\IpDrive.exe" "$(OUTDIR)\IpDrive.bsc"

CLEAN : 
	-@erase ".\Release\IpDrive.idb"
	-@erase ".\Release\IpDrive.pch"
	-@erase ".\Release\IpDrive.bsc"
	-@erase ".\Release\Stdafx.sbr"
	-@erase ".\Release\Ipdridoc.sbr"
	-@erase ".\Release\Ipdrive.sbr"
	-@erase ".\Release\Ipdrivw.sbr"
	-@erase ".\Release\Ipremote.sbr"
	-@erase ".\Release\Mainfrm.sbr"
	-@erase ".\Release\IpDrive.exe"
	-@erase ".\Release\Mainfrm.obj"
	-@erase ".\Release\Stdafx.obj"
	-@erase ".\Release\Ipdridoc.obj"
	-@erase ".\Release\Ipdrive.obj"
	-@erase ".\Release\Ipdrivw.obj"
	-@erase ".\Release\Ipremote.obj"
	-@erase ".\Release\Ipdrive.res"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /Yu"Stdafx.h" /c
CPP_PROJ=/nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /FR"$(INTDIR)/" /Fp"$(INTDIR)/IpDrive.pch" /Yu"Stdafx.h"\
 /Fo"$(INTDIR)/" /Fd"$(OUTDIR)/IpDrive.pdb" /c 
CPP_OBJS=.\Release/
CPP_SBRS=.\Release/
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/Ipdrive.res" /d "NDEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/IpDrive.bsc" 
BSC32_SBRS= \
	"$(INTDIR)/Stdafx.sbr" \
	"$(INTDIR)/Ipdridoc.sbr" \
	"$(INTDIR)/Ipdrive.sbr" \
	"$(INTDIR)/Ipdrivw.sbr" \
	"$(INTDIR)/Ipremote.sbr" \
	"$(INTDIR)/Mainfrm.sbr"

"$(OUTDIR)\IpDrive.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 /nologo /subsystem:windows /machine:I386
LINK32_FLAGS=/nologo /subsystem:windows /incremental:no\
 /pdb:"$(OUTDIR)/IpDrive.pdb" /machine:I386 /out:"$(OUTDIR)/IpDrive.exe" 
LINK32_OBJS= \
	"$(INTDIR)/Mainfrm.obj" \
	"$(INTDIR)/Stdafx.obj" \
	"$(INTDIR)/Ipdridoc.obj" \
	"$(INTDIR)/Ipdrive.obj" \
	"$(INTDIR)/Ipdrivw.obj" \
	"$(INTDIR)/Ipremote.obj" \
	"$(INTDIR)/Ipdrive.res"

"$(OUTDIR)\IpDrive.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "IpDrive - Win32 Debug"

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

ALL : "$(OUTDIR)\IpDrive.exe" "$(OUTDIR)\IpDrive.bsc"

CLEAN : 
	-@erase ".\Debug\IpDrive.pdb"
	-@erase ".\Debug\IpDrive.pch"
	-@erase ".\Debug\Mainfrm.obj"
	-@erase ".\Debug\Ipremote.obj"
	-@erase ".\Debug\Stdafx.obj"
	-@erase ".\Debug\Ipdrive.obj"
	-@erase ".\Debug\Ipdrivw.obj"
	-@erase ".\Debug\Ipdridoc.obj"
	-@erase ".\Debug\Ipdrive.res"
	-@erase ".\Debug\IpDrive.idb"
	-@erase ".\Debug\IpDrive.bsc"
	-@erase ".\Debug\Ipremote.sbr"
	-@erase ".\Debug\Stdafx.sbr"
	-@erase ".\Debug\Ipdrive.sbr"
	-@erase ".\Debug\Ipdrivw.sbr"
	-@erase ".\Debug\Ipdridoc.sbr"
	-@erase ".\Debug\Mainfrm.sbr"
	-@erase ".\Debug\IpDrive.exe"
	-@erase ".\Debug\IpDrive.ilk"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /Yu"Stdafx.h" /c
CPP_PROJ=/nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /D "_AFXDLL" /D "_MBCS" /FR"$(INTDIR)/" /Fp"$(INTDIR)/IpDrive.pch"\
 /Yu"Stdafx.h" /Fo"$(INTDIR)/" /Fd"$(OUTDIR)/IpDrive.pdb" /c 
CPP_OBJS=.\Debug/
CPP_SBRS=.\Debug/
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/Ipdrive.res" /d "_DEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/IpDrive.bsc" 
BSC32_SBRS= \
	"$(INTDIR)/Ipremote.sbr" \
	"$(INTDIR)/Stdafx.sbr" \
	"$(INTDIR)/Ipdrive.sbr" \
	"$(INTDIR)/Ipdrivw.sbr" \
	"$(INTDIR)/Ipdridoc.sbr" \
	"$(INTDIR)/Mainfrm.sbr"

"$(OUTDIR)\IpDrive.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386
# ADD LINK32 /nologo /subsystem:windows /debug /machine:I386
LINK32_FLAGS=/nologo /subsystem:windows /incremental:yes\
 /pdb:"$(OUTDIR)/IpDrive.pdb" /debug /machine:I386 /out:"$(OUTDIR)/IpDrive.exe" 
LINK32_OBJS= \
	"$(INTDIR)/Mainfrm.obj" \
	"$(INTDIR)/Ipremote.obj" \
	"$(INTDIR)/Stdafx.obj" \
	"$(INTDIR)/Ipdrive.obj" \
	"$(INTDIR)/Ipdrivw.obj" \
	"$(INTDIR)/Ipdridoc.obj" \
	"$(INTDIR)/Ipdrive.res"

"$(OUTDIR)\IpDrive.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "IpDrive - Win32 Unicode Release"

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

ALL : "$(OUTDIR)\IpDrive.exe" "$(OUTDIR)\IpDrive.bsc"

CLEAN : 
	-@erase ".\UniRelease\IpDrive.idb"
	-@erase ".\UniRelease\IpDrive.pch"
	-@erase ".\UniRelease\IpDrive.bsc"
	-@erase ".\UniRelease\Mainfrm.sbr"
	-@erase ".\UniRelease\Stdafx.sbr"
	-@erase ".\UniRelease\Ipremote.sbr"
	-@erase ".\UniRelease\Ipdrive.sbr"
	-@erase ".\UniRelease\Ipdrivw.sbr"
	-@erase ".\UniRelease\Ipdridoc.sbr"
	-@erase ".\UniRelease\IpDrive.exe"
	-@erase ".\UniRelease\Ipdridoc.obj"
	-@erase ".\UniRelease\Mainfrm.obj"
	-@erase ".\UniRelease\Stdafx.obj"
	-@erase ".\UniRelease\Ipremote.obj"
	-@erase ".\UniRelease\Ipdrive.obj"
	-@erase ".\UniRelease\Ipdrivw.obj"
	-@erase ".\UniRelease\Ipdrive.res"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_UNICODE" /D "_AFXDLL" /D "_MBCS" /FR /Yu"Stdafx.h" /c
CPP_PROJ=/nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D\
 "_UNICODE" /D "_AFXDLL" /D "_MBCS" /FR"$(INTDIR)/" /Fp"$(INTDIR)/IpDrive.pch"\
 /Yu"Stdafx.h" /Fo"$(INTDIR)/" /Fd"$(OUTDIR)/IpDrive.pdb" /c 
CPP_OBJS=.\UniRelease/
CPP_SBRS=.\UniRelease/
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/Ipdrive.res" /d "NDEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/IpDrive.bsc" 
BSC32_SBRS= \
	"$(INTDIR)/Mainfrm.sbr" \
	"$(INTDIR)/Stdafx.sbr" \
	"$(INTDIR)/Ipremote.sbr" \
	"$(INTDIR)/Ipdrive.sbr" \
	"$(INTDIR)/Ipdrivw.sbr" \
	"$(INTDIR)/Ipdridoc.sbr"

"$(OUTDIR)\IpDrive.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 /nologo /entry:"wWinMainCRTStartup" /subsystem:windows /machine:I386
LINK32_FLAGS=/nologo /entry:"wWinMainCRTStartup" /subsystem:windows\
 /incremental:no /pdb:"$(OUTDIR)/IpDrive.pdb" /machine:I386\
 /out:"$(OUTDIR)/IpDrive.exe" 
LINK32_OBJS= \
	"$(INTDIR)/Ipdridoc.obj" \
	"$(INTDIR)/Mainfrm.obj" \
	"$(INTDIR)/Stdafx.obj" \
	"$(INTDIR)/Ipremote.obj" \
	"$(INTDIR)/Ipdrive.obj" \
	"$(INTDIR)/Ipdrivw.obj" \
	"$(INTDIR)/Ipdrive.res"

"$(OUTDIR)\IpDrive.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "IpDrive - Win32 Unicode Debug"

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

ALL : "$(OUTDIR)\IpDrive.exe" "$(OUTDIR)\IpDrive.bsc"

CLEAN : 
	-@erase ".\UniDebug\IpDrive.pdb"
	-@erase ".\UniDebug\IpDrive.pch"
	-@erase ".\UniDebug\Ipremote.obj"
	-@erase ".\UniDebug\Ipdrive.obj"
	-@erase ".\UniDebug\Ipdrivw.obj"
	-@erase ".\UniDebug\Mainfrm.obj"
	-@erase ".\UniDebug\Ipdridoc.obj"
	-@erase ".\UniDebug\Stdafx.obj"
	-@erase ".\UniDebug\Ipdrive.res"
	-@erase ".\UniDebug\IpDrive.idb"
	-@erase ".\UniDebug\IpDrive.bsc"
	-@erase ".\UniDebug\Ipdrive.sbr"
	-@erase ".\UniDebug\Ipdrivw.sbr"
	-@erase ".\UniDebug\Mainfrm.sbr"
	-@erase ".\UniDebug\Ipdridoc.sbr"
	-@erase ".\UniDebug\Stdafx.sbr"
	-@erase ".\UniDebug\Ipremote.sbr"
	-@erase ".\UniDebug\IpDrive.exe"
	-@erase ".\UniDebug\IpDrive.ilk"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_UNICODE" /D "_AFXDLL" /D "_MBCS" /FR /Yu"Stdafx.h" /c
CPP_PROJ=/nologo /MDd /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS"\
 /D "_UNICODE" /D "_AFXDLL" /D "_MBCS" /FR"$(INTDIR)/"\
 /Fp"$(INTDIR)/IpDrive.pch" /Yu"Stdafx.h" /Fo"$(INTDIR)/"\
 /Fd"$(OUTDIR)/IpDrive.pdb" /c 
CPP_OBJS=.\UniDebug/
CPP_SBRS=.\UniDebug/
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/Ipdrive.res" /d "_DEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/IpDrive.bsc" 
BSC32_SBRS= \
	"$(INTDIR)/Ipdrive.sbr" \
	"$(INTDIR)/Ipdrivw.sbr" \
	"$(INTDIR)/Mainfrm.sbr" \
	"$(INTDIR)/Ipdridoc.sbr" \
	"$(INTDIR)/Stdafx.sbr" \
	"$(INTDIR)/Ipremote.sbr"

"$(OUTDIR)\IpDrive.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386
# ADD LINK32 /nologo /entry:"wWinMainCRTStartup" /subsystem:windows /debug /machine:I386
LINK32_FLAGS=/nologo /entry:"wWinMainCRTStartup" /subsystem:windows\
 /incremental:yes /pdb:"$(OUTDIR)/IpDrive.pdb" /debug /machine:I386\
 /out:"$(OUTDIR)/IpDrive.exe" 
LINK32_OBJS= \
	"$(INTDIR)/Ipremote.obj" \
	"$(INTDIR)/Ipdrive.obj" \
	"$(INTDIR)/Ipdrivw.obj" \
	"$(INTDIR)/Mainfrm.obj" \
	"$(INTDIR)/Ipdridoc.obj" \
	"$(INTDIR)/Stdafx.obj" \
	"$(INTDIR)/Ipdrive.res"

"$(OUTDIR)\IpDrive.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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

# Name "IpDrive - Win32 Release"
# Name "IpDrive - Win32 Debug"
# Name "IpDrive - Win32 Unicode Release"
# Name "IpDrive - Win32 Unicode Debug"

!IF  "$(CFG)" == "IpDrive - Win32 Release"

!ELSEIF  "$(CFG)" == "IpDrive - Win32 Debug"

!ELSEIF  "$(CFG)" == "IpDrive - Win32 Unicode Release"

!ELSEIF  "$(CFG)" == "IpDrive - Win32 Unicode Debug"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\Stdafx.cpp
DEP_CPP_STDAF=\
	".\Stdafx.h"\
	

!IF  "$(CFG)" == "IpDrive - Win32 Release"

# ADD CPP /Yc

BuildCmds= \
	$(CPP) /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /FR"$(INTDIR)/" /Fp"$(INTDIR)/IpDrive.pch" /Yc\
 /Fo"$(INTDIR)/" /Fd"$(OUTDIR)/IpDrive.pdb" /c $(SOURCE) \
	

"$(INTDIR)\Stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\Stdafx.sbr" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\IpDrive.idb" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\IpDrive.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "IpDrive - Win32 Debug"

# ADD CPP /Yc

BuildCmds= \
	$(CPP) /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /D "_AFXDLL" /D "_MBCS" /FR"$(INTDIR)/" /Fp"$(INTDIR)/IpDrive.pch" /Yc\
 /Fo"$(INTDIR)/" /Fd"$(OUTDIR)/IpDrive.pdb" /c $(SOURCE) \
	

"$(INTDIR)\Stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\Stdafx.sbr" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\IpDrive.pdb" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\IpDrive.idb" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\IpDrive.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "IpDrive - Win32 Unicode Release"

# ADD CPP /Yc"Stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D\
 "_UNICODE" /D "_AFXDLL" /D "_MBCS" /FR"$(INTDIR)/" /Fp"$(INTDIR)/IpDrive.pch"\
 /Yc"Stdafx.h" /Fo"$(INTDIR)/" /Fd"$(OUTDIR)/IpDrive.pdb" /c $(SOURCE) \
	

"$(INTDIR)\Stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\Stdafx.sbr" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\IpDrive.idb" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\IpDrive.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "IpDrive - Win32 Unicode Debug"

# ADD CPP /Yc

BuildCmds= \
	$(CPP) /nologo /MDd /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS"\
 /D "_UNICODE" /D "_AFXDLL" /D "_MBCS" /FR"$(INTDIR)/"\
 /Fp"$(INTDIR)/IpDrive.pch" /Yc /Fo"$(INTDIR)/" /Fd"$(OUTDIR)/IpDrive.pdb" /c\
 $(SOURCE) \
	

"$(INTDIR)\Stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\Stdafx.sbr" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\IpDrive.pdb" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\IpDrive.idb" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\IpDrive.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Ipdrive.cpp
DEP_CPP_IPDRI=\
	".\Ipdridoc.h"\
	".\Ipremote.h"\
	".\Ipdrive.h"\
	".\Resource.h"\
	".\Ipdrivw.h"\
	".\Mainfrm.h"\
	".\Stdafx.h"\
	

!IF  "$(CFG)" == "IpDrive - Win32 Release"


"$(INTDIR)\Ipdrive.obj" : $(SOURCE) $(DEP_CPP_IPDRI) "$(INTDIR)"\
 "$(INTDIR)\IpDrive.pch"

"$(INTDIR)\Ipdrive.sbr" : $(SOURCE) $(DEP_CPP_IPDRI) "$(INTDIR)"\
 "$(INTDIR)\IpDrive.pch"

"$(INTDIR)\IpDrive.idb" : $(SOURCE) $(DEP_CPP_IPDRI) "$(INTDIR)"\
 "$(INTDIR)\IpDrive.pch"


!ELSEIF  "$(CFG)" == "IpDrive - Win32 Debug"


"$(INTDIR)\Ipdrive.obj" : $(SOURCE) $(DEP_CPP_IPDRI) "$(INTDIR)"\
 "$(INTDIR)\IpDrive.pch"

"$(INTDIR)\Ipdrive.sbr" : $(SOURCE) $(DEP_CPP_IPDRI) "$(INTDIR)"\
 "$(INTDIR)\IpDrive.pch"

"$(INTDIR)\IpDrive.pdb" : $(SOURCE) $(DEP_CPP_IPDRI) "$(INTDIR)"\
 "$(INTDIR)\IpDrive.pch"

"$(INTDIR)\IpDrive.idb" : $(SOURCE) $(DEP_CPP_IPDRI) "$(INTDIR)"\
 "$(INTDIR)\IpDrive.pch"


!ELSEIF  "$(CFG)" == "IpDrive - Win32 Unicode Release"


"$(INTDIR)\Ipdrive.obj" : $(SOURCE) $(DEP_CPP_IPDRI) "$(INTDIR)"\
 "$(INTDIR)\IpDrive.pch"

"$(INTDIR)\Ipdrive.sbr" : $(SOURCE) $(DEP_CPP_IPDRI) "$(INTDIR)"\
 "$(INTDIR)\IpDrive.pch"

"$(INTDIR)\IpDrive.idb" : $(SOURCE) $(DEP_CPP_IPDRI) "$(INTDIR)"\
 "$(INTDIR)\IpDrive.pch"


!ELSEIF  "$(CFG)" == "IpDrive - Win32 Unicode Debug"


"$(INTDIR)\Ipdrive.obj" : $(SOURCE) $(DEP_CPP_IPDRI) "$(INTDIR)"\
 "$(INTDIR)\IpDrive.pch"

"$(INTDIR)\Ipdrive.sbr" : $(SOURCE) $(DEP_CPP_IPDRI) "$(INTDIR)"\
 "$(INTDIR)\IpDrive.pch"

"$(INTDIR)\IpDrive.pdb" : $(SOURCE) $(DEP_CPP_IPDRI) "$(INTDIR)"\
 "$(INTDIR)\IpDrive.pch"

"$(INTDIR)\IpDrive.idb" : $(SOURCE) $(DEP_CPP_IPDRI) "$(INTDIR)"\
 "$(INTDIR)\IpDrive.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Ipdrive.rc
DEP_RSC_IPDRIV=\
	".\Res\Ipdrive.ico"\
	".\Res\Toolbar.bmp"\
	".\Res\Ipdrive.rc2"\
	

!IF  "$(CFG)" == "IpDrive - Win32 Release"


"$(INTDIR)\Ipdrive.res" : $(SOURCE) $(DEP_RSC_IPDRIV) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "IpDrive - Win32 Debug"


"$(INTDIR)\Ipdrive.res" : $(SOURCE) $(DEP_RSC_IPDRIV) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "IpDrive - Win32 Unicode Release"


"$(INTDIR)\Ipdrive.res" : $(SOURCE) $(DEP_RSC_IPDRIV) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "IpDrive - Win32 Unicode Debug"


"$(INTDIR)\Ipdrive.res" : $(SOURCE) $(DEP_RSC_IPDRIV) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Ipdrivw.cpp
DEP_CPP_IPDRIVW=\
	".\Ipdridoc.h"\
	".\Ipremote.h"\
	".\Ipdrive.h"\
	".\Resource.h"\
	".\Ipdrivw.h"\
	".\Stdafx.h"\
	

!IF  "$(CFG)" == "IpDrive - Win32 Release"


"$(INTDIR)\Ipdrivw.obj" : $(SOURCE) $(DEP_CPP_IPDRIVW) "$(INTDIR)"\
 "$(INTDIR)\IpDrive.pch"

"$(INTDIR)\Ipdrivw.sbr" : $(SOURCE) $(DEP_CPP_IPDRIVW) "$(INTDIR)"\
 "$(INTDIR)\IpDrive.pch"

"$(INTDIR)\IpDrive.idb" : $(SOURCE) $(DEP_CPP_IPDRIVW) "$(INTDIR)"\
 "$(INTDIR)\IpDrive.pch"


!ELSEIF  "$(CFG)" == "IpDrive - Win32 Debug"


"$(INTDIR)\Ipdrivw.obj" : $(SOURCE) $(DEP_CPP_IPDRIVW) "$(INTDIR)"\
 "$(INTDIR)\IpDrive.pch"

"$(INTDIR)\Ipdrivw.sbr" : $(SOURCE) $(DEP_CPP_IPDRIVW) "$(INTDIR)"\
 "$(INTDIR)\IpDrive.pch"

"$(INTDIR)\IpDrive.pdb" : $(SOURCE) $(DEP_CPP_IPDRIVW) "$(INTDIR)"\
 "$(INTDIR)\IpDrive.pch"

"$(INTDIR)\IpDrive.idb" : $(SOURCE) $(DEP_CPP_IPDRIVW) "$(INTDIR)"\
 "$(INTDIR)\IpDrive.pch"


!ELSEIF  "$(CFG)" == "IpDrive - Win32 Unicode Release"


"$(INTDIR)\Ipdrivw.obj" : $(SOURCE) $(DEP_CPP_IPDRIVW) "$(INTDIR)"\
 "$(INTDIR)\IpDrive.pch"

"$(INTDIR)\Ipdrivw.sbr" : $(SOURCE) $(DEP_CPP_IPDRIVW) "$(INTDIR)"\
 "$(INTDIR)\IpDrive.pch"

"$(INTDIR)\IpDrive.idb" : $(SOURCE) $(DEP_CPP_IPDRIVW) "$(INTDIR)"\
 "$(INTDIR)\IpDrive.pch"


!ELSEIF  "$(CFG)" == "IpDrive - Win32 Unicode Debug"


"$(INTDIR)\Ipdrivw.obj" : $(SOURCE) $(DEP_CPP_IPDRIVW) "$(INTDIR)"\
 "$(INTDIR)\IpDrive.pch"

"$(INTDIR)\Ipdrivw.sbr" : $(SOURCE) $(DEP_CPP_IPDRIVW) "$(INTDIR)"\
 "$(INTDIR)\IpDrive.pch"

"$(INTDIR)\IpDrive.pdb" : $(SOURCE) $(DEP_CPP_IPDRIVW) "$(INTDIR)"\
 "$(INTDIR)\IpDrive.pch"

"$(INTDIR)\IpDrive.idb" : $(SOURCE) $(DEP_CPP_IPDRIVW) "$(INTDIR)"\
 "$(INTDIR)\IpDrive.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Ipremote.cpp

!IF  "$(CFG)" == "IpDrive - Win32 Release"

DEP_CPP_IPREM=\
	".\Ipremote.h"\
	".\Stdafx.h"\
	

"$(INTDIR)\Ipremote.obj" : $(SOURCE) $(DEP_CPP_IPREM) "$(INTDIR)"\
 "$(INTDIR)\IpDrive.pch"

"$(INTDIR)\Ipremote.sbr" : $(SOURCE) $(DEP_CPP_IPREM) "$(INTDIR)"\
 "$(INTDIR)\IpDrive.pch"

"$(INTDIR)\IpDrive.idb" : $(SOURCE) $(DEP_CPP_IPREM) "$(INTDIR)"\
 "$(INTDIR)\IpDrive.pch"


!ELSEIF  "$(CFG)" == "IpDrive - Win32 Debug"

DEP_CPP_IPREM=\
	".\Ipremote.h"\
	".\Stdafx.h"\
	

"$(INTDIR)\Ipremote.obj" : $(SOURCE) $(DEP_CPP_IPREM) "$(INTDIR)"\
 "$(INTDIR)\IpDrive.pch"

"$(INTDIR)\Ipremote.sbr" : $(SOURCE) $(DEP_CPP_IPREM) "$(INTDIR)"\
 "$(INTDIR)\IpDrive.pch"

"$(INTDIR)\IpDrive.pdb" : $(SOURCE) $(DEP_CPP_IPREM) "$(INTDIR)"\
 "$(INTDIR)\IpDrive.pch"

"$(INTDIR)\IpDrive.idb" : $(SOURCE) $(DEP_CPP_IPREM) "$(INTDIR)"\
 "$(INTDIR)\IpDrive.pch"


!ELSEIF  "$(CFG)" == "IpDrive - Win32 Unicode Release"

DEP_CPP_IPREM=\
	".\Stdafx.h"\
	".\Ipremote.h"\
	

"$(INTDIR)\Ipremote.obj" : $(SOURCE) $(DEP_CPP_IPREM) "$(INTDIR)"\
 "$(INTDIR)\IpDrive.pch"

"$(INTDIR)\Ipremote.sbr" : $(SOURCE) $(DEP_CPP_IPREM) "$(INTDIR)"\
 "$(INTDIR)\IpDrive.pch"

"$(INTDIR)\IpDrive.idb" : $(SOURCE) $(DEP_CPP_IPREM) "$(INTDIR)"\
 "$(INTDIR)\IpDrive.pch"


!ELSEIF  "$(CFG)" == "IpDrive - Win32 Unicode Debug"

DEP_CPP_IPREM=\
	".\Stdafx.h"\
	".\Ipremote.h"\
	

"$(INTDIR)\Ipremote.obj" : $(SOURCE) $(DEP_CPP_IPREM) "$(INTDIR)"\
 "$(INTDIR)\IpDrive.pch"

"$(INTDIR)\Ipremote.sbr" : $(SOURCE) $(DEP_CPP_IPREM) "$(INTDIR)"\
 "$(INTDIR)\IpDrive.pch"

"$(INTDIR)\IpDrive.pdb" : $(SOURCE) $(DEP_CPP_IPREM) "$(INTDIR)"\
 "$(INTDIR)\IpDrive.pch"

"$(INTDIR)\IpDrive.idb" : $(SOURCE) $(DEP_CPP_IPREM) "$(INTDIR)"\
 "$(INTDIR)\IpDrive.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Mainfrm.cpp
DEP_CPP_MAINF=\
	".\Ipdrive.h"\
	".\Resource.h"\
	".\Mainfrm.h"\
	".\Stdafx.h"\
	

!IF  "$(CFG)" == "IpDrive - Win32 Release"


"$(INTDIR)\Mainfrm.obj" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\IpDrive.pch"

"$(INTDIR)\Mainfrm.sbr" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\IpDrive.pch"

"$(INTDIR)\IpDrive.idb" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\IpDrive.pch"


!ELSEIF  "$(CFG)" == "IpDrive - Win32 Debug"


"$(INTDIR)\Mainfrm.obj" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\IpDrive.pch"

"$(INTDIR)\Mainfrm.sbr" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\IpDrive.pch"

"$(INTDIR)\IpDrive.pdb" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\IpDrive.pch"

"$(INTDIR)\IpDrive.idb" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\IpDrive.pch"


!ELSEIF  "$(CFG)" == "IpDrive - Win32 Unicode Release"


"$(INTDIR)\Mainfrm.obj" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\IpDrive.pch"

"$(INTDIR)\Mainfrm.sbr" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\IpDrive.pch"

"$(INTDIR)\IpDrive.idb" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\IpDrive.pch"


!ELSEIF  "$(CFG)" == "IpDrive - Win32 Unicode Debug"


"$(INTDIR)\Mainfrm.obj" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\IpDrive.pch"

"$(INTDIR)\Mainfrm.sbr" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\IpDrive.pch"

"$(INTDIR)\IpDrive.pdb" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\IpDrive.pch"

"$(INTDIR)\IpDrive.idb" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\IpDrive.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Ipdridoc.cpp
DEP_CPP_IPDRID=\
	".\Ipdridoc.h"\
	".\Ipremote.h"\
	".\Ipdrive.h"\
	".\Resource.h"\
	".\Stdafx.h"\
	

!IF  "$(CFG)" == "IpDrive - Win32 Release"


"$(INTDIR)\Ipdridoc.obj" : $(SOURCE) $(DEP_CPP_IPDRID) "$(INTDIR)"\
 "$(INTDIR)\IpDrive.pch"

"$(INTDIR)\Ipdridoc.sbr" : $(SOURCE) $(DEP_CPP_IPDRID) "$(INTDIR)"\
 "$(INTDIR)\IpDrive.pch"

"$(INTDIR)\IpDrive.idb" : $(SOURCE) $(DEP_CPP_IPDRID) "$(INTDIR)"\
 "$(INTDIR)\IpDrive.pch"


!ELSEIF  "$(CFG)" == "IpDrive - Win32 Debug"


"$(INTDIR)\Ipdridoc.obj" : $(SOURCE) $(DEP_CPP_IPDRID) "$(INTDIR)"\
 "$(INTDIR)\IpDrive.pch"

"$(INTDIR)\Ipdridoc.sbr" : $(SOURCE) $(DEP_CPP_IPDRID) "$(INTDIR)"\
 "$(INTDIR)\IpDrive.pch"

"$(INTDIR)\IpDrive.pdb" : $(SOURCE) $(DEP_CPP_IPDRID) "$(INTDIR)"\
 "$(INTDIR)\IpDrive.pch"

"$(INTDIR)\IpDrive.idb" : $(SOURCE) $(DEP_CPP_IPDRID) "$(INTDIR)"\
 "$(INTDIR)\IpDrive.pch"


!ELSEIF  "$(CFG)" == "IpDrive - Win32 Unicode Release"


"$(INTDIR)\Ipdridoc.obj" : $(SOURCE) $(DEP_CPP_IPDRID) "$(INTDIR)"\
 "$(INTDIR)\IpDrive.pch"

"$(INTDIR)\Ipdridoc.sbr" : $(SOURCE) $(DEP_CPP_IPDRID) "$(INTDIR)"\
 "$(INTDIR)\IpDrive.pch"

"$(INTDIR)\IpDrive.idb" : $(SOURCE) $(DEP_CPP_IPDRID) "$(INTDIR)"\
 "$(INTDIR)\IpDrive.pch"


!ELSEIF  "$(CFG)" == "IpDrive - Win32 Unicode Debug"


"$(INTDIR)\Ipdridoc.obj" : $(SOURCE) $(DEP_CPP_IPDRID) "$(INTDIR)"\
 "$(INTDIR)\IpDrive.pch"

"$(INTDIR)\Ipdridoc.sbr" : $(SOURCE) $(DEP_CPP_IPDRID) "$(INTDIR)"\
 "$(INTDIR)\IpDrive.pch"

"$(INTDIR)\IpDrive.pdb" : $(SOURCE) $(DEP_CPP_IPDRID) "$(INTDIR)"\
 "$(INTDIR)\IpDrive.pch"

"$(INTDIR)\IpDrive.idb" : $(SOURCE) $(DEP_CPP_IPDRID) "$(INTDIR)"\
 "$(INTDIR)\IpDrive.pch"


!ENDIF 

# End Source File
# End Target
# End Project
################################################################################
