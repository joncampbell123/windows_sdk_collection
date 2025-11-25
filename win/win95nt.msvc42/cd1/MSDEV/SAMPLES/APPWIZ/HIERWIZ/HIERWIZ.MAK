# Microsoft Visual C++ Generated NMAKE File, Format Version 40001
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

!IF "$(CFG)" == ""
CFG=hierwiz - Win32 Pseudo-Debug
!MESSAGE No configuration specified.  Defaulting to hierwiz - Win32\
 Pseudo-Debug.
!ENDIF 

!IF "$(CFG)" != "hierwiz - Win32 Release" && "$(CFG)" !=\
 "hierwiz - Win32 Pseudo-Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "hierwiz.mak" CFG="hierwiz - Win32 Pseudo-Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "hierwiz - Win32 Release" (based on\
 "Win32 (x86) Dynamic-Link Library")
!MESSAGE "hierwiz - Win32 Pseudo-Debug" (based on\
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
# PROP Target_Last_Scanned "hierwiz - Win32 Pseudo-Debug"
RSC=rc.exe
CPP=cl.exe
MTL=mktyplib.exe

!IF  "$(CFG)" == "hierwiz - Win32 Release"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir WinRel
# PROP BASE Intermediate_Dir WinRel
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 0
# PROP Output_Dir Release
# PROP Intermediate_Dir Release
OUTDIR=.\Release
INTDIR=.\Release

ALL : "$(OUTDIR)\hierwiz.awx" "$(OUTDIR)\hierwiz.bsc"\
 "$(MSDevDir)\Template\$(TargetName).awx"

CLEAN : 
	-@erase ".\Release\hierwiz.bsc"
	-@erase ".\Release\chooser.sbr"
	-@erase ".\Release\hierwiz.pch"
	-@erase ".\Release\stdafx.sbr"
	-@erase ".\Release\cstm1dlg.sbr"
	-@erase ".\Release\hierwiz.sbr"
	-@erase ".\Release\editdlg.sbr"
	-@erase ".\Release\hierwaw.sbr"
	-@erase ".\Release\usreditc.sbr"
	-@erase ".\Release\debug.sbr"
	-@erase ".\Release\loadfile.sbr"
	-@erase ".\Release\hierwiz.lib"
	-@erase ".\Release\loadfile.obj"
	-@erase ".\Release\chooser.obj"
	-@erase ".\Release\stdafx.obj"
	-@erase ".\Release\cstm1dlg.obj"
	-@erase ".\Release\hierwiz.obj"
	-@erase ".\Release\editdlg.obj"
	-@erase ".\Release\hierwaw.obj"
	-@erase ".\Release\usreditc.obj"
	-@erase ".\Release\debug.obj"
	-@erase ".\Release\hierwiz.res"
	-@erase ".\Release\hierwiz.exp"
	-@erase ".\Release\hierwiz.awx"
	-@erase  "$(MSDevDir)\Template\$(TargetName).awx"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /Yu"stdafx.h" /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /Yu"stdafx.h" /c
CPP_PROJ=/nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /FR"$(INTDIR)/" /Fp"$(INTDIR)/hierwiz.pch" /Yu"stdafx.h"\
 /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\Release/
CPP_SBRS=.\Release/
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/hierwiz.res" /d "NDEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/hierwiz.bsc" 
BSC32_SBRS= \
	"$(INTDIR)/chooser.sbr" \
	"$(INTDIR)/stdafx.sbr" \
	"$(INTDIR)/cstm1dlg.sbr" \
	"$(INTDIR)/hierwiz.sbr" \
	"$(INTDIR)/editdlg.sbr" \
	"$(INTDIR)/hierwaw.sbr" \
	"$(INTDIR)/usreditc.sbr" \
	"$(INTDIR)/debug.sbr" \
	"$(INTDIR)/loadfile.sbr"

"$(OUTDIR)\hierwiz.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 /nologo /subsystem:windows /dll /machine:I386 /out:"Release/hierwiz.awx"
LINK32_FLAGS=/nologo /subsystem:windows /dll /incremental:no\
 /pdb:"$(OUTDIR)/hierwiz.pdb" /machine:I386 /out:"$(OUTDIR)/hierwiz.awx"\
 /implib:"$(OUTDIR)/hierwiz.lib" 
LINK32_OBJS= \
	"$(INTDIR)/loadfile.obj" \
	"$(INTDIR)/chooser.obj" \
	"$(INTDIR)/stdafx.obj" \
	"$(INTDIR)/cstm1dlg.obj" \
	"$(INTDIR)/hierwiz.obj" \
	"$(INTDIR)/editdlg.obj" \
	"$(INTDIR)/hierwaw.obj" \
	"$(INTDIR)/usreditc.obj" \
	"$(INTDIR)/debug.obj" \
	"$(INTDIR)/hierwiz.res"

"$(OUTDIR)\hierwiz.awx" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

# Begin Custom Build - Copying custom AppWizard to Template directory...
OutDir=.\Release
TargetPath=.\Release\hierwiz.awx
TargetName=hierwiz
InputPath=.\Release\hierwiz.awx
SOURCE=$(InputPath)

"$(MSDevDir)\Template\$(TargetName).awx" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   if not exist $(MSDevDir)\Template\nul md $(MSDevDir)\Template
   copy "$(TargetPath)" "$(MSDevDir)\Template"
   if exist "$(OutDir)\$(TargetName).pdb" copy "$(OutDir)\$(TargetName).pdb"\
    "$(MSDevDir)\Template"

# End Custom Build

!ELSEIF  "$(CFG)" == "hierwiz - Win32 Pseudo-Debug"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir WinDebug
# PROP BASE Intermediate_Dir WinDebug
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 0
# PROP Output_Dir Debug
# PROP Intermediate_Dir Debug
OUTDIR=.\Debug
INTDIR=.\Debug

ALL : "$(OUTDIR)\hierwiz.awx" "$(OUTDIR)\hierwiz.bsc"\
 "$(MSDevDir)\Template\$(TargetName).awx"

CLEAN : 
	-@erase ".\Debug\vc40.pdb"
	-@erase ".\Debug\hierwiz.pch"
	-@erase ".\Debug\hierwiz.bsc"
	-@erase ".\Debug\loadfile.sbr"
	-@erase ".\Debug\hierwiz.sbr"
	-@erase ".\Debug\stdafx.sbr"
	-@erase ".\Debug\cstm1dlg.sbr"
	-@erase ".\Debug\hierwaw.sbr"
	-@erase ".\Debug\editdlg.sbr"
	-@erase ".\Debug\usreditc.sbr"
	-@erase ".\Debug\chooser.sbr"
	-@erase ".\Debug\debug.sbr"
	-@erase ".\Debug\hierwiz.lib"
	-@erase ".\Debug\usreditc.obj"
	-@erase ".\Debug\chooser.obj"
	-@erase ".\Debug\debug.obj"
	-@erase ".\Debug\loadfile.obj"
	-@erase ".\Debug\hierwiz.obj"
	-@erase ".\Debug\stdafx.obj"
	-@erase ".\Debug\cstm1dlg.obj"
	-@erase ".\Debug\hierwaw.obj"
	-@erase ".\Debug\editdlg.obj"
	-@erase ".\Debug\hierwiz.res"
	-@erase ".\Debug\hierwiz.exp"
	-@erase ".\Debug\hierwiz.pdb"
	-@erase ".\Debug\hierwiz.awx"
	-@erase  "$(MSDevDir)\Template\$(TargetName).awx"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MD /W3 /GX /Zi /Od /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_PSEUDO_DEBUG" /D "_AFXDLL" /D "_MBCS" /FR /Yu"stdafx.h" /c
# ADD CPP /nologo /MD /W3 /GX /Zi /Od /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_PSEUDO_DEBUG" /D "_AFXDLL" /D "_MBCS" /FR /Yu"stdafx.h" /c
CPP_PROJ=/nologo /MD /W3 /GX /Zi /Od /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_PSEUDO_DEBUG" /D "_AFXDLL" /D "_MBCS" /FR"$(INTDIR)/"\
 /Fp"$(INTDIR)/hierwiz.pch" /Yu"stdafx.h" /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\Debug/
CPP_SBRS=.\Debug/
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_PSEUDO_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_PSEUDO_DEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/hierwiz.res" /d "NDEBUG" /d "_PSEUDO_DEBUG" /d\
 "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/hierwiz.bsc" 
BSC32_SBRS= \
	"$(INTDIR)/loadfile.sbr" \
	"$(INTDIR)/hierwiz.sbr" \
	"$(INTDIR)/stdafx.sbr" \
	"$(INTDIR)/cstm1dlg.sbr" \
	"$(INTDIR)/hierwaw.sbr" \
	"$(INTDIR)/editdlg.sbr" \
	"$(INTDIR)/usreditc.sbr" \
	"$(INTDIR)/chooser.sbr" \
	"$(INTDIR)/debug.sbr"

"$(OUTDIR)\hierwiz.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /debug /machine:I386
# ADD LINK32 /nologo /subsystem:windows /dll /debug /machine:I386 /out:"Debug/hierwiz.awx"
LINK32_FLAGS=/nologo /subsystem:windows /dll /incremental:no\
 /pdb:"$(OUTDIR)/hierwiz.pdb" /debug /machine:I386 /out:"$(OUTDIR)/hierwiz.awx"\
 /implib:"$(OUTDIR)/hierwiz.lib" 
LINK32_OBJS= \
	"$(INTDIR)/usreditc.obj" \
	"$(INTDIR)/chooser.obj" \
	"$(INTDIR)/debug.obj" \
	"$(INTDIR)/loadfile.obj" \
	"$(INTDIR)/hierwiz.obj" \
	"$(INTDIR)/stdafx.obj" \
	"$(INTDIR)/cstm1dlg.obj" \
	"$(INTDIR)/hierwaw.obj" \
	"$(INTDIR)/editdlg.obj" \
	"$(INTDIR)/hierwiz.res"

"$(OUTDIR)\hierwiz.awx" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

# Begin Custom Build - Copying custom AppWizard to Template directory...
OutDir=.\Debug
TargetPath=.\Debug\hierwiz.awx
TargetName=hierwiz
InputPath=.\Debug\hierwiz.awx
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

# Name "hierwiz - Win32 Release"
# Name "hierwiz - Win32 Pseudo-Debug"

!IF  "$(CFG)" == "hierwiz - Win32 Release"

!ELSEIF  "$(CFG)" == "hierwiz - Win32 Pseudo-Debug"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\stdafx.cpp

!IF  "$(CFG)" == "hierwiz - Win32 Release"

DEP_CPP_STDAF=\
	".\stdafx.h"\
	".\debug.h"\
	".\usreditc.h"\
	
# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /FR"$(INTDIR)/" /Fp"$(INTDIR)/hierwiz.pch" /Yc"stdafx.h"\
 /Fo"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\stdafx.sbr" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\hierwiz.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "hierwiz - Win32 Pseudo-Debug"

DEP_CPP_STDAF=\
	".\stdafx.h"\
	".\usreditc.h"\
	".\debug.h"\
	
# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MD /W3 /GX /Zi /Od /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_PSEUDO_DEBUG" /D "_AFXDLL" /D "_MBCS" /FR"$(INTDIR)/"\
 /Fp"$(INTDIR)/hierwiz.pch" /Yc"stdafx.h" /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c\
 $(SOURCE) \
	

"$(INTDIR)\stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\stdafx.sbr" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\hierwiz.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\debug.cpp
DEP_CPP_DEBUG=\
	".\stdafx.h"\
	".\debug.h"\
	".\usreditc.h"\
	

!IF  "$(CFG)" == "hierwiz - Win32 Release"


"$(INTDIR)\debug.obj" : $(SOURCE) $(DEP_CPP_DEBUG) "$(INTDIR)"\
 "$(INTDIR)\hierwiz.pch"

"$(INTDIR)\debug.sbr" : $(SOURCE) $(DEP_CPP_DEBUG) "$(INTDIR)"\
 "$(INTDIR)\hierwiz.pch"


!ELSEIF  "$(CFG)" == "hierwiz - Win32 Pseudo-Debug"


"$(INTDIR)\debug.obj" : $(SOURCE) $(DEP_CPP_DEBUG) "$(INTDIR)"\
 "$(INTDIR)\hierwiz.pch"

"$(INTDIR)\debug.sbr" : $(SOURCE) $(DEP_CPP_DEBUG) "$(INTDIR)"\
 "$(INTDIR)\hierwiz.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_DEBUG) "$(INTDIR)"\
 "$(INTDIR)\hierwiz.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\readme.txt

!IF  "$(CFG)" == "hierwiz - Win32 Release"

!ELSEIF  "$(CFG)" == "hierwiz - Win32 Pseudo-Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\hierwaw.cpp
DEP_CPP_HIERW=\
	".\stdafx.h"\
	".\hierwiz.h"\
	".\hierwaw.h"\
	".\chooser.h"\
	".\debug.h"\
	".\usreditc.h"\
	".\cstm1dlg.h"\
	

!IF  "$(CFG)" == "hierwiz - Win32 Release"


"$(INTDIR)\hierwaw.obj" : $(SOURCE) $(DEP_CPP_HIERW) "$(INTDIR)"\
 "$(INTDIR)\hierwiz.pch"

"$(INTDIR)\hierwaw.sbr" : $(SOURCE) $(DEP_CPP_HIERW) "$(INTDIR)"\
 "$(INTDIR)\hierwiz.pch"


!ELSEIF  "$(CFG)" == "hierwiz - Win32 Pseudo-Debug"


"$(INTDIR)\hierwaw.obj" : $(SOURCE) $(DEP_CPP_HIERW) "$(INTDIR)"\
 "$(INTDIR)\hierwiz.pch"

"$(INTDIR)\hierwaw.sbr" : $(SOURCE) $(DEP_CPP_HIERW) "$(INTDIR)"\
 "$(INTDIR)\hierwiz.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_HIERW) "$(INTDIR)"\
 "$(INTDIR)\hierwiz.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\hierwiz.cpp
DEP_CPP_HIERWI=\
	".\stdafx.h"\
	".\hierwiz.h"\
	".\hierwaw.h"\
	".\debug.h"\
	".\usreditc.h"\
	".\cstm1dlg.h"\
	

!IF  "$(CFG)" == "hierwiz - Win32 Release"


"$(INTDIR)\hierwiz.obj" : $(SOURCE) $(DEP_CPP_HIERWI) "$(INTDIR)"\
 "$(INTDIR)\hierwiz.pch"

"$(INTDIR)\hierwiz.sbr" : $(SOURCE) $(DEP_CPP_HIERWI) "$(INTDIR)"\
 "$(INTDIR)\hierwiz.pch"


!ELSEIF  "$(CFG)" == "hierwiz - Win32 Pseudo-Debug"


"$(INTDIR)\hierwiz.obj" : $(SOURCE) $(DEP_CPP_HIERWI) "$(INTDIR)"\
 "$(INTDIR)\hierwiz.pch"

"$(INTDIR)\hierwiz.sbr" : $(SOURCE) $(DEP_CPP_HIERWI) "$(INTDIR)"\
 "$(INTDIR)\hierwiz.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_HIERWI) "$(INTDIR)"\
 "$(INTDIR)\hierwiz.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\hierwiz.rc
DEP_RSC_HIERWIZ=\
	".\template\textview.h"\
	".\template\textview.cpp"\
	".\template\newproj.inf"\
	".\template\confirm.inf"\
	".\template\root.clw"\
	".\template\root.cpp"\
	".\template\root.h"\
	".\template\root.hpj"\
	".\template\root.rc"\
	".\template\root.reg"\
	".\template\ipframe.cpp"\
	".\template\ipframe.h"\
	".\template\mainfrm.cpp"\
	".\template\mainfrm.h"\
	".\template\makefile"\
	".\template\makehelp.bat"\
	".\template\mfcclass.hie"\
	".\template\mfcclass.txt"\
	".\template\resource.h"\
	".\template\stdafx.cpp"\
	".\template\stdafx.h"\
	".\template\svrdoc.cpp"\
	".\template\svrdoc.h"\
	".\template\svritem.cpp"\
	".\template\svritem.h"\
	".\template\svrview.cpp"\
	".\template\svrview.h"\
	".\template\zoomdlg.cpp"\
	".\template\zoomdlg.h"\
	".\template\afxcore.rtf"\
	".\template\afxprint.rtf"\
	".\template\appexit.bmp"\
	".\template\bullet.bmp"\
	".\template\scmax.bmp"\
	".\template\scmenu.bmp"\
	".\template\scmin.bmp"\
	".\template\itoolbar.bmp"\
	".\template\svrdoc.ico"\
	".\template\toolbar.bmp"\
	".\template\inputtxt.txt"\
	".\tvonly.bmp"\
	".\tvandtxt.bmp"\
	".\nosample.bmp"\
	".\samplefl.bmp"\
	".\samplekb.bmp"\
	

"$(INTDIR)\hierwiz.res" : $(SOURCE) $(DEP_RSC_HIERWIZ) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\cstm1dlg.cpp
DEP_CPP_CSTM1=\
	".\stdafx.h"\
	".\hierwiz.h"\
	".\hierwaw.h"\
	".\cstm1dlg.h"\
	".\debug.h"\
	".\usreditc.h"\
	

!IF  "$(CFG)" == "hierwiz - Win32 Release"


"$(INTDIR)\cstm1dlg.obj" : $(SOURCE) $(DEP_CPP_CSTM1) "$(INTDIR)"\
 "$(INTDIR)\hierwiz.pch"

"$(INTDIR)\cstm1dlg.sbr" : $(SOURCE) $(DEP_CPP_CSTM1) "$(INTDIR)"\
 "$(INTDIR)\hierwiz.pch"


!ELSEIF  "$(CFG)" == "hierwiz - Win32 Pseudo-Debug"


"$(INTDIR)\cstm1dlg.obj" : $(SOURCE) $(DEP_CPP_CSTM1) "$(INTDIR)"\
 "$(INTDIR)\hierwiz.pch"

"$(INTDIR)\cstm1dlg.sbr" : $(SOURCE) $(DEP_CPP_CSTM1) "$(INTDIR)"\
 "$(INTDIR)\hierwiz.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_CSTM1) "$(INTDIR)"\
 "$(INTDIR)\hierwiz.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\chooser.cpp
DEP_CPP_CHOOS=\
	".\stdafx.h"\
	".\hierwiz.h"\
	".\chooser.h"\
	".\loadfile.h"\
	".\editdlg.h"\
	".\debug.h"\
	".\usreditc.h"\
	".\cstm1dlg.h"\
	

!IF  "$(CFG)" == "hierwiz - Win32 Release"


"$(INTDIR)\chooser.obj" : $(SOURCE) $(DEP_CPP_CHOOS) "$(INTDIR)"\
 "$(INTDIR)\hierwiz.pch"

"$(INTDIR)\chooser.sbr" : $(SOURCE) $(DEP_CPP_CHOOS) "$(INTDIR)"\
 "$(INTDIR)\hierwiz.pch"


!ELSEIF  "$(CFG)" == "hierwiz - Win32 Pseudo-Debug"


"$(INTDIR)\chooser.obj" : $(SOURCE) $(DEP_CPP_CHOOS) "$(INTDIR)"\
 "$(INTDIR)\hierwiz.pch"

"$(INTDIR)\chooser.sbr" : $(SOURCE) $(DEP_CPP_CHOOS) "$(INTDIR)"\
 "$(INTDIR)\hierwiz.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_CHOOS) "$(INTDIR)"\
 "$(INTDIR)\hierwiz.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\usreditc.cpp
DEP_CPP_USRED=\
	".\stdafx.h"\
	".\hierwiz.h"\
	".\usreditc.h"\
	".\debug.h"\
	".\cstm1dlg.h"\
	

!IF  "$(CFG)" == "hierwiz - Win32 Release"


"$(INTDIR)\usreditc.obj" : $(SOURCE) $(DEP_CPP_USRED) "$(INTDIR)"\
 "$(INTDIR)\hierwiz.pch"

"$(INTDIR)\usreditc.sbr" : $(SOURCE) $(DEP_CPP_USRED) "$(INTDIR)"\
 "$(INTDIR)\hierwiz.pch"


!ELSEIF  "$(CFG)" == "hierwiz - Win32 Pseudo-Debug"


"$(INTDIR)\usreditc.obj" : $(SOURCE) $(DEP_CPP_USRED) "$(INTDIR)"\
 "$(INTDIR)\hierwiz.pch"

"$(INTDIR)\usreditc.sbr" : $(SOURCE) $(DEP_CPP_USRED) "$(INTDIR)"\
 "$(INTDIR)\hierwiz.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_USRED) "$(INTDIR)"\
 "$(INTDIR)\hierwiz.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\loadfile.cpp
DEP_CPP_LOADF=\
	".\stdafx.h"\
	".\hierwiz.h"\
	".\loadfile.h"\
	".\hierwaw.h"\
	".\debug.h"\
	".\usreditc.h"\
	".\cstm1dlg.h"\
	

!IF  "$(CFG)" == "hierwiz - Win32 Release"


"$(INTDIR)\loadfile.obj" : $(SOURCE) $(DEP_CPP_LOADF) "$(INTDIR)"\
 "$(INTDIR)\hierwiz.pch"

"$(INTDIR)\loadfile.sbr" : $(SOURCE) $(DEP_CPP_LOADF) "$(INTDIR)"\
 "$(INTDIR)\hierwiz.pch"


!ELSEIF  "$(CFG)" == "hierwiz - Win32 Pseudo-Debug"


"$(INTDIR)\loadfile.obj" : $(SOURCE) $(DEP_CPP_LOADF) "$(INTDIR)"\
 "$(INTDIR)\hierwiz.pch"

"$(INTDIR)\loadfile.sbr" : $(SOURCE) $(DEP_CPP_LOADF) "$(INTDIR)"\
 "$(INTDIR)\hierwiz.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_LOADF) "$(INTDIR)"\
 "$(INTDIR)\hierwiz.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\editdlg.cpp
DEP_CPP_EDITD=\
	".\stdafx.h"\
	".\hierwiz.h"\
	".\editdlg.h"\
	".\hierwaw.h"\
	".\debug.h"\
	".\usreditc.h"\
	".\cstm1dlg.h"\
	

!IF  "$(CFG)" == "hierwiz - Win32 Release"


"$(INTDIR)\editdlg.obj" : $(SOURCE) $(DEP_CPP_EDITD) "$(INTDIR)"\
 "$(INTDIR)\hierwiz.pch"

"$(INTDIR)\editdlg.sbr" : $(SOURCE) $(DEP_CPP_EDITD) "$(INTDIR)"\
 "$(INTDIR)\hierwiz.pch"


!ELSEIF  "$(CFG)" == "hierwiz - Win32 Pseudo-Debug"


"$(INTDIR)\editdlg.obj" : $(SOURCE) $(DEP_CPP_EDITD) "$(INTDIR)"\
 "$(INTDIR)\hierwiz.pch"

"$(INTDIR)\editdlg.sbr" : $(SOURCE) $(DEP_CPP_EDITD) "$(INTDIR)"\
 "$(INTDIR)\hierwiz.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_EDITD) "$(INTDIR)"\
 "$(INTDIR)\hierwiz.pch"


!ENDIF 

# End Source File
# End Target
# End Project
################################################################################
