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
!MESSAGE NMAKE /f "printer.mak" CFG="Win32 (80x86) Debug"
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

ALL : $(OUTDIR)/printer.exe $(OUTDIR)/printer.bsc

$(OUTDIR) : 
    if not exist $(OUTDIR)/nul mkdir $(OUTDIR)

MTL=MkTypLib.exe
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
CPP=cl.exe
# ADD BASE CPP /nologo /MD /W3 /GX /YX /O2 /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /c
# ADD CPP /nologo /W3 /GX /YX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /FR /c
CPP_PROJ=/nologo /W3 /GX /YX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D\
 "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"printer.pch" /Fo$(INTDIR)/ /c 
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
RSC_PROJ=/l 0x409 /fo$(INTDIR)/"printer.res" /d "NDEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# SUBTRACT BASE BSC32 /Iu
# ADD BSC32 /nologo
# SUBTRACT BSC32 /Iu
BSC32_FLAGS=/nologo /o$(OUTDIR)/"printer.bsc" 
BSC32_SBRS= \
	$(INTDIR)/getpdriv.sbr \
	$(INTDIR)/enumprt.sbr \
	$(INTDIR)/paint.sbr \
	$(INTDIR)/getcaps.sbr \
	$(INTDIR)/printer.sbr

$(OUTDIR)/printer.bsc : $(OUTDIR)  $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib mfc30.lib mfco30.lib mfcd30.lib /NOLOGO /SUBSYSTEM:windows /MACHINE:I386
# ADD LINK32 user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib /NOLOGO /SUBSYSTEM:windows /MACHINE:I386
LINK32_FLAGS=user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib\
 /NOLOGO /SUBSYSTEM:windows /INCREMENTAL:no /PDB:$(OUTDIR)/"printer.pdb"\
 /MACHINE:I386 /OUT:$(OUTDIR)/"printer.exe" 
DEF_FILE=
LINK32_OBJS= \
	$(INTDIR)/getpdriv.obj \
	$(INTDIR)/enumprt.obj \
	$(INTDIR)/paint.obj \
	$(INTDIR)/getcaps.obj \
	$(INTDIR)/printer.res \
	$(INTDIR)/printer.obj

$(OUTDIR)/printer.exe : $(OUTDIR)  $(DEF_FILE) $(LINK32_OBJS)
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

ALL : $(OUTDIR)/printer.exe $(OUTDIR)/printer.bsc

$(OUTDIR) : 
    if not exist $(OUTDIR)/nul mkdir $(OUTDIR)

MTL=MkTypLib.exe
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
CPP=cl.exe
# ADD BASE CPP /nologo /MD /W3 /GX /Zi /YX /Od /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /c
# ADD CPP /nologo /W3 /GX /Zi /YX /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /FR /c
CPP_PROJ=/nologo /W3 /GX /Zi /YX /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D\
 "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"printer.pch" /Fo$(INTDIR)/\
 /Fd$(OUTDIR)/"printer.pdb" /c 
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
RSC_PROJ=/l 0x409 /fo$(INTDIR)/"printer.res" /d "_DEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# SUBTRACT BASE BSC32 /Iu
# ADD BSC32 /nologo
# SUBTRACT BSC32 /Iu
BSC32_FLAGS=/nologo /o$(OUTDIR)/"printer.bsc" 
BSC32_SBRS= \
	$(INTDIR)/getpdriv.sbr \
	$(INTDIR)/enumprt.sbr \
	$(INTDIR)/paint.sbr \
	$(INTDIR)/getcaps.sbr \
	$(INTDIR)/printer.sbr

$(OUTDIR)/printer.bsc : $(OUTDIR)  $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib mfc30d.lib mfco30d.lib mfcd30d.lib /NOLOGO /SUBSYSTEM:windows /DEBUG /MACHINE:I386
# ADD LINK32 user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib /NOLOGO /SUBSYSTEM:windows /DEBUG /MACHINE:I386
LINK32_FLAGS=user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib\
 /NOLOGO /SUBSYSTEM:windows /INCREMENTAL:yes /PDB:$(OUTDIR)/"printer.pdb" /DEBUG\
 /MACHINE:I386 /OUT:$(OUTDIR)/"printer.exe" 
DEF_FILE=
LINK32_OBJS= \
	$(INTDIR)/getpdriv.obj \
	$(INTDIR)/enumprt.obj \
	$(INTDIR)/paint.obj \
	$(INTDIR)/getcaps.obj \
	$(INTDIR)/printer.res \
	$(INTDIR)/printer.obj

$(OUTDIR)/printer.exe : $(OUTDIR)  $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 1
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "WinDebug"
# PROP Intermediate_Dir "WinDebug"
OUTDIR=.\WinDebug
INTDIR=.\WinDebug

ALL : $(OUTDIR)/printer.exe $(OUTDIR)/printer.bsc

$(OUTDIR) : 
    if not exist $(OUTDIR)/nul mkdir $(OUTDIR)

MTL=MkTypLib.exe
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
CPP=cl.exe
# ADD BASE CPP /nologo /MD /Gt0 /QMOb2000 /W3 /GX /Zi /YX /Od /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /c
# ADD CPP /nologo /Gt0 /QMOb2000 /W3 /GX /Zi /YX /Od /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "WIN32" /FR /c
CPP_PROJ=/nologo /Gt0 /QMOb2000 /W3 /GX /Zi /YX /Od /D "_DEBUG" /D "_WINDOWS"\
 /D "_MBCS" /D "WIN32" /FR$(INTDIR)/ /Fp$(OUTDIR)/"printer.pch" /Fo$(INTDIR)/\
 /Fd$(OUTDIR)/"printer.pdb" /c 
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
RSC_PROJ=/l 0x409 /fo$(INTDIR)/"printer.res" /d "_DEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# SUBTRACT BASE BSC32 /Iu
# ADD BSC32 /nologo
# SUBTRACT BSC32 /Iu
BSC32_FLAGS=/nologo /o$(OUTDIR)/"printer.bsc" 
BSC32_SBRS= \
	$(INTDIR)/getpdriv.sbr \
	$(INTDIR)/enumprt.sbr \
	$(INTDIR)/paint.sbr \
	$(INTDIR)/getcaps.sbr \
	$(INTDIR)/printer.sbr

$(OUTDIR)/printer.bsc : $(OUTDIR)  $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib odbc32.lib mfc30d.lib mfco30d.lib mfcd30d.lib oleaut32.lib uuid.lib /NOLOGO /SUBSYSTEM:windows /DEBUG /MACHINE:MIPS
# SUBTRACT BASE LINK32 /PDB:none
# ADD LINK32 user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib /NOLOGO /SUBSYSTEM:windows /DEBUG /MACHINE:MIPS
# SUBTRACT LINK32 /PDB:none
LINK32_FLAGS=user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib\
 /NOLOGO /SUBSYSTEM:windows /PDB:$(OUTDIR)/"printer.pdb" /DEBUG /MACHINE:MIPS\
 /OUT:$(OUTDIR)/"printer.exe" 
DEF_FILE=
LINK32_OBJS= \
	$(INTDIR)/getpdriv.obj \
	$(INTDIR)/enumprt.obj \
	$(INTDIR)/paint.obj \
	$(INTDIR)/getcaps.obj \
	$(INTDIR)/printer.res \
	$(INTDIR)/printer.obj

$(OUTDIR)/printer.exe : $(OUTDIR)  $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 0
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "WinRel"
# PROP Intermediate_Dir "WinRel"
OUTDIR=.\WinRel
INTDIR=.\WinRel

ALL : $(OUTDIR)/printer.exe $(OUTDIR)/printer.bsc

$(OUTDIR) : 
    if not exist $(OUTDIR)/nul mkdir $(OUTDIR)

MTL=MkTypLib.exe
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
CPP=cl.exe
# ADD BASE CPP /nologo /MD /Gt0 /QMOb2000 /W3 /GX /YX /O2 /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /c
# ADD CPP /nologo /Gt0 /QMOb2000 /W3 /GX /YX /O2 /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "WIN32" /FR /c
CPP_PROJ=/nologo /Gt0 /QMOb2000 /W3 /GX /YX /O2 /D "NDEBUG" /D "_WINDOWS" /D\
 "_MBCS" /D "WIN32" /FR$(INTDIR)/ /Fp$(OUTDIR)/"printer.pch" /Fo$(INTDIR)/ /c 
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
RSC_PROJ=/l 0x409 /fo$(INTDIR)/"printer.res" /d "NDEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# SUBTRACT BASE BSC32 /Iu
# ADD BSC32 /nologo
# SUBTRACT BSC32 /Iu
BSC32_FLAGS=/nologo /o$(OUTDIR)/"printer.bsc" 
BSC32_SBRS= \
	$(INTDIR)/getpdriv.sbr \
	$(INTDIR)/enumprt.sbr \
	$(INTDIR)/paint.sbr \
	$(INTDIR)/getcaps.sbr \
	$(INTDIR)/printer.sbr

$(OUTDIR)/printer.bsc : $(OUTDIR)  $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib odbc32.lib mfc30.lib mfco30.lib mfcd30.lib oleaut32.lib uuid.lib /NOLOGO /SUBSYSTEM:windows /MACHINE:MIPS
# SUBTRACT BASE LINK32 /PDB:none
# ADD LINK32 user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib /NOLOGO /SUBSYSTEM:windows /MACHINE:MIPS
# SUBTRACT LINK32 /PDB:none
LINK32_FLAGS=user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib\
 /NOLOGO /SUBSYSTEM:windows /PDB:$(OUTDIR)/"printer.pdb" /MACHINE:MIPS\
 /OUT:$(OUTDIR)/"printer.exe" 
DEF_FILE=
LINK32_OBJS= \
	$(INTDIR)/getpdriv.obj \
	$(INTDIR)/enumprt.obj \
	$(INTDIR)/paint.obj \
	$(INTDIR)/getcaps.obj \
	$(INTDIR)/printer.res \
	$(INTDIR)/printer.obj

$(OUTDIR)/printer.exe : $(OUTDIR)  $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 

################################################################################
# Begin Group "Source Files"

################################################################################
# Begin Source File

SOURCE=.\getpdriv.c
DEP_GETPD=\
	.\common.h\
	.\getpdriv.h

!IF  "$(CFG)" == "Win32 (80x86) Release"

$(INTDIR)/getpdriv.obj :  $(SOURCE)  $(DEP_GETPD) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

$(INTDIR)/getpdriv.obj :  $(SOURCE)  $(DEP_GETPD) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

$(INTDIR)/getpdriv.obj :  $(SOURCE)  $(DEP_GETPD) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

$(INTDIR)/getpdriv.obj :  $(SOURCE)  $(DEP_GETPD) $(INTDIR)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\enumprt.c
DEP_ENUMP=\
	.\common.h\
	.\enumprt.h

!IF  "$(CFG)" == "Win32 (80x86) Release"

$(INTDIR)/enumprt.obj :  $(SOURCE)  $(DEP_ENUMP) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

$(INTDIR)/enumprt.obj :  $(SOURCE)  $(DEP_ENUMP) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

$(INTDIR)/enumprt.obj :  $(SOURCE)  $(DEP_ENUMP) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

$(INTDIR)/enumprt.obj :  $(SOURCE)  $(DEP_ENUMP) $(INTDIR)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\paint.c
DEP_PAINT=\
	.\common.h\
	.\paint.h

!IF  "$(CFG)" == "Win32 (80x86) Release"

$(INTDIR)/paint.obj :  $(SOURCE)  $(DEP_PAINT) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

$(INTDIR)/paint.obj :  $(SOURCE)  $(DEP_PAINT) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

$(INTDIR)/paint.obj :  $(SOURCE)  $(DEP_PAINT) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

$(INTDIR)/paint.obj :  $(SOURCE)  $(DEP_PAINT) $(INTDIR)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\getcaps.c
DEP_GETCA=\
	.\common.h\
	.\getcaps.h

!IF  "$(CFG)" == "Win32 (80x86) Release"

$(INTDIR)/getcaps.obj :  $(SOURCE)  $(DEP_GETCA) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

$(INTDIR)/getcaps.obj :  $(SOURCE)  $(DEP_GETCA) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

$(INTDIR)/getcaps.obj :  $(SOURCE)  $(DEP_GETCA) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

$(INTDIR)/getcaps.obj :  $(SOURCE)  $(DEP_GETCA) $(INTDIR)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\printer.rc
DEP_PRINT=\
	.\printer.bmp\
	.\printer.ico\
	.\printer.h\
	.\common.h

!IF  "$(CFG)" == "Win32 (80x86) Release"

$(INTDIR)/printer.res :  $(SOURCE)  $(DEP_PRINT) $(INTDIR)
   $(RSC) $(RSC_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

$(INTDIR)/printer.res :  $(SOURCE)  $(DEP_PRINT) $(INTDIR)
   $(RSC) $(RSC_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

$(INTDIR)/printer.res :  $(SOURCE)  $(DEP_PRINT) $(INTDIR)
   $(RSC) $(RSC_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

$(INTDIR)/printer.res :  $(SOURCE)  $(DEP_PRINT) $(INTDIR)
   $(RSC) $(RSC_PROJ)  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\printer.c
DEP_PRINTE=\
	.\common.h\
	.\printer.h

!IF  "$(CFG)" == "Win32 (80x86) Release"

$(INTDIR)/printer.obj :  $(SOURCE)  $(DEP_PRINTE) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

$(INTDIR)/printer.obj :  $(SOURCE)  $(DEP_PRINTE) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

$(INTDIR)/printer.obj :  $(SOURCE)  $(DEP_PRINTE) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

$(INTDIR)/printer.obj :  $(SOURCE)  $(DEP_PRINTE) $(INTDIR)

!ENDIF 

# End Source File
# End Group
# End Project
################################################################################
