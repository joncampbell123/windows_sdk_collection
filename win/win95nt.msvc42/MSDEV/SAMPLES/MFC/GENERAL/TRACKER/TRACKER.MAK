# Microsoft Visual C++ Generated NMAKE File, Format Version 40000
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101
# TARGTYPE "Power Macintosh Application" 0x0401
# TARGTYPE "Macintosh Application" 0x0301

!IF "$(CFG)" == ""
CFG=Tracker - Win32 Debug
!MESSAGE No configuration specified.  Defaulting to Tracker - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "Tracker - Win32 Release" && "$(CFG)" !=\
 "Tracker - Win32 Debug" && "$(CFG)" != "Tracker - Macintosh Release" &&\
 "$(CFG)" != "Tracker - Macintosh Debug" && "$(CFG)" !=\
 "Tracker - Power Macintosh Release" && "$(CFG)" !=\
 "Tracker - Power Macintosh Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "Tracker.mak" CFG="Tracker - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Tracker - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "Tracker - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE "Tracker - Macintosh Release" (based on "Macintosh Application")
!MESSAGE "Tracker - Macintosh Debug" (based on "Macintosh Application")
!MESSAGE "Tracker - Power Macintosh Release" (based on\
 "Power Macintosh Application")
!MESSAGE "Tracker - Power Macintosh Debug" (based on\
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
# PROP Target_Last_Scanned "Tracker - Win32 Debug"

!IF  "$(CFG)" == "Tracker - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir Release
# PROP BASE Intermediate_Dir Release
# PROP BASE Target_Dir
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 0
# PROP Output_Dir Release
# PROP Intermediate_Dir Release
# PROP Target_Dir
OUTDIR=.\Release
INTDIR=.\Release

ALL : "$(OUTDIR)\Tracker.exe"

CLEAN : 
	-@erase ".\Release\vc40.idb"
	-@erase ".\Release\Tracker.pch"
	-@erase ".\Release\Tracker.exe"
	-@erase ".\Release\Dialogs.obj"
	-@erase ".\Release\Trackapp.obj"
	-@erase ".\Release\Trackvw.obj"
	-@erase ".\Release\Mainfrm.obj"
	-@erase ".\Release\Trackdoc.obj"
	-@erase ".\Release\Stdafx.obj"
	-@erase ".\Release\Tracker.res"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"Stdafx.h" /c
# SUBTRACT CPP /Fr
CPP_PROJ=/nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/Tracker.pch" /Yu"Stdafx.h" /Fo"$(INTDIR)/"\
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
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/Tracker.res" /d "NDEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/Tracker.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 /nologo /subsystem:windows /machine:I386
LINK32_FLAGS=/nologo /subsystem:windows /incremental:no\
 /pdb:"$(OUTDIR)/Tracker.pdb" /machine:I386 /out:"$(OUTDIR)/Tracker.exe" 
LINK32_OBJS= \
	"$(INTDIR)/Dialogs.obj" \
	"$(INTDIR)/Trackapp.obj" \
	"$(INTDIR)/Trackvw.obj" \
	"$(INTDIR)/Mainfrm.obj" \
	"$(INTDIR)/Trackdoc.obj" \
	"$(INTDIR)/Stdafx.obj" \
	"$(INTDIR)/Tracker.res"

"$(OUTDIR)\Tracker.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "Tracker - Win32 Debug"

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

ALL : "$(OUTDIR)\Tracker.exe"

CLEAN : 
	-@erase ".\Debug\vc40.pdb"
	-@erase ".\Debug\Tracker.pch"
	-@erase ".\Debug\vc40.idb"
	-@erase ".\Debug\Tracker.exe"
	-@erase ".\Debug\Stdafx.obj"
	-@erase ".\Debug\Trackvw.obj"
	-@erase ".\Debug\Mainfrm.obj"
	-@erase ".\Debug\Trackapp.obj"
	-@erase ".\Debug\Trackdoc.obj"
	-@erase ".\Debug\Dialogs.obj"
	-@erase ".\Debug\Tracker.res"
	-@erase ".\Debug\Tracker.ilk"
	-@erase ".\Debug\Tracker.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"Stdafx.h" /c
# SUBTRACT CPP /Fr
CPP_PROJ=/nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/Tracker.pch" /Yu"Stdafx.h"\
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
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/Tracker.res" /d "_DEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/Tracker.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386
# ADD LINK32 /nologo /subsystem:windows /debug /machine:I386
LINK32_FLAGS=/nologo /subsystem:windows /incremental:yes\
 /pdb:"$(OUTDIR)/Tracker.pdb" /debug /machine:I386 /out:"$(OUTDIR)/Tracker.exe" 
LINK32_OBJS= \
	"$(INTDIR)/Stdafx.obj" \
	"$(INTDIR)/Trackvw.obj" \
	"$(INTDIR)/Mainfrm.obj" \
	"$(INTDIR)/Trackapp.obj" \
	"$(INTDIR)/Trackdoc.obj" \
	"$(INTDIR)/Dialogs.obj" \
	"$(INTDIR)/Tracker.res"

"$(OUTDIR)\Tracker.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "Tracker - Macintosh Release"

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

ALL : "$(OUTDIR)\Tracker.exe" "$(OUTDIR)\Tracker.trg"

CLEAN : 
	-@erase ".\MacRel\Tracker.trg"
	-@erase ".\MacRel\Tracker.exe"
	-@erase ".\MacRel\Trackvw.obj"
	-@erase ".\MacRel\Tracker.pch"
	-@erase ".\MacRel\Trackdoc.obj"
	-@erase ".\MacRel\Mainfrm.obj"
	-@erase ".\MacRel\Stdafx.obj"
	-@erase ".\MacRel\Dialogs.obj"
	-@erase ".\MacRel\Trackapp.obj"
	-@erase ".\MacRel\Tracker.rsc"
	-@erase ".\MacRel\trakmac.rsc"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

MTL=mktyplib.exe
# ADD BASE MTL /nologo /D "NDEBUG" /mac
# ADD MTL /nologo /D "NDEBUG" /mac
MTL_PROJ=/nologo /D "NDEBUG" /mac 
CPP=cl.exe
# ADD BASE CPP /nologo /AL /Q68s /W3 /GX /O2 /D "_WINDOWS" /D "_MAC" /D "_68K_" /D "WIN32" /D "NDEBUG" /YX /c
# ADD CPP /nologo /AL /Q68s /W3 /GX /O2 /D "_WINDOWS" /D "_MAC" /D "_68K_" /D "WIN32" /D "NDEBUG" /D "_MBCS" /Yu"Stdafx.h" /c
# SUBTRACT CPP /Fr
CPP_PROJ=/nologo /AL /Q68s /W3 /GX /O2 /D "_WINDOWS" /D "_MAC" /D "_68K_" /D\
 "WIN32" /D "NDEBUG" /D "_MBCS" /Fp"$(INTDIR)/Tracker.pch" /Yu"Stdafx.h"\
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
RSC_PROJ=/l 0x409 /r /m /fo"$(INTDIR)/Tracker.rsc" /d "_MAC" /d "_68K_" /d\
 "NDEBUG" 
MRC=mrc.exe
# ADD BASE MRC /D "_68K_" /D "_MAC" /D "NDEBUG" /NOLOGO
# ADD MRC /D "_68K_" /D "_MAC" /D "NDEBUG" /NOLOGO
MRC_PROJ=/D "_68K_" /D "_MAC" /D "NDEBUG" /l 0x409 /NOLOGO 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/Tracker.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 wlm.lib /nologo /MAC:bundle /machine:M68K
# ADD LINK32 /nologo /MAC:bundle /MAC:creator="TRAK" /machine:M68K
LINK32_FLAGS=/nologo /MAC:bundle /MAC:type="APPL" /MAC:creator="TRAK"\
 /pdb:"$(OUTDIR)/Tracker.pdb" /machine:M68K /out:"$(OUTDIR)/Tracker.exe" 
LINK32_OBJS= \
	"$(INTDIR)/Trackvw.obj" \
	"$(INTDIR)/Trackdoc.obj" \
	"$(INTDIR)/Mainfrm.obj" \
	"$(INTDIR)/Stdafx.obj" \
	"$(INTDIR)/Dialogs.obj" \
	"$(INTDIR)/Trackapp.obj" \
	"$(INTDIR)/Tracker.rsc" \
	"$(INTDIR)/trakmac.rsc"

"$(OUTDIR)\Tracker.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

MFILE32=mfile.exe
# ADD BASE MFILE32 COPY /NOLOGO
# ADD MFILE32 COPY /NOLOGO
MFILE32_FLAGS=COPY /NOLOGO 
MFILE32_FILES= \
	"$(OUTDIR)/Tracker.exe"

"$(OUTDIR)\Tracker.trg" : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\MacRel\Tracker.exe "$(MFILE32_DEST)"

DOWNLOAD : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\MacRel\Tracker.exe "$(MFILE32_DEST)"

!ELSEIF  "$(CFG)" == "Tracker - Macintosh Debug"

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

ALL : "$(OUTDIR)\Tracker.exe" "$(OUTDIR)\Tracker.trg"

CLEAN : 
	-@erase ".\MacDbg\vc40.pdb"
	-@erase ".\MacDbg\Tracker.pch"
	-@erase ".\MacDbg\Tracker.trg"
	-@erase ".\MacDbg\Tracker.exe"
	-@erase ".\MacDbg\Dialogs.obj"
	-@erase ".\MacDbg\Trackdoc.obj"
	-@erase ".\MacDbg\Stdafx.obj"
	-@erase ".\MacDbg\Trackvw.obj"
	-@erase ".\MacDbg\Mainfrm.obj"
	-@erase ".\MacDbg\Trackapp.obj"
	-@erase ".\MacDbg\trakmac.rsc"
	-@erase ".\MacDbg\Tracker.rsc"
	-@erase ".\MacDbg\Tracker.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

MTL=mktyplib.exe
# ADD BASE MTL /nologo /D "_DEBUG" /mac
# ADD MTL /nologo /D "_DEBUG" /mac
MTL_PROJ=/nologo /D "_DEBUG" /mac 
CPP=cl.exe
# ADD BASE CPP /nologo /AL /Q68s /Q68m /W3 /GX /Zi /Od /D "_WINDOWS" /D "_MAC" /D "_68K_" /D "WIN32" /D "_DEBUG" /YX /c
# ADD CPP /nologo /AL /Q68s /Q68m /W3 /GX /Zi /Od /D "_WINDOWS" /D "_MAC" /D "_68K_" /D "WIN32" /D "_DEBUG" /D "_MBCS" /Yu"Stdafx.h" /c
# SUBTRACT CPP /Fr
CPP_PROJ=/nologo /AL /Q68s /Q68m /W3 /GX /Zi /Od /D "_WINDOWS" /D "_MAC" /D\
 "_68K_" /D "WIN32" /D "_DEBUG" /D "_MBCS" /Fp"$(INTDIR)/Tracker.pch"\
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
RSC_PROJ=/l 0x409 /r /m /fo"$(INTDIR)/Tracker.rsc" /d "_MAC" /d "_68K_" /d\
 "_DEBUG" 
MRC=mrc.exe
# ADD BASE MRC /D "_68K_" /D "_MAC" /D "_DEBUG" /NOLOGO
# ADD MRC /D "_68K_" /D "_MAC" /D "_DEBUG" /NOLOGO
MRC_PROJ=/D "_68K_" /D "_MAC" /D "_DEBUG" /l 0x409 /NOLOGO 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/Tracker.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 wlm.lib /nologo /MAC:bundle /debug /machine:M68K
# ADD LINK32 /nologo /MAC:bundle /MAC:creator="TRAK" /debug /machine:M68K
LINK32_FLAGS=/nologo /MAC:bundle /MAC:type="APPL" /MAC:creator="TRAK"\
 /pdb:"$(OUTDIR)/Tracker.pdb" /debug /machine:M68K /out:"$(OUTDIR)/Tracker.exe" 
LINK32_OBJS= \
	"$(INTDIR)/Dialogs.obj" \
	"$(INTDIR)/Trackdoc.obj" \
	"$(INTDIR)/Stdafx.obj" \
	"$(INTDIR)/Trackvw.obj" \
	"$(INTDIR)/Mainfrm.obj" \
	"$(INTDIR)/Trackapp.obj" \
	"$(INTDIR)/trakmac.rsc" \
	"$(INTDIR)/Tracker.rsc"

"$(OUTDIR)\Tracker.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

MFILE32=mfile.exe
# ADD BASE MFILE32 COPY /NOLOGO
# ADD MFILE32 COPY /NOLOGO
MFILE32_FLAGS=COPY /NOLOGO 
MFILE32_FILES= \
	"$(OUTDIR)/Tracker.exe"

"$(OUTDIR)\Tracker.trg" : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\MacDbg\Tracker.exe "$(MFILE32_DEST)"

DOWNLOAD : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\MacDbg\Tracker.exe "$(MFILE32_DEST)"

!ELSEIF  "$(CFG)" == "Tracker - Power Macintosh Release"

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

ALL : "$(OUTDIR)\Tracker.exe" "$(OUTDIR)\Tracker.trg"

CLEAN : 
	-@erase ".\PMacRel\Tracker.trg"
	-@erase ".\PMacRel\Tracker.exe"
	-@erase ".\PMacRel\Trackdoc.obj"
	-@erase ".\PMacRel\Tracker.pch"
	-@erase ".\PMacRel\Trackvw.obj"
	-@erase ".\PMacRel\Mainfrm.obj"
	-@erase ".\PMacRel\Stdafx.obj"
	-@erase ".\PMacRel\Trackapp.obj"
	-@erase ".\PMacRel\Dialogs.obj"
	-@erase ".\PMacRel\Tracker.rsc"
	-@erase ".\PMacRel\trakmac.rsc"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

MTL=mktyplib.exe
# ADD BASE MTL /nologo /D "NDEBUG"
# ADD MTL /nologo /D "NDEBUG"
MTL_PROJ=/nologo /D "NDEBUG" 
CPP=cl.exe
# ADD BASE CPP /nologo /ML /W3 /GX /O2 /D "_MAC" /D "_MPPC_" /D "_WINDOWS" /D "WIN32" /D "NDEBUG" /YX /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "_MAC" /D "_MPPC_" /D "_WINDOWS" /D "WIN32" /D "_MBCS" /D "_AFXDLL" /Yu"Stdafx.h" /c
# SUBTRACT CPP /Fr
CPP_PROJ=/nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "_MAC" /D "_MPPC_" /D\
 "_WINDOWS" /D "WIN32" /D "_MBCS" /D "_AFXDLL" /Fp"$(INTDIR)/Tracker.pch"\
 /Yu"Stdafx.h" /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
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
RSC_PROJ=/l 0x409 /r /m /fo"$(INTDIR)/Tracker.rsc" /d "_MAC" /d "_MPPC_" /d\
 "NDEBUG" 
MRC=mrc.exe
# ADD BASE MRC /D "_MPPC_" /D "_MAC" /D "NDEBUG" /NOLOGO
# ADD MRC /D "_MPPC_" /D "_MAC" /D "NDEBUG" /NOLOGO
MRC_PROJ=/D "_MPPC_" /D "_MAC" /D "NDEBUG" /l 0x409 /NOLOGO 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/Tracker.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 cfrg.rsc /nologo /mac:bundle /machine:MPPC
# ADD LINK32 /nologo /mac:bundle /mac:creator="TRAK" /machine:MPPC
LINK32_FLAGS=/nologo /mac:bundle /mac:type="APPL" /mac:creator="TRAK"\
 /incremental:no /pdb:"$(OUTDIR)/Tracker.pdb" /machine:MPPC\
 /out:"$(OUTDIR)/Tracker.exe" 
LINK32_OBJS= \
	"$(INTDIR)/Trackdoc.obj" \
	"$(INTDIR)/Trackvw.obj" \
	"$(INTDIR)/Mainfrm.obj" \
	"$(INTDIR)/Stdafx.obj" \
	"$(INTDIR)/Trackapp.obj" \
	"$(INTDIR)/Dialogs.obj" \
	"$(INTDIR)/Tracker.rsc" \
	"$(INTDIR)/trakmac.rsc"

"$(OUTDIR)\Tracker.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

MFILE32=mfile.exe
# ADD BASE MFILE32 COPY /NOLOGO
# ADD MFILE32 COPY /NOLOGO
MFILE32_FLAGS=COPY /NOLOGO 
MFILE32_FILES= \
	"$(OUTDIR)/Tracker.exe"

"$(OUTDIR)\Tracker.trg" : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\PMacRel\Tracker.exe "$(MFILE32_DEST)"

DOWNLOAD : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\PMacRel\Tracker.exe "$(MFILE32_DEST)"

!ELSEIF  "$(CFG)" == "Tracker - Power Macintosh Debug"

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

ALL : "$(OUTDIR)\Tracker.exe" "$(OUTDIR)\Tracker.trg"

CLEAN : 
	-@erase ".\PMacDbg\vc40.pdb"
	-@erase ".\PMacDbg\Tracker.pch"
	-@erase ".\PMacDbg\Tracker.trg"
	-@erase ".\PMacDbg\Tracker.exe"
	-@erase ".\PMacDbg\Dialogs.obj"
	-@erase ".\PMacDbg\Trackdoc.obj"
	-@erase ".\PMacDbg\Stdafx.obj"
	-@erase ".\PMacDbg\Trackvw.obj"
	-@erase ".\PMacDbg\Mainfrm.obj"
	-@erase ".\PMacDbg\Trackapp.obj"
	-@erase ".\PMacDbg\trakmac.rsc"
	-@erase ".\PMacDbg\Tracker.rsc"
	-@erase ".\PMacDbg\Tracker.ilk"
	-@erase ".\PMacDbg\Tracker.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

MTL=mktyplib.exe
# ADD BASE MTL /nologo /D "_DEBUG"
# ADD MTL /nologo /D "_DEBUG"
MTL_PROJ=/nologo /D "_DEBUG" 
CPP=cl.exe
# ADD BASE CPP /nologo /W3 /GX /Zi /Od /D "_MAC" /D "_MPPC_" /D "_WINDOWS" /D "WIN32" /D "_DEBUG" /YX /c
# ADD CPP /nologo /MDd /W3 /GX /Zi /Od /D "_DEBUG" /D "_MAC" /D "_MPPC_" /D "_WINDOWS" /D "WIN32" /D "_MBCS" /D "_AFXDLL" /Yu"Stdafx.h" /c
# SUBTRACT CPP /Fr
CPP_PROJ=/nologo /MDd /W3 /GX /Zi /Od /D "_DEBUG" /D "_MAC" /D "_MPPC_" /D\
 "_WINDOWS" /D "WIN32" /D "_MBCS" /D "_AFXDLL" /Fp"$(INTDIR)/Tracker.pch"\
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
RSC_PROJ=/l 0x409 /r /m /fo"$(INTDIR)/Tracker.rsc" /d "_MAC" /d "_MPPC_" /d\
 "_DEBUG" 
MRC=mrc.exe
# ADD BASE MRC /D "_MPPC_" /D "_MAC" /D "_DEBUG" /NOLOGO
# ADD MRC /D "_MPPC_" /D "_MAC" /D "_DEBUG" /NOLOGO
MRC_PROJ=/D "_MPPC_" /D "_MAC" /D "_DEBUG" /l 0x409 /NOLOGO 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/Tracker.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 cfrg.rsc /nologo /mac:bundle /debug /machine:MPPC
# ADD LINK32 /nologo /mac:bundle /mac:creator="TRAK" /debug /machine:MPPC
LINK32_FLAGS=/nologo /mac:bundle /mac:type="APPL" /mac:creator="TRAK"\
 /incremental:yes /pdb:"$(OUTDIR)/Tracker.pdb" /debug /machine:MPPC\
 /out:"$(OUTDIR)/Tracker.exe" 
LINK32_OBJS= \
	"$(INTDIR)/Dialogs.obj" \
	"$(INTDIR)/Trackdoc.obj" \
	"$(INTDIR)/Stdafx.obj" \
	"$(INTDIR)/Trackvw.obj" \
	"$(INTDIR)/Mainfrm.obj" \
	"$(INTDIR)/Trackapp.obj" \
	"$(INTDIR)/trakmac.rsc" \
	"$(INTDIR)/Tracker.rsc"

"$(OUTDIR)\Tracker.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

MFILE32=mfile.exe
# ADD BASE MFILE32 COPY /NOLOGO
# ADD MFILE32 COPY /NOLOGO
MFILE32_FLAGS=COPY /NOLOGO 
MFILE32_FILES= \
	"$(OUTDIR)/Tracker.exe"

"$(OUTDIR)\Tracker.trg" : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\PMacDbg\Tracker.exe "$(MFILE32_DEST)"

DOWNLOAD : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\PMacDbg\Tracker.exe "$(MFILE32_DEST)"

!ENDIF 

################################################################################
# Begin Target

# Name "Tracker - Win32 Release"
# Name "Tracker - Win32 Debug"
# Name "Tracker - Macintosh Release"
# Name "Tracker - Macintosh Debug"
# Name "Tracker - Power Macintosh Release"
# Name "Tracker - Power Macintosh Debug"

!IF  "$(CFG)" == "Tracker - Win32 Release"

!ELSEIF  "$(CFG)" == "Tracker - Win32 Debug"

!ELSEIF  "$(CFG)" == "Tracker - Macintosh Release"

!ELSEIF  "$(CFG)" == "Tracker - Macintosh Debug"

!ELSEIF  "$(CFG)" == "Tracker - Power Macintosh Release"

!ELSEIF  "$(CFG)" == "Tracker - Power Macintosh Debug"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\Tracker.rc
DEP_RSC_TRACK=\
	".\Res\Tracker.ico"\
	".\Res\Trackdoc.ico"\
	".\Res\Toolbar.bmp"\
	".\Res\Tracker.rc2"\
	

!IF  "$(CFG)" == "Tracker - Win32 Release"


"$(INTDIR)\Tracker.res" : $(SOURCE) $(DEP_RSC_TRACK) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Tracker - Win32 Debug"


"$(INTDIR)\Tracker.res" : $(SOURCE) $(DEP_RSC_TRACK) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Tracker - Macintosh Release"


"$(INTDIR)\Tracker.rsc" : $(SOURCE) $(DEP_RSC_TRACK) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Tracker - Macintosh Debug"


"$(INTDIR)\Tracker.rsc" : $(SOURCE) $(DEP_RSC_TRACK) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Tracker - Power Macintosh Release"


"$(INTDIR)\Tracker.rsc" : $(SOURCE) $(DEP_RSC_TRACK) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Tracker - Power Macintosh Debug"


"$(INTDIR)\Tracker.rsc" : $(SOURCE) $(DEP_RSC_TRACK) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Mainfrm.cpp
DEP_CPP_MAINF=\
	".\Mainfrm.h"\
	".\Stdafx.h"\
	".\Trackapp.h"\
	

!IF  "$(CFG)" == "Tracker - Win32 Release"


"$(INTDIR)\Mainfrm.obj" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\Tracker.pch"

"$(INTDIR)\vc40.idb" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\Tracker.pch"


!ELSEIF  "$(CFG)" == "Tracker - Win32 Debug"


"$(INTDIR)\Mainfrm.obj" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\Tracker.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\Tracker.pch"

"$(INTDIR)\vc40.idb" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\Tracker.pch"


!ELSEIF  "$(CFG)" == "Tracker - Macintosh Release"


"$(INTDIR)\Mainfrm.obj" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\Tracker.pch"


!ELSEIF  "$(CFG)" == "Tracker - Macintosh Debug"


"$(INTDIR)\Mainfrm.obj" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\Tracker.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\Tracker.pch"


!ELSEIF  "$(CFG)" == "Tracker - Power Macintosh Release"


"$(INTDIR)\Mainfrm.obj" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\Tracker.pch"


!ELSEIF  "$(CFG)" == "Tracker - Power Macintosh Debug"


"$(INTDIR)\Mainfrm.obj" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\Tracker.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\Tracker.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Stdafx.cpp
DEP_CPP_STDAF=\
	".\Stdafx.h"\
	

!IF  "$(CFG)" == "Tracker - Win32 Release"

# ADD CPP /Yc

BuildCmds= \
	$(CPP) /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/Tracker.pch" /Yc /Fo"$(INTDIR)/"\
 /Fd"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\Stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\vc40.idb" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\Tracker.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "Tracker - Win32 Debug"

# ADD CPP /Yc"Stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/Tracker.pch" /Yc"Stdafx.h"\
 /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\Stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\vc40.idb" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\Tracker.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "Tracker - Macintosh Release"

# ADD CPP /Yc

BuildCmds= \
	$(CPP) /nologo /AL /Q68s /W3 /GX /O2 /D "_WINDOWS" /D "_MAC" /D "_68K_" /D\
 "WIN32" /D "NDEBUG" /D "_MBCS" /Fp"$(INTDIR)/Tracker.pch" /Yc /Fo"$(INTDIR)/"\
 /Fd"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\Stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\Tracker.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "Tracker - Macintosh Debug"

# ADD CPP /Yc

BuildCmds= \
	$(CPP) /nologo /AL /Q68s /Q68m /W3 /GX /Zi /Od /D "_WINDOWS" /D "_MAC" /D\
 "_68K_" /D "WIN32" /D "_DEBUG" /D "_MBCS" /Fp"$(INTDIR)/Tracker.pch" /Yc\
 /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\Stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\Tracker.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "Tracker - Power Macintosh Release"

# ADD CPP /Yc

BuildCmds= \
	$(CPP) /nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "_MAC" /D "_MPPC_" /D "_WINDOWS"\
 /D "WIN32" /D "_MBCS" /D "_AFXDLL" /Fp"$(INTDIR)/Tracker.pch" /Yc\
 /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\Stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\Tracker.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "Tracker - Power Macintosh Debug"

# ADD CPP /Yc

BuildCmds= \
	$(CPP) /nologo /MDd /W3 /GX /Zi /Od /D "_DEBUG" /D "_MAC" /D "_MPPC_" /D\
 "_WINDOWS" /D "WIN32" /D "_MBCS" /D "_AFXDLL" /Fp"$(INTDIR)/Tracker.pch" /Yc\
 /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\Stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\Tracker.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Trackapp.cpp

!IF  "$(CFG)" == "Tracker - Win32 Release"

DEP_CPP_TRACKA=\
	".\Mainfrm.h"\
	".\Stdafx.h"\
	".\Trackapp.h"\
	".\Trackdoc.h"\
	".\Trackvw.h"\
	

"$(INTDIR)\Trackapp.obj" : $(SOURCE) $(DEP_CPP_TRACKA) "$(INTDIR)"\
 "$(INTDIR)\Tracker.pch"

"$(INTDIR)\vc40.idb" : $(SOURCE) $(DEP_CPP_TRACKA) "$(INTDIR)"\
 "$(INTDIR)\Tracker.pch"


!ELSEIF  "$(CFG)" == "Tracker - Win32 Debug"

DEP_CPP_TRACKA=\
	".\Mainfrm.h"\
	".\Stdafx.h"\
	".\Trackapp.h"\
	".\Trackdoc.h"\
	".\Trackvw.h"\
	

"$(INTDIR)\Trackapp.obj" : $(SOURCE) $(DEP_CPP_TRACKA) "$(INTDIR)"\
 "$(INTDIR)\Tracker.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_TRACKA) "$(INTDIR)"\
 "$(INTDIR)\Tracker.pch"

"$(INTDIR)\vc40.idb" : $(SOURCE) $(DEP_CPP_TRACKA) "$(INTDIR)"\
 "$(INTDIR)\Tracker.pch"


!ELSEIF  "$(CFG)" == "Tracker - Macintosh Release"

DEP_CPP_TRACKA=\
	".\Mainfrm.h"\
	".\Stdafx.h"\
	".\Trackapp.h"\
	".\Resource.h"\
	".\Trackdoc.h"\
	".\Trackvw.h"\
	

"$(INTDIR)\Trackapp.obj" : $(SOURCE) $(DEP_CPP_TRACKA) "$(INTDIR)"\
 "$(INTDIR)\Tracker.pch"


!ELSEIF  "$(CFG)" == "Tracker - Macintosh Debug"

DEP_CPP_TRACKA=\
	".\Mainfrm.h"\
	".\Stdafx.h"\
	".\Trackapp.h"\
	".\Trackdoc.h"\
	".\Trackvw.h"\
	

"$(INTDIR)\Trackapp.obj" : $(SOURCE) $(DEP_CPP_TRACKA) "$(INTDIR)"\
 "$(INTDIR)\Tracker.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_TRACKA) "$(INTDIR)"\
 "$(INTDIR)\Tracker.pch"


!ELSEIF  "$(CFG)" == "Tracker - Power Macintosh Release"

DEP_CPP_TRACKA=\
	".\Mainfrm.h"\
	".\Stdafx.h"\
	".\Trackapp.h"\
	".\Trackdoc.h"\
	".\Trackvw.h"\
	

"$(INTDIR)\Trackapp.obj" : $(SOURCE) $(DEP_CPP_TRACKA) "$(INTDIR)"\
 "$(INTDIR)\Tracker.pch"


!ELSEIF  "$(CFG)" == "Tracker - Power Macintosh Debug"

DEP_CPP_TRACKA=\
	".\Mainfrm.h"\
	".\Stdafx.h"\
	".\Trackapp.h"\
	".\Trackdoc.h"\
	".\Trackvw.h"\
	

"$(INTDIR)\Trackapp.obj" : $(SOURCE) $(DEP_CPP_TRACKA) "$(INTDIR)"\
 "$(INTDIR)\Tracker.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_TRACKA) "$(INTDIR)"\
 "$(INTDIR)\Tracker.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Trackdoc.cpp

!IF  "$(CFG)" == "Tracker - Win32 Release"

DEP_CPP_TRACKD=\
	".\Stdafx.h"\
	".\Trackapp.h"\
	".\Trackdoc.h"\
	

"$(INTDIR)\Trackdoc.obj" : $(SOURCE) $(DEP_CPP_TRACKD) "$(INTDIR)"\
 "$(INTDIR)\Tracker.pch"

"$(INTDIR)\vc40.idb" : $(SOURCE) $(DEP_CPP_TRACKD) "$(INTDIR)"\
 "$(INTDIR)\Tracker.pch"


!ELSEIF  "$(CFG)" == "Tracker - Win32 Debug"

DEP_CPP_TRACKD=\
	".\Stdafx.h"\
	".\Trackapp.h"\
	".\Trackdoc.h"\
	

"$(INTDIR)\Trackdoc.obj" : $(SOURCE) $(DEP_CPP_TRACKD) "$(INTDIR)"\
 "$(INTDIR)\Tracker.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_TRACKD) "$(INTDIR)"\
 "$(INTDIR)\Tracker.pch"

"$(INTDIR)\vc40.idb" : $(SOURCE) $(DEP_CPP_TRACKD) "$(INTDIR)"\
 "$(INTDIR)\Tracker.pch"


!ELSEIF  "$(CFG)" == "Tracker - Macintosh Release"

DEP_CPP_TRACKD=\
	".\Stdafx.h"\
	".\Trackapp.h"\
	".\Resource.h"\
	".\Trackdoc.h"\
	

"$(INTDIR)\Trackdoc.obj" : $(SOURCE) $(DEP_CPP_TRACKD) "$(INTDIR)"\
 "$(INTDIR)\Tracker.pch"


!ELSEIF  "$(CFG)" == "Tracker - Macintosh Debug"

DEP_CPP_TRACKD=\
	".\Stdafx.h"\
	".\Trackapp.h"\
	".\Trackdoc.h"\
	

"$(INTDIR)\Trackdoc.obj" : $(SOURCE) $(DEP_CPP_TRACKD) "$(INTDIR)"\
 "$(INTDIR)\Tracker.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_TRACKD) "$(INTDIR)"\
 "$(INTDIR)\Tracker.pch"


!ELSEIF  "$(CFG)" == "Tracker - Power Macintosh Release"

DEP_CPP_TRACKD=\
	".\Stdafx.h"\
	".\Trackapp.h"\
	".\Trackdoc.h"\
	

"$(INTDIR)\Trackdoc.obj" : $(SOURCE) $(DEP_CPP_TRACKD) "$(INTDIR)"\
 "$(INTDIR)\Tracker.pch"


!ELSEIF  "$(CFG)" == "Tracker - Power Macintosh Debug"

DEP_CPP_TRACKD=\
	".\Stdafx.h"\
	".\Trackapp.h"\
	".\Trackdoc.h"\
	

"$(INTDIR)\Trackdoc.obj" : $(SOURCE) $(DEP_CPP_TRACKD) "$(INTDIR)"\
 "$(INTDIR)\Tracker.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_TRACKD) "$(INTDIR)"\
 "$(INTDIR)\Tracker.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Dialogs.cpp

!IF  "$(CFG)" == "Tracker - Win32 Release"

DEP_CPP_DIALO=\
	".\Dialogs.h"\
	".\Stdafx.h"\
	".\Trackapp.h"\
	

"$(INTDIR)\Dialogs.obj" : $(SOURCE) $(DEP_CPP_DIALO) "$(INTDIR)"\
 "$(INTDIR)\Tracker.pch"

"$(INTDIR)\vc40.idb" : $(SOURCE) $(DEP_CPP_DIALO) "$(INTDIR)"\
 "$(INTDIR)\Tracker.pch"


!ELSEIF  "$(CFG)" == "Tracker - Win32 Debug"

DEP_CPP_DIALO=\
	".\Dialogs.h"\
	".\Stdafx.h"\
	".\Trackapp.h"\
	

"$(INTDIR)\Dialogs.obj" : $(SOURCE) $(DEP_CPP_DIALO) "$(INTDIR)"\
 "$(INTDIR)\Tracker.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_DIALO) "$(INTDIR)"\
 "$(INTDIR)\Tracker.pch"

"$(INTDIR)\vc40.idb" : $(SOURCE) $(DEP_CPP_DIALO) "$(INTDIR)"\
 "$(INTDIR)\Tracker.pch"


!ELSEIF  "$(CFG)" == "Tracker - Macintosh Release"

DEP_CPP_DIALO=\
	".\Dialogs.h"\
	".\Stdafx.h"\
	".\Trackapp.h"\
	".\Resource.h"\
	

"$(INTDIR)\Dialogs.obj" : $(SOURCE) $(DEP_CPP_DIALO) "$(INTDIR)"\
 "$(INTDIR)\Tracker.pch"


!ELSEIF  "$(CFG)" == "Tracker - Macintosh Debug"

DEP_CPP_DIALO=\
	".\Dialogs.h"\
	".\Stdafx.h"\
	".\Trackapp.h"\
	

"$(INTDIR)\Dialogs.obj" : $(SOURCE) $(DEP_CPP_DIALO) "$(INTDIR)"\
 "$(INTDIR)\Tracker.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_DIALO) "$(INTDIR)"\
 "$(INTDIR)\Tracker.pch"


!ELSEIF  "$(CFG)" == "Tracker - Power Macintosh Release"

DEP_CPP_DIALO=\
	".\Dialogs.h"\
	".\Stdafx.h"\
	".\Trackapp.h"\
	

"$(INTDIR)\Dialogs.obj" : $(SOURCE) $(DEP_CPP_DIALO) "$(INTDIR)"\
 "$(INTDIR)\Tracker.pch"


!ELSEIF  "$(CFG)" == "Tracker - Power Macintosh Debug"

DEP_CPP_DIALO=\
	".\Dialogs.h"\
	".\Stdafx.h"\
	".\Trackapp.h"\
	

"$(INTDIR)\Dialogs.obj" : $(SOURCE) $(DEP_CPP_DIALO) "$(INTDIR)"\
 "$(INTDIR)\Tracker.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_DIALO) "$(INTDIR)"\
 "$(INTDIR)\Tracker.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Trackvw.cpp
DEP_CPP_TRACKV=\
	".\Stdafx.h"\
	".\Trackapp.h"\
	".\Trackdoc.h"\
	".\Trackvw.h"\
	".\Dialogs.h"\
	

!IF  "$(CFG)" == "Tracker - Win32 Release"


"$(INTDIR)\Trackvw.obj" : $(SOURCE) $(DEP_CPP_TRACKV) "$(INTDIR)"\
 "$(INTDIR)\Tracker.pch"

"$(INTDIR)\vc40.idb" : $(SOURCE) $(DEP_CPP_TRACKV) "$(INTDIR)"\
 "$(INTDIR)\Tracker.pch"


!ELSEIF  "$(CFG)" == "Tracker - Win32 Debug"


"$(INTDIR)\Trackvw.obj" : $(SOURCE) $(DEP_CPP_TRACKV) "$(INTDIR)"\
 "$(INTDIR)\Tracker.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_TRACKV) "$(INTDIR)"\
 "$(INTDIR)\Tracker.pch"

"$(INTDIR)\vc40.idb" : $(SOURCE) $(DEP_CPP_TRACKV) "$(INTDIR)"\
 "$(INTDIR)\Tracker.pch"


!ELSEIF  "$(CFG)" == "Tracker - Macintosh Release"


"$(INTDIR)\Trackvw.obj" : $(SOURCE) $(DEP_CPP_TRACKV) "$(INTDIR)"\
 "$(INTDIR)\Tracker.pch"


!ELSEIF  "$(CFG)" == "Tracker - Macintosh Debug"


"$(INTDIR)\Trackvw.obj" : $(SOURCE) $(DEP_CPP_TRACKV) "$(INTDIR)"\
 "$(INTDIR)\Tracker.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_TRACKV) "$(INTDIR)"\
 "$(INTDIR)\Tracker.pch"


!ELSEIF  "$(CFG)" == "Tracker - Power Macintosh Release"


"$(INTDIR)\Trackvw.obj" : $(SOURCE) $(DEP_CPP_TRACKV) "$(INTDIR)"\
 "$(INTDIR)\Tracker.pch"


!ELSEIF  "$(CFG)" == "Tracker - Power Macintosh Debug"


"$(INTDIR)\Trackvw.obj" : $(SOURCE) $(DEP_CPP_TRACKV) "$(INTDIR)"\
 "$(INTDIR)\Tracker.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_TRACKV) "$(INTDIR)"\
 "$(INTDIR)\Tracker.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\trakmac.r

!IF  "$(CFG)" == "Tracker - Win32 Release"

!ELSEIF  "$(CFG)" == "Tracker - Win32 Debug"

!ELSEIF  "$(CFG)" == "Tracker - Macintosh Release"

DEP_MRC_TRAKM=\
	{$(INCLUDE)}"\mrc\Balloons.r"\
	

"$(INTDIR)\trakmac.rsc" : $(SOURCE) $(DEP_MRC_TRAKM) "$(INTDIR)"
   $(MRC) /o"$(INTDIR)/trakmac.rsc" /D "_68K_" /D "_MAC" /D "NDEBUG" /l 0x409\
 /NOLOGO $(SOURCE)


!ELSEIF  "$(CFG)" == "Tracker - Macintosh Debug"

DEP_MRC_TRAKM=\
	{$(INCLUDE)}"\mrc\Balloons.r"\
	

"$(INTDIR)\trakmac.rsc" : $(SOURCE) $(DEP_MRC_TRAKM) "$(INTDIR)"
   $(MRC) /o"$(INTDIR)/trakmac.rsc" /D "_68K_" /D "_MAC" /D "_DEBUG" /l 0x409\
 /NOLOGO $(SOURCE)


!ELSEIF  "$(CFG)" == "Tracker - Power Macintosh Release"

DEP_MRC_TRAKM=\
	{$(INCLUDE)}"\mrc\Balloons.r"\
	

"$(INTDIR)\trakmac.rsc" : $(SOURCE) $(DEP_MRC_TRAKM) "$(INTDIR)"
   $(MRC) /o"$(INTDIR)/trakmac.rsc" /D "_MPPC_" /D "_MAC" /D "NDEBUG" /l 0x409\
 /NOLOGO $(SOURCE)


!ELSEIF  "$(CFG)" == "Tracker - Power Macintosh Debug"

DEP_MRC_TRAKM=\
	{$(INCLUDE)}"\mrc\Balloons.r"\
	

"$(INTDIR)\trakmac.rsc" : $(SOURCE) $(DEP_MRC_TRAKM) "$(INTDIR)"
   $(MRC) /o"$(INTDIR)/trakmac.rsc" /D "_MPPC_" /D "_MAC" /D "_DEBUG" /l 0x409\
 /NOLOGO $(SOURCE)


!ENDIF 

# End Source File
# End Target
# End Project
################################################################################
