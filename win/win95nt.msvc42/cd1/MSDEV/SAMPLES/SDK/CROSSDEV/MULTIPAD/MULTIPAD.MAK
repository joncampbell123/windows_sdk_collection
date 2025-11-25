# Microsoft Visual C++ Generated NMAKE File, Format Version 40000
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101
# TARGTYPE "Power Macintosh Application" 0x0401
# TARGTYPE "Macintosh Application" 0x0301

!IF "$(CFG)" == ""
CFG=MultiPad - Win32 Debug
!MESSAGE No configuration specified.  Defaulting to MultiPad - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "MultiPad - Win32 Release" && "$(CFG)" !=\
 "MultiPad - Win32 Debug" && "$(CFG)" != "MultiPad - Macintosh Release" &&\
 "$(CFG)" != "MultiPad - Macintosh Debug" && "$(CFG)" !=\
 "MultiPad - Power Macintosh Release" && "$(CFG)" !=\
 "MultiPad - Power Macintosh Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "multipad.mak" CFG="MultiPad - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "MultiPad - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "MultiPad - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE "MultiPad - Macintosh Release" (based on "Macintosh Application")
!MESSAGE "MultiPad - Macintosh Debug" (based on "Macintosh Application")
!MESSAGE "MultiPad - Power Macintosh Release" (based on\
 "Power Macintosh Application")
!MESSAGE "MultiPad - Power Macintosh Debug" (based on\
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
# PROP Target_Last_Scanned "MultiPad - Power Macintosh Debug"

!IF  "$(CFG)" == "MultiPad - Win32 Release"

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

ALL : "$(OUTDIR)\multipad.exe"

CLEAN : 
	-@erase ".\Release\multipad.exe"
	-@erase ".\Release\mpinit.obj"
	-@erase ".\Release\multipad.pch"
	-@erase ".\Release\multipad.obj"
	-@erase ".\Release\mpopen.obj"
	-@erase ".\Release\stdwin.obj"
	-@erase ".\Release\mpfind.obj"
	-@erase ".\Release\mpfile.obj"
	-@erase ".\Release\mpprint.obj"
	-@erase ".\Release\multipad.res"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /Yu"Stdwin.h" /c
CPP_PROJ=/nologo /ML /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS"\
 /Fp"$(INTDIR)/multipad.pch" /Yu"Stdwin.h" /Fo"$(INTDIR)/" /c 
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
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/multipad.res" /d "NDEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/multipad.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib /nologo /subsystem:windows /incremental:no\
 /pdb:"$(OUTDIR)/multipad.pdb" /machine:I386 /out:"$(OUTDIR)/multipad.exe" 
LINK32_OBJS= \
	"$(INTDIR)/mpinit.obj" \
	"$(INTDIR)/multipad.obj" \
	"$(INTDIR)/mpopen.obj" \
	"$(INTDIR)/stdwin.obj" \
	"$(INTDIR)/mpfind.obj" \
	"$(INTDIR)/mpfile.obj" \
	"$(INTDIR)/mpprint.obj" \
	"$(INTDIR)/multipad.res"

"$(OUTDIR)\multipad.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "MultiPad - Win32 Debug"

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

ALL : "$(OUTDIR)\multipad.exe"

CLEAN : 
	-@erase ".\Debug\vc40.pdb"
	-@erase ".\Debug\multipad.pch"
	-@erase ".\Debug\multipad.exe"
	-@erase ".\Debug\mpinit.obj"
	-@erase ".\Debug\mpopen.obj"
	-@erase ".\Debug\stdwin.obj"
	-@erase ".\Debug\multipad.obj"
	-@erase ".\Debug\mpfind.obj"
	-@erase ".\Debug\mpprint.obj"
	-@erase ".\Debug\mpfile.obj"
	-@erase ".\Debug\multipad.res"
	-@erase ".\Debug\multipad.ilk"
	-@erase ".\Debug\multipad.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /Yu"Stdwin.h" /c
CPP_PROJ=/nologo /MLd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /Fp"$(INTDIR)/multipad.pch" /Yu"Stdwin.h" /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
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
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/multipad.res" /d "_DEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/multipad.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib /nologo /subsystem:windows /incremental:yes\
 /pdb:"$(OUTDIR)/multipad.pdb" /debug /machine:I386\
 /out:"$(OUTDIR)/multipad.exe" 
LINK32_OBJS= \
	"$(INTDIR)/mpinit.obj" \
	"$(INTDIR)/mpopen.obj" \
	"$(INTDIR)/stdwin.obj" \
	"$(INTDIR)/multipad.obj" \
	"$(INTDIR)/mpfind.obj" \
	"$(INTDIR)/mpprint.obj" \
	"$(INTDIR)/mpfile.obj" \
	"$(INTDIR)/multipad.res"

"$(OUTDIR)\multipad.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "MultiPad - Macintosh Release"

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

ALL : "$(OUTDIR)\multipad.exe" "$(OUTDIR)\multipad.trg"

CLEAN : 
	-@erase ".\MacRel\multipad.trg"
	-@erase ".\MacRel\multipad.exe"
	-@erase ".\MacRel\mpfind.obj"
	-@erase ".\MacRel\multipad.pch"
	-@erase ".\MacRel\mpfile.obj"
	-@erase ".\MacRel\mpinit.obj"
	-@erase ".\MacRel\mpopen.obj"
	-@erase ".\MacRel\mpprint.obj"
	-@erase ".\MacRel\stdwin.obj"
	-@erase ".\MacRel\multipad.obj"
	-@erase ".\MacRel\multipad.rsc"
	-@erase ".\MacRel\mpad.rsc"

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
 "WIN32" /D "NDEBUG" /Fp"$(INTDIR)/multipad.pch" /Yu"Stdwin.h" /Fo"$(INTDIR)/"\
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
RSC_PROJ=/l 0x409 /r /m /fo"$(INTDIR)/multipad.rsc" /d "_MAC" /d "_68K_" /d\
 "NDEBUG" 
MRC=mrc.exe
# ADD BASE MRC /D "_68K_" /D "_MAC" /D "NDEBUG" /NOLOGO
# ADD MRC /D "_68K_" /D "_MAC" /D "NDEBUG" /NOLOGO
MRC_PROJ=/D "_68K_" /D "_MAC" /D "NDEBUG" /l 0x409 /NOLOGO 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/multipad.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 wlm.lib /nologo /MAC:bundle /machine:M68K
# ADD LINK32 wlm.lib /nologo /MAC:bundle /MAC:creator="MPAD" /machine:M68K
LINK32_FLAGS=wlm.lib /nologo /MAC:bundle /MAC:type="APPL" /MAC:creator="MPAD"\
 /pdb:"$(OUTDIR)/multipad.pdb" /machine:M68K /out:"$(OUTDIR)/multipad.exe" 
LINK32_OBJS= \
	"$(INTDIR)/mpfind.obj" \
	"$(INTDIR)/mpfile.obj" \
	"$(INTDIR)/mpinit.obj" \
	"$(INTDIR)/mpopen.obj" \
	"$(INTDIR)/mpprint.obj" \
	"$(INTDIR)/stdwin.obj" \
	"$(INTDIR)/multipad.obj" \
	"$(INTDIR)/multipad.rsc" \
	"$(INTDIR)/mpad.rsc"

"$(OUTDIR)\multipad.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

MFILE32=mfile.exe
# ADD BASE MFILE32 COPY /NOLOGO
# ADD MFILE32 COPY /NOLOGO
MFILE32_FLAGS=COPY /NOLOGO 
MFILE32_FILES= \
	"$(OUTDIR)/multipad.exe"

"$(OUTDIR)\multipad.trg" : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\MacRel\multipad.exe\
 "$(MFILE32_DEST):multipad.exe">"$(OUTDIR)\multipad.trg"

DOWNLOAD : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\MacRel\multipad.exe\
 "$(MFILE32_DEST):multipad.exe">"$(OUTDIR)\multipad.trg"

!ELSEIF  "$(CFG)" == "MultiPad - Macintosh Debug"

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

ALL : "$(OUTDIR)\multipad.exe" "$(OUTDIR)\multipad.trg"

CLEAN : 
	-@erase ".\MacDbg\vc40.pdb"
	-@erase ".\MacDbg\multipad.pch"
	-@erase ".\MacDbg\multipad.pdb"
	-@erase ".\MacDbg\mpprint.obj"
	-@erase ".\MacDbg\mpfile.obj"
	-@erase ".\MacDbg\mpinit.obj"
	-@erase ".\MacDbg\mpopen.obj"
	-@erase ".\MacDbg\stdwin.obj"
	-@erase ".\MacDbg\multipad.obj"
	-@erase ".\MacDbg\mpfind.obj"
	-@erase ".\MacDbg\mpad.rsc"
	-@erase ".\MacDbg\multipad.rsc"
	-@erase ".\MacDbg\multipad.trg"
	-@erase ".\MacDbg\multipad.exe"

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
 "_68K_" /D "WIN32" /D "_DEBUG" /Fp"$(INTDIR)/multipad.pch" /Yu"Stdwin.h"\
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
RSC_PROJ=/l 0x409 /r /m /fo"$(INTDIR)/multipad.rsc" /d "_MAC" /d "_68K_" /d\
 "_DEBUG" 
MRC=mrc.exe
# ADD BASE MRC /D "_68K_" /D "_MAC" /D "_DEBUG" /NOLOGO
# ADD MRC /D "_68K_" /D "_MAC" /D "_DEBUG" /NOLOGO
MRC_PROJ=/D "_68K_" /D "_MAC" /D "_DEBUG" /l 0x409 /NOLOGO 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/multipad.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 wlm.lib /nologo /MAC:bundle /debug /machine:M68K
# ADD LINK32 wlm.lib /nologo /MAC:bundle /MAC:creator="MPAD" /debug /machine:M68K
LINK32_FLAGS=wlm.lib /nologo /MAC:bundle /MAC:type="APPL" /MAC:creator="MPAD"\
 /pdb:"$(OUTDIR)/multipad.pdb" /debug /machine:M68K\
 /out:"$(OUTDIR)/multipad.exe" 
LINK32_OBJS= \
	"$(INTDIR)/mpprint.obj" \
	"$(INTDIR)/mpfile.obj" \
	"$(INTDIR)/mpinit.obj" \
	"$(INTDIR)/mpopen.obj" \
	"$(INTDIR)/stdwin.obj" \
	"$(INTDIR)/multipad.obj" \
	"$(INTDIR)/mpfind.obj" \
	"$(INTDIR)/mpad.rsc" \
	"$(INTDIR)/multipad.rsc"

"$(OUTDIR)\multipad.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

MFILE32=mfile.exe
# ADD BASE MFILE32 COPY /NOLOGO
# ADD MFILE32 COPY /NOLOGO
MFILE32_FLAGS=COPY /NOLOGO 
MFILE32_FILES= \
	"$(OUTDIR)/multipad.exe"

"$(OUTDIR)\multipad.trg" : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\MacDbg\multipad.exe\
 "$(MFILE32_DEST):multipad.exe">"$(OUTDIR)\multipad.trg"

DOWNLOAD : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\MacDbg\multipad.exe\
 "$(MFILE32_DEST):multipad.exe">"$(OUTDIR)\multipad.trg"

!ELSEIF  "$(CFG)" == "MultiPad - Power Macintosh Release"

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

ALL : "$(OUTDIR)\multipad.exe" "$(OUTDIR)\multipad.trg"

CLEAN : 
	-@erase ".\PMacRel\multipad.trg"
	-@erase ".\PMacRel\multipad.exe"
	-@erase ".\PMacRel\mpfind.obj"
	-@erase ".\PMacRel\multipad.pch"
	-@erase ".\PMacRel\mpfile.obj"
	-@erase ".\PMacRel\mpprint.obj"
	-@erase ".\PMacRel\multipad.obj"
	-@erase ".\PMacRel\mpinit.obj"
	-@erase ".\PMacRel\mpopen.obj"
	-@erase ".\PMacRel\stdwin.obj"
	-@erase ".\PMacRel\mpad.rsc"
	-@erase ".\PMacRel\multipad.rsc"

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
 /D "NDEBUG" /D "_WLMDLL" /Fp"$(INTDIR)/multipad.pch" /Yu"Stdwin.h"\
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
RSC_PROJ=/l 0x409 /r /m /fo"$(INTDIR)/multipad.rsc" /d "_MAC" /d "_MPPC_" /d\
 "NDEBUG" 
MRC=mrc.exe
# ADD BASE MRC /D "_MPPC_" /D "_MAC" /D "NDEBUG" /NOLOGO
# ADD MRC /D "_MPPC_" /D "_MAC" /D "NDEBUG" /NOLOGO
MRC_PROJ=/D "_MPPC_" /D "_MAC" /D "NDEBUG" /l 0x409 /NOLOGO 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/multipad.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 cfrg.rsc /nologo /mac:bundle /machine:MPPC
# ADD LINK32 /nologo /mac:bundle /mac:creator="MPAD" /machine:MPPC
LINK32_FLAGS=/nologo /mac:bundle /mac:type="APPL" /mac:creator="MPAD"\
 /incremental:no /pdb:"$(OUTDIR)/multipad.pdb" /machine:MPPC\
 /out:"$(OUTDIR)/multipad.exe" 
LINK32_OBJS= \
	"$(INTDIR)/mpfind.obj" \
	"$(INTDIR)/mpfile.obj" \
	"$(INTDIR)/mpprint.obj" \
	"$(INTDIR)/multipad.obj" \
	"$(INTDIR)/mpinit.obj" \
	"$(INTDIR)/mpopen.obj" \
	"$(INTDIR)/stdwin.obj" \
	"$(INTDIR)/mpad.rsc" \
	"$(INTDIR)/multipad.rsc"

"$(OUTDIR)\multipad.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

MFILE32=mfile.exe
# ADD BASE MFILE32 COPY /NOLOGO
# ADD MFILE32 COPY /NOLOGO
MFILE32_FLAGS=COPY /NOLOGO 
MFILE32_FILES= \
	"$(OUTDIR)/multipad.exe"

"$(OUTDIR)\multipad.trg" : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\PMacRel\multipad.exe\
 "$(MFILE32_DEST):multipad.exe">"$(OUTDIR)\multipad.trg"

DOWNLOAD : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\PMacRel\multipad.exe\
 "$(MFILE32_DEST):multipad.exe">"$(OUTDIR)\multipad.trg"

!ELSEIF  "$(CFG)" == "MultiPad - Power Macintosh Debug"

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

ALL : "$(OUTDIR)\multipad.exe" "$(OUTDIR)\multipad.trg"

CLEAN : 
	-@erase ".\PMacDbg\vc40.pdb"
	-@erase ".\PMacDbg\multipad.pch"
	-@erase ".\PMacDbg\multipad.ilk"
	-@erase ".\PMacDbg\mpfind.obj"
	-@erase ".\PMacDbg\mpfile.obj"
	-@erase ".\PMacDbg\mpinit.obj"
	-@erase ".\PMacDbg\multipad.obj"
	-@erase ".\PMacDbg\mpopen.obj"
	-@erase ".\PMacDbg\stdwin.obj"
	-@erase ".\PMacDbg\mpprint.obj"
	-@erase ".\PMacDbg\multipad.rsc"
	-@erase ".\PMacDbg\mpad.rsc"
	-@erase ".\PMacDbg\multipad.pdb"
	-@erase ".\PMacDbg\multipad.trg"
	-@erase ".\PMacDbg\multipad.exe"

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
 "WIN32" /D "_DEBUG" /D "_WLMDLL" /Fp"$(INTDIR)/multipad.pch" /Yu"Stdwin.h"\
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
RSC_PROJ=/l 0x409 /r /m /fo"$(INTDIR)/multipad.rsc" /d "_MAC" /d "_MPPC_" /d\
 "_DEBUG" 
MRC=mrc.exe
# ADD BASE MRC /D "_MPPC_" /D "_MAC" /D "_DEBUG" /NOLOGO
# ADD MRC /D "_MPPC_" /D "_MAC" /D "_DEBUG" /NOLOGO
MRC_PROJ=/D "_MPPC_" /D "_MAC" /D "_DEBUG" /l 0x409 /NOLOGO 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/multipad.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 cfrg.rsc /nologo /mac:bundle /debug /machine:MPPC
# ADD LINK32 /nologo /mac:bundle /mac:creator="MPAD" /debug /machine:MPPC
LINK32_FLAGS=/nologo /mac:bundle /mac:type="APPL" /mac:creator="MPAD"\
 /incremental:yes /pdb:"$(OUTDIR)/multipad.pdb" /debug /machine:MPPC\
 /out:"$(OUTDIR)/multipad.exe" 
LINK32_OBJS= \
	"$(INTDIR)/mpfind.obj" \
	"$(INTDIR)/mpfile.obj" \
	"$(INTDIR)/mpinit.obj" \
	"$(INTDIR)/multipad.obj" \
	"$(INTDIR)/mpopen.obj" \
	"$(INTDIR)/stdwin.obj" \
	"$(INTDIR)/mpprint.obj" \
	"$(INTDIR)/multipad.rsc" \
	"$(INTDIR)/mpad.rsc"

"$(OUTDIR)\multipad.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

MFILE32=mfile.exe
# ADD BASE MFILE32 COPY /NOLOGO
# ADD MFILE32 COPY /NOLOGO
MFILE32_FLAGS=COPY /NOLOGO 
MFILE32_FILES= \
	"$(OUTDIR)/multipad.exe"

"$(OUTDIR)\multipad.trg" : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\PMacDbg\multipad.exe\
 "$(MFILE32_DEST):multipad.exe">"$(OUTDIR)\multipad.trg"

DOWNLOAD : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\PMacDbg\multipad.exe\
 "$(MFILE32_DEST):multipad.exe">"$(OUTDIR)\multipad.trg"

!ENDIF 

################################################################################
# Begin Target

# Name "MultiPad - Win32 Release"
# Name "MultiPad - Win32 Debug"
# Name "MultiPad - Macintosh Release"
# Name "MultiPad - Macintosh Debug"
# Name "MultiPad - Power Macintosh Release"
# Name "MultiPad - Power Macintosh Debug"

!IF  "$(CFG)" == "MultiPad - Win32 Release"

!ELSEIF  "$(CFG)" == "MultiPad - Win32 Debug"

!ELSEIF  "$(CFG)" == "MultiPad - Macintosh Release"

!ELSEIF  "$(CFG)" == "MultiPad - Macintosh Debug"

!ELSEIF  "$(CFG)" == "MultiPad - Power Macintosh Release"

!ELSEIF  "$(CFG)" == "MultiPad - Power Macintosh Debug"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\stdwin.c
DEP_CPP_STDWI=\
	".\stdwin.h"\
	

!IF  "$(CFG)" == "MultiPad - Win32 Release"

# ADD CPP /Yc

BuildCmds= \
	$(CPP) /nologo /ML /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS"\
 /Fp"$(INTDIR)/multipad.pch" /Yc /Fo"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\stdwin.obj" : $(SOURCE) $(DEP_CPP_STDWI) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\multipad.pch" : $(SOURCE) $(DEP_CPP_STDWI) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "MultiPad - Win32 Debug"

# ADD CPP /Yc

BuildCmds= \
	$(CPP) /nologo /MLd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /Fp"$(INTDIR)/multipad.pch" /Yc /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\stdwin.obj" : $(SOURCE) $(DEP_CPP_STDWI) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_STDWI) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\multipad.pch" : $(SOURCE) $(DEP_CPP_STDWI) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "MultiPad - Macintosh Release"

# ADD CPP /Yc

BuildCmds= \
	$(CPP) /nologo /AL /Q68s /W3 /GX /O2 /D "_WINDOWS" /D "_MAC" /D "_68K_" /D\
 "WIN32" /D "NDEBUG" /Fp"$(INTDIR)/multipad.pch" /Yc /Fo"$(INTDIR)/" /c\
 $(SOURCE) \
	

"$(INTDIR)\stdwin.obj" : $(SOURCE) $(DEP_CPP_STDWI) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\multipad.pch" : $(SOURCE) $(DEP_CPP_STDWI) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "MultiPad - Macintosh Debug"

# ADD CPP /Yc

BuildCmds= \
	$(CPP) /nologo /AL /Q68s /Q68m /W3 /GX /Zi /Od /D "_WINDOWS" /D "_MAC" /D\
 "_68K_" /D "WIN32" /D "_DEBUG" /Fp"$(INTDIR)/multipad.pch" /Yc /Fo"$(INTDIR)/"\
 /Fd"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\stdwin.obj" : $(SOURCE) $(DEP_CPP_STDWI) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_STDWI) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\multipad.pch" : $(SOURCE) $(DEP_CPP_STDWI) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "MultiPad - Power Macintosh Release"

# ADD CPP /Yc

BuildCmds= \
	$(CPP) /nologo /MD /W3 /GX /O2 /D "_MAC" /D "_MPPC_" /D "_WINDOWS" /D "WIN32"\
 /D "NDEBUG" /D "_WLMDLL" /Fp"$(INTDIR)/multipad.pch" /Yc /Fo"$(INTDIR)/" /c\
 $(SOURCE) \
	

"$(INTDIR)\stdwin.obj" : $(SOURCE) $(DEP_CPP_STDWI) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\multipad.pch" : $(SOURCE) $(DEP_CPP_STDWI) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "MultiPad - Power Macintosh Debug"

# ADD CPP /Yc

BuildCmds= \
	$(CPP) /nologo /MDd /W3 /GX /Zi /Od /D "_MAC" /D "_MPPC_" /D "_WINDOWS" /D\
 "WIN32" /D "_DEBUG" /D "_WLMDLL" /Fp"$(INTDIR)/multipad.pch" /Yc\
 /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\stdwin.obj" : $(SOURCE) $(DEP_CPP_STDWI) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_STDWI) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\multipad.pch" : $(SOURCE) $(DEP_CPP_STDWI) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\mpfile.c

!IF  "$(CFG)" == "MultiPad - Win32 Release"

DEP_CPP_MPFIL=\
	".\multipad.h"\
	".\stdwin.h"\
	

"$(INTDIR)\mpfile.obj" : $(SOURCE) $(DEP_CPP_MPFIL) "$(INTDIR)"\
 "$(INTDIR)\multipad.pch"


!ELSEIF  "$(CFG)" == "MultiPad - Win32 Debug"

DEP_CPP_MPFIL=\
	".\stdwin.h"\
	".\multipad.h"\
	

"$(INTDIR)\mpfile.obj" : $(SOURCE) $(DEP_CPP_MPFIL) "$(INTDIR)"\
 "$(INTDIR)\multipad.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_MPFIL) "$(INTDIR)"\
 "$(INTDIR)\multipad.pch"


!ELSEIF  "$(CFG)" == "MultiPad - Macintosh Release"

DEP_CPP_MPFIL=\
	".\stdwin.h"\
	".\multipad.h"\
	

"$(INTDIR)\mpfile.obj" : $(SOURCE) $(DEP_CPP_MPFIL) "$(INTDIR)"\
 "$(INTDIR)\multipad.pch"


!ELSEIF  "$(CFG)" == "MultiPad - Macintosh Debug"

DEP_CPP_MPFIL=\
	".\stdwin.h"\
	".\multipad.h"\
	

"$(INTDIR)\mpfile.obj" : $(SOURCE) $(DEP_CPP_MPFIL) "$(INTDIR)"\
 "$(INTDIR)\multipad.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_MPFIL) "$(INTDIR)"\
 "$(INTDIR)\multipad.pch"


!ELSEIF  "$(CFG)" == "MultiPad - Power Macintosh Release"

DEP_CPP_MPFIL=\
	".\stdwin.h"\
	".\multipad.h"\
	

"$(INTDIR)\mpfile.obj" : $(SOURCE) $(DEP_CPP_MPFIL) "$(INTDIR)"\
 "$(INTDIR)\multipad.pch"


!ELSEIF  "$(CFG)" == "MultiPad - Power Macintosh Debug"

DEP_CPP_MPFIL=\
	".\stdwin.h"\
	".\multipad.h"\
	

"$(INTDIR)\mpfile.obj" : $(SOURCE) $(DEP_CPP_MPFIL) "$(INTDIR)"\
 "$(INTDIR)\multipad.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_MPFIL) "$(INTDIR)"\
 "$(INTDIR)\multipad.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\mpfind.c
DEP_CPP_MPFIN=\
	".\stdwin.h"\
	".\multipad.h"\
	

!IF  "$(CFG)" == "MultiPad - Win32 Release"


"$(INTDIR)\mpfind.obj" : $(SOURCE) $(DEP_CPP_MPFIN) "$(INTDIR)"\
 "$(INTDIR)\multipad.pch"


!ELSEIF  "$(CFG)" == "MultiPad - Win32 Debug"


"$(INTDIR)\mpfind.obj" : $(SOURCE) $(DEP_CPP_MPFIN) "$(INTDIR)"\
 "$(INTDIR)\multipad.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_MPFIN) "$(INTDIR)"\
 "$(INTDIR)\multipad.pch"


!ELSEIF  "$(CFG)" == "MultiPad - Macintosh Release"


"$(INTDIR)\mpfind.obj" : $(SOURCE) $(DEP_CPP_MPFIN) "$(INTDIR)"\
 "$(INTDIR)\multipad.pch"


!ELSEIF  "$(CFG)" == "MultiPad - Macintosh Debug"


"$(INTDIR)\mpfind.obj" : $(SOURCE) $(DEP_CPP_MPFIN) "$(INTDIR)"\
 "$(INTDIR)\multipad.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_MPFIN) "$(INTDIR)"\
 "$(INTDIR)\multipad.pch"


!ELSEIF  "$(CFG)" == "MultiPad - Power Macintosh Release"


"$(INTDIR)\mpfind.obj" : $(SOURCE) $(DEP_CPP_MPFIN) "$(INTDIR)"\
 "$(INTDIR)\multipad.pch"


!ELSEIF  "$(CFG)" == "MultiPad - Power Macintosh Debug"


"$(INTDIR)\mpfind.obj" : $(SOURCE) $(DEP_CPP_MPFIN) "$(INTDIR)"\
 "$(INTDIR)\multipad.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_MPFIN) "$(INTDIR)"\
 "$(INTDIR)\multipad.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\mpinit.c
DEP_CPP_MPINI=\
	".\stdwin.h"\
	".\multipad.h"\
	

!IF  "$(CFG)" == "MultiPad - Win32 Release"


"$(INTDIR)\mpinit.obj" : $(SOURCE) $(DEP_CPP_MPINI) "$(INTDIR)"\
 "$(INTDIR)\multipad.pch"


!ELSEIF  "$(CFG)" == "MultiPad - Win32 Debug"


"$(INTDIR)\mpinit.obj" : $(SOURCE) $(DEP_CPP_MPINI) "$(INTDIR)"\
 "$(INTDIR)\multipad.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_MPINI) "$(INTDIR)"\
 "$(INTDIR)\multipad.pch"


!ELSEIF  "$(CFG)" == "MultiPad - Macintosh Release"


"$(INTDIR)\mpinit.obj" : $(SOURCE) $(DEP_CPP_MPINI) "$(INTDIR)"\
 "$(INTDIR)\multipad.pch"


!ELSEIF  "$(CFG)" == "MultiPad - Macintosh Debug"


"$(INTDIR)\mpinit.obj" : $(SOURCE) $(DEP_CPP_MPINI) "$(INTDIR)"\
 "$(INTDIR)\multipad.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_MPINI) "$(INTDIR)"\
 "$(INTDIR)\multipad.pch"


!ELSEIF  "$(CFG)" == "MultiPad - Power Macintosh Release"


"$(INTDIR)\mpinit.obj" : $(SOURCE) $(DEP_CPP_MPINI) "$(INTDIR)"\
 "$(INTDIR)\multipad.pch"


!ELSEIF  "$(CFG)" == "MultiPad - Power Macintosh Debug"


"$(INTDIR)\mpinit.obj" : $(SOURCE) $(DEP_CPP_MPINI) "$(INTDIR)"\
 "$(INTDIR)\multipad.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_MPINI) "$(INTDIR)"\
 "$(INTDIR)\multipad.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\mpopen.c
DEP_CPP_MPOPE=\
	".\stdwin.h"\
	".\multipad.h"\
	

!IF  "$(CFG)" == "MultiPad - Win32 Release"


"$(INTDIR)\mpopen.obj" : $(SOURCE) $(DEP_CPP_MPOPE) "$(INTDIR)"\
 "$(INTDIR)\multipad.pch"


!ELSEIF  "$(CFG)" == "MultiPad - Win32 Debug"


"$(INTDIR)\mpopen.obj" : $(SOURCE) $(DEP_CPP_MPOPE) "$(INTDIR)"\
 "$(INTDIR)\multipad.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_MPOPE) "$(INTDIR)"\
 "$(INTDIR)\multipad.pch"


!ELSEIF  "$(CFG)" == "MultiPad - Macintosh Release"


"$(INTDIR)\mpopen.obj" : $(SOURCE) $(DEP_CPP_MPOPE) "$(INTDIR)"\
 "$(INTDIR)\multipad.pch"


!ELSEIF  "$(CFG)" == "MultiPad - Macintosh Debug"


"$(INTDIR)\mpopen.obj" : $(SOURCE) $(DEP_CPP_MPOPE) "$(INTDIR)"\
 "$(INTDIR)\multipad.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_MPOPE) "$(INTDIR)"\
 "$(INTDIR)\multipad.pch"


!ELSEIF  "$(CFG)" == "MultiPad - Power Macintosh Release"


"$(INTDIR)\mpopen.obj" : $(SOURCE) $(DEP_CPP_MPOPE) "$(INTDIR)"\
 "$(INTDIR)\multipad.pch"


!ELSEIF  "$(CFG)" == "MultiPad - Power Macintosh Debug"


"$(INTDIR)\mpopen.obj" : $(SOURCE) $(DEP_CPP_MPOPE) "$(INTDIR)"\
 "$(INTDIR)\multipad.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_MPOPE) "$(INTDIR)"\
 "$(INTDIR)\multipad.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\mpprint.c
DEP_CPP_MPPRI=\
	".\stdwin.h"\
	".\multipad.h"\
	

!IF  "$(CFG)" == "MultiPad - Win32 Release"


"$(INTDIR)\mpprint.obj" : $(SOURCE) $(DEP_CPP_MPPRI) "$(INTDIR)"\
 "$(INTDIR)\multipad.pch"


!ELSEIF  "$(CFG)" == "MultiPad - Win32 Debug"


"$(INTDIR)\mpprint.obj" : $(SOURCE) $(DEP_CPP_MPPRI) "$(INTDIR)"\
 "$(INTDIR)\multipad.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_MPPRI) "$(INTDIR)"\
 "$(INTDIR)\multipad.pch"


!ELSEIF  "$(CFG)" == "MultiPad - Macintosh Release"


"$(INTDIR)\mpprint.obj" : $(SOURCE) $(DEP_CPP_MPPRI) "$(INTDIR)"\
 "$(INTDIR)\multipad.pch"


!ELSEIF  "$(CFG)" == "MultiPad - Macintosh Debug"


"$(INTDIR)\mpprint.obj" : $(SOURCE) $(DEP_CPP_MPPRI) "$(INTDIR)"\
 "$(INTDIR)\multipad.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_MPPRI) "$(INTDIR)"\
 "$(INTDIR)\multipad.pch"


!ELSEIF  "$(CFG)" == "MultiPad - Power Macintosh Release"


"$(INTDIR)\mpprint.obj" : $(SOURCE) $(DEP_CPP_MPPRI) "$(INTDIR)"\
 "$(INTDIR)\multipad.pch"


!ELSEIF  "$(CFG)" == "MultiPad - Power Macintosh Debug"


"$(INTDIR)\mpprint.obj" : $(SOURCE) $(DEP_CPP_MPPRI) "$(INTDIR)"\
 "$(INTDIR)\multipad.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_MPPRI) "$(INTDIR)"\
 "$(INTDIR)\multipad.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\multipad.c
DEP_CPP_MULTI=\
	".\stdwin.h"\
	".\multipad.h"\
	

!IF  "$(CFG)" == "MultiPad - Win32 Release"


"$(INTDIR)\multipad.obj" : $(SOURCE) $(DEP_CPP_MULTI) "$(INTDIR)"\
 "$(INTDIR)\multipad.pch"


!ELSEIF  "$(CFG)" == "MultiPad - Win32 Debug"


"$(INTDIR)\multipad.obj" : $(SOURCE) $(DEP_CPP_MULTI) "$(INTDIR)"\
 "$(INTDIR)\multipad.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_MULTI) "$(INTDIR)"\
 "$(INTDIR)\multipad.pch"


!ELSEIF  "$(CFG)" == "MultiPad - Macintosh Release"


"$(INTDIR)\multipad.obj" : $(SOURCE) $(DEP_CPP_MULTI) "$(INTDIR)"\
 "$(INTDIR)\multipad.pch"


!ELSEIF  "$(CFG)" == "MultiPad - Macintosh Debug"


"$(INTDIR)\multipad.obj" : $(SOURCE) $(DEP_CPP_MULTI) "$(INTDIR)"\
 "$(INTDIR)\multipad.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_MULTI) "$(INTDIR)"\
 "$(INTDIR)\multipad.pch"


!ELSEIF  "$(CFG)" == "MultiPad - Power Macintosh Release"


"$(INTDIR)\multipad.obj" : $(SOURCE) $(DEP_CPP_MULTI) "$(INTDIR)"\
 "$(INTDIR)\multipad.pch"


!ELSEIF  "$(CFG)" == "MultiPad - Power Macintosh Debug"


"$(INTDIR)\multipad.obj" : $(SOURCE) $(DEP_CPP_MULTI) "$(INTDIR)"\
 "$(INTDIR)\multipad.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_MULTI) "$(INTDIR)"\
 "$(INTDIR)\multipad.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\multipad.rc
DEP_RSC_MULTIP=\
	".\res\mp300.ico"\
	".\res\note300.ico"\
	".\multipad.h"\
	

!IF  "$(CFG)" == "MultiPad - Win32 Release"


"$(INTDIR)\multipad.res" : $(SOURCE) $(DEP_RSC_MULTIP) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "MultiPad - Win32 Debug"


"$(INTDIR)\multipad.res" : $(SOURCE) $(DEP_RSC_MULTIP) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "MultiPad - Macintosh Release"


"$(INTDIR)\multipad.rsc" : $(SOURCE) $(DEP_RSC_MULTIP) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "MultiPad - Macintosh Debug"


"$(INTDIR)\multipad.rsc" : $(SOURCE) $(DEP_RSC_MULTIP) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "MultiPad - Power Macintosh Release"


"$(INTDIR)\multipad.rsc" : $(SOURCE) $(DEP_RSC_MULTIP) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "MultiPad - Power Macintosh Debug"


"$(INTDIR)\multipad.rsc" : $(SOURCE) $(DEP_RSC_MULTIP) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\mpad.r

!IF  "$(CFG)" == "MultiPad - Win32 Release"

!ELSEIF  "$(CFG)" == "MultiPad - Win32 Debug"

!ELSEIF  "$(CFG)" == "MultiPad - Macintosh Release"

DEP_MRC_MPAD_=\
	{$(INCLUDE)}"\mrc\TYPES.R"\
	{$(INCLUDE)}"\mrc\BALLOONS.R"\
	

"$(INTDIR)\mpad.rsc" : $(SOURCE) $(DEP_MRC_MPAD_) "$(INTDIR)"
   $(MRC) /o"$(INTDIR)/mpad.rsc" /D "_68K_" /D "_MAC" /D "NDEBUG" /l 0x409\
 /NOLOGO $(SOURCE)


!ELSEIF  "$(CFG)" == "MultiPad - Macintosh Debug"

DEP_MRC_MPAD_=\
	{$(INCLUDE)}"\mrc\TYPES.R"\
	{$(INCLUDE)}"\mrc\BALLOONS.R"\
	

"$(INTDIR)\mpad.rsc" : $(SOURCE) $(DEP_MRC_MPAD_) "$(INTDIR)"
   $(MRC) /o"$(INTDIR)/mpad.rsc" /D "_68K_" /D "_MAC" /D "_DEBUG" /l 0x409\
 /NOLOGO $(SOURCE)


!ELSEIF  "$(CFG)" == "MultiPad - Power Macintosh Release"

DEP_MRC_MPAD_=\
	{$(INCLUDE)}"\mrc\TYPES.R"\
	{$(INCLUDE)}"\mrc\BALLOONS.R"\
	

"$(INTDIR)\mpad.rsc" : $(SOURCE) $(DEP_MRC_MPAD_) "$(INTDIR)"
   $(MRC) /o"$(INTDIR)/mpad.rsc" /D "_MPPC_" /D "_MAC" /D "NDEBUG" /l 0x409\
 /NOLOGO $(SOURCE)


!ELSEIF  "$(CFG)" == "MultiPad - Power Macintosh Debug"

DEP_MRC_MPAD_=\
	{$(INCLUDE)}"\mrc\TYPES.R"\
	{$(INCLUDE)}"\mrc\BALLOONS.R"\
	

"$(INTDIR)\mpad.rsc" : $(SOURCE) $(DEP_MRC_MPAD_) "$(INTDIR)"
   $(MRC) /o"$(INTDIR)/mpad.rsc" /D "_MPPC_" /D "_MAC" /D "_DEBUG" /l 0x409\
 /NOLOGO $(SOURCE)


!ENDIF 

# End Source File
# End Target
# End Project
################################################################################
