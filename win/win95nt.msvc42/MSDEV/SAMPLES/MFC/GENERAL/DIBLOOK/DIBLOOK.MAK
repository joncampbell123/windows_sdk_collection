# Microsoft Visual C++ Generated NMAKE File, Format Version 40000
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101
# TARGTYPE "Power Macintosh Application" 0x0401
# TARGTYPE "Macintosh Application" 0x0301

!IF "$(CFG)" == ""
CFG=DibLook - Win32 Debug
!MESSAGE No configuration specified.  Defaulting to DibLook - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "DibLook - Win32 Release" && "$(CFG)" !=\
 "DibLook - Win32 Debug" && "$(CFG)" != "DibLook - Macintosh Release" &&\
 "$(CFG)" != "DibLook - Macintosh Debug" && "$(CFG)" !=\
 "DibLook - Power Macintosh Release" && "$(CFG)" !=\
 "DibLook - Power Macintosh Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "diblook.mak" CFG="DibLook - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "DibLook - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "DibLook - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE "DibLook - Macintosh Release" (based on "Macintosh Application")
!MESSAGE "DibLook - Macintosh Debug" (based on "Macintosh Application")
!MESSAGE "DibLook - Power Macintosh Release" (based on\
 "Power Macintosh Application")
!MESSAGE "DibLook - Power Macintosh Debug" (based on\
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
# PROP Target_Last_Scanned "DibLook - Power Macintosh Debug"

!IF  "$(CFG)" == "DibLook - Win32 Release"

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

ALL : "$(OUTDIR)\diblook.exe"

CLEAN : 
	-@erase ".\Release\diblook.exe"
	-@erase ".\Release\dibapi.obj"
	-@erase ".\Release\diblook.pch"
	-@erase ".\Release\myfile.obj"
	-@erase ".\Release\diblook.obj"
	-@erase ".\Release\stdafx.obj"
	-@erase ".\Release\mainfrm.obj"
	-@erase ".\Release\dibview.obj"
	-@erase ".\Release\dibdoc.obj"
	-@erase ".\Release\diblook.res"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"Stdafx.h" /c
# SUBTRACT CPP /Fr
CPP_PROJ=/nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/diblook.pch" /Yu"Stdafx.h" /Fo"$(INTDIR)/"\
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
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/diblook.res" /d "NDEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/diblook.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 /nologo /subsystem:windows /machine:I386
LINK32_FLAGS=/nologo /subsystem:windows /incremental:no\
 /pdb:"$(OUTDIR)/diblook.pdb" /machine:I386 /out:"$(OUTDIR)/diblook.exe" 
LINK32_OBJS= \
	"$(INTDIR)/dibapi.obj" \
	"$(INTDIR)/myfile.obj" \
	"$(INTDIR)/diblook.obj" \
	"$(INTDIR)/stdafx.obj" \
	"$(INTDIR)/mainfrm.obj" \
	"$(INTDIR)/dibview.obj" \
	"$(INTDIR)/dibdoc.obj" \
	"$(INTDIR)/diblook.res"

"$(OUTDIR)\diblook.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "DibLook - Win32 Debug"

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

ALL : "$(OUTDIR)\diblook.exe"

CLEAN : 
	-@erase ".\Debug\vc40.pdb"
	-@erase ".\Debug\diblook.pch"
	-@erase ".\Debug\diblook.exe"
	-@erase ".\Debug\dibdoc.obj"
	-@erase ".\Debug\dibapi.obj"
	-@erase ".\Debug\myfile.obj"
	-@erase ".\Debug\mainfrm.obj"
	-@erase ".\Debug\dibview.obj"
	-@erase ".\Debug\stdafx.obj"
	-@erase ".\Debug\diblook.obj"
	-@erase ".\Debug\diblook.res"
	-@erase ".\Debug\diblook.ilk"
	-@erase ".\Debug\diblook.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"Stdafx.h" /c
# SUBTRACT CPP /Fr
CPP_PROJ=/nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/diblook.pch" /Yu"Stdafx.h"\
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
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/diblook.res" /d "_DEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/diblook.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386
# ADD LINK32 /nologo /subsystem:windows /debug /machine:I386
LINK32_FLAGS=/nologo /subsystem:windows /incremental:yes\
 /pdb:"$(OUTDIR)/diblook.pdb" /debug /machine:I386 /out:"$(OUTDIR)/diblook.exe" 
LINK32_OBJS= \
	"$(INTDIR)/dibdoc.obj" \
	"$(INTDIR)/dibapi.obj" \
	"$(INTDIR)/myfile.obj" \
	"$(INTDIR)/mainfrm.obj" \
	"$(INTDIR)/dibview.obj" \
	"$(INTDIR)/stdafx.obj" \
	"$(INTDIR)/diblook.obj" \
	"$(INTDIR)/diblook.res"

"$(OUTDIR)\diblook.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "DibLook - Macintosh Release"

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

ALL : "$(OUTDIR)\diblook.exe" "$(OUTDIR)\diblook.trg"

CLEAN : 
	-@erase ".\MacRel\diblook.trg"
	-@erase ".\MacRel\diblook.exe"
	-@erase ".\MacRel\dibdoc.obj"
	-@erase ".\MacRel\diblook.pch"
	-@erase ".\MacRel\dibapi.obj"
	-@erase ".\MacRel\diblook.obj"
	-@erase ".\MacRel\myfile.obj"
	-@erase ".\MacRel\stdafx.obj"
	-@erase ".\MacRel\mainfrm.obj"
	-@erase ".\MacRel\dibview.obj"
	-@erase ".\MacRel\diblmac.rsc"
	-@erase ".\MacRel\diblook.rsc"

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
 "WIN32" /D "NDEBUG" /D "_MBCS" /Fp"$(INTDIR)/diblook.pch" /Yu"Stdafx.h"\
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
RSC_PROJ=/l 0x409 /r /m /fo"$(INTDIR)/diblook.rsc" /d "_MAC" /d "_68K_" /d\
 "NDEBUG" 
MRC=mrc.exe
# ADD BASE MRC /D "_68K_" /D "_MAC" /D "NDEBUG" /NOLOGO
# ADD MRC /D "_68K_" /D "_MAC" /D "NDEBUG" /NOLOGO
MRC_PROJ=/D "_68K_" /D "_MAC" /D "NDEBUG" /l 0x409 /NOLOGO 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/diblook.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 wlm.lib /nologo /MAC:bundle /machine:M68K
# ADD LINK32 /nologo /MAC:bundle /MAC:creator="DIBL" /machine:M68K
LINK32_FLAGS=/nologo /MAC:bundle /MAC:type="APPL" /MAC:creator="DIBL"\
 /pdb:"$(OUTDIR)/diblook.pdb" /machine:M68K /out:"$(OUTDIR)/diblook.exe" 
LINK32_OBJS= \
	"$(INTDIR)/dibdoc.obj" \
	"$(INTDIR)/dibapi.obj" \
	"$(INTDIR)/diblook.obj" \
	"$(INTDIR)/myfile.obj" \
	"$(INTDIR)/stdafx.obj" \
	"$(INTDIR)/mainfrm.obj" \
	"$(INTDIR)/dibview.obj" \
	"$(INTDIR)/diblmac.rsc" \
	"$(INTDIR)/diblook.rsc"

"$(OUTDIR)\diblook.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

MFILE32=mfile.exe
# ADD BASE MFILE32 COPY /NOLOGO
# ADD MFILE32 COPY /NOLOGO
MFILE32_FLAGS=COPY /NOLOGO 
MFILE32_FILES= \
	"$(OUTDIR)/diblook.exe"

"$(OUTDIR)\diblook.trg" : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\MacRel\diblook.exe\
 "$(MFILE32_DEST):diblook.exe">"$(OUTDIR)\diblook.trg"

DOWNLOAD : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\MacRel\diblook.exe\
 "$(MFILE32_DEST):diblook.exe">"$(OUTDIR)\diblook.trg"

!ELSEIF  "$(CFG)" == "DibLook - Macintosh Debug"

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

ALL : "$(OUTDIR)\diblook.exe" "$(OUTDIR)\diblook.trg"

CLEAN : 
	-@erase ".\MacDbg\vc40.pdb"
	-@erase ".\MacDbg\diblook.pch"
	-@erase ".\MacDbg\diblook.pdb"
	-@erase ".\MacDbg\dibdoc.obj"
	-@erase ".\MacDbg\dibapi.obj"
	-@erase ".\MacDbg\mainfrm.obj"
	-@erase ".\MacDbg\dibview.obj"
	-@erase ".\MacDbg\myfile.obj"
	-@erase ".\MacDbg\stdafx.obj"
	-@erase ".\MacDbg\diblook.obj"
	-@erase ".\MacDbg\diblook.rsc"
	-@erase ".\MacDbg\diblmac.rsc"
	-@erase ".\MacDbg\diblook.trg"
	-@erase ".\MacDbg\diblook.exe"

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
 "_68K_" /D "WIN32" /D "_DEBUG" /D "_MBCS" /Fp"$(INTDIR)/diblook.pch"\
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
RSC_PROJ=/l 0x409 /r /m /fo"$(INTDIR)/diblook.rsc" /d "_MAC" /d "_68K_" /d\
 "_DEBUG" 
MRC=mrc.exe
# ADD BASE MRC /D "_68K_" /D "_MAC" /D "_DEBUG" /NOLOGO
# ADD MRC /D "_68K_" /D "_MAC" /D "_DEBUG" /NOLOGO
MRC_PROJ=/D "_68K_" /D "_MAC" /D "_DEBUG" /l 0x409 /NOLOGO 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/diblook.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 wlm.lib /nologo /MAC:bundle /debug /machine:M68K
# ADD LINK32 /nologo /MAC:bundle /MAC:creator="DIBL" /debug /machine:M68K
LINK32_FLAGS=/nologo /MAC:bundle /MAC:type="APPL" /MAC:creator="DIBL"\
 /pdb:"$(OUTDIR)/diblook.pdb" /debug /machine:M68K /out:"$(OUTDIR)/diblook.exe" 
LINK32_OBJS= \
	"$(INTDIR)/dibdoc.obj" \
	"$(INTDIR)/dibapi.obj" \
	"$(INTDIR)/mainfrm.obj" \
	"$(INTDIR)/dibview.obj" \
	"$(INTDIR)/myfile.obj" \
	"$(INTDIR)/stdafx.obj" \
	"$(INTDIR)/diblook.obj" \
	"$(INTDIR)/diblook.rsc" \
	"$(INTDIR)/diblmac.rsc"

"$(OUTDIR)\diblook.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

MFILE32=mfile.exe
# ADD BASE MFILE32 COPY /NOLOGO
# ADD MFILE32 COPY /NOLOGO
MFILE32_FLAGS=COPY /NOLOGO 
MFILE32_FILES= \
	"$(OUTDIR)/diblook.exe"

"$(OUTDIR)\diblook.trg" : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\MacDbg\diblook.exe\
 "$(MFILE32_DEST):diblook.exe">"$(OUTDIR)\diblook.trg"

DOWNLOAD : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\MacDbg\diblook.exe\
 "$(MFILE32_DEST):diblook.exe">"$(OUTDIR)\diblook.trg"

!ELSEIF  "$(CFG)" == "DibLook - Power Macintosh Release"

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

ALL : "$(OUTDIR)\diblook.exe" "$(OUTDIR)\diblook.trg"

CLEAN : 
	-@erase ".\PMacRel\diblook.trg"
	-@erase ".\PMacRel\diblook.exe"
	-@erase ".\PMacRel\stdafx.obj"
	-@erase ".\PMacRel\diblook.obj"
	-@erase ".\PMacRel\diblook.pch"
	-@erase ".\PMacRel\dibdoc.obj"
	-@erase ".\PMacRel\dibapi.obj"
	-@erase ".\PMacRel\mainfrm.obj"
	-@erase ".\PMacRel\dibview.obj"
	-@erase ".\PMacRel\myfile.obj"
	-@erase ".\PMacRel\diblmac.rsc"
	-@erase ".\PMacRel\diblook.rsc"

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
 "_WINDOWS" /D "WIN32" /D "_MBCS" /D "_AFXDLL" /Fp"$(INTDIR)/diblook.pch"\
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
RSC_PROJ=/l 0x409 /r /m /fo"$(INTDIR)/diblook.rsc" /d "_MAC" /d "_MPPC_" /d\
 "NDEBUG" 
MRC=mrc.exe
# ADD BASE MRC /D "_MPPC_" /D "_MAC" /D "NDEBUG" /NOLOGO
# ADD MRC /D "_MPPC_" /D "_MAC" /D "NDEBUG" /NOLOGO
MRC_PROJ=/D "_MPPC_" /D "_MAC" /D "NDEBUG" /l 0x409 /NOLOGO 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/diblook.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 cfrg.rsc /nologo /mac:bundle /machine:MPPC
# ADD LINK32 /nologo /mac:bundle /mac:creator="DIBL" /machine:MPPC
LINK32_FLAGS=/nologo /mac:bundle /mac:type="APPL" /mac:creator="DIBL"\
 /incremental:no /pdb:"$(OUTDIR)/diblook.pdb" /machine:MPPC\
 /out:"$(OUTDIR)/diblook.exe" 
LINK32_OBJS= \
	"$(INTDIR)/stdafx.obj" \
	"$(INTDIR)/diblook.obj" \
	"$(INTDIR)/dibdoc.obj" \
	"$(INTDIR)/dibapi.obj" \
	"$(INTDIR)/mainfrm.obj" \
	"$(INTDIR)/dibview.obj" \
	"$(INTDIR)/myfile.obj" \
	"$(INTDIR)/diblmac.rsc" \
	"$(INTDIR)/diblook.rsc"

"$(OUTDIR)\diblook.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

MFILE32=mfile.exe
# ADD BASE MFILE32 COPY /NOLOGO
# ADD MFILE32 COPY /NOLOGO
MFILE32_FLAGS=COPY /NOLOGO 
MFILE32_FILES= \
	"$(OUTDIR)/diblook.exe"

"$(OUTDIR)\diblook.trg" : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\PMacRel\diblook.exe\
 "$(MFILE32_DEST):diblook.exe">"$(OUTDIR)\diblook.trg"

DOWNLOAD : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\PMacRel\diblook.exe\
 "$(MFILE32_DEST):diblook.exe">"$(OUTDIR)\diblook.trg"

!ELSEIF  "$(CFG)" == "DibLook - Power Macintosh Debug"

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

ALL : "$(OUTDIR)\diblook.exe" "$(OUTDIR)\diblook.trg"

CLEAN : 
	-@erase ".\PMacDbg\vc40.pdb"
	-@erase ".\PMacDbg\diblook.pch"
	-@erase ".\PMacDbg\diblook.ilk"
	-@erase ".\PMacDbg\dibview.obj"
	-@erase ".\PMacDbg\stdafx.obj"
	-@erase ".\PMacDbg\diblook.obj"
	-@erase ".\PMacDbg\dibdoc.obj"
	-@erase ".\PMacDbg\dibapi.obj"
	-@erase ".\PMacDbg\myfile.obj"
	-@erase ".\PMacDbg\mainfrm.obj"
	-@erase ".\PMacDbg\diblook.rsc"
	-@erase ".\PMacDbg\diblmac.rsc"
	-@erase ".\PMacDbg\diblook.pdb"
	-@erase ".\PMacDbg\diblook.trg"
	-@erase ".\PMacDbg\diblook.exe"

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
 "_WINDOWS" /D "WIN32" /D "_MBCS" /D "_AFXDLL" /Fp"$(INTDIR)/diblook.pch"\
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
RSC_PROJ=/l 0x409 /r /m /fo"$(INTDIR)/diblook.rsc" /d "_MAC" /d "_MPPC_" /d\
 "_DEBUG" 
MRC=mrc.exe
# ADD BASE MRC /D "_MPPC_" /D "_MAC" /D "_DEBUG" /NOLOGO
# ADD MRC /D "_MPPC_" /D "_MAC" /D "_DEBUG" /NOLOGO
MRC_PROJ=/D "_MPPC_" /D "_MAC" /D "_DEBUG" /l 0x409 /NOLOGO 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/diblook.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 cfrg.rsc /nologo /mac:bundle /debug /machine:MPPC
# ADD LINK32 /nologo /mac:bundle /mac:creator="DIBL" /debug /machine:MPPC
LINK32_FLAGS=/nologo /mac:bundle /mac:type="APPL" /mac:creator="DIBL"\
 /incremental:yes /pdb:"$(OUTDIR)/diblook.pdb" /debug /machine:MPPC\
 /out:"$(OUTDIR)/diblook.exe" 
LINK32_OBJS= \
	"$(INTDIR)/dibview.obj" \
	"$(INTDIR)/stdafx.obj" \
	"$(INTDIR)/diblook.obj" \
	"$(INTDIR)/dibdoc.obj" \
	"$(INTDIR)/dibapi.obj" \
	"$(INTDIR)/myfile.obj" \
	"$(INTDIR)/mainfrm.obj" \
	"$(INTDIR)/diblook.rsc" \
	"$(INTDIR)/diblmac.rsc"

"$(OUTDIR)\diblook.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

MFILE32=mfile.exe
# ADD BASE MFILE32 COPY /NOLOGO
# ADD MFILE32 COPY /NOLOGO
MFILE32_FLAGS=COPY /NOLOGO 
MFILE32_FILES= \
	"$(OUTDIR)/diblook.exe"

"$(OUTDIR)\diblook.trg" : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\PMacDbg\diblook.exe\
 "$(MFILE32_DEST):diblook.exe">"$(OUTDIR)\diblook.trg"

DOWNLOAD : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\PMacDbg\diblook.exe\
 "$(MFILE32_DEST):diblook.exe">"$(OUTDIR)\diblook.trg"

!ENDIF 

################################################################################
# Begin Target

# Name "DibLook - Win32 Release"
# Name "DibLook - Win32 Debug"
# Name "DibLook - Macintosh Release"
# Name "DibLook - Macintosh Debug"
# Name "DibLook - Power Macintosh Release"
# Name "DibLook - Power Macintosh Debug"

!IF  "$(CFG)" == "DibLook - Win32 Release"

!ELSEIF  "$(CFG)" == "DibLook - Win32 Debug"

!ELSEIF  "$(CFG)" == "DibLook - Macintosh Release"

!ELSEIF  "$(CFG)" == "DibLook - Macintosh Debug"

!ELSEIF  "$(CFG)" == "DibLook - Power Macintosh Release"

!ELSEIF  "$(CFG)" == "DibLook - Power Macintosh Debug"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\stdafx.cpp
DEP_CPP_STDAF=\
	".\stdafx.h"\
	

!IF  "$(CFG)" == "DibLook - Win32 Release"

# ADD CPP /Yc

BuildCmds= \
	$(CPP) /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/diblook.pch" /Yc /Fo"$(INTDIR)/" /c\
 $(SOURCE) \
	

"$(INTDIR)\stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\diblook.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "DibLook - Win32 Debug"

# ADD CPP /Yc

BuildCmds= \
	$(CPP) /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/diblook.pch" /Yc /Fo"$(INTDIR)/"\
 /Fd"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\diblook.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "DibLook - Macintosh Release"

# ADD CPP /Yc

BuildCmds= \
	$(CPP) /nologo /AL /Q68s /W3 /GX /O2 /D "_WINDOWS" /D "_MAC" /D "_68K_" /D\
 "WIN32" /D "NDEBUG" /D "_MBCS" /Fp"$(INTDIR)/diblook.pch" /Yc /Fo"$(INTDIR)/"\
 /c $(SOURCE) \
	

"$(INTDIR)\stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\diblook.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "DibLook - Macintosh Debug"

# ADD CPP /Yc

BuildCmds= \
	$(CPP) /nologo /AL /Q68s /Q68m /W3 /GX /Zi /Od /D "_WINDOWS" /D "_MAC" /D\
 "_68K_" /D "WIN32" /D "_DEBUG" /D "_MBCS" /Fp"$(INTDIR)/diblook.pch" /Yc\
 /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\diblook.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "DibLook - Power Macintosh Release"

# ADD CPP /Yc

BuildCmds= \
	$(CPP) /nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "_MAC" /D "_MPPC_" /D "_WINDOWS"\
 /D "WIN32" /D "_MBCS" /D "_AFXDLL" /Fp"$(INTDIR)/diblook.pch" /Yc\
 /Fo"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\diblook.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "DibLook - Power Macintosh Debug"

# ADD CPP /Yc

BuildCmds= \
	$(CPP) /nologo /MDd /W3 /GX /Zi /Od /D "_DEBUG" /D "_MAC" /D "_MPPC_" /D\
 "_WINDOWS" /D "WIN32" /D "_MBCS" /D "_AFXDLL" /Fp"$(INTDIR)/diblook.pch" /Yc\
 /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\diblook.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\dibdoc.cpp
DEP_CPP_DIBDO=\
	".\dibdoc.h"\
	".\dibapi.h"\
	".\diblook.h"\
	".\stdafx.h"\
	

!IF  "$(CFG)" == "DibLook - Win32 Release"


"$(INTDIR)\dibdoc.obj" : $(SOURCE) $(DEP_CPP_DIBDO) "$(INTDIR)"\
 "$(INTDIR)\diblook.pch"


!ELSEIF  "$(CFG)" == "DibLook - Win32 Debug"


"$(INTDIR)\dibdoc.obj" : $(SOURCE) $(DEP_CPP_DIBDO) "$(INTDIR)"\
 "$(INTDIR)\diblook.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_DIBDO) "$(INTDIR)"\
 "$(INTDIR)\diblook.pch"


!ELSEIF  "$(CFG)" == "DibLook - Macintosh Release"


"$(INTDIR)\dibdoc.obj" : $(SOURCE) $(DEP_CPP_DIBDO) "$(INTDIR)"\
 "$(INTDIR)\diblook.pch"


!ELSEIF  "$(CFG)" == "DibLook - Macintosh Debug"


"$(INTDIR)\dibdoc.obj" : $(SOURCE) $(DEP_CPP_DIBDO) "$(INTDIR)"\
 "$(INTDIR)\diblook.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_DIBDO) "$(INTDIR)"\
 "$(INTDIR)\diblook.pch"


!ELSEIF  "$(CFG)" == "DibLook - Power Macintosh Release"


"$(INTDIR)\dibdoc.obj" : $(SOURCE) $(DEP_CPP_DIBDO) "$(INTDIR)"\
 "$(INTDIR)\diblook.pch"


!ELSEIF  "$(CFG)" == "DibLook - Power Macintosh Debug"


"$(INTDIR)\dibdoc.obj" : $(SOURCE) $(DEP_CPP_DIBDO) "$(INTDIR)"\
 "$(INTDIR)\diblook.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_DIBDO) "$(INTDIR)"\
 "$(INTDIR)\diblook.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\diblook.cpp
DEP_CPP_DIBLO=\
	".\dibdoc.h"\
	".\dibapi.h"\
	".\diblook.h"\
	".\dibview.h"\
	".\mainfrm.h"\
	".\stdafx.h"\
	

!IF  "$(CFG)" == "DibLook - Win32 Release"


"$(INTDIR)\diblook.obj" : $(SOURCE) $(DEP_CPP_DIBLO) "$(INTDIR)"\
 "$(INTDIR)\diblook.pch"


!ELSEIF  "$(CFG)" == "DibLook - Win32 Debug"


"$(INTDIR)\diblook.obj" : $(SOURCE) $(DEP_CPP_DIBLO) "$(INTDIR)"\
 "$(INTDIR)\diblook.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_DIBLO) "$(INTDIR)"\
 "$(INTDIR)\diblook.pch"


!ELSEIF  "$(CFG)" == "DibLook - Macintosh Release"


"$(INTDIR)\diblook.obj" : $(SOURCE) $(DEP_CPP_DIBLO) "$(INTDIR)"\
 "$(INTDIR)\diblook.pch"


!ELSEIF  "$(CFG)" == "DibLook - Macintosh Debug"


"$(INTDIR)\diblook.obj" : $(SOURCE) $(DEP_CPP_DIBLO) "$(INTDIR)"\
 "$(INTDIR)\diblook.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_DIBLO) "$(INTDIR)"\
 "$(INTDIR)\diblook.pch"


!ELSEIF  "$(CFG)" == "DibLook - Power Macintosh Release"


"$(INTDIR)\diblook.obj" : $(SOURCE) $(DEP_CPP_DIBLO) "$(INTDIR)"\
 "$(INTDIR)\diblook.pch"


!ELSEIF  "$(CFG)" == "DibLook - Power Macintosh Debug"


"$(INTDIR)\diblook.obj" : $(SOURCE) $(DEP_CPP_DIBLO) "$(INTDIR)"\
 "$(INTDIR)\diblook.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_DIBLO) "$(INTDIR)"\
 "$(INTDIR)\diblook.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\diblook.rc
DEP_RSC_DIBLOO=\
	".\res\diblook.ico"\
	".\res\dibdoc.ico"\
	".\res\toolbar.bmp"\
	

!IF  "$(CFG)" == "DibLook - Win32 Release"


"$(INTDIR)\diblook.res" : $(SOURCE) $(DEP_RSC_DIBLOO) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "DibLook - Win32 Debug"


"$(INTDIR)\diblook.res" : $(SOURCE) $(DEP_RSC_DIBLOO) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "DibLook - Macintosh Release"


"$(INTDIR)\diblook.rsc" : $(SOURCE) $(DEP_RSC_DIBLOO) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "DibLook - Macintosh Debug"


"$(INTDIR)\diblook.rsc" : $(SOURCE) $(DEP_RSC_DIBLOO) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "DibLook - Power Macintosh Release"


"$(INTDIR)\diblook.rsc" : $(SOURCE) $(DEP_RSC_DIBLOO) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "DibLook - Power Macintosh Debug"


"$(INTDIR)\diblook.rsc" : $(SOURCE) $(DEP_RSC_DIBLOO) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\dibview.cpp
DEP_CPP_DIBVI=\
	".\dibapi.h"\
	".\dibdoc.h"\
	".\diblook.h"\
	".\dibview.h"\
	".\mainfrm.h"\
	".\stdafx.h"\
	

!IF  "$(CFG)" == "DibLook - Win32 Release"


"$(INTDIR)\dibview.obj" : $(SOURCE) $(DEP_CPP_DIBVI) "$(INTDIR)"\
 "$(INTDIR)\diblook.pch"


!ELSEIF  "$(CFG)" == "DibLook - Win32 Debug"


"$(INTDIR)\dibview.obj" : $(SOURCE) $(DEP_CPP_DIBVI) "$(INTDIR)"\
 "$(INTDIR)\diblook.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_DIBVI) "$(INTDIR)"\
 "$(INTDIR)\diblook.pch"


!ELSEIF  "$(CFG)" == "DibLook - Macintosh Release"


"$(INTDIR)\dibview.obj" : $(SOURCE) $(DEP_CPP_DIBVI) "$(INTDIR)"\
 "$(INTDIR)\diblook.pch"


!ELSEIF  "$(CFG)" == "DibLook - Macintosh Debug"


"$(INTDIR)\dibview.obj" : $(SOURCE) $(DEP_CPP_DIBVI) "$(INTDIR)"\
 "$(INTDIR)\diblook.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_DIBVI) "$(INTDIR)"\
 "$(INTDIR)\diblook.pch"


!ELSEIF  "$(CFG)" == "DibLook - Power Macintosh Release"


"$(INTDIR)\dibview.obj" : $(SOURCE) $(DEP_CPP_DIBVI) "$(INTDIR)"\
 "$(INTDIR)\diblook.pch"


!ELSEIF  "$(CFG)" == "DibLook - Power Macintosh Debug"


"$(INTDIR)\dibview.obj" : $(SOURCE) $(DEP_CPP_DIBVI) "$(INTDIR)"\
 "$(INTDIR)\diblook.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_DIBVI) "$(INTDIR)"\
 "$(INTDIR)\diblook.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\mainfrm.cpp
DEP_CPP_MAINF=\
	".\diblook.h"\
	".\mainfrm.h"\
	".\stdafx.h"\
	

!IF  "$(CFG)" == "DibLook - Win32 Release"


"$(INTDIR)\mainfrm.obj" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\diblook.pch"


!ELSEIF  "$(CFG)" == "DibLook - Win32 Debug"


"$(INTDIR)\mainfrm.obj" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\diblook.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\diblook.pch"


!ELSEIF  "$(CFG)" == "DibLook - Macintosh Release"


"$(INTDIR)\mainfrm.obj" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\diblook.pch"


!ELSEIF  "$(CFG)" == "DibLook - Macintosh Debug"


"$(INTDIR)\mainfrm.obj" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\diblook.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\diblook.pch"


!ELSEIF  "$(CFG)" == "DibLook - Power Macintosh Release"


"$(INTDIR)\mainfrm.obj" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\diblook.pch"


!ELSEIF  "$(CFG)" == "DibLook - Power Macintosh Debug"


"$(INTDIR)\mainfrm.obj" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\diblook.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\diblook.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\myfile.cpp
DEP_CPP_MYFIL=\
	".\dibapi.h"\
	".\stdafx.h"\
	

!IF  "$(CFG)" == "DibLook - Win32 Release"


"$(INTDIR)\myfile.obj" : $(SOURCE) $(DEP_CPP_MYFIL) "$(INTDIR)"\
 "$(INTDIR)\diblook.pch"


!ELSEIF  "$(CFG)" == "DibLook - Win32 Debug"


"$(INTDIR)\myfile.obj" : $(SOURCE) $(DEP_CPP_MYFIL) "$(INTDIR)"\
 "$(INTDIR)\diblook.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_MYFIL) "$(INTDIR)"\
 "$(INTDIR)\diblook.pch"


!ELSEIF  "$(CFG)" == "DibLook - Macintosh Release"


"$(INTDIR)\myfile.obj" : $(SOURCE) $(DEP_CPP_MYFIL) "$(INTDIR)"\
 "$(INTDIR)\diblook.pch"


!ELSEIF  "$(CFG)" == "DibLook - Macintosh Debug"


"$(INTDIR)\myfile.obj" : $(SOURCE) $(DEP_CPP_MYFIL) "$(INTDIR)"\
 "$(INTDIR)\diblook.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_MYFIL) "$(INTDIR)"\
 "$(INTDIR)\diblook.pch"


!ELSEIF  "$(CFG)" == "DibLook - Power Macintosh Release"


"$(INTDIR)\myfile.obj" : $(SOURCE) $(DEP_CPP_MYFIL) "$(INTDIR)"\
 "$(INTDIR)\diblook.pch"


!ELSEIF  "$(CFG)" == "DibLook - Power Macintosh Debug"


"$(INTDIR)\myfile.obj" : $(SOURCE) $(DEP_CPP_MYFIL) "$(INTDIR)"\
 "$(INTDIR)\diblook.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_MYFIL) "$(INTDIR)"\
 "$(INTDIR)\diblook.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\dibapi.cpp
DEP_CPP_DIBAP=\
	".\dibapi.h"\
	".\stdafx.h"\
	

!IF  "$(CFG)" == "DibLook - Win32 Release"


"$(INTDIR)\dibapi.obj" : $(SOURCE) $(DEP_CPP_DIBAP) "$(INTDIR)"\
 "$(INTDIR)\diblook.pch"


!ELSEIF  "$(CFG)" == "DibLook - Win32 Debug"


"$(INTDIR)\dibapi.obj" : $(SOURCE) $(DEP_CPP_DIBAP) "$(INTDIR)"\
 "$(INTDIR)\diblook.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_DIBAP) "$(INTDIR)"\
 "$(INTDIR)\diblook.pch"


!ELSEIF  "$(CFG)" == "DibLook - Macintosh Release"


"$(INTDIR)\dibapi.obj" : $(SOURCE) $(DEP_CPP_DIBAP) "$(INTDIR)"\
 "$(INTDIR)\diblook.pch"


!ELSEIF  "$(CFG)" == "DibLook - Macintosh Debug"


"$(INTDIR)\dibapi.obj" : $(SOURCE) $(DEP_CPP_DIBAP) "$(INTDIR)"\
 "$(INTDIR)\diblook.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_DIBAP) "$(INTDIR)"\
 "$(INTDIR)\diblook.pch"


!ELSEIF  "$(CFG)" == "DibLook - Power Macintosh Release"


"$(INTDIR)\dibapi.obj" : $(SOURCE) $(DEP_CPP_DIBAP) "$(INTDIR)"\
 "$(INTDIR)\diblook.pch"


!ELSEIF  "$(CFG)" == "DibLook - Power Macintosh Debug"


"$(INTDIR)\dibapi.obj" : $(SOURCE) $(DEP_CPP_DIBAP) "$(INTDIR)"\
 "$(INTDIR)\diblook.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_DIBAP) "$(INTDIR)"\
 "$(INTDIR)\diblook.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\diblmac.r

!IF  "$(CFG)" == "DibLook - Win32 Release"

!ELSEIF  "$(CFG)" == "DibLook - Win32 Debug"

!ELSEIF  "$(CFG)" == "DibLook - Macintosh Release"

DEP_MRC_DIBLM=\
	{$(INCLUDE)}"\mrc\types.r"\
	{$(INCLUDE)}"\mrc\balloons.r"\
	

"$(INTDIR)\diblmac.rsc" : $(SOURCE) $(DEP_MRC_DIBLM) "$(INTDIR)"
   $(MRC) /o"$(INTDIR)/diblmac.rsc" /D "_68K_" /D "_MAC" /D "NDEBUG" /l 0x409\
 /NOLOGO $(SOURCE)


!ELSEIF  "$(CFG)" == "DibLook - Macintosh Debug"

DEP_MRC_DIBLM=\
	{$(INCLUDE)}"\mrc\types.r"\
	{$(INCLUDE)}"\mrc\balloons.r"\
	

"$(INTDIR)\diblmac.rsc" : $(SOURCE) $(DEP_MRC_DIBLM) "$(INTDIR)"
   $(MRC) /o"$(INTDIR)/diblmac.rsc" /D "_68K_" /D "_MAC" /D "_DEBUG" /l 0x409\
 /NOLOGO $(SOURCE)


!ELSEIF  "$(CFG)" == "DibLook - Power Macintosh Release"

DEP_MRC_DIBLM=\
	{$(INCLUDE)}"\mrc\types.r"\
	{$(INCLUDE)}"\mrc\balloons.r"\
	

"$(INTDIR)\diblmac.rsc" : $(SOURCE) $(DEP_MRC_DIBLM) "$(INTDIR)"
   $(MRC) /o"$(INTDIR)/diblmac.rsc" /D "_MPPC_" /D "_MAC" /D "NDEBUG" /l 0x409\
 /NOLOGO $(SOURCE)


!ELSEIF  "$(CFG)" == "DibLook - Power Macintosh Debug"

DEP_MRC_DIBLM=\
	{$(INCLUDE)}"\mrc\types.r"\
	{$(INCLUDE)}"\mrc\balloons.r"\
	

"$(INTDIR)\diblmac.rsc" : $(SOURCE) $(DEP_MRC_DIBLM) "$(INTDIR)"
   $(MRC) /o"$(INTDIR)/diblmac.rsc" /D "_MPPC_" /D "_MAC" /D "_DEBUG" /l 0x409\
 /NOLOGO $(SOURCE)


!ENDIF 

# End Source File
# End Target
# End Project
################################################################################
