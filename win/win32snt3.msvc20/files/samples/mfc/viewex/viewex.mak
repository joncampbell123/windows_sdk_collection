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
 "$(CFG)" != "Macintosh Debug" && "$(CFG)" != "Macintosh Release" && "$(CFG)" !=\
 "Win32 (MIPS) Release" && "$(CFG)" != "Win32 (MIPS) Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "viewex.mak" CFG="Win32 (80x86) Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Win32 (80x86) Release" (based on "Win32 (x86) Application")
!MESSAGE "Win32 (80x86) Debug" (based on "Win32 (x86) Application")
!MESSAGE "Macintosh Debug" (based on "Macintosh Application")
!MESSAGE "Macintosh Release" (based on "Macintosh Application")
!MESSAGE "Win32 (MIPS) Release" (based on "Win32 (MIPS) Application")
!MESSAGE "Win32 (MIPS) Debug" (based on "Win32 (MIPS) Application")
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

ALL : $(OUTDIR)/viewex.exe $(OUTDIR)/viewex.bsc

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
 "_AFXDLL" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"viewex.pch" /Yu"stdafx.h"\
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
RSC_PROJ=/l 0x409 /fo$(INTDIR)/"viewex.res" /d "NDEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# SUBTRACT BASE BSC32 /Iu
# ADD BSC32 /nologo
# SUBTRACT BSC32 /Iu
BSC32_FLAGS=/nologo /o$(OUTDIR)/"viewex.bsc" 
BSC32_SBRS= \
	$(INTDIR)/stdafx.sbr \
	$(INTDIR)/inputvw.sbr \
	$(INTDIR)/maindoc.sbr \
	$(INTDIR)/simpvw.sbr \
	$(INTDIR)/splitter.sbr \
	$(INTDIR)/enterdlg.sbr \
	$(INTDIR)/viewex.sbr

$(OUTDIR)/viewex.bsc : $(OUTDIR)  $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib olecli32.lib olesvr32.lib nafxcw.lib mfcuia32.lib mfcans32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib /NOLOGO /SUBSYSTEM:windows /MACHINE:IX86
# ADD LINK32 /NOLOGO /SUBSYSTEM:windows /MACHINE:IX86
LINK32_FLAGS=/NOLOGO /SUBSYSTEM:windows /INCREMENTAL:no\
 /PDB:$(OUTDIR)/"viewex.pdb" /MACHINE:IX86 /OUT:$(OUTDIR)/"viewex.exe" 
DEF_FILE=
LINK32_OBJS= \
	$(INTDIR)/stdafx.obj \
	$(INTDIR)/inputvw.obj \
	$(INTDIR)/maindoc.obj \
	$(INTDIR)/simpvw.obj \
	$(INTDIR)/splitter.obj \
	$(INTDIR)/enterdlg.obj \
	$(INTDIR)/viewex.obj \
	$(INTDIR)/viewex.res

$(OUTDIR)/viewex.exe : $(OUTDIR)  $(DEF_FILE) $(LINK32_OBJS)
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

ALL : $(OUTDIR)/viewex.exe $(OUTDIR)/viewex.bsc

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
 "_AFXDLL" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"viewex.pch" /Yu"stdafx.h"\
 /Fo$(INTDIR)/ /Fd$(OUTDIR)/"viewex.pdb" /c 
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
RSC_PROJ=/l 0x409 /fo$(INTDIR)/"viewex.res" /d "_DEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# SUBTRACT BASE BSC32 /Iu
# ADD BSC32 /nologo
# SUBTRACT BSC32 /Iu
BSC32_FLAGS=/nologo /o$(OUTDIR)/"viewex.bsc" 
BSC32_SBRS= \
	$(INTDIR)/stdafx.sbr \
	$(INTDIR)/inputvw.sbr \
	$(INTDIR)/maindoc.sbr \
	$(INTDIR)/simpvw.sbr \
	$(INTDIR)/splitter.sbr \
	$(INTDIR)/enterdlg.sbr \
	$(INTDIR)/viewex.sbr

$(OUTDIR)/viewex.bsc : $(OUTDIR)  $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib olecli32.lib olesvr32.lib nafxcwd.lib mfcuia32.lib mfcans32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib /NOLOGO /SUBSYSTEM:windows /DEBUG /MACHINE:IX86
# ADD LINK32 /NOLOGO /SUBSYSTEM:windows /DEBUG /MACHINE:IX86
LINK32_FLAGS=/NOLOGO /SUBSYSTEM:windows /INCREMENTAL:yes\
 /PDB:$(OUTDIR)/"viewex.pdb" /DEBUG /MACHINE:IX86 /OUT:$(OUTDIR)/"viewex.exe" 
DEF_FILE=
LINK32_OBJS= \
	$(INTDIR)/stdafx.obj \
	$(INTDIR)/inputvw.obj \
	$(INTDIR)/maindoc.obj \
	$(INTDIR)/simpvw.obj \
	$(INTDIR)/splitter.obj \
	$(INTDIR)/enterdlg.obj \
	$(INTDIR)/viewex.obj \
	$(INTDIR)/viewex.res

$(OUTDIR)/viewex.exe : $(OUTDIR)  $(DEF_FILE) $(LINK32_OBJS)
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

ALL : $(OUTDIR)/viewex.exe $(OUTDIR)/viewex.bsc

$(OUTDIR) : 
    if not exist $(OUTDIR)/nul mkdir $(OUTDIR)

MTL=MkTypLib.exe
# ADD BASE MTL /nologo /D "_DEBUG" /mac
# ADD MTL /nologo /D "_DEBUG" /mac
MTL_PROJ=/nologo /D "_DEBUG" /mac 
CPP=cl.exe
# ADD BASE CPP /nologo /AL /Gt1 /Q68s /Q68m /W3 /GX /Zi /YX /Od /D "_WINDOWS" /D "_MAC" /D "_68K_" /D "_DEBUG" /D "_MBCS" /FR /c
# ADD CPP /nologo /AL /Gt1 /Q68s /Q68m /W3 /GX /Zi /Od /D "_WINDOWS" /D "_MAC" /D "_68K_" /D "_DEBUG" /FR /Yu"stdafx.h" /c
CPP_PROJ=/nologo /AL /Gt1 /Q68s /Q68m /W3 /GX /Zi /Od /D "_WINDOWS" /D "_MAC"\
 /D "_68K_" /D "_DEBUG" /FR$(INTDIR)/ /Fp$(OUTDIR)/"viewex.pch" /Yu"stdafx.h"\
 /Fo$(INTDIR)/ /Fd$(OUTDIR)/"viewex.pdb" /c 
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
RSC_PROJ=/r /fo$(INTDIR)/"viewex.rsc" /d "_MAC" /d "_68K_" /d "_DEBUG" 
MRC=mrc.exe
# ADD BASE MRC /D "_MAC" /D "_68K_" /D "_DEBUG" /r
# ADD MRC /D "_MAC" /D "_68K_" /D "_DEBUG"
MRC_PROJ=/D "_MAC" /D "_68K_" /D "_DEBUG" 
MFILE32=mfile.exe
# ADD BASE MFILE32 COPY /NOLOGO
# ADD MFILE32 COPY /NOLOGO
MFILE32_FLAGS=COPY /NOLOGO 
MFILE32_FILES= \
	$(OUTDIR)/viewex.exe

DOWNLOAD :  $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) $(MFILE32_FILES) ":$(MFILE32_DEST)"


BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# SUBTRACT BASE BSC32 /Iu
# ADD BSC32 /nologo
# SUBTRACT BSC32 /Iu
BSC32_FLAGS=/nologo /o$(OUTDIR)/"viewex.bsc" 
BSC32_SBRS= \
	$(INTDIR)/stdafx.sbr \
	$(INTDIR)/inputvw.sbr \
	$(INTDIR)/maindoc.sbr \
	$(INTDIR)/simpvw.sbr \
	$(INTDIR)/splitter.sbr \
	$(INTDIR)/enterdlg.sbr \
	$(INTDIR)/viewex.sbr

$(OUTDIR)/viewex.bsc : $(OUTDIR)  $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 swapd.lib sanes.lib wlmd.lib aslmd.lib wlm.rsc commdlg.rsc nafxcmd.lib mfcuia32.lib mfcans32.lib ole32.lib oleaut32.lib uuid.lib /NOLOGO /MAC:bundle /DEBUG /MACHINE:M68K /NODEFAULTLIB:"swap.lib"
# ADD LINK32 swapd.lib sanes.lib wlmd.lib aslmd.lib wlm.rsc commdlg.rsc nafxcmd.lib /NOLOGO /MAC:bundle /MAC:creator="VIEW" /DEBUG /MACHINE:M68K /NODEFAULTLIB:"swap.lib"
LINK32_FLAGS=swapd.lib sanes.lib wlmd.lib aslmd.lib wlm.rsc commdlg.rsc\
 nafxcmd.lib /NOLOGO /MAC:bundle /MAC:type="APPL" /MAC:creator="VIEW"\
 /PDB:$(OUTDIR)/"viewex.pdb" /DEBUG /MACHINE:M68K /NODEFAULTLIB:"swap.lib"\
 /OUT:$(OUTDIR)/"viewex.exe" 
DEF_FILE=
LINK32_OBJS= \
	$(INTDIR)/stdafx.obj \
	$(INTDIR)/inputvw.obj \
	$(INTDIR)/maindoc.obj \
	$(INTDIR)/simpvw.obj \
	$(INTDIR)/splitter.obj \
	$(INTDIR)/enterdlg.obj \
	$(INTDIR)/viewex.obj \
	$(INTDIR)/viewex.rsc \
	$(INTDIR)/viewmac.rsc

$(OUTDIR)/viewex.exe : $(OUTDIR)  $(DEF_FILE) $(LINK32_OBJS)
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

ALL : $(OUTDIR)/viewex.exe $(OUTDIR)/viewex.bsc

$(OUTDIR) : 
    if not exist $(OUTDIR)/nul mkdir $(OUTDIR)

MTL=MkTypLib.exe
# ADD BASE MTL /nologo /D "NDEBUG" /mac
# ADD MTL /nologo /D "NDEBUG" /mac
MTL_PROJ=/nologo /D "NDEBUG" /mac 
CPP=cl.exe
# ADD BASE CPP /nologo /AL /Gt1 /Q68s /W3 /GX /YX /O2 /D "_WINDOWS" /D "_MAC" /D "_68K_" /D "NDEBUG" /D "_MBCS" /FR /c
# ADD CPP /nologo /AL /Gt1 /Q68s /W3 /GX /O2 /D "_WINDOWS" /D "_MAC" /D "_68K_" /D "NDEBUG" /FR /Yu"stdafx.h" /c
CPP_PROJ=/nologo /AL /Gt1 /Q68s /W3 /GX /O2 /D "_WINDOWS" /D "_MAC" /D "_68K_"\
 /D "NDEBUG" /FR$(INTDIR)/ /Fp$(OUTDIR)/"viewex.pch" /Yu"stdafx.h" /Fo$(INTDIR)/\
 /c 
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
RSC_PROJ=/r /fo$(INTDIR)/"viewex.rsc" /d "_MAC" /d "_68K_" /d "NDEBUG" 
MRC=mrc.exe
# ADD BASE MRC /D "_MAC" /D "_68K_" /D "NDEBUG" /r
# ADD MRC /D "_MAC" /D "_68K_" /D "NDEBUG"
MRC_PROJ=/D "_MAC" /D "_68K_" /D "NDEBUG" 
MFILE32=mfile.exe
# ADD BASE MFILE32 COPY /NOLOGO
# ADD MFILE32 COPY /NOLOGO
MFILE32_FLAGS=COPY /NOLOGO 
MFILE32_FILES= \
	$(OUTDIR)/viewex.exe

DOWNLOAD :  $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) $(MFILE32_FILES) ":$(MFILE32_DEST)"


BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# SUBTRACT BASE BSC32 /Iu
# ADD BSC32 /nologo
# SUBTRACT BSC32 /Iu
BSC32_FLAGS=/nologo /o$(OUTDIR)/"viewex.bsc" 
BSC32_SBRS= \
	$(INTDIR)/stdafx.sbr \
	$(INTDIR)/inputvw.sbr \
	$(INTDIR)/maindoc.sbr \
	$(INTDIR)/simpvw.sbr \
	$(INTDIR)/splitter.sbr \
	$(INTDIR)/enterdlg.sbr \
	$(INTDIR)/viewex.sbr

$(OUTDIR)/viewex.bsc : $(OUTDIR)  $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 swap.lib sanes.lib wlm.lib aslm.lib wlm.rsc commdlg.rsc nafxcm.lib mfcuia32.lib mfcans32.lib ole32.lib oleaut32.lib uuid.lib /NOLOGO /MAC:bundle /MACHINE:M68K /NODEFAULTLIB:"swapd.lib"
# ADD LINK32 swap.lib sanes.lib wlm.lib aslm.lib wlm.rsc commdlg.rsc nafxcm.lib /NOLOGO /MAC:bundle /MAC:creator="VIEW" /MACHINE:M68K /NODEFAULTLIB:"swapd.lib"
LINK32_FLAGS=swap.lib sanes.lib wlm.lib aslm.lib wlm.rsc commdlg.rsc nafxcm.lib\
 /NOLOGO /MAC:bundle /MAC:type="APPL" /MAC:creator="VIEW"\
 /PDB:$(OUTDIR)/"viewex.pdb" /MACHINE:M68K /NODEFAULTLIB:"swapd.lib"\
 /OUT:$(OUTDIR)/"viewex.exe" 
DEF_FILE=
LINK32_OBJS= \
	$(INTDIR)/stdafx.obj \
	$(INTDIR)/inputvw.obj \
	$(INTDIR)/maindoc.obj \
	$(INTDIR)/simpvw.obj \
	$(INTDIR)/splitter.obj \
	$(INTDIR)/enterdlg.obj \
	$(INTDIR)/viewex.obj \
	$(INTDIR)/viewex.rsc \
	$(INTDIR)/viewmac.rsc

$(OUTDIR)/viewex.exe : $(OUTDIR)  $(DEF_FILE) $(LINK32_OBJS)
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

ALL : $(OUTDIR)/viewex.exe $(OUTDIR)/viewex.bsc

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
 "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"viewex.pch"\
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
RSC_PROJ=/l 0x409 /fo$(INTDIR)/"viewex.res" /d "NDEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# SUBTRACT BASE BSC32 /Iu
# ADD BSC32 /nologo
# SUBTRACT BSC32 /Iu
BSC32_FLAGS=/nologo /o$(OUTDIR)/"viewex.bsc" 
BSC32_SBRS= \
	$(INTDIR)/stdafx.sbr \
	$(INTDIR)/inputvw.sbr \
	$(INTDIR)/maindoc.sbr \
	$(INTDIR)/simpvw.sbr \
	$(INTDIR)/splitter.sbr \
	$(INTDIR)/enterdlg.sbr \
	$(INTDIR)/viewex.sbr

$(OUTDIR)/viewex.bsc : $(OUTDIR)  $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 /NOLOGO /SUBSYSTEM:windows /MACHINE:MIPS
# ADD LINK32 /NOLOGO /SUBSYSTEM:windows /MACHINE:MIPS
LINK32_FLAGS=/NOLOGO /SUBSYSTEM:windows /PDB:$(OUTDIR)/"viewex.pdb"\
 /MACHINE:MIPS /OUT:$(OUTDIR)/"viewex.exe" 
DEF_FILE=
LINK32_OBJS= \
	$(INTDIR)/stdafx.obj \
	$(INTDIR)/inputvw.obj \
	$(INTDIR)/maindoc.obj \
	$(INTDIR)/simpvw.obj \
	$(INTDIR)/splitter.obj \
	$(INTDIR)/enterdlg.obj \
	$(INTDIR)/viewex.obj \
	$(INTDIR)/viewex.res

$(OUTDIR)/viewex.exe : $(OUTDIR)  $(DEF_FILE) $(LINK32_OBJS)
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

ALL : $(OUTDIR)/viewex.exe $(OUTDIR)/viewex.bsc

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
 "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"viewex.pch"\
 /Yu"stdafx.h" /Fo$(INTDIR)/ /Fd$(OUTDIR)/"viewex.pdb" /c 
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
RSC_PROJ=/l 0x409 /fo$(INTDIR)/"viewex.res" /d "_DEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# SUBTRACT BASE BSC32 /Iu
# ADD BSC32 /nologo
# SUBTRACT BSC32 /Iu
BSC32_FLAGS=/nologo /o$(OUTDIR)/"viewex.bsc" 
BSC32_SBRS= \
	$(INTDIR)/stdafx.sbr \
	$(INTDIR)/inputvw.sbr \
	$(INTDIR)/maindoc.sbr \
	$(INTDIR)/simpvw.sbr \
	$(INTDIR)/splitter.sbr \
	$(INTDIR)/enterdlg.sbr \
	$(INTDIR)/viewex.sbr

$(OUTDIR)/viewex.bsc : $(OUTDIR)  $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 /NOLOGO /SUBSYSTEM:windows /DEBUG /MACHINE:MIPS
# ADD LINK32 /NOLOGO /SUBSYSTEM:windows /DEBUG /MACHINE:MIPS
LINK32_FLAGS=/NOLOGO /SUBSYSTEM:windows /PDB:$(OUTDIR)/"viewex.pdb" /DEBUG\
 /MACHINE:MIPS /OUT:$(OUTDIR)/"viewex.exe" 
DEF_FILE=
LINK32_OBJS= \
	$(INTDIR)/stdafx.obj \
	$(INTDIR)/inputvw.obj \
	$(INTDIR)/maindoc.obj \
	$(INTDIR)/simpvw.obj \
	$(INTDIR)/splitter.obj \
	$(INTDIR)/enterdlg.obj \
	$(INTDIR)/viewex.obj \
	$(INTDIR)/viewex.res

$(OUTDIR)/viewex.exe : $(OUTDIR)  $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 

################################################################################
# Begin Group "Source Files"

################################################################################
# Begin Source File

SOURCE=.\stdafx.cpp
DEP_STDAF=\
	.\stdafx.h

!IF  "$(CFG)" == "Win32 (80x86) Release"

# ADD BASE CPP /Yc"STDAFX.H"
# ADD CPP /Yc"stdafx.h"

$(INTDIR)/stdafx.obj :  $(SOURCE)  $(DEP_STDAF) $(INTDIR)
   $(CPP) /nologo /MD /W3 /GX /Ox /D "NDEBUG" /D "_WINDOWS" /D "_X86_" /D\
 "_AFXDLL" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"viewex.pch" /Yc"stdafx.h"\
 /Fo$(INTDIR)/ /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

# ADD BASE CPP /Yc"STDAFX.H"
# ADD CPP /Yc"stdafx.h"

$(INTDIR)/stdafx.obj :  $(SOURCE)  $(DEP_STDAF) $(INTDIR)
   $(CPP) /nologo /MD /W3 /GX /Zi /Od /D "_DEBUG" /D "_WINDOWS" /D "_X86_" /D\
 "_AFXDLL" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"viewex.pch" /Yc"stdafx.h"\
 /Fo$(INTDIR)/ /Fd$(OUTDIR)/"viewex.pdb" /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Macintosh Debug"

# ADD CPP /Yc"stdafx.h"

$(INTDIR)/stdafx.obj :  $(SOURCE)  $(DEP_STDAF) $(INTDIR)
   $(CPP) /nologo /AL /Gt1 /Q68s /Q68m /W3 /GX /Zi /Od /D "_WINDOWS" /D "_MAC"\
 /D "_68K_" /D "_DEBUG" /FR$(INTDIR)/ /Fp$(OUTDIR)/"viewex.pch" /Yc"stdafx.h"\
 /Fo$(INTDIR)/ /Fd$(OUTDIR)/"viewex.pdb" /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Macintosh Release"

# ADD CPP /Yc"stdafx.h"

$(INTDIR)/stdafx.obj :  $(SOURCE)  $(DEP_STDAF) $(INTDIR)
   $(CPP) /nologo /AL /Gt1 /Q68s /W3 /GX /O2 /D "_WINDOWS" /D "_MAC" /D "_68K_"\
 /D "NDEBUG" /FR$(INTDIR)/ /Fp$(OUTDIR)/"viewex.pch" /Yc"stdafx.h" /Fo$(INTDIR)/\
 /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

# ADD BASE CPP /Gt0 /QMOb2000
# ADD CPP /Gt0 /QMOb2000 /Yc"stdafx.h"

$(INTDIR)/stdafx.obj :  $(SOURCE)  $(DEP_STDAF) $(INTDIR)
   $(CPP) /nologo /MD /Gt0 /QMOb2000 /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D\
 "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"viewex.pch"\
 /Yc"stdafx.h" /Fo$(INTDIR)/ /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

# ADD BASE CPP /Gt0 /QMOb2000
# ADD CPP /Gt0 /QMOb2000 /Yc"stdafx.h"

$(INTDIR)/stdafx.obj :  $(SOURCE)  $(DEP_STDAF) $(INTDIR)
   $(CPP) /nologo /MD /Gt0 /QMOb2000 /W3 /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D\
 "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"viewex.pch"\
 /Yc"stdafx.h" /Fo$(INTDIR)/ /Fd$(OUTDIR)/"viewex.pdb" /c  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\inputvw.cpp
DEP_INPUT=\
	.\stdafx.h\
	.\viewex.h\
	.\maindoc.h\
	.\simpvw.h\
	.\inputvw.h

!IF  "$(CFG)" == "Win32 (80x86) Release"

# ADD BASE CPP /Yu"STDAFX.H"
# ADD CPP /Yu"stdafx.h"

$(INTDIR)/inputvw.obj :  $(SOURCE)  $(DEP_INPUT) $(INTDIR) $(INTDIR)/stdafx.obj
   $(CPP) /nologo /MD /W3 /GX /Ox /D "NDEBUG" /D "_WINDOWS" /D "_X86_" /D\
 "_AFXDLL" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"viewex.pch" /Yu"stdafx.h"\
 /Fo$(INTDIR)/ /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

# ADD BASE CPP /Yu"STDAFX.H"
# ADD CPP /Yu"stdafx.h"

$(INTDIR)/inputvw.obj :  $(SOURCE)  $(DEP_INPUT) $(INTDIR) $(INTDIR)/stdafx.obj
   $(CPP) /nologo /MD /W3 /GX /Zi /Od /D "_DEBUG" /D "_WINDOWS" /D "_X86_" /D\
 "_AFXDLL" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"viewex.pch" /Yu"stdafx.h"\
 /Fo$(INTDIR)/ /Fd$(OUTDIR)/"viewex.pdb" /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Macintosh Debug"

# ADD CPP /Yu"stdafx.h"

$(INTDIR)/inputvw.obj :  $(SOURCE)  $(DEP_INPUT) $(INTDIR) $(INTDIR)/stdafx.obj
   $(CPP) /nologo /AL /Gt1 /Q68s /Q68m /W3 /GX /Zi /Od /D "_WINDOWS" /D "_MAC"\
 /D "_68K_" /D "_DEBUG" /FR$(INTDIR)/ /Fp$(OUTDIR)/"viewex.pch" /Yu"stdafx.h"\
 /Fo$(INTDIR)/ /Fd$(OUTDIR)/"viewex.pdb" /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Macintosh Release"

# ADD CPP /Yu"stdafx.h"

$(INTDIR)/inputvw.obj :  $(SOURCE)  $(DEP_INPUT) $(INTDIR) $(INTDIR)/stdafx.obj
   $(CPP) /nologo /AL /Gt1 /Q68s /W3 /GX /O2 /D "_WINDOWS" /D "_MAC" /D "_68K_"\
 /D "NDEBUG" /FR$(INTDIR)/ /Fp$(OUTDIR)/"viewex.pch" /Yu"stdafx.h" /Fo$(INTDIR)/\
 /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

# ADD BASE CPP /Gt0 /QMOb2000
# ADD CPP /Gt0 /QMOb2000 /Yu"stdafx.h"

$(INTDIR)/inputvw.obj :  $(SOURCE)  $(DEP_INPUT) $(INTDIR) $(INTDIR)/stdafx.obj
   $(CPP) /nologo /MD /Gt0 /QMOb2000 /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D\
 "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"viewex.pch"\
 /Yu"stdafx.h" /Fo$(INTDIR)/ /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

# ADD BASE CPP /Gt0 /QMOb2000
# ADD CPP /Gt0 /QMOb2000 /Yu"stdafx.h"

$(INTDIR)/inputvw.obj :  $(SOURCE)  $(DEP_INPUT) $(INTDIR) $(INTDIR)/stdafx.obj
   $(CPP) /nologo /MD /Gt0 /QMOb2000 /W3 /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D\
 "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"viewex.pch"\
 /Yu"stdafx.h" /Fo$(INTDIR)/ /Fd$(OUTDIR)/"viewex.pdb" /c  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\maindoc.cpp
DEP_MAIND=\
	.\stdafx.h\
	.\viewex.h\
	.\enterdlg.h\
	.\maindoc.h\
	.\simpvw.h\
	.\inputvw.h

!IF  "$(CFG)" == "Win32 (80x86) Release"

# ADD BASE CPP /Yu"STDAFX.H"
# ADD CPP /Yu"stdafx.h"

$(INTDIR)/maindoc.obj :  $(SOURCE)  $(DEP_MAIND) $(INTDIR) $(INTDIR)/stdafx.obj
   $(CPP) /nologo /MD /W3 /GX /Ox /D "NDEBUG" /D "_WINDOWS" /D "_X86_" /D\
 "_AFXDLL" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"viewex.pch" /Yu"stdafx.h"\
 /Fo$(INTDIR)/ /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

# ADD BASE CPP /Yu"STDAFX.H"
# ADD CPP /Yu"stdafx.h"

$(INTDIR)/maindoc.obj :  $(SOURCE)  $(DEP_MAIND) $(INTDIR) $(INTDIR)/stdafx.obj
   $(CPP) /nologo /MD /W3 /GX /Zi /Od /D "_DEBUG" /D "_WINDOWS" /D "_X86_" /D\
 "_AFXDLL" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"viewex.pch" /Yu"stdafx.h"\
 /Fo$(INTDIR)/ /Fd$(OUTDIR)/"viewex.pdb" /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Macintosh Debug"

# ADD CPP /Yu"stdafx.h"

$(INTDIR)/maindoc.obj :  $(SOURCE)  $(DEP_MAIND) $(INTDIR) $(INTDIR)/stdafx.obj
   $(CPP) /nologo /AL /Gt1 /Q68s /Q68m /W3 /GX /Zi /Od /D "_WINDOWS" /D "_MAC"\
 /D "_68K_" /D "_DEBUG" /FR$(INTDIR)/ /Fp$(OUTDIR)/"viewex.pch" /Yu"stdafx.h"\
 /Fo$(INTDIR)/ /Fd$(OUTDIR)/"viewex.pdb" /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Macintosh Release"

# ADD CPP /Yu"stdafx.h"

$(INTDIR)/maindoc.obj :  $(SOURCE)  $(DEP_MAIND) $(INTDIR) $(INTDIR)/stdafx.obj
   $(CPP) /nologo /AL /Gt1 /Q68s /W3 /GX /O2 /D "_WINDOWS" /D "_MAC" /D "_68K_"\
 /D "NDEBUG" /FR$(INTDIR)/ /Fp$(OUTDIR)/"viewex.pch" /Yu"stdafx.h" /Fo$(INTDIR)/\
 /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

# ADD BASE CPP /Gt0 /QMOb2000
# ADD CPP /Gt0 /QMOb2000 /Yu"stdafx.h"

$(INTDIR)/maindoc.obj :  $(SOURCE)  $(DEP_MAIND) $(INTDIR) $(INTDIR)/stdafx.obj
   $(CPP) /nologo /MD /Gt0 /QMOb2000 /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D\
 "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"viewex.pch"\
 /Yu"stdafx.h" /Fo$(INTDIR)/ /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

# ADD BASE CPP /Gt0 /QMOb2000
# ADD CPP /Gt0 /QMOb2000 /Yu"stdafx.h"

$(INTDIR)/maindoc.obj :  $(SOURCE)  $(DEP_MAIND) $(INTDIR) $(INTDIR)/stdafx.obj
   $(CPP) /nologo /MD /Gt0 /QMOb2000 /W3 /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D\
 "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"viewex.pch"\
 /Yu"stdafx.h" /Fo$(INTDIR)/ /Fd$(OUTDIR)/"viewex.pdb" /c  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\simpvw.cpp
DEP_SIMPV=\
	.\stdafx.h\
	.\viewex.h\
	.\maindoc.h\
	.\simpvw.h\
	.\inputvw.h

!IF  "$(CFG)" == "Win32 (80x86) Release"

# ADD BASE CPP /Yu"STDAFX.H"
# ADD CPP /Yu"stdafx.h"

$(INTDIR)/simpvw.obj :  $(SOURCE)  $(DEP_SIMPV) $(INTDIR) $(INTDIR)/stdafx.obj
   $(CPP) /nologo /MD /W3 /GX /Ox /D "NDEBUG" /D "_WINDOWS" /D "_X86_" /D\
 "_AFXDLL" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"viewex.pch" /Yu"stdafx.h"\
 /Fo$(INTDIR)/ /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

# ADD BASE CPP /Yu"STDAFX.H"
# ADD CPP /Yu"stdafx.h"

$(INTDIR)/simpvw.obj :  $(SOURCE)  $(DEP_SIMPV) $(INTDIR) $(INTDIR)/stdafx.obj
   $(CPP) /nologo /MD /W3 /GX /Zi /Od /D "_DEBUG" /D "_WINDOWS" /D "_X86_" /D\
 "_AFXDLL" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"viewex.pch" /Yu"stdafx.h"\
 /Fo$(INTDIR)/ /Fd$(OUTDIR)/"viewex.pdb" /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Macintosh Debug"

# ADD CPP /Yu"stdafx.h"

$(INTDIR)/simpvw.obj :  $(SOURCE)  $(DEP_SIMPV) $(INTDIR) $(INTDIR)/stdafx.obj
   $(CPP) /nologo /AL /Gt1 /Q68s /Q68m /W3 /GX /Zi /Od /D "_WINDOWS" /D "_MAC"\
 /D "_68K_" /D "_DEBUG" /FR$(INTDIR)/ /Fp$(OUTDIR)/"viewex.pch" /Yu"stdafx.h"\
 /Fo$(INTDIR)/ /Fd$(OUTDIR)/"viewex.pdb" /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Macintosh Release"

# ADD CPP /Yu"stdafx.h"

$(INTDIR)/simpvw.obj :  $(SOURCE)  $(DEP_SIMPV) $(INTDIR) $(INTDIR)/stdafx.obj
   $(CPP) /nologo /AL /Gt1 /Q68s /W3 /GX /O2 /D "_WINDOWS" /D "_MAC" /D "_68K_"\
 /D "NDEBUG" /FR$(INTDIR)/ /Fp$(OUTDIR)/"viewex.pch" /Yu"stdafx.h" /Fo$(INTDIR)/\
 /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

# ADD BASE CPP /Gt0 /QMOb2000
# ADD CPP /Gt0 /QMOb2000 /Yu"stdafx.h"

$(INTDIR)/simpvw.obj :  $(SOURCE)  $(DEP_SIMPV) $(INTDIR) $(INTDIR)/stdafx.obj
   $(CPP) /nologo /MD /Gt0 /QMOb2000 /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D\
 "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"viewex.pch"\
 /Yu"stdafx.h" /Fo$(INTDIR)/ /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

# ADD BASE CPP /Gt0 /QMOb2000
# ADD CPP /Gt0 /QMOb2000 /Yu"stdafx.h"

$(INTDIR)/simpvw.obj :  $(SOURCE)  $(DEP_SIMPV) $(INTDIR) $(INTDIR)/stdafx.obj
   $(CPP) /nologo /MD /Gt0 /QMOb2000 /W3 /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D\
 "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"viewex.pch"\
 /Yu"stdafx.h" /Fo$(INTDIR)/ /Fd$(OUTDIR)/"viewex.pdb" /c  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\splitter.cpp
DEP_SPLIT=\
	.\stdafx.h\
	.\viewex.h\
	.\splitter.h\
	.\maindoc.h\
	.\simpvw.h\
	.\inputvw.h

!IF  "$(CFG)" == "Win32 (80x86) Release"

# ADD BASE CPP /Yu"STDAFX.H"
# ADD CPP /Yu"stdafx.h"

$(INTDIR)/splitter.obj :  $(SOURCE)  $(DEP_SPLIT) $(INTDIR)\
 $(INTDIR)/stdafx.obj
   $(CPP) /nologo /MD /W3 /GX /Ox /D "NDEBUG" /D "_WINDOWS" /D "_X86_" /D\
 "_AFXDLL" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"viewex.pch" /Yu"stdafx.h"\
 /Fo$(INTDIR)/ /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

# ADD BASE CPP /Yu"STDAFX.H"
# ADD CPP /Yu"stdafx.h"

$(INTDIR)/splitter.obj :  $(SOURCE)  $(DEP_SPLIT) $(INTDIR)\
 $(INTDIR)/stdafx.obj
   $(CPP) /nologo /MD /W3 /GX /Zi /Od /D "_DEBUG" /D "_WINDOWS" /D "_X86_" /D\
 "_AFXDLL" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"viewex.pch" /Yu"stdafx.h"\
 /Fo$(INTDIR)/ /Fd$(OUTDIR)/"viewex.pdb" /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Macintosh Debug"

# ADD CPP /Yu"stdafx.h"

$(INTDIR)/splitter.obj :  $(SOURCE)  $(DEP_SPLIT) $(INTDIR)\
 $(INTDIR)/stdafx.obj
   $(CPP) /nologo /AL /Gt1 /Q68s /Q68m /W3 /GX /Zi /Od /D "_WINDOWS" /D "_MAC"\
 /D "_68K_" /D "_DEBUG" /FR$(INTDIR)/ /Fp$(OUTDIR)/"viewex.pch" /Yu"stdafx.h"\
 /Fo$(INTDIR)/ /Fd$(OUTDIR)/"viewex.pdb" /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Macintosh Release"

# ADD CPP /Yu"stdafx.h"

$(INTDIR)/splitter.obj :  $(SOURCE)  $(DEP_SPLIT) $(INTDIR)\
 $(INTDIR)/stdafx.obj
   $(CPP) /nologo /AL /Gt1 /Q68s /W3 /GX /O2 /D "_WINDOWS" /D "_MAC" /D "_68K_"\
 /D "NDEBUG" /FR$(INTDIR)/ /Fp$(OUTDIR)/"viewex.pch" /Yu"stdafx.h" /Fo$(INTDIR)/\
 /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

# ADD BASE CPP /Gt0 /QMOb2000
# ADD CPP /Gt0 /QMOb2000 /Yu"stdafx.h"

$(INTDIR)/splitter.obj :  $(SOURCE)  $(DEP_SPLIT) $(INTDIR)\
 $(INTDIR)/stdafx.obj
   $(CPP) /nologo /MD /Gt0 /QMOb2000 /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D\
 "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"viewex.pch"\
 /Yu"stdafx.h" /Fo$(INTDIR)/ /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

# ADD BASE CPP /Gt0 /QMOb2000
# ADD CPP /Gt0 /QMOb2000 /Yu"stdafx.h"

$(INTDIR)/splitter.obj :  $(SOURCE)  $(DEP_SPLIT) $(INTDIR)\
 $(INTDIR)/stdafx.obj
   $(CPP) /nologo /MD /Gt0 /QMOb2000 /W3 /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D\
 "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"viewex.pch"\
 /Yu"stdafx.h" /Fo$(INTDIR)/ /Fd$(OUTDIR)/"viewex.pdb" /c  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\enterdlg.cpp
DEP_ENTER=\
	.\stdafx.h\
	.\viewex.h\
	.\enterdlg.h\
	.\maindoc.h\
	.\simpvw.h\
	.\inputvw.h

!IF  "$(CFG)" == "Win32 (80x86) Release"

# ADD BASE CPP /Yu"STDAFX.H"
# ADD CPP /Yu"stdafx.h"

$(INTDIR)/enterdlg.obj :  $(SOURCE)  $(DEP_ENTER) $(INTDIR)\
 $(INTDIR)/stdafx.obj
   $(CPP) /nologo /MD /W3 /GX /Ox /D "NDEBUG" /D "_WINDOWS" /D "_X86_" /D\
 "_AFXDLL" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"viewex.pch" /Yu"stdafx.h"\
 /Fo$(INTDIR)/ /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

# ADD BASE CPP /Yu"STDAFX.H"
# ADD CPP /Yu"stdafx.h"

$(INTDIR)/enterdlg.obj :  $(SOURCE)  $(DEP_ENTER) $(INTDIR)\
 $(INTDIR)/stdafx.obj
   $(CPP) /nologo /MD /W3 /GX /Zi /Od /D "_DEBUG" /D "_WINDOWS" /D "_X86_" /D\
 "_AFXDLL" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"viewex.pch" /Yu"stdafx.h"\
 /Fo$(INTDIR)/ /Fd$(OUTDIR)/"viewex.pdb" /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Macintosh Debug"

# ADD CPP /Yu"stdafx.h"

$(INTDIR)/enterdlg.obj :  $(SOURCE)  $(DEP_ENTER) $(INTDIR)\
 $(INTDIR)/stdafx.obj
   $(CPP) /nologo /AL /Gt1 /Q68s /Q68m /W3 /GX /Zi /Od /D "_WINDOWS" /D "_MAC"\
 /D "_68K_" /D "_DEBUG" /FR$(INTDIR)/ /Fp$(OUTDIR)/"viewex.pch" /Yu"stdafx.h"\
 /Fo$(INTDIR)/ /Fd$(OUTDIR)/"viewex.pdb" /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Macintosh Release"

# ADD CPP /Yu"stdafx.h"

$(INTDIR)/enterdlg.obj :  $(SOURCE)  $(DEP_ENTER) $(INTDIR)\
 $(INTDIR)/stdafx.obj
   $(CPP) /nologo /AL /Gt1 /Q68s /W3 /GX /O2 /D "_WINDOWS" /D "_MAC" /D "_68K_"\
 /D "NDEBUG" /FR$(INTDIR)/ /Fp$(OUTDIR)/"viewex.pch" /Yu"stdafx.h" /Fo$(INTDIR)/\
 /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

# ADD BASE CPP /Gt0 /QMOb2000
# ADD CPP /Gt0 /QMOb2000 /Yu"stdafx.h"

$(INTDIR)/enterdlg.obj :  $(SOURCE)  $(DEP_ENTER) $(INTDIR)\
 $(INTDIR)/stdafx.obj
   $(CPP) /nologo /MD /Gt0 /QMOb2000 /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D\
 "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"viewex.pch"\
 /Yu"stdafx.h" /Fo$(INTDIR)/ /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

# ADD BASE CPP /Gt0 /QMOb2000
# ADD CPP /Gt0 /QMOb2000 /Yu"stdafx.h"

$(INTDIR)/enterdlg.obj :  $(SOURCE)  $(DEP_ENTER) $(INTDIR)\
 $(INTDIR)/stdafx.obj
   $(CPP) /nologo /MD /Gt0 /QMOb2000 /W3 /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D\
 "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"viewex.pch"\
 /Yu"stdafx.h" /Fo$(INTDIR)/ /Fd$(OUTDIR)/"viewex.pdb" /c  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\viewex.cpp
DEP_VIEWE=\
	.\stdafx.h\
	.\viewex.h\
	.\splitter.h\
	.\maindoc.h\
	.\simpvw.h\
	.\inputvw.h

!IF  "$(CFG)" == "Win32 (80x86) Release"

# ADD BASE CPP /Yu"STDAFX.H"
# ADD CPP /Yu"stdafx.h"

$(INTDIR)/viewex.obj :  $(SOURCE)  $(DEP_VIEWE) $(INTDIR) $(INTDIR)/stdafx.obj
   $(CPP) /nologo /MD /W3 /GX /Ox /D "NDEBUG" /D "_WINDOWS" /D "_X86_" /D\
 "_AFXDLL" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"viewex.pch" /Yu"stdafx.h"\
 /Fo$(INTDIR)/ /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

# ADD BASE CPP /Yu"STDAFX.H"
# ADD CPP /Yu"stdafx.h"

$(INTDIR)/viewex.obj :  $(SOURCE)  $(DEP_VIEWE) $(INTDIR) $(INTDIR)/stdafx.obj
   $(CPP) /nologo /MD /W3 /GX /Zi /Od /D "_DEBUG" /D "_WINDOWS" /D "_X86_" /D\
 "_AFXDLL" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"viewex.pch" /Yu"stdafx.h"\
 /Fo$(INTDIR)/ /Fd$(OUTDIR)/"viewex.pdb" /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Macintosh Debug"

# ADD CPP /Yu"stdafx.h"

$(INTDIR)/viewex.obj :  $(SOURCE)  $(DEP_VIEWE) $(INTDIR) $(INTDIR)/stdafx.obj
   $(CPP) /nologo /AL /Gt1 /Q68s /Q68m /W3 /GX /Zi /Od /D "_WINDOWS" /D "_MAC"\
 /D "_68K_" /D "_DEBUG" /FR$(INTDIR)/ /Fp$(OUTDIR)/"viewex.pch" /Yu"stdafx.h"\
 /Fo$(INTDIR)/ /Fd$(OUTDIR)/"viewex.pdb" /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Macintosh Release"

# ADD CPP /Yu"stdafx.h"

$(INTDIR)/viewex.obj :  $(SOURCE)  $(DEP_VIEWE) $(INTDIR) $(INTDIR)/stdafx.obj
   $(CPP) /nologo /AL /Gt1 /Q68s /W3 /GX /O2 /D "_WINDOWS" /D "_MAC" /D "_68K_"\
 /D "NDEBUG" /FR$(INTDIR)/ /Fp$(OUTDIR)/"viewex.pch" /Yu"stdafx.h" /Fo$(INTDIR)/\
 /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

# ADD BASE CPP /Gt0 /QMOb2000
# ADD CPP /Gt0 /QMOb2000 /Yu"stdafx.h"

$(INTDIR)/viewex.obj :  $(SOURCE)  $(DEP_VIEWE) $(INTDIR) $(INTDIR)/stdafx.obj
   $(CPP) /nologo /MD /Gt0 /QMOb2000 /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D\
 "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"viewex.pch"\
 /Yu"stdafx.h" /Fo$(INTDIR)/ /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

# ADD BASE CPP /Gt0 /QMOb2000
# ADD CPP /Gt0 /QMOb2000 /Yu"stdafx.h"

$(INTDIR)/viewex.obj :  $(SOURCE)  $(DEP_VIEWE) $(INTDIR) $(INTDIR)/stdafx.obj
   $(CPP) /nologo /MD /Gt0 /QMOb2000 /W3 /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D\
 "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"viewex.pch"\
 /Yu"stdafx.h" /Fo$(INTDIR)/ /Fd$(OUTDIR)/"viewex.pdb" /c  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\viewex.rc
DEP_VIEWEX=\
	.\viewex.ico

!IF  "$(CFG)" == "Win32 (80x86) Release"

$(INTDIR)/viewex.res :  $(SOURCE)  $(DEP_VIEWEX) $(INTDIR)
   $(RSC) $(RSC_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

$(INTDIR)/viewex.res :  $(SOURCE)  $(DEP_VIEWEX) $(INTDIR)
   $(RSC) $(RSC_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Macintosh Debug"

$(INTDIR)/viewex.rsc :  $(SOURCE)  $(DEP_VIEWEX) $(INTDIR)
   $(RSC) $(RSC_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Macintosh Release"

$(INTDIR)/viewex.rsc :  $(SOURCE)  $(DEP_VIEWEX) $(INTDIR)
   $(RSC) $(RSC_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

$(INTDIR)/viewex.res :  $(SOURCE)  $(DEP_VIEWEX) $(INTDIR)
   $(RSC) $(RSC_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

$(INTDIR)/viewex.res :  $(SOURCE)  $(DEP_VIEWEX) $(INTDIR)
   $(RSC) $(RSC_PROJ)  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\viewmac.r

!IF  "$(CFG)" == "Win32 (80x86) Release"

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

!ELSEIF  "$(CFG)" == "Macintosh Debug"

$(INTDIR)/viewmac.rsc :  $(SOURCE)  $(INTDIR)
   $(MRC) /o$(INTDIR)/"viewmac.rsc" /D "_MAC" /D "_68K_" /D "_DEBUG"  $(SOURCE)\
 

!ELSEIF  "$(CFG)" == "Macintosh Release"

$(INTDIR)/viewmac.rsc :  $(SOURCE)  $(INTDIR)
   $(MRC) /o$(INTDIR)/"viewmac.rsc" /D "_MAC" /D "_68K_" /D "NDEBUG"  $(SOURCE)\
 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

!ENDIF 

# End Source File
# End Group
# End Project
################################################################################
