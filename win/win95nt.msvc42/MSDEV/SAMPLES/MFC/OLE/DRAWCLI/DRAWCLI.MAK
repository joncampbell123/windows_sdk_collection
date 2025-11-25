# Microsoft Visual C++ Generated NMAKE File, Format Version 40000
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101
# TARGTYPE "Power Macintosh Application" 0x0401
# TARGTYPE "Macintosh Application" 0x0301

!IF "$(CFG)" == ""
CFG=DrawCli - Win32 Debug
!MESSAGE No configuration specified.  Defaulting to DrawCli - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "DrawCli - Win32 Release" && "$(CFG)" !=\
 "DrawCli - Win32 Debug" && "$(CFG)" != "DrawCli - Macintosh Release" &&\
 "$(CFG)" != "DrawCli - Macintosh Debug" && "$(CFG)" !=\
 "DrawCli - Power Macintosh Release" && "$(CFG)" !=\
 "DrawCli - Power Macintosh Debug" && "$(CFG)" !=\
 "DrawCli - Win32 Unicode Release" && "$(CFG)" !=\
 "DrawCli - Win32 Unicode Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "drawcli.mak" CFG="DrawCli - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "DrawCli - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "DrawCli - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE "DrawCli - Macintosh Release" (based on "Macintosh Application")
!MESSAGE "DrawCli - Macintosh Debug" (based on "Macintosh Application")
!MESSAGE "DrawCli - Power Macintosh Release" (based on\
 "Power Macintosh Application")
!MESSAGE "DrawCli - Power Macintosh Debug" (based on\
 "Power Macintosh Application")
!MESSAGE "DrawCli - Win32 Unicode Release" (based on "Win32 (x86) Application")
!MESSAGE "DrawCli - Win32 Unicode Debug" (based on "Win32 (x86) Application")
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
# PROP Target_Last_Scanned "DrawCli - Win32 Unicode Debug"

!IF  "$(CFG)" == "DrawCli - Win32 Release"

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

ALL : "$(OUTDIR)\drawcli.exe"

CLEAN : 
	-@erase ".\Release\drawcli.exe"
	-@erase ".\Release\stdafx.obj"
	-@erase ".\Release\drawcli.obj"
	-@erase ".\Release\stdafx.pch"
	-@erase ".\Release\summpage.obj"
	-@erase ".\Release\rectdlg.obj"
	-@erase ".\Release\drawobj.obj"
	-@erase ".\Release\drawtool.obj"
	-@erase ".\Release\mainfrm.obj"
	-@erase ".\Release\statpage.obj"
	-@erase ".\Release\propset.obj"
	-@erase ".\Release\drawdoc.obj"
	-@erase ".\Release\splitfrm.obj"
	-@erase ".\Release\drawvw.obj"
	-@erase ".\Release\summinfo.obj"
	-@erase ".\Release\cntritem.obj"
	-@erase ".\Release\drawcli.res"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /FR /YX /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Fp"Release/stdafx.pch" /Yu"stdafx.h" /c
CPP_PROJ=/nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/stdafx.pch" /Yu"stdafx.h" /Fo"$(INTDIR)/" /c\
 
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
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/drawcli.res" /d "NDEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/drawcli.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 /nologo /subsystem:windows /machine:I386
LINK32_FLAGS=/nologo /subsystem:windows /incremental:no\
 /pdb:"$(OUTDIR)/drawcli.pdb" /machine:I386 /out:"$(OUTDIR)/drawcli.exe" 
LINK32_OBJS= \
	"$(INTDIR)/stdafx.obj" \
	"$(INTDIR)/drawcli.obj" \
	"$(INTDIR)/summpage.obj" \
	"$(INTDIR)/rectdlg.obj" \
	"$(INTDIR)/drawobj.obj" \
	"$(INTDIR)/drawtool.obj" \
	"$(INTDIR)/mainfrm.obj" \
	"$(INTDIR)/statpage.obj" \
	"$(INTDIR)/propset.obj" \
	"$(INTDIR)/drawdoc.obj" \
	"$(INTDIR)/splitfrm.obj" \
	"$(INTDIR)/drawvw.obj" \
	"$(INTDIR)/summinfo.obj" \
	"$(INTDIR)/cntritem.obj" \
	"$(INTDIR)/drawcli.res"

"$(OUTDIR)\drawcli.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "DrawCli - Win32 Debug"

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

ALL : "$(OUTDIR)\drawcli.exe"

CLEAN : 
	-@erase ".\Debug\vc40.pdb"
	-@erase ".\Debug\stdafx.pch"
	-@erase ".\Debug\drawcli.exe"
	-@erase ".\Debug\summinfo.obj"
	-@erase ".\Debug\drawvw.obj"
	-@erase ".\Debug\summpage.obj"
	-@erase ".\Debug\stdafx.obj"
	-@erase ".\Debug\drawtool.obj"
	-@erase ".\Debug\drawcli.obj"
	-@erase ".\Debug\rectdlg.obj"
	-@erase ".\Debug\drawobj.obj"
	-@erase ".\Debug\statpage.obj"
	-@erase ".\Debug\mainfrm.obj"
	-@erase ".\Debug\propset.obj"
	-@erase ".\Debug\splitfrm.obj"
	-@erase ".\Debug\drawdoc.obj"
	-@erase ".\Debug\cntritem.obj"
	-@erase ".\Debug\drawcli.res"
	-@erase ".\Debug\drawcli.ilk"
	-@erase ".\Debug\drawcli.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /FR /YX /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Fp"Debug/stdafx.pch" /Yu"stdafx.h" /c
CPP_PROJ=/nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/stdafx.pch" /Yu"stdafx.h" /Fo"$(INTDIR)/"\
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
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/drawcli.res" /d "_DEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/drawcli.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386
# ADD LINK32 /nologo /subsystem:windows /debug /machine:I386
LINK32_FLAGS=/nologo /subsystem:windows /incremental:yes\
 /pdb:"$(OUTDIR)/drawcli.pdb" /debug /machine:I386 /out:"$(OUTDIR)/drawcli.exe" 
LINK32_OBJS= \
	"$(INTDIR)/summinfo.obj" \
	"$(INTDIR)/drawvw.obj" \
	"$(INTDIR)/summpage.obj" \
	"$(INTDIR)/stdafx.obj" \
	"$(INTDIR)/drawtool.obj" \
	"$(INTDIR)/drawcli.obj" \
	"$(INTDIR)/rectdlg.obj" \
	"$(INTDIR)/drawobj.obj" \
	"$(INTDIR)/statpage.obj" \
	"$(INTDIR)/mainfrm.obj" \
	"$(INTDIR)/propset.obj" \
	"$(INTDIR)/splitfrm.obj" \
	"$(INTDIR)/drawdoc.obj" \
	"$(INTDIR)/cntritem.obj" \
	"$(INTDIR)/drawcli.res"

"$(OUTDIR)\drawcli.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "DrawCli - Macintosh Release"

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

ALL : "$(OUTDIR)\drawcli.exe" "$(OUTDIR)\drawcli.trg"

CLEAN : 
	-@erase ".\MacRel\drawcli.trg"
	-@erase ".\MacRel\drawcli.exe"
	-@erase ".\MacRel\splitfrm.obj"
	-@erase ".\MacRel\stdafx.pch"
	-@erase ".\MacRel\drawcli.obj"
	-@erase ".\MacRel\summinfo.obj"
	-@erase ".\MacRel\rectdlg.obj"
	-@erase ".\MacRel\cntritem.obj"
	-@erase ".\MacRel\drawobj.obj"
	-@erase ".\MacRel\mainfrm.obj"
	-@erase ".\MacRel\summpage.obj"
	-@erase ".\MacRel\propset.obj"
	-@erase ".\MacRel\drawvw.obj"
	-@erase ".\MacRel\drawdoc.obj"
	-@erase ".\MacRel\drawtool.obj"
	-@erase ".\MacRel\stdafx.obj"
	-@erase ".\MacRel\statpage.obj"
	-@erase ".\MacRel\drawcli.rsc"
	-@erase ".\MacRel\dclimac.rsc"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

MTL=mktyplib.exe
# ADD BASE MTL /nologo /D "NDEBUG" /mac
# ADD MTL /nologo /D "NDEBUG" /mac
MTL_PROJ=/nologo /D "NDEBUG" /mac 
CPP=cl.exe
# ADD BASE CPP /nologo /AL /Q68s /W3 /GX /O2 /D "_WINDOWS" /D "_MAC" /D "_68K_" /D "WIN32" /D "NDEBUG" /FR /YX /c
# ADD CPP /nologo /AL /Q68s /W3 /GX /O2 /D "_WINDOWS" /D "_MAC" /D "_68K_" /D "WIN32" /D "NDEBUG" /D "_MBCS" /Fp"MacRel/stdafx.pch" /Yu"stdafx.h" /c
# SUBTRACT CPP /Fr
CPP_PROJ=/nologo /AL /Q68s /W3 /GX /O2 /D "_WINDOWS" /D "_MAC" /D "_68K_" /D\
 "WIN32" /D "NDEBUG" /D "_MBCS" /Fp"$(INTDIR)/stdafx.pch" /Yu"stdafx.h"\
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
RSC_PROJ=/l 0x409 /r /m /fo"$(INTDIR)/drawcli.rsc" /d "_MAC" /d "_68K_" /d\
 "NDEBUG" 
MRC=mrc.exe
# ADD BASE MRC /D "_68K_" /D "_MAC" /D "NDEBUG" /NOLOGO
# ADD MRC /D "_68K_" /D "_MAC" /D "NDEBUG" /NOLOGO
MRC_PROJ=/D "_68K_" /D "_MAC" /D "NDEBUG" /l 0x409 /NOLOGO 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/drawcli.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 wlm.lib /nologo /MAC:bundle /machine:M68K
# ADD LINK32 /nologo /MAC:bundle /MAC:creator="DCLI" /machine:M68K
LINK32_FLAGS=/nologo /MAC:bundle /MAC:type="APPL" /MAC:creator="DCLI"\
 /pdb:"$(OUTDIR)/drawcli.pdb" /machine:M68K /out:"$(OUTDIR)/drawcli.exe" 
LINK32_OBJS= \
	"$(INTDIR)/splitfrm.obj" \
	"$(INTDIR)/drawcli.obj" \
	"$(INTDIR)/summinfo.obj" \
	"$(INTDIR)/rectdlg.obj" \
	"$(INTDIR)/cntritem.obj" \
	"$(INTDIR)/drawobj.obj" \
	"$(INTDIR)/mainfrm.obj" \
	"$(INTDIR)/summpage.obj" \
	"$(INTDIR)/propset.obj" \
	"$(INTDIR)/drawvw.obj" \
	"$(INTDIR)/drawdoc.obj" \
	"$(INTDIR)/drawtool.obj" \
	"$(INTDIR)/stdafx.obj" \
	"$(INTDIR)/statpage.obj" \
	"$(INTDIR)/drawcli.rsc" \
	"$(INTDIR)/dclimac.rsc"

"$(OUTDIR)\drawcli.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

MFILE32=mfile.exe
# ADD BASE MFILE32 COPY /NOLOGO
# ADD MFILE32 COPY /NOLOGO
MFILE32_FLAGS=COPY /NOLOGO 
MFILE32_FILES= \
	"$(OUTDIR)/drawcli.exe"

"$(OUTDIR)\drawcli.trg" : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\MacRel\drawcli.exe\
 "$(MFILE32_DEST):drawcli.exe">"$(OUTDIR)\drawcli.trg"

DOWNLOAD : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\MacRel\drawcli.exe\
 "$(MFILE32_DEST):drawcli.exe">"$(OUTDIR)\drawcli.trg"

!ELSEIF  "$(CFG)" == "DrawCli - Macintosh Debug"

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

ALL : "$(OUTDIR)\drawcli.exe" "$(OUTDIR)\drawcli.trg"

CLEAN : 
	-@erase ".\MacDbg\vc40.pdb"
	-@erase ".\MacDbg\drawcli.pch"
	-@erase ".\MacDbg\drawcli.pdb"
	-@erase ".\MacDbg\splitfrm.obj"
	-@erase ".\MacDbg\summinfo.obj"
	-@erase ".\MacDbg\cntritem.obj"
	-@erase ".\MacDbg\drawcli.obj"
	-@erase ".\MacDbg\rectdlg.obj"
	-@erase ".\MacDbg\drawvw.obj"
	-@erase ".\MacDbg\drawobj.obj"
	-@erase ".\MacDbg\summpage.obj"
	-@erase ".\MacDbg\mainfrm.obj"
	-@erase ".\MacDbg\stdafx.obj"
	-@erase ".\MacDbg\drawtool.obj"
	-@erase ".\MacDbg\propset.obj"
	-@erase ".\MacDbg\statpage.obj"
	-@erase ".\MacDbg\drawdoc.obj"
	-@erase ".\MacDbg\drawcli.rsc"
	-@erase ".\MacDbg\dclimac.rsc"
	-@erase ".\MacDbg\drawcli.trg"
	-@erase ".\MacDbg\drawcli.exe"

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
 "_68K_" /D "WIN32" /D "_DEBUG" /D "_MBCS" /Fp"$(INTDIR)/drawcli.pch"\
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
RSC_PROJ=/l 0x409 /r /m /fo"$(INTDIR)/drawcli.rsc" /d "_MAC" /d "_68K_" /d\
 "_DEBUG" 
MRC=mrc.exe
# ADD BASE MRC /D "_68K_" /D "_MAC" /D "_DEBUG" /NOLOGO
# ADD MRC /D "_68K_" /D "_MAC" /D "_DEBUG" /NOLOGO
MRC_PROJ=/D "_68K_" /D "_MAC" /D "_DEBUG" /l 0x409 /NOLOGO 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/drawcli.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 wlm.lib /nologo /MAC:bundle /debug /machine:M68K
# ADD LINK32 /nologo /MAC:bundle /MAC:creator="DCLI" /debug /machine:M68K
LINK32_FLAGS=/nologo /MAC:bundle /MAC:type="APPL" /MAC:creator="DCLI"\
 /pdb:"$(OUTDIR)/drawcli.pdb" /debug /machine:M68K /out:"$(OUTDIR)/drawcli.exe" 
LINK32_OBJS= \
	"$(INTDIR)/splitfrm.obj" \
	"$(INTDIR)/summinfo.obj" \
	"$(INTDIR)/cntritem.obj" \
	"$(INTDIR)/drawcli.obj" \
	"$(INTDIR)/rectdlg.obj" \
	"$(INTDIR)/drawvw.obj" \
	"$(INTDIR)/drawobj.obj" \
	"$(INTDIR)/summpage.obj" \
	"$(INTDIR)/mainfrm.obj" \
	"$(INTDIR)/stdafx.obj" \
	"$(INTDIR)/drawtool.obj" \
	"$(INTDIR)/propset.obj" \
	"$(INTDIR)/statpage.obj" \
	"$(INTDIR)/drawdoc.obj" \
	"$(INTDIR)/drawcli.rsc" \
	"$(INTDIR)/dclimac.rsc"

"$(OUTDIR)\drawcli.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

MFILE32=mfile.exe
# ADD BASE MFILE32 COPY /NOLOGO
# ADD MFILE32 COPY /NOLOGO
MFILE32_FLAGS=COPY /NOLOGO 
MFILE32_FILES= \
	"$(OUTDIR)/drawcli.exe"

"$(OUTDIR)\drawcli.trg" : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\MacDbg\drawcli.exe\
 "$(MFILE32_DEST):drawcli.exe">"$(OUTDIR)\drawcli.trg"

DOWNLOAD : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\MacDbg\drawcli.exe\
 "$(MFILE32_DEST):drawcli.exe">"$(OUTDIR)\drawcli.trg"

!ELSEIF  "$(CFG)" == "DrawCli - Power Macintosh Release"

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

ALL : "$(OUTDIR)\drawcli.exe" "$(OUTDIR)\drawcli.trg"

CLEAN : 
	-@erase ".\PMacRel\drawcli.trg"
	-@erase ".\PMacRel\drawcli.exe"
	-@erase ".\PMacRel\statpage.obj"
	-@erase ".\PMacRel\stdafx.pch"
	-@erase ".\PMacRel\rectdlg.obj"
	-@erase ".\PMacRel\drawobj.obj"
	-@erase ".\PMacRel\mainfrm.obj"
	-@erase ".\PMacRel\splitfrm.obj"
	-@erase ".\PMacRel\summinfo.obj"
	-@erase ".\PMacRel\cntritem.obj"
	-@erase ".\PMacRel\propset.obj"
	-@erase ".\PMacRel\drawvw.obj"
	-@erase ".\PMacRel\drawdoc.obj"
	-@erase ".\PMacRel\stdafx.obj"
	-@erase ".\PMacRel\summpage.obj"
	-@erase ".\PMacRel\drawtool.obj"
	-@erase ".\PMacRel\drawcli.obj"
	-@erase ".\PMacRel\drawcli.rsc"
	-@erase ".\PMacRel\dclimac.rsc"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

MTL=mktyplib.exe
# ADD BASE MTL /nologo /D "NDEBUG"
# ADD MTL /nologo /D "NDEBUG"
MTL_PROJ=/nologo /D "NDEBUG" 
CPP=cl.exe
# ADD BASE CPP /nologo /W3 /GX /O2 /D "_MAC" /D "_MPPC_" /D "_WINDOWS" /D "WIN32" /D "NDEBUG" /FR /YX /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "_MAC" /D "_MPPC_" /D "_WINDOWS" /D "WIN32" /D "_MBCS" /D "_AFXDLL" /Fp"PMacRel/stdafx.pch" /Yu"stdafx.h" /c
# SUBTRACT CPP /Fr
CPP_PROJ=/nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "_MAC" /D "_MPPC_" /D\
 "_WINDOWS" /D "WIN32" /D "_MBCS" /D "_AFXDLL" /Fp"$(INTDIR)/stdafx.pch"\
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
RSC_PROJ=/l 0x409 /r /m /fo"$(INTDIR)/drawcli.rsc" /d "_MAC" /d "_MPPC_" /d\
 "NDEBUG" 
MRC=mrc.exe
# ADD BASE MRC /D "_MPPC_" /D "_MAC" /D "NDEBUG" /NOLOGO
# ADD MRC /D "_MPPC_" /D "_MAC" /D "NDEBUG" /NOLOGO
MRC_PROJ=/D "_MPPC_" /D "_MAC" /D "NDEBUG" /l 0x409 /NOLOGO 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/drawcli.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 cfrg.rsc /nologo /mac:bundle /machine:MPPC
# ADD LINK32 /nologo /mac:bundle /mac:creator="DCLI" /machine:MPPC
LINK32_FLAGS=/nologo /mac:bundle /mac:type="APPL" /mac:creator="DCLI"\
 /incremental:no /pdb:"$(OUTDIR)/drawcli.pdb" /machine:MPPC\
 /out:"$(OUTDIR)/drawcli.exe" 
LINK32_OBJS= \
	"$(INTDIR)/statpage.obj" \
	"$(INTDIR)/rectdlg.obj" \
	"$(INTDIR)/drawobj.obj" \
	"$(INTDIR)/mainfrm.obj" \
	"$(INTDIR)/splitfrm.obj" \
	"$(INTDIR)/summinfo.obj" \
	"$(INTDIR)/cntritem.obj" \
	"$(INTDIR)/propset.obj" \
	"$(INTDIR)/drawvw.obj" \
	"$(INTDIR)/drawdoc.obj" \
	"$(INTDIR)/stdafx.obj" \
	"$(INTDIR)/summpage.obj" \
	"$(INTDIR)/drawtool.obj" \
	"$(INTDIR)/drawcli.obj" \
	"$(INTDIR)/drawcli.rsc" \
	"$(INTDIR)/dclimac.rsc"

"$(OUTDIR)\drawcli.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

MFILE32=mfile.exe
# ADD BASE MFILE32 COPY /NOLOGO
# ADD MFILE32 COPY /NOLOGO
MFILE32_FLAGS=COPY /NOLOGO 
MFILE32_FILES= \
	"$(OUTDIR)/drawcli.exe"

"$(OUTDIR)\drawcli.trg" : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\PMacRel\drawcli.exe\
 "$(MFILE32_DEST):drawcli.exe">"$(OUTDIR)\drawcli.trg"

DOWNLOAD : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\PMacRel\drawcli.exe\
 "$(MFILE32_DEST):drawcli.exe">"$(OUTDIR)\drawcli.trg"

!ELSEIF  "$(CFG)" == "DrawCli - Power Macintosh Debug"

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

ALL : "$(OUTDIR)\drawcli.exe" "$(OUTDIR)\drawcli.trg"

CLEAN : 
	-@erase ".\PMacDbg\vc40.pdb"
	-@erase ".\PMacDbg\stdafx.pch"
	-@erase ".\PMacDbg\drawcli.ilk"
	-@erase ".\PMacDbg\statpage.obj"
	-@erase ".\PMacDbg\drawcli.obj"
	-@erase ".\PMacDbg\splitfrm.obj"
	-@erase ".\PMacDbg\rectdlg.obj"
	-@erase ".\PMacDbg\drawvw.obj"
	-@erase ".\PMacDbg\summinfo.obj"
	-@erase ".\PMacDbg\drawobj.obj"
	-@erase ".\PMacDbg\cntritem.obj"
	-@erase ".\PMacDbg\stdafx.obj"
	-@erase ".\PMacDbg\mainfrm.obj"
	-@erase ".\PMacDbg\propset.obj"
	-@erase ".\PMacDbg\summpage.obj"
	-@erase ".\PMacDbg\drawdoc.obj"
	-@erase ".\PMacDbg\drawtool.obj"
	-@erase ".\PMacDbg\dclimac.rsc"
	-@erase ".\PMacDbg\drawcli.rsc"
	-@erase ".\PMacDbg\drawcli.pdb"
	-@erase ".\PMacDbg\drawcli.trg"
	-@erase ".\PMacDbg\drawcli.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

MTL=mktyplib.exe
# ADD BASE MTL /nologo /D "_DEBUG"
# ADD MTL /nologo /D "_DEBUG"
MTL_PROJ=/nologo /D "_DEBUG" 
CPP=cl.exe
# ADD BASE CPP /nologo /W3 /GX /Zi /Od /D "_MAC" /D "_MPPC_" /D "_WINDOWS" /D "WIN32" /D "_DEBUG" /FR /YX /c
# ADD CPP /nologo /MDd /W3 /GX /Zi /Od /D "_DEBUG" /D "_MAC" /D "_MPPC_" /D "_WINDOWS" /D "WIN32" /D "_MBCS" /D "_AFXDLL" /Fp"PMacDbg/stdafx.pch" /Yu"stdafx.h" /c
CPP_PROJ=/nologo /MDd /W3 /GX /Zi /Od /D "_DEBUG" /D "_MAC" /D "_MPPC_" /D\
 "_WINDOWS" /D "WIN32" /D "_MBCS" /D "_AFXDLL" /Fp"$(INTDIR)/stdafx.pch"\
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
RSC_PROJ=/l 0x409 /r /m /fo"$(INTDIR)/drawcli.rsc" /d "_MAC" /d "_MPPC_" /d\
 "_DEBUG" 
MRC=mrc.exe
# ADD BASE MRC /D "_MPPC_" /D "_MAC" /D "_DEBUG" /NOLOGO
# ADD MRC /D "_MPPC_" /D "_MAC" /D "_DEBUG" /NOLOGO
MRC_PROJ=/D "_MPPC_" /D "_MAC" /D "_DEBUG" /l 0x409 /NOLOGO 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/drawcli.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 cfrg.rsc /nologo /mac:bundle /debug /machine:MPPC
# ADD LINK32 /nologo /mac:bundle /mac:creator="DCLI" /debug /machine:MPPC
LINK32_FLAGS=/nologo /mac:bundle /mac:type="APPL" /mac:creator="DCLI"\
 /incremental:yes /pdb:"$(OUTDIR)/drawcli.pdb" /debug /machine:MPPC\
 /out:"$(OUTDIR)/drawcli.exe" 
LINK32_OBJS= \
	"$(INTDIR)/statpage.obj" \
	"$(INTDIR)/drawcli.obj" \
	"$(INTDIR)/splitfrm.obj" \
	"$(INTDIR)/rectdlg.obj" \
	"$(INTDIR)/drawvw.obj" \
	"$(INTDIR)/summinfo.obj" \
	"$(INTDIR)/drawobj.obj" \
	"$(INTDIR)/cntritem.obj" \
	"$(INTDIR)/stdafx.obj" \
	"$(INTDIR)/mainfrm.obj" \
	"$(INTDIR)/propset.obj" \
	"$(INTDIR)/summpage.obj" \
	"$(INTDIR)/drawdoc.obj" \
	"$(INTDIR)/drawtool.obj" \
	"$(INTDIR)/dclimac.rsc" \
	"$(INTDIR)/drawcli.rsc"

"$(OUTDIR)\drawcli.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

MFILE32=mfile.exe
# ADD BASE MFILE32 COPY /NOLOGO
# ADD MFILE32 COPY /NOLOGO
MFILE32_FLAGS=COPY /NOLOGO 
MFILE32_FILES= \
	"$(OUTDIR)/drawcli.exe"

"$(OUTDIR)\drawcli.trg" : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\PMacDbg\drawcli.exe\
 "$(MFILE32_DEST):drawcli.exe">"$(OUTDIR)\drawcli.trg"

DOWNLOAD : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\PMacDbg\drawcli.exe\
 "$(MFILE32_DEST):drawcli.exe">"$(OUTDIR)\drawcli.trg"

!ELSEIF  "$(CFG)" == "DrawCli - Win32 Unicode Release"

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

ALL : "$(OUTDIR)\drawcli.exe"

CLEAN : 
	-@erase ".\UniRelease\drawcli.exe"
	-@erase ".\UniRelease\propset.obj"
	-@erase ".\UniRel\stdafx.pch"
	-@erase ".\UniRelease\summpage.obj"
	-@erase ".\UniRelease\drawdoc.obj"
	-@erase ".\UniRelease\stdafx.obj"
	-@erase ".\UniRelease\drawtool.obj"
	-@erase ".\UniRelease\statpage.obj"
	-@erase ".\UniRelease\drawcli.obj"
	-@erase ".\UniRelease\splitfrm.obj"
	-@erase ".\UniRelease\rectdlg.obj"
	-@erase ".\UniRelease\drawobj.obj"
	-@erase ".\UniRelease\cntritem.obj"
	-@erase ".\UniRelease\summinfo.obj"
	-@erase ".\UniRelease\mainfrm.obj"
	-@erase ".\UniRelease\drawvw.obj"
	-@erase ".\UniRelease\drawcli.res"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /FR /YX /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /D "_UNICODE" /Fp"UniRel/stdafx.pch" /Yu"stdafx.h" /c
# SUBTRACT CPP /Fr
CPP_PROJ=/nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /D "_UNICODE" /Fp"UniRel/stdafx.pch" /Yu"stdafx.h"\
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
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/drawcli.res" /d "NDEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/drawcli.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 /nologo /entry:"wWinMainCRTStartup" /subsystem:windows /machine:I386
LINK32_FLAGS=/nologo /entry:"wWinMainCRTStartup" /subsystem:windows\
 /incremental:no /pdb:"$(OUTDIR)/drawcli.pdb" /machine:I386\
 /out:"$(OUTDIR)/drawcli.exe" 
LINK32_OBJS= \
	"$(INTDIR)/propset.obj" \
	"$(INTDIR)/summpage.obj" \
	"$(INTDIR)/drawdoc.obj" \
	"$(INTDIR)/stdafx.obj" \
	"$(INTDIR)/drawtool.obj" \
	"$(INTDIR)/statpage.obj" \
	"$(INTDIR)/drawcli.obj" \
	"$(INTDIR)/splitfrm.obj" \
	"$(INTDIR)/rectdlg.obj" \
	"$(INTDIR)/drawobj.obj" \
	"$(INTDIR)/cntritem.obj" \
	"$(INTDIR)/summinfo.obj" \
	"$(INTDIR)/mainfrm.obj" \
	"$(INTDIR)/drawvw.obj" \
	"$(INTDIR)/drawcli.res"

"$(OUTDIR)\drawcli.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "DrawCli - Win32 Unicode Debug"

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

ALL : "$(OUTDIR)\drawcli.exe"

CLEAN : 
	-@erase ".\UniDebug\vc40.pdb"
	-@erase ".\UniDebug\stdafx.pch"
	-@erase ".\UniDebug\drawcli.exe"
	-@erase ".\UniDebug\stdafx.obj"
	-@erase ".\UniDebug\splitfrm.obj"
	-@erase ".\UniDebug\cntritem.obj"
	-@erase ".\UniDebug\summinfo.obj"
	-@erase ".\UniDebug\drawcli.obj"
	-@erase ".\UniDebug\summpage.obj"
	-@erase ".\UniDebug\rectdlg.obj"
	-@erase ".\UniDebug\drawtool.obj"
	-@erase ".\UniDebug\drawobj.obj"
	-@erase ".\UniDebug\mainfrm.obj"
	-@erase ".\UniDebug\propset.obj"
	-@erase ".\UniDebug\statpage.obj"
	-@erase ".\UniDebug\drawvw.obj"
	-@erase ".\UniDebug\drawdoc.obj"
	-@erase ".\UniDebug\drawcli.res"
	-@erase ".\UniDebug\drawcli.ilk"
	-@erase ".\UniDebug\drawcli.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /FR /YX /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /D "_UNICODE" /Fp"UniDebug/stdafx.pch" /Yu"stdafx.h" /c
# SUBTRACT CPP /Fr
CPP_PROJ=/nologo /MDd /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS"\
 /D "_AFXDLL" /D "_MBCS" /D "_UNICODE" /Fp"$(INTDIR)/stdafx.pch" /Yu"stdafx.h"\
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
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/drawcli.res" /d "_DEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/drawcli.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386
# ADD LINK32 /nologo /entry:"wWinMainCRTStartup" /subsystem:windows /debug /machine:I386
LINK32_FLAGS=/nologo /entry:"wWinMainCRTStartup" /subsystem:windows\
 /incremental:yes /pdb:"$(OUTDIR)/drawcli.pdb" /debug /machine:I386\
 /out:"$(OUTDIR)/drawcli.exe" 
LINK32_OBJS= \
	"$(INTDIR)/stdafx.obj" \
	"$(INTDIR)/splitfrm.obj" \
	"$(INTDIR)/cntritem.obj" \
	"$(INTDIR)/summinfo.obj" \
	"$(INTDIR)/drawcli.obj" \
	"$(INTDIR)/summpage.obj" \
	"$(INTDIR)/rectdlg.obj" \
	"$(INTDIR)/drawtool.obj" \
	"$(INTDIR)/drawobj.obj" \
	"$(INTDIR)/mainfrm.obj" \
	"$(INTDIR)/propset.obj" \
	"$(INTDIR)/statpage.obj" \
	"$(INTDIR)/drawvw.obj" \
	"$(INTDIR)/drawdoc.obj" \
	"$(INTDIR)/drawcli.res"

"$(OUTDIR)\drawcli.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 

################################################################################
# Begin Target

# Name "DrawCli - Win32 Release"
# Name "DrawCli - Win32 Debug"
# Name "DrawCli - Macintosh Release"
# Name "DrawCli - Macintosh Debug"
# Name "DrawCli - Power Macintosh Release"
# Name "DrawCli - Power Macintosh Debug"
# Name "DrawCli - Win32 Unicode Release"
# Name "DrawCli - Win32 Unicode Debug"

!IF  "$(CFG)" == "DrawCli - Win32 Release"

!ELSEIF  "$(CFG)" == "DrawCli - Win32 Debug"

!ELSEIF  "$(CFG)" == "DrawCli - Macintosh Release"

!ELSEIF  "$(CFG)" == "DrawCli - Macintosh Debug"

!ELSEIF  "$(CFG)" == "DrawCli - Power Macintosh Release"

!ELSEIF  "$(CFG)" == "DrawCli - Power Macintosh Debug"

!ELSEIF  "$(CFG)" == "DrawCli - Win32 Unicode Release"

!ELSEIF  "$(CFG)" == "DrawCli - Win32 Unicode Debug"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\cntritem.cpp
DEP_CPP_CNTRI=\
	".\cntritem.h"\
	".\drawcli.h"\
	".\drawdoc.h"\
	".\drawobj.h"\
	".\summinfo.h"\
	".\propset.h"\
	".\drawvw.h"\
	".\stdafx.h"\
	

!IF  "$(CFG)" == "DrawCli - Win32 Release"


"$(INTDIR)\cntritem.obj" : $(SOURCE) $(DEP_CPP_CNTRI) "$(INTDIR)"\
 "$(INTDIR)\stdafx.pch"


!ELSEIF  "$(CFG)" == "DrawCli - Win32 Debug"


"$(INTDIR)\cntritem.obj" : $(SOURCE) $(DEP_CPP_CNTRI) "$(INTDIR)"\
 "$(INTDIR)\stdafx.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_CNTRI) "$(INTDIR)"\
 "$(INTDIR)\stdafx.pch"


!ELSEIF  "$(CFG)" == "DrawCli - Macintosh Release"


"$(INTDIR)\cntritem.obj" : $(SOURCE) $(DEP_CPP_CNTRI) "$(INTDIR)"\
 "$(INTDIR)\stdafx.pch"


!ELSEIF  "$(CFG)" == "DrawCli - Macintosh Debug"


"$(INTDIR)\cntritem.obj" : $(SOURCE) $(DEP_CPP_CNTRI) "$(INTDIR)"\
 "$(INTDIR)\drawcli.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_CNTRI) "$(INTDIR)"\
 "$(INTDIR)\drawcli.pch"


!ELSEIF  "$(CFG)" == "DrawCli - Power Macintosh Release"


"$(INTDIR)\cntritem.obj" : $(SOURCE) $(DEP_CPP_CNTRI) "$(INTDIR)"\
 "$(INTDIR)\stdafx.pch"


!ELSEIF  "$(CFG)" == "DrawCli - Power Macintosh Debug"


"$(INTDIR)\cntritem.obj" : $(SOURCE) $(DEP_CPP_CNTRI) "$(INTDIR)"\
 "$(INTDIR)\stdafx.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_CNTRI) "$(INTDIR)"\
 "$(INTDIR)\stdafx.pch"


!ELSEIF  "$(CFG)" == "DrawCli - Win32 Unicode Release"


"$(INTDIR)\cntritem.obj" : $(SOURCE) $(DEP_CPP_CNTRI) "$(INTDIR)"\
 ".\UniRel\stdafx.pch"


!ELSEIF  "$(CFG)" == "DrawCli - Win32 Unicode Debug"


"$(INTDIR)\cntritem.obj" : $(SOURCE) $(DEP_CPP_CNTRI) "$(INTDIR)"\
 "$(INTDIR)\stdafx.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_CNTRI) "$(INTDIR)"\
 "$(INTDIR)\stdafx.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\drawcli.cpp
DEP_CPP_DRAWC=\
	".\drawcli.h"\
	".\drawdoc.h"\
	".\drawobj.h"\
	".\summinfo.h"\
	".\propset.h"\
	".\drawvw.h"\
	".\mainfrm.h"\
	".\splitfrm.h"\
	".\stdafx.h"\
	

!IF  "$(CFG)" == "DrawCli - Win32 Release"


"$(INTDIR)\drawcli.obj" : $(SOURCE) $(DEP_CPP_DRAWC) "$(INTDIR)"\
 "$(INTDIR)\stdafx.pch"


!ELSEIF  "$(CFG)" == "DrawCli - Win32 Debug"


"$(INTDIR)\drawcli.obj" : $(SOURCE) $(DEP_CPP_DRAWC) "$(INTDIR)"\
 "$(INTDIR)\stdafx.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_DRAWC) "$(INTDIR)"\
 "$(INTDIR)\stdafx.pch"


!ELSEIF  "$(CFG)" == "DrawCli - Macintosh Release"


"$(INTDIR)\drawcli.obj" : $(SOURCE) $(DEP_CPP_DRAWC) "$(INTDIR)"\
 "$(INTDIR)\stdafx.pch"


!ELSEIF  "$(CFG)" == "DrawCli - Macintosh Debug"


"$(INTDIR)\drawcli.obj" : $(SOURCE) $(DEP_CPP_DRAWC) "$(INTDIR)"\
 "$(INTDIR)\drawcli.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_DRAWC) "$(INTDIR)"\
 "$(INTDIR)\drawcli.pch"


!ELSEIF  "$(CFG)" == "DrawCli - Power Macintosh Release"


"$(INTDIR)\drawcli.obj" : $(SOURCE) $(DEP_CPP_DRAWC) "$(INTDIR)"\
 "$(INTDIR)\stdafx.pch"


!ELSEIF  "$(CFG)" == "DrawCli - Power Macintosh Debug"


"$(INTDIR)\drawcli.obj" : $(SOURCE) $(DEP_CPP_DRAWC) "$(INTDIR)"\
 "$(INTDIR)\stdafx.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_DRAWC) "$(INTDIR)"\
 "$(INTDIR)\stdafx.pch"


!ELSEIF  "$(CFG)" == "DrawCli - Win32 Unicode Release"


"$(INTDIR)\drawcli.obj" : $(SOURCE) $(DEP_CPP_DRAWC) "$(INTDIR)"\
 ".\UniRel\stdafx.pch"


!ELSEIF  "$(CFG)" == "DrawCli - Win32 Unicode Debug"


"$(INTDIR)\drawcli.obj" : $(SOURCE) $(DEP_CPP_DRAWC) "$(INTDIR)"\
 "$(INTDIR)\stdafx.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_DRAWC) "$(INTDIR)"\
 "$(INTDIR)\stdafx.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\drawcli.rc
DEP_RSC_DRAWCL=\
	".\res\drawcli.ico"\
	".\res\drawdoc.ico"\
	".\res\toolbar.bmp"\
	".\res\pencil.cur"\
	".\res\drawcli.rc2"\
	

!IF  "$(CFG)" == "DrawCli - Win32 Release"


"$(INTDIR)\drawcli.res" : $(SOURCE) $(DEP_RSC_DRAWCL) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "DrawCli - Win32 Debug"


"$(INTDIR)\drawcli.res" : $(SOURCE) $(DEP_RSC_DRAWCL) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "DrawCli - Macintosh Release"


"$(INTDIR)\drawcli.rsc" : $(SOURCE) $(DEP_RSC_DRAWCL) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "DrawCli - Macintosh Debug"


"$(INTDIR)\drawcli.rsc" : $(SOURCE) $(DEP_RSC_DRAWCL) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "DrawCli - Power Macintosh Release"


"$(INTDIR)\drawcli.rsc" : $(SOURCE) $(DEP_RSC_DRAWCL) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "DrawCli - Power Macintosh Debug"


"$(INTDIR)\drawcli.rsc" : $(SOURCE) $(DEP_RSC_DRAWCL) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "DrawCli - Win32 Unicode Release"


"$(INTDIR)\drawcli.res" : $(SOURCE) $(DEP_RSC_DRAWCL) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "DrawCli - Win32 Unicode Debug"


"$(INTDIR)\drawcli.res" : $(SOURCE) $(DEP_RSC_DRAWCL) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\drawdoc.cpp
DEP_CPP_DRAWD=\
	".\cntritem.h"\
	".\drawcli.h"\
	".\drawdoc.h"\
	".\drawobj.h"\
	".\summinfo.h"\
	".\propset.h"\
	".\drawvw.h"\
	".\statpage.h"\
	".\stdafx.h"\
	".\summpage.h"\
	

!IF  "$(CFG)" == "DrawCli - Win32 Release"


"$(INTDIR)\drawdoc.obj" : $(SOURCE) $(DEP_CPP_DRAWD) "$(INTDIR)"\
 "$(INTDIR)\stdafx.pch"


!ELSEIF  "$(CFG)" == "DrawCli - Win32 Debug"


"$(INTDIR)\drawdoc.obj" : $(SOURCE) $(DEP_CPP_DRAWD) "$(INTDIR)"\
 "$(INTDIR)\stdafx.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_DRAWD) "$(INTDIR)"\
 "$(INTDIR)\stdafx.pch"


!ELSEIF  "$(CFG)" == "DrawCli - Macintosh Release"


"$(INTDIR)\drawdoc.obj" : $(SOURCE) $(DEP_CPP_DRAWD) "$(INTDIR)"\
 "$(INTDIR)\stdafx.pch"


!ELSEIF  "$(CFG)" == "DrawCli - Macintosh Debug"


"$(INTDIR)\drawdoc.obj" : $(SOURCE) $(DEP_CPP_DRAWD) "$(INTDIR)"\
 "$(INTDIR)\drawcli.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_DRAWD) "$(INTDIR)"\
 "$(INTDIR)\drawcli.pch"


!ELSEIF  "$(CFG)" == "DrawCli - Power Macintosh Release"


"$(INTDIR)\drawdoc.obj" : $(SOURCE) $(DEP_CPP_DRAWD) "$(INTDIR)"\
 "$(INTDIR)\stdafx.pch"


!ELSEIF  "$(CFG)" == "DrawCli - Power Macintosh Debug"


"$(INTDIR)\drawdoc.obj" : $(SOURCE) $(DEP_CPP_DRAWD) "$(INTDIR)"\
 "$(INTDIR)\stdafx.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_DRAWD) "$(INTDIR)"\
 "$(INTDIR)\stdafx.pch"


!ELSEIF  "$(CFG)" == "DrawCli - Win32 Unicode Release"


"$(INTDIR)\drawdoc.obj" : $(SOURCE) $(DEP_CPP_DRAWD) "$(INTDIR)"\
 ".\UniRel\stdafx.pch"


!ELSEIF  "$(CFG)" == "DrawCli - Win32 Unicode Debug"


"$(INTDIR)\drawdoc.obj" : $(SOURCE) $(DEP_CPP_DRAWD) "$(INTDIR)"\
 "$(INTDIR)\stdafx.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_DRAWD) "$(INTDIR)"\
 "$(INTDIR)\stdafx.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\drawobj.cpp
DEP_CPP_DRAWO=\
	".\cntritem.h"\
	".\drawcli.h"\
	".\drawdoc.h"\
	".\drawobj.h"\
	".\summinfo.h"\
	".\propset.h"\
	".\drawvw.h"\
	".\rectdlg.h"\
	".\stdafx.h"\
	

!IF  "$(CFG)" == "DrawCli - Win32 Release"


"$(INTDIR)\drawobj.obj" : $(SOURCE) $(DEP_CPP_DRAWO) "$(INTDIR)"\
 "$(INTDIR)\stdafx.pch"


!ELSEIF  "$(CFG)" == "DrawCli - Win32 Debug"


"$(INTDIR)\drawobj.obj" : $(SOURCE) $(DEP_CPP_DRAWO) "$(INTDIR)"\
 "$(INTDIR)\stdafx.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_DRAWO) "$(INTDIR)"\
 "$(INTDIR)\stdafx.pch"


!ELSEIF  "$(CFG)" == "DrawCli - Macintosh Release"


"$(INTDIR)\drawobj.obj" : $(SOURCE) $(DEP_CPP_DRAWO) "$(INTDIR)"\
 "$(INTDIR)\stdafx.pch"


!ELSEIF  "$(CFG)" == "DrawCli - Macintosh Debug"


"$(INTDIR)\drawobj.obj" : $(SOURCE) $(DEP_CPP_DRAWO) "$(INTDIR)"\
 "$(INTDIR)\drawcli.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_DRAWO) "$(INTDIR)"\
 "$(INTDIR)\drawcli.pch"


!ELSEIF  "$(CFG)" == "DrawCli - Power Macintosh Release"


"$(INTDIR)\drawobj.obj" : $(SOURCE) $(DEP_CPP_DRAWO) "$(INTDIR)"\
 "$(INTDIR)\stdafx.pch"


!ELSEIF  "$(CFG)" == "DrawCli - Power Macintosh Debug"


"$(INTDIR)\drawobj.obj" : $(SOURCE) $(DEP_CPP_DRAWO) "$(INTDIR)"\
 "$(INTDIR)\stdafx.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_DRAWO) "$(INTDIR)"\
 "$(INTDIR)\stdafx.pch"


!ELSEIF  "$(CFG)" == "DrawCli - Win32 Unicode Release"


"$(INTDIR)\drawobj.obj" : $(SOURCE) $(DEP_CPP_DRAWO) "$(INTDIR)"\
 ".\UniRel\stdafx.pch"


!ELSEIF  "$(CFG)" == "DrawCli - Win32 Unicode Debug"


"$(INTDIR)\drawobj.obj" : $(SOURCE) $(DEP_CPP_DRAWO) "$(INTDIR)"\
 "$(INTDIR)\stdafx.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_DRAWO) "$(INTDIR)"\
 "$(INTDIR)\stdafx.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\drawtool.cpp
DEP_CPP_DRAWT=\
	".\drawcli.h"\
	".\drawdoc.h"\
	".\drawobj.h"\
	".\summinfo.h"\
	".\propset.h"\
	".\drawtool.h"\
	".\drawvw.h"\
	".\stdafx.h"\
	

!IF  "$(CFG)" == "DrawCli - Win32 Release"


"$(INTDIR)\drawtool.obj" : $(SOURCE) $(DEP_CPP_DRAWT) "$(INTDIR)"\
 "$(INTDIR)\stdafx.pch"


!ELSEIF  "$(CFG)" == "DrawCli - Win32 Debug"


"$(INTDIR)\drawtool.obj" : $(SOURCE) $(DEP_CPP_DRAWT) "$(INTDIR)"\
 "$(INTDIR)\stdafx.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_DRAWT) "$(INTDIR)"\
 "$(INTDIR)\stdafx.pch"


!ELSEIF  "$(CFG)" == "DrawCli - Macintosh Release"


"$(INTDIR)\drawtool.obj" : $(SOURCE) $(DEP_CPP_DRAWT) "$(INTDIR)"\
 "$(INTDIR)\stdafx.pch"


!ELSEIF  "$(CFG)" == "DrawCli - Macintosh Debug"


"$(INTDIR)\drawtool.obj" : $(SOURCE) $(DEP_CPP_DRAWT) "$(INTDIR)"\
 "$(INTDIR)\drawcli.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_DRAWT) "$(INTDIR)"\
 "$(INTDIR)\drawcli.pch"


!ELSEIF  "$(CFG)" == "DrawCli - Power Macintosh Release"


"$(INTDIR)\drawtool.obj" : $(SOURCE) $(DEP_CPP_DRAWT) "$(INTDIR)"\
 "$(INTDIR)\stdafx.pch"


!ELSEIF  "$(CFG)" == "DrawCli - Power Macintosh Debug"


"$(INTDIR)\drawtool.obj" : $(SOURCE) $(DEP_CPP_DRAWT) "$(INTDIR)"\
 "$(INTDIR)\stdafx.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_DRAWT) "$(INTDIR)"\
 "$(INTDIR)\stdafx.pch"


!ELSEIF  "$(CFG)" == "DrawCli - Win32 Unicode Release"


"$(INTDIR)\drawtool.obj" : $(SOURCE) $(DEP_CPP_DRAWT) "$(INTDIR)"\
 ".\UniRel\stdafx.pch"


!ELSEIF  "$(CFG)" == "DrawCli - Win32 Unicode Debug"


"$(INTDIR)\drawtool.obj" : $(SOURCE) $(DEP_CPP_DRAWT) "$(INTDIR)"\
 "$(INTDIR)\stdafx.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_DRAWT) "$(INTDIR)"\
 "$(INTDIR)\stdafx.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\drawvw.cpp
DEP_CPP_DRAWV=\
	".\cntritem.h"\
	".\drawcli.h"\
	".\drawdoc.h"\
	".\drawobj.h"\
	".\summinfo.h"\
	".\propset.h"\
	".\drawtool.h"\
	".\drawvw.h"\
	".\mainfrm.h"\
	".\stdafx.h"\
	

!IF  "$(CFG)" == "DrawCli - Win32 Release"


"$(INTDIR)\drawvw.obj" : $(SOURCE) $(DEP_CPP_DRAWV) "$(INTDIR)"\
 "$(INTDIR)\stdafx.pch"


!ELSEIF  "$(CFG)" == "DrawCli - Win32 Debug"


"$(INTDIR)\drawvw.obj" : $(SOURCE) $(DEP_CPP_DRAWV) "$(INTDIR)"\
 "$(INTDIR)\stdafx.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_DRAWV) "$(INTDIR)"\
 "$(INTDIR)\stdafx.pch"


!ELSEIF  "$(CFG)" == "DrawCli - Macintosh Release"


"$(INTDIR)\drawvw.obj" : $(SOURCE) $(DEP_CPP_DRAWV) "$(INTDIR)"\
 "$(INTDIR)\stdafx.pch"


!ELSEIF  "$(CFG)" == "DrawCli - Macintosh Debug"


"$(INTDIR)\drawvw.obj" : $(SOURCE) $(DEP_CPP_DRAWV) "$(INTDIR)"\
 "$(INTDIR)\drawcli.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_DRAWV) "$(INTDIR)"\
 "$(INTDIR)\drawcli.pch"


!ELSEIF  "$(CFG)" == "DrawCli - Power Macintosh Release"


"$(INTDIR)\drawvw.obj" : $(SOURCE) $(DEP_CPP_DRAWV) "$(INTDIR)"\
 "$(INTDIR)\stdafx.pch"


!ELSEIF  "$(CFG)" == "DrawCli - Power Macintosh Debug"


"$(INTDIR)\drawvw.obj" : $(SOURCE) $(DEP_CPP_DRAWV) "$(INTDIR)"\
 "$(INTDIR)\stdafx.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_DRAWV) "$(INTDIR)"\
 "$(INTDIR)\stdafx.pch"


!ELSEIF  "$(CFG)" == "DrawCli - Win32 Unicode Release"


"$(INTDIR)\drawvw.obj" : $(SOURCE) $(DEP_CPP_DRAWV) "$(INTDIR)"\
 ".\UniRel\stdafx.pch"


!ELSEIF  "$(CFG)" == "DrawCli - Win32 Unicode Debug"


"$(INTDIR)\drawvw.obj" : $(SOURCE) $(DEP_CPP_DRAWV) "$(INTDIR)"\
 "$(INTDIR)\stdafx.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_DRAWV) "$(INTDIR)"\
 "$(INTDIR)\stdafx.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\mainfrm.cpp
DEP_CPP_MAINF=\
	".\drawcli.h"\
	".\mainfrm.h"\
	".\stdafx.h"\
	

!IF  "$(CFG)" == "DrawCli - Win32 Release"


"$(INTDIR)\mainfrm.obj" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\stdafx.pch"


!ELSEIF  "$(CFG)" == "DrawCli - Win32 Debug"


"$(INTDIR)\mainfrm.obj" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\stdafx.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\stdafx.pch"


!ELSEIF  "$(CFG)" == "DrawCli - Macintosh Release"


"$(INTDIR)\mainfrm.obj" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\stdafx.pch"


!ELSEIF  "$(CFG)" == "DrawCli - Macintosh Debug"


"$(INTDIR)\mainfrm.obj" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\drawcli.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\drawcli.pch"


!ELSEIF  "$(CFG)" == "DrawCli - Power Macintosh Release"


"$(INTDIR)\mainfrm.obj" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\stdafx.pch"


!ELSEIF  "$(CFG)" == "DrawCli - Power Macintosh Debug"


"$(INTDIR)\mainfrm.obj" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\stdafx.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\stdafx.pch"


!ELSEIF  "$(CFG)" == "DrawCli - Win32 Unicode Release"


"$(INTDIR)\mainfrm.obj" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 ".\UniRel\stdafx.pch"


!ELSEIF  "$(CFG)" == "DrawCli - Win32 Unicode Debug"


"$(INTDIR)\mainfrm.obj" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\stdafx.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\stdafx.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\rectdlg.cpp
DEP_CPP_RECTD=\
	".\drawcli.h"\
	".\rectdlg.h"\
	".\stdafx.h"\
	

!IF  "$(CFG)" == "DrawCli - Win32 Release"


"$(INTDIR)\rectdlg.obj" : $(SOURCE) $(DEP_CPP_RECTD) "$(INTDIR)"\
 "$(INTDIR)\stdafx.pch"


!ELSEIF  "$(CFG)" == "DrawCli - Win32 Debug"


"$(INTDIR)\rectdlg.obj" : $(SOURCE) $(DEP_CPP_RECTD) "$(INTDIR)"\
 "$(INTDIR)\stdafx.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_RECTD) "$(INTDIR)"\
 "$(INTDIR)\stdafx.pch"


!ELSEIF  "$(CFG)" == "DrawCli - Macintosh Release"


"$(INTDIR)\rectdlg.obj" : $(SOURCE) $(DEP_CPP_RECTD) "$(INTDIR)"\
 "$(INTDIR)\stdafx.pch"


!ELSEIF  "$(CFG)" == "DrawCli - Macintosh Debug"


"$(INTDIR)\rectdlg.obj" : $(SOURCE) $(DEP_CPP_RECTD) "$(INTDIR)"\
 "$(INTDIR)\drawcli.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_RECTD) "$(INTDIR)"\
 "$(INTDIR)\drawcli.pch"


!ELSEIF  "$(CFG)" == "DrawCli - Power Macintosh Release"


"$(INTDIR)\rectdlg.obj" : $(SOURCE) $(DEP_CPP_RECTD) "$(INTDIR)"\
 "$(INTDIR)\stdafx.pch"


!ELSEIF  "$(CFG)" == "DrawCli - Power Macintosh Debug"


"$(INTDIR)\rectdlg.obj" : $(SOURCE) $(DEP_CPP_RECTD) "$(INTDIR)"\
 "$(INTDIR)\stdafx.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_RECTD) "$(INTDIR)"\
 "$(INTDIR)\stdafx.pch"


!ELSEIF  "$(CFG)" == "DrawCli - Win32 Unicode Release"


"$(INTDIR)\rectdlg.obj" : $(SOURCE) $(DEP_CPP_RECTD) "$(INTDIR)"\
 ".\UniRel\stdafx.pch"


!ELSEIF  "$(CFG)" == "DrawCli - Win32 Unicode Debug"


"$(INTDIR)\rectdlg.obj" : $(SOURCE) $(DEP_CPP_RECTD) "$(INTDIR)"\
 "$(INTDIR)\stdafx.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_RECTD) "$(INTDIR)"\
 "$(INTDIR)\stdafx.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\splitfrm.cpp
DEP_CPP_SPLIT=\
	".\drawcli.h"\
	".\splitfrm.h"\
	".\stdafx.h"\
	

!IF  "$(CFG)" == "DrawCli - Win32 Release"


"$(INTDIR)\splitfrm.obj" : $(SOURCE) $(DEP_CPP_SPLIT) "$(INTDIR)"\
 "$(INTDIR)\stdafx.pch"


!ELSEIF  "$(CFG)" == "DrawCli - Win32 Debug"


"$(INTDIR)\splitfrm.obj" : $(SOURCE) $(DEP_CPP_SPLIT) "$(INTDIR)"\
 "$(INTDIR)\stdafx.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_SPLIT) "$(INTDIR)"\
 "$(INTDIR)\stdafx.pch"


!ELSEIF  "$(CFG)" == "DrawCli - Macintosh Release"


"$(INTDIR)\splitfrm.obj" : $(SOURCE) $(DEP_CPP_SPLIT) "$(INTDIR)"\
 "$(INTDIR)\stdafx.pch"


!ELSEIF  "$(CFG)" == "DrawCli - Macintosh Debug"


"$(INTDIR)\splitfrm.obj" : $(SOURCE) $(DEP_CPP_SPLIT) "$(INTDIR)"\
 "$(INTDIR)\drawcli.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_SPLIT) "$(INTDIR)"\
 "$(INTDIR)\drawcli.pch"


!ELSEIF  "$(CFG)" == "DrawCli - Power Macintosh Release"


"$(INTDIR)\splitfrm.obj" : $(SOURCE) $(DEP_CPP_SPLIT) "$(INTDIR)"\
 "$(INTDIR)\stdafx.pch"


!ELSEIF  "$(CFG)" == "DrawCli - Power Macintosh Debug"


"$(INTDIR)\splitfrm.obj" : $(SOURCE) $(DEP_CPP_SPLIT) "$(INTDIR)"\
 "$(INTDIR)\stdafx.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_SPLIT) "$(INTDIR)"\
 "$(INTDIR)\stdafx.pch"


!ELSEIF  "$(CFG)" == "DrawCli - Win32 Unicode Release"


"$(INTDIR)\splitfrm.obj" : $(SOURCE) $(DEP_CPP_SPLIT) "$(INTDIR)"\
 ".\UniRel\stdafx.pch"


!ELSEIF  "$(CFG)" == "DrawCli - Win32 Unicode Debug"


"$(INTDIR)\splitfrm.obj" : $(SOURCE) $(DEP_CPP_SPLIT) "$(INTDIR)"\
 "$(INTDIR)\stdafx.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_SPLIT) "$(INTDIR)"\
 "$(INTDIR)\stdafx.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\stdafx.cpp
DEP_CPP_STDAF=\
	".\stdafx.h"\
	

!IF  "$(CFG)" == "DrawCli - Win32 Release"

# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/stdafx.pch" /Yc"stdafx.h" /Fo"$(INTDIR)/" /c\
 $(SOURCE) \
	

"$(INTDIR)\stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\stdafx.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "DrawCli - Win32 Debug"

# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/stdafx.pch" /Yc"stdafx.h" /Fo"$(INTDIR)/"\
 /Fd"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\stdafx.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "DrawCli - Macintosh Release"

# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /AL /Q68s /W3 /GX /O2 /D "_WINDOWS" /D "_MAC" /D "_68K_" /D\
 "WIN32" /D "NDEBUG" /D "_MBCS" /Fp"$(INTDIR)/stdafx.pch" /Yc"stdafx.h"\
 /Fo"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\stdafx.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "DrawCli - Macintosh Debug"

# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /AL /Q68s /Q68m /W3 /GX /Zi /Od /D "_WINDOWS" /D "_MAC" /D\
 "_68K_" /D "WIN32" /D "_DEBUG" /D "_MBCS" /Fp"$(INTDIR)/drawcli.pch"\
 /Yc"stdafx.h" /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\drawcli.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "DrawCli - Power Macintosh Release"

# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "_MAC" /D "_MPPC_" /D "_WINDOWS"\
 /D "WIN32" /D "_MBCS" /D "_AFXDLL" /Fp"$(INTDIR)/stdafx.pch" /Yc"stdafx.h"\
 /Fo"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\stdafx.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "DrawCli - Power Macintosh Debug"

# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MDd /W3 /GX /Zi /Od /D "_DEBUG" /D "_MAC" /D "_MPPC_" /D\
 "_WINDOWS" /D "WIN32" /D "_MBCS" /D "_AFXDLL" /Fp"$(INTDIR)/stdafx.pch"\
 /Yc"stdafx.h" /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\stdafx.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "DrawCli - Win32 Unicode Release"

# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /D "_UNICODE" /Fp"UniRel/stdafx.pch" /Yc"stdafx.h"\
 /Fo"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

".\UniRel\stdafx.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "DrawCli - Win32 Unicode Debug"

# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MDd /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS"\
 /D "_AFXDLL" /D "_MBCS" /D "_UNICODE" /Fp"$(INTDIR)/stdafx.pch" /Yc"stdafx.h"\
 /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\stdafx.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\dclimac.r

!IF  "$(CFG)" == "DrawCli - Win32 Release"

!ELSEIF  "$(CFG)" == "DrawCli - Win32 Debug"

!ELSEIF  "$(CFG)" == "DrawCli - Macintosh Release"

DEP_MRC_DCLIM=\
	{$(INCLUDE)}"\mrc\TYPES.R"\
	{$(INCLUDE)}"\mrc\BALLOONS.R"\
	

"$(INTDIR)\dclimac.rsc" : $(SOURCE) $(DEP_MRC_DCLIM) "$(INTDIR)"
   $(MRC) /o"$(INTDIR)/dclimac.rsc" /D "_68K_" /D "_MAC" /D "NDEBUG" /l 0x409\
 /NOLOGO $(SOURCE)


!ELSEIF  "$(CFG)" == "DrawCli - Macintosh Debug"

DEP_MRC_DCLIM=\
	{$(INCLUDE)}"\mrc\TYPES.R"\
	{$(INCLUDE)}"\mrc\BALLOONS.R"\
	

"$(INTDIR)\dclimac.rsc" : $(SOURCE) $(DEP_MRC_DCLIM) "$(INTDIR)"
   $(MRC) /o"$(INTDIR)/dclimac.rsc" /D "_68K_" /D "_MAC" /D "_DEBUG" /l 0x409\
 /NOLOGO $(SOURCE)


!ELSEIF  "$(CFG)" == "DrawCli - Power Macintosh Release"

DEP_MRC_DCLIM=\
	{$(INCLUDE)}"\mrc\TYPES.R"\
	{$(INCLUDE)}"\mrc\BALLOONS.R"\
	

"$(INTDIR)\dclimac.rsc" : $(SOURCE) $(DEP_MRC_DCLIM) "$(INTDIR)"
   $(MRC) /o"$(INTDIR)/dclimac.rsc" /D "_MPPC_" /D "_MAC" /D "NDEBUG" /l 0x409\
 /NOLOGO $(SOURCE)


!ELSEIF  "$(CFG)" == "DrawCli - Power Macintosh Debug"

DEP_MRC_DCLIM=\
	{$(INCLUDE)}"\mrc\TYPES.R"\
	{$(INCLUDE)}"\mrc\BALLOONS.R"\
	

"$(INTDIR)\dclimac.rsc" : $(SOURCE) $(DEP_MRC_DCLIM) "$(INTDIR)"
   $(MRC) /o"$(INTDIR)/dclimac.rsc" /D "_MPPC_" /D "_MAC" /D "_DEBUG" /l 0x409\
 /NOLOGO $(SOURCE)


!ELSEIF  "$(CFG)" == "DrawCli - Win32 Unicode Release"

!ELSEIF  "$(CFG)" == "DrawCli - Win32 Unicode Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\statpage.cpp
DEP_CPP_STATP=\
	".\drawcli.h"\
	".\statpage.h"\
	".\stdafx.h"\
	

!IF  "$(CFG)" == "DrawCli - Win32 Release"


"$(INTDIR)\statpage.obj" : $(SOURCE) $(DEP_CPP_STATP) "$(INTDIR)"\
 "$(INTDIR)\stdafx.pch"


!ELSEIF  "$(CFG)" == "DrawCli - Win32 Debug"


"$(INTDIR)\statpage.obj" : $(SOURCE) $(DEP_CPP_STATP) "$(INTDIR)"\
 "$(INTDIR)\stdafx.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_STATP) "$(INTDIR)"\
 "$(INTDIR)\stdafx.pch"


!ELSEIF  "$(CFG)" == "DrawCli - Macintosh Release"


"$(INTDIR)\statpage.obj" : $(SOURCE) $(DEP_CPP_STATP) "$(INTDIR)"\
 "$(INTDIR)\stdafx.pch"


!ELSEIF  "$(CFG)" == "DrawCli - Macintosh Debug"


"$(INTDIR)\statpage.obj" : $(SOURCE) $(DEP_CPP_STATP) "$(INTDIR)"\
 "$(INTDIR)\drawcli.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_STATP) "$(INTDIR)"\
 "$(INTDIR)\drawcli.pch"


!ELSEIF  "$(CFG)" == "DrawCli - Power Macintosh Release"


"$(INTDIR)\statpage.obj" : $(SOURCE) $(DEP_CPP_STATP) "$(INTDIR)"\
 "$(INTDIR)\stdafx.pch"


!ELSEIF  "$(CFG)" == "DrawCli - Power Macintosh Debug"


"$(INTDIR)\statpage.obj" : $(SOURCE) $(DEP_CPP_STATP) "$(INTDIR)"\
 "$(INTDIR)\stdafx.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_STATP) "$(INTDIR)"\
 "$(INTDIR)\stdafx.pch"


!ELSEIF  "$(CFG)" == "DrawCli - Win32 Unicode Release"


"$(INTDIR)\statpage.obj" : $(SOURCE) $(DEP_CPP_STATP) "$(INTDIR)"\
 ".\UniRel\stdafx.pch"


!ELSEIF  "$(CFG)" == "DrawCli - Win32 Unicode Debug"


"$(INTDIR)\statpage.obj" : $(SOURCE) $(DEP_CPP_STATP) "$(INTDIR)"\
 "$(INTDIR)\stdafx.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_STATP) "$(INTDIR)"\
 "$(INTDIR)\stdafx.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\summinfo.cpp
DEP_CPP_SUMMI=\
	".\drawcli.h"\
	".\stdafx.h"\
	".\summinfo.h"\
	".\propset.h"\
	

!IF  "$(CFG)" == "DrawCli - Win32 Release"


"$(INTDIR)\summinfo.obj" : $(SOURCE) $(DEP_CPP_SUMMI) "$(INTDIR)"\
 "$(INTDIR)\stdafx.pch"


!ELSEIF  "$(CFG)" == "DrawCli - Win32 Debug"


"$(INTDIR)\summinfo.obj" : $(SOURCE) $(DEP_CPP_SUMMI) "$(INTDIR)"\
 "$(INTDIR)\stdafx.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_SUMMI) "$(INTDIR)"\
 "$(INTDIR)\stdafx.pch"


!ELSEIF  "$(CFG)" == "DrawCli - Macintosh Release"


"$(INTDIR)\summinfo.obj" : $(SOURCE) $(DEP_CPP_SUMMI) "$(INTDIR)"\
 "$(INTDIR)\stdafx.pch"


!ELSEIF  "$(CFG)" == "DrawCli - Macintosh Debug"


"$(INTDIR)\summinfo.obj" : $(SOURCE) $(DEP_CPP_SUMMI) "$(INTDIR)"\
 "$(INTDIR)\drawcli.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_SUMMI) "$(INTDIR)"\
 "$(INTDIR)\drawcli.pch"


!ELSEIF  "$(CFG)" == "DrawCli - Power Macintosh Release"


"$(INTDIR)\summinfo.obj" : $(SOURCE) $(DEP_CPP_SUMMI) "$(INTDIR)"\
 "$(INTDIR)\stdafx.pch"


!ELSEIF  "$(CFG)" == "DrawCli - Power Macintosh Debug"


"$(INTDIR)\summinfo.obj" : $(SOURCE) $(DEP_CPP_SUMMI) "$(INTDIR)"\
 "$(INTDIR)\stdafx.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_SUMMI) "$(INTDIR)"\
 "$(INTDIR)\stdafx.pch"


!ELSEIF  "$(CFG)" == "DrawCli - Win32 Unicode Release"


"$(INTDIR)\summinfo.obj" : $(SOURCE) $(DEP_CPP_SUMMI) "$(INTDIR)"\
 ".\UniRel\stdafx.pch"


!ELSEIF  "$(CFG)" == "DrawCli - Win32 Unicode Debug"


"$(INTDIR)\summinfo.obj" : $(SOURCE) $(DEP_CPP_SUMMI) "$(INTDIR)"\
 "$(INTDIR)\stdafx.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_SUMMI) "$(INTDIR)"\
 "$(INTDIR)\stdafx.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\summpage.cpp

!IF  "$(CFG)" == "DrawCli - Win32 Release"

DEP_CPP_SUMMP=\
	".\drawcli.h"\
	".\stdafx.h"\
	".\summpage.h"\
	

"$(INTDIR)\summpage.obj" : $(SOURCE) $(DEP_CPP_SUMMP) "$(INTDIR)"\
 "$(INTDIR)\stdafx.pch"


!ELSEIF  "$(CFG)" == "DrawCli - Win32 Debug"

DEP_CPP_SUMMP=\
	".\drawcli.h"\
	".\stdafx.h"\
	".\summpage.h"\
	

"$(INTDIR)\summpage.obj" : $(SOURCE) $(DEP_CPP_SUMMP) "$(INTDIR)"\
 "$(INTDIR)\stdafx.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_SUMMP) "$(INTDIR)"\
 "$(INTDIR)\stdafx.pch"


!ELSEIF  "$(CFG)" == "DrawCli - Macintosh Release"

DEP_CPP_SUMMP=\
	".\drawcli.h"\
	".\stdafx.h"\
	".\summpage.h"\
	

"$(INTDIR)\summpage.obj" : $(SOURCE) $(DEP_CPP_SUMMP) "$(INTDIR)"\
 "$(INTDIR)\stdafx.pch"


!ELSEIF  "$(CFG)" == "DrawCli - Macintosh Debug"

DEP_CPP_SUMMP=\
	".\drawcli.h"\
	".\stdafx.h"\
	".\summpage.h"\
	

"$(INTDIR)\summpage.obj" : $(SOURCE) $(DEP_CPP_SUMMP) "$(INTDIR)"\
 "$(INTDIR)\drawcli.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_SUMMP) "$(INTDIR)"\
 "$(INTDIR)\drawcli.pch"


!ELSEIF  "$(CFG)" == "DrawCli - Power Macintosh Release"

DEP_CPP_SUMMP=\
	".\drawcli.h"\
	".\stdafx.h"\
	".\summpage.h"\
	

"$(INTDIR)\summpage.obj" : $(SOURCE) $(DEP_CPP_SUMMP) "$(INTDIR)"\
 "$(INTDIR)\stdafx.pch"


!ELSEIF  "$(CFG)" == "DrawCli - Power Macintosh Debug"

DEP_CPP_SUMMP=\
	".\drawcli.h"\
	".\stdafx.h"\
	".\summpage.h"\
	

"$(INTDIR)\summpage.obj" : $(SOURCE) $(DEP_CPP_SUMMP) "$(INTDIR)"\
 "$(INTDIR)\stdafx.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_SUMMP) "$(INTDIR)"\
 "$(INTDIR)\stdafx.pch"


!ELSEIF  "$(CFG)" == "DrawCli - Win32 Unicode Release"

DEP_CPP_SUMMP=\
	".\stdafx.h"\
	".\drawcli.h"\
	".\summpage.h"\
	

"$(INTDIR)\summpage.obj" : $(SOURCE) $(DEP_CPP_SUMMP) "$(INTDIR)"\
 ".\UniRel\stdafx.pch"


!ELSEIF  "$(CFG)" == "DrawCli - Win32 Unicode Debug"

DEP_CPP_SUMMP=\
	".\stdafx.h"\
	".\drawcli.h"\
	".\summpage.h"\
	

"$(INTDIR)\summpage.obj" : $(SOURCE) $(DEP_CPP_SUMMP) "$(INTDIR)"\
 "$(INTDIR)\stdafx.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_SUMMP) "$(INTDIR)"\
 "$(INTDIR)\stdafx.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\propset.cpp
DEP_CPP_PROPS=\
	".\propset.h"\
	".\stdafx.h"\
	

!IF  "$(CFG)" == "DrawCli - Win32 Release"


"$(INTDIR)\propset.obj" : $(SOURCE) $(DEP_CPP_PROPS) "$(INTDIR)"\
 "$(INTDIR)\stdafx.pch"


!ELSEIF  "$(CFG)" == "DrawCli - Win32 Debug"


"$(INTDIR)\propset.obj" : $(SOURCE) $(DEP_CPP_PROPS) "$(INTDIR)"\
 "$(INTDIR)\stdafx.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_PROPS) "$(INTDIR)"\
 "$(INTDIR)\stdafx.pch"


!ELSEIF  "$(CFG)" == "DrawCli - Macintosh Release"


"$(INTDIR)\propset.obj" : $(SOURCE) $(DEP_CPP_PROPS) "$(INTDIR)"\
 "$(INTDIR)\stdafx.pch"


!ELSEIF  "$(CFG)" == "DrawCli - Macintosh Debug"


"$(INTDIR)\propset.obj" : $(SOURCE) $(DEP_CPP_PROPS) "$(INTDIR)"\
 "$(INTDIR)\drawcli.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_PROPS) "$(INTDIR)"\
 "$(INTDIR)\drawcli.pch"


!ELSEIF  "$(CFG)" == "DrawCli - Power Macintosh Release"


"$(INTDIR)\propset.obj" : $(SOURCE) $(DEP_CPP_PROPS) "$(INTDIR)"\
 "$(INTDIR)\stdafx.pch"


!ELSEIF  "$(CFG)" == "DrawCli - Power Macintosh Debug"


"$(INTDIR)\propset.obj" : $(SOURCE) $(DEP_CPP_PROPS) "$(INTDIR)"\
 "$(INTDIR)\stdafx.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_PROPS) "$(INTDIR)"\
 "$(INTDIR)\stdafx.pch"


!ELSEIF  "$(CFG)" == "DrawCli - Win32 Unicode Release"


"$(INTDIR)\propset.obj" : $(SOURCE) $(DEP_CPP_PROPS) "$(INTDIR)"\
 ".\UniRel\stdafx.pch"


!ELSEIF  "$(CFG)" == "DrawCli - Win32 Unicode Debug"


"$(INTDIR)\propset.obj" : $(SOURCE) $(DEP_CPP_PROPS) "$(INTDIR)"\
 "$(INTDIR)\stdafx.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_PROPS) "$(INTDIR)"\
 "$(INTDIR)\stdafx.pch"


!ENDIF 

# End Source File
# End Target
# End Project
################################################################################
