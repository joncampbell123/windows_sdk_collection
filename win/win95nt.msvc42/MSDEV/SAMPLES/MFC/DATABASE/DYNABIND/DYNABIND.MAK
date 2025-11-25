# Microsoft Visual C++ Generated NMAKE File, Format Version 40000
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101
# TARGTYPE "Power Macintosh Application" 0x0401
# TARGTYPE "Macintosh Application" 0x0301

!IF "$(CFG)" == ""
CFG=Dynabind - Win32 Debug
!MESSAGE No configuration specified.  Defaulting to Dynabind - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "Dynabind - Win32 Release" && "$(CFG)" !=\
 "Dynabind - Win32 Debug" && "$(CFG)" != "Dynabind - Macintosh Release" &&\
 "$(CFG)" != "Dynabind - Macintosh Debug" && "$(CFG)" !=\
 "Dynabind - Power Macintosh Release" && "$(CFG)" !=\
 "Dynabind - Power Macintosh Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "dynabind.mak" CFG="Dynabind - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Dynabind - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "Dynabind - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE "Dynabind - Macintosh Release" (based on "Macintosh Application")
!MESSAGE "Dynabind - Macintosh Debug" (based on "Macintosh Application")
!MESSAGE "Dynabind - Power Macintosh Release" (based on\
 "Power Macintosh Application")
!MESSAGE "Dynabind - Power Macintosh Debug" (based on\
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
# PROP Target_Last_Scanned "Dynabind - Win32 Debug"

!IF  "$(CFG)" == "Dynabind - Win32 Release"

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

ALL : "$(OUTDIR)\dynabind.exe"

CLEAN : 
	-@erase ".\Release\dynabind.exe"
	-@erase ".\Release\sectset.obj"
	-@erase ".\Release\dynabind.pch"
	-@erase ".\Release\enroldoc.obj"
	-@erase ".\Release\addfield.obj"
	-@erase ".\Release\coursset.obj"
	-@erase ".\Release\sectform.obj"
	-@erase ".\Release\enroll.obj"
	-@erase ".\Release\stdafx.obj"
	-@erase ".\Release\mainfrm.obj"
	-@erase ".\Release\columnst.obj"
	-@erase ".\Release\dynabind.res"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"Stdafx.h" /c
# SUBTRACT CPP /Fr
CPP_PROJ=/nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/dynabind.pch" /Yu"Stdafx.h" /Fo"$(INTDIR)/"\
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
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/dynabind.res" /d "NDEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/dynabind.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 /nologo /subsystem:windows /machine:I386
LINK32_FLAGS=/nologo /subsystem:windows /incremental:no\
 /pdb:"$(OUTDIR)/dynabind.pdb" /machine:I386 /out:"$(OUTDIR)/dynabind.exe" 
LINK32_OBJS= \
	"$(INTDIR)/sectset.obj" \
	"$(INTDIR)/enroldoc.obj" \
	"$(INTDIR)/addfield.obj" \
	"$(INTDIR)/coursset.obj" \
	"$(INTDIR)/sectform.obj" \
	"$(INTDIR)/enroll.obj" \
	"$(INTDIR)/stdafx.obj" \
	"$(INTDIR)/mainfrm.obj" \
	"$(INTDIR)/columnst.obj" \
	"$(INTDIR)/dynabind.res"

"$(OUTDIR)\dynabind.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "Dynabind - Win32 Debug"

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

ALL : "$(OUTDIR)\dynabind.exe"

CLEAN : 
	-@erase ".\Debug\vc40.pdb"
	-@erase ".\Debug\dynabind.pch"
	-@erase ".\Debug\dynabind.exe"
	-@erase ".\Debug\columnst.obj"
	-@erase ".\Debug\sectset.obj"
	-@erase ".\Debug\enroldoc.obj"
	-@erase ".\Debug\enroll.obj"
	-@erase ".\Debug\addfield.obj"
	-@erase ".\Debug\coursset.obj"
	-@erase ".\Debug\sectform.obj"
	-@erase ".\Debug\stdafx.obj"
	-@erase ".\Debug\mainfrm.obj"
	-@erase ".\Debug\dynabind.res"
	-@erase ".\Debug\dynabind.ilk"
	-@erase ".\Debug\dynabind.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"Stdafx.h" /c
# SUBTRACT CPP /Fr
CPP_PROJ=/nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/dynabind.pch" /Yu"Stdafx.h"\
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
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/dynabind.res" /d "_DEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/dynabind.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386
# ADD LINK32 /nologo /subsystem:windows /debug /machine:I386
LINK32_FLAGS=/nologo /subsystem:windows /incremental:yes\
 /pdb:"$(OUTDIR)/dynabind.pdb" /debug /machine:I386\
 /out:"$(OUTDIR)/dynabind.exe" 
LINK32_OBJS= \
	"$(INTDIR)/columnst.obj" \
	"$(INTDIR)/sectset.obj" \
	"$(INTDIR)/enroldoc.obj" \
	"$(INTDIR)/enroll.obj" \
	"$(INTDIR)/addfield.obj" \
	"$(INTDIR)/coursset.obj" \
	"$(INTDIR)/sectform.obj" \
	"$(INTDIR)/stdafx.obj" \
	"$(INTDIR)/mainfrm.obj" \
	"$(INTDIR)/dynabind.res"

"$(OUTDIR)\dynabind.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "Dynabind - Macintosh Release"

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

ALL : "$(OUTDIR)\dynabind.exe" "$(OUTDIR)\dynabind.trg"

CLEAN : 
	-@erase ".\MacRel\dynabind.trg"
	-@erase ".\MacRel\dynabind.exe"
	-@erase ".\MacRel\stdafx.obj"
	-@erase ".\MacRel\sectset.obj"
	-@erase ".\MacRel\dynabind.pch"
	-@erase ".\MacRel\enroldoc.obj"
	-@erase ".\MacRel\addfield.obj"
	-@erase ".\MacRel\coursset.obj"
	-@erase ".\MacRel\sectform.obj"
	-@erase ".\MacRel\enroll.obj"
	-@erase ".\MacRel\mainfrm.obj"
	-@erase ".\MacRel\columnst.obj"
	-@erase ".\MacRel\dynabind.rsc"
	-@erase ".\MacRel\dynaMac.rsc"

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
 "WIN32" /D "NDEBUG" /D "_MBCS" /Fp"$(INTDIR)/dynabind.pch" /Yu"Stdafx.h"\
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
RSC_PROJ=/l 0x409 /r /m /fo"$(INTDIR)/dynabind.rsc" /d "_MAC" /d "_68K_" /d\
 "NDEBUG" 
MRC=mrc.exe
# ADD BASE MRC /D "_68K_" /D "_MAC" /D "NDEBUG" /NOLOGO
# ADD MRC /D "_68K_" /D "_MAC" /D "NDEBUG" /NOLOGO
MRC_PROJ=/D "_68K_" /D "_MAC" /D "NDEBUG" /l 0x409 /NOLOGO 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/dynabind.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 wlm.lib /nologo /MAC:bundle /machine:M68K
# ADD LINK32 /nologo /MAC:bundle /MAC:creator="DYNA" /machine:M68K
LINK32_FLAGS=/nologo /MAC:bundle /MAC:type="APPL" /MAC:creator="DYNA"\
 /pdb:"$(OUTDIR)/dynabind.pdb" /machine:M68K /out:"$(OUTDIR)/dynabind.exe" 
LINK32_OBJS= \
	"$(INTDIR)/stdafx.obj" \
	"$(INTDIR)/sectset.obj" \
	"$(INTDIR)/enroldoc.obj" \
	"$(INTDIR)/addfield.obj" \
	"$(INTDIR)/coursset.obj" \
	"$(INTDIR)/sectform.obj" \
	"$(INTDIR)/enroll.obj" \
	"$(INTDIR)/mainfrm.obj" \
	"$(INTDIR)/columnst.obj" \
	"$(INTDIR)/dynabind.rsc" \
	"$(INTDIR)/dynaMac.rsc"

"$(OUTDIR)\dynabind.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

MFILE32=mfile.exe
# ADD BASE MFILE32 COPY /NOLOGO
# ADD MFILE32 COPY /NOLOGO
MFILE32_FLAGS=COPY /NOLOGO 
MFILE32_FILES= \
	"$(OUTDIR)/dynabind.exe"

"$(OUTDIR)\dynabind.trg" : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\MacRel\dynabind.exe "$(MFILE32_DEST)"

DOWNLOAD : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\MacRel\dynabind.exe "$(MFILE32_DEST)"

!ELSEIF  "$(CFG)" == "Dynabind - Macintosh Debug"

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

ALL : "$(OUTDIR)\dynabind.exe" "$(OUTDIR)\dynabind.trg"

CLEAN : 
	-@erase ".\MacDbg\vc40.pdb"
	-@erase ".\MacDbg\dynabind.pch"
	-@erase ".\MacDbg\dynabind.pdb"
	-@erase ".\MacDbg\mainfrm.obj"
	-@erase ".\MacDbg\enroldoc.obj"
	-@erase ".\MacDbg\addfield.obj"
	-@erase ".\MacDbg\coursset.obj"
	-@erase ".\MacDbg\sectset.obj"
	-@erase ".\MacDbg\sectform.obj"
	-@erase ".\MacDbg\enroll.obj"
	-@erase ".\MacDbg\stdafx.obj"
	-@erase ".\MacDbg\columnst.obj"
	-@erase ".\MacDbg\dynabind.rsc"
	-@erase ".\MacDbg\dynaMac.rsc"
	-@erase ".\MacDbg\dynabind.trg"
	-@erase ".\MacDbg\dynabind.exe"

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
 "_68K_" /D "WIN32" /D "_DEBUG" /D "_MBCS" /Fp"$(INTDIR)/dynabind.pch"\
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
RSC_PROJ=/l 0x409 /r /m /fo"$(INTDIR)/dynabind.rsc" /d "_MAC" /d "_68K_" /d\
 "_DEBUG" 
MRC=mrc.exe
# ADD BASE MRC /D "_68K_" /D "_MAC" /D "_DEBUG" /NOLOGO
# ADD MRC /D "_68K_" /D "_MAC" /D "_DEBUG" /NOLOGO
MRC_PROJ=/D "_68K_" /D "_MAC" /D "_DEBUG" /l 0x409 /NOLOGO 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/dynabind.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 wlm.lib /nologo /MAC:bundle /debug /machine:M68K
# ADD LINK32 /nologo /MAC:bundle /MAC:creator="DYNA" /debug /machine:M68K
LINK32_FLAGS=/nologo /MAC:bundle /MAC:type="APPL" /MAC:creator="DYNA"\
 /pdb:"$(OUTDIR)/dynabind.pdb" /debug /machine:M68K\
 /out:"$(OUTDIR)/dynabind.exe" 
LINK32_OBJS= \
	"$(INTDIR)/mainfrm.obj" \
	"$(INTDIR)/enroldoc.obj" \
	"$(INTDIR)/addfield.obj" \
	"$(INTDIR)/coursset.obj" \
	"$(INTDIR)/sectset.obj" \
	"$(INTDIR)/sectform.obj" \
	"$(INTDIR)/enroll.obj" \
	"$(INTDIR)/stdafx.obj" \
	"$(INTDIR)/columnst.obj" \
	"$(INTDIR)/dynabind.rsc" \
	"$(INTDIR)/dynaMac.rsc"

"$(OUTDIR)\dynabind.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

MFILE32=mfile.exe
# ADD BASE MFILE32 COPY /NOLOGO
# ADD MFILE32 COPY /NOLOGO
MFILE32_FLAGS=COPY /NOLOGO 
MFILE32_FILES= \
	"$(OUTDIR)/dynabind.exe"

"$(OUTDIR)\dynabind.trg" : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\MacDbg\dynabind.exe "$(MFILE32_DEST)"

DOWNLOAD : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\MacDbg\dynabind.exe "$(MFILE32_DEST)"

!ELSEIF  "$(CFG)" == "Dynabind - Power Macintosh Release"

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

ALL : "$(OUTDIR)\dynabind.exe" "$(OUTDIR)\dynabind.trg"

CLEAN : 
	-@erase ".\PMacRel\dynabind.trg"
	-@erase ".\PMacRel\dynabind.exe"
	-@erase ".\PMacRel\stdafx.obj"
	-@erase ".\PMacRel\columnst.obj"
	-@erase ".\PMacRel\dynabind.pch"
	-@erase ".\PMacRel\mainfrm.obj"
	-@erase ".\PMacRel\enroldoc.obj"
	-@erase ".\PMacRel\addfield.obj"
	-@erase ".\PMacRel\coursset.obj"
	-@erase ".\PMacRel\sectform.obj"
	-@erase ".\PMacRel\sectset.obj"
	-@erase ".\PMacRel\enroll.obj"
	-@erase ".\PMacRel\dynaMac.rsc"
	-@erase ".\PMacRel\dynabind.rsc"

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
 "_WINDOWS" /D "WIN32" /D "_MBCS" /D "_AFXDLL" /Fp"$(INTDIR)/dynabind.pch"\
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
RSC_PROJ=/l 0x409 /r /m /fo"$(INTDIR)/dynabind.rsc" /d "_MAC" /d "_MPPC_" /d\
 "NDEBUG" 
MRC=mrc.exe
# ADD BASE MRC /D "_MPPC_" /D "_MAC" /D "NDEBUG" /NOLOGO
# ADD MRC /D "_MPPC_" /D "_MAC" /D "NDEBUG" /NOLOGO
MRC_PROJ=/D "_MPPC_" /D "_MAC" /D "NDEBUG" /l 0x409 /NOLOGO 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/dynabind.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 cfrg.rsc /nologo /mac:bundle /machine:MPPC
# ADD LINK32 /nologo /mac:bundle /mac:creator="DYNA" /machine:MPPC
LINK32_FLAGS=/nologo /mac:bundle /mac:type="APPL" /mac:creator="DYNA"\
 /incremental:no /pdb:"$(OUTDIR)/dynabind.pdb" /machine:MPPC\
 /out:"$(OUTDIR)/dynabind.exe" 
LINK32_OBJS= \
	"$(INTDIR)/stdafx.obj" \
	"$(INTDIR)/columnst.obj" \
	"$(INTDIR)/mainfrm.obj" \
	"$(INTDIR)/enroldoc.obj" \
	"$(INTDIR)/addfield.obj" \
	"$(INTDIR)/coursset.obj" \
	"$(INTDIR)/sectform.obj" \
	"$(INTDIR)/sectset.obj" \
	"$(INTDIR)/enroll.obj" \
	"$(INTDIR)/dynaMac.rsc" \
	"$(INTDIR)/dynabind.rsc"

"$(OUTDIR)\dynabind.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

MFILE32=mfile.exe
# ADD BASE MFILE32 COPY /NOLOGO
# ADD MFILE32 COPY /NOLOGO
MFILE32_FLAGS=COPY /NOLOGO 
MFILE32_FILES= \
	"$(OUTDIR)/dynabind.exe"

"$(OUTDIR)\dynabind.trg" : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\PMacRel\dynabind.exe "$(MFILE32_DEST)"

DOWNLOAD : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\PMacRel\dynabind.exe "$(MFILE32_DEST)"

!ELSEIF  "$(CFG)" == "Dynabind - Power Macintosh Debug"

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

ALL : "$(OUTDIR)\dynabind.exe" "$(OUTDIR)\dynabind.trg"

CLEAN : 
	-@erase ".\PMacDbg\vc40.pdb"
	-@erase ".\PMacDbg\dynabind.pch"
	-@erase ".\PMacDbg\dynabind.ilk"
	-@erase ".\PMacDbg\stdafx.obj"
	-@erase ".\PMacDbg\sectset.obj"
	-@erase ".\PMacDbg\columnst.obj"
	-@erase ".\PMacDbg\enroldoc.obj"
	-@erase ".\PMacDbg\addfield.obj"
	-@erase ".\PMacDbg\coursset.obj"
	-@erase ".\PMacDbg\sectform.obj"
	-@erase ".\PMacDbg\mainfrm.obj"
	-@erase ".\PMacDbg\enroll.obj"
	-@erase ".\PMacDbg\dynaMac.rsc"
	-@erase ".\PMacDbg\dynabind.rsc"
	-@erase ".\PMacDbg\dynabind.pdb"
	-@erase ".\PMacDbg\dynabind.trg"
	-@erase ".\PMacDbg\dynabind.exe"

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
 "_WINDOWS" /D "WIN32" /D "_MBCS" /D "_AFXDLL" /Fp"$(INTDIR)/dynabind.pch"\
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
RSC_PROJ=/l 0x409 /r /m /fo"$(INTDIR)/dynabind.rsc" /d "_MAC" /d "_MPPC_" /d\
 "_DEBUG" 
MRC=mrc.exe
# ADD BASE MRC /D "_MPPC_" /D "_MAC" /D "_DEBUG" /NOLOGO
# ADD MRC /D "_MPPC_" /D "_MAC" /D "_DEBUG" /NOLOGO
MRC_PROJ=/D "_MPPC_" /D "_MAC" /D "_DEBUG" /l 0x409 /NOLOGO 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/dynabind.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 cfrg.rsc /nologo /mac:bundle /debug /machine:MPPC
# ADD LINK32 /nologo /mac:bundle /mac:creator="DYNA" /debug /machine:MPPC
LINK32_FLAGS=/nologo /mac:bundle /mac:type="APPL" /mac:creator="DYNA"\
 /incremental:yes /pdb:"$(OUTDIR)/dynabind.pdb" /debug /machine:MPPC\
 /out:"$(OUTDIR)/dynabind.exe" 
LINK32_OBJS= \
	"$(INTDIR)/stdafx.obj" \
	"$(INTDIR)/sectset.obj" \
	"$(INTDIR)/columnst.obj" \
	"$(INTDIR)/enroldoc.obj" \
	"$(INTDIR)/addfield.obj" \
	"$(INTDIR)/coursset.obj" \
	"$(INTDIR)/sectform.obj" \
	"$(INTDIR)/mainfrm.obj" \
	"$(INTDIR)/enroll.obj" \
	"$(INTDIR)/dynaMac.rsc" \
	"$(INTDIR)/dynabind.rsc"

"$(OUTDIR)\dynabind.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

MFILE32=mfile.exe
# ADD BASE MFILE32 COPY /NOLOGO
# ADD MFILE32 COPY /NOLOGO
MFILE32_FLAGS=COPY /NOLOGO 
MFILE32_FILES= \
	"$(OUTDIR)/dynabind.exe"

"$(OUTDIR)\dynabind.trg" : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\PMacDbg\dynabind.exe "$(MFILE32_DEST)"

DOWNLOAD : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\PMacDbg\dynabind.exe "$(MFILE32_DEST)"

!ENDIF 

################################################################################
# Begin Target

# Name "Dynabind - Win32 Release"
# Name "Dynabind - Win32 Debug"
# Name "Dynabind - Macintosh Release"
# Name "Dynabind - Macintosh Debug"
# Name "Dynabind - Power Macintosh Release"
# Name "Dynabind - Power Macintosh Debug"

!IF  "$(CFG)" == "Dynabind - Win32 Release"

!ELSEIF  "$(CFG)" == "Dynabind - Win32 Debug"

!ELSEIF  "$(CFG)" == "Dynabind - Macintosh Release"

!ELSEIF  "$(CFG)" == "Dynabind - Macintosh Debug"

!ELSEIF  "$(CFG)" == "Dynabind - Power Macintosh Release"

!ELSEIF  "$(CFG)" == "Dynabind - Power Macintosh Debug"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\dynabind.rc
DEP_RSC_DYNAB=\
	".\res\enroll.ico"\
	".\res\idr_main.bmp"\
	".\res\enroll.rc2"\
	

!IF  "$(CFG)" == "Dynabind - Win32 Release"


"$(INTDIR)\dynabind.res" : $(SOURCE) $(DEP_RSC_DYNAB) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Dynabind - Win32 Debug"


"$(INTDIR)\dynabind.res" : $(SOURCE) $(DEP_RSC_DYNAB) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Dynabind - Macintosh Release"


"$(INTDIR)\dynabind.rsc" : $(SOURCE) $(DEP_RSC_DYNAB) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Dynabind - Macintosh Debug"


"$(INTDIR)\dynabind.rsc" : $(SOURCE) $(DEP_RSC_DYNAB) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Dynabind - Power Macintosh Release"


"$(INTDIR)\dynabind.rsc" : $(SOURCE) $(DEP_RSC_DYNAB) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Dynabind - Power Macintosh Debug"


"$(INTDIR)\dynabind.rsc" : $(SOURCE) $(DEP_RSC_DYNAB) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\columnst.cpp
DEP_CPP_COLUM=\
	".\columnst.h"\
	".\stdafx.h"\
	

!IF  "$(CFG)" == "Dynabind - Win32 Release"


"$(INTDIR)\columnst.obj" : $(SOURCE) $(DEP_CPP_COLUM) "$(INTDIR)"\
 "$(INTDIR)\dynabind.pch"


!ELSEIF  "$(CFG)" == "Dynabind - Win32 Debug"


"$(INTDIR)\columnst.obj" : $(SOURCE) $(DEP_CPP_COLUM) "$(INTDIR)"\
 "$(INTDIR)\dynabind.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_COLUM) "$(INTDIR)"\
 "$(INTDIR)\dynabind.pch"


!ELSEIF  "$(CFG)" == "Dynabind - Macintosh Release"


"$(INTDIR)\columnst.obj" : $(SOURCE) $(DEP_CPP_COLUM) "$(INTDIR)"\
 "$(INTDIR)\dynabind.pch"


!ELSEIF  "$(CFG)" == "Dynabind - Macintosh Debug"


"$(INTDIR)\columnst.obj" : $(SOURCE) $(DEP_CPP_COLUM) "$(INTDIR)"\
 "$(INTDIR)\dynabind.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_COLUM) "$(INTDIR)"\
 "$(INTDIR)\dynabind.pch"


!ELSEIF  "$(CFG)" == "Dynabind - Power Macintosh Release"


"$(INTDIR)\columnst.obj" : $(SOURCE) $(DEP_CPP_COLUM) "$(INTDIR)"\
 "$(INTDIR)\dynabind.pch"


!ELSEIF  "$(CFG)" == "Dynabind - Power Macintosh Debug"


"$(INTDIR)\columnst.obj" : $(SOURCE) $(DEP_CPP_COLUM) "$(INTDIR)"\
 "$(INTDIR)\dynabind.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_COLUM) "$(INTDIR)"\
 "$(INTDIR)\dynabind.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\coursset.cpp
DEP_CPP_COURS=\
	".\coursset.h"\
	".\enroll.h"\
	".\stdafx.h"\
	

!IF  "$(CFG)" == "Dynabind - Win32 Release"


"$(INTDIR)\coursset.obj" : $(SOURCE) $(DEP_CPP_COURS) "$(INTDIR)"\
 "$(INTDIR)\dynabind.pch"


!ELSEIF  "$(CFG)" == "Dynabind - Win32 Debug"


"$(INTDIR)\coursset.obj" : $(SOURCE) $(DEP_CPP_COURS) "$(INTDIR)"\
 "$(INTDIR)\dynabind.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_COURS) "$(INTDIR)"\
 "$(INTDIR)\dynabind.pch"


!ELSEIF  "$(CFG)" == "Dynabind - Macintosh Release"


"$(INTDIR)\coursset.obj" : $(SOURCE) $(DEP_CPP_COURS) "$(INTDIR)"\
 "$(INTDIR)\dynabind.pch"


!ELSEIF  "$(CFG)" == "Dynabind - Macintosh Debug"


"$(INTDIR)\coursset.obj" : $(SOURCE) $(DEP_CPP_COURS) "$(INTDIR)"\
 "$(INTDIR)\dynabind.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_COURS) "$(INTDIR)"\
 "$(INTDIR)\dynabind.pch"


!ELSEIF  "$(CFG)" == "Dynabind - Power Macintosh Release"


"$(INTDIR)\coursset.obj" : $(SOURCE) $(DEP_CPP_COURS) "$(INTDIR)"\
 "$(INTDIR)\dynabind.pch"


!ELSEIF  "$(CFG)" == "Dynabind - Power Macintosh Debug"


"$(INTDIR)\coursset.obj" : $(SOURCE) $(DEP_CPP_COURS) "$(INTDIR)"\
 "$(INTDIR)\dynabind.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_COURS) "$(INTDIR)"\
 "$(INTDIR)\dynabind.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\addfield.cpp
DEP_CPP_ADDFI=\
	".\addfield.h"\
	".\columnst.h"\
	".\enroll.h"\
	".\sectset.h"\
	".\stdafx.h"\
	

!IF  "$(CFG)" == "Dynabind - Win32 Release"


"$(INTDIR)\addfield.obj" : $(SOURCE) $(DEP_CPP_ADDFI) "$(INTDIR)"\
 "$(INTDIR)\dynabind.pch"


!ELSEIF  "$(CFG)" == "Dynabind - Win32 Debug"


"$(INTDIR)\addfield.obj" : $(SOURCE) $(DEP_CPP_ADDFI) "$(INTDIR)"\
 "$(INTDIR)\dynabind.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_ADDFI) "$(INTDIR)"\
 "$(INTDIR)\dynabind.pch"


!ELSEIF  "$(CFG)" == "Dynabind - Macintosh Release"


"$(INTDIR)\addfield.obj" : $(SOURCE) $(DEP_CPP_ADDFI) "$(INTDIR)"\
 "$(INTDIR)\dynabind.pch"


!ELSEIF  "$(CFG)" == "Dynabind - Macintosh Debug"


"$(INTDIR)\addfield.obj" : $(SOURCE) $(DEP_CPP_ADDFI) "$(INTDIR)"\
 "$(INTDIR)\dynabind.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_ADDFI) "$(INTDIR)"\
 "$(INTDIR)\dynabind.pch"


!ELSEIF  "$(CFG)" == "Dynabind - Power Macintosh Release"


"$(INTDIR)\addfield.obj" : $(SOURCE) $(DEP_CPP_ADDFI) "$(INTDIR)"\
 "$(INTDIR)\dynabind.pch"


!ELSEIF  "$(CFG)" == "Dynabind - Power Macintosh Debug"


"$(INTDIR)\addfield.obj" : $(SOURCE) $(DEP_CPP_ADDFI) "$(INTDIR)"\
 "$(INTDIR)\dynabind.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_ADDFI) "$(INTDIR)"\
 "$(INTDIR)\dynabind.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\enroldoc.cpp
DEP_CPP_ENROL=\
	".\coursset.h"\
	".\enroldoc.h"\
	".\enroll.h"\
	".\sectset.h"\
	".\stdafx.h"\
	

!IF  "$(CFG)" == "Dynabind - Win32 Release"


"$(INTDIR)\enroldoc.obj" : $(SOURCE) $(DEP_CPP_ENROL) "$(INTDIR)"\
 "$(INTDIR)\dynabind.pch"


!ELSEIF  "$(CFG)" == "Dynabind - Win32 Debug"


"$(INTDIR)\enroldoc.obj" : $(SOURCE) $(DEP_CPP_ENROL) "$(INTDIR)"\
 "$(INTDIR)\dynabind.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_ENROL) "$(INTDIR)"\
 "$(INTDIR)\dynabind.pch"


!ELSEIF  "$(CFG)" == "Dynabind - Macintosh Release"


"$(INTDIR)\enroldoc.obj" : $(SOURCE) $(DEP_CPP_ENROL) "$(INTDIR)"\
 "$(INTDIR)\dynabind.pch"


!ELSEIF  "$(CFG)" == "Dynabind - Macintosh Debug"


"$(INTDIR)\enroldoc.obj" : $(SOURCE) $(DEP_CPP_ENROL) "$(INTDIR)"\
 "$(INTDIR)\dynabind.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_ENROL) "$(INTDIR)"\
 "$(INTDIR)\dynabind.pch"


!ELSEIF  "$(CFG)" == "Dynabind - Power Macintosh Release"


"$(INTDIR)\enroldoc.obj" : $(SOURCE) $(DEP_CPP_ENROL) "$(INTDIR)"\
 "$(INTDIR)\dynabind.pch"


!ELSEIF  "$(CFG)" == "Dynabind - Power Macintosh Debug"


"$(INTDIR)\enroldoc.obj" : $(SOURCE) $(DEP_CPP_ENROL) "$(INTDIR)"\
 "$(INTDIR)\dynabind.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_ENROL) "$(INTDIR)"\
 "$(INTDIR)\dynabind.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\enroll.cpp

!IF  "$(CFG)" == "Dynabind - Win32 Release"

DEP_CPP_ENROLL=\
	".\stdafx.h"\
	".\enroll.h"\
	".\mainfrm.h"\
	".\sectset.h"\
	".\coursset.h"\
	".\enroldoc.h"\
	".\sectform.h"\
	

"$(INTDIR)\enroll.obj" : $(SOURCE) $(DEP_CPP_ENROLL) "$(INTDIR)"\
 "$(INTDIR)\dynabind.pch"


!ELSEIF  "$(CFG)" == "Dynabind - Win32 Debug"

DEP_CPP_ENROLL=\
	".\stdafx.h"\
	".\enroll.h"\
	".\mainfrm.h"\
	".\sectset.h"\
	".\coursset.h"\
	".\enroldoc.h"\
	".\sectform.h"\
	

"$(INTDIR)\enroll.obj" : $(SOURCE) $(DEP_CPP_ENROLL) "$(INTDIR)"\
 "$(INTDIR)\dynabind.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_ENROLL) "$(INTDIR)"\
 "$(INTDIR)\dynabind.pch"


!ELSEIF  "$(CFG)" == "Dynabind - Macintosh Release"

DEP_CPP_ENROLL=\
	".\coursset.h"\
	".\enroldoc.h"\
	".\enroll.h"\
	".\mainfrm.h"\
	".\sectform.h"\
	".\sectset.h"\
	".\stdafx.h"\
	

"$(INTDIR)\enroll.obj" : $(SOURCE) $(DEP_CPP_ENROLL) "$(INTDIR)"\
 "$(INTDIR)\dynabind.pch"


!ELSEIF  "$(CFG)" == "Dynabind - Macintosh Debug"

DEP_CPP_ENROLL=\
	".\stdafx.h"\
	".\enroll.h"\
	".\mainfrm.h"\
	".\sectset.h"\
	".\coursset.h"\
	".\enroldoc.h"\
	".\sectform.h"\
	

"$(INTDIR)\enroll.obj" : $(SOURCE) $(DEP_CPP_ENROLL) "$(INTDIR)"\
 "$(INTDIR)\dynabind.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_ENROLL) "$(INTDIR)"\
 "$(INTDIR)\dynabind.pch"


!ELSEIF  "$(CFG)" == "Dynabind - Power Macintosh Release"

DEP_CPP_ENROLL=\
	".\stdafx.h"\
	".\enroll.h"\
	".\mainfrm.h"\
	".\sectset.h"\
	".\coursset.h"\
	".\enroldoc.h"\
	".\sectform.h"\
	

"$(INTDIR)\enroll.obj" : $(SOURCE) $(DEP_CPP_ENROLL) "$(INTDIR)"\
 "$(INTDIR)\dynabind.pch"


!ELSEIF  "$(CFG)" == "Dynabind - Power Macintosh Debug"

DEP_CPP_ENROLL=\
	".\stdafx.h"\
	".\enroll.h"\
	".\mainfrm.h"\
	".\sectset.h"\
	".\coursset.h"\
	".\enroldoc.h"\
	".\sectform.h"\
	

"$(INTDIR)\enroll.obj" : $(SOURCE) $(DEP_CPP_ENROLL) "$(INTDIR)"\
 "$(INTDIR)\dynabind.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_ENROLL) "$(INTDIR)"\
 "$(INTDIR)\dynabind.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\mainfrm.cpp

!IF  "$(CFG)" == "Dynabind - Win32 Release"

DEP_CPP_MAINF=\
	".\stdafx.h"\
	".\enroll.h"\
	".\mainfrm.h"\
	

"$(INTDIR)\mainfrm.obj" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\dynabind.pch"


!ELSEIF  "$(CFG)" == "Dynabind - Win32 Debug"

DEP_CPP_MAINF=\
	".\stdafx.h"\
	".\enroll.h"\
	".\mainfrm.h"\
	

"$(INTDIR)\mainfrm.obj" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\dynabind.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\dynabind.pch"


!ELSEIF  "$(CFG)" == "Dynabind - Macintosh Release"

DEP_CPP_MAINF=\
	".\enroll.h"\
	".\mainfrm.h"\
	".\stdafx.h"\
	

"$(INTDIR)\mainfrm.obj" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\dynabind.pch"


!ELSEIF  "$(CFG)" == "Dynabind - Macintosh Debug"

DEP_CPP_MAINF=\
	".\stdafx.h"\
	".\enroll.h"\
	".\mainfrm.h"\
	

"$(INTDIR)\mainfrm.obj" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\dynabind.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\dynabind.pch"


!ELSEIF  "$(CFG)" == "Dynabind - Power Macintosh Release"

DEP_CPP_MAINF=\
	".\stdafx.h"\
	".\enroll.h"\
	".\mainfrm.h"\
	

"$(INTDIR)\mainfrm.obj" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\dynabind.pch"


!ELSEIF  "$(CFG)" == "Dynabind - Power Macintosh Debug"

DEP_CPP_MAINF=\
	".\stdafx.h"\
	".\enroll.h"\
	".\mainfrm.h"\
	

"$(INTDIR)\mainfrm.obj" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\dynabind.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\dynabind.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\sectform.cpp

!IF  "$(CFG)" == "Dynabind - Win32 Release"

DEP_CPP_SECTF=\
	".\stdafx.h"\
	".\enroll.h"\
	".\sectset.h"\
	".\coursset.h"\
	".\enroldoc.h"\
	".\sectform.h"\
	".\addfield.h"\
	

"$(INTDIR)\sectform.obj" : $(SOURCE) $(DEP_CPP_SECTF) "$(INTDIR)"\
 "$(INTDIR)\dynabind.pch"


!ELSEIF  "$(CFG)" == "Dynabind - Win32 Debug"

DEP_CPP_SECTF=\
	".\stdafx.h"\
	".\enroll.h"\
	".\sectset.h"\
	".\coursset.h"\
	".\enroldoc.h"\
	".\sectform.h"\
	".\addfield.h"\
	

"$(INTDIR)\sectform.obj" : $(SOURCE) $(DEP_CPP_SECTF) "$(INTDIR)"\
 "$(INTDIR)\dynabind.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_SECTF) "$(INTDIR)"\
 "$(INTDIR)\dynabind.pch"


!ELSEIF  "$(CFG)" == "Dynabind - Macintosh Release"

DEP_CPP_SECTF=\
	".\addfield.h"\
	".\coursset.h"\
	".\enroldoc.h"\
	".\enroll.h"\
	".\sectform.h"\
	".\sectset.h"\
	".\stdafx.h"\
	

"$(INTDIR)\sectform.obj" : $(SOURCE) $(DEP_CPP_SECTF) "$(INTDIR)"\
 "$(INTDIR)\dynabind.pch"


!ELSEIF  "$(CFG)" == "Dynabind - Macintosh Debug"

DEP_CPP_SECTF=\
	".\stdafx.h"\
	".\enroll.h"\
	".\sectset.h"\
	".\coursset.h"\
	".\enroldoc.h"\
	".\sectform.h"\
	".\addfield.h"\
	

"$(INTDIR)\sectform.obj" : $(SOURCE) $(DEP_CPP_SECTF) "$(INTDIR)"\
 "$(INTDIR)\dynabind.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_SECTF) "$(INTDIR)"\
 "$(INTDIR)\dynabind.pch"


!ELSEIF  "$(CFG)" == "Dynabind - Power Macintosh Release"

DEP_CPP_SECTF=\
	".\stdafx.h"\
	".\enroll.h"\
	".\sectset.h"\
	".\coursset.h"\
	".\enroldoc.h"\
	".\sectform.h"\
	".\addfield.h"\
	

"$(INTDIR)\sectform.obj" : $(SOURCE) $(DEP_CPP_SECTF) "$(INTDIR)"\
 "$(INTDIR)\dynabind.pch"


!ELSEIF  "$(CFG)" == "Dynabind - Power Macintosh Debug"

DEP_CPP_SECTF=\
	".\stdafx.h"\
	".\enroll.h"\
	".\sectset.h"\
	".\coursset.h"\
	".\enroldoc.h"\
	".\sectform.h"\
	".\addfield.h"\
	

"$(INTDIR)\sectform.obj" : $(SOURCE) $(DEP_CPP_SECTF) "$(INTDIR)"\
 "$(INTDIR)\dynabind.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_SECTF) "$(INTDIR)"\
 "$(INTDIR)\dynabind.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\sectset.cpp

!IF  "$(CFG)" == "Dynabind - Win32 Release"

DEP_CPP_SECTS=\
	".\stdafx.h"\
	".\enroll.h"\
	".\sectset.h"\
	

"$(INTDIR)\sectset.obj" : $(SOURCE) $(DEP_CPP_SECTS) "$(INTDIR)"\
 "$(INTDIR)\dynabind.pch"


!ELSEIF  "$(CFG)" == "Dynabind - Win32 Debug"

DEP_CPP_SECTS=\
	".\stdafx.h"\
	".\enroll.h"\
	".\sectset.h"\
	

"$(INTDIR)\sectset.obj" : $(SOURCE) $(DEP_CPP_SECTS) "$(INTDIR)"\
 "$(INTDIR)\dynabind.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_SECTS) "$(INTDIR)"\
 "$(INTDIR)\dynabind.pch"


!ELSEIF  "$(CFG)" == "Dynabind - Macintosh Release"

DEP_CPP_SECTS=\
	".\enroll.h"\
	".\sectset.h"\
	".\stdafx.h"\
	

"$(INTDIR)\sectset.obj" : $(SOURCE) $(DEP_CPP_SECTS) "$(INTDIR)"\
 "$(INTDIR)\dynabind.pch"


!ELSEIF  "$(CFG)" == "Dynabind - Macintosh Debug"

DEP_CPP_SECTS=\
	".\stdafx.h"\
	".\enroll.h"\
	".\sectset.h"\
	

"$(INTDIR)\sectset.obj" : $(SOURCE) $(DEP_CPP_SECTS) "$(INTDIR)"\
 "$(INTDIR)\dynabind.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_SECTS) "$(INTDIR)"\
 "$(INTDIR)\dynabind.pch"


!ELSEIF  "$(CFG)" == "Dynabind - Power Macintosh Release"

DEP_CPP_SECTS=\
	".\stdafx.h"\
	".\enroll.h"\
	".\sectset.h"\
	

"$(INTDIR)\sectset.obj" : $(SOURCE) $(DEP_CPP_SECTS) "$(INTDIR)"\
 "$(INTDIR)\dynabind.pch"


!ELSEIF  "$(CFG)" == "Dynabind - Power Macintosh Debug"

DEP_CPP_SECTS=\
	".\stdafx.h"\
	".\enroll.h"\
	".\sectset.h"\
	

"$(INTDIR)\sectset.obj" : $(SOURCE) $(DEP_CPP_SECTS) "$(INTDIR)"\
 "$(INTDIR)\dynabind.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_SECTS) "$(INTDIR)"\
 "$(INTDIR)\dynabind.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\stdafx.cpp
DEP_CPP_STDAF=\
	".\stdafx.h"\
	

!IF  "$(CFG)" == "Dynabind - Win32 Release"

# ADD CPP /Yc

BuildCmds= \
	$(CPP) /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/dynabind.pch" /Yc /Fo"$(INTDIR)/" /c\
 $(SOURCE) \
	

"$(INTDIR)\stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\dynabind.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "Dynabind - Win32 Debug"

# ADD CPP /Yc

BuildCmds= \
	$(CPP) /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/dynabind.pch" /Yc /Fo"$(INTDIR)/"\
 /Fd"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\dynabind.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "Dynabind - Macintosh Release"

# ADD CPP /Yc

BuildCmds= \
	$(CPP) /nologo /AL /Q68s /W3 /GX /O2 /D "_WINDOWS" /D "_MAC" /D "_68K_" /D\
 "WIN32" /D "NDEBUG" /D "_MBCS" /Fp"$(INTDIR)/dynabind.pch" /Yc /Fo"$(INTDIR)/"\
 /c $(SOURCE) \
	

"$(INTDIR)\stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\dynabind.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "Dynabind - Macintosh Debug"

# ADD CPP /Yc

BuildCmds= \
	$(CPP) /nologo /AL /Q68s /Q68m /W3 /GX /Zi /Od /D "_WINDOWS" /D "_MAC" /D\
 "_68K_" /D "WIN32" /D "_DEBUG" /D "_MBCS" /Fp"$(INTDIR)/dynabind.pch" /Yc\
 /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\dynabind.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "Dynabind - Power Macintosh Release"

# ADD CPP /Yc

BuildCmds= \
	$(CPP) /nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "_MAC" /D "_MPPC_" /D "_WINDOWS"\
 /D "WIN32" /D "_MBCS" /D "_AFXDLL" /Fp"$(INTDIR)/dynabind.pch" /Yc\
 /Fo"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\dynabind.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "Dynabind - Power Macintosh Debug"

# ADD CPP /Yc

BuildCmds= \
	$(CPP) /nologo /MDd /W3 /GX /Zi /Od /D "_DEBUG" /D "_MAC" /D "_MPPC_" /D\
 "_WINDOWS" /D "WIN32" /D "_MBCS" /D "_AFXDLL" /Fp"$(INTDIR)/dynabind.pch" /Yc\
 /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\dynabind.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\dynaMac.r

!IF  "$(CFG)" == "Dynabind - Win32 Release"

!ELSEIF  "$(CFG)" == "Dynabind - Win32 Debug"

!ELSEIF  "$(CFG)" == "Dynabind - Macintosh Release"


"$(INTDIR)\dynaMac.rsc" : $(SOURCE) "$(INTDIR)"
   $(MRC) /o"$(INTDIR)/dynaMac.rsc" /D "_68K_" /D "_MAC" /D "NDEBUG" /l 0x409\
 /NOLOGO $(SOURCE)


!ELSEIF  "$(CFG)" == "Dynabind - Macintosh Debug"


"$(INTDIR)\dynaMac.rsc" : $(SOURCE) "$(INTDIR)"
   $(MRC) /o"$(INTDIR)/dynaMac.rsc" /D "_68K_" /D "_MAC" /D "_DEBUG" /l 0x409\
 /NOLOGO $(SOURCE)


!ELSEIF  "$(CFG)" == "Dynabind - Power Macintosh Release"


"$(INTDIR)\dynaMac.rsc" : $(SOURCE) "$(INTDIR)"
   $(MRC) /o"$(INTDIR)/dynaMac.rsc" /D "_MPPC_" /D "_MAC" /D "NDEBUG" /l 0x409\
 /NOLOGO $(SOURCE)


!ELSEIF  "$(CFG)" == "Dynabind - Power Macintosh Debug"


"$(INTDIR)\dynaMac.rsc" : $(SOURCE) "$(INTDIR)"
   $(MRC) /o"$(INTDIR)/dynaMac.rsc" /D "_MPPC_" /D "_MAC" /D "_DEBUG" /l 0x409\
 /NOLOGO $(SOURCE)


!ENDIF 

# End Source File
# End Target
# End Project
################################################################################
