# Microsoft Visual C++ Generated NMAKE File, Format Version 2.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

!IF "$(CFG)" == ""
CFG=Win32 (80x86) Debug
!MESSAGE No configuration specified.  Defaulting to Win32 (80x86) Debug.
!ENDIF 

!IF "$(CFG)" != "Win32 (80x86) Release" && "$(CFG)" != "Win32 (80x86) Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "dynabind.mak" CFG="Win32 (80x86) Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Win32 (80x86) Release" (based on "Win32 (x86) Application")
!MESSAGE "Win32 (80x86) Debug" (based on "Win32 (x86) Application")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

################################################################################
# Begin Project
# PROP Target_Last_Scanned "Win32 (80x86) Debug"
MTL=MkTypLib.exe
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Win32 (80x86) Release"

# PROP BASE Use_MFC 1
# PROP BASE Use_Debug_Libraries 0
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "WinRel"
# PROP Intermediate_Dir "WinRel"
OUTDIR=.\WinRel
INTDIR=.\WinRel

ALL : $(OUTDIR)/dynabind.exe $(OUTDIR)/dynabind.bsc

$(OUTDIR) : 
    if not exist $(OUTDIR)/nul mkdir $(OUTDIR)

# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
# ADD BASE CPP /nologo /MT /W3 /GX /YX /O1 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /FR /c
# ADD CPP /nologo /MD /W3 /GX /O1 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /Yu"stdafx.h" /c
CPP_PROJ=/nologo /MD /W3 /GX /O1 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"dynabind.pch" /Yu"stdafx.h"\
 /Fo$(INTDIR)/ /c 
CPP_OBJS=.\WinRel/
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /fo$(INTDIR)/"dynabind.res" /d "NDEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# SUBTRACT BASE BSC32 /Iu
# ADD BSC32 /nologo
# SUBTRACT BSC32 /Iu
BSC32_FLAGS=/nologo /o$(OUTDIR)/"dynabind.bsc" 
BSC32_SBRS= \
	$(INTDIR)/stdafx.sbr \
	$(INTDIR)/enroll.sbr \
	$(INTDIR)/mainfrm.sbr \
	$(INTDIR)/enroldoc.sbr \
	$(INTDIR)/sectform.sbr \
	$(INTDIR)/sectset.sbr \
	$(INTDIR)/coursset.sbr \
	$(INTDIR)/columnst.sbr \
	$(INTDIR)/addfield.sbr

$(OUTDIR)/dynabind.bsc : $(OUTDIR)  $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 oldnames.lib /NOLOGO /STACK:0x10240 /SUBSYSTEM:windows /MACHINE:IX86
# ADD LINK32 /NOLOGO /STACK:0x10240 /SUBSYSTEM:windows /MACHINE:IX86
LINK32_FLAGS=/NOLOGO /STACK:0x10240 /SUBSYSTEM:windows /INCREMENTAL:no\
 /PDB:$(OUTDIR)/"dynabind.pdb" /MACHINE:IX86 /OUT:$(OUTDIR)/"dynabind.exe" 
DEF_FILE=
LINK32_OBJS= \
	$(INTDIR)/dynabind.res \
	$(INTDIR)/stdafx.obj \
	$(INTDIR)/enroll.obj \
	$(INTDIR)/mainfrm.obj \
	$(INTDIR)/enroldoc.obj \
	$(INTDIR)/sectform.obj \
	$(INTDIR)/sectset.obj \
	$(INTDIR)/coursset.obj \
	$(INTDIR)/columnst.obj \
	$(INTDIR)/addfield.obj

$(OUTDIR)/dynabind.exe : $(OUTDIR)  $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

# PROP BASE Use_MFC 1
# PROP BASE Use_Debug_Libraries 1
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "WinDebug"
# PROP Intermediate_Dir "WinDebug"
OUTDIR=.\WinDebug
INTDIR=.\WinDebug

ALL : $(OUTDIR)/dynabind.exe $(OUTDIR)/dynabind.bsc

$(OUTDIR) : 
    if not exist $(OUTDIR)/nul mkdir $(OUTDIR)

# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
# ADD BASE CPP /nologo /MT /W3 /GX /Zi /YX /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /FR /c
# ADD CPP /nologo /MD /W3 /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /Yu"stdafx.h" /c
CPP_PROJ=/nologo /MD /W3 /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"dynabind.pch" /Yu"stdafx.h"\
 /Fo$(INTDIR)/ /Fd$(OUTDIR)/"dynabind.pdb" /c 
CPP_OBJS=.\WinDebug/
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /fo$(INTDIR)/"dynabind.res" /d "_DEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# SUBTRACT BASE BSC32 /Iu
# ADD BSC32 /nologo
# SUBTRACT BSC32 /Iu
BSC32_FLAGS=/nologo /o$(OUTDIR)/"dynabind.bsc" 
BSC32_SBRS= \
	$(INTDIR)/stdafx.sbr \
	$(INTDIR)/enroll.sbr \
	$(INTDIR)/mainfrm.sbr \
	$(INTDIR)/enroldoc.sbr \
	$(INTDIR)/sectform.sbr \
	$(INTDIR)/sectset.sbr \
	$(INTDIR)/coursset.sbr \
	$(INTDIR)/columnst.sbr \
	$(INTDIR)/addfield.sbr

$(OUTDIR)/dynabind.bsc : $(OUTDIR)  $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 oldnames.lib /NOLOGO /STACK:0x10240 /SUBSYSTEM:windows /DEBUG /MACHINE:IX86
# ADD LINK32 /NOLOGO /STACK:0x10240 /SUBSYSTEM:windows /DEBUG /MACHINE:IX86
LINK32_FLAGS=/NOLOGO /STACK:0x10240 /SUBSYSTEM:windows /INCREMENTAL:yes\
 /PDB:$(OUTDIR)/"dynabind.pdb" /DEBUG /MACHINE:IX86\
 /OUT:$(OUTDIR)/"dynabind.exe" 
DEF_FILE=
LINK32_OBJS= \
	$(INTDIR)/dynabind.res \
	$(INTDIR)/stdafx.obj \
	$(INTDIR)/enroll.obj \
	$(INTDIR)/mainfrm.obj \
	$(INTDIR)/enroldoc.obj \
	$(INTDIR)/sectform.obj \
	$(INTDIR)/sectset.obj \
	$(INTDIR)/coursset.obj \
	$(INTDIR)/columnst.obj \
	$(INTDIR)/addfield.obj

$(OUTDIR)/dynabind.exe : $(OUTDIR)  $(DEF_FILE) $(LINK32_OBJS)
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

################################################################################
# Begin Group "Source Files"

################################################################################
# Begin Source File

SOURCE=.\dynabind.rc
DEP_DYNAB=\
	.\res\enroll.ico\
	.\res\idr_main.bmp\
	.\res\enroll.rc2

$(INTDIR)/dynabind.res :  $(SOURCE)  $(DEP_DYNAB) $(INTDIR)
   $(RSC) $(RSC_PROJ)  $(SOURCE) 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\stdafx.cpp
DEP_STDAF=\
	.\stdafx.h

!IF  "$(CFG)" == "Win32 (80x86) Release"

# ADD BASE CPP /Yc"STDAFX.H"
# ADD CPP /Yc"stdafx.h"

$(INTDIR)/stdafx.obj :  $(SOURCE)  $(DEP_STDAF) $(INTDIR)
   $(CPP) /nologo /MD /W3 /GX /O1 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"dynabind.pch" /Yc"stdafx.h"\
 /Fo$(INTDIR)/ /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

# ADD BASE CPP /Yc"STDAFX.H"
# ADD CPP /Yc"stdafx.h"

$(INTDIR)/stdafx.obj :  $(SOURCE)  $(DEP_STDAF) $(INTDIR)
   $(CPP) /nologo /MD /W3 /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"dynabind.pch" /Yc"stdafx.h"\
 /Fo$(INTDIR)/ /Fd$(OUTDIR)/"dynabind.pdb" /c  $(SOURCE) 

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
	.\sectform.h

!IF  "$(CFG)" == "Win32 (80x86) Release"

# ADD BASE CPP /Yu"STDAFX.H"
# ADD CPP /Yu"stdafx.h"

$(INTDIR)/enroll.obj :  $(SOURCE)  $(DEP_ENROL) $(INTDIR) $(INTDIR)/stdafx.obj
   $(CPP) /nologo /MD /W3 /GX /O1 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"dynabind.pch" /Yu"stdafx.h"\
 /Fo$(INTDIR)/ /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

# ADD BASE CPP /Yu"STDAFX.H"
# ADD CPP /Yu"stdafx.h"

$(INTDIR)/enroll.obj :  $(SOURCE)  $(DEP_ENROL) $(INTDIR) $(INTDIR)/stdafx.obj
   $(CPP) /nologo /MD /W3 /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"dynabind.pch" /Yu"stdafx.h"\
 /Fo$(INTDIR)/ /Fd$(OUTDIR)/"dynabind.pdb" /c  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\mainfrm.cpp
DEP_MAINF=\
	.\stdafx.h\
	.\enroll.h\
	.\mainfrm.h

!IF  "$(CFG)" == "Win32 (80x86) Release"

# ADD BASE CPP /Yu"STDAFX.H"
# ADD CPP /Yu"stdafx.h"

$(INTDIR)/mainfrm.obj :  $(SOURCE)  $(DEP_MAINF) $(INTDIR) $(INTDIR)/stdafx.obj
   $(CPP) /nologo /MD /W3 /GX /O1 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"dynabind.pch" /Yu"stdafx.h"\
 /Fo$(INTDIR)/ /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

# ADD BASE CPP /Yu"STDAFX.H"
# ADD CPP /Yu"stdafx.h"

$(INTDIR)/mainfrm.obj :  $(SOURCE)  $(DEP_MAINF) $(INTDIR) $(INTDIR)/stdafx.obj
   $(CPP) /nologo /MD /W3 /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"dynabind.pch" /Yu"stdafx.h"\
 /Fo$(INTDIR)/ /Fd$(OUTDIR)/"dynabind.pdb" /c  $(SOURCE) 

!ENDIF 

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

!IF  "$(CFG)" == "Win32 (80x86) Release"

# ADD BASE CPP /Yu"STDAFX.H"
# ADD CPP /Yu"stdafx.h"

$(INTDIR)/enroldoc.obj :  $(SOURCE)  $(DEP_ENROLD) $(INTDIR)\
 $(INTDIR)/stdafx.obj
   $(CPP) /nologo /MD /W3 /GX /O1 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"dynabind.pch" /Yu"stdafx.h"\
 /Fo$(INTDIR)/ /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

# ADD BASE CPP /Yu"STDAFX.H"
# ADD CPP /Yu"stdafx.h"

$(INTDIR)/enroldoc.obj :  $(SOURCE)  $(DEP_ENROLD) $(INTDIR)\
 $(INTDIR)/stdafx.obj
   $(CPP) /nologo /MD /W3 /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"dynabind.pch" /Yu"stdafx.h"\
 /Fo$(INTDIR)/ /Fd$(OUTDIR)/"dynabind.pdb" /c  $(SOURCE) 

!ENDIF 

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
	.\sectform.h\
	.\addfield.h

!IF  "$(CFG)" == "Win32 (80x86) Release"

# ADD BASE CPP /Yu"STDAFX.H"
# ADD CPP /Yu"stdafx.h"

$(INTDIR)/sectform.obj :  $(SOURCE)  $(DEP_SECTF) $(INTDIR)\
 $(INTDIR)/stdafx.obj
   $(CPP) /nologo /MD /W3 /GX /O1 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"dynabind.pch" /Yu"stdafx.h"\
 /Fo$(INTDIR)/ /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

# ADD BASE CPP /Yu"STDAFX.H"
# ADD CPP /Yu"stdafx.h"

$(INTDIR)/sectform.obj :  $(SOURCE)  $(DEP_SECTF) $(INTDIR)\
 $(INTDIR)/stdafx.obj
   $(CPP) /nologo /MD /W3 /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"dynabind.pch" /Yu"stdafx.h"\
 /Fo$(INTDIR)/ /Fd$(OUTDIR)/"dynabind.pdb" /c  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\sectset.cpp
DEP_SECTS=\
	.\stdafx.h\
	.\enroll.h\
	.\sectset.h

!IF  "$(CFG)" == "Win32 (80x86) Release"

# ADD BASE CPP /Yu"STDAFX.H"
# ADD CPP /Yu"stdafx.h"

$(INTDIR)/sectset.obj :  $(SOURCE)  $(DEP_SECTS) $(INTDIR) $(INTDIR)/stdafx.obj
   $(CPP) /nologo /MD /W3 /GX /O1 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"dynabind.pch" /Yu"stdafx.h"\
 /Fo$(INTDIR)/ /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

# ADD BASE CPP /Yu"STDAFX.H"
# ADD CPP /Yu"stdafx.h"

$(INTDIR)/sectset.obj :  $(SOURCE)  $(DEP_SECTS) $(INTDIR) $(INTDIR)/stdafx.obj
   $(CPP) /nologo /MD /W3 /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"dynabind.pch" /Yu"stdafx.h"\
 /Fo$(INTDIR)/ /Fd$(OUTDIR)/"dynabind.pdb" /c  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\coursset.cpp
DEP_COURS=\
	.\stdafx.h\
	.\enroll.h\
	.\coursset.h

!IF  "$(CFG)" == "Win32 (80x86) Release"

# ADD BASE CPP /Yu"STDAFX.H"
# ADD CPP /Yu"stdafx.h"

$(INTDIR)/coursset.obj :  $(SOURCE)  $(DEP_COURS) $(INTDIR)\
 $(INTDIR)/stdafx.obj
   $(CPP) /nologo /MD /W3 /GX /O1 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"dynabind.pch" /Yu"stdafx.h"\
 /Fo$(INTDIR)/ /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

# ADD BASE CPP /Yu"STDAFX.H"
# ADD CPP /Yu"stdafx.h"

$(INTDIR)/coursset.obj :  $(SOURCE)  $(DEP_COURS) $(INTDIR)\
 $(INTDIR)/stdafx.obj
   $(CPP) /nologo /MD /W3 /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"dynabind.pch" /Yu"stdafx.h"\
 /Fo$(INTDIR)/ /Fd$(OUTDIR)/"dynabind.pdb" /c  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\columnst.cpp
DEP_COLUM=\
	.\stdafx.h\
	.\columnst.h

!IF  "$(CFG)" == "Win32 (80x86) Release"

# ADD BASE CPP /Yu"STDAFX.H"
# ADD CPP /Yu"stdafx.h"

$(INTDIR)/columnst.obj :  $(SOURCE)  $(DEP_COLUM) $(INTDIR)\
 $(INTDIR)/stdafx.obj
   $(CPP) /nologo /MD /W3 /GX /O1 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"dynabind.pch" /Yu"stdafx.h"\
 /Fo$(INTDIR)/ /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

# ADD BASE CPP /Yu"STDAFX.H"
# ADD CPP /Yu"stdafx.h"

$(INTDIR)/columnst.obj :  $(SOURCE)  $(DEP_COLUM) $(INTDIR)\
 $(INTDIR)/stdafx.obj
   $(CPP) /nologo /MD /W3 /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"dynabind.pch" /Yu"stdafx.h"\
 /Fo$(INTDIR)/ /Fd$(OUTDIR)/"dynabind.pdb" /c  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\addfield.cpp
DEP_ADDFI=\
	.\stdafx.h\
	.\enroll.h\
	.\sectset.h\
	.\addfield.h\
	.\columnst.h

!IF  "$(CFG)" == "Win32 (80x86) Release"

# ADD BASE CPP /Yu"STDAFX.H"
# ADD CPP /Yu"stdafx.h"

$(INTDIR)/addfield.obj :  $(SOURCE)  $(DEP_ADDFI) $(INTDIR)\
 $(INTDIR)/stdafx.obj
   $(CPP) /nologo /MD /W3 /GX /O1 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"dynabind.pch" /Yu"stdafx.h"\
 /Fo$(INTDIR)/ /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

# ADD BASE CPP /Yu"STDAFX.H"
# ADD CPP /Yu"stdafx.h"

$(INTDIR)/addfield.obj :  $(SOURCE)  $(DEP_ADDFI) $(INTDIR)\
 $(INTDIR)/stdafx.obj
   $(CPP) /nologo /MD /W3 /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"dynabind.pch" /Yu"stdafx.h"\
 /Fo$(INTDIR)/ /Fd$(OUTDIR)/"dynabind.pdb" /c  $(SOURCE) 

!ENDIF 

# End Source File
# End Group
# End Project
################################################################################
