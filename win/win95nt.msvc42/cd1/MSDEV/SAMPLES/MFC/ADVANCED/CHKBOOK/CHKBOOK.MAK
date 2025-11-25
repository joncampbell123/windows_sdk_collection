# Microsoft Visual C++ Generated NMAKE File, Format Version 40000
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101
# TARGTYPE "Power Macintosh Application" 0x0401
# TARGTYPE "Macintosh Application" 0x0301

!IF "$(CFG)" == ""
CFG=ChkBook - Win32 Debug
!MESSAGE No configuration specified.  Defaulting to ChkBook - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "ChkBook - Win32 Release" && "$(CFG)" !=\
 "ChkBook - Win32 Debug" && "$(CFG)" != "ChkBook - Macintosh Release" &&\
 "$(CFG)" != "ChkBook - Macintosh Debug" && "$(CFG)" !=\
 "ChkBook - Power Macintosh Release" && "$(CFG)" !=\
 "ChkBook - Power Macintosh Debug" && "$(CFG)" !=\
 "ChkBook - Win32 Unicode Release" && "$(CFG)" !=\
 "ChkBook - Win32 Unicode Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "chkbook.mak" CFG="ChkBook - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "ChkBook - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "ChkBook - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE "ChkBook - Macintosh Release" (based on "Macintosh Application")
!MESSAGE "ChkBook - Macintosh Debug" (based on "Macintosh Application")
!MESSAGE "ChkBook - Power Macintosh Release" (based on\
 "Power Macintosh Application")
!MESSAGE "ChkBook - Power Macintosh Debug" (based on\
 "Power Macintosh Application")
!MESSAGE "ChkBook - Win32 Unicode Release" (based on "Win32 (x86) Application")
!MESSAGE "ChkBook - Win32 Unicode Debug" (based on "Win32 (x86) Application")
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
# PROP Target_Last_Scanned "ChkBook - Win32 Unicode Debug"

!IF  "$(CFG)" == "ChkBook - Win32 Release"

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

ALL : "$(OUTDIR)\chkbook.exe"

CLEAN : 
	-@erase ".\Release\chkbook.exe"
	-@erase ".\Release\checkvw.obj"
	-@erase ".\Release\chkbook.pch"
	-@erase ".\Release\checkdoc.obj"
	-@erase ".\Release\rowview.obj"
	-@erase ".\Release\mainfrm.obj"
	-@erase ".\Release\chkbook.obj"
	-@erase ".\Release\fxrecdoc.obj"
	-@erase ".\Release\stdafx.obj"
	-@erase ".\Release\dollcent.obj"
	-@erase ".\Release\chkbkfrm.obj"
	-@erase ".\Release\bookvw.obj"
	-@erase ".\Release\chkbook.res"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"Stdafx.h" /c
CPP_PROJ=/nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/chkbook.pch" /Yu"Stdafx.h" /Fo"$(INTDIR)/"\
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
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/chkbook.res" /d "NDEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/chkbook.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 /nologo /subsystem:windows /machine:I386
LINK32_FLAGS=/nologo /subsystem:windows /incremental:no\
 /pdb:"$(OUTDIR)/chkbook.pdb" /machine:I386 /out:"$(OUTDIR)/chkbook.exe" 
LINK32_OBJS= \
	"$(INTDIR)/checkvw.obj" \
	"$(INTDIR)/checkdoc.obj" \
	"$(INTDIR)/rowview.obj" \
	"$(INTDIR)/mainfrm.obj" \
	"$(INTDIR)/chkbook.obj" \
	"$(INTDIR)/fxrecdoc.obj" \
	"$(INTDIR)/stdafx.obj" \
	"$(INTDIR)/dollcent.obj" \
	"$(INTDIR)/chkbkfrm.obj" \
	"$(INTDIR)/bookvw.obj" \
	"$(INTDIR)/chkbook.res"

"$(OUTDIR)\chkbook.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "ChkBook - Win32 Debug"

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

ALL : "$(OUTDIR)\chkbook.exe"

CLEAN : 
	-@erase ".\Debug\vc40.pdb"
	-@erase ".\Debug\chkbook.pch"
	-@erase ".\Debug\chkbook.exe"
	-@erase ".\Debug\bookvw.obj"
	-@erase ".\Debug\fxrecdoc.obj"
	-@erase ".\Debug\checkvw.obj"
	-@erase ".\Debug\dollcent.obj"
	-@erase ".\Debug\chkbkfrm.obj"
	-@erase ".\Debug\rowview.obj"
	-@erase ".\Debug\mainfrm.obj"
	-@erase ".\Debug\chkbook.obj"
	-@erase ".\Debug\stdafx.obj"
	-@erase ".\Debug\checkdoc.obj"
	-@erase ".\Debug\chkbook.res"
	-@erase ".\Debug\chkbook.ilk"
	-@erase ".\Debug\chkbook.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"Stdafx.h" /c
CPP_PROJ=/nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/chkbook.pch" /Yu"Stdafx.h"\
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
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/chkbook.res" /d "_DEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/chkbook.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386
# ADD LINK32 /nologo /subsystem:windows /debug /machine:I386
LINK32_FLAGS=/nologo /subsystem:windows /incremental:yes\
 /pdb:"$(OUTDIR)/chkbook.pdb" /debug /machine:I386 /out:"$(OUTDIR)/chkbook.exe" 
LINK32_OBJS= \
	"$(INTDIR)/bookvw.obj" \
	"$(INTDIR)/fxrecdoc.obj" \
	"$(INTDIR)/checkvw.obj" \
	"$(INTDIR)/dollcent.obj" \
	"$(INTDIR)/chkbkfrm.obj" \
	"$(INTDIR)/rowview.obj" \
	"$(INTDIR)/mainfrm.obj" \
	"$(INTDIR)/chkbook.obj" \
	"$(INTDIR)/stdafx.obj" \
	"$(INTDIR)/checkdoc.obj" \
	"$(INTDIR)/chkbook.res"

"$(OUTDIR)\chkbook.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "ChkBook - Macintosh Release"

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

ALL : "$(OUTDIR)\chkbook.exe" "$(OUTDIR)\chkbook.trg"

CLEAN : 
	-@erase ".\MacRel\chkbook.trg"
	-@erase ".\MacRel\chkbook.exe"
	-@erase ".\MacRel\checkdoc.obj"
	-@erase ".\MacRel\chkbook.pch"
	-@erase ".\MacRel\checkvw.obj"
	-@erase ".\MacRel\stdafx.obj"
	-@erase ".\MacRel\fxrecdoc.obj"
	-@erase ".\MacRel\rowview.obj"
	-@erase ".\MacRel\mainfrm.obj"
	-@erase ".\MacRel\chkbook.obj"
	-@erase ".\MacRel\bookvw.obj"
	-@erase ".\MacRel\dollcent.obj"
	-@erase ".\MacRel\chkbkfrm.obj"
	-@erase ".\MacRel\chkbook.rsc"
	-@erase ".\MacRel\chkbmac.rsc"

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
 "WIN32" /D "NDEBUG" /D "_MBCS" /Fp"$(INTDIR)/chkbook.pch" /Yu"Stdafx.h"\
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
RSC_PROJ=/l 0x409 /r /m /fo"$(INTDIR)/chkbook.rsc" /d "_MAC" /d "_68K_" /d\
 "NDEBUG" 
MRC=mrc.exe
# ADD BASE MRC /D "_68K_" /D "_MAC" /D "NDEBUG" /NOLOGO
# ADD MRC /D "_68K_" /D "_MAC" /D "NDEBUG" /NOLOGO
MRC_PROJ=/D "_68K_" /D "_MAC" /D "NDEBUG" /l 0x409 /NOLOGO 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/chkbook.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 wlm.lib /nologo /MAC:bundle /machine:M68K
# ADD LINK32 /nologo /MAC:bundle /MAC:creator="CHKB" /machine:M68K
LINK32_FLAGS=/nologo /MAC:bundle /MAC:type="APPL" /MAC:creator="CHKB"\
 /pdb:"$(OUTDIR)/chkbook.pdb" /machine:M68K /out:"$(OUTDIR)/chkbook.exe" 
LINK32_OBJS= \
	"$(INTDIR)/checkdoc.obj" \
	"$(INTDIR)/checkvw.obj" \
	"$(INTDIR)/stdafx.obj" \
	"$(INTDIR)/fxrecdoc.obj" \
	"$(INTDIR)/rowview.obj" \
	"$(INTDIR)/mainfrm.obj" \
	"$(INTDIR)/chkbook.obj" \
	"$(INTDIR)/bookvw.obj" \
	"$(INTDIR)/dollcent.obj" \
	"$(INTDIR)/chkbkfrm.obj" \
	"$(INTDIR)/chkbook.rsc" \
	"$(INTDIR)/chkbmac.rsc"

"$(OUTDIR)\chkbook.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

MFILE32=mfile.exe
# ADD BASE MFILE32 COPY /NOLOGO
# ADD MFILE32 COPY /NOLOGO
MFILE32_FLAGS=COPY /NOLOGO 
MFILE32_FILES= \
	"$(OUTDIR)/chkbook.exe"

"$(OUTDIR)\chkbook.trg" : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\MacRel\chkbook.exe\
 "$(MFILE32_DEST):chkbook.exe">"$(OUTDIR)\chkbook.trg"

DOWNLOAD : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\MacRel\chkbook.exe\
 "$(MFILE32_DEST):chkbook.exe">"$(OUTDIR)\chkbook.trg"

!ELSEIF  "$(CFG)" == "ChkBook - Macintosh Debug"

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

ALL : "$(OUTDIR)\chkbook.exe" "$(OUTDIR)\chkbook.trg"

CLEAN : 
	-@erase ".\MacDbg\vc40.pdb"
	-@erase ".\MacDbg\chkbook.pch"
	-@erase ".\MacDbg\chkbook.pdb"
	-@erase ".\MacDbg\dollcent.obj"
	-@erase ".\MacDbg\rowview.obj"
	-@erase ".\MacDbg\mainfrm.obj"
	-@erase ".\MacDbg\chkbook.obj"
	-@erase ".\MacDbg\stdafx.obj"
	-@erase ".\MacDbg\checkdoc.obj"
	-@erase ".\MacDbg\fxrecdoc.obj"
	-@erase ".\MacDbg\bookvw.obj"
	-@erase ".\MacDbg\checkvw.obj"
	-@erase ".\MacDbg\chkbkfrm.obj"
	-@erase ".\MacDbg\chkbmac.rsc"
	-@erase ".\MacDbg\chkbook.rsc"
	-@erase ".\MacDbg\chkbook.trg"
	-@erase ".\MacDbg\chkbook.exe"

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
 "_68K_" /D "WIN32" /D "_DEBUG" /D "_MBCS" /Fp"$(INTDIR)/chkbook.pch"\
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
RSC_PROJ=/l 0x409 /r /m /fo"$(INTDIR)/chkbook.rsc" /d "_MAC" /d "_68K_" /d\
 "_DEBUG" 
MRC=mrc.exe
# ADD BASE MRC /D "_68K_" /D "_MAC" /D "_DEBUG" /NOLOGO
# ADD MRC /D "_68K_" /D "_MAC" /D "_DEBUG" /NOLOGO
MRC_PROJ=/D "_68K_" /D "_MAC" /D "_DEBUG" /l 0x409 /NOLOGO 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/chkbook.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 wlm.lib /nologo /MAC:bundle /debug /machine:M68K
# ADD LINK32 /nologo /MAC:bundle /MAC:creator="CHKB" /debug /machine:M68K
LINK32_FLAGS=/nologo /MAC:bundle /MAC:type="APPL" /MAC:creator="CHKB"\
 /pdb:"$(OUTDIR)/chkbook.pdb" /debug /machine:M68K /out:"$(OUTDIR)/chkbook.exe" 
LINK32_OBJS= \
	"$(INTDIR)/dollcent.obj" \
	"$(INTDIR)/rowview.obj" \
	"$(INTDIR)/mainfrm.obj" \
	"$(INTDIR)/chkbook.obj" \
	"$(INTDIR)/stdafx.obj" \
	"$(INTDIR)/checkdoc.obj" \
	"$(INTDIR)/fxrecdoc.obj" \
	"$(INTDIR)/bookvw.obj" \
	"$(INTDIR)/checkvw.obj" \
	"$(INTDIR)/chkbkfrm.obj" \
	"$(INTDIR)/chkbmac.rsc" \
	"$(INTDIR)/chkbook.rsc"

"$(OUTDIR)\chkbook.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

MFILE32=mfile.exe
# ADD BASE MFILE32 COPY /NOLOGO
# ADD MFILE32 COPY /NOLOGO
MFILE32_FLAGS=COPY /NOLOGO 
MFILE32_FILES= \
	"$(OUTDIR)/chkbook.exe"

"$(OUTDIR)\chkbook.trg" : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\MacDbg\chkbook.exe\
 "$(MFILE32_DEST):chkbook.exe">"$(OUTDIR)\chkbook.trg"

DOWNLOAD : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\MacDbg\chkbook.exe\
 "$(MFILE32_DEST):chkbook.exe">"$(OUTDIR)\chkbook.trg"

!ELSEIF  "$(CFG)" == "ChkBook - Power Macintosh Release"

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

ALL : "$(OUTDIR)\chkbook.exe" "$(OUTDIR)\chkbook.trg"

CLEAN : 
	-@erase ".\PMacRel\chkbook.trg"
	-@erase ".\PMacRel\chkbook.exe"
	-@erase ".\PMacRel\rowview.obj"
	-@erase ".\PMacRel\chkbook.pch"
	-@erase ".\PMacRel\mainfrm.obj"
	-@erase ".\PMacRel\stdafx.obj"
	-@erase ".\PMacRel\dollcent.obj"
	-@erase ".\PMacRel\chkbook.obj"
	-@erase ".\PMacRel\chkbkfrm.obj"
	-@erase ".\PMacRel\bookvw.obj"
	-@erase ".\PMacRel\checkdoc.obj"
	-@erase ".\PMacRel\fxrecdoc.obj"
	-@erase ".\PMacRel\checkvw.obj"
	-@erase ".\PMacRel\chkbmac.rsc"
	-@erase ".\PMacRel\chkbook.rsc"

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
 "_WINDOWS" /D "WIN32" /D "_MBCS" /D "_AFXDLL" /Fp"$(INTDIR)/chkbook.pch"\
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
RSC_PROJ=/l 0x409 /r /m /fo"$(INTDIR)/chkbook.rsc" /d "_MAC" /d "_MPPC_" /d\
 "NDEBUG" 
MRC=mrc.exe
# ADD BASE MRC /D "_MPPC_" /D "_MAC" /D "NDEBUG" /NOLOGO
# ADD MRC /D "_MPPC_" /D "_MAC" /D "NDEBUG" /NOLOGO
MRC_PROJ=/D "_MPPC_" /D "_MAC" /D "NDEBUG" /l 0x409 /NOLOGO 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/chkbook.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 cfrg.rsc /nologo /mac:bundle /machine:MPPC
# ADD LINK32 /nologo /mac:bundle /mac:creator="CHKB" /machine:MPPC
LINK32_FLAGS=/nologo /mac:bundle /mac:type="APPL" /mac:creator="CHKB"\
 /incremental:no /pdb:"$(OUTDIR)/chkbook.pdb" /machine:MPPC\
 /out:"$(OUTDIR)/chkbook.exe" 
LINK32_OBJS= \
	"$(INTDIR)/rowview.obj" \
	"$(INTDIR)/mainfrm.obj" \
	"$(INTDIR)/stdafx.obj" \
	"$(INTDIR)/dollcent.obj" \
	"$(INTDIR)/chkbook.obj" \
	"$(INTDIR)/chkbkfrm.obj" \
	"$(INTDIR)/bookvw.obj" \
	"$(INTDIR)/checkdoc.obj" \
	"$(INTDIR)/fxrecdoc.obj" \
	"$(INTDIR)/checkvw.obj" \
	"$(INTDIR)/chkbmac.rsc" \
	"$(INTDIR)/chkbook.rsc"

"$(OUTDIR)\chkbook.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

MFILE32=mfile.exe
# ADD BASE MFILE32 COPY /NOLOGO
# ADD MFILE32 COPY /NOLOGO
MFILE32_FLAGS=COPY /NOLOGO 
MFILE32_FILES= \
	"$(OUTDIR)/chkbook.exe"

"$(OUTDIR)\chkbook.trg" : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\PMacRel\chkbook.exe\
 "$(MFILE32_DEST):chkbook.exe">"$(OUTDIR)\chkbook.trg"

DOWNLOAD : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\PMacRel\chkbook.exe\
 "$(MFILE32_DEST):chkbook.exe">"$(OUTDIR)\chkbook.trg"

!ELSEIF  "$(CFG)" == "ChkBook - Power Macintosh Debug"

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

ALL : "$(OUTDIR)\chkbook.exe" "$(OUTDIR)\chkbook.trg"

CLEAN : 
	-@erase ".\PMacDbg\vc40.pdb"
	-@erase ".\PMacDbg\chkbook.pch"
	-@erase ".\PMacDbg\chkbook.ilk"
	-@erase ".\PMacDbg\chkbkfrm.obj"
	-@erase ".\PMacDbg\checkvw.obj"
	-@erase ".\PMacDbg\dollcent.obj"
	-@erase ".\PMacDbg\bookvw.obj"
	-@erase ".\PMacDbg\rowview.obj"
	-@erase ".\PMacDbg\mainfrm.obj"
	-@erase ".\PMacDbg\chkbook.obj"
	-@erase ".\PMacDbg\checkdoc.obj"
	-@erase ".\PMacDbg\fxrecdoc.obj"
	-@erase ".\PMacDbg\stdafx.obj"
	-@erase ".\PMacDbg\chkbook.rsc"
	-@erase ".\PMacDbg\chkbmac.rsc"
	-@erase ".\PMacDbg\chkbook.pdb"
	-@erase ".\PMacDbg\chkbook.trg"
	-@erase ".\PMacDbg\chkbook.exe"

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
 "_WINDOWS" /D "WIN32" /D "_MBCS" /D "_AFXDLL" /Fp"$(INTDIR)/chkbook.pch"\
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
RSC_PROJ=/l 0x409 /r /m /fo"$(INTDIR)/chkbook.rsc" /d "_MAC" /d "_MPPC_" /d\
 "_DEBUG" 
MRC=mrc.exe
# ADD BASE MRC /D "_MPPC_" /D "_MAC" /D "_DEBUG" /NOLOGO
# ADD MRC /D "_MPPC_" /D "_MAC" /D "_DEBUG" /NOLOGO
MRC_PROJ=/D "_MPPC_" /D "_MAC" /D "_DEBUG" /l 0x409 /NOLOGO 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/chkbook.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 cfrg.rsc /nologo /mac:bundle /debug /machine:MPPC
# ADD LINK32 /nologo /mac:bundle /mac:creator="CHKB" /debug /machine:MPPC
LINK32_FLAGS=/nologo /mac:bundle /mac:type="APPL" /mac:creator="CHKB"\
 /incremental:yes /pdb:"$(OUTDIR)/chkbook.pdb" /debug /machine:MPPC\
 /out:"$(OUTDIR)/chkbook.exe" 
LINK32_OBJS= \
	"$(INTDIR)/chkbkfrm.obj" \
	"$(INTDIR)/checkvw.obj" \
	"$(INTDIR)/dollcent.obj" \
	"$(INTDIR)/bookvw.obj" \
	"$(INTDIR)/rowview.obj" \
	"$(INTDIR)/mainfrm.obj" \
	"$(INTDIR)/chkbook.obj" \
	"$(INTDIR)/checkdoc.obj" \
	"$(INTDIR)/fxrecdoc.obj" \
	"$(INTDIR)/stdafx.obj" \
	"$(INTDIR)/chkbook.rsc" \
	"$(INTDIR)/chkbmac.rsc"

"$(OUTDIR)\chkbook.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

MFILE32=mfile.exe
# ADD BASE MFILE32 COPY /NOLOGO
# ADD MFILE32 COPY /NOLOGO
MFILE32_FLAGS=COPY /NOLOGO 
MFILE32_FILES= \
	"$(OUTDIR)/chkbook.exe"

"$(OUTDIR)\chkbook.trg" : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\PMacDbg\chkbook.exe\
 "$(MFILE32_DEST):chkbook.exe">"$(OUTDIR)\chkbook.trg"

DOWNLOAD : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\PMacDbg\chkbook.exe\
 "$(MFILE32_DEST):chkbook.exe">"$(OUTDIR)\chkbook.trg"

!ELSEIF  "$(CFG)" == "ChkBook - Win32 Unicode Release"

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

ALL : "$(OUTDIR)\chkbook.exe"

CLEAN : 
	-@erase ".\UniRelease\chkbook.exe"
	-@erase ".\UniRelease\bookvw.obj"
	-@erase ".\UniRelease\chkbook.pch"
	-@erase ".\UniRelease\checkdoc.obj"
	-@erase ".\UniRelease\fxrecdoc.obj"
	-@erase ".\UniRelease\checkvw.obj"
	-@erase ".\UniRelease\dollcent.obj"
	-@erase ".\UniRelease\chkbkfrm.obj"
	-@erase ".\UniRelease\stdafx.obj"
	-@erase ".\UniRelease\rowview.obj"
	-@erase ".\UniRelease\mainfrm.obj"
	-@erase ".\UniRelease\chkbook.obj"
	-@erase ".\UniRelease\chkbook.res"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_UNICODE" /D "_AFXDLL" /D "_MBCS" /Yu"Stdafx.h" /c
CPP_PROJ=/nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D\
 "_UNICODE" /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/chkbook.pch" /Yu"Stdafx.h"\
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
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/chkbook.res" /d "NDEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/chkbook.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 /nologo /entry:"wWinMainCRTStartup" /subsystem:windows /machine:I386
LINK32_FLAGS=/nologo /entry:"wWinMainCRTStartup" /subsystem:windows\
 /incremental:no /pdb:"$(OUTDIR)/chkbook.pdb" /machine:I386\
 /out:"$(OUTDIR)/chkbook.exe" 
LINK32_OBJS= \
	"$(INTDIR)/bookvw.obj" \
	"$(INTDIR)/checkdoc.obj" \
	"$(INTDIR)/fxrecdoc.obj" \
	"$(INTDIR)/checkvw.obj" \
	"$(INTDIR)/dollcent.obj" \
	"$(INTDIR)/chkbkfrm.obj" \
	"$(INTDIR)/stdafx.obj" \
	"$(INTDIR)/rowview.obj" \
	"$(INTDIR)/mainfrm.obj" \
	"$(INTDIR)/chkbook.obj" \
	"$(INTDIR)/chkbook.res"

"$(OUTDIR)\chkbook.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "ChkBook - Win32 Unicode Debug"

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

ALL : "$(OUTDIR)\chkbook.exe"

CLEAN : 
	-@erase ".\UniDebug\vc40.pdb"
	-@erase ".\UniDebug\chkbook.pch"
	-@erase ".\UniDebug\chkbook.exe"
	-@erase ".\UniDebug\checkdoc.obj"
	-@erase ".\UniDebug\checkvw.obj"
	-@erase ".\UniDebug\fxrecdoc.obj"
	-@erase ".\UniDebug\bookvw.obj"
	-@erase ".\UniDebug\rowview.obj"
	-@erase ".\UniDebug\mainfrm.obj"
	-@erase ".\UniDebug\chkbook.obj"
	-@erase ".\UniDebug\chkbkfrm.obj"
	-@erase ".\UniDebug\dollcent.obj"
	-@erase ".\UniDebug\stdafx.obj"
	-@erase ".\UniDebug\chkbook.res"
	-@erase ".\UniDebug\chkbook.ilk"
	-@erase ".\UniDebug\chkbook.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_UNICODE" /D "_AFXDLL" /D "_MBCS" /Yu"Stdafx.h" /c
CPP_PROJ=/nologo /MDd /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS"\
 /D "_UNICODE" /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/chkbook.pch" /Yu"Stdafx.h"\
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
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/chkbook.res" /d "_DEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/chkbook.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386
# ADD LINK32 /nologo /entry:"wWinMainCRTStartup" /subsystem:windows /debug /machine:I386
LINK32_FLAGS=/nologo /entry:"wWinMainCRTStartup" /subsystem:windows\
 /incremental:yes /pdb:"$(OUTDIR)/chkbook.pdb" /debug /machine:I386\
 /out:"$(OUTDIR)/chkbook.exe" 
LINK32_OBJS= \
	"$(INTDIR)/checkdoc.obj" \
	"$(INTDIR)/checkvw.obj" \
	"$(INTDIR)/fxrecdoc.obj" \
	"$(INTDIR)/bookvw.obj" \
	"$(INTDIR)/rowview.obj" \
	"$(INTDIR)/mainfrm.obj" \
	"$(INTDIR)/chkbook.obj" \
	"$(INTDIR)/chkbkfrm.obj" \
	"$(INTDIR)/dollcent.obj" \
	"$(INTDIR)/stdafx.obj" \
	"$(INTDIR)/chkbook.res"

"$(OUTDIR)\chkbook.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 

################################################################################
# Begin Target

# Name "ChkBook - Win32 Release"
# Name "ChkBook - Win32 Debug"
# Name "ChkBook - Macintosh Release"
# Name "ChkBook - Macintosh Debug"
# Name "ChkBook - Power Macintosh Release"
# Name "ChkBook - Power Macintosh Debug"
# Name "ChkBook - Win32 Unicode Release"
# Name "ChkBook - Win32 Unicode Debug"

!IF  "$(CFG)" == "ChkBook - Win32 Release"

!ELSEIF  "$(CFG)" == "ChkBook - Win32 Debug"

!ELSEIF  "$(CFG)" == "ChkBook - Macintosh Release"

!ELSEIF  "$(CFG)" == "ChkBook - Macintosh Debug"

!ELSEIF  "$(CFG)" == "ChkBook - Power Macintosh Release"

!ELSEIF  "$(CFG)" == "ChkBook - Power Macintosh Debug"

!ELSEIF  "$(CFG)" == "ChkBook - Win32 Unicode Release"

!ELSEIF  "$(CFG)" == "ChkBook - Win32 Unicode Debug"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\stdafx.cpp
DEP_CPP_STDAF=\
	".\stdafx.h"\
	

!IF  "$(CFG)" == "ChkBook - Win32 Release"

# ADD CPP /Yc"Stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/chkbook.pch" /Yc"Stdafx.h" /Fo"$(INTDIR)/"\
 /c $(SOURCE) \
	

"$(INTDIR)\stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\chkbook.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "ChkBook - Win32 Debug"

# ADD CPP /Yc"Stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/chkbook.pch" /Yc"Stdafx.h"\
 /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\chkbook.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "ChkBook - Macintosh Release"

# ADD CPP /Yc"Stdafx.h"

BuildCmds= \
	$(CPP) /nologo /AL /Q68s /W3 /GX /O2 /D "_WINDOWS" /D "_MAC" /D "_68K_" /D\
 "WIN32" /D "NDEBUG" /D "_MBCS" /Fp"$(INTDIR)/chkbook.pch" /Yc"Stdafx.h"\
 /Fo"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\chkbook.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "ChkBook - Macintosh Debug"

# ADD CPP /Yc"Stdafx.h"

BuildCmds= \
	$(CPP) /nologo /AL /Q68s /Q68m /W3 /GX /Zi /Od /D "_WINDOWS" /D "_MAC" /D\
 "_68K_" /D "WIN32" /D "_DEBUG" /D "_MBCS" /Fp"$(INTDIR)/chkbook.pch"\
 /Yc"Stdafx.h" /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\chkbook.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "ChkBook - Power Macintosh Release"

# ADD CPP /Yc"Stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "_MAC" /D "_MPPC_" /D "_WINDOWS"\
 /D "WIN32" /D "_MBCS" /D "_AFXDLL" /Fp"$(INTDIR)/chkbook.pch" /Yc"Stdafx.h"\
 /Fo"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\chkbook.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "ChkBook - Power Macintosh Debug"

# ADD CPP /Yc"Stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MDd /W3 /GX /Zi /Od /D "_DEBUG" /D "_MAC" /D "_MPPC_" /D\
 "_WINDOWS" /D "WIN32" /D "_MBCS" /D "_AFXDLL" /Fp"$(INTDIR)/chkbook.pch"\
 /Yc"Stdafx.h" /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\chkbook.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "ChkBook - Win32 Unicode Release"

# ADD CPP /Yc"Stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D\
 "_UNICODE" /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/chkbook.pch" /Yc"Stdafx.h"\
 /Fo"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\chkbook.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "ChkBook - Win32 Unicode Debug"

# ADD CPP /Yc"Stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MDd /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS"\
 /D "_UNICODE" /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/chkbook.pch" /Yc"Stdafx.h"\
 /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\chkbook.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\checkdoc.cpp

!IF  "$(CFG)" == "ChkBook - Win32 Release"

DEP_CPP_CHECK=\
	".\chkbook.h"\
	".\bookvw.h"\
	".\checkdoc.h"\
	".\checkvw.h"\
	".\chkbkfrm.h"\
	".\dollcent.h"\
	".\fxrecdoc.h"\
	".\mainfrm.h"\
	".\rowview.h"\
	".\stdafx.h"\
	

"$(INTDIR)\checkdoc.obj" : $(SOURCE) $(DEP_CPP_CHECK) "$(INTDIR)"\
 "$(INTDIR)\chkbook.pch"


!ELSEIF  "$(CFG)" == "ChkBook - Win32 Debug"

DEP_CPP_CHECK=\
	".\stdafx.h"\
	".\chkbook.h"\
	".\mainfrm.h"\
	".\chkbkfrm.h"\
	".\dollcent.h"\
	".\fxrecdoc.h"\
	".\checkdoc.h"\
	".\checkvw.h"\
	".\rowview.h"\
	".\bookvw.h"\
	

"$(INTDIR)\checkdoc.obj" : $(SOURCE) $(DEP_CPP_CHECK) "$(INTDIR)"\
 "$(INTDIR)\chkbook.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_CHECK) "$(INTDIR)"\
 "$(INTDIR)\chkbook.pch"


!ELSEIF  "$(CFG)" == "ChkBook - Macintosh Release"

DEP_CPP_CHECK=\
	".\chkbook.h"\
	".\bookvw.h"\
	".\checkdoc.h"\
	".\checkvw.h"\
	".\chkbkfrm.h"\
	".\dollcent.h"\
	".\fxrecdoc.h"\
	".\mainfrm.h"\
	".\rowview.h"\
	".\stdafx.h"\
	

"$(INTDIR)\checkdoc.obj" : $(SOURCE) $(DEP_CPP_CHECK) "$(INTDIR)"\
 "$(INTDIR)\chkbook.pch"


!ELSEIF  "$(CFG)" == "ChkBook - Macintosh Debug"

DEP_CPP_CHECK=\
	".\chkbook.h"\
	".\bookvw.h"\
	".\checkdoc.h"\
	".\checkvw.h"\
	".\chkbkfrm.h"\
	".\dollcent.h"\
	".\fxrecdoc.h"\
	".\mainfrm.h"\
	".\rowview.h"\
	".\stdafx.h"\
	

"$(INTDIR)\checkdoc.obj" : $(SOURCE) $(DEP_CPP_CHECK) "$(INTDIR)"\
 "$(INTDIR)\chkbook.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_CHECK) "$(INTDIR)"\
 "$(INTDIR)\chkbook.pch"


!ELSEIF  "$(CFG)" == "ChkBook - Power Macintosh Release"

DEP_CPP_CHECK=\
	".\chkbook.h"\
	".\bookvw.h"\
	".\checkdoc.h"\
	".\checkvw.h"\
	".\chkbkfrm.h"\
	".\dollcent.h"\
	".\fxrecdoc.h"\
	".\mainfrm.h"\
	".\rowview.h"\
	".\stdafx.h"\
	

"$(INTDIR)\checkdoc.obj" : $(SOURCE) $(DEP_CPP_CHECK) "$(INTDIR)"\
 "$(INTDIR)\chkbook.pch"


!ELSEIF  "$(CFG)" == "ChkBook - Power Macintosh Debug"

DEP_CPP_CHECK=\
	".\chkbook.h"\
	".\bookvw.h"\
	".\checkdoc.h"\
	".\checkvw.h"\
	".\chkbkfrm.h"\
	".\dollcent.h"\
	".\fxrecdoc.h"\
	".\mainfrm.h"\
	".\rowview.h"\
	".\stdafx.h"\
	

"$(INTDIR)\checkdoc.obj" : $(SOURCE) $(DEP_CPP_CHECK) "$(INTDIR)"\
 "$(INTDIR)\chkbook.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_CHECK) "$(INTDIR)"\
 "$(INTDIR)\chkbook.pch"


!ELSEIF  "$(CFG)" == "ChkBook - Win32 Unicode Release"

DEP_CPP_CHECK=\
	".\stdafx.h"\
	".\chkbook.h"\
	".\mainfrm.h"\
	".\chkbkfrm.h"\
	".\dollcent.h"\
	".\fxrecdoc.h"\
	".\checkdoc.h"\
	".\checkvw.h"\
	".\rowview.h"\
	".\bookvw.h"\
	

"$(INTDIR)\checkdoc.obj" : $(SOURCE) $(DEP_CPP_CHECK) "$(INTDIR)"\
 "$(INTDIR)\chkbook.pch"


!ELSEIF  "$(CFG)" == "ChkBook - Win32 Unicode Debug"

DEP_CPP_CHECK=\
	".\stdafx.h"\
	".\chkbook.h"\
	".\mainfrm.h"\
	".\chkbkfrm.h"\
	".\dollcent.h"\
	".\fxrecdoc.h"\
	".\checkdoc.h"\
	".\checkvw.h"\
	".\rowview.h"\
	".\bookvw.h"\
	

"$(INTDIR)\checkdoc.obj" : $(SOURCE) $(DEP_CPP_CHECK) "$(INTDIR)"\
 "$(INTDIR)\chkbook.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_CHECK) "$(INTDIR)"\
 "$(INTDIR)\chkbook.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\checkvw.cpp

!IF  "$(CFG)" == "ChkBook - Win32 Release"

DEP_CPP_CHECKV=\
	".\chkbook.h"\
	".\bookvw.h"\
	".\checkdoc.h"\
	".\checkvw.h"\
	".\chkbkfrm.h"\
	".\dollcent.h"\
	".\fxrecdoc.h"\
	".\mainfrm.h"\
	".\rowview.h"\
	".\stdafx.h"\
	

"$(INTDIR)\checkvw.obj" : $(SOURCE) $(DEP_CPP_CHECKV) "$(INTDIR)"\
 "$(INTDIR)\chkbook.pch"


!ELSEIF  "$(CFG)" == "ChkBook - Win32 Debug"

DEP_CPP_CHECKV=\
	".\stdafx.h"\
	".\chkbook.h"\
	".\mainfrm.h"\
	".\chkbkfrm.h"\
	".\dollcent.h"\
	".\fxrecdoc.h"\
	".\checkdoc.h"\
	".\checkvw.h"\
	".\rowview.h"\
	".\bookvw.h"\
	

"$(INTDIR)\checkvw.obj" : $(SOURCE) $(DEP_CPP_CHECKV) "$(INTDIR)"\
 "$(INTDIR)\chkbook.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_CHECKV) "$(INTDIR)"\
 "$(INTDIR)\chkbook.pch"


!ELSEIF  "$(CFG)" == "ChkBook - Macintosh Release"

DEP_CPP_CHECKV=\
	".\chkbook.h"\
	".\bookvw.h"\
	".\checkdoc.h"\
	".\checkvw.h"\
	".\chkbkfrm.h"\
	".\dollcent.h"\
	".\fxrecdoc.h"\
	".\mainfrm.h"\
	".\rowview.h"\
	".\stdafx.h"\
	

"$(INTDIR)\checkvw.obj" : $(SOURCE) $(DEP_CPP_CHECKV) "$(INTDIR)"\
 "$(INTDIR)\chkbook.pch"


!ELSEIF  "$(CFG)" == "ChkBook - Macintosh Debug"

DEP_CPP_CHECKV=\
	".\chkbook.h"\
	".\bookvw.h"\
	".\checkdoc.h"\
	".\checkvw.h"\
	".\chkbkfrm.h"\
	".\dollcent.h"\
	".\fxrecdoc.h"\
	".\mainfrm.h"\
	".\rowview.h"\
	".\stdafx.h"\
	

"$(INTDIR)\checkvw.obj" : $(SOURCE) $(DEP_CPP_CHECKV) "$(INTDIR)"\
 "$(INTDIR)\chkbook.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_CHECKV) "$(INTDIR)"\
 "$(INTDIR)\chkbook.pch"


!ELSEIF  "$(CFG)" == "ChkBook - Power Macintosh Release"

DEP_CPP_CHECKV=\
	".\chkbook.h"\
	".\bookvw.h"\
	".\checkdoc.h"\
	".\checkvw.h"\
	".\chkbkfrm.h"\
	".\dollcent.h"\
	".\fxrecdoc.h"\
	".\mainfrm.h"\
	".\rowview.h"\
	".\stdafx.h"\
	

"$(INTDIR)\checkvw.obj" : $(SOURCE) $(DEP_CPP_CHECKV) "$(INTDIR)"\
 "$(INTDIR)\chkbook.pch"


!ELSEIF  "$(CFG)" == "ChkBook - Power Macintosh Debug"

DEP_CPP_CHECKV=\
	".\chkbook.h"\
	".\bookvw.h"\
	".\checkdoc.h"\
	".\checkvw.h"\
	".\chkbkfrm.h"\
	".\dollcent.h"\
	".\fxrecdoc.h"\
	".\mainfrm.h"\
	".\rowview.h"\
	".\stdafx.h"\
	

"$(INTDIR)\checkvw.obj" : $(SOURCE) $(DEP_CPP_CHECKV) "$(INTDIR)"\
 "$(INTDIR)\chkbook.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_CHECKV) "$(INTDIR)"\
 "$(INTDIR)\chkbook.pch"


!ELSEIF  "$(CFG)" == "ChkBook - Win32 Unicode Release"

DEP_CPP_CHECKV=\
	".\stdafx.h"\
	".\chkbook.h"\
	".\mainfrm.h"\
	".\chkbkfrm.h"\
	".\dollcent.h"\
	".\fxrecdoc.h"\
	".\checkdoc.h"\
	".\checkvw.h"\
	".\rowview.h"\
	".\bookvw.h"\
	

"$(INTDIR)\checkvw.obj" : $(SOURCE) $(DEP_CPP_CHECKV) "$(INTDIR)"\
 "$(INTDIR)\chkbook.pch"


!ELSEIF  "$(CFG)" == "ChkBook - Win32 Unicode Debug"

DEP_CPP_CHECKV=\
	".\stdafx.h"\
	".\chkbook.h"\
	".\mainfrm.h"\
	".\chkbkfrm.h"\
	".\dollcent.h"\
	".\fxrecdoc.h"\
	".\checkdoc.h"\
	".\checkvw.h"\
	".\rowview.h"\
	".\bookvw.h"\
	

"$(INTDIR)\checkvw.obj" : $(SOURCE) $(DEP_CPP_CHECKV) "$(INTDIR)"\
 "$(INTDIR)\chkbook.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_CHECKV) "$(INTDIR)"\
 "$(INTDIR)\chkbook.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\chkbkfrm.cpp
DEP_CPP_CHKBK=\
	".\chkbkfrm.h"\
	".\stdafx.h"\
	

!IF  "$(CFG)" == "ChkBook - Win32 Release"


"$(INTDIR)\chkbkfrm.obj" : $(SOURCE) $(DEP_CPP_CHKBK) "$(INTDIR)"\
 "$(INTDIR)\chkbook.pch"


!ELSEIF  "$(CFG)" == "ChkBook - Win32 Debug"


"$(INTDIR)\chkbkfrm.obj" : $(SOURCE) $(DEP_CPP_CHKBK) "$(INTDIR)"\
 "$(INTDIR)\chkbook.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_CHKBK) "$(INTDIR)"\
 "$(INTDIR)\chkbook.pch"


!ELSEIF  "$(CFG)" == "ChkBook - Macintosh Release"


"$(INTDIR)\chkbkfrm.obj" : $(SOURCE) $(DEP_CPP_CHKBK) "$(INTDIR)"\
 "$(INTDIR)\chkbook.pch"


!ELSEIF  "$(CFG)" == "ChkBook - Macintosh Debug"


"$(INTDIR)\chkbkfrm.obj" : $(SOURCE) $(DEP_CPP_CHKBK) "$(INTDIR)"\
 "$(INTDIR)\chkbook.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_CHKBK) "$(INTDIR)"\
 "$(INTDIR)\chkbook.pch"


!ELSEIF  "$(CFG)" == "ChkBook - Power Macintosh Release"


"$(INTDIR)\chkbkfrm.obj" : $(SOURCE) $(DEP_CPP_CHKBK) "$(INTDIR)"\
 "$(INTDIR)\chkbook.pch"


!ELSEIF  "$(CFG)" == "ChkBook - Power Macintosh Debug"


"$(INTDIR)\chkbkfrm.obj" : $(SOURCE) $(DEP_CPP_CHKBK) "$(INTDIR)"\
 "$(INTDIR)\chkbook.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_CHKBK) "$(INTDIR)"\
 "$(INTDIR)\chkbook.pch"


!ELSEIF  "$(CFG)" == "ChkBook - Win32 Unicode Release"


"$(INTDIR)\chkbkfrm.obj" : $(SOURCE) $(DEP_CPP_CHKBK) "$(INTDIR)"\
 "$(INTDIR)\chkbook.pch"


!ELSEIF  "$(CFG)" == "ChkBook - Win32 Unicode Debug"


"$(INTDIR)\chkbkfrm.obj" : $(SOURCE) $(DEP_CPP_CHKBK) "$(INTDIR)"\
 "$(INTDIR)\chkbook.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_CHKBK) "$(INTDIR)"\
 "$(INTDIR)\chkbook.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\chkbook.cpp

!IF  "$(CFG)" == "ChkBook - Win32 Release"

DEP_CPP_CHKBO=\
	".\chkbook.h"\
	".\bookvw.h"\
	".\checkdoc.h"\
	".\checkvw.h"\
	".\chkbkfrm.h"\
	".\dollcent.h"\
	".\fxrecdoc.h"\
	".\mainfrm.h"\
	".\rowview.h"\
	".\stdafx.h"\
	

"$(INTDIR)\chkbook.obj" : $(SOURCE) $(DEP_CPP_CHKBO) "$(INTDIR)"\
 "$(INTDIR)\chkbook.pch"


!ELSEIF  "$(CFG)" == "ChkBook - Win32 Debug"

DEP_CPP_CHKBO=\
	".\stdafx.h"\
	".\chkbook.h"\
	".\mainfrm.h"\
	".\chkbkfrm.h"\
	".\dollcent.h"\
	".\fxrecdoc.h"\
	".\checkdoc.h"\
	".\checkvw.h"\
	".\rowview.h"\
	".\bookvw.h"\
	

"$(INTDIR)\chkbook.obj" : $(SOURCE) $(DEP_CPP_CHKBO) "$(INTDIR)"\
 "$(INTDIR)\chkbook.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_CHKBO) "$(INTDIR)"\
 "$(INTDIR)\chkbook.pch"


!ELSEIF  "$(CFG)" == "ChkBook - Macintosh Release"

DEP_CPP_CHKBO=\
	".\chkbook.h"\
	".\bookvw.h"\
	".\checkdoc.h"\
	".\checkvw.h"\
	".\chkbkfrm.h"\
	".\dollcent.h"\
	".\fxrecdoc.h"\
	".\mainfrm.h"\
	".\rowview.h"\
	".\stdafx.h"\
	

"$(INTDIR)\chkbook.obj" : $(SOURCE) $(DEP_CPP_CHKBO) "$(INTDIR)"\
 "$(INTDIR)\chkbook.pch"


!ELSEIF  "$(CFG)" == "ChkBook - Macintosh Debug"

DEP_CPP_CHKBO=\
	".\chkbook.h"\
	".\bookvw.h"\
	".\checkdoc.h"\
	".\checkvw.h"\
	".\chkbkfrm.h"\
	".\dollcent.h"\
	".\fxrecdoc.h"\
	".\mainfrm.h"\
	".\rowview.h"\
	".\stdafx.h"\
	

"$(INTDIR)\chkbook.obj" : $(SOURCE) $(DEP_CPP_CHKBO) "$(INTDIR)"\
 "$(INTDIR)\chkbook.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_CHKBO) "$(INTDIR)"\
 "$(INTDIR)\chkbook.pch"


!ELSEIF  "$(CFG)" == "ChkBook - Power Macintosh Release"

DEP_CPP_CHKBO=\
	".\chkbook.h"\
	".\bookvw.h"\
	".\checkdoc.h"\
	".\checkvw.h"\
	".\chkbkfrm.h"\
	".\dollcent.h"\
	".\fxrecdoc.h"\
	".\mainfrm.h"\
	".\rowview.h"\
	".\stdafx.h"\
	

"$(INTDIR)\chkbook.obj" : $(SOURCE) $(DEP_CPP_CHKBO) "$(INTDIR)"\
 "$(INTDIR)\chkbook.pch"


!ELSEIF  "$(CFG)" == "ChkBook - Power Macintosh Debug"

DEP_CPP_CHKBO=\
	".\chkbook.h"\
	".\bookvw.h"\
	".\checkdoc.h"\
	".\checkvw.h"\
	".\chkbkfrm.h"\
	".\dollcent.h"\
	".\fxrecdoc.h"\
	".\mainfrm.h"\
	".\rowview.h"\
	".\stdafx.h"\
	

"$(INTDIR)\chkbook.obj" : $(SOURCE) $(DEP_CPP_CHKBO) "$(INTDIR)"\
 "$(INTDIR)\chkbook.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_CHKBO) "$(INTDIR)"\
 "$(INTDIR)\chkbook.pch"


!ELSEIF  "$(CFG)" == "ChkBook - Win32 Unicode Release"

DEP_CPP_CHKBO=\
	".\stdafx.h"\
	".\chkbook.h"\
	".\mainfrm.h"\
	".\chkbkfrm.h"\
	".\dollcent.h"\
	".\fxrecdoc.h"\
	".\checkdoc.h"\
	".\checkvw.h"\
	".\rowview.h"\
	".\bookvw.h"\
	

"$(INTDIR)\chkbook.obj" : $(SOURCE) $(DEP_CPP_CHKBO) "$(INTDIR)"\
 "$(INTDIR)\chkbook.pch"


!ELSEIF  "$(CFG)" == "ChkBook - Win32 Unicode Debug"

DEP_CPP_CHKBO=\
	".\stdafx.h"\
	".\chkbook.h"\
	".\mainfrm.h"\
	".\chkbkfrm.h"\
	".\dollcent.h"\
	".\fxrecdoc.h"\
	".\checkdoc.h"\
	".\checkvw.h"\
	".\rowview.h"\
	".\bookvw.h"\
	

"$(INTDIR)\chkbook.obj" : $(SOURCE) $(DEP_CPP_CHKBO) "$(INTDIR)"\
 "$(INTDIR)\chkbook.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_CHKBO) "$(INTDIR)"\
 "$(INTDIR)\chkbook.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\chkbook.rc
DEP_RSC_CHKBOO=\
	".\res\chkbook.ico"\
	".\res\doc.ico"\
	".\res\checkvw.ico"\
	".\res\toolbar.bmp"\
	

!IF  "$(CFG)" == "ChkBook - Win32 Release"


"$(INTDIR)\chkbook.res" : $(SOURCE) $(DEP_RSC_CHKBOO) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "ChkBook - Win32 Debug"


"$(INTDIR)\chkbook.res" : $(SOURCE) $(DEP_RSC_CHKBOO) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "ChkBook - Macintosh Release"


"$(INTDIR)\chkbook.rsc" : $(SOURCE) $(DEP_RSC_CHKBOO) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "ChkBook - Macintosh Debug"


"$(INTDIR)\chkbook.rsc" : $(SOURCE) $(DEP_RSC_CHKBOO) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "ChkBook - Power Macintosh Release"


"$(INTDIR)\chkbook.rsc" : $(SOURCE) $(DEP_RSC_CHKBOO) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "ChkBook - Power Macintosh Debug"


"$(INTDIR)\chkbook.rsc" : $(SOURCE) $(DEP_RSC_CHKBOO) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "ChkBook - Win32 Unicode Release"


"$(INTDIR)\chkbook.res" : $(SOURCE) $(DEP_RSC_CHKBOO) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "ChkBook - Win32 Unicode Debug"


"$(INTDIR)\chkbook.res" : $(SOURCE) $(DEP_RSC_CHKBOO) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\dollcent.cpp

!IF  "$(CFG)" == "ChkBook - Win32 Release"

DEP_CPP_DOLLC=\
	".\chkbook.h"\
	".\bookvw.h"\
	".\checkdoc.h"\
	".\checkvw.h"\
	".\chkbkfrm.h"\
	".\dollcent.h"\
	".\fxrecdoc.h"\
	".\mainfrm.h"\
	".\rowview.h"\
	".\stdafx.h"\
	

"$(INTDIR)\dollcent.obj" : $(SOURCE) $(DEP_CPP_DOLLC) "$(INTDIR)"\
 "$(INTDIR)\chkbook.pch"


!ELSEIF  "$(CFG)" == "ChkBook - Win32 Debug"

DEP_CPP_DOLLC=\
	".\stdafx.h"\
	".\chkbook.h"\
	".\mainfrm.h"\
	".\chkbkfrm.h"\
	".\dollcent.h"\
	".\fxrecdoc.h"\
	".\checkdoc.h"\
	".\checkvw.h"\
	".\rowview.h"\
	".\bookvw.h"\
	

"$(INTDIR)\dollcent.obj" : $(SOURCE) $(DEP_CPP_DOLLC) "$(INTDIR)"\
 "$(INTDIR)\chkbook.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_DOLLC) "$(INTDIR)"\
 "$(INTDIR)\chkbook.pch"


!ELSEIF  "$(CFG)" == "ChkBook - Macintosh Release"

DEP_CPP_DOLLC=\
	".\chkbook.h"\
	".\bookvw.h"\
	".\checkdoc.h"\
	".\checkvw.h"\
	".\chkbkfrm.h"\
	".\dollcent.h"\
	".\fxrecdoc.h"\
	".\mainfrm.h"\
	".\rowview.h"\
	".\stdafx.h"\
	

"$(INTDIR)\dollcent.obj" : $(SOURCE) $(DEP_CPP_DOLLC) "$(INTDIR)"\
 "$(INTDIR)\chkbook.pch"


!ELSEIF  "$(CFG)" == "ChkBook - Macintosh Debug"

DEP_CPP_DOLLC=\
	".\chkbook.h"\
	".\bookvw.h"\
	".\checkdoc.h"\
	".\checkvw.h"\
	".\chkbkfrm.h"\
	".\dollcent.h"\
	".\fxrecdoc.h"\
	".\mainfrm.h"\
	".\rowview.h"\
	".\stdafx.h"\
	

"$(INTDIR)\dollcent.obj" : $(SOURCE) $(DEP_CPP_DOLLC) "$(INTDIR)"\
 "$(INTDIR)\chkbook.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_DOLLC) "$(INTDIR)"\
 "$(INTDIR)\chkbook.pch"


!ELSEIF  "$(CFG)" == "ChkBook - Power Macintosh Release"

DEP_CPP_DOLLC=\
	".\chkbook.h"\
	".\bookvw.h"\
	".\checkdoc.h"\
	".\checkvw.h"\
	".\chkbkfrm.h"\
	".\dollcent.h"\
	".\fxrecdoc.h"\
	".\mainfrm.h"\
	".\rowview.h"\
	".\stdafx.h"\
	

"$(INTDIR)\dollcent.obj" : $(SOURCE) $(DEP_CPP_DOLLC) "$(INTDIR)"\
 "$(INTDIR)\chkbook.pch"


!ELSEIF  "$(CFG)" == "ChkBook - Power Macintosh Debug"

DEP_CPP_DOLLC=\
	".\chkbook.h"\
	".\bookvw.h"\
	".\checkdoc.h"\
	".\checkvw.h"\
	".\chkbkfrm.h"\
	".\dollcent.h"\
	".\fxrecdoc.h"\
	".\mainfrm.h"\
	".\rowview.h"\
	".\stdafx.h"\
	

"$(INTDIR)\dollcent.obj" : $(SOURCE) $(DEP_CPP_DOLLC) "$(INTDIR)"\
 "$(INTDIR)\chkbook.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_DOLLC) "$(INTDIR)"\
 "$(INTDIR)\chkbook.pch"


!ELSEIF  "$(CFG)" == "ChkBook - Win32 Unicode Release"

DEP_CPP_DOLLC=\
	".\stdafx.h"\
	".\chkbook.h"\
	".\mainfrm.h"\
	".\chkbkfrm.h"\
	".\dollcent.h"\
	".\fxrecdoc.h"\
	".\checkdoc.h"\
	".\checkvw.h"\
	".\rowview.h"\
	".\bookvw.h"\
	

"$(INTDIR)\dollcent.obj" : $(SOURCE) $(DEP_CPP_DOLLC) "$(INTDIR)"\
 "$(INTDIR)\chkbook.pch"


!ELSEIF  "$(CFG)" == "ChkBook - Win32 Unicode Debug"

DEP_CPP_DOLLC=\
	".\stdafx.h"\
	".\chkbook.h"\
	".\mainfrm.h"\
	".\chkbkfrm.h"\
	".\dollcent.h"\
	".\fxrecdoc.h"\
	".\checkdoc.h"\
	".\checkvw.h"\
	".\rowview.h"\
	".\bookvw.h"\
	

"$(INTDIR)\dollcent.obj" : $(SOURCE) $(DEP_CPP_DOLLC) "$(INTDIR)"\
 "$(INTDIR)\chkbook.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_DOLLC) "$(INTDIR)"\
 "$(INTDIR)\chkbook.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\fxrecdoc.cpp
DEP_CPP_FXREC=\
	".\fxrecdoc.h"\
	".\stdafx.h"\
	

!IF  "$(CFG)" == "ChkBook - Win32 Release"


"$(INTDIR)\fxrecdoc.obj" : $(SOURCE) $(DEP_CPP_FXREC) "$(INTDIR)"\
 "$(INTDIR)\chkbook.pch"


!ELSEIF  "$(CFG)" == "ChkBook - Win32 Debug"


"$(INTDIR)\fxrecdoc.obj" : $(SOURCE) $(DEP_CPP_FXREC) "$(INTDIR)"\
 "$(INTDIR)\chkbook.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_FXREC) "$(INTDIR)"\
 "$(INTDIR)\chkbook.pch"


!ELSEIF  "$(CFG)" == "ChkBook - Macintosh Release"


"$(INTDIR)\fxrecdoc.obj" : $(SOURCE) $(DEP_CPP_FXREC) "$(INTDIR)"\
 "$(INTDIR)\chkbook.pch"


!ELSEIF  "$(CFG)" == "ChkBook - Macintosh Debug"


"$(INTDIR)\fxrecdoc.obj" : $(SOURCE) $(DEP_CPP_FXREC) "$(INTDIR)"\
 "$(INTDIR)\chkbook.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_FXREC) "$(INTDIR)"\
 "$(INTDIR)\chkbook.pch"


!ELSEIF  "$(CFG)" == "ChkBook - Power Macintosh Release"


"$(INTDIR)\fxrecdoc.obj" : $(SOURCE) $(DEP_CPP_FXREC) "$(INTDIR)"\
 "$(INTDIR)\chkbook.pch"


!ELSEIF  "$(CFG)" == "ChkBook - Power Macintosh Debug"


"$(INTDIR)\fxrecdoc.obj" : $(SOURCE) $(DEP_CPP_FXREC) "$(INTDIR)"\
 "$(INTDIR)\chkbook.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_FXREC) "$(INTDIR)"\
 "$(INTDIR)\chkbook.pch"


!ELSEIF  "$(CFG)" == "ChkBook - Win32 Unicode Release"


"$(INTDIR)\fxrecdoc.obj" : $(SOURCE) $(DEP_CPP_FXREC) "$(INTDIR)"\
 "$(INTDIR)\chkbook.pch"


!ELSEIF  "$(CFG)" == "ChkBook - Win32 Unicode Debug"


"$(INTDIR)\fxrecdoc.obj" : $(SOURCE) $(DEP_CPP_FXREC) "$(INTDIR)"\
 "$(INTDIR)\chkbook.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_FXREC) "$(INTDIR)"\
 "$(INTDIR)\chkbook.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\mainfrm.cpp

!IF  "$(CFG)" == "ChkBook - Win32 Release"

DEP_CPP_MAINF=\
	".\chkbook.h"\
	".\bookvw.h"\
	".\checkdoc.h"\
	".\checkvw.h"\
	".\chkbkfrm.h"\
	".\dollcent.h"\
	".\fxrecdoc.h"\
	".\mainfrm.h"\
	".\rowview.h"\
	".\stdafx.h"\
	

"$(INTDIR)\mainfrm.obj" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\chkbook.pch"


!ELSEIF  "$(CFG)" == "ChkBook - Win32 Debug"

DEP_CPP_MAINF=\
	".\stdafx.h"\
	".\chkbook.h"\
	".\mainfrm.h"\
	".\chkbkfrm.h"\
	".\dollcent.h"\
	".\fxrecdoc.h"\
	".\checkdoc.h"\
	".\checkvw.h"\
	".\rowview.h"\
	".\bookvw.h"\
	

"$(INTDIR)\mainfrm.obj" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\chkbook.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\chkbook.pch"


!ELSEIF  "$(CFG)" == "ChkBook - Macintosh Release"

DEP_CPP_MAINF=\
	".\chkbook.h"\
	".\bookvw.h"\
	".\checkdoc.h"\
	".\checkvw.h"\
	".\chkbkfrm.h"\
	".\dollcent.h"\
	".\fxrecdoc.h"\
	".\mainfrm.h"\
	".\rowview.h"\
	".\stdafx.h"\
	

"$(INTDIR)\mainfrm.obj" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\chkbook.pch"


!ELSEIF  "$(CFG)" == "ChkBook - Macintosh Debug"

DEP_CPP_MAINF=\
	".\chkbook.h"\
	".\bookvw.h"\
	".\checkdoc.h"\
	".\checkvw.h"\
	".\chkbkfrm.h"\
	".\dollcent.h"\
	".\fxrecdoc.h"\
	".\mainfrm.h"\
	".\rowview.h"\
	".\stdafx.h"\
	

"$(INTDIR)\mainfrm.obj" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\chkbook.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\chkbook.pch"


!ELSEIF  "$(CFG)" == "ChkBook - Power Macintosh Release"

DEP_CPP_MAINF=\
	".\stdafx.h"\
	".\chkbook.h"\
	".\mainfrm.h"\
	".\chkbkfrm.h"\
	".\dollcent.h"\
	".\fxrecdoc.h"\
	".\checkdoc.h"\
	".\checkvw.h"\
	".\rowview.h"\
	".\bookvw.h"\
	

"$(INTDIR)\mainfrm.obj" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\chkbook.pch"


!ELSEIF  "$(CFG)" == "ChkBook - Power Macintosh Debug"

DEP_CPP_MAINF=\
	".\chkbook.h"\
	".\bookvw.h"\
	".\checkdoc.h"\
	".\checkvw.h"\
	".\chkbkfrm.h"\
	".\dollcent.h"\
	".\fxrecdoc.h"\
	".\mainfrm.h"\
	".\rowview.h"\
	".\stdafx.h"\
	

"$(INTDIR)\mainfrm.obj" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\chkbook.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\chkbook.pch"


!ELSEIF  "$(CFG)" == "ChkBook - Win32 Unicode Release"

DEP_CPP_MAINF=\
	".\stdafx.h"\
	".\chkbook.h"\
	".\mainfrm.h"\
	".\chkbkfrm.h"\
	".\dollcent.h"\
	".\fxrecdoc.h"\
	".\checkdoc.h"\
	".\checkvw.h"\
	".\rowview.h"\
	".\bookvw.h"\
	

"$(INTDIR)\mainfrm.obj" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\chkbook.pch"


!ELSEIF  "$(CFG)" == "ChkBook - Win32 Unicode Debug"

DEP_CPP_MAINF=\
	".\stdafx.h"\
	".\chkbook.h"\
	".\mainfrm.h"\
	".\chkbkfrm.h"\
	".\dollcent.h"\
	".\fxrecdoc.h"\
	".\checkdoc.h"\
	".\checkvw.h"\
	".\rowview.h"\
	".\bookvw.h"\
	

"$(INTDIR)\mainfrm.obj" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\chkbook.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\chkbook.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\rowview.cpp

!IF  "$(CFG)" == "ChkBook - Win32 Release"

DEP_CPP_ROWVI=\
	".\chkbook.h"\
	".\bookvw.h"\
	".\checkdoc.h"\
	".\checkvw.h"\
	".\chkbkfrm.h"\
	".\dollcent.h"\
	".\fxrecdoc.h"\
	".\mainfrm.h"\
	".\rowview.h"\
	".\stdafx.h"\
	

"$(INTDIR)\rowview.obj" : $(SOURCE) $(DEP_CPP_ROWVI) "$(INTDIR)"\
 "$(INTDIR)\chkbook.pch"


!ELSEIF  "$(CFG)" == "ChkBook - Win32 Debug"

DEP_CPP_ROWVI=\
	".\stdafx.h"\
	".\chkbook.h"\
	".\mainfrm.h"\
	".\chkbkfrm.h"\
	".\dollcent.h"\
	".\fxrecdoc.h"\
	".\checkdoc.h"\
	".\checkvw.h"\
	".\rowview.h"\
	".\bookvw.h"\
	

"$(INTDIR)\rowview.obj" : $(SOURCE) $(DEP_CPP_ROWVI) "$(INTDIR)"\
 "$(INTDIR)\chkbook.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_ROWVI) "$(INTDIR)"\
 "$(INTDIR)\chkbook.pch"


!ELSEIF  "$(CFG)" == "ChkBook - Macintosh Release"

DEP_CPP_ROWVI=\
	".\chkbook.h"\
	".\bookvw.h"\
	".\checkdoc.h"\
	".\checkvw.h"\
	".\chkbkfrm.h"\
	".\dollcent.h"\
	".\fxrecdoc.h"\
	".\mainfrm.h"\
	".\rowview.h"\
	".\stdafx.h"\
	

"$(INTDIR)\rowview.obj" : $(SOURCE) $(DEP_CPP_ROWVI) "$(INTDIR)"\
 "$(INTDIR)\chkbook.pch"


!ELSEIF  "$(CFG)" == "ChkBook - Macintosh Debug"

DEP_CPP_ROWVI=\
	".\chkbook.h"\
	".\bookvw.h"\
	".\checkdoc.h"\
	".\checkvw.h"\
	".\chkbkfrm.h"\
	".\dollcent.h"\
	".\fxrecdoc.h"\
	".\mainfrm.h"\
	".\rowview.h"\
	".\stdafx.h"\
	

"$(INTDIR)\rowview.obj" : $(SOURCE) $(DEP_CPP_ROWVI) "$(INTDIR)"\
 "$(INTDIR)\chkbook.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_ROWVI) "$(INTDIR)"\
 "$(INTDIR)\chkbook.pch"


!ELSEIF  "$(CFG)" == "ChkBook - Power Macintosh Release"

DEP_CPP_ROWVI=\
	".\stdafx.h"\
	".\chkbook.h"\
	".\mainfrm.h"\
	".\chkbkfrm.h"\
	".\dollcent.h"\
	".\fxrecdoc.h"\
	".\checkdoc.h"\
	".\checkvw.h"\
	".\rowview.h"\
	".\bookvw.h"\
	

"$(INTDIR)\rowview.obj" : $(SOURCE) $(DEP_CPP_ROWVI) "$(INTDIR)"\
 "$(INTDIR)\chkbook.pch"


!ELSEIF  "$(CFG)" == "ChkBook - Power Macintosh Debug"

DEP_CPP_ROWVI=\
	".\chkbook.h"\
	".\bookvw.h"\
	".\checkdoc.h"\
	".\checkvw.h"\
	".\chkbkfrm.h"\
	".\dollcent.h"\
	".\fxrecdoc.h"\
	".\mainfrm.h"\
	".\rowview.h"\
	".\stdafx.h"\
	

"$(INTDIR)\rowview.obj" : $(SOURCE) $(DEP_CPP_ROWVI) "$(INTDIR)"\
 "$(INTDIR)\chkbook.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_ROWVI) "$(INTDIR)"\
 "$(INTDIR)\chkbook.pch"


!ELSEIF  "$(CFG)" == "ChkBook - Win32 Unicode Release"

DEP_CPP_ROWVI=\
	".\stdafx.h"\
	".\chkbook.h"\
	".\mainfrm.h"\
	".\chkbkfrm.h"\
	".\dollcent.h"\
	".\fxrecdoc.h"\
	".\checkdoc.h"\
	".\checkvw.h"\
	".\rowview.h"\
	".\bookvw.h"\
	

"$(INTDIR)\rowview.obj" : $(SOURCE) $(DEP_CPP_ROWVI) "$(INTDIR)"\
 "$(INTDIR)\chkbook.pch"


!ELSEIF  "$(CFG)" == "ChkBook - Win32 Unicode Debug"

DEP_CPP_ROWVI=\
	".\stdafx.h"\
	".\chkbook.h"\
	".\mainfrm.h"\
	".\chkbkfrm.h"\
	".\dollcent.h"\
	".\fxrecdoc.h"\
	".\checkdoc.h"\
	".\checkvw.h"\
	".\rowview.h"\
	".\bookvw.h"\
	

"$(INTDIR)\rowview.obj" : $(SOURCE) $(DEP_CPP_ROWVI) "$(INTDIR)"\
 "$(INTDIR)\chkbook.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_ROWVI) "$(INTDIR)"\
 "$(INTDIR)\chkbook.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\bookvw.cpp

!IF  "$(CFG)" == "ChkBook - Win32 Release"

DEP_CPP_BOOKV=\
	".\chkbook.h"\
	".\bookvw.h"\
	".\checkdoc.h"\
	".\checkvw.h"\
	".\chkbkfrm.h"\
	".\dollcent.h"\
	".\fxrecdoc.h"\
	".\mainfrm.h"\
	".\rowview.h"\
	".\stdafx.h"\
	

"$(INTDIR)\bookvw.obj" : $(SOURCE) $(DEP_CPP_BOOKV) "$(INTDIR)"\
 "$(INTDIR)\chkbook.pch"


!ELSEIF  "$(CFG)" == "ChkBook - Win32 Debug"

DEP_CPP_BOOKV=\
	".\stdafx.h"\
	".\chkbook.h"\
	".\mainfrm.h"\
	".\chkbkfrm.h"\
	".\dollcent.h"\
	".\fxrecdoc.h"\
	".\checkdoc.h"\
	".\checkvw.h"\
	".\rowview.h"\
	".\bookvw.h"\
	

"$(INTDIR)\bookvw.obj" : $(SOURCE) $(DEP_CPP_BOOKV) "$(INTDIR)"\
 "$(INTDIR)\chkbook.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_BOOKV) "$(INTDIR)"\
 "$(INTDIR)\chkbook.pch"


!ELSEIF  "$(CFG)" == "ChkBook - Macintosh Release"

DEP_CPP_BOOKV=\
	".\chkbook.h"\
	".\bookvw.h"\
	".\checkdoc.h"\
	".\checkvw.h"\
	".\chkbkfrm.h"\
	".\dollcent.h"\
	".\fxrecdoc.h"\
	".\mainfrm.h"\
	".\rowview.h"\
	".\stdafx.h"\
	

"$(INTDIR)\bookvw.obj" : $(SOURCE) $(DEP_CPP_BOOKV) "$(INTDIR)"\
 "$(INTDIR)\chkbook.pch"


!ELSEIF  "$(CFG)" == "ChkBook - Macintosh Debug"

DEP_CPP_BOOKV=\
	".\chkbook.h"\
	".\bookvw.h"\
	".\checkdoc.h"\
	".\checkvw.h"\
	".\chkbkfrm.h"\
	".\dollcent.h"\
	".\fxrecdoc.h"\
	".\mainfrm.h"\
	".\rowview.h"\
	".\stdafx.h"\
	

"$(INTDIR)\bookvw.obj" : $(SOURCE) $(DEP_CPP_BOOKV) "$(INTDIR)"\
 "$(INTDIR)\chkbook.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_BOOKV) "$(INTDIR)"\
 "$(INTDIR)\chkbook.pch"


!ELSEIF  "$(CFG)" == "ChkBook - Power Macintosh Release"

DEP_CPP_BOOKV=\
	".\stdafx.h"\
	".\chkbook.h"\
	".\mainfrm.h"\
	".\chkbkfrm.h"\
	".\dollcent.h"\
	".\fxrecdoc.h"\
	".\checkdoc.h"\
	".\checkvw.h"\
	".\rowview.h"\
	".\bookvw.h"\
	

"$(INTDIR)\bookvw.obj" : $(SOURCE) $(DEP_CPP_BOOKV) "$(INTDIR)"\
 "$(INTDIR)\chkbook.pch"


!ELSEIF  "$(CFG)" == "ChkBook - Power Macintosh Debug"

DEP_CPP_BOOKV=\
	".\chkbook.h"\
	".\bookvw.h"\
	".\checkdoc.h"\
	".\checkvw.h"\
	".\chkbkfrm.h"\
	".\dollcent.h"\
	".\fxrecdoc.h"\
	".\mainfrm.h"\
	".\rowview.h"\
	".\stdafx.h"\
	

"$(INTDIR)\bookvw.obj" : $(SOURCE) $(DEP_CPP_BOOKV) "$(INTDIR)"\
 "$(INTDIR)\chkbook.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_BOOKV) "$(INTDIR)"\
 "$(INTDIR)\chkbook.pch"


!ELSEIF  "$(CFG)" == "ChkBook - Win32 Unicode Release"

DEP_CPP_BOOKV=\
	".\stdafx.h"\
	".\chkbook.h"\
	".\mainfrm.h"\
	".\chkbkfrm.h"\
	".\dollcent.h"\
	".\fxrecdoc.h"\
	".\checkdoc.h"\
	".\checkvw.h"\
	".\rowview.h"\
	".\bookvw.h"\
	

"$(INTDIR)\bookvw.obj" : $(SOURCE) $(DEP_CPP_BOOKV) "$(INTDIR)"\
 "$(INTDIR)\chkbook.pch"


!ELSEIF  "$(CFG)" == "ChkBook - Win32 Unicode Debug"

DEP_CPP_BOOKV=\
	".\stdafx.h"\
	".\chkbook.h"\
	".\mainfrm.h"\
	".\chkbkfrm.h"\
	".\dollcent.h"\
	".\fxrecdoc.h"\
	".\checkdoc.h"\
	".\checkvw.h"\
	".\rowview.h"\
	".\bookvw.h"\
	

"$(INTDIR)\bookvw.obj" : $(SOURCE) $(DEP_CPP_BOOKV) "$(INTDIR)"\
 "$(INTDIR)\chkbook.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_BOOKV) "$(INTDIR)"\
 "$(INTDIR)\chkbook.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\chkbmac.r

!IF  "$(CFG)" == "ChkBook - Win32 Release"

!ELSEIF  "$(CFG)" == "ChkBook - Win32 Debug"

!ELSEIF  "$(CFG)" == "ChkBook - Macintosh Release"

DEP_MRC_CHKBM=\
	{$(INCLUDE)}"\mrc\types.r"\
	{$(INCLUDE)}"\mrc\balloons.r"\
	

"$(INTDIR)\chkbmac.rsc" : $(SOURCE) $(DEP_MRC_CHKBM) "$(INTDIR)"
   $(MRC) /o"$(INTDIR)/chkbmac.rsc" /D "_68K_" /D "_MAC" /D "NDEBUG" /l 0x409\
 /NOLOGO $(SOURCE)


!ELSEIF  "$(CFG)" == "ChkBook - Macintosh Debug"

DEP_MRC_CHKBM=\
	{$(INCLUDE)}"\mrc\types.r"\
	{$(INCLUDE)}"\mrc\balloons.r"\
	

"$(INTDIR)\chkbmac.rsc" : $(SOURCE) $(DEP_MRC_CHKBM) "$(INTDIR)"
   $(MRC) /o"$(INTDIR)/chkbmac.rsc" /D "_68K_" /D "_MAC" /D "_DEBUG" /l 0x409\
 /NOLOGO $(SOURCE)


!ELSEIF  "$(CFG)" == "ChkBook - Power Macintosh Release"

DEP_MRC_CHKBM=\
	{$(INCLUDE)}"\mrc\types.r"\
	{$(INCLUDE)}"\mrc\balloons.r"\
	

"$(INTDIR)\chkbmac.rsc" : $(SOURCE) $(DEP_MRC_CHKBM) "$(INTDIR)"
   $(MRC) /o"$(INTDIR)/chkbmac.rsc" /D "_MPPC_" /D "_MAC" /D "NDEBUG" /l 0x409\
 /NOLOGO $(SOURCE)


!ELSEIF  "$(CFG)" == "ChkBook - Power Macintosh Debug"

DEP_MRC_CHKBM=\
	{$(INCLUDE)}"\mrc\types.r"\
	{$(INCLUDE)}"\mrc\balloons.r"\
	

"$(INTDIR)\chkbmac.rsc" : $(SOURCE) $(DEP_MRC_CHKBM) "$(INTDIR)"
   $(MRC) /o"$(INTDIR)/chkbmac.rsc" /D "_MPPC_" /D "_MAC" /D "_DEBUG" /l 0x409\
 /NOLOGO $(SOURCE)


!ELSEIF  "$(CFG)" == "ChkBook - Win32 Unicode Release"

!ELSEIF  "$(CFG)" == "ChkBook - Win32 Unicode Debug"

!ENDIF 

# End Source File
# End Target
# End Project
################################################################################
