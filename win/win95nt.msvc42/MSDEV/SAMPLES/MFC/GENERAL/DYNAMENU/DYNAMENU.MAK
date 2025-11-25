# Microsoft Visual C++ Generated NMAKE File, Format Version 40000
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101
# TARGTYPE "Power Macintosh Application" 0x0401
# TARGTYPE "Macintosh Application" 0x0301

!IF "$(CFG)" == ""
CFG=DynaMenu - Win32 Debug
!MESSAGE No configuration specified.  Defaulting to DynaMenu - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "DynaMenu - Win32 Release" && "$(CFG)" !=\
 "DynaMenu - Win32 Debug" && "$(CFG)" != "DynaMenu - Macintosh Release" &&\
 "$(CFG)" != "DynaMenu - Macintosh Debug" && "$(CFG)" !=\
 "DynaMenu - Power Macintosh Release" && "$(CFG)" !=\
 "DynaMenu - Power Macintosh Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "dynamenu.mak" CFG="DynaMenu - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "DynaMenu - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "DynaMenu - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE "DynaMenu - Macintosh Release" (based on "Macintosh Application")
!MESSAGE "DynaMenu - Macintosh Debug" (based on "Macintosh Application")
!MESSAGE "DynaMenu - Power Macintosh Release" (based on\
 "Power Macintosh Application")
!MESSAGE "DynaMenu - Power Macintosh Debug" (based on\
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
# PROP Target_Last_Scanned "DynaMenu - Power Macintosh Debug"

!IF  "$(CFG)" == "DynaMenu - Win32 Release"

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

ALL : "$(OUTDIR)\dynamenu.exe"

CLEAN : 
	-@erase ".\Release\dynamenu.exe"
	-@erase ".\Release\mdichild.obj"
	-@erase ".\Release\dynamenu.pch"
	-@erase ".\Release\dynamenu.obj"
	-@erase ".\Release\dmview.obj"
	-@erase ".\Release\dmdoc.obj"
	-@erase ".\Release\mainfrm.obj"
	-@erase ".\Release\stdafx.obj"
	-@erase ".\Release\coloropt.obj"
	-@erase ".\Release\dynamenu.res"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"Stdafx.h" /c
# SUBTRACT CPP /Fr
CPP_PROJ=/nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/dynamenu.pch" /Yu"Stdafx.h" /Fo"$(INTDIR)/"\
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
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/dynamenu.res" /d "NDEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/dynamenu.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 /nologo /subsystem:windows /machine:I386
LINK32_FLAGS=/nologo /subsystem:windows /incremental:no\
 /pdb:"$(OUTDIR)/dynamenu.pdb" /machine:I386 /out:"$(OUTDIR)/dynamenu.exe" 
LINK32_OBJS= \
	"$(INTDIR)/mdichild.obj" \
	"$(INTDIR)/dynamenu.obj" \
	"$(INTDIR)/dmview.obj" \
	"$(INTDIR)/dmdoc.obj" \
	"$(INTDIR)/mainfrm.obj" \
	"$(INTDIR)/stdafx.obj" \
	"$(INTDIR)/coloropt.obj" \
	"$(INTDIR)/dynamenu.res"

"$(OUTDIR)\dynamenu.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "DynaMenu - Win32 Debug"

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

ALL : "$(OUTDIR)\dynamenu.exe"

CLEAN : 
	-@erase ".\Debug\vc40.pdb"
	-@erase ".\Debug\dynamenu.pch"
	-@erase ".\Debug\dynamenu.exe"
	-@erase ".\Debug\dmview.obj"
	-@erase ".\Debug\coloropt.obj"
	-@erase ".\Debug\stdafx.obj"
	-@erase ".\Debug\dmdoc.obj"
	-@erase ".\Debug\mdichild.obj"
	-@erase ".\Debug\mainfrm.obj"
	-@erase ".\Debug\dynamenu.obj"
	-@erase ".\Debug\dynamenu.res"
	-@erase ".\Debug\dynamenu.ilk"
	-@erase ".\Debug\dynamenu.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"Stdafx.h" /c
# SUBTRACT CPP /Fr
CPP_PROJ=/nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/dynamenu.pch" /Yu"Stdafx.h"\
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
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/dynamenu.res" /d "_DEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/dynamenu.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386
# ADD LINK32 /nologo /subsystem:windows /debug /machine:I386
LINK32_FLAGS=/nologo /subsystem:windows /incremental:yes\
 /pdb:"$(OUTDIR)/dynamenu.pdb" /debug /machine:I386\
 /out:"$(OUTDIR)/dynamenu.exe" 
LINK32_OBJS= \
	"$(INTDIR)/dmview.obj" \
	"$(INTDIR)/coloropt.obj" \
	"$(INTDIR)/stdafx.obj" \
	"$(INTDIR)/dmdoc.obj" \
	"$(INTDIR)/mdichild.obj" \
	"$(INTDIR)/mainfrm.obj" \
	"$(INTDIR)/dynamenu.obj" \
	"$(INTDIR)/dynamenu.res"

"$(OUTDIR)\dynamenu.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "DynaMenu - Macintosh Release"

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

ALL : "$(OUTDIR)\dynamenu.exe" "$(OUTDIR)\dynamenu.trg"

CLEAN : 
	-@erase ".\MacRel\dynamenu.trg"
	-@erase ".\MacRel\dynamenu.exe"
	-@erase ".\MacRel\coloropt.obj"
	-@erase ".\MacRel\dynamenu.pch"
	-@erase ".\MacRel\mdichild.obj"
	-@erase ".\MacRel\dynamenu.obj"
	-@erase ".\MacRel\dmview.obj"
	-@erase ".\MacRel\dmdoc.obj"
	-@erase ".\MacRel\stdafx.obj"
	-@erase ".\MacRel\mainfrm.obj"
	-@erase ".\MacRel\dynammac.rsc"
	-@erase ".\MacRel\dynamenu.rsc"

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
 "WIN32" /D "NDEBUG" /D "_MBCS" /Fp"$(INTDIR)/dynamenu.pch" /Yu"Stdafx.h"\
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
RSC_PROJ=/l 0x409 /r /m /fo"$(INTDIR)/dynamenu.rsc" /d "_MAC" /d "_68K_" /d\
 "NDEBUG" 
MRC=mrc.exe
# ADD BASE MRC /D "_68K_" /D "_MAC" /D "NDEBUG" /NOLOGO
# ADD MRC /D "_68K_" /D "_MAC" /D "NDEBUG" /NOLOGO
MRC_PROJ=/D "_68K_" /D "_MAC" /D "NDEBUG" /l 0x409 /NOLOGO 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/dynamenu.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 wlm.lib /nologo /MAC:bundle /machine:M68K
# ADD LINK32 /nologo /MAC:bundle /MAC:creator="DYNA" /machine:M68K
LINK32_FLAGS=/nologo /MAC:bundle /MAC:type="APPL" /MAC:creator="DYNA"\
 /pdb:"$(OUTDIR)/dynamenu.pdb" /machine:M68K /out:"$(OUTDIR)/dynamenu.exe" 
LINK32_OBJS= \
	"$(INTDIR)/coloropt.obj" \
	"$(INTDIR)/mdichild.obj" \
	"$(INTDIR)/dynamenu.obj" \
	"$(INTDIR)/dmview.obj" \
	"$(INTDIR)/dmdoc.obj" \
	"$(INTDIR)/stdafx.obj" \
	"$(INTDIR)/mainfrm.obj" \
	"$(INTDIR)/dynammac.rsc" \
	"$(INTDIR)/dynamenu.rsc"

"$(OUTDIR)\dynamenu.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

MFILE32=mfile.exe
# ADD BASE MFILE32 COPY /NOLOGO
# ADD MFILE32 COPY /NOLOGO
MFILE32_FLAGS=COPY /NOLOGO 
MFILE32_FILES= \
	"$(OUTDIR)/dynamenu.exe"

"$(OUTDIR)\dynamenu.trg" : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\MacRel\dynamenu.exe\
 "$(MFILE32_DEST):dynamenu.exe">"$(OUTDIR)\dynamenu.trg"

DOWNLOAD : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\MacRel\dynamenu.exe\
 "$(MFILE32_DEST):dynamenu.exe">"$(OUTDIR)\dynamenu.trg"

!ELSEIF  "$(CFG)" == "DynaMenu - Macintosh Debug"

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

ALL : "$(OUTDIR)\dynamenu.exe" "$(OUTDIR)\dynamenu.trg"

CLEAN : 
	-@erase ".\MacDbg\vc40.pdb"
	-@erase ".\MacDbg\dynamenu.pch"
	-@erase ".\MacDbg\dynamenu.pdb"
	-@erase ".\MacDbg\coloropt.obj"
	-@erase ".\MacDbg\mainfrm.obj"
	-@erase ".\MacDbg\mdichild.obj"
	-@erase ".\MacDbg\dynamenu.obj"
	-@erase ".\MacDbg\dmview.obj"
	-@erase ".\MacDbg\dmdoc.obj"
	-@erase ".\MacDbg\stdafx.obj"
	-@erase ".\MacDbg\dynamenu.rsc"
	-@erase ".\MacDbg\dynammac.rsc"
	-@erase ".\MacDbg\dynamenu.trg"
	-@erase ".\MacDbg\dynamenu.exe"

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
 "_68K_" /D "WIN32" /D "_DEBUG" /D "_MBCS" /Fp"$(INTDIR)/dynamenu.pch"\
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
RSC_PROJ=/l 0x409 /r /m /fo"$(INTDIR)/dynamenu.rsc" /d "_MAC" /d "_68K_" /d\
 "_DEBUG" 
MRC=mrc.exe
# ADD BASE MRC /D "_68K_" /D "_MAC" /D "_DEBUG" /NOLOGO
# ADD MRC /D "_68K_" /D "_MAC" /D "_DEBUG" /NOLOGO
MRC_PROJ=/D "_68K_" /D "_MAC" /D "_DEBUG" /l 0x409 /NOLOGO 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/dynamenu.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 wlm.lib /nologo /MAC:bundle /debug /machine:M68K
# ADD LINK32 /nologo /MAC:bundle /MAC:creator="DYNA" /debug /machine:M68K
LINK32_FLAGS=/nologo /MAC:bundle /MAC:type="APPL" /MAC:creator="DYNA"\
 /pdb:"$(OUTDIR)/dynamenu.pdb" /debug /machine:M68K\
 /out:"$(OUTDIR)/dynamenu.exe" 
LINK32_OBJS= \
	"$(INTDIR)/coloropt.obj" \
	"$(INTDIR)/mainfrm.obj" \
	"$(INTDIR)/mdichild.obj" \
	"$(INTDIR)/dynamenu.obj" \
	"$(INTDIR)/dmview.obj" \
	"$(INTDIR)/dmdoc.obj" \
	"$(INTDIR)/stdafx.obj" \
	"$(INTDIR)/dynamenu.rsc" \
	"$(INTDIR)/dynammac.rsc"

"$(OUTDIR)\dynamenu.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

MFILE32=mfile.exe
# ADD BASE MFILE32 COPY /NOLOGO
# ADD MFILE32 COPY /NOLOGO
MFILE32_FLAGS=COPY /NOLOGO 
MFILE32_FILES= \
	"$(OUTDIR)/dynamenu.exe"

"$(OUTDIR)\dynamenu.trg" : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\MacDbg\dynamenu.exe\
 "$(MFILE32_DEST):dynamenu.exe">"$(OUTDIR)\dynamenu.trg"

DOWNLOAD : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\MacDbg\dynamenu.exe\
 "$(MFILE32_DEST):dynamenu.exe">"$(OUTDIR)\dynamenu.trg"

!ELSEIF  "$(CFG)" == "DynaMenu - Power Macintosh Release"

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

ALL : "$(OUTDIR)\dynamenu.exe" "$(OUTDIR)\dynamenu.trg"

CLEAN : 
	-@erase ".\PMacRel\dynamenu.trg"
	-@erase ".\PMacRel\dynamenu.exe"
	-@erase ".\PMacRel\coloropt.obj"
	-@erase ".\PMacRel\dynamenu.pch"
	-@erase ".\PMacRel\stdafx.obj"
	-@erase ".\PMacRel\mainfrm.obj"
	-@erase ".\PMacRel\mdichild.obj"
	-@erase ".\PMacRel\dynamenu.obj"
	-@erase ".\PMacRel\dmview.obj"
	-@erase ".\PMacRel\dmdoc.obj"
	-@erase ".\PMacRel\dynamenu.rsc"
	-@erase ".\PMacRel\dynammac.rsc"

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
 "_WINDOWS" /D "WIN32" /D "_MBCS" /D "_AFXDLL" /Fp"$(INTDIR)/dynamenu.pch"\
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
RSC_PROJ=/l 0x409 /r /m /fo"$(INTDIR)/dynamenu.rsc" /d "_MAC" /d "_MPPC_" /d\
 "NDEBUG" 
MRC=mrc.exe
# ADD BASE MRC /D "_MPPC_" /D "_MAC" /D "NDEBUG" /NOLOGO
# ADD MRC /D "_MPPC_" /D "_MAC" /D "NDEBUG" /NOLOGO
MRC_PROJ=/D "_MPPC_" /D "_MAC" /D "NDEBUG" /l 0x409 /NOLOGO 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/dynamenu.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 cfrg.rsc /nologo /mac:bundle /machine:MPPC
# ADD LINK32 /nologo /mac:bundle /mac:creator="DYNA" /machine:MPPC
LINK32_FLAGS=/nologo /mac:bundle /mac:type="APPL" /mac:creator="DYNA"\
 /incremental:no /pdb:"$(OUTDIR)/dynamenu.pdb" /machine:MPPC\
 /out:"$(OUTDIR)/dynamenu.exe" 
LINK32_OBJS= \
	"$(INTDIR)/coloropt.obj" \
	"$(INTDIR)/stdafx.obj" \
	"$(INTDIR)/mainfrm.obj" \
	"$(INTDIR)/mdichild.obj" \
	"$(INTDIR)/dynamenu.obj" \
	"$(INTDIR)/dmview.obj" \
	"$(INTDIR)/dmdoc.obj" \
	"$(INTDIR)/dynamenu.rsc" \
	"$(INTDIR)/dynammac.rsc"

"$(OUTDIR)\dynamenu.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

MFILE32=mfile.exe
# ADD BASE MFILE32 COPY /NOLOGO
# ADD MFILE32 COPY /NOLOGO
MFILE32_FLAGS=COPY /NOLOGO 
MFILE32_FILES= \
	"$(OUTDIR)/dynamenu.exe"

"$(OUTDIR)\dynamenu.trg" : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\PMacRel\dynamenu.exe\
 "$(MFILE32_DEST):dynamenu.exe">"$(OUTDIR)\dynamenu.trg"

DOWNLOAD : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\PMacRel\dynamenu.exe\
 "$(MFILE32_DEST):dynamenu.exe">"$(OUTDIR)\dynamenu.trg"

!ELSEIF  "$(CFG)" == "DynaMenu - Power Macintosh Debug"

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

ALL : "$(OUTDIR)\dynamenu.exe" "$(OUTDIR)\dynamenu.trg"

CLEAN : 
	-@erase ".\PMacDbg\vc40.pdb"
	-@erase ".\PMacDbg\dynamenu.pch"
	-@erase ".\PMacDbg\dynamenu.ilk"
	-@erase ".\PMacDbg\dmdoc.obj"
	-@erase ".\PMacDbg\coloropt.obj"
	-@erase ".\PMacDbg\stdafx.obj"
	-@erase ".\PMacDbg\mdichild.obj"
	-@erase ".\PMacDbg\dynamenu.obj"
	-@erase ".\PMacDbg\mainfrm.obj"
	-@erase ".\PMacDbg\dmview.obj"
	-@erase ".\PMacDbg\dynamenu.rsc"
	-@erase ".\PMacDbg\dynammac.rsc"
	-@erase ".\PMacDbg\dynamenu.pdb"
	-@erase ".\PMacDbg\dynamenu.trg"
	-@erase ".\PMacDbg\dynamenu.exe"

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
 "_WINDOWS" /D "WIN32" /D "_MBCS" /D "_AFXDLL" /Fp"$(INTDIR)/dynamenu.pch"\
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
RSC_PROJ=/l 0x409 /r /m /fo"$(INTDIR)/dynamenu.rsc" /d "_MAC" /d "_MPPC_" /d\
 "_DEBUG" 
MRC=mrc.exe
# ADD BASE MRC /D "_MPPC_" /D "_MAC" /D "_DEBUG" /NOLOGO
# ADD MRC /D "_MPPC_" /D "_MAC" /D "_DEBUG" /NOLOGO
MRC_PROJ=/D "_MPPC_" /D "_MAC" /D "_DEBUG" /l 0x409 /NOLOGO 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/dynamenu.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 cfrg.rsc /nologo /mac:bundle /debug /machine:MPPC
# ADD LINK32 /nologo /mac:bundle /mac:creator="DYNA" /debug /machine:MPPC
LINK32_FLAGS=/nologo /mac:bundle /mac:type="APPL" /mac:creator="DYNA"\
 /incremental:yes /pdb:"$(OUTDIR)/dynamenu.pdb" /debug /machine:MPPC\
 /out:"$(OUTDIR)/dynamenu.exe" 
LINK32_OBJS= \
	"$(INTDIR)/dmdoc.obj" \
	"$(INTDIR)/coloropt.obj" \
	"$(INTDIR)/stdafx.obj" \
	"$(INTDIR)/mdichild.obj" \
	"$(INTDIR)/dynamenu.obj" \
	"$(INTDIR)/mainfrm.obj" \
	"$(INTDIR)/dmview.obj" \
	"$(INTDIR)/dynamenu.rsc" \
	"$(INTDIR)/dynammac.rsc"

"$(OUTDIR)\dynamenu.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

MFILE32=mfile.exe
# ADD BASE MFILE32 COPY /NOLOGO
# ADD MFILE32 COPY /NOLOGO
MFILE32_FLAGS=COPY /NOLOGO 
MFILE32_FILES= \
	"$(OUTDIR)/dynamenu.exe"

"$(OUTDIR)\dynamenu.trg" : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\PMacDbg\dynamenu.exe\
 "$(MFILE32_DEST):dynamenu.exe">"$(OUTDIR)\dynamenu.trg"

DOWNLOAD : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\PMacDbg\dynamenu.exe\
 "$(MFILE32_DEST):dynamenu.exe">"$(OUTDIR)\dynamenu.trg"

!ENDIF 

################################################################################
# Begin Target

# Name "DynaMenu - Win32 Release"
# Name "DynaMenu - Win32 Debug"
# Name "DynaMenu - Macintosh Release"
# Name "DynaMenu - Macintosh Debug"
# Name "DynaMenu - Power Macintosh Release"
# Name "DynaMenu - Power Macintosh Debug"

!IF  "$(CFG)" == "DynaMenu - Win32 Release"

!ELSEIF  "$(CFG)" == "DynaMenu - Win32 Debug"

!ELSEIF  "$(CFG)" == "DynaMenu - Macintosh Release"

!ELSEIF  "$(CFG)" == "DynaMenu - Macintosh Debug"

!ELSEIF  "$(CFG)" == "DynaMenu - Power Macintosh Release"

!ELSEIF  "$(CFG)" == "DynaMenu - Power Macintosh Debug"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\stdafx.cpp
DEP_CPP_STDAF=\
	".\stdafx.h"\
	

!IF  "$(CFG)" == "DynaMenu - Win32 Release"

# ADD CPP /Yc"Stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/dynamenu.pch" /Yc"Stdafx.h" /Fo"$(INTDIR)/"\
 /c $(SOURCE) \
	

"$(INTDIR)\stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\dynamenu.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "DynaMenu - Win32 Debug"

# ADD CPP /Yc

BuildCmds= \
	$(CPP) /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/dynamenu.pch" /Yc /Fo"$(INTDIR)/"\
 /Fd"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\dynamenu.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "DynaMenu - Macintosh Release"

# ADD CPP /Yc

BuildCmds= \
	$(CPP) /nologo /AL /Q68s /W3 /GX /O2 /D "_WINDOWS" /D "_MAC" /D "_68K_" /D\
 "WIN32" /D "NDEBUG" /D "_MBCS" /Fp"$(INTDIR)/dynamenu.pch" /Yc /Fo"$(INTDIR)/"\
 /c $(SOURCE) \
	

"$(INTDIR)\stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\dynamenu.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "DynaMenu - Macintosh Debug"

# ADD CPP /Yc

BuildCmds= \
	$(CPP) /nologo /AL /Q68s /Q68m /W3 /GX /Zi /Od /D "_WINDOWS" /D "_MAC" /D\
 "_68K_" /D "WIN32" /D "_DEBUG" /D "_MBCS" /Fp"$(INTDIR)/dynamenu.pch" /Yc\
 /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\dynamenu.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "DynaMenu - Power Macintosh Release"

# ADD CPP /Yc

BuildCmds= \
	$(CPP) /nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "_MAC" /D "_MPPC_" /D "_WINDOWS"\
 /D "WIN32" /D "_MBCS" /D "_AFXDLL" /Fp"$(INTDIR)/dynamenu.pch" /Yc\
 /Fo"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\dynamenu.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "DynaMenu - Power Macintosh Debug"

# ADD CPP /Yc

BuildCmds= \
	$(CPP) /nologo /MDd /W3 /GX /Zi /Od /D "_DEBUG" /D "_MAC" /D "_MPPC_" /D\
 "_WINDOWS" /D "WIN32" /D "_MBCS" /D "_AFXDLL" /Fp"$(INTDIR)/dynamenu.pch" /Yc\
 /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\dynamenu.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\dmdoc.cpp
DEP_CPP_DMDOC=\
	".\coloropt.h"\
	".\dmdoc.h"\
	".\dynamenu.h"\
	".\mdichild.h"\
	".\stdafx.h"\
	

!IF  "$(CFG)" == "DynaMenu - Win32 Release"


"$(INTDIR)\dmdoc.obj" : $(SOURCE) $(DEP_CPP_DMDOC) "$(INTDIR)"\
 "$(INTDIR)\dynamenu.pch"


!ELSEIF  "$(CFG)" == "DynaMenu - Win32 Debug"


"$(INTDIR)\dmdoc.obj" : $(SOURCE) $(DEP_CPP_DMDOC) "$(INTDIR)"\
 "$(INTDIR)\dynamenu.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_DMDOC) "$(INTDIR)"\
 "$(INTDIR)\dynamenu.pch"


!ELSEIF  "$(CFG)" == "DynaMenu - Macintosh Release"


"$(INTDIR)\dmdoc.obj" : $(SOURCE) $(DEP_CPP_DMDOC) "$(INTDIR)"\
 "$(INTDIR)\dynamenu.pch"


!ELSEIF  "$(CFG)" == "DynaMenu - Macintosh Debug"


"$(INTDIR)\dmdoc.obj" : $(SOURCE) $(DEP_CPP_DMDOC) "$(INTDIR)"\
 "$(INTDIR)\dynamenu.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_DMDOC) "$(INTDIR)"\
 "$(INTDIR)\dynamenu.pch"


!ELSEIF  "$(CFG)" == "DynaMenu - Power Macintosh Release"


"$(INTDIR)\dmdoc.obj" : $(SOURCE) $(DEP_CPP_DMDOC) "$(INTDIR)"\
 "$(INTDIR)\dynamenu.pch"


!ELSEIF  "$(CFG)" == "DynaMenu - Power Macintosh Debug"


"$(INTDIR)\dmdoc.obj" : $(SOURCE) $(DEP_CPP_DMDOC) "$(INTDIR)"\
 "$(INTDIR)\dynamenu.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_DMDOC) "$(INTDIR)"\
 "$(INTDIR)\dynamenu.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\dmview.cpp
DEP_CPP_DMVIE=\
	".\stdafx.h"\
	".\dynamenu.h"\
	".\dmdoc.h"\
	".\dmview.h"\
	

!IF  "$(CFG)" == "DynaMenu - Win32 Release"


"$(INTDIR)\dmview.obj" : $(SOURCE) $(DEP_CPP_DMVIE) "$(INTDIR)"\
 "$(INTDIR)\dynamenu.pch"


!ELSEIF  "$(CFG)" == "DynaMenu - Win32 Debug"


"$(INTDIR)\dmview.obj" : $(SOURCE) $(DEP_CPP_DMVIE) "$(INTDIR)"\
 "$(INTDIR)\dynamenu.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_DMVIE) "$(INTDIR)"\
 "$(INTDIR)\dynamenu.pch"


!ELSEIF  "$(CFG)" == "DynaMenu - Macintosh Release"


"$(INTDIR)\dmview.obj" : $(SOURCE) $(DEP_CPP_DMVIE) "$(INTDIR)"\
 "$(INTDIR)\dynamenu.pch"


!ELSEIF  "$(CFG)" == "DynaMenu - Macintosh Debug"


"$(INTDIR)\dmview.obj" : $(SOURCE) $(DEP_CPP_DMVIE) "$(INTDIR)"\
 "$(INTDIR)\dynamenu.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_DMVIE) "$(INTDIR)"\
 "$(INTDIR)\dynamenu.pch"


!ELSEIF  "$(CFG)" == "DynaMenu - Power Macintosh Release"


"$(INTDIR)\dmview.obj" : $(SOURCE) $(DEP_CPP_DMVIE) "$(INTDIR)"\
 "$(INTDIR)\dynamenu.pch"


!ELSEIF  "$(CFG)" == "DynaMenu - Power Macintosh Debug"


"$(INTDIR)\dmview.obj" : $(SOURCE) $(DEP_CPP_DMVIE) "$(INTDIR)"\
 "$(INTDIR)\dynamenu.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_DMVIE) "$(INTDIR)"\
 "$(INTDIR)\dynamenu.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\dynamenu.cpp
DEP_CPP_DYNAM=\
	".\stdafx.h"\
	".\dynamenu.h"\
	".\mainfrm.h"\
	".\dmdoc.h"\
	".\dmview.h"\
	".\mdichild.h"\
	

!IF  "$(CFG)" == "DynaMenu - Win32 Release"


"$(INTDIR)\dynamenu.obj" : $(SOURCE) $(DEP_CPP_DYNAM) "$(INTDIR)"\
 "$(INTDIR)\dynamenu.pch"


!ELSEIF  "$(CFG)" == "DynaMenu - Win32 Debug"


"$(INTDIR)\dynamenu.obj" : $(SOURCE) $(DEP_CPP_DYNAM) "$(INTDIR)"\
 "$(INTDIR)\dynamenu.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_DYNAM) "$(INTDIR)"\
 "$(INTDIR)\dynamenu.pch"


!ELSEIF  "$(CFG)" == "DynaMenu - Macintosh Release"


"$(INTDIR)\dynamenu.obj" : $(SOURCE) $(DEP_CPP_DYNAM) "$(INTDIR)"\
 "$(INTDIR)\dynamenu.pch"


!ELSEIF  "$(CFG)" == "DynaMenu - Macintosh Debug"


"$(INTDIR)\dynamenu.obj" : $(SOURCE) $(DEP_CPP_DYNAM) "$(INTDIR)"\
 "$(INTDIR)\dynamenu.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_DYNAM) "$(INTDIR)"\
 "$(INTDIR)\dynamenu.pch"


!ELSEIF  "$(CFG)" == "DynaMenu - Power Macintosh Release"


"$(INTDIR)\dynamenu.obj" : $(SOURCE) $(DEP_CPP_DYNAM) "$(INTDIR)"\
 "$(INTDIR)\dynamenu.pch"


!ELSEIF  "$(CFG)" == "DynaMenu - Power Macintosh Debug"


"$(INTDIR)\dynamenu.obj" : $(SOURCE) $(DEP_CPP_DYNAM) "$(INTDIR)"\
 "$(INTDIR)\dynamenu.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_DYNAM) "$(INTDIR)"\
 "$(INTDIR)\dynamenu.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\dynamenu.rc
DEP_RSC_DYNAME=\
	".\res\dynamenu.ico"\
	".\res\dmdoc.ico"\
	".\res\toolbar.bmp"\
	".\res\dynamenu.rc2"\
	

!IF  "$(CFG)" == "DynaMenu - Win32 Release"


"$(INTDIR)\dynamenu.res" : $(SOURCE) $(DEP_RSC_DYNAME) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "DynaMenu - Win32 Debug"


"$(INTDIR)\dynamenu.res" : $(SOURCE) $(DEP_RSC_DYNAME) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "DynaMenu - Macintosh Release"


"$(INTDIR)\dynamenu.rsc" : $(SOURCE) $(DEP_RSC_DYNAME) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "DynaMenu - Macintosh Debug"


"$(INTDIR)\dynamenu.rsc" : $(SOURCE) $(DEP_RSC_DYNAME) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "DynaMenu - Power Macintosh Release"


"$(INTDIR)\dynamenu.rsc" : $(SOURCE) $(DEP_RSC_DYNAME) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "DynaMenu - Power Macintosh Debug"


"$(INTDIR)\dynamenu.rsc" : $(SOURCE) $(DEP_RSC_DYNAME) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\mainfrm.cpp
DEP_CPP_MAINF=\
	".\stdafx.h"\
	".\dynamenu.h"\
	".\mainfrm.h"\
	".\dmdoc.h"\
	

!IF  "$(CFG)" == "DynaMenu - Win32 Release"


"$(INTDIR)\mainfrm.obj" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\dynamenu.pch"


!ELSEIF  "$(CFG)" == "DynaMenu - Win32 Debug"


"$(INTDIR)\mainfrm.obj" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\dynamenu.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\dynamenu.pch"


!ELSEIF  "$(CFG)" == "DynaMenu - Macintosh Release"


"$(INTDIR)\mainfrm.obj" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\dynamenu.pch"


!ELSEIF  "$(CFG)" == "DynaMenu - Macintosh Debug"


"$(INTDIR)\mainfrm.obj" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\dynamenu.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\dynamenu.pch"


!ELSEIF  "$(CFG)" == "DynaMenu - Power Macintosh Release"


"$(INTDIR)\mainfrm.obj" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\dynamenu.pch"


!ELSEIF  "$(CFG)" == "DynaMenu - Power Macintosh Debug"


"$(INTDIR)\mainfrm.obj" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\dynamenu.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\dynamenu.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\mdichild.cpp
DEP_CPP_MDICH=\
	".\stdafx.h"\
	".\dynamenu.h"\
	".\mdichild.h"\
	".\dmdoc.h"\
	

!IF  "$(CFG)" == "DynaMenu - Win32 Release"


"$(INTDIR)\mdichild.obj" : $(SOURCE) $(DEP_CPP_MDICH) "$(INTDIR)"\
 "$(INTDIR)\dynamenu.pch"


!ELSEIF  "$(CFG)" == "DynaMenu - Win32 Debug"


"$(INTDIR)\mdichild.obj" : $(SOURCE) $(DEP_CPP_MDICH) "$(INTDIR)"\
 "$(INTDIR)\dynamenu.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_MDICH) "$(INTDIR)"\
 "$(INTDIR)\dynamenu.pch"


!ELSEIF  "$(CFG)" == "DynaMenu - Macintosh Release"


"$(INTDIR)\mdichild.obj" : $(SOURCE) $(DEP_CPP_MDICH) "$(INTDIR)"\
 "$(INTDIR)\dynamenu.pch"


!ELSEIF  "$(CFG)" == "DynaMenu - Macintosh Debug"


"$(INTDIR)\mdichild.obj" : $(SOURCE) $(DEP_CPP_MDICH) "$(INTDIR)"\
 "$(INTDIR)\dynamenu.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_MDICH) "$(INTDIR)"\
 "$(INTDIR)\dynamenu.pch"


!ELSEIF  "$(CFG)" == "DynaMenu - Power Macintosh Release"


"$(INTDIR)\mdichild.obj" : $(SOURCE) $(DEP_CPP_MDICH) "$(INTDIR)"\
 "$(INTDIR)\dynamenu.pch"


!ELSEIF  "$(CFG)" == "DynaMenu - Power Macintosh Debug"


"$(INTDIR)\mdichild.obj" : $(SOURCE) $(DEP_CPP_MDICH) "$(INTDIR)"\
 "$(INTDIR)\dynamenu.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_MDICH) "$(INTDIR)"\
 "$(INTDIR)\dynamenu.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\coloropt.cpp
DEP_CPP_COLOR=\
	".\stdafx.h"\
	".\dynamenu.h"\
	".\coloropt.h"\
	".\dmdoc.h"\
	

!IF  "$(CFG)" == "DynaMenu - Win32 Release"


"$(INTDIR)\coloropt.obj" : $(SOURCE) $(DEP_CPP_COLOR) "$(INTDIR)"\
 "$(INTDIR)\dynamenu.pch"


!ELSEIF  "$(CFG)" == "DynaMenu - Win32 Debug"


"$(INTDIR)\coloropt.obj" : $(SOURCE) $(DEP_CPP_COLOR) "$(INTDIR)"\
 "$(INTDIR)\dynamenu.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_COLOR) "$(INTDIR)"\
 "$(INTDIR)\dynamenu.pch"


!ELSEIF  "$(CFG)" == "DynaMenu - Macintosh Release"


"$(INTDIR)\coloropt.obj" : $(SOURCE) $(DEP_CPP_COLOR) "$(INTDIR)"\
 "$(INTDIR)\dynamenu.pch"


!ELSEIF  "$(CFG)" == "DynaMenu - Macintosh Debug"


"$(INTDIR)\coloropt.obj" : $(SOURCE) $(DEP_CPP_COLOR) "$(INTDIR)"\
 "$(INTDIR)\dynamenu.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_COLOR) "$(INTDIR)"\
 "$(INTDIR)\dynamenu.pch"


!ELSEIF  "$(CFG)" == "DynaMenu - Power Macintosh Release"


"$(INTDIR)\coloropt.obj" : $(SOURCE) $(DEP_CPP_COLOR) "$(INTDIR)"\
 "$(INTDIR)\dynamenu.pch"


!ELSEIF  "$(CFG)" == "DynaMenu - Power Macintosh Debug"


"$(INTDIR)\coloropt.obj" : $(SOURCE) $(DEP_CPP_COLOR) "$(INTDIR)"\
 "$(INTDIR)\dynamenu.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_COLOR) "$(INTDIR)"\
 "$(INTDIR)\dynamenu.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\dynammac.r

!IF  "$(CFG)" == "DynaMenu - Win32 Release"

!ELSEIF  "$(CFG)" == "DynaMenu - Win32 Debug"

!ELSEIF  "$(CFG)" == "DynaMenu - Macintosh Release"

DEP_MRC_DYNAMM=\
	{$(INCLUDE)}"\mrc\balloons.r"\
	

"$(INTDIR)\dynammac.rsc" : $(SOURCE) $(DEP_MRC_DYNAMM) "$(INTDIR)"
   $(MRC) /o"$(INTDIR)/dynammac.rsc" /D "_68K_" /D "_MAC" /D "NDEBUG" /l 0x409\
 /NOLOGO $(SOURCE)


!ELSEIF  "$(CFG)" == "DynaMenu - Macintosh Debug"

DEP_MRC_DYNAMM=\
	{$(INCLUDE)}"\mrc\balloons.r"\
	

"$(INTDIR)\dynammac.rsc" : $(SOURCE) $(DEP_MRC_DYNAMM) "$(INTDIR)"
   $(MRC) /o"$(INTDIR)/dynammac.rsc" /D "_68K_" /D "_MAC" /D "_DEBUG" /l 0x409\
 /NOLOGO $(SOURCE)


!ELSEIF  "$(CFG)" == "DynaMenu - Power Macintosh Release"

DEP_MRC_DYNAMM=\
	{$(INCLUDE)}"\mrc\balloons.r"\
	

"$(INTDIR)\dynammac.rsc" : $(SOURCE) $(DEP_MRC_DYNAMM) "$(INTDIR)"
   $(MRC) /o"$(INTDIR)/dynammac.rsc" /D "_MPPC_" /D "_MAC" /D "NDEBUG" /l 0x409\
 /NOLOGO $(SOURCE)


!ELSEIF  "$(CFG)" == "DynaMenu - Power Macintosh Debug"

DEP_MRC_DYNAMM=\
	{$(INCLUDE)}"\mrc\balloons.r"\
	

"$(INTDIR)\dynammac.rsc" : $(SOURCE) $(DEP_MRC_DYNAMM) "$(INTDIR)"
   $(MRC) /o"$(INTDIR)/dynammac.rsc" /D "_MPPC_" /D "_MAC" /D "_DEBUG" /l 0x409\
 /NOLOGO $(SOURCE)


!ENDIF 

# End Source File
# End Target
# End Project
################################################################################
