# Microsoft Developer Studio Generated NMAKE File, Format Version 4.10
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

!IF "$(CFG)" == ""
CFG=RowList - Win32 Debug
!MESSAGE No configuration specified.  Defaulting to RowList - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "RowList - Win32 Release" && "$(CFG)" !=\
 "RowList - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "rowlist.mak" CFG="RowList - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "RowList - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "RowList - Win32 Debug" (based on "Win32 (x86) Application")
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
# PROP Target_Last_Scanned "RowList - Win32 Debug"
RSC=rc.exe
MTL=mktyplib.exe
CPP=cl.exe

!IF  "$(CFG)" == "RowList - Win32 Release"

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

ALL : "$(OUTDIR)\rowlist.exe"

CLEAN : 
	-@erase "$(INTDIR)\Listvwex.obj"
	-@erase "$(INTDIR)\MainFrm.obj"
	-@erase "$(INTDIR)\RListDoc.obj"
	-@erase "$(INTDIR)\RListVw.obj"
	-@erase "$(INTDIR)\RowList.obj"
	-@erase "$(INTDIR)\rowlist.pch"
	-@erase "$(INTDIR)\RowList.res"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(OUTDIR)\rowlist.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
CPP_PROJ=/nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/rowlist.pch" /Yu"stdafx.h" /Fo"$(INTDIR)/"\
 /c 
CPP_OBJS=.\Release/
CPP_SBRS=.\.
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/RowList.res" /d "NDEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/rowlist.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 /nologo /subsystem:windows /machine:I386
LINK32_FLAGS=/nologo /subsystem:windows /incremental:no\
 /pdb:"$(OUTDIR)/rowlist.pdb" /machine:I386 /out:"$(OUTDIR)/rowlist.exe" 
LINK32_OBJS= \
	"$(INTDIR)\Listvwex.obj" \
	"$(INTDIR)\MainFrm.obj" \
	"$(INTDIR)\RListDoc.obj" \
	"$(INTDIR)\RListVw.obj" \
	"$(INTDIR)\RowList.obj" \
	"$(INTDIR)\RowList.res" \
	"$(INTDIR)\StdAfx.obj"

"$(OUTDIR)\rowlist.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "RowList - Win32 Debug"

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

ALL : "$(OUTDIR)\rowlist.exe"

CLEAN : 
	-@erase "$(INTDIR)\Listvwex.obj"
	-@erase "$(INTDIR)\MainFrm.obj"
	-@erase "$(INTDIR)\RListDoc.obj"
	-@erase "$(INTDIR)\RListVw.obj"
	-@erase "$(INTDIR)\RowList.obj"
	-@erase "$(INTDIR)\rowlist.pch"
	-@erase "$(INTDIR)\RowList.res"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\vc40.idb"
	-@erase "$(INTDIR)\vc40.pdb"
	-@erase "$(OUTDIR)\rowlist.exe"
	-@erase "$(OUTDIR)\rowlist.ilk"
	-@erase "$(OUTDIR)\rowlist.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
CPP_PROJ=/nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/rowlist.pch" /Yu"stdafx.h"\
 /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\Debug/
CPP_SBRS=.\.
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/RowList.res" /d "_DEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/rowlist.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386
# ADD LINK32 /nologo /subsystem:windows /debug /machine:I386
LINK32_FLAGS=/nologo /subsystem:windows /incremental:yes\
 /pdb:"$(OUTDIR)/rowlist.pdb" /debug /machine:I386 /out:"$(OUTDIR)/rowlist.exe" 
LINK32_OBJS= \
	"$(INTDIR)\Listvwex.obj" \
	"$(INTDIR)\MainFrm.obj" \
	"$(INTDIR)\RListDoc.obj" \
	"$(INTDIR)\RListVw.obj" \
	"$(INTDIR)\RowList.obj" \
	"$(INTDIR)\RowList.res" \
	"$(INTDIR)\StdAfx.obj"

"$(OUTDIR)\rowlist.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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

# Name "RowList - Win32 Release"
# Name "RowList - Win32 Debug"

!IF  "$(CFG)" == "RowList - Win32 Release"

!ELSEIF  "$(CFG)" == "RowList - Win32 Debug"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\ReadMe.txt

!IF  "$(CFG)" == "RowList - Win32 Release"

!ELSEIF  "$(CFG)" == "RowList - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\RowList.cpp
DEP_CPP_ROWLI=\
	".\ListVwEx.h"\
	".\MainFrm.h"\
	".\RListDoc.h"\
	".\RListVw.h"\
	".\RowList.h"\
	".\StdAfx.h"\
	

"$(INTDIR)\RowList.obj" : $(SOURCE) $(DEP_CPP_ROWLI) "$(INTDIR)"\
 "$(INTDIR)\rowlist.pch"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\StdAfx.cpp
DEP_CPP_STDAF=\
	".\StdAfx.h"\
	

!IF  "$(CFG)" == "RowList - Win32 Release"

# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/rowlist.pch" /Yc"stdafx.h" /Fo"$(INTDIR)/"\
 /c $(SOURCE) \
	

"$(INTDIR)\StdAfx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\rowlist.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "RowList - Win32 Debug"

# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/rowlist.pch" /Yc"stdafx.h"\
 /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\StdAfx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\rowlist.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\MainFrm.cpp
DEP_CPP_MAINF=\
	".\MainFrm.h"\
	".\RowList.h"\
	".\StdAfx.h"\
	

"$(INTDIR)\MainFrm.obj" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\rowlist.pch"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\RListDoc.cpp
DEP_CPP_RLIST=\
	".\RListDoc.h"\
	".\RowList.h"\
	".\StdAfx.h"\
	

"$(INTDIR)\RListDoc.obj" : $(SOURCE) $(DEP_CPP_RLIST) "$(INTDIR)"\
 "$(INTDIR)\rowlist.pch"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\RListVw.cpp
DEP_CPP_RLISTV=\
	".\ListVwEx.h"\
	".\RListDoc.h"\
	".\RListVw.h"\
	".\RowList.h"\
	".\StdAfx.h"\
	

"$(INTDIR)\RListVw.obj" : $(SOURCE) $(DEP_CPP_RLISTV) "$(INTDIR)"\
 "$(INTDIR)\rowlist.pch"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\RowList.rc
DEP_RSC_ROWLIS=\
	".\res\largeico.bmp"\
	".\res\RListDoc.ico"\
	".\res\RowList.ico"\
	".\res\RowList.rc2"\
	".\res\smallico.bmp"\
	".\res\stateico.bmp"\
	".\res\Toolbar.bmp"\
	

"$(INTDIR)\RowList.res" : $(SOURCE) $(DEP_RSC_ROWLIS) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\Listvwex.cpp
DEP_CPP_LISTV=\
	".\ListVwEx.h"\
	".\StdAfx.h"\
	

"$(INTDIR)\Listvwex.obj" : $(SOURCE) $(DEP_CPP_LISTV) "$(INTDIR)"\
 "$(INTDIR)\rowlist.pch"


# End Source File
# End Target
# End Project
################################################################################
