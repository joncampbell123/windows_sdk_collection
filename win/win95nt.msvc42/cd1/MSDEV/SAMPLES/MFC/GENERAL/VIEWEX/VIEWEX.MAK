# Microsoft Visual C++ Generated NMAKE File, Format Version 40000
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101
# TARGTYPE "Power Macintosh Application" 0x0401
# TARGTYPE "Macintosh Application" 0x0301

!IF "$(CFG)" == ""
CFG=Viewex - Win32 Debug
!MESSAGE No configuration specified.  Defaulting to Viewex - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "Viewex - Win32 Release" && "$(CFG)" != "Viewex - Win32 Debug"\
 && "$(CFG)" != "Viewex - Macintosh Release" && "$(CFG)" !=\
 "Viewex - Macintosh Debug" && "$(CFG)" != "Viewex - Power Macintosh Release" &&\
 "$(CFG)" != "Viewex - Power Macintosh Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "Viewex.mak" CFG="Viewex - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Viewex - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "Viewex - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE "Viewex - Macintosh Release" (based on "Macintosh Application")
!MESSAGE "Viewex - Macintosh Debug" (based on "Macintosh Application")
!MESSAGE "Viewex - Power Macintosh Release" (based on\
 "Power Macintosh Application")
!MESSAGE "Viewex - Power Macintosh Debug" (based on\
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
# PROP Target_Last_Scanned "Viewex - Win32 Debug"

!IF  "$(CFG)" == "Viewex - Win32 Release"

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

ALL : "$(OUTDIR)\Viewex.exe"

CLEAN : 
	-@erase ".\Release\vc40.idb"
	-@erase ".\Release\Viewex.pch"
	-@erase ".\Release\Viewex.exe"
	-@erase ".\Release\Inputvw.obj"
	-@erase ".\Release\Viewex.obj"
	-@erase ".\Release\Splitter.obj"
	-@erase ".\Release\Maindoc.obj"
	-@erase ".\Release\Enterdlg.obj"
	-@erase ".\Release\Stdafx.obj"
	-@erase ".\Release\Simpvw.obj"
	-@erase ".\Release\Viewex.res"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"Stdafx.h" /c
# SUBTRACT CPP /Fr
CPP_PROJ=/nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/Viewex.pch" /Yu"Stdafx.h" /Fo"$(INTDIR)/"\
 /Fd"$(INTDIR)/" /c 
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
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/Viewex.res" /d "NDEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/Viewex.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 /nologo /subsystem:windows /machine:I386
LINK32_FLAGS=/nologo /subsystem:windows /incremental:no\
 /pdb:"$(OUTDIR)/Viewex.pdb" /machine:I386 /out:"$(OUTDIR)/Viewex.exe" 
LINK32_OBJS= \
	"$(INTDIR)/Inputvw.obj" \
	"$(INTDIR)/Viewex.obj" \
	"$(INTDIR)/Splitter.obj" \
	"$(INTDIR)/Maindoc.obj" \
	"$(INTDIR)/Enterdlg.obj" \
	"$(INTDIR)/Stdafx.obj" \
	"$(INTDIR)/Simpvw.obj" \
	"$(INTDIR)/Viewex.res"

"$(OUTDIR)\Viewex.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "Viewex - Win32 Debug"

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

ALL : "$(OUTDIR)\Viewex.exe"

CLEAN : 
	-@erase ".\Debug\vc40.pdb"
	-@erase ".\Debug\Viewex.pch"
	-@erase ".\Debug\vc40.idb"
	-@erase ".\Debug\Viewex.exe"
	-@erase ".\Debug\Inputvw.obj"
	-@erase ".\Debug\Splitter.obj"
	-@erase ".\Debug\Stdafx.obj"
	-@erase ".\Debug\Simpvw.obj"
	-@erase ".\Debug\Maindoc.obj"
	-@erase ".\Debug\Enterdlg.obj"
	-@erase ".\Debug\Viewex.obj"
	-@erase ".\Debug\Viewex.res"
	-@erase ".\Debug\Viewex.ilk"
	-@erase ".\Debug\Viewex.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"Stdafx.h" /c
# SUBTRACT CPP /Fr
CPP_PROJ=/nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/Viewex.pch" /Yu"Stdafx.h" /Fo"$(INTDIR)/"\
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
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/Viewex.res" /d "_DEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/Viewex.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386
# ADD LINK32 /nologo /subsystem:windows /debug /machine:I386
LINK32_FLAGS=/nologo /subsystem:windows /incremental:yes\
 /pdb:"$(OUTDIR)/Viewex.pdb" /debug /machine:I386 /out:"$(OUTDIR)/Viewex.exe" 
LINK32_OBJS= \
	"$(INTDIR)/Inputvw.obj" \
	"$(INTDIR)/Splitter.obj" \
	"$(INTDIR)/Stdafx.obj" \
	"$(INTDIR)/Simpvw.obj" \
	"$(INTDIR)/Maindoc.obj" \
	"$(INTDIR)/Enterdlg.obj" \
	"$(INTDIR)/Viewex.obj" \
	"$(INTDIR)/Viewex.res"

"$(OUTDIR)\Viewex.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "Viewex - Macintosh Release"

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

ALL : "$(OUTDIR)\Viewex.exe" "$(OUTDIR)\Viewex.trg"

CLEAN : 
	-@erase ".\MacRel\Viewex.trg"
	-@erase ".\MacRel\Viewex.exe"
	-@erase ".\MacRel\Stdafx.obj"
	-@erase ".\MacRel\Simpvw.obj"
	-@erase ".\MacRel\Viewex.pch"
	-@erase ".\MacRel\Inputvw.obj"
	-@erase ".\MacRel\Splitter.obj"
	-@erase ".\MacRel\Viewex.obj"
	-@erase ".\MacRel\Maindoc.obj"
	-@erase ".\MacRel\Enterdlg.obj"
	-@erase ".\MacRel\viewmac.rsc"
	-@erase ".\MacRel\Viewex.rsc"

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
 "WIN32" /D "NDEBUG" /D "_MBCS" /Fp"$(INTDIR)/Viewex.pch" /Yu"Stdafx.h"\
 /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
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
RSC_PROJ=/l 0x409 /r /m /fo"$(INTDIR)/Viewex.rsc" /d "_MAC" /d "_68K_" /d\
 "NDEBUG" 
MRC=mrc.exe
# ADD BASE MRC /D "_68K_" /D "_MAC" /D "NDEBUG" /NOLOGO
# ADD MRC /D "_68K_" /D "_MAC" /D "NDEBUG" /NOLOGO
MRC_PROJ=/D "_68K_" /D "_MAC" /D "NDEBUG" /l 0x409 /NOLOGO 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/Viewex.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 wlm.lib /nologo /MAC:bundle /machine:M68K
# ADD LINK32 /nologo /MAC:bundle /MAC:creator="VIEW" /machine:M68K
LINK32_FLAGS=/nologo /MAC:bundle /MAC:type="APPL" /MAC:creator="VIEW"\
 /pdb:"$(OUTDIR)/Viewex.pdb" /machine:M68K /out:"$(OUTDIR)/Viewex.exe" 
LINK32_OBJS= \
	"$(INTDIR)/Stdafx.obj" \
	"$(INTDIR)/Simpvw.obj" \
	"$(INTDIR)/Inputvw.obj" \
	"$(INTDIR)/Splitter.obj" \
	"$(INTDIR)/Viewex.obj" \
	"$(INTDIR)/Maindoc.obj" \
	"$(INTDIR)/Enterdlg.obj" \
	"$(INTDIR)/viewmac.rsc" \
	"$(INTDIR)/Viewex.rsc"

"$(OUTDIR)\Viewex.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

MFILE32=mfile.exe
# ADD BASE MFILE32 COPY /NOLOGO
# ADD MFILE32 COPY /NOLOGO
MFILE32_FLAGS=COPY /NOLOGO 
MFILE32_FILES= \
	"$(OUTDIR)/Viewex.exe"

"$(OUTDIR)\Viewex.trg" : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\MacRel\Viewex.exe "$(MFILE32_DEST)"

DOWNLOAD : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\MacRel\Viewex.exe "$(MFILE32_DEST)"

!ELSEIF  "$(CFG)" == "Viewex - Macintosh Debug"

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

ALL : "$(OUTDIR)\Viewex.exe" "$(OUTDIR)\Viewex.trg"

CLEAN : 
	-@erase ".\MacDbg\vc40.pdb"
	-@erase ".\MacDbg\Viewex.pch"
	-@erase ".\MacDbg\Viewex.trg"
	-@erase ".\MacDbg\Viewex.exe"
	-@erase ".\MacDbg\Stdafx.obj"
	-@erase ".\MacDbg\Simpvw.obj"
	-@erase ".\MacDbg\Splitter.obj"
	-@erase ".\MacDbg\Viewex.obj"
	-@erase ".\MacDbg\Inputvw.obj"
	-@erase ".\MacDbg\Enterdlg.obj"
	-@erase ".\MacDbg\Maindoc.obj"
	-@erase ".\MacDbg\Viewex.rsc"
	-@erase ".\MacDbg\viewmac.rsc"
	-@erase ".\MacDbg\Viewex.pdb"

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
 "_68K_" /D "WIN32" /D "_DEBUG" /D "_MBCS" /Fp"$(INTDIR)/Viewex.pch"\
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
RSC_PROJ=/l 0x409 /r /m /fo"$(INTDIR)/Viewex.rsc" /d "_MAC" /d "_68K_" /d\
 "_DEBUG" 
MRC=mrc.exe
# ADD BASE MRC /D "_68K_" /D "_MAC" /D "_DEBUG" /NOLOGO
# ADD MRC /D "_68K_" /D "_MAC" /D "_DEBUG" /NOLOGO
MRC_PROJ=/D "_68K_" /D "_MAC" /D "_DEBUG" /l 0x409 /NOLOGO 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/Viewex.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 wlm.lib /nologo /MAC:bundle /debug /machine:M68K
# ADD LINK32 /nologo /MAC:bundle /MAC:creator="VIEW" /debug /machine:M68K
LINK32_FLAGS=/nologo /MAC:bundle /MAC:type="APPL" /MAC:creator="VIEW"\
 /pdb:"$(OUTDIR)/Viewex.pdb" /debug /machine:M68K /out:"$(OUTDIR)/Viewex.exe" 
LINK32_OBJS= \
	"$(INTDIR)/Stdafx.obj" \
	"$(INTDIR)/Simpvw.obj" \
	"$(INTDIR)/Splitter.obj" \
	"$(INTDIR)/Viewex.obj" \
	"$(INTDIR)/Inputvw.obj" \
	"$(INTDIR)/Enterdlg.obj" \
	"$(INTDIR)/Maindoc.obj" \
	"$(INTDIR)/Viewex.rsc" \
	"$(INTDIR)/viewmac.rsc"

"$(OUTDIR)\Viewex.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

MFILE32=mfile.exe
# ADD BASE MFILE32 COPY /NOLOGO
# ADD MFILE32 COPY /NOLOGO
MFILE32_FLAGS=COPY /NOLOGO 
MFILE32_FILES= \
	"$(OUTDIR)/Viewex.exe"

"$(OUTDIR)\Viewex.trg" : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\MacDbg\Viewex.exe "$(MFILE32_DEST)"

DOWNLOAD : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\MacDbg\Viewex.exe "$(MFILE32_DEST)"

!ELSEIF  "$(CFG)" == "Viewex - Power Macintosh Release"

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

ALL : "$(OUTDIR)\Viewex.exe" "$(OUTDIR)\Viewex.trg"

CLEAN : 
	-@erase ".\PMacRel\Viewex.trg"
	-@erase ".\PMacRel\Viewex.exe"
	-@erase ".\PMacRel\Enterdlg.obj"
	-@erase ".\PMacRel\Viewex.pch"
	-@erase ".\PMacRel\Maindoc.obj"
	-@erase ".\PMacRel\Stdafx.obj"
	-@erase ".\PMacRel\Simpvw.obj"
	-@erase ".\PMacRel\Viewex.obj"
	-@erase ".\PMacRel\Splitter.obj"
	-@erase ".\PMacRel\Inputvw.obj"
	-@erase ".\PMacRel\Viewex.rsc"
	-@erase ".\PMacRel\viewmac.rsc"

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
 "_WINDOWS" /D "WIN32" /D "_MBCS" /D "_AFXDLL" /Fp"$(INTDIR)/Viewex.pch"\
 /Yu"Stdafx.h" /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
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
RSC_PROJ=/l 0x409 /r /m /fo"$(INTDIR)/Viewex.rsc" /d "_MAC" /d "_MPPC_" /d\
 "NDEBUG" 
MRC=mrc.exe
# ADD BASE MRC /D "_MPPC_" /D "_MAC" /D "NDEBUG" /NOLOGO
# ADD MRC /D "_MPPC_" /D "_MAC" /D "NDEBUG" /NOLOGO
MRC_PROJ=/D "_MPPC_" /D "_MAC" /D "NDEBUG" /l 0x409 /NOLOGO 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/Viewex.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 cfrg.rsc /nologo /mac:bundle /machine:MPPC
# ADD LINK32 /nologo /mac:bundle /mac:creator="VIEW" /machine:MPPC
LINK32_FLAGS=/nologo /mac:bundle /mac:type="APPL" /mac:creator="VIEW"\
 /incremental:no /pdb:"$(OUTDIR)/Viewex.pdb" /machine:MPPC\
 /out:"$(OUTDIR)/Viewex.exe" 
LINK32_OBJS= \
	"$(INTDIR)/Enterdlg.obj" \
	"$(INTDIR)/Maindoc.obj" \
	"$(INTDIR)/Stdafx.obj" \
	"$(INTDIR)/Simpvw.obj" \
	"$(INTDIR)/Viewex.obj" \
	"$(INTDIR)/Splitter.obj" \
	"$(INTDIR)/Inputvw.obj" \
	"$(INTDIR)/Viewex.rsc" \
	"$(INTDIR)/viewmac.rsc"

"$(OUTDIR)\Viewex.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

MFILE32=mfile.exe
# ADD BASE MFILE32 COPY /NOLOGO
# ADD MFILE32 COPY /NOLOGO
MFILE32_FLAGS=COPY /NOLOGO 
MFILE32_FILES= \
	"$(OUTDIR)/Viewex.exe"

"$(OUTDIR)\Viewex.trg" : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\PMacRel\Viewex.exe "$(MFILE32_DEST)"

DOWNLOAD : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\PMacRel\Viewex.exe "$(MFILE32_DEST)"

!ELSEIF  "$(CFG)" == "Viewex - Power Macintosh Debug"

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

ALL : "$(OUTDIR)\Viewex.exe" "$(OUTDIR)\Viewex.trg"

CLEAN : 
	-@erase ".\PMacDbg\vc40.pdb"
	-@erase ".\PMacDbg\Viewex.pch"
	-@erase ".\PMacDbg\Viewex.trg"
	-@erase ".\PMacDbg\Viewex.exe"
	-@erase ".\PMacDbg\Inputvw.obj"
	-@erase ".\PMacDbg\Enterdlg.obj"
	-@erase ".\PMacDbg\Stdafx.obj"
	-@erase ".\PMacDbg\Simpvw.obj"
	-@erase ".\PMacDbg\Maindoc.obj"
	-@erase ".\PMacDbg\Viewex.obj"
	-@erase ".\PMacDbg\Splitter.obj"
	-@erase ".\PMacDbg\Viewex.rsc"
	-@erase ".\PMacDbg\viewmac.rsc"
	-@erase ".\PMacDbg\Viewex.ilk"
	-@erase ".\PMacDbg\Viewex.pdb"

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
 "_WINDOWS" /D "WIN32" /D "_MBCS" /D "_AFXDLL" /Fp"$(INTDIR)/Viewex.pch"\
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
RSC_PROJ=/l 0x409 /r /m /fo"$(INTDIR)/Viewex.rsc" /d "_MAC" /d "_MPPC_" /d\
 "_DEBUG" 
MRC=mrc.exe
# ADD BASE MRC /D "_MPPC_" /D "_MAC" /D "_DEBUG" /NOLOGO
# ADD MRC /D "_MPPC_" /D "_MAC" /D "_DEBUG" /NOLOGO
MRC_PROJ=/D "_MPPC_" /D "_MAC" /D "_DEBUG" /l 0x409 /NOLOGO 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/Viewex.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 cfrg.rsc /nologo /mac:bundle /debug /machine:MPPC
# ADD LINK32 /nologo /mac:bundle /mac:creator="VIEW" /debug /machine:MPPC
LINK32_FLAGS=/nologo /mac:bundle /mac:type="APPL" /mac:creator="VIEW"\
 /incremental:yes /pdb:"$(OUTDIR)/Viewex.pdb" /debug /machine:MPPC\
 /out:"$(OUTDIR)/Viewex.exe" 
LINK32_OBJS= \
	"$(INTDIR)/Inputvw.obj" \
	"$(INTDIR)/Enterdlg.obj" \
	"$(INTDIR)/Stdafx.obj" \
	"$(INTDIR)/Simpvw.obj" \
	"$(INTDIR)/Maindoc.obj" \
	"$(INTDIR)/Viewex.obj" \
	"$(INTDIR)/Splitter.obj" \
	"$(INTDIR)/Viewex.rsc" \
	"$(INTDIR)/viewmac.rsc"

"$(OUTDIR)\Viewex.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

MFILE32=mfile.exe
# ADD BASE MFILE32 COPY /NOLOGO
# ADD MFILE32 COPY /NOLOGO
MFILE32_FLAGS=COPY /NOLOGO 
MFILE32_FILES= \
	"$(OUTDIR)/Viewex.exe"

"$(OUTDIR)\Viewex.trg" : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\PMacDbg\Viewex.exe "$(MFILE32_DEST)"

DOWNLOAD : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\PMacDbg\Viewex.exe "$(MFILE32_DEST)"

!ENDIF 

################################################################################
# Begin Target

# Name "Viewex - Win32 Release"
# Name "Viewex - Win32 Debug"
# Name "Viewex - Macintosh Release"
# Name "Viewex - Macintosh Debug"
# Name "Viewex - Power Macintosh Release"
# Name "Viewex - Power Macintosh Debug"

!IF  "$(CFG)" == "Viewex - Win32 Release"

!ELSEIF  "$(CFG)" == "Viewex - Win32 Debug"

!ELSEIF  "$(CFG)" == "Viewex - Macintosh Release"

!ELSEIF  "$(CFG)" == "Viewex - Macintosh Debug"

!ELSEIF  "$(CFG)" == "Viewex - Power Macintosh Release"

!ELSEIF  "$(CFG)" == "Viewex - Power Macintosh Debug"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\Viewex.rc
DEP_RSC_VIEWE=\
	".\Viewex.ico"\
	

!IF  "$(CFG)" == "Viewex - Win32 Release"


"$(INTDIR)\Viewex.res" : $(SOURCE) $(DEP_RSC_VIEWE) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Viewex - Win32 Debug"


"$(INTDIR)\Viewex.res" : $(SOURCE) $(DEP_RSC_VIEWE) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Viewex - Macintosh Release"


"$(INTDIR)\Viewex.rsc" : $(SOURCE) $(DEP_RSC_VIEWE) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Viewex - Macintosh Debug"


"$(INTDIR)\Viewex.rsc" : $(SOURCE) $(DEP_RSC_VIEWE) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Viewex - Power Macintosh Release"


"$(INTDIR)\Viewex.rsc" : $(SOURCE) $(DEP_RSC_VIEWE) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Viewex - Power Macintosh Debug"


"$(INTDIR)\Viewex.rsc" : $(SOURCE) $(DEP_RSC_VIEWE) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Inputvw.cpp
DEP_CPP_INPUT=\
	".\Stdafx.h"\
	".\Viewex.h"\
	".\Inputvw.h"\
	".\Maindoc.h"\
	".\Simpvw.h"\
	

!IF  "$(CFG)" == "Viewex - Win32 Release"


"$(INTDIR)\Inputvw.obj" : $(SOURCE) $(DEP_CPP_INPUT) "$(INTDIR)"\
 "$(INTDIR)\Viewex.pch"

"$(INTDIR)\vc40.idb" : $(SOURCE) $(DEP_CPP_INPUT) "$(INTDIR)"\
 "$(INTDIR)\Viewex.pch"


!ELSEIF  "$(CFG)" == "Viewex - Win32 Debug"


"$(INTDIR)\Inputvw.obj" : $(SOURCE) $(DEP_CPP_INPUT) "$(INTDIR)"\
 "$(INTDIR)\Viewex.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_INPUT) "$(INTDIR)"\
 "$(INTDIR)\Viewex.pch"

"$(INTDIR)\vc40.idb" : $(SOURCE) $(DEP_CPP_INPUT) "$(INTDIR)"\
 "$(INTDIR)\Viewex.pch"


!ELSEIF  "$(CFG)" == "Viewex - Macintosh Release"


"$(INTDIR)\Inputvw.obj" : $(SOURCE) $(DEP_CPP_INPUT) "$(INTDIR)"\
 "$(INTDIR)\Viewex.pch"


!ELSEIF  "$(CFG)" == "Viewex - Macintosh Debug"


"$(INTDIR)\Inputvw.obj" : $(SOURCE) $(DEP_CPP_INPUT) "$(INTDIR)"\
 "$(INTDIR)\Viewex.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_INPUT) "$(INTDIR)"\
 "$(INTDIR)\Viewex.pch"


!ELSEIF  "$(CFG)" == "Viewex - Power Macintosh Release"


"$(INTDIR)\Inputvw.obj" : $(SOURCE) $(DEP_CPP_INPUT) "$(INTDIR)"\
 "$(INTDIR)\Viewex.pch"


!ELSEIF  "$(CFG)" == "Viewex - Power Macintosh Debug"


"$(INTDIR)\Inputvw.obj" : $(SOURCE) $(DEP_CPP_INPUT) "$(INTDIR)"\
 "$(INTDIR)\Viewex.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_INPUT) "$(INTDIR)"\
 "$(INTDIR)\Viewex.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Maindoc.cpp
DEP_CPP_MAIND=\
	".\Enterdlg.h"\
	".\Stdafx.h"\
	".\Viewex.h"\
	".\Inputvw.h"\
	".\Maindoc.h"\
	".\Simpvw.h"\
	

!IF  "$(CFG)" == "Viewex - Win32 Release"


"$(INTDIR)\Maindoc.obj" : $(SOURCE) $(DEP_CPP_MAIND) "$(INTDIR)"\
 "$(INTDIR)\Viewex.pch"

"$(INTDIR)\vc40.idb" : $(SOURCE) $(DEP_CPP_MAIND) "$(INTDIR)"\
 "$(INTDIR)\Viewex.pch"


!ELSEIF  "$(CFG)" == "Viewex - Win32 Debug"


"$(INTDIR)\Maindoc.obj" : $(SOURCE) $(DEP_CPP_MAIND) "$(INTDIR)"\
 "$(INTDIR)\Viewex.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_MAIND) "$(INTDIR)"\
 "$(INTDIR)\Viewex.pch"

"$(INTDIR)\vc40.idb" : $(SOURCE) $(DEP_CPP_MAIND) "$(INTDIR)"\
 "$(INTDIR)\Viewex.pch"


!ELSEIF  "$(CFG)" == "Viewex - Macintosh Release"


"$(INTDIR)\Maindoc.obj" : $(SOURCE) $(DEP_CPP_MAIND) "$(INTDIR)"\
 "$(INTDIR)\Viewex.pch"


!ELSEIF  "$(CFG)" == "Viewex - Macintosh Debug"


"$(INTDIR)\Maindoc.obj" : $(SOURCE) $(DEP_CPP_MAIND) "$(INTDIR)"\
 "$(INTDIR)\Viewex.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_MAIND) "$(INTDIR)"\
 "$(INTDIR)\Viewex.pch"


!ELSEIF  "$(CFG)" == "Viewex - Power Macintosh Release"


"$(INTDIR)\Maindoc.obj" : $(SOURCE) $(DEP_CPP_MAIND) "$(INTDIR)"\
 "$(INTDIR)\Viewex.pch"


!ELSEIF  "$(CFG)" == "Viewex - Power Macintosh Debug"


"$(INTDIR)\Maindoc.obj" : $(SOURCE) $(DEP_CPP_MAIND) "$(INTDIR)"\
 "$(INTDIR)\Viewex.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_MAIND) "$(INTDIR)"\
 "$(INTDIR)\Viewex.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Simpvw.cpp
DEP_CPP_SIMPV=\
	".\Stdafx.h"\
	".\Viewex.h"\
	".\Inputvw.h"\
	".\Maindoc.h"\
	".\Simpvw.h"\
	

!IF  "$(CFG)" == "Viewex - Win32 Release"


"$(INTDIR)\Simpvw.obj" : $(SOURCE) $(DEP_CPP_SIMPV) "$(INTDIR)"\
 "$(INTDIR)\Viewex.pch"

"$(INTDIR)\vc40.idb" : $(SOURCE) $(DEP_CPP_SIMPV) "$(INTDIR)"\
 "$(INTDIR)\Viewex.pch"


!ELSEIF  "$(CFG)" == "Viewex - Win32 Debug"


"$(INTDIR)\Simpvw.obj" : $(SOURCE) $(DEP_CPP_SIMPV) "$(INTDIR)"\
 "$(INTDIR)\Viewex.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_SIMPV) "$(INTDIR)"\
 "$(INTDIR)\Viewex.pch"

"$(INTDIR)\vc40.idb" : $(SOURCE) $(DEP_CPP_SIMPV) "$(INTDIR)"\
 "$(INTDIR)\Viewex.pch"


!ELSEIF  "$(CFG)" == "Viewex - Macintosh Release"


"$(INTDIR)\Simpvw.obj" : $(SOURCE) $(DEP_CPP_SIMPV) "$(INTDIR)"\
 "$(INTDIR)\Viewex.pch"


!ELSEIF  "$(CFG)" == "Viewex - Macintosh Debug"


"$(INTDIR)\Simpvw.obj" : $(SOURCE) $(DEP_CPP_SIMPV) "$(INTDIR)"\
 "$(INTDIR)\Viewex.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_SIMPV) "$(INTDIR)"\
 "$(INTDIR)\Viewex.pch"


!ELSEIF  "$(CFG)" == "Viewex - Power Macintosh Release"


"$(INTDIR)\Simpvw.obj" : $(SOURCE) $(DEP_CPP_SIMPV) "$(INTDIR)"\
 "$(INTDIR)\Viewex.pch"


!ELSEIF  "$(CFG)" == "Viewex - Power Macintosh Debug"


"$(INTDIR)\Simpvw.obj" : $(SOURCE) $(DEP_CPP_SIMPV) "$(INTDIR)"\
 "$(INTDIR)\Viewex.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_SIMPV) "$(INTDIR)"\
 "$(INTDIR)\Viewex.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Splitter.cpp
DEP_CPP_SPLIT=\
	".\Splitter.h"\
	".\Stdafx.h"\
	".\Viewex.h"\
	".\Inputvw.h"\
	".\Maindoc.h"\
	".\Simpvw.h"\
	

!IF  "$(CFG)" == "Viewex - Win32 Release"


"$(INTDIR)\Splitter.obj" : $(SOURCE) $(DEP_CPP_SPLIT) "$(INTDIR)"\
 "$(INTDIR)\Viewex.pch"

"$(INTDIR)\vc40.idb" : $(SOURCE) $(DEP_CPP_SPLIT) "$(INTDIR)"\
 "$(INTDIR)\Viewex.pch"


!ELSEIF  "$(CFG)" == "Viewex - Win32 Debug"


"$(INTDIR)\Splitter.obj" : $(SOURCE) $(DEP_CPP_SPLIT) "$(INTDIR)"\
 "$(INTDIR)\Viewex.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_SPLIT) "$(INTDIR)"\
 "$(INTDIR)\Viewex.pch"

"$(INTDIR)\vc40.idb" : $(SOURCE) $(DEP_CPP_SPLIT) "$(INTDIR)"\
 "$(INTDIR)\Viewex.pch"


!ELSEIF  "$(CFG)" == "Viewex - Macintosh Release"


"$(INTDIR)\Splitter.obj" : $(SOURCE) $(DEP_CPP_SPLIT) "$(INTDIR)"\
 "$(INTDIR)\Viewex.pch"


!ELSEIF  "$(CFG)" == "Viewex - Macintosh Debug"


"$(INTDIR)\Splitter.obj" : $(SOURCE) $(DEP_CPP_SPLIT) "$(INTDIR)"\
 "$(INTDIR)\Viewex.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_SPLIT) "$(INTDIR)"\
 "$(INTDIR)\Viewex.pch"


!ELSEIF  "$(CFG)" == "Viewex - Power Macintosh Release"


"$(INTDIR)\Splitter.obj" : $(SOURCE) $(DEP_CPP_SPLIT) "$(INTDIR)"\
 "$(INTDIR)\Viewex.pch"


!ELSEIF  "$(CFG)" == "Viewex - Power Macintosh Debug"


"$(INTDIR)\Splitter.obj" : $(SOURCE) $(DEP_CPP_SPLIT) "$(INTDIR)"\
 "$(INTDIR)\Viewex.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_SPLIT) "$(INTDIR)"\
 "$(INTDIR)\Viewex.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Stdafx.cpp
DEP_CPP_STDAF=\
	".\Stdafx.h"\
	

!IF  "$(CFG)" == "Viewex - Win32 Release"

# ADD CPP /Yc

BuildCmds= \
	$(CPP) /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/Viewex.pch" /Yc /Fo"$(INTDIR)/"\
 /Fd"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\Stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\vc40.idb" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\Viewex.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "Viewex - Win32 Debug"

# ADD CPP /Yc

BuildCmds= \
	$(CPP) /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/Viewex.pch" /Yc /Fo"$(INTDIR)/"\
 /Fd"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\Stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\vc40.idb" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\Viewex.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "Viewex - Macintosh Release"

# ADD CPP /Yc

BuildCmds= \
	$(CPP) /nologo /AL /Q68s /W3 /GX /O2 /D "_WINDOWS" /D "_MAC" /D "_68K_" /D\
 "WIN32" /D "NDEBUG" /D "_MBCS" /Fp"$(INTDIR)/Viewex.pch" /Yc /Fo"$(INTDIR)/"\
 /Fd"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\Stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\Viewex.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "Viewex - Macintosh Debug"

# ADD CPP /Yc

BuildCmds= \
	$(CPP) /nologo /AL /Q68s /Q68m /W3 /GX /Zi /Od /D "_WINDOWS" /D "_MAC" /D\
 "_68K_" /D "WIN32" /D "_DEBUG" /D "_MBCS" /Fp"$(INTDIR)/Viewex.pch" /Yc\
 /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\Stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\Viewex.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "Viewex - Power Macintosh Release"

# ADD CPP /Yc

BuildCmds= \
	$(CPP) /nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "_MAC" /D "_MPPC_" /D "_WINDOWS"\
 /D "WIN32" /D "_MBCS" /D "_AFXDLL" /Fp"$(INTDIR)/Viewex.pch" /Yc\
 /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\Stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\Viewex.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "Viewex - Power Macintosh Debug"

# ADD CPP /Yc"Stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MDd /W3 /GX /Zi /Od /D "_DEBUG" /D "_MAC" /D "_MPPC_" /D\
 "_WINDOWS" /D "WIN32" /D "_MBCS" /D "_AFXDLL" /Fp"$(INTDIR)/Viewex.pch"\
 /Yc"Stdafx.h" /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\Stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\Viewex.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Viewex.cpp
DEP_CPP_VIEWEX=\
	".\Splitter.h"\
	".\Stdafx.h"\
	".\Viewex.h"\
	".\Inputvw.h"\
	".\Maindoc.h"\
	".\Simpvw.h"\
	

!IF  "$(CFG)" == "Viewex - Win32 Release"


"$(INTDIR)\Viewex.obj" : $(SOURCE) $(DEP_CPP_VIEWEX) "$(INTDIR)"\
 "$(INTDIR)\Viewex.pch"

"$(INTDIR)\vc40.idb" : $(SOURCE) $(DEP_CPP_VIEWEX) "$(INTDIR)"\
 "$(INTDIR)\Viewex.pch"


!ELSEIF  "$(CFG)" == "Viewex - Win32 Debug"


"$(INTDIR)\Viewex.obj" : $(SOURCE) $(DEP_CPP_VIEWEX) "$(INTDIR)"\
 "$(INTDIR)\Viewex.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_VIEWEX) "$(INTDIR)"\
 "$(INTDIR)\Viewex.pch"

"$(INTDIR)\vc40.idb" : $(SOURCE) $(DEP_CPP_VIEWEX) "$(INTDIR)"\
 "$(INTDIR)\Viewex.pch"


!ELSEIF  "$(CFG)" == "Viewex - Macintosh Release"


"$(INTDIR)\Viewex.obj" : $(SOURCE) $(DEP_CPP_VIEWEX) "$(INTDIR)"\
 "$(INTDIR)\Viewex.pch"


!ELSEIF  "$(CFG)" == "Viewex - Macintosh Debug"


"$(INTDIR)\Viewex.obj" : $(SOURCE) $(DEP_CPP_VIEWEX) "$(INTDIR)"\
 "$(INTDIR)\Viewex.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_VIEWEX) "$(INTDIR)"\
 "$(INTDIR)\Viewex.pch"


!ELSEIF  "$(CFG)" == "Viewex - Power Macintosh Release"


"$(INTDIR)\Viewex.obj" : $(SOURCE) $(DEP_CPP_VIEWEX) "$(INTDIR)"\
 "$(INTDIR)\Viewex.pch"


!ELSEIF  "$(CFG)" == "Viewex - Power Macintosh Debug"


"$(INTDIR)\Viewex.obj" : $(SOURCE) $(DEP_CPP_VIEWEX) "$(INTDIR)"\
 "$(INTDIR)\Viewex.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_VIEWEX) "$(INTDIR)"\
 "$(INTDIR)\Viewex.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Enterdlg.cpp
DEP_CPP_ENTER=\
	".\Stdafx.h"\
	".\Viewex.h"\
	".\Enterdlg.h"\
	".\Maindoc.h"\
	".\Simpvw.h"\
	".\Inputvw.h"\
	

!IF  "$(CFG)" == "Viewex - Win32 Release"


"$(INTDIR)\Enterdlg.obj" : $(SOURCE) $(DEP_CPP_ENTER) "$(INTDIR)"\
 "$(INTDIR)\Viewex.pch"

"$(INTDIR)\vc40.idb" : $(SOURCE) $(DEP_CPP_ENTER) "$(INTDIR)"\
 "$(INTDIR)\Viewex.pch"


!ELSEIF  "$(CFG)" == "Viewex - Win32 Debug"


"$(INTDIR)\Enterdlg.obj" : $(SOURCE) $(DEP_CPP_ENTER) "$(INTDIR)"\
 "$(INTDIR)\Viewex.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_ENTER) "$(INTDIR)"\
 "$(INTDIR)\Viewex.pch"

"$(INTDIR)\vc40.idb" : $(SOURCE) $(DEP_CPP_ENTER) "$(INTDIR)"\
 "$(INTDIR)\Viewex.pch"


!ELSEIF  "$(CFG)" == "Viewex - Macintosh Release"


"$(INTDIR)\Enterdlg.obj" : $(SOURCE) $(DEP_CPP_ENTER) "$(INTDIR)"\
 "$(INTDIR)\Viewex.pch"


!ELSEIF  "$(CFG)" == "Viewex - Macintosh Debug"


"$(INTDIR)\Enterdlg.obj" : $(SOURCE) $(DEP_CPP_ENTER) "$(INTDIR)"\
 "$(INTDIR)\Viewex.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_ENTER) "$(INTDIR)"\
 "$(INTDIR)\Viewex.pch"


!ELSEIF  "$(CFG)" == "Viewex - Power Macintosh Release"


"$(INTDIR)\Enterdlg.obj" : $(SOURCE) $(DEP_CPP_ENTER) "$(INTDIR)"\
 "$(INTDIR)\Viewex.pch"


!ELSEIF  "$(CFG)" == "Viewex - Power Macintosh Debug"


"$(INTDIR)\Enterdlg.obj" : $(SOURCE) $(DEP_CPP_ENTER) "$(INTDIR)"\
 "$(INTDIR)\Viewex.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_ENTER) "$(INTDIR)"\
 "$(INTDIR)\Viewex.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\viewmac.r

!IF  "$(CFG)" == "Viewex - Win32 Release"

!ELSEIF  "$(CFG)" == "Viewex - Win32 Debug"

!ELSEIF  "$(CFG)" == "Viewex - Macintosh Release"

DEP_MRC_VIEWM=\
	{$(INCLUDE)}"\mrc\Balloons.r"\
	

"$(INTDIR)\viewmac.rsc" : $(SOURCE) $(DEP_MRC_VIEWM) "$(INTDIR)"
   $(MRC) /o"$(INTDIR)/viewmac.rsc" /D "_68K_" /D "_MAC" /D "NDEBUG" /l 0x409\
 /NOLOGO $(SOURCE)


!ELSEIF  "$(CFG)" == "Viewex - Macintosh Debug"

DEP_MRC_VIEWM=\
	{$(INCLUDE)}"\mrc\Balloons.r"\
	

"$(INTDIR)\viewmac.rsc" : $(SOURCE) $(DEP_MRC_VIEWM) "$(INTDIR)"
   $(MRC) /o"$(INTDIR)/viewmac.rsc" /D "_68K_" /D "_MAC" /D "_DEBUG" /l 0x409\
 /NOLOGO $(SOURCE)


!ELSEIF  "$(CFG)" == "Viewex - Power Macintosh Release"

DEP_MRC_VIEWM=\
	{$(INCLUDE)}"\mrc\Balloons.r"\
	

"$(INTDIR)\viewmac.rsc" : $(SOURCE) $(DEP_MRC_VIEWM) "$(INTDIR)"
   $(MRC) /o"$(INTDIR)/viewmac.rsc" /D "_MPPC_" /D "_MAC" /D "NDEBUG" /l 0x409\
 /NOLOGO $(SOURCE)


!ELSEIF  "$(CFG)" == "Viewex - Power Macintosh Debug"

DEP_MRC_VIEWM=\
	{$(INCLUDE)}"\mrc\Balloons.r"\
	

"$(INTDIR)\viewmac.rsc" : $(SOURCE) $(DEP_MRC_VIEWM) "$(INTDIR)"
   $(MRC) /o"$(INTDIR)/viewmac.rsc" /D "_MPPC_" /D "_MAC" /D "_DEBUG" /l 0x409\
 /NOLOGO $(SOURCE)


!ENDIF 

# End Source File
# End Target
# End Project
################################################################################
