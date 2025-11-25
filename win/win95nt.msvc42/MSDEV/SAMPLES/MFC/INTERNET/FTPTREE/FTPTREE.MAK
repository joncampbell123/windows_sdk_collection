# Microsoft Developer Studio Generated NMAKE File, Format Version 4.20
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

!IF "$(CFG)" == ""
CFG=FTPTREE - Win32 Debug
!MESSAGE No configuration specified.  Defaulting to FTPTREE - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "FTPTREE - Win32 Release" && "$(CFG)" !=\
 "FTPTREE - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "Ftptree.mak" CFG="FTPTREE - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "FTPTREE - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "FTPTREE - Win32 Debug" (based on "Win32 (x86) Application")
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
# PROP Target_Last_Scanned "FTPTREE - Win32 Debug"
MTL=mktyplib.exe
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "FTPTREE - Win32 Release"

# PROP BASE Use_MFC 5
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 5
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
OUTDIR=.\Release
INTDIR=.\Release

ALL : "$(OUTDIR)\Ftptree.exe"

CLEAN : 
	-@erase "$(INTDIR)\FtpTrCtl.obj"
	-@erase "$(INTDIR)\FTPtrDlg.obj"
	-@erase "$(INTDIR)\FTPTREE.obj"
	-@erase "$(INTDIR)\Ftptree.pch"
	-@erase "$(INTDIR)\FTPTREE.res"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(OUTDIR)\Ftptree.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Yu"stdafx.h" /c
# ADD CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Yu"stdafx.h" /c
CPP_PROJ=/nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_MBCS" /Fp"$(INTDIR)/Ftptree.pch" /Yu"stdafx.h" /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\Release/
CPP_SBRS=.\.
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/FTPTREE.res" /d "NDEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/Ftptree.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 uuid.lib /nologo /subsystem:windows /machine:I386
LINK32_FLAGS=uuid.lib /nologo /subsystem:windows /incremental:no\
 /pdb:"$(OUTDIR)/Ftptree.pdb" /machine:I386 /out:"$(OUTDIR)/Ftptree.exe" 
LINK32_OBJS= \
	"$(INTDIR)\FtpTrCtl.obj" \
	"$(INTDIR)\FTPtrDlg.obj" \
	"$(INTDIR)\FTPTREE.obj" \
	"$(INTDIR)\FTPTREE.res" \
	"$(INTDIR)\StdAfx.obj"

"$(OUTDIR)\Ftptree.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "FTPTREE - Win32 Debug"

# PROP BASE Use_MFC 5
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 5
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
OUTDIR=.\Debug
INTDIR=.\Debug

ALL : "$(OUTDIR)\Ftptree.exe"

CLEAN : 
	-@erase "$(INTDIR)\FtpTrCtl.obj"
	-@erase "$(INTDIR)\FTPtrDlg.obj"
	-@erase "$(INTDIR)\FTPTREE.obj"
	-@erase "$(INTDIR)\Ftptree.pch"
	-@erase "$(INTDIR)\FTPTREE.res"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\vc40.idb"
	-@erase "$(INTDIR)\vc40.pdb"
	-@erase "$(OUTDIR)\Ftptree.exe"
	-@erase "$(OUTDIR)\Ftptree.ilk"
	-@erase "$(OUTDIR)\Ftptree.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /Yu"stdafx.h" /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /Yu"stdafx.h" /c
CPP_PROJ=/nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /D "_MBCS" /Fp"$(INTDIR)/Ftptree.pch" /Yu"stdafx.h" /Fo"$(INTDIR)/"\
 /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\Debug/
CPP_SBRS=.\.
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/FTPTREE.res" /d "_DEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/Ftptree.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386
# ADD LINK32 uuid.lib /nologo /subsystem:windows /debug /machine:I386
LINK32_FLAGS=uuid.lib /nologo /subsystem:windows /incremental:yes\
 /pdb:"$(OUTDIR)/Ftptree.pdb" /debug /machine:I386 /out:"$(OUTDIR)/Ftptree.exe" 
LINK32_OBJS= \
	"$(INTDIR)\FtpTrCtl.obj" \
	"$(INTDIR)\FTPtrDlg.obj" \
	"$(INTDIR)\FTPTREE.obj" \
	"$(INTDIR)\FTPTREE.res" \
	"$(INTDIR)\StdAfx.obj"

"$(OUTDIR)\Ftptree.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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

# Name "FTPTREE - Win32 Release"
# Name "FTPTREE - Win32 Debug"

!IF  "$(CFG)" == "FTPTREE - Win32 Release"

!ELSEIF  "$(CFG)" == "FTPTREE - Win32 Debug"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\FTPTREE.cpp
DEP_CPP_FTPTR=\
	".\FTPtrCtl.h"\
	".\FTPtrDlg.h"\
	".\FTPTREE.h"\
	".\StdAfx.h"\
	

"$(INTDIR)\FTPTREE.obj" : $(SOURCE) $(DEP_CPP_FTPTR) "$(INTDIR)"\
 "$(INTDIR)\Ftptree.pch"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\StdAfx.cpp
DEP_CPP_STDAF=\
	".\StdAfx.h"\
	

!IF  "$(CFG)" == "FTPTREE - Win32 Release"

# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS"\
 /Fp"$(INTDIR)/Ftptree.pch" /Yc"stdafx.h" /Fo"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\StdAfx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\Ftptree.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "FTPTREE - Win32 Debug"

# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /D "_MBCS" /Fp"$(INTDIR)/Ftptree.pch" /Yc"stdafx.h" /Fo"$(INTDIR)/"\
 /Fd"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\StdAfx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\Ftptree.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\FTPTREE.rc
DEP_RSC_FTPTRE=\
	".\res\bmdir.bmp"\
	".\res\bmerror.bmp"\
	".\res\bmfile.bmp"\
	".\res\FTPTREE.ico"\
	".\res\FTPTREE.rc2"\
	

"$(INTDIR)\FTPTREE.res" : $(SOURCE) $(DEP_RSC_FTPTRE) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\FTPtrDlg.cpp
DEP_CPP_FTPTRD=\
	".\FTPtrCtl.h"\
	".\FTPtrDlg.h"\
	".\FTPTREE.h"\
	".\StdAfx.h"\
	

"$(INTDIR)\FTPtrDlg.obj" : $(SOURCE) $(DEP_CPP_FTPTRD) "$(INTDIR)"\
 "$(INTDIR)\Ftptree.pch"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\FtpTrCtl.cpp
DEP_CPP_FTPTRC=\
	".\FTPtrCtl.h"\
	".\FTPTREE.h"\
	".\StdAfx.h"\
	

"$(INTDIR)\FtpTrCtl.obj" : $(SOURCE) $(DEP_CPP_FTPTRC) "$(INTDIR)"\
 "$(INTDIR)\Ftptree.pch"


# End Source File
# End Target
# End Project
################################################################################
