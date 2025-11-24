# Microsoft Visual C++ Generated NMAKE File, Format Version 2.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

!IF "$(CFG)" == ""
CFG=Win32Debug
!MESSAGE No configuration specified.  Defaulting to Win32Debug.
!ENDIF 

!IF "$(CFG)" != "Win32Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "ICMTEST.MAK" CFG="Win32Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Win32Debug" (based on "Win32 (x86) Application")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

################################################################################
# Begin Project
# PROP Target_Last_Scanned "Win32Debug"
OUTDIR=.\Debug
INTDIR=.\Debug

ALL : $(OUTDIR)/ICMTEST.exe $(OUTDIR)/ICMTEST.map $(OUTDIR)/ICMTEST.bsc

$(OUTDIR) : 
    if not exist $(OUTDIR)/nul mkdir $(OUTDIR)

MTL=MkTypLib.exe
CPP=cl.exe
RSC=rc.exe
BSC32=bscmake.exe
BSC32_SBRS= \
	$(INTDIR)/ICMTEST.SBR \
	$(INTDIR)/PRINT.SBR \
	$(INTDIR)/DIB.SBR \
	$(INTDIR)/DIALOGS.SBR
LINK32=link.exe
DEF_FILE=.\ICMTEST.DEF
LINK32_OBJS= \
	$(INTDIR)/ICMTEST.OBJ \
	$(INTDIR)/PRINT.OBJ \
	$(INTDIR)/DIB.OBJ \
	$(INTDIR)/DIALOGS.OBJ \
	$(INTDIR)/ICMTEST.res
# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
# ADD BASE CPP /nologo /W3 /GX /Zi /YX /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /FR /c
# ADD CPP /nologo /W3 /GX /Zi /YX /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /FR /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o$(OUTDIR)/"ICMTEST.bsc" 
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /NOLOGO /SUBSYSTEM:windows /DEBUG /MACHINE:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /NOLOGO /INCREMENTAL:no /MAP /DEBUG /MACHINE:I386 /SUBSYSTEM:windows,4.00
# SUBTRACT LINK32 /PDB:none
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /NOLOGO\
 /INCREMENTAL:no /PDB:$(OUTDIR)/"ICMTEST.pdb" /MAP:$(INTDIR)/"ICMTEST.map"\
 /DEBUG /MACHINE:I386 /DEF:".\ICMTEST.DEF" /OUT:$(OUTDIR)/"ICMTEST.exe"\
 /SUBSYSTEM:windows,4.00 
MTL_PROJ=/nologo /D "_DEBUG" /win32 
CPP_PROJ=/nologo /W3 /GX /Zi /YX /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /FR$(INTDIR)/ /Fp$(OUTDIR)/"ICMTEST.pch" /Fo$(INTDIR)/\
 /Fd$(OUTDIR)/"ICMTEST.pdb" /c 
CPP_OBJS=.\Debug/

.c{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

RSC_PROJ=/l 0x409 /fo$(INTDIR)/"ICMTEST.res" /d "_DEBUG" 

$(OUTDIR)/ICMTEST.bsc : $(OUTDIR)  $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

$(OUTDIR)/ICMTEST.exe : $(OUTDIR)  $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

################################################################################
# Begin Group "Source Files"

################################################################################
# Begin Source File

SOURCE=.\ICMTEST.C
DEP_ICMTE=\
	.\ICMTEST.H\
	.\DIALOGS.H

$(INTDIR)/ICMTEST.OBJ :  $(SOURCE)  $(DEP_ICMTE) $(INTDIR)

# End Source File
################################################################################
# Begin Source File

SOURCE=.\PRINT.C
DEP_PRINT=\
	.\PRINT.H\
	.\ICMTEST.H

$(INTDIR)/PRINT.OBJ :  $(SOURCE)  $(DEP_PRINT) $(INTDIR)

# End Source File
################################################################################
# Begin Source File

SOURCE=.\DIB.C
DEP_DIB_C=\
	.\DIB.H\
	.\ICMTEST.H

$(INTDIR)/DIB.OBJ :  $(SOURCE)  $(DEP_DIB_C) $(INTDIR)

# End Source File
################################################################################
# Begin Source File

SOURCE=.\ICMTEST.DEF
# End Source File
################################################################################
# Begin Source File

SOURCE=.\DIALOGS.C
DEP_DIALO=\
	.\ICMTEST.H\
	.\DIALOGS.H\
	.\PRINT.H

$(INTDIR)/DIALOGS.OBJ :  $(SOURCE)  $(DEP_DIALO) $(INTDIR)

# End Source File
################################################################################
# Begin Source File

SOURCE=.\ICMTEST.RC
DEP_ICMTES=\
	.\ICON1.ICO\
	.\ICON2.ICO\
	.\ICMTEST.H

$(INTDIR)/ICMTEST.res :  $(SOURCE)  $(DEP_ICMTES) $(INTDIR)
   $(RSC) $(RSC_PROJ)  $(SOURCE) 

# End Source File
# End Group
# End Project
################################################################################
