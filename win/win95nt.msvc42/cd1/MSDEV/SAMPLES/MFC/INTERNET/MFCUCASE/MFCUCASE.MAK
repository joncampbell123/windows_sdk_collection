# Microsoft Developer Studio Generated NMAKE File, Format Version 41001
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

!IF "$(CFG)" == ""
CFG=MFCUCASE - Win32 AFXDLL Debug
!MESSAGE No configuration specified.  Defaulting to MFCUCASE - Win32 AFXDLL\
 Debug.
!ENDIF 

!IF "$(CFG)" != "MFCUCASE - Win32 Release" && "$(CFG)" !=\
 "MFCUCASE - Win32 Debug" && "$(CFG)" != "MFCUCASE - Win32 AFXDLL Debug" &&\
 "$(CFG)" != "MFCUCASE - Win32 AFXDLL Release"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "mfcucase.mak" CFG="MFCUCASE - Win32 AFXDLL Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "MFCUCASE - Win32 Release" (based on\
 "Win32 (x86) Dynamic-Link Library")
!MESSAGE "MFCUCASE - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "MFCUCASE - Win32 AFXDLL Debug" (based on\
 "Win32 (x86) Dynamic-Link Library")
!MESSAGE "MFCUCASE - Win32 AFXDLL Release" (based on\
 "Win32 (x86) Dynamic-Link Library")
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
# PROP Target_Last_Scanned "MFCUCASE - Win32 AFXDLL Release"
RSC=rc.exe
MTL=mktyplib.exe
CPP=cl.exe

!IF  "$(CFG)" == "MFCUCASE - Win32 Release"

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

ALL : "$(OUTDIR)\mfcucase.dll"

CLEAN : 
	-@erase "$(INTDIR)\Mfcucase.obj"
	-@erase "$(INTDIR)\Mfcucase.res"
	-@erase "$(OUTDIR)\mfcucase.dll"
	-@erase "$(OUTDIR)\mfcucase.exp"
	-@erase "$(OUTDIR)\mfcucase.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
CPP_PROJ=/nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS"\
 /Fp"$(INTDIR)/mfcucase.pch" /YX /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\Release/
CPP_SBRS=.\.
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/Mfcucase.res" /d "NDEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/mfcucase.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib /nologo /subsystem:windows /dll /incremental:no\
 /pdb:"$(OUTDIR)/mfcucase.pdb" /machine:I386 /def:".\Mfcucase.def"\
 /out:"$(OUTDIR)/mfcucase.dll" /implib:"$(OUTDIR)/mfcucase.lib" 
DEF_FILE= \
	".\Mfcucase.def"
LINK32_OBJS= \
	"$(INTDIR)\Mfcucase.obj" \
	"$(INTDIR)\Mfcucase.res"

"$(OUTDIR)\mfcucase.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "MFCUCASE - Win32 Debug"

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

ALL : "$(OUTDIR)\mfcucase.dll"

CLEAN : 
	-@erase "$(INTDIR)\Mfcucase.obj"
	-@erase "$(INTDIR)\Mfcucase.res"
	-@erase "$(INTDIR)\vc40.idb"
	-@erase "$(INTDIR)\vc40.pdb"
	-@erase "$(OUTDIR)\mfcucase.dll"
	-@erase "$(OUTDIR)\mfcucase.exp"
	-@erase "$(OUTDIR)\mfcucase.ilk"
	-@erase "$(OUTDIR)\mfcucase.lib"
	-@erase "$(OUTDIR)\mfcucase.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
CPP_PROJ=/nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /Fp"$(INTDIR)/mfcucase.pch" /YX /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\Debug/
CPP_SBRS=.\.
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/Mfcucase.res" /d "_DEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/mfcucase.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib /nologo /subsystem:windows /dll /incremental:yes\
 /pdb:"$(OUTDIR)/mfcucase.pdb" /debug /machine:I386 /def:".\Mfcucase.def"\
 /out:"$(OUTDIR)/mfcucase.dll" /implib:"$(OUTDIR)/mfcucase.lib" 
DEF_FILE= \
	".\Mfcucase.def"
LINK32_OBJS= \
	"$(INTDIR)\Mfcucase.obj" \
	"$(INTDIR)\Mfcucase.res"

"$(OUTDIR)\mfcucase.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "MFCUCASE - Win32 AFXDLL Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "MFCUCASE"
# PROP BASE Intermediate_Dir "MFCUCASE"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "DLLDebug"
# PROP Intermediate_Dir "DLLDebug"
# PROP Target_Dir ""
OUTDIR=.\DLLDebug
INTDIR=.\DLLDebug

ALL : "$(OUTDIR)\mfcucase.dll"

CLEAN : 
	-@erase "$(INTDIR)\Mfcucase.obj"
	-@erase "$(INTDIR)\Mfcucase.res"
	-@erase "$(INTDIR)\vc40.idb"
	-@erase "$(INTDIR)\vc40.pdb"
	-@erase "$(OUTDIR)\mfcucase.dll"
	-@erase "$(OUTDIR)\mfcucase.exp"
	-@erase "$(OUTDIR)\mfcucase.ilk"
	-@erase "$(OUTDIR)\mfcucase.lib"
	-@erase "$(OUTDIR)\mfcucase.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_USRDLL" /YX /c
CPP_PROJ=/nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_USRDLL" /Fp"$(INTDIR)/mfcucase.pch"\
 /YX /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\DLLDebug/
CPP_SBRS=.\.
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/Mfcucase.res" /d "_DEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/mfcucase.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386
# ADD LINK32 /nologo /subsystem:windows /dll /debug /machine:I386
LINK32_FLAGS=/nologo /subsystem:windows /dll /incremental:yes\
 /pdb:"$(OUTDIR)/mfcucase.pdb" /debug /machine:I386 /def:".\Mfcucase.def"\
 /out:"$(OUTDIR)/mfcucase.dll" /implib:"$(OUTDIR)/mfcucase.lib" 
DEF_FILE= \
	".\Mfcucase.def"
LINK32_OBJS= \
	"$(INTDIR)\Mfcucase.obj" \
	"$(INTDIR)\Mfcucase.res"

"$(OUTDIR)\mfcucase.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "MFCUCASE - Win32 AFXDLL Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "MFCUCAS0"
# PROP BASE Intermediate_Dir "MFCUCAS0"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "DLLRelll"
# PROP Intermediate_Dir "DLLRel"
# PROP Target_Dir ""
OUTDIR=.\DLLRelll
INTDIR=.\DLLRel

ALL : "$(OUTDIR)\mfcucase.dll"

CLEAN : 
	-@erase "$(INTDIR)\Mfcucase.obj"
	-@erase "$(INTDIR)\Mfcucase.res"
	-@erase "$(OUTDIR)\mfcucase.dll"
	-@erase "$(OUTDIR)\mfcucase.exp"
	-@erase "$(OUTDIR)\mfcucase.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_USRDLL" /YX /c
CPP_PROJ=/nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_USRDLL" /Fp"$(INTDIR)/mfcucase.pch" /YX\
 /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\DLLRel/
CPP_SBRS=.\.
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/Mfcucase.res" /d "NDEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/mfcucase.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 /nologo /subsystem:windows /dll /machine:I386
LINK32_FLAGS=/nologo /subsystem:windows /dll /incremental:no\
 /pdb:"$(OUTDIR)/mfcucase.pdb" /machine:I386 /def:".\Mfcucase.def"\
 /out:"$(OUTDIR)/mfcucase.dll" /implib:"$(OUTDIR)/mfcucase.lib" 
DEF_FILE= \
	".\Mfcucase.def"
LINK32_OBJS= \
	"$(INTDIR)\Mfcucase.obj" \
	"$(INTDIR)\Mfcucase.res"

"$(OUTDIR)\mfcucase.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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

# Name "MFCUCASE - Win32 Release"
# Name "MFCUCASE - Win32 Debug"
# Name "MFCUCASE - Win32 AFXDLL Debug"
# Name "MFCUCASE - Win32 AFXDLL Release"

!IF  "$(CFG)" == "MFCUCASE - Win32 Release"

!ELSEIF  "$(CFG)" == "MFCUCASE - Win32 Debug"

!ELSEIF  "$(CFG)" == "MFCUCASE - Win32 AFXDLL Debug"

!ELSEIF  "$(CFG)" == "MFCUCASE - Win32 AFXDLL Release"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\Mfcucase.cpp
DEP_CPP_MFCUC=\
	".\mfcucase.h"\
	

!IF  "$(CFG)" == "MFCUCASE - Win32 Release"


"$(INTDIR)\Mfcucase.obj" : $(SOURCE) $(DEP_CPP_MFCUC) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "MFCUCASE - Win32 Debug"


"$(INTDIR)\Mfcucase.obj" : $(SOURCE) $(DEP_CPP_MFCUC) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "MFCUCASE - Win32 AFXDLL Debug"


"$(INTDIR)\Mfcucase.obj" : $(SOURCE) $(DEP_CPP_MFCUC) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "MFCUCASE - Win32 AFXDLL Release"


"$(INTDIR)\Mfcucase.obj" : $(SOURCE) $(DEP_CPP_MFCUC) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Mfcucase.def

!IF  "$(CFG)" == "MFCUCASE - Win32 Release"

!ELSEIF  "$(CFG)" == "MFCUCASE - Win32 Debug"

!ELSEIF  "$(CFG)" == "MFCUCASE - Win32 AFXDLL Debug"

!ELSEIF  "$(CFG)" == "MFCUCASE - Win32 AFXDLL Release"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Mfcucase.rc

!IF  "$(CFG)" == "MFCUCASE - Win32 Release"


"$(INTDIR)\Mfcucase.res" : $(SOURCE) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "MFCUCASE - Win32 Debug"


"$(INTDIR)\Mfcucase.res" : $(SOURCE) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "MFCUCASE - Win32 AFXDLL Debug"


"$(INTDIR)\Mfcucase.res" : $(SOURCE) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "MFCUCASE - Win32 AFXDLL Release"


"$(INTDIR)\Mfcucase.res" : $(SOURCE) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ENDIF 

# End Source File
# End Target
# End Project
################################################################################
