# Microsoft Visual C++ Generated NMAKE File, Format Version 2.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (MIPS) Console Application" 0x0503
# TARGTYPE "Win32 (x86) Console Application" 0x0103

!IF "$(CFG)" == ""
CFG=Win32 (80x86) Debug
!MESSAGE No configuration specified.  Defaulting to Win32 (80x86) Debug.
!ENDIF 

!IF "$(CFG)" != "Win32 (80x86) Release" && "$(CFG)" != "Win32 (80x86) Debug" &&\
 "$(CFG)" != "Win32 (MIPS) Release" && "$(CFG)" != "Win32 (MIPS) Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "mcitest.mak" CFG="Win32 (80x86) Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Win32 (80x86) Release" (based on "Win32 (x86) Console Application")
!MESSAGE "Win32 (80x86) Debug" (based on "Win32 (x86) Console Application")
!MESSAGE "Win32 (MIPS) Release" (based on "Win32 (MIPS) Console Application")
!MESSAGE "Win32 (MIPS) Debug" (based on "Win32 (MIPS) Console Application")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

################################################################################
# Begin Project
# PROP Target_Last_Scanned "Win32 (MIPS) Release"

!IF  "$(CFG)" == "Win32 (80x86) Release"

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

ALL : .\WinRel\mcitest.exe .\WinRel\mcitest.bsc

$(OUTDIR) : 
    if not exist $(OUTDIR)/nul mkdir $(OUTDIR)

CPP=cl.exe
# ADD BASE CPP /nologo /W3 /YX /O2 /D "NDEBUG" /D "_CONSOLE" /FR /c
# ADD CPP /nologo /Gz /W3 /YX /O2 /D "NDEBUG" /D "_CONSOLE" /D "WIN32" /FR /c
CPP_PROJ=/nologo /Gz /W3 /YX /O2 /D "NDEBUG" /D "_CONSOLE" /D "WIN32"\
 /FR$(INTDIR)/ /Fp$(OUTDIR)/"mcitest.pch" /Fo$(INTDIR)/ /c 
CPP_OBJS=.\WinRel/

.c{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

RSC=rc.exe
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
RSC_PROJ=/l 0x409 /fo$(INTDIR)/"mcitest.res" /d "NDEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# SUBTRACT BASE BSC32 /Iu
# ADD BSC32 /nologo
# SUBTRACT BSC32 /Iu
BSC32_FLAGS=/nologo /o$(OUTDIR)/"mcitest.bsc" 
BSC32_SBRS= \
	.\WinRel\mcitest.sbr \
	.\WinRel\debug.sbr \
	.\WinRel\fileopen.sbr \
	.\WinRel\edit.sbr

.\WinRel\mcitest.bsc : $(OUTDIR)  $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib compob32.lib storag32.lib ole2w32.lib ole232.lib ole2di32.lib /NOLOGO /SUBSYSTEM:console /MACHINE:I386
# ADD LINK32 winmm.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib uuid.lib /NOLOGO /SUBSYSTEM:console /MACHINE:I386
LINK32_FLAGS=winmm.lib kernel32.lib user32.lib gdi32.lib winspool.lib\
 comdlg32.lib advapi32.lib shell32.lib uuid.lib /NOLOGO /SUBSYSTEM:console\
 /INCREMENTAL:no /PDB:$(OUTDIR)/"mcitest.pdb" /MACHINE:I386\
 /OUT:$(OUTDIR)/"mcitest.exe" 
DEF_FILE=
LINK32_OBJS= \
	.\WinRel\mcitest.obj \
	.\WinRel\debug.obj \
	.\WinRel\mcitest.res \
	.\WinRel\fileopen.obj \
	.\WinRel\edit.obj

.\WinRel\mcitest.exe : $(OUTDIR)  $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

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

ALL : .\WinDebug\mcitest.exe .\WinDebug\mcitest.bsc

$(OUTDIR) : 
    if not exist $(OUTDIR)/nul mkdir $(OUTDIR)

CPP=cl.exe
# ADD BASE CPP /nologo /W3 /Zi /YX /Od /D "_DEBUG" /D "_CONSOLE" /FR /c
# ADD CPP /nologo /Gz /W3 /Zi /YX /Od /D "_DEBUG" /D "_CONSOLE" /D "WIN32" /FR /c
CPP_PROJ=/nologo /Gz /W3 /Zi /YX /Od /D "_DEBUG" /D "_CONSOLE" /D "WIN32"\
 /FR$(INTDIR)/ /Fp$(OUTDIR)/"mcitest.pch" /Fo$(INTDIR)/\
 /Fd$(OUTDIR)/"mcitest.pdb" /c 
CPP_OBJS=.\WinDebug/

.c{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

RSC=rc.exe
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
RSC_PROJ=/l 0x409 /fo$(INTDIR)/"mcitest.res" /d "_DEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# SUBTRACT BASE BSC32 /Iu
# ADD BSC32 /nologo
# SUBTRACT BSC32 /Iu
BSC32_FLAGS=/nologo /o$(OUTDIR)/"mcitest.bsc" 
BSC32_SBRS= \
	.\WinDebug\mcitest.sbr \
	.\WinDebug\debug.sbr \
	.\WinDebug\fileopen.sbr \
	.\WinDebug\edit.sbr

.\WinDebug\mcitest.bsc : $(OUTDIR)  $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib compob32.lib storag32.lib ole2w32.lib ole232.lib ole2di32.lib /NOLOGO /SUBSYSTEM:console /DEBUG /MACHINE:I386
# ADD LINK32 winmm.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib uuid.lib /NOLOGO /SUBSYSTEM:console /DEBUG /MACHINE:I386
LINK32_FLAGS=winmm.lib kernel32.lib user32.lib gdi32.lib winspool.lib\
 comdlg32.lib advapi32.lib shell32.lib uuid.lib /NOLOGO /SUBSYSTEM:console\
 /INCREMENTAL:yes /PDB:$(OUTDIR)/"mcitest.pdb" /DEBUG /MACHINE:I386\
 /OUT:$(OUTDIR)/"mcitest.exe" 
DEF_FILE=
LINK32_OBJS= \
	.\WinDebug\mcitest.obj \
	.\WinDebug\debug.obj \
	.\WinDebug\mcitest.res \
	.\WinDebug\fileopen.obj \
	.\WinDebug\edit.obj

.\WinDebug\mcitest.exe : $(OUTDIR)  $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Win32__M"
# PROP BASE Intermediate_Dir "Win32__M"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "WinRel"
# PROP Intermediate_Dir "WinRel"
OUTDIR=.\WinRel
INTDIR=.\WinRel

ALL : .\WinRel\mcitest.exe .\WinRel\mcitest.bsc

$(OUTDIR) : 
    if not exist $(OUTDIR)/nul mkdir $(OUTDIR)

CPP=cl.exe
# ADD BASE CPP /nologo /Gt0 /QMOb2000 /W3 /GX /YX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /FR /c
# ADD CPP /nologo /Gt0 /QMOb2000 /W3 /GX /YX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /FR /c
CPP_PROJ=/nologo /Gt0 /QMOb2000 /W3 /GX /YX /O2 /D "WIN32" /D "NDEBUG" /D\
 "_CONSOLE" /FR$(INTDIR)/ /Fp$(OUTDIR)/"mcitest.pch" /Fo$(INTDIR)/ /c 
CPP_OBJS=.\WinRel/

.c{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

RSC=rc.exe
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
RSC_PROJ=/l 0x409 /fo$(INTDIR)/"mcitest.res" /d "NDEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# SUBTRACT BASE BSC32 /Iu
# ADD BSC32 /nologo
# SUBTRACT BSC32 /Iu
BSC32_FLAGS=/nologo /o$(OUTDIR)/"mcitest.bsc" 
BSC32_SBRS= \
	.\WinRel\mcitest.sbr \
	.\WinRel\debug.sbr \
	.\WinRel\fileopen.sbr \
	.\WinRel\edit.sbr

.\WinRel\mcitest.bsc : $(OUTDIR)  $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /NOLOGO /SUBSYSTEM:console /MACHINE:MIPS
# ADD LINK32 winmm.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib uuid.lib /NOLOGO /SUBSYSTEM:console /MACHINE:MIPS
LINK32_FLAGS=winmm.lib kernel32.lib user32.lib gdi32.lib winspool.lib\
 comdlg32.lib advapi32.lib shell32.lib uuid.lib /NOLOGO /SUBSYSTEM:console\
 /PDB:$(OUTDIR)/"mcitest.pdb" /MACHINE:MIPS /OUT:$(OUTDIR)/"mcitest.exe" 
DEF_FILE=
LINK32_OBJS= \
	.\WinRel\mcitest.obj \
	.\WinRel\debug.obj \
	.\WinRel\mcitest.res \
	.\WinRel\fileopen.obj \
	.\WinRel\edit.obj

.\WinRel\mcitest.exe : $(OUTDIR)  $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Win32__0"
# PROP BASE Intermediate_Dir "Win32__0"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "WinDebug"
# PROP Intermediate_Dir "WinDebug"
OUTDIR=.\WinDebug
INTDIR=.\WinDebug

ALL : .\WinDebug\mcitest.exe .\WinDebug\mcitest.bsc

$(OUTDIR) : 
    if not exist $(OUTDIR)/nul mkdir $(OUTDIR)

CPP=cl.exe
# ADD BASE CPP /nologo /Gt0 /QMOb2000 /W3 /GX /Zi /YX /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /FR /c
# ADD CPP /nologo /Gt0 /QMOb2000 /W3 /GX /Zi /YX /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /FR /c
CPP_PROJ=/nologo /Gt0 /QMOb2000 /W3 /GX /Zi /YX /Od /D "WIN32" /D "_DEBUG" /D\
 "_CONSOLE" /FR$(INTDIR)/ /Fp$(OUTDIR)/"mcitest.pch" /Fo$(INTDIR)/\
 /Fd$(OUTDIR)/"mcitest.pdb" /c 
CPP_OBJS=.\WinDebug/

.c{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

RSC=rc.exe
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
RSC_PROJ=/l 0x409 /fo$(INTDIR)/"mcitest.res" /d "_DEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# SUBTRACT BASE BSC32 /Iu
# ADD BSC32 /nologo
# SUBTRACT BSC32 /Iu
BSC32_FLAGS=/nologo /o$(OUTDIR)/"mcitest.bsc" 
BSC32_SBRS= \
	.\WinDebug\mcitest.sbr \
	.\WinDebug\debug.sbr \
	.\WinDebug\fileopen.sbr \
	.\WinDebug\edit.sbr

.\WinDebug\mcitest.bsc : $(OUTDIR)  $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /NOLOGO /SUBSYSTEM:console /DEBUG /MACHINE:MIPS
# ADD LINK32 winmm.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib uuid.lib /NOLOGO /SUBSYSTEM:console /DEBUG /MACHINE:MIPS
LINK32_FLAGS=winmm.lib kernel32.lib user32.lib gdi32.lib winspool.lib\
 comdlg32.lib advapi32.lib shell32.lib uuid.lib /NOLOGO /SUBSYSTEM:console\
 /PDB:$(OUTDIR)/"mcitest.pdb" /DEBUG /MACHINE:MIPS /OUT:$(OUTDIR)/"mcitest.exe" 
DEF_FILE=
LINK32_OBJS= \
	.\WinDebug\mcitest.obj \
	.\WinDebug\debug.obj \
	.\WinDebug\mcitest.res \
	.\WinDebug\fileopen.obj \
	.\WinDebug\edit.obj

.\WinDebug\mcitest.exe : $(OUTDIR)  $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 

################################################################################
# Begin Group "Source Files"

################################################################################
# Begin Source File

SOURCE=.\mcitest.c
DEP_MCITE=\
	.\mcitest.h\
	.\mcimain.h\
	.\edit.h

!IF  "$(CFG)" == "Win32 (80x86) Release"

.\WinRel\mcitest.obj :  $(SOURCE)  $(DEP_MCITE) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

.\WinDebug\mcitest.obj :  $(SOURCE)  $(DEP_MCITE) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

.\WinRel\mcitest.obj :  $(SOURCE)  $(DEP_MCITE) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

.\WinDebug\mcitest.obj :  $(SOURCE)  $(DEP_MCITE) $(INTDIR)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\debug.c
DEP_DEBUG=\
	.\mcitest.h\
	.\mcimain.h

!IF  "$(CFG)" == "Win32 (80x86) Release"

.\WinRel\debug.obj :  $(SOURCE)  $(DEP_DEBUG) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

.\WinDebug\debug.obj :  $(SOURCE)  $(DEP_DEBUG) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

.\WinRel\debug.obj :  $(SOURCE)  $(DEP_DEBUG) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

.\WinDebug\debug.obj :  $(SOURCE)  $(DEP_DEBUG) $(INTDIR)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\mcitest.rc

!IF  "$(CFG)" == "Win32 (80x86) Release"

.\WinRel\mcitest.res :  $(SOURCE)  $(INTDIR)
   $(RSC) $(RSC_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

.\WinDebug\mcitest.res :  $(SOURCE)  $(INTDIR)
   $(RSC) $(RSC_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

.\WinRel\mcitest.res :  $(SOURCE)  $(INTDIR)
   $(RSC) $(RSC_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

.\WinDebug\mcitest.res :  $(SOURCE)  $(INTDIR)
   $(RSC) $(RSC_PROJ)  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\fileopen.c
DEP_FILEO=\
	.\mcitest.h\
	.\mcimain.h

!IF  "$(CFG)" == "Win32 (80x86) Release"

.\WinRel\fileopen.obj :  $(SOURCE)  $(DEP_FILEO) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

.\WinDebug\fileopen.obj :  $(SOURCE)  $(DEP_FILEO) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

.\WinRel\fileopen.obj :  $(SOURCE)  $(DEP_FILEO) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

.\WinDebug\fileopen.obj :  $(SOURCE)  $(DEP_FILEO) $(INTDIR)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\edit.c
DEP_EDIT_=\
	.\gmem.h\
	.\edit.h\
	.\mcitest.h\
	.\mcimain.h

!IF  "$(CFG)" == "Win32 (80x86) Release"

.\WinRel\edit.obj :  $(SOURCE)  $(DEP_EDIT_) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

.\WinDebug\edit.obj :  $(SOURCE)  $(DEP_EDIT_) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

.\WinRel\edit.obj :  $(SOURCE)  $(DEP_EDIT_) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

.\WinDebug\edit.obj :  $(SOURCE)  $(DEP_EDIT_) $(INTDIR)

!ENDIF 

# End Source File
# End Group
# End Project
################################################################################
