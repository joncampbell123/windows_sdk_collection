# Microsoft Visual C++ Generated NMAKE File, Format Version 2.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101
# TARGTYPE "Win32 (MIPS) Application" 0x0501

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
!MESSAGE NMAKE /f "install.mak" CFG="Win32 (80x86) Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Win32 (80x86) Release" (based on "Win32 (x86) Application")
!MESSAGE "Win32 (80x86) Debug" (based on "Win32 (x86) Application")
!MESSAGE "Win32 (MIPS) Debug" (based on "Win32 (MIPS) Application")
!MESSAGE "Win32 (MIPS) Release" (based on "Win32 (MIPS) Application")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

################################################################################
# Begin Project
# PROP Target_Last_Scanned "Win32 (MIPS) Debug"

!IF  "$(CFG)" == "Win32 (80x86) Release"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "WinRel"
# PROP BASE Intermediate_Dir "WinRel"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "WinRel"
# PROP Intermediate_Dir "WinRel"
OUTDIR=.\WinRel
INTDIR=.\WinRel

ALL : .\WinRel\install.exe .\WinRel\install.bsc

$(OUTDIR) : 
    if not exist $(OUTDIR)/nul mkdir $(OUTDIR)

MTL=MkTypLib.exe
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
CPP=cl.exe
# ADD BASE CPP /nologo /MD /W3 /GX /YX /O2 /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /c
# ADD CPP /nologo /W3 /GX /YX /O2 /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "WIN32" /FR /c
CPP_PROJ=/nologo /W3 /GX /YX /O2 /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D\
 "WIN32" /FR$(INTDIR)/ /Fp$(OUTDIR)/"install.pch" /Fo$(INTDIR)/ /c 
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
RSC_PROJ=/l 0x409 /fo$(INTDIR)/"install.res" /d "NDEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# SUBTRACT BASE BSC32 /Iu
# ADD BSC32 /nologo
# SUBTRACT BSC32 /Iu
BSC32_FLAGS=/nologo /o$(OUTDIR)/"install.bsc" 
BSC32_SBRS= \
	.\WinRel\ddedlg.sbr \
	.\WinRel\ddeadd.sbr \
	.\WinRel\ddeinst.sbr \
	.\WinRel\ddeprocs.sbr \
	.\WinRel\ddemain.sbr

.\WinRel\install.bsc : $(OUTDIR)  $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib mfc30.lib mfco30.lib mfcd30.lib compob32.lib storag32.lib ole2w32.lib ole232.lib ole2di32.lib /NOLOGO /SUBSYSTEM:windows /MACHINE:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /NOLOGO /SUBSYSTEM:windows /MACHINE:I386
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /NOLOGO\
 /SUBSYSTEM:windows /INCREMENTAL:no /PDB:$(OUTDIR)/"install.pdb" /MACHINE:I386\
 /OUT:$(OUTDIR)/"install.exe" 
DEF_FILE=
LINK32_OBJS= \
	.\WinRel\install.res \
	.\WinRel\ddedlg.obj \
	.\WinRel\ddeadd.obj \
	.\WinRel\ddeinst.obj \
	.\WinRel\ddeprocs.obj \
	.\WinRel\ddemain.obj

.\WinRel\install.exe : $(OUTDIR)  $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "WinDebug"
# PROP BASE Intermediate_Dir "WinDebug"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "WinDebug"
# PROP Intermediate_Dir "WinDebug"
OUTDIR=.\WinDebug
INTDIR=.\WinDebug

ALL : .\WinDebug\install.exe .\WinDebug\install.bsc

$(OUTDIR) : 
    if not exist $(OUTDIR)/nul mkdir $(OUTDIR)

MTL=MkTypLib.exe
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
CPP=cl.exe
# ADD BASE CPP /nologo /MD /W3 /GX /Zi /YX /Od /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /c
# ADD CPP /nologo /W3 /GX /Zi /YX /Od /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "WIN32" /FR /c
CPP_PROJ=/nologo /W3 /GX /Zi /YX /Od /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D\
 "WIN32" /FR$(INTDIR)/ /Fp$(OUTDIR)/"install.pch" /Fo$(INTDIR)/\
 /Fd$(OUTDIR)/"install.pdb" /c 
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
RSC_PROJ=/l 0x409 /fo$(INTDIR)/"install.res" /d "_DEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# SUBTRACT BASE BSC32 /Iu
# ADD BSC32 /nologo
# SUBTRACT BSC32 /Iu
BSC32_FLAGS=/nologo /o$(OUTDIR)/"install.bsc" 
BSC32_SBRS= \
	.\WinDebug\ddedlg.sbr \
	.\WinDebug\ddeadd.sbr \
	.\WinDebug\ddeinst.sbr \
	.\WinDebug\ddeprocs.sbr \
	.\WinDebug\ddemain.sbr

.\WinDebug\install.bsc : $(OUTDIR)  $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib mfc30d.lib mfco30d.lib mfcd30d.lib compob32.lib storag32.lib ole2w32.lib ole232.lib ole2di32.lib /NOLOGO /SUBSYSTEM:windows /DEBUG /MACHINE:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /NOLOGO /SUBSYSTEM:windows /DEBUG /MACHINE:I386
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /NOLOGO\
 /SUBSYSTEM:windows /INCREMENTAL:yes /PDB:$(OUTDIR)/"install.pdb" /DEBUG\
 /MACHINE:I386 /OUT:$(OUTDIR)/"install.exe" 
DEF_FILE=
LINK32_OBJS= \
	.\WinDebug\install.res \
	.\WinDebug\ddedlg.obj \
	.\WinDebug\ddeadd.obj \
	.\WinDebug\ddeinst.obj \
	.\WinDebug\ddeprocs.obj \
	.\WinDebug\ddemain.obj

.\WinDebug\install.exe : $(OUTDIR)  $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Win32__M"
# PROP BASE Intermediate_Dir "Win32__M"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "WinDebug"
# PROP Intermediate_Dir "WinDebug"
OUTDIR=.\WinDebug
INTDIR=.\WinDebug

ALL : .\WinDebug\install.exe .\WinDebug\install.bsc

$(OUTDIR) : 
    if not exist $(OUTDIR)/nul mkdir $(OUTDIR)

MTL=MkTypLib.exe
# ADD BASE MTL /nologo /D "_DEBUG" /mips
# ADD MTL /nologo /D "_DEBUG" /mips
MTL_PROJ=/nologo /D "_DEBUG" /mips 
CPP=cl.exe
# ADD BASE CPP /nologo /MD /Gt0 /QMOb2000 /W3 /GX /Zi /YX /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /c
# ADD CPP /nologo /Gt0 /QMOb2000 /W3 /GX /Zi /YX /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /FR /c
CPP_PROJ=/nologo /Gt0 /QMOb2000 /W3 /GX /Zi /YX /Od /D "WIN32" /D "_DEBUG" /D\
 "_WINDOWS" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"install.pch" /Fo$(INTDIR)/\
 /Fd$(OUTDIR)/"install.pdb" /c 
CPP_OBJS=.\WinDebug/

.c{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

RSC=rc.exe
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG"
RSC_PROJ=/l 0x409 /fo$(INTDIR)/"install.res" /d "_DEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# SUBTRACT BASE BSC32 /Iu
# ADD BSC32 /nologo
# SUBTRACT BSC32 /Iu
BSC32_FLAGS=/nologo /o$(OUTDIR)/"install.bsc" 
BSC32_SBRS= \
	.\WinDebug\ddedlg.sbr \
	.\WinDebug\ddeadd.sbr \
	.\WinDebug\ddeinst.sbr \
	.\WinDebug\ddeprocs.sbr \
	.\WinDebug\ddemain.sbr

.\WinDebug\install.bsc : $(OUTDIR)  $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 /NOLOGO /SUBSYSTEM:windows /DEBUG /MACHINE:MIPS
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /NOLOGO /SUBSYSTEM:windows /DEBUG /MACHINE:MIPS
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /NOLOGO\
 /SUBSYSTEM:windows /PDB:$(OUTDIR)/"install.pdb" /DEBUG /MACHINE:MIPS\
 /OUT:$(OUTDIR)/"install.exe" 
DEF_FILE=
LINK32_OBJS= \
	.\WinDebug\install.res \
	.\WinDebug\ddedlg.obj \
	.\WinDebug\ddeadd.obj \
	.\WinDebug\ddeinst.obj \
	.\WinDebug\ddeprocs.obj \
	.\WinDebug\ddemain.obj

.\WinDebug\install.exe : $(OUTDIR)  $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Win32__0"
# PROP BASE Intermediate_Dir "Win32__0"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "WinRel"
# PROP Intermediate_Dir "WinRel"
OUTDIR=.\WinRel
INTDIR=.\WinRel

ALL : .\WinRel\install.exe .\WinRel\install.bsc

$(OUTDIR) : 
    if not exist $(OUTDIR)/nul mkdir $(OUTDIR)

MTL=MkTypLib.exe
# ADD BASE MTL /nologo /D "NDEBUG" /mips
# ADD MTL /nologo /D "NDEBUG" /mips
MTL_PROJ=/nologo /D "NDEBUG" /mips 
CPP=cl.exe
# ADD BASE CPP /nologo /MD /Gt0 /QMOb2000 /W3 /GX /YX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /c
# ADD CPP /nologo /Gt0 /QMOb2000 /W3 /GX /YX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /FR /c
CPP_PROJ=/nologo /Gt0 /QMOb2000 /W3 /GX /YX /O2 /D "WIN32" /D "NDEBUG" /D\
 "_WINDOWS" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"install.pch" /Fo$(INTDIR)/ /c\
 
CPP_OBJS=.\WinRel/

.c{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

RSC=rc.exe
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG"
RSC_PROJ=/l 0x409 /fo$(INTDIR)/"install.res" /d "NDEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# SUBTRACT BASE BSC32 /Iu
# ADD BSC32 /nologo
# SUBTRACT BSC32 /Iu
BSC32_FLAGS=/nologo /o$(OUTDIR)/"install.bsc" 
BSC32_SBRS= \
	.\WinRel\ddedlg.sbr \
	.\WinRel\ddeadd.sbr \
	.\WinRel\ddeinst.sbr \
	.\WinRel\ddeprocs.sbr \
	.\WinRel\ddemain.sbr

.\WinRel\install.bsc : $(OUTDIR)  $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 /NOLOGO /SUBSYSTEM:windows /MACHINE:MIPS
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /NOLOGO /SUBSYSTEM:windows /MACHINE:MIPS
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /NOLOGO\
 /SUBSYSTEM:windows /PDB:$(OUTDIR)/"install.pdb" /MACHINE:MIPS\
 /OUT:$(OUTDIR)/"install.exe" 
DEF_FILE=
LINK32_OBJS= \
	.\WinRel\install.res \
	.\WinRel\ddedlg.obj \
	.\WinRel\ddeadd.obj \
	.\WinRel\ddeinst.obj \
	.\WinRel\ddeprocs.obj \
	.\WinRel\ddemain.obj

.\WinRel\install.exe : $(OUTDIR)  $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 

################################################################################
# Begin Group "Source Files"

################################################################################
# Begin Source File

SOURCE=.\install.rc

!IF  "$(CFG)" == "Win32 (80x86) Release"

.\WinRel\install.res :  $(SOURCE)  $(INTDIR)
   $(RSC) $(RSC_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

.\WinDebug\install.res :  $(SOURCE)  $(INTDIR)
   $(RSC) $(RSC_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

.\WinDebug\install.res :  $(SOURCE)  $(INTDIR)
   $(RSC) $(RSC_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

.\WinRel\install.res :  $(SOURCE)  $(INTDIR)
   $(RSC) $(RSC_PROJ)  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\ddedlg.c
DEP_DDEDL=\
	.\ddeinst.h\
	.\ddextrn.h

!IF  "$(CFG)" == "Win32 (80x86) Release"

.\WinRel\ddedlg.obj :  $(SOURCE)  $(DEP_DDEDL) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

.\WinDebug\ddedlg.obj :  $(SOURCE)  $(DEP_DDEDL) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

.\WinDebug\ddedlg.obj :  $(SOURCE)  $(DEP_DDEDL) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

.\WinRel\ddedlg.obj :  $(SOURCE)  $(DEP_DDEDL) $(INTDIR)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\ddeadd.c
DEP_DDEAD=\
	.\ddeinst.h\
	.\ddextrn.h\
	.\dialogs.h

!IF  "$(CFG)" == "Win32 (80x86) Release"

.\WinRel\ddeadd.obj :  $(SOURCE)  $(DEP_DDEAD) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

.\WinDebug\ddeadd.obj :  $(SOURCE)  $(DEP_DDEAD) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

.\WinDebug\ddeadd.obj :  $(SOURCE)  $(DEP_DDEAD) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

.\WinRel\ddeadd.obj :  $(SOURCE)  $(DEP_DDEAD) $(INTDIR)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\ddeinst.c
DEP_DDEIN=\
	.\ddeinst.h\
	.\ddextrn.h

!IF  "$(CFG)" == "Win32 (80x86) Release"

.\WinRel\ddeinst.obj :  $(SOURCE)  $(DEP_DDEIN) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

.\WinDebug\ddeinst.obj :  $(SOURCE)  $(DEP_DDEIN) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

.\WinDebug\ddeinst.obj :  $(SOURCE)  $(DEP_DDEIN) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

.\WinRel\ddeinst.obj :  $(SOURCE)  $(DEP_DDEIN) $(INTDIR)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\ddeprocs.c
DEP_DDEPR=\
	.\ddeinst.h\
	.\ddextrn.h\
	.\dialogs.h

!IF  "$(CFG)" == "Win32 (80x86) Release"

.\WinRel\ddeprocs.obj :  $(SOURCE)  $(DEP_DDEPR) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

.\WinDebug\ddeprocs.obj :  $(SOURCE)  $(DEP_DDEPR) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

.\WinDebug\ddeprocs.obj :  $(SOURCE)  $(DEP_DDEPR) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

.\WinRel\ddeprocs.obj :  $(SOURCE)  $(DEP_DDEPR) $(INTDIR)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\ddemain.c
DEP_DDEMA=\
	.\ddeinst.h\
	.\ddeglbl.h

!IF  "$(CFG)" == "Win32 (80x86) Release"

.\WinRel\ddemain.obj :  $(SOURCE)  $(DEP_DDEMA) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

.\WinDebug\ddemain.obj :  $(SOURCE)  $(DEP_DDEMA) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

.\WinDebug\ddemain.obj :  $(SOURCE)  $(DEP_DDEMA) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

.\WinRel\ddemain.obj :  $(SOURCE)  $(DEP_DDEMA) $(INTDIR)

!ENDIF 

# End Source File
# End Group
# End Project
################################################################################
