# Microsoft Visual C++ Generated NMAKE File, Format Version 40001
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

!IF "$(CFG)" == ""
CFG=customwz - Win32 Pseudo-Debug
!MESSAGE No configuration specified.  Defaulting to customwz - Win32\
 Pseudo-Debug.
!ENDIF 

!IF "$(CFG)" != "customwz - Win32 Release" && "$(CFG)" !=\
 "customwz - Win32 Pseudo-Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "customwz.mak" CFG="customwz - Win32 Pseudo-Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "customwz - Win32 Release" (based on\
 "Win32 (x86) Dynamic-Link Library")
!MESSAGE "customwz - Win32 Pseudo-Debug" (based on\
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
# PROP Target_Last_Scanned "customwz - Win32 Pseudo-Debug"
RSC=rc.exe
MTL=mktyplib.exe
CPP=cl.exe

!IF  "$(CFG)" == "customwz - Win32 Release"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir Release
# PROP BASE Intermediate_Dir Release
# PROP BASE Target_Dir
# PROP BASE Target_Ext awx
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 0
# PROP Output_Dir Release
# PROP Intermediate_Dir Release
# PROP Target_Dir
# PROP Target_Ext awx
OUTDIR=.\Release
INTDIR=.\Release

ALL : "$(OUTDIR)\custmwz.awx" "$(OUTDIR)\customwz.bsc"\
 "$(MSDevDir)\bin\ide\$(TargetName).awx"

CLEAN : 
	-@erase ".\Release\customwz.bsc"
	-@erase ".\Release\zap.sbr"
	-@erase ".\Release\customwz.pch"
	-@erase ".\Release\chooser.sbr"
	-@erase ".\Release\typedlg.sbr"
	-@erase ".\Release\seqdlg.sbr"
	-@erase ".\Release\paint.sbr"
	-@erase ".\Release\stdafx.sbr"
	-@erase ".\Release\zapdlg.sbr"
	-@erase ".\Release\sampleaw.sbr"
	-@erase ".\Release\options.sbr"
	-@erase ".\Release\customwz.sbr"
	-@erase ".\Release\custmwz.lib"
	-@erase ".\Release\options.obj"
	-@erase ".\Release\customwz.obj"
	-@erase ".\Release\zap.obj"
	-@erase ".\Release\chooser.obj"
	-@erase ".\Release\typedlg.obj"
	-@erase ".\Release\seqdlg.obj"
	-@erase ".\Release\paint.obj"
	-@erase ".\Release\stdafx.obj"
	-@erase ".\Release\zapdlg.obj"
	-@erase ".\Release\sampleaw.obj"
	-@erase ".\Release\customwz.res"
	-@erase ".\Release\custmwz.exp"
	-@erase ".\Release\custmwz.awx"
	-@erase  "$(MSDevDir)\bin\ide\$(TargetName).awx"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

F90=fl32.exe
# ADD BASE F90 /Ox /I "Release/" /c /nologo /MD
# ADD F90 /Ox /I "Release/" /c /nologo /MD
F90_OBJS=.\Release/
F90_PROJ=/Ox /I "Release/" /c /nologo /MD /Fo"Release/" 

.for{$(F90_OBJS)}.obj:
   $(F90) $(F90_PROJ) $<  

.f{$(F90_OBJS)}.obj:
   $(F90) $(F90_PROJ) $<  

.f90{$(F90_OBJS)}.obj:
   $(F90) $(F90_PROJ) $<  

# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /Yu"stdafx.h" /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /Yu"stdafx.h" /c
CPP_PROJ=/nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /FR"$(INTDIR)/" /Fp"$(INTDIR)/customwz.pch" /Yu"stdafx.h"\
 /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\Release/
CPP_SBRS=.\Release/
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/customwz.res" /d "NDEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/customwz.bsc" 
BSC32_SBRS= \
	"$(INTDIR)/zap.sbr" \
	"$(INTDIR)/chooser.sbr" \
	"$(INTDIR)/typedlg.sbr" \
	"$(INTDIR)/seqdlg.sbr" \
	"$(INTDIR)/paint.sbr" \
	"$(INTDIR)/stdafx.sbr" \
	"$(INTDIR)/zapdlg.sbr" \
	"$(INTDIR)/sampleaw.sbr" \
	"$(INTDIR)/options.sbr" \
	"$(INTDIR)/customwz.sbr"

"$(OUTDIR)\customwz.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 /nologo /subsystem:windows /dll /machine:I386 /out:"Release/custmwz.awx"
LINK32_FLAGS=/nologo /subsystem:windows /dll /incremental:no\
 /pdb:"$(OUTDIR)/custmwz.pdb" /machine:I386 /out:"$(OUTDIR)/custmwz.awx"\
 /implib:"$(OUTDIR)/custmwz.lib" 
LINK32_OBJS= \
	"$(INTDIR)/options.obj" \
	"$(INTDIR)/customwz.obj" \
	"$(INTDIR)/zap.obj" \
	"$(INTDIR)/chooser.obj" \
	"$(INTDIR)/typedlg.obj" \
	"$(INTDIR)/seqdlg.obj" \
	"$(INTDIR)/paint.obj" \
	"$(INTDIR)/stdafx.obj" \
	"$(INTDIR)/zapdlg.obj" \
	"$(INTDIR)/sampleaw.obj" \
	"$(INTDIR)/customwz.res"

"$(OUTDIR)\custmwz.awx" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

# ADD BASE  /Ox /I "Release/" /c /nologo /MD
# ADD  /Ox /I "Release/" /c /nologo /MD
# Begin Custom Build - Copying custom AppWizard to Template directory...
OutDir=.\Release
TargetPath=.\Release\custmwz.awx
TargetName=custmwz
InputPath=.\Release\custmwz.awx
SOURCE=$(InputPath)

"$(MSDevDir)\bin\ide\$(TargetName).awx" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   copy "$(TargetPath)" "$(MSDevDir)\bin\ide"
   if exist "$(OutDir)\$(TargetName).pdb" copy "$(OutDir)\$(TargetName).pdb"\
     "$(MSDevDir)\bin\ide"

# End Custom Build

!ELSEIF  "$(CFG)" == "customwz - Win32 Pseudo-Debug"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir Debug
# PROP BASE Intermediate_Dir Debug
# PROP BASE Target_Dir
# PROP BASE Target_Ext awx
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 0
# PROP Output_Dir Debug
# PROP Intermediate_Dir Debug
# PROP Target_Dir
# PROP Target_Ext awx
OUTDIR=.\Debug
INTDIR=.\Debug

ALL : "$(OUTDIR)\custmwz.awx" "$(OUTDIR)\customwz.bsc"\
 "$(MSDevDir)\bin\ide\$(TargetName).awx"

CLEAN : 
	-@erase ".\Debug\vc40.pdb"
	-@erase ".\Debug\customwz.pch"
	-@erase ".\Debug\customwz.bsc"
	-@erase ".\Debug\seqdlg.sbr"
	-@erase ".\Debug\customwz.sbr"
	-@erase ".\Debug\chooser.sbr"
	-@erase ".\Debug\typedlg.sbr"
	-@erase ".\Debug\zapdlg.sbr"
	-@erase ".\Debug\stdafx.sbr"
	-@erase ".\Debug\zap.sbr"
	-@erase ".\Debug\paint.sbr"
	-@erase ".\Debug\sampleaw.sbr"
	-@erase ".\Debug\options.sbr"
	-@erase ".\Debug\custmwz.ilk"
	-@erase ".\Debug\zap.obj"
	-@erase ".\Debug\paint.obj"
	-@erase ".\Debug\sampleaw.obj"
	-@erase ".\Debug\options.obj"
	-@erase ".\Debug\seqdlg.obj"
	-@erase ".\Debug\customwz.obj"
	-@erase ".\Debug\chooser.obj"
	-@erase ".\Debug\typedlg.obj"
	-@erase ".\Debug\zapdlg.obj"
	-@erase ".\Debug\stdafx.obj"
	-@erase ".\Debug\customwz.res"
	-@erase ".\Debug\custmwz.lib"
	-@erase ".\Debug\custmwz.exp"
	-@erase ".\Debug\custmwz.pdb"
	-@erase ".\Debug\custmwz.awx"
	-@erase  "$(MSDevDir)\bin\ide\$(TargetName).awx"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

F90=fl32.exe
# ADD BASE F90 /Ox /I "Debug/" /c /nologo /MD
# ADD F90 /Ox /I "Debug/" /c /nologo /MD
F90_OBJS=.\Debug/
F90_PROJ=/Ox /I "Release/" /c /nologo /MD /Fo"Release/" 

.for{$(F90_OBJS)}.obj:
   $(F90) $(F90_PROJ) $<  

.f{$(F90_OBJS)}.obj:
   $(F90) $(F90_PROJ) $<  

.f90{$(F90_OBJS)}.obj:
   $(F90) $(F90_PROJ) $<  

# ADD BASE CPP /nologo /MD /W3 /Gm /GX /Zi /Od /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_PSEUDO_DEBUG" /D "_AFXDLL" /D "_MBCS" /FR /Yu"stdafx.h" /c
# ADD CPP /nologo /MD /W3 /Gm /GX /Zi /Od /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_PSEUDO_DEBUG" /D "_AFXDLL" /D "_MBCS" /FR /Yu"stdafx.h" /c
CPP_PROJ=/nologo /MD /W3 /Gm /GX /Zi /Od /D "WIN32" /D "NDEBUG" /D "_WINDOWS"\
 /D "_PSEUDO_DEBUG" /D "_AFXDLL" /D "_MBCS" /FR"$(INTDIR)/"\
 /Fp"$(INTDIR)/customwz.pch" /Yu"stdafx.h" /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\Debug/
CPP_SBRS=.\Debug/
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_PSEUDO_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_PSEUDO_DEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/customwz.res" /d "NDEBUG" /d "_PSEUDO_DEBUG" /d\
 "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/customwz.bsc" 
BSC32_SBRS= \
	"$(INTDIR)/seqdlg.sbr" \
	"$(INTDIR)/customwz.sbr" \
	"$(INTDIR)/chooser.sbr" \
	"$(INTDIR)/typedlg.sbr" \
	"$(INTDIR)/zapdlg.sbr" \
	"$(INTDIR)/stdafx.sbr" \
	"$(INTDIR)/zap.sbr" \
	"$(INTDIR)/paint.sbr" \
	"$(INTDIR)/sampleaw.sbr" \
	"$(INTDIR)/options.sbr"

"$(OUTDIR)\customwz.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /incremental:yes /debug /machine:I386
# ADD LINK32 /nologo /subsystem:windows /dll /incremental:yes /debug /machine:I386 /out:"Debug/custmwz.awx"
LINK32_FLAGS=/nologo /subsystem:windows /dll /incremental:yes\
 /pdb:"$(OUTDIR)/custmwz.pdb" /debug /machine:I386 /out:"$(OUTDIR)/custmwz.awx"\
 /implib:"$(OUTDIR)/custmwz.lib" 
LINK32_OBJS= \
	"$(INTDIR)/zap.obj" \
	"$(INTDIR)/paint.obj" \
	"$(INTDIR)/sampleaw.obj" \
	"$(INTDIR)/options.obj" \
	"$(INTDIR)/seqdlg.obj" \
	"$(INTDIR)/customwz.obj" \
	"$(INTDIR)/chooser.obj" \
	"$(INTDIR)/typedlg.obj" \
	"$(INTDIR)/zapdlg.obj" \
	"$(INTDIR)/stdafx.obj" \
	"$(INTDIR)/customwz.res"

"$(OUTDIR)\custmwz.awx" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

# ADD BASE  /Ox /I "Debug/" /c /nologo /MD
# ADD  /Ox /I "Debug/" /c /nologo /MD
# Begin Custom Build - Copying custom AppWizard to Template directory...
OutDir=.\Debug
TargetPath=.\Debug\custmwz.awx
TargetName=custmwz
InputPath=.\Debug\custmwz.awx
SOURCE=$(InputPath)

"$(MSDevDir)\bin\ide\$(TargetName).awx" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   copy "$(TargetPath)" "$(MSDevDir)\bin\ide"
   if exist "$(OutDir)\$(TargetName).pdb" copy "$(OutDir)\$(TargetName).pdb"\
     "$(MSDevDir)\bin\ide"

# End Custom Build

!ENDIF 

MTL_PROJ=/nologo /D "NDEBUG" /win32 

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

# Name "customwz - Win32 Release"
# Name "customwz - Win32 Pseudo-Debug"

!IF  "$(CFG)" == "customwz - Win32 Release"

!ELSEIF  "$(CFG)" == "customwz - Win32 Pseudo-Debug"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\zapdlg.cpp

!IF  "$(CFG)" == "customwz - Win32 Release"

DEP_CPP_ZAPDL=\
	".\stdafx.h"\
	".\customwz.h"\
	".\zapdlg.h"\
	".\sampleaw.h"\
	".\zap.h"\
	".\paint.h"\
	

"$(INTDIR)\zapdlg.obj" : $(SOURCE) $(DEP_CPP_ZAPDL) "$(INTDIR)"\
 "$(INTDIR)\customwz.pch"

"$(INTDIR)\zapdlg.sbr" : $(SOURCE) $(DEP_CPP_ZAPDL) "$(INTDIR)"\
 "$(INTDIR)\customwz.pch"


!ELSEIF  "$(CFG)" == "customwz - Win32 Pseudo-Debug"

DEP_CPP_ZAPDL=\
	".\customwz.h"\
	".\paint.h"\
	".\sampleaw.h"\
	".\zap.h"\
	".\stdafx.h"\
	".\zapdlg.h"\
	

"$(INTDIR)\zapdlg.obj" : $(SOURCE) $(DEP_CPP_ZAPDL) "$(INTDIR)"\
 "$(INTDIR)\customwz.pch"

"$(INTDIR)\zapdlg.sbr" : $(SOURCE) $(DEP_CPP_ZAPDL) "$(INTDIR)"\
 "$(INTDIR)\customwz.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_ZAPDL) "$(INTDIR)"\
 "$(INTDIR)\customwz.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\customwz.cpp

!IF  "$(CFG)" == "customwz - Win32 Release"

DEP_CPP_CUSTO=\
	".\stdafx.h"\
	".\customwz.h"\
	".\chooser.h"\
	".\sampleaw.h"\
	".\zap.h"\
	

"$(INTDIR)\customwz.obj" : $(SOURCE) $(DEP_CPP_CUSTO) "$(INTDIR)"\
 "$(INTDIR)\customwz.pch"

"$(INTDIR)\customwz.sbr" : $(SOURCE) $(DEP_CPP_CUSTO) "$(INTDIR)"\
 "$(INTDIR)\customwz.pch"


!ELSEIF  "$(CFG)" == "customwz - Win32 Pseudo-Debug"

DEP_CPP_CUSTO=\
	".\chooser.h"\
	".\customwz.h"\
	".\sampleaw.h"\
	".\zap.h"\
	".\stdafx.h"\
	

"$(INTDIR)\customwz.obj" : $(SOURCE) $(DEP_CPP_CUSTO) "$(INTDIR)"\
 "$(INTDIR)\customwz.pch"

"$(INTDIR)\customwz.sbr" : $(SOURCE) $(DEP_CPP_CUSTO) "$(INTDIR)"\
 "$(INTDIR)\customwz.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_CUSTO) "$(INTDIR)"\
 "$(INTDIR)\customwz.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\options.cpp

!IF  "$(CFG)" == "customwz - Win32 Release"

DEP_CPP_OPTIO=\
	".\stdafx.h"\
	".\customwz.h"\
	".\sampleaw.h"\
	".\options.h"\
	".\typedlg.h"\
	".\seqdlg.h"\
	".\zap.h"\
	

"$(INTDIR)\options.obj" : $(SOURCE) $(DEP_CPP_OPTIO) "$(INTDIR)"\
 "$(INTDIR)\customwz.pch"

"$(INTDIR)\options.sbr" : $(SOURCE) $(DEP_CPP_OPTIO) "$(INTDIR)"\
 "$(INTDIR)\customwz.pch"


!ELSEIF  "$(CFG)" == "customwz - Win32 Pseudo-Debug"

DEP_CPP_OPTIO=\
	".\customwz.h"\
	".\options.h"\
	".\sampleaw.h"\
	".\zap.h"\
	".\seqdlg.h"\
	".\stdafx.h"\
	".\typedlg.h"\
	

"$(INTDIR)\options.obj" : $(SOURCE) $(DEP_CPP_OPTIO) "$(INTDIR)"\
 "$(INTDIR)\customwz.pch"

"$(INTDIR)\options.sbr" : $(SOURCE) $(DEP_CPP_OPTIO) "$(INTDIR)"\
 "$(INTDIR)\customwz.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_OPTIO) "$(INTDIR)"\
 "$(INTDIR)\customwz.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\paint.cpp

!IF  "$(CFG)" == "customwz - Win32 Release"

DEP_CPP_PAINT=\
	".\stdafx.h"\
	".\customwz.h"\
	

"$(INTDIR)\paint.obj" : $(SOURCE) $(DEP_CPP_PAINT) "$(INTDIR)"\
 "$(INTDIR)\customwz.pch"

"$(INTDIR)\paint.sbr" : $(SOURCE) $(DEP_CPP_PAINT) "$(INTDIR)"\
 "$(INTDIR)\customwz.pch"


!ELSEIF  "$(CFG)" == "customwz - Win32 Pseudo-Debug"

DEP_CPP_PAINT=\
	".\customwz.h"\
	".\stdafx.h"\
	

"$(INTDIR)\paint.obj" : $(SOURCE) $(DEP_CPP_PAINT) "$(INTDIR)"\
 "$(INTDIR)\customwz.pch"

"$(INTDIR)\paint.sbr" : $(SOURCE) $(DEP_CPP_PAINT) "$(INTDIR)"\
 "$(INTDIR)\customwz.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_PAINT) "$(INTDIR)"\
 "$(INTDIR)\customwz.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\sampleaw.cpp

!IF  "$(CFG)" == "customwz - Win32 Release"

DEP_CPP_SAMPL=\
	".\stdafx.h"\
	".\customwz.h"\
	".\sampleaw.h"\
	".\chooser.h"\
	".\zap.h"\
	".\options.h"\
	

"$(INTDIR)\sampleaw.obj" : $(SOURCE) $(DEP_CPP_SAMPL) "$(INTDIR)"\
 "$(INTDIR)\customwz.pch"

"$(INTDIR)\sampleaw.sbr" : $(SOURCE) $(DEP_CPP_SAMPL) "$(INTDIR)"\
 "$(INTDIR)\customwz.pch"


!ELSEIF  "$(CFG)" == "customwz - Win32 Pseudo-Debug"

DEP_CPP_SAMPL=\
	".\chooser.h"\
	".\customwz.h"\
	".\options.h"\
	".\sampleaw.h"\
	".\zap.h"\
	".\stdafx.h"\
	

"$(INTDIR)\sampleaw.obj" : $(SOURCE) $(DEP_CPP_SAMPL) "$(INTDIR)"\
 "$(INTDIR)\customwz.pch"

"$(INTDIR)\sampleaw.sbr" : $(SOURCE) $(DEP_CPP_SAMPL) "$(INTDIR)"\
 "$(INTDIR)\customwz.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_SAMPL) "$(INTDIR)"\
 "$(INTDIR)\customwz.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\seqdlg.cpp

!IF  "$(CFG)" == "customwz - Win32 Release"

DEP_CPP_SEQDL=\
	".\stdafx.h"\
	".\customwz.h"\
	".\seqdlg.h"\
	".\sampleaw.h"\
	".\typedlg.h"\
	".\options.h"\
	".\paint.h"\
	".\zap.h"\
	

"$(INTDIR)\seqdlg.obj" : $(SOURCE) $(DEP_CPP_SEQDL) "$(INTDIR)"\
 "$(INTDIR)\customwz.pch"

"$(INTDIR)\seqdlg.sbr" : $(SOURCE) $(DEP_CPP_SEQDL) "$(INTDIR)"\
 "$(INTDIR)\customwz.pch"


!ELSEIF  "$(CFG)" == "customwz - Win32 Pseudo-Debug"

DEP_CPP_SEQDL=\
	".\customwz.h"\
	".\options.h"\
	".\paint.h"\
	".\sampleaw.h"\
	".\zap.h"\
	".\seqdlg.h"\
	".\stdafx.h"\
	".\typedlg.h"\
	

"$(INTDIR)\seqdlg.obj" : $(SOURCE) $(DEP_CPP_SEQDL) "$(INTDIR)"\
 "$(INTDIR)\customwz.pch"

"$(INTDIR)\seqdlg.sbr" : $(SOURCE) $(DEP_CPP_SEQDL) "$(INTDIR)"\
 "$(INTDIR)\customwz.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_SEQDL) "$(INTDIR)"\
 "$(INTDIR)\customwz.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\stdafx.cpp
DEP_CPP_STDAF=\
	".\stdafx.h"\
	

!IF  "$(CFG)" == "customwz - Win32 Release"

# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /FR"$(INTDIR)/" /Fp"$(INTDIR)/customwz.pch" /Yc"stdafx.h"\
 /Fo"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\stdafx.sbr" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\customwz.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "customwz - Win32 Pseudo-Debug"

# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MD /W3 /Gm /GX /Zi /Od /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_PSEUDO_DEBUG" /D "_AFXDLL" /D "_MBCS" /FR"$(INTDIR)/"\
 /Fp"$(INTDIR)/customwz.pch" /Yc"stdafx.h" /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c\
 $(SOURCE) \
	

"$(INTDIR)\stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\stdafx.sbr" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\customwz.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\typedlg.cpp

!IF  "$(CFG)" == "customwz - Win32 Release"

DEP_CPP_TYPED=\
	".\stdafx.h"\
	".\customwz.h"\
	".\sampleaw.h"\
	".\typedlg.h"\
	".\options.h"\
	".\paint.h"\
	".\zap.h"\
	

"$(INTDIR)\typedlg.obj" : $(SOURCE) $(DEP_CPP_TYPED) "$(INTDIR)"\
 "$(INTDIR)\customwz.pch"

"$(INTDIR)\typedlg.sbr" : $(SOURCE) $(DEP_CPP_TYPED) "$(INTDIR)"\
 "$(INTDIR)\customwz.pch"


!ELSEIF  "$(CFG)" == "customwz - Win32 Pseudo-Debug"

DEP_CPP_TYPED=\
	".\customwz.h"\
	".\options.h"\
	".\paint.h"\
	".\sampleaw.h"\
	".\zap.h"\
	".\stdafx.h"\
	".\typedlg.h"\
	

"$(INTDIR)\typedlg.obj" : $(SOURCE) $(DEP_CPP_TYPED) "$(INTDIR)"\
 "$(INTDIR)\customwz.pch"

"$(INTDIR)\typedlg.sbr" : $(SOURCE) $(DEP_CPP_TYPED) "$(INTDIR)"\
 "$(INTDIR)\customwz.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_TYPED) "$(INTDIR)"\
 "$(INTDIR)\customwz.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\zap.cpp

!IF  "$(CFG)" == "customwz - Win32 Release"

DEP_CPP_ZAP_C=\
	".\stdafx.h"\
	".\customwz.h"\
	".\sampleaw.h"\
	".\zap.h"\
	

"$(INTDIR)\zap.obj" : $(SOURCE) $(DEP_CPP_ZAP_C) "$(INTDIR)"\
 "$(INTDIR)\customwz.pch"

"$(INTDIR)\zap.sbr" : $(SOURCE) $(DEP_CPP_ZAP_C) "$(INTDIR)"\
 "$(INTDIR)\customwz.pch"


!ELSEIF  "$(CFG)" == "customwz - Win32 Pseudo-Debug"

DEP_CPP_ZAP_C=\
	".\customwz.h"\
	".\sampleaw.h"\
	".\zap.h"\
	".\stdafx.h"\
	

"$(INTDIR)\zap.obj" : $(SOURCE) $(DEP_CPP_ZAP_C) "$(INTDIR)"\
 "$(INTDIR)\customwz.pch"

"$(INTDIR)\zap.sbr" : $(SOURCE) $(DEP_CPP_ZAP_C) "$(INTDIR)"\
 "$(INTDIR)\customwz.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_ZAP_C) "$(INTDIR)"\
 "$(INTDIR)\customwz.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\chooser.cpp

!IF  "$(CFG)" == "customwz - Win32 Release"

DEP_CPP_CHOOS=\
	".\stdafx.h"\
	".\customwz.h"\
	".\sampleaw.h"\
	".\chooser.h"\
	".\typedlg.h"\
	".\zapdlg.h"\
	".\seqdlg.h"\
	".\zap.h"\
	

"$(INTDIR)\chooser.obj" : $(SOURCE) $(DEP_CPP_CHOOS) "$(INTDIR)"\
 "$(INTDIR)\customwz.pch"

"$(INTDIR)\chooser.sbr" : $(SOURCE) $(DEP_CPP_CHOOS) "$(INTDIR)"\
 "$(INTDIR)\customwz.pch"


!ELSEIF  "$(CFG)" == "customwz - Win32 Pseudo-Debug"

DEP_CPP_CHOOS=\
	".\chooser.h"\
	".\customwz.h"\
	".\sampleaw.h"\
	".\zap.h"\
	".\seqdlg.h"\
	".\stdafx.h"\
	".\typedlg.h"\
	".\zapdlg.h"\
	

"$(INTDIR)\chooser.obj" : $(SOURCE) $(DEP_CPP_CHOOS) "$(INTDIR)"\
 "$(INTDIR)\customwz.pch"

"$(INTDIR)\chooser.sbr" : $(SOURCE) $(DEP_CPP_CHOOS) "$(INTDIR)"\
 "$(INTDIR)\customwz.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_CHOOS) "$(INTDIR)"\
 "$(INTDIR)\customwz.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\customwz.rc
DEP_RSC_CUSTOM=\
	".\template\newaw.cpp"\
	".\template\newaw.h"\
	".\template\newproj.inf"\
	".\template\resource.h"\
	".\template\root.cpp"\
	".\template\root.h"\
	".\template\root.rc"\
	".\template\root.clw"\
	".\template\stdafx.cpp"\
	".\template\stdafx.h"\
	".\template\newnwprj.inf"\
	".\template\confirm.inf"\
	".\template\chooser.cpp"\
	".\template\chooser.h"\
	".\template\cstmdlg.cpp"\
	".\template\cstmdlg.h"\
	".\template\debug.cpp"\
	".\template\debug.h"\
	".\template\newcnfrm.inf"\
	".\template\root.hpj"\
	".\template\makehelp.bat"\
	".\template\root.rtf"\
	".\template\readme.txt"\
	".\res\apwzsrc.bmp"\
	".\res\dest.bmp"\
	".\res\wand.bmp"\
	".\res\yellowpn.bmp"\
	".\res\projsrc.bmp"\
	".\res\customsr.bmp"\
	".\res\zap.bmp"\
	".\res\botomexe.bmp"\
	".\res\seqtop.bmp"\
	".\res\botomdll.bmp"\
	

"$(INTDIR)\customwz.res" : $(SOURCE) $(DEP_RSC_CUSTOM) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


# End Source File
# End Target
# End Project
################################################################################
