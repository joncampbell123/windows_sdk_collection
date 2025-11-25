# Microsoft Visual C++ Generated NMAKE File, Format Version 40000
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101
# TARGTYPE "Power Macintosh Application" 0x0401
# TARGTYPE "Macintosh Application" 0x0301

!IF "$(CFG)" == ""
CFG=stdreg - Win32 Debug
!MESSAGE No configuration specified.  Defaulting to stdreg - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "stdreg - Macintosh Release" && "$(CFG)" !=\
 "stdreg - Macintosh Debug" && "$(CFG)" != "stdreg - Win32 Release" && "$(CFG)"\
 != "stdreg - Win32 Debug" && "$(CFG)" != "stdreg - Power Macintosh Release" &&\
 "$(CFG)" != "stdreg - Power Macintosh Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "stdreg.mak" CFG="stdreg - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "stdreg - Macintosh Release" (based on "Macintosh Application")
!MESSAGE "stdreg - Macintosh Debug" (based on "Macintosh Application")
!MESSAGE "stdreg - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "stdreg - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE "stdreg - Power Macintosh Release" (based on\
 "Power Macintosh Application")
!MESSAGE "stdreg - Power Macintosh Debug" (based on\
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
# PROP Target_Last_Scanned "stdreg - Win32 Debug"

!IF  "$(CFG)" == "stdreg - Macintosh Release"

# PROP BASE Use_MFC 5
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir MacRel
# PROP BASE Intermediate_Dir MacRel
# PROP BASE Target_Dir
# PROP Use_MFC 5
# PROP Use_Debug_Libraries 0
# PROP Output_Dir MacRel
# PROP Intermediate_Dir MacRel
# PROP Target_Dir
OUTDIR=.\MacRel
INTDIR=.\MacRel

ALL : "$(OUTDIR)\stdreg.exe" "$(OUTDIR)\stdreg.trg"

CLEAN : 
	-@erase ".\MacRel\stdreg.trg"
	-@erase ".\MacRel\stdreg.exe"
	-@erase ".\MacRel\stdafx.obj"
	-@erase ".\MacRel\sectset.obj"
	-@erase ".\MacRel\stdreg.pch"
	-@erase ".\MacRel\instrset.obj"
	-@erase ".\MacRel\enrolset.obj"
	-@erase ".\MacRel\dsectset.obj"
	-@erase ".\MacRel\dialog.obj"
	-@erase ".\MacRel\coursset.obj"
	-@erase ".\MacRel\columdlg.obj"
	-@erase ".\MacRel\typeinfo.obj"
	-@erase ".\MacRel\stdset.obj"
	-@erase ".\MacRel\stdreg.obj"
	-@erase ".\MacRel\stdrMac.rsc"
	-@erase ".\MacRel\stdreg.rsc"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

MTL=mktyplib.exe
# ADD BASE MTL /nologo /D "NDEBUG" /mac
# ADD MTL /nologo /D "NDEBUG" /mac
MTL_PROJ=/nologo /D "NDEBUG" /mac 
CPP=cl.exe
# ADD BASE CPP /nologo /AL /Q68s /W3 /GX /O2 /D "_WINDOWS" /D "_MAC" /D "_68K_" /D "WIN32" /D "NDEBUG" /D "_MBCS" /Yu"stdafx.h" /c
# ADD CPP /nologo /AL /Q68s /W3 /GX /O2 /D "_WINDOWS" /D "_MAC" /D "_68K_" /D "WIN32" /D "NDEBUG" /D "_MBCS" /Yu"stdafx.h" /c
CPP_PROJ=/nologo /AL /Q68s /W3 /GX /O2 /D "_WINDOWS" /D "_MAC" /D "_68K_" /D\
 "WIN32" /D "NDEBUG" /D "_MBCS" /Fp"$(INTDIR)/stdreg.pch" /Yu"stdafx.h"\
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
RSC_PROJ=/l 0x409 /r /m /fo"$(INTDIR)/stdreg.rsc" /d "_MAC" /d "_68K_" /d\
 "NDEBUG" 
MRC=mrc.exe
# ADD BASE MRC /D "_68K_" /D "_MAC" /D "NDEBUG" /NOLOGO
# ADD MRC /D "_68K_" /D "_MAC" /D "NDEBUG" /NOLOGO
MRC_PROJ=/D "_68K_" /D "_MAC" /D "NDEBUG" /l 0x409 /NOLOGO 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/stdreg.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 /nologo /MAC:bundle /machine:M68K
# ADD LINK32 /nologo /MAC:bundle /MAC:creator="STDR" /machine:M68K
LINK32_FLAGS=/nologo /MAC:bundle /MAC:type="APPL" /MAC:creator="STDR"\
 /pdb:"$(OUTDIR)/stdreg.pdb" /machine:M68K /out:"$(OUTDIR)/stdreg.exe" 
LINK32_OBJS= \
	"$(INTDIR)/stdafx.obj" \
	"$(INTDIR)/sectset.obj" \
	"$(INTDIR)/instrset.obj" \
	"$(INTDIR)/enrolset.obj" \
	"$(INTDIR)/dsectset.obj" \
	"$(INTDIR)/dialog.obj" \
	"$(INTDIR)/coursset.obj" \
	"$(INTDIR)/columdlg.obj" \
	"$(INTDIR)/typeinfo.obj" \
	"$(INTDIR)/stdset.obj" \
	"$(INTDIR)/stdreg.obj" \
	"$(INTDIR)/stdrMac.rsc" \
	"$(INTDIR)/stdreg.rsc"

"$(OUTDIR)\stdreg.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

MFILE32=mfile.exe
# ADD BASE MFILE32 COPY /NOLOGO
# ADD MFILE32 COPY /NOLOGO
MFILE32_FLAGS=COPY /NOLOGO 
MFILE32_FILES= \
	"$(OUTDIR)/stdreg.exe"

"$(OUTDIR)\stdreg.trg" : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\MacRel\stdreg.exe "$(MFILE32_DEST)"

DOWNLOAD : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\MacRel\stdreg.exe "$(MFILE32_DEST)"

!ELSEIF  "$(CFG)" == "stdreg - Macintosh Debug"

# PROP BASE Use_MFC 5
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir MacDbg
# PROP BASE Intermediate_Dir MacDbg
# PROP BASE Target_Dir
# PROP Use_MFC 5
# PROP Use_Debug_Libraries 1
# PROP Output_Dir MacDbg
# PROP Intermediate_Dir MacDbg
# PROP Target_Dir
OUTDIR=.\MacDbg
INTDIR=.\MacDbg

ALL : "$(OUTDIR)\stdreg.exe" "$(OUTDIR)\stdreg.trg"

CLEAN : 
	-@erase ".\MacDbg\vc40.pdb"
	-@erase ".\MacDbg\stdreg.pch"
	-@erase ".\MacDbg\stdreg.pdb"
	-@erase ".\MacDbg\stdafx.obj"
	-@erase ".\MacDbg\sectset.obj"
	-@erase ".\MacDbg\instrset.obj"
	-@erase ".\MacDbg\enrolset.obj"
	-@erase ".\MacDbg\dsectset.obj"
	-@erase ".\MacDbg\dialog.obj"
	-@erase ".\MacDbg\coursset.obj"
	-@erase ".\MacDbg\columdlg.obj"
	-@erase ".\MacDbg\typeinfo.obj"
	-@erase ".\MacDbg\stdset.obj"
	-@erase ".\MacDbg\stdreg.obj"
	-@erase ".\MacDbg\stdrMac.rsc"
	-@erase ".\MacDbg\stdreg.rsc"
	-@erase ".\MacDbg\stdreg.trg"
	-@erase ".\MacDbg\stdreg.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

MTL=mktyplib.exe
# ADD BASE MTL /nologo /D "_DEBUG" /mac
# ADD MTL /nologo /D "_DEBUG" /mac
MTL_PROJ=/nologo /D "_DEBUG" /mac 
CPP=cl.exe
# ADD BASE CPP /nologo /AL /Q68s /Q68m /W3 /GX /Zi /Od /D "_WINDOWS" /D "_MAC" /D "_68K_" /D "WIN32" /D "_DEBUG" /D "_MBCS" /Yu"stdafx.h" /c
# ADD CPP /nologo /AL /Q68s /Q68m /W3 /GX /Zi /Od /D "_WINDOWS" /D "_MAC" /D "_68K_" /D "WIN32" /D "_DEBUG" /D "_MBCS" /Yu"stdafx.h" /c
CPP_PROJ=/nologo /AL /Q68s /Q68m /W3 /GX /Zi /Od /D "_WINDOWS" /D "_MAC" /D\
 "_68K_" /D "WIN32" /D "_DEBUG" /D "_MBCS" /Fp"$(INTDIR)/stdreg.pch"\
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
RSC_PROJ=/l 0x409 /r /m /fo"$(INTDIR)/stdreg.rsc" /d "_MAC" /d "_68K_" /d\
 "_DEBUG" 
MRC=mrc.exe
# ADD BASE MRC /D "_68K_" /D "_MAC" /D "_DEBUG" /NOLOGO
# ADD MRC /D "_68K_" /D "_MAC" /D "_DEBUG" /NOLOGO
MRC_PROJ=/D "_68K_" /D "_MAC" /D "_DEBUG" /l 0x409 /NOLOGO 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/stdreg.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 /nologo /MAC:bundle /debug /machine:M68K
# ADD LINK32 /nologo /MAC:bundle /MAC:creator="STDR" /debug /machine:M68K
LINK32_FLAGS=/nologo /MAC:bundle /MAC:type="APPL" /MAC:creator="STDR"\
 /pdb:"$(OUTDIR)/stdreg.pdb" /debug /machine:M68K /out:"$(OUTDIR)/stdreg.exe" 
LINK32_OBJS= \
	"$(INTDIR)/stdafx.obj" \
	"$(INTDIR)/sectset.obj" \
	"$(INTDIR)/instrset.obj" \
	"$(INTDIR)/enrolset.obj" \
	"$(INTDIR)/dsectset.obj" \
	"$(INTDIR)/dialog.obj" \
	"$(INTDIR)/coursset.obj" \
	"$(INTDIR)/columdlg.obj" \
	"$(INTDIR)/typeinfo.obj" \
	"$(INTDIR)/stdset.obj" \
	"$(INTDIR)/stdreg.obj" \
	"$(INTDIR)/stdrMac.rsc" \
	"$(INTDIR)/stdreg.rsc"

"$(OUTDIR)\stdreg.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

MFILE32=mfile.exe
# ADD BASE MFILE32 COPY /NOLOGO
# ADD MFILE32 COPY /NOLOGO
MFILE32_FLAGS=COPY /NOLOGO 
MFILE32_FILES= \
	"$(OUTDIR)/stdreg.exe"

"$(OUTDIR)\stdreg.trg" : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\MacDbg\stdreg.exe "$(MFILE32_DEST)"

DOWNLOAD : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\MacDbg\stdreg.exe "$(MFILE32_DEST)"

!ELSEIF  "$(CFG)" == "stdreg - Win32 Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir Release
# PROP BASE Intermediate_Dir Release
# PROP BASE Target_Dir
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir Release
# PROP Intermediate_Dir Release
# PROP Target_Dir
OUTDIR=.\Release
INTDIR=.\Release

ALL : "$(OUTDIR)\stdreg.exe"

CLEAN : 
	-@erase ".\Release\stdreg.exe"
	-@erase ".\Release\stdafx.obj"
	-@erase ".\Release\sectset.obj"
	-@erase ".\Release\stdreg.pch"
	-@erase ".\Release\instrset.obj"
	-@erase ".\Release\enrolset.obj"
	-@erase ".\Release\dsectset.obj"
	-@erase ".\Release\dialog.obj"
	-@erase ".\Release\coursset.obj"
	-@erase ".\Release\columdlg.obj"
	-@erase ".\Release\typeinfo.obj"
	-@erase ".\Release\stdset.obj"
	-@erase ".\Release\stdreg.obj"
	-@erase ".\Release\stdreg.res"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
CPP_PROJ=/nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/stdreg.pch" /Yu"stdafx.h" /Fo"$(INTDIR)/" /c\
 
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
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/stdreg.res" /d "NDEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/stdreg.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 /nologo /subsystem:windows /machine:I386
LINK32_FLAGS=/nologo /subsystem:windows /incremental:no\
 /pdb:"$(OUTDIR)/stdreg.pdb" /machine:I386 /out:"$(OUTDIR)/stdreg.exe" 
LINK32_OBJS= \
	"$(INTDIR)/stdafx.obj" \
	"$(INTDIR)/sectset.obj" \
	"$(INTDIR)/instrset.obj" \
	"$(INTDIR)/enrolset.obj" \
	"$(INTDIR)/dsectset.obj" \
	"$(INTDIR)/dialog.obj" \
	"$(INTDIR)/coursset.obj" \
	"$(INTDIR)/columdlg.obj" \
	"$(INTDIR)/typeinfo.obj" \
	"$(INTDIR)/stdset.obj" \
	"$(INTDIR)/stdreg.obj" \
	"$(INTDIR)/stdreg.res"

"$(OUTDIR)\stdreg.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "stdreg - Win32 Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir Debug
# PROP BASE Intermediate_Dir Debug
# PROP BASE Target_Dir
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir Debug
# PROP Intermediate_Dir Debug
# PROP Target_Dir
OUTDIR=.\Debug
INTDIR=.\Debug

ALL : "$(OUTDIR)\stdreg.exe"

CLEAN : 
	-@erase ".\Debug\vc40.pdb"
	-@erase ".\Debug\stdreg.pch"
	-@erase ".\Debug\stdreg.exe"
	-@erase ".\Debug\stdafx.obj"
	-@erase ".\Debug\sectset.obj"
	-@erase ".\Debug\instrset.obj"
	-@erase ".\Debug\enrolset.obj"
	-@erase ".\Debug\dsectset.obj"
	-@erase ".\Debug\dialog.obj"
	-@erase ".\Debug\coursset.obj"
	-@erase ".\Debug\columdlg.obj"
	-@erase ".\Debug\typeinfo.obj"
	-@erase ".\Debug\stdset.obj"
	-@erase ".\Debug\stdreg.obj"
	-@erase ".\Debug\stdreg.res"
	-@erase ".\Debug\stdreg.ilk"
	-@erase ".\Debug\stdreg.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
CPP_PROJ=/nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/stdreg.pch" /Yu"stdafx.h" /Fo"$(INTDIR)/"\
 /Fd"$(INTDIR)/" /c 
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
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/stdreg.res" /d "_DEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/stdreg.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386
# ADD LINK32 /nologo /subsystem:windows /debug /machine:I386
LINK32_FLAGS=/nologo /subsystem:windows /incremental:yes\
 /pdb:"$(OUTDIR)/stdreg.pdb" /debug /machine:I386 /out:"$(OUTDIR)/stdreg.exe" 
LINK32_OBJS= \
	"$(INTDIR)/stdafx.obj" \
	"$(INTDIR)/sectset.obj" \
	"$(INTDIR)/instrset.obj" \
	"$(INTDIR)/enrolset.obj" \
	"$(INTDIR)/dsectset.obj" \
	"$(INTDIR)/dialog.obj" \
	"$(INTDIR)/coursset.obj" \
	"$(INTDIR)/columdlg.obj" \
	"$(INTDIR)/typeinfo.obj" \
	"$(INTDIR)/stdset.obj" \
	"$(INTDIR)/stdreg.obj" \
	"$(INTDIR)/stdreg.res"

"$(OUTDIR)\stdreg.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "stdreg - Power Macintosh Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir PMacRel
# PROP BASE Intermediate_Dir PMacRel
# PROP BASE Target_Dir
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir PMacRel
# PROP Intermediate_Dir PMacRel
# PROP Target_Dir
OUTDIR=.\PMacRel
INTDIR=.\PMacRel

ALL : "$(OUTDIR)\stdreg.exe" "$(OUTDIR)\stdreg.trg"

CLEAN : 
	-@erase ".\PMacRel\stdreg.trg"
	-@erase ".\PMacRel\stdreg.exe"
	-@erase ".\PMacRel\stdafx.obj"
	-@erase ".\PMacRel\sectset.obj"
	-@erase ".\PMacRel\stdreg.pch"
	-@erase ".\PMacRel\instrset.obj"
	-@erase ".\PMacRel\enrolset.obj"
	-@erase ".\PMacRel\dsectset.obj"
	-@erase ".\PMacRel\dialog.obj"
	-@erase ".\PMacRel\coursset.obj"
	-@erase ".\PMacRel\columdlg.obj"
	-@erase ".\PMacRel\typeinfo.obj"
	-@erase ".\PMacRel\stdset.obj"
	-@erase ".\PMacRel\stdreg.obj"
	-@erase ".\PMacRel\stdrMac.rsc"
	-@erase ".\PMacRel\stdreg.rsc"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

MTL=mktyplib.exe
# ADD BASE MTL /nologo /D "NDEBUG"
# ADD MTL /nologo /D "NDEBUG"
MTL_PROJ=/nologo /D "NDEBUG" 
CPP=cl.exe
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "_MAC" /D "_MPPC_" /D "_WINDOWS" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_AFXDLL" /Yu"stdafx.h" /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "_MAC" /D "_MPPC_" /D "_WINDOWS" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_AFXDLL" /Yu"stdafx.h" /c
CPP_PROJ=/nologo /MD /W3 /GX /O2 /D "_MAC" /D "_MPPC_" /D "_WINDOWS" /D "WIN32"\
 /D "NDEBUG" /D "_MBCS" /D "_AFXDLL" /Fp"$(INTDIR)/stdreg.pch" /Yu"stdafx.h"\
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
# ADD BASE RSC /l 0x409 /r /d "_MAC" /d "_MPPC_" /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /r /d "_MAC" /d "_MPPC_" /d "NDEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /r /m /fo"$(INTDIR)/stdreg.rsc" /d "_MAC" /d "_MPPC_" /d\
 "NDEBUG" /d "_AFXDLL" 
MRC=mrc.exe
# ADD BASE MRC /D "_MPPC_" /D "_MAC" /D "NDEBUG" /NOLOGO
# ADD MRC /D "_MPPC_" /D "_MAC" /D "NDEBUG" /NOLOGO
MRC_PROJ=/D "_MPPC_" /D "_MAC" /D "NDEBUG" /l 0x409 /NOLOGO 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/stdreg.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 /nologo /mac:bundle /machine:MPPC
# ADD LINK32 /nologo /mac:bundle /mac:creator="STDR" /machine:MPPC
LINK32_FLAGS=/nologo /mac:bundle /mac:type="APPL" /mac:creator="STDR"\
 /incremental:no /pdb:"$(OUTDIR)/stdreg.pdb" /machine:MPPC\
 /out:"$(OUTDIR)/stdreg.exe" 
LINK32_OBJS= \
	"$(INTDIR)/stdafx.obj" \
	"$(INTDIR)/sectset.obj" \
	"$(INTDIR)/instrset.obj" \
	"$(INTDIR)/enrolset.obj" \
	"$(INTDIR)/dsectset.obj" \
	"$(INTDIR)/dialog.obj" \
	"$(INTDIR)/coursset.obj" \
	"$(INTDIR)/columdlg.obj" \
	"$(INTDIR)/typeinfo.obj" \
	"$(INTDIR)/stdset.obj" \
	"$(INTDIR)/stdreg.obj" \
	"$(INTDIR)/stdrMac.rsc" \
	"$(INTDIR)/stdreg.rsc"

"$(OUTDIR)\stdreg.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

MFILE32=mfile.exe
# ADD BASE MFILE32 COPY /NOLOGO
# ADD MFILE32 COPY /NOLOGO
MFILE32_FLAGS=COPY /NOLOGO 
MFILE32_FILES= \
	"$(OUTDIR)/stdreg.exe"

"$(OUTDIR)\stdreg.trg" : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\PMacRel\stdreg.exe "$(MFILE32_DEST)"

DOWNLOAD : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\PMacRel\stdreg.exe "$(MFILE32_DEST)"

!ELSEIF  "$(CFG)" == "stdreg - Power Macintosh Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir PMacDbg
# PROP BASE Intermediate_Dir PMacDbg
# PROP BASE Target_Dir
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir PMacDbg
# PROP Intermediate_Dir PMacDbg
# PROP Target_Dir
OUTDIR=.\PMacDbg
INTDIR=.\PMacDbg

ALL : "$(OUTDIR)\stdreg.exe" "$(OUTDIR)\stdreg.trg"

CLEAN : 
	-@erase ".\PMacDbg\vc40.pdb"
	-@erase ".\PMacDbg\stdreg.pch"
	-@erase ".\PMacDbg\stdreg.ilk"
	-@erase ".\PMacDbg\stdafx.obj"
	-@erase ".\PMacDbg\sectset.obj"
	-@erase ".\PMacDbg\instrset.obj"
	-@erase ".\PMacDbg\enrolset.obj"
	-@erase ".\PMacDbg\dsectset.obj"
	-@erase ".\PMacDbg\dialog.obj"
	-@erase ".\PMacDbg\coursset.obj"
	-@erase ".\PMacDbg\columdlg.obj"
	-@erase ".\PMacDbg\typeinfo.obj"
	-@erase ".\PMacDbg\stdset.obj"
	-@erase ".\PMacDbg\stdreg.obj"
	-@erase ".\PMacDbg\stdrMac.rsc"
	-@erase ".\PMacDbg\stdreg.rsc"
	-@erase ".\PMacDbg\stdreg.pdb"
	-@erase ".\PMacDbg\stdreg.trg"
	-@erase ".\PMacDbg\stdreg.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

MTL=mktyplib.exe
# ADD BASE MTL /nologo /D "_DEBUG"
# ADD MTL /nologo /D "_DEBUG"
MTL_PROJ=/nologo /D "_DEBUG" 
CPP=cl.exe
# ADD BASE CPP /nologo /MDd /W3 /GX /Zi /Od /D "_MAC" /D "_MPPC_" /D "_WINDOWS" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_AFXDLL" /Yu"stdafx.h" /c
# ADD CPP /nologo /MDd /W3 /GX /Zi /Od /D "_MAC" /D "_MPPC_" /D "_WINDOWS" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_AFXDLL" /Yu"stdafx.h" /c
CPP_PROJ=/nologo /MDd /W3 /GX /Zi /Od /D "_MAC" /D "_MPPC_" /D "_WINDOWS" /D\
 "WIN32" /D "_DEBUG" /D "_MBCS" /D "_AFXDLL" /Fp"$(INTDIR)/stdreg.pch"\
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
# ADD BASE RSC /l 0x409 /r /d "_MAC" /d "_MPPC_" /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /r /d "_MAC" /d "_MPPC_" /d "_DEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /r /m /fo"$(INTDIR)/stdreg.rsc" /d "_MAC" /d "_MPPC_" /d\
 "_DEBUG" /d "_AFXDLL" 
MRC=mrc.exe
# ADD BASE MRC /D "_MPPC_" /D "_MAC" /D "_DEBUG" /NOLOGO
# ADD MRC /D "_MPPC_" /D "_MAC" /D "_DEBUG" /NOLOGO
MRC_PROJ=/D "_MPPC_" /D "_MAC" /D "_DEBUG" /l 0x409 /NOLOGO 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/stdreg.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 /nologo /mac:bundle /debug /machine:MPPC
# ADD LINK32 /nologo /mac:bundle /mac:creator="STDR" /debug /machine:MPPC
LINK32_FLAGS=/nologo /mac:bundle /mac:type="APPL" /mac:creator="STDR"\
 /incremental:yes /pdb:"$(OUTDIR)/stdreg.pdb" /debug /machine:MPPC\
 /out:"$(OUTDIR)/stdreg.exe" 
LINK32_OBJS= \
	"$(INTDIR)/stdafx.obj" \
	"$(INTDIR)/sectset.obj" \
	"$(INTDIR)/instrset.obj" \
	"$(INTDIR)/enrolset.obj" \
	"$(INTDIR)/dsectset.obj" \
	"$(INTDIR)/dialog.obj" \
	"$(INTDIR)/coursset.obj" \
	"$(INTDIR)/columdlg.obj" \
	"$(INTDIR)/typeinfo.obj" \
	"$(INTDIR)/stdset.obj" \
	"$(INTDIR)/stdreg.obj" \
	"$(INTDIR)/stdrMac.rsc" \
	"$(INTDIR)/stdreg.rsc"

"$(OUTDIR)\stdreg.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

MFILE32=mfile.exe
# ADD BASE MFILE32 COPY /NOLOGO
# ADD MFILE32 COPY /NOLOGO
MFILE32_FLAGS=COPY /NOLOGO 
MFILE32_FILES= \
	"$(OUTDIR)/stdreg.exe"

"$(OUTDIR)\stdreg.trg" : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\PMacDbg\stdreg.exe "$(MFILE32_DEST)"

DOWNLOAD : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\PMacDbg\stdreg.exe "$(MFILE32_DEST)"

!ENDIF 

################################################################################
# Begin Target

# Name "stdreg - Macintosh Release"
# Name "stdreg - Macintosh Debug"
# Name "stdreg - Win32 Release"
# Name "stdreg - Win32 Debug"
# Name "stdreg - Power Macintosh Release"
# Name "stdreg - Power Macintosh Debug"

!IF  "$(CFG)" == "stdreg - Macintosh Release"

!ELSEIF  "$(CFG)" == "stdreg - Macintosh Debug"

!ELSEIF  "$(CFG)" == "stdreg - Win32 Release"

!ELSEIF  "$(CFG)" == "stdreg - Win32 Debug"

!ELSEIF  "$(CFG)" == "stdreg - Power Macintosh Release"

!ELSEIF  "$(CFG)" == "stdreg - Power Macintosh Debug"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\stdafx.cpp
DEP_CPP_STDAF=\
	".\stdafx.h"\
	

!IF  "$(CFG)" == "stdreg - Macintosh Release"

# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /AL /Q68s /W3 /GX /O2 /D "_WINDOWS" /D "_MAC" /D "_68K_" /D\
 "WIN32" /D "NDEBUG" /D "_MBCS" /Fp"$(INTDIR)/stdreg.pch" /Yc"stdafx.h"\
 /Fo"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\stdreg.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "stdreg - Macintosh Debug"

# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /AL /Q68s /Q68m /W3 /GX /Zi /Od /D "_WINDOWS" /D "_MAC" /D\
 "_68K_" /D "WIN32" /D "_DEBUG" /D "_MBCS" /Fp"$(INTDIR)/stdreg.pch"\
 /Yc"stdafx.h" /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\stdreg.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "stdreg - Win32 Release"

# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/stdreg.pch" /Yc"stdafx.h" /Fo"$(INTDIR)/" /c\
 $(SOURCE) \
	

"$(INTDIR)\stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\stdreg.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "stdreg - Win32 Debug"

# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/stdreg.pch" /Yc"stdafx.h" /Fo"$(INTDIR)/"\
 /Fd"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\stdreg.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "stdreg - Power Macintosh Release"

# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MD /W3 /GX /O2 /D "_MAC" /D "_MPPC_" /D "_WINDOWS" /D "WIN32"\
 /D "NDEBUG" /D "_MBCS" /D "_AFXDLL" /Fp"$(INTDIR)/stdreg.pch" /Yc"stdafx.h"\
 /Fo"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\stdreg.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "stdreg - Power Macintosh Debug"

# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MDd /W3 /GX /Zi /Od /D "_MAC" /D "_MPPC_" /D "_WINDOWS" /D\
 "WIN32" /D "_DEBUG" /D "_MBCS" /D "_AFXDLL" /Fp"$(INTDIR)/stdreg.pch"\
 /Yc"stdafx.h" /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\stdreg.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\sectset.cpp
DEP_CPP_SECTS=\
	".\sectset.h"\
	".\stdafx.h"\
	".\stdreg.h"\
	

!IF  "$(CFG)" == "stdreg - Macintosh Release"


"$(INTDIR)\sectset.obj" : $(SOURCE) $(DEP_CPP_SECTS) "$(INTDIR)"\
 "$(INTDIR)\stdreg.pch"


!ELSEIF  "$(CFG)" == "stdreg - Macintosh Debug"


"$(INTDIR)\sectset.obj" : $(SOURCE) $(DEP_CPP_SECTS) "$(INTDIR)"\
 "$(INTDIR)\stdreg.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_SECTS) "$(INTDIR)"\
 "$(INTDIR)\stdreg.pch"


!ELSEIF  "$(CFG)" == "stdreg - Win32 Release"


"$(INTDIR)\sectset.obj" : $(SOURCE) $(DEP_CPP_SECTS) "$(INTDIR)"\
 "$(INTDIR)\stdreg.pch"


!ELSEIF  "$(CFG)" == "stdreg - Win32 Debug"


"$(INTDIR)\sectset.obj" : $(SOURCE) $(DEP_CPP_SECTS) "$(INTDIR)"\
 "$(INTDIR)\stdreg.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_SECTS) "$(INTDIR)"\
 "$(INTDIR)\stdreg.pch"


!ELSEIF  "$(CFG)" == "stdreg - Power Macintosh Release"


"$(INTDIR)\sectset.obj" : $(SOURCE) $(DEP_CPP_SECTS) "$(INTDIR)"\
 "$(INTDIR)\stdreg.pch"


!ELSEIF  "$(CFG)" == "stdreg - Power Macintosh Debug"


"$(INTDIR)\sectset.obj" : $(SOURCE) $(DEP_CPP_SECTS) "$(INTDIR)"\
 "$(INTDIR)\stdreg.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_SECTS) "$(INTDIR)"\
 "$(INTDIR)\stdreg.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\instrset.cpp
DEP_CPP_INSTR=\
	".\instrset.h"\
	".\stdafx.h"\
	".\stdreg.h"\
	

!IF  "$(CFG)" == "stdreg - Macintosh Release"


"$(INTDIR)\instrset.obj" : $(SOURCE) $(DEP_CPP_INSTR) "$(INTDIR)"\
 "$(INTDIR)\stdreg.pch"


!ELSEIF  "$(CFG)" == "stdreg - Macintosh Debug"


"$(INTDIR)\instrset.obj" : $(SOURCE) $(DEP_CPP_INSTR) "$(INTDIR)"\
 "$(INTDIR)\stdreg.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_INSTR) "$(INTDIR)"\
 "$(INTDIR)\stdreg.pch"


!ELSEIF  "$(CFG)" == "stdreg - Win32 Release"


"$(INTDIR)\instrset.obj" : $(SOURCE) $(DEP_CPP_INSTR) "$(INTDIR)"\
 "$(INTDIR)\stdreg.pch"


!ELSEIF  "$(CFG)" == "stdreg - Win32 Debug"


"$(INTDIR)\instrset.obj" : $(SOURCE) $(DEP_CPP_INSTR) "$(INTDIR)"\
 "$(INTDIR)\stdreg.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_INSTR) "$(INTDIR)"\
 "$(INTDIR)\stdreg.pch"


!ELSEIF  "$(CFG)" == "stdreg - Power Macintosh Release"


"$(INTDIR)\instrset.obj" : $(SOURCE) $(DEP_CPP_INSTR) "$(INTDIR)"\
 "$(INTDIR)\stdreg.pch"


!ELSEIF  "$(CFG)" == "stdreg - Power Macintosh Debug"


"$(INTDIR)\instrset.obj" : $(SOURCE) $(DEP_CPP_INSTR) "$(INTDIR)"\
 "$(INTDIR)\stdreg.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_INSTR) "$(INTDIR)"\
 "$(INTDIR)\stdreg.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\enrolset.cpp
DEP_CPP_ENROL=\
	".\enrolset.h"\
	".\stdafx.h"\
	".\stdreg.h"\
	

!IF  "$(CFG)" == "stdreg - Macintosh Release"


"$(INTDIR)\enrolset.obj" : $(SOURCE) $(DEP_CPP_ENROL) "$(INTDIR)"\
 "$(INTDIR)\stdreg.pch"


!ELSEIF  "$(CFG)" == "stdreg - Macintosh Debug"


"$(INTDIR)\enrolset.obj" : $(SOURCE) $(DEP_CPP_ENROL) "$(INTDIR)"\
 "$(INTDIR)\stdreg.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_ENROL) "$(INTDIR)"\
 "$(INTDIR)\stdreg.pch"


!ELSEIF  "$(CFG)" == "stdreg - Win32 Release"


"$(INTDIR)\enrolset.obj" : $(SOURCE) $(DEP_CPP_ENROL) "$(INTDIR)"\
 "$(INTDIR)\stdreg.pch"


!ELSEIF  "$(CFG)" == "stdreg - Win32 Debug"


"$(INTDIR)\enrolset.obj" : $(SOURCE) $(DEP_CPP_ENROL) "$(INTDIR)"\
 "$(INTDIR)\stdreg.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_ENROL) "$(INTDIR)"\
 "$(INTDIR)\stdreg.pch"


!ELSEIF  "$(CFG)" == "stdreg - Power Macintosh Release"


"$(INTDIR)\enrolset.obj" : $(SOURCE) $(DEP_CPP_ENROL) "$(INTDIR)"\
 "$(INTDIR)\stdreg.pch"


!ELSEIF  "$(CFG)" == "stdreg - Power Macintosh Debug"


"$(INTDIR)\enrolset.obj" : $(SOURCE) $(DEP_CPP_ENROL) "$(INTDIR)"\
 "$(INTDIR)\stdreg.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_ENROL) "$(INTDIR)"\
 "$(INTDIR)\stdreg.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\dsectset.cpp
DEP_CPP_DSECT=\
	".\dsectset.h"\
	".\stdafx.h"\
	".\stdreg.h"\
	

!IF  "$(CFG)" == "stdreg - Macintosh Release"


"$(INTDIR)\dsectset.obj" : $(SOURCE) $(DEP_CPP_DSECT) "$(INTDIR)"\
 "$(INTDIR)\stdreg.pch"


!ELSEIF  "$(CFG)" == "stdreg - Macintosh Debug"


"$(INTDIR)\dsectset.obj" : $(SOURCE) $(DEP_CPP_DSECT) "$(INTDIR)"\
 "$(INTDIR)\stdreg.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_DSECT) "$(INTDIR)"\
 "$(INTDIR)\stdreg.pch"


!ELSEIF  "$(CFG)" == "stdreg - Win32 Release"


"$(INTDIR)\dsectset.obj" : $(SOURCE) $(DEP_CPP_DSECT) "$(INTDIR)"\
 "$(INTDIR)\stdreg.pch"


!ELSEIF  "$(CFG)" == "stdreg - Win32 Debug"


"$(INTDIR)\dsectset.obj" : $(SOURCE) $(DEP_CPP_DSECT) "$(INTDIR)"\
 "$(INTDIR)\stdreg.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_DSECT) "$(INTDIR)"\
 "$(INTDIR)\stdreg.pch"


!ELSEIF  "$(CFG)" == "stdreg - Power Macintosh Release"


"$(INTDIR)\dsectset.obj" : $(SOURCE) $(DEP_CPP_DSECT) "$(INTDIR)"\
 "$(INTDIR)\stdreg.pch"


!ELSEIF  "$(CFG)" == "stdreg - Power Macintosh Debug"


"$(INTDIR)\dsectset.obj" : $(SOURCE) $(DEP_CPP_DSECT) "$(INTDIR)"\
 "$(INTDIR)\stdreg.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_DSECT) "$(INTDIR)"\
 "$(INTDIR)\stdreg.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\dialog.cpp
DEP_CPP_DIALO=\
	".\dialog.h"\
	".\stdafx.h"\
	".\stdreg.h"\
	".\typeinfo.h"\
	

!IF  "$(CFG)" == "stdreg - Macintosh Release"


"$(INTDIR)\dialog.obj" : $(SOURCE) $(DEP_CPP_DIALO) "$(INTDIR)"\
 "$(INTDIR)\stdreg.pch"


!ELSEIF  "$(CFG)" == "stdreg - Macintosh Debug"


"$(INTDIR)\dialog.obj" : $(SOURCE) $(DEP_CPP_DIALO) "$(INTDIR)"\
 "$(INTDIR)\stdreg.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_DIALO) "$(INTDIR)"\
 "$(INTDIR)\stdreg.pch"


!ELSEIF  "$(CFG)" == "stdreg - Win32 Release"


"$(INTDIR)\dialog.obj" : $(SOURCE) $(DEP_CPP_DIALO) "$(INTDIR)"\
 "$(INTDIR)\stdreg.pch"


!ELSEIF  "$(CFG)" == "stdreg - Win32 Debug"


"$(INTDIR)\dialog.obj" : $(SOURCE) $(DEP_CPP_DIALO) "$(INTDIR)"\
 "$(INTDIR)\stdreg.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_DIALO) "$(INTDIR)"\
 "$(INTDIR)\stdreg.pch"


!ELSEIF  "$(CFG)" == "stdreg - Power Macintosh Release"


"$(INTDIR)\dialog.obj" : $(SOURCE) $(DEP_CPP_DIALO) "$(INTDIR)"\
 "$(INTDIR)\stdreg.pch"


!ELSEIF  "$(CFG)" == "stdreg - Power Macintosh Debug"


"$(INTDIR)\dialog.obj" : $(SOURCE) $(DEP_CPP_DIALO) "$(INTDIR)"\
 "$(INTDIR)\stdreg.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_DIALO) "$(INTDIR)"\
 "$(INTDIR)\stdreg.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\coursset.cpp
DEP_CPP_COURS=\
	".\coursset.h"\
	".\stdafx.h"\
	".\stdreg.h"\
	

!IF  "$(CFG)" == "stdreg - Macintosh Release"


"$(INTDIR)\coursset.obj" : $(SOURCE) $(DEP_CPP_COURS) "$(INTDIR)"\
 "$(INTDIR)\stdreg.pch"


!ELSEIF  "$(CFG)" == "stdreg - Macintosh Debug"


"$(INTDIR)\coursset.obj" : $(SOURCE) $(DEP_CPP_COURS) "$(INTDIR)"\
 "$(INTDIR)\stdreg.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_COURS) "$(INTDIR)"\
 "$(INTDIR)\stdreg.pch"


!ELSEIF  "$(CFG)" == "stdreg - Win32 Release"


"$(INTDIR)\coursset.obj" : $(SOURCE) $(DEP_CPP_COURS) "$(INTDIR)"\
 "$(INTDIR)\stdreg.pch"


!ELSEIF  "$(CFG)" == "stdreg - Win32 Debug"


"$(INTDIR)\coursset.obj" : $(SOURCE) $(DEP_CPP_COURS) "$(INTDIR)"\
 "$(INTDIR)\stdreg.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_COURS) "$(INTDIR)"\
 "$(INTDIR)\stdreg.pch"


!ELSEIF  "$(CFG)" == "stdreg - Power Macintosh Release"


"$(INTDIR)\coursset.obj" : $(SOURCE) $(DEP_CPP_COURS) "$(INTDIR)"\
 "$(INTDIR)\stdreg.pch"


!ELSEIF  "$(CFG)" == "stdreg - Power Macintosh Debug"


"$(INTDIR)\coursset.obj" : $(SOURCE) $(DEP_CPP_COURS) "$(INTDIR)"\
 "$(INTDIR)\stdreg.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_COURS) "$(INTDIR)"\
 "$(INTDIR)\stdreg.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\columdlg.cpp
DEP_CPP_COLUM=\
	".\columdlg.h"\
	".\stdafx.h"\
	".\stdreg.h"\
	".\typeinfo.h"\
	

!IF  "$(CFG)" == "stdreg - Macintosh Release"


"$(INTDIR)\columdlg.obj" : $(SOURCE) $(DEP_CPP_COLUM) "$(INTDIR)"\
 "$(INTDIR)\stdreg.pch"


!ELSEIF  "$(CFG)" == "stdreg - Macintosh Debug"


"$(INTDIR)\columdlg.obj" : $(SOURCE) $(DEP_CPP_COLUM) "$(INTDIR)"\
 "$(INTDIR)\stdreg.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_COLUM) "$(INTDIR)"\
 "$(INTDIR)\stdreg.pch"


!ELSEIF  "$(CFG)" == "stdreg - Win32 Release"


"$(INTDIR)\columdlg.obj" : $(SOURCE) $(DEP_CPP_COLUM) "$(INTDIR)"\
 "$(INTDIR)\stdreg.pch"


!ELSEIF  "$(CFG)" == "stdreg - Win32 Debug"


"$(INTDIR)\columdlg.obj" : $(SOURCE) $(DEP_CPP_COLUM) "$(INTDIR)"\
 "$(INTDIR)\stdreg.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_COLUM) "$(INTDIR)"\
 "$(INTDIR)\stdreg.pch"


!ELSEIF  "$(CFG)" == "stdreg - Power Macintosh Release"


"$(INTDIR)\columdlg.obj" : $(SOURCE) $(DEP_CPP_COLUM) "$(INTDIR)"\
 "$(INTDIR)\stdreg.pch"


!ELSEIF  "$(CFG)" == "stdreg - Power Macintosh Debug"


"$(INTDIR)\columdlg.obj" : $(SOURCE) $(DEP_CPP_COLUM) "$(INTDIR)"\
 "$(INTDIR)\stdreg.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_COLUM) "$(INTDIR)"\
 "$(INTDIR)\stdreg.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\typeinfo.cpp
DEP_CPP_TYPEI=\
	".\stdafx.h"\
	".\typeinfo.h"\
	

!IF  "$(CFG)" == "stdreg - Macintosh Release"


"$(INTDIR)\typeinfo.obj" : $(SOURCE) $(DEP_CPP_TYPEI) "$(INTDIR)"\
 "$(INTDIR)\stdreg.pch"


!ELSEIF  "$(CFG)" == "stdreg - Macintosh Debug"


"$(INTDIR)\typeinfo.obj" : $(SOURCE) $(DEP_CPP_TYPEI) "$(INTDIR)"\
 "$(INTDIR)\stdreg.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_TYPEI) "$(INTDIR)"\
 "$(INTDIR)\stdreg.pch"


!ELSEIF  "$(CFG)" == "stdreg - Win32 Release"


"$(INTDIR)\typeinfo.obj" : $(SOURCE) $(DEP_CPP_TYPEI) "$(INTDIR)"\
 "$(INTDIR)\stdreg.pch"


!ELSEIF  "$(CFG)" == "stdreg - Win32 Debug"


"$(INTDIR)\typeinfo.obj" : $(SOURCE) $(DEP_CPP_TYPEI) "$(INTDIR)"\
 "$(INTDIR)\stdreg.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_TYPEI) "$(INTDIR)"\
 "$(INTDIR)\stdreg.pch"


!ELSEIF  "$(CFG)" == "stdreg - Power Macintosh Release"


"$(INTDIR)\typeinfo.obj" : $(SOURCE) $(DEP_CPP_TYPEI) "$(INTDIR)"\
 "$(INTDIR)\stdreg.pch"


!ELSEIF  "$(CFG)" == "stdreg - Power Macintosh Debug"


"$(INTDIR)\typeinfo.obj" : $(SOURCE) $(DEP_CPP_TYPEI) "$(INTDIR)"\
 "$(INTDIR)\stdreg.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_TYPEI) "$(INTDIR)"\
 "$(INTDIR)\stdreg.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\stdset.cpp
DEP_CPP_STDSE=\
	".\stdafx.h"\
	".\stdreg.h"\
	".\stdset.h"\
	

!IF  "$(CFG)" == "stdreg - Macintosh Release"


"$(INTDIR)\stdset.obj" : $(SOURCE) $(DEP_CPP_STDSE) "$(INTDIR)"\
 "$(INTDIR)\stdreg.pch"


!ELSEIF  "$(CFG)" == "stdreg - Macintosh Debug"


"$(INTDIR)\stdset.obj" : $(SOURCE) $(DEP_CPP_STDSE) "$(INTDIR)"\
 "$(INTDIR)\stdreg.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_STDSE) "$(INTDIR)"\
 "$(INTDIR)\stdreg.pch"


!ELSEIF  "$(CFG)" == "stdreg - Win32 Release"


"$(INTDIR)\stdset.obj" : $(SOURCE) $(DEP_CPP_STDSE) "$(INTDIR)"\
 "$(INTDIR)\stdreg.pch"


!ELSEIF  "$(CFG)" == "stdreg - Win32 Debug"


"$(INTDIR)\stdset.obj" : $(SOURCE) $(DEP_CPP_STDSE) "$(INTDIR)"\
 "$(INTDIR)\stdreg.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_STDSE) "$(INTDIR)"\
 "$(INTDIR)\stdreg.pch"


!ELSEIF  "$(CFG)" == "stdreg - Power Macintosh Release"


"$(INTDIR)\stdset.obj" : $(SOURCE) $(DEP_CPP_STDSE) "$(INTDIR)"\
 "$(INTDIR)\stdreg.pch"


!ELSEIF  "$(CFG)" == "stdreg - Power Macintosh Debug"


"$(INTDIR)\stdset.obj" : $(SOURCE) $(DEP_CPP_STDSE) "$(INTDIR)"\
 "$(INTDIR)\stdreg.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_STDSE) "$(INTDIR)"\
 "$(INTDIR)\stdreg.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\stdreg.cpp
DEP_CPP_STDRE=\
	".\columdlg.h"\
	".\coursset.h"\
	".\dialog.h"\
	".\dsectset.h"\
	".\enrolset.h"\
	".\initdata.h"\
	".\instrset.h"\
	".\sectset.h"\
	".\stdafx.h"\
	".\stdreg.h"\
	".\stdset.h"\
	".\typeinfo.h"\
	

!IF  "$(CFG)" == "stdreg - Macintosh Release"


"$(INTDIR)\stdreg.obj" : $(SOURCE) $(DEP_CPP_STDRE) "$(INTDIR)"\
 "$(INTDIR)\stdreg.pch"


!ELSEIF  "$(CFG)" == "stdreg - Macintosh Debug"


"$(INTDIR)\stdreg.obj" : $(SOURCE) $(DEP_CPP_STDRE) "$(INTDIR)"\
 "$(INTDIR)\stdreg.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_STDRE) "$(INTDIR)"\
 "$(INTDIR)\stdreg.pch"


!ELSEIF  "$(CFG)" == "stdreg - Win32 Release"


"$(INTDIR)\stdreg.obj" : $(SOURCE) $(DEP_CPP_STDRE) "$(INTDIR)"\
 "$(INTDIR)\stdreg.pch"


!ELSEIF  "$(CFG)" == "stdreg - Win32 Debug"


"$(INTDIR)\stdreg.obj" : $(SOURCE) $(DEP_CPP_STDRE) "$(INTDIR)"\
 "$(INTDIR)\stdreg.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_STDRE) "$(INTDIR)"\
 "$(INTDIR)\stdreg.pch"


!ELSEIF  "$(CFG)" == "stdreg - Power Macintosh Release"


"$(INTDIR)\stdreg.obj" : $(SOURCE) $(DEP_CPP_STDRE) "$(INTDIR)"\
 "$(INTDIR)\stdreg.pch"


!ELSEIF  "$(CFG)" == "stdreg - Power Macintosh Debug"


"$(INTDIR)\stdreg.obj" : $(SOURCE) $(DEP_CPP_STDRE) "$(INTDIR)"\
 "$(INTDIR)\stdreg.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_STDRE) "$(INTDIR)"\
 "$(INTDIR)\stdreg.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\stdrMac.r

!IF  "$(CFG)" == "stdreg - Macintosh Release"


"$(INTDIR)\stdrMac.rsc" : $(SOURCE) "$(INTDIR)"
   $(MRC) /o"$(INTDIR)/stdrMac.rsc" /D "_68K_" /D "_MAC" /D "NDEBUG" /l 0x409\
 /NOLOGO $(SOURCE)


!ELSEIF  "$(CFG)" == "stdreg - Macintosh Debug"


"$(INTDIR)\stdrMac.rsc" : $(SOURCE) "$(INTDIR)"
   $(MRC) /o"$(INTDIR)/stdrMac.rsc" /D "_68K_" /D "_MAC" /D "_DEBUG" /l 0x409\
 /NOLOGO $(SOURCE)


!ELSEIF  "$(CFG)" == "stdreg - Win32 Release"

!ELSEIF  "$(CFG)" == "stdreg - Win32 Debug"

!ELSEIF  "$(CFG)" == "stdreg - Power Macintosh Release"


"$(INTDIR)\stdrMac.rsc" : $(SOURCE) "$(INTDIR)"
   $(MRC) /o"$(INTDIR)/stdrMac.rsc" /D "_MPPC_" /D "_MAC" /D "NDEBUG" /l 0x409\
 /NOLOGO $(SOURCE)


!ELSEIF  "$(CFG)" == "stdreg - Power Macintosh Debug"


"$(INTDIR)\stdrMac.rsc" : $(SOURCE) "$(INTDIR)"
   $(MRC) /o"$(INTDIR)/stdrMac.rsc" /D "_MPPC_" /D "_MAC" /D "_DEBUG" /l 0x409\
 /NOLOGO $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\stdreg.rc
DEP_RSC_STDREG=\
	".\RES\STDREG.ICO"\
	".\res\stdreg.rc2"\
	

!IF  "$(CFG)" == "stdreg - Macintosh Release"


"$(INTDIR)\stdreg.rsc" : $(SOURCE) $(DEP_RSC_STDREG) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "stdreg - Macintosh Debug"


"$(INTDIR)\stdreg.rsc" : $(SOURCE) $(DEP_RSC_STDREG) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "stdreg - Win32 Release"


"$(INTDIR)\stdreg.res" : $(SOURCE) $(DEP_RSC_STDREG) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "stdreg - Win32 Debug"


"$(INTDIR)\stdreg.res" : $(SOURCE) $(DEP_RSC_STDREG) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "stdreg - Power Macintosh Release"


"$(INTDIR)\stdreg.rsc" : $(SOURCE) $(DEP_RSC_STDREG) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "stdreg - Power Macintosh Debug"


"$(INTDIR)\stdreg.rsc" : $(SOURCE) $(DEP_RSC_STDREG) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ENDIF 

# End Source File
# End Target
# End Project
################################################################################
