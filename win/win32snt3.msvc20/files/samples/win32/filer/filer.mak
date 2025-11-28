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
!MESSAGE NMAKE /f "filer.mak" CFG="Win32 (80x86) Debug"
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

ALL : $(OUTDIR)/filer.exe $(OUTDIR)/filer.bsc

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
 "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"filer.pch" /Fo$(INTDIR)/ /c 
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
RSC_PROJ=/l 0x409 /fo$(INTDIR)/"filer.res" /d "NDEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# SUBTRACT BASE BSC32 /Iu
# ADD BSC32 /nologo
# SUBTRACT BSC32 /Iu
BSC32_FLAGS=/nologo /o$(OUTDIR)/"filer.bsc" 
BSC32_SBRS= \
	$(INTDIR)/filer.sbr \
	$(INTDIR)/enumdrv.sbr \
	$(INTDIR)/expdir.sbr \
	$(INTDIR)/drvproc.sbr

$(OUTDIR)/filer.bsc : $(OUTDIR)  $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib mfc30.lib mfco30.lib mfcd30.lib /NOLOGO /SUBSYSTEM:windows /MACHINE:I386
# ADD LINK32 user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib version.lib /NOLOGO /SUBSYSTEM:windows /MACHINE:I386
LINK32_FLAGS=user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib\
 version.lib /NOLOGO /SUBSYSTEM:windows /INCREMENTAL:no\
 /PDB:$(OUTDIR)/"filer.pdb" /MACHINE:I386 /OUT:$(OUTDIR)/"filer.exe" 
DEF_FILE=
LINK32_OBJS= \
	$(INTDIR)/filer.res \
	$(INTDIR)/filer.obj \
	$(INTDIR)/enumdrv.obj \
	$(INTDIR)/expdir.obj \
	$(INTDIR)/drvproc.obj

$(OUTDIR)/filer.exe : $(OUTDIR)  $(DEF_FILE) $(LINK32_OBJS)
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

ALL : $(OUTDIR)/filer.exe $(OUTDIR)/filer.bsc

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
 "WIN32" /FR$(INTDIR)/ /Fp$(OUTDIR)/"filer.pch" /Fo$(INTDIR)/\
 /Fd$(OUTDIR)/"filer.pdb" /c 
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
RSC_PROJ=/l 0x409 /fo$(INTDIR)/"filer.res" /d "_DEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# SUBTRACT BASE BSC32 /Iu
# ADD BSC32 /nologo
# SUBTRACT BSC32 /Iu
BSC32_FLAGS=/nologo /o$(OUTDIR)/"filer.bsc" 
BSC32_SBRS= \
	$(INTDIR)/filer.sbr \
	$(INTDIR)/enumdrv.sbr \
	$(INTDIR)/expdir.sbr \
	$(INTDIR)/drvproc.sbr

$(OUTDIR)/filer.bsc : $(OUTDIR)  $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib mfc30d.lib mfco30d.lib mfcd30d.lib /NOLOGO /SUBSYSTEM:windows /DEBUG /MACHINE:I386
# ADD LINK32 user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib version.lib /NOLOGO /SUBSYSTEM:windows /DEBUG /MACHINE:I386
LINK32_FLAGS=user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib\
 version.lib /NOLOGO /SUBSYSTEM:windows /INCREMENTAL:yes\
 /PDB:$(OUTDIR)/"filer.pdb" /DEBUG /MACHINE:I386 /OUT:$(OUTDIR)/"filer.exe" 
DEF_FILE=
LINK32_OBJS= \
	$(INTDIR)/filer.res \
	$(INTDIR)/filer.obj \
	$(INTDIR)/enumdrv.obj \
	$(INTDIR)/expdir.obj \
	$(INTDIR)/drvproc.obj

$(OUTDIR)/filer.exe : $(OUTDIR)  $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 1
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "WinDebug"
# PROP Intermediate_Dir "WinDebug"
OUTDIR=.\WinDebug
INTDIR=.\WinDebug

ALL : $(OUTDIR)/filer.exe $(OUTDIR)/filer.bsc

$(OUTDIR) : 
    if not exist $(OUTDIR)/nul mkdir $(OUTDIR)

MTL=MkTypLib.exe
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
CPP=cl.exe
# ADD BASE CPP /nologo /MD /Gt0 /QMOb2000 /W3 /GX /Zi /YX /Od /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /c
# ADD CPP /nologo /MD /Gt0 /QMOb2000 /W3 /GX /Zi /YX /Od /D "_DEBUG" /D "_AFXDLL" /D "_WINDOWS" /D "_MBCS" /D "WIN32" /FR /c
CPP_PROJ=/nologo /MD /Gt0 /QMOb2000 /W3 /GX /Zi /YX /Od /D "_DEBUG" /D\
 "_AFXDLL" /D "_WINDOWS" /D "_MBCS" /D "WIN32" /FR$(INTDIR)/\
 /Fp$(OUTDIR)/"filer.pch" /Fo$(INTDIR)/ /Fd$(OUTDIR)/"filer.pdb" /c 
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
RSC_PROJ=/l 0x409 /fo$(INTDIR)/"filer.res" /d "_DEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# SUBTRACT BASE BSC32 /Iu
# ADD BSC32 /nologo
# SUBTRACT BSC32 /Iu
BSC32_FLAGS=/nologo /o$(OUTDIR)/"filer.bsc" 
BSC32_SBRS= \
	$(INTDIR)/filer.sbr \
	$(INTDIR)/enumdrv.sbr \
	$(INTDIR)/expdir.sbr \
	$(INTDIR)/drvproc.sbr

$(OUTDIR)/filer.bsc : $(OUTDIR)  $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib odbc32.lib mfc30d.lib mfco30d.lib mfcd30d.lib oleaut32.lib uuid.lib /NOLOGO /SUBSYSTEM:windows /DEBUG /MACHINE:MIPS
# SUBTRACT BASE LINK32 /PDB:none
# ADD LINK32 user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib version.lib /NOLOGO /SUBSYSTEM:windows /DEBUG /MACHINE:MIPS
# SUBTRACT LINK32 /PDB:none
LINK32_FLAGS=user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib\
 version.lib /NOLOGO /SUBSYSTEM:windows /PDB:$(OUTDIR)/"filer.pdb" /DEBUG\
 /MACHINE:MIPS /OUT:$(OUTDIR)/"filer.exe" 
DEF_FILE=
LINK32_OBJS= \
	$(INTDIR)/filer.res \
	$(INTDIR)/filer.obj \
	$(INTDIR)/enumdrv.obj \
	$(INTDIR)/expdir.obj \
	$(INTDIR)/drvproc.obj

$(OUTDIR)/filer.exe : $(OUTDIR)  $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 0
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "WinRel"
# PROP Intermediate_Dir "WinRel"
OUTDIR=.\WinRel
INTDIR=.\WinRel

ALL : $(OUTDIR)/filer.exe $(OUTDIR)/filer.bsc

$(OUTDIR) : 
    if not exist $(OUTDIR)/nul mkdir $(OUTDIR)

MTL=MkTypLib.exe
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
CPP=cl.exe
# ADD BASE CPP /nologo /MD /Gt0 /QMOb2000 /W3 /GX /YX /O2 /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /c
# ADD CPP /nologo /MD /Gt0 /QMOb2000 /W3 /GX /YX /O2 /D "NDEBUG" /D "_AFXDLL" /D "_WINDOWS" /D "_MBCS" /D "WIN32" /FR /c
CPP_PROJ=/nologo /MD /Gt0 /QMOb2000 /W3 /GX /YX /O2 /D "NDEBUG" /D "_AFXDLL" /D\
 "_WINDOWS" /D "_MBCS" /D "WIN32" /FR$(INTDIR)/ /Fp$(OUTDIR)/"filer.pch"\
 /Fo$(INTDIR)/ /c 
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
RSC_PROJ=/l 0x409 /fo$(INTDIR)/"filer.res" /d "NDEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# SUBTRACT BASE BSC32 /Iu
# ADD BSC32 /nologo
# SUBTRACT BSC32 /Iu
BSC32_FLAGS=/nologo /o$(OUTDIR)/"filer.bsc" 
BSC32_SBRS= \
	$(INTDIR)/filer.sbr \
	$(INTDIR)/enumdrv.sbr \
	$(INTDIR)/expdir.sbr \
	$(INTDIR)/drvproc.sbr

$(OUTDIR)/filer.bsc : $(OUTDIR)  $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib odbc32.lib mfc30.lib mfco30.lib mfcd30.lib oleaut32.lib uuid.lib /NOLOGO /SUBSYSTEM:windows /MACHINE:MIPS
# SUBTRACT BASE LINK32 /PDB:none
# ADD LINK32 user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib version.lib /NOLOGO /SUBSYSTEM:windows /MACHINE:MIPS
# SUBTRACT LINK32 /PDB:none
LINK32_FLAGS=user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib\
 version.lib /NOLOGO /SUBSYSTEM:windows /PDB:$(OUTDIR)/"filer.pdb" /MACHINE:MIPS\
 /OUT:$(OUTDIR)/"filer.exe" 
DEF_FILE=
LINK32_OBJS= \
	$(INTDIR)/filer.res \
	$(INTDIR)/filer.obj \
	$(INTDIR)/enumdrv.obj \
	$(INTDIR)/expdir.obj \
	$(INTDIR)/drvproc.obj

$(OUTDIR)/filer.exe : $(OUTDIR)  $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 

################################################################################
# Begin Group "Source Files"

################################################################################
# Begin Source File

SOURCE=.\filer.rc
DEP_FILER=\
	.\filer.ico\
	.\floppy.bmp\
	.\floppy2.bmp\
	.\remote.bmp\
	.\remote2.bmp\
	.\cd.bmp\
	.\cd2.bmp\
	.\fixed.bmp\
	.\fixed2.bmp\
	.\globals.h

!IF  "$(CFG)" == "Win32 (80x86) Release"

$(INTDIR)/filer.res :  $(SOURCE)  $(DEP_FILER) $(INTDIR)
   $(RSC) $(RSC_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

$(INTDIR)/filer.res :  $(SOURCE)  $(DEP_FILER) $(INTDIR)
   $(RSC) $(RSC_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

$(INTDIR)/filer.res :  $(SOURCE)  $(DEP_FILER) $(INTDIR)
   $(RSC) $(RSC_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

$(INTDIR)/filer.res :  $(SOURCE)  $(DEP_FILER) $(INTDIR)
   $(RSC) $(RSC_PROJ)  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\filer.c
DEP_FILER_=\
	.\globals.h\
	.\enumdrv.h\
	.\drvproc.h\
	.\filer.h

!IF  "$(CFG)" == "Win32 (80x86) Release"

$(INTDIR)/filer.obj :  $(SOURCE)  $(DEP_FILER_) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

$(INTDIR)/filer.obj :  $(SOURCE)  $(DEP_FILER_) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

$(INTDIR)/filer.obj :  $(SOURCE)  $(DEP_FILER_) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

$(INTDIR)/filer.obj :  $(SOURCE)  $(DEP_FILER_) $(INTDIR)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\enumdrv.c
DEP_ENUMD=\
	.\globals.h\
	.\filer.h\
	.\enumdrv.h

!IF  "$(CFG)" == "Win32 (80x86) Release"

$(INTDIR)/enumdrv.obj :  $(SOURCE)  $(DEP_ENUMD) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

$(INTDIR)/enumdrv.obj :  $(SOURCE)  $(DEP_ENUMD) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

$(INTDIR)/enumdrv.obj :  $(SOURCE)  $(DEP_ENUMD) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

$(INTDIR)/enumdrv.obj :  $(SOURCE)  $(DEP_ENUMD) $(INTDIR)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\expdir.c
DEP_EXPDI=\
	.\globals.h\
	.\filer.h\
	.\drvproc.h\
	.\expdir.h

!IF  "$(CFG)" == "Win32 (80x86) Release"

$(INTDIR)/expdir.obj :  $(SOURCE)  $(DEP_EXPDI) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

$(INTDIR)/expdir.obj :  $(SOURCE)  $(DEP_EXPDI) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

$(INTDIR)/expdir.obj :  $(SOURCE)  $(DEP_EXPDI) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

$(INTDIR)/expdir.obj :  $(SOURCE)  $(DEP_EXPDI) $(INTDIR)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\drvproc.c
DEP_DRVPR=\
	.\globals.h\
	.\filer.h\
	.\expdir.h\
	.\drvproc.h

!IF  "$(CFG)" == "Win32 (80x86) Release"

$(INTDIR)/drvproc.obj :  $(SOURCE)  $(DEP_DRVPR) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

$(INTDIR)/drvproc.obj :  $(SOURCE)  $(DEP_DRVPR) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

$(INTDIR)/drvproc.obj :  $(SOURCE)  $(DEP_DRVPR) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

$(INTDIR)/drvproc.obj :  $(SOURCE)  $(DEP_DRVPR) $(INTDIR)

!ENDIF 

# End Source File
# End Group
# End Project
################################################################################
