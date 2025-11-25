# Microsoft Visual C++ Generated NMAKE File, Format Version 40000
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101
# TARGTYPE "Power Macintosh Application" 0x0401
# TARGTYPE "Macintosh Application" 0x0301

!IF "$(CFG)" == ""
CFG=OClient - Win32 Debug
!MESSAGE No configuration specified.  Defaulting to OClient - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "OClient - Win32 Release" && "$(CFG)" !=\
 "OClient - Win32 Debug" && "$(CFG)" != "OClient - Macintosh Release" &&\
 "$(CFG)" != "OClient - Macintosh Debug" && "$(CFG)" !=\
 "OClient - Power Macintosh Release" && "$(CFG)" !=\
 "OClient - Power Macintosh Debug" && "$(CFG)" !=\
 "OClient - Win32 Unicode Debug" && "$(CFG)" !=\
 "OClient - Win32 Unicode Release"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "oclient.mak" CFG="OClient - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "OClient - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "OClient - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE "OClient - Macintosh Release" (based on "Macintosh Application")
!MESSAGE "OClient - Macintosh Debug" (based on "Macintosh Application")
!MESSAGE "OClient - Power Macintosh Release" (based on\
 "Power Macintosh Application")
!MESSAGE "OClient - Power Macintosh Debug" (based on\
 "Power Macintosh Application")
!MESSAGE "OClient - Win32 Unicode Debug" (based on "Win32 (x86) Application")
!MESSAGE "OClient - Win32 Unicode Release" (based on "Win32 (x86) Application")
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
# PROP Target_Last_Scanned "OClient - Win32 Unicode Release"

!IF  "$(CFG)" == "OClient - Win32 Release"

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

ALL : ".\Release\oclient.exe" ".\Release\oclient.HLP"

CLEAN : 
	-@erase ".\Release\oclient.exe"
	-@erase ".\Release\frame.obj"
	-@erase ".\Release\oclient.pch"
	-@erase ".\Release\splitfra.obj"
	-@erase ".\Release\oclient.obj"
	-@erase ".\Release\mainview.obj"
	-@erase ".\Release\maindoc.obj"
	-@erase ".\Release\rectitem.obj"
	-@erase ".\Release\stdafx.obj"
	-@erase ".\Release\oclient.res"
	-@erase ".\Release\oclient.HLP"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /FR /YX /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
# SUBTRACT CPP /Fr
CPP_PROJ=/nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/oclient.pch" /Yu"stdafx.h" /Fo"$(INTDIR)/"\
 /c 
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
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/oclient.res" /d "NDEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/oclient.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 /nologo /subsystem:windows /machine:I386
LINK32_FLAGS=/nologo /subsystem:windows /incremental:no\
 /pdb:"$(OUTDIR)/oclient.pdb" /machine:I386 /out:"$(OUTDIR)/oclient.exe" 
LINK32_OBJS= \
	".\Release\frame.obj" \
	".\Release\splitfra.obj" \
	".\Release\oclient.obj" \
	".\Release\mainview.obj" \
	".\Release\maindoc.obj" \
	".\Release\rectitem.obj" \
	".\Release\stdafx.obj" \
	".\Release\oclient.res"

".\Release\oclient.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "OClient - Win32 Debug"

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

ALL : ".\Debug\oclient.exe" ".\Debug\oclient.HLP"

CLEAN : 
	-@erase ".\Debug\vc40.pdb"
	-@erase ".\Debug\oclient.pch"
	-@erase ".\Debug\oclient.exe"
	-@erase ".\Debug\splitfra.obj"
	-@erase ".\Debug\mainview.obj"
	-@erase ".\Debug\maindoc.obj"
	-@erase ".\Debug\stdafx.obj"
	-@erase ".\Debug\rectitem.obj"
	-@erase ".\Debug\frame.obj"
	-@erase ".\Debug\oclient.obj"
	-@erase ".\Debug\oclient.res"
	-@erase ".\Debug\oclient.ilk"
	-@erase ".\Debug\oclient.pdb"
	-@erase ".\Debug\oclient.HLP"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /FR /YX /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
# SUBTRACT CPP /Fr
CPP_PROJ=/nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/oclient.pch" /Yu"stdafx.h"\
 /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
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
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/oclient.res" /d "_DEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/oclient.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386
# ADD LINK32 /nologo /subsystem:windows /debug /machine:I386
LINK32_FLAGS=/nologo /subsystem:windows /incremental:yes\
 /pdb:"$(OUTDIR)/oclient.pdb" /debug /machine:I386 /out:"$(OUTDIR)/oclient.exe" 
LINK32_OBJS= \
	".\Debug\splitfra.obj" \
	".\Debug\mainview.obj" \
	".\Debug\maindoc.obj" \
	".\Debug\stdafx.obj" \
	".\Debug\rectitem.obj" \
	".\Debug\frame.obj" \
	".\Debug\oclient.obj" \
	".\Debug\oclient.res"

".\Debug\oclient.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "OClient - Macintosh Release"

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

ALL : ".\MacRel\oclient.exe" ".\MacRel\oclient.trg" ".\MacRel\oclient.HLP"

CLEAN : 
	-@erase ".\MacRel\oclient.trg"
	-@erase ".\MacRel\oclient.exe"
	-@erase ".\MacRel\mainview.obj"
	-@erase ".\MacRel\oclient.pch"
	-@erase ".\MacRel\frame.obj"
	-@erase ".\MacRel\oclient.obj"
	-@erase ".\MacRel\rectitem.obj"
	-@erase ".\MacRel\maindoc.obj"
	-@erase ".\MacRel\stdafx.obj"
	-@erase ".\MacRel\splitfra.obj"
	-@erase ".\MacRel\oclient.rsc"
	-@erase ".\MacRel\oclimac.rsc"
	-@erase ".\MacRel\oclient.HLP"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

MTL=mktyplib.exe
# ADD BASE MTL /nologo /D "NDEBUG" /mac
# ADD MTL /nologo /D "NDEBUG" /mac
MTL_PROJ=/nologo /D "NDEBUG" /mac 
CPP=cl.exe
# ADD BASE CPP /nologo /AL /Q68s /W3 /GX /O2 /D "_WINDOWS" /D "_MAC" /D "_68K_" /D "WIN32" /D "NDEBUG" /FR /YX /c
# ADD CPP /nologo /AL /Q68s /W3 /GX /O2 /D "_WINDOWS" /D "_MAC" /D "_68K_" /D "WIN32" /D "NDEBUG" /D "_MBCS" /Yu"stdafx.h" /c
# SUBTRACT CPP /Fr
CPP_PROJ=/nologo /AL /Q68s /W3 /GX /O2 /D "_WINDOWS" /D "_MAC" /D "_68K_" /D\
 "WIN32" /D "NDEBUG" /D "_MBCS" /Fp"$(INTDIR)/oclient.pch" /Yu"stdafx.h"\
 /Fo"$(INTDIR)/" /c 
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
RSC_PROJ=/l 0x409 /r /m /fo"$(INTDIR)/oclient.rsc" /d "_MAC" /d "_68K_" /d\
 "NDEBUG" 
MRC=mrc.exe
# ADD BASE MRC /D "_68K_" /D "_MAC" /D "NDEBUG" /NOLOGO
# ADD MRC /D "_68K_" /D "_MAC" /D "NDEBUG" /NOLOGO
MRC_PROJ=/D "_68K_" /D "_MAC" /D "NDEBUG" /l 0x409 /NOLOGO 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/oclient.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 wlm.lib /nologo /MAC:bundle /machine:M68K
# ADD LINK32 /nologo /MAC:bundle /MAC:creator="OCLI" /machine:M68K
LINK32_FLAGS=/nologo /MAC:bundle /MAC:type="APPL" /MAC:creator="OCLI"\
 /pdb:"$(OUTDIR)/oclient.pdb" /machine:M68K /out:"$(OUTDIR)/oclient.exe" 
LINK32_OBJS= \
	".\MacRel\mainview.obj" \
	".\MacRel\frame.obj" \
	".\MacRel\oclient.obj" \
	".\MacRel\rectitem.obj" \
	".\MacRel\maindoc.obj" \
	".\MacRel\stdafx.obj" \
	".\MacRel\splitfra.obj" \
	".\MacRel\oclient.rsc" \
	".\MacRel\oclimac.rsc"

".\MacRel\oclient.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

MFILE32=mfile.exe
# ADD BASE MFILE32 COPY /NOLOGO
# ADD MFILE32 COPY /NOLOGO
MFILE32_FLAGS=COPY /NOLOGO 
MFILE32_FILES= \
	".\MacRel\oclient.exe"

".\MacRel\oclient.trg" : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\MacRel\oclient.exe\
 "$(MFILE32_DEST)":oclient.exe

DOWNLOAD : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\MacRel\oclient.exe\
 "$(MFILE32_DEST)":oclient.exe

!ELSEIF  "$(CFG)" == "OClient - Macintosh Debug"

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

ALL : ".\MacDbg\oclient.exe" ".\MacDbg\oclient.trg" ".\MacDbg\oclient.HLP"

CLEAN : 
	-@erase ".\MacDbg\vc40.pdb"
	-@erase ".\MacDbg\oclient.pch"
	-@erase ".\MacDbg\oclient.pdb"
	-@erase ".\MacDbg\maindoc.obj"
	-@erase ".\MacDbg\mainview.obj"
	-@erase ".\MacDbg\frame.obj"
	-@erase ".\MacDbg\rectitem.obj"
	-@erase ".\MacDbg\oclient.obj"
	-@erase ".\MacDbg\stdafx.obj"
	-@erase ".\MacDbg\splitfra.obj"
	-@erase ".\MacDbg\oclient.rsc"
	-@erase ".\MacDbg\oclimac.rsc"
	-@erase ".\MacDbg\oclient.trg"
	-@erase ".\MacDbg\oclient.exe"
	-@erase ".\MacDbg\oclient.HLP"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

MTL=mktyplib.exe
# ADD BASE MTL /nologo /D "_DEBUG" /mac
# ADD MTL /nologo /D "_DEBUG" /mac
MTL_PROJ=/nologo /D "_DEBUG" /mac 
CPP=cl.exe
# ADD BASE CPP /nologo /AL /Q68s /Q68m /W3 /GX /Zi /Od /D "_WINDOWS" /D "_MAC" /D "_68K_" /D "WIN32" /D "_DEBUG" /FR /YX /c
# ADD CPP /nologo /AL /Q68s /Q68m /W3 /GX /Zi /Od /D "_WINDOWS" /D "_MAC" /D "_68K_" /D "WIN32" /D "_DEBUG" /D "_MBCS" /Yu"stdafx.h" /c
# SUBTRACT CPP /Fr
CPP_PROJ=/nologo /AL /Q68s /Q68m /W3 /GX /Zi /Od /D "_WINDOWS" /D "_MAC" /D\
 "_68K_" /D "WIN32" /D "_DEBUG" /D "_MBCS" /Fp"$(INTDIR)/oclient.pch"\
 /Yu"stdafx.h" /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
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
RSC_PROJ=/l 0x409 /r /m /fo"$(INTDIR)/oclient.rsc" /d "_MAC" /d "_68K_" /d\
 "_DEBUG" 
MRC=mrc.exe
# ADD BASE MRC /D "_68K_" /D "_MAC" /D "_DEBUG" /NOLOGO
# ADD MRC /D "_68K_" /D "_MAC" /D "_DEBUG" /NOLOGO
MRC_PROJ=/D "_68K_" /D "_MAC" /D "_DEBUG" /l 0x409 /NOLOGO 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/oclient.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 wlm.lib /nologo /MAC:bundle /debug /machine:M68K
# ADD LINK32 /nologo /MAC:bundle /MAC:creator="OCLI" /debug /machine:M68K
LINK32_FLAGS=/nologo /MAC:bundle /MAC:type="APPL" /MAC:creator="OCLI"\
 /pdb:"$(OUTDIR)/oclient.pdb" /debug /machine:M68K /out:"$(OUTDIR)/oclient.exe" 
LINK32_OBJS= \
	".\MacDbg\maindoc.obj" \
	".\MacDbg\mainview.obj" \
	".\MacDbg\frame.obj" \
	".\MacDbg\rectitem.obj" \
	".\MacDbg\oclient.obj" \
	".\MacDbg\stdafx.obj" \
	".\MacDbg\splitfra.obj" \
	".\MacDbg\oclient.rsc" \
	".\MacDbg\oclimac.rsc"

".\MacDbg\oclient.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

MFILE32=mfile.exe
# ADD BASE MFILE32 COPY /NOLOGO
# ADD MFILE32 COPY /NOLOGO
MFILE32_FLAGS=COPY /NOLOGO 
MFILE32_FILES= \
	".\MacDbg\oclient.exe"

".\MacDbg\oclient.trg" : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\MacDbg\oclient.exe\
 "$(MFILE32_DEST)":oclient.exe

DOWNLOAD : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\MacDbg\oclient.exe\
 "$(MFILE32_DEST)":oclient.exe

!ELSEIF  "$(CFG)" == "OClient - Power Macintosh Release"

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

ALL : ".\PMacRel\oclient.exe" ".\PMacRel\oclient.trg" ".\PMacRel\oclient.HLP"

CLEAN : 
	-@erase ".\PMacRel\oclient.trg"
	-@erase ".\PMacRel\oclient.exe"
	-@erase ".\PMacRel\rectitem.obj"
	-@erase ".\PMacRel\oclient.pch"
	-@erase ".\PMacRel\maindoc.obj"
	-@erase ".\PMacRel\stdafx.obj"
	-@erase ".\PMacRel\frame.obj"
	-@erase ".\PMacRel\splitfra.obj"
	-@erase ".\PMacRel\mainview.obj"
	-@erase ".\PMacRel\oclient.obj"
	-@erase ".\PMacRel\oclient.rsc"
	-@erase ".\PMacRel\oclimac.rsc"
	-@erase ".\PMacRel\oclient.HLP"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

MTL=mktyplib.exe
# ADD BASE MTL /nologo /D "NDEBUG"
# ADD MTL /nologo /D "NDEBUG"
MTL_PROJ=/nologo /D "NDEBUG" 
CPP=cl.exe
# ADD BASE CPP /nologo /ML /W3 /GX /O2 /D "_MAC" /D "_MPPC_" /D "_WINDOWS" /D "WIN32" /D "NDEBUG" /FR /YX /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "_MAC" /D "_MPPC_" /D "_WINDOWS" /D "WIN32" /D "_MBCS" /D "_AFXDLL" /Yu"stdafx.h" /c
# SUBTRACT CPP /Fr
CPP_PROJ=/nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "_MAC" /D "_MPPC_" /D\
 "_WINDOWS" /D "WIN32" /D "_MBCS" /D "_AFXDLL" /Fp"$(INTDIR)/oclient.pch"\
 /Yu"stdafx.h" /Fo"$(INTDIR)/" /c 
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
RSC_PROJ=/l 0x409 /r /m /fo"$(INTDIR)/oclient.rsc" /d "_MAC" /d "_MPPC_" /d\
 "NDEBUG" 
MRC=mrc.exe
# ADD BASE MRC /D "_MPPC_" /D "_MAC" /D "NDEBUG" /NOLOGO
# ADD MRC /D "_MPPC_" /D "_MAC" /D "NDEBUG" /NOLOGO
MRC_PROJ=/D "_MPPC_" /D "_MAC" /D "NDEBUG" /l 0x409 /NOLOGO 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/oclient.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 cfrg.rsc /nologo /mac:bundle /machine:MPPC
# ADD LINK32 /nologo /mac:bundle /mac:creator="OCLI" /machine:MPPC
LINK32_FLAGS=/nologo /mac:bundle /mac:type="APPL" /mac:creator="OCLI"\
 /incremental:no /pdb:"$(OUTDIR)/oclient.pdb" /machine:MPPC\
 /out:"$(OUTDIR)/oclient.exe" 
LINK32_OBJS= \
	".\PMacRel\rectitem.obj" \
	".\PMacRel\maindoc.obj" \
	".\PMacRel\stdafx.obj" \
	".\PMacRel\frame.obj" \
	".\PMacRel\splitfra.obj" \
	".\PMacRel\mainview.obj" \
	".\PMacRel\oclient.obj" \
	".\PMacRel\oclient.rsc" \
	".\PMacRel\oclimac.rsc"

".\PMacRel\oclient.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

MFILE32=mfile.exe
# ADD BASE MFILE32 COPY /NOLOGO
# ADD MFILE32 COPY /NOLOGO
MFILE32_FLAGS=COPY /NOLOGO 
MFILE32_FILES= \
	".\PMacRel\oclient.exe"

".\PMacRel\oclient.trg" : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\PMacRel\oclient.exe\
 "$(MFILE32_DEST)":oclient.exe

DOWNLOAD : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\PMacRel\oclient.exe\
 "$(MFILE32_DEST)":oclient.exe

!ELSEIF  "$(CFG)" == "OClient - Power Macintosh Debug"

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

ALL : ".\PMacDbg\oclient.exe" ".\PMacDbg\oclient.trg" ".\PMacDbg\oclient.HLP"

CLEAN : 
	-@erase ".\PMacDbg\vc40.pdb"
	-@erase ".\PMacDbg\oclient.pch"
	-@erase ".\PMacDbg\oclient.ilk"
	-@erase ".\PMacDbg\rectitem.obj"
	-@erase ".\PMacDbg\oclient.obj"
	-@erase ".\PMacDbg\stdafx.obj"
	-@erase ".\PMacDbg\splitfra.obj"
	-@erase ".\PMacDbg\maindoc.obj"
	-@erase ".\PMacDbg\frame.obj"
	-@erase ".\PMacDbg\mainview.obj"
	-@erase ".\PMacDbg\oclimac.rsc"
	-@erase ".\PMacDbg\oclient.rsc"
	-@erase ".\PMacDbg\oclient.pdb"
	-@erase ".\PMacDbg\oclient.trg"
	-@erase ".\PMacDbg\oclient.exe"
	-@erase ".\PMacDbg\oclient.HLP"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

MTL=mktyplib.exe
# ADD BASE MTL /nologo /D "_DEBUG"
# ADD MTL /nologo /D "_DEBUG"
MTL_PROJ=/nologo /D "_DEBUG" 
CPP=cl.exe
# ADD BASE CPP /nologo /W3 /GX /Zi /Od /D "_MAC" /D "_MPPC_" /D "_WINDOWS" /D "WIN32" /D "_DEBUG" /FR /YX /c
# ADD CPP /nologo /MDd /W3 /GX /Zi /Od /D "_DEBUG" /D "_MAC" /D "_MPPC_" /D "_WINDOWS" /D "WIN32" /D "_MBCS" /D "_AFXDLL" /Yu"stdafx.h" /c
# SUBTRACT CPP /Fr
CPP_PROJ=/nologo /MDd /W3 /GX /Zi /Od /D "_DEBUG" /D "_MAC" /D "_MPPC_" /D\
 "_WINDOWS" /D "WIN32" /D "_MBCS" /D "_AFXDLL" /Fp"$(INTDIR)/oclient.pch"\
 /Yu"stdafx.h" /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
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
RSC_PROJ=/l 0x409 /r /m /fo"$(INTDIR)/oclient.rsc" /d "_MAC" /d "_MPPC_" /d\
 "_DEBUG" 
MRC=mrc.exe
# ADD BASE MRC /D "_MPPC_" /D "_MAC" /D "_DEBUG" /NOLOGO
# ADD MRC /D "_MPPC_" /D "_MAC" /D "_DEBUG" /NOLOGO
MRC_PROJ=/D "_MPPC_" /D "_MAC" /D "_DEBUG" /l 0x409 /NOLOGO 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/oclient.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 cfrg.rsc /nologo /mac:bundle /debug /machine:MPPC
# ADD LINK32 /nologo /mac:bundle /mac:creator="OCLI" /debug /machine:MPPC
LINK32_FLAGS=/nologo /mac:bundle /mac:type="APPL" /mac:creator="OCLI"\
 /incremental:yes /pdb:"$(OUTDIR)/oclient.pdb" /debug /machine:MPPC\
 /out:"$(OUTDIR)/oclient.exe" 
LINK32_OBJS= \
	".\PMacDbg\rectitem.obj" \
	".\PMacDbg\oclient.obj" \
	".\PMacDbg\stdafx.obj" \
	".\PMacDbg\splitfra.obj" \
	".\PMacDbg\maindoc.obj" \
	".\PMacDbg\frame.obj" \
	".\PMacDbg\mainview.obj" \
	".\PMacDbg\oclimac.rsc" \
	".\PMacDbg\oclient.rsc"

".\PMacDbg\oclient.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

MFILE32=mfile.exe
# ADD BASE MFILE32 COPY /NOLOGO
# ADD MFILE32 COPY /NOLOGO
MFILE32_FLAGS=COPY /NOLOGO 
MFILE32_FILES= \
	".\PMacDbg\oclient.exe"

".\PMacDbg\oclient.trg" : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\PMacDbg\oclient.exe\
 "$(MFILE32_DEST)":oclient.exe

DOWNLOAD : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\PMacDbg\oclient.exe\
 "$(MFILE32_DEST)":oclient.exe

!ELSEIF  "$(CFG)" == "OClient - Win32 Unicode Debug"

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

ALL : ".\UniDebug\oclient.exe" ".\UniDebug\oclient.HLP"

CLEAN : 
	-@erase ".\UniDebug\vc40.pdb"
	-@erase ".\UniDebug\oclient.pch"
	-@erase ".\UniDebug\oclient.exe"
	-@erase ".\UniDebug\frame.obj"
	-@erase ".\UniDebug\maindoc.obj"
	-@erase ".\UniDebug\splitfra.obj"
	-@erase ".\UniDebug\mainview.obj"
	-@erase ".\UniDebug\stdafx.obj"
	-@erase ".\UniDebug\rectitem.obj"
	-@erase ".\UniDebug\oclient.obj"
	-@erase ".\UniDebug\oclient.res"
	-@erase ".\UniDebug\oclient.ilk"
	-@erase ".\UniDebug\oclient.pdb"
	-@erase ".\UniDebug\oclient.HLP"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /FR /YX /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_UNICODE" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
# SUBTRACT CPP /Fr
CPP_PROJ=/nologo /MDd /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS"\
 /D "_UNICODE" /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/oclient.pch" /Yu"stdafx.h"\
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
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/oclient.res" /d "_DEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/oclient.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386
# ADD LINK32 /nologo /entry:"wWinMainCRTStartup" /subsystem:windows /debug /machine:I386
LINK32_FLAGS=/nologo /entry:"wWinMainCRTStartup" /subsystem:windows\
 /incremental:yes /pdb:"$(OUTDIR)/oclient.pdb" /debug /machine:I386\
 /out:"$(OUTDIR)/oclient.exe" 
LINK32_OBJS= \
	".\UniDebug\frame.obj" \
	".\UniDebug\maindoc.obj" \
	".\UniDebug\splitfra.obj" \
	".\UniDebug\mainview.obj" \
	".\UniDebug\stdafx.obj" \
	".\UniDebug\rectitem.obj" \
	".\UniDebug\oclient.obj" \
	".\UniDebug\oclient.res"

".\UniDebug\oclient.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "OClient - Win32 Unicode Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir Release
# PROP BASE Intermediate_Dir Release
# PROP BASE Target_Dir
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 0
# PROP Output_Dir UniRel
# PROP Intermediate_Dir UniRel
# PROP Target_Dir
OUTDIR=.\UniRel
INTDIR=.\UniRel

ALL : ".\UniRel\oclient.exe" ".\UniRel\oclient.HLP"

CLEAN : 
	-@erase ".\UniRel\oclient.exe"
	-@erase ".\UniRel\mainview.obj"
	-@erase ".\UniRel\oclient.pch"
	-@erase ".\UniRel\stdafx.obj"
	-@erase ".\UniRel\maindoc.obj"
	-@erase ".\UniRel\rectitem.obj"
	-@erase ".\UniRel\frame.obj"
	-@erase ".\UniRel\splitfra.obj"
	-@erase ".\UniRel\oclient.obj"
	-@erase ".\UniRel\oclient.res"
	-@erase ".\UniRel\oclient.HLP"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /FR /YX /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_UNICODE" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
# SUBTRACT CPP /Fr
CPP_PROJ=/nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D\
 "_UNICODE" /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/oclient.pch" /Yu"stdafx.h"\
 /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\UniRel/
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
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/oclient.res" /d "NDEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/oclient.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 /nologo /entry:"wWinMainCRTStartup" /subsystem:windows /machine:I386
LINK32_FLAGS=/nologo /entry:"wWinMainCRTStartup" /subsystem:windows\
 /incremental:no /pdb:"$(OUTDIR)/oclient.pdb" /machine:I386\
 /out:"$(OUTDIR)/oclient.exe" 
LINK32_OBJS= \
	".\UniRel\mainview.obj" \
	".\UniRel\stdafx.obj" \
	".\UniRel\maindoc.obj" \
	".\UniRel\rectitem.obj" \
	".\UniRel\frame.obj" \
	".\UniRel\splitfra.obj" \
	".\UniRel\oclient.obj" \
	".\UniRel\oclient.res"

".\UniRel\oclient.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 

################################################################################
# Begin Target

# Name "OClient - Win32 Release"
# Name "OClient - Win32 Debug"
# Name "OClient - Macintosh Release"
# Name "OClient - Macintosh Debug"
# Name "OClient - Power Macintosh Release"
# Name "OClient - Power Macintosh Debug"
# Name "OClient - Win32 Unicode Debug"
# Name "OClient - Win32 Unicode Release"

!IF  "$(CFG)" == "OClient - Win32 Release"

!ELSEIF  "$(CFG)" == "OClient - Win32 Debug"

!ELSEIF  "$(CFG)" == "OClient - Macintosh Release"

!ELSEIF  "$(CFG)" == "OClient - Macintosh Debug"

!ELSEIF  "$(CFG)" == "OClient - Power Macintosh Release"

!ELSEIF  "$(CFG)" == "OClient - Power Macintosh Debug"

!ELSEIF  "$(CFG)" == "OClient - Win32 Unicode Debug"

!ELSEIF  "$(CFG)" == "OClient - Win32 Unicode Release"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\frame.cpp

!IF  "$(CFG)" == "OClient - Win32 Release"

DEP_CPP_FRAME=\
	".\stdafx.h"\
	".\oclient.h"\
	".\frame.h"\
	

".\Release\frame.obj" : $(SOURCE) $(DEP_CPP_FRAME) "$(INTDIR)"\
 ".\Release\oclient.pch"
   $(CPP) /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/oclient.pch" /Yu"stdafx.h" /Fo"$(INTDIR)/"\
 /c $(SOURCE)


!ELSEIF  "$(CFG)" == "OClient - Win32 Debug"

DEP_CPP_FRAME=\
	".\frame.h"\
	".\oclient.h"\
	".\stdafx.h"\
	

BuildCmds= \
	$(CPP) /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/oclient.pch" /Yu"stdafx.h"\
 /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c $(SOURCE) \
	

".\Debug\frame.obj" : $(SOURCE) $(DEP_CPP_FRAME) "$(INTDIR)"\
 ".\Debug\oclient.pch"
   $(BuildCmds)

".\Debug\vc40.pdb" : $(SOURCE) $(DEP_CPP_FRAME) "$(INTDIR)"\
 ".\Debug\oclient.pch"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "OClient - Macintosh Release"

DEP_CPP_FRAME=\
	".\frame.h"\
	".\oclient.h"\
	".\stdafx.h"\
	

".\MacRel\frame.obj" : $(SOURCE) $(DEP_CPP_FRAME) "$(INTDIR)"\
 ".\MacRel\oclient.pch"
   $(CPP) /nologo /AL /Q68s /W3 /GX /O2 /D "_WINDOWS" /D "_MAC" /D "_68K_" /D\
 "WIN32" /D "NDEBUG" /D "_MBCS" /Fp"$(INTDIR)/oclient.pch" /Yu"stdafx.h"\
 /Fo"$(INTDIR)/" /c $(SOURCE)


!ELSEIF  "$(CFG)" == "OClient - Macintosh Debug"

DEP_CPP_FRAME=\
	".\stdafx.h"\
	".\oclient.h"\
	".\frame.h"\
	
# SUBTRACT CPP /WX

BuildCmds= \
	$(CPP) /nologo /AL /Q68s /Q68m /W3 /GX /Zi /Od /D "_WINDOWS" /D "_MAC" /D\
 "_68K_" /D "WIN32" /D "_DEBUG" /D "_MBCS" /Fp"$(INTDIR)/oclient.pch"\
 /Yu"stdafx.h" /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c $(SOURCE) \
	

".\MacDbg\frame.obj" : $(SOURCE) $(DEP_CPP_FRAME) "$(INTDIR)"\
 ".\MacDbg\oclient.pch"
   $(BuildCmds)

".\MacDbg\vc40.pdb" : $(SOURCE) $(DEP_CPP_FRAME) "$(INTDIR)"\
 ".\MacDbg\oclient.pch"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "OClient - Power Macintosh Release"

DEP_CPP_FRAME=\
	".\stdafx.h"\
	".\oclient.h"\
	".\frame.h"\
	

".\PMacRel\frame.obj" : $(SOURCE) $(DEP_CPP_FRAME) "$(INTDIR)"\
 ".\PMacRel\oclient.pch"
   $(CPP) /nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "_MAC" /D "_MPPC_" /D\
 "_WINDOWS" /D "WIN32" /D "_MBCS" /D "_AFXDLL" /Fp"$(INTDIR)/oclient.pch"\
 /Yu"stdafx.h" /Fo"$(INTDIR)/" /c $(SOURCE)


!ELSEIF  "$(CFG)" == "OClient - Power Macintosh Debug"

DEP_CPP_FRAME=\
	".\stdafx.h"\
	".\oclient.h"\
	".\frame.h"\
	

BuildCmds= \
	$(CPP) /nologo /MDd /W3 /GX /Zi /Od /D "_DEBUG" /D "_MAC" /D "_MPPC_" /D\
 "_WINDOWS" /D "WIN32" /D "_MBCS" /D "_AFXDLL" /Fp"$(INTDIR)/oclient.pch"\
 /Yu"stdafx.h" /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c $(SOURCE) \
	

".\PMacDbg\frame.obj" : $(SOURCE) $(DEP_CPP_FRAME) "$(INTDIR)"\
 ".\PMacDbg\oclient.pch"
   $(BuildCmds)

".\PMacDbg\vc40.pdb" : $(SOURCE) $(DEP_CPP_FRAME) "$(INTDIR)"\
 ".\PMacDbg\oclient.pch"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "OClient - Win32 Unicode Debug"

DEP_CPP_FRAME=\
	".\frame.h"\
	".\oclient.h"\
	".\stdafx.h"\
	

BuildCmds= \
	$(CPP) /nologo /MDd /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS"\
 /D "_UNICODE" /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/oclient.pch" /Yu"stdafx.h"\
 /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c $(SOURCE) \
	

".\UniDebug\frame.obj" : $(SOURCE) $(DEP_CPP_FRAME) "$(INTDIR)"\
 ".\UniDebug\oclient.pch"
   $(BuildCmds)

".\UniDebug\vc40.pdb" : $(SOURCE) $(DEP_CPP_FRAME) "$(INTDIR)"\
 ".\UniDebug\oclient.pch"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "OClient - Win32 Unicode Release"

DEP_CPP_FRAME=\
	".\frame.h"\
	".\oclient.h"\
	".\stdafx.h"\
	

".\UniRel\frame.obj" : $(SOURCE) $(DEP_CPP_FRAME) "$(INTDIR)"\
 ".\UniRel\oclient.pch"
   $(CPP) /nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D\
 "_UNICODE" /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/oclient.pch" /Yu"stdafx.h"\
 /Fo"$(INTDIR)/" /c $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\maindoc.cpp

!IF  "$(CFG)" == "OClient - Win32 Release"

DEP_CPP_MAIND=\
	".\stdafx.h"\
	".\oclient.h"\
	".\maindoc.h"\
	".\rectitem.h"\
	

".\Release\maindoc.obj" : $(SOURCE) $(DEP_CPP_MAIND) "$(INTDIR)"\
 ".\Release\oclient.pch"


!ELSEIF  "$(CFG)" == "OClient - Win32 Debug"

DEP_CPP_MAIND=\
	".\maindoc.h"\
	".\oclient.h"\
	".\rectitem.h"\
	".\stdafx.h"\
	

".\Debug\maindoc.obj" : $(SOURCE) $(DEP_CPP_MAIND) "$(INTDIR)"\
 ".\Debug\oclient.pch"

".\Debug\vc40.pdb" : $(SOURCE) $(DEP_CPP_MAIND) "$(INTDIR)"\
 ".\Debug\oclient.pch"


!ELSEIF  "$(CFG)" == "OClient - Macintosh Release"

DEP_CPP_MAIND=\
	".\maindoc.h"\
	".\oclient.h"\
	".\rectitem.h"\
	".\stdafx.h"\
	

".\MacRel\maindoc.obj" : $(SOURCE) $(DEP_CPP_MAIND) "$(INTDIR)"\
 ".\MacRel\oclient.pch"


!ELSEIF  "$(CFG)" == "OClient - Macintosh Debug"

DEP_CPP_MAIND=\
	".\stdafx.h"\
	".\oclient.h"\
	".\maindoc.h"\
	".\rectitem.h"\
	

".\MacDbg\maindoc.obj" : $(SOURCE) $(DEP_CPP_MAIND) "$(INTDIR)"\
 ".\MacDbg\oclient.pch"

".\MacDbg\vc40.pdb" : $(SOURCE) $(DEP_CPP_MAIND) "$(INTDIR)"\
 ".\MacDbg\oclient.pch"


!ELSEIF  "$(CFG)" == "OClient - Power Macintosh Release"

DEP_CPP_MAIND=\
	".\stdafx.h"\
	".\oclient.h"\
	".\maindoc.h"\
	".\rectitem.h"\
	

".\PMacRel\maindoc.obj" : $(SOURCE) $(DEP_CPP_MAIND) "$(INTDIR)"\
 ".\PMacRel\oclient.pch"


!ELSEIF  "$(CFG)" == "OClient - Power Macintosh Debug"

DEP_CPP_MAIND=\
	".\stdafx.h"\
	".\oclient.h"\
	".\maindoc.h"\
	".\rectitem.h"\
	

".\PMacDbg\maindoc.obj" : $(SOURCE) $(DEP_CPP_MAIND) "$(INTDIR)"\
 ".\PMacDbg\oclient.pch"

".\PMacDbg\vc40.pdb" : $(SOURCE) $(DEP_CPP_MAIND) "$(INTDIR)"\
 ".\PMacDbg\oclient.pch"


!ELSEIF  "$(CFG)" == "OClient - Win32 Unicode Debug"

DEP_CPP_MAIND=\
	".\maindoc.h"\
	".\oclient.h"\
	".\rectitem.h"\
	".\stdafx.h"\
	

".\UniDebug\maindoc.obj" : $(SOURCE) $(DEP_CPP_MAIND) "$(INTDIR)"\
 ".\UniDebug\oclient.pch"

".\UniDebug\vc40.pdb" : $(SOURCE) $(DEP_CPP_MAIND) "$(INTDIR)"\
 ".\UniDebug\oclient.pch"


!ELSEIF  "$(CFG)" == "OClient - Win32 Unicode Release"

DEP_CPP_MAIND=\
	".\maindoc.h"\
	".\oclient.h"\
	".\rectitem.h"\
	".\stdafx.h"\
	

".\UniRel\maindoc.obj" : $(SOURCE) $(DEP_CPP_MAIND) "$(INTDIR)"\
 ".\UniRel\oclient.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\mainview.cpp

!IF  "$(CFG)" == "OClient - Win32 Release"

DEP_CPP_MAINV=\
	".\stdafx.h"\
	".\oclient.h"\
	".\maindoc.h"\
	".\mainview.h"\
	".\rectitem.h"\
	

".\Release\mainview.obj" : $(SOURCE) $(DEP_CPP_MAINV) "$(INTDIR)"\
 ".\Release\oclient.pch"


!ELSEIF  "$(CFG)" == "OClient - Win32 Debug"

DEP_CPP_MAINV=\
	".\maindoc.h"\
	".\mainview.h"\
	".\oclient.h"\
	".\rectitem.h"\
	".\stdafx.h"\
	

".\Debug\mainview.obj" : $(SOURCE) $(DEP_CPP_MAINV) "$(INTDIR)"\
 ".\Debug\oclient.pch"

".\Debug\vc40.pdb" : $(SOURCE) $(DEP_CPP_MAINV) "$(INTDIR)"\
 ".\Debug\oclient.pch"


!ELSEIF  "$(CFG)" == "OClient - Macintosh Release"

DEP_CPP_MAINV=\
	".\maindoc.h"\
	".\mainview.h"\
	".\oclient.h"\
	".\rectitem.h"\
	".\stdafx.h"\
	

".\MacRel\mainview.obj" : $(SOURCE) $(DEP_CPP_MAINV) "$(INTDIR)"\
 ".\MacRel\oclient.pch"


!ELSEIF  "$(CFG)" == "OClient - Macintosh Debug"

DEP_CPP_MAINV=\
	".\stdafx.h"\
	".\oclient.h"\
	".\maindoc.h"\
	".\mainview.h"\
	".\rectitem.h"\
	

".\MacDbg\mainview.obj" : $(SOURCE) $(DEP_CPP_MAINV) "$(INTDIR)"\
 ".\MacDbg\oclient.pch"

".\MacDbg\vc40.pdb" : $(SOURCE) $(DEP_CPP_MAINV) "$(INTDIR)"\
 ".\MacDbg\oclient.pch"


!ELSEIF  "$(CFG)" == "OClient - Power Macintosh Release"

DEP_CPP_MAINV=\
	".\stdafx.h"\
	".\oclient.h"\
	".\maindoc.h"\
	".\mainview.h"\
	".\rectitem.h"\
	

".\PMacRel\mainview.obj" : $(SOURCE) $(DEP_CPP_MAINV) "$(INTDIR)"\
 ".\PMacRel\oclient.pch"


!ELSEIF  "$(CFG)" == "OClient - Power Macintosh Debug"

DEP_CPP_MAINV=\
	".\stdafx.h"\
	".\oclient.h"\
	".\maindoc.h"\
	".\mainview.h"\
	".\rectitem.h"\
	

".\PMacDbg\mainview.obj" : $(SOURCE) $(DEP_CPP_MAINV) "$(INTDIR)"\
 ".\PMacDbg\oclient.pch"

".\PMacDbg\vc40.pdb" : $(SOURCE) $(DEP_CPP_MAINV) "$(INTDIR)"\
 ".\PMacDbg\oclient.pch"


!ELSEIF  "$(CFG)" == "OClient - Win32 Unicode Debug"

DEP_CPP_MAINV=\
	".\maindoc.h"\
	".\mainview.h"\
	".\oclient.h"\
	".\rectitem.h"\
	".\stdafx.h"\
	

".\UniDebug\mainview.obj" : $(SOURCE) $(DEP_CPP_MAINV) "$(INTDIR)"\
 ".\UniDebug\oclient.pch"

".\UniDebug\vc40.pdb" : $(SOURCE) $(DEP_CPP_MAINV) "$(INTDIR)"\
 ".\UniDebug\oclient.pch"


!ELSEIF  "$(CFG)" == "OClient - Win32 Unicode Release"

DEP_CPP_MAINV=\
	".\maindoc.h"\
	".\mainview.h"\
	".\oclient.h"\
	".\rectitem.h"\
	".\stdafx.h"\
	

".\UniRel\mainview.obj" : $(SOURCE) $(DEP_CPP_MAINV) "$(INTDIR)"\
 ".\UniRel\oclient.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\oclient.cpp

!IF  "$(CFG)" == "OClient - Win32 Release"

DEP_CPP_OCLIE=\
	".\stdafx.h"\
	".\oclient.h"\
	".\frame.h"\
	".\maindoc.h"\
	".\mainview.h"\
	".\splitfra.h"\
	

".\Release\oclient.obj" : $(SOURCE) $(DEP_CPP_OCLIE) "$(INTDIR)"\
 ".\Release\oclient.pch"


!ELSEIF  "$(CFG)" == "OClient - Win32 Debug"

DEP_CPP_OCLIE=\
	".\frame.h"\
	".\maindoc.h"\
	".\mainview.h"\
	".\oclient.h"\
	".\splitfra.h"\
	".\stdafx.h"\
	

".\Debug\oclient.obj" : $(SOURCE) $(DEP_CPP_OCLIE) "$(INTDIR)"\
 ".\Debug\oclient.pch"

".\Debug\vc40.pdb" : $(SOURCE) $(DEP_CPP_OCLIE) "$(INTDIR)"\
 ".\Debug\oclient.pch"


!ELSEIF  "$(CFG)" == "OClient - Macintosh Release"

DEP_CPP_OCLIE=\
	".\frame.h"\
	".\maindoc.h"\
	".\mainview.h"\
	".\oclient.h"\
	".\splitfra.h"\
	".\stdafx.h"\
	

".\MacRel\oclient.obj" : $(SOURCE) $(DEP_CPP_OCLIE) "$(INTDIR)"\
 ".\MacRel\oclient.pch"


!ELSEIF  "$(CFG)" == "OClient - Macintosh Debug"

DEP_CPP_OCLIE=\
	".\stdafx.h"\
	".\oclient.h"\
	".\frame.h"\
	".\maindoc.h"\
	".\mainview.h"\
	".\splitfra.h"\
	

".\MacDbg\oclient.obj" : $(SOURCE) $(DEP_CPP_OCLIE) "$(INTDIR)"\
 ".\MacDbg\oclient.pch"

".\MacDbg\vc40.pdb" : $(SOURCE) $(DEP_CPP_OCLIE) "$(INTDIR)"\
 ".\MacDbg\oclient.pch"


!ELSEIF  "$(CFG)" == "OClient - Power Macintosh Release"

DEP_CPP_OCLIE=\
	".\stdafx.h"\
	".\oclient.h"\
	".\frame.h"\
	".\maindoc.h"\
	".\mainview.h"\
	".\splitfra.h"\
	

".\PMacRel\oclient.obj" : $(SOURCE) $(DEP_CPP_OCLIE) "$(INTDIR)"\
 ".\PMacRel\oclient.pch"


!ELSEIF  "$(CFG)" == "OClient - Power Macintosh Debug"

DEP_CPP_OCLIE=\
	".\stdafx.h"\
	".\oclient.h"\
	".\frame.h"\
	".\maindoc.h"\
	".\mainview.h"\
	".\splitfra.h"\
	

".\PMacDbg\oclient.obj" : $(SOURCE) $(DEP_CPP_OCLIE) "$(INTDIR)"\
 ".\PMacDbg\oclient.pch"

".\PMacDbg\vc40.pdb" : $(SOURCE) $(DEP_CPP_OCLIE) "$(INTDIR)"\
 ".\PMacDbg\oclient.pch"


!ELSEIF  "$(CFG)" == "OClient - Win32 Unicode Debug"

DEP_CPP_OCLIE=\
	".\frame.h"\
	".\maindoc.h"\
	".\mainview.h"\
	".\oclient.h"\
	".\splitfra.h"\
	".\stdafx.h"\
	

".\UniDebug\oclient.obj" : $(SOURCE) $(DEP_CPP_OCLIE) "$(INTDIR)"\
 ".\UniDebug\oclient.pch"

".\UniDebug\vc40.pdb" : $(SOURCE) $(DEP_CPP_OCLIE) "$(INTDIR)"\
 ".\UniDebug\oclient.pch"


!ELSEIF  "$(CFG)" == "OClient - Win32 Unicode Release"

DEP_CPP_OCLIE=\
	".\frame.h"\
	".\maindoc.h"\
	".\mainview.h"\
	".\oclient.h"\
	".\splitfra.h"\
	".\stdafx.h"\
	

".\UniRel\oclient.obj" : $(SOURCE) $(DEP_CPP_OCLIE) "$(INTDIR)"\
 ".\UniRel\oclient.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\oclient.rc
DEP_RSC_OCLIEN=\
	".\res\oclient.ico"\
	".\res\toolbar.bmp"\
	

!IF  "$(CFG)" == "OClient - Win32 Release"


".\Release\oclient.res" : $(SOURCE) $(DEP_RSC_OCLIEN) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "OClient - Win32 Debug"


".\Debug\oclient.res" : $(SOURCE) $(DEP_RSC_OCLIEN) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "OClient - Macintosh Release"


".\MacRel\oclient.rsc" : $(SOURCE) $(DEP_RSC_OCLIEN) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "OClient - Macintosh Debug"


".\MacDbg\oclient.rsc" : $(SOURCE) $(DEP_RSC_OCLIEN) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "OClient - Power Macintosh Release"


".\PMacRel\oclient.rsc" : $(SOURCE) $(DEP_RSC_OCLIEN) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "OClient - Power Macintosh Debug"


".\PMacDbg\oclient.rsc" : $(SOURCE) $(DEP_RSC_OCLIEN) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "OClient - Win32 Unicode Debug"


".\UniDebug\oclient.res" : $(SOURCE) $(DEP_RSC_OCLIEN) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "OClient - Win32 Unicode Release"


".\UniRel\oclient.res" : $(SOURCE) $(DEP_RSC_OCLIEN) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\rectitem.cpp

!IF  "$(CFG)" == "OClient - Win32 Release"

DEP_CPP_RECTI=\
	".\stdafx.h"\
	".\oclient.h"\
	".\maindoc.h"\
	".\mainview.h"\
	".\rectitem.h"\
	

".\Release\rectitem.obj" : $(SOURCE) $(DEP_CPP_RECTI) "$(INTDIR)"\
 ".\Release\oclient.pch"


!ELSEIF  "$(CFG)" == "OClient - Win32 Debug"

DEP_CPP_RECTI=\
	".\maindoc.h"\
	".\mainview.h"\
	".\oclient.h"\
	".\rectitem.h"\
	".\stdafx.h"\
	

".\Debug\rectitem.obj" : $(SOURCE) $(DEP_CPP_RECTI) "$(INTDIR)"\
 ".\Debug\oclient.pch"

".\Debug\vc40.pdb" : $(SOURCE) $(DEP_CPP_RECTI) "$(INTDIR)"\
 ".\Debug\oclient.pch"


!ELSEIF  "$(CFG)" == "OClient - Macintosh Release"

DEP_CPP_RECTI=\
	".\maindoc.h"\
	".\mainview.h"\
	".\oclient.h"\
	".\rectitem.h"\
	".\stdafx.h"\
	

".\MacRel\rectitem.obj" : $(SOURCE) $(DEP_CPP_RECTI) "$(INTDIR)"\
 ".\MacRel\oclient.pch"


!ELSEIF  "$(CFG)" == "OClient - Macintosh Debug"

DEP_CPP_RECTI=\
	".\stdafx.h"\
	".\oclient.h"\
	".\maindoc.h"\
	".\mainview.h"\
	".\rectitem.h"\
	

".\MacDbg\rectitem.obj" : $(SOURCE) $(DEP_CPP_RECTI) "$(INTDIR)"\
 ".\MacDbg\oclient.pch"

".\MacDbg\vc40.pdb" : $(SOURCE) $(DEP_CPP_RECTI) "$(INTDIR)"\
 ".\MacDbg\oclient.pch"


!ELSEIF  "$(CFG)" == "OClient - Power Macintosh Release"

DEP_CPP_RECTI=\
	".\stdafx.h"\
	".\oclient.h"\
	".\maindoc.h"\
	".\mainview.h"\
	".\rectitem.h"\
	

".\PMacRel\rectitem.obj" : $(SOURCE) $(DEP_CPP_RECTI) "$(INTDIR)"\
 ".\PMacRel\oclient.pch"


!ELSEIF  "$(CFG)" == "OClient - Power Macintosh Debug"

DEP_CPP_RECTI=\
	".\stdafx.h"\
	".\oclient.h"\
	".\maindoc.h"\
	".\mainview.h"\
	".\rectitem.h"\
	

".\PMacDbg\rectitem.obj" : $(SOURCE) $(DEP_CPP_RECTI) "$(INTDIR)"\
 ".\PMacDbg\oclient.pch"

".\PMacDbg\vc40.pdb" : $(SOURCE) $(DEP_CPP_RECTI) "$(INTDIR)"\
 ".\PMacDbg\oclient.pch"


!ELSEIF  "$(CFG)" == "OClient - Win32 Unicode Debug"

DEP_CPP_RECTI=\
	".\maindoc.h"\
	".\mainview.h"\
	".\oclient.h"\
	".\rectitem.h"\
	".\stdafx.h"\
	

".\UniDebug\rectitem.obj" : $(SOURCE) $(DEP_CPP_RECTI) "$(INTDIR)"\
 ".\UniDebug\oclient.pch"

".\UniDebug\vc40.pdb" : $(SOURCE) $(DEP_CPP_RECTI) "$(INTDIR)"\
 ".\UniDebug\oclient.pch"


!ELSEIF  "$(CFG)" == "OClient - Win32 Unicode Release"

DEP_CPP_RECTI=\
	".\maindoc.h"\
	".\mainview.h"\
	".\oclient.h"\
	".\rectitem.h"\
	".\stdafx.h"\
	

".\UniRel\rectitem.obj" : $(SOURCE) $(DEP_CPP_RECTI) "$(INTDIR)"\
 ".\UniRel\oclient.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\splitfra.cpp

!IF  "$(CFG)" == "OClient - Win32 Release"

DEP_CPP_SPLIT=\
	".\stdafx.h"\
	".\oclient.h"\
	".\splitfra.h"\
	

".\Release\splitfra.obj" : $(SOURCE) $(DEP_CPP_SPLIT) "$(INTDIR)"\
 ".\Release\oclient.pch"


!ELSEIF  "$(CFG)" == "OClient - Win32 Debug"

DEP_CPP_SPLIT=\
	".\oclient.h"\
	".\splitfra.h"\
	".\stdafx.h"\
	

".\Debug\splitfra.obj" : $(SOURCE) $(DEP_CPP_SPLIT) "$(INTDIR)"\
 ".\Debug\oclient.pch"

".\Debug\vc40.pdb" : $(SOURCE) $(DEP_CPP_SPLIT) "$(INTDIR)"\
 ".\Debug\oclient.pch"


!ELSEIF  "$(CFG)" == "OClient - Macintosh Release"

DEP_CPP_SPLIT=\
	".\oclient.h"\
	".\splitfra.h"\
	".\stdafx.h"\
	

".\MacRel\splitfra.obj" : $(SOURCE) $(DEP_CPP_SPLIT) "$(INTDIR)"\
 ".\MacRel\oclient.pch"


!ELSEIF  "$(CFG)" == "OClient - Macintosh Debug"

DEP_CPP_SPLIT=\
	".\stdafx.h"\
	".\oclient.h"\
	".\splitfra.h"\
	

".\MacDbg\splitfra.obj" : $(SOURCE) $(DEP_CPP_SPLIT) "$(INTDIR)"\
 ".\MacDbg\oclient.pch"

".\MacDbg\vc40.pdb" : $(SOURCE) $(DEP_CPP_SPLIT) "$(INTDIR)"\
 ".\MacDbg\oclient.pch"


!ELSEIF  "$(CFG)" == "OClient - Power Macintosh Release"

DEP_CPP_SPLIT=\
	".\stdafx.h"\
	".\oclient.h"\
	".\splitfra.h"\
	

".\PMacRel\splitfra.obj" : $(SOURCE) $(DEP_CPP_SPLIT) "$(INTDIR)"\
 ".\PMacRel\oclient.pch"


!ELSEIF  "$(CFG)" == "OClient - Power Macintosh Debug"

DEP_CPP_SPLIT=\
	".\stdafx.h"\
	".\oclient.h"\
	".\splitfra.h"\
	

".\PMacDbg\splitfra.obj" : $(SOURCE) $(DEP_CPP_SPLIT) "$(INTDIR)"\
 ".\PMacDbg\oclient.pch"

".\PMacDbg\vc40.pdb" : $(SOURCE) $(DEP_CPP_SPLIT) "$(INTDIR)"\
 ".\PMacDbg\oclient.pch"


!ELSEIF  "$(CFG)" == "OClient - Win32 Unicode Debug"

DEP_CPP_SPLIT=\
	".\oclient.h"\
	".\splitfra.h"\
	".\stdafx.h"\
	

".\UniDebug\splitfra.obj" : $(SOURCE) $(DEP_CPP_SPLIT) "$(INTDIR)"\
 ".\UniDebug\oclient.pch"

".\UniDebug\vc40.pdb" : $(SOURCE) $(DEP_CPP_SPLIT) "$(INTDIR)"\
 ".\UniDebug\oclient.pch"


!ELSEIF  "$(CFG)" == "OClient - Win32 Unicode Release"

DEP_CPP_SPLIT=\
	".\oclient.h"\
	".\splitfra.h"\
	".\stdafx.h"\
	

".\UniRel\splitfra.obj" : $(SOURCE) $(DEP_CPP_SPLIT) "$(INTDIR)"\
 ".\UniRel\oclient.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\stdafx.cpp
DEP_CPP_STDAF=\
	".\stdafx.h"\
	

!IF  "$(CFG)" == "OClient - Win32 Release"

# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/oclient.pch" /Yc"stdafx.h" /Fo"$(INTDIR)/"\
 /c $(SOURCE) \
	

".\Release\stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

".\Release\oclient.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "OClient - Win32 Debug"

# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/oclient.pch" /Yc"stdafx.h"\
 /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c $(SOURCE) \
	

".\Debug\stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

".\Debug\vc40.pdb" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

".\Debug\oclient.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "OClient - Macintosh Release"

# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /AL /Q68s /W3 /GX /O2 /D "_WINDOWS" /D "_MAC" /D "_68K_" /D\
 "WIN32" /D "NDEBUG" /D "_MBCS" /Fp"$(INTDIR)/oclient.pch" /Yc"stdafx.h"\
 /Fo"$(INTDIR)/" /c $(SOURCE) \
	

".\MacRel\stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

".\MacRel\oclient.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "OClient - Macintosh Debug"

# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /AL /Q68s /Q68m /W3 /GX /Zi /Od /D "_WINDOWS" /D "_MAC" /D\
 "_68K_" /D "WIN32" /D "_DEBUG" /D "_MBCS" /Fp"$(INTDIR)/oclient.pch"\
 /Yc"stdafx.h" /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c $(SOURCE) \
	

".\MacDbg\stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

".\MacDbg\vc40.pdb" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

".\MacDbg\oclient.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "OClient - Power Macintosh Release"

# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "_MAC" /D "_MPPC_" /D "_WINDOWS"\
 /D "WIN32" /D "_MBCS" /D "_AFXDLL" /Fp"$(INTDIR)/oclient.pch" /Yc"stdafx.h"\
 /Fo"$(INTDIR)/" /c $(SOURCE) \
	

".\PMacRel\stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

".\PMacRel\oclient.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "OClient - Power Macintosh Debug"

# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MDd /W3 /GX /Zi /Od /D "_DEBUG" /D "_MAC" /D "_MPPC_" /D\
 "_WINDOWS" /D "WIN32" /D "_MBCS" /D "_AFXDLL" /Fp"$(INTDIR)/oclient.pch"\
 /Yc"stdafx.h" /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c $(SOURCE) \
	

".\PMacDbg\stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

".\PMacDbg\vc40.pdb" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

".\PMacDbg\oclient.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "OClient - Win32 Unicode Debug"

# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MDd /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS"\
 /D "_UNICODE" /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/oclient.pch" /Yc"stdafx.h"\
 /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c $(SOURCE) \
	

".\UniDebug\stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

".\UniDebug\vc40.pdb" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

".\UniDebug\oclient.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "OClient - Win32 Unicode Release"

# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D\
 "_UNICODE" /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/oclient.pch" /Yc"stdafx.h"\
 /Fo"$(INTDIR)/" /c $(SOURCE) \
	

".\UniRel\stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

".\UniRel\oclient.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\hlp\oclient.hpj

!IF  "$(CFG)" == "OClient - Win32 Release"

# Begin Custom Build
OutDir=.\Release
ProjDir=.
TargetName=oclient
InputPath=.\hlp\oclient.hpj

"$(OutDir)\$(TargetName).HLP" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   "$(ProjDir)\makehelp.bat"

# End Custom Build

!ELSEIF  "$(CFG)" == "OClient - Win32 Debug"

# Begin Custom Build
OutDir=.\Debug
ProjDir=.
TargetName=oclient
InputPath=.\hlp\oclient.hpj

"$(OutDir)\$(TargetName).HLP" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   "$(ProjDir)\makehelp.bat"

# End Custom Build

!ELSEIF  "$(CFG)" == "OClient - Macintosh Release"

!ELSEIF  "$(CFG)" == "OClient - Macintosh Debug"

!ELSEIF  "$(CFG)" == "OClient - Power Macintosh Release"

!ELSEIF  "$(CFG)" == "OClient - Power Macintosh Debug"

!ELSEIF  "$(CFG)" == "OClient - Win32 Unicode Debug"

# Begin Custom Build
OutDir=.\UniDebug
ProjDir=.
TargetName=oclient
InputPath=.\hlp\oclient.hpj

"$(OutDir)\$(TargetName).HLP" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   "$(ProjDir)\makehelp.bat"

# End Custom Build

!ELSEIF  "$(CFG)" == "OClient - Win32 Unicode Release"

# Begin Custom Build
OutDir=.\UniRel
ProjDir=.
TargetName=oclient
InputPath=.\hlp\oclient.hpj

"$(OutDir)\$(TargetName).HLP" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   "$(ProjDir)\makehelp.bat"

# End Custom Build

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\oclimac.r

!IF  "$(CFG)" == "OClient - Win32 Release"

!ELSEIF  "$(CFG)" == "OClient - Win32 Debug"

!ELSEIF  "$(CFG)" == "OClient - Macintosh Release"


".\MacRel\oclimac.rsc" : $(SOURCE) "$(INTDIR)"
   $(MRC) /o"$(INTDIR)/oclimac.rsc" /D "_68K_" /D "_MAC" /D "NDEBUG" /l 0x409\
 /NOLOGO $(SOURCE)


!ELSEIF  "$(CFG)" == "OClient - Macintosh Debug"


".\MacDbg\oclimac.rsc" : $(SOURCE) "$(INTDIR)"
   $(MRC) /o"$(INTDIR)/oclimac.rsc" /D "_68K_" /D "_MAC" /D "_DEBUG" /l 0x409\
 /NOLOGO $(SOURCE)


!ELSEIF  "$(CFG)" == "OClient - Power Macintosh Release"


".\PMacRel\oclimac.rsc" : $(SOURCE) "$(INTDIR)"
   $(MRC) /o"$(INTDIR)/oclimac.rsc" /D "_MPPC_" /D "_MAC" /D "NDEBUG" /l 0x409\
 /NOLOGO $(SOURCE)


!ELSEIF  "$(CFG)" == "OClient - Power Macintosh Debug"


".\PMacDbg\oclimac.rsc" : $(SOURCE) "$(INTDIR)"
   $(MRC) /o"$(INTDIR)/oclimac.rsc" /D "_MPPC_" /D "_MAC" /D "_DEBUG" /l 0x409\
 /NOLOGO $(SOURCE)


!ELSEIF  "$(CFG)" == "OClient - Win32 Unicode Debug"

!ELSEIF  "$(CFG)" == "OClient - Win32 Unicode Release"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\hlp\oclimac.hpj

!IF  "$(CFG)" == "OClient - Win32 Release"

!ELSEIF  "$(CFG)" == "OClient - Win32 Debug"

!ELSEIF  "$(CFG)" == "OClient - Macintosh Release"

# Begin Custom Build
OutDir=.\MacRel
ProjDir=.
TargetName=oclient
InputPath=.\hlp\oclimac.hpj
RemoteTargetPath=

"$(OutDir)\$(TargetName).HLP" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(ProjDir)\makehelp.bat MAC ":$(RemoteTargetPath) Help"

# End Custom Build

!ELSEIF  "$(CFG)" == "OClient - Macintosh Debug"

# Begin Custom Build
OutDir=.\MacDbg
ProjDir=.
TargetName=oclient
InputPath=.\hlp\oclimac.hpj
RemoteTargetPath=

"$(OutDir)\$(TargetName).HLP" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(ProjDir)\makehelp.bat MAC ":$(RemoteTargetPath) Help"

# End Custom Build

!ELSEIF  "$(CFG)" == "OClient - Power Macintosh Release"

# Begin Custom Build
OutDir=.\PMacRel
ProjDir=.
TargetName=oclient
InputPath=.\hlp\oclimac.hpj
RemoteTargetPath=

"$(OutDir)\$(TargetName).HLP" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(ProjDir)\makehelp.bat MAC ":$(RemoteTargetPath) Help"

# End Custom Build

!ELSEIF  "$(CFG)" == "OClient - Power Macintosh Debug"

# Begin Custom Build
OutDir=.\PMacDbg
ProjDir=.
TargetName=oclient
InputPath=.\hlp\oclimac.hpj
RemoteTargetPath=

"$(OutDir)\$(TargetName).HLP" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(ProjDir)\makehelp.bat MAC ":$(RemoteTargetPath) Help"

# End Custom Build

!ELSEIF  "$(CFG)" == "OClient - Win32 Unicode Debug"

!ELSEIF  "$(CFG)" == "OClient - Win32 Unicode Release"

!ENDIF 

# End Source File
# End Target
# End Project
################################################################################
