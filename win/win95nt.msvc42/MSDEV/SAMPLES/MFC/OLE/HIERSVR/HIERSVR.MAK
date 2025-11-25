# Microsoft Visual C++ Generated NMAKE File, Format Version 40000
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101
# TARGTYPE "Power Macintosh Application" 0x0401
# TARGTYPE "Macintosh Application" 0x0301

!IF "$(CFG)" == ""
CFG=HierSvr - Win32 Debug
!MESSAGE No configuration specified.  Defaulting to HierSvr - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "HierSvr - Win32 Release" && "$(CFG)" !=\
 "HierSvr - Win32 Debug" && "$(CFG)" != "HierSvr - Macintosh Release" &&\
 "$(CFG)" != "HierSvr - Macintosh Debug" && "$(CFG)" !=\
 "HierSvr - Power Macintosh Release" && "$(CFG)" !=\
 "HierSvr - Power Macintosh Debug" && "$(CFG)" !=\
 "HierSvr - Win32 Unicode Debug" && "$(CFG)" !=\
 "HierSvr - Win32 Unicode Release"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "hiersvr.mak" CFG="HierSvr - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "HierSvr - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "HierSvr - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE "HierSvr - Macintosh Release" (based on "Macintosh Application")
!MESSAGE "HierSvr - Macintosh Debug" (based on "Macintosh Application")
!MESSAGE "HierSvr - Power Macintosh Release" (based on\
 "Power Macintosh Application")
!MESSAGE "HierSvr - Power Macintosh Debug" (based on\
 "Power Macintosh Application")
!MESSAGE "HierSvr - Win32 Unicode Debug" (based on "Win32 (x86) Application")
!MESSAGE "HierSvr - Win32 Unicode Release" (based on "Win32 (x86) Application")
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
# PROP Target_Last_Scanned "HierSvr - Win32 Unicode Release"

!IF  "$(CFG)" == "HierSvr - Win32 Release"

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

ALL : "$(OUTDIR)\hiersvr.exe" "$(OUTDIR)\hiersvr.HLP"

CLEAN : 
	-@erase ".\Release\hiersvr.exe"
	-@erase ".\Release\hiersvr.obj"
	-@erase ".\Release\hiersvr.pch"
	-@erase ".\Release\svritem.obj"
	-@erase ".\Release\mainfrm.obj"
	-@erase ".\Release\zoomdlg.obj"
	-@erase ".\Release\ipframe.obj"
	-@erase ".\Release\svrdoc.obj"
	-@erase ".\Release\svrview.obj"
	-@erase ".\Release\stdafx.obj"
	-@erase ".\Release\hiersvr.res"
	-@erase ".\Release\hiersvr.HLP"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /FR /YX /c
# ADD CPP /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
CPP_PROJ=/MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D\
 "_MBCS" /Fp"$(INTDIR)/hiersvr.pch" /Yu"stdafx.h" /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\Release/
CPP_SBRS=

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
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/hiersvr.res" /d "NDEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/hiersvr.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 /nologo /subsystem:windows /machine:I386
LINK32_FLAGS=/nologo /subsystem:windows /incremental:no\
 /pdb:"$(OUTDIR)/hiersvr.pdb" /machine:I386 /out:"$(OUTDIR)/hiersvr.exe" 
LINK32_OBJS= \
	"$(INTDIR)/hiersvr.obj" \
	"$(INTDIR)/svritem.obj" \
	"$(INTDIR)/mainfrm.obj" \
	"$(INTDIR)/zoomdlg.obj" \
	"$(INTDIR)/ipframe.obj" \
	"$(INTDIR)/svrdoc.obj" \
	"$(INTDIR)/svrview.obj" \
	"$(INTDIR)/stdafx.obj" \
	"$(INTDIR)/hiersvr.res"

"$(OUTDIR)\hiersvr.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "HierSvr - Win32 Debug"

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

ALL : "$(OUTDIR)\hiersvr.exe" "$(OUTDIR)\hiersvr.HLP"

CLEAN : 
	-@erase ".\Debug\vc40.pdb"
	-@erase ".\Debug\hiersvr.pch"
	-@erase ".\Debug\hiersvr.exe"
	-@erase ".\Debug\svrview.obj"
	-@erase ".\Debug\hiersvr.obj"
	-@erase ".\Debug\svritem.obj"
	-@erase ".\Debug\mainfrm.obj"
	-@erase ".\Debug\svrdoc.obj"
	-@erase ".\Debug\stdafx.obj"
	-@erase ".\Debug\zoomdlg.obj"
	-@erase ".\Debug\ipframe.obj"
	-@erase ".\Debug\hiersvr.res"
	-@erase ".\Debug\hiersvr.ilk"
	-@erase ".\Debug\hiersvr.pdb"
	-@erase ".\Debug\hiersvr.HLP"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /FR /YX /c
# ADD CPP /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
CPP_PROJ=/MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/hiersvr.pch" /Yu"stdafx.h" /Fo"$(INTDIR)/"\
 /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\Debug/
CPP_SBRS=

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
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/hiersvr.res" /d "_DEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/hiersvr.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386
# ADD LINK32 /nologo /subsystem:windows /debug /machine:I386
LINK32_FLAGS=/nologo /subsystem:windows /incremental:yes\
 /pdb:"$(OUTDIR)/hiersvr.pdb" /debug /machine:I386 /out:"$(OUTDIR)/hiersvr.exe" 
LINK32_OBJS= \
	"$(INTDIR)/svrview.obj" \
	"$(INTDIR)/hiersvr.obj" \
	"$(INTDIR)/svritem.obj" \
	"$(INTDIR)/mainfrm.obj" \
	"$(INTDIR)/svrdoc.obj" \
	"$(INTDIR)/stdafx.obj" \
	"$(INTDIR)/zoomdlg.obj" \
	"$(INTDIR)/ipframe.obj" \
	"$(INTDIR)/hiersvr.res"

"$(OUTDIR)\hiersvr.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "HierSvr - Macintosh Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir MacRel
# PROP BASE Intermediate_Dir MacRel
# PROP BASE Target_Dir
# PROP Use_MFC 1
# PROP Use_Debug_Libraries 0
# PROP Output_Dir MacRel
# PROP Intermediate_Dir MacRel
# PROP Target_Dir
OUTDIR=.\MacRel
INTDIR=.\MacRel

ALL : "$(OUTDIR)\hiersvr.exe" "$(OUTDIR)\hiersvr.trg" "$(OUTDIR)\hiersvr.HLP"

CLEAN : 
	-@erase ".\MacRel\hiersvr.trg"
	-@erase ".\MacRel\hiersvr.exe"
	-@erase ".\MacRel\hiersvr.obj"
	-@erase ".\MacRel\hiersvr.pch"
	-@erase ".\MacRel\svritem.obj"
	-@erase ".\MacRel\mainfrm.obj"
	-@erase ".\MacRel\zoomdlg.obj"
	-@erase ".\MacRel\svrdoc.obj"
	-@erase ".\MacRel\stdafx.obj"
	-@erase ".\MacRel\ipframe.obj"
	-@erase ".\MacRel\svrview.obj"
	-@erase ".\MacRel\hiersvr.rsc"
	-@erase ".\MacRel\hiermac.rsc"
	-@erase ".\MacRel\hiersvr.HLP"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

MTL=mktyplib.exe
# ADD BASE MTL /nologo /D "NDEBUG" /mac
# ADD MTL /nologo /D "NDEBUG" /mac
MTL_PROJ=/nologo /D "NDEBUG" /mac 
CPP=cl.exe
# ADD BASE CPP /nologo /AL /Q68s /W3 /GX /O2 /D "_WINDOWS" /D "_MAC" /D "_68K_" /D "WIN32" /D "NDEBUG" /FR /YX /c
# ADD CPP /AL /Q68s /W3 /GX /O2 /D "_WINDOWS" /D "_MAC" /D "_68K_" /D "WIN32" /D "NDEBUG" /D "_MBCS" /Yu"stdafx.h" /c
CPP_PROJ=/AL /Q68s /W3 /GX /O2 /D "_WINDOWS" /D "_MAC" /D "_68K_" /D "WIN32" /D\
 "NDEBUG" /D "_MBCS" /Fp"$(INTDIR)/hiersvr.pch" /Yu"stdafx.h" /Fo"$(INTDIR)/" /c\
 
CPP_OBJS=.\MacRel/
CPP_SBRS=

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
# ADD BASE RSC /l 0x409 /r /d "_MAC" /d "_68K_" /d "NDEBUG"
# ADD RSC /l 0x409 /r /d "_MAC" /d "_68K_" /d "NDEBUG"
RSC_PROJ=/l 0x409 /r /m /fo"$(INTDIR)/hiersvr.rsc" /d "_MAC" /d "_68K_" /d\
 "NDEBUG" 
MRC=mrc.exe
# ADD BASE MRC /D "_68K_" /D "_MAC" /D "NDEBUG" /NOLOGO
# ADD MRC /D "_68K_" /D "_MAC" /D "NDEBUG" /NOLOGO
MRC_PROJ=/D "_68K_" /D "_MAC" /D "NDEBUG" /l 0x409 /NOLOGO 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/hiersvr.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 wlm.lib /nologo /MAC:bundle /machine:M68K
# ADD LINK32 /nologo /MAC:bundle /MAC:creator="HIER" /machine:M68K
LINK32_FLAGS=/nologo /MAC:bundle /MAC:type="APPL" /MAC:creator="HIER"\
 /pdb:"$(OUTDIR)/hiersvr.pdb" /machine:M68K /out:"$(OUTDIR)/hiersvr.exe" 
LINK32_OBJS= \
	"$(INTDIR)/hiersvr.obj" \
	"$(INTDIR)/svritem.obj" \
	"$(INTDIR)/mainfrm.obj" \
	"$(INTDIR)/zoomdlg.obj" \
	"$(INTDIR)/svrdoc.obj" \
	"$(INTDIR)/stdafx.obj" \
	"$(INTDIR)/ipframe.obj" \
	"$(INTDIR)/svrview.obj" \
	"$(INTDIR)/hiersvr.rsc" \
	"$(INTDIR)/hiermac.rsc"

"$(OUTDIR)\hiersvr.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

MFILE32=mfile.exe
# ADD BASE MFILE32 COPY /NOLOGO
# ADD MFILE32 COPY /NOLOGO
MFILE32_FLAGS=COPY /NOLOGO 
MFILE32_FILES= \
	"$(OUTDIR)/hiersvr.exe"

"$(OUTDIR)\hiersvr.trg" : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\MacRel\hiersvr.exe\
 "$(MFILE32_DEST)":hiersvr.exe

DOWNLOAD : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\MacRel\hiersvr.exe\
 "$(MFILE32_DEST)":hiersvr.exe

!ELSEIF  "$(CFG)" == "HierSvr - Macintosh Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir MacDbg
# PROP BASE Intermediate_Dir MacDbg
# PROP BASE Target_Dir
# PROP Use_MFC 1
# PROP Use_Debug_Libraries 1
# PROP Output_Dir MacDbg
# PROP Intermediate_Dir MacDbg
# PROP Target_Dir
OUTDIR=.\MacDbg
INTDIR=.\MacDbg

ALL : "$(OUTDIR)\hiersvr.exe" "$(OUTDIR)\hiersvr.trg" "$(OUTDIR)\hiersvr.HLP"

CLEAN : 
	-@erase ".\MacDbg\vc40.pdb"
	-@erase ".\MacDbg\hiersvr.pch"
	-@erase ".\MacDbg\hiersvr.pdb"
	-@erase ".\MacDbg\svrview.obj"
	-@erase ".\MacDbg\hiersvr.obj"
	-@erase ".\MacDbg\svrdoc.obj"
	-@erase ".\MacDbg\svritem.obj"
	-@erase ".\MacDbg\stdafx.obj"
	-@erase ".\MacDbg\mainfrm.obj"
	-@erase ".\MacDbg\zoomdlg.obj"
	-@erase ".\MacDbg\ipframe.obj"
	-@erase ".\MacDbg\hiersvr.rsc"
	-@erase ".\MacDbg\hiermac.rsc"
	-@erase ".\MacDbg\hiersvr.trg"
	-@erase ".\MacDbg\hiersvr.exe"
	-@erase ".\MacDbg\hiersvr.HLP"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

MTL=mktyplib.exe
# ADD BASE MTL /nologo /D "_DEBUG" /mac
# ADD MTL /nologo /D "_DEBUG" /mac
MTL_PROJ=/nologo /D "_DEBUG" /mac 
CPP=cl.exe
# ADD BASE CPP /nologo /AL /Q68s /Q68m /W3 /GX /Zi /Od /D "_WINDOWS" /D "_MAC" /D "_68K_" /D "WIN32" /D "_DEBUG" /FR /YX /c
# ADD CPP /AL /Q68s /Q68m /W3 /GX /Zi /Od /D "_WINDOWS" /D "_MAC" /D "_68K_" /D "WIN32" /D "_DEBUG" /D "_MBCS" /Yu"stdafx.h" /c
CPP_PROJ=/AL /Q68s /Q68m /W3 /GX /Zi /Od /D "_WINDOWS" /D "_MAC" /D "_68K_" /D\
 "WIN32" /D "_DEBUG" /D "_MBCS" /Fp"$(INTDIR)/hiersvr.pch" /Yu"stdafx.h"\
 /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\MacDbg/
CPP_SBRS=

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
# ADD BASE RSC /l 0x409 /r /d "_MAC" /d "_68K_" /d "_DEBUG"
# ADD RSC /l 0x409 /r /d "_MAC" /d "_68K_" /d "_DEBUG"
RSC_PROJ=/l 0x409 /r /m /fo"$(INTDIR)/hiersvr.rsc" /d "_MAC" /d "_68K_" /d\
 "_DEBUG" 
MRC=mrc.exe
# ADD BASE MRC /D "_68K_" /D "_MAC" /D "_DEBUG" /NOLOGO
# ADD MRC /D "_68K_" /D "_MAC" /D "_DEBUG" /NOLOGO
MRC_PROJ=/D "_68K_" /D "_MAC" /D "_DEBUG" /l 0x409 /NOLOGO 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/hiersvr.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 wlm.lib /nologo /MAC:bundle /debug /machine:M68K
# ADD LINK32 /nologo /MAC:bundle /MAC:creator="HIER" /debug /machine:M68K
LINK32_FLAGS=/nologo /MAC:bundle /MAC:type="APPL" /MAC:creator="HIER"\
 /pdb:"$(OUTDIR)/hiersvr.pdb" /debug /machine:M68K /out:"$(OUTDIR)/hiersvr.exe" 
LINK32_OBJS= \
	"$(INTDIR)/svrview.obj" \
	"$(INTDIR)/hiersvr.obj" \
	"$(INTDIR)/svrdoc.obj" \
	"$(INTDIR)/svritem.obj" \
	"$(INTDIR)/stdafx.obj" \
	"$(INTDIR)/mainfrm.obj" \
	"$(INTDIR)/zoomdlg.obj" \
	"$(INTDIR)/ipframe.obj" \
	"$(INTDIR)/hiersvr.rsc" \
	"$(INTDIR)/hiermac.rsc"

"$(OUTDIR)\hiersvr.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

MFILE32=mfile.exe
# ADD BASE MFILE32 COPY /NOLOGO
# ADD MFILE32 COPY /NOLOGO
MFILE32_FLAGS=COPY /NOLOGO 
MFILE32_FILES= \
	"$(OUTDIR)/hiersvr.exe"

"$(OUTDIR)\hiersvr.trg" : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\MacDbg\hiersvr.exe\
 "$(MFILE32_DEST)":hiersvr.exe

DOWNLOAD : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\MacDbg\hiersvr.exe\
 "$(MFILE32_DEST)":hiersvr.exe

!ELSEIF  "$(CFG)" == "HierSvr - Power Macintosh Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir PMacRel
# PROP BASE Intermediate_Dir PMacRel
# PROP BASE Target_Dir
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 0
# PROP Output_Dir PMacRel
# PROP Intermediate_Dir PMacRel
# PROP Target_Dir
OUTDIR=.\PMacRel
INTDIR=.\PMacRel

ALL : "$(OUTDIR)\hiersvr.exe" "$(OUTDIR)\hiersvr.trg" "$(OUTDIR)\hiersvr.HLP"

CLEAN : 
	-@erase ".\PMacRel\hiersvr.trg"
	-@erase ".\PMacRel\hiersvr.exe"
	-@erase ".\PMacRel\mainfrm.obj"
	-@erase ".\PMacRel\hiersvr.pch"
	-@erase ".\PMacRel\zoomdlg.obj"
	-@erase ".\PMacRel\ipframe.obj"
	-@erase ".\PMacRel\svrdoc.obj"
	-@erase ".\PMacRel\stdafx.obj"
	-@erase ".\PMacRel\svrview.obj"
	-@erase ".\PMacRel\hiersvr.obj"
	-@erase ".\PMacRel\svritem.obj"
	-@erase ".\PMacRel\hiersvr.rsc"
	-@erase ".\PMacRel\hiermac.rsc"
	-@erase ".\PMacRel\hiersvr.HLP"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

MTL=mktyplib.exe
# ADD BASE MTL /nologo /D "NDEBUG"
# ADD MTL /nologo /D "NDEBUG"
MTL_PROJ=/nologo /D "NDEBUG" 
CPP=cl.exe
# ADD BASE CPP /nologo /ML /W3 /GX /O2 /D "_MAC" /D "_MPPC_" /D "_WINDOWS" /D "WIN32" /D "NDEBUG" /FR /YX /c
# ADD CPP /MD /W3 /GX /O2 /D "NDEBUG" /D "_MAC" /D "_MPPC_" /D "_WINDOWS" /D "WIN32" /D "_MBCS" /D "_AFXDLL" /Yu"stdafx.h" /c
CPP_PROJ=/MD /W3 /GX /O2 /D "NDEBUG" /D "_MAC" /D "_MPPC_" /D "_WINDOWS" /D\
 "WIN32" /D "_MBCS" /D "_AFXDLL" /Fp"$(INTDIR)/hiersvr.pch" /Yu"stdafx.h"\
 /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\PMacRel/
CPP_SBRS=

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
# ADD BASE RSC /l 0x409 /r /d "_MAC" /d "_MPPC_" /d "NDEBUG"
# ADD RSC /l 0x409 /r /d "_MAC" /d "_MPPC_" /d "NDEBUG"
RSC_PROJ=/l 0x409 /r /m /fo"$(INTDIR)/hiersvr.rsc" /d "_MAC" /d "_MPPC_" /d\
 "NDEBUG" 
MRC=mrc.exe
# ADD BASE MRC /D "_MPPC_" /D "_MAC" /D "NDEBUG" /NOLOGO
# ADD MRC /D "_MPPC_" /D "_MAC" /D "NDEBUG" /NOLOGO
MRC_PROJ=/D "_MPPC_" /D "_MAC" /D "NDEBUG" /l 0x409 /NOLOGO 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/hiersvr.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 cfrg.rsc /nologo /mac:bundle /machine:MPPC
# ADD LINK32 /nologo /mac:bundle /mac:creator="HIER" /machine:MPPC
LINK32_FLAGS=/nologo /mac:bundle /mac:type="APPL" /mac:creator="HIER"\
 /incremental:no /pdb:"$(OUTDIR)/hiersvr.pdb" /machine:MPPC\
 /out:"$(OUTDIR)/hiersvr.exe" 
LINK32_OBJS= \
	"$(INTDIR)/mainfrm.obj" \
	"$(INTDIR)/zoomdlg.obj" \
	"$(INTDIR)/ipframe.obj" \
	"$(INTDIR)/svrdoc.obj" \
	"$(INTDIR)/stdafx.obj" \
	"$(INTDIR)/svrview.obj" \
	"$(INTDIR)/hiersvr.obj" \
	"$(INTDIR)/svritem.obj" \
	"$(INTDIR)/hiersvr.rsc" \
	"$(INTDIR)/hiermac.rsc"

"$(OUTDIR)\hiersvr.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

MFILE32=mfile.exe
# ADD BASE MFILE32 COPY /NOLOGO
# ADD MFILE32 COPY /NOLOGO
MFILE32_FLAGS=COPY /NOLOGO 
MFILE32_FILES= \
	"$(OUTDIR)/hiersvr.exe"

"$(OUTDIR)\hiersvr.trg" : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\PMacRel\hiersvr.exe\
 "$(MFILE32_DEST)":hiersvr.exe

DOWNLOAD : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\PMacRel\hiersvr.exe\
 "$(MFILE32_DEST)":hiersvr.exe

!ELSEIF  "$(CFG)" == "HierSvr - Power Macintosh Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir PMacDbg
# PROP BASE Intermediate_Dir PMacDbg
# PROP BASE Target_Dir
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 1
# PROP Output_Dir PMacDbg
# PROP Intermediate_Dir PMacDbg
# PROP Target_Dir
OUTDIR=.\PMacDbg
INTDIR=.\PMacDbg

ALL : "$(OUTDIR)\hiersvr.exe" "$(OUTDIR)\hiersvr.trg" "$(OUTDIR)\hiersvr.HLP"

CLEAN : 
	-@erase ".\PMacDbg\vc40.pdb"
	-@erase ".\PMacDbg\hiersvr.pch"
	-@erase ".\PMacDbg\hiersvr.ilk"
	-@erase ".\PMacDbg\svrview.obj"
	-@erase ".\PMacDbg\hiersvr.obj"
	-@erase ".\PMacDbg\svritem.obj"
	-@erase ".\PMacDbg\mainfrm.obj"
	-@erase ".\PMacDbg\svrdoc.obj"
	-@erase ".\PMacDbg\stdafx.obj"
	-@erase ".\PMacDbg\zoomdlg.obj"
	-@erase ".\PMacDbg\ipframe.obj"
	-@erase ".\PMacDbg\hiermac.rsc"
	-@erase ".\PMacDbg\hiersvr.rsc"
	-@erase ".\PMacDbg\hiersvr.pdb"
	-@erase ".\PMacDbg\hiersvr.trg"
	-@erase ".\PMacDbg\hiersvr.exe"
	-@erase ".\PMacDbg\hiersvr.HLP"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

MTL=mktyplib.exe
# ADD BASE MTL /nologo /D "_DEBUG"
# ADD MTL /nologo /D "_DEBUG"
MTL_PROJ=/nologo /D "_DEBUG" 
CPP=cl.exe
# ADD BASE CPP /nologo /W3 /GX /Zi /Od /D "_MAC" /D "_MPPC_" /D "_WINDOWS" /D "WIN32" /D "_DEBUG" /FR /YX /c
# ADD CPP /MDd /W3 /GX /Zi /Od /D "_DEBUG" /D "_MAC" /D "_MPPC_" /D "_WINDOWS" /D "WIN32" /D "_MBCS" /D "_AFXDLL" /Yu"stdafx.h" /c
CPP_PROJ=/MDd /W3 /GX /Zi /Od /D "_DEBUG" /D "_MAC" /D "_MPPC_" /D "_WINDOWS"\
 /D "WIN32" /D "_MBCS" /D "_AFXDLL" /Fp"$(INTDIR)/hiersvr.pch" /Yu"stdafx.h"\
 /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\PMacDbg/
CPP_SBRS=

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
# ADD BASE RSC /l 0x409 /r /d "_MAC" /d "_MPPC_" /d "_DEBUG"
# ADD RSC /l 0x409 /r /d "_MAC" /d "_MPPC_" /d "_DEBUG"
RSC_PROJ=/l 0x409 /r /m /fo"$(INTDIR)/hiersvr.rsc" /d "_MAC" /d "_MPPC_" /d\
 "_DEBUG" 
MRC=mrc.exe
# ADD BASE MRC /D "_MPPC_" /D "_MAC" /D "_DEBUG" /NOLOGO
# ADD MRC /D "_MPPC_" /D "_MAC" /D "_DEBUG" /NOLOGO
MRC_PROJ=/D "_MPPC_" /D "_MAC" /D "_DEBUG" /l 0x409 /NOLOGO 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/hiersvr.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 cfrg.rsc /nologo /mac:bundle /debug /machine:MPPC
# ADD LINK32 /nologo /mac:bundle /mac:creator="HIER" /debug /machine:MPPC
LINK32_FLAGS=/nologo /mac:bundle /mac:type="APPL" /mac:creator="HIER"\
 /incremental:yes /pdb:"$(OUTDIR)/hiersvr.pdb" /debug /machine:MPPC\
 /out:"$(OUTDIR)/hiersvr.exe" 
LINK32_OBJS= \
	"$(INTDIR)/svrview.obj" \
	"$(INTDIR)/hiersvr.obj" \
	"$(INTDIR)/svritem.obj" \
	"$(INTDIR)/mainfrm.obj" \
	"$(INTDIR)/svrdoc.obj" \
	"$(INTDIR)/stdafx.obj" \
	"$(INTDIR)/zoomdlg.obj" \
	"$(INTDIR)/ipframe.obj" \
	"$(INTDIR)/hiermac.rsc" \
	"$(INTDIR)/hiersvr.rsc"

"$(OUTDIR)\hiersvr.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

MFILE32=mfile.exe
# ADD BASE MFILE32 COPY /NOLOGO
# ADD MFILE32 COPY /NOLOGO
MFILE32_FLAGS=COPY /NOLOGO 
MFILE32_FILES= \
	"$(OUTDIR)/hiersvr.exe"

"$(OUTDIR)\hiersvr.trg" : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\PMacDbg\hiersvr.exe\
 "$(MFILE32_DEST)":hiersvr.exe

DOWNLOAD : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\PMacDbg\hiersvr.exe\
 "$(MFILE32_DEST)":hiersvr.exe

!ELSEIF  "$(CFG)" == "HierSvr - Win32 Unicode Debug"

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

ALL : "$(OUTDIR)\hiersvr.exe" "$(OUTDIR)\hiersvr.HLP"

CLEAN : 
	-@erase ".\UniDebug\vc40.pdb"
	-@erase ".\UniDebug\hiersvr.pch"
	-@erase ".\UniDebug\hiersvr.exe"
	-@erase ".\UniDebug\svrview.obj"
	-@erase ".\UniDebug\hiersvr.obj"
	-@erase ".\UniDebug\svritem.obj"
	-@erase ".\UniDebug\mainfrm.obj"
	-@erase ".\UniDebug\zoomdlg.obj"
	-@erase ".\UniDebug\ipframe.obj"
	-@erase ".\UniDebug\svrdoc.obj"
	-@erase ".\UniDebug\stdafx.obj"
	-@erase ".\UniDebug\hiersvr.res"
	-@erase ".\UniDebug\hiersvr.ilk"
	-@erase ".\UniDebug\hiersvr.pdb"
	-@erase ".\UniDebug\hiersvr.HLP"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /FR /YX /c
# ADD CPP /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_UNICODE" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
CPP_PROJ=/MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D\
 "_UNICODE" /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/hiersvr.pch" /Yu"stdafx.h"\
 /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\UniDebug/
CPP_SBRS=

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
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/hiersvr.res" /d "_DEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/hiersvr.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386
# ADD LINK32 /nologo /entry:"wWinMainCRTStartup" /subsystem:windows /debug /machine:I386
LINK32_FLAGS=/nologo /entry:"wWinMainCRTStartup" /subsystem:windows\
 /incremental:yes /pdb:"$(OUTDIR)/hiersvr.pdb" /debug /machine:I386\
 /out:"$(OUTDIR)/hiersvr.exe" 
LINK32_OBJS= \
	"$(INTDIR)/svrview.obj" \
	"$(INTDIR)/hiersvr.obj" \
	"$(INTDIR)/svritem.obj" \
	"$(INTDIR)/mainfrm.obj" \
	"$(INTDIR)/zoomdlg.obj" \
	"$(INTDIR)/ipframe.obj" \
	"$(INTDIR)/svrdoc.obj" \
	"$(INTDIR)/stdafx.obj" \
	"$(INTDIR)/hiersvr.res"

"$(OUTDIR)\hiersvr.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "HierSvr - Win32 Unicode Release"

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

ALL : "$(OUTDIR)\hiersvr.exe" "$(OUTDIR)\hiersvr.HLP"

CLEAN : 
	-@erase ".\UniRelease\hiersvr.exe"
	-@erase ".\UniRelease\zoomdlg.obj"
	-@erase ".\UniRelease\hiersvr.pch"
	-@erase ".\UniRelease\ipframe.obj"
	-@erase ".\UniRelease\svrview.obj"
	-@erase ".\UniRelease\hiersvr.obj"
	-@erase ".\UniRelease\svrdoc.obj"
	-@erase ".\UniRelease\svritem.obj"
	-@erase ".\UniRelease\stdafx.obj"
	-@erase ".\UniRelease\mainfrm.obj"
	-@erase ".\UniRelease\hiersvr.res"
	-@erase ".\UniRelease\hiersvr.HLP"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /FR /YX /c
# ADD CPP /MD /W3 /WX /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_UNICODE" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
CPP_PROJ=/MD /W3 /WX /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_UNICODE"\
 /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/hiersvr.pch" /Yu"stdafx.h"\
 /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\UniRelease/
CPP_SBRS=

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
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/hiersvr.res" /d "NDEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/hiersvr.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 /nologo /entry:"wWinMainCRTStartup" /subsystem:windows /machine:I386
LINK32_FLAGS=/nologo /entry:"wWinMainCRTStartup" /subsystem:windows\
 /incremental:no /pdb:"$(OUTDIR)/hiersvr.pdb" /machine:I386\
 /out:"$(OUTDIR)/hiersvr.exe" 
LINK32_OBJS= \
	"$(INTDIR)/zoomdlg.obj" \
	"$(INTDIR)/ipframe.obj" \
	"$(INTDIR)/svrview.obj" \
	"$(INTDIR)/hiersvr.obj" \
	"$(INTDIR)/svrdoc.obj" \
	"$(INTDIR)/svritem.obj" \
	"$(INTDIR)/stdafx.obj" \
	"$(INTDIR)/mainfrm.obj" \
	"$(INTDIR)/hiersvr.res"

"$(OUTDIR)\hiersvr.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 

################################################################################
# Begin Target

# Name "HierSvr - Win32 Release"
# Name "HierSvr - Win32 Debug"
# Name "HierSvr - Macintosh Release"
# Name "HierSvr - Macintosh Debug"
# Name "HierSvr - Power Macintosh Release"
# Name "HierSvr - Power Macintosh Debug"
# Name "HierSvr - Win32 Unicode Debug"
# Name "HierSvr - Win32 Unicode Release"

!IF  "$(CFG)" == "HierSvr - Win32 Release"

!ELSEIF  "$(CFG)" == "HierSvr - Win32 Debug"

!ELSEIF  "$(CFG)" == "HierSvr - Macintosh Release"

!ELSEIF  "$(CFG)" == "HierSvr - Macintosh Debug"

!ELSEIF  "$(CFG)" == "HierSvr - Power Macintosh Release"

!ELSEIF  "$(CFG)" == "HierSvr - Power Macintosh Debug"

!ELSEIF  "$(CFG)" == "HierSvr - Win32 Unicode Debug"

!ELSEIF  "$(CFG)" == "HierSvr - Win32 Unicode Release"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\hiersvr.cpp
DEP_CPP_HIERS=\
	".\hiersvr.h"\
	".\ipframe.h"\
	".\mainfrm.h"\
	".\stdafx.h"\
	".\svrdoc.h"\
	".\svrview.h"\
	".\svritem.h"\
	

!IF  "$(CFG)" == "HierSvr - Win32 Release"


"$(INTDIR)\hiersvr.obj" : $(SOURCE) $(DEP_CPP_HIERS) "$(INTDIR)"\
 "$(INTDIR)\hiersvr.pch"


!ELSEIF  "$(CFG)" == "HierSvr - Win32 Debug"


"$(INTDIR)\hiersvr.obj" : $(SOURCE) $(DEP_CPP_HIERS) "$(INTDIR)"\
 "$(INTDIR)\hiersvr.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_HIERS) "$(INTDIR)"\
 "$(INTDIR)\hiersvr.pch"


!ELSEIF  "$(CFG)" == "HierSvr - Macintosh Release"


"$(INTDIR)\hiersvr.obj" : $(SOURCE) $(DEP_CPP_HIERS) "$(INTDIR)"\
 "$(INTDIR)\hiersvr.pch"


!ELSEIF  "$(CFG)" == "HierSvr - Macintosh Debug"


"$(INTDIR)\hiersvr.obj" : $(SOURCE) $(DEP_CPP_HIERS) "$(INTDIR)"\
 "$(INTDIR)\hiersvr.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_HIERS) "$(INTDIR)"\
 "$(INTDIR)\hiersvr.pch"


!ELSEIF  "$(CFG)" == "HierSvr - Power Macintosh Release"


"$(INTDIR)\hiersvr.obj" : $(SOURCE) $(DEP_CPP_HIERS) "$(INTDIR)"\
 "$(INTDIR)\hiersvr.pch"


!ELSEIF  "$(CFG)" == "HierSvr - Power Macintosh Debug"


"$(INTDIR)\hiersvr.obj" : $(SOURCE) $(DEP_CPP_HIERS) "$(INTDIR)"\
 "$(INTDIR)\hiersvr.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_HIERS) "$(INTDIR)"\
 "$(INTDIR)\hiersvr.pch"


!ELSEIF  "$(CFG)" == "HierSvr - Win32 Unicode Debug"


"$(INTDIR)\hiersvr.obj" : $(SOURCE) $(DEP_CPP_HIERS) "$(INTDIR)"\
 "$(INTDIR)\hiersvr.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_HIERS) "$(INTDIR)"\
 "$(INTDIR)\hiersvr.pch"


!ELSEIF  "$(CFG)" == "HierSvr - Win32 Unicode Release"


"$(INTDIR)\hiersvr.obj" : $(SOURCE) $(DEP_CPP_HIERS) "$(INTDIR)"\
 "$(INTDIR)\hiersvr.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\hiersvr.rc
DEP_RSC_HIERSV=\
	".\res\hiersvr.ico"\
	".\res\svrdoc.ico"\
	".\res\toolbar.bmp"\
	".\res\itoolbar.bmp"\
	

!IF  "$(CFG)" == "HierSvr - Win32 Release"


"$(INTDIR)\hiersvr.res" : $(SOURCE) $(DEP_RSC_HIERSV) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "HierSvr - Win32 Debug"


"$(INTDIR)\hiersvr.res" : $(SOURCE) $(DEP_RSC_HIERSV) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "HierSvr - Macintosh Release"


"$(INTDIR)\hiersvr.rsc" : $(SOURCE) $(DEP_RSC_HIERSV) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "HierSvr - Macintosh Debug"


"$(INTDIR)\hiersvr.rsc" : $(SOURCE) $(DEP_RSC_HIERSV) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "HierSvr - Power Macintosh Release"


"$(INTDIR)\hiersvr.rsc" : $(SOURCE) $(DEP_RSC_HIERSV) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "HierSvr - Power Macintosh Debug"


"$(INTDIR)\hiersvr.rsc" : $(SOURCE) $(DEP_RSC_HIERSV) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "HierSvr - Win32 Unicode Debug"


"$(INTDIR)\hiersvr.res" : $(SOURCE) $(DEP_RSC_HIERSV) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "HierSvr - Win32 Unicode Release"


"$(INTDIR)\hiersvr.res" : $(SOURCE) $(DEP_RSC_HIERSV) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\ipframe.cpp
DEP_CPP_IPFRA=\
	".\hiersvr.h"\
	".\ipframe.h"\
	".\stdafx.h"\
	

!IF  "$(CFG)" == "HierSvr - Win32 Release"


"$(INTDIR)\ipframe.obj" : $(SOURCE) $(DEP_CPP_IPFRA) "$(INTDIR)"\
 "$(INTDIR)\hiersvr.pch"


!ELSEIF  "$(CFG)" == "HierSvr - Win32 Debug"


"$(INTDIR)\ipframe.obj" : $(SOURCE) $(DEP_CPP_IPFRA) "$(INTDIR)"\
 "$(INTDIR)\hiersvr.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_IPFRA) "$(INTDIR)"\
 "$(INTDIR)\hiersvr.pch"


!ELSEIF  "$(CFG)" == "HierSvr - Macintosh Release"


"$(INTDIR)\ipframe.obj" : $(SOURCE) $(DEP_CPP_IPFRA) "$(INTDIR)"\
 "$(INTDIR)\hiersvr.pch"


!ELSEIF  "$(CFG)" == "HierSvr - Macintosh Debug"


"$(INTDIR)\ipframe.obj" : $(SOURCE) $(DEP_CPP_IPFRA) "$(INTDIR)"\
 "$(INTDIR)\hiersvr.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_IPFRA) "$(INTDIR)"\
 "$(INTDIR)\hiersvr.pch"


!ELSEIF  "$(CFG)" == "HierSvr - Power Macintosh Release"


"$(INTDIR)\ipframe.obj" : $(SOURCE) $(DEP_CPP_IPFRA) "$(INTDIR)"\
 "$(INTDIR)\hiersvr.pch"


!ELSEIF  "$(CFG)" == "HierSvr - Power Macintosh Debug"


"$(INTDIR)\ipframe.obj" : $(SOURCE) $(DEP_CPP_IPFRA) "$(INTDIR)"\
 "$(INTDIR)\hiersvr.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_IPFRA) "$(INTDIR)"\
 "$(INTDIR)\hiersvr.pch"


!ELSEIF  "$(CFG)" == "HierSvr - Win32 Unicode Debug"


"$(INTDIR)\ipframe.obj" : $(SOURCE) $(DEP_CPP_IPFRA) "$(INTDIR)"\
 "$(INTDIR)\hiersvr.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_IPFRA) "$(INTDIR)"\
 "$(INTDIR)\hiersvr.pch"


!ELSEIF  "$(CFG)" == "HierSvr - Win32 Unicode Release"


"$(INTDIR)\ipframe.obj" : $(SOURCE) $(DEP_CPP_IPFRA) "$(INTDIR)"\
 "$(INTDIR)\hiersvr.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\mainfrm.cpp
DEP_CPP_MAINF=\
	".\hiersvr.h"\
	".\mainfrm.h"\
	".\stdafx.h"\
	

!IF  "$(CFG)" == "HierSvr - Win32 Release"


"$(INTDIR)\mainfrm.obj" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\hiersvr.pch"


!ELSEIF  "$(CFG)" == "HierSvr - Win32 Debug"


"$(INTDIR)\mainfrm.obj" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\hiersvr.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\hiersvr.pch"


!ELSEIF  "$(CFG)" == "HierSvr - Macintosh Release"


"$(INTDIR)\mainfrm.obj" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\hiersvr.pch"


!ELSEIF  "$(CFG)" == "HierSvr - Macintosh Debug"


"$(INTDIR)\mainfrm.obj" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\hiersvr.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\hiersvr.pch"


!ELSEIF  "$(CFG)" == "HierSvr - Power Macintosh Release"


"$(INTDIR)\mainfrm.obj" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\hiersvr.pch"


!ELSEIF  "$(CFG)" == "HierSvr - Power Macintosh Debug"


"$(INTDIR)\mainfrm.obj" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\hiersvr.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\hiersvr.pch"


!ELSEIF  "$(CFG)" == "HierSvr - Win32 Unicode Debug"


"$(INTDIR)\mainfrm.obj" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\hiersvr.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\hiersvr.pch"


!ELSEIF  "$(CFG)" == "HierSvr - Win32 Unicode Release"


"$(INTDIR)\mainfrm.obj" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\hiersvr.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\stdafx.cpp
DEP_CPP_STDAF=\
	".\stdafx.h"\
	

!IF  "$(CFG)" == "HierSvr - Win32 Release"

# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D\
 "_MBCS" /Fp"$(INTDIR)/hiersvr.pch" /Yc"stdafx.h" /Fo"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\hiersvr.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "HierSvr - Win32 Debug"

# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/hiersvr.pch" /Yc"stdafx.h" /Fo"$(INTDIR)/"\
 /Fd"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\hiersvr.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "HierSvr - Macintosh Release"

# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /AL /Q68s /W3 /GX /O2 /D "_WINDOWS" /D "_MAC" /D "_68K_" /D "WIN32" /D\
 "NDEBUG" /D "_MBCS" /Fp"$(INTDIR)/hiersvr.pch" /Yc"stdafx.h" /Fo"$(INTDIR)/" /c\
 $(SOURCE) \
	

"$(INTDIR)\stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\hiersvr.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "HierSvr - Macintosh Debug"

# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /AL /Q68s /Q68m /W3 /GX /Zi /Od /D "_WINDOWS" /D "_MAC" /D "_68K_" /D\
 "WIN32" /D "_DEBUG" /D "_MBCS" /Fp"$(INTDIR)/hiersvr.pch" /Yc"stdafx.h"\
 /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\hiersvr.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "HierSvr - Power Macintosh Release"

# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /MD /W3 /GX /O2 /D "NDEBUG" /D "_MAC" /D "_MPPC_" /D "_WINDOWS" /D\
 "WIN32" /D "_MBCS" /D "_AFXDLL" /Fp"$(INTDIR)/hiersvr.pch" /Yc"stdafx.h"\
 /Fo"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\hiersvr.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "HierSvr - Power Macintosh Debug"

# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /MDd /W3 /GX /Zi /Od /D "_DEBUG" /D "_MAC" /D "_MPPC_" /D "_WINDOWS" /D\
 "WIN32" /D "_MBCS" /D "_AFXDLL" /Fp"$(INTDIR)/hiersvr.pch" /Yc"stdafx.h"\
 /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\hiersvr.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "HierSvr - Win32 Unicode Debug"

# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D\
 "_UNICODE" /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/hiersvr.pch" /Yc"stdafx.h"\
 /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\hiersvr.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "HierSvr - Win32 Unicode Release"

# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /MD /W3 /WX /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_UNICODE"\
 /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/hiersvr.pch" /Yc"stdafx.h"\
 /Fo"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\hiersvr.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\svrdoc.cpp
DEP_CPP_SVRDO=\
	".\hiersvr.h"\
	".\stdafx.h"\
	".\svrdoc.h"\
	".\svritem.h"\
	".\svrview.h"\
	

!IF  "$(CFG)" == "HierSvr - Win32 Release"


"$(INTDIR)\svrdoc.obj" : $(SOURCE) $(DEP_CPP_SVRDO) "$(INTDIR)"\
 "$(INTDIR)\hiersvr.pch"


!ELSEIF  "$(CFG)" == "HierSvr - Win32 Debug"


"$(INTDIR)\svrdoc.obj" : $(SOURCE) $(DEP_CPP_SVRDO) "$(INTDIR)"\
 "$(INTDIR)\hiersvr.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_SVRDO) "$(INTDIR)"\
 "$(INTDIR)\hiersvr.pch"


!ELSEIF  "$(CFG)" == "HierSvr - Macintosh Release"


"$(INTDIR)\svrdoc.obj" : $(SOURCE) $(DEP_CPP_SVRDO) "$(INTDIR)"\
 "$(INTDIR)\hiersvr.pch"


!ELSEIF  "$(CFG)" == "HierSvr - Macintosh Debug"


"$(INTDIR)\svrdoc.obj" : $(SOURCE) $(DEP_CPP_SVRDO) "$(INTDIR)"\
 "$(INTDIR)\hiersvr.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_SVRDO) "$(INTDIR)"\
 "$(INTDIR)\hiersvr.pch"


!ELSEIF  "$(CFG)" == "HierSvr - Power Macintosh Release"


"$(INTDIR)\svrdoc.obj" : $(SOURCE) $(DEP_CPP_SVRDO) "$(INTDIR)"\
 "$(INTDIR)\hiersvr.pch"


!ELSEIF  "$(CFG)" == "HierSvr - Power Macintosh Debug"


"$(INTDIR)\svrdoc.obj" : $(SOURCE) $(DEP_CPP_SVRDO) "$(INTDIR)"\
 "$(INTDIR)\hiersvr.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_SVRDO) "$(INTDIR)"\
 "$(INTDIR)\hiersvr.pch"


!ELSEIF  "$(CFG)" == "HierSvr - Win32 Unicode Debug"


"$(INTDIR)\svrdoc.obj" : $(SOURCE) $(DEP_CPP_SVRDO) "$(INTDIR)"\
 "$(INTDIR)\hiersvr.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_SVRDO) "$(INTDIR)"\
 "$(INTDIR)\hiersvr.pch"


!ELSEIF  "$(CFG)" == "HierSvr - Win32 Unicode Release"


"$(INTDIR)\svrdoc.obj" : $(SOURCE) $(DEP_CPP_SVRDO) "$(INTDIR)"\
 "$(INTDIR)\hiersvr.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\svritem.cpp
DEP_CPP_SVRIT=\
	".\hiersvr.h"\
	".\stdafx.h"\
	".\svrdoc.h"\
	".\svritem.h"\
	".\svrview.h"\
	

!IF  "$(CFG)" == "HierSvr - Win32 Release"


"$(INTDIR)\svritem.obj" : $(SOURCE) $(DEP_CPP_SVRIT) "$(INTDIR)"\
 "$(INTDIR)\hiersvr.pch"
   $(CPP) /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D\
 "_MBCS" /Fp"$(INTDIR)/hiersvr.pch" /Yu"stdafx.h" /Fo"$(INTDIR)/" /c $(SOURCE)


!ELSEIF  "$(CFG)" == "HierSvr - Win32 Debug"


BuildCmds= \
	$(CPP) /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/hiersvr.pch" /Yu"stdafx.h" /Fo"$(INTDIR)/"\
 /Fd"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\svritem.obj" : $(SOURCE) $(DEP_CPP_SVRIT) "$(INTDIR)"\
 "$(INTDIR)\hiersvr.pch"
   $(BuildCmds)

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_SVRIT) "$(INTDIR)"\
 "$(INTDIR)\hiersvr.pch"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "HierSvr - Macintosh Release"


"$(INTDIR)\svritem.obj" : $(SOURCE) $(DEP_CPP_SVRIT) "$(INTDIR)"\
 "$(INTDIR)\hiersvr.pch"
   $(CPP) /AL /Q68s /W3 /GX /O2 /D "_WINDOWS" /D "_MAC" /D "_68K_" /D "WIN32"\
 /D "NDEBUG" /D "_MBCS" /Fp"$(INTDIR)/hiersvr.pch" /Yu"stdafx.h" /Fo"$(INTDIR)/"\
 /c $(SOURCE)


!ELSEIF  "$(CFG)" == "HierSvr - Macintosh Debug"


BuildCmds= \
	$(CPP) /AL /Q68s /Q68m /W3 /GX /Zi /Od /D "_WINDOWS" /D "_MAC" /D "_68K_" /D\
 "WIN32" /D "_DEBUG" /D "_MBCS" /Fp"$(INTDIR)/hiersvr.pch" /Yu"stdafx.h"\
 /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\svritem.obj" : $(SOURCE) $(DEP_CPP_SVRIT) "$(INTDIR)"\
 "$(INTDIR)\hiersvr.pch"
   $(BuildCmds)

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_SVRIT) "$(INTDIR)"\
 "$(INTDIR)\hiersvr.pch"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "HierSvr - Power Macintosh Release"


"$(INTDIR)\svritem.obj" : $(SOURCE) $(DEP_CPP_SVRIT) "$(INTDIR)"\
 "$(INTDIR)\hiersvr.pch"
   $(CPP) /MD /W3 /GX /O2 /D "NDEBUG" /D "_MAC" /D "_MPPC_" /D "_WINDOWS" /D\
 "WIN32" /D "_MBCS" /D "_AFXDLL" /Fp"$(INTDIR)/hiersvr.pch" /Yu"stdafx.h"\
 /Fo"$(INTDIR)/" /c $(SOURCE)


!ELSEIF  "$(CFG)" == "HierSvr - Power Macintosh Debug"


BuildCmds= \
	$(CPP) /MDd /W3 /GX /Zi /Od /D "_DEBUG" /D "_MAC" /D "_MPPC_" /D "_WINDOWS" /D\
 "WIN32" /D "_MBCS" /D "_AFXDLL" /Fp"$(INTDIR)/hiersvr.pch" /Yu"stdafx.h"\
 /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\svritem.obj" : $(SOURCE) $(DEP_CPP_SVRIT) "$(INTDIR)"\
 "$(INTDIR)\hiersvr.pch"
   $(BuildCmds)

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_SVRIT) "$(INTDIR)"\
 "$(INTDIR)\hiersvr.pch"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "HierSvr - Win32 Unicode Debug"


BuildCmds= \
	$(CPP) /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D\
 "_UNICODE" /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/hiersvr.pch" /Yu"stdafx.h"\
 /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\svritem.obj" : $(SOURCE) $(DEP_CPP_SVRIT) "$(INTDIR)"\
 "$(INTDIR)\hiersvr.pch"
   $(BuildCmds)

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_SVRIT) "$(INTDIR)"\
 "$(INTDIR)\hiersvr.pch"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "HierSvr - Win32 Unicode Release"

# ADD CPP /GX

"$(INTDIR)\svritem.obj" : $(SOURCE) $(DEP_CPP_SVRIT) "$(INTDIR)"\
 "$(INTDIR)\hiersvr.pch"
   $(CPP) /MD /W3 /WX /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_UNICODE" /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/hiersvr.pch" /Yu"stdafx.h"\
 /Fo"$(INTDIR)/" /c $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\svrview.cpp
DEP_CPP_SVRVI=\
	".\hiersvr.h"\
	".\stdafx.h"\
	".\svrdoc.h"\
	".\svritem.h"\
	".\svrview.h"\
	".\zoomdlg.h"\
	

!IF  "$(CFG)" == "HierSvr - Win32 Release"


"$(INTDIR)\svrview.obj" : $(SOURCE) $(DEP_CPP_SVRVI) "$(INTDIR)"\
 "$(INTDIR)\hiersvr.pch"
   $(CPP) /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D\
 "_MBCS" /Fp"$(INTDIR)/hiersvr.pch" /Yu"stdafx.h" /Fo"$(INTDIR)/" /c $(SOURCE)


!ELSEIF  "$(CFG)" == "HierSvr - Win32 Debug"


BuildCmds= \
	$(CPP) /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/hiersvr.pch" /Yu"stdafx.h" /Fo"$(INTDIR)/"\
 /Fd"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\svrview.obj" : $(SOURCE) $(DEP_CPP_SVRVI) "$(INTDIR)"\
 "$(INTDIR)\hiersvr.pch"
   $(BuildCmds)

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_SVRVI) "$(INTDIR)"\
 "$(INTDIR)\hiersvr.pch"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "HierSvr - Macintosh Release"


"$(INTDIR)\svrview.obj" : $(SOURCE) $(DEP_CPP_SVRVI) "$(INTDIR)"\
 "$(INTDIR)\hiersvr.pch"
   $(CPP) /AL /Q68s /W3 /GX /O2 /D "_WINDOWS" /D "_MAC" /D "_68K_" /D "WIN32"\
 /D "NDEBUG" /D "_MBCS" /Fp"$(INTDIR)/hiersvr.pch" /Yu"stdafx.h" /Fo"$(INTDIR)/"\
 /c $(SOURCE)


!ELSEIF  "$(CFG)" == "HierSvr - Macintosh Debug"


BuildCmds= \
	$(CPP) /AL /Q68s /Q68m /W3 /GX /Zi /Od /D "_WINDOWS" /D "_MAC" /D "_68K_" /D\
 "WIN32" /D "_DEBUG" /D "_MBCS" /Fp"$(INTDIR)/hiersvr.pch" /Yu"stdafx.h"\
 /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\svrview.obj" : $(SOURCE) $(DEP_CPP_SVRVI) "$(INTDIR)"\
 "$(INTDIR)\hiersvr.pch"
   $(BuildCmds)

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_SVRVI) "$(INTDIR)"\
 "$(INTDIR)\hiersvr.pch"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "HierSvr - Power Macintosh Release"


"$(INTDIR)\svrview.obj" : $(SOURCE) $(DEP_CPP_SVRVI) "$(INTDIR)"\
 "$(INTDIR)\hiersvr.pch"
   $(CPP) /MD /W3 /GX /O2 /D "NDEBUG" /D "_MAC" /D "_MPPC_" /D "_WINDOWS" /D\
 "WIN32" /D "_MBCS" /D "_AFXDLL" /Fp"$(INTDIR)/hiersvr.pch" /Yu"stdafx.h"\
 /Fo"$(INTDIR)/" /c $(SOURCE)


!ELSEIF  "$(CFG)" == "HierSvr - Power Macintosh Debug"


BuildCmds= \
	$(CPP) /MDd /W3 /GX /Zi /Od /D "_DEBUG" /D "_MAC" /D "_MPPC_" /D "_WINDOWS" /D\
 "WIN32" /D "_MBCS" /D "_AFXDLL" /Fp"$(INTDIR)/hiersvr.pch" /Yu"stdafx.h"\
 /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\svrview.obj" : $(SOURCE) $(DEP_CPP_SVRVI) "$(INTDIR)"\
 "$(INTDIR)\hiersvr.pch"
   $(BuildCmds)

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_SVRVI) "$(INTDIR)"\
 "$(INTDIR)\hiersvr.pch"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "HierSvr - Win32 Unicode Debug"


BuildCmds= \
	$(CPP) /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D\
 "_UNICODE" /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/hiersvr.pch" /Yu"stdafx.h"\
 /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\svrview.obj" : $(SOURCE) $(DEP_CPP_SVRVI) "$(INTDIR)"\
 "$(INTDIR)\hiersvr.pch"
   $(BuildCmds)

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_SVRVI) "$(INTDIR)"\
 "$(INTDIR)\hiersvr.pch"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "HierSvr - Win32 Unicode Release"

# ADD CPP /GX

"$(INTDIR)\svrview.obj" : $(SOURCE) $(DEP_CPP_SVRVI) "$(INTDIR)"\
 "$(INTDIR)\hiersvr.pch"
   $(CPP) /MD /W3 /WX /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_UNICODE" /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/hiersvr.pch" /Yu"stdafx.h"\
 /Fo"$(INTDIR)/" /c $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\zoomdlg.cpp
DEP_CPP_ZOOMD=\
	".\hiersvr.h"\
	".\stdafx.h"\
	".\zoomdlg.h"\
	

!IF  "$(CFG)" == "HierSvr - Win32 Release"


"$(INTDIR)\zoomdlg.obj" : $(SOURCE) $(DEP_CPP_ZOOMD) "$(INTDIR)"\
 "$(INTDIR)\hiersvr.pch"


!ELSEIF  "$(CFG)" == "HierSvr - Win32 Debug"


"$(INTDIR)\zoomdlg.obj" : $(SOURCE) $(DEP_CPP_ZOOMD) "$(INTDIR)"\
 "$(INTDIR)\hiersvr.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_ZOOMD) "$(INTDIR)"\
 "$(INTDIR)\hiersvr.pch"


!ELSEIF  "$(CFG)" == "HierSvr - Macintosh Release"


"$(INTDIR)\zoomdlg.obj" : $(SOURCE) $(DEP_CPP_ZOOMD) "$(INTDIR)"\
 "$(INTDIR)\hiersvr.pch"


!ELSEIF  "$(CFG)" == "HierSvr - Macintosh Debug"


"$(INTDIR)\zoomdlg.obj" : $(SOURCE) $(DEP_CPP_ZOOMD) "$(INTDIR)"\
 "$(INTDIR)\hiersvr.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_ZOOMD) "$(INTDIR)"\
 "$(INTDIR)\hiersvr.pch"


!ELSEIF  "$(CFG)" == "HierSvr - Power Macintosh Release"


"$(INTDIR)\zoomdlg.obj" : $(SOURCE) $(DEP_CPP_ZOOMD) "$(INTDIR)"\
 "$(INTDIR)\hiersvr.pch"


!ELSEIF  "$(CFG)" == "HierSvr - Power Macintosh Debug"


"$(INTDIR)\zoomdlg.obj" : $(SOURCE) $(DEP_CPP_ZOOMD) "$(INTDIR)"\
 "$(INTDIR)\hiersvr.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_ZOOMD) "$(INTDIR)"\
 "$(INTDIR)\hiersvr.pch"


!ELSEIF  "$(CFG)" == "HierSvr - Win32 Unicode Debug"


"$(INTDIR)\zoomdlg.obj" : $(SOURCE) $(DEP_CPP_ZOOMD) "$(INTDIR)"\
 "$(INTDIR)\hiersvr.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_ZOOMD) "$(INTDIR)"\
 "$(INTDIR)\hiersvr.pch"


!ELSEIF  "$(CFG)" == "HierSvr - Win32 Unicode Release"


"$(INTDIR)\zoomdlg.obj" : $(SOURCE) $(DEP_CPP_ZOOMD) "$(INTDIR)"\
 "$(INTDIR)\hiersvr.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\hiermac.r

!IF  "$(CFG)" == "HierSvr - Win32 Release"

!ELSEIF  "$(CFG)" == "HierSvr - Win32 Debug"

!ELSEIF  "$(CFG)" == "HierSvr - Macintosh Release"

DEP_MRC_HIERM=\
	{$(INCLUDE)}"\mrc\types.r"\
	{$(INCLUDE)}"\mrc\balloons.r"\
	

"$(INTDIR)\hiermac.rsc" : $(SOURCE) $(DEP_MRC_HIERM) "$(INTDIR)"
   $(MRC) /o"$(INTDIR)/hiermac.rsc" /D "_68K_" /D "_MAC" /D "NDEBUG" /l 0x409\
 /NOLOGO $(SOURCE)


!ELSEIF  "$(CFG)" == "HierSvr - Macintosh Debug"

DEP_MRC_HIERM=\
	{$(INCLUDE)}"\mrc\types.r"\
	{$(INCLUDE)}"\mrc\balloons.r"\
	

"$(INTDIR)\hiermac.rsc" : $(SOURCE) $(DEP_MRC_HIERM) "$(INTDIR)"
   $(MRC) /o"$(INTDIR)/hiermac.rsc" /D "_68K_" /D "_MAC" /D "_DEBUG" /l 0x409\
 /NOLOGO $(SOURCE)


!ELSEIF  "$(CFG)" == "HierSvr - Power Macintosh Release"

DEP_MRC_HIERM=\
	{$(INCLUDE)}"\mrc\types.r"\
	{$(INCLUDE)}"\mrc\balloons.r"\
	

"$(INTDIR)\hiermac.rsc" : $(SOURCE) $(DEP_MRC_HIERM) "$(INTDIR)"
   $(MRC) /o"$(INTDIR)/hiermac.rsc" /D "_MPPC_" /D "_MAC" /D "NDEBUG" /l 0x409\
 /NOLOGO $(SOURCE)


!ELSEIF  "$(CFG)" == "HierSvr - Power Macintosh Debug"

DEP_MRC_HIERM=\
	{$(INCLUDE)}"\mrc\types.r"\
	{$(INCLUDE)}"\mrc\balloons.r"\
	

"$(INTDIR)\hiermac.rsc" : $(SOURCE) $(DEP_MRC_HIERM) "$(INTDIR)"
   $(MRC) /o"$(INTDIR)/hiermac.rsc" /D "_MPPC_" /D "_MAC" /D "_DEBUG" /l 0x409\
 /NOLOGO $(SOURCE)


!ELSEIF  "$(CFG)" == "HierSvr - Win32 Unicode Debug"

!ELSEIF  "$(CFG)" == "HierSvr - Win32 Unicode Release"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\hlp\hiersvr.hpj

!IF  "$(CFG)" == "HierSvr - Win32 Release"

# Begin Custom Build
OutDir=.\Release
ProjDir=.
TargetName=hiersvr
InputPath=.\hlp\hiersvr.hpj

"$(OutDir)\$(TargetName).HLP" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   "$(ProjDir)\makehelp.bat"

# End Custom Build

!ELSEIF  "$(CFG)" == "HierSvr - Win32 Debug"

# Begin Custom Build
OutDir=.\Debug
ProjDir=.
TargetName=hiersvr
InputPath=.\hlp\hiersvr.hpj

"$(OutDir)\$(TargetName).HLP" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   "$(ProjDir)\makehelp.bat"

# End Custom Build

!ELSEIF  "$(CFG)" == "HierSvr - Macintosh Release"

!ELSEIF  "$(CFG)" == "HierSvr - Macintosh Debug"

!ELSEIF  "$(CFG)" == "HierSvr - Power Macintosh Release"

!ELSEIF  "$(CFG)" == "HierSvr - Power Macintosh Debug"

!ELSEIF  "$(CFG)" == "HierSvr - Win32 Unicode Debug"

# Begin Custom Build
OutDir=.\UniDebug
ProjDir=.
TargetName=hiersvr
InputPath=.\hlp\hiersvr.hpj

"$(OutDir)\$(TargetName).HLP" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   "$(ProjDir)\makehelp.bat"

# End Custom Build

!ELSEIF  "$(CFG)" == "HierSvr - Win32 Unicode Release"

# Begin Custom Build
OutDir=.\UniRelease
ProjDir=.
TargetName=hiersvr
InputPath=.\hlp\hiersvr.hpj

"$(OutDir)\$(TargetName).HLP" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   "$(ProjDir)\makehelp.bat"

# End Custom Build

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\hlp\hiermac.hpj

!IF  "$(CFG)" == "HierSvr - Win32 Release"

!ELSEIF  "$(CFG)" == "HierSvr - Win32 Debug"

!ELSEIF  "$(CFG)" == "HierSvr - Macintosh Release"

# Begin Custom Build
OutDir=.\MacRel
ProjDir=.
TargetName=hiersvr
InputPath=.\hlp\hiermac.hpj
RemoteTargetPath=macintosh hd:thuy:hiersvr_macR

"$(OutDir)\$(TargetName).HLP" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(ProjDir)\makehelp.bat MAC ":$(RemoteTargetPath) Help"

# End Custom Build

!ELSEIF  "$(CFG)" == "HierSvr - Macintosh Debug"

# Begin Custom Build
OutDir=.\MacDbg
ProjDir=.
TargetName=hiersvr
InputPath=.\hlp\hiermac.hpj
RemoteTargetPath=macintosh hd:thuy:hiersvr_macD

"$(OutDir)\$(TargetName).HLP" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(ProjDir)\makehelp.bat MAC ":$(RemoteTargetPath) Help"

# End Custom Build

!ELSEIF  "$(CFG)" == "HierSvr - Power Macintosh Release"

# Begin Custom Build
OutDir=.\PMacRel
ProjDir=.
TargetName=hiersvr
InputPath=.\hlp\hiermac.hpj
RemoteTargetPath=macintosh hd:thuy:hiersvr_mppcR

"$(OutDir)\$(TargetName).HLP" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(ProjDir)\makehelp.bat MAC ":$(RemoteTargetPath) Help"

# End Custom Build

!ELSEIF  "$(CFG)" == "HierSvr - Power Macintosh Debug"

# Begin Custom Build
OutDir=.\PMacDbg
ProjDir=.
TargetName=hiersvr
InputPath=.\hlp\hiermac.hpj
RemoteTargetPath=macintosh hd:thuy:hiersvr_mppcD

"$(OutDir)\$(TargetName).HLP" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(ProjDir)\makehelp.bat MAC ":$(RemoteTargetPath) Help"

# End Custom Build

!ELSEIF  "$(CFG)" == "HierSvr - Win32 Unicode Debug"

!ELSEIF  "$(CFG)" == "HierSvr - Win32 Unicode Release"

!ENDIF 

# End Source File
# End Target
# End Project
################################################################################
