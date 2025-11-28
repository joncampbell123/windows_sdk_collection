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
!MESSAGE NMAKE /f "showdib.mak" CFG="Win32 (80x86) Debug"
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

ALL : $(OUTDIR)/showdib.exe $(OUTDIR)/showdib.bsc

$(OUTDIR) : 
    if not exist $(OUTDIR)/nul mkdir $(OUTDIR)

MTL=MkTypLib.exe
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
CPP=cl.exe
# ADD BASE CPP /nologo /MD /W3 /GX /YX /O2 /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /c
# ADD CPP /nologo /W3 /GX /YX /O2 /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "WIN32" /FR /c
CPP_PROJ=/nologo /W3 /GX /YX /O2 /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D\
 "WIN32" /FR$(INTDIR)/ /Fp$(OUTDIR)/"showdib.pch" /Fo$(INTDIR)/ /c 
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
RSC_PROJ=/l 0x409 /fo$(INTDIR)/"showdib.res" /d "NDEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# SUBTRACT BASE BSC32 /Iu
# ADD BSC32 /nologo
# SUBTRACT BSC32 /Iu
BSC32_FLAGS=/nologo /o$(OUTDIR)/"showdib.bsc" 
BSC32_SBRS= \
	$(INTDIR)/showdib.sbr \
	$(INTDIR)/drawdib.sbr \
	$(INTDIR)/print.sbr \
	$(INTDIR)/dlgopen.sbr \
	$(INTDIR)/dib.sbr

$(OUTDIR)/showdib.bsc : $(OUTDIR)  $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib mfc30.lib mfco30.lib mfcd30.lib compob32.lib storag32.lib ole2w32.lib ole232.lib ole2di32.lib /NOLOGO /SUBSYSTEM:windows /MACHINE:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /NOLOGO /SUBSYSTEM:windows /MACHINE:I386
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /NOLOGO\
 /SUBSYSTEM:windows /INCREMENTAL:no /PDB:$(OUTDIR)/"showdib.pdb" /MACHINE:I386\
 /OUT:$(OUTDIR)/"showdib.exe" 
DEF_FILE=
LINK32_OBJS= \
	$(INTDIR)/showdib.obj \
	$(INTDIR)/drawdib.obj \
	$(INTDIR)/print.obj \
	$(INTDIR)/dlgopen.obj \
	$(INTDIR)/dib.obj \
	$(INTDIR)/showdib.res

$(OUTDIR)/showdib.exe : $(OUTDIR)  $(DEF_FILE) $(LINK32_OBJS)
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

ALL : $(OUTDIR)/showdib.exe $(OUTDIR)/showdib.bsc

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
 "WIN32" /FR$(INTDIR)/ /Fp$(OUTDIR)/"showdib.pch" /Fo$(INTDIR)/\
 /Fd$(OUTDIR)/"showdib.pdb" /c 
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
RSC_PROJ=/l 0x409 /fo$(INTDIR)/"showdib.res" /d "_DEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# SUBTRACT BASE BSC32 /Iu
# ADD BSC32 /nologo
# SUBTRACT BSC32 /Iu
BSC32_FLAGS=/nologo /o$(OUTDIR)/"showdib.bsc" 
BSC32_SBRS= \
	$(INTDIR)/showdib.sbr \
	$(INTDIR)/drawdib.sbr \
	$(INTDIR)/print.sbr \
	$(INTDIR)/dlgopen.sbr \
	$(INTDIR)/dib.sbr

$(OUTDIR)/showdib.bsc : $(OUTDIR)  $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib mfc30d.lib mfco30d.lib mfcd30d.lib compob32.lib storag32.lib ole2w32.lib ole232.lib ole2di32.lib /NOLOGO /SUBSYSTEM:windows /DEBUG /MACHINE:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /NOLOGO /SUBSYSTEM:windows /DEBUG /MACHINE:I386
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /NOLOGO\
 /SUBSYSTEM:windows /INCREMENTAL:yes /PDB:$(OUTDIR)/"showdib.pdb" /DEBUG\
 /MACHINE:I386 /OUT:$(OUTDIR)/"showdib.exe" 
DEF_FILE=
LINK32_OBJS= \
	$(INTDIR)/showdib.obj \
	$(INTDIR)/drawdib.obj \
	$(INTDIR)/print.obj \
	$(INTDIR)/dlgopen.obj \
	$(INTDIR)/dib.obj \
	$(INTDIR)/showdib.res

$(OUTDIR)/showdib.exe : $(OUTDIR)  $(DEF_FILE) $(LINK32_OBJS)
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

ALL : $(OUTDIR)/showdib.exe $(OUTDIR)/showdib.bsc

$(OUTDIR) : 
    if not exist $(OUTDIR)/nul mkdir $(OUTDIR)

MTL=MkTypLib.exe
# ADD BASE MTL /nologo /D "NDEBUG" /mips
# ADD MTL /nologo /D "NDEBUG" /mips
MTL_PROJ=/nologo /D "NDEBUG" /mips 
CPP=cl.exe
# ADD BASE CPP /nologo /MD /Gt0 /QMOb2000 /W3 /GX /YX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /c
# ADD CPP /nologo /Gt0 /QMOb2000 /W3 /GX /YX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /FR /c
CPP_PROJ=/nologo /Gt0 /QMOb2000 /W3 /GX /YX /O2 /D "WIN32" /D "NDEBUG" /D\
 "_WINDOWS" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"showdib.pch" /Fo$(INTDIR)/ /c\
 
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
RSC_PROJ=/l 0x409 /fo$(INTDIR)/"showdib.res" /d "NDEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# SUBTRACT BASE BSC32 /Iu
# ADD BSC32 /nologo
# SUBTRACT BSC32 /Iu
BSC32_FLAGS=/nologo /o$(OUTDIR)/"showdib.bsc" 
BSC32_SBRS= \
	$(INTDIR)/showdib.sbr \
	$(INTDIR)/drawdib.sbr \
	$(INTDIR)/print.sbr \
	$(INTDIR)/dlgopen.sbr \
	$(INTDIR)/dib.sbr

$(OUTDIR)/showdib.bsc : $(OUTDIR)  $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 /NOLOGO /SUBSYSTEM:windows /MACHINE:MIPS
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /NOLOGO /SUBSYSTEM:windows /MACHINE:MIPS
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /NOLOGO\
 /SUBSYSTEM:windows /PDB:$(OUTDIR)/"showdib.pdb" /MACHINE:MIPS\
 /OUT:$(OUTDIR)/"showdib.exe" 
DEF_FILE=
LINK32_OBJS= \
	$(INTDIR)/showdib.obj \
	$(INTDIR)/drawdib.obj \
	$(INTDIR)/print.obj \
	$(INTDIR)/dlgopen.obj \
	$(INTDIR)/dib.obj \
	$(INTDIR)/showdib.res

$(OUTDIR)/showdib.exe : $(OUTDIR)  $(DEF_FILE) $(LINK32_OBJS)
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

ALL : $(OUTDIR)/showdib.exe $(OUTDIR)/showdib.bsc

$(OUTDIR) : 
    if not exist $(OUTDIR)/nul mkdir $(OUTDIR)

MTL=MkTypLib.exe
# ADD BASE MTL /nologo /D "_DEBUG" /mips
# ADD MTL /nologo /D "_DEBUG" /mips
MTL_PROJ=/nologo /D "_DEBUG" /mips 
CPP=cl.exe
# ADD BASE CPP /nologo /MD /Gt0 /QMOb2000 /W3 /GX /Zi /YX /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /c
# ADD CPP /nologo /Gt0 /QMOb2000 /W3 /GX /Zi /YX /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /FR /c
CPP_PROJ=/nologo /Gt0 /QMOb2000 /W3 /GX /Zi /YX /Od /D "WIN32" /D "_DEBUG" /D\
 "_WINDOWS" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"showdib.pch" /Fo$(INTDIR)/\
 /Fd$(OUTDIR)/"showdib.pdb" /c 
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
RSC_PROJ=/l 0x409 /fo$(INTDIR)/"showdib.res" /d "_DEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# SUBTRACT BASE BSC32 /Iu
# ADD BSC32 /nologo
# SUBTRACT BSC32 /Iu
BSC32_FLAGS=/nologo /o$(OUTDIR)/"showdib.bsc" 
BSC32_SBRS= \
	$(INTDIR)/showdib.sbr \
	$(INTDIR)/drawdib.sbr \
	$(INTDIR)/print.sbr \
	$(INTDIR)/dlgopen.sbr \
	$(INTDIR)/dib.sbr

$(OUTDIR)/showdib.bsc : $(OUTDIR)  $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 /NOLOGO /SUBSYSTEM:windows /DEBUG /MACHINE:MIPS
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /NOLOGO /SUBSYSTEM:windows /DEBUG /MACHINE:MIPS
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /NOLOGO\
 /SUBSYSTEM:windows /PDB:$(OUTDIR)/"showdib.pdb" /DEBUG /MACHINE:MIPS\
 /OUT:$(OUTDIR)/"showdib.exe" 
DEF_FILE=
LINK32_OBJS= \
	$(INTDIR)/showdib.obj \
	$(INTDIR)/drawdib.obj \
	$(INTDIR)/print.obj \
	$(INTDIR)/dlgopen.obj \
	$(INTDIR)/dib.obj \
	$(INTDIR)/showdib.res

$(OUTDIR)/showdib.exe : $(OUTDIR)  $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 

################################################################################
# Begin Group "Source Files"

################################################################################
# Begin Source File

SOURCE=.\showdib.c
DEP_SHOWD=\
	.\showdib.h

!IF  "$(CFG)" == "Win32 (80x86) Release"

$(INTDIR)/showdib.obj :  $(SOURCE)  $(DEP_SHOWD) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

$(INTDIR)/showdib.obj :  $(SOURCE)  $(DEP_SHOWD) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

$(INTDIR)/showdib.obj :  $(SOURCE)  $(DEP_SHOWD) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

$(INTDIR)/showdib.obj :  $(SOURCE)  $(DEP_SHOWD) $(INTDIR)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\drawdib.c
DEP_DRAWD=\
	.\showdib.h

!IF  "$(CFG)" == "Win32 (80x86) Release"

$(INTDIR)/drawdib.obj :  $(SOURCE)  $(DEP_DRAWD) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

$(INTDIR)/drawdib.obj :  $(SOURCE)  $(DEP_DRAWD) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

$(INTDIR)/drawdib.obj :  $(SOURCE)  $(DEP_DRAWD) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

$(INTDIR)/drawdib.obj :  $(SOURCE)  $(DEP_DRAWD) $(INTDIR)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\print.c
DEP_PRINT=\
	.\showdib.h

!IF  "$(CFG)" == "Win32 (80x86) Release"

$(INTDIR)/print.obj :  $(SOURCE)  $(DEP_PRINT) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

$(INTDIR)/print.obj :  $(SOURCE)  $(DEP_PRINT) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

$(INTDIR)/print.obj :  $(SOURCE)  $(DEP_PRINT) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

$(INTDIR)/print.obj :  $(SOURCE)  $(DEP_PRINT) $(INTDIR)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\dlgopen.c
DEP_DLGOP=\
	.\showdib.h

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
################################################################################
# Begin Source File

SOURCE=.\dib.c
DEP_DIB_C=\
	.\showdib.h

!IF  "$(CFG)" == "Win32 (80x86) Release"

$(INTDIR)/dib.obj :  $(SOURCE)  $(DEP_DIB_C) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

$(INTDIR)/dib.obj :  $(SOURCE)  $(DEP_DIB_C) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

$(INTDIR)/dib.obj :  $(SOURCE)  $(DEP_DIB_C) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

$(INTDIR)/dib.obj :  $(SOURCE)  $(DEP_DIB_C) $(INTDIR)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\showdib.rc

!IF  "$(CFG)" == "Win32 (80x86) Release"

$(INTDIR)/showdib.res :  $(SOURCE)  $(INTDIR)
   $(RSC) $(RSC_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

$(INTDIR)/showdib.res :  $(SOURCE)  $(INTDIR)
   $(RSC) $(RSC_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

$(INTDIR)/showdib.res :  $(SOURCE)  $(INTDIR)
   $(RSC) $(RSC_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

$(INTDIR)/showdib.res :  $(SOURCE)  $(INTDIR)
   $(RSC) $(RSC_PROJ)  $(SOURCE) 

!ENDIF 

# End Source File
# End Group
# End Project
################################################################################
