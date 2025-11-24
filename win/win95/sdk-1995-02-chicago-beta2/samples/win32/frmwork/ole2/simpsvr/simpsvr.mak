# Microsoft Visual C++ Generated NMAKE File, Format Version 2.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

!IF "$(CFG)" == ""
CFG=Win32 ANSI Debug
!MESSAGE No configuration specified.  Defaulting to Win32 ANSI Debug.
!ENDIF 

!IF "$(CFG)" != "Win32 ANSI Release" && "$(CFG)" != "Win32 ANSI Debug" &&\
 "$(CFG)" != "Win32 Unicode Release" && "$(CFG)" != "Win32 Unicode Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "simpsvr.mak" CFG="Win32 ANSI Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Win32 ANSI Release" (based on "Win32 (x86) Application")
!MESSAGE "Win32 ANSI Debug" (based on "Win32 (x86) Application")
!MESSAGE "Win32 Unicode Release" (based on "Win32 (x86) Application")
!MESSAGE "Win32 Unicode Debug" (based on "Win32 (x86) Application")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

################################################################################
# Begin Project
# PROP Target_Last_Scanned "Win32 ANSI Debug"
MTL=MkTypLib.exe
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Win32 ANSI Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "WinRel"
# PROP Intermediate_Dir "WinRel"
OUTDIR=.\WinRel
INTDIR=.\WinRel

ALL : .\WinRel\simpsvr.exe .\WinRel\simpsvr.bsc

$(OUTDIR) : 
    if not exist $(OUTDIR)/nul mkdir $(OUTDIR)

# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
# ADD BASE CPP /nologo /Zp4 /W3 /YX /O2 /D "NDEBUG" /D "_WINDOWS" /D "_X86_" /D "WIN32" /D "STRICT" /FR /c
# ADD CPP /nologo /Zp4 /W3 /O2 /D "NDEBUG" /D "_WINDOWS" /D "_X86_" /D "WIN32" /D "STRICT" /FR /Yu"precomp.h" /c
CPP_PROJ=/nologo /Zp4 /W3 /O2 /D "NDEBUG" /D "_WINDOWS" /D "_X86_" /D "WIN32"\
 /D "STRICT" /FR$(INTDIR)/ /Fp$(OUTDIR)/"simpsvr.pch" /Yu"precomp.h"\
 /Fo$(INTDIR)/ /c 
CPP_OBJS=.\WinRel/
# ADD BASE RSC /l 0x1 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
RSC_PROJ=/l 0x409 /fo$(INTDIR)/"SIMPSVR.res" /d "NDEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# SUBTRACT BASE BSC32 /Iu
# ADD BSC32 /nologo
# SUBTRACT BSC32 /Iu
BSC32_FLAGS=/nologo /o$(OUTDIR)/"simpsvr.bsc" 
BSC32_SBRS= \
	.\WinRel\DOC.SBR \
	.\WinRel\ICF.SBR \
	.\WinRel\IDO.SBR \
	.\WinRel\IEC.SBR \
	.\WinRel\IOIPAO.SBR \
	.\WinRel\IOIPO.SBR \
	.\WinRel\IOO.SBR \
	.\WinRel\IPS.SBR \
	.\WinRel\obj.sbr \
	.\WinRel\APP.SBR \
	.\WinRel\SIMPSVR.SBR \
	.\WinRel\SIMPUI.SBR

.\WinRel\simpsvr.bsc : $(OUTDIR)  $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib compob32.lib ole232.lib storag32.lib odbc32.lib /NOLOGO /STACK:0x16000 /SUBSYSTEM:windows /MACHINE:IX86
# ADD LINK32 user32.lib gdi32.lib kernel32.lib libc.lib ole32.lib uuid.lib /NOLOGO /BASE:0x400000 /STACK:0x16000 /MACHINE:IX86 /NODEFAULTLIB /SUBSYSTEM:windows,4.0
# SUBTRACT LINK32 /PDB:none
LINK32_FLAGS=user32.lib gdi32.lib kernel32.lib libc.lib ole32.lib uuid.lib\
 /NOLOGO /BASE:0x400000 /STACK:0x16000 /INCREMENTAL:no\
 /PDB:$(OUTDIR)/"simpsvr.pdb" /MACHINE:IX86 /NODEFAULTLIB\
 /OUT:$(OUTDIR)/"simpsvr.exe" /SUBSYSTEM:windows,4.0  
DEF_FILE=
LINK32_OBJS= \
	.\WinRel\DOC.OBJ \
	.\WinRel\ICF.OBJ \
	.\WinRel\IDO.OBJ \
	.\WinRel\IEC.OBJ \
	.\WinRel\IOIPAO.OBJ \
	.\WinRel\IOIPO.OBJ \
	.\WinRel\IOO.OBJ \
	.\WinRel\IPS.OBJ \
	.\WinRel\obj.obj \
	.\WinRel\APP.OBJ \
	.\WinRel\SIMPSVR.OBJ \
	.\WinRel\SIMPSVR.res \
	.\WinRel\SIMPUI.OBJ

.\WinRel\simpsvr.exe : $(OUTDIR)  $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "Win32 ANSI Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "WinDebug"
# PROP Intermediate_Dir "WinDebug"
OUTDIR=.\WinDebug
INTDIR=.\WinDebug

ALL : .\WinDebug\simpsvr.exe .\WinDebug\simpsvr.bsc

$(OUTDIR) : 
    if not exist $(OUTDIR)/nul mkdir $(OUTDIR)

# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
# ADD BASE CPP /nologo /Zp4 /W3 /Zi /YX /Od /D "_DEBUG" /D "_WINDOWS" /D "_X86_" /D "WIN32" /D "STRICT" /FR /c
# ADD CPP /nologo /Zp4 /W3 /Zi /Od /D "_DEBUG" /D "_WINDOWS" /D "_X86_" /D "WIN32" /D "STRICT" /FR /Yu"precomp.h" /c
CPP_PROJ=/nologo /Zp4 /W3 /Zi /Od /D "_DEBUG" /D "_WINDOWS" /D "_X86_" /D\
 "WIN32" /D "STRICT" /FR$(INTDIR)/ /Fp$(OUTDIR)/"simpsvr.pch" /Yu"precomp.h"\
 /Fo$(INTDIR)/ /Fd$(OUTDIR)/"simpsvr.pdb" /c 
CPP_OBJS=.\WinDebug/
# ADD BASE RSC /l 0x1 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
RSC_PROJ=/l 0x409 /fo$(INTDIR)/"SIMPSVR.res" /d "_DEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# SUBTRACT BASE BSC32 /Iu
# ADD BSC32 /nologo
# SUBTRACT BSC32 /Iu
BSC32_FLAGS=/nologo /o$(OUTDIR)/"simpsvr.bsc" 
BSC32_SBRS= \
	.\WinDebug\DOC.SBR \
	.\WinDebug\ICF.SBR \
	.\WinDebug\IDO.SBR \
	.\WinDebug\IEC.SBR \
	.\WinDebug\IOIPAO.SBR \
	.\WinDebug\IOIPO.SBR \
	.\WinDebug\IOO.SBR \
	.\WinDebug\IPS.SBR \
	.\WinDebug\obj.sbr \
	.\WinDebug\APP.SBR \
	.\WinDebug\SIMPSVR.SBR \
	.\WinDebug\SIMPUI.SBR

.\WinDebug\simpsvr.bsc : $(OUTDIR)  $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib compob32.lib ole232.lib storag32.lib odbc32.lib /NOLOGO /STACK:0x16000 /SUBSYSTEM:windows /DEBUG /MACHINE:IX86
# ADD LINK32 user32.lib gdi32.lib kernel32.lib libc.lib ole32.lib uuid.lib /NOLOGO /BASE:0x400000 /STACK:0x16000 /INCREMENTAL:no /DEBUG /MACHINE:IX86 /NODEFAULTLIB /SUBSYSTEM:windows,4.0
# SUBTRACT LINK32 /PDB:none
LINK32_FLAGS=user32.lib gdi32.lib kernel32.lib libc.lib ole32.lib uuid.lib\
 /NOLOGO /BASE:0x400000 /STACK:0x16000 /INCREMENTAL:no\
 /PDB:$(OUTDIR)/"simpsvr.pdb" /DEBUG /MACHINE:IX86 /NODEFAULTLIB\
 /OUT:$(OUTDIR)/"simpsvr.exe" /SUBSYSTEM:windows,4.0  
DEF_FILE=
LINK32_OBJS= \
	.\WinDebug\DOC.OBJ \
	.\WinDebug\ICF.OBJ \
	.\WinDebug\IDO.OBJ \
	.\WinDebug\IEC.OBJ \
	.\WinDebug\IOIPAO.OBJ \
	.\WinDebug\IOIPO.OBJ \
	.\WinDebug\IOO.OBJ \
	.\WinDebug\IPS.OBJ \
	.\WinDebug\obj.obj \
	.\WinDebug\APP.OBJ \
	.\WinDebug\SIMPSVR.OBJ \
	.\WinDebug\SIMPSVR.res \
	.\WinDebug\SIMPUI.OBJ

.\WinDebug\simpsvr.exe : $(OUTDIR)  $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "Win32 Unicode Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Win32_Un"
# PROP BASE Intermediate_Dir "Win32_Un"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "UniRel"
# PROP Intermediate_Dir "UniRel"
OUTDIR=.\UniRel
INTDIR=.\UniRel

ALL : .\UniRel\simpsvr.exe .\UniRel\simpsvr.bsc

$(OUTDIR) : 
    if not exist $(OUTDIR)/nul mkdir $(OUTDIR)

# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
# ADD BASE CPP /nologo /Zp4 /W3 /O2 /D "NDEBUG" /D "_WINDOWS" /D "_X86_" /D "WIN32" /D "STRICT" /FR /Yu"precomp.h" /c
# ADD CPP /nologo /Zp4 /W3 /O2 /D "NDEBUG" /D "_WINDOWS" /D "_X86_" /D "WIN32" /D "STRICT" /D "UNICODE" /FR /Yu"precomp.h" /c
CPP_PROJ=/nologo /Zp4 /W3 /O2 /D "NDEBUG" /D "_WINDOWS" /D "_X86_" /D "WIN32"\
 /D "STRICT" /D "UNICODE" /FR$(INTDIR)/ /Fp$(OUTDIR)/"simpsvr.pch"\
 /Yu"precomp.h" /Fo$(INTDIR)/ /c 
CPP_OBJS=.\UniRel/
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
RSC_PROJ=/l 0x409 /fo$(INTDIR)/"SIMPSVR.res" /d "NDEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# SUBTRACT BASE BSC32 /Iu
# ADD BSC32 /nologo
# SUBTRACT BSC32 /Iu
BSC32_FLAGS=/nologo /o$(OUTDIR)/"simpsvr.bsc" 
BSC32_SBRS= \
	.\UniRel\DOC.SBR \
	.\UniRel\ICF.SBR \
	.\UniRel\IDO.SBR \
	.\UniRel\IEC.SBR \
	.\UniRel\IOIPAO.SBR \
	.\UniRel\IOIPO.SBR \
	.\UniRel\IOO.SBR \
	.\UniRel\IPS.SBR \
	.\UniRel\obj.sbr \
	.\UniRel\APP.SBR \
	.\UniRel\SIMPSVR.SBR \
	.\UniRel\SIMPUI.SBR

.\UniRel\simpsvr.bsc : $(OUTDIR)  $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 user32.lib gdi32.lib kernel32.lib libc.lib ole32.lib uuid.lib /NOLOGO /BASE:0x400000 /STACK:0x16000 /SUBSYSTEM:windows /MACHINE:IX86 /NODEFAULTLIB
# ADD LINK32 user32.lib gdi32.lib kernel32.lib libc.lib ole32.lib uuid.lib /NOLOGO /BASE:0x400000 /STACK:0x16000 /MACHINE:IX86 /NODEFAULTLIB /SUBSYSTEM:windows,4.0
# SUBTRACT LINK32 /PDB:none
LINK32_FLAGS=user32.lib gdi32.lib kernel32.lib libc.lib ole32.lib uuid.lib\
 /NOLOGO /BASE:0x400000 /STACK:0x16000 /INCREMENTAL:no\
 /PDB:$(OUTDIR)/"simpsvr.pdb" /MACHINE:IX86 /NODEFAULTLIB\
 /OUT:$(OUTDIR)/"simpsvr.exe" /SUBSYSTEM:windows,4.0  
DEF_FILE=
LINK32_OBJS= \
	.\UniRel\DOC.OBJ \
	.\UniRel\ICF.OBJ \
	.\UniRel\IDO.OBJ \
	.\UniRel\IEC.OBJ \
	.\UniRel\IOIPAO.OBJ \
	.\UniRel\IOIPO.OBJ \
	.\UniRel\IOO.OBJ \
	.\UniRel\IPS.OBJ \
	.\UniRel\obj.obj \
	.\UniRel\APP.OBJ \
	.\UniRel\SIMPSVR.OBJ \
	.\UniRel\SIMPSVR.res \
	.\UniRel\SIMPUI.OBJ

.\UniRel\simpsvr.exe : $(OUTDIR)  $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "Win32 Unicode Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Win32_U0"
# PROP BASE Intermediate_Dir "Win32_U0"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "UniDebug"
# PROP Intermediate_Dir "UniDebug"
OUTDIR=.\UniDebug
INTDIR=.\UniDebug

ALL : .\UniDebug\simpsvr.exe .\UniDebug\simpsvr.bsc

$(OUTDIR) : 
    if not exist $(OUTDIR)/nul mkdir $(OUTDIR)

# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
# ADD BASE CPP /nologo /Zp4 /W3 /Zi /Od /D "_DEBUG" /D "_WINDOWS" /D "_X86_" /D "WIN32" /D "STRICT" /FR /Yu"precomp.h" /c
# ADD CPP /nologo /Zp4 /W3 /Zi /Od /D "_DEBUG" /D "_WINDOWS" /D "_X86_" /D "WIN32" /D "STRICT" /D "UNICODE" /FR /Yu"precomp.h" /c
CPP_PROJ=/nologo /Zp4 /W3 /Zi /Od /D "_DEBUG" /D "_WINDOWS" /D "_X86_" /D\
 "WIN32" /D "STRICT" /D "UNICODE" /FR$(INTDIR)/ /Fp$(OUTDIR)/"simpsvr.pch"\
 /Yu"precomp.h" /Fo$(INTDIR)/ /Fd$(OUTDIR)/"simpsvr.pdb" /c 
CPP_OBJS=.\UniDebug/
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
RSC_PROJ=/l 0x409 /fo$(INTDIR)/"SIMPSVR.res" /d "_DEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# SUBTRACT BASE BSC32 /Iu
# ADD BSC32 /nologo
# SUBTRACT BSC32 /Iu
BSC32_FLAGS=/nologo /o$(OUTDIR)/"simpsvr.bsc" 
BSC32_SBRS= \
	.\UniDebug\DOC.SBR \
	.\UniDebug\ICF.SBR \
	.\UniDebug\IDO.SBR \
	.\UniDebug\IEC.SBR \
	.\UniDebug\IOIPAO.SBR \
	.\UniDebug\IOIPO.SBR \
	.\UniDebug\IOO.SBR \
	.\UniDebug\IPS.SBR \
	.\UniDebug\obj.sbr \
	.\UniDebug\APP.SBR \
	.\UniDebug\SIMPSVR.SBR \
	.\UniDebug\SIMPUI.SBR

.\UniDebug\simpsvr.bsc : $(OUTDIR)  $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 user32.lib gdi32.lib kernel32.lib libc.lib ole32.lib uuid.lib /NOLOGO /BASE:0x400000 /STACK:0x16000 /SUBSYSTEM:windows /INCREMENTAL:no /DEBUG /MACHINE:IX86 /NODEFAULTLIB
# ADD LINK32 user32.lib gdi32.lib kernel32.lib libc.lib ole32.lib uuid.lib /NOLOGO /BASE:0x400000 /STACK:0x16000 /INCREMENTAL:no /DEBUG /MACHINE:IX86 /NODEFAULTLIB /SUBSYSTEM:windows,4.0
# SUBTRACT LINK32 /PDB:none
LINK32_FLAGS=user32.lib gdi32.lib kernel32.lib libc.lib ole32.lib uuid.lib\
 /NOLOGO /BASE:0x400000 /STACK:0x16000 /INCREMENTAL:no\
 /PDB:$(OUTDIR)/"simpsvr.pdb" /DEBUG /MACHINE:IX86 /NODEFAULTLIB\
 /OUT:$(OUTDIR)/"simpsvr.exe" /SUBSYSTEM:windows,4.0  
DEF_FILE=
LINK32_OBJS= \
	.\UniDebug\DOC.OBJ \
	.\UniDebug\ICF.OBJ \
	.\UniDebug\IDO.OBJ \
	.\UniDebug\IEC.OBJ \
	.\UniDebug\IOIPAO.OBJ \
	.\UniDebug\IOIPO.OBJ \
	.\UniDebug\IOO.OBJ \
	.\UniDebug\IPS.OBJ \
	.\UniDebug\obj.obj \
	.\UniDebug\APP.OBJ \
	.\UniDebug\SIMPSVR.OBJ \
	.\UniDebug\SIMPSVR.res \
	.\UniDebug\SIMPUI.OBJ

.\UniDebug\simpsvr.exe : $(OUTDIR)  $(DEF_FILE) $(LINK32_OBJS)
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

SOURCE=.\DOC.CPP
DEP_DOC_C=\
	.\PRECOMP.H\
	.\OBJ.H\
	.\APP.H\
	.\DOC.H\
	.\SIMPUI.H\
	.\SIMPSVR.H

!IF  "$(CFG)" == "Win32 ANSI Release"

# ADD BASE CPP /Yu"PRE.H"

.\WinRel\DOC.OBJ :  $(SOURCE)  $(DEP_DOC_C) $(INTDIR) .\WinRel\SIMPSVR.OBJ
   $(CPP) /nologo /Zp4 /W3 /O2 /D "NDEBUG" /D "_WINDOWS" /D "_X86_" /D "WIN32"\
 /D "STRICT" /FR$(INTDIR)/ /Fp$(OUTDIR)/"simpsvr.pch" /Yu"precomp.h"\
 /Fo$(INTDIR)/ /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 ANSI Debug"

# ADD BASE CPP /Yu"PRE.H"

.\WinDebug\DOC.OBJ :  $(SOURCE)  $(DEP_DOC_C) $(INTDIR) .\WinDebug\SIMPSVR.OBJ
   $(CPP) /nologo /Zp4 /W3 /Zi /Od /D "_DEBUG" /D "_WINDOWS" /D "_X86_" /D\
 "WIN32" /D "STRICT" /FR$(INTDIR)/ /Fp$(OUTDIR)/"simpsvr.pch" /Yu"precomp.h"\
 /Fo$(INTDIR)/ /Fd$(OUTDIR)/"simpsvr.pdb" /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 Unicode Release"

.\UniRel\DOC.OBJ :  $(SOURCE)  $(DEP_DOC_C) $(INTDIR) .\UniRel\SIMPSVR.OBJ
   $(CPP) /nologo /Zp4 /W3 /O2 /D "NDEBUG" /D "_WINDOWS" /D "_X86_" /D "WIN32"\
 /D "STRICT" /D "UNICODE" /FR$(INTDIR)/ /Fp$(OUTDIR)/"simpsvr.pch"\
 /Yu"precomp.h" /Fo$(INTDIR)/ /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 Unicode Debug"

.\UniDebug\DOC.OBJ :  $(SOURCE)  $(DEP_DOC_C) $(INTDIR) .\UniDebug\SIMPSVR.OBJ
   $(CPP) /nologo /Zp4 /W3 /Zi /Od /D "_DEBUG" /D "_WINDOWS" /D "_X86_" /D\
 "WIN32" /D "STRICT" /D "UNICODE" /FR$(INTDIR)/ /Fp$(OUTDIR)/"simpsvr.pch"\
 /Yu"precomp.h" /Fo$(INTDIR)/ /Fd$(OUTDIR)/"simpsvr.pdb" /c  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\ICF.CPP
DEP_ICF_C=\
	.\PRECOMP.H\
	.\APP.H\
	.\DOC.H\
	.\ICF.H\
	.\SIMPUI.H\
	.\SIMPSVR.H

!IF  "$(CFG)" == "Win32 ANSI Release"

# ADD BASE CPP /Yu"PRE.H"

.\WinRel\ICF.OBJ :  $(SOURCE)  $(DEP_ICF_C) $(INTDIR) .\WinRel\SIMPSVR.OBJ
   $(CPP) /nologo /Zp4 /W3 /O2 /D "NDEBUG" /D "_WINDOWS" /D "_X86_" /D "WIN32"\
 /D "STRICT" /FR$(INTDIR)/ /Fp$(OUTDIR)/"simpsvr.pch" /Yu"precomp.h"\
 /Fo$(INTDIR)/ /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 ANSI Debug"

# ADD BASE CPP /Yu"PRE.H"

.\WinDebug\ICF.OBJ :  $(SOURCE)  $(DEP_ICF_C) $(INTDIR) .\WinDebug\SIMPSVR.OBJ
   $(CPP) /nologo /Zp4 /W3 /Zi /Od /D "_DEBUG" /D "_WINDOWS" /D "_X86_" /D\
 "WIN32" /D "STRICT" /FR$(INTDIR)/ /Fp$(OUTDIR)/"simpsvr.pch" /Yu"precomp.h"\
 /Fo$(INTDIR)/ /Fd$(OUTDIR)/"simpsvr.pdb" /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 Unicode Release"

.\UniRel\ICF.OBJ :  $(SOURCE)  $(DEP_ICF_C) $(INTDIR) .\UniRel\SIMPSVR.OBJ
   $(CPP) /nologo /Zp4 /W3 /O2 /D "NDEBUG" /D "_WINDOWS" /D "_X86_" /D "WIN32"\
 /D "STRICT" /D "UNICODE" /FR$(INTDIR)/ /Fp$(OUTDIR)/"simpsvr.pch"\
 /Yu"precomp.h" /Fo$(INTDIR)/ /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 Unicode Debug"

.\UniDebug\ICF.OBJ :  $(SOURCE)  $(DEP_ICF_C) $(INTDIR) .\UniDebug\SIMPSVR.OBJ
   $(CPP) /nologo /Zp4 /W3 /Zi /Od /D "_DEBUG" /D "_WINDOWS" /D "_X86_" /D\
 "WIN32" /D "STRICT" /D "UNICODE" /FR$(INTDIR)/ /Fp$(OUTDIR)/"simpsvr.pch"\
 /Yu"precomp.h" /Fo$(INTDIR)/ /Fd$(OUTDIR)/"simpsvr.pdb" /c  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\IDO.CPP
DEP_IDO_C=\
	.\PRECOMP.H\
	.\OBJ.H\
	.\APP.H\
	.\DOC.H\
	.\SIMPUI.H\
	.\SIMPSVR.H

!IF  "$(CFG)" == "Win32 ANSI Release"

# ADD BASE CPP /Yu"PRE.H"

.\WinRel\IDO.OBJ :  $(SOURCE)  $(DEP_IDO_C) $(INTDIR) .\WinRel\SIMPSVR.OBJ
   $(CPP) /nologo /Zp4 /W3 /O2 /D "NDEBUG" /D "_WINDOWS" /D "_X86_" /D "WIN32"\
 /D "STRICT" /FR$(INTDIR)/ /Fp$(OUTDIR)/"simpsvr.pch" /Yu"precomp.h"\
 /Fo$(INTDIR)/ /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 ANSI Debug"

# ADD BASE CPP /Yu"PRE.H"

.\WinDebug\IDO.OBJ :  $(SOURCE)  $(DEP_IDO_C) $(INTDIR) .\WinDebug\SIMPSVR.OBJ
   $(CPP) /nologo /Zp4 /W3 /Zi /Od /D "_DEBUG" /D "_WINDOWS" /D "_X86_" /D\
 "WIN32" /D "STRICT" /FR$(INTDIR)/ /Fp$(OUTDIR)/"simpsvr.pch" /Yu"precomp.h"\
 /Fo$(INTDIR)/ /Fd$(OUTDIR)/"simpsvr.pdb" /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 Unicode Release"

.\UniRel\IDO.OBJ :  $(SOURCE)  $(DEP_IDO_C) $(INTDIR) .\UniRel\SIMPSVR.OBJ
   $(CPP) /nologo /Zp4 /W3 /O2 /D "NDEBUG" /D "_WINDOWS" /D "_X86_" /D "WIN32"\
 /D "STRICT" /D "UNICODE" /FR$(INTDIR)/ /Fp$(OUTDIR)/"simpsvr.pch"\
 /Yu"precomp.h" /Fo$(INTDIR)/ /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 Unicode Debug"

.\UniDebug\IDO.OBJ :  $(SOURCE)  $(DEP_IDO_C) $(INTDIR) .\UniDebug\SIMPSVR.OBJ
   $(CPP) /nologo /Zp4 /W3 /Zi /Od /D "_DEBUG" /D "_WINDOWS" /D "_X86_" /D\
 "WIN32" /D "STRICT" /D "UNICODE" /FR$(INTDIR)/ /Fp$(OUTDIR)/"simpsvr.pch"\
 /Yu"precomp.h" /Fo$(INTDIR)/ /Fd$(OUTDIR)/"simpsvr.pdb" /c  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\IEC.CPP
DEP_IEC_C=\
	.\PRECOMP.H\
	.\OBJ.H\
	.\APP.H\
	.\DOC.H\
	.\SIMPUI.H\
	.\SIMPSVR.H

!IF  "$(CFG)" == "Win32 ANSI Release"

# ADD BASE CPP /Yu"PRE.H"

.\WinRel\IEC.OBJ :  $(SOURCE)  $(DEP_IEC_C) $(INTDIR) .\WinRel\SIMPSVR.OBJ
   $(CPP) /nologo /Zp4 /W3 /O2 /D "NDEBUG" /D "_WINDOWS" /D "_X86_" /D "WIN32"\
 /D "STRICT" /FR$(INTDIR)/ /Fp$(OUTDIR)/"simpsvr.pch" /Yu"precomp.h"\
 /Fo$(INTDIR)/ /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 ANSI Debug"

# ADD BASE CPP /Yu"PRE.H"

.\WinDebug\IEC.OBJ :  $(SOURCE)  $(DEP_IEC_C) $(INTDIR) .\WinDebug\SIMPSVR.OBJ
   $(CPP) /nologo /Zp4 /W3 /Zi /Od /D "_DEBUG" /D "_WINDOWS" /D "_X86_" /D\
 "WIN32" /D "STRICT" /FR$(INTDIR)/ /Fp$(OUTDIR)/"simpsvr.pch" /Yu"precomp.h"\
 /Fo$(INTDIR)/ /Fd$(OUTDIR)/"simpsvr.pdb" /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 Unicode Release"

.\UniRel\IEC.OBJ :  $(SOURCE)  $(DEP_IEC_C) $(INTDIR) .\UniRel\SIMPSVR.OBJ
   $(CPP) /nologo /Zp4 /W3 /O2 /D "NDEBUG" /D "_WINDOWS" /D "_X86_" /D "WIN32"\
 /D "STRICT" /D "UNICODE" /FR$(INTDIR)/ /Fp$(OUTDIR)/"simpsvr.pch"\
 /Yu"precomp.h" /Fo$(INTDIR)/ /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 Unicode Debug"

.\UniDebug\IEC.OBJ :  $(SOURCE)  $(DEP_IEC_C) $(INTDIR) .\UniDebug\SIMPSVR.OBJ
   $(CPP) /nologo /Zp4 /W3 /Zi /Od /D "_DEBUG" /D "_WINDOWS" /D "_X86_" /D\
 "WIN32" /D "STRICT" /D "UNICODE" /FR$(INTDIR)/ /Fp$(OUTDIR)/"simpsvr.pch"\
 /Yu"precomp.h" /Fo$(INTDIR)/ /Fd$(OUTDIR)/"simpsvr.pdb" /c  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\IOIPAO.CPP
DEP_IOIPA=\
	.\PRECOMP.H\
	.\OBJ.H\
	.\APP.H\
	.\DOC.H\
	.\SIMPUI.H\
	.\SIMPSVR.H

!IF  "$(CFG)" == "Win32 ANSI Release"

# ADD BASE CPP /Yu"PRE.H"

.\WinRel\IOIPAO.OBJ :  $(SOURCE)  $(DEP_IOIPA) $(INTDIR) .\WinRel\SIMPSVR.OBJ
   $(CPP) /nologo /Zp4 /W3 /O2 /D "NDEBUG" /D "_WINDOWS" /D "_X86_" /D "WIN32"\
 /D "STRICT" /FR$(INTDIR)/ /Fp$(OUTDIR)/"simpsvr.pch" /Yu"precomp.h"\
 /Fo$(INTDIR)/ /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 ANSI Debug"

# ADD BASE CPP /Yu"PRE.H"

.\WinDebug\IOIPAO.OBJ :  $(SOURCE)  $(DEP_IOIPA) $(INTDIR)\
 .\WinDebug\SIMPSVR.OBJ
   $(CPP) /nologo /Zp4 /W3 /Zi /Od /D "_DEBUG" /D "_WINDOWS" /D "_X86_" /D\
 "WIN32" /D "STRICT" /FR$(INTDIR)/ /Fp$(OUTDIR)/"simpsvr.pch" /Yu"precomp.h"\
 /Fo$(INTDIR)/ /Fd$(OUTDIR)/"simpsvr.pdb" /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 Unicode Release"

.\UniRel\IOIPAO.OBJ :  $(SOURCE)  $(DEP_IOIPA) $(INTDIR) .\UniRel\SIMPSVR.OBJ
   $(CPP) /nologo /Zp4 /W3 /O2 /D "NDEBUG" /D "_WINDOWS" /D "_X86_" /D "WIN32"\
 /D "STRICT" /D "UNICODE" /FR$(INTDIR)/ /Fp$(OUTDIR)/"simpsvr.pch"\
 /Yu"precomp.h" /Fo$(INTDIR)/ /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 Unicode Debug"

.\UniDebug\IOIPAO.OBJ :  $(SOURCE)  $(DEP_IOIPA) $(INTDIR)\
 .\UniDebug\SIMPSVR.OBJ
   $(CPP) /nologo /Zp4 /W3 /Zi /Od /D "_DEBUG" /D "_WINDOWS" /D "_X86_" /D\
 "WIN32" /D "STRICT" /D "UNICODE" /FR$(INTDIR)/ /Fp$(OUTDIR)/"simpsvr.pch"\
 /Yu"precomp.h" /Fo$(INTDIR)/ /Fd$(OUTDIR)/"simpsvr.pdb" /c  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\IOIPO.CPP
DEP_IOIPO=\
	.\PRECOMP.H\
	.\OBJ.H\
	.\APP.H\
	.\DOC.H\
	.\SIMPUI.H\
	.\SIMPSVR.H

!IF  "$(CFG)" == "Win32 ANSI Release"

# ADD BASE CPP /Yu"PRE.H"

.\WinRel\IOIPO.OBJ :  $(SOURCE)  $(DEP_IOIPO) $(INTDIR) .\WinRel\SIMPSVR.OBJ
   $(CPP) /nologo /Zp4 /W3 /O2 /D "NDEBUG" /D "_WINDOWS" /D "_X86_" /D "WIN32"\
 /D "STRICT" /FR$(INTDIR)/ /Fp$(OUTDIR)/"simpsvr.pch" /Yu"precomp.h"\
 /Fo$(INTDIR)/ /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 ANSI Debug"

# ADD BASE CPP /Yu"PRE.H"

.\WinDebug\IOIPO.OBJ :  $(SOURCE)  $(DEP_IOIPO) $(INTDIR)\
 .\WinDebug\SIMPSVR.OBJ
   $(CPP) /nologo /Zp4 /W3 /Zi /Od /D "_DEBUG" /D "_WINDOWS" /D "_X86_" /D\
 "WIN32" /D "STRICT" /FR$(INTDIR)/ /Fp$(OUTDIR)/"simpsvr.pch" /Yu"precomp.h"\
 /Fo$(INTDIR)/ /Fd$(OUTDIR)/"simpsvr.pdb" /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 Unicode Release"

.\UniRel\IOIPO.OBJ :  $(SOURCE)  $(DEP_IOIPO) $(INTDIR) .\UniRel\SIMPSVR.OBJ
   $(CPP) /nologo /Zp4 /W3 /O2 /D "NDEBUG" /D "_WINDOWS" /D "_X86_" /D "WIN32"\
 /D "STRICT" /D "UNICODE" /FR$(INTDIR)/ /Fp$(OUTDIR)/"simpsvr.pch"\
 /Yu"precomp.h" /Fo$(INTDIR)/ /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 Unicode Debug"

.\UniDebug\IOIPO.OBJ :  $(SOURCE)  $(DEP_IOIPO) $(INTDIR)\
 .\UniDebug\SIMPSVR.OBJ
   $(CPP) /nologo /Zp4 /W3 /Zi /Od /D "_DEBUG" /D "_WINDOWS" /D "_X86_" /D\
 "WIN32" /D "STRICT" /D "UNICODE" /FR$(INTDIR)/ /Fp$(OUTDIR)/"simpsvr.pch"\
 /Yu"precomp.h" /Fo$(INTDIR)/ /Fd$(OUTDIR)/"simpsvr.pdb" /c  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\IOO.CPP
DEP_IOO_C=\
	.\PRECOMP.H\
	.\OBJ.H\
	.\APP.H\
	.\DOC.H\
	.\SIMPUI.H\
	.\SIMPSVR.H

!IF  "$(CFG)" == "Win32 ANSI Release"

# ADD BASE CPP /Yu"PRE.H"

.\WinRel\IOO.OBJ :  $(SOURCE)  $(DEP_IOO_C) $(INTDIR) .\WinRel\SIMPSVR.OBJ
   $(CPP) /nologo /Zp4 /W3 /O2 /D "NDEBUG" /D "_WINDOWS" /D "_X86_" /D "WIN32"\
 /D "STRICT" /FR$(INTDIR)/ /Fp$(OUTDIR)/"simpsvr.pch" /Yu"precomp.h"\
 /Fo$(INTDIR)/ /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 ANSI Debug"

# ADD BASE CPP /Yu"PRE.H"

.\WinDebug\IOO.OBJ :  $(SOURCE)  $(DEP_IOO_C) $(INTDIR) .\WinDebug\SIMPSVR.OBJ
   $(CPP) /nologo /Zp4 /W3 /Zi /Od /D "_DEBUG" /D "_WINDOWS" /D "_X86_" /D\
 "WIN32" /D "STRICT" /FR$(INTDIR)/ /Fp$(OUTDIR)/"simpsvr.pch" /Yu"precomp.h"\
 /Fo$(INTDIR)/ /Fd$(OUTDIR)/"simpsvr.pdb" /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 Unicode Release"

.\UniRel\IOO.OBJ :  $(SOURCE)  $(DEP_IOO_C) $(INTDIR) .\UniRel\SIMPSVR.OBJ
   $(CPP) /nologo /Zp4 /W3 /O2 /D "NDEBUG" /D "_WINDOWS" /D "_X86_" /D "WIN32"\
 /D "STRICT" /D "UNICODE" /FR$(INTDIR)/ /Fp$(OUTDIR)/"simpsvr.pch"\
 /Yu"precomp.h" /Fo$(INTDIR)/ /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 Unicode Debug"

.\UniDebug\IOO.OBJ :  $(SOURCE)  $(DEP_IOO_C) $(INTDIR) .\UniDebug\SIMPSVR.OBJ
   $(CPP) /nologo /Zp4 /W3 /Zi /Od /D "_DEBUG" /D "_WINDOWS" /D "_X86_" /D\
 "WIN32" /D "STRICT" /D "UNICODE" /FR$(INTDIR)/ /Fp$(OUTDIR)/"simpsvr.pch"\
 /Yu"precomp.h" /Fo$(INTDIR)/ /Fd$(OUTDIR)/"simpsvr.pdb" /c  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\IPS.CPP
DEP_IPS_C=\
	.\PRECOMP.H\
	.\OBJ.H\
	.\APP.H\
	.\DOC.H\
	.\SIMPUI.H\
	.\SIMPSVR.H

!IF  "$(CFG)" == "Win32 ANSI Release"

# ADD BASE CPP /Yu"PRE.H"

.\WinRel\IPS.OBJ :  $(SOURCE)  $(DEP_IPS_C) $(INTDIR) .\WinRel\SIMPSVR.OBJ
   $(CPP) /nologo /Zp4 /W3 /O2 /D "NDEBUG" /D "_WINDOWS" /D "_X86_" /D "WIN32"\
 /D "STRICT" /FR$(INTDIR)/ /Fp$(OUTDIR)/"simpsvr.pch" /Yu"precomp.h"\
 /Fo$(INTDIR)/ /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 ANSI Debug"

# ADD BASE CPP /Yu"PRE.H"

.\WinDebug\IPS.OBJ :  $(SOURCE)  $(DEP_IPS_C) $(INTDIR) .\WinDebug\SIMPSVR.OBJ
   $(CPP) /nologo /Zp4 /W3 /Zi /Od /D "_DEBUG" /D "_WINDOWS" /D "_X86_" /D\
 "WIN32" /D "STRICT" /FR$(INTDIR)/ /Fp$(OUTDIR)/"simpsvr.pch" /Yu"precomp.h"\
 /Fo$(INTDIR)/ /Fd$(OUTDIR)/"simpsvr.pdb" /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 Unicode Release"

.\UniRel\IPS.OBJ :  $(SOURCE)  $(DEP_IPS_C) $(INTDIR) .\UniRel\SIMPSVR.OBJ
   $(CPP) /nologo /Zp4 /W3 /O2 /D "NDEBUG" /D "_WINDOWS" /D "_X86_" /D "WIN32"\
 /D "STRICT" /D "UNICODE" /FR$(INTDIR)/ /Fp$(OUTDIR)/"simpsvr.pch"\
 /Yu"precomp.h" /Fo$(INTDIR)/ /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 Unicode Debug"

.\UniDebug\IPS.OBJ :  $(SOURCE)  $(DEP_IPS_C) $(INTDIR) .\UniDebug\SIMPSVR.OBJ
   $(CPP) /nologo /Zp4 /W3 /Zi /Od /D "_DEBUG" /D "_WINDOWS" /D "_X86_" /D\
 "WIN32" /D "STRICT" /D "UNICODE" /FR$(INTDIR)/ /Fp$(OUTDIR)/"simpsvr.pch"\
 /Yu"precomp.h" /Fo$(INTDIR)/ /Fd$(OUTDIR)/"simpsvr.pdb" /c  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\obj.cpp
DEP_OBJ_C=\
	.\PRECOMP.H\
	.\OBJ.H\
	.\APP.H\
	.\DOC.H\
	.\SIMPUI.H\
	.\SIMPSVR.H

!IF  "$(CFG)" == "Win32 ANSI Release"

# ADD BASE CPP /Yu"PRE.H"

.\WinRel\obj.obj :  $(SOURCE)  $(DEP_OBJ_C) $(INTDIR) .\WinRel\SIMPSVR.OBJ
   $(CPP) /nologo /Zp4 /W3 /O2 /D "NDEBUG" /D "_WINDOWS" /D "_X86_" /D "WIN32"\
 /D "STRICT" /FR$(INTDIR)/ /Fp$(OUTDIR)/"simpsvr.pch" /Yu"precomp.h"\
 /Fo$(INTDIR)/ /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 ANSI Debug"

# ADD BASE CPP /Yu"PRE.H"

.\WinDebug\obj.obj :  $(SOURCE)  $(DEP_OBJ_C) $(INTDIR) .\WinDebug\SIMPSVR.OBJ
   $(CPP) /nologo /Zp4 /W3 /Zi /Od /D "_DEBUG" /D "_WINDOWS" /D "_X86_" /D\
 "WIN32" /D "STRICT" /FR$(INTDIR)/ /Fp$(OUTDIR)/"simpsvr.pch" /Yu"precomp.h"\
 /Fo$(INTDIR)/ /Fd$(OUTDIR)/"simpsvr.pdb" /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 Unicode Release"

.\UniRel\obj.obj :  $(SOURCE)  $(DEP_OBJ_C) $(INTDIR) .\UniRel\SIMPSVR.OBJ
   $(CPP) /nologo /Zp4 /W3 /O2 /D "NDEBUG" /D "_WINDOWS" /D "_X86_" /D "WIN32"\
 /D "STRICT" /D "UNICODE" /FR$(INTDIR)/ /Fp$(OUTDIR)/"simpsvr.pch"\
 /Yu"precomp.h" /Fo$(INTDIR)/ /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 Unicode Debug"

.\UniDebug\obj.obj :  $(SOURCE)  $(DEP_OBJ_C) $(INTDIR) .\UniDebug\SIMPSVR.OBJ
   $(CPP) /nologo /Zp4 /W3 /Zi /Od /D "_DEBUG" /D "_WINDOWS" /D "_X86_" /D\
 "WIN32" /D "STRICT" /D "UNICODE" /FR$(INTDIR)/ /Fp$(OUTDIR)/"simpsvr.pch"\
 /Yu"precomp.h" /Fo$(INTDIR)/ /Fd$(OUTDIR)/"simpsvr.pdb" /c  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\APP.CPP
DEP_APP_C=\
	.\PRECOMP.H\
	.\OBJ.H\
	.\APP.H\
	.\DOC.H\
	.\ICF.H\
	.\SIMPUI.H\
	.\SIMPSVR.H

!IF  "$(CFG)" == "Win32 ANSI Release"

# ADD BASE CPP /Yu"PRE.H"

.\WinRel\APP.OBJ :  $(SOURCE)  $(DEP_APP_C) $(INTDIR) .\WinRel\SIMPSVR.OBJ
   $(CPP) /nologo /Zp4 /W3 /O2 /D "NDEBUG" /D "_WINDOWS" /D "_X86_" /D "WIN32"\
 /D "STRICT" /FR$(INTDIR)/ /Fp$(OUTDIR)/"simpsvr.pch" /Yu"precomp.h"\
 /Fo$(INTDIR)/ /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 ANSI Debug"

# ADD BASE CPP /Yu"PRE.H"

.\WinDebug\APP.OBJ :  $(SOURCE)  $(DEP_APP_C) $(INTDIR) .\WinDebug\SIMPSVR.OBJ
   $(CPP) /nologo /Zp4 /W3 /Zi /Od /D "_DEBUG" /D "_WINDOWS" /D "_X86_" /D\
 "WIN32" /D "STRICT" /FR$(INTDIR)/ /Fp$(OUTDIR)/"simpsvr.pch" /Yu"precomp.h"\
 /Fo$(INTDIR)/ /Fd$(OUTDIR)/"simpsvr.pdb" /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 Unicode Release"

.\UniRel\APP.OBJ :  $(SOURCE)  $(DEP_APP_C) $(INTDIR) .\UniRel\SIMPSVR.OBJ
   $(CPP) /nologo /Zp4 /W3 /O2 /D "NDEBUG" /D "_WINDOWS" /D "_X86_" /D "WIN32"\
 /D "STRICT" /D "UNICODE" /FR$(INTDIR)/ /Fp$(OUTDIR)/"simpsvr.pch"\
 /Yu"precomp.h" /Fo$(INTDIR)/ /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 Unicode Debug"

.\UniDebug\APP.OBJ :  $(SOURCE)  $(DEP_APP_C) $(INTDIR) .\UniDebug\SIMPSVR.OBJ
   $(CPP) /nologo /Zp4 /W3 /Zi /Od /D "_DEBUG" /D "_WINDOWS" /D "_X86_" /D\
 "WIN32" /D "STRICT" /D "UNICODE" /FR$(INTDIR)/ /Fp$(OUTDIR)/"simpsvr.pch"\
 /Yu"precomp.h" /Fo$(INTDIR)/ /Fd$(OUTDIR)/"simpsvr.pdb" /c  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\SIMPSVR.CPP
DEP_SIMPS=\
	.\PRECOMP.H\
	.\OBJ.H\
	.\APP.H\
	.\DOC.H\
	.\ICF.H\
	.\SIMPUI.H\
	.\SIMPSVR.H

!IF  "$(CFG)" == "Win32 ANSI Release"

# ADD BASE CPP /Yu"PRE.H"
# ADD CPP /Yc"precomp.h"

.\WinRel\SIMPSVR.OBJ :  $(SOURCE)  $(DEP_SIMPS) $(INTDIR)
   $(CPP) /nologo /Zp4 /W3 /O2 /D "NDEBUG" /D "_WINDOWS" /D "_X86_" /D "WIN32"\
 /D "STRICT" /FR$(INTDIR)/ /Fp$(OUTDIR)/"simpsvr.pch" /Yc"precomp.h"\
 /Fo$(INTDIR)/ /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 ANSI Debug"

# ADD BASE CPP /Yu"PRE.H"
# ADD CPP /Yc"precomp.h"

.\WinDebug\SIMPSVR.OBJ :  $(SOURCE)  $(DEP_SIMPS) $(INTDIR)
   $(CPP) /nologo /Zp4 /W3 /Zi /Od /D "_DEBUG" /D "_WINDOWS" /D "_X86_" /D\
 "WIN32" /D "STRICT" /FR$(INTDIR)/ /Fp$(OUTDIR)/"simpsvr.pch" /Yc"precomp.h"\
 /Fo$(INTDIR)/ /Fd$(OUTDIR)/"simpsvr.pdb" /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 Unicode Release"

# ADD BASE CPP /Yc"precomp.h"
# ADD CPP /Yc"precomp.h"

.\UniRel\SIMPSVR.OBJ :  $(SOURCE)  $(DEP_SIMPS) $(INTDIR)
   $(CPP) /nologo /Zp4 /W3 /O2 /D "NDEBUG" /D "_WINDOWS" /D "_X86_" /D "WIN32"\
 /D "STRICT" /D "UNICODE" /FR$(INTDIR)/ /Fp$(OUTDIR)/"simpsvr.pch"\
 /Yc"precomp.h" /Fo$(INTDIR)/ /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 Unicode Debug"

# ADD BASE CPP /Yc"precomp.h"
# ADD CPP /Yc"precomp.h"

.\UniDebug\SIMPSVR.OBJ :  $(SOURCE)  $(DEP_SIMPS) $(INTDIR)
   $(CPP) /nologo /Zp4 /W3 /Zi /Od /D "_DEBUG" /D "_WINDOWS" /D "_X86_" /D\
 "WIN32" /D "STRICT" /D "UNICODE" /FR$(INTDIR)/ /Fp$(OUTDIR)/"simpsvr.pch"\
 /Yc"precomp.h" /Fo$(INTDIR)/ /Fd$(OUTDIR)/"simpsvr.pdb" /c  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\SIMPSVR.RC
DEP_SIMPSV=\
	.\simpsvr.ico\
	.\SIMPSVR.H

!IF  "$(CFG)" == "Win32 ANSI Release"

# ADD BASE RSC /l 0x409
# ADD RSC /l 0x0

.\WinRel\SIMPSVR.res :  $(SOURCE)  $(DEP_SIMPSV) $(INTDIR)
   $(RSC) /l 0x0 /fo$(INTDIR)/"SIMPSVR.res" /d "NDEBUG"  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 ANSI Debug"

# ADD BASE RSC /l 0x409
# ADD RSC /l 0x0

.\WinDebug\SIMPSVR.res :  $(SOURCE)  $(DEP_SIMPSV) $(INTDIR)
   $(RSC) /l 0x0 /fo$(INTDIR)/"SIMPSVR.res" /d "_DEBUG"  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 Unicode Release"

# ADD BASE RSC /l 0x0
# ADD RSC /l 0x0

.\UniRel\SIMPSVR.res :  $(SOURCE)  $(DEP_SIMPSV) $(INTDIR)
   $(RSC) /l 0x0 /fo$(INTDIR)/"SIMPSVR.res" /d "NDEBUG"  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 Unicode Debug"

# ADD BASE RSC /l 0x0
# ADD RSC /l 0x0

.\UniDebug\SIMPSVR.res :  $(SOURCE)  $(DEP_SIMPSV) $(INTDIR)
   $(RSC) /l 0x0 /fo$(INTDIR)/"SIMPSVR.res" /d "_DEBUG"  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\SIMPUI.CPP
DEP_SIMPU=\
	.\PRECOMP.H\
	.\SIMPUI.H\
	.\SIMPSVR.H

!IF  "$(CFG)" == "Win32 ANSI Release"

.\WinRel\SIMPUI.OBJ :  $(SOURCE)  $(DEP_SIMPU) $(INTDIR) .\WinRel\SIMPSVR.OBJ

!ELSEIF  "$(CFG)" == "Win32 ANSI Debug"

.\WinDebug\SIMPUI.OBJ :  $(SOURCE)  $(DEP_SIMPU) $(INTDIR)\
 .\WinDebug\SIMPSVR.OBJ

!ELSEIF  "$(CFG)" == "Win32 Unicode Release"

.\UniRel\SIMPUI.OBJ :  $(SOURCE)  $(DEP_SIMPU) $(INTDIR) .\UniRel\SIMPSVR.OBJ

!ELSEIF  "$(CFG)" == "Win32 Unicode Debug"

.\UniDebug\SIMPUI.OBJ :  $(SOURCE)  $(DEP_SIMPU) $(INTDIR)\
 .\UniDebug\SIMPSVR.OBJ

!ENDIF 

# End Source File
# End Group
# End Project
################################################################################
