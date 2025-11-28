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
!MESSAGE NMAKE /f "wincap32.mak" CFG="Win32 (80x86) Debug"
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

ALL : $(OUTDIR)/wincap32.exe $(OUTDIR)/wincap32.bsc

$(OUTDIR) : 
    if not exist $(OUTDIR)/nul mkdir $(OUTDIR)

MTL=MkTypLib.exe
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
CPP=cl.exe
# ADD BASE CPP /nologo /MD /W3 /GX /YX /O2 /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /c
# ADD CPP /nologo /W3 /GX /YX /O2 /D "NDEBUG" /D "WIN32" /FR /c
CPP_PROJ=/nologo /W3 /GX /YX /O2 /D "NDEBUG" /D "WIN32" /FR$(INTDIR)/\
 /Fp$(OUTDIR)/"wincap32.pch" /Fo$(INTDIR)/ /c 
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
RSC_PROJ=/l 0x409 /fo$(INTDIR)/"wincap.res" /d "NDEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# SUBTRACT BASE BSC32 /Iu
# ADD BSC32 /nologo
# SUBTRACT BSC32 /Iu
BSC32_FLAGS=/nologo /o$(OUTDIR)/"wincap32.bsc" 
BSC32_SBRS= \
	$(INTDIR)/wincap.sbr \
	$(INTDIR)/dialogs.sbr \
	$(INTDIR)/dlgopen.sbr

$(OUTDIR)/wincap32.bsc : $(OUTDIR)  $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib mfc30.lib mfco30.lib mfcd30.lib mfcuia32.lib mfcans32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib /NOLOGO /SUBSYSTEM:windows /MACHINE:I386
# SUBTRACT BASE LINK32 /PDB:none
# ADD LINK32 user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib /NOLOGO /SUBSYSTEM:windows /MACHINE:I386
# SUBTRACT LINK32 /PDB:none
LINK32_FLAGS=user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib\
 /NOLOGO /SUBSYSTEM:windows /INCREMENTAL:no /PDB:$(OUTDIR)/"wincap32.pdb"\
 /MACHINE:I386 /OUT:$(OUTDIR)/"wincap32.exe" 
DEF_FILE=
LINK32_OBJS= \
	.\dibapi32.lib \
	$(INTDIR)/wincap.obj \
	$(INTDIR)/wincap.res \
	$(INTDIR)/dialogs.obj \
	$(INTDIR)/dlgopen.obj

$(OUTDIR)/wincap32.exe : $(OUTDIR)  $(DEF_FILE) $(LINK32_OBJS)
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

ALL : $(OUTDIR)/wincap32.exe $(OUTDIR)/wincap32.bsc

$(OUTDIR) : 
    if not exist $(OUTDIR)/nul mkdir $(OUTDIR)

MTL=MkTypLib.exe
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
CPP=cl.exe
# ADD BASE CPP /nologo /MD /W3 /GX /Zi /YX /Od /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /c
# ADD CPP /nologo /W3 /GX /Zi /YX /Od /D "_DEBUG" /D "WIN32" /FR /c
CPP_PROJ=/nologo /W3 /GX /Zi /YX /Od /D "_DEBUG" /D "WIN32" /FR$(INTDIR)/\
 /Fp$(OUTDIR)/"wincap32.pch" /Fo$(INTDIR)/ /Fd$(OUTDIR)/"wincap32.pdb" /c 
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
RSC_PROJ=/l 0x409 /fo$(INTDIR)/"wincap.res" /d "_DEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# SUBTRACT BASE BSC32 /Iu
# ADD BSC32 /nologo
# SUBTRACT BSC32 /Iu
BSC32_FLAGS=/nologo /o$(OUTDIR)/"wincap32.bsc" 
BSC32_SBRS= \
	$(INTDIR)/wincap.sbr \
	$(INTDIR)/dialogs.sbr \
	$(INTDIR)/dlgopen.sbr

$(OUTDIR)/wincap32.bsc : $(OUTDIR)  $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib mfc30d.lib mfco30d.lib mfcd30d.lib mfcuia32.lib mfcans32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib /NOLOGO /SUBSYSTEM:windows /DEBUG /MACHINE:I386
# SUBTRACT BASE LINK32 /PDB:none
# ADD LINK32 user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib /NOLOGO /SUBSYSTEM:windows /DEBUG /MACHINE:I386
# SUBTRACT LINK32 /PDB:none
LINK32_FLAGS=user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib\
 /NOLOGO /SUBSYSTEM:windows /INCREMENTAL:yes /PDB:$(OUTDIR)/"wincap32.pdb"\
 /DEBUG /MACHINE:I386 /OUT:$(OUTDIR)/"wincap32.exe" 
DEF_FILE=
LINK32_OBJS= \
	.\dibapi32.lib \
	$(INTDIR)/wincap.obj \
	$(INTDIR)/wincap.res \
	$(INTDIR)/dialogs.obj \
	$(INTDIR)/dlgopen.obj

$(OUTDIR)/wincap32.exe : $(OUTDIR)  $(DEF_FILE) $(LINK32_OBJS)
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

ALL : $(OUTDIR)/wincap32.exe $(OUTDIR)/wincap32.bsc

$(OUTDIR) : 
    if not exist $(OUTDIR)/nul mkdir $(OUTDIR)

MTL=MkTypLib.exe
# ADD BASE MTL /nologo /D "NDEBUG" /mips
# ADD MTL /nologo /D "NDEBUG" /mips
MTL_PROJ=/nologo /D "NDEBUG" /mips 
CPP=cl.exe
# ADD BASE CPP /nologo /MD /Gt0 /QMOb2000 /W3 /GX /YX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /c
# ADD CPP /nologo /Gt0 /QMOb2000 /W3 /GX /YX /O2 /D "WIN32" /D "NDEBUG" /FR /c
CPP_PROJ=/nologo /Gt0 /QMOb2000 /W3 /GX /YX /O2 /D "WIN32" /D "NDEBUG"\
 /FR$(INTDIR)/ /Fp$(OUTDIR)/"wincap32.pch" /Fo$(INTDIR)/ /c 
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
RSC_PROJ=/l 0x409 /fo$(INTDIR)/"wincap.res" /d "NDEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o$(OUTDIR)/"wincap32.bsc" 
BSC32_SBRS= \
	$(INTDIR)/wincap.sbr \
	$(INTDIR)/dialogs.sbr \
	$(INTDIR)/dlgopen.sbr

$(OUTDIR)/wincap32.bsc : $(OUTDIR)  $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 /NOLOGO /SUBSYSTEM:windows /MACHINE:MIPS
# SUBTRACT BASE LINK32 /PDB:none
# ADD LINK32 user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib /NOLOGO /SUBSYSTEM:windows /MACHINE:MIPS
# SUBTRACT LINK32 /PDB:none
LINK32_FLAGS=user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib\
 /NOLOGO /SUBSYSTEM:windows /PDB:$(OUTDIR)/"wincap32.pdb" /MACHINE:MIPS\
 /OUT:$(OUTDIR)/"wincap32.exe" 
DEF_FILE=
LINK32_OBJS= \
	.\dibapi32.lib \
	$(INTDIR)/wincap.obj \
	$(INTDIR)/wincap.res \
	$(INTDIR)/dialogs.obj \
	$(INTDIR)/dlgopen.obj

$(OUTDIR)/wincap32.exe : $(OUTDIR)  $(DEF_FILE) $(LINK32_OBJS)
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

ALL : $(OUTDIR)/wincap32.exe $(OUTDIR)/wincap32.bsc

$(OUTDIR) : 
    if not exist $(OUTDIR)/nul mkdir $(OUTDIR)

MTL=MkTypLib.exe
# ADD BASE MTL /nologo /D "_DEBUG" /mips
# ADD MTL /nologo /D "_DEBUG" /mips
MTL_PROJ=/nologo /D "_DEBUG" /mips 
CPP=cl.exe
# ADD BASE CPP /nologo /MD /Gt0 /QMOb2000 /W3 /GX /Zi /YX /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /c
# ADD CPP /nologo /Gt0 /QMOb2000 /W3 /GX /Zi /YX /Od /D "WIN32" /D "_DEBUG" /FR /c
CPP_PROJ=/nologo /Gt0 /QMOb2000 /W3 /GX /Zi /YX /Od /D "WIN32" /D "_DEBUG"\
 /FR$(INTDIR)/ /Fp$(OUTDIR)/"wincap32.pch" /Fo$(INTDIR)/\
 /Fd$(OUTDIR)/"wincap32.pdb" /c 
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
RSC_PROJ=/l 0x409 /fo$(INTDIR)/"wincap.res" /d "_DEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o$(OUTDIR)/"wincap32.bsc" 
BSC32_SBRS= \
	$(INTDIR)/wincap.sbr \
	$(INTDIR)/dialogs.sbr \
	$(INTDIR)/dlgopen.sbr

$(OUTDIR)/wincap32.bsc : $(OUTDIR)  $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 /NOLOGO /SUBSYSTEM:windows /DEBUG /MACHINE:MIPS
# SUBTRACT BASE LINK32 /PDB:none
# ADD LINK32 user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib /NOLOGO /SUBSYSTEM:windows /DEBUG /MACHINE:MIPS
# SUBTRACT LINK32 /PDB:none
LINK32_FLAGS=user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib\
 /NOLOGO /SUBSYSTEM:windows /PDB:$(OUTDIR)/"wincap32.pdb" /DEBUG /MACHINE:MIPS\
 /OUT:$(OUTDIR)/"wincap32.exe" 
DEF_FILE=
LINK32_OBJS= \
	.\dibapi32.lib \
	$(INTDIR)/wincap.obj \
	$(INTDIR)/wincap.res \
	$(INTDIR)/dialogs.obj \
	$(INTDIR)/dlgopen.obj

$(OUTDIR)/wincap32.exe : $(OUTDIR)  $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 

################################################################################
# Begin Group "Source Files"

################################################################################
# Begin Source File

SOURCE=.\dibapi32.lib
# End Source File
################################################################################
# Begin Source File

SOURCE=.\wincap.c
DEP_WINCA=\
	.\wincap.h\
	.\dialogs.h\
	.\dibapi.h

!IF  "$(CFG)" == "Win32 (80x86) Release"

$(INTDIR)/wincap.obj :  $(SOURCE)  $(DEP_WINCA) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

$(INTDIR)/wincap.obj :  $(SOURCE)  $(DEP_WINCA) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

$(INTDIR)/wincap.obj :  $(SOURCE)  $(DEP_WINCA) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

$(INTDIR)/wincap.obj :  $(SOURCE)  $(DEP_WINCA) $(INTDIR)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\wincap.rc

!IF  "$(CFG)" == "Win32 (80x86) Release"

$(INTDIR)/wincap.res :  $(SOURCE)  $(INTDIR)
   $(RSC) $(RSC_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

$(INTDIR)/wincap.res :  $(SOURCE)  $(INTDIR)
   $(RSC) $(RSC_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

$(INTDIR)/wincap.res :  $(SOURCE)  $(INTDIR)
   $(RSC) $(RSC_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

$(INTDIR)/wincap.res :  $(SOURCE)  $(INTDIR)
   $(RSC) $(RSC_PROJ)  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\dialogs.c
DEP_DIALO=\
	.\wincap.h\
	.\dialogs.h

!IF  "$(CFG)" == "Win32 (80x86) Release"

$(INTDIR)/dialogs.obj :  $(SOURCE)  $(DEP_DIALO) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

$(INTDIR)/dialogs.obj :  $(SOURCE)  $(DEP_DIALO) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

$(INTDIR)/dialogs.obj :  $(SOURCE)  $(DEP_DIALO) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

$(INTDIR)/dialogs.obj :  $(SOURCE)  $(DEP_DIALO) $(INTDIR)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\dlgopen.c
DEP_DLGOP=\
	.\wincap.h\
	.\dialogs.h

!IF  "$(CFG)" == "Win32 (80x86) Release"

$(INTDIR)/dlgopen.obj :  $(SOURCE)  $(DEP_DLGOP) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

$(INTDIR)/dlgopen.obj :  $(SOURCE)  $(DEP_DLGOP) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

$(INTDIR)/dlgopen.obj :  $(SOURCE)  $(DEP_DLGOP) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

$(INTDIR)/dlgopen.obj :  $(SOURCE)  $(DEP_DLGOP) $(INTDIR)

!ENDIF 

# End Source File
# End Group
# End Project
################################################################################
