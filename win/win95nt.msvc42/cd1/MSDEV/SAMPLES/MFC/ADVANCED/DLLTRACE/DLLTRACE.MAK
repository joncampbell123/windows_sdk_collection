# Microsoft Developer Studio Generated NMAKE File, Format Version 40001
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101
# TARGTYPE "Power Macintosh Dynamic-Link Library" 0x0402
# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102
# TARGTYPE "Power Macintosh Application" 0x0401

!IF "$(CFG)" == ""
CFG=Trace - Win32 Debug
!MESSAGE No configuration specified.  Defaulting to Trace - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "DllTrace - Win32 Release" && "$(CFG)" !=\
 "DllTrace - Win32 Debug" && "$(CFG)" != "DllTrace - Power Macintosh Release" &&\
 "$(CFG)" != "DllTrace - Power Macintosh Debug" && "$(CFG)" !=\
 "Trace - Win32 Release" && "$(CFG)" != "Trace - Win32 Debug" && "$(CFG)" !=\
 "Trace - Power Macintosh Release" && "$(CFG)" !=\
 "Trace - Power Macintosh Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "dlltrace.mak" CFG="Trace - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "DllTrace - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "DllTrace - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE "DllTrace - Power Macintosh Release" (based on\
 "Power Macintosh Application")
!MESSAGE "DllTrace - Power Macintosh Debug" (based on\
 "Power Macintosh Application")
!MESSAGE "Trace - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "Trace - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "Trace - Power Macintosh Release" (based on\
 "Power Macintosh Dynamic-Link Library")
!MESSAGE "Trace - Power Macintosh Debug" (based on\
 "Power Macintosh Dynamic-Link Library")
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
# PROP Target_Last_Scanned "Trace - Power Macintosh Debug"

!IF  "$(CFG)" == "DllTrace - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
OUTDIR=.\Release
INTDIR=.\Release

ALL : "Trace - Win32 Release" "$(OUTDIR)\dlltrace.exe"

CLEAN : 
	-@erase ".\Release\dlltrace.exe"
	-@erase ".\Release\dlltrace.obj"
	-@erase ".\Release\dlltrace.res"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /c
CPP_PROJ=/nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /Fo"$(INTDIR)/" /c 
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
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/dlltrace.res" /d "NDEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/dlltrace.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 Release/Trace.lib /nologo /subsystem:windows /machine:I386
LINK32_FLAGS=Release/Trace.lib /nologo /subsystem:windows /incremental:no\
 /pdb:"$(OUTDIR)/dlltrace.pdb" /machine:I386 /out:"$(OUTDIR)/dlltrace.exe" 
LINK32_OBJS= \
	"$(INTDIR)/dlltrace.obj" \
	"$(INTDIR)/dlltrace.res" \
	".\Release\Trace.lib"

"$(OUTDIR)\dlltrace.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "DllTrace - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
OUTDIR=.\Debug
INTDIR=.\Debug

ALL : "Trace - Win32 Debug" "$(OUTDIR)\dlltrace.exe"

CLEAN : 
	-@erase ".\Debug\vc40.pdb"
	-@erase ".\Debug\dlltrace.exe"
	-@erase ".\Debug\dlltrace.obj"
	-@erase ".\Debug\dlltrace.res"
	-@erase ".\Debug\dlltrace.ilk"
	-@erase ".\Debug\dlltrace.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /c
CPP_PROJ=/nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /D "_AFXDLL" /D "_MBCS" /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
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
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/dlltrace.res" /d "_DEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/dlltrace.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386
# ADD LINK32 Debug/Trace.lib /nologo /subsystem:windows /debug /machine:I386
LINK32_FLAGS=Debug/Trace.lib /nologo /subsystem:windows /incremental:yes\
 /pdb:"$(OUTDIR)/dlltrace.pdb" /debug /machine:I386\
 /out:"$(OUTDIR)/dlltrace.exe" 
LINK32_OBJS= \
	"$(INTDIR)/dlltrace.obj" \
	"$(INTDIR)/dlltrace.res" \
	".\Debug\Trace.lib"

"$(OUTDIR)\dlltrace.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "DllTrace - Power Macintosh Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "PMacRel"
# PROP BASE Intermediate_Dir "PMacRel"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "PMacRel"
# PROP Intermediate_Dir "PMacRel"
# PROP Target_Dir ""
OUTDIR=.\PMacRel
INTDIR=.\PMacRel

ALL : "Trace - Power Macintosh Release" "$(OUTDIR)\dlltrace.exe"\
 "$(OUTDIR)\dlltrace.trg"

CLEAN : 
	-@erase ".\PMacRel\dlltrace.trg"
	-@erase ".\PMacRel\dlltrace.exe"
	-@erase ".\PMacRel\dlltrace.obj"
	-@erase ".\PMacRel\dlltmac.rsc"
	-@erase ".\PMacRel\dlltrace.rsc"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

MTL=mktyplib.exe
# ADD BASE MTL /nologo /D "NDEBUG"
# ADD MTL /nologo /D "NDEBUG"
MTL_PROJ=/nologo /D "NDEBUG" 
CPP=cl.exe
# ADD BASE CPP /nologo /W3 /GX /O2 /D "_MAC" /D "_MPPC_" /D "_WINDOWS" /D "WIN32" /D "NDEBUG" /YX /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "_MAC" /D "_MPPC_" /D "_WINDOWS" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_AFXDLL" /c
CPP_PROJ=/nologo /MD /W3 /GX /O2 /D "_MAC" /D "_MPPC_" /D "_WINDOWS" /D "WIN32"\
 /D "NDEBUG" /D "_MBCS" /D "_AFXDLL" /Fo"$(INTDIR)/" /c 
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
RSC_PROJ=/l 0x409 /r /m /fo"$(INTDIR)/dlltrace.rsc" /d "_MAC" /d "_MPPC_" /d\
 "NDEBUG" 
MRC=mrc.exe
# ADD BASE MRC /D "_MPPC_" /D "_MAC" /D "NDEBUG" /NOLOGO
# ADD MRC /D "_MPPC_" /D "_MAC" /D "NDEBUG" /NOLOGO
MRC_PROJ=/D "_MPPC_" /D "_MAC" /D "NDEBUG" /l 0x409 /NOLOGO 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/dlltrace.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 cfrg.rsc /nologo /mac:bundle /machine:MPPC
# ADD LINK32 PMacRel/Trace.lib /nologo /mac:bundle /mac:creator="HELL" /machine:MPPC
LINK32_FLAGS=PMacRel/Trace.lib /nologo /mac:bundle /mac:type="APPL"\
 /mac:creator="HELL" /incremental:no /pdb:"$(OUTDIR)/dlltrace.pdb" /machine:MPPC\
 /out:"$(OUTDIR)/dlltrace.exe" 
LINK32_OBJS= \
	"$(INTDIR)/dlltrace.obj" \
	".\PMacRel\Trace.lib" \
	"$(INTDIR)/dlltmac.rsc" \
	"$(INTDIR)/dlltrace.rsc"

"$(OUTDIR)\dlltrace.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

MFILE32=mfile.exe
# ADD BASE MFILE32 COPY /NOLOGO
# ADD MFILE32 COPY /NOLOGO
MFILE32_FLAGS=COPY /NOLOGO 
MFILE32_FILES= \
	".\PMacRel\Trace.dll" \
	"$(OUTDIR)/dlltrace.exe"

"$(OUTDIR)\dlltrace.trg" : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\PMacRel\Trace.dll\
 "$(MFILE32_DEST):Trace.dll">"$(OUTDIR)\dlltrace.trg"
    $(MFILE32) $(MFILE32_FLAGS) .\PMacRel\dlltrace.exe\
 "$(MFILE32_DEST):dlltrace.exe">"$(OUTDIR)\dlltrace.trg"

DOWNLOAD : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\PMacRel\Trace.dll\
 "$(MFILE32_DEST):Trace.dll">"$(OUTDIR)\dlltrace.trg"
    $(MFILE32) $(MFILE32_FLAGS) .\PMacRel\dlltrace.exe\
 "$(MFILE32_DEST):dlltrace.exe">"$(OUTDIR)\dlltrace.trg"

!ELSEIF  "$(CFG)" == "DllTrace - Power Macintosh Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "PMacDbg"
# PROP BASE Intermediate_Dir "PMacDbg"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "PMacDbg"
# PROP Intermediate_Dir "PMacDbg"
# PROP Target_Dir ""
OUTDIR=.\PMacDbg
INTDIR=.\PMacDbg

ALL : "Trace - Power Macintosh Debug" "$(OUTDIR)\dlltrace.exe"\
 "$(OUTDIR)\dlltrace.trg"

CLEAN : 
	-@erase ".\PMacDbg\vc40.pdb"
	-@erase ".\PMacDbg\dlltrace.ilk"
	-@erase ".\PMacDbg\dlltrace.obj"
	-@erase ".\PMacDbg\dlltmac.rsc"
	-@erase ".\PMacDbg\dlltrace.rsc"
	-@erase ".\PMacDbg\dlltrace.pdb"
	-@erase ".\PMacDbg\dlltrace.trg"
	-@erase ".\PMacDbg\dlltrace.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

MTL=mktyplib.exe
# ADD BASE MTL /nologo /D "_DEBUG"
# ADD MTL /nologo /D "_DEBUG"
MTL_PROJ=/nologo /D "_DEBUG" 
CPP=cl.exe
# ADD BASE CPP /nologo /W3 /GX /Zi /Od /D "_MAC" /D "_MPPC_" /D "_WINDOWS" /D "WIN32" /D "_DEBUG" /YX /c
# ADD CPP /nologo /MDd /W3 /GX /Zi /Od /D "_MAC" /D "_MPPC_" /D "_WINDOWS" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_AFXDLL" /c
CPP_PROJ=/nologo /MDd /W3 /GX /Zi /Od /D "_MAC" /D "_MPPC_" /D "_WINDOWS" /D\
 "WIN32" /D "_DEBUG" /D "_MBCS" /D "_AFXDLL" /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
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
RSC_PROJ=/l 0x409 /r /m /fo"$(INTDIR)/dlltrace.rsc" /d "_MAC" /d "_MPPC_" /d\
 "_DEBUG" 
MRC=mrc.exe
# ADD BASE MRC /D "_MPPC_" /D "_MAC" /D "_DEBUG" /NOLOGO
# ADD MRC /D "_MPPC_" /D "_MAC" /D "_DEBUG" /NOLOGO
MRC_PROJ=/D "_MPPC_" /D "_MAC" /D "_DEBUG" /l 0x409 /NOLOGO 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/dlltrace.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 cfrg.rsc /nologo /mac:bundle /debug /machine:MPPC
# ADD LINK32 PMacDbg/Trace.lib /nologo /mac:bundle /mac:creator="HELL" /debug /machine:MPPC
LINK32_FLAGS=PMacDbg/Trace.lib /nologo /mac:bundle /mac:type="APPL"\
 /mac:creator="HELL" /incremental:yes /pdb:"$(OUTDIR)/dlltrace.pdb" /debug\
 /machine:MPPC /out:"$(OUTDIR)/dlltrace.exe" 
LINK32_OBJS= \
	"$(INTDIR)/dlltrace.obj" \
	".\PMacDbg\Trace.lib" \
	"$(INTDIR)/dlltmac.rsc" \
	"$(INTDIR)/dlltrace.rsc"

"$(OUTDIR)\dlltrace.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

MFILE32=mfile.exe
# ADD BASE MFILE32 COPY /NOLOGO
# ADD MFILE32 COPY /NOLOGO
MFILE32_FLAGS=COPY /NOLOGO 
MFILE32_FILES= \
	".\PMacDbg\Trace.dll" \
	"$(OUTDIR)/dlltrace.exe"

"$(OUTDIR)\dlltrace.trg" : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\PMacDbg\Trace.dll\
 "$(MFILE32_DEST):Trace.dll">"$(OUTDIR)\dlltrace.trg"
    $(MFILE32) $(MFILE32_FLAGS) .\PMacDbg\dlltrace.exe\
 "$(MFILE32_DEST):dlltrace.exe">"$(OUTDIR)\dlltrace.trg"

DOWNLOAD : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\PMacDbg\Trace.dll\
 "$(MFILE32_DEST):Trace.dll">"$(OUTDIR)\dlltrace.trg"
    $(MFILE32) $(MFILE32_FLAGS) .\PMacDbg\dlltrace.exe\
 "$(MFILE32_DEST):dlltrace.exe">"$(OUTDIR)\dlltrace.trg"

!ELSEIF  "$(CFG)" == "Trace - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Trace\Release"
# PROP BASE Intermediate_Dir "Trace\Release"
# PROP BASE Target_Dir "Trace"
# PROP Use_MFC 1
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir "Trace"
OUTDIR=.\Release
INTDIR=.\Release

ALL : "$(OUTDIR)\Trace.dll"

CLEAN : 
	-@erase ".\Release\Trace.dll"
	-@erase ".\Release\trace.obj"
	-@erase ".\Release\trace.res"
	-@erase ".\Release\Trace.lib"
	-@erase ".\Release\Trace.exp"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_USRDLL" /D "_WINDLL" /D "_MBCS" /c
CPP_PROJ=/nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_USRDLL" /D "_WINDLL" /D "_MBCS" /Fo"$(INTDIR)/" /c 
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
# ADD RSC /l 0x409 /d "NDEBUG"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/trace.res" /d "NDEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/Trace.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 /nologo /subsystem:windows /dll /machine:I386
LINK32_FLAGS=/nologo /subsystem:windows /dll /incremental:no\
 /pdb:"$(OUTDIR)/Trace.pdb" /machine:I386 /def:".\trace.def"\
 /out:"$(OUTDIR)/Trace.dll" /implib:"$(OUTDIR)/Trace.lib" 
DEF_FILE= \
	".\trace.def"
LINK32_OBJS= \
	"$(INTDIR)/trace.obj" \
	"$(INTDIR)/trace.res"

"$(OUTDIR)\Trace.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "Trace - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Trace\Debug"
# PROP BASE Intermediate_Dir "Trace\Debug"
# PROP BASE Target_Dir "Trace"
# PROP Use_MFC 1
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir "Trace"
OUTDIR=.\Debug
INTDIR=.\Debug

ALL : "$(OUTDIR)\Trace.dll"

CLEAN : 
	-@erase ".\Debug\vc40.pdb"
	-@erase ".\Debug\Trace.dll"
	-@erase ".\Debug\trace.obj"
	-@erase ".\Debug\trace.res"
	-@erase ".\Debug\Trace.ilk"
	-@erase ".\Debug\Trace.lib"
	-@erase ".\Debug\Trace.exp"
	-@erase ".\Debug\Trace.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_USRDLL" /D "_WINDLL" /D "_MBCS" /c
CPP_PROJ=/nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /D "_USRDLL" /D "_WINDLL" /D "_MBCS" /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
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
# ADD RSC /l 0x409 /d "_DEBUG"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/trace.res" /d "_DEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/Trace.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386
# ADD LINK32 /nologo /subsystem:windows /dll /debug /machine:I386
LINK32_FLAGS=/nologo /subsystem:windows /dll /incremental:yes\
 /pdb:"$(OUTDIR)/Trace.pdb" /debug /machine:I386 /def:".\trace.def"\
 /out:"$(OUTDIR)/Trace.dll" /implib:"$(OUTDIR)/Trace.lib" 
DEF_FILE= \
	".\trace.def"
LINK32_OBJS= \
	"$(INTDIR)/trace.obj" \
	"$(INTDIR)/trace.res"

"$(OUTDIR)\Trace.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "Trace - Power Macintosh Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Trace\PMacRel"
# PROP BASE Intermediate_Dir "Trace\PMacRel"
# PROP BASE Target_Dir "Trace"
# PROP Use_MFC 1
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "PMacRel"
# PROP Intermediate_Dir "PMacRel"
# PROP Target_Dir "Trace"
OUTDIR=.\PMacRel
INTDIR=.\PMacRel

ALL : "$(OUTDIR)\Trace.dll" "$(OUTDIR)\Trace.trg"

CLEAN : 
	-@erase ".\PMacRel\Trace.lib"
	-@erase ".\PMacRel\trace.obj"
	-@erase ".\PMacRel\trace.rsc"
	-@erase ".\PMacRel\tracemac.rsc"
	-@erase ".\PMacRel\Trace.exp"
	-@erase ".\PMacRel\Trace.trg"
	-@erase ".\PMacRel\Trace.dll"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

MTL=mktyplib.exe
# ADD BASE MTL /nologo /D "NDEBUG"
# ADD MTL /nologo /D "NDEBUG"
MTL_PROJ=/nologo /D "NDEBUG" 
CPP=cl.exe
# ADD BASE CPP /nologo /W3 /GX /O2 /D "_MAC" /D "_MPPC_" /D "_WINDOWS" /D "WIN32" /D "NDEBUG" /D "_WLMDLL" /YX /c
# ADD CPP /nologo /W3 /GX /O2 /D "NDEBUG" /D "_MAC" /D "_MPPC_" /D "_WINDOWS" /D "WIN32" /D "_MBCS" /D "_USRDLL" /D "_WINDLL" /c
CPP_PROJ=/nologo /W3 /GX /O2 /D "NDEBUG" /D "_MAC" /D "_MPPC_" /D "_WINDOWS" /D\
 "WIN32" /D "_MBCS" /D "_USRDLL" /D "_WINDLL" /Fo"$(INTDIR)/" /c 
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
RSC_PROJ=/l 0x409 /r /m /fo"$(INTDIR)/trace.rsc" /d "_MAC" /d "_MPPC_" /d\
 "NDEBUG" 
MRC=mrc.exe
# ADD BASE MRC /D "_MPPC_" /D "_MAC" /D "NDEBUG" /NOLOGO
# ADD MRC /D "_MPPC_" /D "_MAC" /D "NDEBUG" /NOLOGO
MRC_PROJ=/D "_MPPC_" /D "_MAC" /D "NDEBUG" /l 0x409 /NOLOGO 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/Trace.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 /nologo /mac:nobundle /dll /machine:MPPC
# ADD LINK32 /nologo /mac:nobundle /mac:creator="TRAC" /dll /machine:MPPC
# SUBTRACT LINK32 /nodefaultlib
LINK32_FLAGS=/nologo /mac:nobundle /mac:type="shlb" /mac:creator="TRAC"\
 /mac:init="WlmConnectionInit" /mac:term="WlmConnectionTerm" /dll\
 /incremental:no /pdb:"$(OUTDIR)/Trace.pdb" /machine:MPPC /def:".\trace.def"\
 /out:"$(OUTDIR)/Trace.dll" /implib:"$(OUTDIR)/Trace.lib" 
DEF_FILE= \
	".\trace.def"
LINK32_OBJS= \
	"$(INTDIR)/trace.obj" \
	"$(INTDIR)/trace.rsc" \
	"$(INTDIR)/tracemac.rsc"

"$(OUTDIR)\Trace.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

MFILE32=mfile.exe
# ADD BASE MFILE32 COPY /NOLOGO
# ADD MFILE32 COPY /NOLOGO
MFILE32_FLAGS=COPY /NOLOGO 
MFILE32_FILES= \
	"$(OUTDIR)/Trace.dll"

"$(OUTDIR)\Trace.trg" : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\PMacRel\Trace.dll\
 "$(MFILE32_DEST):Trace.dll">"$(OUTDIR)\Trace.trg"

DOWNLOAD : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\PMacRel\Trace.dll\
 "$(MFILE32_DEST):Trace.dll">"$(OUTDIR)\Trace.trg"

!ELSEIF  "$(CFG)" == "Trace - Power Macintosh Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Trace\PMacDbg"
# PROP BASE Intermediate_Dir "Trace\PMacDbg"
# PROP BASE Target_Dir "Trace"
# PROP Use_MFC 1
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "PMacDbg"
# PROP Intermediate_Dir "PMacDbg"
# PROP Target_Dir "Trace"
OUTDIR=.\PMacDbg
INTDIR=.\PMacDbg

ALL : "$(OUTDIR)\Trace.dll" "$(OUTDIR)\Trace.trg"

CLEAN : 
	-@erase ".\PMacDbg\vc40.pdb"
	-@erase ".\PMacDbg\Trace.ilk"
	-@erase ".\PMacDbg\trace.obj"
	-@erase ".\PMacDbg\trace.rsc"
	-@erase ".\PMacDbg\tracemac.rsc"
	-@erase ".\PMacDbg\Trace.lib"
	-@erase ".\PMacDbg\Trace.exp"
	-@erase ".\PMacDbg\Trace.pdb"
	-@erase ".\PMacDbg\Trace.trg"
	-@erase ".\PMacDbg\Trace.dll"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

MTL=mktyplib.exe
# ADD BASE MTL /nologo /D "_DEBUG"
# ADD MTL /nologo /D "_DEBUG"
MTL_PROJ=/nologo /D "_DEBUG" 
CPP=cl.exe
# ADD BASE CPP /nologo /W3 /GX /Zi /Od /D "_MAC" /D "_MPPC_" /D "_WINDOWS" /D "WIN32" /D "_DEBUG" /D "_WLMDLL" /YX /c
# ADD CPP /nologo /MLd /W3 /GX /Zi /Od /D "_DEBUG" /D "_MAC" /D "_MPPC_" /D "_WINDOWS" /D "WIN32" /D "_MBCS" /D "_USRDLL" /D "_WINDLL" /c
CPP_PROJ=/nologo /MLd /W3 /GX /Zi /Od /D "_DEBUG" /D "_MAC" /D "_MPPC_" /D\
 "_WINDOWS" /D "WIN32" /D "_MBCS" /D "_USRDLL" /D "_WINDLL" /Fo"$(INTDIR)/"\
 /Fd"$(INTDIR)/" /c 
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
RSC_PROJ=/l 0x409 /r /m /fo"$(INTDIR)/trace.rsc" /d "_MAC" /d "_MPPC_" /d\
 "_DEBUG" 
MRC=mrc.exe
# ADD BASE MRC /D "_MPPC_" /D "_MAC" /D "_DEBUG" /NOLOGO
# ADD MRC /D "_MPPC_" /D "_MAC" /D "_DEBUG" /NOLOGO
MRC_PROJ=/D "_MPPC_" /D "_MAC" /D "_DEBUG" /l 0x409 /NOLOGO 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/Trace.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 /nologo /mac:nobundle /dll /debug /machine:MPPC
# ADD LINK32 /nologo /mac:nobundle /mac:creator="TRAC" /dll /debug /machine:MPPC
# SUBTRACT LINK32 /nodefaultlib
LINK32_FLAGS=/nologo /mac:nobundle /mac:type="shlb" /mac:creator="TRAC"\
 /mac:init="WlmConnectionInit" /mac:term="WlmConnectionTerm" /dll\
 /incremental:yes /pdb:"$(OUTDIR)/Trace.pdb" /debug /machine:MPPC\
 /def:".\trace.def" /out:"$(OUTDIR)/Trace.dll" /implib:"$(OUTDIR)/Trace.lib" 
DEF_FILE= \
	".\trace.def"
LINK32_OBJS= \
	"$(INTDIR)/trace.obj" \
	"$(INTDIR)/trace.rsc" \
	"$(INTDIR)/tracemac.rsc"

"$(OUTDIR)\Trace.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

MFILE32=mfile.exe
# ADD BASE MFILE32 COPY /NOLOGO
# ADD MFILE32 COPY /NOLOGO
MFILE32_FLAGS=COPY /NOLOGO 
MFILE32_FILES= \
	"$(OUTDIR)/Trace.dll"

"$(OUTDIR)\Trace.trg" : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\PMacDbg\Trace.dll\
 "$(MFILE32_DEST):Trace.dll">"$(OUTDIR)\Trace.trg"

DOWNLOAD : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\PMacDbg\Trace.dll\
 "$(MFILE32_DEST):Trace.dll">"$(OUTDIR)\Trace.trg"

!ENDIF 

################################################################################
# Begin Target

# Name "DllTrace - Win32 Release"
# Name "DllTrace - Win32 Debug"
# Name "DllTrace - Power Macintosh Release"
# Name "DllTrace - Power Macintosh Debug"

!IF  "$(CFG)" == "DllTrace - Win32 Release"

!ELSEIF  "$(CFG)" == "DllTrace - Win32 Debug"

!ELSEIF  "$(CFG)" == "DllTrace - Power Macintosh Release"

!ELSEIF  "$(CFG)" == "DllTrace - Power Macintosh Debug"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\dlltrace.rc
DEP_RSC_DLLTR=\
	".\hello.ico"\
	

!IF  "$(CFG)" == "DllTrace - Win32 Release"


"$(INTDIR)\dlltrace.res" : $(SOURCE) $(DEP_RSC_DLLTR) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "DllTrace - Win32 Debug"


"$(INTDIR)\dlltrace.res" : $(SOURCE) $(DEP_RSC_DLLTR) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "DllTrace - Power Macintosh Release"


"$(INTDIR)\dlltrace.rsc" : $(SOURCE) $(DEP_RSC_DLLTR) "$(INTDIR)"
   $(RSC) /l 0x409 /r /m /fo"$(INTDIR)/dlltrace.rsc" /d "_MAC" /d "_MPPC_" /d\
 "NDEBUG" $(SOURCE)


!ELSEIF  "$(CFG)" == "DllTrace - Power Macintosh Debug"


"$(INTDIR)\dlltrace.rsc" : $(SOURCE) $(DEP_RSC_DLLTR) "$(INTDIR)"
   $(RSC) /l 0x409 /r /m /fo"$(INTDIR)/dlltrace.rsc" /d "_MAC" /d "_MPPC_" /d\
 "_DEBUG" $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\dlltrace.cpp
DEP_CPP_DLLTRA=\
	".\traceapi.h"\
	

!IF  "$(CFG)" == "DllTrace - Win32 Release"


"$(INTDIR)\dlltrace.obj" : $(SOURCE) $(DEP_CPP_DLLTRA) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "DllTrace - Win32 Debug"


"$(INTDIR)\dlltrace.obj" : $(SOURCE) $(DEP_CPP_DLLTRA) "$(INTDIR)"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_DLLTRA) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "DllTrace - Power Macintosh Release"


"$(INTDIR)\dlltrace.obj" : $(SOURCE) $(DEP_CPP_DLLTRA) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "DllTrace - Power Macintosh Debug"


"$(INTDIR)\dlltrace.obj" : $(SOURCE) $(DEP_CPP_DLLTRA) "$(INTDIR)"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_DLLTRA) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\dlltmac.r

!IF  "$(CFG)" == "DllTrace - Win32 Release"

!ELSEIF  "$(CFG)" == "DllTrace - Win32 Debug"

!ELSEIF  "$(CFG)" == "DllTrace - Power Macintosh Release"


"$(INTDIR)\dlltmac.rsc" : $(SOURCE) "$(INTDIR)"
   $(MRC) /o"$(INTDIR)/dlltmac.rsc" /D "_MPPC_" /D "_MAC" /D "NDEBUG" /l 0x409\
 /NOLOGO $(SOURCE)


!ELSEIF  "$(CFG)" == "DllTrace - Power Macintosh Debug"


"$(INTDIR)\dlltmac.rsc" : $(SOURCE) "$(INTDIR)"
   $(MRC) /o"$(INTDIR)/dlltmac.rsc" /D "_MPPC_" /D "_MAC" /D "_DEBUG" /l 0x409\
 /NOLOGO $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Project Dependency

# Project_Dep_Name "Trace"

!IF  "$(CFG)" == "DllTrace - Win32 Release"

"Trace - Win32 Release" : 
   $(MAKE) /$(MAKEFLAGS) /F .\dlltrace.mak CFG="Trace - Win32 Release" 

!ELSEIF  "$(CFG)" == "DllTrace - Win32 Debug"

"Trace - Win32 Debug" : 
   $(MAKE) /$(MAKEFLAGS) /F .\dlltrace.mak CFG="Trace - Win32 Debug" 

!ELSEIF  "$(CFG)" == "DllTrace - Power Macintosh Release"

"Trace - Power Macintosh Release" : 
   $(MAKE) /$(MAKEFLAGS) /F .\dlltrace.mak\
 CFG="Trace - Power Macintosh Release" 

!ELSEIF  "$(CFG)" == "DllTrace - Power Macintosh Debug"

"Trace - Power Macintosh Debug" : 
   $(MAKE) /$(MAKEFLAGS) /F .\dlltrace.mak CFG="Trace - Power Macintosh Debug" 

!ENDIF 

# End Project Dependency
# End Target
################################################################################
# Begin Target

# Name "Trace - Win32 Release"
# Name "Trace - Win32 Debug"
# Name "Trace - Power Macintosh Release"
# Name "Trace - Power Macintosh Debug"

!IF  "$(CFG)" == "Trace - Win32 Release"

!ELSEIF  "$(CFG)" == "Trace - Win32 Debug"

!ELSEIF  "$(CFG)" == "Trace - Power Macintosh Release"

!ELSEIF  "$(CFG)" == "Trace - Power Macintosh Debug"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\tracemac.r

!IF  "$(CFG)" == "Trace - Win32 Release"

!ELSEIF  "$(CFG)" == "Trace - Win32 Debug"

!ELSEIF  "$(CFG)" == "Trace - Power Macintosh Release"


"$(INTDIR)\tracemac.rsc" : $(SOURCE) "$(INTDIR)"
   $(MRC) /o"$(INTDIR)/tracemac.rsc" /D "_MPPC_" /D "_MAC" /D "NDEBUG" /l 0x409\
 /NOLOGO $(SOURCE)


!ELSEIF  "$(CFG)" == "Trace - Power Macintosh Debug"


"$(INTDIR)\tracemac.rsc" : $(SOURCE) "$(INTDIR)"
   $(MRC) /o"$(INTDIR)/tracemac.rsc" /D "_MPPC_" /D "_MAC" /D "_DEBUG" /l 0x409\
 /NOLOGO $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\trace.def

!IF  "$(CFG)" == "Trace - Win32 Release"

!ELSEIF  "$(CFG)" == "Trace - Win32 Debug"

!ELSEIF  "$(CFG)" == "Trace - Power Macintosh Release"

!ELSEIF  "$(CFG)" == "Trace - Power Macintosh Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\trace.rc
DEP_RSC_TRACE=\
	".\traceres.h"\
	

!IF  "$(CFG)" == "Trace - Win32 Release"


"$(INTDIR)\trace.res" : $(SOURCE) $(DEP_RSC_TRACE) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Trace - Win32 Debug"


"$(INTDIR)\trace.res" : $(SOURCE) $(DEP_RSC_TRACE) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Trace - Power Macintosh Release"


"$(INTDIR)\trace.rsc" : $(SOURCE) $(DEP_RSC_TRACE) "$(INTDIR)"
   $(RSC) /l 0x409 /r /m /fo"$(INTDIR)/trace.rsc" /d "_MAC" /d "_MPPC_" /d\
 "NDEBUG" $(SOURCE)


!ELSEIF  "$(CFG)" == "Trace - Power Macintosh Debug"


"$(INTDIR)\trace.rsc" : $(SOURCE) $(DEP_RSC_TRACE) "$(INTDIR)"
   $(RSC) /l 0x409 /r /m /fo"$(INTDIR)/trace.rsc" /d "_MAC" /d "_MPPC_" /d\
 "_DEBUG" $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\trace.cpp
DEP_CPP_TRACE_=\
	".\traceres.h"\
	

!IF  "$(CFG)" == "Trace - Win32 Release"


"$(INTDIR)\trace.obj" : $(SOURCE) $(DEP_CPP_TRACE_) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Trace - Win32 Debug"


"$(INTDIR)\trace.obj" : $(SOURCE) $(DEP_CPP_TRACE_) "$(INTDIR)"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_TRACE_) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Trace - Power Macintosh Release"


"$(INTDIR)\trace.obj" : $(SOURCE) $(DEP_CPP_TRACE_) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Trace - Power Macintosh Debug"


"$(INTDIR)\trace.obj" : $(SOURCE) $(DEP_CPP_TRACE_) "$(INTDIR)"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_TRACE_) "$(INTDIR)"


!ENDIF 

# End Source File
# End Target
# End Project
################################################################################
