# Microsoft Visual C++ Generated NMAKE File, Format Version 40000
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101
# TARGTYPE "Power Macintosh Application" 0x0401
# TARGTYPE "Macintosh Application" 0x0301

!IF "$(CFG)" == ""
CFG=DockTool - Win32 Debug
!MESSAGE No configuration specified.  Defaulting to DockTool - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "DockTool - Win32 Release" && "$(CFG)" !=\
 "DockTool - Win32 Debug" && "$(CFG)" != "DockTool - Macintosh Release" &&\
 "$(CFG)" != "DockTool - Macintosh Debug" && "$(CFG)" !=\
 "DockTool - Power Macintosh Release" && "$(CFG)" !=\
 "DockTool - Power Macintosh Debug" && "$(CFG)" !=\
 "DockTool - Win32 Unicode Release" && "$(CFG)" !=\
 "DockTool - Win32 Unicode Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "docktool.mak" CFG="DockTool - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "DockTool - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "DockTool - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE "DockTool - Macintosh Release" (based on "Macintosh Application")
!MESSAGE "DockTool - Macintosh Debug" (based on "Macintosh Application")
!MESSAGE "DockTool - Power Macintosh Release" (based on\
 "Power Macintosh Application")
!MESSAGE "DockTool - Power Macintosh Debug" (based on\
 "Power Macintosh Application")
!MESSAGE "DockTool - Win32 Unicode Release" (based on\
 "Win32 (x86) Application")
!MESSAGE "DockTool - Win32 Unicode Debug" (based on "Win32 (x86) Application")
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
# PROP Target_Last_Scanned "DockTool - Win32 Unicode Debug"

!IF  "$(CFG)" == "DockTool - Win32 Release"

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

ALL : "$(OUTDIR)\docktool.exe"

CLEAN : 
	-@erase ".\Release\docktool.exe"
	-@erase ".\Release\docktool.obj"
	-@erase ".\Release\docktool.pch"
	-@erase ".\Release\dockvw.obj"
	-@erase ".\Release\dockdoc.obj"
	-@erase ".\Release\editbar.obj"
	-@erase ".\Release\tooldlg.obj"
	-@erase ".\Release\stdafx.obj"
	-@erase ".\Release\searchbx.obj"
	-@erase ".\Release\mainfrm.obj"
	-@erase ".\Release\palette.obj"
	-@erase ".\Release\docktool.res"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /FR /YX /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
# SUBTRACT CPP /Fr
CPP_PROJ=/nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/docktool.pch" /Yu"stdafx.h" /Fo"$(INTDIR)/"\
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
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/docktool.res" /d "NDEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/docktool.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 /nologo /subsystem:windows /machine:I386
LINK32_FLAGS=/nologo /subsystem:windows /incremental:no\
 /pdb:"$(OUTDIR)/docktool.pdb" /machine:I386 /out:"$(OUTDIR)/docktool.exe" 
LINK32_OBJS= \
	"$(INTDIR)/docktool.obj" \
	"$(INTDIR)/dockvw.obj" \
	"$(INTDIR)/dockdoc.obj" \
	"$(INTDIR)/editbar.obj" \
	"$(INTDIR)/tooldlg.obj" \
	"$(INTDIR)/stdafx.obj" \
	"$(INTDIR)/searchbx.obj" \
	"$(INTDIR)/mainfrm.obj" \
	"$(INTDIR)/palette.obj" \
	"$(INTDIR)/docktool.res"

"$(OUTDIR)\docktool.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "DockTool - Win32 Debug"

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

ALL : "$(OUTDIR)\docktool.exe"

CLEAN : 
	-@erase ".\Debug\vc40.pdb"
	-@erase ".\Debug\docktool.pch"
	-@erase ".\Debug\docktool.exe"
	-@erase ".\Debug\mainfrm.obj"
	-@erase ".\Debug\palette.obj"
	-@erase ".\Debug\searchbx.obj"
	-@erase ".\Debug\dockvw.obj"
	-@erase ".\Debug\dockdoc.obj"
	-@erase ".\Debug\tooldlg.obj"
	-@erase ".\Debug\stdafx.obj"
	-@erase ".\Debug\editbar.obj"
	-@erase ".\Debug\docktool.obj"
	-@erase ".\Debug\docktool.res"
	-@erase ".\Debug\docktool.ilk"
	-@erase ".\Debug\docktool.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /FR /YX /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
# SUBTRACT CPP /Fr
CPP_PROJ=/nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/docktool.pch" /Yu"stdafx.h"\
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
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/docktool.res" /d "_DEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/docktool.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386
# ADD LINK32 /nologo /subsystem:windows /debug /machine:I386
LINK32_FLAGS=/nologo /subsystem:windows /incremental:yes\
 /pdb:"$(OUTDIR)/docktool.pdb" /debug /machine:I386\
 /out:"$(OUTDIR)/docktool.exe" 
LINK32_OBJS= \
	"$(INTDIR)/mainfrm.obj" \
	"$(INTDIR)/palette.obj" \
	"$(INTDIR)/searchbx.obj" \
	"$(INTDIR)/dockvw.obj" \
	"$(INTDIR)/dockdoc.obj" \
	"$(INTDIR)/tooldlg.obj" \
	"$(INTDIR)/stdafx.obj" \
	"$(INTDIR)/editbar.obj" \
	"$(INTDIR)/docktool.obj" \
	"$(INTDIR)/docktool.res"

"$(OUTDIR)\docktool.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "DockTool - Macintosh Release"

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

ALL : "$(OUTDIR)\docktool.exe" "$(OUTDIR)\docktool.trg"

CLEAN : 
	-@erase ".\MacRel\docktool.trg"
	-@erase ".\MacRel\docktool.exe"
	-@erase ".\MacRel\stdafx.obj"
	-@erase ".\MacRel\searchbx.obj"
	-@erase ".\MacRel\docktool.pch"
	-@erase ".\MacRel\dockdoc.obj"
	-@erase ".\MacRel\editbar.obj"
	-@erase ".\MacRel\tooldlg.obj"
	-@erase ".\MacRel\mainfrm.obj"
	-@erase ".\MacRel\dockvw.obj"
	-@erase ".\MacRel\docktool.obj"
	-@erase ".\MacRel\palette.obj"
	-@erase ".\MacRel\dockmac.rsc"
	-@erase ".\MacRel\docktool.rsc"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

MTL=mktyplib.exe
# ADD BASE MTL /nologo /D "NDEBUG" /mac
# ADD MTL /nologo /D "NDEBUG" /mac
MTL_PROJ=/nologo /D "NDEBUG" /mac 
CPP=cl.exe
# ADD BASE CPP /nologo /AL /Q68s /W3 /GX /O2 /D "_WINDOWS" /D "_MAC" /D "_68K_" /D "WIN32" /D "NDEBUG" /FR /YX /c
# ADD CPP /nologo /AL /Q68s /W3 /GX /O2 /D "_WINDOWS" /D "_MAC" /D "_68K_" /D "WIN32" /D "NDEBUG" /D "_MBCS" /Yu"stdafx.h" /c
# SUBTRACT CPP /Fr
CPP_PROJ=/nologo /AL /Q68s /W3 /GX /O2 /D "_WINDOWS" /D "_MAC" /D "_68K_" /D\
 "WIN32" /D "NDEBUG" /D "_MBCS" /Fp"$(INTDIR)/docktool.pch" /Yu"stdafx.h"\
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
RSC_PROJ=/l 0x409 /r /m /fo"$(INTDIR)/docktool.rsc" /d "_MAC" /d "_68K_" /d\
 "NDEBUG" 
MRC=mrc.exe
# ADD BASE MRC /D "_68K_" /D "_MAC" /D "NDEBUG" /NOLOGO
# ADD MRC /D "_68K_" /D "_MAC" /D "NDEBUG" /NOLOGO
MRC_PROJ=/D "_68K_" /D "_MAC" /D "NDEBUG" /l 0x409 /NOLOGO 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/docktool.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 wlm.lib /nologo /MAC:bundle /machine:M68K
# ADD LINK32 /nologo /MAC:bundle /MAC:creator="DOCK" /machine:M68K
LINK32_FLAGS=/nologo /MAC:bundle /MAC:type="APPL" /MAC:creator="DOCK"\
 /pdb:"$(OUTDIR)/docktool.pdb" /machine:M68K /out:"$(OUTDIR)/docktool.exe" 
LINK32_OBJS= \
	"$(INTDIR)/stdafx.obj" \
	"$(INTDIR)/searchbx.obj" \
	"$(INTDIR)/dockdoc.obj" \
	"$(INTDIR)/editbar.obj" \
	"$(INTDIR)/tooldlg.obj" \
	"$(INTDIR)/mainfrm.obj" \
	"$(INTDIR)/dockvw.obj" \
	"$(INTDIR)/docktool.obj" \
	"$(INTDIR)/palette.obj" \
	"$(INTDIR)/dockmac.rsc" \
	"$(INTDIR)/docktool.rsc"

"$(OUTDIR)\docktool.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

MFILE32=mfile.exe
# ADD BASE MFILE32 COPY /NOLOGO
# ADD MFILE32 COPY /NOLOGO
MFILE32_FLAGS=COPY /NOLOGO 
MFILE32_FILES= \
	"$(OUTDIR)/docktool.exe"

"$(OUTDIR)\docktool.trg" : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\MacRel\docktool.exe\
 "$(MFILE32_DEST)":docktool.exe

DOWNLOAD : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\MacRel\docktool.exe\
 "$(MFILE32_DEST)":docktool.exe

!ELSEIF  "$(CFG)" == "DockTool - Macintosh Debug"

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

ALL : "$(OUTDIR)\docktool.exe" "$(OUTDIR)\docktool.trg"

CLEAN : 
	-@erase ".\MacDbg\vc40.pdb"
	-@erase ".\MacDbg\docktool.pch"
	-@erase ".\MacDbg\docktool.pdb"
	-@erase ".\MacDbg\stdafx.obj"
	-@erase ".\MacDbg\mainfrm.obj"
	-@erase ".\MacDbg\palette.obj"
	-@erase ".\MacDbg\searchbx.obj"
	-@erase ".\MacDbg\dockvw.obj"
	-@erase ".\MacDbg\dockdoc.obj"
	-@erase ".\MacDbg\editbar.obj"
	-@erase ".\MacDbg\docktool.obj"
	-@erase ".\MacDbg\tooldlg.obj"
	-@erase ".\MacDbg\docktool.rsc"
	-@erase ".\MacDbg\dockmac.rsc"
	-@erase ".\MacDbg\docktool.trg"
	-@erase ".\MacDbg\docktool.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

MTL=mktyplib.exe
# ADD BASE MTL /nologo /D "_DEBUG" /mac
# ADD MTL /nologo /D "_DEBUG" /mac
MTL_PROJ=/nologo /D "_DEBUG" /mac 
CPP=cl.exe
# ADD BASE CPP /nologo /AL /Q68s /Q68m /W3 /GX /Zi /Od /D "_WINDOWS" /D "_MAC" /D "_68K_" /D "WIN32" /D "_DEBUG" /FR /YX /c
# ADD CPP /nologo /AL /Q68s /Q68m /W3 /GX /Zi /Od /D "_WINDOWS" /D "_MAC" /D "_68K_" /D "WIN32" /D "_DEBUG" /D "_MBCS" /Yu"stdafx.h" /c
# SUBTRACT CPP /Fr
CPP_PROJ=/nologo /AL /Q68s /Q68m /W3 /GX /Zi /Od /D "_WINDOWS" /D "_MAC" /D\
 "_68K_" /D "WIN32" /D "_DEBUG" /D "_MBCS" /Fp"$(INTDIR)/docktool.pch"\
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
RSC_PROJ=/l 0x409 /r /m /fo"$(INTDIR)/docktool.rsc" /d "_MAC" /d "_68K_" /d\
 "_DEBUG" 
MRC=mrc.exe
# ADD BASE MRC /D "_68K_" /D "_MAC" /D "_DEBUG" /NOLOGO
# ADD MRC /D "_68K_" /D "_MAC" /D "_DEBUG" /NOLOGO
MRC_PROJ=/D "_68K_" /D "_MAC" /D "_DEBUG" /l 0x409 /NOLOGO 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/docktool.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 wlm.lib /nologo /MAC:bundle /debug /machine:M68K
# ADD LINK32 /nologo /MAC:bundle /MAC:creator="DOCK" /debug /machine:M68K
LINK32_FLAGS=/nologo /MAC:bundle /MAC:type="APPL" /MAC:creator="DOCK"\
 /pdb:"$(OUTDIR)/docktool.pdb" /debug /machine:M68K\
 /out:"$(OUTDIR)/docktool.exe" 
LINK32_OBJS= \
	"$(INTDIR)/stdafx.obj" \
	"$(INTDIR)/mainfrm.obj" \
	"$(INTDIR)/palette.obj" \
	"$(INTDIR)/searchbx.obj" \
	"$(INTDIR)/dockvw.obj" \
	"$(INTDIR)/dockdoc.obj" \
	"$(INTDIR)/editbar.obj" \
	"$(INTDIR)/docktool.obj" \
	"$(INTDIR)/tooldlg.obj" \
	"$(INTDIR)/docktool.rsc" \
	"$(INTDIR)/dockmac.rsc"

"$(OUTDIR)\docktool.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

MFILE32=mfile.exe
# ADD BASE MFILE32 COPY /NOLOGO
# ADD MFILE32 COPY /NOLOGO
MFILE32_FLAGS=COPY /NOLOGO 
MFILE32_FILES= \
	"$(OUTDIR)/docktool.exe"

"$(OUTDIR)\docktool.trg" : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\MacDbg\docktool.exe\
 "$(MFILE32_DEST)":docktool.exe

DOWNLOAD : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\MacDbg\docktool.exe\
 "$(MFILE32_DEST)":docktool.exe

!ELSEIF  "$(CFG)" == "DockTool - Power Macintosh Release"

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

ALL : "$(OUTDIR)\docktool.exe" "$(OUTDIR)\docktool.trg"

CLEAN : 
	-@erase ".\PMacRel\docktool.trg"
	-@erase ".\PMacRel\docktool.exe"
	-@erase ".\PMacRel\stdafx.obj"
	-@erase ".\PMacRel\searchbx.obj"
	-@erase ".\PMacRel\docktool.pch"
	-@erase ".\PMacRel\dockdoc.obj"
	-@erase ".\PMacRel\editbar.obj"
	-@erase ".\PMacRel\tooldlg.obj"
	-@erase ".\PMacRel\mainfrm.obj"
	-@erase ".\PMacRel\docktool.obj"
	-@erase ".\PMacRel\palette.obj"
	-@erase ".\PMacRel\dockvw.obj"
	-@erase ".\PMacRel\docktool.rsc"
	-@erase ".\PMacRel\dockmac.rsc"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

MTL=mktyplib.exe
# ADD BASE MTL /nologo /D "NDEBUG"
# ADD MTL /nologo /D "NDEBUG"
MTL_PROJ=/nologo /D "NDEBUG" 
CPP=cl.exe
# ADD BASE CPP /nologo /ML /W3 /GX /O2 /D "_MAC" /D "_MPPC_" /D "_WINDOWS" /D "WIN32" /D "NDEBUG" /FR /YX /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "_MAC" /D "_MPPC_" /D "_WINDOWS" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_AFXDLL" /Yu"stdafx.h" /c
# SUBTRACT CPP /Fr
CPP_PROJ=/nologo /MD /W3 /GX /O2 /D "_MAC" /D "_MPPC_" /D "_WINDOWS" /D "WIN32"\
 /D "NDEBUG" /D "_MBCS" /D "_AFXDLL" /Fp"$(INTDIR)/docktool.pch" /Yu"stdafx.h"\
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
# ADD BASE RSC /l 0x409 /r /d "_MAC" /d "_MPPC_" /d "NDEBUG"
# ADD RSC /l 0x409 /r /d "_MAC" /d "_MPPC_" /d "NDEBUG"
RSC_PROJ=/l 0x409 /r /m /fo"$(INTDIR)/docktool.rsc" /d "_MAC" /d "_MPPC_" /d\
 "NDEBUG" 
MRC=mrc.exe
# ADD BASE MRC /D "_MPPC_" /D "_MAC" /D "NDEBUG" /NOLOGO
# ADD MRC /D "_MPPC_" /D "_MAC" /D "NDEBUG" /NOLOGO
MRC_PROJ=/D "_MPPC_" /D "_MAC" /D "NDEBUG" /l 0x409 /NOLOGO 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/docktool.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 cfrg.rsc /nologo /mac:bundle /machine:MPPC
# ADD LINK32 /nologo /mac:bundle /mac:creator="DOCK" /machine:MPPC
LINK32_FLAGS=/nologo /mac:bundle /mac:type="APPL" /mac:creator="DOCK"\
 /incremental:no /pdb:"$(OUTDIR)/docktool.pdb" /machine:MPPC\
 /out:"$(OUTDIR)/docktool.exe" 
LINK32_OBJS= \
	"$(INTDIR)/stdafx.obj" \
	"$(INTDIR)/searchbx.obj" \
	"$(INTDIR)/dockdoc.obj" \
	"$(INTDIR)/editbar.obj" \
	"$(INTDIR)/tooldlg.obj" \
	"$(INTDIR)/mainfrm.obj" \
	"$(INTDIR)/docktool.obj" \
	"$(INTDIR)/palette.obj" \
	"$(INTDIR)/dockvw.obj" \
	"$(INTDIR)/docktool.rsc" \
	"$(INTDIR)/dockmac.rsc"

"$(OUTDIR)\docktool.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

MFILE32=mfile.exe
# ADD BASE MFILE32 COPY /NOLOGO
# ADD MFILE32 COPY /NOLOGO
MFILE32_FLAGS=COPY /NOLOGO 
MFILE32_FILES= \
	"$(OUTDIR)/docktool.exe"

"$(OUTDIR)\docktool.trg" : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\PMacRel\docktool.exe\
 "$(MFILE32_DEST)":docktool.exe

DOWNLOAD : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\PMacRel\docktool.exe\
 "$(MFILE32_DEST)":docktool.exe

!ELSEIF  "$(CFG)" == "DockTool - Power Macintosh Debug"

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

ALL : "$(OUTDIR)\docktool.exe" "$(OUTDIR)\docktool.trg"

CLEAN : 
	-@erase ".\PMacDbg\vc40.pdb"
	-@erase ".\PMacDbg\docktool.pch"
	-@erase ".\PMacDbg\docktool.ilk"
	-@erase ".\PMacDbg\searchbx.obj"
	-@erase ".\PMacDbg\dockdoc.obj"
	-@erase ".\PMacDbg\editbar.obj"
	-@erase ".\PMacDbg\tooldlg.obj"
	-@erase ".\PMacDbg\docktool.obj"
	-@erase ".\PMacDbg\mainfrm.obj"
	-@erase ".\PMacDbg\dockvw.obj"
	-@erase ".\PMacDbg\palette.obj"
	-@erase ".\PMacDbg\stdafx.obj"
	-@erase ".\PMacDbg\docktool.rsc"
	-@erase ".\PMacDbg\dockmac.rsc"
	-@erase ".\PMacDbg\docktool.pdb"
	-@erase ".\PMacDbg\docktool.trg"
	-@erase ".\PMacDbg\docktool.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

MTL=mktyplib.exe
# ADD BASE MTL /nologo /D "_DEBUG"
# ADD MTL /nologo /D "_DEBUG"
MTL_PROJ=/nologo /D "_DEBUG" 
CPP=cl.exe
# ADD BASE CPP /nologo /W3 /GX /Zi /Od /D "_MAC" /D "_MPPC_" /D "_WINDOWS" /D "WIN32" /D "_DEBUG" /FR /YX /c
# ADD CPP /nologo /MDd /W3 /GX /Zi /Od /D "_MAC" /D "_MPPC_" /D "_WINDOWS" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_AFXDLL" /Yu"stdafx.h" /c
# SUBTRACT CPP /Fr
CPP_PROJ=/nologo /MDd /W3 /GX /Zi /Od /D "_MAC" /D "_MPPC_" /D "_WINDOWS" /D\
 "WIN32" /D "_DEBUG" /D "_MBCS" /D "_AFXDLL" /Fp"$(INTDIR)/docktool.pch"\
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
RSC_PROJ=/l 0x409 /r /m /fo"$(INTDIR)/docktool.rsc" /d "_MAC" /d "_MPPC_" /d\
 "_DEBUG" 
MRC=mrc.exe
# ADD BASE MRC /D "_MPPC_" /D "_MAC" /D "_DEBUG" /NOLOGO
# ADD MRC /D "_MPPC_" /D "_MAC" /D "_DEBUG" /NOLOGO
MRC_PROJ=/D "_MPPC_" /D "_MAC" /D "_DEBUG" /l 0x409 /NOLOGO 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/docktool.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 cfrg.rsc /nologo /mac:bundle /debug /machine:MPPC
# ADD LINK32 /nologo /mac:bundle /mac:creator="DOCK" /debug /machine:MPPC
LINK32_FLAGS=/nologo /mac:bundle /mac:type="APPL" /mac:creator="DOCK"\
 /incremental:yes /pdb:"$(OUTDIR)/docktool.pdb" /debug /machine:MPPC\
 /out:"$(OUTDIR)/docktool.exe" 
LINK32_OBJS= \
	"$(INTDIR)/searchbx.obj" \
	"$(INTDIR)/dockdoc.obj" \
	"$(INTDIR)/editbar.obj" \
	"$(INTDIR)/tooldlg.obj" \
	"$(INTDIR)/docktool.obj" \
	"$(INTDIR)/mainfrm.obj" \
	"$(INTDIR)/dockvw.obj" \
	"$(INTDIR)/palette.obj" \
	"$(INTDIR)/stdafx.obj" \
	"$(INTDIR)/docktool.rsc" \
	"$(INTDIR)/dockmac.rsc"

"$(OUTDIR)\docktool.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

MFILE32=mfile.exe
# ADD BASE MFILE32 COPY /NOLOGO
# ADD MFILE32 COPY /NOLOGO
MFILE32_FLAGS=COPY /NOLOGO 
MFILE32_FILES= \
	"$(OUTDIR)/docktool.exe"

"$(OUTDIR)\docktool.trg" : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\PMacDbg\docktool.exe\
 "$(MFILE32_DEST)":docktool.exe

DOWNLOAD : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\PMacDbg\docktool.exe\
 "$(MFILE32_DEST)":docktool.exe

!ELSEIF  "$(CFG)" == "DockTool - Win32 Unicode Release"

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

ALL : "$(OUTDIR)\docktool.exe"

CLEAN : 
	-@erase ".\UniRelease\docktool.exe"
	-@erase ".\UniRelease\editbar.obj"
	-@erase ".\UniRelease\docktool.pch"
	-@erase ".\UniRelease\tooldlg.obj"
	-@erase ".\UniRelease\mainfrm.obj"
	-@erase ".\UniRelease\dockvw.obj"
	-@erase ".\UniRelease\searchbx.obj"
	-@erase ".\UniRelease\palette.obj"
	-@erase ".\UniRelease\stdafx.obj"
	-@erase ".\UniRelease\docktool.obj"
	-@erase ".\UniRelease\dockdoc.obj"
	-@erase ".\UniRelease\docktool.res"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /FR /YX /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_UNICODE" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
# SUBTRACT CPP /Fr
CPP_PROJ=/nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D\
 "_UNICODE" /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/docktool.pch" /Yu"stdafx.h"\
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
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/docktool.res" /d "NDEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/docktool.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 /nologo /entry:"wWinMainCRTStartup" /subsystem:windows /machine:I386
LINK32_FLAGS=/nologo /entry:"wWinMainCRTStartup" /subsystem:windows\
 /incremental:no /pdb:"$(OUTDIR)/docktool.pdb" /machine:I386\
 /out:"$(OUTDIR)/docktool.exe" 
LINK32_OBJS= \
	"$(INTDIR)/editbar.obj" \
	"$(INTDIR)/tooldlg.obj" \
	"$(INTDIR)/mainfrm.obj" \
	"$(INTDIR)/dockvw.obj" \
	"$(INTDIR)/searchbx.obj" \
	"$(INTDIR)/palette.obj" \
	"$(INTDIR)/stdafx.obj" \
	"$(INTDIR)/docktool.obj" \
	"$(INTDIR)/dockdoc.obj" \
	"$(INTDIR)/docktool.res"

"$(OUTDIR)\docktool.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "DockTool - Win32 Unicode Debug"

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

ALL : "$(OUTDIR)\docktool.exe"

CLEAN : 
	-@erase ".\UniDebug\vc40.pdb"
	-@erase ".\UniDebug\docktool.pch"
	-@erase ".\UniDebug\docktool.exe"
	-@erase ".\UniDebug\docktool.obj"
	-@erase ".\UniDebug\dockdoc.obj"
	-@erase ".\UniDebug\editbar.obj"
	-@erase ".\UniDebug\tooldlg.obj"
	-@erase ".\UniDebug\stdafx.obj"
	-@erase ".\UniDebug\mainfrm.obj"
	-@erase ".\UniDebug\palette.obj"
	-@erase ".\UniDebug\searchbx.obj"
	-@erase ".\UniDebug\dockvw.obj"
	-@erase ".\UniDebug\docktool.res"
	-@erase ".\UniDebug\docktool.ilk"
	-@erase ".\UniDebug\docktool.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /FR /YX /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_UNICODE" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
# SUBTRACT CPP /Fr
CPP_PROJ=/nologo /MDd /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS"\
 /D "_UNICODE" /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/docktool.pch" /Yu"stdafx.h"\
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
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/docktool.res" /d "_DEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/docktool.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386
# ADD LINK32 /nologo /entry:"wWinMainCRTStartup" /subsystem:windows /debug /machine:I386
LINK32_FLAGS=/nologo /entry:"wWinMainCRTStartup" /subsystem:windows\
 /incremental:yes /pdb:"$(OUTDIR)/docktool.pdb" /debug /machine:I386\
 /out:"$(OUTDIR)/docktool.exe" 
LINK32_OBJS= \
	"$(INTDIR)/docktool.obj" \
	"$(INTDIR)/dockdoc.obj" \
	"$(INTDIR)/editbar.obj" \
	"$(INTDIR)/tooldlg.obj" \
	"$(INTDIR)/stdafx.obj" \
	"$(INTDIR)/mainfrm.obj" \
	"$(INTDIR)/palette.obj" \
	"$(INTDIR)/searchbx.obj" \
	"$(INTDIR)/dockvw.obj" \
	"$(INTDIR)/docktool.res"

"$(OUTDIR)\docktool.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 

################################################################################
# Begin Target

# Name "DockTool - Win32 Release"
# Name "DockTool - Win32 Debug"
# Name "DockTool - Macintosh Release"
# Name "DockTool - Macintosh Debug"
# Name "DockTool - Power Macintosh Release"
# Name "DockTool - Power Macintosh Debug"
# Name "DockTool - Win32 Unicode Release"
# Name "DockTool - Win32 Unicode Debug"

!IF  "$(CFG)" == "DockTool - Win32 Release"

!ELSEIF  "$(CFG)" == "DockTool - Win32 Debug"

!ELSEIF  "$(CFG)" == "DockTool - Macintosh Release"

!ELSEIF  "$(CFG)" == "DockTool - Macintosh Debug"

!ELSEIF  "$(CFG)" == "DockTool - Power Macintosh Release"

!ELSEIF  "$(CFG)" == "DockTool - Power Macintosh Debug"

!ELSEIF  "$(CFG)" == "DockTool - Win32 Unicode Release"

!ELSEIF  "$(CFG)" == "DockTool - Win32 Unicode Debug"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\dockdoc.cpp
DEP_CPP_DOCKD=\
	".\stdafx.h"\
	".\docktool.h"\
	".\dockdoc.h"\
	

!IF  "$(CFG)" == "DockTool - Win32 Release"


"$(INTDIR)\dockdoc.obj" : $(SOURCE) $(DEP_CPP_DOCKD) "$(INTDIR)"\
 "$(INTDIR)\docktool.pch"


!ELSEIF  "$(CFG)" == "DockTool - Win32 Debug"


"$(INTDIR)\dockdoc.obj" : $(SOURCE) $(DEP_CPP_DOCKD) "$(INTDIR)"\
 "$(INTDIR)\docktool.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_DOCKD) "$(INTDIR)"\
 "$(INTDIR)\docktool.pch"


!ELSEIF  "$(CFG)" == "DockTool - Macintosh Release"


"$(INTDIR)\dockdoc.obj" : $(SOURCE) $(DEP_CPP_DOCKD) "$(INTDIR)"\
 "$(INTDIR)\docktool.pch"


!ELSEIF  "$(CFG)" == "DockTool - Macintosh Debug"


"$(INTDIR)\dockdoc.obj" : $(SOURCE) $(DEP_CPP_DOCKD) "$(INTDIR)"\
 "$(INTDIR)\docktool.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_DOCKD) "$(INTDIR)"\
 "$(INTDIR)\docktool.pch"


!ELSEIF  "$(CFG)" == "DockTool - Power Macintosh Release"


"$(INTDIR)\dockdoc.obj" : $(SOURCE) $(DEP_CPP_DOCKD) "$(INTDIR)"\
 "$(INTDIR)\docktool.pch"


!ELSEIF  "$(CFG)" == "DockTool - Power Macintosh Debug"


"$(INTDIR)\dockdoc.obj" : $(SOURCE) $(DEP_CPP_DOCKD) "$(INTDIR)"\
 "$(INTDIR)\docktool.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_DOCKD) "$(INTDIR)"\
 "$(INTDIR)\docktool.pch"


!ELSEIF  "$(CFG)" == "DockTool - Win32 Unicode Release"


"$(INTDIR)\dockdoc.obj" : $(SOURCE) $(DEP_CPP_DOCKD) "$(INTDIR)"\
 "$(INTDIR)\docktool.pch"


!ELSEIF  "$(CFG)" == "DockTool - Win32 Unicode Debug"


"$(INTDIR)\dockdoc.obj" : $(SOURCE) $(DEP_CPP_DOCKD) "$(INTDIR)"\
 "$(INTDIR)\docktool.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_DOCKD) "$(INTDIR)"\
 "$(INTDIR)\docktool.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\docktool.cpp

!IF  "$(CFG)" == "DockTool - Win32 Release"

DEP_CPP_DOCKT=\
	".\dockdoc.h"\
	".\docktool.h"\
	".\dockvw.h"\
	".\mainfrm.h"\
	".\editbar.h"\
	".\searchbx.h"\
	".\palette.h"\
	".\stdafx.h"\
	

"$(INTDIR)\docktool.obj" : $(SOURCE) $(DEP_CPP_DOCKT) "$(INTDIR)"\
 "$(INTDIR)\docktool.pch"


!ELSEIF  "$(CFG)" == "DockTool - Win32 Debug"

DEP_CPP_DOCKT=\
	".\dockdoc.h"\
	".\docktool.h"\
	".\dockvw.h"\
	".\mainfrm.h"\
	".\editbar.h"\
	".\searchbx.h"\
	".\palette.h"\
	".\stdafx.h"\
	

"$(INTDIR)\docktool.obj" : $(SOURCE) $(DEP_CPP_DOCKT) "$(INTDIR)"\
 "$(INTDIR)\docktool.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_DOCKT) "$(INTDIR)"\
 "$(INTDIR)\docktool.pch"


!ELSEIF  "$(CFG)" == "DockTool - Macintosh Release"

DEP_CPP_DOCKT=\
	".\stdafx.h"\
	".\docktool.h"\
	".\mainfrm.h"\
	".\dockdoc.h"\
	".\dockvw.h"\
	".\palette.h"\
	".\editbar.h"\
	".\searchbx.h"\
	

"$(INTDIR)\docktool.obj" : $(SOURCE) $(DEP_CPP_DOCKT) "$(INTDIR)"\
 "$(INTDIR)\docktool.pch"


!ELSEIF  "$(CFG)" == "DockTool - Macintosh Debug"

DEP_CPP_DOCKT=\
	".\stdafx.h"\
	".\docktool.h"\
	".\mainfrm.h"\
	".\dockdoc.h"\
	".\dockvw.h"\
	".\palette.h"\
	".\editbar.h"\
	".\searchbx.h"\
	

"$(INTDIR)\docktool.obj" : $(SOURCE) $(DEP_CPP_DOCKT) "$(INTDIR)"\
 "$(INTDIR)\docktool.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_DOCKT) "$(INTDIR)"\
 "$(INTDIR)\docktool.pch"


!ELSEIF  "$(CFG)" == "DockTool - Power Macintosh Release"

DEP_CPP_DOCKT=\
	".\stdafx.h"\
	".\docktool.h"\
	".\mainfrm.h"\
	".\dockdoc.h"\
	".\dockvw.h"\
	".\palette.h"\
	".\editbar.h"\
	".\searchbx.h"\
	

"$(INTDIR)\docktool.obj" : $(SOURCE) $(DEP_CPP_DOCKT) "$(INTDIR)"\
 "$(INTDIR)\docktool.pch"


!ELSEIF  "$(CFG)" == "DockTool - Power Macintosh Debug"

DEP_CPP_DOCKT=\
	".\stdafx.h"\
	".\docktool.h"\
	".\mainfrm.h"\
	".\dockdoc.h"\
	".\dockvw.h"\
	".\palette.h"\
	".\editbar.h"\
	".\searchbx.h"\
	

"$(INTDIR)\docktool.obj" : $(SOURCE) $(DEP_CPP_DOCKT) "$(INTDIR)"\
 "$(INTDIR)\docktool.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_DOCKT) "$(INTDIR)"\
 "$(INTDIR)\docktool.pch"


!ELSEIF  "$(CFG)" == "DockTool - Win32 Unicode Release"

DEP_CPP_DOCKT=\
	".\stdafx.h"\
	".\docktool.h"\
	".\mainfrm.h"\
	".\dockdoc.h"\
	".\dockvw.h"\
	".\palette.h"\
	".\editbar.h"\
	".\searchbx.h"\
	

"$(INTDIR)\docktool.obj" : $(SOURCE) $(DEP_CPP_DOCKT) "$(INTDIR)"\
 "$(INTDIR)\docktool.pch"


!ELSEIF  "$(CFG)" == "DockTool - Win32 Unicode Debug"

DEP_CPP_DOCKT=\
	".\stdafx.h"\
	".\docktool.h"\
	".\mainfrm.h"\
	".\dockdoc.h"\
	".\dockvw.h"\
	".\palette.h"\
	".\editbar.h"\
	".\searchbx.h"\
	

"$(INTDIR)\docktool.obj" : $(SOURCE) $(DEP_CPP_DOCKT) "$(INTDIR)"\
 "$(INTDIR)\docktool.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_DOCKT) "$(INTDIR)"\
 "$(INTDIR)\docktool.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\docktool.rc
DEP_RSC_DOCKTO=\
	".\res\docktool.ico"\
	".\res\cresrc.bmp"\
	".\res\cedit.bmp"\
	".\res\cbrowse.bmp"\
	".\res\cdebug.bmp"\
	".\res\cmain.bmp"\
	".\res\mdebug.bmp"\
	".\res\medit.bmp"\
	".\res\mmain.bmp"\
	".\res\mresrc.bmp"\
	".\res\mbrowse.bmp"\
	".\res\cpalette.bmp"\
	".\res\mpalette.bmp"\
	".\res\docktool.rc2"\
	

!IF  "$(CFG)" == "DockTool - Win32 Release"


"$(INTDIR)\docktool.res" : $(SOURCE) $(DEP_RSC_DOCKTO) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "DockTool - Win32 Debug"


"$(INTDIR)\docktool.res" : $(SOURCE) $(DEP_RSC_DOCKTO) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "DockTool - Macintosh Release"


"$(INTDIR)\docktool.rsc" : $(SOURCE) $(DEP_RSC_DOCKTO) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "DockTool - Macintosh Debug"


"$(INTDIR)\docktool.rsc" : $(SOURCE) $(DEP_RSC_DOCKTO) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "DockTool - Power Macintosh Release"


"$(INTDIR)\docktool.rsc" : $(SOURCE) $(DEP_RSC_DOCKTO) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "DockTool - Power Macintosh Debug"


"$(INTDIR)\docktool.rsc" : $(SOURCE) $(DEP_RSC_DOCKTO) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "DockTool - Win32 Unicode Release"


"$(INTDIR)\docktool.res" : $(SOURCE) $(DEP_RSC_DOCKTO) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "DockTool - Win32 Unicode Debug"


"$(INTDIR)\docktool.res" : $(SOURCE) $(DEP_RSC_DOCKTO) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\dockvw.cpp
DEP_CPP_DOCKV=\
	".\stdafx.h"\
	".\docktool.h"\
	".\dockdoc.h"\
	".\dockvw.h"\
	

!IF  "$(CFG)" == "DockTool - Win32 Release"


"$(INTDIR)\dockvw.obj" : $(SOURCE) $(DEP_CPP_DOCKV) "$(INTDIR)"\
 "$(INTDIR)\docktool.pch"


!ELSEIF  "$(CFG)" == "DockTool - Win32 Debug"


"$(INTDIR)\dockvw.obj" : $(SOURCE) $(DEP_CPP_DOCKV) "$(INTDIR)"\
 "$(INTDIR)\docktool.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_DOCKV) "$(INTDIR)"\
 "$(INTDIR)\docktool.pch"


!ELSEIF  "$(CFG)" == "DockTool - Macintosh Release"


"$(INTDIR)\dockvw.obj" : $(SOURCE) $(DEP_CPP_DOCKV) "$(INTDIR)"\
 "$(INTDIR)\docktool.pch"


!ELSEIF  "$(CFG)" == "DockTool - Macintosh Debug"


"$(INTDIR)\dockvw.obj" : $(SOURCE) $(DEP_CPP_DOCKV) "$(INTDIR)"\
 "$(INTDIR)\docktool.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_DOCKV) "$(INTDIR)"\
 "$(INTDIR)\docktool.pch"


!ELSEIF  "$(CFG)" == "DockTool - Power Macintosh Release"


"$(INTDIR)\dockvw.obj" : $(SOURCE) $(DEP_CPP_DOCKV) "$(INTDIR)"\
 "$(INTDIR)\docktool.pch"


!ELSEIF  "$(CFG)" == "DockTool - Power Macintosh Debug"


"$(INTDIR)\dockvw.obj" : $(SOURCE) $(DEP_CPP_DOCKV) "$(INTDIR)"\
 "$(INTDIR)\docktool.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_DOCKV) "$(INTDIR)"\
 "$(INTDIR)\docktool.pch"


!ELSEIF  "$(CFG)" == "DockTool - Win32 Unicode Release"


"$(INTDIR)\dockvw.obj" : $(SOURCE) $(DEP_CPP_DOCKV) "$(INTDIR)"\
 "$(INTDIR)\docktool.pch"


!ELSEIF  "$(CFG)" == "DockTool - Win32 Unicode Debug"


"$(INTDIR)\dockvw.obj" : $(SOURCE) $(DEP_CPP_DOCKV) "$(INTDIR)"\
 "$(INTDIR)\docktool.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_DOCKV) "$(INTDIR)"\
 "$(INTDIR)\docktool.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\editbar.cpp
DEP_CPP_EDITB=\
	".\stdafx.h"\
	".\docktool.h"\
	".\editbar.h"\
	".\searchbx.h"\
	

!IF  "$(CFG)" == "DockTool - Win32 Release"


"$(INTDIR)\editbar.obj" : $(SOURCE) $(DEP_CPP_EDITB) "$(INTDIR)"\
 "$(INTDIR)\docktool.pch"


!ELSEIF  "$(CFG)" == "DockTool - Win32 Debug"


"$(INTDIR)\editbar.obj" : $(SOURCE) $(DEP_CPP_EDITB) "$(INTDIR)"\
 "$(INTDIR)\docktool.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_EDITB) "$(INTDIR)"\
 "$(INTDIR)\docktool.pch"


!ELSEIF  "$(CFG)" == "DockTool - Macintosh Release"


"$(INTDIR)\editbar.obj" : $(SOURCE) $(DEP_CPP_EDITB) "$(INTDIR)"\
 "$(INTDIR)\docktool.pch"


!ELSEIF  "$(CFG)" == "DockTool - Macintosh Debug"


"$(INTDIR)\editbar.obj" : $(SOURCE) $(DEP_CPP_EDITB) "$(INTDIR)"\
 "$(INTDIR)\docktool.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_EDITB) "$(INTDIR)"\
 "$(INTDIR)\docktool.pch"


!ELSEIF  "$(CFG)" == "DockTool - Power Macintosh Release"


"$(INTDIR)\editbar.obj" : $(SOURCE) $(DEP_CPP_EDITB) "$(INTDIR)"\
 "$(INTDIR)\docktool.pch"


!ELSEIF  "$(CFG)" == "DockTool - Power Macintosh Debug"


"$(INTDIR)\editbar.obj" : $(SOURCE) $(DEP_CPP_EDITB) "$(INTDIR)"\
 "$(INTDIR)\docktool.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_EDITB) "$(INTDIR)"\
 "$(INTDIR)\docktool.pch"


!ELSEIF  "$(CFG)" == "DockTool - Win32 Unicode Release"


"$(INTDIR)\editbar.obj" : $(SOURCE) $(DEP_CPP_EDITB) "$(INTDIR)"\
 "$(INTDIR)\docktool.pch"


!ELSEIF  "$(CFG)" == "DockTool - Win32 Unicode Debug"


"$(INTDIR)\editbar.obj" : $(SOURCE) $(DEP_CPP_EDITB) "$(INTDIR)"\
 "$(INTDIR)\docktool.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_EDITB) "$(INTDIR)"\
 "$(INTDIR)\docktool.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\mainfrm.cpp

!IF  "$(CFG)" == "DockTool - Win32 Release"

DEP_CPP_MAINF=\
	".\docktool.h"\
	".\mainfrm.h"\
	".\editbar.h"\
	".\searchbx.h"\
	".\palette.h"\
	".\stdafx.h"\
	".\tooldlg.h"\
	

"$(INTDIR)\mainfrm.obj" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\docktool.pch"


!ELSEIF  "$(CFG)" == "DockTool - Win32 Debug"

DEP_CPP_MAINF=\
	".\docktool.h"\
	".\mainfrm.h"\
	".\editbar.h"\
	".\searchbx.h"\
	".\palette.h"\
	".\stdafx.h"\
	".\tooldlg.h"\
	

"$(INTDIR)\mainfrm.obj" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\docktool.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\docktool.pch"


!ELSEIF  "$(CFG)" == "DockTool - Macintosh Release"

DEP_CPP_MAINF=\
	".\stdafx.h"\
	".\docktool.h"\
	".\tooldlg.h"\
	".\mainfrm.h"\
	".\palette.h"\
	".\editbar.h"\
	".\searchbx.h"\
	

"$(INTDIR)\mainfrm.obj" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\docktool.pch"


!ELSEIF  "$(CFG)" == "DockTool - Macintosh Debug"

DEP_CPP_MAINF=\
	".\stdafx.h"\
	".\docktool.h"\
	".\tooldlg.h"\
	".\mainfrm.h"\
	".\palette.h"\
	".\editbar.h"\
	".\searchbx.h"\
	

"$(INTDIR)\mainfrm.obj" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\docktool.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\docktool.pch"


!ELSEIF  "$(CFG)" == "DockTool - Power Macintosh Release"

DEP_CPP_MAINF=\
	".\stdafx.h"\
	".\docktool.h"\
	".\tooldlg.h"\
	".\mainfrm.h"\
	".\palette.h"\
	".\editbar.h"\
	".\searchbx.h"\
	

"$(INTDIR)\mainfrm.obj" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\docktool.pch"


!ELSEIF  "$(CFG)" == "DockTool - Power Macintosh Debug"

DEP_CPP_MAINF=\
	".\stdafx.h"\
	".\docktool.h"\
	".\tooldlg.h"\
	".\mainfrm.h"\
	".\palette.h"\
	".\editbar.h"\
	".\searchbx.h"\
	

"$(INTDIR)\mainfrm.obj" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\docktool.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\docktool.pch"


!ELSEIF  "$(CFG)" == "DockTool - Win32 Unicode Release"

DEP_CPP_MAINF=\
	".\stdafx.h"\
	".\docktool.h"\
	".\tooldlg.h"\
	".\mainfrm.h"\
	".\palette.h"\
	".\editbar.h"\
	".\searchbx.h"\
	

"$(INTDIR)\mainfrm.obj" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\docktool.pch"


!ELSEIF  "$(CFG)" == "DockTool - Win32 Unicode Debug"

DEP_CPP_MAINF=\
	".\stdafx.h"\
	".\docktool.h"\
	".\tooldlg.h"\
	".\mainfrm.h"\
	".\palette.h"\
	".\editbar.h"\
	".\searchbx.h"\
	

"$(INTDIR)\mainfrm.obj" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\docktool.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\docktool.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\palette.cpp
DEP_CPP_PALET=\
	".\palette.h"\
	".\stdafx.h"\
	

!IF  "$(CFG)" == "DockTool - Win32 Release"


"$(INTDIR)\palette.obj" : $(SOURCE) $(DEP_CPP_PALET) "$(INTDIR)"\
 "$(INTDIR)\docktool.pch"


!ELSEIF  "$(CFG)" == "DockTool - Win32 Debug"


"$(INTDIR)\palette.obj" : $(SOURCE) $(DEP_CPP_PALET) "$(INTDIR)"\
 "$(INTDIR)\docktool.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_PALET) "$(INTDIR)"\
 "$(INTDIR)\docktool.pch"


!ELSEIF  "$(CFG)" == "DockTool - Macintosh Release"


"$(INTDIR)\palette.obj" : $(SOURCE) $(DEP_CPP_PALET) "$(INTDIR)"\
 "$(INTDIR)\docktool.pch"


!ELSEIF  "$(CFG)" == "DockTool - Macintosh Debug"


"$(INTDIR)\palette.obj" : $(SOURCE) $(DEP_CPP_PALET) "$(INTDIR)"\
 "$(INTDIR)\docktool.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_PALET) "$(INTDIR)"\
 "$(INTDIR)\docktool.pch"


!ELSEIF  "$(CFG)" == "DockTool - Power Macintosh Release"


"$(INTDIR)\palette.obj" : $(SOURCE) $(DEP_CPP_PALET) "$(INTDIR)"\
 "$(INTDIR)\docktool.pch"


!ELSEIF  "$(CFG)" == "DockTool - Power Macintosh Debug"


"$(INTDIR)\palette.obj" : $(SOURCE) $(DEP_CPP_PALET) "$(INTDIR)"\
 "$(INTDIR)\docktool.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_PALET) "$(INTDIR)"\
 "$(INTDIR)\docktool.pch"


!ELSEIF  "$(CFG)" == "DockTool - Win32 Unicode Release"


"$(INTDIR)\palette.obj" : $(SOURCE) $(DEP_CPP_PALET) "$(INTDIR)"\
 "$(INTDIR)\docktool.pch"


!ELSEIF  "$(CFG)" == "DockTool - Win32 Unicode Debug"


"$(INTDIR)\palette.obj" : $(SOURCE) $(DEP_CPP_PALET) "$(INTDIR)"\
 "$(INTDIR)\docktool.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_PALET) "$(INTDIR)"\
 "$(INTDIR)\docktool.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\searchbx.cpp
DEP_CPP_SEARC=\
	".\stdafx.h"\
	".\docktool.h"\
	".\searchbx.h"\
	

!IF  "$(CFG)" == "DockTool - Win32 Release"


"$(INTDIR)\searchbx.obj" : $(SOURCE) $(DEP_CPP_SEARC) "$(INTDIR)"\
 "$(INTDIR)\docktool.pch"


!ELSEIF  "$(CFG)" == "DockTool - Win32 Debug"


"$(INTDIR)\searchbx.obj" : $(SOURCE) $(DEP_CPP_SEARC) "$(INTDIR)"\
 "$(INTDIR)\docktool.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_SEARC) "$(INTDIR)"\
 "$(INTDIR)\docktool.pch"


!ELSEIF  "$(CFG)" == "DockTool - Macintosh Release"


"$(INTDIR)\searchbx.obj" : $(SOURCE) $(DEP_CPP_SEARC) "$(INTDIR)"\
 "$(INTDIR)\docktool.pch"


!ELSEIF  "$(CFG)" == "DockTool - Macintosh Debug"


"$(INTDIR)\searchbx.obj" : $(SOURCE) $(DEP_CPP_SEARC) "$(INTDIR)"\
 "$(INTDIR)\docktool.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_SEARC) "$(INTDIR)"\
 "$(INTDIR)\docktool.pch"


!ELSEIF  "$(CFG)" == "DockTool - Power Macintosh Release"


"$(INTDIR)\searchbx.obj" : $(SOURCE) $(DEP_CPP_SEARC) "$(INTDIR)"\
 "$(INTDIR)\docktool.pch"


!ELSEIF  "$(CFG)" == "DockTool - Power Macintosh Debug"


"$(INTDIR)\searchbx.obj" : $(SOURCE) $(DEP_CPP_SEARC) "$(INTDIR)"\
 "$(INTDIR)\docktool.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_SEARC) "$(INTDIR)"\
 "$(INTDIR)\docktool.pch"


!ELSEIF  "$(CFG)" == "DockTool - Win32 Unicode Release"


"$(INTDIR)\searchbx.obj" : $(SOURCE) $(DEP_CPP_SEARC) "$(INTDIR)"\
 "$(INTDIR)\docktool.pch"


!ELSEIF  "$(CFG)" == "DockTool - Win32 Unicode Debug"


"$(INTDIR)\searchbx.obj" : $(SOURCE) $(DEP_CPP_SEARC) "$(INTDIR)"\
 "$(INTDIR)\docktool.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_SEARC) "$(INTDIR)"\
 "$(INTDIR)\docktool.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\stdafx.cpp
DEP_CPP_STDAF=\
	".\stdafx.h"\
	

!IF  "$(CFG)" == "DockTool - Win32 Release"

# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/docktool.pch" /Yc"stdafx.h" /Fo"$(INTDIR)/"\
 /c $(SOURCE) \
	

"$(INTDIR)\stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\docktool.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "DockTool - Win32 Debug"

# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/docktool.pch" /Yc"stdafx.h"\
 /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\docktool.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "DockTool - Macintosh Release"

# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /AL /Q68s /W3 /GX /O2 /D "_WINDOWS" /D "_MAC" /D "_68K_" /D\
 "WIN32" /D "NDEBUG" /D "_MBCS" /Fp"$(INTDIR)/docktool.pch" /Yc"stdafx.h"\
 /Fo"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\docktool.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "DockTool - Macintosh Debug"

# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /AL /Q68s /Q68m /W3 /GX /Zi /Od /D "_WINDOWS" /D "_MAC" /D\
 "_68K_" /D "WIN32" /D "_DEBUG" /D "_MBCS" /Fp"$(INTDIR)/docktool.pch"\
 /Yc"stdafx.h" /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\docktool.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "DockTool - Power Macintosh Release"

# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MD /W3 /GX /O2 /D "_MAC" /D "_MPPC_" /D "_WINDOWS" /D "WIN32"\
 /D "NDEBUG" /D "_MBCS" /D "_AFXDLL" /Fp"$(INTDIR)/docktool.pch" /Yc"stdafx.h"\
 /Fo"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\docktool.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "DockTool - Power Macintosh Debug"

# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MDd /W3 /GX /Zi /Od /D "_MAC" /D "_MPPC_" /D "_WINDOWS" /D\
 "WIN32" /D "_DEBUG" /D "_MBCS" /D "_AFXDLL" /Fp"$(INTDIR)/docktool.pch"\
 /Yc"stdafx.h" /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\docktool.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "DockTool - Win32 Unicode Release"

# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D\
 "_UNICODE" /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/docktool.pch" /Yc"stdafx.h"\
 /Fo"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\docktool.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "DockTool - Win32 Unicode Debug"

# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MDd /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS"\
 /D "_UNICODE" /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/docktool.pch" /Yc"stdafx.h"\
 /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\docktool.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\tooldlg.cpp
DEP_CPP_TOOLD=\
	".\stdafx.h"\
	".\docktool.h"\
	".\tooldlg.h"\
	

!IF  "$(CFG)" == "DockTool - Win32 Release"


"$(INTDIR)\tooldlg.obj" : $(SOURCE) $(DEP_CPP_TOOLD) "$(INTDIR)"\
 "$(INTDIR)\docktool.pch"


!ELSEIF  "$(CFG)" == "DockTool - Win32 Debug"


"$(INTDIR)\tooldlg.obj" : $(SOURCE) $(DEP_CPP_TOOLD) "$(INTDIR)"\
 "$(INTDIR)\docktool.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_TOOLD) "$(INTDIR)"\
 "$(INTDIR)\docktool.pch"


!ELSEIF  "$(CFG)" == "DockTool - Macintosh Release"


"$(INTDIR)\tooldlg.obj" : $(SOURCE) $(DEP_CPP_TOOLD) "$(INTDIR)"\
 "$(INTDIR)\docktool.pch"


!ELSEIF  "$(CFG)" == "DockTool - Macintosh Debug"


"$(INTDIR)\tooldlg.obj" : $(SOURCE) $(DEP_CPP_TOOLD) "$(INTDIR)"\
 "$(INTDIR)\docktool.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_TOOLD) "$(INTDIR)"\
 "$(INTDIR)\docktool.pch"


!ELSEIF  "$(CFG)" == "DockTool - Power Macintosh Release"


"$(INTDIR)\tooldlg.obj" : $(SOURCE) $(DEP_CPP_TOOLD) "$(INTDIR)"\
 "$(INTDIR)\docktool.pch"


!ELSEIF  "$(CFG)" == "DockTool - Power Macintosh Debug"


"$(INTDIR)\tooldlg.obj" : $(SOURCE) $(DEP_CPP_TOOLD) "$(INTDIR)"\
 "$(INTDIR)\docktool.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_TOOLD) "$(INTDIR)"\
 "$(INTDIR)\docktool.pch"


!ELSEIF  "$(CFG)" == "DockTool - Win32 Unicode Release"


"$(INTDIR)\tooldlg.obj" : $(SOURCE) $(DEP_CPP_TOOLD) "$(INTDIR)"\
 "$(INTDIR)\docktool.pch"


!ELSEIF  "$(CFG)" == "DockTool - Win32 Unicode Debug"


"$(INTDIR)\tooldlg.obj" : $(SOURCE) $(DEP_CPP_TOOLD) "$(INTDIR)"\
 "$(INTDIR)\docktool.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_TOOLD) "$(INTDIR)"\
 "$(INTDIR)\docktool.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\dockmac.r

!IF  "$(CFG)" == "DockTool - Win32 Release"

!ELSEIF  "$(CFG)" == "DockTool - Win32 Debug"

!ELSEIF  "$(CFG)" == "DockTool - Macintosh Release"

DEP_MRC_DOCKM=\
	{$(INCLUDE)}"\mrc\types.r"\
	{$(INCLUDE)}"\mrc\balloons.r"\
	

"$(INTDIR)\dockmac.rsc" : $(SOURCE) $(DEP_MRC_DOCKM) "$(INTDIR)"
   $(MRC) /o"$(INTDIR)/dockmac.rsc" /D "_68K_" /D "_MAC" /D "NDEBUG" /l 0x409\
 /NOLOGO $(SOURCE)


!ELSEIF  "$(CFG)" == "DockTool - Macintosh Debug"

DEP_MRC_DOCKM=\
	{$(INCLUDE)}"\mrc\types.r"\
	{$(INCLUDE)}"\mrc\balloons.r"\
	

"$(INTDIR)\dockmac.rsc" : $(SOURCE) $(DEP_MRC_DOCKM) "$(INTDIR)"
   $(MRC) /o"$(INTDIR)/dockmac.rsc" /D "_68K_" /D "_MAC" /D "_DEBUG" /l 0x409\
 /NOLOGO $(SOURCE)


!ELSEIF  "$(CFG)" == "DockTool - Power Macintosh Release"

DEP_MRC_DOCKM=\
	{$(INCLUDE)}"\mrc\types.r"\
	{$(INCLUDE)}"\mrc\balloons.r"\
	

"$(INTDIR)\dockmac.rsc" : $(SOURCE) $(DEP_MRC_DOCKM) "$(INTDIR)"
   $(MRC) /o"$(INTDIR)/dockmac.rsc" /D "_MPPC_" /D "_MAC" /D "NDEBUG" /l 0x409\
 /NOLOGO $(SOURCE)


!ELSEIF  "$(CFG)" == "DockTool - Power Macintosh Debug"

DEP_MRC_DOCKM=\
	{$(INCLUDE)}"\mrc\types.r"\
	{$(INCLUDE)}"\mrc\balloons.r"\
	

"$(INTDIR)\dockmac.rsc" : $(SOURCE) $(DEP_MRC_DOCKM) "$(INTDIR)"
   $(MRC) /o"$(INTDIR)/dockmac.rsc" /D "_MPPC_" /D "_MAC" /D "_DEBUG" /l 0x409\
 /NOLOGO $(SOURCE)


!ELSEIF  "$(CFG)" == "DockTool - Win32 Unicode Release"

!ELSEIF  "$(CFG)" == "DockTool - Win32 Unicode Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\readme.txt

!IF  "$(CFG)" == "DockTool - Win32 Release"

!ELSEIF  "$(CFG)" == "DockTool - Win32 Debug"

!ELSEIF  "$(CFG)" == "DockTool - Macintosh Release"

!ELSEIF  "$(CFG)" == "DockTool - Macintosh Debug"

!ELSEIF  "$(CFG)" == "DockTool - Power Macintosh Release"

!ELSEIF  "$(CFG)" == "DockTool - Power Macintosh Debug"

!ELSEIF  "$(CFG)" == "DockTool - Win32 Unicode Release"

!ELSEIF  "$(CFG)" == "DockTool - Win32 Unicode Debug"

!ENDIF 

# End Source File
# End Target
# End Project
################################################################################
