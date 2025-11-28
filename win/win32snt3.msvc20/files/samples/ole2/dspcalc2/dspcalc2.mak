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
!MESSAGE NMAKE /f "dspcalc2.mak" CFG="Win32 (80x86) Debug"
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

ALL : MTL_TLBS $(OUTDIR)/dspcalc2.exe $(OUTDIR)/dspcalc2.bsc

$(OUTDIR) : 
    if not exist $(OUTDIR)/nul mkdir $(OUTDIR)

MTL=MkTypLib.exe
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /D "WIN32" /win32
MTL_PROJ=/nologo /D "NDEBUG" /D "WIN32" /win32 

MTL_TLBS : .\dspcalc2.tlb
CPP=cl.exe
# ADD BASE CPP /nologo /MD /W3 /GX /YX /O2 /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /c
# ADD CPP /nologo /W3 /GX /YX /O2 /D "NDEBUG" /D "WIN32" /D "INC_OLE2" /D "UNICODE" /FR /c
CPP_PROJ=/nologo /W3 /GX /YX /O2 /D "NDEBUG" /D "WIN32" /D "INC_OLE2" /D\
 "UNICODE" /FR$(INTDIR)/ /Fp$(OUTDIR)/"dspcalc2.pch" /Fo$(INTDIR)/ /c 
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
RSC_PROJ=/l 0x409 /fo$(INTDIR)/"dspcalc2.res" /d "NDEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# SUBTRACT BASE BSC32 /Iu
# ADD BSC32 /nologo
# SUBTRACT BSC32 /Iu
BSC32_FLAGS=/nologo /o$(OUTDIR)/"dspcalc2.bsc" 
BSC32_SBRS= \
	$(INTDIR)/main.sbr \
	$(INTDIR)/dspcalc2.sbr \
	$(INTDIR)/clsid.sbr

$(OUTDIR)/dspcalc2.bsc : $(OUTDIR)  $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib mfc30.lib mfco30.lib mfcd30.lib mfcuia32.lib mfcans32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib /NOLOGO /SUBSYSTEM:windows /MACHINE:I386
# ADD LINK32 user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /NOLOGO /SUBSYSTEM:windows /MACHINE:I386
LINK32_FLAGS=user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib\
 shell32.lib ole32.lib oleaut32.lib uuid.lib /NOLOGO /SUBSYSTEM:windows\
 /INCREMENTAL:no /PDB:$(OUTDIR)/"dspcalc2.pdb" /MACHINE:I386\
 /OUT:$(OUTDIR)/"dspcalc2.exe" 
DEF_FILE=
LINK32_OBJS= \
	$(INTDIR)/main.obj \
	$(INTDIR)/dspcalc2.obj \
	$(INTDIR)/clsid.obj \
	$(INTDIR)/dspcalc2.res

$(OUTDIR)/dspcalc2.exe : $(OUTDIR)  $(DEF_FILE) $(LINK32_OBJS)
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

ALL : MTL_TLBS $(OUTDIR)/dspcalc2.exe $(OUTDIR)/dspcalc2.bsc

$(OUTDIR) : 
    if not exist $(OUTDIR)/nul mkdir $(OUTDIR)

MTL=MkTypLib.exe
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /D "WIN32" /win32
MTL_PROJ=/nologo /D "_DEBUG" /D "WIN32" /win32 

MTL_TLBS : .\dspcalc2.tlb
CPP=cl.exe
# ADD BASE CPP /nologo /MD /W3 /GX /Zi /YX /Od /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /c
# ADD CPP /nologo /W3 /GX /Zi /YX /Od /D "_DEBUG" /D "WIN32" /D "INC_OLE2" /D "UNICODE" /FR /c
CPP_PROJ=/nologo /W3 /GX /Zi /YX /Od /D "_DEBUG" /D "WIN32" /D "INC_OLE2" /D\
 "UNICODE" /FR$(INTDIR)/ /Fp$(OUTDIR)/"dspcalc2.pch" /Fo$(INTDIR)/\
 /Fd$(OUTDIR)/"dspcalc2.pdb" /c 
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
RSC_PROJ=/l 0x409 /fo$(INTDIR)/"dspcalc2.res" /d "_DEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# SUBTRACT BASE BSC32 /Iu
# ADD BSC32 /nologo
# SUBTRACT BSC32 /Iu
BSC32_FLAGS=/nologo /o$(OUTDIR)/"dspcalc2.bsc" 
BSC32_SBRS= \
	$(INTDIR)/main.sbr \
	$(INTDIR)/dspcalc2.sbr \
	$(INTDIR)/clsid.sbr

$(OUTDIR)/dspcalc2.bsc : $(OUTDIR)  $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib mfc30d.lib mfco30d.lib mfcd30d.lib mfcuia32.lib mfcans32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib /NOLOGO /SUBSYSTEM:windows /DEBUG /MACHINE:I386
# ADD LINK32 user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /NOLOGO /SUBSYSTEM:windows /DEBUG /MACHINE:I386
LINK32_FLAGS=user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib\
 shell32.lib ole32.lib oleaut32.lib uuid.lib /NOLOGO /SUBSYSTEM:windows\
 /INCREMENTAL:yes /PDB:$(OUTDIR)/"dspcalc2.pdb" /DEBUG /MACHINE:I386\
 /OUT:$(OUTDIR)/"dspcalc2.exe" 
DEF_FILE=
LINK32_OBJS= \
	$(INTDIR)/main.obj \
	$(INTDIR)/dspcalc2.obj \
	$(INTDIR)/clsid.obj \
	$(INTDIR)/dspcalc2.res

$(OUTDIR)/dspcalc2.exe : $(OUTDIR)  $(DEF_FILE) $(LINK32_OBJS)
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

ALL : MTL_TLBS $(OUTDIR)/dspcalc2.exe $(OUTDIR)/dspcalc2.bsc

$(OUTDIR) : 
    if not exist $(OUTDIR)/nul mkdir $(OUTDIR)

MTL=MkTypLib.exe
# ADD BASE MTL /nologo /D "NDEBUG" /mips
# ADD MTL /nologo /D "NDEBUG" /D "WIN32" /mips
MTL_PROJ=/nologo /D "NDEBUG" /D "WIN32" /mips 

MTL_TLBS : .\dspcalc2.tlb
CPP=cl.exe
# ADD BASE CPP /nologo /MD /Gt0 /QMOb2000 /W3 /GX /YX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /c
# ADD CPP /nologo /Gt0 /QMOb2000 /W3 /GX /YX /O2 /D "NDEBUG" /D "WIN32" /D "INC_OLE2" /D "UNICODE" /FR /c
CPP_PROJ=/nologo /Gt0 /QMOb2000 /W3 /GX /YX /O2 /D "NDEBUG" /D "WIN32" /D\
 "INC_OLE2" /D "UNICODE" /FR$(INTDIR)/ /Fp$(OUTDIR)/"dspcalc2.pch" /Fo$(INTDIR)/\
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
RSC_PROJ=/l 0x409 /fo$(INTDIR)/"dspcalc2.res" /d "NDEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# SUBTRACT BASE BSC32 /Iu
# ADD BSC32 /nologo
# SUBTRACT BSC32 /Iu
BSC32_FLAGS=/nologo /o$(OUTDIR)/"dspcalc2.bsc" 
BSC32_SBRS= \
	$(INTDIR)/main.sbr \
	$(INTDIR)/dspcalc2.sbr \
	$(INTDIR)/clsid.sbr

$(OUTDIR)/dspcalc2.bsc : $(OUTDIR)  $(BSC32_SBRS)
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
 /PDB:$(OUTDIR)/"dspcalc2.pdb" /MACHINE:MIPS /OUT:$(OUTDIR)/"dspcalc2.exe" 
DEF_FILE=
LINK32_OBJS= \
	$(INTDIR)/main.obj \
	$(INTDIR)/dspcalc2.obj \
	$(INTDIR)/clsid.obj \
	$(INTDIR)/dspcalc2.res

$(OUTDIR)/dspcalc2.exe : $(OUTDIR)  $(DEF_FILE) $(LINK32_OBJS)
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

ALL : MTL_TLBS $(OUTDIR)/dspcalc2.exe $(OUTDIR)/dspcalc2.bsc

$(OUTDIR) : 
    if not exist $(OUTDIR)/nul mkdir $(OUTDIR)

MTL=MkTypLib.exe
# ADD BASE MTL /nologo /D "_DEBUG" /mips
# ADD MTL /nologo /D "_DEBUG" /D "WIN32" /mips
MTL_PROJ=/nologo /D "_DEBUG" /D "WIN32" /mips 

MTL_TLBS : .\dspcalc2.tlb
CPP=cl.exe
# ADD BASE CPP /nologo /MD /Gt0 /QMOb2000 /W3 /GX /Zi /YX /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /c
# ADD CPP /nologo /Gt0 /QMOb2000 /W3 /GX /Zi /YX /Od /D "_DEBUG" /D "WIN32" /D "INC_OLE2" /D "UNICODE" /FR /c
CPP_PROJ=/nologo /Gt0 /QMOb2000 /W3 /GX /Zi /YX /Od /D "_DEBUG" /D "WIN32" /D\
 "INC_OLE2" /D "UNICODE" /FR$(INTDIR)/ /Fp$(OUTDIR)/"dspcalc2.pch" /Fo$(INTDIR)/\
 /Fd$(OUTDIR)/"dspcalc2.pdb" /c 
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
RSC_PROJ=/l 0x409 /fo$(INTDIR)/"dspcalc2.res" /d "_DEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# SUBTRACT BASE BSC32 /Iu
# ADD BSC32 /nologo
# SUBTRACT BSC32 /Iu
BSC32_FLAGS=/nologo /o$(OUTDIR)/"dspcalc2.bsc" 
BSC32_SBRS= \
	$(INTDIR)/main.sbr \
	$(INTDIR)/dspcalc2.sbr \
	$(INTDIR)/clsid.sbr

$(OUTDIR)/dspcalc2.bsc : $(OUTDIR)  $(BSC32_SBRS)
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
 /PDB:$(OUTDIR)/"dspcalc2.pdb" /DEBUG /MACHINE:MIPS\
 /OUT:$(OUTDIR)/"dspcalc2.exe" 
DEF_FILE=
LINK32_OBJS= \
	$(INTDIR)/main.obj \
	$(INTDIR)/dspcalc2.obj \
	$(INTDIR)/clsid.obj \
	$(INTDIR)/dspcalc2.res

$(OUTDIR)/dspcalc2.exe : $(OUTDIR)  $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 

################################################################################
# Begin Group "Source Files"

################################################################################
# Begin Source File

SOURCE=.\main.cpp
DEP_MAIN_=\
	.\dspcalc2.h\
	.\hostenv.h\
	.\clsid.h\
	.\calctype.h

!IF  "$(CFG)" == "Win32 (80x86) Release"

$(INTDIR)/main.obj :  $(SOURCE)  $(DEP_MAIN_) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

$(INTDIR)/main.obj :  $(SOURCE)  $(DEP_MAIN_) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

$(INTDIR)/main.obj :  $(SOURCE)  $(DEP_MAIN_) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

$(INTDIR)/main.obj :  $(SOURCE)  $(DEP_MAIN_) $(INTDIR)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\dspcalc2.cpp
DEP_DSPCA=\
	.\dspcalc2.h\
	.\hostenv.h\
	.\clsid.h\
	.\calctype.h

!IF  "$(CFG)" == "Win32 (80x86) Release"

$(INTDIR)/dspcalc2.obj :  $(SOURCE)  $(DEP_DSPCA) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

$(INTDIR)/dspcalc2.obj :  $(SOURCE)  $(DEP_DSPCA) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

$(INTDIR)/dspcalc2.obj :  $(SOURCE)  $(DEP_DSPCA) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

$(INTDIR)/dspcalc2.obj :  $(SOURCE)  $(DEP_DSPCA) $(INTDIR)

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

SOURCE=.\dspcalc2.rc
DEP_DSPCAL=\
	.\dspcalc2.ico

!IF  "$(CFG)" == "Win32 (80x86) Release"

$(INTDIR)/dspcalc2.res :  $(SOURCE)  $(DEP_DSPCAL) $(INTDIR)
   $(RSC) /l 0x409 /fo$(INTDIR)/"dspcalc2.res" /i "$(OUTDIR)" /d "NDEBUG"\
  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

$(INTDIR)/dspcalc2.res :  $(SOURCE)  $(DEP_DSPCAL) $(INTDIR)
   $(RSC) /l 0x409 /fo$(INTDIR)/"dspcalc2.res" /i "$(OUTDIR)" /d "_DEBUG"\
  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

$(INTDIR)/dspcalc2.res :  $(SOURCE)  $(DEP_DSPCAL) $(INTDIR)
   $(RSC) /l 0x409 /fo$(INTDIR)/"dspcalc2.res" /i "$(OUTDIR)" /d "NDEBUG"\
  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

$(INTDIR)/dspcalc2.res :  $(SOURCE)  $(DEP_DSPCAL) $(INTDIR)
   $(RSC) /l 0x409 /fo$(INTDIR)/"dspcalc2.res" /i "$(OUTDIR)" /d "_DEBUG"\
  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\calctype.odl

!IF  "$(CFG)" == "Win32 (80x86) Release"

# ADD MTL /tlb "dspcalc2.tlb" /h "calctype.h"

.\dspcalc2.tlb :  $(SOURCE)  $(OUTDIR)
   $(MTL) /nologo /D "NDEBUG" /D "WIN32" /tlb "dspcalc2.tlb" /h "calctype.h"\
 /win32   $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

# ADD MTL /tlb "dspcalc2.tlb" /h "calctype.h"

.\dspcalc2.tlb :  $(SOURCE)  $(OUTDIR)
   $(MTL) /nologo /D "_DEBUG" /D "WIN32" /tlb "dspcalc2.tlb" /h "calctype.h"\
 /win32   $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

# ADD MTL /tlb "dspcalc2.tlb" /h "calctype.h"

.\dspcalc2.tlb :  $(SOURCE)  $(OUTDIR)
   $(MTL) /nologo /D "NDEBUG" /D "WIN32" /tlb "dspcalc2.tlb" /h "calctype.h"\
 /mips   $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

# ADD MTL /tlb "dspcalc2.tlb" /h "calctype.h"

.\dspcalc2.tlb :  $(SOURCE)  $(OUTDIR)
   $(MTL) /nologo /D "_DEBUG" /D "WIN32" /tlb "dspcalc2.tlb" /h "calctype.h"\
 /mips   $(SOURCE) 

!ENDIF 

# End Source File
# End Group
# End Project
################################################################################
