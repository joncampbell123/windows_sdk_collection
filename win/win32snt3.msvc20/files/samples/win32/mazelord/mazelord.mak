# Microsoft Visual C++ Generated NMAKE File, Format Version 2.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101
# TARGTYPE "Win32 (MIPS) Application" 0x0501

!IF "$(CFG)" == ""
CFG=Win32 (80x86) Debug
!MESSAGE No configuration specified.  Defaulting to Win32 (80x86) Debug.
!ENDIF 

!IF "$(CFG)" != "Win32 (80x86) Release" && "$(CFG)" != "Win32 (80x86) Debug" &&\
 "$(CFG)" != "Win32 (MIPS) Debug" && "$(CFG)" != "Win32 (MIPS) Release"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "mazelord.mak" CFG="Win32 (80x86) Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Win32 (80x86) Release" (based on "Win32 (x86) Application")
!MESSAGE "Win32 (80x86) Debug" (based on "Win32 (x86) Application")
!MESSAGE "Win32 (MIPS) Debug" (based on "Win32 (MIPS) Application")
!MESSAGE "Win32 (MIPS) Release" (based on "Win32 (MIPS) Application")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

################################################################################
# Begin Project
# PROP Target_Last_Scanned "Win32 (MIPS) Debug"

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

ALL : $(OUTDIR)/mazelord.exe $(OUTDIR)/mazelord.bsc

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
 "WIN32" /FR$(INTDIR)/ /Fp$(OUTDIR)/"mazelord.pch" /Fo$(INTDIR)/ /c 
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
RSC_PROJ=/l 0x409 /fo$(INTDIR)/"maze.res" /d "NDEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# SUBTRACT BASE BSC32 /Iu
# ADD BSC32 /nologo
# SUBTRACT BSC32 /Iu
BSC32_FLAGS=/nologo /o$(OUTDIR)/"mazelord.bsc" 
BSC32_SBRS= \
	$(INTDIR)/topwnd.sbr \
	$(INTDIR)/readsgrd.sbr \
	$(INTDIR)/bitmap.sbr \
	$(INTDIR)/network.sbr \
	$(INTDIR)/scorewnd.sbr \
	$(INTDIR)/drones.sbr \
	$(INTDIR)/textwnd.sbr \
	$(INTDIR)/mazewnd.sbr \
	$(INTDIR)/initmaze.sbr \
	$(INTDIR)/mazedlg.sbr \
	$(INTDIR)/maze.sbr \
	$(INTDIR)/draw.sbr

$(OUTDIR)/mazelord.bsc : $(OUTDIR)  $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib mfc30.lib mfco30.lib mfcd30.lib /NOLOGO /SUBSYSTEM:windows /MACHINE:I386
# ADD LINK32 winmm.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /NOLOGO /SUBSYSTEM:windows /MACHINE:I386
LINK32_FLAGS=winmm.lib kernel32.lib user32.lib gdi32.lib winspool.lib\
 comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /NOLOGO\
 /SUBSYSTEM:windows /INCREMENTAL:no /PDB:$(OUTDIR)/"mazelord.pdb" /MACHINE:I386\
 /OUT:$(OUTDIR)/"mazelord.exe" 
DEF_FILE=
LINK32_OBJS= \
	$(INTDIR)/topwnd.obj \
	$(INTDIR)/readsgrd.obj \
	$(INTDIR)/bitmap.obj \
	$(INTDIR)/maze.res \
	$(INTDIR)/network.obj \
	$(INTDIR)/scorewnd.obj \
	$(INTDIR)/drones.obj \
	$(INTDIR)/textwnd.obj \
	$(INTDIR)/mazewnd.obj \
	$(INTDIR)/initmaze.obj \
	$(INTDIR)/mazedlg.obj \
	$(INTDIR)/maze.obj \
	$(INTDIR)/draw.obj

$(OUTDIR)/mazelord.exe : $(OUTDIR)  $(DEF_FILE) $(LINK32_OBJS)
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

ALL : $(OUTDIR)/mazelord.exe $(OUTDIR)/mazelord.bsc

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
 "WIN32" /FR$(INTDIR)/ /Fp$(OUTDIR)/"mazelord.pch" /Fo$(INTDIR)/\
 /Fd$(OUTDIR)/"mazelord.pdb" /c 
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
RSC_PROJ=/l 0x409 /fo$(INTDIR)/"maze.res" /d "_DEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# SUBTRACT BASE BSC32 /Iu
# ADD BSC32 /nologo
# SUBTRACT BSC32 /Iu
BSC32_FLAGS=/nologo /o$(OUTDIR)/"mazelord.bsc" 
BSC32_SBRS= \
	$(INTDIR)/topwnd.sbr \
	$(INTDIR)/readsgrd.sbr \
	$(INTDIR)/bitmap.sbr \
	$(INTDIR)/network.sbr \
	$(INTDIR)/scorewnd.sbr \
	$(INTDIR)/drones.sbr \
	$(INTDIR)/textwnd.sbr \
	$(INTDIR)/mazewnd.sbr \
	$(INTDIR)/initmaze.sbr \
	$(INTDIR)/mazedlg.sbr \
	$(INTDIR)/maze.sbr \
	$(INTDIR)/draw.sbr

$(OUTDIR)/mazelord.bsc : $(OUTDIR)  $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib mfc30d.lib mfco30d.lib mfcd30d.lib /NOLOGO /SUBSYSTEM:windows /DEBUG /MACHINE:I386
# ADD LINK32 winmm.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /NOLOGO /SUBSYSTEM:windows /DEBUG /MACHINE:I386
LINK32_FLAGS=winmm.lib kernel32.lib user32.lib gdi32.lib winspool.lib\
 comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /NOLOGO\
 /SUBSYSTEM:windows /INCREMENTAL:yes /PDB:$(OUTDIR)/"mazelord.pdb" /DEBUG\
 /MACHINE:I386 /OUT:$(OUTDIR)/"mazelord.exe" 
DEF_FILE=
LINK32_OBJS= \
	$(INTDIR)/topwnd.obj \
	$(INTDIR)/readsgrd.obj \
	$(INTDIR)/bitmap.obj \
	$(INTDIR)/maze.res \
	$(INTDIR)/network.obj \
	$(INTDIR)/scorewnd.obj \
	$(INTDIR)/drones.obj \
	$(INTDIR)/textwnd.obj \
	$(INTDIR)/mazewnd.obj \
	$(INTDIR)/initmaze.obj \
	$(INTDIR)/mazedlg.obj \
	$(INTDIR)/maze.obj \
	$(INTDIR)/draw.obj

$(OUTDIR)/mazelord.exe : $(OUTDIR)  $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 1
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "WinDebug"
# PROP Intermediate_Dir "WinDebug"
OUTDIR=.\WinDebug
INTDIR=.\WinDebug

ALL : $(OUTDIR)/mazelord.exe $(OUTDIR)/mazelord.bsc

$(OUTDIR) : 
    if not exist $(OUTDIR)/nul mkdir $(OUTDIR)

MTL=MkTypLib.exe
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
CPP=cl.exe
# ADD BASE CPP /nologo /MD /Gt0 /QMOb2000 /W3 /GX /Zi /YX /Od /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /c
# ADD CPP /nologo /Gt0 /QMOb2000 /W3 /GX /Zi /YX /Od /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "WIN32" /FR /c
CPP_PROJ=/nologo /Gt0 /QMOb2000 /W3 /GX /Zi /YX /Od /D "_DEBUG" /D "_WINDOWS"\
 /D "_MBCS" /D "WIN32" /FR$(INTDIR)/ /Fp$(OUTDIR)/"mazelord.pch" /Fo$(INTDIR)/\
 /Fd$(OUTDIR)/"mazelord.pdb" /c 
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
RSC_PROJ=/l 0x409 /fo$(INTDIR)/"maze.res" /d "_DEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# SUBTRACT BASE BSC32 /Iu
# ADD BSC32 /nologo
# SUBTRACT BSC32 /Iu
BSC32_FLAGS=/nologo /o$(OUTDIR)/"mazelord.bsc" 
BSC32_SBRS= \
	$(INTDIR)/topwnd.sbr \
	$(INTDIR)/readsgrd.sbr \
	$(INTDIR)/bitmap.sbr \
	$(INTDIR)/network.sbr \
	$(INTDIR)/scorewnd.sbr \
	$(INTDIR)/drones.sbr \
	$(INTDIR)/textwnd.sbr \
	$(INTDIR)/mazewnd.sbr \
	$(INTDIR)/initmaze.sbr \
	$(INTDIR)/mazedlg.sbr \
	$(INTDIR)/maze.sbr \
	$(INTDIR)/draw.sbr

$(OUTDIR)/mazelord.bsc : $(OUTDIR)  $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib odbc32.lib mfc30d.lib mfco30d.lib mfcd30d.lib oleaut32.lib uuid.lib /NOLOGO /SUBSYSTEM:windows /DEBUG /MACHINE:MIPS
# SUBTRACT BASE LINK32 /PDB:none
# ADD LINK32 winmm.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /NOLOGO /SUBSYSTEM:windows /DEBUG /MACHINE:MIPS
# SUBTRACT LINK32 /PDB:none
LINK32_FLAGS=winmm.lib kernel32.lib user32.lib gdi32.lib winspool.lib\
 comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /NOLOGO\
 /SUBSYSTEM:windows /PDB:$(OUTDIR)/"mazelord.pdb" /DEBUG /MACHINE:MIPS\
 /OUT:$(OUTDIR)/"mazelord.exe" 
DEF_FILE=
LINK32_OBJS= \
	$(INTDIR)/topwnd.obj \
	$(INTDIR)/readsgrd.obj \
	$(INTDIR)/bitmap.obj \
	$(INTDIR)/maze.res \
	$(INTDIR)/network.obj \
	$(INTDIR)/scorewnd.obj \
	$(INTDIR)/drones.obj \
	$(INTDIR)/textwnd.obj \
	$(INTDIR)/mazewnd.obj \
	$(INTDIR)/initmaze.obj \
	$(INTDIR)/mazedlg.obj \
	$(INTDIR)/maze.obj \
	$(INTDIR)/draw.obj

$(OUTDIR)/mazelord.exe : $(OUTDIR)  $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 0
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "WinRel"
# PROP Intermediate_Dir "WinRel"
OUTDIR=.\WinRel
INTDIR=.\WinRel

ALL : $(OUTDIR)/mazelord.exe $(OUTDIR)/mazelord.bsc

$(OUTDIR) : 
    if not exist $(OUTDIR)/nul mkdir $(OUTDIR)

MTL=MkTypLib.exe
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
CPP=cl.exe
# ADD BASE CPP /nologo /MD /Gt0 /QMOb2000 /W3 /GX /YX /O2 /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /c
# ADD CPP /nologo /Gt0 /QMOb2000 /W3 /GX /YX /O2 /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "WIN32" /FR /c
CPP_PROJ=/nologo /Gt0 /QMOb2000 /W3 /GX /YX /O2 /D "NDEBUG" /D "_WINDOWS" /D\
 "_MBCS" /D "WIN32" /FR$(INTDIR)/ /Fp$(OUTDIR)/"mazelord.pch" /Fo$(INTDIR)/ /c 
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
RSC_PROJ=/l 0x409 /fo$(INTDIR)/"maze.res" /d "NDEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# SUBTRACT BASE BSC32 /Iu
# ADD BSC32 /nologo
# SUBTRACT BSC32 /Iu
BSC32_FLAGS=/nologo /o$(OUTDIR)/"mazelord.bsc" 
BSC32_SBRS= \
	$(INTDIR)/topwnd.sbr \
	$(INTDIR)/readsgrd.sbr \
	$(INTDIR)/bitmap.sbr \
	$(INTDIR)/network.sbr \
	$(INTDIR)/scorewnd.sbr \
	$(INTDIR)/drones.sbr \
	$(INTDIR)/textwnd.sbr \
	$(INTDIR)/mazewnd.sbr \
	$(INTDIR)/initmaze.sbr \
	$(INTDIR)/mazedlg.sbr \
	$(INTDIR)/maze.sbr \
	$(INTDIR)/draw.sbr

$(OUTDIR)/mazelord.bsc : $(OUTDIR)  $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib odbc32.lib mfc30.lib mfco30.lib mfcd30.lib oleaut32.lib uuid.lib /NOLOGO /SUBSYSTEM:windows /MACHINE:MIPS
# SUBTRACT BASE LINK32 /PDB:none
# ADD LINK32 winmm.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /NOLOGO /SUBSYSTEM:windows /MACHINE:MIPS
# SUBTRACT LINK32 /PDB:none
LINK32_FLAGS=winmm.lib kernel32.lib user32.lib gdi32.lib winspool.lib\
 comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /NOLOGO\
 /SUBSYSTEM:windows /PDB:$(OUTDIR)/"mazelord.pdb" /MACHINE:MIPS\
 /OUT:$(OUTDIR)/"mazelord.exe" 
DEF_FILE=
LINK32_OBJS= \
	$(INTDIR)/topwnd.obj \
	$(INTDIR)/readsgrd.obj \
	$(INTDIR)/bitmap.obj \
	$(INTDIR)/maze.res \
	$(INTDIR)/network.obj \
	$(INTDIR)/scorewnd.obj \
	$(INTDIR)/drones.obj \
	$(INTDIR)/textwnd.obj \
	$(INTDIR)/mazewnd.obj \
	$(INTDIR)/initmaze.obj \
	$(INTDIR)/mazedlg.obj \
	$(INTDIR)/maze.obj \
	$(INTDIR)/draw.obj

$(OUTDIR)/mazelord.exe : $(OUTDIR)  $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 

################################################################################
# Begin Group "Source Files"

################################################################################
# Begin Source File

SOURCE=.\topwnd.c
DEP_TOPWN=\
	.\winmaze.h\
	.\mazproto.h\
	.\net.h\
	.\mazedlg.h

!IF  "$(CFG)" == "Win32 (80x86) Release"

$(INTDIR)/topwnd.obj :  $(SOURCE)  $(DEP_TOPWN) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

$(INTDIR)/topwnd.obj :  $(SOURCE)  $(DEP_TOPWN) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

$(INTDIR)/topwnd.obj :  $(SOURCE)  $(DEP_TOPWN) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

$(INTDIR)/topwnd.obj :  $(SOURCE)  $(DEP_TOPWN) $(INTDIR)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\readsgrd.c
DEP_READS=\
	.\winmaze.h\
	.\mazproto.h\
	.\net.h\
	.\mazedlg.h

!IF  "$(CFG)" == "Win32 (80x86) Release"

$(INTDIR)/readsgrd.obj :  $(SOURCE)  $(DEP_READS) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

$(INTDIR)/readsgrd.obj :  $(SOURCE)  $(DEP_READS) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

$(INTDIR)/readsgrd.obj :  $(SOURCE)  $(DEP_READS) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

$(INTDIR)/readsgrd.obj :  $(SOURCE)  $(DEP_READS) $(INTDIR)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\bitmap.c
DEP_BITMA=\
	.\winmaze.h\
	.\mazproto.h\
	.\net.h\
	.\mazedlg.h

!IF  "$(CFG)" == "Win32 (80x86) Release"

$(INTDIR)/bitmap.obj :  $(SOURCE)  $(DEP_BITMA) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

$(INTDIR)/bitmap.obj :  $(SOURCE)  $(DEP_BITMA) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

$(INTDIR)/bitmap.obj :  $(SOURCE)  $(DEP_BITMA) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

$(INTDIR)/bitmap.obj :  $(SOURCE)  $(DEP_BITMA) $(INTDIR)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\maze.rc
DEP_MAZE_=\
	.\maze.ico\
	.\rsc\fishf.bmp\
	.\rsc\fishl.bmp\
	.\rsc\fishr.bmp\
	.\rsc\fishb.bmp\
	.\rsc\fishfm.bmp\
	.\rsc\fishlm.bmp\
	.\rsc\fishrm.bmp\
	.\rsc\fishbm.bmp\
	.\rsc\reapf.bmp\
	.\rsc\reapl.bmp\
	.\rsc\reapr.bmp\
	.\rsc\reapb.bmp\
	.\rsc\reapfm.bmp\
	.\rsc\reaplm.bmp\
	.\rsc\reaprm.bmp\
	.\rsc\reapbm.bmp\
	.\rsc\robof.bmp\
	.\rsc\robol.bmp\
	.\rsc\robor.bmp\
	.\rsc\robob.bmp\
	.\rsc\robofm.bmp\
	.\rsc\robolm.bmp\
	.\rsc\roborm.bmp\
	.\rsc\robobm.bmp\
	.\rsc\smilf.bmp\
	.\rsc\smill.bmp\
	.\rsc\smilr.bmp\
	.\rsc\smilb.bmp\
	.\rsc\smilfm.bmp\
	.\rsc\smillm.bmp\
	.\rsc\smilrm.bmp\
	.\rsc\smilbm.bmp\
	.\rsc\fade1.bmp\
	.\rsc\fade2.bmp\
	.\rsc\shot1.bmp\
	.\rsc\shot2.bmp\
	.\rsc\maze.bmp\
	.\winmaze.h\
	.\mazedlg.dlg\
	.\grids.rc\
	.\net.h\
	.\mazedlg.h

!IF  "$(CFG)" == "Win32 (80x86) Release"

$(INTDIR)/maze.res :  $(SOURCE)  $(DEP_MAZE_) $(INTDIR)
   $(RSC) $(RSC_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

$(INTDIR)/maze.res :  $(SOURCE)  $(DEP_MAZE_) $(INTDIR)
   $(RSC) $(RSC_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

$(INTDIR)/maze.res :  $(SOURCE)  $(DEP_MAZE_) $(INTDIR)
   $(RSC) $(RSC_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

$(INTDIR)/maze.res :  $(SOURCE)  $(DEP_MAZE_) $(INTDIR)
   $(RSC) $(RSC_PROJ)  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\network.c
DEP_NETWO=\
	.\winmaze.h\
	.\mazproto.h\
	.\net.h\
	.\crctable.h\
	.\mazedlg.h

!IF  "$(CFG)" == "Win32 (80x86) Release"

$(INTDIR)/network.obj :  $(SOURCE)  $(DEP_NETWO) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

$(INTDIR)/network.obj :  $(SOURCE)  $(DEP_NETWO) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

$(INTDIR)/network.obj :  $(SOURCE)  $(DEP_NETWO) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

$(INTDIR)/network.obj :  $(SOURCE)  $(DEP_NETWO) $(INTDIR)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\scorewnd.c
DEP_SCORE=\
	.\winmaze.h\
	.\mazproto.h\
	.\net.h\
	.\mazedlg.h

!IF  "$(CFG)" == "Win32 (80x86) Release"

$(INTDIR)/scorewnd.obj :  $(SOURCE)  $(DEP_SCORE) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

$(INTDIR)/scorewnd.obj :  $(SOURCE)  $(DEP_SCORE) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

$(INTDIR)/scorewnd.obj :  $(SOURCE)  $(DEP_SCORE) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

$(INTDIR)/scorewnd.obj :  $(SOURCE)  $(DEP_SCORE) $(INTDIR)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\drones.c
DEP_DRONE=\
	.\winmaze.h\
	.\mazproto.h\
	.\net.h\
	.\mazedlg.h

!IF  "$(CFG)" == "Win32 (80x86) Release"

$(INTDIR)/drones.obj :  $(SOURCE)  $(DEP_DRONE) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

$(INTDIR)/drones.obj :  $(SOURCE)  $(DEP_DRONE) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

$(INTDIR)/drones.obj :  $(SOURCE)  $(DEP_DRONE) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

$(INTDIR)/drones.obj :  $(SOURCE)  $(DEP_DRONE) $(INTDIR)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\textwnd.c
DEP_TEXTW=\
	.\winmaze.h\
	.\mazproto.h\
	.\net.h\
	.\mazedlg.h

!IF  "$(CFG)" == "Win32 (80x86) Release"

$(INTDIR)/textwnd.obj :  $(SOURCE)  $(DEP_TEXTW) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

$(INTDIR)/textwnd.obj :  $(SOURCE)  $(DEP_TEXTW) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

$(INTDIR)/textwnd.obj :  $(SOURCE)  $(DEP_TEXTW) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

$(INTDIR)/textwnd.obj :  $(SOURCE)  $(DEP_TEXTW) $(INTDIR)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\mazewnd.c
DEP_MAZEW=\
	.\winmaze.h\
	.\mazproto.h\
	.\net.h\
	.\mazedlg.h

!IF  "$(CFG)" == "Win32 (80x86) Release"

$(INTDIR)/mazewnd.obj :  $(SOURCE)  $(DEP_MAZEW) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

$(INTDIR)/mazewnd.obj :  $(SOURCE)  $(DEP_MAZEW) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

$(INTDIR)/mazewnd.obj :  $(SOURCE)  $(DEP_MAZEW) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

$(INTDIR)/mazewnd.obj :  $(SOURCE)  $(DEP_MAZEW) $(INTDIR)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\initmaze.c
DEP_INITM=\
	.\winmaze.h\
	.\mazproto.h\
	.\net.h\
	.\mazedlg.h

!IF  "$(CFG)" == "Win32 (80x86) Release"

$(INTDIR)/initmaze.obj :  $(SOURCE)  $(DEP_INITM) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

$(INTDIR)/initmaze.obj :  $(SOURCE)  $(DEP_INITM) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

$(INTDIR)/initmaze.obj :  $(SOURCE)  $(DEP_INITM) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

$(INTDIR)/initmaze.obj :  $(SOURCE)  $(DEP_INITM) $(INTDIR)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\mazedlg.c
DEP_MAZED=\
	.\winmaze.h\
	.\mazproto.h\
	.\mazedlg.h\
	.\net.h

!IF  "$(CFG)" == "Win32 (80x86) Release"

$(INTDIR)/mazedlg.obj :  $(SOURCE)  $(DEP_MAZED) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

$(INTDIR)/mazedlg.obj :  $(SOURCE)  $(DEP_MAZED) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

$(INTDIR)/mazedlg.obj :  $(SOURCE)  $(DEP_MAZED) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

$(INTDIR)/mazedlg.obj :  $(SOURCE)  $(DEP_MAZED) $(INTDIR)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\maze.c
DEP_MAZE_C=\
	.\winmaze.h\
	.\mazproto.h\
	.\net.h\
	.\mazedlg.h

!IF  "$(CFG)" == "Win32 (80x86) Release"

$(INTDIR)/maze.obj :  $(SOURCE)  $(DEP_MAZE_C) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

$(INTDIR)/maze.obj :  $(SOURCE)  $(DEP_MAZE_C) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

$(INTDIR)/maze.obj :  $(SOURCE)  $(DEP_MAZE_C) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

$(INTDIR)/maze.obj :  $(SOURCE)  $(DEP_MAZE_C) $(INTDIR)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\draw.c
DEP_DRAW_=\
	.\winmaze.h\
	.\mazproto.h\
	.\net.h\
	.\mazedlg.h

!IF  "$(CFG)" == "Win32 (80x86) Release"

$(INTDIR)/draw.obj :  $(SOURCE)  $(DEP_DRAW_) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

$(INTDIR)/draw.obj :  $(SOURCE)  $(DEP_DRAW_) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

$(INTDIR)/draw.obj :  $(SOURCE)  $(DEP_DRAW_) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

$(INTDIR)/draw.obj :  $(SOURCE)  $(DEP_DRAW_) $(INTDIR)

!ENDIF 

# End Source File
# End Group
# End Project
################################################################################
