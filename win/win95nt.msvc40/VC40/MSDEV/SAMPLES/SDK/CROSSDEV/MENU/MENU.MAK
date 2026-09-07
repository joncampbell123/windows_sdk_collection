# Microsoft Visual C++ Generated NMAKE File, Format Version 40000
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101
# TARGTYPE "Power Macintosh Application" 0x0401
# TARGTYPE "Macintosh Application" 0x0301

!IF "$(CFG)" == ""
CFG=Menu - Win32 Debug
!MESSAGE No configuration specified.  Defaulting to Menu - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "Menu - Win32 Release" && "$(CFG)" != "Menu - Win32 Debug" &&\
 "$(CFG)" != "Menu - Macintosh Release" && "$(CFG)" != "Menu - Macintosh Debug"\
 && "$(CFG)" != "Menu - Power Macintosh Release" && "$(CFG)" !=\
 "Menu - Power Macintosh Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "menu.mak" CFG="Menu - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Menu - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "Menu - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE "Menu - Macintosh Release" (based on "Macintosh Application")
!MESSAGE "Menu - Macintosh Debug" (based on "Macintosh Application")
!MESSAGE "Menu - Power Macintosh Release" (based on\
 "Power Macintosh Application")
!MESSAGE "Menu - Power Macintosh Debug" (based on\
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
# PROP Target_Last_Scanned "Menu - Power Macintosh Debug"

!IF  "$(CFG)" == "Menu - Win32 Release"

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

ALL : "$(OUTDIR)\menu.exe"

CLEAN : 
	-@erase ".\Release\menu.exe"
	-@erase ".\Release\menu.obj"
	-@erase ".\Release\menu.pch"
	-@erase ".\Release\stdwin.obj"
	-@erase ".\Release\menu.res"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /Yu"Stdwin.h" /c
CPP_PROJ=/nologo /ML /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS"\
 /Fp"$(INTDIR)/menu.pch" /Yu"Stdwin.h" /Fo"$(INTDIR)/" /c 
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
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/menu.res" /d "NDEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/menu.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib /nologo /subsystem:windows /incremental:no\
 /pdb:"$(OUTDIR)/menu.pdb" /machine:I386 /out:"$(OUTDIR)/menu.exe" 
LINK32_OBJS= \
	"$(INTDIR)/menu.obj" \
	"$(INTDIR)/stdwin.obj" \
	"$(INTDIR)/menu.res"

"$(OUTDIR)\menu.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "Menu - Win32 Debug"

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

ALL : "$(OUTDIR)\menu.exe"

CLEAN : 
	-@erase ".\Debug\vc40.pdb"
	-@erase ".\Debug\menu.pch"
	-@erase ".\Debug\menu.exe"
	-@erase ".\Debug\menu.obj"
	-@erase ".\Debug\stdwin.obj"
	-@erase ".\Debug\menu.res"
	-@erase ".\Debug\menu.ilk"
	-@erase ".\Debug\menu.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /Yu"Stdwin.h" /c
CPP_PROJ=/nologo /MLd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /Fp"$(INTDIR)/menu.pch" /Yu"Stdwin.h" /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
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
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/menu.res" /d "_DEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/menu.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib /nologo /subsystem:windows /incremental:yes\
 /pdb:"$(OUTDIR)/menu.pdb" /debug /machine:I386 /out:"$(OUTDIR)/menu.exe" 
LINK32_OBJS= \
	"$(INTDIR)/menu.obj" \
	"$(INTDIR)/stdwin.obj" \
	"$(INTDIR)/menu.res"

"$(OUTDIR)\menu.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "Menu - Macintosh Release"

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

ALL : "$(OUTDIR)\menu.exe" "$(OUTDIR)\menu.trg"

CLEAN : 
	-@erase ".\MacRel\menu.trg"
	-@erase ".\MacRel\menu.exe"
	-@erase ".\MacRel\stdwin.obj"
	-@erase ".\MacRel\menu.obj"
	-@erase ".\MacRel\menu.pch"
	-@erase ".\MacRel\smnu.rsc"
	-@erase ".\MacRel\menu.rsc"

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
 "WIN32" /D "NDEBUG" /Fp"$(INTDIR)/menu.pch" /Yu"Stdwin.h" /Fo"$(INTDIR)/" /c 
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
RSC_PROJ=/l 0x409 /r /m /fo"$(INTDIR)/menu.rsc" /d "_MAC" /d "_68K_" /d\
 "NDEBUG" 
MRC=mrc.exe
# ADD BASE MRC /D "_68K_" /D "_MAC" /D "NDEBUG" /NOLOGO
# ADD MRC /D "_68K_" /D "_MAC" /D "NDEBUG" /NOLOGO
MRC_PROJ=/D "_68K_" /D "_MAC" /D "NDEBUG" /l 0x409 /NOLOGO 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/menu.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 wlm.lib /nologo /MAC:bundle /machine:M68K
# ADD LINK32 wlm.lib /nologo /MAC:bundle /MAC:creator="SMNU" /machine:M68K
# SUBTRACT LINK32 /debug
LINK32_FLAGS=wlm.lib /nologo /MAC:bundle /MAC:type="APPL" /MAC:creator="SMNU"\
 /pdb:"$(OUTDIR)/menu.pdb" /machine:M68K /out:"$(OUTDIR)/menu.exe" 
LINK32_OBJS= \
	"$(INTDIR)/stdwin.obj" \
	"$(INTDIR)/menu.obj" \
	"$(INTDIR)/smnu.rsc" \
	"$(INTDIR)/menu.rsc"

"$(OUTDIR)\menu.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

MFILE32=mfile.exe
# ADD BASE MFILE32 COPY /NOLOGO
# ADD MFILE32 COPY /NOLOGO
MFILE32_FLAGS=COPY /NOLOGO 
MFILE32_FILES= \
	"$(OUTDIR)/menu.exe"

"$(OUTDIR)\menu.trg" : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\MacRel\menu.exe\
 "$(MFILE32_DEST):menu.exe">"$(OUTDIR)\menu.trg"

DOWNLOAD : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\MacRel\menu.exe\
 "$(MFILE32_DEST):menu.exe">"$(OUTDIR)\menu.trg"

!ELSEIF  "$(CFG)" == "Menu - Macintosh Debug"

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

ALL : "$(OUTDIR)\menu.exe" "$(OUTDIR)\menu.trg"

CLEAN : 
	-@erase ".\MacDbg\vc40.pdb"
	-@erase ".\MacDbg\menu.pch"
	-@erase ".\MacDbg\menu.pdb"
	-@erase ".\MacDbg\stdwin.obj"
	-@erase ".\MacDbg\menu.obj"
	-@erase ".\MacDbg\menu.rsc"
	-@erase ".\MacDbg\smnu.rsc"
	-@erase ".\MacDbg\menu.trg"
	-@erase ".\MacDbg\menu.exe"

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
 "_68K_" /D "WIN32" /D "_DEBUG" /Fp"$(INTDIR)/menu.pch" /Yu"Stdwin.h"\
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
RSC_PROJ=/l 0x409 /r /m /fo"$(INTDIR)/menu.rsc" /d "_MAC" /d "_68K_" /d\
 "_DEBUG" 
MRC=mrc.exe
# ADD BASE MRC /D "_68K_" /D "_MAC" /D "_DEBUG" /NOLOGO
# ADD MRC /D "_68K_" /D "_MAC" /D "_DEBUG" /NOLOGO
MRC_PROJ=/D "_68K_" /D "_MAC" /D "_DEBUG" /l 0x409 /NOLOGO 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/menu.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 wlm.lib /nologo /MAC:bundle /debug /machine:M68K
# ADD LINK32 wlm.lib /nologo /MAC:bundle /MAC:creator="SMNU" /debug /machine:M68K
LINK32_FLAGS=wlm.lib /nologo /MAC:bundle /MAC:type="APPL" /MAC:creator="SMNU"\
 /pdb:"$(OUTDIR)/menu.pdb" /debug /machine:M68K /out:"$(OUTDIR)/menu.exe" 
LINK32_OBJS= \
	"$(INTDIR)/stdwin.obj" \
	"$(INTDIR)/menu.obj" \
	"$(INTDIR)/menu.rsc" \
	"$(INTDIR)/smnu.rsc"

"$(OUTDIR)\menu.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

MFILE32=mfile.exe
# ADD BASE MFILE32 COPY /NOLOGO
# ADD MFILE32 COPY /NOLOGO
MFILE32_FLAGS=COPY /NOLOGO 
MFILE32_FILES= \
	"$(OUTDIR)/menu.exe"

"$(OUTDIR)\menu.trg" : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\MacDbg\menu.exe\
 "$(MFILE32_DEST):menu.exe">"$(OUTDIR)\menu.trg"

DOWNLOAD : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\MacDbg\menu.exe\
 "$(MFILE32_DEST):menu.exe">"$(OUTDIR)\menu.trg"

!ELSEIF  "$(CFG)" == "Menu - Power Macintosh Release"

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

ALL : "$(OUTDIR)\menu.exe" "$(OUTDIR)\menu.trg"

CLEAN : 
	-@erase ".\PMacRel\menu.trg"
	-@erase ".\PMacRel\menu.exe"
	-@erase ".\PMacRel\stdwin.obj"
	-@erase ".\PMacRel\menu.obj"
	-@erase ".\PMacRel\menu.pch"
	-@erase ".\PMacRel\smnu.rsc"
	-@erase ".\PMacRel\menu.rsc"

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
 /D "NDEBUG" /D "_WLMDLL" /Fp"$(INTDIR)/menu.pch" /Yu"Stdwin.h" /Fo"$(INTDIR)/"\
 /c 
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
RSC_PROJ=/l 0x409 /r /m /fo"$(INTDIR)/menu.rsc" /d "_MAC" /d "_MPPC_" /d\
 "NDEBUG" 
MRC=mrc.exe
# ADD BASE MRC /D "_MPPC_" /D "_MAC" /D "NDEBUG" /NOLOGO
# ADD MRC /D "_MPPC_" /D "_MAC" /D "NDEBUG" /NOLOGO
MRC_PROJ=/D "_MPPC_" /D "_MAC" /D "NDEBUG" /l 0x409 /NOLOGO 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/menu.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 /nologo /mac:bundle /machine:MPPC
# ADD LINK32 /nologo /mac:bundle /mac:creator="SMNU" /machine:MPPC
LINK32_FLAGS=/nologo /mac:bundle /mac:type="APPL" /mac:creator="SMNU"\
 /incremental:no /pdb:"$(OUTDIR)/menu.pdb" /machine:MPPC\
 /out:"$(OUTDIR)/menu.exe" 
LINK32_OBJS= \
	"$(INTDIR)/stdwin.obj" \
	"$(INTDIR)/menu.obj" \
	"$(INTDIR)/smnu.rsc" \
	"$(INTDIR)/menu.rsc"

"$(OUTDIR)\menu.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

MFILE32=mfile.exe
# ADD BASE MFILE32 COPY /NOLOGO
# ADD MFILE32 COPY /NOLOGO
MFILE32_FLAGS=COPY /NOLOGO 
MFILE32_FILES= \
	"$(OUTDIR)/menu.exe"

"$(OUTDIR)\menu.trg" : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\PMacRel\menu.exe\
 "$(MFILE32_DEST):menu.exe">"$(OUTDIR)\menu.trg"

DOWNLOAD : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\PMacRel\menu.exe\
 "$(MFILE32_DEST):menu.exe">"$(OUTDIR)\menu.trg"

!ELSEIF  "$(CFG)" == "Menu - Power Macintosh Debug"

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

ALL : "$(OUTDIR)\menu.exe" "$(OUTDIR)\menu.trg"

CLEAN : 
	-@erase ".\PMacDbg\vc40.pdb"
	-@erase ".\PMacDbg\menu.pch"
	-@erase ".\PMacDbg\menu.ilk"
	-@erase ".\PMacDbg\menu.obj"
	-@erase ".\PMacDbg\stdwin.obj"
	-@erase ".\PMacDbg\menu.rsc"
	-@erase ".\PMacDbg\smnu.rsc"
	-@erase ".\PMacDbg\menu.pdb"
	-@erase ".\PMacDbg\menu.trg"
	-@erase ".\PMacDbg\menu.exe"

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
 "WIN32" /D "_DEBUG" /D "_WLMDLL" /Fp"$(INTDIR)/menu.pch" /Yu"Stdwin.h"\
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
RSC_PROJ=/l 0x409 /r /m /fo"$(INTDIR)/menu.rsc" /d "_MAC" /d "_MPPC_" /d\
 "_DEBUG" 
MRC=mrc.exe
# ADD BASE MRC /D "_MPPC_" /D "_MAC" /D "_DEBUG" /NOLOGO
# ADD MRC /D "_MPPC_" /D "_MAC" /D "_DEBUG" /NOLOGO
MRC_PROJ=/D "_MPPC_" /D "_MAC" /D "_DEBUG" /l 0x409 /NOLOGO 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/menu.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 /nologo /mac:bundle /debug /machine:MPPC
# ADD LINK32 /nologo /mac:bundle /mac:creator="SMNU" /debug /machine:MPPC
LINK32_FLAGS=/nologo /mac:bundle /mac:type="APPL" /mac:creator="SMNU"\
 /incremental:yes /pdb:"$(OUTDIR)/menu.pdb" /debug /machine:MPPC\
 /out:"$(OUTDIR)/menu.exe" 
LINK32_OBJS= \
	"$(INTDIR)/menu.obj" \
	"$(INTDIR)/stdwin.obj" \
	"$(INTDIR)/menu.rsc" \
	"$(INTDIR)/smnu.rsc"

"$(OUTDIR)\menu.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

MFILE32=mfile.exe
# ADD BASE MFILE32 COPY /NOLOGO
# ADD MFILE32 COPY /NOLOGO
MFILE32_FLAGS=COPY /NOLOGO 
MFILE32_FILES= \
	"$(OUTDIR)/menu.exe"

"$(OUTDIR)\menu.trg" : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\PMacDbg\menu.exe\
 "$(MFILE32_DEST):menu.exe">"$(OUTDIR)\menu.trg"

DOWNLOAD : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\PMacDbg\menu.exe\
 "$(MFILE32_DEST):menu.exe">"$(OUTDIR)\menu.trg"

!ENDIF 

################################################################################
# Begin Target

# Name "Menu - Win32 Release"
# Name "Menu - Win32 Debug"
# Name "Menu - Macintosh Release"
# Name "Menu - Macintosh Debug"
# Name "Menu - Power Macintosh Release"
# Name "Menu - Power Macintosh Debug"

!IF  "$(CFG)" == "Menu - Win32 Release"

!ELSEIF  "$(CFG)" == "Menu - Win32 Debug"

!ELSEIF  "$(CFG)" == "Menu - Macintosh Release"

!ELSEIF  "$(CFG)" == "Menu - Macintosh Debug"

!ELSEIF  "$(CFG)" == "Menu - Power Macintosh Release"

!ELSEIF  "$(CFG)" == "Menu - Power Macintosh Debug"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\stdwin.c
DEP_CPP_STDWI=\
	".\stdwin.h"\
	

!IF  "$(CFG)" == "Menu - Win32 Release"

# ADD CPP /Yc

BuildCmds= \
	$(CPP) /nologo /ML /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS"\
 /Fp"$(INTDIR)/menu.pch" /Yc /Fo"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\stdwin.obj" : $(SOURCE) $(DEP_CPP_STDWI) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\menu.pch" : $(SOURCE) $(DEP_CPP_STDWI) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "Menu - Win32 Debug"

# ADD CPP /Yc"Stdwin.h"

BuildCmds= \
	$(CPP) /nologo /MLd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /Fp"$(INTDIR)/menu.pch" /Yc"Stdwin.h" /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c\
 $(SOURCE) \
	

"$(INTDIR)\stdwin.obj" : $(SOURCE) $(DEP_CPP_STDWI) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_STDWI) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\menu.pch" : $(SOURCE) $(DEP_CPP_STDWI) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "Menu - Macintosh Release"

# ADD CPP /Yc"Stdwin.h"

BuildCmds= \
	$(CPP) /nologo /AL /Q68s /W3 /GX /O2 /D "_WINDOWS" /D "_MAC" /D "_68K_" /D\
 "WIN32" /D "NDEBUG" /Fp"$(INTDIR)/menu.pch" /Yc"Stdwin.h" /Fo"$(INTDIR)/" /c\
 $(SOURCE) \
	

"$(INTDIR)\stdwin.obj" : $(SOURCE) $(DEP_CPP_STDWI) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\menu.pch" : $(SOURCE) $(DEP_CPP_STDWI) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "Menu - Macintosh Debug"

# ADD CPP /Yc"Stdwin.h"

BuildCmds= \
	$(CPP) /nologo /AL /Q68s /Q68m /W3 /GX /Zi /Od /D "_WINDOWS" /D "_MAC" /D\
 "_68K_" /D "WIN32" /D "_DEBUG" /Fp"$(INTDIR)/menu.pch" /Yc"Stdwin.h"\
 /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\stdwin.obj" : $(SOURCE) $(DEP_CPP_STDWI) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_STDWI) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\menu.pch" : $(SOURCE) $(DEP_CPP_STDWI) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "Menu - Power Macintosh Release"

# ADD CPP /Yc"Stdwin.h"

BuildCmds= \
	$(CPP) /nologo /MD /W3 /GX /O2 /D "_MAC" /D "_MPPC_" /D "_WINDOWS" /D "WIN32"\
 /D "NDEBUG" /D "_WLMDLL" /Fp"$(INTDIR)/menu.pch" /Yc"Stdwin.h" /Fo"$(INTDIR)/"\
 /c $(SOURCE) \
	

"$(INTDIR)\stdwin.obj" : $(SOURCE) $(DEP_CPP_STDWI) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\menu.pch" : $(SOURCE) $(DEP_CPP_STDWI) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "Menu - Power Macintosh Debug"

# ADD CPP /Yc"Stdwin.h"

BuildCmds= \
	$(CPP) /nologo /MDd /W3 /GX /Zi /Od /D "_MAC" /D "_MPPC_" /D "_WINDOWS" /D\
 "WIN32" /D "_DEBUG" /D "_WLMDLL" /Fp"$(INTDIR)/menu.pch" /Yc"Stdwin.h"\
 /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\stdwin.obj" : $(SOURCE) $(DEP_CPP_STDWI) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_STDWI) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\menu.pch" : $(SOURCE) $(DEP_CPP_STDWI) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\menu.rc
DEP_RSC_MENU_=\
	".\res\checkon.bmp"\
	".\res\checkoff.bmp"\
	".\res\menu.ico"\
	".\menu.h"\
	

!IF  "$(CFG)" == "Menu - Win32 Release"


"$(INTDIR)\menu.res" : $(SOURCE) $(DEP_RSC_MENU_) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Menu - Win32 Debug"


"$(INTDIR)\menu.res" : $(SOURCE) $(DEP_RSC_MENU_) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Menu - Macintosh Release"


"$(INTDIR)\menu.rsc" : $(SOURCE) $(DEP_RSC_MENU_) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Menu - Macintosh Debug"


"$(INTDIR)\menu.rsc" : $(SOURCE) $(DEP_RSC_MENU_) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Menu - Power Macintosh Release"


"$(INTDIR)\menu.rsc" : $(SOURCE) $(DEP_RSC_MENU_) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Menu - Power Macintosh Debug"


"$(INTDIR)\menu.rsc" : $(SOURCE) $(DEP_RSC_MENU_) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\menu.c
DEP_CPP_MENU_C=\
	".\stdwin.h"\
	".\menu.h"\
	

!IF  "$(CFG)" == "Menu - Win32 Release"


"$(INTDIR)\menu.obj" : $(SOURCE) $(DEP_CPP_MENU_C) "$(INTDIR)"\
 "$(INTDIR)\menu.pch"


!ELSEIF  "$(CFG)" == "Menu - Win32 Debug"


"$(INTDIR)\menu.obj" : $(SOURCE) $(DEP_CPP_MENU_C) "$(INTDIR)"\
 "$(INTDIR)\menu.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_MENU_C) "$(INTDIR)"\
 "$(INTDIR)\menu.pch"


!ELSEIF  "$(CFG)" == "Menu - Macintosh Release"


"$(INTDIR)\menu.obj" : $(SOURCE) $(DEP_CPP_MENU_C) "$(INTDIR)"\
 "$(INTDIR)\menu.pch"


!ELSEIF  "$(CFG)" == "Menu - Macintosh Debug"


"$(INTDIR)\menu.obj" : $(SOURCE) $(DEP_CPP_MENU_C) "$(INTDIR)"\
 "$(INTDIR)\menu.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_MENU_C) "$(INTDIR)"\
 "$(INTDIR)\menu.pch"


!ELSEIF  "$(CFG)" == "Menu - Power Macintosh Release"


"$(INTDIR)\menu.obj" : $(SOURCE) $(DEP_CPP_MENU_C) "$(INTDIR)"\
 "$(INTDIR)\menu.pch"


!ELSEIF  "$(CFG)" == "Menu - Power Macintosh Debug"


"$(INTDIR)\menu.obj" : $(SOURCE) $(DEP_CPP_MENU_C) "$(INTDIR)"\
 "$(INTDIR)\menu.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_MENU_C) "$(INTDIR)"\
 "$(INTDIR)\menu.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\smnu.r

!IF  "$(CFG)" == "Menu - Win32 Release"

!ELSEIF  "$(CFG)" == "Menu - Win32 Debug"

!ELSEIF  "$(CFG)" == "Menu - Macintosh Release"

DEP_MRC_SMNU_=\
	{$(INCLUDE)}"\mrc\TYPES.R"\
	{$(INCLUDE)}"\mrc\BALLOONS.R"\
	

"$(INTDIR)\smnu.rsc" : $(SOURCE) $(DEP_MRC_SMNU_) "$(INTDIR)"
   $(MRC) /o"$(INTDIR)/smnu.rsc" /D "_68K_" /D "_MAC" /D "NDEBUG" /l 0x409\
 /NOLOGO $(SOURCE)


!ELSEIF  "$(CFG)" == "Menu - Macintosh Debug"

DEP_MRC_SMNU_=\
	{$(INCLUDE)}"\mrc\TYPES.R"\
	{$(INCLUDE)}"\mrc\BALLOONS.R"\
	

"$(INTDIR)\smnu.rsc" : $(SOURCE) $(DEP_MRC_SMNU_) "$(INTDIR)"
   $(MRC) /o"$(INTDIR)/smnu.rsc" /D "_68K_" /D "_MAC" /D "_DEBUG" /l 0x409\
 /NOLOGO $(SOURCE)


!ELSEIF  "$(CFG)" == "Menu - Power Macintosh Release"

DEP_MRC_SMNU_=\
	{$(INCLUDE)}"\mrc\TYPES.R"\
	{$(INCLUDE)}"\mrc\BALLOONS.R"\
	

"$(INTDIR)\smnu.rsc" : $(SOURCE) $(DEP_MRC_SMNU_) "$(INTDIR)"
   $(MRC) /o"$(INTDIR)/smnu.rsc" /D "_MPPC_" /D "_MAC" /D "NDEBUG" /l 0x409\
 /NOLOGO $(SOURCE)


!ELSEIF  "$(CFG)" == "Menu - Power Macintosh Debug"

DEP_MRC_SMNU_=\
	{$(INCLUDE)}"\mrc\TYPES.R"\
	{$(INCLUDE)}"\mrc\BALLOONS.R"\
	

"$(INTDIR)\smnu.rsc" : $(SOURCE) $(DEP_MRC_SMNU_) "$(INTDIR)"
   $(MRC) /o"$(INTDIR)/smnu.rsc" /D "_MPPC_" /D "_MAC" /D "_DEBUG" /l 0x409\
 /NOLOGO $(SOURCE)


!ENDIF 

# End Source File
# End Target
# End Project
################################################################################
