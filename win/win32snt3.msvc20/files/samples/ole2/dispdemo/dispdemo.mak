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
!MESSAGE NMAKE /f "dispdemo.mak" CFG="Win32 (80x86) Debug"
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

ALL : $(OUTDIR)/dispdemo.exe $(OUTDIR)/dispdemo.bsc

$(OUTDIR) : 
    if not exist $(OUTDIR)/nul mkdir $(OUTDIR)

MTL=MkTypLib.exe
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
CPP=cl.exe
# ADD BASE CPP /nologo /MD /W3 /GX /YX /O2 /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /c
# ADD CPP /nologo /W3 /GX /YX /O2 /D "NDEBUG" /D "WIN32" /D "INC_OLE2" /D "UNICODE" /FR /c
CPP_PROJ=/nologo /W3 /GX /YX /O2 /D "NDEBUG" /D "WIN32" /D "INC_OLE2" /D\
 "UNICODE" /FR$(INTDIR)/ /Fp$(OUTDIR)/"dispdemo.pch" /Fo$(INTDIR)/ /c 
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
RSC_PROJ=/l 0x409 /fo$(INTDIR)/"dispdemo.res" /d "NDEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# SUBTRACT BASE BSC32 /Iu
# ADD BSC32 /nologo
# SUBTRACT BSC32 /Iu
BSC32_FLAGS=/nologo /o$(OUTDIR)/"dispdemo.bsc" 
BSC32_SBRS= \
	$(INTDIR)/misc.sbr \
	$(INTDIR)/clsid.sbr \
	$(INTDIR)/crempoly.sbr \
	$(INTDIR)/winmain.sbr

$(OUTDIR)/dispdemo.bsc : $(OUTDIR)  $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib mfc30.lib mfco30.lib mfcd30.lib mfcuia32.lib mfcans32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib /NOLOGO /SUBSYSTEM:windows /MACHINE:I386
# ADD LINK32 user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /NOLOGO /SUBSYSTEM:windows /MACHINE:I386
LINK32_FLAGS=user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib\
 shell32.lib ole32.lib oleaut32.lib uuid.lib /NOLOGO /SUBSYSTEM:windows\
 /INCREMENTAL:no /PDB:$(OUTDIR)/"dispdemo.pdb" /MACHINE:I386\
 /OUT:$(OUTDIR)/"dispdemo.exe" 
DEF_FILE=
LINK32_OBJS= \
	$(INTDIR)/misc.obj \
	$(INTDIR)/clsid.obj \
	$(INTDIR)/dispdemo.res \
	$(INTDIR)/crempoly.obj \
	$(INTDIR)/winmain.obj

$(OUTDIR)/dispdemo.exe : $(OUTDIR)  $(DEF_FILE) $(LINK32_OBJS)
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

ALL : $(OUTDIR)/dispdemo.exe $(OUTDIR)/dispdemo.bsc

$(OUTDIR) : 
    if not exist $(OUTDIR)/nul mkdir $(OUTDIR)

MTL=MkTypLib.exe
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
CPP=cl.exe
# ADD BASE CPP /nologo /MD /W3 /GX /Zi /YX /Od /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /c
# ADD CPP /nologo /W3 /GX /Zi /YX /Od /D "_DEBUG" /D "WIN32" /D "INC_OLE2" /D "UNICODE" /FR /c
CPP_PROJ=/nologo /W3 /GX /Zi /YX /Od /D "_DEBUG" /D "WIN32" /D "INC_OLE2" /D\
 "UNICODE" /FR$(INTDIR)/ /Fp$(OUTDIR)/"dispdemo.pch" /Fo$(INTDIR)/\
 /Fd$(OUTDIR)/"dispdemo.pdb" /c 
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
RSC_PROJ=/l 0x409 /fo$(INTDIR)/"dispdemo.res" /d "_DEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# SUBTRACT BASE BSC32 /Iu
# ADD BSC32 /nologo
# SUBTRACT BSC32 /Iu
BSC32_FLAGS=/nologo /o$(OUTDIR)/"dispdemo.bsc" 
BSC32_SBRS= \
	$(INTDIR)/misc.sbr \
	$(INTDIR)/clsid.sbr \
	$(INTDIR)/crempoly.sbr \
	$(INTDIR)/winmain.sbr

$(OUTDIR)/dispdemo.bsc : $(OUTDIR)  $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib mfc30d.lib mfco30d.lib mfcd30d.lib mfcuia32.lib mfcans32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib /NOLOGO /SUBSYSTEM:windows /DEBUG /MACHINE:I386
# ADD LINK32 user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /NOLOGO /SUBSYSTEM:windows /DEBUG /MACHINE:I386
LINK32_FLAGS=user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib\
 shell32.lib ole32.lib oleaut32.lib uuid.lib /NOLOGO /SUBSYSTEM:windows\
 /INCREMENTAL:yes /PDB:$(OUTDIR)/"dispdemo.pdb" /DEBUG /MACHINE:I386\
 /OUT:$(OUTDIR)/"dispdemo.exe" 
DEF_FILE=
LINK32_OBJS= \
	$(INTDIR)/misc.obj \
	$(INTDIR)/clsid.obj \
	$(INTDIR)/dispdemo.res \
	$(INTDIR)/crempoly.obj \
	$(INTDIR)/winmain.obj

$(OUTDIR)/dispdemo.exe : $(OUTDIR)  $(DEF_FILE) $(LINK32_OBJS)
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

ALL : $(OUTDIR)/dispdemo.exe $(OUTDIR)/dispdemo.bsc

$(OUTDIR) : 
    if not exist $(OUTDIR)/nul mkdir $(OUTDIR)

MTL=MkTypLib.exe
# ADD BASE MTL /nologo /D "NDEBUG" /mips
# ADD MTL /nologo /D "NDEBUG" /mips
MTL_PROJ=/nologo /D "NDEBUG" /mips 
CPP=cl.exe
# ADD BASE CPP /nologo /MD /Gt0 /QMOb2000 /W3 /GX /YX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /c
# ADD CPP /nologo /Gt0 /QMOb2000 /W3 /GX /YX /O2 /D "NDEBUG" /D "WIN32" /D "INC_OLE2" /D "UNICODE" /FR /c
CPP_PROJ=/nologo /Gt0 /QMOb2000 /W3 /GX /YX /O2 /D "NDEBUG" /D "WIN32" /D\
 "INC_OLE2" /D "UNICODE" /FR$(INTDIR)/ /Fp$(OUTDIR)/"dispdemo.pch" /Fo$(INTDIR)/\
 /c 
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
RSC_PROJ=/l 0x409 /fo$(INTDIR)/"dispdemo.res" /d "NDEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# SUBTRACT BASE BSC32 /Iu
# ADD BSC32 /nologo
# SUBTRACT BSC32 /Iu
BSC32_FLAGS=/nologo /o$(OUTDIR)/"dispdemo.bsc" 
BSC32_SBRS= \
	$(INTDIR)/misc.sbr \
	$(INTDIR)/clsid.sbr \
	$(INTDIR)/crempoly.sbr \
	$(INTDIR)/winmain.sbr

$(OUTDIR)/dispdemo.bsc : $(OUTDIR)  $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 /NOLOGO /SUBSYSTEM:windows /MACHINE:MIPS
# SUBTRACT BASE LINK32 /PDB:none
# ADD LINK32 user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /NOLOGO /SUBSYSTEM:windows /MACHINE:MIPS
# SUBTRACT LINK32 /PDB:none
LINK32_FLAGS=user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib\
 shell32.lib ole32.lib oleaut32.lib uuid.lib /NOLOGO /SUBSYSTEM:windows\
 /PDB:$(OUTDIR)/"dispdemo.pdb" /MACHINE:MIPS /OUT:$(OUTDIR)/"dispdemo.exe" 
DEF_FILE=
LINK32_OBJS= \
	$(INTDIR)/misc.obj \
	$(INTDIR)/clsid.obj \
	$(INTDIR)/dispdemo.res \
	$(INTDIR)/crempoly.obj \
	$(INTDIR)/winmain.obj

$(OUTDIR)/dispdemo.exe : $(OUTDIR)  $(DEF_FILE) $(LINK32_OBJS)
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

ALL : $(OUTDIR)/dispdemo.exe $(OUTDIR)/dispdemo.bsc

$(OUTDIR) : 
    if not exist $(OUTDIR)/nul mkdir $(OUTDIR)

MTL=MkTypLib.exe
# ADD BASE MTL /nologo /D "_DEBUG" /mips
# ADD MTL /nologo /D "_DEBUG" /mips
MTL_PROJ=/nologo /D "_DEBUG" /mips 
CPP=cl.exe
# ADD BASE CPP /nologo /MD /Gt0 /QMOb2000 /W3 /GX /Zi /YX /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /c
# ADD CPP /nologo /Gt0 /QMOb2000 /W3 /GX /Zi /YX /Od /D "_DEBUG" /D "WIN32" /D "INC_OLE2" /D "UNICODE" /FR /c
CPP_PROJ=/nologo /Gt0 /QMOb2000 /W3 /GX /Zi /YX /Od /D "_DEBUG" /D "WIN32" /D\
 "INC_OLE2" /D "UNICODE" /FR$(INTDIR)/ /Fp$(OUTDIR)/"dispdemo.pch" /Fo$(INTDIR)/\
 /Fd$(OUTDIR)/"dispdemo.pdb" /c 
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
RSC_PROJ=/l 0x409 /fo$(INTDIR)/"dispdemo.res" /d "_DEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# SUBTRACT BASE BSC32 /Iu
# ADD BSC32 /nologo
# SUBTRACT BSC32 /Iu
BSC32_FLAGS=/nologo /o$(OUTDIR)/"dispdemo.bsc" 
BSC32_SBRS= \
	$(INTDIR)/misc.sbr \
	$(INTDIR)/clsid.sbr \
	$(INTDIR)/crempoly.sbr \
	$(INTDIR)/winmain.sbr

$(OUTDIR)/dispdemo.bsc : $(OUTDIR)  $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 /NOLOGO /SUBSYSTEM:windows /DEBUG /MACHINE:MIPS
# SUBTRACT BASE LINK32 /PDB:none
# ADD LINK32 user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /NOLOGO /SUBSYSTEM:windows /DEBUG /MACHINE:MIPS
# SUBTRACT LINK32 /PDB:none
LINK32_FLAGS=user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib\
 shell32.lib ole32.lib oleaut32.lib uuid.lib /NOLOGO /SUBSYSTEM:windows\
 /PDB:$(OUTDIR)/"dispdemo.pdb" /DEBUG /MACHINE:MIPS\
 /OUT:$(OUTDIR)/"dispdemo.exe" 
DEF_FILE=
LINK32_OBJS= \
	$(INTDIR)/misc.obj \
	$(INTDIR)/clsid.obj \
	$(INTDIR)/dispdemo.res \
	$(INTDIR)/crempoly.obj \
	$(INTDIR)/winmain.obj

$(OUTDIR)/dispdemo.exe : $(OUTDIR)  $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 

################################################################################
# Begin Group "Source Files"

################################################################################
# Begin Source File

SOURCE=.\misc.cpp
DEP_MISC_=\
	.\dispdemo.h\
	.\hostenv.h\
	.\clsid.h

!IF  "$(CFG)" == "Win32 (80x86) Release"

$(INTDIR)/misc.obj :  $(SOURCE)  $(DEP_MISC_) $(INTDIR)
   $(CPP) /nologo /W3 /GX /YX /O2 /D "NDEBUG" /D "WIN32" /D "INC_OLE2" /D\
 "UNICODE" /FR$(INTDIR)/ /Fp$(OUTDIR)/"dispdemo.pch" /Fo$(INTDIR)/ /c  $(SOURCE)\
 

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

$(INTDIR)/misc.obj :  $(SOURCE)  $(DEP_MISC_) $(INTDIR)
   $(CPP) /nologo /W3 /GX /Zi /YX /Od /D "_DEBUG" /D "WIN32" /D "INC_OLE2" /D\
 "UNICODE" /FR$(INTDIR)/ /Fp$(OUTDIR)/"dispdemo.pch" /Fo$(INTDIR)/\
 /Fd$(OUTDIR)/"dispdemo.pdb" /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

# ADD BASE CPP /Gt0 /QMOb2000
# ADD CPP /Gt0 /QMOb2000 /YX

$(INTDIR)/misc.obj :  $(SOURCE)  $(DEP_MISC_) $(INTDIR)
   $(CPP) /nologo /Gt0 /QMOb2000 /W3 /GX /YX /O2 /D "NDEBUG" /D "WIN32" /D\
 "INC_OLE2" /D "UNICODE" /FR$(INTDIR)/ /Fp$(OUTDIR)/"dispdemo.pch" /Fo$(INTDIR)/\
 /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

$(INTDIR)/misc.obj :  $(SOURCE)  $(DEP_MISC_) $(INTDIR)
   $(CPP) /nologo /Gt0 /QMOb2000 /W3 /GX /Zi /YX /Od /D "_DEBUG" /D "WIN32" /D\
 "INC_OLE2" /D "UNICODE" /FR$(INTDIR)/ /Fp$(OUTDIR)/"dispdemo.pch" /Fo$(INTDIR)/\
 /Fd$(OUTDIR)/"dispdemo.pdb" /c  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\clsid.c
DEP_CLSID=\
	.\clsid.h

!IF  "$(CFG)" == "Win32 (80x86) Release"

$(INTDIR)/clsid.obj :  $(SOURCE)  $(DEP_CLSID) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

$(INTDIR)/clsid.obj :  $(SOURCE)  $(DEP_CLSID) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

$(INTDIR)/clsid.obj :  $(SOURCE)  $(DEP_CLSID) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

$(INTDIR)/clsid.obj :  $(SOURCE)  $(DEP_CLSID) $(INTDIR)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\dispdemo.rc
DEP_DISPD=\
	.\dispdemo.ico

!IF  "$(CFG)" == "Win32 (80x86) Release"

$(INTDIR)/dispdemo.res :  $(SOURCE)  $(DEP_DISPD) $(INTDIR)
   $(RSC) $(RSC_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

$(INTDIR)/dispdemo.res :  $(SOURCE)  $(DEP_DISPD) $(INTDIR)
   $(RSC) $(RSC_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

$(INTDIR)/dispdemo.res :  $(SOURCE)  $(DEP_DISPD) $(INTDIR)
   $(RSC) $(RSC_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

$(INTDIR)/dispdemo.res :  $(SOURCE)  $(DEP_DISPD) $(INTDIR)
   $(RSC) $(RSC_PROJ)  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\crempoly.cpp
DEP_CREMP=\
	.\dispdemo.h\
	.\crempoly.h\
	.\hostenv.h\
	.\clsid.h

!IF  "$(CFG)" == "Win32 (80x86) Release"

$(INTDIR)/crempoly.obj :  $(SOURCE)  $(DEP_CREMP) $(INTDIR)
   $(CPP) /nologo /W3 /GX /YX /O2 /D "NDEBUG" /D "WIN32" /D "INC_OLE2" /D\
 "UNICODE" /FR$(INTDIR)/ /Fp$(OUTDIR)/"dispdemo.pch" /Fo$(INTDIR)/ /c  $(SOURCE)\
 

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

$(INTDIR)/crempoly.obj :  $(SOURCE)  $(DEP_CREMP) $(INTDIR)
   $(CPP) /nologo /W3 /GX /Zi /YX /Od /D "_DEBUG" /D "WIN32" /D "INC_OLE2" /D\
 "UNICODE" /FR$(INTDIR)/ /Fp$(OUTDIR)/"dispdemo.pch" /Fo$(INTDIR)/\
 /Fd$(OUTDIR)/"dispdemo.pdb" /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

# ADD BASE CPP /Gt0 /QMOb2000
# ADD CPP /Gt0 /QMOb2000 /YX

$(INTDIR)/crempoly.obj :  $(SOURCE)  $(DEP_CREMP) $(INTDIR)
   $(CPP) /nologo /Gt0 /QMOb2000 /W3 /GX /YX /O2 /D "NDEBUG" /D "WIN32" /D\
 "INC_OLE2" /D "UNICODE" /FR$(INTDIR)/ /Fp$(OUTDIR)/"dispdemo.pch" /Fo$(INTDIR)/\
 /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

$(INTDIR)/crempoly.obj :  $(SOURCE)  $(DEP_CREMP) $(INTDIR)
   $(CPP) /nologo /Gt0 /QMOb2000 /W3 /GX /Zi /YX /Od /D "_DEBUG" /D "WIN32" /D\
 "INC_OLE2" /D "UNICODE" /FR$(INTDIR)/ /Fp$(OUTDIR)/"dispdemo.pch" /Fo$(INTDIR)/\
 /Fd$(OUTDIR)/"dispdemo.pdb" /c  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\winmain.cpp
DEP_WINMA=\
	.\dispdemo.h\
	.\hostenv.h\
	.\clsid.h

!IF  "$(CFG)" == "Win32 (80x86) Release"

$(INTDIR)/winmain.obj :  $(SOURCE)  $(DEP_WINMA) $(INTDIR)
   $(CPP) /nologo /W3 /GX /YX /O2 /D "NDEBUG" /D "WIN32" /D "INC_OLE2" /D\
 "UNICODE" /FR$(INTDIR)/ /Fp$(OUTDIR)/"dispdemo.pch" /Fo$(INTDIR)/ /c  $(SOURCE)\
 

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

$(INTDIR)/winmain.obj :  $(SOURCE)  $(DEP_WINMA) $(INTDIR)
   $(CPP) /nologo /W3 /GX /Zi /YX /Od /D "_DEBUG" /D "WIN32" /D "INC_OLE2" /D\
 "UNICODE" /FR$(INTDIR)/ /Fp$(OUTDIR)/"dispdemo.pch" /Fo$(INTDIR)/\
 /Fd$(OUTDIR)/"dispdemo.pdb" /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

# ADD BASE CPP /Gt0 /QMOb2000
# ADD CPP /Gt0 /QMOb2000 /YX

$(INTDIR)/winmain.obj :  $(SOURCE)  $(DEP_WINMA) $(INTDIR)
   $(CPP) /nologo /Gt0 /QMOb2000 /W3 /GX /YX /O2 /D "NDEBUG" /D "WIN32" /D\
 "INC_OLE2" /D "UNICODE" /FR$(INTDIR)/ /Fp$(OUTDIR)/"dispdemo.pch" /Fo$(INTDIR)/\
 /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

$(INTDIR)/winmain.obj :  $(SOURCE)  $(DEP_WINMA) $(INTDIR)
   $(CPP) /nologo /Gt0 /QMOb2000 /W3 /GX /Zi /YX /Od /D "_DEBUG" /D "WIN32" /D\
 "INC_OLE2" /D "UNICODE" /FR$(INTDIR)/ /Fp$(OUTDIR)/"dispdemo.pch" /Fo$(INTDIR)/\
 /Fd$(OUTDIR)/"dispdemo.pdb" /c  $(SOURCE) 

!ENDIF 

# End Source File
# End Group
# End Project
################################################################################
