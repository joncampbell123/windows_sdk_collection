# Microsoft Visual C++ Generated NMAKE File, Format Version 2.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101
# TARGTYPE "Win32 (MIPS) Application" 0x0501
# TARGTYPE "Macintosh Application" 0x0301

!IF "$(CFG)" == ""
CFG=Win32 (80x86) Debug
!MESSAGE No configuration specified.  Defaulting to Win32 (80x86) Debug.
!ENDIF 

!IF "$(CFG)" != "Win32 (80x86) Release" && "$(CFG)" != "Win32 (80x86) Debug" &&\
 "$(CFG)" != "Win32 (MIPS) Release" && "$(CFG)" != "Win32 (MIPS) Debug" &&\
 "$(CFG)" != "Macintosh Debug" && "$(CFG)" != "Macintosh Release"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "mdi.mak" CFG="Win32 (80x86) Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Win32 (80x86) Release" (based on "Win32 (x86) Application")
!MESSAGE "Win32 (80x86) Debug" (based on "Win32 (x86) Application")
!MESSAGE "Win32 (MIPS) Release" (based on "Win32 (MIPS) Application")
!MESSAGE "Win32 (MIPS) Debug" (based on "Win32 (MIPS) Application")
!MESSAGE "Macintosh Debug" (based on "Macintosh Application")
!MESSAGE "Macintosh Release" (based on "Macintosh Application")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

################################################################################
# Begin Project
# PROP Target_Last_Scanned "Win32 (MIPS) Release"

!IF  "$(CFG)" == "Win32 (80x86) Release"

# PROP BASE Use_MFC 1
# PROP BASE Use_Debug_Libraries 0
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "WinRel"
# PROP Intermediate_Dir "WinRel"
OUTDIR=.\WinRel
INTDIR=.\WinRel

ALL : $(OUTDIR)/mdi.exe $(OUTDIR)/mdi.bsc

$(OUTDIR) : 
    if not exist $(OUTDIR)/nul mkdir $(OUTDIR)

MTL=MkTypLib.exe
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
CPP=cl.exe
# ADD BASE CPP /nologo /MT /W3 /GX /YX /Ox /D "NDEBUG" /D "_WINDOWS" /D "_X86_" /D "_MBCS" /FR /c
# ADD CPP /nologo /MD /W3 /GX /Ox /D "NDEBUG" /D "_WINDOWS" /D "_X86_" /D "_AFXDLL" /D "_MBCS" /FR /Yu"stdafx.h" /c
CPP_PROJ=/nologo /MD /W3 /GX /Ox /D "NDEBUG" /D "_WINDOWS" /D "_X86_" /D\
 "_AFXDLL" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"mdi.pch" /Yu"stdafx.h"\
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
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /fo$(INTDIR)/"mdi.res" /d "NDEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# SUBTRACT BASE BSC32 /Iu
# ADD BSC32 /nologo
# SUBTRACT BSC32 /Iu
BSC32_FLAGS=/nologo /o$(OUTDIR)/"mdi.bsc" 
BSC32_SBRS= \
	$(INTDIR)/hello.sbr \
	$(INTDIR)/mainfrm.sbr \
	$(INTDIR)/mdi.sbr \
	$(INTDIR)/bounce.sbr \
	$(INTDIR)/stdafx.sbr

$(OUTDIR)/mdi.bsc : $(OUTDIR)  $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib olecli32.lib olesvr32.lib nafxcw.lib mfcuia32.lib mfcans32.lib ole32.lib oleaut32.lib uuid.lib /NOLOGO /SUBSYSTEM:windows /MACHINE:IX86
# ADD LINK32 /NOLOGO /SUBSYSTEM:windows /MACHINE:IX86
LINK32_FLAGS=/NOLOGO /SUBSYSTEM:windows /INCREMENTAL:no\
 /PDB:$(OUTDIR)/"mdi.pdb" /MACHINE:IX86 /OUT:$(OUTDIR)/"mdi.exe" 
DEF_FILE=
LINK32_OBJS= \
	$(INTDIR)/hello.obj \
	$(INTDIR)/mainfrm.obj \
	$(INTDIR)/mdi.obj \
	$(INTDIR)/bounce.obj \
	$(INTDIR)/mdi.res \
	$(INTDIR)/stdafx.obj

$(OUTDIR)/mdi.exe : $(OUTDIR)  $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

# PROP BASE Use_MFC 1
# PROP BASE Use_Debug_Libraries 1
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "WinDebug"
# PROP Intermediate_Dir "WinDebug"
OUTDIR=.\WinDebug
INTDIR=.\WinDebug

ALL : $(OUTDIR)/mdi.exe $(OUTDIR)/mdi.bsc

$(OUTDIR) : 
    if not exist $(OUTDIR)/nul mkdir $(OUTDIR)

MTL=MkTypLib.exe
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
CPP=cl.exe
# ADD BASE CPP /nologo /MT /W3 /GX /Zi /YX /Od /D "_DEBUG" /D "_WINDOWS" /D "_X86_" /D "_MBCS" /FR /c
# ADD CPP /nologo /MD /W3 /GX /Zi /Od /D "_DEBUG" /D "_WINDOWS" /D "_X86_" /D "_AFXDLL" /D "_MBCS" /FR /Yu"stdafx.h" /c
CPP_PROJ=/nologo /MD /W3 /GX /Zi /Od /D "_DEBUG" /D "_WINDOWS" /D "_X86_" /D\
 "_AFXDLL" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"mdi.pch" /Yu"stdafx.h"\
 /Fo$(INTDIR)/ /Fd$(OUTDIR)/"mdi.pdb" /c 
CPP_OBJS=.\WinDebug/

.c{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

RSC=rc.exe
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /fo$(INTDIR)/"mdi.res" /d "_DEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# SUBTRACT BASE BSC32 /Iu
# ADD BSC32 /nologo
# SUBTRACT BSC32 /Iu
BSC32_FLAGS=/nologo /o$(OUTDIR)/"mdi.bsc" 
BSC32_SBRS= \
	$(INTDIR)/hello.sbr \
	$(INTDIR)/mainfrm.sbr \
	$(INTDIR)/mdi.sbr \
	$(INTDIR)/bounce.sbr \
	$(INTDIR)/stdafx.sbr

$(OUTDIR)/mdi.bsc : $(OUTDIR)  $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib olecli32.lib olesvr32.lib nafxcwd.lib mfcuia32.lib mfcans32.lib ole32.lib oleaut32.lib uuid.lib /NOLOGO /SUBSYSTEM:windows /DEBUG /MACHINE:IX86
# ADD LINK32 /NOLOGO /SUBSYSTEM:windows /DEBUG /MACHINE:IX86
LINK32_FLAGS=/NOLOGO /SUBSYSTEM:windows /INCREMENTAL:yes\
 /PDB:$(OUTDIR)/"mdi.pdb" /DEBUG /MACHINE:IX86 /OUT:$(OUTDIR)/"mdi.exe" 
DEF_FILE=
LINK32_OBJS= \
	$(INTDIR)/hello.obj \
	$(INTDIR)/mainfrm.obj \
	$(INTDIR)/mdi.obj \
	$(INTDIR)/bounce.obj \
	$(INTDIR)/mdi.res \
	$(INTDIR)/stdafx.obj

$(OUTDIR)/mdi.exe : $(OUTDIR)  $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Win32__M"
# PROP BASE Intermediate_Dir "Win32__M"
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "WinRel"
# PROP Intermediate_Dir "WinRel"
OUTDIR=.\WinRel
INTDIR=.\WinRel

ALL : $(OUTDIR)/mdi.exe $(OUTDIR)/mdi.bsc

$(OUTDIR) : 
    if not exist $(OUTDIR)/nul mkdir $(OUTDIR)

MTL=MkTypLib.exe
# ADD BASE MTL /nologo /D "NDEBUG" /mips
# ADD MTL /nologo /D "NDEBUG" /mips
MTL_PROJ=/nologo /D "NDEBUG" /mips 
CPP=cl.exe
# ADD BASE CPP /nologo /MD /Gt0 /QMOb2000 /W3 /GX /YX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /c
# ADD CPP /nologo /MD /Gt0 /QMOb2000 /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /Yu"stdafx.h" /c
CPP_PROJ=/nologo /MD /Gt0 /QMOb2000 /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D\
 "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"mdi.pch"\
 /Yu"stdafx.h" /Fo$(INTDIR)/ /c 
CPP_OBJS=.\WinRel/

.c{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

RSC=rc.exe
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /fo$(INTDIR)/"mdi.res" /d "NDEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o$(OUTDIR)/"mdi.bsc" 
BSC32_SBRS= \
	$(INTDIR)/hello.sbr \
	$(INTDIR)/mainfrm.sbr \
	$(INTDIR)/mdi.sbr \
	$(INTDIR)/bounce.sbr \
	$(INTDIR)/stdafx.sbr

$(OUTDIR)/mdi.bsc : $(OUTDIR)  $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 /NOLOGO /SUBSYSTEM:windows /MACHINE:MIPS
# ADD LINK32 /NOLOGO /SUBSYSTEM:windows /MACHINE:MIPS
LINK32_FLAGS=/NOLOGO /SUBSYSTEM:windows /PDB:$(OUTDIR)/"mdi.pdb" /MACHINE:MIPS\
 /OUT:$(OUTDIR)/"mdi.exe" 
DEF_FILE=
LINK32_OBJS= \
	$(INTDIR)/hello.obj \
	$(INTDIR)/mainfrm.obj \
	$(INTDIR)/mdi.obj \
	$(INTDIR)/bounce.obj \
	$(INTDIR)/mdi.res \
	$(INTDIR)/stdafx.obj

$(OUTDIR)/mdi.exe : $(OUTDIR)  $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Win32__0"
# PROP BASE Intermediate_Dir "Win32__0"
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "WinDebug"
# PROP Intermediate_Dir "WinDebug"
OUTDIR=.\WinDebug
INTDIR=.\WinDebug

ALL : $(OUTDIR)/mdi.exe $(OUTDIR)/mdi.bsc

$(OUTDIR) : 
    if not exist $(OUTDIR)/nul mkdir $(OUTDIR)

MTL=MkTypLib.exe
# ADD BASE MTL /nologo /D "_DEBUG" /mips
# ADD MTL /nologo /D "_DEBUG" /mips
MTL_PROJ=/nologo /D "_DEBUG" /mips 
CPP=cl.exe
# ADD BASE CPP /nologo /MD /Gt0 /QMOb2000 /W3 /GX /Zi /YX /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /c
# ADD CPP /nologo /MD /Gt0 /QMOb2000 /W3 /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /Yu"stdafx.h" /c
CPP_PROJ=/nologo /MD /Gt0 /QMOb2000 /W3 /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D\
 "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"mdi.pch"\
 /Yu"stdafx.h" /Fo$(INTDIR)/ /Fd$(OUTDIR)/"mdi.pdb" /c 
CPP_OBJS=.\WinDebug/

.c{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

RSC=rc.exe
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /fo$(INTDIR)/"mdi.res" /d "_DEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o$(OUTDIR)/"mdi.bsc" 
BSC32_SBRS= \
	$(INTDIR)/hello.sbr \
	$(INTDIR)/mainfrm.sbr \
	$(INTDIR)/mdi.sbr \
	$(INTDIR)/bounce.sbr \
	$(INTDIR)/stdafx.sbr

$(OUTDIR)/mdi.bsc : $(OUTDIR)  $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 /NOLOGO /SUBSYSTEM:windows /DEBUG /MACHINE:MIPS
# ADD LINK32 /NOLOGO /SUBSYSTEM:windows /DEBUG /MACHINE:MIPS
LINK32_FLAGS=/NOLOGO /SUBSYSTEM:windows /PDB:$(OUTDIR)/"mdi.pdb" /DEBUG\
 /MACHINE:MIPS /OUT:$(OUTDIR)/"mdi.exe" 
DEF_FILE=
LINK32_OBJS= \
	$(INTDIR)/hello.obj \
	$(INTDIR)/mainfrm.obj \
	$(INTDIR)/mdi.obj \
	$(INTDIR)/bounce.obj \
	$(INTDIR)/mdi.res \
	$(INTDIR)/stdafx.obj

$(OUTDIR)/mdi.exe : $(OUTDIR)  $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "Macintosh Debug"

# PROP BASE Use_MFC 1
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Macintos"
# PROP BASE Intermediate_Dir "Macintos"
# PROP Use_MFC 1
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "MacDebug"
# PROP Intermediate_Dir "MacDebug"
OUTDIR=.\MacDebug
INTDIR=.\MacDebug

ALL : $(OUTDIR)/mdi.exe $(OUTDIR)/mdi.bsc

$(OUTDIR) : 
    if not exist $(OUTDIR)/nul mkdir $(OUTDIR)

MTL=MkTypLib.exe
# ADD BASE MTL /nologo /D "_DEBUG" /mac
# ADD MTL /nologo /D "_DEBUG" /mac
MTL_PROJ=/nologo /D "_DEBUG" /mac 
CPP=cl.exe
# ADD BASE CPP /nologo /AL /Q68s /Q68m /W3 /GX /Zi /YX /Od /D "_WINDOWS" /D "_MAC" /D "_68K_" /D "_DEBUG" /D "_MBCS" /FR /c
# ADD CPP /nologo /AL /Q68s /Q68m /W3 /GX /Zi /Od /D "_WINDOWS" /D "_MAC" /D "_68K_" /D "_DEBUG" /D "_MBCS" /FR /Yu"stdafx.h" /c
CPP_PROJ=/nologo /AL /Q68s /Q68m /W3 /GX /Zi /Od /D "_WINDOWS" /D "_MAC" /D\
 "_68K_" /D "_DEBUG" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"mdi.pch"\
 /Yu"stdafx.h" /Fo$(INTDIR)/ /Fd$(OUTDIR)/"mdi.pdb" /c 
CPP_OBJS=.\MacDebug/

.c{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

RSC=rc.exe
# ADD BASE RSC /r /d "_MAC" /d "_68K_" /d "_DEBUG"
# ADD RSC /r /d "_MAC" /d "_68K_" /d "_DEBUG"
RSC_PROJ=/r /fo$(INTDIR)/"mdi.rsc" /d "_MAC" /d "_68K_" /d "_DEBUG" 
MRC=mrc.exe
# ADD BASE MRC /D "_MAC" /D "_DEBUG"
# ADD MRC /D "_MAC" /D "_DEBUG"
MRC_PROJ=/D "_MAC" /D "_DEBUG" 
MFILE32=mfile.exe
# ADD BASE MFILE32 COPY /NOLOGO
# ADD MFILE32 COPY /NOLOGO
MFILE32_FLAGS=COPY /NOLOGO 
MFILE32_FILES= \
	$(OUTDIR)/mdi.exe

DOWNLOAD :  $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) $(MFILE32_FILES) ":$(MFILE32_DEST)"


BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o$(OUTDIR)/"mdi.bsc" 
BSC32_SBRS= \
	$(INTDIR)/hello.sbr \
	$(INTDIR)/mainfrm.sbr \
	$(INTDIR)/mdi.sbr \
	$(INTDIR)/bounce.sbr \
	$(INTDIR)/stdafx.sbr

$(OUTDIR)/mdi.bsc : $(OUTDIR)  $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 wlm.rsc commdlg.rsc /NOLOGO /MAC:bundle /DEBUG /MACHINE:M68K
# ADD LINK32 wlm.rsc commdlg.rsc wlmd.lib nafxcmd.lib /NOLOGO /MAC:bundle /MAC:creator="MDI " /DEBUG /MACHINE:M68K
LINK32_FLAGS=wlm.rsc commdlg.rsc wlmd.lib nafxcmd.lib /NOLOGO /MAC:bundle\
 /MAC:type="APPL" /MAC:creator="MDI " /PDB:$(OUTDIR)/"mdi.pdb" /DEBUG\
 /MACHINE:M68K /OUT:$(OUTDIR)/"mdi.exe" 
DEF_FILE=
LINK32_OBJS= \
	$(INTDIR)/hello.obj \
	$(INTDIR)/mainfrm.obj \
	$(INTDIR)/mdi.obj \
	$(INTDIR)/bounce.obj \
	$(INTDIR)/mdi.rsc \
	$(INTDIR)/mdimac.rsc \
	$(INTDIR)/stdafx.obj

$(OUTDIR)/mdi.exe : $(OUTDIR)  $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "Macintosh Release"

# PROP BASE Use_MFC 1
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Macinto0"
# PROP BASE Intermediate_Dir "Macinto0"
# PROP Use_MFC 1
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "MacRel"
# PROP Intermediate_Dir "MacRel"
OUTDIR=.\MacRel
INTDIR=.\MacRel

ALL : $(OUTDIR)/mdi.exe $(OUTDIR)/mdi.bsc

$(OUTDIR) : 
    if not exist $(OUTDIR)/nul mkdir $(OUTDIR)

MTL=MkTypLib.exe
# ADD BASE MTL /nologo /D "NDEBUG" /mac
# ADD MTL /nologo /D "NDEBUG" /mac
MTL_PROJ=/nologo /D "NDEBUG" /mac 
CPP=cl.exe
# ADD BASE CPP /nologo /AL /Q68s /W3 /GX /YX /O2 /D "_WINDOWS" /D "_MAC" /D "_68K_" /D "NDEBUG" /D "_MBCS" /FR /c
# ADD CPP /nologo /AL /Q68s /W3 /GX /O2 /D "_WINDOWS" /D "_MAC" /D "_68K_" /D "NDEBUG" /D "_MBCS" /FR /Yu"stdafx.h" /c
CPP_PROJ=/nologo /AL /Q68s /W3 /GX /O2 /D "_WINDOWS" /D "_MAC" /D "_68K_" /D\
 "NDEBUG" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"mdi.pch" /Yu"stdafx.h"\
 /Fo$(INTDIR)/ /c 
CPP_OBJS=.\MacRel/

.c{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

RSC=rc.exe
# ADD BASE RSC /r /d "_MAC" /d "_68K_" /d "NDEBUG"
# ADD RSC /r /d "_MAC" /d "_68K_" /d "NDEBUG"
RSC_PROJ=/r /fo$(INTDIR)/"mdi.rsc" /d "_MAC" /d "_68K_" /d "NDEBUG" 
MRC=mrc.exe
# ADD BASE MRC /D "_MAC" /D "NDEBUG"
# ADD MRC /D "_MAC" /D "NDEBUG"
MRC_PROJ=/D "_MAC" /D "NDEBUG" 
MFILE32=mfile.exe
# ADD BASE MFILE32 COPY /NOLOGO
# ADD MFILE32 COPY /NOLOGO
MFILE32_FLAGS=COPY /NOLOGO 
MFILE32_FILES= \
	$(OUTDIR)/mdi.exe

DOWNLOAD :  $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) $(MFILE32_FILES) ":$(MFILE32_DEST)"


BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o$(OUTDIR)/"mdi.bsc" 
BSC32_SBRS= \
	$(INTDIR)/hello.sbr \
	$(INTDIR)/mainfrm.sbr \
	$(INTDIR)/mdi.sbr \
	$(INTDIR)/bounce.sbr \
	$(INTDIR)/stdafx.sbr

$(OUTDIR)/mdi.bsc : $(OUTDIR)  $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 wlm.rsc commdlg.rsc /NOLOGO /MAC:bundle /MACHINE:M68K
# ADD LINK32 wlm.rsc commdlg.rsc wlm.lib nafxcm.lib /NOLOGO /MAC:bundle /MAC:creator="MDI " /MACHINE:M68K
LINK32_FLAGS=wlm.rsc commdlg.rsc wlm.lib nafxcm.lib /NOLOGO /MAC:bundle\
 /MAC:type="APPL" /MAC:creator="MDI " /PDB:$(OUTDIR)/"mdi.pdb" /MACHINE:M68K\
 /OUT:$(OUTDIR)/"mdi.exe" 
DEF_FILE=
LINK32_OBJS= \
	$(INTDIR)/hello.obj \
	$(INTDIR)/mainfrm.obj \
	$(INTDIR)/mdi.obj \
	$(INTDIR)/bounce.obj \
	$(INTDIR)/mdi.rsc \
	$(INTDIR)/mdimac.rsc \
	$(INTDIR)/stdafx.obj

$(OUTDIR)/mdi.exe : $(OUTDIR)  $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 

################################################################################
# Begin Group "Source Files"

################################################################################
# Begin Source File

SOURCE=.\hello.cpp
DEP_HELLO=\
	.\stdafx.h\
	.\hello.h\
	.\mdi.h

!IF  "$(CFG)" == "Win32 (80x86) Release"

# ADD BASE CPP /Yu"STDAFX.H"
# ADD CPP /Yu"stdafx.h"

$(INTDIR)/hello.obj :  $(SOURCE)  $(DEP_HELLO) $(INTDIR) $(INTDIR)/stdafx.obj
   $(CPP) /nologo /MD /W3 /GX /Ox /D "NDEBUG" /D "_WINDOWS" /D "_X86_" /D\
 "_AFXDLL" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"mdi.pch" /Yu"stdafx.h"\
 /Fo$(INTDIR)/ /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

# ADD BASE CPP /Yu"STDAFX.H"
# ADD CPP /Yu"stdafx.h"

$(INTDIR)/hello.obj :  $(SOURCE)  $(DEP_HELLO) $(INTDIR) $(INTDIR)/stdafx.obj
   $(CPP) /nologo /MD /W3 /GX /Zi /Od /D "_DEBUG" /D "_WINDOWS" /D "_X86_" /D\
 "_AFXDLL" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"mdi.pch" /Yu"stdafx.h"\
 /Fo$(INTDIR)/ /Fd$(OUTDIR)/"mdi.pdb" /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

# ADD BASE CPP /Gt0 /QMOb2000
# ADD CPP /Gt0 /QMOb2000 /Yu"stdafx.h"

$(INTDIR)/hello.obj :  $(SOURCE)  $(DEP_HELLO) $(INTDIR) $(INTDIR)/stdafx.obj
   $(CPP) /nologo /MD /Gt0 /QMOb2000 /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D\
 "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"mdi.pch"\
 /Yu"stdafx.h" /Fo$(INTDIR)/ /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

# ADD BASE CPP /Gt0 /QMOb2000
# ADD CPP /Gt0 /QMOb2000 /Yu"stdafx.h"

$(INTDIR)/hello.obj :  $(SOURCE)  $(DEP_HELLO) $(INTDIR) $(INTDIR)/stdafx.obj
   $(CPP) /nologo /MD /Gt0 /QMOb2000 /W3 /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D\
 "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"mdi.pch"\
 /Yu"stdafx.h" /Fo$(INTDIR)/ /Fd$(OUTDIR)/"mdi.pdb" /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Macintosh Debug"

# ADD CPP /Yu"stdafx.h"

$(INTDIR)/hello.obj :  $(SOURCE)  $(DEP_HELLO) $(INTDIR) $(INTDIR)/stdafx.obj
   $(CPP) /nologo /AL /Q68s /Q68m /W3 /GX /Zi /Od /D "_WINDOWS" /D "_MAC" /D\
 "_68K_" /D "_DEBUG" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"mdi.pch"\
 /Yu"stdafx.h" /Fo$(INTDIR)/ /Fd$(OUTDIR)/"mdi.pdb" /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Macintosh Release"

# ADD CPP /Yu"stdafx.h"

$(INTDIR)/hello.obj :  $(SOURCE)  $(DEP_HELLO) $(INTDIR) $(INTDIR)/stdafx.obj
   $(CPP) /nologo /AL /Q68s /W3 /GX /O2 /D "_WINDOWS" /D "_MAC" /D "_68K_" /D\
 "NDEBUG" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"mdi.pch" /Yu"stdafx.h"\
 /Fo$(INTDIR)/ /c  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\mainfrm.cpp
DEP_MAINF=\
	.\stdafx.h\
	.\bounce.h\
	.\hello.h\
	.\mdi.h\
	.\mainfrm.h

!IF  "$(CFG)" == "Win32 (80x86) Release"

# ADD BASE CPP /Yu"STDAFX.H"
# ADD CPP /Yu"stdafx.h"

$(INTDIR)/mainfrm.obj :  $(SOURCE)  $(DEP_MAINF) $(INTDIR) $(INTDIR)/stdafx.obj
   $(CPP) /nologo /MD /W3 /GX /Ox /D "NDEBUG" /D "_WINDOWS" /D "_X86_" /D\
 "_AFXDLL" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"mdi.pch" /Yu"stdafx.h"\
 /Fo$(INTDIR)/ /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

# ADD BASE CPP /Yu"STDAFX.H"
# ADD CPP /Yu"stdafx.h"

$(INTDIR)/mainfrm.obj :  $(SOURCE)  $(DEP_MAINF) $(INTDIR) $(INTDIR)/stdafx.obj
   $(CPP) /nologo /MD /W3 /GX /Zi /Od /D "_DEBUG" /D "_WINDOWS" /D "_X86_" /D\
 "_AFXDLL" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"mdi.pch" /Yu"stdafx.h"\
 /Fo$(INTDIR)/ /Fd$(OUTDIR)/"mdi.pdb" /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

# ADD BASE CPP /Gt0 /QMOb2000
# ADD CPP /Gt0 /QMOb2000 /Yu"stdafx.h"

$(INTDIR)/mainfrm.obj :  $(SOURCE)  $(DEP_MAINF) $(INTDIR) $(INTDIR)/stdafx.obj
   $(CPP) /nologo /MD /Gt0 /QMOb2000 /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D\
 "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"mdi.pch"\
 /Yu"stdafx.h" /Fo$(INTDIR)/ /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

# ADD BASE CPP /Gt0 /QMOb2000
# ADD CPP /Gt0 /QMOb2000 /Yu"stdafx.h"

$(INTDIR)/mainfrm.obj :  $(SOURCE)  $(DEP_MAINF) $(INTDIR) $(INTDIR)/stdafx.obj
   $(CPP) /nologo /MD /Gt0 /QMOb2000 /W3 /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D\
 "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"mdi.pch"\
 /Yu"stdafx.h" /Fo$(INTDIR)/ /Fd$(OUTDIR)/"mdi.pdb" /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Macintosh Debug"

# ADD CPP /Yu"stdafx.h"

$(INTDIR)/mainfrm.obj :  $(SOURCE)  $(DEP_MAINF) $(INTDIR) $(INTDIR)/stdafx.obj
   $(CPP) /nologo /AL /Q68s /Q68m /W3 /GX /Zi /Od /D "_WINDOWS" /D "_MAC" /D\
 "_68K_" /D "_DEBUG" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"mdi.pch"\
 /Yu"stdafx.h" /Fo$(INTDIR)/ /Fd$(OUTDIR)/"mdi.pdb" /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Macintosh Release"

# ADD CPP /Yu"stdafx.h"

$(INTDIR)/mainfrm.obj :  $(SOURCE)  $(DEP_MAINF) $(INTDIR) $(INTDIR)/stdafx.obj
   $(CPP) /nologo /AL /Q68s /W3 /GX /O2 /D "_WINDOWS" /D "_MAC" /D "_68K_" /D\
 "NDEBUG" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"mdi.pch" /Yu"stdafx.h"\
 /Fo$(INTDIR)/ /c  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\mdi.cpp
DEP_MDI_C=\
	.\stdafx.h\
	.\mdi.h\
	.\mainfrm.h

!IF  "$(CFG)" == "Win32 (80x86) Release"

# ADD BASE CPP /Yu"STDAFX.H"
# ADD CPP /Yu"stdafx.h"

$(INTDIR)/mdi.obj :  $(SOURCE)  $(DEP_MDI_C) $(INTDIR) $(INTDIR)/stdafx.obj
   $(CPP) /nologo /MD /W3 /GX /Ox /D "NDEBUG" /D "_WINDOWS" /D "_X86_" /D\
 "_AFXDLL" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"mdi.pch" /Yu"stdafx.h"\
 /Fo$(INTDIR)/ /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

# ADD BASE CPP /Yu"STDAFX.H"
# ADD CPP /Yu"stdafx.h"

$(INTDIR)/mdi.obj :  $(SOURCE)  $(DEP_MDI_C) $(INTDIR) $(INTDIR)/stdafx.obj
   $(CPP) /nologo /MD /W3 /GX /Zi /Od /D "_DEBUG" /D "_WINDOWS" /D "_X86_" /D\
 "_AFXDLL" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"mdi.pch" /Yu"stdafx.h"\
 /Fo$(INTDIR)/ /Fd$(OUTDIR)/"mdi.pdb" /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

# ADD BASE CPP /Gt0 /QMOb2000
# ADD CPP /Gt0 /QMOb2000 /Yu"stdafx.h"

$(INTDIR)/mdi.obj :  $(SOURCE)  $(DEP_MDI_C) $(INTDIR) $(INTDIR)/stdafx.obj
   $(CPP) /nologo /MD /Gt0 /QMOb2000 /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D\
 "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"mdi.pch"\
 /Yu"stdafx.h" /Fo$(INTDIR)/ /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

# ADD BASE CPP /Gt0 /QMOb2000
# ADD CPP /Gt0 /QMOb2000 /Yu"stdafx.h"

$(INTDIR)/mdi.obj :  $(SOURCE)  $(DEP_MDI_C) $(INTDIR) $(INTDIR)/stdafx.obj
   $(CPP) /nologo /MD /Gt0 /QMOb2000 /W3 /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D\
 "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"mdi.pch"\
 /Yu"stdafx.h" /Fo$(INTDIR)/ /Fd$(OUTDIR)/"mdi.pdb" /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Macintosh Debug"

# ADD CPP /Yu"stdafx.h"

$(INTDIR)/mdi.obj :  $(SOURCE)  $(DEP_MDI_C) $(INTDIR) $(INTDIR)/stdafx.obj
   $(CPP) /nologo /AL /Q68s /Q68m /W3 /GX /Zi /Od /D "_WINDOWS" /D "_MAC" /D\
 "_68K_" /D "_DEBUG" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"mdi.pch"\
 /Yu"stdafx.h" /Fo$(INTDIR)/ /Fd$(OUTDIR)/"mdi.pdb" /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Macintosh Release"

# ADD CPP /Yu"stdafx.h"

$(INTDIR)/mdi.obj :  $(SOURCE)  $(DEP_MDI_C) $(INTDIR) $(INTDIR)/stdafx.obj
   $(CPP) /nologo /AL /Q68s /W3 /GX /O2 /D "_WINDOWS" /D "_MAC" /D "_68K_" /D\
 "NDEBUG" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"mdi.pch" /Yu"stdafx.h"\
 /Fo$(INTDIR)/ /c  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\bounce.cpp
DEP_BOUNC=\
	.\stdafx.h\
	.\mdi.h\
	.\bounce.h

!IF  "$(CFG)" == "Win32 (80x86) Release"

# ADD BASE CPP /Yu"STDAFX.H"
# ADD CPP /Yu"stdafx.h"

$(INTDIR)/bounce.obj :  $(SOURCE)  $(DEP_BOUNC) $(INTDIR) $(INTDIR)/stdafx.obj
   $(CPP) /nologo /MD /W3 /GX /Ox /D "NDEBUG" /D "_WINDOWS" /D "_X86_" /D\
 "_AFXDLL" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"mdi.pch" /Yu"stdafx.h"\
 /Fo$(INTDIR)/ /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

# ADD BASE CPP /Yu"STDAFX.H"
# ADD CPP /Yu"stdafx.h"

$(INTDIR)/bounce.obj :  $(SOURCE)  $(DEP_BOUNC) $(INTDIR) $(INTDIR)/stdafx.obj
   $(CPP) /nologo /MD /W3 /GX /Zi /Od /D "_DEBUG" /D "_WINDOWS" /D "_X86_" /D\
 "_AFXDLL" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"mdi.pch" /Yu"stdafx.h"\
 /Fo$(INTDIR)/ /Fd$(OUTDIR)/"mdi.pdb" /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

# ADD BASE CPP /Gt0 /QMOb2000
# ADD CPP /Gt0 /QMOb2000 /Yu"stdafx.h"

$(INTDIR)/bounce.obj :  $(SOURCE)  $(DEP_BOUNC) $(INTDIR) $(INTDIR)/stdafx.obj
   $(CPP) /nologo /MD /Gt0 /QMOb2000 /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D\
 "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"mdi.pch"\
 /Yu"stdafx.h" /Fo$(INTDIR)/ /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

# ADD BASE CPP /Gt0 /QMOb2000
# ADD CPP /Gt0 /QMOb2000 /Yu"stdafx.h"

$(INTDIR)/bounce.obj :  $(SOURCE)  $(DEP_BOUNC) $(INTDIR) $(INTDIR)/stdafx.obj
   $(CPP) /nologo /MD /Gt0 /QMOb2000 /W3 /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D\
 "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"mdi.pch"\
 /Yu"stdafx.h" /Fo$(INTDIR)/ /Fd$(OUTDIR)/"mdi.pdb" /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Macintosh Debug"

# ADD CPP /Yu"stdafx.h"

$(INTDIR)/bounce.obj :  $(SOURCE)  $(DEP_BOUNC) $(INTDIR) $(INTDIR)/stdafx.obj
   $(CPP) /nologo /AL /Q68s /Q68m /W3 /GX /Zi /Od /D "_WINDOWS" /D "_MAC" /D\
 "_68K_" /D "_DEBUG" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"mdi.pch"\
 /Yu"stdafx.h" /Fo$(INTDIR)/ /Fd$(OUTDIR)/"mdi.pdb" /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Macintosh Release"

# ADD CPP /Yu"stdafx.h"

$(INTDIR)/bounce.obj :  $(SOURCE)  $(DEP_BOUNC) $(INTDIR) $(INTDIR)/stdafx.obj
   $(CPP) /nologo /AL /Q68s /W3 /GX /O2 /D "_WINDOWS" /D "_MAC" /D "_68K_" /D\
 "NDEBUG" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"mdi.pch" /Yu"stdafx.h"\
 /Fo$(INTDIR)/ /c  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\mdi.rc
DEP_MDI_R=\
	.\mdi.ico\
	.\hello.ico

!IF  "$(CFG)" == "Win32 (80x86) Release"

$(INTDIR)/mdi.res :  $(SOURCE)  $(DEP_MDI_R) $(INTDIR)
   $(RSC) $(RSC_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

$(INTDIR)/mdi.res :  $(SOURCE)  $(DEP_MDI_R) $(INTDIR)
   $(RSC) $(RSC_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

$(INTDIR)/mdi.res :  $(SOURCE)  $(DEP_MDI_R) $(INTDIR)
   $(RSC) $(RSC_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

$(INTDIR)/mdi.res :  $(SOURCE)  $(DEP_MDI_R) $(INTDIR)
   $(RSC) $(RSC_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Macintosh Debug"

$(INTDIR)/mdi.rsc :  $(SOURCE)  $(DEP_MDI_R) $(INTDIR)
   $(RSC) $(RSC_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Macintosh Release"

$(INTDIR)/mdi.rsc :  $(SOURCE)  $(DEP_MDI_R) $(INTDIR)
   $(RSC) $(RSC_PROJ)  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\mdimac.r

!IF  "$(CFG)" == "Win32 (80x86) Release"

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

!ELSEIF  "$(CFG)" == "Macintosh Debug"

$(INTDIR)/mdimac.rsc :  $(SOURCE)  $(INTDIR)
   $(MRC) /o$(INTDIR)/"mdimac.rsc" /D "_MAC" /D "_DEBUG"  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Macintosh Release"

$(INTDIR)/mdimac.rsc :  $(SOURCE)  $(INTDIR)
   $(MRC) /o$(INTDIR)/"mdimac.rsc" /D "_MAC" /D "NDEBUG"  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\stdafx.cpp
DEP_STDAF=\
	.\stdafx.h

!IF  "$(CFG)" == "Win32 (80x86) Release"

# ADD CPP /Yc"stdafx.h"

$(INTDIR)/stdafx.obj :  $(SOURCE)  $(DEP_STDAF) $(INTDIR)
   $(CPP) /nologo /MD /W3 /GX /Ox /D "NDEBUG" /D "_WINDOWS" /D "_X86_" /D\
 "_AFXDLL" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"mdi.pch" /Yc"stdafx.h"\
 /Fo$(INTDIR)/ /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

# ADD CPP /Yc"stdafx.h"

$(INTDIR)/stdafx.obj :  $(SOURCE)  $(DEP_STDAF) $(INTDIR)
   $(CPP) /nologo /MD /W3 /GX /Zi /Od /D "_DEBUG" /D "_WINDOWS" /D "_X86_" /D\
 "_AFXDLL" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"mdi.pch" /Yc"stdafx.h"\
 /Fo$(INTDIR)/ /Fd$(OUTDIR)/"mdi.pdb" /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

# ADD BASE CPP /Gt0 /QMOb2000
# ADD CPP /Gt0 /QMOb2000 /Yc"stdafx.h"

$(INTDIR)/stdafx.obj :  $(SOURCE)  $(DEP_STDAF) $(INTDIR)
   $(CPP) /nologo /MD /Gt0 /QMOb2000 /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D\
 "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"mdi.pch"\
 /Yc"stdafx.h" /Fo$(INTDIR)/ /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

# ADD BASE CPP /Gt0 /QMOb2000
# ADD CPP /Gt0 /QMOb2000 /Yc"stdafx.h"

$(INTDIR)/stdafx.obj :  $(SOURCE)  $(DEP_STDAF) $(INTDIR)
   $(CPP) /nologo /MD /Gt0 /QMOb2000 /W3 /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D\
 "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"mdi.pch"\
 /Yc"stdafx.h" /Fo$(INTDIR)/ /Fd$(OUTDIR)/"mdi.pdb" /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Macintosh Debug"

# ADD CPP /Yc"stdafx.h"

$(INTDIR)/stdafx.obj :  $(SOURCE)  $(DEP_STDAF) $(INTDIR)
   $(CPP) /nologo /AL /Q68s /Q68m /W3 /GX /Zi /Od /D "_WINDOWS" /D "_MAC" /D\
 "_68K_" /D "_DEBUG" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"mdi.pch"\
 /Yc"stdafx.h" /Fo$(INTDIR)/ /Fd$(OUTDIR)/"mdi.pdb" /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Macintosh Release"

# ADD CPP /Yc"stdafx.h"

$(INTDIR)/stdafx.obj :  $(SOURCE)  $(DEP_STDAF) $(INTDIR)
   $(CPP) /nologo /AL /Q68s /W3 /GX /O2 /D "_WINDOWS" /D "_MAC" /D "_68K_" /D\
 "NDEBUG" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"mdi.pch" /Yc"stdafx.h"\
 /Fo$(INTDIR)/ /c  $(SOURCE) 

!ENDIF 

# End Source File
# End Group
# End Project
################################################################################
