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
!MESSAGE NMAKE /f "DSSTREAM.MAK" CFG="Win32 Debug"
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
# PROP Output_Dir "retail"
# PROP Intermediate_Dir "retail"
OUTDIR=.\retail
INTDIR=.\retail

ALL : $(OUTDIR)/DSSTREAM.exe $(OUTDIR)/DSSTREAM.bsc

$(OUTDIR) : 
    if not exist $(OUTDIR)/nul mkdir $(OUTDIR)

# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
# ADD BASE CPP /nologo /W3 /GX /YX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /FR /c
# ADD CPP /nologo /W3 /GX /YX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /FR /c
CPP_PROJ=/nologo /W3 /GX /YX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS"\
 /FR$(INTDIR)/ /Fp$(OUTDIR)/"DSSTREAM.pch" /Fo$(INTDIR)/ /c 
CPP_OBJS=.\retail/
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
RSC_PROJ=/l 0x409 /fo$(INTDIR)/"DSStream.res" /d "NDEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o$(OUTDIR)/"DSSTREAM.bsc" 
BSC32_SBRS= \
	$(INTDIR)/DEBUG.SBR \
	$(INTDIR)/DSStream.sbr \
	$(INTDIR)/dstrtime.sbr \
	$(INTDIR)/DSTRWAVE.SBR \
	$(INTDIR)/WASSERT.SBR \
	$(INTDIR)/dstrenum.sbr

$(OUTDIR)/DSSTREAM.bsc : $(OUTDIR)  $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /NOLOGO /SUBSYSTEM:windows /MACHINE:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib comdlg32.lib shell32.lib uuid.lib winmm.lib dsound.lib /NOLOGO /SUBSYSTEM:windows /MACHINE:I386
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib comdlg32.lib shell32.lib\
 uuid.lib winmm.lib dsound.lib /NOLOGO /SUBSYSTEM:windows /INCREMENTAL:no\
 /PDB:$(OUTDIR)/"DSSTREAM.pdb" /MACHINE:I386 /DEF:".\DSStream.def"\
 /OUT:$(OUTDIR)/"DSSTREAM.exe" 
DEF_FILE=.\DSStream.def
LINK32_OBJS= \
	$(INTDIR)/DSStream.res \
	$(INTDIR)/DEBUG.OBJ \
	$(INTDIR)/DSStream.obj \
	$(INTDIR)/dstrtime.obj \
	$(INTDIR)/DSTRWAVE.OBJ \
	$(INTDIR)/WASSERT.OBJ \
	$(INTDIR)/dstrenum.obj

$(OUTDIR)/DSSTREAM.exe : $(OUTDIR)  $(DEF_FILE) $(LINK32_OBJS)
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
# PROP Output_Dir "debug"
# PROP Intermediate_Dir "debug"
OUTDIR=.\debug
INTDIR=.\debug

ALL : $(OUTDIR)/DSSTREAM.exe $(OUTDIR)/DSSTREAM.bsc

$(OUTDIR) : 
    if not exist $(OUTDIR)/nul mkdir $(OUTDIR)

# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
# ADD BASE CPP /nologo /W3 /GX /Zi /YX /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /FR /c
# ADD CPP /nologo /W3 /GX /Zi /YX /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "DEBUG" /FR /c
CPP_PROJ=/nologo /W3 /GX /Zi /YX /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D\
 "DEBUG" /FR$(INTDIR)/ /Fp$(OUTDIR)/"DSSTREAM.pch" /Fo$(INTDIR)/\
 /Fd$(OUTDIR)/"DSSTREAM.pdb" /c 
CPP_OBJS=.\debug/
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
RSC_PROJ=/l 0x409 /fo$(INTDIR)/"DSStream.res" /d "_DEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o$(OUTDIR)/"DSSTREAM.bsc" 
BSC32_SBRS= \
	$(INTDIR)/DEBUG.SBR \
	$(INTDIR)/DSStream.sbr \
	$(INTDIR)/dstrtime.sbr \
	$(INTDIR)/DSTRWAVE.SBR \
	$(INTDIR)/WASSERT.SBR \
	$(INTDIR)/dstrenum.sbr

$(OUTDIR)/DSSTREAM.bsc : $(OUTDIR)  $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /NOLOGO /SUBSYSTEM:windows /DEBUG /MACHINE:I386
# ADD LINK32 comctl32.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib shell32.lib uuid.lib winmm.lib dsound.lib /NOLOGO /SUBSYSTEM:windows /DEBUG /MACHINE:I386
# SUBTRACT LINK32 /PROFILE /MAP
LINK32_FLAGS=comctl32.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib\
 shell32.lib uuid.lib winmm.lib dsound.lib /NOLOGO /SUBSYSTEM:windows\
 /INCREMENTAL:yes /PDB:$(OUTDIR)/"DSSTREAM.pdb" /DEBUG /MACHINE:I386\
 /DEF:".\DSStream.def" /OUT:$(OUTDIR)/"DSSTREAM.exe" 
DEF_FILE=.\DSStream.def
LINK32_OBJS= \
	$(INTDIR)/DSStream.res \
	$(INTDIR)/DEBUG.OBJ \
	$(INTDIR)/DSStream.obj \
	$(INTDIR)/dstrtime.obj \
	$(INTDIR)/DSTRWAVE.OBJ \
	$(INTDIR)/WASSERT.OBJ \
	$(INTDIR)/dstrenum.obj

$(OUTDIR)/DSSTREAM.exe : $(OUTDIR)  $(DEF_FILE) $(LINK32_OBJS)
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

SOURCE=.\DSStream.rc
DEP_DSSTR=\
	.\icon2.ico\
	.\icon3.ico

$(INTDIR)/DSStream.res :  $(SOURCE)  $(DEP_DSSTR) $(INTDIR)
   $(RSC) $(RSC_PROJ)  $(SOURCE) 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\DEBUG.C
DEP_DEBUG=\
	.\DEBUG.H

$(INTDIR)/DEBUG.OBJ :  $(SOURCE)  $(DEP_DEBUG) $(INTDIR)

# End Source File
################################################################################
# Begin Source File

SOURCE=.\DSStream.c
DEP_DSSTRE=\
	.\DSStream.h\
	\mhtn\debug\inc\DSOUND.H\
	.\DEBUG.H

$(INTDIR)/DSStream.obj :  $(SOURCE)  $(DEP_DSSTRE) $(INTDIR)

# End Source File
################################################################################
# Begin Source File

SOURCE=.\DSStream.def
# End Source File
################################################################################
# Begin Source File

SOURCE=.\dstrtime.c
DEP_DSTRT=\
	.\DSStream.h\
	\mhtn\debug\inc\DSOUND.H\
	.\DEBUG.H

$(INTDIR)/dstrtime.obj :  $(SOURCE)  $(DEP_DSTRT) $(INTDIR)

# End Source File
################################################################################
# Begin Source File

SOURCE=.\DSTRWAVE.C
DEP_DSTRW=\
	.\DSStream.h\
	.\WASSERT.H\
	\mhtn\debug\inc\DSOUND.H\
	.\DEBUG.H

$(INTDIR)/DSTRWAVE.OBJ :  $(SOURCE)  $(DEP_DSTRW) $(INTDIR)

# End Source File
################################################################################
# Begin Source File

SOURCE=.\WASSERT.C
DEP_WASSE=\
	.\WASSERT.H

$(INTDIR)/WASSERT.OBJ :  $(SOURCE)  $(DEP_WASSE) $(INTDIR)

# End Source File
################################################################################
# Begin Source File

SOURCE=.\dstrenum.c
DEP_DSTRE=\
	.\DSStream.h\
	\mhtn\debug\inc\DSOUND.H\
	.\DEBUG.H

$(INTDIR)/dstrenum.obj :  $(SOURCE)  $(DEP_DSTRE) $(INTDIR)

# End Source File
# End Group
# End Project
################################################################################
