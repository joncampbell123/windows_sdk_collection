# Microsoft Visual C++ Generated NMAKE File, Format Version 40000
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101
# TARGTYPE "Power Macintosh Application" 0x0401
# TARGTYPE "Macintosh Application" 0x0301

!IF "$(CFG)" == ""
CFG=SubClass - Win32 Debug
!MESSAGE No configuration specified.  Defaulting to SubClass - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "SubClass - Win32 Release" && "$(CFG)" !=\
 "SubClass - Win32 Debug" && "$(CFG)" != "SubClass - Macintosh Release" &&\
 "$(CFG)" != "SubClass - Macintosh Debug" && "$(CFG)" !=\
 "SubClass - Power Macintosh Release" && "$(CFG)" !=\
 "SubClass - Power Macintosh Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "subclass.mak" CFG="SubClass - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "SubClass - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "SubClass - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE "SubClass - Macintosh Release" (based on "Macintosh Application")
!MESSAGE "SubClass - Macintosh Debug" (based on "Macintosh Application")
!MESSAGE "SubClass - Power Macintosh Release" (based on\
 "Power Macintosh Application")
!MESSAGE "SubClass - Power Macintosh Debug" (based on\
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
# PROP Target_Last_Scanned "SubClass - Power Macintosh Debug"

!IF  "$(CFG)" == "SubClass - Win32 Release"

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

ALL : "$(OUTDIR)\subclass.exe"

CLEAN : 
	-@erase ".\Release\subclass.exe"
	-@erase ".\Release\subclass.obj"
	-@erase ".\Release\subclass.pch"
	-@erase ".\Release\stdwin.obj"
	-@erase ".\Release\subclass.res"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /Yu"Stdwin.h" /c
CPP_PROJ=/nologo /ML /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS"\
 /Fp"$(INTDIR)/subclass.pch" /Yu"Stdwin.h" /Fo"$(INTDIR)/" /c 
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
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/subclass.res" /d "NDEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/subclass.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib /nologo /subsystem:windows /incremental:no\
 /pdb:"$(OUTDIR)/subclass.pdb" /machine:I386 /out:"$(OUTDIR)/subclass.exe" 
LINK32_OBJS= \
	"$(INTDIR)/subclass.obj" \
	"$(INTDIR)/stdwin.obj" \
	"$(INTDIR)/subclass.res"

"$(OUTDIR)\subclass.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "SubClass - Win32 Debug"

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

ALL : "$(OUTDIR)\subclass.exe"

CLEAN : 
	-@erase ".\Debug\vc40.pdb"
	-@erase ".\Debug\subclass.pch"
	-@erase ".\Debug\subclass.exe"
	-@erase ".\Debug\subclass.obj"
	-@erase ".\Debug\stdwin.obj"
	-@erase ".\Debug\subclass.res"
	-@erase ".\Debug\subclass.ilk"
	-@erase ".\Debug\subclass.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /Yu"Stdwin.h" /c
CPP_PROJ=/nologo /MLd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /Fp"$(INTDIR)/subclass.pch" /Yu"Stdwin.h" /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
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
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/subclass.res" /d "_DEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/subclass.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib /nologo /subsystem:windows /incremental:yes\
 /pdb:"$(OUTDIR)/subclass.pdb" /debug /machine:I386\
 /out:"$(OUTDIR)/subclass.exe" 
LINK32_OBJS= \
	"$(INTDIR)/subclass.obj" \
	"$(INTDIR)/stdwin.obj" \
	"$(INTDIR)/subclass.res"

"$(OUTDIR)\subclass.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "SubClass - Macintosh Release"

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

ALL : "$(OUTDIR)\subclass.exe" "$(OUTDIR)\subclass.trg"

CLEAN : 
	-@erase ".\MacRel\subclass.trg"
	-@erase ".\MacRel\subclass.exe"
	-@erase ".\MacRel\stdwin.obj"
	-@erase ".\MacRel\subclass.obj"
	-@erase ".\MacRel\subclass.pch"
	-@erase ".\MacRel\subclass.rsc"
	-@erase ".\MacRel\scls.rsc"

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
 "WIN32" /D "NDEBUG" /Fp"$(INTDIR)/subclass.pch" /Yu"Stdwin.h" /Fo"$(INTDIR)/"\
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
RSC_PROJ=/l 0x409 /r /m /fo"$(INTDIR)/subclass.rsc" /d "_MAC" /d "_68K_" /d\
 "NDEBUG" 
MRC=mrc.exe
# ADD BASE MRC /D "_68K_" /D "_MAC" /D "NDEBUG" /NOLOGO
# ADD MRC /D "_68K_" /D "_MAC" /D "NDEBUG" /NOLOGO
MRC_PROJ=/D "_68K_" /D "_MAC" /D "NDEBUG" /l 0x409 /NOLOGO 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/subclass.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 wlm.lib /nologo /MAC:bundle /machine:M68K
# ADD LINK32 wlm.lib /nologo /MAC:bundle /MAC:creator="SCLS" /machine:M68K
LINK32_FLAGS=wlm.lib /nologo /MAC:bundle /MAC:type="APPL" /MAC:creator="SCLS"\
 /pdb:"$(OUTDIR)/subclass.pdb" /machine:M68K /out:"$(OUTDIR)/subclass.exe" 
LINK32_OBJS= \
	"$(INTDIR)/stdwin.obj" \
	"$(INTDIR)/subclass.obj" \
	"$(INTDIR)/subclass.rsc" \
	"$(INTDIR)/scls.rsc"

"$(OUTDIR)\subclass.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

MFILE32=mfile.exe
# ADD BASE MFILE32 COPY /NOLOGO
# ADD MFILE32 COPY /NOLOGO
MFILE32_FLAGS=COPY /NOLOGO 
MFILE32_FILES= \
	"$(OUTDIR)/subclass.exe"

"$(OUTDIR)\subclass.trg" : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\MacRel\subclass.exe\
 "$(MFILE32_DEST):subclass.exe">"$(OUTDIR)\subclass.trg"

DOWNLOAD : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\MacRel\subclass.exe\
 "$(MFILE32_DEST):subclass.exe">"$(OUTDIR)\subclass.trg"

!ELSEIF  "$(CFG)" == "SubClass - Macintosh Debug"

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

ALL : "$(OUTDIR)\subclass.exe" "$(OUTDIR)\subclass.trg"

CLEAN : 
	-@erase ".\MacDbg\vc40.pdb"
	-@erase ".\MacDbg\subclass.pch"
	-@erase ".\MacDbg\subclass.pdb"
	-@erase ".\MacDbg\stdwin.obj"
	-@erase ".\MacDbg\subclass.obj"
	-@erase ".\MacDbg\scls.rsc"
	-@erase ".\MacDbg\subclass.rsc"
	-@erase ".\MacDbg\subclass.trg"
	-@erase ".\MacDbg\subclass.exe"

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
 "_68K_" /D "WIN32" /D "_DEBUG" /Fp"$(INTDIR)/subclass.pch" /Yu"Stdwin.h"\
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
RSC_PROJ=/l 0x409 /r /m /fo"$(INTDIR)/subclass.rsc" /d "_MAC" /d "_68K_" /d\
 "_DEBUG" 
MRC=mrc.exe
# ADD BASE MRC /D "_68K_" /D "_MAC" /D "_DEBUG" /NOLOGO
# ADD MRC /D "_68K_" /D "_MAC" /D "_DEBUG" /NOLOGO
MRC_PROJ=/D "_68K_" /D "_MAC" /D "_DEBUG" /l 0x409 /NOLOGO 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/subclass.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 wlm.lib /nologo /MAC:bundle /debug /machine:M68K
# ADD LINK32 wlm.lib /nologo /MAC:bundle /MAC:creator="SCLS" /debug /machine:M68K
LINK32_FLAGS=wlm.lib /nologo /MAC:bundle /MAC:type="APPL" /MAC:creator="SCLS"\
 /pdb:"$(OUTDIR)/subclass.pdb" /debug /machine:M68K\
 /out:"$(OUTDIR)/subclass.exe" 
LINK32_OBJS= \
	"$(INTDIR)/stdwin.obj" \
	"$(INTDIR)/subclass.obj" \
	"$(INTDIR)/scls.rsc" \
	"$(INTDIR)/subclass.rsc"

"$(OUTDIR)\subclass.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

MFILE32=mfile.exe
# ADD BASE MFILE32 COPY /NOLOGO
# ADD MFILE32 COPY /NOLOGO
MFILE32_FLAGS=COPY /NOLOGO 
MFILE32_FILES= \
	"$(OUTDIR)/subclass.exe"

"$(OUTDIR)\subclass.trg" : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\MacDbg\subclass.exe\
 "$(MFILE32_DEST):subclass.exe">"$(OUTDIR)\subclass.trg"

DOWNLOAD : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\MacDbg\subclass.exe\
 "$(MFILE32_DEST):subclass.exe">"$(OUTDIR)\subclass.trg"

!ELSEIF  "$(CFG)" == "SubClass - Power Macintosh Release"

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

ALL : "$(OUTDIR)\subclass.exe" "$(OUTDIR)\subclass.trg"

CLEAN : 
	-@erase ".\PMacRel\subclass.trg"
	-@erase ".\PMacRel\subclass.exe"
	-@erase ".\PMacRel\stdwin.obj"
	-@erase ".\PMacRel\subclass.obj"
	-@erase ".\PMacRel\subclass.pch"
	-@erase ".\PMacRel\subclass.rsc"
	-@erase ".\PMacRel\scls.rsc"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

MTL=mktyplib.exe
# ADD BASE MTL /nologo /D "NDEBUG"
# ADD MTL /nologo /D "NDEBUG"
MTL_PROJ=/nologo /D "NDEBUG" 
CPP=cl.exe
# ADD BASE CPP /nologo /W3 /GX /O2 /D "_MAC" /D "_MPPC_" /D "_WINDOWS" /D "WIN32" /D "NDEBUG" /YX /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "_MAC" /D "_MPPC_" /D "_WINDOWS" /D "WIN32" /D "NDEBUG" /D "_WLMDLL" /Yu"Stdwin.h" /c
CPP_PROJ=/nologo /MD /W3 /GX /O2 /D "_MAC" /D "_MPPC_" /D "_WINDOWS" /D "WIN32"\
 /D "NDEBUG" /D "_WLMDLL" /Fp"$(INTDIR)/subclass.pch" /Yu"Stdwin.h"\
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
RSC_PROJ=/l 0x409 /r /m /fo"$(INTDIR)/subclass.rsc" /d "_MAC" /d "_MPPC_" /d\
 "NDEBUG" 
MRC=mrc.exe
# ADD BASE MRC /D "_MPPC_" /D "_MAC" /D "NDEBUG" /NOLOGO
# ADD MRC /D "_MPPC_" /D "_MAC" /D "NDEBUG" /NOLOGO
MRC_PROJ=/D "_MPPC_" /D "_MAC" /D "NDEBUG" /l 0x409 /NOLOGO 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/subclass.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 cfrg.rsc /nologo /mac:bundle /machine:MPPC
# ADD LINK32 /nologo /mac:bundle /mac:creator="SCLS" /machine:MPPC
# SUBTRACT LINK32 /debug
LINK32_FLAGS=/nologo /mac:bundle /mac:type="APPL" /mac:creator="SCLS"\
 /incremental:no /pdb:"$(OUTDIR)/subclass.pdb" /machine:MPPC\
 /out:"$(OUTDIR)/subclass.exe" 
LINK32_OBJS= \
	"$(INTDIR)/stdwin.obj" \
	"$(INTDIR)/subclass.obj" \
	"$(INTDIR)/subclass.rsc" \
	"$(INTDIR)/scls.rsc"

"$(OUTDIR)\subclass.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

MFILE32=mfile.exe
# ADD BASE MFILE32 COPY /NOLOGO
# ADD MFILE32 COPY /NOLOGO
MFILE32_FLAGS=COPY /NOLOGO 
MFILE32_FILES= \
	"$(OUTDIR)/subclass.exe"

"$(OUTDIR)\subclass.trg" : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\PMacRel\subclass.exe\
 "$(MFILE32_DEST):subclass.exe">"$(OUTDIR)\subclass.trg"

DOWNLOAD : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\PMacRel\subclass.exe\
 "$(MFILE32_DEST):subclass.exe">"$(OUTDIR)\subclass.trg"

!ELSEIF  "$(CFG)" == "SubClass - Power Macintosh Debug"

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

ALL : "$(OUTDIR)\subclass.exe" "$(OUTDIR)\subclass.trg"

CLEAN : 
	-@erase ".\PMacDbg\vc40.pdb"
	-@erase ".\PMacDbg\subclass.pch"
	-@erase ".\PMacDbg\subclass.ilk"
	-@erase ".\PMacDbg\stdwin.obj"
	-@erase ".\PMacDbg\subclass.obj"
	-@erase ".\PMacDbg\scls.rsc"
	-@erase ".\PMacDbg\subclass.rsc"
	-@erase ".\PMacDbg\subclass.pdb"
	-@erase ".\PMacDbg\subclass.trg"
	-@erase ".\PMacDbg\subclass.exe"

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
 "WIN32" /D "_DEBUG" /D "_WLMDLL" /Fp"$(INTDIR)/subclass.pch" /Yu"Stdwin.h"\
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
RSC_PROJ=/l 0x409 /r /m /fo"$(INTDIR)/subclass.rsc" /d "_MAC" /d "_MPPC_" /d\
 "_DEBUG" 
MRC=mrc.exe
# ADD BASE MRC /D "_MPPC_" /D "_MAC" /D "_DEBUG" /NOLOGO
# ADD MRC /D "_MPPC_" /D "_MAC" /D "_DEBUG" /NOLOGO
MRC_PROJ=/D "_MPPC_" /D "_MAC" /D "_DEBUG" /l 0x409 /NOLOGO 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/subclass.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 cfrg.rsc /nologo /mac:bundle /debug /machine:MPPC
# ADD LINK32 /nologo /mac:bundle /mac:creator="SCLS" /debug /machine:MPPC
LINK32_FLAGS=/nologo /mac:bundle /mac:type="APPL" /mac:creator="SCLS"\
 /incremental:yes /pdb:"$(OUTDIR)/subclass.pdb" /debug /machine:MPPC\
 /out:"$(OUTDIR)/subclass.exe" 
LINK32_OBJS= \
	"$(INTDIR)/stdwin.obj" \
	"$(INTDIR)/subclass.obj" \
	"$(INTDIR)/scls.rsc" \
	"$(INTDIR)/subclass.rsc"

"$(OUTDIR)\subclass.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

MFILE32=mfile.exe
# ADD BASE MFILE32 COPY /NOLOGO
# ADD MFILE32 COPY /NOLOGO
MFILE32_FLAGS=COPY /NOLOGO 
MFILE32_FILES= \
	"$(OUTDIR)/subclass.exe"

"$(OUTDIR)\subclass.trg" : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\PMacDbg\subclass.exe\
 "$(MFILE32_DEST):subclass.exe">"$(OUTDIR)\subclass.trg"

DOWNLOAD : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\PMacDbg\subclass.exe\
 "$(MFILE32_DEST):subclass.exe">"$(OUTDIR)\subclass.trg"

!ENDIF 

################################################################################
# Begin Target

# Name "SubClass - Win32 Release"
# Name "SubClass - Win32 Debug"
# Name "SubClass - Macintosh Release"
# Name "SubClass - Macintosh Debug"
# Name "SubClass - Power Macintosh Release"
# Name "SubClass - Power Macintosh Debug"

!IF  "$(CFG)" == "SubClass - Win32 Release"

!ELSEIF  "$(CFG)" == "SubClass - Win32 Debug"

!ELSEIF  "$(CFG)" == "SubClass - Macintosh Release"

!ELSEIF  "$(CFG)" == "SubClass - Macintosh Debug"

!ELSEIF  "$(CFG)" == "SubClass - Power Macintosh Release"

!ELSEIF  "$(CFG)" == "SubClass - Power Macintosh Debug"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\subclass.rc
DEP_RSC_SUBCL=\
	".\res\subclass.ico"\
	".\subclass.h"\
	

!IF  "$(CFG)" == "SubClass - Win32 Release"


"$(INTDIR)\subclass.res" : $(SOURCE) $(DEP_RSC_SUBCL) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "SubClass - Win32 Debug"


"$(INTDIR)\subclass.res" : $(SOURCE) $(DEP_RSC_SUBCL) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "SubClass - Macintosh Release"


"$(INTDIR)\subclass.rsc" : $(SOURCE) $(DEP_RSC_SUBCL) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "SubClass - Macintosh Debug"


"$(INTDIR)\subclass.rsc" : $(SOURCE) $(DEP_RSC_SUBCL) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "SubClass - Power Macintosh Release"


"$(INTDIR)\subclass.rsc" : $(SOURCE) $(DEP_RSC_SUBCL) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "SubClass - Power Macintosh Debug"


"$(INTDIR)\subclass.rsc" : $(SOURCE) $(DEP_RSC_SUBCL) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\scls.r

!IF  "$(CFG)" == "SubClass - Win32 Release"

!ELSEIF  "$(CFG)" == "SubClass - Win32 Debug"

!ELSEIF  "$(CFG)" == "SubClass - Macintosh Release"

DEP_MRC_SCLS_=\
	{$(INCLUDE)}"\mrc\TYPES.R"\
	{$(INCLUDE)}"\mrc\BALLOONS.R"\
	

"$(INTDIR)\scls.rsc" : $(SOURCE) $(DEP_MRC_SCLS_) "$(INTDIR)"
   $(MRC) /o"$(INTDIR)/scls.rsc" /D "_68K_" /D "_MAC" /D "NDEBUG" /l 0x409\
 /NOLOGO $(SOURCE)


!ELSEIF  "$(CFG)" == "SubClass - Macintosh Debug"

DEP_MRC_SCLS_=\
	{$(INCLUDE)}"\mrc\TYPES.R"\
	{$(INCLUDE)}"\mrc\BALLOONS.R"\
	

"$(INTDIR)\scls.rsc" : $(SOURCE) $(DEP_MRC_SCLS_) "$(INTDIR)"
   $(MRC) /o"$(INTDIR)/scls.rsc" /D "_68K_" /D "_MAC" /D "_DEBUG" /l 0x409\
 /NOLOGO $(SOURCE)


!ELSEIF  "$(CFG)" == "SubClass - Power Macintosh Release"

DEP_MRC_SCLS_=\
	{$(INCLUDE)}"\mrc\TYPES.R"\
	{$(INCLUDE)}"\mrc\BALLOONS.R"\
	

"$(INTDIR)\scls.rsc" : $(SOURCE) $(DEP_MRC_SCLS_) "$(INTDIR)"
   $(MRC) /o"$(INTDIR)/scls.rsc" /D "_MPPC_" /D "_MAC" /D "NDEBUG" /l 0x409\
 /NOLOGO $(SOURCE)


!ELSEIF  "$(CFG)" == "SubClass - Power Macintosh Debug"

DEP_MRC_SCLS_=\
	{$(INCLUDE)}"\mrc\TYPES.R"\
	{$(INCLUDE)}"\mrc\BALLOONS.R"\
	

"$(INTDIR)\scls.rsc" : $(SOURCE) $(DEP_MRC_SCLS_) "$(INTDIR)"
   $(MRC) /o"$(INTDIR)/scls.rsc" /D "_MPPC_" /D "_MAC" /D "_DEBUG" /l 0x409\
 /NOLOGO $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\stdwin.c
DEP_CPP_STDWI=\
	".\stdwin.h"\
	

!IF  "$(CFG)" == "SubClass - Win32 Release"

# ADD CPP /Yc

BuildCmds= \
	$(CPP) /nologo /ML /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS"\
 /Fp"$(INTDIR)/subclass.pch" /Yc /Fo"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\stdwin.obj" : $(SOURCE) $(DEP_CPP_STDWI) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\subclass.pch" : $(SOURCE) $(DEP_CPP_STDWI) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "SubClass - Win32 Debug"

# ADD CPP /Yc

BuildCmds= \
	$(CPP) /nologo /MLd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /Fp"$(INTDIR)/subclass.pch" /Yc /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\stdwin.obj" : $(SOURCE) $(DEP_CPP_STDWI) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_STDWI) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\subclass.pch" : $(SOURCE) $(DEP_CPP_STDWI) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "SubClass - Macintosh Release"

# ADD CPP /Yc

BuildCmds= \
	$(CPP) /nologo /AL /Q68s /W3 /GX /O2 /D "_WINDOWS" /D "_MAC" /D "_68K_" /D\
 "WIN32" /D "NDEBUG" /Fp"$(INTDIR)/subclass.pch" /Yc /Fo"$(INTDIR)/" /c\
 $(SOURCE) \
	

"$(INTDIR)\stdwin.obj" : $(SOURCE) $(DEP_CPP_STDWI) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\subclass.pch" : $(SOURCE) $(DEP_CPP_STDWI) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "SubClass - Macintosh Debug"

# ADD CPP /Yc

BuildCmds= \
	$(CPP) /nologo /AL /Q68s /Q68m /W3 /GX /Zi /Od /D "_WINDOWS" /D "_MAC" /D\
 "_68K_" /D "WIN32" /D "_DEBUG" /Fp"$(INTDIR)/subclass.pch" /Yc /Fo"$(INTDIR)/"\
 /Fd"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\stdwin.obj" : $(SOURCE) $(DEP_CPP_STDWI) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_STDWI) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\subclass.pch" : $(SOURCE) $(DEP_CPP_STDWI) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "SubClass - Power Macintosh Release"

# ADD CPP /Yc

BuildCmds= \
	$(CPP) /nologo /MD /W3 /GX /O2 /D "_MAC" /D "_MPPC_" /D "_WINDOWS" /D "WIN32"\
 /D "NDEBUG" /D "_WLMDLL" /Fp"$(INTDIR)/subclass.pch" /Yc /Fo"$(INTDIR)/" /c\
 $(SOURCE) \
	

"$(INTDIR)\stdwin.obj" : $(SOURCE) $(DEP_CPP_STDWI) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\subclass.pch" : $(SOURCE) $(DEP_CPP_STDWI) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "SubClass - Power Macintosh Debug"

# ADD CPP /Yc

BuildCmds= \
	$(CPP) /nologo /MDd /W3 /GX /Zi /Od /D "_MAC" /D "_MPPC_" /D "_WINDOWS" /D\
 "WIN32" /D "_DEBUG" /D "_WLMDLL" /Fp"$(INTDIR)/subclass.pch" /Yc\
 /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\stdwin.obj" : $(SOURCE) $(DEP_CPP_STDWI) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_STDWI) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\subclass.pch" : $(SOURCE) $(DEP_CPP_STDWI) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\subclass.c
DEP_CPP_SUBCLA=\
	".\stdwin.h"\
	".\subclass.h"\
	

!IF  "$(CFG)" == "SubClass - Win32 Release"


"$(INTDIR)\subclass.obj" : $(SOURCE) $(DEP_CPP_SUBCLA) "$(INTDIR)"\
 "$(INTDIR)\subclass.pch"


!ELSEIF  "$(CFG)" == "SubClass - Win32 Debug"


"$(INTDIR)\subclass.obj" : $(SOURCE) $(DEP_CPP_SUBCLA) "$(INTDIR)"\
 "$(INTDIR)\subclass.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_SUBCLA) "$(INTDIR)"\
 "$(INTDIR)\subclass.pch"


!ELSEIF  "$(CFG)" == "SubClass - Macintosh Release"


"$(INTDIR)\subclass.obj" : $(SOURCE) $(DEP_CPP_SUBCLA) "$(INTDIR)"\
 "$(INTDIR)\subclass.pch"


!ELSEIF  "$(CFG)" == "SubClass - Macintosh Debug"


"$(INTDIR)\subclass.obj" : $(SOURCE) $(DEP_CPP_SUBCLA) "$(INTDIR)"\
 "$(INTDIR)\subclass.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_SUBCLA) "$(INTDIR)"\
 "$(INTDIR)\subclass.pch"


!ELSEIF  "$(CFG)" == "SubClass - Power Macintosh Release"


"$(INTDIR)\subclass.obj" : $(SOURCE) $(DEP_CPP_SUBCLA) "$(INTDIR)"\
 "$(INTDIR)\subclass.pch"


!ELSEIF  "$(CFG)" == "SubClass - Power Macintosh Debug"


"$(INTDIR)\subclass.obj" : $(SOURCE) $(DEP_CPP_SUBCLA) "$(INTDIR)"\
 "$(INTDIR)\subclass.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_SUBCLA) "$(INTDIR)"\
 "$(INTDIR)\subclass.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\readme.txt

!IF  "$(CFG)" == "SubClass - Win32 Release"

!ELSEIF  "$(CFG)" == "SubClass - Win32 Debug"

!ELSEIF  "$(CFG)" == "SubClass - Macintosh Release"

!ELSEIF  "$(CFG)" == "SubClass - Macintosh Debug"

!ELSEIF  "$(CFG)" == "SubClass - Power Macintosh Release"

!ELSEIF  "$(CFG)" == "SubClass - Power Macintosh Debug"

!ENDIF 

# End Source File
# End Target
# End Project
################################################################################
