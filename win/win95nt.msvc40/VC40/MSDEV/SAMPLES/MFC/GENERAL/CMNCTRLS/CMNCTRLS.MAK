# Microsoft Visual C++ Generated NMAKE File, Format Version 40001
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101
# TARGTYPE "Power Macintosh Application" 0x0401
# TARGTYPE "Macintosh Application" 0x0301

!IF "$(CFG)" == ""
CFG=cmnctrls - Win32 Debug
!MESSAGE No configuration specified.  Defaulting to cmnctrls - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "cmnctrls - Win32 Release" && "$(CFG)" !=\
 "cmnctrls - Win32 Debug" && "$(CFG)" != "cmnctrls - Macintosh Release" &&\
 "$(CFG)" != "cmnctrls - Macintosh Debug" && "$(CFG)" !=\
 "cmnctrls - Power Macintosh Release" && "$(CFG)" !=\
 "cmnctrls - Power Macintosh Debug" && "$(CFG)" !=\
 "cmnctrls - Win32 Unicode Debug" && "$(CFG)" !=\
 "cmnctrls - Win32 Unicode Release"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "cmnctrls.mak" CFG="cmnctrls - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "cmnctrls - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "cmnctrls - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE "cmnctrls - Macintosh Release" (based on "Macintosh Application")
!MESSAGE "cmnctrls - Macintosh Debug" (based on "Macintosh Application")
!MESSAGE "cmnctrls - Power Macintosh Release" (based on\
 "Power Macintosh Application")
!MESSAGE "cmnctrls - Power Macintosh Debug" (based on\
 "Power Macintosh Application")
!MESSAGE "cmnctrls - Win32 Unicode Debug" (based on "Win32 (x86) Application")
!MESSAGE "cmnctrls - Win32 Unicode Release" (based on\
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
# PROP Target_Last_Scanned "cmnctrls - Win32 Debug"

!IF  "$(CFG)" == "cmnctrls - Win32 Release"

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

ALL : "$(OUTDIR)\cmnctrls.exe"

CLEAN : 
	-@erase ".\Release\cmnctrls.exe"
	-@erase ".\Release\progctrl.obj"
	-@erase ".\Release\cmnctrls.pch"
	-@erase ".\Release\treecpg.obj"
	-@erase ".\Release\propsht.obj"
	-@erase ".\Release\animctrl.obj"
	-@erase ".\Release\notifwdw.obj"
	-@erase ".\Release\mlistctl.obj"
	-@erase ".\Release\toolpage.obj"
	-@erase ".\Release\toolbar1.obj"
	-@erase ".\Release\spinctrl.obj"
	-@erase ".\Release\toolbar2.obj"
	-@erase ".\Release\stdafx.obj"
	-@erase ".\Release\listcpg.obj"
	-@erase ".\Release\slidctrl.obj"
	-@erase ".\Release\mtreectl.obj"
	-@erase ".\Release\ctrldemo.obj"
	-@erase ".\Release\cmnctrls.res"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
CPP_PROJ=/nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/cmnctrls.pch" /Yu"stdafx.h" /Fo"$(INTDIR)/"\
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
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/cmnctrls.res" /d "NDEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/cmnctrls.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 /nologo /subsystem:windows /machine:I386
LINK32_FLAGS=/nologo /subsystem:windows /incremental:no\
 /pdb:"$(OUTDIR)/cmnctrls.pdb" /machine:I386 /out:"$(OUTDIR)/cmnctrls.exe" 
LINK32_OBJS= \
	"$(INTDIR)/progctrl.obj" \
	"$(INTDIR)/treecpg.obj" \
	"$(INTDIR)/propsht.obj" \
	"$(INTDIR)/animctrl.obj" \
	"$(INTDIR)/notifwdw.obj" \
	"$(INTDIR)/mlistctl.obj" \
	"$(INTDIR)/toolpage.obj" \
	"$(INTDIR)/toolbar1.obj" \
	"$(INTDIR)/spinctrl.obj" \
	"$(INTDIR)/toolbar2.obj" \
	"$(INTDIR)/stdafx.obj" \
	"$(INTDIR)/listcpg.obj" \
	"$(INTDIR)/slidctrl.obj" \
	"$(INTDIR)/mtreectl.obj" \
	"$(INTDIR)/ctrldemo.obj" \
	"$(INTDIR)/cmnctrls.res"

"$(OUTDIR)\cmnctrls.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "cmnctrls - Win32 Debug"

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

ALL : "$(OUTDIR)\cmnctrls.exe"

CLEAN : 
	-@erase ".\Debug\vc40.pdb"
	-@erase ".\Debug\cmnctrls.pch"
	-@erase ".\Debug\cmnctrls.exe"
	-@erase ".\Debug\notifwdw.obj"
	-@erase ".\Debug\mlistctl.obj"
	-@erase ".\Debug\toolpage.obj"
	-@erase ".\Debug\stdafx.obj"
	-@erase ".\Debug\listcpg.obj"
	-@erase ".\Debug\spinctrl.obj"
	-@erase ".\Debug\toolbar1.obj"
	-@erase ".\Debug\toolbar2.obj"
	-@erase ".\Debug\slidctrl.obj"
	-@erase ".\Debug\mtreectl.obj"
	-@erase ".\Debug\treecpg.obj"
	-@erase ".\Debug\ctrldemo.obj"
	-@erase ".\Debug\propsht.obj"
	-@erase ".\Debug\progctrl.obj"
	-@erase ".\Debug\animctrl.obj"
	-@erase ".\Debug\cmnctrls.res"
	-@erase ".\Debug\cmnctrls.ilk"
	-@erase ".\Debug\cmnctrls.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
CPP_PROJ=/nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/cmnctrls.pch" /Yu"stdafx.h"\
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
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/cmnctrls.res" /d "_DEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/cmnctrls.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386
# ADD LINK32 /nologo /subsystem:windows /debug /machine:I386
LINK32_FLAGS=/nologo /subsystem:windows /incremental:yes\
 /pdb:"$(OUTDIR)/cmnctrls.pdb" /debug /machine:I386\
 /out:"$(OUTDIR)/cmnctrls.exe" 
LINK32_OBJS= \
	"$(INTDIR)/notifwdw.obj" \
	"$(INTDIR)/mlistctl.obj" \
	"$(INTDIR)/toolpage.obj" \
	"$(INTDIR)/stdafx.obj" \
	"$(INTDIR)/listcpg.obj" \
	"$(INTDIR)/spinctrl.obj" \
	"$(INTDIR)/toolbar1.obj" \
	"$(INTDIR)/toolbar2.obj" \
	"$(INTDIR)/slidctrl.obj" \
	"$(INTDIR)/mtreectl.obj" \
	"$(INTDIR)/treecpg.obj" \
	"$(INTDIR)/ctrldemo.obj" \
	"$(INTDIR)/propsht.obj" \
	"$(INTDIR)/progctrl.obj" \
	"$(INTDIR)/animctrl.obj" \
	"$(INTDIR)/cmnctrls.res"

"$(OUTDIR)\cmnctrls.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "cmnctrls - Macintosh Release"

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

ALL : "$(OUTDIR)\cmnctrls.exe" "$(OUTDIR)\cmnctrls.trg"\
 "$(OUTDIR)\avifiles.trg"

CLEAN : 
	-@erase ".\MacRel\cmnctrls.trg"
	-@erase ".\MacRel\cmnctrls.exe"
	-@erase ".\MacRel\propsht.obj"
	-@erase ".\MacRel\cmnctrls.pch"
	-@erase ".\MacRel\progctrl.obj"
	-@erase ".\MacRel\animctrl.obj"
	-@erase ".\MacRel\notifwdw.obj"
	-@erase ".\MacRel\mlistctl.obj"
	-@erase ".\MacRel\toolpage.obj"
	-@erase ".\MacRel\spinctrl.obj"
	-@erase ".\MacRel\listcpg.obj"
	-@erase ".\MacRel\toolbar1.obj"
	-@erase ".\MacRel\toolbar2.obj"
	-@erase ".\MacRel\stdafx.obj"
	-@erase ".\MacRel\slidctrl.obj"
	-@erase ".\MacRel\mtreectl.obj"
	-@erase ".\MacRel\ctrldemo.obj"
	-@erase ".\MacRel\treecpg.obj"
	-@erase ".\MacRel\cmnctrls.rsc"
	-@erase ".\MacRel\cmncmac.rsc"
	-@erase ".\MacRel\avifiles.trg"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

MTL=mktyplib.exe
# ADD BASE MTL /nologo /D "NDEBUG" /mac
# ADD MTL /nologo /D "NDEBUG" /mac
MTL_PROJ=/nologo /D "NDEBUG" /mac 
CPP=cl.exe
# ADD BASE CPP /nologo /AL /Q68s /W3 /GX /O2 /D "_WINDOWS" /D "_MAC" /D "_68K_" /D "WIN32" /D "NDEBUG" /YX /c
# ADD CPP /nologo /AL /Q68s /W3 /GX /O2 /D "_WINDOWS" /D "_MAC" /D "_68K_" /D "WIN32" /D "NDEBUG" /D "_MBCS" /Yu"stdafx.h" /c
CPP_PROJ=/nologo /AL /Q68s /W3 /GX /O2 /D "_WINDOWS" /D "_MAC" /D "_68K_" /D\
 "WIN32" /D "NDEBUG" /D "_MBCS" /Fp"$(INTDIR)/cmnctrls.pch" /Yu"stdafx.h"\
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
RSC_PROJ=/l 0x409 /r /m /fo"$(INTDIR)/cmnctrls.rsc" /d "_MAC" /d "_68K_" /d\
 "NDEBUG" 
MRC=mrc.exe
# ADD BASE MRC /D "_68K_" /D "_MAC" /D "NDEBUG" /NOLOGO
# ADD MRC /D "_68K_" /D "_MAC" /D "NDEBUG" /NOLOGO
MRC_PROJ=/D "_68K_" /D "_MAC" /D "NDEBUG" /l 0x409 /NOLOGO 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/cmnctrls.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 /nologo /MAC:bundle /machine:M68K
# ADD LINK32 /nologo /MAC:bundle /machine:M68K
LINK32_FLAGS=/nologo /MAC:bundle /MAC:type="APPL" /MAC:creator="????"\
 /pdb:"$(OUTDIR)/cmnctrls.pdb" /machine:M68K /out:"$(OUTDIR)/cmnctrls.exe" 
LINK32_OBJS= \
	"$(INTDIR)/propsht.obj" \
	"$(INTDIR)/progctrl.obj" \
	"$(INTDIR)/animctrl.obj" \
	"$(INTDIR)/notifwdw.obj" \
	"$(INTDIR)/mlistctl.obj" \
	"$(INTDIR)/toolpage.obj" \
	"$(INTDIR)/spinctrl.obj" \
	"$(INTDIR)/listcpg.obj" \
	"$(INTDIR)/toolbar1.obj" \
	"$(INTDIR)/toolbar2.obj" \
	"$(INTDIR)/stdafx.obj" \
	"$(INTDIR)/slidctrl.obj" \
	"$(INTDIR)/mtreectl.obj" \
	"$(INTDIR)/ctrldemo.obj" \
	"$(INTDIR)/treecpg.obj" \
	"$(INTDIR)/cmnctrls.rsc" \
	"$(INTDIR)/cmncmac.rsc"

"$(OUTDIR)\cmnctrls.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

MFILE32=mfile.exe
# ADD BASE MFILE32 COPY /NOLOGO
# ADD MFILE32 COPY /NOLOGO
MFILE32_FLAGS=COPY /NOLOGO 
MFILE32_FILES= \
	"$(OUTDIR)/cmnctrls.exe"

"$(OUTDIR)\cmnctrls.trg" : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\MacRel\cmnctrls.exe\
 "$(MFILE32_DEST):cmnctrls.exe">"$(OUTDIR)\cmnctrls.trg"

DOWNLOAD : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\MacRel\cmnctrls.exe\
 "$(MFILE32_DEST):cmnctrls.exe">"$(OUTDIR)\cmnctrls.trg"
# Begin Custom Build - Copying AVI files for animation control
OutDir=.\MacRel
RemoteDir=
InputPath=.\MacRel\cmnctrls.exe
SOURCE=$(InputPath)

"$(OutDir)\avifiles.trg" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   mfile copy /nologo /v /t "AVI " *.avi ":$(RemoteDir):"
   echo >$(OutDir)\avifiles.trg

# End Custom Build

!ELSEIF  "$(CFG)" == "cmnctrls - Macintosh Debug"

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

ALL : "$(OUTDIR)\cmnctrls.exe" "$(OUTDIR)\cmnctrls.trg"\
 "$(OUTDIR)\avifiles.trg"

CLEAN : 
	-@erase ".\MacDbg\vc40.pdb"
	-@erase ".\MacDbg\cmnctrls.pch"
	-@erase ".\MacDbg\cmnctrls.pdb"
	-@erase ".\MacDbg\progctrl.obj"
	-@erase ".\MacDbg\animctrl.obj"
	-@erase ".\MacDbg\treecpg.obj"
	-@erase ".\MacDbg\toolpage.obj"
	-@erase ".\MacDbg\propsht.obj"
	-@erase ".\MacDbg\notifwdw.obj"
	-@erase ".\MacDbg\mlistctl.obj"
	-@erase ".\MacDbg\toolbar1.obj"
	-@erase ".\MacDbg\spinctrl.obj"
	-@erase ".\MacDbg\toolbar2.obj"
	-@erase ".\MacDbg\stdafx.obj"
	-@erase ".\MacDbg\listcpg.obj"
	-@erase ".\MacDbg\slidctrl.obj"
	-@erase ".\MacDbg\mtreectl.obj"
	-@erase ".\MacDbg\ctrldemo.obj"
	-@erase ".\MacDbg\cmnctrls.rsc"
	-@erase ".\MacDbg\cmncmac.rsc"
	-@erase ".\MacDbg\cmnctrls.trg"
	-@erase ".\MacDbg\cmnctrls.exe"
	-@erase ".\MacDbg\avifiles.trg"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

MTL=mktyplib.exe
# ADD BASE MTL /nologo /D "_DEBUG" /mac
# ADD MTL /nologo /D "_DEBUG" /mac
MTL_PROJ=/nologo /D "_DEBUG" /mac 
CPP=cl.exe
# ADD BASE CPP /nologo /AL /Q68s /Q68m /W3 /GX /Zi /Od /D "_WINDOWS" /D "_MAC" /D "_68K_" /D "WIN32" /D "_DEBUG" /YX /c
# ADD CPP /nologo /AL /Q68s /Q68m /W3 /GX /Zi /Od /D "_WINDOWS" /D "_MAC" /D "_68K_" /D "WIN32" /D "_DEBUG" /D "_MBCS" /Yu"stdafx.h" /c
CPP_PROJ=/nologo /AL /Q68s /Q68m /W3 /GX /Zi /Od /D "_WINDOWS" /D "_MAC" /D\
 "_68K_" /D "WIN32" /D "_DEBUG" /D "_MBCS" /Fp"$(INTDIR)/cmnctrls.pch"\
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
RSC_PROJ=/l 0x409 /r /m /fo"$(INTDIR)/cmnctrls.rsc" /d "_MAC" /d "_68K_" /d\
 "_DEBUG" 
MRC=mrc.exe
# ADD BASE MRC /D "_68K_" /D "_MAC" /D "_DEBUG" /NOLOGO
# ADD MRC /D "_68K_" /D "_MAC" /D "_DEBUG" /NOLOGO
MRC_PROJ=/D "_68K_" /D "_MAC" /D "_DEBUG" /l 0x409 /NOLOGO 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/cmnctrls.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 /nologo /MAC:bundle /debug /machine:M68K
# ADD LINK32 /nologo /MAC:bundle /debug /machine:M68K
LINK32_FLAGS=/nologo /MAC:bundle /MAC:type="APPL" /MAC:creator="????"\
 /pdb:"$(OUTDIR)/cmnctrls.pdb" /debug /machine:M68K\
 /out:"$(OUTDIR)/cmnctrls.exe" 
LINK32_OBJS= \
	"$(INTDIR)/progctrl.obj" \
	"$(INTDIR)/animctrl.obj" \
	"$(INTDIR)/treecpg.obj" \
	"$(INTDIR)/toolpage.obj" \
	"$(INTDIR)/propsht.obj" \
	"$(INTDIR)/notifwdw.obj" \
	"$(INTDIR)/mlistctl.obj" \
	"$(INTDIR)/toolbar1.obj" \
	"$(INTDIR)/spinctrl.obj" \
	"$(INTDIR)/toolbar2.obj" \
	"$(INTDIR)/stdafx.obj" \
	"$(INTDIR)/listcpg.obj" \
	"$(INTDIR)/slidctrl.obj" \
	"$(INTDIR)/mtreectl.obj" \
	"$(INTDIR)/ctrldemo.obj" \
	"$(INTDIR)/cmnctrls.rsc" \
	"$(INTDIR)/cmncmac.rsc"

"$(OUTDIR)\cmnctrls.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

MFILE32=mfile.exe
# ADD BASE MFILE32 COPY /NOLOGO
# ADD MFILE32 COPY /NOLOGO
MFILE32_FLAGS=COPY /NOLOGO 
MFILE32_FILES= \
	"$(OUTDIR)/cmnctrls.exe"

"$(OUTDIR)\cmnctrls.trg" : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\MacDbg\cmnctrls.exe\
 "$(MFILE32_DEST):cmnctrls.exe">"$(OUTDIR)\cmnctrls.trg"

DOWNLOAD : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\MacDbg\cmnctrls.exe\
 "$(MFILE32_DEST):cmnctrls.exe">"$(OUTDIR)\cmnctrls.trg"
# Begin Custom Build - Copying AVI files for animation control
OutDir=.\MacDbg
RemoteDir=
InputPath=.\MacDbg\cmnctrls.exe
SOURCE=$(InputPath)

"$(OutDir)\avifiles.trg" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   mfile copy /nologo /v /t "AVI " *.avi ":$(RemoteDir):"
   echo >$(OutDir)\avifiles.trg

# End Custom Build

!ELSEIF  "$(CFG)" == "cmnctrls - Power Macintosh Release"

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

ALL : "$(OUTDIR)\cmnctrls.exe" "$(OUTDIR)\cmnctrls.trg"\
 "$(OUTDIR)\avifiles.trg"

CLEAN : 
	-@erase ".\PMacRel\cmnctrls.trg"
	-@erase ".\PMacRel\cmnctrls.exe"
	-@erase ".\PMacRel\toolbar2.obj"
	-@erase ".\PMacRel\cmnctrls.pch"
	-@erase ".\PMacRel\propsht.obj"
	-@erase ".\PMacRel\stdafx.obj"
	-@erase ".\PMacRel\slidctrl.obj"
	-@erase ".\PMacRel\mtreectl.obj"
	-@erase ".\PMacRel\ctrldemo.obj"
	-@erase ".\PMacRel\listcpg.obj"
	-@erase ".\PMacRel\progctrl.obj"
	-@erase ".\PMacRel\animctrl.obj"
	-@erase ".\PMacRel\notifwdw.obj"
	-@erase ".\PMacRel\mlistctl.obj"
	-@erase ".\PMacRel\toolpage.obj"
	-@erase ".\PMacRel\treecpg.obj"
	-@erase ".\PMacRel\toolbar1.obj"
	-@erase ".\PMacRel\spinctrl.obj"
	-@erase ".\PMacRel\cmncmac.rsc"
	-@erase ".\PMacRel\cmnctrls.rsc"
	-@erase ".\PMacRel\avifiles.trg"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

MTL=mktyplib.exe
# ADD BASE MTL /nologo /D "NDEBUG" /ppc
# ADD MTL /nologo /D "NDEBUG" /ppc
MTL_PROJ=/nologo /D "NDEBUG" /ppc 
CPP=cl.exe
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "_MAC" /D "_MPPC_" /D "_WINDOWS" /D "WIN32" /D "NDEBUG" /YX /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "_MAC" /D "_MPPC_" /D "_WINDOWS" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_AFXDLL" /Yu"stdafx.h" /c
CPP_PROJ=/nologo /MD /W3 /GX /O2 /D "_MAC" /D "_MPPC_" /D "_WINDOWS" /D "WIN32"\
 /D "NDEBUG" /D "_MBCS" /D "_AFXDLL" /Fp"$(INTDIR)/cmnctrls.pch" /Yu"stdafx.h"\
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
RSC_PROJ=/l 0x409 /r /m /fo"$(INTDIR)/cmnctrls.rsc" /d "_MAC" /d "_MPPC_" /d\
 "NDEBUG" 
MRC=mrc.exe
# ADD BASE MRC /D "_MPPC_" /D "_MAC" /D "NDEBUG" /NOLOGO
# ADD MRC /D "_MPPC_" /D "_MAC" /D "NDEBUG" /NOLOGO
MRC_PROJ=/D "_MPPC_" /D "_MAC" /D "NDEBUG" /l 0x409 /NOLOGO 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/cmnctrls.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 cfrg.rsc /nologo /mac:bundle /machine:MPPC
# ADD LINK32 /nologo /mac:bundle /mac:creator="CMNC" /machine:MPPC
LINK32_FLAGS=/nologo /mac:bundle /mac:type="APPL" /mac:creator="CMNC"\
 /incremental:no /pdb:"$(OUTDIR)/cmnctrls.pdb" /machine:MPPC\
 /out:"$(OUTDIR)/cmnctrls.exe" 
LINK32_OBJS= \
	"$(INTDIR)/toolbar2.obj" \
	"$(INTDIR)/propsht.obj" \
	"$(INTDIR)/stdafx.obj" \
	"$(INTDIR)/slidctrl.obj" \
	"$(INTDIR)/mtreectl.obj" \
	"$(INTDIR)/ctrldemo.obj" \
	"$(INTDIR)/listcpg.obj" \
	"$(INTDIR)/progctrl.obj" \
	"$(INTDIR)/animctrl.obj" \
	"$(INTDIR)/notifwdw.obj" \
	"$(INTDIR)/mlistctl.obj" \
	"$(INTDIR)/toolpage.obj" \
	"$(INTDIR)/treecpg.obj" \
	"$(INTDIR)/toolbar1.obj" \
	"$(INTDIR)/spinctrl.obj" \
	"$(INTDIR)/cmncmac.rsc" \
	"$(INTDIR)/cmnctrls.rsc"

"$(OUTDIR)\cmnctrls.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

MFILE32=mfile.exe
# ADD BASE MFILE32 COPY /NOLOGO
# ADD MFILE32 COPY /NOLOGO
MFILE32_FLAGS=COPY /NOLOGO 
MFILE32_FILES= \
	"$(OUTDIR)/cmnctrls.exe"

"$(OUTDIR)\cmnctrls.trg" : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\PMacRel\cmnctrls.exe\
 "$(MFILE32_DEST):cmnctrls.exe">"$(OUTDIR)\cmnctrls.trg"

DOWNLOAD : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\PMacRel\cmnctrls.exe\
 "$(MFILE32_DEST):cmnctrls.exe">"$(OUTDIR)\cmnctrls.trg"
# Begin Custom Build - Copying AVI files for animation control
OutDir=.\PMacRel
RemoteDir=
InputPath=.\PMacRel\cmnctrls.exe
SOURCE=$(InputPath)

"$(OutDir)\avifiles.trg" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   mfile copy /nologo /v /t "AVI " *.avi ":$(RemoteDir):"
   echo >$(OutDir)\avifiles.trg

# End Custom Build

!ELSEIF  "$(CFG)" == "cmnctrls - Power Macintosh Debug"

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

ALL : "$(OUTDIR)\cmnctrls.exe" "$(OUTDIR)\cmnctrls.trg"\
 "$(OUTDIR)\avifiles.trg"

CLEAN : 
	-@erase ".\PMacDbg\vc40.pdb"
	-@erase ".\PMacDbg\cmnctrls.pch"
	-@erase ".\PMacDbg\cmnctrls.ilk"
	-@erase ".\PMacDbg\toolbar1.obj"
	-@erase ".\PMacDbg\toolbar2.obj"
	-@erase ".\PMacDbg\stdafx.obj"
	-@erase ".\PMacDbg\treecpg.obj"
	-@erase ".\PMacDbg\slidctrl.obj"
	-@erase ".\PMacDbg\propsht.obj"
	-@erase ".\PMacDbg\mtreectl.obj"
	-@erase ".\PMacDbg\ctrldemo.obj"
	-@erase ".\PMacDbg\progctrl.obj"
	-@erase ".\PMacDbg\listcpg.obj"
	-@erase ".\PMacDbg\animctrl.obj"
	-@erase ".\PMacDbg\notifwdw.obj"
	-@erase ".\PMacDbg\toolpage.obj"
	-@erase ".\PMacDbg\mlistctl.obj"
	-@erase ".\PMacDbg\spinctrl.obj"
	-@erase ".\PMacDbg\cmncmac.rsc"
	-@erase ".\PMacDbg\cmnctrls.rsc"
	-@erase ".\PMacDbg\cmnctrls.pdb"
	-@erase ".\PMacDbg\cmnctrls.trg"
	-@erase ".\PMacDbg\cmnctrls.exe"
	-@erase ".\PMacDbg\avifiles.trg"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

MTL=mktyplib.exe
# ADD BASE MTL /nologo /D "_DEBUG" /ppc
# ADD MTL /nologo /D "_DEBUG" /ppc
MTL_PROJ=/nologo /D "_DEBUG" /ppc 
CPP=cl.exe
# ADD BASE CPP /nologo /MDd /W3 /GX /Zi /Od /D "_MAC" /D "_MPPC_" /D "_WINDOWS" /D "WIN32" /D "_DEBUG" /YX /QPm /c
# ADD CPP /nologo /MDd /W3 /GX /Zi /Od /D "_MAC" /D "_MPPC_" /D "_WINDOWS" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_AFXDLL" /Yu"stdafx.h" /QPm /c
CPP_PROJ=/nologo /MDd /W3 /GX /Zi /Od /D "_MAC" /D "_MPPC_" /D "_WINDOWS" /D\
 "WIN32" /D "_DEBUG" /D "_MBCS" /D "_AFXDLL" /Fp"$(INTDIR)/cmnctrls.pch"\
 /Yu"stdafx.h" /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /QPm /c 
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
RSC_PROJ=/l 0x409 /r /m /fo"$(INTDIR)/cmnctrls.rsc" /d "_MAC" /d "_MPPC_" /d\
 "_DEBUG" 
MRC=mrc.exe
# ADD BASE MRC /D "_MPPC_" /D "_MAC" /D "_DEBUG" /NOLOGO
# ADD MRC /D "_MPPC_" /D "_MAC" /D "_DEBUG" /NOLOGO
MRC_PROJ=/D "_MPPC_" /D "_MAC" /D "_DEBUG" /l 0x409 /NOLOGO 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/cmnctrls.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 cfrg.rsc /nologo /mac:bundle /debug /machine:MPPC
# ADD LINK32 /nologo /mac:bundle /mac:creator="CMNC" /debug /machine:MPPC
LINK32_FLAGS=/nologo /mac:bundle /mac:type="APPL" /mac:creator="CMNC"\
 /incremental:yes /pdb:"$(OUTDIR)/cmnctrls.pdb" /debug /machine:MPPC\
 /out:"$(OUTDIR)/cmnctrls.exe" 
LINK32_OBJS= \
	"$(INTDIR)/toolbar1.obj" \
	"$(INTDIR)/toolbar2.obj" \
	"$(INTDIR)/stdafx.obj" \
	"$(INTDIR)/treecpg.obj" \
	"$(INTDIR)/slidctrl.obj" \
	"$(INTDIR)/propsht.obj" \
	"$(INTDIR)/mtreectl.obj" \
	"$(INTDIR)/ctrldemo.obj" \
	"$(INTDIR)/progctrl.obj" \
	"$(INTDIR)/listcpg.obj" \
	"$(INTDIR)/animctrl.obj" \
	"$(INTDIR)/notifwdw.obj" \
	"$(INTDIR)/toolpage.obj" \
	"$(INTDIR)/mlistctl.obj" \
	"$(INTDIR)/spinctrl.obj" \
	"$(INTDIR)/cmncmac.rsc" \
	"$(INTDIR)/cmnctrls.rsc"

"$(OUTDIR)\cmnctrls.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

MFILE32=mfile.exe
# ADD BASE MFILE32 COPY /NOLOGO
# ADD MFILE32 COPY /NOLOGO
MFILE32_FLAGS=COPY /NOLOGO 
MFILE32_FILES= \
	"$(OUTDIR)/cmnctrls.exe"

"$(OUTDIR)\cmnctrls.trg" : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\PMacDbg\cmnctrls.exe\
 "$(MFILE32_DEST):cmnctrls.exe">"$(OUTDIR)\cmnctrls.trg"

DOWNLOAD : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\PMacDbg\cmnctrls.exe\
 "$(MFILE32_DEST):cmnctrls.exe">"$(OUTDIR)\cmnctrls.trg"
# Begin Custom Build - Copying AVI files for animation control
OutDir=.\PMacDbg
RemoteDir=
InputPath=.\PMacDbg\cmnctrls.exe
SOURCE=$(InputPath)

"$(OutDir)\avifiles.trg" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   mfile copy /nologo /v /t "AVI " *.avi ":$(RemoteDir):"
   echo >$(OutDir)\avifiles.trg

# End Custom Build

!ELSEIF  "$(CFG)" == "cmnctrls - Win32 Unicode Debug"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir UniDebug
# PROP BASE Intermediate_Dir UniDebug
# PROP BASE Target_Dir
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 1
# PROP Output_Dir UniDebug
# PROP Intermediate_Dir UniDebug
# PROP Target_Dir
OUTDIR=.\UniDebug
INTDIR=.\UniDebug

ALL : "$(OUTDIR)\cmnctrls.exe"

CLEAN : 
	-@erase ".\UniDebug\vc40.pdb"
	-@erase ".\UniDebug\cmnctrls.pch"
	-@erase ".\UniDebug\cmnctrls.exe"
	-@erase ".\UniDebug\progctrl.obj"
	-@erase ".\UniDebug\listcpg.obj"
	-@erase ".\UniDebug\animctrl.obj"
	-@erase ".\UniDebug\notifwdw.obj"
	-@erase ".\UniDebug\mlistctl.obj"
	-@erase ".\UniDebug\toolpage.obj"
	-@erase ".\UniDebug\spinctrl.obj"
	-@erase ".\UniDebug\toolbar1.obj"
	-@erase ".\UniDebug\toolbar2.obj"
	-@erase ".\UniDebug\treecpg.obj"
	-@erase ".\UniDebug\slidctrl.obj"
	-@erase ".\UniDebug\mtreectl.obj"
	-@erase ".\UniDebug\propsht.obj"
	-@erase ".\UniDebug\stdafx.obj"
	-@erase ".\UniDebug\ctrldemo.obj"
	-@erase ".\UniDebug\cmnctrls.res"
	-@erase ".\UniDebug\cmnctrls.ilk"
	-@erase ".\UniDebug\cmnctrls.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /YX /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /D "_UNICODE" /Yu"stdafx.h" /c
CPP_PROJ=/nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /D "_AFXDLL" /D "_MBCS" /D "_UNICODE" /Fp"$(INTDIR)/cmnctrls.pch" /Yu"stdafx.h"\
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
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/cmnctrls.res" /d "_DEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/cmnctrls.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386
# ADD LINK32 /nologo /entry:"wWinMainCRTStartup" /subsystem:windows /debug /machine:I386
LINK32_FLAGS=/nologo /entry:"wWinMainCRTStartup" /subsystem:windows\
 /incremental:yes /pdb:"$(OUTDIR)/cmnctrls.pdb" /debug /machine:I386\
 /out:"$(OUTDIR)/cmnctrls.exe" 
LINK32_OBJS= \
	"$(INTDIR)/progctrl.obj" \
	"$(INTDIR)/listcpg.obj" \
	"$(INTDIR)/animctrl.obj" \
	"$(INTDIR)/notifwdw.obj" \
	"$(INTDIR)/mlistctl.obj" \
	"$(INTDIR)/toolpage.obj" \
	"$(INTDIR)/spinctrl.obj" \
	"$(INTDIR)/toolbar1.obj" \
	"$(INTDIR)/toolbar2.obj" \
	"$(INTDIR)/treecpg.obj" \
	"$(INTDIR)/slidctrl.obj" \
	"$(INTDIR)/mtreectl.obj" \
	"$(INTDIR)/propsht.obj" \
	"$(INTDIR)/stdafx.obj" \
	"$(INTDIR)/ctrldemo.obj" \
	"$(INTDIR)/cmnctrls.res"

"$(OUTDIR)\cmnctrls.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "cmnctrls - Win32 Unicode Release"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir UniRelease
# PROP BASE Intermediate_Dir UniRelease
# PROP BASE Target_Dir
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 0
# PROP Output_Dir UniRelease
# PROP Intermediate_Dir UniRelease
# PROP Target_Dir
OUTDIR=.\UniRelease
INTDIR=.\UniRelease

ALL : "$(OUTDIR)\cmnctrls.exe"

CLEAN : 
	-@erase ".\UniRelease\cmnctrls.exe"
	-@erase ".\UniRelease\animctrl.obj"
	-@erase ".\UniRelease\cmnctrls.pch"
	-@erase ".\UniRelease\notifwdw.obj"
	-@erase ".\UniRelease\toolpage.obj"
	-@erase ".\UniRelease\mlistctl.obj"
	-@erase ".\UniRelease\listcpg.obj"
	-@erase ".\UniRelease\spinctrl.obj"
	-@erase ".\UniRelease\toolbar1.obj"
	-@erase ".\UniRelease\toolbar2.obj"
	-@erase ".\UniRelease\slidctrl.obj"
	-@erase ".\UniRelease\treecpg.obj"
	-@erase ".\UniRelease\mtreectl.obj"
	-@erase ".\UniRelease\ctrldemo.obj"
	-@erase ".\UniRelease\propsht.obj"
	-@erase ".\UniRelease\stdafx.obj"
	-@erase ".\UniRelease\progctrl.obj"
	-@erase ".\UniRelease\cmnctrls.res"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /YX /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /D "_UNICODE" /Yu"stdafx.h" /c
CPP_PROJ=/nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /D "_UNICODE" /Fp"$(INTDIR)/cmnctrls.pch" /Yu"stdafx.h"\
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
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/cmnctrls.res" /d "NDEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/cmnctrls.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 /nologo /entry:"wWinMainCRTStartup" /subsystem:windows /machine:I386
LINK32_FLAGS=/nologo /entry:"wWinMainCRTStartup" /subsystem:windows\
 /incremental:no /pdb:"$(OUTDIR)/cmnctrls.pdb" /machine:I386\
 /out:"$(OUTDIR)/cmnctrls.exe" 
LINK32_OBJS= \
	"$(INTDIR)/animctrl.obj" \
	"$(INTDIR)/notifwdw.obj" \
	"$(INTDIR)/toolpage.obj" \
	"$(INTDIR)/mlistctl.obj" \
	"$(INTDIR)/listcpg.obj" \
	"$(INTDIR)/spinctrl.obj" \
	"$(INTDIR)/toolbar1.obj" \
	"$(INTDIR)/toolbar2.obj" \
	"$(INTDIR)/slidctrl.obj" \
	"$(INTDIR)/treecpg.obj" \
	"$(INTDIR)/mtreectl.obj" \
	"$(INTDIR)/ctrldemo.obj" \
	"$(INTDIR)/propsht.obj" \
	"$(INTDIR)/stdafx.obj" \
	"$(INTDIR)/progctrl.obj" \
	"$(INTDIR)/cmnctrls.res"

"$(OUTDIR)\cmnctrls.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 

################################################################################
# Begin Target

# Name "cmnctrls - Win32 Release"
# Name "cmnctrls - Win32 Debug"
# Name "cmnctrls - Macintosh Release"
# Name "cmnctrls - Macintosh Debug"
# Name "cmnctrls - Power Macintosh Release"
# Name "cmnctrls - Power Macintosh Debug"
# Name "cmnctrls - Win32 Unicode Debug"
# Name "cmnctrls - Win32 Unicode Release"

!IF  "$(CFG)" == "cmnctrls - Win32 Release"

!ELSEIF  "$(CFG)" == "cmnctrls - Win32 Debug"

!ELSEIF  "$(CFG)" == "cmnctrls - Macintosh Release"

!ELSEIF  "$(CFG)" == "cmnctrls - Macintosh Debug"

!ELSEIF  "$(CFG)" == "cmnctrls - Power Macintosh Release"

!ELSEIF  "$(CFG)" == "cmnctrls - Power Macintosh Debug"

!ELSEIF  "$(CFG)" == "cmnctrls - Win32 Unicode Debug"

!ELSEIF  "$(CFG)" == "cmnctrls - Win32 Unicode Release"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\cmnctrls.rc
DEP_RSC_CMNCT=\
	".\res\ctrldemo.ico"\
	".\res\icon1.ico"\
	".\res\icon2.ico"\
	".\res\icon3.ico"\
	".\res\icon4.ico"\
	".\res\bmp00001.bmp"\
	".\res\bmtreebi.bmp"\
	".\res\bmtreedo.bmp"\
	".\res\bmp00002.bmp"\
	".\res\bmtreefi.bmp"\
	".\res\bmp00003.bmp"\
	".\res\toolbar.bmp"\
	".\res\cpalette.bmp"\
	".\res\standard.bmp"\
	".\res\ctrldemo.rc2"\
	

!IF  "$(CFG)" == "cmnctrls - Win32 Release"


"$(INTDIR)\cmnctrls.res" : $(SOURCE) $(DEP_RSC_CMNCT) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "cmnctrls - Win32 Debug"


"$(INTDIR)\cmnctrls.res" : $(SOURCE) $(DEP_RSC_CMNCT) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "cmnctrls - Macintosh Release"


"$(INTDIR)\cmnctrls.rsc" : $(SOURCE) $(DEP_RSC_CMNCT) "$(INTDIR)"
   $(RSC) /l 0x409 /r /m /fo"$(INTDIR)/cmnctrls.rsc" /d "_MAC" /d "_68K_" /d\
 "NDEBUG" $(SOURCE)


!ELSEIF  "$(CFG)" == "cmnctrls - Macintosh Debug"


"$(INTDIR)\cmnctrls.rsc" : $(SOURCE) $(DEP_RSC_CMNCT) "$(INTDIR)"
   $(RSC) /l 0x409 /r /m /fo"$(INTDIR)/cmnctrls.rsc" /d "_MAC" /d "_68K_" /d\
 "_DEBUG" $(SOURCE)


!ELSEIF  "$(CFG)" == "cmnctrls - Power Macintosh Release"


"$(INTDIR)\cmnctrls.rsc" : $(SOURCE) $(DEP_RSC_CMNCT) "$(INTDIR)"
   $(RSC) /l 0x409 /r /m /fo"$(INTDIR)/cmnctrls.rsc" /d "_MAC" /d "_MPPC_" /d\
 "NDEBUG" $(SOURCE)


!ELSEIF  "$(CFG)" == "cmnctrls - Power Macintosh Debug"


"$(INTDIR)\cmnctrls.rsc" : $(SOURCE) $(DEP_RSC_CMNCT) "$(INTDIR)"
   $(RSC) /l 0x409 /r /m /fo"$(INTDIR)/cmnctrls.rsc" /d "_MAC" /d "_MPPC_" /d\
 "_DEBUG" $(SOURCE)


!ELSEIF  "$(CFG)" == "cmnctrls - Win32 Unicode Debug"


"$(INTDIR)\cmnctrls.res" : $(SOURCE) $(DEP_RSC_CMNCT) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "cmnctrls - Win32 Unicode Release"


"$(INTDIR)\cmnctrls.res" : $(SOURCE) $(DEP_RSC_CMNCT) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\animctrl.cpp
DEP_CPP_ANIMC=\
	".\animctrl.h"\
	".\ctrldemo.h"\
	".\stdafx.h"\
	

!IF  "$(CFG)" == "cmnctrls - Win32 Release"


"$(INTDIR)\animctrl.obj" : $(SOURCE) $(DEP_CPP_ANIMC) "$(INTDIR)"\
 "$(INTDIR)\cmnctrls.pch"


!ELSEIF  "$(CFG)" == "cmnctrls - Win32 Debug"


"$(INTDIR)\animctrl.obj" : $(SOURCE) $(DEP_CPP_ANIMC) "$(INTDIR)"\
 "$(INTDIR)\cmnctrls.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_ANIMC) "$(INTDIR)"\
 "$(INTDIR)\cmnctrls.pch"


!ELSEIF  "$(CFG)" == "cmnctrls - Macintosh Release"


"$(INTDIR)\animctrl.obj" : $(SOURCE) $(DEP_CPP_ANIMC) "$(INTDIR)"\
 "$(INTDIR)\cmnctrls.pch"


!ELSEIF  "$(CFG)" == "cmnctrls - Macintosh Debug"


"$(INTDIR)\animctrl.obj" : $(SOURCE) $(DEP_CPP_ANIMC) "$(INTDIR)"\
 "$(INTDIR)\cmnctrls.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_ANIMC) "$(INTDIR)"\
 "$(INTDIR)\cmnctrls.pch"


!ELSEIF  "$(CFG)" == "cmnctrls - Power Macintosh Release"


"$(INTDIR)\animctrl.obj" : $(SOURCE) $(DEP_CPP_ANIMC) "$(INTDIR)"\
 "$(INTDIR)\cmnctrls.pch"


!ELSEIF  "$(CFG)" == "cmnctrls - Power Macintosh Debug"


"$(INTDIR)\animctrl.obj" : $(SOURCE) $(DEP_CPP_ANIMC) "$(INTDIR)"\
 "$(INTDIR)\cmnctrls.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_ANIMC) "$(INTDIR)"\
 "$(INTDIR)\cmnctrls.pch"


!ELSEIF  "$(CFG)" == "cmnctrls - Win32 Unicode Debug"


"$(INTDIR)\animctrl.obj" : $(SOURCE) $(DEP_CPP_ANIMC) "$(INTDIR)"\
 "$(INTDIR)\cmnctrls.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_ANIMC) "$(INTDIR)"\
 "$(INTDIR)\cmnctrls.pch"


!ELSEIF  "$(CFG)" == "cmnctrls - Win32 Unicode Release"


"$(INTDIR)\animctrl.obj" : $(SOURCE) $(DEP_CPP_ANIMC) "$(INTDIR)"\
 "$(INTDIR)\cmnctrls.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\ctrldemo.cpp
DEP_CPP_CTRLD=\
	".\ctrldemo.h"\
	".\propsht.h"\
	".\animctrl.h"\
	".\listcpg.h"\
	".\mlistctl.h"\
	".\notifwdw.h"\
	".\progctrl.h"\
	".\slidctrl.h"\
	".\spinctrl.h"\
	".\toolpage.h"\
	".\toolbar1.h"\
	".\toolbar2.h"\
	".\treecpg.h"\
	".\mtreectl.h"\
	".\stdafx.h"\
	

!IF  "$(CFG)" == "cmnctrls - Win32 Release"


"$(INTDIR)\ctrldemo.obj" : $(SOURCE) $(DEP_CPP_CTRLD) "$(INTDIR)"\
 "$(INTDIR)\cmnctrls.pch"


!ELSEIF  "$(CFG)" == "cmnctrls - Win32 Debug"


"$(INTDIR)\ctrldemo.obj" : $(SOURCE) $(DEP_CPP_CTRLD) "$(INTDIR)"\
 "$(INTDIR)\cmnctrls.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_CTRLD) "$(INTDIR)"\
 "$(INTDIR)\cmnctrls.pch"


!ELSEIF  "$(CFG)" == "cmnctrls - Macintosh Release"


"$(INTDIR)\ctrldemo.obj" : $(SOURCE) $(DEP_CPP_CTRLD) "$(INTDIR)"\
 "$(INTDIR)\cmnctrls.pch"


!ELSEIF  "$(CFG)" == "cmnctrls - Macintosh Debug"


"$(INTDIR)\ctrldemo.obj" : $(SOURCE) $(DEP_CPP_CTRLD) "$(INTDIR)"\
 "$(INTDIR)\cmnctrls.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_CTRLD) "$(INTDIR)"\
 "$(INTDIR)\cmnctrls.pch"


!ELSEIF  "$(CFG)" == "cmnctrls - Power Macintosh Release"


"$(INTDIR)\ctrldemo.obj" : $(SOURCE) $(DEP_CPP_CTRLD) "$(INTDIR)"\
 "$(INTDIR)\cmnctrls.pch"


!ELSEIF  "$(CFG)" == "cmnctrls - Power Macintosh Debug"


"$(INTDIR)\ctrldemo.obj" : $(SOURCE) $(DEP_CPP_CTRLD) "$(INTDIR)"\
 "$(INTDIR)\cmnctrls.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_CTRLD) "$(INTDIR)"\
 "$(INTDIR)\cmnctrls.pch"


!ELSEIF  "$(CFG)" == "cmnctrls - Win32 Unicode Debug"


"$(INTDIR)\ctrldemo.obj" : $(SOURCE) $(DEP_CPP_CTRLD) "$(INTDIR)"\
 "$(INTDIR)\cmnctrls.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_CTRLD) "$(INTDIR)"\
 "$(INTDIR)\cmnctrls.pch"


!ELSEIF  "$(CFG)" == "cmnctrls - Win32 Unicode Release"


"$(INTDIR)\ctrldemo.obj" : $(SOURCE) $(DEP_CPP_CTRLD) "$(INTDIR)"\
 "$(INTDIR)\cmnctrls.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\listcpg.cpp
DEP_CPP_LISTC=\
	".\ctrldemo.h"\
	".\listcpg.h"\
	".\mlistctl.h"\
	".\notifwdw.h"\
	".\stdafx.h"\
	

!IF  "$(CFG)" == "cmnctrls - Win32 Release"


"$(INTDIR)\listcpg.obj" : $(SOURCE) $(DEP_CPP_LISTC) "$(INTDIR)"\
 "$(INTDIR)\cmnctrls.pch"


!ELSEIF  "$(CFG)" == "cmnctrls - Win32 Debug"


"$(INTDIR)\listcpg.obj" : $(SOURCE) $(DEP_CPP_LISTC) "$(INTDIR)"\
 "$(INTDIR)\cmnctrls.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_LISTC) "$(INTDIR)"\
 "$(INTDIR)\cmnctrls.pch"


!ELSEIF  "$(CFG)" == "cmnctrls - Macintosh Release"


"$(INTDIR)\listcpg.obj" : $(SOURCE) $(DEP_CPP_LISTC) "$(INTDIR)"\
 "$(INTDIR)\cmnctrls.pch"


!ELSEIF  "$(CFG)" == "cmnctrls - Macintosh Debug"


"$(INTDIR)\listcpg.obj" : $(SOURCE) $(DEP_CPP_LISTC) "$(INTDIR)"\
 "$(INTDIR)\cmnctrls.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_LISTC) "$(INTDIR)"\
 "$(INTDIR)\cmnctrls.pch"


!ELSEIF  "$(CFG)" == "cmnctrls - Power Macintosh Release"


"$(INTDIR)\listcpg.obj" : $(SOURCE) $(DEP_CPP_LISTC) "$(INTDIR)"\
 "$(INTDIR)\cmnctrls.pch"


!ELSEIF  "$(CFG)" == "cmnctrls - Power Macintosh Debug"


"$(INTDIR)\listcpg.obj" : $(SOURCE) $(DEP_CPP_LISTC) "$(INTDIR)"\
 "$(INTDIR)\cmnctrls.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_LISTC) "$(INTDIR)"\
 "$(INTDIR)\cmnctrls.pch"


!ELSEIF  "$(CFG)" == "cmnctrls - Win32 Unicode Debug"


"$(INTDIR)\listcpg.obj" : $(SOURCE) $(DEP_CPP_LISTC) "$(INTDIR)"\
 "$(INTDIR)\cmnctrls.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_LISTC) "$(INTDIR)"\
 "$(INTDIR)\cmnctrls.pch"


!ELSEIF  "$(CFG)" == "cmnctrls - Win32 Unicode Release"


"$(INTDIR)\listcpg.obj" : $(SOURCE) $(DEP_CPP_LISTC) "$(INTDIR)"\
 "$(INTDIR)\cmnctrls.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\mlistctl.cpp
DEP_CPP_MLIST=\
	".\ctrldemo.h"\
	".\listcpg.h"\
	".\mlistctl.h"\
	".\notifwdw.h"\
	".\stdafx.h"\
	

!IF  "$(CFG)" == "cmnctrls - Win32 Release"


"$(INTDIR)\mlistctl.obj" : $(SOURCE) $(DEP_CPP_MLIST) "$(INTDIR)"\
 "$(INTDIR)\cmnctrls.pch"


!ELSEIF  "$(CFG)" == "cmnctrls - Win32 Debug"


"$(INTDIR)\mlistctl.obj" : $(SOURCE) $(DEP_CPP_MLIST) "$(INTDIR)"\
 "$(INTDIR)\cmnctrls.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_MLIST) "$(INTDIR)"\
 "$(INTDIR)\cmnctrls.pch"


!ELSEIF  "$(CFG)" == "cmnctrls - Macintosh Release"


"$(INTDIR)\mlistctl.obj" : $(SOURCE) $(DEP_CPP_MLIST) "$(INTDIR)"\
 "$(INTDIR)\cmnctrls.pch"


!ELSEIF  "$(CFG)" == "cmnctrls - Macintosh Debug"


"$(INTDIR)\mlistctl.obj" : $(SOURCE) $(DEP_CPP_MLIST) "$(INTDIR)"\
 "$(INTDIR)\cmnctrls.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_MLIST) "$(INTDIR)"\
 "$(INTDIR)\cmnctrls.pch"


!ELSEIF  "$(CFG)" == "cmnctrls - Power Macintosh Release"


"$(INTDIR)\mlistctl.obj" : $(SOURCE) $(DEP_CPP_MLIST) "$(INTDIR)"\
 "$(INTDIR)\cmnctrls.pch"


!ELSEIF  "$(CFG)" == "cmnctrls - Power Macintosh Debug"


"$(INTDIR)\mlistctl.obj" : $(SOURCE) $(DEP_CPP_MLIST) "$(INTDIR)"\
 "$(INTDIR)\cmnctrls.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_MLIST) "$(INTDIR)"\
 "$(INTDIR)\cmnctrls.pch"


!ELSEIF  "$(CFG)" == "cmnctrls - Win32 Unicode Debug"


"$(INTDIR)\mlistctl.obj" : $(SOURCE) $(DEP_CPP_MLIST) "$(INTDIR)"\
 "$(INTDIR)\cmnctrls.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_MLIST) "$(INTDIR)"\
 "$(INTDIR)\cmnctrls.pch"


!ELSEIF  "$(CFG)" == "cmnctrls - Win32 Unicode Release"


"$(INTDIR)\mlistctl.obj" : $(SOURCE) $(DEP_CPP_MLIST) "$(INTDIR)"\
 "$(INTDIR)\cmnctrls.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\mtreectl.cpp
DEP_CPP_MTREE=\
	".\ctrldemo.h"\
	".\mtreectl.h"\
	".\stdafx.h"\
	".\treecpg.h"\
	".\mlistctl.h"\
	".\notifwdw.h"\
	

!IF  "$(CFG)" == "cmnctrls - Win32 Release"


"$(INTDIR)\mtreectl.obj" : $(SOURCE) $(DEP_CPP_MTREE) "$(INTDIR)"\
 "$(INTDIR)\cmnctrls.pch"


!ELSEIF  "$(CFG)" == "cmnctrls - Win32 Debug"


"$(INTDIR)\mtreectl.obj" : $(SOURCE) $(DEP_CPP_MTREE) "$(INTDIR)"\
 "$(INTDIR)\cmnctrls.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_MTREE) "$(INTDIR)"\
 "$(INTDIR)\cmnctrls.pch"


!ELSEIF  "$(CFG)" == "cmnctrls - Macintosh Release"


"$(INTDIR)\mtreectl.obj" : $(SOURCE) $(DEP_CPP_MTREE) "$(INTDIR)"\
 "$(INTDIR)\cmnctrls.pch"


!ELSEIF  "$(CFG)" == "cmnctrls - Macintosh Debug"


"$(INTDIR)\mtreectl.obj" : $(SOURCE) $(DEP_CPP_MTREE) "$(INTDIR)"\
 "$(INTDIR)\cmnctrls.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_MTREE) "$(INTDIR)"\
 "$(INTDIR)\cmnctrls.pch"


!ELSEIF  "$(CFG)" == "cmnctrls - Power Macintosh Release"


"$(INTDIR)\mtreectl.obj" : $(SOURCE) $(DEP_CPP_MTREE) "$(INTDIR)"\
 "$(INTDIR)\cmnctrls.pch"


!ELSEIF  "$(CFG)" == "cmnctrls - Power Macintosh Debug"


"$(INTDIR)\mtreectl.obj" : $(SOURCE) $(DEP_CPP_MTREE) "$(INTDIR)"\
 "$(INTDIR)\cmnctrls.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_MTREE) "$(INTDIR)"\
 "$(INTDIR)\cmnctrls.pch"


!ELSEIF  "$(CFG)" == "cmnctrls - Win32 Unicode Debug"


"$(INTDIR)\mtreectl.obj" : $(SOURCE) $(DEP_CPP_MTREE) "$(INTDIR)"\
 "$(INTDIR)\cmnctrls.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_MTREE) "$(INTDIR)"\
 "$(INTDIR)\cmnctrls.pch"


!ELSEIF  "$(CFG)" == "cmnctrls - Win32 Unicode Release"


"$(INTDIR)\mtreectl.obj" : $(SOURCE) $(DEP_CPP_MTREE) "$(INTDIR)"\
 "$(INTDIR)\cmnctrls.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\notifwdw.cpp
DEP_CPP_NOTIF=\
	".\ctrldemo.h"\
	".\notifwdw.h"\
	".\stdafx.h"\
	

!IF  "$(CFG)" == "cmnctrls - Win32 Release"


"$(INTDIR)\notifwdw.obj" : $(SOURCE) $(DEP_CPP_NOTIF) "$(INTDIR)"\
 "$(INTDIR)\cmnctrls.pch"


!ELSEIF  "$(CFG)" == "cmnctrls - Win32 Debug"


"$(INTDIR)\notifwdw.obj" : $(SOURCE) $(DEP_CPP_NOTIF) "$(INTDIR)"\
 "$(INTDIR)\cmnctrls.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_NOTIF) "$(INTDIR)"\
 "$(INTDIR)\cmnctrls.pch"


!ELSEIF  "$(CFG)" == "cmnctrls - Macintosh Release"


"$(INTDIR)\notifwdw.obj" : $(SOURCE) $(DEP_CPP_NOTIF) "$(INTDIR)"\
 "$(INTDIR)\cmnctrls.pch"


!ELSEIF  "$(CFG)" == "cmnctrls - Macintosh Debug"


"$(INTDIR)\notifwdw.obj" : $(SOURCE) $(DEP_CPP_NOTIF) "$(INTDIR)"\
 "$(INTDIR)\cmnctrls.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_NOTIF) "$(INTDIR)"\
 "$(INTDIR)\cmnctrls.pch"


!ELSEIF  "$(CFG)" == "cmnctrls - Power Macintosh Release"


"$(INTDIR)\notifwdw.obj" : $(SOURCE) $(DEP_CPP_NOTIF) "$(INTDIR)"\
 "$(INTDIR)\cmnctrls.pch"


!ELSEIF  "$(CFG)" == "cmnctrls - Power Macintosh Debug"


"$(INTDIR)\notifwdw.obj" : $(SOURCE) $(DEP_CPP_NOTIF) "$(INTDIR)"\
 "$(INTDIR)\cmnctrls.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_NOTIF) "$(INTDIR)"\
 "$(INTDIR)\cmnctrls.pch"


!ELSEIF  "$(CFG)" == "cmnctrls - Win32 Unicode Debug"


"$(INTDIR)\notifwdw.obj" : $(SOURCE) $(DEP_CPP_NOTIF) "$(INTDIR)"\
 "$(INTDIR)\cmnctrls.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_NOTIF) "$(INTDIR)"\
 "$(INTDIR)\cmnctrls.pch"


!ELSEIF  "$(CFG)" == "cmnctrls - Win32 Unicode Release"


"$(INTDIR)\notifwdw.obj" : $(SOURCE) $(DEP_CPP_NOTIF) "$(INTDIR)"\
 "$(INTDIR)\cmnctrls.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\progctrl.cpp
DEP_CPP_PROGC=\
	".\progctrl.h"\
	".\ctrldemo.h"\
	".\stdafx.h"\
	

!IF  "$(CFG)" == "cmnctrls - Win32 Release"


"$(INTDIR)\progctrl.obj" : $(SOURCE) $(DEP_CPP_PROGC) "$(INTDIR)"\
 "$(INTDIR)\cmnctrls.pch"


!ELSEIF  "$(CFG)" == "cmnctrls - Win32 Debug"


"$(INTDIR)\progctrl.obj" : $(SOURCE) $(DEP_CPP_PROGC) "$(INTDIR)"\
 "$(INTDIR)\cmnctrls.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_PROGC) "$(INTDIR)"\
 "$(INTDIR)\cmnctrls.pch"


!ELSEIF  "$(CFG)" == "cmnctrls - Macintosh Release"


"$(INTDIR)\progctrl.obj" : $(SOURCE) $(DEP_CPP_PROGC) "$(INTDIR)"\
 "$(INTDIR)\cmnctrls.pch"


!ELSEIF  "$(CFG)" == "cmnctrls - Macintosh Debug"


"$(INTDIR)\progctrl.obj" : $(SOURCE) $(DEP_CPP_PROGC) "$(INTDIR)"\
 "$(INTDIR)\cmnctrls.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_PROGC) "$(INTDIR)"\
 "$(INTDIR)\cmnctrls.pch"


!ELSEIF  "$(CFG)" == "cmnctrls - Power Macintosh Release"


"$(INTDIR)\progctrl.obj" : $(SOURCE) $(DEP_CPP_PROGC) "$(INTDIR)"\
 "$(INTDIR)\cmnctrls.pch"


!ELSEIF  "$(CFG)" == "cmnctrls - Power Macintosh Debug"


"$(INTDIR)\progctrl.obj" : $(SOURCE) $(DEP_CPP_PROGC) "$(INTDIR)"\
 "$(INTDIR)\cmnctrls.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_PROGC) "$(INTDIR)"\
 "$(INTDIR)\cmnctrls.pch"


!ELSEIF  "$(CFG)" == "cmnctrls - Win32 Unicode Debug"


"$(INTDIR)\progctrl.obj" : $(SOURCE) $(DEP_CPP_PROGC) "$(INTDIR)"\
 "$(INTDIR)\cmnctrls.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_PROGC) "$(INTDIR)"\
 "$(INTDIR)\cmnctrls.pch"


!ELSEIF  "$(CFG)" == "cmnctrls - Win32 Unicode Release"


"$(INTDIR)\progctrl.obj" : $(SOURCE) $(DEP_CPP_PROGC) "$(INTDIR)"\
 "$(INTDIR)\cmnctrls.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\propsht.cpp
DEP_CPP_PROPS=\
	".\ctrldemo.h"\
	".\mtreectl.h"\
	".\propsht.h"\
	".\animctrl.h"\
	".\listcpg.h"\
	".\mlistctl.h"\
	".\notifwdw.h"\
	".\progctrl.h"\
	".\slidctrl.h"\
	".\spinctrl.h"\
	".\toolpage.h"\
	".\toolbar1.h"\
	".\toolbar2.h"\
	".\treecpg.h"\
	".\stdafx.h"\
	

!IF  "$(CFG)" == "cmnctrls - Win32 Release"


"$(INTDIR)\propsht.obj" : $(SOURCE) $(DEP_CPP_PROPS) "$(INTDIR)"\
 "$(INTDIR)\cmnctrls.pch"


!ELSEIF  "$(CFG)" == "cmnctrls - Win32 Debug"


"$(INTDIR)\propsht.obj" : $(SOURCE) $(DEP_CPP_PROPS) "$(INTDIR)"\
 "$(INTDIR)\cmnctrls.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_PROPS) "$(INTDIR)"\
 "$(INTDIR)\cmnctrls.pch"


!ELSEIF  "$(CFG)" == "cmnctrls - Macintosh Release"


"$(INTDIR)\propsht.obj" : $(SOURCE) $(DEP_CPP_PROPS) "$(INTDIR)"\
 "$(INTDIR)\cmnctrls.pch"


!ELSEIF  "$(CFG)" == "cmnctrls - Macintosh Debug"


"$(INTDIR)\propsht.obj" : $(SOURCE) $(DEP_CPP_PROPS) "$(INTDIR)"\
 "$(INTDIR)\cmnctrls.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_PROPS) "$(INTDIR)"\
 "$(INTDIR)\cmnctrls.pch"


!ELSEIF  "$(CFG)" == "cmnctrls - Power Macintosh Release"


"$(INTDIR)\propsht.obj" : $(SOURCE) $(DEP_CPP_PROPS) "$(INTDIR)"\
 "$(INTDIR)\cmnctrls.pch"


!ELSEIF  "$(CFG)" == "cmnctrls - Power Macintosh Debug"


"$(INTDIR)\propsht.obj" : $(SOURCE) $(DEP_CPP_PROPS) "$(INTDIR)"\
 "$(INTDIR)\cmnctrls.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_PROPS) "$(INTDIR)"\
 "$(INTDIR)\cmnctrls.pch"


!ELSEIF  "$(CFG)" == "cmnctrls - Win32 Unicode Debug"


"$(INTDIR)\propsht.obj" : $(SOURCE) $(DEP_CPP_PROPS) "$(INTDIR)"\
 "$(INTDIR)\cmnctrls.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_PROPS) "$(INTDIR)"\
 "$(INTDIR)\cmnctrls.pch"


!ELSEIF  "$(CFG)" == "cmnctrls - Win32 Unicode Release"


"$(INTDIR)\propsht.obj" : $(SOURCE) $(DEP_CPP_PROPS) "$(INTDIR)"\
 "$(INTDIR)\cmnctrls.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\slidctrl.cpp
DEP_CPP_SLIDC=\
	".\slidctrl.h"\
	".\ctrldemo.h"\
	".\stdafx.h"\
	

!IF  "$(CFG)" == "cmnctrls - Win32 Release"


"$(INTDIR)\slidctrl.obj" : $(SOURCE) $(DEP_CPP_SLIDC) "$(INTDIR)"\
 "$(INTDIR)\cmnctrls.pch"


!ELSEIF  "$(CFG)" == "cmnctrls - Win32 Debug"


"$(INTDIR)\slidctrl.obj" : $(SOURCE) $(DEP_CPP_SLIDC) "$(INTDIR)"\
 "$(INTDIR)\cmnctrls.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_SLIDC) "$(INTDIR)"\
 "$(INTDIR)\cmnctrls.pch"


!ELSEIF  "$(CFG)" == "cmnctrls - Macintosh Release"


"$(INTDIR)\slidctrl.obj" : $(SOURCE) $(DEP_CPP_SLIDC) "$(INTDIR)"\
 "$(INTDIR)\cmnctrls.pch"


!ELSEIF  "$(CFG)" == "cmnctrls - Macintosh Debug"


"$(INTDIR)\slidctrl.obj" : $(SOURCE) $(DEP_CPP_SLIDC) "$(INTDIR)"\
 "$(INTDIR)\cmnctrls.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_SLIDC) "$(INTDIR)"\
 "$(INTDIR)\cmnctrls.pch"


!ELSEIF  "$(CFG)" == "cmnctrls - Power Macintosh Release"


"$(INTDIR)\slidctrl.obj" : $(SOURCE) $(DEP_CPP_SLIDC) "$(INTDIR)"\
 "$(INTDIR)\cmnctrls.pch"


!ELSEIF  "$(CFG)" == "cmnctrls - Power Macintosh Debug"


"$(INTDIR)\slidctrl.obj" : $(SOURCE) $(DEP_CPP_SLIDC) "$(INTDIR)"\
 "$(INTDIR)\cmnctrls.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_SLIDC) "$(INTDIR)"\
 "$(INTDIR)\cmnctrls.pch"


!ELSEIF  "$(CFG)" == "cmnctrls - Win32 Unicode Debug"


"$(INTDIR)\slidctrl.obj" : $(SOURCE) $(DEP_CPP_SLIDC) "$(INTDIR)"\
 "$(INTDIR)\cmnctrls.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_SLIDC) "$(INTDIR)"\
 "$(INTDIR)\cmnctrls.pch"


!ELSEIF  "$(CFG)" == "cmnctrls - Win32 Unicode Release"


"$(INTDIR)\slidctrl.obj" : $(SOURCE) $(DEP_CPP_SLIDC) "$(INTDIR)"\
 "$(INTDIR)\cmnctrls.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\spinctrl.cpp

!IF  "$(CFG)" == "cmnctrls - Win32 Release"

DEP_CPP_SPINC=\
	".\stdafx.h"\
	".\ctrldemo.h"\
	".\spinctrl.h"\
	

"$(INTDIR)\spinctrl.obj" : $(SOURCE) $(DEP_CPP_SPINC) "$(INTDIR)"\
 "$(INTDIR)\cmnctrls.pch"


!ELSEIF  "$(CFG)" == "cmnctrls - Win32 Debug"

DEP_CPP_SPINC=\
	".\stdafx.h"\
	".\ctrldemo.h"\
	".\spinctrl.h"\
	

"$(INTDIR)\spinctrl.obj" : $(SOURCE) $(DEP_CPP_SPINC) "$(INTDIR)"\
 "$(INTDIR)\cmnctrls.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_SPINC) "$(INTDIR)"\
 "$(INTDIR)\cmnctrls.pch"


!ELSEIF  "$(CFG)" == "cmnctrls - Macintosh Release"

DEP_CPP_SPINC=\
	".\stdafx.h"\
	".\ctrldemo.h"\
	".\spinctrl.h"\
	

"$(INTDIR)\spinctrl.obj" : $(SOURCE) $(DEP_CPP_SPINC) "$(INTDIR)"\
 "$(INTDIR)\cmnctrls.pch"


!ELSEIF  "$(CFG)" == "cmnctrls - Macintosh Debug"

DEP_CPP_SPINC=\
	".\spinctrl.h"\
	".\ctrldemo.h"\
	".\stdafx.h"\
	

"$(INTDIR)\spinctrl.obj" : $(SOURCE) $(DEP_CPP_SPINC) "$(INTDIR)"\
 "$(INTDIR)\cmnctrls.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_SPINC) "$(INTDIR)"\
 "$(INTDIR)\cmnctrls.pch"


!ELSEIF  "$(CFG)" == "cmnctrls - Power Macintosh Release"

DEP_CPP_SPINC=\
	".\stdafx.h"\
	".\ctrldemo.h"\
	".\spinctrl.h"\
	

"$(INTDIR)\spinctrl.obj" : $(SOURCE) $(DEP_CPP_SPINC) "$(INTDIR)"\
 "$(INTDIR)\cmnctrls.pch"


!ELSEIF  "$(CFG)" == "cmnctrls - Power Macintosh Debug"

DEP_CPP_SPINC=\
	".\stdafx.h"\
	".\ctrldemo.h"\
	".\spinctrl.h"\
	

"$(INTDIR)\spinctrl.obj" : $(SOURCE) $(DEP_CPP_SPINC) "$(INTDIR)"\
 "$(INTDIR)\cmnctrls.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_SPINC) "$(INTDIR)"\
 "$(INTDIR)\cmnctrls.pch"


!ELSEIF  "$(CFG)" == "cmnctrls - Win32 Unicode Debug"

DEP_CPP_SPINC=\
	".\spinctrl.h"\
	".\ctrldemo.h"\
	".\stdafx.h"\
	

"$(INTDIR)\spinctrl.obj" : $(SOURCE) $(DEP_CPP_SPINC) "$(INTDIR)"\
 "$(INTDIR)\cmnctrls.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_SPINC) "$(INTDIR)"\
 "$(INTDIR)\cmnctrls.pch"


!ELSEIF  "$(CFG)" == "cmnctrls - Win32 Unicode Release"

DEP_CPP_SPINC=\
	".\spinctrl.h"\
	".\ctrldemo.h"\
	".\stdafx.h"\
	

"$(INTDIR)\spinctrl.obj" : $(SOURCE) $(DEP_CPP_SPINC) "$(INTDIR)"\
 "$(INTDIR)\cmnctrls.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\stdafx.cpp
DEP_CPP_STDAF=\
	".\stdafx.h"\
	

!IF  "$(CFG)" == "cmnctrls - Win32 Release"

# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/cmnctrls.pch" /Yc"stdafx.h" /Fo"$(INTDIR)/"\
 /c $(SOURCE) \
	

"$(INTDIR)\stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\cmnctrls.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "cmnctrls - Win32 Debug"

# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/cmnctrls.pch" /Yc"stdafx.h"\
 /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\cmnctrls.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "cmnctrls - Macintosh Release"

# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /AL /Q68s /W3 /GX /O2 /D "_WINDOWS" /D "_MAC" /D "_68K_" /D\
 "WIN32" /D "NDEBUG" /D "_MBCS" /Fp"$(INTDIR)/cmnctrls.pch" /Yc"stdafx.h"\
 /Fo"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\cmnctrls.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "cmnctrls - Macintosh Debug"

# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /AL /Q68s /Q68m /W3 /GX /Zi /Od /D "_WINDOWS" /D "_MAC" /D\
 "_68K_" /D "WIN32" /D "_DEBUG" /D "_MBCS" /Fp"$(INTDIR)/cmnctrls.pch"\
 /Yc"stdafx.h" /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\cmnctrls.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "cmnctrls - Power Macintosh Release"

# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MD /W3 /GX /O2 /D "_MAC" /D "_MPPC_" /D "_WINDOWS" /D "WIN32"\
 /D "NDEBUG" /D "_MBCS" /D "_AFXDLL" /Fp"$(INTDIR)/cmnctrls.pch" /Yc"stdafx.h"\
 /Fo"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\cmnctrls.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "cmnctrls - Power Macintosh Debug"

# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MDd /W3 /GX /Zi /Od /D "_MAC" /D "_MPPC_" /D "_WINDOWS" /D\
 "WIN32" /D "_DEBUG" /D "_MBCS" /D "_AFXDLL" /Fp"$(INTDIR)/cmnctrls.pch"\
 /Yc"stdafx.h" /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /QPm /c $(SOURCE) \
	

"$(INTDIR)\stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\cmnctrls.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "cmnctrls - Win32 Unicode Debug"

# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /D "_AFXDLL" /D "_MBCS" /D "_UNICODE" /Fp"$(INTDIR)/cmnctrls.pch" /Yc"stdafx.h"\
 /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\cmnctrls.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "cmnctrls - Win32 Unicode Release"

# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /D "_UNICODE" /Fp"$(INTDIR)/cmnctrls.pch" /Yc"stdafx.h"\
 /Fo"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\cmnctrls.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\toolbar1.cpp

!IF  "$(CFG)" == "cmnctrls - Win32 Release"

DEP_CPP_TOOLB=\
	".\stdafx.h"\
	".\ctrldemo.h"\
	".\toolbar1.h"\
	

"$(INTDIR)\toolbar1.obj" : $(SOURCE) $(DEP_CPP_TOOLB) "$(INTDIR)"\
 "$(INTDIR)\cmnctrls.pch"


!ELSEIF  "$(CFG)" == "cmnctrls - Win32 Debug"

DEP_CPP_TOOLB=\
	".\stdafx.h"\
	".\ctrldemo.h"\
	".\toolbar1.h"\
	

"$(INTDIR)\toolbar1.obj" : $(SOURCE) $(DEP_CPP_TOOLB) "$(INTDIR)"\
 "$(INTDIR)\cmnctrls.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_TOOLB) "$(INTDIR)"\
 "$(INTDIR)\cmnctrls.pch"


!ELSEIF  "$(CFG)" == "cmnctrls - Macintosh Release"

DEP_CPP_TOOLB=\
	".\stdafx.h"\
	".\ctrldemo.h"\
	".\toolbar1.h"\
	

"$(INTDIR)\toolbar1.obj" : $(SOURCE) $(DEP_CPP_TOOLB) "$(INTDIR)"\
 "$(INTDIR)\cmnctrls.pch"


!ELSEIF  "$(CFG)" == "cmnctrls - Macintosh Debug"

DEP_CPP_TOOLB=\
	".\toolbar1.h"\
	".\ctrldemo.h"\
	".\stdafx.h"\
	

"$(INTDIR)\toolbar1.obj" : $(SOURCE) $(DEP_CPP_TOOLB) "$(INTDIR)"\
 "$(INTDIR)\cmnctrls.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_TOOLB) "$(INTDIR)"\
 "$(INTDIR)\cmnctrls.pch"


!ELSEIF  "$(CFG)" == "cmnctrls - Power Macintosh Release"

DEP_CPP_TOOLB=\
	".\stdafx.h"\
	".\ctrldemo.h"\
	".\toolbar1.h"\
	

"$(INTDIR)\toolbar1.obj" : $(SOURCE) $(DEP_CPP_TOOLB) "$(INTDIR)"\
 "$(INTDIR)\cmnctrls.pch"


!ELSEIF  "$(CFG)" == "cmnctrls - Power Macintosh Debug"

DEP_CPP_TOOLB=\
	".\stdafx.h"\
	".\ctrldemo.h"\
	".\toolbar1.h"\
	

"$(INTDIR)\toolbar1.obj" : $(SOURCE) $(DEP_CPP_TOOLB) "$(INTDIR)"\
 "$(INTDIR)\cmnctrls.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_TOOLB) "$(INTDIR)"\
 "$(INTDIR)\cmnctrls.pch"


!ELSEIF  "$(CFG)" == "cmnctrls - Win32 Unicode Debug"

DEP_CPP_TOOLB=\
	".\toolbar1.h"\
	".\ctrldemo.h"\
	".\stdafx.h"\
	

"$(INTDIR)\toolbar1.obj" : $(SOURCE) $(DEP_CPP_TOOLB) "$(INTDIR)"\
 "$(INTDIR)\cmnctrls.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_TOOLB) "$(INTDIR)"\
 "$(INTDIR)\cmnctrls.pch"


!ELSEIF  "$(CFG)" == "cmnctrls - Win32 Unicode Release"

DEP_CPP_TOOLB=\
	".\toolbar1.h"\
	".\ctrldemo.h"\
	".\stdafx.h"\
	

"$(INTDIR)\toolbar1.obj" : $(SOURCE) $(DEP_CPP_TOOLB) "$(INTDIR)"\
 "$(INTDIR)\cmnctrls.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\toolbar2.cpp

!IF  "$(CFG)" == "cmnctrls - Win32 Release"

DEP_CPP_TOOLBA=\
	".\stdafx.h"\
	".\ctrldemo.h"\
	".\toolbar2.h"\
	

"$(INTDIR)\toolbar2.obj" : $(SOURCE) $(DEP_CPP_TOOLBA) "$(INTDIR)"\
 "$(INTDIR)\cmnctrls.pch"


!ELSEIF  "$(CFG)" == "cmnctrls - Win32 Debug"

DEP_CPP_TOOLBA=\
	".\stdafx.h"\
	".\ctrldemo.h"\
	".\toolbar2.h"\
	

"$(INTDIR)\toolbar2.obj" : $(SOURCE) $(DEP_CPP_TOOLBA) "$(INTDIR)"\
 "$(INTDIR)\cmnctrls.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_TOOLBA) "$(INTDIR)"\
 "$(INTDIR)\cmnctrls.pch"


!ELSEIF  "$(CFG)" == "cmnctrls - Macintosh Release"

DEP_CPP_TOOLBA=\
	".\stdafx.h"\
	".\ctrldemo.h"\
	".\toolbar2.h"\
	

"$(INTDIR)\toolbar2.obj" : $(SOURCE) $(DEP_CPP_TOOLBA) "$(INTDIR)"\
 "$(INTDIR)\cmnctrls.pch"


!ELSEIF  "$(CFG)" == "cmnctrls - Macintosh Debug"

DEP_CPP_TOOLBA=\
	".\toolbar2.h"\
	".\ctrldemo.h"\
	".\stdafx.h"\
	

"$(INTDIR)\toolbar2.obj" : $(SOURCE) $(DEP_CPP_TOOLBA) "$(INTDIR)"\
 "$(INTDIR)\cmnctrls.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_TOOLBA) "$(INTDIR)"\
 "$(INTDIR)\cmnctrls.pch"


!ELSEIF  "$(CFG)" == "cmnctrls - Power Macintosh Release"

DEP_CPP_TOOLBA=\
	".\stdafx.h"\
	".\ctrldemo.h"\
	".\toolbar2.h"\
	

"$(INTDIR)\toolbar2.obj" : $(SOURCE) $(DEP_CPP_TOOLBA) "$(INTDIR)"\
 "$(INTDIR)\cmnctrls.pch"


!ELSEIF  "$(CFG)" == "cmnctrls - Power Macintosh Debug"

DEP_CPP_TOOLBA=\
	".\stdafx.h"\
	".\ctrldemo.h"\
	".\toolbar2.h"\
	

"$(INTDIR)\toolbar2.obj" : $(SOURCE) $(DEP_CPP_TOOLBA) "$(INTDIR)"\
 "$(INTDIR)\cmnctrls.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_TOOLBA) "$(INTDIR)"\
 "$(INTDIR)\cmnctrls.pch"


!ELSEIF  "$(CFG)" == "cmnctrls - Win32 Unicode Debug"

DEP_CPP_TOOLBA=\
	".\toolbar2.h"\
	".\ctrldemo.h"\
	".\stdafx.h"\
	

"$(INTDIR)\toolbar2.obj" : $(SOURCE) $(DEP_CPP_TOOLBA) "$(INTDIR)"\
 "$(INTDIR)\cmnctrls.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_TOOLBA) "$(INTDIR)"\
 "$(INTDIR)\cmnctrls.pch"


!ELSEIF  "$(CFG)" == "cmnctrls - Win32 Unicode Release"

DEP_CPP_TOOLBA=\
	".\toolbar2.h"\
	".\ctrldemo.h"\
	".\stdafx.h"\
	

"$(INTDIR)\toolbar2.obj" : $(SOURCE) $(DEP_CPP_TOOLBA) "$(INTDIR)"\
 "$(INTDIR)\cmnctrls.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\toolpage.cpp

!IF  "$(CFG)" == "cmnctrls - Win32 Release"

DEP_CPP_TOOLP=\
	".\stdafx.h"\
	".\ctrldemo.h"\
	".\toolbar1.h"\
	".\toolbar2.h"\
	".\toolpage.h"\
	

"$(INTDIR)\toolpage.obj" : $(SOURCE) $(DEP_CPP_TOOLP) "$(INTDIR)"\
 "$(INTDIR)\cmnctrls.pch"


!ELSEIF  "$(CFG)" == "cmnctrls - Win32 Debug"

DEP_CPP_TOOLP=\
	".\stdafx.h"\
	".\ctrldemo.h"\
	".\toolbar1.h"\
	".\toolbar2.h"\
	".\toolpage.h"\
	

"$(INTDIR)\toolpage.obj" : $(SOURCE) $(DEP_CPP_TOOLP) "$(INTDIR)"\
 "$(INTDIR)\cmnctrls.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_TOOLP) "$(INTDIR)"\
 "$(INTDIR)\cmnctrls.pch"


!ELSEIF  "$(CFG)" == "cmnctrls - Macintosh Release"

DEP_CPP_TOOLP=\
	".\stdafx.h"\
	".\ctrldemo.h"\
	".\toolbar1.h"\
	".\toolbar2.h"\
	".\toolpage.h"\
	

"$(INTDIR)\toolpage.obj" : $(SOURCE) $(DEP_CPP_TOOLP) "$(INTDIR)"\
 "$(INTDIR)\cmnctrls.pch"


!ELSEIF  "$(CFG)" == "cmnctrls - Macintosh Debug"

DEP_CPP_TOOLP=\
	".\toolpage.h"\
	".\toolbar1.h"\
	".\toolbar2.h"\
	".\ctrldemo.h"\
	".\stdafx.h"\
	

"$(INTDIR)\toolpage.obj" : $(SOURCE) $(DEP_CPP_TOOLP) "$(INTDIR)"\
 "$(INTDIR)\cmnctrls.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_TOOLP) "$(INTDIR)"\
 "$(INTDIR)\cmnctrls.pch"


!ELSEIF  "$(CFG)" == "cmnctrls - Power Macintosh Release"

DEP_CPP_TOOLP=\
	".\stdafx.h"\
	".\ctrldemo.h"\
	".\toolbar1.h"\
	".\toolbar2.h"\
	".\toolpage.h"\
	

"$(INTDIR)\toolpage.obj" : $(SOURCE) $(DEP_CPP_TOOLP) "$(INTDIR)"\
 "$(INTDIR)\cmnctrls.pch"


!ELSEIF  "$(CFG)" == "cmnctrls - Power Macintosh Debug"

DEP_CPP_TOOLP=\
	".\stdafx.h"\
	".\ctrldemo.h"\
	".\toolbar1.h"\
	".\toolbar2.h"\
	".\toolpage.h"\
	

"$(INTDIR)\toolpage.obj" : $(SOURCE) $(DEP_CPP_TOOLP) "$(INTDIR)"\
 "$(INTDIR)\cmnctrls.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_TOOLP) "$(INTDIR)"\
 "$(INTDIR)\cmnctrls.pch"


!ELSEIF  "$(CFG)" == "cmnctrls - Win32 Unicode Debug"

DEP_CPP_TOOLP=\
	".\toolpage.h"\
	".\toolbar1.h"\
	".\toolbar2.h"\
	".\ctrldemo.h"\
	".\stdafx.h"\
	

"$(INTDIR)\toolpage.obj" : $(SOURCE) $(DEP_CPP_TOOLP) "$(INTDIR)"\
 "$(INTDIR)\cmnctrls.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_TOOLP) "$(INTDIR)"\
 "$(INTDIR)\cmnctrls.pch"


!ELSEIF  "$(CFG)" == "cmnctrls - Win32 Unicode Release"

DEP_CPP_TOOLP=\
	".\toolpage.h"\
	".\toolbar1.h"\
	".\toolbar2.h"\
	".\ctrldemo.h"\
	".\stdafx.h"\
	

"$(INTDIR)\toolpage.obj" : $(SOURCE) $(DEP_CPP_TOOLP) "$(INTDIR)"\
 "$(INTDIR)\cmnctrls.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\treecpg.cpp

!IF  "$(CFG)" == "cmnctrls - Win32 Release"

DEP_CPP_TREEC=\
	".\stdafx.h"\
	".\ctrldemo.h"\
	".\treecpg.h"\
	".\mtreectl.h"\
	".\mlistctl.h"\
	".\notifwdw.h"\
	

"$(INTDIR)\treecpg.obj" : $(SOURCE) $(DEP_CPP_TREEC) "$(INTDIR)"\
 "$(INTDIR)\cmnctrls.pch"


!ELSEIF  "$(CFG)" == "cmnctrls - Win32 Debug"

DEP_CPP_TREEC=\
	".\stdafx.h"\
	".\ctrldemo.h"\
	".\treecpg.h"\
	".\mtreectl.h"\
	".\mlistctl.h"\
	".\notifwdw.h"\
	

"$(INTDIR)\treecpg.obj" : $(SOURCE) $(DEP_CPP_TREEC) "$(INTDIR)"\
 "$(INTDIR)\cmnctrls.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_TREEC) "$(INTDIR)"\
 "$(INTDIR)\cmnctrls.pch"


!ELSEIF  "$(CFG)" == "cmnctrls - Macintosh Release"

DEP_CPP_TREEC=\
	".\stdafx.h"\
	".\ctrldemo.h"\
	".\treecpg.h"\
	".\mtreectl.h"\
	".\mlistctl.h"\
	".\notifwdw.h"\
	

"$(INTDIR)\treecpg.obj" : $(SOURCE) $(DEP_CPP_TREEC) "$(INTDIR)"\
 "$(INTDIR)\cmnctrls.pch"


!ELSEIF  "$(CFG)" == "cmnctrls - Macintosh Debug"

DEP_CPP_TREEC=\
	".\ctrldemo.h"\
	".\mtreectl.h"\
	".\stdafx.h"\
	".\treecpg.h"\
	".\mlistctl.h"\
	".\notifwdw.h"\
	

"$(INTDIR)\treecpg.obj" : $(SOURCE) $(DEP_CPP_TREEC) "$(INTDIR)"\
 "$(INTDIR)\cmnctrls.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_TREEC) "$(INTDIR)"\
 "$(INTDIR)\cmnctrls.pch"


!ELSEIF  "$(CFG)" == "cmnctrls - Power Macintosh Release"

DEP_CPP_TREEC=\
	".\stdafx.h"\
	".\ctrldemo.h"\
	".\treecpg.h"\
	".\mtreectl.h"\
	".\mlistctl.h"\
	".\notifwdw.h"\
	

"$(INTDIR)\treecpg.obj" : $(SOURCE) $(DEP_CPP_TREEC) "$(INTDIR)"\
 "$(INTDIR)\cmnctrls.pch"


!ELSEIF  "$(CFG)" == "cmnctrls - Power Macintosh Debug"

DEP_CPP_TREEC=\
	".\ctrldemo.h"\
	".\mtreectl.h"\
	".\stdafx.h"\
	".\treecpg.h"\
	".\mlistctl.h"\
	".\notifwdw.h"\
	

"$(INTDIR)\treecpg.obj" : $(SOURCE) $(DEP_CPP_TREEC) "$(INTDIR)"\
 "$(INTDIR)\cmnctrls.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_TREEC) "$(INTDIR)"\
 "$(INTDIR)\cmnctrls.pch"


!ELSEIF  "$(CFG)" == "cmnctrls - Win32 Unicode Debug"

DEP_CPP_TREEC=\
	".\ctrldemo.h"\
	".\mtreectl.h"\
	".\stdafx.h"\
	".\treecpg.h"\
	".\mlistctl.h"\
	".\notifwdw.h"\
	

"$(INTDIR)\treecpg.obj" : $(SOURCE) $(DEP_CPP_TREEC) "$(INTDIR)"\
 "$(INTDIR)\cmnctrls.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_TREEC) "$(INTDIR)"\
 "$(INTDIR)\cmnctrls.pch"


!ELSEIF  "$(CFG)" == "cmnctrls - Win32 Unicode Release"

DEP_CPP_TREEC=\
	".\ctrldemo.h"\
	".\mtreectl.h"\
	".\stdafx.h"\
	".\treecpg.h"\
	".\mlistctl.h"\
	".\notifwdw.h"\
	

"$(INTDIR)\treecpg.obj" : $(SOURCE) $(DEP_CPP_TREEC) "$(INTDIR)"\
 "$(INTDIR)\cmnctrls.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\cmncmac.r

!IF  "$(CFG)" == "cmnctrls - Win32 Release"

!ELSEIF  "$(CFG)" == "cmnctrls - Win32 Debug"

!ELSEIF  "$(CFG)" == "cmnctrls - Macintosh Release"


"$(INTDIR)\cmncmac.rsc" : $(SOURCE) "$(INTDIR)"
   $(MRC) /o"$(INTDIR)/cmncmac.rsc" /D "_68K_" /D "_MAC" /D "NDEBUG" /l 0x409\
 /NOLOGO $(SOURCE)


!ELSEIF  "$(CFG)" == "cmnctrls - Macintosh Debug"


"$(INTDIR)\cmncmac.rsc" : $(SOURCE) "$(INTDIR)"
   $(MRC) /o"$(INTDIR)/cmncmac.rsc" /D "_68K_" /D "_MAC" /D "_DEBUG" /l 0x409\
 /NOLOGO $(SOURCE)


!ELSEIF  "$(CFG)" == "cmnctrls - Power Macintosh Release"


"$(INTDIR)\cmncmac.rsc" : $(SOURCE) "$(INTDIR)"
   $(MRC) /o"$(INTDIR)/cmncmac.rsc" /D "_MPPC_" /D "_MAC" /D "NDEBUG" /l 0x409\
 /NOLOGO $(SOURCE)


!ELSEIF  "$(CFG)" == "cmnctrls - Power Macintosh Debug"


"$(INTDIR)\cmncmac.rsc" : $(SOURCE) "$(INTDIR)"
   $(MRC) /o"$(INTDIR)/cmncmac.rsc" /D "_MPPC_" /D "_MAC" /D "_DEBUG" /l 0x409\
 /NOLOGO $(SOURCE)


!ELSEIF  "$(CFG)" == "cmnctrls - Win32 Unicode Debug"

!ELSEIF  "$(CFG)" == "cmnctrls - Win32 Unicode Release"

!ENDIF 

# End Source File
# End Target
# End Project
################################################################################
