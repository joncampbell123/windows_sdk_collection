#****************************************************************************
#                                                                           *
# THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY     *
# KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE       *
# IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR     *
# PURPOSE.                                                                  *
#                                                                           *
# Copyright (C) 1993-95  Microsoft Corporation.  All Rights Reserved.       *
#                                                                           *
#****************************************************************************

# Microsoft Visual C++ Generated NMAKE File, Format Version 20054
# MSVCPRJ: version 2.00.4242
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
!MESSAGE NMAKE /f "aspi32ln.MAK" CFG="Win32 (80x86) Debug"
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
# PROP Target_Last_Scanned "Win32 (80x86) Debug"

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

ALL : .\WinRel\aspi32ln.exe .\WinRel\aspi32ln.bsc

$(OUTDIR) : 
    if not exist $(OUTDIR)/nul mkdir $(OUTDIR)

MTL=MkTypLib.exe
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
CPP=cl.exe
# ADD BASE CPP /nologo /MD /W3 /GX /YX /O2 /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /c
# ADD CPP /nologo /Zp1 /W3 /GX /YX /O2 /D "NDEBUG" /D "_WINDOWS" /D "WIN32" /D "_MBCS" /FR /c
CPP_PROJ=/nologo /Zp1 /W3 /GX /YX /O2 /D "NDEBUG" /D "_WINDOWS" /D "WIN32" /D\
 "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"aspi32ln.pch" /Fo$(INTDIR)/ /c 
CPP_OBJS=.\WinRel/

.c{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

RSC=rc.exe
# ADD BASE RSC /l 0x1 /d "NDEBUG"
# ADD RSC /l 0x1 /d "NDEBUG"
RSC_PROJ=/l 0x1 /fo$(INTDIR)/"aspi32ln.res" /d "NDEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# SUBTRACT BASE BSC32 /Iu
# ADD BSC32 /nologo
# SUBTRACT BSC32 /Iu
BSC32_FLAGS=/nologo /o$(OUTDIR)/"aspi32ln.bsc" 
BSC32_SBRS= \
	.\WinRel\aspi32ln.sbr

.\WinRel\aspi32ln.bsc : $(OUTDIR)  $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib mfc30.lib mfco30.lib mfcd30.lib /NOLOGO /SUBSYSTEM:windows /MACHINE:I386
# ADD LINK32 mfc30.lib mfco30.lib mfcd30.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /NOLOGO /SUBSYSTEM:windows /MACHINE:I386
LINK32_FLAGS=mfc30.lib mfco30.lib mfcd30.lib kernel32.lib user32.lib gdi32.lib\
 winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib\
 uuid.lib odbc32.lib odbccp32.lib /NOLOGO /SUBSYSTEM:windows /INCREMENTAL:no\
 /PDB:$(OUTDIR)/"aspi32ln.pdb" /MACHINE:I386 /OUT:$(OUTDIR)/"aspi32ln.exe" 
DEF_FILE=
LINK32_OBJS= \
	.\wnaspi32.lib \
	.\WinRel\aspi32ln.obj \
	.\WinRel\aspi32ln.res

.\WinRel\aspi32ln.exe : $(OUTDIR)  $(DEF_FILE) $(LINK32_OBJS)
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

ALL : .\WinDebug\aspi32ln.exe .\WinDebug\aspi32ln.bsc

$(OUTDIR) : 
    if not exist $(OUTDIR)/nul mkdir $(OUTDIR)

MTL=MkTypLib.exe
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
CPP=cl.exe
# ADD BASE CPP /nologo /MD /W3 /GX /Zi /YX /Od /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /c
# ADD CPP /nologo /Zp1 /W3 /GX /Zi /YX /Od /D "_DEBUG" /D "_WINDOWS" /D "WIN32" /D "_MBCS" /FR /c
CPP_PROJ=/nologo /Zp1 /W3 /GX /Zi /YX /Od /D "_DEBUG" /D "_WINDOWS" /D "WIN32"\
 /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"aspi32ln.pch" /Fo$(INTDIR)/\
 /Fd$(OUTDIR)/"aspi32ln.pdb" /c 
CPP_OBJS=.\WinDebug/

.c{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

RSC=rc.exe
# ADD BASE RSC /l 0x1 /d "_DEBUG"
# ADD RSC /l 0x1 /d "_DEBUG"
RSC_PROJ=/l 0x1 /fo$(INTDIR)/"aspi32ln.res" /d "_DEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# SUBTRACT BASE BSC32 /Iu
# ADD BSC32 /nologo
# SUBTRACT BSC32 /Iu
BSC32_FLAGS=/nologo /o$(OUTDIR)/"aspi32ln.bsc" 
BSC32_SBRS= \
	.\WinDebug\aspi32ln.sbr

.\WinDebug\aspi32ln.bsc : $(OUTDIR)  $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib mfc30d.lib mfco30d.lib mfcd30d.lib /NOLOGO /SUBSYSTEM:windows /DEBUG /MACHINE:I386
# ADD LINK32 mfc30d.lib mfco30d.lib mfcd30d.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /NOLOGO /SUBSYSTEM:windows /DEBUG /MACHINE:I386
LINK32_FLAGS=mfc30d.lib mfco30d.lib mfcd30d.lib kernel32.lib user32.lib\
 gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib\
 oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /NOLOGO /SUBSYSTEM:windows\
 /INCREMENTAL:yes /PDB:$(OUTDIR)/"aspi32ln.pdb" /DEBUG /MACHINE:I386\
 /OUT:$(OUTDIR)/"aspi32ln.exe" 
DEF_FILE=
LINK32_OBJS= \
	.\wnaspi32.lib \
	.\WinDebug\aspi32ln.obj \
	.\WinDebug\aspi32ln.res

.\WinDebug\aspi32ln.exe : $(OUTDIR)  $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 1
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Win32__M"
# PROP Intermediate_Dir "Win32__M"
OUTDIR=.\Win32__M
INTDIR=.\Win32__M

ALL : .\Win32__M\LINKTEST.exe .\Win32__M\LINKTEST.bsc ".\$(MTL_TLBS)"\
 ".\$(MTL_TLBS)" ".\$(MTL_TLBS)" ".\$(MTL_TLBS)" ".\$(MTL_TLBS)" ".\$(MTL_TLBS)"\
 ".\$(MTL_TLBS)" ".\$(MTL_TLBS)" ".\$(MTL_TLBS)" ".\$(MTL_TLBS)"

$(OUTDIR) : 
    if not exist $(OUTDIR)/nul mkdir $(OUTDIR)

MTL=MkTypLib.exe
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
RSC=rc.exe
# ADD BASE RSC /l 0x1 /d "_DEBUG"
# ADD RSC /l 0x1 /d "_DEBUG"
RSC_PROJ=/l 0x1 /fo$(INTDIR)/"aspi32ln.res" /d "_DEBUG" 
CPP=cl.exe
# ADD BASE CPP /nologo /MD /W3 /GX /Zi /YX /Od /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /c
# ADD CPP /nologo /MD /W3 /GX /Zi /YX /Od /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /c
CPP_PROJ=/nologo /MD /W3 /GX /Zi /YX /Od /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL"\
 /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"LINKTEST.pch" /Fo$(INTDIR)/\
 /Fd$(OUTDIR)/"LINKTEST.pdb" /c 
CPP_OBJS=.\Win32__M/

.c{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o$(OUTDIR)/"LINKTEST.bsc" 
BSC32_SBRS= \
	$(INTDIR)/linktest.sbr

$(OUTDIR)/LINKTEST.bsc : $(OUTDIR)  $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib odbc32.lib mfc30d.lib mfco30d.lib mfcd30d.lib oleaut32.lib uuid.lib /NOLOGO /SUBSYSTEM:windows /DEBUG /MACHINE:MIPS
# ADD LINK32 user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib odbc32.lib mfc30d.lib mfco30d.lib mfcd30d.lib oleaut32.lib uuid.lib /NOLOGO /SUBSYSTEM:windows /DEBUG /MACHINE:MIPS
LINK32_FLAGS=user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib\
 shell32.lib odbc32.lib mfc30d.lib mfco30d.lib mfcd30d.lib oleaut32.lib uuid.lib\
  /NOLOGO /SUBSYSTEM:windows /PDB:$(OUTDIR)/"LINKTEST.pdb" /DEBUG /MACHINE:MIPS\
 /OUT:$(OUTDIR)/"LINKTEST.exe" 
DEF_FLAGS=
DEF_FILE=
LINK32_OBJS= \
	$(INTDIR)/linktest.res \
	.\wnaspi32.lib \
	$(INTDIR)/linktest.obj

$(OUTDIR)/LINKTEST.exe : $(OUTDIR)  $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(DEF_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 0
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Win32__0"
# PROP Intermediate_Dir "Win32__0"
OUTDIR=.\Win32__0
INTDIR=.\Win32__0

ALL : .\Win32__0\LINKTEST.exe .\Win32__0\LINKTEST.bsc ".\$(MTL_TLBS)"\
 ".\$(MTL_TLBS)" ".\$(MTL_TLBS)" ".\$(MTL_TLBS)" ".\$(MTL_TLBS)" ".\$(MTL_TLBS)"\
 ".\$(MTL_TLBS)" ".\$(MTL_TLBS)" ".\$(MTL_TLBS)" ".\$(MTL_TLBS)"

$(OUTDIR) : 
    if not exist $(OUTDIR)/nul mkdir $(OUTDIR)

MTL=MkTypLib.exe
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
RSC=rc.exe
# ADD BASE RSC /l 0x1 /d "NDEBUG"
# ADD RSC /l 0x1 /d "NDEBUG"
RSC_PROJ=/l 0x1 /fo$(INTDIR)/"aspi32ln.res" /d "NDEBUG" 
CPP=cl.exe
# ADD BASE CPP /nologo /MD /W3 /GX /YX /O2 /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /c
# ADD CPP /nologo /MD /W3 /GX /YX /O2 /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /c
CPP_PROJ=/nologo /MD /W3 /GX /YX /O2 /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D\
 "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"LINKTEST.pch" /Fo$(INTDIR)/ /c 
CPP_OBJS=.\Win32__0/

.c{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o$(OUTDIR)/"LINKTEST.bsc" 
BSC32_SBRS= \
	$(INTDIR)/linktest.sbr

$(OUTDIR)/LINKTEST.bsc : $(OUTDIR)  $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib odbc32.lib mfc30.lib mfco30.lib mfcd30.lib oleaut32.lib uuid.lib /NOLOGO /SUBSYSTEM:windows /MACHINE:MIPS
# ADD LINK32 user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib odbc32.lib mfc30.lib mfco30.lib mfcd30.lib oleaut32.lib uuid.lib /NOLOGO /SUBSYSTEM:windows /MACHINE:MIPS
LINK32_FLAGS=user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib\
 shell32.lib odbc32.lib mfc30.lib mfco30.lib mfcd30.lib oleaut32.lib uuid.lib\
  /NOLOGO /SUBSYSTEM:windows /PDB:$(OUTDIR)/"LINKTEST.pdb" /MACHINE:MIPS\
 /OUT:$(OUTDIR)/"LINKTEST.exe" 
DEF_FLAGS=
DEF_FILE=
LINK32_OBJS= \
	$(INTDIR)/linktest.res \
	.\wnaspi32.lib \
	$(INTDIR)/linktest.obj

$(OUTDIR)/LINKTEST.exe : $(OUTDIR)  $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(DEF_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 

################################################################################
# Begin Group "Source Files"

################################################################################
# Begin Source File

SOURCE=.\wnaspi32.lib
# End Source File
################################################################################
# Begin Source File

SOURCE=.\aspi32ln.C
DEP_ASPI3=\
	.\aspi32ln.h\
	.\SCSIDEFS.H\
	.\wnaspi32.h

!IF  "$(CFG)" == "Win32 (80x86) Release"

.\WinRel\aspi32ln.obj :  $(SOURCE)  $(DEP_ASPI3) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

.\WinDebug\aspi32ln.obj :  $(SOURCE)  $(DEP_ASPI3) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\aspi32ln.RC
DEP_ASPI32=\
	.\aspi32ln.h

!IF  "$(CFG)" == "Win32 (80x86) Release"

.\WinRel\aspi32ln.res :  $(SOURCE)  $(DEP_ASPI32) $(INTDIR)
   $(RSC) $(RSC_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

.\WinDebug\aspi32ln.res :  $(SOURCE)  $(DEP_ASPI32) $(INTDIR)
   $(RSC) $(RSC_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

!ENDIF 

# End Source File
# End Group
# End Project
################################################################################
