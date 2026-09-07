# Microsoft Visual C++ Generated NMAKE File, Format Version 40000
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

!IF "$(CFG)" == ""
CFG=RegSvr32 - Win32 Debug
!MESSAGE No configuration specified.  Defaulting to RegSvr32 - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "RegSvr32 - Win32 Release" && "$(CFG)" !=\
 "RegSvr32 - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "REGSVR32.MAK" CFG="RegSvr32 - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "RegSvr32 - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "RegSvr32 - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 
################################################################################
# Begin Project
# PROP Target_Last_Scanned "RegSvr32 - Win32 Debug"
CPP=cl.exe
RSC=rc.exe
MTL=mktyplib.exe

!IF  "$(CFG)" == "RegSvr32 - Win32 Release"

# PROP BASE Use_MFC 1
# PROP BASE Use_Debug_Libraries 0
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir WinRel
# PROP Intermediate_Dir WinRel
OUTDIR=.\WinRel
INTDIR=.\WinRel

ALL : "$(OUTDIR)\REGSVR32.exe" "$(OUTDIR)\REGSVR32.bsc"

CLEAN : 
	-@erase ".\WinRel\REGSVR32.bsc"
	-@erase ".\WinRel\REGSVR.SBR"
	-@erase ".\WinRel\REGSVR32.exe"
	-@erase ".\WinRel\REGSVR.OBJ"
	-@erase ".\WinRel\regsvr.res"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MT /W3 /GX /O1 /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /FR /YX /c
# ADD CPP /nologo /W3 /GX /O1 /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /FR /YX /c
CPP_PROJ=/nologo /ML /W3 /GX /O1 /D "NDEBUG" /D "_WINDOWS" /D "_MBCS"\
 /FR"$(INTDIR)/" /Fp"$(INTDIR)/REGSVR32.pch" /YX /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\WinRel/
CPP_SBRS=.\WinRel/
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
# ADD BASE RSC /l 0x1 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_WIN32"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/regsvr.res" /d "NDEBUG" /d "_WIN32" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# SUBTRACT BASE BSC32 /Iu
# ADD BSC32 /nologo
# SUBTRACT BSC32 /Iu
BSC32_FLAGS=/nologo /o"$(OUTDIR)/REGSVR32.bsc" 
BSC32_SBRS= \
	"$(INTDIR)/REGSVR.SBR"

"$(OUTDIR)\REGSVR32.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib oldnames.lib nafxcw.lib mfcuia32.lib mfcans32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib /nologo /stack:0x10240 /subsystem:windows /machine:IX86 /nodefaultlib
# ADD LINK32 kernel32.lib user32.lib ole32.lib /nologo /stack:0x10240 /subsystem:windows /machine:IX86
# SUBTRACT LINK32 /pdb:none
LINK32_FLAGS=kernel32.lib user32.lib ole32.lib /nologo /stack:0x10240\
 /subsystem:windows /incremental:no /pdb:"$(OUTDIR)/REGSVR32.pdb" /machine:IX86\
 /out:"$(OUTDIR)/REGSVR32.exe" 
LINK32_OBJS= \
	"$(INTDIR)/REGSVR.OBJ" \
	"$(INTDIR)/regsvr.res"

"$(OUTDIR)\REGSVR32.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "RegSvr32 - Win32 Debug"

# PROP BASE Use_MFC 1
# PROP BASE Use_Debug_Libraries 1
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir WinDebug
# PROP Intermediate_Dir WinDebug
OUTDIR=.\WinDebug
INTDIR=.\WinDebug

ALL : "$(OUTDIR)\REGSVR32.exe" "$(OUTDIR)\REGSVR32.bsc"

CLEAN : 
	-@erase ".\WinDebug\vc40.pdb"
	-@erase ".\WinDebug\REGSVR32.bsc"
	-@erase ".\WinDebug\REGSVR.SBR"
	-@erase ".\WinDebug\REGSVR32.exe"
	-@erase ".\WinDebug\REGSVR.OBJ"
	-@erase ".\WinDebug\regsvr.res"
	-@erase ".\WinDebug\REGSVR32.ilk"
	-@erase ".\WinDebug\REGSVR32.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MT /W3 /GX /Zi /Od /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /FR /YX /c
# ADD CPP /nologo /W3 /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /FR /YX /c
CPP_PROJ=/nologo /MLd /W3 /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE"\
 /FR"$(INTDIR)/" /Fp"$(INTDIR)/REGSVR32.pch" /YX /Fo"$(INTDIR)/" /Fd"$(INTDIR)/"\
 /c 
CPP_OBJS=.\WinDebug/
CPP_SBRS=.\WinDebug/
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
# ADD BASE RSC /l 0x1 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/regsvr.res" /d "_DEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# SUBTRACT BASE BSC32 /Iu
# ADD BSC32 /nologo
# SUBTRACT BSC32 /Iu
BSC32_FLAGS=/nologo /o"$(OUTDIR)/REGSVR32.bsc" 
BSC32_SBRS= \
	"$(INTDIR)/REGSVR.SBR"

"$(OUTDIR)\REGSVR32.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib oldnames.lib nafxcwd.lib mfcuia32.lib mfcans32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib /nologo /stack:0x10240 /subsystem:windows /debug /machine:IX86 /nodefaultlib
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /stack:0x10240 /subsystem:windows /debug /machine:IX86
# SUBTRACT LINK32 /pdb:none
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib /nologo /stack:0x10240 /subsystem:windows /incremental:yes\
 /pdb:"$(OUTDIR)/REGSVR32.pdb" /debug /machine:IX86\
 /out:"$(OUTDIR)/REGSVR32.exe" 
LINK32_OBJS= \
	"$(INTDIR)/REGSVR.OBJ" \
	"$(INTDIR)/regsvr.res"

"$(OUTDIR)\REGSVR32.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 

.c{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.c{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

################################################################################
# Begin Target

# Name "RegSvr32 - Win32 Debug"
# Name "RegSvr32 - Win32 Release"

!IF  "$(CFG)" == "RegSvr32 - Win32 Debug"

!ELSEIF  "$(CFG)" == "RegSvr32 - Win32 Release"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\REGSVR.CPP

!IF  "$(CFG)" == "RegSvr32 - Win32 Release"


"$(INTDIR)\REGSVR.OBJ" : $(SOURCE) "$(INTDIR)"

"$(INTDIR)\REGSVR.SBR" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "RegSvr32 - Win32 Debug"


"$(INTDIR)\REGSVR.OBJ" : $(SOURCE) "$(INTDIR)"

"$(INTDIR)\REGSVR.SBR" : $(SOURCE) "$(INTDIR)"

"$(INTDIR)\vc40.pdb" : $(SOURCE) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\regsvr.rc
DEP_RSC_REGSV=\
	".\regsvr.rc2"\
	

"$(INTDIR)\regsvr.res" : $(SOURCE) $(DEP_RSC_REGSV) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


# End Source File
# End Target
# End Project
################################################################################
