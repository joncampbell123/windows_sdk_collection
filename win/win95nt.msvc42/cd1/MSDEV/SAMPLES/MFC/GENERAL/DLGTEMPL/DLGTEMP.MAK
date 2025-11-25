# Microsoft Developer Studio Generated NMAKE File, Format Version 4.10
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101
# TARGTYPE "Power Macintosh Application" 0x0401

!IF "$(CFG)" == ""
CFG=dlgtemp - Win32 Debug
!MESSAGE No configuration specified.  Defaulting to dlgtemp - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "dlgtemp - Win32 Release" && "$(CFG)" !=\
 "dlgtemp - Win32 Debug" && "$(CFG)" != "dlgtemp - Power Macintosh Release" &&\
 "$(CFG)" != "dlgtemp - Power Macintosh Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "dlgtemp.mak" CFG="dlgtemp - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "dlgtemp - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "dlgtemp - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE "dlgtemp - Power Macintosh Release" (based on\
 "Power Macintosh Application")
!MESSAGE "dlgtemp - Power Macintosh Debug" (based on\
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
# PROP Target_Last_Scanned "dlgtemp - Power Macintosh Release"

!IF  "$(CFG)" == "dlgtemp - Win32 Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
OUTDIR=.\Release
INTDIR=.\Release

ALL : "$(OUTDIR)\dlgtemp.exe"

CLEAN : 
	-@erase "$(INTDIR)\dlgtemp.obj"
	-@erase "$(INTDIR)\dlgtemp.pch"
	-@erase "$(INTDIR)\dlgtemp.res"
	-@erase "$(INTDIR)\itemtemp.obj"
	-@erase "$(INTDIR)\setupdlg.obj"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(OUTDIR)\dlgtemp.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
CPP_PROJ=/nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/dlgtemp.pch" /Yu"stdafx.h" /Fo"$(INTDIR)/"\
 /c 
CPP_OBJS=.\Release/
CPP_SBRS=.\.

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
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/dlgtemp.res" /d "NDEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/dlgtemp.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 /nologo /subsystem:windows /machine:I386
LINK32_FLAGS=/nologo /subsystem:windows /incremental:no\
 /pdb:"$(OUTDIR)/dlgtemp.pdb" /machine:I386 /out:"$(OUTDIR)/dlgtemp.exe" 
LINK32_OBJS= \
	"$(INTDIR)\dlgtemp.obj" \
	"$(INTDIR)\dlgtemp.res" \
	"$(INTDIR)\itemtemp.obj" \
	"$(INTDIR)\setupdlg.obj" \
	"$(INTDIR)\StdAfx.obj"

"$(OUTDIR)\dlgtemp.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "dlgtemp - Win32 Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
OUTDIR=.\Debug
INTDIR=.\Debug

ALL : "$(OUTDIR)\dlgtemp.exe" "$(OUTDIR)\dlgtemp.bsc"

CLEAN : 
	-@erase "$(INTDIR)\dlgtemp.obj"
	-@erase "$(INTDIR)\dlgtemp.pch"
	-@erase "$(INTDIR)\dlgtemp.res"
	-@erase "$(INTDIR)\dlgtemp.sbr"
	-@erase "$(INTDIR)\itemtemp.obj"
	-@erase "$(INTDIR)\itemtemp.sbr"
	-@erase "$(INTDIR)\setupdlg.obj"
	-@erase "$(INTDIR)\setupdlg.sbr"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\StdAfx.sbr"
	-@erase "$(INTDIR)\vc40.idb"
	-@erase "$(INTDIR)\vc40.pdb"
	-@erase "$(OUTDIR)\dlgtemp.bsc"
	-@erase "$(OUTDIR)\dlgtemp.exe"
	-@erase "$(OUTDIR)\dlgtemp.ilk"
	-@erase "$(OUTDIR)\dlgtemp.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /Yu"stdafx.h" /c
CPP_PROJ=/nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /D "_AFXDLL" /D "_MBCS" /FR"$(INTDIR)/" /Fp"$(INTDIR)/dlgtemp.pch"\
 /Yu"stdafx.h" /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\Debug/
CPP_SBRS=.\Debug/

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
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/dlgtemp.res" /d "_DEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/dlgtemp.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\dlgtemp.sbr" \
	"$(INTDIR)\itemtemp.sbr" \
	"$(INTDIR)\setupdlg.sbr" \
	"$(INTDIR)\StdAfx.sbr"

"$(OUTDIR)\dlgtemp.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386
# ADD LINK32 /nologo /subsystem:windows /debug /machine:I386
LINK32_FLAGS=/nologo /subsystem:windows /incremental:yes\
 /pdb:"$(OUTDIR)/dlgtemp.pdb" /debug /machine:I386 /out:"$(OUTDIR)/dlgtemp.exe" 
LINK32_OBJS= \
	"$(INTDIR)\dlgtemp.obj" \
	"$(INTDIR)\dlgtemp.res" \
	"$(INTDIR)\itemtemp.obj" \
	"$(INTDIR)\setupdlg.obj" \
	"$(INTDIR)\StdAfx.obj"

"$(OUTDIR)\dlgtemp.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "dlgtemp - Power Macintosh Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "PMacRel"
# PROP BASE Intermediate_Dir "PMacRel"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "PMacRel"
# PROP Intermediate_Dir "PMacRel"
# PROP Target_Dir ""
OUTDIR=.\PMacRel
INTDIR=.\PMacRel

ALL : "$(OUTDIR)\dlgtemp.exe" "$(OUTDIR)\dlgtemp.trg"

CLEAN : 
	-@erase "$(INTDIR)\dlgtemp.obj"
	-@erase "$(INTDIR)\dlgtemp.pch"
	-@erase "$(INTDIR)\dlgtemp.rsc"
	-@erase "$(INTDIR)\dlgtmac.rsc"
	-@erase "$(INTDIR)\itemtemp.obj"
	-@erase "$(INTDIR)\setupdlg.obj"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(OUTDIR)\dlgtemp.exe"
	-@erase "$(OUTDIR)\dlgtemp.trg"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

MTL=mktyplib.exe
# ADD BASE MTL /nologo /D "NDEBUG" /ppc
# ADD MTL /nologo /D "NDEBUG" /ppc
MTL_PROJ=/nologo /D "NDEBUG" /ppc 
CPP=cl.exe
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "_MAC" /D "_MPPC_" /D "_WINDOWS" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_AFXDLL" /Yu"stdafx.h" /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "_MAC" /D "_MPPC_" /D "_WINDOWS" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_AFXDLL" /Yu"stdafx.h" /c
CPP_PROJ=/nologo /MD /W3 /GX /O2 /D "_MAC" /D "_MPPC_" /D "_WINDOWS" /D "WIN32"\
 /D "NDEBUG" /D "_MBCS" /D "_AFXDLL" /Fp"$(INTDIR)/dlgtemp.pch" /Yu"stdafx.h"\
 /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\PMacRel/
CPP_SBRS=.\.

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
# ADD BASE RSC /l 0x409 /r /d "_MAC" /d "_MPPC_" /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /r /d "_MAC" /d "_MPPC_" /d "NDEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /r /m /fo"$(INTDIR)/dlgtemp.rsc" /d "_MAC" /d "_MPPC_" /d\
 "NDEBUG" /d "_AFXDLL" 
MRC=mrc.exe
# ADD BASE MRC /D "_MPPC_" /D "_MAC" /D "NDEBUG" /NOLOGO
# ADD MRC /D "_MPPC_" /D "_MAC" /D "NDEBUG" /NOLOGO
MRC_PROJ=/D "_MPPC_" /D "_MAC" /D "NDEBUG" /l 0x409 /NOLOGO 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/dlgtemp.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 /nologo /mac:bundle /machine:MPPC
# ADD LINK32 /nologo /mac:bundle /mac:creator="DLGT" /machine:MPPC
LINK32_FLAGS=/nologo /mac:bundle /mac:type="APPL" /mac:creator="DLGT"\
 /incremental:no /pdb:"$(OUTDIR)/dlgtemp.pdb" /machine:MPPC\
 /out:"$(OUTDIR)/dlgtemp.exe" 
LINK32_OBJS= \
	"$(INTDIR)\dlgtemp.obj" \
	"$(INTDIR)\dlgtemp.rsc" \
	"$(INTDIR)\dlgtmac.rsc" \
	"$(INTDIR)\itemtemp.obj" \
	"$(INTDIR)\setupdlg.obj" \
	"$(INTDIR)\StdAfx.obj"

"$(OUTDIR)\dlgtemp.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

MFILE32=mfile.exe
# ADD BASE MFILE32 COPY /NOLOGO
# ADD MFILE32 COPY /NOLOGO
MFILE32_FLAGS=COPY /NOLOGO 
MFILE32_FILES= \
	"$(OUTDIR)\dlgtemp.exe"

"$(OUTDIR)\dlgtemp.trg" : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\PMacRel\dlgtemp.exe\
 "$(MFILE32_DEST):dlgtemp.exe">"$(OUTDIR)\dlgtemp.trg"

DOWNLOAD : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\PMacRel\dlgtemp.exe\
 "$(MFILE32_DEST):dlgtemp.exe">"$(OUTDIR)\dlgtemp.trg"

!ELSEIF  "$(CFG)" == "dlgtemp - Power Macintosh Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "PMacDbg"
# PROP BASE Intermediate_Dir "PMacDbg"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "PMacDbg"
# PROP Intermediate_Dir "PMacDbg"
# PROP Target_Dir ""
OUTDIR=.\PMacDbg
INTDIR=.\PMacDbg

ALL : "$(OUTDIR)\dlgtemp.exe" "$(OUTDIR)\dlgtemp.trg"

CLEAN : 
	-@erase "$(INTDIR)\dlgtemp.obj"
	-@erase "$(INTDIR)\dlgtemp.pch"
	-@erase "$(INTDIR)\dlgtemp.rsc"
	-@erase "$(INTDIR)\dlgtmac.rsc"
	-@erase "$(INTDIR)\itemtemp.obj"
	-@erase "$(INTDIR)\setupdlg.obj"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\vc40.pdb"
	-@erase "$(OUTDIR)\dlgtemp.exe"
	-@erase "$(OUTDIR)\dlgtemp.ilk"
	-@erase "$(OUTDIR)\dlgtemp.pdb"
	-@erase "$(OUTDIR)\dlgtemp.trg"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

MTL=mktyplib.exe
# ADD BASE MTL /nologo /D "_DEBUG" /ppc
# ADD MTL /nologo /D "_DEBUG" /ppc
MTL_PROJ=/nologo /D "_DEBUG" /ppc 
CPP=cl.exe
# ADD BASE CPP /nologo /MDd /W3 /GX /Zi /Od /D "_MAC" /D "_MPPC_" /D "_WINDOWS" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_AFXDLL" /Yu"stdafx.h" /QPm /c
# ADD CPP /nologo /MDd /W3 /GX /Zi /Od /D "_MAC" /D "_MPPC_" /D "_WINDOWS" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_AFXDLL" /Yu"stdafx.h" /QPm /c
CPP_PROJ=/nologo /MDd /W3 /GX /Zi /Od /D "_MAC" /D "_MPPC_" /D "_WINDOWS" /D\
 "WIN32" /D "_DEBUG" /D "_MBCS" /D "_AFXDLL" /Fp"$(INTDIR)/dlgtemp.pch"\
 /Yu"stdafx.h" /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /QPm /c 
CPP_OBJS=.\PMacDbg/
CPP_SBRS=.\.

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
# ADD BASE RSC /l 0x409 /r /d "_MAC" /d "_MPPC_" /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /r /d "_MAC" /d "_MPPC_" /d "_DEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /r /m /fo"$(INTDIR)/dlgtemp.rsc" /d "_MAC" /d "_MPPC_" /d\
 "_DEBUG" /d "_AFXDLL" 
MRC=mrc.exe
# ADD BASE MRC /D "_MPPC_" /D "_MAC" /D "_DEBUG" /NOLOGO
# ADD MRC /D "_MPPC_" /D "_MAC" /D "_DEBUG" /NOLOGO
MRC_PROJ=/D "_MPPC_" /D "_MAC" /D "_DEBUG" /l 0x409 /NOLOGO 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/dlgtemp.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 /nologo /mac:bundle /debug /machine:MPPC
# ADD LINK32 /nologo /mac:bundle /mac:creator="DLGT" /debug /machine:MPPC
LINK32_FLAGS=/nologo /mac:bundle /mac:type="APPL" /mac:creator="DLGT"\
 /incremental:yes /pdb:"$(OUTDIR)/dlgtemp.pdb" /debug /machine:MPPC\
 /out:"$(OUTDIR)/dlgtemp.exe" 
LINK32_OBJS= \
	"$(INTDIR)\dlgtemp.obj" \
	"$(INTDIR)\dlgtemp.rsc" \
	"$(INTDIR)\dlgtmac.rsc" \
	"$(INTDIR)\itemtemp.obj" \
	"$(INTDIR)\setupdlg.obj" \
	"$(INTDIR)\StdAfx.obj"

"$(OUTDIR)\dlgtemp.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

MFILE32=mfile.exe
# ADD BASE MFILE32 COPY /NOLOGO
# ADD MFILE32 COPY /NOLOGO
MFILE32_FLAGS=COPY /NOLOGO 
MFILE32_FILES= \
	"$(OUTDIR)\dlgtemp.exe"

"$(OUTDIR)\dlgtemp.trg" : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\PMacDbg\dlgtemp.exe\
 "$(MFILE32_DEST):dlgtemp.exe">"$(OUTDIR)\dlgtemp.trg"

DOWNLOAD : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\PMacDbg\dlgtemp.exe\
 "$(MFILE32_DEST):dlgtemp.exe">"$(OUTDIR)\dlgtemp.trg"

!ENDIF 

################################################################################
# Begin Target

# Name "dlgtemp - Win32 Release"
# Name "dlgtemp - Win32 Debug"
# Name "dlgtemp - Power Macintosh Release"
# Name "dlgtemp - Power Macintosh Debug"

!IF  "$(CFG)" == "dlgtemp - Win32 Release"

!ELSEIF  "$(CFG)" == "dlgtemp - Win32 Debug"

!ELSEIF  "$(CFG)" == "dlgtemp - Power Macintosh Release"

!ELSEIF  "$(CFG)" == "dlgtemp - Power Macintosh Debug"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\ReadMe.txt

!IF  "$(CFG)" == "dlgtemp - Win32 Release"

!ELSEIF  "$(CFG)" == "dlgtemp - Win32 Debug"

!ELSEIF  "$(CFG)" == "dlgtemp - Power Macintosh Release"

!ELSEIF  "$(CFG)" == "dlgtemp - Power Macintosh Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\dlgtemp.cpp

!IF  "$(CFG)" == "dlgtemp - Win32 Release"

DEP_CPP_DLGTE=\
	".\dlgtemp.h"\
	".\itemtemp.h"\
	".\setupdlg.h"\
	".\StdAfx.h"\
	

"$(INTDIR)\dlgtemp.obj" : $(SOURCE) $(DEP_CPP_DLGTE) "$(INTDIR)"\
 "$(INTDIR)\dlgtemp.pch"


!ELSEIF  "$(CFG)" == "dlgtemp - Win32 Debug"

DEP_CPP_DLGTE=\
	".\dlgtemp.h"\
	".\itemtemp.h"\
	".\setupdlg.h"\
	".\StdAfx.h"\
	

"$(INTDIR)\dlgtemp.obj" : $(SOURCE) $(DEP_CPP_DLGTE) "$(INTDIR)"\
 "$(INTDIR)\dlgtemp.pch"

"$(INTDIR)\dlgtemp.sbr" : $(SOURCE) $(DEP_CPP_DLGTE) "$(INTDIR)"\
 "$(INTDIR)\dlgtemp.pch"


!ELSEIF  "$(CFG)" == "dlgtemp - Power Macintosh Release"

DEP_CPP_DLGTE=\
	".\dlgtemp.h"\
	".\itemtemp.h"\
	".\setupdlg.h"\
	".\StdAfx.h"\
	

"$(INTDIR)\dlgtemp.obj" : $(SOURCE) $(DEP_CPP_DLGTE) "$(INTDIR)"\
 "$(INTDIR)\dlgtemp.pch"


!ELSEIF  "$(CFG)" == "dlgtemp - Power Macintosh Debug"

DEP_CPP_DLGTE=\
	".\dlgtemp.h"\
	".\itemtemp.h"\
	".\setupdlg.h"\
	".\StdAfx.h"\
	

"$(INTDIR)\dlgtemp.obj" : $(SOURCE) $(DEP_CPP_DLGTE) "$(INTDIR)"\
 "$(INTDIR)\dlgtemp.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\StdAfx.cpp

!IF  "$(CFG)" == "dlgtemp - Win32 Release"

DEP_CPP_STDAF=\
	".\StdAfx.h"\
	
# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/dlgtemp.pch" /Yc"stdafx.h" /Fo"$(INTDIR)/"\
 /c $(SOURCE) \
	

"$(INTDIR)\StdAfx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\dlgtemp.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "dlgtemp - Win32 Debug"

DEP_CPP_STDAF=\
	".\StdAfx.h"\
	
# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /D "_AFXDLL" /D "_MBCS" /FR"$(INTDIR)/" /Fp"$(INTDIR)/dlgtemp.pch"\
 /Yc"stdafx.h" /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\StdAfx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\StdAfx.sbr" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\dlgtemp.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "dlgtemp - Power Macintosh Release"

DEP_CPP_STDAF=\
	".\StdAfx.h"\
	
# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MD /W3 /GX /O2 /D "_MAC" /D "_MPPC_" /D "_WINDOWS" /D "WIN32"\
 /D "NDEBUG" /D "_MBCS" /D "_AFXDLL" /Fp"$(INTDIR)/dlgtemp.pch" /Yc"stdafx.h"\
 /Fo"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\StdAfx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\dlgtemp.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "dlgtemp - Power Macintosh Debug"

DEP_CPP_STDAF=\
	".\StdAfx.h"\
	
# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MDd /W3 /GX /Zi /Od /D "_MAC" /D "_MPPC_" /D "_WINDOWS" /D\
 "WIN32" /D "_DEBUG" /D "_MBCS" /D "_AFXDLL" /Fp"$(INTDIR)/dlgtemp.pch"\
 /Yc"stdafx.h" /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /QPm /c $(SOURCE) \
	

"$(INTDIR)\StdAfx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\dlgtemp.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\dlgtemp.rc

!IF  "$(CFG)" == "dlgtemp - Win32 Release"

DEP_RSC_DLGTEM=\
	".\res\dlgtemp.ico"\
	".\res\dlgtemp.rc2"\
	

"$(INTDIR)\dlgtemp.res" : $(SOURCE) $(DEP_RSC_DLGTEM) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "dlgtemp - Win32 Debug"

DEP_RSC_DLGTEM=\
	".\res\dlgtemp.ico"\
	".\res\dlgtemp.rc2"\
	

"$(INTDIR)\dlgtemp.res" : $(SOURCE) $(DEP_RSC_DLGTEM) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "dlgtemp - Power Macintosh Release"

DEP_RSC_DLGTEM=\
	".\res\dlgtemp.ico"\
	".\res\dlgtemp.rc2"\
	

"$(INTDIR)\dlgtemp.rsc" : $(SOURCE) $(DEP_RSC_DLGTEM) "$(INTDIR)"
   $(RSC) /l 0x409 /r /m /fo"$(INTDIR)/dlgtemp.rsc" /d "_MAC" /d "_MPPC_" /d\
 "NDEBUG" /d "_AFXDLL" $(SOURCE)


!ELSEIF  "$(CFG)" == "dlgtemp - Power Macintosh Debug"

DEP_RSC_DLGTEM=\
	".\res\dlgtemp.ico"\
	".\res\dlgtemp.rc2"\
	

"$(INTDIR)\dlgtemp.rsc" : $(SOURCE) $(DEP_RSC_DLGTEM) "$(INTDIR)"
   $(RSC) /l 0x409 /r /m /fo"$(INTDIR)/dlgtemp.rsc" /d "_MAC" /d "_MPPC_" /d\
 "_DEBUG" /d "_AFXDLL" $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\itemtemp.cpp

!IF  "$(CFG)" == "dlgtemp - Win32 Release"

DEP_CPP_ITEMT=\
	".\itemtemp.h"\
	".\StdAfx.h"\
	

"$(INTDIR)\itemtemp.obj" : $(SOURCE) $(DEP_CPP_ITEMT) "$(INTDIR)"\
 "$(INTDIR)\dlgtemp.pch"


!ELSEIF  "$(CFG)" == "dlgtemp - Win32 Debug"

DEP_CPP_ITEMT=\
	".\itemtemp.h"\
	".\StdAfx.h"\
	

"$(INTDIR)\itemtemp.obj" : $(SOURCE) $(DEP_CPP_ITEMT) "$(INTDIR)"\
 "$(INTDIR)\dlgtemp.pch"

"$(INTDIR)\itemtemp.sbr" : $(SOURCE) $(DEP_CPP_ITEMT) "$(INTDIR)"\
 "$(INTDIR)\dlgtemp.pch"


!ELSEIF  "$(CFG)" == "dlgtemp - Power Macintosh Release"

DEP_CPP_ITEMT=\
	".\itemtemp.h"\
	".\StdAfx.h"\
	

"$(INTDIR)\itemtemp.obj" : $(SOURCE) $(DEP_CPP_ITEMT) "$(INTDIR)"\
 "$(INTDIR)\dlgtemp.pch"


!ELSEIF  "$(CFG)" == "dlgtemp - Power Macintosh Debug"

DEP_CPP_ITEMT=\
	".\itemtemp.h"\
	".\StdAfx.h"\
	

"$(INTDIR)\itemtemp.obj" : $(SOURCE) $(DEP_CPP_ITEMT) "$(INTDIR)"\
 "$(INTDIR)\dlgtemp.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\setupdlg.cpp

!IF  "$(CFG)" == "dlgtemp - Win32 Release"

DEP_CPP_SETUP=\
	".\dlgtemp.h"\
	".\itemtemp.h"\
	".\setupdlg.h"\
	".\StdAfx.h"\
	

"$(INTDIR)\setupdlg.obj" : $(SOURCE) $(DEP_CPP_SETUP) "$(INTDIR)"\
 "$(INTDIR)\dlgtemp.pch"


!ELSEIF  "$(CFG)" == "dlgtemp - Win32 Debug"

DEP_CPP_SETUP=\
	".\dlgtemp.h"\
	".\itemtemp.h"\
	".\setupdlg.h"\
	".\StdAfx.h"\
	

"$(INTDIR)\setupdlg.obj" : $(SOURCE) $(DEP_CPP_SETUP) "$(INTDIR)"\
 "$(INTDIR)\dlgtemp.pch"

"$(INTDIR)\setupdlg.sbr" : $(SOURCE) $(DEP_CPP_SETUP) "$(INTDIR)"\
 "$(INTDIR)\dlgtemp.pch"


!ELSEIF  "$(CFG)" == "dlgtemp - Power Macintosh Release"

DEP_CPP_SETUP=\
	".\dlgtemp.h"\
	".\itemtemp.h"\
	".\setupdlg.h"\
	".\StdAfx.h"\
	

"$(INTDIR)\setupdlg.obj" : $(SOURCE) $(DEP_CPP_SETUP) "$(INTDIR)"\
 "$(INTDIR)\dlgtemp.pch"


!ELSEIF  "$(CFG)" == "dlgtemp - Power Macintosh Debug"

DEP_CPP_SETUP=\
	".\dlgtemp.h"\
	".\itemtemp.h"\
	".\setupdlg.h"\
	".\StdAfx.h"\
	

"$(INTDIR)\setupdlg.obj" : $(SOURCE) $(DEP_CPP_SETUP) "$(INTDIR)"\
 "$(INTDIR)\dlgtemp.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\dlgtmac.r

!IF  "$(CFG)" == "dlgtemp - Win32 Release"

!ELSEIF  "$(CFG)" == "dlgtemp - Win32 Debug"

!ELSEIF  "$(CFG)" == "dlgtemp - Power Macintosh Release"


"$(INTDIR)\dlgtmac.rsc" : $(SOURCE) "$(INTDIR)"
   $(MRC) /o"$(INTDIR)/dlgtmac.rsc" /D "_MPPC_" /D "_MAC" /D "NDEBUG" /l 0x409\
 /NOLOGO $(SOURCE)


!ELSEIF  "$(CFG)" == "dlgtemp - Power Macintosh Debug"


"$(INTDIR)\dlgtmac.rsc" : $(SOURCE) "$(INTDIR)"
   $(MRC) /o"$(INTDIR)/dlgtmac.rsc" /D "_MPPC_" /D "_MAC" /D "_DEBUG" /l 0x409\
 /NOLOGO $(SOURCE)


!ENDIF 

# End Source File
# End Target
# End Project
################################################################################
