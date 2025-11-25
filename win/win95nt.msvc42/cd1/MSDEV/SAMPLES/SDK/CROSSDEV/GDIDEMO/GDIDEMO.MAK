# Microsoft Visual C++ Generated NMAKE File, Format Version 40000
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101
# TARGTYPE "Power Macintosh Application" 0x0401
# TARGTYPE "Macintosh Application" 0x0301

!IF "$(CFG)" == ""
CFG=GdiDemo - Win32 Debug
!MESSAGE No configuration specified.  Defaulting to GdiDemo - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "GdiDemo - Win32 Release" && "$(CFG)" !=\
 "GdiDemo - Win32 Debug" && "$(CFG)" != "GdiDemo - Macintosh Release" &&\
 "$(CFG)" != "GdiDemo - Macintosh Debug" && "$(CFG)" !=\
 "GdiDemo - Power Macintosh Release" && "$(CFG)" !=\
 "GdiDemo - Power Macintosh Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "gdidemo.mak" CFG="GdiDemo - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "GdiDemo - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "GdiDemo - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE "GdiDemo - Macintosh Release" (based on "Macintosh Application")
!MESSAGE "GdiDemo - Macintosh Debug" (based on "Macintosh Application")
!MESSAGE "GdiDemo - Power Macintosh Release" (based on\
 "Power Macintosh Application")
!MESSAGE "GdiDemo - Power Macintosh Debug" (based on\
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
# PROP Target_Last_Scanned "GdiDemo - Power Macintosh Debug"

!IF  "$(CFG)" == "GdiDemo - Win32 Release"

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

ALL : "$(OUTDIR)\gdidemo.exe"

CLEAN : 
	-@erase ".\Release\gdidemo.exe"
	-@erase ".\Release\maze.obj"
	-@erase ".\Release\gdidemo.pch"
	-@erase ".\Release\init.obj"
	-@erase ".\Release\draw.obj"
	-@erase ".\Release\dialog.obj"
	-@erase ".\Release\bounce.obj"
	-@erase ".\Release\wininfo.obj"
	-@erase ".\Release\xform.obj"
	-@erase ".\Release\stdwin.obj"
	-@erase ".\Release\gdidemo.obj"
	-@erase ".\Release\poly.obj"
	-@erase ".\Release\gdidemo.res"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(OUTDIR)\GdiDemo.bsc" : "$(OUTDIR)" $(BSC32_SBRS)

"$(OUTDIR)\GdiDemo.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)

CPP=cl.exe
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /FR /YX /c
# ADD CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /Yu"stdwin.h" /c
# SUBTRACT CPP /Fr
CPP_PROJ=/nologo /ML /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS"\
 /Fp"$(INTDIR)/gdidemo.pch" /Yu"stdwin.h" /Fo"$(INTDIR)/" /c 
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
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/gdidemo.res" /d "NDEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/gdidemo.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib /nologo /subsystem:windows /incremental:no\
 /pdb:"$(OUTDIR)/gdidemo.pdb" /machine:I386 /out:"$(OUTDIR)/gdidemo.exe" 
LINK32_OBJS= \
	"$(INTDIR)/maze.obj" \
	"$(INTDIR)/init.obj" \
	"$(INTDIR)/draw.obj" \
	"$(INTDIR)/dialog.obj" \
	"$(INTDIR)/bounce.obj" \
	"$(INTDIR)/wininfo.obj" \
	"$(INTDIR)/xform.obj" \
	"$(INTDIR)/stdwin.obj" \
	"$(INTDIR)/gdidemo.obj" \
	"$(INTDIR)/poly.obj" \
	"$(INTDIR)/gdidemo.res"

"$(OUTDIR)\gdidemo.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "GdiDemo - Win32 Debug"

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

ALL : "$(OUTDIR)\gdidemo.exe"

CLEAN : 
	-@erase ".\Debug\vc40.pdb"
	-@erase ".\Debug\gdidemo.pch"
	-@erase ".\Debug\gdidemo.exe"
	-@erase ".\Debug\dialog.obj"
	-@erase ".\Debug\bounce.obj"
	-@erase ".\Debug\wininfo.obj"
	-@erase ".\Debug\maze.obj"
	-@erase ".\Debug\init.obj"
	-@erase ".\Debug\draw.obj"
	-@erase ".\Debug\gdidemo.obj"
	-@erase ".\Debug\stdwin.obj"
	-@erase ".\Debug\xform.obj"
	-@erase ".\Debug\poly.obj"
	-@erase ".\Debug\gdidemo.res"
	-@erase ".\Debug\gdidemo.ilk"
	-@erase ".\Debug\gdidemo.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(OUTDIR)\GdiDemo.bsc" : "$(OUTDIR)" $(BSC32_SBRS)

"$(OUTDIR)\GdiDemo.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)

CPP=cl.exe
# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /FR /YX /c
# ADD CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /Yu"stdwin.h" /c
# SUBTRACT CPP /Fr
CPP_PROJ=/nologo /MLd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /Fp"$(INTDIR)/gdidemo.pch" /Yu"stdwin.h" /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
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
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/gdidemo.res" /d "_DEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/gdidemo.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib /nologo /subsystem:windows /incremental:yes\
 /pdb:"$(OUTDIR)/gdidemo.pdb" /debug /machine:I386 /out:"$(OUTDIR)/gdidemo.exe" 
LINK32_OBJS= \
	"$(INTDIR)/dialog.obj" \
	"$(INTDIR)/bounce.obj" \
	"$(INTDIR)/wininfo.obj" \
	"$(INTDIR)/maze.obj" \
	"$(INTDIR)/init.obj" \
	"$(INTDIR)/draw.obj" \
	"$(INTDIR)/gdidemo.obj" \
	"$(INTDIR)/stdwin.obj" \
	"$(INTDIR)/xform.obj" \
	"$(INTDIR)/poly.obj" \
	"$(INTDIR)/gdidemo.res"

"$(OUTDIR)\gdidemo.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "GdiDemo - Macintosh Release"

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

ALL : "$(OUTDIR)\gdidemo.exe" "$(OUTDIR)\gdidemo.trg"

CLEAN : 
	-@erase ".\MacRel\gdidemo.trg"
	-@erase ".\MacRel\gdidemo.exe"
	-@erase ".\MacRel\xform.obj"
	-@erase ".\MacRel\gdidemo.pch"
	-@erase ".\MacRel\wininfo.obj"
	-@erase ".\MacRel\poly.obj"
	-@erase ".\MacRel\dialog.obj"
	-@erase ".\MacRel\bounce.obj"
	-@erase ".\MacRel\gdidemo.obj"
	-@erase ".\MacRel\maze.obj"
	-@erase ".\MacRel\init.obj"
	-@erase ".\MacRel\draw.obj"
	-@erase ".\MacRel\stdwin.obj"
	-@erase ".\MacRel\gdidemo.rsc"
	-@erase ".\MacRel\gdid.rsc"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(OUTDIR)\GdiDemo.bsc" : "$(OUTDIR)" $(BSC32_SBRS)

"$(OUTDIR)\GdiDemo.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)

MTL=mktyplib.exe
# ADD BASE MTL /nologo /D "NDEBUG" /mac
# ADD MTL /nologo /D "NDEBUG" /mac
MTL_PROJ=/nologo /D "NDEBUG" /mac 
CPP=cl.exe
# ADD BASE CPP /nologo /AL /Q68s /W3 /GX /O2 /D "_WINDOWS" /D "_MAC" /D "_68K_" /D "WIN32" /D "NDEBUG" /FR /YX /c
# ADD CPP /nologo /AL /Q68s /W3 /GX /O2 /D "_WINDOWS" /D "_MAC" /D "_68K_" /D "WIN32" /D "NDEBUG" /Yu"stdwin.h" /c
# SUBTRACT CPP /Fr
CPP_PROJ=/nologo /AL /Q68s /W3 /GX /O2 /D "_WINDOWS" /D "_MAC" /D "_68K_" /D\
 "WIN32" /D "NDEBUG" /Fp"$(INTDIR)/gdidemo.pch" /Yu"stdwin.h" /Fo"$(INTDIR)/" /c\
 
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
RSC_PROJ=/l 0x409 /r /m /fo"$(INTDIR)/gdidemo.rsc" /d "_MAC" /d "_68K_" /d\
 "NDEBUG" 
MRC=mrc.exe
# ADD BASE MRC /D "_68K_" /D "_MAC" /D "NDEBUG" /NOLOGO
# ADD MRC /D "_68K_" /D "_MAC" /D "NDEBUG" /NOLOGO
MRC_PROJ=/D "_68K_" /D "_MAC" /D "NDEBUG" /l 0x409 /NOLOGO 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/gdidemo.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 wlm.lib /nologo /MAC:bundle /machine:M68K
# ADD LINK32 wlm.lib /nologo /MAC:bundle /MAC:creator="GDID" /machine:M68K
LINK32_FLAGS=wlm.lib /nologo /MAC:bundle /MAC:type="APPL" /MAC:creator="GDID"\
 /pdb:"$(OUTDIR)/gdidemo.pdb" /machine:M68K /out:"$(OUTDIR)/gdidemo.exe" 
LINK32_OBJS= \
	"$(INTDIR)/xform.obj" \
	"$(INTDIR)/wininfo.obj" \
	"$(INTDIR)/poly.obj" \
	"$(INTDIR)/dialog.obj" \
	"$(INTDIR)/bounce.obj" \
	"$(INTDIR)/gdidemo.obj" \
	"$(INTDIR)/maze.obj" \
	"$(INTDIR)/init.obj" \
	"$(INTDIR)/draw.obj" \
	"$(INTDIR)/stdwin.obj" \
	"$(INTDIR)/gdidemo.rsc" \
	"$(INTDIR)/gdid.rsc"

"$(OUTDIR)\gdidemo.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

MFILE32=mfile.exe
# ADD BASE MFILE32 COPY /NOLOGO
# ADD MFILE32 COPY /NOLOGO
MFILE32_FLAGS=COPY /NOLOGO 
MFILE32_FILES= \
	"$(OUTDIR)/gdidemo.exe"

"$(OUTDIR)\gdidemo.trg" : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\MacRel\gdidemo.exe\
 "$(MFILE32_DEST):gdidemo.exe">"$(OUTDIR)\gdidemo.trg"

DOWNLOAD : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\MacRel\gdidemo.exe\
 "$(MFILE32_DEST):gdidemo.exe">"$(OUTDIR)\gdidemo.trg"

!ELSEIF  "$(CFG)" == "GdiDemo - Macintosh Debug"

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

ALL : "$(OUTDIR)\gdidemo.exe" "$(OUTDIR)\gdidemo.trg"

CLEAN : 
	-@erase ".\MacDbg\vc40.pdb"
	-@erase ".\MacDbg\gdidemo.pch"
	-@erase ".\MacDbg\gdidemo.pdb"
	-@erase ".\MacDbg\xform.obj"
	-@erase ".\MacDbg\maze.obj"
	-@erase ".\MacDbg\init.obj"
	-@erase ".\MacDbg\draw.obj"
	-@erase ".\MacDbg\dialog.obj"
	-@erase ".\MacDbg\bounce.obj"
	-@erase ".\MacDbg\wininfo.obj"
	-@erase ".\MacDbg\gdidemo.obj"
	-@erase ".\MacDbg\stdwin.obj"
	-@erase ".\MacDbg\poly.obj"
	-@erase ".\MacDbg\gdid.rsc"
	-@erase ".\MacDbg\gdidemo.rsc"
	-@erase ".\MacDbg\gdidemo.trg"
	-@erase ".\MacDbg\gdidemo.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(OUTDIR)\GdiDemo.bsc" : "$(OUTDIR)" $(BSC32_SBRS)

"$(OUTDIR)\GdiDemo.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)

MTL=mktyplib.exe
# ADD BASE MTL /nologo /D "_DEBUG" /mac
# ADD MTL /nologo /D "_DEBUG" /mac
MTL_PROJ=/nologo /D "_DEBUG" /mac 
CPP=cl.exe
# ADD BASE CPP /nologo /AL /Q68s /Q68m /W3 /GX /Zi /Od /D "_WINDOWS" /D "_MAC" /D "_68K_" /D "WIN32" /D "_DEBUG" /FR /YX /c
# ADD CPP /nologo /AL /Q68s /Q68m /W3 /GX /Zi /Od /D "_WINDOWS" /D "_MAC" /D "_68K_" /D "WIN32" /D "_DEBUG" /Yu"stdwin.h" /c
# SUBTRACT CPP /Fr
CPP_PROJ=/nologo /AL /Q68s /Q68m /W3 /GX /Zi /Od /D "_WINDOWS" /D "_MAC" /D\
 "_68K_" /D "WIN32" /D "_DEBUG" /Fp"$(INTDIR)/gdidemo.pch" /Yu"stdwin.h"\
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
RSC_PROJ=/l 0x409 /r /m /fo"$(INTDIR)/gdidemo.rsc" /d "_MAC" /d "_68K_" /d\
 "_DEBUG" 
MRC=mrc.exe
# ADD BASE MRC /D "_68K_" /D "_MAC" /D "_DEBUG" /NOLOGO
# ADD MRC /D "_68K_" /D "_MAC" /D "_DEBUG" /NOLOGO
MRC_PROJ=/D "_68K_" /D "_MAC" /D "_DEBUG" /l 0x409 /NOLOGO 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/gdidemo.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 wlm.lib /nologo /MAC:bundle /debug /machine:M68K
# ADD LINK32 wlm.lib /nologo /MAC:bundle /MAC:creator="GDID" /debug /machine:M68K
LINK32_FLAGS=wlm.lib /nologo /MAC:bundle /MAC:type="APPL" /MAC:creator="GDID"\
 /pdb:"$(OUTDIR)/gdidemo.pdb" /debug /machine:M68K /out:"$(OUTDIR)/gdidemo.exe" 
LINK32_OBJS= \
	"$(INTDIR)/xform.obj" \
	"$(INTDIR)/maze.obj" \
	"$(INTDIR)/init.obj" \
	"$(INTDIR)/draw.obj" \
	"$(INTDIR)/dialog.obj" \
	"$(INTDIR)/bounce.obj" \
	"$(INTDIR)/wininfo.obj" \
	"$(INTDIR)/gdidemo.obj" \
	"$(INTDIR)/stdwin.obj" \
	"$(INTDIR)/poly.obj" \
	"$(INTDIR)/gdid.rsc" \
	"$(INTDIR)/gdidemo.rsc"

"$(OUTDIR)\gdidemo.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

MFILE32=mfile.exe
# ADD BASE MFILE32 COPY /NOLOGO
# ADD MFILE32 COPY /NOLOGO
MFILE32_FLAGS=COPY /NOLOGO 
MFILE32_FILES= \
	"$(OUTDIR)/gdidemo.exe"

"$(OUTDIR)\gdidemo.trg" : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\MacDbg\gdidemo.exe\
 "$(MFILE32_DEST):gdidemo.exe">"$(OUTDIR)\gdidemo.trg"

DOWNLOAD : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\MacDbg\gdidemo.exe\
 "$(MFILE32_DEST):gdidemo.exe">"$(OUTDIR)\gdidemo.trg"

!ELSEIF  "$(CFG)" == "GdiDemo - Power Macintosh Release"

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

ALL : "$(OUTDIR)\gdidemo.exe" "$(OUTDIR)\gdidemo.trg"

CLEAN : 
	-@erase ".\PMacRel\gdidemo.trg"
	-@erase ".\PMacRel\gdidemo.exe"
	-@erase ".\PMacRel\xform.obj"
	-@erase ".\PMacRel\gdidemo.pch"
	-@erase ".\PMacRel\wininfo.obj"
	-@erase ".\PMacRel\stdwin.obj"
	-@erase ".\PMacRel\gdidemo.obj"
	-@erase ".\PMacRel\poly.obj"
	-@erase ".\PMacRel\dialog.obj"
	-@erase ".\PMacRel\bounce.obj"
	-@erase ".\PMacRel\maze.obj"
	-@erase ".\PMacRel\init.obj"
	-@erase ".\PMacRel\draw.obj"
	-@erase ".\PMacRel\gdidemo.rsc"
	-@erase ".\PMacRel\gdid.rsc"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(OUTDIR)\GdiDemo.bsc" : "$(OUTDIR)" $(BSC32_SBRS)

"$(OUTDIR)\GdiDemo.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)

MTL=mktyplib.exe
# ADD BASE MTL /nologo /D "NDEBUG"
# ADD MTL /nologo /D "NDEBUG"
MTL_PROJ=/nologo /D "NDEBUG" 
CPP=cl.exe
# ADD BASE CPP /nologo /W3 /GX /O2 /D "_MAC" /D "_MPPC_" /D "_WINDOWS" /D "WIN32" /D "NDEBUG" /FR /YX /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "_MAC" /D "_MPPC_" /D "_WINDOWS" /D "WIN32" /D "_WLMDLL" /Yu"stdwin.h" /c
# SUBTRACT CPP /Fr
CPP_PROJ=/nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "_MAC" /D "_MPPC_" /D\
 "_WINDOWS" /D "WIN32" /D "_WLMDLL" /Fp"$(INTDIR)/gdidemo.pch" /Yu"stdwin.h"\
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
RSC_PROJ=/l 0x409 /r /m /fo"$(INTDIR)/gdidemo.rsc" /d "_MAC" /d "_MPPC_" /d\
 "NDEBUG" 
MRC=mrc.exe
# ADD BASE MRC /D "_MPPC_" /D "_MAC" /D "NDEBUG" /NOLOGO
# ADD MRC /D "_MPPC_" /D "_MAC" /D "NDEBUG" /NOLOGO
MRC_PROJ=/D "_MPPC_" /D "_MAC" /D "NDEBUG" /l 0x409 /NOLOGO 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/gdidemo.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 cfrg.rsc /nologo /mac:bundle /machine:MPPC
# ADD LINK32 /nologo /mac:bundle /mac:creator="GDID" /machine:MPPC
LINK32_FLAGS=/nologo /mac:bundle /mac:type="APPL" /mac:creator="GDID"\
 /incremental:no /pdb:"$(OUTDIR)/gdidemo.pdb" /machine:MPPC\
 /out:"$(OUTDIR)/gdidemo.exe" 
LINK32_OBJS= \
	"$(INTDIR)/xform.obj" \
	"$(INTDIR)/wininfo.obj" \
	"$(INTDIR)/stdwin.obj" \
	"$(INTDIR)/gdidemo.obj" \
	"$(INTDIR)/poly.obj" \
	"$(INTDIR)/dialog.obj" \
	"$(INTDIR)/bounce.obj" \
	"$(INTDIR)/maze.obj" \
	"$(INTDIR)/init.obj" \
	"$(INTDIR)/draw.obj" \
	"$(INTDIR)/gdidemo.rsc" \
	"$(INTDIR)/gdid.rsc"

"$(OUTDIR)\gdidemo.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

MFILE32=mfile.exe
# ADD BASE MFILE32 COPY /NOLOGO
# ADD MFILE32 COPY /NOLOGO
MFILE32_FLAGS=COPY /NOLOGO 
MFILE32_FILES= \
	"$(OUTDIR)/gdidemo.exe"

"$(OUTDIR)\gdidemo.trg" : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\PMacRel\gdidemo.exe\
 "$(MFILE32_DEST):gdidemo.exe">"$(OUTDIR)\gdidemo.trg"

DOWNLOAD : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\PMacRel\gdidemo.exe\
 "$(MFILE32_DEST):gdidemo.exe">"$(OUTDIR)\gdidemo.trg"

!ELSEIF  "$(CFG)" == "GdiDemo - Power Macintosh Debug"

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

ALL : "$(OUTDIR)\gdidemo.exe" "$(OUTDIR)\gdidemo.trg"

CLEAN : 
	-@erase ".\PMacDbg\vc40.pdb"
	-@erase ".\PMacDbg\gdidemo.pch"
	-@erase ".\PMacDbg\gdidemo.ilk"
	-@erase ".\PMacDbg\poly.obj"
	-@erase ".\PMacDbg\stdwin.obj"
	-@erase ".\PMacDbg\xform.obj"
	-@erase ".\PMacDbg\maze.obj"
	-@erase ".\PMacDbg\init.obj"
	-@erase ".\PMacDbg\draw.obj"
	-@erase ".\PMacDbg\wininfo.obj"
	-@erase ".\PMacDbg\gdidemo.obj"
	-@erase ".\PMacDbg\dialog.obj"
	-@erase ".\PMacDbg\bounce.obj"
	-@erase ".\PMacDbg\gdid.rsc"
	-@erase ".\PMacDbg\gdidemo.rsc"
	-@erase ".\PMacDbg\gdidemo.pdb"
	-@erase ".\PMacDbg\gdidemo.trg"
	-@erase ".\PMacDbg\gdidemo.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(OUTDIR)\GdiDemo.bsc" : "$(OUTDIR)" $(BSC32_SBRS)

"$(OUTDIR)\GdiDemo.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)

MTL=mktyplib.exe
# ADD BASE MTL /nologo /D "_DEBUG"
# ADD MTL /nologo /D "_DEBUG"
MTL_PROJ=/nologo /D "_DEBUG" 
CPP=cl.exe
# ADD BASE CPP /nologo /W3 /GX /Zi /Od /D "_MAC" /D "_MPPC_" /D "_WINDOWS" /D "WIN32" /D "_DEBUG" /FR /YX /c
# ADD CPP /nologo /MDd /W3 /GX /Zi /Od /D "_DEBUG" /D "_MAC" /D "_MPPC_" /D "_WINDOWS" /D "WIN32" /D "_WLMDLL" /Yu"stdwin.h" /c
# SUBTRACT CPP /Fr
CPP_PROJ=/nologo /MDd /W3 /GX /Zi /Od /D "_DEBUG" /D "_MAC" /D "_MPPC_" /D\
 "_WINDOWS" /D "WIN32" /D "_WLMDLL" /Fp"$(INTDIR)/gdidemo.pch" /Yu"stdwin.h"\
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
RSC_PROJ=/l 0x409 /r /m /fo"$(INTDIR)/gdidemo.rsc" /d "_MAC" /d "_MPPC_" /d\
 "_DEBUG" 
MRC=mrc.exe
# ADD BASE MRC /D "_MPPC_" /D "_MAC" /D "_DEBUG" /NOLOGO
# ADD MRC /D "_MPPC_" /D "_MAC" /D "_DEBUG" /NOLOGO
MRC_PROJ=/D "_MPPC_" /D "_MAC" /D "_DEBUG" /l 0x409 /NOLOGO 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/gdidemo.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 cfrg.rsc /nologo /mac:bundle /debug /machine:MPPC
# ADD LINK32 /nologo /mac:bundle /mac:creator="GDID" /debug /machine:MPPC
LINK32_FLAGS=/nologo /mac:bundle /mac:type="APPL" /mac:creator="GDID"\
 /incremental:yes /pdb:"$(OUTDIR)/gdidemo.pdb" /debug /machine:MPPC\
 /out:"$(OUTDIR)/gdidemo.exe" 
LINK32_OBJS= \
	"$(INTDIR)/poly.obj" \
	"$(INTDIR)/stdwin.obj" \
	"$(INTDIR)/xform.obj" \
	"$(INTDIR)/maze.obj" \
	"$(INTDIR)/init.obj" \
	"$(INTDIR)/draw.obj" \
	"$(INTDIR)/wininfo.obj" \
	"$(INTDIR)/gdidemo.obj" \
	"$(INTDIR)/dialog.obj" \
	"$(INTDIR)/bounce.obj" \
	"$(INTDIR)/gdid.rsc" \
	"$(INTDIR)/gdidemo.rsc"

"$(OUTDIR)\gdidemo.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

MFILE32=mfile.exe
# ADD BASE MFILE32 COPY /NOLOGO
# ADD MFILE32 COPY /NOLOGO
MFILE32_FLAGS=COPY /NOLOGO 
MFILE32_FILES= \
	"$(OUTDIR)/gdidemo.exe"

"$(OUTDIR)\gdidemo.trg" : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\PMacDbg\gdidemo.exe\
 "$(MFILE32_DEST):gdidemo.exe">"$(OUTDIR)\gdidemo.trg"

DOWNLOAD : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\PMacDbg\gdidemo.exe\
 "$(MFILE32_DEST):gdidemo.exe">"$(OUTDIR)\gdidemo.trg"

!ENDIF 

################################################################################
# Begin Target

# Name "GdiDemo - Win32 Release"
# Name "GdiDemo - Win32 Debug"
# Name "GdiDemo - Macintosh Release"
# Name "GdiDemo - Macintosh Debug"
# Name "GdiDemo - Power Macintosh Release"
# Name "GdiDemo - Power Macintosh Debug"

!IF  "$(CFG)" == "GdiDemo - Win32 Release"

!ELSEIF  "$(CFG)" == "GdiDemo - Win32 Debug"

!ELSEIF  "$(CFG)" == "GdiDemo - Macintosh Release"

!ELSEIF  "$(CFG)" == "GdiDemo - Macintosh Debug"

!ELSEIF  "$(CFG)" == "GdiDemo - Power Macintosh Release"

!ELSEIF  "$(CFG)" == "GdiDemo - Power Macintosh Debug"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\bounce.c

!IF  "$(CFG)" == "GdiDemo - Win32 Release"

DEP_CPP_BOUNC=\
	".\bounce.h"\
	".\gdidemo.h"\
	".\stdwin.h"\
	

"$(INTDIR)\bounce.obj" : $(SOURCE) $(DEP_CPP_BOUNC) "$(INTDIR)"\
 "$(INTDIR)\gdidemo.pch"


!ELSEIF  "$(CFG)" == "GdiDemo - Win32 Debug"

DEP_CPP_BOUNC=\
	".\stdwin.h"\
	".\gdidemo.h"\
	".\bounce.h"\
	

"$(INTDIR)\bounce.obj" : $(SOURCE) $(DEP_CPP_BOUNC) "$(INTDIR)"\
 "$(INTDIR)\gdidemo.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_BOUNC) "$(INTDIR)"\
 "$(INTDIR)\gdidemo.pch"


!ELSEIF  "$(CFG)" == "GdiDemo - Macintosh Release"

DEP_CPP_BOUNC=\
	".\stdwin.h"\
	".\gdidemo.h"\
	".\bounce.h"\
	

"$(INTDIR)\bounce.obj" : $(SOURCE) $(DEP_CPP_BOUNC) "$(INTDIR)"\
 "$(INTDIR)\gdidemo.pch"


!ELSEIF  "$(CFG)" == "GdiDemo - Macintosh Debug"

DEP_CPP_BOUNC=\
	".\bounce.h"\
	".\gdidemo.h"\
	".\stdwin.h"\
	

"$(INTDIR)\bounce.obj" : $(SOURCE) $(DEP_CPP_BOUNC) "$(INTDIR)"\
 "$(INTDIR)\gdidemo.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_BOUNC) "$(INTDIR)"\
 "$(INTDIR)\gdidemo.pch"


!ELSEIF  "$(CFG)" == "GdiDemo - Power Macintosh Release"

DEP_CPP_BOUNC=\
	".\bounce.h"\
	".\gdidemo.h"\
	".\stdwin.h"\
	

"$(INTDIR)\bounce.obj" : $(SOURCE) $(DEP_CPP_BOUNC) "$(INTDIR)"\
 "$(INTDIR)\gdidemo.pch"


!ELSEIF  "$(CFG)" == "GdiDemo - Power Macintosh Debug"

DEP_CPP_BOUNC=\
	".\bounce.h"\
	".\gdidemo.h"\
	".\stdwin.h"\
	

"$(INTDIR)\bounce.obj" : $(SOURCE) $(DEP_CPP_BOUNC) "$(INTDIR)"\
 "$(INTDIR)\gdidemo.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_BOUNC) "$(INTDIR)"\
 "$(INTDIR)\gdidemo.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\dialog.c

!IF  "$(CFG)" == "GdiDemo - Win32 Release"

DEP_CPP_DIALO=\
	".\gdidemo.h"\
	".\stdwin.h"\
	

"$(INTDIR)\dialog.obj" : $(SOURCE) $(DEP_CPP_DIALO) "$(INTDIR)"\
 "$(INTDIR)\gdidemo.pch"


!ELSEIF  "$(CFG)" == "GdiDemo - Win32 Debug"

DEP_CPP_DIALO=\
	".\stdwin.h"\
	".\gdidemo.h"\
	

"$(INTDIR)\dialog.obj" : $(SOURCE) $(DEP_CPP_DIALO) "$(INTDIR)"\
 "$(INTDIR)\gdidemo.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_DIALO) "$(INTDIR)"\
 "$(INTDIR)\gdidemo.pch"


!ELSEIF  "$(CFG)" == "GdiDemo - Macintosh Release"

DEP_CPP_DIALO=\
	".\stdwin.h"\
	".\gdidemo.h"\
	

"$(INTDIR)\dialog.obj" : $(SOURCE) $(DEP_CPP_DIALO) "$(INTDIR)"\
 "$(INTDIR)\gdidemo.pch"


!ELSEIF  "$(CFG)" == "GdiDemo - Macintosh Debug"

DEP_CPP_DIALO=\
	".\gdidemo.h"\
	".\stdwin.h"\
	

"$(INTDIR)\dialog.obj" : $(SOURCE) $(DEP_CPP_DIALO) "$(INTDIR)"\
 "$(INTDIR)\gdidemo.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_DIALO) "$(INTDIR)"\
 "$(INTDIR)\gdidemo.pch"


!ELSEIF  "$(CFG)" == "GdiDemo - Power Macintosh Release"

DEP_CPP_DIALO=\
	".\gdidemo.h"\
	".\stdwin.h"\
	

"$(INTDIR)\dialog.obj" : $(SOURCE) $(DEP_CPP_DIALO) "$(INTDIR)"\
 "$(INTDIR)\gdidemo.pch"


!ELSEIF  "$(CFG)" == "GdiDemo - Power Macintosh Debug"

DEP_CPP_DIALO=\
	".\gdidemo.h"\
	".\stdwin.h"\
	

"$(INTDIR)\dialog.obj" : $(SOURCE) $(DEP_CPP_DIALO) "$(INTDIR)"\
 "$(INTDIR)\gdidemo.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_DIALO) "$(INTDIR)"\
 "$(INTDIR)\gdidemo.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\draw.c

!IF  "$(CFG)" == "GdiDemo - Win32 Release"

DEP_CPP_DRAW_=\
	".\draw.h"\
	".\gdidemo.h"\
	".\stdwin.h"\
	

"$(INTDIR)\draw.obj" : $(SOURCE) $(DEP_CPP_DRAW_) "$(INTDIR)"\
 "$(INTDIR)\gdidemo.pch"


!ELSEIF  "$(CFG)" == "GdiDemo - Win32 Debug"

DEP_CPP_DRAW_=\
	".\stdwin.h"\
	".\gdidemo.h"\
	".\draw.h"\
	

"$(INTDIR)\draw.obj" : $(SOURCE) $(DEP_CPP_DRAW_) "$(INTDIR)"\
 "$(INTDIR)\gdidemo.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_DRAW_) "$(INTDIR)"\
 "$(INTDIR)\gdidemo.pch"


!ELSEIF  "$(CFG)" == "GdiDemo - Macintosh Release"

DEP_CPP_DRAW_=\
	".\stdwin.h"\
	".\gdidemo.h"\
	".\draw.h"\
	

"$(INTDIR)\draw.obj" : $(SOURCE) $(DEP_CPP_DRAW_) "$(INTDIR)"\
 "$(INTDIR)\gdidemo.pch"


!ELSEIF  "$(CFG)" == "GdiDemo - Macintosh Debug"

DEP_CPP_DRAW_=\
	".\draw.h"\
	".\gdidemo.h"\
	".\stdwin.h"\
	

"$(INTDIR)\draw.obj" : $(SOURCE) $(DEP_CPP_DRAW_) "$(INTDIR)"\
 "$(INTDIR)\gdidemo.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_DRAW_) "$(INTDIR)"\
 "$(INTDIR)\gdidemo.pch"


!ELSEIF  "$(CFG)" == "GdiDemo - Power Macintosh Release"

DEP_CPP_DRAW_=\
	".\draw.h"\
	".\gdidemo.h"\
	".\stdwin.h"\
	

"$(INTDIR)\draw.obj" : $(SOURCE) $(DEP_CPP_DRAW_) "$(INTDIR)"\
 "$(INTDIR)\gdidemo.pch"


!ELSEIF  "$(CFG)" == "GdiDemo - Power Macintosh Debug"

DEP_CPP_DRAW_=\
	".\draw.h"\
	".\gdidemo.h"\
	".\stdwin.h"\
	

"$(INTDIR)\draw.obj" : $(SOURCE) $(DEP_CPP_DRAW_) "$(INTDIR)"\
 "$(INTDIR)\gdidemo.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_DRAW_) "$(INTDIR)"\
 "$(INTDIR)\gdidemo.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\gdidemo.c

!IF  "$(CFG)" == "GdiDemo - Win32 Release"

DEP_CPP_GDIDE=\
	".\bounce.h"\
	".\draw.h"\
	".\gdidemo.h"\
	".\maze.h"\
	".\poly.h"\
	".\stdwin.h"\
	".\xform.h"\
	

"$(INTDIR)\gdidemo.obj" : $(SOURCE) $(DEP_CPP_GDIDE) "$(INTDIR)"\
 "$(INTDIR)\gdidemo.pch"


!ELSEIF  "$(CFG)" == "GdiDemo - Win32 Debug"

DEP_CPP_GDIDE=\
	".\stdwin.h"\
	".\gdidemo.h"\
	".\poly.h"\
	".\xform.h"\
	".\maze.h"\
	".\draw.h"\
	".\bounce.h"\
	

"$(INTDIR)\gdidemo.obj" : $(SOURCE) $(DEP_CPP_GDIDE) "$(INTDIR)"\
 "$(INTDIR)\gdidemo.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_GDIDE) "$(INTDIR)"\
 "$(INTDIR)\gdidemo.pch"


!ELSEIF  "$(CFG)" == "GdiDemo - Macintosh Release"

DEP_CPP_GDIDE=\
	".\stdwin.h"\
	".\gdidemo.h"\
	".\poly.h"\
	".\xform.h"\
	".\maze.h"\
	".\draw.h"\
	".\bounce.h"\
	

"$(INTDIR)\gdidemo.obj" : $(SOURCE) $(DEP_CPP_GDIDE) "$(INTDIR)"\
 "$(INTDIR)\gdidemo.pch"


!ELSEIF  "$(CFG)" == "GdiDemo - Macintosh Debug"

DEP_CPP_GDIDE=\
	".\bounce.h"\
	".\draw.h"\
	".\gdidemo.h"\
	".\maze.h"\
	".\poly.h"\
	".\stdwin.h"\
	".\xform.h"\
	

"$(INTDIR)\gdidemo.obj" : $(SOURCE) $(DEP_CPP_GDIDE) "$(INTDIR)"\
 "$(INTDIR)\gdidemo.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_GDIDE) "$(INTDIR)"\
 "$(INTDIR)\gdidemo.pch"


!ELSEIF  "$(CFG)" == "GdiDemo - Power Macintosh Release"

DEP_CPP_GDIDE=\
	".\bounce.h"\
	".\draw.h"\
	".\gdidemo.h"\
	".\maze.h"\
	".\poly.h"\
	".\stdwin.h"\
	".\xform.h"\
	

"$(INTDIR)\gdidemo.obj" : $(SOURCE) $(DEP_CPP_GDIDE) "$(INTDIR)"\
 "$(INTDIR)\gdidemo.pch"


!ELSEIF  "$(CFG)" == "GdiDemo - Power Macintosh Debug"

DEP_CPP_GDIDE=\
	".\bounce.h"\
	".\draw.h"\
	".\gdidemo.h"\
	".\maze.h"\
	".\poly.h"\
	".\stdwin.h"\
	".\xform.h"\
	

"$(INTDIR)\gdidemo.obj" : $(SOURCE) $(DEP_CPP_GDIDE) "$(INTDIR)"\
 "$(INTDIR)\gdidemo.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_GDIDE) "$(INTDIR)"\
 "$(INTDIR)\gdidemo.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\gdidemo.rc
DEP_RSC_GDIDEM=\
	".\res\gdidemo.ico"\
	".\gdidemo.h"\
	

!IF  "$(CFG)" == "GdiDemo - Win32 Release"


"$(INTDIR)\gdidemo.res" : $(SOURCE) $(DEP_RSC_GDIDEM) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "GdiDemo - Win32 Debug"


"$(INTDIR)\gdidemo.res" : $(SOURCE) $(DEP_RSC_GDIDEM) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "GdiDemo - Macintosh Release"


"$(INTDIR)\gdidemo.rsc" : $(SOURCE) $(DEP_RSC_GDIDEM) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "GdiDemo - Macintosh Debug"


"$(INTDIR)\gdidemo.rsc" : $(SOURCE) $(DEP_RSC_GDIDEM) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "GdiDemo - Power Macintosh Release"


"$(INTDIR)\gdidemo.rsc" : $(SOURCE) $(DEP_RSC_GDIDEM) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "GdiDemo - Power Macintosh Debug"


"$(INTDIR)\gdidemo.rsc" : $(SOURCE) $(DEP_RSC_GDIDEM) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\init.c

!IF  "$(CFG)" == "GdiDemo - Win32 Release"

DEP_CPP_INIT_=\
	".\bounce.h"\
	".\draw.h"\
	".\gdidemo.h"\
	".\maze.h"\
	".\poly.h"\
	".\stdwin.h"\
	".\xform.h"\
	

"$(INTDIR)\init.obj" : $(SOURCE) $(DEP_CPP_INIT_) "$(INTDIR)"\
 "$(INTDIR)\gdidemo.pch"


!ELSEIF  "$(CFG)" == "GdiDemo - Win32 Debug"

DEP_CPP_INIT_=\
	".\stdwin.h"\
	".\gdidemo.h"\
	".\poly.h"\
	".\xform.h"\
	".\maze.h"\
	".\draw.h"\
	".\bounce.h"\
	

"$(INTDIR)\init.obj" : $(SOURCE) $(DEP_CPP_INIT_) "$(INTDIR)"\
 "$(INTDIR)\gdidemo.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_INIT_) "$(INTDIR)"\
 "$(INTDIR)\gdidemo.pch"


!ELSEIF  "$(CFG)" == "GdiDemo - Macintosh Release"

DEP_CPP_INIT_=\
	".\stdwin.h"\
	".\gdidemo.h"\
	".\poly.h"\
	".\xform.h"\
	".\maze.h"\
	".\draw.h"\
	".\bounce.h"\
	

"$(INTDIR)\init.obj" : $(SOURCE) $(DEP_CPP_INIT_) "$(INTDIR)"\
 "$(INTDIR)\gdidemo.pch"


!ELSEIF  "$(CFG)" == "GdiDemo - Macintosh Debug"

DEP_CPP_INIT_=\
	".\bounce.h"\
	".\draw.h"\
	".\gdidemo.h"\
	".\maze.h"\
	".\poly.h"\
	".\stdwin.h"\
	".\xform.h"\
	

"$(INTDIR)\init.obj" : $(SOURCE) $(DEP_CPP_INIT_) "$(INTDIR)"\
 "$(INTDIR)\gdidemo.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_INIT_) "$(INTDIR)"\
 "$(INTDIR)\gdidemo.pch"


!ELSEIF  "$(CFG)" == "GdiDemo - Power Macintosh Release"

DEP_CPP_INIT_=\
	".\bounce.h"\
	".\draw.h"\
	".\gdidemo.h"\
	".\maze.h"\
	".\poly.h"\
	".\stdwin.h"\
	".\xform.h"\
	

"$(INTDIR)\init.obj" : $(SOURCE) $(DEP_CPP_INIT_) "$(INTDIR)"\
 "$(INTDIR)\gdidemo.pch"


!ELSEIF  "$(CFG)" == "GdiDemo - Power Macintosh Debug"

DEP_CPP_INIT_=\
	".\bounce.h"\
	".\draw.h"\
	".\gdidemo.h"\
	".\maze.h"\
	".\poly.h"\
	".\stdwin.h"\
	".\xform.h"\
	

"$(INTDIR)\init.obj" : $(SOURCE) $(DEP_CPP_INIT_) "$(INTDIR)"\
 "$(INTDIR)\gdidemo.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_INIT_) "$(INTDIR)"\
 "$(INTDIR)\gdidemo.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\maze.c

!IF  "$(CFG)" == "GdiDemo - Win32 Release"

DEP_CPP_MAZE_=\
	".\gdidemo.h"\
	".\maze.h"\
	".\stdwin.h"\
	

"$(INTDIR)\maze.obj" : $(SOURCE) $(DEP_CPP_MAZE_) "$(INTDIR)"\
 "$(INTDIR)\gdidemo.pch"


!ELSEIF  "$(CFG)" == "GdiDemo - Win32 Debug"

DEP_CPP_MAZE_=\
	".\stdwin.h"\
	".\gdidemo.h"\
	".\maze.h"\
	

"$(INTDIR)\maze.obj" : $(SOURCE) $(DEP_CPP_MAZE_) "$(INTDIR)"\
 "$(INTDIR)\gdidemo.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_MAZE_) "$(INTDIR)"\
 "$(INTDIR)\gdidemo.pch"


!ELSEIF  "$(CFG)" == "GdiDemo - Macintosh Release"

DEP_CPP_MAZE_=\
	".\stdwin.h"\
	".\gdidemo.h"\
	".\maze.h"\
	

"$(INTDIR)\maze.obj" : $(SOURCE) $(DEP_CPP_MAZE_) "$(INTDIR)"\
 "$(INTDIR)\gdidemo.pch"


!ELSEIF  "$(CFG)" == "GdiDemo - Macintosh Debug"

DEP_CPP_MAZE_=\
	".\gdidemo.h"\
	".\maze.h"\
	".\stdwin.h"\
	

"$(INTDIR)\maze.obj" : $(SOURCE) $(DEP_CPP_MAZE_) "$(INTDIR)"\
 "$(INTDIR)\gdidemo.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_MAZE_) "$(INTDIR)"\
 "$(INTDIR)\gdidemo.pch"


!ELSEIF  "$(CFG)" == "GdiDemo - Power Macintosh Release"

DEP_CPP_MAZE_=\
	".\gdidemo.h"\
	".\maze.h"\
	".\stdwin.h"\
	

"$(INTDIR)\maze.obj" : $(SOURCE) $(DEP_CPP_MAZE_) "$(INTDIR)"\
 "$(INTDIR)\gdidemo.pch"


!ELSEIF  "$(CFG)" == "GdiDemo - Power Macintosh Debug"

DEP_CPP_MAZE_=\
	".\gdidemo.h"\
	".\maze.h"\
	".\stdwin.h"\
	

"$(INTDIR)\maze.obj" : $(SOURCE) $(DEP_CPP_MAZE_) "$(INTDIR)"\
 "$(INTDIR)\gdidemo.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_MAZE_) "$(INTDIR)"\
 "$(INTDIR)\gdidemo.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\poly.c

!IF  "$(CFG)" == "GdiDemo - Win32 Release"

DEP_CPP_POLY_=\
	".\gdidemo.h"\
	".\poly.h"\
	".\stdwin.h"\
	

"$(INTDIR)\poly.obj" : $(SOURCE) $(DEP_CPP_POLY_) "$(INTDIR)"\
 "$(INTDIR)\gdidemo.pch"


!ELSEIF  "$(CFG)" == "GdiDemo - Win32 Debug"

DEP_CPP_POLY_=\
	".\stdwin.h"\
	".\gdidemo.h"\
	".\poly.h"\
	

"$(INTDIR)\poly.obj" : $(SOURCE) $(DEP_CPP_POLY_) "$(INTDIR)"\
 "$(INTDIR)\gdidemo.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_POLY_) "$(INTDIR)"\
 "$(INTDIR)\gdidemo.pch"


!ELSEIF  "$(CFG)" == "GdiDemo - Macintosh Release"

DEP_CPP_POLY_=\
	".\stdwin.h"\
	".\gdidemo.h"\
	".\poly.h"\
	

"$(INTDIR)\poly.obj" : $(SOURCE) $(DEP_CPP_POLY_) "$(INTDIR)"\
 "$(INTDIR)\gdidemo.pch"


!ELSEIF  "$(CFG)" == "GdiDemo - Macintosh Debug"

DEP_CPP_POLY_=\
	".\gdidemo.h"\
	".\poly.h"\
	".\stdwin.h"\
	

"$(INTDIR)\poly.obj" : $(SOURCE) $(DEP_CPP_POLY_) "$(INTDIR)"\
 "$(INTDIR)\gdidemo.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_POLY_) "$(INTDIR)"\
 "$(INTDIR)\gdidemo.pch"


!ELSEIF  "$(CFG)" == "GdiDemo - Power Macintosh Release"

DEP_CPP_POLY_=\
	".\gdidemo.h"\
	".\poly.h"\
	".\stdwin.h"\
	

"$(INTDIR)\poly.obj" : $(SOURCE) $(DEP_CPP_POLY_) "$(INTDIR)"\
 "$(INTDIR)\gdidemo.pch"


!ELSEIF  "$(CFG)" == "GdiDemo - Power Macintosh Debug"

DEP_CPP_POLY_=\
	".\gdidemo.h"\
	".\poly.h"\
	".\stdwin.h"\
	

"$(INTDIR)\poly.obj" : $(SOURCE) $(DEP_CPP_POLY_) "$(INTDIR)"\
 "$(INTDIR)\gdidemo.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_POLY_) "$(INTDIR)"\
 "$(INTDIR)\gdidemo.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\stdwin.c
DEP_CPP_STDWI=\
	".\stdwin.h"\
	

!IF  "$(CFG)" == "GdiDemo - Win32 Release"

# ADD CPP /Yc"stdwin.h"

BuildCmds= \
	$(CPP) /nologo /ML /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS"\
 /Fp"$(INTDIR)/gdidemo.pch" /Yc"stdwin.h" /Fo"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\stdwin.obj" : $(SOURCE) $(DEP_CPP_STDWI) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\gdidemo.pch" : $(SOURCE) $(DEP_CPP_STDWI) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "GdiDemo - Win32 Debug"

# ADD CPP /Yc"stdwin.h"

BuildCmds= \
	$(CPP) /nologo /MLd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /Fp"$(INTDIR)/gdidemo.pch" /Yc"stdwin.h" /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c\
 $(SOURCE) \
	

"$(INTDIR)\stdwin.obj" : $(SOURCE) $(DEP_CPP_STDWI) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_STDWI) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\gdidemo.pch" : $(SOURCE) $(DEP_CPP_STDWI) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "GdiDemo - Macintosh Release"

# ADD CPP /Yc"stdwin.h"

BuildCmds= \
	$(CPP) /nologo /AL /Q68s /W3 /GX /O2 /D "_WINDOWS" /D "_MAC" /D "_68K_" /D\
 "WIN32" /D "NDEBUG" /Fp"$(INTDIR)/gdidemo.pch" /Yc"stdwin.h" /Fo"$(INTDIR)/" /c\
 $(SOURCE) \
	

"$(INTDIR)\stdwin.obj" : $(SOURCE) $(DEP_CPP_STDWI) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\gdidemo.pch" : $(SOURCE) $(DEP_CPP_STDWI) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "GdiDemo - Macintosh Debug"

# ADD CPP /Yc"stdwin.h"

BuildCmds= \
	$(CPP) /nologo /AL /Q68s /Q68m /W3 /GX /Zi /Od /D "_WINDOWS" /D "_MAC" /D\
 "_68K_" /D "WIN32" /D "_DEBUG" /Fp"$(INTDIR)/gdidemo.pch" /Yc"stdwin.h"\
 /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\stdwin.obj" : $(SOURCE) $(DEP_CPP_STDWI) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_STDWI) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\gdidemo.pch" : $(SOURCE) $(DEP_CPP_STDWI) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "GdiDemo - Power Macintosh Release"

# ADD CPP /Yc"stdwin.h"

BuildCmds= \
	$(CPP) /nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "_MAC" /D "_MPPC_" /D "_WINDOWS"\
 /D "WIN32" /D "_WLMDLL" /Fp"$(INTDIR)/gdidemo.pch" /Yc"stdwin.h"\
 /Fo"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\stdwin.obj" : $(SOURCE) $(DEP_CPP_STDWI) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\gdidemo.pch" : $(SOURCE) $(DEP_CPP_STDWI) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "GdiDemo - Power Macintosh Debug"

# ADD CPP /Yc"stdwin.h"

BuildCmds= \
	$(CPP) /nologo /MDd /W3 /GX /Zi /Od /D "_DEBUG" /D "_MAC" /D "_MPPC_" /D\
 "_WINDOWS" /D "WIN32" /D "_WLMDLL" /Fp"$(INTDIR)/gdidemo.pch" /Yc"stdwin.h"\
 /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\stdwin.obj" : $(SOURCE) $(DEP_CPP_STDWI) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_STDWI) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\gdidemo.pch" : $(SOURCE) $(DEP_CPP_STDWI) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\wininfo.c

!IF  "$(CFG)" == "GdiDemo - Win32 Release"

DEP_CPP_WININ=\
	".\stdwin.h"\
	".\gdidemo.h"\
	

"$(INTDIR)\wininfo.obj" : $(SOURCE) $(DEP_CPP_WININ) "$(INTDIR)"\
 "$(INTDIR)\gdidemo.pch"


!ELSEIF  "$(CFG)" == "GdiDemo - Win32 Debug"

DEP_CPP_WININ=\
	".\stdwin.h"\
	".\gdidemo.h"\
	

"$(INTDIR)\wininfo.obj" : $(SOURCE) $(DEP_CPP_WININ) "$(INTDIR)"\
 "$(INTDIR)\gdidemo.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_WININ) "$(INTDIR)"\
 "$(INTDIR)\gdidemo.pch"


!ELSEIF  "$(CFG)" == "GdiDemo - Macintosh Release"

DEP_CPP_WININ=\
	".\stdwin.h"\
	".\gdidemo.h"\
	

"$(INTDIR)\wininfo.obj" : $(SOURCE) $(DEP_CPP_WININ) "$(INTDIR)"\
 "$(INTDIR)\gdidemo.pch"


!ELSEIF  "$(CFG)" == "GdiDemo - Macintosh Debug"

DEP_CPP_WININ=\
	".\gdidemo.h"\
	".\stdwin.h"\
	

"$(INTDIR)\wininfo.obj" : $(SOURCE) $(DEP_CPP_WININ) "$(INTDIR)"\
 "$(INTDIR)\gdidemo.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_WININ) "$(INTDIR)"\
 "$(INTDIR)\gdidemo.pch"


!ELSEIF  "$(CFG)" == "GdiDemo - Power Macintosh Release"

DEP_CPP_WININ=\
	".\gdidemo.h"\
	".\stdwin.h"\
	

"$(INTDIR)\wininfo.obj" : $(SOURCE) $(DEP_CPP_WININ) "$(INTDIR)"\
 "$(INTDIR)\gdidemo.pch"


!ELSEIF  "$(CFG)" == "GdiDemo - Power Macintosh Debug"

DEP_CPP_WININ=\
	".\gdidemo.h"\
	".\stdwin.h"\
	

"$(INTDIR)\wininfo.obj" : $(SOURCE) $(DEP_CPP_WININ) "$(INTDIR)"\
 "$(INTDIR)\gdidemo.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_WININ) "$(INTDIR)"\
 "$(INTDIR)\gdidemo.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\xform.c

!IF  "$(CFG)" == "GdiDemo - Win32 Release"

DEP_CPP_XFORM=\
	".\stdwin.h"\
	".\gdidemo.h"\
	".\xform.h"\
	

"$(INTDIR)\xform.obj" : $(SOURCE) $(DEP_CPP_XFORM) "$(INTDIR)"\
 "$(INTDIR)\gdidemo.pch"


!ELSEIF  "$(CFG)" == "GdiDemo - Win32 Debug"

DEP_CPP_XFORM=\
	".\stdwin.h"\
	".\gdidemo.h"\
	".\xform.h"\
	

"$(INTDIR)\xform.obj" : $(SOURCE) $(DEP_CPP_XFORM) "$(INTDIR)"\
 "$(INTDIR)\gdidemo.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_XFORM) "$(INTDIR)"\
 "$(INTDIR)\gdidemo.pch"


!ELSEIF  "$(CFG)" == "GdiDemo - Macintosh Release"

DEP_CPP_XFORM=\
	".\stdwin.h"\
	".\gdidemo.h"\
	".\xform.h"\
	

"$(INTDIR)\xform.obj" : $(SOURCE) $(DEP_CPP_XFORM) "$(INTDIR)"\
 "$(INTDIR)\gdidemo.pch"


!ELSEIF  "$(CFG)" == "GdiDemo - Macintosh Debug"

DEP_CPP_XFORM=\
	".\gdidemo.h"\
	".\stdwin.h"\
	".\xform.h"\
	

"$(INTDIR)\xform.obj" : $(SOURCE) $(DEP_CPP_XFORM) "$(INTDIR)"\
 "$(INTDIR)\gdidemo.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_XFORM) "$(INTDIR)"\
 "$(INTDIR)\gdidemo.pch"


!ELSEIF  "$(CFG)" == "GdiDemo - Power Macintosh Release"

DEP_CPP_XFORM=\
	".\gdidemo.h"\
	".\stdwin.h"\
	".\xform.h"\
	

"$(INTDIR)\xform.obj" : $(SOURCE) $(DEP_CPP_XFORM) "$(INTDIR)"\
 "$(INTDIR)\gdidemo.pch"


!ELSEIF  "$(CFG)" == "GdiDemo - Power Macintosh Debug"

DEP_CPP_XFORM=\
	".\gdidemo.h"\
	".\stdwin.h"\
	".\xform.h"\
	

"$(INTDIR)\xform.obj" : $(SOURCE) $(DEP_CPP_XFORM) "$(INTDIR)"\
 "$(INTDIR)\gdidemo.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_XFORM) "$(INTDIR)"\
 "$(INTDIR)\gdidemo.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\gdid.r

!IF  "$(CFG)" == "GdiDemo - Win32 Release"

!ELSEIF  "$(CFG)" == "GdiDemo - Win32 Debug"

!ELSEIF  "$(CFG)" == "GdiDemo - Macintosh Release"

DEP_MRC_GDID_=\
	{$(INCLUDE)}"\mrc\TYPES.R"\
	{$(INCLUDE)}"\mrc\BALLOONS.R"\
	

"$(INTDIR)\gdid.rsc" : $(SOURCE) $(DEP_MRC_GDID_) "$(INTDIR)"
   $(MRC) /o"$(INTDIR)/gdid.rsc" /D "_68K_" /D "_MAC" /D "NDEBUG" /l 0x409\
 /NOLOGO $(SOURCE)


!ELSEIF  "$(CFG)" == "GdiDemo - Macintosh Debug"

DEP_MRC_GDID_=\
	{$(INCLUDE)}"\mrc\TYPES.R"\
	{$(INCLUDE)}"\mrc\BALLOONS.R"\
	

"$(INTDIR)\gdid.rsc" : $(SOURCE) $(DEP_MRC_GDID_) "$(INTDIR)"
   $(MRC) /o"$(INTDIR)/gdid.rsc" /D "_68K_" /D "_MAC" /D "_DEBUG" /l 0x409\
 /NOLOGO $(SOURCE)


!ELSEIF  "$(CFG)" == "GdiDemo - Power Macintosh Release"

DEP_MRC_GDID_=\
	{$(INCLUDE)}"\mrc\TYPES.R"\
	{$(INCLUDE)}"\mrc\BALLOONS.R"\
	

"$(INTDIR)\gdid.rsc" : $(SOURCE) $(DEP_MRC_GDID_) "$(INTDIR)"
   $(MRC) /o"$(INTDIR)/gdid.rsc" /D "_MPPC_" /D "_MAC" /D "NDEBUG" /l 0x409\
 /NOLOGO $(SOURCE)


!ELSEIF  "$(CFG)" == "GdiDemo - Power Macintosh Debug"

DEP_MRC_GDID_=\
	{$(INCLUDE)}"\mrc\TYPES.R"\
	{$(INCLUDE)}"\mrc\BALLOONS.R"\
	

"$(INTDIR)\gdid.rsc" : $(SOURCE) $(DEP_MRC_GDID_) "$(INTDIR)"
   $(MRC) /o"$(INTDIR)/gdid.rsc" /D "_MPPC_" /D "_MAC" /D "_DEBUG" /l 0x409\
 /NOLOGO $(SOURCE)


!ENDIF 

# End Source File
# End Target
# End Project
################################################################################
