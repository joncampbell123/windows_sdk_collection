# Microsoft Developer Studio Generated NMAKE File, Format Version 4.20
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

!IF "$(CFG)" == ""
CFG=duel - Win32 Debug
!MESSAGE No configuration specified.  Defaulting to duel - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "duel - Win32 Release" && "$(CFG)" != "duel - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "duel.mak" CFG="duel - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "duel - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "duel - Win32 Debug" (based on "Win32 (x86) Application")
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

!IF  "$(CFG)" == "duel - Win32 Release"

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

ALL : "$(OUTDIR)\duel.exe"

CLEAN : 
	-@erase "$(INTDIR)\comm.obj"
	-@erase "$(INTDIR)\ddutil.obj"
	-@erase "$(INTDIR)\ds3dutil.obj"
	-@erase "$(INTDIR)\duel.obj"
	-@erase "$(INTDIR)\duel.res"
	-@erase "$(INTDIR)\gameproc.obj"
	-@erase "$(INTDIR)\gfx.obj"
	-@erase "$(INTDIR)\input.obj"
	-@erase "$(INTDIR)\lobby.obj"
	-@erase "$(INTDIR)\sfx.obj"
	-@erase "$(INTDIR)\util.obj"
	-@erase "$(INTDIR)\wizard.obj"
	-@erase "$(OUTDIR)\duel.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
CPP_PROJ=/nologo /ML /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS"\
 /Fp"$(INTDIR)/duel.pch" /YX /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\Release/
CPP_SBRS=.\.
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/duel.res" /d "NDEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/duel.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 ddraw.lib dinput.lib dplayx.lib dsound.lib comctl32.lib winmm.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
LINK32_FLAGS=ddraw.lib dinput.lib dplayx.lib dsound.lib comctl32.lib winmm.lib\
 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib\
 ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo\
 /subsystem:windows /incremental:no /pdb:"$(OUTDIR)/duel.pdb" /machine:I386\
 /out:"$(OUTDIR)/duel.exe" 
LINK32_OBJS= \
	"$(INTDIR)\comm.obj" \
	"$(INTDIR)\ddutil.obj" \
	"$(INTDIR)\ds3dutil.obj" \
	"$(INTDIR)\duel.obj" \
	"$(INTDIR)\duel.res" \
	"$(INTDIR)\gameproc.obj" \
	"$(INTDIR)\gfx.obj" \
	"$(INTDIR)\input.obj" \
	"$(INTDIR)\lobby.obj" \
	"$(INTDIR)\sfx.obj" \
	"$(INTDIR)\util.obj" \
	"$(INTDIR)\wizard.obj"

"$(OUTDIR)\duel.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "duel - Win32 Debug"

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

ALL : "$(OUTDIR)\duel.exe"

CLEAN : 
	-@erase "$(INTDIR)\comm.obj"
	-@erase "$(INTDIR)\ddutil.obj"
	-@erase "$(INTDIR)\ds3dutil.obj"
	-@erase "$(INTDIR)\duel.obj"
	-@erase "$(INTDIR)\duel.res"
	-@erase "$(INTDIR)\gameproc.obj"
	-@erase "$(INTDIR)\gfx.obj"
	-@erase "$(INTDIR)\input.obj"
	-@erase "$(INTDIR)\lobby.obj"
	-@erase "$(INTDIR)\sfx.obj"
	-@erase "$(INTDIR)\util.obj"
	-@erase "$(INTDIR)\vc40.idb"
	-@erase "$(INTDIR)\vc40.pdb"
	-@erase "$(INTDIR)\wizard.obj"
	-@erase "$(OUTDIR)\duel.exe"
	-@erase "$(OUTDIR)\duel.ilk"
	-@erase "$(OUTDIR)\duel.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
CPP_PROJ=/nologo /MLd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /Fp"$(INTDIR)/duel.pch" /YX /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\Debug/
CPP_SBRS=.\.
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/duel.res" /d "_DEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/duel.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386
# ADD LINK32 ddraw.lib dinput.lib dplayx.lib dsound.lib comctl32.lib winmm.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386
LINK32_FLAGS=ddraw.lib dinput.lib dplayx.lib dsound.lib comctl32.lib winmm.lib\
 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib\
 ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo\
 /subsystem:windows /incremental:yes /pdb:"$(OUTDIR)/duel.pdb" /debug\
 /machine:I386 /out:"$(OUTDIR)/duel.exe" 
LINK32_OBJS= \
	"$(INTDIR)\comm.obj" \
	"$(INTDIR)\ddutil.obj" \
	"$(INTDIR)\ds3dutil.obj" \
	"$(INTDIR)\duel.obj" \
	"$(INTDIR)\duel.res" \
	"$(INTDIR)\gameproc.obj" \
	"$(INTDIR)\gfx.obj" \
	"$(INTDIR)\input.obj" \
	"$(INTDIR)\lobby.obj" \
	"$(INTDIR)\sfx.obj" \
	"$(INTDIR)\util.obj" \
	"$(INTDIR)\wizard.obj"

"$(OUTDIR)\duel.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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

# Name "duel - Win32 Release"
# Name "duel - Win32 Debug"

!IF  "$(CFG)" == "duel - Win32 Release"

!ELSEIF  "$(CFG)" == "duel - Win32 Debug"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\comm.c
DEP_CPP_COMM_=\
	".\comm.h"\
	".\duel.h"\
	".\lobby.h"\
	{$(INCLUDE)}"\dplobby.h"\
	

"$(INTDIR)\comm.obj" : $(SOURCE) $(DEP_CPP_COMM_) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\comm.h

!IF  "$(CFG)" == "duel - Win32 Release"

!ELSEIF  "$(CFG)" == "duel - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\ddutil.cpp
DEP_CPP_DDUTI=\
	".\ddutil.h"\
	

"$(INTDIR)\ddutil.obj" : $(SOURCE) $(DEP_CPP_DDUTI) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\ddutil.h

!IF  "$(CFG)" == "duel - Win32 Release"

!ELSEIF  "$(CFG)" == "duel - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\ds3dutil.c
DEP_CPP_DS3DU=\
	".\ds3dutil.h"\
	".\duel.h"\
	

"$(INTDIR)\ds3dutil.obj" : $(SOURCE) $(DEP_CPP_DS3DU) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\ds3dutil.h

!IF  "$(CFG)" == "duel - Win32 Release"

!ELSEIF  "$(CFG)" == "duel - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\duel.c
DEP_CPP_DUEL_=\
	".\comm.h"\
	".\ds3dutil.h"\
	".\duel.h"\
	".\gameproc.h"\
	".\gfx.h"\
	".\input.h"\
	".\lobby.h"\
	".\sfx.h"\
	".\wizard.h"\
	{$(INCLUDE)}"\dinput.h"\
	{$(INCLUDE)}"\dplobby.h"\
	

"$(INTDIR)\duel.obj" : $(SOURCE) $(DEP_CPP_DUEL_) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\duel.h

!IF  "$(CFG)" == "duel - Win32 Release"

!ELSEIF  "$(CFG)" == "duel - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\duel.rc
DEP_RSC_DUEL_R=\
	".\Bfire.wav"\
	".\csession.bmp"\
	".\DUEL.BMP"\
	".\duel.ico"\
	".\Lboom.wav"\
	".\osession.bmp"\
	".\player.bmp"\
	".\Sboom.wav"\
	".\Sbounce.wav"\
	".\Sengine.wav"\
	".\SPLASH.BMP"\
	".\Sstart.wav"\
	".\Sstop.wav"\
	

"$(INTDIR)\duel.res" : $(SOURCE) $(DEP_RSC_DUEL_R) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\gameproc.c
DEP_CPP_GAMEP=\
	".\comm.h"\
	".\ds3dutil.h"\
	".\duel.h"\
	".\gameproc.h"\
	".\gfx.h"\
	".\input.h"\
	".\lobby.h"\
	".\sfx.h"\
	".\wizard.h"\
	{$(INCLUDE)}"\dinput.h"\
	{$(INCLUDE)}"\dplobby.h"\
	

"$(INTDIR)\gameproc.obj" : $(SOURCE) $(DEP_CPP_GAMEP) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\gameproc.h

!IF  "$(CFG)" == "duel - Win32 Release"

!ELSEIF  "$(CFG)" == "duel - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\gfx.c
DEP_CPP_GFX_C=\
	".\ddutil.h"\
	".\duel.h"\
	".\gfx.h"\
	".\input.h"\
	{$(INCLUDE)}"\dinput.h"\
	

"$(INTDIR)\gfx.obj" : $(SOURCE) $(DEP_CPP_GFX_C) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\gfx.h

!IF  "$(CFG)" == "duel - Win32 Release"

!ELSEIF  "$(CFG)" == "duel - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\input.c
DEP_CPP_INPUT=\
	".\ds3dutil.h"\
	".\duel.h"\
	".\gameproc.h"\
	".\input.h"\
	".\sfx.h"\
	{$(INCLUDE)}"\dinput.h"\
	

"$(INTDIR)\input.obj" : $(SOURCE) $(DEP_CPP_INPUT) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\input.h

!IF  "$(CFG)" == "duel - Win32 Release"

!ELSEIF  "$(CFG)" == "duel - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\lobby.c
DEP_CPP_LOBBY=\
	".\duel.h"\
	".\lobby.h"\
	{$(INCLUDE)}"\dplobby.h"\
	

"$(INTDIR)\lobby.obj" : $(SOURCE) $(DEP_CPP_LOBBY) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\lobby.h

!IF  "$(CFG)" == "duel - Win32 Release"

!ELSEIF  "$(CFG)" == "duel - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\resource.h

!IF  "$(CFG)" == "duel - Win32 Release"

!ELSEIF  "$(CFG)" == "duel - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\sfx.c
DEP_CPP_SFX_C=\
	".\ds3dutil.h"\
	".\duel.h"\
	".\sfx.h"\
	

"$(INTDIR)\sfx.obj" : $(SOURCE) $(DEP_CPP_SFX_C) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\sfx.h

!IF  "$(CFG)" == "duel - Win32 Release"

!ELSEIF  "$(CFG)" == "duel - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\util.c
DEP_CPP_UTIL_=\
	".\duel.h"\
	

"$(INTDIR)\util.obj" : $(SOURCE) $(DEP_CPP_UTIL_) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\wizard.c
DEP_CPP_WIZAR=\
	".\comm.h"\
	".\ds3dutil.h"\
	".\duel.h"\
	".\gameproc.h"\
	".\lobby.h"\
	".\sfx.h"\
	".\wizard.h"\
	{$(INCLUDE)}"\dplobby.h"\
	

"$(INTDIR)\wizard.obj" : $(SOURCE) $(DEP_CPP_WIZAR) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\wizard.h

!IF  "$(CFG)" == "duel - Win32 Release"

!ELSEIF  "$(CFG)" == "duel - Win32 Debug"

!ENDIF 

# End Source File
# End Target
# End Project
################################################################################
