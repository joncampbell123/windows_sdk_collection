# Microsoft Developer Studio Generated NMAKE File, Format Version 4.20
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

!IF "$(CFG)" == ""
CFG=dsshow3d - Win32 Debug
!MESSAGE No configuration specified.  Defaulting to dsshow3d - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "dsshow3d - Win32 Release" && "$(CFG)" !=\
 "dsshow3d - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "dsshow3d.mak" CFG="dsshow3d - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "dsshow3d - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "dsshow3d - Win32 Debug" (based on "Win32 (x86) Application")
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
# PROP Target_Last_Scanned "dsshow3d - Win32 Debug"
CPP=cl.exe
RSC=rc.exe
MTL=mktyplib.exe

!IF  "$(CFG)" == "dsshow3d - Win32 Release"

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

ALL : "$(OUTDIR)\dsshow3d.exe"

CLEAN : 
	-@erase "$(INTDIR)\debug.obj"
	-@erase "$(INTDIR)\dsshow3d.obj"
	-@erase "$(INTDIR)\dsshow3d.res"
	-@erase "$(INTDIR)\fileinfo.obj"
	-@erase "$(INTDIR)\finfo3d.obj"
	-@erase "$(INTDIR)\lsnrinfo.obj"
	-@erase "$(INTDIR)\mainwnd.obj"
	-@erase "$(INTDIR)\wave.obj"
	-@erase "$(OUTDIR)\dsshow3d.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
CPP_PROJ=/nologo /ML /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS"\
 /Fp"$(INTDIR)/dsshow3d.pch" /YX /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\Release/
CPP_SBRS=.\.
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/dsshow3d.res" /d "NDEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/dsshow3d.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 comctl32.lib msacm32.lib winmm.lib dsound.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
LINK32_FLAGS=comctl32.lib msacm32.lib winmm.lib dsound.lib kernel32.lib\
 user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib\
 ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo\
 /subsystem:windows /incremental:no /pdb:"$(OUTDIR)/dsshow3d.pdb" /machine:I386\
 /out:"$(OUTDIR)/dsshow3d.exe" 
LINK32_OBJS= \
	"$(INTDIR)\debug.obj" \
	"$(INTDIR)\dsshow3d.obj" \
	"$(INTDIR)\dsshow3d.res" \
	"$(INTDIR)\fileinfo.obj" \
	"$(INTDIR)\finfo3d.obj" \
	"$(INTDIR)\lsnrinfo.obj" \
	"$(INTDIR)\mainwnd.obj" \
	"$(INTDIR)\wave.obj"

"$(OUTDIR)\dsshow3d.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "dsshow3d - Win32 Debug"

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

ALL : "$(OUTDIR)\dsshow3d.exe"

CLEAN : 
	-@erase "$(INTDIR)\debug.obj"
	-@erase "$(INTDIR)\dsshow3d.obj"
	-@erase "$(INTDIR)\dsshow3d.res"
	-@erase "$(INTDIR)\fileinfo.obj"
	-@erase "$(INTDIR)\finfo3d.obj"
	-@erase "$(INTDIR)\lsnrinfo.obj"
	-@erase "$(INTDIR)\mainwnd.obj"
	-@erase "$(INTDIR)\vc40.idb"
	-@erase "$(INTDIR)\vc40.pdb"
	-@erase "$(INTDIR)\wave.obj"
	-@erase "$(OUTDIR)\dsshow3d.exe"
	-@erase "$(OUTDIR)\dsshow3d.ilk"
	-@erase "$(OUTDIR)\dsshow3d.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
CPP_PROJ=/nologo /MLd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /Fp"$(INTDIR)/dsshow3d.pch" /YX /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\Debug/
CPP_SBRS=.\.
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/dsshow3d.res" /d "_DEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/dsshow3d.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386
# ADD LINK32 comctl32.lib msacm32.lib winmm.lib dsound.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386
LINK32_FLAGS=comctl32.lib msacm32.lib winmm.lib dsound.lib kernel32.lib\
 user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib\
 ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo\
 /subsystem:windows /incremental:yes /pdb:"$(OUTDIR)/dsshow3d.pdb" /debug\
 /machine:I386 /out:"$(OUTDIR)/dsshow3d.exe" 
LINK32_OBJS= \
	"$(INTDIR)\debug.obj" \
	"$(INTDIR)\dsshow3d.obj" \
	"$(INTDIR)\dsshow3d.res" \
	"$(INTDIR)\fileinfo.obj" \
	"$(INTDIR)\finfo3d.obj" \
	"$(INTDIR)\lsnrinfo.obj" \
	"$(INTDIR)\mainwnd.obj" \
	"$(INTDIR)\wave.obj"

"$(OUTDIR)\dsshow3d.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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

# Name "dsshow3d - Win32 Release"
# Name "dsshow3d - Win32 Debug"

!IF  "$(CFG)" == "dsshow3d - Win32 Release"

!ELSEIF  "$(CFG)" == "dsshow3d - Win32 Debug"

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

SOURCE=.\dsshow3d.cpp
DEP_CPP_DSSHO=\
	"..\..\..\..\..\INCLUDE\d3dvec.inl"\
	".\dblist.cpp"\
	".\dblist.h"\
	".\debug.h"\
	".\dsshow3d.h"\
	".\fileinfo.h"\
	".\gvars.h"\
	".\mainwnd.h"\
	".\wave.h"\
	{$(INCLUDE)}"\d3dtypes.h"\
	

"$(INTDIR)\dsshow3d.obj" : $(SOURCE) $(DEP_CPP_DSSHO) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\dsshow3d.rc
DEP_RSC_DSSHOW=\
	".\ico00001.ico"\
	".\icon1.ico"\
	".\icon3.ico"\
	".\reshead.h"\
	

"$(INTDIR)\dsshow3d.res" : $(SOURCE) $(DEP_RSC_DSSHOW) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\fileinfo.cpp
DEP_CPP_FILEI=\
	"..\..\..\..\..\INCLUDE\d3dvec.inl"\
	".\dblist.cpp"\
	".\dblist.h"\
	".\debug.h"\
	".\dsshow3d.h"\
	".\fileinfo.h"\
	".\gvars.h"\
	".\mainwnd.h"\
	".\wave.h"\
	{$(INCLUDE)}"\d3dtypes.h"\
	

"$(INTDIR)\fileinfo.obj" : $(SOURCE) $(DEP_CPP_FILEI) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\finfo3d.cpp
DEP_CPP_FINFO=\
	"..\..\..\..\..\INCLUDE\d3dvec.inl"\
	".\dblist.cpp"\
	".\dblist.h"\
	".\debug.h"\
	".\dsshow3d.h"\
	".\fileinfo.h"\
	".\finfo3d.h"\
	".\gvars.h"\
	".\lsnrinfo.h"\
	".\mainwnd.h"\
	".\wave.h"\
	{$(INCLUDE)}"\d3dtypes.h"\
	

"$(INTDIR)\finfo3d.obj" : $(SOURCE) $(DEP_CPP_FINFO) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\lsnrinfo.cpp
DEP_CPP_LSNRI=\
	"..\..\..\..\..\INCLUDE\d3dvec.inl"\
	".\dblist.cpp"\
	".\dblist.h"\
	".\debug.h"\
	".\dsshow3d.h"\
	".\fileinfo.h"\
	".\finfo3d.h"\
	".\gvars.h"\
	".\lsnrinfo.h"\
	".\mainwnd.h"\
	{$(INCLUDE)}"\d3dtypes.h"\
	

"$(INTDIR)\lsnrinfo.obj" : $(SOURCE) $(DEP_CPP_LSNRI) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\mainwnd.cpp
DEP_CPP_MAINW=\
	"..\..\..\..\..\INCLUDE\d3dvec.inl"\
	".\dblist.cpp"\
	".\dblist.h"\
	".\debug.h"\
	".\dsshow3d.h"\
	".\fileinfo.h"\
	".\finfo3d.h"\
	".\gvars.h"\
	".\lsnrinfo.h"\
	".\mainwnd.h"\
	".\wave.h"\
	{$(INCLUDE)}"\d3dtypes.h"\
	

"$(INTDIR)\mainwnd.obj" : $(SOURCE) $(DEP_CPP_MAINW) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\wave.c
DEP_CPP_WAVE_=\
	".\debug.h"\
	".\wave.h"\
	

"$(INTDIR)\wave.obj" : $(SOURCE) $(DEP_CPP_WAVE_) "$(INTDIR)"


# End Source File
# End Target
# End Project
################################################################################
