# Microsoft Visual C++ Generated NMAKE File, Format Version 40000
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101
# TARGTYPE "Power Macintosh Application" 0x0401
# TARGTYPE "Macintosh Application" 0x0301

!IF "$(CFG)" == ""
CFG=CtrlTest - Win32 Debug
!MESSAGE No configuration specified.  Defaulting to CtrlTest - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "CtrlTest - Win32 Release" && "$(CFG)" !=\
 "CtrlTest - Win32 Debug" && "$(CFG)" != "CtrlTest - Macintosh Release" &&\
 "$(CFG)" != "CtrlTest - Macintosh Debug" && "$(CFG)" !=\
 "CtrlTest - Power Macintosh Release" && "$(CFG)" !=\
 "CtrlTest - Power Macintosh Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "ctrltest.mak" CFG="CtrlTest - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "CtrlTest - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "CtrlTest - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE "CtrlTest - Macintosh Release" (based on "Macintosh Application")
!MESSAGE "CtrlTest - Macintosh Debug" (based on "Macintosh Application")
!MESSAGE "CtrlTest - Power Macintosh Release" (based on\
 "Power Macintosh Application")
!MESSAGE "CtrlTest - Power Macintosh Debug" (based on\
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
# PROP Target_Last_Scanned "CtrlTest - Power Macintosh Debug"

!IF  "$(CFG)" == "CtrlTest - Win32 Release"

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

ALL : "$(OUTDIR)\ctrltest.exe"

CLEAN : 
	-@erase ".\Release\ctrltest.exe"
	-@erase ".\Release\bbutton.obj"
	-@erase ".\Release\ctrltest.pch"
	-@erase ".\Release\custlist.obj"
	-@erase ".\Release\paredit2.obj"
	-@erase ".\Release\spintest.obj"
	-@erase ".\Release\dertest.obj"
	-@erase ".\Release\subtest.obj"
	-@erase ".\Release\wclstest.obj"
	-@erase ".\Release\paredit.obj"
	-@erase ".\Release\ctrltest.obj"
	-@erase ".\Release\stdafx.obj"
	-@erase ".\Release\custmenu.obj"
	-@erase ".\Release\ctrltest.res"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"Stdafx.h" /c
# SUBTRACT CPP /Fr
CPP_PROJ=/nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/ctrltest.pch" /Yu"Stdafx.h" /Fo"$(INTDIR)/"\
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
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/ctrltest.res" /d "NDEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/ctrltest.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 /nologo /subsystem:windows /machine:I386
LINK32_FLAGS=/nologo /subsystem:windows /incremental:no\
 /pdb:"$(OUTDIR)/ctrltest.pdb" /machine:I386 /out:"$(OUTDIR)/ctrltest.exe" 
LINK32_OBJS= \
	"$(INTDIR)/bbutton.obj" \
	"$(INTDIR)/custlist.obj" \
	"$(INTDIR)/paredit2.obj" \
	"$(INTDIR)/spintest.obj" \
	"$(INTDIR)/dertest.obj" \
	"$(INTDIR)/subtest.obj" \
	"$(INTDIR)/wclstest.obj" \
	"$(INTDIR)/paredit.obj" \
	"$(INTDIR)/ctrltest.obj" \
	"$(INTDIR)/stdafx.obj" \
	"$(INTDIR)/custmenu.obj" \
	"$(INTDIR)/ctrltest.res"

"$(OUTDIR)\ctrltest.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "CtrlTest - Win32 Debug"

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

ALL : "$(OUTDIR)\ctrltest.exe"

CLEAN : 
	-@erase ".\Debug\vc40.pdb"
	-@erase ".\Debug\ctrltest.pch"
	-@erase ".\Debug\ctrltest.exe"
	-@erase ".\Debug\paredit.obj"
	-@erase ".\Debug\spintest.obj"
	-@erase ".\Debug\wclstest.obj"
	-@erase ".\Debug\ctrltest.obj"
	-@erase ".\Debug\custmenu.obj"
	-@erase ".\Debug\bbutton.obj"
	-@erase ".\Debug\stdafx.obj"
	-@erase ".\Debug\dertest.obj"
	-@erase ".\Debug\subtest.obj"
	-@erase ".\Debug\paredit2.obj"
	-@erase ".\Debug\custlist.obj"
	-@erase ".\Debug\ctrltest.res"
	-@erase ".\Debug\ctrltest.ilk"
	-@erase ".\Debug\ctrltest.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"Stdafx.h" /c
# SUBTRACT CPP /Fr
CPP_PROJ=/nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/ctrltest.pch" /Yu"Stdafx.h"\
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
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/ctrltest.res" /d "_DEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/ctrltest.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386
# ADD LINK32 /nologo /subsystem:windows /debug /machine:I386
LINK32_FLAGS=/nologo /subsystem:windows /incremental:yes\
 /pdb:"$(OUTDIR)/ctrltest.pdb" /debug /machine:I386\
 /out:"$(OUTDIR)/ctrltest.exe" 
LINK32_OBJS= \
	"$(INTDIR)/paredit.obj" \
	"$(INTDIR)/spintest.obj" \
	"$(INTDIR)/wclstest.obj" \
	"$(INTDIR)/ctrltest.obj" \
	"$(INTDIR)/custmenu.obj" \
	"$(INTDIR)/bbutton.obj" \
	"$(INTDIR)/stdafx.obj" \
	"$(INTDIR)/dertest.obj" \
	"$(INTDIR)/subtest.obj" \
	"$(INTDIR)/paredit2.obj" \
	"$(INTDIR)/custlist.obj" \
	"$(INTDIR)/ctrltest.res"

"$(OUTDIR)\ctrltest.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "CtrlTest - Macintosh Release"

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

ALL : "$(OUTDIR)\ctrltest.exe" "$(OUTDIR)\ctrltest.trg"

CLEAN : 
	-@erase ".\MacRel\ctrltest.trg"
	-@erase ".\MacRel\ctrltest.exe"
	-@erase ".\MacRel\wclstest.obj"
	-@erase ".\MacRel\ctrltest.pch"
	-@erase ".\MacRel\ctrltest.obj"
	-@erase ".\MacRel\stdafx.obj"
	-@erase ".\MacRel\custmenu.obj"
	-@erase ".\MacRel\bbutton.obj"
	-@erase ".\MacRel\paredit2.obj"
	-@erase ".\MacRel\custlist.obj"
	-@erase ".\MacRel\dertest.obj"
	-@erase ".\MacRel\subtest.obj"
	-@erase ".\MacRel\spintest.obj"
	-@erase ".\MacRel\paredit.obj"
	-@erase ".\MacRel\ctrlmac.rsc"
	-@erase ".\MacRel\ctrltest.rsc"

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
 "WIN32" /D "NDEBUG" /D "_MBCS" /Fp"$(INTDIR)/ctrltest.pch" /Yu"Stdafx.h"\
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
RSC_PROJ=/l 0x409 /r /m /fo"$(INTDIR)/ctrltest.rsc" /d "_MAC" /d "_68K_" /d\
 "NDEBUG" 
MRC=mrc.exe
# ADD BASE MRC /D "_68K_" /D "_MAC" /D "NDEBUG" /NOLOGO
# ADD MRC /D "_68K_" /D "_MAC" /D "NDEBUG" /NOLOGO
MRC_PROJ=/D "_68K_" /D "_MAC" /D "NDEBUG" /l 0x409 /NOLOGO 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/ctrltest.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 wlm.lib /nologo /MAC:bundle /machine:M68K
# ADD LINK32 /nologo /MAC:bundle /MAC:creator="CTRL" /machine:M68K
LINK32_FLAGS=/nologo /MAC:bundle /MAC:type="APPL" /MAC:creator="CTRL"\
 /pdb:"$(OUTDIR)/ctrltest.pdb" /machine:M68K /out:"$(OUTDIR)/ctrltest.exe" 
LINK32_OBJS= \
	"$(INTDIR)/wclstest.obj" \
	"$(INTDIR)/ctrltest.obj" \
	"$(INTDIR)/stdafx.obj" \
	"$(INTDIR)/custmenu.obj" \
	"$(INTDIR)/bbutton.obj" \
	"$(INTDIR)/paredit2.obj" \
	"$(INTDIR)/custlist.obj" \
	"$(INTDIR)/dertest.obj" \
	"$(INTDIR)/subtest.obj" \
	"$(INTDIR)/spintest.obj" \
	"$(INTDIR)/paredit.obj" \
	"$(INTDIR)/ctrlmac.rsc" \
	"$(INTDIR)/ctrltest.rsc"

"$(OUTDIR)\ctrltest.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

MFILE32=mfile.exe
# ADD BASE MFILE32 COPY /NOLOGO
# ADD MFILE32 COPY /NOLOGO
MFILE32_FLAGS=COPY /NOLOGO 
MFILE32_FILES= \
	"$(OUTDIR)/ctrltest.exe"

"$(OUTDIR)\ctrltest.trg" : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\MacRel\ctrltest.exe\
 "$(MFILE32_DEST):ctrltest.exe">"$(OUTDIR)\ctrltest.trg"

DOWNLOAD : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\MacRel\ctrltest.exe\
 "$(MFILE32_DEST):ctrltest.exe">"$(OUTDIR)\ctrltest.trg"

!ELSEIF  "$(CFG)" == "CtrlTest - Macintosh Debug"

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

ALL : "$(OUTDIR)\ctrltest.exe" "$(OUTDIR)\ctrltest.trg"

CLEAN : 
	-@erase ".\MacDbg\vc40.pdb"
	-@erase ".\MacDbg\ctrltest.pdb"
	-@erase ".\MacDbg\dertest.obj"
	-@erase ".\MacDbg\ctrltest.pch"
	-@erase ".\MacDbg\wclstest.obj"
	-@erase ".\MacDbg\subtest.obj"
	-@erase ".\MacDbg\ctrltest.obj"
	-@erase ".\MacDbg\stdafx.obj"
	-@erase ".\MacDbg\paredit.obj"
	-@erase ".\MacDbg\custmenu.obj"
	-@erase ".\MacDbg\bbutton.obj"
	-@erase ".\MacDbg\paredit2.obj"
	-@erase ".\MacDbg\custlist.obj"
	-@erase ".\MacDbg\spintest.obj"
	-@erase ".\MacDbg\ctrlmac.rsc"
	-@erase ".\MacDbg\ctrltest.rsc"
	-@erase ".\MacDbg\ctrltest.trg"
	-@erase ".\MacDbg\ctrltest.exe"

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
 "_68K_" /D "WIN32" /D "_DEBUG" /D "_MBCS" /Fp"$(INTDIR)/ctrltest.pch"\
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
RSC_PROJ=/l 0x409 /r /m /fo"$(INTDIR)/ctrltest.rsc" /d "_MAC" /d "_68K_" /d\
 "_DEBUG" 
MRC=mrc.exe
# ADD BASE MRC /D "_68K_" /D "_MAC" /D "_DEBUG" /NOLOGO
# ADD MRC /D "_68K_" /D "_MAC" /D "_DEBUG" /NOLOGO
MRC_PROJ=/D "_68K_" /D "_MAC" /D "_DEBUG" /l 0x409 /NOLOGO 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/ctrltest.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 wlm.lib /nologo /MAC:bundle /debug /machine:M68K
# ADD LINK32 /nologo /MAC:bundle /MAC:creator="CTRL" /debug /machine:M68K
LINK32_FLAGS=/nologo /MAC:bundle /MAC:type="APPL" /MAC:creator="CTRL"\
 /pdb:"$(OUTDIR)/ctrltest.pdb" /debug /machine:M68K\
 /out:"$(OUTDIR)/ctrltest.exe" 
LINK32_OBJS= \
	"$(INTDIR)/dertest.obj" \
	"$(INTDIR)/wclstest.obj" \
	"$(INTDIR)/subtest.obj" \
	"$(INTDIR)/ctrltest.obj" \
	"$(INTDIR)/stdafx.obj" \
	"$(INTDIR)/paredit.obj" \
	"$(INTDIR)/custmenu.obj" \
	"$(INTDIR)/bbutton.obj" \
	"$(INTDIR)/paredit2.obj" \
	"$(INTDIR)/custlist.obj" \
	"$(INTDIR)/spintest.obj" \
	"$(INTDIR)/ctrlmac.rsc" \
	"$(INTDIR)/ctrltest.rsc"

"$(OUTDIR)\ctrltest.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

MFILE32=mfile.exe
# ADD BASE MFILE32 COPY /NOLOGO
# ADD MFILE32 COPY /NOLOGO
MFILE32_FLAGS=COPY /NOLOGO 
MFILE32_FILES= \
	"$(OUTDIR)/ctrltest.exe"

"$(OUTDIR)\ctrltest.trg" : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\MacDbg\ctrltest.exe\
 "$(MFILE32_DEST):ctrltest.exe">"$(OUTDIR)\ctrltest.trg"

DOWNLOAD : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\MacDbg\ctrltest.exe\
 "$(MFILE32_DEST):ctrltest.exe">"$(OUTDIR)\ctrltest.trg"

!ELSEIF  "$(CFG)" == "CtrlTest - Power Macintosh Release"

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

ALL : "$(OUTDIR)\ctrltest.exe" "$(OUTDIR)\ctrltest.trg"

CLEAN : 
	-@erase ".\PMacRel\ctrltest.trg"
	-@erase ".\PMacRel\ctrltest.exe"
	-@erase ".\PMacRel\wclstest.obj"
	-@erase ".\PMacRel\ctrltest.pch"
	-@erase ".\PMacRel\ctrltest.obj"
	-@erase ".\PMacRel\dertest.obj"
	-@erase ".\PMacRel\subtest.obj"
	-@erase ".\PMacRel\custmenu.obj"
	-@erase ".\PMacRel\stdafx.obj"
	-@erase ".\PMacRel\paredit.obj"
	-@erase ".\PMacRel\custlist.obj"
	-@erase ".\PMacRel\paredit2.obj"
	-@erase ".\PMacRel\bbutton.obj"
	-@erase ".\PMacRel\spintest.obj"
	-@erase ".\PMacRel\ctrlmac.rsc"
	-@erase ".\PMacRel\ctrltest.rsc"

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
 "_WINDOWS" /D "WIN32" /D "_MBCS" /D "_AFXDLL" /Fp"$(INTDIR)/ctrltest.pch"\
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
RSC_PROJ=/l 0x409 /r /m /fo"$(INTDIR)/ctrltest.rsc" /d "_MAC" /d "_MPPC_" /d\
 "NDEBUG" 
MRC=mrc.exe
# ADD BASE MRC /D "_MPPC_" /D "_MAC" /D "NDEBUG" /NOLOGO
# ADD MRC /D "_MPPC_" /D "_MAC" /D "NDEBUG" /NOLOGO
MRC_PROJ=/D "_MPPC_" /D "_MAC" /D "NDEBUG" /l 0x409 /NOLOGO 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/ctrltest.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 cfrg.rsc /nologo /mac:bundle /machine:MPPC
# ADD LINK32 /nologo /mac:bundle /mac:creator="CTRL" /machine:MPPC
LINK32_FLAGS=/nologo /mac:bundle /mac:type="APPL" /mac:creator="CTRL"\
 /incremental:no /pdb:"$(OUTDIR)/ctrltest.pdb" /machine:MPPC\
 /out:"$(OUTDIR)/ctrltest.exe" 
LINK32_OBJS= \
	"$(INTDIR)/wclstest.obj" \
	"$(INTDIR)/ctrltest.obj" \
	"$(INTDIR)/dertest.obj" \
	"$(INTDIR)/subtest.obj" \
	"$(INTDIR)/custmenu.obj" \
	"$(INTDIR)/stdafx.obj" \
	"$(INTDIR)/paredit.obj" \
	"$(INTDIR)/custlist.obj" \
	"$(INTDIR)/paredit2.obj" \
	"$(INTDIR)/bbutton.obj" \
	"$(INTDIR)/spintest.obj" \
	"$(INTDIR)/ctrlmac.rsc" \
	"$(INTDIR)/ctrltest.rsc"

"$(OUTDIR)\ctrltest.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

MFILE32=mfile.exe
# ADD BASE MFILE32 COPY /NOLOGO
# ADD MFILE32 COPY /NOLOGO
MFILE32_FLAGS=COPY /NOLOGO 
MFILE32_FILES= \
	"$(OUTDIR)/ctrltest.exe"

"$(OUTDIR)\ctrltest.trg" : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\PMacRel\ctrltest.exe\
 "$(MFILE32_DEST):ctrltest.exe">"$(OUTDIR)\ctrltest.trg"

DOWNLOAD : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\PMacRel\ctrltest.exe\
 "$(MFILE32_DEST):ctrltest.exe">"$(OUTDIR)\ctrltest.trg"

!ELSEIF  "$(CFG)" == "CtrlTest - Power Macintosh Debug"

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

ALL : "$(OUTDIR)\ctrltest.exe" "$(OUTDIR)\ctrltest.trg"

CLEAN : 
	-@erase ".\PMacDbg\vc40.pdb"
	-@erase ".\PMacDbg\ctrltest.ilk"
	-@erase ".\PMacDbg\wclstest.obj"
	-@erase ".\PMacDbg\ctrltest.pch"
	-@erase ".\PMacDbg\ctrltest.obj"
	-@erase ".\PMacDbg\bbutton.obj"
	-@erase ".\PMacDbg\stdafx.obj"
	-@erase ".\PMacDbg\custmenu.obj"
	-@erase ".\PMacDbg\dertest.obj"
	-@erase ".\PMacDbg\subtest.obj"
	-@erase ".\PMacDbg\paredit.obj"
	-@erase ".\PMacDbg\paredit2.obj"
	-@erase ".\PMacDbg\custlist.obj"
	-@erase ".\PMacDbg\spintest.obj"
	-@erase ".\PMacDbg\ctrlmac.rsc"
	-@erase ".\PMacDbg\ctrltest.rsc"
	-@erase ".\PMacDbg\ctrltest.pdb"
	-@erase ".\PMacDbg\ctrltest.trg"
	-@erase ".\PMacDbg\ctrltest.exe"

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
 "_WINDOWS" /D "WIN32" /D "_MBCS" /D "_AFXDLL" /Fp"$(INTDIR)/ctrltest.pch"\
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
RSC_PROJ=/l 0x409 /r /m /fo"$(INTDIR)/ctrltest.rsc" /d "_MAC" /d "_MPPC_" /d\
 "_DEBUG" 
MRC=mrc.exe
# ADD BASE MRC /D "_MPPC_" /D "_MAC" /D "_DEBUG" /NOLOGO
# ADD MRC /D "_MPPC_" /D "_MAC" /D "_DEBUG" /NOLOGO
MRC_PROJ=/D "_MPPC_" /D "_MAC" /D "_DEBUG" /l 0x409 /NOLOGO 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/ctrltest.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 cfrg.rsc /nologo /mac:bundle /debug /machine:MPPC
# ADD LINK32 /nologo /mac:bundle /mac:creator="CTRL" /debug /machine:MPPC
LINK32_FLAGS=/nologo /mac:bundle /mac:type="APPL" /mac:creator="CTRL"\
 /incremental:yes /pdb:"$(OUTDIR)/ctrltest.pdb" /debug /machine:MPPC\
 /out:"$(OUTDIR)/ctrltest.exe" 
LINK32_OBJS= \
	"$(INTDIR)/wclstest.obj" \
	"$(INTDIR)/ctrltest.obj" \
	"$(INTDIR)/bbutton.obj" \
	"$(INTDIR)/stdafx.obj" \
	"$(INTDIR)/custmenu.obj" \
	"$(INTDIR)/dertest.obj" \
	"$(INTDIR)/subtest.obj" \
	"$(INTDIR)/paredit.obj" \
	"$(INTDIR)/paredit2.obj" \
	"$(INTDIR)/custlist.obj" \
	"$(INTDIR)/spintest.obj" \
	"$(INTDIR)/ctrlmac.rsc" \
	"$(INTDIR)/ctrltest.rsc"

"$(OUTDIR)\ctrltest.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

MFILE32=mfile.exe
# ADD BASE MFILE32 COPY /NOLOGO
# ADD MFILE32 COPY /NOLOGO
MFILE32_FLAGS=COPY /NOLOGO 
MFILE32_FILES= \
	"$(OUTDIR)/ctrltest.exe"

"$(OUTDIR)\ctrltest.trg" : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\PMacDbg\ctrltest.exe\
 "$(MFILE32_DEST):ctrltest.exe">"$(OUTDIR)\ctrltest.trg"

DOWNLOAD : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\PMacDbg\ctrltest.exe\
 "$(MFILE32_DEST):ctrltest.exe">"$(OUTDIR)\ctrltest.trg"

!ENDIF 

################################################################################
# Begin Target

# Name "CtrlTest - Win32 Release"
# Name "CtrlTest - Win32 Debug"
# Name "CtrlTest - Macintosh Release"
# Name "CtrlTest - Macintosh Debug"
# Name "CtrlTest - Power Macintosh Release"
# Name "CtrlTest - Power Macintosh Debug"

!IF  "$(CFG)" == "CtrlTest - Win32 Release"

!ELSEIF  "$(CFG)" == "CtrlTest - Win32 Debug"

!ELSEIF  "$(CFG)" == "CtrlTest - Macintosh Release"

!ELSEIF  "$(CFG)" == "CtrlTest - Macintosh Debug"

!ELSEIF  "$(CFG)" == "CtrlTest - Power Macintosh Release"

!ELSEIF  "$(CFG)" == "CtrlTest - Power Macintosh Debug"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\stdafx.cpp
DEP_CPP_STDAF=\
	".\stdafx.h"\
	

!IF  "$(CFG)" == "CtrlTest - Win32 Release"

# ADD CPP /Yc

BuildCmds= \
	$(CPP) /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/ctrltest.pch" /Yc /Fo"$(INTDIR)/" /c\
 $(SOURCE) \
	

"$(INTDIR)\stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\ctrltest.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "CtrlTest - Win32 Debug"

# ADD CPP /Yc

BuildCmds= \
	$(CPP) /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/ctrltest.pch" /Yc /Fo"$(INTDIR)/"\
 /Fd"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\ctrltest.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "CtrlTest - Macintosh Release"

# ADD CPP /Yc

BuildCmds= \
	$(CPP) /nologo /AL /Q68s /W3 /GX /O2 /D "_WINDOWS" /D "_MAC" /D "_68K_" /D\
 "WIN32" /D "NDEBUG" /D "_MBCS" /Fp"$(INTDIR)/ctrltest.pch" /Yc /Fo"$(INTDIR)/"\
 /c $(SOURCE) \
	

"$(INTDIR)\stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\ctrltest.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "CtrlTest - Macintosh Debug"

# ADD CPP /Yc

BuildCmds= \
	$(CPP) /nologo /AL /Q68s /Q68m /W3 /GX /Zi /Od /D "_WINDOWS" /D "_MAC" /D\
 "_68K_" /D "WIN32" /D "_DEBUG" /D "_MBCS" /Fp"$(INTDIR)/ctrltest.pch" /Yc\
 /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\ctrltest.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "CtrlTest - Power Macintosh Release"

# ADD CPP /Yc

BuildCmds= \
	$(CPP) /nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "_MAC" /D "_MPPC_" /D "_WINDOWS"\
 /D "WIN32" /D "_MBCS" /D "_AFXDLL" /Fp"$(INTDIR)/ctrltest.pch" /Yc\
 /Fo"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\ctrltest.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "CtrlTest - Power Macintosh Debug"

# ADD CPP /Yc

BuildCmds= \
	$(CPP) /nologo /MDd /W3 /GX /Zi /Od /D "_DEBUG" /D "_MAC" /D "_MPPC_" /D\
 "_WINDOWS" /D "WIN32" /D "_MBCS" /D "_AFXDLL" /Fp"$(INTDIR)/ctrltest.pch" /Yc\
 /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\ctrltest.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\ctrltest.cpp
DEP_CPP_CTRLT=\
	".\ctrltest.h"\
	".\stdafx.h"\
	

!IF  "$(CFG)" == "CtrlTest - Win32 Release"


"$(INTDIR)\ctrltest.obj" : $(SOURCE) $(DEP_CPP_CTRLT) "$(INTDIR)"\
 "$(INTDIR)\ctrltest.pch"


!ELSEIF  "$(CFG)" == "CtrlTest - Win32 Debug"


"$(INTDIR)\ctrltest.obj" : $(SOURCE) $(DEP_CPP_CTRLT) "$(INTDIR)"\
 "$(INTDIR)\ctrltest.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_CTRLT) "$(INTDIR)"\
 "$(INTDIR)\ctrltest.pch"


!ELSEIF  "$(CFG)" == "CtrlTest - Macintosh Release"


"$(INTDIR)\ctrltest.obj" : $(SOURCE) $(DEP_CPP_CTRLT) "$(INTDIR)"\
 "$(INTDIR)\ctrltest.pch"


!ELSEIF  "$(CFG)" == "CtrlTest - Macintosh Debug"


"$(INTDIR)\ctrltest.obj" : $(SOURCE) $(DEP_CPP_CTRLT) "$(INTDIR)"\
 "$(INTDIR)\ctrltest.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_CTRLT) "$(INTDIR)"\
 "$(INTDIR)\ctrltest.pch"


!ELSEIF  "$(CFG)" == "CtrlTest - Power Macintosh Release"


"$(INTDIR)\ctrltest.obj" : $(SOURCE) $(DEP_CPP_CTRLT) "$(INTDIR)"\
 "$(INTDIR)\ctrltest.pch"


!ELSEIF  "$(CFG)" == "CtrlTest - Power Macintosh Debug"


"$(INTDIR)\ctrltest.obj" : $(SOURCE) $(DEP_CPP_CTRLT) "$(INTDIR)"\
 "$(INTDIR)\ctrltest.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_CTRLT) "$(INTDIR)"\
 "$(INTDIR)\ctrltest.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\custlist.cpp
DEP_CPP_CUSTL=\
	".\ctrltest.h"\
	".\stdafx.h"\
	

!IF  "$(CFG)" == "CtrlTest - Win32 Release"


"$(INTDIR)\custlist.obj" : $(SOURCE) $(DEP_CPP_CUSTL) "$(INTDIR)"\
 "$(INTDIR)\ctrltest.pch"


!ELSEIF  "$(CFG)" == "CtrlTest - Win32 Debug"


"$(INTDIR)\custlist.obj" : $(SOURCE) $(DEP_CPP_CUSTL) "$(INTDIR)"\
 "$(INTDIR)\ctrltest.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_CUSTL) "$(INTDIR)"\
 "$(INTDIR)\ctrltest.pch"


!ELSEIF  "$(CFG)" == "CtrlTest - Macintosh Release"


"$(INTDIR)\custlist.obj" : $(SOURCE) $(DEP_CPP_CUSTL) "$(INTDIR)"\
 "$(INTDIR)\ctrltest.pch"


!ELSEIF  "$(CFG)" == "CtrlTest - Macintosh Debug"


"$(INTDIR)\custlist.obj" : $(SOURCE) $(DEP_CPP_CUSTL) "$(INTDIR)"\
 "$(INTDIR)\ctrltest.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_CUSTL) "$(INTDIR)"\
 "$(INTDIR)\ctrltest.pch"


!ELSEIF  "$(CFG)" == "CtrlTest - Power Macintosh Release"


"$(INTDIR)\custlist.obj" : $(SOURCE) $(DEP_CPP_CUSTL) "$(INTDIR)"\
 "$(INTDIR)\ctrltest.pch"


!ELSEIF  "$(CFG)" == "CtrlTest - Power Macintosh Debug"


"$(INTDIR)\custlist.obj" : $(SOURCE) $(DEP_CPP_CUSTL) "$(INTDIR)"\
 "$(INTDIR)\ctrltest.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_CUSTL) "$(INTDIR)"\
 "$(INTDIR)\ctrltest.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\custmenu.cpp
DEP_CPP_CUSTM=\
	".\ctrltest.h"\
	".\stdafx.h"\
	

!IF  "$(CFG)" == "CtrlTest - Win32 Release"


"$(INTDIR)\custmenu.obj" : $(SOURCE) $(DEP_CPP_CUSTM) "$(INTDIR)"\
 "$(INTDIR)\ctrltest.pch"


!ELSEIF  "$(CFG)" == "CtrlTest - Win32 Debug"


"$(INTDIR)\custmenu.obj" : $(SOURCE) $(DEP_CPP_CUSTM) "$(INTDIR)"\
 "$(INTDIR)\ctrltest.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_CUSTM) "$(INTDIR)"\
 "$(INTDIR)\ctrltest.pch"


!ELSEIF  "$(CFG)" == "CtrlTest - Macintosh Release"


"$(INTDIR)\custmenu.obj" : $(SOURCE) $(DEP_CPP_CUSTM) "$(INTDIR)"\
 "$(INTDIR)\ctrltest.pch"


!ELSEIF  "$(CFG)" == "CtrlTest - Macintosh Debug"


"$(INTDIR)\custmenu.obj" : $(SOURCE) $(DEP_CPP_CUSTM) "$(INTDIR)"\
 "$(INTDIR)\ctrltest.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_CUSTM) "$(INTDIR)"\
 "$(INTDIR)\ctrltest.pch"


!ELSEIF  "$(CFG)" == "CtrlTest - Power Macintosh Release"


"$(INTDIR)\custmenu.obj" : $(SOURCE) $(DEP_CPP_CUSTM) "$(INTDIR)"\
 "$(INTDIR)\ctrltest.pch"


!ELSEIF  "$(CFG)" == "CtrlTest - Power Macintosh Debug"


"$(INTDIR)\custmenu.obj" : $(SOURCE) $(DEP_CPP_CUSTM) "$(INTDIR)"\
 "$(INTDIR)\ctrltest.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_CUSTM) "$(INTDIR)"\
 "$(INTDIR)\ctrltest.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\dertest.cpp
DEP_CPP_DERTE=\
	".\stdafx.h"\
	".\ctrltest.h"\
	".\paredit.h"\
	

!IF  "$(CFG)" == "CtrlTest - Win32 Release"


"$(INTDIR)\dertest.obj" : $(SOURCE) $(DEP_CPP_DERTE) "$(INTDIR)"\
 "$(INTDIR)\ctrltest.pch"


!ELSEIF  "$(CFG)" == "CtrlTest - Win32 Debug"


"$(INTDIR)\dertest.obj" : $(SOURCE) $(DEP_CPP_DERTE) "$(INTDIR)"\
 "$(INTDIR)\ctrltest.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_DERTE) "$(INTDIR)"\
 "$(INTDIR)\ctrltest.pch"


!ELSEIF  "$(CFG)" == "CtrlTest - Macintosh Release"


"$(INTDIR)\dertest.obj" : $(SOURCE) $(DEP_CPP_DERTE) "$(INTDIR)"\
 "$(INTDIR)\ctrltest.pch"


!ELSEIF  "$(CFG)" == "CtrlTest - Macintosh Debug"


"$(INTDIR)\dertest.obj" : $(SOURCE) $(DEP_CPP_DERTE) "$(INTDIR)"\
 "$(INTDIR)\ctrltest.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_DERTE) "$(INTDIR)"\
 "$(INTDIR)\ctrltest.pch"


!ELSEIF  "$(CFG)" == "CtrlTest - Power Macintosh Release"


"$(INTDIR)\dertest.obj" : $(SOURCE) $(DEP_CPP_DERTE) "$(INTDIR)"\
 "$(INTDIR)\ctrltest.pch"


!ELSEIF  "$(CFG)" == "CtrlTest - Power Macintosh Debug"


"$(INTDIR)\dertest.obj" : $(SOURCE) $(DEP_CPP_DERTE) "$(INTDIR)"\
 "$(INTDIR)\ctrltest.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_DERTE) "$(INTDIR)"\
 "$(INTDIR)\ctrltest.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\paredit.cpp
DEP_CPP_PARED=\
	".\stdafx.h"\
	".\ctrltest.h"\
	".\paredit.h"\
	

!IF  "$(CFG)" == "CtrlTest - Win32 Release"


"$(INTDIR)\paredit.obj" : $(SOURCE) $(DEP_CPP_PARED) "$(INTDIR)"\
 "$(INTDIR)\ctrltest.pch"


!ELSEIF  "$(CFG)" == "CtrlTest - Win32 Debug"


"$(INTDIR)\paredit.obj" : $(SOURCE) $(DEP_CPP_PARED) "$(INTDIR)"\
 "$(INTDIR)\ctrltest.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_PARED) "$(INTDIR)"\
 "$(INTDIR)\ctrltest.pch"


!ELSEIF  "$(CFG)" == "CtrlTest - Macintosh Release"


"$(INTDIR)\paredit.obj" : $(SOURCE) $(DEP_CPP_PARED) "$(INTDIR)"\
 "$(INTDIR)\ctrltest.pch"


!ELSEIF  "$(CFG)" == "CtrlTest - Macintosh Debug"


"$(INTDIR)\paredit.obj" : $(SOURCE) $(DEP_CPP_PARED) "$(INTDIR)"\
 "$(INTDIR)\ctrltest.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_PARED) "$(INTDIR)"\
 "$(INTDIR)\ctrltest.pch"


!ELSEIF  "$(CFG)" == "CtrlTest - Power Macintosh Release"


"$(INTDIR)\paredit.obj" : $(SOURCE) $(DEP_CPP_PARED) "$(INTDIR)"\
 "$(INTDIR)\ctrltest.pch"


!ELSEIF  "$(CFG)" == "CtrlTest - Power Macintosh Debug"


"$(INTDIR)\paredit.obj" : $(SOURCE) $(DEP_CPP_PARED) "$(INTDIR)"\
 "$(INTDIR)\ctrltest.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_PARED) "$(INTDIR)"\
 "$(INTDIR)\ctrltest.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\paredit2.cpp
DEP_CPP_PAREDI=\
	".\stdafx.h"\
	".\ctrltest.h"\
	".\paredit.h"\
	

!IF  "$(CFG)" == "CtrlTest - Win32 Release"


"$(INTDIR)\paredit2.obj" : $(SOURCE) $(DEP_CPP_PAREDI) "$(INTDIR)"\
 "$(INTDIR)\ctrltest.pch"


!ELSEIF  "$(CFG)" == "CtrlTest - Win32 Debug"


"$(INTDIR)\paredit2.obj" : $(SOURCE) $(DEP_CPP_PAREDI) "$(INTDIR)"\
 "$(INTDIR)\ctrltest.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_PAREDI) "$(INTDIR)"\
 "$(INTDIR)\ctrltest.pch"


!ELSEIF  "$(CFG)" == "CtrlTest - Macintosh Release"


"$(INTDIR)\paredit2.obj" : $(SOURCE) $(DEP_CPP_PAREDI) "$(INTDIR)"\
 "$(INTDIR)\ctrltest.pch"


!ELSEIF  "$(CFG)" == "CtrlTest - Macintosh Debug"


"$(INTDIR)\paredit2.obj" : $(SOURCE) $(DEP_CPP_PAREDI) "$(INTDIR)"\
 "$(INTDIR)\ctrltest.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_PAREDI) "$(INTDIR)"\
 "$(INTDIR)\ctrltest.pch"


!ELSEIF  "$(CFG)" == "CtrlTest - Power Macintosh Release"


"$(INTDIR)\paredit2.obj" : $(SOURCE) $(DEP_CPP_PAREDI) "$(INTDIR)"\
 "$(INTDIR)\ctrltest.pch"


!ELSEIF  "$(CFG)" == "CtrlTest - Power Macintosh Debug"


"$(INTDIR)\paredit2.obj" : $(SOURCE) $(DEP_CPP_PAREDI) "$(INTDIR)"\
 "$(INTDIR)\ctrltest.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_PAREDI) "$(INTDIR)"\
 "$(INTDIR)\ctrltest.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\spintest.cpp
DEP_CPP_SPINT=\
	".\stdafx.h"\
	".\ctrltest.h"\
	".\paredit.h"\
	

!IF  "$(CFG)" == "CtrlTest - Win32 Release"


"$(INTDIR)\spintest.obj" : $(SOURCE) $(DEP_CPP_SPINT) "$(INTDIR)"\
 "$(INTDIR)\ctrltest.pch"


!ELSEIF  "$(CFG)" == "CtrlTest - Win32 Debug"


"$(INTDIR)\spintest.obj" : $(SOURCE) $(DEP_CPP_SPINT) "$(INTDIR)"\
 "$(INTDIR)\ctrltest.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_SPINT) "$(INTDIR)"\
 "$(INTDIR)\ctrltest.pch"


!ELSEIF  "$(CFG)" == "CtrlTest - Macintosh Release"


"$(INTDIR)\spintest.obj" : $(SOURCE) $(DEP_CPP_SPINT) "$(INTDIR)"\
 "$(INTDIR)\ctrltest.pch"


!ELSEIF  "$(CFG)" == "CtrlTest - Macintosh Debug"


"$(INTDIR)\spintest.obj" : $(SOURCE) $(DEP_CPP_SPINT) "$(INTDIR)"\
 "$(INTDIR)\ctrltest.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_SPINT) "$(INTDIR)"\
 "$(INTDIR)\ctrltest.pch"


!ELSEIF  "$(CFG)" == "CtrlTest - Power Macintosh Release"


"$(INTDIR)\spintest.obj" : $(SOURCE) $(DEP_CPP_SPINT) "$(INTDIR)"\
 "$(INTDIR)\ctrltest.pch"


!ELSEIF  "$(CFG)" == "CtrlTest - Power Macintosh Debug"


"$(INTDIR)\spintest.obj" : $(SOURCE) $(DEP_CPP_SPINT) "$(INTDIR)"\
 "$(INTDIR)\ctrltest.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_SPINT) "$(INTDIR)"\
 "$(INTDIR)\ctrltest.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\bbutton.cpp
DEP_CPP_BBUTT=\
	".\stdafx.h"\
	".\ctrltest.h"\
	

!IF  "$(CFG)" == "CtrlTest - Win32 Release"


"$(INTDIR)\bbutton.obj" : $(SOURCE) $(DEP_CPP_BBUTT) "$(INTDIR)"\
 "$(INTDIR)\ctrltest.pch"


!ELSEIF  "$(CFG)" == "CtrlTest - Win32 Debug"


"$(INTDIR)\bbutton.obj" : $(SOURCE) $(DEP_CPP_BBUTT) "$(INTDIR)"\
 "$(INTDIR)\ctrltest.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_BBUTT) "$(INTDIR)"\
 "$(INTDIR)\ctrltest.pch"


!ELSEIF  "$(CFG)" == "CtrlTest - Macintosh Release"


"$(INTDIR)\bbutton.obj" : $(SOURCE) $(DEP_CPP_BBUTT) "$(INTDIR)"\
 "$(INTDIR)\ctrltest.pch"


!ELSEIF  "$(CFG)" == "CtrlTest - Macintosh Debug"


"$(INTDIR)\bbutton.obj" : $(SOURCE) $(DEP_CPP_BBUTT) "$(INTDIR)"\
 "$(INTDIR)\ctrltest.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_BBUTT) "$(INTDIR)"\
 "$(INTDIR)\ctrltest.pch"


!ELSEIF  "$(CFG)" == "CtrlTest - Power Macintosh Release"


"$(INTDIR)\bbutton.obj" : $(SOURCE) $(DEP_CPP_BBUTT) "$(INTDIR)"\
 "$(INTDIR)\ctrltest.pch"


!ELSEIF  "$(CFG)" == "CtrlTest - Power Macintosh Debug"


"$(INTDIR)\bbutton.obj" : $(SOURCE) $(DEP_CPP_BBUTT) "$(INTDIR)"\
 "$(INTDIR)\ctrltest.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_BBUTT) "$(INTDIR)"\
 "$(INTDIR)\ctrltest.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\subtest.cpp
DEP_CPP_SUBTE=\
	".\stdafx.h"\
	".\ctrltest.h"\
	".\paredit.h"\
	

!IF  "$(CFG)" == "CtrlTest - Win32 Release"


"$(INTDIR)\subtest.obj" : $(SOURCE) $(DEP_CPP_SUBTE) "$(INTDIR)"\
 "$(INTDIR)\ctrltest.pch"


!ELSEIF  "$(CFG)" == "CtrlTest - Win32 Debug"


"$(INTDIR)\subtest.obj" : $(SOURCE) $(DEP_CPP_SUBTE) "$(INTDIR)"\
 "$(INTDIR)\ctrltest.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_SUBTE) "$(INTDIR)"\
 "$(INTDIR)\ctrltest.pch"


!ELSEIF  "$(CFG)" == "CtrlTest - Macintosh Release"


"$(INTDIR)\subtest.obj" : $(SOURCE) $(DEP_CPP_SUBTE) "$(INTDIR)"\
 "$(INTDIR)\ctrltest.pch"


!ELSEIF  "$(CFG)" == "CtrlTest - Macintosh Debug"


"$(INTDIR)\subtest.obj" : $(SOURCE) $(DEP_CPP_SUBTE) "$(INTDIR)"\
 "$(INTDIR)\ctrltest.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_SUBTE) "$(INTDIR)"\
 "$(INTDIR)\ctrltest.pch"


!ELSEIF  "$(CFG)" == "CtrlTest - Power Macintosh Release"


"$(INTDIR)\subtest.obj" : $(SOURCE) $(DEP_CPP_SUBTE) "$(INTDIR)"\
 "$(INTDIR)\ctrltest.pch"


!ELSEIF  "$(CFG)" == "CtrlTest - Power Macintosh Debug"


"$(INTDIR)\subtest.obj" : $(SOURCE) $(DEP_CPP_SUBTE) "$(INTDIR)"\
 "$(INTDIR)\ctrltest.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_SUBTE) "$(INTDIR)"\
 "$(INTDIR)\ctrltest.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\wclstest.cpp
DEP_CPP_WCLST=\
	".\stdafx.h"\
	".\ctrltest.h"\
	".\paredit.h"\
	

!IF  "$(CFG)" == "CtrlTest - Win32 Release"


"$(INTDIR)\wclstest.obj" : $(SOURCE) $(DEP_CPP_WCLST) "$(INTDIR)"\
 "$(INTDIR)\ctrltest.pch"


!ELSEIF  "$(CFG)" == "CtrlTest - Win32 Debug"


"$(INTDIR)\wclstest.obj" : $(SOURCE) $(DEP_CPP_WCLST) "$(INTDIR)"\
 "$(INTDIR)\ctrltest.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_WCLST) "$(INTDIR)"\
 "$(INTDIR)\ctrltest.pch"


!ELSEIF  "$(CFG)" == "CtrlTest - Macintosh Release"


"$(INTDIR)\wclstest.obj" : $(SOURCE) $(DEP_CPP_WCLST) "$(INTDIR)"\
 "$(INTDIR)\ctrltest.pch"


!ELSEIF  "$(CFG)" == "CtrlTest - Macintosh Debug"


"$(INTDIR)\wclstest.obj" : $(SOURCE) $(DEP_CPP_WCLST) "$(INTDIR)"\
 "$(INTDIR)\ctrltest.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_WCLST) "$(INTDIR)"\
 "$(INTDIR)\ctrltest.pch"


!ELSEIF  "$(CFG)" == "CtrlTest - Power Macintosh Release"


"$(INTDIR)\wclstest.obj" : $(SOURCE) $(DEP_CPP_WCLST) "$(INTDIR)"\
 "$(INTDIR)\ctrltest.pch"


!ELSEIF  "$(CFG)" == "CtrlTest - Power Macintosh Debug"


"$(INTDIR)\wclstest.obj" : $(SOURCE) $(DEP_CPP_WCLST) "$(INTDIR)"\
 "$(INTDIR)\ctrltest.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_WCLST) "$(INTDIR)"\
 "$(INTDIR)\ctrltest.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\ctrltest.rc
DEP_RSC_CTRLTE=\
	".\res\ctrltest.ico"\
	".\res\image1u.bmp"\
	".\res\image1d.bmp"\
	".\res\image1f.bmp"\
	".\res\image2u.bmp"\
	".\res\image2d.bmp"\
	".\res\image2f.bmp"\
	".\res\oku.bmp"\
	".\res\okd.bmp"\
	".\res\okf.bmp"\
	".\res\cancelu.bmp"\
	".\res\canceld.bmp"\
	".\res\cancelf.bmp"\
	".\res\nextu.bmp"\
	".\res\nextd.bmp"\
	".\res\nextf.bmp"\
	".\res\nextx.bmp"\
	".\res\prevu.bmp"\
	".\res\prevd.bmp"\
	".\res\prevf.bmp"\
	".\res\prevx.bmp"\
	".\res\ctrltest.rc2"\
	

!IF  "$(CFG)" == "CtrlTest - Win32 Release"


"$(INTDIR)\ctrltest.res" : $(SOURCE) $(DEP_RSC_CTRLTE) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "CtrlTest - Win32 Debug"


"$(INTDIR)\ctrltest.res" : $(SOURCE) $(DEP_RSC_CTRLTE) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "CtrlTest - Macintosh Release"


"$(INTDIR)\ctrltest.rsc" : $(SOURCE) $(DEP_RSC_CTRLTE) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "CtrlTest - Macintosh Debug"


"$(INTDIR)\ctrltest.rsc" : $(SOURCE) $(DEP_RSC_CTRLTE) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "CtrlTest - Power Macintosh Release"


"$(INTDIR)\ctrltest.rsc" : $(SOURCE) $(DEP_RSC_CTRLTE) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "CtrlTest - Power Macintosh Debug"


"$(INTDIR)\ctrltest.rsc" : $(SOURCE) $(DEP_RSC_CTRLTE) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\ctrlmac.r

!IF  "$(CFG)" == "CtrlTest - Win32 Release"

!ELSEIF  "$(CFG)" == "CtrlTest - Win32 Debug"

!ELSEIF  "$(CFG)" == "CtrlTest - Macintosh Release"

DEP_MRC_CTRLM=\
	{$(INCLUDE)}"\mrc\types.r"\
	{$(INCLUDE)}"\mrc\balloons.r"\
	

"$(INTDIR)\ctrlmac.rsc" : $(SOURCE) $(DEP_MRC_CTRLM) "$(INTDIR)"
   $(MRC) /o"$(INTDIR)/ctrlmac.rsc" /D "_68K_" /D "_MAC" /D "NDEBUG" /l 0x409\
 /NOLOGO $(SOURCE)


!ELSEIF  "$(CFG)" == "CtrlTest - Macintosh Debug"

DEP_MRC_CTRLM=\
	{$(INCLUDE)}"\mrc\types.r"\
	{$(INCLUDE)}"\mrc\balloons.r"\
	

"$(INTDIR)\ctrlmac.rsc" : $(SOURCE) $(DEP_MRC_CTRLM) "$(INTDIR)"
   $(MRC) /o"$(INTDIR)/ctrlmac.rsc" /D "_68K_" /D "_MAC" /D "_DEBUG" /l 0x409\
 /NOLOGO $(SOURCE)


!ELSEIF  "$(CFG)" == "CtrlTest - Power Macintosh Release"

DEP_MRC_CTRLM=\
	{$(INCLUDE)}"\mrc\types.r"\
	{$(INCLUDE)}"\mrc\balloons.r"\
	

"$(INTDIR)\ctrlmac.rsc" : $(SOURCE) $(DEP_MRC_CTRLM) "$(INTDIR)"
   $(MRC) /o"$(INTDIR)/ctrlmac.rsc" /D "_MPPC_" /D "_MAC" /D "NDEBUG" /l 0x409\
 /NOLOGO $(SOURCE)


!ELSEIF  "$(CFG)" == "CtrlTest - Power Macintosh Debug"

DEP_MRC_CTRLM=\
	{$(INCLUDE)}"\mrc\types.r"\
	{$(INCLUDE)}"\mrc\balloons.r"\
	

"$(INTDIR)\ctrlmac.rsc" : $(SOURCE) $(DEP_MRC_CTRLM) "$(INTDIR)"
   $(MRC) /o"$(INTDIR)/ctrlmac.rsc" /D "_MPPC_" /D "_MAC" /D "_DEBUG" /l 0x409\
 /NOLOGO $(SOURCE)


!ENDIF 

# End Source File
# End Target
# End Project
################################################################################
