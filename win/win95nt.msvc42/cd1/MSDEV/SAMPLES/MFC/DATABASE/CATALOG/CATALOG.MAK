# Microsoft Visual C++ Generated NMAKE File, Format Version 40000
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101
# TARGTYPE "Power Macintosh Application" 0x0401
# TARGTYPE "Macintosh Application" 0x0301

!IF "$(CFG)" == ""
CFG=Catalog - Win32 Debug
!MESSAGE No configuration specified.  Defaulting to Catalog - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "Catalog - Win32 Release" && "$(CFG)" !=\
 "Catalog - Win32 Debug" && "$(CFG)" != "Catalog - Macintosh Release" &&\
 "$(CFG)" != "Catalog - Macintosh Debug" && "$(CFG)" !=\
 "Catalog - Power Macintosh Release" && "$(CFG)" !=\
 "Catalog - Power Macintosh Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "catalog.mak" CFG="Catalog - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Catalog - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "Catalog - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE "Catalog - Macintosh Release" (based on "Macintosh Application")
!MESSAGE "Catalog - Macintosh Debug" (based on "Macintosh Application")
!MESSAGE "Catalog - Power Macintosh Release" (based on\
 "Power Macintosh Application")
!MESSAGE "Catalog - Power Macintosh Debug" (based on\
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
# PROP Target_Last_Scanned "Catalog - Win32 Debug"

!IF  "$(CFG)" == "Catalog - Win32 Release"

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

ALL : "$(OUTDIR)\catalog.exe"

CLEAN : 
	-@erase ".\Release\catalog.idb"
	-@erase ".\Release\catalog.pch"
	-@erase ".\Release\catalog.exe"
	-@erase ".\Release\catalog.obj"
	-@erase ".\Release\tableset.obj"
	-@erase ".\Release\cataldoc.obj"
	-@erase ".\Release\stdafx.obj"
	-@erase ".\Release\catalvw.obj"
	-@erase ".\Release\mainfrm.obj"
	-@erase ".\Release\columnst.obj"
	-@erase ".\Release\catalog.res"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"Stdafx.h" /c
# SUBTRACT CPP /Fr
CPP_PROJ=/nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/catalog.pch" /Yu"Stdafx.h" /Fo"$(INTDIR)/"\
 /Fd"$(OUTDIR)/catalog.pdb" /c 
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
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/catalog.res" /d "NDEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/catalog.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 /nologo /subsystem:windows /machine:I386
LINK32_FLAGS=/nologo /subsystem:windows /incremental:no\
 /pdb:"$(OUTDIR)/catalog.pdb" /machine:I386 /out:"$(OUTDIR)/catalog.exe" 
LINK32_OBJS= \
	"$(INTDIR)/catalog.obj" \
	"$(INTDIR)/tableset.obj" \
	"$(INTDIR)/cataldoc.obj" \
	"$(INTDIR)/stdafx.obj" \
	"$(INTDIR)/catalvw.obj" \
	"$(INTDIR)/mainfrm.obj" \
	"$(INTDIR)/columnst.obj" \
	"$(INTDIR)/catalog.res"

"$(OUTDIR)\catalog.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "Catalog - Win32 Debug"

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

ALL : "$(OUTDIR)\catalog.exe"

CLEAN : 
	-@erase ".\Debug\catalog.pdb"
	-@erase ".\Debug\catalog.pch"
	-@erase ".\Debug\cataldoc.obj"
	-@erase ".\Debug\tableset.obj"
	-@erase ".\Debug\mainfrm.obj"
	-@erase ".\Debug\catalvw.obj"
	-@erase ".\Debug\stdafx.obj"
	-@erase ".\Debug\catalog.obj"
	-@erase ".\Debug\columnst.obj"
	-@erase ".\Debug\catalog.res"
	-@erase ".\Debug\catalog.idb"
	-@erase ".\Debug\catalog.exe"
	-@erase ".\Debug\catalog.ilk"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"Stdafx.h" /c
# SUBTRACT CPP /Fr
CPP_PROJ=/nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/catalog.pch" /Yu"Stdafx.h"\
 /Fo"$(INTDIR)/" /Fd"$(OUTDIR)/catalog.pdb" /c 
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
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/catalog.res" /d "_DEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/catalog.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386
# ADD LINK32 /nologo /subsystem:windows /debug /machine:I386
LINK32_FLAGS=/nologo /subsystem:windows /incremental:yes\
 /pdb:"$(OUTDIR)/catalog.pdb" /debug /machine:I386 /out:"$(OUTDIR)/catalog.exe" 
LINK32_OBJS= \
	"$(INTDIR)/cataldoc.obj" \
	"$(INTDIR)/tableset.obj" \
	"$(INTDIR)/mainfrm.obj" \
	"$(INTDIR)/catalvw.obj" \
	"$(INTDIR)/stdafx.obj" \
	"$(INTDIR)/catalog.obj" \
	"$(INTDIR)/columnst.obj" \
	"$(INTDIR)/catalog.res"

"$(OUTDIR)\catalog.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "Catalog - Macintosh Release"

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

ALL : "$(OUTDIR)\catalog.exe"

CLEAN : 
	-@erase ".\MacRel\catalog.exe"
	-@erase ".\MacRel\tableset.obj"
	-@erase ".\MacRel\catalog.pch"
	-@erase ".\MacRel\cataldoc.obj"
	-@erase ".\MacRel\catalog.obj"
	-@erase ".\MacRel\stdafx.obj"
	-@erase ".\MacRel\columnst.obj"
	-@erase ".\MacRel\catalvw.obj"
	-@erase ".\MacRel\mainfrm.obj"
	-@erase ".\MacRel\catalog.rsc"
	-@erase ".\MacRel\cataMac.rsc"

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
 "WIN32" /D "NDEBUG" /D "_MBCS" /Fp"$(INTDIR)/catalog.pch" /Yu"Stdafx.h"\
 /Fo"$(INTDIR)/" /Fd"$(OUTDIR)/catalog.pdb" /c 
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
RSC_PROJ=/l 0x409 /r /m /fo"$(INTDIR)/catalog.rsc" /d "_MAC" /d "_68K_" /d\
 "NDEBUG" 
MRC=mrc.exe
# ADD BASE MRC /D "_68K_" /D "_MAC" /D "NDEBUG" /NOLOGO
# ADD MRC /D "_68K_" /D "_MAC" /D "NDEBUG" /NOLOGO
MRC_PROJ=/D "_68K_" /D "_MAC" /D "NDEBUG" /l 0x409 /NOLOGO 
MFILE32=mfile.exe
# ADD BASE MFILE32 COPY /NOLOGO
# ADD MFILE32 COPY /NOLOGO
MFILE32_FLAGS=COPY /NOLOGO 
MFILE32_FILES= \
	"$(OUTDIR)/catalog.exe"
    $(MFILE32) @<<
  $(MFILE32_FLAGS) $(MFILE32_FILES)
<<

BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/catalog.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 wlm.lib /nologo /MAC:bundle /machine:M68K
# ADD LINK32 /nologo /MAC:bundle /MAC:creator="CATA" /machine:M68K
LINK32_FLAGS=/nologo /MAC:bundle /MAC:type="APPL" /MAC:creator="CATA"\
 /pdb:"$(OUTDIR)/catalog.pdb" /machine:M68K /out:"$(OUTDIR)/catalog.exe" 
LINK32_OBJS= \
	"$(INTDIR)/tableset.obj" \
	"$(INTDIR)/cataldoc.obj" \
	"$(INTDIR)/catalog.obj" \
	"$(INTDIR)/stdafx.obj" \
	"$(INTDIR)/columnst.obj" \
	"$(INTDIR)/catalvw.obj" \
	"$(INTDIR)/mainfrm.obj" \
	"$(INTDIR)/catalog.rsc" \
	"$(INTDIR)/cataMac.rsc"

"$(OUTDIR)\catalog.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "Catalog - Macintosh Debug"

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

ALL : "$(OUTDIR)\catalog.exe" "$(OUTDIR)\catalog.trg"

CLEAN : 
	-@erase ".\MacDbg\catalog.pdb"
	-@erase ".\MacDbg\catalog.pch"
	-@erase ".\MacDbg\catalvw.obj"
	-@erase ".\MacDbg\mainfrm.obj"
	-@erase ".\MacDbg\tableset.obj"
	-@erase ".\MacDbg\cataldoc.obj"
	-@erase ".\MacDbg\stdafx.obj"
	-@erase ".\MacDbg\catalog.obj"
	-@erase ".\MacDbg\columnst.obj"
	-@erase ".\MacDbg\catalog.rsc"
	-@erase ".\MacDbg\cataMac.rsc"
	-@erase ".\MacDbg\catalog.trg"
	-@erase ".\MacDbg\catalog.exe"

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
 "_68K_" /D "WIN32" /D "_DEBUG" /D "_MBCS" /Fp"$(INTDIR)/catalog.pch"\
 /Yu"Stdafx.h" /Fo"$(INTDIR)/" /Fd"$(OUTDIR)/catalog.pdb" /c 
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
RSC_PROJ=/l 0x409 /r /m /fo"$(INTDIR)/catalog.rsc" /d "_MAC" /d "_68K_" /d\
 "_DEBUG" 
MRC=mrc.exe
# ADD BASE MRC /D "_68K_" /D "_MAC" /D "_DEBUG" /NOLOGO
# ADD MRC /D "_68K_" /D "_MAC" /D "_DEBUG" /NOLOGO
MRC_PROJ=/D "_68K_" /D "_MAC" /D "_DEBUG" /l 0x409 /NOLOGO 
MFILE32=mfile.exe
# ADD BASE MFILE32 COPY /NOLOGO
# ADD MFILE32 COPY /NOLOGO
MFILE32_FLAGS=COPY /NOLOGO 
MFILE32_FILES= \
	"$(OUTDIR)/catalog.exe"

"$(OUTDIR)\catalog.trg" : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) @<<
  $(MFILE32_FLAGS) $(MFILE32_FILES)
<<

BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/catalog.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 wlm.lib /nologo /MAC:bundle /debug /machine:M68K
# ADD LINK32 /nologo /MAC:bundle /MAC:creator="CATA" /debug /machine:M68K
LINK32_FLAGS=/nologo /MAC:bundle /MAC:type="APPL" /MAC:creator="CATA"\
 /pdb:"$(OUTDIR)/catalog.pdb" /debug /machine:M68K /out:"$(OUTDIR)/catalog.exe" 
LINK32_OBJS= \
	"$(INTDIR)/catalvw.obj" \
	"$(INTDIR)/mainfrm.obj" \
	"$(INTDIR)/tableset.obj" \
	"$(INTDIR)/cataldoc.obj" \
	"$(INTDIR)/stdafx.obj" \
	"$(INTDIR)/catalog.obj" \
	"$(INTDIR)/columnst.obj" \
	"$(INTDIR)/catalog.rsc" \
	"$(INTDIR)/cataMac.rsc"

"$(OUTDIR)\catalog.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "Catalog - Power Macintosh Release"

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

ALL : "$(OUTDIR)\catalog.exe"

CLEAN : 
	-@erase ".\PMacRel\catalog.exe"
	-@erase ".\PMacRel\stdafx.obj"
	-@erase ".\PMacRel\columnst.obj"
	-@erase ".\PMacRel\catalog.pch"
	-@erase ".\PMacRel\mainfrm.obj"
	-@erase ".\PMacRel\catalvw.obj"
	-@erase ".\PMacRel\tableset.obj"
	-@erase ".\PMacRel\cataldoc.obj"
	-@erase ".\PMacRel\catalog.obj"
	-@erase ".\PMacRel\cataMac.rsc"
	-@erase ".\PMacRel\catalog.rsc"

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
 "_WINDOWS" /D "WIN32" /D "_MBCS" /D "_AFXDLL" /Fp"$(INTDIR)/catalog.pch"\
 /Yu"Stdafx.h" /Fo"$(INTDIR)/" /Fd"$(OUTDIR)/catalog.pdb" /c 
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
RSC_PROJ=/l 0x409 /r /m /fo"$(INTDIR)/catalog.rsc" /d "_MAC" /d "_MPPC_" /d\
 "NDEBUG" 
MRC=mrc.exe
# ADD BASE MRC /D "_MPPC_" /D "_MAC" /D "NDEBUG" /NOLOGO
# ADD MRC /D "_MPPC_" /D "_MAC" /D "NDEBUG" /NOLOGO
MRC_PROJ=/D "_MPPC_" /D "_MAC" /D "NDEBUG" /l 0x409 /NOLOGO 
MFILE32=mfile.exe
# ADD BASE MFILE32 COPY /NOLOGO
# ADD MFILE32 COPY /NOLOGO
MFILE32_FLAGS=COPY /NOLOGO 
MFILE32_FILES= \
	"$(OUTDIR)/catalog.exe"
    $(MFILE32) @<<
  $(MFILE32_FLAGS) $(MFILE32_FILES)
<<

BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/catalog.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 cfrg.rsc /nologo /mac:bundle /machine:MPPC
# ADD LINK32 /nologo /mac:bundle /mac:creator="CATA" /machine:MPPC
LINK32_FLAGS=/nologo /mac:bundle /mac:type="APPL" /mac:creator="CATA"\
 /incremental:no /pdb:"$(OUTDIR)/catalog.pdb" /machine:MPPC\
 /out:"$(OUTDIR)/catalog.exe" 
LINK32_OBJS= \
	"$(INTDIR)/stdafx.obj" \
	"$(INTDIR)/columnst.obj" \
	"$(INTDIR)/mainfrm.obj" \
	"$(INTDIR)/catalvw.obj" \
	"$(INTDIR)/tableset.obj" \
	"$(INTDIR)/cataldoc.obj" \
	"$(INTDIR)/catalog.obj" \
	"$(INTDIR)/cataMac.rsc" \
	"$(INTDIR)/catalog.rsc"

"$(OUTDIR)\catalog.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "Catalog - Power Macintosh Debug"

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

ALL : "$(OUTDIR)\catalog.exe" "$(OUTDIR)\catalog.trg"

CLEAN : 
	-@erase ".\PMacDbg\catalog.pdb"
	-@erase ".\PMacDbg\catalog.pch"
	-@erase ".\PMacDbg\stdafx.obj"
	-@erase ".\PMacDbg\catalog.obj"
	-@erase ".\PMacDbg\columnst.obj"
	-@erase ".\PMacDbg\tableset.obj"
	-@erase ".\PMacDbg\mainfrm.obj"
	-@erase ".\PMacDbg\catalvw.obj"
	-@erase ".\PMacDbg\cataldoc.obj"
	-@erase ".\PMacDbg\cataMac.rsc"
	-@erase ".\PMacDbg\catalog.rsc"
	-@erase ".\PMacDbg\catalog.ilk"
	-@erase ".\PMacDbg\catalog.trg"
	-@erase ".\PMacDbg\catalog.exe"

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
 "_WINDOWS" /D "WIN32" /D "_MBCS" /D "_AFXDLL" /Fp"$(INTDIR)/catalog.pch"\
 /Yu"Stdafx.h" /Fo"$(INTDIR)/" /Fd"$(OUTDIR)/catalog.pdb" /c 
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
RSC_PROJ=/l 0x409 /r /m /fo"$(INTDIR)/catalog.rsc" /d "_MAC" /d "_MPPC_" /d\
 "_DEBUG" 
MRC=mrc.exe
# ADD BASE MRC /D "_MPPC_" /D "_MAC" /D "_DEBUG" /NOLOGO
# ADD MRC /D "_MPPC_" /D "_MAC" /D "_DEBUG" /NOLOGO
MRC_PROJ=/D "_MPPC_" /D "_MAC" /D "_DEBUG" /l 0x409 /NOLOGO 
MFILE32=mfile.exe
# ADD BASE MFILE32 COPY /NOLOGO
# ADD MFILE32 COPY /NOLOGO
MFILE32_FLAGS=COPY /NOLOGO 
MFILE32_FILES= \
	"$(OUTDIR)/catalog.exe"

"$(OUTDIR)\catalog.trg" : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) @<<
  $(MFILE32_FLAGS) $(MFILE32_FILES)
<<

BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/catalog.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 cfrg.rsc /nologo /mac:bundle /debug /machine:MPPC
# ADD LINK32 /nologo /mac:bundle /mac:creator="CATA" /debug /machine:MPPC
LINK32_FLAGS=/nologo /mac:bundle /mac:type="APPL" /mac:creator="CATA"\
 /incremental:yes /pdb:"$(OUTDIR)/catalog.pdb" /debug /machine:MPPC\
 /out:"$(OUTDIR)/catalog.exe" 
LINK32_OBJS= \
	"$(INTDIR)/stdafx.obj" \
	"$(INTDIR)/catalog.obj" \
	"$(INTDIR)/columnst.obj" \
	"$(INTDIR)/tableset.obj" \
	"$(INTDIR)/mainfrm.obj" \
	"$(INTDIR)/catalvw.obj" \
	"$(INTDIR)/cataldoc.obj" \
	"$(INTDIR)/cataMac.rsc" \
	"$(INTDIR)/catalog.rsc"

"$(OUTDIR)\catalog.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 

################################################################################
# Begin Target

# Name "Catalog - Win32 Release"
# Name "Catalog - Win32 Debug"
# Name "Catalog - Macintosh Release"
# Name "Catalog - Macintosh Debug"
# Name "Catalog - Power Macintosh Release"
# Name "Catalog - Power Macintosh Debug"

!IF  "$(CFG)" == "Catalog - Win32 Release"

!ELSEIF  "$(CFG)" == "Catalog - Win32 Debug"

!ELSEIF  "$(CFG)" == "Catalog - Macintosh Release"

!ELSEIF  "$(CFG)" == "Catalog - Macintosh Debug"

!ELSEIF  "$(CFG)" == "Catalog - Power Macintosh Release"

!ELSEIF  "$(CFG)" == "Catalog - Power Macintosh Debug"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\catalog.rc
DEP_RSC_CATAL=\
	".\res\catalog.ico"\
	".\res\toolbar.bmp"\
	".\res\catalog.rc2"\
	

!IF  "$(CFG)" == "Catalog - Win32 Release"


"$(INTDIR)\catalog.res" : $(SOURCE) $(DEP_RSC_CATAL) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Catalog - Win32 Debug"


"$(INTDIR)\catalog.res" : $(SOURCE) $(DEP_RSC_CATAL) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Catalog - Macintosh Release"


"$(INTDIR)\catalog.rsc" : $(SOURCE) $(DEP_RSC_CATAL) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Catalog - Macintosh Debug"


"$(INTDIR)\catalog.rsc" : $(SOURCE) $(DEP_RSC_CATAL) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Catalog - Power Macintosh Release"


"$(INTDIR)\catalog.rsc" : $(SOURCE) $(DEP_RSC_CATAL) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Catalog - Power Macintosh Debug"


"$(INTDIR)\catalog.rsc" : $(SOURCE) $(DEP_RSC_CATAL) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\catalog.cpp
DEP_CPP_CATALO=\
	".\cataldoc.h"\
	".\catalog.h"\
	".\catalvw.h"\
	".\columnst.h"\
	".\mainfrm.h"\
	".\stdafx.h"\
	".\tableset.h"\
	

!IF  "$(CFG)" == "Catalog - Win32 Release"


"$(INTDIR)\catalog.obj" : $(SOURCE) $(DEP_CPP_CATALO) "$(INTDIR)"\
 "$(INTDIR)\catalog.pch"

"$(INTDIR)\catalog.idb" : $(SOURCE) $(DEP_CPP_CATALO) "$(INTDIR)"\
 "$(INTDIR)\catalog.pch"


!ELSEIF  "$(CFG)" == "Catalog - Win32 Debug"


"$(INTDIR)\catalog.obj" : $(SOURCE) $(DEP_CPP_CATALO) "$(INTDIR)"\
 "$(INTDIR)\catalog.pch"

"$(INTDIR)\catalog.pdb" : $(SOURCE) $(DEP_CPP_CATALO) "$(INTDIR)"\
 "$(INTDIR)\catalog.pch"

"$(INTDIR)\catalog.idb" : $(SOURCE) $(DEP_CPP_CATALO) "$(INTDIR)"\
 "$(INTDIR)\catalog.pch"


!ELSEIF  "$(CFG)" == "Catalog - Macintosh Release"


"$(INTDIR)\catalog.obj" : $(SOURCE) $(DEP_CPP_CATALO) "$(INTDIR)"\
 "$(INTDIR)\catalog.pch"


!ELSEIF  "$(CFG)" == "Catalog - Macintosh Debug"


"$(INTDIR)\catalog.obj" : $(SOURCE) $(DEP_CPP_CATALO) "$(INTDIR)"\
 "$(INTDIR)\catalog.pch"

"$(INTDIR)\catalog.pdb" : $(SOURCE) $(DEP_CPP_CATALO) "$(INTDIR)"\
 "$(INTDIR)\catalog.pch"


!ELSEIF  "$(CFG)" == "Catalog - Power Macintosh Release"


"$(INTDIR)\catalog.obj" : $(SOURCE) $(DEP_CPP_CATALO) "$(INTDIR)"\
 "$(INTDIR)\catalog.pch"


!ELSEIF  "$(CFG)" == "Catalog - Power Macintosh Debug"


"$(INTDIR)\catalog.obj" : $(SOURCE) $(DEP_CPP_CATALO) "$(INTDIR)"\
 "$(INTDIR)\catalog.pch"

"$(INTDIR)\catalog.pdb" : $(SOURCE) $(DEP_CPP_CATALO) "$(INTDIR)"\
 "$(INTDIR)\catalog.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\cataldoc.cpp
DEP_CPP_CATALD=\
	".\cataldoc.h"\
	".\catalog.h"\
	".\columnst.h"\
	".\stdafx.h"\
	".\tableset.h"\
	

!IF  "$(CFG)" == "Catalog - Win32 Release"


"$(INTDIR)\cataldoc.obj" : $(SOURCE) $(DEP_CPP_CATALD) "$(INTDIR)"\
 "$(INTDIR)\catalog.pch"

"$(INTDIR)\catalog.idb" : $(SOURCE) $(DEP_CPP_CATALD) "$(INTDIR)"\
 "$(INTDIR)\catalog.pch"


!ELSEIF  "$(CFG)" == "Catalog - Win32 Debug"


"$(INTDIR)\cataldoc.obj" : $(SOURCE) $(DEP_CPP_CATALD) "$(INTDIR)"\
 "$(INTDIR)\catalog.pch"

"$(INTDIR)\catalog.pdb" : $(SOURCE) $(DEP_CPP_CATALD) "$(INTDIR)"\
 "$(INTDIR)\catalog.pch"

"$(INTDIR)\catalog.idb" : $(SOURCE) $(DEP_CPP_CATALD) "$(INTDIR)"\
 "$(INTDIR)\catalog.pch"


!ELSEIF  "$(CFG)" == "Catalog - Macintosh Release"


"$(INTDIR)\cataldoc.obj" : $(SOURCE) $(DEP_CPP_CATALD) "$(INTDIR)"\
 "$(INTDIR)\catalog.pch"


!ELSEIF  "$(CFG)" == "Catalog - Macintosh Debug"


"$(INTDIR)\cataldoc.obj" : $(SOURCE) $(DEP_CPP_CATALD) "$(INTDIR)"\
 "$(INTDIR)\catalog.pch"

"$(INTDIR)\catalog.pdb" : $(SOURCE) $(DEP_CPP_CATALD) "$(INTDIR)"\
 "$(INTDIR)\catalog.pch"


!ELSEIF  "$(CFG)" == "Catalog - Power Macintosh Release"


"$(INTDIR)\cataldoc.obj" : $(SOURCE) $(DEP_CPP_CATALD) "$(INTDIR)"\
 "$(INTDIR)\catalog.pch"


!ELSEIF  "$(CFG)" == "Catalog - Power Macintosh Debug"


"$(INTDIR)\cataldoc.obj" : $(SOURCE) $(DEP_CPP_CATALD) "$(INTDIR)"\
 "$(INTDIR)\catalog.pch"

"$(INTDIR)\catalog.pdb" : $(SOURCE) $(DEP_CPP_CATALD) "$(INTDIR)"\
 "$(INTDIR)\catalog.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\catalvw.cpp
DEP_CPP_CATALV=\
	".\cataldoc.h"\
	".\catalog.h"\
	".\catalvw.h"\
	".\columnst.h"\
	".\stdafx.h"\
	".\tableset.h"\
	

!IF  "$(CFG)" == "Catalog - Win32 Release"


"$(INTDIR)\catalvw.obj" : $(SOURCE) $(DEP_CPP_CATALV) "$(INTDIR)"\
 "$(INTDIR)\catalog.pch"

"$(INTDIR)\catalog.idb" : $(SOURCE) $(DEP_CPP_CATALV) "$(INTDIR)"\
 "$(INTDIR)\catalog.pch"


!ELSEIF  "$(CFG)" == "Catalog - Win32 Debug"


"$(INTDIR)\catalvw.obj" : $(SOURCE) $(DEP_CPP_CATALV) "$(INTDIR)"\
 "$(INTDIR)\catalog.pch"

"$(INTDIR)\catalog.pdb" : $(SOURCE) $(DEP_CPP_CATALV) "$(INTDIR)"\
 "$(INTDIR)\catalog.pch"

"$(INTDIR)\catalog.idb" : $(SOURCE) $(DEP_CPP_CATALV) "$(INTDIR)"\
 "$(INTDIR)\catalog.pch"


!ELSEIF  "$(CFG)" == "Catalog - Macintosh Release"


"$(INTDIR)\catalvw.obj" : $(SOURCE) $(DEP_CPP_CATALV) "$(INTDIR)"\
 "$(INTDIR)\catalog.pch"


!ELSEIF  "$(CFG)" == "Catalog - Macintosh Debug"


"$(INTDIR)\catalvw.obj" : $(SOURCE) $(DEP_CPP_CATALV) "$(INTDIR)"\
 "$(INTDIR)\catalog.pch"

"$(INTDIR)\catalog.pdb" : $(SOURCE) $(DEP_CPP_CATALV) "$(INTDIR)"\
 "$(INTDIR)\catalog.pch"


!ELSEIF  "$(CFG)" == "Catalog - Power Macintosh Release"


"$(INTDIR)\catalvw.obj" : $(SOURCE) $(DEP_CPP_CATALV) "$(INTDIR)"\
 "$(INTDIR)\catalog.pch"


!ELSEIF  "$(CFG)" == "Catalog - Power Macintosh Debug"


"$(INTDIR)\catalvw.obj" : $(SOURCE) $(DEP_CPP_CATALV) "$(INTDIR)"\
 "$(INTDIR)\catalog.pch"

"$(INTDIR)\catalog.pdb" : $(SOURCE) $(DEP_CPP_CATALV) "$(INTDIR)"\
 "$(INTDIR)\catalog.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\columnst.cpp
DEP_CPP_COLUM=\
	".\columnst.h"\
	".\stdafx.h"\
	

!IF  "$(CFG)" == "Catalog - Win32 Release"


"$(INTDIR)\columnst.obj" : $(SOURCE) $(DEP_CPP_COLUM) "$(INTDIR)"\
 "$(INTDIR)\catalog.pch"

"$(INTDIR)\catalog.idb" : $(SOURCE) $(DEP_CPP_COLUM) "$(INTDIR)"\
 "$(INTDIR)\catalog.pch"


!ELSEIF  "$(CFG)" == "Catalog - Win32 Debug"


"$(INTDIR)\columnst.obj" : $(SOURCE) $(DEP_CPP_COLUM) "$(INTDIR)"\
 "$(INTDIR)\catalog.pch"

"$(INTDIR)\catalog.pdb" : $(SOURCE) $(DEP_CPP_COLUM) "$(INTDIR)"\
 "$(INTDIR)\catalog.pch"

"$(INTDIR)\catalog.idb" : $(SOURCE) $(DEP_CPP_COLUM) "$(INTDIR)"\
 "$(INTDIR)\catalog.pch"


!ELSEIF  "$(CFG)" == "Catalog - Macintosh Release"


"$(INTDIR)\columnst.obj" : $(SOURCE) $(DEP_CPP_COLUM) "$(INTDIR)"\
 "$(INTDIR)\catalog.pch"


!ELSEIF  "$(CFG)" == "Catalog - Macintosh Debug"


"$(INTDIR)\columnst.obj" : $(SOURCE) $(DEP_CPP_COLUM) "$(INTDIR)"\
 "$(INTDIR)\catalog.pch"

"$(INTDIR)\catalog.pdb" : $(SOURCE) $(DEP_CPP_COLUM) "$(INTDIR)"\
 "$(INTDIR)\catalog.pch"


!ELSEIF  "$(CFG)" == "Catalog - Power Macintosh Release"


"$(INTDIR)\columnst.obj" : $(SOURCE) $(DEP_CPP_COLUM) "$(INTDIR)"\
 "$(INTDIR)\catalog.pch"


!ELSEIF  "$(CFG)" == "Catalog - Power Macintosh Debug"


"$(INTDIR)\columnst.obj" : $(SOURCE) $(DEP_CPP_COLUM) "$(INTDIR)"\
 "$(INTDIR)\catalog.pch"

"$(INTDIR)\catalog.pdb" : $(SOURCE) $(DEP_CPP_COLUM) "$(INTDIR)"\
 "$(INTDIR)\catalog.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\mainfrm.cpp
DEP_CPP_MAINF=\
	".\catalog.h"\
	".\mainfrm.h"\
	".\stdafx.h"\
	

!IF  "$(CFG)" == "Catalog - Win32 Release"


"$(INTDIR)\mainfrm.obj" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\catalog.pch"

"$(INTDIR)\catalog.idb" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\catalog.pch"


!ELSEIF  "$(CFG)" == "Catalog - Win32 Debug"


"$(INTDIR)\mainfrm.obj" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\catalog.pch"

"$(INTDIR)\catalog.pdb" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\catalog.pch"

"$(INTDIR)\catalog.idb" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\catalog.pch"


!ELSEIF  "$(CFG)" == "Catalog - Macintosh Release"


"$(INTDIR)\mainfrm.obj" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\catalog.pch"


!ELSEIF  "$(CFG)" == "Catalog - Macintosh Debug"


"$(INTDIR)\mainfrm.obj" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\catalog.pch"

"$(INTDIR)\catalog.pdb" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\catalog.pch"


!ELSEIF  "$(CFG)" == "Catalog - Power Macintosh Release"


"$(INTDIR)\mainfrm.obj" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\catalog.pch"


!ELSEIF  "$(CFG)" == "Catalog - Power Macintosh Debug"


"$(INTDIR)\mainfrm.obj" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\catalog.pch"

"$(INTDIR)\catalog.pdb" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\catalog.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\stdafx.cpp
DEP_CPP_STDAF=\
	".\stdafx.h"\
	

!IF  "$(CFG)" == "Catalog - Win32 Release"

# ADD CPP /Yc

BuildCmds= \
	$(CPP) /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/catalog.pch" /Yc /Fo"$(INTDIR)/"\
 /Fd"$(OUTDIR)/catalog.pdb" /c $(SOURCE) \
	

"$(INTDIR)\stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\catalog.idb" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\catalog.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "Catalog - Win32 Debug"

# ADD CPP /Yc

BuildCmds= \
	$(CPP) /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/catalog.pch" /Yc /Fo"$(INTDIR)/"\
 /Fd"$(OUTDIR)/catalog.pdb" /c $(SOURCE) \
	

"$(INTDIR)\stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\catalog.pdb" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\catalog.idb" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\catalog.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "Catalog - Macintosh Release"

# ADD CPP /Yc

BuildCmds= \
	$(CPP) /nologo /AL /Q68s /W3 /GX /O2 /D "_WINDOWS" /D "_MAC" /D "_68K_" /D\
 "WIN32" /D "NDEBUG" /D "_MBCS" /Fp"$(INTDIR)/catalog.pch" /Yc /Fo"$(INTDIR)/"\
 /Fd"$(OUTDIR)/catalog.pdb" /c $(SOURCE) \
	

"$(INTDIR)\stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\catalog.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "Catalog - Macintosh Debug"

# ADD CPP /Yc

BuildCmds= \
	$(CPP) /nologo /AL /Q68s /Q68m /W3 /GX /Zi /Od /D "_WINDOWS" /D "_MAC" /D\
 "_68K_" /D "WIN32" /D "_DEBUG" /D "_MBCS" /Fp"$(INTDIR)/catalog.pch" /Yc\
 /Fo"$(INTDIR)/" /Fd"$(OUTDIR)/catalog.pdb" /c $(SOURCE) \
	

"$(INTDIR)\stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\catalog.pdb" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\catalog.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "Catalog - Power Macintosh Release"

# ADD CPP /Yc

BuildCmds= \
	$(CPP) /nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "_MAC" /D "_MPPC_" /D "_WINDOWS"\
 /D "WIN32" /D "_MBCS" /D "_AFXDLL" /Fp"$(INTDIR)/catalog.pch" /Yc\
 /Fo"$(INTDIR)/" /Fd"$(OUTDIR)/catalog.pdb" /c $(SOURCE) \
	

"$(INTDIR)\stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\catalog.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "Catalog - Power Macintosh Debug"

# ADD CPP /Yc

BuildCmds= \
	$(CPP) /nologo /MDd /W3 /GX /Zi /Od /D "_DEBUG" /D "_MAC" /D "_MPPC_" /D\
 "_WINDOWS" /D "WIN32" /D "_MBCS" /D "_AFXDLL" /Fp"$(INTDIR)/catalog.pch" /Yc\
 /Fo"$(INTDIR)/" /Fd"$(OUTDIR)/catalog.pdb" /c $(SOURCE) \
	

"$(INTDIR)\stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\catalog.pdb" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\catalog.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\tableset.cpp
DEP_CPP_TABLE=\
	".\stdafx.h"\
	".\tableset.h"\
	

!IF  "$(CFG)" == "Catalog - Win32 Release"


"$(INTDIR)\tableset.obj" : $(SOURCE) $(DEP_CPP_TABLE) "$(INTDIR)"\
 "$(INTDIR)\catalog.pch"

"$(INTDIR)\catalog.idb" : $(SOURCE) $(DEP_CPP_TABLE) "$(INTDIR)"\
 "$(INTDIR)\catalog.pch"


!ELSEIF  "$(CFG)" == "Catalog - Win32 Debug"


"$(INTDIR)\tableset.obj" : $(SOURCE) $(DEP_CPP_TABLE) "$(INTDIR)"\
 "$(INTDIR)\catalog.pch"

"$(INTDIR)\catalog.pdb" : $(SOURCE) $(DEP_CPP_TABLE) "$(INTDIR)"\
 "$(INTDIR)\catalog.pch"

"$(INTDIR)\catalog.idb" : $(SOURCE) $(DEP_CPP_TABLE) "$(INTDIR)"\
 "$(INTDIR)\catalog.pch"


!ELSEIF  "$(CFG)" == "Catalog - Macintosh Release"


"$(INTDIR)\tableset.obj" : $(SOURCE) $(DEP_CPP_TABLE) "$(INTDIR)"\
 "$(INTDIR)\catalog.pch"


!ELSEIF  "$(CFG)" == "Catalog - Macintosh Debug"


"$(INTDIR)\tableset.obj" : $(SOURCE) $(DEP_CPP_TABLE) "$(INTDIR)"\
 "$(INTDIR)\catalog.pch"

"$(INTDIR)\catalog.pdb" : $(SOURCE) $(DEP_CPP_TABLE) "$(INTDIR)"\
 "$(INTDIR)\catalog.pch"


!ELSEIF  "$(CFG)" == "Catalog - Power Macintosh Release"


"$(INTDIR)\tableset.obj" : $(SOURCE) $(DEP_CPP_TABLE) "$(INTDIR)"\
 "$(INTDIR)\catalog.pch"


!ELSEIF  "$(CFG)" == "Catalog - Power Macintosh Debug"


"$(INTDIR)\tableset.obj" : $(SOURCE) $(DEP_CPP_TABLE) "$(INTDIR)"\
 "$(INTDIR)\catalog.pch"

"$(INTDIR)\catalog.pdb" : $(SOURCE) $(DEP_CPP_TABLE) "$(INTDIR)"\
 "$(INTDIR)\catalog.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\cataMac.r

!IF  "$(CFG)" == "Catalog - Win32 Release"

!ELSEIF  "$(CFG)" == "Catalog - Win32 Debug"

!ELSEIF  "$(CFG)" == "Catalog - Macintosh Release"


"$(INTDIR)\cataMac.rsc" : $(SOURCE) "$(INTDIR)"
   $(MRC) /o"$(INTDIR)/cataMac.rsc" /D "_68K_" /D "_MAC" /D "NDEBUG" /l 0x409\
 /NOLOGO $(SOURCE)


!ELSEIF  "$(CFG)" == "Catalog - Macintosh Debug"


"$(INTDIR)\cataMac.rsc" : $(SOURCE) "$(INTDIR)"
   $(MRC) /o"$(INTDIR)/cataMac.rsc" /D "_68K_" /D "_MAC" /D "_DEBUG" /l 0x409\
 /NOLOGO $(SOURCE)


!ELSEIF  "$(CFG)" == "Catalog - Power Macintosh Release"


"$(INTDIR)\cataMac.rsc" : $(SOURCE) "$(INTDIR)"
   $(MRC) /o"$(INTDIR)/cataMac.rsc" /D "_MPPC_" /D "_MAC" /D "NDEBUG" /l 0x409\
 /NOLOGO $(SOURCE)


!ELSEIF  "$(CFG)" == "Catalog - Power Macintosh Debug"


"$(INTDIR)\cataMac.rsc" : $(SOURCE) "$(INTDIR)"
   $(MRC) /o"$(INTDIR)/cataMac.rsc" /D "_MPPC_" /D "_MAC" /D "_DEBUG" /l 0x409\
 /NOLOGO $(SOURCE)


!ENDIF 

# End Source File
# End Target
# End Project
################################################################################
