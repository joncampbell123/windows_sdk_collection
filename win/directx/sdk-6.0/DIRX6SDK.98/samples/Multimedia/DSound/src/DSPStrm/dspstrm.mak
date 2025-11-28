# Microsoft Developer Studio Generated NMAKE File, Format Version 4.20
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

!IF "$(CFG)" == ""
CFG=dspstrm - Win32 Debug
!MESSAGE No configuration specified.  Defaulting to dspstrm - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "dspstrm - Win32 Release" && "$(CFG)" !=\
 "dspstrm - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "dspstrm.mak" CFG="dspstrm - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "dspstrm - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "dspstrm - Win32 Debug" (based on "Win32 (x86) Application")
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

!IF  "$(CFG)" == "dspstrm - Win32 Release"

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

ALL : "$(OUTDIR)\dspstrm.exe"

CLEAN : 
	-@erase "$(INTDIR)\debug.obj"
	-@erase "$(INTDIR)\dsstream.obj"
	-@erase "$(INTDIR)\dsstream.res"
	-@erase "$(INTDIR)\dstrenum.obj"
	-@erase "$(INTDIR)\dstrtime.obj"
	-@erase "$(INTDIR)\dstrwave.obj"
	-@erase "$(INTDIR)\wassert.obj"
	-@erase "$(OUTDIR)\dspstrm.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
CPP_PROJ=/nologo /ML /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS"\
 /Fp"$(INTDIR)/dspstrm.pch" /YX /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\Release/
CPP_SBRS=.\.
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/dsstream.res" /d "NDEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/dspstrm.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 dsound.lib winmm.lib comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
LINK32_FLAGS=dsound.lib winmm.lib comctl32.lib kernel32.lib user32.lib\
 gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib\
 oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows\
 /incremental:no /pdb:"$(OUTDIR)/dspstrm.pdb" /machine:I386\
 /out:"$(OUTDIR)/dspstrm.exe" 
LINK32_OBJS= \
	"$(INTDIR)\debug.obj" \
	"$(INTDIR)\dsstream.obj" \
	"$(INTDIR)\dsstream.res" \
	"$(INTDIR)\dstrenum.obj" \
	"$(INTDIR)\dstrtime.obj" \
	"$(INTDIR)\dstrwave.obj" \
	"$(INTDIR)\wassert.obj"

"$(OUTDIR)\dspstrm.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "dspstrm - Win32 Debug"

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

ALL : "$(OUTDIR)\dspstrm.exe"

CLEAN : 
	-@erase "$(INTDIR)\debug.obj"
	-@erase "$(INTDIR)\dsstream.obj"
	-@erase "$(INTDIR)\dsstream.res"
	-@erase "$(INTDIR)\dstrenum.obj"
	-@erase "$(INTDIR)\dstrtime.obj"
	-@erase "$(INTDIR)\dstrwave.obj"
	-@erase "$(INTDIR)\vc40.idb"
	-@erase "$(INTDIR)\vc40.pdb"
	-@erase "$(INTDIR)\wassert.obj"
	-@erase "$(OUTDIR)\dspstrm.exe"
	-@erase "$(OUTDIR)\dspstrm.ilk"
	-@erase "$(OUTDIR)\dspstrm.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
CPP_PROJ=/nologo /MLd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /Fp"$(INTDIR)/dspstrm.pch" /YX /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\Debug/
CPP_SBRS=.\.
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/dsstream.res" /d "_DEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/dspstrm.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386
# ADD LINK32 dsound.lib winmm.lib comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386
LINK32_FLAGS=dsound.lib winmm.lib comctl32.lib kernel32.lib user32.lib\
 gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib\
 oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows\
 /incremental:yes /pdb:"$(OUTDIR)/dspstrm.pdb" /debug /machine:I386\
 /out:"$(OUTDIR)/dspstrm.exe" 
LINK32_OBJS= \
	"$(INTDIR)\debug.obj" \
	"$(INTDIR)\dsstream.obj" \
	"$(INTDIR)\dsstream.res" \
	"$(INTDIR)\dstrenum.obj" \
	"$(INTDIR)\dstrtime.obj" \
	"$(INTDIR)\dstrwave.obj" \
	"$(INTDIR)\wassert.obj"

"$(OUTDIR)\dspstrm.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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

# Name "dspstrm - Win32 Release"
# Name "dspstrm - Win32 Debug"

!IF  "$(CFG)" == "dspstrm - Win32 Release"

!ELSEIF  "$(CFG)" == "dspstrm - Win32 Debug"

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

!IF  "$(CFG)" == "dspstrm - Win32 Release"

!ELSEIF  "$(CFG)" == "dspstrm - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\dsstream.c
DEP_CPP_DSSTR=\
	".\debug.h"\
	".\dsstream.h"\
	".\wassert.h"\
	

"$(INTDIR)\dsstream.obj" : $(SOURCE) $(DEP_CPP_DSSTR) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\dsstream.h

!IF  "$(CFG)" == "dspstrm - Win32 Release"

!ELSEIF  "$(CFG)" == "dspstrm - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\dsstream.rc
DEP_RSC_DSSTRE=\
	".\icon2.ico"\
	".\icon3.ico"\
	

"$(INTDIR)\dsstream.res" : $(SOURCE) $(DEP_RSC_DSSTRE) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\dstrenum.c
DEP_CPP_DSTRE=\
	".\debug.h"\
	".\dsstream.h"\
	

"$(INTDIR)\dstrenum.obj" : $(SOURCE) $(DEP_CPP_DSTRE) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\dstrtime.c
DEP_CPP_DSTRT=\
	".\debug.h"\
	".\dsstream.h"\
	

"$(INTDIR)\dstrtime.obj" : $(SOURCE) $(DEP_CPP_DSTRT) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\dstrwave.c
DEP_CPP_DSTRW=\
	".\debug.h"\
	".\dsstream.h"\
	".\wassert.h"\
	

"$(INTDIR)\dstrwave.obj" : $(SOURCE) $(DEP_CPP_DSTRW) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\resource.h

!IF  "$(CFG)" == "dspstrm - Win32 Release"

!ELSEIF  "$(CFG)" == "dspstrm - Win32 Debug"

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

!IF  "$(CFG)" == "dspstrm - Win32 Release"

!ELSEIF  "$(CFG)" == "dspstrm - Win32 Debug"

!ENDIF 

# End Source File
# End Target
# End Project
################################################################################
