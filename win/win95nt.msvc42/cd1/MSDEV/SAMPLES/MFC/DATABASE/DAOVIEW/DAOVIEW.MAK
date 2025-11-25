# Microsoft Visual C++ Generated NMAKE File, Format Version 40000
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

!IF "$(CFG)" == ""
CFG=daoview - Win32 Debug
!MESSAGE No configuration specified.  Defaulting to daoview - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "daoview - Win32 Release" && "$(CFG)" !=\
 "daoview - Win32 Debug" && "$(CFG)" != "daoview - Win32 Unicode Release" &&\
 "$(CFG)" != "daoview - Win32 Unicode Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "daoview.mak" CFG="daoview - Win32 Unicode Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "daoview - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "daoview - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE "daoview - Win32 Unicode Release" (based on "Win32 (x86) Application")
!MESSAGE "daoview - Win32 Unicode Debug" (based on "Win32 (x86) Application")
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
# PROP Target_Last_Scanned "daoview - Win32 Debug"
RSC=rc.exe
MTL=mktyplib.exe
CPP=cl.exe

!IF  "$(CFG)" == "daoview - Win32 Release"

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

ALL : "$(OUTDIR)\daoview.exe"

CLEAN : 
	-@erase ".\Release\daoview.exe"
	-@erase ".\Release\optionsd.obj"
	-@erase ".\Release\daoview.pch"
	-@erase ".\Release\mainfrm.obj"
	-@erase ".\Release\daoview.obj"
	-@erase ".\Release\listview.obj"
	-@erase ".\Release\daovwdoc.obj"
	-@erase ".\Release\ctrlext.obj"
	-@erase ".\Release\treeview.obj"
	-@erase ".\Release\dragitem.obj"
	-@erase ".\Release\renamedl.obj"
	-@erase ".\Release\dlgsql.obj"
	-@erase ".\Release\dlgparam.obj"
	-@erase ".\Release\crack.obj"
	-@erase ".\Release\stdafx.obj"
	-@erase ".\Release\daoview.res"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
CPP_PROJ=/nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/daoview.pch" /Yu"stdafx.h" /Fo"$(INTDIR)/"\
 /c 
CPP_OBJS=.\Release/
CPP_SBRS=
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/daoview.res" /d "NDEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/daoview.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 /nologo /subsystem:windows /machine:I386
LINK32_FLAGS=/nologo /subsystem:windows /incremental:no\
 /pdb:"$(OUTDIR)/daoview.pdb" /machine:I386 /out:"$(OUTDIR)/daoview.exe" 
LINK32_OBJS= \
	"$(INTDIR)/optionsd.obj" \
	"$(INTDIR)/mainfrm.obj" \
	"$(INTDIR)/daoview.obj" \
	"$(INTDIR)/listview.obj" \
	"$(INTDIR)/daovwdoc.obj" \
	"$(INTDIR)/ctrlext.obj" \
	"$(INTDIR)/treeview.obj" \
	"$(INTDIR)/dragitem.obj" \
	"$(INTDIR)/renamedl.obj" \
	"$(INTDIR)/dlgsql.obj" \
	"$(INTDIR)/dlgparam.obj" \
	"$(INTDIR)/crack.obj" \
	"$(INTDIR)/stdafx.obj" \
	"$(INTDIR)/daoview.res"

"$(OUTDIR)\daoview.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "daoview - Win32 Debug"

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

ALL : "$(OUTDIR)\daoview.exe"

CLEAN : 
	-@erase ".\Debug\vc40.pdb"
	-@erase ".\Debug\daoview.pch"
	-@erase ".\Debug\daoview.exe"
	-@erase ".\Debug\optionsd.obj"
	-@erase ".\Debug\mainfrm.obj"
	-@erase ".\Debug\stdafx.obj"
	-@erase ".\Debug\daoview.obj"
	-@erase ".\Debug\listview.obj"
	-@erase ".\Debug\crack.obj"
	-@erase ".\Debug\daovwdoc.obj"
	-@erase ".\Debug\ctrlext.obj"
	-@erase ".\Debug\treeview.obj"
	-@erase ".\Debug\renamedl.obj"
	-@erase ".\Debug\dragitem.obj"
	-@erase ".\Debug\dlgparam.obj"
	-@erase ".\Debug\dlgsql.obj"
	-@erase ".\Debug\daoview.res"
	-@erase ".\Debug\daoview.ilk"
	-@erase ".\Debug\daoview.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
# SUBTRACT CPP /Fr
CPP_PROJ=/nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/daoview.pch" /Yu"stdafx.h"\
 /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\Debug/
CPP_SBRS=
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/daoview.res" /d "_DEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/daoview.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386
# ADD LINK32 /nologo /subsystem:windows /debug /machine:I386
LINK32_FLAGS=/nologo /subsystem:windows /incremental:yes\
 /pdb:"$(OUTDIR)/daoview.pdb" /debug /machine:I386 /out:"$(OUTDIR)/daoview.exe" 
LINK32_OBJS= \
	"$(INTDIR)/optionsd.obj" \
	"$(INTDIR)/mainfrm.obj" \
	"$(INTDIR)/stdafx.obj" \
	"$(INTDIR)/daoview.obj" \
	"$(INTDIR)/listview.obj" \
	"$(INTDIR)/crack.obj" \
	"$(INTDIR)/daovwdoc.obj" \
	"$(INTDIR)/ctrlext.obj" \
	"$(INTDIR)/treeview.obj" \
	"$(INTDIR)/renamedl.obj" \
	"$(INTDIR)/dragitem.obj" \
	"$(INTDIR)/dlgparam.obj" \
	"$(INTDIR)/dlgsql.obj" \
	"$(INTDIR)/daoview.res"

"$(OUTDIR)\daoview.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "daoview - Win32 Unicode Release"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir Unicode Release
# PROP BASE Intermediate_Dir Unicode Release
# PROP BASE Target_Dir
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 0
# PROP Output_Dir UniRelease
# PROP Intermediate_Dir UniRelease
# PROP Target_Dir
OUTDIR=.\UniRelease
INTDIR=.\UniRelease

ALL : "$(OUTDIR)\daoview.exe"

CLEAN : 
	-@erase ".\UniRelease\daoview.exe"
	-@erase ".\UniRelease\treeview.obj"
	-@erase ".\UniRelease\daoview.pch"
	-@erase ".\UniRelease\ctrlext.obj"
	-@erase ".\UniRelease\daoview.obj"
	-@erase ".\UniRelease\daovwdoc.obj"
	-@erase ".\UniRelease\dlgparam.obj"
	-@erase ".\UniRelease\dlgsql.obj"
	-@erase ".\UniRelease\dragitem.obj"
	-@erase ".\UniRelease\listview.obj"
	-@erase ".\UniRelease\mainfrm.obj"
	-@erase ".\UniRelease\optionsd.obj"
	-@erase ".\UniRelease\renamedl.obj"
	-@erase ".\UniRelease\stdafx.obj"
	-@erase ".\UniRelease\crack.obj"
	-@erase ".\UniRelease\daoview.res"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /YX /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /D "_UNICODE" /Yu"stdafx.h" /c
CPP_PROJ=/nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /D "_UNICODE" /Fp"$(INTDIR)/daoview.pch" /Yu"stdafx.h"\
 /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\UniRelease/
CPP_SBRS=
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/daoview.res" /d "NDEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/daoview.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 /nologo /entry:"wWinMainCRTStartup" /subsystem:windows /machine:I386
LINK32_FLAGS=/nologo /entry:"wWinMainCRTStartup" /subsystem:windows\
 /incremental:no /pdb:"$(OUTDIR)/daoview.pdb" /machine:I386\
 /out:"$(OUTDIR)/daoview.exe" 
LINK32_OBJS= \
	"$(INTDIR)/treeview.obj" \
	"$(INTDIR)/ctrlext.obj" \
	"$(INTDIR)/daoview.obj" \
	"$(INTDIR)/daovwdoc.obj" \
	"$(INTDIR)/dlgparam.obj" \
	"$(INTDIR)/dlgsql.obj" \
	"$(INTDIR)/dragitem.obj" \
	"$(INTDIR)/listview.obj" \
	"$(INTDIR)/mainfrm.obj" \
	"$(INTDIR)/optionsd.obj" \
	"$(INTDIR)/renamedl.obj" \
	"$(INTDIR)/stdafx.obj" \
	"$(INTDIR)/crack.obj" \
	"$(INTDIR)/daoview.res"

"$(OUTDIR)\daoview.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "daoview - Win32 Unicode Debug"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir Unicode Debug
# PROP BASE Intermediate_Dir Unicode Debug
# PROP BASE Target_Dir
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 1
# PROP Output_Dir UniDebug
# PROP Intermediate_Dir UniDebug
# PROP Target_Dir
OUTDIR=.\UniDebug
INTDIR=.\UniDebug

ALL : "$(OUTDIR)\daoview.exe"

CLEAN : 
	-@erase ".\UniDebug\vc40.pdb"
	-@erase ".\UniDebug\daoview.pch"
	-@erase ".\UniDebug\daoview.exe"
	-@erase ".\UniDebug\dlgparam.obj"
	-@erase ".\UniDebug\dlgsql.obj"
	-@erase ".\UniDebug\optionsd.obj"
	-@erase ".\UniDebug\listview.obj"
	-@erase ".\UniDebug\stdafx.obj"
	-@erase ".\UniDebug\daovwdoc.obj"
	-@erase ".\UniDebug\crack.obj"
	-@erase ".\UniDebug\mainfrm.obj"
	-@erase ".\UniDebug\daoview.obj"
	-@erase ".\UniDebug\treeview.obj"
	-@erase ".\UniDebug\renamedl.obj"
	-@erase ".\UniDebug\dragitem.obj"
	-@erase ".\UniDebug\ctrlext.obj"
	-@erase ".\UniDebug\daoview.res"
	-@erase ".\UniDebug\daoview.ilk"
	-@erase ".\UniDebug\daoview.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /YX /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /D "_UNICODE" /Yu"stdafx.h" /c
CPP_PROJ=/nologo /MDd /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS"\
 /D "_AFXDLL" /D "_MBCS" /D "_UNICODE" /Fp"$(INTDIR)/daoview.pch" /Yu"stdafx.h"\
 /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\UniDebug/
CPP_SBRS=
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/daoview.res" /d "_DEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/daoview.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386
# ADD LINK32 /nologo /entry:"wWinMainCRTStartup" /subsystem:windows /debug /machine:I386
LINK32_FLAGS=/nologo /entry:"wWinMainCRTStartup" /subsystem:windows\
 /incremental:yes /pdb:"$(OUTDIR)/daoview.pdb" /debug /machine:I386\
 /out:"$(OUTDIR)/daoview.exe" 
LINK32_OBJS= \
	"$(INTDIR)/dlgparam.obj" \
	"$(INTDIR)/dlgsql.obj" \
	"$(INTDIR)/optionsd.obj" \
	"$(INTDIR)/listview.obj" \
	"$(INTDIR)/stdafx.obj" \
	"$(INTDIR)/daovwdoc.obj" \
	"$(INTDIR)/crack.obj" \
	"$(INTDIR)/mainfrm.obj" \
	"$(INTDIR)/daoview.obj" \
	"$(INTDIR)/treeview.obj" \
	"$(INTDIR)/renamedl.obj" \
	"$(INTDIR)/dragitem.obj" \
	"$(INTDIR)/ctrlext.obj" \
	"$(INTDIR)/daoview.res"

"$(OUTDIR)\daoview.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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

# Name "daoview - Win32 Release"
# Name "daoview - Win32 Debug"
# Name "daoview - Win32 Unicode Release"
# Name "daoview - Win32 Unicode Debug"

!IF  "$(CFG)" == "daoview - Win32 Release"

!ELSEIF  "$(CFG)" == "daoview - Win32 Debug"

!ELSEIF  "$(CFG)" == "daoview - Win32 Unicode Release"

!ELSEIF  "$(CFG)" == "daoview - Win32 Unicode Debug"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\treeview.cpp

!IF  "$(CFG)" == "daoview - Win32 Release"

DEP_CPP_TREEV=\
	".\daoview.h"\
	".\crack.h"\
	".\daovwdoc.h"\
	".\listview.h"\
	".\treeview.h"\
	".\dlgsql.h"\
	".\dragitem.h"\
	".\stdafx.h"\
	".\ctrlext.h"\
	".\ctrlext.inl"\
	

"$(INTDIR)\treeview.obj" : $(SOURCE) $(DEP_CPP_TREEV) "$(INTDIR)"\
 "$(INTDIR)\daoview.pch"


!ELSEIF  "$(CFG)" == "daoview - Win32 Debug"

DEP_CPP_TREEV=\
	".\daoview.h"\
	".\crack.h"\
	".\daovwdoc.h"\
	".\listview.h"\
	".\treeview.h"\
	".\dlgsql.h"\
	".\dragitem.h"\
	".\stdafx.h"\
	".\ctrlext.h"\
	".\ctrlext.inl"\
	

"$(INTDIR)\treeview.obj" : $(SOURCE) $(DEP_CPP_TREEV) "$(INTDIR)"\
 "$(INTDIR)\daoview.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_TREEV) "$(INTDIR)"\
 "$(INTDIR)\daoview.pch"


!ELSEIF  "$(CFG)" == "daoview - Win32 Unicode Release"

DEP_CPP_TREEV=\
	".\daoview.h"\
	".\crack.h"\
	".\daovwdoc.h"\
	".\listview.h"\
	".\treeview.h"\
	".\dlgsql.h"\
	".\dragitem.h"\
	".\mainfrm.h"\
	".\stdafx.h"\
	".\ctrlext.h"\
	".\ctrlext.inl"\
	

"$(INTDIR)\treeview.obj" : $(SOURCE) $(DEP_CPP_TREEV) "$(INTDIR)"\
 "$(INTDIR)\daoview.pch"


!ELSEIF  "$(CFG)" == "daoview - Win32 Unicode Debug"

DEP_CPP_TREEV=\
	".\daoview.h"\
	".\crack.h"\
	".\daovwdoc.h"\
	".\listview.h"\
	".\treeview.h"\
	".\dlgsql.h"\
	".\dragitem.h"\
	".\stdafx.h"\
	".\ctrlext.h"\
	".\ctrlext.inl"\
	

"$(INTDIR)\treeview.obj" : $(SOURCE) $(DEP_CPP_TREEV) "$(INTDIR)"\
 "$(INTDIR)\daoview.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_TREEV) "$(INTDIR)"\
 "$(INTDIR)\daoview.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\ctrlext.cpp
DEP_CPP_CTRLE=\
	".\ctrlext.h"\
	".\ctrlext.inl"\
	".\stdafx.h"\
	

!IF  "$(CFG)" == "daoview - Win32 Release"


"$(INTDIR)\ctrlext.obj" : $(SOURCE) $(DEP_CPP_CTRLE) "$(INTDIR)"\
 "$(INTDIR)\daoview.pch"


!ELSEIF  "$(CFG)" == "daoview - Win32 Debug"


"$(INTDIR)\ctrlext.obj" : $(SOURCE) $(DEP_CPP_CTRLE) "$(INTDIR)"\
 "$(INTDIR)\daoview.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_CTRLE) "$(INTDIR)"\
 "$(INTDIR)\daoview.pch"


!ELSEIF  "$(CFG)" == "daoview - Win32 Unicode Release"


"$(INTDIR)\ctrlext.obj" : $(SOURCE) $(DEP_CPP_CTRLE) "$(INTDIR)"\
 "$(INTDIR)\daoview.pch"


!ELSEIF  "$(CFG)" == "daoview - Win32 Unicode Debug"


"$(INTDIR)\ctrlext.obj" : $(SOURCE) $(DEP_CPP_CTRLE) "$(INTDIR)"\
 "$(INTDIR)\daoview.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_CTRLE) "$(INTDIR)"\
 "$(INTDIR)\daoview.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\daoview.cpp
DEP_CPP_DAOVI=\
	".\daoview.h"\
	".\crack.h"\
	".\daovwdoc.h"\
	".\listview.h"\
	".\treeview.h"\
	".\mainfrm.h"\
	".\stdafx.h"\
	".\ctrlext.h"\
	".\ctrlext.inl"\
	

!IF  "$(CFG)" == "daoview - Win32 Release"


"$(INTDIR)\daoview.obj" : $(SOURCE) $(DEP_CPP_DAOVI) "$(INTDIR)"\
 "$(INTDIR)\daoview.pch"


!ELSEIF  "$(CFG)" == "daoview - Win32 Debug"


"$(INTDIR)\daoview.obj" : $(SOURCE) $(DEP_CPP_DAOVI) "$(INTDIR)"\
 "$(INTDIR)\daoview.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_DAOVI) "$(INTDIR)"\
 "$(INTDIR)\daoview.pch"


!ELSEIF  "$(CFG)" == "daoview - Win32 Unicode Release"


"$(INTDIR)\daoview.obj" : $(SOURCE) $(DEP_CPP_DAOVI) "$(INTDIR)"\
 "$(INTDIR)\daoview.pch"


!ELSEIF  "$(CFG)" == "daoview - Win32 Unicode Debug"


"$(INTDIR)\daoview.obj" : $(SOURCE) $(DEP_CPP_DAOVI) "$(INTDIR)"\
 "$(INTDIR)\daoview.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_DAOVI) "$(INTDIR)"\
 "$(INTDIR)\daoview.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\daovwdoc.cpp

!IF  "$(CFG)" == "daoview - Win32 Release"

DEP_CPP_DAOVW=\
	".\daoview.h"\
	".\crack.h"\
	".\daovwdoc.h"\
	".\listview.h"\
	".\treeview.h"\
	".\optionsd.h"\
	".\stdafx.h"\
	".\ctrlext.h"\
	".\ctrlext.inl"\
	

"$(INTDIR)\daovwdoc.obj" : $(SOURCE) $(DEP_CPP_DAOVW) "$(INTDIR)"\
 "$(INTDIR)\daoview.pch"


!ELSEIF  "$(CFG)" == "daoview - Win32 Debug"

DEP_CPP_DAOVW=\
	".\daoview.h"\
	".\crack.h"\
	".\daovwdoc.h"\
	".\listview.h"\
	".\treeview.h"\
	".\optionsd.h"\
	".\stdafx.h"\
	".\ctrlext.h"\
	".\ctrlext.inl"\
	

"$(INTDIR)\daovwdoc.obj" : $(SOURCE) $(DEP_CPP_DAOVW) "$(INTDIR)"\
 "$(INTDIR)\daoview.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_DAOVW) "$(INTDIR)"\
 "$(INTDIR)\daoview.pch"


!ELSEIF  "$(CFG)" == "daoview - Win32 Unicode Release"

DEP_CPP_DAOVW=\
	".\daoview.h"\
	".\crack.h"\
	".\daovwdoc.h"\
	".\listview.h"\
	".\treeview.h"\
	".\optionsd.h"\
	".\mainfrm.h"\
	".\stdafx.h"\
	".\ctrlext.h"\
	".\ctrlext.inl"\
	

"$(INTDIR)\daovwdoc.obj" : $(SOURCE) $(DEP_CPP_DAOVW) "$(INTDIR)"\
 "$(INTDIR)\daoview.pch"


!ELSEIF  "$(CFG)" == "daoview - Win32 Unicode Debug"

DEP_CPP_DAOVW=\
	".\daoview.h"\
	".\crack.h"\
	".\daovwdoc.h"\
	".\listview.h"\
	".\treeview.h"\
	".\optionsd.h"\
	".\stdafx.h"\
	".\ctrlext.h"\
	".\ctrlext.inl"\
	

"$(INTDIR)\daovwdoc.obj" : $(SOURCE) $(DEP_CPP_DAOVW) "$(INTDIR)"\
 "$(INTDIR)\daoview.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_DAOVW) "$(INTDIR)"\
 "$(INTDIR)\daoview.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\dlgparam.cpp
DEP_CPP_DLGPA=\
	".\daoview.h"\
	".\crack.h"\
	".\daovwdoc.h"\
	".\listview.h"\
	".\treeview.h"\
	".\dlgparam.h"\
	".\stdafx.h"\
	".\ctrlext.h"\
	".\ctrlext.inl"\
	

!IF  "$(CFG)" == "daoview - Win32 Release"


"$(INTDIR)\dlgparam.obj" : $(SOURCE) $(DEP_CPP_DLGPA) "$(INTDIR)"\
 "$(INTDIR)\daoview.pch"


!ELSEIF  "$(CFG)" == "daoview - Win32 Debug"


"$(INTDIR)\dlgparam.obj" : $(SOURCE) $(DEP_CPP_DLGPA) "$(INTDIR)"\
 "$(INTDIR)\daoview.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_DLGPA) "$(INTDIR)"\
 "$(INTDIR)\daoview.pch"


!ELSEIF  "$(CFG)" == "daoview - Win32 Unicode Release"


"$(INTDIR)\dlgparam.obj" : $(SOURCE) $(DEP_CPP_DLGPA) "$(INTDIR)"\
 "$(INTDIR)\daoview.pch"


!ELSEIF  "$(CFG)" == "daoview - Win32 Unicode Debug"


"$(INTDIR)\dlgparam.obj" : $(SOURCE) $(DEP_CPP_DLGPA) "$(INTDIR)"\
 "$(INTDIR)\daoview.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_DLGPA) "$(INTDIR)"\
 "$(INTDIR)\daoview.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\dlgsql.cpp
DEP_CPP_DLGSQ=\
	".\daoview.h"\
	".\crack.h"\
	".\daovwdoc.h"\
	".\listview.h"\
	".\treeview.h"\
	".\dlgsql.h"\
	".\stdafx.h"\
	".\ctrlext.h"\
	".\ctrlext.inl"\
	

!IF  "$(CFG)" == "daoview - Win32 Release"


"$(INTDIR)\dlgsql.obj" : $(SOURCE) $(DEP_CPP_DLGSQ) "$(INTDIR)"\
 "$(INTDIR)\daoview.pch"


!ELSEIF  "$(CFG)" == "daoview - Win32 Debug"


"$(INTDIR)\dlgsql.obj" : $(SOURCE) $(DEP_CPP_DLGSQ) "$(INTDIR)"\
 "$(INTDIR)\daoview.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_DLGSQ) "$(INTDIR)"\
 "$(INTDIR)\daoview.pch"


!ELSEIF  "$(CFG)" == "daoview - Win32 Unicode Release"


"$(INTDIR)\dlgsql.obj" : $(SOURCE) $(DEP_CPP_DLGSQ) "$(INTDIR)"\
 "$(INTDIR)\daoview.pch"


!ELSEIF  "$(CFG)" == "daoview - Win32 Unicode Debug"


"$(INTDIR)\dlgsql.obj" : $(SOURCE) $(DEP_CPP_DLGSQ) "$(INTDIR)"\
 "$(INTDIR)\daoview.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_DLGSQ) "$(INTDIR)"\
 "$(INTDIR)\daoview.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\dragitem.cpp
DEP_CPP_DRAGI=\
	".\daoview.h"\
	".\crack.h"\
	".\daovwdoc.h"\
	".\listview.h"\
	".\treeview.h"\
	".\dragitem.h"\
	".\renamedl.h"\
	".\stdafx.h"\
	".\ctrlext.h"\
	".\ctrlext.inl"\
	

!IF  "$(CFG)" == "daoview - Win32 Release"


"$(INTDIR)\dragitem.obj" : $(SOURCE) $(DEP_CPP_DRAGI) "$(INTDIR)"\
 "$(INTDIR)\daoview.pch"


!ELSEIF  "$(CFG)" == "daoview - Win32 Debug"


"$(INTDIR)\dragitem.obj" : $(SOURCE) $(DEP_CPP_DRAGI) "$(INTDIR)"\
 "$(INTDIR)\daoview.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_DRAGI) "$(INTDIR)"\
 "$(INTDIR)\daoview.pch"


!ELSEIF  "$(CFG)" == "daoview - Win32 Unicode Release"


"$(INTDIR)\dragitem.obj" : $(SOURCE) $(DEP_CPP_DRAGI) "$(INTDIR)"\
 "$(INTDIR)\daoview.pch"


!ELSEIF  "$(CFG)" == "daoview - Win32 Unicode Debug"


"$(INTDIR)\dragitem.obj" : $(SOURCE) $(DEP_CPP_DRAGI) "$(INTDIR)"\
 "$(INTDIR)\daoview.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_DRAGI) "$(INTDIR)"\
 "$(INTDIR)\daoview.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\listview.cpp

!IF  "$(CFG)" == "daoview - Win32 Release"

DEP_CPP_LISTV=\
	".\daoview.h"\
	".\crack.h"\
	".\daovwdoc.h"\
	".\listview.h"\
	".\treeview.h"\
	".\dlgparam.h"\
	".\stdafx.h"\
	".\ctrlext.h"\
	".\ctrlext.inl"\
	

"$(INTDIR)\listview.obj" : $(SOURCE) $(DEP_CPP_LISTV) "$(INTDIR)"\
 "$(INTDIR)\daoview.pch"


!ELSEIF  "$(CFG)" == "daoview - Win32 Debug"

DEP_CPP_LISTV=\
	".\daoview.h"\
	".\crack.h"\
	".\daovwdoc.h"\
	".\listview.h"\
	".\treeview.h"\
	".\dlgparam.h"\
	".\stdafx.h"\
	".\ctrlext.h"\
	".\ctrlext.inl"\
	

"$(INTDIR)\listview.obj" : $(SOURCE) $(DEP_CPP_LISTV) "$(INTDIR)"\
 "$(INTDIR)\daoview.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_LISTV) "$(INTDIR)"\
 "$(INTDIR)\daoview.pch"


!ELSEIF  "$(CFG)" == "daoview - Win32 Unicode Release"

DEP_CPP_LISTV=\
	".\daoview.h"\
	".\crack.h"\
	".\daovwdoc.h"\
	".\listview.h"\
	".\treeview.h"\
	".\dlgparam.h"\
	".\mainfrm.h"\
	".\stdafx.h"\
	".\ctrlext.h"\
	".\ctrlext.inl"\
	

"$(INTDIR)\listview.obj" : $(SOURCE) $(DEP_CPP_LISTV) "$(INTDIR)"\
 "$(INTDIR)\daoview.pch"


!ELSEIF  "$(CFG)" == "daoview - Win32 Unicode Debug"

DEP_CPP_LISTV=\
	".\daoview.h"\
	".\crack.h"\
	".\daovwdoc.h"\
	".\listview.h"\
	".\treeview.h"\
	".\dlgparam.h"\
	".\stdafx.h"\
	".\ctrlext.h"\
	".\ctrlext.inl"\
	

"$(INTDIR)\listview.obj" : $(SOURCE) $(DEP_CPP_LISTV) "$(INTDIR)"\
 "$(INTDIR)\daoview.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_LISTV) "$(INTDIR)"\
 "$(INTDIR)\daoview.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\mainfrm.cpp
DEP_CPP_MAINF=\
	".\daoview.h"\
	".\crack.h"\
	".\daovwdoc.h"\
	".\listview.h"\
	".\treeview.h"\
	".\mainfrm.h"\
	".\optionsd.h"\
	".\stdafx.h"\
	".\ctrlext.h"\
	".\ctrlext.inl"\
	

!IF  "$(CFG)" == "daoview - Win32 Release"


"$(INTDIR)\mainfrm.obj" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\daoview.pch"


!ELSEIF  "$(CFG)" == "daoview - Win32 Debug"


"$(INTDIR)\mainfrm.obj" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\daoview.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\daoview.pch"


!ELSEIF  "$(CFG)" == "daoview - Win32 Unicode Release"


"$(INTDIR)\mainfrm.obj" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\daoview.pch"


!ELSEIF  "$(CFG)" == "daoview - Win32 Unicode Debug"


"$(INTDIR)\mainfrm.obj" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\daoview.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\daoview.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\optionsd.cpp
DEP_CPP_OPTIO=\
	".\daoview.h"\
	".\crack.h"\
	".\daovwdoc.h"\
	".\listview.h"\
	".\treeview.h"\
	".\optionsd.h"\
	".\stdafx.h"\
	".\ctrlext.h"\
	".\ctrlext.inl"\
	

!IF  "$(CFG)" == "daoview - Win32 Release"


"$(INTDIR)\optionsd.obj" : $(SOURCE) $(DEP_CPP_OPTIO) "$(INTDIR)"\
 "$(INTDIR)\daoview.pch"


!ELSEIF  "$(CFG)" == "daoview - Win32 Debug"


"$(INTDIR)\optionsd.obj" : $(SOURCE) $(DEP_CPP_OPTIO) "$(INTDIR)"\
 "$(INTDIR)\daoview.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_OPTIO) "$(INTDIR)"\
 "$(INTDIR)\daoview.pch"


!ELSEIF  "$(CFG)" == "daoview - Win32 Unicode Release"


"$(INTDIR)\optionsd.obj" : $(SOURCE) $(DEP_CPP_OPTIO) "$(INTDIR)"\
 "$(INTDIR)\daoview.pch"


!ELSEIF  "$(CFG)" == "daoview - Win32 Unicode Debug"


"$(INTDIR)\optionsd.obj" : $(SOURCE) $(DEP_CPP_OPTIO) "$(INTDIR)"\
 "$(INTDIR)\daoview.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_OPTIO) "$(INTDIR)"\
 "$(INTDIR)\daoview.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\renamedl.cpp
DEP_CPP_RENAM=\
	".\daoview.h"\
	".\crack.h"\
	".\daovwdoc.h"\
	".\listview.h"\
	".\treeview.h"\
	".\renamedl.h"\
	".\stdafx.h"\
	".\ctrlext.h"\
	".\ctrlext.inl"\
	

!IF  "$(CFG)" == "daoview - Win32 Release"


"$(INTDIR)\renamedl.obj" : $(SOURCE) $(DEP_CPP_RENAM) "$(INTDIR)"\
 "$(INTDIR)\daoview.pch"


!ELSEIF  "$(CFG)" == "daoview - Win32 Debug"


"$(INTDIR)\renamedl.obj" : $(SOURCE) $(DEP_CPP_RENAM) "$(INTDIR)"\
 "$(INTDIR)\daoview.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_RENAM) "$(INTDIR)"\
 "$(INTDIR)\daoview.pch"


!ELSEIF  "$(CFG)" == "daoview - Win32 Unicode Release"


"$(INTDIR)\renamedl.obj" : $(SOURCE) $(DEP_CPP_RENAM) "$(INTDIR)"\
 "$(INTDIR)\daoview.pch"


!ELSEIF  "$(CFG)" == "daoview - Win32 Unicode Debug"


"$(INTDIR)\renamedl.obj" : $(SOURCE) $(DEP_CPP_RENAM) "$(INTDIR)"\
 "$(INTDIR)\daoview.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_RENAM) "$(INTDIR)"\
 "$(INTDIR)\daoview.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\stdafx.cpp
DEP_CPP_STDAF=\
	".\stdafx.h"\
	".\ctrlext.h"\
	".\ctrlext.inl"\
	

!IF  "$(CFG)" == "daoview - Win32 Release"

# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/daoview.pch" /Yc"stdafx.h" /Fo"$(INTDIR)/"\
 /c $(SOURCE) \
	

"$(INTDIR)\stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\daoview.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "daoview - Win32 Debug"

# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/daoview.pch" /Yc"stdafx.h"\
 /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\daoview.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "daoview - Win32 Unicode Release"

# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /D "_UNICODE" /Fp"$(INTDIR)/daoview.pch" /Yc"stdafx.h"\
 /Fo"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\daoview.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "daoview - Win32 Unicode Debug"

# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MDd /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS"\
 /D "_AFXDLL" /D "_MBCS" /D "_UNICODE" /Fp"$(INTDIR)/daoview.pch" /Yc"stdafx.h"\
 /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\daoview.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\crack.cpp
DEP_CPP_CRACK=\
	".\crack.h"\
	".\stdafx.h"\
	".\ctrlext.h"\
	".\ctrlext.inl"\
	

!IF  "$(CFG)" == "daoview - Win32 Release"


"$(INTDIR)\crack.obj" : $(SOURCE) $(DEP_CPP_CRACK) "$(INTDIR)"\
 "$(INTDIR)\daoview.pch"


!ELSEIF  "$(CFG)" == "daoview - Win32 Debug"


"$(INTDIR)\crack.obj" : $(SOURCE) $(DEP_CPP_CRACK) "$(INTDIR)"\
 "$(INTDIR)\daoview.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_CRACK) "$(INTDIR)"\
 "$(INTDIR)\daoview.pch"


!ELSEIF  "$(CFG)" == "daoview - Win32 Unicode Release"


"$(INTDIR)\crack.obj" : $(SOURCE) $(DEP_CPP_CRACK) "$(INTDIR)"\
 "$(INTDIR)\daoview.pch"


!ELSEIF  "$(CFG)" == "daoview - Win32 Unicode Debug"


"$(INTDIR)\crack.obj" : $(SOURCE) $(DEP_CPP_CRACK) "$(INTDIR)"\
 "$(INTDIR)\daoview.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_CRACK) "$(INTDIR)"\
 "$(INTDIR)\daoview.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\daoview.rc
DEP_RSC_DAOVIE=\
	".\res\daoview.ico"\
	".\res\daovwdoc.ico"\
	".\res\bmp00002.bmp"\
	".\res\toolbar.bmp"\
	".\res\daoview.rc2"\
	

!IF  "$(CFG)" == "daoview - Win32 Release"


"$(INTDIR)\daoview.res" : $(SOURCE) $(DEP_RSC_DAOVIE) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "daoview - Win32 Debug"


"$(INTDIR)\daoview.res" : $(SOURCE) $(DEP_RSC_DAOVIE) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "daoview - Win32 Unicode Release"


"$(INTDIR)\daoview.res" : $(SOURCE) $(DEP_RSC_DAOVIE) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "daoview - Win32 Unicode Debug"


"$(INTDIR)\daoview.res" : $(SOURCE) $(DEP_RSC_DAOVIE) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ENDIF 

# End Source File
# End Target
# End Project
################################################################################
