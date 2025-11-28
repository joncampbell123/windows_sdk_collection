# Microsoft Developer Studio Generated NMAKE File, Format Version 4.20
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

!IF "$(CFG)" == ""
CFG=dsshow - Win32 Debug
!MESSAGE No configuration specified.  Defaulting to dsshow - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "dsshow - Win32 Release" && "$(CFG)" != "dsshow - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "dsshow.mak" CFG="dsshow - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "dsshow - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "dsshow - Win32 Debug" (based on "Win32 (x86) Application")
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
RSC=rc.exe
MTL=mktyplib.exe
CPP=cl.exe

!IF  "$(CFG)" == "dsshow - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
OUTDIR=.\Release
INTDIR=.\Release

ALL : "$(OUTDIR)\dsshow.exe"

CLEAN : 
	-@erase "$(INTDIR)\debug.obj"
	-@erase "$(INTDIR)\dsenum.obj"
	-@erase "$(INTDIR)\dsshow.res"
	-@erase "$(INTDIR)\shell.obj"
	-@erase "$(INTDIR)\wassert.obj"
	-@erase "$(INTDIR)\wave.obj"
	-@erase "$(OUTDIR)\dsshow.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
CPP_PROJ=/nologo /ML /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS"\
 /Fp"$(INTDIR)/dsshow.pch" /YX /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\Release/
CPP_SBRS=.\.
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/dsshow.res" /d "NDEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/dsshow.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 dsound.lib winmm.lib comctl32.lib msacm32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
LINK32_FLAGS=dsound.lib winmm.lib comctl32.lib msacm32.lib kernel32.lib\
 user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib\
 ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo\
 /subsystem:windows /incremental:no /pdb:"$(OUTDIR)/dsshow.pdb" /machine:I386\
 /out:"$(OUTDIR)/dsshow.exe" 
LINK32_OBJS= \
	"$(INTDIR)\debug.obj" \
	"$(INTDIR)\dsenum.obj" \
	"$(INTDIR)\dsshow.res" \
	"$(INTDIR)\shell.obj" \
	"$(INTDIR)\wassert.obj" \
	"$(INTDIR)\wave.obj"

"$(OUTDIR)\dsshow.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "dsshow - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
OUTDIR=.\Debug
INTDIR=.\Debug

ALL : "$(OUTDIR)\dsshow.exe"

CLEAN : 
	-@erase "$(INTDIR)\debug.obj"
	-@erase "$(INTDIR)\dsenum.obj"
	-@erase "$(INTDIR)\dsshow.res"
	-@erase "$(INTDIR)\shell.obj"
	-@erase "$(INTDIR)\vc40.idb"
	-@erase "$(INTDIR)\vc40.pdb"
	-@erase "$(INTDIR)\wassert.obj"
	-@erase "$(INTDIR)\wave.obj"
	-@erase "$(OUTDIR)\dsshow.exe"
	-@erase "$(OUTDIR)\dsshow.ilk"
	-@erase "$(OUTDIR)\dsshow.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
CPP_PROJ=/nologo /MLd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /Fp"$(INTDIR)/dsshow.pch" /YX /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\Debug/
CPP_SBRS=.\.
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/dsshow.res" /d "_DEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/dsshow.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386
# ADD LINK32 dsound.lib winmm.lib comctl32.lib msacm32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386
LINK32_FLAGS=dsound.lib winmm.lib comctl32.lib msacm32.lib kernel32.lib\
 user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib\
 ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo\
 /subsystem:windows /incremental:yes /pdb:"$(OUTDIR)/dsshow.pdb" /debug\
 /machine:I386 /out:"$(OUTDIR)/dsshow.exe" 
LINK32_OBJS= \
	"$(INTDIR)\debug.obj" \
	"$(INTDIR)\dsenum.obj" \
	"$(INTDIR)\dsshow.res" \
	"$(INTDIR)\shell.obj" \
	"$(INTDIR)\wassert.obj" \
	"$(INTDIR)\wave.obj"

"$(OUTDIR)\dsshow.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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

# Name "dsshow - Win32 Release"
# Name "dsshow - Win32 Debug"

!IF  "$(CFG)" == "dsshow - Win32 Release"

!ELSEIF  "$(CFG)" == "dsshow - Win32 Debug"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\debug.c
DEP_CPP_DEBUG=\
	".\debug.h"\
	

"$(INTDIR)\debug.obj" : $(SOURCE) $(DEP_CPP_DEBUG) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\debug.h

!IF  "$(CFG)" == "dsshow - Win32 Release"

!ELSEIF  "$(CFG)" == "dsshow - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\dsenum.c
DEP_CPP_DSENU=\
	".\dsenum.h"\
	

"$(INTDIR)\dsenum.obj" : $(SOURCE) $(DEP_CPP_DSENU) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\dsenum.h

!IF  "$(CFG)" == "dsshow - Win32 Release"

!ELSEIF  "$(CFG)" == "dsshow - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\dsshow.rc
DEP_RSC_DSSHO=\
	".\GENERIC.ICO"\
	".\icon1.ico"\
	".\icon2.ico"\
	".\icon3.ico"\
	

"$(INTDIR)\dsshow.res" : $(SOURCE) $(DEP_RSC_DSSHO) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\resource.h

!IF  "$(CFG)" == "dsshow - Win32 Release"

!ELSEIF  "$(CFG)" == "dsshow - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\shell.c
DEP_CPP_SHELL=\
	".\dsenum.h"\
	".\shell.h"\
	".\wassert.h"\
	".\wave.h"\
	

"$(INTDIR)\shell.obj" : $(SOURCE) $(DEP_CPP_SHELL) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\shell.h

!IF  "$(CFG)" == "dsshow - Win32 Release"

!ELSEIF  "$(CFG)" == "dsshow - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\wassert.c
DEP_CPP_WASSE=\
	".\wassert.h"\
	

"$(INTDIR)\wassert.obj" : $(SOURCE) $(DEP_CPP_WASSE) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\wassert.h

!IF  "$(CFG)" == "dsshow - Win32 Release"

!ELSEIF  "$(CFG)" == "dsshow - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\wave.c
DEP_CPP_WAVE_=\
	".\wassert.h"\
	".\wave.h"\
	

"$(INTDIR)\wave.obj" : $(SOURCE) $(DEP_CPP_WAVE_) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\wave.h

!IF  "$(CFG)" == "dsshow - Win32 Release"

!ELSEIF  "$(CFG)" == "dsshow - Win32 Debug"

!ENDIF 

# End Source File
# End Target
# End Project
################################################################################
