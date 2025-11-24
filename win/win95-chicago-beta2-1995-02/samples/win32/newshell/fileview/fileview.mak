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
!MESSAGE NMAKE /f "fileview.mak" CFG="Win32 Debug"
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

ALL : $(OUTDIR)/fvtext.dll $(OUTDIR)/fileview.bsc

$(OUTDIR) : 
    if not exist $(OUTDIR)/nul mkdir $(OUTDIR)

# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
# ADD BASE CPP /nologo /MT /W3 /GX /YX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /FR /c
# ADD CPP /nologo /MT /W3 /GX /YX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /FR /c
CPP_PROJ=/nologo /MT /W3 /GX /YX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS"\
 /FR$(INTDIR)/ /Fp$(OUTDIR)/"fileview.pch" /Fo$(INTDIR)/ /c 
CPP_OBJS=.\WinRel/
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
RSC_PROJ=/l 0x409 /fo$(INTDIR)/"FVTEXT.res" /d "NDEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o$(OUTDIR)/"fileview.bsc" 
BSC32_SBRS= \
	$(INTDIR)/FVPROC.SBR \
	$(INTDIR)/CSTATHLP.SBR \
	$(INTDIR)/CSTRTABL.SBR \
	$(INTDIR)/IFILEVW.SBR \
	$(INTDIR)/IPERFILE.SBR \
	$(INTDIR)/FVTEXT.SBR \
	$(INTDIR)/FVINIT.SBR \
	$(INTDIR)/FILEVIEW.SBR

$(OUTDIR)/fileview.bsc : $(OUTDIR)  $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /NOLOGO /SUBSYSTEM:windows /DLL /MACHINE:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib comctl32.lib /NOLOGO /DLL /MACHINE:I386 /OUT:"WinRel/fvtext.dll" /SUBSYSTEM:windows,4.0
# SUBTRACT LINK32 /PDB:none
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib comctl32.lib /NOLOGO\
 /DLL /INCREMENTAL:no /PDB:$(OUTDIR)/"fileview.pdb" /MACHINE:I386\
 /DEF:".\fvtext.def" /OUT:"WinRel/fvtext.dll" /IMPLIB:$(OUTDIR)/"fileview.lib"\
 /SUBSYSTEM:windows,4.0  
DEF_FILE=.\fvtext.def
LINK32_OBJS= \
	$(INTDIR)/FVPROC.OBJ \
	$(INTDIR)/CSTATHLP.OBJ \
	$(INTDIR)/CSTRTABL.OBJ \
	$(INTDIR)/IFILEVW.OBJ \
	$(INTDIR)/IPERFILE.OBJ \
	$(INTDIR)/FVTEXT.res \
	$(INTDIR)/FVTEXT.OBJ \
	$(INTDIR)/FVINIT.OBJ \
	$(INTDIR)/FILEVIEW.OBJ

$(OUTDIR)/fvtext.dll : $(OUTDIR)  $(DEF_FILE) $(LINK32_OBJS)
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

ALL : $(OUTDIR)/fvtext.dll $(OUTDIR)/fileview.bsc

$(OUTDIR) : 
    if not exist $(OUTDIR)/nul mkdir $(OUTDIR)

# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
# ADD BASE CPP /nologo /MT /W3 /GX /Zi /YX /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /FR /c
# ADD CPP /nologo /MT /W3 /GX /Zi /YX /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /FR /c
CPP_PROJ=/nologo /MT /W3 /GX /Zi /YX /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /FR$(INTDIR)/ /Fp$(OUTDIR)/"fileview.pch" /Fo$(INTDIR)/\
 /Fd$(OUTDIR)/"fileview.pdb" /c 
CPP_OBJS=.\WinDebug/
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
RSC_PROJ=/l 0x409 /fo$(INTDIR)/"FVTEXT.res" /d "_DEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o$(OUTDIR)/"fileview.bsc" 
BSC32_SBRS= \
	$(INTDIR)/FVPROC.SBR \
	$(INTDIR)/CSTATHLP.SBR \
	$(INTDIR)/CSTRTABL.SBR \
	$(INTDIR)/IFILEVW.SBR \
	$(INTDIR)/IPERFILE.SBR \
	$(INTDIR)/FVTEXT.SBR \
	$(INTDIR)/FVINIT.SBR \
	$(INTDIR)/FILEVIEW.SBR

$(OUTDIR)/fileview.bsc : $(OUTDIR)  $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /NOLOGO /SUBSYSTEM:windows /DLL /DEBUG /MACHINE:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib comctl32.lib /NOLOGO /DLL /DEBUG /MACHINE:I386 /OUT:"WinDebug/fvtext.dll" /SUBSYSTEM:windows,4.0
# SUBTRACT LINK32 /PDB:none
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib comctl32.lib /NOLOGO\
 /DLL /INCREMENTAL:yes /PDB:$(OUTDIR)/"fileview.pdb" /DEBUG /MACHINE:I386\
 /DEF:".\fvtext.def" /OUT:"WinDebug/fvtext.dll" /IMPLIB:$(OUTDIR)/"fileview.lib"\
 /SUBSYSTEM:windows,4.0  
DEF_FILE=.\fvtext.def
LINK32_OBJS= \
	$(INTDIR)/FVPROC.OBJ \
	$(INTDIR)/CSTATHLP.OBJ \
	$(INTDIR)/CSTRTABL.OBJ \
	$(INTDIR)/IFILEVW.OBJ \
	$(INTDIR)/IPERFILE.OBJ \
	$(INTDIR)/FVTEXT.res \
	$(INTDIR)/FVTEXT.OBJ \
	$(INTDIR)/FVINIT.OBJ \
	$(INTDIR)/FILEVIEW.OBJ

$(OUTDIR)/fvtext.dll : $(OUTDIR)  $(DEF_FILE) $(LINK32_OBJS)
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

SOURCE=.\FVPROC.CPP
DEP_FVPRO=\
	.\FILEVIEW.H\
	.\dbgout.h\
	.\cstrtabl.h\
	.\cstathlp.h\
	.\FVTEXT.H\
	.\RESOURCE.H\

$(INTDIR)/FVPROC.OBJ :  $(SOURCE)  $(DEP_FVPRO) $(INTDIR)

# End Source File
################################################################################
# Begin Source File

SOURCE=.\CSTATHLP.CPP
DEP_CSTAT=\
	.\FILEVIEW.H\
	.\dbgout.h\
	.\cstrtabl.h\
	.\cstathlp.h\
	.\FVTEXT.H\
	.\RESOURCE.H\

$(INTDIR)/CSTATHLP.OBJ :  $(SOURCE)  $(DEP_CSTAT) $(INTDIR)

# End Source File
################################################################################
# Begin Source File

SOURCE=.\fvtext.def
# End Source File
################################################################################
# Begin Source File

SOURCE=.\CSTRTABL.CPP
DEP_CSTRT=\
	.\FILEVIEW.H\
	.\dbgout.h\
	.\cstrtabl.h\
	.\cstathlp.h\
	.\FVTEXT.H\
	.\RESOURCE.H\

$(INTDIR)/CSTRTABL.OBJ :  $(SOURCE)  $(DEP_CSTRT) $(INTDIR)

# End Source File
################################################################################
# Begin Source File

SOURCE=.\IFILEVW.CPP
DEP_IFILE=\
	.\FILEVIEW.H\
	.\dbgout.h\
	.\cstrtabl.h\
	.\cstathlp.h\
	.\FVTEXT.H\
	.\RESOURCE.H\

$(INTDIR)/IFILEVW.OBJ :  $(SOURCE)  $(DEP_IFILE) $(INTDIR)

# End Source File
################################################################################
# Begin Source File

SOURCE=.\IPERFILE.CPP
DEP_IPERF=\
	.\FILEVIEW.H\
	.\dbgout.h\
	.\cstrtabl.h\
	.\cstathlp.h\
	.\FVTEXT.H\
	.\RESOURCE.H\

$(INTDIR)/IPERFILE.OBJ :  $(SOURCE)  $(DEP_IPERF) $(INTDIR)

# End Source File
################################################################################
# Begin Source File

SOURCE=.\FVTEXT.RC
DEP_FVTEX=\
	.\tools.bmp\
	.\FILEVIEW.H\
	.\dbgout.h\
	.\cstrtabl.h\
	.\cstathlp.h\
	.\FVTEXT.H\
	.\RESOURCE.H\

$(INTDIR)/FVTEXT.res :  $(SOURCE)  $(DEP_FVTEX) $(INTDIR)
   $(RSC) $(RSC_PROJ)  $(SOURCE) 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\FVTEXT.CPP
DEP_FVTEXT=\
	.\FILEVIEW.H\
	.\dbgout.h\
	.\cstrtabl.h\
	.\cstathlp.h\
	.\FVTEXT.H\
	.\RESOURCE.H\

$(INTDIR)/FVTEXT.OBJ :  $(SOURCE)  $(DEP_FVTEXT) $(INTDIR)

# End Source File
################################################################################
# Begin Source File

SOURCE=.\FVINIT.CPP
DEP_FVINI=\
	.\FILEVIEW.H\
	.\dbgout.h\
	.\cstrtabl.h\
	.\cstathlp.h\
	.\FVTEXT.H\
	.\RESOURCE.H\

$(INTDIR)/FVINIT.OBJ :  $(SOURCE)  $(DEP_FVINI) $(INTDIR)

# End Source File
################################################################################
# Begin Source File

SOURCE=.\FILEVIEW.CPP
DEP_FILEV=\
	.\FILEVIEW.H\
	.\FVTEXT.H\
	.\dbgout.h\
	.\cstrtabl.h\
	.\cstathlp.h\
	.\RESOURCE.H\

$(INTDIR)/FILEVIEW.OBJ :  $(SOURCE)  $(DEP_FILEV) $(INTDIR)

# End Source File
# End Group
# End Project
################################################################################
