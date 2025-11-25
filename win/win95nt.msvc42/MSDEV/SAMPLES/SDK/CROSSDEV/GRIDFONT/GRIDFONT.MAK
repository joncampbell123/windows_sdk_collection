# Microsoft Visual C++ Generated NMAKE File, Format Version 40000
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101
# TARGTYPE "Power Macintosh Application" 0x0401
# TARGTYPE "Macintosh Application" 0x0301

!IF "$(CFG)" == ""
CFG=GridFont - Win32 Debug
!MESSAGE No configuration specified.  Defaulting to GridFont - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "GridFont - Win32 Release" && "$(CFG)" !=\
 "GridFont - Win32 Debug" && "$(CFG)" != "GridFont - Macintosh Release" &&\
 "$(CFG)" != "GridFont - Macintosh Debug" && "$(CFG)" !=\
 "GridFont - Power Macintosh Release" && "$(CFG)" !=\
 "GridFont - Power Macintosh Debug" && "$(CFG)" !=\
 "GridFont - Win32 Unicode Release" && "$(CFG)" !=\
 "GridFont - Win32 Unicode Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "gridfont.mak" CFG="GridFont - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "GridFont - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "GridFont - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE "GridFont - Macintosh Release" (based on "Macintosh Application")
!MESSAGE "GridFont - Macintosh Debug" (based on "Macintosh Application")
!MESSAGE "GridFont - Power Macintosh Release" (based on\
 "Power Macintosh Application")
!MESSAGE "GridFont - Power Macintosh Debug" (based on\
 "Power Macintosh Application")
!MESSAGE "GridFont - Win32 Unicode Release" (based on\
 "Win32 (x86) Application")
!MESSAGE "GridFont - Win32 Unicode Debug" (based on "Win32 (x86) Application")
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
# PROP Target_Last_Scanned "GridFont - Win32 Unicode Debug"

!IF  "$(CFG)" == "GridFont - Win32 Release"

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

ALL : "$(OUTDIR)\gridfont.exe"

CLEAN : 
	-@erase ".\Release\gridfont.exe"
	-@erase ".\Release\font.obj"
	-@erase ".\Release\gridfont.pch"
	-@erase ".\Release\view.obj"
	-@erase ".\Release\box.obj"
	-@erase ".\Release\cset.obj"
	-@erase ".\Release\app.obj"
	-@erase ".\Release\stdwin.obj"
	-@erase ".\Release\grid.obj"
	-@erase ".\Release\gridfont.res"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /Yu"Stdwin.h" /c
CPP_PROJ=/nologo /ML /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS"\
 /Fp"$(INTDIR)/gridfont.pch" /Yu"Stdwin.h" /Fo"$(INTDIR)/" /c 
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
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/gridfont.res" /d "NDEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/gridfont.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib /nologo /subsystem:windows /incremental:no\
 /pdb:"$(OUTDIR)/gridfont.pdb" /machine:I386 /out:"$(OUTDIR)/gridfont.exe" 
LINK32_OBJS= \
	"$(INTDIR)/font.obj" \
	"$(INTDIR)/view.obj" \
	"$(INTDIR)/box.obj" \
	"$(INTDIR)/cset.obj" \
	"$(INTDIR)/app.obj" \
	"$(INTDIR)/stdwin.obj" \
	"$(INTDIR)/grid.obj" \
	"$(INTDIR)/gridfont.res"

"$(OUTDIR)\gridfont.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "GridFont - Win32 Debug"

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

ALL : "$(OUTDIR)\gridfont.exe"

CLEAN : 
	-@erase ".\Debug\vc40.pdb"
	-@erase ".\Debug\gridfont.pch"
	-@erase ".\Debug\gridfont.exe"
	-@erase ".\Debug\cset.obj"
	-@erase ".\Debug\box.obj"
	-@erase ".\Debug\grid.obj"
	-@erase ".\Debug\font.obj"
	-@erase ".\Debug\app.obj"
	-@erase ".\Debug\view.obj"
	-@erase ".\Debug\stdwin.obj"
	-@erase ".\Debug\gridfont.res"
	-@erase ".\Debug\gridfont.ilk"
	-@erase ".\Debug\gridfont.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /Yu"Stdwin.h" /c
CPP_PROJ=/nologo /MLd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /Fp"$(INTDIR)/gridfont.pch" /Yu"Stdwin.h" /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
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
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/gridfont.res" /d "_DEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/gridfont.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib /nologo /subsystem:windows /incremental:yes\
 /pdb:"$(OUTDIR)/gridfont.pdb" /debug /machine:I386\
 /out:"$(OUTDIR)/gridfont.exe" 
LINK32_OBJS= \
	"$(INTDIR)/cset.obj" \
	"$(INTDIR)/box.obj" \
	"$(INTDIR)/grid.obj" \
	"$(INTDIR)/font.obj" \
	"$(INTDIR)/app.obj" \
	"$(INTDIR)/view.obj" \
	"$(INTDIR)/stdwin.obj" \
	"$(INTDIR)/gridfont.res"

"$(OUTDIR)\gridfont.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "GridFont - Macintosh Release"

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

ALL : "$(OUTDIR)\gridfont.exe" "$(OUTDIR)\gridfont.trg"

CLEAN : 
	-@erase ".\MacRel\gridfont.trg"
	-@erase ".\MacRel\gridfont.exe"
	-@erase ".\MacRel\view.obj"
	-@erase ".\MacRel\gridfont.pch"
	-@erase ".\MacRel\app.obj"
	-@erase ".\MacRel\cset.obj"
	-@erase ".\MacRel\stdwin.obj"
	-@erase ".\MacRel\grid.obj"
	-@erase ".\MacRel\font.obj"
	-@erase ".\MacRel\box.obj"
	-@erase ".\MacRel\gridfont.rsc"
	-@erase ".\MacRel\grid.rsc"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

MTL=mktyplib.exe
# ADD BASE MTL /nologo /D "NDEBUG" /mac
# ADD MTL /nologo /D "NDEBUG" /mac
MTL_PROJ=/nologo /D "NDEBUG" /mac 
CPP=cl.exe
# ADD BASE CPP /nologo /AL /Q68s /W3 /GX /O2 /D "_WINDOWS" /D "_MAC" /D "_68K_" /D "WIN32" /D "NDEBUG" /YX /c
# ADD CPP /nologo /AL /Q68s /W3 /GX /O2 /D "_WINDOWS" /D "_MAC" /D "_68K_" /D "WIN32" /D "NDEBUG" /Yu"Stdwin.h" /c
CPP_PROJ=/nologo /AL /Q68s /W3 /GX /O2 /D "_WINDOWS" /D "_MAC" /D "_68K_" /D\
 "WIN32" /D "NDEBUG" /Fp"$(INTDIR)/gridfont.pch" /Yu"Stdwin.h" /Fo"$(INTDIR)/"\
 /c 
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
RSC_PROJ=/l 0x409 /r /m /fo"$(INTDIR)/gridfont.rsc" /d "_MAC" /d "_68K_" /d\
 "NDEBUG" 
MRC=mrc.exe
# ADD BASE MRC /D "_68K_" /D "_MAC" /D "NDEBUG" /NOLOGO
# ADD MRC /D "_68K_" /D "_MAC" /D "NDEBUG" /NOLOGO
MRC_PROJ=/D "_68K_" /D "_MAC" /D "NDEBUG" /l 0x409 /NOLOGO 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/gridfont.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 wlm.lib /nologo /MAC:bundle /machine:M68K
# ADD LINK32 wlm.lib /nologo /MAC:bundle /MAC:creator="GRID" /machine:M68K
LINK32_FLAGS=wlm.lib /nologo /MAC:bundle /MAC:type="APPL" /MAC:creator="GRID"\
 /pdb:"$(OUTDIR)/gridfont.pdb" /machine:M68K /out:"$(OUTDIR)/gridfont.exe" 
LINK32_OBJS= \
	"$(INTDIR)/view.obj" \
	"$(INTDIR)/app.obj" \
	"$(INTDIR)/cset.obj" \
	"$(INTDIR)/stdwin.obj" \
	"$(INTDIR)/grid.obj" \
	"$(INTDIR)/font.obj" \
	"$(INTDIR)/box.obj" \
	"$(INTDIR)/gridfont.rsc" \
	"$(INTDIR)/grid.rsc"

"$(OUTDIR)\gridfont.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

MFILE32=mfile.exe
# ADD BASE MFILE32 COPY /NOLOGO
# ADD MFILE32 COPY /NOLOGO
MFILE32_FLAGS=COPY /NOLOGO 
MFILE32_FILES= \
	"$(OUTDIR)/gridfont.exe"

"$(OUTDIR)\gridfont.trg" : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\MacRel\gridfont.exe\
 "$(MFILE32_DEST):gridfont.exe">"$(OUTDIR)\gridfont.trg"

DOWNLOAD : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\MacRel\gridfont.exe\
 "$(MFILE32_DEST):gridfont.exe">"$(OUTDIR)\gridfont.trg"

!ELSEIF  "$(CFG)" == "GridFont - Macintosh Debug"

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

ALL : "$(OUTDIR)\gridfont.exe" "$(OUTDIR)\gridfont.trg"

CLEAN : 
	-@erase ".\MacDbg\vc40.pdb"
	-@erase ".\MacDbg\gridfont.pch"
	-@erase ".\MacDbg\gridfont.pdb"
	-@erase ".\MacDbg\app.obj"
	-@erase ".\MacDbg\cset.obj"
	-@erase ".\MacDbg\grid.obj"
	-@erase ".\MacDbg\font.obj"
	-@erase ".\MacDbg\box.obj"
	-@erase ".\MacDbg\view.obj"
	-@erase ".\MacDbg\stdwin.obj"
	-@erase ".\MacDbg\grid.rsc"
	-@erase ".\MacDbg\gridfont.rsc"
	-@erase ".\MacDbg\gridfont.trg"
	-@erase ".\MacDbg\gridfont.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

MTL=mktyplib.exe
# ADD BASE MTL /nologo /D "_DEBUG" /mac
# ADD MTL /nologo /D "_DEBUG" /mac
MTL_PROJ=/nologo /D "_DEBUG" /mac 
CPP=cl.exe
# ADD BASE CPP /nologo /AL /Q68s /Q68m /W3 /GX /Zi /Od /D "_WINDOWS" /D "_MAC" /D "_68K_" /D "WIN32" /D "_DEBUG" /YX /c
# ADD CPP /nologo /AL /Q68s /Q68m /W3 /GX /Zi /Od /D "_WINDOWS" /D "_MAC" /D "_68K_" /D "WIN32" /D "_DEBUG" /Yu"Stdwin.h" /c
CPP_PROJ=/nologo /AL /Q68s /Q68m /W3 /GX /Zi /Od /D "_WINDOWS" /D "_MAC" /D\
 "_68K_" /D "WIN32" /D "_DEBUG" /Fp"$(INTDIR)/gridfont.pch" /Yu"Stdwin.h"\
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
RSC_PROJ=/l 0x409 /r /m /fo"$(INTDIR)/gridfont.rsc" /d "_MAC" /d "_68K_" /d\
 "_DEBUG" 
MRC=mrc.exe
# ADD BASE MRC /D "_68K_" /D "_MAC" /D "_DEBUG" /NOLOGO
# ADD MRC /D "_68K_" /D "_MAC" /D "_DEBUG" /NOLOGO
MRC_PROJ=/D "_68K_" /D "_MAC" /D "_DEBUG" /l 0x409 /NOLOGO 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/gridfont.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 wlm.lib /nologo /MAC:bundle /debug /machine:M68K
# ADD LINK32 wlm.lib /nologo /MAC:bundle /MAC:creator="GRID" /debug /machine:M68K
LINK32_FLAGS=wlm.lib /nologo /MAC:bundle /MAC:type="APPL" /MAC:creator="GRID"\
 /pdb:"$(OUTDIR)/gridfont.pdb" /debug /machine:M68K\
 /out:"$(OUTDIR)/gridfont.exe" 
LINK32_OBJS= \
	"$(INTDIR)/app.obj" \
	"$(INTDIR)/cset.obj" \
	"$(INTDIR)/grid.obj" \
	"$(INTDIR)/font.obj" \
	"$(INTDIR)/box.obj" \
	"$(INTDIR)/view.obj" \
	"$(INTDIR)/stdwin.obj" \
	"$(INTDIR)/grid.rsc" \
	"$(INTDIR)/gridfont.rsc"

"$(OUTDIR)\gridfont.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

MFILE32=mfile.exe
# ADD BASE MFILE32 COPY /NOLOGO
# ADD MFILE32 COPY /NOLOGO
MFILE32_FLAGS=COPY /NOLOGO 
MFILE32_FILES= \
	"$(OUTDIR)/gridfont.exe"

"$(OUTDIR)\gridfont.trg" : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\MacDbg\gridfont.exe\
 "$(MFILE32_DEST):gridfont.exe">"$(OUTDIR)\gridfont.trg"

DOWNLOAD : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\MacDbg\gridfont.exe\
 "$(MFILE32_DEST):gridfont.exe">"$(OUTDIR)\gridfont.trg"

!ELSEIF  "$(CFG)" == "GridFont - Power Macintosh Release"

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

ALL : "$(OUTDIR)\gridfont.exe" "$(OUTDIR)\gridfont.trg"

CLEAN : 
	-@erase ".\PMacRel\gridfont.trg"
	-@erase ".\PMacRel\gridfont.exe"
	-@erase ".\PMacRel\view.obj"
	-@erase ".\PMacRel\gridfont.pch"
	-@erase ".\PMacRel\stdwin.obj"
	-@erase ".\PMacRel\cset.obj"
	-@erase ".\PMacRel\box.obj"
	-@erase ".\PMacRel\grid.obj"
	-@erase ".\PMacRel\font.obj"
	-@erase ".\PMacRel\app.obj"
	-@erase ".\PMacRel\grid.rsc"
	-@erase ".\PMacRel\gridfont.rsc"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

MTL=mktyplib.exe
# ADD BASE MTL /nologo /D "NDEBUG"
# ADD MTL /nologo /D "NDEBUG"
MTL_PROJ=/nologo /D "NDEBUG" 
CPP=cl.exe
# ADD BASE CPP /nologo /W3 /GX /O2 /D "_MAC" /D "_MPPC_" /D "_WINDOWS" /D "WIN32" /D "NDEBUG" /YX /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "_MAC" /D "_MPPC_" /D "_WINDOWS" /D "WIN32" /D "NDEBUG" /D "_WLMDLL" /Yu"Stdwin.h" /c
# SUBTRACT CPP /Fr
CPP_PROJ=/nologo /MD /W3 /GX /O2 /D "_MAC" /D "_MPPC_" /D "_WINDOWS" /D "WIN32"\
 /D "NDEBUG" /D "_WLMDLL" /Fp"$(INTDIR)/gridfont.pch" /Yu"Stdwin.h"\
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
RSC_PROJ=/l 0x409 /r /m /fo"$(INTDIR)/gridfont.rsc" /d "_MAC" /d "_MPPC_" /d\
 "NDEBUG" 
MRC=mrc.exe
# ADD BASE MRC /D "_MPPC_" /D "_MAC" /D "NDEBUG" /NOLOGO
# ADD MRC /D "_MPPC_" /D "_MAC" /D "NDEBUG" /NOLOGO
MRC_PROJ=/D "_MPPC_" /D "_MAC" /D "NDEBUG" /l 0x409 /NOLOGO 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/gridfont.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 cfrg.rsc /nologo /mac:bundle /machine:MPPC
# ADD LINK32 /nologo /mac:bundle /mac:creator="GRID" /machine:MPPC
# SUBTRACT LINK32 /debug
LINK32_FLAGS=/nologo /mac:bundle /mac:type="APPL" /mac:creator="GRID"\
 /incremental:no /pdb:"$(OUTDIR)/gridfont.pdb" /machine:MPPC\
 /out:"$(OUTDIR)/gridfont.exe" 
LINK32_OBJS= \
	"$(INTDIR)/view.obj" \
	"$(INTDIR)/stdwin.obj" \
	"$(INTDIR)/cset.obj" \
	"$(INTDIR)/box.obj" \
	"$(INTDIR)/grid.obj" \
	"$(INTDIR)/font.obj" \
	"$(INTDIR)/app.obj" \
	"$(INTDIR)/grid.rsc" \
	"$(INTDIR)/gridfont.rsc"

"$(OUTDIR)\gridfont.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

MFILE32=mfile.exe
# ADD BASE MFILE32 COPY /NOLOGO
# ADD MFILE32 COPY /NOLOGO
MFILE32_FLAGS=COPY /NOLOGO 
MFILE32_FILES= \
	"$(OUTDIR)/gridfont.exe"

"$(OUTDIR)\gridfont.trg" : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\PMacRel\gridfont.exe\
 "$(MFILE32_DEST):gridfont.exe">"$(OUTDIR)\gridfont.trg"

DOWNLOAD : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\PMacRel\gridfont.exe\
 "$(MFILE32_DEST):gridfont.exe">"$(OUTDIR)\gridfont.trg"

!ELSEIF  "$(CFG)" == "GridFont - Power Macintosh Debug"

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

ALL : "$(OUTDIR)\gridfont.exe" "$(OUTDIR)\gridfont.trg"

CLEAN : 
	-@erase ".\PMacDbg\vc40.pdb"
	-@erase ".\PMacDbg\gridfont.pch"
	-@erase ".\PMacDbg\gridfont.ilk"
	-@erase ".\PMacDbg\cset.obj"
	-@erase ".\PMacDbg\box.obj"
	-@erase ".\PMacDbg\grid.obj"
	-@erase ".\PMacDbg\stdwin.obj"
	-@erase ".\PMacDbg\font.obj"
	-@erase ".\PMacDbg\view.obj"
	-@erase ".\PMacDbg\app.obj"
	-@erase ".\PMacDbg\grid.rsc"
	-@erase ".\PMacDbg\gridfont.rsc"
	-@erase ".\PMacDbg\gridfont.pdb"
	-@erase ".\PMacDbg\gridfont.trg"
	-@erase ".\PMacDbg\gridfont.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

MTL=mktyplib.exe
# ADD BASE MTL /nologo /D "_DEBUG"
# ADD MTL /nologo /D "_DEBUG"
MTL_PROJ=/nologo /D "_DEBUG" 
CPP=cl.exe
# ADD BASE CPP /nologo /W3 /GX /Zi /Od /D "_MAC" /D "_MPPC_" /D "_WINDOWS" /D "WIN32" /D "_DEBUG" /YX /c
# ADD CPP /nologo /MDd /W3 /GX /Zi /Od /D "_MAC" /D "_MPPC_" /D "_WINDOWS" /D "WIN32" /D "_DEBUG" /D "_WLMDLL" /Yu"Stdwin.h" /c
CPP_PROJ=/nologo /MDd /W3 /GX /Zi /Od /D "_MAC" /D "_MPPC_" /D "_WINDOWS" /D\
 "WIN32" /D "_DEBUG" /D "_WLMDLL" /Fp"$(INTDIR)/gridfont.pch" /Yu"Stdwin.h"\
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
RSC_PROJ=/l 0x409 /r /m /fo"$(INTDIR)/gridfont.rsc" /d "_MAC" /d "_MPPC_" /d\
 "_DEBUG" 
MRC=mrc.exe
# ADD BASE MRC /D "_MPPC_" /D "_MAC" /D "_DEBUG" /NOLOGO
# ADD MRC /D "_MPPC_" /D "_MAC" /D "_DEBUG" /NOLOGO
MRC_PROJ=/D "_MPPC_" /D "_MAC" /D "_DEBUG" /l 0x409 /NOLOGO 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/gridfont.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 cfrg.rsc /nologo /mac:bundle /debug /machine:MPPC
# ADD LINK32 /nologo /mac:bundle /mac:creator="GRID" /debug /machine:MPPC
LINK32_FLAGS=/nologo /mac:bundle /mac:type="APPL" /mac:creator="GRID"\
 /incremental:yes /pdb:"$(OUTDIR)/gridfont.pdb" /debug /machine:MPPC\
 /out:"$(OUTDIR)/gridfont.exe" 
LINK32_OBJS= \
	"$(INTDIR)/cset.obj" \
	"$(INTDIR)/box.obj" \
	"$(INTDIR)/grid.obj" \
	"$(INTDIR)/stdwin.obj" \
	"$(INTDIR)/font.obj" \
	"$(INTDIR)/view.obj" \
	"$(INTDIR)/app.obj" \
	"$(INTDIR)/grid.rsc" \
	"$(INTDIR)/gridfont.rsc"

"$(OUTDIR)\gridfont.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

MFILE32=mfile.exe
# ADD BASE MFILE32 COPY /NOLOGO
# ADD MFILE32 COPY /NOLOGO
MFILE32_FLAGS=COPY /NOLOGO 
MFILE32_FILES= \
	"$(OUTDIR)/gridfont.exe"

"$(OUTDIR)\gridfont.trg" : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\PMacDbg\gridfont.exe\
 "$(MFILE32_DEST):gridfont.exe">"$(OUTDIR)\gridfont.trg"

DOWNLOAD : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\PMacDbg\gridfont.exe\
 "$(MFILE32_DEST):gridfont.exe">"$(OUTDIR)\gridfont.trg"

!ELSEIF  "$(CFG)" == "GridFont - Win32 Unicode Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir Release
# PROP BASE Intermediate_Dir Release
# PROP BASE Target_Dir
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir UniRelease
# PROP Intermediate_Dir UniRelease
# PROP Target_Dir
OUTDIR=.\UniRelease
INTDIR=.\UniRelease

ALL : "$(OUTDIR)\gridfont.exe"

CLEAN : 
	-@erase ".\UniRelease\gridfont.exe"
	-@erase ".\UniRelease\font.obj"
	-@erase ".\UniRelease\gridfont.pch"
	-@erase ".\UniRelease\stdwin.obj"
	-@erase ".\UniRelease\app.obj"
	-@erase ".\UniRelease\view.obj"
	-@erase ".\UniRelease\cset.obj"
	-@erase ".\UniRelease\box.obj"
	-@erase ".\UniRelease\grid.obj"
	-@erase ".\UniRelease\gridfont.res"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_UNICODE" /Yu"Stdwin.h" /c
CPP_PROJ=/nologo /ML /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D\
 "_UNICODE" /Fp"$(INTDIR)/gridfont.pch" /Yu"Stdwin.h" /Fo"$(INTDIR)/" /c 
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
# ADD RSC /l 0x409 /d "NDEBUG"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/gridfont.res" /d "NDEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/gridfont.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib /nologo /subsystem:windows /incremental:no\
 /pdb:"$(OUTDIR)/gridfont.pdb" /machine:I386 /out:"$(OUTDIR)/gridfont.exe" 
LINK32_OBJS= \
	"$(INTDIR)/font.obj" \
	"$(INTDIR)/stdwin.obj" \
	"$(INTDIR)/app.obj" \
	"$(INTDIR)/view.obj" \
	"$(INTDIR)/cset.obj" \
	"$(INTDIR)/box.obj" \
	"$(INTDIR)/grid.obj" \
	"$(INTDIR)/gridfont.res"

"$(OUTDIR)\gridfont.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "GridFont - Win32 Unicode Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir Debug
# PROP BASE Intermediate_Dir Debug
# PROP BASE Target_Dir
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir UniDebug
# PROP Intermediate_Dir UniDebug
# PROP Target_Dir
OUTDIR=.\UniDebug
INTDIR=.\UniDebug

ALL : "$(OUTDIR)\gridfont.exe"

CLEAN : 
	-@erase ".\UniDebug\vc40.pdb"
	-@erase ".\UniDebug\gridfont.pch"
	-@erase ".\UniDebug\gridfont.exe"
	-@erase ".\UniDebug\grid.obj"
	-@erase ".\UniDebug\font.obj"
	-@erase ".\UniDebug\app.obj"
	-@erase ".\UniDebug\view.obj"
	-@erase ".\UniDebug\stdwin.obj"
	-@erase ".\UniDebug\cset.obj"
	-@erase ".\UniDebug\box.obj"
	-@erase ".\UniDebug\gridfont.res"
	-@erase ".\UniDebug\gridfont.ilk"
	-@erase ".\UniDebug\gridfont.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_UNICODE" /Yu"Stdwin.h" /c
CPP_PROJ=/nologo /MLd /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS"\
 /D "_UNICODE" /Fp"$(INTDIR)/gridfont.pch" /Yu"Stdwin.h" /Fo"$(INTDIR)/"\
 /Fd"$(INTDIR)/" /c 
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
# ADD RSC /l 0x409 /d "_DEBUG"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/gridfont.res" /d "_DEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/gridfont.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib /nologo /subsystem:windows /incremental:yes\
 /pdb:"$(OUTDIR)/gridfont.pdb" /debug /machine:I386\
 /out:"$(OUTDIR)/gridfont.exe" 
LINK32_OBJS= \
	"$(INTDIR)/grid.obj" \
	"$(INTDIR)/font.obj" \
	"$(INTDIR)/app.obj" \
	"$(INTDIR)/view.obj" \
	"$(INTDIR)/stdwin.obj" \
	"$(INTDIR)/cset.obj" \
	"$(INTDIR)/box.obj" \
	"$(INTDIR)/gridfont.res"

"$(OUTDIR)\gridfont.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 

################################################################################
# Begin Target

# Name "GridFont - Win32 Release"
# Name "GridFont - Win32 Debug"
# Name "GridFont - Macintosh Release"
# Name "GridFont - Macintosh Debug"
# Name "GridFont - Power Macintosh Release"
# Name "GridFont - Power Macintosh Debug"
# Name "GridFont - Win32 Unicode Release"
# Name "GridFont - Win32 Unicode Debug"

!IF  "$(CFG)" == "GridFont - Win32 Release"

!ELSEIF  "$(CFG)" == "GridFont - Win32 Debug"

!ELSEIF  "$(CFG)" == "GridFont - Macintosh Release"

!ELSEIF  "$(CFG)" == "GridFont - Macintosh Debug"

!ELSEIF  "$(CFG)" == "GridFont - Power Macintosh Release"

!ELSEIF  "$(CFG)" == "GridFont - Power Macintosh Debug"

!ELSEIF  "$(CFG)" == "GridFont - Win32 Unicode Release"

!ELSEIF  "$(CFG)" == "GridFont - Win32 Unicode Debug"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\view.cxx
DEP_CPP_VIEW_=\
	".\app.h"\
	".\grid.hxx"\
	".\canvas.hxx"\
	".\cset.hxx"\
	".\font.hxx"\
	".\stdwin.h"\
	".\view.hxx"\
	

!IF  "$(CFG)" == "GridFont - Win32 Release"


"$(INTDIR)\view.obj" : $(SOURCE) $(DEP_CPP_VIEW_) "$(INTDIR)"\
 "$(INTDIR)\gridfont.pch"


!ELSEIF  "$(CFG)" == "GridFont - Win32 Debug"


"$(INTDIR)\view.obj" : $(SOURCE) $(DEP_CPP_VIEW_) "$(INTDIR)"\
 "$(INTDIR)\gridfont.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_VIEW_) "$(INTDIR)"\
 "$(INTDIR)\gridfont.pch"


!ELSEIF  "$(CFG)" == "GridFont - Macintosh Release"


"$(INTDIR)\view.obj" : $(SOURCE) $(DEP_CPP_VIEW_) "$(INTDIR)"\
 "$(INTDIR)\gridfont.pch"


!ELSEIF  "$(CFG)" == "GridFont - Macintosh Debug"


"$(INTDIR)\view.obj" : $(SOURCE) $(DEP_CPP_VIEW_) "$(INTDIR)"\
 "$(INTDIR)\gridfont.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_VIEW_) "$(INTDIR)"\
 "$(INTDIR)\gridfont.pch"


!ELSEIF  "$(CFG)" == "GridFont - Power Macintosh Release"


"$(INTDIR)\view.obj" : $(SOURCE) $(DEP_CPP_VIEW_) "$(INTDIR)"\
 "$(INTDIR)\gridfont.pch"


!ELSEIF  "$(CFG)" == "GridFont - Power Macintosh Debug"


"$(INTDIR)\view.obj" : $(SOURCE) $(DEP_CPP_VIEW_) "$(INTDIR)"\
 "$(INTDIR)\gridfont.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_VIEW_) "$(INTDIR)"\
 "$(INTDIR)\gridfont.pch"


!ELSEIF  "$(CFG)" == "GridFont - Win32 Unicode Release"


"$(INTDIR)\view.obj" : $(SOURCE) $(DEP_CPP_VIEW_) "$(INTDIR)"\
 "$(INTDIR)\gridfont.pch"


!ELSEIF  "$(CFG)" == "GridFont - Win32 Unicode Debug"


"$(INTDIR)\view.obj" : $(SOURCE) $(DEP_CPP_VIEW_) "$(INTDIR)"\
 "$(INTDIR)\gridfont.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_VIEW_) "$(INTDIR)"\
 "$(INTDIR)\gridfont.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\box.cxx
DEP_CPP_BOX_C=\
	".\app.h"\
	".\box.hxx"\
	".\font.hxx"\
	".\canvas.hxx"\
	".\grid.hxx"\
	".\cset.hxx"\
	".\stdwin.h"\
	

!IF  "$(CFG)" == "GridFont - Win32 Release"


"$(INTDIR)\box.obj" : $(SOURCE) $(DEP_CPP_BOX_C) "$(INTDIR)"\
 "$(INTDIR)\gridfont.pch"


!ELSEIF  "$(CFG)" == "GridFont - Win32 Debug"


"$(INTDIR)\box.obj" : $(SOURCE) $(DEP_CPP_BOX_C) "$(INTDIR)"\
 "$(INTDIR)\gridfont.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_BOX_C) "$(INTDIR)"\
 "$(INTDIR)\gridfont.pch"


!ELSEIF  "$(CFG)" == "GridFont - Macintosh Release"


"$(INTDIR)\box.obj" : $(SOURCE) $(DEP_CPP_BOX_C) "$(INTDIR)"\
 "$(INTDIR)\gridfont.pch"


!ELSEIF  "$(CFG)" == "GridFont - Macintosh Debug"


"$(INTDIR)\box.obj" : $(SOURCE) $(DEP_CPP_BOX_C) "$(INTDIR)"\
 "$(INTDIR)\gridfont.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_BOX_C) "$(INTDIR)"\
 "$(INTDIR)\gridfont.pch"


!ELSEIF  "$(CFG)" == "GridFont - Power Macintosh Release"


"$(INTDIR)\box.obj" : $(SOURCE) $(DEP_CPP_BOX_C) "$(INTDIR)"\
 "$(INTDIR)\gridfont.pch"


!ELSEIF  "$(CFG)" == "GridFont - Power Macintosh Debug"


"$(INTDIR)\box.obj" : $(SOURCE) $(DEP_CPP_BOX_C) "$(INTDIR)"\
 "$(INTDIR)\gridfont.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_BOX_C) "$(INTDIR)"\
 "$(INTDIR)\gridfont.pch"


!ELSEIF  "$(CFG)" == "GridFont - Win32 Unicode Release"


"$(INTDIR)\box.obj" : $(SOURCE) $(DEP_CPP_BOX_C) "$(INTDIR)"\
 "$(INTDIR)\gridfont.pch"


!ELSEIF  "$(CFG)" == "GridFont - Win32 Unicode Debug"


"$(INTDIR)\box.obj" : $(SOURCE) $(DEP_CPP_BOX_C) "$(INTDIR)"\
 "$(INTDIR)\gridfont.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_BOX_C) "$(INTDIR)"\
 "$(INTDIR)\gridfont.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\cset.cxx
DEP_CPP_CSET_=\
	".\app.h"\
	".\cset.hxx"\
	".\stdwin.h"\
	

!IF  "$(CFG)" == "GridFont - Win32 Release"


"$(INTDIR)\cset.obj" : $(SOURCE) $(DEP_CPP_CSET_) "$(INTDIR)"\
 "$(INTDIR)\gridfont.pch"


!ELSEIF  "$(CFG)" == "GridFont - Win32 Debug"


"$(INTDIR)\cset.obj" : $(SOURCE) $(DEP_CPP_CSET_) "$(INTDIR)"\
 "$(INTDIR)\gridfont.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_CSET_) "$(INTDIR)"\
 "$(INTDIR)\gridfont.pch"


!ELSEIF  "$(CFG)" == "GridFont - Macintosh Release"


"$(INTDIR)\cset.obj" : $(SOURCE) $(DEP_CPP_CSET_) "$(INTDIR)"\
 "$(INTDIR)\gridfont.pch"


!ELSEIF  "$(CFG)" == "GridFont - Macintosh Debug"


"$(INTDIR)\cset.obj" : $(SOURCE) $(DEP_CPP_CSET_) "$(INTDIR)"\
 "$(INTDIR)\gridfont.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_CSET_) "$(INTDIR)"\
 "$(INTDIR)\gridfont.pch"


!ELSEIF  "$(CFG)" == "GridFont - Power Macintosh Release"


"$(INTDIR)\cset.obj" : $(SOURCE) $(DEP_CPP_CSET_) "$(INTDIR)"\
 "$(INTDIR)\gridfont.pch"


!ELSEIF  "$(CFG)" == "GridFont - Power Macintosh Debug"


"$(INTDIR)\cset.obj" : $(SOURCE) $(DEP_CPP_CSET_) "$(INTDIR)"\
 "$(INTDIR)\gridfont.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_CSET_) "$(INTDIR)"\
 "$(INTDIR)\gridfont.pch"


!ELSEIF  "$(CFG)" == "GridFont - Win32 Unicode Release"


"$(INTDIR)\cset.obj" : $(SOURCE) $(DEP_CPP_CSET_) "$(INTDIR)"\
 "$(INTDIR)\gridfont.pch"


!ELSEIF  "$(CFG)" == "GridFont - Win32 Unicode Debug"


"$(INTDIR)\cset.obj" : $(SOURCE) $(DEP_CPP_CSET_) "$(INTDIR)"\
 "$(INTDIR)\gridfont.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_CSET_) "$(INTDIR)"\
 "$(INTDIR)\gridfont.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\font.cxx
DEP_CPP_FONT_=\
	".\app.h"\
	".\font.hxx"\
	".\canvas.hxx"\
	".\stdwin.h"\
	

!IF  "$(CFG)" == "GridFont - Win32 Release"


"$(INTDIR)\font.obj" : $(SOURCE) $(DEP_CPP_FONT_) "$(INTDIR)"\
 "$(INTDIR)\gridfont.pch"


!ELSEIF  "$(CFG)" == "GridFont - Win32 Debug"


"$(INTDIR)\font.obj" : $(SOURCE) $(DEP_CPP_FONT_) "$(INTDIR)"\
 "$(INTDIR)\gridfont.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_FONT_) "$(INTDIR)"\
 "$(INTDIR)\gridfont.pch"


!ELSEIF  "$(CFG)" == "GridFont - Macintosh Release"


"$(INTDIR)\font.obj" : $(SOURCE) $(DEP_CPP_FONT_) "$(INTDIR)"\
 "$(INTDIR)\gridfont.pch"


!ELSEIF  "$(CFG)" == "GridFont - Macintosh Debug"


"$(INTDIR)\font.obj" : $(SOURCE) $(DEP_CPP_FONT_) "$(INTDIR)"\
 "$(INTDIR)\gridfont.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_FONT_) "$(INTDIR)"\
 "$(INTDIR)\gridfont.pch"


!ELSEIF  "$(CFG)" == "GridFont - Power Macintosh Release"


"$(INTDIR)\font.obj" : $(SOURCE) $(DEP_CPP_FONT_) "$(INTDIR)"\
 "$(INTDIR)\gridfont.pch"


!ELSEIF  "$(CFG)" == "GridFont - Power Macintosh Debug"


"$(INTDIR)\font.obj" : $(SOURCE) $(DEP_CPP_FONT_) "$(INTDIR)"\
 "$(INTDIR)\gridfont.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_FONT_) "$(INTDIR)"\
 "$(INTDIR)\gridfont.pch"


!ELSEIF  "$(CFG)" == "GridFont - Win32 Unicode Release"


"$(INTDIR)\font.obj" : $(SOURCE) $(DEP_CPP_FONT_) "$(INTDIR)"\
 "$(INTDIR)\gridfont.pch"


!ELSEIF  "$(CFG)" == "GridFont - Win32 Unicode Debug"


"$(INTDIR)\font.obj" : $(SOURCE) $(DEP_CPP_FONT_) "$(INTDIR)"\
 "$(INTDIR)\gridfont.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_FONT_) "$(INTDIR)"\
 "$(INTDIR)\gridfont.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\grid.cxx
DEP_CPP_GRID_=\
	".\app.h"\
	".\grid.hxx"\
	".\canvas.hxx"\
	".\cset.hxx"\
	".\font.hxx"\
	".\stdwin.h"\
	

!IF  "$(CFG)" == "GridFont - Win32 Release"


"$(INTDIR)\grid.obj" : $(SOURCE) $(DEP_CPP_GRID_) "$(INTDIR)"\
 "$(INTDIR)\gridfont.pch"


!ELSEIF  "$(CFG)" == "GridFont - Win32 Debug"


"$(INTDIR)\grid.obj" : $(SOURCE) $(DEP_CPP_GRID_) "$(INTDIR)"\
 "$(INTDIR)\gridfont.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_GRID_) "$(INTDIR)"\
 "$(INTDIR)\gridfont.pch"


!ELSEIF  "$(CFG)" == "GridFont - Macintosh Release"


"$(INTDIR)\grid.obj" : $(SOURCE) $(DEP_CPP_GRID_) "$(INTDIR)"\
 "$(INTDIR)\gridfont.pch"


!ELSEIF  "$(CFG)" == "GridFont - Macintosh Debug"


"$(INTDIR)\grid.obj" : $(SOURCE) $(DEP_CPP_GRID_) "$(INTDIR)"\
 "$(INTDIR)\gridfont.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_GRID_) "$(INTDIR)"\
 "$(INTDIR)\gridfont.pch"


!ELSEIF  "$(CFG)" == "GridFont - Power Macintosh Release"


"$(INTDIR)\grid.obj" : $(SOURCE) $(DEP_CPP_GRID_) "$(INTDIR)"\
 "$(INTDIR)\gridfont.pch"


!ELSEIF  "$(CFG)" == "GridFont - Power Macintosh Debug"


"$(INTDIR)\grid.obj" : $(SOURCE) $(DEP_CPP_GRID_) "$(INTDIR)"\
 "$(INTDIR)\gridfont.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_GRID_) "$(INTDIR)"\
 "$(INTDIR)\gridfont.pch"


!ELSEIF  "$(CFG)" == "GridFont - Win32 Unicode Release"


"$(INTDIR)\grid.obj" : $(SOURCE) $(DEP_CPP_GRID_) "$(INTDIR)"\
 "$(INTDIR)\gridfont.pch"


!ELSEIF  "$(CFG)" == "GridFont - Win32 Unicode Debug"


"$(INTDIR)\grid.obj" : $(SOURCE) $(DEP_CPP_GRID_) "$(INTDIR)"\
 "$(INTDIR)\gridfont.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_GRID_) "$(INTDIR)"\
 "$(INTDIR)\gridfont.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\gridfont.rc
DEP_RSC_GRIDF=\
	".\res\grid.ico"\
	

!IF  "$(CFG)" == "GridFont - Win32 Release"


"$(INTDIR)\gridfont.res" : $(SOURCE) $(DEP_RSC_GRIDF) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "GridFont - Win32 Debug"


"$(INTDIR)\gridfont.res" : $(SOURCE) $(DEP_RSC_GRIDF) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "GridFont - Macintosh Release"


"$(INTDIR)\gridfont.rsc" : $(SOURCE) $(DEP_RSC_GRIDF) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "GridFont - Macintosh Debug"


"$(INTDIR)\gridfont.rsc" : $(SOURCE) $(DEP_RSC_GRIDF) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "GridFont - Power Macintosh Release"


"$(INTDIR)\gridfont.rsc" : $(SOURCE) $(DEP_RSC_GRIDF) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "GridFont - Power Macintosh Debug"


"$(INTDIR)\gridfont.rsc" : $(SOURCE) $(DEP_RSC_GRIDF) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "GridFont - Win32 Unicode Release"


"$(INTDIR)\gridfont.res" : $(SOURCE) $(DEP_RSC_GRIDF) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "GridFont - Win32 Unicode Debug"


"$(INTDIR)\gridfont.res" : $(SOURCE) $(DEP_RSC_GRIDF) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\stdwin.cpp
DEP_CPP_STDWI=\
	".\stdwin.h"\
	

!IF  "$(CFG)" == "GridFont - Win32 Release"

# ADD CPP /Yc"Stdwin.h"

BuildCmds= \
	$(CPP) /nologo /ML /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS"\
 /Fp"$(INTDIR)/gridfont.pch" /Yc"Stdwin.h" /Fo"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\stdwin.obj" : $(SOURCE) $(DEP_CPP_STDWI) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\gridfont.pch" : $(SOURCE) $(DEP_CPP_STDWI) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "GridFont - Win32 Debug"

# ADD CPP /Yc"Stdwin.h"

BuildCmds= \
	$(CPP) /nologo /MLd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /Fp"$(INTDIR)/gridfont.pch" /Yc"Stdwin.h" /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c\
 $(SOURCE) \
	

"$(INTDIR)\stdwin.obj" : $(SOURCE) $(DEP_CPP_STDWI) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_STDWI) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\gridfont.pch" : $(SOURCE) $(DEP_CPP_STDWI) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "GridFont - Macintosh Release"

# ADD CPP /Yc"Stdwin.h"

BuildCmds= \
	$(CPP) /nologo /AL /Q68s /W3 /GX /O2 /D "_WINDOWS" /D "_MAC" /D "_68K_" /D\
 "WIN32" /D "NDEBUG" /Fp"$(INTDIR)/gridfont.pch" /Yc"Stdwin.h" /Fo"$(INTDIR)/"\
 /c $(SOURCE) \
	

"$(INTDIR)\stdwin.obj" : $(SOURCE) $(DEP_CPP_STDWI) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\gridfont.pch" : $(SOURCE) $(DEP_CPP_STDWI) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "GridFont - Macintosh Debug"

# ADD CPP /Yc"Stdwin.h"

BuildCmds= \
	$(CPP) /nologo /AL /Q68s /Q68m /W3 /GX /Zi /Od /D "_WINDOWS" /D "_MAC" /D\
 "_68K_" /D "WIN32" /D "_DEBUG" /Fp"$(INTDIR)/gridfont.pch" /Yc"Stdwin.h"\
 /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\stdwin.obj" : $(SOURCE) $(DEP_CPP_STDWI) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_STDWI) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\gridfont.pch" : $(SOURCE) $(DEP_CPP_STDWI) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "GridFont - Power Macintosh Release"

# ADD CPP /Yc"Stdwin.h"

BuildCmds= \
	$(CPP) /nologo /MD /W3 /GX /O2 /D "_MAC" /D "_MPPC_" /D "_WINDOWS" /D "WIN32"\
 /D "NDEBUG" /D "_WLMDLL" /Fp"$(INTDIR)/gridfont.pch" /Yc"Stdwin.h"\
 /Fo"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\stdwin.obj" : $(SOURCE) $(DEP_CPP_STDWI) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\gridfont.pch" : $(SOURCE) $(DEP_CPP_STDWI) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "GridFont - Power Macintosh Debug"

# ADD CPP /Yc"Stdwin.h"

BuildCmds= \
	$(CPP) /nologo /MDd /W3 /GX /Zi /Od /D "_MAC" /D "_MPPC_" /D "_WINDOWS" /D\
 "WIN32" /D "_DEBUG" /D "_WLMDLL" /Fp"$(INTDIR)/gridfont.pch" /Yc"Stdwin.h"\
 /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\stdwin.obj" : $(SOURCE) $(DEP_CPP_STDWI) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_STDWI) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\gridfont.pch" : $(SOURCE) $(DEP_CPP_STDWI) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "GridFont - Win32 Unicode Release"

# ADD CPP /Yc"Stdwin.h"

BuildCmds= \
	$(CPP) /nologo /ML /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D\
 "_UNICODE" /Fp"$(INTDIR)/gridfont.pch" /Yc"Stdwin.h" /Fo"$(INTDIR)/" /c\
 $(SOURCE) \
	

"$(INTDIR)\stdwin.obj" : $(SOURCE) $(DEP_CPP_STDWI) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\gridfont.pch" : $(SOURCE) $(DEP_CPP_STDWI) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "GridFont - Win32 Unicode Debug"

# ADD CPP /Yc"Stdwin.h"

BuildCmds= \
	$(CPP) /nologo /MLd /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS"\
 /D "_UNICODE" /Fp"$(INTDIR)/gridfont.pch" /Yc"Stdwin.h" /Fo"$(INTDIR)/"\
 /Fd"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\stdwin.obj" : $(SOURCE) $(DEP_CPP_STDWI) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_STDWI) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\gridfont.pch" : $(SOURCE) $(DEP_CPP_STDWI) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\app.cxx
DEP_CPP_APP_C=\
	".\app.hxx"\
	".\app.h"\
	".\box.hxx"\
	".\font.hxx"\
	".\canvas.hxx"\
	".\grid.hxx"\
	".\cset.hxx"\
	".\view.hxx"\
	".\stdwin.h"\
	

!IF  "$(CFG)" == "GridFont - Win32 Release"


"$(INTDIR)\app.obj" : $(SOURCE) $(DEP_CPP_APP_C) "$(INTDIR)"\
 "$(INTDIR)\gridfont.pch"


!ELSEIF  "$(CFG)" == "GridFont - Win32 Debug"


"$(INTDIR)\app.obj" : $(SOURCE) $(DEP_CPP_APP_C) "$(INTDIR)"\
 "$(INTDIR)\gridfont.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_APP_C) "$(INTDIR)"\
 "$(INTDIR)\gridfont.pch"


!ELSEIF  "$(CFG)" == "GridFont - Macintosh Release"


"$(INTDIR)\app.obj" : $(SOURCE) $(DEP_CPP_APP_C) "$(INTDIR)"\
 "$(INTDIR)\gridfont.pch"


!ELSEIF  "$(CFG)" == "GridFont - Macintosh Debug"


"$(INTDIR)\app.obj" : $(SOURCE) $(DEP_CPP_APP_C) "$(INTDIR)"\
 "$(INTDIR)\gridfont.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_APP_C) "$(INTDIR)"\
 "$(INTDIR)\gridfont.pch"


!ELSEIF  "$(CFG)" == "GridFont - Power Macintosh Release"


"$(INTDIR)\app.obj" : $(SOURCE) $(DEP_CPP_APP_C) "$(INTDIR)"\
 "$(INTDIR)\gridfont.pch"


!ELSEIF  "$(CFG)" == "GridFont - Power Macintosh Debug"


"$(INTDIR)\app.obj" : $(SOURCE) $(DEP_CPP_APP_C) "$(INTDIR)"\
 "$(INTDIR)\gridfont.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_APP_C) "$(INTDIR)"\
 "$(INTDIR)\gridfont.pch"


!ELSEIF  "$(CFG)" == "GridFont - Win32 Unicode Release"


"$(INTDIR)\app.obj" : $(SOURCE) $(DEP_CPP_APP_C) "$(INTDIR)"\
 "$(INTDIR)\gridfont.pch"


!ELSEIF  "$(CFG)" == "GridFont - Win32 Unicode Debug"


"$(INTDIR)\app.obj" : $(SOURCE) $(DEP_CPP_APP_C) "$(INTDIR)"\
 "$(INTDIR)\gridfont.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_APP_C) "$(INTDIR)"\
 "$(INTDIR)\gridfont.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\grid.r

!IF  "$(CFG)" == "GridFont - Win32 Release"

!ELSEIF  "$(CFG)" == "GridFont - Win32 Debug"

!ELSEIF  "$(CFG)" == "GridFont - Macintosh Release"

DEP_MRC_GRID_R=\
	{$(INCLUDE)}"\mrc\TYPES.R"\
	{$(INCLUDE)}"\mrc\BALLOONS.R"\
	

"$(INTDIR)\grid.rsc" : $(SOURCE) $(DEP_MRC_GRID_R) "$(INTDIR)"
   $(MRC) /o"$(INTDIR)/grid.rsc" /D "_68K_" /D "_MAC" /D "NDEBUG" /l 0x409\
 /NOLOGO $(SOURCE)


!ELSEIF  "$(CFG)" == "GridFont - Macintosh Debug"

DEP_MRC_GRID_R=\
	{$(INCLUDE)}"\mrc\TYPES.R"\
	{$(INCLUDE)}"\mrc\BALLOONS.R"\
	

"$(INTDIR)\grid.rsc" : $(SOURCE) $(DEP_MRC_GRID_R) "$(INTDIR)"
   $(MRC) /o"$(INTDIR)/grid.rsc" /D "_68K_" /D "_MAC" /D "_DEBUG" /l 0x409\
 /NOLOGO $(SOURCE)


!ELSEIF  "$(CFG)" == "GridFont - Power Macintosh Release"

DEP_MRC_GRID_R=\
	{$(INCLUDE)}"\mrc\TYPES.R"\
	{$(INCLUDE)}"\mrc\BALLOONS.R"\
	

"$(INTDIR)\grid.rsc" : $(SOURCE) $(DEP_MRC_GRID_R) "$(INTDIR)"
   $(MRC) /o"$(INTDIR)/grid.rsc" /D "_MPPC_" /D "_MAC" /D "NDEBUG" /l 0x409\
 /NOLOGO $(SOURCE)


!ELSEIF  "$(CFG)" == "GridFont - Power Macintosh Debug"

DEP_MRC_GRID_R=\
	{$(INCLUDE)}"\mrc\TYPES.R"\
	{$(INCLUDE)}"\mrc\BALLOONS.R"\
	

"$(INTDIR)\grid.rsc" : $(SOURCE) $(DEP_MRC_GRID_R) "$(INTDIR)"
   $(MRC) /o"$(INTDIR)/grid.rsc" /D "_MPPC_" /D "_MAC" /D "_DEBUG" /l 0x409\
 /NOLOGO $(SOURCE)


!ELSEIF  "$(CFG)" == "GridFont - Win32 Unicode Release"

!ELSEIF  "$(CFG)" == "GridFont - Win32 Unicode Debug"

!ENDIF 

# End Source File
# End Target
# End Project
################################################################################
