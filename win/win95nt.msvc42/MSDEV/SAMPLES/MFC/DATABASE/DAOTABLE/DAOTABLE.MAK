# Microsoft Developer Studio Generated NMAKE File, Format Version 4.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

!IF "$(CFG)" == ""
CFG=DAOTable - Win32 Debug
!MESSAGE No configuration specified.  Defaulting to DAOTable - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "DAOTable - Win32 Release" && "$(CFG)" !=\
 "DAOTable - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "DAOTable.mak" CFG="DAOTable - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "DAOTable - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "DAOTable - Win32 Debug" (based on "Win32 (x86) Application")
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
# PROP Target_Last_Scanned "DAOTable - Win32 Debug"
MTL=mktyplib.exe
RSC=rc.exe
CPP=cl.exe

!IF  "$(CFG)" == "DAOTable - Win32 Release"

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

ALL : "$(OUTDIR)\DAOTable.exe" "$(OUTDIR)\DAOTable.hlp"

CLEAN : 
	-@erase ".\Release\DAOTable.exe"
	-@erase ".\Release\field.obj"
	-@erase ".\Release\DAOTable.pch"
	-@erase ".\Release\AddIxDlg.obj"
	-@erase ".\Release\tabledef.obj"
	-@erase ".\Release\StdAfx.obj"
	-@erase ".\Release\database.obj"
	-@erase ".\Release\AddTbDlg.obj"
	-@erase ".\Release\AddDbDlg.obj"
	-@erase ".\Release\AddQyDlg.obj"
	-@erase ".\Release\DAOTable.obj"
	-@erase ".\Release\ListCtrl.obj"
	-@erase ".\Release\DAOTDlg.obj"
	-@erase ".\Release\index.obj"
	-@erase ".\Release\querydef.obj"
	-@erase ".\Release\DAOTable.res"
	-@erase ".\Release\DAOTable.hlp"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
CPP_PROJ=/nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/DAOTable.pch" /Yu"stdafx.h" /Fo"$(INTDIR)/"\
 /c 
CPP_OBJS=.\Release/
CPP_SBRS=
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/DAOTable.res" /d "NDEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/DAOTable.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 /nologo /subsystem:windows /machine:I386
LINK32_FLAGS=/nologo /subsystem:windows /incremental:no\
 /pdb:"$(OUTDIR)/DAOTable.pdb" /machine:I386 /out:"$(OUTDIR)/DAOTable.exe" 
LINK32_OBJS= \
	"$(INTDIR)/field.obj" \
	"$(INTDIR)/AddIxDlg.obj" \
	"$(INTDIR)/tabledef.obj" \
	"$(INTDIR)/StdAfx.obj" \
	"$(INTDIR)/database.obj" \
	"$(INTDIR)/AddTbDlg.obj" \
	"$(INTDIR)/AddDbDlg.obj" \
	"$(INTDIR)/AddQyDlg.obj" \
	"$(INTDIR)/DAOTable.obj" \
	"$(INTDIR)/ListCtrl.obj" \
	"$(INTDIR)/DAOTDlg.obj" \
	"$(INTDIR)/index.obj" \
	"$(INTDIR)/querydef.obj" \
	"$(INTDIR)/DAOTable.res"

"$(OUTDIR)\DAOTable.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "DAOTable - Win32 Debug"

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

ALL : "$(OUTDIR)\DAOTable.exe" "$(OUTDIR)\DAOTable.hlp"

CLEAN : 
	-@erase ".\Debug\vc40.pdb"
	-@erase ".\Debug\DAOTable.pch"
	-@erase ".\Debug\vc40.idb"
	-@erase ".\Debug\DAOTable.exe"
	-@erase ".\Debug\DAOTable.obj"
	-@erase ".\Debug\AddIxDlg.obj"
	-@erase ".\Debug\ListCtrl.obj"
	-@erase ".\Debug\index.obj"
	-@erase ".\Debug\querydef.obj"
	-@erase ".\Debug\DAOTDlg.obj"
	-@erase ".\Debug\tabledef.obj"
	-@erase ".\Debug\database.obj"
	-@erase ".\Debug\AddTbDlg.obj"
	-@erase ".\Debug\AddDbDlg.obj"
	-@erase ".\Debug\AddQyDlg.obj"
	-@erase ".\Debug\field.obj"
	-@erase ".\Debug\StdAfx.obj"
	-@erase ".\Debug\DAOTable.res"
	-@erase ".\Debug\DAOTable.ilk"
	-@erase ".\Debug\DAOTable.pdb"
	-@erase ".\Debug\DAOTable.hlp"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
CPP_PROJ=/nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/DAOTable.pch" /Yu"stdafx.h"\
 /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\Debug/
CPP_SBRS=
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/DAOTable.res" /d "_DEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/DAOTable.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386
# ADD LINK32 /nologo /subsystem:windows /debug /machine:I386
LINK32_FLAGS=/nologo /subsystem:windows /incremental:yes\
 /pdb:"$(OUTDIR)/DAOTable.pdb" /debug /machine:I386\
 /out:"$(OUTDIR)/DAOTable.exe" 
LINK32_OBJS= \
	"$(INTDIR)/DAOTable.obj" \
	"$(INTDIR)/AddIxDlg.obj" \
	"$(INTDIR)/ListCtrl.obj" \
	"$(INTDIR)/index.obj" \
	"$(INTDIR)/querydef.obj" \
	"$(INTDIR)/DAOTDlg.obj" \
	"$(INTDIR)/tabledef.obj" \
	"$(INTDIR)/database.obj" \
	"$(INTDIR)/AddTbDlg.obj" \
	"$(INTDIR)/AddDbDlg.obj" \
	"$(INTDIR)/AddQyDlg.obj" \
	"$(INTDIR)/field.obj" \
	"$(INTDIR)/StdAfx.obj" \
	"$(INTDIR)/DAOTable.res"

"$(OUTDIR)\DAOTable.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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

# Name "DAOTable - Win32 Release"
# Name "DAOTable - Win32 Debug"

!IF  "$(CFG)" == "DAOTable - Win32 Release"

!ELSEIF  "$(CFG)" == "DAOTable - Win32 Debug"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\ReadMe.txt

!IF  "$(CFG)" == "DAOTable - Win32 Release"

!ELSEIF  "$(CFG)" == "DAOTable - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\DAOTable.cpp
DEP_CPP_DAOTA=\
	".\StdAfx.h"\
	".\DAOTable.h"\
	".\DAOTDlg.h"\
	

"$(INTDIR)\DAOTable.obj" : $(SOURCE) $(DEP_CPP_DAOTA) "$(INTDIR)"\
 "$(INTDIR)\DAOTable.pch"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\DAOTDlg.cpp
DEP_CPP_DAOTD=\
	".\StdAfx.h"\
	".\DAOTable.h"\
	".\DAOTDlg.h"\
	".\database.h"\
	".\tabledef.h"\
	".\querydef.h"\
	".\addtbdlg.h"\
	".\ListCtrl.h"\
	".\AddIxDlg.h"\
	".\AddQyDlg.h"\
	".\AddDbDlg.h"\
	

"$(INTDIR)\DAOTDlg.obj" : $(SOURCE) $(DEP_CPP_DAOTD) "$(INTDIR)"\
 "$(INTDIR)\DAOTable.pch"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\StdAfx.cpp
DEP_CPP_STDAF=\
	".\StdAfx.h"\
	

!IF  "$(CFG)" == "DAOTable - Win32 Release"

# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/DAOTable.pch" /Yc"stdafx.h" /Fo"$(INTDIR)/"\
 /c $(SOURCE) \
	

"$(INTDIR)\StdAfx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\DAOTable.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "DAOTable - Win32 Debug"

# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/DAOTable.pch" /Yc"stdafx.h"\
 /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\StdAfx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\DAOTable.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\DAOTable.rc
DEP_RSC_DAOTAB=\
	".\res\DAOTable.ico"\
	".\resource.hm"\
	".\res\DAOTable.rc2"\
	

"$(INTDIR)\DAOTable.res" : $(SOURCE) $(DEP_RSC_DAOTAB) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\hlp\DAOTable.hpj

!IF  "$(CFG)" == "DAOTable - Win32 Release"

# Begin Custom Build - Making help file...
OutDir=.\Release
ProjDir=.
TargetName=DAOTable
InputPath=.\hlp\DAOTable.hpj

"$(OutDir)\$(TargetName).hlp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   "$(ProjDir)\makehelp.bat"

# End Custom Build

!ELSEIF  "$(CFG)" == "DAOTable - Win32 Debug"

# Begin Custom Build - Making help file...
OutDir=.\Debug
ProjDir=.
TargetName=DAOTable
InputPath=.\hlp\DAOTable.hpj

"$(OutDir)\$(TargetName).hlp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   "$(ProjDir)\makehelp.bat"

# End Custom Build

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\database.cpp
DEP_CPP_DATAB=\
	".\StdAfx.h"\
	".\database.h"\
	

"$(INTDIR)\database.obj" : $(SOURCE) $(DEP_CPP_DATAB) "$(INTDIR)"\
 "$(INTDIR)\DAOTable.pch"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\tabledef.cpp
DEP_CPP_TABLE=\
	".\StdAfx.h"\
	".\tabledef.h"\
	

"$(INTDIR)\tabledef.obj" : $(SOURCE) $(DEP_CPP_TABLE) "$(INTDIR)"\
 "$(INTDIR)\DAOTable.pch"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\AddTbDlg.cpp
DEP_CPP_ADDTB=\
	".\StdAfx.h"\
	".\DAOTable.h"\
	".\tabledef.h"\
	".\field.h"\
	".\addtbdlg.h"\
	

"$(INTDIR)\AddTbDlg.obj" : $(SOURCE) $(DEP_CPP_ADDTB) "$(INTDIR)"\
 "$(INTDIR)\DAOTable.pch"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\field.cpp
DEP_CPP_FIELD=\
	".\StdAfx.h"\
	".\field.h"\
	

"$(INTDIR)\field.obj" : $(SOURCE) $(DEP_CPP_FIELD) "$(INTDIR)"\
 "$(INTDIR)\DAOTable.pch"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\AddIxDlg.cpp
DEP_CPP_ADDIX=\
	".\StdAfx.h"\
	".\DAOTable.h"\
	".\AddIxDlg.h"\
	".\tabledef.h"\
	".\index.h"\
	".\field.h"\
	".\ListCtrl.h"\
	

"$(INTDIR)\AddIxDlg.obj" : $(SOURCE) $(DEP_CPP_ADDIX) "$(INTDIR)"\
 "$(INTDIR)\DAOTable.pch"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\index.cpp
DEP_CPP_INDEX=\
	".\StdAfx.h"\
	".\index.h"\
	

"$(INTDIR)\index.obj" : $(SOURCE) $(DEP_CPP_INDEX) "$(INTDIR)"\
 "$(INTDIR)\DAOTable.pch"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\AddQyDlg.cpp
DEP_CPP_ADDQY=\
	".\StdAfx.h"\
	".\DAOTable.h"\
	".\AddQyDlg.h"\
	".\querydef.h"\
	

"$(INTDIR)\AddQyDlg.obj" : $(SOURCE) $(DEP_CPP_ADDQY) "$(INTDIR)"\
 "$(INTDIR)\DAOTable.pch"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\querydef.cpp
DEP_CPP_QUERY=\
	".\StdAfx.h"\
	".\querydef.h"\
	

"$(INTDIR)\querydef.obj" : $(SOURCE) $(DEP_CPP_QUERY) "$(INTDIR)"\
 "$(INTDIR)\DAOTable.pch"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\ListCtrl.cpp
DEP_CPP_LISTC=\
	".\StdAfx.h"\
	".\DAOTable.h"\
	".\ListCtrl.h"\
	

"$(INTDIR)\ListCtrl.obj" : $(SOURCE) $(DEP_CPP_LISTC) "$(INTDIR)"\
 "$(INTDIR)\DAOTable.pch"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\AddDbDlg.cpp
DEP_CPP_ADDDB=\
	".\StdAfx.h"\
	".\DAOTable.h"\
	".\database.h"\
	".\AddDbDlg.h"\
	

"$(INTDIR)\AddDbDlg.obj" : $(SOURCE) $(DEP_CPP_ADDDB) "$(INTDIR)"\
 "$(INTDIR)\DAOTable.pch"


# End Source File
# End Target
# End Project
################################################################################
