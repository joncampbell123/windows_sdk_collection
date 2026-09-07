# Microsoft Visual C++ Generated NMAKE File, Format Version 40000
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101
# TARGTYPE "Power Macintosh Application" 0x0401
# TARGTYPE "Macintosh Application" 0x0301

!IF "$(CFG)" == ""
CFG=CalcDriv - Win32 Debug
!MESSAGE No configuration specified.  Defaulting to CalcDriv - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "CalcDriv - Win32 Release" && "$(CFG)" !=\
 "CalcDriv - Win32 Debug" && "$(CFG)" != "CalcDriv - Macintosh Release" &&\
 "$(CFG)" != "CalcDriv - Macintosh Debug" && "$(CFG)" !=\
 "CalcDriv - Power Macintosh Release" && "$(CFG)" !=\
 "CalcDriv - Power Macintosh Debug" && "$(CFG)" !=\
 "CalcDriv - Win32 Unicode Release" && "$(CFG)" !=\
 "CalcDriv - Win32 Unicode Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "Calcdriv.mak" CFG="CalcDriv - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "CalcDriv - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "CalcDriv - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE "CalcDriv - Macintosh Release" (based on "Macintosh Application")
!MESSAGE "CalcDriv - Macintosh Debug" (based on "Macintosh Application")
!MESSAGE "CalcDriv - Power Macintosh Release" (based on\
 "Power Macintosh Application")
!MESSAGE "CalcDriv - Power Macintosh Debug" (based on\
 "Power Macintosh Application")
!MESSAGE "CalcDriv - Win32 Unicode Release" (based on\
 "Win32 (x86) Application")
!MESSAGE "CalcDriv - Win32 Unicode Debug" (based on "Win32 (x86) Application")
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
# PROP Target_Last_Scanned "CalcDriv - Win32 Debug"

!IF  "$(CFG)" == "CalcDriv - Win32 Release"

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

ALL : "$(OUTDIR)\Calcdriv.exe"

CLEAN : 
	-@erase ".\Release\vc40.idb"
	-@erase ".\Release\Calcdriv.pch"
	-@erase ".\Release\Calcdriv.exe"
	-@erase ".\Release\calcdriv.obj"
	-@erase ".\Release\Calctype.obj"
	-@erase ".\Release\Stdafx.obj"
	-@erase ".\Release\Calcdriv.res"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(OUTDIR)\calcdriv.bsc" : "$(OUTDIR)" $(BSC32_SBRS)

"$(OUTDIR)\calcdriv.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
CPP=cl.exe
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"Stdafx.h" /c
CPP_PROJ=/MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D\
 "_MBCS" /Fp"$(INTDIR)/Calcdriv.pch" /Yu"Stdafx.h" /Fo"$(INTDIR)/"\
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
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/Calcdriv.res" /d "NDEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/Calcdriv.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 /subsystem:windows /machine:I386
# SUBTRACT LINK32 /pdb:none
LINK32_FLAGS=/subsystem:windows /incremental:no /pdb:"$(OUTDIR)/Calcdriv.pdb"\
 /machine:I386 /out:"$(OUTDIR)/Calcdriv.exe" 
LINK32_OBJS= \
	"$(INTDIR)/calcdriv.obj" \
	"$(INTDIR)/Calctype.obj" \
	"$(INTDIR)/Stdafx.obj" \
	"$(INTDIR)/Calcdriv.res"

"$(OUTDIR)\Calcdriv.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "CalcDriv - Win32 Debug"

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

ALL : "$(OUTDIR)\Calcdriv.exe"

CLEAN : 
	-@erase ".\Debug\vc40.pdb"
	-@erase ".\Debug\Calcdriv.pch"
	-@erase ".\Debug\vc40.idb"
	-@erase ".\Debug\Calcdriv.exe"
	-@erase ".\Debug\calcdriv.obj"
	-@erase ".\Debug\Stdafx.obj"
	-@erase ".\Debug\Calctype.obj"
	-@erase ".\Debug\Calcdriv.res"
	-@erase ".\Debug\Calcdriv.ilk"
	-@erase ".\Debug\Calcdriv.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(OUTDIR)\calcdriv.bsc" : "$(OUTDIR)" $(BSC32_SBRS)

"$(OUTDIR)\calcdriv.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
CPP=cl.exe
# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"Stdafx.h" /c
CPP_PROJ=/nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/Calcdriv.pch" /Yu"Stdafx.h"\
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
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/Calcdriv.res" /d "_DEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/Calcdriv.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386
# ADD LINK32 /nologo /subsystem:windows /debug /machine:I386
LINK32_FLAGS=/nologo /subsystem:windows /incremental:yes\
 /pdb:"$(OUTDIR)/Calcdriv.pdb" /debug /machine:I386\
 /out:"$(OUTDIR)/Calcdriv.exe" 
LINK32_OBJS= \
	"$(INTDIR)/calcdriv.obj" \
	"$(INTDIR)/Stdafx.obj" \
	"$(INTDIR)/Calctype.obj" \
	"$(INTDIR)/Calcdriv.res"

"$(OUTDIR)\Calcdriv.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "CalcDriv - Macintosh Release"

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

ALL : "$(OUTDIR)\Calcdriv.exe" "$(OUTDIR)\Calcdriv.trg"

CLEAN : 
	-@erase ".\MacRel\Calcdriv.trg"
	-@erase ".\MacRel\Calcdriv.exe"
	-@erase ".\MacRel\Calctype.obj"
	-@erase ".\MacRel\Calcdriv.pch"
	-@erase ".\MacRel\Stdafx.obj"
	-@erase ".\MacRel\calcdriv.obj"
	-@erase ".\MacRel\Calcdriv.rsc"
	-@erase ".\MacRel\cdrvmac.rsc"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(OUTDIR)\calcdriv.bsc" : "$(OUTDIR)" $(BSC32_SBRS)

"$(OUTDIR)\calcdriv.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
MTL=mktyplib.exe
# ADD BASE MTL /nologo /D "NDEBUG" /mac
# ADD MTL /nologo /D "NDEBUG" /mac
MTL_PROJ=/nologo /D "NDEBUG" /mac 
CPP=cl.exe
# ADD BASE CPP /nologo /AL /Q68s /W3 /GX /O2 /D "_WINDOWS" /D "_MAC" /D "_68K_" /D "WIN32" /D "NDEBUG" /YX /c
# ADD CPP /nologo /AL /Q68s /W3 /GX /O2 /D "_WINDOWS" /D "_MAC" /D "_68K_" /D "WIN32" /D "NDEBUG" /D "_MBCS" /Yu"Stdafx.h" /c
CPP_PROJ=/nologo /AL /Q68s /W3 /GX /O2 /D "_WINDOWS" /D "_MAC" /D "_68K_" /D\
 "WIN32" /D "NDEBUG" /D "_MBCS" /Fp"$(INTDIR)/Calcdriv.pch" /Yu"Stdafx.h"\
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
RSC_PROJ=/l 0x409 /r /m /fo"$(INTDIR)/Calcdriv.rsc" /d "_MAC" /d "_68K_" /d\
 "NDEBUG" 
MRC=mrc.exe
# ADD BASE MRC /D "_68K_" /D "_MAC" /D "NDEBUG" /NOLOGO
# ADD MRC /D "_68K_" /D "_MAC" /D "NDEBUG" /NOLOGO
MRC_PROJ=/D "_68K_" /D "_MAC" /D "NDEBUG" /l 0x409 /NOLOGO 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/Calcdriv.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 wlm.lib /nologo /MAC:bundle /machine:M68K
# ADD LINK32 /nologo /MAC:bundle /MAC:creator="CDRV" /machine:M68K
LINK32_FLAGS=/nologo /MAC:bundle /MAC:type="APPL" /MAC:creator="CDRV"\
 /pdb:"$(OUTDIR)/Calcdriv.pdb" /machine:M68K /out:"$(OUTDIR)/Calcdriv.exe" 
LINK32_OBJS= \
	"$(INTDIR)/Calctype.obj" \
	"$(INTDIR)/Stdafx.obj" \
	"$(INTDIR)/calcdriv.obj" \
	"$(INTDIR)/Calcdriv.rsc" \
	"$(INTDIR)/cdrvmac.rsc"

"$(OUTDIR)\Calcdriv.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

MFILE32=mfile.exe
# ADD BASE MFILE32 COPY /NOLOGO
# ADD MFILE32 COPY /NOLOGO
MFILE32_FLAGS=COPY /NOLOGO 
MFILE32_FILES= \
	"$(OUTDIR)/Calcdriv.exe"

"$(OUTDIR)\Calcdriv.trg" : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\MacRel\Calcdriv.exe "$(MFILE32_DEST)"

DOWNLOAD : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\MacRel\Calcdriv.exe "$(MFILE32_DEST)"

!ELSEIF  "$(CFG)" == "CalcDriv - Macintosh Debug"

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

ALL : "$(OUTDIR)\Calcdriv.exe" "$(OUTDIR)\Calcdriv.trg"

CLEAN : 
	-@erase ".\MacDbg\vc40.pdb"
	-@erase ".\MacDbg\Calcdriv.pch"
	-@erase ".\MacDbg\Calcdriv.trg"
	-@erase ".\MacDbg\Calcdriv.exe"
	-@erase ".\MacDbg\Calctype.obj"
	-@erase ".\MacDbg\Stdafx.obj"
	-@erase ".\MacDbg\calcdriv.obj"
	-@erase ".\MacDbg\Calcdriv.rsc"
	-@erase ".\MacDbg\cdrvmac.rsc"
	-@erase ".\MacDbg\Calcdriv.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(OUTDIR)\calcdriv.bsc" : "$(OUTDIR)" $(BSC32_SBRS)

"$(OUTDIR)\calcdriv.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
MTL=mktyplib.exe
# ADD BASE MTL /nologo /D "_DEBUG" /mac
# ADD MTL /nologo /D "_DEBUG" /mac
MTL_PROJ=/nologo /D "_DEBUG" /mac 
CPP=cl.exe
# ADD BASE CPP /nologo /AL /Q68s /Q68m /W3 /GX /Zi /Od /D "_WINDOWS" /D "_MAC" /D "_68K_" /D "WIN32" /D "_DEBUG" /YX /c
# ADD CPP /nologo /AL /Q68s /Q68m /W3 /GX /Zi /Od /D "_WINDOWS" /D "_MAC" /D "_68K_" /D "WIN32" /D "_DEBUG" /D "_MBCS" /Yu"Stdafx.h" /c
CPP_PROJ=/nologo /AL /Q68s /Q68m /W3 /GX /Zi /Od /D "_WINDOWS" /D "_MAC" /D\
 "_68K_" /D "WIN32" /D "_DEBUG" /D "_MBCS" /Fp"$(INTDIR)/Calcdriv.pch"\
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
RSC_PROJ=/l 0x409 /r /m /fo"$(INTDIR)/Calcdriv.rsc" /d "_MAC" /d "_68K_" /d\
 "_DEBUG" 
MRC=mrc.exe
# ADD BASE MRC /D "_68K_" /D "_MAC" /D "_DEBUG" /NOLOGO
# ADD MRC /D "_68K_" /D "_MAC" /D "_DEBUG" /NOLOGO
MRC_PROJ=/D "_68K_" /D "_MAC" /D "_DEBUG" /l 0x409 /NOLOGO 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/Calcdriv.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 wlm.lib /nologo /MAC:bundle /debug /machine:M68K
# ADD LINK32 /nologo /MAC:bundle /MAC:creator="CDRV" /debug /machine:M68K
LINK32_FLAGS=/nologo /MAC:bundle /MAC:type="APPL" /MAC:creator="CDRV"\
 /pdb:"$(OUTDIR)/Calcdriv.pdb" /debug /machine:M68K\
 /out:"$(OUTDIR)/Calcdriv.exe" 
LINK32_OBJS= \
	"$(INTDIR)/Calctype.obj" \
	"$(INTDIR)/Stdafx.obj" \
	"$(INTDIR)/calcdriv.obj" \
	"$(INTDIR)/Calcdriv.rsc" \
	"$(INTDIR)/cdrvmac.rsc"

"$(OUTDIR)\Calcdriv.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

MFILE32=mfile.exe
# ADD BASE MFILE32 COPY /NOLOGO
# ADD MFILE32 COPY /NOLOGO
MFILE32_FLAGS=COPY /NOLOGO 
MFILE32_FILES= \
	"$(OUTDIR)/Calcdriv.exe"

"$(OUTDIR)\Calcdriv.trg" : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\MacDbg\Calcdriv.exe "$(MFILE32_DEST)"

DOWNLOAD : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\MacDbg\Calcdriv.exe "$(MFILE32_DEST)"

!ELSEIF  "$(CFG)" == "CalcDriv - Power Macintosh Release"

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

ALL : "$(OUTDIR)\Calcdriv.exe" "$(OUTDIR)\Calcdriv.trg"

CLEAN : 
	-@erase ".\PMacRel\Calcdriv.trg"
	-@erase ".\PMacRel\Calcdriv.exe"
	-@erase ".\PMacRel\Calctype.obj"
	-@erase ".\PMacRel\Calcdriv.pch"
	-@erase ".\PMacRel\Stdafx.obj"
	-@erase ".\PMacRel\calcdriv.obj"
	-@erase ".\PMacRel\Calcdriv.rsc"
	-@erase ".\PMacRel\cdrvmac.rsc"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(OUTDIR)\calcdriv.bsc" : "$(OUTDIR)" $(BSC32_SBRS)

"$(OUTDIR)\calcdriv.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
MTL=mktyplib.exe
# ADD BASE MTL /nologo /D "NDEBUG"
# ADD MTL /nologo /D "NDEBUG"
MTL_PROJ=/nologo /D "NDEBUG" 
CPP=cl.exe
# ADD BASE CPP /nologo /ML /W3 /GX /O2 /D "_MAC" /D "_MPPC_" /D "_WINDOWS" /D "WIN32" /D "NDEBUG" /YX /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "_MAC" /D "_MPPC_" /D "_WINDOWS" /D "WIN32" /D "_MBCS" /D "_AFXDLL" /Yu"Stdafx.h" /c
CPP_PROJ=/nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "_MAC" /D "_MPPC_" /D\
 "_WINDOWS" /D "WIN32" /D "_MBCS" /D "_AFXDLL" /Fp"$(INTDIR)/Calcdriv.pch"\
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
RSC_PROJ=/l 0x409 /r /m /fo"$(INTDIR)/Calcdriv.rsc" /d "_MAC" /d "_MPPC_" /d\
 "NDEBUG" 
MRC=mrc.exe
# ADD BASE MRC /D "_MPPC_" /D "_MAC" /D "NDEBUG" /NOLOGO
# ADD MRC /D "_MPPC_" /D "_MAC" /D "NDEBUG" /NOLOGO
MRC_PROJ=/D "_MPPC_" /D "_MAC" /D "NDEBUG" /l 0x409 /NOLOGO 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/Calcdriv.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 cfrg.rsc /nologo /mac:bundle /machine:MPPC
# ADD LINK32 /nologo /mac:bundle /mac:creator="CDRV" /machine:MPPC
LINK32_FLAGS=/nologo /mac:bundle /mac:type="APPL" /mac:creator="CDRV"\
 /incremental:no /pdb:"$(OUTDIR)/Calcdriv.pdb" /machine:MPPC\
 /out:"$(OUTDIR)/Calcdriv.exe" 
LINK32_OBJS= \
	"$(INTDIR)/Calctype.obj" \
	"$(INTDIR)/Stdafx.obj" \
	"$(INTDIR)/calcdriv.obj" \
	"$(INTDIR)/Calcdriv.rsc" \
	"$(INTDIR)/cdrvmac.rsc"

"$(OUTDIR)\Calcdriv.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

MFILE32=mfile.exe
# ADD BASE MFILE32 COPY /NOLOGO
# ADD MFILE32 COPY /NOLOGO
MFILE32_FLAGS=COPY /NOLOGO 
MFILE32_FILES= \
	"$(OUTDIR)/Calcdriv.exe"

"$(OUTDIR)\Calcdriv.trg" : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\PMacRel\Calcdriv.exe "$(MFILE32_DEST)"

DOWNLOAD : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\PMacRel\Calcdriv.exe "$(MFILE32_DEST)"

!ELSEIF  "$(CFG)" == "CalcDriv - Power Macintosh Debug"

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

ALL : "$(OUTDIR)\Calcdriv.exe" "$(OUTDIR)\Calcdriv.trg"

CLEAN : 
	-@erase ".\PMacDbg\vc40.pdb"
	-@erase ".\PMacDbg\Calcdriv.pch"
	-@erase ".\PMacDbg\Calcdriv.trg"
	-@erase ".\PMacDbg\Calcdriv.exe"
	-@erase ".\PMacDbg\Calctype.obj"
	-@erase ".\PMacDbg\Stdafx.obj"
	-@erase ".\PMacDbg\calcdriv.obj"
	-@erase ".\PMacDbg\cdrvmac.rsc"
	-@erase ".\PMacDbg\Calcdriv.rsc"
	-@erase ".\PMacDbg\Calcdriv.ilk"
	-@erase ".\PMacDbg\Calcdriv.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(OUTDIR)\calcdriv.bsc" : "$(OUTDIR)" $(BSC32_SBRS)

"$(OUTDIR)\calcdriv.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
MTL=mktyplib.exe
# ADD BASE MTL /nologo /D "_DEBUG"
# ADD MTL /nologo /D "_DEBUG"
MTL_PROJ=/nologo /D "_DEBUG" 
CPP=cl.exe
# ADD BASE CPP /nologo /W3 /GX /Zi /Od /D "_MAC" /D "_MPPC_" /D "_WINDOWS" /D "WIN32" /D "_DEBUG" /YX /c
# ADD CPP /nologo /MDd /W3 /GX /Zi /Od /D "_DEBUG" /D "_MAC" /D "_MPPC_" /D "_WINDOWS" /D "WIN32" /D "_MBCS" /D "_AFXDLL" /Yu"Stdafx.h" /c
CPP_PROJ=/nologo /MDd /W3 /GX /Zi /Od /D "_DEBUG" /D "_MAC" /D "_MPPC_" /D\
 "_WINDOWS" /D "WIN32" /D "_MBCS" /D "_AFXDLL" /Fp"$(INTDIR)/Calcdriv.pch"\
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
RSC_PROJ=/l 0x409 /r /m /fo"$(INTDIR)/Calcdriv.rsc" /d "_MAC" /d "_MPPC_" /d\
 "_DEBUG" 
MRC=mrc.exe
# ADD BASE MRC /D "_MPPC_" /D "_MAC" /D "_DEBUG" /NOLOGO
# ADD MRC /D "_MPPC_" /D "_MAC" /D "_DEBUG" /NOLOGO
MRC_PROJ=/D "_MPPC_" /D "_MAC" /D "_DEBUG" /l 0x409 /NOLOGO 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/Calcdriv.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 cfrg.rsc /nologo /mac:bundle /debug /machine:MPPC
# ADD LINK32 /nologo /mac:bundle /mac:creator="CDRV" /debug /machine:MPPC
LINK32_FLAGS=/nologo /mac:bundle /mac:type="APPL" /mac:creator="CDRV"\
 /incremental:yes /pdb:"$(OUTDIR)/Calcdriv.pdb" /debug /machine:MPPC\
 /out:"$(OUTDIR)/Calcdriv.exe" 
LINK32_OBJS= \
	"$(INTDIR)/Calctype.obj" \
	"$(INTDIR)/Stdafx.obj" \
	"$(INTDIR)/calcdriv.obj" \
	"$(INTDIR)/cdrvmac.rsc" \
	"$(INTDIR)/Calcdriv.rsc"

"$(OUTDIR)\Calcdriv.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

MFILE32=mfile.exe
# ADD BASE MFILE32 COPY /NOLOGO
# ADD MFILE32 COPY /NOLOGO
MFILE32_FLAGS=COPY /NOLOGO 
MFILE32_FILES= \
	"$(OUTDIR)/Calcdriv.exe"

"$(OUTDIR)\Calcdriv.trg" : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\PMacDbg\Calcdriv.exe "$(MFILE32_DEST)"

DOWNLOAD : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\PMacDbg\Calcdriv.exe "$(MFILE32_DEST)"

!ELSEIF  "$(CFG)" == "CalcDriv - Win32 Unicode Release"

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

ALL : "$(OUTDIR)\Calcdriv.exe"

CLEAN : 
	-@erase ".\UniRelease\vc40.idb"
	-@erase ".\UniRelease\Calcdriv.pch"
	-@erase ".\UniRelease\Calcdriv.exe"
	-@erase ".\UniRelease\calcdriv.obj"
	-@erase ".\UniRelease\Stdafx.obj"
	-@erase ".\UniRelease\Calctype.obj"
	-@erase ".\UniRelease\Calcdriv.res"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(OUTDIR)\calcdriv.bsc" : "$(OUTDIR)" $(BSC32_SBRS)

"$(OUTDIR)\calcdriv.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
CPP=cl.exe
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_UNICODE" /D "_AFXDLL" /D "_MBCS" /Yu"Stdafx.h" /c
CPP_PROJ=/nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D\
 "_UNICODE" /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/Calcdriv.pch" /Yu"Stdafx.h"\
 /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
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
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/Calcdriv.res" /d "NDEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/Calcdriv.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 /nologo /entry:"wWinMainCRTStartup" /subsystem:windows /machine:I386
LINK32_FLAGS=/nologo /entry:"wWinMainCRTStartup" /subsystem:windows\
 /incremental:no /pdb:"$(OUTDIR)/Calcdriv.pdb" /machine:I386\
 /out:"$(OUTDIR)/Calcdriv.exe" 
LINK32_OBJS= \
	"$(INTDIR)/calcdriv.obj" \
	"$(INTDIR)/Stdafx.obj" \
	"$(INTDIR)/Calctype.obj" \
	"$(INTDIR)/Calcdriv.res"

"$(OUTDIR)\Calcdriv.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "CalcDriv - Win32 Unicode Debug"

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

ALL : "$(OUTDIR)\Calcdriv.exe"

CLEAN : 
	-@erase ".\UniDebug\vc40.pdb"
	-@erase ".\UniDebug\Calcdriv.pch"
	-@erase ".\UniDebug\vc40.idb"
	-@erase ".\UniDebug\Calcdriv.exe"
	-@erase ".\UniDebug\Stdafx.obj"
	-@erase ".\UniDebug\Calctype.obj"
	-@erase ".\UniDebug\calcdriv.obj"
	-@erase ".\UniDebug\Calcdriv.res"
	-@erase ".\UniDebug\Calcdriv.ilk"
	-@erase ".\UniDebug\Calcdriv.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(OUTDIR)\calcdriv.bsc" : "$(OUTDIR)" $(BSC32_SBRS)

"$(OUTDIR)\calcdriv.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
CPP=cl.exe
# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_UNICODE" /D "_AFXDLL" /D "_MBCS" /Yu"Stdafx.h" /c
CPP_PROJ=/nologo /MDd /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS"\
 /D "_UNICODE" /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/Calcdriv.pch" /Yu"Stdafx.h"\
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
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/Calcdriv.res" /d "_DEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/Calcdriv.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386
# ADD LINK32 /nologo /entry:"wWinMainCRTStartup" /subsystem:windows /debug /machine:I386
LINK32_FLAGS=/nologo /entry:"wWinMainCRTStartup" /subsystem:windows\
 /incremental:yes /pdb:"$(OUTDIR)/Calcdriv.pdb" /debug /machine:I386\
 /out:"$(OUTDIR)/Calcdriv.exe" 
LINK32_OBJS= \
	"$(INTDIR)/Stdafx.obj" \
	"$(INTDIR)/Calctype.obj" \
	"$(INTDIR)/calcdriv.obj" \
	"$(INTDIR)/Calcdriv.res"

"$(OUTDIR)\Calcdriv.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 

################################################################################
# Begin Target

# Name "CalcDriv - Win32 Release"
# Name "CalcDriv - Win32 Debug"
# Name "CalcDriv - Macintosh Release"
# Name "CalcDriv - Macintosh Debug"
# Name "CalcDriv - Power Macintosh Release"
# Name "CalcDriv - Power Macintosh Debug"
# Name "CalcDriv - Win32 Unicode Release"
# Name "CalcDriv - Win32 Unicode Debug"

!IF  "$(CFG)" == "CalcDriv - Win32 Release"

!ELSEIF  "$(CFG)" == "CalcDriv - Win32 Debug"

!ELSEIF  "$(CFG)" == "CalcDriv - Macintosh Release"

!ELSEIF  "$(CFG)" == "CalcDriv - Macintosh Debug"

!ELSEIF  "$(CFG)" == "CalcDriv - Power Macintosh Release"

!ELSEIF  "$(CFG)" == "CalcDriv - Power Macintosh Debug"

!ELSEIF  "$(CFG)" == "CalcDriv - Win32 Unicode Release"

!ELSEIF  "$(CFG)" == "CalcDriv - Win32 Unicode Debug"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\Stdafx.cpp
DEP_CPP_STDAF=\
	".\Stdafx.h"\
	

!IF  "$(CFG)" == "CalcDriv - Win32 Release"

# ADD CPP /Yc

BuildCmds= \
	$(CPP) /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D\
 "_MBCS" /Fp"$(INTDIR)/Calcdriv.pch" /Yc /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c\
 $(SOURCE) \
	

"$(INTDIR)\Stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\vc40.idb" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\Calcdriv.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "CalcDriv - Win32 Debug"

# ADD CPP /Yc

BuildCmds= \
	$(CPP) /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/Calcdriv.pch" /Yc /Fo"$(INTDIR)/"\
 /Fd"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\Stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\vc40.idb" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\Calcdriv.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "CalcDriv - Macintosh Release"

# ADD CPP /Yc

BuildCmds= \
	$(CPP) /nologo /AL /Q68s /W3 /GX /O2 /D "_WINDOWS" /D "_MAC" /D "_68K_" /D\
 "WIN32" /D "NDEBUG" /D "_MBCS" /Fp"$(INTDIR)/Calcdriv.pch" /Yc /Fo"$(INTDIR)/"\
 /Fd"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\Stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\Calcdriv.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "CalcDriv - Macintosh Debug"

# ADD CPP /Yc

BuildCmds= \
	$(CPP) /nologo /AL /Q68s /Q68m /W3 /GX /Zi /Od /D "_WINDOWS" /D "_MAC" /D\
 "_68K_" /D "WIN32" /D "_DEBUG" /D "_MBCS" /Fp"$(INTDIR)/Calcdriv.pch" /Yc\
 /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\Stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\Calcdriv.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "CalcDriv - Power Macintosh Release"

# ADD CPP /Yc

BuildCmds= \
	$(CPP) /nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "_MAC" /D "_MPPC_" /D "_WINDOWS"\
 /D "WIN32" /D "_MBCS" /D "_AFXDLL" /Fp"$(INTDIR)/Calcdriv.pch" /Yc\
 /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\Stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\Calcdriv.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "CalcDriv - Power Macintosh Debug"

# ADD CPP /Yc

BuildCmds= \
	$(CPP) /nologo /MDd /W3 /GX /Zi /Od /D "_DEBUG" /D "_MAC" /D "_MPPC_" /D\
 "_WINDOWS" /D "WIN32" /D "_MBCS" /D "_AFXDLL" /Fp"$(INTDIR)/Calcdriv.pch" /Yc\
 /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\Stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\Calcdriv.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "CalcDriv - Win32 Unicode Release"

# ADD CPP /Yc

BuildCmds= \
	$(CPP) /nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D\
 "_UNICODE" /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/Calcdriv.pch" /Yc\
 /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\Stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\vc40.idb" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\Calcdriv.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "CalcDriv - Win32 Unicode Debug"

# ADD CPP /Yc

BuildCmds= \
	$(CPP) /nologo /MDd /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS"\
 /D "_UNICODE" /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/Calcdriv.pch" /Yc\
 /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\Stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\vc40.idb" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\Calcdriv.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Calcdriv.rc
DEP_RSC_CALCD=\
	".\Res\Calcdriv.ico"\
	".\Res\Calcdriv.rc2"\
	

!IF  "$(CFG)" == "CalcDriv - Win32 Release"


"$(INTDIR)\Calcdriv.res" : $(SOURCE) $(DEP_RSC_CALCD) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "CalcDriv - Win32 Debug"


"$(INTDIR)\Calcdriv.res" : $(SOURCE) $(DEP_RSC_CALCD) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "CalcDriv - Macintosh Release"


"$(INTDIR)\Calcdriv.rsc" : $(SOURCE) $(DEP_RSC_CALCD) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "CalcDriv - Macintosh Debug"


"$(INTDIR)\Calcdriv.rsc" : $(SOURCE) $(DEP_RSC_CALCD) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "CalcDriv - Power Macintosh Release"


"$(INTDIR)\Calcdriv.rsc" : $(SOURCE) $(DEP_RSC_CALCD) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "CalcDriv - Power Macintosh Debug"


"$(INTDIR)\Calcdriv.rsc" : $(SOURCE) $(DEP_RSC_CALCD) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "CalcDriv - Win32 Unicode Release"


"$(INTDIR)\Calcdriv.res" : $(SOURCE) $(DEP_RSC_CALCD) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "CalcDriv - Win32 Unicode Debug"


"$(INTDIR)\Calcdriv.res" : $(SOURCE) $(DEP_RSC_CALCD) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Calctype.cpp
DEP_CPP_CALCT=\
	".\Calctype.h"\
	".\Stdafx.h"\
	

!IF  "$(CFG)" == "CalcDriv - Win32 Release"


"$(INTDIR)\Calctype.obj" : $(SOURCE) $(DEP_CPP_CALCT) "$(INTDIR)"\
 "$(INTDIR)\Calcdriv.pch"

"$(INTDIR)\vc40.idb" : $(SOURCE) $(DEP_CPP_CALCT) "$(INTDIR)"\
 "$(INTDIR)\Calcdriv.pch"


!ELSEIF  "$(CFG)" == "CalcDriv - Win32 Debug"


"$(INTDIR)\Calctype.obj" : $(SOURCE) $(DEP_CPP_CALCT) "$(INTDIR)"\
 "$(INTDIR)\Calcdriv.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_CALCT) "$(INTDIR)"\
 "$(INTDIR)\Calcdriv.pch"

"$(INTDIR)\vc40.idb" : $(SOURCE) $(DEP_CPP_CALCT) "$(INTDIR)"\
 "$(INTDIR)\Calcdriv.pch"


!ELSEIF  "$(CFG)" == "CalcDriv - Macintosh Release"


"$(INTDIR)\Calctype.obj" : $(SOURCE) $(DEP_CPP_CALCT) "$(INTDIR)"\
 "$(INTDIR)\Calcdriv.pch"


!ELSEIF  "$(CFG)" == "CalcDriv - Macintosh Debug"


"$(INTDIR)\Calctype.obj" : $(SOURCE) $(DEP_CPP_CALCT) "$(INTDIR)"\
 "$(INTDIR)\Calcdriv.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_CALCT) "$(INTDIR)"\
 "$(INTDIR)\Calcdriv.pch"


!ELSEIF  "$(CFG)" == "CalcDriv - Power Macintosh Release"


"$(INTDIR)\Calctype.obj" : $(SOURCE) $(DEP_CPP_CALCT) "$(INTDIR)"\
 "$(INTDIR)\Calcdriv.pch"


!ELSEIF  "$(CFG)" == "CalcDriv - Power Macintosh Debug"


"$(INTDIR)\Calctype.obj" : $(SOURCE) $(DEP_CPP_CALCT) "$(INTDIR)"\
 "$(INTDIR)\Calcdriv.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_CALCT) "$(INTDIR)"\
 "$(INTDIR)\Calcdriv.pch"


!ELSEIF  "$(CFG)" == "CalcDriv - Win32 Unicode Release"


"$(INTDIR)\Calctype.obj" : $(SOURCE) $(DEP_CPP_CALCT) "$(INTDIR)"\
 "$(INTDIR)\Calcdriv.pch"

"$(INTDIR)\vc40.idb" : $(SOURCE) $(DEP_CPP_CALCT) "$(INTDIR)"\
 "$(INTDIR)\Calcdriv.pch"


!ELSEIF  "$(CFG)" == "CalcDriv - Win32 Unicode Debug"


"$(INTDIR)\Calctype.obj" : $(SOURCE) $(DEP_CPP_CALCT) "$(INTDIR)"\
 "$(INTDIR)\Calcdriv.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_CALCT) "$(INTDIR)"\
 "$(INTDIR)\Calcdriv.pch"

"$(INTDIR)\vc40.idb" : $(SOURCE) $(DEP_CPP_CALCT) "$(INTDIR)"\
 "$(INTDIR)\Calcdriv.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\cdrvmac.r

!IF  "$(CFG)" == "CalcDriv - Win32 Release"

!ELSEIF  "$(CFG)" == "CalcDriv - Win32 Debug"

!ELSEIF  "$(CFG)" == "CalcDriv - Macintosh Release"


"$(INTDIR)\cdrvmac.rsc" : $(SOURCE) "$(INTDIR)"
   $(MRC) /o"$(INTDIR)/cdrvmac.rsc" /D "_68K_" /D "_MAC" /D "NDEBUG" /l 0x409\
 /NOLOGO $(SOURCE)


!ELSEIF  "$(CFG)" == "CalcDriv - Macintosh Debug"


"$(INTDIR)\cdrvmac.rsc" : $(SOURCE) "$(INTDIR)"
   $(MRC) /o"$(INTDIR)/cdrvmac.rsc" /D "_68K_" /D "_MAC" /D "_DEBUG" /l 0x409\
 /NOLOGO $(SOURCE)


!ELSEIF  "$(CFG)" == "CalcDriv - Power Macintosh Release"


"$(INTDIR)\cdrvmac.rsc" : $(SOURCE) "$(INTDIR)"
   $(MRC) /o"$(INTDIR)/cdrvmac.rsc" /D "_MPPC_" /D "_MAC" /D "NDEBUG" /l 0x409\
 /NOLOGO $(SOURCE)


!ELSEIF  "$(CFG)" == "CalcDriv - Power Macintosh Debug"


"$(INTDIR)\cdrvmac.rsc" : $(SOURCE) "$(INTDIR)"
   $(MRC) /o"$(INTDIR)/cdrvmac.rsc" /D "_MPPC_" /D "_MAC" /D "_DEBUG" /l 0x409\
 /NOLOGO $(SOURCE)


!ELSEIF  "$(CFG)" == "CalcDriv - Win32 Unicode Release"

!ELSEIF  "$(CFG)" == "CalcDriv - Win32 Unicode Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\calcdriv.cpp
DEP_CPP_CALCDR=\
	".\Calcdriv.h"\
	".\Calctype.h"\
	".\Stdafx.h"\
	

!IF  "$(CFG)" == "CalcDriv - Win32 Release"


"$(INTDIR)\calcdriv.obj" : $(SOURCE) $(DEP_CPP_CALCDR) "$(INTDIR)"\
 "$(INTDIR)\Calcdriv.pch"

"$(INTDIR)\vc40.idb" : $(SOURCE) $(DEP_CPP_CALCDR) "$(INTDIR)"\
 "$(INTDIR)\Calcdriv.pch"


!ELSEIF  "$(CFG)" == "CalcDriv - Win32 Debug"


"$(INTDIR)\calcdriv.obj" : $(SOURCE) $(DEP_CPP_CALCDR) "$(INTDIR)"\
 "$(INTDIR)\Calcdriv.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_CALCDR) "$(INTDIR)"\
 "$(INTDIR)\Calcdriv.pch"

"$(INTDIR)\vc40.idb" : $(SOURCE) $(DEP_CPP_CALCDR) "$(INTDIR)"\
 "$(INTDIR)\Calcdriv.pch"


!ELSEIF  "$(CFG)" == "CalcDriv - Macintosh Release"


"$(INTDIR)\calcdriv.obj" : $(SOURCE) $(DEP_CPP_CALCDR) "$(INTDIR)"\
 "$(INTDIR)\Calcdriv.pch"


!ELSEIF  "$(CFG)" == "CalcDriv - Macintosh Debug"


"$(INTDIR)\calcdriv.obj" : $(SOURCE) $(DEP_CPP_CALCDR) "$(INTDIR)"\
 "$(INTDIR)\Calcdriv.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_CALCDR) "$(INTDIR)"\
 "$(INTDIR)\Calcdriv.pch"


!ELSEIF  "$(CFG)" == "CalcDriv - Power Macintosh Release"


"$(INTDIR)\calcdriv.obj" : $(SOURCE) $(DEP_CPP_CALCDR) "$(INTDIR)"\
 "$(INTDIR)\Calcdriv.pch"


!ELSEIF  "$(CFG)" == "CalcDriv - Power Macintosh Debug"


"$(INTDIR)\calcdriv.obj" : $(SOURCE) $(DEP_CPP_CALCDR) "$(INTDIR)"\
 "$(INTDIR)\Calcdriv.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_CALCDR) "$(INTDIR)"\
 "$(INTDIR)\Calcdriv.pch"


!ELSEIF  "$(CFG)" == "CalcDriv - Win32 Unicode Release"


"$(INTDIR)\calcdriv.obj" : $(SOURCE) $(DEP_CPP_CALCDR) "$(INTDIR)"\
 "$(INTDIR)\Calcdriv.pch"

"$(INTDIR)\vc40.idb" : $(SOURCE) $(DEP_CPP_CALCDR) "$(INTDIR)"\
 "$(INTDIR)\Calcdriv.pch"


!ELSEIF  "$(CFG)" == "CalcDriv - Win32 Unicode Debug"


"$(INTDIR)\calcdriv.obj" : $(SOURCE) $(DEP_CPP_CALCDR) "$(INTDIR)"\
 "$(INTDIR)\Calcdriv.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_CALCDR) "$(INTDIR)"\
 "$(INTDIR)\Calcdriv.pch"

"$(INTDIR)\vc40.idb" : $(SOURCE) $(DEP_CPP_CALCDR) "$(INTDIR)"\
 "$(INTDIR)\Calcdriv.pch"


!ENDIF 

# End Source File
# End Target
# End Project
################################################################################
