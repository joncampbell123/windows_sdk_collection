# Microsoft Visual C++ Generated NMAKE File, Format Version 2.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (MIPS) Console Application" 0x0503
# TARGTYPE "Win32 (x86) Console Application" 0x0103

!IF "$(CFG)" == ""
CFG=Win32 (80x86) Debug
!MESSAGE No configuration specified.  Defaulting to Win32 (80x86) Debug.
!ENDIF 

!IF "$(CFG)" != "Win32 (80x86) Release" && "$(CFG)" != "Win32 (80x86) Debug" &&\
 "$(CFG)" != "Win32 (MIPS) Debug" && "$(CFG)" != "Win32 (MIPS) Release"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "playsnd.mak" CFG="Win32 (80x86) Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Win32 (80x86) Release" (based on "Win32 (x86) Console Application")
!MESSAGE "Win32 (80x86) Debug" (based on "Win32 (x86) Console Application")
!MESSAGE "Win32 (MIPS) Debug" (based on "Win32 (MIPS) Console Application")
!MESSAGE "Win32 (MIPS) Release" (based on "Win32 (MIPS) Console Application")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

################################################################################
# Begin Project
# PROP Target_Last_Scanned "Win32 (MIPS) Debug"

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

ALL : $(OUTDIR)/playsnd.exe $(OUTDIR)/playsnd.bsc

$(OUTDIR) : 
    if not exist $(OUTDIR)/nul mkdir $(OUTDIR)

CPP=cl.exe
# ADD BASE CPP /nologo /W3 /YX /O2 /D "NDEBUG" /D "_CONSOLE" /FR /c
# ADD CPP /nologo /W3 /YX /O2 /D "NDEBUG" /D "WIN32" /D "_CONSOLE" /FR /c
CPP_PROJ=/nologo /W3 /YX /O2 /D "NDEBUG" /D "WIN32" /D "_CONSOLE" /FR$(INTDIR)/\
 /Fp$(OUTDIR)/"playsnd.pch" /Fo$(INTDIR)/ /c 
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
RSC_PROJ=/l 0x409 /fo$(INTDIR)/"playsnd.res" /d "NDEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# SUBTRACT BASE BSC32 /Iu
# ADD BSC32 /nologo
# SUBTRACT BSC32 /Iu
BSC32_FLAGS=/nologo /o$(OUTDIR)/"playsnd.bsc" 
BSC32_SBRS= \
	$(INTDIR)/playsnd.sbr \
	$(INTDIR)/sound.sbr \
	$(INTDIR)/about.sbr \
	$(INTDIR)/file.sbr \
	$(INTDIR)/init.sbr \
	$(INTDIR)/res.sbr \
	$(INTDIR)/help.sbr \
	$(INTDIR)/debug.sbr

$(OUTDIR)/playsnd.bsc : $(OUTDIR)  $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib /NOLOGO /SUBSYSTEM:console /MACHINE:I386
# ADD LINK32 user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib winmm.lib /NOLOGO /SUBSYSTEM:console /MACHINE:I386
LINK32_FLAGS=user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib\
 shell32.lib winmm.lib /NOLOGO /SUBSYSTEM:console /INCREMENTAL:no\
 /PDB:$(OUTDIR)/"playsnd.pdb" /MACHINE:I386 /OUT:$(OUTDIR)/"playsnd.exe" 
DEF_FILE=
LINK32_OBJS= \
	$(INTDIR)/playsnd.obj \
	$(INTDIR)/playsnd.res \
	$(INTDIR)/sound.obj \
	$(INTDIR)/about.obj \
	$(INTDIR)/file.obj \
	$(INTDIR)/init.obj \
	$(INTDIR)/res.obj \
	$(INTDIR)/help.obj \
	$(INTDIR)/debug.obj

$(OUTDIR)/playsnd.exe : $(OUTDIR)  $(DEF_FILE) $(LINK32_OBJS)
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

ALL : $(OUTDIR)/playsnd.exe $(OUTDIR)/playsnd.bsc

$(OUTDIR) : 
    if not exist $(OUTDIR)/nul mkdir $(OUTDIR)

CPP=cl.exe
# ADD BASE CPP /nologo /W3 /Zi /YX /Od /D "_DEBUG" /D "_CONSOLE" /FR /c
# ADD CPP /nologo /W3 /Zi /YX /Od /D "_DEBUG" /D "WIN32" /D "_CONSOLE" /FR /c
CPP_PROJ=/nologo /W3 /Zi /YX /Od /D "_DEBUG" /D "WIN32" /D "_CONSOLE"\
 /FR$(INTDIR)/ /Fp$(OUTDIR)/"playsnd.pch" /Fo$(INTDIR)/\
 /Fd$(OUTDIR)/"playsnd.pdb" /c 
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
RSC_PROJ=/l 0x409 /fo$(INTDIR)/"playsnd.res" /d "_DEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# SUBTRACT BASE BSC32 /Iu
# ADD BSC32 /nologo
# SUBTRACT BSC32 /Iu
BSC32_FLAGS=/nologo /o$(OUTDIR)/"playsnd.bsc" 
BSC32_SBRS= \
	$(INTDIR)/playsnd.sbr \
	$(INTDIR)/sound.sbr \
	$(INTDIR)/about.sbr \
	$(INTDIR)/file.sbr \
	$(INTDIR)/init.sbr \
	$(INTDIR)/res.sbr \
	$(INTDIR)/help.sbr \
	$(INTDIR)/debug.sbr

$(OUTDIR)/playsnd.bsc : $(OUTDIR)  $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib /NOLOGO /SUBSYSTEM:console /DEBUG /MACHINE:I386
# ADD LINK32 user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib winmm.lib /NOLOGO /SUBSYSTEM:console /DEBUG /MACHINE:I386
LINK32_FLAGS=user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib\
 shell32.lib winmm.lib /NOLOGO /SUBSYSTEM:console /INCREMENTAL:yes\
 /PDB:$(OUTDIR)/"playsnd.pdb" /DEBUG /MACHINE:I386 /OUT:$(OUTDIR)/"playsnd.exe" 
DEF_FILE=
LINK32_OBJS= \
	$(INTDIR)/playsnd.obj \
	$(INTDIR)/playsnd.res \
	$(INTDIR)/sound.obj \
	$(INTDIR)/about.obj \
	$(INTDIR)/file.obj \
	$(INTDIR)/init.obj \
	$(INTDIR)/res.obj \
	$(INTDIR)/help.obj \
	$(INTDIR)/debug.obj

$(OUTDIR)/playsnd.exe : $(OUTDIR)  $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "WinDebug"
# PROP Intermediate_Dir "WinDebug"
OUTDIR=.\WinDebug
INTDIR=.\WinDebug

ALL : $(OUTDIR)/playsnd.exe $(OUTDIR)/playsnd.bsc

$(OUTDIR) : 
    if not exist $(OUTDIR)/nul mkdir $(OUTDIR)

CPP=cl.exe
# ADD BASE CPP /nologo /Gt0 /QMOb2000 /W3 /Zi /YX /Od /D "_DEBUG" /D "_CONSOLE" /FR /c
# ADD CPP /nologo /Gt0 /QMOb2000 /W3 /Zi /YX /Od /D "_DEBUG" /D "_CONSOLE" /D "WIN32" /FR /c
CPP_PROJ=/nologo /Gt0 /QMOb2000 /W3 /Zi /YX /Od /D "_DEBUG" /D "_CONSOLE" /D\
 "WIN32" /FR$(INTDIR)/ /Fp$(OUTDIR)/"playsnd.pch" /Fo$(INTDIR)/\
 /Fd$(OUTDIR)/"playsnd.pdb" /c 
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
RSC_PROJ=/l 0x409 /fo$(INTDIR)/"playsnd.res" /d "_DEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# SUBTRACT BASE BSC32 /Iu
# ADD BSC32 /nologo
# SUBTRACT BSC32 /Iu
BSC32_FLAGS=/nologo /o$(OUTDIR)/"playsnd.bsc" 
BSC32_SBRS= \
	$(INTDIR)/playsnd.sbr \
	$(INTDIR)/sound.sbr \
	$(INTDIR)/about.sbr \
	$(INTDIR)/file.sbr \
	$(INTDIR)/init.sbr \
	$(INTDIR)/res.sbr \
	$(INTDIR)/help.sbr \
	$(INTDIR)/debug.sbr

$(OUTDIR)/playsnd.bsc : $(OUTDIR)  $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib odbc32.lib oleaut32.lib uuid.lib /NOLOGO /SUBSYSTEM:console /DEBUG /MACHINE:MIPS
# SUBTRACT BASE LINK32 /PDB:none
# ADD LINK32 user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib oleaut32.lib uuid.lib winmm.lib /NOLOGO /SUBSYSTEM:console /DEBUG /MACHINE:MIPS
# SUBTRACT LINK32 /PDB:none
LINK32_FLAGS=user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib\
 shell32.lib oleaut32.lib uuid.lib winmm.lib /NOLOGO /SUBSYSTEM:console\
 /PDB:$(OUTDIR)/"playsnd.pdb" /DEBUG /MACHINE:MIPS /OUT:$(OUTDIR)/"playsnd.exe" 
DEF_FILE=
LINK32_OBJS= \
	$(INTDIR)/playsnd.obj \
	$(INTDIR)/playsnd.res \
	$(INTDIR)/sound.obj \
	$(INTDIR)/about.obj \
	$(INTDIR)/file.obj \
	$(INTDIR)/init.obj \
	$(INTDIR)/res.obj \
	$(INTDIR)/help.obj \
	$(INTDIR)/debug.obj

$(OUTDIR)/playsnd.exe : $(OUTDIR)  $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "WinRel"
# PROP Intermediate_Dir "WinRel"
OUTDIR=.\WinRel
INTDIR=.\WinRel

ALL : $(OUTDIR)/playsnd.exe $(OUTDIR)/playsnd.bsc

$(OUTDIR) : 
    if not exist $(OUTDIR)/nul mkdir $(OUTDIR)

CPP=cl.exe
# ADD BASE CPP /nologo /Gt0 /QMOb2000 /W3 /YX /O2 /D "NDEBUG" /D "_CONSOLE" /FR /c
# ADD CPP /nologo /Gt0 /QMOb2000 /W3 /YX /O2 /D "NDEBUG" /D "_CONSOLE" /D "WIN32" /FR /c
CPP_PROJ=/nologo /Gt0 /QMOb2000 /W3 /YX /O2 /D "NDEBUG" /D "_CONSOLE" /D\
 "WIN32" /FR$(INTDIR)/ /Fp$(OUTDIR)/"playsnd.pch" /Fo$(INTDIR)/ /c 
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
RSC_PROJ=/l 0x409 /fo$(INTDIR)/"playsnd.res" /d "NDEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# SUBTRACT BASE BSC32 /Iu
# ADD BSC32 /nologo
# SUBTRACT BSC32 /Iu
BSC32_FLAGS=/nologo /o$(OUTDIR)/"playsnd.bsc" 
BSC32_SBRS= \
	$(INTDIR)/playsnd.sbr \
	$(INTDIR)/sound.sbr \
	$(INTDIR)/about.sbr \
	$(INTDIR)/file.sbr \
	$(INTDIR)/init.sbr \
	$(INTDIR)/res.sbr \
	$(INTDIR)/help.sbr \
	$(INTDIR)/debug.sbr

$(OUTDIR)/playsnd.bsc : $(OUTDIR)  $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib odbc32.lib oleaut32.lib uuid.lib /NOLOGO /SUBSYSTEM:console /MACHINE:MIPS
# SUBTRACT BASE LINK32 /PDB:none
# ADD LINK32 user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib oleaut32.lib uuid.lib winmm.lib /NOLOGO /SUBSYSTEM:console /MACHINE:MIPS
# SUBTRACT LINK32 /PDB:none
LINK32_FLAGS=user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib\
 shell32.lib oleaut32.lib uuid.lib winmm.lib /NOLOGO /SUBSYSTEM:console\
 /PDB:$(OUTDIR)/"playsnd.pdb" /MACHINE:MIPS /OUT:$(OUTDIR)/"playsnd.exe" 
DEF_FILE=
LINK32_OBJS= \
	$(INTDIR)/playsnd.obj \
	$(INTDIR)/playsnd.res \
	$(INTDIR)/sound.obj \
	$(INTDIR)/about.obj \
	$(INTDIR)/file.obj \
	$(INTDIR)/init.obj \
	$(INTDIR)/res.obj \
	$(INTDIR)/help.obj \
	$(INTDIR)/debug.obj

$(OUTDIR)/playsnd.exe : $(OUTDIR)  $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 

################################################################################
# Begin Group "Source Files"

################################################################################
# Begin Source File

SOURCE=.\playsnd.c
DEP_PLAYS=\
	.\playsnd.h\
	.\sounddlg.h

!IF  "$(CFG)" == "Win32 (80x86) Release"

$(INTDIR)/playsnd.obj :  $(SOURCE)  $(DEP_PLAYS) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

$(INTDIR)/playsnd.obj :  $(SOURCE)  $(DEP_PLAYS) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

$(INTDIR)/playsnd.obj :  $(SOURCE)  $(DEP_PLAYS) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

$(INTDIR)/playsnd.obj :  $(SOURCE)  $(DEP_PLAYS) $(INTDIR)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\playsnd.rc

!IF  "$(CFG)" == "Win32 (80x86) Release"

$(INTDIR)/playsnd.res :  $(SOURCE)  $(INTDIR)
   $(RSC) $(RSC_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

$(INTDIR)/playsnd.res :  $(SOURCE)  $(INTDIR)
   $(RSC) $(RSC_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

$(INTDIR)/playsnd.res :  $(SOURCE)  $(INTDIR)
   $(RSC) $(RSC_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

$(INTDIR)/playsnd.res :  $(SOURCE)  $(INTDIR)
   $(RSC) $(RSC_PROJ)  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\sound.c
DEP_SOUND=\
	.\playsnd.h\
	.\sounddlg.h

!IF  "$(CFG)" == "Win32 (80x86) Release"

$(INTDIR)/sound.obj :  $(SOURCE)  $(DEP_SOUND) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

$(INTDIR)/sound.obj :  $(SOURCE)  $(DEP_SOUND) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

$(INTDIR)/sound.obj :  $(SOURCE)  $(DEP_SOUND) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

$(INTDIR)/sound.obj :  $(SOURCE)  $(DEP_SOUND) $(INTDIR)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\about.c
DEP_ABOUT=\
	.\playsnd.h\
	.\sounddlg.h

!IF  "$(CFG)" == "Win32 (80x86) Release"

$(INTDIR)/about.obj :  $(SOURCE)  $(DEP_ABOUT) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

$(INTDIR)/about.obj :  $(SOURCE)  $(DEP_ABOUT) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

$(INTDIR)/about.obj :  $(SOURCE)  $(DEP_ABOUT) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

$(INTDIR)/about.obj :  $(SOURCE)  $(DEP_ABOUT) $(INTDIR)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\file.c
DEP_FILE_=\
	.\playsnd.h\
	.\sounddlg.h

!IF  "$(CFG)" == "Win32 (80x86) Release"

$(INTDIR)/file.obj :  $(SOURCE)  $(DEP_FILE_) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

$(INTDIR)/file.obj :  $(SOURCE)  $(DEP_FILE_) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

$(INTDIR)/file.obj :  $(SOURCE)  $(DEP_FILE_) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

$(INTDIR)/file.obj :  $(SOURCE)  $(DEP_FILE_) $(INTDIR)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\init.c
DEP_INIT_=\
	.\playsnd.h\
	.\sounddlg.h

!IF  "$(CFG)" == "Win32 (80x86) Release"

$(INTDIR)/init.obj :  $(SOURCE)  $(DEP_INIT_) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

$(INTDIR)/init.obj :  $(SOURCE)  $(DEP_INIT_) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

$(INTDIR)/init.obj :  $(SOURCE)  $(DEP_INIT_) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

$(INTDIR)/init.obj :  $(SOURCE)  $(DEP_INIT_) $(INTDIR)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\res.c
DEP_RES_C=\
	.\playsnd.h\
	.\sounddlg.h

!IF  "$(CFG)" == "Win32 (80x86) Release"

$(INTDIR)/res.obj :  $(SOURCE)  $(DEP_RES_C) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

$(INTDIR)/res.obj :  $(SOURCE)  $(DEP_RES_C) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

$(INTDIR)/res.obj :  $(SOURCE)  $(DEP_RES_C) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

$(INTDIR)/res.obj :  $(SOURCE)  $(DEP_RES_C) $(INTDIR)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\help.c
DEP_HELP_=\
	.\playsnd.h\
	.\sounddlg.h

!IF  "$(CFG)" == "Win32 (80x86) Release"

$(INTDIR)/help.obj :  $(SOURCE)  $(DEP_HELP_) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

$(INTDIR)/help.obj :  $(SOURCE)  $(DEP_HELP_) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

$(INTDIR)/help.obj :  $(SOURCE)  $(DEP_HELP_) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

$(INTDIR)/help.obj :  $(SOURCE)  $(DEP_HELP_) $(INTDIR)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\debug.c
DEP_DEBUG=\
	.\playsnd.h\
	.\sounddlg.h

!IF  "$(CFG)" == "Win32 (80x86) Release"

$(INTDIR)/debug.obj :  $(SOURCE)  $(DEP_DEBUG) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

$(INTDIR)/debug.obj :  $(SOURCE)  $(DEP_DEBUG) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

$(INTDIR)/debug.obj :  $(SOURCE)  $(DEP_DEBUG) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

$(INTDIR)/debug.obj :  $(SOURCE)  $(DEP_DEBUG) $(INTDIR)

!ENDIF 

# End Source File
# End Group
# End Project
################################################################################
