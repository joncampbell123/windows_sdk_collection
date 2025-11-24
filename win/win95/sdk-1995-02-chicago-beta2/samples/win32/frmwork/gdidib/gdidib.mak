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
!MESSAGE NMAKE /f "gdidib.mak" CFG="Win32 Debug"
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

ALL : $(OUTDIR)/gdidib.exe $(OUTDIR)/gdidib.bsc

$(OUTDIR) : 
    if not exist $(OUTDIR)/nul mkdir $(OUTDIR)

# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
# ADD BASE CPP /nologo /W3 /GX /YX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /FR /c
# ADD CPP /nologo /W3 /GX /YX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /FR /c
CPP_PROJ=/nologo /W3 /GX /YX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS"\
 /FR$(INTDIR)/ /Fp$(OUTDIR)/"gdidib.pch" /Fo$(INTDIR)/ /c 
CPP_OBJS=.\WinRel/
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
RSC_PROJ=/l 0x409 /fo$(INTDIR)/"gdidib.res" /d "NDEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o$(OUTDIR)/"gdidib.bsc" 
BSC32_SBRS= \
	$(INTDIR)/fileio.sbr \
	$(INTDIR)/filedlg.sbr \
	$(INTDIR)/about.sbr \
	$(INTDIR)/print.sbr \
	$(INTDIR)/winmain.sbr \
	$(INTDIR)/statbar.sbr \
	$(INTDIR)/filenew.sbr \
	$(INTDIR)/brushdlg.sbr \
	$(INTDIR)/dispatch.sbr \
	$(INTDIR)/toolbar.sbr \
	$(INTDIR)/dibutil.sbr \
	$(INTDIR)/palette.sbr \
	$(INTDIR)/misc.sbr \
	$(INTDIR)/colordlg.sbr \
	$(INTDIR)/pendlg.sbr \
	$(INTDIR)/infodlg.sbr \
	$(INTDIR)/client.sbr \
	$(INTDIR)/palctrl.sbr \
	$(INTDIR)/gdidib.sbr \
	$(INTDIR)/init.sbr

$(OUTDIR)/gdidib.bsc : $(OUTDIR)  $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /NOLOGO /SUBSYSTEM:windows /MACHINE:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib comctl32.lib version.lib /NOLOGO /MACHINE:I386 /SUBSYSTEM:windows,4.0
# SUBTRACT LINK32 /PDB:none
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib comctl32.lib\
 version.lib /NOLOGO /INCREMENTAL:no /PDB:$(OUTDIR)/"gdidib.pdb" /MACHINE:I386\
 /OUT:$(OUTDIR)/"gdidib.exe" /SUBSYSTEM:windows,4.0  
DEF_FILE=
LINK32_OBJS= \
	$(INTDIR)/fileio.obj \
	$(INTDIR)/filedlg.obj \
	$(INTDIR)/about.obj \
	$(INTDIR)/print.obj \
	$(INTDIR)/winmain.obj \
	$(INTDIR)/statbar.obj \
	$(INTDIR)/filenew.obj \
	$(INTDIR)/brushdlg.obj \
	$(INTDIR)/gdidib.res \
	$(INTDIR)/dispatch.obj \
	$(INTDIR)/toolbar.obj \
	$(INTDIR)/dibutil.obj \
	$(INTDIR)/palette.obj \
	$(INTDIR)/misc.obj \
	$(INTDIR)/colordlg.obj \
	$(INTDIR)/pendlg.obj \
	$(INTDIR)/infodlg.obj \
	$(INTDIR)/client.obj \
	$(INTDIR)/palctrl.obj \
	$(INTDIR)/gdidib.obj \
	$(INTDIR)/init.obj

$(OUTDIR)/gdidib.exe : $(OUTDIR)  $(DEF_FILE) $(LINK32_OBJS)
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

ALL : $(OUTDIR)/gdidib.exe $(OUTDIR)/gdidib.bsc

$(OUTDIR) : 
    if not exist $(OUTDIR)/nul mkdir $(OUTDIR)

# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
# ADD BASE CPP /nologo /W3 /GX /Zi /YX /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /FR /c
# ADD CPP /nologo /W3 /GX /Zi /YX /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /FR /c
CPP_PROJ=/nologo /W3 /GX /Zi /YX /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /FR$(INTDIR)/ /Fp$(OUTDIR)/"gdidib.pch" /Fo$(INTDIR)/ /Fd$(OUTDIR)/"gdidib.pdb"\
 /c 
CPP_OBJS=.\WinDebug/
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
RSC_PROJ=/l 0x409 /fo$(INTDIR)/"gdidib.res" /d "_DEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o$(OUTDIR)/"gdidib.bsc" 
BSC32_SBRS= \
	$(INTDIR)/fileio.sbr \
	$(INTDIR)/filedlg.sbr \
	$(INTDIR)/about.sbr \
	$(INTDIR)/print.sbr \
	$(INTDIR)/winmain.sbr \
	$(INTDIR)/statbar.sbr \
	$(INTDIR)/filenew.sbr \
	$(INTDIR)/brushdlg.sbr \
	$(INTDIR)/dispatch.sbr \
	$(INTDIR)/toolbar.sbr \
	$(INTDIR)/dibutil.sbr \
	$(INTDIR)/palette.sbr \
	$(INTDIR)/misc.sbr \
	$(INTDIR)/colordlg.sbr \
	$(INTDIR)/pendlg.sbr \
	$(INTDIR)/infodlg.sbr \
	$(INTDIR)/client.sbr \
	$(INTDIR)/palctrl.sbr \
	$(INTDIR)/gdidib.sbr \
	$(INTDIR)/init.sbr

$(OUTDIR)/gdidib.bsc : $(OUTDIR)  $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /NOLOGO /SUBSYSTEM:windows /DEBUG /MACHINE:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib comctl32.lib version.lib /NOLOGO /DEBUG /MACHINE:I386 /SUBSYSTEM:windows,4.0
# SUBTRACT LINK32 /PDB:none
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib comctl32.lib\
 version.lib /NOLOGO /INCREMENTAL:yes /PDB:$(OUTDIR)/"gdidib.pdb" /DEBUG\
 /MACHINE:I386 /OUT:$(OUTDIR)/"gdidib.exe" /SUBSYSTEM:windows,4.0  
DEF_FILE=
LINK32_OBJS= \
	$(INTDIR)/fileio.obj \
	$(INTDIR)/filedlg.obj \
	$(INTDIR)/about.obj \
	$(INTDIR)/print.obj \
	$(INTDIR)/winmain.obj \
	$(INTDIR)/statbar.obj \
	$(INTDIR)/filenew.obj \
	$(INTDIR)/brushdlg.obj \
	$(INTDIR)/gdidib.res \
	$(INTDIR)/dispatch.obj \
	$(INTDIR)/toolbar.obj \
	$(INTDIR)/dibutil.obj \
	$(INTDIR)/palette.obj \
	$(INTDIR)/misc.obj \
	$(INTDIR)/colordlg.obj \
	$(INTDIR)/pendlg.obj \
	$(INTDIR)/infodlg.obj \
	$(INTDIR)/client.obj \
	$(INTDIR)/palctrl.obj \
	$(INTDIR)/gdidib.obj \
	$(INTDIR)/init.obj

$(OUTDIR)/gdidib.exe : $(OUTDIR)  $(DEF_FILE) $(LINK32_OBJS)
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

SOURCE=.\fileio.c
DEP_FILEI=\
	.\globals.h\
	.\toolbar.h\
	.\palette.h\
	.\dibutil.h

$(INTDIR)/fileio.obj :  $(SOURCE)  $(DEP_FILEI) $(INTDIR)

# End Source File
################################################################################
# Begin Source File

SOURCE=.\filedlg.c
DEP_FILED=\
	.\globals.h\
	.\statbar.h\
	.\toolbar.h\
	.\palette.h\
	.\dibutil.h

$(INTDIR)/filedlg.obj :  $(SOURCE)  $(DEP_FILED) $(INTDIR)

# End Source File
################################################################################
# Begin Source File

SOURCE=.\about.c
DEP_ABOUT=\
	.\globals.h

$(INTDIR)/about.obj :  $(SOURCE)  $(DEP_ABOUT) $(INTDIR)

# End Source File
################################################################################
# Begin Source File

SOURCE=.\print.c
DEP_PRINT=\
	.\globals.h\
	.\statbar.h

$(INTDIR)/print.obj :  $(SOURCE)  $(DEP_PRINT) $(INTDIR)

# End Source File
################################################################################
# Begin Source File

SOURCE=.\winmain.c
DEP_WINMA=\
	.\globals.h

$(INTDIR)/winmain.obj :  $(SOURCE)  $(DEP_WINMA) $(INTDIR)

# End Source File
################################################################################
# Begin Source File

SOURCE=.\statbar.c
DEP_STATB=\
	.\globals.h\
	.\statbar.h

$(INTDIR)/statbar.obj :  $(SOURCE)  $(DEP_STATB) $(INTDIR)

# End Source File
################################################################################
# Begin Source File

SOURCE=.\filenew.c
DEP_FILEN=\
	.\globals.h\
	.\toolbar.h\
	.\statbar.h\
	.\palette.h\
	.\dibutil.h

$(INTDIR)/filenew.obj :  $(SOURCE)  $(DEP_FILEN) $(INTDIR)

# End Source File
################################################################################
# Begin Source File

SOURCE=.\brushdlg.c
DEP_BRUSH=\
	.\globals.h\
	.\brushdlg.h\
	.\palette.h

$(INTDIR)/brushdlg.obj :  $(SOURCE)  $(DEP_BRUSH) $(INTDIR)

# End Source File
################################################################################
# Begin Source File

SOURCE=.\gdidib.rc
DEP_GDIDI=\
	.\gdidib.ico\
	.\toolbar.bmp\
	.\globals.h\
	.\toolbar.h\
	.\statbar.h\
	.\pendlg.h\
	.\brushdlg.h\
	.\infodlg.h

$(INTDIR)/gdidib.res :  $(SOURCE)  $(DEP_GDIDI) $(INTDIR)
   $(RSC) $(RSC_PROJ)  $(SOURCE) 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\dispatch.c
DEP_DISPA=\
	.\globals.h

$(INTDIR)/dispatch.obj :  $(SOURCE)  $(DEP_DISPA) $(INTDIR)

# End Source File
################################################################################
# Begin Source File

SOURCE=.\toolbar.c
DEP_TOOLB=\
	.\globals.h\
	.\toolbar.h

$(INTDIR)/toolbar.obj :  $(SOURCE)  $(DEP_TOOLB) $(INTDIR)

# End Source File
################################################################################
# Begin Source File

SOURCE=.\dibutil.c
DEP_DIBUT=\
	.\globals.h\
	.\palette.h\
	.\dibutil.h

$(INTDIR)/dibutil.obj :  $(SOURCE)  $(DEP_DIBUT) $(INTDIR)

# End Source File
################################################################################
# Begin Source File

SOURCE=.\palette.c
DEP_PALET=\
	.\globals.h\
	.\palette.h

$(INTDIR)/palette.obj :  $(SOURCE)  $(DEP_PALET) $(INTDIR)

# End Source File
################################################################################
# Begin Source File

SOURCE=.\misc.c
DEP_MISC_=\
	.\globals.h\
	.\toolbar.h

$(INTDIR)/misc.obj :  $(SOURCE)  $(DEP_MISC_) $(INTDIR)

# End Source File
################################################################################
# Begin Source File

SOURCE=.\colordlg.c
DEP_COLOR=\
	.\globals.h

$(INTDIR)/colordlg.obj :  $(SOURCE)  $(DEP_COLOR) $(INTDIR)

# End Source File
################################################################################
# Begin Source File

SOURCE=.\pendlg.c
DEP_PENDL=\
	.\globals.h\
	.\pendlg.h

$(INTDIR)/pendlg.obj :  $(SOURCE)  $(DEP_PENDL) $(INTDIR)

# End Source File
################################################################################
# Begin Source File

SOURCE=.\infodlg.c
DEP_INFOD=\
	.\globals.h\
	.\infodlg.h

$(INTDIR)/infodlg.obj :  $(SOURCE)  $(DEP_INFOD) $(INTDIR)

# End Source File
################################################################################
# Begin Source File

SOURCE=.\client.c
DEP_CLIEN=\
	.\globals.h\
	.\toolbar.h\
	.\statbar.h\
	.\pendlg.h\
	.\brushdlg.h\
	.\dibutil.h

$(INTDIR)/client.obj :  $(SOURCE)  $(DEP_CLIEN) $(INTDIR)

# End Source File
################################################################################
# Begin Source File

SOURCE=.\palctrl.c
DEP_PALCT=\
	.\globals.h\
	.\palette.h\
	.\palctrl.h

$(INTDIR)/palctrl.obj :  $(SOURCE)  $(DEP_PALCT) $(INTDIR)

# End Source File
################################################################################
# Begin Source File

SOURCE=.\gdidib.c
DEP_GDIDIB=\
	.\globals.h\
	.\toolbar.h\
	.\statbar.h\
	.\palette.h

$(INTDIR)/gdidib.obj :  $(SOURCE)  $(DEP_GDIDIB) $(INTDIR)

# End Source File
################################################################################
# Begin Source File

SOURCE=.\init.c
DEP_INIT_=\
	.\globals.h\
	.\palctrl.h

$(INTDIR)/init.obj :  $(SOURCE)  $(DEP_INIT_) $(INTDIR)

# End Source File
# End Group
# End Project
################################################################################
