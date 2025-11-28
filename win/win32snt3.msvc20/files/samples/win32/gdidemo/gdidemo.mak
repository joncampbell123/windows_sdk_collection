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
!MESSAGE NMAKE /f "gdidemo.mak" CFG="Win32 (80x86) Debug"
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

ALL : $(OUTDIR)/gdidemo.exe $(OUTDIR)/gdidemo.bsc

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
 "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"gdidemo.pch" /Fo$(INTDIR)/ /c 
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
RSC_PROJ=/l 0x409 /fo$(INTDIR)/"gdidemo.res" /d "NDEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# SUBTRACT BASE BSC32 /Iu
# ADD BSC32 /nologo
# SUBTRACT BSC32 /Iu
BSC32_FLAGS=/nologo /o$(OUTDIR)/"gdidemo.bsc" 
BSC32_SBRS= \
	$(INTDIR)/xform.sbr \
	$(INTDIR)/gdidemo.sbr \
	$(INTDIR)/wininfo.sbr \
	$(INTDIR)/dialog.sbr \
	$(INTDIR)/poly.sbr \
	$(INTDIR)/bounce.sbr \
	$(INTDIR)/init.sbr \
	$(INTDIR)/maze.sbr \
	$(INTDIR)/draw.sbr

$(OUTDIR)/gdidemo.bsc : $(OUTDIR)  $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib mfc30.lib mfco30.lib mfcd30.lib /NOLOGO /SUBSYSTEM:windows /MACHINE:I386
# ADD LINK32 user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib /NOLOGO /SUBSYSTEM:windows /MACHINE:I386
LINK32_FLAGS=user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib\
 shell32.lib /NOLOGO /SUBSYSTEM:windows /INCREMENTAL:no\
 /PDB:$(OUTDIR)/"gdidemo.pdb" /MACHINE:I386 /OUT:$(OUTDIR)/"gdidemo.exe" 
DEF_FILE=
LINK32_OBJS= \
	$(INTDIR)/gdidemo.res \
	$(INTDIR)/xform.obj \
	$(INTDIR)/gdidemo.obj \
	$(INTDIR)/wininfo.obj \
	$(INTDIR)/dialog.obj \
	$(INTDIR)/poly.obj \
	$(INTDIR)/bounce.obj \
	$(INTDIR)/init.obj \
	$(INTDIR)/maze.obj \
	$(INTDIR)/draw.obj

$(OUTDIR)/gdidemo.exe : $(OUTDIR)  $(DEF_FILE) $(LINK32_OBJS)
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

ALL : $(OUTDIR)/gdidemo.exe $(OUTDIR)/gdidemo.bsc

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
 "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"gdidemo.pch" /Fo$(INTDIR)/\
 /Fd$(OUTDIR)/"gdidemo.pdb" /c 
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
RSC_PROJ=/l 0x409 /fo$(INTDIR)/"gdidemo.res" /d "_DEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# SUBTRACT BASE BSC32 /Iu
# ADD BSC32 /nologo
# SUBTRACT BSC32 /Iu
BSC32_FLAGS=/nologo /o$(OUTDIR)/"gdidemo.bsc" 
BSC32_SBRS= \
	$(INTDIR)/xform.sbr \
	$(INTDIR)/gdidemo.sbr \
	$(INTDIR)/wininfo.sbr \
	$(INTDIR)/dialog.sbr \
	$(INTDIR)/poly.sbr \
	$(INTDIR)/bounce.sbr \
	$(INTDIR)/init.sbr \
	$(INTDIR)/maze.sbr \
	$(INTDIR)/draw.sbr

$(OUTDIR)/gdidemo.bsc : $(OUTDIR)  $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib mfc30d.lib mfco30d.lib mfcd30d.lib /NOLOGO /SUBSYSTEM:windows /DEBUG /MACHINE:I386
# ADD LINK32 user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib /NOLOGO /SUBSYSTEM:windows /DEBUG /MACHINE:I386
LINK32_FLAGS=user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib\
 shell32.lib /NOLOGO /SUBSYSTEM:windows /INCREMENTAL:yes\
 /PDB:$(OUTDIR)/"gdidemo.pdb" /DEBUG /MACHINE:I386 /OUT:$(OUTDIR)/"gdidemo.exe" 
DEF_FILE=
LINK32_OBJS= \
	$(INTDIR)/gdidemo.res \
	$(INTDIR)/xform.obj \
	$(INTDIR)/gdidemo.obj \
	$(INTDIR)/wininfo.obj \
	$(INTDIR)/dialog.obj \
	$(INTDIR)/poly.obj \
	$(INTDIR)/bounce.obj \
	$(INTDIR)/init.obj \
	$(INTDIR)/maze.obj \
	$(INTDIR)/draw.obj

$(OUTDIR)/gdidemo.exe : $(OUTDIR)  $(DEF_FILE) $(LINK32_OBJS)
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

ALL : $(OUTDIR)/gdidemo.exe $(OUTDIR)/gdidemo.bsc

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
 /Fp$(OUTDIR)/"gdidemo.pch" /Fo$(INTDIR)/ /Fd$(OUTDIR)/"gdidemo.pdb" /c 
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
RSC_PROJ=/l 0x409 /fo$(INTDIR)/"gdidemo.res" /d "_DEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# SUBTRACT BASE BSC32 /Iu
# ADD BSC32 /nologo
# SUBTRACT BSC32 /Iu
BSC32_FLAGS=/nologo /o$(OUTDIR)/"gdidemo.bsc" 
BSC32_SBRS= \
	$(INTDIR)/xform.sbr \
	$(INTDIR)/gdidemo.sbr \
	$(INTDIR)/wininfo.sbr \
	$(INTDIR)/dialog.sbr \
	$(INTDIR)/poly.sbr \
	$(INTDIR)/bounce.sbr \
	$(INTDIR)/init.sbr \
	$(INTDIR)/maze.sbr \
	$(INTDIR)/draw.sbr

$(OUTDIR)/gdidemo.bsc : $(OUTDIR)  $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib odbc32.lib mfc30d.lib mfco30d.lib mfcd30d.lib oleaut32.lib uuid.lib /NOLOGO /SUBSYSTEM:windows /DEBUG /MACHINE:MIPS
# SUBTRACT BASE LINK32 /PDB:none
# ADD LINK32 user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib /NOLOGO /SUBSYSTEM:windows /DEBUG /MACHINE:MIPS
# SUBTRACT LINK32 /PDB:none
LINK32_FLAGS=user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib\
 shell32.lib /NOLOGO /SUBSYSTEM:windows /PDB:$(OUTDIR)/"gdidemo.pdb" /DEBUG\
 /MACHINE:MIPS /OUT:$(OUTDIR)/"gdidemo.exe" 
DEF_FILE=
LINK32_OBJS= \
	$(INTDIR)/gdidemo.res \
	$(INTDIR)/xform.obj \
	$(INTDIR)/gdidemo.obj \
	$(INTDIR)/wininfo.obj \
	$(INTDIR)/dialog.obj \
	$(INTDIR)/poly.obj \
	$(INTDIR)/bounce.obj \
	$(INTDIR)/init.obj \
	$(INTDIR)/maze.obj \
	$(INTDIR)/draw.obj

$(OUTDIR)/gdidemo.exe : $(OUTDIR)  $(DEF_FILE) $(LINK32_OBJS)
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

ALL : $(OUTDIR)/gdidemo.exe $(OUTDIR)/gdidemo.bsc

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
 "_WINDOWS" /D "_MBCS" /D "WIN32" /FR$(INTDIR)/ /Fp$(OUTDIR)/"gdidemo.pch"\
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
RSC_PROJ=/l 0x409 /fo$(INTDIR)/"gdidemo.res" /d "NDEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# SUBTRACT BASE BSC32 /Iu
# ADD BSC32 /nologo
# SUBTRACT BSC32 /Iu
BSC32_FLAGS=/nologo /o$(OUTDIR)/"gdidemo.bsc" 
BSC32_SBRS= \
	$(INTDIR)/xform.sbr \
	$(INTDIR)/gdidemo.sbr \
	$(INTDIR)/wininfo.sbr \
	$(INTDIR)/dialog.sbr \
	$(INTDIR)/poly.sbr \
	$(INTDIR)/bounce.sbr \
	$(INTDIR)/init.sbr \
	$(INTDIR)/maze.sbr \
	$(INTDIR)/draw.sbr

$(OUTDIR)/gdidemo.bsc : $(OUTDIR)  $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib odbc32.lib mfc30.lib mfco30.lib mfcd30.lib oleaut32.lib uuid.lib /NOLOGO /SUBSYSTEM:windows /MACHINE:MIPS
# SUBTRACT BASE LINK32 /PDB:none
# ADD LINK32 user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib /NOLOGO /SUBSYSTEM:windows /MACHINE:MIPS
# SUBTRACT LINK32 /PDB:none
LINK32_FLAGS=user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib\
 shell32.lib /NOLOGO /SUBSYSTEM:windows /PDB:$(OUTDIR)/"gdidemo.pdb"\
 /MACHINE:MIPS /OUT:$(OUTDIR)/"gdidemo.exe" 
DEF_FILE=
LINK32_OBJS= \
	$(INTDIR)/gdidemo.res \
	$(INTDIR)/xform.obj \
	$(INTDIR)/gdidemo.obj \
	$(INTDIR)/wininfo.obj \
	$(INTDIR)/dialog.obj \
	$(INTDIR)/poly.obj \
	$(INTDIR)/bounce.obj \
	$(INTDIR)/init.obj \
	$(INTDIR)/maze.obj \
	$(INTDIR)/draw.obj

$(OUTDIR)/gdidemo.exe : $(OUTDIR)  $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 

################################################################################
# Begin Group "Source Files"

################################################################################
# Begin Source File

SOURCE=.\gdidemo.rc
DEP_GDIDE=\
	.\gdidemo.ico\
	.\gdidemo.h

!IF  "$(CFG)" == "Win32 (80x86) Release"

$(INTDIR)/gdidemo.res :  $(SOURCE)  $(DEP_GDIDE) $(INTDIR)
   $(RSC) $(RSC_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

$(INTDIR)/gdidemo.res :  $(SOURCE)  $(DEP_GDIDE) $(INTDIR)
   $(RSC) $(RSC_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

$(INTDIR)/gdidemo.res :  $(SOURCE)  $(DEP_GDIDE) $(INTDIR)
   $(RSC) $(RSC_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

$(INTDIR)/gdidemo.res :  $(SOURCE)  $(DEP_GDIDE) $(INTDIR)
   $(RSC) $(RSC_PROJ)  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\xform.c
DEP_XFORM=\
	.\gdidemo.h\
	.\xform.h

!IF  "$(CFG)" == "Win32 (80x86) Release"

$(INTDIR)/xform.obj :  $(SOURCE)  $(DEP_XFORM) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

$(INTDIR)/xform.obj :  $(SOURCE)  $(DEP_XFORM) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

$(INTDIR)/xform.obj :  $(SOURCE)  $(DEP_XFORM) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

$(INTDIR)/xform.obj :  $(SOURCE)  $(DEP_XFORM) $(INTDIR)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\gdidemo.c
DEP_GDIDEM=\
	.\gdidemo.h\
	.\poly.h\
	.\xform.h\
	.\maze.h\
	.\draw.h\
	.\bounce.h

!IF  "$(CFG)" == "Win32 (80x86) Release"

$(INTDIR)/gdidemo.obj :  $(SOURCE)  $(DEP_GDIDEM) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

$(INTDIR)/gdidemo.obj :  $(SOURCE)  $(DEP_GDIDEM) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

$(INTDIR)/gdidemo.obj :  $(SOURCE)  $(DEP_GDIDEM) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

$(INTDIR)/gdidemo.obj :  $(SOURCE)  $(DEP_GDIDEM) $(INTDIR)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\wininfo.c
DEP_WININ=\
	.\gdidemo.h

!IF  "$(CFG)" == "Win32 (80x86) Release"

$(INTDIR)/wininfo.obj :  $(SOURCE)  $(DEP_WININ) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

$(INTDIR)/wininfo.obj :  $(SOURCE)  $(DEP_WININ) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

$(INTDIR)/wininfo.obj :  $(SOURCE)  $(DEP_WININ) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

$(INTDIR)/wininfo.obj :  $(SOURCE)  $(DEP_WININ) $(INTDIR)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\dialog.c
DEP_DIALO=\
	.\gdidemo.h

!IF  "$(CFG)" == "Win32 (80x86) Release"

$(INTDIR)/dialog.obj :  $(SOURCE)  $(DEP_DIALO) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

$(INTDIR)/dialog.obj :  $(SOURCE)  $(DEP_DIALO) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

$(INTDIR)/dialog.obj :  $(SOURCE)  $(DEP_DIALO) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

$(INTDIR)/dialog.obj :  $(SOURCE)  $(DEP_DIALO) $(INTDIR)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\poly.c
DEP_POLY_=\
	.\gdidemo.h\
	.\poly.h

!IF  "$(CFG)" == "Win32 (80x86) Release"

$(INTDIR)/poly.obj :  $(SOURCE)  $(DEP_POLY_) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

$(INTDIR)/poly.obj :  $(SOURCE)  $(DEP_POLY_) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

$(INTDIR)/poly.obj :  $(SOURCE)  $(DEP_POLY_) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

$(INTDIR)/poly.obj :  $(SOURCE)  $(DEP_POLY_) $(INTDIR)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\bounce.c
DEP_BOUNC=\
	.\gdidemo.h\
	.\bounce.h

!IF  "$(CFG)" == "Win32 (80x86) Release"

$(INTDIR)/bounce.obj :  $(SOURCE)  $(DEP_BOUNC) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

$(INTDIR)/bounce.obj :  $(SOURCE)  $(DEP_BOUNC) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

$(INTDIR)/bounce.obj :  $(SOURCE)  $(DEP_BOUNC) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

$(INTDIR)/bounce.obj :  $(SOURCE)  $(DEP_BOUNC) $(INTDIR)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\init.c
DEP_INIT_=\
	.\gdidemo.h\
	.\poly.h\
	.\xform.h\
	.\maze.h\
	.\draw.h\
	.\bounce.h

!IF  "$(CFG)" == "Win32 (80x86) Release"

$(INTDIR)/init.obj :  $(SOURCE)  $(DEP_INIT_) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

$(INTDIR)/init.obj :  $(SOURCE)  $(DEP_INIT_) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

$(INTDIR)/init.obj :  $(SOURCE)  $(DEP_INIT_) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

$(INTDIR)/init.obj :  $(SOURCE)  $(DEP_INIT_) $(INTDIR)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\maze.c
DEP_MAZE_=\
	.\gdidemo.h\
	.\maze.h

!IF  "$(CFG)" == "Win32 (80x86) Release"

$(INTDIR)/maze.obj :  $(SOURCE)  $(DEP_MAZE_) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

$(INTDIR)/maze.obj :  $(SOURCE)  $(DEP_MAZE_) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

$(INTDIR)/maze.obj :  $(SOURCE)  $(DEP_MAZE_) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

$(INTDIR)/maze.obj :  $(SOURCE)  $(DEP_MAZE_) $(INTDIR)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\draw.c
DEP_DRAW_=\
	.\gdidemo.h\
	.\draw.h

!IF  "$(CFG)" == "Win32 (80x86) Release"

$(INTDIR)/draw.obj :  $(SOURCE)  $(DEP_DRAW_) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

$(INTDIR)/draw.obj :  $(SOURCE)  $(DEP_DRAW_) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

$(INTDIR)/draw.obj :  $(SOURCE)  $(DEP_DRAW_) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

$(INTDIR)/draw.obj :  $(SOURCE)  $(DEP_DRAW_) $(INTDIR)

!ENDIF 

# End Source File
# End Group
# End Project
################################################################################
