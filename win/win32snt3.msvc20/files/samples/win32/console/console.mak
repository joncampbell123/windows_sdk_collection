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
!MESSAGE NMAKE /f "console.mak" CFG="Win32 (80x86) Debug"
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

ALL : $(OUTDIR)/console.exe $(OUTDIR)/console.bsc

$(OUTDIR) : 
    if not exist $(OUTDIR)/nul mkdir $(OUTDIR)

CPP=cl.exe
# ADD BASE CPP /nologo /W3 /YX /O2 /D "NDEBUG" /D "_CONSOLE" /FR /c
# ADD CPP /nologo /W3 /YX /O2 /D "NDEBUG" /D "WIN32" /D "_CONSOLE" /FR /c
CPP_PROJ=/nologo /W3 /YX /O2 /D "NDEBUG" /D "WIN32" /D "_CONSOLE" /FR$(INTDIR)/\
 /Fp$(OUTDIR)/"console.pch" /Fo$(INTDIR)/ /c 
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
RSC_PROJ=/l 0x409 /fo$(INTDIR)/"console.res" /d "NDEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# SUBTRACT BASE BSC32 /Iu
# ADD BSC32 /nologo
# SUBTRACT BSC32 /Iu
BSC32_FLAGS=/nologo /o$(OUTDIR)/"console.bsc" 
BSC32_SBRS= \
	$(INTDIR)/coninfo.sbr \
	$(INTDIR)/create.sbr \
	$(INTDIR)/scroll.sbr \
	$(INTDIR)/contitle.sbr \
	$(INTDIR)/conmode.sbr \
	$(INTDIR)/writein.sbr \
	$(INTDIR)/handler.sbr \
	$(INTDIR)/fillchar.sbr \
	$(INTDIR)/readchar.sbr \
	$(INTDIR)/getlrgst.sbr \
	$(INTDIR)/console.sbr \
	$(INTDIR)/alocfree.sbr \
	$(INTDIR)/size.sbr \
	$(INTDIR)/readout.sbr \
	$(INTDIR)/numbut.sbr \
	$(INTDIR)/cursor.sbr \
	$(INTDIR)/fillatt.sbr \
	$(INTDIR)/getnumev.sbr \
	$(INTDIR)/flush.sbr

$(OUTDIR)/console.bsc : $(OUTDIR)  $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib /NOLOGO /SUBSYSTEM:console /MACHINE:I386
# ADD LINK32 user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib /NOLOGO /SUBSYSTEM:console /MACHINE:I386
LINK32_FLAGS=user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib\
 shell32.lib /NOLOGO /SUBSYSTEM:console /INCREMENTAL:no\
 /PDB:$(OUTDIR)/"console.pdb" /MACHINE:I386 /OUT:$(OUTDIR)/"console.exe" 
DEF_FILE=
LINK32_OBJS= \
	$(INTDIR)/coninfo.obj \
	$(INTDIR)/create.obj \
	$(INTDIR)/scroll.obj \
	$(INTDIR)/contitle.obj \
	$(INTDIR)/conmode.obj \
	$(INTDIR)/console.res \
	$(INTDIR)/writein.obj \
	$(INTDIR)/handler.obj \
	$(INTDIR)/fillchar.obj \
	$(INTDIR)/readchar.obj \
	$(INTDIR)/getlrgst.obj \
	$(INTDIR)/console.obj \
	$(INTDIR)/alocfree.obj \
	$(INTDIR)/size.obj \
	$(INTDIR)/readout.obj \
	$(INTDIR)/numbut.obj \
	$(INTDIR)/cursor.obj \
	$(INTDIR)/fillatt.obj \
	$(INTDIR)/getnumev.obj \
	$(INTDIR)/flush.obj

$(OUTDIR)/console.exe : $(OUTDIR)  $(DEF_FILE) $(LINK32_OBJS)
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

ALL : $(OUTDIR)/console.exe $(OUTDIR)/console.bsc

$(OUTDIR) : 
    if not exist $(OUTDIR)/nul mkdir $(OUTDIR)

CPP=cl.exe
# ADD BASE CPP /nologo /W3 /Zi /YX /Od /D "_DEBUG" /D "_CONSOLE" /FR /c
# ADD CPP /nologo /W3 /Zi /YX /Od /D "_DEBUG" /D "WIN32" /D "_CONSOLE" /FR /c
CPP_PROJ=/nologo /W3 /Zi /YX /Od /D "_DEBUG" /D "WIN32" /D "_CONSOLE"\
 /FR$(INTDIR)/ /Fp$(OUTDIR)/"console.pch" /Fo$(INTDIR)/\
 /Fd$(OUTDIR)/"console.pdb" /c 
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
RSC_PROJ=/l 0x409 /fo$(INTDIR)/"console.res" /d "_DEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# SUBTRACT BASE BSC32 /Iu
# ADD BSC32 /nologo
# SUBTRACT BSC32 /Iu
BSC32_FLAGS=/nologo /o$(OUTDIR)/"console.bsc" 
BSC32_SBRS= \
	$(INTDIR)/coninfo.sbr \
	$(INTDIR)/create.sbr \
	$(INTDIR)/scroll.sbr \
	$(INTDIR)/contitle.sbr \
	$(INTDIR)/conmode.sbr \
	$(INTDIR)/writein.sbr \
	$(INTDIR)/handler.sbr \
	$(INTDIR)/fillchar.sbr \
	$(INTDIR)/readchar.sbr \
	$(INTDIR)/getlrgst.sbr \
	$(INTDIR)/console.sbr \
	$(INTDIR)/alocfree.sbr \
	$(INTDIR)/size.sbr \
	$(INTDIR)/readout.sbr \
	$(INTDIR)/numbut.sbr \
	$(INTDIR)/cursor.sbr \
	$(INTDIR)/fillatt.sbr \
	$(INTDIR)/getnumev.sbr \
	$(INTDIR)/flush.sbr

$(OUTDIR)/console.bsc : $(OUTDIR)  $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib /NOLOGO /SUBSYSTEM:console /DEBUG /MACHINE:I386
# ADD LINK32 user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib /NOLOGO /SUBSYSTEM:console /DEBUG /MACHINE:I386
LINK32_FLAGS=user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib\
 shell32.lib /NOLOGO /SUBSYSTEM:console /INCREMENTAL:yes\
 /PDB:$(OUTDIR)/"console.pdb" /DEBUG /MACHINE:I386 /OUT:$(OUTDIR)/"console.exe" 
DEF_FILE=
LINK32_OBJS= \
	$(INTDIR)/coninfo.obj \
	$(INTDIR)/create.obj \
	$(INTDIR)/scroll.obj \
	$(INTDIR)/contitle.obj \
	$(INTDIR)/conmode.obj \
	$(INTDIR)/console.res \
	$(INTDIR)/writein.obj \
	$(INTDIR)/handler.obj \
	$(INTDIR)/fillchar.obj \
	$(INTDIR)/readchar.obj \
	$(INTDIR)/getlrgst.obj \
	$(INTDIR)/console.obj \
	$(INTDIR)/alocfree.obj \
	$(INTDIR)/size.obj \
	$(INTDIR)/readout.obj \
	$(INTDIR)/numbut.obj \
	$(INTDIR)/cursor.obj \
	$(INTDIR)/fillatt.obj \
	$(INTDIR)/getnumev.obj \
	$(INTDIR)/flush.obj

$(OUTDIR)/console.exe : $(OUTDIR)  $(DEF_FILE) $(LINK32_OBJS)
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

ALL : $(OUTDIR)/console.exe $(OUTDIR)/console.bsc

$(OUTDIR) : 
    if not exist $(OUTDIR)/nul mkdir $(OUTDIR)

CPP=cl.exe
# ADD BASE CPP /nologo /Gt0 /QMOb2000 /W3 /Zi /YX /Od /D "_DEBUG" /D "_CONSOLE" /FR /c
# ADD CPP /nologo /Gt0 /QMOb2000 /W3 /Zi /YX /Od /D "_DEBUG" /D "_CONSOLE" /D "WIN32" /FR /c
CPP_PROJ=/nologo /Gt0 /QMOb2000 /W3 /Zi /YX /Od /D "_DEBUG" /D "_CONSOLE" /D\
 "WIN32" /FR$(INTDIR)/ /Fp$(OUTDIR)/"console.pch" /Fo$(INTDIR)/\
 /Fd$(OUTDIR)/"console.pdb" /c 
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
RSC_PROJ=/l 0x409 /fo$(INTDIR)/"console.res" /d "_DEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# SUBTRACT BASE BSC32 /Iu
# ADD BSC32 /nologo
# SUBTRACT BSC32 /Iu
BSC32_FLAGS=/nologo /o$(OUTDIR)/"console.bsc" 
BSC32_SBRS= \
	$(INTDIR)/coninfo.sbr \
	$(INTDIR)/create.sbr \
	$(INTDIR)/scroll.sbr \
	$(INTDIR)/contitle.sbr \
	$(INTDIR)/conmode.sbr \
	$(INTDIR)/writein.sbr \
	$(INTDIR)/handler.sbr \
	$(INTDIR)/fillchar.sbr \
	$(INTDIR)/readchar.sbr \
	$(INTDIR)/getlrgst.sbr \
	$(INTDIR)/console.sbr \
	$(INTDIR)/alocfree.sbr \
	$(INTDIR)/size.sbr \
	$(INTDIR)/readout.sbr \
	$(INTDIR)/numbut.sbr \
	$(INTDIR)/cursor.sbr \
	$(INTDIR)/fillatt.sbr \
	$(INTDIR)/getnumev.sbr \
	$(INTDIR)/flush.sbr

$(OUTDIR)/console.bsc : $(OUTDIR)  $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib odbc32.lib oleaut32.lib uuid.lib /NOLOGO /SUBSYSTEM:console /DEBUG /MACHINE:MIPS
# SUBTRACT BASE LINK32 /PDB:none
# ADD LINK32 user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib oleaut32.lib uuid.lib /NOLOGO /SUBSYSTEM:console /DEBUG /MACHINE:MIPS
# SUBTRACT LINK32 /PDB:none
LINK32_FLAGS=user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib\
 shell32.lib oleaut32.lib uuid.lib /NOLOGO /SUBSYSTEM:console\
 /PDB:$(OUTDIR)/"console.pdb" /DEBUG /MACHINE:MIPS /OUT:$(OUTDIR)/"console.exe" 
DEF_FILE=
LINK32_OBJS= \
	$(INTDIR)/coninfo.obj \
	$(INTDIR)/create.obj \
	$(INTDIR)/scroll.obj \
	$(INTDIR)/contitle.obj \
	$(INTDIR)/conmode.obj \
	$(INTDIR)/console.res \
	$(INTDIR)/writein.obj \
	$(INTDIR)/handler.obj \
	$(INTDIR)/fillchar.obj \
	$(INTDIR)/readchar.obj \
	$(INTDIR)/getlrgst.obj \
	$(INTDIR)/console.obj \
	$(INTDIR)/alocfree.obj \
	$(INTDIR)/size.obj \
	$(INTDIR)/readout.obj \
	$(INTDIR)/numbut.obj \
	$(INTDIR)/cursor.obj \
	$(INTDIR)/fillatt.obj \
	$(INTDIR)/getnumev.obj \
	$(INTDIR)/flush.obj

$(OUTDIR)/console.exe : $(OUTDIR)  $(DEF_FILE) $(LINK32_OBJS)
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

ALL : $(OUTDIR)/console.exe $(OUTDIR)/console.bsc

$(OUTDIR) : 
    if not exist $(OUTDIR)/nul mkdir $(OUTDIR)

CPP=cl.exe
# ADD BASE CPP /nologo /Gt0 /QMOb2000 /W3 /YX /O2 /D "NDEBUG" /D "_CONSOLE" /FR /c
# ADD CPP /nologo /Gt0 /QMOb2000 /W3 /YX /O2 /D "NDEBUG" /D "_CONSOLE" /D "WIN32" /FR /c
CPP_PROJ=/nologo /Gt0 /QMOb2000 /W3 /YX /O2 /D "NDEBUG" /D "_CONSOLE" /D\
 "WIN32" /FR$(INTDIR)/ /Fp$(OUTDIR)/"console.pch" /Fo$(INTDIR)/ /c 
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
RSC_PROJ=/l 0x409 /fo$(INTDIR)/"console.res" /d "NDEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# SUBTRACT BASE BSC32 /Iu
# ADD BSC32 /nologo
# SUBTRACT BSC32 /Iu
BSC32_FLAGS=/nologo /o$(OUTDIR)/"console.bsc" 
BSC32_SBRS= \
	$(INTDIR)/coninfo.sbr \
	$(INTDIR)/create.sbr \
	$(INTDIR)/scroll.sbr \
	$(INTDIR)/contitle.sbr \
	$(INTDIR)/conmode.sbr \
	$(INTDIR)/writein.sbr \
	$(INTDIR)/handler.sbr \
	$(INTDIR)/fillchar.sbr \
	$(INTDIR)/readchar.sbr \
	$(INTDIR)/getlrgst.sbr \
	$(INTDIR)/console.sbr \
	$(INTDIR)/alocfree.sbr \
	$(INTDIR)/size.sbr \
	$(INTDIR)/readout.sbr \
	$(INTDIR)/numbut.sbr \
	$(INTDIR)/cursor.sbr \
	$(INTDIR)/fillatt.sbr \
	$(INTDIR)/getnumev.sbr \
	$(INTDIR)/flush.sbr

$(OUTDIR)/console.bsc : $(OUTDIR)  $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib odbc32.lib oleaut32.lib uuid.lib /NOLOGO /SUBSYSTEM:console /MACHINE:MIPS
# SUBTRACT BASE LINK32 /PDB:none
# ADD LINK32 user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib oleaut32.lib uuid.lib /NOLOGO /SUBSYSTEM:console /MACHINE:MIPS
# SUBTRACT LINK32 /PDB:none
LINK32_FLAGS=user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib\
 shell32.lib oleaut32.lib uuid.lib /NOLOGO /SUBSYSTEM:console\
 /PDB:$(OUTDIR)/"console.pdb" /MACHINE:MIPS /OUT:$(OUTDIR)/"console.exe" 
DEF_FILE=
LINK32_OBJS= \
	$(INTDIR)/coninfo.obj \
	$(INTDIR)/create.obj \
	$(INTDIR)/scroll.obj \
	$(INTDIR)/contitle.obj \
	$(INTDIR)/conmode.obj \
	$(INTDIR)/console.res \
	$(INTDIR)/writein.obj \
	$(INTDIR)/handler.obj \
	$(INTDIR)/fillchar.obj \
	$(INTDIR)/readchar.obj \
	$(INTDIR)/getlrgst.obj \
	$(INTDIR)/console.obj \
	$(INTDIR)/alocfree.obj \
	$(INTDIR)/size.obj \
	$(INTDIR)/readout.obj \
	$(INTDIR)/numbut.obj \
	$(INTDIR)/cursor.obj \
	$(INTDIR)/fillatt.obj \
	$(INTDIR)/getnumev.obj \
	$(INTDIR)/flush.obj

$(OUTDIR)/console.exe : $(OUTDIR)  $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 

################################################################################
# Begin Group "Source Files"

################################################################################
# Begin Source File

SOURCE=.\coninfo.c
DEP_CONIN=\
	.\console.h

!IF  "$(CFG)" == "Win32 (80x86) Release"

$(INTDIR)/coninfo.obj :  $(SOURCE)  $(DEP_CONIN) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

$(INTDIR)/coninfo.obj :  $(SOURCE)  $(DEP_CONIN) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

$(INTDIR)/coninfo.obj :  $(SOURCE)  $(DEP_CONIN) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

$(INTDIR)/coninfo.obj :  $(SOURCE)  $(DEP_CONIN) $(INTDIR)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\create.c
DEP_CREAT=\
	.\console.h

!IF  "$(CFG)" == "Win32 (80x86) Release"

$(INTDIR)/create.obj :  $(SOURCE)  $(DEP_CREAT) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

$(INTDIR)/create.obj :  $(SOURCE)  $(DEP_CREAT) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

$(INTDIR)/create.obj :  $(SOURCE)  $(DEP_CREAT) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

$(INTDIR)/create.obj :  $(SOURCE)  $(DEP_CREAT) $(INTDIR)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\scroll.c
DEP_SCROL=\
	.\console.h

!IF  "$(CFG)" == "Win32 (80x86) Release"

$(INTDIR)/scroll.obj :  $(SOURCE)  $(DEP_SCROL) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

$(INTDIR)/scroll.obj :  $(SOURCE)  $(DEP_SCROL) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

$(INTDIR)/scroll.obj :  $(SOURCE)  $(DEP_SCROL) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

$(INTDIR)/scroll.obj :  $(SOURCE)  $(DEP_SCROL) $(INTDIR)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\contitle.c
DEP_CONTI=\
	.\console.h

!IF  "$(CFG)" == "Win32 (80x86) Release"

$(INTDIR)/contitle.obj :  $(SOURCE)  $(DEP_CONTI) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

$(INTDIR)/contitle.obj :  $(SOURCE)  $(DEP_CONTI) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

$(INTDIR)/contitle.obj :  $(SOURCE)  $(DEP_CONTI) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

$(INTDIR)/contitle.obj :  $(SOURCE)  $(DEP_CONTI) $(INTDIR)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\conmode.c
DEP_CONMO=\
	.\console.h

!IF  "$(CFG)" == "Win32 (80x86) Release"

$(INTDIR)/conmode.obj :  $(SOURCE)  $(DEP_CONMO) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

$(INTDIR)/conmode.obj :  $(SOURCE)  $(DEP_CONMO) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

$(INTDIR)/conmode.obj :  $(SOURCE)  $(DEP_CONMO) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

$(INTDIR)/conmode.obj :  $(SOURCE)  $(DEP_CONMO) $(INTDIR)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\console.rc

!IF  "$(CFG)" == "Win32 (80x86) Release"

$(INTDIR)/console.res :  $(SOURCE)  $(INTDIR)
   $(RSC) $(RSC_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

$(INTDIR)/console.res :  $(SOURCE)  $(INTDIR)
   $(RSC) $(RSC_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

$(INTDIR)/console.res :  $(SOURCE)  $(INTDIR)
   $(RSC) $(RSC_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

$(INTDIR)/console.res :  $(SOURCE)  $(INTDIR)
   $(RSC) $(RSC_PROJ)  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\writein.c
DEP_WRITE=\
	.\console.h

!IF  "$(CFG)" == "Win32 (80x86) Release"

$(INTDIR)/writein.obj :  $(SOURCE)  $(DEP_WRITE) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

$(INTDIR)/writein.obj :  $(SOURCE)  $(DEP_WRITE) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

$(INTDIR)/writein.obj :  $(SOURCE)  $(DEP_WRITE) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

$(INTDIR)/writein.obj :  $(SOURCE)  $(DEP_WRITE) $(INTDIR)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\handler.c
DEP_HANDL=\
	.\console.h

!IF  "$(CFG)" == "Win32 (80x86) Release"

$(INTDIR)/handler.obj :  $(SOURCE)  $(DEP_HANDL) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

$(INTDIR)/handler.obj :  $(SOURCE)  $(DEP_HANDL) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

$(INTDIR)/handler.obj :  $(SOURCE)  $(DEP_HANDL) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

$(INTDIR)/handler.obj :  $(SOURCE)  $(DEP_HANDL) $(INTDIR)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\fillchar.c
DEP_FILLC=\
	.\console.h

!IF  "$(CFG)" == "Win32 (80x86) Release"

$(INTDIR)/fillchar.obj :  $(SOURCE)  $(DEP_FILLC) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

$(INTDIR)/fillchar.obj :  $(SOURCE)  $(DEP_FILLC) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

$(INTDIR)/fillchar.obj :  $(SOURCE)  $(DEP_FILLC) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

$(INTDIR)/fillchar.obj :  $(SOURCE)  $(DEP_FILLC) $(INTDIR)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\readchar.c
DEP_READC=\
	.\console.h

!IF  "$(CFG)" == "Win32 (80x86) Release"

$(INTDIR)/readchar.obj :  $(SOURCE)  $(DEP_READC) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

$(INTDIR)/readchar.obj :  $(SOURCE)  $(DEP_READC) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

$(INTDIR)/readchar.obj :  $(SOURCE)  $(DEP_READC) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

$(INTDIR)/readchar.obj :  $(SOURCE)  $(DEP_READC) $(INTDIR)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\getlrgst.c
DEP_GETLR=\
	.\console.h

!IF  "$(CFG)" == "Win32 (80x86) Release"

$(INTDIR)/getlrgst.obj :  $(SOURCE)  $(DEP_GETLR) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

$(INTDIR)/getlrgst.obj :  $(SOURCE)  $(DEP_GETLR) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

$(INTDIR)/getlrgst.obj :  $(SOURCE)  $(DEP_GETLR) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

$(INTDIR)/getlrgst.obj :  $(SOURCE)  $(DEP_GETLR) $(INTDIR)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\console.c
DEP_CONSO=\
	.\console.h\
	.\size.h

!IF  "$(CFG)" == "Win32 (80x86) Release"

$(INTDIR)/console.obj :  $(SOURCE)  $(DEP_CONSO) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

$(INTDIR)/console.obj :  $(SOURCE)  $(DEP_CONSO) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

$(INTDIR)/console.obj :  $(SOURCE)  $(DEP_CONSO) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

$(INTDIR)/console.obj :  $(SOURCE)  $(DEP_CONSO) $(INTDIR)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\alocfree.c
DEP_ALOCF=\
	.\console.h

!IF  "$(CFG)" == "Win32 (80x86) Release"

$(INTDIR)/alocfree.obj :  $(SOURCE)  $(DEP_ALOCF) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

$(INTDIR)/alocfree.obj :  $(SOURCE)  $(DEP_ALOCF) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

$(INTDIR)/alocfree.obj :  $(SOURCE)  $(DEP_ALOCF) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

$(INTDIR)/alocfree.obj :  $(SOURCE)  $(DEP_ALOCF) $(INTDIR)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\size.c
DEP_SIZE_=\
	.\console.h

!IF  "$(CFG)" == "Win32 (80x86) Release"

$(INTDIR)/size.obj :  $(SOURCE)  $(DEP_SIZE_) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

$(INTDIR)/size.obj :  $(SOURCE)  $(DEP_SIZE_) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

$(INTDIR)/size.obj :  $(SOURCE)  $(DEP_SIZE_) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

$(INTDIR)/size.obj :  $(SOURCE)  $(DEP_SIZE_) $(INTDIR)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\readout.c
DEP_READO=\
	.\console.h

!IF  "$(CFG)" == "Win32 (80x86) Release"

$(INTDIR)/readout.obj :  $(SOURCE)  $(DEP_READO) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

$(INTDIR)/readout.obj :  $(SOURCE)  $(DEP_READO) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

$(INTDIR)/readout.obj :  $(SOURCE)  $(DEP_READO) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

$(INTDIR)/readout.obj :  $(SOURCE)  $(DEP_READO) $(INTDIR)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\numbut.c
DEP_NUMBU=\
	.\console.h

!IF  "$(CFG)" == "Win32 (80x86) Release"

$(INTDIR)/numbut.obj :  $(SOURCE)  $(DEP_NUMBU) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

$(INTDIR)/numbut.obj :  $(SOURCE)  $(DEP_NUMBU) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

$(INTDIR)/numbut.obj :  $(SOURCE)  $(DEP_NUMBU) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

$(INTDIR)/numbut.obj :  $(SOURCE)  $(DEP_NUMBU) $(INTDIR)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\cursor.c
DEP_CURSO=\
	.\console.h

!IF  "$(CFG)" == "Win32 (80x86) Release"

$(INTDIR)/cursor.obj :  $(SOURCE)  $(DEP_CURSO) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

$(INTDIR)/cursor.obj :  $(SOURCE)  $(DEP_CURSO) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

$(INTDIR)/cursor.obj :  $(SOURCE)  $(DEP_CURSO) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

$(INTDIR)/cursor.obj :  $(SOURCE)  $(DEP_CURSO) $(INTDIR)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\fillatt.c
DEP_FILLA=\
	.\console.h

!IF  "$(CFG)" == "Win32 (80x86) Release"

$(INTDIR)/fillatt.obj :  $(SOURCE)  $(DEP_FILLA) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

$(INTDIR)/fillatt.obj :  $(SOURCE)  $(DEP_FILLA) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

$(INTDIR)/fillatt.obj :  $(SOURCE)  $(DEP_FILLA) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

$(INTDIR)/fillatt.obj :  $(SOURCE)  $(DEP_FILLA) $(INTDIR)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\getnumev.c
DEP_GETNU=\
	.\console.h

!IF  "$(CFG)" == "Win32 (80x86) Release"

$(INTDIR)/getnumev.obj :  $(SOURCE)  $(DEP_GETNU) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

$(INTDIR)/getnumev.obj :  $(SOURCE)  $(DEP_GETNU) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

$(INTDIR)/getnumev.obj :  $(SOURCE)  $(DEP_GETNU) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

$(INTDIR)/getnumev.obj :  $(SOURCE)  $(DEP_GETNU) $(INTDIR)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\flush.c
DEP_FLUSH=\
	.\console.h

!IF  "$(CFG)" == "Win32 (80x86) Release"

$(INTDIR)/flush.obj :  $(SOURCE)  $(DEP_FLUSH) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

$(INTDIR)/flush.obj :  $(SOURCE)  $(DEP_FLUSH) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

$(INTDIR)/flush.obj :  $(SOURCE)  $(DEP_FLUSH) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

$(INTDIR)/flush.obj :  $(SOURCE)  $(DEP_FLUSH) $(INTDIR)

!ENDIF 

# End Source File
# End Group
# End Project
################################################################################
