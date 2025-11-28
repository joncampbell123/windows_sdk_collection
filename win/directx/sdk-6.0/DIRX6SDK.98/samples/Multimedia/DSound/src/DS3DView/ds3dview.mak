# Microsoft Developer Studio Generated NMAKE File, Format Version 4.20
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

!IF "$(CFG)" == ""
CFG=ds3dview - Win32 Debug
!MESSAGE No configuration specified.  Defaulting to ds3dview - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "ds3dview - Win32 Release" && "$(CFG)" !=\
 "ds3dview - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "ds3dview.mak" CFG="ds3dview - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "ds3dview - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "ds3dview - Win32 Debug" (based on "Win32 (x86) Application")
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
# PROP Target_Last_Scanned "ds3dview - Win32 Debug"
CPP=cl.exe
RSC=rc.exe
MTL=mktyplib.exe

!IF  "$(CFG)" == "ds3dview - Win32 Release"

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

ALL : "$(OUTDIR)\ds3dview.exe"

CLEAN : 
	-@erase "$(INTDIR)\dsutil3d.obj"
	-@erase "$(INTDIR)\file.obj"
	-@erase "$(INTDIR)\rlds3d.obj"
	-@erase "$(INTDIR)\rodcone.obj"
	-@erase "$(INTDIR)\viewer.obj"
	-@erase "$(INTDIR)\viewer.res"
	-@erase "$(OUTDIR)\ds3dview.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D CDECL=__cdecl /YX /c
CPP_PROJ=/nologo /ML /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D\
 CDECL=__cdecl /Fp"$(INTDIR)/ds3dview.pch" /YX /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\Release/
CPP_SBRS=.\.
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/viewer.res" /d "NDEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/ds3dview.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 winmm.lib d3drm.lib ddraw.lib dsound.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
LINK32_FLAGS=winmm.lib d3drm.lib ddraw.lib dsound.lib kernel32.lib user32.lib\
 gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib\
 oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows\
 /incremental:no /pdb:"$(OUTDIR)/ds3dview.pdb" /machine:I386\
 /out:"$(OUTDIR)/ds3dview.exe" 
LINK32_OBJS= \
	"$(INTDIR)\dsutil3d.obj" \
	"$(INTDIR)\file.obj" \
	"$(INTDIR)\rlds3d.obj" \
	"$(INTDIR)\rodcone.obj" \
	"$(INTDIR)\viewer.obj" \
	"$(INTDIR)\viewer.res"

"$(OUTDIR)\ds3dview.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "ds3dview - Win32 Debug"

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

ALL : "$(OUTDIR)\ds3dview.exe"

CLEAN : 
	-@erase "$(INTDIR)\dsutil3d.obj"
	-@erase "$(INTDIR)\file.obj"
	-@erase "$(INTDIR)\rlds3d.obj"
	-@erase "$(INTDIR)\rodcone.obj"
	-@erase "$(INTDIR)\vc40.idb"
	-@erase "$(INTDIR)\vc40.pdb"
	-@erase "$(INTDIR)\viewer.obj"
	-@erase "$(INTDIR)\viewer.res"
	-@erase "$(OUTDIR)\ds3dview.exe"
	-@erase "$(OUTDIR)\ds3dview.ilk"
	-@erase "$(OUTDIR)\ds3dview.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D CDECL=__cdecl /YX /c
CPP_PROJ=/nologo /MLd /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS"\
 /D CDECL=__cdecl /Fp"$(INTDIR)/ds3dview.pch" /YX /Fo"$(INTDIR)/"\
 /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\Debug/
CPP_SBRS=.\.
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/viewer.res" /d "_DEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/ds3dview.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386
# ADD LINK32 winmm.lib d3drm.lib ddraw.lib dsound.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386
LINK32_FLAGS=winmm.lib d3drm.lib ddraw.lib dsound.lib kernel32.lib user32.lib\
 gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib\
 oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows\
 /incremental:yes /pdb:"$(OUTDIR)/ds3dview.pdb" /debug /machine:I386\
 /out:"$(OUTDIR)/ds3dview.exe" 
LINK32_OBJS= \
	"$(INTDIR)\dsutil3d.obj" \
	"$(INTDIR)\file.obj" \
	"$(INTDIR)\rlds3d.obj" \
	"$(INTDIR)\rodcone.obj" \
	"$(INTDIR)\viewer.obj" \
	"$(INTDIR)\viewer.res"

"$(OUTDIR)\ds3dview.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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

# Name "ds3dview - Win32 Release"
# Name "ds3dview - Win32 Debug"

!IF  "$(CFG)" == "ds3dview - Win32 Release"

!ELSEIF  "$(CFG)" == "ds3dview - Win32 Debug"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\dsutil3d.c
DEP_CPP_DSUTI=\
	".\dsutil3d.h"\
	

"$(INTDIR)\dsutil3d.obj" : $(SOURCE) $(DEP_CPP_DSUTI) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\file.cpp
DEP_CPP_FILE_=\
	"..\..\..\..\..\INCLUDE\d3d.h"\
	"..\..\..\..\..\INCLUDE\d3dcaps.h"\
	"..\..\..\..\..\INCLUDE\d3drm.h"\
	"..\..\..\..\..\INCLUDE\d3drmdef.h"\
	"..\..\..\..\..\INCLUDE\d3drmobj.h"\
	"..\..\..\..\..\INCLUDE\d3dtypes.h"\
	"..\..\..\..\..\INCLUDE\d3dvec.inl"\
	{$(INCLUDE)}"\d3drmwin.h"\
	

"$(INTDIR)\file.obj" : $(SOURCE) $(DEP_CPP_FILE_) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\rlds3d.cpp
DEP_CPP_RLDS3=\
	"..\..\..\..\..\INCLUDE\d3d.h"\
	"..\..\..\..\..\INCLUDE\d3dcaps.h"\
	"..\..\..\..\..\INCLUDE\d3drm.h"\
	"..\..\..\..\..\INCLUDE\d3drmdef.h"\
	"..\..\..\..\..\INCLUDE\d3drmobj.h"\
	"..\..\..\..\..\INCLUDE\d3dtypes.h"\
	"..\..\..\..\..\INCLUDE\d3dvec.inl"\
	".\ds3dvi.h"\
	".\dsutil3d.h"\
	".\file.h"\
	".\rlds3d.h"\
	".\rodcone.h"\
	{$(INCLUDE)}"\d3drmwin.h"\
	

"$(INTDIR)\rlds3d.obj" : $(SOURCE) $(DEP_CPP_RLDS3) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\rodcone.cpp
DEP_CPP_RODCO=\
	"..\..\..\..\..\INCLUDE\d3d.h"\
	"..\..\..\..\..\INCLUDE\d3dcaps.h"\
	"..\..\..\..\..\INCLUDE\d3drm.h"\
	"..\..\..\..\..\INCLUDE\d3drmdef.h"\
	"..\..\..\..\..\INCLUDE\d3drmobj.h"\
	"..\..\..\..\..\INCLUDE\d3dtypes.h"\
	"..\..\..\..\..\INCLUDE\d3dvec.inl"\
	".\rodcone.h"\
	{$(INCLUDE)}"\d3drmwin.h"\
	

"$(INTDIR)\rodcone.obj" : $(SOURCE) $(DEP_CPP_RODCO) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\viewer.cpp
DEP_CPP_VIEWE=\
	"..\..\..\..\..\INCLUDE\d3d.h"\
	"..\..\..\..\..\INCLUDE\d3dcaps.h"\
	"..\..\..\..\..\INCLUDE\d3drm.h"\
	"..\..\..\..\..\INCLUDE\d3drmdef.h"\
	"..\..\..\..\..\INCLUDE\d3drmobj.h"\
	"..\..\..\..\..\INCLUDE\d3dtypes.h"\
	"..\..\..\..\..\INCLUDE\d3dvec.inl"\
	".\file.h"\
	".\rlds3d.h"\
	{$(INCLUDE)}"\d3drmwin.h"\
	

"$(INTDIR)\viewer.obj" : $(SOURCE) $(DEP_CPP_VIEWE) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\viewer.rc

!IF  "$(CFG)" == "ds3dview - Win32 Release"

DEP_RSC_VIEWER=\
	".\viewer.ico"\
	
NODEP_RSC_VIEWER=\
	".\Release\__cardoorclose1.wav"\
	".\Release\__cardoorclose2.wav"\
	".\Release\__cardooropen1.wav"\
	".\Release\__cardooropen2.wav"\
	".\Release\__geek.wav"\
	".\Release\__guncock1.wav"\
	".\Release\__guncock2.wav"\
	".\Release\__guncock3.wav"\
	".\Release\__helicopter.wav"\
	".\Release\__monster.wav"\
	".\Release\__police.wav"\
	".\Release\__scream1.wav"\
	".\Release\__scream2.wav"\
	".\Release\__scream3.wav"\
	".\Release\__siren.wav"\
	".\Release\__siren2.wav"\
	".\Release\__skid1.wav"\
	".\Release\__skid2.wav"\
	".\Release\sphere2.x"\
	

"$(INTDIR)\viewer.res" : $(SOURCE) $(DEP_RSC_VIEWER) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "ds3dview - Win32 Debug"

DEP_RSC_VIEWER=\
	".\viewer.ico"\
	
NODEP_RSC_VIEWER=\
	".\Debug\__cardoorclose1.wav"\
	".\Debug\__cardoorclose2.wav"\
	".\Debug\__cardooropen1.wav"\
	".\Debug\__cardooropen2.wav"\
	".\Debug\__geek.wav"\
	".\Debug\__guncock1.wav"\
	".\Debug\__guncock2.wav"\
	".\Debug\__guncock3.wav"\
	".\Debug\__helicopter.wav"\
	".\Debug\__monster.wav"\
	".\Debug\__police.wav"\
	".\Debug\__scream1.wav"\
	".\Debug\__scream2.wav"\
	".\Debug\__scream3.wav"\
	".\Debug\__siren.wav"\
	".\Debug\__siren2.wav"\
	".\Debug\__skid1.wav"\
	".\Debug\__skid2.wav"\
	".\Debug\sphere2.x"\
	

"$(INTDIR)\viewer.res" : $(SOURCE) $(DEP_RSC_VIEWER) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ENDIF 

# End Source File
# End Target
# End Project
################################################################################
