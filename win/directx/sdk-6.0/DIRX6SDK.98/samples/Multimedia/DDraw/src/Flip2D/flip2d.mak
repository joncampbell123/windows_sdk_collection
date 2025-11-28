# Microsoft Developer Studio Generated NMAKE File, Format Version 4.20
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

!IF "$(CFG)" == ""
CFG=flip2d - Win32 Debug
!MESSAGE No configuration specified.  Defaulting to flip2d - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "flip2d - Win32 Release" && "$(CFG)" != "flip2d - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "flip2d.mak" CFG="flip2d - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "flip2d - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "flip2d - Win32 Debug" (based on "Win32 (x86) Application")
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

!IF  "$(CFG)" == "flip2d - Win32 Release"

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

ALL : "$(OUTDIR)\flip2d.exe"

CLEAN : 
	-@erase "$(INTDIR)\dumb3d.obj"
	-@erase "$(INTDIR)\flipcube.obj"
	-@erase "$(INTDIR)\flipcube.res"
	-@erase "$(INTDIR)\tri.obj"
	-@erase "$(OUTDIR)\flip2d.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
CPP_PROJ=/nologo /ML /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS"\
 /Fp"$(INTDIR)/flip2d.pch" /YX /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\Release/
CPP_SBRS=.\.
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/flipcube.res" /d "NDEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/flip2d.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 ddraw.lib dinput.lib winmm.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
LINK32_FLAGS=ddraw.lib dinput.lib winmm.lib kernel32.lib user32.lib gdi32.lib\
 winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib\
 uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /incremental:no\
 /pdb:"$(OUTDIR)/flip2d.pdb" /machine:I386 /out:"$(OUTDIR)/flip2d.exe" 
LINK32_OBJS= \
	"$(INTDIR)\dumb3d.obj" \
	"$(INTDIR)\flipcube.obj" \
	"$(INTDIR)\flipcube.res" \
	"$(INTDIR)\tri.obj"

"$(OUTDIR)\flip2d.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "flip2d - Win32 Debug"

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

ALL : "$(OUTDIR)\flip2d.exe"

CLEAN : 
	-@erase "$(INTDIR)\dumb3d.obj"
	-@erase "$(INTDIR)\flipcube.obj"
	-@erase "$(INTDIR)\flipcube.res"
	-@erase "$(INTDIR)\tri.obj"
	-@erase "$(INTDIR)\vc40.idb"
	-@erase "$(INTDIR)\vc40.pdb"
	-@erase "$(OUTDIR)\flip2d.exe"
	-@erase "$(OUTDIR)\flip2d.ilk"
	-@erase "$(OUTDIR)\flip2d.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
CPP_PROJ=/nologo /MLd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /Fp"$(INTDIR)/flip2d.pch" /YX /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\Debug/
CPP_SBRS=.\.
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/flipcube.res" /d "_DEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/flip2d.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386
# ADD LINK32 ddraw.lib dinput.lib winmm.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386
LINK32_FLAGS=ddraw.lib dinput.lib winmm.lib kernel32.lib user32.lib gdi32.lib\
 winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib\
 uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /incremental:yes\
 /pdb:"$(OUTDIR)/flip2d.pdb" /debug /machine:I386 /out:"$(OUTDIR)/flip2d.exe" 
LINK32_OBJS= \
	"$(INTDIR)\dumb3d.obj" \
	"$(INTDIR)\flipcube.obj" \
	"$(INTDIR)\flipcube.res" \
	"$(INTDIR)\tri.obj"

"$(OUTDIR)\flip2d.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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

# Name "flip2d - Win32 Release"
# Name "flip2d - Win32 Debug"

!IF  "$(CFG)" == "flip2d - Win32 Release"

!ELSEIF  "$(CFG)" == "flip2d - Win32 Debug"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\dumb3d.cpp
DEP_CPP_DUMB3=\
	".\dumb3d.h"\
	

"$(INTDIR)\dumb3d.obj" : $(SOURCE) $(DEP_CPP_DUMB3) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\dumb3d.h

!IF  "$(CFG)" == "flip2d - Win32 Release"

!ELSEIF  "$(CFG)" == "flip2d - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\fixed.h

!IF  "$(CFG)" == "flip2d - Win32 Release"

!ELSEIF  "$(CFG)" == "flip2d - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\flipcube.cpp
DEP_CPP_FLIPC=\
	".\dumb3d.h"\
	".\flipcube.h"\
	{$(INCLUDE)}"\dinput.h"\
	

"$(INTDIR)\flipcube.obj" : $(SOURCE) $(DEP_CPP_FLIPC) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\flipcube.h

!IF  "$(CFG)" == "flip2d - Win32 Release"

!ELSEIF  "$(CFG)" == "flip2d - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\flipcube.rc
DEP_RSC_FLIPCU=\
	".\flipcube.h"\
	".\flipcube.ico"\
	

"$(INTDIR)\flipcube.res" : $(SOURCE) $(DEP_RSC_FLIPCU) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\tri.cpp
DEP_CPP_TRI_C=\
	".\fixed.h"\
	

"$(INTDIR)\tri.obj" : $(SOURCE) $(DEP_CPP_TRI_C) "$(INTDIR)"


# End Source File
# End Target
# End Project
################################################################################
