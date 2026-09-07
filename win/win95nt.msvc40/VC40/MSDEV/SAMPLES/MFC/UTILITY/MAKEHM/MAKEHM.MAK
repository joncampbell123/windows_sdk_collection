# Microsoft Visual C++ Generated NMAKE File, Format Version 40000
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

!IF "$(CFG)" == ""
CFG=MakeHm - Win32 Debug
!MESSAGE No configuration specified.  Defaulting to MakeHm - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "MakeHm - Win32 Release" && "$(CFG)" != "MakeHm - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "MakeHm.mak" CFG="MakeHm - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "MakeHm - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "MakeHm - Win32 Debug" (based on "Win32 (x86) Console Application")
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
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "MakeHm - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir Release
# PROP BASE Intermediate_Dir Release
# PROP BASE Target_Dir
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 0
# PROP Output_Dir Release
# PROP Intermediate_Dir Release
# PROP Target_Dir
OUTDIR=.\Release
INTDIR=.\Release

ALL : "$(OUTDIR)\MakeHm.exe" "$(OUTDIR)\MakeHm.bsc"

CLEAN : 
	-@erase ".\Release\MakeHm.exe"
	-@erase ".\Release\Makehm.obj"
	-@erase ".\Release\MakeHm.idb"
	-@erase ".\Release\MakeHm.bsc"
	-@erase ".\Release\Makehm.sbr"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /YX /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_AFXDLL" /D "_MBCS" /FR /YX /c
CPP_PROJ=/nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D\
 "_AFXDLL" /D "_MBCS" /FR"$(INTDIR)/" /Fp"$(INTDIR)/MakeHm.pch" /YX\
 /Fo"$(INTDIR)/" /Fd"$(OUTDIR)/MakeHm.pdb" /c 
CPP_OBJS=.\Release/
CPP_SBRS=.\Release/
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/MakeHm.bsc" 
BSC32_SBRS= \
	"$(INTDIR)/Makehm.sbr"

"$(OUTDIR)\MakeHm.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 /nologo /subsystem:console /machine:I386
LINK32_FLAGS=/nologo /subsystem:console /incremental:no\
 /pdb:"$(OUTDIR)/MakeHm.pdb" /machine:I386 /out:"$(OUTDIR)/MakeHm.exe" 
LINK32_OBJS= \
	"$(INTDIR)/Makehm.obj"

"$(OUTDIR)\MakeHm.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "MakeHm - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir Debug
# PROP BASE Intermediate_Dir Debug
# PROP BASE Target_Dir
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 1
# PROP Output_Dir Debug
# PROP Intermediate_Dir Debug
# PROP Target_Dir
OUTDIR=.\Debug
INTDIR=.\Debug

ALL : "$(OUTDIR)\MakeHm.exe" "$(OUTDIR)\MakeHm.bsc"

CLEAN : 
	-@erase ".\Debug\MakeHm.exe"
	-@erase ".\Debug\Makehm.obj"
	-@erase ".\Debug\MakeHm.ilk"
	-@erase ".\Debug\MakeHm.pdb"
	-@erase ".\Debug\MakeHm.idb"
	-@erase ".\Debug\MakeHm.bsc"
	-@erase ".\Debug\Makehm.sbr"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /YX /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_AFXDLL" /D "_MBCS" /FR /YX /c
CPP_PROJ=/nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE"\
 /D "_AFXDLL" /D "_MBCS" /FR"$(INTDIR)/" /Fp"$(INTDIR)/MakeHm.pch" /YX\
 /Fo"$(INTDIR)/" /Fd"$(OUTDIR)/MakeHm.pdb" /c 
CPP_OBJS=.\Debug/
CPP_SBRS=.\Debug/
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/MakeHm.bsc" 
BSC32_SBRS= \
	"$(INTDIR)/Makehm.sbr"

"$(OUTDIR)\MakeHm.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386
# ADD LINK32 /nologo /subsystem:console /debug /machine:I386
LINK32_FLAGS=/nologo /subsystem:console /incremental:yes\
 /pdb:"$(OUTDIR)/MakeHm.pdb" /debug /machine:I386 /out:"$(OUTDIR)/MakeHm.exe" 
LINK32_OBJS= \
	"$(INTDIR)/Makehm.obj"

"$(OUTDIR)\MakeHm.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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

# Name "MakeHm - Win32 Release"
# Name "MakeHm - Win32 Debug"

!IF  "$(CFG)" == "MakeHm - Win32 Release"

!ELSEIF  "$(CFG)" == "MakeHm - Win32 Debug"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\Makehm.cpp

!IF  "$(CFG)" == "MakeHm - Win32 Release"


"$(INTDIR)\Makehm.obj" : $(SOURCE) "$(INTDIR)"

"$(INTDIR)\Makehm.sbr" : $(SOURCE) "$(INTDIR)"

"$(INTDIR)\MakeHm.idb" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "MakeHm - Win32 Debug"


"$(INTDIR)\Makehm.obj" : $(SOURCE) "$(INTDIR)"

"$(INTDIR)\Makehm.sbr" : $(SOURCE) "$(INTDIR)"

"$(INTDIR)\MakeHm.pdb" : $(SOURCE) "$(INTDIR)"

"$(INTDIR)\MakeHm.idb" : $(SOURCE) "$(INTDIR)"


!ENDIF 

# End Source File
# End Target
# End Project
################################################################################
