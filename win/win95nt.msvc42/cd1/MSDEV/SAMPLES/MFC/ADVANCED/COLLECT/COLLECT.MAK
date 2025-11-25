# Microsoft Visual C++ Generated NMAKE File, Format Version 40000
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101
# TARGTYPE "Power Macintosh Application" 0x0401
# TARGTYPE "Macintosh Application" 0x0301

!IF "$(CFG)" == ""
CFG=Collect - Win32 Debug
!MESSAGE No configuration specified.  Defaulting to Collect - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "Collect - Win32 Release" && "$(CFG)" !=\
 "Collect - Win32 Debug" && "$(CFG)" != "Collect - Macintosh Release" &&\
 "$(CFG)" != "Collect - Macintosh Debug" && "$(CFG)" !=\
 "Collect - Power Macintosh Release" && "$(CFG)" !=\
 "Collect - Power Macintosh Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "collect.mak" CFG="Collect - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Collect - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "Collect - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE "Collect - Macintosh Release" (based on "Macintosh Application")
!MESSAGE "Collect - Macintosh Debug" (based on "Macintosh Application")
!MESSAGE "Collect - Power Macintosh Release" (based on\
 "Power Macintosh Application")
!MESSAGE "Collect - Power Macintosh Debug" (based on\
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
# PROP Target_Last_Scanned "Collect - Power Macintosh Debug"

!IF  "$(CFG)" == "Collect - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir Collect_
# PROP BASE Intermediate_Dir Collect_
# PROP BASE Target_Dir
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 0
# PROP Output_Dir Release
# PROP Intermediate_Dir Release
# PROP Target_Dir
OUTDIR=.\Release
INTDIR=.\Release

ALL : "$(OUTDIR)\collect.exe"

CLEAN : 
	-@erase ".\Release\collect.exe"
	-@erase ".\Release\stdafx.obj"
	-@erase ".\Release\dwarryvw.obj"
	-@erase ".\Release\collect.pch"
	-@erase ".\Release\mainfrm.obj"
	-@erase ".\Release\mapdwvw.obj"
	-@erase ".\Release\typtrmap.obj"
	-@erase ".\Release\intlstvw.obj"
	-@erase ".\Release\mapssvw.obj"
	-@erase ".\Release\collect.obj"
	-@erase ".\Release\ptarryvw.obj"
	-@erase ".\Release\collevw.obj"
	-@erase ".\Release\colledoc.obj"
	-@erase ".\Release\strlstvw.obj"
	-@erase ".\Release\typlstvw.obj"
	-@erase ".\Release\typaryvw.obj"
	-@erase ".\Release\collect.res"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"Stdafx.h" /c
CPP_PROJ=/nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/collect.pch" /Yu"Stdafx.h" /Fo"$(INTDIR)/"\
 /c 
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
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/collect.res" /d "NDEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/collect.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 /nologo /subsystem:windows /machine:I386
LINK32_FLAGS=/nologo /subsystem:windows /incremental:no\
 /pdb:"$(OUTDIR)/collect.pdb" /machine:I386 /out:"$(OUTDIR)/collect.exe" 
LINK32_OBJS= \
	"$(INTDIR)/stdafx.obj" \
	"$(INTDIR)/dwarryvw.obj" \
	"$(INTDIR)/mainfrm.obj" \
	"$(INTDIR)/mapdwvw.obj" \
	"$(INTDIR)/typtrmap.obj" \
	"$(INTDIR)/intlstvw.obj" \
	"$(INTDIR)/mapssvw.obj" \
	"$(INTDIR)/collect.obj" \
	"$(INTDIR)/ptarryvw.obj" \
	"$(INTDIR)/collevw.obj" \
	"$(INTDIR)/colledoc.obj" \
	"$(INTDIR)/strlstvw.obj" \
	"$(INTDIR)/typlstvw.obj" \
	"$(INTDIR)/typaryvw.obj" \
	"$(INTDIR)/collect.res"

"$(OUTDIR)\collect.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "Collect - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir Debug
# PROP BASE Intermediate_Dir Debug
# PROP BASE Target_Dir
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 1
# PROP Output_Dir Debug
# PROP Intermediate_Dir Debug
# PROP Target_Dir
OUTDIR=.\Debug
INTDIR=.\Debug

ALL : "$(OUTDIR)\collect.exe"

CLEAN : 
	-@erase ".\Debug\vc40.pdb"
	-@erase ".\Debug\collect.pch"
	-@erase ".\Debug\collect.exe"
	-@erase ".\Debug\typtrmap.obj"
	-@erase ".\Debug\intlstvw.obj"
	-@erase ".\Debug\mainfrm.obj"
	-@erase ".\Debug\ptarryvw.obj"
	-@erase ".\Debug\mapdwvw.obj"
	-@erase ".\Debug\colledoc.obj"
	-@erase ".\Debug\strlstvw.obj"
	-@erase ".\Debug\mapssvw.obj"
	-@erase ".\Debug\collect.obj"
	-@erase ".\Debug\typlstvw.obj"
	-@erase ".\Debug\collevw.obj"
	-@erase ".\Debug\typaryvw.obj"
	-@erase ".\Debug\stdafx.obj"
	-@erase ".\Debug\dwarryvw.obj"
	-@erase ".\Debug\collect.res"
	-@erase ".\Debug\collect.ilk"
	-@erase ".\Debug\collect.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"Stdafx.h" /c
CPP_PROJ=/nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/collect.pch" /Yu"Stdafx.h"\
 /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
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
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/collect.res" /d "_DEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/collect.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386
# ADD LINK32 /nologo /subsystem:windows /debug /machine:I386
LINK32_FLAGS=/nologo /subsystem:windows /incremental:yes\
 /pdb:"$(OUTDIR)/collect.pdb" /debug /machine:I386 /out:"$(OUTDIR)/collect.exe" 
LINK32_OBJS= \
	"$(INTDIR)/typtrmap.obj" \
	"$(INTDIR)/intlstvw.obj" \
	"$(INTDIR)/mainfrm.obj" \
	"$(INTDIR)/ptarryvw.obj" \
	"$(INTDIR)/mapdwvw.obj" \
	"$(INTDIR)/colledoc.obj" \
	"$(INTDIR)/strlstvw.obj" \
	"$(INTDIR)/mapssvw.obj" \
	"$(INTDIR)/collect.obj" \
	"$(INTDIR)/typlstvw.obj" \
	"$(INTDIR)/collevw.obj" \
	"$(INTDIR)/typaryvw.obj" \
	"$(INTDIR)/stdafx.obj" \
	"$(INTDIR)/dwarryvw.obj" \
	"$(INTDIR)/collect.res"

"$(OUTDIR)\collect.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "Collect - Macintosh Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir MacRel
# PROP BASE Intermediate_Dir MacRel
# PROP BASE Target_Dir
# PROP Use_MFC 1
# PROP Use_Debug_Libraries 0
# PROP Output_Dir MacRel
# PROP Intermediate_Dir MacRel
# PROP Target_Dir
OUTDIR=.\MacRel
INTDIR=.\MacRel

ALL : "$(OUTDIR)\collect.exe" "$(OUTDIR)\collect.trg"

CLEAN : 
	-@erase ".\MacRel\collect.trg"
	-@erase ".\MacRel\collect.exe"
	-@erase ".\MacRel\intlstvw.obj"
	-@erase ".\MacRel\collect.pch"
	-@erase ".\MacRel\stdafx.obj"
	-@erase ".\MacRel\ptarryvw.obj"
	-@erase ".\MacRel\colledoc.obj"
	-@erase ".\MacRel\strlstvw.obj"
	-@erase ".\MacRel\mainfrm.obj"
	-@erase ".\MacRel\typlstvw.obj"
	-@erase ".\MacRel\mapdwvw.obj"
	-@erase ".\MacRel\typaryvw.obj"
	-@erase ".\MacRel\mapssvw.obj"
	-@erase ".\MacRel\collect.obj"
	-@erase ".\MacRel\collevw.obj"
	-@erase ".\MacRel\dwarryvw.obj"
	-@erase ".\MacRel\typtrmap.obj"
	-@erase ".\MacRel\collect.rsc"
	-@erase ".\MacRel\collmac.rsc"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

MTL=mktyplib.exe
# ADD BASE MTL /nologo /D "NDEBUG" /mac
# ADD MTL /nologo /D "NDEBUG" /mac
MTL_PROJ=/nologo /D "NDEBUG" /mac 
CPP=cl.exe
# ADD BASE CPP /nologo /AL /Q68s /W3 /GX /O2 /D "_WINDOWS" /D "_MAC" /D "_68K_" /D "WIN32" /D "NDEBUG" /YX /c
# ADD CPP /nologo /AL /Q68s /W3 /GX /O2 /D "_WINDOWS" /D "_MAC" /D "_68K_" /D "WIN32" /D "NDEBUG" /D "_MBCS" /Yu"Stdafx.h" /c
CPP_PROJ=/nologo /AL /Q68s /W3 /GX /O2 /D "_WINDOWS" /D "_MAC" /D "_68K_" /D\
 "WIN32" /D "NDEBUG" /D "_MBCS" /Fp"$(INTDIR)/collect.pch" /Yu"Stdafx.h"\
 /Fo"$(INTDIR)/" /c 
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
RSC_PROJ=/l 0x409 /r /m /fo"$(INTDIR)/collect.rsc" /d "_MAC" /d "_68K_" /d\
 "NDEBUG" 
MRC=mrc.exe
# ADD BASE MRC /D "_68K_" /D "_MAC" /D "NDEBUG" /NOLOGO
# ADD MRC /D "_68K_" /D "_MAC" /D "NDEBUG" /NOLOGO
MRC_PROJ=/D "_68K_" /D "_MAC" /D "NDEBUG" /l 0x409 /NOLOGO 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/collect.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 wlm.lib /nologo /MAC:bundle /machine:M68K
# ADD LINK32 /nologo /MAC:bundle /MAC:creator="COLL" /machine:M68K
LINK32_FLAGS=/nologo /MAC:bundle /MAC:type="APPL" /MAC:creator="COLL"\
 /pdb:"$(OUTDIR)/collect.pdb" /machine:M68K /out:"$(OUTDIR)/collect.exe" 
LINK32_OBJS= \
	"$(INTDIR)/intlstvw.obj" \
	"$(INTDIR)/stdafx.obj" \
	"$(INTDIR)/ptarryvw.obj" \
	"$(INTDIR)/colledoc.obj" \
	"$(INTDIR)/strlstvw.obj" \
	"$(INTDIR)/mainfrm.obj" \
	"$(INTDIR)/typlstvw.obj" \
	"$(INTDIR)/mapdwvw.obj" \
	"$(INTDIR)/typaryvw.obj" \
	"$(INTDIR)/mapssvw.obj" \
	"$(INTDIR)/collect.obj" \
	"$(INTDIR)/collevw.obj" \
	"$(INTDIR)/dwarryvw.obj" \
	"$(INTDIR)/typtrmap.obj" \
	"$(INTDIR)/collect.rsc" \
	"$(INTDIR)/collmac.rsc"

"$(OUTDIR)\collect.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

MFILE32=mfile.exe
# ADD BASE MFILE32 COPY /NOLOGO
# ADD MFILE32 COPY /NOLOGO
MFILE32_FLAGS=COPY /NOLOGO 
MFILE32_FILES= \
	"$(OUTDIR)/collect.exe"

"$(OUTDIR)\collect.trg" : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\MacRel\collect.exe\
 "$(MFILE32_DEST):collect.exe">"$(OUTDIR)\collect.trg"

DOWNLOAD : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\MacRel\collect.exe\
 "$(MFILE32_DEST):collect.exe">"$(OUTDIR)\collect.trg"

!ELSEIF  "$(CFG)" == "Collect - Macintosh Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir MacDbg
# PROP BASE Intermediate_Dir MacDbg
# PROP BASE Target_Dir
# PROP Use_MFC 1
# PROP Use_Debug_Libraries 1
# PROP Output_Dir MacDbg
# PROP Intermediate_Dir MacDbg
# PROP Target_Dir
OUTDIR=.\MacDbg
INTDIR=.\MacDbg

ALL : "$(OUTDIR)\collect.exe" "$(OUTDIR)\collect.trg"

CLEAN : 
	-@erase ".\MacDbg\vc40.pdb"
	-@erase ".\MacDbg\collect.pch"
	-@erase ".\MacDbg\collect.pdb"
	-@erase ".\MacDbg\collevw.obj"
	-@erase ".\MacDbg\typtrmap.obj"
	-@erase ".\MacDbg\stdafx.obj"
	-@erase ".\MacDbg\intlstvw.obj"
	-@erase ".\MacDbg\ptarryvw.obj"
	-@erase ".\MacDbg\colledoc.obj"
	-@erase ".\MacDbg\strlstvw.obj"
	-@erase ".\MacDbg\typlstvw.obj"
	-@erase ".\MacDbg\typaryvw.obj"
	-@erase ".\MacDbg\mainfrm.obj"
	-@erase ".\MacDbg\mapdwvw.obj"
	-@erase ".\MacDbg\dwarryvw.obj"
	-@erase ".\MacDbg\mapssvw.obj"
	-@erase ".\MacDbg\collect.obj"
	-@erase ".\MacDbg\collmac.rsc"
	-@erase ".\MacDbg\collect.rsc"
	-@erase ".\MacDbg\collect.trg"
	-@erase ".\MacDbg\collect.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

MTL=mktyplib.exe
# ADD BASE MTL /nologo /D "_DEBUG" /mac
# ADD MTL /nologo /D "_DEBUG" /mac
MTL_PROJ=/nologo /D "_DEBUG" /mac 
CPP=cl.exe
# ADD BASE CPP /nologo /AL /Q68s /Q68m /W3 /GX /Zi /Od /D "_WINDOWS" /D "_MAC" /D "_68K_" /D "WIN32" /D "_DEBUG" /YX /c
# ADD CPP /nologo /AL /Q68s /Q68m /W3 /GX /Zi /Od /D "_WINDOWS" /D "_MAC" /D "_68K_" /D "WIN32" /D "_DEBUG" /D "_MBCS" /Yu"Stdafx.h" /c
CPP_PROJ=/nologo /AL /Q68s /Q68m /W3 /GX /Zi /Od /D "_WINDOWS" /D "_MAC" /D\
 "_68K_" /D "WIN32" /D "_DEBUG" /D "_MBCS" /Fp"$(INTDIR)/collect.pch"\
 /Yu"Stdafx.h" /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
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
RSC_PROJ=/l 0x409 /r /m /fo"$(INTDIR)/collect.rsc" /d "_MAC" /d "_68K_" /d\
 "_DEBUG" 
MRC=mrc.exe
# ADD BASE MRC /D "_68K_" /D "_MAC" /D "_DEBUG" /NOLOGO
# ADD MRC /D "_68K_" /D "_MAC" /D "_DEBUG" /NOLOGO
MRC_PROJ=/D "_68K_" /D "_MAC" /D "_DEBUG" /l 0x409 /NOLOGO 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/collect.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 wlm.lib /nologo /MAC:bundle /debug /machine:M68K
# ADD LINK32 /nologo /MAC:bundle /MAC:creator="COLL" /debug /machine:M68K
LINK32_FLAGS=/nologo /MAC:bundle /MAC:type="APPL" /MAC:creator="COLL"\
 /pdb:"$(OUTDIR)/collect.pdb" /debug /machine:M68K /out:"$(OUTDIR)/collect.exe" 
LINK32_OBJS= \
	"$(INTDIR)/collevw.obj" \
	"$(INTDIR)/typtrmap.obj" \
	"$(INTDIR)/stdafx.obj" \
	"$(INTDIR)/intlstvw.obj" \
	"$(INTDIR)/ptarryvw.obj" \
	"$(INTDIR)/colledoc.obj" \
	"$(INTDIR)/strlstvw.obj" \
	"$(INTDIR)/typlstvw.obj" \
	"$(INTDIR)/typaryvw.obj" \
	"$(INTDIR)/mainfrm.obj" \
	"$(INTDIR)/mapdwvw.obj" \
	"$(INTDIR)/dwarryvw.obj" \
	"$(INTDIR)/mapssvw.obj" \
	"$(INTDIR)/collect.obj" \
	"$(INTDIR)/collmac.rsc" \
	"$(INTDIR)/collect.rsc"

"$(OUTDIR)\collect.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

MFILE32=mfile.exe
# ADD BASE MFILE32 COPY /NOLOGO
# ADD MFILE32 COPY /NOLOGO
MFILE32_FLAGS=COPY /NOLOGO 
MFILE32_FILES= \
	"$(OUTDIR)/collect.exe"

"$(OUTDIR)\collect.trg" : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\MacDbg\collect.exe\
 "$(MFILE32_DEST):collect.exe">"$(OUTDIR)\collect.trg"

DOWNLOAD : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\MacDbg\collect.exe\
 "$(MFILE32_DEST):collect.exe">"$(OUTDIR)\collect.trg"

!ELSEIF  "$(CFG)" == "Collect - Power Macintosh Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir PMacRel
# PROP BASE Intermediate_Dir PMacRel
# PROP BASE Target_Dir
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 0
# PROP Output_Dir PMacRel
# PROP Intermediate_Dir PMacRel
# PROP Target_Dir
OUTDIR=.\PMacRel
INTDIR=.\PMacRel

ALL : "$(OUTDIR)\collect.exe" "$(OUTDIR)\collect.trg"

CLEAN : 
	-@erase ".\PMacRel\collect.trg"
	-@erase ".\PMacRel\collect.exe"
	-@erase ".\PMacRel\mainfrm.obj"
	-@erase ".\PMacRel\collect.pch"
	-@erase ".\PMacRel\mapdwvw.obj"
	-@erase ".\PMacRel\colledoc.obj"
	-@erase ".\PMacRel\strlstvw.obj"
	-@erase ".\PMacRel\typlstvw.obj"
	-@erase ".\PMacRel\mapssvw.obj"
	-@erase ".\PMacRel\collect.obj"
	-@erase ".\PMacRel\typaryvw.obj"
	-@erase ".\PMacRel\collevw.obj"
	-@erase ".\PMacRel\dwarryvw.obj"
	-@erase ".\PMacRel\stdafx.obj"
	-@erase ".\PMacRel\typtrmap.obj"
	-@erase ".\PMacRel\intlstvw.obj"
	-@erase ".\PMacRel\ptarryvw.obj"
	-@erase ".\PMacRel\collect.rsc"
	-@erase ".\PMacRel\collmac.rsc"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

MTL=mktyplib.exe
# ADD BASE MTL /nologo /D "NDEBUG"
# ADD MTL /nologo /D "NDEBUG"
MTL_PROJ=/nologo /D "NDEBUG" 
CPP=cl.exe
# ADD BASE CPP /nologo /ML /W3 /GX /O2 /D "_MAC" /D "_MPPC_" /D "_WINDOWS" /D "WIN32" /D "NDEBUG" /YX /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "_MAC" /D "_MPPC_" /D "_WINDOWS" /D "WIN32" /D "_MBCS" /D "_AFXDLL" /Yu"Stdafx.h" /c
CPP_PROJ=/nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "_MAC" /D "_MPPC_" /D\
 "_WINDOWS" /D "WIN32" /D "_MBCS" /D "_AFXDLL" /Fp"$(INTDIR)/collect.pch"\
 /Yu"Stdafx.h" /Fo"$(INTDIR)/" /c 
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
RSC_PROJ=/l 0x409 /r /m /fo"$(INTDIR)/collect.rsc" /d "_MAC" /d "_MPPC_" /d\
 "NDEBUG" 
MRC=mrc.exe
# ADD BASE MRC /D "_MPPC_" /D "_MAC" /D "NDEBUG" /NOLOGO
# ADD MRC /D "_MPPC_" /D "_MAC" /D "NDEBUG" /NOLOGO
MRC_PROJ=/D "_MPPC_" /D "_MAC" /D "NDEBUG" /l 0x409 /NOLOGO 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/collect.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 cfrg.rsc /nologo /mac:bundle /machine:MPPC
# ADD LINK32 /nologo /mac:bundle /mac:creator="COLL" /machine:MPPC
LINK32_FLAGS=/nologo /mac:bundle /mac:type="APPL" /mac:creator="COLL"\
 /incremental:no /pdb:"$(OUTDIR)/collect.pdb" /machine:MPPC\
 /out:"$(OUTDIR)/collect.exe" 
LINK32_OBJS= \
	"$(INTDIR)/mainfrm.obj" \
	"$(INTDIR)/mapdwvw.obj" \
	"$(INTDIR)/colledoc.obj" \
	"$(INTDIR)/strlstvw.obj" \
	"$(INTDIR)/typlstvw.obj" \
	"$(INTDIR)/mapssvw.obj" \
	"$(INTDIR)/collect.obj" \
	"$(INTDIR)/typaryvw.obj" \
	"$(INTDIR)/collevw.obj" \
	"$(INTDIR)/dwarryvw.obj" \
	"$(INTDIR)/stdafx.obj" \
	"$(INTDIR)/typtrmap.obj" \
	"$(INTDIR)/intlstvw.obj" \
	"$(INTDIR)/ptarryvw.obj" \
	"$(INTDIR)/collect.rsc" \
	"$(INTDIR)/collmac.rsc"

"$(OUTDIR)\collect.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

MFILE32=mfile.exe
# ADD BASE MFILE32 COPY /NOLOGO
# ADD MFILE32 COPY /NOLOGO
MFILE32_FLAGS=COPY /NOLOGO 
MFILE32_FILES= \
	"$(OUTDIR)/collect.exe"

"$(OUTDIR)\collect.trg" : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\PMacRel\collect.exe\
 "$(MFILE32_DEST):collect.exe">"$(OUTDIR)\collect.trg"

DOWNLOAD : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\PMacRel\collect.exe\
 "$(MFILE32_DEST):collect.exe">"$(OUTDIR)\collect.trg"

!ELSEIF  "$(CFG)" == "Collect - Power Macintosh Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir PMacDbg
# PROP BASE Intermediate_Dir PMacDbg
# PROP BASE Target_Dir
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 1
# PROP Output_Dir PMacDbg
# PROP Intermediate_Dir PMacDbg
# PROP Target_Dir
OUTDIR=.\PMacDbg
INTDIR=.\PMacDbg

ALL : "$(OUTDIR)\collect.exe" "$(OUTDIR)\collect.trg"

CLEAN : 
	-@erase ".\PMacDbg\vc40.pdb"
	-@erase ".\PMacDbg\collect.pch"
	-@erase ".\PMacDbg\collect.ilk"
	-@erase ".\PMacDbg\colledoc.obj"
	-@erase ".\PMacDbg\strlstvw.obj"
	-@erase ".\PMacDbg\typlstvw.obj"
	-@erase ".\PMacDbg\mainfrm.obj"
	-@erase ".\PMacDbg\typaryvw.obj"
	-@erase ".\PMacDbg\mapdwvw.obj"
	-@erase ".\PMacDbg\stdafx.obj"
	-@erase ".\PMacDbg\mapssvw.obj"
	-@erase ".\PMacDbg\collect.obj"
	-@erase ".\PMacDbg\dwarryvw.obj"
	-@erase ".\PMacDbg\collevw.obj"
	-@erase ".\PMacDbg\typtrmap.obj"
	-@erase ".\PMacDbg\intlstvw.obj"
	-@erase ".\PMacDbg\ptarryvw.obj"
	-@erase ".\PMacDbg\collect.rsc"
	-@erase ".\PMacDbg\collmac.rsc"
	-@erase ".\PMacDbg\collect.pdb"
	-@erase ".\PMacDbg\collect.trg"
	-@erase ".\PMacDbg\collect.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

MTL=mktyplib.exe
# ADD BASE MTL /nologo /D "_DEBUG"
# ADD MTL /nologo /D "_DEBUG"
MTL_PROJ=/nologo /D "_DEBUG" 
CPP=cl.exe
# ADD BASE CPP /nologo /W3 /GX /Zi /Od /D "_MAC" /D "_MPPC_" /D "_WINDOWS" /D "WIN32" /D "_DEBUG" /YX /c
# ADD CPP /nologo /MDd /W3 /GX /Zi /Od /D "_DEBUG" /D "_MAC" /D "_MPPC_" /D "_WINDOWS" /D "WIN32" /D "_MBCS" /D "_AFXDLL" /Yu"Stdafx.h" /c
CPP_PROJ=/nologo /MDd /W3 /GX /Zi /Od /D "_DEBUG" /D "_MAC" /D "_MPPC_" /D\
 "_WINDOWS" /D "WIN32" /D "_MBCS" /D "_AFXDLL" /Fp"$(INTDIR)/collect.pch"\
 /Yu"Stdafx.h" /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
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
RSC_PROJ=/l 0x409 /r /m /fo"$(INTDIR)/collect.rsc" /d "_MAC" /d "_MPPC_" /d\
 "_DEBUG" 
MRC=mrc.exe
# ADD BASE MRC /D "_MPPC_" /D "_MAC" /D "_DEBUG" /NOLOGO
# ADD MRC /D "_MPPC_" /D "_MAC" /D "_DEBUG" /NOLOGO
MRC_PROJ=/D "_MPPC_" /D "_MAC" /D "_DEBUG" /l 0x409 /NOLOGO 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/collect.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 cfrg.rsc /nologo /mac:bundle /debug /machine:MPPC
# ADD LINK32 /nologo /mac:bundle /mac:creator="COLL" /debug /machine:MPPC
LINK32_FLAGS=/nologo /mac:bundle /mac:type="APPL" /mac:creator="COLL"\
 /incremental:yes /pdb:"$(OUTDIR)/collect.pdb" /debug /machine:MPPC\
 /out:"$(OUTDIR)/collect.exe" 
LINK32_OBJS= \
	"$(INTDIR)/colledoc.obj" \
	"$(INTDIR)/strlstvw.obj" \
	"$(INTDIR)/typlstvw.obj" \
	"$(INTDIR)/mainfrm.obj" \
	"$(INTDIR)/typaryvw.obj" \
	"$(INTDIR)/mapdwvw.obj" \
	"$(INTDIR)/stdafx.obj" \
	"$(INTDIR)/mapssvw.obj" \
	"$(INTDIR)/collect.obj" \
	"$(INTDIR)/dwarryvw.obj" \
	"$(INTDIR)/collevw.obj" \
	"$(INTDIR)/typtrmap.obj" \
	"$(INTDIR)/intlstvw.obj" \
	"$(INTDIR)/ptarryvw.obj" \
	"$(INTDIR)/collect.rsc" \
	"$(INTDIR)/collmac.rsc"

"$(OUTDIR)\collect.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

MFILE32=mfile.exe
# ADD BASE MFILE32 COPY /NOLOGO
# ADD MFILE32 COPY /NOLOGO
MFILE32_FLAGS=COPY /NOLOGO 
MFILE32_FILES= \
	"$(OUTDIR)/collect.exe"

"$(OUTDIR)\collect.trg" : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\PMacDbg\collect.exe\
 "$(MFILE32_DEST):collect.exe">"$(OUTDIR)\collect.trg"

DOWNLOAD : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\PMacDbg\collect.exe\
 "$(MFILE32_DEST):collect.exe">"$(OUTDIR)\collect.trg"

!ENDIF 

################################################################################
# Begin Target

# Name "Collect - Win32 Release"
# Name "Collect - Win32 Debug"
# Name "Collect - Macintosh Release"
# Name "Collect - Macintosh Debug"
# Name "Collect - Power Macintosh Release"
# Name "Collect - Power Macintosh Debug"

!IF  "$(CFG)" == "Collect - Win32 Release"

!ELSEIF  "$(CFG)" == "Collect - Win32 Debug"

!ELSEIF  "$(CFG)" == "Collect - Macintosh Release"

!ELSEIF  "$(CFG)" == "Collect - Macintosh Debug"

!ELSEIF  "$(CFG)" == "Collect - Power Macintosh Release"

!ELSEIF  "$(CFG)" == "Collect - Power Macintosh Debug"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\typtrmap.cpp

!IF  "$(CFG)" == "Collect - Win32 Release"

DEP_CPP_TYPTR=\
	".\stdafx.h"\
	".\collect.h"\
	".\colledoc.h"\
	".\typtrmap.h"\
	

"$(INTDIR)\typtrmap.obj" : $(SOURCE) $(DEP_CPP_TYPTR) "$(INTDIR)"\
 "$(INTDIR)\collect.pch"


!ELSEIF  "$(CFG)" == "Collect - Win32 Debug"

DEP_CPP_TYPTR=\
	".\stdafx.h"\
	".\collect.h"\
	".\colledoc.h"\
	".\typtrmap.h"\
	

"$(INTDIR)\typtrmap.obj" : $(SOURCE) $(DEP_CPP_TYPTR) "$(INTDIR)"\
 "$(INTDIR)\collect.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_TYPTR) "$(INTDIR)"\
 "$(INTDIR)\collect.pch"


!ELSEIF  "$(CFG)" == "Collect - Macintosh Release"

DEP_CPP_TYPTR=\
	".\collect.h"\
	".\colledoc.h"\
	".\stdafx.h"\
	".\typtrmap.h"\
	

"$(INTDIR)\typtrmap.obj" : $(SOURCE) $(DEP_CPP_TYPTR) "$(INTDIR)"\
 "$(INTDIR)\collect.pch"


!ELSEIF  "$(CFG)" == "Collect - Macintosh Debug"

DEP_CPP_TYPTR=\
	".\stdafx.h"\
	".\collect.h"\
	".\colledoc.h"\
	".\typtrmap.h"\
	

"$(INTDIR)\typtrmap.obj" : $(SOURCE) $(DEP_CPP_TYPTR) "$(INTDIR)"\
 "$(INTDIR)\collect.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_TYPTR) "$(INTDIR)"\
 "$(INTDIR)\collect.pch"


!ELSEIF  "$(CFG)" == "Collect - Power Macintosh Release"

DEP_CPP_TYPTR=\
	".\stdafx.h"\
	".\collect.h"\
	".\colledoc.h"\
	".\typtrmap.h"\
	

"$(INTDIR)\typtrmap.obj" : $(SOURCE) $(DEP_CPP_TYPTR) "$(INTDIR)"\
 "$(INTDIR)\collect.pch"


!ELSEIF  "$(CFG)" == "Collect - Power Macintosh Debug"

DEP_CPP_TYPTR=\
	".\collect.h"\
	".\colledoc.h"\
	".\stdafx.h"\
	".\typtrmap.h"\
	

"$(INTDIR)\typtrmap.obj" : $(SOURCE) $(DEP_CPP_TYPTR) "$(INTDIR)"\
 "$(INTDIR)\collect.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_TYPTR) "$(INTDIR)"\
 "$(INTDIR)\collect.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\collect.rc
DEP_RSC_COLLE=\
	".\res\collect.ico"\
	".\res\toolbar.bmp"\
	".\res\collect.rc2"\
	

!IF  "$(CFG)" == "Collect - Win32 Release"


"$(INTDIR)\collect.res" : $(SOURCE) $(DEP_RSC_COLLE) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Collect - Win32 Debug"


"$(INTDIR)\collect.res" : $(SOURCE) $(DEP_RSC_COLLE) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Collect - Macintosh Release"


"$(INTDIR)\collect.rsc" : $(SOURCE) $(DEP_RSC_COLLE) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Collect - Macintosh Debug"


"$(INTDIR)\collect.rsc" : $(SOURCE) $(DEP_RSC_COLLE) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Collect - Power Macintosh Release"


"$(INTDIR)\collect.rsc" : $(SOURCE) $(DEP_RSC_COLLE) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Collect - Power Macintosh Debug"


"$(INTDIR)\collect.rsc" : $(SOURCE) $(DEP_RSC_COLLE) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\colledoc.cpp

!IF  "$(CFG)" == "Collect - Win32 Release"

DEP_CPP_COLLED=\
	".\stdafx.h"\
	".\collect.h"\
	".\colledoc.h"\
	

"$(INTDIR)\colledoc.obj" : $(SOURCE) $(DEP_CPP_COLLED) "$(INTDIR)"\
 "$(INTDIR)\collect.pch"


!ELSEIF  "$(CFG)" == "Collect - Win32 Debug"

DEP_CPP_COLLED=\
	".\stdafx.h"\
	".\collect.h"\
	".\colledoc.h"\
	

"$(INTDIR)\colledoc.obj" : $(SOURCE) $(DEP_CPP_COLLED) "$(INTDIR)"\
 "$(INTDIR)\collect.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_COLLED) "$(INTDIR)"\
 "$(INTDIR)\collect.pch"


!ELSEIF  "$(CFG)" == "Collect - Macintosh Release"

DEP_CPP_COLLED=\
	".\collect.h"\
	".\colledoc.h"\
	".\stdafx.h"\
	

"$(INTDIR)\colledoc.obj" : $(SOURCE) $(DEP_CPP_COLLED) "$(INTDIR)"\
 "$(INTDIR)\collect.pch"


!ELSEIF  "$(CFG)" == "Collect - Macintosh Debug"

DEP_CPP_COLLED=\
	".\stdafx.h"\
	".\collect.h"\
	".\colledoc.h"\
	

"$(INTDIR)\colledoc.obj" : $(SOURCE) $(DEP_CPP_COLLED) "$(INTDIR)"\
 "$(INTDIR)\collect.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_COLLED) "$(INTDIR)"\
 "$(INTDIR)\collect.pch"


!ELSEIF  "$(CFG)" == "Collect - Power Macintosh Release"

DEP_CPP_COLLED=\
	".\stdafx.h"\
	".\collect.h"\
	".\colledoc.h"\
	

"$(INTDIR)\colledoc.obj" : $(SOURCE) $(DEP_CPP_COLLED) "$(INTDIR)"\
 "$(INTDIR)\collect.pch"


!ELSEIF  "$(CFG)" == "Collect - Power Macintosh Debug"

DEP_CPP_COLLED=\
	".\collect.h"\
	".\colledoc.h"\
	".\stdafx.h"\
	

"$(INTDIR)\colledoc.obj" : $(SOURCE) $(DEP_CPP_COLLED) "$(INTDIR)"\
 "$(INTDIR)\collect.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_COLLED) "$(INTDIR)"\
 "$(INTDIR)\collect.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\collevw.cpp

!IF  "$(CFG)" == "Collect - Win32 Release"

DEP_CPP_COLLEV=\
	".\stdafx.h"\
	".\collect.h"\
	".\colledoc.h"\
	".\collevw.h"\
	

"$(INTDIR)\collevw.obj" : $(SOURCE) $(DEP_CPP_COLLEV) "$(INTDIR)"\
 "$(INTDIR)\collect.pch"


!ELSEIF  "$(CFG)" == "Collect - Win32 Debug"

DEP_CPP_COLLEV=\
	".\stdafx.h"\
	".\collect.h"\
	".\colledoc.h"\
	".\collevw.h"\
	

"$(INTDIR)\collevw.obj" : $(SOURCE) $(DEP_CPP_COLLEV) "$(INTDIR)"\
 "$(INTDIR)\collect.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_COLLEV) "$(INTDIR)"\
 "$(INTDIR)\collect.pch"


!ELSEIF  "$(CFG)" == "Collect - Macintosh Release"

DEP_CPP_COLLEV=\
	".\collect.h"\
	".\colledoc.h"\
	".\collevw.h"\
	".\stdafx.h"\
	

"$(INTDIR)\collevw.obj" : $(SOURCE) $(DEP_CPP_COLLEV) "$(INTDIR)"\
 "$(INTDIR)\collect.pch"


!ELSEIF  "$(CFG)" == "Collect - Macintosh Debug"

DEP_CPP_COLLEV=\
	".\stdafx.h"\
	".\collect.h"\
	".\colledoc.h"\
	".\collevw.h"\
	

"$(INTDIR)\collevw.obj" : $(SOURCE) $(DEP_CPP_COLLEV) "$(INTDIR)"\
 "$(INTDIR)\collect.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_COLLEV) "$(INTDIR)"\
 "$(INTDIR)\collect.pch"


!ELSEIF  "$(CFG)" == "Collect - Power Macintosh Release"

DEP_CPP_COLLEV=\
	".\stdafx.h"\
	".\collect.h"\
	".\colledoc.h"\
	".\collevw.h"\
	

"$(INTDIR)\collevw.obj" : $(SOURCE) $(DEP_CPP_COLLEV) "$(INTDIR)"\
 "$(INTDIR)\collect.pch"


!ELSEIF  "$(CFG)" == "Collect - Power Macintosh Debug"

DEP_CPP_COLLEV=\
	".\collect.h"\
	".\colledoc.h"\
	".\collevw.h"\
	".\stdafx.h"\
	

"$(INTDIR)\collevw.obj" : $(SOURCE) $(DEP_CPP_COLLEV) "$(INTDIR)"\
 "$(INTDIR)\collect.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_COLLEV) "$(INTDIR)"\
 "$(INTDIR)\collect.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\dwarryvw.cpp

!IF  "$(CFG)" == "Collect - Win32 Release"

DEP_CPP_DWARR=\
	".\stdafx.h"\
	".\collect.h"\
	".\colledoc.h"\
	".\dwarryvw.h"\
	

"$(INTDIR)\dwarryvw.obj" : $(SOURCE) $(DEP_CPP_DWARR) "$(INTDIR)"\
 "$(INTDIR)\collect.pch"


!ELSEIF  "$(CFG)" == "Collect - Win32 Debug"

DEP_CPP_DWARR=\
	".\stdafx.h"\
	".\collect.h"\
	".\colledoc.h"\
	".\dwarryvw.h"\
	

"$(INTDIR)\dwarryvw.obj" : $(SOURCE) $(DEP_CPP_DWARR) "$(INTDIR)"\
 "$(INTDIR)\collect.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_DWARR) "$(INTDIR)"\
 "$(INTDIR)\collect.pch"


!ELSEIF  "$(CFG)" == "Collect - Macintosh Release"

DEP_CPP_DWARR=\
	".\collect.h"\
	".\colledoc.h"\
	".\dwarryvw.h"\
	".\stdafx.h"\
	

"$(INTDIR)\dwarryvw.obj" : $(SOURCE) $(DEP_CPP_DWARR) "$(INTDIR)"\
 "$(INTDIR)\collect.pch"


!ELSEIF  "$(CFG)" == "Collect - Macintosh Debug"

DEP_CPP_DWARR=\
	".\stdafx.h"\
	".\collect.h"\
	".\colledoc.h"\
	".\dwarryvw.h"\
	

"$(INTDIR)\dwarryvw.obj" : $(SOURCE) $(DEP_CPP_DWARR) "$(INTDIR)"\
 "$(INTDIR)\collect.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_DWARR) "$(INTDIR)"\
 "$(INTDIR)\collect.pch"


!ELSEIF  "$(CFG)" == "Collect - Power Macintosh Release"

DEP_CPP_DWARR=\
	".\stdafx.h"\
	".\collect.h"\
	".\colledoc.h"\
	".\dwarryvw.h"\
	

"$(INTDIR)\dwarryvw.obj" : $(SOURCE) $(DEP_CPP_DWARR) "$(INTDIR)"\
 "$(INTDIR)\collect.pch"


!ELSEIF  "$(CFG)" == "Collect - Power Macintosh Debug"

DEP_CPP_DWARR=\
	".\collect.h"\
	".\colledoc.h"\
	".\dwarryvw.h"\
	".\stdafx.h"\
	

"$(INTDIR)\dwarryvw.obj" : $(SOURCE) $(DEP_CPP_DWARR) "$(INTDIR)"\
 "$(INTDIR)\collect.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_DWARR) "$(INTDIR)"\
 "$(INTDIR)\collect.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\intlstvw.cpp

!IF  "$(CFG)" == "Collect - Win32 Release"

DEP_CPP_INTLS=\
	".\stdafx.h"\
	".\collect.h"\
	".\colledoc.h"\
	".\intlstvw.h"\
	

"$(INTDIR)\intlstvw.obj" : $(SOURCE) $(DEP_CPP_INTLS) "$(INTDIR)"\
 "$(INTDIR)\collect.pch"


!ELSEIF  "$(CFG)" == "Collect - Win32 Debug"

DEP_CPP_INTLS=\
	".\stdafx.h"\
	".\collect.h"\
	".\colledoc.h"\
	".\intlstvw.h"\
	

"$(INTDIR)\intlstvw.obj" : $(SOURCE) $(DEP_CPP_INTLS) "$(INTDIR)"\
 "$(INTDIR)\collect.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_INTLS) "$(INTDIR)"\
 "$(INTDIR)\collect.pch"


!ELSEIF  "$(CFG)" == "Collect - Macintosh Release"

DEP_CPP_INTLS=\
	".\collect.h"\
	".\colledoc.h"\
	".\intlstvw.h"\
	".\stdafx.h"\
	

"$(INTDIR)\intlstvw.obj" : $(SOURCE) $(DEP_CPP_INTLS) "$(INTDIR)"\
 "$(INTDIR)\collect.pch"


!ELSEIF  "$(CFG)" == "Collect - Macintosh Debug"

DEP_CPP_INTLS=\
	".\stdafx.h"\
	".\collect.h"\
	".\colledoc.h"\
	".\intlstvw.h"\
	

"$(INTDIR)\intlstvw.obj" : $(SOURCE) $(DEP_CPP_INTLS) "$(INTDIR)"\
 "$(INTDIR)\collect.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_INTLS) "$(INTDIR)"\
 "$(INTDIR)\collect.pch"


!ELSEIF  "$(CFG)" == "Collect - Power Macintosh Release"

DEP_CPP_INTLS=\
	".\stdafx.h"\
	".\collect.h"\
	".\colledoc.h"\
	".\intlstvw.h"\
	

"$(INTDIR)\intlstvw.obj" : $(SOURCE) $(DEP_CPP_INTLS) "$(INTDIR)"\
 "$(INTDIR)\collect.pch"


!ELSEIF  "$(CFG)" == "Collect - Power Macintosh Debug"

DEP_CPP_INTLS=\
	".\collect.h"\
	".\colledoc.h"\
	".\intlstvw.h"\
	".\stdafx.h"\
	

"$(INTDIR)\intlstvw.obj" : $(SOURCE) $(DEP_CPP_INTLS) "$(INTDIR)"\
 "$(INTDIR)\collect.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_INTLS) "$(INTDIR)"\
 "$(INTDIR)\collect.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\mainfrm.cpp

!IF  "$(CFG)" == "Collect - Win32 Release"

DEP_CPP_MAINF=\
	".\stdafx.h"\
	".\collect.h"\
	".\mainfrm.h"\
	".\colledoc.h"\
	".\strlstvw.h"\
	".\typlstvw.h"\
	".\intlstvw.h"\
	".\dwarryvw.h"\
	".\typaryvw.h"\
	".\ptarryvw.h"\
	".\mapssvw.h"\
	".\typtrmap.h"\
	".\mapdwvw.h"\
	

"$(INTDIR)\mainfrm.obj" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\collect.pch"


!ELSEIF  "$(CFG)" == "Collect - Win32 Debug"

DEP_CPP_MAINF=\
	".\stdafx.h"\
	".\collect.h"\
	".\mainfrm.h"\
	".\colledoc.h"\
	".\strlstvw.h"\
	".\typlstvw.h"\
	".\intlstvw.h"\
	".\dwarryvw.h"\
	".\typaryvw.h"\
	".\ptarryvw.h"\
	".\mapssvw.h"\
	".\typtrmap.h"\
	".\mapdwvw.h"\
	

"$(INTDIR)\mainfrm.obj" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\collect.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\collect.pch"


!ELSEIF  "$(CFG)" == "Collect - Macintosh Release"

DEP_CPP_MAINF=\
	".\collect.h"\
	".\colledoc.h"\
	".\dwarryvw.h"\
	".\intlstvw.h"\
	".\mainfrm.h"\
	".\mapdwvw.h"\
	".\mapssvw.h"\
	".\ptarryvw.h"\
	".\stdafx.h"\
	".\strlstvw.h"\
	".\typaryvw.h"\
	".\typlstvw.h"\
	".\typtrmap.h"\
	

"$(INTDIR)\mainfrm.obj" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\collect.pch"


!ELSEIF  "$(CFG)" == "Collect - Macintosh Debug"

DEP_CPP_MAINF=\
	".\stdafx.h"\
	".\collect.h"\
	".\mainfrm.h"\
	".\colledoc.h"\
	".\strlstvw.h"\
	".\typlstvw.h"\
	".\intlstvw.h"\
	".\dwarryvw.h"\
	".\typaryvw.h"\
	".\ptarryvw.h"\
	".\mapssvw.h"\
	".\typtrmap.h"\
	".\mapdwvw.h"\
	

"$(INTDIR)\mainfrm.obj" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\collect.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\collect.pch"


!ELSEIF  "$(CFG)" == "Collect - Power Macintosh Release"

DEP_CPP_MAINF=\
	".\stdafx.h"\
	".\collect.h"\
	".\mainfrm.h"\
	".\colledoc.h"\
	".\strlstvw.h"\
	".\typlstvw.h"\
	".\intlstvw.h"\
	".\dwarryvw.h"\
	".\typaryvw.h"\
	".\ptarryvw.h"\
	".\mapssvw.h"\
	".\typtrmap.h"\
	".\mapdwvw.h"\
	

"$(INTDIR)\mainfrm.obj" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\collect.pch"


!ELSEIF  "$(CFG)" == "Collect - Power Macintosh Debug"

DEP_CPP_MAINF=\
	".\collect.h"\
	".\colledoc.h"\
	".\dwarryvw.h"\
	".\intlstvw.h"\
	".\mainfrm.h"\
	".\mapdwvw.h"\
	".\mapssvw.h"\
	".\ptarryvw.h"\
	".\stdafx.h"\
	".\strlstvw.h"\
	".\typaryvw.h"\
	".\typlstvw.h"\
	".\typtrmap.h"\
	

"$(INTDIR)\mainfrm.obj" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\collect.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\collect.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\mapdwvw.cpp

!IF  "$(CFG)" == "Collect - Win32 Release"

DEP_CPP_MAPDW=\
	".\stdafx.h"\
	".\collect.h"\
	".\colledoc.h"\
	".\mapdwvw.h"\
	

"$(INTDIR)\mapdwvw.obj" : $(SOURCE) $(DEP_CPP_MAPDW) "$(INTDIR)"\
 "$(INTDIR)\collect.pch"


!ELSEIF  "$(CFG)" == "Collect - Win32 Debug"

DEP_CPP_MAPDW=\
	".\stdafx.h"\
	".\collect.h"\
	".\colledoc.h"\
	".\mapdwvw.h"\
	

"$(INTDIR)\mapdwvw.obj" : $(SOURCE) $(DEP_CPP_MAPDW) "$(INTDIR)"\
 "$(INTDIR)\collect.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_MAPDW) "$(INTDIR)"\
 "$(INTDIR)\collect.pch"


!ELSEIF  "$(CFG)" == "Collect - Macintosh Release"

DEP_CPP_MAPDW=\
	".\collect.h"\
	".\colledoc.h"\
	".\mapdwvw.h"\
	".\stdafx.h"\
	

"$(INTDIR)\mapdwvw.obj" : $(SOURCE) $(DEP_CPP_MAPDW) "$(INTDIR)"\
 "$(INTDIR)\collect.pch"


!ELSEIF  "$(CFG)" == "Collect - Macintosh Debug"

DEP_CPP_MAPDW=\
	".\stdafx.h"\
	".\collect.h"\
	".\colledoc.h"\
	".\mapdwvw.h"\
	

"$(INTDIR)\mapdwvw.obj" : $(SOURCE) $(DEP_CPP_MAPDW) "$(INTDIR)"\
 "$(INTDIR)\collect.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_MAPDW) "$(INTDIR)"\
 "$(INTDIR)\collect.pch"


!ELSEIF  "$(CFG)" == "Collect - Power Macintosh Release"

DEP_CPP_MAPDW=\
	".\stdafx.h"\
	".\collect.h"\
	".\colledoc.h"\
	".\mapdwvw.h"\
	

"$(INTDIR)\mapdwvw.obj" : $(SOURCE) $(DEP_CPP_MAPDW) "$(INTDIR)"\
 "$(INTDIR)\collect.pch"


!ELSEIF  "$(CFG)" == "Collect - Power Macintosh Debug"

DEP_CPP_MAPDW=\
	".\collect.h"\
	".\colledoc.h"\
	".\mapdwvw.h"\
	".\stdafx.h"\
	

"$(INTDIR)\mapdwvw.obj" : $(SOURCE) $(DEP_CPP_MAPDW) "$(INTDIR)"\
 "$(INTDIR)\collect.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_MAPDW) "$(INTDIR)"\
 "$(INTDIR)\collect.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\mapssvw.cpp

!IF  "$(CFG)" == "Collect - Win32 Release"

DEP_CPP_MAPSS=\
	".\stdafx.h"\
	".\collect.h"\
	".\colledoc.h"\
	".\mapssvw.h"\
	

"$(INTDIR)\mapssvw.obj" : $(SOURCE) $(DEP_CPP_MAPSS) "$(INTDIR)"\
 "$(INTDIR)\collect.pch"


!ELSEIF  "$(CFG)" == "Collect - Win32 Debug"

DEP_CPP_MAPSS=\
	".\stdafx.h"\
	".\collect.h"\
	".\colledoc.h"\
	".\mapssvw.h"\
	

"$(INTDIR)\mapssvw.obj" : $(SOURCE) $(DEP_CPP_MAPSS) "$(INTDIR)"\
 "$(INTDIR)\collect.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_MAPSS) "$(INTDIR)"\
 "$(INTDIR)\collect.pch"


!ELSEIF  "$(CFG)" == "Collect - Macintosh Release"

DEP_CPP_MAPSS=\
	".\collect.h"\
	".\colledoc.h"\
	".\mapssvw.h"\
	".\stdafx.h"\
	

"$(INTDIR)\mapssvw.obj" : $(SOURCE) $(DEP_CPP_MAPSS) "$(INTDIR)"\
 "$(INTDIR)\collect.pch"


!ELSEIF  "$(CFG)" == "Collect - Macintosh Debug"

DEP_CPP_MAPSS=\
	".\stdafx.h"\
	".\collect.h"\
	".\colledoc.h"\
	".\mapssvw.h"\
	

"$(INTDIR)\mapssvw.obj" : $(SOURCE) $(DEP_CPP_MAPSS) "$(INTDIR)"\
 "$(INTDIR)\collect.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_MAPSS) "$(INTDIR)"\
 "$(INTDIR)\collect.pch"


!ELSEIF  "$(CFG)" == "Collect - Power Macintosh Release"

DEP_CPP_MAPSS=\
	".\stdafx.h"\
	".\collect.h"\
	".\colledoc.h"\
	".\mapssvw.h"\
	

"$(INTDIR)\mapssvw.obj" : $(SOURCE) $(DEP_CPP_MAPSS) "$(INTDIR)"\
 "$(INTDIR)\collect.pch"


!ELSEIF  "$(CFG)" == "Collect - Power Macintosh Debug"

DEP_CPP_MAPSS=\
	".\collect.h"\
	".\colledoc.h"\
	".\mapssvw.h"\
	".\stdafx.h"\
	

"$(INTDIR)\mapssvw.obj" : $(SOURCE) $(DEP_CPP_MAPSS) "$(INTDIR)"\
 "$(INTDIR)\collect.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_MAPSS) "$(INTDIR)"\
 "$(INTDIR)\collect.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\ptarryvw.cpp

!IF  "$(CFG)" == "Collect - Win32 Release"

DEP_CPP_PTARR=\
	".\stdafx.h"\
	".\collect.h"\
	".\colledoc.h"\
	".\ptarryvw.h"\
	

"$(INTDIR)\ptarryvw.obj" : $(SOURCE) $(DEP_CPP_PTARR) "$(INTDIR)"\
 "$(INTDIR)\collect.pch"


!ELSEIF  "$(CFG)" == "Collect - Win32 Debug"

DEP_CPP_PTARR=\
	".\stdafx.h"\
	".\collect.h"\
	".\colledoc.h"\
	".\ptarryvw.h"\
	

"$(INTDIR)\ptarryvw.obj" : $(SOURCE) $(DEP_CPP_PTARR) "$(INTDIR)"\
 "$(INTDIR)\collect.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_PTARR) "$(INTDIR)"\
 "$(INTDIR)\collect.pch"


!ELSEIF  "$(CFG)" == "Collect - Macintosh Release"

DEP_CPP_PTARR=\
	".\collect.h"\
	".\colledoc.h"\
	".\ptarryvw.h"\
	".\stdafx.h"\
	

"$(INTDIR)\ptarryvw.obj" : $(SOURCE) $(DEP_CPP_PTARR) "$(INTDIR)"\
 "$(INTDIR)\collect.pch"


!ELSEIF  "$(CFG)" == "Collect - Macintosh Debug"

DEP_CPP_PTARR=\
	".\stdafx.h"\
	".\collect.h"\
	".\colledoc.h"\
	".\ptarryvw.h"\
	

"$(INTDIR)\ptarryvw.obj" : $(SOURCE) $(DEP_CPP_PTARR) "$(INTDIR)"\
 "$(INTDIR)\collect.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_PTARR) "$(INTDIR)"\
 "$(INTDIR)\collect.pch"


!ELSEIF  "$(CFG)" == "Collect - Power Macintosh Release"

DEP_CPP_PTARR=\
	".\stdafx.h"\
	".\collect.h"\
	".\colledoc.h"\
	".\ptarryvw.h"\
	

"$(INTDIR)\ptarryvw.obj" : $(SOURCE) $(DEP_CPP_PTARR) "$(INTDIR)"\
 "$(INTDIR)\collect.pch"


!ELSEIF  "$(CFG)" == "Collect - Power Macintosh Debug"

DEP_CPP_PTARR=\
	".\collect.h"\
	".\colledoc.h"\
	".\ptarryvw.h"\
	".\stdafx.h"\
	

"$(INTDIR)\ptarryvw.obj" : $(SOURCE) $(DEP_CPP_PTARR) "$(INTDIR)"\
 "$(INTDIR)\collect.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_PTARR) "$(INTDIR)"\
 "$(INTDIR)\collect.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\stdafx.cpp
DEP_CPP_STDAF=\
	".\stdafx.h"\
	

!IF  "$(CFG)" == "Collect - Win32 Release"

# ADD CPP /Yc"Stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/collect.pch" /Yc"Stdafx.h" /Fo"$(INTDIR)/"\
 /c $(SOURCE) \
	

"$(INTDIR)\stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\collect.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "Collect - Win32 Debug"

# ADD CPP /Yc"Stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/collect.pch" /Yc"Stdafx.h"\
 /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\collect.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "Collect - Macintosh Release"

# ADD CPP /Yc"Stdafx.h"

BuildCmds= \
	$(CPP) /nologo /AL /Q68s /W3 /GX /O2 /D "_WINDOWS" /D "_MAC" /D "_68K_" /D\
 "WIN32" /D "NDEBUG" /D "_MBCS" /Fp"$(INTDIR)/collect.pch" /Yc"Stdafx.h"\
 /Fo"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\collect.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "Collect - Macintosh Debug"

# ADD CPP /Yc"Stdafx.h"

BuildCmds= \
	$(CPP) /nologo /AL /Q68s /Q68m /W3 /GX /Zi /Od /D "_WINDOWS" /D "_MAC" /D\
 "_68K_" /D "WIN32" /D "_DEBUG" /D "_MBCS" /Fp"$(INTDIR)/collect.pch"\
 /Yc"Stdafx.h" /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\collect.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "Collect - Power Macintosh Release"

# ADD CPP /Yc"Stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "_MAC" /D "_MPPC_" /D "_WINDOWS"\
 /D "WIN32" /D "_MBCS" /D "_AFXDLL" /Fp"$(INTDIR)/collect.pch" /Yc"Stdafx.h"\
 /Fo"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\collect.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "Collect - Power Macintosh Debug"

# ADD CPP /Yc"Stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MDd /W3 /GX /Zi /Od /D "_DEBUG" /D "_MAC" /D "_MPPC_" /D\
 "_WINDOWS" /D "WIN32" /D "_MBCS" /D "_AFXDLL" /Fp"$(INTDIR)/collect.pch"\
 /Yc"Stdafx.h" /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\collect.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\strlstvw.cpp

!IF  "$(CFG)" == "Collect - Win32 Release"

DEP_CPP_STRLS=\
	".\stdafx.h"\
	".\collect.h"\
	".\colledoc.h"\
	".\strlstvw.h"\
	

"$(INTDIR)\strlstvw.obj" : $(SOURCE) $(DEP_CPP_STRLS) "$(INTDIR)"\
 "$(INTDIR)\collect.pch"


!ELSEIF  "$(CFG)" == "Collect - Win32 Debug"

DEP_CPP_STRLS=\
	".\stdafx.h"\
	".\collect.h"\
	".\colledoc.h"\
	".\strlstvw.h"\
	

"$(INTDIR)\strlstvw.obj" : $(SOURCE) $(DEP_CPP_STRLS) "$(INTDIR)"\
 "$(INTDIR)\collect.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_STRLS) "$(INTDIR)"\
 "$(INTDIR)\collect.pch"


!ELSEIF  "$(CFG)" == "Collect - Macintosh Release"

DEP_CPP_STRLS=\
	".\collect.h"\
	".\colledoc.h"\
	".\stdafx.h"\
	".\strlstvw.h"\
	

"$(INTDIR)\strlstvw.obj" : $(SOURCE) $(DEP_CPP_STRLS) "$(INTDIR)"\
 "$(INTDIR)\collect.pch"


!ELSEIF  "$(CFG)" == "Collect - Macintosh Debug"

DEP_CPP_STRLS=\
	".\stdafx.h"\
	".\collect.h"\
	".\colledoc.h"\
	".\strlstvw.h"\
	

"$(INTDIR)\strlstvw.obj" : $(SOURCE) $(DEP_CPP_STRLS) "$(INTDIR)"\
 "$(INTDIR)\collect.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_STRLS) "$(INTDIR)"\
 "$(INTDIR)\collect.pch"


!ELSEIF  "$(CFG)" == "Collect - Power Macintosh Release"

DEP_CPP_STRLS=\
	".\stdafx.h"\
	".\collect.h"\
	".\colledoc.h"\
	".\strlstvw.h"\
	

"$(INTDIR)\strlstvw.obj" : $(SOURCE) $(DEP_CPP_STRLS) "$(INTDIR)"\
 "$(INTDIR)\collect.pch"


!ELSEIF  "$(CFG)" == "Collect - Power Macintosh Debug"

DEP_CPP_STRLS=\
	".\stdafx.h"\
	".\collect.h"\
	".\colledoc.h"\
	".\strlstvw.h"\
	

"$(INTDIR)\strlstvw.obj" : $(SOURCE) $(DEP_CPP_STRLS) "$(INTDIR)"\
 "$(INTDIR)\collect.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_STRLS) "$(INTDIR)"\
 "$(INTDIR)\collect.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\typaryvw.cpp

!IF  "$(CFG)" == "Collect - Win32 Release"

DEP_CPP_TYPAR=\
	".\stdafx.h"\
	".\collect.h"\
	".\colledoc.h"\
	".\typaryvw.h"\
	

"$(INTDIR)\typaryvw.obj" : $(SOURCE) $(DEP_CPP_TYPAR) "$(INTDIR)"\
 "$(INTDIR)\collect.pch"


!ELSEIF  "$(CFG)" == "Collect - Win32 Debug"

DEP_CPP_TYPAR=\
	".\stdafx.h"\
	".\collect.h"\
	".\colledoc.h"\
	".\typaryvw.h"\
	

"$(INTDIR)\typaryvw.obj" : $(SOURCE) $(DEP_CPP_TYPAR) "$(INTDIR)"\
 "$(INTDIR)\collect.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_TYPAR) "$(INTDIR)"\
 "$(INTDIR)\collect.pch"


!ELSEIF  "$(CFG)" == "Collect - Macintosh Release"

DEP_CPP_TYPAR=\
	".\collect.h"\
	".\colledoc.h"\
	".\stdafx.h"\
	".\typaryvw.h"\
	

"$(INTDIR)\typaryvw.obj" : $(SOURCE) $(DEP_CPP_TYPAR) "$(INTDIR)"\
 "$(INTDIR)\collect.pch"


!ELSEIF  "$(CFG)" == "Collect - Macintosh Debug"

DEP_CPP_TYPAR=\
	".\stdafx.h"\
	".\collect.h"\
	".\colledoc.h"\
	".\typaryvw.h"\
	

"$(INTDIR)\typaryvw.obj" : $(SOURCE) $(DEP_CPP_TYPAR) "$(INTDIR)"\
 "$(INTDIR)\collect.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_TYPAR) "$(INTDIR)"\
 "$(INTDIR)\collect.pch"


!ELSEIF  "$(CFG)" == "Collect - Power Macintosh Release"

DEP_CPP_TYPAR=\
	".\stdafx.h"\
	".\collect.h"\
	".\colledoc.h"\
	".\typaryvw.h"\
	

"$(INTDIR)\typaryvw.obj" : $(SOURCE) $(DEP_CPP_TYPAR) "$(INTDIR)"\
 "$(INTDIR)\collect.pch"


!ELSEIF  "$(CFG)" == "Collect - Power Macintosh Debug"

DEP_CPP_TYPAR=\
	".\stdafx.h"\
	".\collect.h"\
	".\colledoc.h"\
	".\typaryvw.h"\
	

"$(INTDIR)\typaryvw.obj" : $(SOURCE) $(DEP_CPP_TYPAR) "$(INTDIR)"\
 "$(INTDIR)\collect.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_TYPAR) "$(INTDIR)"\
 "$(INTDIR)\collect.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\typlstvw.cpp

!IF  "$(CFG)" == "Collect - Win32 Release"

DEP_CPP_TYPLS=\
	".\stdafx.h"\
	".\collect.h"\
	".\colledoc.h"\
	".\typlstvw.h"\
	

"$(INTDIR)\typlstvw.obj" : $(SOURCE) $(DEP_CPP_TYPLS) "$(INTDIR)"\
 "$(INTDIR)\collect.pch"


!ELSEIF  "$(CFG)" == "Collect - Win32 Debug"

DEP_CPP_TYPLS=\
	".\stdafx.h"\
	".\collect.h"\
	".\colledoc.h"\
	".\typlstvw.h"\
	

"$(INTDIR)\typlstvw.obj" : $(SOURCE) $(DEP_CPP_TYPLS) "$(INTDIR)"\
 "$(INTDIR)\collect.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_TYPLS) "$(INTDIR)"\
 "$(INTDIR)\collect.pch"


!ELSEIF  "$(CFG)" == "Collect - Macintosh Release"

DEP_CPP_TYPLS=\
	".\collect.h"\
	".\colledoc.h"\
	".\stdafx.h"\
	".\typlstvw.h"\
	

"$(INTDIR)\typlstvw.obj" : $(SOURCE) $(DEP_CPP_TYPLS) "$(INTDIR)"\
 "$(INTDIR)\collect.pch"


!ELSEIF  "$(CFG)" == "Collect - Macintosh Debug"

DEP_CPP_TYPLS=\
	".\stdafx.h"\
	".\collect.h"\
	".\colledoc.h"\
	".\typlstvw.h"\
	

"$(INTDIR)\typlstvw.obj" : $(SOURCE) $(DEP_CPP_TYPLS) "$(INTDIR)"\
 "$(INTDIR)\collect.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_TYPLS) "$(INTDIR)"\
 "$(INTDIR)\collect.pch"


!ELSEIF  "$(CFG)" == "Collect - Power Macintosh Release"

DEP_CPP_TYPLS=\
	".\stdafx.h"\
	".\collect.h"\
	".\colledoc.h"\
	".\typlstvw.h"\
	

"$(INTDIR)\typlstvw.obj" : $(SOURCE) $(DEP_CPP_TYPLS) "$(INTDIR)"\
 "$(INTDIR)\collect.pch"


!ELSEIF  "$(CFG)" == "Collect - Power Macintosh Debug"

DEP_CPP_TYPLS=\
	".\stdafx.h"\
	".\collect.h"\
	".\colledoc.h"\
	".\typlstvw.h"\
	

"$(INTDIR)\typlstvw.obj" : $(SOURCE) $(DEP_CPP_TYPLS) "$(INTDIR)"\
 "$(INTDIR)\collect.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_TYPLS) "$(INTDIR)"\
 "$(INTDIR)\collect.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\collect.cpp

!IF  "$(CFG)" == "Collect - Win32 Release"

DEP_CPP_COLLEC=\
	".\stdafx.h"\
	".\collect.h"\
	".\mainfrm.h"\
	".\colledoc.h"\
	".\strlstvw.h"\
	

"$(INTDIR)\collect.obj" : $(SOURCE) $(DEP_CPP_COLLEC) "$(INTDIR)"\
 "$(INTDIR)\collect.pch"


!ELSEIF  "$(CFG)" == "Collect - Win32 Debug"

DEP_CPP_COLLEC=\
	".\stdafx.h"\
	".\collect.h"\
	".\mainfrm.h"\
	".\colledoc.h"\
	".\strlstvw.h"\
	

"$(INTDIR)\collect.obj" : $(SOURCE) $(DEP_CPP_COLLEC) "$(INTDIR)"\
 "$(INTDIR)\collect.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_COLLEC) "$(INTDIR)"\
 "$(INTDIR)\collect.pch"


!ELSEIF  "$(CFG)" == "Collect - Macintosh Release"

DEP_CPP_COLLEC=\
	".\collect.h"\
	".\colledoc.h"\
	".\mainfrm.h"\
	".\stdafx.h"\
	".\strlstvw.h"\
	

"$(INTDIR)\collect.obj" : $(SOURCE) $(DEP_CPP_COLLEC) "$(INTDIR)"\
 "$(INTDIR)\collect.pch"


!ELSEIF  "$(CFG)" == "Collect - Macintosh Debug"

DEP_CPP_COLLEC=\
	".\stdafx.h"\
	".\collect.h"\
	".\mainfrm.h"\
	".\colledoc.h"\
	".\strlstvw.h"\
	

"$(INTDIR)\collect.obj" : $(SOURCE) $(DEP_CPP_COLLEC) "$(INTDIR)"\
 "$(INTDIR)\collect.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_COLLEC) "$(INTDIR)"\
 "$(INTDIR)\collect.pch"


!ELSEIF  "$(CFG)" == "Collect - Power Macintosh Release"

DEP_CPP_COLLEC=\
	".\stdafx.h"\
	".\collect.h"\
	".\mainfrm.h"\
	".\colledoc.h"\
	".\strlstvw.h"\
	

"$(INTDIR)\collect.obj" : $(SOURCE) $(DEP_CPP_COLLEC) "$(INTDIR)"\
 "$(INTDIR)\collect.pch"


!ELSEIF  "$(CFG)" == "Collect - Power Macintosh Debug"

DEP_CPP_COLLEC=\
	".\stdafx.h"\
	".\collect.h"\
	".\mainfrm.h"\
	".\colledoc.h"\
	".\strlstvw.h"\
	

"$(INTDIR)\collect.obj" : $(SOURCE) $(DEP_CPP_COLLEC) "$(INTDIR)"\
 "$(INTDIR)\collect.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_COLLEC) "$(INTDIR)"\
 "$(INTDIR)\collect.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\collmac.r

!IF  "$(CFG)" == "Collect - Win32 Release"

!ELSEIF  "$(CFG)" == "Collect - Win32 Debug"

!ELSEIF  "$(CFG)" == "Collect - Macintosh Release"

DEP_MRC_COLLM=\
	{$(INCLUDE)}"\mrc\balloons.r"\
	

"$(INTDIR)\collmac.rsc" : $(SOURCE) $(DEP_MRC_COLLM) "$(INTDIR)"
   $(MRC) /o"$(INTDIR)/collmac.rsc" /D "_68K_" /D "_MAC" /D "NDEBUG" /l 0x409\
 /NOLOGO $(SOURCE)


!ELSEIF  "$(CFG)" == "Collect - Macintosh Debug"

DEP_MRC_COLLM=\
	{$(INCLUDE)}"\mrc\balloons.r"\
	

"$(INTDIR)\collmac.rsc" : $(SOURCE) $(DEP_MRC_COLLM) "$(INTDIR)"
   $(MRC) /o"$(INTDIR)/collmac.rsc" /D "_68K_" /D "_MAC" /D "_DEBUG" /l 0x409\
 /NOLOGO $(SOURCE)


!ELSEIF  "$(CFG)" == "Collect - Power Macintosh Release"

DEP_MRC_COLLM=\
	{$(INCLUDE)}"\mrc\balloons.r"\
	

"$(INTDIR)\collmac.rsc" : $(SOURCE) $(DEP_MRC_COLLM) "$(INTDIR)"
   $(MRC) /o"$(INTDIR)/collmac.rsc" /D "_MPPC_" /D "_MAC" /D "NDEBUG" /l 0x409\
 /NOLOGO $(SOURCE)


!ELSEIF  "$(CFG)" == "Collect - Power Macintosh Debug"

DEP_MRC_COLLM=\
	{$(INCLUDE)}"\mrc\balloons.r"\
	

"$(INTDIR)\collmac.rsc" : $(SOURCE) $(DEP_MRC_COLLM) "$(INTDIR)"
   $(MRC) /o"$(INTDIR)/collmac.rsc" /D "_MPPC_" /D "_MAC" /D "_DEBUG" /l 0x409\
 /NOLOGO $(SOURCE)


!ENDIF 

# End Source File
# End Target
# End Project
################################################################################
