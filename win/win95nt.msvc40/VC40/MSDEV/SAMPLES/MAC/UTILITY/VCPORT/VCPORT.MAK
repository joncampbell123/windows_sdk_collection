# Microsoft Visual C++ Generated NMAKE File, Format Version 40000
# ** DO NOT EDIT **

# TARGTYPE "Win32 (MIPS) Console Application" 0x0503
# TARGTYPE "Win32 (x86) Console Application" 0x0103

!IF "$(CFG)" == ""
CFG=VCPort - Win32 Release
!MESSAGE No configuration specified.  Defaulting to VCPort - Win32 Release.
!ENDIF 

!IF "$(CFG)" != "VCPort - Win32 Release" && "$(CFG)" !=\
 "VCPort - Win32 (MIPS) Release"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "VCPort1.mak" CFG="VCPort - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "VCPort - Win32 Release" (based on\
 "Win32 (x86) Console Application")
!MESSAGE "VCPort - Win32 (MIPS) Release" (based on\
 "Win32 (MIPS) Console Application")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 
################################################################################
# Begin Project

!IF  "$(CFG)" == "VCPort - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir WinRel
# PROP BASE Intermediate_Dir WinRel
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir Release
# PROP Intermediate_Dir Release
OUTDIR=.\Release
INTDIR=.\Release

ALL : "$(OUTDIR)\VCPort1.exe"

CLEAN : 
        -@erase ".\Release\VCPort1.exe"
        -@erase ".\Release\VCPort.obj"
	-@erase ".\Release\vc40.idb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

BSC32=bscmake.exe

$(OUTDIR)/VCPort.bsc : $(OUTDIR)  $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

CPP=cl.exe
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /FR /YX /c
# ADD CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /YX /c
# SUBTRACT CPP /Fr
CPP_PROJ=/nologo /ML /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE"\
 /Fp"$(INTDIR)/VCPort1.pch" /YX /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\Release/
CPP_SBRS=

.c{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.c{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

RSC=rc.exe
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/VCPort1.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /subsystem:console /machine:I386
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo\
 /subsystem:console /incremental:no /pdb:"$(OUTDIR)/VCPort1.pdb" /machine:I386\
 /out:"$(OUTDIR)/VCPort1.exe" 
LINK32_OBJS= \
        "$(INTDIR)/VCPort.obj"

"$(OUTDIR)\VCPort1.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "VCPort - Win32 (MIPS) Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir WinRel
# PROP BASE Intermediate_Dir WinRel
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir
# PROP Intermediate_Dir
OUTDIR=.
INTDIR=.

ALL : 

CLEAN : 
	-@erase 

BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_SBRS= \
        $(INTDIR)/VCPort.sbr
BSC32_FLAGS=/nologo /o$(OUTDIR)/"VCPort.bsc" 
CPP=cl.exe
# ADD BASE CPP /nologo /ML /Gt0 /QMOb2000 /W3 /GX /YX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /FR /c
# ADD CPP /nologo /ML /Gt0 /QMOb2000 /W3 /GX /YX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /FR /c
CPP_PROJ=/nologo /ML /Gt0 /QMOb2000 /W3 /GX /YX /O2 /D "WIN32" /D "NDEBUG" /D\
 "_CONSOLE" /FR$(INTDIR)/ /Fp$(OUTDIR)/"VCPort.pch" /Fo$(INTDIR)/ /c 
CPP_OBJS=

.c{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /NOLOGO /SUBSYSTEM:console /MACHINE:MIPS
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /NOLOGO /SUBSYSTEM:console /MACHINE:MIPS
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /NOLOGO\
 /SUBSYSTEM:console /PDB:$(OUTDIR)/"VCPort.pdb" /MACHINE:MIPS\
 /OUT:$(OUTDIR)/"VCPort.exe" 
DEF_FILE=
LINK32_OBJS= \
        $(INTDIR)/VCPort.obj

$(OUTDIR)/VCPort.exe : $(OUTDIR)  $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

$(OUTDIR)/VCPort.bsc : $(OUTDIR)  $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

!ENDIF 

################################################################################
# Begin Target

# Name "VCPort - Win32 Release"
# Name "VCPort - Win32 (MIPS) Release"

!IF  "$(CFG)" == "VCPort - Win32 Release"

!ELSEIF  "$(CFG)" == "VCPort - Win32 (MIPS) Release"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\VCPort.c

!IF  "$(CFG)" == "VCPort - Win32 Release"


"$(INTDIR)\VCPort.obj" : $(SOURCE) "$(INTDIR)"

"$(INTDIR)\vc40.idb" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "VCPort - Win32 (MIPS) Release"

$(INTDIR)/VCPort.obj :  $(SOURCE)  $(INTDIR)

!ENDIF 

# End Source File
# End Target
# End Project
################################################################################
