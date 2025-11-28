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
!MESSAGE NMAKE /f "stdreg.mak" CFG="Win32 (80x86) Debug"
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
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "WinDebug"
# PROP Intermediate_Dir "WinDebug"
OUTDIR=.\WinDebug
INTDIR=.\WinDebug

ALL : $(OUTDIR)/stdreg.exe $(OUTDIR)/stdreg.bsc

$(OUTDIR) : 
    if not exist $(OUTDIR)/nul mkdir $(OUTDIR)

# ADD BASE CPP /nologo /MT /W3 /GX /Zi /Od /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /FR /Yu"stdafx.h" /c
# ADD CPP /nologo /MD /W3 /GX /Zi /Od /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /Yu"stdafx.h" /c
CPP_PROJ=/nologo /MD /W3 /GX /Zi /Od /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D\
 "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"stdreg.pch" /Yu"stdafx.h" /Fo$(INTDIR)/\
 /Fd$(OUTDIR)/"stdreg.pdb" /c 
CPP_OBJS=.\WinDebug/
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /fo$(INTDIR)/"stdreg.res" /d "_DEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# SUBTRACT BASE BSC32 /Iu
# ADD BSC32 /nologo
# SUBTRACT BSC32 /Iu
BSC32_FLAGS=/nologo /o$(OUTDIR)/"stdreg.bsc" 
BSC32_SBRS= \
	$(INTDIR)/stdafx.sbr \
	$(INTDIR)/stdreg.sbr \
	$(INTDIR)/dialog.sbr \
	$(INTDIR)/typeinfo.sbr \
	$(INTDIR)/columdlg.sbr \
	$(INTDIR)/coursset.sbr \
	$(INTDIR)/stdset.sbr \
	$(INTDIR)/sectset.sbr \
	$(INTDIR)/dsectset.sbr \
	$(INTDIR)/instrset.sbr \
	$(INTDIR)/enrolset.sbr

$(OUTDIR)/stdreg.bsc : $(OUTDIR)  $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib nafxcwd.lib ctl3d32.lib /NOLOGO /SUBSYSTEM:windows /DEBUG /MACHINE:I386
# ADD LINK32 /NOLOGO /SUBSYSTEM:windows /DEBUG /MACHINE:I386
LINK32_FLAGS=/NOLOGO /SUBSYSTEM:windows /INCREMENTAL:yes\
 /PDB:$(OUTDIR)/"stdreg.pdb" /DEBUG /MACHINE:I386 /OUT:$(OUTDIR)/"stdreg.exe" 
DEF_FILE=
LINK32_OBJS= \
	$(INTDIR)/stdafx.obj \
	$(INTDIR)/stdreg.obj \
	$(INTDIR)/dialog.obj \
	$(INTDIR)/stdreg.res \
	$(INTDIR)/typeinfo.obj \
	$(INTDIR)/columdlg.obj \
	$(INTDIR)/coursset.obj \
	$(INTDIR)/stdset.obj \
	$(INTDIR)/sectset.obj \
	$(INTDIR)/dsectset.obj \
	$(INTDIR)/instrset.obj \
	$(INTDIR)/enrolset.obj

$(OUTDIR)/stdreg.exe : $(OUTDIR)  $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "Win32 (80x86) Release"

# PROP BASE Use_MFC 5
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "WinRel"
# PROP BASE Intermediate_Dir "WinRel"
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "WinRel"
# PROP Intermediate_Dir "WinRel"
OUTDIR=.\WinRel
INTDIR=.\WinRel

ALL : $(OUTDIR)/stdreg.exe $(OUTDIR)/stdreg.bsc

$(OUTDIR) : 
    if not exist $(OUTDIR)/nul mkdir $(OUTDIR)

# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /FR /Yu"stdafx.h" /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /Yu"stdafx.h" /c
CPP_PROJ=/nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D\
 "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"stdreg.pch" /Yu"stdafx.h" /Fo$(INTDIR)/ /c 
CPP_OBJS=.\WinRel/
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /fo$(INTDIR)/"stdreg.res" /d "NDEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# SUBTRACT BASE BSC32 /Iu
# ADD BSC32 /nologo
# SUBTRACT BSC32 /Iu
BSC32_FLAGS=/nologo /o$(OUTDIR)/"stdreg.bsc" 
BSC32_SBRS= \
	$(INTDIR)/stdafx.sbr \
	$(INTDIR)/stdreg.sbr \
	$(INTDIR)/dialog.sbr \
	$(INTDIR)/typeinfo.sbr \
	$(INTDIR)/columdlg.sbr \
	$(INTDIR)/coursset.sbr \
	$(INTDIR)/stdset.sbr \
	$(INTDIR)/sectset.sbr \
	$(INTDIR)/dsectset.sbr \
	$(INTDIR)/instrset.sbr \
	$(INTDIR)/enrolset.sbr

$(OUTDIR)/stdreg.bsc : $(OUTDIR)  $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib nafxcw.lib ctl3d32.lib /NOLOGO /SUBSYSTEM:windows /MACHINE:I386
# ADD LINK32 /NOLOGO /SUBSYSTEM:windows /MACHINE:I386
LINK32_FLAGS=/NOLOGO /SUBSYSTEM:windows /INCREMENTAL:no\
 /PDB:$(OUTDIR)/"stdreg.pdb" /MACHINE:I386 /OUT:$(OUTDIR)/"stdreg.exe" 
DEF_FILE=
LINK32_OBJS= \
	$(INTDIR)/stdafx.obj \
	$(INTDIR)/stdreg.obj \
	$(INTDIR)/dialog.obj \
	$(INTDIR)/stdreg.res \
	$(INTDIR)/typeinfo.obj \
	$(INTDIR)/columdlg.obj \
	$(INTDIR)/coursset.obj \
	$(INTDIR)/stdset.obj \
	$(INTDIR)/sectset.obj \
	$(INTDIR)/dsectset.obj \
	$(INTDIR)/instrset.obj \
	$(INTDIR)/enrolset.obj

$(OUTDIR)/stdreg.exe : $(OUTDIR)  $(DEF_FILE) $(LINK32_OBJS)
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
   $(CPP) /nologo /MD /W3 /GX /Zi /Od /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D\
 "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"stdreg.pch" /Yc"stdafx.h" /Fo$(INTDIR)/\
 /Fd$(OUTDIR)/"stdreg.pdb" /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (80x86) Release"

# ADD BASE CPP /Yc"stdafx.h"
# ADD CPP /Yc"stdafx.h"

$(INTDIR)/stdafx.obj :  $(SOURCE)  $(DEP_STDAF) $(INTDIR)
   $(CPP) /nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D\
 "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"stdreg.pch" /Yc"stdafx.h" /Fo$(INTDIR)/ /c\
  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\stdreg.cpp
DEP_STDRE=\
	.\stdafx.h\
	.\stdreg.h\
	.\typeinfo.h\
	.\dialog.h\
	.\columdlg.h\
	.\coursset.h\
	.\stdset.h\
	.\instrset.h\
	.\sectset.h\
	.\dsectset.h\
	.\enrolset.h\
	.\initdata.h

$(INTDIR)/stdreg.obj :  $(SOURCE)  $(DEP_STDRE) $(INTDIR) $(INTDIR)/stdafx.obj

# End Source File
################################################################################
# Begin Source File

SOURCE=.\dialog.cpp
DEP_DIALO=\
	.\stdafx.h\
	.\stdreg.h\
	.\typeinfo.h\
	.\dialog.h

$(INTDIR)/dialog.obj :  $(SOURCE)  $(DEP_DIALO) $(INTDIR) $(INTDIR)/stdafx.obj

# End Source File
################################################################################
# Begin Source File

SOURCE=.\stdreg.rc
DEP_STDREG=\
	.\res\stdreg.ico\
	.\res\stdreg.rc2

$(INTDIR)/stdreg.res :  $(SOURCE)  $(DEP_STDREG) $(INTDIR)
   $(RSC) $(RSC_PROJ)  $(SOURCE) 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\typeinfo.cpp
DEP_TYPEI=\
	.\stdafx.h\
	.\typeinfo.h

$(INTDIR)/typeinfo.obj :  $(SOURCE)  $(DEP_TYPEI) $(INTDIR)\
 $(INTDIR)/stdafx.obj

# End Source File
################################################################################
# Begin Source File

SOURCE=.\columdlg.cpp
DEP_COLUM=\
	.\stdafx.h\
	.\stdreg.h\
	.\columdlg.h\
	.\typeinfo.h

$(INTDIR)/columdlg.obj :  $(SOURCE)  $(DEP_COLUM) $(INTDIR)\
 $(INTDIR)/stdafx.obj

# End Source File
################################################################################
# Begin Source File

SOURCE=.\coursset.cpp
DEP_COURS=\
	.\stdafx.h\
	.\stdreg.h\
	.\coursset.h

$(INTDIR)/coursset.obj :  $(SOURCE)  $(DEP_COURS) $(INTDIR)\
 $(INTDIR)/stdafx.obj

# End Source File
################################################################################
# Begin Source File

SOURCE=.\stdset.cpp
DEP_STDSE=\
	.\stdafx.h\
	.\stdreg.h\
	.\stdset.h

$(INTDIR)/stdset.obj :  $(SOURCE)  $(DEP_STDSE) $(INTDIR) $(INTDIR)/stdafx.obj

# End Source File
################################################################################
# Begin Source File

SOURCE=.\sectset.cpp
DEP_SECTS=\
	.\stdafx.h\
	.\stdreg.h\
	.\sectset.h

$(INTDIR)/sectset.obj :  $(SOURCE)  $(DEP_SECTS) $(INTDIR) $(INTDIR)/stdafx.obj

# End Source File
################################################################################
# Begin Source File

SOURCE=.\dsectset.cpp
DEP_DSECT=\
	.\stdafx.h\
	.\stdreg.h\
	.\dsectset.h

$(INTDIR)/dsectset.obj :  $(SOURCE)  $(DEP_DSECT) $(INTDIR)\
 $(INTDIR)/stdafx.obj

# End Source File
################################################################################
# Begin Source File

SOURCE=.\instrset.cpp
DEP_INSTR=\
	.\stdafx.h\
	.\stdreg.h\
	.\instrset.h

$(INTDIR)/instrset.obj :  $(SOURCE)  $(DEP_INSTR) $(INTDIR)\
 $(INTDIR)/stdafx.obj

# End Source File
################################################################################
# Begin Source File

SOURCE=.\enrolset.cpp
DEP_ENROL=\
	.\stdafx.h\
	.\stdreg.h\
	.\enrolset.h

$(INTDIR)/enrolset.obj :  $(SOURCE)  $(DEP_ENROL) $(INTDIR)\
 $(INTDIR)/stdafx.obj

# End Source File
# End Group
# End Project
################################################################################
