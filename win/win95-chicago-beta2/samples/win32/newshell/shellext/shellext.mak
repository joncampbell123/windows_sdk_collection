# Microsoft Visual C++ Generated NMAKE File, Format Version 2.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

!IF "$(CFG)" == ""
CFG=Win32 Debug
!MESSAGE No configuration specified.  Defaulting to Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "Win32 Release" && "$(CFG)" != "Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "SHELLEXT.MAK" CFG="Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
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

ALL : $(OUTDIR)/SHELLEXT.dll $(OUTDIR)/SHELLEXT.bsc

$(OUTDIR) : 
    if not exist $(OUTDIR)/nul mkdir $(OUTDIR)

# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
# ADD BASE CPP /nologo /MT /W3 /GX /YX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /FR /c
# ADD CPP /nologo /MT /W3 /GX /YX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /FR /c
CPP_PROJ=/nologo /MT /W3 /GX /YX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS"\
 /FR$(INTDIR)/ /Fp$(OUTDIR)/"SHELLEXT.pch" /Fo$(INTDIR)/ /c 
CPP_OBJS=.\WinRel/
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
RSC_PROJ=/l 0x409 /fo$(INTDIR)/"SHELLEXT.res" /d "NDEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o$(OUTDIR)/"SHELLEXT.bsc" 
BSC32_SBRS= \
	$(INTDIR)/SHELLEXT.SBR \
	$(INTDIR)/ICONHDLR.SBR \
	$(INTDIR)/PROPSHET.SBR \
	$(INTDIR)/COPYHOOK.SBR \
	$(INTDIR)/CTXMENU.SBR \
	$(INTDIR)/SHEXINIT.SBR

$(OUTDIR)/SHELLEXT.bsc : $(OUTDIR)  $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /NOLOGO /SUBSYSTEM:windows /DLL /MACHINE:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib comctl32.lib /NOLOGO /DLL /MACHINE:I386 /SUBSYSTEM:windows,4.0
# SUBTRACT LINK32 /PDB:none
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib comctl32.lib /NOLOGO\
 /DLL /INCREMENTAL:no /PDB:$(OUTDIR)/"SHELLEXT.pdb" /MACHINE:I386\
 /DEF:".\SHELLEXT.DEF" /OUT:$(OUTDIR)/"SHELLEXT.dll"\
 /IMPLIB:$(OUTDIR)/"SHELLEXT.lib" /SUBSYSTEM:windows,4.0  
DEF_FILE=.\SHELLEXT.DEF
LINK32_OBJS= \
	$(INTDIR)/SHELLEXT.OBJ \
	$(INTDIR)/SHELLEXT.res \
	$(INTDIR)/ICONHDLR.OBJ \
	$(INTDIR)/PROPSHET.OBJ \
	$(INTDIR)/COPYHOOK.OBJ \
	$(INTDIR)/CTXMENU.OBJ \
	$(INTDIR)/SHEXINIT.OBJ

$(OUTDIR)/SHELLEXT.dll : $(OUTDIR)  $(DEF_FILE) $(LINK32_OBJS)
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

ALL : $(OUTDIR)/SHELLEXT.dll $(OUTDIR)/SHELLEXT.bsc

$(OUTDIR) : 
    if not exist $(OUTDIR)/nul mkdir $(OUTDIR)

# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
# ADD BASE CPP /nologo /MT /W3 /GX /Zi /YX /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /FR /c
# ADD CPP /nologo /MT /W3 /GX /Zi /YX /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /FR /c
CPP_PROJ=/nologo /MT /W3 /GX /Zi /YX /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /FR$(INTDIR)/ /Fp$(OUTDIR)/"SHELLEXT.pch" /Fo$(INTDIR)/\
 /Fd$(OUTDIR)/"SHELLEXT.pdb" /c 
CPP_OBJS=.\WinDebug/
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
RSC_PROJ=/l 0x409 /fo$(INTDIR)/"SHELLEXT.res" /d "_DEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o$(OUTDIR)/"SHELLEXT.bsc" 
BSC32_SBRS= \
	$(INTDIR)/SHELLEXT.SBR \
	$(INTDIR)/ICONHDLR.SBR \
	$(INTDIR)/PROPSHET.SBR \
	$(INTDIR)/COPYHOOK.SBR \
	$(INTDIR)/CTXMENU.SBR \
	$(INTDIR)/SHEXINIT.SBR

$(OUTDIR)/SHELLEXT.bsc : $(OUTDIR)  $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /NOLOGO /SUBSYSTEM:windows /DLL /DEBUG /MACHINE:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib comctl32.lib /NOLOGO /DLL /DEBUG /MACHINE:I386 /SUBSYSTEM:windows,4.0
# SUBTRACT LINK32 /PDB:none
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib comctl32.lib /NOLOGO\
 /DLL /INCREMENTAL:yes /PDB:$(OUTDIR)/"SHELLEXT.pdb" /DEBUG /MACHINE:I386\
 /DEF:".\SHELLEXT.DEF" /OUT:$(OUTDIR)/"SHELLEXT.dll"\
 /IMPLIB:$(OUTDIR)/"SHELLEXT.lib" /SUBSYSTEM:windows,4.0  
DEF_FILE=.\SHELLEXT.DEF
LINK32_OBJS= \
	$(INTDIR)/SHELLEXT.OBJ \
	$(INTDIR)/SHELLEXT.res \
	$(INTDIR)/ICONHDLR.OBJ \
	$(INTDIR)/PROPSHET.OBJ \
	$(INTDIR)/COPYHOOK.OBJ \
	$(INTDIR)/CTXMENU.OBJ \
	$(INTDIR)/SHEXINIT.OBJ

$(OUTDIR)/SHELLEXT.dll : $(OUTDIR)  $(DEF_FILE) $(LINK32_OBJS)
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

SOURCE=.\SHELLEXT.CPP
DEP_SHELL=\
	.\Priv.h\
	.\SHELLEXT.H

$(INTDIR)/SHELLEXT.OBJ :  $(SOURCE)  $(DEP_SHELL) $(INTDIR)

# End Source File
################################################################################
# Begin Source File

SOURCE=.\SHELLEXT.RC
DEP_SHELLE=\
	.\GAK.ICO\
	.\GAK2.ICO\
	.\GAK3.ICO

$(INTDIR)/SHELLEXT.res :  $(SOURCE)  $(DEP_SHELLE) $(INTDIR)
   $(RSC) $(RSC_PROJ)  $(SOURCE) 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\SHELLEXT.DEF
# End Source File
################################################################################
# Begin Source File

SOURCE=.\ICONHDLR.CPP
DEP_ICONH=\
	.\Priv.h\
	.\SHELLEXT.H

$(INTDIR)/ICONHDLR.OBJ :  $(SOURCE)  $(DEP_ICONH) $(INTDIR)

# End Source File
################################################################################
# Begin Source File

SOURCE=.\PROPSHET.CPP
DEP_PROPS=\
	.\Priv.h\
	.\SHELLEXT.H

$(INTDIR)/PROPSHET.OBJ :  $(SOURCE)  $(DEP_PROPS) $(INTDIR)

# End Source File
################################################################################
# Begin Source File

SOURCE=.\COPYHOOK.CPP
DEP_COPYH=\
	.\Priv.h\
	.\SHELLEXT.H

$(INTDIR)/COPYHOOK.OBJ :  $(SOURCE)  $(DEP_COPYH) $(INTDIR)

# End Source File
################################################################################
# Begin Source File

SOURCE=.\CTXMENU.CPP
DEP_CTXME=\
	.\Priv.h\
	.\SHELLEXT.H

$(INTDIR)/CTXMENU.OBJ :  $(SOURCE)  $(DEP_CTXME) $(INTDIR)

# End Source File
################################################################################
# Begin Source File

SOURCE=.\SHEXINIT.CPP
DEP_SHEXI=\
	.\Priv.h\
	.\SHELLEXT.H

$(INTDIR)/SHEXINIT.OBJ :  $(SOURCE)  $(DEP_SHEXI) $(INTDIR)

# End Source File
# End Group
# End Project
################################################################################
