# Microsoft Visual C++ Generated NMAKE File, Format Version 2.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (MIPS) Dynamic-Link Library" 0x0502
# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

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
!MESSAGE NMAKE /f "gizmobar.mak" CFG="Win32 (80x86) Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Win32 (80x86) Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "Win32 (80x86) Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "Win32 (MIPS) Release" (based on "Win32 (MIPS) Dynamic-Link Library")
!MESSAGE "Win32 (MIPS) Debug" (based on "Win32 (MIPS) Dynamic-Link Library")
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
# PROP Output_Dir "..\WinRel"
# PROP Intermediate_Dir "WinRel"
OUTDIR=..\WinRel
INTDIR=.\WinRel

ALL : $(OUTDIR)/gizmobar.dll $(OUTDIR)/gizmobar.bsc

$(OUTDIR) : 
    if not exist $(OUTDIR)/nul mkdir $(OUTDIR)

$(INTDIR) : 
    if not exist $(INTDIR)/nul mkdir $(INTDIR)

MTL=MkTypLib.exe
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
CPP=cl.exe
# ADD BASE CPP /nologo /MD /W3 /GX /YX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /c
# ADD CPP /nologo /MT /W3 /GX /YX /O2 /I "..\include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /FR /c
CPP_PROJ=/nologo /MT /W3 /GX /YX /O2 /I "..\include" /D "WIN32" /D "NDEBUG" /D\
 "_WINDOWS" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"gizmobar.pch" /Fo$(INTDIR)/\
 /c 
CPP_OBJS=.\WinRel/

.c{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

RSC=rc.exe
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /i "..\include" /d "NDEBUG"
RSC_PROJ=/l 0x409 /fo$(INTDIR)/"gizmobar.res" /i "..\include" /d "NDEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# SUBTRACT BASE BSC32 /Iu
# ADD BSC32 /nologo
# SUBTRACT BSC32 /Iu
BSC32_FLAGS=/nologo /o$(OUTDIR)/"gizmobar.bsc" 
BSC32_SBRS= \
	.\WinRel\api.sbr \
	.\WinRel\paint.sbr \
	.\WinRel\gizmo.sbr \
	.\WinRel\init.sbr \
	.\WinRel\gizmobar.sbr

$(OUTDIR)/gizmobar.bsc : $(OUTDIR)  $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 /NOLOGO /SUBSYSTEM:windows /DLL /MACHINE:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib ..\bttncur\bttncur.lib /NOLOGO /ENTRY:"LibMain@12" /SUBSYSTEM:windows /DLL /MACHINE:I386 /IMPLIB:"GIZMOBAR.lib"
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib ..\bttncur\bttncur.lib /NOLOGO /ENTRY:"LibMain@12"\
 /SUBSYSTEM:windows /DLL /INCREMENTAL:no /PDB:$(OUTDIR)/"gizmobar.pdb"\
 /MACHINE:I386 /DEF:".\gizmobar.def" /OUT:$(OUTDIR)/"gizmobar.dll"\
 /IMPLIB:"GIZMOBAR.lib" 
DEF_FILE=.\gizmobar.def
LINK32_OBJS= \
	.\WinRel\api.obj \
	.\WinRel\paint.obj \
	.\WinRel\gizmobar.res \
	.\WinRel\gizmo.obj \
	.\WinRel\init.obj \
	.\WinRel\gizmobar.obj

$(OUTDIR)/gizmobar.dll : $(OUTDIR)  $(DEF_FILE) $(LINK32_OBJS)
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
# PROP Output_Dir "..\WinDebug"
# PROP Intermediate_Dir "WinDebug"
OUTDIR=..\WinDebug
INTDIR=.\WinDebug

ALL : $(OUTDIR)/gizmobar.dll $(OUTDIR)/gizmobar.bsc

$(OUTDIR) : 
    if not exist $(OUTDIR)/nul mkdir $(OUTDIR)

$(INTDIR) : 
    if not exist $(INTDIR)/nul mkdir $(INTDIR)

MTL=MkTypLib.exe
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
CPP=cl.exe
# ADD BASE CPP /nologo /MD /W3 /GX /Zi /YX /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /c
# ADD CPP /nologo /MT /W3 /GX /Zi /YX /Od /I "..\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /FR /c
CPP_PROJ=/nologo /MT /W3 /GX /Zi /YX /Od /I "..\include" /D "WIN32" /D "_DEBUG"\
 /D "_WINDOWS" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"gizmobar.pch"\
 /Fo$(INTDIR)/ /Fd$(OUTDIR)/"gizmobar.pdb" /c 
CPP_OBJS=.\WinDebug/

.c{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

RSC=rc.exe
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /i "..\include" /d "_DEBUG"
RSC_PROJ=/l 0x409 /fo$(INTDIR)/"gizmobar.res" /i "..\include" /d "_DEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# SUBTRACT BASE BSC32 /Iu
# ADD BSC32 /nologo
# SUBTRACT BSC32 /Iu
BSC32_FLAGS=/nologo /o$(OUTDIR)/"gizmobar.bsc" 
BSC32_SBRS= \
	.\WinDebug\api.sbr \
	.\WinDebug\paint.sbr \
	.\WinDebug\gizmo.sbr \
	.\WinDebug\init.sbr \
	.\WinDebug\gizmobar.sbr

$(OUTDIR)/gizmobar.bsc : $(OUTDIR)  $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 /NOLOGO /SUBSYSTEM:windows /DLL /DEBUG /MACHINE:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib ..\bttncur\bttncur.lib /NOLOGO /ENTRY:"LibMain@12" /SUBSYSTEM:windows /DLL /DEBUG /MACHINE:I386 /IMPLIB:"GIZMOBAR.lib"
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib ..\bttncur\bttncur.lib /NOLOGO /ENTRY:"LibMain@12"\
 /SUBSYSTEM:windows /DLL /INCREMENTAL:yes /PDB:$(OUTDIR)/"gizmobar.pdb" /DEBUG\
 /MACHINE:I386 /DEF:".\gizmobar.def" /OUT:$(OUTDIR)/"gizmobar.dll"\
 /IMPLIB:"GIZMOBAR.lib" 
DEF_FILE=.\gizmobar.def
LINK32_OBJS= \
	.\WinDebug\api.obj \
	.\WinDebug\paint.obj \
	.\WinDebug\gizmobar.res \
	.\WinDebug\gizmo.obj \
	.\WinDebug\init.obj \
	.\WinDebug\gizmobar.obj

$(OUTDIR)/gizmobar.dll : $(OUTDIR)  $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "WinRel"
# PROP BASE Intermediate_Dir "WinRel"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\WinRel"
# PROP Intermediate_Dir "WinRel"
OUTDIR=..\WinRel
INTDIR=.\WinRel

ALL : $(OUTDIR)/gizmobar.dll $(OUTDIR)/gizmobar.bsc

$(OUTDIR) : 
    if not exist $(OUTDIR)/nul mkdir $(OUTDIR)

$(INTDIR) : 
    if not exist $(INTDIR)/nul mkdir $(INTDIR)

MTL=MkTypLib.exe
# ADD BASE MTL /nologo /D "NDEBUG" /mips
# ADD MTL /nologo /D "NDEBUG" /mips
MTL_PROJ=/nologo /D "NDEBUG" /mips 
CPP=cl.exe
# ADD BASE CPP /nologo /MD /Gt0 /QMOb2000 /W3 /GX /YX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /c
# ADD CPP /nologo /MT /Gt0 /QMOb2000 /W3 /GX /YX /O2 /I "..\include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /FR /c
CPP_PROJ=/nologo /MT /Gt0 /QMOb2000 /W3 /GX /YX /O2 /I "..\include" /D "WIN32"\
 /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"gizmobar.pch"\
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
# ADD RSC /l 0x409 /i "..\include" /d "NDEBUG"
RSC_PROJ=/l 0x409 /fo$(INTDIR)/"gizmobar.res" /i "..\include" /d "NDEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# SUBTRACT BASE BSC32 /Iu
# ADD BSC32 /nologo
# SUBTRACT BSC32 /Iu
BSC32_FLAGS=/nologo /o$(OUTDIR)/"gizmobar.bsc" 
BSC32_SBRS= \
	.\WinRel\api.sbr \
	.\WinRel\paint.sbr \
	.\WinRel\gizmo.sbr \
	.\WinRel\init.sbr \
	.\WinRel\gizmobar.sbr

$(OUTDIR)/gizmobar.bsc : $(OUTDIR)  $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 /NOLOGO /SUBSYSTEM:windows /DLL /MACHINE:MIPS
# SUBTRACT BASE LINK32 /PDB:none
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib ..\bttncur\bttncur.lib /NOLOGO /ENTRY:"LibMain" /SUBSYSTEM:windows /DLL /MACHINE:MIPS /IMPLIB:"GIZMOBAR.lib"
# SUBTRACT LINK32 /PDB:none
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib ..\bttncur\bttncur.lib\
 /NOLOGO /ENTRY:"LibMain" /SUBSYSTEM:windows /DLL /PDB:$(OUTDIR)/"gizmobar.pdb"\
 /MACHINE:MIPS /DEF:".\gizmobar.def" /OUT:$(OUTDIR)/"gizmobar.dll"\
 /IMPLIB:"GIZMOBAR.lib" 
DEF_FILE=.\gizmobar.def
LINK32_OBJS= \
	.\WinRel\api.obj \
	.\WinRel\paint.obj \
	.\WinRel\gizmobar.res \
	.\WinRel\gizmo.obj \
	.\WinRel\init.obj \
	.\WinRel\gizmobar.obj

$(OUTDIR)/gizmobar.dll : $(OUTDIR)  $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "WinDebug"
# PROP BASE Intermediate_Dir "WinDebug"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\WinDebug"
# PROP Intermediate_Dir "WinDebug"
OUTDIR=..\WinDebug
INTDIR=.\WinDebug

ALL : $(OUTDIR)/gizmobar.dll $(OUTDIR)/gizmobar.bsc

$(OUTDIR) : 
    if not exist $(OUTDIR)/nul mkdir $(OUTDIR)

$(INTDIR) : 
    if not exist $(INTDIR)/nul mkdir $(INTDIR)

MTL=MkTypLib.exe
# ADD BASE MTL /nologo /D "_DEBUG" /mips
# ADD MTL /nologo /D "_DEBUG" /mips
MTL_PROJ=/nologo /D "_DEBUG" /mips 
CPP=cl.exe
# ADD BASE CPP /nologo /MD /Gt0 /QMOb2000 /W3 /GX /Zi /YX /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /c
# ADD CPP /nologo /MT /Gt0 /QMOb2000 /W3 /GX /Zi /YX /Od /I "..\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /FR /c
CPP_PROJ=/nologo /MT /Gt0 /QMOb2000 /W3 /GX /Zi /YX /Od /I "..\include" /D\
 "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /FR$(INTDIR)/\
 /Fp$(OUTDIR)/"gizmobar.pch" /Fo$(INTDIR)/ /Fd$(OUTDIR)/"gizmobar.pdb" /c 
CPP_OBJS=.\WinDebug/

.c{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

RSC=rc.exe
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /i "..\include" /d "_DEBUG"
RSC_PROJ=/l 0x409 /fo$(INTDIR)/"gizmobar.res" /i "..\include" /d "_DEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# SUBTRACT BASE BSC32 /Iu
# ADD BSC32 /nologo
# SUBTRACT BSC32 /Iu
BSC32_FLAGS=/nologo /o$(OUTDIR)/"gizmobar.bsc" 
BSC32_SBRS= \
	.\WinDebug\api.sbr \
	.\WinDebug\paint.sbr \
	.\WinDebug\gizmo.sbr \
	.\WinDebug\init.sbr \
	.\WinDebug\gizmobar.sbr

$(OUTDIR)/gizmobar.bsc : $(OUTDIR)  $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 /NOLOGO /SUBSYSTEM:windows /DLL /DEBUG /MACHINE:MIPS
# SUBTRACT BASE LINK32 /PDB:none
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib ..\bttncur\bttncur.lib /NOLOGO /ENTRY:"LibMain" /SUBSYSTEM:windows /DLL /DEBUG /MACHINE:MIPS /IMPLIB:"GIZMOBAR.lib"
# SUBTRACT LINK32 /PDB:none
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib ..\bttncur\bttncur.lib\
 /NOLOGO /ENTRY:"LibMain" /SUBSYSTEM:windows /DLL /PDB:$(OUTDIR)/"gizmobar.pdb"\
 /DEBUG /MACHINE:MIPS /DEF:".\gizmobar.def" /OUT:$(OUTDIR)/"gizmobar.dll"\
 /IMPLIB:"GIZMOBAR.lib" 
DEF_FILE=.\gizmobar.def
LINK32_OBJS= \
	.\WinDebug\api.obj \
	.\WinDebug\paint.obj \
	.\WinDebug\gizmobar.res \
	.\WinDebug\gizmo.obj \
	.\WinDebug\init.obj \
	.\WinDebug\gizmobar.obj

$(OUTDIR)/gizmobar.dll : $(OUTDIR)  $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 

################################################################################
# Begin Group "Source Files"

################################################################################
# Begin Source File

SOURCE=.\api.c
DEP_API_C=\
	.\gizmoint.h\
	..\include\bttncur.h\
	.\win1632.h\
	.\gizmo.h\
	..\include\gizmobar.h

!IF  "$(CFG)" == "Win32 (80x86) Release"

.\WinRel\api.obj :  $(SOURCE)  $(DEP_API_C) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

.\WinDebug\api.obj :  $(SOURCE)  $(DEP_API_C) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

.\WinRel\api.obj :  $(SOURCE)  $(DEP_API_C) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

.\WinDebug\api.obj :  $(SOURCE)  $(DEP_API_C) $(INTDIR)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\paint.c
DEP_PAINT=\
	.\gizmoint.h\
	..\include\bttncur.h\
	.\win1632.h\
	.\gizmo.h\
	..\include\gizmobar.h

!IF  "$(CFG)" == "Win32 (80x86) Release"

.\WinRel\paint.obj :  $(SOURCE)  $(DEP_PAINT) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

.\WinDebug\paint.obj :  $(SOURCE)  $(DEP_PAINT) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

.\WinRel\paint.obj :  $(SOURCE)  $(DEP_PAINT) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

.\WinDebug\paint.obj :  $(SOURCE)  $(DEP_PAINT) $(INTDIR)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\gizmobar.rc

!IF  "$(CFG)" == "Win32 (80x86) Release"

.\WinRel\gizmobar.res :  $(SOURCE)  $(INTDIR)
   $(RSC) $(RSC_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

.\WinDebug\gizmobar.res :  $(SOURCE)  $(INTDIR)
   $(RSC) $(RSC_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

.\WinRel\gizmobar.res :  $(SOURCE)  $(INTDIR)
   $(RSC) $(RSC_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

.\WinDebug\gizmobar.res :  $(SOURCE)  $(INTDIR)
   $(RSC) $(RSC_PROJ)  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\gizmo.c
DEP_GIZMO=\
	.\gizmoint.h\
	..\include\bttncur.h\
	.\win1632.h\
	.\gizmo.h\
	..\include\gizmobar.h

!IF  "$(CFG)" == "Win32 (80x86) Release"

.\WinRel\gizmo.obj :  $(SOURCE)  $(DEP_GIZMO) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

.\WinDebug\gizmo.obj :  $(SOURCE)  $(DEP_GIZMO) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

.\WinRel\gizmo.obj :  $(SOURCE)  $(DEP_GIZMO) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

.\WinDebug\gizmo.obj :  $(SOURCE)  $(DEP_GIZMO) $(INTDIR)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\init.c
DEP_INIT_=\
	.\gizmoint.h\
	..\include\bttncur.h\
	.\win1632.h\
	.\gizmo.h\
	..\include\gizmobar.h

!IF  "$(CFG)" == "Win32 (80x86) Release"

.\WinRel\init.obj :  $(SOURCE)  $(DEP_INIT_) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

.\WinDebug\init.obj :  $(SOURCE)  $(DEP_INIT_) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

.\WinRel\init.obj :  $(SOURCE)  $(DEP_INIT_) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

.\WinDebug\init.obj :  $(SOURCE)  $(DEP_INIT_) $(INTDIR)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\gizmobar.def
# End Source File
################################################################################
# Begin Source File

SOURCE=.\gizmobar.c
DEP_GIZMOB=\
	.\gizmoint.h\
	..\include\bttncur.h\
	.\win1632.h\
	.\gizmo.h\
	..\include\gizmobar.h

!IF  "$(CFG)" == "Win32 (80x86) Release"

.\WinRel\gizmobar.obj :  $(SOURCE)  $(DEP_GIZMOB) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

.\WinDebug\gizmobar.obj :  $(SOURCE)  $(DEP_GIZMOB) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

.\WinRel\gizmobar.obj :  $(SOURCE)  $(DEP_GIZMOB) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

.\WinDebug\gizmobar.obj :  $(SOURCE)  $(DEP_GIZMOB) $(INTDIR)

!ENDIF 

# End Source File
# End Group
# End Project
################################################################################
