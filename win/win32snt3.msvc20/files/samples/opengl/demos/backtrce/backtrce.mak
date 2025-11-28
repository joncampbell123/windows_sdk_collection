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
!MESSAGE NMAKE /f "backtrce.mak" CFG="Win32 (80x86) Debug"
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

ALL : $(OUTDIR)/backtrce.exe $(OUTDIR)/backtrce.bsc

$(OUTDIR) : 
    if not exist $(OUTDIR)/nul mkdir $(OUTDIR)

MTL=MkTypLib.exe
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
CPP=cl.exe
# ADD BASE CPP /nologo /MD /W3 /GX /YX"glos.h" /O2 /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /c
# ADD CPP /W3 /GX /YX"glos.h" /O2 /D "NDEBUG" /D TEXTURE=1 /D "WIN32" /FR /c
# SUBTRACT CPP /nologo
CPP_PROJ=/W3 /GX /YX"glos.h" /O2 /D "NDEBUG" /D TEXTURE=1 /D "WIN32"\
 /FR$(INTDIR)/ /Fp$(OUTDIR)/"backtrce.pch" /Fo$(INTDIR)/ /c 
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
RSC_PROJ=/l 0x409 /fo$(INTDIR)/"backtrac.res" /d "NDEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# SUBTRACT BASE BSC32 /Iu
# ADD BSC32 /nologo
# SUBTRACT BSC32 /Iu
BSC32_FLAGS=/nologo /o$(OUTDIR)/"backtrce.bsc" 
BSC32_SBRS= \
	$(INTDIR)/unitdisk.sbr \
	$(INTDIR)/point.sbr \
	$(INTDIR)/scene.sbr \
	$(INTDIR)/cbacks.sbr \
	$(INTDIR)/backtrac.sbr

$(OUTDIR)/backtrce.bsc : $(OUTDIR)  $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib mfc30.lib mfco30.lib mfcd30.lib mfcuia32.lib mfcans32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib /NOLOGO /SUBSYSTEM:windows /MACHINE:I386
# ADD LINK32 libc.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib opengl32.lib glu32.lib glaux.lib /ENTRY:"mainCRTStartup" /SUBSYSTEM:windows /PDB:none /MACHINE:I386 /NODEFAULTLIB
LINK32_FLAGS=libc.lib kernel32.lib user32.lib gdi32.lib winspool.lib\
 comdlg32.lib advapi32.lib opengl32.lib glu32.lib glaux.lib\
 /ENTRY:"mainCRTStartup" /SUBSYSTEM:windows /PDB:none /MACHINE:I386\
 /NODEFAULTLIB /OUT:$(OUTDIR)/"backtrce.exe" 
DEF_FILE=
LINK32_OBJS= \
	$(INTDIR)/unitdisk.obj \
	$(INTDIR)/point.obj \
	$(INTDIR)/scene.obj \
	$(INTDIR)/cbacks.obj \
	$(INTDIR)/backtrac.res \
	$(INTDIR)/backtrac.obj

$(OUTDIR)/backtrce.exe : $(OUTDIR)  $(DEF_FILE) $(LINK32_OBJS)
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

ALL : $(OUTDIR)/backtrce.exe $(OUTDIR)/backtrce.bsc

$(OUTDIR) : 
    if not exist $(OUTDIR)/nul mkdir $(OUTDIR)

MTL=MkTypLib.exe
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
CPP=cl.exe
# ADD BASE CPP /nologo /MD /W3 /GX /Zi /YX"glos.h" /Od /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /c
# ADD CPP /W3 /GX /Zi /YX"glos.h" /Od /D "_DEBUG" /D TEXTURE=1 /D "WIN32" /FR /c
# SUBTRACT CPP /nologo
CPP_PROJ=/W3 /GX /Zi /YX"glos.h" /Od /D "_DEBUG" /D TEXTURE=1 /D "WIN32"\
 /FR$(INTDIR)/ /Fp$(OUTDIR)/"backtrce.pch" /Fo$(INTDIR)/\
 /Fd$(OUTDIR)/"backtrce.pdb" /c 
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
RSC_PROJ=/l 0x409 /fo$(INTDIR)/"backtrac.res" /d "_DEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# SUBTRACT BASE BSC32 /Iu
# ADD BSC32 /nologo
# SUBTRACT BSC32 /Iu
BSC32_FLAGS=/nologo /o$(OUTDIR)/"backtrce.bsc" 
BSC32_SBRS= \
	$(INTDIR)/unitdisk.sbr \
	$(INTDIR)/point.sbr \
	$(INTDIR)/scene.sbr \
	$(INTDIR)/cbacks.sbr \
	$(INTDIR)/backtrac.sbr

$(OUTDIR)/backtrce.bsc : $(OUTDIR)  $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib mfc30d.lib mfco30d.lib mfcd30d.lib mfcuia32.lib mfcans32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib /NOLOGO /SUBSYSTEM:windows /DEBUG /MACHINE:I386
# ADD LINK32 libc.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib opengl32.lib glu32.lib glaux.lib /ENTRY:"mainCRTStartup" /SUBSYSTEM:windows /PDB:none /DEBUG /MACHINE:I386 /NODEFAULTLIB
LINK32_FLAGS=libc.lib kernel32.lib user32.lib gdi32.lib winspool.lib\
 comdlg32.lib advapi32.lib opengl32.lib glu32.lib glaux.lib\
 /ENTRY:"mainCRTStartup" /SUBSYSTEM:windows /PDB:none /DEBUG /MACHINE:I386\
 /NODEFAULTLIB /OUT:$(OUTDIR)/"backtrce.exe" 
DEF_FILE=
LINK32_OBJS= \
	$(INTDIR)/unitdisk.obj \
	$(INTDIR)/point.obj \
	$(INTDIR)/scene.obj \
	$(INTDIR)/cbacks.obj \
	$(INTDIR)/backtrac.res \
	$(INTDIR)/backtrac.obj

$(OUTDIR)/backtrce.exe : $(OUTDIR)  $(DEF_FILE) $(LINK32_OBJS)
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

ALL : $(OUTDIR)/backtrce.exe $(OUTDIR)/backtrce.bsc

$(OUTDIR) : 
    if not exist $(OUTDIR)/nul mkdir $(OUTDIR)

MTL=MkTypLib.exe
# ADD BASE MTL /nologo /D "NDEBUG" /mips
# ADD MTL /nologo /D "NDEBUG" /mips
MTL_PROJ=/nologo /D "NDEBUG" /mips 
CPP=cl.exe
# ADD BASE CPP /nologo /MD /Gt0 /QMOb2000 /W3 /GX /YX"glos.h" /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /c
# ADD CPP /nologo /Gt0 /QMOb2000 /W3 /GX /YX"glos.h" /O2 /D "NDEBUG" /D TEXTURE=1 /D "WIN32" /FR /c
CPP_PROJ=/nologo /Gt0 /QMOb2000 /W3 /GX /YX"glos.h" /O2 /D "NDEBUG" /D\
 TEXTURE=1 /D "WIN32" /FR$(INTDIR)/ /Fp$(OUTDIR)/"backtrce.pch" /Fo$(INTDIR)/ /c\
 
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
RSC_PROJ=/l 0x409 /fo$(INTDIR)/"backtrac.res" /d "NDEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# SUBTRACT BASE BSC32 /Iu
# ADD BSC32 /nologo
# SUBTRACT BSC32 /Iu
BSC32_FLAGS=/nologo /o$(OUTDIR)/"backtrce.bsc" 
BSC32_SBRS= \
	$(INTDIR)/unitdisk.sbr \
	$(INTDIR)/point.sbr \
	$(INTDIR)/scene.sbr \
	$(INTDIR)/cbacks.sbr \
	$(INTDIR)/backtrac.sbr

$(OUTDIR)/backtrce.bsc : $(OUTDIR)  $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 /NOLOGO /SUBSYSTEM:windows /MACHINE:MIPS
# SUBTRACT BASE LINK32 /PDB:none
# ADD LINK32 libc.lib opengl32.lib glu32.lib glaux.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /NOLOGO /ENTRY:"mainCRTStartup" /SUBSYSTEM:windows /MACHINE:MIPS
# SUBTRACT LINK32 /PDB:none
LINK32_FLAGS=libc.lib opengl32.lib glu32.lib glaux.lib kernel32.lib user32.lib\
 gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib\
 oleaut32.lib uuid.lib /NOLOGO /ENTRY:"mainCRTStartup" /SUBSYSTEM:windows\
 /PDB:$(OUTDIR)/"backtrce.pdb" /MACHINE:MIPS /OUT:$(OUTDIR)/"backtrce.exe" 
DEF_FILE=
LINK32_OBJS= \
	$(INTDIR)/unitdisk.obj \
	$(INTDIR)/point.obj \
	$(INTDIR)/scene.obj \
	$(INTDIR)/cbacks.obj \
	$(INTDIR)/backtrac.res \
	$(INTDIR)/backtrac.obj

$(OUTDIR)/backtrce.exe : $(OUTDIR)  $(DEF_FILE) $(LINK32_OBJS)
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

ALL : $(OUTDIR)/backtrce.exe $(OUTDIR)/backtrce.bsc

$(OUTDIR) : 
    if not exist $(OUTDIR)/nul mkdir $(OUTDIR)

MTL=MkTypLib.exe
# ADD BASE MTL /nologo /D "_DEBUG" /mips
# ADD MTL /nologo /D "_DEBUG" /mips
MTL_PROJ=/nologo /D "_DEBUG" /mips 
CPP=cl.exe
# ADD BASE CPP /nologo /MD /Gt0 /QMOb2000 /W3 /GX /Zi /YX"glos.h" /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /c
# ADD CPP /nologo /Gt0 /QMOb2000 /W3 /GX /Zi /YX"glos.h" /Od /D "_DEBUG" /D TEXTURE=1 /D "WIN32" /FR /c
CPP_PROJ=/nologo /Gt0 /QMOb2000 /W3 /GX /Zi /YX"glos.h" /Od /D "_DEBUG" /D\
 TEXTURE=1 /D "WIN32" /FR$(INTDIR)/ /Fp$(OUTDIR)/"backtrce.pch" /Fo$(INTDIR)/\
 /Fd$(OUTDIR)/"backtrce.pdb" /c 
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
RSC_PROJ=/l 0x409 /fo$(INTDIR)/"backtrac.res" /d "_DEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# SUBTRACT BASE BSC32 /Iu
# ADD BSC32 /nologo
# SUBTRACT BSC32 /Iu
BSC32_FLAGS=/nologo /o$(OUTDIR)/"backtrce.bsc" 
BSC32_SBRS= \
	$(INTDIR)/unitdisk.sbr \
	$(INTDIR)/point.sbr \
	$(INTDIR)/scene.sbr \
	$(INTDIR)/cbacks.sbr \
	$(INTDIR)/backtrac.sbr

$(OUTDIR)/backtrce.bsc : $(OUTDIR)  $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 /NOLOGO /SUBSYSTEM:windows /DEBUG /MACHINE:MIPS
# SUBTRACT BASE LINK32 /PDB:none
# ADD LINK32 libc.lib opengl32.lib glu32.lib glaux.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /NOLOGO /ENTRY:"mainCRTStartup" /SUBSYSTEM:windows /DEBUG /MACHINE:MIPS
# SUBTRACT LINK32 /PDB:none
LINK32_FLAGS=libc.lib opengl32.lib glu32.lib glaux.lib kernel32.lib user32.lib\
 gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib\
 oleaut32.lib uuid.lib /NOLOGO /ENTRY:"mainCRTStartup" /SUBSYSTEM:windows\
 /PDB:$(OUTDIR)/"backtrce.pdb" /DEBUG /MACHINE:MIPS\
 /OUT:$(OUTDIR)/"backtrce.exe" 
DEF_FILE=
LINK32_OBJS= \
	$(INTDIR)/unitdisk.obj \
	$(INTDIR)/point.obj \
	$(INTDIR)/scene.obj \
	$(INTDIR)/cbacks.obj \
	$(INTDIR)/backtrac.res \
	$(INTDIR)/backtrac.obj

$(OUTDIR)/backtrce.exe : $(OUTDIR)  $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 

################################################################################
# Begin Group "Source Files"

################################################################################
# Begin Source File

SOURCE=.\unitdisk.cxx
DEP_UNITD=\
	.\glos.h\
	.\GL\GLU.H\
	.\unitdisk.hxx\
	.\color.hxx\
	.\point.hxx

!IF  "$(CFG)" == "Win32 (80x86) Release"

$(INTDIR)/unitdisk.obj :  $(SOURCE)  $(DEP_UNITD) $(INTDIR)
   $(CPP) /W3 /GX /YX"glos.h" /O2 /D "NDEBUG" /D TEXTURE=1 /D "WIN32"\
 /FR$(INTDIR)/ /Fp$(OUTDIR)/"backtrce.pch" /Fo$(INTDIR)/ /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

$(INTDIR)/unitdisk.obj :  $(SOURCE)  $(DEP_UNITD) $(INTDIR)
   $(CPP) /W3 /GX /Zi /YX"glos.h" /Od /D "_DEBUG" /D TEXTURE=1 /D "WIN32"\
 /FR$(INTDIR)/ /Fp$(OUTDIR)/"backtrce.pch" /Fo$(INTDIR)/\
 /Fd$(OUTDIR)/"backtrce.pdb" /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

# ADD BASE CPP /Gt0 /QMOb2000
# ADD CPP /Gt0 /QMOb2000

$(INTDIR)/unitdisk.obj :  $(SOURCE)  $(DEP_UNITD) $(INTDIR)
   $(CPP) /nologo /Gt0 /QMOb2000 /W3 /GX /YX"glos.h" /O2 /D "NDEBUG" /D\
 TEXTURE=1 /D "WIN32" /FR$(INTDIR)/ /Fp$(OUTDIR)/"backtrce.pch" /Fo$(INTDIR)/ /c\
  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

# ADD BASE CPP /Gt0 /QMOb2000
# ADD CPP /Gt0 /QMOb2000

$(INTDIR)/unitdisk.obj :  $(SOURCE)  $(DEP_UNITD) $(INTDIR)
   $(CPP) /nologo /Gt0 /QMOb2000 /W3 /GX /Zi /YX"glos.h" /Od /D "_DEBUG" /D\
 TEXTURE=1 /D "WIN32" /FR$(INTDIR)/ /Fp$(OUTDIR)/"backtrce.pch" /Fo$(INTDIR)/\
 /Fd$(OUTDIR)/"backtrce.pdb" /c  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\point.cxx
DEP_POINT=\
	.\glos.h\
	.\GL\GL.H\
	.\point.hxx

!IF  "$(CFG)" == "Win32 (80x86) Release"

$(INTDIR)/point.obj :  $(SOURCE)  $(DEP_POINT) $(INTDIR)
   $(CPP) /W3 /GX /YX"glos.h" /O2 /D "NDEBUG" /D TEXTURE=1 /D "WIN32"\
 /FR$(INTDIR)/ /Fp$(OUTDIR)/"backtrce.pch" /Fo$(INTDIR)/ /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

$(INTDIR)/point.obj :  $(SOURCE)  $(DEP_POINT) $(INTDIR)
   $(CPP) /W3 /GX /Zi /YX"glos.h" /Od /D "_DEBUG" /D TEXTURE=1 /D "WIN32"\
 /FR$(INTDIR)/ /Fp$(OUTDIR)/"backtrce.pch" /Fo$(INTDIR)/\
 /Fd$(OUTDIR)/"backtrce.pdb" /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

# ADD BASE CPP /Gt0 /QMOb2000
# ADD CPP /Gt0 /QMOb2000

$(INTDIR)/point.obj :  $(SOURCE)  $(DEP_POINT) $(INTDIR)
   $(CPP) /nologo /Gt0 /QMOb2000 /W3 /GX /YX"glos.h" /O2 /D "NDEBUG" /D\
 TEXTURE=1 /D "WIN32" /FR$(INTDIR)/ /Fp$(OUTDIR)/"backtrce.pch" /Fo$(INTDIR)/ /c\
  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

# ADD BASE CPP /Gt0 /QMOb2000
# ADD CPP /Gt0 /QMOb2000

$(INTDIR)/point.obj :  $(SOURCE)  $(DEP_POINT) $(INTDIR)
   $(CPP) /nologo /Gt0 /QMOb2000 /W3 /GX /Zi /YX"glos.h" /Od /D "_DEBUG" /D\
 TEXTURE=1 /D "WIN32" /FR$(INTDIR)/ /Fp$(OUTDIR)/"backtrce.pch" /Fo$(INTDIR)/\
 /Fd$(OUTDIR)/"backtrce.pdb" /c  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\scene.cxx
DEP_SCENE=\
	.\glos.h\
	.\GL\GLAUX.H\
	.\GL\GLU.H\
	.\GL\GL.H\
	.\unitdisk.hxx\
	.\scene.hxx\
	.\color.hxx\
	.\point.hxx

!IF  "$(CFG)" == "Win32 (80x86) Release"

$(INTDIR)/scene.obj :  $(SOURCE)  $(DEP_SCENE) $(INTDIR)
   $(CPP) /W3 /GX /YX"glos.h" /O2 /D "NDEBUG" /D TEXTURE=1 /D "WIN32"\
 /FR$(INTDIR)/ /Fp$(OUTDIR)/"backtrce.pch" /Fo$(INTDIR)/ /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

$(INTDIR)/scene.obj :  $(SOURCE)  $(DEP_SCENE) $(INTDIR)
   $(CPP) /W3 /GX /Zi /YX"glos.h" /Od /D "_DEBUG" /D TEXTURE=1 /D "WIN32"\
 /FR$(INTDIR)/ /Fp$(OUTDIR)/"backtrce.pch" /Fo$(INTDIR)/\
 /Fd$(OUTDIR)/"backtrce.pdb" /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

# ADD BASE CPP /Gt0 /QMOb2000
# ADD CPP /Gt0 /QMOb2000

$(INTDIR)/scene.obj :  $(SOURCE)  $(DEP_SCENE) $(INTDIR)
   $(CPP) /nologo /Gt0 /QMOb2000 /W3 /GX /YX"glos.h" /O2 /D "NDEBUG" /D\
 TEXTURE=1 /D "WIN32" /FR$(INTDIR)/ /Fp$(OUTDIR)/"backtrce.pch" /Fo$(INTDIR)/ /c\
  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

# ADD BASE CPP /Gt0 /QMOb2000
# ADD CPP /Gt0 /QMOb2000

$(INTDIR)/scene.obj :  $(SOURCE)  $(DEP_SCENE) $(INTDIR)
   $(CPP) /nologo /Gt0 /QMOb2000 /W3 /GX /Zi /YX"glos.h" /Od /D "_DEBUG" /D\
 TEXTURE=1 /D "WIN32" /FR$(INTDIR)/ /Fp$(OUTDIR)/"backtrce.pch" /Fo$(INTDIR)/\
 /Fd$(OUTDIR)/"backtrce.pdb" /c  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\cbacks.cxx
DEP_CBACK=\
	.\glos.h\
	.\GL\GLU.H\
	.\GL\GL.H\
	.\GL\GLAUX.H\
	.\scene.hxx\
	.\cbacks.hxx\
	.\point.hxx

!IF  "$(CFG)" == "Win32 (80x86) Release"

$(INTDIR)/cbacks.obj :  $(SOURCE)  $(DEP_CBACK) $(INTDIR)
   $(CPP) /W3 /GX /YX"glos.h" /O2 /D "NDEBUG" /D TEXTURE=1 /D "WIN32"\
 /FR$(INTDIR)/ /Fp$(OUTDIR)/"backtrce.pch" /Fo$(INTDIR)/ /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

$(INTDIR)/cbacks.obj :  $(SOURCE)  $(DEP_CBACK) $(INTDIR)
   $(CPP) /W3 /GX /Zi /YX"glos.h" /Od /D "_DEBUG" /D TEXTURE=1 /D "WIN32"\
 /FR$(INTDIR)/ /Fp$(OUTDIR)/"backtrce.pch" /Fo$(INTDIR)/\
 /Fd$(OUTDIR)/"backtrce.pdb" /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

# ADD BASE CPP /Gt0 /QMOb2000
# ADD CPP /Gt0 /QMOb2000

$(INTDIR)/cbacks.obj :  $(SOURCE)  $(DEP_CBACK) $(INTDIR)
   $(CPP) /nologo /Gt0 /QMOb2000 /W3 /GX /YX"glos.h" /O2 /D "NDEBUG" /D\
 TEXTURE=1 /D "WIN32" /FR$(INTDIR)/ /Fp$(OUTDIR)/"backtrce.pch" /Fo$(INTDIR)/ /c\
  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

# ADD BASE CPP /Gt0 /QMOb2000
# ADD CPP /Gt0 /QMOb2000

$(INTDIR)/cbacks.obj :  $(SOURCE)  $(DEP_CBACK) $(INTDIR)
   $(CPP) /nologo /Gt0 /QMOb2000 /W3 /GX /Zi /YX"glos.h" /Od /D "_DEBUG" /D\
 TEXTURE=1 /D "WIN32" /FR$(INTDIR)/ /Fp$(OUTDIR)/"backtrce.pch" /Fo$(INTDIR)/\
 /Fd$(OUTDIR)/"backtrce.pdb" /c  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\backtrac.rc

!IF  "$(CFG)" == "Win32 (80x86) Release"

$(INTDIR)/backtrac.res :  $(SOURCE)  $(INTDIR)
   $(RSC) $(RSC_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

$(INTDIR)/backtrac.res :  $(SOURCE)  $(INTDIR)
   $(RSC) $(RSC_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

$(INTDIR)/backtrac.res :  $(SOURCE)  $(INTDIR)
   $(RSC) $(RSC_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

$(INTDIR)/backtrac.res :  $(SOURCE)  $(INTDIR)
   $(RSC) $(RSC_PROJ)  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\backtrac.cxx
DEP_BACKT=\
	.\glos.h\
	.\GL\GLU.H\
	.\GL\GL.H\
	.\GL\GLAUX.H\
	.\scene.hxx\
	.\cbacks.hxx\
	.\point.hxx

!IF  "$(CFG)" == "Win32 (80x86) Release"

$(INTDIR)/backtrac.obj :  $(SOURCE)  $(DEP_BACKT) $(INTDIR)
   $(CPP) /W3 /GX /YX"glos.h" /O2 /D "NDEBUG" /D TEXTURE=1 /D "WIN32"\
 /FR$(INTDIR)/ /Fp$(OUTDIR)/"backtrce.pch" /Fo$(INTDIR)/ /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

$(INTDIR)/backtrac.obj :  $(SOURCE)  $(DEP_BACKT) $(INTDIR)
   $(CPP) /W3 /GX /Zi /YX"glos.h" /Od /D "_DEBUG" /D TEXTURE=1 /D "WIN32"\
 /FR$(INTDIR)/ /Fp$(OUTDIR)/"backtrce.pch" /Fo$(INTDIR)/\
 /Fd$(OUTDIR)/"backtrce.pdb" /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

# ADD BASE CPP /Gt0 /QMOb2000
# ADD CPP /Gt0 /QMOb2000

$(INTDIR)/backtrac.obj :  $(SOURCE)  $(DEP_BACKT) $(INTDIR)
   $(CPP) /nologo /Gt0 /QMOb2000 /W3 /GX /YX"glos.h" /O2 /D "NDEBUG" /D\
 TEXTURE=1 /D "WIN32" /FR$(INTDIR)/ /Fp$(OUTDIR)/"backtrce.pch" /Fo$(INTDIR)/ /c\
  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

# ADD BASE CPP /Gt0 /QMOb2000
# ADD CPP /Gt0 /QMOb2000

$(INTDIR)/backtrac.obj :  $(SOURCE)  $(DEP_BACKT) $(INTDIR)
   $(CPP) /nologo /Gt0 /QMOb2000 /W3 /GX /Zi /YX"glos.h" /Od /D "_DEBUG" /D\
 TEXTURE=1 /D "WIN32" /FR$(INTDIR)/ /Fp$(OUTDIR)/"backtrce.pch" /Fo$(INTDIR)/\
 /Fd$(OUTDIR)/"backtrce.pdb" /c  $(SOURCE) 

!ENDIF 

# End Source File
# End Group
# End Project
################################################################################
