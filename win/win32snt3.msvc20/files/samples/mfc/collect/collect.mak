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
!MESSAGE NMAKE /f "collect.mak" CFG="Win32 (80x86) Debug"
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

# PROP BASE Use_MFC 1
# PROP BASE Output_Dir "WinDebug"
# PROP BASE Intermediate_Dir "WinDebug"
# PROP Use_MFC 2
# PROP Output_Dir "WinDebug"
# PROP Intermediate_Dir "WinDebug"
OUTDIR=.\WinDebug
INTDIR=.\WinDebug

ALL : $(OUTDIR)/collect.exe $(OUTDIR)/collect.bsc

$(OUTDIR) : 
    if not exist $(OUTDIR)/nul mkdir $(OUTDIR)

MTL=MkTypLib.exe
MTL_PROJ=
CPP=cl.exe
# ADD BASE CPP /nologo /MT /W3 /GX /Zi /Od /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /FR /Yu"stdafx.h" /c
# ADD CPP /nologo /MD /W3 /GX /Zi /Od /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /Yu"stdafx.h" /c
CPP_PROJ=/nologo /MD /W3 /GX /Zi /Od /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D\
 "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"collect.pch" /Yu"stdafx.h" /Fo$(INTDIR)/\
 /Fd$(OUTDIR)/"collect.pdb" /c 
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
RSC_PROJ=/l 0x409 /fo$(INTDIR)/"collect.res" /d "_DEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# SUBTRACT BASE BSC32 /Iu
# ADD BSC32 /nologo
# SUBTRACT BSC32 /Iu
BSC32_FLAGS=/nologo /o$(OUTDIR)/"collect.bsc" 
BSC32_SBRS= \
	$(INTDIR)/stdafx.sbr \
	$(INTDIR)/collect.sbr \
	$(INTDIR)/mainfrm.sbr \
	$(INTDIR)/colledoc.sbr \
	$(INTDIR)/strlstvw.sbr \
	$(INTDIR)/typlstvw.sbr \
	$(INTDIR)/intlstvw.sbr \
	$(INTDIR)/dwarryvw.sbr \
	$(INTDIR)/typaryvw.sbr \
	$(INTDIR)/ptarryvw.sbr \
	$(INTDIR)/mapssvw.sbr \
	$(INTDIR)/typtrmap.sbr \
	$(INTDIR)/mapdwvw.sbr

$(OUTDIR)/collect.bsc : $(OUTDIR)  $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib nafxcwd.lib ctl3d32.lib /NOLOGO /SUBSYSTEM:windows /DEBUG /MACHINE:I386
# ADD LINK32 /NOLOGO /SUBSYSTEM:windows /DEBUG /MACHINE:I386
LINK32_FLAGS=/NOLOGO /SUBSYSTEM:windows /INCREMENTAL:no\
 /PDB:$(OUTDIR)/"collect.pdb" /DEBUG /MACHINE:I386 /OUT:$(OUTDIR)/"collect.exe" 
DEF_FILE=
LINK32_OBJS= \
	$(INTDIR)/stdafx.obj \
	$(INTDIR)/collect.obj \
	$(INTDIR)/mainfrm.obj \
	$(INTDIR)/colledoc.obj \
	$(INTDIR)/collect.res \
	$(INTDIR)/strlstvw.obj \
	$(INTDIR)/typlstvw.obj \
	$(INTDIR)/intlstvw.obj \
	$(INTDIR)/dwarryvw.obj \
	$(INTDIR)/typaryvw.obj \
	$(INTDIR)/ptarryvw.obj \
	$(INTDIR)/mapssvw.obj \
	$(INTDIR)/typtrmap.obj \
	$(INTDIR)/mapdwvw.obj

$(OUTDIR)/collect.exe : $(OUTDIR)  $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "Win32 (80x86) Release"

# PROP BASE Use_MFC 1
# PROP BASE Output_Dir "WinRel"
# PROP BASE Intermediate_Dir "WinRel"
# PROP Use_MFC 2
# PROP Output_Dir "WinRel"
# PROP Intermediate_Dir "WinRel"
OUTDIR=.\WinRel
INTDIR=.\WinRel

ALL : $(OUTDIR)/collect.exe $(OUTDIR)/collect.bsc

$(OUTDIR) : 
    if not exist $(OUTDIR)/nul mkdir $(OUTDIR)

MTL=MkTypLib.exe
MTL_PROJ=
CPP=cl.exe
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /FR /Yu"stdafx.h" /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /Yu"stdafx.h" /c
CPP_PROJ=/nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D\
 "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"collect.pch" /Yu"stdafx.h" /Fo$(INTDIR)/ /c\
 
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
RSC_PROJ=/l 0x409 /fo$(INTDIR)/"collect.res" /d "NDEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# SUBTRACT BASE BSC32 /Iu
# ADD BSC32 /nologo
# SUBTRACT BSC32 /Iu
BSC32_FLAGS=/nologo /o$(OUTDIR)/"collect.bsc" 
BSC32_SBRS= \
	$(INTDIR)/stdafx.sbr \
	$(INTDIR)/collect.sbr \
	$(INTDIR)/mainfrm.sbr \
	$(INTDIR)/colledoc.sbr \
	$(INTDIR)/strlstvw.sbr \
	$(INTDIR)/typlstvw.sbr \
	$(INTDIR)/intlstvw.sbr \
	$(INTDIR)/dwarryvw.sbr \
	$(INTDIR)/typaryvw.sbr \
	$(INTDIR)/ptarryvw.sbr \
	$(INTDIR)/mapssvw.sbr \
	$(INTDIR)/typtrmap.sbr \
	$(INTDIR)/mapdwvw.sbr

$(OUTDIR)/collect.bsc : $(OUTDIR)  $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib nafxcw.lib ctl3d32.lib /NOLOGO /SUBSYSTEM:windows /MACHINE:I386
# ADD LINK32 /NOLOGO /SUBSYSTEM:windows /MACHINE:I386
LINK32_FLAGS=/NOLOGO /SUBSYSTEM:windows /INCREMENTAL:no\
 /PDB:$(OUTDIR)/"collect.pdb" /MACHINE:I386 /OUT:$(OUTDIR)/"collect.exe" 
DEF_FILE=
LINK32_OBJS= \
	$(INTDIR)/stdafx.obj \
	$(INTDIR)/collect.obj \
	$(INTDIR)/mainfrm.obj \
	$(INTDIR)/colledoc.obj \
	$(INTDIR)/collect.res \
	$(INTDIR)/strlstvw.obj \
	$(INTDIR)/typlstvw.obj \
	$(INTDIR)/intlstvw.obj \
	$(INTDIR)/dwarryvw.obj \
	$(INTDIR)/typaryvw.obj \
	$(INTDIR)/ptarryvw.obj \
	$(INTDIR)/mapssvw.obj \
	$(INTDIR)/typtrmap.obj \
	$(INTDIR)/mapdwvw.obj

$(OUTDIR)/collect.exe : $(OUTDIR)  $(DEF_FILE) $(LINK32_OBJS)
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

ALL : $(OUTDIR)/collect.exe $(OUTDIR)/collect.bsc

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
 /D "_68K_" /D "_DEBUG" /FR$(INTDIR)/ /Fp$(OUTDIR)/"collect.pch" /Yu"stdafx.h"\
 /Fo$(INTDIR)/ /Fd$(OUTDIR)/"collect.pdb" /c 
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
RSC_PROJ=/r /fo$(INTDIR)/"collect.rsc" /d "_MAC" /d "_68K_" /d "_DEBUG" 
MRC=mrc.exe
# ADD BASE MRC /D "_MAC" /D "_68K_" /D "_DEBUG" /r
# ADD MRC /D "_MAC" /D "_68K_" /D "_DEBUG"
MRC_PROJ=/D "_MAC" /D "_68K_" /D "_DEBUG" 
MFILE32=mfile.exe
# ADD BASE MFILE32 COPY /NOLOGO
# ADD MFILE32 COPY /NOLOGO
MFILE32_FLAGS=COPY /NOLOGO 
MFILE32_FILES= \
	$(OUTDIR)/collect.exe

DOWNLOAD :  $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) $(MFILE32_FILES) ":$(MFILE32_DEST)"


BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# SUBTRACT BASE BSC32 /Iu
# ADD BSC32 /nologo
# SUBTRACT BSC32 /Iu
BSC32_FLAGS=/nologo /o$(OUTDIR)/"collect.bsc" 
BSC32_SBRS= \
	$(INTDIR)/stdafx.sbr \
	$(INTDIR)/collect.sbr \
	$(INTDIR)/mainfrm.sbr \
	$(INTDIR)/colledoc.sbr \
	$(INTDIR)/strlstvw.sbr \
	$(INTDIR)/typlstvw.sbr \
	$(INTDIR)/intlstvw.sbr \
	$(INTDIR)/dwarryvw.sbr \
	$(INTDIR)/typaryvw.sbr \
	$(INTDIR)/ptarryvw.sbr \
	$(INTDIR)/mapssvw.sbr \
	$(INTDIR)/typtrmap.sbr \
	$(INTDIR)/mapdwvw.sbr

$(OUTDIR)/collect.bsc : $(OUTDIR)  $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 swapd.lib sanes.lib wlmd.lib aslmd.lib wlm.rsc commdlg.rsc nafxcmd.lib mfcuia32.lib mfcans32.lib ole32.lib oleaut32.lib uuid.lib /NOLOGO /MAC:bundle /DEBUG /MACHINE:M68K /NODEFAULTLIB:"swap.lib"
# ADD LINK32 swapd.lib sanes.lib wlmd.lib aslmd.lib wlm.rsc commdlg.rsc nafxcmd.lib /NOLOGO /MAC:bundle /MAC:creator="COLL" /DEBUG /MACHINE:M68K /NODEFAULTLIB:"swap.lib"
LINK32_FLAGS=swapd.lib sanes.lib wlmd.lib aslmd.lib wlm.rsc commdlg.rsc\
 nafxcmd.lib /NOLOGO /MAC:bundle /MAC:type="APPL" /MAC:creator="COLL"\
 /PDB:$(OUTDIR)/"collect.pdb" /DEBUG /MACHINE:M68K /NODEFAULTLIB:"swap.lib"\
 /OUT:$(OUTDIR)/"collect.exe" 
DEF_FILE=
LINK32_OBJS= \
	$(INTDIR)/stdafx.obj \
	$(INTDIR)/collect.obj \
	$(INTDIR)/mainfrm.obj \
	$(INTDIR)/colledoc.obj \
	$(INTDIR)/collect.rsc \
	$(INTDIR)/strlstvw.obj \
	$(INTDIR)/typlstvw.obj \
	$(INTDIR)/intlstvw.obj \
	$(INTDIR)/dwarryvw.obj \
	$(INTDIR)/typaryvw.obj \
	$(INTDIR)/ptarryvw.obj \
	$(INTDIR)/mapssvw.obj \
	$(INTDIR)/typtrmap.obj \
	$(INTDIR)/mapdwvw.obj \
	$(INTDIR)/collmac.rsc

$(OUTDIR)/collect.exe : $(OUTDIR)  $(DEF_FILE) $(LINK32_OBJS)
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

ALL : $(OUTDIR)/collect.exe $(OUTDIR)/collect.bsc

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
 /D "NDEBUG" /FR$(INTDIR)/ /Fp$(OUTDIR)/"collect.pch" /Yu"stdafx.h"\
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
RSC_PROJ=/r /fo$(INTDIR)/"collect.rsc" /d "_MAC" /d "_68K_" /d "NDEBUG" 
MRC=mrc.exe
# ADD BASE MRC /D "_MAC" /D "_68K_" /D "NDEBUG" /r
# ADD MRC /D "_MAC" /D "_68K_" /D "NDEBUG"
# SUBTRACT MRC /ro
MRC_PROJ=/D "_MAC" /D "_68K_" /D "NDEBUG" 
MFILE32=mfile.exe
# ADD BASE MFILE32 COPY /NOLOGO
# ADD MFILE32 COPY /NOLOGO
MFILE32_FLAGS=COPY /NOLOGO 
MFILE32_FILES= \
	$(OUTDIR)/collect.exe

DOWNLOAD :  $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) $(MFILE32_FILES) ":$(MFILE32_DEST)"


BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# SUBTRACT BASE BSC32 /Iu
# ADD BSC32 /nologo
# SUBTRACT BSC32 /Iu
BSC32_FLAGS=/nologo /o$(OUTDIR)/"collect.bsc" 
BSC32_SBRS= \
	$(INTDIR)/stdafx.sbr \
	$(INTDIR)/collect.sbr \
	$(INTDIR)/mainfrm.sbr \
	$(INTDIR)/colledoc.sbr \
	$(INTDIR)/strlstvw.sbr \
	$(INTDIR)/typlstvw.sbr \
	$(INTDIR)/intlstvw.sbr \
	$(INTDIR)/dwarryvw.sbr \
	$(INTDIR)/typaryvw.sbr \
	$(INTDIR)/ptarryvw.sbr \
	$(INTDIR)/mapssvw.sbr \
	$(INTDIR)/typtrmap.sbr \
	$(INTDIR)/mapdwvw.sbr

$(OUTDIR)/collect.bsc : $(OUTDIR)  $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 swap.lib sanes.lib wlm.lib aslm.lib wlm.rsc commdlg.rsc nafxcm.lib mfcuia32.lib mfcans32.lib ole32.lib oleaut32.lib uuid.lib /NOLOGO /MAC:bundle /MACHINE:M68K /NODEFAULTLIB:"swapd.lib"
# ADD LINK32 swap.lib sanes.lib wlm.lib aslm.lib wlm.rsc commdlg.rsc nafxcm.lib /NOLOGO /MAC:bundle /MAC:creator="COLL" /MACHINE:M68K /NODEFAULTLIB:"swapd.lib"
LINK32_FLAGS=swap.lib sanes.lib wlm.lib aslm.lib wlm.rsc commdlg.rsc nafxcm.lib\
 /NOLOGO /MAC:bundle /MAC:type="APPL" /MAC:creator="COLL"\
 /PDB:$(OUTDIR)/"collect.pdb" /MACHINE:M68K /NODEFAULTLIB:"swapd.lib"\
 /OUT:$(OUTDIR)/"collect.exe" 
DEF_FILE=
LINK32_OBJS= \
	$(INTDIR)/stdafx.obj \
	$(INTDIR)/collect.obj \
	$(INTDIR)/mainfrm.obj \
	$(INTDIR)/colledoc.obj \
	$(INTDIR)/collect.rsc \
	$(INTDIR)/strlstvw.obj \
	$(INTDIR)/typlstvw.obj \
	$(INTDIR)/intlstvw.obj \
	$(INTDIR)/dwarryvw.obj \
	$(INTDIR)/typaryvw.obj \
	$(INTDIR)/ptarryvw.obj \
	$(INTDIR)/mapssvw.obj \
	$(INTDIR)/typtrmap.obj \
	$(INTDIR)/mapdwvw.obj \
	$(INTDIR)/collmac.rsc

$(OUTDIR)/collect.exe : $(OUTDIR)  $(DEF_FILE) $(LINK32_OBJS)
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

ALL : $(OUTDIR)/collect.exe $(OUTDIR)/collect.bsc

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
 "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"collect.pch"\
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
RSC_PROJ=/l 0x409 /fo$(INTDIR)/"collect.res" /d "NDEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# SUBTRACT BASE BSC32 /Iu
# ADD BSC32 /nologo
# SUBTRACT BSC32 /Iu
BSC32_FLAGS=/nologo /o$(OUTDIR)/"collect.bsc" 
BSC32_SBRS= \
	$(INTDIR)/stdafx.sbr \
	$(INTDIR)/collect.sbr \
	$(INTDIR)/mainfrm.sbr \
	$(INTDIR)/colledoc.sbr \
	$(INTDIR)/strlstvw.sbr \
	$(INTDIR)/typlstvw.sbr \
	$(INTDIR)/intlstvw.sbr \
	$(INTDIR)/dwarryvw.sbr \
	$(INTDIR)/typaryvw.sbr \
	$(INTDIR)/ptarryvw.sbr \
	$(INTDIR)/mapssvw.sbr \
	$(INTDIR)/typtrmap.sbr \
	$(INTDIR)/mapdwvw.sbr

$(OUTDIR)/collect.bsc : $(OUTDIR)  $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 /NOLOGO /SUBSYSTEM:windows /MACHINE:MIPS
# ADD LINK32 /NOLOGO /SUBSYSTEM:windows /MACHINE:MIPS
LINK32_FLAGS=/NOLOGO /SUBSYSTEM:windows /PDB:$(OUTDIR)/"collect.pdb"\
 /MACHINE:MIPS /OUT:$(OUTDIR)/"collect.exe" 
DEF_FILE=
LINK32_OBJS= \
	$(INTDIR)/stdafx.obj \
	$(INTDIR)/collect.obj \
	$(INTDIR)/mainfrm.obj \
	$(INTDIR)/colledoc.obj \
	$(INTDIR)/collect.res \
	$(INTDIR)/strlstvw.obj \
	$(INTDIR)/typlstvw.obj \
	$(INTDIR)/intlstvw.obj \
	$(INTDIR)/dwarryvw.obj \
	$(INTDIR)/typaryvw.obj \
	$(INTDIR)/ptarryvw.obj \
	$(INTDIR)/mapssvw.obj \
	$(INTDIR)/typtrmap.obj \
	$(INTDIR)/mapdwvw.obj

$(OUTDIR)/collect.exe : $(OUTDIR)  $(DEF_FILE) $(LINK32_OBJS)
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

ALL : $(OUTDIR)/collect.exe $(OUTDIR)/collect.bsc

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
 "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"collect.pch"\
 /Yu"stdafx.h" /Fo$(INTDIR)/ /Fd$(OUTDIR)/"collect.pdb" /c 
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
RSC_PROJ=/l 0x409 /fo$(INTDIR)/"collect.res" /d "_DEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# SUBTRACT BASE BSC32 /Iu
# ADD BSC32 /nologo
# SUBTRACT BSC32 /Iu
BSC32_FLAGS=/nologo /o$(OUTDIR)/"collect.bsc" 
BSC32_SBRS= \
	$(INTDIR)/stdafx.sbr \
	$(INTDIR)/collect.sbr \
	$(INTDIR)/mainfrm.sbr \
	$(INTDIR)/colledoc.sbr \
	$(INTDIR)/strlstvw.sbr \
	$(INTDIR)/typlstvw.sbr \
	$(INTDIR)/intlstvw.sbr \
	$(INTDIR)/dwarryvw.sbr \
	$(INTDIR)/typaryvw.sbr \
	$(INTDIR)/ptarryvw.sbr \
	$(INTDIR)/mapssvw.sbr \
	$(INTDIR)/typtrmap.sbr \
	$(INTDIR)/mapdwvw.sbr

$(OUTDIR)/collect.bsc : $(OUTDIR)  $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 /NOLOGO /SUBSYSTEM:windows /DEBUG /MACHINE:MIPS
# ADD LINK32 /NOLOGO /SUBSYSTEM:windows /DEBUG /MACHINE:MIPS
LINK32_FLAGS=/NOLOGO /SUBSYSTEM:windows /PDB:$(OUTDIR)/"collect.pdb" /DEBUG\
 /MACHINE:MIPS /OUT:$(OUTDIR)/"collect.exe" 
DEF_FILE=
LINK32_OBJS= \
	$(INTDIR)/stdafx.obj \
	$(INTDIR)/collect.obj \
	$(INTDIR)/mainfrm.obj \
	$(INTDIR)/colledoc.obj \
	$(INTDIR)/collect.res \
	$(INTDIR)/strlstvw.obj \
	$(INTDIR)/typlstvw.obj \
	$(INTDIR)/intlstvw.obj \
	$(INTDIR)/dwarryvw.obj \
	$(INTDIR)/typaryvw.obj \
	$(INTDIR)/ptarryvw.obj \
	$(INTDIR)/mapssvw.obj \
	$(INTDIR)/typtrmap.obj \
	$(INTDIR)/mapdwvw.obj

$(OUTDIR)/collect.exe : $(OUTDIR)  $(DEF_FILE) $(LINK32_OBJS)
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

$(INTDIR)/stdafx.obj :  $(SOURCE)  $(DEP_STDAF) $(INTDIR)
   $(CPP) /nologo /MD /W3 /GX /Zi /Od /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D\
 "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"collect.pch" /Yc"stdafx.h" /Fo$(INTDIR)/\
 /Fd$(OUTDIR)/"collect.pdb" /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (80x86) Release"

# ADD BASE CPP /Yc"stdafx.h"
# ADD CPP /Yc"stdafx.h"

$(INTDIR)/stdafx.obj :  $(SOURCE)  $(DEP_STDAF) $(INTDIR)
   $(CPP) /nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D\
 "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"collect.pch" /Yc"stdafx.h" /Fo$(INTDIR)/ /c\
  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Macintosh Debug"

# ADD CPP /Yc"stdafx.h"

$(INTDIR)/stdafx.obj :  $(SOURCE)  $(DEP_STDAF) $(INTDIR)
   $(CPP) /nologo /AL /Gt1 /Q68s /Q68m /W3 /GX /Zi /Od /D "_WINDOWS" /D "_MAC"\
 /D "_68K_" /D "_DEBUG" /FR$(INTDIR)/ /Fp$(OUTDIR)/"collect.pch" /Yc"stdafx.h"\
 /Fo$(INTDIR)/ /Fd$(OUTDIR)/"collect.pdb" /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Macintosh Release"

# ADD CPP /Yc"stdafx.h"

$(INTDIR)/stdafx.obj :  $(SOURCE)  $(DEP_STDAF) $(INTDIR)
   $(CPP) /nologo /AL /Gt1 /Q68s /W3 /GX /O2 /D "_WINDOWS" /D "_MAC" /D "_68K_"\
 /D "NDEBUG" /FR$(INTDIR)/ /Fp$(OUTDIR)/"collect.pch" /Yc"stdafx.h"\
 /Fo$(INTDIR)/ /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

# ADD BASE CPP /Gt0 /QMOb2000
# ADD CPP /Gt0 /QMOb2000 /Yc"stdafx.h"

$(INTDIR)/stdafx.obj :  $(SOURCE)  $(DEP_STDAF) $(INTDIR)
   $(CPP) /nologo /MD /Gt0 /QMOb2000 /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D\
 "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"collect.pch"\
 /Yc"stdafx.h" /Fo$(INTDIR)/ /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

# ADD BASE CPP /Gt0 /QMOb2000
# ADD CPP /Gt0 /QMOb2000 /Yc"stdafx.h"

$(INTDIR)/stdafx.obj :  $(SOURCE)  $(DEP_STDAF) $(INTDIR)
   $(CPP) /nologo /MD /Gt0 /QMOb2000 /W3 /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D\
 "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"collect.pch"\
 /Yc"stdafx.h" /Fo$(INTDIR)/ /Fd$(OUTDIR)/"collect.pdb" /c  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\collect.cpp
DEP_COLLE=\
	.\stdafx.h\
	.\collect.h\
	.\mainfrm.h\
	.\colledoc.h\
	.\strlstvw.h

!IF  "$(CFG)" == "Win32 (80x86) Debug"

$(INTDIR)/collect.obj :  $(SOURCE)  $(DEP_COLLE) $(INTDIR) $(INTDIR)/stdafx.obj
   $(CPP) /nologo /MD /W3 /GX /Zi /Od /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D\
 "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"collect.pch" /Yu"stdafx.h" /Fo$(INTDIR)/\
 /Fd$(OUTDIR)/"collect.pdb" /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (80x86) Release"

$(INTDIR)/collect.obj :  $(SOURCE)  $(DEP_COLLE) $(INTDIR) $(INTDIR)/stdafx.obj
   $(CPP) /nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D\
 "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"collect.pch" /Yu"stdafx.h" /Fo$(INTDIR)/ /c\
  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Macintosh Debug"

# ADD CPP /Yu"stdafx.h"

$(INTDIR)/collect.obj :  $(SOURCE)  $(DEP_COLLE) $(INTDIR) $(INTDIR)/stdafx.obj
   $(CPP) /nologo /AL /Gt1 /Q68s /Q68m /W3 /GX /Zi /Od /D "_WINDOWS" /D "_MAC"\
 /D "_68K_" /D "_DEBUG" /FR$(INTDIR)/ /Fp$(OUTDIR)/"collect.pch" /Yu"stdafx.h"\
 /Fo$(INTDIR)/ /Fd$(OUTDIR)/"collect.pdb" /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Macintosh Release"

# ADD CPP /Yu"stdafx.h"

$(INTDIR)/collect.obj :  $(SOURCE)  $(DEP_COLLE) $(INTDIR) $(INTDIR)/stdafx.obj
   $(CPP) /nologo /AL /Gt1 /Q68s /W3 /GX /O2 /D "_WINDOWS" /D "_MAC" /D "_68K_"\
 /D "NDEBUG" /FR$(INTDIR)/ /Fp$(OUTDIR)/"collect.pch" /Yu"stdafx.h"\
 /Fo$(INTDIR)/ /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

# ADD BASE CPP /Gt0 /QMOb2000
# ADD CPP /Gt0 /QMOb2000 /Yu"stdafx.h"

$(INTDIR)/collect.obj :  $(SOURCE)  $(DEP_COLLE) $(INTDIR) $(INTDIR)/stdafx.obj
   $(CPP) /nologo /MD /Gt0 /QMOb2000 /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D\
 "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"collect.pch"\
 /Yu"stdafx.h" /Fo$(INTDIR)/ /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

# ADD BASE CPP /Gt0 /QMOb2000
# ADD CPP /Gt0 /QMOb2000 /Yu"stdafx.h"

$(INTDIR)/collect.obj :  $(SOURCE)  $(DEP_COLLE) $(INTDIR) $(INTDIR)/stdafx.obj
   $(CPP) /nologo /MD /Gt0 /QMOb2000 /W3 /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D\
 "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"collect.pch"\
 /Yu"stdafx.h" /Fo$(INTDIR)/ /Fd$(OUTDIR)/"collect.pdb" /c  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\mainfrm.cpp
DEP_MAINF=\
	.\stdafx.h\
	.\collect.h\
	.\mainfrm.h\
	.\colledoc.h\
	.\strlstvw.h\
	.\typlstvw.h\
	.\intlstvw.h\
	.\dwarryvw.h\
	.\typaryvw.h\
	.\ptarryvw.h\
	.\mapssvw.h\
	.\typtrmap.h\
	.\mapdwvw.h

!IF  "$(CFG)" == "Win32 (80x86) Debug"

$(INTDIR)/mainfrm.obj :  $(SOURCE)  $(DEP_MAINF) $(INTDIR) $(INTDIR)/stdafx.obj
   $(CPP) /nologo /MD /W3 /GX /Zi /Od /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D\
 "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"collect.pch" /Yu"stdafx.h" /Fo$(INTDIR)/\
 /Fd$(OUTDIR)/"collect.pdb" /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (80x86) Release"

$(INTDIR)/mainfrm.obj :  $(SOURCE)  $(DEP_MAINF) $(INTDIR) $(INTDIR)/stdafx.obj
   $(CPP) /nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D\
 "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"collect.pch" /Yu"stdafx.h" /Fo$(INTDIR)/ /c\
  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Macintosh Debug"

# ADD CPP /Yu"stdafx.h"

$(INTDIR)/mainfrm.obj :  $(SOURCE)  $(DEP_MAINF) $(INTDIR) $(INTDIR)/stdafx.obj
   $(CPP) /nologo /AL /Gt1 /Q68s /Q68m /W3 /GX /Zi /Od /D "_WINDOWS" /D "_MAC"\
 /D "_68K_" /D "_DEBUG" /FR$(INTDIR)/ /Fp$(OUTDIR)/"collect.pch" /Yu"stdafx.h"\
 /Fo$(INTDIR)/ /Fd$(OUTDIR)/"collect.pdb" /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Macintosh Release"

# ADD CPP /Yu"stdafx.h"

$(INTDIR)/mainfrm.obj :  $(SOURCE)  $(DEP_MAINF) $(INTDIR) $(INTDIR)/stdafx.obj
   $(CPP) /nologo /AL /Gt1 /Q68s /W3 /GX /O2 /D "_WINDOWS" /D "_MAC" /D "_68K_"\
 /D "NDEBUG" /FR$(INTDIR)/ /Fp$(OUTDIR)/"collect.pch" /Yu"stdafx.h"\
 /Fo$(INTDIR)/ /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

# ADD BASE CPP /Gt0 /QMOb2000
# ADD CPP /Gt0 /QMOb2000 /Yu"stdafx.h"

$(INTDIR)/mainfrm.obj :  $(SOURCE)  $(DEP_MAINF) $(INTDIR) $(INTDIR)/stdafx.obj
   $(CPP) /nologo /MD /Gt0 /QMOb2000 /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D\
 "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"collect.pch"\
 /Yu"stdafx.h" /Fo$(INTDIR)/ /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

# ADD BASE CPP /Gt0 /QMOb2000
# ADD CPP /Gt0 /QMOb2000 /Yu"stdafx.h"

$(INTDIR)/mainfrm.obj :  $(SOURCE)  $(DEP_MAINF) $(INTDIR) $(INTDIR)/stdafx.obj
   $(CPP) /nologo /MD /Gt0 /QMOb2000 /W3 /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D\
 "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"collect.pch"\
 /Yu"stdafx.h" /Fo$(INTDIR)/ /Fd$(OUTDIR)/"collect.pdb" /c  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\colledoc.cpp
DEP_COLLED=\
	.\stdafx.h\
	.\collect.h\
	.\colledoc.h

!IF  "$(CFG)" == "Win32 (80x86) Debug"

$(INTDIR)/colledoc.obj :  $(SOURCE)  $(DEP_COLLED) $(INTDIR)\
 $(INTDIR)/stdafx.obj
   $(CPP) /nologo /MD /W3 /GX /Zi /Od /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D\
 "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"collect.pch" /Yu"stdafx.h" /Fo$(INTDIR)/\
 /Fd$(OUTDIR)/"collect.pdb" /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (80x86) Release"

$(INTDIR)/colledoc.obj :  $(SOURCE)  $(DEP_COLLED) $(INTDIR)\
 $(INTDIR)/stdafx.obj
   $(CPP) /nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D\
 "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"collect.pch" /Yu"stdafx.h" /Fo$(INTDIR)/ /c\
  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Macintosh Debug"

# ADD CPP /Yu"stdafx.h"

$(INTDIR)/colledoc.obj :  $(SOURCE)  $(DEP_COLLED) $(INTDIR)\
 $(INTDIR)/stdafx.obj
   $(CPP) /nologo /AL /Gt1 /Q68s /Q68m /W3 /GX /Zi /Od /D "_WINDOWS" /D "_MAC"\
 /D "_68K_" /D "_DEBUG" /FR$(INTDIR)/ /Fp$(OUTDIR)/"collect.pch" /Yu"stdafx.h"\
 /Fo$(INTDIR)/ /Fd$(OUTDIR)/"collect.pdb" /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Macintosh Release"

# ADD CPP /Yu"stdafx.h"

$(INTDIR)/colledoc.obj :  $(SOURCE)  $(DEP_COLLED) $(INTDIR)\
 $(INTDIR)/stdafx.obj
   $(CPP) /nologo /AL /Gt1 /Q68s /W3 /GX /O2 /D "_WINDOWS" /D "_MAC" /D "_68K_"\
 /D "NDEBUG" /FR$(INTDIR)/ /Fp$(OUTDIR)/"collect.pch" /Yu"stdafx.h"\
 /Fo$(INTDIR)/ /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

# ADD BASE CPP /Gt0 /QMOb2000
# ADD CPP /Gt0 /QMOb2000 /Yu"stdafx.h"

$(INTDIR)/colledoc.obj :  $(SOURCE)  $(DEP_COLLED) $(INTDIR)\
 $(INTDIR)/stdafx.obj
   $(CPP) /nologo /MD /Gt0 /QMOb2000 /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D\
 "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"collect.pch"\
 /Yu"stdafx.h" /Fo$(INTDIR)/ /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

# ADD BASE CPP /Gt0 /QMOb2000
# ADD CPP /Gt0 /QMOb2000 /Yu"stdafx.h"

$(INTDIR)/colledoc.obj :  $(SOURCE)  $(DEP_COLLED) $(INTDIR)\
 $(INTDIR)/stdafx.obj
   $(CPP) /nologo /MD /Gt0 /QMOb2000 /W3 /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D\
 "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"collect.pch"\
 /Yu"stdafx.h" /Fo$(INTDIR)/ /Fd$(OUTDIR)/"collect.pdb" /c  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\collect.rc
DEP_COLLEC=\
	.\res\collect.ico\
	.\res\toolbar.bmp\
	.\res\collect.rc2

!IF  "$(CFG)" == "Win32 (80x86) Debug"

$(INTDIR)/collect.res :  $(SOURCE)  $(DEP_COLLEC) $(INTDIR)
   $(RSC) $(RSC_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (80x86) Release"

$(INTDIR)/collect.res :  $(SOURCE)  $(DEP_COLLEC) $(INTDIR)
   $(RSC) $(RSC_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Macintosh Debug"

$(INTDIR)/collect.rsc :  $(SOURCE)  $(DEP_COLLEC) $(INTDIR)
   $(RSC) $(RSC_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Macintosh Release"

$(INTDIR)/collect.rsc :  $(SOURCE)  $(DEP_COLLEC) $(INTDIR)
   $(RSC) $(RSC_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

$(INTDIR)/collect.res :  $(SOURCE)  $(DEP_COLLEC) $(INTDIR)
   $(RSC) $(RSC_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

$(INTDIR)/collect.res :  $(SOURCE)  $(DEP_COLLEC) $(INTDIR)
   $(RSC) $(RSC_PROJ)  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\strlstvw.cpp
DEP_STRLS=\
	.\stdafx.h\
	.\collect.h\
	.\colledoc.h\
	.\strlstvw.h

!IF  "$(CFG)" == "Win32 (80x86) Debug"

$(INTDIR)/strlstvw.obj :  $(SOURCE)  $(DEP_STRLS) $(INTDIR)\
 $(INTDIR)/stdafx.obj
   $(CPP) /nologo /MD /W3 /GX /Zi /Od /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D\
 "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"collect.pch" /Yu"stdafx.h" /Fo$(INTDIR)/\
 /Fd$(OUTDIR)/"collect.pdb" /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (80x86) Release"

$(INTDIR)/strlstvw.obj :  $(SOURCE)  $(DEP_STRLS) $(INTDIR)\
 $(INTDIR)/stdafx.obj
   $(CPP) /nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D\
 "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"collect.pch" /Yu"stdafx.h" /Fo$(INTDIR)/ /c\
  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Macintosh Debug"

# ADD CPP /Yu"stdafx.h"

$(INTDIR)/strlstvw.obj :  $(SOURCE)  $(DEP_STRLS) $(INTDIR)\
 $(INTDIR)/stdafx.obj
   $(CPP) /nologo /AL /Gt1 /Q68s /Q68m /W3 /GX /Zi /Od /D "_WINDOWS" /D "_MAC"\
 /D "_68K_" /D "_DEBUG" /FR$(INTDIR)/ /Fp$(OUTDIR)/"collect.pch" /Yu"stdafx.h"\
 /Fo$(INTDIR)/ /Fd$(OUTDIR)/"collect.pdb" /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Macintosh Release"

# ADD CPP /Yu"stdafx.h"

$(INTDIR)/strlstvw.obj :  $(SOURCE)  $(DEP_STRLS) $(INTDIR)\
 $(INTDIR)/stdafx.obj
   $(CPP) /nologo /AL /Gt1 /Q68s /W3 /GX /O2 /D "_WINDOWS" /D "_MAC" /D "_68K_"\
 /D "NDEBUG" /FR$(INTDIR)/ /Fp$(OUTDIR)/"collect.pch" /Yu"stdafx.h"\
 /Fo$(INTDIR)/ /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

# ADD BASE CPP /Gt0 /QMOb2000
# ADD CPP /Gt0 /QMOb2000 /Yu"stdafx.h"

$(INTDIR)/strlstvw.obj :  $(SOURCE)  $(DEP_STRLS) $(INTDIR)\
 $(INTDIR)/stdafx.obj
   $(CPP) /nologo /MD /Gt0 /QMOb2000 /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D\
 "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"collect.pch"\
 /Yu"stdafx.h" /Fo$(INTDIR)/ /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

# ADD BASE CPP /Gt0 /QMOb2000
# ADD CPP /Gt0 /QMOb2000 /Yu"stdafx.h"

$(INTDIR)/strlstvw.obj :  $(SOURCE)  $(DEP_STRLS) $(INTDIR)\
 $(INTDIR)/stdafx.obj
   $(CPP) /nologo /MD /Gt0 /QMOb2000 /W3 /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D\
 "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"collect.pch"\
 /Yu"stdafx.h" /Fo$(INTDIR)/ /Fd$(OUTDIR)/"collect.pdb" /c  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\typlstvw.cpp
DEP_TYPLS=\
	.\stdafx.h\
	.\collect.h\
	.\colledoc.h\
	.\typlstvw.h

!IF  "$(CFG)" == "Win32 (80x86) Debug"

$(INTDIR)/typlstvw.obj :  $(SOURCE)  $(DEP_TYPLS) $(INTDIR)\
 $(INTDIR)/stdafx.obj
   $(CPP) /nologo /MD /W3 /GX /Zi /Od /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D\
 "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"collect.pch" /Yu"stdafx.h" /Fo$(INTDIR)/\
 /Fd$(OUTDIR)/"collect.pdb" /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (80x86) Release"

$(INTDIR)/typlstvw.obj :  $(SOURCE)  $(DEP_TYPLS) $(INTDIR)\
 $(INTDIR)/stdafx.obj
   $(CPP) /nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D\
 "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"collect.pch" /Yu"stdafx.h" /Fo$(INTDIR)/ /c\
  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Macintosh Debug"

# ADD CPP /Yu"stdafx.h"

$(INTDIR)/typlstvw.obj :  $(SOURCE)  $(DEP_TYPLS) $(INTDIR)\
 $(INTDIR)/stdafx.obj
   $(CPP) /nologo /AL /Gt1 /Q68s /Q68m /W3 /GX /Zi /Od /D "_WINDOWS" /D "_MAC"\
 /D "_68K_" /D "_DEBUG" /FR$(INTDIR)/ /Fp$(OUTDIR)/"collect.pch" /Yu"stdafx.h"\
 /Fo$(INTDIR)/ /Fd$(OUTDIR)/"collect.pdb" /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Macintosh Release"

# ADD CPP /Yu"stdafx.h"

$(INTDIR)/typlstvw.obj :  $(SOURCE)  $(DEP_TYPLS) $(INTDIR)\
 $(INTDIR)/stdafx.obj
   $(CPP) /nologo /AL /Gt1 /Q68s /W3 /GX /O2 /D "_WINDOWS" /D "_MAC" /D "_68K_"\
 /D "NDEBUG" /FR$(INTDIR)/ /Fp$(OUTDIR)/"collect.pch" /Yu"stdafx.h"\
 /Fo$(INTDIR)/ /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

# ADD BASE CPP /Gt0 /QMOb2000
# ADD CPP /Gt0 /QMOb2000 /Yu"stdafx.h"

$(INTDIR)/typlstvw.obj :  $(SOURCE)  $(DEP_TYPLS) $(INTDIR)\
 $(INTDIR)/stdafx.obj
   $(CPP) /nologo /MD /Gt0 /QMOb2000 /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D\
 "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"collect.pch"\
 /Yu"stdafx.h" /Fo$(INTDIR)/ /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

# ADD BASE CPP /Gt0 /QMOb2000
# ADD CPP /Gt0 /QMOb2000 /Yu"stdafx.h"

$(INTDIR)/typlstvw.obj :  $(SOURCE)  $(DEP_TYPLS) $(INTDIR)\
 $(INTDIR)/stdafx.obj
   $(CPP) /nologo /MD /Gt0 /QMOb2000 /W3 /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D\
 "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"collect.pch"\
 /Yu"stdafx.h" /Fo$(INTDIR)/ /Fd$(OUTDIR)/"collect.pdb" /c  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\intlstvw.cpp
DEP_INTLS=\
	.\stdafx.h\
	.\collect.h\
	.\colledoc.h\
	.\intlstvw.h

!IF  "$(CFG)" == "Win32 (80x86) Debug"

$(INTDIR)/intlstvw.obj :  $(SOURCE)  $(DEP_INTLS) $(INTDIR)\
 $(INTDIR)/stdafx.obj
   $(CPP) /nologo /MD /W3 /GX /Zi /Od /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D\
 "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"collect.pch" /Yu"stdafx.h" /Fo$(INTDIR)/\
 /Fd$(OUTDIR)/"collect.pdb" /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (80x86) Release"

$(INTDIR)/intlstvw.obj :  $(SOURCE)  $(DEP_INTLS) $(INTDIR)\
 $(INTDIR)/stdafx.obj
   $(CPP) /nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D\
 "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"collect.pch" /Yu"stdafx.h" /Fo$(INTDIR)/ /c\
  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Macintosh Debug"

# ADD CPP /Yu"stdafx.h"

$(INTDIR)/intlstvw.obj :  $(SOURCE)  $(DEP_INTLS) $(INTDIR)\
 $(INTDIR)/stdafx.obj
   $(CPP) /nologo /AL /Gt1 /Q68s /Q68m /W3 /GX /Zi /Od /D "_WINDOWS" /D "_MAC"\
 /D "_68K_" /D "_DEBUG" /FR$(INTDIR)/ /Fp$(OUTDIR)/"collect.pch" /Yu"stdafx.h"\
 /Fo$(INTDIR)/ /Fd$(OUTDIR)/"collect.pdb" /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Macintosh Release"

# ADD CPP /Yu"stdafx.h"

$(INTDIR)/intlstvw.obj :  $(SOURCE)  $(DEP_INTLS) $(INTDIR)\
 $(INTDIR)/stdafx.obj
   $(CPP) /nologo /AL /Gt1 /Q68s /W3 /GX /O2 /D "_WINDOWS" /D "_MAC" /D "_68K_"\
 /D "NDEBUG" /FR$(INTDIR)/ /Fp$(OUTDIR)/"collect.pch" /Yu"stdafx.h"\
 /Fo$(INTDIR)/ /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

# ADD BASE CPP /Gt0 /QMOb2000
# ADD CPP /Gt0 /QMOb2000 /Yu"stdafx.h"

$(INTDIR)/intlstvw.obj :  $(SOURCE)  $(DEP_INTLS) $(INTDIR)\
 $(INTDIR)/stdafx.obj
   $(CPP) /nologo /MD /Gt0 /QMOb2000 /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D\
 "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"collect.pch"\
 /Yu"stdafx.h" /Fo$(INTDIR)/ /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

# ADD BASE CPP /Gt0 /QMOb2000
# ADD CPP /Gt0 /QMOb2000 /Yu"stdafx.h"

$(INTDIR)/intlstvw.obj :  $(SOURCE)  $(DEP_INTLS) $(INTDIR)\
 $(INTDIR)/stdafx.obj
   $(CPP) /nologo /MD /Gt0 /QMOb2000 /W3 /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D\
 "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"collect.pch"\
 /Yu"stdafx.h" /Fo$(INTDIR)/ /Fd$(OUTDIR)/"collect.pdb" /c  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\dwarryvw.cpp
DEP_DWARR=\
	.\stdafx.h\
	.\collect.h\
	.\colledoc.h\
	.\dwarryvw.h

!IF  "$(CFG)" == "Win32 (80x86) Debug"

$(INTDIR)/dwarryvw.obj :  $(SOURCE)  $(DEP_DWARR) $(INTDIR)\
 $(INTDIR)/stdafx.obj
   $(CPP) /nologo /MD /W3 /GX /Zi /Od /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D\
 "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"collect.pch" /Yu"stdafx.h" /Fo$(INTDIR)/\
 /Fd$(OUTDIR)/"collect.pdb" /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (80x86) Release"

$(INTDIR)/dwarryvw.obj :  $(SOURCE)  $(DEP_DWARR) $(INTDIR)\
 $(INTDIR)/stdafx.obj
   $(CPP) /nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D\
 "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"collect.pch" /Yu"stdafx.h" /Fo$(INTDIR)/ /c\
  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Macintosh Debug"

# ADD CPP /Yu"stdafx.h"

$(INTDIR)/dwarryvw.obj :  $(SOURCE)  $(DEP_DWARR) $(INTDIR)\
 $(INTDIR)/stdafx.obj
   $(CPP) /nologo /AL /Gt1 /Q68s /Q68m /W3 /GX /Zi /Od /D "_WINDOWS" /D "_MAC"\
 /D "_68K_" /D "_DEBUG" /FR$(INTDIR)/ /Fp$(OUTDIR)/"collect.pch" /Yu"stdafx.h"\
 /Fo$(INTDIR)/ /Fd$(OUTDIR)/"collect.pdb" /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Macintosh Release"

# ADD CPP /Yu"stdafx.h"

$(INTDIR)/dwarryvw.obj :  $(SOURCE)  $(DEP_DWARR) $(INTDIR)\
 $(INTDIR)/stdafx.obj
   $(CPP) /nologo /AL /Gt1 /Q68s /W3 /GX /O2 /D "_WINDOWS" /D "_MAC" /D "_68K_"\
 /D "NDEBUG" /FR$(INTDIR)/ /Fp$(OUTDIR)/"collect.pch" /Yu"stdafx.h"\
 /Fo$(INTDIR)/ /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

# ADD BASE CPP /Gt0 /QMOb2000
# ADD CPP /Gt0 /QMOb2000 /Yu"stdafx.h"

$(INTDIR)/dwarryvw.obj :  $(SOURCE)  $(DEP_DWARR) $(INTDIR)\
 $(INTDIR)/stdafx.obj
   $(CPP) /nologo /MD /Gt0 /QMOb2000 /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D\
 "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"collect.pch"\
 /Yu"stdafx.h" /Fo$(INTDIR)/ /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

# ADD BASE CPP /Gt0 /QMOb2000
# ADD CPP /Gt0 /QMOb2000 /Yu"stdafx.h"

$(INTDIR)/dwarryvw.obj :  $(SOURCE)  $(DEP_DWARR) $(INTDIR)\
 $(INTDIR)/stdafx.obj
   $(CPP) /nologo /MD /Gt0 /QMOb2000 /W3 /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D\
 "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"collect.pch"\
 /Yu"stdafx.h" /Fo$(INTDIR)/ /Fd$(OUTDIR)/"collect.pdb" /c  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\typaryvw.cpp
DEP_TYPAR=\
	.\stdafx.h\
	.\collect.h\
	.\colledoc.h\
	.\typaryvw.h

!IF  "$(CFG)" == "Win32 (80x86) Debug"

$(INTDIR)/typaryvw.obj :  $(SOURCE)  $(DEP_TYPAR) $(INTDIR)\
 $(INTDIR)/stdafx.obj
   $(CPP) /nologo /MD /W3 /GX /Zi /Od /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D\
 "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"collect.pch" /Yu"stdafx.h" /Fo$(INTDIR)/\
 /Fd$(OUTDIR)/"collect.pdb" /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (80x86) Release"

$(INTDIR)/typaryvw.obj :  $(SOURCE)  $(DEP_TYPAR) $(INTDIR)\
 $(INTDIR)/stdafx.obj
   $(CPP) /nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D\
 "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"collect.pch" /Yu"stdafx.h" /Fo$(INTDIR)/ /c\
  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Macintosh Debug"

# ADD CPP /Yu"stdafx.h"

$(INTDIR)/typaryvw.obj :  $(SOURCE)  $(DEP_TYPAR) $(INTDIR)\
 $(INTDIR)/stdafx.obj
   $(CPP) /nologo /AL /Gt1 /Q68s /Q68m /W3 /GX /Zi /Od /D "_WINDOWS" /D "_MAC"\
 /D "_68K_" /D "_DEBUG" /FR$(INTDIR)/ /Fp$(OUTDIR)/"collect.pch" /Yu"stdafx.h"\
 /Fo$(INTDIR)/ /Fd$(OUTDIR)/"collect.pdb" /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Macintosh Release"

# ADD CPP /Yu"stdafx.h"

$(INTDIR)/typaryvw.obj :  $(SOURCE)  $(DEP_TYPAR) $(INTDIR)\
 $(INTDIR)/stdafx.obj
   $(CPP) /nologo /AL /Gt1 /Q68s /W3 /GX /O2 /D "_WINDOWS" /D "_MAC" /D "_68K_"\
 /D "NDEBUG" /FR$(INTDIR)/ /Fp$(OUTDIR)/"collect.pch" /Yu"stdafx.h"\
 /Fo$(INTDIR)/ /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

# ADD BASE CPP /Gt0 /QMOb2000
# ADD CPP /Gt0 /QMOb2000 /Yu"stdafx.h"

$(INTDIR)/typaryvw.obj :  $(SOURCE)  $(DEP_TYPAR) $(INTDIR)\
 $(INTDIR)/stdafx.obj
   $(CPP) /nologo /MD /Gt0 /QMOb2000 /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D\
 "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"collect.pch"\
 /Yu"stdafx.h" /Fo$(INTDIR)/ /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

# ADD BASE CPP /Gt0 /QMOb2000
# ADD CPP /Gt0 /QMOb2000 /Yu"stdafx.h"

$(INTDIR)/typaryvw.obj :  $(SOURCE)  $(DEP_TYPAR) $(INTDIR)\
 $(INTDIR)/stdafx.obj
   $(CPP) /nologo /MD /Gt0 /QMOb2000 /W3 /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D\
 "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"collect.pch"\
 /Yu"stdafx.h" /Fo$(INTDIR)/ /Fd$(OUTDIR)/"collect.pdb" /c  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\ptarryvw.cpp
DEP_PTARR=\
	.\stdafx.h\
	.\collect.h\
	.\colledoc.h\
	.\ptarryvw.h

!IF  "$(CFG)" == "Win32 (80x86) Debug"

$(INTDIR)/ptarryvw.obj :  $(SOURCE)  $(DEP_PTARR) $(INTDIR)\
 $(INTDIR)/stdafx.obj
   $(CPP) /nologo /MD /W3 /GX /Zi /Od /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D\
 "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"collect.pch" /Yu"stdafx.h" /Fo$(INTDIR)/\
 /Fd$(OUTDIR)/"collect.pdb" /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (80x86) Release"

$(INTDIR)/ptarryvw.obj :  $(SOURCE)  $(DEP_PTARR) $(INTDIR)\
 $(INTDIR)/stdafx.obj
   $(CPP) /nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D\
 "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"collect.pch" /Yu"stdafx.h" /Fo$(INTDIR)/ /c\
  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Macintosh Debug"

# ADD CPP /Yu"stdafx.h"

$(INTDIR)/ptarryvw.obj :  $(SOURCE)  $(DEP_PTARR) $(INTDIR)\
 $(INTDIR)/stdafx.obj
   $(CPP) /nologo /AL /Gt1 /Q68s /Q68m /W3 /GX /Zi /Od /D "_WINDOWS" /D "_MAC"\
 /D "_68K_" /D "_DEBUG" /FR$(INTDIR)/ /Fp$(OUTDIR)/"collect.pch" /Yu"stdafx.h"\
 /Fo$(INTDIR)/ /Fd$(OUTDIR)/"collect.pdb" /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Macintosh Release"

# ADD CPP /Yu"stdafx.h"

$(INTDIR)/ptarryvw.obj :  $(SOURCE)  $(DEP_PTARR) $(INTDIR)\
 $(INTDIR)/stdafx.obj
   $(CPP) /nologo /AL /Gt1 /Q68s /W3 /GX /O2 /D "_WINDOWS" /D "_MAC" /D "_68K_"\
 /D "NDEBUG" /FR$(INTDIR)/ /Fp$(OUTDIR)/"collect.pch" /Yu"stdafx.h"\
 /Fo$(INTDIR)/ /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

# ADD BASE CPP /Gt0 /QMOb2000
# ADD CPP /Gt0 /QMOb2000 /Yu"stdafx.h"

$(INTDIR)/ptarryvw.obj :  $(SOURCE)  $(DEP_PTARR) $(INTDIR)\
 $(INTDIR)/stdafx.obj
   $(CPP) /nologo /MD /Gt0 /QMOb2000 /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D\
 "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"collect.pch"\
 /Yu"stdafx.h" /Fo$(INTDIR)/ /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

# ADD BASE CPP /Gt0 /QMOb2000
# ADD CPP /Gt0 /QMOb2000 /Yu"stdafx.h"

$(INTDIR)/ptarryvw.obj :  $(SOURCE)  $(DEP_PTARR) $(INTDIR)\
 $(INTDIR)/stdafx.obj
   $(CPP) /nologo /MD /Gt0 /QMOb2000 /W3 /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D\
 "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"collect.pch"\
 /Yu"stdafx.h" /Fo$(INTDIR)/ /Fd$(OUTDIR)/"collect.pdb" /c  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\mapssvw.cpp
DEP_MAPSS=\
	.\stdafx.h\
	.\collect.h\
	.\colledoc.h\
	.\mapssvw.h

!IF  "$(CFG)" == "Win32 (80x86) Debug"

$(INTDIR)/mapssvw.obj :  $(SOURCE)  $(DEP_MAPSS) $(INTDIR) $(INTDIR)/stdafx.obj
   $(CPP) /nologo /MD /W3 /GX /Zi /Od /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D\
 "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"collect.pch" /Yu"stdafx.h" /Fo$(INTDIR)/\
 /Fd$(OUTDIR)/"collect.pdb" /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (80x86) Release"

$(INTDIR)/mapssvw.obj :  $(SOURCE)  $(DEP_MAPSS) $(INTDIR) $(INTDIR)/stdafx.obj
   $(CPP) /nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D\
 "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"collect.pch" /Yu"stdafx.h" /Fo$(INTDIR)/ /c\
  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Macintosh Debug"

# ADD CPP /Yu"stdafx.h"

$(INTDIR)/mapssvw.obj :  $(SOURCE)  $(DEP_MAPSS) $(INTDIR) $(INTDIR)/stdafx.obj
   $(CPP) /nologo /AL /Gt1 /Q68s /Q68m /W3 /GX /Zi /Od /D "_WINDOWS" /D "_MAC"\
 /D "_68K_" /D "_DEBUG" /FR$(INTDIR)/ /Fp$(OUTDIR)/"collect.pch" /Yu"stdafx.h"\
 /Fo$(INTDIR)/ /Fd$(OUTDIR)/"collect.pdb" /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Macintosh Release"

# ADD CPP /Yu"stdafx.h"

$(INTDIR)/mapssvw.obj :  $(SOURCE)  $(DEP_MAPSS) $(INTDIR) $(INTDIR)/stdafx.obj
   $(CPP) /nologo /AL /Gt1 /Q68s /W3 /GX /O2 /D "_WINDOWS" /D "_MAC" /D "_68K_"\
 /D "NDEBUG" /FR$(INTDIR)/ /Fp$(OUTDIR)/"collect.pch" /Yu"stdafx.h"\
 /Fo$(INTDIR)/ /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

# ADD BASE CPP /Gt0 /QMOb2000
# ADD CPP /Gt0 /QMOb2000 /Yu"stdafx.h"

$(INTDIR)/mapssvw.obj :  $(SOURCE)  $(DEP_MAPSS) $(INTDIR) $(INTDIR)/stdafx.obj
   $(CPP) /nologo /MD /Gt0 /QMOb2000 /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D\
 "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"collect.pch"\
 /Yu"stdafx.h" /Fo$(INTDIR)/ /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

# ADD BASE CPP /Gt0 /QMOb2000
# ADD CPP /Gt0 /QMOb2000 /Yu"stdafx.h"

$(INTDIR)/mapssvw.obj :  $(SOURCE)  $(DEP_MAPSS) $(INTDIR) $(INTDIR)/stdafx.obj
   $(CPP) /nologo /MD /Gt0 /QMOb2000 /W3 /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D\
 "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"collect.pch"\
 /Yu"stdafx.h" /Fo$(INTDIR)/ /Fd$(OUTDIR)/"collect.pdb" /c  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\typtrmap.cpp
DEP_TYPTR=\
	.\stdafx.h\
	.\collect.h\
	.\colledoc.h\
	.\typtrmap.h

!IF  "$(CFG)" == "Win32 (80x86) Debug"

$(INTDIR)/typtrmap.obj :  $(SOURCE)  $(DEP_TYPTR) $(INTDIR)\
 $(INTDIR)/stdafx.obj
   $(CPP) /nologo /MD /W3 /GX /Zi /Od /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D\
 "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"collect.pch" /Yu"stdafx.h" /Fo$(INTDIR)/\
 /Fd$(OUTDIR)/"collect.pdb" /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (80x86) Release"

$(INTDIR)/typtrmap.obj :  $(SOURCE)  $(DEP_TYPTR) $(INTDIR)\
 $(INTDIR)/stdafx.obj
   $(CPP) /nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D\
 "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"collect.pch" /Yu"stdafx.h" /Fo$(INTDIR)/ /c\
  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Macintosh Debug"

# ADD CPP /Yu"stdafx.h"

$(INTDIR)/typtrmap.obj :  $(SOURCE)  $(DEP_TYPTR) $(INTDIR)\
 $(INTDIR)/stdafx.obj
   $(CPP) /nologo /AL /Gt1 /Q68s /Q68m /W3 /GX /Zi /Od /D "_WINDOWS" /D "_MAC"\
 /D "_68K_" /D "_DEBUG" /FR$(INTDIR)/ /Fp$(OUTDIR)/"collect.pch" /Yu"stdafx.h"\
 /Fo$(INTDIR)/ /Fd$(OUTDIR)/"collect.pdb" /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Macintosh Release"

# ADD CPP /Yu"stdafx.h"

$(INTDIR)/typtrmap.obj :  $(SOURCE)  $(DEP_TYPTR) $(INTDIR)\
 $(INTDIR)/stdafx.obj
   $(CPP) /nologo /AL /Gt1 /Q68s /W3 /GX /O2 /D "_WINDOWS" /D "_MAC" /D "_68K_"\
 /D "NDEBUG" /FR$(INTDIR)/ /Fp$(OUTDIR)/"collect.pch" /Yu"stdafx.h"\
 /Fo$(INTDIR)/ /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

# ADD BASE CPP /Gt0 /QMOb2000
# ADD CPP /Gt0 /QMOb2000 /Yu"stdafx.h"

$(INTDIR)/typtrmap.obj :  $(SOURCE)  $(DEP_TYPTR) $(INTDIR)\
 $(INTDIR)/stdafx.obj
   $(CPP) /nologo /MD /Gt0 /QMOb2000 /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D\
 "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"collect.pch"\
 /Yu"stdafx.h" /Fo$(INTDIR)/ /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

# ADD BASE CPP /Gt0 /QMOb2000
# ADD CPP /Gt0 /QMOb2000 /Yu"stdafx.h"

$(INTDIR)/typtrmap.obj :  $(SOURCE)  $(DEP_TYPTR) $(INTDIR)\
 $(INTDIR)/stdafx.obj
   $(CPP) /nologo /MD /Gt0 /QMOb2000 /W3 /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D\
 "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"collect.pch"\
 /Yu"stdafx.h" /Fo$(INTDIR)/ /Fd$(OUTDIR)/"collect.pdb" /c  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\mapdwvw.cpp
DEP_MAPDW=\
	.\stdafx.h\
	.\collect.h\
	.\colledoc.h\
	.\mapdwvw.h

!IF  "$(CFG)" == "Win32 (80x86) Debug"

$(INTDIR)/mapdwvw.obj :  $(SOURCE)  $(DEP_MAPDW) $(INTDIR) $(INTDIR)/stdafx.obj
   $(CPP) /nologo /MD /W3 /GX /Zi /Od /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D\
 "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"collect.pch" /Yu"stdafx.h" /Fo$(INTDIR)/\
 /Fd$(OUTDIR)/"collect.pdb" /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (80x86) Release"

$(INTDIR)/mapdwvw.obj :  $(SOURCE)  $(DEP_MAPDW) $(INTDIR) $(INTDIR)/stdafx.obj
   $(CPP) /nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D\
 "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"collect.pch" /Yu"stdafx.h" /Fo$(INTDIR)/ /c\
  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Macintosh Debug"

# ADD CPP /Yu"stdafx.h"

$(INTDIR)/mapdwvw.obj :  $(SOURCE)  $(DEP_MAPDW) $(INTDIR) $(INTDIR)/stdafx.obj
   $(CPP) /nologo /AL /Gt1 /Q68s /Q68m /W3 /GX /Zi /Od /D "_WINDOWS" /D "_MAC"\
 /D "_68K_" /D "_DEBUG" /FR$(INTDIR)/ /Fp$(OUTDIR)/"collect.pch" /Yu"stdafx.h"\
 /Fo$(INTDIR)/ /Fd$(OUTDIR)/"collect.pdb" /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Macintosh Release"

# ADD CPP /Yu"stdafx.h"

$(INTDIR)/mapdwvw.obj :  $(SOURCE)  $(DEP_MAPDW) $(INTDIR) $(INTDIR)/stdafx.obj
   $(CPP) /nologo /AL /Gt1 /Q68s /W3 /GX /O2 /D "_WINDOWS" /D "_MAC" /D "_68K_"\
 /D "NDEBUG" /FR$(INTDIR)/ /Fp$(OUTDIR)/"collect.pch" /Yu"stdafx.h"\
 /Fo$(INTDIR)/ /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

# ADD BASE CPP /Gt0 /QMOb2000
# ADD CPP /Gt0 /QMOb2000 /Yu"stdafx.h"

$(INTDIR)/mapdwvw.obj :  $(SOURCE)  $(DEP_MAPDW) $(INTDIR) $(INTDIR)/stdafx.obj
   $(CPP) /nologo /MD /Gt0 /QMOb2000 /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D\
 "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"collect.pch"\
 /Yu"stdafx.h" /Fo$(INTDIR)/ /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

# ADD BASE CPP /Gt0 /QMOb2000
# ADD CPP /Gt0 /QMOb2000 /Yu"stdafx.h"

$(INTDIR)/mapdwvw.obj :  $(SOURCE)  $(DEP_MAPDW) $(INTDIR) $(INTDIR)/stdafx.obj
   $(CPP) /nologo /MD /Gt0 /QMOb2000 /W3 /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D\
 "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"collect.pch"\
 /Yu"stdafx.h" /Fo$(INTDIR)/ /Fd$(OUTDIR)/"collect.pdb" /c  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\collmac.r

!IF  "$(CFG)" == "Win32 (80x86) Debug"

!ELSEIF  "$(CFG)" == "Win32 (80x86) Release"

!ELSEIF  "$(CFG)" == "Macintosh Debug"

$(INTDIR)/collmac.rsc :  $(SOURCE)  $(INTDIR)
   $(MRC) /o$(INTDIR)/"collmac.rsc" /D "_MAC" /D "_68K_" /D "_DEBUG"  $(SOURCE)\
 

!ELSEIF  "$(CFG)" == "Macintosh Release"

$(INTDIR)/collmac.rsc :  $(SOURCE)  $(INTDIR)
   $(MRC) /o$(INTDIR)/"collmac.rsc" /D "_MAC" /D "_68K_" /D "NDEBUG"  $(SOURCE)\
 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

!ENDIF 

# End Source File
# End Group
# End Project
################################################################################
