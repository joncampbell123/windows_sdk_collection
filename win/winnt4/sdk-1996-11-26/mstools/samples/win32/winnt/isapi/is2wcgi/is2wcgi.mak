# Microsoft Developer Studio Generated NMAKE File, Format Version 4.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

!IF "$(CFG)" == ""
CFG=IS2WCGI - Win32 Debug
!MESSAGE No configuration specified.  Defaulting to IS2WCGI - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "IS2WCGI - Win32 Release" && "$(CFG)" !=\
 "IS2WCGI - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "IS2WCGI.mak" CFG="IS2WCGI - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "IS2WCGI - Win32 Release" (based on\
 "Win32 (x86) Dynamic-Link Library")
!MESSAGE "IS2WCGI - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
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
# PROP Target_Last_Scanned "IS2WCGI - Win32 Debug"
CPP=cl.exe
RSC=rc.exe
MTL=mktyplib.exe

!IF  "$(CFG)" == "IS2WCGI - Win32 Release"

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

ALL : "$(OUTDIR)\IS2WCGI.dll"

CLEAN : 
	-@erase ".\Release\IS2WCGI.dll"
	-@erase ".\Release\keys.obj"
	-@erase ".\Release\IS2WCGI.OBJ"
	-@erase ".\Release\entry.obj"
	-@erase ".\Release\IS2WCGI.lib"
	-@erase ".\Release\IS2WCGI.exp"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
CPP_PROJ=/nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS"\
 /Fp"$(INTDIR)/IS2WCGI.pch" /YX /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\Release/
CPP_SBRS=
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/IS2WCGI.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /entry:"_DllMainCRTStartup@12" /subsystem:windows /dll /machine:I386
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib /nologo /entry:"_DllMainCRTStartup@12" /subsystem:windows /dll\
 /incremental:no /pdb:"$(OUTDIR)/IS2WCGI.pdb" /machine:I386 /def:".\IS2WCGI.DEF"\
 /out:"$(OUTDIR)/IS2WCGI.dll" /implib:"$(OUTDIR)/IS2WCGI.lib" 
DEF_FILE= \
	".\IS2WCGI.DEF"
LINK32_OBJS= \
	".\Release\keys.obj" \
	".\Release\IS2WCGI.OBJ" \
	".\Release\entry.obj"

"$(OUTDIR)\IS2WCGI.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "IS2WCGI - Win32 Debug"

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

ALL : "$(OUTDIR)\IS2WCGI.dll"

CLEAN : 
	-@erase ".\Debug\vc40.pdb"
	-@erase ".\Debug\vc40.idb"
	-@erase ".\Debug\IS2WCGI.dll"
	-@erase ".\Debug\entry.obj"
	-@erase ".\Debug\IS2WCGI.OBJ"
	-@erase ".\Debug\keys.obj"
	-@erase ".\Debug\IS2WCGI.ilk"
	-@erase ".\Debug\IS2WCGI.lib"
	-@erase ".\Debug\IS2WCGI.exp"
	-@erase ".\Debug\IS2WCGI.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MT /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
CPP_PROJ=/nologo /MT /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /Fp"$(INTDIR)/IS2WCGI.pch" /YX /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\Debug/
CPP_SBRS=
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/IS2WCGI.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /entry:"_DllMainCRTStartup@12" /subsystem:windows /dll /debug /machine:I386
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib /nologo /entry:"_DllMainCRTStartup@12" /subsystem:windows /dll\
 /incremental:yes /pdb:"$(OUTDIR)/IS2WCGI.pdb" /debug /machine:I386\
 /def:".\IS2WCGI.DEF" /out:"$(OUTDIR)/IS2WCGI.dll"\
 /implib:"$(OUTDIR)/IS2WCGI.lib" 
DEF_FILE= \
	".\IS2WCGI.DEF"
LINK32_OBJS= \
	".\Debug\entry.obj" \
	".\Debug\IS2WCGI.OBJ" \
	".\Debug\keys.obj"

"$(OUTDIR)\IS2WCGI.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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

# Name "IS2WCGI - Win32 Release"
# Name "IS2WCGI - Win32 Debug"

!IF  "$(CFG)" == "IS2WCGI - Win32 Release"

!ELSEIF  "$(CFG)" == "IS2WCGI - Win32 Debug"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\keys.cpp
DEP_CPP_KEYS_=\
	{$(INCLUDE)}"\httpext.h"\
	".\keys.h"\
	

"$(INTDIR)\keys.obj" : $(SOURCE) $(DEP_CPP_KEYS_) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\IS2WCGI.DEF

!IF  "$(CFG)" == "IS2WCGI - Win32 Release"

!ELSEIF  "$(CFG)" == "IS2WCGI - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\IS2WCGI.CPP
DEP_CPP_IS2WC=\
	{$(INCLUDE)}"\httpext.h"\
	".\keys.h"\
	

"$(INTDIR)\IS2WCGI.OBJ" : $(SOURCE) $(DEP_CPP_IS2WC) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\entry.cpp
DEP_CPP_ENTRY=\
	{$(INCLUDE)}"\httpext.h"\
	

"$(INTDIR)\entry.obj" : $(SOURCE) $(DEP_CPP_ENTRY) "$(INTDIR)"


# End Source File
# End Target
# End Project
################################################################################
