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
!MESSAGE NMAKE /f "stonehng.mak" CFG="Win32 (80x86) Debug"
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

ALL : $(OUTDIR)/stonehng.exe $(OUTDIR)/stonehng.bsc

$(OUTDIR) : 
    if not exist $(OUTDIR)/nul mkdir $(OUTDIR)

MTL=MkTypLib.exe
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
CPP=cl.exe
# ADD BASE CPP /nologo /MD /W3 /GX /YX"glos.h" /O2 /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /c
# ADD CPP /nologo /W3 /GX /YX"glos.h" /O2 /D "NDEBUG" /D "NT" /D "WIN32" /D "NOT_IMPLEMENTED" /D DATADIR= /D TEXTURE=1 /FR /c
CPP_PROJ=/nologo /W3 /GX /YX"glos.h" /O2 /D "NDEBUG" /D "NT" /D "WIN32" /D\
 "NOT_IMPLEMENTED" /D DATADIR= /D TEXTURE=1 /FR$(INTDIR)/\
 /Fp$(OUTDIR)/"stonehng.pch" /Fo$(INTDIR)/ /c 
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
RSC_PROJ=/l 0x409 /fo$(INTDIR)/"stonehen.res" /d "NDEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# SUBTRACT BASE BSC32 /Iu
# ADD BSC32 /nologo
# SUBTRACT BSC32 /Iu
BSC32_FLAGS=/nologo /o$(OUTDIR)/"stonehng.bsc" 
BSC32_SBRS= \
	$(INTDIR)/callback.sbr \
	$(INTDIR)/ellipse.sbr \
	$(INTDIR)/setpixel.sbr \
	$(INTDIR)/timedate.sbr \
	$(INTDIR)/stonehen.sbr \
	$(INTDIR)/rgbimage.sbr \
	$(INTDIR)/stone.sbr \
	$(INTDIR)/ring.sbr \
	$(INTDIR)/telescop.sbr \
	$(INTDIR)/roundwal.sbr \
	$(INTDIR)/point.sbr \
	$(INTDIR)/scene.sbr \
	$(INTDIR)/atmosphe.sbr

$(OUTDIR)/stonehng.bsc : $(OUTDIR)  $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib mfc30.lib mfco30.lib mfcd30.lib mfcuia32.lib mfcans32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib /NOLOGO /SUBSYSTEM:windows /MACHINE:I386
# ADD LINK32 user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib opengl32.lib glu32.lib glaux.lib /NOLOGO /SUBSYSTEM:windows /PDB:none /MACHINE:I386
LINK32_FLAGS=user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib\
 shell32.lib ole32.lib oleaut32.lib uuid.lib opengl32.lib glu32.lib glaux.lib\
 /NOLOGO /SUBSYSTEM:windows /PDB:none /MACHINE:I386\
 /OUT:$(OUTDIR)/"stonehng.exe" 
DEF_FILE=
LINK32_OBJS= \
	$(INTDIR)/callback.obj \
	$(INTDIR)/ellipse.obj \
	$(INTDIR)/setpixel.obj \
	$(INTDIR)/timedate.obj \
	$(INTDIR)/stonehen.res \
	$(INTDIR)/stonehen.obj \
	$(INTDIR)/rgbimage.obj \
	$(INTDIR)/stone.obj \
	$(INTDIR)/ring.obj \
	$(INTDIR)/telescop.obj \
	$(INTDIR)/roundwal.obj \
	$(INTDIR)/point.obj \
	$(INTDIR)/scene.obj \
	$(INTDIR)/atmosphe.obj

$(OUTDIR)/stonehng.exe : $(OUTDIR)  $(DEF_FILE) $(LINK32_OBJS)
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

ALL : $(OUTDIR)/stonehng.exe $(OUTDIR)/stonehng.bsc

$(OUTDIR) : 
    if not exist $(OUTDIR)/nul mkdir $(OUTDIR)

MTL=MkTypLib.exe
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
CPP=cl.exe
# ADD BASE CPP /nologo /MD /W3 /GX /Zi /YX"glos.h" /Od /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /c
# ADD CPP /nologo /W3 /GX /Zi /YX"glos.h" /Od /D "_DEBUG" /D "NT" /D "WIN32" /D "NOT_IMPLEMENTED" /D DATADIR= /D TEXTURE=1 /FR /c
CPP_PROJ=/nologo /W3 /GX /Zi /YX"glos.h" /Od /D "_DEBUG" /D "NT" /D "WIN32" /D\
 "NOT_IMPLEMENTED" /D DATADIR= /D TEXTURE=1 /FR$(INTDIR)/\
 /Fp$(OUTDIR)/"stonehng.pch" /Fo$(INTDIR)/ /Fd$(OUTDIR)/"stonehng.pdb" /c 
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
RSC_PROJ=/l 0x409 /fo$(INTDIR)/"stonehen.res" /d "_DEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# SUBTRACT BASE BSC32 /Iu
# ADD BSC32 /nologo
# SUBTRACT BSC32 /Iu
BSC32_FLAGS=/nologo /o$(OUTDIR)/"stonehng.bsc" 
BSC32_SBRS= \
	$(INTDIR)/callback.sbr \
	$(INTDIR)/ellipse.sbr \
	$(INTDIR)/setpixel.sbr \
	$(INTDIR)/timedate.sbr \
	$(INTDIR)/stonehen.sbr \
	$(INTDIR)/rgbimage.sbr \
	$(INTDIR)/stone.sbr \
	$(INTDIR)/ring.sbr \
	$(INTDIR)/telescop.sbr \
	$(INTDIR)/roundwal.sbr \
	$(INTDIR)/point.sbr \
	$(INTDIR)/scene.sbr \
	$(INTDIR)/atmosphe.sbr

$(OUTDIR)/stonehng.bsc : $(OUTDIR)  $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib mfc30d.lib mfco30d.lib mfcd30d.lib mfcuia32.lib mfcans32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib /NOLOGO /SUBSYSTEM:windows /DEBUG /MACHINE:I386
# ADD LINK32 user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib opengl32.lib glu32.lib glaux.lib /NOLOGO /SUBSYSTEM:windows /PDB:none /DEBUG /MACHINE:I386
LINK32_FLAGS=user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib\
 shell32.lib ole32.lib oleaut32.lib uuid.lib opengl32.lib glu32.lib glaux.lib\
 /NOLOGO /SUBSYSTEM:windows /PDB:none /DEBUG /MACHINE:I386\
 /OUT:$(OUTDIR)/"stonehng.exe" 
DEF_FILE=
LINK32_OBJS= \
	$(INTDIR)/callback.obj \
	$(INTDIR)/ellipse.obj \
	$(INTDIR)/setpixel.obj \
	$(INTDIR)/timedate.obj \
	$(INTDIR)/stonehen.res \
	$(INTDIR)/stonehen.obj \
	$(INTDIR)/rgbimage.obj \
	$(INTDIR)/stone.obj \
	$(INTDIR)/ring.obj \
	$(INTDIR)/telescop.obj \
	$(INTDIR)/roundwal.obj \
	$(INTDIR)/point.obj \
	$(INTDIR)/scene.obj \
	$(INTDIR)/atmosphe.obj

$(OUTDIR)/stonehng.exe : $(OUTDIR)  $(DEF_FILE) $(LINK32_OBJS)
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

ALL : $(OUTDIR)/stonehng.exe $(OUTDIR)/stonehng.bsc

$(OUTDIR) : 
    if not exist $(OUTDIR)/nul mkdir $(OUTDIR)

MTL=MkTypLib.exe
# ADD BASE MTL /nologo /D "NDEBUG" /mips
# ADD MTL /nologo /D "NDEBUG" /mips
MTL_PROJ=/nologo /D "NDEBUG" /mips 
CPP=cl.exe
# ADD BASE CPP /nologo /MD /Gt0 /QMOb2000 /W3 /GX /YX"glos.h" /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /c
# ADD CPP /nologo /Gt0 /QMOb2000 /W3 /GX /YX"glos.h" /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D DATADIR= /FR /c
CPP_PROJ=/nologo /Gt0 /QMOb2000 /W3 /GX /YX"glos.h" /O2 /D "WIN32" /D "NDEBUG"\
 /D "_WINDOWS" /D "_MBCS" /D DATADIR= /FR$(INTDIR)/ /Fp$(OUTDIR)/"stonehng.pch"\
 /Fo$(INTDIR)/ /c 
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
RSC_PROJ=/l 0x409 /fo$(INTDIR)/"stonehen.res" /d "NDEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# SUBTRACT BASE BSC32 /Iu
# ADD BSC32 /nologo
# SUBTRACT BSC32 /Iu
BSC32_FLAGS=/nologo /o$(OUTDIR)/"stonehng.bsc" 
BSC32_SBRS= \
	$(INTDIR)/callback.sbr \
	$(INTDIR)/ellipse.sbr \
	$(INTDIR)/setpixel.sbr \
	$(INTDIR)/timedate.sbr \
	$(INTDIR)/stonehen.sbr \
	$(INTDIR)/rgbimage.sbr \
	$(INTDIR)/stone.sbr \
	$(INTDIR)/ring.sbr \
	$(INTDIR)/telescop.sbr \
	$(INTDIR)/roundwal.sbr \
	$(INTDIR)/point.sbr \
	$(INTDIR)/scene.sbr \
	$(INTDIR)/atmosphe.sbr

$(OUTDIR)/stonehng.bsc : $(OUTDIR)  $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 /NOLOGO /SUBSYSTEM:windows /MACHINE:MIPS
# ADD LINK32 opengl32.lib glu32.lib glaux.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /NOLOGO /SUBSYSTEM:windows /MACHINE:MIPS
LINK32_FLAGS=opengl32.lib glu32.lib glaux.lib kernel32.lib user32.lib gdi32.lib\
 winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib\
 uuid.lib /NOLOGO /SUBSYSTEM:windows /PDB:$(OUTDIR)/"stonehng.pdb" /MACHINE:MIPS\
 /OUT:$(OUTDIR)/"stonehng.exe" 
DEF_FILE=
LINK32_OBJS= \
	$(INTDIR)/callback.obj \
	$(INTDIR)/ellipse.obj \
	$(INTDIR)/setpixel.obj \
	$(INTDIR)/timedate.obj \
	$(INTDIR)/stonehen.res \
	$(INTDIR)/stonehen.obj \
	$(INTDIR)/rgbimage.obj \
	$(INTDIR)/stone.obj \
	$(INTDIR)/ring.obj \
	$(INTDIR)/telescop.obj \
	$(INTDIR)/roundwal.obj \
	$(INTDIR)/point.obj \
	$(INTDIR)/scene.obj \
	$(INTDIR)/atmosphe.obj

$(OUTDIR)/stonehng.exe : $(OUTDIR)  $(DEF_FILE) $(LINK32_OBJS)
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

ALL : $(OUTDIR)/stonehng.exe $(OUTDIR)/stonehng.bsc

$(OUTDIR) : 
    if not exist $(OUTDIR)/nul mkdir $(OUTDIR)

MTL=MkTypLib.exe
# ADD BASE MTL /nologo /D "_DEBUG" /mips
# ADD MTL /nologo /D "_DEBUG" /mips
MTL_PROJ=/nologo /D "_DEBUG" /mips 
CPP=cl.exe
# ADD BASE CPP /nologo /MD /Gt0 /QMOb2000 /W3 /GX /Zi /YX"glos.h" /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /c
# ADD CPP /nologo /Gt0 /QMOb2000 /W3 /GX /Zi /YX"glos.h" /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D DATADIR= /FR /c
CPP_PROJ=/nologo /Gt0 /QMOb2000 /W3 /GX /Zi /YX"glos.h" /Od /D "WIN32" /D\
 "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D DATADIR= /FR$(INTDIR)/\
 /Fp$(OUTDIR)/"stonehng.pch" /Fo$(INTDIR)/ /Fd$(OUTDIR)/"stonehng.pdb" /c 
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
RSC_PROJ=/l 0x409 /fo$(INTDIR)/"stonehen.res" /d "_DEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# SUBTRACT BASE BSC32 /Iu
# ADD BSC32 /nologo
# SUBTRACT BSC32 /Iu
BSC32_FLAGS=/nologo /o$(OUTDIR)/"stonehng.bsc" 
BSC32_SBRS= \
	$(INTDIR)/callback.sbr \
	$(INTDIR)/ellipse.sbr \
	$(INTDIR)/setpixel.sbr \
	$(INTDIR)/timedate.sbr \
	$(INTDIR)/stonehen.sbr \
	$(INTDIR)/rgbimage.sbr \
	$(INTDIR)/stone.sbr \
	$(INTDIR)/ring.sbr \
	$(INTDIR)/telescop.sbr \
	$(INTDIR)/roundwal.sbr \
	$(INTDIR)/point.sbr \
	$(INTDIR)/scene.sbr \
	$(INTDIR)/atmosphe.sbr

$(OUTDIR)/stonehng.bsc : $(OUTDIR)  $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 /NOLOGO /SUBSYSTEM:windows /DEBUG /MACHINE:MIPS
# ADD LINK32 opengl32.lib glu32.lib glaux.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /NOLOGO /SUBSYSTEM:windows /DEBUG /MACHINE:MIPS
LINK32_FLAGS=opengl32.lib glu32.lib glaux.lib kernel32.lib user32.lib gdi32.lib\
 winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib\
 uuid.lib /NOLOGO /SUBSYSTEM:windows /PDB:$(OUTDIR)/"stonehng.pdb" /DEBUG\
 /MACHINE:MIPS /OUT:$(OUTDIR)/"stonehng.exe" 
DEF_FILE=
LINK32_OBJS= \
	$(INTDIR)/callback.obj \
	$(INTDIR)/ellipse.obj \
	$(INTDIR)/setpixel.obj \
	$(INTDIR)/timedate.obj \
	$(INTDIR)/stonehen.res \
	$(INTDIR)/stonehen.obj \
	$(INTDIR)/rgbimage.obj \
	$(INTDIR)/stone.obj \
	$(INTDIR)/ring.obj \
	$(INTDIR)/telescop.obj \
	$(INTDIR)/roundwal.obj \
	$(INTDIR)/point.obj \
	$(INTDIR)/scene.obj \
	$(INTDIR)/atmosphe.obj

$(OUTDIR)/stonehng.exe : $(OUTDIR)  $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 

################################################################################
# Begin Group "Source Files"

################################################################################
# Begin Source File

SOURCE=.\callback.cxx
DEP_CALLB=\
	.\glos.h\
	GL\GLU.H\
	.\stonehen.h\
	.\atmosphe.h\
	.\scene.h\
	.\callback.h\
	.\color.h\
	.\point.h\
	.\timedate.h

!IF  "$(CFG)" == "Win32 (80x86) Release"

$(INTDIR)/callback.obj :  $(SOURCE)  $(DEP_CALLB) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

$(INTDIR)/callback.obj :  $(SOURCE)  $(DEP_CALLB) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

$(INTDIR)/callback.obj :  $(SOURCE)  $(DEP_CALLB) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

$(INTDIR)/callback.obj :  $(SOURCE)  $(DEP_CALLB) $(INTDIR)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\ellipse.cxx
DEP_ELLIP=\
	.\glos.h\
	GL\GLU.H\
	.\stonehen.h\
	.\ellipse.h\
	.\stone.h\
	.\color.h\
	.\point.h

!IF  "$(CFG)" == "Win32 (80x86) Release"

$(INTDIR)/ellipse.obj :  $(SOURCE)  $(DEP_ELLIP) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

$(INTDIR)/ellipse.obj :  $(SOURCE)  $(DEP_ELLIP) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

$(INTDIR)/ellipse.obj :  $(SOURCE)  $(DEP_ELLIP) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

$(INTDIR)/ellipse.obj :  $(SOURCE)  $(DEP_ELLIP) $(INTDIR)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\setpixel.cxx
DEP_SETPI=\
	.\glos.h\
	GL\GL.H\
	.\stonehen.h\
	.\setpixel.h

!IF  "$(CFG)" == "Win32 (80x86) Release"

$(INTDIR)/setpixel.obj :  $(SOURCE)  $(DEP_SETPI) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

$(INTDIR)/setpixel.obj :  $(SOURCE)  $(DEP_SETPI) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

$(INTDIR)/setpixel.obj :  $(SOURCE)  $(DEP_SETPI) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

$(INTDIR)/setpixel.obj :  $(SOURCE)  $(DEP_SETPI) $(INTDIR)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\timedate.cxx
DEP_TIMED=\
	.\glos.h\
	GL\GLU.H\
	.\stonehen.h\
	.\timedate.h\
	.\point.h

!IF  "$(CFG)" == "Win32 (80x86) Release"

$(INTDIR)/timedate.obj :  $(SOURCE)  $(DEP_TIMED) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

$(INTDIR)/timedate.obj :  $(SOURCE)  $(DEP_TIMED) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

$(INTDIR)/timedate.obj :  $(SOURCE)  $(DEP_TIMED) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

$(INTDIR)/timedate.obj :  $(SOURCE)  $(DEP_TIMED) $(INTDIR)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\stonehen.rc

!IF  "$(CFG)" == "Win32 (80x86) Release"

$(INTDIR)/stonehen.res :  $(SOURCE)  $(INTDIR)
   $(RSC) $(RSC_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

$(INTDIR)/stonehen.res :  $(SOURCE)  $(INTDIR)
   $(RSC) $(RSC_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

$(INTDIR)/stonehen.res :  $(SOURCE)  $(INTDIR)
   $(RSC) $(RSC_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

$(INTDIR)/stonehen.res :  $(SOURCE)  $(INTDIR)
   $(RSC) $(RSC_PROJ)  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\stonehen.cxx
DEP_STONE=\
	.\glos.h\
	GL\GL.H\
	GL\GLU.H\
	.\stonehen.h\
	.\atmosphe.h\
	.\scene.h\
	.\callback.h\
	.\setpixel.h\
	.\color.h\
	.\point.h\
	.\timedate.h

!IF  "$(CFG)" == "Win32 (80x86) Release"

$(INTDIR)/stonehen.obj :  $(SOURCE)  $(DEP_STONE) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

$(INTDIR)/stonehen.obj :  $(SOURCE)  $(DEP_STONE) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

$(INTDIR)/stonehen.obj :  $(SOURCE)  $(DEP_STONE) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

$(INTDIR)/stonehen.obj :  $(SOURCE)  $(DEP_STONE) $(INTDIR)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\rgbimage.cxx
DEP_RGBIM=\
	.\glos.h\
	GL\GL.H\
	.\stonehen.h

!IF  "$(CFG)" == "Win32 (80x86) Release"

$(INTDIR)/rgbimage.obj :  $(SOURCE)  $(DEP_RGBIM) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

$(INTDIR)/rgbimage.obj :  $(SOURCE)  $(DEP_RGBIM) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

$(INTDIR)/rgbimage.obj :  $(SOURCE)  $(DEP_RGBIM) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

$(INTDIR)/rgbimage.obj :  $(SOURCE)  $(DEP_RGBIM) $(INTDIR)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\stone.cxx
DEP_STONE_=\
	.\glos.h\
	GL\GLU.H\
	.\stonehen.h\
	.\stone.h\
	.\color.h\
	.\point.h

!IF  "$(CFG)" == "Win32 (80x86) Release"

$(INTDIR)/stone.obj :  $(SOURCE)  $(DEP_STONE_) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

$(INTDIR)/stone.obj :  $(SOURCE)  $(DEP_STONE_) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

$(INTDIR)/stone.obj :  $(SOURCE)  $(DEP_STONE_) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

$(INTDIR)/stone.obj :  $(SOURCE)  $(DEP_STONE_) $(INTDIR)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\ring.cxx
DEP_RING_=\
	.\glos.h\
	GL\GLU.H\
	.\stonehen.h\
	.\ring.h\
	.\stone.h\
	.\color.h\
	.\point.h

!IF  "$(CFG)" == "Win32 (80x86) Release"

$(INTDIR)/ring.obj :  $(SOURCE)  $(DEP_RING_) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

$(INTDIR)/ring.obj :  $(SOURCE)  $(DEP_RING_) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

$(INTDIR)/ring.obj :  $(SOURCE)  $(DEP_RING_) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

$(INTDIR)/ring.obj :  $(SOURCE)  $(DEP_RING_) $(INTDIR)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\telescop.cxx
DEP_TELES=\
	.\glos.h\
	GL\GLU.H\
	.\stonehen.h\
	.\color.h\
	.\telescop.h

!IF  "$(CFG)" == "Win32 (80x86) Release"

$(INTDIR)/telescop.obj :  $(SOURCE)  $(DEP_TELES) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

$(INTDIR)/telescop.obj :  $(SOURCE)  $(DEP_TELES) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

$(INTDIR)/telescop.obj :  $(SOURCE)  $(DEP_TELES) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

$(INTDIR)/telescop.obj :  $(SOURCE)  $(DEP_TELES) $(INTDIR)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\roundwal.cxx
DEP_ROUND=\
	.\glos.h\
	GL\GLU.H\
	.\stonehen.h\
	.\roundwal.h

!IF  "$(CFG)" == "Win32 (80x86) Release"

$(INTDIR)/roundwal.obj :  $(SOURCE)  $(DEP_ROUND) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

$(INTDIR)/roundwal.obj :  $(SOURCE)  $(DEP_ROUND) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

$(INTDIR)/roundwal.obj :  $(SOURCE)  $(DEP_ROUND) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

$(INTDIR)/roundwal.obj :  $(SOURCE)  $(DEP_ROUND) $(INTDIR)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\point.cxx
DEP_POINT=\
	.\glos.h\
	GL\GL.H\
	.\stonehen.h\
	.\point.h

!IF  "$(CFG)" == "Win32 (80x86) Release"

$(INTDIR)/point.obj :  $(SOURCE)  $(DEP_POINT) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

$(INTDIR)/point.obj :  $(SOURCE)  $(DEP_POINT) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

$(INTDIR)/point.obj :  $(SOURCE)  $(DEP_POINT) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

$(INTDIR)/point.obj :  $(SOURCE)  $(DEP_POINT) $(INTDIR)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\scene.cxx
DEP_SCENE=\
	.\glos.h\
	GL\GLU.H\
	GL\GLAUX.H\
	.\stonehen.h\
	.\point.h\
	.\ring.h\
	.\roundwal.h\
	.\ellipse.h\
	.\telescop.h\
	.\scene.h\
	.\stone.h\
	.\timedate.h\
	.\atmosphe.h\
	.\color.h

!IF  "$(CFG)" == "Win32 (80x86) Release"

$(INTDIR)/scene.obj :  $(SOURCE)  $(DEP_SCENE) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

$(INTDIR)/scene.obj :  $(SOURCE)  $(DEP_SCENE) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

$(INTDIR)/scene.obj :  $(SOURCE)  $(DEP_SCENE) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

$(INTDIR)/scene.obj :  $(SOURCE)  $(DEP_SCENE) $(INTDIR)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\atmosphe.cxx
DEP_ATMOS=\
	.\glos.h\
	GL\GLU.H\
	.\stonehen.h\
	.\atmosphe.h\
	.\color.h\
	.\point.h

!IF  "$(CFG)" == "Win32 (80x86) Release"

$(INTDIR)/atmosphe.obj :  $(SOURCE)  $(DEP_ATMOS) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

$(INTDIR)/atmosphe.obj :  $(SOURCE)  $(DEP_ATMOS) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

$(INTDIR)/atmosphe.obj :  $(SOURCE)  $(DEP_ATMOS) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

$(INTDIR)/atmosphe.obj :  $(SOURCE)  $(DEP_ATMOS) $(INTDIR)

!ENDIF 

# End Source File
# End Group
# End Project
################################################################################
