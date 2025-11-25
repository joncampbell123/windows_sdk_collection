# Microsoft Visual C++ Generated NMAKE File, Format Version 2.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

!IF "$(CFG)" == ""
CFG=Win32 Debug
!MESSAGE No configuration specified.  Defaulting to Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "Win32 Release" && "$(CFG)" != "Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "IKLOWNS.MAK" CFG="Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

################################################################################
# Begin Project
# PROP Target_Last_Scanned "Win32 Debug"
MTL=MkTypLib.exe
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "WinRel"
# PROP BASE Intermediate_Dir "WinRel"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "WinRel"
# PROP Intermediate_Dir "WinRel"
OUTDIR=.\WinRel
INTDIR=.\WinRel

ALL : $(OUTDIR)/IKLOWNS.ovl $(OUTDIR)/IKLOWNS.bsc

$(OUTDIR) : 
    if not exist $(OUTDIR)/nul mkdir $(OUTDIR)

# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
# ADD BASE CPP /nologo /W3 /GX /YX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /FR /c
# ADD CPP /nologo /G4 /MT /W3 /GX /YX /O2 /I "..\include" /I "..\..\inc" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /c
# SUBTRACT CPP /Fr
CPP_PROJ=/nologo /G4 /MT /W3 /GX /YX /O2 /I "..\include" /I "..\..\inc" \
 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" \
 /Fp$(OUTDIR)/"IKLOWNS.pch" /Fo$(INTDIR)/ /c 
CPP_OBJS=.\WinRel/
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
RSC_PROJ=/l 0x409 /fo$(INTDIR)/"IKLOWNS.res" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_SBRS= \
    
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o$(OUTDIR)/"IKLOWNS.bsc" 

$(OUTDIR)/IKLOWNS.bsc : $(OUTDIR)  $(BSC32_SBRS)
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /NOLOGO /SUBSYSTEM:windows,4.0 /MACHINE:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib winmm.lib msacm32.lib /NOLOGO /SUBSYSTEM:windows,4.0 /DEBUG /MACHINE:I386
# SUBTRACT LINK32 /INCREMENTAL:yes
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib winmm.lib msacm32.lib\
 /NOLOGO /SUBSYSTEM:windows,4.0 /INCREMENTAL:no /PDB:$(OUTDIR)/"IKLOWNS.pdb" /DEBUG\
 /MACHINE:I386 /OUT:$(OUTDIR)/"IKLOWNS.ovl" 
DEF_FILE=
LINK32_OBJS= \
    $(INTDIR)/CGSCREEN.OBJ \
    $(INTDIR)/CGDIB.OBJ \
    $(INTDIR)/iklowns.obj \
    $(INTDIR)/CGGLOBL.OBJ \
    $(INTDIR)/IKLOWNS.res \
    $(INTDIR)/CGBITBUF.OBJ \
    $(INTDIR)/CGIMAGE.OBJ \
    $(INTDIR)/CGCHAR.OBJ \
    $(INTDIR)/CGSPRITE.OBJ \
    $(INTDIR)/CGACTION.OBJ \
    ..\LIB\DEBUG\LINKLIST.LIB \
    $(INTDIR)/CGUPDATE.OBJ \
    ..\LIB\DEBUG\cgutil.lib \
    $(INTDIR)/CGLEVEL.OBJ \
    $(INTDIR)/CGTIMER.OBJ \
    $(INTDIR)/CGINPUT.OBJ \
    $(INTDIR)/CGGRAPH.OBJ \
    $(INTDIR)/CGDLIST.OBJ \
    $(INTDIR)/CGCHRINT.OBJ \
    ..\..\LIB\DDRAW.LIB \
    $(INTDIR)/CGREMOTE.OBJ \
    $(INTDIR)/CGREMQUE.OBJ \
    ..\..\LIB\DPLAY.LIB \
    $(INTDIR)/cgmidi.obj \
    $(INTDIR)/cgsound.obj \
    $(INTDIR)/CGWAVE.OBJ \
    ..\..\LIB\DSOUND.LIB \
    $(INTDIR)/CGOPTION.OBJ \
    $(INTDIR)/CGTEXT.OBJ \
    $(INTDIR)/CGLOAD.OBJ

$(OUTDIR)/IKLOWNS.ovl : $(OUTDIR)  $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<
    copy $(OUTDIR)\IKLOWNS.OVL IKLOWNS.OVL

!ELSEIF  "$(CFG)" == "Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "WinDebug"
# PROP BASE Intermediate_Dir "WinDebug"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "WinDebug"
# PROP Intermediate_Dir "WinDebug"
OUTDIR=.\WinDebug
INTDIR=.\WinDebug

ALL : $(OUTDIR)/IKLOWNS.ovl $(OUTDIR)/IKLOWNS.bsc

$(OUTDIR) : 
    if not exist $(OUTDIR)/nul mkdir $(OUTDIR)

# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
# ADD BASE CPP /nologo /W3 /GX /Zi /YX /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /FR /c
# ADD CPP /nologo /G4 /MT /W3 /GX /Zi /YX /Od /I "..\include" /I "..\..\inc" /D "_DEBUG" /D "DEBUG" /D "WIN32" /D "_WINDOWS" /c
# SUBTRACT CPP /Fr
CPP_PROJ=/nologo /G4 /MT /W3 /GX /Zi /YX /Od /I "..\include" /I "..\..\inc"\
 /D "_DEBUG" /D "DEBUG" /D "WIN32" /D\
 "_WINDOWS" /D "WINVER=0x0400" /Fp$(OUTDIR)/"IKLOWNS.pch" /Fo$(INTDIR)/\
 /Fd$(OUTDIR)/"IKLOWNS.pdb" /c 
CPP_OBJS=.\WinDebug/
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
RSC_PROJ=/l 0x409 /fo$(INTDIR)/"IKLOWNS.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_SBRS= \
    
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o$(OUTDIR)/"IKLOWNS.bsc" 

$(OUTDIR)/IKLOWNS.bsc : $(OUTDIR)  $(BSC32_SBRS)
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /NOLOGO /SUBSYSTEM:windows,4.0 /DEBUG /MACHINE:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib winmm.lib msacm32.lib /NOLOGO /SUBSYSTEM:windows,4.0 /INCREMENTAL:no /DEBUG /MACHINE:I386
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib winmm.lib msacm32.lib\
 /NOLOGO /SUBSYSTEM:windows,4.0 /INCREMENTAL:no /PDB:$(OUTDIR)/"IKLOWNS.pdb" /DEBUG\
 /MACHINE:I386 /OUT:$(OUTDIR)/"IKLOWNS.ovl" 
DEF_FILE=
LINK32_OBJS= \
    $(INTDIR)/CGSCREEN.OBJ \
    $(INTDIR)/CGDIB.OBJ \
    $(INTDIR)/iklowns.obj \
    $(INTDIR)/CGGLOBL.OBJ \
    $(INTDIR)/IKLOWNS.res \
    $(INTDIR)/CGBITBUF.OBJ \
    $(INTDIR)/CGIMAGE.OBJ \
    $(INTDIR)/CGCHAR.OBJ \
    $(INTDIR)/CGSPRITE.OBJ \
    $(INTDIR)/CGACTION.OBJ \
    ..\LIB\DEBUG\LINKLIST.LIB \
    $(INTDIR)/CGUPDATE.OBJ \
    ..\LIB\DEBUG\cgutil.lib \
    $(INTDIR)/CGLEVEL.OBJ \
    $(INTDIR)/CGTIMER.OBJ \
    $(INTDIR)/CGINPUT.OBJ \
    $(INTDIR)/CGGRAPH.OBJ \
    $(INTDIR)/CGDLIST.OBJ \
    $(INTDIR)/CGCHRINT.OBJ \
    ..\..\LIB\DDRAW.LIB \
    $(INTDIR)/CGREMOTE.OBJ \
    $(INTDIR)/CGREMQUE.OBJ \
    ..\..\LIB\DPLAY.LIB \
    $(INTDIR)/cgmidi.obj \
    $(INTDIR)/cgsound.obj \
    $(INTDIR)/CGWAVE.OBJ \
    ..\..\LIB\DSOUND.LIB \
    $(INTDIR)/CGOPTION.OBJ \
    $(INTDIR)/CGTEXT.OBJ \
    $(INTDIR)/CGLOAD.OBJ

$(OUTDIR)/IKLOWNS.ovl : $(OUTDIR)  $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<
    copy $(OUTDIR)\IKLOWNS.OVL IKLOWNS.OVL

!ENDIF 

.c{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

################################################################################
# Begin Group "Source Files"

################################################################################
# Begin Source File

SOURCE=.\CGSCREEN.CPP
DEP_CGSCR=\
    ..\INCLUDE\CGDEBUG.H\
    .\CGBITBUF.H\
    .\CGSCREEN.H\
    ..\..\inc\ddraw.h\
    ..\INCLUDE\CGDIB.H

$(INTDIR)/CGSCREEN.OBJ :  $(SOURCE)  $(DEP_CGSCR) $(INTDIR)

# End Source File
################################################################################
# Begin Source File

SOURCE=.\CGDIB.CPP
DEP_CGDIB=\
    .\CGGLOBL.H\
    .\cgrsrce.h\
    .\CGSCREEN.H\
    ..\INCLUDE\CGEXCPT.H\
    ..\INCLUDE\CGDEBUG.H\
    ..\INCLUDE\CGDIB.H\
    ..\..\inc\ddraw.h\
    .\CGBITBUF.H

$(INTDIR)/CGDIB.OBJ :  $(SOURCE)  $(DEP_CGDIB) $(INTDIR)

# End Source File
################################################################################
# Begin Source File

SOURCE=.\iklowns.cpp
DEP_IKLOW=\
    ..\INCLUDE\STRREC.H\
    .\CGGLOBL.H\
    .\cgrsrce.h\
    ..\INCLUDE\CGEXCPT.H\
    .\CGIMAGE.H\
    ..\INCLUDE\CGDIB.H\
    .\CGCHAR.H\
    .\CGLEVEL.H\
    .\CGTIMER.H\
    .\CGINPUT.H\
    .\CGMIDI.H\
    .\CGOPTION.H\
    .\CGLOAD.H\
    .\IKLOWNS.H\
    .\CGSCREEN.H\
    .\CGBITBUF.H\
    .\CGGRAPH.H\
    .\CGUPDATE.H\
    .\CGACTION.H\
    .\CGCHDLL.H\
    .\CGREMOTE.H\
    .\CGDLIST.H\
    .\cgsound.h\
    .\CGTEXT.H\
    ..\..\inc\ddraw.h\
    ..\INCLUDE\LINKLIST.H\
    .\CGSPRITE.H\
    ..\..\inc\DPLAY.H\
    .\CGREMQUE.H\
    ..\..\inc\dsound.h

$(INTDIR)/iklowns.obj :  $(SOURCE)  $(DEP_IKLOW) $(INTDIR)

# End Source File
################################################################################
# Begin Source File

SOURCE=.\CGGLOBL.CPP
DEP_CGGLO=\
    .\CGGLOBL.H

$(INTDIR)/CGGLOBL.OBJ :  $(SOURCE)  $(DEP_CGGLO) $(INTDIR)

# End Source File
################################################################################
# Begin Source File

SOURCE=.\IKLOWNS.RC
DEP_IKLOWN=\
    .\IKLOWNS.ICO\
    .\cgrsrce.h

$(INTDIR)/IKLOWNS.res :  $(SOURCE)  $(DEP_IKLOWN) $(INTDIR)
   $(RSC) $(RSC_PROJ)  $(SOURCE) 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\CGBITBUF.CPP
DEP_CGBIT=\
    ..\INCLUDE\CGDEBUG.H\
    ..\INCLUDE\CGDIB.H\
    .\CGBITBUF.H\
    ..\..\inc\ddraw.h

$(INTDIR)/CGBITBUF.OBJ :  $(SOURCE)  $(DEP_CGBIT) $(INTDIR)

# End Source File
################################################################################
# Begin Source File

SOURCE=.\CGIMAGE.CPP
DEP_CGIMA=\
    .\cgrsrce.h\
    ..\INCLUDE\CGEXCPT.H\
    ..\INCLUDE\CGDEBUG.H\
    .\CGGLOBL.H\
    .\CGSCREEN.H\
    .\CGLEVEL.H\
    .\CGUPDATE.H\
    .\CGIMAGE.H\
    ..\..\inc\ddraw.h\
    .\CGBITBUF.H\
    .\CGDLIST.H\
    ..\INCLUDE\LINKLIST.H\
    .\CGGRAPH.H\
    ..\INCLUDE\CGDIB.H

$(INTDIR)/CGIMAGE.OBJ :  $(SOURCE)  $(DEP_CGIMA) $(INTDIR)

# End Source File
################################################################################
# Begin Source File

SOURCE=.\CGCHAR.CPP
DEP_CGCHA=\
    ..\INCLUDE\CGDEBUG.H\
    .\CGTIMER.H\
    .\CGINPUT.H\
    .\CGCHAR.H\
    .\CGCHDLL.H\
    .\CGCHRINT.H\
    .\CGREMOTE.H\
    ..\INCLUDE\STRREC.H\
    .\CGUPDATE.H\
    .\CGACTION.H\
    .\CGGRAPH.H\
    ..\INCLUDE\LINKLIST.H\
    ..\..\inc\DPLAY.H\
    .\CGREMQUE.H\
    .\CGSPRITE.H\
    .\cgsound.h\
    .\CGLEVEL.H\
    .\CGSCREEN.H\
    .\CGBITBUF.H\
    ..\..\inc\dsound.h\
    .\CGDLIST.H\
    ..\..\inc\ddraw.h\
    ..\INCLUDE\CGDIB.H

$(INTDIR)/CGCHAR.OBJ :  $(SOURCE)  $(DEP_CGCHA) $(INTDIR)

# End Source File
################################################################################
# Begin Source File

SOURCE=.\CGSPRITE.CPP
DEP_CGSPR=\
    ..\INCLUDE\CGDEBUG.H\
    .\CGGLOBL.H\
    .\CGTIMER.H\
    .\CGSPRITE.H\
    .\CGUPDATE.H\
    ..\INCLUDE\LINKLIST.H\
    .\CGBITBUF.H\
    .\CGSCREEN.H\
    ..\..\inc\ddraw.h\
    ..\INCLUDE\CGDIB.H

$(INTDIR)/CGSPRITE.OBJ :  $(SOURCE)  $(DEP_CGSPR) $(INTDIR)

# End Source File
################################################################################
# Begin Source File

SOURCE=.\CGACTION.CPP
DEP_CGACT=\
    .\CGGLOBL.H\
    ..\INCLUDE\CGDEBUG.H\
    .\CGSPRITE.H\
    ..\INCLUDE\STRREC.H\
    .\CGACTION.H\
    ..\INCLUDE\LINKLIST.H\
    .\CGBITBUF.H\
    .\cgsound.h\
    ..\..\inc\ddraw.h\
    ..\INCLUDE\CGDIB.H\
    ..\..\inc\dsound.h

$(INTDIR)/CGACTION.OBJ :  $(SOURCE)  $(DEP_CGACT) $(INTDIR)

# End Source File
################################################################################
# Begin Source File

SOURCE=..\LIB\DEBUG\LINKLIST.LIB
# End Source File
################################################################################
# Begin Source File

SOURCE=.\CGUPDATE.CPP
DEP_CGUPD=\
    .\CGUPDATE.H\
    .\CGGLOBL.H\
    ..\INCLUDE\LINKLIST.H

$(INTDIR)/CGUPDATE.OBJ :  $(SOURCE)  $(DEP_CGUPD) $(INTDIR)

# End Source File
################################################################################
# Begin Source File

SOURCE=..\LIB\DEBUG\cgutil.lib
# End Source File
################################################################################
# Begin Source File

SOURCE=.\CGLEVEL.CPP
DEP_CGLEV=\
    ..\INCLUDE\LINKLIST.H\
    .\CGGRAPH.H\
    .\CGLEVEL.H\
    .\CGTIMER.H\
    .\CGCHDLL.H\
    .\CGCHRINT.H\
    ..\INCLUDE\STRREC.H\
    .\CGINPUT.H\
    .\CGCHAR.H\
    .\CGIMAGE.H\
    .\CGMIDI.H\
    .\CGREMOTE.H\
    .\cgsound.h\
    .\CGGLOBL.H\
    .\CGSCREEN.H\
    .\CGUPDATE.H\
    .\CGDLIST.H\
    .\CGACTION.H\
    ..\..\inc\DPLAY.H\
    .\CGREMQUE.H\
    ..\..\inc\dsound.h\
    ..\..\inc\ddraw.h\
    .\CGBITBUF.H\
    .\CGSPRITE.H\
    ..\INCLUDE\CGDIB.H

$(INTDIR)/CGLEVEL.OBJ :  $(SOURCE)  $(DEP_CGLEV) $(INTDIR)

# End Source File
################################################################################
# Begin Source File

SOURCE=.\CGTIMER.CPP
DEP_CGTIM=\
    .\CGTIMER.H

$(INTDIR)/CGTIMER.OBJ :  $(SOURCE)  $(DEP_CGTIM) $(INTDIR)

# End Source File
################################################################################
# Begin Source File

SOURCE=.\CGINPUT.CPP
DEP_CGINP=\
    .\CGINPUT.H

$(INTDIR)/CGINPUT.OBJ :  $(SOURCE)  $(DEP_CGINP) $(INTDIR)

# End Source File
################################################################################
# Begin Source File

SOURCE=.\CGGRAPH.CPP
DEP_CGGRA=\
    .\CGGRAPH.H\
    .\CGLEVEL.H\
    .\CGSCREEN.H\
    .\CGUPDATE.H\
    .\CGDLIST.H\
    ..\..\inc\ddraw.h\
    .\CGBITBUF.H\
    ..\INCLUDE\LINKLIST.H\
    ..\INCLUDE\CGDIB.H

$(INTDIR)/CGGRAPH.OBJ :  $(SOURCE)  $(DEP_CGGRA) $(INTDIR)

# End Source File
################################################################################
# Begin Source File

SOURCE=.\CGDLIST.CPP
DEP_CGDLI=\
    .\cgrsrce.h\
    ..\INCLUDE\CGEXCPT.H\
    ..\INCLUDE\STRREC.H\
    ..\INCLUDE\CGDEBUG.H\
    .\CGCHAR.H\
    .\CGIMAGE.H\
    .\CGLOAD.H\
    .\CGDLIST.H\
    .\CGUPDATE.H\
    .\CGACTION.H\
    .\CGGRAPH.H\
    .\CGCHDLL.H\
    .\CGREMOTE.H\
    .\cgsound.h\
    .\CGTEXT.H\
    ..\INCLUDE\LINKLIST.H\
    .\CGSPRITE.H\
    .\CGLEVEL.H\
    .\CGSCREEN.H\
    ..\..\inc\DPLAY.H\
    .\CGREMQUE.H\
    ..\..\inc\dsound.h\
    .\CGBITBUF.H\
    ..\..\inc\ddraw.h\
    ..\INCLUDE\CGDIB.H

$(INTDIR)/CGDLIST.OBJ :  $(SOURCE)  $(DEP_CGDLI) $(INTDIR)

# End Source File
################################################################################
# Begin Source File

SOURCE=.\CGCHRINT.CPP
DEP_CGCHR=\
    .\CGCHDLL.H\
    .\CGLEVEL.H\
    .\CGCHAR.H\
    .\CGCHRINT.H\
    .\CGINPUT.H\
    .\CGDLIST.H\
    .\CGUPDATE.H\
    .\CGACTION.H\
    .\CGGRAPH.H\
    .\CGREMOTE.H\
    ..\INCLUDE\LINKLIST.H\
    .\CGSPRITE.H\
    .\cgsound.h\
    .\CGSCREEN.H\
    ..\..\inc\DPLAY.H\
    .\CGREMQUE.H\
    .\CGBITBUF.H\
    ..\..\inc\dsound.h\
    ..\..\inc\ddraw.h\
    ..\INCLUDE\CGDIB.H

$(INTDIR)/CGCHRINT.OBJ :  $(SOURCE)  $(DEP_CGCHR) $(INTDIR)

# End Source File
################################################################################
# Begin Source File

SOURCE=..\..\LIB\DDRAW.LIB
# End Source File
################################################################################
# Begin Source File

SOURCE=.\CGREMOTE.CPP
DEP_CGREM=\
    ..\..\inc\DPLAY.H\
    .\CGLEVEL.H\
    .\CGREMOTE.H\
    ..\INCLUDE\STRREC.H\
    .\CGDLIST.H\
    ..\INCLUDE\LINKLIST.H\
    .\CGREMQUE.H\
    .\CGUPDATE.H

$(INTDIR)/CGREMOTE.OBJ :  $(SOURCE)  $(DEP_CGREM) $(INTDIR)

# End Source File
################################################################################
# Begin Source File

SOURCE=.\CGREMQUE.CPP
DEP_CGREMQ=\
    ..\INCLUDE\LINKLIST.H\
    ..\..\inc\DPLAY.H\
    .\CGREMOTE.H\
    .\CGREMQUE.H

$(INTDIR)/CGREMQUE.OBJ :  $(SOURCE)  $(DEP_CGREMQ) $(INTDIR)

# End Source File
################################################################################
# Begin Source File

SOURCE=..\..\LIB\DPLAY.LIB
# End Source File
################################################################################
# Begin Source File

SOURCE=.\cgmidi.cpp
DEP_CGMID=\
    .\CGMIDI.H

$(INTDIR)/cgmidi.obj :  $(SOURCE)  $(DEP_CGMID) $(INTDIR)

# End Source File
################################################################################
# Begin Source File

SOURCE=.\cgsound.cpp
DEP_CGSOU=\
    ..\INCLUDE\LINKLIST.H\
    .\CGWAVE.H\
    .\cgsound.h\
    ..\..\inc\dsound.h

$(INTDIR)/cgsound.obj :  $(SOURCE)  $(DEP_CGSOU) $(INTDIR)

# End Source File
################################################################################
# Begin Source File

SOURCE=.\CGWAVE.CPP
DEP_CGWAV=\
    .\CGWAVE.H

$(INTDIR)/CGWAVE.OBJ :  $(SOURCE)  $(DEP_CGWAV) $(INTDIR)

# End Source File
################################################################################
# Begin Source File

SOURCE=..\..\LIB\DSOUND.LIB
# End Source File
################################################################################
# Begin Source File

SOURCE=.\CGOPTION.CPP
DEP_CGOPT=\
    .\CGGLOBL.H\
    ..\INCLUDE\CGDIB.H\
    ..\INCLUDE\STRREC.H\
    .\CGINPUT.H\
    .\CGTEXT.H\
    ..\INCLUDE\LINKLIST.H

$(INTDIR)/CGOPTION.OBJ :  $(SOURCE)  $(DEP_CGOPT) $(INTDIR)

# End Source File
################################################################################
# Begin Source File

SOURCE=.\CGTEXT.CPP
DEP_CGTEX=\
    .\CGTEXT.H\
    ..\INCLUDE\LINKLIST.H

$(INTDIR)/CGTEXT.OBJ :  $(SOURCE)  $(DEP_CGTEX) $(INTDIR)

# End Source File
################################################################################
# Begin Source File

SOURCE=.\CGLOAD.CPP
DEP_CGLOA=\
    .\CGGLOBL.H\
    ..\INCLUDE\STRREC.H\
    ..\INCLUDE\CGDIB.H\
    .\cgsound.h\
    .\CGLOAD.H\
    ..\..\inc\dsound.h\
    .\CGTEXT.H\
    ..\INCLUDE\LINKLIST.H

$(INTDIR)/CGLOAD.OBJ :  $(SOURCE)  $(DEP_CGLOA) $(INTDIR)

# End Source File
# End Group
# End Project
################################################################################
