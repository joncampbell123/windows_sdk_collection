# Microsoft Visual C++ Generated NMAKE File, Format Version 40000
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101
# TARGTYPE "Power Macintosh Application" 0x0401
# TARGTYPE "Macintosh Application" 0x0301

!IF "$(CFG)" == ""
CFG=Generic - Win32 Debug
!MESSAGE No configuration specified.  Defaulting to Generic - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "Generic - Win32 Release" && "$(CFG)" !=\
 "Generic - Win32 Debug" && "$(CFG)" != "Generic - Macintosh Release" &&\
 "$(CFG)" != "Generic - Macintosh Debug" && "$(CFG)" !=\
 "Generic - Power Macintosh Release" && "$(CFG)" !=\
 "Generic - Power Macintosh Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "generic.mak" CFG="Generic - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Generic - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "Generic - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE "Generic - Macintosh Release" (based on "Macintosh Application")
!MESSAGE "Generic - Macintosh Debug" (based on "Macintosh Application")
!MESSAGE "Generic - Power Macintosh Release" (based on\
 "Power Macintosh Application")
!MESSAGE "Generic - Power Macintosh Debug" (based on\
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
# PROP Target_Last_Scanned "Generic - Power Macintosh Debug"

!IF  "$(CFG)" == "Generic - Win32 Release"

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

ALL : "$(OUTDIR)\generic.exe" "$(OUTDIR)\generic.hlp"

CLEAN : 
	-@erase ".\Release\generic.exe"
	-@erase ".\Release\stdwin.obj"
	-@erase ".\Release\generic.obj"
	-@erase ".\Release\generic.pch"
	-@erase ".\Release\generic.res"
	-@erase ".\Release\generic.hlp"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /Yu"stdwin.h" /c
CPP_PROJ=/nologo /ML /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS"\
 /Fp"$(INTDIR)/generic.pch" /Yu"stdwin.h" /Fo"$(INTDIR)/" /c 
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
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/generic.res" /d "NDEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/generic.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib version.lib /nologo /subsystem:windows /machine:I386
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib version.lib /nologo /subsystem:windows /incremental:no\
 /pdb:"$(OUTDIR)/generic.pdb" /machine:I386 /out:"$(OUTDIR)/generic.exe" 
LINK32_OBJS= \
	"$(INTDIR)/stdwin.obj" \
	"$(INTDIR)/generic.obj" \
	"$(INTDIR)/generic.res"

"$(OUTDIR)\generic.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "Generic - Win32 Debug"

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

ALL : "$(OUTDIR)\generic.exe" "$(OUTDIR)\generic.hlp"

CLEAN : 
	-@erase ".\Debug\vc40.pdb"
	-@erase ".\Debug\generic.pch"
	-@erase ".\Debug\generic.exe"
	-@erase ".\Debug\stdwin.obj"
	-@erase ".\Debug\generic.obj"
	-@erase ".\Debug\generic.res"
	-@erase ".\Debug\generic.ilk"
	-@erase ".\Debug\generic.pdb"
	-@erase ".\Debug\generic.hlp"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /Yu"stdwin.h" /c
CPP_PROJ=/nologo /MLd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /Fp"$(INTDIR)/generic.pch" /Yu"stdwin.h" /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
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
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/generic.res" /d "_DEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/generic.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib version.lib /nologo /subsystem:windows /debug /machine:I386
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib version.lib /nologo /subsystem:windows /incremental:yes\
 /pdb:"$(OUTDIR)/generic.pdb" /debug /machine:I386 /out:"$(OUTDIR)/generic.exe" 
LINK32_OBJS= \
	"$(INTDIR)/stdwin.obj" \
	"$(INTDIR)/generic.obj" \
	"$(INTDIR)/generic.res"

"$(OUTDIR)\generic.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "Generic - Macintosh Release"

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

ALL : "$(OUTDIR)\generic.exe" "$(OUTDIR)\generic.trg" ".\gen_mac.HLP"

CLEAN : 
	-@erase ".\MacRel\generic.trg"
	-@erase ".\MacRel\generic.exe"
	-@erase ".\MacRel\stdwin.obj"
	-@erase ".\MacRel\generic.obj"
	-@erase ".\MacRel\generic.pch"
	-@erase ".\MacRel\wgen.rsc"
	-@erase ".\MacRel\generic.rsc"
	-@erase ".\gen_mac.HLP"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

MTL=mktyplib.exe
# ADD BASE MTL /nologo /D "NDEBUG" /mac
# ADD MTL /nologo /D "NDEBUG" /mac
MTL_PROJ=/nologo /D "NDEBUG" /mac 
CPP=cl.exe
# ADD BASE CPP /nologo /AL /Q68s /W3 /GX /O2 /D "_WINDOWS" /D "_MAC" /D "_68K_" /D "WIN32" /D "NDEBUG" /YX /c
# ADD CPP /nologo /AL /Q68s /W3 /GX /O2 /D "_WINDOWS" /D "_MAC" /D "_68K_" /D "WIN32" /D "NDEBUG" /Yu"stdwin.h" /c
CPP_PROJ=/nologo /AL /Q68s /W3 /GX /O2 /D "_WINDOWS" /D "_MAC" /D "_68K_" /D\
 "WIN32" /D "NDEBUG" /Fp"$(INTDIR)/generic.pch" /Yu"stdwin.h" /Fo"$(INTDIR)/" /c\
 
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
RSC_PROJ=/l 0x409 /r /m /fo"$(INTDIR)/generic.rsc" /d "_MAC" /d "_68K_" /d\
 "NDEBUG" 
MRC=mrc.exe
# ADD BASE MRC /D "_68K_" /D "_MAC" /D "NDEBUG" /NOLOGO
# ADD MRC /D "_68K_" /D "_MAC" /D "NDEBUG" /NOLOGO
MRC_PROJ=/D "_68K_" /D "_MAC" /D "NDEBUG" /l 0x409 /NOLOGO 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/generic.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 wlm.lib /nologo /MAC:bundle /machine:M68K
# ADD LINK32 /nologo /MAC:bundle /MAC:creator="WGEN" /machine:M68K
LINK32_FLAGS=/nologo /MAC:bundle /MAC:type="APPL" /MAC:creator="WGEN"\
 /pdb:"$(OUTDIR)/generic.pdb" /machine:M68K /out:"$(OUTDIR)/generic.exe" 
LINK32_OBJS= \
	"$(INTDIR)/stdwin.obj" \
	"$(INTDIR)/generic.obj" \
	"$(INTDIR)/wgen.rsc" \
	"$(INTDIR)/generic.rsc"

"$(OUTDIR)\generic.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

MFILE32=mfile.exe
# ADD BASE MFILE32 COPY /NOLOGO
# ADD MFILE32 COPY /NOLOGO
MFILE32_FLAGS=COPY /NOLOGO 
MFILE32_FILES= \
	"$(OUTDIR)/generic.exe"

"$(OUTDIR)\generic.trg" : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\MacRel\generic.exe\
 "$(MFILE32_DEST):generic.exe">"$(OUTDIR)\generic.trg"

DOWNLOAD : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\MacRel\generic.exe\
 "$(MFILE32_DEST):generic.exe">"$(OUTDIR)\generic.trg"

!ELSEIF  "$(CFG)" == "Generic - Macintosh Debug"

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

ALL : "$(OUTDIR)\generic.exe" "$(OUTDIR)\generic.trg" ".\gen_mac.HLP"

CLEAN : 
	-@erase ".\MacDbg\vc40.pdb"
	-@erase ".\MacDbg\generic.pch"
	-@erase ".\MacDbg\generic.pdb"
	-@erase ".\MacDbg\stdwin.obj"
	-@erase ".\MacDbg\generic.obj"
	-@erase ".\MacDbg\generic.rsc"
	-@erase ".\MacDbg\wgen.rsc"
	-@erase ".\MacDbg\generic.trg"
	-@erase ".\MacDbg\generic.exe"
	-@erase ".\gen_mac.HLP"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

MTL=mktyplib.exe
# ADD BASE MTL /nologo /D "_DEBUG" /mac
# ADD MTL /nologo /D "_DEBUG" /mac
MTL_PROJ=/nologo /D "_DEBUG" /mac 
CPP=cl.exe
# ADD BASE CPP /nologo /AL /Q68s /Q68m /W3 /GX /Zi /Od /D "_WINDOWS" /D "_MAC" /D "_68K_" /D "WIN32" /D "_DEBUG" /YX /c
# ADD CPP /nologo /AL /Q68s /Q68m /W3 /GX /Zi /Od /D "_WINDOWS" /D "_MAC" /D "_68K_" /D "WIN32" /D "_DEBUG" /Yu"stdwin.h" /c
CPP_PROJ=/nologo /AL /Q68s /Q68m /W3 /GX /Zi /Od /D "_WINDOWS" /D "_MAC" /D\
 "_68K_" /D "WIN32" /D "_DEBUG" /Fp"$(INTDIR)/generic.pch" /Yu"stdwin.h"\
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
RSC_PROJ=/l 0x409 /r /m /fo"$(INTDIR)/generic.rsc" /d "_MAC" /d "_68K_" /d\
 "_DEBUG" 
MRC=mrc.exe
# ADD BASE MRC /D "_68K_" /D "_MAC" /D "_DEBUG" /NOLOGO
# ADD MRC /D "_68K_" /D "_MAC" /D "_DEBUG" /NOLOGO
MRC_PROJ=/D "_68K_" /D "_MAC" /D "_DEBUG" /l 0x409 /NOLOGO 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/generic.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 wlm.lib /nologo /MAC:bundle /debug /machine:M68K
# ADD LINK32 /nologo /MAC:bundle /MAC:creator="WGEN" /debug /machine:M68K
LINK32_FLAGS=/nologo /MAC:bundle /MAC:type="APPL" /MAC:creator="WGEN"\
 /pdb:"$(OUTDIR)/generic.pdb" /debug /machine:M68K /out:"$(OUTDIR)/generic.exe" 
LINK32_OBJS= \
	"$(INTDIR)/stdwin.obj" \
	"$(INTDIR)/generic.obj" \
	"$(INTDIR)/generic.rsc" \
	"$(INTDIR)/wgen.rsc"

"$(OUTDIR)\generic.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

MFILE32=mfile.exe
# ADD BASE MFILE32 COPY /NOLOGO
# ADD MFILE32 COPY /NOLOGO
MFILE32_FLAGS=COPY /NOLOGO 
MFILE32_FILES= \
	"$(OUTDIR)/generic.exe"

"$(OUTDIR)\generic.trg" : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\MacDbg\generic.exe\
 "$(MFILE32_DEST):generic.exe">"$(OUTDIR)\generic.trg"

DOWNLOAD : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\MacDbg\generic.exe\
 "$(MFILE32_DEST):generic.exe">"$(OUTDIR)\generic.trg"

!ELSEIF  "$(CFG)" == "Generic - Power Macintosh Release"

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

ALL : "$(OUTDIR)\generic.exe" "$(OUTDIR)\generic.trg" ".\gen_mac.HLP"

CLEAN : 
	-@erase ".\PMacRel\generic.trg"
	-@erase ".\PMacRel\generic.exe"
	-@erase ".\PMacRel\generic.obj"
	-@erase ".\PMacRel\generic.pch"
	-@erase ".\PMacRel\stdwin.obj"
	-@erase ".\PMacRel\wgen.rsc"
	-@erase ".\PMacRel\generic.rsc"
	-@erase ".\gen_mac.HLP"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

MTL=mktyplib.exe
# ADD BASE MTL /nologo /D "NDEBUG"
# ADD MTL /nologo /D "NDEBUG"
MTL_PROJ=/nologo /D "NDEBUG" 
CPP=cl.exe
# ADD BASE CPP /nologo /W3 /GX /O2 /D "_MAC" /D "_MPPC_" /D "_WINDOWS" /D "WIN32" /D "NDEBUG" /YX /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "_MAC" /D "_MPPC_" /D "_WINDOWS" /D "WIN32" /D "_WLMDLL" /Yu"stdwin.h" /c
CPP_PROJ=/nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "_MAC" /D "_MPPC_" /D\
 "_WINDOWS" /D "WIN32" /D "_WLMDLL" /Fp"$(INTDIR)/generic.pch" /Yu"stdwin.h"\
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
RSC_PROJ=/l 0x409 /r /m /fo"$(INTDIR)/generic.rsc" /d "_MAC" /d "_MPPC_" /d\
 "NDEBUG" 
MRC=mrc.exe
# ADD BASE MRC /D "_MPPC_" /D "_MAC" /D "NDEBUG" /NOLOGO
# ADD MRC /D "_MPPC_" /D "_MAC" /D "NDEBUG" /NOLOGO
MRC_PROJ=/D "_MPPC_" /D "_MAC" /D "NDEBUG" /l 0x409 /NOLOGO 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/generic.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 cfrg.rsc /nologo /mac:bundle /machine:MPPC
# ADD LINK32 /nologo /mac:bundle /mac:creator="WGEN" /machine:MPPC
LINK32_FLAGS=/nologo /mac:bundle /mac:type="APPL" /mac:creator="WGEN"\
 /incremental:no /pdb:"$(OUTDIR)/generic.pdb" /machine:MPPC\
 /out:"$(OUTDIR)/generic.exe" 
LINK32_OBJS= \
	"$(INTDIR)/generic.obj" \
	"$(INTDIR)/stdwin.obj" \
	"$(INTDIR)/wgen.rsc" \
	"$(INTDIR)/generic.rsc"

"$(OUTDIR)\generic.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

MFILE32=mfile.exe
# ADD BASE MFILE32 COPY /NOLOGO
# ADD MFILE32 COPY /NOLOGO
MFILE32_FLAGS=COPY /NOLOGO 
MFILE32_FILES= \
	"$(OUTDIR)/generic.exe"

"$(OUTDIR)\generic.trg" : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\PMacRel\generic.exe\
 "$(MFILE32_DEST):generic.exe">"$(OUTDIR)\generic.trg"

DOWNLOAD : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\PMacRel\generic.exe\
 "$(MFILE32_DEST):generic.exe">"$(OUTDIR)\generic.trg"

!ELSEIF  "$(CFG)" == "Generic - Power Macintosh Debug"

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

ALL : "$(OUTDIR)\generic.exe" "$(OUTDIR)\generic.trg" ".\gen_mac.HLP"

CLEAN : 
	-@erase ".\PMacDbg\vc40.pdb"
	-@erase ".\PMacDbg\generic.pch"
	-@erase ".\PMacDbg\generic.ilk"
	-@erase ".\PMacDbg\generic.obj"
	-@erase ".\PMacDbg\stdwin.obj"
	-@erase ".\PMacDbg\generic.rsc"
	-@erase ".\PMacDbg\wgen.rsc"
	-@erase ".\PMacDbg\generic.pdb"
	-@erase ".\PMacDbg\generic.trg"
	-@erase ".\PMacDbg\generic.exe"
	-@erase ".\gen_mac.HLP"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

MTL=mktyplib.exe
# ADD BASE MTL /nologo /D "_DEBUG"
# ADD MTL /nologo /D "_DEBUG"
MTL_PROJ=/nologo /D "_DEBUG" 
CPP=cl.exe
# ADD BASE CPP /nologo /W3 /GX /Zi /Od /D "_MAC" /D "_MPPC_" /D "_WINDOWS" /D "WIN32" /D "_DEBUG" /YX /c
# ADD CPP /nologo /MDd /W3 /GX /Zi /Od /D "_DEBUG" /D "_MAC" /D "_MPPC_" /D "_WINDOWS" /D "WIN32" /D "_WLMDLL" /Yu"stdwin.h" /c
CPP_PROJ=/nologo /MDd /W3 /GX /Zi /Od /D "_DEBUG" /D "_MAC" /D "_MPPC_" /D\
 "_WINDOWS" /D "WIN32" /D "_WLMDLL" /Fp"$(INTDIR)/generic.pch" /Yu"stdwin.h"\
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
RSC_PROJ=/l 0x409 /r /m /fo"$(INTDIR)/generic.rsc" /d "_MAC" /d "_MPPC_" /d\
 "_DEBUG" 
MRC=mrc.exe
# ADD BASE MRC /D "_MPPC_" /D "_MAC" /D "_DEBUG" /NOLOGO
# ADD MRC /D "_MPPC_" /D "_MAC" /D "_DEBUG" /NOLOGO
MRC_PROJ=/D "_MPPC_" /D "_MAC" /D "_DEBUG" /l 0x409 /NOLOGO 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/generic.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 cfrg.rsc /nologo /mac:bundle /debug /machine:MPPC
# ADD LINK32 /nologo /mac:bundle /mac:creator="WGEN" /debug /machine:MPPC
LINK32_FLAGS=/nologo /mac:bundle /mac:type="APPL" /mac:creator="WGEN"\
 /incremental:yes /pdb:"$(OUTDIR)/generic.pdb" /debug /machine:MPPC\
 /out:"$(OUTDIR)/generic.exe" 
LINK32_OBJS= \
	"$(INTDIR)/generic.obj" \
	"$(INTDIR)/stdwin.obj" \
	"$(INTDIR)/generic.rsc" \
	"$(INTDIR)/wgen.rsc"

"$(OUTDIR)\generic.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

MFILE32=mfile.exe
# ADD BASE MFILE32 COPY /NOLOGO
# ADD MFILE32 COPY /NOLOGO
MFILE32_FLAGS=COPY /NOLOGO 
MFILE32_FILES= \
	"$(OUTDIR)/generic.exe"

"$(OUTDIR)\generic.trg" : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\PMacDbg\generic.exe\
 "$(MFILE32_DEST):generic.exe">"$(OUTDIR)\generic.trg"

DOWNLOAD : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\PMacDbg\generic.exe\
 "$(MFILE32_DEST):generic.exe">"$(OUTDIR)\generic.trg"

!ENDIF 

################################################################################
# Begin Target

# Name "Generic - Win32 Release"
# Name "Generic - Win32 Debug"
# Name "Generic - Macintosh Release"
# Name "Generic - Macintosh Debug"
# Name "Generic - Power Macintosh Release"
# Name "Generic - Power Macintosh Debug"

!IF  "$(CFG)" == "Generic - Win32 Release"

!ELSEIF  "$(CFG)" == "Generic - Win32 Debug"

!ELSEIF  "$(CFG)" == "Generic - Macintosh Release"

!ELSEIF  "$(CFG)" == "Generic - Macintosh Debug"

!ELSEIF  "$(CFG)" == "Generic - Power Macintosh Release"

!ELSEIF  "$(CFG)" == "Generic - Power Macintosh Debug"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\generic.rc
DEP_RSC_GENER=\
	".\res\generic.ico"\
	".\res\small.ico"\
	".\generic.h"\
	

!IF  "$(CFG)" == "Generic - Win32 Release"


"$(INTDIR)\generic.res" : $(SOURCE) $(DEP_RSC_GENER) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Generic - Win32 Debug"


"$(INTDIR)\generic.res" : $(SOURCE) $(DEP_RSC_GENER) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Generic - Macintosh Release"


"$(INTDIR)\generic.rsc" : $(SOURCE) $(DEP_RSC_GENER) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Generic - Macintosh Debug"


"$(INTDIR)\generic.rsc" : $(SOURCE) $(DEP_RSC_GENER) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Generic - Power Macintosh Release"


"$(INTDIR)\generic.rsc" : $(SOURCE) $(DEP_RSC_GENER) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Generic - Power Macintosh Debug"


"$(INTDIR)\generic.rsc" : $(SOURCE) $(DEP_RSC_GENER) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\generic.hpj

!IF  "$(CFG)" == "Generic - Win32 Release"

# Begin Custom Build - Building the Intel help file....
OutDir=.\Release
InputPath=.\generic.hpj
InputName=generic

"$(OutDir)\$(InputName).hlp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   makehelp

# End Custom Build

!ELSEIF  "$(CFG)" == "Generic - Win32 Debug"

# Begin Custom Build - Building the Intel help file....
OutDir=.\Debug
InputPath=.\generic.hpj
InputName=generic

"$(OutDir)\$(InputName).hlp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   makehelp

# End Custom Build

!ELSEIF  "$(CFG)" == "Generic - Macintosh Release"

!ELSEIF  "$(CFG)" == "Generic - Macintosh Debug"

!ELSEIF  "$(CFG)" == "Generic - Power Macintosh Release"

!ELSEIF  "$(CFG)" == "Generic - Power Macintosh Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\generic.c

!IF  "$(CFG)" == "Generic - Win32 Release"

DEP_CPP_GENERI=\
	".\generic.h"\
	".\stdwin.h"\
	

"$(INTDIR)\generic.obj" : $(SOURCE) $(DEP_CPP_GENERI) "$(INTDIR)"\
 "$(INTDIR)\generic.pch"


!ELSEIF  "$(CFG)" == "Generic - Win32 Debug"

DEP_CPP_GENERI=\
	".\generic.h"\
	".\stdwin.h"\
	

"$(INTDIR)\generic.obj" : $(SOURCE) $(DEP_CPP_GENERI) "$(INTDIR)"\
 "$(INTDIR)\generic.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_GENERI) "$(INTDIR)"\
 "$(INTDIR)\generic.pch"


!ELSEIF  "$(CFG)" == "Generic - Macintosh Release"

DEP_CPP_GENERI=\
	".\generic.h"\
	".\stdwin.h"\
	

"$(INTDIR)\generic.obj" : $(SOURCE) $(DEP_CPP_GENERI) "$(INTDIR)"\
 "$(INTDIR)\generic.pch"


!ELSEIF  "$(CFG)" == "Generic - Macintosh Debug"

DEP_CPP_GENERI=\
	".\generic.h"\
	".\stdwin.h"\
	

"$(INTDIR)\generic.obj" : $(SOURCE) $(DEP_CPP_GENERI) "$(INTDIR)"\
 "$(INTDIR)\generic.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_GENERI) "$(INTDIR)"\
 "$(INTDIR)\generic.pch"


!ELSEIF  "$(CFG)" == "Generic - Power Macintosh Release"

DEP_CPP_GENERI=\
	".\generic.h"\
	".\stdwin.h"\
	

"$(INTDIR)\generic.obj" : $(SOURCE) $(DEP_CPP_GENERI) "$(INTDIR)"\
 "$(INTDIR)\generic.pch"


!ELSEIF  "$(CFG)" == "Generic - Power Macintosh Debug"

DEP_CPP_GENERI=\
	".\stdwin.h"\
	".\generic.h"\
	

"$(INTDIR)\generic.obj" : $(SOURCE) $(DEP_CPP_GENERI) "$(INTDIR)"\
 "$(INTDIR)\generic.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_GENERI) "$(INTDIR)"\
 "$(INTDIR)\generic.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\wgen.r

!IF  "$(CFG)" == "Generic - Win32 Release"

!ELSEIF  "$(CFG)" == "Generic - Win32 Debug"

!ELSEIF  "$(CFG)" == "Generic - Macintosh Release"

DEP_MRC_WGEN_=\
	{$(INCLUDE)}"\mrc\TYPES.R"\
	{$(INCLUDE)}"\mrc\BALLOONS.R"\
	

"$(INTDIR)\wgen.rsc" : $(SOURCE) $(DEP_MRC_WGEN_) "$(INTDIR)"
   $(MRC) /o"$(INTDIR)/wgen.rsc" /D "_68K_" /D "_MAC" /D "NDEBUG" /l 0x409\
 /NOLOGO $(SOURCE)


!ELSEIF  "$(CFG)" == "Generic - Macintosh Debug"

DEP_MRC_WGEN_=\
	{$(INCLUDE)}"\mrc\TYPES.R"\
	{$(INCLUDE)}"\mrc\BALLOONS.R"\
	

"$(INTDIR)\wgen.rsc" : $(SOURCE) $(DEP_MRC_WGEN_) "$(INTDIR)"
   $(MRC) /o"$(INTDIR)/wgen.rsc" /D "_68K_" /D "_MAC" /D "_DEBUG" /l 0x409\
 /NOLOGO $(SOURCE)


!ELSEIF  "$(CFG)" == "Generic - Power Macintosh Release"

DEP_MRC_WGEN_=\
	{$(INCLUDE)}"\mrc\TYPES.R"\
	{$(INCLUDE)}"\mrc\BALLOONS.R"\
	

"$(INTDIR)\wgen.rsc" : $(SOURCE) $(DEP_MRC_WGEN_) "$(INTDIR)"
   $(MRC) /o"$(INTDIR)/wgen.rsc" /D "_MPPC_" /D "_MAC" /D "NDEBUG" /l 0x409\
 /NOLOGO $(SOURCE)


!ELSEIF  "$(CFG)" == "Generic - Power Macintosh Debug"

DEP_MRC_WGEN_=\
	{$(INCLUDE)}"\mrc\TYPES.R"\
	{$(INCLUDE)}"\mrc\BALLOONS.R"\
	

"$(INTDIR)\wgen.rsc" : $(SOURCE) $(DEP_MRC_WGEN_) "$(INTDIR)"
   $(MRC) /o"$(INTDIR)/wgen.rsc" /D "_MPPC_" /D "_MAC" /D "_DEBUG" /l 0x409\
 /NOLOGO $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\gen_mac.hpj

!IF  "$(CFG)" == "Generic - Win32 Release"

!ELSEIF  "$(CFG)" == "Generic - Win32 Debug"

!ELSEIF  "$(CFG)" == "Generic - Macintosh Release"

# Begin Custom Build - Building the Mac help file....
RemoteDir=
TargetName=generic
InputPath=.\gen_mac.hpj
InputName=gen_mac

".\$(InputName).HLP" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   makehelp MAC ":$(RemoteDir):$(TargetName) Help"

# End Custom Build

!ELSEIF  "$(CFG)" == "Generic - Macintosh Debug"

# Begin Custom Build - Building the Mac help file....
RemoteDir=
TargetName=generic
InputPath=.\gen_mac.hpj
InputName=gen_mac

".\$(InputName).HLP" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   makehelp MAC ":$(RemoteDir):$(TargetName) Help"

# End Custom Build

!ELSEIF  "$(CFG)" == "Generic - Power Macintosh Release"

# Begin Custom Build - Building the Mac help file....
RemoteDir=
TargetName=generic
InputPath=.\gen_mac.hpj
InputName=gen_mac

".\$(InputName).HLP" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   makehelp MAC ":$(RemoteDir):$(TargetName) Help"

# End Custom Build

!ELSEIF  "$(CFG)" == "Generic - Power Macintosh Debug"

# Begin Custom Build - Building the Mac help file....
RemoteDir=
TargetName=generic
InputPath=.\gen_mac.hpj
InputName=gen_mac

".\$(InputName).HLP" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   makehelp MAC ":$(RemoteDir):$(TargetName) Help"

# End Custom Build

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\stdwin.c
DEP_CPP_STDWI=\
	".\stdwin.h"\
	

!IF  "$(CFG)" == "Generic - Win32 Release"

# ADD CPP /Yc"stdwin.h"

BuildCmds= \
	$(CPP) /nologo /ML /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS"\
 /Fp"$(INTDIR)/generic.pch" /Yc"stdwin.h" /Fo"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\stdwin.obj" : $(SOURCE) $(DEP_CPP_STDWI) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\generic.pch" : $(SOURCE) $(DEP_CPP_STDWI) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "Generic - Win32 Debug"

# ADD CPP /Yc"stdwin.h"

BuildCmds= \
	$(CPP) /nologo /MLd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /Fp"$(INTDIR)/generic.pch" /Yc"stdwin.h" /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c\
 $(SOURCE) \
	

"$(INTDIR)\stdwin.obj" : $(SOURCE) $(DEP_CPP_STDWI) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_STDWI) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\generic.pch" : $(SOURCE) $(DEP_CPP_STDWI) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "Generic - Macintosh Release"

# ADD CPP /Yc"stdwin.h"

BuildCmds= \
	$(CPP) /nologo /AL /Q68s /W3 /GX /O2 /D "_WINDOWS" /D "_MAC" /D "_68K_" /D\
 "WIN32" /D "NDEBUG" /Fp"$(INTDIR)/generic.pch" /Yc"stdwin.h" /Fo"$(INTDIR)/" /c\
 $(SOURCE) \
	

"$(INTDIR)\stdwin.obj" : $(SOURCE) $(DEP_CPP_STDWI) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\generic.pch" : $(SOURCE) $(DEP_CPP_STDWI) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "Generic - Macintosh Debug"

# ADD CPP /Yc"stdwin.h"

BuildCmds= \
	$(CPP) /nologo /AL /Q68s /Q68m /W3 /GX /Zi /Od /D "_WINDOWS" /D "_MAC" /D\
 "_68K_" /D "WIN32" /D "_DEBUG" /Fp"$(INTDIR)/generic.pch" /Yc"stdwin.h"\
 /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\stdwin.obj" : $(SOURCE) $(DEP_CPP_STDWI) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_STDWI) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\generic.pch" : $(SOURCE) $(DEP_CPP_STDWI) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "Generic - Power Macintosh Release"

# ADD CPP /Yc"stdwin.h"

BuildCmds= \
	$(CPP) /nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "_MAC" /D "_MPPC_" /D "_WINDOWS"\
 /D "WIN32" /D "_WLMDLL" /Fp"$(INTDIR)/generic.pch" /Yc"stdwin.h"\
 /Fo"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\stdwin.obj" : $(SOURCE) $(DEP_CPP_STDWI) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\generic.pch" : $(SOURCE) $(DEP_CPP_STDWI) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "Generic - Power Macintosh Debug"

# ADD CPP /Yc"stdwin.h"

BuildCmds= \
	$(CPP) /nologo /MDd /W3 /GX /Zi /Od /D "_DEBUG" /D "_MAC" /D "_MPPC_" /D\
 "_WINDOWS" /D "WIN32" /D "_WLMDLL" /Fp"$(INTDIR)/generic.pch" /Yc"stdwin.h"\
 /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\stdwin.obj" : $(SOURCE) $(DEP_CPP_STDWI) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_STDWI) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\generic.pch" : $(SOURCE) $(DEP_CPP_STDWI) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
# End Target
# End Project
################################################################################
