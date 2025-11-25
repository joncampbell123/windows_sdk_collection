# Microsoft Visual C++ Generated NMAKE File, Format Version 40000
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

!IF "$(CFG)" == ""
CFG=Localize - Win32 Debug
!MESSAGE No configuration specified.  Defaulting to Localize - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "Localize - Win32 Release" && "$(CFG)" !=\
 "Localize - Win32 Debug" && "$(CFG)" != "Localize - Win32 Unicode Release" &&\
 "$(CFG)" != "Localize - Win32 Unicode Debug" && "$(CFG)" !=\
 "Locresde - Win32 Release" && "$(CFG)" != "Locresde - Win32 Debug" && "$(CFG)"\
 != "Locresfr - Win32 Release" && "$(CFG)" != "Locresfr - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "localize.mak" CFG="Localize - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Localize - Win32 Release" (based on\
 "Win32 (x86) Dynamic-Link Library")
!MESSAGE "Localize - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "Localize - Win32 Unicode Release" (based on\
 "Win32 (x86) Dynamic-Link Library")
!MESSAGE "Localize - Win32 Unicode Debug" (based on\
 "Win32 (x86) Dynamic-Link Library")
!MESSAGE "Locresde - Win32 Release" (based on\
 "Win32 (x86) Dynamic-Link Library")
!MESSAGE "Locresde - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "Locresfr - Win32 Release" (based on\
 "Win32 (x86) Dynamic-Link Library")
!MESSAGE "Locresfr - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
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
# PROP Target_Last_Scanned "Locresfr - Win32 Debug"
RSC=rc.exe
MTL=mktyplib.exe
CPP=cl.exe

!IF  "$(CFG)" == "Localize - Win32 Release"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Classwizard_Name localize.clw
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 0
# PROP Output_Dir Release
# PROP Intermediate_Dir Release
# PROP Classwizard_Name localize.clw
OUTDIR=.\Release
INTDIR=.\Release

ALL : "$(OUTDIR)\Localize.ocx"

CLEAN : 
	-@erase ".\Release\Localize.ocx"
	-@erase ".\Release\stdafx.obj"
	-@erase ".\Release\localppg.obj"
	-@erase ".\Release\localize.pch"
	-@erase ".\Release\localize.obj"
	-@erase ".\Release\localctl.obj"
	-@erase ".\Release\localize.res"
	-@erase ".\Release\localize.tlb"
	-@erase ".\Release\Localize.lib"
	-@erase ".\Release\Localize.exp"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MD /W3 /GX /Ox /D "NDEBUG" /D "_MBCS" /D "_WINDOWS" /D "_AFXDLL" /D "_AFXCTL" /D "_WINDLL" /FR /Yu"stdafx.h" /c
# ADD CPP /nologo /MD /W4 /GX /Ox /D "NDEBUG" /D "_WINDOWS" /D "_AFXCTL" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
# SUBTRACT CPP /Fr
CPP_PROJ=/nologo /MD /W4 /GX /Ox /D "NDEBUG" /D "_WINDOWS" /D "_AFXCTL" /D\
 "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/localize.pch" /Yu"stdafx.h" /Fo"$(INTDIR)/"\
 /c 
CPP_OBJS=.\Release/
CPP_SBRS=
# ADD BASE MTL /nologo /D "NDEBUG" /D "_WIN32" /win32
# ADD MTL /nologo /D "NDEBUG" /D "_WIN32" /win32
MTL_PROJ=/nologo /D "NDEBUG" /D "_WIN32" /win32 
# ADD BASE RSC /l 0x409 /i "$(OUTDIR)" /d "NDEBUG" /d "_WIN32"
# ADD RSC /l 0x409 /i "$(OUTDIR)" /d "NDEBUG" /d "_WIN32" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/localize.res" /i "$(OUTDIR)" /d "NDEBUG" /d\
 "_WIN32" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo /o"Release/localize.bsc"
# SUBTRACT BASE BSC32 /Iu
# ADD BSC32 /nologo
# SUBTRACT BSC32 /Iu
BSC32_FLAGS=/nologo /o"$(OUTDIR)/localize.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /machine:IX86 /out:"Release/localize.ocx"
# SUBTRACT BASE LINK32 /pdb:none /nodefaultlib
# ADD LINK32 /nologo /subsystem:windows /dll /machine:IX86 /out:"Release/Localize.ocx"
# SUBTRACT LINK32 /pdb:none
LINK32_FLAGS=/nologo /subsystem:windows /dll /incremental:no\
 /pdb:"$(OUTDIR)/Localize.pdb" /machine:IX86 /def:".\localize.def"\
 /out:"$(OUTDIR)/Localize.ocx" /implib:"$(OUTDIR)/Localize.lib" 
DEF_FILE= \
	".\localize.def"
LINK32_OBJS= \
	"$(INTDIR)/stdafx.obj" \
	"$(INTDIR)/localppg.obj" \
	"$(INTDIR)/localize.obj" \
	"$(INTDIR)/localctl.obj" \
	"$(INTDIR)/localize.res"

"$(OUTDIR)\Localize.ocx" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "Localize - Win32 Debug"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Classwizard_Name localize.clw
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 1
# PROP Output_Dir Debug
# PROP Intermediate_Dir Debug
# PROP Classwizard_Name localize.clw
OUTDIR=.\Debug
INTDIR=.\Debug

ALL : "$(OUTDIR)\Localize.ocx"

CLEAN : 
	-@erase ".\Debug\vc40.pdb"
	-@erase ".\Debug\localize.pch"
	-@erase ".\Debug\Localize.ocx"
	-@erase ".\Debug\localctl.obj"
	-@erase ".\Debug\localppg.obj"
	-@erase ".\Debug\localize.obj"
	-@erase ".\Debug\stdafx.obj"
	-@erase ".\Debug\localize.res"
	-@erase ".\Debug\localize.tlb"
	-@erase ".\Debug\Localize.ilk"
	-@erase ".\Debug\Localize.lib"
	-@erase ".\Debug\Localize.exp"
	-@erase ".\Debug\Localize.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MD /W3 /GX /Zi /Od /D "_DEBUG" /D "_MBCS" /D "_WINDOWS" /D "_AFXDLL" /D "_AFXCTL" /D "_WINDLL" /FR /Yu"stdafx.h" /c
# ADD CPP /nologo /MDd /W4 /Gm /GX /Zi /Od /D "_DEBUG" /D "_WINDOWS" /D "_AFXCTL" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
# SUBTRACT CPP /WX /Fr
CPP_PROJ=/nologo /MDd /W4 /Gm /GX /Zi /Od /D "_DEBUG" /D "_WINDOWS" /D\
 "_AFXCTL" /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/localize.pch" /Yu"stdafx.h"\
 /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\Debug/
CPP_SBRS=
# ADD BASE MTL /nologo /D "_DEBUG" /D "_WIN32" /win32
# ADD MTL /nologo /D "_DEBUG" /D "_WIN32" /win32
MTL_PROJ=/nologo /D "_DEBUG" /D "_WIN32" /win32 
# ADD BASE RSC /l 0x409 /i "$(OUTDIR)" /d "_DEBUG" /d "_WIN32"
# ADD RSC /l 0x409 /i "$(OUTDIR)" /d "_DEBUG" /d "_WIN32" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/localize.res" /i "$(OUTDIR)" /d "_DEBUG" /d\
 "_WIN32" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo /o"Debug/localize.bsc"
# SUBTRACT BASE BSC32 /Iu
# ADD BSC32 /nologo
# SUBTRACT BSC32 /Iu
BSC32_FLAGS=/nologo /o"$(OUTDIR)/localize.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /debug /machine:IX86 /out:"Debug/localize.ocx"
# SUBTRACT BASE LINK32 /pdb:none /nodefaultlib
# ADD LINK32 /nologo /subsystem:windows /dll /debug /machine:IX86 /out:"Debug/Localize.ocx"
# SUBTRACT LINK32 /pdb:none
LINK32_FLAGS=/nologo /subsystem:windows /dll /incremental:yes\
 /pdb:"$(OUTDIR)/Localize.pdb" /debug /machine:IX86 /def:".\localize.def"\
 /out:"$(OUTDIR)/Localize.ocx" /implib:"$(OUTDIR)/Localize.lib" 
DEF_FILE= \
	".\localize.def"
LINK32_OBJS= \
	"$(INTDIR)/localctl.obj" \
	"$(INTDIR)/localppg.obj" \
	"$(INTDIR)/localize.obj" \
	"$(INTDIR)/stdafx.obj" \
	"$(INTDIR)/localize.res"

"$(OUTDIR)\Localize.ocx" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "Localize - Win32 Unicode Release"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Classwizard_Name localize.clw
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 0
# PROP Output_Dir UniRelease
# PROP Intermediate_Dir UniRelease
# PROP Classwizard_Name localize.clw
OUTDIR=.\UniRelease
INTDIR=.\UniRelease

ALL : "$(OUTDIR)\Localize.ocx"

CLEAN : 
	-@erase ".\UniRelease\Localize.ocx"
	-@erase ".\UniRelease\localppg.obj"
	-@erase ".\UniRelease\localize.pch"
	-@erase ".\UniRelease\localize.obj"
	-@erase ".\UniRelease\stdafx.obj"
	-@erase ".\UniRelease\localctl.obj"
	-@erase ".\UniRelease\localize.res"
	-@erase ".\UniRelease\localize.tlb"
	-@erase ".\UniRelease\Localize.lib"
	-@erase ".\UniRelease\Localize.exp"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MD /W3 /GX /Ox /D "NDEBUG" /D "_UNICODE" /D "_WINDOWS" /D "_AFXDLL" /D "_AFXCTL" /D "_WINDLL" /FR /Yu"stdafx.h" /c
# ADD CPP /nologo /MD /W4 /GX /Ox /D "NDEBUG" /D "_UNICODE" /D "_WINDOWS" /D "_AFXCTL" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
# SUBTRACT CPP /Fr
CPP_PROJ=/nologo /MD /W4 /GX /Ox /D "NDEBUG" /D "_UNICODE" /D "_WINDOWS" /D\
 "_AFXCTL" /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/localize.pch" /Yu"stdafx.h"\
 /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\UniRelease/
CPP_SBRS=
# ADD BASE MTL /nologo /D "NDEBUG" /D "_UNICODE" /D "_WIN32" /win32
# ADD MTL /nologo /D "NDEBUG" /D "_UNICODE" /D "_WIN32" /win32
MTL_PROJ=/nologo /D "NDEBUG" /D "_UNICODE" /D "_WIN32" /win32 
# ADD BASE RSC /l 0x409 /i "$(OUTDIR)" /d "NDEBUG" /d "_WIN32" /d "_UNICODE"
# ADD RSC /l 0x409 /i "$(OUTDIR)" /d "NDEBUG" /d "_WIN32" /d "_UNICODE" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/localize.res" /i "$(OUTDIR)" /d "NDEBUG" /d\
 "_WIN32" /d "_UNICODE" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo /o"UniRelease/localize.bsc"
# SUBTRACT BASE BSC32 /Iu
# ADD BSC32 /nologo
# SUBTRACT BSC32 /Iu
BSC32_FLAGS=/nologo /o"$(OUTDIR)/localize.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /machine:IX86 /out:"UniRelease/localize.ocx"
# SUBTRACT BASE LINK32 /pdb:none /nodefaultlib
# ADD LINK32 /nologo /subsystem:windows /dll /machine:IX86 /out:"UniRelease/Localize.ocx"
# SUBTRACT LINK32 /pdb:none
LINK32_FLAGS=/nologo /subsystem:windows /dll /incremental:no\
 /pdb:"$(OUTDIR)/Localize.pdb" /machine:IX86 /def:".\localize.def"\
 /out:"$(OUTDIR)/Localize.ocx" /implib:"$(OUTDIR)/Localize.lib" 
DEF_FILE= \
	".\localize.def"
LINK32_OBJS= \
	"$(INTDIR)/localppg.obj" \
	"$(INTDIR)/localize.obj" \
	"$(INTDIR)/stdafx.obj" \
	"$(INTDIR)/localctl.obj" \
	"$(INTDIR)/localize.res"

"$(OUTDIR)\Localize.ocx" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "Localize - Win32 Unicode Debug"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Classwizard_Name localize.clw
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 1
# PROP Output_Dir UniDebug
# PROP Intermediate_Dir UniDebug
# PROP Classwizard_Name localize.clw
OUTDIR=.\UniDebug
INTDIR=.\UniDebug

ALL : "$(OUTDIR)\Localize.ocx"

CLEAN : 
	-@erase ".\UniDebug\vc40.pdb"
	-@erase ".\UniDebug\localize.pch"
	-@erase ".\UniDebug\Localize.ocx"
	-@erase ".\UniDebug\localctl.obj"
	-@erase ".\UniDebug\stdafx.obj"
	-@erase ".\UniDebug\localppg.obj"
	-@erase ".\UniDebug\localize.obj"
	-@erase ".\UniDebug\localize.res"
	-@erase ".\UniDebug\localize.tlb"
	-@erase ".\UniDebug\Localize.ilk"
	-@erase ".\UniDebug\Localize.lib"
	-@erase ".\UniDebug\Localize.exp"
	-@erase ".\UniDebug\Localize.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MD /W3 /GX /Zi /Od /D "_DEBUG" /D "_UNICODE" /D "_WINDOWS" /D "_AFXDLL" /D "_AFXCTL" /D "_WINDLL" /FR /Yu"stdafx.h" /c
# SUBTRACT BASE CPP /WX
# ADD CPP /nologo /MDd /W4 /Gm /GX /Zi /Od /D "_DEBUG" /D "_UNICODE" /D "_WINDOWS" /D "_AFXCTL" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
# SUBTRACT CPP /WX /Fr
CPP_PROJ=/nologo /MDd /W4 /Gm /GX /Zi /Od /D "_DEBUG" /D "_UNICODE" /D\
 "_WINDOWS" /D "_AFXCTL" /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/localize.pch"\
 /Yu"stdafx.h" /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\UniDebug/
CPP_SBRS=
# ADD BASE MTL /nologo /D "_DEBUG" /D "_UNICODE" /D "_WIN32" /win32
# ADD MTL /nologo /D "_DEBUG" /D "_UNICODE" /D "_WIN32" /win32
MTL_PROJ=/nologo /D "_DEBUG" /D "_UNICODE" /D "_WIN32" /win32 
# ADD BASE RSC /l 0x409 /i "$(OUTDIR)" /d "_DEBUG" /d "_WIN32" /d "_UNICODE"
# ADD RSC /l 0x409 /i "$(OUTDIR)" /d "_DEBUG" /d "_WIN32" /d "_UNICODE" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/localize.res" /i "$(OUTDIR)" /d "_DEBUG" /d\
 "_WIN32" /d "_UNICODE" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo /o"UniDebug/localize.bsc"
# SUBTRACT BASE BSC32 /Iu
# ADD BSC32 /nologo
# SUBTRACT BSC32 /Iu
BSC32_FLAGS=/nologo /o"$(OUTDIR)/localize.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /debug /machine:IX86 /out:"UniDebug/localize.ocx"
# SUBTRACT BASE LINK32 /pdb:none /nodefaultlib
# ADD LINK32 /nologo /subsystem:windows /dll /debug /machine:IX86 /out:"UniDebug/Localize.ocx"
# SUBTRACT LINK32 /pdb:none
LINK32_FLAGS=/nologo /subsystem:windows /dll /incremental:yes\
 /pdb:"$(OUTDIR)/Localize.pdb" /debug /machine:IX86 /def:".\localize.def"\
 /out:"$(OUTDIR)/Localize.ocx" /implib:"$(OUTDIR)/Localize.lib" 
DEF_FILE= \
	".\localize.def"
LINK32_OBJS= \
	"$(INTDIR)/localctl.obj" \
	"$(INTDIR)/stdafx.obj" \
	"$(INTDIR)/localppg.obj" \
	"$(INTDIR)/localize.obj" \
	"$(INTDIR)/localize.res"

"$(OUTDIR)\Localize.ocx" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "Locresde - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir Locresde\Release
# PROP BASE Intermediate_Dir Locresde\Release
# PROP BASE Target_Dir Locresde
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 0
# PROP Output_Dir Release
# PROP Intermediate_Dir Release
# PROP Target_Dir Locresde
OUTDIR=.\Release
INTDIR=.\Release

ALL : "$(OUTDIR)\Locresde.dll"

CLEAN : 
	-@erase ".\Release\Locresde.dll"
	-@erase ".\Release\locresde.obj"
	-@erase ".\Release\locresde.res"
	-@erase ".\Release\Locresde.lib"
	-@erase ".\Release\Locresde.exp"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /YX /c
CPP_PROJ=/nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/Locresde.pch" /YX /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\Release/
CPP_SBRS=
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/locresde.res" /d "NDEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/Locresde.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 /nologo /subsystem:windows /dll /machine:I386
LINK32_FLAGS=/nologo /subsystem:windows /dll /incremental:no\
 /pdb:"$(OUTDIR)/Locresde.pdb" /machine:I386 /out:"$(OUTDIR)/Locresde.dll"\
 /implib:"$(OUTDIR)/Locresde.lib" 
LINK32_OBJS= \
	"$(INTDIR)/locresde.obj" \
	"$(INTDIR)/locresde.res"

"$(OUTDIR)\Locresde.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "Locresde - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir Locresde\Debug
# PROP BASE Intermediate_Dir Locresde\Debug
# PROP BASE Target_Dir Locresde
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 1
# PROP Output_Dir Debug
# PROP Intermediate_Dir Debug
# PROP Target_Dir Locresde
OUTDIR=.\Debug
INTDIR=.\Debug

ALL : "$(OUTDIR)\Locresde.dll"

CLEAN : 
	-@erase ".\Debug\vc40.pdb"
	-@erase ".\Debug\Locresde.dll"
	-@erase ".\Debug\locresde.obj"
	-@erase ".\Debug\locresde.res"
	-@erase ".\Debug\Locresde.ilk"
	-@erase ".\Debug\Locresde.lib"
	-@erase ".\Debug\Locresde.exp"
	-@erase ".\Debug\Locresde.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /YX /c
CPP_PROJ=/nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/Locresde.pch" /YX /Fo"$(INTDIR)/"\
 /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\Debug/
CPP_SBRS=
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/locresde.res" /d "_DEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/Locresde.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386
# ADD LINK32 /nologo /subsystem:windows /dll /debug /machine:I386
LINK32_FLAGS=/nologo /subsystem:windows /dll /incremental:yes\
 /pdb:"$(OUTDIR)/Locresde.pdb" /debug /machine:I386\
 /out:"$(OUTDIR)/Locresde.dll" /implib:"$(OUTDIR)/Locresde.lib" 
LINK32_OBJS= \
	"$(INTDIR)/locresde.obj" \
	"$(INTDIR)/locresde.res"

"$(OUTDIR)\Locresde.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "Locresfr - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir Locresfr\Release
# PROP BASE Intermediate_Dir Locresfr\Release
# PROP BASE Target_Dir Locresfr
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 0
# PROP Output_Dir Release
# PROP Intermediate_Dir Release
# PROP Target_Dir Locresfr
OUTDIR=.\Release
INTDIR=.\Release

ALL : "$(OUTDIR)\Locresfr.dll"

CLEAN : 
	-@erase ".\Release\Locresfr.dll"
	-@erase ".\Release\locresfr.obj"
	-@erase ".\Release\locresfr.res"
	-@erase ".\Release\Locresfr.lib"
	-@erase ".\Release\Locresfr.exp"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /YX /c
CPP_PROJ=/nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/Locresfr.pch" /YX /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\Release/
CPP_SBRS=
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/locresfr.res" /d "NDEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/Locresfr.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 /nologo /subsystem:windows /dll /machine:I386
LINK32_FLAGS=/nologo /subsystem:windows /dll /incremental:no\
 /pdb:"$(OUTDIR)/Locresfr.pdb" /machine:I386 /out:"$(OUTDIR)/Locresfr.dll"\
 /implib:"$(OUTDIR)/Locresfr.lib" 
LINK32_OBJS= \
	"$(INTDIR)/locresfr.obj" \
	"$(INTDIR)/locresfr.res"

"$(OUTDIR)\Locresfr.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "Locresfr - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir Locresfr\Debug
# PROP BASE Intermediate_Dir Locresfr\Debug
# PROP BASE Target_Dir Locresfr
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 1
# PROP Output_Dir Debug
# PROP Intermediate_Dir Debug
# PROP Target_Dir Locresfr
OUTDIR=.\Debug
INTDIR=.\Debug

ALL : "$(OUTDIR)\Locresfr.dll"

CLEAN : 
	-@erase ".\Debug\vc40.pdb"
	-@erase ".\Debug\Locresfr.dll"
	-@erase ".\Debug\locresfr.obj"
	-@erase ".\Debug\locresfr.res"
	-@erase ".\Debug\Locresfr.ilk"
	-@erase ".\Debug\Locresfr.lib"
	-@erase ".\Debug\Locresfr.exp"
	-@erase ".\Debug\Locresfr.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /YX /c
CPP_PROJ=/nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/Locresfr.pch" /YX /Fo"$(INTDIR)/"\
 /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\Debug/
CPP_SBRS=
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/locresfr.res" /d "_DEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/Locresfr.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386
# ADD LINK32 /nologo /subsystem:windows /dll /debug /machine:I386
LINK32_FLAGS=/nologo /subsystem:windows /dll /incremental:yes\
 /pdb:"$(OUTDIR)/Locresfr.pdb" /debug /machine:I386\
 /out:"$(OUTDIR)/Locresfr.dll" /implib:"$(OUTDIR)/Locresfr.lib" 
LINK32_OBJS= \
	"$(INTDIR)/locresfr.obj" \
	"$(INTDIR)/locresfr.res"

"$(OUTDIR)\Locresfr.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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

# Name "Localize - Win32 Release"
# Name "Localize - Win32 Debug"
# Name "Localize - Win32 Unicode Release"
# Name "Localize - Win32 Unicode Debug"

!IF  "$(CFG)" == "Localize - Win32 Release"

!ELSEIF  "$(CFG)" == "Localize - Win32 Debug"

!ELSEIF  "$(CFG)" == "Localize - Win32 Unicode Release"

!ELSEIF  "$(CFG)" == "Localize - Win32 Unicode Debug"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\stdafx.cpp

!IF  "$(CFG)" == "Localize - Win32 Release"

DEP_CPP_STDAF=\
	".\stdafx.h"\
	
# ADD BASE CPP /Yc"STDAFX.H"
# ADD CPP /Yc"STDAFX.H"

BuildCmds= \
	$(CPP) /nologo /MD /W4 /GX /Ox /D "NDEBUG" /D "_WINDOWS" /D "_AFXCTL" /D\
 "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/localize.pch" /Yc"STDAFX.H" /Fo"$(INTDIR)/"\
 /c $(SOURCE) \
	

"$(INTDIR)\stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\localize.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "Localize - Win32 Debug"

DEP_CPP_STDAF=\
	".\stdafx.h"\
	
# ADD BASE CPP /Yc"STDAFX.H"
# ADD CPP /Yc"STDAFX.H"

BuildCmds= \
	$(CPP) /nologo /MDd /W4 /Gm /GX /Zi /Od /D "_DEBUG" /D "_WINDOWS" /D "_AFXCTL"\
 /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/localize.pch" /Yc"STDAFX.H"\
 /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\localize.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "Localize - Win32 Unicode Release"

DEP_CPP_STDAF=\
	".\stdafx.h"\
	
# ADD BASE CPP /Yc"STDAFX.H"
# ADD CPP /Yc"STDAFX.H"

BuildCmds= \
	$(CPP) /nologo /MD /W4 /GX /Ox /D "NDEBUG" /D "_UNICODE" /D "_WINDOWS" /D\
 "_AFXCTL" /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/localize.pch" /Yc"STDAFX.H"\
 /Fo"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\localize.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "Localize - Win32 Unicode Debug"

DEP_CPP_STDAF=\
	".\stdafx.h"\
	
# ADD BASE CPP /Yc"STDAFX.H"
# ADD CPP /Yc"STDAFX.H"

BuildCmds= \
	$(CPP) /nologo /MDd /W4 /Gm /GX /Zi /Od /D "_DEBUG" /D "_UNICODE" /D\
 "_WINDOWS" /D "_AFXCTL" /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/localize.pch"\
 /Yc"STDAFX.H" /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\localize.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\localize.cpp

!IF  "$(CFG)" == "Localize - Win32 Release"

DEP_CPP_LOCAL=\
	".\stdafx.h"\
	".\localize.h"\
	".\resource.h"\
	

"$(INTDIR)\localize.obj" : $(SOURCE) $(DEP_CPP_LOCAL) "$(INTDIR)"\
 "$(INTDIR)\localize.pch"


!ELSEIF  "$(CFG)" == "Localize - Win32 Debug"

DEP_CPP_LOCAL=\
	".\stdafx.h"\
	".\localize.h"\
	".\resource.h"\
	

"$(INTDIR)\localize.obj" : $(SOURCE) $(DEP_CPP_LOCAL) "$(INTDIR)"\
 "$(INTDIR)\localize.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_LOCAL) "$(INTDIR)"\
 "$(INTDIR)\localize.pch"


!ELSEIF  "$(CFG)" == "Localize - Win32 Unicode Release"

DEP_CPP_LOCAL=\
	".\stdafx.h"\
	".\localize.h"\
	".\resource.h"\
	

"$(INTDIR)\localize.obj" : $(SOURCE) $(DEP_CPP_LOCAL) "$(INTDIR)"\
 "$(INTDIR)\localize.pch"


!ELSEIF  "$(CFG)" == "Localize - Win32 Unicode Debug"

DEP_CPP_LOCAL=\
	".\stdafx.h"\
	".\localize.h"\
	".\resource.h"\
	

"$(INTDIR)\localize.obj" : $(SOURCE) $(DEP_CPP_LOCAL) "$(INTDIR)"\
 "$(INTDIR)\localize.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_LOCAL) "$(INTDIR)"\
 "$(INTDIR)\localize.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\localctl.cpp

!IF  "$(CFG)" == "Localize - Win32 Release"

DEP_CPP_LOCALC=\
	".\stdafx.h"\
	".\localize.h"\
	".\localctl.h"\
	".\localppg.h"\
	".\resource.h"\
	

"$(INTDIR)\localctl.obj" : $(SOURCE) $(DEP_CPP_LOCALC) "$(INTDIR)"\
 "$(INTDIR)\localize.pch"


!ELSEIF  "$(CFG)" == "Localize - Win32 Debug"

DEP_CPP_LOCALC=\
	".\localize.h"\
	".\resource.h"\
	".\localppg.h"\
	".\stdafx.h"\
	

"$(INTDIR)\localctl.obj" : $(SOURCE) $(DEP_CPP_LOCALC) "$(INTDIR)"\
 "$(INTDIR)\localize.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_LOCALC) "$(INTDIR)"\
 "$(INTDIR)\localize.pch"


!ELSEIF  "$(CFG)" == "Localize - Win32 Unicode Release"

DEP_CPP_LOCALC=\
	".\stdafx.h"\
	".\localize.h"\
	".\localctl.h"\
	".\localppg.h"\
	".\resource.h"\
	

"$(INTDIR)\localctl.obj" : $(SOURCE) $(DEP_CPP_LOCALC) "$(INTDIR)"\
 "$(INTDIR)\localize.pch"


!ELSEIF  "$(CFG)" == "Localize - Win32 Unicode Debug"

DEP_CPP_LOCALC=\
	".\localize.h"\
	".\resource.h"\
	".\localppg.h"\
	".\stdafx.h"\
	

"$(INTDIR)\localctl.obj" : $(SOURCE) $(DEP_CPP_LOCALC) "$(INTDIR)"\
 "$(INTDIR)\localize.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_LOCALC) "$(INTDIR)"\
 "$(INTDIR)\localize.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\localppg.cpp

!IF  "$(CFG)" == "Localize - Win32 Release"

DEP_CPP_LOCALP=\
	".\stdafx.h"\
	".\localize.h"\
	".\localppg.h"\
	".\resource.h"\
	

"$(INTDIR)\localppg.obj" : $(SOURCE) $(DEP_CPP_LOCALP) "$(INTDIR)"\
 "$(INTDIR)\localize.pch"


!ELSEIF  "$(CFG)" == "Localize - Win32 Debug"

DEP_CPP_LOCALP=\
	".\stdafx.h"\
	".\localize.h"\
	".\localppg.h"\
	".\resource.h"\
	

"$(INTDIR)\localppg.obj" : $(SOURCE) $(DEP_CPP_LOCALP) "$(INTDIR)"\
 "$(INTDIR)\localize.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_LOCALP) "$(INTDIR)"\
 "$(INTDIR)\localize.pch"


!ELSEIF  "$(CFG)" == "Localize - Win32 Unicode Release"

DEP_CPP_LOCALP=\
	".\stdafx.h"\
	".\localize.h"\
	".\localppg.h"\
	".\resource.h"\
	

"$(INTDIR)\localppg.obj" : $(SOURCE) $(DEP_CPP_LOCALP) "$(INTDIR)"\
 "$(INTDIR)\localize.pch"


!ELSEIF  "$(CFG)" == "Localize - Win32 Unicode Debug"

DEP_CPP_LOCALP=\
	".\stdafx.h"\
	".\localize.h"\
	".\localppg.h"\
	".\resource.h"\
	

"$(INTDIR)\localppg.obj" : $(SOURCE) $(DEP_CPP_LOCALP) "$(INTDIR)"\
 "$(INTDIR)\localize.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_LOCALP) "$(INTDIR)"\
 "$(INTDIR)\localize.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\localize.def

!IF  "$(CFG)" == "Localize - Win32 Release"

!ELSEIF  "$(CFG)" == "Localize - Win32 Debug"

!ELSEIF  "$(CFG)" == "Localize - Win32 Unicode Release"

!ELSEIF  "$(CFG)" == "Localize - Win32 Unicode Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\localize.rc

!IF  "$(CFG)" == "Localize - Win32 Release"

DEP_RSC_LOCALI=\
	".\localize.ico"\
	".\localctl.bmp"\
	".\resource.h"\
	".\localize.rc2"\
	".\Release\localize.tlb"\
	

"$(INTDIR)\localize.res" : $(SOURCE) $(DEP_RSC_LOCALI) "$(INTDIR)"\
 "$(INTDIR)\localize.tlb"
   $(RSC) /l 0x409 /fo"$(INTDIR)/localize.res" /i "$(OUTDIR)" /i "Release" /d\
 "NDEBUG" /d "_WIN32" /d "_AFXDLL" $(SOURCE)


!ELSEIF  "$(CFG)" == "Localize - Win32 Debug"

DEP_RSC_LOCALI=\
	".\Debug\localize.tlb"\
	".\localize.ico"\
	".\localctl.bmp"\
	".\resource.h"\
	".\localize.rc2"\
	

"$(INTDIR)\localize.res" : $(SOURCE) $(DEP_RSC_LOCALI) "$(INTDIR)"\
 "$(INTDIR)\localize.tlb"
   $(RSC) /l 0x409 /fo"$(INTDIR)/localize.res" /i "$(OUTDIR)" /i "Debug" /d\
 "_DEBUG" /d "_WIN32" /d "_AFXDLL" $(SOURCE)


!ELSEIF  "$(CFG)" == "Localize - Win32 Unicode Release"

DEP_RSC_LOCALI=\
	".\UniRelease\localize.tlb"\
	".\localize.ico"\
	".\localctl.bmp"\
	".\resource.h"\
	".\localize.rc2"\
	

"$(INTDIR)\localize.res" : $(SOURCE) $(DEP_RSC_LOCALI) "$(INTDIR)"\
 "$(INTDIR)\localize.tlb"
   $(RSC) /l 0x409 /fo"$(INTDIR)/localize.res" /i "$(OUTDIR)" /i "UniRelease"\
 /d "NDEBUG" /d "_WIN32" /d "_UNICODE" /d "_AFXDLL" $(SOURCE)


!ELSEIF  "$(CFG)" == "Localize - Win32 Unicode Debug"

DEP_RSC_LOCALI=\
	".\UniDebug\localize.tlb"\
	".\localize.ico"\
	".\localctl.bmp"\
	".\resource.h"\
	".\localize.rc2"\
	

"$(INTDIR)\localize.res" : $(SOURCE) $(DEP_RSC_LOCALI) "$(INTDIR)"\
 "$(INTDIR)\localize.tlb"
   $(RSC) /l 0x409 /fo"$(INTDIR)/localize.res" /i "$(OUTDIR)" /i "UniDebug" /d\
 "_DEBUG" /d "_WIN32" /d "_UNICODE" /d "_AFXDLL" $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\localize.odl

!IF  "$(CFG)" == "Localize - Win32 Release"


"$(OUTDIR)\localize.tlb" : $(SOURCE) "$(OUTDIR)"
   $(MTL) /nologo /D "NDEBUG" /D "_WIN32" /tlb "$(OUTDIR)/localize.tlb" /win32\
 $(SOURCE)


!ELSEIF  "$(CFG)" == "Localize - Win32 Debug"


"$(OUTDIR)\localize.tlb" : $(SOURCE) "$(OUTDIR)"
   $(MTL) /nologo /D "_DEBUG" /D "_WIN32" /tlb "$(OUTDIR)/localize.tlb" /win32\
 $(SOURCE)


!ELSEIF  "$(CFG)" == "Localize - Win32 Unicode Release"


"$(OUTDIR)\localize.tlb" : $(SOURCE) "$(OUTDIR)"
   $(MTL) /nologo /D "NDEBUG" /D "_UNICODE" /D "_WIN32" /tlb\
 "$(OUTDIR)/localize.tlb" /win32 $(SOURCE)


!ELSEIF  "$(CFG)" == "Localize - Win32 Unicode Debug"


"$(OUTDIR)\localize.tlb" : $(SOURCE) "$(OUTDIR)"
   $(MTL) /nologo /D "_DEBUG" /D "_UNICODE" /D "_WIN32" /tlb\
 "$(OUTDIR)/localize.tlb" /win32 $(SOURCE)


!ENDIF 

# End Source File
# End Target
################################################################################
# Begin Target

# Name "Locresde - Win32 Release"
# Name "Locresde - Win32 Debug"

!IF  "$(CFG)" == "Locresde - Win32 Release"

!ELSEIF  "$(CFG)" == "Locresde - Win32 Debug"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\locresde.cpp

!IF  "$(CFG)" == "Locresde - Win32 Release"


"$(INTDIR)\locresde.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Locresde - Win32 Debug"


"$(INTDIR)\locresde.obj" : $(SOURCE) "$(INTDIR)"

"$(INTDIR)\vc40.pdb" : $(SOURCE) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\locresde.rc

!IF  "$(CFG)" == "Locresde - Win32 Release"

DEP_RSC_LOCRE=\
	".\resource.h"\
	

"$(INTDIR)\locresde.res" : $(SOURCE) $(DEP_RSC_LOCRE) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Locresde - Win32 Debug"

DEP_RSC_LOCRE=\
	".\resource.h"\
	

"$(INTDIR)\locresde.res" : $(SOURCE) $(DEP_RSC_LOCRE) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ENDIF 

# End Source File
# End Target
################################################################################
# Begin Target

# Name "Locresfr - Win32 Release"
# Name "Locresfr - Win32 Debug"

!IF  "$(CFG)" == "Locresfr - Win32 Release"

!ELSEIF  "$(CFG)" == "Locresfr - Win32 Debug"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\locresfr.cpp

!IF  "$(CFG)" == "Locresfr - Win32 Release"


"$(INTDIR)\locresfr.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Locresfr - Win32 Debug"


"$(INTDIR)\locresfr.obj" : $(SOURCE) "$(INTDIR)"

"$(INTDIR)\vc40.pdb" : $(SOURCE) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\locresfr.rc

!IF  "$(CFG)" == "Locresfr - Win32 Release"

DEP_RSC_LOCRES=\
	".\resource.h"\
	

"$(INTDIR)\locresfr.res" : $(SOURCE) $(DEP_RSC_LOCRES) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Locresfr - Win32 Debug"

DEP_RSC_LOCRES=\
	".\resource.h"\
	

"$(INTDIR)\locresfr.res" : $(SOURCE) $(DEP_RSC_LOCRES) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ENDIF 

# End Source File
# End Target
# End Project
################################################################################
