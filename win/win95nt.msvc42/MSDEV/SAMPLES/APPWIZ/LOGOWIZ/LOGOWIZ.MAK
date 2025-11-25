# Microsoft Visual C++ Generated NMAKE File, Format Version 40001
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

!IF "$(CFG)" == ""
CFG=LogoWiz - Win32 Pseudo-Debug
!MESSAGE No configuration specified.  Defaulting to LogoWiz - Win32\
 Pseudo-Debug.
!ENDIF 

!IF "$(CFG)" != "LogoWiz - Win32 Release" && "$(CFG)" !=\
 "LogoWiz - Win32 Pseudo-Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "logowiz.mak" CFG="LogoWiz - Win32 Pseudo-Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "LogoWiz - Win32 Release" (based on\
 "Win32 (x86) Dynamic-Link Library")
!MESSAGE "LogoWiz - Win32 Pseudo-Debug" (based on\
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
# PROP Target_Last_Scanned "LogoWiz - Win32 Pseudo-Debug"
CPP=cl.exe
RSC=rc.exe
MTL=mktyplib.exe
F90=fl32.exe

!IF  "$(CFG)" == "LogoWiz - Win32 Release"

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

ALL : "$(OUTDIR)\logowiz.awx" "$(MSDevDir)\Template\LogoWiz.awx"

CLEAN : 
	-@erase ".\Release\logowiz.lib"
	-@erase ".\Release\logowaw.obj"
	-@erase ".\Release\logowiz.pch"
	-@erase ".\Release\cstm1dlg.obj"
	-@erase ".\Release\chooser.obj"
	-@erase ".\Release\logowiz.obj"
	-@erase ".\Release\stdafx.obj"
	-@erase ".\Release\debug.obj"
	-@erase ".\Release\logowiz.res"
	-@erase ".\Release\logowiz.exp"
	-@erase "$(MSDevDir)\Template\LogoWiz.awx"
	-@erase ".\Release\logowiz.awx"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
CPP_PROJ=/nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/logowiz.pch" /Yu"stdafx.h" /Fo"$(INTDIR)/"\
 /c 
CPP_OBJS=.\Release/
CPP_SBRS=
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/logowiz.res" /d "NDEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/logowiz.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 /nologo /subsystem:windows /dll /machine:I386
LINK32_FLAGS=/nologo /subsystem:windows /dll /incremental:no\
 /pdb:"$(OUTDIR)/logowiz.pdb" /machine:I386 /out:"$(OUTDIR)/logowiz.awx"\
 /implib:"$(OUTDIR)/logowiz.lib" 
LINK32_OBJS= \
	"$(INTDIR)/logowaw.obj" \
	"$(INTDIR)/cstm1dlg.obj" \
	"$(INTDIR)/chooser.obj" \
	"$(INTDIR)/logowiz.obj" \
	"$(INTDIR)/stdafx.obj" \
	"$(INTDIR)/debug.obj" \
	"$(INTDIR)/logowiz.res"

"$(OUTDIR)\logowiz.awx" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

# Begin Custom Build - Copying custom AppWizard to Template directory...
OutDir=.\Release
TargetPath=.\Release\logowiz.awx
TargetName=logowiz
InputPath=.\Release\logowiz.awx
SOURCE=$(InputPath)

"$(MSDevDir)\Template\$(TargetName).awx" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   if not exist $(MSDevDir)\Template\nul md $(MSDevDir)\Template
   copy "$(TargetPath)" "$(MSDevDir)\Template"
   if exist "$(OutDir)\$(TargetName).pdb" copy "$(OutDir)\$(TargetName).pdb"\
    "$(MSDevDir)\Template"

# End Custom Build

!ELSEIF  "$(CFG)" == "LogoWiz - Win32 Pseudo-Debug"

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

ALL : "$(OUTDIR)\logowiz.awx" "$(MSDevDir)\Template\logowiz.awx"

CLEAN : 
	-@erase ".\Debug\vc40.pdb"
	-@erase ".\Debug\logowiz.pch"
	-@erase ".\Debug\logowiz.ilk"
	-@erase ".\Debug\debug.obj"
	-@erase ".\Debug\stdafx.obj"
	-@erase ".\Debug\cstm1dlg.obj"
	-@erase ".\Debug\chooser.obj"
	-@erase ".\Debug\logowiz.obj"
	-@erase ".\Debug\logowaw.obj"
	-@erase ".\Debug\logowiz.res"
	-@erase ".\Debug\logowiz.lib"
	-@erase ".\Debug\logowiz.exp"
	-@erase ".\Debug\logowiz.pdb"
	-@erase "$(MSDevDir)\Template\$(TargetName).awx"
	-@erase ".\Debug\logowiz.awx"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MD /W3 /Gm /GX /Zi /Od /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_PSEUDO_DEBUG" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
# ADD CPP /nologo /MD /W3 /Gm /GX /Zi /Od /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_PSEUDO_DEBUG" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
CPP_PROJ=/nologo /MD /W3 /Gm /GX /Zi /Od /D "WIN32" /D "NDEBUG" /D "_WINDOWS"\
 /D "_PSEUDO_DEBUG" /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/logowiz.pch"\
 /Yu"stdafx.h" /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\Debug/
CPP_SBRS=
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_PSEUDO_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_PSEUDO_DEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/logowiz.res" /d "NDEBUG" /d "_PSEUDO_DEBUG" /d\
 "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/logowiz.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /incremental:yes /debug /machine:I386
# ADD LINK32 /nologo /subsystem:windows /dll /incremental:yes /debug /machine:I386
LINK32_FLAGS=/nologo /subsystem:windows /dll /incremental:yes\
 /pdb:"$(OUTDIR)/logowiz.pdb" /debug /machine:I386 /out:"$(OUTDIR)/logowiz.awx"\
 /implib:"$(OUTDIR)/logowiz.lib" 
LINK32_OBJS= \
	"$(INTDIR)/debug.obj" \
	"$(INTDIR)/stdafx.obj" \
	"$(INTDIR)/cstm1dlg.obj" \
	"$(INTDIR)/chooser.obj" \
	"$(INTDIR)/logowiz.obj" \
	"$(INTDIR)/logowaw.obj" \
	"$(INTDIR)/logowiz.res"

"$(OUTDIR)\logowiz.awx" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

# Begin Custom Build - Copying custom AppWizard to Template directory...
OutDir=.\Debug
TargetPath=.\Debug\logowiz.awx
TargetName=logowiz
InputPath=.\Debug\logowiz.awx
SOURCE=$(InputPath)

"$(MSDevDir)\Template\$(TargetName).awx" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   if not exist $(MSDevDir)\Template\nul md $(MSDevDir)\Template
   copy "$(TargetPath)" "$(MSDevDir)\Template"
   if exist "$(OutDir)\$(TargetName).pdb" copy "$(OutDir)\$(TargetName).pdb"\
    "$(MSDevDir)\Template"

# End Custom Build

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

MTL_PROJ=/nologo /D "NDEBUG" /win32 

################################################################################
# Begin Target

# Name "LogoWiz - Win32 Release"
# Name "LogoWiz - Win32 Pseudo-Debug"

!IF  "$(CFG)" == "LogoWiz - Win32 Release"

!ELSEIF  "$(CFG)" == "LogoWiz - Win32 Pseudo-Debug"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\logowiz.cpp
DEP_CPP_LOGOW=\
	".\logowaw.h"\
	".\logowiz.h"\
	".\stdafx.h"\
	".\debug.h"\
	

!IF  "$(CFG)" == "LogoWiz - Win32 Release"


"$(INTDIR)\logowiz.obj" : $(SOURCE) $(DEP_CPP_LOGOW) "$(INTDIR)"\
 "$(INTDIR)\logowiz.pch"


!ELSEIF  "$(CFG)" == "LogoWiz - Win32 Pseudo-Debug"


"$(INTDIR)\logowiz.obj" : $(SOURCE) $(DEP_CPP_LOGOW) "$(INTDIR)"\
 "$(INTDIR)\logowiz.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_LOGOW) "$(INTDIR)"\
 "$(INTDIR)\logowiz.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\stdafx.cpp
DEP_CPP_STDAF=\
	".\stdafx.h"\
	".\debug.h"\
	

!IF  "$(CFG)" == "LogoWiz - Win32 Release"

# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/logowiz.pch" /Yc"stdafx.h" /Fo"$(INTDIR)/"\
 /c $(SOURCE) \
	

"$(INTDIR)\stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\logowiz.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "LogoWiz - Win32 Pseudo-Debug"

# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MD /W3 /Gm /GX /Zi /Od /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_PSEUDO_DEBUG" /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/logowiz.pch"\
 /Yc"stdafx.h" /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\logowiz.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\debug.cpp
DEP_CPP_DEBUG=\
	".\stdafx.h"\
	".\debug.h"\
	

!IF  "$(CFG)" == "LogoWiz - Win32 Release"


"$(INTDIR)\debug.obj" : $(SOURCE) $(DEP_CPP_DEBUG) "$(INTDIR)"\
 "$(INTDIR)\logowiz.pch"


!ELSEIF  "$(CFG)" == "LogoWiz - Win32 Pseudo-Debug"


"$(INTDIR)\debug.obj" : $(SOURCE) $(DEP_CPP_DEBUG) "$(INTDIR)"\
 "$(INTDIR)\logowiz.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_DEBUG) "$(INTDIR)"\
 "$(INTDIR)\logowiz.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\logowiz.rc
DEP_RSC_LOGOWI=\
	".\TEMPLATE\newproj.inf"\
	".\TEMPLATE\confirm.inf"\
	".\TEMPLATE\ROOT.MAK"\
	".\TEMPLATE\dlgroot.clw"\
	".\TEMPLATE\readme.txt"\
	".\TEMPLATE\dialog.h"\
	".\TEMPLATE\dialog.cpp"\
	".\TEMPLATE\dlgres.h"\
	".\TEMPLATE\dlgall.rc"\
	".\TEMPLATE\DLGLOC.RC"\
	".\TEMPLATE\pressdlg.cpp"\
	".\TEMPLATE\pressdlg.h"\
	".\TEMPLATE\SMILEU.BMP"\
	".\TEMPLATE\SMILED.BMP"\
	".\TEMPLATE\SMILEF.BMP"\
	".\TEMPLATE\FROWNU.BMP"\
	".\TEMPLATE\FROWND.BMP"\
	".\TEMPLATE\FROWNF.BMP"\
	".\res\LogoWiz.ico"\
	".\RES\YELLOWPN.BMP"\
	".\RES\DLGWHLP.BMP"\
	".\RES\DLGNOHLP.BMP"\
	".\RES\SMLOGO.BMP"\
	".\RES\PRESSME.BMP"\
	

"$(INTDIR)\logowiz.res" : $(SOURCE) $(DEP_RSC_LOGOWI) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\readme.txt

!IF  "$(CFG)" == "LogoWiz - Win32 Release"

!ELSEIF  "$(CFG)" == "LogoWiz - Win32 Pseudo-Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\chooser.cpp
DEP_CPP_CHOOS=\
	".\chooser.h"\
	".\cstm1dlg.h"\
	".\logowaw.h"\
	".\logowiz.h"\
	".\stdafx.h"\
	".\debug.h"\
	

!IF  "$(CFG)" == "LogoWiz - Win32 Release"


"$(INTDIR)\chooser.obj" : $(SOURCE) $(DEP_CPP_CHOOS) "$(INTDIR)"\
 "$(INTDIR)\logowiz.pch"


!ELSEIF  "$(CFG)" == "LogoWiz - Win32 Pseudo-Debug"


"$(INTDIR)\chooser.obj" : $(SOURCE) $(DEP_CPP_CHOOS) "$(INTDIR)"\
 "$(INTDIR)\logowiz.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_CHOOS) "$(INTDIR)"\
 "$(INTDIR)\logowiz.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\cstm1dlg.cpp
DEP_CPP_CSTM1=\
	".\cstm1dlg.h"\
	".\logowaw.h"\
	".\logowiz.h"\
	".\stdafx.h"\
	".\debug.h"\
	

!IF  "$(CFG)" == "LogoWiz - Win32 Release"


"$(INTDIR)\cstm1dlg.obj" : $(SOURCE) $(DEP_CPP_CSTM1) "$(INTDIR)"\
 "$(INTDIR)\logowiz.pch"


!ELSEIF  "$(CFG)" == "LogoWiz - Win32 Pseudo-Debug"


"$(INTDIR)\cstm1dlg.obj" : $(SOURCE) $(DEP_CPP_CSTM1) "$(INTDIR)"\
 "$(INTDIR)\logowiz.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_CSTM1) "$(INTDIR)"\
 "$(INTDIR)\logowiz.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\logowaw.cpp
DEP_CPP_LOGOWA=\
	".\chooser.h"\
	".\logowaw.h"\
	".\logowiz.h"\
	".\stdafx.h"\
	".\debug.h"\
	

!IF  "$(CFG)" == "LogoWiz - Win32 Release"


"$(INTDIR)\logowaw.obj" : $(SOURCE) $(DEP_CPP_LOGOWA) "$(INTDIR)"\
 "$(INTDIR)\logowiz.pch"


!ELSEIF  "$(CFG)" == "LogoWiz - Win32 Pseudo-Debug"


"$(INTDIR)\logowaw.obj" : $(SOURCE) $(DEP_CPP_LOGOWA) "$(INTDIR)"\
 "$(INTDIR)\logowiz.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_LOGOWA) "$(INTDIR)"\
 "$(INTDIR)\logowiz.pch"


!ENDIF 

# End Source File
# End Target
# End Project
################################################################################
