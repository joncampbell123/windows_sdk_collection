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
!MESSAGE NMAKE /f "dibapi32.mak" CFG="Win32 (80x86) Debug"
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
# PROP Output_Dir "WinRel"
# PROP Intermediate_Dir "WinRel"
OUTDIR=.\WinRel
INTDIR=.\WinRel

ALL : $(OUTDIR)/dibapi32.dll $(OUTDIR)/dibapi32.bsc

$(OUTDIR) : 
    if not exist $(OUTDIR)/nul mkdir $(OUTDIR)

MTL=MkTypLib.exe
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
CPP=cl.exe
# ADD BASE CPP /nologo /MD /W3 /GX /YX /O2 /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /c
# ADD CPP /nologo /MT /W3 /GX /YX /O2 /D "NDEBUG" /D "WIN32" /FR /c
CPP_PROJ=/nologo /MT /W3 /GX /YX /O2 /D "NDEBUG" /D "WIN32" /FR$(INTDIR)/\
 /Fp$(OUTDIR)/"dibapi32.pch" /Fo$(INTDIR)/ /c 
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
RSC_PROJ=/l 0x409 /fo$(INTDIR)/"dibapi.res" /d "NDEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# SUBTRACT BASE BSC32 /Iu
# ADD BSC32 /nologo
# SUBTRACT BSC32 /Iu
BSC32_FLAGS=/nologo /o$(OUTDIR)/"dibapi32.bsc" 
BSC32_SBRS= \
	$(INTDIR)/print.sbr \
	$(INTDIR)/wepcode.sbr \
	$(INTDIR)/dibutil.sbr \
	$(INTDIR)/file.sbr \
	$(INTDIR)/dllinit.sbr \
	$(INTDIR)/errors.sbr \
	$(INTDIR)/hook.sbr \
	$(INTDIR)/copy.sbr

$(OUTDIR)/dibapi32.bsc : $(OUTDIR)  $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib mfc30.lib mfco30.lib mfcd30.lib mfcuia32.lib mfcans32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib /NOLOGO /SUBSYSTEM:windows /DLL /MACHINE:I386
# SUBTRACT BASE LINK32 /PDB:none
# ADD LINK32 user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib /NOLOGO /SUBSYSTEM:windows /DLL /MACHINE:I386 /IMPLIB:"dibapi32.lib"
# SUBTRACT LINK32 /PDB:none
LINK32_FLAGS=user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib\
 /NOLOGO /SUBSYSTEM:windows /DLL /INCREMENTAL:no /PDB:$(OUTDIR)/"dibapi32.pdb"\
 /MACHINE:I386 /DEF:".\dibapi.def" /OUT:$(OUTDIR)/"dibapi32.dll"\
 /IMPLIB:"dibapi32.lib" 
DEF_FILE=.\dibapi.def
LINK32_OBJS= \
	$(INTDIR)/print.obj \
	$(INTDIR)/wepcode.obj \
	$(INTDIR)/dibutil.obj \
	$(INTDIR)/dibapi.res \
	$(INTDIR)/file.obj \
	$(INTDIR)/dllinit.obj \
	$(INTDIR)/errors.obj \
	$(INTDIR)/hook.obj \
	$(INTDIR)/copy.obj

$(OUTDIR)/dibapi32.dll : $(OUTDIR)  $(DEF_FILE) $(LINK32_OBJS)
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

ALL : $(OUTDIR)/dibapi32.dll $(OUTDIR)/dibapi32.bsc

$(OUTDIR) : 
    if not exist $(OUTDIR)/nul mkdir $(OUTDIR)

MTL=MkTypLib.exe
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
CPP=cl.exe
# ADD BASE CPP /nologo /MD /W3 /GX /Zi /YX /Od /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /c
# ADD CPP /nologo /MT /W3 /GX /Zi /YX /Od /D "_DEBUG" /D "WIN32" /FR /c
CPP_PROJ=/nologo /MT /W3 /GX /Zi /YX /Od /D "_DEBUG" /D "WIN32" /FR$(INTDIR)/\
 /Fp$(OUTDIR)/"dibapi32.pch" /Fo$(INTDIR)/ /Fd$(OUTDIR)/"dibapi32.pdb" /c 
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
RSC_PROJ=/l 0x409 /fo$(INTDIR)/"dibapi.res" /d "_DEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# SUBTRACT BASE BSC32 /Iu
# ADD BSC32 /nologo
# SUBTRACT BSC32 /Iu
BSC32_FLAGS=/nologo /o$(OUTDIR)/"dibapi32.bsc" 
BSC32_SBRS= \
	$(INTDIR)/print.sbr \
	$(INTDIR)/wepcode.sbr \
	$(INTDIR)/dibutil.sbr \
	$(INTDIR)/file.sbr \
	$(INTDIR)/dllinit.sbr \
	$(INTDIR)/errors.sbr \
	$(INTDIR)/hook.sbr \
	$(INTDIR)/copy.sbr

$(OUTDIR)/dibapi32.bsc : $(OUTDIR)  $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib mfc30d.lib mfco30d.lib mfcd30d.lib mfcuia32.lib mfcans32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib /NOLOGO /SUBSYSTEM:windows /DLL /DEBUG /MACHINE:I386
# SUBTRACT BASE LINK32 /PDB:none
# ADD LINK32 user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib /NOLOGO /SUBSYSTEM:windows /DLL /DEBUG /MACHINE:I386 /IMPLIB:"dibapi32.lib"
# SUBTRACT LINK32 /PDB:none
LINK32_FLAGS=user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib\
 /NOLOGO /SUBSYSTEM:windows /DLL /INCREMENTAL:yes /PDB:$(OUTDIR)/"dibapi32.pdb"\
 /DEBUG /MACHINE:I386 /DEF:".\dibapi.def" /OUT:$(OUTDIR)/"dibapi32.dll"\
 /IMPLIB:"dibapi32.lib" 
DEF_FILE=.\dibapi.def
LINK32_OBJS= \
	$(INTDIR)/print.obj \
	$(INTDIR)/wepcode.obj \
	$(INTDIR)/dibutil.obj \
	$(INTDIR)/dibapi.res \
	$(INTDIR)/file.obj \
	$(INTDIR)/dllinit.obj \
	$(INTDIR)/errors.obj \
	$(INTDIR)/hook.obj \
	$(INTDIR)/copy.obj

$(OUTDIR)/dibapi32.dll : $(OUTDIR)  $(DEF_FILE) $(LINK32_OBJS)
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

ALL : $(OUTDIR)/dibapi32.dll $(OUTDIR)/dibapi32.bsc

$(OUTDIR) : 
    if not exist $(OUTDIR)/nul mkdir $(OUTDIR)

MTL=MkTypLib.exe
# ADD BASE MTL /nologo /D "NDEBUG" /mips
# ADD MTL /nologo /D "NDEBUG" /mips
MTL_PROJ=/nologo /D "NDEBUG" /mips 
CPP=cl.exe
# ADD BASE CPP /nologo /MD /Gt0 /QMOb2000 /W3 /GX /YX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /c
# ADD CPP /nologo /MT /Gt0 /QMOb2000 /W3 /GX /YX /O2 /D "WIN32" /D "NDEBUG" /FR /c
CPP_PROJ=/nologo /MT /Gt0 /QMOb2000 /W3 /GX /YX /O2 /D "WIN32" /D "NDEBUG"\
 /FR$(INTDIR)/ /Fp$(OUTDIR)/"dibapi32.pch" /Fo$(INTDIR)/ /c 
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
RSC_PROJ=/l 0x409 /fo$(INTDIR)/"dibapi.res" /d "NDEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o$(OUTDIR)/"dibapi32.bsc" 
BSC32_SBRS= \
	$(INTDIR)/print.sbr \
	$(INTDIR)/wepcode.sbr \
	$(INTDIR)/dibutil.sbr \
	$(INTDIR)/file.sbr \
	$(INTDIR)/dllinit.sbr \
	$(INTDIR)/errors.sbr \
	$(INTDIR)/hook.sbr \
	$(INTDIR)/copy.sbr

$(OUTDIR)/dibapi32.bsc : $(OUTDIR)  $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 /NOLOGO /SUBSYSTEM:windows /DLL /MACHINE:MIPS
# SUBTRACT BASE LINK32 /PDB:none
# ADD LINK32 user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib /NOLOGO /SUBSYSTEM:windows /DLL /MACHINE:MIPS /IMPLIB:"dibapi32.lib"
# SUBTRACT LINK32 /PDB:none
LINK32_FLAGS=user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib\
 /NOLOGO /SUBSYSTEM:windows /DLL /PDB:$(OUTDIR)/"dibapi32.pdb" /MACHINE:MIPS\
 /DEF:".\dibapi.def" /OUT:$(OUTDIR)/"dibapi32.dll" /IMPLIB:"dibapi32.lib" 
DEF_FILE=.\dibapi.def
LINK32_OBJS= \
	$(INTDIR)/print.obj \
	$(INTDIR)/wepcode.obj \
	$(INTDIR)/dibutil.obj \
	$(INTDIR)/dibapi.res \
	$(INTDIR)/file.obj \
	$(INTDIR)/dllinit.obj \
	$(INTDIR)/errors.obj \
	$(INTDIR)/hook.obj \
	$(INTDIR)/copy.obj

$(OUTDIR)/dibapi32.dll : $(OUTDIR)  $(DEF_FILE) $(LINK32_OBJS)
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

ALL : $(OUTDIR)/dibapi32.dll $(OUTDIR)/dibapi32.bsc

$(OUTDIR) : 
    if not exist $(OUTDIR)/nul mkdir $(OUTDIR)

MTL=MkTypLib.exe
# ADD BASE MTL /nologo /D "_DEBUG" /mips
# ADD MTL /nologo /D "_DEBUG" /mips
MTL_PROJ=/nologo /D "_DEBUG" /mips 
CPP=cl.exe
# ADD BASE CPP /nologo /MD /Gt0 /QMOb2000 /W3 /GX /Zi /YX /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /c
# ADD CPP /nologo /MT /Gt0 /QMOb2000 /W3 /GX /Zi /YX /Od /D "WIN32" /D "_DEBUG" /FR /c
CPP_PROJ=/nologo /MT /Gt0 /QMOb2000 /W3 /GX /Zi /YX /Od /D "WIN32" /D "_DEBUG"\
 /FR$(INTDIR)/ /Fp$(OUTDIR)/"dibapi32.pch" /Fo$(INTDIR)/\
 /Fd$(OUTDIR)/"dibapi32.pdb" /c 
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
RSC_PROJ=/l 0x409 /fo$(INTDIR)/"dibapi.res" /d "_DEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o$(OUTDIR)/"dibapi32.bsc" 
BSC32_SBRS= \
	$(INTDIR)/print.sbr \
	$(INTDIR)/wepcode.sbr \
	$(INTDIR)/dibutil.sbr \
	$(INTDIR)/file.sbr \
	$(INTDIR)/dllinit.sbr \
	$(INTDIR)/errors.sbr \
	$(INTDIR)/hook.sbr \
	$(INTDIR)/copy.sbr

$(OUTDIR)/dibapi32.bsc : $(OUTDIR)  $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 /NOLOGO /SUBSYSTEM:windows /DLL /DEBUG /MACHINE:MIPS
# SUBTRACT BASE LINK32 /PDB:none
# ADD LINK32 user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib /NOLOGO /SUBSYSTEM:windows /DLL /DEBUG /MACHINE:MIPS /IMPLIB:"dibapi32.lib"
# SUBTRACT LINK32 /PDB:none
LINK32_FLAGS=user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib\
 /NOLOGO /SUBSYSTEM:windows /DLL /PDB:$(OUTDIR)/"dibapi32.pdb" /DEBUG\
 /MACHINE:MIPS /DEF:".\dibapi.def" /OUT:$(OUTDIR)/"dibapi32.dll"\
 /IMPLIB:"dibapi32.lib" 
DEF_FILE=.\dibapi.def
LINK32_OBJS= \
	$(INTDIR)/print.obj \
	$(INTDIR)/wepcode.obj \
	$(INTDIR)/dibutil.obj \
	$(INTDIR)/dibapi.res \
	$(INTDIR)/file.obj \
	$(INTDIR)/dllinit.obj \
	$(INTDIR)/errors.obj \
	$(INTDIR)/hook.obj \
	$(INTDIR)/copy.obj

$(OUTDIR)/dibapi32.dll : $(OUTDIR)  $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 

################################################################################
# Begin Group "Source Files"

################################################################################
# Begin Source File

SOURCE=.\print.c
DEP_PRINT=\
	.\dibdll.h\
	.\dibapi.h\
	.\dibutil.h

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

SOURCE=.\wepcode.c

!IF  "$(CFG)" == "Win32 (80x86) Release"

$(INTDIR)/wepcode.obj :  $(SOURCE)  $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

$(INTDIR)/wepcode.obj :  $(SOURCE)  $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

$(INTDIR)/wepcode.obj :  $(SOURCE)  $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

$(INTDIR)/wepcode.obj :  $(SOURCE)  $(INTDIR)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\dibutil.c
DEP_DIBUT=\
	.\dibapi.h\
	.\dibutil.h

!IF  "$(CFG)" == "Win32 (80x86) Release"

$(INTDIR)/dibutil.obj :  $(SOURCE)  $(DEP_DIBUT) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

$(INTDIR)/dibutil.obj :  $(SOURCE)  $(DEP_DIBUT) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

$(INTDIR)/dibutil.obj :  $(SOURCE)  $(DEP_DIBUT) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

$(INTDIR)/dibutil.obj :  $(SOURCE)  $(DEP_DIBUT) $(INTDIR)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\dibapi.rc

!IF  "$(CFG)" == "Win32 (80x86) Release"

$(INTDIR)/dibapi.res :  $(SOURCE)  $(INTDIR)
   $(RSC) $(RSC_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

$(INTDIR)/dibapi.res :  $(SOURCE)  $(INTDIR)
   $(RSC) $(RSC_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

$(INTDIR)/dibapi.res :  $(SOURCE)  $(INTDIR)
   $(RSC) $(RSC_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

$(INTDIR)/dibapi.res :  $(SOURCE)  $(INTDIR)
   $(RSC) $(RSC_PROJ)  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\dibapi.def
# End Source File
################################################################################
# Begin Source File

SOURCE=.\file.c
DEP_FILE_=\
	.\dibutil.h\
	.\dibapi.h

!IF  "$(CFG)" == "Win32 (80x86) Release"

$(INTDIR)/file.obj :  $(SOURCE)  $(DEP_FILE_) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

$(INTDIR)/file.obj :  $(SOURCE)  $(DEP_FILE_) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

$(INTDIR)/file.obj :  $(SOURCE)  $(DEP_FILE_) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

$(INTDIR)/file.obj :  $(SOURCE)  $(DEP_FILE_) $(INTDIR)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\dllinit.c
DEP_DLLIN=\
	.\dibdll.h

!IF  "$(CFG)" == "Win32 (80x86) Release"

$(INTDIR)/dllinit.obj :  $(SOURCE)  $(DEP_DLLIN) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

$(INTDIR)/dllinit.obj :  $(SOURCE)  $(DEP_DLLIN) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

$(INTDIR)/dllinit.obj :  $(SOURCE)  $(DEP_DLLIN) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

$(INTDIR)/dllinit.obj :  $(SOURCE)  $(DEP_DLLIN) $(INTDIR)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\errors.c
DEP_ERROR=\
	.\dibapi.h

!IF  "$(CFG)" == "Win32 (80x86) Release"

$(INTDIR)/errors.obj :  $(SOURCE)  $(DEP_ERROR) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

$(INTDIR)/errors.obj :  $(SOURCE)  $(DEP_ERROR) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

$(INTDIR)/errors.obj :  $(SOURCE)  $(DEP_ERROR) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

$(INTDIR)/errors.obj :  $(SOURCE)  $(DEP_ERROR) $(INTDIR)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\hook.c
DEP_HOOK_=\
	.\wincap.h

!IF  "$(CFG)" == "Win32 (80x86) Release"

$(INTDIR)/hook.obj :  $(SOURCE)  $(DEP_HOOK_) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

$(INTDIR)/hook.obj :  $(SOURCE)  $(DEP_HOOK_) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

$(INTDIR)/hook.obj :  $(SOURCE)  $(DEP_HOOK_) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

$(INTDIR)/hook.obj :  $(SOURCE)  $(DEP_HOOK_) $(INTDIR)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\copy.c
DEP_COPY_=\
	.\dibutil.h\
	.\dibapi.h

!IF  "$(CFG)" == "Win32 (80x86) Release"

$(INTDIR)/copy.obj :  $(SOURCE)  $(DEP_COPY_) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

$(INTDIR)/copy.obj :  $(SOURCE)  $(DEP_COPY_) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

$(INTDIR)/copy.obj :  $(SOURCE)  $(DEP_COPY_) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

$(INTDIR)/copy.obj :  $(SOURCE)  $(DEP_COPY_) $(INTDIR)

!ENDIF 

# End Source File
# End Group
# End Project
################################################################################
