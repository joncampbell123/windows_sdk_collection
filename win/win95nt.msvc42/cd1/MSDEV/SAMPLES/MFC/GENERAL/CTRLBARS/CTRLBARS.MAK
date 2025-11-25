# Microsoft Developer Studio Generated NMAKE File, Format Version 40001
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101
# TARGTYPE "Power Macintosh Application" 0x0401
# TARGTYPE "Macintosh Application" 0x0301

!IF "$(CFG)" == ""
CFG=CtrlBars - Win32 Debug
!MESSAGE No configuration specified.  Defaulting to CtrlBars - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "CtrlBars - Win32 Release" && "$(CFG)" !=\
 "CtrlBars - Win32 Debug" && "$(CFG)" != "CtrlBars - Macintosh Release" &&\
 "$(CFG)" != "CtrlBars - Macintosh Debug" && "$(CFG)" !=\
 "CtrlBars - Power Macintosh Release" && "$(CFG)" !=\
 "CtrlBars - Power Macintosh Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "ctrlbars.mak" CFG="CtrlBars - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "CtrlBars - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "CtrlBars - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE "CtrlBars - Macintosh Release" (based on "Macintosh Application")
!MESSAGE "CtrlBars - Macintosh Debug" (based on "Macintosh Application")
!MESSAGE "CtrlBars - Power Macintosh Release" (based on\
 "Power Macintosh Application")
!MESSAGE "CtrlBars - Power Macintosh Debug" (based on\
 "Power Macintosh Application")
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
# PROP Target_Last_Scanned "CtrlBars - Power Macintosh Debug"

!IF  "$(CFG)" == "CtrlBars - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
OUTDIR=.\Release
INTDIR=.\Release

ALL : "$(OUTDIR)\ctrlbars.exe"

CLEAN : 
	-@erase ".\Release\ctrlbars.exe"
	-@erase ".\Release\stdafx.obj"
	-@erase ".\Release\mainfrm.obj"
	-@erase ".\Release\ctrlbars.pch"
	-@erase ".\Release\ctrlbars.obj"
	-@erase ".\Release\palette.obj"
	-@erase ".\Release\ctrlbars.res"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /FR /YX /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
# SUBTRACT CPP /Fr
CPP_PROJ=/nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/ctrlbars.pch" /Yu"stdafx.h" /Fo"$(INTDIR)/"\
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
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/ctrlbars.res" /d "NDEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/ctrlbars.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 /nologo /subsystem:windows /machine:I386
LINK32_FLAGS=/nologo /subsystem:windows /incremental:no\
 /pdb:"$(OUTDIR)/ctrlbars.pdb" /machine:I386 /out:"$(OUTDIR)/ctrlbars.exe" 
LINK32_OBJS= \
	"$(INTDIR)/stdafx.obj" \
	"$(INTDIR)/mainfrm.obj" \
	"$(INTDIR)/ctrlbars.obj" \
	"$(INTDIR)/palette.obj" \
	"$(INTDIR)/ctrlbars.res"

"$(OUTDIR)\ctrlbars.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "CtrlBars - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
OUTDIR=.\Debug
INTDIR=.\Debug

ALL : "$(OUTDIR)\ctrlbars.exe"

CLEAN : 
	-@erase ".\Debug\vc40.pdb"
	-@erase ".\Debug\ctrlbars.pch"
	-@erase ".\Debug\ctrlbars.exe"
	-@erase ".\Debug\ctrlbars.obj"
	-@erase ".\Debug\mainfrm.obj"
	-@erase ".\Debug\stdafx.obj"
	-@erase ".\Debug\palette.obj"
	-@erase ".\Debug\ctrlbars.res"
	-@erase ".\Debug\ctrlbars.ilk"
	-@erase ".\Debug\ctrlbars.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /FR /YX /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
# SUBTRACT CPP /Fr
CPP_PROJ=/nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/ctrlbars.pch" /Yu"stdafx.h"\
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
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/ctrlbars.res" /d "_DEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/ctrlbars.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386
# ADD LINK32 /nologo /subsystem:windows /debug /machine:I386
LINK32_FLAGS=/nologo /subsystem:windows /incremental:yes\
 /pdb:"$(OUTDIR)/ctrlbars.pdb" /debug /machine:I386\
 /out:"$(OUTDIR)/ctrlbars.exe" 
LINK32_OBJS= \
	"$(INTDIR)/ctrlbars.obj" \
	"$(INTDIR)/mainfrm.obj" \
	"$(INTDIR)/stdafx.obj" \
	"$(INTDIR)/palette.obj" \
	"$(INTDIR)/ctrlbars.res"

"$(OUTDIR)\ctrlbars.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "CtrlBars - Macintosh Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "MacRel"
# PROP BASE Intermediate_Dir "MacRel"
# PROP BASE Target_Dir ""
# PROP Use_MFC 1
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "MacRel"
# PROP Intermediate_Dir "MacRel"
# PROP Target_Dir ""
OUTDIR=.\MacRel
INTDIR=.\MacRel

ALL : "$(OUTDIR)\ctrlbars.exe" "$(OUTDIR)\ctrlbars.trg"

CLEAN : 
	-@erase ".\MacRel\ctrlbars.trg"
	-@erase ".\MacRel\ctrlbars.exe"
	-@erase ".\MacRel\ctrlbars.obj"
	-@erase ".\MacRel\ctrlbars.pch"
	-@erase ".\MacRel\stdafx.obj"
	-@erase ".\MacRel\mainfrm.obj"
	-@erase ".\MacRel\palette.obj"
	-@erase ".\MacRel\ctrlbars.rsc"
	-@erase ".\MacRel\ctlbmac.rsc"

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
 "WIN32" /D "NDEBUG" /D "_MBCS" /Fp"$(INTDIR)/ctrlbars.pch" /Yu"stdafx.h"\
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
RSC_PROJ=/l 0x409 /r /m /fo"$(INTDIR)/ctrlbars.rsc" /d "_MAC" /d "_68K_" /d\
 "NDEBUG" 
MRC=mrc.exe
# ADD BASE MRC /D "_68K_" /D "_MAC" /D "NDEBUG" /NOLOGO
# ADD MRC /D "_68K_" /D "_MAC" /D "NDEBUG" /NOLOGO
MRC_PROJ=/D "_68K_" /D "_MAC" /D "NDEBUG" /l 0x409 /NOLOGO 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/ctrlbars.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 wlm.lib /nologo /MAC:bundle /machine:M68K
# ADD LINK32 /nologo /MAC:bundle /MAC:creator="CTLB" /machine:M68K
LINK32_FLAGS=/nologo /MAC:bundle /MAC:type="APPL" /MAC:creator="CTLB"\
 /pdb:"$(OUTDIR)/ctrlbars.pdb" /machine:M68K /out:"$(OUTDIR)/ctrlbars.exe" 
LINK32_OBJS= \
	"$(INTDIR)/ctrlbars.obj" \
	"$(INTDIR)/stdafx.obj" \
	"$(INTDIR)/mainfrm.obj" \
	"$(INTDIR)/palette.obj" \
	"$(INTDIR)/ctrlbars.rsc" \
	"$(INTDIR)/ctlbmac.rsc"

"$(OUTDIR)\ctrlbars.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

MFILE32=mfile.exe
# ADD BASE MFILE32 COPY /NOLOGO
# ADD MFILE32 COPY /NOLOGO
MFILE32_FLAGS=COPY /NOLOGO 
MFILE32_FILES= \
	"$(OUTDIR)/ctrlbars.exe"

"$(OUTDIR)\ctrlbars.trg" : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\MacRel\ctrlbars.exe\
 "$(MFILE32_DEST):ctrlbars.exe">"$(OUTDIR)\ctrlbars.trg"

DOWNLOAD : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\MacRel\ctrlbars.exe\
 "$(MFILE32_DEST):ctrlbars.exe">"$(OUTDIR)\ctrlbars.trg"

!ELSEIF  "$(CFG)" == "CtrlBars - Macintosh Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "MacDbg"
# PROP BASE Intermediate_Dir "MacDbg"
# PROP BASE Target_Dir ""
# PROP Use_MFC 1
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "MacDbg"
# PROP Intermediate_Dir "MacDbg"
# PROP Target_Dir ""
OUTDIR=.\MacDbg
INTDIR=.\MacDbg

ALL : "$(OUTDIR)\ctrlbars.exe" "$(OUTDIR)\ctrlbars.trg"

CLEAN : 
	-@erase ".\MacDbg\vc40.pdb"
	-@erase ".\MacDbg\ctrlbars.pch"
	-@erase ".\MacDbg\ctrlbars.pdb"
	-@erase ".\MacDbg\ctrlbars.obj"
	-@erase ".\MacDbg\stdafx.obj"
	-@erase ".\MacDbg\mainfrm.obj"
	-@erase ".\MacDbg\palette.obj"
	-@erase ".\MacDbg\ctlbmac.rsc"
	-@erase ".\MacDbg\ctrlbars.rsc"
	-@erase ".\MacDbg\ctrlbars.trg"
	-@erase ".\MacDbg\ctrlbars.exe"

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
 "_68K_" /D "WIN32" /D "_DEBUG" /D "_MBCS" /Fp"$(INTDIR)/ctrlbars.pch"\
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
RSC_PROJ=/l 0x409 /r /m /fo"$(INTDIR)/ctrlbars.rsc" /d "_MAC" /d "_68K_" /d\
 "_DEBUG" 
MRC=mrc.exe
# ADD BASE MRC /D "_68K_" /D "_MAC" /D "_DEBUG" /NOLOGO
# ADD MRC /D "_68K_" /D "_MAC" /D "_DEBUG" /NOLOGO
MRC_PROJ=/D "_68K_" /D "_MAC" /D "_DEBUG" /l 0x409 /NOLOGO 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/ctrlbars.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 wlm.lib /nologo /MAC:bundle /debug /machine:M68K
# ADD LINK32 /nologo /MAC:bundle /MAC:creator="CTLB" /debug /machine:M68K
LINK32_FLAGS=/nologo /MAC:bundle /MAC:type="APPL" /MAC:creator="CTLB"\
 /pdb:"$(OUTDIR)/ctrlbars.pdb" /debug /machine:M68K\
 /out:"$(OUTDIR)/ctrlbars.exe" 
LINK32_OBJS= \
	"$(INTDIR)/ctrlbars.obj" \
	"$(INTDIR)/stdafx.obj" \
	"$(INTDIR)/mainfrm.obj" \
	"$(INTDIR)/palette.obj" \
	"$(INTDIR)/ctlbmac.rsc" \
	"$(INTDIR)/ctrlbars.rsc"

"$(OUTDIR)\ctrlbars.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

MFILE32=mfile.exe
# ADD BASE MFILE32 COPY /NOLOGO
# ADD MFILE32 COPY /NOLOGO
MFILE32_FLAGS=COPY /NOLOGO 
MFILE32_FILES= \
	"$(OUTDIR)/ctrlbars.exe"

"$(OUTDIR)\ctrlbars.trg" : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\MacDbg\ctrlbars.exe\
 "$(MFILE32_DEST):ctrlbars.exe">"$(OUTDIR)\ctrlbars.trg"

DOWNLOAD : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\MacDbg\ctrlbars.exe\
 "$(MFILE32_DEST):ctrlbars.exe">"$(OUTDIR)\ctrlbars.trg"

!ELSEIF  "$(CFG)" == "CtrlBars - Power Macintosh Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "PMacRel"
# PROP BASE Intermediate_Dir "PMacRel"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "PMacRel"
# PROP Intermediate_Dir "PMacRel"
# PROP Target_Dir ""
OUTDIR=.\PMacRel
INTDIR=.\PMacRel

ALL : "$(OUTDIR)\ctrlbars.exe" "$(OUTDIR)\ctrlbars.trg"

CLEAN : 
	-@erase ".\PMacRel\ctrlbars.trg"
	-@erase ".\PMacRel\ctrlbars.exe"
	-@erase ".\PMacRel\mainfrm.obj"
	-@erase ".\PMacRel\ctrlbars.pch"
	-@erase ".\PMacRel\palette.obj"
	-@erase ".\PMacRel\stdafx.obj"
	-@erase ".\PMacRel\ctrlbars.obj"
	-@erase ".\PMacRel\ctlbmac.rsc"
	-@erase ".\PMacRel\ctrlbars.rsc"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

MTL=mktyplib.exe
# ADD BASE MTL /nologo /D "NDEBUG"
# ADD MTL /nologo /D "NDEBUG"
MTL_PROJ=/nologo /D "NDEBUG" 
CPP=cl.exe
# ADD BASE CPP /nologo /W3 /GX /O2 /D "_MAC" /D "_MPPC_" /D "_WINDOWS" /D "WIN32" /D "NDEBUG" /FR /YX /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "_MAC" /D "_MPPC_" /D "_WINDOWS" /D "WIN32" /D "_MBCS" /D "_AFXDLL" /Yu"stdafx.h" /c
# SUBTRACT CPP /Fr
CPP_PROJ=/nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "_MAC" /D "_MPPC_" /D\
 "_WINDOWS" /D "WIN32" /D "_MBCS" /D "_AFXDLL" /Fp"$(INTDIR)/ctrlbars.pch"\
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
RSC_PROJ=/l 0x409 /r /m /fo"$(INTDIR)/ctrlbars.rsc" /d "_MAC" /d "_MPPC_" /d\
 "NDEBUG" 
MRC=mrc.exe
# ADD BASE MRC /D "_MPPC_" /D "_MAC" /D "NDEBUG" /NOLOGO
# ADD MRC /D "_MPPC_" /D "_MAC" /D "NDEBUG" /NOLOGO
MRC_PROJ=/D "_MPPC_" /D "_MAC" /D "NDEBUG" /l 0x409 /NOLOGO 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/ctrlbars.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 cfrg.rsc /nologo /mac:bundle /machine:MPPC
# ADD LINK32 /nologo /mac:bundle /mac:creator="CTLB" /machine:MPPC
LINK32_FLAGS=/nologo /mac:bundle /mac:type="APPL" /mac:creator="CTLB"\
 /incremental:no /pdb:"$(OUTDIR)/ctrlbars.pdb" /machine:MPPC\
 /out:"$(OUTDIR)/ctrlbars.exe" 
LINK32_OBJS= \
	"$(INTDIR)/mainfrm.obj" \
	"$(INTDIR)/palette.obj" \
	"$(INTDIR)/stdafx.obj" \
	"$(INTDIR)/ctrlbars.obj" \
	"$(INTDIR)/ctlbmac.rsc" \
	"$(INTDIR)/ctrlbars.rsc"

"$(OUTDIR)\ctrlbars.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

MFILE32=mfile.exe
# ADD BASE MFILE32 COPY /NOLOGO
# ADD MFILE32 COPY /NOLOGO
MFILE32_FLAGS=COPY /NOLOGO 
MFILE32_FILES= \
	"$(OUTDIR)/ctrlbars.exe"

"$(OUTDIR)\ctrlbars.trg" : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\PMacRel\ctrlbars.exe\
 "$(MFILE32_DEST):ctrlbars.exe">"$(OUTDIR)\ctrlbars.trg"

DOWNLOAD : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\PMacRel\ctrlbars.exe\
 "$(MFILE32_DEST):ctrlbars.exe">"$(OUTDIR)\ctrlbars.trg"

!ELSEIF  "$(CFG)" == "CtrlBars - Power Macintosh Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "PMacDbg"
# PROP BASE Intermediate_Dir "PMacDbg"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "PMacDbg"
# PROP Intermediate_Dir "PMacDbg"
# PROP Target_Dir ""
OUTDIR=.\PMacDbg
INTDIR=.\PMacDbg

ALL : "$(OUTDIR)\ctrlbars.exe" "$(OUTDIR)\ctrlbars.trg"

CLEAN : 
	-@erase ".\PMacDbg\vc40.pdb"
	-@erase ".\PMacDbg\ctrlbars.pch"
	-@erase ".\PMacDbg\ctrlbars.ilk"
	-@erase ".\PMacDbg\mainfrm.obj"
	-@erase ".\PMacDbg\palette.obj"
	-@erase ".\PMacDbg\stdafx.obj"
	-@erase ".\PMacDbg\ctrlbars.obj"
	-@erase ".\PMacDbg\ctrlbars.rsc"
	-@erase ".\PMacDbg\ctlbmac.rsc"
	-@erase ".\PMacDbg\ctrlbars.pdb"
	-@erase ".\PMacDbg\ctrlbars.trg"
	-@erase ".\PMacDbg\ctrlbars.exe"

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
 "_WINDOWS" /D "WIN32" /D "_MBCS" /D "_AFXDLL" /Fp"$(INTDIR)/ctrlbars.pch"\
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
RSC_PROJ=/l 0x409 /r /m /fo"$(INTDIR)/ctrlbars.rsc" /d "_MAC" /d "_MPPC_" /d\
 "_DEBUG" 
MRC=mrc.exe
# ADD BASE MRC /D "_MPPC_" /D "_MAC" /D "_DEBUG" /NOLOGO
# ADD MRC /D "_MPPC_" /D "_MAC" /D "_DEBUG" /NOLOGO
MRC_PROJ=/D "_MPPC_" /D "_MAC" /D "_DEBUG" /l 0x409 /NOLOGO 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/ctrlbars.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 cfrg.rsc /nologo /mac:bundle /debug /machine:MPPC
# ADD LINK32 /nologo /mac:bundle /mac:creator="CTLB" /debug /machine:MPPC
LINK32_FLAGS=/nologo /mac:bundle /mac:type="APPL" /mac:creator="CTLB"\
 /incremental:yes /pdb:"$(OUTDIR)/ctrlbars.pdb" /debug /machine:MPPC\
 /out:"$(OUTDIR)/ctrlbars.exe" 
LINK32_OBJS= \
	"$(INTDIR)/mainfrm.obj" \
	"$(INTDIR)/palette.obj" \
	"$(INTDIR)/stdafx.obj" \
	"$(INTDIR)/ctrlbars.obj" \
	"$(INTDIR)/ctrlbars.rsc" \
	"$(INTDIR)/ctlbmac.rsc"

"$(OUTDIR)\ctrlbars.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

MFILE32=mfile.exe
# ADD BASE MFILE32 COPY /NOLOGO
# ADD MFILE32 COPY /NOLOGO
MFILE32_FLAGS=COPY /NOLOGO 
MFILE32_FILES= \
	"$(OUTDIR)/ctrlbars.exe"

"$(OUTDIR)\ctrlbars.trg" : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\PMacDbg\ctrlbars.exe\
 "$(MFILE32_DEST):ctrlbars.exe">"$(OUTDIR)\ctrlbars.trg"

DOWNLOAD : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\PMacDbg\ctrlbars.exe\
 "$(MFILE32_DEST):ctrlbars.exe">"$(OUTDIR)\ctrlbars.trg"

!ENDIF 

################################################################################
# Begin Target

# Name "CtrlBars - Win32 Release"
# Name "CtrlBars - Win32 Debug"
# Name "CtrlBars - Macintosh Release"
# Name "CtrlBars - Macintosh Debug"
# Name "CtrlBars - Power Macintosh Release"
# Name "CtrlBars - Power Macintosh Debug"

!IF  "$(CFG)" == "CtrlBars - Win32 Release"

!ELSEIF  "$(CFG)" == "CtrlBars - Win32 Debug"

!ELSEIF  "$(CFG)" == "CtrlBars - Macintosh Release"

!ELSEIF  "$(CFG)" == "CtrlBars - Macintosh Debug"

!ELSEIF  "$(CFG)" == "CtrlBars - Power Macintosh Release"

!ELSEIF  "$(CFG)" == "CtrlBars - Power Macintosh Debug"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\ctrlbars.cpp

!IF  "$(CFG)" == "CtrlBars - Win32 Release"

DEP_CPP_CTRLB=\
	".\stdafx.h"\
	".\ctrlbars.h"\
	".\mainfrm.h"\
	".\palette.h"\
	

"$(INTDIR)\ctrlbars.obj" : $(SOURCE) $(DEP_CPP_CTRLB) "$(INTDIR)"\
 "$(INTDIR)\ctrlbars.pch"


!ELSEIF  "$(CFG)" == "CtrlBars - Win32 Debug"

DEP_CPP_CTRLB=\
	".\ctrlbars.h"\
	".\resource.h"\
	".\stdafx.h"\
	

"$(INTDIR)\ctrlbars.obj" : $(SOURCE) $(DEP_CPP_CTRLB) "$(INTDIR)"\
 "$(INTDIR)\ctrlbars.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_CTRLB) "$(INTDIR)"\
 "$(INTDIR)\ctrlbars.pch"


!ELSEIF  "$(CFG)" == "CtrlBars - Macintosh Release"

DEP_CPP_CTRLB=\
	".\ctrlbars.h"\
	".\mainfrm.h"\
	".\palette.h"\
	".\stdafx.h"\
	

"$(INTDIR)\ctrlbars.obj" : $(SOURCE) $(DEP_CPP_CTRLB) "$(INTDIR)"\
 "$(INTDIR)\ctrlbars.pch"


!ELSEIF  "$(CFG)" == "CtrlBars - Macintosh Debug"

DEP_CPP_CTRLB=\
	".\ctrlbars.h"\
	".\mainfrm.h"\
	".\palette.h"\
	".\stdafx.h"\
	

"$(INTDIR)\ctrlbars.obj" : $(SOURCE) $(DEP_CPP_CTRLB) "$(INTDIR)"\
 "$(INTDIR)\ctrlbars.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_CTRLB) "$(INTDIR)"\
 "$(INTDIR)\ctrlbars.pch"


!ELSEIF  "$(CFG)" == "CtrlBars - Power Macintosh Release"

DEP_CPP_CTRLB=\
	".\ctrlbars.h"\
	".\mainfrm.h"\
	".\palette.h"\
	".\stdafx.h"\
	

"$(INTDIR)\ctrlbars.obj" : $(SOURCE) $(DEP_CPP_CTRLB) "$(INTDIR)"\
 "$(INTDIR)\ctrlbars.pch"


!ELSEIF  "$(CFG)" == "CtrlBars - Power Macintosh Debug"

DEP_CPP_CTRLB=\
	".\ctrlbars.h"\
	".\mainfrm.h"\
	".\palette.h"\
	".\stdafx.h"\
	

"$(INTDIR)\ctrlbars.obj" : $(SOURCE) $(DEP_CPP_CTRLB) "$(INTDIR)"\
 "$(INTDIR)\ctrlbars.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_CTRLB) "$(INTDIR)"\
 "$(INTDIR)\ctrlbars.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\ctrlbars.rc
DEP_RSC_CTRLBA=\
	".\res\ctrlbars.ico"\
	".\res\toolbar.bmp"\
	".\res\styles.bmp"\
	".\res\palette.bmp"\
	

!IF  "$(CFG)" == "CtrlBars - Win32 Release"


"$(INTDIR)\ctrlbars.res" : $(SOURCE) $(DEP_RSC_CTRLBA) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "CtrlBars - Win32 Debug"


"$(INTDIR)\ctrlbars.res" : $(SOURCE) $(DEP_RSC_CTRLBA) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "CtrlBars - Macintosh Release"


"$(INTDIR)\ctrlbars.rsc" : $(SOURCE) $(DEP_RSC_CTRLBA) "$(INTDIR)"
   $(RSC) /l 0x409 /r /m /fo"$(INTDIR)/ctrlbars.rsc" /d "_MAC" /d "_68K_" /d\
 "NDEBUG" $(SOURCE)


!ELSEIF  "$(CFG)" == "CtrlBars - Macintosh Debug"


"$(INTDIR)\ctrlbars.rsc" : $(SOURCE) $(DEP_RSC_CTRLBA) "$(INTDIR)"
   $(RSC) /l 0x409 /r /m /fo"$(INTDIR)/ctrlbars.rsc" /d "_MAC" /d "_68K_" /d\
 "_DEBUG" $(SOURCE)


!ELSEIF  "$(CFG)" == "CtrlBars - Power Macintosh Release"


"$(INTDIR)\ctrlbars.rsc" : $(SOURCE) $(DEP_RSC_CTRLBA) "$(INTDIR)"
   $(RSC) /l 0x409 /r /m /fo"$(INTDIR)/ctrlbars.rsc" /d "_MAC" /d "_MPPC_" /d\
 "NDEBUG" $(SOURCE)


!ELSEIF  "$(CFG)" == "CtrlBars - Power Macintosh Debug"


"$(INTDIR)\ctrlbars.rsc" : $(SOURCE) $(DEP_RSC_CTRLBA) "$(INTDIR)"
   $(RSC) /l 0x409 /r /m /fo"$(INTDIR)/ctrlbars.rsc" /d "_MAC" /d "_MPPC_" /d\
 "_DEBUG" $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\mainfrm.cpp

!IF  "$(CFG)" == "CtrlBars - Win32 Release"

DEP_CPP_MAINF=\
	".\ctrlbars.h"\
	".\mainfrm.h"\
	".\palette.h"\
	".\stdafx.h"\
	

"$(INTDIR)\mainfrm.obj" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\ctrlbars.pch"


!ELSEIF  "$(CFG)" == "CtrlBars - Win32 Debug"

DEP_CPP_MAINF=\
	".\ctrlbars.h"\
	".\resource.h"\
	".\stdafx.h"\
	

"$(INTDIR)\mainfrm.obj" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\ctrlbars.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\ctrlbars.pch"


!ELSEIF  "$(CFG)" == "CtrlBars - Macintosh Release"

DEP_CPP_MAINF=\
	".\ctrlbars.h"\
	".\mainfrm.h"\
	".\palette.h"\
	".\stdafx.h"\
	

"$(INTDIR)\mainfrm.obj" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\ctrlbars.pch"


!ELSEIF  "$(CFG)" == "CtrlBars - Macintosh Debug"

DEP_CPP_MAINF=\
	".\ctrlbars.h"\
	".\mainfrm.h"\
	".\palette.h"\
	".\stdafx.h"\
	

"$(INTDIR)\mainfrm.obj" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\ctrlbars.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\ctrlbars.pch"


!ELSEIF  "$(CFG)" == "CtrlBars - Power Macintosh Release"

DEP_CPP_MAINF=\
	".\ctrlbars.h"\
	".\mainfrm.h"\
	".\palette.h"\
	".\stdafx.h"\
	

"$(INTDIR)\mainfrm.obj" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\ctrlbars.pch"


!ELSEIF  "$(CFG)" == "CtrlBars - Power Macintosh Debug"

DEP_CPP_MAINF=\
	".\ctrlbars.h"\
	".\mainfrm.h"\
	".\palette.h"\
	".\stdafx.h"\
	

"$(INTDIR)\mainfrm.obj" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\ctrlbars.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\ctrlbars.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\palette.cpp

!IF  "$(CFG)" == "CtrlBars - Win32 Release"

DEP_CPP_PALET=\
	".\ctrlbars.h"\
	".\palette.h"\
	".\stdafx.h"\
	

"$(INTDIR)\palette.obj" : $(SOURCE) $(DEP_CPP_PALET) "$(INTDIR)"\
 "$(INTDIR)\ctrlbars.pch"


!ELSEIF  "$(CFG)" == "CtrlBars - Win32 Debug"

DEP_CPP_PALET=\
	".\ctrlbars.h"\
	".\resource.h"\
	".\palette.h"\
	".\stdafx.h"\
	

"$(INTDIR)\palette.obj" : $(SOURCE) $(DEP_CPP_PALET) "$(INTDIR)"\
 "$(INTDIR)\ctrlbars.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_PALET) "$(INTDIR)"\
 "$(INTDIR)\ctrlbars.pch"


!ELSEIF  "$(CFG)" == "CtrlBars - Macintosh Release"

DEP_CPP_PALET=\
	".\ctrlbars.h"\
	".\palette.h"\
	".\stdafx.h"\
	

"$(INTDIR)\palette.obj" : $(SOURCE) $(DEP_CPP_PALET) "$(INTDIR)"\
 "$(INTDIR)\ctrlbars.pch"


!ELSEIF  "$(CFG)" == "CtrlBars - Macintosh Debug"

DEP_CPP_PALET=\
	".\ctrlbars.h"\
	".\palette.h"\
	".\stdafx.h"\
	

"$(INTDIR)\palette.obj" : $(SOURCE) $(DEP_CPP_PALET) "$(INTDIR)"\
 "$(INTDIR)\ctrlbars.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_PALET) "$(INTDIR)"\
 "$(INTDIR)\ctrlbars.pch"


!ELSEIF  "$(CFG)" == "CtrlBars - Power Macintosh Release"

DEP_CPP_PALET=\
	".\ctrlbars.h"\
	".\palette.h"\
	".\stdafx.h"\
	

"$(INTDIR)\palette.obj" : $(SOURCE) $(DEP_CPP_PALET) "$(INTDIR)"\
 "$(INTDIR)\ctrlbars.pch"


!ELSEIF  "$(CFG)" == "CtrlBars - Power Macintosh Debug"

DEP_CPP_PALET=\
	".\ctrlbars.h"\
	".\palette.h"\
	".\stdafx.h"\
	

"$(INTDIR)\palette.obj" : $(SOURCE) $(DEP_CPP_PALET) "$(INTDIR)"\
 "$(INTDIR)\ctrlbars.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_PALET) "$(INTDIR)"\
 "$(INTDIR)\ctrlbars.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\ctlbmac.r

!IF  "$(CFG)" == "CtrlBars - Win32 Release"

!ELSEIF  "$(CFG)" == "CtrlBars - Win32 Debug"

!ELSEIF  "$(CFG)" == "CtrlBars - Macintosh Release"

DEP_MRC_CTLBM=\
	{$(INCLUDE)}"\mrc\TYPES.R"\
	{$(INCLUDE)}"\mrc\BALLOONS.R"\
	

"$(INTDIR)\ctlbmac.rsc" : $(SOURCE) $(DEP_MRC_CTLBM) "$(INTDIR)"
   $(MRC) /o"$(INTDIR)/ctlbmac.rsc" /D "_68K_" /D "_MAC" /D "NDEBUG" /l 0x409\
 /NOLOGO $(SOURCE)


!ELSEIF  "$(CFG)" == "CtrlBars - Macintosh Debug"

DEP_MRC_CTLBM=\
	{$(INCLUDE)}"\mrc\TYPES.R"\
	{$(INCLUDE)}"\mrc\BALLOONS.R"\
	

"$(INTDIR)\ctlbmac.rsc" : $(SOURCE) $(DEP_MRC_CTLBM) "$(INTDIR)"
   $(MRC) /o"$(INTDIR)/ctlbmac.rsc" /D "_68K_" /D "_MAC" /D "_DEBUG" /l 0x409\
 /NOLOGO $(SOURCE)


!ELSEIF  "$(CFG)" == "CtrlBars - Power Macintosh Release"

DEP_MRC_CTLBM=\
	{$(INCLUDE)}"\mrc\TYPES.R"\
	{$(INCLUDE)}"\mrc\BALLOONS.R"\
	

"$(INTDIR)\ctlbmac.rsc" : $(SOURCE) $(DEP_MRC_CTLBM) "$(INTDIR)"
   $(MRC) /o"$(INTDIR)/ctlbmac.rsc" /D "_MPPC_" /D "_MAC" /D "NDEBUG" /l 0x409\
 /NOLOGO $(SOURCE)


!ELSEIF  "$(CFG)" == "CtrlBars - Power Macintosh Debug"

DEP_MRC_CTLBM=\
	{$(INCLUDE)}"\mrc\TYPES.R"\
	{$(INCLUDE)}"\mrc\BALLOONS.R"\
	

"$(INTDIR)\ctlbmac.rsc" : $(SOURCE) $(DEP_MRC_CTLBM) "$(INTDIR)"
   $(MRC) /o"$(INTDIR)/ctlbmac.rsc" /D "_MPPC_" /D "_MAC" /D "_DEBUG" /l 0x409\
 /NOLOGO $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\stdafx.cpp
DEP_CPP_STDAF=\
	".\stdafx.h"\
	

!IF  "$(CFG)" == "CtrlBars - Win32 Release"

# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/ctrlbars.pch" /Yc"stdafx.h" /Fo"$(INTDIR)/"\
 /c $(SOURCE) \
	

"$(INTDIR)\stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\ctrlbars.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "CtrlBars - Win32 Debug"

# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/ctrlbars.pch" /Yc"stdafx.h"\
 /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\ctrlbars.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "CtrlBars - Macintosh Release"

# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /AL /Q68s /W3 /GX /O2 /D "_WINDOWS" /D "_MAC" /D "_68K_" /D\
 "WIN32" /D "NDEBUG" /D "_MBCS" /Fp"$(INTDIR)/ctrlbars.pch" /Yc"stdafx.h"\
 /Fo"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\ctrlbars.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "CtrlBars - Macintosh Debug"

# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /AL /Q68s /Q68m /W3 /GX /Zi /Od /D "_WINDOWS" /D "_MAC" /D\
 "_68K_" /D "WIN32" /D "_DEBUG" /D "_MBCS" /Fp"$(INTDIR)/ctrlbars.pch"\
 /Yc"stdafx.h" /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\ctrlbars.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "CtrlBars - Power Macintosh Release"

# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "_MAC" /D "_MPPC_" /D "_WINDOWS"\
 /D "WIN32" /D "_MBCS" /D "_AFXDLL" /Fp"$(INTDIR)/ctrlbars.pch" /Yc"stdafx.h"\
 /Fo"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\ctrlbars.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "CtrlBars - Power Macintosh Debug"

# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MDd /W3 /GX /Zi /Od /D "_DEBUG" /D "_MAC" /D "_MPPC_" /D\
 "_WINDOWS" /D "WIN32" /D "_MBCS" /D "_AFXDLL" /Fp"$(INTDIR)/ctrlbars.pch"\
 /Yc"stdafx.h" /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\ctrlbars.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
# End Target
# End Project
################################################################################
