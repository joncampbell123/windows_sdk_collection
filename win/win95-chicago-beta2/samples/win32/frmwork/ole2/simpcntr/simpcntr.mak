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
!MESSAGE NMAKE /f "simpcntr.mak" CFG="Win32 ANSI Debug"
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

ALL : $(OUTDIR)/simpcntr.exe $(OUTDIR)/simpcntr.bsc

$(OUTDIR) : 
    if not exist $(OUTDIR)/nul mkdir $(OUTDIR)

# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
# ADD BASE CPP /nologo /Zp4 /W3 /YX /O2 /D "NDEBUG" /D "_WINDOWS" /D "_X86_" /D "WIN32" /D "STRICT" /FR /c
# ADD CPP /nologo /Zp4 /W3 /O2 /D "NDEBUG" /D "_WINDOWS" /D "_X86_" /D "WIN32" /D "STRICT" /FR /Yu"precomp.h" /c
CPP_PROJ=/nologo /Zp4 /W3 /O2 /D "NDEBUG" /D "_WINDOWS" /D "_X86_" /D "WIN32"\
 /D "STRICT" /FR$(INTDIR)/ /Fp$(OUTDIR)/"simpcntr.pch" /Yu"precomp.h"\
 /Fo$(INTDIR)/ /c 
CPP_OBJS=.\WinRel/
# ADD BASE RSC /l 0x1 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
RSC_PROJ=/l 0x409 /fo$(INTDIR)/"SIMPCNTR.res" /d "NDEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# SUBTRACT BASE BSC32 /Iu
# ADD BSC32 /nologo
# SUBTRACT BSC32 /Iu
BSC32_FLAGS=/nologo /o$(OUTDIR)/"simpcntr.bsc" 
BSC32_SBRS= \
	$(INTDIR)/DOC.SBR \
	$(INTDIR)/IAS.SBR \
	$(INTDIR)/IOCS.SBR \
	$(INTDIR)/IOIPF.SBR \
	$(INTDIR)/IOIPS.SBR \
	$(INTDIR)/APP.SBR \
	$(INTDIR)/SIMPCNTR.SBR \
	$(INTDIR)/SITE.SBR

$(OUTDIR)/simpcntr.bsc : $(OUTDIR)  $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib compob32.lib ole232.lib storag32.lib odbc32.lib /NOLOGO /STACK:0x16000 /SUBSYSTEM:windows /MACHINE:IX86
# ADD LINK32 mfcuia32.lib user32.lib gdi32.lib kernel32.lib shell32.lib ole32.lib uuid.lib /NOLOGO /BASE:0x400000 /STACK:0x16000 /MACHINE:IX86 /SUBSYSTEM:windows,4.0
# SUBTRACT LINK32 /PDB:none
LINK32_FLAGS=mfcuia32.lib user32.lib gdi32.lib kernel32.lib shell32.lib\
 ole32.lib uuid.lib /NOLOGO /BASE:0x400000 /STACK:0x16000 /INCREMENTAL:no\
 /PDB:$(OUTDIR)/"simpcntr.pdb" /MACHINE:IX86 /OUT:$(OUTDIR)/"simpcntr.exe"\
 /SUBSYSTEM:windows,4.0  
DEF_FILE=
LINK32_OBJS= \
	$(INTDIR)/DOC.OBJ \
	$(INTDIR)/IAS.OBJ \
	$(INTDIR)/IOCS.OBJ \
	$(INTDIR)/IOIPF.OBJ \
	$(INTDIR)/IOIPS.OBJ \
	$(INTDIR)/APP.OBJ \
	$(INTDIR)/SIMPCNTR.OBJ \
	$(INTDIR)/SITE.OBJ \
	$(INTDIR)/SIMPCNTR.res

$(OUTDIR)/simpcntr.exe : $(OUTDIR)  $(DEF_FILE) $(LINK32_OBJS)
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

ALL : $(OUTDIR)/simpcntr.exe $(OUTDIR)/simpcntr.bsc

$(OUTDIR) : 
    if not exist $(OUTDIR)/nul mkdir $(OUTDIR)

# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
# ADD BASE CPP /nologo /Zp4 /W3 /Zi /YX /Od /D "_DEBUG" /D "_WINDOWS" /D "_X86_" /D "WIN32" /D "STRICT" /FR /c
# ADD CPP /nologo /Zp4 /W3 /Zi /Od /D "_DEBUG" /D "_WINDOWS" /D "_X86_" /D "WIN32" /D "STRICT" /FR /Yu"precomp.h" /c
CPP_PROJ=/nologo /Zp4 /W3 /Zi /Od /D "_DEBUG" /D "_WINDOWS" /D "_X86_" /D\
 "WIN32" /D "STRICT" /FR$(INTDIR)/ /Fp$(OUTDIR)/"simpcntr.pch" /Yu"precomp.h"\
 /Fo$(INTDIR)/ /Fd$(OUTDIR)/"simpcntr.pdb" /c 
CPP_OBJS=.\WinDebug/
# ADD BASE RSC /l 0x1 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
RSC_PROJ=/l 0x409 /fo$(INTDIR)/"SIMPCNTR.res" /d "_DEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# SUBTRACT BASE BSC32 /Iu
# ADD BSC32 /nologo
# SUBTRACT BSC32 /Iu
BSC32_FLAGS=/nologo /o$(OUTDIR)/"simpcntr.bsc" 
BSC32_SBRS= \
	$(INTDIR)/DOC.SBR \
	$(INTDIR)/IAS.SBR \
	$(INTDIR)/IOCS.SBR \
	$(INTDIR)/IOIPF.SBR \
	$(INTDIR)/IOIPS.SBR \
	$(INTDIR)/APP.SBR \
	$(INTDIR)/SIMPCNTR.SBR \
	$(INTDIR)/SITE.SBR

$(OUTDIR)/simpcntr.bsc : $(OUTDIR)  $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib compob32.lib ole232.lib storag32.lib odbc32.lib /NOLOGO /STACK:0x16000 /SUBSYSTEM:windows /DEBUG /MACHINE:IX86
# ADD LINK32 mfcuia32.lib user32.lib gdi32.lib kernel32.lib shell32.lib ole32.lib uuid.lib /NOLOGO /BASE:0x400000 /STACK:0x16000 /DEBUG /MACHINE:I386 /SUBSYSTEM:windows,4.0
# SUBTRACT LINK32 /PDB:none
LINK32_FLAGS=mfcuia32.lib user32.lib gdi32.lib kernel32.lib shell32.lib\
 ole32.lib uuid.lib /NOLOGO /BASE:0x400000 /STACK:0x16000 /INCREMENTAL:yes\
 /PDB:$(OUTDIR)/"simpcntr.pdb" /DEBUG /MACHINE:I386\
 /OUT:$(OUTDIR)/"simpcntr.exe" /SUBSYSTEM:windows,4.0  
DEF_FILE=
LINK32_OBJS= \
	$(INTDIR)/DOC.OBJ \
	$(INTDIR)/IAS.OBJ \
	$(INTDIR)/IOCS.OBJ \
	$(INTDIR)/IOIPF.OBJ \
	$(INTDIR)/IOIPS.OBJ \
	$(INTDIR)/APP.OBJ \
	$(INTDIR)/SIMPCNTR.OBJ \
	$(INTDIR)/SITE.OBJ \
	$(INTDIR)/SIMPCNTR.res

$(OUTDIR)/simpcntr.exe : $(OUTDIR)  $(DEF_FILE) $(LINK32_OBJS)
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

ALL : $(OUTDIR)/simpcntr.exe $(OUTDIR)/simpcntr.bsc

$(OUTDIR) : 
    if not exist $(OUTDIR)/nul mkdir $(OUTDIR)

# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
# ADD BASE CPP /nologo /Zp4 /W3 /O2 /D "NDEBUG" /D "_WINDOWS" /D "_X86_" /D "WIN32" /D "STRICT" /FR /Yu"precomp.h" /c
# ADD CPP /nologo /Zp4 /W3 /O2 /D "NDEBUG" /D "_WINDOWS" /D "_X86_" /D "WIN32" /D "STRICT" /D "UNICODE" /FR /Yu"precomp.h" /c
CPP_PROJ=/nologo /Zp4 /W3 /O2 /D "NDEBUG" /D "_WINDOWS" /D "_X86_" /D "WIN32"\
 /D "STRICT" /D "UNICODE" /FR$(INTDIR)/ /Fp$(OUTDIR)/"simpcntr.pch"\
 /Yu"precomp.h" /Fo$(INTDIR)/ /c 
CPP_OBJS=.\UniRel/
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
RSC_PROJ=/l 0x409 /fo$(INTDIR)/"SIMPCNTR.res" /d "NDEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# SUBTRACT BASE BSC32 /Iu
# ADD BSC32 /nologo
# SUBTRACT BSC32 /Iu
BSC32_FLAGS=/nologo /o$(OUTDIR)/"simpcntr.bsc" 
BSC32_SBRS= \
	$(INTDIR)/DOC.SBR \
	$(INTDIR)/IAS.SBR \
	$(INTDIR)/IOCS.SBR \
	$(INTDIR)/IOIPF.SBR \
	$(INTDIR)/IOIPS.SBR \
	$(INTDIR)/APP.SBR \
	$(INTDIR)/SIMPCNTR.SBR \
	$(INTDIR)/SITE.SBR

$(OUTDIR)/simpcntr.bsc : $(OUTDIR)  $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 mfcuia32.lib user32.lib gdi32.lib kernel32.lib shell32.lib ole32.lib uuid.lib /NOLOGO /BASE:0x400000 /STACK:0x16000 /MACHINE:IX86 /SUBSYSTEM:windows,4.0
# SUBTRACT BASE LINK32 /PDB:none
# ADD LINK32 mfcuiw32.lib user32.lib gdi32.lib kernel32.lib shell32.lib ole32.lib uuid.lib /NOLOGO /BASE:0x400000 /STACK:0x16000 /MACHINE:IX86 /SUBSYSTEM:windows,4.0
# SUBTRACT LINK32 /PDB:none
LINK32_FLAGS=mfcuiw32.lib user32.lib gdi32.lib kernel32.lib shell32.lib\
 ole32.lib uuid.lib /NOLOGO /BASE:0x400000 /STACK:0x16000 /INCREMENTAL:no\
 /PDB:$(OUTDIR)/"simpcntr.pdb" /MACHINE:IX86 /OUT:$(OUTDIR)/"simpcntr.exe"\
 /SUBSYSTEM:windows,4.0  
DEF_FILE=
LINK32_OBJS= \
	$(INTDIR)/DOC.OBJ \
	$(INTDIR)/IAS.OBJ \
	$(INTDIR)/IOCS.OBJ \
	$(INTDIR)/IOIPF.OBJ \
	$(INTDIR)/IOIPS.OBJ \
	$(INTDIR)/APP.OBJ \
	$(INTDIR)/SIMPCNTR.OBJ \
	$(INTDIR)/SITE.OBJ \
	$(INTDIR)/SIMPCNTR.res

$(OUTDIR)/simpcntr.exe : $(OUTDIR)  $(DEF_FILE) $(LINK32_OBJS)
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

ALL : $(OUTDIR)/simpcntr.exe $(OUTDIR)/simpcntr.bsc

$(OUTDIR) : 
    if not exist $(OUTDIR)/nul mkdir $(OUTDIR)

# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
# ADD BASE CPP /nologo /Zp4 /W3 /Zi /Od /D "_DEBUG" /D "_WINDOWS" /D "_X86_" /D "WIN32" /D "STRICT" /FR /Yu"precomp.h" /c
# ADD CPP /nologo /Zp4 /W3 /Zi /Od /D "_DEBUG" /D "_WINDOWS" /D "_X86_" /D "WIN32" /D "STRICT" /D "UNICODE" /FR /Yu"precomp.h" /c
CPP_PROJ=/nologo /Zp4 /W3 /Zi /Od /D "_DEBUG" /D "_WINDOWS" /D "_X86_" /D\
 "WIN32" /D "STRICT" /D "UNICODE" /FR$(INTDIR)/ /Fp$(OUTDIR)/"simpcntr.pch"\
 /Yu"precomp.h" /Fo$(INTDIR)/ /Fd$(OUTDIR)/"simpcntr.pdb" /c 
CPP_OBJS=.\UniDebug/
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
RSC_PROJ=/l 0x409 /fo$(INTDIR)/"SIMPCNTR.res" /d "_DEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# SUBTRACT BASE BSC32 /Iu
# ADD BSC32 /nologo
# SUBTRACT BSC32 /Iu
BSC32_FLAGS=/nologo /o$(OUTDIR)/"simpcntr.bsc" 
BSC32_SBRS= \
	$(INTDIR)/DOC.SBR \
	$(INTDIR)/IAS.SBR \
	$(INTDIR)/IOCS.SBR \
	$(INTDIR)/IOIPF.SBR \
	$(INTDIR)/IOIPS.SBR \
	$(INTDIR)/APP.SBR \
	$(INTDIR)/SIMPCNTR.SBR \
	$(INTDIR)/SITE.SBR

$(OUTDIR)/simpcntr.bsc : $(OUTDIR)  $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 mfcuia32.lib user32.lib gdi32.lib kernel32.lib shell32.lib ole32.lib uuid.lib /NOLOGO /BASE:0x400000 /STACK:0x16000 /DEBUG /MACHINE:I386 /SUBSYSTEM:windows,4.0
# SUBTRACT BASE LINK32 /PDB:none
# ADD LINK32 mfcuiw32.lib user32.lib gdi32.lib kernel32.lib shell32.lib ole32.lib uuid.lib /NOLOGO /BASE:0x400000 /STACK:0x16000 /DEBUG /MACHINE:I386 /SUBSYSTEM:windows,4.0
# SUBTRACT LINK32 /PDB:none
LINK32_FLAGS=mfcuiw32.lib user32.lib gdi32.lib kernel32.lib shell32.lib\
 ole32.lib uuid.lib /NOLOGO /BASE:0x400000 /STACK:0x16000 /INCREMENTAL:yes\
 /PDB:$(OUTDIR)/"simpcntr.pdb" /DEBUG /MACHINE:I386\
 /OUT:$(OUTDIR)/"simpcntr.exe" /SUBSYSTEM:windows,4.0  
DEF_FILE=
LINK32_OBJS= \
	$(INTDIR)/DOC.OBJ \
	$(INTDIR)/IAS.OBJ \
	$(INTDIR)/IOCS.OBJ \
	$(INTDIR)/IOIPF.OBJ \
	$(INTDIR)/IOIPS.OBJ \
	$(INTDIR)/APP.OBJ \
	$(INTDIR)/SIMPCNTR.OBJ \
	$(INTDIR)/SITE.OBJ \
	$(INTDIR)/SIMPCNTR.res

$(OUTDIR)/simpcntr.exe : $(OUTDIR)  $(DEF_FILE) $(LINK32_OBJS)
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
	.\APP.H\
	.\SITE.H\
	.\DOC.H\
	.\SIMPCNTR.H

!IF  "$(CFG)" == "Win32 ANSI Release"

# ADD BASE CPP /Yu"PRE.H"

$(INTDIR)/DOC.OBJ :  $(SOURCE)  $(DEP_DOC_C) $(INTDIR) $(INTDIR)/SIMPCNTR.OBJ
   $(CPP) /nologo /Zp4 /W3 /O2 /D "NDEBUG" /D "_WINDOWS" /D "_X86_" /D "WIN32"\
 /D "STRICT" /FR$(INTDIR)/ /Fp$(OUTDIR)/"simpcntr.pch" /Yu"precomp.h"\
 /Fo$(INTDIR)/ /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 ANSI Debug"

# ADD BASE CPP /Yu"PRE.H"

$(INTDIR)/DOC.OBJ :  $(SOURCE)  $(DEP_DOC_C) $(INTDIR) $(INTDIR)/SIMPCNTR.OBJ
   $(CPP) /nologo /Zp4 /W3 /Zi /Od /D "_DEBUG" /D "_WINDOWS" /D "_X86_" /D\
 "WIN32" /D "STRICT" /FR$(INTDIR)/ /Fp$(OUTDIR)/"simpcntr.pch" /Yu"precomp.h"\
 /Fo$(INTDIR)/ /Fd$(OUTDIR)/"simpcntr.pdb" /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 Unicode Release"

$(INTDIR)/DOC.OBJ :  $(SOURCE)  $(DEP_DOC_C) $(INTDIR) $(INTDIR)/SIMPCNTR.OBJ
   $(CPP) /nologo /Zp4 /W3 /O2 /D "NDEBUG" /D "_WINDOWS" /D "_X86_" /D "WIN32"\
 /D "STRICT" /D "UNICODE" /FR$(INTDIR)/ /Fp$(OUTDIR)/"simpcntr.pch"\
 /Yu"precomp.h" /Fo$(INTDIR)/ /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 Unicode Debug"

$(INTDIR)/DOC.OBJ :  $(SOURCE)  $(DEP_DOC_C) $(INTDIR) $(INTDIR)/SIMPCNTR.OBJ
   $(CPP) /nologo /Zp4 /W3 /Zi /Od /D "_DEBUG" /D "_WINDOWS" /D "_X86_" /D\
 "WIN32" /D "STRICT" /D "UNICODE" /FR$(INTDIR)/ /Fp$(OUTDIR)/"simpcntr.pch"\
 /Yu"precomp.h" /Fo$(INTDIR)/ /Fd$(OUTDIR)/"simpcntr.pdb" /c  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\IAS.CPP
DEP_IAS_C=\
	.\PRECOMP.H\
	.\APP.H\
	.\SITE.H\
	.\DOC.H\
	.\SIMPCNTR.H

!IF  "$(CFG)" == "Win32 ANSI Release"

# ADD BASE CPP /Yu"PRE.H"

$(INTDIR)/IAS.OBJ :  $(SOURCE)  $(DEP_IAS_C) $(INTDIR) $(INTDIR)/SIMPCNTR.OBJ
   $(CPP) /nologo /Zp4 /W3 /O2 /D "NDEBUG" /D "_WINDOWS" /D "_X86_" /D "WIN32"\
 /D "STRICT" /FR$(INTDIR)/ /Fp$(OUTDIR)/"simpcntr.pch" /Yu"precomp.h"\
 /Fo$(INTDIR)/ /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 ANSI Debug"

# ADD BASE CPP /Yu"PRE.H"

$(INTDIR)/IAS.OBJ :  $(SOURCE)  $(DEP_IAS_C) $(INTDIR) $(INTDIR)/SIMPCNTR.OBJ
   $(CPP) /nologo /Zp4 /W3 /Zi /Od /D "_DEBUG" /D "_WINDOWS" /D "_X86_" /D\
 "WIN32" /D "STRICT" /FR$(INTDIR)/ /Fp$(OUTDIR)/"simpcntr.pch" /Yu"precomp.h"\
 /Fo$(INTDIR)/ /Fd$(OUTDIR)/"simpcntr.pdb" /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 Unicode Release"

$(INTDIR)/IAS.OBJ :  $(SOURCE)  $(DEP_IAS_C) $(INTDIR) $(INTDIR)/SIMPCNTR.OBJ
   $(CPP) /nologo /Zp4 /W3 /O2 /D "NDEBUG" /D "_WINDOWS" /D "_X86_" /D "WIN32"\
 /D "STRICT" /D "UNICODE" /FR$(INTDIR)/ /Fp$(OUTDIR)/"simpcntr.pch"\
 /Yu"precomp.h" /Fo$(INTDIR)/ /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 Unicode Debug"

$(INTDIR)/IAS.OBJ :  $(SOURCE)  $(DEP_IAS_C) $(INTDIR) $(INTDIR)/SIMPCNTR.OBJ
   $(CPP) /nologo /Zp4 /W3 /Zi /Od /D "_DEBUG" /D "_WINDOWS" /D "_X86_" /D\
 "WIN32" /D "STRICT" /D "UNICODE" /FR$(INTDIR)/ /Fp$(OUTDIR)/"simpcntr.pch"\
 /Yu"precomp.h" /Fo$(INTDIR)/ /Fd$(OUTDIR)/"simpcntr.pdb" /c  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\IOCS.CPP
DEP_IOCS_=\
	.\PRECOMP.H\
	.\APP.H\
	.\SITE.H\
	.\DOC.H\
	.\SIMPCNTR.H

!IF  "$(CFG)" == "Win32 ANSI Release"

# ADD BASE CPP /Yu"PRE.H"

$(INTDIR)/IOCS.OBJ :  $(SOURCE)  $(DEP_IOCS_) $(INTDIR) $(INTDIR)/SIMPCNTR.OBJ
   $(CPP) /nologo /Zp4 /W3 /O2 /D "NDEBUG" /D "_WINDOWS" /D "_X86_" /D "WIN32"\
 /D "STRICT" /FR$(INTDIR)/ /Fp$(OUTDIR)/"simpcntr.pch" /Yu"precomp.h"\
 /Fo$(INTDIR)/ /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 ANSI Debug"

# ADD BASE CPP /Yu"PRE.H"

$(INTDIR)/IOCS.OBJ :  $(SOURCE)  $(DEP_IOCS_) $(INTDIR) $(INTDIR)/SIMPCNTR.OBJ
   $(CPP) /nologo /Zp4 /W3 /Zi /Od /D "_DEBUG" /D "_WINDOWS" /D "_X86_" /D\
 "WIN32" /D "STRICT" /FR$(INTDIR)/ /Fp$(OUTDIR)/"simpcntr.pch" /Yu"precomp.h"\
 /Fo$(INTDIR)/ /Fd$(OUTDIR)/"simpcntr.pdb" /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 Unicode Release"

$(INTDIR)/IOCS.OBJ :  $(SOURCE)  $(DEP_IOCS_) $(INTDIR) $(INTDIR)/SIMPCNTR.OBJ
   $(CPP) /nologo /Zp4 /W3 /O2 /D "NDEBUG" /D "_WINDOWS" /D "_X86_" /D "WIN32"\
 /D "STRICT" /D "UNICODE" /FR$(INTDIR)/ /Fp$(OUTDIR)/"simpcntr.pch"\
 /Yu"precomp.h" /Fo$(INTDIR)/ /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 Unicode Debug"

$(INTDIR)/IOCS.OBJ :  $(SOURCE)  $(DEP_IOCS_) $(INTDIR) $(INTDIR)/SIMPCNTR.OBJ
   $(CPP) /nologo /Zp4 /W3 /Zi /Od /D "_DEBUG" /D "_WINDOWS" /D "_X86_" /D\
 "WIN32" /D "STRICT" /D "UNICODE" /FR$(INTDIR)/ /Fp$(OUTDIR)/"simpcntr.pch"\
 /Yu"precomp.h" /Fo$(INTDIR)/ /Fd$(OUTDIR)/"simpcntr.pdb" /c  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\IOIPF.CPP
DEP_IOIPF=\
	.\PRECOMP.H\
	.\APP.H\
	.\SITE.H\
	.\DOC.H\
	.\SIMPCNTR.H

!IF  "$(CFG)" == "Win32 ANSI Release"

# ADD BASE CPP /Yu"PRE.H"

$(INTDIR)/IOIPF.OBJ :  $(SOURCE)  $(DEP_IOIPF) $(INTDIR) $(INTDIR)/SIMPCNTR.OBJ
   $(CPP) /nologo /Zp4 /W3 /O2 /D "NDEBUG" /D "_WINDOWS" /D "_X86_" /D "WIN32"\
 /D "STRICT" /FR$(INTDIR)/ /Fp$(OUTDIR)/"simpcntr.pch" /Yu"precomp.h"\
 /Fo$(INTDIR)/ /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 ANSI Debug"

# ADD BASE CPP /Yu"PRE.H"

$(INTDIR)/IOIPF.OBJ :  $(SOURCE)  $(DEP_IOIPF) $(INTDIR) $(INTDIR)/SIMPCNTR.OBJ
   $(CPP) /nologo /Zp4 /W3 /Zi /Od /D "_DEBUG" /D "_WINDOWS" /D "_X86_" /D\
 "WIN32" /D "STRICT" /FR$(INTDIR)/ /Fp$(OUTDIR)/"simpcntr.pch" /Yu"precomp.h"\
 /Fo$(INTDIR)/ /Fd$(OUTDIR)/"simpcntr.pdb" /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 Unicode Release"

$(INTDIR)/IOIPF.OBJ :  $(SOURCE)  $(DEP_IOIPF) $(INTDIR) $(INTDIR)/SIMPCNTR.OBJ
   $(CPP) /nologo /Zp4 /W3 /O2 /D "NDEBUG" /D "_WINDOWS" /D "_X86_" /D "WIN32"\
 /D "STRICT" /D "UNICODE" /FR$(INTDIR)/ /Fp$(OUTDIR)/"simpcntr.pch"\
 /Yu"precomp.h" /Fo$(INTDIR)/ /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 Unicode Debug"

$(INTDIR)/IOIPF.OBJ :  $(SOURCE)  $(DEP_IOIPF) $(INTDIR) $(INTDIR)/SIMPCNTR.OBJ
   $(CPP) /nologo /Zp4 /W3 /Zi /Od /D "_DEBUG" /D "_WINDOWS" /D "_X86_" /D\
 "WIN32" /D "STRICT" /D "UNICODE" /FR$(INTDIR)/ /Fp$(OUTDIR)/"simpcntr.pch"\
 /Yu"precomp.h" /Fo$(INTDIR)/ /Fd$(OUTDIR)/"simpcntr.pdb" /c  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\IOIPS.CPP
DEP_IOIPS=\
	.\PRECOMP.H\
	.\APP.H\
	.\SITE.H\
	.\DOC.H\
	.\SIMPCNTR.H

!IF  "$(CFG)" == "Win32 ANSI Release"

# ADD BASE CPP /Yu"PRE.H"

$(INTDIR)/IOIPS.OBJ :  $(SOURCE)  $(DEP_IOIPS) $(INTDIR) $(INTDIR)/SIMPCNTR.OBJ
   $(CPP) /nologo /Zp4 /W3 /O2 /D "NDEBUG" /D "_WINDOWS" /D "_X86_" /D "WIN32"\
 /D "STRICT" /FR$(INTDIR)/ /Fp$(OUTDIR)/"simpcntr.pch" /Yu"precomp.h"\
 /Fo$(INTDIR)/ /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 ANSI Debug"

# ADD BASE CPP /Yu"PRE.H"

$(INTDIR)/IOIPS.OBJ :  $(SOURCE)  $(DEP_IOIPS) $(INTDIR) $(INTDIR)/SIMPCNTR.OBJ
   $(CPP) /nologo /Zp4 /W3 /Zi /Od /D "_DEBUG" /D "_WINDOWS" /D "_X86_" /D\
 "WIN32" /D "STRICT" /FR$(INTDIR)/ /Fp$(OUTDIR)/"simpcntr.pch" /Yu"precomp.h"\
 /Fo$(INTDIR)/ /Fd$(OUTDIR)/"simpcntr.pdb" /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 Unicode Release"

$(INTDIR)/IOIPS.OBJ :  $(SOURCE)  $(DEP_IOIPS) $(INTDIR) $(INTDIR)/SIMPCNTR.OBJ
   $(CPP) /nologo /Zp4 /W3 /O2 /D "NDEBUG" /D "_WINDOWS" /D "_X86_" /D "WIN32"\
 /D "STRICT" /D "UNICODE" /FR$(INTDIR)/ /Fp$(OUTDIR)/"simpcntr.pch"\
 /Yu"precomp.h" /Fo$(INTDIR)/ /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 Unicode Debug"

$(INTDIR)/IOIPS.OBJ :  $(SOURCE)  $(DEP_IOIPS) $(INTDIR) $(INTDIR)/SIMPCNTR.OBJ
   $(CPP) /nologo /Zp4 /W3 /Zi /Od /D "_DEBUG" /D "_WINDOWS" /D "_X86_" /D\
 "WIN32" /D "STRICT" /D "UNICODE" /FR$(INTDIR)/ /Fp$(OUTDIR)/"simpcntr.pch"\
 /Yu"precomp.h" /Fo$(INTDIR)/ /Fd$(OUTDIR)/"simpcntr.pdb" /c  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\APP.CPP
DEP_APP_C=\
	.\PRECOMP.H\
	.\APP.H\
	.\SITE.H\
	.\DOC.H\
	.\SIMPCNTR.H

!IF  "$(CFG)" == "Win32 ANSI Release"

# ADD BASE CPP /Yu"PRE.H"

$(INTDIR)/APP.OBJ :  $(SOURCE)  $(DEP_APP_C) $(INTDIR) $(INTDIR)/SIMPCNTR.OBJ
   $(CPP) /nologo /Zp4 /W3 /O2 /D "NDEBUG" /D "_WINDOWS" /D "_X86_" /D "WIN32"\
 /D "STRICT" /FR$(INTDIR)/ /Fp$(OUTDIR)/"simpcntr.pch" /Yu"precomp.h"\
 /Fo$(INTDIR)/ /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 ANSI Debug"

# ADD BASE CPP /Yu"PRE.H"

$(INTDIR)/APP.OBJ :  $(SOURCE)  $(DEP_APP_C) $(INTDIR) $(INTDIR)/SIMPCNTR.OBJ
   $(CPP) /nologo /Zp4 /W3 /Zi /Od /D "_DEBUG" /D "_WINDOWS" /D "_X86_" /D\
 "WIN32" /D "STRICT" /FR$(INTDIR)/ /Fp$(OUTDIR)/"simpcntr.pch" /Yu"precomp.h"\
 /Fo$(INTDIR)/ /Fd$(OUTDIR)/"simpcntr.pdb" /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 Unicode Release"

$(INTDIR)/APP.OBJ :  $(SOURCE)  $(DEP_APP_C) $(INTDIR) $(INTDIR)/SIMPCNTR.OBJ
   $(CPP) /nologo /Zp4 /W3 /O2 /D "NDEBUG" /D "_WINDOWS" /D "_X86_" /D "WIN32"\
 /D "STRICT" /D "UNICODE" /FR$(INTDIR)/ /Fp$(OUTDIR)/"simpcntr.pch"\
 /Yu"precomp.h" /Fo$(INTDIR)/ /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 Unicode Debug"

$(INTDIR)/APP.OBJ :  $(SOURCE)  $(DEP_APP_C) $(INTDIR) $(INTDIR)/SIMPCNTR.OBJ
   $(CPP) /nologo /Zp4 /W3 /Zi /Od /D "_DEBUG" /D "_WINDOWS" /D "_X86_" /D\
 "WIN32" /D "STRICT" /D "UNICODE" /FR$(INTDIR)/ /Fp$(OUTDIR)/"simpcntr.pch"\
 /Yu"precomp.h" /Fo$(INTDIR)/ /Fd$(OUTDIR)/"simpcntr.pdb" /c  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\SIMPCNTR.CPP
DEP_SIMPC=\
	.\PRECOMP.H\
	.\APP.H\
	.\SITE.H\
	.\DOC.H\
	.\SIMPCNTR.H

!IF  "$(CFG)" == "Win32 ANSI Release"

# ADD BASE CPP /Yu"PRE.H"
# ADD CPP /Yc"precomp.h"

$(INTDIR)/SIMPCNTR.OBJ :  $(SOURCE)  $(DEP_SIMPC) $(INTDIR)
   $(CPP) /nologo /Zp4 /W3 /O2 /D "NDEBUG" /D "_WINDOWS" /D "_X86_" /D "WIN32"\
 /D "STRICT" /FR$(INTDIR)/ /Fp$(OUTDIR)/"simpcntr.pch" /Yc"precomp.h"\
 /Fo$(INTDIR)/ /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 ANSI Debug"

# ADD BASE CPP /Yu"PRE.H"
# ADD CPP /Yc"precomp.h"

$(INTDIR)/SIMPCNTR.OBJ :  $(SOURCE)  $(DEP_SIMPC) $(INTDIR)
   $(CPP) /nologo /Zp4 /W3 /Zi /Od /D "_DEBUG" /D "_WINDOWS" /D "_X86_" /D\
 "WIN32" /D "STRICT" /FR$(INTDIR)/ /Fp$(OUTDIR)/"simpcntr.pch" /Yc"precomp.h"\
 /Fo$(INTDIR)/ /Fd$(OUTDIR)/"simpcntr.pdb" /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 Unicode Release"

# ADD BASE CPP /Yc"precomp.h"
# ADD CPP /Yc"precomp.h"

$(INTDIR)/SIMPCNTR.OBJ :  $(SOURCE)  $(DEP_SIMPC) $(INTDIR)
   $(CPP) /nologo /Zp4 /W3 /O2 /D "NDEBUG" /D "_WINDOWS" /D "_X86_" /D "WIN32"\
 /D "STRICT" /D "UNICODE" /FR$(INTDIR)/ /Fp$(OUTDIR)/"simpcntr.pch"\
 /Yc"precomp.h" /Fo$(INTDIR)/ /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 Unicode Debug"

# ADD BASE CPP /Yc"precomp.h"
# ADD CPP /Yc"precomp.h"

$(INTDIR)/SIMPCNTR.OBJ :  $(SOURCE)  $(DEP_SIMPC) $(INTDIR)
   $(CPP) /nologo /Zp4 /W3 /Zi /Od /D "_DEBUG" /D "_WINDOWS" /D "_X86_" /D\
 "WIN32" /D "STRICT" /D "UNICODE" /FR$(INTDIR)/ /Fp$(OUTDIR)/"simpcntr.pch"\
 /Yc"precomp.h" /Fo$(INTDIR)/ /Fd$(OUTDIR)/"simpcntr.pdb" /c  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\SITE.CPP
DEP_SITE_=\
	.\PRECOMP.H\
	.\APP.H\
	.\SITE.H\
	.\DOC.H\
	.\SIMPCNTR.H

!IF  "$(CFG)" == "Win32 ANSI Release"

# ADD BASE CPP /Yu"PRE.H"

$(INTDIR)/SITE.OBJ :  $(SOURCE)  $(DEP_SITE_) $(INTDIR) $(INTDIR)/SIMPCNTR.OBJ
   $(CPP) /nologo /Zp4 /W3 /O2 /D "NDEBUG" /D "_WINDOWS" /D "_X86_" /D "WIN32"\
 /D "STRICT" /FR$(INTDIR)/ /Fp$(OUTDIR)/"simpcntr.pch" /Yu"precomp.h"\
 /Fo$(INTDIR)/ /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 ANSI Debug"

# ADD BASE CPP /Yu"PRE.H"

$(INTDIR)/SITE.OBJ :  $(SOURCE)  $(DEP_SITE_) $(INTDIR) $(INTDIR)/SIMPCNTR.OBJ
   $(CPP) /nologo /Zp4 /W3 /Zi /Od /D "_DEBUG" /D "_WINDOWS" /D "_X86_" /D\
 "WIN32" /D "STRICT" /FR$(INTDIR)/ /Fp$(OUTDIR)/"simpcntr.pch" /Yu"precomp.h"\
 /Fo$(INTDIR)/ /Fd$(OUTDIR)/"simpcntr.pdb" /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 Unicode Release"

$(INTDIR)/SITE.OBJ :  $(SOURCE)  $(DEP_SITE_) $(INTDIR) $(INTDIR)/SIMPCNTR.OBJ
   $(CPP) /nologo /Zp4 /W3 /O2 /D "NDEBUG" /D "_WINDOWS" /D "_X86_" /D "WIN32"\
 /D "STRICT" /D "UNICODE" /FR$(INTDIR)/ /Fp$(OUTDIR)/"simpcntr.pch"\
 /Yu"precomp.h" /Fo$(INTDIR)/ /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 Unicode Debug"

$(INTDIR)/SITE.OBJ :  $(SOURCE)  $(DEP_SITE_) $(INTDIR) $(INTDIR)/SIMPCNTR.OBJ
   $(CPP) /nologo /Zp4 /W3 /Zi /Od /D "_DEBUG" /D "_WINDOWS" /D "_X86_" /D\
 "WIN32" /D "STRICT" /D "UNICODE" /FR$(INTDIR)/ /Fp$(OUTDIR)/"simpcntr.pch"\
 /Yu"precomp.h" /Fo$(INTDIR)/ /Fd$(OUTDIR)/"simpcntr.pdb" /c  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\SIMPCNTR.RC
DEP_SIMPCN=\
	.\simpcntr.ico\
	.\SIMPCNTR.H

!IF  "$(CFG)" == "Win32 ANSI Release"

# ADD BASE RSC /l 0x409
# ADD RSC /l 0x1

$(INTDIR)/SIMPCNTR.res :  $(SOURCE)  $(DEP_SIMPCN) $(INTDIR)
   $(RSC) /l 0x1 /fo$(INTDIR)/"SIMPCNTR.res" /d "NDEBUG"  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 ANSI Debug"

# ADD BASE RSC /l 0x409
# ADD RSC /l 0x1

$(INTDIR)/SIMPCNTR.res :  $(SOURCE)  $(DEP_SIMPCN) $(INTDIR)
   $(RSC) /l 0x1 /fo$(INTDIR)/"SIMPCNTR.res" /d "_DEBUG"  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 Unicode Release"

# ADD BASE RSC /l 0x1
# ADD RSC /l 0x1

$(INTDIR)/SIMPCNTR.res :  $(SOURCE)  $(DEP_SIMPCN) $(INTDIR)
   $(RSC) /l 0x1 /fo$(INTDIR)/"SIMPCNTR.res" /d "NDEBUG"  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 Unicode Debug"

# ADD BASE RSC /l 0x1
# ADD RSC /l 0x1

$(INTDIR)/SIMPCNTR.res :  $(SOURCE)  $(DEP_SIMPCN) $(INTDIR)
   $(RSC) /l 0x1 /fo$(INTDIR)/"SIMPCNTR.res" /d "_DEBUG"  $(SOURCE) 

!ENDIF 

# End Source File
# End Group
# End Project
################################################################################
