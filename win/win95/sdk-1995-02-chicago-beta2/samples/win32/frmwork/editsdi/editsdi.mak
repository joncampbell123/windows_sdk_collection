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
!MESSAGE NMAKE /f "editsdi.mak" CFG="Win32 Debug"
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

ALL : $(OUTDIR)/editsdi.exe $(OUTDIR)/editsdi.bsc

$(OUTDIR) : 
    if not exist $(OUTDIR)/nul mkdir $(OUTDIR)

# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
# ADD BASE CPP /nologo /W3 /GX /YX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /FR /c
# ADD CPP /nologo /W3 /GX /YX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /FR /c
CPP_PROJ=/nologo /W3 /GX /YX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS"\
 /FR$(INTDIR)/ /Fp$(OUTDIR)/"editsdi.pch" /Fo$(INTDIR)/ /c 
CPP_OBJS=.\WinRel/
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
RSC_PROJ=/l 0x409 /fo$(INTDIR)/"editsdi.res" /d "NDEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o$(OUTDIR)/"editsdi.bsc" 
BSC32_SBRS= \
	$(INTDIR)/search.sbr \
	$(INTDIR)/dispatch.sbr \
	$(INTDIR)/print.sbr \
	$(INTDIR)/misc.sbr \
	$(INTDIR)/finddlg.sbr \
	$(INTDIR)/init.sbr \
	$(INTDIR)/editsdi.sbr \
	$(INTDIR)/file.sbr \
	$(INTDIR)/about.sbr \
	$(INTDIR)/filedlg.sbr \
	$(INTDIR)/printdlg.sbr \
	$(INTDIR)/winmain.sbr

$(OUTDIR)/editsdi.bsc : $(OUTDIR)  $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /NOLOGO /SUBSYSTEM:windows /MACHINE:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib version.lib /NOLOGO /MACHINE:I386 /SUBSYSTEM:windows,4.0
# SUBTRACT LINK32 /PDB:none
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib version.lib /NOLOGO\
 /INCREMENTAL:no /PDB:$(OUTDIR)/"editsdi.pdb" /MACHINE:I386\
 /OUT:$(OUTDIR)/"editsdi.exe" /SUBSYSTEM:windows,4.0  
DEF_FILE=
LINK32_OBJS= \
	$(INTDIR)/search.obj \
	$(INTDIR)/dispatch.obj \
	$(INTDIR)/print.obj \
	$(INTDIR)/misc.obj \
	$(INTDIR)/finddlg.obj \
	$(INTDIR)/editsdi.res \
	$(INTDIR)/init.obj \
	$(INTDIR)/editsdi.obj \
	$(INTDIR)/file.obj \
	$(INTDIR)/about.obj \
	$(INTDIR)/filedlg.obj \
	$(INTDIR)/printdlg.obj \
	$(INTDIR)/winmain.obj

$(OUTDIR)/editsdi.exe : $(OUTDIR)  $(DEF_FILE) $(LINK32_OBJS)
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

ALL : $(OUTDIR)/editsdi.exe $(OUTDIR)/editsdi.bsc

$(OUTDIR) : 
    if not exist $(OUTDIR)/nul mkdir $(OUTDIR)

# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
# ADD BASE CPP /nologo /W3 /GX /Zi /YX /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /FR /c
# ADD CPP /nologo /W3 /GX /Zi /YX /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /FR /c
CPP_PROJ=/nologo /W3 /GX /Zi /YX /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /FR$(INTDIR)/ /Fp$(OUTDIR)/"editsdi.pch" /Fo$(INTDIR)/\
 /Fd$(OUTDIR)/"editsdi.pdb" /c 
CPP_OBJS=.\WinDebug/
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
RSC_PROJ=/l 0x409 /fo$(INTDIR)/"editsdi.res" /d "_DEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o$(OUTDIR)/"editsdi.bsc" 
BSC32_SBRS= \
	$(INTDIR)/search.sbr \
	$(INTDIR)/dispatch.sbr \
	$(INTDIR)/print.sbr \
	$(INTDIR)/misc.sbr \
	$(INTDIR)/finddlg.sbr \
	$(INTDIR)/init.sbr \
	$(INTDIR)/editsdi.sbr \
	$(INTDIR)/file.sbr \
	$(INTDIR)/about.sbr \
	$(INTDIR)/filedlg.sbr \
	$(INTDIR)/printdlg.sbr \
	$(INTDIR)/winmain.sbr

$(OUTDIR)/editsdi.bsc : $(OUTDIR)  $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /NOLOGO /SUBSYSTEM:windows /DEBUG /MACHINE:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib version.lib /NOLOGO /DEBUG /MACHINE:I386 /SUBSYSTEM:windows,4.0
# SUBTRACT LINK32 /PDB:none
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib version.lib /NOLOGO\
 /INCREMENTAL:yes /PDB:$(OUTDIR)/"editsdi.pdb" /DEBUG /MACHINE:I386\
 /OUT:$(OUTDIR)/"editsdi.exe" /SUBSYSTEM:windows,4.0  
DEF_FILE=
LINK32_OBJS= \
	$(INTDIR)/search.obj \
	$(INTDIR)/dispatch.obj \
	$(INTDIR)/print.obj \
	$(INTDIR)/misc.obj \
	$(INTDIR)/finddlg.obj \
	$(INTDIR)/editsdi.res \
	$(INTDIR)/init.obj \
	$(INTDIR)/editsdi.obj \
	$(INTDIR)/file.obj \
	$(INTDIR)/about.obj \
	$(INTDIR)/filedlg.obj \
	$(INTDIR)/printdlg.obj \
	$(INTDIR)/winmain.obj

$(OUTDIR)/editsdi.exe : $(OUTDIR)  $(DEF_FILE) $(LINK32_OBJS)
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

SOURCE=.\search.c
DEP_SEARC=\
	.\win16ext.h\
	.\globals.h\
	.\editsdi.inl

$(INTDIR)/search.obj :  $(SOURCE)  $(DEP_SEARC) $(INTDIR)

# End Source File
################################################################################
# Begin Source File

SOURCE=.\dispatch.c
DEP_DISPA=\
	.\win16ext.h\
	.\globals.h\
	.\editsdi.inl

$(INTDIR)/dispatch.obj :  $(SOURCE)  $(DEP_DISPA) $(INTDIR)

# End Source File
################################################################################
# Begin Source File

SOURCE=.\print.c
DEP_PRINT=\
	.\win16ext.h\
	.\globals.h\
	.\editsdi.inl

$(INTDIR)/print.obj :  $(SOURCE)  $(DEP_PRINT) $(INTDIR)

# End Source File
################################################################################
# Begin Source File

SOURCE=.\misc.c
DEP_MISC_=\
	.\win16ext.h\
	.\globals.h\
	.\editsdi.inl

$(INTDIR)/misc.obj :  $(SOURCE)  $(DEP_MISC_) $(INTDIR)

# End Source File
################################################################################
# Begin Source File

SOURCE=.\finddlg.c
DEP_FINDD=\
	.\win16ext.h\
	.\globals.h\
	.\editsdi.inl

$(INTDIR)/finddlg.obj :  $(SOURCE)  $(DEP_FINDD) $(INTDIR)

# End Source File
################################################################################
# Begin Source File

SOURCE=.\editsdi.rc
DEP_EDITS=\
	.\editsdi.ico\
	.\globals.h\
	.\version.h\
	.\editsdi.inl

$(INTDIR)/editsdi.res :  $(SOURCE)  $(DEP_EDITS) $(INTDIR)
   $(RSC) $(RSC_PROJ)  $(SOURCE) 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\init.c
DEP_INIT_=\
	.\win16ext.h\
	.\globals.h\
	.\editsdi.inl

$(INTDIR)/init.obj :  $(SOURCE)  $(DEP_INIT_) $(INTDIR)

# End Source File
################################################################################
# Begin Source File

SOURCE=.\editsdi.c
DEP_EDITSD=\
	.\win16ext.h\
	.\globals.h\
	.\editsdi.inl

$(INTDIR)/editsdi.obj :  $(SOURCE)  $(DEP_EDITSD) $(INTDIR)

# End Source File
################################################################################
# Begin Source File

SOURCE=.\file.c
DEP_FILE_=\
	.\win16ext.h\
	.\globals.h\
	.\editsdi.inl

$(INTDIR)/file.obj :  $(SOURCE)  $(DEP_FILE_) $(INTDIR)

# End Source File
################################################################################
# Begin Source File

SOURCE=.\about.c
DEP_ABOUT=\
	.\win16ext.h\
	.\globals.h\
	.\editsdi.inl

$(INTDIR)/about.obj :  $(SOURCE)  $(DEP_ABOUT) $(INTDIR)

# End Source File
################################################################################
# Begin Source File

SOURCE=.\filedlg.c
DEP_FILED=\
	.\win16ext.h\
	.\globals.h\
	.\editsdi.inl

$(INTDIR)/filedlg.obj :  $(SOURCE)  $(DEP_FILED) $(INTDIR)

# End Source File
################################################################################
# Begin Source File

SOURCE=.\printdlg.c
DEP_PRINTD=\
	.\win16ext.h\
	.\globals.h\
	.\editsdi.inl

$(INTDIR)/printdlg.obj :  $(SOURCE)  $(DEP_PRINTD) $(INTDIR)

# End Source File
################################################################################
# Begin Source File

SOURCE=.\winmain.c
DEP_WINMA=\
	.\win16ext.h\
	.\globals.h\
	.\editsdi.inl

$(INTDIR)/winmain.obj :  $(SOURCE)  $(DEP_WINMA) $(INTDIR)

# End Source File
# End Group
# End Project
################################################################################
