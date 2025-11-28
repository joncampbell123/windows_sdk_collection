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
 "$(CFG)" != "Win32 (80x86) Unicode Debug" && "$(CFG)" !=\
 "Win32 (80x86) Unicode Release" && "$(CFG)" != "Macintosh Debug" && "$(CFG)" !=\
 "Macintosh Release" && "$(CFG)" != "Win32 (MIPS) Release" && "$(CFG)" !=\
 "Win32 (MIPS) Debug" && "$(CFG)" != "Win32 (MIPS) Unicode Release" && "$(CFG)"\
 != "Win32 (MIPS) Unicode Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "chkbook.mak" CFG="Win32 (80x86) Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Win32 (80x86) Release" (based on "Win32 (x86) Application")
!MESSAGE "Win32 (80x86) Debug" (based on "Win32 (x86) Application")
!MESSAGE "Win32 (80x86) Unicode Debug" (based on "Win32 (x86) Application")
!MESSAGE "Win32 (80x86) Unicode Release" (based on "Win32 (x86) Application")
!MESSAGE "Macintosh Debug" (based on "Macintosh Application")
!MESSAGE "Macintosh Release" (based on "Macintosh Application")
!MESSAGE "Win32 (MIPS) Release" (based on "Win32 (MIPS) Application")
!MESSAGE "Win32 (MIPS) Debug" (based on "Win32 (MIPS) Application")
!MESSAGE "Win32 (MIPS) Unicode Release" (based on "Win32 (MIPS) Application")
!MESSAGE "Win32 (MIPS) Unicode Debug" (based on "Win32 (MIPS) Application")
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

ALL : $(OUTDIR)/chkbook.exe $(OUTDIR)/chkbook.bsc

$(OUTDIR) : 
    if not exist $(OUTDIR)/nul mkdir $(OUTDIR)

MTL=MkTypLib.exe
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
CPP=cl.exe
# ADD BASE CPP /nologo /MT /W3 /GX /YX /Ox /D "NDEBUG" /D "_WINDOWS" /D "_X86_" /D "_MBCS" /FR /c
# ADD CPP /nologo /MD /W3 /GX /Ox /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /Yu"STDAFX.H" /c
CPP_PROJ=/nologo /MD /W3 /GX /Ox /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D\
 "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"chkbook.pch" /Yu"STDAFX.H" /Fo$(INTDIR)/ /c\
 
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
RSC_PROJ=/l 0x409 /fo$(INTDIR)/"chkbook.res" /d "NDEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# SUBTRACT BASE BSC32 /Iu
# ADD BSC32 /nologo
# SUBTRACT BSC32 /Iu
BSC32_FLAGS=/nologo /o$(OUTDIR)/"chkbook.bsc" 
BSC32_SBRS= \
	$(INTDIR)/stdafx.sbr \
	$(INTDIR)/checkdoc.sbr \
	$(INTDIR)/checkvw.sbr \
	$(INTDIR)/chkbkfrm.sbr \
	$(INTDIR)/chkbook.sbr \
	$(INTDIR)/dollcent.sbr \
	$(INTDIR)/fxrecdoc.sbr \
	$(INTDIR)/mainfrm.sbr \
	$(INTDIR)/rowview.sbr \
	$(INTDIR)/bookvw.sbr

$(OUTDIR)/chkbook.bsc : $(OUTDIR)  $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib olecli32.lib olesvr32.lib nafxcw.lib mfcuia32.lib mfcans32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib /NOLOGO /SUBSYSTEM:windows /MACHINE:IX86
# ADD LINK32 /NOLOGO /SUBSYSTEM:windows /MACHINE:IX86
LINK32_FLAGS=/NOLOGO /SUBSYSTEM:windows /INCREMENTAL:no\
 /PDB:$(OUTDIR)/"chkbook.pdb" /MACHINE:IX86 /OUT:$(OUTDIR)/"chkbook.exe" 
DEF_FILE=
LINK32_OBJS= \
	$(INTDIR)/stdafx.obj \
	$(INTDIR)/checkdoc.obj \
	$(INTDIR)/checkvw.obj \
	$(INTDIR)/chkbkfrm.obj \
	$(INTDIR)/chkbook.obj \
	$(INTDIR)/dollcent.obj \
	$(INTDIR)/fxrecdoc.obj \
	$(INTDIR)/mainfrm.obj \
	$(INTDIR)/rowview.obj \
	$(INTDIR)/bookvw.obj \
	$(INTDIR)/chkbook.res

$(OUTDIR)/chkbook.exe : $(OUTDIR)  $(DEF_FILE) $(LINK32_OBJS)
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

ALL : $(OUTDIR)/chkbook.exe $(OUTDIR)/chkbook.bsc

$(OUTDIR) : 
    if not exist $(OUTDIR)/nul mkdir $(OUTDIR)

MTL=MkTypLib.exe
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
CPP=cl.exe
# ADD BASE CPP /nologo /MT /W3 /GX /Zi /YX /Od /D "_DEBUG" /D "_WINDOWS" /D "_X86_" /D "_MBCS" /FR /c
# ADD CPP /nologo /MD /W3 /GX /Zi /Od /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /Yu"STDAFX.H" /c
CPP_PROJ=/nologo /MD /W3 /GX /Zi /Od /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D\
 "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"chkbook.pch" /Yu"STDAFX.H" /Fo$(INTDIR)/\
 /Fd$(OUTDIR)/"chkbook.pdb" /c 
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
RSC_PROJ=/l 0x409 /fo$(INTDIR)/"chkbook.res" /d "_DEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# SUBTRACT BASE BSC32 /Iu
# ADD BSC32 /nologo
# SUBTRACT BSC32 /Iu
BSC32_FLAGS=/nologo /o$(OUTDIR)/"chkbook.bsc" 
BSC32_SBRS= \
	$(INTDIR)/stdafx.sbr \
	$(INTDIR)/checkdoc.sbr \
	$(INTDIR)/checkvw.sbr \
	$(INTDIR)/chkbkfrm.sbr \
	$(INTDIR)/chkbook.sbr \
	$(INTDIR)/dollcent.sbr \
	$(INTDIR)/fxrecdoc.sbr \
	$(INTDIR)/mainfrm.sbr \
	$(INTDIR)/rowview.sbr \
	$(INTDIR)/bookvw.sbr

$(OUTDIR)/chkbook.bsc : $(OUTDIR)  $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib olecli32.lib olesvr32.lib nafxcwd.lib mfcuia32.lib mfcans32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib /NOLOGO /SUBSYSTEM:windows /DEBUG /MACHINE:IX86
# ADD LINK32 /NOLOGO /SUBSYSTEM:windows /DEBUG /MACHINE:IX86
LINK32_FLAGS=/NOLOGO /SUBSYSTEM:windows /INCREMENTAL:yes\
 /PDB:$(OUTDIR)/"chkbook.pdb" /DEBUG /MACHINE:IX86 /OUT:$(OUTDIR)/"chkbook.exe" 
DEF_FILE=
LINK32_OBJS= \
	$(INTDIR)/stdafx.obj \
	$(INTDIR)/checkdoc.obj \
	$(INTDIR)/checkvw.obj \
	$(INTDIR)/chkbkfrm.obj \
	$(INTDIR)/chkbook.obj \
	$(INTDIR)/dollcent.obj \
	$(INTDIR)/fxrecdoc.obj \
	$(INTDIR)/mainfrm.obj \
	$(INTDIR)/rowview.obj \
	$(INTDIR)/bookvw.obj \
	$(INTDIR)/chkbook.res

$(OUTDIR)/chkbook.exe : $(OUTDIR)  $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "Win32 (80x86) Unicode Debug"

# PROP BASE Use_MFC 1
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Win32_Un"
# PROP BASE Intermediate_Dir "Win32_Un"
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "UniDebug"
# PROP Intermediate_Dir "UniDebug"
OUTDIR=.\UniDebug
INTDIR=.\UniDebug

ALL : $(OUTDIR)/chkbook.exe $(OUTDIR)/chkbook.bsc

$(OUTDIR) : 
    if not exist $(OUTDIR)/nul mkdir $(OUTDIR)

MTL=MkTypLib.exe
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
CPP=cl.exe
# ADD BASE CPP /nologo /MT /W3 /GX /Zi /YX /Od /D "_DEBUG" /D "_WINDOWS" /D "_X86_" /D "_MBCS" /FR /c
# ADD CPP /nologo /MD /W3 /GX /Zi /Od /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /D "_UNICODE" /FR /Yu"STDAFX.H" /c
CPP_PROJ=/nologo /MD /W3 /GX /Zi /Od /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D\
 "_MBCS" /D "_UNICODE" /FR$(INTDIR)/ /Fp$(OUTDIR)/"chkbook.pch" /Yu"STDAFX.H"\
 /Fo$(INTDIR)/ /Fd$(OUTDIR)/"chkbook.pdb" /c 
CPP_OBJS=.\UniDebug/

.c{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

RSC=rc.exe
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /fo$(INTDIR)/"chkbook.res" /d "_DEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# SUBTRACT BASE BSC32 /Iu
# ADD BSC32 /nologo
# SUBTRACT BSC32 /Iu
BSC32_FLAGS=/nologo /o$(OUTDIR)/"chkbook.bsc" 
BSC32_SBRS= \
	$(INTDIR)/stdafx.sbr \
	$(INTDIR)/checkdoc.sbr \
	$(INTDIR)/checkvw.sbr \
	$(INTDIR)/chkbkfrm.sbr \
	$(INTDIR)/chkbook.sbr \
	$(INTDIR)/dollcent.sbr \
	$(INTDIR)/fxrecdoc.sbr \
	$(INTDIR)/mainfrm.sbr \
	$(INTDIR)/rowview.sbr \
	$(INTDIR)/bookvw.sbr

$(OUTDIR)/chkbook.bsc : $(OUTDIR)  $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib olecli32.lib olesvr32.lib nafxcwd.lib mfcuia32.lib mfcans32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib /NOLOGO /SUBSYSTEM:windows /DEBUG /MACHINE:IX86
# ADD LINK32 /NOLOGO /ENTRY:"wWinMainCRTStartup" /SUBSYSTEM:windows /DEBUG /MACHINE:IX86
LINK32_FLAGS=/NOLOGO /ENTRY:"wWinMainCRTStartup" /SUBSYSTEM:windows\
 /INCREMENTAL:yes /PDB:$(OUTDIR)/"chkbook.pdb" /DEBUG /MACHINE:IX86\
 /OUT:$(OUTDIR)/"chkbook.exe" 
DEF_FILE=
LINK32_OBJS= \
	$(INTDIR)/stdafx.obj \
	$(INTDIR)/checkdoc.obj \
	$(INTDIR)/checkvw.obj \
	$(INTDIR)/chkbkfrm.obj \
	$(INTDIR)/chkbook.obj \
	$(INTDIR)/dollcent.obj \
	$(INTDIR)/fxrecdoc.obj \
	$(INTDIR)/mainfrm.obj \
	$(INTDIR)/rowview.obj \
	$(INTDIR)/bookvw.obj \
	$(INTDIR)/chkbook.res

$(OUTDIR)/chkbook.exe : $(OUTDIR)  $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "Win32 (80x86) Unicode Release"

# PROP BASE Use_MFC 1
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Win32_Un"
# PROP BASE Intermediate_Dir "Win32_Un"
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "UniRel"
# PROP Intermediate_Dir "UniRel"
OUTDIR=.\UniRel
INTDIR=.\UniRel

ALL : $(OUTDIR)/chkbook.exe $(OUTDIR)/chkbook.bsc

$(OUTDIR) : 
    if not exist $(OUTDIR)/nul mkdir $(OUTDIR)

MTL=MkTypLib.exe
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
CPP=cl.exe
# ADD BASE CPP /nologo /MT /W3 /GX /YX /Ox /D "NDEBUG" /D "_WINDOWS" /D "_X86_" /D "_MBCS" /FR /c
# ADD CPP /nologo /MD /W3 /GX /Ox /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /D "_UNICODE" /FR /Yu"STDAFX.H" /c
CPP_PROJ=/nologo /MD /W3 /GX /Ox /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D\
 "_MBCS" /D "_UNICODE" /FR$(INTDIR)/ /Fp$(OUTDIR)/"chkbook.pch" /Yu"STDAFX.H"\
 /Fo$(INTDIR)/ /c 
CPP_OBJS=.\UniRel/

.c{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

RSC=rc.exe
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /fo$(INTDIR)/"chkbook.res" /d "NDEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# SUBTRACT BASE BSC32 /Iu
# ADD BSC32 /nologo
# SUBTRACT BSC32 /Iu
BSC32_FLAGS=/nologo /o$(OUTDIR)/"chkbook.bsc" 
BSC32_SBRS= \
	$(INTDIR)/stdafx.sbr \
	$(INTDIR)/checkdoc.sbr \
	$(INTDIR)/checkvw.sbr \
	$(INTDIR)/chkbkfrm.sbr \
	$(INTDIR)/chkbook.sbr \
	$(INTDIR)/dollcent.sbr \
	$(INTDIR)/fxrecdoc.sbr \
	$(INTDIR)/mainfrm.sbr \
	$(INTDIR)/rowview.sbr \
	$(INTDIR)/bookvw.sbr

$(OUTDIR)/chkbook.bsc : $(OUTDIR)  $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib olecli32.lib olesvr32.lib nafxcw.lib mfcuia32.lib mfcans32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib /NOLOGO /SUBSYSTEM:windows /MACHINE:IX86
# ADD LINK32 /NOLOGO /ENTRY:"wWinMainCRTStartup" /SUBSYSTEM:windows /MACHINE:IX86
LINK32_FLAGS=/NOLOGO /ENTRY:"wWinMainCRTStartup" /SUBSYSTEM:windows\
 /INCREMENTAL:no /PDB:$(OUTDIR)/"chkbook.pdb" /MACHINE:IX86\
 /OUT:$(OUTDIR)/"chkbook.exe" 
DEF_FILE=
LINK32_OBJS= \
	$(INTDIR)/stdafx.obj \
	$(INTDIR)/checkdoc.obj \
	$(INTDIR)/checkvw.obj \
	$(INTDIR)/chkbkfrm.obj \
	$(INTDIR)/chkbook.obj \
	$(INTDIR)/dollcent.obj \
	$(INTDIR)/fxrecdoc.obj \
	$(INTDIR)/mainfrm.obj \
	$(INTDIR)/rowview.obj \
	$(INTDIR)/bookvw.obj \
	$(INTDIR)/chkbook.res

$(OUTDIR)/chkbook.exe : $(OUTDIR)  $(DEF_FILE) $(LINK32_OBJS)
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

ALL : $(OUTDIR)/chkbook.exe $(OUTDIR)/chkbook.bsc

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
 /D "_68K_" /D "_DEBUG" /FR$(INTDIR)/ /Fp$(OUTDIR)/"chkbook.pch" /Yu"stdafx.h"\
 /Fo$(INTDIR)/ /Fd$(OUTDIR)/"chkbook.pdb" /c 
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
RSC_PROJ=/r /fo$(INTDIR)/"chkbook.rsc" /d "_MAC" /d "_68K_" /d "_DEBUG" 
MRC=mrc.exe
# ADD BASE MRC /D "_MAC" /D "_68K_" /D "_DEBUG"
# ADD MRC /D "_MAC" /D "_68K_" /D "_DEBUG"
MRC_PROJ=/D "_MAC" /D "_68K_" /D "_DEBUG" 
MFILE32=mfile.exe
# ADD BASE MFILE32 COPY /NOLOGO
# ADD MFILE32 COPY /NOLOGO
MFILE32_FLAGS=COPY /NOLOGO 
MFILE32_FILES= \
	$(OUTDIR)/chkbook.exe

DOWNLOAD :  $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) $(MFILE32_FILES) ":$(MFILE32_DEST)"


BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# SUBTRACT BASE BSC32 /Iu
# ADD BSC32 /nologo
# SUBTRACT BSC32 /Iu
BSC32_FLAGS=/nologo /o$(OUTDIR)/"chkbook.bsc" 
BSC32_SBRS= \
	$(INTDIR)/stdafx.sbr \
	$(INTDIR)/checkdoc.sbr \
	$(INTDIR)/checkvw.sbr \
	$(INTDIR)/chkbkfrm.sbr \
	$(INTDIR)/chkbook.sbr \
	$(INTDIR)/dollcent.sbr \
	$(INTDIR)/fxrecdoc.sbr \
	$(INTDIR)/mainfrm.sbr \
	$(INTDIR)/rowview.sbr \
	$(INTDIR)/bookvw.sbr

$(OUTDIR)/chkbook.bsc : $(OUTDIR)  $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 swapd.lib sanes.lib wlmd.lib aslmd.lib wlm.rsc commdlg.rsc nafxcmd.lib mfcuia32.lib mfcans32.lib ole32.lib oleaut32.lib uuid.lib /NOLOGO /MAC:bundle /DEBUG /MACHINE:M68K /NODEFAULTLIB:"swap.lib"
# ADD LINK32 swapd.lib sanes.lib wlmd.lib aslmd.lib wlm.rsc commdlg.rsc nafxcmd.lib /NOLOGO /MAC:bundle /MAC:creator="CHKB" /DEBUG /MACHINE:M68K /NODEFAULTLIB:"swap.lib"
LINK32_FLAGS=swapd.lib sanes.lib wlmd.lib aslmd.lib wlm.rsc commdlg.rsc\
 nafxcmd.lib /NOLOGO /MAC:bundle /MAC:type="APPL" /MAC:creator="CHKB"\
 /PDB:$(OUTDIR)/"chkbook.pdb" /DEBUG /MACHINE:M68K /NODEFAULTLIB:"swap.lib"\
 /OUT:$(OUTDIR)/"chkbook.exe" 
DEF_FILE=
LINK32_OBJS= \
	$(INTDIR)/stdafx.obj \
	$(INTDIR)/checkdoc.obj \
	$(INTDIR)/checkvw.obj \
	$(INTDIR)/chkbkfrm.obj \
	$(INTDIR)/chkbook.obj \
	$(INTDIR)/dollcent.obj \
	$(INTDIR)/fxrecdoc.obj \
	$(INTDIR)/mainfrm.obj \
	$(INTDIR)/rowview.obj \
	$(INTDIR)/bookvw.obj \
	$(INTDIR)/chkbook.rsc \
	$(INTDIR)/chkbmac.rsc

$(OUTDIR)/chkbook.exe : $(OUTDIR)  $(DEF_FILE) $(LINK32_OBJS)
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

ALL : $(OUTDIR)/chkbook.exe $(OUTDIR)/chkbook.bsc

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
 /D "NDEBUG" /FR$(INTDIR)/ /Fp$(OUTDIR)/"chkbook.pch" /Yu"stdafx.h"\
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
RSC_PROJ=/r /fo$(INTDIR)/"chkbook.rsc" /d "_MAC" /d "_68K_" /d "NDEBUG" 
MRC=mrc.exe
# ADD BASE MRC /D "_MAC" /D "_68K_" /D "NDEBUG"
# ADD MRC /D "_MAC" /D "_68K_" /D "NDEBUG"
MRC_PROJ=/D "_MAC" /D "_68K_" /D "NDEBUG" 
MFILE32=mfile.exe
# ADD BASE MFILE32 COPY /NOLOGO
# ADD MFILE32 COPY /NOLOGO
MFILE32_FLAGS=COPY /NOLOGO 
MFILE32_FILES= \
	$(OUTDIR)/chkbook.exe

DOWNLOAD :  $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) $(MFILE32_FILES) ":$(MFILE32_DEST)"


BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# SUBTRACT BASE BSC32 /Iu
# ADD BSC32 /nologo
# SUBTRACT BSC32 /Iu
BSC32_FLAGS=/nologo /o$(OUTDIR)/"chkbook.bsc" 
BSC32_SBRS= \
	$(INTDIR)/stdafx.sbr \
	$(INTDIR)/checkdoc.sbr \
	$(INTDIR)/checkvw.sbr \
	$(INTDIR)/chkbkfrm.sbr \
	$(INTDIR)/chkbook.sbr \
	$(INTDIR)/dollcent.sbr \
	$(INTDIR)/fxrecdoc.sbr \
	$(INTDIR)/mainfrm.sbr \
	$(INTDIR)/rowview.sbr \
	$(INTDIR)/bookvw.sbr

$(OUTDIR)/chkbook.bsc : $(OUTDIR)  $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 swap.lib sanes.lib wlm.lib aslm.lib wlm.rsc commdlg.rsc nafxcm.lib mfcuia32.lib mfcans32.lib ole32.lib oleaut32.lib uuid.lib /NOLOGO /MAC:bundle /MACHINE:M68K /NODEFAULTLIB:"swapd.lib"
# ADD LINK32 swap.lib sanes.lib wlm.lib aslm.lib wlm.rsc commdlg.rsc nafxcm.lib /NOLOGO /MAC:bundle /MAC:creator="CHKB" /MACHINE:M68K /NODEFAULTLIB:"swapd.lib"
LINK32_FLAGS=swap.lib sanes.lib wlm.lib aslm.lib wlm.rsc commdlg.rsc nafxcm.lib\
 /NOLOGO /MAC:bundle /MAC:type="APPL" /MAC:creator="CHKB"\
 /PDB:$(OUTDIR)/"chkbook.pdb" /MACHINE:M68K /NODEFAULTLIB:"swapd.lib"\
 /OUT:$(OUTDIR)/"chkbook.exe" 
DEF_FILE=
LINK32_OBJS= \
	$(INTDIR)/stdafx.obj \
	$(INTDIR)/checkdoc.obj \
	$(INTDIR)/checkvw.obj \
	$(INTDIR)/chkbkfrm.obj \
	$(INTDIR)/chkbook.obj \
	$(INTDIR)/dollcent.obj \
	$(INTDIR)/fxrecdoc.obj \
	$(INTDIR)/mainfrm.obj \
	$(INTDIR)/rowview.obj \
	$(INTDIR)/bookvw.obj \
	$(INTDIR)/chkbook.rsc \
	$(INTDIR)/chkbmac.rsc

$(OUTDIR)/chkbook.exe : $(OUTDIR)  $(DEF_FILE) $(LINK32_OBJS)
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

ALL : $(OUTDIR)/chkbook.exe $(OUTDIR)/chkbook.bsc

$(OUTDIR) : 
    if not exist $(OUTDIR)/nul mkdir $(OUTDIR)

MTL=MkTypLib.exe
# ADD BASE MTL /nologo /D "NDEBUG" /mips
# ADD MTL /nologo /D "NDEBUG" /mips
MTL_PROJ=/nologo /D "NDEBUG" /mips 
CPP=cl.exe
# ADD BASE CPP /nologo /MD /Gt0 /QMOb2000 /W3 /GX /YX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /c
# ADD CPP /nologo /MD /Gt0 /QMOb2000 /W3 /GX /O2 /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /Yu"stdafx.h" /c
CPP_PROJ=/nologo /MD /Gt0 /QMOb2000 /W3 /GX /O2 /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"chkbook.pch" /Yu"stdafx.h"\
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
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /fo$(INTDIR)/"chkbook.res" /d "NDEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o$(OUTDIR)/"chkbook.bsc" 
BSC32_SBRS= \
	$(INTDIR)/stdafx.sbr \
	$(INTDIR)/checkdoc.sbr \
	$(INTDIR)/checkvw.sbr \
	$(INTDIR)/chkbkfrm.sbr \
	$(INTDIR)/chkbook.sbr \
	$(INTDIR)/dollcent.sbr \
	$(INTDIR)/fxrecdoc.sbr \
	$(INTDIR)/mainfrm.sbr \
	$(INTDIR)/rowview.sbr \
	$(INTDIR)/bookvw.sbr

$(OUTDIR)/chkbook.bsc : $(OUTDIR)  $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 /NOLOGO /SUBSYSTEM:windows /MACHINE:MIPS
# ADD LINK32 /NOLOGO /SUBSYSTEM:windows /MACHINE:MIPS
LINK32_FLAGS=/NOLOGO /SUBSYSTEM:windows /PDB:$(OUTDIR)/"chkbook.pdb"\
 /MACHINE:MIPS /OUT:$(OUTDIR)/"chkbook.exe" 
DEF_FILE=
LINK32_OBJS= \
	$(INTDIR)/stdafx.obj \
	$(INTDIR)/checkdoc.obj \
	$(INTDIR)/checkvw.obj \
	$(INTDIR)/chkbkfrm.obj \
	$(INTDIR)/chkbook.obj \
	$(INTDIR)/dollcent.obj \
	$(INTDIR)/fxrecdoc.obj \
	$(INTDIR)/mainfrm.obj \
	$(INTDIR)/rowview.obj \
	$(INTDIR)/bookvw.obj \
	$(INTDIR)/chkbook.res

$(OUTDIR)/chkbook.exe : $(OUTDIR)  $(DEF_FILE) $(LINK32_OBJS)
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

ALL : $(OUTDIR)/chkbook.exe $(OUTDIR)/chkbook.bsc

$(OUTDIR) : 
    if not exist $(OUTDIR)/nul mkdir $(OUTDIR)

MTL=MkTypLib.exe
# ADD BASE MTL /nologo /D "_DEBUG" /mips
# ADD MTL /nologo /D "_DEBUG" /mips
MTL_PROJ=/nologo /D "_DEBUG" /mips 
CPP=cl.exe
# ADD BASE CPP /nologo /MD /Gt0 /QMOb2000 /W3 /GX /Zi /YX /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /c
# ADD CPP /nologo /MD /Gt0 /QMOb2000 /W3 /GX /Zi /Od /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /Yu"stdafx.h" /c
CPP_PROJ=/nologo /MD /Gt0 /QMOb2000 /W3 /GX /Zi /Od /D "_DEBUG" /D "_WINDOWS"\
 /D "_AFXDLL" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"chkbook.pch" /Yu"stdafx.h"\
 /Fo$(INTDIR)/ /Fd$(OUTDIR)/"chkbook.pdb" /c 
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
RSC_PROJ=/l 0x409 /fo$(INTDIR)/"chkbook.res" /d "_DEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o$(OUTDIR)/"chkbook.bsc" 
BSC32_SBRS= \
	$(INTDIR)/stdafx.sbr \
	$(INTDIR)/checkdoc.sbr \
	$(INTDIR)/checkvw.sbr \
	$(INTDIR)/chkbkfrm.sbr \
	$(INTDIR)/chkbook.sbr \
	$(INTDIR)/dollcent.sbr \
	$(INTDIR)/fxrecdoc.sbr \
	$(INTDIR)/mainfrm.sbr \
	$(INTDIR)/rowview.sbr \
	$(INTDIR)/bookvw.sbr

$(OUTDIR)/chkbook.bsc : $(OUTDIR)  $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 /NOLOGO /SUBSYSTEM:windows /DEBUG /MACHINE:MIPS
# ADD LINK32 /NOLOGO /SUBSYSTEM:windows /DEBUG /MACHINE:MIPS
LINK32_FLAGS=/NOLOGO /SUBSYSTEM:windows /PDB:$(OUTDIR)/"chkbook.pdb" /DEBUG\
 /MACHINE:MIPS /OUT:$(OUTDIR)/"chkbook.exe" 
DEF_FILE=
LINK32_OBJS= \
	$(INTDIR)/stdafx.obj \
	$(INTDIR)/checkdoc.obj \
	$(INTDIR)/checkvw.obj \
	$(INTDIR)/chkbkfrm.obj \
	$(INTDIR)/chkbook.obj \
	$(INTDIR)/dollcent.obj \
	$(INTDIR)/fxrecdoc.obj \
	$(INTDIR)/mainfrm.obj \
	$(INTDIR)/rowview.obj \
	$(INTDIR)/bookvw.obj \
	$(INTDIR)/chkbook.res

$(OUTDIR)/chkbook.exe : $(OUTDIR)  $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Unicode Release"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Win32__1"
# PROP BASE Intermediate_Dir "Win32__1"
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "UniRel"
# PROP Intermediate_Dir "UniRel"
OUTDIR=.\UniRel
INTDIR=.\UniRel

ALL : $(OUTDIR)/chkbook.exe $(OUTDIR)/chkbook.bsc

$(OUTDIR) : 
    if not exist $(OUTDIR)/nul mkdir $(OUTDIR)

MTL=MkTypLib.exe
# ADD BASE MTL /nologo /D "NDEBUG" /mips
# ADD MTL /nologo /D "NDEBUG" /mips
MTL_PROJ=/nologo /D "NDEBUG" /mips 
CPP=cl.exe
# ADD BASE CPP /nologo /MD /Gt0 /QMOb2000 /W3 /GX /YX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /c
# ADD CPP /nologo /MD /Gt0 /QMOb2000 /W3 /GX /O2 /D "NDEBUG" /D "_UNICODE" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /Yu"stdafx.h" /c
CPP_PROJ=/nologo /MD /Gt0 /QMOb2000 /W3 /GX /O2 /D "NDEBUG" /D "_UNICODE" /D\
 "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"chkbook.pch"\
 /Yu"stdafx.h" /Fo$(INTDIR)/ /c 
CPP_OBJS=.\UniRel/

.c{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

RSC=rc.exe
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /fo$(INTDIR)/"chkbook.res" /d "NDEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o$(OUTDIR)/"chkbook.bsc" 
BSC32_SBRS= \
	$(INTDIR)/stdafx.sbr \
	$(INTDIR)/checkdoc.sbr \
	$(INTDIR)/checkvw.sbr \
	$(INTDIR)/chkbkfrm.sbr \
	$(INTDIR)/chkbook.sbr \
	$(INTDIR)/dollcent.sbr \
	$(INTDIR)/fxrecdoc.sbr \
	$(INTDIR)/mainfrm.sbr \
	$(INTDIR)/rowview.sbr \
	$(INTDIR)/bookvw.sbr

$(OUTDIR)/chkbook.bsc : $(OUTDIR)  $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 /NOLOGO /SUBSYSTEM:windows /MACHINE:MIPS
# ADD LINK32 /NOLOGO /ENTRY:"wWinMainCRTStartup" /SUBSYSTEM:windows /MACHINE:MIPS
LINK32_FLAGS=/NOLOGO /ENTRY:"wWinMainCRTStartup" /SUBSYSTEM:windows\
 /PDB:$(OUTDIR)/"chkbook.pdb" /MACHINE:MIPS /OUT:$(OUTDIR)/"chkbook.exe" 
DEF_FILE=
LINK32_OBJS= \
	$(INTDIR)/stdafx.obj \
	$(INTDIR)/checkdoc.obj \
	$(INTDIR)/checkvw.obj \
	$(INTDIR)/chkbkfrm.obj \
	$(INTDIR)/chkbook.obj \
	$(INTDIR)/dollcent.obj \
	$(INTDIR)/fxrecdoc.obj \
	$(INTDIR)/mainfrm.obj \
	$(INTDIR)/rowview.obj \
	$(INTDIR)/bookvw.obj \
	$(INTDIR)/chkbook.res

$(OUTDIR)/chkbook.exe : $(OUTDIR)  $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Unicode Debug"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Win32__2"
# PROP BASE Intermediate_Dir "Win32__2"
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "UniDebug"
# PROP Intermediate_Dir "UniDebug"
OUTDIR=.\UniDebug
INTDIR=.\UniDebug

ALL : $(OUTDIR)/chkbook.exe $(OUTDIR)/chkbook.bsc

$(OUTDIR) : 
    if not exist $(OUTDIR)/nul mkdir $(OUTDIR)

MTL=MkTypLib.exe
# ADD BASE MTL /nologo /D "_DEBUG" /mips
# ADD MTL /nologo /D "_DEBUG" /mips
MTL_PROJ=/nologo /D "_DEBUG" /mips 
CPP=cl.exe
# ADD BASE CPP /nologo /MD /Gt0 /QMOb2000 /W3 /GX /Zi /YX /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /c
# ADD CPP /nologo /MD /Gt0 /QMOb2000 /W3 /GX /Zi /Od /D "_DEBUG" /D "_UNICODE" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /Yu"stdafx.h" /c
CPP_PROJ=/nologo /MD /Gt0 /QMOb2000 /W3 /GX /Zi /Od /D "_DEBUG" /D "_UNICODE"\
 /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"chkbook.pch"\
 /Yu"stdafx.h" /Fo$(INTDIR)/ /Fd$(OUTDIR)/"chkbook.pdb" /c 
CPP_OBJS=.\UniDebug/

.c{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

RSC=rc.exe
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /fo$(INTDIR)/"chkbook.res" /d "_DEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o$(OUTDIR)/"chkbook.bsc" 
BSC32_SBRS= \
	$(INTDIR)/stdafx.sbr \
	$(INTDIR)/checkdoc.sbr \
	$(INTDIR)/checkvw.sbr \
	$(INTDIR)/chkbkfrm.sbr \
	$(INTDIR)/chkbook.sbr \
	$(INTDIR)/dollcent.sbr \
	$(INTDIR)/fxrecdoc.sbr \
	$(INTDIR)/mainfrm.sbr \
	$(INTDIR)/rowview.sbr \
	$(INTDIR)/bookvw.sbr

$(OUTDIR)/chkbook.bsc : $(OUTDIR)  $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 /NOLOGO /SUBSYSTEM:windows /DEBUG /MACHINE:MIPS
# ADD LINK32 /NOLOGO /ENTRY:"wWinMainCRTStartup" /SUBSYSTEM:windows /DEBUG /MACHINE:MIPS
LINK32_FLAGS=/NOLOGO /ENTRY:"wWinMainCRTStartup" /SUBSYSTEM:windows\
 /PDB:$(OUTDIR)/"chkbook.pdb" /DEBUG /MACHINE:MIPS /OUT:$(OUTDIR)/"chkbook.exe" 
DEF_FILE=
LINK32_OBJS= \
	$(INTDIR)/stdafx.obj \
	$(INTDIR)/checkdoc.obj \
	$(INTDIR)/checkvw.obj \
	$(INTDIR)/chkbkfrm.obj \
	$(INTDIR)/chkbook.obj \
	$(INTDIR)/dollcent.obj \
	$(INTDIR)/fxrecdoc.obj \
	$(INTDIR)/mainfrm.obj \
	$(INTDIR)/rowview.obj \
	$(INTDIR)/bookvw.obj \
	$(INTDIR)/chkbook.res

$(OUTDIR)/chkbook.exe : $(OUTDIR)  $(DEF_FILE) $(LINK32_OBJS)
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
# ADD CPP /Yc"STDAFX.H"

$(INTDIR)/stdafx.obj :  $(SOURCE)  $(DEP_STDAF) $(INTDIR)
   $(CPP) /nologo /MD /W3 /GX /Ox /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D\
 "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"chkbook.pch" /Yc"STDAFX.H" /Fo$(INTDIR)/ /c\
  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

# ADD BASE CPP /Yc"STDAFX.H"
# ADD CPP /Yc"STDAFX.H"

$(INTDIR)/stdafx.obj :  $(SOURCE)  $(DEP_STDAF) $(INTDIR)
   $(CPP) /nologo /MD /W3 /GX /Zi /Od /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D\
 "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"chkbook.pch" /Yc"STDAFX.H" /Fo$(INTDIR)/\
 /Fd$(OUTDIR)/"chkbook.pdb" /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (80x86) Unicode Debug"

# ADD BASE CPP /Yc"STDAFX.H"
# ADD CPP /Yc"STDAFX.H"

$(INTDIR)/stdafx.obj :  $(SOURCE)  $(DEP_STDAF) $(INTDIR)
   $(CPP) /nologo /MD /W3 /GX /Zi /Od /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D\
 "_MBCS" /D "_UNICODE" /FR$(INTDIR)/ /Fp$(OUTDIR)/"chkbook.pch" /Yc"STDAFX.H"\
 /Fo$(INTDIR)/ /Fd$(OUTDIR)/"chkbook.pdb" /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (80x86) Unicode Release"

# ADD BASE CPP /Yc"STDAFX.H"
# ADD CPP /Yc"STDAFX.H"

$(INTDIR)/stdafx.obj :  $(SOURCE)  $(DEP_STDAF) $(INTDIR)
   $(CPP) /nologo /MD /W3 /GX /Ox /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D\
 "_MBCS" /D "_UNICODE" /FR$(INTDIR)/ /Fp$(OUTDIR)/"chkbook.pch" /Yc"STDAFX.H"\
 /Fo$(INTDIR)/ /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Macintosh Debug"

# ADD CPP /Yc"stdafx.h"

$(INTDIR)/stdafx.obj :  $(SOURCE)  $(DEP_STDAF) $(INTDIR)
   $(CPP) /nologo /AL /Gt1 /Q68s /Q68m /W3 /GX /Zi /Od /D "_WINDOWS" /D "_MAC"\
 /D "_68K_" /D "_DEBUG" /FR$(INTDIR)/ /Fp$(OUTDIR)/"chkbook.pch" /Yc"stdafx.h"\
 /Fo$(INTDIR)/ /Fd$(OUTDIR)/"chkbook.pdb" /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Macintosh Release"

# ADD CPP /Yc"stdafx.h"

$(INTDIR)/stdafx.obj :  $(SOURCE)  $(DEP_STDAF) $(INTDIR)
   $(CPP) /nologo /AL /Gt1 /Q68s /W3 /GX /O2 /D "_WINDOWS" /D "_MAC" /D "_68K_"\
 /D "NDEBUG" /FR$(INTDIR)/ /Fp$(OUTDIR)/"chkbook.pch" /Yc"stdafx.h"\
 /Fo$(INTDIR)/ /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

# ADD BASE CPP /Gt0 /QMOb2000
# ADD CPP /Gt0 /QMOb2000 /Yc"stdafx.h"

$(INTDIR)/stdafx.obj :  $(SOURCE)  $(DEP_STDAF) $(INTDIR)
   $(CPP) /nologo /MD /Gt0 /QMOb2000 /W3 /GX /O2 /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"chkbook.pch" /Yc"stdafx.h"\
 /Fo$(INTDIR)/ /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

# ADD BASE CPP /Gt0 /QMOb2000
# ADD CPP /Gt0 /QMOb2000 /Yc"stdafx.h"

$(INTDIR)/stdafx.obj :  $(SOURCE)  $(DEP_STDAF) $(INTDIR)
   $(CPP) /nologo /MD /Gt0 /QMOb2000 /W3 /GX /Zi /Od /D "_DEBUG" /D "_WINDOWS"\
 /D "_AFXDLL" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"chkbook.pch" /Yc"stdafx.h"\
 /Fo$(INTDIR)/ /Fd$(OUTDIR)/"chkbook.pdb" /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Unicode Release"

# ADD BASE CPP /Gt0 /QMOb2000
# ADD CPP /Gt0 /QMOb2000 /Yc"stdafx.h"

$(INTDIR)/stdafx.obj :  $(SOURCE)  $(DEP_STDAF) $(INTDIR)
   $(CPP) /nologo /MD /Gt0 /QMOb2000 /W3 /GX /O2 /D "NDEBUG" /D "_UNICODE" /D\
 "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"chkbook.pch"\
 /Yc"stdafx.h" /Fo$(INTDIR)/ /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Unicode Debug"

# ADD BASE CPP /Gt0 /QMOb2000
# ADD CPP /Gt0 /QMOb2000 /Yc"stdafx.h"

$(INTDIR)/stdafx.obj :  $(SOURCE)  $(DEP_STDAF) $(INTDIR)
   $(CPP) /nologo /MD /Gt0 /QMOb2000 /W3 /GX /Zi /Od /D "_DEBUG" /D "_UNICODE"\
 /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"chkbook.pch"\
 /Yc"stdafx.h" /Fo$(INTDIR)/ /Fd$(OUTDIR)/"chkbook.pdb" /c  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\checkdoc.cpp
DEP_CHECK=\
	.\stdafx.h\
	.\chkbook.h\
	.\mainfrm.h\
	.\chkbkfrm.h\
	.\dollcent.h\
	.\fxrecdoc.h\
	.\checkdoc.h\
	.\checkvw.h\
	.\rowview.h\
	.\bookvw.h

!IF  "$(CFG)" == "Win32 (80x86) Release"

# ADD BASE CPP /Yu"STDAFX.H"
# ADD CPP /Yu"STDAFX.H"

$(INTDIR)/checkdoc.obj :  $(SOURCE)  $(DEP_CHECK) $(INTDIR)\
 $(INTDIR)/stdafx.obj
   $(CPP) /nologo /MD /W3 /GX /Ox /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D\
 "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"chkbook.pch" /Yu"STDAFX.H" /Fo$(INTDIR)/ /c\
  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

# ADD BASE CPP /Yu"STDAFX.H"
# ADD CPP /Yu"STDAFX.H"

$(INTDIR)/checkdoc.obj :  $(SOURCE)  $(DEP_CHECK) $(INTDIR)\
 $(INTDIR)/stdafx.obj
   $(CPP) /nologo /MD /W3 /GX /Zi /Od /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D\
 "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"chkbook.pch" /Yu"STDAFX.H" /Fo$(INTDIR)/\
 /Fd$(OUTDIR)/"chkbook.pdb" /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (80x86) Unicode Debug"

# ADD BASE CPP /Yu"STDAFX.H"
# ADD CPP /Yu"STDAFX.H"

$(INTDIR)/checkdoc.obj :  $(SOURCE)  $(DEP_CHECK) $(INTDIR)\
 $(INTDIR)/stdafx.obj
   $(CPP) /nologo /MD /W3 /GX /Zi /Od /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D\
 "_MBCS" /D "_UNICODE" /FR$(INTDIR)/ /Fp$(OUTDIR)/"chkbook.pch" /Yu"STDAFX.H"\
 /Fo$(INTDIR)/ /Fd$(OUTDIR)/"chkbook.pdb" /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (80x86) Unicode Release"

# ADD BASE CPP /Yu"STDAFX.H"
# ADD CPP /Yu"STDAFX.H"

$(INTDIR)/checkdoc.obj :  $(SOURCE)  $(DEP_CHECK) $(INTDIR)\
 $(INTDIR)/stdafx.obj
   $(CPP) /nologo /MD /W3 /GX /Ox /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D\
 "_MBCS" /D "_UNICODE" /FR$(INTDIR)/ /Fp$(OUTDIR)/"chkbook.pch" /Yu"STDAFX.H"\
 /Fo$(INTDIR)/ /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Macintosh Debug"

# ADD CPP /Yu"stdafx.h"

$(INTDIR)/checkdoc.obj :  $(SOURCE)  $(DEP_CHECK) $(INTDIR)\
 $(INTDIR)/stdafx.obj
   $(CPP) /nologo /AL /Gt1 /Q68s /Q68m /W3 /GX /Zi /Od /D "_WINDOWS" /D "_MAC"\
 /D "_68K_" /D "_DEBUG" /FR$(INTDIR)/ /Fp$(OUTDIR)/"chkbook.pch" /Yu"stdafx.h"\
 /Fo$(INTDIR)/ /Fd$(OUTDIR)/"chkbook.pdb" /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Macintosh Release"

# ADD CPP /Yu"stdafx.h"

$(INTDIR)/checkdoc.obj :  $(SOURCE)  $(DEP_CHECK) $(INTDIR)\
 $(INTDIR)/stdafx.obj
   $(CPP) /nologo /AL /Gt1 /Q68s /W3 /GX /O2 /D "_WINDOWS" /D "_MAC" /D "_68K_"\
 /D "NDEBUG" /FR$(INTDIR)/ /Fp$(OUTDIR)/"chkbook.pch" /Yu"stdafx.h"\
 /Fo$(INTDIR)/ /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

# ADD BASE CPP /Gt0 /QMOb2000
# ADD CPP /Gt0 /QMOb2000 /Yu"stdafx.h"

$(INTDIR)/checkdoc.obj :  $(SOURCE)  $(DEP_CHECK) $(INTDIR)\
 $(INTDIR)/stdafx.obj
   $(CPP) /nologo /MD /Gt0 /QMOb2000 /W3 /GX /O2 /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"chkbook.pch" /Yu"stdafx.h"\
 /Fo$(INTDIR)/ /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

# ADD BASE CPP /Gt0 /QMOb2000
# ADD CPP /Gt0 /QMOb2000 /Yu"stdafx.h"

$(INTDIR)/checkdoc.obj :  $(SOURCE)  $(DEP_CHECK) $(INTDIR)\
 $(INTDIR)/stdafx.obj
   $(CPP) /nologo /MD /Gt0 /QMOb2000 /W3 /GX /Zi /Od /D "_DEBUG" /D "_WINDOWS"\
 /D "_AFXDLL" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"chkbook.pch" /Yu"stdafx.h"\
 /Fo$(INTDIR)/ /Fd$(OUTDIR)/"chkbook.pdb" /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Unicode Release"

# ADD BASE CPP /Gt0 /QMOb2000
# ADD CPP /Gt0 /QMOb2000 /Yu"stdafx.h"

$(INTDIR)/checkdoc.obj :  $(SOURCE)  $(DEP_CHECK) $(INTDIR)\
 $(INTDIR)/stdafx.obj
   $(CPP) /nologo /MD /Gt0 /QMOb2000 /W3 /GX /O2 /D "NDEBUG" /D "_UNICODE" /D\
 "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"chkbook.pch"\
 /Yu"stdafx.h" /Fo$(INTDIR)/ /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Unicode Debug"

# ADD BASE CPP /Gt0 /QMOb2000
# ADD CPP /Gt0 /QMOb2000 /Yu"stdafx.h"

$(INTDIR)/checkdoc.obj :  $(SOURCE)  $(DEP_CHECK) $(INTDIR)\
 $(INTDIR)/stdafx.obj
   $(CPP) /nologo /MD /Gt0 /QMOb2000 /W3 /GX /Zi /Od /D "_DEBUG" /D "_UNICODE"\
 /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"chkbook.pch"\
 /Yu"stdafx.h" /Fo$(INTDIR)/ /Fd$(OUTDIR)/"chkbook.pdb" /c  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\checkvw.cpp
DEP_CHECKV=\
	.\stdafx.h\
	.\chkbook.h\
	.\mainfrm.h\
	.\chkbkfrm.h\
	.\dollcent.h\
	.\fxrecdoc.h\
	.\checkdoc.h\
	.\checkvw.h\
	.\rowview.h\
	.\bookvw.h

!IF  "$(CFG)" == "Win32 (80x86) Release"

# ADD BASE CPP /Yu"STDAFX.H"
# ADD CPP /Yu"STDAFX.H"

$(INTDIR)/checkvw.obj :  $(SOURCE)  $(DEP_CHECKV) $(INTDIR)\
 $(INTDIR)/stdafx.obj
   $(CPP) /nologo /MD /W3 /GX /Ox /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D\
 "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"chkbook.pch" /Yu"STDAFX.H" /Fo$(INTDIR)/ /c\
  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

# ADD BASE CPP /Yu"STDAFX.H"
# ADD CPP /Yu"STDAFX.H"

$(INTDIR)/checkvw.obj :  $(SOURCE)  $(DEP_CHECKV) $(INTDIR)\
 $(INTDIR)/stdafx.obj
   $(CPP) /nologo /MD /W3 /GX /Zi /Od /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D\
 "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"chkbook.pch" /Yu"STDAFX.H" /Fo$(INTDIR)/\
 /Fd$(OUTDIR)/"chkbook.pdb" /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (80x86) Unicode Debug"

# ADD BASE CPP /Yu"STDAFX.H"
# ADD CPP /Yu"STDAFX.H"

$(INTDIR)/checkvw.obj :  $(SOURCE)  $(DEP_CHECKV) $(INTDIR)\
 $(INTDIR)/stdafx.obj
   $(CPP) /nologo /MD /W3 /GX /Zi /Od /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D\
 "_MBCS" /D "_UNICODE" /FR$(INTDIR)/ /Fp$(OUTDIR)/"chkbook.pch" /Yu"STDAFX.H"\
 /Fo$(INTDIR)/ /Fd$(OUTDIR)/"chkbook.pdb" /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (80x86) Unicode Release"

# ADD BASE CPP /Yu"STDAFX.H"
# ADD CPP /Yu"STDAFX.H"

$(INTDIR)/checkvw.obj :  $(SOURCE)  $(DEP_CHECKV) $(INTDIR)\
 $(INTDIR)/stdafx.obj
   $(CPP) /nologo /MD /W3 /GX /Ox /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D\
 "_MBCS" /D "_UNICODE" /FR$(INTDIR)/ /Fp$(OUTDIR)/"chkbook.pch" /Yu"STDAFX.H"\
 /Fo$(INTDIR)/ /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Macintosh Debug"

# ADD CPP /Yu"stdafx.h"

$(INTDIR)/checkvw.obj :  $(SOURCE)  $(DEP_CHECKV) $(INTDIR)\
 $(INTDIR)/stdafx.obj
   $(CPP) /nologo /AL /Gt1 /Q68s /Q68m /W3 /GX /Zi /Od /D "_WINDOWS" /D "_MAC"\
 /D "_68K_" /D "_DEBUG" /FR$(INTDIR)/ /Fp$(OUTDIR)/"chkbook.pch" /Yu"stdafx.h"\
 /Fo$(INTDIR)/ /Fd$(OUTDIR)/"chkbook.pdb" /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Macintosh Release"

# ADD CPP /Yu"stdafx.h"

$(INTDIR)/checkvw.obj :  $(SOURCE)  $(DEP_CHECKV) $(INTDIR)\
 $(INTDIR)/stdafx.obj
   $(CPP) /nologo /AL /Gt1 /Q68s /W3 /GX /O2 /D "_WINDOWS" /D "_MAC" /D "_68K_"\
 /D "NDEBUG" /FR$(INTDIR)/ /Fp$(OUTDIR)/"chkbook.pch" /Yu"stdafx.h"\
 /Fo$(INTDIR)/ /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

# ADD BASE CPP /Gt0 /QMOb2000
# ADD CPP /Gt0 /QMOb2000 /Yu"stdafx.h"

$(INTDIR)/checkvw.obj :  $(SOURCE)  $(DEP_CHECKV) $(INTDIR)\
 $(INTDIR)/stdafx.obj
   $(CPP) /nologo /MD /Gt0 /QMOb2000 /W3 /GX /O2 /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"chkbook.pch" /Yu"stdafx.h"\
 /Fo$(INTDIR)/ /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

# ADD BASE CPP /Gt0 /QMOb2000
# ADD CPP /Gt0 /QMOb2000 /Yu"stdafx.h"

$(INTDIR)/checkvw.obj :  $(SOURCE)  $(DEP_CHECKV) $(INTDIR)\
 $(INTDIR)/stdafx.obj
   $(CPP) /nologo /MD /Gt0 /QMOb2000 /W3 /GX /Zi /Od /D "_DEBUG" /D "_WINDOWS"\
 /D "_AFXDLL" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"chkbook.pch" /Yu"stdafx.h"\
 /Fo$(INTDIR)/ /Fd$(OUTDIR)/"chkbook.pdb" /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Unicode Release"

# ADD BASE CPP /Gt0 /QMOb2000
# ADD CPP /Gt0 /QMOb2000 /Yu"stdafx.h"

$(INTDIR)/checkvw.obj :  $(SOURCE)  $(DEP_CHECKV) $(INTDIR)\
 $(INTDIR)/stdafx.obj
   $(CPP) /nologo /MD /Gt0 /QMOb2000 /W3 /GX /O2 /D "NDEBUG" /D "_UNICODE" /D\
 "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"chkbook.pch"\
 /Yu"stdafx.h" /Fo$(INTDIR)/ /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Unicode Debug"

# ADD BASE CPP /Gt0 /QMOb2000
# ADD CPP /Gt0 /QMOb2000 /Yu"stdafx.h"

$(INTDIR)/checkvw.obj :  $(SOURCE)  $(DEP_CHECKV) $(INTDIR)\
 $(INTDIR)/stdafx.obj
   $(CPP) /nologo /MD /Gt0 /QMOb2000 /W3 /GX /Zi /Od /D "_DEBUG" /D "_UNICODE"\
 /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"chkbook.pch"\
 /Yu"stdafx.h" /Fo$(INTDIR)/ /Fd$(OUTDIR)/"chkbook.pdb" /c  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\chkbkfrm.cpp
DEP_CHKBK=\
	.\stdafx.h\
	.\chkbkfrm.h

!IF  "$(CFG)" == "Win32 (80x86) Release"

# ADD BASE CPP /Yu"STDAFX.H"
# ADD CPP /Yu"STDAFX.H"

$(INTDIR)/chkbkfrm.obj :  $(SOURCE)  $(DEP_CHKBK) $(INTDIR)\
 $(INTDIR)/stdafx.obj
   $(CPP) /nologo /MD /W3 /GX /Ox /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D\
 "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"chkbook.pch" /Yu"STDAFX.H" /Fo$(INTDIR)/ /c\
  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

# ADD BASE CPP /Yu"STDAFX.H"
# ADD CPP /Yu"STDAFX.H"

$(INTDIR)/chkbkfrm.obj :  $(SOURCE)  $(DEP_CHKBK) $(INTDIR)\
 $(INTDIR)/stdafx.obj
   $(CPP) /nologo /MD /W3 /GX /Zi /Od /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D\
 "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"chkbook.pch" /Yu"STDAFX.H" /Fo$(INTDIR)/\
 /Fd$(OUTDIR)/"chkbook.pdb" /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (80x86) Unicode Debug"

# ADD BASE CPP /Yu"STDAFX.H"
# ADD CPP /Yu"STDAFX.H"

$(INTDIR)/chkbkfrm.obj :  $(SOURCE)  $(DEP_CHKBK) $(INTDIR)\
 $(INTDIR)/stdafx.obj
   $(CPP) /nologo /MD /W3 /GX /Zi /Od /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D\
 "_MBCS" /D "_UNICODE" /FR$(INTDIR)/ /Fp$(OUTDIR)/"chkbook.pch" /Yu"STDAFX.H"\
 /Fo$(INTDIR)/ /Fd$(OUTDIR)/"chkbook.pdb" /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (80x86) Unicode Release"

# ADD BASE CPP /Yu"STDAFX.H"
# ADD CPP /Yu"STDAFX.H"

$(INTDIR)/chkbkfrm.obj :  $(SOURCE)  $(DEP_CHKBK) $(INTDIR)\
 $(INTDIR)/stdafx.obj
   $(CPP) /nologo /MD /W3 /GX /Ox /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D\
 "_MBCS" /D "_UNICODE" /FR$(INTDIR)/ /Fp$(OUTDIR)/"chkbook.pch" /Yu"STDAFX.H"\
 /Fo$(INTDIR)/ /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Macintosh Debug"

# ADD CPP /Yu"stdafx.h"

$(INTDIR)/chkbkfrm.obj :  $(SOURCE)  $(DEP_CHKBK) $(INTDIR)\
 $(INTDIR)/stdafx.obj
   $(CPP) /nologo /AL /Gt1 /Q68s /Q68m /W3 /GX /Zi /Od /D "_WINDOWS" /D "_MAC"\
 /D "_68K_" /D "_DEBUG" /FR$(INTDIR)/ /Fp$(OUTDIR)/"chkbook.pch" /Yu"stdafx.h"\
 /Fo$(INTDIR)/ /Fd$(OUTDIR)/"chkbook.pdb" /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Macintosh Release"

# ADD CPP /Yu"stdafx.h"

$(INTDIR)/chkbkfrm.obj :  $(SOURCE)  $(DEP_CHKBK) $(INTDIR)\
 $(INTDIR)/stdafx.obj
   $(CPP) /nologo /AL /Gt1 /Q68s /W3 /GX /O2 /D "_WINDOWS" /D "_MAC" /D "_68K_"\
 /D "NDEBUG" /FR$(INTDIR)/ /Fp$(OUTDIR)/"chkbook.pch" /Yu"stdafx.h"\
 /Fo$(INTDIR)/ /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

# ADD BASE CPP /Gt0 /QMOb2000
# ADD CPP /Gt0 /QMOb2000 /Yu"stdafx.h"

$(INTDIR)/chkbkfrm.obj :  $(SOURCE)  $(DEP_CHKBK) $(INTDIR)\
 $(INTDIR)/stdafx.obj
   $(CPP) /nologo /MD /Gt0 /QMOb2000 /W3 /GX /O2 /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"chkbook.pch" /Yu"stdafx.h"\
 /Fo$(INTDIR)/ /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

# ADD BASE CPP /Gt0 /QMOb2000
# ADD CPP /Gt0 /QMOb2000 /Yu"stdafx.h"

$(INTDIR)/chkbkfrm.obj :  $(SOURCE)  $(DEP_CHKBK) $(INTDIR)\
 $(INTDIR)/stdafx.obj
   $(CPP) /nologo /MD /Gt0 /QMOb2000 /W3 /GX /Zi /Od /D "_DEBUG" /D "_WINDOWS"\
 /D "_AFXDLL" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"chkbook.pch" /Yu"stdafx.h"\
 /Fo$(INTDIR)/ /Fd$(OUTDIR)/"chkbook.pdb" /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Unicode Release"

# ADD BASE CPP /Gt0 /QMOb2000
# ADD CPP /Gt0 /QMOb2000 /Yu"stdafx.h"

$(INTDIR)/chkbkfrm.obj :  $(SOURCE)  $(DEP_CHKBK) $(INTDIR)\
 $(INTDIR)/stdafx.obj
   $(CPP) /nologo /MD /Gt0 /QMOb2000 /W3 /GX /O2 /D "NDEBUG" /D "_UNICODE" /D\
 "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"chkbook.pch"\
 /Yu"stdafx.h" /Fo$(INTDIR)/ /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Unicode Debug"

# ADD BASE CPP /Gt0 /QMOb2000
# ADD CPP /Gt0 /QMOb2000 /Yu"stdafx.h"

$(INTDIR)/chkbkfrm.obj :  $(SOURCE)  $(DEP_CHKBK) $(INTDIR)\
 $(INTDIR)/stdafx.obj
   $(CPP) /nologo /MD /Gt0 /QMOb2000 /W3 /GX /Zi /Od /D "_DEBUG" /D "_UNICODE"\
 /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"chkbook.pch"\
 /Yu"stdafx.h" /Fo$(INTDIR)/ /Fd$(OUTDIR)/"chkbook.pdb" /c  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\chkbook.cpp
DEP_CHKBO=\
	.\stdafx.h\
	.\chkbook.h\
	.\mainfrm.h\
	.\chkbkfrm.h\
	.\dollcent.h\
	.\fxrecdoc.h\
	.\checkdoc.h\
	.\checkvw.h\
	.\rowview.h\
	.\bookvw.h

!IF  "$(CFG)" == "Win32 (80x86) Release"

# ADD BASE CPP /Yu"STDAFX.H"
# ADD CPP /Yu"STDAFX.H"

$(INTDIR)/chkbook.obj :  $(SOURCE)  $(DEP_CHKBO) $(INTDIR) $(INTDIR)/stdafx.obj
   $(CPP) /nologo /MD /W3 /GX /Ox /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D\
 "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"chkbook.pch" /Yu"STDAFX.H" /Fo$(INTDIR)/ /c\
  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

# ADD BASE CPP /Yu"STDAFX.H"
# ADD CPP /Yu"STDAFX.H"

$(INTDIR)/chkbook.obj :  $(SOURCE)  $(DEP_CHKBO) $(INTDIR) $(INTDIR)/stdafx.obj
   $(CPP) /nologo /MD /W3 /GX /Zi /Od /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D\
 "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"chkbook.pch" /Yu"STDAFX.H" /Fo$(INTDIR)/\
 /Fd$(OUTDIR)/"chkbook.pdb" /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (80x86) Unicode Debug"

# ADD BASE CPP /Yu"STDAFX.H"
# ADD CPP /Yu"STDAFX.H"

$(INTDIR)/chkbook.obj :  $(SOURCE)  $(DEP_CHKBO) $(INTDIR) $(INTDIR)/stdafx.obj
   $(CPP) /nologo /MD /W3 /GX /Zi /Od /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D\
 "_MBCS" /D "_UNICODE" /FR$(INTDIR)/ /Fp$(OUTDIR)/"chkbook.pch" /Yu"STDAFX.H"\
 /Fo$(INTDIR)/ /Fd$(OUTDIR)/"chkbook.pdb" /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (80x86) Unicode Release"

# ADD BASE CPP /Yu"STDAFX.H"
# ADD CPP /Yu"STDAFX.H"

$(INTDIR)/chkbook.obj :  $(SOURCE)  $(DEP_CHKBO) $(INTDIR) $(INTDIR)/stdafx.obj
   $(CPP) /nologo /MD /W3 /GX /Ox /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D\
 "_MBCS" /D "_UNICODE" /FR$(INTDIR)/ /Fp$(OUTDIR)/"chkbook.pch" /Yu"STDAFX.H"\
 /Fo$(INTDIR)/ /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Macintosh Debug"

# ADD CPP /Yu"stdafx.h"

$(INTDIR)/chkbook.obj :  $(SOURCE)  $(DEP_CHKBO) $(INTDIR) $(INTDIR)/stdafx.obj
   $(CPP) /nologo /AL /Gt1 /Q68s /Q68m /W3 /GX /Zi /Od /D "_WINDOWS" /D "_MAC"\
 /D "_68K_" /D "_DEBUG" /FR$(INTDIR)/ /Fp$(OUTDIR)/"chkbook.pch" /Yu"stdafx.h"\
 /Fo$(INTDIR)/ /Fd$(OUTDIR)/"chkbook.pdb" /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Macintosh Release"

# ADD CPP /Yu"stdafx.h"

$(INTDIR)/chkbook.obj :  $(SOURCE)  $(DEP_CHKBO) $(INTDIR) $(INTDIR)/stdafx.obj
   $(CPP) /nologo /AL /Gt1 /Q68s /W3 /GX /O2 /D "_WINDOWS" /D "_MAC" /D "_68K_"\
 /D "NDEBUG" /FR$(INTDIR)/ /Fp$(OUTDIR)/"chkbook.pch" /Yu"stdafx.h"\
 /Fo$(INTDIR)/ /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

# ADD BASE CPP /Gt0 /QMOb2000
# ADD CPP /Gt0 /QMOb2000 /Yu"stdafx.h"

$(INTDIR)/chkbook.obj :  $(SOURCE)  $(DEP_CHKBO) $(INTDIR) $(INTDIR)/stdafx.obj
   $(CPP) /nologo /MD /Gt0 /QMOb2000 /W3 /GX /O2 /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"chkbook.pch" /Yu"stdafx.h"\
 /Fo$(INTDIR)/ /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

# ADD BASE CPP /Gt0 /QMOb2000
# ADD CPP /Gt0 /QMOb2000 /Yu"stdafx.h"

$(INTDIR)/chkbook.obj :  $(SOURCE)  $(DEP_CHKBO) $(INTDIR) $(INTDIR)/stdafx.obj
   $(CPP) /nologo /MD /Gt0 /QMOb2000 /W3 /GX /Zi /Od /D "_DEBUG" /D "_WINDOWS"\
 /D "_AFXDLL" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"chkbook.pch" /Yu"stdafx.h"\
 /Fo$(INTDIR)/ /Fd$(OUTDIR)/"chkbook.pdb" /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Unicode Release"

# ADD BASE CPP /Gt0 /QMOb2000
# ADD CPP /Gt0 /QMOb2000 /Yu"stdafx.h"

$(INTDIR)/chkbook.obj :  $(SOURCE)  $(DEP_CHKBO) $(INTDIR) $(INTDIR)/stdafx.obj
   $(CPP) /nologo /MD /Gt0 /QMOb2000 /W3 /GX /O2 /D "NDEBUG" /D "_UNICODE" /D\
 "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"chkbook.pch"\
 /Yu"stdafx.h" /Fo$(INTDIR)/ /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Unicode Debug"

# ADD BASE CPP /Gt0 /QMOb2000
# ADD CPP /Gt0 /QMOb2000 /Yu"stdafx.h"

$(INTDIR)/chkbook.obj :  $(SOURCE)  $(DEP_CHKBO) $(INTDIR) $(INTDIR)/stdafx.obj
   $(CPP) /nologo /MD /Gt0 /QMOb2000 /W3 /GX /Zi /Od /D "_DEBUG" /D "_UNICODE"\
 /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"chkbook.pch"\
 /Yu"stdafx.h" /Fo$(INTDIR)/ /Fd$(OUTDIR)/"chkbook.pdb" /c  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\dollcent.cpp
DEP_DOLLC=\
	.\stdafx.h\
	.\chkbook.h\
	.\mainfrm.h\
	.\chkbkfrm.h\
	.\dollcent.h\
	.\fxrecdoc.h\
	.\checkdoc.h\
	.\checkvw.h\
	.\rowview.h\
	.\bookvw.h

!IF  "$(CFG)" == "Win32 (80x86) Release"

# ADD BASE CPP /Yu"STDAFX.H"
# ADD CPP /Yu"STDAFX.H"

$(INTDIR)/dollcent.obj :  $(SOURCE)  $(DEP_DOLLC) $(INTDIR)\
 $(INTDIR)/stdafx.obj
   $(CPP) /nologo /MD /W3 /GX /Ox /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D\
 "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"chkbook.pch" /Yu"STDAFX.H" /Fo$(INTDIR)/ /c\
  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

# ADD BASE CPP /Yu"STDAFX.H"
# ADD CPP /Yu"STDAFX.H"

$(INTDIR)/dollcent.obj :  $(SOURCE)  $(DEP_DOLLC) $(INTDIR)\
 $(INTDIR)/stdafx.obj
   $(CPP) /nologo /MD /W3 /GX /Zi /Od /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D\
 "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"chkbook.pch" /Yu"STDAFX.H" /Fo$(INTDIR)/\
 /Fd$(OUTDIR)/"chkbook.pdb" /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (80x86) Unicode Debug"

# ADD BASE CPP /Yu"STDAFX.H"
# ADD CPP /Yu"STDAFX.H"

$(INTDIR)/dollcent.obj :  $(SOURCE)  $(DEP_DOLLC) $(INTDIR)\
 $(INTDIR)/stdafx.obj
   $(CPP) /nologo /MD /W3 /GX /Zi /Od /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D\
 "_MBCS" /D "_UNICODE" /FR$(INTDIR)/ /Fp$(OUTDIR)/"chkbook.pch" /Yu"STDAFX.H"\
 /Fo$(INTDIR)/ /Fd$(OUTDIR)/"chkbook.pdb" /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (80x86) Unicode Release"

# ADD BASE CPP /Yu"STDAFX.H"
# ADD CPP /Yu"STDAFX.H"

$(INTDIR)/dollcent.obj :  $(SOURCE)  $(DEP_DOLLC) $(INTDIR)\
 $(INTDIR)/stdafx.obj
   $(CPP) /nologo /MD /W3 /GX /Ox /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D\
 "_MBCS" /D "_UNICODE" /FR$(INTDIR)/ /Fp$(OUTDIR)/"chkbook.pch" /Yu"STDAFX.H"\
 /Fo$(INTDIR)/ /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Macintosh Debug"

# ADD CPP /Yu"stdafx.h"

$(INTDIR)/dollcent.obj :  $(SOURCE)  $(DEP_DOLLC) $(INTDIR)\
 $(INTDIR)/stdafx.obj
   $(CPP) /nologo /AL /Gt1 /Q68s /Q68m /W3 /GX /Zi /Od /D "_WINDOWS" /D "_MAC"\
 /D "_68K_" /D "_DEBUG" /FR$(INTDIR)/ /Fp$(OUTDIR)/"chkbook.pch" /Yu"stdafx.h"\
 /Fo$(INTDIR)/ /Fd$(OUTDIR)/"chkbook.pdb" /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Macintosh Release"

# ADD CPP /Yu"stdafx.h"

$(INTDIR)/dollcent.obj :  $(SOURCE)  $(DEP_DOLLC) $(INTDIR)\
 $(INTDIR)/stdafx.obj
   $(CPP) /nologo /AL /Gt1 /Q68s /W3 /GX /O2 /D "_WINDOWS" /D "_MAC" /D "_68K_"\
 /D "NDEBUG" /FR$(INTDIR)/ /Fp$(OUTDIR)/"chkbook.pch" /Yu"stdafx.h"\
 /Fo$(INTDIR)/ /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

# ADD BASE CPP /Gt0 /QMOb2000
# ADD CPP /Gt0 /QMOb2000 /Yu"stdafx.h"

$(INTDIR)/dollcent.obj :  $(SOURCE)  $(DEP_DOLLC) $(INTDIR)\
 $(INTDIR)/stdafx.obj
   $(CPP) /nologo /MD /Gt0 /QMOb2000 /W3 /GX /O2 /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"chkbook.pch" /Yu"stdafx.h"\
 /Fo$(INTDIR)/ /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

# ADD BASE CPP /Gt0 /QMOb2000
# ADD CPP /Gt0 /QMOb2000 /Yu"stdafx.h"

$(INTDIR)/dollcent.obj :  $(SOURCE)  $(DEP_DOLLC) $(INTDIR)\
 $(INTDIR)/stdafx.obj
   $(CPP) /nologo /MD /Gt0 /QMOb2000 /W3 /GX /Zi /Od /D "_DEBUG" /D "_WINDOWS"\
 /D "_AFXDLL" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"chkbook.pch" /Yu"stdafx.h"\
 /Fo$(INTDIR)/ /Fd$(OUTDIR)/"chkbook.pdb" /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Unicode Release"

# ADD BASE CPP /Gt0 /QMOb2000
# ADD CPP /Gt0 /QMOb2000 /Yu"stdafx.h"

$(INTDIR)/dollcent.obj :  $(SOURCE)  $(DEP_DOLLC) $(INTDIR)\
 $(INTDIR)/stdafx.obj
   $(CPP) /nologo /MD /Gt0 /QMOb2000 /W3 /GX /O2 /D "NDEBUG" /D "_UNICODE" /D\
 "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"chkbook.pch"\
 /Yu"stdafx.h" /Fo$(INTDIR)/ /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Unicode Debug"

# ADD BASE CPP /Gt0 /QMOb2000
# ADD CPP /Gt0 /QMOb2000 /Yu"stdafx.h"

$(INTDIR)/dollcent.obj :  $(SOURCE)  $(DEP_DOLLC) $(INTDIR)\
 $(INTDIR)/stdafx.obj
   $(CPP) /nologo /MD /Gt0 /QMOb2000 /W3 /GX /Zi /Od /D "_DEBUG" /D "_UNICODE"\
 /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"chkbook.pch"\
 /Yu"stdafx.h" /Fo$(INTDIR)/ /Fd$(OUTDIR)/"chkbook.pdb" /c  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\fxrecdoc.cpp
DEP_FXREC=\
	.\stdafx.h\
	.\fxrecdoc.h

!IF  "$(CFG)" == "Win32 (80x86) Release"

# ADD BASE CPP /Yu"STDAFX.H"
# ADD CPP /Yu"STDAFX.H"

$(INTDIR)/fxrecdoc.obj :  $(SOURCE)  $(DEP_FXREC) $(INTDIR)\
 $(INTDIR)/stdafx.obj
   $(CPP) /nologo /MD /W3 /GX /Ox /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D\
 "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"chkbook.pch" /Yu"STDAFX.H" /Fo$(INTDIR)/ /c\
  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

# ADD BASE CPP /Yu"STDAFX.H"
# ADD CPP /Yu"STDAFX.H"

$(INTDIR)/fxrecdoc.obj :  $(SOURCE)  $(DEP_FXREC) $(INTDIR)\
 $(INTDIR)/stdafx.obj
   $(CPP) /nologo /MD /W3 /GX /Zi /Od /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D\
 "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"chkbook.pch" /Yu"STDAFX.H" /Fo$(INTDIR)/\
 /Fd$(OUTDIR)/"chkbook.pdb" /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (80x86) Unicode Debug"

# ADD BASE CPP /Yu"STDAFX.H"
# ADD CPP /Yu"STDAFX.H"

$(INTDIR)/fxrecdoc.obj :  $(SOURCE)  $(DEP_FXREC) $(INTDIR)\
 $(INTDIR)/stdafx.obj
   $(CPP) /nologo /MD /W3 /GX /Zi /Od /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D\
 "_MBCS" /D "_UNICODE" /FR$(INTDIR)/ /Fp$(OUTDIR)/"chkbook.pch" /Yu"STDAFX.H"\
 /Fo$(INTDIR)/ /Fd$(OUTDIR)/"chkbook.pdb" /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (80x86) Unicode Release"

# ADD BASE CPP /Yu"STDAFX.H"
# ADD CPP /Yu"STDAFX.H"

$(INTDIR)/fxrecdoc.obj :  $(SOURCE)  $(DEP_FXREC) $(INTDIR)\
 $(INTDIR)/stdafx.obj
   $(CPP) /nologo /MD /W3 /GX /Ox /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D\
 "_MBCS" /D "_UNICODE" /FR$(INTDIR)/ /Fp$(OUTDIR)/"chkbook.pch" /Yu"STDAFX.H"\
 /Fo$(INTDIR)/ /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Macintosh Debug"

# ADD CPP /Yu"stdafx.h"

$(INTDIR)/fxrecdoc.obj :  $(SOURCE)  $(DEP_FXREC) $(INTDIR)\
 $(INTDIR)/stdafx.obj
   $(CPP) /nologo /AL /Gt1 /Q68s /Q68m /W3 /GX /Zi /Od /D "_WINDOWS" /D "_MAC"\
 /D "_68K_" /D "_DEBUG" /FR$(INTDIR)/ /Fp$(OUTDIR)/"chkbook.pch" /Yu"stdafx.h"\
 /Fo$(INTDIR)/ /Fd$(OUTDIR)/"chkbook.pdb" /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Macintosh Release"

# ADD CPP /Yu"stdafx.h"

$(INTDIR)/fxrecdoc.obj :  $(SOURCE)  $(DEP_FXREC) $(INTDIR)\
 $(INTDIR)/stdafx.obj
   $(CPP) /nologo /AL /Gt1 /Q68s /W3 /GX /O2 /D "_WINDOWS" /D "_MAC" /D "_68K_"\
 /D "NDEBUG" /FR$(INTDIR)/ /Fp$(OUTDIR)/"chkbook.pch" /Yu"stdafx.h"\
 /Fo$(INTDIR)/ /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

# ADD BASE CPP /Gt0 /QMOb2000
# ADD CPP /Gt0 /QMOb2000 /Yu"stdafx.h"

$(INTDIR)/fxrecdoc.obj :  $(SOURCE)  $(DEP_FXREC) $(INTDIR)\
 $(INTDIR)/stdafx.obj
   $(CPP) /nologo /MD /Gt0 /QMOb2000 /W3 /GX /O2 /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"chkbook.pch" /Yu"stdafx.h"\
 /Fo$(INTDIR)/ /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

# ADD BASE CPP /Gt0 /QMOb2000
# ADD CPP /Gt0 /QMOb2000 /Yu"stdafx.h"

$(INTDIR)/fxrecdoc.obj :  $(SOURCE)  $(DEP_FXREC) $(INTDIR)\
 $(INTDIR)/stdafx.obj
   $(CPP) /nologo /MD /Gt0 /QMOb2000 /W3 /GX /Zi /Od /D "_DEBUG" /D "_WINDOWS"\
 /D "_AFXDLL" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"chkbook.pch" /Yu"stdafx.h"\
 /Fo$(INTDIR)/ /Fd$(OUTDIR)/"chkbook.pdb" /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Unicode Release"

# ADD BASE CPP /Gt0 /QMOb2000
# ADD CPP /Gt0 /QMOb2000 /Yu"stdafx.h"

$(INTDIR)/fxrecdoc.obj :  $(SOURCE)  $(DEP_FXREC) $(INTDIR)\
 $(INTDIR)/stdafx.obj
   $(CPP) /nologo /MD /Gt0 /QMOb2000 /W3 /GX /O2 /D "NDEBUG" /D "_UNICODE" /D\
 "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"chkbook.pch"\
 /Yu"stdafx.h" /Fo$(INTDIR)/ /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Unicode Debug"

# ADD BASE CPP /Gt0 /QMOb2000
# ADD CPP /Gt0 /QMOb2000 /Yu"stdafx.h"

$(INTDIR)/fxrecdoc.obj :  $(SOURCE)  $(DEP_FXREC) $(INTDIR)\
 $(INTDIR)/stdafx.obj
   $(CPP) /nologo /MD /Gt0 /QMOb2000 /W3 /GX /Zi /Od /D "_DEBUG" /D "_UNICODE"\
 /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"chkbook.pch"\
 /Yu"stdafx.h" /Fo$(INTDIR)/ /Fd$(OUTDIR)/"chkbook.pdb" /c  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\mainfrm.cpp
DEP_MAINF=\
	.\stdafx.h\
	.\chkbook.h\
	.\mainfrm.h\
	.\chkbkfrm.h\
	.\dollcent.h\
	.\fxrecdoc.h\
	.\checkdoc.h\
	.\checkvw.h\
	.\rowview.h\
	.\bookvw.h

!IF  "$(CFG)" == "Win32 (80x86) Release"

# ADD BASE CPP /Yu"STDAFX.H"
# ADD CPP /Yu"STDAFX.H"

$(INTDIR)/mainfrm.obj :  $(SOURCE)  $(DEP_MAINF) $(INTDIR) $(INTDIR)/stdafx.obj
   $(CPP) /nologo /MD /W3 /GX /Ox /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D\
 "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"chkbook.pch" /Yu"STDAFX.H" /Fo$(INTDIR)/ /c\
  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

# ADD BASE CPP /Yu"STDAFX.H"
# ADD CPP /Yu"STDAFX.H"

$(INTDIR)/mainfrm.obj :  $(SOURCE)  $(DEP_MAINF) $(INTDIR) $(INTDIR)/stdafx.obj
   $(CPP) /nologo /MD /W3 /GX /Zi /Od /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D\
 "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"chkbook.pch" /Yu"STDAFX.H" /Fo$(INTDIR)/\
 /Fd$(OUTDIR)/"chkbook.pdb" /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (80x86) Unicode Debug"

# ADD BASE CPP /Yu"STDAFX.H"
# ADD CPP /Yu"STDAFX.H"

$(INTDIR)/mainfrm.obj :  $(SOURCE)  $(DEP_MAINF) $(INTDIR) $(INTDIR)/stdafx.obj
   $(CPP) /nologo /MD /W3 /GX /Zi /Od /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D\
 "_MBCS" /D "_UNICODE" /FR$(INTDIR)/ /Fp$(OUTDIR)/"chkbook.pch" /Yu"STDAFX.H"\
 /Fo$(INTDIR)/ /Fd$(OUTDIR)/"chkbook.pdb" /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (80x86) Unicode Release"

# ADD BASE CPP /Yu"STDAFX.H"
# ADD CPP /Yu"STDAFX.H"

$(INTDIR)/mainfrm.obj :  $(SOURCE)  $(DEP_MAINF) $(INTDIR) $(INTDIR)/stdafx.obj
   $(CPP) /nologo /MD /W3 /GX /Ox /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D\
 "_MBCS" /D "_UNICODE" /FR$(INTDIR)/ /Fp$(OUTDIR)/"chkbook.pch" /Yu"STDAFX.H"\
 /Fo$(INTDIR)/ /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Macintosh Debug"

# ADD CPP /Yu"stdafx.h"

$(INTDIR)/mainfrm.obj :  $(SOURCE)  $(DEP_MAINF) $(INTDIR) $(INTDIR)/stdafx.obj
   $(CPP) /nologo /AL /Gt1 /Q68s /Q68m /W3 /GX /Zi /Od /D "_WINDOWS" /D "_MAC"\
 /D "_68K_" /D "_DEBUG" /FR$(INTDIR)/ /Fp$(OUTDIR)/"chkbook.pch" /Yu"stdafx.h"\
 /Fo$(INTDIR)/ /Fd$(OUTDIR)/"chkbook.pdb" /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Macintosh Release"

# ADD CPP /Yu"stdafx.h"

$(INTDIR)/mainfrm.obj :  $(SOURCE)  $(DEP_MAINF) $(INTDIR) $(INTDIR)/stdafx.obj
   $(CPP) /nologo /AL /Gt1 /Q68s /W3 /GX /O2 /D "_WINDOWS" /D "_MAC" /D "_68K_"\
 /D "NDEBUG" /FR$(INTDIR)/ /Fp$(OUTDIR)/"chkbook.pch" /Yu"stdafx.h"\
 /Fo$(INTDIR)/ /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

# ADD BASE CPP /Gt0 /QMOb2000
# ADD CPP /Gt0 /QMOb2000 /Yu"stdafx.h"

$(INTDIR)/mainfrm.obj :  $(SOURCE)  $(DEP_MAINF) $(INTDIR) $(INTDIR)/stdafx.obj
   $(CPP) /nologo /MD /Gt0 /QMOb2000 /W3 /GX /O2 /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"chkbook.pch" /Yu"stdafx.h"\
 /Fo$(INTDIR)/ /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

# ADD BASE CPP /Gt0 /QMOb2000
# ADD CPP /Gt0 /QMOb2000 /Yu"stdafx.h"

$(INTDIR)/mainfrm.obj :  $(SOURCE)  $(DEP_MAINF) $(INTDIR) $(INTDIR)/stdafx.obj
   $(CPP) /nologo /MD /Gt0 /QMOb2000 /W3 /GX /Zi /Od /D "_DEBUG" /D "_WINDOWS"\
 /D "_AFXDLL" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"chkbook.pch" /Yu"stdafx.h"\
 /Fo$(INTDIR)/ /Fd$(OUTDIR)/"chkbook.pdb" /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Unicode Release"

# ADD BASE CPP /Gt0 /QMOb2000
# ADD CPP /Gt0 /QMOb2000 /Yu"stdafx.h"

$(INTDIR)/mainfrm.obj :  $(SOURCE)  $(DEP_MAINF) $(INTDIR) $(INTDIR)/stdafx.obj
   $(CPP) /nologo /MD /Gt0 /QMOb2000 /W3 /GX /O2 /D "NDEBUG" /D "_UNICODE" /D\
 "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"chkbook.pch"\
 /Yu"stdafx.h" /Fo$(INTDIR)/ /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Unicode Debug"

# ADD BASE CPP /Gt0 /QMOb2000
# ADD CPP /Gt0 /QMOb2000 /Yu"stdafx.h"

$(INTDIR)/mainfrm.obj :  $(SOURCE)  $(DEP_MAINF) $(INTDIR) $(INTDIR)/stdafx.obj
   $(CPP) /nologo /MD /Gt0 /QMOb2000 /W3 /GX /Zi /Od /D "_DEBUG" /D "_UNICODE"\
 /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"chkbook.pch"\
 /Yu"stdafx.h" /Fo$(INTDIR)/ /Fd$(OUTDIR)/"chkbook.pdb" /c  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\rowview.cpp
DEP_ROWVI=\
	.\stdafx.h\
	.\chkbook.h\
	.\mainfrm.h\
	.\chkbkfrm.h\
	.\dollcent.h\
	.\fxrecdoc.h\
	.\checkdoc.h\
	.\checkvw.h\
	.\rowview.h\
	.\bookvw.h

!IF  "$(CFG)" == "Win32 (80x86) Release"

# ADD BASE CPP /Yu"STDAFX.H"
# ADD CPP /Yu"STDAFX.H"

$(INTDIR)/rowview.obj :  $(SOURCE)  $(DEP_ROWVI) $(INTDIR) $(INTDIR)/stdafx.obj
   $(CPP) /nologo /MD /W3 /GX /Ox /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D\
 "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"chkbook.pch" /Yu"STDAFX.H" /Fo$(INTDIR)/ /c\
  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

# ADD BASE CPP /Yu"STDAFX.H"
# ADD CPP /Yu"STDAFX.H"

$(INTDIR)/rowview.obj :  $(SOURCE)  $(DEP_ROWVI) $(INTDIR) $(INTDIR)/stdafx.obj
   $(CPP) /nologo /MD /W3 /GX /Zi /Od /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D\
 "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"chkbook.pch" /Yu"STDAFX.H" /Fo$(INTDIR)/\
 /Fd$(OUTDIR)/"chkbook.pdb" /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (80x86) Unicode Debug"

# ADD BASE CPP /Yu"STDAFX.H"
# ADD CPP /Yu"STDAFX.H"

$(INTDIR)/rowview.obj :  $(SOURCE)  $(DEP_ROWVI) $(INTDIR) $(INTDIR)/stdafx.obj
   $(CPP) /nologo /MD /W3 /GX /Zi /Od /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D\
 "_MBCS" /D "_UNICODE" /FR$(INTDIR)/ /Fp$(OUTDIR)/"chkbook.pch" /Yu"STDAFX.H"\
 /Fo$(INTDIR)/ /Fd$(OUTDIR)/"chkbook.pdb" /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (80x86) Unicode Release"

# ADD BASE CPP /Yu"STDAFX.H"
# ADD CPP /Yu"STDAFX.H"

$(INTDIR)/rowview.obj :  $(SOURCE)  $(DEP_ROWVI) $(INTDIR) $(INTDIR)/stdafx.obj
   $(CPP) /nologo /MD /W3 /GX /Ox /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D\
 "_MBCS" /D "_UNICODE" /FR$(INTDIR)/ /Fp$(OUTDIR)/"chkbook.pch" /Yu"STDAFX.H"\
 /Fo$(INTDIR)/ /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Macintosh Debug"

# ADD CPP /Yu"stdafx.h"

$(INTDIR)/rowview.obj :  $(SOURCE)  $(DEP_ROWVI) $(INTDIR) $(INTDIR)/stdafx.obj
   $(CPP) /nologo /AL /Gt1 /Q68s /Q68m /W3 /GX /Zi /Od /D "_WINDOWS" /D "_MAC"\
 /D "_68K_" /D "_DEBUG" /FR$(INTDIR)/ /Fp$(OUTDIR)/"chkbook.pch" /Yu"stdafx.h"\
 /Fo$(INTDIR)/ /Fd$(OUTDIR)/"chkbook.pdb" /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Macintosh Release"

# ADD CPP /Yu"stdafx.h"

$(INTDIR)/rowview.obj :  $(SOURCE)  $(DEP_ROWVI) $(INTDIR) $(INTDIR)/stdafx.obj
   $(CPP) /nologo /AL /Gt1 /Q68s /W3 /GX /O2 /D "_WINDOWS" /D "_MAC" /D "_68K_"\
 /D "NDEBUG" /FR$(INTDIR)/ /Fp$(OUTDIR)/"chkbook.pch" /Yu"stdafx.h"\
 /Fo$(INTDIR)/ /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

# ADD BASE CPP /Gt0 /QMOb2000
# ADD CPP /Gt0 /QMOb2000 /Yu"stdafx.h"

$(INTDIR)/rowview.obj :  $(SOURCE)  $(DEP_ROWVI) $(INTDIR) $(INTDIR)/stdafx.obj
   $(CPP) /nologo /MD /Gt0 /QMOb2000 /W3 /GX /O2 /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"chkbook.pch" /Yu"stdafx.h"\
 /Fo$(INTDIR)/ /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

# ADD BASE CPP /Gt0 /QMOb2000
# ADD CPP /Gt0 /QMOb2000 /Yu"stdafx.h"

$(INTDIR)/rowview.obj :  $(SOURCE)  $(DEP_ROWVI) $(INTDIR) $(INTDIR)/stdafx.obj
   $(CPP) /nologo /MD /Gt0 /QMOb2000 /W3 /GX /Zi /Od /D "_DEBUG" /D "_WINDOWS"\
 /D "_AFXDLL" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"chkbook.pch" /Yu"stdafx.h"\
 /Fo$(INTDIR)/ /Fd$(OUTDIR)/"chkbook.pdb" /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Unicode Release"

# ADD BASE CPP /Gt0 /QMOb2000
# ADD CPP /Gt0 /QMOb2000 /Yu"stdafx.h"

$(INTDIR)/rowview.obj :  $(SOURCE)  $(DEP_ROWVI) $(INTDIR) $(INTDIR)/stdafx.obj
   $(CPP) /nologo /MD /Gt0 /QMOb2000 /W3 /GX /O2 /D "NDEBUG" /D "_UNICODE" /D\
 "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"chkbook.pch"\
 /Yu"stdafx.h" /Fo$(INTDIR)/ /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Unicode Debug"

# ADD BASE CPP /Gt0 /QMOb2000
# ADD CPP /Gt0 /QMOb2000 /Yu"stdafx.h"

$(INTDIR)/rowview.obj :  $(SOURCE)  $(DEP_ROWVI) $(INTDIR) $(INTDIR)/stdafx.obj
   $(CPP) /nologo /MD /Gt0 /QMOb2000 /W3 /GX /Zi /Od /D "_DEBUG" /D "_UNICODE"\
 /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"chkbook.pch"\
 /Yu"stdafx.h" /Fo$(INTDIR)/ /Fd$(OUTDIR)/"chkbook.pdb" /c  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\bookvw.cpp
DEP_BOOKV=\
	.\stdafx.h\
	.\chkbook.h\
	.\mainfrm.h\
	.\chkbkfrm.h\
	.\dollcent.h\
	.\fxrecdoc.h\
	.\checkdoc.h\
	.\checkvw.h\
	.\rowview.h\
	.\bookvw.h

!IF  "$(CFG)" == "Win32 (80x86) Release"

# ADD BASE CPP /Yu"STDAFX.H"
# ADD CPP /Yu"STDAFX.H"

$(INTDIR)/bookvw.obj :  $(SOURCE)  $(DEP_BOOKV) $(INTDIR) $(INTDIR)/stdafx.obj
   $(CPP) /nologo /MD /W3 /GX /Ox /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D\
 "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"chkbook.pch" /Yu"STDAFX.H" /Fo$(INTDIR)/ /c\
  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

# ADD BASE CPP /Yu"STDAFX.H"
# ADD CPP /Yu"STDAFX.H"

$(INTDIR)/bookvw.obj :  $(SOURCE)  $(DEP_BOOKV) $(INTDIR) $(INTDIR)/stdafx.obj
   $(CPP) /nologo /MD /W3 /GX /Zi /Od /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D\
 "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"chkbook.pch" /Yu"STDAFX.H" /Fo$(INTDIR)/\
 /Fd$(OUTDIR)/"chkbook.pdb" /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (80x86) Unicode Debug"

# ADD BASE CPP /Yu"STDAFX.H"
# ADD CPP /Yu"STDAFX.H"

$(INTDIR)/bookvw.obj :  $(SOURCE)  $(DEP_BOOKV) $(INTDIR) $(INTDIR)/stdafx.obj
   $(CPP) /nologo /MD /W3 /GX /Zi /Od /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D\
 "_MBCS" /D "_UNICODE" /FR$(INTDIR)/ /Fp$(OUTDIR)/"chkbook.pch" /Yu"STDAFX.H"\
 /Fo$(INTDIR)/ /Fd$(OUTDIR)/"chkbook.pdb" /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (80x86) Unicode Release"

# ADD BASE CPP /Yu"STDAFX.H"
# ADD CPP /Yu"STDAFX.H"

$(INTDIR)/bookvw.obj :  $(SOURCE)  $(DEP_BOOKV) $(INTDIR) $(INTDIR)/stdafx.obj
   $(CPP) /nologo /MD /W3 /GX /Ox /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D\
 "_MBCS" /D "_UNICODE" /FR$(INTDIR)/ /Fp$(OUTDIR)/"chkbook.pch" /Yu"STDAFX.H"\
 /Fo$(INTDIR)/ /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Macintosh Debug"

# ADD CPP /Yu"stdafx.h"

$(INTDIR)/bookvw.obj :  $(SOURCE)  $(DEP_BOOKV) $(INTDIR) $(INTDIR)/stdafx.obj
   $(CPP) /nologo /AL /Gt1 /Q68s /Q68m /W3 /GX /Zi /Od /D "_WINDOWS" /D "_MAC"\
 /D "_68K_" /D "_DEBUG" /FR$(INTDIR)/ /Fp$(OUTDIR)/"chkbook.pch" /Yu"stdafx.h"\
 /Fo$(INTDIR)/ /Fd$(OUTDIR)/"chkbook.pdb" /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Macintosh Release"

# ADD CPP /Yu"stdafx.h"

$(INTDIR)/bookvw.obj :  $(SOURCE)  $(DEP_BOOKV) $(INTDIR) $(INTDIR)/stdafx.obj
   $(CPP) /nologo /AL /Gt1 /Q68s /W3 /GX /O2 /D "_WINDOWS" /D "_MAC" /D "_68K_"\
 /D "NDEBUG" /FR$(INTDIR)/ /Fp$(OUTDIR)/"chkbook.pch" /Yu"stdafx.h"\
 /Fo$(INTDIR)/ /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

# ADD BASE CPP /Gt0 /QMOb2000
# ADD CPP /Gt0 /QMOb2000 /Yu"stdafx.h"

$(INTDIR)/bookvw.obj :  $(SOURCE)  $(DEP_BOOKV) $(INTDIR) $(INTDIR)/stdafx.obj
   $(CPP) /nologo /MD /Gt0 /QMOb2000 /W3 /GX /O2 /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"chkbook.pch" /Yu"stdafx.h"\
 /Fo$(INTDIR)/ /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

# ADD BASE CPP /Gt0 /QMOb2000
# ADD CPP /Gt0 /QMOb2000 /Yu"stdafx.h"

$(INTDIR)/bookvw.obj :  $(SOURCE)  $(DEP_BOOKV) $(INTDIR) $(INTDIR)/stdafx.obj
   $(CPP) /nologo /MD /Gt0 /QMOb2000 /W3 /GX /Zi /Od /D "_DEBUG" /D "_WINDOWS"\
 /D "_AFXDLL" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"chkbook.pch" /Yu"stdafx.h"\
 /Fo$(INTDIR)/ /Fd$(OUTDIR)/"chkbook.pdb" /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Unicode Release"

# ADD BASE CPP /Gt0 /QMOb2000
# ADD CPP /Gt0 /QMOb2000 /Yu"stdafx.h"

$(INTDIR)/bookvw.obj :  $(SOURCE)  $(DEP_BOOKV) $(INTDIR) $(INTDIR)/stdafx.obj
   $(CPP) /nologo /MD /Gt0 /QMOb2000 /W3 /GX /O2 /D "NDEBUG" /D "_UNICODE" /D\
 "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"chkbook.pch"\
 /Yu"stdafx.h" /Fo$(INTDIR)/ /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Unicode Debug"

# ADD BASE CPP /Gt0 /QMOb2000
# ADD CPP /Gt0 /QMOb2000 /Yu"stdafx.h"

$(INTDIR)/bookvw.obj :  $(SOURCE)  $(DEP_BOOKV) $(INTDIR) $(INTDIR)/stdafx.obj
   $(CPP) /nologo /MD /Gt0 /QMOb2000 /W3 /GX /Zi /Od /D "_DEBUG" /D "_UNICODE"\
 /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"chkbook.pch"\
 /Yu"stdafx.h" /Fo$(INTDIR)/ /Fd$(OUTDIR)/"chkbook.pdb" /c  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\chkbook.rc
DEP_CHKBOO=\
	.\res\chkbook.ico\
	.\res\doc.ico\
	.\res\checkvw.ico\
	.\res\toolbar.bmp

!IF  "$(CFG)" == "Win32 (80x86) Release"

$(INTDIR)/chkbook.res :  $(SOURCE)  $(DEP_CHKBOO) $(INTDIR)
   $(RSC) $(RSC_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

$(INTDIR)/chkbook.res :  $(SOURCE)  $(DEP_CHKBOO) $(INTDIR)
   $(RSC) $(RSC_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (80x86) Unicode Debug"

$(INTDIR)/chkbook.res :  $(SOURCE)  $(DEP_CHKBOO) $(INTDIR)
   $(RSC) $(RSC_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (80x86) Unicode Release"

$(INTDIR)/chkbook.res :  $(SOURCE)  $(DEP_CHKBOO) $(INTDIR)
   $(RSC) $(RSC_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Macintosh Debug"

$(INTDIR)/chkbook.rsc :  $(SOURCE)  $(DEP_CHKBOO) $(INTDIR)
   $(RSC) $(RSC_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Macintosh Release"

$(INTDIR)/chkbook.rsc :  $(SOURCE)  $(DEP_CHKBOO) $(INTDIR)
   $(RSC) $(RSC_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

$(INTDIR)/chkbook.res :  $(SOURCE)  $(DEP_CHKBOO) $(INTDIR)
   $(RSC) $(RSC_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

$(INTDIR)/chkbook.res :  $(SOURCE)  $(DEP_CHKBOO) $(INTDIR)
   $(RSC) $(RSC_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Unicode Release"

$(INTDIR)/chkbook.res :  $(SOURCE)  $(DEP_CHKBOO) $(INTDIR)
   $(RSC) $(RSC_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Unicode Debug"

$(INTDIR)/chkbook.res :  $(SOURCE)  $(DEP_CHKBOO) $(INTDIR)
   $(RSC) $(RSC_PROJ)  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\chkbmac.r

!IF  "$(CFG)" == "Win32 (80x86) Release"

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

!ELSEIF  "$(CFG)" == "Win32 (80x86) Unicode Debug"

!ELSEIF  "$(CFG)" == "Win32 (80x86) Unicode Release"

!ELSEIF  "$(CFG)" == "Macintosh Debug"

$(INTDIR)/chkbmac.rsc :  $(SOURCE)  $(INTDIR)
   $(MRC) /o$(INTDIR)/"chkbmac.rsc" /D "_MAC" /D "_68K_" /D "_DEBUG"  $(SOURCE)\
 

!ELSEIF  "$(CFG)" == "Macintosh Release"

$(INTDIR)/chkbmac.rsc :  $(SOURCE)  $(INTDIR)
   $(MRC) /o$(INTDIR)/"chkbmac.rsc" /D "_MAC" /D "_68K_" /D "NDEBUG"  $(SOURCE)\
 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Unicode Release"

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Unicode Debug"

!ENDIF 

# End Source File
# End Group
# End Project
################################################################################
