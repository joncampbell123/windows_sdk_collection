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
!MESSAGE NMAKE /f "icntrotl.mak" CFG="Win32 (80x86) Debug"
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
# PROP Output_Dir "..\WinRel"
# PROP Intermediate_Dir "icntrrel"
OUTDIR=..\WinRel
INTDIR=.\icntrrel

ALL : $(OUTDIR)/icntrotl.exe $(OUTDIR)/icntrotl.bsc

$(OUTDIR) : 
    if not exist $(OUTDIR)/nul mkdir $(OUTDIR)

$(INTDIR) : 
    if not exist $(INTDIR)/nul mkdir $(INTDIR)

MTL=MkTypLib.exe
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
CPP=cl.exe
# ADD BASE CPP /nologo /MD /W3 /GX /YX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /c
# ADD CPP /nologo /W3 /GX /O2 /I "..\include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "INC_OLE2" /D "OLE_CNTR" /D "INPLACE_CNTR" /D "OLE2FINAL" /D "OLE2SHIP" /D "WIN32S" /D "OLE_VERSION" /D "USE_MSGFILTER" /D "NOASSERT" /FR /Yu"outline.h" /c
CPP_PROJ=/nologo /W3 /GX /O2 /I "..\include" /D "NDEBUG" /D "WIN32" /D\
 "_WINDOWS" /D "_MBCS" /D "INC_OLE2" /D "OLE_CNTR" /D "INPLACE_CNTR" /D\
 "OLE2FINAL" /D "OLE2SHIP" /D "WIN32S" /D "OLE_VERSION" /D "USE_MSGFILTER" /D\
 "NOASSERT" /FR$(INTDIR)/ /Fp$(OUTDIR)/"icntrotl.pch" /Yu"outline.h"\
 /Fo$(INTDIR)/ /c 
CPP_OBJS=.\icntrrel/

.c{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

RSC=rc.exe
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG"
RSC_PROJ=/l 0x409 /fo$(INTDIR)/"icntrotl.res" /d "NDEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# SUBTRACT BASE BSC32 /Iu
# ADD BSC32 /nologo
# SUBTRACT BSC32 /Iu
BSC32_FLAGS=/nologo /o$(OUTDIR)/"icntrotl.bsc" 
BSC32_SBRS= \
	$(INTDIR)/dialogs.sbr \
	$(INTDIR)/outlapp.sbr \
	$(INTDIR)/cntrinpl.sbr \
	$(INTDIR)/frametls.sbr \
	$(INTDIR)/outltxtl.sbr \
	$(INTDIR)/heading.sbr \
	$(INTDIR)/status.sbr \
	$(INTDIR)/outllist.sbr \
	$(INTDIR)/linking.sbr \
	$(INTDIR)/main.sbr \
	$(INTDIR)/cntrbase.sbr \
	$(INTDIR)/outlname.sbr \
	$(INTDIR)/oledoc.sbr \
	$(INTDIR)/outlntbl.sbr \
	$(INTDIR)/cntrline.sbr \
	$(INTDIR)/outlline.sbr \
	$(INTDIR)/classfac.sbr \
	$(INTDIR)/outldoc.sbr \
	$(INTDIR)/debug2.sbr \
	$(INTDIR)/memmgr.sbr \
	$(INTDIR)/dragdrop.sbr \
	$(INTDIR)/icntrotl.sbr \
	$(INTDIR)/clipbrd.sbr \
	$(INTDIR)/oleapp.sbr \
	$(INTDIR)/debug.sbr

$(OUTDIR)/icntrotl.bsc : $(OUTDIR)  $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 /NOLOGO /SUBSYSTEM:windows /MACHINE:I386
# ADD LINK32 user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib ..\ole2ui\ole2ui.lib ..\bttncur\bttncur.lib ..\gizmobar\gizmobar.lib kernel32.lib /NOLOGO /SUBSYSTEM:windows /MACHINE:I386
LINK32_FLAGS=user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib\
 shell32.lib ole32.lib oleaut32.lib uuid.lib ..\ole2ui\ole2ui.lib\
 ..\bttncur\bttncur.lib ..\gizmobar\gizmobar.lib kernel32.lib /NOLOGO\
 /SUBSYSTEM:windows /INCREMENTAL:no /PDB:$(OUTDIR)/"icntrotl.pdb" /MACHINE:I386\
 /OUT:$(OUTDIR)/"icntrotl.exe" 
DEF_FILE=
LINK32_OBJS= \
	$(INTDIR)/dialogs.obj \
	$(INTDIR)/outlapp.obj \
	$(INTDIR)/cntrinpl.obj \
	$(INTDIR)/frametls.obj \
	$(INTDIR)/outltxtl.obj \
	$(INTDIR)/heading.obj \
	$(INTDIR)/status.obj \
	$(INTDIR)/outllist.obj \
	$(INTDIR)/linking.obj \
	$(INTDIR)/main.obj \
	$(INTDIR)/cntrbase.obj \
	$(INTDIR)/outlname.obj \
	$(INTDIR)/icntrotl.res \
	$(INTDIR)/oledoc.obj \
	$(INTDIR)/outlntbl.obj \
	$(INTDIR)/cntrline.obj \
	$(INTDIR)/outlline.obj \
	$(INTDIR)/classfac.obj \
	$(INTDIR)/outldoc.obj \
	$(INTDIR)/debug2.obj \
	$(INTDIR)/memmgr.obj \
	$(INTDIR)/dragdrop.obj \
	$(INTDIR)/icntrotl.obj \
	$(INTDIR)/clipbrd.obj \
	$(INTDIR)/oleapp.obj \
	$(INTDIR)/debug.obj

$(OUTDIR)/icntrotl.exe : $(OUTDIR)  $(DEF_FILE) $(LINK32_OBJS)
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
# PROP Output_Dir "..\WinDebug"
# PROP Intermediate_Dir "icntrdbg"
OUTDIR=..\WinDebug
INTDIR=.\icntrdbg

ALL : $(OUTDIR)/icntrotl.exe $(OUTDIR)/icntrotl.bsc

$(OUTDIR) : 
    if not exist $(OUTDIR)/nul mkdir $(OUTDIR)

$(INTDIR) : 
    if not exist $(INTDIR)/nul mkdir $(INTDIR)

MTL=MkTypLib.exe
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
CPP=cl.exe
# ADD BASE CPP /nologo /MD /W3 /GX /Zi /YX /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /c
# ADD CPP /nologo /W3 /GX /Zi /Od /I "..\include" /D "DBG" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "INC_OLE2" /D "OLE_CNTR" /D "INPLACE_CNTR" /D "OLE2FINAL" /D "OLE2SHIP" /D "WIN32S" /D "OLE_VERSION" /D "USE_MSGFILTER" /D "NOASSERT" /FR /Yu"outline.h" /c
CPP_PROJ=/nologo /W3 /GX /Zi /Od /I "..\include" /D "DBG" /D "_DEBUG" /D\
 "WIN32" /D "_WINDOWS" /D "_MBCS" /D "INC_OLE2" /D "OLE_CNTR" /D "INPLACE_CNTR"\
 /D "OLE2FINAL" /D "OLE2SHIP" /D "WIN32S" /D "OLE_VERSION" /D "USE_MSGFILTER" /D\
 "NOASSERT" /FR$(INTDIR)/ /Fp$(OUTDIR)/"icntrotl.pch" /Yu"outline.h"\
 /Fo$(INTDIR)/ /Fd$(OUTDIR)/"icntrotl.pdb" /c 
CPP_OBJS=.\icntrdbg/

.c{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

RSC=rc.exe
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG"
RSC_PROJ=/l 0x409 /fo$(INTDIR)/"icntrotl.res" /d "_DEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# SUBTRACT BASE BSC32 /Iu
# ADD BSC32 /nologo
# SUBTRACT BSC32 /Iu
BSC32_FLAGS=/nologo /o$(OUTDIR)/"icntrotl.bsc" 
BSC32_SBRS= \
	$(INTDIR)/dialogs.sbr \
	$(INTDIR)/outlapp.sbr \
	$(INTDIR)/cntrinpl.sbr \
	$(INTDIR)/frametls.sbr \
	$(INTDIR)/outltxtl.sbr \
	$(INTDIR)/heading.sbr \
	$(INTDIR)/status.sbr \
	$(INTDIR)/outllist.sbr \
	$(INTDIR)/linking.sbr \
	$(INTDIR)/main.sbr \
	$(INTDIR)/cntrbase.sbr \
	$(INTDIR)/outlname.sbr \
	$(INTDIR)/oledoc.sbr \
	$(INTDIR)/outlntbl.sbr \
	$(INTDIR)/cntrline.sbr \
	$(INTDIR)/outlline.sbr \
	$(INTDIR)/classfac.sbr \
	$(INTDIR)/outldoc.sbr \
	$(INTDIR)/debug2.sbr \
	$(INTDIR)/memmgr.sbr \
	$(INTDIR)/dragdrop.sbr \
	$(INTDIR)/icntrotl.sbr \
	$(INTDIR)/clipbrd.sbr \
	$(INTDIR)/oleapp.sbr \
	$(INTDIR)/debug.sbr

$(OUTDIR)/icntrotl.bsc : $(OUTDIR)  $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 /NOLOGO /SUBSYSTEM:windows /DEBUG /MACHINE:I386
# ADD LINK32 user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib ..\ole2ui\ole2ui.lib ..\bttncur\bttncur.lib ..\gizmobar\gizmobar.lib kernel32.lib /NOLOGO /SUBSYSTEM:windows /DEBUG /MACHINE:I386
LINK32_FLAGS=user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib\
 shell32.lib ole32.lib oleaut32.lib uuid.lib ..\ole2ui\ole2ui.lib\
 ..\bttncur\bttncur.lib ..\gizmobar\gizmobar.lib kernel32.lib /NOLOGO\
 /SUBSYSTEM:windows /INCREMENTAL:yes /PDB:$(OUTDIR)/"icntrotl.pdb" /DEBUG\
 /MACHINE:I386 /OUT:$(OUTDIR)/"icntrotl.exe" 
DEF_FILE=
LINK32_OBJS= \
	$(INTDIR)/dialogs.obj \
	$(INTDIR)/outlapp.obj \
	$(INTDIR)/cntrinpl.obj \
	$(INTDIR)/frametls.obj \
	$(INTDIR)/outltxtl.obj \
	$(INTDIR)/heading.obj \
	$(INTDIR)/status.obj \
	$(INTDIR)/outllist.obj \
	$(INTDIR)/linking.obj \
	$(INTDIR)/main.obj \
	$(INTDIR)/cntrbase.obj \
	$(INTDIR)/outlname.obj \
	$(INTDIR)/icntrotl.res \
	$(INTDIR)/oledoc.obj \
	$(INTDIR)/outlntbl.obj \
	$(INTDIR)/cntrline.obj \
	$(INTDIR)/outlline.obj \
	$(INTDIR)/classfac.obj \
	$(INTDIR)/outldoc.obj \
	$(INTDIR)/debug2.obj \
	$(INTDIR)/memmgr.obj \
	$(INTDIR)/dragdrop.obj \
	$(INTDIR)/icntrotl.obj \
	$(INTDIR)/clipbrd.obj \
	$(INTDIR)/oleapp.obj \
	$(INTDIR)/debug.obj

$(OUTDIR)/icntrotl.exe : $(OUTDIR)  $(DEF_FILE) $(LINK32_OBJS)
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
# PROP Output_Dir "..\WinRel"
# PROP Intermediate_Dir "icntrrel"
OUTDIR=..\WinRel
INTDIR=.\icntrrel

ALL : $(OUTDIR)/icntrotl.exe $(OUTDIR)/icntrotl.bsc

$(OUTDIR) : 
    if not exist $(OUTDIR)/nul mkdir $(OUTDIR)

$(INTDIR) : 
    if not exist $(INTDIR)/nul mkdir $(INTDIR)

MTL=MkTypLib.exe
# ADD BASE MTL /nologo /D "NDEBUG" /mips
# ADD MTL /nologo /D "NDEBUG" /mips
MTL_PROJ=/nologo /D "NDEBUG" /mips 
CPP=cl.exe
# ADD BASE CPP /nologo /MD /Gt0 /QMOb2000 /W3 /GX /YX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /c
# ADD CPP /nologo /Gt0 /QMOb2000 /W3 /GX /O2 /I "..\include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "INC_OLE2" /D "OLE_CNTR" /D "INPLACE_CNTR" /D "OLE2FINAL" /D "OLE2SHIP" /D "WIN32S" /D "OLE_VERSION" /D "USE_MSGFILTER" /D "NOASSERT" /FR /Yu"outline.h" /c
CPP_PROJ=/nologo /Gt0 /QMOb2000 /W3 /GX /O2 /I "..\include" /D "NDEBUG" /D\
 "WIN32" /D "_WINDOWS" /D "_MBCS" /D "INC_OLE2" /D "OLE_CNTR" /D "INPLACE_CNTR"\
 /D "OLE2FINAL" /D "OLE2SHIP" /D "WIN32S" /D "OLE_VERSION" /D "USE_MSGFILTER" /D\
 "NOASSERT" /FR$(INTDIR)/ /Fp$(OUTDIR)/"icntrotl.pch" /Yu"outline.h"\
 /Fo$(INTDIR)/ /c 
CPP_OBJS=.\icntrrel/

.c{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

RSC=rc.exe
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG"
RSC_PROJ=/l 0x409 /fo$(INTDIR)/"icntrotl.res" /d "NDEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o$(OUTDIR)/"icntrotl.bsc" 
BSC32_SBRS= \
	$(INTDIR)/dialogs.sbr \
	$(INTDIR)/outlapp.sbr \
	$(INTDIR)/cntrinpl.sbr \
	$(INTDIR)/frametls.sbr \
	$(INTDIR)/outltxtl.sbr \
	$(INTDIR)/heading.sbr \
	$(INTDIR)/status.sbr \
	$(INTDIR)/outllist.sbr \
	$(INTDIR)/linking.sbr \
	$(INTDIR)/main.sbr \
	$(INTDIR)/cntrbase.sbr \
	$(INTDIR)/outlname.sbr \
	$(INTDIR)/oledoc.sbr \
	$(INTDIR)/outlntbl.sbr \
	$(INTDIR)/cntrline.sbr \
	$(INTDIR)/outlline.sbr \
	$(INTDIR)/classfac.sbr \
	$(INTDIR)/outldoc.sbr \
	$(INTDIR)/debug2.sbr \
	$(INTDIR)/memmgr.sbr \
	$(INTDIR)/dragdrop.sbr \
	$(INTDIR)/icntrotl.sbr \
	$(INTDIR)/clipbrd.sbr \
	$(INTDIR)/oleapp.sbr \
	$(INTDIR)/debug.sbr

$(OUTDIR)/icntrotl.bsc : $(OUTDIR)  $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 /NOLOGO /SUBSYSTEM:windows /MACHINE:MIPS
# ADD LINK32 user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib ..\ole2ui\ole2ui.lib ..\bttncur\bttncur.lib ..\gizmobar\gizmobar.lib kernel32.lib /NOLOGO /SUBSYSTEM:windows /MACHINE:MIPS
LINK32_FLAGS=user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib\
 shell32.lib ole32.lib oleaut32.lib uuid.lib ..\ole2ui\ole2ui.lib\
 ..\bttncur\bttncur.lib ..\gizmobar\gizmobar.lib kernel32.lib /NOLOGO\
 /SUBSYSTEM:windows /PDB:$(OUTDIR)/"icntrotl.pdb" /MACHINE:MIPS\
 /OUT:$(OUTDIR)/"icntrotl.exe" 
DEF_FILE=
LINK32_OBJS= \
	$(INTDIR)/dialogs.obj \
	$(INTDIR)/outlapp.obj \
	$(INTDIR)/cntrinpl.obj \
	$(INTDIR)/frametls.obj \
	$(INTDIR)/outltxtl.obj \
	$(INTDIR)/heading.obj \
	$(INTDIR)/status.obj \
	$(INTDIR)/outllist.obj \
	$(INTDIR)/linking.obj \
	$(INTDIR)/main.obj \
	$(INTDIR)/cntrbase.obj \
	$(INTDIR)/outlname.obj \
	$(INTDIR)/icntrotl.res \
	$(INTDIR)/oledoc.obj \
	$(INTDIR)/outlntbl.obj \
	$(INTDIR)/cntrline.obj \
	$(INTDIR)/outlline.obj \
	$(INTDIR)/classfac.obj \
	$(INTDIR)/outldoc.obj \
	$(INTDIR)/debug2.obj \
	$(INTDIR)/memmgr.obj \
	$(INTDIR)/dragdrop.obj \
	$(INTDIR)/icntrotl.obj \
	$(INTDIR)/clipbrd.obj \
	$(INTDIR)/oleapp.obj \
	$(INTDIR)/debug.obj

$(OUTDIR)/icntrotl.exe : $(OUTDIR)  $(DEF_FILE) $(LINK32_OBJS)
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
# PROP Output_Dir "..\WinDebug"
# PROP Intermediate_Dir "icntrdbg"
OUTDIR=..\WinDebug
INTDIR=.\icntrdbg

ALL : $(OUTDIR)/icntrotl.exe $(OUTDIR)/icntrotl.bsc

$(OUTDIR) : 
    if not exist $(OUTDIR)/nul mkdir $(OUTDIR)

$(INTDIR) : 
    if not exist $(INTDIR)/nul mkdir $(INTDIR)

MTL=MkTypLib.exe
# ADD BASE MTL /nologo /D "_DEBUG" /mips
# ADD MTL /nologo /D "_DEBUG" /mips
MTL_PROJ=/nologo /D "_DEBUG" /mips 
CPP=cl.exe
# ADD BASE CPP /nologo /MD /Gt0 /QMOb2000 /W3 /GX /Zi /YX /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /c
# ADD CPP /nologo /Gt0 /QMOb2000 /W3 /GX /Zi /Od /I "..\include" /D "DBG" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "INC_OLE2" /D "OLE_CNTR" /D "INPLACE_CNTR" /D "OLE2FINAL" /D "OLE2SHIP" /D "WIN32S" /D "OLE_VERSION" /D "USE_MSGFILTER" /D "NOASSERT" /FR /Yu"outline.h" /c
CPP_PROJ=/nologo /Gt0 /QMOb2000 /W3 /GX /Zi /Od /I "..\include" /D "DBG" /D\
 "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "INC_OLE2" /D "OLE_CNTR" /D\
 "INPLACE_CNTR" /D "OLE2FINAL" /D "OLE2SHIP" /D "WIN32S" /D "OLE_VERSION" /D\
 "USE_MSGFILTER" /D "NOASSERT" /FR$(INTDIR)/ /Fp$(OUTDIR)/"icntrotl.pch"\
 /Yu"outline.h" /Fo$(INTDIR)/ /Fd$(OUTDIR)/"icntrotl.pdb" /c 
CPP_OBJS=.\icntrdbg/

.c{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

RSC=rc.exe
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG"
RSC_PROJ=/l 0x409 /fo$(INTDIR)/"icntrotl.res" /d "_DEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o$(OUTDIR)/"icntrotl.bsc" 
BSC32_SBRS= \
	$(INTDIR)/dialogs.sbr \
	$(INTDIR)/outlapp.sbr \
	$(INTDIR)/cntrinpl.sbr \
	$(INTDIR)/frametls.sbr \
	$(INTDIR)/outltxtl.sbr \
	$(INTDIR)/heading.sbr \
	$(INTDIR)/status.sbr \
	$(INTDIR)/outllist.sbr \
	$(INTDIR)/linking.sbr \
	$(INTDIR)/main.sbr \
	$(INTDIR)/cntrbase.sbr \
	$(INTDIR)/outlname.sbr \
	$(INTDIR)/oledoc.sbr \
	$(INTDIR)/outlntbl.sbr \
	$(INTDIR)/cntrline.sbr \
	$(INTDIR)/outlline.sbr \
	$(INTDIR)/classfac.sbr \
	$(INTDIR)/outldoc.sbr \
	$(INTDIR)/debug2.sbr \
	$(INTDIR)/memmgr.sbr \
	$(INTDIR)/dragdrop.sbr \
	$(INTDIR)/icntrotl.sbr \
	$(INTDIR)/clipbrd.sbr \
	$(INTDIR)/oleapp.sbr \
	$(INTDIR)/debug.sbr

$(OUTDIR)/icntrotl.bsc : $(OUTDIR)  $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 /NOLOGO /SUBSYSTEM:windows /DEBUG /MACHINE:MIPS
# ADD LINK32 user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib ..\ole2ui\ole2ui.lib ..\bttncur\bttncur.lib ..\gizmobar\gizmobar.lib kernel32.lib /NOLOGO /SUBSYSTEM:windows /DEBUG /MACHINE:MIPS
LINK32_FLAGS=user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib\
 shell32.lib ole32.lib oleaut32.lib uuid.lib ..\ole2ui\ole2ui.lib\
 ..\bttncur\bttncur.lib ..\gizmobar\gizmobar.lib kernel32.lib /NOLOGO\
 /SUBSYSTEM:windows /PDB:$(OUTDIR)/"icntrotl.pdb" /DEBUG /MACHINE:MIPS\
 /OUT:$(OUTDIR)/"icntrotl.exe" 
DEF_FILE=
LINK32_OBJS= \
	$(INTDIR)/dialogs.obj \
	$(INTDIR)/outlapp.obj \
	$(INTDIR)/cntrinpl.obj \
	$(INTDIR)/frametls.obj \
	$(INTDIR)/outltxtl.obj \
	$(INTDIR)/heading.obj \
	$(INTDIR)/status.obj \
	$(INTDIR)/outllist.obj \
	$(INTDIR)/linking.obj \
	$(INTDIR)/main.obj \
	$(INTDIR)/cntrbase.obj \
	$(INTDIR)/outlname.obj \
	$(INTDIR)/icntrotl.res \
	$(INTDIR)/oledoc.obj \
	$(INTDIR)/outlntbl.obj \
	$(INTDIR)/cntrline.obj \
	$(INTDIR)/outlline.obj \
	$(INTDIR)/classfac.obj \
	$(INTDIR)/outldoc.obj \
	$(INTDIR)/debug2.obj \
	$(INTDIR)/memmgr.obj \
	$(INTDIR)/dragdrop.obj \
	$(INTDIR)/icntrotl.obj \
	$(INTDIR)/clipbrd.obj \
	$(INTDIR)/oleapp.obj \
	$(INTDIR)/debug.obj

$(OUTDIR)/icntrotl.exe : $(OUTDIR)  $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 

################################################################################
# Begin Group "Source Files"

################################################################################
# Begin Source File

SOURCE=.\dialogs.c
DEP_DIALO=\
	.\outline.h\
	.\outlrc.h\
	.\ansiapi.h\
	.\frametls.h\
	.\heading.h\
	.\oleoutl.h\
	..\include\bttncur.h\
	..\include\gizmobar.h\
	..\include\msgfiltr.h\
	.\defguid.h\
	.\svroutl.h\
	.\cntroutl.h\
	.\cntrrc.h

!IF  "$(CFG)" == "Win32 (80x86) Release"

# ADD CPP /Yu"outline.h"

$(INTDIR)/dialogs.obj :  $(SOURCE)  $(DEP_DIALO) $(INTDIR)\
 $(INTDIR)/icntrotl.obj
   $(CPP) /nologo /W3 /GX /O2 /I "..\include" /D "NDEBUG" /D "WIN32" /D\
 "_WINDOWS" /D "_MBCS" /D "INC_OLE2" /D "OLE_CNTR" /D "INPLACE_CNTR" /D\
 "OLE2FINAL" /D "OLE2SHIP" /D "WIN32S" /D "OLE_VERSION" /D "USE_MSGFILTER" /D\
 "NOASSERT" /FR$(INTDIR)/ /Fp$(OUTDIR)/"icntrotl.pch" /Yu"outline.h"\
 /Fo$(INTDIR)/ /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

# ADD CPP /Yu"outline.h"

$(INTDIR)/dialogs.obj :  $(SOURCE)  $(DEP_DIALO) $(INTDIR)\
 $(INTDIR)/icntrotl.obj
   $(CPP) /nologo /W3 /GX /Zi /Od /I "..\include" /D "DBG" /D "_DEBUG" /D\
 "WIN32" /D "_WINDOWS" /D "_MBCS" /D "INC_OLE2" /D "OLE_CNTR" /D "INPLACE_CNTR"\
 /D "OLE2FINAL" /D "OLE2SHIP" /D "WIN32S" /D "OLE_VERSION" /D "USE_MSGFILTER" /D\
 "NOASSERT" /FR$(INTDIR)/ /Fp$(OUTDIR)/"icntrotl.pch" /Yu"outline.h"\
 /Fo$(INTDIR)/ /Fd$(OUTDIR)/"icntrotl.pdb" /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

$(INTDIR)/dialogs.obj :  $(SOURCE)  $(DEP_DIALO) $(INTDIR)\
 $(INTDIR)/icntrotl.obj
   $(CPP) /nologo /Gt0 /QMOb2000 /W3 /GX /O2 /I "..\include" /D "NDEBUG" /D\
 "WIN32" /D "_WINDOWS" /D "_MBCS" /D "INC_OLE2" /D "OLE_CNTR" /D "INPLACE_CNTR"\
 /D "OLE2FINAL" /D "OLE2SHIP" /D "WIN32S" /D "OLE_VERSION" /D "USE_MSGFILTER" /D\
 "NOASSERT" /FR$(INTDIR)/ /Fp$(OUTDIR)/"icntrotl.pch" /Yu"outline.h"\
 /Fo$(INTDIR)/ /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

$(INTDIR)/dialogs.obj :  $(SOURCE)  $(DEP_DIALO) $(INTDIR)\
 $(INTDIR)/icntrotl.obj
   $(CPP) /nologo /Gt0 /QMOb2000 /W3 /GX /Zi /Od /I "..\include" /D "DBG" /D\
 "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "INC_OLE2" /D "OLE_CNTR" /D\
 "INPLACE_CNTR" /D "OLE2FINAL" /D "OLE2SHIP" /D "WIN32S" /D "OLE_VERSION" /D\
 "USE_MSGFILTER" /D "NOASSERT" /FR$(INTDIR)/ /Fp$(OUTDIR)/"icntrotl.pch"\
 /Yu"outline.h" /Fo$(INTDIR)/ /Fd$(OUTDIR)/"icntrotl.pdb" /c  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\outlapp.c
DEP_OUTLA=\
	.\outline.h\
	.\ansiapi.h\
	.\status.h\
	.\outlrc.h\
	.\frametls.h\
	.\heading.h\
	.\oleoutl.h\
	..\include\bttncur.h\
	..\include\gizmobar.h\
	..\include\msgfiltr.h\
	.\defguid.h\
	.\svroutl.h\
	.\cntroutl.h\
	.\cntrrc.h

!IF  "$(CFG)" == "Win32 (80x86) Release"

# ADD CPP /Yu"outline.h"

$(INTDIR)/outlapp.obj :  $(SOURCE)  $(DEP_OUTLA) $(INTDIR)\
 $(INTDIR)/icntrotl.obj
   $(CPP) /nologo /W3 /GX /O2 /I "..\include" /D "NDEBUG" /D "WIN32" /D\
 "_WINDOWS" /D "_MBCS" /D "INC_OLE2" /D "OLE_CNTR" /D "INPLACE_CNTR" /D\
 "OLE2FINAL" /D "OLE2SHIP" /D "WIN32S" /D "OLE_VERSION" /D "USE_MSGFILTER" /D\
 "NOASSERT" /FR$(INTDIR)/ /Fp$(OUTDIR)/"icntrotl.pch" /Yu"outline.h"\
 /Fo$(INTDIR)/ /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

# ADD CPP /Yu"outline.h"

$(INTDIR)/outlapp.obj :  $(SOURCE)  $(DEP_OUTLA) $(INTDIR)\
 $(INTDIR)/icntrotl.obj
   $(CPP) /nologo /W3 /GX /Zi /Od /I "..\include" /D "DBG" /D "_DEBUG" /D\
 "WIN32" /D "_WINDOWS" /D "_MBCS" /D "INC_OLE2" /D "OLE_CNTR" /D "INPLACE_CNTR"\
 /D "OLE2FINAL" /D "OLE2SHIP" /D "WIN32S" /D "OLE_VERSION" /D "USE_MSGFILTER" /D\
 "NOASSERT" /FR$(INTDIR)/ /Fp$(OUTDIR)/"icntrotl.pch" /Yu"outline.h"\
 /Fo$(INTDIR)/ /Fd$(OUTDIR)/"icntrotl.pdb" /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

$(INTDIR)/outlapp.obj :  $(SOURCE)  $(DEP_OUTLA) $(INTDIR)\
 $(INTDIR)/icntrotl.obj
   $(CPP) /nologo /Gt0 /QMOb2000 /W3 /GX /O2 /I "..\include" /D "NDEBUG" /D\
 "WIN32" /D "_WINDOWS" /D "_MBCS" /D "INC_OLE2" /D "OLE_CNTR" /D "INPLACE_CNTR"\
 /D "OLE2FINAL" /D "OLE2SHIP" /D "WIN32S" /D "OLE_VERSION" /D "USE_MSGFILTER" /D\
 "NOASSERT" /FR$(INTDIR)/ /Fp$(OUTDIR)/"icntrotl.pch" /Yu"outline.h"\
 /Fo$(INTDIR)/ /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

$(INTDIR)/outlapp.obj :  $(SOURCE)  $(DEP_OUTLA) $(INTDIR)\
 $(INTDIR)/icntrotl.obj
   $(CPP) /nologo /Gt0 /QMOb2000 /W3 /GX /Zi /Od /I "..\include" /D "DBG" /D\
 "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "INC_OLE2" /D "OLE_CNTR" /D\
 "INPLACE_CNTR" /D "OLE2FINAL" /D "OLE2SHIP" /D "WIN32S" /D "OLE_VERSION" /D\
 "USE_MSGFILTER" /D "NOASSERT" /FR$(INTDIR)/ /Fp$(OUTDIR)/"icntrotl.pch"\
 /Yu"outline.h" /Fo$(INTDIR)/ /Fd$(OUTDIR)/"icntrotl.pdb" /c  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\cntrinpl.c
DEP_CNTRI=\
	.\outline.h\
	.\status.h\
	.\outlrc.h\
	.\ansiapi.h\
	.\frametls.h\
	.\heading.h\
	.\oleoutl.h\
	..\include\bttncur.h\
	..\include\gizmobar.h\
	..\include\msgfiltr.h\
	.\defguid.h\
	.\svroutl.h\
	.\cntroutl.h\
	.\cntrrc.h

!IF  "$(CFG)" == "Win32 (80x86) Release"

# ADD CPP /Yu"outline.h"

$(INTDIR)/cntrinpl.obj :  $(SOURCE)  $(DEP_CNTRI) $(INTDIR)\
 $(INTDIR)/icntrotl.obj
   $(CPP) /nologo /W3 /GX /O2 /I "..\include" /D "NDEBUG" /D "WIN32" /D\
 "_WINDOWS" /D "_MBCS" /D "INC_OLE2" /D "OLE_CNTR" /D "INPLACE_CNTR" /D\
 "OLE2FINAL" /D "OLE2SHIP" /D "WIN32S" /D "OLE_VERSION" /D "USE_MSGFILTER" /D\
 "NOASSERT" /FR$(INTDIR)/ /Fp$(OUTDIR)/"icntrotl.pch" /Yu"outline.h"\
 /Fo$(INTDIR)/ /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

# ADD CPP /Yu"outline.h"

$(INTDIR)/cntrinpl.obj :  $(SOURCE)  $(DEP_CNTRI) $(INTDIR)\
 $(INTDIR)/icntrotl.obj
   $(CPP) /nologo /W3 /GX /Zi /Od /I "..\include" /D "DBG" /D "_DEBUG" /D\
 "WIN32" /D "_WINDOWS" /D "_MBCS" /D "INC_OLE2" /D "OLE_CNTR" /D "INPLACE_CNTR"\
 /D "OLE2FINAL" /D "OLE2SHIP" /D "WIN32S" /D "OLE_VERSION" /D "USE_MSGFILTER" /D\
 "NOASSERT" /FR$(INTDIR)/ /Fp$(OUTDIR)/"icntrotl.pch" /Yu"outline.h"\
 /Fo$(INTDIR)/ /Fd$(OUTDIR)/"icntrotl.pdb" /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

$(INTDIR)/cntrinpl.obj :  $(SOURCE)  $(DEP_CNTRI) $(INTDIR)\
 $(INTDIR)/icntrotl.obj
   $(CPP) /nologo /Gt0 /QMOb2000 /W3 /GX /O2 /I "..\include" /D "NDEBUG" /D\
 "WIN32" /D "_WINDOWS" /D "_MBCS" /D "INC_OLE2" /D "OLE_CNTR" /D "INPLACE_CNTR"\
 /D "OLE2FINAL" /D "OLE2SHIP" /D "WIN32S" /D "OLE_VERSION" /D "USE_MSGFILTER" /D\
 "NOASSERT" /FR$(INTDIR)/ /Fp$(OUTDIR)/"icntrotl.pch" /Yu"outline.h"\
 /Fo$(INTDIR)/ /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

$(INTDIR)/cntrinpl.obj :  $(SOURCE)  $(DEP_CNTRI) $(INTDIR)\
 $(INTDIR)/icntrotl.obj
   $(CPP) /nologo /Gt0 /QMOb2000 /W3 /GX /Zi /Od /I "..\include" /D "DBG" /D\
 "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "INC_OLE2" /D "OLE_CNTR" /D\
 "INPLACE_CNTR" /D "OLE2FINAL" /D "OLE2SHIP" /D "WIN32S" /D "OLE_VERSION" /D\
 "USE_MSGFILTER" /D "NOASSERT" /FR$(INTDIR)/ /Fp$(OUTDIR)/"icntrotl.pch"\
 /Yu"outline.h" /Fo$(INTDIR)/ /Fd$(OUTDIR)/"icntrotl.pdb" /c  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\frametls.c
DEP_FRAME=\
	.\outline.h\
	.\outlrc.h\
	.\ansiapi.h\
	.\frametls.h\
	.\heading.h\
	.\oleoutl.h\
	..\include\bttncur.h\
	..\include\gizmobar.h\
	..\include\msgfiltr.h\
	.\defguid.h\
	.\svroutl.h\
	.\cntroutl.h\
	.\cntrrc.h

!IF  "$(CFG)" == "Win32 (80x86) Release"

# ADD CPP /Yu"outline.h"

$(INTDIR)/frametls.obj :  $(SOURCE)  $(DEP_FRAME) $(INTDIR)\
 $(INTDIR)/icntrotl.obj
   $(CPP) /nologo /W3 /GX /O2 /I "..\include" /D "NDEBUG" /D "WIN32" /D\
 "_WINDOWS" /D "_MBCS" /D "INC_OLE2" /D "OLE_CNTR" /D "INPLACE_CNTR" /D\
 "OLE2FINAL" /D "OLE2SHIP" /D "WIN32S" /D "OLE_VERSION" /D "USE_MSGFILTER" /D\
 "NOASSERT" /FR$(INTDIR)/ /Fp$(OUTDIR)/"icntrotl.pch" /Yu"outline.h"\
 /Fo$(INTDIR)/ /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

# ADD CPP /Yu"outline.h"

$(INTDIR)/frametls.obj :  $(SOURCE)  $(DEP_FRAME) $(INTDIR)\
 $(INTDIR)/icntrotl.obj
   $(CPP) /nologo /W3 /GX /Zi /Od /I "..\include" /D "DBG" /D "_DEBUG" /D\
 "WIN32" /D "_WINDOWS" /D "_MBCS" /D "INC_OLE2" /D "OLE_CNTR" /D "INPLACE_CNTR"\
 /D "OLE2FINAL" /D "OLE2SHIP" /D "WIN32S" /D "OLE_VERSION" /D "USE_MSGFILTER" /D\
 "NOASSERT" /FR$(INTDIR)/ /Fp$(OUTDIR)/"icntrotl.pch" /Yu"outline.h"\
 /Fo$(INTDIR)/ /Fd$(OUTDIR)/"icntrotl.pdb" /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

$(INTDIR)/frametls.obj :  $(SOURCE)  $(DEP_FRAME) $(INTDIR)\
 $(INTDIR)/icntrotl.obj
   $(CPP) /nologo /Gt0 /QMOb2000 /W3 /GX /O2 /I "..\include" /D "NDEBUG" /D\
 "WIN32" /D "_WINDOWS" /D "_MBCS" /D "INC_OLE2" /D "OLE_CNTR" /D "INPLACE_CNTR"\
 /D "OLE2FINAL" /D "OLE2SHIP" /D "WIN32S" /D "OLE_VERSION" /D "USE_MSGFILTER" /D\
 "NOASSERT" /FR$(INTDIR)/ /Fp$(OUTDIR)/"icntrotl.pch" /Yu"outline.h"\
 /Fo$(INTDIR)/ /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

$(INTDIR)/frametls.obj :  $(SOURCE)  $(DEP_FRAME) $(INTDIR)\
 $(INTDIR)/icntrotl.obj
   $(CPP) /nologo /Gt0 /QMOb2000 /W3 /GX /Zi /Od /I "..\include" /D "DBG" /D\
 "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "INC_OLE2" /D "OLE_CNTR" /D\
 "INPLACE_CNTR" /D "OLE2FINAL" /D "OLE2SHIP" /D "WIN32S" /D "OLE_VERSION" /D\
 "USE_MSGFILTER" /D "NOASSERT" /FR$(INTDIR)/ /Fp$(OUTDIR)/"icntrotl.pch"\
 /Yu"outline.h" /Fo$(INTDIR)/ /Fd$(OUTDIR)/"icntrotl.pdb" /c  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\outltxtl.c
DEP_OUTLT=\
	.\outline.h\
	.\outlrc.h\
	.\ansiapi.h\
	.\frametls.h\
	.\heading.h\
	.\oleoutl.h\
	..\include\bttncur.h\
	..\include\gizmobar.h\
	..\include\msgfiltr.h\
	.\defguid.h\
	.\svroutl.h\
	.\cntroutl.h\
	.\cntrrc.h

!IF  "$(CFG)" == "Win32 (80x86) Release"

# ADD CPP /Yu"outline.h"

$(INTDIR)/outltxtl.obj :  $(SOURCE)  $(DEP_OUTLT) $(INTDIR)\
 $(INTDIR)/icntrotl.obj
   $(CPP) /nologo /W3 /GX /O2 /I "..\include" /D "NDEBUG" /D "WIN32" /D\
 "_WINDOWS" /D "_MBCS" /D "INC_OLE2" /D "OLE_CNTR" /D "INPLACE_CNTR" /D\
 "OLE2FINAL" /D "OLE2SHIP" /D "WIN32S" /D "OLE_VERSION" /D "USE_MSGFILTER" /D\
 "NOASSERT" /FR$(INTDIR)/ /Fp$(OUTDIR)/"icntrotl.pch" /Yu"outline.h"\
 /Fo$(INTDIR)/ /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

# ADD CPP /Yu"outline.h"

$(INTDIR)/outltxtl.obj :  $(SOURCE)  $(DEP_OUTLT) $(INTDIR)\
 $(INTDIR)/icntrotl.obj
   $(CPP) /nologo /W3 /GX /Zi /Od /I "..\include" /D "DBG" /D "_DEBUG" /D\
 "WIN32" /D "_WINDOWS" /D "_MBCS" /D "INC_OLE2" /D "OLE_CNTR" /D "INPLACE_CNTR"\
 /D "OLE2FINAL" /D "OLE2SHIP" /D "WIN32S" /D "OLE_VERSION" /D "USE_MSGFILTER" /D\
 "NOASSERT" /FR$(INTDIR)/ /Fp$(OUTDIR)/"icntrotl.pch" /Yu"outline.h"\
 /Fo$(INTDIR)/ /Fd$(OUTDIR)/"icntrotl.pdb" /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

$(INTDIR)/outltxtl.obj :  $(SOURCE)  $(DEP_OUTLT) $(INTDIR)\
 $(INTDIR)/icntrotl.obj
   $(CPP) /nologo /Gt0 /QMOb2000 /W3 /GX /O2 /I "..\include" /D "NDEBUG" /D\
 "WIN32" /D "_WINDOWS" /D "_MBCS" /D "INC_OLE2" /D "OLE_CNTR" /D "INPLACE_CNTR"\
 /D "OLE2FINAL" /D "OLE2SHIP" /D "WIN32S" /D "OLE_VERSION" /D "USE_MSGFILTER" /D\
 "NOASSERT" /FR$(INTDIR)/ /Fp$(OUTDIR)/"icntrotl.pch" /Yu"outline.h"\
 /Fo$(INTDIR)/ /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

$(INTDIR)/outltxtl.obj :  $(SOURCE)  $(DEP_OUTLT) $(INTDIR)\
 $(INTDIR)/icntrotl.obj
   $(CPP) /nologo /Gt0 /QMOb2000 /W3 /GX /Zi /Od /I "..\include" /D "DBG" /D\
 "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "INC_OLE2" /D "OLE_CNTR" /D\
 "INPLACE_CNTR" /D "OLE2FINAL" /D "OLE2SHIP" /D "WIN32S" /D "OLE_VERSION" /D\
 "USE_MSGFILTER" /D "NOASSERT" /FR$(INTDIR)/ /Fp$(OUTDIR)/"icntrotl.pch"\
 /Yu"outline.h" /Fo$(INTDIR)/ /Fd$(OUTDIR)/"icntrotl.pdb" /c  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\heading.c
DEP_HEADI=\
	.\outline.h\
	.\outlrc.h\
	.\ansiapi.h\
	.\frametls.h\
	.\heading.h\
	.\oleoutl.h\
	..\include\bttncur.h\
	..\include\gizmobar.h\
	..\include\msgfiltr.h\
	.\defguid.h\
	.\svroutl.h\
	.\cntroutl.h\
	.\cntrrc.h

!IF  "$(CFG)" == "Win32 (80x86) Release"

# ADD CPP /Yu"outline.h"

$(INTDIR)/heading.obj :  $(SOURCE)  $(DEP_HEADI) $(INTDIR)\
 $(INTDIR)/icntrotl.obj
   $(CPP) /nologo /W3 /GX /O2 /I "..\include" /D "NDEBUG" /D "WIN32" /D\
 "_WINDOWS" /D "_MBCS" /D "INC_OLE2" /D "OLE_CNTR" /D "INPLACE_CNTR" /D\
 "OLE2FINAL" /D "OLE2SHIP" /D "WIN32S" /D "OLE_VERSION" /D "USE_MSGFILTER" /D\
 "NOASSERT" /FR$(INTDIR)/ /Fp$(OUTDIR)/"icntrotl.pch" /Yu"outline.h"\
 /Fo$(INTDIR)/ /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

# ADD CPP /Yu"outline.h"

$(INTDIR)/heading.obj :  $(SOURCE)  $(DEP_HEADI) $(INTDIR)\
 $(INTDIR)/icntrotl.obj
   $(CPP) /nologo /W3 /GX /Zi /Od /I "..\include" /D "DBG" /D "_DEBUG" /D\
 "WIN32" /D "_WINDOWS" /D "_MBCS" /D "INC_OLE2" /D "OLE_CNTR" /D "INPLACE_CNTR"\
 /D "OLE2FINAL" /D "OLE2SHIP" /D "WIN32S" /D "OLE_VERSION" /D "USE_MSGFILTER" /D\
 "NOASSERT" /FR$(INTDIR)/ /Fp$(OUTDIR)/"icntrotl.pch" /Yu"outline.h"\
 /Fo$(INTDIR)/ /Fd$(OUTDIR)/"icntrotl.pdb" /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

$(INTDIR)/heading.obj :  $(SOURCE)  $(DEP_HEADI) $(INTDIR)\
 $(INTDIR)/icntrotl.obj
   $(CPP) /nologo /Gt0 /QMOb2000 /W3 /GX /O2 /I "..\include" /D "NDEBUG" /D\
 "WIN32" /D "_WINDOWS" /D "_MBCS" /D "INC_OLE2" /D "OLE_CNTR" /D "INPLACE_CNTR"\
 /D "OLE2FINAL" /D "OLE2SHIP" /D "WIN32S" /D "OLE_VERSION" /D "USE_MSGFILTER" /D\
 "NOASSERT" /FR$(INTDIR)/ /Fp$(OUTDIR)/"icntrotl.pch" /Yu"outline.h"\
 /Fo$(INTDIR)/ /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

$(INTDIR)/heading.obj :  $(SOURCE)  $(DEP_HEADI) $(INTDIR)\
 $(INTDIR)/icntrotl.obj
   $(CPP) /nologo /Gt0 /QMOb2000 /W3 /GX /Zi /Od /I "..\include" /D "DBG" /D\
 "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "INC_OLE2" /D "OLE_CNTR" /D\
 "INPLACE_CNTR" /D "OLE2FINAL" /D "OLE2SHIP" /D "WIN32S" /D "OLE_VERSION" /D\
 "USE_MSGFILTER" /D "NOASSERT" /FR$(INTDIR)/ /Fp$(OUTDIR)/"icntrotl.pch"\
 /Yu"outline.h" /Fo$(INTDIR)/ /Fd$(OUTDIR)/"icntrotl.pdb" /c  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\status.c
DEP_STATU=\
	.\outline.h\
	.\message.h\
	.\status.h\
	.\outlrc.h\
	.\ansiapi.h\
	.\frametls.h\
	.\heading.h\
	.\oleoutl.h\
	..\include\bttncur.h\
	..\include\gizmobar.h\
	..\include\msgfiltr.h\
	.\defguid.h\
	.\svroutl.h\
	.\cntroutl.h\
	.\cntrrc.h

!IF  "$(CFG)" == "Win32 (80x86) Release"

# ADD CPP /Yu"outline.h"

$(INTDIR)/status.obj :  $(SOURCE)  $(DEP_STATU) $(INTDIR)\
 $(INTDIR)/icntrotl.obj
   $(CPP) /nologo /W3 /GX /O2 /I "..\include" /D "NDEBUG" /D "WIN32" /D\
 "_WINDOWS" /D "_MBCS" /D "INC_OLE2" /D "OLE_CNTR" /D "INPLACE_CNTR" /D\
 "OLE2FINAL" /D "OLE2SHIP" /D "WIN32S" /D "OLE_VERSION" /D "USE_MSGFILTER" /D\
 "NOASSERT" /FR$(INTDIR)/ /Fp$(OUTDIR)/"icntrotl.pch" /Yu"outline.h"\
 /Fo$(INTDIR)/ /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

# ADD CPP /Yu"outline.h"

$(INTDIR)/status.obj :  $(SOURCE)  $(DEP_STATU) $(INTDIR)\
 $(INTDIR)/icntrotl.obj
   $(CPP) /nologo /W3 /GX /Zi /Od /I "..\include" /D "DBG" /D "_DEBUG" /D\
 "WIN32" /D "_WINDOWS" /D "_MBCS" /D "INC_OLE2" /D "OLE_CNTR" /D "INPLACE_CNTR"\
 /D "OLE2FINAL" /D "OLE2SHIP" /D "WIN32S" /D "OLE_VERSION" /D "USE_MSGFILTER" /D\
 "NOASSERT" /FR$(INTDIR)/ /Fp$(OUTDIR)/"icntrotl.pch" /Yu"outline.h"\
 /Fo$(INTDIR)/ /Fd$(OUTDIR)/"icntrotl.pdb" /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

$(INTDIR)/status.obj :  $(SOURCE)  $(DEP_STATU) $(INTDIR)\
 $(INTDIR)/icntrotl.obj
   $(CPP) /nologo /Gt0 /QMOb2000 /W3 /GX /O2 /I "..\include" /D "NDEBUG" /D\
 "WIN32" /D "_WINDOWS" /D "_MBCS" /D "INC_OLE2" /D "OLE_CNTR" /D "INPLACE_CNTR"\
 /D "OLE2FINAL" /D "OLE2SHIP" /D "WIN32S" /D "OLE_VERSION" /D "USE_MSGFILTER" /D\
 "NOASSERT" /FR$(INTDIR)/ /Fp$(OUTDIR)/"icntrotl.pch" /Yu"outline.h"\
 /Fo$(INTDIR)/ /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

$(INTDIR)/status.obj :  $(SOURCE)  $(DEP_STATU) $(INTDIR)\
 $(INTDIR)/icntrotl.obj
   $(CPP) /nologo /Gt0 /QMOb2000 /W3 /GX /Zi /Od /I "..\include" /D "DBG" /D\
 "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "INC_OLE2" /D "OLE_CNTR" /D\
 "INPLACE_CNTR" /D "OLE2FINAL" /D "OLE2SHIP" /D "WIN32S" /D "OLE_VERSION" /D\
 "USE_MSGFILTER" /D "NOASSERT" /FR$(INTDIR)/ /Fp$(OUTDIR)/"icntrotl.pch"\
 /Yu"outline.h" /Fo$(INTDIR)/ /Fd$(OUTDIR)/"icntrotl.pdb" /c  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\outllist.c
DEP_OUTLL=\
	.\outline.h\
	.\outlrc.h\
	.\ansiapi.h\
	.\frametls.h\
	.\heading.h\
	.\oleoutl.h\
	..\include\bttncur.h\
	..\include\gizmobar.h\
	..\include\msgfiltr.h\
	.\defguid.h\
	.\svroutl.h\
	.\cntroutl.h\
	.\cntrrc.h

!IF  "$(CFG)" == "Win32 (80x86) Release"

# ADD CPP /Yu"outline.h"

$(INTDIR)/outllist.obj :  $(SOURCE)  $(DEP_OUTLL) $(INTDIR)\
 $(INTDIR)/icntrotl.obj
   $(CPP) /nologo /W3 /GX /O2 /I "..\include" /D "NDEBUG" /D "WIN32" /D\
 "_WINDOWS" /D "_MBCS" /D "INC_OLE2" /D "OLE_CNTR" /D "INPLACE_CNTR" /D\
 "OLE2FINAL" /D "OLE2SHIP" /D "WIN32S" /D "OLE_VERSION" /D "USE_MSGFILTER" /D\
 "NOASSERT" /FR$(INTDIR)/ /Fp$(OUTDIR)/"icntrotl.pch" /Yu"outline.h"\
 /Fo$(INTDIR)/ /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

# ADD CPP /Yu"outline.h"

$(INTDIR)/outllist.obj :  $(SOURCE)  $(DEP_OUTLL) $(INTDIR)\
 $(INTDIR)/icntrotl.obj
   $(CPP) /nologo /W3 /GX /Zi /Od /I "..\include" /D "DBG" /D "_DEBUG" /D\
 "WIN32" /D "_WINDOWS" /D "_MBCS" /D "INC_OLE2" /D "OLE_CNTR" /D "INPLACE_CNTR"\
 /D "OLE2FINAL" /D "OLE2SHIP" /D "WIN32S" /D "OLE_VERSION" /D "USE_MSGFILTER" /D\
 "NOASSERT" /FR$(INTDIR)/ /Fp$(OUTDIR)/"icntrotl.pch" /Yu"outline.h"\
 /Fo$(INTDIR)/ /Fd$(OUTDIR)/"icntrotl.pdb" /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

$(INTDIR)/outllist.obj :  $(SOURCE)  $(DEP_OUTLL) $(INTDIR)\
 $(INTDIR)/icntrotl.obj
   $(CPP) /nologo /Gt0 /QMOb2000 /W3 /GX /O2 /I "..\include" /D "NDEBUG" /D\
 "WIN32" /D "_WINDOWS" /D "_MBCS" /D "INC_OLE2" /D "OLE_CNTR" /D "INPLACE_CNTR"\
 /D "OLE2FINAL" /D "OLE2SHIP" /D "WIN32S" /D "OLE_VERSION" /D "USE_MSGFILTER" /D\
 "NOASSERT" /FR$(INTDIR)/ /Fp$(OUTDIR)/"icntrotl.pch" /Yu"outline.h"\
 /Fo$(INTDIR)/ /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

$(INTDIR)/outllist.obj :  $(SOURCE)  $(DEP_OUTLL) $(INTDIR)\
 $(INTDIR)/icntrotl.obj
   $(CPP) /nologo /Gt0 /QMOb2000 /W3 /GX /Zi /Od /I "..\include" /D "DBG" /D\
 "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "INC_OLE2" /D "OLE_CNTR" /D\
 "INPLACE_CNTR" /D "OLE2FINAL" /D "OLE2SHIP" /D "WIN32S" /D "OLE_VERSION" /D\
 "USE_MSGFILTER" /D "NOASSERT" /FR$(INTDIR)/ /Fp$(OUTDIR)/"icntrotl.pch"\
 /Yu"outline.h" /Fo$(INTDIR)/ /Fd$(OUTDIR)/"icntrotl.pdb" /c  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\linking.c
DEP_LINKI=\
	.\outline.h\
	.\outlrc.h\
	.\ansiapi.h\
	.\frametls.h\
	.\heading.h\
	.\oleoutl.h\
	..\include\bttncur.h\
	..\include\gizmobar.h\
	..\include\msgfiltr.h\
	.\defguid.h\
	.\svroutl.h\
	.\cntroutl.h\
	.\cntrrc.h

!IF  "$(CFG)" == "Win32 (80x86) Release"

# ADD CPP /Yu"outline.h"

$(INTDIR)/linking.obj :  $(SOURCE)  $(DEP_LINKI) $(INTDIR)\
 $(INTDIR)/icntrotl.obj
   $(CPP) /nologo /W3 /GX /O2 /I "..\include" /D "NDEBUG" /D "WIN32" /D\
 "_WINDOWS" /D "_MBCS" /D "INC_OLE2" /D "OLE_CNTR" /D "INPLACE_CNTR" /D\
 "OLE2FINAL" /D "OLE2SHIP" /D "WIN32S" /D "OLE_VERSION" /D "USE_MSGFILTER" /D\
 "NOASSERT" /FR$(INTDIR)/ /Fp$(OUTDIR)/"icntrotl.pch" /Yu"outline.h"\
 /Fo$(INTDIR)/ /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

# ADD CPP /Yu"outline.h"

$(INTDIR)/linking.obj :  $(SOURCE)  $(DEP_LINKI) $(INTDIR)\
 $(INTDIR)/icntrotl.obj
   $(CPP) /nologo /W3 /GX /Zi /Od /I "..\include" /D "DBG" /D "_DEBUG" /D\
 "WIN32" /D "_WINDOWS" /D "_MBCS" /D "INC_OLE2" /D "OLE_CNTR" /D "INPLACE_CNTR"\
 /D "OLE2FINAL" /D "OLE2SHIP" /D "WIN32S" /D "OLE_VERSION" /D "USE_MSGFILTER" /D\
 "NOASSERT" /FR$(INTDIR)/ /Fp$(OUTDIR)/"icntrotl.pch" /Yu"outline.h"\
 /Fo$(INTDIR)/ /Fd$(OUTDIR)/"icntrotl.pdb" /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

$(INTDIR)/linking.obj :  $(SOURCE)  $(DEP_LINKI) $(INTDIR)\
 $(INTDIR)/icntrotl.obj
   $(CPP) /nologo /Gt0 /QMOb2000 /W3 /GX /O2 /I "..\include" /D "NDEBUG" /D\
 "WIN32" /D "_WINDOWS" /D "_MBCS" /D "INC_OLE2" /D "OLE_CNTR" /D "INPLACE_CNTR"\
 /D "OLE2FINAL" /D "OLE2SHIP" /D "WIN32S" /D "OLE_VERSION" /D "USE_MSGFILTER" /D\
 "NOASSERT" /FR$(INTDIR)/ /Fp$(OUTDIR)/"icntrotl.pch" /Yu"outline.h"\
 /Fo$(INTDIR)/ /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

$(INTDIR)/linking.obj :  $(SOURCE)  $(DEP_LINKI) $(INTDIR)\
 $(INTDIR)/icntrotl.obj
   $(CPP) /nologo /Gt0 /QMOb2000 /W3 /GX /Zi /Od /I "..\include" /D "DBG" /D\
 "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "INC_OLE2" /D "OLE_CNTR" /D\
 "INPLACE_CNTR" /D "OLE2FINAL" /D "OLE2SHIP" /D "WIN32S" /D "OLE_VERSION" /D\
 "USE_MSGFILTER" /D "NOASSERT" /FR$(INTDIR)/ /Fp$(OUTDIR)/"icntrotl.pch"\
 /Yu"outline.h" /Fo$(INTDIR)/ /Fd$(OUTDIR)/"icntrotl.pdb" /c  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\main.c
DEP_MAIN_=\
	.\ansiapi.h\
	.\outline.h\
	.\status.h\
	.\defguid.h\
	.\outlrc.h\
	.\frametls.h\
	.\heading.h\
	.\oleoutl.h\
	..\include\bttncur.h\
	..\include\gizmobar.h\
	..\include\msgfiltr.h\
	.\svroutl.h\
	.\cntroutl.h\
	.\cntrrc.h

!IF  "$(CFG)" == "Win32 (80x86) Release"

# ADD CPP /Yu"outline.h"

$(INTDIR)/main.obj :  $(SOURCE)  $(DEP_MAIN_) $(INTDIR) $(INTDIR)/icntrotl.obj
   $(CPP) /nologo /W3 /GX /O2 /I "..\include" /D "NDEBUG" /D "WIN32" /D\
 "_WINDOWS" /D "_MBCS" /D "INC_OLE2" /D "OLE_CNTR" /D "INPLACE_CNTR" /D\
 "OLE2FINAL" /D "OLE2SHIP" /D "WIN32S" /D "OLE_VERSION" /D "USE_MSGFILTER" /D\
 "NOASSERT" /FR$(INTDIR)/ /Fp$(OUTDIR)/"icntrotl.pch" /Yu"outline.h"\
 /Fo$(INTDIR)/ /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

# ADD CPP /Yu"outline.h"

$(INTDIR)/main.obj :  $(SOURCE)  $(DEP_MAIN_) $(INTDIR) $(INTDIR)/icntrotl.obj
   $(CPP) /nologo /W3 /GX /Zi /Od /I "..\include" /D "DBG" /D "_DEBUG" /D\
 "WIN32" /D "_WINDOWS" /D "_MBCS" /D "INC_OLE2" /D "OLE_CNTR" /D "INPLACE_CNTR"\
 /D "OLE2FINAL" /D "OLE2SHIP" /D "WIN32S" /D "OLE_VERSION" /D "USE_MSGFILTER" /D\
 "NOASSERT" /FR$(INTDIR)/ /Fp$(OUTDIR)/"icntrotl.pch" /Yu"outline.h"\
 /Fo$(INTDIR)/ /Fd$(OUTDIR)/"icntrotl.pdb" /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

$(INTDIR)/main.obj :  $(SOURCE)  $(DEP_MAIN_) $(INTDIR) $(INTDIR)/icntrotl.obj
   $(CPP) /nologo /Gt0 /QMOb2000 /W3 /GX /O2 /I "..\include" /D "NDEBUG" /D\
 "WIN32" /D "_WINDOWS" /D "_MBCS" /D "INC_OLE2" /D "OLE_CNTR" /D "INPLACE_CNTR"\
 /D "OLE2FINAL" /D "OLE2SHIP" /D "WIN32S" /D "OLE_VERSION" /D "USE_MSGFILTER" /D\
 "NOASSERT" /FR$(INTDIR)/ /Fp$(OUTDIR)/"icntrotl.pch" /Yu"outline.h"\
 /Fo$(INTDIR)/ /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

$(INTDIR)/main.obj :  $(SOURCE)  $(DEP_MAIN_) $(INTDIR) $(INTDIR)/icntrotl.obj
   $(CPP) /nologo /Gt0 /QMOb2000 /W3 /GX /Zi /Od /I "..\include" /D "DBG" /D\
 "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "INC_OLE2" /D "OLE_CNTR" /D\
 "INPLACE_CNTR" /D "OLE2FINAL" /D "OLE2SHIP" /D "WIN32S" /D "OLE_VERSION" /D\
 "USE_MSGFILTER" /D "NOASSERT" /FR$(INTDIR)/ /Fp$(OUTDIR)/"icntrotl.pch"\
 /Yu"outline.h" /Fo$(INTDIR)/ /Fd$(OUTDIR)/"icntrotl.pdb" /c  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\cntrbase.c
DEP_CNTRB=\
	.\outline.h\
	.\outlrc.h\
	.\ansiapi.h\
	.\frametls.h\
	.\heading.h\
	.\oleoutl.h\
	..\include\bttncur.h\
	..\include\gizmobar.h\
	..\include\msgfiltr.h\
	.\defguid.h\
	.\svroutl.h\
	.\cntroutl.h\
	.\cntrrc.h

!IF  "$(CFG)" == "Win32 (80x86) Release"

# ADD CPP /Yu"outline.h"

$(INTDIR)/cntrbase.obj :  $(SOURCE)  $(DEP_CNTRB) $(INTDIR)\
 $(INTDIR)/icntrotl.obj
   $(CPP) /nologo /W3 /GX /O2 /I "..\include" /D "NDEBUG" /D "WIN32" /D\
 "_WINDOWS" /D "_MBCS" /D "INC_OLE2" /D "OLE_CNTR" /D "INPLACE_CNTR" /D\
 "OLE2FINAL" /D "OLE2SHIP" /D "WIN32S" /D "OLE_VERSION" /D "USE_MSGFILTER" /D\
 "NOASSERT" /FR$(INTDIR)/ /Fp$(OUTDIR)/"icntrotl.pch" /Yu"outline.h"\
 /Fo$(INTDIR)/ /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

# ADD CPP /Yu"outline.h"

$(INTDIR)/cntrbase.obj :  $(SOURCE)  $(DEP_CNTRB) $(INTDIR)\
 $(INTDIR)/icntrotl.obj
   $(CPP) /nologo /W3 /GX /Zi /Od /I "..\include" /D "DBG" /D "_DEBUG" /D\
 "WIN32" /D "_WINDOWS" /D "_MBCS" /D "INC_OLE2" /D "OLE_CNTR" /D "INPLACE_CNTR"\
 /D "OLE2FINAL" /D "OLE2SHIP" /D "WIN32S" /D "OLE_VERSION" /D "USE_MSGFILTER" /D\
 "NOASSERT" /FR$(INTDIR)/ /Fp$(OUTDIR)/"icntrotl.pch" /Yu"outline.h"\
 /Fo$(INTDIR)/ /Fd$(OUTDIR)/"icntrotl.pdb" /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

$(INTDIR)/cntrbase.obj :  $(SOURCE)  $(DEP_CNTRB) $(INTDIR)\
 $(INTDIR)/icntrotl.obj
   $(CPP) /nologo /Gt0 /QMOb2000 /W3 /GX /O2 /I "..\include" /D "NDEBUG" /D\
 "WIN32" /D "_WINDOWS" /D "_MBCS" /D "INC_OLE2" /D "OLE_CNTR" /D "INPLACE_CNTR"\
 /D "OLE2FINAL" /D "OLE2SHIP" /D "WIN32S" /D "OLE_VERSION" /D "USE_MSGFILTER" /D\
 "NOASSERT" /FR$(INTDIR)/ /Fp$(OUTDIR)/"icntrotl.pch" /Yu"outline.h"\
 /Fo$(INTDIR)/ /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

$(INTDIR)/cntrbase.obj :  $(SOURCE)  $(DEP_CNTRB) $(INTDIR)\
 $(INTDIR)/icntrotl.obj
   $(CPP) /nologo /Gt0 /QMOb2000 /W3 /GX /Zi /Od /I "..\include" /D "DBG" /D\
 "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "INC_OLE2" /D "OLE_CNTR" /D\
 "INPLACE_CNTR" /D "OLE2FINAL" /D "OLE2SHIP" /D "WIN32S" /D "OLE_VERSION" /D\
 "USE_MSGFILTER" /D "NOASSERT" /FR$(INTDIR)/ /Fp$(OUTDIR)/"icntrotl.pch"\
 /Yu"outline.h" /Fo$(INTDIR)/ /Fd$(OUTDIR)/"icntrotl.pdb" /c  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\outlname.c
DEP_OUTLN=\
	.\outline.h\
	.\outlrc.h\
	.\ansiapi.h\
	.\frametls.h\
	.\heading.h\
	.\oleoutl.h\
	..\include\bttncur.h\
	..\include\gizmobar.h\
	..\include\msgfiltr.h\
	.\defguid.h\
	.\svroutl.h\
	.\cntroutl.h\
	.\cntrrc.h

!IF  "$(CFG)" == "Win32 (80x86) Release"

# ADD CPP /Yu"outline.h"

$(INTDIR)/outlname.obj :  $(SOURCE)  $(DEP_OUTLN) $(INTDIR)\
 $(INTDIR)/icntrotl.obj
   $(CPP) /nologo /W3 /GX /O2 /I "..\include" /D "NDEBUG" /D "WIN32" /D\
 "_WINDOWS" /D "_MBCS" /D "INC_OLE2" /D "OLE_CNTR" /D "INPLACE_CNTR" /D\
 "OLE2FINAL" /D "OLE2SHIP" /D "WIN32S" /D "OLE_VERSION" /D "USE_MSGFILTER" /D\
 "NOASSERT" /FR$(INTDIR)/ /Fp$(OUTDIR)/"icntrotl.pch" /Yu"outline.h"\
 /Fo$(INTDIR)/ /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

# ADD CPP /Yu"outline.h"

$(INTDIR)/outlname.obj :  $(SOURCE)  $(DEP_OUTLN) $(INTDIR)\
 $(INTDIR)/icntrotl.obj
   $(CPP) /nologo /W3 /GX /Zi /Od /I "..\include" /D "DBG" /D "_DEBUG" /D\
 "WIN32" /D "_WINDOWS" /D "_MBCS" /D "INC_OLE2" /D "OLE_CNTR" /D "INPLACE_CNTR"\
 /D "OLE2FINAL" /D "OLE2SHIP" /D "WIN32S" /D "OLE_VERSION" /D "USE_MSGFILTER" /D\
 "NOASSERT" /FR$(INTDIR)/ /Fp$(OUTDIR)/"icntrotl.pch" /Yu"outline.h"\
 /Fo$(INTDIR)/ /Fd$(OUTDIR)/"icntrotl.pdb" /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

$(INTDIR)/outlname.obj :  $(SOURCE)  $(DEP_OUTLN) $(INTDIR)\
 $(INTDIR)/icntrotl.obj
   $(CPP) /nologo /Gt0 /QMOb2000 /W3 /GX /O2 /I "..\include" /D "NDEBUG" /D\
 "WIN32" /D "_WINDOWS" /D "_MBCS" /D "INC_OLE2" /D "OLE_CNTR" /D "INPLACE_CNTR"\
 /D "OLE2FINAL" /D "OLE2SHIP" /D "WIN32S" /D "OLE_VERSION" /D "USE_MSGFILTER" /D\
 "NOASSERT" /FR$(INTDIR)/ /Fp$(OUTDIR)/"icntrotl.pch" /Yu"outline.h"\
 /Fo$(INTDIR)/ /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

$(INTDIR)/outlname.obj :  $(SOURCE)  $(DEP_OUTLN) $(INTDIR)\
 $(INTDIR)/icntrotl.obj
   $(CPP) /nologo /Gt0 /QMOb2000 /W3 /GX /Zi /Od /I "..\include" /D "DBG" /D\
 "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "INC_OLE2" /D "OLE_CNTR" /D\
 "INPLACE_CNTR" /D "OLE2FINAL" /D "OLE2SHIP" /D "WIN32S" /D "OLE_VERSION" /D\
 "USE_MSGFILTER" /D "NOASSERT" /FR$(INTDIR)/ /Fp$(OUTDIR)/"icntrotl.pch"\
 /Yu"outline.h" /Fo$(INTDIR)/ /Fd$(OUTDIR)/"icntrotl.pdb" /c  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\icntrotl.rc
DEP_ICNTR=\
	.\selcross.cur\
	.\dragmove.cur\
	.\icntrotl.ico\
	.\image72.bmp\
	.\image96.bmp\
	.\image120.bmp\
	.\ole2.bmp\
	.\outlrc.h\
	.\cntrrc.h\
	.\debug.rc

!IF  "$(CFG)" == "Win32 (80x86) Release"

$(INTDIR)/icntrotl.res :  $(SOURCE)  $(DEP_ICNTR) $(INTDIR)
   $(RSC) $(RSC_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

$(INTDIR)/icntrotl.res :  $(SOURCE)  $(DEP_ICNTR) $(INTDIR)
   $(RSC) $(RSC_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

$(INTDIR)/icntrotl.res :  $(SOURCE)  $(DEP_ICNTR) $(INTDIR)
   $(RSC) $(RSC_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

$(INTDIR)/icntrotl.res :  $(SOURCE)  $(DEP_ICNTR) $(INTDIR)
   $(RSC) $(RSC_PROJ)  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\oledoc.c
DEP_OLEDO=\
	.\outline.h\
	.\outlrc.h\
	.\ansiapi.h\
	.\frametls.h\
	.\heading.h\
	.\oleoutl.h\
	..\include\bttncur.h\
	..\include\gizmobar.h\
	..\include\msgfiltr.h\
	.\defguid.h\
	.\svroutl.h\
	.\cntroutl.h\
	.\cntrrc.h

!IF  "$(CFG)" == "Win32 (80x86) Release"

# ADD CPP /Yu"outline.h"

$(INTDIR)/oledoc.obj :  $(SOURCE)  $(DEP_OLEDO) $(INTDIR)\
 $(INTDIR)/icntrotl.obj
   $(CPP) /nologo /W3 /GX /O2 /I "..\include" /D "NDEBUG" /D "WIN32" /D\
 "_WINDOWS" /D "_MBCS" /D "INC_OLE2" /D "OLE_CNTR" /D "INPLACE_CNTR" /D\
 "OLE2FINAL" /D "OLE2SHIP" /D "WIN32S" /D "OLE_VERSION" /D "USE_MSGFILTER" /D\
 "NOASSERT" /FR$(INTDIR)/ /Fp$(OUTDIR)/"icntrotl.pch" /Yu"outline.h"\
 /Fo$(INTDIR)/ /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

# ADD CPP /Yu"outline.h"

$(INTDIR)/oledoc.obj :  $(SOURCE)  $(DEP_OLEDO) $(INTDIR)\
 $(INTDIR)/icntrotl.obj
   $(CPP) /nologo /W3 /GX /Zi /Od /I "..\include" /D "DBG" /D "_DEBUG" /D\
 "WIN32" /D "_WINDOWS" /D "_MBCS" /D "INC_OLE2" /D "OLE_CNTR" /D "INPLACE_CNTR"\
 /D "OLE2FINAL" /D "OLE2SHIP" /D "WIN32S" /D "OLE_VERSION" /D "USE_MSGFILTER" /D\
 "NOASSERT" /FR$(INTDIR)/ /Fp$(OUTDIR)/"icntrotl.pch" /Yu"outline.h"\
 /Fo$(INTDIR)/ /Fd$(OUTDIR)/"icntrotl.pdb" /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

$(INTDIR)/oledoc.obj :  $(SOURCE)  $(DEP_OLEDO) $(INTDIR)\
 $(INTDIR)/icntrotl.obj
   $(CPP) /nologo /Gt0 /QMOb2000 /W3 /GX /O2 /I "..\include" /D "NDEBUG" /D\
 "WIN32" /D "_WINDOWS" /D "_MBCS" /D "INC_OLE2" /D "OLE_CNTR" /D "INPLACE_CNTR"\
 /D "OLE2FINAL" /D "OLE2SHIP" /D "WIN32S" /D "OLE_VERSION" /D "USE_MSGFILTER" /D\
 "NOASSERT" /FR$(INTDIR)/ /Fp$(OUTDIR)/"icntrotl.pch" /Yu"outline.h"\
 /Fo$(INTDIR)/ /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

$(INTDIR)/oledoc.obj :  $(SOURCE)  $(DEP_OLEDO) $(INTDIR)\
 $(INTDIR)/icntrotl.obj
   $(CPP) /nologo /Gt0 /QMOb2000 /W3 /GX /Zi /Od /I "..\include" /D "DBG" /D\
 "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "INC_OLE2" /D "OLE_CNTR" /D\
 "INPLACE_CNTR" /D "OLE2FINAL" /D "OLE2SHIP" /D "WIN32S" /D "OLE_VERSION" /D\
 "USE_MSGFILTER" /D "NOASSERT" /FR$(INTDIR)/ /Fp$(OUTDIR)/"icntrotl.pch"\
 /Yu"outline.h" /Fo$(INTDIR)/ /Fd$(OUTDIR)/"icntrotl.pdb" /c  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\outlntbl.c
DEP_OUTLNT=\
	.\outline.h\
	.\outlrc.h\
	.\ansiapi.h\
	.\frametls.h\
	.\heading.h\
	.\oleoutl.h\
	..\include\bttncur.h\
	..\include\gizmobar.h\
	..\include\msgfiltr.h\
	.\defguid.h\
	.\svroutl.h\
	.\cntroutl.h\
	.\cntrrc.h

!IF  "$(CFG)" == "Win32 (80x86) Release"

# ADD CPP /Yu"outline.h"

$(INTDIR)/outlntbl.obj :  $(SOURCE)  $(DEP_OUTLNT) $(INTDIR)\
 $(INTDIR)/icntrotl.obj
   $(CPP) /nologo /W3 /GX /O2 /I "..\include" /D "NDEBUG" /D "WIN32" /D\
 "_WINDOWS" /D "_MBCS" /D "INC_OLE2" /D "OLE_CNTR" /D "INPLACE_CNTR" /D\
 "OLE2FINAL" /D "OLE2SHIP" /D "WIN32S" /D "OLE_VERSION" /D "USE_MSGFILTER" /D\
 "NOASSERT" /FR$(INTDIR)/ /Fp$(OUTDIR)/"icntrotl.pch" /Yu"outline.h"\
 /Fo$(INTDIR)/ /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

# ADD CPP /Yu"outline.h"

$(INTDIR)/outlntbl.obj :  $(SOURCE)  $(DEP_OUTLNT) $(INTDIR)\
 $(INTDIR)/icntrotl.obj
   $(CPP) /nologo /W3 /GX /Zi /Od /I "..\include" /D "DBG" /D "_DEBUG" /D\
 "WIN32" /D "_WINDOWS" /D "_MBCS" /D "INC_OLE2" /D "OLE_CNTR" /D "INPLACE_CNTR"\
 /D "OLE2FINAL" /D "OLE2SHIP" /D "WIN32S" /D "OLE_VERSION" /D "USE_MSGFILTER" /D\
 "NOASSERT" /FR$(INTDIR)/ /Fp$(OUTDIR)/"icntrotl.pch" /Yu"outline.h"\
 /Fo$(INTDIR)/ /Fd$(OUTDIR)/"icntrotl.pdb" /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

$(INTDIR)/outlntbl.obj :  $(SOURCE)  $(DEP_OUTLNT) $(INTDIR)\
 $(INTDIR)/icntrotl.obj
   $(CPP) /nologo /Gt0 /QMOb2000 /W3 /GX /O2 /I "..\include" /D "NDEBUG" /D\
 "WIN32" /D "_WINDOWS" /D "_MBCS" /D "INC_OLE2" /D "OLE_CNTR" /D "INPLACE_CNTR"\
 /D "OLE2FINAL" /D "OLE2SHIP" /D "WIN32S" /D "OLE_VERSION" /D "USE_MSGFILTER" /D\
 "NOASSERT" /FR$(INTDIR)/ /Fp$(OUTDIR)/"icntrotl.pch" /Yu"outline.h"\
 /Fo$(INTDIR)/ /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

$(INTDIR)/outlntbl.obj :  $(SOURCE)  $(DEP_OUTLNT) $(INTDIR)\
 $(INTDIR)/icntrotl.obj
   $(CPP) /nologo /Gt0 /QMOb2000 /W3 /GX /Zi /Od /I "..\include" /D "DBG" /D\
 "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "INC_OLE2" /D "OLE_CNTR" /D\
 "INPLACE_CNTR" /D "OLE2FINAL" /D "OLE2SHIP" /D "WIN32S" /D "OLE_VERSION" /D\
 "USE_MSGFILTER" /D "NOASSERT" /FR$(INTDIR)/ /Fp$(OUTDIR)/"icntrotl.pch"\
 /Yu"outline.h" /Fo$(INTDIR)/ /Fd$(OUTDIR)/"icntrotl.pdb" /c  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\cntrline.c
DEP_CNTRL=\
	.\outline.h\
	.\outlrc.h\
	.\ansiapi.h\
	.\frametls.h\
	.\heading.h\
	.\oleoutl.h\
	..\include\bttncur.h\
	..\include\gizmobar.h\
	..\include\msgfiltr.h\
	.\defguid.h\
	.\svroutl.h\
	.\cntroutl.h\
	.\cntrrc.h

!IF  "$(CFG)" == "Win32 (80x86) Release"

# ADD CPP /Yu"outline.h"

$(INTDIR)/cntrline.obj :  $(SOURCE)  $(DEP_CNTRL) $(INTDIR)\
 $(INTDIR)/icntrotl.obj
   $(CPP) /nologo /W3 /GX /O2 /I "..\include" /D "NDEBUG" /D "WIN32" /D\
 "_WINDOWS" /D "_MBCS" /D "INC_OLE2" /D "OLE_CNTR" /D "INPLACE_CNTR" /D\
 "OLE2FINAL" /D "OLE2SHIP" /D "WIN32S" /D "OLE_VERSION" /D "USE_MSGFILTER" /D\
 "NOASSERT" /FR$(INTDIR)/ /Fp$(OUTDIR)/"icntrotl.pch" /Yu"outline.h"\
 /Fo$(INTDIR)/ /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

# ADD CPP /Yu"outline.h"

$(INTDIR)/cntrline.obj :  $(SOURCE)  $(DEP_CNTRL) $(INTDIR)\
 $(INTDIR)/icntrotl.obj
   $(CPP) /nologo /W3 /GX /Zi /Od /I "..\include" /D "DBG" /D "_DEBUG" /D\
 "WIN32" /D "_WINDOWS" /D "_MBCS" /D "INC_OLE2" /D "OLE_CNTR" /D "INPLACE_CNTR"\
 /D "OLE2FINAL" /D "OLE2SHIP" /D "WIN32S" /D "OLE_VERSION" /D "USE_MSGFILTER" /D\
 "NOASSERT" /FR$(INTDIR)/ /Fp$(OUTDIR)/"icntrotl.pch" /Yu"outline.h"\
 /Fo$(INTDIR)/ /Fd$(OUTDIR)/"icntrotl.pdb" /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

$(INTDIR)/cntrline.obj :  $(SOURCE)  $(DEP_CNTRL) $(INTDIR)\
 $(INTDIR)/icntrotl.obj
   $(CPP) /nologo /Gt0 /QMOb2000 /W3 /GX /O2 /I "..\include" /D "NDEBUG" /D\
 "WIN32" /D "_WINDOWS" /D "_MBCS" /D "INC_OLE2" /D "OLE_CNTR" /D "INPLACE_CNTR"\
 /D "OLE2FINAL" /D "OLE2SHIP" /D "WIN32S" /D "OLE_VERSION" /D "USE_MSGFILTER" /D\
 "NOASSERT" /FR$(INTDIR)/ /Fp$(OUTDIR)/"icntrotl.pch" /Yu"outline.h"\
 /Fo$(INTDIR)/ /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

$(INTDIR)/cntrline.obj :  $(SOURCE)  $(DEP_CNTRL) $(INTDIR)\
 $(INTDIR)/icntrotl.obj
   $(CPP) /nologo /Gt0 /QMOb2000 /W3 /GX /Zi /Od /I "..\include" /D "DBG" /D\
 "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "INC_OLE2" /D "OLE_CNTR" /D\
 "INPLACE_CNTR" /D "OLE2FINAL" /D "OLE2SHIP" /D "WIN32S" /D "OLE_VERSION" /D\
 "USE_MSGFILTER" /D "NOASSERT" /FR$(INTDIR)/ /Fp$(OUTDIR)/"icntrotl.pch"\
 /Yu"outline.h" /Fo$(INTDIR)/ /Fd$(OUTDIR)/"icntrotl.pdb" /c  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\outlline.c
DEP_OUTLLI=\
	.\outline.h\
	.\outlrc.h\
	.\ansiapi.h\
	.\frametls.h\
	.\heading.h\
	.\oleoutl.h\
	..\include\bttncur.h\
	..\include\gizmobar.h\
	..\include\msgfiltr.h\
	.\defguid.h\
	.\svroutl.h\
	.\cntroutl.h\
	.\cntrrc.h

!IF  "$(CFG)" == "Win32 (80x86) Release"

# ADD CPP /Yu"outline.h"

$(INTDIR)/outlline.obj :  $(SOURCE)  $(DEP_OUTLLI) $(INTDIR)\
 $(INTDIR)/icntrotl.obj
   $(CPP) /nologo /W3 /GX /O2 /I "..\include" /D "NDEBUG" /D "WIN32" /D\
 "_WINDOWS" /D "_MBCS" /D "INC_OLE2" /D "OLE_CNTR" /D "INPLACE_CNTR" /D\
 "OLE2FINAL" /D "OLE2SHIP" /D "WIN32S" /D "OLE_VERSION" /D "USE_MSGFILTER" /D\
 "NOASSERT" /FR$(INTDIR)/ /Fp$(OUTDIR)/"icntrotl.pch" /Yu"outline.h"\
 /Fo$(INTDIR)/ /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

# ADD CPP /Yu"outline.h"

$(INTDIR)/outlline.obj :  $(SOURCE)  $(DEP_OUTLLI) $(INTDIR)\
 $(INTDIR)/icntrotl.obj
   $(CPP) /nologo /W3 /GX /Zi /Od /I "..\include" /D "DBG" /D "_DEBUG" /D\
 "WIN32" /D "_WINDOWS" /D "_MBCS" /D "INC_OLE2" /D "OLE_CNTR" /D "INPLACE_CNTR"\
 /D "OLE2FINAL" /D "OLE2SHIP" /D "WIN32S" /D "OLE_VERSION" /D "USE_MSGFILTER" /D\
 "NOASSERT" /FR$(INTDIR)/ /Fp$(OUTDIR)/"icntrotl.pch" /Yu"outline.h"\
 /Fo$(INTDIR)/ /Fd$(OUTDIR)/"icntrotl.pdb" /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

$(INTDIR)/outlline.obj :  $(SOURCE)  $(DEP_OUTLLI) $(INTDIR)\
 $(INTDIR)/icntrotl.obj
   $(CPP) /nologo /Gt0 /QMOb2000 /W3 /GX /O2 /I "..\include" /D "NDEBUG" /D\
 "WIN32" /D "_WINDOWS" /D "_MBCS" /D "INC_OLE2" /D "OLE_CNTR" /D "INPLACE_CNTR"\
 /D "OLE2FINAL" /D "OLE2SHIP" /D "WIN32S" /D "OLE_VERSION" /D "USE_MSGFILTER" /D\
 "NOASSERT" /FR$(INTDIR)/ /Fp$(OUTDIR)/"icntrotl.pch" /Yu"outline.h"\
 /Fo$(INTDIR)/ /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

$(INTDIR)/outlline.obj :  $(SOURCE)  $(DEP_OUTLLI) $(INTDIR)\
 $(INTDIR)/icntrotl.obj
   $(CPP) /nologo /Gt0 /QMOb2000 /W3 /GX /Zi /Od /I "..\include" /D "DBG" /D\
 "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "INC_OLE2" /D "OLE_CNTR" /D\
 "INPLACE_CNTR" /D "OLE2FINAL" /D "OLE2SHIP" /D "WIN32S" /D "OLE_VERSION" /D\
 "USE_MSGFILTER" /D "NOASSERT" /FR$(INTDIR)/ /Fp$(OUTDIR)/"icntrotl.pch"\
 /Yu"outline.h" /Fo$(INTDIR)/ /Fd$(OUTDIR)/"icntrotl.pdb" /c  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\classfac.c
DEP_CLASS=\
	.\outline.h\
	.\outlrc.h\
	.\ansiapi.h\
	.\frametls.h\
	.\heading.h\
	.\oleoutl.h\
	..\include\bttncur.h\
	..\include\gizmobar.h\
	..\include\msgfiltr.h\
	.\defguid.h\
	.\svroutl.h\
	.\cntroutl.h\
	.\cntrrc.h

!IF  "$(CFG)" == "Win32 (80x86) Release"

# ADD CPP /Yu"outline.h"

$(INTDIR)/classfac.obj :  $(SOURCE)  $(DEP_CLASS) $(INTDIR)\
 $(INTDIR)/icntrotl.obj
   $(CPP) /nologo /W3 /GX /O2 /I "..\include" /D "NDEBUG" /D "WIN32" /D\
 "_WINDOWS" /D "_MBCS" /D "INC_OLE2" /D "OLE_CNTR" /D "INPLACE_CNTR" /D\
 "OLE2FINAL" /D "OLE2SHIP" /D "WIN32S" /D "OLE_VERSION" /D "USE_MSGFILTER" /D\
 "NOASSERT" /FR$(INTDIR)/ /Fp$(OUTDIR)/"icntrotl.pch" /Yu"outline.h"\
 /Fo$(INTDIR)/ /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

# ADD CPP /Yu"outline.h"

$(INTDIR)/classfac.obj :  $(SOURCE)  $(DEP_CLASS) $(INTDIR)\
 $(INTDIR)/icntrotl.obj
   $(CPP) /nologo /W3 /GX /Zi /Od /I "..\include" /D "DBG" /D "_DEBUG" /D\
 "WIN32" /D "_WINDOWS" /D "_MBCS" /D "INC_OLE2" /D "OLE_CNTR" /D "INPLACE_CNTR"\
 /D "OLE2FINAL" /D "OLE2SHIP" /D "WIN32S" /D "OLE_VERSION" /D "USE_MSGFILTER" /D\
 "NOASSERT" /FR$(INTDIR)/ /Fp$(OUTDIR)/"icntrotl.pch" /Yu"outline.h"\
 /Fo$(INTDIR)/ /Fd$(OUTDIR)/"icntrotl.pdb" /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

$(INTDIR)/classfac.obj :  $(SOURCE)  $(DEP_CLASS) $(INTDIR)\
 $(INTDIR)/icntrotl.obj
   $(CPP) /nologo /Gt0 /QMOb2000 /W3 /GX /O2 /I "..\include" /D "NDEBUG" /D\
 "WIN32" /D "_WINDOWS" /D "_MBCS" /D "INC_OLE2" /D "OLE_CNTR" /D "INPLACE_CNTR"\
 /D "OLE2FINAL" /D "OLE2SHIP" /D "WIN32S" /D "OLE_VERSION" /D "USE_MSGFILTER" /D\
 "NOASSERT" /FR$(INTDIR)/ /Fp$(OUTDIR)/"icntrotl.pch" /Yu"outline.h"\
 /Fo$(INTDIR)/ /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

$(INTDIR)/classfac.obj :  $(SOURCE)  $(DEP_CLASS) $(INTDIR)\
 $(INTDIR)/icntrotl.obj
   $(CPP) /nologo /Gt0 /QMOb2000 /W3 /GX /Zi /Od /I "..\include" /D "DBG" /D\
 "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "INC_OLE2" /D "OLE_CNTR" /D\
 "INPLACE_CNTR" /D "OLE2FINAL" /D "OLE2SHIP" /D "WIN32S" /D "OLE_VERSION" /D\
 "USE_MSGFILTER" /D "NOASSERT" /FR$(INTDIR)/ /Fp$(OUTDIR)/"icntrotl.pch"\
 /Yu"outline.h" /Fo$(INTDIR)/ /Fd$(OUTDIR)/"icntrotl.pdb" /c  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\outldoc.c
DEP_OUTLD=\
	.\outline.h\
	.\outlrc.h\
	.\ansiapi.h\
	.\frametls.h\
	.\heading.h\
	.\oleoutl.h\
	..\include\bttncur.h\
	..\include\gizmobar.h\
	..\include\msgfiltr.h\
	.\defguid.h\
	.\svroutl.h\
	.\cntroutl.h\
	.\cntrrc.h

!IF  "$(CFG)" == "Win32 (80x86) Release"

# ADD CPP /Yu"outline.h"

$(INTDIR)/outldoc.obj :  $(SOURCE)  $(DEP_OUTLD) $(INTDIR)\
 $(INTDIR)/icntrotl.obj
   $(CPP) /nologo /W3 /GX /O2 /I "..\include" /D "NDEBUG" /D "WIN32" /D\
 "_WINDOWS" /D "_MBCS" /D "INC_OLE2" /D "OLE_CNTR" /D "INPLACE_CNTR" /D\
 "OLE2FINAL" /D "OLE2SHIP" /D "WIN32S" /D "OLE_VERSION" /D "USE_MSGFILTER" /D\
 "NOASSERT" /FR$(INTDIR)/ /Fp$(OUTDIR)/"icntrotl.pch" /Yu"outline.h"\
 /Fo$(INTDIR)/ /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

# ADD CPP /Yu"outline.h"

$(INTDIR)/outldoc.obj :  $(SOURCE)  $(DEP_OUTLD) $(INTDIR)\
 $(INTDIR)/icntrotl.obj
   $(CPP) /nologo /W3 /GX /Zi /Od /I "..\include" /D "DBG" /D "_DEBUG" /D\
 "WIN32" /D "_WINDOWS" /D "_MBCS" /D "INC_OLE2" /D "OLE_CNTR" /D "INPLACE_CNTR"\
 /D "OLE2FINAL" /D "OLE2SHIP" /D "WIN32S" /D "OLE_VERSION" /D "USE_MSGFILTER" /D\
 "NOASSERT" /FR$(INTDIR)/ /Fp$(OUTDIR)/"icntrotl.pch" /Yu"outline.h"\
 /Fo$(INTDIR)/ /Fd$(OUTDIR)/"icntrotl.pdb" /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

$(INTDIR)/outldoc.obj :  $(SOURCE)  $(DEP_OUTLD) $(INTDIR)\
 $(INTDIR)/icntrotl.obj
   $(CPP) /nologo /Gt0 /QMOb2000 /W3 /GX /O2 /I "..\include" /D "NDEBUG" /D\
 "WIN32" /D "_WINDOWS" /D "_MBCS" /D "INC_OLE2" /D "OLE_CNTR" /D "INPLACE_CNTR"\
 /D "OLE2FINAL" /D "OLE2SHIP" /D "WIN32S" /D "OLE_VERSION" /D "USE_MSGFILTER" /D\
 "NOASSERT" /FR$(INTDIR)/ /Fp$(OUTDIR)/"icntrotl.pch" /Yu"outline.h"\
 /Fo$(INTDIR)/ /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

$(INTDIR)/outldoc.obj :  $(SOURCE)  $(DEP_OUTLD) $(INTDIR)\
 $(INTDIR)/icntrotl.obj
   $(CPP) /nologo /Gt0 /QMOb2000 /W3 /GX /Zi /Od /I "..\include" /D "DBG" /D\
 "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "INC_OLE2" /D "OLE_CNTR" /D\
 "INPLACE_CNTR" /D "OLE2FINAL" /D "OLE2SHIP" /D "WIN32S" /D "OLE_VERSION" /D\
 "USE_MSGFILTER" /D "NOASSERT" /FR$(INTDIR)/ /Fp$(OUTDIR)/"icntrotl.pch"\
 /Yu"outline.h" /Fo$(INTDIR)/ /Fd$(OUTDIR)/"icntrotl.pdb" /c  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\debug2.c
DEP_DEBUG=\
	.\outline.h\
	.\outlrc.h\
	.\ansiapi.h\
	.\frametls.h\
	.\heading.h\
	.\oleoutl.h\
	..\include\bttncur.h\
	..\include\gizmobar.h\
	..\include\msgfiltr.h\
	.\defguid.h\
	.\svroutl.h\
	.\cntroutl.h\
	.\cntrrc.h

!IF  "$(CFG)" == "Win32 (80x86) Release"

# ADD CPP /Yu"outline.h"

$(INTDIR)/debug2.obj :  $(SOURCE)  $(DEP_DEBUG) $(INTDIR)\
 $(INTDIR)/icntrotl.obj
   $(CPP) /nologo /W3 /GX /O2 /I "..\include" /D "NDEBUG" /D "WIN32" /D\
 "_WINDOWS" /D "_MBCS" /D "INC_OLE2" /D "OLE_CNTR" /D "INPLACE_CNTR" /D\
 "OLE2FINAL" /D "OLE2SHIP" /D "WIN32S" /D "OLE_VERSION" /D "USE_MSGFILTER" /D\
 "NOASSERT" /FR$(INTDIR)/ /Fp$(OUTDIR)/"icntrotl.pch" /Yu"outline.h"\
 /Fo$(INTDIR)/ /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

# ADD CPP /Yu"outline.h"

$(INTDIR)/debug2.obj :  $(SOURCE)  $(DEP_DEBUG) $(INTDIR)\
 $(INTDIR)/icntrotl.obj
   $(CPP) /nologo /W3 /GX /Zi /Od /I "..\include" /D "DBG" /D "_DEBUG" /D\
 "WIN32" /D "_WINDOWS" /D "_MBCS" /D "INC_OLE2" /D "OLE_CNTR" /D "INPLACE_CNTR"\
 /D "OLE2FINAL" /D "OLE2SHIP" /D "WIN32S" /D "OLE_VERSION" /D "USE_MSGFILTER" /D\
 "NOASSERT" /FR$(INTDIR)/ /Fp$(OUTDIR)/"icntrotl.pch" /Yu"outline.h"\
 /Fo$(INTDIR)/ /Fd$(OUTDIR)/"icntrotl.pdb" /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

$(INTDIR)/debug2.obj :  $(SOURCE)  $(DEP_DEBUG) $(INTDIR)\
 $(INTDIR)/icntrotl.obj
   $(CPP) /nologo /Gt0 /QMOb2000 /W3 /GX /O2 /I "..\include" /D "NDEBUG" /D\
 "WIN32" /D "_WINDOWS" /D "_MBCS" /D "INC_OLE2" /D "OLE_CNTR" /D "INPLACE_CNTR"\
 /D "OLE2FINAL" /D "OLE2SHIP" /D "WIN32S" /D "OLE_VERSION" /D "USE_MSGFILTER" /D\
 "NOASSERT" /FR$(INTDIR)/ /Fp$(OUTDIR)/"icntrotl.pch" /Yu"outline.h"\
 /Fo$(INTDIR)/ /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

$(INTDIR)/debug2.obj :  $(SOURCE)  $(DEP_DEBUG) $(INTDIR)\
 $(INTDIR)/icntrotl.obj
   $(CPP) /nologo /Gt0 /QMOb2000 /W3 /GX /Zi /Od /I "..\include" /D "DBG" /D\
 "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "INC_OLE2" /D "OLE_CNTR" /D\
 "INPLACE_CNTR" /D "OLE2FINAL" /D "OLE2SHIP" /D "WIN32S" /D "OLE_VERSION" /D\
 "USE_MSGFILTER" /D "NOASSERT" /FR$(INTDIR)/ /Fp$(OUTDIR)/"icntrotl.pch"\
 /Yu"outline.h" /Fo$(INTDIR)/ /Fd$(OUTDIR)/"icntrotl.pdb" /c  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\memmgr.c
DEP_MEMMG=\
	.\outline.h\
	.\outlrc.h\
	.\ansiapi.h\
	.\frametls.h\
	.\heading.h\
	.\oleoutl.h\
	..\include\bttncur.h\
	..\include\gizmobar.h\
	..\include\msgfiltr.h\
	.\defguid.h\
	.\svroutl.h\
	.\cntroutl.h\
	.\cntrrc.h

!IF  "$(CFG)" == "Win32 (80x86) Release"

# ADD CPP /Yu"outline.h"

$(INTDIR)/memmgr.obj :  $(SOURCE)  $(DEP_MEMMG) $(INTDIR)\
 $(INTDIR)/icntrotl.obj
   $(CPP) /nologo /W3 /GX /O2 /I "..\include" /D "NDEBUG" /D "WIN32" /D\
 "_WINDOWS" /D "_MBCS" /D "INC_OLE2" /D "OLE_CNTR" /D "INPLACE_CNTR" /D\
 "OLE2FINAL" /D "OLE2SHIP" /D "WIN32S" /D "OLE_VERSION" /D "USE_MSGFILTER" /D\
 "NOASSERT" /FR$(INTDIR)/ /Fp$(OUTDIR)/"icntrotl.pch" /Yu"outline.h"\
 /Fo$(INTDIR)/ /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

# ADD CPP /Yu"outline.h"

$(INTDIR)/memmgr.obj :  $(SOURCE)  $(DEP_MEMMG) $(INTDIR)\
 $(INTDIR)/icntrotl.obj
   $(CPP) /nologo /W3 /GX /Zi /Od /I "..\include" /D "DBG" /D "_DEBUG" /D\
 "WIN32" /D "_WINDOWS" /D "_MBCS" /D "INC_OLE2" /D "OLE_CNTR" /D "INPLACE_CNTR"\
 /D "OLE2FINAL" /D "OLE2SHIP" /D "WIN32S" /D "OLE_VERSION" /D "USE_MSGFILTER" /D\
 "NOASSERT" /FR$(INTDIR)/ /Fp$(OUTDIR)/"icntrotl.pch" /Yu"outline.h"\
 /Fo$(INTDIR)/ /Fd$(OUTDIR)/"icntrotl.pdb" /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

$(INTDIR)/memmgr.obj :  $(SOURCE)  $(DEP_MEMMG) $(INTDIR)\
 $(INTDIR)/icntrotl.obj
   $(CPP) /nologo /Gt0 /QMOb2000 /W3 /GX /O2 /I "..\include" /D "NDEBUG" /D\
 "WIN32" /D "_WINDOWS" /D "_MBCS" /D "INC_OLE2" /D "OLE_CNTR" /D "INPLACE_CNTR"\
 /D "OLE2FINAL" /D "OLE2SHIP" /D "WIN32S" /D "OLE_VERSION" /D "USE_MSGFILTER" /D\
 "NOASSERT" /FR$(INTDIR)/ /Fp$(OUTDIR)/"icntrotl.pch" /Yu"outline.h"\
 /Fo$(INTDIR)/ /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

$(INTDIR)/memmgr.obj :  $(SOURCE)  $(DEP_MEMMG) $(INTDIR)\
 $(INTDIR)/icntrotl.obj
   $(CPP) /nologo /Gt0 /QMOb2000 /W3 /GX /Zi /Od /I "..\include" /D "DBG" /D\
 "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "INC_OLE2" /D "OLE_CNTR" /D\
 "INPLACE_CNTR" /D "OLE2FINAL" /D "OLE2SHIP" /D "WIN32S" /D "OLE_VERSION" /D\
 "USE_MSGFILTER" /D "NOASSERT" /FR$(INTDIR)/ /Fp$(OUTDIR)/"icntrotl.pch"\
 /Yu"outline.h" /Fo$(INTDIR)/ /Fd$(OUTDIR)/"icntrotl.pdb" /c  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\dragdrop.c
DEP_DRAGD=\
	.\outline.h\
	.\outlrc.h\
	.\ansiapi.h\
	.\frametls.h\
	.\heading.h\
	.\oleoutl.h\
	..\include\bttncur.h\
	..\include\gizmobar.h\
	..\include\msgfiltr.h\
	.\defguid.h\
	.\svroutl.h\
	.\cntroutl.h\
	.\cntrrc.h

!IF  "$(CFG)" == "Win32 (80x86) Release"

# ADD CPP /Yu"outline.h"

$(INTDIR)/dragdrop.obj :  $(SOURCE)  $(DEP_DRAGD) $(INTDIR)\
 $(INTDIR)/icntrotl.obj
   $(CPP) /nologo /W3 /GX /O2 /I "..\include" /D "NDEBUG" /D "WIN32" /D\
 "_WINDOWS" /D "_MBCS" /D "INC_OLE2" /D "OLE_CNTR" /D "INPLACE_CNTR" /D\
 "OLE2FINAL" /D "OLE2SHIP" /D "WIN32S" /D "OLE_VERSION" /D "USE_MSGFILTER" /D\
 "NOASSERT" /FR$(INTDIR)/ /Fp$(OUTDIR)/"icntrotl.pch" /Yu"outline.h"\
 /Fo$(INTDIR)/ /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

# ADD CPP /Yu"outline.h"

$(INTDIR)/dragdrop.obj :  $(SOURCE)  $(DEP_DRAGD) $(INTDIR)\
 $(INTDIR)/icntrotl.obj
   $(CPP) /nologo /W3 /GX /Zi /Od /I "..\include" /D "DBG" /D "_DEBUG" /D\
 "WIN32" /D "_WINDOWS" /D "_MBCS" /D "INC_OLE2" /D "OLE_CNTR" /D "INPLACE_CNTR"\
 /D "OLE2FINAL" /D "OLE2SHIP" /D "WIN32S" /D "OLE_VERSION" /D "USE_MSGFILTER" /D\
 "NOASSERT" /FR$(INTDIR)/ /Fp$(OUTDIR)/"icntrotl.pch" /Yu"outline.h"\
 /Fo$(INTDIR)/ /Fd$(OUTDIR)/"icntrotl.pdb" /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

$(INTDIR)/dragdrop.obj :  $(SOURCE)  $(DEP_DRAGD) $(INTDIR)\
 $(INTDIR)/icntrotl.obj
   $(CPP) /nologo /Gt0 /QMOb2000 /W3 /GX /O2 /I "..\include" /D "NDEBUG" /D\
 "WIN32" /D "_WINDOWS" /D "_MBCS" /D "INC_OLE2" /D "OLE_CNTR" /D "INPLACE_CNTR"\
 /D "OLE2FINAL" /D "OLE2SHIP" /D "WIN32S" /D "OLE_VERSION" /D "USE_MSGFILTER" /D\
 "NOASSERT" /FR$(INTDIR)/ /Fp$(OUTDIR)/"icntrotl.pch" /Yu"outline.h"\
 /Fo$(INTDIR)/ /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

$(INTDIR)/dragdrop.obj :  $(SOURCE)  $(DEP_DRAGD) $(INTDIR)\
 $(INTDIR)/icntrotl.obj
   $(CPP) /nologo /Gt0 /QMOb2000 /W3 /GX /Zi /Od /I "..\include" /D "DBG" /D\
 "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "INC_OLE2" /D "OLE_CNTR" /D\
 "INPLACE_CNTR" /D "OLE2FINAL" /D "OLE2SHIP" /D "WIN32S" /D "OLE_VERSION" /D\
 "USE_MSGFILTER" /D "NOASSERT" /FR$(INTDIR)/ /Fp$(OUTDIR)/"icntrotl.pch"\
 /Yu"outline.h" /Fo$(INTDIR)/ /Fd$(OUTDIR)/"icntrotl.pdb" /c  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\icntrotl.c
DEP_ICNTRO=\
	.\outline.h\
	.\outlrc.h\
	.\ansiapi.h\
	.\frametls.h\
	.\heading.h\
	.\oleoutl.h\
	..\include\bttncur.h\
	..\include\gizmobar.h\
	..\include\msgfiltr.h\
	.\defguid.h\
	.\svroutl.h\
	.\cntroutl.h\
	.\cntrrc.h

!IF  "$(CFG)" == "Win32 (80x86) Release"

# ADD CPP /Yc"outline.h"

$(INTDIR)/icntrotl.obj :  $(SOURCE)  $(DEP_ICNTRO) $(INTDIR)
   $(CPP) /nologo /W3 /GX /O2 /I "..\include" /D "NDEBUG" /D "WIN32" /D\
 "_WINDOWS" /D "_MBCS" /D "INC_OLE2" /D "OLE_CNTR" /D "INPLACE_CNTR" /D\
 "OLE2FINAL" /D "OLE2SHIP" /D "WIN32S" /D "OLE_VERSION" /D "USE_MSGFILTER" /D\
 "NOASSERT" /FR$(INTDIR)/ /Fp$(OUTDIR)/"icntrotl.pch" /Yc"outline.h"\
 /Fo$(INTDIR)/ /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

# ADD CPP /Yc"outline.h"

$(INTDIR)/icntrotl.obj :  $(SOURCE)  $(DEP_ICNTRO) $(INTDIR)
   $(CPP) /nologo /W3 /GX /Zi /Od /I "..\include" /D "DBG" /D "_DEBUG" /D\
 "WIN32" /D "_WINDOWS" /D "_MBCS" /D "INC_OLE2" /D "OLE_CNTR" /D "INPLACE_CNTR"\
 /D "OLE2FINAL" /D "OLE2SHIP" /D "WIN32S" /D "OLE_VERSION" /D "USE_MSGFILTER" /D\
 "NOASSERT" /FR$(INTDIR)/ /Fp$(OUTDIR)/"icntrotl.pch" /Yc"outline.h"\
 /Fo$(INTDIR)/ /Fd$(OUTDIR)/"icntrotl.pdb" /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

# ADD BASE CPP /Gt0 /QMOb2000
# ADD CPP /Gt0 /QMOb2000 /Yc"outline.h"

$(INTDIR)/icntrotl.obj :  $(SOURCE)  $(DEP_ICNTRO) $(INTDIR)
   $(CPP) /nologo /Gt0 /QMOb2000 /W3 /GX /O2 /I "..\include" /D "NDEBUG" /D\
 "WIN32" /D "_WINDOWS" /D "_MBCS" /D "INC_OLE2" /D "OLE_CNTR" /D "INPLACE_CNTR"\
 /D "OLE2FINAL" /D "OLE2SHIP" /D "WIN32S" /D "OLE_VERSION" /D "USE_MSGFILTER" /D\
 "NOASSERT" /FR$(INTDIR)/ /Fp$(OUTDIR)/"icntrotl.pch" /Yc"outline.h"\
 /Fo$(INTDIR)/ /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

# ADD BASE CPP /Gt0 /QMOb2000
# ADD CPP /Gt0 /QMOb2000 /Yc"outline.h"

$(INTDIR)/icntrotl.obj :  $(SOURCE)  $(DEP_ICNTRO) $(INTDIR)
   $(CPP) /nologo /Gt0 /QMOb2000 /W3 /GX /Zi /Od /I "..\include" /D "DBG" /D\
 "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "INC_OLE2" /D "OLE_CNTR" /D\
 "INPLACE_CNTR" /D "OLE2FINAL" /D "OLE2SHIP" /D "WIN32S" /D "OLE_VERSION" /D\
 "USE_MSGFILTER" /D "NOASSERT" /FR$(INTDIR)/ /Fp$(OUTDIR)/"icntrotl.pch"\
 /Yc"outline.h" /Fo$(INTDIR)/ /Fd$(OUTDIR)/"icntrotl.pdb" /c  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\clipbrd.c
DEP_CLIPB=\
	.\outline.h\
	.\outlrc.h\
	.\ansiapi.h\
	.\frametls.h\
	.\heading.h\
	.\oleoutl.h\
	..\include\bttncur.h\
	..\include\gizmobar.h\
	..\include\msgfiltr.h\
	.\defguid.h\
	.\svroutl.h\
	.\cntroutl.h\
	.\cntrrc.h

!IF  "$(CFG)" == "Win32 (80x86) Release"

# ADD CPP /Yu"outline.h"

$(INTDIR)/clipbrd.obj :  $(SOURCE)  $(DEP_CLIPB) $(INTDIR)\
 $(INTDIR)/icntrotl.obj
   $(CPP) /nologo /W3 /GX /O2 /I "..\include" /D "NDEBUG" /D "WIN32" /D\
 "_WINDOWS" /D "_MBCS" /D "INC_OLE2" /D "OLE_CNTR" /D "INPLACE_CNTR" /D\
 "OLE2FINAL" /D "OLE2SHIP" /D "WIN32S" /D "OLE_VERSION" /D "USE_MSGFILTER" /D\
 "NOASSERT" /FR$(INTDIR)/ /Fp$(OUTDIR)/"icntrotl.pch" /Yu"outline.h"\
 /Fo$(INTDIR)/ /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

# ADD CPP /Yu"outline.h"

$(INTDIR)/clipbrd.obj :  $(SOURCE)  $(DEP_CLIPB) $(INTDIR)\
 $(INTDIR)/icntrotl.obj
   $(CPP) /nologo /W3 /GX /Zi /Od /I "..\include" /D "DBG" /D "_DEBUG" /D\
 "WIN32" /D "_WINDOWS" /D "_MBCS" /D "INC_OLE2" /D "OLE_CNTR" /D "INPLACE_CNTR"\
 /D "OLE2FINAL" /D "OLE2SHIP" /D "WIN32S" /D "OLE_VERSION" /D "USE_MSGFILTER" /D\
 "NOASSERT" /FR$(INTDIR)/ /Fp$(OUTDIR)/"icntrotl.pch" /Yu"outline.h"\
 /Fo$(INTDIR)/ /Fd$(OUTDIR)/"icntrotl.pdb" /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

$(INTDIR)/clipbrd.obj :  $(SOURCE)  $(DEP_CLIPB) $(INTDIR)\
 $(INTDIR)/icntrotl.obj
   $(CPP) /nologo /Gt0 /QMOb2000 /W3 /GX /O2 /I "..\include" /D "NDEBUG" /D\
 "WIN32" /D "_WINDOWS" /D "_MBCS" /D "INC_OLE2" /D "OLE_CNTR" /D "INPLACE_CNTR"\
 /D "OLE2FINAL" /D "OLE2SHIP" /D "WIN32S" /D "OLE_VERSION" /D "USE_MSGFILTER" /D\
 "NOASSERT" /FR$(INTDIR)/ /Fp$(OUTDIR)/"icntrotl.pch" /Yu"outline.h"\
 /Fo$(INTDIR)/ /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

$(INTDIR)/clipbrd.obj :  $(SOURCE)  $(DEP_CLIPB) $(INTDIR)\
 $(INTDIR)/icntrotl.obj
   $(CPP) /nologo /Gt0 /QMOb2000 /W3 /GX /Zi /Od /I "..\include" /D "DBG" /D\
 "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "INC_OLE2" /D "OLE_CNTR" /D\
 "INPLACE_CNTR" /D "OLE2FINAL" /D "OLE2SHIP" /D "WIN32S" /D "OLE_VERSION" /D\
 "USE_MSGFILTER" /D "NOASSERT" /FR$(INTDIR)/ /Fp$(OUTDIR)/"icntrotl.pch"\
 /Yu"outline.h" /Fo$(INTDIR)/ /Fd$(OUTDIR)/"icntrotl.pdb" /c  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\oleapp.c
DEP_OLEAP=\
	.\outline.h\
	.\outlrc.h\
	.\ansiapi.h\
	.\frametls.h\
	.\heading.h\
	.\oleoutl.h\
	..\include\bttncur.h\
	..\include\gizmobar.h\
	..\include\msgfiltr.h\
	.\defguid.h\
	.\svroutl.h\
	.\cntroutl.h\
	.\cntrrc.h

!IF  "$(CFG)" == "Win32 (80x86) Release"

# ADD CPP /Yu"outline.h"

$(INTDIR)/oleapp.obj :  $(SOURCE)  $(DEP_OLEAP) $(INTDIR)\
 $(INTDIR)/icntrotl.obj
   $(CPP) /nologo /W3 /GX /O2 /I "..\include" /D "NDEBUG" /D "WIN32" /D\
 "_WINDOWS" /D "_MBCS" /D "INC_OLE2" /D "OLE_CNTR" /D "INPLACE_CNTR" /D\
 "OLE2FINAL" /D "OLE2SHIP" /D "WIN32S" /D "OLE_VERSION" /D "USE_MSGFILTER" /D\
 "NOASSERT" /FR$(INTDIR)/ /Fp$(OUTDIR)/"icntrotl.pch" /Yu"outline.h"\
 /Fo$(INTDIR)/ /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

# ADD CPP /Yu"outline.h"

$(INTDIR)/oleapp.obj :  $(SOURCE)  $(DEP_OLEAP) $(INTDIR)\
 $(INTDIR)/icntrotl.obj
   $(CPP) /nologo /W3 /GX /Zi /Od /I "..\include" /D "DBG" /D "_DEBUG" /D\
 "WIN32" /D "_WINDOWS" /D "_MBCS" /D "INC_OLE2" /D "OLE_CNTR" /D "INPLACE_CNTR"\
 /D "OLE2FINAL" /D "OLE2SHIP" /D "WIN32S" /D "OLE_VERSION" /D "USE_MSGFILTER" /D\
 "NOASSERT" /FR$(INTDIR)/ /Fp$(OUTDIR)/"icntrotl.pch" /Yu"outline.h"\
 /Fo$(INTDIR)/ /Fd$(OUTDIR)/"icntrotl.pdb" /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

$(INTDIR)/oleapp.obj :  $(SOURCE)  $(DEP_OLEAP) $(INTDIR)\
 $(INTDIR)/icntrotl.obj
   $(CPP) /nologo /Gt0 /QMOb2000 /W3 /GX /O2 /I "..\include" /D "NDEBUG" /D\
 "WIN32" /D "_WINDOWS" /D "_MBCS" /D "INC_OLE2" /D "OLE_CNTR" /D "INPLACE_CNTR"\
 /D "OLE2FINAL" /D "OLE2SHIP" /D "WIN32S" /D "OLE_VERSION" /D "USE_MSGFILTER" /D\
 "NOASSERT" /FR$(INTDIR)/ /Fp$(OUTDIR)/"icntrotl.pch" /Yu"outline.h"\
 /Fo$(INTDIR)/ /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

$(INTDIR)/oleapp.obj :  $(SOURCE)  $(DEP_OLEAP) $(INTDIR)\
 $(INTDIR)/icntrotl.obj
   $(CPP) /nologo /Gt0 /QMOb2000 /W3 /GX /Zi /Od /I "..\include" /D "DBG" /D\
 "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "INC_OLE2" /D "OLE_CNTR" /D\
 "INPLACE_CNTR" /D "OLE2FINAL" /D "OLE2SHIP" /D "WIN32S" /D "OLE_VERSION" /D\
 "USE_MSGFILTER" /D "NOASSERT" /FR$(INTDIR)/ /Fp$(OUTDIR)/"icntrotl.pch"\
 /Yu"outline.h" /Fo$(INTDIR)/ /Fd$(OUTDIR)/"icntrotl.pdb" /c  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\debug.c
DEP_DEBUG_=\
	.\outline.h\
	.\outlrc.h\
	.\ansiapi.h\
	.\frametls.h\
	.\heading.h\
	.\oleoutl.h\
	..\include\bttncur.h\
	..\include\gizmobar.h\
	..\include\msgfiltr.h\
	.\defguid.h\
	.\svroutl.h\
	.\cntroutl.h\
	.\cntrrc.h

!IF  "$(CFG)" == "Win32 (80x86) Release"

# ADD CPP /Yu"outline.h"

$(INTDIR)/debug.obj :  $(SOURCE)  $(DEP_DEBUG_) $(INTDIR)\
 $(INTDIR)/icntrotl.obj
   $(CPP) /nologo /W3 /GX /O2 /I "..\include" /D "NDEBUG" /D "WIN32" /D\
 "_WINDOWS" /D "_MBCS" /D "INC_OLE2" /D "OLE_CNTR" /D "INPLACE_CNTR" /D\
 "OLE2FINAL" /D "OLE2SHIP" /D "WIN32S" /D "OLE_VERSION" /D "USE_MSGFILTER" /D\
 "NOASSERT" /FR$(INTDIR)/ /Fp$(OUTDIR)/"icntrotl.pch" /Yu"outline.h"\
 /Fo$(INTDIR)/ /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

# ADD CPP /Yu"outline.h"

$(INTDIR)/debug.obj :  $(SOURCE)  $(DEP_DEBUG_) $(INTDIR)\
 $(INTDIR)/icntrotl.obj
   $(CPP) /nologo /W3 /GX /Zi /Od /I "..\include" /D "DBG" /D "_DEBUG" /D\
 "WIN32" /D "_WINDOWS" /D "_MBCS" /D "INC_OLE2" /D "OLE_CNTR" /D "INPLACE_CNTR"\
 /D "OLE2FINAL" /D "OLE2SHIP" /D "WIN32S" /D "OLE_VERSION" /D "USE_MSGFILTER" /D\
 "NOASSERT" /FR$(INTDIR)/ /Fp$(OUTDIR)/"icntrotl.pch" /Yu"outline.h"\
 /Fo$(INTDIR)/ /Fd$(OUTDIR)/"icntrotl.pdb" /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

$(INTDIR)/debug.obj :  $(SOURCE)  $(DEP_DEBUG_) $(INTDIR)\
 $(INTDIR)/icntrotl.obj
   $(CPP) /nologo /Gt0 /QMOb2000 /W3 /GX /O2 /I "..\include" /D "NDEBUG" /D\
 "WIN32" /D "_WINDOWS" /D "_MBCS" /D "INC_OLE2" /D "OLE_CNTR" /D "INPLACE_CNTR"\
 /D "OLE2FINAL" /D "OLE2SHIP" /D "WIN32S" /D "OLE_VERSION" /D "USE_MSGFILTER" /D\
 "NOASSERT" /FR$(INTDIR)/ /Fp$(OUTDIR)/"icntrotl.pch" /Yu"outline.h"\
 /Fo$(INTDIR)/ /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

$(INTDIR)/debug.obj :  $(SOURCE)  $(DEP_DEBUG_) $(INTDIR)\
 $(INTDIR)/icntrotl.obj
   $(CPP) /nologo /Gt0 /QMOb2000 /W3 /GX /Zi /Od /I "..\include" /D "DBG" /D\
 "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "INC_OLE2" /D "OLE_CNTR" /D\
 "INPLACE_CNTR" /D "OLE2FINAL" /D "OLE2SHIP" /D "WIN32S" /D "OLE_VERSION" /D\
 "USE_MSGFILTER" /D "NOASSERT" /FR$(INTDIR)/ /Fp$(OUTDIR)/"icntrotl.pch"\
 /Yu"outline.h" /Fo$(INTDIR)/ /Fd$(OUTDIR)/"icntrotl.pdb" /c  $(SOURCE) 

!ENDIF 

# End Source File
# End Group
# End Project
################################################################################
