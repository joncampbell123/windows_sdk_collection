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
!MESSAGE NMAKE /f "acmapp.mak" CFG="Win32 (80x86) Debug"
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

ALL : $(OUTDIR)/acmapp.exe $(OUTDIR)/acmapp.bsc

$(OUTDIR) : 
    if not exist $(OUTDIR)/nul mkdir $(OUTDIR)

MTL=MkTypLib.exe
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
CPP=cl.exe
# ADD BASE CPP /nologo /MD /W3 /GX /YX /O2 /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /c
# ADD CPP /nologo /W3 /GX /YX /O2 /D "NDEBUG" /D "WIN32" /D _X86_=1 /FR /c
CPP_PROJ=/nologo /W3 /GX /YX /O2 /D "NDEBUG" /D "WIN32" /D _X86_=1\
 /FR$(INTDIR)/ /Fp$(OUTDIR)/"acmapp.pch" /Fo$(INTDIR)/ /c 
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
RSC_PROJ=/l 0x409 /fo$(INTDIR)/"acmapp.res" /d "NDEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# SUBTRACT BASE BSC32 /Iu
# ADD BSC32 /nologo
# SUBTRACT BSC32 /Iu
BSC32_FLAGS=/nologo /o$(OUTDIR)/"acmapp.bsc" 
BSC32_SBRS= \
	$(INTDIR)/acmapp.sbr \
	$(INTDIR)/aawavdev.sbr \
	$(INTDIR)/aadrvs.sbr \
	$(INTDIR)/tlb.sbr \
	$(INTDIR)/debug.sbr \
	$(INTDIR)/aafile.sbr \
	$(INTDIR)/aainit.sbr \
	$(INTDIR)/aachoose.sbr \
	$(INTDIR)/acmthunk.sbr \
	$(INTDIR)/aaplyrec.sbr \
	$(INTDIR)/aasysinf.sbr \
	$(INTDIR)/waveio.sbr \
	$(INTDIR)/aaprops.sbr

$(OUTDIR)/acmapp.bsc : $(OUTDIR)  $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib mfc30.lib mfco30.lib mfcd30.lib mfcuia32.lib mfcans32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib /NOLOGO /SUBSYSTEM:windows /MACHINE:I386
# ADD LINK32 winmm.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /NOLOGO /SUBSYSTEM:windows /MACHINE:I386
LINK32_FLAGS=winmm.lib kernel32.lib user32.lib gdi32.lib winspool.lib\
 comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /NOLOGO\
 /SUBSYSTEM:windows /INCREMENTAL:no /PDB:$(OUTDIR)/"acmapp.pdb" /MACHINE:I386\
 /OUT:$(OUTDIR)/"acmapp.exe" 
DEF_FILE=
LINK32_OBJS= \
	$(INTDIR)/acmapp.obj \
	$(INTDIR)/acmapp.res \
	$(INTDIR)/aawavdev.obj \
	$(INTDIR)/aadrvs.obj \
	$(INTDIR)/tlb.obj \
	$(INTDIR)/debug.obj \
	$(INTDIR)/aafile.obj \
	$(INTDIR)/aainit.obj \
	$(INTDIR)/aachoose.obj \
	$(INTDIR)/acmthunk.obj \
	$(INTDIR)/aaplyrec.obj \
	$(INTDIR)/aasysinf.obj \
	$(INTDIR)/waveio.obj \
	$(INTDIR)/aaprops.obj

$(OUTDIR)/acmapp.exe : $(OUTDIR)  $(DEF_FILE) $(LINK32_OBJS)
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

ALL : $(OUTDIR)/acmapp.exe $(OUTDIR)/acmapp.bsc

$(OUTDIR) : 
    if not exist $(OUTDIR)/nul mkdir $(OUTDIR)

MTL=MkTypLib.exe
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
CPP=cl.exe
# ADD BASE CPP /nologo /MD /W3 /GX /Zi /YX /Od /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /c
# ADD CPP /nologo /W3 /GX /Zi /YX /Od /D "_DEBUG" /D "WIN32" /D _X86_=1 /FR /c
CPP_PROJ=/nologo /W3 /GX /Zi /YX /Od /D "_DEBUG" /D "WIN32" /D _X86_=1\
 /FR$(INTDIR)/ /Fp$(OUTDIR)/"acmapp.pch" /Fo$(INTDIR)/ /Fd$(OUTDIR)/"acmapp.pdb"\
 /c 
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
RSC_PROJ=/l 0x409 /fo$(INTDIR)/"acmapp.res" /d "_DEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# SUBTRACT BASE BSC32 /Iu
# ADD BSC32 /nologo
# SUBTRACT BSC32 /Iu
BSC32_FLAGS=/nologo /o$(OUTDIR)/"acmapp.bsc" 
BSC32_SBRS= \
	$(INTDIR)/acmapp.sbr \
	$(INTDIR)/aawavdev.sbr \
	$(INTDIR)/aadrvs.sbr \
	$(INTDIR)/tlb.sbr \
	$(INTDIR)/debug.sbr \
	$(INTDIR)/aafile.sbr \
	$(INTDIR)/aainit.sbr \
	$(INTDIR)/aachoose.sbr \
	$(INTDIR)/acmthunk.sbr \
	$(INTDIR)/aaplyrec.sbr \
	$(INTDIR)/aasysinf.sbr \
	$(INTDIR)/waveio.sbr \
	$(INTDIR)/aaprops.sbr

$(OUTDIR)/acmapp.bsc : $(OUTDIR)  $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib mfc30d.lib mfco30d.lib mfcd30d.lib mfcuia32.lib mfcans32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib /NOLOGO /SUBSYSTEM:windows /DEBUG /MACHINE:I386
# ADD LINK32 winmm.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /NOLOGO /SUBSYSTEM:windows /DEBUG /MACHINE:I386
LINK32_FLAGS=winmm.lib kernel32.lib user32.lib gdi32.lib winspool.lib\
 comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /NOLOGO\
 /SUBSYSTEM:windows /INCREMENTAL:yes /PDB:$(OUTDIR)/"acmapp.pdb" /DEBUG\
 /MACHINE:I386 /OUT:$(OUTDIR)/"acmapp.exe" 
DEF_FILE=
LINK32_OBJS= \
	$(INTDIR)/acmapp.obj \
	$(INTDIR)/acmapp.res \
	$(INTDIR)/aawavdev.obj \
	$(INTDIR)/aadrvs.obj \
	$(INTDIR)/tlb.obj \
	$(INTDIR)/debug.obj \
	$(INTDIR)/aafile.obj \
	$(INTDIR)/aainit.obj \
	$(INTDIR)/aachoose.obj \
	$(INTDIR)/acmthunk.obj \
	$(INTDIR)/aaplyrec.obj \
	$(INTDIR)/aasysinf.obj \
	$(INTDIR)/waveio.obj \
	$(INTDIR)/aaprops.obj

$(OUTDIR)/acmapp.exe : $(OUTDIR)  $(DEF_FILE) $(LINK32_OBJS)
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

ALL : $(OUTDIR)/acmapp.exe $(OUTDIR)/acmapp.bsc

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
 "_WINDOWS" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"acmapp.pch" /Fo$(INTDIR)/ /c 
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
RSC_PROJ=/l 0x409 /fo$(INTDIR)/"acmapp.res" /d "NDEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# SUBTRACT BASE BSC32 /Iu
# ADD BSC32 /nologo
# SUBTRACT BSC32 /Iu
BSC32_FLAGS=/nologo /o$(OUTDIR)/"acmapp.bsc" 
BSC32_SBRS= \
	$(INTDIR)/acmapp.sbr \
	$(INTDIR)/aawavdev.sbr \
	$(INTDIR)/aadrvs.sbr \
	$(INTDIR)/tlb.sbr \
	$(INTDIR)/debug.sbr \
	$(INTDIR)/aafile.sbr \
	$(INTDIR)/aainit.sbr \
	$(INTDIR)/aachoose.sbr \
	$(INTDIR)/acmthunk.sbr \
	$(INTDIR)/aaplyrec.sbr \
	$(INTDIR)/aasysinf.sbr \
	$(INTDIR)/waveio.sbr \
	$(INTDIR)/aaprops.sbr

$(OUTDIR)/acmapp.bsc : $(OUTDIR)  $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 /NOLOGO /SUBSYSTEM:windows /MACHINE:MIPS
# ADD LINK32 winmm.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /NOLOGO /SUBSYSTEM:windows /MACHINE:MIPS
LINK32_FLAGS=winmm.lib kernel32.lib user32.lib gdi32.lib winspool.lib\
 comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /NOLOGO\
 /SUBSYSTEM:windows /PDB:$(OUTDIR)/"acmapp.pdb" /MACHINE:MIPS\
 /OUT:$(OUTDIR)/"acmapp.exe" 
DEF_FILE=
LINK32_OBJS= \
	$(INTDIR)/acmapp.obj \
	$(INTDIR)/acmapp.res \
	$(INTDIR)/aawavdev.obj \
	$(INTDIR)/aadrvs.obj \
	$(INTDIR)/tlb.obj \
	$(INTDIR)/debug.obj \
	$(INTDIR)/aafile.obj \
	$(INTDIR)/aainit.obj \
	$(INTDIR)/aachoose.obj \
	$(INTDIR)/acmthunk.obj \
	$(INTDIR)/aaplyrec.obj \
	$(INTDIR)/aasysinf.obj \
	$(INTDIR)/waveio.obj \
	$(INTDIR)/aaprops.obj

$(OUTDIR)/acmapp.exe : $(OUTDIR)  $(DEF_FILE) $(LINK32_OBJS)
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

ALL : $(OUTDIR)/acmapp.exe $(OUTDIR)/acmapp.bsc

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
 "_WINDOWS" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"acmapp.pch" /Fo$(INTDIR)/\
 /Fd$(OUTDIR)/"acmapp.pdb" /c 
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
RSC_PROJ=/l 0x409 /fo$(INTDIR)/"acmapp.res" /d "_DEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# SUBTRACT BASE BSC32 /Iu
# ADD BSC32 /nologo
# SUBTRACT BSC32 /Iu
BSC32_FLAGS=/nologo /o$(OUTDIR)/"acmapp.bsc" 
BSC32_SBRS= \
	$(INTDIR)/acmapp.sbr \
	$(INTDIR)/aawavdev.sbr \
	$(INTDIR)/aadrvs.sbr \
	$(INTDIR)/tlb.sbr \
	$(INTDIR)/debug.sbr \
	$(INTDIR)/aafile.sbr \
	$(INTDIR)/aainit.sbr \
	$(INTDIR)/aachoose.sbr \
	$(INTDIR)/acmthunk.sbr \
	$(INTDIR)/aaplyrec.sbr \
	$(INTDIR)/aasysinf.sbr \
	$(INTDIR)/waveio.sbr \
	$(INTDIR)/aaprops.sbr

$(OUTDIR)/acmapp.bsc : $(OUTDIR)  $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 /NOLOGO /SUBSYSTEM:windows /DEBUG /MACHINE:MIPS
# ADD LINK32 winmm.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /NOLOGO /SUBSYSTEM:windows /DEBUG /MACHINE:MIPS
LINK32_FLAGS=winmm.lib kernel32.lib user32.lib gdi32.lib winspool.lib\
 comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /NOLOGO\
 /SUBSYSTEM:windows /PDB:$(OUTDIR)/"acmapp.pdb" /DEBUG /MACHINE:MIPS\
 /OUT:$(OUTDIR)/"acmapp.exe" 
DEF_FILE=
LINK32_OBJS= \
	$(INTDIR)/acmapp.obj \
	$(INTDIR)/acmapp.res \
	$(INTDIR)/aawavdev.obj \
	$(INTDIR)/aadrvs.obj \
	$(INTDIR)/tlb.obj \
	$(INTDIR)/debug.obj \
	$(INTDIR)/aafile.obj \
	$(INTDIR)/aainit.obj \
	$(INTDIR)/aachoose.obj \
	$(INTDIR)/acmthunk.obj \
	$(INTDIR)/aaplyrec.obj \
	$(INTDIR)/aasysinf.obj \
	$(INTDIR)/waveio.obj \
	$(INTDIR)/aaprops.obj

$(OUTDIR)/acmapp.exe : $(OUTDIR)  $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 

################################################################################
# Begin Group "Source Files"

################################################################################
# Begin Source File

SOURCE=.\acmapp.c
DEP_ACMAP=\
	.\appport.h\
	.\acmapp.h\
	.\debug.h

!IF  "$(CFG)" == "Win32 (80x86) Release"

$(INTDIR)/acmapp.obj :  $(SOURCE)  $(DEP_ACMAP) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

$(INTDIR)/acmapp.obj :  $(SOURCE)  $(DEP_ACMAP) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

$(INTDIR)/acmapp.obj :  $(SOURCE)  $(DEP_ACMAP) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

$(INTDIR)/acmapp.obj :  $(SOURCE)  $(DEP_ACMAP) $(INTDIR)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\acmapp.rc

!IF  "$(CFG)" == "Win32 (80x86) Release"

$(INTDIR)/acmapp.res :  $(SOURCE)  $(INTDIR)
   $(RSC) $(RSC_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

$(INTDIR)/acmapp.res :  $(SOURCE)  $(INTDIR)
   $(RSC) $(RSC_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

$(INTDIR)/acmapp.res :  $(SOURCE)  $(INTDIR)
   $(RSC) $(RSC_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

$(INTDIR)/acmapp.res :  $(SOURCE)  $(INTDIR)
   $(RSC) $(RSC_PROJ)  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\aawavdev.c
DEP_AAWAV=\
	.\appport.h\
	.\acmapp.h\
	.\debug.h

!IF  "$(CFG)" == "Win32 (80x86) Release"

$(INTDIR)/aawavdev.obj :  $(SOURCE)  $(DEP_AAWAV) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

$(INTDIR)/aawavdev.obj :  $(SOURCE)  $(DEP_AAWAV) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

$(INTDIR)/aawavdev.obj :  $(SOURCE)  $(DEP_AAWAV) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

$(INTDIR)/aawavdev.obj :  $(SOURCE)  $(DEP_AAWAV) $(INTDIR)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\aadrvs.c
DEP_AADRV=\
	.\tlb.h\
	.\appport.h\
	.\acmapp.h\
	.\debug.h

!IF  "$(CFG)" == "Win32 (80x86) Release"

$(INTDIR)/aadrvs.obj :  $(SOURCE)  $(DEP_AADRV) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

$(INTDIR)/aadrvs.obj :  $(SOURCE)  $(DEP_AADRV) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

$(INTDIR)/aadrvs.obj :  $(SOURCE)  $(DEP_AADRV) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

$(INTDIR)/aadrvs.obj :  $(SOURCE)  $(DEP_AADRV) $(INTDIR)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\tlb.c
DEP_TLB_C=\
	.\tlb.h

!IF  "$(CFG)" == "Win32 (80x86) Release"

$(INTDIR)/tlb.obj :  $(SOURCE)  $(DEP_TLB_C) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

$(INTDIR)/tlb.obj :  $(SOURCE)  $(DEP_TLB_C) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

$(INTDIR)/tlb.obj :  $(SOURCE)  $(DEP_TLB_C) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

$(INTDIR)/tlb.obj :  $(SOURCE)  $(DEP_TLB_C) $(INTDIR)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\debug.c
DEP_DEBUG=\
	.\debug.h

!IF  "$(CFG)" == "Win32 (80x86) Release"

$(INTDIR)/debug.obj :  $(SOURCE)  $(DEP_DEBUG) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

$(INTDIR)/debug.obj :  $(SOURCE)  $(DEP_DEBUG) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

$(INTDIR)/debug.obj :  $(SOURCE)  $(DEP_DEBUG) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

$(INTDIR)/debug.obj :  $(SOURCE)  $(DEP_DEBUG) $(INTDIR)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\aafile.c
DEP_AAFIL=\
	.\appport.h\
	.\waveio.h\
	.\acmapp.h\
	.\debug.h

!IF  "$(CFG)" == "Win32 (80x86) Release"

$(INTDIR)/aafile.obj :  $(SOURCE)  $(DEP_AAFIL) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

$(INTDIR)/aafile.obj :  $(SOURCE)  $(DEP_AAFIL) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

$(INTDIR)/aafile.obj :  $(SOURCE)  $(DEP_AAFIL) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

$(INTDIR)/aafile.obj :  $(SOURCE)  $(DEP_AAFIL) $(INTDIR)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\aainit.c
DEP_AAINI=\
	.\acmthunk.h\
	.\appport.h\
	.\acmapp.h\
	.\debug.h

!IF  "$(CFG)" == "Win32 (80x86) Release"

$(INTDIR)/aainit.obj :  $(SOURCE)  $(DEP_AAINI) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

$(INTDIR)/aainit.obj :  $(SOURCE)  $(DEP_AAINI) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

$(INTDIR)/aainit.obj :  $(SOURCE)  $(DEP_AAINI) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

$(INTDIR)/aainit.obj :  $(SOURCE)  $(DEP_AAINI) $(INTDIR)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\aachoose.c
DEP_AACHO=\
	.\muldiv32.h\
	.\appport.h\
	.\waveio.h\
	.\acmapp.h\
	.\debug.h

!IF  "$(CFG)" == "Win32 (80x86) Release"

$(INTDIR)/aachoose.obj :  $(SOURCE)  $(DEP_AACHO) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

$(INTDIR)/aachoose.obj :  $(SOURCE)  $(DEP_AACHO) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

$(INTDIR)/aachoose.obj :  $(SOURCE)  $(DEP_AACHO) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

$(INTDIR)/aachoose.obj :  $(SOURCE)  $(DEP_AACHO) $(INTDIR)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\acmthunk.c

!IF  "$(CFG)" == "Win32 (80x86) Release"

$(INTDIR)/acmthunk.obj :  $(SOURCE)  $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

$(INTDIR)/acmthunk.obj :  $(SOURCE)  $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

$(INTDIR)/acmthunk.obj :  $(SOURCE)  $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

$(INTDIR)/acmthunk.obj :  $(SOURCE)  $(INTDIR)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\aaplyrec.c
DEP_AAPLY=\
	.\muldiv32.h\
	.\appport.h\
	.\acmapp.h\
	.\debug.h

!IF  "$(CFG)" == "Win32 (80x86) Release"

$(INTDIR)/aaplyrec.obj :  $(SOURCE)  $(DEP_AAPLY) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

$(INTDIR)/aaplyrec.obj :  $(SOURCE)  $(DEP_AAPLY) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

$(INTDIR)/aaplyrec.obj :  $(SOURCE)  $(DEP_AAPLY) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

$(INTDIR)/aaplyrec.obj :  $(SOURCE)  $(DEP_AAPLY) $(INTDIR)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\aasysinf.c
DEP_AASYS=\
	.\appport.h\
	.\acmapp.h\
	.\debug.h

!IF  "$(CFG)" == "Win32 (80x86) Release"

$(INTDIR)/aasysinf.obj :  $(SOURCE)  $(DEP_AASYS) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

$(INTDIR)/aasysinf.obj :  $(SOURCE)  $(DEP_AASYS) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

$(INTDIR)/aasysinf.obj :  $(SOURCE)  $(DEP_AASYS) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

$(INTDIR)/aasysinf.obj :  $(SOURCE)  $(DEP_AASYS) $(INTDIR)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\waveio.c
DEP_WAVEI=\
	.\appport.h\
	.\waveio.h\
	.\debug.h

!IF  "$(CFG)" == "Win32 (80x86) Release"

$(INTDIR)/waveio.obj :  $(SOURCE)  $(DEP_WAVEI) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

$(INTDIR)/waveio.obj :  $(SOURCE)  $(DEP_WAVEI) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

$(INTDIR)/waveio.obj :  $(SOURCE)  $(DEP_WAVEI) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

$(INTDIR)/waveio.obj :  $(SOURCE)  $(DEP_WAVEI) $(INTDIR)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\aaprops.c
DEP_AAPRO=\
	.\muldiv32.h\
	.\appport.h\
	.\acmapp.h\
	.\debug.h

!IF  "$(CFG)" == "Win32 (80x86) Release"

$(INTDIR)/aaprops.obj :  $(SOURCE)  $(DEP_AAPRO) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

$(INTDIR)/aaprops.obj :  $(SOURCE)  $(DEP_AAPRO) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

$(INTDIR)/aaprops.obj :  $(SOURCE)  $(DEP_AAPRO) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

$(INTDIR)/aaprops.obj :  $(SOURCE)  $(DEP_AAPRO) $(INTDIR)

!ENDIF 

# End Source File
# End Group
# End Project
################################################################################
