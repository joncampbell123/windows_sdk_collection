# Microsoft Visual C++ Generated NMAKE File, Format Version 40000
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101
# TARGTYPE "Power Macintosh Application" 0x0401
# TARGTYPE "Macintosh Application" 0x0301

!IF "$(CFG)" == ""
CFG=admndemo - Win32 Debug
!MESSAGE No configuration specified.  Defaulting to admndemo - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "admndemo - Win32 Release" && "$(CFG)" !=\
 "admndemo - Win32 Debug" && "$(CFG)" != "admndemo - Macintosh Release" &&\
 "$(CFG)" != "admndemo - Macintosh Debug" && "$(CFG)" !=\
 "admndemo - Power Macintosh Release" && "$(CFG)" !=\
 "admndemo - Power Macintosh Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "admndemo.mak" CFG="admndemo - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "admndemo - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "admndemo - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE "admndemo - Macintosh Release" (based on "Macintosh Application")
!MESSAGE "admndemo - Macintosh Debug" (based on "Macintosh Application")
!MESSAGE "admndemo - Power Macintosh Release" (based on\
 "Power Macintosh Application")
!MESSAGE "admndemo - Power Macintosh Debug" (based on\
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
# PROP Target_Last_Scanned "admndemo - Power Macintosh Debug"

!IF  "$(CFG)" == "admndemo - Win32 Release"

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

ALL : "$(OUTDIR)\admndemo.exe"

CLEAN : 
	-@erase ".\Release\admndemo.exe"
	-@erase ".\Release\menu.obj"
	-@erase ".\Release\admndemo.pch"
	-@erase ".\Release\ini.obj"
	-@erase ".\Release\results.obj"
	-@erase ".\Release\info.obj"
	-@erase ".\Release\dialogs.obj"
	-@erase ".\Release\admndemo.obj"
	-@erase ".\Release\errcheck.obj"
	-@erase ".\Release\standard.obj"
	-@erase ".\Release\headers.obj"
	-@erase ".\Release\execute.obj"
	-@erase ".\Release\admndemo.res"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /Yu"headers.h" /c
CPP_PROJ=/nologo /ML /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS"\
 /Fp"$(INTDIR)/admndemo.pch" /Yu"headers.h" /Fo"$(INTDIR)/" /c 
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
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/admndemo.res" /d "NDEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/admndemo.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib ctl3d32.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib ctl3d32.lib odbc32.lib\
 odbccp32.lib /nologo /subsystem:windows /incremental:no\
 /pdb:"$(OUTDIR)/admndemo.pdb" /machine:I386 /out:"$(OUTDIR)/admndemo.exe" 
LINK32_OBJS= \
	"$(INTDIR)/menu.obj" \
	"$(INTDIR)/ini.obj" \
	"$(INTDIR)/results.obj" \
	"$(INTDIR)/info.obj" \
	"$(INTDIR)/dialogs.obj" \
	"$(INTDIR)/admndemo.obj" \
	"$(INTDIR)/errcheck.obj" \
	"$(INTDIR)/standard.obj" \
	"$(INTDIR)/headers.obj" \
	"$(INTDIR)/execute.obj" \
	"$(INTDIR)/admndemo.res"

"$(OUTDIR)\admndemo.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "admndemo - Win32 Debug"

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

ALL : "$(OUTDIR)\admndemo.exe"

CLEAN : 
	-@erase ".\Debug\vc40.pdb"
	-@erase ".\Debug\admndemo.pch"
	-@erase ".\Debug\admndemo.exe"
	-@erase ".\Debug\menu.obj"
	-@erase ".\Debug\results.obj"
	-@erase ".\Debug\info.obj"
	-@erase ".\Debug\dialogs.obj"
	-@erase ".\Debug\ini.obj"
	-@erase ".\Debug\admndemo.obj"
	-@erase ".\Debug\errcheck.obj"
	-@erase ".\Debug\headers.obj"
	-@erase ".\Debug\standard.obj"
	-@erase ".\Debug\execute.obj"
	-@erase ".\Debug\admndemo.res"
	-@erase ".\Debug\admndemo.ilk"
	-@erase ".\Debug\admndemo.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /Yu"headers.h" /c
CPP_PROJ=/nologo /MLd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /Fp"$(INTDIR)/admndemo.pch" /Yu"headers.h" /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
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
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/admndemo.res" /d "_DEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/admndemo.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib ctl3d32.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib ctl3d32.lib odbc32.lib\
 odbccp32.lib /nologo /subsystem:windows /incremental:yes\
 /pdb:"$(OUTDIR)/admndemo.pdb" /debug /machine:I386\
 /out:"$(OUTDIR)/admndemo.exe" 
LINK32_OBJS= \
	"$(INTDIR)/menu.obj" \
	"$(INTDIR)/results.obj" \
	"$(INTDIR)/info.obj" \
	"$(INTDIR)/dialogs.obj" \
	"$(INTDIR)/ini.obj" \
	"$(INTDIR)/admndemo.obj" \
	"$(INTDIR)/errcheck.obj" \
	"$(INTDIR)/headers.obj" \
	"$(INTDIR)/standard.obj" \
	"$(INTDIR)/execute.obj" \
	"$(INTDIR)/admndemo.res"

"$(OUTDIR)\admndemo.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "admndemo - Macintosh Release"

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

ALL : "$(OUTDIR)\admndemo.exe" "$(OUTDIR)\admndemo.trg"

CLEAN : 
	-@erase ".\MacRel\admndemo.trg"
	-@erase ".\MacRel\admndemo.exe"
	-@erase ".\MacRel\errcheck.obj"
	-@erase ".\MacRel\admndemo.pch"
	-@erase ".\MacRel\menu.obj"
	-@erase ".\MacRel\standard.obj"
	-@erase ".\MacRel\info.obj"
	-@erase ".\MacRel\headers.obj"
	-@erase ".\MacRel\execute.obj"
	-@erase ".\MacRel\results.obj"
	-@erase ".\MacRel\dialogs.obj"
	-@erase ".\MacRel\ini.obj"
	-@erase ".\MacRel\admndemo.obj"
	-@erase ".\MacRel\admndemo.rsc"
	-@erase ".\MacRel\admnmac.rsc"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

MTL=mktyplib.exe
# ADD BASE MTL /nologo /D "NDEBUG" /mac
# ADD MTL /nologo /D "NDEBUG" /mac
MTL_PROJ=/nologo /D "NDEBUG" /mac 
CPP=cl.exe
# ADD BASE CPP /nologo /AL /Q68s /W3 /GX /O2 /D "_WINDOWS" /D "_MAC" /D "_68K_" /D "WIN32" /D "NDEBUG" /YX /c
# ADD CPP /nologo /AL /Q68s /W3 /GX /O2 /D "_WINDOWS" /D "_MAC" /D "_68K_" /D "WIN32" /D "NDEBUG" /Yu"headers.h" /c
CPP_PROJ=/nologo /AL /Q68s /W3 /GX /O2 /D "_WINDOWS" /D "_MAC" /D "_68K_" /D\
 "WIN32" /D "NDEBUG" /Fp"$(INTDIR)/admndemo.pch" /Yu"headers.h" /Fo"$(INTDIR)/"\
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
RSC_PROJ=/l 0x409 /r /m /fo"$(INTDIR)/admndemo.rsc" /d "_MAC" /d "_68K_" /d\
 "NDEBUG" 
MRC=mrc.exe
# ADD BASE MRC /D "_68K_" /D "_MAC" /D "NDEBUG" /NOLOGO
# ADD MRC /D "_68K_" /D "_MAC" /D "NDEBUG" /NOLOGO
MRC_PROJ=/D "_68K_" /D "_MAC" /D "NDEBUG" /l 0x409 /NOLOGO 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/admndemo.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 wlm.lib /nologo /MAC:bundle /machine:M68K
# ADD LINK32 wlm.lib /nologo /MAC:bundle /MAC:creator="ADMN" /machine:M68K
LINK32_FLAGS=wlm.lib /nologo /MAC:bundle /MAC:type="APPL" /MAC:creator="ADMN"\
 /pdb:"$(OUTDIR)/admndemo.pdb" /machine:M68K /out:"$(OUTDIR)/admndemo.exe" 
LINK32_OBJS= \
	"$(INTDIR)/errcheck.obj" \
	"$(INTDIR)/menu.obj" \
	"$(INTDIR)/standard.obj" \
	"$(INTDIR)/info.obj" \
	"$(INTDIR)/headers.obj" \
	"$(INTDIR)/execute.obj" \
	"$(INTDIR)/results.obj" \
	"$(INTDIR)/dialogs.obj" \
	"$(INTDIR)/ini.obj" \
	"$(INTDIR)/admndemo.obj" \
	"$(INTDIR)/admndemo.rsc" \
	"$(INTDIR)/admnmac.rsc"

"$(OUTDIR)\admndemo.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

MFILE32=mfile.exe
# ADD BASE MFILE32 COPY /NOLOGO
# ADD MFILE32 COPY /NOLOGO
MFILE32_FLAGS=COPY /NOLOGO 
MFILE32_FILES= \
	"$(OUTDIR)/admndemo.exe"

"$(OUTDIR)\admndemo.trg" : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\MacRel\admndemo.exe\
 "$(MFILE32_DEST):admndemo.exe">"$(OUTDIR)\admndemo.trg"

DOWNLOAD : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\MacRel\admndemo.exe\
 "$(MFILE32_DEST):admndemo.exe">"$(OUTDIR)\admndemo.trg"

!ELSEIF  "$(CFG)" == "admndemo - Macintosh Debug"

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

ALL : "$(OUTDIR)\admndemo.exe" "$(OUTDIR)\admndemo.trg"

CLEAN : 
	-@erase ".\MacDbg\vc40.pdb"
	-@erase ".\MacDbg\admndemo.pch"
	-@erase ".\MacDbg\admndemo.pdb"
	-@erase ".\MacDbg\admndemo.obj"
	-@erase ".\MacDbg\errcheck.obj"
	-@erase ".\MacDbg\results.obj"
	-@erase ".\MacDbg\dialogs.obj"
	-@erase ".\MacDbg\standard.obj"
	-@erase ".\MacDbg\ini.obj"
	-@erase ".\MacDbg\menu.obj"
	-@erase ".\MacDbg\headers.obj"
	-@erase ".\MacDbg\info.obj"
	-@erase ".\MacDbg\execute.obj"
	-@erase ".\MacDbg\admnmac.rsc"
	-@erase ".\MacDbg\admndemo.rsc"
	-@erase ".\MacDbg\admndemo.trg"
	-@erase ".\MacDbg\admndemo.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

MTL=mktyplib.exe
# ADD BASE MTL /nologo /D "_DEBUG" /mac
# ADD MTL /nologo /D "_DEBUG" /mac
MTL_PROJ=/nologo /D "_DEBUG" /mac 
CPP=cl.exe
# ADD BASE CPP /nologo /AL /Q68s /Q68m /W3 /GX /Zi /Od /D "_WINDOWS" /D "_MAC" /D "_68K_" /D "WIN32" /D "_DEBUG" /YX /c
# ADD CPP /nologo /AL /Q68s /Q68m /W3 /GX /Zi /Od /D "_WINDOWS" /D "_MAC" /D "_68K_" /D "WIN32" /D "_DEBUG" /Yu"headers.h" /c
CPP_PROJ=/nologo /AL /Q68s /Q68m /W3 /GX /Zi /Od /D "_WINDOWS" /D "_MAC" /D\
 "_68K_" /D "WIN32" /D "_DEBUG" /Fp"$(INTDIR)/admndemo.pch" /Yu"headers.h"\
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
RSC_PROJ=/l 0x409 /r /m /fo"$(INTDIR)/admndemo.rsc" /d "_MAC" /d "_68K_" /d\
 "_DEBUG" 
MRC=mrc.exe
# ADD BASE MRC /D "_68K_" /D "_MAC" /D "_DEBUG" /NOLOGO
# ADD MRC /D "_68K_" /D "_MAC" /D "_DEBUG" /NOLOGO
MRC_PROJ=/D "_68K_" /D "_MAC" /D "_DEBUG" /l 0x409 /NOLOGO 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/admndemo.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 wlm.lib /nologo /MAC:bundle /debug /machine:M68K
# ADD LINK32 wlm.lib /nologo /MAC:bundle /MAC:creator="ADMN" /debug /machine:M68K
LINK32_FLAGS=wlm.lib /nologo /MAC:bundle /MAC:type="APPL" /MAC:creator="ADMN"\
 /pdb:"$(OUTDIR)/admndemo.pdb" /debug /machine:M68K\
 /out:"$(OUTDIR)/admndemo.exe" 
LINK32_OBJS= \
	"$(INTDIR)/admndemo.obj" \
	"$(INTDIR)/errcheck.obj" \
	"$(INTDIR)/results.obj" \
	"$(INTDIR)/dialogs.obj" \
	"$(INTDIR)/standard.obj" \
	"$(INTDIR)/ini.obj" \
	"$(INTDIR)/menu.obj" \
	"$(INTDIR)/headers.obj" \
	"$(INTDIR)/info.obj" \
	"$(INTDIR)/execute.obj" \
	"$(INTDIR)/admnmac.rsc" \
	"$(INTDIR)/admndemo.rsc"

"$(OUTDIR)\admndemo.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

MFILE32=mfile.exe
# ADD BASE MFILE32 COPY /NOLOGO
# ADD MFILE32 COPY /NOLOGO
MFILE32_FLAGS=COPY /NOLOGO 
MFILE32_FILES= \
	"$(OUTDIR)/admndemo.exe"

"$(OUTDIR)\admndemo.trg" : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\MacDbg\admndemo.exe\
 "$(MFILE32_DEST):admndemo.exe">"$(OUTDIR)\admndemo.trg"

DOWNLOAD : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\MacDbg\admndemo.exe\
 "$(MFILE32_DEST):admndemo.exe">"$(OUTDIR)\admndemo.trg"

!ELSEIF  "$(CFG)" == "admndemo - Power Macintosh Release"

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

ALL : "$(OUTDIR)\admndemo.exe" "$(OUTDIR)\admndemo.trg"

CLEAN : 
	-@erase ".\PMacRel\admndemo.trg"
	-@erase ".\PMacRel\admndemo.exe"
	-@erase ".\PMacRel\ini.obj"
	-@erase ".\PMacRel\admndemo.pch"
	-@erase ".\PMacRel\headers.obj"
	-@erase ".\PMacRel\execute.obj"
	-@erase ".\PMacRel\admndemo.obj"
	-@erase ".\PMacRel\errcheck.obj"
	-@erase ".\PMacRel\results.obj"
	-@erase ".\PMacRel\menu.obj"
	-@erase ".\PMacRel\info.obj"
	-@erase ".\PMacRel\dialogs.obj"
	-@erase ".\PMacRel\standard.obj"
	-@erase ".\PMacRel\admndemo.rsc"
	-@erase ".\PMacRel\admnmac.rsc"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

MTL=mktyplib.exe
# ADD BASE MTL /nologo /D "NDEBUG"
# ADD MTL /nologo /D "NDEBUG"
MTL_PROJ=/nologo /D "NDEBUG" 
CPP=cl.exe
# ADD BASE CPP /nologo /ML /W3 /GX /O2 /D "_MAC" /D "_MPPC_" /D "_WINDOWS" /D "WIN32" /D "NDEBUG" /YX /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "_MAC" /D "_MPPC_" /D "_WINDOWS" /D "WIN32" /D "NDEBUG" /D "_WLMDLL" /Yu"headers.h" /c
CPP_PROJ=/nologo /MD /W3 /GX /O2 /D "_MAC" /D "_MPPC_" /D "_WINDOWS" /D "WIN32"\
 /D "NDEBUG" /D "_WLMDLL" /Fp"$(INTDIR)/admndemo.pch" /Yu"headers.h"\
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
RSC_PROJ=/l 0x409 /r /m /fo"$(INTDIR)/admndemo.rsc" /d "_MAC" /d "_MPPC_" /d\
 "NDEBUG" 
MRC=mrc.exe
# ADD BASE MRC /D "_MPPC_" /D "_MAC" /D "NDEBUG" /NOLOGO
# ADD MRC /D "_MPPC_" /D "_MAC" /D "NDEBUG" /NOLOGO
MRC_PROJ=/D "_MPPC_" /D "_MAC" /D "NDEBUG" /l 0x409 /NOLOGO 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/admndemo.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 cfrg.rsc /nologo /mac:bundle /machine:MPPC
# ADD LINK32 /nologo /mac:bundle /mac:creator="ADMN" /machine:MPPC
LINK32_FLAGS=/nologo /mac:bundle /mac:type="APPL" /mac:creator="ADMN"\
 /incremental:no /pdb:"$(OUTDIR)/admndemo.pdb" /machine:MPPC\
 /out:"$(OUTDIR)/admndemo.exe" 
LINK32_OBJS= \
	"$(INTDIR)/ini.obj" \
	"$(INTDIR)/headers.obj" \
	"$(INTDIR)/execute.obj" \
	"$(INTDIR)/admndemo.obj" \
	"$(INTDIR)/errcheck.obj" \
	"$(INTDIR)/results.obj" \
	"$(INTDIR)/menu.obj" \
	"$(INTDIR)/info.obj" \
	"$(INTDIR)/dialogs.obj" \
	"$(INTDIR)/standard.obj" \
	"$(INTDIR)/admndemo.rsc" \
	"$(INTDIR)/admnmac.rsc"

"$(OUTDIR)\admndemo.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

MFILE32=mfile.exe
# ADD BASE MFILE32 COPY /NOLOGO
# ADD MFILE32 COPY /NOLOGO
MFILE32_FLAGS=COPY /NOLOGO 
MFILE32_FILES= \
	"$(OUTDIR)/admndemo.exe"

"$(OUTDIR)\admndemo.trg" : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\PMacRel\admndemo.exe\
 "$(MFILE32_DEST):admndemo.exe">"$(OUTDIR)\admndemo.trg"

DOWNLOAD : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\PMacRel\admndemo.exe\
 "$(MFILE32_DEST):admndemo.exe">"$(OUTDIR)\admndemo.trg"

!ELSEIF  "$(CFG)" == "admndemo - Power Macintosh Debug"

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

ALL : "$(OUTDIR)\admndemo.exe" "$(OUTDIR)\admndemo.trg"

CLEAN : 
	-@erase ".\PMacDbg\vc40.pdb"
	-@erase ".\PMacDbg\admndemo.pch"
	-@erase ".\PMacDbg\admndemo.ilk"
	-@erase ".\PMacDbg\results.obj"
	-@erase ".\PMacDbg\dialogs.obj"
	-@erase ".\PMacDbg\menu.obj"
	-@erase ".\PMacDbg\info.obj"
	-@erase ".\PMacDbg\admndemo.obj"
	-@erase ".\PMacDbg\ini.obj"
	-@erase ".\PMacDbg\headers.obj"
	-@erase ".\PMacDbg\errcheck.obj"
	-@erase ".\PMacDbg\execute.obj"
	-@erase ".\PMacDbg\standard.obj"
	-@erase ".\PMacDbg\admnmac.rsc"
	-@erase ".\PMacDbg\admndemo.rsc"
	-@erase ".\PMacDbg\admndemo.pdb"
	-@erase ".\PMacDbg\admndemo.trg"
	-@erase ".\PMacDbg\admndemo.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

MTL=mktyplib.exe
# ADD BASE MTL /nologo /D "_DEBUG"
# ADD MTL /nologo /D "_DEBUG"
MTL_PROJ=/nologo /D "_DEBUG" 
CPP=cl.exe
# ADD BASE CPP /nologo /W3 /GX /Zi /Od /D "_MAC" /D "_MPPC_" /D "_WINDOWS" /D "WIN32" /D "_DEBUG" /YX /c
# ADD CPP /nologo /MDd /W3 /GX /Zi /Od /D "_MAC" /D "_MPPC_" /D "_WINDOWS" /D "WIN32" /D "_DEBUG" /D "_WLMDLL" /Yu"headers.h" /c
CPP_PROJ=/nologo /MDd /W3 /GX /Zi /Od /D "_MAC" /D "_MPPC_" /D "_WINDOWS" /D\
 "WIN32" /D "_DEBUG" /D "_WLMDLL" /Fp"$(INTDIR)/admndemo.pch" /Yu"headers.h"\
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
RSC_PROJ=/l 0x409 /r /m /fo"$(INTDIR)/admndemo.rsc" /d "_MAC" /d "_MPPC_" /d\
 "_DEBUG" 
MRC=mrc.exe
# ADD BASE MRC /D "_MPPC_" /D "_MAC" /D "_DEBUG" /NOLOGO
# ADD MRC /D "_MPPC_" /D "_MAC" /D "_DEBUG" /NOLOGO
MRC_PROJ=/D "_MPPC_" /D "_MAC" /D "_DEBUG" /l 0x409 /NOLOGO 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/admndemo.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 cfrg.rsc /nologo /mac:bundle /debug /machine:MPPC
# ADD LINK32 /nologo /mac:bundle /mac:creator="ADMN" /debug /machine:MPPC
LINK32_FLAGS=/nologo /mac:bundle /mac:type="APPL" /mac:creator="ADMN"\
 /incremental:yes /pdb:"$(OUTDIR)/admndemo.pdb" /debug /machine:MPPC\
 /out:"$(OUTDIR)/admndemo.exe" 
LINK32_OBJS= \
	"$(INTDIR)/results.obj" \
	"$(INTDIR)/dialogs.obj" \
	"$(INTDIR)/menu.obj" \
	"$(INTDIR)/info.obj" \
	"$(INTDIR)/admndemo.obj" \
	"$(INTDIR)/ini.obj" \
	"$(INTDIR)/headers.obj" \
	"$(INTDIR)/errcheck.obj" \
	"$(INTDIR)/execute.obj" \
	"$(INTDIR)/standard.obj" \
	"$(INTDIR)/admnmac.rsc" \
	"$(INTDIR)/admndemo.rsc"

"$(OUTDIR)\admndemo.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

MFILE32=mfile.exe
# ADD BASE MFILE32 COPY /NOLOGO
# ADD MFILE32 COPY /NOLOGO
MFILE32_FLAGS=COPY /NOLOGO 
MFILE32_FILES= \
	"$(OUTDIR)/admndemo.exe"

"$(OUTDIR)\admndemo.trg" : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\PMacDbg\admndemo.exe\
 "$(MFILE32_DEST):admndemo.exe">"$(OUTDIR)\admndemo.trg"

DOWNLOAD : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\PMacDbg\admndemo.exe\
 "$(MFILE32_DEST):admndemo.exe">"$(OUTDIR)\admndemo.trg"

!ENDIF 

################################################################################
# Begin Target

# Name "admndemo - Win32 Release"
# Name "admndemo - Win32 Debug"
# Name "admndemo - Macintosh Release"
# Name "admndemo - Macintosh Debug"
# Name "admndemo - Power Macintosh Release"
# Name "admndemo - Power Macintosh Debug"

!IF  "$(CFG)" == "admndemo - Win32 Release"

!ELSEIF  "$(CFG)" == "admndemo - Win32 Debug"

!ELSEIF  "$(CFG)" == "admndemo - Macintosh Release"

!ELSEIF  "$(CFG)" == "admndemo - Macintosh Debug"

!ELSEIF  "$(CFG)" == "admndemo - Power Macintosh Release"

!ELSEIF  "$(CFG)" == "admndemo - Power Macintosh Debug"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\admndemo.c
DEP_CPP_ADMND=\
	".\headers.h"\
	".\admndemo.h"\
	".\child.h"\
	".\menu.h"\
	".\results.h"\
	".\standard.h"\
	".\dialogs.h"\
	".\errcheck.h"\
	".\execute.h"\
	".\ini.h"\
	".\info.h"\
	".\resource.h"\
	

!IF  "$(CFG)" == "admndemo - Win32 Release"


"$(INTDIR)\admndemo.obj" : $(SOURCE) $(DEP_CPP_ADMND) "$(INTDIR)"\
 "$(INTDIR)\admndemo.pch"


!ELSEIF  "$(CFG)" == "admndemo - Win32 Debug"


"$(INTDIR)\admndemo.obj" : $(SOURCE) $(DEP_CPP_ADMND) "$(INTDIR)"\
 "$(INTDIR)\admndemo.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_ADMND) "$(INTDIR)"\
 "$(INTDIR)\admndemo.pch"


!ELSEIF  "$(CFG)" == "admndemo - Macintosh Release"


"$(INTDIR)\admndemo.obj" : $(SOURCE) $(DEP_CPP_ADMND) "$(INTDIR)"\
 "$(INTDIR)\admndemo.pch"


!ELSEIF  "$(CFG)" == "admndemo - Macintosh Debug"


"$(INTDIR)\admndemo.obj" : $(SOURCE) $(DEP_CPP_ADMND) "$(INTDIR)"\
 "$(INTDIR)\admndemo.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_ADMND) "$(INTDIR)"\
 "$(INTDIR)\admndemo.pch"


!ELSEIF  "$(CFG)" == "admndemo - Power Macintosh Release"


"$(INTDIR)\admndemo.obj" : $(SOURCE) $(DEP_CPP_ADMND) "$(INTDIR)"\
 "$(INTDIR)\admndemo.pch"


!ELSEIF  "$(CFG)" == "admndemo - Power Macintosh Debug"


"$(INTDIR)\admndemo.obj" : $(SOURCE) $(DEP_CPP_ADMND) "$(INTDIR)"\
 "$(INTDIR)\admndemo.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_ADMND) "$(INTDIR)"\
 "$(INTDIR)\admndemo.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\dialogs.c
DEP_CPP_DIALO=\
	".\headers.h"\
	".\admndemo.h"\
	".\child.h"\
	".\menu.h"\
	".\results.h"\
	".\standard.h"\
	".\dialogs.h"\
	".\errcheck.h"\
	".\execute.h"\
	".\ini.h"\
	".\info.h"\
	".\resource.h"\
	

!IF  "$(CFG)" == "admndemo - Win32 Release"


"$(INTDIR)\dialogs.obj" : $(SOURCE) $(DEP_CPP_DIALO) "$(INTDIR)"\
 "$(INTDIR)\admndemo.pch"


!ELSEIF  "$(CFG)" == "admndemo - Win32 Debug"


"$(INTDIR)\dialogs.obj" : $(SOURCE) $(DEP_CPP_DIALO) "$(INTDIR)"\
 "$(INTDIR)\admndemo.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_DIALO) "$(INTDIR)"\
 "$(INTDIR)\admndemo.pch"


!ELSEIF  "$(CFG)" == "admndemo - Macintosh Release"


"$(INTDIR)\dialogs.obj" : $(SOURCE) $(DEP_CPP_DIALO) "$(INTDIR)"\
 "$(INTDIR)\admndemo.pch"


!ELSEIF  "$(CFG)" == "admndemo - Macintosh Debug"


"$(INTDIR)\dialogs.obj" : $(SOURCE) $(DEP_CPP_DIALO) "$(INTDIR)"\
 "$(INTDIR)\admndemo.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_DIALO) "$(INTDIR)"\
 "$(INTDIR)\admndemo.pch"


!ELSEIF  "$(CFG)" == "admndemo - Power Macintosh Release"


"$(INTDIR)\dialogs.obj" : $(SOURCE) $(DEP_CPP_DIALO) "$(INTDIR)"\
 "$(INTDIR)\admndemo.pch"


!ELSEIF  "$(CFG)" == "admndemo - Power Macintosh Debug"


"$(INTDIR)\dialogs.obj" : $(SOURCE) $(DEP_CPP_DIALO) "$(INTDIR)"\
 "$(INTDIR)\admndemo.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_DIALO) "$(INTDIR)"\
 "$(INTDIR)\admndemo.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\errcheck.c
DEP_CPP_ERRCH=\
	".\headers.h"\
	".\admndemo.h"\
	".\child.h"\
	".\dialogs.h"\
	".\errcheck.h"\
	".\execute.h"\
	".\info.h"\
	".\ini.h"\
	".\menu.h"\
	".\resource.h"\
	".\results.h"\
	".\standard.h"\
	

!IF  "$(CFG)" == "admndemo - Win32 Release"


"$(INTDIR)\errcheck.obj" : $(SOURCE) $(DEP_CPP_ERRCH) "$(INTDIR)"\
 "$(INTDIR)\admndemo.pch"


!ELSEIF  "$(CFG)" == "admndemo - Win32 Debug"


"$(INTDIR)\errcheck.obj" : $(SOURCE) $(DEP_CPP_ERRCH) "$(INTDIR)"\
 "$(INTDIR)\admndemo.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_ERRCH) "$(INTDIR)"\
 "$(INTDIR)\admndemo.pch"


!ELSEIF  "$(CFG)" == "admndemo - Macintosh Release"


"$(INTDIR)\errcheck.obj" : $(SOURCE) $(DEP_CPP_ERRCH) "$(INTDIR)"\
 "$(INTDIR)\admndemo.pch"


!ELSEIF  "$(CFG)" == "admndemo - Macintosh Debug"


"$(INTDIR)\errcheck.obj" : $(SOURCE) $(DEP_CPP_ERRCH) "$(INTDIR)"\
 "$(INTDIR)\admndemo.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_ERRCH) "$(INTDIR)"\
 "$(INTDIR)\admndemo.pch"


!ELSEIF  "$(CFG)" == "admndemo - Power Macintosh Release"


"$(INTDIR)\errcheck.obj" : $(SOURCE) $(DEP_CPP_ERRCH) "$(INTDIR)"\
 "$(INTDIR)\admndemo.pch"


!ELSEIF  "$(CFG)" == "admndemo - Power Macintosh Debug"


"$(INTDIR)\errcheck.obj" : $(SOURCE) $(DEP_CPP_ERRCH) "$(INTDIR)"\
 "$(INTDIR)\admndemo.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_ERRCH) "$(INTDIR)"\
 "$(INTDIR)\admndemo.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\execute.c
DEP_CPP_EXECU=\
	".\headers.h"\
	".\admndemo.h"\
	".\child.h"\
	".\menu.h"\
	".\results.h"\
	".\standard.h"\
	".\dialogs.h"\
	".\errcheck.h"\
	".\execute.h"\
	".\ini.h"\
	".\info.h"\
	".\resource.h"\
	

!IF  "$(CFG)" == "admndemo - Win32 Release"


"$(INTDIR)\execute.obj" : $(SOURCE) $(DEP_CPP_EXECU) "$(INTDIR)"\
 "$(INTDIR)\admndemo.pch"


!ELSEIF  "$(CFG)" == "admndemo - Win32 Debug"


"$(INTDIR)\execute.obj" : $(SOURCE) $(DEP_CPP_EXECU) "$(INTDIR)"\
 "$(INTDIR)\admndemo.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_EXECU) "$(INTDIR)"\
 "$(INTDIR)\admndemo.pch"


!ELSEIF  "$(CFG)" == "admndemo - Macintosh Release"


"$(INTDIR)\execute.obj" : $(SOURCE) $(DEP_CPP_EXECU) "$(INTDIR)"\
 "$(INTDIR)\admndemo.pch"


!ELSEIF  "$(CFG)" == "admndemo - Macintosh Debug"


"$(INTDIR)\execute.obj" : $(SOURCE) $(DEP_CPP_EXECU) "$(INTDIR)"\
 "$(INTDIR)\admndemo.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_EXECU) "$(INTDIR)"\
 "$(INTDIR)\admndemo.pch"


!ELSEIF  "$(CFG)" == "admndemo - Power Macintosh Release"


"$(INTDIR)\execute.obj" : $(SOURCE) $(DEP_CPP_EXECU) "$(INTDIR)"\
 "$(INTDIR)\admndemo.pch"


!ELSEIF  "$(CFG)" == "admndemo - Power Macintosh Debug"


"$(INTDIR)\execute.obj" : $(SOURCE) $(DEP_CPP_EXECU) "$(INTDIR)"\
 "$(INTDIR)\admndemo.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_EXECU) "$(INTDIR)"\
 "$(INTDIR)\admndemo.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\headers.c
DEP_CPP_HEADE=\
	".\headers.h"\
	".\admndemo.h"\
	".\child.h"\
	".\dialogs.h"\
	".\errcheck.h"\
	".\execute.h"\
	".\info.h"\
	".\ini.h"\
	".\menu.h"\
	".\resource.h"\
	".\results.h"\
	".\standard.h"\
	

!IF  "$(CFG)" == "admndemo - Win32 Release"

# ADD CPP /Yc"headers.h"

BuildCmds= \
	$(CPP) /nologo /ML /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS"\
 /Fp"$(INTDIR)/admndemo.pch" /Yc"headers.h" /Fo"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\headers.obj" : $(SOURCE) $(DEP_CPP_HEADE) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\admndemo.pch" : $(SOURCE) $(DEP_CPP_HEADE) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "admndemo - Win32 Debug"

# ADD CPP /Yc"headers.h"

BuildCmds= \
	$(CPP) /nologo /MLd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /Fp"$(INTDIR)/admndemo.pch" /Yc"headers.h" /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c\
 $(SOURCE) \
	

"$(INTDIR)\headers.obj" : $(SOURCE) $(DEP_CPP_HEADE) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_HEADE) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\admndemo.pch" : $(SOURCE) $(DEP_CPP_HEADE) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "admndemo - Macintosh Release"

# ADD CPP /Yc

BuildCmds= \
	$(CPP) /nologo /AL /Q68s /W3 /GX /O2 /D "_WINDOWS" /D "_MAC" /D "_68K_" /D\
 "WIN32" /D "NDEBUG" /Fp"$(INTDIR)/admndemo.pch" /Yc /Fo"$(INTDIR)/" /c\
 $(SOURCE) \
	

"$(INTDIR)\headers.obj" : $(SOURCE) $(DEP_CPP_HEADE) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\admndemo.pch" : $(SOURCE) $(DEP_CPP_HEADE) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "admndemo - Macintosh Debug"

# ADD CPP /Yc

BuildCmds= \
	$(CPP) /nologo /AL /Q68s /Q68m /W3 /GX /Zi /Od /D "_WINDOWS" /D "_MAC" /D\
 "_68K_" /D "WIN32" /D "_DEBUG" /Fp"$(INTDIR)/admndemo.pch" /Yc /Fo"$(INTDIR)/"\
 /Fd"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\headers.obj" : $(SOURCE) $(DEP_CPP_HEADE) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_HEADE) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\admndemo.pch" : $(SOURCE) $(DEP_CPP_HEADE) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "admndemo - Power Macintosh Release"

# ADD CPP /Yc

BuildCmds= \
	$(CPP) /nologo /MD /W3 /GX /O2 /D "_MAC" /D "_MPPC_" /D "_WINDOWS" /D "WIN32"\
 /D "NDEBUG" /D "_WLMDLL" /Fp"$(INTDIR)/admndemo.pch" /Yc /Fo"$(INTDIR)/" /c\
 $(SOURCE) \
	

"$(INTDIR)\headers.obj" : $(SOURCE) $(DEP_CPP_HEADE) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\admndemo.pch" : $(SOURCE) $(DEP_CPP_HEADE) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "admndemo - Power Macintosh Debug"

# ADD CPP /Yc"headers.h"

BuildCmds= \
	$(CPP) /nologo /MDd /W3 /GX /Zi /Od /D "_MAC" /D "_MPPC_" /D "_WINDOWS" /D\
 "WIN32" /D "_DEBUG" /D "_WLMDLL" /Fp"$(INTDIR)/admndemo.pch" /Yc"headers.h"\
 /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\headers.obj" : $(SOURCE) $(DEP_CPP_HEADE) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_HEADE) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\admndemo.pch" : $(SOURCE) $(DEP_CPP_HEADE) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\info.c
DEP_CPP_INFO_=\
	".\headers.h"\
	".\admndemo.h"\
	".\child.h"\
	".\dialogs.h"\
	".\errcheck.h"\
	".\execute.h"\
	".\info.h"\
	".\ini.h"\
	".\menu.h"\
	".\resource.h"\
	".\results.h"\
	".\standard.h"\
	

!IF  "$(CFG)" == "admndemo - Win32 Release"


"$(INTDIR)\info.obj" : $(SOURCE) $(DEP_CPP_INFO_) "$(INTDIR)"\
 "$(INTDIR)\admndemo.pch"


!ELSEIF  "$(CFG)" == "admndemo - Win32 Debug"


"$(INTDIR)\info.obj" : $(SOURCE) $(DEP_CPP_INFO_) "$(INTDIR)"\
 "$(INTDIR)\admndemo.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_INFO_) "$(INTDIR)"\
 "$(INTDIR)\admndemo.pch"


!ELSEIF  "$(CFG)" == "admndemo - Macintosh Release"


"$(INTDIR)\info.obj" : $(SOURCE) $(DEP_CPP_INFO_) "$(INTDIR)"\
 "$(INTDIR)\admndemo.pch"


!ELSEIF  "$(CFG)" == "admndemo - Macintosh Debug"


"$(INTDIR)\info.obj" : $(SOURCE) $(DEP_CPP_INFO_) "$(INTDIR)"\
 "$(INTDIR)\admndemo.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_INFO_) "$(INTDIR)"\
 "$(INTDIR)\admndemo.pch"


!ELSEIF  "$(CFG)" == "admndemo - Power Macintosh Release"


"$(INTDIR)\info.obj" : $(SOURCE) $(DEP_CPP_INFO_) "$(INTDIR)"\
 "$(INTDIR)\admndemo.pch"


!ELSEIF  "$(CFG)" == "admndemo - Power Macintosh Debug"


"$(INTDIR)\info.obj" : $(SOURCE) $(DEP_CPP_INFO_) "$(INTDIR)"\
 "$(INTDIR)\admndemo.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_INFO_) "$(INTDIR)"\
 "$(INTDIR)\admndemo.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\ini.c
DEP_CPP_INI_C=\
	".\headers.h"\
	".\admndemo.h"\
	".\child.h"\
	".\dialogs.h"\
	".\errcheck.h"\
	".\execute.h"\
	".\info.h"\
	".\ini.h"\
	".\menu.h"\
	".\resource.h"\
	".\results.h"\
	".\standard.h"\
	

!IF  "$(CFG)" == "admndemo - Win32 Release"


"$(INTDIR)\ini.obj" : $(SOURCE) $(DEP_CPP_INI_C) "$(INTDIR)"\
 "$(INTDIR)\admndemo.pch"


!ELSEIF  "$(CFG)" == "admndemo - Win32 Debug"


"$(INTDIR)\ini.obj" : $(SOURCE) $(DEP_CPP_INI_C) "$(INTDIR)"\
 "$(INTDIR)\admndemo.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_INI_C) "$(INTDIR)"\
 "$(INTDIR)\admndemo.pch"


!ELSEIF  "$(CFG)" == "admndemo - Macintosh Release"


"$(INTDIR)\ini.obj" : $(SOURCE) $(DEP_CPP_INI_C) "$(INTDIR)"\
 "$(INTDIR)\admndemo.pch"


!ELSEIF  "$(CFG)" == "admndemo - Macintosh Debug"


"$(INTDIR)\ini.obj" : $(SOURCE) $(DEP_CPP_INI_C) "$(INTDIR)"\
 "$(INTDIR)\admndemo.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_INI_C) "$(INTDIR)"\
 "$(INTDIR)\admndemo.pch"


!ELSEIF  "$(CFG)" == "admndemo - Power Macintosh Release"


"$(INTDIR)\ini.obj" : $(SOURCE) $(DEP_CPP_INI_C) "$(INTDIR)"\
 "$(INTDIR)\admndemo.pch"


!ELSEIF  "$(CFG)" == "admndemo - Power Macintosh Debug"


"$(INTDIR)\ini.obj" : $(SOURCE) $(DEP_CPP_INI_C) "$(INTDIR)"\
 "$(INTDIR)\admndemo.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_INI_C) "$(INTDIR)"\
 "$(INTDIR)\admndemo.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\menu.c
DEP_CPP_MENU_=\
	".\headers.h"\
	".\admndemo.h"\
	".\child.h"\
	".\dialogs.h"\
	".\errcheck.h"\
	".\execute.h"\
	".\info.h"\
	".\ini.h"\
	".\menu.h"\
	".\resource.h"\
	".\results.h"\
	".\standard.h"\
	

!IF  "$(CFG)" == "admndemo - Win32 Release"


"$(INTDIR)\menu.obj" : $(SOURCE) $(DEP_CPP_MENU_) "$(INTDIR)"\
 "$(INTDIR)\admndemo.pch"


!ELSEIF  "$(CFG)" == "admndemo - Win32 Debug"


"$(INTDIR)\menu.obj" : $(SOURCE) $(DEP_CPP_MENU_) "$(INTDIR)"\
 "$(INTDIR)\admndemo.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_MENU_) "$(INTDIR)"\
 "$(INTDIR)\admndemo.pch"


!ELSEIF  "$(CFG)" == "admndemo - Macintosh Release"


"$(INTDIR)\menu.obj" : $(SOURCE) $(DEP_CPP_MENU_) "$(INTDIR)"\
 "$(INTDIR)\admndemo.pch"


!ELSEIF  "$(CFG)" == "admndemo - Macintosh Debug"


"$(INTDIR)\menu.obj" : $(SOURCE) $(DEP_CPP_MENU_) "$(INTDIR)"\
 "$(INTDIR)\admndemo.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_MENU_) "$(INTDIR)"\
 "$(INTDIR)\admndemo.pch"


!ELSEIF  "$(CFG)" == "admndemo - Power Macintosh Release"


"$(INTDIR)\menu.obj" : $(SOURCE) $(DEP_CPP_MENU_) "$(INTDIR)"\
 "$(INTDIR)\admndemo.pch"


!ELSEIF  "$(CFG)" == "admndemo - Power Macintosh Debug"


"$(INTDIR)\menu.obj" : $(SOURCE) $(DEP_CPP_MENU_) "$(INTDIR)"\
 "$(INTDIR)\admndemo.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_MENU_) "$(INTDIR)"\
 "$(INTDIR)\admndemo.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\results.c
DEP_CPP_RESUL=\
	".\headers.h"\
	".\admndemo.h"\
	".\child.h"\
	".\dialogs.h"\
	".\errcheck.h"\
	".\execute.h"\
	".\info.h"\
	".\ini.h"\
	".\menu.h"\
	".\resource.h"\
	".\results.h"\
	".\standard.h"\
	

!IF  "$(CFG)" == "admndemo - Win32 Release"


"$(INTDIR)\results.obj" : $(SOURCE) $(DEP_CPP_RESUL) "$(INTDIR)"\
 "$(INTDIR)\admndemo.pch"


!ELSEIF  "$(CFG)" == "admndemo - Win32 Debug"


"$(INTDIR)\results.obj" : $(SOURCE) $(DEP_CPP_RESUL) "$(INTDIR)"\
 "$(INTDIR)\admndemo.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_RESUL) "$(INTDIR)"\
 "$(INTDIR)\admndemo.pch"


!ELSEIF  "$(CFG)" == "admndemo - Macintosh Release"


"$(INTDIR)\results.obj" : $(SOURCE) $(DEP_CPP_RESUL) "$(INTDIR)"\
 "$(INTDIR)\admndemo.pch"


!ELSEIF  "$(CFG)" == "admndemo - Macintosh Debug"


"$(INTDIR)\results.obj" : $(SOURCE) $(DEP_CPP_RESUL) "$(INTDIR)"\
 "$(INTDIR)\admndemo.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_RESUL) "$(INTDIR)"\
 "$(INTDIR)\admndemo.pch"


!ELSEIF  "$(CFG)" == "admndemo - Power Macintosh Release"


"$(INTDIR)\results.obj" : $(SOURCE) $(DEP_CPP_RESUL) "$(INTDIR)"\
 "$(INTDIR)\admndemo.pch"


!ELSEIF  "$(CFG)" == "admndemo - Power Macintosh Debug"


"$(INTDIR)\results.obj" : $(SOURCE) $(DEP_CPP_RESUL) "$(INTDIR)"\
 "$(INTDIR)\admndemo.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_RESUL) "$(INTDIR)"\
 "$(INTDIR)\admndemo.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\standard.c
DEP_CPP_STAND=\
	".\headers.h"\
	".\admndemo.h"\
	".\child.h"\
	".\dialogs.h"\
	".\errcheck.h"\
	".\execute.h"\
	".\info.h"\
	".\ini.h"\
	".\menu.h"\
	".\resource.h"\
	".\results.h"\
	".\standard.h"\
	

!IF  "$(CFG)" == "admndemo - Win32 Release"


"$(INTDIR)\standard.obj" : $(SOURCE) $(DEP_CPP_STAND) "$(INTDIR)"\
 "$(INTDIR)\admndemo.pch"


!ELSEIF  "$(CFG)" == "admndemo - Win32 Debug"


"$(INTDIR)\standard.obj" : $(SOURCE) $(DEP_CPP_STAND) "$(INTDIR)"\
 "$(INTDIR)\admndemo.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_STAND) "$(INTDIR)"\
 "$(INTDIR)\admndemo.pch"


!ELSEIF  "$(CFG)" == "admndemo - Macintosh Release"


"$(INTDIR)\standard.obj" : $(SOURCE) $(DEP_CPP_STAND) "$(INTDIR)"\
 "$(INTDIR)\admndemo.pch"


!ELSEIF  "$(CFG)" == "admndemo - Macintosh Debug"


"$(INTDIR)\standard.obj" : $(SOURCE) $(DEP_CPP_STAND) "$(INTDIR)"\
 "$(INTDIR)\admndemo.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_STAND) "$(INTDIR)"\
 "$(INTDIR)\admndemo.pch"


!ELSEIF  "$(CFG)" == "admndemo - Power Macintosh Release"


"$(INTDIR)\standard.obj" : $(SOURCE) $(DEP_CPP_STAND) "$(INTDIR)"\
 "$(INTDIR)\admndemo.pch"


!ELSEIF  "$(CFG)" == "admndemo - Power Macintosh Debug"


"$(INTDIR)\standard.obj" : $(SOURCE) $(DEP_CPP_STAND) "$(INTDIR)"\
 "$(INTDIR)\admndemo.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_STAND) "$(INTDIR)"\
 "$(INTDIR)\admndemo.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\admndemo.rc
DEP_RSC_ADMNDE=\
	".\admndemo.ico"\
	".\results.ico"\
	".\connect.ico"\
	".\resource.h"\
	".\dialogs.h"\
	".\execute.h"\
	

!IF  "$(CFG)" == "admndemo - Win32 Release"


"$(INTDIR)\admndemo.res" : $(SOURCE) $(DEP_RSC_ADMNDE) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "admndemo - Win32 Debug"


"$(INTDIR)\admndemo.res" : $(SOURCE) $(DEP_RSC_ADMNDE) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "admndemo - Macintosh Release"


"$(INTDIR)\admndemo.rsc" : $(SOURCE) $(DEP_RSC_ADMNDE) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "admndemo - Macintosh Debug"


"$(INTDIR)\admndemo.rsc" : $(SOURCE) $(DEP_RSC_ADMNDE) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "admndemo - Power Macintosh Release"


"$(INTDIR)\admndemo.rsc" : $(SOURCE) $(DEP_RSC_ADMNDE) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "admndemo - Power Macintosh Debug"


"$(INTDIR)\admndemo.rsc" : $(SOURCE) $(DEP_RSC_ADMNDE) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\admnmac.r

!IF  "$(CFG)" == "admndemo - Win32 Release"

!ELSEIF  "$(CFG)" == "admndemo - Win32 Debug"

!ELSEIF  "$(CFG)" == "admndemo - Macintosh Release"

DEP_MRC_ADMNM=\
	{$(INCLUDE)}"\mrc\types.r"\
	{$(INCLUDE)}"\mrc\balloons.r"\
	".\resource.h"\
	

"$(INTDIR)\admnmac.rsc" : $(SOURCE) $(DEP_MRC_ADMNM) "$(INTDIR)"
   $(MRC) /o"$(INTDIR)/admnmac.rsc" /D "_68K_" /D "_MAC" /D "NDEBUG" /l 0x409\
 /NOLOGO $(SOURCE)


!ELSEIF  "$(CFG)" == "admndemo - Macintosh Debug"

DEP_MRC_ADMNM=\
	{$(INCLUDE)}"\mrc\types.r"\
	{$(INCLUDE)}"\mrc\balloons.r"\
	".\resource.h"\
	

"$(INTDIR)\admnmac.rsc" : $(SOURCE) $(DEP_MRC_ADMNM) "$(INTDIR)"
   $(MRC) /o"$(INTDIR)/admnmac.rsc" /D "_68K_" /D "_MAC" /D "_DEBUG" /l 0x409\
 /NOLOGO $(SOURCE)


!ELSEIF  "$(CFG)" == "admndemo - Power Macintosh Release"

DEP_MRC_ADMNM=\
	{$(INCLUDE)}"\mrc\types.r"\
	{$(INCLUDE)}"\mrc\balloons.r"\
	".\resource.h"\
	

"$(INTDIR)\admnmac.rsc" : $(SOURCE) $(DEP_MRC_ADMNM) "$(INTDIR)"
   $(MRC) /o"$(INTDIR)/admnmac.rsc" /D "_MPPC_" /D "_MAC" /D "NDEBUG" /l 0x409\
 /NOLOGO $(SOURCE)


!ELSEIF  "$(CFG)" == "admndemo - Power Macintosh Debug"

DEP_MRC_ADMNM=\
	{$(INCLUDE)}"\mrc\types.r"\
	{$(INCLUDE)}"\mrc\balloons.r"\
	".\resource.h"\
	

"$(INTDIR)\admnmac.rsc" : $(SOURCE) $(DEP_MRC_ADMNM) "$(INTDIR)"
   $(MRC) /o"$(INTDIR)/admnmac.rsc" /D "_MPPC_" /D "_MAC" /D "_DEBUG" /l 0x409\
 /NOLOGO $(SOURCE)


!ENDIF 

# End Source File
# End Target
# End Project
################################################################################
