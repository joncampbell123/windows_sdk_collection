# Microsoft Visual C++ Generated NMAKE File, Format Version 40000
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101
# TARGTYPE "Power Macintosh Application" 0x0401
# TARGTYPE "Macintosh Application" 0x0301

!IF "$(CFG)" == ""
CFG=crsrdemo - Win32 Debug
!MESSAGE No configuration specified.  Defaulting to crsrdemo - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "crsrdemo - Win32 Release" && "$(CFG)" !=\
 "crsrdemo - Win32 Debug" && "$(CFG)" != "crsrdemo - Macintosh Release" &&\
 "$(CFG)" != "crsrdemo - Macintosh Debug" && "$(CFG)" !=\
 "crsrdemo - Power Macintosh Release" && "$(CFG)" !=\
 "crsrdemo - Power Macintosh Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "crsrdemo.mak" CFG="crsrdemo - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "crsrdemo - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "crsrdemo - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE "crsrdemo - Macintosh Release" (based on "Macintosh Application")
!MESSAGE "crsrdemo - Macintosh Debug" (based on "Macintosh Application")
!MESSAGE "crsrdemo - Power Macintosh Release" (based on\
 "Power Macintosh Application")
!MESSAGE "crsrdemo - Power Macintosh Debug" (based on\
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
# PROP Target_Last_Scanned "crsrdemo - Power Macintosh Debug"

!IF  "$(CFG)" == "crsrdemo - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir Release
# PROP BASE Intermediate_Dir Release
# PROP BASE Target_Dir
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir Release
# PROP Intermediate_Dir Release
# PROP Target_Dir
OUTDIR=.\Release
INTDIR=.\Release

ALL : "$(OUTDIR)\crsrdemo.exe"

CLEAN : 
	-@erase ".\Release\crsrdemo.exe"
	-@erase ".\Release\frame.obj"
	-@erase ".\Release\crsrdemo.obj"
	-@erase ".\Release\child.obj"
	-@erase ".\Release\headers.obj"
	-@erase ".\Release\dialogs.obj"
	-@erase ".\Release\crsrdemo.res"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
CPP_PROJ=/nologo /ML /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS"\
 /Fp"$(INTDIR)/crsrdemo.pch" /YX /Fo"$(INTDIR)/" /c 
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
# ADD RSC /l 0x409 /d "NDEBUG"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/crsrdemo.res" /d "NDEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/crsrdemo.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib ctl3d32.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib ctl3d32.lib odbc32.lib\
 odbccp32.lib /nologo /subsystem:windows /incremental:no\
 /pdb:"$(OUTDIR)/crsrdemo.pdb" /machine:I386 /out:"$(OUTDIR)/crsrdemo.exe" 
LINK32_OBJS= \
	"$(INTDIR)/frame.obj" \
	"$(INTDIR)/crsrdemo.obj" \
	"$(INTDIR)/child.obj" \
	"$(INTDIR)/headers.obj" \
	"$(INTDIR)/dialogs.obj" \
	"$(INTDIR)/crsrdemo.res"

"$(OUTDIR)\crsrdemo.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "crsrdemo - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir Debug
# PROP BASE Intermediate_Dir Debug
# PROP BASE Target_Dir
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir Debug
# PROP Intermediate_Dir Debug
# PROP Target_Dir
OUTDIR=.\Debug
INTDIR=.\Debug

ALL : "$(OUTDIR)\crsrdemo.exe"

CLEAN : 
	-@erase ".\Debug\vc40.pdb"
	-@erase ".\Debug\crsrdemo.pch"
	-@erase ".\Debug\crsrdemo.exe"
	-@erase ".\Debug\headers.obj"
	-@erase ".\Debug\frame.obj"
	-@erase ".\Debug\crsrdemo.obj"
	-@erase ".\Debug\child.obj"
	-@erase ".\Debug\dialogs.obj"
	-@erase ".\Debug\crsrdemo.res"
	-@erase ".\Debug\crsrdemo.ilk"
	-@erase ".\Debug\crsrdemo.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /Yu"headers.h" /c
CPP_PROJ=/nologo /MLd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /Fp"$(INTDIR)/crsrdemo.pch" /Yu"headers.h" /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
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
# ADD RSC /l 0x409 /d "_DEBUG"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/crsrdemo.res" /d "_DEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/crsrdemo.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib ctl3d32.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib ctl3d32.lib odbc32.lib\
 odbccp32.lib /nologo /subsystem:windows /incremental:yes\
 /pdb:"$(OUTDIR)/crsrdemo.pdb" /debug /machine:I386\
 /out:"$(OUTDIR)/crsrdemo.exe" 
LINK32_OBJS= \
	"$(INTDIR)/headers.obj" \
	"$(INTDIR)/frame.obj" \
	"$(INTDIR)/crsrdemo.obj" \
	"$(INTDIR)/child.obj" \
	"$(INTDIR)/dialogs.obj" \
	"$(INTDIR)/crsrdemo.res"

"$(OUTDIR)\crsrdemo.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "crsrdemo - Macintosh Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir MacRel
# PROP BASE Intermediate_Dir MacRel
# PROP BASE Target_Dir
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir MacRel
# PROP Intermediate_Dir MacRel
# PROP Target_Dir
OUTDIR=.\MacRel
INTDIR=.\MacRel

ALL : "$(OUTDIR)\crsrdemo.exe" "$(OUTDIR)\crsrdemo.trg"

CLEAN : 
	-@erase ".\MacRel\crsrdemo.trg"
	-@erase ".\MacRel\crsrdemo.exe"
	-@erase ".\MacRel\headers.obj"
	-@erase ".\MacRel\frame.obj"
	-@erase ".\MacRel\dialogs.obj"
	-@erase ".\MacRel\child.obj"
	-@erase ".\MacRel\crsrdemo.obj"
	-@erase ".\MacRel\crsrdemo.rsc"
	-@erase ".\MacRel\crsrmac.rsc"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

MTL=mktyplib.exe
# ADD BASE MTL /nologo /D "NDEBUG" /mac
# ADD MTL /nologo /D "NDEBUG" /mac
MTL_PROJ=/nologo /D "NDEBUG" /mac 
CPP=cl.exe
# ADD BASE CPP /nologo /AL /Q68s /W3 /GX /O2 /D "_WINDOWS" /D "_MAC" /D "_68K_" /D "WIN32" /D "NDEBUG" /YX /c
# ADD CPP /nologo /AL /Q68s /W3 /GX /O2 /D "_WINDOWS" /D "_MAC" /D "_68K_" /D "WIN32" /D "NDEBUG" /YX /c
CPP_PROJ=/nologo /AL /Q68s /W3 /GX /O2 /D "_WINDOWS" /D "_MAC" /D "_68K_" /D\
 "WIN32" /D "NDEBUG" /Fp"$(INTDIR)/crsrdemo.pch" /YX /Fo"$(INTDIR)/" /c 
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
RSC_PROJ=/l 0x409 /r /m /fo"$(INTDIR)/crsrdemo.rsc" /d "_MAC" /d "_68K_" /d\
 "NDEBUG" 
MRC=mrc.exe
# ADD BASE MRC /D "_68K_" /D "_MAC" /D "NDEBUG" /NOLOGO
# ADD MRC /D "_68K_" /D "_MAC" /D "NDEBUG" /NOLOGO
MRC_PROJ=/D "_68K_" /D "_MAC" /D "NDEBUG" /l 0x409 /NOLOGO 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/crsrdemo.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 wlm.lib /nologo /MAC:bundle /machine:M68K
# ADD LINK32 wlm.lib /nologo /MAC:bundle /MAC:creator="CRSR" /machine:M68K
LINK32_FLAGS=wlm.lib /nologo /MAC:bundle /MAC:type="APPL" /MAC:creator="CRSR"\
 /pdb:"$(OUTDIR)/crsrdemo.pdb" /machine:M68K /out:"$(OUTDIR)/crsrdemo.exe" 
LINK32_OBJS= \
	"$(INTDIR)/headers.obj" \
	"$(INTDIR)/frame.obj" \
	"$(INTDIR)/dialogs.obj" \
	"$(INTDIR)/child.obj" \
	"$(INTDIR)/crsrdemo.obj" \
	"$(INTDIR)/crsrdemo.rsc" \
	"$(INTDIR)/crsrmac.rsc"

"$(OUTDIR)\crsrdemo.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

MFILE32=mfile.exe
# ADD BASE MFILE32 COPY /NOLOGO
# ADD MFILE32 COPY /NOLOGO
MFILE32_FLAGS=COPY /NOLOGO 
MFILE32_FILES= \
	"$(OUTDIR)/crsrdemo.exe"

"$(OUTDIR)\crsrdemo.trg" : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\MacRel\crsrdemo.exe\
 "$(MFILE32_DEST):crsrdemo.exe">"$(OUTDIR)\crsrdemo.trg"

DOWNLOAD : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\MacRel\crsrdemo.exe\
 "$(MFILE32_DEST):crsrdemo.exe">"$(OUTDIR)\crsrdemo.trg"

!ELSEIF  "$(CFG)" == "crsrdemo - Macintosh Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir MacDbg
# PROP BASE Intermediate_Dir MacDbg
# PROP BASE Target_Dir
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir MacDbg
# PROP Intermediate_Dir MacDbg
# PROP Target_Dir
OUTDIR=.\MacDbg
INTDIR=.\MacDbg

ALL : "$(OUTDIR)\crsrdemo.exe" "$(OUTDIR)\crsrdemo.trg"

CLEAN : 
	-@erase ".\MacDbg\vc40.pdb"
	-@erase ".\MacDbg\crsrdemo.pdb"
	-@erase ".\MacDbg\headers.obj"
	-@erase ".\MacDbg\frame.obj"
	-@erase ".\MacDbg\dialogs.obj"
	-@erase ".\MacDbg\crsrdemo.obj"
	-@erase ".\MacDbg\child.obj"
	-@erase ".\MacDbg\crsrmac.rsc"
	-@erase ".\MacDbg\crsrdemo.rsc"
	-@erase ".\MacDbg\crsrdemo.trg"
	-@erase ".\MacDbg\crsrdemo.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

MTL=mktyplib.exe
# ADD BASE MTL /nologo /D "_DEBUG" /mac
# ADD MTL /nologo /D "_DEBUG" /mac
MTL_PROJ=/nologo /D "_DEBUG" /mac 
CPP=cl.exe
# ADD BASE CPP /nologo /AL /Q68s /Q68m /W3 /GX /Zi /Od /D "_WINDOWS" /D "_MAC" /D "_68K_" /D "WIN32" /D "_DEBUG" /YX /c
# ADD CPP /nologo /AL /Q68s /Q68m /W3 /GX /Zi /Od /D "_WINDOWS" /D "_MAC" /D "_68K_" /D "WIN32" /D "_DEBUG" /YX /c
CPP_PROJ=/nologo /AL /Q68s /Q68m /W3 /GX /Zi /Od /D "_WINDOWS" /D "_MAC" /D\
 "_68K_" /D "WIN32" /D "_DEBUG" /Fp"$(INTDIR)/crsrdemo.pch" /YX /Fo"$(INTDIR)/"\
 /Fd"$(INTDIR)/" /c 
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
RSC_PROJ=/l 0x409 /r /m /fo"$(INTDIR)/crsrdemo.rsc" /d "_MAC" /d "_68K_" /d\
 "_DEBUG" 
MRC=mrc.exe
# ADD BASE MRC /D "_68K_" /D "_MAC" /D "_DEBUG" /NOLOGO
# ADD MRC /D "_68K_" /D "_MAC" /D "_DEBUG" /NOLOGO
MRC_PROJ=/D "_68K_" /D "_MAC" /D "_DEBUG" /l 0x409 /NOLOGO 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/crsrdemo.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 wlm.lib /nologo /MAC:bundle /debug /machine:M68K
# ADD LINK32 wlm.lib /nologo /MAC:bundle /MAC:creator="CRSR" /debug /machine:M68K
LINK32_FLAGS=wlm.lib /nologo /MAC:bundle /MAC:type="APPL" /MAC:creator="CRSR"\
 /pdb:"$(OUTDIR)/crsrdemo.pdb" /debug /machine:M68K\
 /out:"$(OUTDIR)/crsrdemo.exe" 
LINK32_OBJS= \
	"$(INTDIR)/headers.obj" \
	"$(INTDIR)/frame.obj" \
	"$(INTDIR)/dialogs.obj" \
	"$(INTDIR)/crsrdemo.obj" \
	"$(INTDIR)/child.obj" \
	"$(INTDIR)/crsrmac.rsc" \
	"$(INTDIR)/crsrdemo.rsc"

"$(OUTDIR)\crsrdemo.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

MFILE32=mfile.exe
# ADD BASE MFILE32 COPY /NOLOGO
# ADD MFILE32 COPY /NOLOGO
MFILE32_FLAGS=COPY /NOLOGO 
MFILE32_FILES= \
	"$(OUTDIR)/crsrdemo.exe"

"$(OUTDIR)\crsrdemo.trg" : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\MacDbg\crsrdemo.exe\
 "$(MFILE32_DEST):crsrdemo.exe">"$(OUTDIR)\crsrdemo.trg"

DOWNLOAD : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\MacDbg\crsrdemo.exe\
 "$(MFILE32_DEST):crsrdemo.exe">"$(OUTDIR)\crsrdemo.trg"

!ELSEIF  "$(CFG)" == "crsrdemo - Power Macintosh Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir PMacRel
# PROP BASE Intermediate_Dir PMacRel
# PROP BASE Target_Dir
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir PMacRel
# PROP Intermediate_Dir PMacRel
# PROP Target_Dir
OUTDIR=.\PMacRel
INTDIR=.\PMacRel

ALL : "$(OUTDIR)\crsrdemo.exe" "$(OUTDIR)\crsrdemo.trg"

CLEAN : 
	-@erase ".\PMacRel\crsrdemo.trg"
	-@erase ".\PMacRel\crsrdemo.exe"
	-@erase ".\PMacRel\headers.obj"
	-@erase ".\PMacRel\frame.obj"
	-@erase ".\PMacRel\dialogs.obj"
	-@erase ".\PMacRel\crsrdemo.obj"
	-@erase ".\PMacRel\child.obj"
	-@erase ".\PMacRel\crsrmac.rsc"
	-@erase ".\PMacRel\crsrdemo.rsc"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

MTL=mktyplib.exe
# ADD BASE MTL /nologo /D "NDEBUG"
# ADD MTL /nologo /D "NDEBUG"
MTL_PROJ=/nologo /D "NDEBUG" 
CPP=cl.exe
# ADD BASE CPP /nologo /W3 /GX /O2 /D "_MAC" /D "_MPPC_" /D "_WINDOWS" /D "WIN32" /D "NDEBUG" /YX /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "_MAC" /D "_MPPC_" /D "_WINDOWS" /D "WIN32" /D "NDEBUG" /D "_WLMDLL" /YX /c
CPP_PROJ=/nologo /MD /W3 /GX /O2 /D "_MAC" /D "_MPPC_" /D "_WINDOWS" /D "WIN32"\
 /D "NDEBUG" /D "_WLMDLL" /Fp"$(INTDIR)/crsrdemo.pch" /YX /Fo"$(INTDIR)/" /c 
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
RSC_PROJ=/l 0x409 /r /m /fo"$(INTDIR)/crsrdemo.rsc" /d "_MAC" /d "_MPPC_" /d\
 "NDEBUG" 
MRC=mrc.exe
# ADD BASE MRC /D "_MPPC_" /D "_MAC" /D "NDEBUG" /NOLOGO
# ADD MRC /D "_MPPC_" /D "_MAC" /D "NDEBUG" /NOLOGO
MRC_PROJ=/D "_MPPC_" /D "_MAC" /D "NDEBUG" /l 0x409 /NOLOGO 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/crsrdemo.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 cfrg.rsc /nologo /mac:bundle /machine:MPPC
# ADD LINK32 /nologo /mac:bundle /mac:creator="CRSR" /machine:MPPC
LINK32_FLAGS=/nologo /mac:bundle /mac:type="APPL" /mac:creator="CRSR"\
 /incremental:no /pdb:"$(OUTDIR)/crsrdemo.pdb" /machine:MPPC\
 /out:"$(OUTDIR)/crsrdemo.exe" 
LINK32_OBJS= \
	"$(INTDIR)/headers.obj" \
	"$(INTDIR)/frame.obj" \
	"$(INTDIR)/dialogs.obj" \
	"$(INTDIR)/crsrdemo.obj" \
	"$(INTDIR)/child.obj" \
	"$(INTDIR)/crsrmac.rsc" \
	"$(INTDIR)/crsrdemo.rsc"

"$(OUTDIR)\crsrdemo.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

MFILE32=mfile.exe
# ADD BASE MFILE32 COPY /NOLOGO
# ADD MFILE32 COPY /NOLOGO
MFILE32_FLAGS=COPY /NOLOGO 
MFILE32_FILES= \
	"$(OUTDIR)/crsrdemo.exe"

"$(OUTDIR)\crsrdemo.trg" : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\PMacRel\crsrdemo.exe\
 "$(MFILE32_DEST):crsrdemo.exe">"$(OUTDIR)\crsrdemo.trg"

DOWNLOAD : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\PMacRel\crsrdemo.exe\
 "$(MFILE32_DEST):crsrdemo.exe">"$(OUTDIR)\crsrdemo.trg"

!ELSEIF  "$(CFG)" == "crsrdemo - Power Macintosh Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir PMacDbg
# PROP BASE Intermediate_Dir PMacDbg
# PROP BASE Target_Dir
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir PMacDbg
# PROP Intermediate_Dir PMacDbg
# PROP Target_Dir
OUTDIR=.\PMacDbg
INTDIR=.\PMacDbg

ALL : "$(OUTDIR)\crsrdemo.exe" "$(OUTDIR)\crsrdemo.trg"

CLEAN : 
	-@erase ".\PMacDbg\vc40.pdb"
	-@erase ".\PMacDbg\crsrdemo.ilk"
	-@erase ".\PMacDbg\child.obj"
	-@erase ".\PMacDbg\headers.obj"
	-@erase ".\PMacDbg\frame.obj"
	-@erase ".\PMacDbg\crsrdemo.obj"
	-@erase ".\PMacDbg\dialogs.obj"
	-@erase ".\PMacDbg\crsrmac.rsc"
	-@erase ".\PMacDbg\crsrdemo.rsc"
	-@erase ".\PMacDbg\crsrdemo.pdb"
	-@erase ".\PMacDbg\crsrdemo.trg"
	-@erase ".\PMacDbg\crsrdemo.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

MTL=mktyplib.exe
# ADD BASE MTL /nologo /D "_DEBUG"
# ADD MTL /nologo /D "_DEBUG"
MTL_PROJ=/nologo /D "_DEBUG" 
CPP=cl.exe
# ADD BASE CPP /nologo /W3 /GX /Zi /Od /D "_MAC" /D "_MPPC_" /D "_WINDOWS" /D "WIN32" /D "_DEBUG" /YX /c
# ADD CPP /nologo /MDd /W3 /GX /Zi /Od /D "_MAC" /D "_MPPC_" /D "_WINDOWS" /D "WIN32" /D "_DEBUG" /D "_WLMDLL" /YX /c
CPP_PROJ=/nologo /MDd /W3 /GX /Zi /Od /D "_MAC" /D "_MPPC_" /D "_WINDOWS" /D\
 "WIN32" /D "_DEBUG" /D "_WLMDLL" /Fp"$(INTDIR)/crsrdemo.pch" /YX\
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
RSC_PROJ=/l 0x409 /r /m /fo"$(INTDIR)/crsrdemo.rsc" /d "_MAC" /d "_MPPC_" /d\
 "_DEBUG" 
MRC=mrc.exe
# ADD BASE MRC /D "_MPPC_" /D "_MAC" /D "_DEBUG" /NOLOGO
# ADD MRC /D "_MPPC_" /D "_MAC" /D "_DEBUG" /NOLOGO
MRC_PROJ=/D "_MPPC_" /D "_MAC" /D "_DEBUG" /l 0x409 /NOLOGO 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/crsrdemo.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 cfrg.rsc /nologo /mac:bundle /debug /machine:MPPC
# ADD LINK32 /nologo /mac:bundle /mac:creator="CRSR" /debug /machine:MPPC
LINK32_FLAGS=/nologo /mac:bundle /mac:type="APPL" /mac:creator="CRSR"\
 /incremental:yes /pdb:"$(OUTDIR)/crsrdemo.pdb" /debug /machine:MPPC\
 /out:"$(OUTDIR)/crsrdemo.exe" 
LINK32_OBJS= \
	"$(INTDIR)/child.obj" \
	"$(INTDIR)/headers.obj" \
	"$(INTDIR)/frame.obj" \
	"$(INTDIR)/crsrdemo.obj" \
	"$(INTDIR)/dialogs.obj" \
	"$(INTDIR)/crsrmac.rsc" \
	"$(INTDIR)/crsrdemo.rsc"

"$(OUTDIR)\crsrdemo.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

MFILE32=mfile.exe
# ADD BASE MFILE32 COPY /NOLOGO
# ADD MFILE32 COPY /NOLOGO
MFILE32_FLAGS=COPY /NOLOGO 
MFILE32_FILES= \
	"$(OUTDIR)/crsrdemo.exe"

"$(OUTDIR)\crsrdemo.trg" : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\PMacDbg\crsrdemo.exe\
 "$(MFILE32_DEST):crsrdemo.exe">"$(OUTDIR)\crsrdemo.trg"

DOWNLOAD : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\PMacDbg\crsrdemo.exe\
 "$(MFILE32_DEST):crsrdemo.exe">"$(OUTDIR)\crsrdemo.trg"

!ENDIF 

################################################################################
# Begin Target

# Name "crsrdemo - Win32 Release"
# Name "crsrdemo - Win32 Debug"
# Name "crsrdemo - Macintosh Release"
# Name "crsrdemo - Macintosh Debug"
# Name "crsrdemo - Power Macintosh Release"
# Name "crsrdemo - Power Macintosh Debug"

!IF  "$(CFG)" == "crsrdemo - Win32 Release"

!ELSEIF  "$(CFG)" == "crsrdemo - Win32 Debug"

!ELSEIF  "$(CFG)" == "crsrdemo - Macintosh Release"

!ELSEIF  "$(CFG)" == "crsrdemo - Macintosh Debug"

!ELSEIF  "$(CFG)" == "crsrdemo - Power Macintosh Release"

!ELSEIF  "$(CFG)" == "crsrdemo - Power Macintosh Debug"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\child.c
DEP_CPP_CHILD=\
	".\crsrdemo.h"\
	".\headers.h"\
	

!IF  "$(CFG)" == "crsrdemo - Win32 Release"


"$(INTDIR)\child.obj" : $(SOURCE) $(DEP_CPP_CHILD) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "crsrdemo - Win32 Debug"


"$(INTDIR)\child.obj" : $(SOURCE) $(DEP_CPP_CHILD) "$(INTDIR)"\
 "$(INTDIR)\crsrdemo.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_CHILD) "$(INTDIR)"\
 "$(INTDIR)\crsrdemo.pch"


!ELSEIF  "$(CFG)" == "crsrdemo - Macintosh Release"


"$(INTDIR)\child.obj" : $(SOURCE) $(DEP_CPP_CHILD) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "crsrdemo - Macintosh Debug"


"$(INTDIR)\child.obj" : $(SOURCE) $(DEP_CPP_CHILD) "$(INTDIR)"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_CHILD) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "crsrdemo - Power Macintosh Release"


"$(INTDIR)\child.obj" : $(SOURCE) $(DEP_CPP_CHILD) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "crsrdemo - Power Macintosh Debug"


"$(INTDIR)\child.obj" : $(SOURCE) $(DEP_CPP_CHILD) "$(INTDIR)"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_CHILD) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\crsrdemo.c
DEP_CPP_CRSRD=\
	".\crsrdemo.h"\
	".\headers.h"\
	

!IF  "$(CFG)" == "crsrdemo - Win32 Release"


"$(INTDIR)\crsrdemo.obj" : $(SOURCE) $(DEP_CPP_CRSRD) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "crsrdemo - Win32 Debug"


"$(INTDIR)\crsrdemo.obj" : $(SOURCE) $(DEP_CPP_CRSRD) "$(INTDIR)"\
 "$(INTDIR)\crsrdemo.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_CRSRD) "$(INTDIR)"\
 "$(INTDIR)\crsrdemo.pch"


!ELSEIF  "$(CFG)" == "crsrdemo - Macintosh Release"


"$(INTDIR)\crsrdemo.obj" : $(SOURCE) $(DEP_CPP_CRSRD) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "crsrdemo - Macintosh Debug"


"$(INTDIR)\crsrdemo.obj" : $(SOURCE) $(DEP_CPP_CRSRD) "$(INTDIR)"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_CRSRD) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "crsrdemo - Power Macintosh Release"


"$(INTDIR)\crsrdemo.obj" : $(SOURCE) $(DEP_CPP_CRSRD) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "crsrdemo - Power Macintosh Debug"


"$(INTDIR)\crsrdemo.obj" : $(SOURCE) $(DEP_CPP_CRSRD) "$(INTDIR)"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_CRSRD) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\dialogs.c
DEP_CPP_DIALO=\
	".\crsrdemo.h"\
	".\headers.h"\
	

!IF  "$(CFG)" == "crsrdemo - Win32 Release"


"$(INTDIR)\dialogs.obj" : $(SOURCE) $(DEP_CPP_DIALO) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "crsrdemo - Win32 Debug"


"$(INTDIR)\dialogs.obj" : $(SOURCE) $(DEP_CPP_DIALO) "$(INTDIR)"\
 "$(INTDIR)\crsrdemo.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_DIALO) "$(INTDIR)"\
 "$(INTDIR)\crsrdemo.pch"


!ELSEIF  "$(CFG)" == "crsrdemo - Macintosh Release"


"$(INTDIR)\dialogs.obj" : $(SOURCE) $(DEP_CPP_DIALO) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "crsrdemo - Macintosh Debug"


"$(INTDIR)\dialogs.obj" : $(SOURCE) $(DEP_CPP_DIALO) "$(INTDIR)"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_DIALO) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "crsrdemo - Power Macintosh Release"


"$(INTDIR)\dialogs.obj" : $(SOURCE) $(DEP_CPP_DIALO) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "crsrdemo - Power Macintosh Debug"


"$(INTDIR)\dialogs.obj" : $(SOURCE) $(DEP_CPP_DIALO) "$(INTDIR)"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_DIALO) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\frame.c
DEP_CPP_FRAME=\
	".\crsrdemo.h"\
	".\headers.h"\
	

!IF  "$(CFG)" == "crsrdemo - Win32 Release"


"$(INTDIR)\frame.obj" : $(SOURCE) $(DEP_CPP_FRAME) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "crsrdemo - Win32 Debug"


"$(INTDIR)\frame.obj" : $(SOURCE) $(DEP_CPP_FRAME) "$(INTDIR)"\
 "$(INTDIR)\crsrdemo.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_FRAME) "$(INTDIR)"\
 "$(INTDIR)\crsrdemo.pch"


!ELSEIF  "$(CFG)" == "crsrdemo - Macintosh Release"


"$(INTDIR)\frame.obj" : $(SOURCE) $(DEP_CPP_FRAME) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "crsrdemo - Macintosh Debug"


"$(INTDIR)\frame.obj" : $(SOURCE) $(DEP_CPP_FRAME) "$(INTDIR)"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_FRAME) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "crsrdemo - Power Macintosh Release"


"$(INTDIR)\frame.obj" : $(SOURCE) $(DEP_CPP_FRAME) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "crsrdemo - Power Macintosh Debug"


"$(INTDIR)\frame.obj" : $(SOURCE) $(DEP_CPP_FRAME) "$(INTDIR)"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_FRAME) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\headers.c
DEP_CPP_HEADE=\
	".\headers.h"\
	

!IF  "$(CFG)" == "crsrdemo - Win32 Release"


"$(INTDIR)\headers.obj" : $(SOURCE) $(DEP_CPP_HEADE) "$(INTDIR)"
   $(CPP) /nologo /ML /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS"\
 /Fp"$(INTDIR)/crsrdemo.pch" /YX /Fo"$(INTDIR)/" /c $(SOURCE)


!ELSEIF  "$(CFG)" == "crsrdemo - Win32 Debug"

# ADD CPP /Yc"headers.h"

BuildCmds= \
	$(CPP) /nologo /MLd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /Fp"$(INTDIR)/crsrdemo.pch" /Yc"headers.h" /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c\
 $(SOURCE) \
	

"$(INTDIR)\headers.obj" : $(SOURCE) $(DEP_CPP_HEADE) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_HEADE) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\crsrdemo.pch" : $(SOURCE) $(DEP_CPP_HEADE) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "crsrdemo - Macintosh Release"


"$(INTDIR)\headers.obj" : $(SOURCE) $(DEP_CPP_HEADE) "$(INTDIR)"
   $(CPP) /nologo /AL /Q68s /W3 /GX /O2 /D "_WINDOWS" /D "_MAC" /D "_68K_" /D\
 "WIN32" /D "NDEBUG" /Fp"$(INTDIR)/crsrdemo.pch" /YX /Fo"$(INTDIR)/" /c\
 $(SOURCE)


!ELSEIF  "$(CFG)" == "crsrdemo - Macintosh Debug"


BuildCmds= \
	$(CPP) /nologo /AL /Q68s /Q68m /W3 /GX /Zi /Od /D "_WINDOWS" /D "_MAC" /D\
 "_68K_" /D "WIN32" /D "_DEBUG" /Fp"$(INTDIR)/crsrdemo.pch" /YX /Fo"$(INTDIR)/"\
 /Fd"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\headers.obj" : $(SOURCE) $(DEP_CPP_HEADE) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_HEADE) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "crsrdemo - Power Macintosh Release"


"$(INTDIR)\headers.obj" : $(SOURCE) $(DEP_CPP_HEADE) "$(INTDIR)"
   $(CPP) /nologo /MD /W3 /GX /O2 /D "_MAC" /D "_MPPC_" /D "_WINDOWS" /D\
 "WIN32" /D "NDEBUG" /D "_WLMDLL" /Fp"$(INTDIR)/crsrdemo.pch" /YX\
 /Fo"$(INTDIR)/" /c $(SOURCE)


!ELSEIF  "$(CFG)" == "crsrdemo - Power Macintosh Debug"


BuildCmds= \
	$(CPP) /nologo /MDd /W3 /GX /Zi /Od /D "_MAC" /D "_MPPC_" /D "_WINDOWS" /D\
 "WIN32" /D "_DEBUG" /D "_WLMDLL" /Fp"$(INTDIR)/crsrdemo.pch" /YX\
 /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\headers.obj" : $(SOURCE) $(DEP_CPP_HEADE) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_HEADE) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\crsrdemo.rc
DEP_RSC_CRSRDE=\
	".\crsrdemo.ico"\
	".\child.ico"\
	

!IF  "$(CFG)" == "crsrdemo - Win32 Release"


"$(INTDIR)\crsrdemo.res" : $(SOURCE) $(DEP_RSC_CRSRDE) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "crsrdemo - Win32 Debug"


"$(INTDIR)\crsrdemo.res" : $(SOURCE) $(DEP_RSC_CRSRDE) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "crsrdemo - Macintosh Release"


"$(INTDIR)\crsrdemo.rsc" : $(SOURCE) $(DEP_RSC_CRSRDE) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "crsrdemo - Macintosh Debug"


"$(INTDIR)\crsrdemo.rsc" : $(SOURCE) $(DEP_RSC_CRSRDE) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "crsrdemo - Power Macintosh Release"


"$(INTDIR)\crsrdemo.rsc" : $(SOURCE) $(DEP_RSC_CRSRDE) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "crsrdemo - Power Macintosh Debug"


"$(INTDIR)\crsrdemo.rsc" : $(SOURCE) $(DEP_RSC_CRSRDE) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\crsrmac.r

!IF  "$(CFG)" == "crsrdemo - Win32 Release"

!ELSEIF  "$(CFG)" == "crsrdemo - Win32 Debug"

!ELSEIF  "$(CFG)" == "crsrdemo - Macintosh Release"


"$(INTDIR)\crsrmac.rsc" : $(SOURCE) "$(INTDIR)"
   $(MRC) /o"$(INTDIR)/crsrmac.rsc" /D "_68K_" /D "_MAC" /D "NDEBUG" /l 0x409\
 /NOLOGO $(SOURCE)


!ELSEIF  "$(CFG)" == "crsrdemo - Macintosh Debug"


"$(INTDIR)\crsrmac.rsc" : $(SOURCE) "$(INTDIR)"
   $(MRC) /o"$(INTDIR)/crsrmac.rsc" /D "_68K_" /D "_MAC" /D "_DEBUG" /l 0x409\
 /NOLOGO $(SOURCE)


!ELSEIF  "$(CFG)" == "crsrdemo - Power Macintosh Release"


"$(INTDIR)\crsrmac.rsc" : $(SOURCE) "$(INTDIR)"
   $(MRC) /o"$(INTDIR)/crsrmac.rsc" /D "_MPPC_" /D "_MAC" /D "NDEBUG" /l 0x409\
 /NOLOGO $(SOURCE)


!ELSEIF  "$(CFG)" == "crsrdemo - Power Macintosh Debug"


"$(INTDIR)\crsrmac.rsc" : $(SOURCE) "$(INTDIR)"
   $(MRC) /o"$(INTDIR)/crsrmac.rsc" /D "_MPPC_" /D "_MAC" /D "_DEBUG" /l 0x409\
 /NOLOGO $(SOURCE)


!ENDIF 

# End Source File
# End Target
# End Project
################################################################################
