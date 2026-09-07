# Microsoft Visual C++ Generated NMAKE File, Format Version 40000
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101
# TARGTYPE "Power Macintosh Application" 0x0401
# TARGTYPE "Macintosh Application" 0x0301

!IF "$(CFG)" == ""
CFG=modeless - Win32 Debug
!MESSAGE No configuration specified.  Defaulting to modeless - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "modeless - Win32 Release" && "$(CFG)" !=\
 "modeless - Win32 Debug" && "$(CFG)" != "modeless - Power Macintosh Debug" &&\
 "$(CFG)" != "modeless - Power Macintosh Release" && "$(CFG)" !=\
 "modeless - Macintosh Debug" && "$(CFG)" != "modeless - Macintosh Release"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "modeless.mak" CFG="modeless - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "modeless - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "modeless - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE "modeless - Power Macintosh Debug" (based on\
 "Power Macintosh Application")
!MESSAGE "modeless - Power Macintosh Release" (based on\
 "Power Macintosh Application")
!MESSAGE "modeless - Macintosh Debug" (based on "Macintosh Application")
!MESSAGE "modeless - Macintosh Release" (based on "Macintosh Application")
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
# PROP Target_Last_Scanned "modeless - Macintosh Release"

!IF  "$(CFG)" == "modeless - Win32 Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir WinRel
# PROP BASE Intermediate_Dir WinRel
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir Release
# PROP Intermediate_Dir Release
OUTDIR=.\Release
INTDIR=.\Release

ALL : "$(OUTDIR)\modeless.exe" "$(OUTDIR)\modeless.bsc"

CLEAN : 
	-@erase ".\Release\modeless.bsc"
	-@erase ".\Release\adderdlg.sbr"
	-@erase ".\Release\modeless.pch"
	-@erase ".\Release\modeless.sbr"
	-@erase ".\Release\stdafx.sbr"
	-@erase ".\Release\modeldlg.sbr"
	-@erase ".\Release\modeless.exe"
	-@erase ".\Release\modeldlg.obj"
	-@erase ".\Release\adderdlg.obj"
	-@erase ".\Release\modeless.obj"
	-@erase ".\Release\stdafx.obj"
	-@erase ".\Release\modeless.res"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

+++++++++++++: Original source
	-@erase ".\WinRel\modeless.bsc"
	-@erase ".\WinRel\modeldlg.sbr"
	-@erase ".\WinRel\modeless.pch"
	-@erase ".\WinRel\stdafx.sbr"
	-@erase ".\WinRel\adderdlg.sbr"
	-@erase ".\WinRel\modeless.sbr"
	-@erase ".\WinRel\modeless.exe"
	-@erase ".\WinRel\stdafx.obj"
	-@erase ".\WinRel\adderdlg.obj"
	-@erase ".\WinRel\modeless.obj"
	-@erase ".\WinRel\modeldlg.obj"
	-@erase ".\WinRel\modeless.res"

+++++++++++++: Current source
	-@erase ".\WinRel\modeless.bsc"
	-@erase ".\WinRel\stdafx.sbr"
	-@erase ".\WinRel\modeldlg.sbr"
	-@erase ".\WinRel\modeless.pch"
	-@erase ".\WinRel\adderdlg.sbr"
	-@erase ".\WinRel\modeless.sbr"
	-@erase ".\WinRel\modeless.exe"
	-@erase ".\WinRel\adderdlg.obj"
	-@erase ".\WinRel\modeless.obj"
	-@erase ".\WinRel\stdafx.obj"
	-@erase ".\WinRel\modeldlg.obj"
	-@erase ".\WinRel\modeless.res"

+++++++++++++: User's version
	-@erase ".\Release\modeless.bsc"
	-@erase ".\Release\modeless.sbr"
	-@erase ".\Release\modeless.pch"
	-@erase ".\Release\modeldlg.sbr"
	-@erase ".\Release\stdafx.sbr"
	-@erase ".\Release\adderdlg.sbr"
	-@erase ".\Release\modeless.exe"
	-@erase ".\Release\modeless.obj"
	-@erase ".\Release\modeldlg.obj"
	-@erase ".\Release\stdafx.obj"
	-@erase ".\Release\modeless.res"
	-@erase ".\Release\adderdlg.obj"

+++++++++++++: Current source
CPP=cl.exe
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /Yu"stdafx.h" /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /Yu"stdafx.h" /c
CPP_PROJ=/nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /FR"$(INTDIR)/" /Fp"$(INTDIR)/modeless.pch" /Yu"stdafx.h"\
 /Fo"$(INTDIR)/" /c 
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
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/modeless.res" /d "NDEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/modeless.bsc" 
BSC32_SBRS= \
	"$(INTDIR)/adderdlg.sbr" \
	"$(INTDIR)/modeless.sbr" \
	"$(INTDIR)/stdafx.sbr" \
	"$(INTDIR)/modeldlg.sbr"

"$(OUTDIR)\modeless.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 /nologo /subsystem:windows /machine:I386
LINK32_FLAGS=/nologo /subsystem:windows /incremental:no\
 /pdb:"$(OUTDIR)/modeless.pdb" /machine:I386 /out:"$(OUTDIR)/modeless.exe" 
LINK32_OBJS= \
	"$(INTDIR)/modeldlg.obj" \
	"$(INTDIR)/adderdlg.obj" \
	"$(INTDIR)/modeless.obj" \
	"$(INTDIR)/stdafx.obj" \
	"$(INTDIR)/modeless.res"

"$(OUTDIR)\modeless.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "modeless - Win32 Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir WinDebug
# PROP BASE Intermediate_Dir WinDebug
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir WinDebug
# PROP Intermediate_Dir WinDebug
OUTDIR=.\WinDebug
INTDIR=.\WinDebug

ALL : "$(OUTDIR)\modeless.exe" "$(OUTDIR)\modeless.bsc"

CLEAN : 
	-@erase ".\WinDebug\vc40.pdb"
	-@erase ".\WinDebug\modeless.pch"
	-@erase ".\WinDebug\modeless.bsc"
	-@erase ".\WinDebug\modeldlg.sbr"
	-@erase ".\WinDebug\stdafx.sbr"
	-@erase ".\WinDebug\adderdlg.sbr"
	-@erase ".\WinDebug\modeless.sbr"
	-@erase ".\WinDebug\modeless.exe"
	-@erase ".\WinDebug\modeldlg.obj"
	-@erase ".\WinDebug\stdafx.obj"
	-@erase ".\WinDebug\adderdlg.obj"
	-@erase ".\WinDebug\modeless.obj"
	-@erase ".\WinDebug\modeless.res"
	-@erase ".\WinDebug\modeless.ilk"
	-@erase ".\WinDebug\modeless.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /MDd /W3 /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /Yu"stdafx.h" /c
# ADD CPP /nologo /MDd /W3 /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /Yu"stdafx.h" /c
CPP_PROJ=/nologo /MDd /W3 /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /FR"$(INTDIR)/" /Fp"$(INTDIR)/modeless.pch" /Yu"stdafx.h"\
 /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\WinDebug/
CPP_SBRS=.\WinDebug/

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
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/modeless.res" /d "_DEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/modeless.bsc" 
BSC32_SBRS= \
	"$(INTDIR)/modeldlg.sbr" \
	"$(INTDIR)/stdafx.sbr" \
	"$(INTDIR)/adderdlg.sbr" \
	"$(INTDIR)/modeless.sbr"

"$(OUTDIR)\modeless.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386
# ADD LINK32 /nologo /subsystem:windows /debug /machine:I386
LINK32_FLAGS=/nologo /subsystem:windows /incremental:yes\
 /pdb:"$(OUTDIR)/modeless.pdb" /debug /machine:I386\
 /out:"$(OUTDIR)/modeless.exe" 
LINK32_OBJS= \
	"$(INTDIR)/modeldlg.obj" \
	"$(INTDIR)/stdafx.obj" \
	"$(INTDIR)/adderdlg.obj" \
	"$(INTDIR)/modeless.obj" \
	"$(INTDIR)/modeless.res"

"$(OUTDIR)\modeless.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "modeless - Power Macintosh Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir modeless
# PROP BASE Intermediate_Dir modeless
# PROP BASE Target_Dir
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir PMacDbg
# PROP Intermediate_Dir PMacDbg
# PROP Target_Dir
OUTDIR=.\PMacDbg
INTDIR=.\PMacDbg

ALL : "$(OUTDIR)\modeless.exe" "$(OUTDIR)\modeless.bsc"\
 "$(OUTDIR)\modeless.trg"

CLEAN : 
	-@erase ".\PMacDbg\vc40.pdb"
	-@erase ".\PMacDbg\modeless.pch"
	-@erase ".\PMacDbg\modeless.bsc"
	-@erase ".\PMacDbg\modeldlg.sbr"
	-@erase ".\PMacDbg\adderdlg.sbr"
	-@erase ".\PMacDbg\modeless.sbr"
	-@erase ".\PMacDbg\stdafx.sbr"
	-@erase ".\PMacDbg\modeless.ilk"
	-@erase ".\PMacDbg\modeldlg.obj"
	-@erase ".\PMacDbg\adderdlg.obj"
	-@erase ".\PMacDbg\modeless.obj"
	-@erase ".\PMacDbg\stdafx.obj"
	-@erase ".\PMacDbg\mdlsmac.rsc"
	-@erase ".\PMacDbg\modeless.rsc"
	-@erase ".\PMacDbg\modeless.pdb"
	-@erase ".\PMacDbg\modeless.trg"
	-@erase ".\PMacDbg\modeless.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

MTL=mktyplib.exe
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
CPP=cl.exe
# ADD BASE CPP /nologo /MDd /W3 /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_AFXDLL" /FR /Yu"stdafx.h" /c
# ADD CPP /nologo /MDd /W3 /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_AFXDLL" /D "_MAC" /D "_MPPC_" /FR /Yu"stdafx.h" /c
CPP_PROJ=/nologo /MDd /W3 /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D\
 "_MBCS" /D "_AFXDLL" /D "_MAC" /D "_MPPC_" /FR"$(INTDIR)/"\
 /Fp"$(INTDIR)/modeless.pch" /Yu"stdafx.h" /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\PMacDbg/
CPP_SBRS=.\PMacDbg/

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

RSC=rc.exe
# ADD BASE RSC /l 0x409 /r /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /r /d "_DEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /r /m /fo"$(INTDIR)/modeless.rsc" /d "_DEBUG" /d "_AFXDLL" 
MRC=mrc.exe
# ADD BASE MRC /D "_MPPC_" /D "_MAC" /D "_DEBUG" /NOLOGO
# ADD MRC /D "_MPPC_" /D "_MAC" /D "_DEBUG" /NOLOGO
MRC_PROJ=/D "_MPPC_" /D "_MAC" /D "_DEBUG" /l 0x409 /NOLOGO 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/modeless.bsc" 
BSC32_SBRS= \
	"$(INTDIR)/modeldlg.sbr" \
	"$(INTDIR)/adderdlg.sbr" \
	"$(INTDIR)/modeless.sbr" \
	"$(INTDIR)/stdafx.sbr"

"$(OUTDIR)\modeless.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 /nologo /mac:bundle /debug /machine:I386
# SUBTRACT BASE LINK32 /incremental:no
# ADD LINK32 /nologo /mac:bundle /debug /machine:MPPC
# SUBTRACT LINK32 /pdb:none
LINK32_FLAGS=/nologo /mac:bundle /mac:type="APPL" /mac:creator="????"\
 /incremental:yes /pdb:"$(OUTDIR)/modeless.pdb" /debug /machine:MPPC\
 /out:"$(OUTDIR)/modeless.exe" 
LINK32_OBJS= \
	"$(INTDIR)/modeldlg.obj" \
	"$(INTDIR)/adderdlg.obj" \
	"$(INTDIR)/modeless.obj" \
	"$(INTDIR)/stdafx.obj" \
	"$(INTDIR)/mdlsmac.rsc" \
	"$(INTDIR)/modeless.rsc"

"$(OUTDIR)\modeless.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

MFILE32=mfile.exe
# ADD BASE MFILE32 COPY /NOLOGO
# ADD MFILE32 COPY /NOLOGO
MFILE32_FLAGS=COPY /NOLOGO 
MFILE32_FILES= \
	"$(OUTDIR)/modeless.exe"

"$(OUTDIR)\modeless.trg" : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\PMacDbg\modeless.exe\
 "$(MFILE32_DEST):modeless.exe">"$(OUTDIR)\modeless.trg"

DOWNLOAD : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\PMacDbg\modeless.exe\
 "$(MFILE32_DEST):modeless.exe">"$(OUTDIR)\modeless.trg"

!ELSEIF  "$(CFG)" == "modeless - Power Macintosh Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir modeles0
# PROP BASE Intermediate_Dir modeles0
# PROP BASE Target_Dir
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir PMacRel
# PROP Intermediate_Dir PMacRel
# PROP Target_Dir
OUTDIR=.\PMacRel
INTDIR=.\PMacRel

ALL : "$(OUTDIR)\modeless.exe" "$(OUTDIR)\modeless.bsc"\
 "$(OUTDIR)\modeless.trg"

CLEAN : 
	-@erase ".\PMacRel\vc40.pdb"
	-@erase ".\PMacRel\modeless.bsc"
	-@erase ".\PMacRel\modeldlg.sbr"
	-@erase ".\PMacRel\modeless.pch"
	-@erase ".\PMacRel\adderdlg.sbr"
	-@erase ".\PMacRel\modeless.sbr"
	-@erase ".\PMacRel\stdafx.sbr"
	-@erase ".\PMacRel\modeless.pdb"
	-@erase ".\PMacRel\stdafx.obj"
	-@erase ".\PMacRel\modeldlg.obj"
	-@erase ".\PMacRel\adderdlg.obj"
	-@erase ".\PMacRel\modeless.obj"
	-@erase ".\PMacRel\modeless.rsc"
	-@erase ".\PMacRel\mdlsmac.rsc"
	-@erase ".\PMacRel\modeless.trg"
	-@erase ".\PMacRel\modeless.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

MTL=mktyplib.exe
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
CPP=cl.exe
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_AFXDLL" /FR /Yu"stdafx.h" /c
# ADD CPP /nologo /MD /W3 /GX /Zi /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_AFXDLL" /D "_MAC" /D "_MPPC_" /FR /Yu"stdafx.h" /c
CPP_PROJ=/nologo /MD /W3 /GX /Zi /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D\
 "_MBCS" /D "_AFXDLL" /D "_MAC" /D "_MPPC_" /FR"$(INTDIR)/"\
 /Fp"$(INTDIR)/modeless.pch" /Yu"stdafx.h" /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\PMacRel/
CPP_SBRS=.\PMacRel/

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

RSC=rc.exe
# ADD BASE RSC /l 0x409 /r /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /r /d "NDEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /r /m /fo"$(INTDIR)/modeless.rsc" /d "NDEBUG" /d "_AFXDLL" 
MRC=mrc.exe
# ADD BASE MRC /D "_MPPC_" /D "_MAC" /D "NDEBUG" /NOLOGO
# ADD MRC /D "_MPPC_" /D "_MAC" /D "NDEBUG" /NOLOGO
MRC_PROJ=/D "_MPPC_" /D "_MAC" /D "NDEBUG" /l 0x409 /NOLOGO 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/modeless.bsc" 
BSC32_SBRS= \
	"$(INTDIR)/modeldlg.sbr" \
	"$(INTDIR)/adderdlg.sbr" \
	"$(INTDIR)/modeless.sbr" \
	"$(INTDIR)/stdafx.sbr"

"$(OUTDIR)\modeless.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 /nologo /mac:bundle /machine:I386
# ADD LINK32 /nologo /mac:bundle /debug /machine:MPPC
# SUBTRACT LINK32 /pdb:none
LINK32_FLAGS=/nologo /mac:bundle /mac:type="APPL" /mac:creator="????"\
 /incremental:no /pdb:"$(OUTDIR)/modeless.pdb" /debug /machine:MPPC\
 /out:"$(OUTDIR)/modeless.exe" 
LINK32_OBJS= \
	"$(INTDIR)/stdafx.obj" \
	"$(INTDIR)/modeldlg.obj" \
	"$(INTDIR)/adderdlg.obj" \
	"$(INTDIR)/modeless.obj" \
	"$(INTDIR)/modeless.rsc" \
	"$(INTDIR)/mdlsmac.rsc"

"$(OUTDIR)\modeless.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

MFILE32=mfile.exe
# ADD BASE MFILE32 COPY /NOLOGO
# ADD MFILE32 COPY /NOLOGO
MFILE32_FLAGS=COPY /NOLOGO 
MFILE32_FILES= \
	"$(OUTDIR)/modeless.exe"

"$(OUTDIR)\modeless.trg" : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\PMacRel\modeless.exe\
 "$(MFILE32_DEST):modeless.exe">"$(OUTDIR)\modeless.trg"

DOWNLOAD : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\PMacRel\modeless.exe\
 "$(MFILE32_DEST):modeless.exe">"$(OUTDIR)\modeless.trg"

!ELSEIF  "$(CFG)" == "modeless - Macintosh Debug"

# PROP BASE Use_MFC 5
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir modeles1
# PROP BASE Intermediate_Dir modeles1
# PROP BASE Target_Dir
# PROP Use_MFC 5
# PROP Use_Debug_Libraries 1
# PROP Output_Dir MacDbg
# PROP Intermediate_Dir MacDbg
# PROP Target_Dir
OUTDIR=.\MacDbg
INTDIR=.\MacDbg

ALL : "$(OUTDIR)\modeless.exe" "$(OUTDIR)\modeless.bsc"\
 "$(OUTDIR)\modeless.trg"

CLEAN : 
	-@erase ".\MacDbg\vc40.pdb"
	-@erase ".\MacDbg\modeless.pch"
	-@erase ".\MacDbg\modeless.bsc"
	-@erase ".\MacDbg\modeldlg.sbr"
	-@erase ".\MacDbg\stdafx.sbr"
	-@erase ".\MacDbg\adderdlg.sbr"
	-@erase ".\MacDbg\modeless.sbr"
	-@erase ".\MacDbg\modeless.pdb"
	-@erase ".\MacDbg\modeldlg.obj"
	-@erase ".\MacDbg\stdafx.obj"
	-@erase ".\MacDbg\adderdlg.obj"
	-@erase ".\MacDbg\modeless.obj"
	-@erase ".\M68KDbg\mdlsmac.rsc"
	-@erase ".\MacDbg\modeless.rsc"
	-@erase ".\MacDbg\modeless.trg"
	-@erase ".\MacDbg\modeless.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

MTL=mktyplib.exe
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
CPP=cl.exe
# ADD BASE CPP /nologo /AL /Q68s /Q68m /W3 /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /Yu"stdafx.h" /c
# ADD CPP /nologo /AL /Q68s /Q68m /W3 /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_MAC" /D "_68K_" /D "_MBCS" /FR /Yu"stdafx.h" /c
CPP_PROJ=/nologo /AL /Q68s /Q68m /W3 /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D\
 "_WINDOWS" /D "_MAC" /D "_68K_" /D "_MBCS" /FR"$(INTDIR)/"\
 /Fp"$(INTDIR)/modeless.pch" /Yu"stdafx.h" /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\MacDbg/
CPP_SBRS=.\MacDbg/

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

RSC=rc.exe
# ADD BASE RSC /l 0x409 /r /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /r /d "_DEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /r /m /fo"$(INTDIR)/modeless.rsc" /d "_DEBUG" /d "_AFXDLL" 
MRC=mrc.exe
# ADD BASE MRC /D "_68K_" /D "_MAC" /D "_DEBUG" /NOLOGO
# ADD MRC /D "_68K_" /D "_MAC" /D "_DEBUG" /NOLOGO
MRC_PROJ=/D "_68K_" /D "_MAC" /D "_DEBUG" /l 0x409 /NOLOGO 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/modeless.bsc" 
BSC32_SBRS= \
	"$(INTDIR)/modeldlg.sbr" \
	"$(INTDIR)/stdafx.sbr" \
	"$(INTDIR)/adderdlg.sbr" \
	"$(INTDIR)/modeless.sbr"

"$(OUTDIR)\modeless.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 /nologo /MAC:bundle /debug /machine:I386
# ADD LINK32 /nologo /MAC:bundle /debug /machine:M68K
# SUBTRACT LINK32 /pdb:none
LINK32_FLAGS=/nologo /MAC:bundle /MAC:type="APPL" /MAC:creator="????"\
 /pdb:"$(OUTDIR)/modeless.pdb" /debug /machine:M68K\
 /out:"$(OUTDIR)/modeless.exe" 
LINK32_OBJS= \
	"$(INTDIR)/modeldlg.obj" \
	"$(INTDIR)/stdafx.obj" \
	"$(INTDIR)/adderdlg.obj" \
	"$(INTDIR)/modeless.obj" \
	".\M68KDbg\mdlsmac.rsc" \
	"$(INTDIR)/modeless.rsc"

"$(OUTDIR)\modeless.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

MFILE32=mfile.exe
# ADD BASE MFILE32 COPY /NOLOGO
# ADD MFILE32 COPY /NOLOGO
MFILE32_FLAGS=COPY /NOLOGO 
MFILE32_FILES= \
	"$(OUTDIR)/modeless.exe"

"$(OUTDIR)\modeless.trg" : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\MacDbg\modeless.exe\
 "$(MFILE32_DEST):modeless.exe">"$(OUTDIR)\modeless.trg"

DOWNLOAD : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\MacDbg\modeless.exe\
 "$(MFILE32_DEST):modeless.exe">"$(OUTDIR)\modeless.trg"

!ELSEIF  "$(CFG)" == "modeless - Macintosh Release"

# PROP BASE Use_MFC 5
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir modeles2
# PROP BASE Intermediate_Dir modeles2
# PROP BASE Target_Dir
# PROP Use_MFC 5
# PROP Use_Debug_Libraries 0
# PROP Output_Dir MacRel
# PROP Intermediate_Dir MacRel
# PROP Target_Dir
OUTDIR=.\MacRel
INTDIR=.\MacRel

ALL : "$(OUTDIR)\modeless.exe" "$(OUTDIR)\modeless.bsc"\
 "$(OUTDIR)\modeless.trg"

CLEAN : 
	-@erase ".\MacRel\modeless.bsc"
	-@erase ".\MacRel\modeldlg.sbr"
	-@erase ".\MacRel\modeless.pch"
	-@erase ".\MacRel\stdafx.sbr"
	-@erase ".\MacRel\adderdlg.sbr"
	-@erase ".\MacRel\modeless.sbr"
	-@erase ".\MacRel\modeless.trg"
	-@erase ".\MacRel\modeless.exe"
	-@erase ".\MacRel\modeldlg.obj"
	-@erase ".\MacRel\stdafx.obj"
	-@erase ".\MacRel\adderdlg.obj"
	-@erase ".\MacRel\modeless.obj"
	-@erase ".\MacRel\modeless.rsc"
	-@erase ".\M68KRel\mdlsmac.rsc"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

MTL=mktyplib.exe
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
CPP=cl.exe
# ADD BASE CPP /nologo /AL /Q68s /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /Yu"stdafx.h" /c
# ADD CPP /nologo /AL /Q68s /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MAC" /D "_68K_" /D "_MBCS" /FR /Yu"stdafx.h" /c
CPP_PROJ=/nologo /AL /Q68s /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D\
 "_MAC" /D "_68K_" /D "_MBCS" /FR"$(INTDIR)/" /Fp"$(INTDIR)/modeless.pch"\
 /Yu"stdafx.h" /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\MacRel/
CPP_SBRS=.\MacRel/

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

RSC=rc.exe
# ADD BASE RSC /l 0x409 /r /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /r /d "NDEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /r /m /fo"$(INTDIR)/modeless.rsc" /d "NDEBUG" /d "_AFXDLL" 
MRC=mrc.exe
# ADD BASE MRC /D "_68K_" /D "_MAC" /D "NDEBUG" /NOLOGO
# ADD MRC /D "_68K_" /D "_MAC" /D "NDEBUG" /NOLOGO
MRC_PROJ=/D "_68K_" /D "_MAC" /D "NDEBUG" /l 0x409 /NOLOGO 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/modeless.bsc" 
BSC32_SBRS= \
	"$(INTDIR)/modeldlg.sbr" \
	"$(INTDIR)/stdafx.sbr" \
	"$(INTDIR)/adderdlg.sbr" \
	"$(INTDIR)/modeless.sbr"

"$(OUTDIR)\modeless.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 /nologo /MAC:bundle /machine:I386
# ADD LINK32 /nologo /MAC:bundle /machine:M68K
# SUBTRACT LINK32 /pdb:none
LINK32_FLAGS=/nologo /MAC:bundle /MAC:type="APPL" /MAC:creator="????"\
 /pdb:"$(OUTDIR)/modeless.pdb" /machine:M68K /out:"$(OUTDIR)/modeless.exe" 
LINK32_OBJS= \
	"$(INTDIR)/modeldlg.obj" \
	"$(INTDIR)/stdafx.obj" \
	"$(INTDIR)/adderdlg.obj" \
	"$(INTDIR)/modeless.obj" \
	"$(INTDIR)/modeless.rsc" \
	".\M68KRel\mdlsmac.rsc"

"$(OUTDIR)\modeless.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

MFILE32=mfile.exe
# ADD BASE MFILE32 COPY /NOLOGO
# ADD MFILE32 COPY /NOLOGO
MFILE32_FLAGS=COPY /NOLOGO 
MFILE32_FILES= \
	"$(OUTDIR)/modeless.exe"

"$(OUTDIR)\modeless.trg" : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\MacRel\modeless.exe\
 "$(MFILE32_DEST):modeless.exe">"$(OUTDIR)\modeless.trg"

DOWNLOAD : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\MacRel\modeless.exe\
 "$(MFILE32_DEST):modeless.exe">"$(OUTDIR)\modeless.trg"

!ENDIF 

################################################################################
# Begin Target

# Name "modeless - Win32 Release"
# Name "modeless - Win32 Debug"
# Name "modeless - Power Macintosh Debug"
# Name "modeless - Power Macintosh Release"
# Name "modeless - Macintosh Debug"
# Name "modeless - Macintosh Release"

!IF  "$(CFG)" == "modeless - Win32 Release"

!ELSEIF  "$(CFG)" == "modeless - Win32 Debug"

!ELSEIF  "$(CFG)" == "modeless - Power Macintosh Debug"

!ELSEIF  "$(CFG)" == "modeless - Power Macintosh Release"

!ELSEIF  "$(CFG)" == "modeless - Macintosh Debug"

!ELSEIF  "$(CFG)" == "modeless - Macintosh Release"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\readme.txt

!IF  "$(CFG)" == "modeless - Win32 Release"

!ELSEIF  "$(CFG)" == "modeless - Win32 Debug"

!ELSEIF  "$(CFG)" == "modeless - Power Macintosh Debug"

!ELSEIF  "$(CFG)" == "modeless - Power Macintosh Release"

!ELSEIF  "$(CFG)" == "modeless - Macintosh Debug"

!ELSEIF  "$(CFG)" == "modeless - Macintosh Release"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\modeless.cpp

!IF  "$(CFG)" == "modeless - Win32 Release"

DEP_CPP_MODEL=\
	".\stdafx.h"\
	".\modeless.h"\
	".\adderdlg.h"\
	".\modeldlg.h"\
	

"$(INTDIR)\modeless.obj" : $(SOURCE) $(DEP_CPP_MODEL) "$(INTDIR)"\
 "$(INTDIR)\modeless.pch"

"$(INTDIR)\modeless.sbr" : $(SOURCE) $(DEP_CPP_MODEL) "$(INTDIR)"\
 "$(INTDIR)\modeless.pch"


!ELSEIF  "$(CFG)" == "modeless - Win32 Debug"

DEP_CPP_MODEL=\
	".\adderdlg.h"\
	".\modeldlg.h"\
	".\modeless.h"\
	".\stdafx.h"\
	

"$(INTDIR)\modeless.obj" : $(SOURCE) $(DEP_CPP_MODEL) "$(INTDIR)"\
 "$(INTDIR)\modeless.pch"

"$(INTDIR)\modeless.sbr" : $(SOURCE) $(DEP_CPP_MODEL) "$(INTDIR)"\
 "$(INTDIR)\modeless.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_MODEL) "$(INTDIR)"\
 "$(INTDIR)\modeless.pch"


!ELSEIF  "$(CFG)" == "modeless - Power Macintosh Debug"

DEP_CPP_MODEL=\
	".\stdafx.h"\
	".\modeless.h"\
	".\adderdlg.h"\
	".\modeldlg.h"\
	

"$(INTDIR)\modeless.obj" : $(SOURCE) $(DEP_CPP_MODEL) "$(INTDIR)"\
 "$(INTDIR)\modeless.pch"

"$(INTDIR)\modeless.sbr" : $(SOURCE) $(DEP_CPP_MODEL) "$(INTDIR)"\
 "$(INTDIR)\modeless.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_MODEL) "$(INTDIR)"\
 "$(INTDIR)\modeless.pch"


!ELSEIF  "$(CFG)" == "modeless - Power Macintosh Release"

DEP_CPP_MODEL=\
	".\stdafx.h"\
	".\modeless.h"\
	".\adderdlg.h"\
	".\modeldlg.h"\
	

"$(INTDIR)\modeless.obj" : $(SOURCE) $(DEP_CPP_MODEL) "$(INTDIR)"\
 "$(INTDIR)\modeless.pch"

"$(INTDIR)\modeless.sbr" : $(SOURCE) $(DEP_CPP_MODEL) "$(INTDIR)"\
 "$(INTDIR)\modeless.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_MODEL) "$(INTDIR)"\
 "$(INTDIR)\modeless.pch"


!ELSEIF  "$(CFG)" == "modeless - Macintosh Debug"

DEP_CPP_MODEL=\
	".\stdafx.h"\
	".\modeless.h"\
	".\adderdlg.h"\
	".\modeldlg.h"\
	

"$(INTDIR)\modeless.obj" : $(SOURCE) $(DEP_CPP_MODEL) "$(INTDIR)"\
 "$(INTDIR)\modeless.pch"

"$(INTDIR)\modeless.sbr" : $(SOURCE) $(DEP_CPP_MODEL) "$(INTDIR)"\
 "$(INTDIR)\modeless.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_MODEL) "$(INTDIR)"\
 "$(INTDIR)\modeless.pch"


!ELSEIF  "$(CFG)" == "modeless - Macintosh Release"

DEP_CPP_MODEL=\
	".\stdafx.h"\
	".\modeless.h"\
	".\adderdlg.h"\
	".\modeldlg.h"\
	

"$(INTDIR)\modeless.obj" : $(SOURCE) $(DEP_CPP_MODEL) "$(INTDIR)"\
 "$(INTDIR)\modeless.pch"

"$(INTDIR)\modeless.sbr" : $(SOURCE) $(DEP_CPP_MODEL) "$(INTDIR)"\
 "$(INTDIR)\modeless.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\modeldlg.cpp

!IF  "$(CFG)" == "modeless - Win32 Release"

DEP_CPP_MODELD=\
	".\adderdlg.h"\
	".\modeldlg.h"\
	".\modeless.h"\
	".\stdafx.h"\
	

"$(INTDIR)\modeldlg.obj" : $(SOURCE) $(DEP_CPP_MODELD) "$(INTDIR)"\
 "$(INTDIR)\modeless.pch"

"$(INTDIR)\modeldlg.sbr" : $(SOURCE) $(DEP_CPP_MODELD) "$(INTDIR)"\
 "$(INTDIR)\modeless.pch"


!ELSEIF  "$(CFG)" == "modeless - Win32 Debug"

DEP_CPP_MODELD=\
	".\adderdlg.h"\
	".\modeldlg.h"\
	".\modeless.h"\
	".\stdafx.h"\
	

"$(INTDIR)\modeldlg.obj" : $(SOURCE) $(DEP_CPP_MODELD) "$(INTDIR)"\
 "$(INTDIR)\modeless.pch"

"$(INTDIR)\modeldlg.sbr" : $(SOURCE) $(DEP_CPP_MODELD) "$(INTDIR)"\
 "$(INTDIR)\modeless.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_MODELD) "$(INTDIR)"\
 "$(INTDIR)\modeless.pch"


!ELSEIF  "$(CFG)" == "modeless - Power Macintosh Debug"

DEP_CPP_MODELD=\
	".\stdafx.h"\
	".\modeless.h"\
	".\adderdlg.h"\
	".\modeldlg.h"\
	

"$(INTDIR)\modeldlg.obj" : $(SOURCE) $(DEP_CPP_MODELD) "$(INTDIR)"\
 "$(INTDIR)\modeless.pch"

"$(INTDIR)\modeldlg.sbr" : $(SOURCE) $(DEP_CPP_MODELD) "$(INTDIR)"\
 "$(INTDIR)\modeless.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_MODELD) "$(INTDIR)"\
 "$(INTDIR)\modeless.pch"


!ELSEIF  "$(CFG)" == "modeless - Power Macintosh Release"

DEP_CPP_MODELD=\
	".\stdafx.h"\
	".\modeless.h"\
	".\adderdlg.h"\
	".\modeldlg.h"\
	

"$(INTDIR)\modeldlg.obj" : $(SOURCE) $(DEP_CPP_MODELD) "$(INTDIR)"\
 "$(INTDIR)\modeless.pch"

"$(INTDIR)\modeldlg.sbr" : $(SOURCE) $(DEP_CPP_MODELD) "$(INTDIR)"\
 "$(INTDIR)\modeless.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_MODELD) "$(INTDIR)"\
 "$(INTDIR)\modeless.pch"


!ELSEIF  "$(CFG)" == "modeless - Macintosh Debug"

DEP_CPP_MODELD=\
	".\stdafx.h"\
	".\modeless.h"\
	".\adderdlg.h"\
	".\modeldlg.h"\
	

"$(INTDIR)\modeldlg.obj" : $(SOURCE) $(DEP_CPP_MODELD) "$(INTDIR)"\
 "$(INTDIR)\modeless.pch"

"$(INTDIR)\modeldlg.sbr" : $(SOURCE) $(DEP_CPP_MODELD) "$(INTDIR)"\
 "$(INTDIR)\modeless.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_MODELD) "$(INTDIR)"\
 "$(INTDIR)\modeless.pch"


!ELSEIF  "$(CFG)" == "modeless - Macintosh Release"

DEP_CPP_MODELD=\
	".\stdafx.h"\
	".\modeless.h"\
	".\adderdlg.h"\
	".\modeldlg.h"\
	

"$(INTDIR)\modeldlg.obj" : $(SOURCE) $(DEP_CPP_MODELD) "$(INTDIR)"\
 "$(INTDIR)\modeless.pch"

"$(INTDIR)\modeldlg.sbr" : $(SOURCE) $(DEP_CPP_MODELD) "$(INTDIR)"\
 "$(INTDIR)\modeless.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\stdafx.cpp
DEP_CPP_STDAF=\
	".\stdafx.h"\
	

!IF  "$(CFG)" == "modeless - Win32 Release"

# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /FR"$(INTDIR)/" /Fp"$(INTDIR)/modeless.pch" /Yc"stdafx.h"\
 /Fo"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\stdafx.sbr" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\modeless.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "modeless - Win32 Debug"

# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MDd /W3 /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /FR"$(INTDIR)/" /Fp"$(INTDIR)/modeless.pch" /Yc"stdafx.h"\
 /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\stdafx.sbr" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\modeless.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "modeless - Power Macintosh Debug"

# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MDd /W3 /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D\
 "_MBCS" /D "_AFXDLL" /D "_MAC" /D "_MPPC_" /FR"$(INTDIR)/"\
 /Fp"$(INTDIR)/modeless.pch" /Yc"stdafx.h" /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c\
 $(SOURCE) \
	

"$(INTDIR)\stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\stdafx.sbr" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\modeless.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "modeless - Power Macintosh Release"

# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MD /W3 /GX /Zi /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D\
 "_MBCS" /D "_AFXDLL" /D "_MAC" /D "_MPPC_" /FR"$(INTDIR)/"\
 /Fp"$(INTDIR)/modeless.pch" /Yc"stdafx.h" /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c\
 $(SOURCE) \
	

"$(INTDIR)\stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\stdafx.sbr" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\modeless.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "modeless - Macintosh Debug"

# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /AL /Q68s /Q68m /W3 /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D\
 "_WINDOWS" /D "_MAC" /D "_68K_" /D "_MBCS" /FR"$(INTDIR)/"\
 /Fp"$(INTDIR)/modeless.pch" /Yc"stdafx.h" /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c\
 $(SOURCE) \
	

"$(INTDIR)\stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\stdafx.sbr" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\modeless.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "modeless - Macintosh Release"

# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /AL /Q68s /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D\
 "_MAC" /D "_68K_" /D "_MBCS" /FR"$(INTDIR)/" /Fp"$(INTDIR)/modeless.pch"\
 /Yc"stdafx.h" /Fo"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\stdafx.sbr" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\modeless.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\modeless.rc
DEP_RSC_MODELE=\
	".\res\modeless.ico"\
	".\res\modeless.rc2"\
	

!IF  "$(CFG)" == "modeless - Win32 Release"


"$(INTDIR)\modeless.res" : $(SOURCE) $(DEP_RSC_MODELE) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "modeless - Win32 Debug"


"$(INTDIR)\modeless.res" : $(SOURCE) $(DEP_RSC_MODELE) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "modeless - Power Macintosh Debug"


"$(INTDIR)\modeless.rsc" : $(SOURCE) $(DEP_RSC_MODELE) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "modeless - Power Macintosh Release"


"$(INTDIR)\modeless.rsc" : $(SOURCE) $(DEP_RSC_MODELE) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "modeless - Macintosh Debug"


"$(INTDIR)\modeless.rsc" : $(SOURCE) $(DEP_RSC_MODELE) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "modeless - Macintosh Release"


"$(INTDIR)\modeless.rsc" : $(SOURCE) $(DEP_RSC_MODELE) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\adderdlg.cpp

!IF  "$(CFG)" == "modeless - Win32 Release"

DEP_CPP_ADDER=\
	".\adderdlg.h"\
	".\modeldlg.h"\
	".\modeless.h"\
	".\stdafx.h"\
	

"$(INTDIR)\adderdlg.obj" : $(SOURCE) $(DEP_CPP_ADDER) "$(INTDIR)"\
 "$(INTDIR)\modeless.pch"

"$(INTDIR)\adderdlg.sbr" : $(SOURCE) $(DEP_CPP_ADDER) "$(INTDIR)"\
 "$(INTDIR)\modeless.pch"


!ELSEIF  "$(CFG)" == "modeless - Win32 Debug"

DEP_CPP_ADDER=\
	".\adderdlg.h"\
	".\modeldlg.h"\
	".\modeless.h"\
	".\stdafx.h"\
	

"$(INTDIR)\adderdlg.obj" : $(SOURCE) $(DEP_CPP_ADDER) "$(INTDIR)"\
 "$(INTDIR)\modeless.pch"

"$(INTDIR)\adderdlg.sbr" : $(SOURCE) $(DEP_CPP_ADDER) "$(INTDIR)"\
 "$(INTDIR)\modeless.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_ADDER) "$(INTDIR)"\
 "$(INTDIR)\modeless.pch"


!ELSEIF  "$(CFG)" == "modeless - Power Macintosh Debug"

DEP_CPP_ADDER=\
	".\stdafx.h"\
	".\modeless.h"\
	".\adderdlg.h"\
	".\modeldlg.h"\
	

"$(INTDIR)\adderdlg.obj" : $(SOURCE) $(DEP_CPP_ADDER) "$(INTDIR)"\
 "$(INTDIR)\modeless.pch"

"$(INTDIR)\adderdlg.sbr" : $(SOURCE) $(DEP_CPP_ADDER) "$(INTDIR)"\
 "$(INTDIR)\modeless.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_ADDER) "$(INTDIR)"\
 "$(INTDIR)\modeless.pch"


!ELSEIF  "$(CFG)" == "modeless - Power Macintosh Release"

DEP_CPP_ADDER=\
	".\stdafx.h"\
	".\modeless.h"\
	".\adderdlg.h"\
	".\modeldlg.h"\
	

"$(INTDIR)\adderdlg.obj" : $(SOURCE) $(DEP_CPP_ADDER) "$(INTDIR)"\
 "$(INTDIR)\modeless.pch"

"$(INTDIR)\adderdlg.sbr" : $(SOURCE) $(DEP_CPP_ADDER) "$(INTDIR)"\
 "$(INTDIR)\modeless.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_ADDER) "$(INTDIR)"\
 "$(INTDIR)\modeless.pch"


!ELSEIF  "$(CFG)" == "modeless - Macintosh Debug"

DEP_CPP_ADDER=\
	".\stdafx.h"\
	".\modeless.h"\
	".\adderdlg.h"\
	".\modeldlg.h"\
	

"$(INTDIR)\adderdlg.obj" : $(SOURCE) $(DEP_CPP_ADDER) "$(INTDIR)"\
 "$(INTDIR)\modeless.pch"

"$(INTDIR)\adderdlg.sbr" : $(SOURCE) $(DEP_CPP_ADDER) "$(INTDIR)"\
 "$(INTDIR)\modeless.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_ADDER) "$(INTDIR)"\
 "$(INTDIR)\modeless.pch"


!ELSEIF  "$(CFG)" == "modeless - Macintosh Release"

DEP_CPP_ADDER=\
	".\stdafx.h"\
	".\modeless.h"\
	".\adderdlg.h"\
	".\modeldlg.h"\
	

"$(INTDIR)\adderdlg.obj" : $(SOURCE) $(DEP_CPP_ADDER) "$(INTDIR)"\
 "$(INTDIR)\modeless.pch"

"$(INTDIR)\adderdlg.sbr" : $(SOURCE) $(DEP_CPP_ADDER) "$(INTDIR)"\
 "$(INTDIR)\modeless.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\modemac.r

!IF  "$(CFG)" == "modeless - Win32 Release"

!ELSEIF  "$(CFG)" == "modeless - Win32 Debug"

!ELSEIF  "$(CFG)" == "modeless - Power Macintosh Debug"

DEP_MRC_MODEM=\
	{$(INCLUDE)}"\mrc\balloons.r"\
	
# ADD MRC /o"PMacDbg/mdlsmac.rsc"

"$(INTDIR)\mdlsmac.rsc" : $(SOURCE) $(DEP_MRC_MODEM) "$(INTDIR)"
   $(MRC) /o"$(INTDIR)/mdlsmac.rsc" /D "_MPPC_" /D "_MAC" /D "_DEBUG" /l 0x409\
 /NOLOGO $(SOURCE)


!ELSEIF  "$(CFG)" == "modeless - Power Macintosh Release"

DEP_MRC_MODEM=\
	{$(INCLUDE)}"\mrc\balloons.r"\
	
# ADD MRC /o"PMacRel/mdlsmac.rsc"

"$(INTDIR)\mdlsmac.rsc" : $(SOURCE) $(DEP_MRC_MODEM) "$(INTDIR)"
   $(MRC) /o"$(INTDIR)/mdlsmac.rsc" /D "_MPPC_" /D "_MAC" /D "NDEBUG" /l 0x409\
 /NOLOGO $(SOURCE)


!ELSEIF  "$(CFG)" == "modeless - Macintosh Debug"

DEP_MRC_MODEM=\
	{$(INCLUDE)}"\mrc\balloons.r"\
	
# ADD MRC /o"M68KDbg/mdlsmac.rsc"

".\M68KDbg\mdlsmac.rsc" : $(SOURCE) $(DEP_MRC_MODEM) "$(INTDIR)"
   $(MRC) /o"M68KDbg/mdlsmac.rsc" /D "_68K_" /D "_MAC" /D "_DEBUG" /l 0x409\
 /NOLOGO $(SOURCE)


!ELSEIF  "$(CFG)" == "modeless - Macintosh Release"

DEP_MRC_MODEM=\
	{$(INCLUDE)}"\mrc\balloons.r"\
	
# ADD MRC /o"M68KRel/mdlsmac.rsc"

".\M68KRel\mdlsmac.rsc" : $(SOURCE) $(DEP_MRC_MODEM) "$(INTDIR)"
   $(MRC) /o"M68KRel/mdlsmac.rsc" /D "_68K_" /D "_MAC" /D "NDEBUG" /l 0x409\
 /NOLOGO $(SOURCE)


!ENDIF 

# End Source File
# End Target
# End Project
################################################################################
