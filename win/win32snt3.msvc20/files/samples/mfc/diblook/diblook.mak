# Microsoft Visual C++ Generated NMAKE File, Format Version 2.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101
# TARGTYPE "Win32 (MIPS) Application" 0x0501
# TARGTYPE "Macintosh Application" 0x0301

!IF "$(CFG)" == ""
CFG=Win32 (80x86) Debug
!MESSAGE No configuration specified.  Defaulting to Win32 (80x86) Debug.
!ENDIF 

!IF "$(CFG)" != "Win32 (80x86) Debug" && "$(CFG)" != "Win32 (80x86) Release" &&\
 "$(CFG)" != "Macintosh Debug" && "$(CFG)" != "Macintosh Release" && "$(CFG)" !=\
 "Win32 (MIPS) Release" && "$(CFG)" != "Win32 (MIPS) Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "diblook.mak" CFG="Win32 (80x86) Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Win32 (80x86) Debug" (based on "Win32 (x86) Application")
!MESSAGE "Win32 (80x86) Release" (based on "Win32 (x86) Application")
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

!IF  "$(CFG)" == "Win32 (80x86) Debug"

# PROP BASE Use_MFC 5
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "WinDebug"
# PROP BASE Intermediate_Dir "WinDebug"
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "WinDebug"
# PROP Intermediate_Dir "WinDebug"
OUTDIR=.\WinDebug
INTDIR=.\WinDebug

ALL : .\WinDebug\diblook.exe .\WinDebug\diblook.bsc

$(OUTDIR) : 
    if not exist $(OUTDIR)/nul mkdir $(OUTDIR)

MTL=MkTypLib.exe
MTL_PROJ=
CPP=cl.exe
# ADD BASE CPP /nologo /MT /W3 /GX /Zi /Od /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /FR /Yu"stdafx.h" /c
# ADD CPP /nologo /MD /W3 /GX /Zi /Od /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /Yu"stdafx.h" /c
CPP_PROJ=/nologo /MD /W3 /GX /Zi /Od /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D\
 "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"diblook.pch" /Yu"stdafx.h" /Fo$(INTDIR)/\
 /Fd$(OUTDIR)/"diblook.pdb" /c 
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
RSC_PROJ=/l 0x409 /fo$(INTDIR)/"diblook.res" /d "_DEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o$(OUTDIR)/"diblook.bsc" 
BSC32_SBRS= \
	.\WinDebug\stdafx.sbr \
	.\WinDebug\mainfrm.sbr \
	.\WinDebug\dibview.sbr \
	.\WinDebug\diblook.sbr \
	.\WinDebug\dibdoc.sbr \
	.\WinDebug\dibapi.sbr \
	.\WinDebug\myfile.sbr

.\WinDebug\diblook.bsc : $(OUTDIR)  $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib nafxcwd.lib ctl3d32.lib /NOLOGO /SUBSYSTEM:windows /PDB:"WinDebug\TMP.pdb" /DEBUG /MACHINE:I386
# SUBTRACT BASE LINK32 /PDB:none
# ADD LINK32 /NOLOGO /SUBSYSTEM:windows /PDB:"WinDebug\TMP.pdb" /DEBUG /MACHINE:I386
# SUBTRACT LINK32 /PDB:none
LINK32_FLAGS=/NOLOGO /SUBSYSTEM:windows /INCREMENTAL:yes\
 /PDB:"WinDebug\TMP.pdb" /DEBUG /MACHINE:I386 /OUT:$(OUTDIR)/"diblook.exe" 
DEF_FILE=
LINK32_OBJS= \
	.\WinDebug\stdafx.obj \
	.\WinDebug\mainfrm.obj \
	.\WinDebug\dibview.obj \
	.\WinDebug\diblook.obj \
	.\WinDebug\dibdoc.obj \
	.\WinDebug\diblook.res \
	.\WinDebug\dibapi.obj \
	.\WinDebug\myfile.obj

.\WinDebug\diblook.exe : $(OUTDIR)  $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "Win32 (80x86) Release"

# PROP BASE Use_MFC 5
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "WinRel"
# PROP BASE Intermediate_Dir "WinRel"
# PROP Use_MFC 5
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "WinRel"
# PROP Intermediate_Dir "WinRel"
OUTDIR=.\WinRel
INTDIR=.\WinRel

ALL : .\WinRel\diblook.exe .\WinRel\diblook.bsc

$(OUTDIR) : 
    if not exist $(OUTDIR)/nul mkdir $(OUTDIR)

MTL=MkTypLib.exe
MTL_PROJ=
CPP=cl.exe
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /FR /Yu"stdafx.h" /c
# ADD CPP /nologo /MT /W3 /GX /O2 /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /FR /Yu"stdafx.h" /c
CPP_PROJ=/nologo /MT /W3 /GX /O2 /D "NDEBUG" /D "_WINDOWS" /D "_MBCS"\
 /FR$(INTDIR)/ /Fp$(OUTDIR)/"diblook.pch" /Yu"stdafx.h" /Fo$(INTDIR)/ /c 
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
RSC_PROJ=/l 0x409 /fo$(INTDIR)/"diblook.res" /d "NDEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o$(OUTDIR)/"diblook.bsc" 
BSC32_SBRS= \
	.\WinRel\stdafx.sbr \
	.\WinRel\mainfrm.sbr \
	.\WinRel\dibview.sbr \
	.\WinRel\diblook.sbr \
	.\WinRel\dibdoc.sbr \
	.\WinRel\dibapi.sbr \
	.\WinRel\myfile.sbr

.\WinRel\diblook.bsc : $(OUTDIR)  $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib nafxcw.lib ctl3d32.lib /NOLOGO /SUBSYSTEM:windows /PDB:"WinRel\TMP.pdb" /MACHINE:I386
# SUBTRACT BASE LINK32 /PDB:none
# ADD LINK32 /NOLOGO /SUBSYSTEM:windows /PDB:"WinRel\TMP.pdb" /MACHINE:I386
# SUBTRACT LINK32 /PDB:none
LINK32_FLAGS=/NOLOGO /SUBSYSTEM:windows /INCREMENTAL:no /PDB:"WinRel\TMP.pdb"\
 /MACHINE:I386 /OUT:$(OUTDIR)/"diblook.exe" 
DEF_FILE=
LINK32_OBJS= \
	.\WinRel\stdafx.obj \
	.\WinRel\mainfrm.obj \
	.\WinRel\dibview.obj \
	.\WinRel\diblook.obj \
	.\WinRel\dibdoc.obj \
	.\WinRel\diblook.res \
	.\WinRel\dibapi.obj \
	.\WinRel\myfile.obj

.\WinRel\diblook.exe : $(OUTDIR)  $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "Macintosh Debug"

# PROP BASE Use_MFC 5
# PROP BASE Output_Dir "MacDebug"
# PROP BASE Intermediate_Dir "MacDebug"
# PROP Use_MFC 5
# PROP Output_Dir "MacDebug"
# PROP Intermediate_Dir "MacDebug"
OUTDIR=.\MacDebug
INTDIR=.\MacDebug

ALL : .\MacDebug\diblook.exe .\MacDebug\diblook.map .\MacDebug\diblook.bsc

$(OUTDIR) : 
    if not exist $(OUTDIR)/nul mkdir $(OUTDIR)

MTL=MkTypLib.exe
MTL_PROJ=
CPP=cl.exe
# ADD BASE CPP /nologo /AL /Gt1 /Q68s /Q68m /W3 /GX /Zi /Od /D "_WINDOWS" /D "_DEBUG" /D "_MBCS" /D "_MAC" /D "_68K_" /FR /Yu"stdafx.h" /c
# ADD CPP /nologo /AL /Gt1 /Q68s /Q68m /W3 /GX /Zi /Od /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_MAC" /D "_68K_" /FR /Yu"stdafx.h" /c
CPP_PROJ=/nologo /AL /Gt1 /Q68s /Q68m /W3 /GX /Zi /Od /D "_DEBUG" /D "_WINDOWS"\
 /D "_MBCS" /D "_MAC" /D "_68K_" /FR$(INTDIR)/ /Fp$(OUTDIR)/"diblook.pch"\
 /Yu"stdafx.h" /Fo$(INTDIR)/ /Fd$(OUTDIR)/"diblook.pdb" /c 
CPP_OBJS=.\MacDebug/

.c{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

RSC=rc.exe
# ADD BASE RSC /r /d "_DEBUG" /d "_MAC" /d "_68K_"
# ADD RSC /r /d "_DEBUG" /d "_MAC" /d "_68K_"
RSC_PROJ=/r /fo$(INTDIR)/"diblook.rsc" /d "_DEBUG" /d "_MAC" /d "_68K_" 
MRC=mrc.exe
# ADD BASE MRC /D "_MAC" /D "_DEBUG"
# ADD MRC /D "_MAC" /D "_DEBUG"
MRC_PROJ=/D "_MAC" /D "_DEBUG" 
MFILE32=mfile.exe
# ADD BASE MFILE32 COPY /NOLOGO
# ADD MFILE32 COPY /NOLOGO
MFILE32_FLAGS=COPY /NOLOGO 
MFILE32_FILES= \
	.\MacDebug\diblook.exe

DOWNLOAD :  $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) $(MFILE32_FILES) ":$(MFILE32_DEST)"


BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o$(OUTDIR)/"diblook.bsc" 
BSC32_SBRS= \
	.\MacDebug\stdafx.sbr \
	.\MacDebug\mainfrm.sbr \
	.\MacDebug\dibview.sbr \
	.\MacDebug\diblook.sbr \
	.\MacDebug\dibdoc.sbr \
	.\MacDebug\dibapi.sbr \
	.\MacDebug\myfile.sbr

.\MacDebug\diblook.bsc : $(OUTDIR)  $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 nafxcmd.lib wlmd.lib aslmd.lib swapd.lib sanes.lib wlm.rsc commdlg.rsc /NOLOGO /MAC:bundle /MAC:creator="TMP " /PDB:"MacDebug\TMP.pdb" /MAP /DEBUG /MACHINE:M68K /NODEFAULTLIB:"swap.lib"
# SUBTRACT BASE LINK32 /PDB:none
# ADD LINK32 nafxcmd.lib wlmd.lib aslmd.lib swapd.lib sanes.lib wlm.rsc commdlg.rsc /NOLOGO /MAC:bundle /MAC:creator="DIBL " /MAP /DEBUG /MACHINE:M68K /NODEFAULTLIB:"swap.lib"
# SUBTRACT LINK32 /PDB:none
LINK32_FLAGS=nafxcmd.lib wlmd.lib aslmd.lib swapd.lib sanes.lib wlm.rsc\
 commdlg.rsc /NOLOGO /MAC:bundle /MAC:type="APPL" /MAC:creator="DIBL "\
 /PDB:$(OUTDIR)/"diblook.pdb" /MAP:$(INTDIR)/"diblook.map" /DEBUG /MACHINE:M68K\
 /NODEFAULTLIB:"swap.lib" /OUT:$(OUTDIR)/"diblook.exe" 
DEF_FILE=
LINK32_OBJS= \
	.\MacDebug\stdafx.obj \
	.\MacDebug\mainfrm.obj \
	.\MacDebug\dibview.obj \
	.\MacDebug\diblmac.rsc \
	.\MacDebug\diblook.obj \
	.\MacDebug\dibdoc.obj \
	.\MacDebug\diblook.rsc \
	.\MacDebug\dibapi.obj \
	.\MacDebug\myfile.obj

.\MacDebug\diblook.exe : $(OUTDIR)  $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "Macintosh Release"

# PROP BASE Use_MFC 5
# PROP BASE Output_Dir "MacRel"
# PROP BASE Intermediate_Dir "MacRel"
# PROP Use_MFC 5
# PROP Output_Dir "MacRel"
# PROP Intermediate_Dir "MacRel"
OUTDIR=.\MacRel
INTDIR=.\MacRel

ALL : .\MacRel\diblook.exe .\MacRel\diblook.map .\MacRel\diblook.bsc

$(OUTDIR) : 
    if not exist $(OUTDIR)/nul mkdir $(OUTDIR)

MTL=MkTypLib.exe
MTL_PROJ=
CPP=cl.exe
# ADD BASE CPP /nologo /AL /Gt1 /Q68s /W3 /GX /O2 /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_MAC" /D "_68K_" /FR /Yu"stdafx.h" /c
# ADD CPP /nologo /AL /Gt1 /Q68s /W3 /GX /O2 /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_MAC" /D "_68K_" /FR /Yu"stdafx.h" /c
CPP_PROJ=/nologo /AL /Gt1 /Q68s /W3 /GX /O2 /D "NDEBUG" /D "_WINDOWS" /D\
 "_MBCS" /D "_MAC" /D "_68K_" /FR$(INTDIR)/ /Fp$(OUTDIR)/"diblook.pch"\
 /Yu"stdafx.h" /Fo$(INTDIR)/ /c 
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
RSC_PROJ=/r /fo$(INTDIR)/"diblook.rsc" /d "_MAC" /d "_68K_" /d "NDEBUG" 
MRC=mrc.exe
# ADD BASE MRC /D "_MAC" /D "NDEBUG"
# ADD MRC /D "_MAC" /D "NDEBUG"
MRC_PROJ=/D "_MAC" /D "NDEBUG" 
MFILE32=mfile.exe
# ADD BASE MFILE32 COPY /NOLOGO
# ADD MFILE32 COPY /NOLOGO
MFILE32_FLAGS=COPY /NOLOGO 
MFILE32_FILES= \
	.\MacRel\diblook.exe

DOWNLOAD :  $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) $(MFILE32_FILES) ":$(MFILE32_DEST)"


BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o$(OUTDIR)/"diblook.bsc" 
BSC32_SBRS= \
	.\MacRel\stdafx.sbr \
	.\MacRel\mainfrm.sbr \
	.\MacRel\dibview.sbr \
	.\MacRel\diblook.sbr \
	.\MacRel\dibdoc.sbr \
	.\MacRel\dibapi.sbr \
	.\MacRel\myfile.sbr

.\MacRel\diblook.bsc : $(OUTDIR)  $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 nafxcm.lib wlm.lib aslm.lib swap.lib sanes.lib wlm.rsc commdlg.rsc /NOLOGO /MAC:bundle /MAC:creator="TMP " /PDB:"MacRel\TMP.pdb" /MAP /MACHINE:M68K /NODEFAULTLIB:"swapd.lib"
# SUBTRACT BASE LINK32 /PDB:none
# ADD LINK32 nafxcm.lib wlm.lib aslm.lib swap.lib sanes.lib wlm.rsc commdlg.rsc /NOLOGO /MAC:bundle /MAC:creator="TMP " /PDB:"MacRel\TMP.pdb" /MAP /MACHINE:M68K /NODEFAULTLIB:"swapd.lib"
# SUBTRACT LINK32 /PDB:none
LINK32_FLAGS=nafxcm.lib wlm.lib aslm.lib swap.lib sanes.lib wlm.rsc commdlg.rsc\
 /NOLOGO /MAC:bundle /MAC:type="APPL" /MAC:creator="TMP " /PDB:"MacRel\TMP.pdb"\
 /MAP:$(INTDIR)/"diblook.map" /MACHINE:M68K /NODEFAULTLIB:"swapd.lib"\
 /OUT:$(OUTDIR)/"diblook.exe" 
DEF_FILE=
LINK32_OBJS= \
	.\MacRel\stdafx.obj \
	.\MacRel\mainfrm.obj \
	.\MacRel\dibview.obj \
	.\MacRel\diblmac.rsc \
	.\MacRel\diblook.obj \
	.\MacRel\dibdoc.obj \
	.\MacRel\diblook.rsc \
	.\MacRel\dibapi.obj \
	.\MacRel\myfile.obj

.\MacRel\diblook.exe : $(OUTDIR)  $(DEF_FILE) $(LINK32_OBJS)
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

ALL : .\WinRel\diblook.exe .\WinRel\diblook.bsc

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
 "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"diblook.pch"\
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
RSC_PROJ=/l 0x409 /fo$(INTDIR)/"diblook.res" /d "NDEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o$(OUTDIR)/"diblook.bsc" 
BSC32_SBRS= \
	.\WinRel\stdafx.sbr \
	.\WinRel\mainfrm.sbr \
	.\WinRel\dibview.sbr \
	.\WinRel\diblook.sbr \
	.\WinRel\dibdoc.sbr \
	.\WinRel\dibapi.sbr \
	.\WinRel\myfile.sbr

.\WinRel\diblook.bsc : $(OUTDIR)  $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 /NOLOGO /SUBSYSTEM:windows /MACHINE:MIPS
# ADD LINK32 /NOLOGO /SUBSYSTEM:windows /MACHINE:MIPS
LINK32_FLAGS=/NOLOGO /SUBSYSTEM:windows /PDB:$(OUTDIR)/"diblook.pdb"\
 /MACHINE:MIPS /OUT:$(OUTDIR)/"diblook.exe" 
DEF_FILE=
LINK32_OBJS= \
	.\WinRel\stdafx.obj \
	.\WinRel\mainfrm.obj \
	.\WinRel\dibview.obj \
	.\WinRel\diblook.obj \
	.\WinRel\dibdoc.obj \
	.\WinRel\diblook.res \
	.\WinRel\dibapi.obj \
	.\WinRel\myfile.obj

.\WinRel\diblook.exe : $(OUTDIR)  $(DEF_FILE) $(LINK32_OBJS)
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

ALL : .\WinDebug\diblook.exe .\WinDebug\diblook.bsc

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
 "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"diblook.pch"\
 /Yu"stdafx.h" /Fo$(INTDIR)/ /Fd$(OUTDIR)/"diblook.pdb" /c 
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
RSC_PROJ=/l 0x409 /fo$(INTDIR)/"diblook.res" /d "_DEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o$(OUTDIR)/"diblook.bsc" 
BSC32_SBRS= \
	.\WinDebug\stdafx.sbr \
	.\WinDebug\mainfrm.sbr \
	.\WinDebug\dibview.sbr \
	.\WinDebug\diblook.sbr \
	.\WinDebug\dibdoc.sbr \
	.\WinDebug\dibapi.sbr \
	.\WinDebug\myfile.sbr

.\WinDebug\diblook.bsc : $(OUTDIR)  $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 /NOLOGO /SUBSYSTEM:windows /DEBUG /MACHINE:MIPS
# ADD LINK32 /NOLOGO /SUBSYSTEM:windows /DEBUG /MACHINE:MIPS
LINK32_FLAGS=/NOLOGO /SUBSYSTEM:windows /PDB:$(OUTDIR)/"diblook.pdb" /DEBUG\
 /MACHINE:MIPS /OUT:$(OUTDIR)/"diblook.exe" 
DEF_FILE=
LINK32_OBJS= \
	.\WinDebug\stdafx.obj \
	.\WinDebug\mainfrm.obj \
	.\WinDebug\dibview.obj \
	.\WinDebug\diblook.obj \
	.\WinDebug\dibdoc.obj \
	.\WinDebug\diblook.res \
	.\WinDebug\dibapi.obj \
	.\WinDebug\myfile.obj

.\WinDebug\diblook.exe : $(OUTDIR)  $(DEF_FILE) $(LINK32_OBJS)
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

!IF  "$(CFG)" == "Win32 (80x86) Debug"

# ADD BASE CPP /Yc"stdafx.h"
# ADD CPP /Yc"stdafx.h"

.\WinDebug\stdafx.obj :  $(SOURCE)  $(DEP_STDAF) $(INTDIR)
   $(CPP) /nologo /MD /W3 /GX /Zi /Od /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D\
 "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"diblook.pch" /Yc"stdafx.h" /Fo$(INTDIR)/\
 /Fd$(OUTDIR)/"diblook.pdb" /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (80x86) Release"

# ADD BASE CPP /Yc"stdafx.h"
# ADD CPP /Yc"stdafx.h"

.\WinRel\stdafx.obj :  $(SOURCE)  $(DEP_STDAF) $(INTDIR)
   $(CPP) /nologo /MT /W3 /GX /O2 /D "NDEBUG" /D "_WINDOWS" /D "_MBCS"\
 /FR$(INTDIR)/ /Fp$(OUTDIR)/"diblook.pch" /Yc"stdafx.h" /Fo$(INTDIR)/ /c\
  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Macintosh Debug"

# ADD BASE CPP /Gt1 /Yc"stdafx.h"
# ADD CPP /Gt1 /Yc"stdafx.h"

.\MacDebug\stdafx.obj :  $(SOURCE)  $(DEP_STDAF) $(INTDIR)
   $(CPP) /nologo /AL /Gt1 /Q68s /Q68m /W3 /GX /Zi /Od /D "_DEBUG" /D\
 "_WINDOWS" /D "_MBCS" /D "_MAC" /D "_68K_" /FR$(INTDIR)/\
 /Fp$(OUTDIR)/"diblook.pch" /Yc"stdafx.h" /Fo$(INTDIR)/\
 /Fd$(OUTDIR)/"diblook.pdb" /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Macintosh Release"

# ADD BASE CPP /Gt1 /Yc"stdafx.h"
# ADD CPP /Gt1 /Yc"stdafx.h"

.\MacRel\stdafx.obj :  $(SOURCE)  $(DEP_STDAF) $(INTDIR)
   $(CPP) /nologo /AL /Gt1 /Q68s /W3 /GX /O2 /D "NDEBUG" /D "_WINDOWS" /D\
 "_MBCS" /D "_MAC" /D "_68K_" /FR$(INTDIR)/ /Fp$(OUTDIR)/"diblook.pch"\
 /Yc"stdafx.h" /Fo$(INTDIR)/ /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

# ADD BASE CPP /Gt0 /QMOb2000
# ADD CPP /Gt0 /QMOb2000 /Yc"stdafx.h"

.\WinRel\stdafx.obj :  $(SOURCE)  $(DEP_STDAF) $(INTDIR)
   $(CPP) /nologo /MD /Gt0 /QMOb2000 /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D\
 "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"diblook.pch"\
 /Yc"stdafx.h" /Fo$(INTDIR)/ /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

# ADD BASE CPP /Gt0 /QMOb2000
# ADD CPP /Gt0 /QMOb2000 /Yc"stdafx.h"

.\WinDebug\stdafx.obj :  $(SOURCE)  $(DEP_STDAF) $(INTDIR)
   $(CPP) /nologo /MD /Gt0 /QMOb2000 /W3 /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D\
 "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"diblook.pch"\
 /Yc"stdafx.h" /Fo$(INTDIR)/ /Fd$(OUTDIR)/"diblook.pdb" /c  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\mainfrm.cpp
DEP_MAINF=\
	.\stdafx.h\
	.\diblook.h\
	.\mainfrm.h

!IF  "$(CFG)" == "Win32 (80x86) Debug"

.\WinDebug\mainfrm.obj :  $(SOURCE)  $(DEP_MAINF) $(INTDIR)\
 .\WinDebug\stdafx.obj
   $(CPP) /nologo /MD /W3 /GX /Zi /Od /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D\
 "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"diblook.pch" /Yu"stdafx.h" /Fo$(INTDIR)/\
 /Fd$(OUTDIR)/"diblook.pdb" /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (80x86) Release"

.\WinRel\mainfrm.obj :  $(SOURCE)  $(DEP_MAINF) $(INTDIR) .\WinRel\stdafx.obj
   $(CPP) /nologo /MT /W3 /GX /O2 /D "NDEBUG" /D "_WINDOWS" /D "_MBCS"\
 /FR$(INTDIR)/ /Fp$(OUTDIR)/"diblook.pch" /Yu"stdafx.h" /Fo$(INTDIR)/ /c\
  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Macintosh Debug"

.\MacDebug\mainfrm.obj :  $(SOURCE)  $(DEP_MAINF) $(INTDIR)\
 .\MacDebug\stdafx.obj
   $(CPP) /nologo /AL /Gt1 /Q68s /Q68m /W3 /GX /Zi /Od /D "_DEBUG" /D\
 "_WINDOWS" /D "_MBCS" /D "_MAC" /D "_68K_" /FR$(INTDIR)/\
 /Fp$(OUTDIR)/"diblook.pch" /Yu"stdafx.h" /Fo$(INTDIR)/\
 /Fd$(OUTDIR)/"diblook.pdb" /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Macintosh Release"

.\MacRel\mainfrm.obj :  $(SOURCE)  $(DEP_MAINF) $(INTDIR) .\MacRel\stdafx.obj
   $(CPP) /nologo /AL /Gt1 /Q68s /W3 /GX /O2 /D "NDEBUG" /D "_WINDOWS" /D\
 "_MBCS" /D "_MAC" /D "_68K_" /FR$(INTDIR)/ /Fp$(OUTDIR)/"diblook.pch"\
 /Yu"stdafx.h" /Fo$(INTDIR)/ /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

# ADD BASE CPP /Gt0 /QMOb2000
# ADD CPP /Gt0 /QMOb2000 /Yu"stdafx.h"

.\WinRel\mainfrm.obj :  $(SOURCE)  $(DEP_MAINF) $(INTDIR) .\WinRel\stdafx.obj
   $(CPP) /nologo /MD /Gt0 /QMOb2000 /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D\
 "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"diblook.pch"\
 /Yu"stdafx.h" /Fo$(INTDIR)/ /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

# ADD BASE CPP /Gt0 /QMOb2000
# ADD CPP /Gt0 /QMOb2000 /Yu"stdafx.h"

.\WinDebug\mainfrm.obj :  $(SOURCE)  $(DEP_MAINF) $(INTDIR)\
 .\WinDebug\stdafx.obj
   $(CPP) /nologo /MD /Gt0 /QMOb2000 /W3 /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D\
 "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"diblook.pch"\
 /Yu"stdafx.h" /Fo$(INTDIR)/ /Fd$(OUTDIR)/"diblook.pdb" /c  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\readme.txt
# End Source File
################################################################################
# Begin Source File

SOURCE=.\dibview.cpp
DEP_DIBVI=\
	.\stdafx.h\
	.\diblook.h\
	.\dibdoc.h\
	.\dibview.h\
	.\dibapi.h\
	.\mainfrm.h

!IF  "$(CFG)" == "Win32 (80x86) Debug"

.\WinDebug\dibview.obj :  $(SOURCE)  $(DEP_DIBVI) $(INTDIR)\
 .\WinDebug\stdafx.obj
   $(CPP) /nologo /MD /W3 /GX /Zi /Od /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D\
 "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"diblook.pch" /Yu"stdafx.h" /Fo$(INTDIR)/\
 /Fd$(OUTDIR)/"diblook.pdb" /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (80x86) Release"

.\WinRel\dibview.obj :  $(SOURCE)  $(DEP_DIBVI) $(INTDIR) .\WinRel\stdafx.obj
   $(CPP) /nologo /MT /W3 /GX /O2 /D "NDEBUG" /D "_WINDOWS" /D "_MBCS"\
 /FR$(INTDIR)/ /Fp$(OUTDIR)/"diblook.pch" /Yu"stdafx.h" /Fo$(INTDIR)/ /c\
  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Macintosh Debug"

.\MacDebug\dibview.obj :  $(SOURCE)  $(DEP_DIBVI) $(INTDIR)\
 .\MacDebug\stdafx.obj
   $(CPP) /nologo /AL /Gt1 /Q68s /Q68m /W3 /GX /Zi /Od /D "_DEBUG" /D\
 "_WINDOWS" /D "_MBCS" /D "_MAC" /D "_68K_" /FR$(INTDIR)/\
 /Fp$(OUTDIR)/"diblook.pch" /Yu"stdafx.h" /Fo$(INTDIR)/\
 /Fd$(OUTDIR)/"diblook.pdb" /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Macintosh Release"

.\MacRel\dibview.obj :  $(SOURCE)  $(DEP_DIBVI) $(INTDIR) .\MacRel\stdafx.obj
   $(CPP) /nologo /AL /Gt1 /Q68s /W3 /GX /O2 /D "NDEBUG" /D "_WINDOWS" /D\
 "_MBCS" /D "_MAC" /D "_68K_" /FR$(INTDIR)/ /Fp$(OUTDIR)/"diblook.pch"\
 /Yu"stdafx.h" /Fo$(INTDIR)/ /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

# ADD BASE CPP /Gt0 /QMOb2000
# ADD CPP /Gt0 /QMOb2000 /Yu"stdafx.h"

.\WinRel\dibview.obj :  $(SOURCE)  $(DEP_DIBVI) $(INTDIR) .\WinRel\stdafx.obj
   $(CPP) /nologo /MD /Gt0 /QMOb2000 /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D\
 "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"diblook.pch"\
 /Yu"stdafx.h" /Fo$(INTDIR)/ /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

# ADD BASE CPP /Gt0 /QMOb2000
# ADD CPP /Gt0 /QMOb2000 /Yu"stdafx.h"

.\WinDebug\dibview.obj :  $(SOURCE)  $(DEP_DIBVI) $(INTDIR)\
 .\WinDebug\stdafx.obj
   $(CPP) /nologo /MD /Gt0 /QMOb2000 /W3 /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D\
 "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"diblook.pch"\
 /Yu"stdafx.h" /Fo$(INTDIR)/ /Fd$(OUTDIR)/"diblook.pdb" /c  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\diblmac.r

!IF  "$(CFG)" == "Win32 (80x86) Debug"

!ELSEIF  "$(CFG)" == "Win32 (80x86) Release"

!ELSEIF  "$(CFG)" == "Macintosh Debug"

.\MacDebug\diblmac.rsc :  $(SOURCE)  $(INTDIR)
   $(MRC) /o$(INTDIR)/"diblmac.rsc" /D "_MAC" /D "_DEBUG"  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Macintosh Release"

.\MacRel\diblmac.rsc :  $(SOURCE)  $(INTDIR)
   $(MRC) /o$(INTDIR)/"diblmac.rsc" /D "_MAC" /D "NDEBUG"  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\diblook.cpp
DEP_DIBLO=\
	.\stdafx.h\
	.\diblook.h\
	.\mainfrm.h\
	.\dibdoc.h\
	.\dibview.h\
	.\dibapi.h

!IF  "$(CFG)" == "Win32 (80x86) Debug"

.\WinDebug\diblook.obj :  $(SOURCE)  $(DEP_DIBLO) $(INTDIR)\
 .\WinDebug\stdafx.obj
   $(CPP) /nologo /MD /W3 /GX /Zi /Od /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D\
 "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"diblook.pch" /Yu"stdafx.h" /Fo$(INTDIR)/\
 /Fd$(OUTDIR)/"diblook.pdb" /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (80x86) Release"

.\WinRel\diblook.obj :  $(SOURCE)  $(DEP_DIBLO) $(INTDIR) .\WinRel\stdafx.obj
   $(CPP) /nologo /MT /W3 /GX /O2 /D "NDEBUG" /D "_WINDOWS" /D "_MBCS"\
 /FR$(INTDIR)/ /Fp$(OUTDIR)/"diblook.pch" /Yu"stdafx.h" /Fo$(INTDIR)/ /c\
  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Macintosh Debug"

.\MacDebug\diblook.obj :  $(SOURCE)  $(DEP_DIBLO) $(INTDIR)\
 .\MacDebug\stdafx.obj
   $(CPP) /nologo /AL /Gt1 /Q68s /Q68m /W3 /GX /Zi /Od /D "_DEBUG" /D\
 "_WINDOWS" /D "_MBCS" /D "_MAC" /D "_68K_" /FR$(INTDIR)/\
 /Fp$(OUTDIR)/"diblook.pch" /Yu"stdafx.h" /Fo$(INTDIR)/\
 /Fd$(OUTDIR)/"diblook.pdb" /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Macintosh Release"

.\MacRel\diblook.obj :  $(SOURCE)  $(DEP_DIBLO) $(INTDIR) .\MacRel\stdafx.obj
   $(CPP) /nologo /AL /Gt1 /Q68s /W3 /GX /O2 /D "NDEBUG" /D "_WINDOWS" /D\
 "_MBCS" /D "_MAC" /D "_68K_" /FR$(INTDIR)/ /Fp$(OUTDIR)/"diblook.pch"\
 /Yu"stdafx.h" /Fo$(INTDIR)/ /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

# ADD BASE CPP /Gt0 /QMOb2000
# ADD CPP /Gt0 /QMOb2000 /Yu"stdafx.h"

.\WinRel\diblook.obj :  $(SOURCE)  $(DEP_DIBLO) $(INTDIR) .\WinRel\stdafx.obj
   $(CPP) /nologo /MD /Gt0 /QMOb2000 /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D\
 "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"diblook.pch"\
 /Yu"stdafx.h" /Fo$(INTDIR)/ /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

# ADD BASE CPP /Gt0 /QMOb2000
# ADD CPP /Gt0 /QMOb2000 /Yu"stdafx.h"

.\WinDebug\diblook.obj :  $(SOURCE)  $(DEP_DIBLO) $(INTDIR)\
 .\WinDebug\stdafx.obj
   $(CPP) /nologo /MD /Gt0 /QMOb2000 /W3 /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D\
 "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"diblook.pch"\
 /Yu"stdafx.h" /Fo$(INTDIR)/ /Fd$(OUTDIR)/"diblook.pdb" /c  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\dibdoc.cpp
DEP_DIBDO=\
	.\stdafx.h\
	.\diblook.h\
	.\dibdoc.h\
	.\dibapi.h

!IF  "$(CFG)" == "Win32 (80x86) Debug"

.\WinDebug\dibdoc.obj :  $(SOURCE)  $(DEP_DIBDO) $(INTDIR)\
 .\WinDebug\stdafx.obj
   $(CPP) /nologo /MD /W3 /GX /Zi /Od /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D\
 "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"diblook.pch" /Yu"stdafx.h" /Fo$(INTDIR)/\
 /Fd$(OUTDIR)/"diblook.pdb" /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (80x86) Release"

.\WinRel\dibdoc.obj :  $(SOURCE)  $(DEP_DIBDO) $(INTDIR) .\WinRel\stdafx.obj
   $(CPP) /nologo /MT /W3 /GX /O2 /D "NDEBUG" /D "_WINDOWS" /D "_MBCS"\
 /FR$(INTDIR)/ /Fp$(OUTDIR)/"diblook.pch" /Yu"stdafx.h" /Fo$(INTDIR)/ /c\
  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Macintosh Debug"

.\MacDebug\dibdoc.obj :  $(SOURCE)  $(DEP_DIBDO) $(INTDIR)\
 .\MacDebug\stdafx.obj
   $(CPP) /nologo /AL /Gt1 /Q68s /Q68m /W3 /GX /Zi /Od /D "_DEBUG" /D\
 "_WINDOWS" /D "_MBCS" /D "_MAC" /D "_68K_" /FR$(INTDIR)/\
 /Fp$(OUTDIR)/"diblook.pch" /Yu"stdafx.h" /Fo$(INTDIR)/\
 /Fd$(OUTDIR)/"diblook.pdb" /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Macintosh Release"

.\MacRel\dibdoc.obj :  $(SOURCE)  $(DEP_DIBDO) $(INTDIR) .\MacRel\stdafx.obj
   $(CPP) /nologo /AL /Gt1 /Q68s /W3 /GX /O2 /D "NDEBUG" /D "_WINDOWS" /D\
 "_MBCS" /D "_MAC" /D "_68K_" /FR$(INTDIR)/ /Fp$(OUTDIR)/"diblook.pch"\
 /Yu"stdafx.h" /Fo$(INTDIR)/ /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

# ADD BASE CPP /Gt0 /QMOb2000
# ADD CPP /Gt0 /QMOb2000 /Yu"stdafx.h"

.\WinRel\dibdoc.obj :  $(SOURCE)  $(DEP_DIBDO) $(INTDIR) .\WinRel\stdafx.obj
   $(CPP) /nologo /MD /Gt0 /QMOb2000 /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D\
 "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"diblook.pch"\
 /Yu"stdafx.h" /Fo$(INTDIR)/ /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

# ADD BASE CPP /Gt0 /QMOb2000
# ADD CPP /Gt0 /QMOb2000 /Yu"stdafx.h"

.\WinDebug\dibdoc.obj :  $(SOURCE)  $(DEP_DIBDO) $(INTDIR)\
 .\WinDebug\stdafx.obj
   $(CPP) /nologo /MD /Gt0 /QMOb2000 /W3 /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D\
 "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"diblook.pch"\
 /Yu"stdafx.h" /Fo$(INTDIR)/ /Fd$(OUTDIR)/"diblook.pdb" /c  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\diblook.rc
DEP_DIBLOO=\
	.\res\diblook.ico\
	.\res\dibdoc.ico\
	.\res\toolbar.bmp

!IF  "$(CFG)" == "Win32 (80x86) Debug"

.\WinDebug\diblook.res :  $(SOURCE)  $(DEP_DIBLOO) $(INTDIR)
   $(RSC) $(RSC_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (80x86) Release"

.\WinRel\diblook.res :  $(SOURCE)  $(DEP_DIBLOO) $(INTDIR)
   $(RSC) $(RSC_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Macintosh Debug"

.\MacDebug\diblook.rsc :  $(SOURCE)  $(DEP_DIBLOO) $(INTDIR)
   $(RSC) $(RSC_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Macintosh Release"

.\MacRel\diblook.rsc :  $(SOURCE)  $(DEP_DIBLOO) $(INTDIR)
   $(RSC) $(RSC_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

.\WinRel\diblook.res :  $(SOURCE)  $(DEP_DIBLOO) $(INTDIR)
   $(RSC) $(RSC_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

.\WinDebug\diblook.res :  $(SOURCE)  $(DEP_DIBLOO) $(INTDIR)
   $(RSC) $(RSC_PROJ)  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\dibapi.cpp
DEP_DIBAP=\
	.\stdafx.h\
	.\dibapi.h

!IF  "$(CFG)" == "Win32 (80x86) Debug"

.\WinDebug\dibapi.obj :  $(SOURCE)  $(DEP_DIBAP) $(INTDIR)\
 .\WinDebug\stdafx.obj
   $(CPP) /nologo /MD /W3 /GX /Zi /Od /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D\
 "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"diblook.pch" /Yu"stdafx.h" /Fo$(INTDIR)/\
 /Fd$(OUTDIR)/"diblook.pdb" /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (80x86) Release"

.\WinRel\dibapi.obj :  $(SOURCE)  $(DEP_DIBAP) $(INTDIR) .\WinRel\stdafx.obj
   $(CPP) /nologo /MT /W3 /GX /O2 /D "NDEBUG" /D "_WINDOWS" /D "_MBCS"\
 /FR$(INTDIR)/ /Fp$(OUTDIR)/"diblook.pch" /Yu"stdafx.h" /Fo$(INTDIR)/ /c\
  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Macintosh Debug"

.\MacDebug\dibapi.obj :  $(SOURCE)  $(DEP_DIBAP) $(INTDIR)\
 .\MacDebug\stdafx.obj
   $(CPP) /nologo /AL /Gt1 /Q68s /Q68m /W3 /GX /Zi /Od /D "_DEBUG" /D\
 "_WINDOWS" /D "_MBCS" /D "_MAC" /D "_68K_" /FR$(INTDIR)/\
 /Fp$(OUTDIR)/"diblook.pch" /Yu"stdafx.h" /Fo$(INTDIR)/\
 /Fd$(OUTDIR)/"diblook.pdb" /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Macintosh Release"

.\MacRel\dibapi.obj :  $(SOURCE)  $(DEP_DIBAP) $(INTDIR) .\MacRel\stdafx.obj
   $(CPP) /nologo /AL /Gt1 /Q68s /W3 /GX /O2 /D "NDEBUG" /D "_WINDOWS" /D\
 "_MBCS" /D "_MAC" /D "_68K_" /FR$(INTDIR)/ /Fp$(OUTDIR)/"diblook.pch"\
 /Yu"stdafx.h" /Fo$(INTDIR)/ /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

# ADD BASE CPP /Gt0 /QMOb2000
# ADD CPP /Gt0 /QMOb2000 /Yu"stdafx.h"

.\WinRel\dibapi.obj :  $(SOURCE)  $(DEP_DIBAP) $(INTDIR) .\WinRel\stdafx.obj
   $(CPP) /nologo /MD /Gt0 /QMOb2000 /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D\
 "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"diblook.pch"\
 /Yu"stdafx.h" /Fo$(INTDIR)/ /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

# ADD BASE CPP /Gt0 /QMOb2000
# ADD CPP /Gt0 /QMOb2000 /Yu"stdafx.h"

.\WinDebug\dibapi.obj :  $(SOURCE)  $(DEP_DIBAP) $(INTDIR)\
 .\WinDebug\stdafx.obj
   $(CPP) /nologo /MD /Gt0 /QMOb2000 /W3 /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D\
 "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"diblook.pch"\
 /Yu"stdafx.h" /Fo$(INTDIR)/ /Fd$(OUTDIR)/"diblook.pdb" /c  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\myfile.cpp
DEP_MYFIL=\
	.\stdafx.h\
	.\dibapi.h

!IF  "$(CFG)" == "Win32 (80x86) Debug"

.\WinDebug\myfile.obj :  $(SOURCE)  $(DEP_MYFIL) $(INTDIR)\
 .\WinDebug\stdafx.obj
   $(CPP) /nologo /MD /W3 /GX /Zi /Od /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D\
 "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"diblook.pch" /Yu"stdafx.h" /Fo$(INTDIR)/\
 /Fd$(OUTDIR)/"diblook.pdb" /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (80x86) Release"

.\WinRel\myfile.obj :  $(SOURCE)  $(DEP_MYFIL) $(INTDIR) .\WinRel\stdafx.obj
   $(CPP) /nologo /MT /W3 /GX /O2 /D "NDEBUG" /D "_WINDOWS" /D "_MBCS"\
 /FR$(INTDIR)/ /Fp$(OUTDIR)/"diblook.pch" /Yu"stdafx.h" /Fo$(INTDIR)/ /c\
  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Macintosh Debug"

.\MacDebug\myfile.obj :  $(SOURCE)  $(DEP_MYFIL) $(INTDIR)\
 .\MacDebug\stdafx.obj
   $(CPP) /nologo /AL /Gt1 /Q68s /Q68m /W3 /GX /Zi /Od /D "_DEBUG" /D\
 "_WINDOWS" /D "_MBCS" /D "_MAC" /D "_68K_" /FR$(INTDIR)/\
 /Fp$(OUTDIR)/"diblook.pch" /Yu"stdafx.h" /Fo$(INTDIR)/\
 /Fd$(OUTDIR)/"diblook.pdb" /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Macintosh Release"

.\MacRel\myfile.obj :  $(SOURCE)  $(DEP_MYFIL) $(INTDIR) .\MacRel\stdafx.obj
   $(CPP) /nologo /AL /Gt1 /Q68s /W3 /GX /O2 /D "NDEBUG" /D "_WINDOWS" /D\
 "_MBCS" /D "_MAC" /D "_68K_" /FR$(INTDIR)/ /Fp$(OUTDIR)/"diblook.pch"\
 /Yu"stdafx.h" /Fo$(INTDIR)/ /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

# ADD BASE CPP /Gt0 /QMOb2000
# ADD CPP /Gt0 /QMOb2000 /Yu"stdafx.h"

.\WinRel\myfile.obj :  $(SOURCE)  $(DEP_MYFIL) $(INTDIR) .\WinRel\stdafx.obj
   $(CPP) /nologo /MD /Gt0 /QMOb2000 /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D\
 "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"diblook.pch"\
 /Yu"stdafx.h" /Fo$(INTDIR)/ /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

# ADD BASE CPP /Gt0 /QMOb2000
# ADD CPP /Gt0 /QMOb2000 /Yu"stdafx.h"

.\WinDebug\myfile.obj :  $(SOURCE)  $(DEP_MYFIL) $(INTDIR)\
 .\WinDebug\stdafx.obj
   $(CPP) /nologo /MD /Gt0 /QMOb2000 /W3 /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D\
 "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"diblook.pch"\
 /Yu"stdafx.h" /Fo$(INTDIR)/ /Fd$(OUTDIR)/"diblook.pdb" /c  $(SOURCE) 

!ENDIF 

# End Source File
# End Group
# End Project
################################################################################
