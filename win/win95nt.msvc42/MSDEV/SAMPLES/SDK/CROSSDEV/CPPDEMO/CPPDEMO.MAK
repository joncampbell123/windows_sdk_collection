# Microsoft Visual C++ Generated NMAKE File, Format Version 40000
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101
# TARGTYPE "Power Macintosh Application" 0x0401
# TARGTYPE "Macintosh Application" 0x0301

!IF "$(CFG)" == ""
CFG=cppdemo - Win32 Debug
!MESSAGE No configuration specified.  Defaulting to cppdemo - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "cppdemo - Win32 Release" && "$(CFG)" !=\
 "cppdemo - Win32 Debug" && "$(CFG)" != "cppdemo - Macintosh Release" &&\
 "$(CFG)" != "cppdemo - Macintosh Debug" && "$(CFG)" !=\
 "cppdemo - Power Macintosh Release" && "$(CFG)" !=\
 "cppdemo - Power Macintosh Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "cppdemo.mak" CFG="cppdemo - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "cppdemo - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "cppdemo - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE "cppdemo - Macintosh Release" (based on "Macintosh Application")
!MESSAGE "cppdemo - Macintosh Debug" (based on "Macintosh Application")
!MESSAGE "cppdemo - Power Macintosh Release" (based on\
 "Power Macintosh Application")
!MESSAGE "cppdemo - Power Macintosh Debug" (based on\
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
# PROP Target_Last_Scanned "cppdemo - Power Macintosh Debug"

!IF  "$(CFG)" == "cppdemo - Win32 Release"

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

ALL : "$(OUTDIR)\cppdemo.exe"

CLEAN : 
	-@erase ".\Release\cppdemo.exe"
	-@erase ".\Release\cppdemo.obj"
	-@erase ".\Release\codbc.obj"
	-@erase ".\Release\headers.obj"
	-@erase ".\Release\dialogs.obj"
	-@erase ".\Release\cppdemo.res"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
CPP_PROJ=/nologo /ML /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS"\
 /Fp"$(INTDIR)/cppdemo.pch" /YX /Fo"$(INTDIR)/" /c 
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
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/cppdemo.res" /d "NDEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/cppdemo.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib ctl3d32.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib ctl3d32.lib odbc32.lib\
 odbccp32.lib /nologo /subsystem:windows /incremental:no\
 /pdb:"$(OUTDIR)/cppdemo.pdb" /machine:I386 /out:"$(OUTDIR)/cppdemo.exe" 
LINK32_OBJS= \
	"$(INTDIR)/cppdemo.obj" \
	"$(INTDIR)/codbc.obj" \
	"$(INTDIR)/headers.obj" \
	"$(INTDIR)/dialogs.obj" \
	"$(INTDIR)/cppdemo.res"

"$(OUTDIR)\cppdemo.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "cppdemo - Win32 Debug"

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

ALL : "$(OUTDIR)\cppdemo.exe"

CLEAN : 
	-@erase ".\Debug\vc40.pdb"
	-@erase ".\Debug\cppdemo.exe"
	-@erase ".\Debug\dialogs.obj"
	-@erase ".\Debug\codbc.obj"
	-@erase ".\Debug\cppdemo.obj"
	-@erase ".\Debug\headers.obj"
	-@erase ".\Debug\cppdemo.res"
	-@erase ".\Debug\cppdemo.ilk"
	-@erase ".\Debug\cppdemo.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
CPP_PROJ=/nologo /MLd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /Fp"$(INTDIR)/cppdemo.pch" /YX /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
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
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/cppdemo.res" /d "_DEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/cppdemo.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib ctl3d32.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib ctl3d32.lib odbc32.lib\
 odbccp32.lib /nologo /subsystem:windows /incremental:yes\
 /pdb:"$(OUTDIR)/cppdemo.pdb" /debug /machine:I386 /out:"$(OUTDIR)/cppdemo.exe" 
LINK32_OBJS= \
	"$(INTDIR)/dialogs.obj" \
	"$(INTDIR)/codbc.obj" \
	"$(INTDIR)/cppdemo.obj" \
	"$(INTDIR)/headers.obj" \
	"$(INTDIR)/cppdemo.res"

"$(OUTDIR)\cppdemo.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "cppdemo - Macintosh Release"

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

ALL : "$(OUTDIR)\cppdemo.exe" "$(OUTDIR)\cppdemo.trg"

CLEAN : 
	-@erase ".\MacRel\cppdemo.trg"
	-@erase ".\MacRel\cppdemo.exe"
	-@erase ".\MacRel\cppdemo.obj"
	-@erase ".\MacRel\codbc.obj"
	-@erase ".\MacRel\headers.obj"
	-@erase ".\MacRel\dialogs.obj"
	-@erase ".\MacRel\cppdemo.rsc"
	-@erase ".\MacRel\cppdmac.rsc"

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
 "WIN32" /D "NDEBUG" /Fp"$(INTDIR)/cppdemo.pch" /YX /Fo"$(INTDIR)/" /c 
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
RSC_PROJ=/l 0x409 /r /m /fo"$(INTDIR)/cppdemo.rsc" /d "_MAC" /d "_68K_" /d\
 "NDEBUG" 
MRC=mrc.exe
# ADD BASE MRC /D "_68K_" /D "_MAC" /D "NDEBUG" /NOLOGO
# ADD MRC /D "_68K_" /D "_MAC" /D "NDEBUG" /NOLOGO
MRC_PROJ=/D "_68K_" /D "_MAC" /D "NDEBUG" /l 0x409 /NOLOGO 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/cppdemo.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 wlm.lib /nologo /MAC:bundle /machine:M68K
# ADD LINK32 wlm.lib /nologo /MAC:bundle /MAC:creator="CPPD" /machine:M68K
LINK32_FLAGS=wlm.lib /nologo /MAC:bundle /MAC:type="APPL" /MAC:creator="CPPD"\
 /pdb:"$(OUTDIR)/cppdemo.pdb" /machine:M68K /out:"$(OUTDIR)/cppdemo.exe" 
LINK32_OBJS= \
	"$(INTDIR)/cppdemo.obj" \
	"$(INTDIR)/codbc.obj" \
	"$(INTDIR)/headers.obj" \
	"$(INTDIR)/dialogs.obj" \
	"$(INTDIR)/cppdemo.rsc" \
	"$(INTDIR)/cppdmac.rsc"

"$(OUTDIR)\cppdemo.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

MFILE32=mfile.exe
# ADD BASE MFILE32 COPY /NOLOGO
# ADD MFILE32 COPY /NOLOGO
MFILE32_FLAGS=COPY /NOLOGO 
MFILE32_FILES= \
	"$(OUTDIR)/cppdemo.exe"

"$(OUTDIR)\cppdemo.trg" : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\MacRel\cppdemo.exe\
 "$(MFILE32_DEST):cppdemo.exe">"$(OUTDIR)\cppdemo.trg"

DOWNLOAD : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\MacRel\cppdemo.exe\
 "$(MFILE32_DEST):cppdemo.exe">"$(OUTDIR)\cppdemo.trg"

!ELSEIF  "$(CFG)" == "cppdemo - Macintosh Debug"

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

ALL : "$(OUTDIR)\cppdemo.exe" "$(OUTDIR)\cppdemo.trg"

CLEAN : 
	-@erase ".\MacDbg\vc40.pdb"
	-@erase ".\MacDbg\cppdemo.pdb"
	-@erase ".\MacDbg\dialogs.obj"
	-@erase ".\MacDbg\codbc.obj"
	-@erase ".\MacDbg\cppdemo.obj"
	-@erase ".\MacDbg\headers.obj"
	-@erase ".\MacDbg\cppdemo.rsc"
	-@erase ".\MacDbg\cppdmac.rsc"
	-@erase ".\MacDbg\cppdemo.trg"
	-@erase ".\MacDbg\cppdemo.exe"

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
 "_68K_" /D "WIN32" /D "_DEBUG" /Fp"$(INTDIR)/cppdemo.pch" /YX /Fo"$(INTDIR)/"\
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
RSC_PROJ=/l 0x409 /r /m /fo"$(INTDIR)/cppdemo.rsc" /d "_MAC" /d "_68K_" /d\
 "_DEBUG" 
MRC=mrc.exe
# ADD BASE MRC /D "_68K_" /D "_MAC" /D "_DEBUG" /NOLOGO
# ADD MRC /D "_68K_" /D "_MAC" /D "_DEBUG" /NOLOGO
MRC_PROJ=/D "_68K_" /D "_MAC" /D "_DEBUG" /l 0x409 /NOLOGO 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/cppdemo.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 wlm.lib /nologo /MAC:bundle /debug /machine:M68K
# ADD LINK32 wlm.lib /nologo /MAC:bundle /MAC:creator="CPPD" /debug /machine:M68K
LINK32_FLAGS=wlm.lib /nologo /MAC:bundle /MAC:type="APPL" /MAC:creator="CPPD"\
 /pdb:"$(OUTDIR)/cppdemo.pdb" /debug /machine:M68K /out:"$(OUTDIR)/cppdemo.exe" 
LINK32_OBJS= \
	"$(INTDIR)/dialogs.obj" \
	"$(INTDIR)/codbc.obj" \
	"$(INTDIR)/cppdemo.obj" \
	"$(INTDIR)/headers.obj" \
	"$(INTDIR)/cppdemo.rsc" \
	"$(INTDIR)/cppdmac.rsc"

"$(OUTDIR)\cppdemo.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

MFILE32=mfile.exe
# ADD BASE MFILE32 COPY /NOLOGO
# ADD MFILE32 COPY /NOLOGO
MFILE32_FLAGS=COPY /NOLOGO 
MFILE32_FILES= \
	"$(OUTDIR)/cppdemo.exe"

"$(OUTDIR)\cppdemo.trg" : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\MacDbg\cppdemo.exe\
 "$(MFILE32_DEST):cppdemo.exe">"$(OUTDIR)\cppdemo.trg"

DOWNLOAD : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\MacDbg\cppdemo.exe\
 "$(MFILE32_DEST):cppdemo.exe">"$(OUTDIR)\cppdemo.trg"

!ELSEIF  "$(CFG)" == "cppdemo - Power Macintosh Release"

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

ALL : "$(OUTDIR)\cppdemo.exe" "$(OUTDIR)\cppdemo.trg"

CLEAN : 
	-@erase ".\PMacRel\cppdemo.trg"
	-@erase ".\PMacRel\cppdemo.exe"
	-@erase ".\PMacRel\dialogs.obj"
	-@erase ".\PMacRel\codbc.obj"
	-@erase ".\PMacRel\cppdemo.obj"
	-@erase ".\PMacRel\headers.obj"
	-@erase ".\PMacRel\cppdemo.rsc"
	-@erase ".\PMacRel\cppdmac.rsc"

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
 /D "NDEBUG" /D "_WLMDLL" /Fp"$(INTDIR)/cppdemo.pch" /YX /Fo"$(INTDIR)/" /c 
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
RSC_PROJ=/l 0x409 /r /m /fo"$(INTDIR)/cppdemo.rsc" /d "_MAC" /d "_MPPC_" /d\
 "NDEBUG" 
MRC=mrc.exe
# ADD BASE MRC /D "_MPPC_" /D "_MAC" /D "NDEBUG" /NOLOGO
# ADD MRC /D "_MPPC_" /D "_MAC" /D "NDEBUG" /NOLOGO
MRC_PROJ=/D "_MPPC_" /D "_MAC" /D "NDEBUG" /l 0x409 /NOLOGO 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/cppdemo.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 cfrg.rsc /nologo /mac:bundle /machine:MPPC
# ADD LINK32 /nologo /mac:bundle /mac:creator="CPPD" /machine:MPPC
LINK32_FLAGS=/nologo /mac:bundle /mac:type="APPL" /mac:creator="CPPD"\
 /incremental:no /pdb:"$(OUTDIR)/cppdemo.pdb" /machine:MPPC\
 /out:"$(OUTDIR)/cppdemo.exe" 
LINK32_OBJS= \
	"$(INTDIR)/dialogs.obj" \
	"$(INTDIR)/codbc.obj" \
	"$(INTDIR)/cppdemo.obj" \
	"$(INTDIR)/headers.obj" \
	"$(INTDIR)/cppdemo.rsc" \
	"$(INTDIR)/cppdmac.rsc"

"$(OUTDIR)\cppdemo.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

MFILE32=mfile.exe
# ADD BASE MFILE32 COPY /NOLOGO
# ADD MFILE32 COPY /NOLOGO
MFILE32_FLAGS=COPY /NOLOGO 
MFILE32_FILES= \
	"$(OUTDIR)/cppdemo.exe"

"$(OUTDIR)\cppdemo.trg" : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\PMacRel\cppdemo.exe\
 "$(MFILE32_DEST):cppdemo.exe">"$(OUTDIR)\cppdemo.trg"

DOWNLOAD : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\PMacRel\cppdemo.exe\
 "$(MFILE32_DEST):cppdemo.exe">"$(OUTDIR)\cppdemo.trg"

!ELSEIF  "$(CFG)" == "cppdemo - Power Macintosh Debug"

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

ALL : "$(OUTDIR)\cppdemo.exe" "$(OUTDIR)\cppdemo.trg"

CLEAN : 
	-@erase ".\PMacDbg\vc40.pdb"
	-@erase ".\PMacDbg\cppdemo.ilk"
	-@erase ".\PMacDbg\cppdemo.obj"
	-@erase ".\PMacDbg\headers.obj"
	-@erase ".\PMacDbg\dialogs.obj"
	-@erase ".\PMacDbg\codbc.obj"
	-@erase ".\PMacDbg\cppdmac.rsc"
	-@erase ".\PMacDbg\cppdemo.rsc"
	-@erase ".\PMacDbg\cppdemo.pdb"
	-@erase ".\PMacDbg\cppdemo.trg"
	-@erase ".\PMacDbg\cppdemo.exe"

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
 "WIN32" /D "_DEBUG" /D "_WLMDLL" /Fp"$(INTDIR)/cppdemo.pch" /YX /Fo"$(INTDIR)/"\
 /Fd"$(INTDIR)/" /c 
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
RSC_PROJ=/l 0x409 /r /m /fo"$(INTDIR)/cppdemo.rsc" /d "_MAC" /d "_MPPC_" /d\
 "_DEBUG" 
MRC=mrc.exe
# ADD BASE MRC /D "_MPPC_" /D "_MAC" /D "_DEBUG" /NOLOGO
# ADD MRC /D "_MPPC_" /D "_MAC" /D "_DEBUG" /NOLOGO
MRC_PROJ=/D "_MPPC_" /D "_MAC" /D "_DEBUG" /l 0x409 /NOLOGO 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/cppdemo.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 cfrg.rsc /nologo /mac:bundle /debug /machine:MPPC
# ADD LINK32 /nologo /mac:bundle /mac:creator="CPPD" /debug /machine:MPPC
LINK32_FLAGS=/nologo /mac:bundle /mac:type="APPL" /mac:creator="CPPD"\
 /incremental:yes /pdb:"$(OUTDIR)/cppdemo.pdb" /debug /machine:MPPC\
 /out:"$(OUTDIR)/cppdemo.exe" 
LINK32_OBJS= \
	"$(INTDIR)/cppdemo.obj" \
	"$(INTDIR)/headers.obj" \
	"$(INTDIR)/dialogs.obj" \
	"$(INTDIR)/codbc.obj" \
	"$(INTDIR)/cppdmac.rsc" \
	"$(INTDIR)/cppdemo.rsc"

"$(OUTDIR)\cppdemo.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

MFILE32=mfile.exe
# ADD BASE MFILE32 COPY /NOLOGO
# ADD MFILE32 COPY /NOLOGO
MFILE32_FLAGS=COPY /NOLOGO 
MFILE32_FILES= \
	"$(OUTDIR)/cppdemo.exe"

"$(OUTDIR)\cppdemo.trg" : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\PMacDbg\cppdemo.exe\
 "$(MFILE32_DEST):cppdemo.exe">"$(OUTDIR)\cppdemo.trg"

DOWNLOAD : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\PMacDbg\cppdemo.exe\
 "$(MFILE32_DEST):cppdemo.exe">"$(OUTDIR)\cppdemo.trg"

!ENDIF 

################################################################################
# Begin Target

# Name "cppdemo - Win32 Release"
# Name "cppdemo - Win32 Debug"
# Name "cppdemo - Macintosh Release"
# Name "cppdemo - Macintosh Debug"
# Name "cppdemo - Power Macintosh Release"
# Name "cppdemo - Power Macintosh Debug"

!IF  "$(CFG)" == "cppdemo - Win32 Release"

!ELSEIF  "$(CFG)" == "cppdemo - Win32 Debug"

!ELSEIF  "$(CFG)" == "cppdemo - Macintosh Release"

!ELSEIF  "$(CFG)" == "cppdemo - Macintosh Debug"

!ELSEIF  "$(CFG)" == "cppdemo - Power Macintosh Release"

!ELSEIF  "$(CFG)" == "cppdemo - Power Macintosh Debug"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\headers.cpp
DEP_CPP_HEADE=\
	".\headers.h"\
	".\codbc.h"\
	".\codbc.inl"\
	

!IF  "$(CFG)" == "cppdemo - Win32 Release"


"$(INTDIR)\headers.obj" : $(SOURCE) $(DEP_CPP_HEADE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "cppdemo - Win32 Debug"


"$(INTDIR)\headers.obj" : $(SOURCE) $(DEP_CPP_HEADE) "$(INTDIR)"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_HEADE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "cppdemo - Macintosh Release"


"$(INTDIR)\headers.obj" : $(SOURCE) $(DEP_CPP_HEADE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "cppdemo - Macintosh Debug"


"$(INTDIR)\headers.obj" : $(SOURCE) $(DEP_CPP_HEADE) "$(INTDIR)"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_HEADE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "cppdemo - Power Macintosh Release"


"$(INTDIR)\headers.obj" : $(SOURCE) $(DEP_CPP_HEADE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "cppdemo - Power Macintosh Debug"


"$(INTDIR)\headers.obj" : $(SOURCE) $(DEP_CPP_HEADE) "$(INTDIR)"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_HEADE) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\codbc.cpp
DEP_CPP_CODBC=\
	".\headers.h"\
	".\codbc.h"\
	".\codbc.inl"\
	

!IF  "$(CFG)" == "cppdemo - Win32 Release"


"$(INTDIR)\codbc.obj" : $(SOURCE) $(DEP_CPP_CODBC) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "cppdemo - Win32 Debug"


"$(INTDIR)\codbc.obj" : $(SOURCE) $(DEP_CPP_CODBC) "$(INTDIR)"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_CODBC) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "cppdemo - Macintosh Release"


"$(INTDIR)\codbc.obj" : $(SOURCE) $(DEP_CPP_CODBC) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "cppdemo - Macintosh Debug"


"$(INTDIR)\codbc.obj" : $(SOURCE) $(DEP_CPP_CODBC) "$(INTDIR)"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_CODBC) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "cppdemo - Power Macintosh Release"


"$(INTDIR)\codbc.obj" : $(SOURCE) $(DEP_CPP_CODBC) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "cppdemo - Power Macintosh Debug"


"$(INTDIR)\codbc.obj" : $(SOURCE) $(DEP_CPP_CODBC) "$(INTDIR)"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_CODBC) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\cppdemo.cpp
DEP_CPP_CPPDE=\
	".\cppdemo.h"\
	".\headers.h"\
	".\codbc.h"\
	".\codbc.inl"\
	

!IF  "$(CFG)" == "cppdemo - Win32 Release"


"$(INTDIR)\cppdemo.obj" : $(SOURCE) $(DEP_CPP_CPPDE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "cppdemo - Win32 Debug"


"$(INTDIR)\cppdemo.obj" : $(SOURCE) $(DEP_CPP_CPPDE) "$(INTDIR)"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_CPPDE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "cppdemo - Macintosh Release"


"$(INTDIR)\cppdemo.obj" : $(SOURCE) $(DEP_CPP_CPPDE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "cppdemo - Macintosh Debug"


"$(INTDIR)\cppdemo.obj" : $(SOURCE) $(DEP_CPP_CPPDE) "$(INTDIR)"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_CPPDE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "cppdemo - Power Macintosh Release"


"$(INTDIR)\cppdemo.obj" : $(SOURCE) $(DEP_CPP_CPPDE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "cppdemo - Power Macintosh Debug"


"$(INTDIR)\cppdemo.obj" : $(SOURCE) $(DEP_CPP_CPPDE) "$(INTDIR)"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_CPPDE) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\dialogs.cpp
DEP_CPP_DIALO=\
	".\cppdemo.h"\
	".\headers.h"\
	".\codbc.h"\
	".\codbc.inl"\
	

!IF  "$(CFG)" == "cppdemo - Win32 Release"


"$(INTDIR)\dialogs.obj" : $(SOURCE) $(DEP_CPP_DIALO) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "cppdemo - Win32 Debug"


"$(INTDIR)\dialogs.obj" : $(SOURCE) $(DEP_CPP_DIALO) "$(INTDIR)"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_DIALO) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "cppdemo - Macintosh Release"


"$(INTDIR)\dialogs.obj" : $(SOURCE) $(DEP_CPP_DIALO) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "cppdemo - Macintosh Debug"


"$(INTDIR)\dialogs.obj" : $(SOURCE) $(DEP_CPP_DIALO) "$(INTDIR)"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_DIALO) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "cppdemo - Power Macintosh Release"


"$(INTDIR)\dialogs.obj" : $(SOURCE) $(DEP_CPP_DIALO) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "cppdemo - Power Macintosh Debug"


"$(INTDIR)\dialogs.obj" : $(SOURCE) $(DEP_CPP_DIALO) "$(INTDIR)"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_DIALO) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\cppdmac.r

!IF  "$(CFG)" == "cppdemo - Win32 Release"

!ELSEIF  "$(CFG)" == "cppdemo - Win32 Debug"

!ELSEIF  "$(CFG)" == "cppdemo - Macintosh Release"


"$(INTDIR)\cppdmac.rsc" : $(SOURCE) "$(INTDIR)"
   $(MRC) /o"$(INTDIR)/cppdmac.rsc" /D "_68K_" /D "_MAC" /D "NDEBUG" /l 0x409\
 /NOLOGO $(SOURCE)


!ELSEIF  "$(CFG)" == "cppdemo - Macintosh Debug"


"$(INTDIR)\cppdmac.rsc" : $(SOURCE) "$(INTDIR)"
   $(MRC) /o"$(INTDIR)/cppdmac.rsc" /D "_68K_" /D "_MAC" /D "_DEBUG" /l 0x409\
 /NOLOGO $(SOURCE)


!ELSEIF  "$(CFG)" == "cppdemo - Power Macintosh Release"


"$(INTDIR)\cppdmac.rsc" : $(SOURCE) "$(INTDIR)"
   $(MRC) /o"$(INTDIR)/cppdmac.rsc" /D "_MPPC_" /D "_MAC" /D "NDEBUG" /l 0x409\
 /NOLOGO $(SOURCE)


!ELSEIF  "$(CFG)" == "cppdemo - Power Macintosh Debug"


"$(INTDIR)\cppdmac.rsc" : $(SOURCE) "$(INTDIR)"
   $(MRC) /o"$(INTDIR)/cppdmac.rsc" /D "_MPPC_" /D "_MAC" /D "_DEBUG" /l 0x409\
 /NOLOGO $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\cppdemo.rc
DEP_RSC_CPPDEM=\
	".\cppdemo.ico"\
	

!IF  "$(CFG)" == "cppdemo - Win32 Release"


"$(INTDIR)\cppdemo.res" : $(SOURCE) $(DEP_RSC_CPPDEM) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "cppdemo - Win32 Debug"


"$(INTDIR)\cppdemo.res" : $(SOURCE) $(DEP_RSC_CPPDEM) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "cppdemo - Macintosh Release"


"$(INTDIR)\cppdemo.rsc" : $(SOURCE) $(DEP_RSC_CPPDEM) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "cppdemo - Macintosh Debug"


"$(INTDIR)\cppdemo.rsc" : $(SOURCE) $(DEP_RSC_CPPDEM) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "cppdemo - Power Macintosh Release"


"$(INTDIR)\cppdemo.rsc" : $(SOURCE) $(DEP_RSC_CPPDEM) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "cppdemo - Power Macintosh Debug"


"$(INTDIR)\cppdemo.rsc" : $(SOURCE) $(DEP_RSC_CPPDEM) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ENDIF 

# End Source File
# End Target
# End Project
################################################################################
