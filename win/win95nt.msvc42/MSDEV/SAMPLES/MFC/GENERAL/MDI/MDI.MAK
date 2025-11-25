# Microsoft Developer Studio Generated NMAKE File, Format Version 42001
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101
# TARGTYPE "Power Macintosh Application" 0x0401
# TARGTYPE "Macintosh Application" 0x0301

!IF "$(CFG)" == ""
CFG=MDI - Win32 Debug
!MESSAGE No configuration specified.  Defaulting to MDI - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "MDI - Win32 Release" && "$(CFG)" != "MDI - Win32 Debug" &&\
 "$(CFG)" != "MDI - Macintosh Release" && "$(CFG)" != "MDI - Macintosh Debug" &&\
 "$(CFG)" != "MDI - Power Macintosh Release" && "$(CFG)" !=\
 "MDI - Power Macintosh Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "mdi.mak" CFG="MDI - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "MDI - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "MDI - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE "MDI - Macintosh Release" (based on "Macintosh Application")
!MESSAGE "MDI - Macintosh Debug" (based on "Macintosh Application")
!MESSAGE "MDI - Power Macintosh Release" (based on\
 "Power Macintosh Application")
!MESSAGE "MDI - Power Macintosh Debug" (based on "Power Macintosh Application")
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
# PROP Target_Last_Scanned "MDI - Power Macintosh Debug"

!IF  "$(CFG)" == "MDI - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
OUTDIR=.\Release
INTDIR=.\Release

ALL : "$(OUTDIR)\mdi.exe"

CLEAN : 
	-@erase "$(INTDIR)\bounce.obj"
	-@erase "$(INTDIR)\hello.obj"
	-@erase "$(INTDIR)\mainfrm.obj"
	-@erase "$(INTDIR)\mdi.obj"
	-@erase "$(INTDIR)\mdi.pch"
	-@erase "$(INTDIR)\mdi.res"
	-@erase "$(INTDIR)\stdafx.obj"
	-@erase "$(OUTDIR)\mdi.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"Stdafx.h" /c
# SUBTRACT CPP /Fr
CPP_PROJ=/nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/mdi.pch" /Yu"Stdafx.h" /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\Release/
CPP_SBRS=.\.

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
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/mdi.res" /d "NDEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/mdi.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 /nologo /subsystem:windows /machine:I386
LINK32_FLAGS=/nologo /subsystem:windows /incremental:no\
 /pdb:"$(OUTDIR)/mdi.pdb" /machine:I386 /out:"$(OUTDIR)/mdi.exe" 
LINK32_OBJS= \
	"$(INTDIR)\bounce.obj" \
	"$(INTDIR)\hello.obj" \
	"$(INTDIR)\mainfrm.obj" \
	"$(INTDIR)\mdi.obj" \
	"$(INTDIR)\mdi.res" \
	"$(INTDIR)\stdafx.obj"

"$(OUTDIR)\mdi.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "MDI - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
OUTDIR=.\Debug
INTDIR=.\Debug

ALL : "$(OUTDIR)\mdi.exe"

CLEAN : 
	-@erase "$(INTDIR)\bounce.obj"
	-@erase "$(INTDIR)\hello.obj"
	-@erase "$(INTDIR)\mainfrm.obj"
	-@erase "$(INTDIR)\mdi.obj"
	-@erase "$(INTDIR)\mdi.pch"
	-@erase "$(INTDIR)\mdi.res"
	-@erase "$(INTDIR)\stdafx.obj"
	-@erase "$(INTDIR)\vc40.idb"
	-@erase "$(INTDIR)\vc40.pdb"
	-@erase "$(OUTDIR)\mdi.exe"
	-@erase "$(OUTDIR)\mdi.ilk"
	-@erase "$(OUTDIR)\mdi.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"Stdafx.h" /c
# SUBTRACT CPP /Fr
CPP_PROJ=/nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/mdi.pch" /Yu"Stdafx.h" /Fo"$(INTDIR)/"\
 /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\Debug/
CPP_SBRS=.\.

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
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/mdi.res" /d "_DEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/mdi.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386
# ADD LINK32 /nologo /subsystem:windows /debug /machine:I386
LINK32_FLAGS=/nologo /subsystem:windows /incremental:yes\
 /pdb:"$(OUTDIR)/mdi.pdb" /debug /machine:I386 /out:"$(OUTDIR)/mdi.exe" 
LINK32_OBJS= \
	"$(INTDIR)\bounce.obj" \
	"$(INTDIR)\hello.obj" \
	"$(INTDIR)\mainfrm.obj" \
	"$(INTDIR)\mdi.obj" \
	"$(INTDIR)\mdi.res" \
	"$(INTDIR)\stdafx.obj"

"$(OUTDIR)\mdi.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "MDI - Macintosh Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "MacRel"
# PROP BASE Intermediate_Dir "MacRel"
# PROP BASE Target_Dir ""
# PROP Use_MFC 1
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "MacRel"
# PROP Intermediate_Dir "MacRel"
# PROP Target_Dir ""
OUTDIR=.\MacRel
INTDIR=.\MacRel

ALL :  "$(OUTDIR)\mdi.exe" "$(OUTDIR)\mdi.trg"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CLEAN : 
	-@erase ".\MacRel\mdi.trg"
	-@erase ".\MacRel\mdi.exe"
	-@erase ".\MacRel\bounce.obj"
	-@erase ".\MacRel\mdi.pch"
	-@erase ".\MacRel\stdafx.obj"
	-@erase ".\MacRel\mdi.obj"
	-@erase ".\MacRel\hello.obj"
	-@erase ".\MacRel\mainfrm.obj"
	-@erase ".\MacRel\mdi.rsc"
	-@erase ".\MacRel\mdimac.rsc"

MTL=mktyplib.exe
# ADD BASE MTL /nologo /D "NDEBUG" /mac
# ADD MTL /nologo /D "NDEBUG" /mac
MTL_PROJ=/nologo /D "NDEBUG" /mac 
CPP=cl.exe
# ADD BASE CPP /nologo /AL /Q68s /W3 /GX /O2 /D "_WINDOWS" /D "_MAC" /D "_68K_" /D "WIN32" /D "NDEBUG" /YX /c
# ADD CPP /nologo /AL /Q68s /W3 /GX /O2 /D "_WINDOWS" /D "_MAC" /D "_68K_" /D "WIN32" /D "NDEBUG" /D "_MBCS" /Yu"Stdafx.h" /c
# SUBTRACT CPP /Fr
CPP_PROJ=/nologo /AL /Q68s /W3 /GX /O2 /D "_WINDOWS" /D "_MAC" /D "_68K_" /D\
 "WIN32" /D "NDEBUG" /D "_MBCS" /Fp"$(INTDIR)/mdi.pch" /Yu"Stdafx.h"\
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
RSC_PROJ=/l 0x409 /r /m /fo"$(INTDIR)/mdi.rsc" /d "_MAC" /d "_68K_" /d "NDEBUG"\
 
MRC=mrc.exe
# ADD BASE MRC /D "_68K_" /D "_MAC" /D "NDEBUG" /NOLOGO
# ADD MRC /D "_68K_" /D "_MAC" /D "NDEBUG" /NOLOGO
MRC_PROJ=/D "_68K_" /D "_MAC" /D "NDEBUG" /l 0x409 /NOLOGO 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/mdi.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 wlm.lib /nologo /MAC:bundle /machine:M68K
# ADD LINK32 /nologo /MAC:bundle /MAC:creator="MDIX" /machine:M68K
LINK32_FLAGS=/nologo /MAC:bundle /MAC:type="APPL" /MAC:creator="MDIX"\
 /pdb:"$(OUTDIR)/mdi.pdb" /machine:M68K /out:"$(OUTDIR)/mdi.exe" 
LINK32_OBJS= \
	"$(INTDIR)/bounce.obj" \
	"$(INTDIR)/stdafx.obj" \
	"$(INTDIR)/mdi.obj" \
	"$(INTDIR)/hello.obj" \
	"$(INTDIR)/mainfrm.obj" \
	"$(INTDIR)/mdi.rsc" \
	"$(INTDIR)/mdimac.rsc"

"$(OUTDIR)\mdi.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

MFILE32=mfile.exe
# ADD BASE MFILE32 COPY /NOLOGO
# ADD MFILE32 COPY /NOLOGO
MFILE32_FLAGS=COPY /NOLOGO 
MFILE32_FILES= \
	"$(OUTDIR)/mdi.exe"

"$(OUTDIR)\mdi.trg" : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\MacRel\mdi.exe\
 "$(MFILE32_DEST):mdi.exe">"$(OUTDIR)\mdi.trg"

DOWNLOAD : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\MacRel\mdi.exe\
 "$(MFILE32_DEST):mdi.exe">"$(OUTDIR)\mdi.trg"

!ELSEIF  "$(CFG)" == "MDI - Macintosh Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "MacDbg"
# PROP BASE Intermediate_Dir "MacDbg"
# PROP BASE Target_Dir ""
# PROP Use_MFC 1
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "MacDbg"
# PROP Intermediate_Dir "MacDbg"
# PROP Target_Dir ""
OUTDIR=.\MacDbg
INTDIR=.\MacDbg

ALL :  "$(OUTDIR)\mdi.exe" "$(OUTDIR)\mdi.trg"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CLEAN : 
	-@erase ".\MacDbg\vc40.pdb"
	-@erase ".\MacDbg\mdi.pch"
	-@erase ".\MacDbg\mdi.pdb"
	-@erase ".\MacDbg\mainfrm.obj"
	-@erase ".\MacDbg\mdi.obj"
	-@erase ".\MacDbg\stdafx.obj"
	-@erase ".\MacDbg\hello.obj"
	-@erase ".\MacDbg\bounce.obj"
	-@erase ".\MacDbg\mdimac.rsc"
	-@erase ".\MacDbg\mdi.rsc"
	-@erase ".\MacDbg\mdi.trg"
	-@erase ".\MacDbg\mdi.exe"

MTL=mktyplib.exe
# ADD BASE MTL /nologo /D "_DEBUG" /mac
# ADD MTL /nologo /D "_DEBUG" /mac
MTL_PROJ=/nologo /D "_DEBUG" /mac 
CPP=cl.exe
# ADD BASE CPP /nologo /AL /Q68s /Q68m /W3 /GX /Zi /Od /D "_WINDOWS" /D "_MAC" /D "_68K_" /D "WIN32" /D "_DEBUG" /YX /c
# ADD CPP /nologo /AL /Q68s /Q68m /W3 /GX /Zi /Od /D "_WINDOWS" /D "_MAC" /D "_68K_" /D "WIN32" /D "_DEBUG" /D "_MBCS" /Yu"Stdafx.h" /c
# SUBTRACT CPP /Fr
CPP_PROJ=/nologo /AL /Q68s /Q68m /W3 /GX /Zi /Od /D "_WINDOWS" /D "_MAC" /D\
 "_68K_" /D "WIN32" /D "_DEBUG" /D "_MBCS" /Fp"$(INTDIR)/mdi.pch" /Yu"Stdafx.h"\
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
RSC_PROJ=/l 0x409 /r /m /fo"$(INTDIR)/mdi.rsc" /d "_MAC" /d "_68K_" /d "_DEBUG"\
 
MRC=mrc.exe
# ADD BASE MRC /D "_68K_" /D "_MAC" /D "_DEBUG" /NOLOGO
# ADD MRC /D "_68K_" /D "_MAC" /D "_DEBUG" /NOLOGO
MRC_PROJ=/D "_68K_" /D "_MAC" /D "_DEBUG" /l 0x409 /NOLOGO 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/mdi.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 wlm.lib /nologo /MAC:bundle /debug /machine:M68K
# ADD LINK32 /nologo /MAC:bundle /MAC:creator="MDIX" /debug /machine:M68K
LINK32_FLAGS=/nologo /MAC:bundle /MAC:type="APPL" /MAC:creator="MDIX"\
 /pdb:"$(OUTDIR)/mdi.pdb" /debug /machine:M68K /out:"$(OUTDIR)/mdi.exe" 
LINK32_OBJS= \
	"$(INTDIR)/mainfrm.obj" \
	"$(INTDIR)/mdi.obj" \
	"$(INTDIR)/stdafx.obj" \
	"$(INTDIR)/hello.obj" \
	"$(INTDIR)/bounce.obj" \
	"$(INTDIR)/mdimac.rsc" \
	"$(INTDIR)/mdi.rsc"

"$(OUTDIR)\mdi.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

MFILE32=mfile.exe
# ADD BASE MFILE32 COPY /NOLOGO
# ADD MFILE32 COPY /NOLOGO
MFILE32_FLAGS=COPY /NOLOGO 
MFILE32_FILES= \
	"$(OUTDIR)/mdi.exe"

"$(OUTDIR)\mdi.trg" : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\MacDbg\mdi.exe\
 "$(MFILE32_DEST):mdi.exe">"$(OUTDIR)\mdi.trg"

DOWNLOAD : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\MacDbg\mdi.exe\
 "$(MFILE32_DEST):mdi.exe">"$(OUTDIR)\mdi.trg"

!ELSEIF  "$(CFG)" == "MDI - Power Macintosh Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "PMacRel"
# PROP BASE Intermediate_Dir "PMacRel"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "PMacRel"
# PROP Intermediate_Dir "PMacRel"
# PROP Target_Dir ""
OUTDIR=.\PMacRel
INTDIR=.\PMacRel

ALL :  "$(OUTDIR)\mdi.exe" "$(OUTDIR)\mdi.trg"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CLEAN : 
	-@erase ".\PMacRel\mdi.trg"
	-@erase ".\PMacRel\mdi.exe"
	-@erase ".\PMacRel\hello.obj"
	-@erase ".\PMacRel\mdi.pch"
	-@erase ".\PMacRel\stdafx.obj"
	-@erase ".\PMacRel\mainfrm.obj"
	-@erase ".\PMacRel\mdi.obj"
	-@erase ".\PMacRel\bounce.obj"
	-@erase ".\PMacRel\mdi.rsc"
	-@erase ".\PMacRel\mdimac.rsc"

MTL=mktyplib.exe
# ADD BASE MTL /nologo /D "NDEBUG"
# ADD MTL /nologo /D "NDEBUG"
MTL_PROJ=/nologo /D "NDEBUG" 
CPP=cl.exe
# ADD BASE CPP /nologo /ML /W3 /GX /O2 /D "_MAC" /D "_MPPC_" /D "_WINDOWS" /D "WIN32" /D "NDEBUG" /YX /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "_MAC" /D "_MPPC_" /D "_WINDOWS" /D "WIN32" /D "_MBCS" /D "_AFXDLL" /Yu"Stdafx.h" /c
# SUBTRACT CPP /Fr
CPP_PROJ=/nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "_MAC" /D "_MPPC_" /D\
 "_WINDOWS" /D "WIN32" /D "_MBCS" /D "_AFXDLL" /Fp"$(INTDIR)/mdi.pch"\
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
RSC_PROJ=/l 0x409 /r /m /fo"$(INTDIR)/mdi.rsc" /d "_MAC" /d "_MPPC_" /d\
 "NDEBUG" 
MRC=mrc.exe
# ADD BASE MRC /D "_MPPC_" /D "_MAC" /D "NDEBUG" /NOLOGO
# ADD MRC /D "_MPPC_" /D "_MAC" /D "NDEBUG" /NOLOGO
MRC_PROJ=/D "_MPPC_" /D "_MAC" /D "NDEBUG" /l 0x409 /NOLOGO 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/mdi.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 cfrg.rsc /nologo /mac:bundle /machine:MPPC
# ADD LINK32 /nologo /mac:bundle /mac:creator="MDIX" /machine:MPPC
LINK32_FLAGS=/nologo /mac:bundle /mac:type="APPL" /mac:creator="MDIX"\
 /incremental:no /pdb:"$(OUTDIR)/mdi.pdb" /machine:MPPC /out:"$(OUTDIR)/mdi.exe"\
 
LINK32_OBJS= \
	"$(INTDIR)/hello.obj" \
	"$(INTDIR)/stdafx.obj" \
	"$(INTDIR)/mainfrm.obj" \
	"$(INTDIR)/mdi.obj" \
	"$(INTDIR)/bounce.obj" \
	"$(INTDIR)/mdi.rsc" \
	"$(INTDIR)/mdimac.rsc"

"$(OUTDIR)\mdi.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

MFILE32=mfile.exe
# ADD BASE MFILE32 COPY /NOLOGO
# ADD MFILE32 COPY /NOLOGO
MFILE32_FLAGS=COPY /NOLOGO 
MFILE32_FILES= \
	"$(OUTDIR)/mdi.exe"

"$(OUTDIR)\mdi.trg" : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\PMacRel\mdi.exe\
 "$(MFILE32_DEST):mdi.exe">"$(OUTDIR)\mdi.trg"

DOWNLOAD : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\PMacRel\mdi.exe\
 "$(MFILE32_DEST):mdi.exe">"$(OUTDIR)\mdi.trg"

!ELSEIF  "$(CFG)" == "MDI - Power Macintosh Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "PMacDbg"
# PROP BASE Intermediate_Dir "PMacDbg"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "PMacDbg"
# PROP Intermediate_Dir "PMacDbg"
# PROP Target_Dir ""
OUTDIR=.\PMacDbg
INTDIR=.\PMacDbg

ALL :  "$(OUTDIR)\mdi.exe" "$(OUTDIR)\mdi.trg"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CLEAN : 
	-@erase ".\PMacDbg\vc40.pdb"
	-@erase ".\PMacDbg\mdi.pch"
	-@erase ".\PMacDbg\mdi.ilk"
	-@erase ".\PMacDbg\hello.obj"
	-@erase ".\PMacDbg\mdi.obj"
	-@erase ".\PMacDbg\bounce.obj"
	-@erase ".\PMacDbg\mainfrm.obj"
	-@erase ".\PMacDbg\stdafx.obj"
	-@erase ".\PMacDbg\mdi.rsc"
	-@erase ".\PMacDbg\mdimac.rsc"
	-@erase ".\PMacDbg\mdi.pdb"
	-@erase ".\PMacDbg\mdi.trg"
	-@erase ".\PMacDbg\mdi.exe"

MTL=mktyplib.exe
# ADD BASE MTL /nologo /D "_DEBUG"
# ADD MTL /nologo /D "_DEBUG"
MTL_PROJ=/nologo /D "_DEBUG" 
CPP=cl.exe
# ADD BASE CPP /nologo /W3 /GX /Zi /Od /D "_MAC" /D "_MPPC_" /D "_WINDOWS" /D "WIN32" /D "_DEBUG" /YX /c
# ADD CPP /nologo /MDd /W3 /GX /Zi /Od /D "_DEBUG" /D "_MAC" /D "_MPPC_" /D "_WINDOWS" /D "WIN32" /D "_MBCS" /D "_AFXDLL" /Yu"Stdafx.h" /c
# SUBTRACT CPP /Fr
CPP_PROJ=/nologo /MDd /W3 /GX /Zi /Od /D "_DEBUG" /D "_MAC" /D "_MPPC_" /D\
 "_WINDOWS" /D "WIN32" /D "_MBCS" /D "_AFXDLL" /Fp"$(INTDIR)/mdi.pch"\
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
RSC_PROJ=/l 0x409 /r /m /fo"$(INTDIR)/mdi.rsc" /d "_MAC" /d "_MPPC_" /d\
 "_DEBUG" 
MRC=mrc.exe
# ADD BASE MRC /D "_MPPC_" /D "_MAC" /D "_DEBUG" /NOLOGO
# ADD MRC /D "_MPPC_" /D "_MAC" /D "_DEBUG" /NOLOGO
MRC_PROJ=/D "_MPPC_" /D "_MAC" /D "_DEBUG" /l 0x409 /NOLOGO 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/mdi.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 cfrg.rsc /nologo /mac:bundle /debug /machine:MPPC
# ADD LINK32 /nologo /mac:bundle /mac:creator="MDIX" /debug /machine:MPPC
LINK32_FLAGS=/nologo /mac:bundle /mac:type="APPL" /mac:creator="MDIX"\
 /incremental:yes /pdb:"$(OUTDIR)/mdi.pdb" /debug /machine:MPPC\
 /out:"$(OUTDIR)/mdi.exe" 
LINK32_OBJS= \
	"$(INTDIR)/hello.obj" \
	"$(INTDIR)/mdi.obj" \
	"$(INTDIR)/bounce.obj" \
	"$(INTDIR)/mainfrm.obj" \
	"$(INTDIR)/stdafx.obj" \
	"$(INTDIR)/mdi.rsc" \
	"$(INTDIR)/mdimac.rsc"

"$(OUTDIR)\mdi.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

MFILE32=mfile.exe
# ADD BASE MFILE32 COPY /NOLOGO
# ADD MFILE32 COPY /NOLOGO
MFILE32_FLAGS=COPY /NOLOGO 
MFILE32_FILES= \
	"$(OUTDIR)/mdi.exe"

"$(OUTDIR)\mdi.trg" : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\PMacDbg\mdi.exe\
 "$(MFILE32_DEST):mdi.exe">"$(OUTDIR)\mdi.trg"

DOWNLOAD : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\PMacDbg\mdi.exe\
 "$(MFILE32_DEST):mdi.exe">"$(OUTDIR)\mdi.trg"

!ENDIF 

################################################################################
# Begin Target

# Name "MDI - Win32 Release"
# Name "MDI - Win32 Debug"
# Name "MDI - Macintosh Release"
# Name "MDI - Macintosh Debug"
# Name "MDI - Power Macintosh Release"
# Name "MDI - Power Macintosh Debug"

!IF  "$(CFG)" == "MDI - Win32 Release"

!ELSEIF  "$(CFG)" == "MDI - Win32 Debug"

!ELSEIF  "$(CFG)" == "MDI - Macintosh Release"

!ELSEIF  "$(CFG)" == "MDI - Macintosh Debug"

!ELSEIF  "$(CFG)" == "MDI - Power Macintosh Release"

!ELSEIF  "$(CFG)" == "MDI - Power Macintosh Debug"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\stdafx.cpp

!IF  "$(CFG)" == "MDI - Win32 Release"

DEP_CPP_STDAF=\
	".\stdafx.h"\
	
# ADD CPP /Yc"Stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/mdi.pch" /Yc"Stdafx.h" /Fo"$(INTDIR)/" /c\
 $(SOURCE) \
	

"$(INTDIR)\stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\mdi.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "MDI - Win32 Debug"

DEP_CPP_STDAF=\
	".\stdafx.h"\
	
# ADD CPP /Yc

BuildCmds= \
	$(CPP) /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/mdi.pch" /Yc /Fo"$(INTDIR)/"\
 /Fd"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\mdi.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "MDI - Macintosh Release"

"$(INTDIR)\stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\mdi.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "MDI - Macintosh Debug"

"$(INTDIR)\stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\mdi.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "MDI - Power Macintosh Release"

"$(INTDIR)\stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\mdi.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "MDI - Power Macintosh Debug"

DEP_CPP_STDAF=\
	".\stdafx.h"\
	
# ADD CPP /Yc
BuildCmds= \
	$(CPP) /nologo /MDd /W3 /GX /Zi /Od /D "_DEBUG" /D "_MAC" /D "_MPPC_" /D\
 "_WINDOWS" /D "WIN32" /D "_MBCS" /D "_AFXDLL" /Fp"$(INTDIR)/mdi.pch" /Yc\
 /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\mdi.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\hello.cpp

!IF  "$(CFG)" == "MDI - Win32 Release"

DEP_CPP_HELLO=\
	".\hello.h"\
	".\mdi.h"\
	".\stdafx.h"\
	

"$(INTDIR)\hello.obj" : $(SOURCE) $(DEP_CPP_HELLO) "$(INTDIR)"\
 "$(INTDIR)\mdi.pch"


!ELSEIF  "$(CFG)" == "MDI - Win32 Debug"

DEP_CPP_HELLO=\
	".\hello.h"\
	".\mdi.h"\
	".\stdafx.h"\
	

"$(INTDIR)\hello.obj" : $(SOURCE) $(DEP_CPP_HELLO) "$(INTDIR)"\
 "$(INTDIR)\mdi.pch"


!ELSEIF  "$(CFG)" == "MDI - Macintosh Release"

"$(INTDIR)\hello.obj" : $(SOURCE) $(DEP_CPP_HELLO) "$(INTDIR)"\
 "$(INTDIR)\mdi.pch"

!ELSEIF  "$(CFG)" == "MDI - Macintosh Debug"

"$(INTDIR)\hello.obj" : $(SOURCE) $(DEP_CPP_HELLO) "$(INTDIR)"\
 "$(INTDIR)\mdi.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_HELLO) "$(INTDIR)"\
 "$(INTDIR)\mdi.pch"

!ELSEIF  "$(CFG)" == "MDI - Power Macintosh Release"

"$(INTDIR)\hello.obj" : $(SOURCE) $(DEP_CPP_HELLO) "$(INTDIR)"\
 "$(INTDIR)\mdi.pch"

!ELSEIF  "$(CFG)" == "MDI - Power Macintosh Debug"

DEP_CPP_HELLO=\
	".\hello.h"\
	".\mdi.h"\
	".\stdafx.h"\
	

"$(INTDIR)\hello.obj" : $(SOURCE) $(DEP_CPP_HELLO) "$(INTDIR)"\
 "$(INTDIR)\mdi.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_HELLO) "$(INTDIR)"\
 "$(INTDIR)\mdi.pch"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\mainfrm.cpp

!IF  "$(CFG)" == "MDI - Win32 Release"

DEP_CPP_MAINF=\
	".\bounce.h"\
	".\hello.h"\
	".\mainfrm.h"\
	".\mdi.h"\
	".\stdafx.h"\
	

"$(INTDIR)\mainfrm.obj" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\mdi.pch"


!ELSEIF  "$(CFG)" == "MDI - Win32 Debug"

DEP_CPP_MAINF=\
	".\bounce.h"\
	".\hello.h"\
	".\mainfrm.h"\
	".\mdi.h"\
	".\stdafx.h"\
	

"$(INTDIR)\mainfrm.obj" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\mdi.pch"


!ELSEIF  "$(CFG)" == "MDI - Macintosh Release"

"$(INTDIR)\mainfrm.obj" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\mdi.pch"

!ELSEIF  "$(CFG)" == "MDI - Macintosh Debug"

"$(INTDIR)\mainfrm.obj" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\mdi.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\mdi.pch"

!ELSEIF  "$(CFG)" == "MDI - Power Macintosh Release"

"$(INTDIR)\mainfrm.obj" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\mdi.pch"

!ELSEIF  "$(CFG)" == "MDI - Power Macintosh Debug"

DEP_CPP_MAINF=\
	".\bounce.h"\
	".\hello.h"\
	".\mainfrm.h"\
	".\mdi.h"\
	".\stdafx.h"\
	

"$(INTDIR)\mainfrm.obj" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\mdi.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\mdi.pch"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\mdi.cpp

!IF  "$(CFG)" == "MDI - Win32 Release"

DEP_CPP_MDI_C=\
	".\mainfrm.h"\
	".\mdi.h"\
	".\stdafx.h"\
	

"$(INTDIR)\mdi.obj" : $(SOURCE) $(DEP_CPP_MDI_C) "$(INTDIR)"\
 "$(INTDIR)\mdi.pch"


!ELSEIF  "$(CFG)" == "MDI - Win32 Debug"

DEP_CPP_MDI_C=\
	".\mainfrm.h"\
	".\mdi.h"\
	".\stdafx.h"\
	

"$(INTDIR)\mdi.obj" : $(SOURCE) $(DEP_CPP_MDI_C) "$(INTDIR)"\
 "$(INTDIR)\mdi.pch"


!ELSEIF  "$(CFG)" == "MDI - Macintosh Release"

"$(INTDIR)\mdi.obj" : $(SOURCE) $(DEP_CPP_MDI_C) "$(INTDIR)"\
 "$(INTDIR)\mdi.pch"

!ELSEIF  "$(CFG)" == "MDI - Macintosh Debug"

"$(INTDIR)\mdi.obj" : $(SOURCE) $(DEP_CPP_MDI_C) "$(INTDIR)"\
 "$(INTDIR)\mdi.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_MDI_C) "$(INTDIR)"\
 "$(INTDIR)\mdi.pch"

!ELSEIF  "$(CFG)" == "MDI - Power Macintosh Release"

"$(INTDIR)\mdi.obj" : $(SOURCE) $(DEP_CPP_MDI_C) "$(INTDIR)"\
 "$(INTDIR)\mdi.pch"

!ELSEIF  "$(CFG)" == "MDI - Power Macintosh Debug"

DEP_CPP_MDI_C=\
	".\mainfrm.h"\
	".\mdi.h"\
	".\stdafx.h"\
	

"$(INTDIR)\mdi.obj" : $(SOURCE) $(DEP_CPP_MDI_C) "$(INTDIR)"\
 "$(INTDIR)\mdi.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_MDI_C) "$(INTDIR)"\
 "$(INTDIR)\mdi.pch"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\mdi.rc

!IF  "$(CFG)" == "MDI - Win32 Release"

DEP_RSC_MDI_R=\
	".\hello.ico"\
	".\mdi.ico"\
	

"$(INTDIR)\mdi.res" : $(SOURCE) $(DEP_RSC_MDI_R) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "MDI - Win32 Debug"

DEP_RSC_MDI_R=\
	".\hello.ico"\
	".\mdi.ico"\
	

"$(INTDIR)\mdi.res" : $(SOURCE) $(DEP_RSC_MDI_R) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "MDI - Macintosh Release"

"$(INTDIR)\mdi.rsc" : $(SOURCE) $(DEP_RSC_MDI_R) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "MDI - Macintosh Debug"

"$(INTDIR)\mdi.rsc" : $(SOURCE) $(DEP_RSC_MDI_R) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "MDI - Power Macintosh Release"

"$(INTDIR)\mdi.rsc" : $(SOURCE) $(DEP_RSC_MDI_R) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "MDI - Power Macintosh Debug"

DEP_RSC_MDI_R=\
	".\hello.ico"\
	".\mdi.ico"\
	

"$(INTDIR)\mdi.rsc" : $(SOURCE) $(DEP_RSC_MDI_R) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\bounce.cpp

!IF  "$(CFG)" == "MDI - Win32 Release"

DEP_CPP_BOUNC=\
	".\bounce.h"\
	".\mdi.h"\
	".\stdafx.h"\
	

"$(INTDIR)\bounce.obj" : $(SOURCE) $(DEP_CPP_BOUNC) "$(INTDIR)"\
 "$(INTDIR)\mdi.pch"


!ELSEIF  "$(CFG)" == "MDI - Win32 Debug"

DEP_CPP_BOUNC=\
	".\bounce.h"\
	".\mdi.h"\
	".\stdafx.h"\
	

"$(INTDIR)\bounce.obj" : $(SOURCE) $(DEP_CPP_BOUNC) "$(INTDIR)"\
 "$(INTDIR)\mdi.pch"


!ELSEIF  "$(CFG)" == "MDI - Macintosh Release"

DEP_CPP_BOUNC=\
	".\bounce.h"\
	".\mdi.h"\
	".\stdafx.h"\
	

"$(INTDIR)\bounce.obj" : $(SOURCE) $(DEP_CPP_BOUNC) "$(INTDIR)"\
 "$(INTDIR)\mdi.pch"

!ELSEIF  "$(CFG)" == "MDI - Macintosh Debug"

DEP_CPP_BOUNC=\
	".\bounce.h"\
	".\mdi.h"\
	".\stdafx.h"\
	

"$(INTDIR)\bounce.obj" : $(SOURCE) $(DEP_CPP_BOUNC) "$(INTDIR)"\
 "$(INTDIR)\mdi.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_BOUNC) "$(INTDIR)"\
 "$(INTDIR)\mdi.pch"

!ELSEIF  "$(CFG)" == "MDI - Power Macintosh Release"

DEP_CPP_BOUNC=\
	".\bounce.h"\
	".\mdi.h"\
	".\stdafx.h"\
	

"$(INTDIR)\bounce.obj" : $(SOURCE) $(DEP_CPP_BOUNC) "$(INTDIR)"\
 "$(INTDIR)\mdi.pch"

!ELSEIF  "$(CFG)" == "MDI - Power Macintosh Debug"

DEP_CPP_BOUNC=\
	".\bounce.h"\
	".\mdi.h"\
	".\stdafx.h"\
	

"$(INTDIR)\bounce.obj" : $(SOURCE) $(DEP_CPP_BOUNC) "$(INTDIR)"\
 "$(INTDIR)\mdi.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_BOUNC) "$(INTDIR)"\
 "$(INTDIR)\mdi.pch"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\mdimac.r

!IF  "$(CFG)" == "MDI - Win32 Release"

!ELSEIF  "$(CFG)" == "MDI - Win32 Debug"

!ELSEIF  "$(CFG)" == "MDI - Macintosh Release"

DEP_MRC_MDIMA=\
	".\mrc\balloons.r"\
	".\mrc\types.r"\
	

"$(INTDIR)\mdimac.rsc" : $(SOURCE) $(DEP_MRC_MDIMA) "$(INTDIR)"
   $(MRC) /o"$(INTDIR)/mdimac.rsc" /D "_68K_" /D "_MAC" /D "NDEBUG" /l 0x409\
 /NOLOGO $(SOURCE)


!ELSEIF  "$(CFG)" == "MDI - Macintosh Debug"

DEP_MRC_MDIMA=\
	".\mrc\balloons.r"\
	".\mrc\types.r"\
	

"$(INTDIR)\mdimac.rsc" : $(SOURCE) $(DEP_MRC_MDIMA) "$(INTDIR)"
   $(MRC) /o"$(INTDIR)/mdimac.rsc" /D "_68K_" /D "_MAC" /D "_DEBUG" /l 0x409\
 /NOLOGO $(SOURCE)


!ELSEIF  "$(CFG)" == "MDI - Power Macintosh Release"

DEP_MRC_MDIMA=\
	".\mrc\balloons.r"\
	".\mrc\types.r"\
	

"$(INTDIR)\mdimac.rsc" : $(SOURCE) $(DEP_MRC_MDIMA) "$(INTDIR)"
   $(MRC) /o"$(INTDIR)/mdimac.rsc" /D "_MPPC_" /D "_MAC" /D "NDEBUG" /l 0x409\
 /NOLOGO $(SOURCE)


!ELSEIF  "$(CFG)" == "MDI - Power Macintosh Debug"

DEP_MRC_MDIMA=\
	".\mrc\balloons.r"\
	".\mrc\types.r"\
	

"$(INTDIR)\mdimac.rsc" : $(SOURCE) $(DEP_MRC_MDIMA) "$(INTDIR)"
   $(MRC) /o"$(INTDIR)/mdimac.rsc" /D "_MPPC_" /D "_MAC" /D "_DEBUG" /l 0x409\
 /NOLOGO $(SOURCE)


!ENDIF 

# End Source File
# End Target
# End Project
################################################################################
