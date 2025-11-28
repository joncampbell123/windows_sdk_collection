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
!MESSAGE NMAKE /f "mc.mak" CFG="Win32 (80x86) Debug"
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

ALL : $(OUTDIR)/mc.exe $(OUTDIR)/mc.bsc

$(OUTDIR) : 
    if not exist $(OUTDIR)/nul mkdir $(OUTDIR)

CPP=cl.exe
# ADD BASE CPP /nologo /W3 /GX /YX"mc.h" /O2 /D "NDEBUG" /D "_CONSOLE" /FR /c
# ADD CPP /nologo /W3 /GX /YX"mc.h" /O2 /D "NDEBUG" /D "_CONSOLE" /D "WIN32" /FR /c
CPP_PROJ=/nologo /W3 /GX /YX"mc.h" /O2 /D "NDEBUG" /D "_CONSOLE" /D "WIN32"\
 /FR$(INTDIR)/ /Fp$(OUTDIR)/"mc.pch" /Fo$(INTDIR)/ /c 
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
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# SUBTRACT BASE BSC32 /Iu
# ADD BSC32 /nologo
# SUBTRACT BSC32 /Iu
BSC32_FLAGS=/nologo /o$(OUTDIR)/"mc.bsc" 
BSC32_SBRS= \
	$(INTDIR)/mc.sbr \
	$(INTDIR)/mcout.sbr \
	$(INTDIR)/mclex.sbr \
	$(INTDIR)/mcutil.sbr \
	$(INTDIR)/mcparse.sbr

$(OUTDIR)/mc.bsc : $(OUTDIR)  $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib /NOLOGO /SUBSYSTEM:console /MACHINE:I386
# ADD LINK32 user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib /NOLOGO /SUBSYSTEM:console /MACHINE:I386
LINK32_FLAGS=user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib\
 /NOLOGO /SUBSYSTEM:console /INCREMENTAL:no /PDB:$(OUTDIR)/"mc.pdb"\
 /MACHINE:I386 /OUT:$(OUTDIR)/"mc.exe" 
DEF_FILE=
LINK32_OBJS= \
	$(INTDIR)/mc.obj \
	$(INTDIR)/mcout.obj \
	$(INTDIR)/mclex.obj \
	$(INTDIR)/mcutil.obj \
	$(INTDIR)/mcparse.obj

$(OUTDIR)/mc.exe : $(OUTDIR)  $(DEF_FILE) $(LINK32_OBJS)
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

ALL : $(OUTDIR)/mc.exe $(OUTDIR)/mc.bsc

$(OUTDIR) : 
    if not exist $(OUTDIR)/nul mkdir $(OUTDIR)

CPP=cl.exe
# ADD BASE CPP /nologo /W3 /GX /Zi /YX"mc.h" /Od /D "_DEBUG" /D "_CONSOLE" /FR /c
# ADD CPP /nologo /W3 /GX /Zi /YX"mc.h" /Od /D "_DEBUG" /D "_CONSOLE" /D "WIN32" /FR /c
CPP_PROJ=/nologo /W3 /GX /Zi /YX"mc.h" /Od /D "_DEBUG" /D "_CONSOLE" /D "WIN32"\
 /FR$(INTDIR)/ /Fp$(OUTDIR)/"mc.pch" /Fo$(INTDIR)/ /Fd$(OUTDIR)/"mc.pdb" /c 
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
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# SUBTRACT BASE BSC32 /Iu
# ADD BSC32 /nologo
# SUBTRACT BSC32 /Iu
BSC32_FLAGS=/nologo /o$(OUTDIR)/"mc.bsc" 
BSC32_SBRS= \
	$(INTDIR)/mc.sbr \
	$(INTDIR)/mcout.sbr \
	$(INTDIR)/mclex.sbr \
	$(INTDIR)/mcutil.sbr \
	$(INTDIR)/mcparse.sbr

$(OUTDIR)/mc.bsc : $(OUTDIR)  $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib /NOLOGO /SUBSYSTEM:console /DEBUG /MACHINE:I386
# ADD LINK32 user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib /NOLOGO /SUBSYSTEM:console /DEBUG /MACHINE:I386
LINK32_FLAGS=user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib\
 /NOLOGO /SUBSYSTEM:console /INCREMENTAL:yes /PDB:$(OUTDIR)/"mc.pdb" /DEBUG\
 /MACHINE:I386 /OUT:$(OUTDIR)/"mc.exe" 
DEF_FILE=
LINK32_OBJS= \
	$(INTDIR)/mc.obj \
	$(INTDIR)/mcout.obj \
	$(INTDIR)/mclex.obj \
	$(INTDIR)/mcutil.obj \
	$(INTDIR)/mcparse.obj

$(OUTDIR)/mc.exe : $(OUTDIR)  $(DEF_FILE) $(LINK32_OBJS)
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

ALL : $(OUTDIR)/mc.exe $(OUTDIR)/mc.bsc

$(OUTDIR) : 
    if not exist $(OUTDIR)/nul mkdir $(OUTDIR)

CPP=cl.exe
# ADD BASE CPP /nologo /Gt0 /QMOb2000 /W3 /GX /YX"mc.h" /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /FR /c
# ADD CPP /nologo /Gt0 /QMOb2000 /W3 /GX /YX"mc.h" /O2 /D "NDEBUG" /D "_CONSOLE" /D "WIN32" /FR /c
CPP_PROJ=/nologo /Gt0 /QMOb2000 /W3 /GX /YX"mc.h" /O2 /D "NDEBUG" /D "_CONSOLE"\
 /D "WIN32" /FR$(INTDIR)/ /Fp$(OUTDIR)/"mc.pch" /Fo$(INTDIR)/ /c 
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
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# SUBTRACT BASE BSC32 /Iu
# ADD BSC32 /nologo
# SUBTRACT BSC32 /Iu
BSC32_FLAGS=/nologo /o$(OUTDIR)/"mc.bsc" 
BSC32_SBRS= \
	$(INTDIR)/mc.sbr \
	$(INTDIR)/mcout.sbr \
	$(INTDIR)/mclex.sbr \
	$(INTDIR)/mcutil.sbr \
	$(INTDIR)/mcparse.sbr

$(OUTDIR)/mc.bsc : $(OUTDIR)  $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /NOLOGO /SUBSYSTEM:console /MACHINE:MIPS
# ADD LINK32 user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib /NOLOGO /SUBSYSTEM:console /MACHINE:MIPS
LINK32_FLAGS=user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib\
 /NOLOGO /SUBSYSTEM:console /PDB:$(OUTDIR)/"mc.pdb" /MACHINE:MIPS\
 /OUT:$(OUTDIR)/"mc.exe" 
DEF_FILE=
LINK32_OBJS= \
	$(INTDIR)/mc.obj \
	$(INTDIR)/mcout.obj \
	$(INTDIR)/mclex.obj \
	$(INTDIR)/mcutil.obj \
	$(INTDIR)/mcparse.obj

$(OUTDIR)/mc.exe : $(OUTDIR)  $(DEF_FILE) $(LINK32_OBJS)
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

ALL : $(OUTDIR)/mc.exe $(OUTDIR)/mc.bsc

$(OUTDIR) : 
    if not exist $(OUTDIR)/nul mkdir $(OUTDIR)

CPP=cl.exe
# ADD BASE CPP /nologo /Gt0 /QMOb2000 /W3 /GX /Zi /YX"mc.h" /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /FR /c
# ADD CPP /nologo /Gt0 /QMOb2000 /W3 /GX /Zi /YX"mc.h" /Od /D "_DEBUG" /D "_CONSOLE" /D "WIN32" /FR /c
CPP_PROJ=/nologo /Gt0 /QMOb2000 /W3 /GX /Zi /YX"mc.h" /Od /D "_DEBUG" /D\
 "_CONSOLE" /D "WIN32" /FR$(INTDIR)/ /Fp$(OUTDIR)/"mc.pch" /Fo$(INTDIR)/\
 /Fd$(OUTDIR)/"mc.pdb" /c 
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
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# SUBTRACT BASE BSC32 /Iu
# ADD BSC32 /nologo
# SUBTRACT BSC32 /Iu
BSC32_FLAGS=/nologo /o$(OUTDIR)/"mc.bsc" 
BSC32_SBRS= \
	$(INTDIR)/mc.sbr \
	$(INTDIR)/mcout.sbr \
	$(INTDIR)/mclex.sbr \
	$(INTDIR)/mcutil.sbr \
	$(INTDIR)/mcparse.sbr

$(OUTDIR)/mc.bsc : $(OUTDIR)  $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /NOLOGO /SUBSYSTEM:console /DEBUG /MACHINE:MIPS
# ADD LINK32 user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib /NOLOGO /SUBSYSTEM:console /DEBUG /MACHINE:MIPS
LINK32_FLAGS=user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib\
 /NOLOGO /SUBSYSTEM:console /PDB:$(OUTDIR)/"mc.pdb" /DEBUG /MACHINE:MIPS\
 /OUT:$(OUTDIR)/"mc.exe" 
DEF_FILE=
LINK32_OBJS= \
	$(INTDIR)/mc.obj \
	$(INTDIR)/mcout.obj \
	$(INTDIR)/mclex.obj \
	$(INTDIR)/mcutil.obj \
	$(INTDIR)/mcparse.obj

$(OUTDIR)/mc.exe : $(OUTDIR)  $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 

################################################################################
# Begin Group "Source Files"

################################################################################
# Begin Source File

SOURCE=.\mc.c
DEP_MC_C0=\
	.\mc.h\
	.\SYS\TYPES.H\
	.\SYS\STAT.H

!IF  "$(CFG)" == "Win32 (80x86) Release"

$(INTDIR)/mc.obj :  $(SOURCE)  $(DEP_MC_C0) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

$(INTDIR)/mc.obj :  $(SOURCE)  $(DEP_MC_C0) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

$(INTDIR)/mc.obj :  $(SOURCE)  $(DEP_MC_C0) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

$(INTDIR)/mc.obj :  $(SOURCE)  $(DEP_MC_C0) $(INTDIR)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\mcout.c
DEP_MCOUT=\
	.\mc.h\
	.\SYS\TYPES.H\
	.\SYS\STAT.H

!IF  "$(CFG)" == "Win32 (80x86) Release"

$(INTDIR)/mcout.obj :  $(SOURCE)  $(DEP_MCOUT) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

$(INTDIR)/mcout.obj :  $(SOURCE)  $(DEP_MCOUT) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

$(INTDIR)/mcout.obj :  $(SOURCE)  $(DEP_MCOUT) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

$(INTDIR)/mcout.obj :  $(SOURCE)  $(DEP_MCOUT) $(INTDIR)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\mclex.c
DEP_MCLEX=\
	.\mc.h\
	.\SYS\TYPES.H\
	.\SYS\STAT.H

!IF  "$(CFG)" == "Win32 (80x86) Release"

$(INTDIR)/mclex.obj :  $(SOURCE)  $(DEP_MCLEX) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

$(INTDIR)/mclex.obj :  $(SOURCE)  $(DEP_MCLEX) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

$(INTDIR)/mclex.obj :  $(SOURCE)  $(DEP_MCLEX) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

$(INTDIR)/mclex.obj :  $(SOURCE)  $(DEP_MCLEX) $(INTDIR)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\mcutil.c
DEP_MCUTI=\
	.\mc.h\
	.\SYS\TYPES.H\
	.\SYS\STAT.H

!IF  "$(CFG)" == "Win32 (80x86) Release"

$(INTDIR)/mcutil.obj :  $(SOURCE)  $(DEP_MCUTI) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

$(INTDIR)/mcutil.obj :  $(SOURCE)  $(DEP_MCUTI) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

$(INTDIR)/mcutil.obj :  $(SOURCE)  $(DEP_MCUTI) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

$(INTDIR)/mcutil.obj :  $(SOURCE)  $(DEP_MCUTI) $(INTDIR)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\mcparse.c
DEP_MCPAR=\
	.\mc.h\
	.\SYS\TYPES.H\
	.\SYS\STAT.H

!IF  "$(CFG)" == "Win32 (80x86) Release"

$(INTDIR)/mcparse.obj :  $(SOURCE)  $(DEP_MCPAR) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

$(INTDIR)/mcparse.obj :  $(SOURCE)  $(DEP_MCPAR) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

$(INTDIR)/mcparse.obj :  $(SOURCE)  $(DEP_MCPAR) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

$(INTDIR)/mcparse.obj :  $(SOURCE)  $(DEP_MCPAR) $(INTDIR)

!ENDIF 

# End Source File
# End Group
# End Project
################################################################################
