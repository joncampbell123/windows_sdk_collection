# Microsoft Visual C++ Generated NMAKE File, Format Version 2.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101
# TARGTYPE "Win32 (MIPS) Application" 0x0501

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
!MESSAGE NMAKE /f "regmpad.mak" CFG="Win32 (80x86) Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Win32 (80x86) Release" (based on "Win32 (x86) Application")
!MESSAGE "Win32 (80x86) Debug" (based on "Win32 (x86) Application")
!MESSAGE "Win32 (MIPS) Release" (based on "Win32 (MIPS) Application")
!MESSAGE "Win32 (MIPS) Debug" (based on "Win32 (MIPS) Application")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

################################################################################
# Begin Project
# PROP Target_Last_Scanned "Win32 (MIPS) Release"

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

ALL : .\WinRel\regmpad.exe .\WinRel\regmpad.bsc

$(OUTDIR) : 
    if not exist $(OUTDIR)/nul mkdir $(OUTDIR)

MTL=MkTypLib.exe
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
CPP=cl.exe
# ADD BASE CPP /nologo /MD /W3 /GX /YX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /c
# ADD CPP /nologo /W3 /GX /YX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /FR /c
CPP_PROJ=/nologo /W3 /GX /YX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D\
 "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"regmpad.pch" /Fo$(INTDIR)/ /c 
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
RSC_PROJ=/l 0x409 /fo$(INTDIR)/"multipad.res" /d "NDEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# SUBTRACT BASE BSC32 /Iu
# ADD BSC32 /nologo
# SUBTRACT BSC32 /Iu
BSC32_FLAGS=/nologo /o$(OUTDIR)/"regmpad.bsc" 
BSC32_SBRS= \
	.\WinRel\mpfile.sbr \
	.\WinRel\multipad.sbr \
	.\WinRel\mpinit.sbr \
	.\WinRel\regdb.sbr \
	.\WinRel\mpprint.sbr \
	.\WinRel\mpopen.sbr \
	.\WinRel\mpfind.sbr

.\WinRel\regmpad.bsc : $(OUTDIR)  $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib mfc30.lib mfco30.lib mfcd30.lib mfcuia32.lib mfcans32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib /NOLOGO /SUBSYSTEM:windows /MACHINE:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /NOLOGO /SUBSYSTEM:windows /MACHINE:I386
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /NOLOGO\
 /SUBSYSTEM:windows /INCREMENTAL:no /PDB:$(OUTDIR)/"regmpad.pdb" /MACHINE:I386\
 /OUT:$(OUTDIR)/"regmpad.exe" 
DEF_FILE=
LINK32_OBJS= \
	.\WinRel\mpfile.obj \
	.\WinRel\multipad.obj \
	.\WinRel\mpinit.obj \
	.\WinRel\regdb.obj \
	.\WinRel\multipad.res \
	.\WinRel\mpprint.obj \
	.\WinRel\mpopen.obj \
	.\WinRel\mpfind.obj

.\WinRel\regmpad.exe : $(OUTDIR)  $(DEF_FILE) $(LINK32_OBJS)
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

ALL : .\WinDebug\regmpad.exe .\WinDebug\regmpad.bsc

$(OUTDIR) : 
    if not exist $(OUTDIR)/nul mkdir $(OUTDIR)

MTL=MkTypLib.exe
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
CPP=cl.exe
# ADD BASE CPP /nologo /MD /W3 /GX /Zi /YX /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /c
# ADD CPP /nologo /W3 /GX /Zi /YX /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /FR /c
CPP_PROJ=/nologo /W3 /GX /Zi /YX /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D\
 "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"regmpad.pch" /Fo$(INTDIR)/\
 /Fd$(OUTDIR)/"regmpad.pdb" /c 
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
RSC_PROJ=/l 0x409 /fo$(INTDIR)/"multipad.res" /d "_DEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# SUBTRACT BASE BSC32 /Iu
# ADD BSC32 /nologo
# SUBTRACT BSC32 /Iu
BSC32_FLAGS=/nologo /o$(OUTDIR)/"regmpad.bsc" 
BSC32_SBRS= \
	.\WinDebug\mpfile.sbr \
	.\WinDebug\multipad.sbr \
	.\WinDebug\mpinit.sbr \
	.\WinDebug\regdb.sbr \
	.\WinDebug\mpprint.sbr \
	.\WinDebug\mpopen.sbr \
	.\WinDebug\mpfind.sbr

.\WinDebug\regmpad.bsc : $(OUTDIR)  $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib mfc30d.lib mfco30d.lib mfcd30d.lib mfcuia32.lib mfcans32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib /NOLOGO /SUBSYSTEM:windows /DEBUG /MACHINE:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /NOLOGO /SUBSYSTEM:windows /DEBUG /MACHINE:I386
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /NOLOGO\
 /SUBSYSTEM:windows /INCREMENTAL:yes /PDB:$(OUTDIR)/"regmpad.pdb" /DEBUG\
 /MACHINE:I386 /OUT:$(OUTDIR)/"regmpad.exe" 
DEF_FILE=
LINK32_OBJS= \
	.\WinDebug\mpfile.obj \
	.\WinDebug\multipad.obj \
	.\WinDebug\mpinit.obj \
	.\WinDebug\regdb.obj \
	.\WinDebug\multipad.res \
	.\WinDebug\mpprint.obj \
	.\WinDebug\mpopen.obj \
	.\WinDebug\mpfind.obj

.\WinDebug\regmpad.exe : $(OUTDIR)  $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Win32__M"
# PROP BASE Intermediate_Dir "Win32__M"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "WinRel"
# PROP Intermediate_Dir "WinRel"
OUTDIR=.\WinRel
INTDIR=.\WinRel

ALL : .\WinRel\regmpad.exe .\WinRel\regmpad.bsc

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
 "_WINDOWS" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"regmpad.pch" /Fo$(INTDIR)/ /c\
 
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
RSC_PROJ=/l 0x409 /fo$(INTDIR)/"multipad.res" /d "NDEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# SUBTRACT BASE BSC32 /Iu
# ADD BSC32 /nologo
# SUBTRACT BSC32 /Iu
BSC32_FLAGS=/nologo /o$(OUTDIR)/"regmpad.bsc" 
BSC32_SBRS= \
	.\WinRel\mpfile.sbr \
	.\WinRel\multipad.sbr \
	.\WinRel\mpinit.sbr \
	.\WinRel\regdb.sbr \
	.\WinRel\mpprint.sbr \
	.\WinRel\mpopen.sbr \
	.\WinRel\mpfind.sbr

.\WinRel\regmpad.bsc : $(OUTDIR)  $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 /NOLOGO /SUBSYSTEM:windows /MACHINE:MIPS
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /NOLOGO /SUBSYSTEM:windows /MACHINE:MIPS
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /NOLOGO\
 /SUBSYSTEM:windows /PDB:$(OUTDIR)/"regmpad.pdb" /MACHINE:MIPS\
 /OUT:$(OUTDIR)/"regmpad.exe" 
DEF_FILE=
LINK32_OBJS= \
	.\WinRel\mpfile.obj \
	.\WinRel\multipad.obj \
	.\WinRel\mpinit.obj \
	.\WinRel\regdb.obj \
	.\WinRel\multipad.res \
	.\WinRel\mpprint.obj \
	.\WinRel\mpopen.obj \
	.\WinRel\mpfind.obj

.\WinRel\regmpad.exe : $(OUTDIR)  $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Win32__0"
# PROP BASE Intermediate_Dir "Win32__0"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "WinDebug"
# PROP Intermediate_Dir "WinDebug"
OUTDIR=.\WinDebug
INTDIR=.\WinDebug

ALL : .\WinDebug\regmpad.exe .\WinDebug\regmpad.bsc

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
 "_WINDOWS" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"regmpad.pch" /Fo$(INTDIR)/\
 /Fd$(OUTDIR)/"regmpad.pdb" /c 
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
RSC_PROJ=/l 0x409 /fo$(INTDIR)/"multipad.res" /d "_DEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# SUBTRACT BASE BSC32 /Iu
# ADD BSC32 /nologo
# SUBTRACT BSC32 /Iu
BSC32_FLAGS=/nologo /o$(OUTDIR)/"regmpad.bsc" 
BSC32_SBRS= \
	.\WinDebug\mpfile.sbr \
	.\WinDebug\multipad.sbr \
	.\WinDebug\mpinit.sbr \
	.\WinDebug\regdb.sbr \
	.\WinDebug\mpprint.sbr \
	.\WinDebug\mpopen.sbr \
	.\WinDebug\mpfind.sbr

.\WinDebug\regmpad.bsc : $(OUTDIR)  $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 /NOLOGO /SUBSYSTEM:windows /DEBUG /MACHINE:MIPS
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /NOLOGO /SUBSYSTEM:windows /DEBUG /MACHINE:MIPS
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /NOLOGO\
 /SUBSYSTEM:windows /PDB:$(OUTDIR)/"regmpad.pdb" /DEBUG /MACHINE:MIPS\
 /OUT:$(OUTDIR)/"regmpad.exe" 
DEF_FILE=
LINK32_OBJS= \
	.\WinDebug\mpfile.obj \
	.\WinDebug\multipad.obj \
	.\WinDebug\mpinit.obj \
	.\WinDebug\regdb.obj \
	.\WinDebug\multipad.res \
	.\WinDebug\mpprint.obj \
	.\WinDebug\mpopen.obj \
	.\WinDebug\mpfind.obj

.\WinDebug\regmpad.exe : $(OUTDIR)  $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 

################################################################################
# Begin Group "Source Files"

################################################################################
# Begin Source File

SOURCE=.\mpfile.c
DEP_MPFIL=\
	.\multipad.h\
	SYS\TYPES.H\
	SYS\STAT.H

!IF  "$(CFG)" == "Win32 (80x86) Release"

.\WinRel\mpfile.obj :  $(SOURCE)  $(DEP_MPFIL) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

.\WinDebug\mpfile.obj :  $(SOURCE)  $(DEP_MPFIL) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

.\WinRel\mpfile.obj :  $(SOURCE)  $(DEP_MPFIL) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

.\WinDebug\mpfile.obj :  $(SOURCE)  $(DEP_MPFIL) $(INTDIR)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\multipad.c
DEP_MULTI=\
	.\multipad.h\
	.\regdb.h

!IF  "$(CFG)" == "Win32 (80x86) Release"

.\WinRel\multipad.obj :  $(SOURCE)  $(DEP_MULTI) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

.\WinDebug\multipad.obj :  $(SOURCE)  $(DEP_MULTI) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

.\WinRel\multipad.obj :  $(SOURCE)  $(DEP_MULTI) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

.\WinDebug\multipad.obj :  $(SOURCE)  $(DEP_MULTI) $(INTDIR)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\mpinit.c
DEP_MPINI=\
	.\multipad.h\
	.\regdb.h

!IF  "$(CFG)" == "Win32 (80x86) Release"

.\WinRel\mpinit.obj :  $(SOURCE)  $(DEP_MPINI) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

.\WinDebug\mpinit.obj :  $(SOURCE)  $(DEP_MPINI) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

.\WinRel\mpinit.obj :  $(SOURCE)  $(DEP_MPINI) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

.\WinDebug\mpinit.obj :  $(SOURCE)  $(DEP_MPINI) $(INTDIR)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\regdb.c
DEP_REGDB=\
	.\multipad.h\
	.\regdb.h

!IF  "$(CFG)" == "Win32 (80x86) Release"

.\WinRel\regdb.obj :  $(SOURCE)  $(DEP_REGDB) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

.\WinDebug\regdb.obj :  $(SOURCE)  $(DEP_REGDB) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

.\WinRel\regdb.obj :  $(SOURCE)  $(DEP_REGDB) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

.\WinDebug\regdb.obj :  $(SOURCE)  $(DEP_REGDB) $(INTDIR)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\multipad.rc

!IF  "$(CFG)" == "Win32 (80x86) Release"

.\WinRel\multipad.res :  $(SOURCE)  $(INTDIR)
   $(RSC) $(RSC_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

.\WinDebug\multipad.res :  $(SOURCE)  $(INTDIR)
   $(RSC) $(RSC_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

.\WinRel\multipad.res :  $(SOURCE)  $(INTDIR)
   $(RSC) $(RSC_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

.\WinDebug\multipad.res :  $(SOURCE)  $(INTDIR)
   $(RSC) $(RSC_PROJ)  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\mpprint.c
DEP_MPPRI=\
	.\multipad.h

!IF  "$(CFG)" == "Win32 (80x86) Release"

.\WinRel\mpprint.obj :  $(SOURCE)  $(DEP_MPPRI) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

.\WinDebug\mpprint.obj :  $(SOURCE)  $(DEP_MPPRI) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

.\WinRel\mpprint.obj :  $(SOURCE)  $(DEP_MPPRI) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

.\WinDebug\mpprint.obj :  $(SOURCE)  $(DEP_MPPRI) $(INTDIR)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\mpopen.c
DEP_MPOPE=\
	.\multipad.h

!IF  "$(CFG)" == "Win32 (80x86) Release"

.\WinRel\mpopen.obj :  $(SOURCE)  $(DEP_MPOPE) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

.\WinDebug\mpopen.obj :  $(SOURCE)  $(DEP_MPOPE) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

.\WinRel\mpopen.obj :  $(SOURCE)  $(DEP_MPOPE) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

.\WinDebug\mpopen.obj :  $(SOURCE)  $(DEP_MPOPE) $(INTDIR)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\mpfind.c
DEP_MPFIN=\
	.\multipad.h

!IF  "$(CFG)" == "Win32 (80x86) Release"

.\WinRel\mpfind.obj :  $(SOURCE)  $(DEP_MPFIN) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

.\WinDebug\mpfind.obj :  $(SOURCE)  $(DEP_MPFIN) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

.\WinRel\mpfind.obj :  $(SOURCE)  $(DEP_MPFIN) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

.\WinDebug\mpfind.obj :  $(SOURCE)  $(DEP_MPFIN) $(INTDIR)

!ENDIF 

# End Source File
# End Group
# End Project
################################################################################
