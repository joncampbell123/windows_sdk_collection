# Microsoft Visual C++ Generated NMAKE File, Format Version 40000
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101
# TARGTYPE "Power Macintosh Application" 0x0401
# TARGTYPE "Macintosh Application" 0x0301

!IF "$(CFG)" == ""
CFG=qurydemo - Win32 Debug
!MESSAGE No configuration specified.  Defaulting to qurydemo - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "qurydemo - Win32 Release" && "$(CFG)" !=\
 "qurydemo - Win32 Debug" && "$(CFG)" != "qurydemo - Macintosh Release" &&\
 "$(CFG)" != "qurydemo - Macintosh Debug" && "$(CFG)" !=\
 "qurydemo - Power Macintosh Release" && "$(CFG)" !=\
 "qurydemo - Power Macintosh Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "qurydemo.mak" CFG="qurydemo - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "qurydemo - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "qurydemo - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE "qurydemo - Macintosh Release" (based on "Macintosh Application")
!MESSAGE "qurydemo - Macintosh Debug" (based on "Macintosh Application")
!MESSAGE "qurydemo - Power Macintosh Release" (based on\
 "Power Macintosh Application")
!MESSAGE "qurydemo - Power Macintosh Debug" (based on\
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
# PROP Target_Last_Scanned "qurydemo - Power Macintosh Debug"

!IF  "$(CFG)" == "qurydemo - Win32 Release"

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

ALL : "$(OUTDIR)\qurydemo.exe"

CLEAN : 
	-@erase ".\Release\qurydemo.exe"
	-@erase ".\Release\headers.obj"
	-@erase ".\Release\main.obj"
	-@erase ".\Release\qurydemo.pch"
	-@erase ".\Release\query.obj"
	-@erase ".\Release\qurydemo.res"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /Yu"qurydemo.h" /c
# SUBTRACT CPP /nologo
CPP_PROJ=/ML /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS"\
 /Fp"$(INTDIR)/qurydemo.pch" /Yu"qurydemo.h" /Fo"$(INTDIR)/" /c 
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
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/qurydemo.res" /d "NDEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/qurydemo.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ctl3d32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ctl3d32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib /nologo /subsystem:windows /incremental:no\
 /pdb:"$(OUTDIR)/qurydemo.pdb" /machine:I386 /out:"$(OUTDIR)/qurydemo.exe" 
LINK32_OBJS= \
	"$(INTDIR)/headers.obj" \
	"$(INTDIR)/main.obj" \
	"$(INTDIR)/query.obj" \
	"$(INTDIR)/qurydemo.res"

"$(OUTDIR)\qurydemo.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "qurydemo - Win32 Debug"

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

ALL : "$(OUTDIR)\qurydemo.exe"

CLEAN : 
	-@erase ".\Debug\vc40.pdb"
	-@erase ".\Debug\qurydemo.pch"
	-@erase ".\Debug\qurydemo.exe"
	-@erase ".\Debug\query.obj"
	-@erase ".\Debug\main.obj"
	-@erase ".\Debug\headers.obj"
	-@erase ".\Debug\qurydemo.res"
	-@erase ".\Debug\qurydemo.ilk"
	-@erase ".\Debug\qurydemo.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /Yu"qurydemo.h" /c
# SUBTRACT CPP /nologo
CPP_PROJ=/MLd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /Fp"$(INTDIR)/qurydemo.pch" /Yu"qurydemo.h" /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
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
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/qurydemo.res" /d "_DEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/qurydemo.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ctl3d32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ctl3d32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib /nologo /subsystem:windows /incremental:yes\
 /pdb:"$(OUTDIR)/qurydemo.pdb" /debug /machine:I386\
 /out:"$(OUTDIR)/qurydemo.exe" 
LINK32_OBJS= \
	"$(INTDIR)/query.obj" \
	"$(INTDIR)/main.obj" \
	"$(INTDIR)/headers.obj" \
	"$(INTDIR)/qurydemo.res"

"$(OUTDIR)\qurydemo.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "qurydemo - Macintosh Release"

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

ALL : "$(OUTDIR)\qurydemo.exe" "$(OUTDIR)\qurydemo.trg"

CLEAN : 
	-@erase ".\MacRel\qurydemo.trg"
	-@erase ".\MacRel\qurydemo.exe"
	-@erase ".\MacRel\query.obj"
	-@erase ".\MacRel\qurydemo.pch"
	-@erase ".\MacRel\main.obj"
	-@erase ".\MacRel\headers.obj"
	-@erase ".\MacRel\qurydemo.rsc"
	-@erase ".\MacRel\qurymac.rsc"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

MTL=mktyplib.exe
# ADD BASE MTL /nologo /D "NDEBUG" /mac
# ADD MTL /nologo /D "NDEBUG" /mac
MTL_PROJ=/nologo /D "NDEBUG" /mac 
CPP=cl.exe
# ADD BASE CPP /nologo /AL /Q68s /W3 /GX /O2 /D "_WINDOWS" /D "_MAC" /D "_68K_" /D "WIN32" /D "NDEBUG" /YX /c
# ADD CPP /AL /Q68s /W3 /GX /O2 /D "_WINDOWS" /D "_MAC" /D "_68K_" /D "WIN32" /D "NDEBUG" /Yu"qurydemo.h" /c
# SUBTRACT CPP /nologo
CPP_PROJ=/AL /Q68s /W3 /GX /O2 /D "_WINDOWS" /D "_MAC" /D "_68K_" /D "WIN32" /D\
 "NDEBUG" /Fp"$(INTDIR)/qurydemo.pch" /Yu"qurydemo.h" /Fo"$(INTDIR)/" /c 
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
RSC_PROJ=/l 0x409 /r /m /fo"$(INTDIR)/qurydemo.rsc" /d "_MAC" /d "_68K_" /d\
 "NDEBUG" 
MRC=mrc.exe
# ADD BASE MRC /D "_68K_" /D "_MAC" /D "NDEBUG" /NOLOGO
# ADD MRC /D "_68K_" /D "_MAC" /D "NDEBUG" /NOLOGO
MRC_PROJ=/D "_68K_" /D "_MAC" /D "NDEBUG" /l 0x409 /NOLOGO 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/qurydemo.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 wlm.lib /nologo /MAC:bundle /machine:M68K
# ADD LINK32 wlm.lib /nologo /MAC:bundle /MAC:creator="QURY" /machine:M68K
LINK32_FLAGS=wlm.lib /nologo /MAC:bundle /MAC:type="APPL" /MAC:creator="QURY"\
 /pdb:"$(OUTDIR)/qurydemo.pdb" /machine:M68K /out:"$(OUTDIR)/qurydemo.exe" 
LINK32_OBJS= \
	"$(INTDIR)/query.obj" \
	"$(INTDIR)/main.obj" \
	"$(INTDIR)/headers.obj" \
	"$(INTDIR)/qurydemo.rsc" \
	"$(INTDIR)/qurymac.rsc"

"$(OUTDIR)\qurydemo.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

MFILE32=mfile.exe
# ADD BASE MFILE32 COPY /NOLOGO
# ADD MFILE32 COPY /NOLOGO
MFILE32_FLAGS=COPY /NOLOGO 
MFILE32_FILES= \
	"$(OUTDIR)/qurydemo.exe"

"$(OUTDIR)\qurydemo.trg" : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\MacRel\qurydemo.exe\
 "$(MFILE32_DEST):qurydemo.exe">"$(OUTDIR)\qurydemo.trg"

DOWNLOAD : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\MacRel\qurydemo.exe\
 "$(MFILE32_DEST):qurydemo.exe">"$(OUTDIR)\qurydemo.trg"

!ELSEIF  "$(CFG)" == "qurydemo - Macintosh Debug"

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

ALL : "$(OUTDIR)\qurydemo.exe" "$(OUTDIR)\qurydemo.trg"

CLEAN : 
	-@erase ".\MacDbg\vc40.pdb"
	-@erase ".\MacDbg\qurydemo.pch"
	-@erase ".\MacDbg\qurydemo.pdb"
	-@erase ".\MacDbg\main.obj"
	-@erase ".\MacDbg\query.obj"
	-@erase ".\MacDbg\headers.obj"
	-@erase ".\MacDbg\qurydemo.rsc"
	-@erase ".\MacDbg\qurymac.rsc"
	-@erase ".\MacDbg\qurydemo.trg"
	-@erase ".\MacDbg\qurydemo.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

MTL=mktyplib.exe
# ADD BASE MTL /nologo /D "_DEBUG" /mac
# ADD MTL /nologo /D "_DEBUG" /mac
MTL_PROJ=/nologo /D "_DEBUG" /mac 
CPP=cl.exe
# ADD BASE CPP /nologo /AL /Q68s /Q68m /W3 /GX /Zi /Od /D "_WINDOWS" /D "_MAC" /D "_68K_" /D "WIN32" /D "_DEBUG" /YX /c
# ADD CPP /AL /Q68s /Q68m /W3 /GX /Zi /Od /D "_WINDOWS" /D "_MAC" /D "_68K_" /D "WIN32" /D "_DEBUG" /Yu"qurydemo.h" /c
# SUBTRACT CPP /nologo
CPP_PROJ=/AL /Q68s /Q68m /W3 /GX /Zi /Od /D "_WINDOWS" /D "_MAC" /D "_68K_" /D\
 "WIN32" /D "_DEBUG" /Fp"$(INTDIR)/qurydemo.pch" /Yu"qurydemo.h" /Fo"$(INTDIR)/"\
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
RSC_PROJ=/l 0x409 /r /m /fo"$(INTDIR)/qurydemo.rsc" /d "_MAC" /d "_68K_" /d\
 "_DEBUG" 
MRC=mrc.exe
# ADD BASE MRC /D "_68K_" /D "_MAC" /D "_DEBUG" /NOLOGO
# ADD MRC /D "_68K_" /D "_MAC" /D "_DEBUG" /NOLOGO
MRC_PROJ=/D "_68K_" /D "_MAC" /D "_DEBUG" /l 0x409 /NOLOGO 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/qurydemo.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 wlm.lib /nologo /MAC:bundle /debug /machine:M68K
# ADD LINK32 wlm.lib /nologo /MAC:bundle /MAC:creator="QURY" /debug /machine:M68K
LINK32_FLAGS=wlm.lib /nologo /MAC:bundle /MAC:type="APPL" /MAC:creator="QURY"\
 /pdb:"$(OUTDIR)/qurydemo.pdb" /debug /machine:M68K\
 /out:"$(OUTDIR)/qurydemo.exe" 
LINK32_OBJS= \
	"$(INTDIR)/main.obj" \
	"$(INTDIR)/query.obj" \
	"$(INTDIR)/headers.obj" \
	"$(INTDIR)/qurydemo.rsc" \
	"$(INTDIR)/qurymac.rsc"

"$(OUTDIR)\qurydemo.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

MFILE32=mfile.exe
# ADD BASE MFILE32 COPY /NOLOGO
# ADD MFILE32 COPY /NOLOGO
MFILE32_FLAGS=COPY /NOLOGO 
MFILE32_FILES= \
	"$(OUTDIR)/qurydemo.exe"

"$(OUTDIR)\qurydemo.trg" : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\MacDbg\qurydemo.exe\
 "$(MFILE32_DEST):qurydemo.exe">"$(OUTDIR)\qurydemo.trg"

DOWNLOAD : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\MacDbg\qurydemo.exe\
 "$(MFILE32_DEST):qurydemo.exe">"$(OUTDIR)\qurydemo.trg"

!ELSEIF  "$(CFG)" == "qurydemo - Power Macintosh Release"

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

ALL : "$(OUTDIR)\qurydemo.exe" "$(OUTDIR)\qurydemo.trg"

CLEAN : 
	-@erase ".\PMacRel\qurydemo.trg"
	-@erase ".\PMacRel\qurydemo.exe"
	-@erase ".\PMacRel\headers.obj"
	-@erase ".\PMacRel\main.obj"
	-@erase ".\PMacRel\qurydemo.pch"
	-@erase ".\PMacRel\query.obj"
	-@erase ".\PMacRel\qurydemo.rsc"
	-@erase ".\PMacRel\qurymac.rsc"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

MTL=mktyplib.exe
# ADD BASE MTL /nologo /D "NDEBUG"
# ADD MTL /nologo /D "NDEBUG"
MTL_PROJ=/nologo /D "NDEBUG" 
CPP=cl.exe
# ADD BASE CPP /nologo /W3 /GX /O2 /D "_MAC" /D "_MPPC_" /D "_WINDOWS" /D "WIN32" /D "NDEBUG" /YX /c
# ADD CPP /MD /W3 /GX /O2 /D "_MAC" /D "_MPPC_" /D "_WINDOWS" /D "WIN32" /D "NDEBUG" /D "_WLMDLL" /Yu"qurydemo.h" /c
# SUBTRACT CPP /nologo
CPP_PROJ=/MD /W3 /GX /O2 /D "_MAC" /D "_MPPC_" /D "_WINDOWS" /D "WIN32" /D\
 "NDEBUG" /D "_WLMDLL" /Fp"$(INTDIR)/qurydemo.pch" /Yu"qurydemo.h"\
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
RSC_PROJ=/l 0x409 /r /m /fo"$(INTDIR)/qurydemo.rsc" /d "_MAC" /d "_MPPC_" /d\
 "NDEBUG" 
MRC=mrc.exe
# ADD BASE MRC /D "_MPPC_" /D "_MAC" /D "NDEBUG" /NOLOGO
# ADD MRC /D "_MPPC_" /D "_MAC" /D "NDEBUG" /NOLOGO
MRC_PROJ=/D "_MPPC_" /D "_MAC" /D "NDEBUG" /l 0x409 /NOLOGO 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/qurydemo.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 cfrg.rsc /nologo /mac:bundle /machine:MPPC
# ADD LINK32 /nologo /mac:bundle /mac:creator="QURY" /machine:MPPC
LINK32_FLAGS=/nologo /mac:bundle /mac:type="APPL" /mac:creator="QURY"\
 /incremental:no /pdb:"$(OUTDIR)/qurydemo.pdb" /machine:MPPC\
 /out:"$(OUTDIR)/qurydemo.exe" 
LINK32_OBJS= \
	"$(INTDIR)/headers.obj" \
	"$(INTDIR)/main.obj" \
	"$(INTDIR)/query.obj" \
	"$(INTDIR)/qurydemo.rsc" \
	"$(INTDIR)/qurymac.rsc"

"$(OUTDIR)\qurydemo.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

MFILE32=mfile.exe
# ADD BASE MFILE32 COPY /NOLOGO
# ADD MFILE32 COPY /NOLOGO
MFILE32_FLAGS=COPY /NOLOGO 
MFILE32_FILES= \
	"$(OUTDIR)/qurydemo.exe"

"$(OUTDIR)\qurydemo.trg" : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\PMacRel\qurydemo.exe\
 "$(MFILE32_DEST):qurydemo.exe">"$(OUTDIR)\qurydemo.trg"

DOWNLOAD : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\PMacRel\qurydemo.exe\
 "$(MFILE32_DEST):qurydemo.exe">"$(OUTDIR)\qurydemo.trg"

!ELSEIF  "$(CFG)" == "qurydemo - Power Macintosh Debug"

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

ALL : "$(OUTDIR)\qurydemo.exe" "$(OUTDIR)\qurydemo.trg"

CLEAN : 
	-@erase ".\PMacDbg\vc40.pdb"
	-@erase ".\PMacDbg\qurydemo.pch"
	-@erase ".\PMacDbg\qurydemo.ilk"
	-@erase ".\PMacDbg\headers.obj"
	-@erase ".\PMacDbg\query.obj"
	-@erase ".\PMacDbg\main.obj"
	-@erase ".\PMacDbg\qurymac.rsc"
	-@erase ".\PMacDbg\qurydemo.rsc"
	-@erase ".\PMacDbg\qurydemo.pdb"
	-@erase ".\PMacDbg\qurydemo.trg"
	-@erase ".\PMacDbg\qurydemo.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

MTL=mktyplib.exe
# ADD BASE MTL /nologo /D "_DEBUG"
# ADD MTL /nologo /D "_DEBUG"
MTL_PROJ=/nologo /D "_DEBUG" 
CPP=cl.exe
# ADD BASE CPP /nologo /W3 /GX /Zi /Od /D "_MAC" /D "_MPPC_" /D "_WINDOWS" /D "WIN32" /D "_DEBUG" /YX /c
# ADD CPP /MDd /W3 /GX /Zi /Od /D "_MAC" /D "_MPPC_" /D "_WINDOWS" /D "WIN32" /D "_DEBUG" /D "_WLMDLL" /Yu"qurydemo.h" /c
# SUBTRACT CPP /nologo
CPP_PROJ=/MDd /W3 /GX /Zi /Od /D "_MAC" /D "_MPPC_" /D "_WINDOWS" /D "WIN32" /D\
 "_DEBUG" /D "_WLMDLL" /Fp"$(INTDIR)/qurydemo.pch" /Yu"qurydemo.h"\
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
RSC_PROJ=/l 0x409 /r /m /fo"$(INTDIR)/qurydemo.rsc" /d "_MAC" /d "_MPPC_" /d\
 "_DEBUG" 
MRC=mrc.exe
# ADD BASE MRC /D "_MPPC_" /D "_MAC" /D "_DEBUG" /NOLOGO
# ADD MRC /D "_MPPC_" /D "_MAC" /D "_DEBUG" /NOLOGO
MRC_PROJ=/D "_MPPC_" /D "_MAC" /D "_DEBUG" /l 0x409 /NOLOGO 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/qurydemo.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 cfrg.rsc /nologo /mac:bundle /debug /machine:MPPC
# ADD LINK32 /nologo /mac:bundle /mac:creator="QURY" /debug /machine:MPPC
LINK32_FLAGS=/nologo /mac:bundle /mac:type="APPL" /mac:creator="QURY"\
 /incremental:yes /pdb:"$(OUTDIR)/qurydemo.pdb" /debug /machine:MPPC\
 /out:"$(OUTDIR)/qurydemo.exe" 
LINK32_OBJS= \
	"$(INTDIR)/headers.obj" \
	"$(INTDIR)/query.obj" \
	"$(INTDIR)/main.obj" \
	"$(INTDIR)/qurymac.rsc" \
	"$(INTDIR)/qurydemo.rsc"

"$(OUTDIR)\qurydemo.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

MFILE32=mfile.exe
# ADD BASE MFILE32 COPY /NOLOGO
# ADD MFILE32 COPY /NOLOGO
MFILE32_FLAGS=COPY /NOLOGO 
MFILE32_FILES= \
	"$(OUTDIR)/qurydemo.exe"

"$(OUTDIR)\qurydemo.trg" : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\PMacDbg\qurydemo.exe\
 "$(MFILE32_DEST):qurydemo.exe">"$(OUTDIR)\qurydemo.trg"

DOWNLOAD : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\PMacDbg\qurydemo.exe\
 "$(MFILE32_DEST):qurydemo.exe">"$(OUTDIR)\qurydemo.trg"

!ENDIF 

################################################################################
# Begin Target

# Name "qurydemo - Win32 Release"
# Name "qurydemo - Win32 Debug"
# Name "qurydemo - Macintosh Release"
# Name "qurydemo - Macintosh Debug"
# Name "qurydemo - Power Macintosh Release"
# Name "qurydemo - Power Macintosh Debug"

!IF  "$(CFG)" == "qurydemo - Win32 Release"

!ELSEIF  "$(CFG)" == "qurydemo - Win32 Debug"

!ELSEIF  "$(CFG)" == "qurydemo - Macintosh Release"

!ELSEIF  "$(CFG)" == "qurydemo - Macintosh Debug"

!ELSEIF  "$(CFG)" == "qurydemo - Power Macintosh Release"

!ELSEIF  "$(CFG)" == "qurydemo - Power Macintosh Debug"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\main.c
DEP_CPP_MAIN_=\
	".\qurydemo.h"\
	

!IF  "$(CFG)" == "qurydemo - Win32 Release"


"$(INTDIR)\main.obj" : $(SOURCE) $(DEP_CPP_MAIN_) "$(INTDIR)"\
 "$(INTDIR)\qurydemo.pch"


!ELSEIF  "$(CFG)" == "qurydemo - Win32 Debug"


"$(INTDIR)\main.obj" : $(SOURCE) $(DEP_CPP_MAIN_) "$(INTDIR)"\
 "$(INTDIR)\qurydemo.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_MAIN_) "$(INTDIR)"\
 "$(INTDIR)\qurydemo.pch"


!ELSEIF  "$(CFG)" == "qurydemo - Macintosh Release"


"$(INTDIR)\main.obj" : $(SOURCE) $(DEP_CPP_MAIN_) "$(INTDIR)"\
 "$(INTDIR)\qurydemo.pch"


!ELSEIF  "$(CFG)" == "qurydemo - Macintosh Debug"


"$(INTDIR)\main.obj" : $(SOURCE) $(DEP_CPP_MAIN_) "$(INTDIR)"\
 "$(INTDIR)\qurydemo.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_MAIN_) "$(INTDIR)"\
 "$(INTDIR)\qurydemo.pch"


!ELSEIF  "$(CFG)" == "qurydemo - Power Macintosh Release"


"$(INTDIR)\main.obj" : $(SOURCE) $(DEP_CPP_MAIN_) "$(INTDIR)"\
 "$(INTDIR)\qurydemo.pch"


!ELSEIF  "$(CFG)" == "qurydemo - Power Macintosh Debug"


"$(INTDIR)\main.obj" : $(SOURCE) $(DEP_CPP_MAIN_) "$(INTDIR)"\
 "$(INTDIR)\qurydemo.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_MAIN_) "$(INTDIR)"\
 "$(INTDIR)\qurydemo.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\query.c
DEP_CPP_QUERY=\
	".\qurydemo.h"\
	

!IF  "$(CFG)" == "qurydemo - Win32 Release"


"$(INTDIR)\query.obj" : $(SOURCE) $(DEP_CPP_QUERY) "$(INTDIR)"\
 "$(INTDIR)\qurydemo.pch"


!ELSEIF  "$(CFG)" == "qurydemo - Win32 Debug"


"$(INTDIR)\query.obj" : $(SOURCE) $(DEP_CPP_QUERY) "$(INTDIR)"\
 "$(INTDIR)\qurydemo.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_QUERY) "$(INTDIR)"\
 "$(INTDIR)\qurydemo.pch"


!ELSEIF  "$(CFG)" == "qurydemo - Macintosh Release"


"$(INTDIR)\query.obj" : $(SOURCE) $(DEP_CPP_QUERY) "$(INTDIR)"\
 "$(INTDIR)\qurydemo.pch"


!ELSEIF  "$(CFG)" == "qurydemo - Macintosh Debug"


"$(INTDIR)\query.obj" : $(SOURCE) $(DEP_CPP_QUERY) "$(INTDIR)"\
 "$(INTDIR)\qurydemo.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_QUERY) "$(INTDIR)"\
 "$(INTDIR)\qurydemo.pch"


!ELSEIF  "$(CFG)" == "qurydemo - Power Macintosh Release"


"$(INTDIR)\query.obj" : $(SOURCE) $(DEP_CPP_QUERY) "$(INTDIR)"\
 "$(INTDIR)\qurydemo.pch"


!ELSEIF  "$(CFG)" == "qurydemo - Power Macintosh Debug"


"$(INTDIR)\query.obj" : $(SOURCE) $(DEP_CPP_QUERY) "$(INTDIR)"\
 "$(INTDIR)\qurydemo.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_QUERY) "$(INTDIR)"\
 "$(INTDIR)\qurydemo.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\qurydemo.rc
DEP_RSC_QURYD=\
	".\newquery.bmp"\
	".\runquery.bmp"\
	".\qurydemo.ico"\
	".\querywdw.ico"\
	".\qurydemo.h"\
	".\resource.h"\
	

!IF  "$(CFG)" == "qurydemo - Win32 Release"


"$(INTDIR)\qurydemo.res" : $(SOURCE) $(DEP_RSC_QURYD) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "qurydemo - Win32 Debug"


"$(INTDIR)\qurydemo.res" : $(SOURCE) $(DEP_RSC_QURYD) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "qurydemo - Macintosh Release"


"$(INTDIR)\qurydemo.rsc" : $(SOURCE) $(DEP_RSC_QURYD) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "qurydemo - Macintosh Debug"


"$(INTDIR)\qurydemo.rsc" : $(SOURCE) $(DEP_RSC_QURYD) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "qurydemo - Power Macintosh Release"


"$(INTDIR)\qurydemo.rsc" : $(SOURCE) $(DEP_RSC_QURYD) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "qurydemo - Power Macintosh Debug"


"$(INTDIR)\qurydemo.rsc" : $(SOURCE) $(DEP_RSC_QURYD) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\qurymac.r

!IF  "$(CFG)" == "qurydemo - Win32 Release"

!ELSEIF  "$(CFG)" == "qurydemo - Win32 Debug"

!ELSEIF  "$(CFG)" == "qurydemo - Macintosh Release"

DEP_MRC_QURYM=\
	".\resource.h"\
	

"$(INTDIR)\qurymac.rsc" : $(SOURCE) $(DEP_MRC_QURYM) "$(INTDIR)"
   $(MRC) /o"$(INTDIR)/qurymac.rsc" /D "_68K_" /D "_MAC" /D "NDEBUG" /l 0x409\
 /NOLOGO $(SOURCE)


!ELSEIF  "$(CFG)" == "qurydemo - Macintosh Debug"

DEP_MRC_QURYM=\
	".\resource.h"\
	

"$(INTDIR)\qurymac.rsc" : $(SOURCE) $(DEP_MRC_QURYM) "$(INTDIR)"
   $(MRC) /o"$(INTDIR)/qurymac.rsc" /D "_68K_" /D "_MAC" /D "_DEBUG" /l 0x409\
 /NOLOGO $(SOURCE)


!ELSEIF  "$(CFG)" == "qurydemo - Power Macintosh Release"

DEP_MRC_QURYM=\
	".\resource.h"\
	

"$(INTDIR)\qurymac.rsc" : $(SOURCE) $(DEP_MRC_QURYM) "$(INTDIR)"
   $(MRC) /o"$(INTDIR)/qurymac.rsc" /D "_MPPC_" /D "_MAC" /D "NDEBUG" /l 0x409\
 /NOLOGO $(SOURCE)


!ELSEIF  "$(CFG)" == "qurydemo - Power Macintosh Debug"

DEP_MRC_QURYM=\
	".\resource.h"\
	

"$(INTDIR)\qurymac.rsc" : $(SOURCE) $(DEP_MRC_QURYM) "$(INTDIR)"
   $(MRC) /o"$(INTDIR)/qurymac.rsc" /D "_MPPC_" /D "_MAC" /D "_DEBUG" /l 0x409\
 /NOLOGO $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\headers.c
DEP_CPP_HEADE=\
	".\qurydemo.h"\
	

!IF  "$(CFG)" == "qurydemo - Win32 Release"

# ADD CPP /Yc"qurydemo.h"

BuildCmds= \
	$(CPP) /ML /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS"\
 /Fp"$(INTDIR)/qurydemo.pch" /Yc"qurydemo.h" /Fo"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\headers.obj" : $(SOURCE) $(DEP_CPP_HEADE) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\qurydemo.pch" : $(SOURCE) $(DEP_CPP_HEADE) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "qurydemo - Win32 Debug"

# ADD CPP /Yc"qurydemo.h"

BuildCmds= \
	$(CPP) /MLd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /Fp"$(INTDIR)/qurydemo.pch" /Yc"qurydemo.h" /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c\
 $(SOURCE) \
	

"$(INTDIR)\headers.obj" : $(SOURCE) $(DEP_CPP_HEADE) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_HEADE) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\qurydemo.pch" : $(SOURCE) $(DEP_CPP_HEADE) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "qurydemo - Macintosh Release"

# ADD CPP /Yc

BuildCmds= \
	$(CPP) /AL /Q68s /W3 /GX /O2 /D "_WINDOWS" /D "_MAC" /D "_68K_" /D "WIN32" /D\
 "NDEBUG" /Fp"$(INTDIR)/qurydemo.pch" /Yc /Fo"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\headers.obj" : $(SOURCE) $(DEP_CPP_HEADE) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\qurydemo.pch" : $(SOURCE) $(DEP_CPP_HEADE) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "qurydemo - Macintosh Debug"

# ADD CPP /Yc"qurydemo.h"

BuildCmds= \
	$(CPP) /AL /Q68s /Q68m /W3 /GX /Zi /Od /D "_WINDOWS" /D "_MAC" /D "_68K_" /D\
 "WIN32" /D "_DEBUG" /Fp"$(INTDIR)/qurydemo.pch" /Yc"qurydemo.h" /Fo"$(INTDIR)/"\
 /Fd"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\headers.obj" : $(SOURCE) $(DEP_CPP_HEADE) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_HEADE) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\qurydemo.pch" : $(SOURCE) $(DEP_CPP_HEADE) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "qurydemo - Power Macintosh Release"

# ADD CPP /Yc

BuildCmds= \
	$(CPP) /MD /W3 /GX /O2 /D "_MAC" /D "_MPPC_" /D "_WINDOWS" /D "WIN32" /D\
 "NDEBUG" /D "_WLMDLL" /Fp"$(INTDIR)/qurydemo.pch" /Yc /Fo"$(INTDIR)/" /c\
 $(SOURCE) \
	

"$(INTDIR)\headers.obj" : $(SOURCE) $(DEP_CPP_HEADE) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\qurydemo.pch" : $(SOURCE) $(DEP_CPP_HEADE) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "qurydemo - Power Macintosh Debug"

# ADD CPP /Yc

BuildCmds= \
	$(CPP) /MDd /W3 /GX /Zi /Od /D "_MAC" /D "_MPPC_" /D "_WINDOWS" /D "WIN32" /D\
 "_DEBUG" /D "_WLMDLL" /Fp"$(INTDIR)/qurydemo.pch" /Yc /Fo"$(INTDIR)/"\
 /Fd"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\headers.obj" : $(SOURCE) $(DEP_CPP_HEADE) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_HEADE) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\qurydemo.pch" : $(SOURCE) $(DEP_CPP_HEADE) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
# End Target
# End Project
################################################################################
