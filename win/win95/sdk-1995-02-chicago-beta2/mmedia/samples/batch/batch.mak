# Microsoft Visual C++ Generated NMAKE File, Format Version 2.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

!IF "$(CFG)" == ""
CFG=Win32 Debug
!MESSAGE No configuration specified.  Defaulting to Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "Win32 Debug" && "$(CFG)" != "Win32 Release"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "batch.mak" CFG="Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE "Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

################################################################################
# Begin Project
# PROP Target_Last_Scanned "Win32 Debug"
MTL=MkTypLib.exe
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Win32 Debug"

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

ALL : $(OUTDIR)/batch.exe $(OUTDIR)/batch.bsc

$(OUTDIR) : 
    if not exist $(OUTDIR)/nul mkdir $(OUTDIR)

# ADD BASE CPP /nologo /MT /W3 /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /FR /Yu"stdafx.h" /c
# ADD CPP /nologo /MT /W3 /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /Yu"stdafx.h" /c
# SUBTRACT CPP /Fr
CPP_PROJ=/nologo /MT /W3 /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D\
 "_MBCS" /Fp$(OUTDIR)/"batch.pch" /Yu"stdafx.h" /Fo$(INTDIR)/\
 /Fd$(OUTDIR)/"batch.pdb" /c 
CPP_OBJS=.\WinDebug/
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
RSC_PROJ=/l 0x409 /fo$(INTDIR)/"batch.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_SBRS= \
	
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o$(OUTDIR)/"batch.bsc" 

$(OUTDIR)/batch.bsc : $(OUTDIR)  $(BSC32_SBRS)
LINK32=link.exe
# ADD BASE LINK32 /NOLOGO /SUBSYSTEM:windows /DEBUG /MACHINE:I386
# SUBTRACT BASE LINK32 /PDB:none
# ADD LINK32 vfw32.lib msacm32.lib /NOLOGO /SUBSYSTEM:windows /DEBUG /MACHINE:I386
# SUBTRACT LINK32 /PDB:none
LINK32_FLAGS=vfw32.lib msacm32.lib /NOLOGO /SUBSYSTEM:windows /INCREMENTAL:yes\
 /PDB:$(OUTDIR)/"batch.pdb" /DEBUG /MACHINE:I386 /OUT:$(OUTDIR)/"batch.exe" 
DEF_FILE=
LINK32_OBJS= \
	$(INTDIR)/stdafx.obj \
	$(INTDIR)/batch.obj \
	$(INTDIR)/mainfrm.obj \
	$(INTDIR)/batchdoc.obj \
	$(INTDIR)/batchvw.obj \
	$(INTDIR)/batch.res \
	$(INTDIR)/compress.obj \
	$(INTDIR)/destdird.obj \
	$(INTDIR)/interlea.obj

$(OUTDIR)/batch.exe : $(OUTDIR)  $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "Win32 Release"

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

ALL : $(OUTDIR)/batch.exe $(OUTDIR)/batch.bsc

$(OUTDIR) : 
    if not exist $(OUTDIR)/nul mkdir $(OUTDIR)

# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /FR /Yu"stdafx.h" /c
# ADD CPP /nologo /MT /W3 /GX /Zi /Od /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /Yu"stdafx.h" /c
# SUBTRACT CPP /Fr
CPP_PROJ=/nologo /MT /W3 /GX /Zi /Od /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D\
 "_MBCS" /Fp$(OUTDIR)/"batch.pch" /Yu"stdafx.h" /Fo$(INTDIR)/\
 /Fd$(OUTDIR)/"batch.pdb" /c 
CPP_OBJS=.\WinRel/
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
RSC_PROJ=/l 0x409 /fo$(INTDIR)/"batch.res" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_SBRS= \
	
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o$(OUTDIR)/"batch.bsc" 

$(OUTDIR)/batch.bsc : $(OUTDIR)  $(BSC32_SBRS)
LINK32=link.exe
# ADD BASE LINK32 /NOLOGO /SUBSYSTEM:windows /MACHINE:I386
# SUBTRACT BASE LINK32 /PDB:none
# ADD LINK32 vfw32.lib msacm32.lib /NOLOGO /SUBSYSTEM:windows /MACHINE:I386
# SUBTRACT LINK32 /PDB:none
LINK32_FLAGS=vfw32.lib msacm32.lib /NOLOGO /SUBSYSTEM:windows /INCREMENTAL:no\
 /PDB:$(OUTDIR)/"batch.pdb" /MACHINE:I386 /OUT:$(OUTDIR)/"batch.exe" 
DEF_FILE=
LINK32_OBJS= \
	$(INTDIR)/stdafx.obj \
	$(INTDIR)/batch.obj \
	$(INTDIR)/mainfrm.obj \
	$(INTDIR)/batchdoc.obj \
	$(INTDIR)/batchvw.obj \
	$(INTDIR)/batch.res \
	$(INTDIR)/compress.obj \
	$(INTDIR)/destdird.obj \
	$(INTDIR)/interlea.obj

$(OUTDIR)/batch.exe : $(OUTDIR)  $(DEF_FILE) $(LINK32_OBJS)
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

!IF  "$(CFG)" == "Win32 Debug"

# ADD BASE CPP /Yc"stdafx.h"
# ADD CPP /Yc"stdafx.h"

$(INTDIR)/stdafx.obj :  $(SOURCE)  $(DEP_STDAF) $(INTDIR)
   $(CPP) /nologo /MT /W3 /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D\
 "_MBCS" /Fp$(OUTDIR)/"batch.pch" /Yc"stdafx.h" /Fo$(INTDIR)/\
 /Fd$(OUTDIR)/"batch.pdb" /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 Release"

# ADD BASE CPP /Yc"stdafx.h"
# ADD CPP /Yc"stdafx.h"

$(INTDIR)/stdafx.obj :  $(SOURCE)  $(DEP_STDAF) $(INTDIR)
   $(CPP) /nologo /MT /W3 /GX /Zi /Od /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D\
 "_MBCS" /Fp$(OUTDIR)/"batch.pch" /Yc"stdafx.h" /Fo$(INTDIR)/\
 /Fd$(OUTDIR)/"batch.pdb" /c  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\batch.cpp
DEP_BATCH=\
	.\mainfrm.h\
	.\batchdoc.h\
	.\batchvw.h\
	.\batch.h

$(INTDIR)/batch.obj :  $(SOURCE)  $(DEP_BATCH) $(INTDIR) $(INTDIR)/stdafx.obj

# End Source File
################################################################################
# Begin Source File

SOURCE=.\mainfrm.cpp
DEP_MAINF=\
	.\batch.h\
	.\mainfrm.h

$(INTDIR)/mainfrm.obj :  $(SOURCE)  $(DEP_MAINF) $(INTDIR) $(INTDIR)/stdafx.obj

# End Source File
################################################################################
# Begin Source File

SOURCE=.\batchdoc.cpp
DEP_BATCHD=\
	.\batch.h\
	.\batchdoc.h

$(INTDIR)/batchdoc.obj :  $(SOURCE)  $(DEP_BATCHD) $(INTDIR)\
 $(INTDIR)/stdafx.obj

# End Source File
################################################################################
# Begin Source File

SOURCE=.\batchvw.cpp
DEP_BATCHV=\
	.\batch.h\
	.\batchdoc.h\
	.\batchvw.h

$(INTDIR)/batchvw.obj :  $(SOURCE)  $(DEP_BATCHV) $(INTDIR)\
 $(INTDIR)/stdafx.obj

# End Source File
################################################################################
# Begin Source File

SOURCE=.\batch.rc
DEP_BATCH_=\
	.\BATCH.ICO\
	.\BATCHDOC.ICO\
	.\TOOLBAR.BMP\
	.\BATCH.RC2

$(INTDIR)/batch.res :  $(SOURCE)  $(DEP_BATCH_) $(INTDIR)
   $(RSC) $(RSC_PROJ)  $(SOURCE) 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\readme.txt
# End Source File
################################################################################
# Begin Source File

SOURCE=.\compress.cpp
DEP_COMPR=\
	.\stdafx.h\
	.\batch.h\
	.\compress.h\
	.\resource.h

$(INTDIR)/compress.obj :  $(SOURCE)  $(DEP_COMPR) $(INTDIR)\
 $(INTDIR)/stdafx.obj

# End Source File
################################################################################
# Begin Source File

SOURCE=.\destdird.cpp
DEP_DESTD=\
	.\stdafx.h\
	.\batch.h\
	.\destdird.h\
	.\resource.h

$(INTDIR)/destdird.obj :  $(SOURCE)  $(DEP_DESTD) $(INTDIR)\
 $(INTDIR)/stdafx.obj

# End Source File
################################################################################
# Begin Source File

SOURCE=.\interlea.cpp
DEP_INTER=\
	.\stdafx.h\
	.\batch.h\
	.\interlea.h\
	.\resource.h

$(INTDIR)/interlea.obj :  $(SOURCE)  $(DEP_INTER) $(INTDIR)\
 $(INTDIR)/stdafx.obj

# End Source File
# End Group
# End Project
################################################################################
