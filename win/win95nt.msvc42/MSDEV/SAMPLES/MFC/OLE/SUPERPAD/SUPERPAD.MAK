# Microsoft Visual C++ Generated NMAKE File, Format Version 40000
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101
# TARGTYPE "Power Macintosh Application" 0x0401
# TARGTYPE "Macintosh Application" 0x0301

!IF "$(CFG)" == ""
CFG=SuperPad - Win32 Debug
!MESSAGE No configuration specified.  Defaulting to SuperPad - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "SuperPad - Win32 Release" && "$(CFG)" !=\
 "SuperPad - Win32 Debug" && "$(CFG)" != "SuperPad - Macintosh Release" &&\
 "$(CFG)" != "SuperPad - Macintosh Debug" && "$(CFG)" !=\
 "SuperPad - Power Macintosh Release" && "$(CFG)" !=\
 "SuperPad - Power Macintosh Debug" && "$(CFG)" !=\
 "SuperPad - Win32 Unicode Debug" && "$(CFG)" !=\
 "SuperPad - Win32 Unicode Release"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "superpad.mak" CFG="SuperPad - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "SuperPad - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "SuperPad - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE "SuperPad - Macintosh Release" (based on "Macintosh Application")
!MESSAGE "SuperPad - Macintosh Debug" (based on "Macintosh Application")
!MESSAGE "SuperPad - Power Macintosh Release" (based on\
 "Power Macintosh Application")
!MESSAGE "SuperPad - Power Macintosh Debug" (based on\
 "Power Macintosh Application")
!MESSAGE "SuperPad - Win32 Unicode Debug" (based on "Win32 (x86) Application")
!MESSAGE "SuperPad - Win32 Unicode Release" (based on\
 "Win32 (x86) Application")
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
# PROP Target_Last_Scanned "SuperPad - Win32 Unicode Release"

!IF  "$(CFG)" == "SuperPad - Win32 Release"

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

ALL : "$(OUTDIR)\superpad.exe"

CLEAN : 
	-@erase ".\Release\superpad.exe"
	-@erase ".\Release\linkitem.obj"
	-@erase ".\Release\superpad.pch"
	-@erase ".\Release\tabstop.obj"
	-@erase ".\Release\pageset.obj"
	-@erase ".\Release\mainfrm.obj"
	-@erase ".\Release\padframe.obj"
	-@erase ".\Release\stdafx.obj"
	-@erase ".\Release\padview.obj"
	-@erase ".\Release\ipframe.obj"
	-@erase ".\Release\superpad.obj"
	-@erase ".\Release\aboutbox.obj"
	-@erase ".\Release\paddoc.obj"
	-@erase ".\Release\paditem.obj"
	-@erase ".\Release\superpad.res"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /FR /YX /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
CPP_PROJ=/nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/superpad.pch" /Yu"stdafx.h" /Fo"$(INTDIR)/"\
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
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/superpad.res" /d "NDEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/superpad.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 /nologo /subsystem:windows /machine:I386
LINK32_FLAGS=/nologo /subsystem:windows /incremental:no\
 /pdb:"$(OUTDIR)/superpad.pdb" /machine:I386 /out:"$(OUTDIR)/superpad.exe" 
LINK32_OBJS= \
	"$(INTDIR)/linkitem.obj" \
	"$(INTDIR)/tabstop.obj" \
	"$(INTDIR)/pageset.obj" \
	"$(INTDIR)/mainfrm.obj" \
	"$(INTDIR)/padframe.obj" \
	"$(INTDIR)/stdafx.obj" \
	"$(INTDIR)/padview.obj" \
	"$(INTDIR)/ipframe.obj" \
	"$(INTDIR)/superpad.obj" \
	"$(INTDIR)/aboutbox.obj" \
	"$(INTDIR)/paddoc.obj" \
	"$(INTDIR)/paditem.obj" \
	"$(INTDIR)/superpad.res"

"$(OUTDIR)\superpad.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "SuperPad - Win32 Debug"

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

ALL : "$(OUTDIR)\superpad.exe"

CLEAN : 
	-@erase ".\Debug\vc40.pdb"
	-@erase ".\Debug\superpad.pch"
	-@erase ".\Debug\superpad.exe"
	-@erase ".\Debug\linkitem.obj"
	-@erase ".\Debug\paddoc.obj"
	-@erase ".\Debug\padview.obj"
	-@erase ".\Debug\ipframe.obj"
	-@erase ".\Debug\paditem.obj"
	-@erase ".\Debug\padframe.obj"
	-@erase ".\Debug\tabstop.obj"
	-@erase ".\Debug\pageset.obj"
	-@erase ".\Debug\superpad.obj"
	-@erase ".\Debug\mainfrm.obj"
	-@erase ".\Debug\aboutbox.obj"
	-@erase ".\Debug\stdafx.obj"
	-@erase ".\Debug\superpad.res"
	-@erase ".\Debug\superpad.ilk"
	-@erase ".\Debug\superpad.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /FR /YX /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
CPP_PROJ=/nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/superpad.pch" /Yu"stdafx.h"\
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
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/superpad.res" /d "_DEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/superpad.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386
# ADD LINK32 /nologo /subsystem:windows /debug /machine:I386
LINK32_FLAGS=/nologo /subsystem:windows /incremental:yes\
 /pdb:"$(OUTDIR)/superpad.pdb" /debug /machine:I386\
 /out:"$(OUTDIR)/superpad.exe" 
LINK32_OBJS= \
	"$(INTDIR)/linkitem.obj" \
	"$(INTDIR)/paddoc.obj" \
	"$(INTDIR)/padview.obj" \
	"$(INTDIR)/ipframe.obj" \
	"$(INTDIR)/paditem.obj" \
	"$(INTDIR)/padframe.obj" \
	"$(INTDIR)/tabstop.obj" \
	"$(INTDIR)/pageset.obj" \
	"$(INTDIR)/superpad.obj" \
	"$(INTDIR)/mainfrm.obj" \
	"$(INTDIR)/aboutbox.obj" \
	"$(INTDIR)/stdafx.obj" \
	"$(INTDIR)/superpad.res"

"$(OUTDIR)\superpad.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "SuperPad - Macintosh Release"

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

ALL : "$(OUTDIR)\superpad.exe" "$(OUTDIR)\superpad.trg"

CLEAN : 
	-@erase ".\MacRel\superpad.trg"
	-@erase ".\MacRel\superpad.exe"
	-@erase ".\MacRel\ipframe.obj"
	-@erase ".\MacRel\superpad.pch"
	-@erase ".\MacRel\padview.obj"
	-@erase ".\MacRel\padframe.obj"
	-@erase ".\MacRel\paditem.obj"
	-@erase ".\MacRel\paddoc.obj"
	-@erase ".\MacRel\superpad.obj"
	-@erase ".\MacRel\aboutbox.obj"
	-@erase ".\MacRel\tabstop.obj"
	-@erase ".\MacRel\pageset.obj"
	-@erase ".\MacRel\mainfrm.obj"
	-@erase ".\MacRel\linkitem.obj"
	-@erase ".\MacRel\stdafx.obj"
	-@erase ".\MacRel\superpad.rsc"
	-@erase ".\MacRel\spadmac.rsc"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

MTL=mktyplib.exe
# ADD BASE MTL /nologo /D "NDEBUG" /mac
# ADD MTL /nologo /D "NDEBUG" /mac
MTL_PROJ=/nologo /D "NDEBUG" /mac 
CPP=cl.exe
# ADD BASE CPP /nologo /AL /Q68s /W3 /GX /O2 /D "_WINDOWS" /D "_MAC" /D "_68K_" /D "WIN32" /D "NDEBUG" /FR /YX /c
# ADD CPP /nologo /AL /Gt1 /Q68s /W3 /GX /O2 /D "_WINDOWS" /D "_MAC" /D "_68K_" /D "WIN32" /D "NDEBUG" /D "_MBCS" /Yu"stdafx.h" /c
CPP_PROJ=/nologo /AL /Gt1 /Q68s /W3 /GX /O2 /D "_WINDOWS" /D "_MAC" /D "_68K_"\
 /D "WIN32" /D "NDEBUG" /D "_MBCS" /Fp"$(INTDIR)/superpad.pch" /Yu"stdafx.h"\
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
RSC_PROJ=/l 0x409 /r /m /fo"$(INTDIR)/superpad.rsc" /d "_MAC" /d "_68K_" /d\
 "NDEBUG" 
MRC=mrc.exe
# ADD BASE MRC /D "_68K_" /D "_MAC" /D "NDEBUG" /NOLOGO
# ADD MRC /D "_68K_" /D "_MAC" /D "NDEBUG" /NOLOGO
MRC_PROJ=/D "_68K_" /D "_MAC" /D "NDEBUG" /l 0x409 /NOLOGO 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/superpad.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 wlm.lib /nologo /MAC:bundle /machine:M68K
# ADD LINK32 /nologo /MAC:bundle /MAC:creator="SPAD" /machine:M68K
LINK32_FLAGS=/nologo /MAC:bundle /MAC:type="APPL" /MAC:creator="SPAD"\
 /pdb:"$(OUTDIR)/superpad.pdb" /machine:M68K /out:"$(OUTDIR)/superpad.exe" 
LINK32_OBJS= \
	"$(INTDIR)/ipframe.obj" \
	"$(INTDIR)/padview.obj" \
	"$(INTDIR)/padframe.obj" \
	"$(INTDIR)/paditem.obj" \
	"$(INTDIR)/paddoc.obj" \
	"$(INTDIR)/superpad.obj" \
	"$(INTDIR)/aboutbox.obj" \
	"$(INTDIR)/tabstop.obj" \
	"$(INTDIR)/pageset.obj" \
	"$(INTDIR)/mainfrm.obj" \
	"$(INTDIR)/linkitem.obj" \
	"$(INTDIR)/stdafx.obj" \
	"$(INTDIR)/superpad.rsc" \
	"$(INTDIR)/spadmac.rsc"

"$(OUTDIR)\superpad.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

MFILE32=mfile.exe
# ADD BASE MFILE32 COPY /NOLOGO
# ADD MFILE32 COPY /NOLOGO
MFILE32_FLAGS=COPY /NOLOGO 
MFILE32_FILES= \
	"$(OUTDIR)/superpad.exe"

"$(OUTDIR)\superpad.trg" : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\MacRel\superpad.exe\
 "$(MFILE32_DEST):superpad.exe">"$(OUTDIR)\superpad.trg"

DOWNLOAD : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\MacRel\superpad.exe\
 "$(MFILE32_DEST):superpad.exe">"$(OUTDIR)\superpad.trg"

!ELSEIF  "$(CFG)" == "SuperPad - Macintosh Debug"

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

ALL : "$(OUTDIR)\superpad.exe" "$(OUTDIR)\superpad.trg"

CLEAN : 
	-@erase ".\MacDbg\vc40.pdb"
	-@erase ".\MacDbg\superpad.pch"
	-@erase ".\MacDbg\superpad.pdb"
	-@erase ".\MacDbg\mainfrm.obj"
	-@erase ".\MacDbg\padframe.obj"
	-@erase ".\MacDbg\paddoc.obj"
	-@erase ".\MacDbg\ipframe.obj"
	-@erase ".\MacDbg\padview.obj"
	-@erase ".\MacDbg\superpad.obj"
	-@erase ".\MacDbg\aboutbox.obj"
	-@erase ".\MacDbg\paditem.obj"
	-@erase ".\MacDbg\linkitem.obj"
	-@erase ".\MacDbg\tabstop.obj"
	-@erase ".\MacDbg\stdafx.obj"
	-@erase ".\MacDbg\pageset.obj"
	-@erase ".\MacDbg\spadmac.rsc"
	-@erase ".\MacDbg\superpad.rsc"
	-@erase ".\MacDbg\superpad.trg"
	-@erase ".\MacDbg\superpad.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

MTL=mktyplib.exe
# ADD BASE MTL /nologo /D "_DEBUG" /mac
# ADD MTL /nologo /D "_DEBUG" /mac
MTL_PROJ=/nologo /D "_DEBUG" /mac 
CPP=cl.exe
# ADD BASE CPP /nologo /AL /Q68s /Q68m /W3 /GX /Zi /Od /D "_WINDOWS" /D "_MAC" /D "_68K_" /D "WIN32" /D "_DEBUG" /FR /YX /c
# ADD CPP /nologo /AL /Gt1 /Q68s /Q68m /W3 /GX /Zi /Od /D "_WINDOWS" /D "_MAC" /D "_68K_" /D "WIN32" /D "_DEBUG" /D "_MBCS" /Yu"stdafx.h" /c
CPP_PROJ=/nologo /AL /Gt1 /Q68s /Q68m /W3 /GX /Zi /Od /D "_WINDOWS" /D "_MAC"\
 /D "_68K_" /D "WIN32" /D "_DEBUG" /D "_MBCS" /Fp"$(INTDIR)/superpad.pch"\
 /Yu"stdafx.h" /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
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
RSC_PROJ=/l 0x409 /r /m /fo"$(INTDIR)/superpad.rsc" /d "_MAC" /d "_68K_" /d\
 "_DEBUG" 
MRC=mrc.exe
# ADD BASE MRC /D "_68K_" /D "_MAC" /D "_DEBUG" /NOLOGO
# ADD MRC /D "_68K_" /D "_MAC" /D "_DEBUG" /NOLOGO
MRC_PROJ=/D "_68K_" /D "_MAC" /D "_DEBUG" /l 0x409 /NOLOGO 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/superpad.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 wlm.lib /nologo /MAC:bundle /debug /machine:M68K
# ADD LINK32 /nologo /MAC:bundle /MAC:creator="SPAD" /debug /machine:M68K
LINK32_FLAGS=/nologo /MAC:bundle /MAC:type="APPL" /MAC:creator="SPAD"\
 /pdb:"$(OUTDIR)/superpad.pdb" /debug /machine:M68K\
 /out:"$(OUTDIR)/superpad.exe" 
LINK32_OBJS= \
	"$(INTDIR)/mainfrm.obj" \
	"$(INTDIR)/padframe.obj" \
	"$(INTDIR)/paddoc.obj" \
	"$(INTDIR)/ipframe.obj" \
	"$(INTDIR)/padview.obj" \
	"$(INTDIR)/superpad.obj" \
	"$(INTDIR)/aboutbox.obj" \
	"$(INTDIR)/paditem.obj" \
	"$(INTDIR)/linkitem.obj" \
	"$(INTDIR)/tabstop.obj" \
	"$(INTDIR)/stdafx.obj" \
	"$(INTDIR)/pageset.obj" \
	"$(INTDIR)/spadmac.rsc" \
	"$(INTDIR)/superpad.rsc"

"$(OUTDIR)\superpad.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

MFILE32=mfile.exe
# ADD BASE MFILE32 COPY /NOLOGO
# ADD MFILE32 COPY /NOLOGO
MFILE32_FLAGS=COPY /NOLOGO 
MFILE32_FILES= \
	"$(OUTDIR)/superpad.exe"

"$(OUTDIR)\superpad.trg" : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\MacDbg\superpad.exe\
 "$(MFILE32_DEST):superpad.exe">"$(OUTDIR)\superpad.trg"

DOWNLOAD : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\MacDbg\superpad.exe\
 "$(MFILE32_DEST):superpad.exe">"$(OUTDIR)\superpad.trg"

!ELSEIF  "$(CFG)" == "SuperPad - Power Macintosh Release"

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

ALL : "$(OUTDIR)\superpad.exe" "$(OUTDIR)\superpad.trg"

CLEAN : 
	-@erase ".\PMacRel\superpad.trg"
	-@erase ".\PMacRel\superpad.exe"
	-@erase ".\PMacRel\mainfrm.obj"
	-@erase ".\PMacRel\superpad.pch"
	-@erase ".\PMacRel\superpad.obj"
	-@erase ".\PMacRel\aboutbox.obj"
	-@erase ".\PMacRel\paddoc.obj"
	-@erase ".\PMacRel\ipframe.obj"
	-@erase ".\PMacRel\padview.obj"
	-@erase ".\PMacRel\linkitem.obj"
	-@erase ".\PMacRel\paditem.obj"
	-@erase ".\PMacRel\padframe.obj"
	-@erase ".\PMacRel\tabstop.obj"
	-@erase ".\PMacRel\stdafx.obj"
	-@erase ".\PMacRel\pageset.obj"
	-@erase ".\PMacRel\spadmac.rsc"
	-@erase ".\PMacRel\superpad.rsc"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

MTL=mktyplib.exe
# ADD BASE MTL /nologo /D "NDEBUG"
# ADD MTL /nologo /D "NDEBUG"
MTL_PROJ=/nologo /D "NDEBUG" 
CPP=cl.exe
# ADD BASE CPP /nologo /W3 /GX /O2 /D "_MAC" /D "_MPPC_" /D "_WINDOWS" /D "WIN32" /D "NDEBUG" /FR /YX /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "_MAC" /D "_MPPC_" /D "_WINDOWS" /D "WIN32" /D "_MBCS" /D "_AFXDLL" /Yu"stdafx.h" /c
CPP_PROJ=/nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "_MAC" /D "_MPPC_" /D\
 "_WINDOWS" /D "WIN32" /D "_MBCS" /D "_AFXDLL" /Fp"$(INTDIR)/superpad.pch"\
 /Yu"stdafx.h" /Fo"$(INTDIR)/" /c 
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
RSC_PROJ=/l 0x409 /r /m /fo"$(INTDIR)/superpad.rsc" /d "_MAC" /d "_MPPC_" /d\
 "NDEBUG" 
MRC=mrc.exe
# ADD BASE MRC /D "_MPPC_" /D "_MAC" /D "NDEBUG" /NOLOGO
# ADD MRC /D "_MPPC_" /D "_MAC" /D "NDEBUG" /NOLOGO
MRC_PROJ=/D "_MPPC_" /D "_MAC" /D "NDEBUG" /l 0x409 /NOLOGO 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/superpad.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 cfrg.rsc /nologo /mac:bundle /machine:MPPC
# ADD LINK32 /nologo /mac:bundle /mac:creator="SPAD" /machine:MPPC
LINK32_FLAGS=/nologo /mac:bundle /mac:type="APPL" /mac:creator="SPAD"\
 /incremental:no /pdb:"$(OUTDIR)/superpad.pdb" /machine:MPPC\
 /out:"$(OUTDIR)/superpad.exe" 
LINK32_OBJS= \
	"$(INTDIR)/mainfrm.obj" \
	"$(INTDIR)/superpad.obj" \
	"$(INTDIR)/aboutbox.obj" \
	"$(INTDIR)/paddoc.obj" \
	"$(INTDIR)/ipframe.obj" \
	"$(INTDIR)/padview.obj" \
	"$(INTDIR)/linkitem.obj" \
	"$(INTDIR)/paditem.obj" \
	"$(INTDIR)/padframe.obj" \
	"$(INTDIR)/tabstop.obj" \
	"$(INTDIR)/stdafx.obj" \
	"$(INTDIR)/pageset.obj" \
	"$(INTDIR)/spadmac.rsc" \
	"$(INTDIR)/superpad.rsc"

"$(OUTDIR)\superpad.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

MFILE32=mfile.exe
# ADD BASE MFILE32 COPY /NOLOGO
# ADD MFILE32 COPY /NOLOGO
MFILE32_FLAGS=COPY /NOLOGO 
MFILE32_FILES= \
	"$(OUTDIR)/superpad.exe"

"$(OUTDIR)\superpad.trg" : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\PMacRel\superpad.exe\
 "$(MFILE32_DEST):superpad.exe">"$(OUTDIR)\superpad.trg"

DOWNLOAD : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\PMacRel\superpad.exe\
 "$(MFILE32_DEST):superpad.exe">"$(OUTDIR)\superpad.trg"

!ELSEIF  "$(CFG)" == "SuperPad - Power Macintosh Debug"

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

ALL : "$(OUTDIR)\superpad.exe" "$(OUTDIR)\superpad.trg"

CLEAN : 
	-@erase ".\PMacDbg\vc40.pdb"
	-@erase ".\PMacDbg\superpad.pch"
	-@erase ".\PMacDbg\superpad.ilk"
	-@erase ".\PMacDbg\tabstop.obj"
	-@erase ".\PMacDbg\superpad.obj"
	-@erase ".\PMacDbg\aboutbox.obj"
	-@erase ".\PMacDbg\pageset.obj"
	-@erase ".\PMacDbg\paddoc.obj"
	-@erase ".\PMacDbg\mainfrm.obj"
	-@erase ".\PMacDbg\linkitem.obj"
	-@erase ".\PMacDbg\ipframe.obj"
	-@erase ".\PMacDbg\padview.obj"
	-@erase ".\PMacDbg\padframe.obj"
	-@erase ".\PMacDbg\stdafx.obj"
	-@erase ".\PMacDbg\paditem.obj"
	-@erase ".\PMacDbg\spadmac.rsc"
	-@erase ".\PMacDbg\superpad.rsc"
	-@erase ".\PMacDbg\superpad.pdb"
	-@erase ".\PMacDbg\superpad.trg"
	-@erase ".\PMacDbg\superpad.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

MTL=mktyplib.exe
# ADD BASE MTL /nologo /D "_DEBUG"
# ADD MTL /nologo /D "_DEBUG"
MTL_PROJ=/nologo /D "_DEBUG" 
CPP=cl.exe
# ADD BASE CPP /nologo /W3 /GX /Zi /Od /D "_MAC" /D "_MPPC_" /D "_WINDOWS" /D "WIN32" /D "_DEBUG" /FR /YX /c
# ADD CPP /nologo /MDd /W3 /GX /Zi /Od /D "_DEBUG" /D "_MAC" /D "_MPPC_" /D "_WINDOWS" /D "WIN32" /D "_MBCS" /D "_AFXDLL" /Yu"stdafx.h" /c
CPP_PROJ=/nologo /MDd /W3 /GX /Zi /Od /D "_DEBUG" /D "_MAC" /D "_MPPC_" /D\
 "_WINDOWS" /D "WIN32" /D "_MBCS" /D "_AFXDLL" /Fp"$(INTDIR)/superpad.pch"\
 /Yu"stdafx.h" /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
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
RSC_PROJ=/l 0x409 /r /m /fo"$(INTDIR)/superpad.rsc" /d "_MAC" /d "_MPPC_" /d\
 "_DEBUG" 
MRC=mrc.exe
# ADD BASE MRC /D "_MPPC_" /D "_MAC" /D "_DEBUG" /NOLOGO
# ADD MRC /D "_MPPC_" /D "_MAC" /D "_DEBUG" /NOLOGO
MRC_PROJ=/D "_MPPC_" /D "_MAC" /D "_DEBUG" /l 0x409 /NOLOGO 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/superpad.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 cfrg.rsc /nologo /mac:bundle /debug /machine:MPPC
# ADD LINK32 /nologo /mac:bundle /mac:creator="SPAD" /debug /machine:MPPC
LINK32_FLAGS=/nologo /mac:bundle /mac:type="APPL" /mac:creator="SPAD"\
 /incremental:yes /pdb:"$(OUTDIR)/superpad.pdb" /debug /machine:MPPC\
 /out:"$(OUTDIR)/superpad.exe" 
LINK32_OBJS= \
	"$(INTDIR)/tabstop.obj" \
	"$(INTDIR)/superpad.obj" \
	"$(INTDIR)/aboutbox.obj" \
	"$(INTDIR)/pageset.obj" \
	"$(INTDIR)/paddoc.obj" \
	"$(INTDIR)/mainfrm.obj" \
	"$(INTDIR)/linkitem.obj" \
	"$(INTDIR)/ipframe.obj" \
	"$(INTDIR)/padview.obj" \
	"$(INTDIR)/padframe.obj" \
	"$(INTDIR)/stdafx.obj" \
	"$(INTDIR)/paditem.obj" \
	"$(INTDIR)/spadmac.rsc" \
	"$(INTDIR)/superpad.rsc"

"$(OUTDIR)\superpad.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

MFILE32=mfile.exe
# ADD BASE MFILE32 COPY /NOLOGO
# ADD MFILE32 COPY /NOLOGO
MFILE32_FLAGS=COPY /NOLOGO 
MFILE32_FILES= \
	"$(OUTDIR)/superpad.exe"

"$(OUTDIR)\superpad.trg" : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\PMacDbg\superpad.exe\
 "$(MFILE32_DEST):superpad.exe">"$(OUTDIR)\superpad.trg"

DOWNLOAD : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\PMacDbg\superpad.exe\
 "$(MFILE32_DEST):superpad.exe">"$(OUTDIR)\superpad.trg"

!ELSEIF  "$(CFG)" == "SuperPad - Win32 Unicode Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir Debug
# PROP BASE Intermediate_Dir Debug
# PROP BASE Target_Dir
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 1
# PROP Output_Dir UniDebug
# PROP Intermediate_Dir UniDebug
# PROP Target_Dir
OUTDIR=.\UniDebug
INTDIR=.\UniDebug

ALL : "$(OUTDIR)\superpad.exe"

CLEAN : 
	-@erase ".\UniDebug\vc40.pdb"
	-@erase ".\UniDebug\superpad.pch"
	-@erase ".\UniDebug\superpad.exe"
	-@erase ".\UniDebug\mainfrm.obj"
	-@erase ".\UniDebug\padframe.obj"
	-@erase ".\UniDebug\paddoc.obj"
	-@erase ".\UniDebug\padview.obj"
	-@erase ".\UniDebug\ipframe.obj"
	-@erase ".\UniDebug\superpad.obj"
	-@erase ".\UniDebug\aboutbox.obj"
	-@erase ".\UniDebug\paditem.obj"
	-@erase ".\UniDebug\linkitem.obj"
	-@erase ".\UniDebug\tabstop.obj"
	-@erase ".\UniDebug\stdafx.obj"
	-@erase ".\UniDebug\pageset.obj"
	-@erase ".\UniDebug\superpad.res"
	-@erase ".\UniDebug\superpad.ilk"
	-@erase ".\UniDebug\superpad.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /FR /YX /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_UNICODE" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
CPP_PROJ=/nologo /MDd /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS"\
 /D "_UNICODE" /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/superpad.pch" /Yu"stdafx.h"\
 /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\UniDebug/
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
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/superpad.res" /d "_DEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/superpad.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386
# ADD LINK32 /nologo /entry:"wWinMainCRTStartup" /subsystem:windows /debug /machine:I386
LINK32_FLAGS=/nologo /entry:"wWinMainCRTStartup" /subsystem:windows\
 /incremental:yes /pdb:"$(OUTDIR)/superpad.pdb" /debug /machine:I386\
 /out:"$(OUTDIR)/superpad.exe" 
LINK32_OBJS= \
	"$(INTDIR)/mainfrm.obj" \
	"$(INTDIR)/padframe.obj" \
	"$(INTDIR)/paddoc.obj" \
	"$(INTDIR)/padview.obj" \
	"$(INTDIR)/ipframe.obj" \
	"$(INTDIR)/superpad.obj" \
	"$(INTDIR)/aboutbox.obj" \
	"$(INTDIR)/paditem.obj" \
	"$(INTDIR)/linkitem.obj" \
	"$(INTDIR)/tabstop.obj" \
	"$(INTDIR)/stdafx.obj" \
	"$(INTDIR)/pageset.obj" \
	"$(INTDIR)/superpad.res"

"$(OUTDIR)\superpad.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "SuperPad - Win32 Unicode Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir Release
# PROP BASE Intermediate_Dir Release
# PROP BASE Target_Dir
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 0
# PROP Output_Dir UniRelease
# PROP Intermediate_Dir UniRelease
# PROP Target_Dir
OUTDIR=.\UniRelease
INTDIR=.\UniRelease

ALL : "$(OUTDIR)\superpad.exe"

CLEAN : 
	-@erase ".\UniRelease\superpad.exe"
	-@erase ".\UniRelease\ipframe.obj"
	-@erase ".\UniRelease\superpad.pch"
	-@erase ".\UniRelease\padview.obj"
	-@erase ".\UniRelease\linkitem.obj"
	-@erase ".\UniRelease\paditem.obj"
	-@erase ".\UniRelease\paddoc.obj"
	-@erase ".\UniRelease\padframe.obj"
	-@erase ".\UniRelease\tabstop.obj"
	-@erase ".\UniRelease\pageset.obj"
	-@erase ".\UniRelease\mainfrm.obj"
	-@erase ".\UniRelease\superpad.obj"
	-@erase ".\UniRelease\aboutbox.obj"
	-@erase ".\UniRelease\stdafx.obj"
	-@erase ".\UniRelease\superpad.res"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /FR /YX /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_UNICODE" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
CPP_PROJ=/nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D\
 "_UNICODE" /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/superpad.pch" /Yu"stdafx.h"\
 /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\UniRelease/
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
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/superpad.res" /d "NDEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/superpad.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 /nologo /entry:"wWinMainCRTStartup" /subsystem:windows /machine:I386
LINK32_FLAGS=/nologo /entry:"wWinMainCRTStartup" /subsystem:windows\
 /incremental:no /pdb:"$(OUTDIR)/superpad.pdb" /machine:I386\
 /out:"$(OUTDIR)/superpad.exe" 
LINK32_OBJS= \
	"$(INTDIR)/ipframe.obj" \
	"$(INTDIR)/padview.obj" \
	"$(INTDIR)/linkitem.obj" \
	"$(INTDIR)/paditem.obj" \
	"$(INTDIR)/paddoc.obj" \
	"$(INTDIR)/padframe.obj" \
	"$(INTDIR)/tabstop.obj" \
	"$(INTDIR)/pageset.obj" \
	"$(INTDIR)/mainfrm.obj" \
	"$(INTDIR)/superpad.obj" \
	"$(INTDIR)/aboutbox.obj" \
	"$(INTDIR)/stdafx.obj" \
	"$(INTDIR)/superpad.res"

"$(OUTDIR)\superpad.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 

################################################################################
# Begin Target

# Name "SuperPad - Win32 Release"
# Name "SuperPad - Win32 Debug"
# Name "SuperPad - Macintosh Release"
# Name "SuperPad - Macintosh Debug"
# Name "SuperPad - Power Macintosh Release"
# Name "SuperPad - Power Macintosh Debug"
# Name "SuperPad - Win32 Unicode Debug"
# Name "SuperPad - Win32 Unicode Release"

!IF  "$(CFG)" == "SuperPad - Win32 Release"

!ELSEIF  "$(CFG)" == "SuperPad - Win32 Debug"

!ELSEIF  "$(CFG)" == "SuperPad - Macintosh Release"

!ELSEIF  "$(CFG)" == "SuperPad - Macintosh Debug"

!ELSEIF  "$(CFG)" == "SuperPad - Power Macintosh Release"

!ELSEIF  "$(CFG)" == "SuperPad - Power Macintosh Debug"

!ELSEIF  "$(CFG)" == "SuperPad - Win32 Unicode Debug"

!ELSEIF  "$(CFG)" == "SuperPad - Win32 Unicode Release"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\aboutbox.cpp
DEP_CPP_ABOUT=\
	".\aboutbox.h"\
	".\stdafx.h"\
	

!IF  "$(CFG)" == "SuperPad - Win32 Release"


"$(INTDIR)\aboutbox.obj" : $(SOURCE) $(DEP_CPP_ABOUT) "$(INTDIR)"\
 "$(INTDIR)\superpad.pch"


!ELSEIF  "$(CFG)" == "SuperPad - Win32 Debug"


"$(INTDIR)\aboutbox.obj" : $(SOURCE) $(DEP_CPP_ABOUT) "$(INTDIR)"\
 "$(INTDIR)\superpad.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_ABOUT) "$(INTDIR)"\
 "$(INTDIR)\superpad.pch"


!ELSEIF  "$(CFG)" == "SuperPad - Macintosh Release"


"$(INTDIR)\aboutbox.obj" : $(SOURCE) $(DEP_CPP_ABOUT) "$(INTDIR)"\
 "$(INTDIR)\superpad.pch"


!ELSEIF  "$(CFG)" == "SuperPad - Macintosh Debug"


"$(INTDIR)\aboutbox.obj" : $(SOURCE) $(DEP_CPP_ABOUT) "$(INTDIR)"\
 "$(INTDIR)\superpad.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_ABOUT) "$(INTDIR)"\
 "$(INTDIR)\superpad.pch"


!ELSEIF  "$(CFG)" == "SuperPad - Power Macintosh Release"


"$(INTDIR)\aboutbox.obj" : $(SOURCE) $(DEP_CPP_ABOUT) "$(INTDIR)"\
 "$(INTDIR)\superpad.pch"


!ELSEIF  "$(CFG)" == "SuperPad - Power Macintosh Debug"


"$(INTDIR)\aboutbox.obj" : $(SOURCE) $(DEP_CPP_ABOUT) "$(INTDIR)"\
 "$(INTDIR)\superpad.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_ABOUT) "$(INTDIR)"\
 "$(INTDIR)\superpad.pch"


!ELSEIF  "$(CFG)" == "SuperPad - Win32 Unicode Debug"


"$(INTDIR)\aboutbox.obj" : $(SOURCE) $(DEP_CPP_ABOUT) "$(INTDIR)"\
 "$(INTDIR)\superpad.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_ABOUT) "$(INTDIR)"\
 "$(INTDIR)\superpad.pch"


!ELSEIF  "$(CFG)" == "SuperPad - Win32 Unicode Release"


"$(INTDIR)\aboutbox.obj" : $(SOURCE) $(DEP_CPP_ABOUT) "$(INTDIR)"\
 "$(INTDIR)\superpad.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\ipframe.cpp
DEP_CPP_IPFRA=\
	".\ipframe.h"\
	".\stdafx.h"\
	".\superpad.h"\
	".\aboutbox.h"\
	".\pageset.h"\
	

!IF  "$(CFG)" == "SuperPad - Win32 Release"


"$(INTDIR)\ipframe.obj" : $(SOURCE) $(DEP_CPP_IPFRA) "$(INTDIR)"\
 "$(INTDIR)\superpad.pch"


!ELSEIF  "$(CFG)" == "SuperPad - Win32 Debug"


"$(INTDIR)\ipframe.obj" : $(SOURCE) $(DEP_CPP_IPFRA) "$(INTDIR)"\
 "$(INTDIR)\superpad.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_IPFRA) "$(INTDIR)"\
 "$(INTDIR)\superpad.pch"


!ELSEIF  "$(CFG)" == "SuperPad - Macintosh Release"


"$(INTDIR)\ipframe.obj" : $(SOURCE) $(DEP_CPP_IPFRA) "$(INTDIR)"\
 "$(INTDIR)\superpad.pch"


!ELSEIF  "$(CFG)" == "SuperPad - Macintosh Debug"


"$(INTDIR)\ipframe.obj" : $(SOURCE) $(DEP_CPP_IPFRA) "$(INTDIR)"\
 "$(INTDIR)\superpad.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_IPFRA) "$(INTDIR)"\
 "$(INTDIR)\superpad.pch"


!ELSEIF  "$(CFG)" == "SuperPad - Power Macintosh Release"


"$(INTDIR)\ipframe.obj" : $(SOURCE) $(DEP_CPP_IPFRA) "$(INTDIR)"\
 "$(INTDIR)\superpad.pch"


!ELSEIF  "$(CFG)" == "SuperPad - Power Macintosh Debug"


"$(INTDIR)\ipframe.obj" : $(SOURCE) $(DEP_CPP_IPFRA) "$(INTDIR)"\
 "$(INTDIR)\superpad.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_IPFRA) "$(INTDIR)"\
 "$(INTDIR)\superpad.pch"


!ELSEIF  "$(CFG)" == "SuperPad - Win32 Unicode Debug"


"$(INTDIR)\ipframe.obj" : $(SOURCE) $(DEP_CPP_IPFRA) "$(INTDIR)"\
 "$(INTDIR)\superpad.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_IPFRA) "$(INTDIR)"\
 "$(INTDIR)\superpad.pch"


!ELSEIF  "$(CFG)" == "SuperPad - Win32 Unicode Release"


"$(INTDIR)\ipframe.obj" : $(SOURCE) $(DEP_CPP_IPFRA) "$(INTDIR)"\
 "$(INTDIR)\superpad.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\linkitem.cpp
DEP_CPP_LINKI=\
	".\linkitem.h"\
	".\paddoc.h"\
	".\paditem.h"\
	".\padview.h"\
	".\stdafx.h"\
	

!IF  "$(CFG)" == "SuperPad - Win32 Release"


"$(INTDIR)\linkitem.obj" : $(SOURCE) $(DEP_CPP_LINKI) "$(INTDIR)"\
 "$(INTDIR)\superpad.pch"


!ELSEIF  "$(CFG)" == "SuperPad - Win32 Debug"


"$(INTDIR)\linkitem.obj" : $(SOURCE) $(DEP_CPP_LINKI) "$(INTDIR)"\
 "$(INTDIR)\superpad.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_LINKI) "$(INTDIR)"\
 "$(INTDIR)\superpad.pch"


!ELSEIF  "$(CFG)" == "SuperPad - Macintosh Release"


"$(INTDIR)\linkitem.obj" : $(SOURCE) $(DEP_CPP_LINKI) "$(INTDIR)"\
 "$(INTDIR)\superpad.pch"


!ELSEIF  "$(CFG)" == "SuperPad - Macintosh Debug"


"$(INTDIR)\linkitem.obj" : $(SOURCE) $(DEP_CPP_LINKI) "$(INTDIR)"\
 "$(INTDIR)\superpad.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_LINKI) "$(INTDIR)"\
 "$(INTDIR)\superpad.pch"


!ELSEIF  "$(CFG)" == "SuperPad - Power Macintosh Release"


"$(INTDIR)\linkitem.obj" : $(SOURCE) $(DEP_CPP_LINKI) "$(INTDIR)"\
 "$(INTDIR)\superpad.pch"


!ELSEIF  "$(CFG)" == "SuperPad - Power Macintosh Debug"


"$(INTDIR)\linkitem.obj" : $(SOURCE) $(DEP_CPP_LINKI) "$(INTDIR)"\
 "$(INTDIR)\superpad.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_LINKI) "$(INTDIR)"\
 "$(INTDIR)\superpad.pch"


!ELSEIF  "$(CFG)" == "SuperPad - Win32 Unicode Debug"


"$(INTDIR)\linkitem.obj" : $(SOURCE) $(DEP_CPP_LINKI) "$(INTDIR)"\
 "$(INTDIR)\superpad.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_LINKI) "$(INTDIR)"\
 "$(INTDIR)\superpad.pch"


!ELSEIF  "$(CFG)" == "SuperPad - Win32 Unicode Release"


"$(INTDIR)\linkitem.obj" : $(SOURCE) $(DEP_CPP_LINKI) "$(INTDIR)"\
 "$(INTDIR)\superpad.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\mainfrm.cpp
DEP_CPP_MAINF=\
	".\mainfrm.h"\
	".\stdafx.h"\
	".\superpad.h"\
	".\aboutbox.h"\
	".\pageset.h"\
	

!IF  "$(CFG)" == "SuperPad - Win32 Release"


"$(INTDIR)\mainfrm.obj" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\superpad.pch"


!ELSEIF  "$(CFG)" == "SuperPad - Win32 Debug"


"$(INTDIR)\mainfrm.obj" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\superpad.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\superpad.pch"


!ELSEIF  "$(CFG)" == "SuperPad - Macintosh Release"


"$(INTDIR)\mainfrm.obj" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\superpad.pch"


!ELSEIF  "$(CFG)" == "SuperPad - Macintosh Debug"


"$(INTDIR)\mainfrm.obj" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\superpad.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\superpad.pch"


!ELSEIF  "$(CFG)" == "SuperPad - Power Macintosh Release"


"$(INTDIR)\mainfrm.obj" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\superpad.pch"


!ELSEIF  "$(CFG)" == "SuperPad - Power Macintosh Debug"


"$(INTDIR)\mainfrm.obj" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\superpad.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\superpad.pch"


!ELSEIF  "$(CFG)" == "SuperPad - Win32 Unicode Debug"


"$(INTDIR)\mainfrm.obj" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\superpad.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\superpad.pch"


!ELSEIF  "$(CFG)" == "SuperPad - Win32 Unicode Release"


"$(INTDIR)\mainfrm.obj" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\superpad.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\paddoc.cpp
DEP_CPP_PADDO=\
	".\linkitem.h"\
	".\paddoc.h"\
	".\paditem.h"\
	".\stdafx.h"\
	".\superpad.h"\
	".\aboutbox.h"\
	".\pageset.h"\
	

!IF  "$(CFG)" == "SuperPad - Win32 Release"


"$(INTDIR)\paddoc.obj" : $(SOURCE) $(DEP_CPP_PADDO) "$(INTDIR)"\
 "$(INTDIR)\superpad.pch"


!ELSEIF  "$(CFG)" == "SuperPad - Win32 Debug"


"$(INTDIR)\paddoc.obj" : $(SOURCE) $(DEP_CPP_PADDO) "$(INTDIR)"\
 "$(INTDIR)\superpad.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_PADDO) "$(INTDIR)"\
 "$(INTDIR)\superpad.pch"


!ELSEIF  "$(CFG)" == "SuperPad - Macintosh Release"


"$(INTDIR)\paddoc.obj" : $(SOURCE) $(DEP_CPP_PADDO) "$(INTDIR)"\
 "$(INTDIR)\superpad.pch"


!ELSEIF  "$(CFG)" == "SuperPad - Macintosh Debug"


"$(INTDIR)\paddoc.obj" : $(SOURCE) $(DEP_CPP_PADDO) "$(INTDIR)"\
 "$(INTDIR)\superpad.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_PADDO) "$(INTDIR)"\
 "$(INTDIR)\superpad.pch"


!ELSEIF  "$(CFG)" == "SuperPad - Power Macintosh Release"


"$(INTDIR)\paddoc.obj" : $(SOURCE) $(DEP_CPP_PADDO) "$(INTDIR)"\
 "$(INTDIR)\superpad.pch"


!ELSEIF  "$(CFG)" == "SuperPad - Power Macintosh Debug"


"$(INTDIR)\paddoc.obj" : $(SOURCE) $(DEP_CPP_PADDO) "$(INTDIR)"\
 "$(INTDIR)\superpad.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_PADDO) "$(INTDIR)"\
 "$(INTDIR)\superpad.pch"


!ELSEIF  "$(CFG)" == "SuperPad - Win32 Unicode Debug"


"$(INTDIR)\paddoc.obj" : $(SOURCE) $(DEP_CPP_PADDO) "$(INTDIR)"\
 "$(INTDIR)\superpad.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_PADDO) "$(INTDIR)"\
 "$(INTDIR)\superpad.pch"


!ELSEIF  "$(CFG)" == "SuperPad - Win32 Unicode Release"


"$(INTDIR)\paddoc.obj" : $(SOURCE) $(DEP_CPP_PADDO) "$(INTDIR)"\
 "$(INTDIR)\superpad.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\padframe.cpp
DEP_CPP_PADFR=\
	".\padframe.h"\
	".\stdafx.h"\
	".\superpad.h"\
	".\aboutbox.h"\
	".\pageset.h"\
	

!IF  "$(CFG)" == "SuperPad - Win32 Release"


"$(INTDIR)\padframe.obj" : $(SOURCE) $(DEP_CPP_PADFR) "$(INTDIR)"\
 "$(INTDIR)\superpad.pch"


!ELSEIF  "$(CFG)" == "SuperPad - Win32 Debug"


"$(INTDIR)\padframe.obj" : $(SOURCE) $(DEP_CPP_PADFR) "$(INTDIR)"\
 "$(INTDIR)\superpad.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_PADFR) "$(INTDIR)"\
 "$(INTDIR)\superpad.pch"


!ELSEIF  "$(CFG)" == "SuperPad - Macintosh Release"


"$(INTDIR)\padframe.obj" : $(SOURCE) $(DEP_CPP_PADFR) "$(INTDIR)"\
 "$(INTDIR)\superpad.pch"


!ELSEIF  "$(CFG)" == "SuperPad - Macintosh Debug"


"$(INTDIR)\padframe.obj" : $(SOURCE) $(DEP_CPP_PADFR) "$(INTDIR)"\
 "$(INTDIR)\superpad.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_PADFR) "$(INTDIR)"\
 "$(INTDIR)\superpad.pch"


!ELSEIF  "$(CFG)" == "SuperPad - Power Macintosh Release"


"$(INTDIR)\padframe.obj" : $(SOURCE) $(DEP_CPP_PADFR) "$(INTDIR)"\
 "$(INTDIR)\superpad.pch"


!ELSEIF  "$(CFG)" == "SuperPad - Power Macintosh Debug"


"$(INTDIR)\padframe.obj" : $(SOURCE) $(DEP_CPP_PADFR) "$(INTDIR)"\
 "$(INTDIR)\superpad.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_PADFR) "$(INTDIR)"\
 "$(INTDIR)\superpad.pch"


!ELSEIF  "$(CFG)" == "SuperPad - Win32 Unicode Debug"


"$(INTDIR)\padframe.obj" : $(SOURCE) $(DEP_CPP_PADFR) "$(INTDIR)"\
 "$(INTDIR)\superpad.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_PADFR) "$(INTDIR)"\
 "$(INTDIR)\superpad.pch"


!ELSEIF  "$(CFG)" == "SuperPad - Win32 Unicode Release"


"$(INTDIR)\padframe.obj" : $(SOURCE) $(DEP_CPP_PADFR) "$(INTDIR)"\
 "$(INTDIR)\superpad.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\paditem.cpp
DEP_CPP_PADIT=\
	".\paddoc.h"\
	".\paditem.h"\
	".\padview.h"\
	".\stdafx.h"\
	

!IF  "$(CFG)" == "SuperPad - Win32 Release"


"$(INTDIR)\paditem.obj" : $(SOURCE) $(DEP_CPP_PADIT) "$(INTDIR)"\
 "$(INTDIR)\superpad.pch"


!ELSEIF  "$(CFG)" == "SuperPad - Win32 Debug"


"$(INTDIR)\paditem.obj" : $(SOURCE) $(DEP_CPP_PADIT) "$(INTDIR)"\
 "$(INTDIR)\superpad.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_PADIT) "$(INTDIR)"\
 "$(INTDIR)\superpad.pch"


!ELSEIF  "$(CFG)" == "SuperPad - Macintosh Release"


"$(INTDIR)\paditem.obj" : $(SOURCE) $(DEP_CPP_PADIT) "$(INTDIR)"\
 "$(INTDIR)\superpad.pch"


!ELSEIF  "$(CFG)" == "SuperPad - Macintosh Debug"


"$(INTDIR)\paditem.obj" : $(SOURCE) $(DEP_CPP_PADIT) "$(INTDIR)"\
 "$(INTDIR)\superpad.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_PADIT) "$(INTDIR)"\
 "$(INTDIR)\superpad.pch"


!ELSEIF  "$(CFG)" == "SuperPad - Power Macintosh Release"


"$(INTDIR)\paditem.obj" : $(SOURCE) $(DEP_CPP_PADIT) "$(INTDIR)"\
 "$(INTDIR)\superpad.pch"


!ELSEIF  "$(CFG)" == "SuperPad - Power Macintosh Debug"


"$(INTDIR)\paditem.obj" : $(SOURCE) $(DEP_CPP_PADIT) "$(INTDIR)"\
 "$(INTDIR)\superpad.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_PADIT) "$(INTDIR)"\
 "$(INTDIR)\superpad.pch"


!ELSEIF  "$(CFG)" == "SuperPad - Win32 Unicode Debug"


"$(INTDIR)\paditem.obj" : $(SOURCE) $(DEP_CPP_PADIT) "$(INTDIR)"\
 "$(INTDIR)\superpad.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_PADIT) "$(INTDIR)"\
 "$(INTDIR)\superpad.pch"


!ELSEIF  "$(CFG)" == "SuperPad - Win32 Unicode Release"


"$(INTDIR)\paditem.obj" : $(SOURCE) $(DEP_CPP_PADIT) "$(INTDIR)"\
 "$(INTDIR)\superpad.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\padview.cpp
DEP_CPP_PADVI=\
	".\linkitem.h"\
	".\paditem.h"\
	".\padview.h"\
	".\paddoc.h"\
	".\stdafx.h"\
	".\superpad.h"\
	".\aboutbox.h"\
	".\pageset.h"\
	".\tabstop.h"\
	

!IF  "$(CFG)" == "SuperPad - Win32 Release"


"$(INTDIR)\padview.obj" : $(SOURCE) $(DEP_CPP_PADVI) "$(INTDIR)"\
 "$(INTDIR)\superpad.pch"


!ELSEIF  "$(CFG)" == "SuperPad - Win32 Debug"


"$(INTDIR)\padview.obj" : $(SOURCE) $(DEP_CPP_PADVI) "$(INTDIR)"\
 "$(INTDIR)\superpad.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_PADVI) "$(INTDIR)"\
 "$(INTDIR)\superpad.pch"


!ELSEIF  "$(CFG)" == "SuperPad - Macintosh Release"


"$(INTDIR)\padview.obj" : $(SOURCE) $(DEP_CPP_PADVI) "$(INTDIR)"\
 "$(INTDIR)\superpad.pch"


!ELSEIF  "$(CFG)" == "SuperPad - Macintosh Debug"


"$(INTDIR)\padview.obj" : $(SOURCE) $(DEP_CPP_PADVI) "$(INTDIR)"\
 "$(INTDIR)\superpad.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_PADVI) "$(INTDIR)"\
 "$(INTDIR)\superpad.pch"


!ELSEIF  "$(CFG)" == "SuperPad - Power Macintosh Release"


"$(INTDIR)\padview.obj" : $(SOURCE) $(DEP_CPP_PADVI) "$(INTDIR)"\
 "$(INTDIR)\superpad.pch"


!ELSEIF  "$(CFG)" == "SuperPad - Power Macintosh Debug"


"$(INTDIR)\padview.obj" : $(SOURCE) $(DEP_CPP_PADVI) "$(INTDIR)"\
 "$(INTDIR)\superpad.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_PADVI) "$(INTDIR)"\
 "$(INTDIR)\superpad.pch"


!ELSEIF  "$(CFG)" == "SuperPad - Win32 Unicode Debug"


"$(INTDIR)\padview.obj" : $(SOURCE) $(DEP_CPP_PADVI) "$(INTDIR)"\
 "$(INTDIR)\superpad.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_PADVI) "$(INTDIR)"\
 "$(INTDIR)\superpad.pch"


!ELSEIF  "$(CFG)" == "SuperPad - Win32 Unicode Release"


"$(INTDIR)\padview.obj" : $(SOURCE) $(DEP_CPP_PADVI) "$(INTDIR)"\
 "$(INTDIR)\superpad.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\pageset.cpp
DEP_CPP_PAGES=\
	".\stdafx.h"\
	".\superpad.h"\
	".\aboutbox.h"\
	".\pageset.h"\
	

!IF  "$(CFG)" == "SuperPad - Win32 Release"


"$(INTDIR)\pageset.obj" : $(SOURCE) $(DEP_CPP_PAGES) "$(INTDIR)"\
 "$(INTDIR)\superpad.pch"


!ELSEIF  "$(CFG)" == "SuperPad - Win32 Debug"


"$(INTDIR)\pageset.obj" : $(SOURCE) $(DEP_CPP_PAGES) "$(INTDIR)"\
 "$(INTDIR)\superpad.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_PAGES) "$(INTDIR)"\
 "$(INTDIR)\superpad.pch"


!ELSEIF  "$(CFG)" == "SuperPad - Macintosh Release"


"$(INTDIR)\pageset.obj" : $(SOURCE) $(DEP_CPP_PAGES) "$(INTDIR)"\
 "$(INTDIR)\superpad.pch"


!ELSEIF  "$(CFG)" == "SuperPad - Macintosh Debug"


"$(INTDIR)\pageset.obj" : $(SOURCE) $(DEP_CPP_PAGES) "$(INTDIR)"\
 "$(INTDIR)\superpad.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_PAGES) "$(INTDIR)"\
 "$(INTDIR)\superpad.pch"


!ELSEIF  "$(CFG)" == "SuperPad - Power Macintosh Release"


"$(INTDIR)\pageset.obj" : $(SOURCE) $(DEP_CPP_PAGES) "$(INTDIR)"\
 "$(INTDIR)\superpad.pch"


!ELSEIF  "$(CFG)" == "SuperPad - Power Macintosh Debug"


"$(INTDIR)\pageset.obj" : $(SOURCE) $(DEP_CPP_PAGES) "$(INTDIR)"\
 "$(INTDIR)\superpad.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_PAGES) "$(INTDIR)"\
 "$(INTDIR)\superpad.pch"


!ELSEIF  "$(CFG)" == "SuperPad - Win32 Unicode Debug"


"$(INTDIR)\pageset.obj" : $(SOURCE) $(DEP_CPP_PAGES) "$(INTDIR)"\
 "$(INTDIR)\superpad.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_PAGES) "$(INTDIR)"\
 "$(INTDIR)\superpad.pch"


!ELSEIF  "$(CFG)" == "SuperPad - Win32 Unicode Release"


"$(INTDIR)\pageset.obj" : $(SOURCE) $(DEP_CPP_PAGES) "$(INTDIR)"\
 "$(INTDIR)\superpad.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\stdafx.cpp
DEP_CPP_STDAF=\
	".\stdafx.h"\
	

!IF  "$(CFG)" == "SuperPad - Win32 Release"

# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/superpad.pch" /Yc"stdafx.h" /Fo"$(INTDIR)/"\
 /c $(SOURCE) \
	

"$(INTDIR)\stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\superpad.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "SuperPad - Win32 Debug"

# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/superpad.pch" /Yc"stdafx.h"\
 /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\superpad.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "SuperPad - Macintosh Release"

# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /AL /Gt1 /Q68s /W3 /GX /O2 /D "_WINDOWS" /D "_MAC" /D "_68K_"\
 /D "WIN32" /D "NDEBUG" /D "_MBCS" /Fp"$(INTDIR)/superpad.pch" /Yc"stdafx.h"\
 /Fo"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\superpad.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "SuperPad - Macintosh Debug"

# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /AL /Gt1 /Q68s /Q68m /W3 /GX /Zi /Od /D "_WINDOWS" /D "_MAC" /D\
 "_68K_" /D "WIN32" /D "_DEBUG" /D "_MBCS" /Fp"$(INTDIR)/superpad.pch"\
 /Yc"stdafx.h" /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\superpad.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "SuperPad - Power Macintosh Release"

# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "_MAC" /D "_MPPC_" /D "_WINDOWS"\
 /D "WIN32" /D "_MBCS" /D "_AFXDLL" /Fp"$(INTDIR)/superpad.pch" /Yc"stdafx.h"\
 /Fo"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\superpad.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "SuperPad - Power Macintosh Debug"

# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MDd /W3 /GX /Zi /Od /D "_DEBUG" /D "_MAC" /D "_MPPC_" /D\
 "_WINDOWS" /D "WIN32" /D "_MBCS" /D "_AFXDLL" /Fp"$(INTDIR)/superpad.pch"\
 /Yc"stdafx.h" /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\superpad.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "SuperPad - Win32 Unicode Debug"

# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MDd /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS"\
 /D "_UNICODE" /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/superpad.pch" /Yc"stdafx.h"\
 /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\superpad.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "SuperPad - Win32 Unicode Release"

# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D\
 "_UNICODE" /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/superpad.pch" /Yc"stdafx.h"\
 /Fo"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\superpad.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\superpad.cpp
DEP_CPP_SUPER=\
	".\ipframe.h"\
	".\mainfrm.h"\
	".\paddoc.h"\
	".\padframe.h"\
	".\padview.h"\
	".\stdafx.h"\
	".\superpad.h"\
	".\aboutbox.h"\
	".\pageset.h"\
	

!IF  "$(CFG)" == "SuperPad - Win32 Release"


"$(INTDIR)\superpad.obj" : $(SOURCE) $(DEP_CPP_SUPER) "$(INTDIR)"\
 "$(INTDIR)\superpad.pch"


!ELSEIF  "$(CFG)" == "SuperPad - Win32 Debug"


"$(INTDIR)\superpad.obj" : $(SOURCE) $(DEP_CPP_SUPER) "$(INTDIR)"\
 "$(INTDIR)\superpad.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_SUPER) "$(INTDIR)"\
 "$(INTDIR)\superpad.pch"


!ELSEIF  "$(CFG)" == "SuperPad - Macintosh Release"


"$(INTDIR)\superpad.obj" : $(SOURCE) $(DEP_CPP_SUPER) "$(INTDIR)"\
 "$(INTDIR)\superpad.pch"


!ELSEIF  "$(CFG)" == "SuperPad - Macintosh Debug"


"$(INTDIR)\superpad.obj" : $(SOURCE) $(DEP_CPP_SUPER) "$(INTDIR)"\
 "$(INTDIR)\superpad.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_SUPER) "$(INTDIR)"\
 "$(INTDIR)\superpad.pch"


!ELSEIF  "$(CFG)" == "SuperPad - Power Macintosh Release"


"$(INTDIR)\superpad.obj" : $(SOURCE) $(DEP_CPP_SUPER) "$(INTDIR)"\
 "$(INTDIR)\superpad.pch"


!ELSEIF  "$(CFG)" == "SuperPad - Power Macintosh Debug"


"$(INTDIR)\superpad.obj" : $(SOURCE) $(DEP_CPP_SUPER) "$(INTDIR)"\
 "$(INTDIR)\superpad.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_SUPER) "$(INTDIR)"\
 "$(INTDIR)\superpad.pch"


!ELSEIF  "$(CFG)" == "SuperPad - Win32 Unicode Debug"


"$(INTDIR)\superpad.obj" : $(SOURCE) $(DEP_CPP_SUPER) "$(INTDIR)"\
 "$(INTDIR)\superpad.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_SUPER) "$(INTDIR)"\
 "$(INTDIR)\superpad.pch"


!ELSEIF  "$(CFG)" == "SuperPad - Win32 Unicode Release"


"$(INTDIR)\superpad.obj" : $(SOURCE) $(DEP_CPP_SUPER) "$(INTDIR)"\
 "$(INTDIR)\superpad.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\superpad.rc
DEP_RSC_SUPERP=\
	".\res\superpad.ico"\
	".\res\paddoc.ico"\
	".\res\toolbar.bmp"\
	".\res\itoolbar.bmp"\
	

!IF  "$(CFG)" == "SuperPad - Win32 Release"


"$(INTDIR)\superpad.res" : $(SOURCE) $(DEP_RSC_SUPERP) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "SuperPad - Win32 Debug"


"$(INTDIR)\superpad.res" : $(SOURCE) $(DEP_RSC_SUPERP) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "SuperPad - Macintosh Release"


"$(INTDIR)\superpad.rsc" : $(SOURCE) $(DEP_RSC_SUPERP) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "SuperPad - Macintosh Debug"


"$(INTDIR)\superpad.rsc" : $(SOURCE) $(DEP_RSC_SUPERP) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "SuperPad - Power Macintosh Release"


"$(INTDIR)\superpad.rsc" : $(SOURCE) $(DEP_RSC_SUPERP) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "SuperPad - Power Macintosh Debug"


"$(INTDIR)\superpad.rsc" : $(SOURCE) $(DEP_RSC_SUPERP) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "SuperPad - Win32 Unicode Debug"


"$(INTDIR)\superpad.res" : $(SOURCE) $(DEP_RSC_SUPERP) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "SuperPad - Win32 Unicode Release"


"$(INTDIR)\superpad.res" : $(SOURCE) $(DEP_RSC_SUPERP) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\tabstop.cpp
DEP_CPP_TABST=\
	".\stdafx.h"\
	".\superpad.h"\
	".\aboutbox.h"\
	".\pageset.h"\
	".\tabstop.h"\
	

!IF  "$(CFG)" == "SuperPad - Win32 Release"


"$(INTDIR)\tabstop.obj" : $(SOURCE) $(DEP_CPP_TABST) "$(INTDIR)"\
 "$(INTDIR)\superpad.pch"


!ELSEIF  "$(CFG)" == "SuperPad - Win32 Debug"


"$(INTDIR)\tabstop.obj" : $(SOURCE) $(DEP_CPP_TABST) "$(INTDIR)"\
 "$(INTDIR)\superpad.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_TABST) "$(INTDIR)"\
 "$(INTDIR)\superpad.pch"


!ELSEIF  "$(CFG)" == "SuperPad - Macintosh Release"


"$(INTDIR)\tabstop.obj" : $(SOURCE) $(DEP_CPP_TABST) "$(INTDIR)"\
 "$(INTDIR)\superpad.pch"


!ELSEIF  "$(CFG)" == "SuperPad - Macintosh Debug"


"$(INTDIR)\tabstop.obj" : $(SOURCE) $(DEP_CPP_TABST) "$(INTDIR)"\
 "$(INTDIR)\superpad.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_TABST) "$(INTDIR)"\
 "$(INTDIR)\superpad.pch"


!ELSEIF  "$(CFG)" == "SuperPad - Power Macintosh Release"


"$(INTDIR)\tabstop.obj" : $(SOURCE) $(DEP_CPP_TABST) "$(INTDIR)"\
 "$(INTDIR)\superpad.pch"


!ELSEIF  "$(CFG)" == "SuperPad - Power Macintosh Debug"


"$(INTDIR)\tabstop.obj" : $(SOURCE) $(DEP_CPP_TABST) "$(INTDIR)"\
 "$(INTDIR)\superpad.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_TABST) "$(INTDIR)"\
 "$(INTDIR)\superpad.pch"


!ELSEIF  "$(CFG)" == "SuperPad - Win32 Unicode Debug"


"$(INTDIR)\tabstop.obj" : $(SOURCE) $(DEP_CPP_TABST) "$(INTDIR)"\
 "$(INTDIR)\superpad.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_TABST) "$(INTDIR)"\
 "$(INTDIR)\superpad.pch"


!ELSEIF  "$(CFG)" == "SuperPad - Win32 Unicode Release"


"$(INTDIR)\tabstop.obj" : $(SOURCE) $(DEP_CPP_TABST) "$(INTDIR)"\
 "$(INTDIR)\superpad.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\spadmac.r

!IF  "$(CFG)" == "SuperPad - Win32 Release"

!ELSEIF  "$(CFG)" == "SuperPad - Win32 Debug"

!ELSEIF  "$(CFG)" == "SuperPad - Macintosh Release"

DEP_MRC_SPADM=\
	{$(INCLUDE)}"\mrc\types.r"\
	{$(INCLUDE)}"\mrc\balloons.r"\
	

"$(INTDIR)\spadmac.rsc" : $(SOURCE) $(DEP_MRC_SPADM) "$(INTDIR)"
   $(MRC) /o"$(INTDIR)/spadmac.rsc" /D "_68K_" /D "_MAC" /D "NDEBUG" /l 0x409\
 /NOLOGO $(SOURCE)


!ELSEIF  "$(CFG)" == "SuperPad - Macintosh Debug"

DEP_MRC_SPADM=\
	{$(INCLUDE)}"\mrc\types.r"\
	{$(INCLUDE)}"\mrc\balloons.r"\
	

"$(INTDIR)\spadmac.rsc" : $(SOURCE) $(DEP_MRC_SPADM) "$(INTDIR)"
   $(MRC) /o"$(INTDIR)/spadmac.rsc" /D "_68K_" /D "_MAC" /D "_DEBUG" /l 0x409\
 /NOLOGO $(SOURCE)


!ELSEIF  "$(CFG)" == "SuperPad - Power Macintosh Release"

DEP_MRC_SPADM=\
	{$(INCLUDE)}"\mrc\types.r"\
	{$(INCLUDE)}"\mrc\balloons.r"\
	

"$(INTDIR)\spadmac.rsc" : $(SOURCE) $(DEP_MRC_SPADM) "$(INTDIR)"
   $(MRC) /o"$(INTDIR)/spadmac.rsc" /D "_MPPC_" /D "_MAC" /D "NDEBUG" /l 0x409\
 /NOLOGO $(SOURCE)


!ELSEIF  "$(CFG)" == "SuperPad - Power Macintosh Debug"

DEP_MRC_SPADM=\
	{$(INCLUDE)}"\mrc\types.r"\
	{$(INCLUDE)}"\mrc\balloons.r"\
	

"$(INTDIR)\spadmac.rsc" : $(SOURCE) $(DEP_MRC_SPADM) "$(INTDIR)"
   $(MRC) /o"$(INTDIR)/spadmac.rsc" /D "_MPPC_" /D "_MAC" /D "_DEBUG" /l 0x409\
 /NOLOGO $(SOURCE)


!ELSEIF  "$(CFG)" == "SuperPad - Win32 Unicode Debug"

!ELSEIF  "$(CFG)" == "SuperPad - Win32 Unicode Release"

!ENDIF 

# End Source File
# End Target
# End Project
################################################################################
