# Microsoft Visual C++ Generated NMAKE File, Format Version 40000
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101
# TARGTYPE "Power Macintosh Application" 0x0401
# TARGTYPE "Macintosh Application" 0x0301

!IF "$(CFG)" == ""
CFG=OldBars - Win32 Debug
!MESSAGE No configuration specified.  Defaulting to OldBars - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "OldBars - Win32 Release" && "$(CFG)" !=\
 "OldBars - Win32 Debug" && "$(CFG)" != "OldBars - Macintosh Release" &&\
 "$(CFG)" != "OldBars - Macintosh Debug" && "$(CFG)" !=\
 "OldBars - Power Macintosh Release" && "$(CFG)" !=\
 "OldBars - Power Macintosh Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "OldBars.mak" CFG="OldBars - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "OldBars - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "OldBars - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE "OldBars - Macintosh Release" (based on "Macintosh Application")
!MESSAGE "OldBars - Macintosh Debug" (based on "Macintosh Application")
!MESSAGE "OldBars - Power Macintosh Release" (based on\
 "Power Macintosh Application")
!MESSAGE "OldBars - Power Macintosh Debug" (based on\
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
# PROP Target_Last_Scanned "OldBars - Win32 Debug"

!IF  "$(CFG)" == "OldBars - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir OldBars_
# PROP BASE Intermediate_Dir OldBars_
# PROP BASE Target_Dir
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 0
# PROP Output_Dir Release
# PROP Intermediate_Dir Release
# PROP Target_Dir
OUTDIR=.\Release
INTDIR=.\Release

ALL : "$(OUTDIR)\OldBars.exe"

CLEAN : 
	-@erase ".\Release\vc40.idb"
	-@erase ".\Release\OldBars.pch"
	-@erase ".\Release\OldBars.exe"
	-@erase ".\Release\Stdafx.obj"
	-@erase ".\Release\Globals.obj"
	-@erase ".\Release\Oldbavw.obj"
	-@erase ".\Release\Oldbars.obj"
	-@erase ".\Release\Statbar.obj"
	-@erase ".\Release\Oldbadoc.obj"
	-@erase ".\Release\Toolbar.obj"
	-@erase ".\Release\Mainfrm.obj"
	-@erase ".\Release\oldbars.res"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"Stdafx.h" /c
CPP_PROJ=/nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/OldBars.pch" /Yu"Stdafx.h" /Fo"$(INTDIR)/"\
 /Fd"$(INTDIR)/" /c 
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
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/oldbars.res" /d "NDEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/OldBars.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 /nologo /subsystem:windows /machine:I386
LINK32_FLAGS=/nologo /subsystem:windows /incremental:no\
 /pdb:"$(OUTDIR)/OldBars.pdb" /machine:I386 /out:"$(OUTDIR)/OldBars.exe" 
LINK32_OBJS= \
	"$(INTDIR)/Stdafx.obj" \
	"$(INTDIR)/Globals.obj" \
	"$(INTDIR)/Oldbavw.obj" \
	"$(INTDIR)/Oldbars.obj" \
	"$(INTDIR)/Statbar.obj" \
	"$(INTDIR)/Oldbadoc.obj" \
	"$(INTDIR)/Toolbar.obj" \
	"$(INTDIR)/Mainfrm.obj" \
	"$(INTDIR)/oldbars.res"

"$(OUTDIR)\OldBars.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "OldBars - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir OldBars0
# PROP BASE Intermediate_Dir OldBars0
# PROP BASE Target_Dir
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 1
# PROP Output_Dir Debug
# PROP Intermediate_Dir Debug
# PROP Target_Dir
OUTDIR=.\Debug
INTDIR=.\Debug

ALL : "$(OUTDIR)\OldBars.exe"

CLEAN : 
	-@erase ".\Debug\vc40.pdb"
	-@erase ".\Debug\OldBars.pch"
	-@erase ".\Debug\vc40.idb"
	-@erase ".\Debug\OldBars.exe"
	-@erase ".\Debug\Oldbavw.obj"
	-@erase ".\Debug\Oldbars.obj"
	-@erase ".\Debug\Statbar.obj"
	-@erase ".\Debug\Mainfrm.obj"
	-@erase ".\Debug\Toolbar.obj"
	-@erase ".\Debug\Stdafx.obj"
	-@erase ".\Debug\Oldbadoc.obj"
	-@erase ".\Debug\Globals.obj"
	-@erase ".\Debug\oldbars.res"
	-@erase ".\Debug\OldBars.ilk"
	-@erase ".\Debug\OldBars.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"Stdafx.h" /c
CPP_PROJ=/nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/OldBars.pch" /Yu"Stdafx.h"\
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
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/oldbars.res" /d "_DEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/OldBars.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386
# ADD LINK32 /nologo /subsystem:windows /debug /machine:I386
LINK32_FLAGS=/nologo /subsystem:windows /incremental:yes\
 /pdb:"$(OUTDIR)/OldBars.pdb" /debug /machine:I386 /out:"$(OUTDIR)/OldBars.exe" 
LINK32_OBJS= \
	"$(INTDIR)/Oldbavw.obj" \
	"$(INTDIR)/Oldbars.obj" \
	"$(INTDIR)/Statbar.obj" \
	"$(INTDIR)/Mainfrm.obj" \
	"$(INTDIR)/Toolbar.obj" \
	"$(INTDIR)/Stdafx.obj" \
	"$(INTDIR)/Oldbadoc.obj" \
	"$(INTDIR)/Globals.obj" \
	"$(INTDIR)/oldbars.res"

"$(OUTDIR)\OldBars.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "OldBars - Macintosh Release"

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

ALL : "$(OUTDIR)\OldBars.exe" "$(OUTDIR)\OldBars.trg"

CLEAN : 
	-@erase ".\MacRel\OldBars.trg"
	-@erase ".\MacRel\OldBars.exe"
	-@erase ".\MacRel\Toolbar.obj"
	-@erase ".\MacRel\OldBars.pch"
	-@erase ".\MacRel\Mainfrm.obj"
	-@erase ".\MacRel\Globals.obj"
	-@erase ".\MacRel\Oldbadoc.obj"
	-@erase ".\MacRel\Oldbavw.obj"
	-@erase ".\MacRel\Oldbars.obj"
	-@erase ".\MacRel\Stdafx.obj"
	-@erase ".\MacRel\Statbar.obj"
	-@erase ".\MacRel\Oldbmac.rsc"
	-@erase ".\MacRel\oldbars.rsc"

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
 "WIN32" /D "NDEBUG" /D "_MBCS" /Fp"$(INTDIR)/OldBars.pch" /Yu"Stdafx.h"\
 /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
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
RSC_PROJ=/l 0x409 /r /m /fo"$(INTDIR)/oldbars.rsc" /d "_MAC" /d "_68K_" /d\
 "NDEBUG" 
MRC=mrc.exe
# ADD BASE MRC /D "_68K_" /D "_MAC" /D "NDEBUG" /NOLOGO
# ADD MRC /D "_68K_" /D "_MAC" /D "NDEBUG" /NOLOGO
MRC_PROJ=/D "_68K_" /D "_MAC" /D "NDEBUG" /l 0x409 /NOLOGO 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/OldBars.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 wlm.lib /nologo /MAC:bundle /machine:M68K
# ADD LINK32 /nologo /MAC:bundle /MAC:creator="OLDB" /machine:M68K
LINK32_FLAGS=/nologo /MAC:bundle /MAC:type="APPL" /MAC:creator="OLDB"\
 /pdb:"$(OUTDIR)/OldBars.pdb" /machine:M68K /out:"$(OUTDIR)/OldBars.exe" 
LINK32_OBJS= \
	"$(INTDIR)/Toolbar.obj" \
	"$(INTDIR)/Mainfrm.obj" \
	"$(INTDIR)/Globals.obj" \
	"$(INTDIR)/Oldbadoc.obj" \
	"$(INTDIR)/Oldbavw.obj" \
	"$(INTDIR)/Oldbars.obj" \
	"$(INTDIR)/Stdafx.obj" \
	"$(INTDIR)/Statbar.obj" \
	"$(INTDIR)/Oldbmac.rsc" \
	"$(INTDIR)/oldbars.rsc"

"$(OUTDIR)\OldBars.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

MFILE32=mfile.exe
# ADD BASE MFILE32 COPY /NOLOGO
# ADD MFILE32 COPY /NOLOGO
MFILE32_FLAGS=COPY /NOLOGO 
MFILE32_FILES= \
	"$(OUTDIR)/OldBars.exe"

"$(OUTDIR)\OldBars.trg" : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\MacRel\OldBars.exe "$(MFILE32_DEST)"

DOWNLOAD : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\MacRel\OldBars.exe "$(MFILE32_DEST)"

!ELSEIF  "$(CFG)" == "OldBars - Macintosh Debug"

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

ALL : "$(OUTDIR)\OldBars.exe" "$(OUTDIR)\OldBars.trg"

CLEAN : 
	-@erase ".\MacDbg\vc40.pdb"
	-@erase ".\MacDbg\OldBars.pch"
	-@erase ".\MacDbg\OldBars.trg"
	-@erase ".\MacDbg\OldBars.exe"
	-@erase ".\MacDbg\Oldbavw.obj"
	-@erase ".\MacDbg\Oldbars.obj"
	-@erase ".\MacDbg\Statbar.obj"
	-@erase ".\MacDbg\Oldbadoc.obj"
	-@erase ".\MacDbg\Mainfrm.obj"
	-@erase ".\MacDbg\Toolbar.obj"
	-@erase ".\MacDbg\Stdafx.obj"
	-@erase ".\MacDbg\Globals.obj"
	-@erase ".\MacDbg\Oldbmac.rsc"
	-@erase ".\MacDbg\oldbars.rsc"
	-@erase ".\MacDbg\OldBars.pdb"

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
 "_68K_" /D "WIN32" /D "_DEBUG" /D "_MBCS" /Fp"$(INTDIR)/OldBars.pch"\
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
RSC_PROJ=/l 0x409 /r /m /fo"$(INTDIR)/oldbars.rsc" /d "_MAC" /d "_68K_" /d\
 "_DEBUG" 
MRC=mrc.exe
# ADD BASE MRC /D "_68K_" /D "_MAC" /D "_DEBUG" /NOLOGO
# ADD MRC /D "_68K_" /D "_MAC" /D "_DEBUG" /NOLOGO
MRC_PROJ=/D "_68K_" /D "_MAC" /D "_DEBUG" /l 0x409 /NOLOGO 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/OldBars.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 wlm.lib /nologo /MAC:bundle /debug /machine:M68K
# ADD LINK32 /nologo /MAC:bundle /MAC:creator="OLDB" /debug /machine:M68K
LINK32_FLAGS=/nologo /MAC:bundle /MAC:type="APPL" /MAC:creator="OLDB"\
 /pdb:"$(OUTDIR)/OldBars.pdb" /debug /machine:M68K /out:"$(OUTDIR)/OldBars.exe" 
LINK32_OBJS= \
	"$(INTDIR)/Oldbavw.obj" \
	"$(INTDIR)/Oldbars.obj" \
	"$(INTDIR)/Statbar.obj" \
	"$(INTDIR)/Oldbadoc.obj" \
	"$(INTDIR)/Mainfrm.obj" \
	"$(INTDIR)/Toolbar.obj" \
	"$(INTDIR)/Stdafx.obj" \
	"$(INTDIR)/Globals.obj" \
	"$(INTDIR)/Oldbmac.rsc" \
	"$(INTDIR)/oldbars.rsc"

"$(OUTDIR)\OldBars.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

MFILE32=mfile.exe
# ADD BASE MFILE32 COPY /NOLOGO
# ADD MFILE32 COPY /NOLOGO
MFILE32_FLAGS=COPY /NOLOGO 
MFILE32_FILES= \
	"$(OUTDIR)/OldBars.exe"

"$(OUTDIR)\OldBars.trg" : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\MacDbg\OldBars.exe "$(MFILE32_DEST)"

DOWNLOAD : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\MacDbg\OldBars.exe "$(MFILE32_DEST)"

!ELSEIF  "$(CFG)" == "OldBars - Power Macintosh Release"

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

ALL : "$(OUTDIR)\OldBars.exe" "$(OUTDIR)\OldBars.trg"

CLEAN : 
	-@erase ".\PMacRel\OldBars.trg"
	-@erase ".\PMacRel\OldBars.exe"
	-@erase ".\PMacRel\Oldbavw.obj"
	-@erase ".\PMacRel\OldBars.pch"
	-@erase ".\PMacRel\Oldbars.obj"
	-@erase ".\PMacRel\Statbar.obj"
	-@erase ".\PMacRel\Oldbadoc.obj"
	-@erase ".\PMacRel\Stdafx.obj"
	-@erase ".\PMacRel\Mainfrm.obj"
	-@erase ".\PMacRel\Toolbar.obj"
	-@erase ".\PMacRel\Globals.obj"
	-@erase ".\PMacRel\Oldbmac.rsc"
	-@erase ".\PMacRel\oldbars.rsc"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

MTL=mktyplib.exe
# ADD BASE MTL /nologo /D "NDEBUG"
# ADD MTL /nologo /D "NDEBUG"
MTL_PROJ=/nologo /D "NDEBUG" 
CPP=cl.exe
# ADD BASE CPP /nologo /ML /W3 /GX /O2 /D "_MAC" /D "_MPPC_" /D "_WINDOWS" /D "WIN32" /D "NDEBUG" /YX /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "_MAC" /D "_MPPC_" /D "_WINDOWS" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_AFXDLL" /Yu"Stdafx.h" /c
CPP_PROJ=/nologo /MD /W3 /GX /O2 /D "_MAC" /D "_MPPC_" /D "_WINDOWS" /D "WIN32"\
 /D "NDEBUG" /D "_MBCS" /D "_AFXDLL" /Fp"$(INTDIR)/OldBars.pch" /Yu"Stdafx.h"\
 /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
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
RSC_PROJ=/l 0x409 /r /m /fo"$(INTDIR)/oldbars.rsc" /d "_MAC" /d "_MPPC_" /d\
 "NDEBUG" 
MRC=mrc.exe
# ADD BASE MRC /D "_MPPC_" /D "_MAC" /D "NDEBUG" /NOLOGO
# ADD MRC /D "_MPPC_" /D "_MAC" /D "NDEBUG" /NOLOGO
MRC_PROJ=/D "_MPPC_" /D "_MAC" /D "NDEBUG" /l 0x409 /NOLOGO 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/OldBars.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 cfrg.rsc /nologo /mac:bundle /machine:MPPC
# ADD LINK32 /nologo /mac:bundle /mac:creator="OLDB" /machine:MPPC
LINK32_FLAGS=/nologo /mac:bundle /mac:type="APPL" /mac:creator="OLDB"\
 /incremental:no /pdb:"$(OUTDIR)/OldBars.pdb" /machine:MPPC\
 /out:"$(OUTDIR)/OldBars.exe" 
LINK32_OBJS= \
	"$(INTDIR)/Oldbavw.obj" \
	"$(INTDIR)/Oldbars.obj" \
	"$(INTDIR)/Statbar.obj" \
	"$(INTDIR)/Oldbadoc.obj" \
	"$(INTDIR)/Stdafx.obj" \
	"$(INTDIR)/Mainfrm.obj" \
	"$(INTDIR)/Toolbar.obj" \
	"$(INTDIR)/Globals.obj" \
	"$(INTDIR)/Oldbmac.rsc" \
	"$(INTDIR)/oldbars.rsc"

"$(OUTDIR)\OldBars.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

MFILE32=mfile.exe
# ADD BASE MFILE32 COPY /NOLOGO
# ADD MFILE32 COPY /NOLOGO
MFILE32_FLAGS=COPY /NOLOGO 
MFILE32_FILES= \
	"$(OUTDIR)/OldBars.exe"

"$(OUTDIR)\OldBars.trg" : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\PMacRel\OldBars.exe "$(MFILE32_DEST)"

DOWNLOAD : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\PMacRel\OldBars.exe "$(MFILE32_DEST)"

!ELSEIF  "$(CFG)" == "OldBars - Power Macintosh Debug"

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

ALL : "$(OUTDIR)\OldBars.exe" "$(OUTDIR)\OldBars.trg"

CLEAN : 
	-@erase ".\PMacDbg\vc40.pdb"
	-@erase ".\PMacDbg\OldBars.pch"
	-@erase ".\PMacDbg\OldBars.trg"
	-@erase ".\PMacDbg\OldBars.exe"
	-@erase ".\PMacDbg\Toolbar.obj"
	-@erase ".\PMacDbg\Globals.obj"
	-@erase ".\PMacDbg\Oldbadoc.obj"
	-@erase ".\PMacDbg\Oldbavw.obj"
	-@erase ".\PMacDbg\Oldbars.obj"
	-@erase ".\PMacDbg\Stdafx.obj"
	-@erase ".\PMacDbg\Statbar.obj"
	-@erase ".\PMacDbg\Mainfrm.obj"
	-@erase ".\PMacDbg\oldbars.rsc"
	-@erase ".\PMacDbg\Oldbmac.rsc"
	-@erase ".\PMacDbg\OldBars.ilk"
	-@erase ".\PMacDbg\OldBars.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

MTL=mktyplib.exe
# ADD BASE MTL /nologo /D "_DEBUG"
# ADD MTL /nologo /D "_DEBUG"
MTL_PROJ=/nologo /D "_DEBUG" 
CPP=cl.exe
# ADD BASE CPP /nologo /W3 /GX /Zi /Od /D "_MAC" /D "_MPPC_" /D "_WINDOWS" /D "WIN32" /D "_DEBUG" /YX /c
# ADD CPP /nologo /MDd /W3 /GX /Zi /Od /D "_MAC" /D "_MPPC_" /D "_WINDOWS" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_AFXDLL" /Yu"Stdafx.h" /c
CPP_PROJ=/nologo /MDd /W3 /GX /Zi /Od /D "_MAC" /D "_MPPC_" /D "_WINDOWS" /D\
 "WIN32" /D "_DEBUG" /D "_MBCS" /D "_AFXDLL" /Fp"$(INTDIR)/OldBars.pch"\
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
RSC_PROJ=/l 0x409 /r /m /fo"$(INTDIR)/oldbars.rsc" /d "_MAC" /d "_MPPC_" /d\
 "_DEBUG" 
MRC=mrc.exe
# ADD BASE MRC /D "_MPPC_" /D "_MAC" /D "_DEBUG" /NOLOGO
# ADD MRC /D "_MPPC_" /D "_MAC" /D "_DEBUG" /NOLOGO
MRC_PROJ=/D "_MPPC_" /D "_MAC" /D "_DEBUG" /l 0x409 /NOLOGO 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/OldBars.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 cfrg.rsc /nologo /mac:bundle /debug /machine:MPPC
# ADD LINK32 /nologo /mac:bundle /mac:creator="OLDB" /debug /machine:MPPC
LINK32_FLAGS=/nologo /mac:bundle /mac:type="APPL" /mac:creator="OLDB"\
 /incremental:yes /pdb:"$(OUTDIR)/OldBars.pdb" /debug /machine:MPPC\
 /out:"$(OUTDIR)/OldBars.exe" 
LINK32_OBJS= \
	"$(INTDIR)/Toolbar.obj" \
	"$(INTDIR)/Globals.obj" \
	"$(INTDIR)/Oldbadoc.obj" \
	"$(INTDIR)/Oldbavw.obj" \
	"$(INTDIR)/Oldbars.obj" \
	"$(INTDIR)/Stdafx.obj" \
	"$(INTDIR)/Statbar.obj" \
	"$(INTDIR)/Mainfrm.obj" \
	"$(INTDIR)/oldbars.rsc" \
	"$(INTDIR)/Oldbmac.rsc"

"$(OUTDIR)\OldBars.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

MFILE32=mfile.exe
# ADD BASE MFILE32 COPY /NOLOGO
# ADD MFILE32 COPY /NOLOGO
MFILE32_FLAGS=COPY /NOLOGO 
MFILE32_FILES= \
	"$(OUTDIR)/OldBars.exe"

"$(OUTDIR)\OldBars.trg" : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\PMacDbg\OldBars.exe "$(MFILE32_DEST)"

DOWNLOAD : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\PMacDbg\OldBars.exe "$(MFILE32_DEST)"

!ENDIF 

################################################################################
# Begin Target

# Name "OldBars - Win32 Release"
# Name "OldBars - Win32 Debug"
# Name "OldBars - Macintosh Release"
# Name "OldBars - Macintosh Debug"
# Name "OldBars - Power Macintosh Release"
# Name "OldBars - Power Macintosh Debug"

!IF  "$(CFG)" == "OldBars - Win32 Release"

!ELSEIF  "$(CFG)" == "OldBars - Win32 Debug"

!ELSEIF  "$(CFG)" == "OldBars - Macintosh Release"

!ELSEIF  "$(CFG)" == "OldBars - Macintosh Debug"

!ELSEIF  "$(CFG)" == "OldBars - Power Macintosh Release"

!ELSEIF  "$(CFG)" == "OldBars - Power Macintosh Debug"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\oldbars.rc
DEP_RSC_OLDBA=\
	".\Res\Oldbars.ico"\
	".\Res\Oldbadoc.ico"\
	".\Res\Toolbar.bmp"\
	".\Res\Oldbars.rc2"\
	

!IF  "$(CFG)" == "OldBars - Win32 Release"


"$(INTDIR)\oldbars.res" : $(SOURCE) $(DEP_RSC_OLDBA) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "OldBars - Win32 Debug"


"$(INTDIR)\oldbars.res" : $(SOURCE) $(DEP_RSC_OLDBA) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "OldBars - Macintosh Release"


"$(INTDIR)\oldbars.rsc" : $(SOURCE) $(DEP_RSC_OLDBA) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "OldBars - Macintosh Debug"


"$(INTDIR)\oldbars.rsc" : $(SOURCE) $(DEP_RSC_OLDBA) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "OldBars - Power Macintosh Release"


"$(INTDIR)\oldbars.rsc" : $(SOURCE) $(DEP_RSC_OLDBA) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "OldBars - Power Macintosh Debug"


"$(INTDIR)\oldbars.rsc" : $(SOURCE) $(DEP_RSC_OLDBA) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Mainfrm.cpp

!IF  "$(CFG)" == "OldBars - Win32 Release"

DEP_CPP_MAINF=\
	".\Mainfrm.h"\
	".\Oldbars.h"\
	".\Resource.h"\
	".\Toolbar.h"\
	".\Stdafx.h"\
	

"$(INTDIR)\Mainfrm.obj" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\OldBars.pch"

"$(INTDIR)\vc40.idb" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\OldBars.pch"


!ELSEIF  "$(CFG)" == "OldBars - Win32 Debug"

DEP_CPP_MAINF=\
	".\Mainfrm.h"\
	".\Oldbars.h"\
	".\Resource.h"\
	".\Toolbar.h"\
	".\Stdafx.h"\
	

"$(INTDIR)\Mainfrm.obj" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\OldBars.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\OldBars.pch"

"$(INTDIR)\vc40.idb" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\OldBars.pch"


!ELSEIF  "$(CFG)" == "OldBars - Macintosh Release"

DEP_CPP_MAINF=\
	".\Mainfrm.h"\
	".\Oldbars.h"\
	".\Resource.h"\
	".\Statbar.h"\
	".\Toolbar.h"\
	".\Stdafx.h"\
	

"$(INTDIR)\Mainfrm.obj" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\OldBars.pch"


!ELSEIF  "$(CFG)" == "OldBars - Macintosh Debug"

DEP_CPP_MAINF=\
	".\Mainfrm.h"\
	".\Oldbars.h"\
	".\Resource.h"\
	".\Statbar.h"\
	".\Toolbar.h"\
	".\Stdafx.h"\
	

"$(INTDIR)\Mainfrm.obj" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\OldBars.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\OldBars.pch"


!ELSEIF  "$(CFG)" == "OldBars - Power Macintosh Release"

DEP_CPP_MAINF=\
	".\Mainfrm.h"\
	".\Oldbars.h"\
	".\Resource.h"\
	".\Statbar.h"\
	".\Toolbar.h"\
	".\Stdafx.h"\
	

"$(INTDIR)\Mainfrm.obj" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\OldBars.pch"


!ELSEIF  "$(CFG)" == "OldBars - Power Macintosh Debug"

DEP_CPP_MAINF=\
	".\Mainfrm.h"\
	".\Oldbars.h"\
	".\Resource.h"\
	".\Statbar.h"\
	".\Toolbar.h"\
	".\Stdafx.h"\
	

"$(INTDIR)\Mainfrm.obj" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\OldBars.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\OldBars.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Oldbadoc.cpp

!IF  "$(CFG)" == "OldBars - Win32 Release"

DEP_CPP_OLDBAD=\
	".\Oldbadoc.h"\
	".\Oldbars.h"\
	".\Resource.h"\
	".\Toolbar.h"\
	".\Stdafx.h"\
	

"$(INTDIR)\Oldbadoc.obj" : $(SOURCE) $(DEP_CPP_OLDBAD) "$(INTDIR)"\
 "$(INTDIR)\OldBars.pch"

"$(INTDIR)\vc40.idb" : $(SOURCE) $(DEP_CPP_OLDBAD) "$(INTDIR)"\
 "$(INTDIR)\OldBars.pch"


!ELSEIF  "$(CFG)" == "OldBars - Win32 Debug"

DEP_CPP_OLDBAD=\
	".\Oldbadoc.h"\
	".\Oldbars.h"\
	".\Resource.h"\
	".\Toolbar.h"\
	".\Stdafx.h"\
	

"$(INTDIR)\Oldbadoc.obj" : $(SOURCE) $(DEP_CPP_OLDBAD) "$(INTDIR)"\
 "$(INTDIR)\OldBars.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_OLDBAD) "$(INTDIR)"\
 "$(INTDIR)\OldBars.pch"

"$(INTDIR)\vc40.idb" : $(SOURCE) $(DEP_CPP_OLDBAD) "$(INTDIR)"\
 "$(INTDIR)\OldBars.pch"


!ELSEIF  "$(CFG)" == "OldBars - Macintosh Release"

DEP_CPP_OLDBAD=\
	".\Oldbadoc.h"\
	".\Oldbars.h"\
	".\Resource.h"\
	".\Statbar.h"\
	".\Toolbar.h"\
	".\Stdafx.h"\
	

"$(INTDIR)\Oldbadoc.obj" : $(SOURCE) $(DEP_CPP_OLDBAD) "$(INTDIR)"\
 "$(INTDIR)\OldBars.pch"


!ELSEIF  "$(CFG)" == "OldBars - Macintosh Debug"

DEP_CPP_OLDBAD=\
	".\Oldbadoc.h"\
	".\Oldbars.h"\
	".\Resource.h"\
	".\Statbar.h"\
	".\Toolbar.h"\
	".\Stdafx.h"\
	

"$(INTDIR)\Oldbadoc.obj" : $(SOURCE) $(DEP_CPP_OLDBAD) "$(INTDIR)"\
 "$(INTDIR)\OldBars.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_OLDBAD) "$(INTDIR)"\
 "$(INTDIR)\OldBars.pch"


!ELSEIF  "$(CFG)" == "OldBars - Power Macintosh Release"

DEP_CPP_OLDBAD=\
	".\Oldbadoc.h"\
	".\Oldbars.h"\
	".\Resource.h"\
	".\Statbar.h"\
	".\Toolbar.h"\
	".\Stdafx.h"\
	

"$(INTDIR)\Oldbadoc.obj" : $(SOURCE) $(DEP_CPP_OLDBAD) "$(INTDIR)"\
 "$(INTDIR)\OldBars.pch"


!ELSEIF  "$(CFG)" == "OldBars - Power Macintosh Debug"

DEP_CPP_OLDBAD=\
	".\Oldbadoc.h"\
	".\Oldbars.h"\
	".\Resource.h"\
	".\Statbar.h"\
	".\Toolbar.h"\
	".\Stdafx.h"\
	

"$(INTDIR)\Oldbadoc.obj" : $(SOURCE) $(DEP_CPP_OLDBAD) "$(INTDIR)"\
 "$(INTDIR)\OldBars.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_OLDBAD) "$(INTDIR)"\
 "$(INTDIR)\OldBars.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Oldbars.cpp

!IF  "$(CFG)" == "OldBars - Win32 Release"

DEP_CPP_OLDBAR=\
	".\Mainfrm.h"\
	".\Oldbadoc.h"\
	".\Oldbars.h"\
	".\Resource.h"\
	".\Toolbar.h"\
	".\Oldbavw.h"\
	".\Stdafx.h"\
	

"$(INTDIR)\Oldbars.obj" : $(SOURCE) $(DEP_CPP_OLDBAR) "$(INTDIR)"\
 "$(INTDIR)\OldBars.pch"

"$(INTDIR)\vc40.idb" : $(SOURCE) $(DEP_CPP_OLDBAR) "$(INTDIR)"\
 "$(INTDIR)\OldBars.pch"


!ELSEIF  "$(CFG)" == "OldBars - Win32 Debug"

DEP_CPP_OLDBAR=\
	".\Mainfrm.h"\
	".\Oldbadoc.h"\
	".\Oldbars.h"\
	".\Resource.h"\
	".\Toolbar.h"\
	".\Oldbavw.h"\
	".\Stdafx.h"\
	

"$(INTDIR)\Oldbars.obj" : $(SOURCE) $(DEP_CPP_OLDBAR) "$(INTDIR)"\
 "$(INTDIR)\OldBars.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_OLDBAR) "$(INTDIR)"\
 "$(INTDIR)\OldBars.pch"

"$(INTDIR)\vc40.idb" : $(SOURCE) $(DEP_CPP_OLDBAR) "$(INTDIR)"\
 "$(INTDIR)\OldBars.pch"


!ELSEIF  "$(CFG)" == "OldBars - Macintosh Release"

DEP_CPP_OLDBAR=\
	".\Mainfrm.h"\
	".\Oldbadoc.h"\
	".\Oldbars.h"\
	".\Resource.h"\
	".\Statbar.h"\
	".\Toolbar.h"\
	".\Oldbavw.h"\
	".\Stdafx.h"\
	

"$(INTDIR)\Oldbars.obj" : $(SOURCE) $(DEP_CPP_OLDBAR) "$(INTDIR)"\
 "$(INTDIR)\OldBars.pch"


!ELSEIF  "$(CFG)" == "OldBars - Macintosh Debug"

DEP_CPP_OLDBAR=\
	".\Mainfrm.h"\
	".\Oldbadoc.h"\
	".\Oldbars.h"\
	".\Resource.h"\
	".\Statbar.h"\
	".\Toolbar.h"\
	".\Oldbavw.h"\
	".\Stdafx.h"\
	

"$(INTDIR)\Oldbars.obj" : $(SOURCE) $(DEP_CPP_OLDBAR) "$(INTDIR)"\
 "$(INTDIR)\OldBars.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_OLDBAR) "$(INTDIR)"\
 "$(INTDIR)\OldBars.pch"


!ELSEIF  "$(CFG)" == "OldBars - Power Macintosh Release"

DEP_CPP_OLDBAR=\
	".\Mainfrm.h"\
	".\Oldbadoc.h"\
	".\Oldbars.h"\
	".\Resource.h"\
	".\Statbar.h"\
	".\Toolbar.h"\
	".\Oldbavw.h"\
	".\Stdafx.h"\
	

"$(INTDIR)\Oldbars.obj" : $(SOURCE) $(DEP_CPP_OLDBAR) "$(INTDIR)"\
 "$(INTDIR)\OldBars.pch"


!ELSEIF  "$(CFG)" == "OldBars - Power Macintosh Debug"

DEP_CPP_OLDBAR=\
	".\Mainfrm.h"\
	".\Oldbadoc.h"\
	".\Oldbars.h"\
	".\Resource.h"\
	".\Statbar.h"\
	".\Toolbar.h"\
	".\Oldbavw.h"\
	".\Stdafx.h"\
	

"$(INTDIR)\Oldbars.obj" : $(SOURCE) $(DEP_CPP_OLDBAR) "$(INTDIR)"\
 "$(INTDIR)\OldBars.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_OLDBAR) "$(INTDIR)"\
 "$(INTDIR)\OldBars.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Globals.cpp
DEP_CPP_GLOBA=\
	".\Globals.h"\
	".\Stdafx.h"\
	

!IF  "$(CFG)" == "OldBars - Win32 Release"


"$(INTDIR)\Globals.obj" : $(SOURCE) $(DEP_CPP_GLOBA) "$(INTDIR)"\
 "$(INTDIR)\OldBars.pch"

"$(INTDIR)\vc40.idb" : $(SOURCE) $(DEP_CPP_GLOBA) "$(INTDIR)"\
 "$(INTDIR)\OldBars.pch"


!ELSEIF  "$(CFG)" == "OldBars - Win32 Debug"


"$(INTDIR)\Globals.obj" : $(SOURCE) $(DEP_CPP_GLOBA) "$(INTDIR)"\
 "$(INTDIR)\OldBars.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_GLOBA) "$(INTDIR)"\
 "$(INTDIR)\OldBars.pch"

"$(INTDIR)\vc40.idb" : $(SOURCE) $(DEP_CPP_GLOBA) "$(INTDIR)"\
 "$(INTDIR)\OldBars.pch"


!ELSEIF  "$(CFG)" == "OldBars - Macintosh Release"


"$(INTDIR)\Globals.obj" : $(SOURCE) $(DEP_CPP_GLOBA) "$(INTDIR)"\
 "$(INTDIR)\OldBars.pch"


!ELSEIF  "$(CFG)" == "OldBars - Macintosh Debug"


"$(INTDIR)\Globals.obj" : $(SOURCE) $(DEP_CPP_GLOBA) "$(INTDIR)"\
 "$(INTDIR)\OldBars.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_GLOBA) "$(INTDIR)"\
 "$(INTDIR)\OldBars.pch"


!ELSEIF  "$(CFG)" == "OldBars - Power Macintosh Release"


"$(INTDIR)\Globals.obj" : $(SOURCE) $(DEP_CPP_GLOBA) "$(INTDIR)"\
 "$(INTDIR)\OldBars.pch"


!ELSEIF  "$(CFG)" == "OldBars - Power Macintosh Debug"


"$(INTDIR)\Globals.obj" : $(SOURCE) $(DEP_CPP_GLOBA) "$(INTDIR)"\
 "$(INTDIR)\OldBars.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_GLOBA) "$(INTDIR)"\
 "$(INTDIR)\OldBars.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Oldbavw.cpp

!IF  "$(CFG)" == "OldBars - Win32 Release"

DEP_CPP_OLDBAV=\
	".\Oldbadoc.h"\
	".\Oldbars.h"\
	".\Resource.h"\
	".\Toolbar.h"\
	".\Oldbavw.h"\
	".\Stdafx.h"\
	

"$(INTDIR)\Oldbavw.obj" : $(SOURCE) $(DEP_CPP_OLDBAV) "$(INTDIR)"\
 "$(INTDIR)\OldBars.pch"

"$(INTDIR)\vc40.idb" : $(SOURCE) $(DEP_CPP_OLDBAV) "$(INTDIR)"\
 "$(INTDIR)\OldBars.pch"


!ELSEIF  "$(CFG)" == "OldBars - Win32 Debug"

DEP_CPP_OLDBAV=\
	".\Oldbadoc.h"\
	".\Oldbars.h"\
	".\Resource.h"\
	".\Toolbar.h"\
	".\Oldbavw.h"\
	".\Stdafx.h"\
	

"$(INTDIR)\Oldbavw.obj" : $(SOURCE) $(DEP_CPP_OLDBAV) "$(INTDIR)"\
 "$(INTDIR)\OldBars.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_OLDBAV) "$(INTDIR)"\
 "$(INTDIR)\OldBars.pch"

"$(INTDIR)\vc40.idb" : $(SOURCE) $(DEP_CPP_OLDBAV) "$(INTDIR)"\
 "$(INTDIR)\OldBars.pch"


!ELSEIF  "$(CFG)" == "OldBars - Macintosh Release"

DEP_CPP_OLDBAV=\
	".\Oldbadoc.h"\
	".\Oldbars.h"\
	".\Resource.h"\
	".\Statbar.h"\
	".\Toolbar.h"\
	".\Oldbavw.h"\
	".\Stdafx.h"\
	

"$(INTDIR)\Oldbavw.obj" : $(SOURCE) $(DEP_CPP_OLDBAV) "$(INTDIR)"\
 "$(INTDIR)\OldBars.pch"


!ELSEIF  "$(CFG)" == "OldBars - Macintosh Debug"

DEP_CPP_OLDBAV=\
	".\Oldbadoc.h"\
	".\Oldbars.h"\
	".\Resource.h"\
	".\Statbar.h"\
	".\Toolbar.h"\
	".\Oldbavw.h"\
	".\Stdafx.h"\
	

"$(INTDIR)\Oldbavw.obj" : $(SOURCE) $(DEP_CPP_OLDBAV) "$(INTDIR)"\
 "$(INTDIR)\OldBars.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_OLDBAV) "$(INTDIR)"\
 "$(INTDIR)\OldBars.pch"


!ELSEIF  "$(CFG)" == "OldBars - Power Macintosh Release"

DEP_CPP_OLDBAV=\
	".\Oldbadoc.h"\
	".\Oldbars.h"\
	".\Resource.h"\
	".\Statbar.h"\
	".\Toolbar.h"\
	".\Oldbavw.h"\
	".\Stdafx.h"\
	

"$(INTDIR)\Oldbavw.obj" : $(SOURCE) $(DEP_CPP_OLDBAV) "$(INTDIR)"\
 "$(INTDIR)\OldBars.pch"


!ELSEIF  "$(CFG)" == "OldBars - Power Macintosh Debug"

DEP_CPP_OLDBAV=\
	".\Oldbadoc.h"\
	".\Oldbars.h"\
	".\Resource.h"\
	".\Statbar.h"\
	".\Toolbar.h"\
	".\Oldbavw.h"\
	".\Stdafx.h"\
	

"$(INTDIR)\Oldbavw.obj" : $(SOURCE) $(DEP_CPP_OLDBAV) "$(INTDIR)"\
 "$(INTDIR)\OldBars.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_OLDBAV) "$(INTDIR)"\
 "$(INTDIR)\OldBars.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Statbar.cpp

!IF  "$(CFG)" == "OldBars - Win32 Release"

DEP_CPP_STATB=\
	".\Globals.h"\
	".\Stdafx.h"\
	

"$(INTDIR)\Statbar.obj" : $(SOURCE) $(DEP_CPP_STATB) "$(INTDIR)"\
 "$(INTDIR)\OldBars.pch"

"$(INTDIR)\vc40.idb" : $(SOURCE) $(DEP_CPP_STATB) "$(INTDIR)"\
 "$(INTDIR)\OldBars.pch"


!ELSEIF  "$(CFG)" == "OldBars - Win32 Debug"

DEP_CPP_STATB=\
	".\Globals.h"\
	".\Stdafx.h"\
	

"$(INTDIR)\Statbar.obj" : $(SOURCE) $(DEP_CPP_STATB) "$(INTDIR)"\
 "$(INTDIR)\OldBars.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_STATB) "$(INTDIR)"\
 "$(INTDIR)\OldBars.pch"

"$(INTDIR)\vc40.idb" : $(SOURCE) $(DEP_CPP_STATB) "$(INTDIR)"\
 "$(INTDIR)\OldBars.pch"


!ELSEIF  "$(CFG)" == "OldBars - Macintosh Release"

DEP_CPP_STATB=\
	".\Globals.h"\
	".\Statbar.h"\
	".\Stdafx.h"\
	
NODEP_CPP_STATB=\
	".\macos\Windows.h"\
	

"$(INTDIR)\Statbar.obj" : $(SOURCE) $(DEP_CPP_STATB) "$(INTDIR)"\
 "$(INTDIR)\OldBars.pch"


!ELSEIF  "$(CFG)" == "OldBars - Macintosh Debug"

DEP_CPP_STATB=\
	".\Globals.h"\
	".\Statbar.h"\
	".\Stdafx.h"\
	{$(INCLUDE)}"\macos\Windows.h"\
	

"$(INTDIR)\Statbar.obj" : $(SOURCE) $(DEP_CPP_STATB) "$(INTDIR)"\
 "$(INTDIR)\OldBars.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_STATB) "$(INTDIR)"\
 "$(INTDIR)\OldBars.pch"


!ELSEIF  "$(CFG)" == "OldBars - Power Macintosh Release"

DEP_CPP_STATB=\
	".\Globals.h"\
	".\Statbar.h"\
	".\Stdafx.h"\
	{$(INCLUDE)}"\macos\Windows.h"\
	

"$(INTDIR)\Statbar.obj" : $(SOURCE) $(DEP_CPP_STATB) "$(INTDIR)"\
 "$(INTDIR)\OldBars.pch"


!ELSEIF  "$(CFG)" == "OldBars - Power Macintosh Debug"

DEP_CPP_STATB=\
	".\Globals.h"\
	".\Statbar.h"\
	".\Stdafx.h"\
	{$(INCLUDE)}"\macos\Windows.h"\
	

"$(INTDIR)\Statbar.obj" : $(SOURCE) $(DEP_CPP_STATB) "$(INTDIR)"\
 "$(INTDIR)\OldBars.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_STATB) "$(INTDIR)"\
 "$(INTDIR)\OldBars.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Stdafx.cpp
DEP_CPP_STDAF=\
	".\Stdafx.h"\
	

!IF  "$(CFG)" == "OldBars - Win32 Release"

# ADD CPP /Yc

BuildCmds= \
	$(CPP) /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/OldBars.pch" /Yc /Fo"$(INTDIR)/"\
 /Fd"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\Stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\vc40.idb" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\OldBars.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "OldBars - Win32 Debug"

# ADD CPP /Yc

BuildCmds= \
	$(CPP) /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/OldBars.pch" /Yc /Fo"$(INTDIR)/"\
 /Fd"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\Stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\vc40.idb" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\OldBars.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "OldBars - Macintosh Release"

# ADD CPP /Yc

BuildCmds= \
	$(CPP) /nologo /AL /Q68s /W3 /GX /O2 /D "_WINDOWS" /D "_MAC" /D "_68K_" /D\
 "WIN32" /D "NDEBUG" /D "_MBCS" /Fp"$(INTDIR)/OldBars.pch" /Yc /Fo"$(INTDIR)/"\
 /Fd"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\Stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\OldBars.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "OldBars - Macintosh Debug"

# ADD CPP /Yc"Stdafx.h"

BuildCmds= \
	$(CPP) /nologo /AL /Q68s /Q68m /W3 /GX /Zi /Od /D "_WINDOWS" /D "_MAC" /D\
 "_68K_" /D "WIN32" /D "_DEBUG" /D "_MBCS" /Fp"$(INTDIR)/OldBars.pch"\
 /Yc"Stdafx.h" /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\Stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\OldBars.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "OldBars - Power Macintosh Release"

# ADD CPP /Yc

BuildCmds= \
	$(CPP) /nologo /MD /W3 /GX /O2 /D "_MAC" /D "_MPPC_" /D "_WINDOWS" /D "WIN32"\
 /D "NDEBUG" /D "_MBCS" /D "_AFXDLL" /Fp"$(INTDIR)/OldBars.pch" /Yc\
 /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\Stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\OldBars.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "OldBars - Power Macintosh Debug"

# ADD CPP /Yc

BuildCmds= \
	$(CPP) /nologo /MDd /W3 /GX /Zi /Od /D "_MAC" /D "_MPPC_" /D "_WINDOWS" /D\
 "WIN32" /D "_DEBUG" /D "_MBCS" /D "_AFXDLL" /Fp"$(INTDIR)/OldBars.pch" /Yc\
 /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\Stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\OldBars.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Toolbar.cpp
DEP_CPP_TOOLB=\
	".\Globals.h"\
	".\Stdafx.h"\
	".\Toolbar.h"\
	

!IF  "$(CFG)" == "OldBars - Win32 Release"


"$(INTDIR)\Toolbar.obj" : $(SOURCE) $(DEP_CPP_TOOLB) "$(INTDIR)"\
 "$(INTDIR)\OldBars.pch"

"$(INTDIR)\vc40.idb" : $(SOURCE) $(DEP_CPP_TOOLB) "$(INTDIR)"\
 "$(INTDIR)\OldBars.pch"


!ELSEIF  "$(CFG)" == "OldBars - Win32 Debug"


"$(INTDIR)\Toolbar.obj" : $(SOURCE) $(DEP_CPP_TOOLB) "$(INTDIR)"\
 "$(INTDIR)\OldBars.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_TOOLB) "$(INTDIR)"\
 "$(INTDIR)\OldBars.pch"

"$(INTDIR)\vc40.idb" : $(SOURCE) $(DEP_CPP_TOOLB) "$(INTDIR)"\
 "$(INTDIR)\OldBars.pch"


!ELSEIF  "$(CFG)" == "OldBars - Macintosh Release"


"$(INTDIR)\Toolbar.obj" : $(SOURCE) $(DEP_CPP_TOOLB) "$(INTDIR)"\
 "$(INTDIR)\OldBars.pch"


!ELSEIF  "$(CFG)" == "OldBars - Macintosh Debug"


"$(INTDIR)\Toolbar.obj" : $(SOURCE) $(DEP_CPP_TOOLB) "$(INTDIR)"\
 "$(INTDIR)\OldBars.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_TOOLB) "$(INTDIR)"\
 "$(INTDIR)\OldBars.pch"


!ELSEIF  "$(CFG)" == "OldBars - Power Macintosh Release"


"$(INTDIR)\Toolbar.obj" : $(SOURCE) $(DEP_CPP_TOOLB) "$(INTDIR)"\
 "$(INTDIR)\OldBars.pch"


!ELSEIF  "$(CFG)" == "OldBars - Power Macintosh Debug"


"$(INTDIR)\Toolbar.obj" : $(SOURCE) $(DEP_CPP_TOOLB) "$(INTDIR)"\
 "$(INTDIR)\OldBars.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_TOOLB) "$(INTDIR)"\
 "$(INTDIR)\OldBars.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Oldbmac.r

!IF  "$(CFG)" == "OldBars - Win32 Release"

!ELSEIF  "$(CFG)" == "OldBars - Win32 Debug"

!ELSEIF  "$(CFG)" == "OldBars - Macintosh Release"


"$(INTDIR)\Oldbmac.rsc" : $(SOURCE) "$(INTDIR)"
   $(MRC) /o"$(INTDIR)/Oldbmac.rsc" /D "_68K_" /D "_MAC" /D "NDEBUG" /l 0x409\
 /NOLOGO $(SOURCE)


!ELSEIF  "$(CFG)" == "OldBars - Macintosh Debug"


"$(INTDIR)\Oldbmac.rsc" : $(SOURCE) "$(INTDIR)"
   $(MRC) /o"$(INTDIR)/Oldbmac.rsc" /D "_68K_" /D "_MAC" /D "_DEBUG" /l 0x409\
 /NOLOGO $(SOURCE)


!ELSEIF  "$(CFG)" == "OldBars - Power Macintosh Release"


"$(INTDIR)\Oldbmac.rsc" : $(SOURCE) "$(INTDIR)"
   $(MRC) /o"$(INTDIR)/Oldbmac.rsc" /D "_MPPC_" /D "_MAC" /D "NDEBUG" /l 0x409\
 /NOLOGO $(SOURCE)


!ELSEIF  "$(CFG)" == "OldBars - Power Macintosh Debug"


"$(INTDIR)\Oldbmac.rsc" : $(SOURCE) "$(INTDIR)"
   $(MRC) /o"$(INTDIR)/Oldbmac.rsc" /D "_MPPC_" /D "_MAC" /D "_DEBUG" /l 0x409\
 /NOLOGO $(SOURCE)


!ENDIF 

# End Source File
# End Target
# End Project
################################################################################
