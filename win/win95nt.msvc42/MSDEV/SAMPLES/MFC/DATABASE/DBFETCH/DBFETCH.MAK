# Microsoft Developer Studio Generated NMAKE File, Format Version 4.20
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

!IF "$(CFG)" == ""
CFG=dbfetch - Win32 UNICODE debug
!MESSAGE No configuration specified.  Defaulting to dbfetch - Win32 UNICODE\
 debug.
!ENDIF 

!IF "$(CFG)" != "dbfetch - Win32 Release" && "$(CFG)" !=\
 "dbfetch - Win32 Debug" && "$(CFG)" != "dbfetch - Win32 UNICODE debug" &&\
 "$(CFG)" != "dbfetch - Win32 UNICODE Release"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "dbfetch.mak" CFG="dbfetch - Win32 UNICODE debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "dbfetch - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "dbfetch - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE "dbfetch - Win32 UNICODE debug" (based on "Win32 (x86) Application")
!MESSAGE "dbfetch - Win32 UNICODE Release" (based on "Win32 (x86) Application")
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
# PROP Target_Last_Scanned "dbfetch - Win32 UNICODE Release"
MTL=mktyplib.exe
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "dbfetch - Win32 Release"

# PROP BASE Use_MFC 5
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

ALL : "$(OUTDIR)\dbfetch.exe"

CLEAN : 
	-@erase "$(INTDIR)\bulkset.obj"
	-@erase "$(INTDIR)\datadlg.obj"
	-@erase "$(INTDIR)\dbfetch.obj"
	-@erase "$(INTDIR)\dbfetch.pch"
	-@erase "$(INTDIR)\dbfetch.res"
	-@erase "$(INTDIR)\fetchDlg.obj"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(OUTDIR)\dbfetch.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Yu"stdafx.h" /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
CPP_PROJ=/nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/dbfetch.pch" /Yu"stdafx.h" /Fo"$(INTDIR)/"\
 /c 
CPP_OBJS=.\Release/
CPP_SBRS=.\.
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/dbfetch.res" /d "NDEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/dbfetch.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 /nologo /subsystem:windows /machine:I386
LINK32_FLAGS=/nologo /subsystem:windows /incremental:no\
 /pdb:"$(OUTDIR)/dbfetch.pdb" /machine:I386 /out:"$(OUTDIR)/dbfetch.exe" 
LINK32_OBJS= \
	"$(INTDIR)\bulkset.obj" \
	"$(INTDIR)\datadlg.obj" \
	"$(INTDIR)\dbfetch.obj" \
	"$(INTDIR)\dbfetch.res" \
	"$(INTDIR)\fetchDlg.obj" \
	"$(INTDIR)\StdAfx.obj"

"$(OUTDIR)\dbfetch.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "dbfetch - Win32 Debug"

# PROP BASE Use_MFC 5
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

ALL : "$(OUTDIR)\dbfetch.exe"

CLEAN : 
	-@erase "$(INTDIR)\bulkset.obj"
	-@erase "$(INTDIR)\datadlg.obj"
	-@erase "$(INTDIR)\dbfetch.obj"
	-@erase "$(INTDIR)\dbfetch.pch"
	-@erase "$(INTDIR)\dbfetch.res"
	-@erase "$(INTDIR)\fetchDlg.obj"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\vc40.idb"
	-@erase "$(INTDIR)\vc40.pdb"
	-@erase "$(OUTDIR)\dbfetch.exe"
	-@erase "$(OUTDIR)\dbfetch.ilk"
	-@erase "$(OUTDIR)\dbfetch.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /Yu"stdafx.h" /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
CPP_PROJ=/nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/dbfetch.pch" /Yu"stdafx.h"\
 /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\Debug/
CPP_SBRS=.\.
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/dbfetch.res" /d "_DEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/dbfetch.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386
# ADD LINK32 /nologo /subsystem:windows /debug /machine:I386
LINK32_FLAGS=/nologo /subsystem:windows /incremental:yes\
 /pdb:"$(OUTDIR)/dbfetch.pdb" /debug /machine:I386 /out:"$(OUTDIR)/dbfetch.exe" 
LINK32_OBJS= \
	"$(INTDIR)\bulkset.obj" \
	"$(INTDIR)\datadlg.obj" \
	"$(INTDIR)\dbfetch.obj" \
	"$(INTDIR)\dbfetch.res" \
	"$(INTDIR)\fetchDlg.obj" \
	"$(INTDIR)\StdAfx.obj"

"$(OUTDIR)\dbfetch.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "dbfetch - Win32 UNICODE debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "dbfetch_"
# PROP BASE Intermediate_Dir "dbfetch_"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "dbfetch_"
# PROP Intermediate_Dir "dbfetch_"
# PROP Target_Dir ""
OUTDIR=.\dbfetch_
INTDIR=.\dbfetch_

ALL : "$(OUTDIR)\dbfetch.exe"

CLEAN : 
	-@erase "$(INTDIR)\bulkset.obj"
	-@erase "$(INTDIR)\datadlg.obj"
	-@erase "$(INTDIR)\dbfetch.obj"
	-@erase "$(INTDIR)\dbfetch.pch"
	-@erase "$(INTDIR)\dbfetch.res"
	-@erase "$(INTDIR)\fetchDlg.obj"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\vc40.idb"
	-@erase "$(INTDIR)\vc40.pdb"
	-@erase "$(OUTDIR)\dbfetch.exe"
	-@erase "$(OUTDIR)\dbfetch.ilk"
	-@erase "$(OUTDIR)\dbfetch.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_AFXDLL" /D "_UNICODE" /Yu"stdafx.h" /c
CPP_PROJ=/nologo /MDd /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS"\
 /D "_AFXDLL" /D "_UNICODE" /Fp"$(INTDIR)/dbfetch.pch" /Yu"stdafx.h"\
 /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\dbfetch_/
CPP_SBRS=.\.
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/dbfetch.res" /d "_DEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/dbfetch.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386
# ADD LINK32 /nologo /entry:"wWinMainCRTStartup" /subsystem:windows /debug /machine:I386
LINK32_FLAGS=/nologo /entry:"wWinMainCRTStartup" /subsystem:windows\
 /incremental:yes /pdb:"$(OUTDIR)/dbfetch.pdb" /debug /machine:I386\
 /out:"$(OUTDIR)/dbfetch.exe" 
LINK32_OBJS= \
	"$(INTDIR)\bulkset.obj" \
	"$(INTDIR)\datadlg.obj" \
	"$(INTDIR)\dbfetch.obj" \
	"$(INTDIR)\dbfetch.res" \
	"$(INTDIR)\fetchDlg.obj" \
	"$(INTDIR)\StdAfx.obj"

"$(OUTDIR)\dbfetch.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "dbfetch - Win32 UNICODE Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "dbfetch0"
# PROP BASE Intermediate_Dir "dbfetch0"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "dbfetch0"
# PROP Intermediate_Dir "dbfetch0"
# PROP Target_Dir ""
OUTDIR=.\dbfetch0
INTDIR=.\dbfetch0

ALL : "$(OUTDIR)\dbfetch.exe"

CLEAN : 
	-@erase "$(INTDIR)\bulkset.obj"
	-@erase "$(INTDIR)\datadlg.obj"
	-@erase "$(INTDIR)\dbfetch.obj"
	-@erase "$(INTDIR)\dbfetch.pch"
	-@erase "$(INTDIR)\dbfetch.res"
	-@erase "$(INTDIR)\fetchDlg.obj"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(OUTDIR)\dbfetch.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_AFXDLL" /D "_UNICODE" /Yu"stdafx.h" /c
CPP_PROJ=/nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_UNICODE" /Fp"$(INTDIR)/dbfetch.pch" /Yu"stdafx.h"\
 /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\dbfetch0/
CPP_SBRS=.\.
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/dbfetch.res" /d "NDEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/dbfetch.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 /nologo /entry:"wWinMainCRTStartup" /subsystem:windows /machine:I386
LINK32_FLAGS=/nologo /entry:"wWinMainCRTStartup" /subsystem:windows\
 /incremental:no /pdb:"$(OUTDIR)/dbfetch.pdb" /machine:I386\
 /out:"$(OUTDIR)/dbfetch.exe" 
LINK32_OBJS= \
	"$(INTDIR)\bulkset.obj" \
	"$(INTDIR)\datadlg.obj" \
	"$(INTDIR)\dbfetch.obj" \
	"$(INTDIR)\dbfetch.res" \
	"$(INTDIR)\fetchDlg.obj" \
	"$(INTDIR)\StdAfx.obj"

"$(OUTDIR)\dbfetch.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 

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

################################################################################
# Begin Target

# Name "dbfetch - Win32 Release"
# Name "dbfetch - Win32 Debug"
# Name "dbfetch - Win32 UNICODE debug"
# Name "dbfetch - Win32 UNICODE Release"

!IF  "$(CFG)" == "dbfetch - Win32 Release"

!ELSEIF  "$(CFG)" == "dbfetch - Win32 Debug"

!ELSEIF  "$(CFG)" == "dbfetch - Win32 UNICODE debug"

!ELSEIF  "$(CFG)" == "dbfetch - Win32 UNICODE Release"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\dbfetch.cpp
DEP_CPP_DBFET=\
	".\dbfetch.h"\
	".\fetchDlg.h"\
	".\StdAfx.h"\
	

!IF  "$(CFG)" == "dbfetch - Win32 Release"


"$(INTDIR)\dbfetch.obj" : $(SOURCE) $(DEP_CPP_DBFET) "$(INTDIR)"\
 "$(INTDIR)\dbfetch.pch"


!ELSEIF  "$(CFG)" == "dbfetch - Win32 Debug"


"$(INTDIR)\dbfetch.obj" : $(SOURCE) $(DEP_CPP_DBFET) "$(INTDIR)"\
 "$(INTDIR)\dbfetch.pch"


!ELSEIF  "$(CFG)" == "dbfetch - Win32 UNICODE debug"


"$(INTDIR)\dbfetch.obj" : $(SOURCE) $(DEP_CPP_DBFET) "$(INTDIR)"\
 "$(INTDIR)\dbfetch.pch"


!ELSEIF  "$(CFG)" == "dbfetch - Win32 UNICODE Release"


"$(INTDIR)\dbfetch.obj" : $(SOURCE) $(DEP_CPP_DBFET) "$(INTDIR)"\
 "$(INTDIR)\dbfetch.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\fetchDlg.cpp
DEP_CPP_FETCH=\
	".\bulkset.h"\
	".\datadlg.h"\
	".\dbfetch.h"\
	".\fetchDlg.h"\
	".\StdAfx.h"\
	

!IF  "$(CFG)" == "dbfetch - Win32 Release"


"$(INTDIR)\fetchDlg.obj" : $(SOURCE) $(DEP_CPP_FETCH) "$(INTDIR)"\
 "$(INTDIR)\dbfetch.pch"


!ELSEIF  "$(CFG)" == "dbfetch - Win32 Debug"


"$(INTDIR)\fetchDlg.obj" : $(SOURCE) $(DEP_CPP_FETCH) "$(INTDIR)"\
 "$(INTDIR)\dbfetch.pch"


!ELSEIF  "$(CFG)" == "dbfetch - Win32 UNICODE debug"


"$(INTDIR)\fetchDlg.obj" : $(SOURCE) $(DEP_CPP_FETCH) "$(INTDIR)"\
 "$(INTDIR)\dbfetch.pch"


!ELSEIF  "$(CFG)" == "dbfetch - Win32 UNICODE Release"


"$(INTDIR)\fetchDlg.obj" : $(SOURCE) $(DEP_CPP_FETCH) "$(INTDIR)"\
 "$(INTDIR)\dbfetch.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\StdAfx.cpp
DEP_CPP_STDAF=\
	".\StdAfx.h"\
	

!IF  "$(CFG)" == "dbfetch - Win32 Release"

# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/dbfetch.pch" /Yc"stdafx.h" /Fo"$(INTDIR)/"\
 /c $(SOURCE) \
	

"$(INTDIR)\StdAfx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\dbfetch.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "dbfetch - Win32 Debug"

# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/dbfetch.pch" /Yc"stdafx.h"\
 /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\StdAfx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\dbfetch.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "dbfetch - Win32 UNICODE debug"

# ADD BASE CPP /Yc"stdafx.h"
# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MDd /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS"\
 /D "_AFXDLL" /D "_UNICODE" /Fp"$(INTDIR)/dbfetch.pch" /Yc"stdafx.h"\
 /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\StdAfx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\dbfetch.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "dbfetch - Win32 UNICODE Release"

# ADD BASE CPP /Yc"stdafx.h"
# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_UNICODE" /Fp"$(INTDIR)/dbfetch.pch" /Yc"stdafx.h"\
 /Fo"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\StdAfx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\dbfetch.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\dbfetch.rc
DEP_RSC_DBFETC=\
	".\res\dbfetch.ico"\
	".\res\dbfetch.rc2"\
	

!IF  "$(CFG)" == "dbfetch - Win32 Release"


"$(INTDIR)\dbfetch.res" : $(SOURCE) $(DEP_RSC_DBFETC) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "dbfetch - Win32 Debug"


"$(INTDIR)\dbfetch.res" : $(SOURCE) $(DEP_RSC_DBFETC) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "dbfetch - Win32 UNICODE debug"


"$(INTDIR)\dbfetch.res" : $(SOURCE) $(DEP_RSC_DBFETC) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "dbfetch - Win32 UNICODE Release"


"$(INTDIR)\dbfetch.res" : $(SOURCE) $(DEP_RSC_DBFETC) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\datadlg.cpp
DEP_CPP_DATAD=\
	".\bulkset.h"\
	".\datadlg.h"\
	".\dbfetch.h"\
	".\StdAfx.h"\
	

!IF  "$(CFG)" == "dbfetch - Win32 Release"


"$(INTDIR)\datadlg.obj" : $(SOURCE) $(DEP_CPP_DATAD) "$(INTDIR)"\
 "$(INTDIR)\dbfetch.pch"


!ELSEIF  "$(CFG)" == "dbfetch - Win32 Debug"


"$(INTDIR)\datadlg.obj" : $(SOURCE) $(DEP_CPP_DATAD) "$(INTDIR)"\
 "$(INTDIR)\dbfetch.pch"


!ELSEIF  "$(CFG)" == "dbfetch - Win32 UNICODE debug"


"$(INTDIR)\datadlg.obj" : $(SOURCE) $(DEP_CPP_DATAD) "$(INTDIR)"\
 "$(INTDIR)\dbfetch.pch"


!ELSEIF  "$(CFG)" == "dbfetch - Win32 UNICODE Release"


"$(INTDIR)\datadlg.obj" : $(SOURCE) $(DEP_CPP_DATAD) "$(INTDIR)"\
 "$(INTDIR)\dbfetch.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\bulkset.cpp
DEP_CPP_BULKS=\
	".\bulkset.h"\
	".\dbfetch.h"\
	".\StdAfx.h"\
	

!IF  "$(CFG)" == "dbfetch - Win32 Release"


"$(INTDIR)\bulkset.obj" : $(SOURCE) $(DEP_CPP_BULKS) "$(INTDIR)"\
 "$(INTDIR)\dbfetch.pch"


!ELSEIF  "$(CFG)" == "dbfetch - Win32 Debug"


"$(INTDIR)\bulkset.obj" : $(SOURCE) $(DEP_CPP_BULKS) "$(INTDIR)"\
 "$(INTDIR)\dbfetch.pch"


!ELSEIF  "$(CFG)" == "dbfetch - Win32 UNICODE debug"


"$(INTDIR)\bulkset.obj" : $(SOURCE) $(DEP_CPP_BULKS) "$(INTDIR)"\
 "$(INTDIR)\dbfetch.pch"


!ELSEIF  "$(CFG)" == "dbfetch - Win32 UNICODE Release"


"$(INTDIR)\bulkset.obj" : $(SOURCE) $(DEP_CPP_BULKS) "$(INTDIR)"\
 "$(INTDIR)\dbfetch.pch"


!ENDIF 

# End Source File
# End Target
# End Project
################################################################################
