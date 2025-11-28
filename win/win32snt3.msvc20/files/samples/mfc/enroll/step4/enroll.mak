# Microsoft Visual C++ Generated NMAKE File, Format Version 2.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

!IF "$(CFG)" == ""
CFG=Win32 (80x86) Debug
!MESSAGE No configuration specified.  Defaulting to Win32 (80x86) Debug.
!ENDIF 

!IF "$(CFG)" != "Win32 (80x86) Debug" && "$(CFG)" != "Win32 (80x86) Release"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "enroll.mak" CFG="Win32 (80x86) Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Win32 (80x86) Debug" (based on "Win32 (x86) Application")
!MESSAGE "Win32 (80x86) Release" (based on "Win32 (x86) Application")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

################################################################################
# Begin Project
# PROP Target_Last_Scanned "Win32 (80x86) Debug"
MTL=MkTypLib.exe
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Win32 (80x86) Debug"

# PROP BASE Use_MFC 5
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "WinDebug"
# PROP BASE Intermediate_Dir "WinDebug"
# PROP Use_MFC 5
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "WinDebug"
# PROP Intermediate_Dir "WinDebug"
OUTDIR=.\WinDebug
INTDIR=.\WinDebug

ALL : $(OUTDIR)/enroll.exe $(OUTDIR)/enroll.bsc

$(OUTDIR) : 
    if not exist $(OUTDIR)/nul mkdir $(OUTDIR)

# ADD BASE CPP /nologo /MT /W3 /GX /Zi /Od /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /FR /Yu"stdafx.h" /c
# ADD CPP /nologo /MT /W3 /GX /Zi /Od /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /FR /Yu"stdafx.h" /c
CPP_PROJ=/nologo /MT /W3 /GX /Zi /Od /D "_DEBUG" /D "_WINDOWS" /D "_MBCS"\
 /FR$(INTDIR)/ /Fp$(OUTDIR)/"enroll.pch" /Yu"stdafx.h" /Fo$(INTDIR)/\
 /Fd$(OUTDIR)/"enroll.pdb" /c 
CPP_OBJS=.\WinDebug/
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
RSC_PROJ=/l 0x409 /fo$(INTDIR)/"enroll.res" /d "_DEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# SUBTRACT BASE BSC32 /Iu
# ADD BSC32 /nologo
# SUBTRACT BSC32 /Iu
BSC32_FLAGS=/nologo /o$(OUTDIR)/"enroll.bsc" 
BSC32_SBRS= \
	$(INTDIR)/stdafx.sbr \
	$(INTDIR)/enroll.sbr \
	$(INTDIR)/mainfrm.sbr \
	$(INTDIR)/enroldoc.sbr \
	$(INTDIR)/sectform.sbr \
	$(INTDIR)/sectset.sbr \
	$(INTDIR)/coursset.sbr \
	$(INTDIR)/addform.sbr \
	$(INTDIR)/crsform.sbr

$(OUTDIR)/enroll.bsc : $(OUTDIR)  $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib odbc32.lib nafxcwd.lib ctl3d32.lib /NOLOGO /SUBSYSTEM:windows /DEBUG /MACHINE:I386
# SUBTRACT BASE LINK32 /PDB:none
# ADD LINK32 user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib odbc32.lib nafxcwd.lib ctl3d32.lib /NOLOGO /SUBSYSTEM:windows /DEBUG /MACHINE:I386
# SUBTRACT LINK32 /PDB:none
LINK32_FLAGS=user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib\
 shell32.lib odbc32.lib nafxcwd.lib ctl3d32.lib /NOLOGO /SUBSYSTEM:windows\
 /INCREMENTAL:yes /PDB:$(OUTDIR)/"enroll.pdb" /DEBUG /MACHINE:I386\
 /OUT:$(OUTDIR)/"enroll.exe" 
DEF_FILE=
LINK32_OBJS= \
	$(INTDIR)/stdafx.obj \
	$(INTDIR)/enroll.obj \
	$(INTDIR)/mainfrm.obj \
	$(INTDIR)/enroldoc.obj \
	$(INTDIR)/sectform.obj \
	$(INTDIR)/sectset.obj \
	$(INTDIR)/enroll.res \
	$(INTDIR)/coursset.obj \
	$(INTDIR)/addform.obj \
	$(INTDIR)/crsform.obj

$(OUTDIR)/enroll.exe : $(OUTDIR)  $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "Win32 (80x86) Release"

# PROP BASE Use_MFC 5
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "WinRel"
# PROP BASE Intermediate_Dir "WinRel"
# PROP Use_MFC 5
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "WinRel"
# PROP Intermediate_Dir "WinRel"
OUTDIR=.\WinRel
INTDIR=.\WinRel

ALL : $(OUTDIR)/enroll.exe $(OUTDIR)/enroll.bsc

$(OUTDIR) : 
    if not exist $(OUTDIR)/nul mkdir $(OUTDIR)

# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /FR /Yu"stdafx.h" /c
# ADD CPP /nologo /MT /W3 /GX /O2 /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /FR /Yu"stdafx.h" /c
CPP_PROJ=/nologo /MT /W3 /GX /O2 /D "NDEBUG" /D "_WINDOWS" /D "_MBCS"\
 /FR$(INTDIR)/ /Fp$(OUTDIR)/"enroll.pch" /Yu"stdafx.h" /Fo$(INTDIR)/ /c 
CPP_OBJS=.\WinRel/
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
RSC_PROJ=/l 0x409 /fo$(INTDIR)/"enroll.res" /d "NDEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# SUBTRACT BASE BSC32 /Iu
# ADD BSC32 /nologo
# SUBTRACT BSC32 /Iu
BSC32_FLAGS=/nologo /o$(OUTDIR)/"enroll.bsc" 
BSC32_SBRS= \
	$(INTDIR)/stdafx.sbr \
	$(INTDIR)/enroll.sbr \
	$(INTDIR)/mainfrm.sbr \
	$(INTDIR)/enroldoc.sbr \
	$(INTDIR)/sectform.sbr \
	$(INTDIR)/sectset.sbr \
	$(INTDIR)/coursset.sbr \
	$(INTDIR)/addform.sbr \
	$(INTDIR)/crsform.sbr

$(OUTDIR)/enroll.bsc : $(OUTDIR)  $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib odbc32.lib nafxcw.lib ctl3d32.lib /NOLOGO /SUBSYSTEM:windows /MACHINE:I386
# SUBTRACT BASE LINK32 /PDB:none
# ADD LINK32 user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib odbc32.lib nafxcw.lib ctl3d32.lib /NOLOGO /SUBSYSTEM:windows /MACHINE:I386
# SUBTRACT LINK32 /PDB:none
LINK32_FLAGS=user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib\
 shell32.lib odbc32.lib nafxcw.lib ctl3d32.lib /NOLOGO /SUBSYSTEM:windows\
 /INCREMENTAL:no /PDB:$(OUTDIR)/"enroll.pdb" /MACHINE:I386\
 /OUT:$(OUTDIR)/"enroll.exe" 
DEF_FILE=
LINK32_OBJS= \
	$(INTDIR)/stdafx.obj \
	$(INTDIR)/enroll.obj \
	$(INTDIR)/mainfrm.obj \
	$(INTDIR)/enroldoc.obj \
	$(INTDIR)/sectform.obj \
	$(INTDIR)/sectset.obj \
	$(INTDIR)/enroll.res \
	$(INTDIR)/coursset.obj \
	$(INTDIR)/addform.obj \
	$(INTDIR)/crsform.obj

$(OUTDIR)/enroll.exe : $(OUTDIR)  $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 

MTL_PROJ=

.c{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

################################################################################
# Begin Group "Source Files"

################################################################################
# Begin Source File

SOURCE=.\stdafx.cpp
DEP_STDAF=\
	.\stdafx.h

!IF  "$(CFG)" == "Win32 (80x86) Debug"

# ADD BASE CPP /Yc"stdafx.h"
# ADD CPP /Yc"stdafx.h"

$(INTDIR)/stdafx.obj :  $(SOURCE)  $(DEP_STDAF) $(INTDIR)
   $(CPP) /nologo /MT /W3 /GX /Zi /Od /D "_DEBUG" /D "_WINDOWS" /D "_MBCS"\
 /FR$(INTDIR)/ /Fp$(OUTDIR)/"enroll.pch" /Yc"stdafx.h" /Fo$(INTDIR)/\
 /Fd$(OUTDIR)/"enroll.pdb" /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (80x86) Release"

# ADD BASE CPP /Yc"stdafx.h"
# ADD CPP /Yc"stdafx.h"

$(INTDIR)/stdafx.obj :  $(SOURCE)  $(DEP_STDAF) $(INTDIR)
   $(CPP) /nologo /MT /W3 /GX /O2 /D "NDEBUG" /D "_WINDOWS" /D "_MBCS"\
 /FR$(INTDIR)/ /Fp$(OUTDIR)/"enroll.pch" /Yc"stdafx.h" /Fo$(INTDIR)/ /c\
  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\enroll.cpp
DEP_ENROL=\
	.\stdafx.h\
	.\enroll.h\
	.\mainfrm.h\
	.\sectset.h\
	.\coursset.h\
	.\enroldoc.h\
	.\addform.h\
	.\sectform.h

$(INTDIR)/enroll.obj :  $(SOURCE)  $(DEP_ENROL) $(INTDIR) $(INTDIR)/stdafx.obj

# End Source File
################################################################################
# Begin Source File

SOURCE=.\mainfrm.cpp
DEP_MAINF=\
	.\stdafx.h\
	.\enroll.h\
	.\mainfrm.h\
	.\sectset.h\
	.\coursset.h\
	.\addform.h\
	.\crsform.h\
	.\enroldoc.h\
	.\sectform.h

$(INTDIR)/mainfrm.obj :  $(SOURCE)  $(DEP_MAINF) $(INTDIR) $(INTDIR)/stdafx.obj

# End Source File
################################################################################
# Begin Source File

SOURCE=.\enroldoc.cpp
DEP_ENROLD=\
	.\stdafx.h\
	.\enroll.h\
	.\sectset.h\
	.\coursset.h\
	.\enroldoc.h

$(INTDIR)/enroldoc.obj :  $(SOURCE)  $(DEP_ENROLD) $(INTDIR)\
 $(INTDIR)/stdafx.obj

# End Source File
################################################################################
# Begin Source File

SOURCE=.\sectform.cpp
DEP_SECTF=\
	.\stdafx.h\
	.\enroll.h\
	.\sectset.h\
	.\coursset.h\
	.\enroldoc.h\
	.\addform.h\
	.\sectform.h\
	.\mainfrm.h

$(INTDIR)/sectform.obj :  $(SOURCE)  $(DEP_SECTF) $(INTDIR)\
 $(INTDIR)/stdafx.obj

# End Source File
################################################################################
# Begin Source File

SOURCE=.\sectset.cpp
DEP_SECTS=\
	.\stdafx.h\
	.\enroll.h\
	.\sectset.h

$(INTDIR)/sectset.obj :  $(SOURCE)  $(DEP_SECTS) $(INTDIR) $(INTDIR)/stdafx.obj

# End Source File
################################################################################
# Begin Source File

SOURCE=.\enroll.rc
DEP_ENROLL=\
	.\res\enroll.ico\
	.\res\toolbar.bmp\
	.\res\enroll.rc2

$(INTDIR)/enroll.res :  $(SOURCE)  $(DEP_ENROLL) $(INTDIR)
   $(RSC) $(RSC_PROJ)  $(SOURCE) 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\readme.txt
# End Source File
################################################################################
# Begin Source File

SOURCE=.\coursset.cpp
DEP_COURS=\
	.\stdafx.h\
	.\enroll.h\
	.\coursset.h

$(INTDIR)/coursset.obj :  $(SOURCE)  $(DEP_COURS) $(INTDIR)\
 $(INTDIR)/stdafx.obj

# End Source File
################################################################################
# Begin Source File

SOURCE=.\addform.cpp
DEP_ADDFO=\
	.\stdafx.h\
	.\enroll.h\
	.\addform.h

$(INTDIR)/addform.obj :  $(SOURCE)  $(DEP_ADDFO) $(INTDIR) $(INTDIR)/stdafx.obj

# End Source File
################################################################################
# Begin Source File

SOURCE=.\crsform.cpp
DEP_CRSFO=\
	.\stdafx.h\
	.\enroll.h\
	.\coursset.h\
	.\addform.h\
	.\crsform.h\
	.\sectset.h\
	.\enroldoc.h\
	.\mainfrm.h

$(INTDIR)/crsform.obj :  $(SOURCE)  $(DEP_CRSFO) $(INTDIR) $(INTDIR)/stdafx.obj

# End Source File
# End Group
# End Project
################################################################################
