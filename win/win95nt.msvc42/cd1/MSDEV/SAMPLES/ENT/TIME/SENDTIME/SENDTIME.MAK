# Microsoft Developer Studio Generated NMAKE File, Format Version 4.20
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

!IF "$(CFG)" == ""
CFG=SendTime - Win32 Debug
!MESSAGE No configuration specified.  Defaulting to SendTime - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "SendTime - Win32 Release" && "$(CFG)" !=\
 "SendTime - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "SendTime.mak" CFG="SendTime - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "SendTime - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "SendTime - Win32 Debug" (based on "Win32 (x86) Application")
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
# PROP Target_Last_Scanned "SendTime - Win32 Release"
MTL=mktyplib.exe
RSC=rc.exe
CPP=cl.exe

!IF  "$(CFG)" == "SendTime - Win32 Release"

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

ALL : "$(OUTDIR)\SendTime.exe"

CLEAN : 
	-@erase "$(INTDIR)\cl.obj"
	-@erase "$(INTDIR)\SendTDlg.obj"
	-@erase "$(INTDIR)\SendTime.obj"
	-@erase "$(INTDIR)\SendTime.pch"
	-@erase "$(INTDIR)\SendTime.res"
	-@erase "$(INTDIR)\settime_i.obj"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(OUTDIR)\SendTime.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
CPP_PROJ=/nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/SendTime.pch" /Yu"stdafx.h" /Fo"$(INTDIR)/"\
 /c 
CPP_OBJS=.\Release/
CPP_SBRS=.\.
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/SendTime.res" /d "NDEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/SendTime.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 odbc32.lib /nologo /subsystem:windows /machine:I386
LINK32_FLAGS=odbc32.lib /nologo /subsystem:windows /incremental:no\
 /pdb:"$(OUTDIR)/SendTime.pdb" /machine:I386 /out:"$(OUTDIR)/SendTime.exe" 
LINK32_OBJS= \
	"$(INTDIR)\cl.obj" \
	"$(INTDIR)\SendTDlg.obj" \
	"$(INTDIR)\SendTime.obj" \
	"$(INTDIR)\SendTime.res" \
	"$(INTDIR)\settime_i.obj" \
	"$(INTDIR)\StdAfx.obj"

"$(OUTDIR)\SendTime.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "SendTime - Win32 Debug"

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

ALL : "$(OUTDIR)\SendTime.exe"

CLEAN : 
	-@erase "$(INTDIR)\cl.obj"
	-@erase "$(INTDIR)\SendTDlg.obj"
	-@erase "$(INTDIR)\SendTime.obj"
	-@erase "$(INTDIR)\SendTime.pch"
	-@erase "$(INTDIR)\SendTime.res"
	-@erase "$(INTDIR)\settime_i.obj"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\vc40.idb"
	-@erase "$(INTDIR)\vc40.pdb"
	-@erase "$(OUTDIR)\SendTime.exe"
	-@erase "$(OUTDIR)\SendTime.ilk"
	-@erase "$(OUTDIR)\SendTime.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
CPP_PROJ=/nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/SendTime.pch" /Yu"stdafx.h"\
 /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\Debug/
CPP_SBRS=.\.
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/SendTime.res" /d "_DEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/SendTime.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386
# ADD LINK32 odbc32.lib /nologo /subsystem:windows /debug /machine:I386
LINK32_FLAGS=odbc32.lib /nologo /subsystem:windows /incremental:yes\
 /pdb:"$(OUTDIR)/SendTime.pdb" /debug /machine:I386\
 /out:"$(OUTDIR)/SendTime.exe" 
LINK32_OBJS= \
	"$(INTDIR)\cl.obj" \
	"$(INTDIR)\SendTDlg.obj" \
	"$(INTDIR)\SendTime.obj" \
	"$(INTDIR)\SendTime.res" \
	"$(INTDIR)\settime_i.obj" \
	"$(INTDIR)\StdAfx.obj"

"$(OUTDIR)\SendTime.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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

# Name "SendTime - Win32 Release"
# Name "SendTime - Win32 Debug"

!IF  "$(CFG)" == "SendTime - Win32 Release"

!ELSEIF  "$(CFG)" == "SendTime - Win32 Debug"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\SendTime.cpp
DEP_CPP_SENDT=\
	".\cl.h"\
	".\SendTDlg.h"\
	".\SendTime.h"\
	".\StdAfx.h"\
	

"$(INTDIR)\SendTime.obj" : $(SOURCE) $(DEP_CPP_SENDT) "$(INTDIR)"\
 "$(INTDIR)\SendTime.pch"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\StdAfx.cpp
DEP_CPP_STDAF=\
	".\StdAfx.h"\
	

!IF  "$(CFG)" == "SendTime - Win32 Release"

# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/SendTime.pch" /Yc"stdafx.h" /Fo"$(INTDIR)/"\
 /c $(SOURCE) \
	

"$(INTDIR)\StdAfx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\SendTime.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "SendTime - Win32 Debug"

# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/SendTime.pch" /Yc"stdafx.h"\
 /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\StdAfx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\SendTime.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\SendTime.rc
DEP_RSC_SENDTI=\
	".\res\SendTime.ico"\
	".\res\SendTime.rc2"\
	

"$(INTDIR)\SendTime.res" : $(SOURCE) $(DEP_RSC_SENDTI) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\cl.cpp
DEP_CPP_CL_CP=\
	".\cl.h"\
	".\SendTime.h"\
	".\StdAfx.h"\
	

"$(INTDIR)\cl.obj" : $(SOURCE) $(DEP_CPP_CL_CP) "$(INTDIR)"\
 "$(INTDIR)\SendTime.pch"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\settime_i.c

!IF  "$(CFG)" == "SendTime - Win32 Release"

# SUBTRACT CPP /YX /Yc /Yu

"$(INTDIR)\settime_i.obj" : $(SOURCE) "$(INTDIR)"
   $(CPP) /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /Fo"$(INTDIR)/" /c $(SOURCE)


!ELSEIF  "$(CFG)" == "SendTime - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

"$(INTDIR)\settime_i.obj" : $(SOURCE) "$(INTDIR)"
   $(CPP) /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /D "_AFXDLL" /D "_MBCS" /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\SendTDlg.cpp
DEP_CPP_SENDTD=\
	"..\\settime\settime.h"\
	".\cl.h"\
	".\SendTDlg.h"\
	".\SendTime.h"\
	".\StdAfx.h"\
	

"$(INTDIR)\SendTDlg.obj" : $(SOURCE) $(DEP_CPP_SENDTD) "$(INTDIR)"\
 "$(INTDIR)\SendTime.pch"


# End Source File
# End Target
# End Project
################################################################################
