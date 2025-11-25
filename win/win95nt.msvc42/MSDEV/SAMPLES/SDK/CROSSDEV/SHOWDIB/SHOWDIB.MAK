# Microsoft Visual C++ Generated NMAKE File, Format Version 40000
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101
# TARGTYPE "Power Macintosh Application" 0x0401
# TARGTYPE "Macintosh Application" 0x0301

!IF "$(CFG)" == ""
CFG=ShowDib - Win32 Debug
!MESSAGE No configuration specified.  Defaulting to ShowDib - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "ShowDib - Win32 Release" && "$(CFG)" !=\
 "ShowDib - Win32 Debug" && "$(CFG)" != "ShowDib - Macintosh Release" &&\
 "$(CFG)" != "ShowDib - Macintosh Debug" && "$(CFG)" !=\
 "ShowDib - Power Macintosh Release" && "$(CFG)" !=\
 "ShowDib - Power Macintosh Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "showdib.mak" CFG="ShowDib - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "ShowDib - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "ShowDib - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE "ShowDib - Macintosh Release" (based on "Macintosh Application")
!MESSAGE "ShowDib - Macintosh Debug" (based on "Macintosh Application")
!MESSAGE "ShowDib - Power Macintosh Release" (based on\
 "Power Macintosh Application")
!MESSAGE "ShowDib - Power Macintosh Debug" (based on\
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
# PROP Target_Last_Scanned "ShowDib - Power Macintosh Debug"

!IF  "$(CFG)" == "ShowDib - Win32 Release"

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

ALL : "$(OUTDIR)\showdib.exe"

CLEAN : 
	-@erase ".\Release\showdib.exe"
	-@erase ".\Release\drawdib.obj"
	-@erase ".\Release\showdib.pch"
	-@erase ".\Release\print.obj"
	-@erase ".\Release\stdwin.obj"
	-@erase ".\Release\showdib.obj"
	-@erase ".\Release\dib.obj"
	-@erase ".\Release\dlgopen.obj"
	-@erase ".\Release\showdib.res"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /Yu"Stdwin.h" /c
CPP_PROJ=/nologo /ML /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS"\
 /Fp"$(INTDIR)/showdib.pch" /Yu"Stdwin.h" /Fo"$(INTDIR)/" /c 
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
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/showdib.res" /d "NDEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/showdib.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib /nologo /subsystem:windows /incremental:no\
 /pdb:"$(OUTDIR)/showdib.pdb" /machine:I386 /out:"$(OUTDIR)/showdib.exe" 
LINK32_OBJS= \
	"$(INTDIR)/drawdib.obj" \
	"$(INTDIR)/print.obj" \
	"$(INTDIR)/stdwin.obj" \
	"$(INTDIR)/showdib.obj" \
	"$(INTDIR)/dib.obj" \
	"$(INTDIR)/dlgopen.obj" \
	"$(INTDIR)/showdib.res"

"$(OUTDIR)\showdib.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "ShowDib - Win32 Debug"

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

ALL : "$(OUTDIR)\showdib.exe"

CLEAN : 
	-@erase ".\Debug\vc40.pdb"
	-@erase ".\Debug\showdib.pch"
	-@erase ".\Debug\showdib.exe"
	-@erase ".\Debug\dlgopen.obj"
	-@erase ".\Debug\drawdib.obj"
	-@erase ".\Debug\print.obj"
	-@erase ".\Debug\dib.obj"
	-@erase ".\Debug\showdib.obj"
	-@erase ".\Debug\stdwin.obj"
	-@erase ".\Debug\showdib.res"
	-@erase ".\Debug\showdib.ilk"
	-@erase ".\Debug\showdib.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /Yu"Stdwin.h" /c
CPP_PROJ=/nologo /MLd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /Fp"$(INTDIR)/showdib.pch" /Yu"Stdwin.h" /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
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
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/showdib.res" /d "_DEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/showdib.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib /nologo /subsystem:windows /incremental:yes\
 /pdb:"$(OUTDIR)/showdib.pdb" /debug /machine:I386 /out:"$(OUTDIR)/showdib.exe" 
LINK32_OBJS= \
	"$(INTDIR)/dlgopen.obj" \
	"$(INTDIR)/drawdib.obj" \
	"$(INTDIR)/print.obj" \
	"$(INTDIR)/dib.obj" \
	"$(INTDIR)/showdib.obj" \
	"$(INTDIR)/stdwin.obj" \
	"$(INTDIR)/showdib.res"

"$(OUTDIR)\showdib.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "ShowDib - Macintosh Release"

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

ALL : "$(OUTDIR)\showdib.exe" "$(OUTDIR)\showdib.trg"

CLEAN : 
	-@erase ".\MacRel\showdib.trg"
	-@erase ".\MacRel\showdib.exe"
	-@erase ".\MacRel\drawdib.obj"
	-@erase ".\MacRel\showdib.pch"
	-@erase ".\MacRel\stdwin.obj"
	-@erase ".\MacRel\showdib.obj"
	-@erase ".\MacRel\print.obj"
	-@erase ".\MacRel\dlgopen.obj"
	-@erase ".\MacRel\dib.obj"
	-@erase ".\MacRel\showdib.rsc"
	-@erase ".\MacRel\sdib.rsc"

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
 "WIN32" /D "NDEBUG" /Fp"$(INTDIR)/showdib.pch" /Yu"Stdwin.h" /Fo"$(INTDIR)/" /c\
 
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
RSC_PROJ=/l 0x409 /r /m /fo"$(INTDIR)/showdib.rsc" /d "_MAC" /d "_68K_" /d\
 "NDEBUG" 
MRC=mrc.exe
# ADD BASE MRC /D "_68K_" /D "_MAC" /D "NDEBUG" /NOLOGO
# ADD MRC /D "_68K_" /D "_MAC" /D "NDEBUG" /NOLOGO
MRC_PROJ=/D "_68K_" /D "_MAC" /D "NDEBUG" /l 0x409 /NOLOGO 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/showdib.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 wlm.lib /nologo /MAC:bundle /machine:M68K
# ADD LINK32 wlm.lib /nologo /MAC:bundle /MAC:creator="SDIB" /machine:M68K
LINK32_FLAGS=wlm.lib /nologo /MAC:bundle /MAC:type="APPL" /MAC:creator="SDIB"\
 /pdb:"$(OUTDIR)/showdib.pdb" /machine:M68K /out:"$(OUTDIR)/showdib.exe" 
LINK32_OBJS= \
	"$(INTDIR)/drawdib.obj" \
	"$(INTDIR)/stdwin.obj" \
	"$(INTDIR)/showdib.obj" \
	"$(INTDIR)/print.obj" \
	"$(INTDIR)/dlgopen.obj" \
	"$(INTDIR)/dib.obj" \
	"$(INTDIR)/showdib.rsc" \
	"$(INTDIR)/sdib.rsc"

"$(OUTDIR)\showdib.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

MFILE32=mfile.exe
# ADD BASE MFILE32 COPY /NOLOGO
# ADD MFILE32 COPY /NOLOGO
MFILE32_FLAGS=COPY /NOLOGO 
MFILE32_FILES= \
	"$(OUTDIR)/showdib.exe"

"$(OUTDIR)\showdib.trg" : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\MacRel\showdib.exe\
 "$(MFILE32_DEST):showdib.exe">"$(OUTDIR)\showdib.trg"

DOWNLOAD : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\MacRel\showdib.exe\
 "$(MFILE32_DEST):showdib.exe">"$(OUTDIR)\showdib.trg"

!ELSEIF  "$(CFG)" == "ShowDib - Macintosh Debug"

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

ALL : "$(OUTDIR)\showdib.exe" "$(OUTDIR)\showdib.trg"

CLEAN : 
	-@erase ".\MacDbg\vc40.pdb"
	-@erase ".\MacDbg\showdib.pch"
	-@erase ".\MacDbg\showdib.pdb"
	-@erase ".\MacDbg\stdwin.obj"
	-@erase ".\MacDbg\dib.obj"
	-@erase ".\MacDbg\drawdib.obj"
	-@erase ".\MacDbg\showdib.obj"
	-@erase ".\MacDbg\print.obj"
	-@erase ".\MacDbg\dlgopen.obj"
	-@erase ".\MacDbg\showdib.rsc"
	-@erase ".\MacDbg\sdib.rsc"
	-@erase ".\MacDbg\showdib.trg"
	-@erase ".\MacDbg\showdib.exe"

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
 "_68K_" /D "WIN32" /D "_DEBUG" /Fp"$(INTDIR)/showdib.pch" /Yu"Stdwin.h"\
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
RSC_PROJ=/l 0x409 /r /m /fo"$(INTDIR)/showdib.rsc" /d "_MAC" /d "_68K_" /d\
 "_DEBUG" 
MRC=mrc.exe
# ADD BASE MRC /D "_68K_" /D "_MAC" /D "_DEBUG" /NOLOGO
# ADD MRC /D "_68K_" /D "_MAC" /D "_DEBUG" /NOLOGO
MRC_PROJ=/D "_68K_" /D "_MAC" /D "_DEBUG" /l 0x409 /NOLOGO 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/showdib.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 wlm.lib /nologo /MAC:bundle /debug /machine:M68K
# ADD LINK32 wlm.lib /nologo /MAC:bundle /MAC:creator="SDIB" /debug /machine:M68K
LINK32_FLAGS=wlm.lib /nologo /MAC:bundle /MAC:type="APPL" /MAC:creator="SDIB"\
 /pdb:"$(OUTDIR)/showdib.pdb" /debug /machine:M68K /out:"$(OUTDIR)/showdib.exe" 
LINK32_OBJS= \
	"$(INTDIR)/stdwin.obj" \
	"$(INTDIR)/dib.obj" \
	"$(INTDIR)/drawdib.obj" \
	"$(INTDIR)/showdib.obj" \
	"$(INTDIR)/print.obj" \
	"$(INTDIR)/dlgopen.obj" \
	"$(INTDIR)/showdib.rsc" \
	"$(INTDIR)/sdib.rsc"

"$(OUTDIR)\showdib.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

MFILE32=mfile.exe
# ADD BASE MFILE32 COPY /NOLOGO
# ADD MFILE32 COPY /NOLOGO
MFILE32_FLAGS=COPY /NOLOGO 
MFILE32_FILES= \
	"$(OUTDIR)/showdib.exe"

"$(OUTDIR)\showdib.trg" : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\MacDbg\showdib.exe\
 "$(MFILE32_DEST):showdib.exe">"$(OUTDIR)\showdib.trg"

DOWNLOAD : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\MacDbg\showdib.exe\
 "$(MFILE32_DEST):showdib.exe">"$(OUTDIR)\showdib.trg"

!ELSEIF  "$(CFG)" == "ShowDib - Power Macintosh Release"

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

ALL : "$(OUTDIR)\showdib.exe" "$(OUTDIR)\showdib.trg"

CLEAN : 
	-@erase ".\PMacRel\showdib.trg"
	-@erase ".\PMacRel\showdib.exe"
	-@erase ".\PMacRel\showdib.obj"
	-@erase ".\PMacRel\showdib.pch"
	-@erase ".\PMacRel\dlgopen.obj"
	-@erase ".\PMacRel\dib.obj"
	-@erase ".\PMacRel\print.obj"
	-@erase ".\PMacRel\drawdib.obj"
	-@erase ".\PMacRel\stdwin.obj"
	-@erase ".\PMacRel\sdib.rsc"
	-@erase ".\PMacRel\showdib.rsc"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

MTL=mktyplib.exe
# ADD BASE MTL /nologo /D "NDEBUG"
# ADD MTL /nologo /D "NDEBUG"
MTL_PROJ=/nologo /D "NDEBUG" 
CPP=cl.exe
# ADD BASE CPP /nologo /ML /W3 /GX /O2 /D "_MAC" /D "_MPPC_" /D "_WINDOWS" /D "WIN32" /D "NDEBUG" /YX /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "_MAC" /D "_MPPC_" /D "_WINDOWS" /D "WIN32" /D "_WLMDLL" /Yu"Stdwin.h" /c
CPP_PROJ=/nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "_MAC" /D "_MPPC_" /D\
 "_WINDOWS" /D "WIN32" /D "_WLMDLL" /Fp"$(INTDIR)/showdib.pch" /Yu"Stdwin.h"\
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
RSC_PROJ=/l 0x409 /r /m /fo"$(INTDIR)/showdib.rsc" /d "_MAC" /d "_MPPC_" /d\
 "NDEBUG" 
MRC=mrc.exe
# ADD BASE MRC /D "_MPPC_" /D "_MAC" /D "NDEBUG" /NOLOGO
# ADD MRC /D "_MPPC_" /D "_MAC" /D "NDEBUG" /NOLOGO
MRC_PROJ=/D "_MPPC_" /D "_MAC" /D "NDEBUG" /l 0x409 /NOLOGO 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/showdib.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 cfrg.rsc /nologo /mac:bundle /machine:MPPC
# ADD LINK32 /nologo /mac:bundle /mac:creator="SDIB" /machine:MPPC
LINK32_FLAGS=/nologo /mac:bundle /mac:type="APPL" /mac:creator="SDIB"\
 /incremental:no /pdb:"$(OUTDIR)/showdib.pdb" /machine:MPPC\
 /out:"$(OUTDIR)/showdib.exe" 
LINK32_OBJS= \
	"$(INTDIR)/showdib.obj" \
	"$(INTDIR)/dlgopen.obj" \
	"$(INTDIR)/dib.obj" \
	"$(INTDIR)/print.obj" \
	"$(INTDIR)/drawdib.obj" \
	"$(INTDIR)/stdwin.obj" \
	"$(INTDIR)/sdib.rsc" \
	"$(INTDIR)/showdib.rsc"

"$(OUTDIR)\showdib.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

MFILE32=mfile.exe
# ADD BASE MFILE32 COPY /NOLOGO
# ADD MFILE32 COPY /NOLOGO
MFILE32_FLAGS=COPY /NOLOGO 
MFILE32_FILES= \
	"$(OUTDIR)/showdib.exe"

"$(OUTDIR)\showdib.trg" : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\PMacRel\showdib.exe\
 "$(MFILE32_DEST):showdib.exe">"$(OUTDIR)\showdib.trg"

DOWNLOAD : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\PMacRel\showdib.exe\
 "$(MFILE32_DEST):showdib.exe">"$(OUTDIR)\showdib.trg"

!ELSEIF  "$(CFG)" == "ShowDib - Power Macintosh Debug"

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

ALL : "$(OUTDIR)\showdib.exe" "$(OUTDIR)\showdib.trg"

CLEAN : 
	-@erase ".\PMacDbg\vc40.pdb"
	-@erase ".\PMacDbg\showdib.pch"
	-@erase ".\PMacDbg\showdib.ilk"
	-@erase ".\PMacDbg\drawdib.obj"
	-@erase ".\PMacDbg\dib.obj"
	-@erase ".\PMacDbg\showdib.obj"
	-@erase ".\PMacDbg\dlgopen.obj"
	-@erase ".\PMacDbg\stdwin.obj"
	-@erase ".\PMacDbg\print.obj"
	-@erase ".\PMacDbg\showdib.rsc"
	-@erase ".\PMacDbg\sdib.rsc"
	-@erase ".\PMacDbg\showdib.pdb"
	-@erase ".\PMacDbg\showdib.trg"
	-@erase ".\PMacDbg\showdib.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

MTL=mktyplib.exe
# ADD BASE MTL /nologo /D "_DEBUG"
# ADD MTL /nologo /D "_DEBUG"
MTL_PROJ=/nologo /D "_DEBUG" 
CPP=cl.exe
# ADD BASE CPP /nologo /W3 /GX /Zi /Od /D "_MAC" /D "_MPPC_" /D "_WINDOWS" /D "WIN32" /D "_DEBUG" /YX /c
# ADD CPP /nologo /MDd /Ze /W3 /vd1 /GR- /GX /Zi /Od /D "_DEBUG" /D "_MAC" /D "_MPPC_" /D "_WINDOWS" /D "WIN32" /D "_WLMDLL" /Yu"Stdwin.h" /c
# SUBTRACT CPP /WX /Gf /Gy /X /u /Fr
CPP_PROJ=/nologo /MDd /Ze /W3 /vd1 /GR- /GX /Zi /Od /D "_DEBUG" /D "_MAC" /D\
 "_MPPC_" /D "_WINDOWS" /D "WIN32" /D "_WLMDLL" /Fp"$(INTDIR)/showdib.pch"\
 /Yu"Stdwin.h" /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
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
RSC_PROJ=/l 0x409 /r /m /fo"$(INTDIR)/showdib.rsc" /d "_MAC" /d "_MPPC_" /d\
 "_DEBUG" 
MRC=mrc.exe
# ADD BASE MRC /D "_MPPC_" /D "_MAC" /D "_DEBUG" /NOLOGO
# ADD MRC /D "_MPPC_" /D "_MAC" /D "_DEBUG" /NOLOGO
MRC_PROJ=/D "_MPPC_" /D "_MAC" /D "_DEBUG" /l 0x409 /NOLOGO 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/showdib.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 cfrg.rsc /nologo /mac:bundle /debug /machine:MPPC
# ADD LINK32 /nologo /mac:bundle /mac:creator="SDIB" /debug /machine:MPPC
LINK32_FLAGS=/nologo /mac:bundle /mac:type="APPL" /mac:creator="SDIB"\
 /incremental:yes /pdb:"$(OUTDIR)/showdib.pdb" /debug /machine:MPPC\
 /out:"$(OUTDIR)/showdib.exe" 
LINK32_OBJS= \
	"$(INTDIR)/drawdib.obj" \
	"$(INTDIR)/dib.obj" \
	"$(INTDIR)/showdib.obj" \
	"$(INTDIR)/dlgopen.obj" \
	"$(INTDIR)/stdwin.obj" \
	"$(INTDIR)/print.obj" \
	"$(INTDIR)/showdib.rsc" \
	"$(INTDIR)/sdib.rsc"

"$(OUTDIR)\showdib.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

MFILE32=mfile.exe
# ADD BASE MFILE32 COPY /NOLOGO
# ADD MFILE32 COPY /NOLOGO
MFILE32_FLAGS=COPY /NOLOGO 
MFILE32_FILES= \
	"$(OUTDIR)/showdib.exe"

"$(OUTDIR)\showdib.trg" : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\PMacDbg\showdib.exe\
 "$(MFILE32_DEST):showdib.exe">"$(OUTDIR)\showdib.trg"

DOWNLOAD : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\PMacDbg\showdib.exe\
 "$(MFILE32_DEST):showdib.exe">"$(OUTDIR)\showdib.trg"

!ENDIF 

################################################################################
# Begin Target

# Name "ShowDib - Win32 Release"
# Name "ShowDib - Win32 Debug"
# Name "ShowDib - Macintosh Release"
# Name "ShowDib - Macintosh Debug"
# Name "ShowDib - Power Macintosh Release"
# Name "ShowDib - Power Macintosh Debug"

!IF  "$(CFG)" == "ShowDib - Win32 Release"

!ELSEIF  "$(CFG)" == "ShowDib - Win32 Debug"

!ELSEIF  "$(CFG)" == "ShowDib - Macintosh Release"

!ELSEIF  "$(CFG)" == "ShowDib - Macintosh Debug"

!ELSEIF  "$(CFG)" == "ShowDib - Power Macintosh Release"

!ELSEIF  "$(CFG)" == "ShowDib - Power Macintosh Debug"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\stdwin.c
DEP_CPP_STDWI=\
	".\stdwin.h"\
	

!IF  "$(CFG)" == "ShowDib - Win32 Release"

# ADD CPP /Yc

BuildCmds= \
	$(CPP) /nologo /ML /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS"\
 /Fp"$(INTDIR)/showdib.pch" /Yc /Fo"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\stdwin.obj" : $(SOURCE) $(DEP_CPP_STDWI) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\showdib.pch" : $(SOURCE) $(DEP_CPP_STDWI) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "ShowDib - Win32 Debug"

# ADD CPP /Yc

BuildCmds= \
	$(CPP) /nologo /MLd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /Fp"$(INTDIR)/showdib.pch" /Yc /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\stdwin.obj" : $(SOURCE) $(DEP_CPP_STDWI) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_STDWI) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\showdib.pch" : $(SOURCE) $(DEP_CPP_STDWI) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "ShowDib - Macintosh Release"

# ADD CPP /Yc

BuildCmds= \
	$(CPP) /nologo /AL /Q68s /W3 /GX /O2 /D "_WINDOWS" /D "_MAC" /D "_68K_" /D\
 "WIN32" /D "NDEBUG" /Fp"$(INTDIR)/showdib.pch" /Yc /Fo"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\stdwin.obj" : $(SOURCE) $(DEP_CPP_STDWI) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\showdib.pch" : $(SOURCE) $(DEP_CPP_STDWI) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "ShowDib - Macintosh Debug"

# ADD CPP /Yc

BuildCmds= \
	$(CPP) /nologo /AL /Q68s /Q68m /W3 /GX /Zi /Od /D "_WINDOWS" /D "_MAC" /D\
 "_68K_" /D "WIN32" /D "_DEBUG" /Fp"$(INTDIR)/showdib.pch" /Yc /Fo"$(INTDIR)/"\
 /Fd"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\stdwin.obj" : $(SOURCE) $(DEP_CPP_STDWI) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_STDWI) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\showdib.pch" : $(SOURCE) $(DEP_CPP_STDWI) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "ShowDib - Power Macintosh Release"

# ADD CPP /Yc

BuildCmds= \
	$(CPP) /nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "_MAC" /D "_MPPC_" /D "_WINDOWS"\
 /D "WIN32" /D "_WLMDLL" /Fp"$(INTDIR)/showdib.pch" /Yc /Fo"$(INTDIR)/" /c\
 $(SOURCE) \
	

"$(INTDIR)\stdwin.obj" : $(SOURCE) $(DEP_CPP_STDWI) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\showdib.pch" : $(SOURCE) $(DEP_CPP_STDWI) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "ShowDib - Power Macintosh Debug"

# ADD CPP /Yc

BuildCmds= \
	$(CPP) /nologo /MDd /Ze /W3 /vd1 /GR- /GX /Zi /Od /D "_DEBUG" /D "_MAC" /D\
 "_MPPC_" /D "_WINDOWS" /D "WIN32" /D "_WLMDLL" /Fp"$(INTDIR)/showdib.pch" /Yc\
 /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\stdwin.obj" : $(SOURCE) $(DEP_CPP_STDWI) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_STDWI) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\showdib.pch" : $(SOURCE) $(DEP_CPP_STDWI) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\dlgopen.c
DEP_CPP_DLGOP=\
	".\stdwin.h"\
	".\showdib.h"\
	

!IF  "$(CFG)" == "ShowDib - Win32 Release"


"$(INTDIR)\dlgopen.obj" : $(SOURCE) $(DEP_CPP_DLGOP) "$(INTDIR)"\
 "$(INTDIR)\showdib.pch"


!ELSEIF  "$(CFG)" == "ShowDib - Win32 Debug"


"$(INTDIR)\dlgopen.obj" : $(SOURCE) $(DEP_CPP_DLGOP) "$(INTDIR)"\
 "$(INTDIR)\showdib.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_DLGOP) "$(INTDIR)"\
 "$(INTDIR)\showdib.pch"


!ELSEIF  "$(CFG)" == "ShowDib - Macintosh Release"


"$(INTDIR)\dlgopen.obj" : $(SOURCE) $(DEP_CPP_DLGOP) "$(INTDIR)"\
 "$(INTDIR)\showdib.pch"


!ELSEIF  "$(CFG)" == "ShowDib - Macintosh Debug"


"$(INTDIR)\dlgopen.obj" : $(SOURCE) $(DEP_CPP_DLGOP) "$(INTDIR)"\
 "$(INTDIR)\showdib.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_DLGOP) "$(INTDIR)"\
 "$(INTDIR)\showdib.pch"


!ELSEIF  "$(CFG)" == "ShowDib - Power Macintosh Release"


"$(INTDIR)\dlgopen.obj" : $(SOURCE) $(DEP_CPP_DLGOP) "$(INTDIR)"\
 "$(INTDIR)\showdib.pch"


!ELSEIF  "$(CFG)" == "ShowDib - Power Macintosh Debug"


"$(INTDIR)\dlgopen.obj" : $(SOURCE) $(DEP_CPP_DLGOP) "$(INTDIR)"\
 "$(INTDIR)\showdib.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_DLGOP) "$(INTDIR)"\
 "$(INTDIR)\showdib.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\drawdib.c
DEP_CPP_DRAWD=\
	".\stdwin.h"\
	".\showdib.h"\
	

!IF  "$(CFG)" == "ShowDib - Win32 Release"


"$(INTDIR)\drawdib.obj" : $(SOURCE) $(DEP_CPP_DRAWD) "$(INTDIR)"\
 "$(INTDIR)\showdib.pch"


!ELSEIF  "$(CFG)" == "ShowDib - Win32 Debug"


"$(INTDIR)\drawdib.obj" : $(SOURCE) $(DEP_CPP_DRAWD) "$(INTDIR)"\
 "$(INTDIR)\showdib.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_DRAWD) "$(INTDIR)"\
 "$(INTDIR)\showdib.pch"


!ELSEIF  "$(CFG)" == "ShowDib - Macintosh Release"


"$(INTDIR)\drawdib.obj" : $(SOURCE) $(DEP_CPP_DRAWD) "$(INTDIR)"\
 "$(INTDIR)\showdib.pch"


!ELSEIF  "$(CFG)" == "ShowDib - Macintosh Debug"


"$(INTDIR)\drawdib.obj" : $(SOURCE) $(DEP_CPP_DRAWD) "$(INTDIR)"\
 "$(INTDIR)\showdib.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_DRAWD) "$(INTDIR)"\
 "$(INTDIR)\showdib.pch"


!ELSEIF  "$(CFG)" == "ShowDib - Power Macintosh Release"


"$(INTDIR)\drawdib.obj" : $(SOURCE) $(DEP_CPP_DRAWD) "$(INTDIR)"\
 "$(INTDIR)\showdib.pch"


!ELSEIF  "$(CFG)" == "ShowDib - Power Macintosh Debug"


"$(INTDIR)\drawdib.obj" : $(SOURCE) $(DEP_CPP_DRAWD) "$(INTDIR)"\
 "$(INTDIR)\showdib.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_DRAWD) "$(INTDIR)"\
 "$(INTDIR)\showdib.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\print.c
DEP_CPP_PRINT=\
	".\stdwin.h"\
	".\showdib.h"\
	

!IF  "$(CFG)" == "ShowDib - Win32 Release"


"$(INTDIR)\print.obj" : $(SOURCE) $(DEP_CPP_PRINT) "$(INTDIR)"\
 "$(INTDIR)\showdib.pch"


!ELSEIF  "$(CFG)" == "ShowDib - Win32 Debug"


"$(INTDIR)\print.obj" : $(SOURCE) $(DEP_CPP_PRINT) "$(INTDIR)"\
 "$(INTDIR)\showdib.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_PRINT) "$(INTDIR)"\
 "$(INTDIR)\showdib.pch"


!ELSEIF  "$(CFG)" == "ShowDib - Macintosh Release"


"$(INTDIR)\print.obj" : $(SOURCE) $(DEP_CPP_PRINT) "$(INTDIR)"\
 "$(INTDIR)\showdib.pch"


!ELSEIF  "$(CFG)" == "ShowDib - Macintosh Debug"


"$(INTDIR)\print.obj" : $(SOURCE) $(DEP_CPP_PRINT) "$(INTDIR)"\
 "$(INTDIR)\showdib.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_PRINT) "$(INTDIR)"\
 "$(INTDIR)\showdib.pch"


!ELSEIF  "$(CFG)" == "ShowDib - Power Macintosh Release"


"$(INTDIR)\print.obj" : $(SOURCE) $(DEP_CPP_PRINT) "$(INTDIR)"\
 "$(INTDIR)\showdib.pch"


!ELSEIF  "$(CFG)" == "ShowDib - Power Macintosh Debug"


"$(INTDIR)\print.obj" : $(SOURCE) $(DEP_CPP_PRINT) "$(INTDIR)"\
 "$(INTDIR)\showdib.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_PRINT) "$(INTDIR)"\
 "$(INTDIR)\showdib.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\sdib.r

!IF  "$(CFG)" == "ShowDib - Win32 Release"

!ELSEIF  "$(CFG)" == "ShowDib - Win32 Debug"

!ELSEIF  "$(CFG)" == "ShowDib - Macintosh Release"

DEP_MRC_SDIB_=\
	{$(INCLUDE)}"\mrc\types.r"\
	{$(INCLUDE)}"\mrc\balloons.r"\
	

"$(INTDIR)\sdib.rsc" : $(SOURCE) $(DEP_MRC_SDIB_) "$(INTDIR)"
   $(MRC) /o"$(INTDIR)/sdib.rsc" /D "_68K_" /D "_MAC" /D "NDEBUG" /l 0x409\
 /NOLOGO $(SOURCE)


!ELSEIF  "$(CFG)" == "ShowDib - Macintosh Debug"

DEP_MRC_SDIB_=\
	{$(INCLUDE)}"\mrc\types.r"\
	{$(INCLUDE)}"\mrc\balloons.r"\
	

"$(INTDIR)\sdib.rsc" : $(SOURCE) $(DEP_MRC_SDIB_) "$(INTDIR)"
   $(MRC) /o"$(INTDIR)/sdib.rsc" /D "_68K_" /D "_MAC" /D "_DEBUG" /l 0x409\
 /NOLOGO $(SOURCE)


!ELSEIF  "$(CFG)" == "ShowDib - Power Macintosh Release"

DEP_MRC_SDIB_=\
	{$(INCLUDE)}"\mrc\types.r"\
	{$(INCLUDE)}"\mrc\balloons.r"\
	

"$(INTDIR)\sdib.rsc" : $(SOURCE) $(DEP_MRC_SDIB_) "$(INTDIR)"
   $(MRC) /o"$(INTDIR)/sdib.rsc" /D "_MPPC_" /D "_MAC" /D "NDEBUG" /l 0x409\
 /NOLOGO $(SOURCE)


!ELSEIF  "$(CFG)" == "ShowDib - Power Macintosh Debug"

DEP_MRC_SDIB_=\
	{$(INCLUDE)}"\mrc\types.r"\
	{$(INCLUDE)}"\mrc\balloons.r"\
	

"$(INTDIR)\sdib.rsc" : $(SOURCE) $(DEP_MRC_SDIB_) "$(INTDIR)"
   $(MRC) /o"$(INTDIR)/sdib.rsc" /D "_MPPC_" /D "_MAC" /D "_DEBUG" /l 0x409\
 /NOLOGO $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\showdib.c
DEP_CPP_SHOWD=\
	".\stdwin.h"\
	".\showdib.h"\
	

!IF  "$(CFG)" == "ShowDib - Win32 Release"


"$(INTDIR)\showdib.obj" : $(SOURCE) $(DEP_CPP_SHOWD) "$(INTDIR)"\
 "$(INTDIR)\showdib.pch"


!ELSEIF  "$(CFG)" == "ShowDib - Win32 Debug"


"$(INTDIR)\showdib.obj" : $(SOURCE) $(DEP_CPP_SHOWD) "$(INTDIR)"\
 "$(INTDIR)\showdib.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_SHOWD) "$(INTDIR)"\
 "$(INTDIR)\showdib.pch"


!ELSEIF  "$(CFG)" == "ShowDib - Macintosh Release"


"$(INTDIR)\showdib.obj" : $(SOURCE) $(DEP_CPP_SHOWD) "$(INTDIR)"\
 "$(INTDIR)\showdib.pch"


!ELSEIF  "$(CFG)" == "ShowDib - Macintosh Debug"


"$(INTDIR)\showdib.obj" : $(SOURCE) $(DEP_CPP_SHOWD) "$(INTDIR)"\
 "$(INTDIR)\showdib.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_SHOWD) "$(INTDIR)"\
 "$(INTDIR)\showdib.pch"


!ELSEIF  "$(CFG)" == "ShowDib - Power Macintosh Release"


"$(INTDIR)\showdib.obj" : $(SOURCE) $(DEP_CPP_SHOWD) "$(INTDIR)"\
 "$(INTDIR)\showdib.pch"


!ELSEIF  "$(CFG)" == "ShowDib - Power Macintosh Debug"


"$(INTDIR)\showdib.obj" : $(SOURCE) $(DEP_CPP_SHOWD) "$(INTDIR)"\
 "$(INTDIR)\showdib.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_SHOWD) "$(INTDIR)"\
 "$(INTDIR)\showdib.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\showdib.rc
DEP_RSC_SHOWDI=\
	".\res\showdib.ico"\
	".\res\dib.ico"\
	

!IF  "$(CFG)" == "ShowDib - Win32 Release"


"$(INTDIR)\showdib.res" : $(SOURCE) $(DEP_RSC_SHOWDI) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "ShowDib - Win32 Debug"


"$(INTDIR)\showdib.res" : $(SOURCE) $(DEP_RSC_SHOWDI) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "ShowDib - Macintosh Release"


"$(INTDIR)\showdib.rsc" : $(SOURCE) $(DEP_RSC_SHOWDI) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "ShowDib - Macintosh Debug"


"$(INTDIR)\showdib.rsc" : $(SOURCE) $(DEP_RSC_SHOWDI) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "ShowDib - Power Macintosh Release"


"$(INTDIR)\showdib.rsc" : $(SOURCE) $(DEP_RSC_SHOWDI) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "ShowDib - Power Macintosh Debug"


"$(INTDIR)\showdib.rsc" : $(SOURCE) $(DEP_RSC_SHOWDI) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\dib.c
DEP_CPP_DIB_C=\
	".\stdwin.h"\
	".\showdib.h"\
	

!IF  "$(CFG)" == "ShowDib - Win32 Release"


"$(INTDIR)\dib.obj" : $(SOURCE) $(DEP_CPP_DIB_C) "$(INTDIR)"\
 "$(INTDIR)\showdib.pch"


!ELSEIF  "$(CFG)" == "ShowDib - Win32 Debug"


"$(INTDIR)\dib.obj" : $(SOURCE) $(DEP_CPP_DIB_C) "$(INTDIR)"\
 "$(INTDIR)\showdib.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_DIB_C) "$(INTDIR)"\
 "$(INTDIR)\showdib.pch"


!ELSEIF  "$(CFG)" == "ShowDib - Macintosh Release"


"$(INTDIR)\dib.obj" : $(SOURCE) $(DEP_CPP_DIB_C) "$(INTDIR)"\
 "$(INTDIR)\showdib.pch"


!ELSEIF  "$(CFG)" == "ShowDib - Macintosh Debug"


"$(INTDIR)\dib.obj" : $(SOURCE) $(DEP_CPP_DIB_C) "$(INTDIR)"\
 "$(INTDIR)\showdib.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_DIB_C) "$(INTDIR)"\
 "$(INTDIR)\showdib.pch"


!ELSEIF  "$(CFG)" == "ShowDib - Power Macintosh Release"


"$(INTDIR)\dib.obj" : $(SOURCE) $(DEP_CPP_DIB_C) "$(INTDIR)"\
 "$(INTDIR)\showdib.pch"


!ELSEIF  "$(CFG)" == "ShowDib - Power Macintosh Debug"


"$(INTDIR)\dib.obj" : $(SOURCE) $(DEP_CPP_DIB_C) "$(INTDIR)"\
 "$(INTDIR)\showdib.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_DIB_C) "$(INTDIR)"\
 "$(INTDIR)\showdib.pch"


!ENDIF 

# End Source File
# End Target
# End Project
################################################################################
