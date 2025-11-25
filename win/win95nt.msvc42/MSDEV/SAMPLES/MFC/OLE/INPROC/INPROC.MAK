# Microsoft Visual C++ Generated NMAKE File, Format Version 40000
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101
# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

!IF "$(CFG)" == ""
CFG=InProc - Win32 Debug
!MESSAGE No configuration specified.  Defaulting to InProc - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "InProc - Win32 Release" && "$(CFG)" != "InProc - Win32 Debug"\
 && "$(CFG)" != "InProc - Win32 Unicode Release" && "$(CFG)" !=\
 "InProc - Win32 Unicode Debug" && "$(CFG)" != "InProc - Win32 Dll Release" &&\
 "$(CFG)" != "InProc - Win32 Dll Debug" && "$(CFG)" !=\
 "InProc - Win32 Unicode Dll Release" && "$(CFG)" !=\
 "InProc - Win32 Unicode Dll Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "InProc.mak" CFG="InProc - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "InProc - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "InProc - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE "InProc - Win32 Unicode Release" (based on "Win32 (x86) Application")
!MESSAGE "InProc - Win32 Unicode Debug" (based on "Win32 (x86) Application")
!MESSAGE "InProc - Win32 Dll Release" (based on\
 "Win32 (x86) Dynamic-Link Library")
!MESSAGE "InProc - Win32 Dll Debug" (based on\
 "Win32 (x86) Dynamic-Link Library")
!MESSAGE "InProc - Win32 Unicode Dll Release" (based on\
 "Win32 (x86) Dynamic-Link Library")
!MESSAGE "InProc - Win32 Unicode Dll Debug" (based on\
 "Win32 (x86) Dynamic-Link Library")
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
# PROP Target_Last_Scanned "InProc - Win32 Debug"

!IF  "$(CFG)" == "InProc - Win32 Release"

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

ALL : "$(OUTDIR)\InProc.exe" "$(OUTDIR)\inproc.tlb" "$(OUTDIR)\InProc.bsc"

CLEAN : 
	-@erase ".\Release\InProc.idb"
	-@erase ".\Release\InProc.pch"
	-@erase ".\Release\inproc.tlb"
	-@erase ".\Release\InProc.exe"
	-@erase ".\Release\strcoll.obj"
	-@erase ".\Release\enumvar.obj"
	-@erase ".\Release\stdafx.obj"
	-@erase ".\Release\varassoc.obj"
	-@erase ".\Release\varmap.obj"
	-@erase ".\Release\inproc.obj"
	-@erase ".\Release\inproc.res"
	-@erase ".\Release\InProc.bsc"
	-@erase ".\Release\varmap.sbr"
	-@erase ".\Release\inproc.sbr"
	-@erase ".\Release\stdafx.sbr"
	-@erase ".\Release\strcoll.sbr"
	-@erase ".\Release\varassoc.sbr"
	-@erase ".\Release\enumvar.sbr"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /Yu"Stdafx.h" /c
CPP_PROJ=/nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /FR"$(INTDIR)/" /Fp"$(INTDIR)/InProc.pch" /Yu"Stdafx.h"\
 /Fo"$(INTDIR)/" /Fd"$(OUTDIR)/InProc.pdb" /c 
CPP_OBJS=.\Release/
CPP_SBRS=.\Release/

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

MTL=mktyplib.exe
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
RSC=rc.exe
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/inproc.res" /d "NDEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/InProc.bsc" 
BSC32_SBRS= \
	"$(INTDIR)/varmap.sbr" \
	"$(INTDIR)/inproc.sbr" \
	"$(INTDIR)/stdafx.sbr" \
	"$(INTDIR)/strcoll.sbr" \
	"$(INTDIR)/varassoc.sbr" \
	"$(INTDIR)/enumvar.sbr"

"$(OUTDIR)\InProc.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 /nologo /subsystem:windows /machine:I386
LINK32_FLAGS=/nologo /subsystem:windows /incremental:no\
 /pdb:"$(OUTDIR)/InProc.pdb" /machine:I386 /out:"$(OUTDIR)/InProc.exe" 
LINK32_OBJS= \
	"$(INTDIR)/strcoll.obj" \
	"$(INTDIR)/enumvar.obj" \
	"$(INTDIR)/stdafx.obj" \
	"$(INTDIR)/varassoc.obj" \
	"$(INTDIR)/varmap.obj" \
	"$(INTDIR)/inproc.obj" \
	"$(INTDIR)/inproc.res"

"$(OUTDIR)\InProc.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "InProc - Win32 Debug"

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

ALL : "$(OUTDIR)\InProc.exe" "$(OUTDIR)\inproc.tlb" "$(OUTDIR)\InProc.bsc"

CLEAN : 
	-@erase ".\Debug\InProc.pdb"
	-@erase ".\Debug\stdafx.obj"
	-@erase ".\Debug\varassoc.obj"
	-@erase ".\Debug\InProc.pch"
	-@erase ".\Debug\varmap.obj"
	-@erase ".\Debug\strcoll.obj"
	-@erase ".\Debug\enumvar.obj"
	-@erase ".\Debug\inproc.obj"
	-@erase ".\Debug\inproc.res"
	-@erase ".\Debug\InProc.idb"
	-@erase ".\Debug\inproc.tlb"
	-@erase ".\Debug\InProc.exe"
	-@erase ".\Debug\InProc.ilk"
	-@erase ".\Debug\InProc.bsc"
	-@erase ".\Debug\varmap.sbr"
	-@erase ".\Debug\inproc.sbr"
	-@erase ".\Debug\stdafx.sbr"
	-@erase ".\Debug\strcoll.sbr"
	-@erase ".\Debug\varassoc.sbr"
	-@erase ".\Debug\enumvar.sbr"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /Yu"Stdafx.h" /c
CPP_PROJ=/nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /D "_AFXDLL" /D "_MBCS" /FR"$(INTDIR)/" /Fp"$(INTDIR)/InProc.pch" /Yu"Stdafx.h"\
 /Fo"$(INTDIR)/" /Fd"$(OUTDIR)/InProc.pdb" /c 
CPP_OBJS=.\Debug/
CPP_SBRS=.\Debug/

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

MTL=mktyplib.exe
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
RSC=rc.exe
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/inproc.res" /d "_DEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/InProc.bsc" 
BSC32_SBRS= \
	"$(INTDIR)/varmap.sbr" \
	"$(INTDIR)/inproc.sbr" \
	"$(INTDIR)/stdafx.sbr" \
	"$(INTDIR)/strcoll.sbr" \
	"$(INTDIR)/varassoc.sbr" \
	"$(INTDIR)/enumvar.sbr"

"$(OUTDIR)\InProc.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386
# ADD LINK32 /nologo /subsystem:windows /debug /machine:I386
LINK32_FLAGS=/nologo /subsystem:windows /incremental:yes\
 /pdb:"$(OUTDIR)/InProc.pdb" /debug /machine:I386 /out:"$(OUTDIR)/InProc.exe" 
LINK32_OBJS= \
	"$(INTDIR)/stdafx.obj" \
	"$(INTDIR)/varassoc.obj" \
	"$(INTDIR)/varmap.obj" \
	"$(INTDIR)/strcoll.obj" \
	"$(INTDIR)/enumvar.obj" \
	"$(INTDIR)/inproc.obj" \
	"$(INTDIR)/inproc.res"

"$(OUTDIR)\InProc.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "InProc - Win32 Unicode Release"

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

ALL : "$(OUTDIR)\InProc.exe" "$(OUTDIR)\inproc.tlb" "$(OUTDIR)\InProc.bsc"

CLEAN : 
	-@erase ".\UniRelease\InProc.idb"
	-@erase ".\UniRelease\InProc.pch"
	-@erase ".\UniRelease\inproc.tlb"
	-@erase ".\UniRelease\InProc.exe"
	-@erase ".\UniRelease\inproc.obj"
	-@erase ".\UniRelease\stdafx.obj"
	-@erase ".\UniRelease\varmap.obj"
	-@erase ".\UniRelease\varassoc.obj"
	-@erase ".\UniRelease\strcoll.obj"
	-@erase ".\UniRelease\enumvar.obj"
	-@erase ".\UniRelease\inproc.res"
	-@erase ".\UniRelease\InProc.bsc"
	-@erase ".\UniRelease\varmap.sbr"
	-@erase ".\UniRelease\inproc.sbr"
	-@erase ".\UniRelease\stdafx.sbr"
	-@erase ".\UniRelease\strcoll.sbr"
	-@erase ".\UniRelease\varassoc.sbr"
	-@erase ".\UniRelease\enumvar.sbr"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_UNICODE" /D "_AFXDLL" /D "_MBCS" /FR /Yu"Stdafx.h" /c
CPP_PROJ=/nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D\
 "_UNICODE" /D "_AFXDLL" /D "_MBCS" /FR"$(INTDIR)/" /Fp"$(INTDIR)/InProc.pch"\
 /Yu"Stdafx.h" /Fo"$(INTDIR)/" /Fd"$(OUTDIR)/InProc.pdb" /c 
CPP_OBJS=.\UniRelease/
CPP_SBRS=.\UniRelease/

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

MTL=mktyplib.exe
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
RSC=rc.exe
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/inproc.res" /d "NDEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/InProc.bsc" 
BSC32_SBRS= \
	"$(INTDIR)/varmap.sbr" \
	"$(INTDIR)/inproc.sbr" \
	"$(INTDIR)/stdafx.sbr" \
	"$(INTDIR)/strcoll.sbr" \
	"$(INTDIR)/varassoc.sbr" \
	"$(INTDIR)/enumvar.sbr"

"$(OUTDIR)\InProc.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 /nologo /entry:"wWinMainCRTStartup" /subsystem:windows /machine:I386
LINK32_FLAGS=/nologo /entry:"wWinMainCRTStartup" /subsystem:windows\
 /incremental:no /pdb:"$(OUTDIR)/InProc.pdb" /machine:I386\
 /out:"$(OUTDIR)/InProc.exe" 
LINK32_OBJS= \
	"$(INTDIR)/inproc.obj" \
	"$(INTDIR)/stdafx.obj" \
	"$(INTDIR)/varmap.obj" \
	"$(INTDIR)/varassoc.obj" \
	"$(INTDIR)/strcoll.obj" \
	"$(INTDIR)/enumvar.obj" \
	"$(INTDIR)/inproc.res"

"$(OUTDIR)\InProc.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "InProc - Win32 Unicode Debug"

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

ALL : "$(OUTDIR)\InProc.exe" "$(OUTDIR)\inproc.tlb" "$(OUTDIR)\InProc.bsc"

CLEAN : 
	-@erase ".\UniDebug\InProc.pdb"
	-@erase ".\UniDebug\enumvar.obj"
	-@erase ".\UniDebug\InProc.pch"
	-@erase ".\UniDebug\varassoc.obj"
	-@erase ".\UniDebug\stdafx.obj"
	-@erase ".\UniDebug\varmap.obj"
	-@erase ".\UniDebug\inproc.obj"
	-@erase ".\UniDebug\strcoll.obj"
	-@erase ".\UniDebug\inproc.res"
	-@erase ".\UniDebug\InProc.idb"
	-@erase ".\UniDebug\inproc.tlb"
	-@erase ".\UniDebug\InProc.exe"
	-@erase ".\UniDebug\InProc.ilk"
	-@erase ".\UniDebug\InProc.bsc"
	-@erase ".\UniDebug\varmap.sbr"
	-@erase ".\UniDebug\inproc.sbr"
	-@erase ".\UniDebug\stdafx.sbr"
	-@erase ".\UniDebug\strcoll.sbr"
	-@erase ".\UniDebug\varassoc.sbr"
	-@erase ".\UniDebug\enumvar.sbr"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_UNICODE" /D "_AFXDLL" /D "_MBCS" /FR /Yu"Stdafx.h" /c
CPP_PROJ=/nologo /MDd /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS"\
 /D "_UNICODE" /D "_AFXDLL" /D "_MBCS" /FR"$(INTDIR)/" /Fp"$(INTDIR)/InProc.pch"\
 /Yu"Stdafx.h" /Fo"$(INTDIR)/" /Fd"$(OUTDIR)/InProc.pdb" /c 
CPP_OBJS=.\UniDebug/
CPP_SBRS=.\UniDebug/

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

MTL=mktyplib.exe
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
RSC=rc.exe
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/inproc.res" /d "_DEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/InProc.bsc" 
BSC32_SBRS= \
	"$(INTDIR)/varmap.sbr" \
	"$(INTDIR)/inproc.sbr" \
	"$(INTDIR)/stdafx.sbr" \
	"$(INTDIR)/strcoll.sbr" \
	"$(INTDIR)/varassoc.sbr" \
	"$(INTDIR)/enumvar.sbr"

"$(OUTDIR)\InProc.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386
# ADD LINK32 /nologo /entry:"wWinMainCRTStartup" /subsystem:windows /debug /machine:I386
LINK32_FLAGS=/nologo /entry:"wWinMainCRTStartup" /subsystem:windows\
 /incremental:yes /pdb:"$(OUTDIR)/InProc.pdb" /debug /machine:I386\
 /out:"$(OUTDIR)/InProc.exe" 
LINK32_OBJS= \
	"$(INTDIR)/enumvar.obj" \
	"$(INTDIR)/varassoc.obj" \
	"$(INTDIR)/stdafx.obj" \
	"$(INTDIR)/varmap.obj" \
	"$(INTDIR)/inproc.obj" \
	"$(INTDIR)/strcoll.obj" \
	"$(INTDIR)/inproc.res"

"$(OUTDIR)\InProc.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "InProc - Win32 Dll Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir DllRelease
# PROP BASE Intermediate_Dir DllRelease
# PROP BASE Target_Dir
# PROP Use_MFC 1
# PROP Use_Debug_Libraries 0
# PROP Output_Dir DllRelease
# PROP Intermediate_Dir DllRelease
# PROP Target_Dir
OUTDIR=.\DllRelease
INTDIR=.\DllRelease

ALL : "$(OUTDIR)\InProc.dll" "$(OUTDIR)\inproc.tlb" "$(OUTDIR)\InProc.bsc"

CLEAN : 
	-@erase ".\DllRelease\InProc.idb"
	-@erase ".\DllRelease\InProc.pch"
	-@erase ".\DllRelease\inproc.tlb"
	-@erase ".\DllRelease\InProc.dll"
	-@erase ".\DllRelease\stdafx.obj"
	-@erase ".\DllRelease\varmap.obj"
	-@erase ".\DllRelease\varassoc.obj"
	-@erase ".\DllRelease\strcoll.obj"
	-@erase ".\DllRelease\enumvar.obj"
	-@erase ".\DllRelease\inproc.obj"
	-@erase ".\DllRelease\inproc.res"
	-@erase ".\DllRelease\InProc.lib"
	-@erase ".\DllRelease\InProc.exp"
	-@erase ".\DllRelease\InProc.bsc"
	-@erase ".\DllRelease\varmap.sbr"
	-@erase ".\DllRelease\inproc.sbr"
	-@erase ".\DllRelease\stdafx.sbr"
	-@erase ".\DllRelease\strcoll.sbr"
	-@erase ".\DllRelease\varassoc.sbr"
	-@erase ".\DllRelease\enumvar.sbr"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_USRDLL" /D "_WINDLL" /D "_MBCS" /FR /Yu"Stdafx.h" /c
CPP_PROJ=/nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_USRDLL" /D "_WINDLL" /D "_MBCS" /FR"$(INTDIR)/" /Fp"$(INTDIR)/InProc.pch"\
 /Yu"Stdafx.h" /Fo"$(INTDIR)/" /Fd"$(OUTDIR)/InProc.pdb" /c 
CPP_OBJS=.\DllRelease/
CPP_SBRS=.\DllRelease/

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

MTL=mktyplib.exe
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
RSC=rc.exe
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/inproc.res" /d "NDEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/InProc.bsc" 
BSC32_SBRS= \
	"$(INTDIR)/varmap.sbr" \
	"$(INTDIR)/inproc.sbr" \
	"$(INTDIR)/stdafx.sbr" \
	"$(INTDIR)/strcoll.sbr" \
	"$(INTDIR)/varassoc.sbr" \
	"$(INTDIR)/enumvar.sbr"

"$(OUTDIR)\InProc.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 /nologo /subsystem:windows /dll /machine:I386
# SUBTRACT LINK32 /pdb:none /nodefaultlib
LINK32_FLAGS=/nologo /subsystem:windows /dll /incremental:no\
 /pdb:"$(OUTDIR)/InProc.pdb" /machine:I386 /def:".\inproc.def"\
 /out:"$(OUTDIR)/InProc.dll" /implib:"$(OUTDIR)/InProc.lib" 
DEF_FILE= \
	".\inproc.def"
LINK32_OBJS= \
	"$(INTDIR)/stdafx.obj" \
	"$(INTDIR)/varmap.obj" \
	"$(INTDIR)/varassoc.obj" \
	"$(INTDIR)/strcoll.obj" \
	"$(INTDIR)/enumvar.obj" \
	"$(INTDIR)/inproc.obj" \
	"$(INTDIR)/inproc.res"

"$(OUTDIR)\InProc.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "InProc - Win32 Dll Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir DllDebug
# PROP BASE Intermediate_Dir DllDebug
# PROP BASE Target_Dir
# PROP Use_MFC 1
# PROP Use_Debug_Libraries 1
# PROP Output_Dir DllDebug
# PROP Intermediate_Dir DllDebug
# PROP Target_Dir
OUTDIR=.\DllDebug
INTDIR=.\DllDebug

ALL : "$(OUTDIR)\InProc.dll" "$(OUTDIR)\inproc.tlb" "$(OUTDIR)\InProc.bsc"

CLEAN : 
	-@erase ".\DllDebug\InProc.pdb"
	-@erase ".\DllDebug\varassoc.obj"
	-@erase ".\DllDebug\InProc.pch"
	-@erase ".\DllDebug\inproc.obj"
	-@erase ".\DllDebug\strcoll.obj"
	-@erase ".\DllDebug\enumvar.obj"
	-@erase ".\DllDebug\stdafx.obj"
	-@erase ".\DllDebug\varmap.obj"
	-@erase ".\DllDebug\inproc.res"
	-@erase ".\DllDebug\InProc.idb"
	-@erase ".\DllDebug\inproc.tlb"
	-@erase ".\DllDebug\InProc.dll"
	-@erase ".\DllDebug\InProc.ilk"
	-@erase ".\DllDebug\InProc.lib"
	-@erase ".\DllDebug\InProc.exp"
	-@erase ".\DllDebug\InProc.bsc"
	-@erase ".\DllDebug\varmap.sbr"
	-@erase ".\DllDebug\inproc.sbr"
	-@erase ".\DllDebug\stdafx.sbr"
	-@erase ".\DllDebug\strcoll.sbr"
	-@erase ".\DllDebug\varassoc.sbr"
	-@erase ".\DllDebug\enumvar.sbr"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_USRDLL" /D "_WINDLL" /D "_MBCS" /FR /Yu"Stdafx.h" /c
CPP_PROJ=/nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /D "_USRDLL" /D "_WINDLL" /D "_MBCS" /FR"$(INTDIR)/" /Fp"$(INTDIR)/InProc.pch"\
 /Yu"Stdafx.h" /Fo"$(INTDIR)/" /Fd"$(OUTDIR)/InProc.pdb" /c 
CPP_OBJS=.\DllDebug/
CPP_SBRS=.\DllDebug/

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

MTL=mktyplib.exe
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
RSC=rc.exe
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/inproc.res" /d "_DEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/InProc.bsc" 
BSC32_SBRS= \
	"$(INTDIR)/varmap.sbr" \
	"$(INTDIR)/inproc.sbr" \
	"$(INTDIR)/stdafx.sbr" \
	"$(INTDIR)/strcoll.sbr" \
	"$(INTDIR)/varassoc.sbr" \
	"$(INTDIR)/enumvar.sbr"

"$(OUTDIR)\InProc.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386
# ADD LINK32 /nologo /subsystem:windows /dll /debug /machine:I386
LINK32_FLAGS=/nologo /subsystem:windows /dll /incremental:yes\
 /pdb:"$(OUTDIR)/InProc.pdb" /debug /machine:I386 /def:".\inproc.def"\
 /out:"$(OUTDIR)/InProc.dll" /implib:"$(OUTDIR)/InProc.lib" 
DEF_FILE= \
	".\inproc.def"
LINK32_OBJS= \
	"$(INTDIR)/varassoc.obj" \
	"$(INTDIR)/inproc.obj" \
	"$(INTDIR)/strcoll.obj" \
	"$(INTDIR)/enumvar.obj" \
	"$(INTDIR)/stdafx.obj" \
	"$(INTDIR)/varmap.obj" \
	"$(INTDIR)/inproc.res"

"$(OUTDIR)\InProc.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "InProc - Win32 Unicode Dll Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir UniDllRelease
# PROP BASE Intermediate_Dir UniDllRelease
# PROP BASE Target_Dir
# PROP Use_MFC 1
# PROP Use_Debug_Libraries 0
# PROP Output_Dir UniDllRelease
# PROP Intermediate_Dir UniDllRelease
# PROP Target_Dir
OUTDIR=.\UniDllRelease
INTDIR=.\UniDllRelease

ALL : "$(OUTDIR)\InProc.dll" "$(OUTDIR)\inproc.tlb" "$(OUTDIR)\InProc.bsc"

CLEAN : 
	-@erase ".\UniDllRelease\InProc.idb"
	-@erase ".\UniDllRelease\InProc.pch"
	-@erase ".\UniDllRelease\inproc.tlb"
	-@erase ".\UniDllRelease\InProc.dll"
	-@erase ".\UniDllRelease\stdafx.obj"
	-@erase ".\UniDllRelease\varmap.obj"
	-@erase ".\UniDllRelease\strcoll.obj"
	-@erase ".\UniDllRelease\enumvar.obj"
	-@erase ".\UniDllRelease\inproc.obj"
	-@erase ".\UniDllRelease\varassoc.obj"
	-@erase ".\UniDllRelease\inproc.res"
	-@erase ".\UniDllRelease\InProc.lib"
	-@erase ".\UniDllRelease\InProc.exp"
	-@erase ".\UniDllRelease\InProc.bsc"
	-@erase ".\UniDllRelease\varmap.sbr"
	-@erase ".\UniDllRelease\inproc.sbr"
	-@erase ".\UniDllRelease\stdafx.sbr"
	-@erase ".\UniDllRelease\strcoll.sbr"
	-@erase ".\UniDllRelease\varassoc.sbr"
	-@erase ".\UniDllRelease\enumvar.sbr"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MT /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_UNICODE" /D "_USRDLL" /D "_WINDLL" /D "_MBCS" /FR /Yu"Stdafx.h" /c
CPP_PROJ=/nologo /MT /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D\
 "_UNICODE" /D "_USRDLL" /D "_WINDLL" /D "_MBCS" /FR"$(INTDIR)/"\
 /Fp"$(INTDIR)/InProc.pch" /Yu"Stdafx.h" /Fo"$(INTDIR)/"\
 /Fd"$(OUTDIR)/InProc.pdb" /c 
CPP_OBJS=.\UniDllRelease/
CPP_SBRS=.\UniDllRelease/

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

MTL=mktyplib.exe
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
RSC=rc.exe
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/inproc.res" /d "NDEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/InProc.bsc" 
BSC32_SBRS= \
	"$(INTDIR)/varmap.sbr" \
	"$(INTDIR)/inproc.sbr" \
	"$(INTDIR)/stdafx.sbr" \
	"$(INTDIR)/strcoll.sbr" \
	"$(INTDIR)/varassoc.sbr" \
	"$(INTDIR)/enumvar.sbr"

"$(OUTDIR)\InProc.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 /nologo /subsystem:windows /dll /machine:I386
LINK32_FLAGS=/nologo /subsystem:windows /dll /incremental:no\
 /pdb:"$(OUTDIR)/InProc.pdb" /machine:I386 /def:".\inproc.def"\
 /out:"$(OUTDIR)/InProc.dll" /implib:"$(OUTDIR)/InProc.lib" 
DEF_FILE= \
	".\inproc.def"
LINK32_OBJS= \
	"$(INTDIR)/stdafx.obj" \
	"$(INTDIR)/varmap.obj" \
	"$(INTDIR)/strcoll.obj" \
	"$(INTDIR)/enumvar.obj" \
	"$(INTDIR)/inproc.obj" \
	"$(INTDIR)/varassoc.obj" \
	"$(INTDIR)/inproc.res"

"$(OUTDIR)\InProc.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "InProc - Win32 Unicode Dll Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir UniDllDebug
# PROP BASE Intermediate_Dir UniDllDebug
# PROP BASE Target_Dir
# PROP Use_MFC 1
# PROP Use_Debug_Libraries 1
# PROP Output_Dir UniDllDebug
# PROP Intermediate_Dir UniDllDebug
# PROP Target_Dir
OUTDIR=.\UniDllDebug
INTDIR=.\UniDllDebug

ALL : "$(OUTDIR)\InProc.dll" "$(OUTDIR)\inproc.tlb" "$(OUTDIR)\InProc.bsc"

CLEAN : 
	-@erase ".\UniDllDebug\InProc.pdb"
	-@erase ".\UniDllDebug\varassoc.obj"
	-@erase ".\UniDllDebug\InProc.pch"
	-@erase ".\UniDllDebug\stdafx.obj"
	-@erase ".\UniDllDebug\varmap.obj"
	-@erase ".\UniDllDebug\strcoll.obj"
	-@erase ".\UniDllDebug\enumvar.obj"
	-@erase ".\UniDllDebug\inproc.obj"
	-@erase ".\UniDllDebug\inproc.res"
	-@erase ".\UniDllDebug\InProc.idb"
	-@erase ".\UniDllDebug\inproc.tlb"
	-@erase ".\UniDllDebug\InProc.dll"
	-@erase ".\UniDllDebug\InProc.ilk"
	-@erase ".\UniDllDebug\InProc.lib"
	-@erase ".\UniDllDebug\InProc.exp"
	-@erase ".\UniDllDebug\InProc.bsc"
	-@erase ".\UniDllDebug\varmap.sbr"
	-@erase ".\UniDllDebug\inproc.sbr"
	-@erase ".\UniDllDebug\stdafx.sbr"
	-@erase ".\UniDllDebug\strcoll.sbr"
	-@erase ".\UniDllDebug\varassoc.sbr"
	-@erase ".\UniDllDebug\enumvar.sbr"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_UNICODE" /D "_USRDLL" /D "_WINDLL" /D "_MBCS" /FR /Yu"Stdafx.h" /c
CPP_PROJ=/nologo /MTd /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS"\
 /D "_UNICODE" /D "_USRDLL" /D "_WINDLL" /D "_MBCS" /FR"$(INTDIR)/"\
 /Fp"$(INTDIR)/InProc.pch" /Yu"Stdafx.h" /Fo"$(INTDIR)/"\
 /Fd"$(OUTDIR)/InProc.pdb" /c 
CPP_OBJS=.\UniDllDebug/
CPP_SBRS=.\UniDllDebug/

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

MTL=mktyplib.exe
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
RSC=rc.exe
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/inproc.res" /d "_DEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/InProc.bsc" 
BSC32_SBRS= \
	"$(INTDIR)/varmap.sbr" \
	"$(INTDIR)/inproc.sbr" \
	"$(INTDIR)/stdafx.sbr" \
	"$(INTDIR)/strcoll.sbr" \
	"$(INTDIR)/varassoc.sbr" \
	"$(INTDIR)/enumvar.sbr"

"$(OUTDIR)\InProc.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386
# ADD LINK32 /nologo /subsystem:windows /dll /debug /machine:I386
LINK32_FLAGS=/nologo /subsystem:windows /dll /incremental:yes\
 /pdb:"$(OUTDIR)/InProc.pdb" /debug /machine:I386 /def:".\inproc.def"\
 /out:"$(OUTDIR)/InProc.dll" /implib:"$(OUTDIR)/InProc.lib" 
DEF_FILE= \
	".\inproc.def"
LINK32_OBJS= \
	"$(INTDIR)/varassoc.obj" \
	"$(INTDIR)/stdafx.obj" \
	"$(INTDIR)/varmap.obj" \
	"$(INTDIR)/strcoll.obj" \
	"$(INTDIR)/enumvar.obj" \
	"$(INTDIR)/inproc.obj" \
	"$(INTDIR)/inproc.res"

"$(OUTDIR)\InProc.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 

################################################################################
# Begin Target

# Name "InProc - Win32 Release"
# Name "InProc - Win32 Debug"
# Name "InProc - Win32 Unicode Release"
# Name "InProc - Win32 Unicode Debug"
# Name "InProc - Win32 Dll Release"
# Name "InProc - Win32 Dll Debug"
# Name "InProc - Win32 Unicode Dll Release"
# Name "InProc - Win32 Unicode Dll Debug"

!IF  "$(CFG)" == "InProc - Win32 Release"

!ELSEIF  "$(CFG)" == "InProc - Win32 Debug"

!ELSEIF  "$(CFG)" == "InProc - Win32 Unicode Release"

!ELSEIF  "$(CFG)" == "InProc - Win32 Unicode Debug"

!ELSEIF  "$(CFG)" == "InProc - Win32 Dll Release"

!ELSEIF  "$(CFG)" == "InProc - Win32 Dll Debug"

!ELSEIF  "$(CFG)" == "InProc - Win32 Unicode Dll Release"

!ELSEIF  "$(CFG)" == "InProc - Win32 Unicode Dll Debug"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\varmap.cpp
DEP_CPP_VARMA=\
	".\enumvar.h"\
	".\inproc.h"\
	".\resource.h"\
	".\stdafx.h"\
	".\varassoc.h"\
	".\varmap.h"\
	

!IF  "$(CFG)" == "InProc - Win32 Release"


"$(INTDIR)\varmap.obj" : $(SOURCE) $(DEP_CPP_VARMA) "$(INTDIR)"\
 "$(INTDIR)\InProc.pch"

"$(INTDIR)\varmap.sbr" : $(SOURCE) $(DEP_CPP_VARMA) "$(INTDIR)"\
 "$(INTDIR)\InProc.pch"

"$(INTDIR)\InProc.idb" : $(SOURCE) $(DEP_CPP_VARMA) "$(INTDIR)"\
 "$(INTDIR)\InProc.pch"


!ELSEIF  "$(CFG)" == "InProc - Win32 Debug"


"$(INTDIR)\varmap.obj" : $(SOURCE) $(DEP_CPP_VARMA) "$(INTDIR)"\
 "$(INTDIR)\InProc.pch"

"$(INTDIR)\varmap.sbr" : $(SOURCE) $(DEP_CPP_VARMA) "$(INTDIR)"\
 "$(INTDIR)\InProc.pch"

"$(INTDIR)\InProc.pdb" : $(SOURCE) $(DEP_CPP_VARMA) "$(INTDIR)"\
 "$(INTDIR)\InProc.pch"

"$(INTDIR)\InProc.idb" : $(SOURCE) $(DEP_CPP_VARMA) "$(INTDIR)"\
 "$(INTDIR)\InProc.pch"


!ELSEIF  "$(CFG)" == "InProc - Win32 Unicode Release"


"$(INTDIR)\varmap.obj" : $(SOURCE) $(DEP_CPP_VARMA) "$(INTDIR)"\
 "$(INTDIR)\InProc.pch"

"$(INTDIR)\varmap.sbr" : $(SOURCE) $(DEP_CPP_VARMA) "$(INTDIR)"\
 "$(INTDIR)\InProc.pch"

"$(INTDIR)\InProc.idb" : $(SOURCE) $(DEP_CPP_VARMA) "$(INTDIR)"\
 "$(INTDIR)\InProc.pch"


!ELSEIF  "$(CFG)" == "InProc - Win32 Unicode Debug"


"$(INTDIR)\varmap.obj" : $(SOURCE) $(DEP_CPP_VARMA) "$(INTDIR)"\
 "$(INTDIR)\InProc.pch"

"$(INTDIR)\varmap.sbr" : $(SOURCE) $(DEP_CPP_VARMA) "$(INTDIR)"\
 "$(INTDIR)\InProc.pch"

"$(INTDIR)\InProc.pdb" : $(SOURCE) $(DEP_CPP_VARMA) "$(INTDIR)"\
 "$(INTDIR)\InProc.pch"

"$(INTDIR)\InProc.idb" : $(SOURCE) $(DEP_CPP_VARMA) "$(INTDIR)"\
 "$(INTDIR)\InProc.pch"


!ELSEIF  "$(CFG)" == "InProc - Win32 Dll Release"


"$(INTDIR)\varmap.obj" : $(SOURCE) $(DEP_CPP_VARMA) "$(INTDIR)"\
 "$(INTDIR)\InProc.pch"

"$(INTDIR)\varmap.sbr" : $(SOURCE) $(DEP_CPP_VARMA) "$(INTDIR)"\
 "$(INTDIR)\InProc.pch"

"$(INTDIR)\InProc.idb" : $(SOURCE) $(DEP_CPP_VARMA) "$(INTDIR)"\
 "$(INTDIR)\InProc.pch"


!ELSEIF  "$(CFG)" == "InProc - Win32 Dll Debug"


"$(INTDIR)\varmap.obj" : $(SOURCE) $(DEP_CPP_VARMA) "$(INTDIR)"\
 "$(INTDIR)\InProc.pch"

"$(INTDIR)\varmap.sbr" : $(SOURCE) $(DEP_CPP_VARMA) "$(INTDIR)"\
 "$(INTDIR)\InProc.pch"

"$(INTDIR)\InProc.pdb" : $(SOURCE) $(DEP_CPP_VARMA) "$(INTDIR)"\
 "$(INTDIR)\InProc.pch"

"$(INTDIR)\InProc.idb" : $(SOURCE) $(DEP_CPP_VARMA) "$(INTDIR)"\
 "$(INTDIR)\InProc.pch"


!ELSEIF  "$(CFG)" == "InProc - Win32 Unicode Dll Release"


"$(INTDIR)\varmap.obj" : $(SOURCE) $(DEP_CPP_VARMA) "$(INTDIR)"\
 "$(INTDIR)\InProc.pch"

"$(INTDIR)\varmap.sbr" : $(SOURCE) $(DEP_CPP_VARMA) "$(INTDIR)"\
 "$(INTDIR)\InProc.pch"

"$(INTDIR)\InProc.idb" : $(SOURCE) $(DEP_CPP_VARMA) "$(INTDIR)"\
 "$(INTDIR)\InProc.pch"


!ELSEIF  "$(CFG)" == "InProc - Win32 Unicode Dll Debug"


"$(INTDIR)\varmap.obj" : $(SOURCE) $(DEP_CPP_VARMA) "$(INTDIR)"\
 "$(INTDIR)\InProc.pch"

"$(INTDIR)\varmap.sbr" : $(SOURCE) $(DEP_CPP_VARMA) "$(INTDIR)"\
 "$(INTDIR)\InProc.pch"

"$(INTDIR)\InProc.pdb" : $(SOURCE) $(DEP_CPP_VARMA) "$(INTDIR)"\
 "$(INTDIR)\InProc.pch"

"$(INTDIR)\InProc.idb" : $(SOURCE) $(DEP_CPP_VARMA) "$(INTDIR)"\
 "$(INTDIR)\InProc.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\inproc.cpp
DEP_CPP_INPRO=\
	".\inproc.h"\
	".\resource.h"\
	".\stdafx.h"\
	

!IF  "$(CFG)" == "InProc - Win32 Release"


"$(INTDIR)\inproc.obj" : $(SOURCE) $(DEP_CPP_INPRO) "$(INTDIR)"\
 "$(INTDIR)\InProc.pch"

"$(INTDIR)\inproc.sbr" : $(SOURCE) $(DEP_CPP_INPRO) "$(INTDIR)"\
 "$(INTDIR)\InProc.pch"

"$(INTDIR)\InProc.idb" : $(SOURCE) $(DEP_CPP_INPRO) "$(INTDIR)"\
 "$(INTDIR)\InProc.pch"


!ELSEIF  "$(CFG)" == "InProc - Win32 Debug"


"$(INTDIR)\inproc.obj" : $(SOURCE) $(DEP_CPP_INPRO) "$(INTDIR)"\
 "$(INTDIR)\InProc.pch"

"$(INTDIR)\inproc.sbr" : $(SOURCE) $(DEP_CPP_INPRO) "$(INTDIR)"\
 "$(INTDIR)\InProc.pch"

"$(INTDIR)\InProc.pdb" : $(SOURCE) $(DEP_CPP_INPRO) "$(INTDIR)"\
 "$(INTDIR)\InProc.pch"

"$(INTDIR)\InProc.idb" : $(SOURCE) $(DEP_CPP_INPRO) "$(INTDIR)"\
 "$(INTDIR)\InProc.pch"


!ELSEIF  "$(CFG)" == "InProc - Win32 Unicode Release"


"$(INTDIR)\inproc.obj" : $(SOURCE) $(DEP_CPP_INPRO) "$(INTDIR)"\
 "$(INTDIR)\InProc.pch"

"$(INTDIR)\inproc.sbr" : $(SOURCE) $(DEP_CPP_INPRO) "$(INTDIR)"\
 "$(INTDIR)\InProc.pch"

"$(INTDIR)\InProc.idb" : $(SOURCE) $(DEP_CPP_INPRO) "$(INTDIR)"\
 "$(INTDIR)\InProc.pch"


!ELSEIF  "$(CFG)" == "InProc - Win32 Unicode Debug"


"$(INTDIR)\inproc.obj" : $(SOURCE) $(DEP_CPP_INPRO) "$(INTDIR)"\
 "$(INTDIR)\InProc.pch"

"$(INTDIR)\inproc.sbr" : $(SOURCE) $(DEP_CPP_INPRO) "$(INTDIR)"\
 "$(INTDIR)\InProc.pch"

"$(INTDIR)\InProc.pdb" : $(SOURCE) $(DEP_CPP_INPRO) "$(INTDIR)"\
 "$(INTDIR)\InProc.pch"

"$(INTDIR)\InProc.idb" : $(SOURCE) $(DEP_CPP_INPRO) "$(INTDIR)"\
 "$(INTDIR)\InProc.pch"


!ELSEIF  "$(CFG)" == "InProc - Win32 Dll Release"


"$(INTDIR)\inproc.obj" : $(SOURCE) $(DEP_CPP_INPRO) "$(INTDIR)"\
 "$(INTDIR)\InProc.pch"

"$(INTDIR)\inproc.sbr" : $(SOURCE) $(DEP_CPP_INPRO) "$(INTDIR)"\
 "$(INTDIR)\InProc.pch"

"$(INTDIR)\InProc.idb" : $(SOURCE) $(DEP_CPP_INPRO) "$(INTDIR)"\
 "$(INTDIR)\InProc.pch"


!ELSEIF  "$(CFG)" == "InProc - Win32 Dll Debug"


"$(INTDIR)\inproc.obj" : $(SOURCE) $(DEP_CPP_INPRO) "$(INTDIR)"\
 "$(INTDIR)\InProc.pch"

"$(INTDIR)\inproc.sbr" : $(SOURCE) $(DEP_CPP_INPRO) "$(INTDIR)"\
 "$(INTDIR)\InProc.pch"

"$(INTDIR)\InProc.pdb" : $(SOURCE) $(DEP_CPP_INPRO) "$(INTDIR)"\
 "$(INTDIR)\InProc.pch"

"$(INTDIR)\InProc.idb" : $(SOURCE) $(DEP_CPP_INPRO) "$(INTDIR)"\
 "$(INTDIR)\InProc.pch"


!ELSEIF  "$(CFG)" == "InProc - Win32 Unicode Dll Release"


"$(INTDIR)\inproc.obj" : $(SOURCE) $(DEP_CPP_INPRO) "$(INTDIR)"\
 "$(INTDIR)\InProc.pch"

"$(INTDIR)\inproc.sbr" : $(SOURCE) $(DEP_CPP_INPRO) "$(INTDIR)"\
 "$(INTDIR)\InProc.pch"

"$(INTDIR)\InProc.idb" : $(SOURCE) $(DEP_CPP_INPRO) "$(INTDIR)"\
 "$(INTDIR)\InProc.pch"


!ELSEIF  "$(CFG)" == "InProc - Win32 Unicode Dll Debug"


"$(INTDIR)\inproc.obj" : $(SOURCE) $(DEP_CPP_INPRO) "$(INTDIR)"\
 "$(INTDIR)\InProc.pch"

"$(INTDIR)\inproc.sbr" : $(SOURCE) $(DEP_CPP_INPRO) "$(INTDIR)"\
 "$(INTDIR)\InProc.pch"

"$(INTDIR)\InProc.pdb" : $(SOURCE) $(DEP_CPP_INPRO) "$(INTDIR)"\
 "$(INTDIR)\InProc.pch"

"$(INTDIR)\InProc.idb" : $(SOURCE) $(DEP_CPP_INPRO) "$(INTDIR)"\
 "$(INTDIR)\InProc.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\inproc.rc
DEP_RSC_INPROC=\
	".\res\inproc.ico"\
	".\res\inproc.rc2"\
	

!IF  "$(CFG)" == "InProc - Win32 Release"


"$(INTDIR)\inproc.res" : $(SOURCE) $(DEP_RSC_INPROC) "$(INTDIR)"
   $(RSC) /l 0x409 /fo"$(INTDIR)/inproc.res" /i "Release" /d "NDEBUG" /d\
 "_AFXDLL" $(SOURCE)


!ELSEIF  "$(CFG)" == "InProc - Win32 Debug"


"$(INTDIR)\inproc.res" : $(SOURCE) $(DEP_RSC_INPROC) "$(INTDIR)"
   $(RSC) /l 0x409 /fo"$(INTDIR)/inproc.res" /i "Debug" /d "_DEBUG" /d\
 "_AFXDLL" $(SOURCE)


!ELSEIF  "$(CFG)" == "InProc - Win32 Unicode Release"


"$(INTDIR)\inproc.res" : $(SOURCE) $(DEP_RSC_INPROC) "$(INTDIR)"
   $(RSC) /l 0x409 /fo"$(INTDIR)/inproc.res" /i "UniRelease" /d "NDEBUG" /d\
 "_AFXDLL" $(SOURCE)


!ELSEIF  "$(CFG)" == "InProc - Win32 Unicode Debug"


"$(INTDIR)\inproc.res" : $(SOURCE) $(DEP_RSC_INPROC) "$(INTDIR)"
   $(RSC) /l 0x409 /fo"$(INTDIR)/inproc.res" /i "UniDebug" /d "_DEBUG" /d\
 "_AFXDLL" $(SOURCE)


!ELSEIF  "$(CFG)" == "InProc - Win32 Dll Release"


"$(INTDIR)\inproc.res" : $(SOURCE) $(DEP_RSC_INPROC) "$(INTDIR)"
   $(RSC) /l 0x409 /fo"$(INTDIR)/inproc.res" /i "DllRelease" /d "NDEBUG"\
 $(SOURCE)


!ELSEIF  "$(CFG)" == "InProc - Win32 Dll Debug"


"$(INTDIR)\inproc.res" : $(SOURCE) $(DEP_RSC_INPROC) "$(INTDIR)"
   $(RSC) /l 0x409 /fo"$(INTDIR)/inproc.res" /i "DllDebug" /d "_DEBUG"\
 $(SOURCE)


!ELSEIF  "$(CFG)" == "InProc - Win32 Unicode Dll Release"


"$(INTDIR)\inproc.res" : $(SOURCE) $(DEP_RSC_INPROC) "$(INTDIR)"
   $(RSC) /l 0x409 /fo"$(INTDIR)/inproc.res" /i "UniDllRelease" /d "NDEBUG"\
 $(SOURCE)


!ELSEIF  "$(CFG)" == "InProc - Win32 Unicode Dll Debug"


"$(INTDIR)\inproc.res" : $(SOURCE) $(DEP_RSC_INPROC) "$(INTDIR)"
   $(RSC) /l 0x409 /fo"$(INTDIR)/inproc.res" /i "UniDllDebug" /d "_DEBUG"\
 $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\stdafx.cpp
DEP_CPP_STDAF=\
	".\stdafx.h"\
	

!IF  "$(CFG)" == "InProc - Win32 Release"

# ADD CPP /Yc

BuildCmds= \
	$(CPP) /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /FR"$(INTDIR)/" /Fp"$(INTDIR)/InProc.pch" /Yc\
 /Fo"$(INTDIR)/" /Fd"$(OUTDIR)/InProc.pdb" /c $(SOURCE) \
	

"$(INTDIR)\stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\stdafx.sbr" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\InProc.idb" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\InProc.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "InProc - Win32 Debug"

# ADD CPP /Yc"Stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /D "_AFXDLL" /D "_MBCS" /FR"$(INTDIR)/" /Fp"$(INTDIR)/InProc.pch" /Yc"Stdafx.h"\
 /Fo"$(INTDIR)/" /Fd"$(OUTDIR)/InProc.pdb" /c $(SOURCE) \
	

"$(INTDIR)\stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\stdafx.sbr" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\InProc.pdb" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\InProc.idb" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\InProc.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "InProc - Win32 Unicode Release"

# ADD CPP /Yc

BuildCmds= \
	$(CPP) /nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D\
 "_UNICODE" /D "_AFXDLL" /D "_MBCS" /FR"$(INTDIR)/" /Fp"$(INTDIR)/InProc.pch"\
 /Yc /Fo"$(INTDIR)/" /Fd"$(OUTDIR)/InProc.pdb" /c $(SOURCE) \
	

"$(INTDIR)\stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\stdafx.sbr" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\InProc.idb" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\InProc.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "InProc - Win32 Unicode Debug"

# ADD CPP /Yc

BuildCmds= \
	$(CPP) /nologo /MDd /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS"\
 /D "_UNICODE" /D "_AFXDLL" /D "_MBCS" /FR"$(INTDIR)/" /Fp"$(INTDIR)/InProc.pch"\
 /Yc /Fo"$(INTDIR)/" /Fd"$(OUTDIR)/InProc.pdb" /c $(SOURCE) \
	

"$(INTDIR)\stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\stdafx.sbr" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\InProc.pdb" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\InProc.idb" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\InProc.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "InProc - Win32 Dll Release"

# ADD CPP /Yc

BuildCmds= \
	$(CPP) /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_USRDLL" /D "_WINDLL" /D "_MBCS" /FR"$(INTDIR)/" /Fp"$(INTDIR)/InProc.pch" /Yc\
 /Fo"$(INTDIR)/" /Fd"$(OUTDIR)/InProc.pdb" /c $(SOURCE) \
	

"$(INTDIR)\stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\stdafx.sbr" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\InProc.idb" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\InProc.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "InProc - Win32 Dll Debug"

# ADD CPP /Yc

BuildCmds= \
	$(CPP) /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /D "_USRDLL" /D "_WINDLL" /D "_MBCS" /FR"$(INTDIR)/" /Fp"$(INTDIR)/InProc.pch"\
 /Yc /Fo"$(INTDIR)/" /Fd"$(OUTDIR)/InProc.pdb" /c $(SOURCE) \
	

"$(INTDIR)\stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\stdafx.sbr" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\InProc.pdb" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\InProc.idb" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\InProc.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "InProc - Win32 Unicode Dll Release"

# ADD CPP /Yc

BuildCmds= \
	$(CPP) /nologo /MT /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D\
 "_UNICODE" /D "_USRDLL" /D "_WINDLL" /D "_MBCS" /FR"$(INTDIR)/"\
 /Fp"$(INTDIR)/InProc.pch" /Yc /Fo"$(INTDIR)/" /Fd"$(OUTDIR)/InProc.pdb" /c\
 $(SOURCE) \
	

"$(INTDIR)\stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\stdafx.sbr" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\InProc.idb" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\InProc.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "InProc - Win32 Unicode Dll Debug"

# ADD CPP /Yc

BuildCmds= \
	$(CPP) /nologo /MTd /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS"\
 /D "_UNICODE" /D "_USRDLL" /D "_WINDLL" /D "_MBCS" /FR"$(INTDIR)/"\
 /Fp"$(INTDIR)/InProc.pch" /Yc /Fo"$(INTDIR)/" /Fd"$(OUTDIR)/InProc.pdb" /c\
 $(SOURCE) \
	

"$(INTDIR)\stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\stdafx.sbr" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\InProc.pdb" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\InProc.idb" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\InProc.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\strcoll.cpp
DEP_CPP_STRCO=\
	".\enumvar.h"\
	".\inproc.h"\
	".\resource.h"\
	".\stdafx.h"\
	".\strcoll.h"\
	

!IF  "$(CFG)" == "InProc - Win32 Release"


"$(INTDIR)\strcoll.obj" : $(SOURCE) $(DEP_CPP_STRCO) "$(INTDIR)"\
 "$(INTDIR)\InProc.pch"

"$(INTDIR)\strcoll.sbr" : $(SOURCE) $(DEP_CPP_STRCO) "$(INTDIR)"\
 "$(INTDIR)\InProc.pch"

"$(INTDIR)\InProc.idb" : $(SOURCE) $(DEP_CPP_STRCO) "$(INTDIR)"\
 "$(INTDIR)\InProc.pch"


!ELSEIF  "$(CFG)" == "InProc - Win32 Debug"


"$(INTDIR)\strcoll.obj" : $(SOURCE) $(DEP_CPP_STRCO) "$(INTDIR)"\
 "$(INTDIR)\InProc.pch"

"$(INTDIR)\strcoll.sbr" : $(SOURCE) $(DEP_CPP_STRCO) "$(INTDIR)"\
 "$(INTDIR)\InProc.pch"

"$(INTDIR)\InProc.pdb" : $(SOURCE) $(DEP_CPP_STRCO) "$(INTDIR)"\
 "$(INTDIR)\InProc.pch"

"$(INTDIR)\InProc.idb" : $(SOURCE) $(DEP_CPP_STRCO) "$(INTDIR)"\
 "$(INTDIR)\InProc.pch"


!ELSEIF  "$(CFG)" == "InProc - Win32 Unicode Release"


"$(INTDIR)\strcoll.obj" : $(SOURCE) $(DEP_CPP_STRCO) "$(INTDIR)"\
 "$(INTDIR)\InProc.pch"

"$(INTDIR)\strcoll.sbr" : $(SOURCE) $(DEP_CPP_STRCO) "$(INTDIR)"\
 "$(INTDIR)\InProc.pch"

"$(INTDIR)\InProc.idb" : $(SOURCE) $(DEP_CPP_STRCO) "$(INTDIR)"\
 "$(INTDIR)\InProc.pch"


!ELSEIF  "$(CFG)" == "InProc - Win32 Unicode Debug"


"$(INTDIR)\strcoll.obj" : $(SOURCE) $(DEP_CPP_STRCO) "$(INTDIR)"\
 "$(INTDIR)\InProc.pch"

"$(INTDIR)\strcoll.sbr" : $(SOURCE) $(DEP_CPP_STRCO) "$(INTDIR)"\
 "$(INTDIR)\InProc.pch"

"$(INTDIR)\InProc.pdb" : $(SOURCE) $(DEP_CPP_STRCO) "$(INTDIR)"\
 "$(INTDIR)\InProc.pch"

"$(INTDIR)\InProc.idb" : $(SOURCE) $(DEP_CPP_STRCO) "$(INTDIR)"\
 "$(INTDIR)\InProc.pch"


!ELSEIF  "$(CFG)" == "InProc - Win32 Dll Release"


"$(INTDIR)\strcoll.obj" : $(SOURCE) $(DEP_CPP_STRCO) "$(INTDIR)"\
 "$(INTDIR)\InProc.pch"

"$(INTDIR)\strcoll.sbr" : $(SOURCE) $(DEP_CPP_STRCO) "$(INTDIR)"\
 "$(INTDIR)\InProc.pch"

"$(INTDIR)\InProc.idb" : $(SOURCE) $(DEP_CPP_STRCO) "$(INTDIR)"\
 "$(INTDIR)\InProc.pch"


!ELSEIF  "$(CFG)" == "InProc - Win32 Dll Debug"


"$(INTDIR)\strcoll.obj" : $(SOURCE) $(DEP_CPP_STRCO) "$(INTDIR)"\
 "$(INTDIR)\InProc.pch"

"$(INTDIR)\strcoll.sbr" : $(SOURCE) $(DEP_CPP_STRCO) "$(INTDIR)"\
 "$(INTDIR)\InProc.pch"

"$(INTDIR)\InProc.pdb" : $(SOURCE) $(DEP_CPP_STRCO) "$(INTDIR)"\
 "$(INTDIR)\InProc.pch"

"$(INTDIR)\InProc.idb" : $(SOURCE) $(DEP_CPP_STRCO) "$(INTDIR)"\
 "$(INTDIR)\InProc.pch"


!ELSEIF  "$(CFG)" == "InProc - Win32 Unicode Dll Release"


"$(INTDIR)\strcoll.obj" : $(SOURCE) $(DEP_CPP_STRCO) "$(INTDIR)"\
 "$(INTDIR)\InProc.pch"

"$(INTDIR)\strcoll.sbr" : $(SOURCE) $(DEP_CPP_STRCO) "$(INTDIR)"\
 "$(INTDIR)\InProc.pch"

"$(INTDIR)\InProc.idb" : $(SOURCE) $(DEP_CPP_STRCO) "$(INTDIR)"\
 "$(INTDIR)\InProc.pch"


!ELSEIF  "$(CFG)" == "InProc - Win32 Unicode Dll Debug"


"$(INTDIR)\strcoll.obj" : $(SOURCE) $(DEP_CPP_STRCO) "$(INTDIR)"\
 "$(INTDIR)\InProc.pch"

"$(INTDIR)\strcoll.sbr" : $(SOURCE) $(DEP_CPP_STRCO) "$(INTDIR)"\
 "$(INTDIR)\InProc.pch"

"$(INTDIR)\InProc.pdb" : $(SOURCE) $(DEP_CPP_STRCO) "$(INTDIR)"\
 "$(INTDIR)\InProc.pch"

"$(INTDIR)\InProc.idb" : $(SOURCE) $(DEP_CPP_STRCO) "$(INTDIR)"\
 "$(INTDIR)\InProc.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\varassoc.cpp
DEP_CPP_VARAS=\
	".\inproc.h"\
	".\resource.h"\
	".\stdafx.h"\
	".\varassoc.h"\
	

!IF  "$(CFG)" == "InProc - Win32 Release"


"$(INTDIR)\varassoc.obj" : $(SOURCE) $(DEP_CPP_VARAS) "$(INTDIR)"\
 "$(INTDIR)\InProc.pch"

"$(INTDIR)\varassoc.sbr" : $(SOURCE) $(DEP_CPP_VARAS) "$(INTDIR)"\
 "$(INTDIR)\InProc.pch"

"$(INTDIR)\InProc.idb" : $(SOURCE) $(DEP_CPP_VARAS) "$(INTDIR)"\
 "$(INTDIR)\InProc.pch"


!ELSEIF  "$(CFG)" == "InProc - Win32 Debug"


"$(INTDIR)\varassoc.obj" : $(SOURCE) $(DEP_CPP_VARAS) "$(INTDIR)"\
 "$(INTDIR)\InProc.pch"

"$(INTDIR)\varassoc.sbr" : $(SOURCE) $(DEP_CPP_VARAS) "$(INTDIR)"\
 "$(INTDIR)\InProc.pch"

"$(INTDIR)\InProc.pdb" : $(SOURCE) $(DEP_CPP_VARAS) "$(INTDIR)"\
 "$(INTDIR)\InProc.pch"

"$(INTDIR)\InProc.idb" : $(SOURCE) $(DEP_CPP_VARAS) "$(INTDIR)"\
 "$(INTDIR)\InProc.pch"


!ELSEIF  "$(CFG)" == "InProc - Win32 Unicode Release"


"$(INTDIR)\varassoc.obj" : $(SOURCE) $(DEP_CPP_VARAS) "$(INTDIR)"\
 "$(INTDIR)\InProc.pch"

"$(INTDIR)\varassoc.sbr" : $(SOURCE) $(DEP_CPP_VARAS) "$(INTDIR)"\
 "$(INTDIR)\InProc.pch"

"$(INTDIR)\InProc.idb" : $(SOURCE) $(DEP_CPP_VARAS) "$(INTDIR)"\
 "$(INTDIR)\InProc.pch"


!ELSEIF  "$(CFG)" == "InProc - Win32 Unicode Debug"


"$(INTDIR)\varassoc.obj" : $(SOURCE) $(DEP_CPP_VARAS) "$(INTDIR)"\
 "$(INTDIR)\InProc.pch"

"$(INTDIR)\varassoc.sbr" : $(SOURCE) $(DEP_CPP_VARAS) "$(INTDIR)"\
 "$(INTDIR)\InProc.pch"

"$(INTDIR)\InProc.pdb" : $(SOURCE) $(DEP_CPP_VARAS) "$(INTDIR)"\
 "$(INTDIR)\InProc.pch"

"$(INTDIR)\InProc.idb" : $(SOURCE) $(DEP_CPP_VARAS) "$(INTDIR)"\
 "$(INTDIR)\InProc.pch"


!ELSEIF  "$(CFG)" == "InProc - Win32 Dll Release"


"$(INTDIR)\varassoc.obj" : $(SOURCE) $(DEP_CPP_VARAS) "$(INTDIR)"\
 "$(INTDIR)\InProc.pch"

"$(INTDIR)\varassoc.sbr" : $(SOURCE) $(DEP_CPP_VARAS) "$(INTDIR)"\
 "$(INTDIR)\InProc.pch"

"$(INTDIR)\InProc.idb" : $(SOURCE) $(DEP_CPP_VARAS) "$(INTDIR)"\
 "$(INTDIR)\InProc.pch"


!ELSEIF  "$(CFG)" == "InProc - Win32 Dll Debug"


"$(INTDIR)\varassoc.obj" : $(SOURCE) $(DEP_CPP_VARAS) "$(INTDIR)"\
 "$(INTDIR)\InProc.pch"

"$(INTDIR)\varassoc.sbr" : $(SOURCE) $(DEP_CPP_VARAS) "$(INTDIR)"\
 "$(INTDIR)\InProc.pch"

"$(INTDIR)\InProc.pdb" : $(SOURCE) $(DEP_CPP_VARAS) "$(INTDIR)"\
 "$(INTDIR)\InProc.pch"

"$(INTDIR)\InProc.idb" : $(SOURCE) $(DEP_CPP_VARAS) "$(INTDIR)"\
 "$(INTDIR)\InProc.pch"


!ELSEIF  "$(CFG)" == "InProc - Win32 Unicode Dll Release"


"$(INTDIR)\varassoc.obj" : $(SOURCE) $(DEP_CPP_VARAS) "$(INTDIR)"\
 "$(INTDIR)\InProc.pch"

"$(INTDIR)\varassoc.sbr" : $(SOURCE) $(DEP_CPP_VARAS) "$(INTDIR)"\
 "$(INTDIR)\InProc.pch"

"$(INTDIR)\InProc.idb" : $(SOURCE) $(DEP_CPP_VARAS) "$(INTDIR)"\
 "$(INTDIR)\InProc.pch"


!ELSEIF  "$(CFG)" == "InProc - Win32 Unicode Dll Debug"


"$(INTDIR)\varassoc.obj" : $(SOURCE) $(DEP_CPP_VARAS) "$(INTDIR)"\
 "$(INTDIR)\InProc.pch"

"$(INTDIR)\varassoc.sbr" : $(SOURCE) $(DEP_CPP_VARAS) "$(INTDIR)"\
 "$(INTDIR)\InProc.pch"

"$(INTDIR)\InProc.pdb" : $(SOURCE) $(DEP_CPP_VARAS) "$(INTDIR)"\
 "$(INTDIR)\InProc.pch"

"$(INTDIR)\InProc.idb" : $(SOURCE) $(DEP_CPP_VARAS) "$(INTDIR)"\
 "$(INTDIR)\InProc.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\enumvar.cpp
DEP_CPP_ENUMV=\
	".\enumvar.h"\
	".\inproc.h"\
	".\resource.h"\
	".\stdafx.h"\
	

!IF  "$(CFG)" == "InProc - Win32 Release"


"$(INTDIR)\enumvar.obj" : $(SOURCE) $(DEP_CPP_ENUMV) "$(INTDIR)"\
 "$(INTDIR)\InProc.pch"

"$(INTDIR)\enumvar.sbr" : $(SOURCE) $(DEP_CPP_ENUMV) "$(INTDIR)"\
 "$(INTDIR)\InProc.pch"

"$(INTDIR)\InProc.idb" : $(SOURCE) $(DEP_CPP_ENUMV) "$(INTDIR)"\
 "$(INTDIR)\InProc.pch"


!ELSEIF  "$(CFG)" == "InProc - Win32 Debug"


"$(INTDIR)\enumvar.obj" : $(SOURCE) $(DEP_CPP_ENUMV) "$(INTDIR)"\
 "$(INTDIR)\InProc.pch"

"$(INTDIR)\enumvar.sbr" : $(SOURCE) $(DEP_CPP_ENUMV) "$(INTDIR)"\
 "$(INTDIR)\InProc.pch"

"$(INTDIR)\InProc.pdb" : $(SOURCE) $(DEP_CPP_ENUMV) "$(INTDIR)"\
 "$(INTDIR)\InProc.pch"

"$(INTDIR)\InProc.idb" : $(SOURCE) $(DEP_CPP_ENUMV) "$(INTDIR)"\
 "$(INTDIR)\InProc.pch"


!ELSEIF  "$(CFG)" == "InProc - Win32 Unicode Release"


"$(INTDIR)\enumvar.obj" : $(SOURCE) $(DEP_CPP_ENUMV) "$(INTDIR)"\
 "$(INTDIR)\InProc.pch"

"$(INTDIR)\enumvar.sbr" : $(SOURCE) $(DEP_CPP_ENUMV) "$(INTDIR)"\
 "$(INTDIR)\InProc.pch"

"$(INTDIR)\InProc.idb" : $(SOURCE) $(DEP_CPP_ENUMV) "$(INTDIR)"\
 "$(INTDIR)\InProc.pch"


!ELSEIF  "$(CFG)" == "InProc - Win32 Unicode Debug"


"$(INTDIR)\enumvar.obj" : $(SOURCE) $(DEP_CPP_ENUMV) "$(INTDIR)"\
 "$(INTDIR)\InProc.pch"

"$(INTDIR)\enumvar.sbr" : $(SOURCE) $(DEP_CPP_ENUMV) "$(INTDIR)"\
 "$(INTDIR)\InProc.pch"

"$(INTDIR)\InProc.pdb" : $(SOURCE) $(DEP_CPP_ENUMV) "$(INTDIR)"\
 "$(INTDIR)\InProc.pch"

"$(INTDIR)\InProc.idb" : $(SOURCE) $(DEP_CPP_ENUMV) "$(INTDIR)"\
 "$(INTDIR)\InProc.pch"


!ELSEIF  "$(CFG)" == "InProc - Win32 Dll Release"


"$(INTDIR)\enumvar.obj" : $(SOURCE) $(DEP_CPP_ENUMV) "$(INTDIR)"\
 "$(INTDIR)\InProc.pch"

"$(INTDIR)\enumvar.sbr" : $(SOURCE) $(DEP_CPP_ENUMV) "$(INTDIR)"\
 "$(INTDIR)\InProc.pch"

"$(INTDIR)\InProc.idb" : $(SOURCE) $(DEP_CPP_ENUMV) "$(INTDIR)"\
 "$(INTDIR)\InProc.pch"


!ELSEIF  "$(CFG)" == "InProc - Win32 Dll Debug"


"$(INTDIR)\enumvar.obj" : $(SOURCE) $(DEP_CPP_ENUMV) "$(INTDIR)"\
 "$(INTDIR)\InProc.pch"

"$(INTDIR)\enumvar.sbr" : $(SOURCE) $(DEP_CPP_ENUMV) "$(INTDIR)"\
 "$(INTDIR)\InProc.pch"

"$(INTDIR)\InProc.pdb" : $(SOURCE) $(DEP_CPP_ENUMV) "$(INTDIR)"\
 "$(INTDIR)\InProc.pch"

"$(INTDIR)\InProc.idb" : $(SOURCE) $(DEP_CPP_ENUMV) "$(INTDIR)"\
 "$(INTDIR)\InProc.pch"


!ELSEIF  "$(CFG)" == "InProc - Win32 Unicode Dll Release"


"$(INTDIR)\enumvar.obj" : $(SOURCE) $(DEP_CPP_ENUMV) "$(INTDIR)"\
 "$(INTDIR)\InProc.pch"

"$(INTDIR)\enumvar.sbr" : $(SOURCE) $(DEP_CPP_ENUMV) "$(INTDIR)"\
 "$(INTDIR)\InProc.pch"

"$(INTDIR)\InProc.idb" : $(SOURCE) $(DEP_CPP_ENUMV) "$(INTDIR)"\
 "$(INTDIR)\InProc.pch"


!ELSEIF  "$(CFG)" == "InProc - Win32 Unicode Dll Debug"


"$(INTDIR)\enumvar.obj" : $(SOURCE) $(DEP_CPP_ENUMV) "$(INTDIR)"\
 "$(INTDIR)\InProc.pch"

"$(INTDIR)\enumvar.sbr" : $(SOURCE) $(DEP_CPP_ENUMV) "$(INTDIR)"\
 "$(INTDIR)\InProc.pch"

"$(INTDIR)\InProc.pdb" : $(SOURCE) $(DEP_CPP_ENUMV) "$(INTDIR)"\
 "$(INTDIR)\InProc.pch"

"$(INTDIR)\InProc.idb" : $(SOURCE) $(DEP_CPP_ENUMV) "$(INTDIR)"\
 "$(INTDIR)\InProc.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\inproc.def

!IF  "$(CFG)" == "InProc - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "InProc - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "InProc - Win32 Unicode Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "InProc - Win32 Unicode Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "InProc - Win32 Dll Release"

!ELSEIF  "$(CFG)" == "InProc - Win32 Dll Debug"

!ELSEIF  "$(CFG)" == "InProc - Win32 Unicode Dll Release"

!ELSEIF  "$(CFG)" == "InProc - Win32 Unicode Dll Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\inproc.odl

!IF  "$(CFG)" == "InProc - Win32 Release"


"$(OUTDIR)\inproc.tlb" : $(SOURCE) "$(OUTDIR)"
   $(MTL) /nologo /D "NDEBUG" /tlb "$(OUTDIR)/inproc.tlb" /win32 $(SOURCE)


!ELSEIF  "$(CFG)" == "InProc - Win32 Debug"


"$(OUTDIR)\inproc.tlb" : $(SOURCE) "$(OUTDIR)"
   $(MTL) /nologo /D "_DEBUG" /tlb "$(OUTDIR)/inproc.tlb" /win32 $(SOURCE)


!ELSEIF  "$(CFG)" == "InProc - Win32 Unicode Release"


"$(OUTDIR)\inproc.tlb" : $(SOURCE) "$(OUTDIR)"
   $(MTL) /nologo /D "NDEBUG" /tlb "$(OUTDIR)/inproc.tlb" /win32 $(SOURCE)


!ELSEIF  "$(CFG)" == "InProc - Win32 Unicode Debug"


"$(OUTDIR)\inproc.tlb" : $(SOURCE) "$(OUTDIR)"
   $(MTL) /nologo /D "_DEBUG" /tlb "$(OUTDIR)/inproc.tlb" /win32 $(SOURCE)


!ELSEIF  "$(CFG)" == "InProc - Win32 Dll Release"


"$(OUTDIR)\inproc.tlb" : $(SOURCE) "$(OUTDIR)"
   $(MTL) /nologo /D "NDEBUG" /tlb "$(OUTDIR)/inproc.tlb" /win32 $(SOURCE)


!ELSEIF  "$(CFG)" == "InProc - Win32 Dll Debug"


"$(OUTDIR)\inproc.tlb" : $(SOURCE) "$(OUTDIR)"
   $(MTL) /nologo /D "_DEBUG" /tlb "$(OUTDIR)/inproc.tlb" /win32 $(SOURCE)


!ELSEIF  "$(CFG)" == "InProc - Win32 Unicode Dll Release"


"$(OUTDIR)\inproc.tlb" : $(SOURCE) "$(OUTDIR)"
   $(MTL) /nologo /D "NDEBUG" /tlb "$(OUTDIR)/inproc.tlb" /win32 $(SOURCE)


!ELSEIF  "$(CFG)" == "InProc - Win32 Unicode Dll Debug"


"$(OUTDIR)\inproc.tlb" : $(SOURCE) "$(OUTDIR)"
   $(MTL) /nologo /D "_DEBUG" /tlb "$(OUTDIR)/inproc.tlb" /win32 $(SOURCE)


!ENDIF 

# End Source File
# End Target
# End Project
################################################################################
