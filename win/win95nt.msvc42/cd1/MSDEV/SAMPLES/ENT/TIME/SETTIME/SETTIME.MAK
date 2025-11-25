# Microsoft Developer Studio Generated NMAKE File, Format Version 42001
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

!IF "$(CFG)" == ""
CFG=settime - Win32 Debug
!MESSAGE No configuration specified.  Defaulting to settime - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "settime - Win32 Release" && "$(CFG)" !=\
 "settime - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "settime.mak" CFG="settime - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "settime - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "settime - Win32 Debug" (based on "Win32 (x86) Application")
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
# PROP Target_Last_Scanned "settime - Win32 Release"
CPP=cl.exe
RSC=rc.exe
MTL=mktyplib.exe

!IF  "$(CFG)" == "settime - Win32 Release"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
OUTDIR=.\Release
INTDIR=.\Release

ALL : "$(OUTDIR)\settime.exe"

CLEAN : 
        -@erase "$(INTDIR)\settime.obj"
        -@erase "$(INTDIR)\settime.pch"
        -@erase "$(INTDIR)\settime.res"
        -@erase "$(INTDIR)\StdAfx.obj"
        -@erase "$(INTDIR)\STimeObj.obj"
        -@erase "$(OUTDIR)\settime.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
CPP_PROJ=/nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/settime.pch" /Yu"stdafx.h" /Fo"$(INTDIR)/"\
 /c 
CPP_OBJS=.\Release/
CPP_SBRS=.\.
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/settime.res" /d "NDEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/settime.bsc" 
BSC32_SBRS= \
        
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 /nologo /subsystem:windows /machine:I386
LINK32_FLAGS=/nologo /subsystem:windows /incremental:no\
 /pdb:"$(OUTDIR)/settime.pdb" /machine:I386 /out:"$(OUTDIR)/settime.exe" 
LINK32_OBJS= \
        "$(INTDIR)\settime.obj" \
        "$(INTDIR)\settime.res" \
        "$(INTDIR)\StdAfx.obj" \
        "$(INTDIR)\STimeObj.obj"

"$(OUTDIR)\settime.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "settime - Win32 Debug"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
OUTDIR=.\Debug
INTDIR=.\Debug

ALL : "$(OUTDIR)\settime.exe" ".\settime.tlb"

CLEAN : 
        -@erase "$(INTDIR)\settime.obj"
        -@erase "$(INTDIR)\settime.pch"
        -@erase "$(INTDIR)\settime.res"
        -@erase "$(INTDIR)\StdAfx.obj"
        -@erase "$(INTDIR)\STimeObj.obj"
        -@erase "$(INTDIR)\vc40.idb"
        -@erase "$(INTDIR)\vc40.pdb"
        -@erase "$(OUTDIR)\settime.exe"
        -@erase "$(OUTDIR)\settime.ilk"
        -@erase "$(OUTDIR)\settime.pdb"
        -@erase ".\settime.h"
        -@erase ".\settime.tlb"
        -@erase ".\settime_i.c"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
CPP_PROJ=/nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/settime.pch" /Yu"stdafx.h"\
 /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\Debug/
CPP_SBRS=.\.
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/settime.res" /d "_DEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/settime.bsc" 
BSC32_SBRS= \
        
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386
# ADD LINK32 /nologo /subsystem:windows /debug /machine:I386
LINK32_FLAGS=/nologo /subsystem:windows /incremental:yes\
 /pdb:"$(OUTDIR)/settime.pdb" /debug /machine:I386 /out:"$(OUTDIR)/settime.exe" 
LINK32_OBJS= \
        "$(INTDIR)\settime.obj" \
        "$(INTDIR)\settime.res" \
        "$(INTDIR)\StdAfx.obj" \
        "$(INTDIR)\STimeObj.obj"

"$(OUTDIR)\settime.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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

# Name "settime - Win32 Release"
# Name "settime - Win32 Debug"

!IF  "$(CFG)" == "settime - Win32 Release"

!ELSEIF  "$(CFG)" == "settime - Win32 Debug"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\StdAfx.cpp
DEP_CPP_STDAF=\
        ".\StdAfx.h"\
        {$(INCLUDE)}"\atlbase.h"\
        {$(INCLUDE)}"\atlcom.h"\
        {$(INCLUDE)}"\atlimpl.cpp"\
        

!IF  "$(CFG)" == "settime - Win32 Release"

# ADD CPP /Yc"stdafx.h"

BuildCmds= \
        $(CPP) /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/settime.pch" /Yc"stdafx.h" /Fo"$(INTDIR)/"\
 /c $(SOURCE) \
        

"$(INTDIR)\StdAfx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\settime.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "settime - Win32 Debug"

# ADD CPP /Yc"stdafx.h"

BuildCmds= \
        $(CPP) /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/settime.pch" /Yc"stdafx.h"\
 /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c $(SOURCE) \
        

"$(INTDIR)\StdAfx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\settime.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\settime.cpp
DEP_CPP_SETTI=\
        ".\settime.h"\
        ".\settime_i.c"\
        ".\StdAfx.h"\
        ".\STimeObj.h"\
        {$(INCLUDE)}"\atlbase.h"\
        {$(INCLUDE)}"\atlcom.h"\
        

"$(INTDIR)\settime.obj" : $(SOURCE) $(DEP_CPP_SETTI) "$(INTDIR)"\
 "$(INTDIR)\settime.pch"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\settime.rc

!IF  "$(CFG)" == "settime - Win32 Release"

DEP_RSC_SETTIM=\
        ".\settime.tlb"\
        

"$(INTDIR)\settime.res" : $(SOURCE) $(DEP_RSC_SETTIM) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "settime - Win32 Debug"

NODEP_RSC_SETTIM=\
        ".\Debug\settime.tlb"\
        

"$(INTDIR)\settime.res" : $(SOURCE) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\settime.idl

!IF  "$(CFG)" == "settime - Win32 Release"

!ELSEIF  "$(CFG)" == "settime - Win32 Debug"

# Begin Custom Build - Running MIDL
InputPath=.\settime.idl

BuildCmds= \
        midl settime.idl \
        

"settime.tlb" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"settime.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"settime_i.c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\STimeObj.cpp
DEP_CPP_STIME=\
        ".\settime.h"\
        ".\StdAfx.h"\
        ".\STimeObj.h"\
        {$(INCLUDE)}"\atlbase.h"\
        {$(INCLUDE)}"\atlcom.h"\
        

"$(INTDIR)\STimeObj.obj" : $(SOURCE) $(DEP_CPP_STIME) "$(INTDIR)"\
 "$(INTDIR)\settime.pch"


# End Source File
# End Target
# End Project
################################################################################
