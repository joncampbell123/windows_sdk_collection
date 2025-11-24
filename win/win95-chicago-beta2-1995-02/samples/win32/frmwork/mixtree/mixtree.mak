# Microsoft Visual C++ Generated NMAKE File, Format Version 2.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

!IF "$(CFG)" == ""
CFG=Win32 Debug
!MESSAGE No configuration specified.  Defaulting to Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "Win32 Release" && "$(CFG)" != "Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "MIXTREE.MAK" CFG="Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

################################################################################
# Begin Project
# PROP Target_Last_Scanned "Win32 Debug"
MTL=MkTypLib.exe
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "WinRel"
# PROP BASE Intermediate_Dir "WinRel"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "WinRel"
# PROP Intermediate_Dir "WinRel"
OUTDIR=.\WinRel
INTDIR=.\WinRel

ALL : $(OUTDIR)/MIXTREE.exe $(OUTDIR)/MIXTREE.bsc

$(OUTDIR) : 
    if not exist $(OUTDIR)/nul mkdir $(OUTDIR)

# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
# ADD BASE CPP /nologo /W3 /GX /YX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /FR /c
# ADD CPP /nologo /W3 /GX /YX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /FR /c
CPP_PROJ=/nologo /W3 /GX /YX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS"\
 /FR$(INTDIR)/ /Fp$(OUTDIR)/"MIXTREE.pch" /Fo$(INTDIR)/ /c 
CPP_OBJS=.\WinRel/
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
RSC_PROJ=/l 0x409 /fo$(INTDIR)/"MIXTREE.res" /d "NDEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o$(OUTDIR)/"MIXTREE.bsc" 
BSC32_SBRS= \
	$(INTDIR)/ABOUT.SBR \
	$(INTDIR)/INIT.SBR \
	$(INTDIR)/MIXLINE.SBR \
	$(INTDIR)/DISPATCH.SBR \
	$(INTDIR)/WINMAIN.SBR \
	$(INTDIR)/MISC.SBR \
	$(INTDIR)/MIXTREE.SBR \
	$(INTDIR)/MIXINFO.SBR \
	$(INTDIR)/ITEMINFO.SBR

$(OUTDIR)/MIXTREE.bsc : $(OUTDIR)  $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /NOLOGO /SUBSYSTEM:windows /MACHINE:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib version.lib comdlg32.lib advapi32.lib winmm.lib comctl32.lib /NOLOGO /SUBSYSTEM:windows /MACHINE:I386
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib version.lib comdlg32.lib\
 advapi32.lib winmm.lib comctl32.lib /NOLOGO /SUBSYSTEM:windows /INCREMENTAL:no\
 /PDB:$(OUTDIR)/"MIXTREE.pdb" /MACHINE:I386 /OUT:$(OUTDIR)/"MIXTREE.exe" 
DEF_FILE=
LINK32_OBJS= \
	$(INTDIR)/ABOUT.OBJ \
	$(INTDIR)/INIT.OBJ \
	$(INTDIR)/MIXLINE.OBJ \
	$(INTDIR)/DISPATCH.OBJ \
	$(INTDIR)/WINMAIN.OBJ \
	$(INTDIR)/MIXTREE.res \
	$(INTDIR)/MISC.OBJ \
	$(INTDIR)/MIXTREE.OBJ \
	$(INTDIR)/MIXINFO.OBJ \
	$(INTDIR)/ITEMINFO.OBJ

$(OUTDIR)/MIXTREE.exe : $(OUTDIR)  $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "WinDebug"
# PROP BASE Intermediate_Dir "WinDebug"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "WinDebug"
# PROP Intermediate_Dir "WinDebug"
OUTDIR=.\WinDebug
INTDIR=.\WinDebug

ALL : $(OUTDIR)/MIXTREE.exe $(OUTDIR)/MIXTREE.bsc

$(OUTDIR) : 
    if not exist $(OUTDIR)/nul mkdir $(OUTDIR)

# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
# ADD BASE CPP /nologo /W3 /GX /Zi /YX /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /FR /c
# ADD CPP /nologo /W3 /GX /Zi /YX /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /FR /c
CPP_PROJ=/nologo /W3 /GX /Zi /YX /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /FR$(INTDIR)/ /Fp$(OUTDIR)/"MIXTREE.pch" /Fo$(INTDIR)/\
 /Fd$(OUTDIR)/"MIXTREE.pdb" /c 
CPP_OBJS=.\WinDebug/
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
RSC_PROJ=/l 0x409 /fo$(INTDIR)/"MIXTREE.res" /d "_DEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o$(OUTDIR)/"MIXTREE.bsc" 
BSC32_SBRS= \
	$(INTDIR)/ABOUT.SBR \
	$(INTDIR)/INIT.SBR \
	$(INTDIR)/MIXLINE.SBR \
	$(INTDIR)/DISPATCH.SBR \
	$(INTDIR)/WINMAIN.SBR \
	$(INTDIR)/MISC.SBR \
	$(INTDIR)/MIXTREE.SBR \
	$(INTDIR)/MIXINFO.SBR \
	$(INTDIR)/ITEMINFO.SBR

$(OUTDIR)/MIXTREE.bsc : $(OUTDIR)  $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /NOLOGO /SUBSYSTEM:windows /DEBUG /MACHINE:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib version.lib comdlg32.lib advapi32.lib winmm.lib comctl32.lib /NOLOGO /SUBSYSTEM:windows /DEBUG /MACHINE:I386
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib version.lib comdlg32.lib\
 advapi32.lib winmm.lib comctl32.lib /NOLOGO /SUBSYSTEM:windows /INCREMENTAL:yes\
 /PDB:$(OUTDIR)/"MIXTREE.pdb" /DEBUG /MACHINE:I386 /OUT:$(OUTDIR)/"MIXTREE.exe" 
DEF_FILE=
LINK32_OBJS= \
	$(INTDIR)/ABOUT.OBJ \
	$(INTDIR)/INIT.OBJ \
	$(INTDIR)/MIXLINE.OBJ \
	$(INTDIR)/DISPATCH.OBJ \
	$(INTDIR)/WINMAIN.OBJ \
	$(INTDIR)/MIXTREE.res \
	$(INTDIR)/MISC.OBJ \
	$(INTDIR)/MIXTREE.OBJ \
	$(INTDIR)/MIXINFO.OBJ \
	$(INTDIR)/ITEMINFO.OBJ

$(OUTDIR)/MIXTREE.exe : $(OUTDIR)  $(DEF_FILE) $(LINK32_OBJS)
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

SOURCE=.\ABOUT.C
DEP_ABOUT=\
	.\GLOBALS.H

$(INTDIR)/ABOUT.OBJ :  $(SOURCE)  $(DEP_ABOUT) $(INTDIR)

# End Source File
################################################################################
# Begin Source File

SOURCE=.\INIT.C
DEP_INIT_=\
	.\GLOBALS.H

$(INTDIR)/INIT.OBJ :  $(SOURCE)  $(DEP_INIT_) $(INTDIR)

# End Source File
################################################################################
# Begin Source File

SOURCE=.\MIXLINE.C
DEP_MIXLI=\
	.\GLOBALS.H\
	.\MIXLINE.H

$(INTDIR)/MIXLINE.OBJ :  $(SOURCE)  $(DEP_MIXLI) $(INTDIR)

# End Source File
################################################################################
# Begin Source File

SOURCE=.\DISPATCH.C
DEP_DISPA=\
	.\GLOBALS.H

$(INTDIR)/DISPATCH.OBJ :  $(SOURCE)  $(DEP_DISPA) $(INTDIR)

# End Source File
################################################################################
# Begin Source File

SOURCE=.\WINMAIN.C
DEP_WINMA=\
	.\GLOBALS.H

$(INTDIR)/WINMAIN.OBJ :  $(SOURCE)  $(DEP_WINMA) $(INTDIR)

# End Source File
################################################################################
# Begin Source File

SOURCE=.\MIXTREE.RC
DEP_MIXTR=\
	.\MIXTREE.ICO\
	.\DESTLINE.BMP\
	.\SRCLINE.BMP\
	.\CUSTOM.BMP\
	.\METER.BMP\
	.\SWITCH.BMP\
	.\NUMBER.BMP\
	.\SLIDER.BMP\
	.\FADER.BMP\
	.\TIME.BMP\
	.\LIST.BMP\
	.\GLOBALS.H

$(INTDIR)/MIXTREE.res :  $(SOURCE)  $(DEP_MIXTR) $(INTDIR)
   $(RSC) $(RSC_PROJ)  $(SOURCE) 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\MISC.C
DEP_MISC_=\
	.\GLOBALS.H

$(INTDIR)/MISC.OBJ :  $(SOURCE)  $(DEP_MISC_) $(INTDIR)

# End Source File
################################################################################
# Begin Source File

SOURCE=.\MIXTREE.C
DEP_MIXTRE=\
	.\GLOBALS.H

$(INTDIR)/MIXTREE.OBJ :  $(SOURCE)  $(DEP_MIXTRE) $(INTDIR)

# End Source File
################################################################################
# Begin Source File

SOURCE=.\MIXINFO.C
DEP_MIXIN=\
	.\GLOBALS.H\
	.\MIXINFO.H

$(INTDIR)/MIXINFO.OBJ :  $(SOURCE)  $(DEP_MIXIN) $(INTDIR)

# End Source File
################################################################################
# Begin Source File

SOURCE=.\ITEMINFO.C
DEP_ITEMI=\
	.\GLOBALS.H\
	.\ITEMINFO.H

$(INTDIR)/ITEMINFO.OBJ :  $(SOURCE)  $(DEP_ITEMI) $(INTDIR)

# End Source File
# End Group
# End Project
################################################################################
