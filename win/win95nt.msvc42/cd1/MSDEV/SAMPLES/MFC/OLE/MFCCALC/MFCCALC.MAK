# Microsoft Visual C++ Generated NMAKE File, Format Version 40000
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101
# TARGTYPE "Power Macintosh Application" 0x0401
# TARGTYPE "Macintosh Application" 0x0301

!IF "$(CFG)" == ""
CFG=MfcCalc - Win32 Debug
!MESSAGE No configuration specified.  Defaulting to MfcCalc - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "MfcCalc - Win32 Release" && "$(CFG)" !=\
 "MfcCalc - Win32 Debug" && "$(CFG)" != "MfcCalc - Macintosh Release" &&\
 "$(CFG)" != "MfcCalc - Macintosh Debug" && "$(CFG)" !=\
 "MfcCalc - Power Macintosh Release" && "$(CFG)" !=\
 "MfcCalc - Power Macintosh Debug" && "$(CFG)" !=\
 "MfcCalc - Win32 Unicode Release" && "$(CFG)" !=\
 "MfcCalc - Win32 Unicode Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "mfccalc.mak" CFG="MfcCalc - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "MfcCalc - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "MfcCalc - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE "MfcCalc - Macintosh Release" (based on "Macintosh Application")
!MESSAGE "MfcCalc - Macintosh Debug" (based on "Macintosh Application")
!MESSAGE "MfcCalc - Power Macintosh Release" (based on\
 "Power Macintosh Application")
!MESSAGE "MfcCalc - Power Macintosh Debug" (based on\
 "Power Macintosh Application")
!MESSAGE "MfcCalc - Win32 Unicode Release" (based on "Win32 (x86) Application")
!MESSAGE "MfcCalc - Win32 Unicode Debug" (based on "Win32 (x86) Application")
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
# PROP Target_Last_Scanned "MfcCalc - Win32 Unicode Debug"

!IF  "$(CFG)" == "MfcCalc - Win32 Release"

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

ALL : "$(OUTDIR)\mfccalc.exe"

CLEAN : 
	-@erase ".\Release\mfccalc.exe"
	-@erase ".\Release\calcdlg.obj"
	-@erase ".\Release\mfccalc.pch"
	-@erase ".\Release\stdafx.obj"
	-@erase ".\Release\mfccalc.obj"
	-@erase ".\Release\mfccalc.res"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"Stdafx.h" /c
CPP_PROJ=/nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/mfccalc.pch" /Yu"Stdafx.h" /Fo"$(INTDIR)/"\
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
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/mfccalc.res" /d "NDEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/mfccalc.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 /nologo /subsystem:windows /machine:I386
LINK32_FLAGS=/nologo /subsystem:windows /incremental:no\
 /pdb:"$(OUTDIR)/mfccalc.pdb" /machine:I386 /out:"$(OUTDIR)/mfccalc.exe" 
LINK32_OBJS= \
	"$(INTDIR)/calcdlg.obj" \
	"$(INTDIR)/stdafx.obj" \
	"$(INTDIR)/mfccalc.obj" \
	"$(INTDIR)/mfccalc.res"

"$(OUTDIR)\mfccalc.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "MfcCalc - Win32 Debug"

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

ALL : "$(OUTDIR)\mfccalc.exe"

CLEAN : 
	-@erase ".\Debug\vc40.pdb"
	-@erase ".\Debug\mfccalc.pch"
	-@erase ".\Debug\mfccalc.exe"
	-@erase ".\Debug\stdafx.obj"
	-@erase ".\Debug\mfccalc.obj"
	-@erase ".\Debug\calcdlg.obj"
	-@erase ".\Debug\mfccalc.res"
	-@erase ".\Debug\mfccalc.ilk"
	-@erase ".\Debug\mfccalc.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"Stdafx.h" /c
CPP_PROJ=/nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/mfccalc.pch" /Yu"Stdafx.h"\
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
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/mfccalc.res" /d "_DEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/mfccalc.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386
# ADD LINK32 /nologo /subsystem:windows /debug /machine:I386
LINK32_FLAGS=/nologo /subsystem:windows /incremental:yes\
 /pdb:"$(OUTDIR)/mfccalc.pdb" /debug /machine:I386 /out:"$(OUTDIR)/mfccalc.exe" 
LINK32_OBJS= \
	"$(INTDIR)/stdafx.obj" \
	"$(INTDIR)/mfccalc.obj" \
	"$(INTDIR)/calcdlg.obj" \
	"$(INTDIR)/mfccalc.res"

"$(OUTDIR)\mfccalc.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "MfcCalc - Macintosh Release"

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

ALL : "$(OUTDIR)\mfccalc.exe" "$(OUTDIR)\mfccalc.trg"

CLEAN : 
	-@erase ".\MacRel\mfccalc.trg"
	-@erase ".\MacRel\mfccalc.exe"
	-@erase ".\MacRel\calcdlg.obj"
	-@erase ".\MacRel\mfccalc.pch"
	-@erase ".\MacRel\stdafx.obj"
	-@erase ".\MacRel\mfccalc.obj"
	-@erase ".\MacRel\mfccalc.rsc"
	-@erase ".\MacRel\calcmac.rsc"

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
 "WIN32" /D "NDEBUG" /D "_MBCS" /Fp"$(INTDIR)/mfccalc.pch" /Yu"Stdafx.h"\
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
RSC_PROJ=/l 0x409 /r /m /fo"$(INTDIR)/mfccalc.rsc" /d "_MAC" /d "_68K_" /d\
 "NDEBUG" 
MRC=mrc.exe
# ADD BASE MRC /D "_68K_" /D "_MAC" /D "NDEBUG" /NOLOGO
# ADD MRC /D "_68K_" /D "_MAC" /D "NDEBUG" /NOLOGO
MRC_PROJ=/D "_68K_" /D "_MAC" /D "NDEBUG" /l 0x409 /NOLOGO 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/mfccalc.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 wlm.lib /nologo /MAC:bundle /machine:M68K
# ADD LINK32 /nologo /MAC:bundle /MAC:creator="CALC" /machine:M68K
LINK32_FLAGS=/nologo /MAC:bundle /MAC:type="APPL" /MAC:creator="CALC"\
 /pdb:"$(OUTDIR)/mfccalc.pdb" /machine:M68K /out:"$(OUTDIR)/mfccalc.exe" 
LINK32_OBJS= \
	"$(INTDIR)/calcdlg.obj" \
	"$(INTDIR)/stdafx.obj" \
	"$(INTDIR)/mfccalc.obj" \
	"$(INTDIR)/mfccalc.rsc" \
	"$(INTDIR)/calcmac.rsc"

"$(OUTDIR)\mfccalc.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

MFILE32=mfile.exe
# ADD BASE MFILE32 COPY /NOLOGO
# ADD MFILE32 COPY /NOLOGO
MFILE32_FLAGS=COPY /NOLOGO 
MFILE32_FILES= \
	"$(OUTDIR)/mfccalc.exe"

"$(OUTDIR)\mfccalc.trg" : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\MacRel\mfccalc.exe\
 "$(MFILE32_DEST)":mfccalc.exe

DOWNLOAD : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\MacRel\mfccalc.exe\
 "$(MFILE32_DEST)":mfccalc.exe

!ELSEIF  "$(CFG)" == "MfcCalc - Macintosh Debug"

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

ALL : "$(OUTDIR)\mfccalc.exe" "$(OUTDIR)\mfccalc.trg"

CLEAN : 
	-@erase ".\MacDbg\vc40.pdb"
	-@erase ".\MacDbg\mfccalc.pch"
	-@erase ".\MacDbg\mfccalc.pdb"
	-@erase ".\MacDbg\mfccalc.obj"
	-@erase ".\MacDbg\stdafx.obj"
	-@erase ".\MacDbg\calcdlg.obj"
	-@erase ".\MacDbg\mfccalc.rsc"
	-@erase ".\MacDbg\calcmac.rsc"
	-@erase ".\MacDbg\mfccalc.trg"
	-@erase ".\MacDbg\mfccalc.exe"

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
 "_68K_" /D "WIN32" /D "_DEBUG" /D "_MBCS" /Fp"$(INTDIR)/mfccalc.pch"\
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
RSC_PROJ=/l 0x409 /r /m /fo"$(INTDIR)/mfccalc.rsc" /d "_MAC" /d "_68K_" /d\
 "_DEBUG" 
MRC=mrc.exe
# ADD BASE MRC /D "_68K_" /D "_MAC" /D "_DEBUG" /NOLOGO
# ADD MRC /D "_68K_" /D "_MAC" /D "_DEBUG" /NOLOGO
MRC_PROJ=/D "_68K_" /D "_MAC" /D "_DEBUG" /l 0x409 /NOLOGO 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/mfccalc.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 wlm.lib /nologo /MAC:bundle /debug /machine:M68K
# ADD LINK32 /nologo /MAC:bundle /MAC:creator="CALC" /debug /machine:M68K
LINK32_FLAGS=/nologo /MAC:bundle /MAC:type="APPL" /MAC:creator="CALC"\
 /pdb:"$(OUTDIR)/mfccalc.pdb" /debug /machine:M68K /out:"$(OUTDIR)/mfccalc.exe" 
LINK32_OBJS= \
	"$(INTDIR)/mfccalc.obj" \
	"$(INTDIR)/stdafx.obj" \
	"$(INTDIR)/calcdlg.obj" \
	"$(INTDIR)/mfccalc.rsc" \
	"$(INTDIR)/calcmac.rsc"

"$(OUTDIR)\mfccalc.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

MFILE32=mfile.exe
# ADD BASE MFILE32 COPY /NOLOGO
# ADD MFILE32 COPY /NOLOGO
MFILE32_FLAGS=COPY /NOLOGO 
MFILE32_FILES= \
	"$(OUTDIR)/mfccalc.exe"

"$(OUTDIR)\mfccalc.trg" : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\MacDbg\mfccalc.exe\
 "$(MFILE32_DEST)":mfccalc.exe

DOWNLOAD : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\MacDbg\mfccalc.exe\
 "$(MFILE32_DEST)":mfccalc.exe

!ELSEIF  "$(CFG)" == "MfcCalc - Power Macintosh Release"

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

ALL : "$(OUTDIR)\mfccalc.exe" "$(OUTDIR)\mfccalc.trg"

CLEAN : 
	-@erase ".\PMacRel\mfccalc.trg"
	-@erase ".\PMacRel\mfccalc.exe"
	-@erase ".\PMacRel\mfccalc.obj"
	-@erase ".\PMacRel\mfccalc.pch"
	-@erase ".\PMacRel\stdafx.obj"
	-@erase ".\PMacRel\calcdlg.obj"
	-@erase ".\PMacRel\mfccalc.rsc"
	-@erase ".\PMacRel\calcmac.rsc"

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
 "_WINDOWS" /D "WIN32" /D "_MBCS" /D "_AFXDLL" /Fp"$(INTDIR)/mfccalc.pch"\
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
RSC_PROJ=/l 0x409 /r /m /fo"$(INTDIR)/mfccalc.rsc" /d "_MAC" /d "_MPPC_" /d\
 "NDEBUG" 
MRC=mrc.exe
# ADD BASE MRC /D "_MPPC_" /D "_MAC" /D "NDEBUG" /NOLOGO
# ADD MRC /D "_MPPC_" /D "_MAC" /D "NDEBUG" /NOLOGO
MRC_PROJ=/D "_MPPC_" /D "_MAC" /D "NDEBUG" /l 0x409 /NOLOGO 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/mfccalc.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 cfrg.rsc /nologo /mac:bundle /machine:MPPC
# ADD LINK32 /nologo /mac:bundle /mac:creator="CALC" /machine:MPPC
LINK32_FLAGS=/nologo /mac:bundle /mac:type="APPL" /mac:creator="CALC"\
 /incremental:no /pdb:"$(OUTDIR)/mfccalc.pdb" /machine:MPPC\
 /out:"$(OUTDIR)/mfccalc.exe" 
LINK32_OBJS= \
	"$(INTDIR)/mfccalc.obj" \
	"$(INTDIR)/stdafx.obj" \
	"$(INTDIR)/calcdlg.obj" \
	"$(INTDIR)/mfccalc.rsc" \
	"$(INTDIR)/calcmac.rsc"

"$(OUTDIR)\mfccalc.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

MFILE32=mfile.exe
# ADD BASE MFILE32 COPY /NOLOGO
# ADD MFILE32 COPY /NOLOGO
MFILE32_FLAGS=COPY /NOLOGO 
MFILE32_FILES= \
	"$(OUTDIR)/mfccalc.exe"

"$(OUTDIR)\mfccalc.trg" : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\PMacRel\mfccalc.exe\
 "$(MFILE32_DEST)":mfccalc.exe

DOWNLOAD : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\PMacRel\mfccalc.exe\
 "$(MFILE32_DEST)":mfccalc.exe

!ELSEIF  "$(CFG)" == "MfcCalc - Power Macintosh Debug"

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

ALL : "$(OUTDIR)\mfccalc.exe" "$(OUTDIR)\mfccalc.trg"

CLEAN : 
	-@erase ".\PMacDbg\vc40.pdb"
	-@erase ".\PMacDbg\mfccalc.pch"
	-@erase ".\PMacDbg\mfccalc.ilk"
	-@erase ".\PMacDbg\calcdlg.obj"
	-@erase ".\PMacDbg\stdafx.obj"
	-@erase ".\PMacDbg\mfccalc.obj"
	-@erase ".\PMacDbg\mfccalc.rsc"
	-@erase ".\PMacDbg\calcmac.rsc"
	-@erase ".\PMacDbg\mfccalc.pdb"
	-@erase ".\PMacDbg\mfccalc.trg"
	-@erase ".\PMacDbg\mfccalc.exe"

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
 "_WINDOWS" /D "WIN32" /D "_MBCS" /D "_AFXDLL" /Fp"$(INTDIR)/mfccalc.pch"\
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
RSC_PROJ=/l 0x409 /r /m /fo"$(INTDIR)/mfccalc.rsc" /d "_MAC" /d "_MPPC_" /d\
 "_DEBUG" 
MRC=mrc.exe
# ADD BASE MRC /D "_MPPC_" /D "_MAC" /D "_DEBUG" /NOLOGO
# ADD MRC /D "_MPPC_" /D "_MAC" /D "_DEBUG" /NOLOGO
MRC_PROJ=/D "_MPPC_" /D "_MAC" /D "_DEBUG" /l 0x409 /NOLOGO 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/mfccalc.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 cfrg.rsc /nologo /mac:bundle /debug /machine:MPPC
# ADD LINK32 /nologo /mac:bundle /mac:creator="CALC" /debug /machine:MPPC
LINK32_FLAGS=/nologo /mac:bundle /mac:type="APPL" /mac:creator="CALC"\
 /incremental:yes /pdb:"$(OUTDIR)/mfccalc.pdb" /debug /machine:MPPC\
 /out:"$(OUTDIR)/mfccalc.exe" 
LINK32_OBJS= \
	"$(INTDIR)/calcdlg.obj" \
	"$(INTDIR)/stdafx.obj" \
	"$(INTDIR)/mfccalc.obj" \
	"$(INTDIR)/mfccalc.rsc" \
	"$(INTDIR)/calcmac.rsc"

"$(OUTDIR)\mfccalc.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

MFILE32=mfile.exe
# ADD BASE MFILE32 COPY /NOLOGO
# ADD MFILE32 COPY /NOLOGO
MFILE32_FLAGS=COPY /NOLOGO 
MFILE32_FILES= \
	"$(OUTDIR)/mfccalc.exe"

"$(OUTDIR)\mfccalc.trg" : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\PMacDbg\mfccalc.exe\
 "$(MFILE32_DEST)":mfccalc.exe

DOWNLOAD : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\PMacDbg\mfccalc.exe\
 "$(MFILE32_DEST)":mfccalc.exe

!ELSEIF  "$(CFG)" == "MfcCalc - Win32 Unicode Release"

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

ALL : "$(OUTDIR)\mfccalc.exe"

CLEAN : 
	-@erase ".\UniRelease\mfccalc.exe"
	-@erase ".\UniRelease\calcdlg.obj"
	-@erase ".\UniRelease\mfccalc.pch"
	-@erase ".\UniRelease\stdafx.obj"
	-@erase ".\UniRelease\mfccalc.obj"
	-@erase ".\UniRelease\mfccalc.res"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_UNICODE" /D "_AFXDLL" /D "_MBCS" /Yu"Stdafx.h" /c
CPP_PROJ=/nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D\
 "_UNICODE" /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/mfccalc.pch" /Yu"Stdafx.h"\
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
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/mfccalc.res" /d "NDEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/mfccalc.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 /nologo /entry:"wWinMainCRTStartup" /subsystem:windows /machine:I386
LINK32_FLAGS=/nologo /entry:"wWinMainCRTStartup" /subsystem:windows\
 /incremental:no /pdb:"$(OUTDIR)/mfccalc.pdb" /machine:I386\
 /out:"$(OUTDIR)/mfccalc.exe" 
LINK32_OBJS= \
	"$(INTDIR)/calcdlg.obj" \
	"$(INTDIR)/stdafx.obj" \
	"$(INTDIR)/mfccalc.obj" \
	"$(INTDIR)/mfccalc.res"

"$(OUTDIR)\mfccalc.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "MfcCalc - Win32 Unicode Debug"

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

ALL : "$(OUTDIR)\mfccalc.exe"

CLEAN : 
	-@erase ".\UniDebug\vc40.pdb"
	-@erase ".\UniDebug\mfccalc.pch"
	-@erase ".\UniDebug\mfccalc.exe"
	-@erase ".\UniDebug\stdafx.obj"
	-@erase ".\UniDebug\mfccalc.obj"
	-@erase ".\UniDebug\calcdlg.obj"
	-@erase ".\UniDebug\mfccalc.res"
	-@erase ".\UniDebug\mfccalc.ilk"
	-@erase ".\UniDebug\mfccalc.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_UNICODE" /D "_AFXDLL" /D "_MBCS" /Yu"Stdafx.h" /c
CPP_PROJ=/nologo /MDd /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS"\
 /D "_UNICODE" /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/mfccalc.pch" /Yu"Stdafx.h"\
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
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/mfccalc.res" /d "_DEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/mfccalc.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386
# ADD LINK32 /nologo /entry:"wWinMainCRTStartup" /subsystem:windows /debug /machine:I386
LINK32_FLAGS=/nologo /entry:"wWinMainCRTStartup" /subsystem:windows\
 /incremental:yes /pdb:"$(OUTDIR)/mfccalc.pdb" /debug /machine:I386\
 /out:"$(OUTDIR)/mfccalc.exe" 
LINK32_OBJS= \
	"$(INTDIR)/stdafx.obj" \
	"$(INTDIR)/mfccalc.obj" \
	"$(INTDIR)/calcdlg.obj" \
	"$(INTDIR)/mfccalc.res"

"$(OUTDIR)\mfccalc.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 

################################################################################
# Begin Target

# Name "MfcCalc - Win32 Release"
# Name "MfcCalc - Win32 Debug"
# Name "MfcCalc - Macintosh Release"
# Name "MfcCalc - Macintosh Debug"
# Name "MfcCalc - Power Macintosh Release"
# Name "MfcCalc - Power Macintosh Debug"
# Name "MfcCalc - Win32 Unicode Release"
# Name "MfcCalc - Win32 Unicode Debug"

!IF  "$(CFG)" == "MfcCalc - Win32 Release"

!ELSEIF  "$(CFG)" == "MfcCalc - Win32 Debug"

!ELSEIF  "$(CFG)" == "MfcCalc - Macintosh Release"

!ELSEIF  "$(CFG)" == "MfcCalc - Macintosh Debug"

!ELSEIF  "$(CFG)" == "MfcCalc - Power Macintosh Release"

!ELSEIF  "$(CFG)" == "MfcCalc - Power Macintosh Debug"

!ELSEIF  "$(CFG)" == "MfcCalc - Win32 Unicode Release"

!ELSEIF  "$(CFG)" == "MfcCalc - Win32 Unicode Debug"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\stdafx.cpp
DEP_CPP_STDAF=\
	".\stdafx.h"\
	

!IF  "$(CFG)" == "MfcCalc - Win32 Release"

# ADD CPP /Yc

BuildCmds= \
	$(CPP) /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/mfccalc.pch" /Yc /Fo"$(INTDIR)/" /c\
 $(SOURCE) \
	

"$(INTDIR)\stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\mfccalc.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "MfcCalc - Win32 Debug"

# ADD CPP /Yc

BuildCmds= \
	$(CPP) /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/mfccalc.pch" /Yc /Fo"$(INTDIR)/"\
 /Fd"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\mfccalc.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "MfcCalc - Macintosh Release"

# ADD CPP /Yc

BuildCmds= \
	$(CPP) /nologo /AL /Q68s /W3 /GX /O2 /D "_WINDOWS" /D "_MAC" /D "_68K_" /D\
 "WIN32" /D "NDEBUG" /D "_MBCS" /Fp"$(INTDIR)/mfccalc.pch" /Yc /Fo"$(INTDIR)/"\
 /c $(SOURCE) \
	

"$(INTDIR)\stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\mfccalc.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "MfcCalc - Macintosh Debug"

# ADD CPP /Yc

BuildCmds= \
	$(CPP) /nologo /AL /Q68s /Q68m /W3 /GX /Zi /Od /D "_WINDOWS" /D "_MAC" /D\
 "_68K_" /D "WIN32" /D "_DEBUG" /D "_MBCS" /Fp"$(INTDIR)/mfccalc.pch" /Yc\
 /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\mfccalc.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "MfcCalc - Power Macintosh Release"

# ADD CPP /Yc

BuildCmds= \
	$(CPP) /nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "_MAC" /D "_MPPC_" /D "_WINDOWS"\
 /D "WIN32" /D "_MBCS" /D "_AFXDLL" /Fp"$(INTDIR)/mfccalc.pch" /Yc\
 /Fo"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\mfccalc.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "MfcCalc - Power Macintosh Debug"

# ADD CPP /Yc

BuildCmds= \
	$(CPP) /nologo /MDd /W3 /GX /Zi /Od /D "_DEBUG" /D "_MAC" /D "_MPPC_" /D\
 "_WINDOWS" /D "WIN32" /D "_MBCS" /D "_AFXDLL" /Fp"$(INTDIR)/mfccalc.pch" /Yc\
 /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\mfccalc.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "MfcCalc - Win32 Unicode Release"

# ADD CPP /Yc

BuildCmds= \
	$(CPP) /nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D\
 "_UNICODE" /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/mfccalc.pch" /Yc\
 /Fo"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\mfccalc.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "MfcCalc - Win32 Unicode Debug"

# ADD CPP /Yc

BuildCmds= \
	$(CPP) /nologo /MDd /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS"\
 /D "_UNICODE" /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/mfccalc.pch" /Yc\
 /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\mfccalc.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\calcmac.r

!IF  "$(CFG)" == "MfcCalc - Win32 Release"

!ELSEIF  "$(CFG)" == "MfcCalc - Win32 Debug"

!ELSEIF  "$(CFG)" == "MfcCalc - Macintosh Release"


"$(INTDIR)\calcmac.rsc" : $(SOURCE) "$(INTDIR)"
   $(MRC) /o"$(INTDIR)/calcmac.rsc" /D "_68K_" /D "_MAC" /D "NDEBUG" /l 0x409\
 /NOLOGO $(SOURCE)


!ELSEIF  "$(CFG)" == "MfcCalc - Macintosh Debug"


"$(INTDIR)\calcmac.rsc" : $(SOURCE) "$(INTDIR)"
   $(MRC) /o"$(INTDIR)/calcmac.rsc" /D "_68K_" /D "_MAC" /D "_DEBUG" /l 0x409\
 /NOLOGO $(SOURCE)


!ELSEIF  "$(CFG)" == "MfcCalc - Power Macintosh Release"


"$(INTDIR)\calcmac.rsc" : $(SOURCE) "$(INTDIR)"
   $(MRC) /o"$(INTDIR)/calcmac.rsc" /D "_MPPC_" /D "_MAC" /D "NDEBUG" /l 0x409\
 /NOLOGO $(SOURCE)


!ELSEIF  "$(CFG)" == "MfcCalc - Power Macintosh Debug"


"$(INTDIR)\calcmac.rsc" : $(SOURCE) "$(INTDIR)"
   $(MRC) /o"$(INTDIR)/calcmac.rsc" /D "_MPPC_" /D "_MAC" /D "_DEBUG" /l 0x409\
 /NOLOGO $(SOURCE)


!ELSEIF  "$(CFG)" == "MfcCalc - Win32 Unicode Release"

!ELSEIF  "$(CFG)" == "MfcCalc - Win32 Unicode Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\mfccalc.cpp

!IF  "$(CFG)" == "MfcCalc - Win32 Release"

DEP_CPP_MFCCA=\
	".\calcdlg.h"\
	".\mfccalc.h"\
	".\stdafx.h"\
	

"$(INTDIR)\mfccalc.obj" : $(SOURCE) $(DEP_CPP_MFCCA) "$(INTDIR)"\
 "$(INTDIR)\mfccalc.pch"


!ELSEIF  "$(CFG)" == "MfcCalc - Win32 Debug"

DEP_CPP_MFCCA=\
	".\stdafx.h"\
	".\mfccalc.h"\
	".\calcdlg.h"\
	

"$(INTDIR)\mfccalc.obj" : $(SOURCE) $(DEP_CPP_MFCCA) "$(INTDIR)"\
 "$(INTDIR)\mfccalc.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_MFCCA) "$(INTDIR)"\
 "$(INTDIR)\mfccalc.pch"


!ELSEIF  "$(CFG)" == "MfcCalc - Macintosh Release"

DEP_CPP_MFCCA=\
	".\stdafx.h"\
	".\mfccalc.h"\
	".\calcdlg.h"\
	

"$(INTDIR)\mfccalc.obj" : $(SOURCE) $(DEP_CPP_MFCCA) "$(INTDIR)"\
 "$(INTDIR)\mfccalc.pch"


!ELSEIF  "$(CFG)" == "MfcCalc - Macintosh Debug"

DEP_CPP_MFCCA=\
	".\calcdlg.h"\
	".\mfccalc.h"\
	".\stdafx.h"\
	

"$(INTDIR)\mfccalc.obj" : $(SOURCE) $(DEP_CPP_MFCCA) "$(INTDIR)"\
 "$(INTDIR)\mfccalc.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_MFCCA) "$(INTDIR)"\
 "$(INTDIR)\mfccalc.pch"


!ELSEIF  "$(CFG)" == "MfcCalc - Power Macintosh Release"

DEP_CPP_MFCCA=\
	".\calcdlg.h"\
	".\mfccalc.h"\
	".\stdafx.h"\
	

"$(INTDIR)\mfccalc.obj" : $(SOURCE) $(DEP_CPP_MFCCA) "$(INTDIR)"\
 "$(INTDIR)\mfccalc.pch"


!ELSEIF  "$(CFG)" == "MfcCalc - Power Macintosh Debug"

DEP_CPP_MFCCA=\
	".\calcdlg.h"\
	".\mfccalc.h"\
	".\stdafx.h"\
	

"$(INTDIR)\mfccalc.obj" : $(SOURCE) $(DEP_CPP_MFCCA) "$(INTDIR)"\
 "$(INTDIR)\mfccalc.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_MFCCA) "$(INTDIR)"\
 "$(INTDIR)\mfccalc.pch"


!ELSEIF  "$(CFG)" == "MfcCalc - Win32 Unicode Release"

DEP_CPP_MFCCA=\
	".\calcdlg.h"\
	".\mfccalc.h"\
	".\stdafx.h"\
	

"$(INTDIR)\mfccalc.obj" : $(SOURCE) $(DEP_CPP_MFCCA) "$(INTDIR)"\
 "$(INTDIR)\mfccalc.pch"


!ELSEIF  "$(CFG)" == "MfcCalc - Win32 Unicode Debug"

DEP_CPP_MFCCA=\
	".\stdafx.h"\
	".\mfccalc.h"\
	".\calcdlg.h"\
	

"$(INTDIR)\mfccalc.obj" : $(SOURCE) $(DEP_CPP_MFCCA) "$(INTDIR)"\
 "$(INTDIR)\mfccalc.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_MFCCA) "$(INTDIR)"\
 "$(INTDIR)\mfccalc.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\mfccalc.rc
DEP_RSC_MFCCAL=\
	".\res\mfccalc.ico"\
	".\res\mfccalc.rc2"\
	

!IF  "$(CFG)" == "MfcCalc - Win32 Release"


"$(INTDIR)\mfccalc.res" : $(SOURCE) $(DEP_RSC_MFCCAL) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "MfcCalc - Win32 Debug"


"$(INTDIR)\mfccalc.res" : $(SOURCE) $(DEP_RSC_MFCCAL) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "MfcCalc - Macintosh Release"


"$(INTDIR)\mfccalc.rsc" : $(SOURCE) $(DEP_RSC_MFCCAL) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "MfcCalc - Macintosh Debug"


"$(INTDIR)\mfccalc.rsc" : $(SOURCE) $(DEP_RSC_MFCCAL) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "MfcCalc - Power Macintosh Release"


"$(INTDIR)\mfccalc.rsc" : $(SOURCE) $(DEP_RSC_MFCCAL) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "MfcCalc - Power Macintosh Debug"


"$(INTDIR)\mfccalc.rsc" : $(SOURCE) $(DEP_RSC_MFCCAL) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "MfcCalc - Win32 Unicode Release"


"$(INTDIR)\mfccalc.res" : $(SOURCE) $(DEP_RSC_MFCCAL) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "MfcCalc - Win32 Unicode Debug"


"$(INTDIR)\mfccalc.res" : $(SOURCE) $(DEP_RSC_MFCCAL) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\calcdlg.cpp

!IF  "$(CFG)" == "MfcCalc - Win32 Release"

DEP_CPP_CALCD=\
	".\calcdlg.h"\
	".\mfccalc.h"\
	".\stdafx.h"\
	

"$(INTDIR)\calcdlg.obj" : $(SOURCE) $(DEP_CPP_CALCD) "$(INTDIR)"\
 "$(INTDIR)\mfccalc.pch"


!ELSEIF  "$(CFG)" == "MfcCalc - Win32 Debug"

DEP_CPP_CALCD=\
	".\stdafx.h"\
	".\mfccalc.h"\
	".\calcdlg.h"\
	

"$(INTDIR)\calcdlg.obj" : $(SOURCE) $(DEP_CPP_CALCD) "$(INTDIR)"\
 "$(INTDIR)\mfccalc.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_CALCD) "$(INTDIR)"\
 "$(INTDIR)\mfccalc.pch"


!ELSEIF  "$(CFG)" == "MfcCalc - Macintosh Release"

DEP_CPP_CALCD=\
	".\stdafx.h"\
	".\mfccalc.h"\
	".\calcdlg.h"\
	

"$(INTDIR)\calcdlg.obj" : $(SOURCE) $(DEP_CPP_CALCD) "$(INTDIR)"\
 "$(INTDIR)\mfccalc.pch"


!ELSEIF  "$(CFG)" == "MfcCalc - Macintosh Debug"

DEP_CPP_CALCD=\
	".\calcdlg.h"\
	".\mfccalc.h"\
	".\stdafx.h"\
	

"$(INTDIR)\calcdlg.obj" : $(SOURCE) $(DEP_CPP_CALCD) "$(INTDIR)"\
 "$(INTDIR)\mfccalc.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_CALCD) "$(INTDIR)"\
 "$(INTDIR)\mfccalc.pch"


!ELSEIF  "$(CFG)" == "MfcCalc - Power Macintosh Release"

DEP_CPP_CALCD=\
	".\calcdlg.h"\
	".\mfccalc.h"\
	".\stdafx.h"\
	

"$(INTDIR)\calcdlg.obj" : $(SOURCE) $(DEP_CPP_CALCD) "$(INTDIR)"\
 "$(INTDIR)\mfccalc.pch"


!ELSEIF  "$(CFG)" == "MfcCalc - Power Macintosh Debug"

DEP_CPP_CALCD=\
	".\calcdlg.h"\
	".\mfccalc.h"\
	".\stdafx.h"\
	

"$(INTDIR)\calcdlg.obj" : $(SOURCE) $(DEP_CPP_CALCD) "$(INTDIR)"\
 "$(INTDIR)\mfccalc.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_CALCD) "$(INTDIR)"\
 "$(INTDIR)\mfccalc.pch"


!ELSEIF  "$(CFG)" == "MfcCalc - Win32 Unicode Release"

DEP_CPP_CALCD=\
	".\calcdlg.h"\
	".\mfccalc.h"\
	".\stdafx.h"\
	

"$(INTDIR)\calcdlg.obj" : $(SOURCE) $(DEP_CPP_CALCD) "$(INTDIR)"\
 "$(INTDIR)\mfccalc.pch"


!ELSEIF  "$(CFG)" == "MfcCalc - Win32 Unicode Debug"

DEP_CPP_CALCD=\
	".\stdafx.h"\
	".\mfccalc.h"\
	".\calcdlg.h"\
	

"$(INTDIR)\calcdlg.obj" : $(SOURCE) $(DEP_CPP_CALCD) "$(INTDIR)"\
 "$(INTDIR)\mfccalc.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_CALCD) "$(INTDIR)"\
 "$(INTDIR)\mfccalc.pch"


!ENDIF 

# End Source File
# End Target
# End Project
################################################################################
