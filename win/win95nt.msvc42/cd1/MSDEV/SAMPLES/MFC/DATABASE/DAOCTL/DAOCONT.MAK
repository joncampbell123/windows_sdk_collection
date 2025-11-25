# Microsoft Developer Studio Generated NMAKE File, Format Version 40001
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

!IF "$(CFG)" == ""
CFG=DaoCont - Win32 Debug
!MESSAGE No configuration specified.  Defaulting to DaoCont - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "DaoCont - Win32 Release" && "$(CFG)" !=\
 "DaoCont - Win32 Debug" && "$(CFG)" != "DaoCont - Win32 Unicode Release" &&\
 "$(CFG)" != "DaoCont - Win32 Unicode Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "daocont.mak" CFG="DaoCont - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "DaoCont - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "DaoCont - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE "DaoCont - Win32 Unicode Release" (based on "Win32 (x86) Application")
!MESSAGE "DaoCont - Win32 Unicode Debug" (based on "Win32 (x86) Application")
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
# PROP Target_Last_Scanned "DaoCont - Win32 Debug"
CPP=cl.exe
RSC=rc.exe
MTL=mktyplib.exe

!IF  "$(CFG)" == "DaoCont - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "DaoCont_"
# PROP BASE Intermediate_Dir "DaoCont_"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
OUTDIR=.\Release
INTDIR=.\Release

ALL : "$(OUTDIR)\daocont.exe"

CLEAN : 
	-@erase ".\Release\daocont.exe"
	-@erase ".\Release\ctrlext.obj"
	-@erase ".\Release\daocont.pch"
	-@erase ".\Release\daocndlg.obj"
	-@erase ".\Release\daoedit.obj"
	-@erase ".\Release\daocont.obj"
	-@erase ".\Release\accpict.obj"
	-@erase ".\Release\stdafx.obj"
	-@erase ".\Release\daocont.res"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
CPP_PROJ=/nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/daocont.pch" /Yu"stdafx.h" /Fo"$(INTDIR)/"\
 /c 
CPP_OBJS=.\Release/
CPP_SBRS=
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/daocont.res" /d "NDEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/daocont.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 /nologo /subsystem:windows /machine:I386
LINK32_FLAGS=/nologo /subsystem:windows /incremental:no\
 /pdb:"$(OUTDIR)/daocont.pdb" /machine:I386 /out:"$(OUTDIR)/daocont.exe" 
LINK32_OBJS= \
	"$(INTDIR)/ctrlext.obj" \
	"$(INTDIR)/daocndlg.obj" \
	"$(INTDIR)/daoedit.obj" \
	"$(INTDIR)/daocont.obj" \
	"$(INTDIR)/accpict.obj" \
	"$(INTDIR)/stdafx.obj" \
	"$(INTDIR)/daocont.res"

"$(OUTDIR)\daocont.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "DaoCont - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "DaoCont0"
# PROP BASE Intermediate_Dir "DaoCont0"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
OUTDIR=.\Debug
INTDIR=.\Debug

ALL : "$(OUTDIR)\daocont.exe"

CLEAN : 
	-@erase ".\Debug\vc40.pdb"
	-@erase ".\Debug\daocont.pch"
	-@erase ".\Debug\vc40.idb"
	-@erase ".\Debug\daocont.exe"
	-@erase ".\Debug\daoedit.obj"
	-@erase ".\Debug\daocndlg.obj"
	-@erase ".\Debug\stdafx.obj"
	-@erase ".\Debug\daocont.obj"
	-@erase ".\Debug\accpict.obj"
	-@erase ".\Debug\ctrlext.obj"
	-@erase ".\Debug\daocont.res"
	-@erase ".\Debug\daocont.ilk"
	-@erase ".\Debug\daocont.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
# SUBTRACT CPP /Fr
CPP_PROJ=/nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/daocont.pch" /Yu"stdafx.h"\
 /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\Debug/
CPP_SBRS=
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/daocont.res" /d "_DEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/daocont.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386
# ADD LINK32 /nologo /subsystem:windows /debug /machine:I386
LINK32_FLAGS=/nologo /subsystem:windows /incremental:yes\
 /pdb:"$(OUTDIR)/daocont.pdb" /debug /machine:I386 /out:"$(OUTDIR)/daocont.exe" 
LINK32_OBJS= \
	"$(INTDIR)/daoedit.obj" \
	"$(INTDIR)/daocndlg.obj" \
	"$(INTDIR)/stdafx.obj" \
	"$(INTDIR)/daocont.obj" \
	"$(INTDIR)/accpict.obj" \
	"$(INTDIR)/ctrlext.obj" \
	"$(INTDIR)/daocont.res"

"$(OUTDIR)\daocont.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "DaoCont - Win32 Unicode Release"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "DaoCont_"
# PROP BASE Intermediate_Dir "DaoCont_"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "UniRel"
# PROP Intermediate_Dir "UniRel"
# PROP Target_Dir ""
OUTDIR=.\UniRel
INTDIR=.\UniRel

ALL : "$(OUTDIR)\daocont.exe"

CLEAN : 
	-@erase ".\UniRel\daocont.exe"
	-@erase ".\UniRel\daocndlg.obj"
	-@erase ".\UniRel\daocont.pch"
	-@erase ".\UniRel\stdafx.obj"
	-@erase ".\UniRel\daocont.obj"
	-@erase ".\UniRel\accpict.obj"
	-@erase ".\UniRel\ctrlext.obj"
	-@erase ".\UniRel\daoedit.obj"
	-@erase ".\UniRel\daocont.res"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /D "_UNICODE" /Yu"stdafx.h" /c
CPP_PROJ=/nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /D "_UNICODE" /Fp"$(INTDIR)/daocont.pch" /Yu"stdafx.h"\
 /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\UniRel/
CPP_SBRS=
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/daocont.res" /d "NDEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/daocont.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 /nologo /entry:"wWinMainCRTStartup" /subsystem:windows /machine:I386
LINK32_FLAGS=/nologo /entry:"wWinMainCRTStartup" /subsystem:windows\
 /incremental:no /pdb:"$(OUTDIR)/daocont.pdb" /machine:I386\
 /out:"$(OUTDIR)/daocont.exe" 
LINK32_OBJS= \
	"$(INTDIR)/daocndlg.obj" \
	"$(INTDIR)/stdafx.obj" \
	"$(INTDIR)/daocont.obj" \
	"$(INTDIR)/accpict.obj" \
	"$(INTDIR)/ctrlext.obj" \
	"$(INTDIR)/daoedit.obj" \
	"$(INTDIR)/daocont.res"

"$(OUTDIR)\daocont.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "DaoCont - Win32 Unicode Debug"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "DaoCont0"
# PROP BASE Intermediate_Dir "DaoCont0"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "UniDebug"
# PROP Intermediate_Dir "UniDebug"
# PROP Target_Dir ""
OUTDIR=.\UniDebug
INTDIR=.\UniDebug

ALL : "$(OUTDIR)\daocont.exe"

CLEAN : 
	-@erase ".\UniDebug\vc40.pdb"
	-@erase ".\UniDebug\daocont.pch"
	-@erase ".\UniDebug\vc40.idb"
	-@erase ".\UniDebug\daocont.exe"
	-@erase ".\UniDebug\daocont.obj"
	-@erase ".\UniDebug\accpict.obj"
	-@erase ".\UniDebug\daocndlg.obj"
	-@erase ".\UniDebug\stdafx.obj"
	-@erase ".\UniDebug\ctrlext.obj"
	-@erase ".\UniDebug\daoedit.obj"
	-@erase ".\UniDebug\daocont.res"
	-@erase ".\UniDebug\daocont.ilk"
	-@erase ".\UniDebug\daocont.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
# SUBTRACT BASE CPP /Fr
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /D "_UNICODE" /Yu"stdafx.h" /c
# SUBTRACT CPP /Fr
CPP_PROJ=/nologo /MDd /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS"\
 /D "_AFXDLL" /D "_MBCS" /D "_UNICODE" /Fp"$(INTDIR)/daocont.pch" /Yu"stdafx.h"\
 /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\UniDebug/
CPP_SBRS=
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/daocont.res" /d "_DEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/daocont.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386
# ADD LINK32 /nologo /entry:"wWinMainCRTStartup" /subsystem:windows /debug /machine:I386
LINK32_FLAGS=/nologo /entry:"wWinMainCRTStartup" /subsystem:windows\
 /incremental:yes /pdb:"$(OUTDIR)/daocont.pdb" /debug /machine:I386\
 /out:"$(OUTDIR)/daocont.exe" 
LINK32_OBJS= \
	"$(INTDIR)/daocont.obj" \
	"$(INTDIR)/accpict.obj" \
	"$(INTDIR)/daocndlg.obj" \
	"$(INTDIR)/stdafx.obj" \
	"$(INTDIR)/ctrlext.obj" \
	"$(INTDIR)/daoedit.obj" \
	"$(INTDIR)/daocont.res"

"$(OUTDIR)\daocont.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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

# Name "DaoCont - Win32 Release"
# Name "DaoCont - Win32 Debug"
# Name "DaoCont - Win32 Unicode Release"
# Name "DaoCont - Win32 Unicode Debug"

!IF  "$(CFG)" == "DaoCont - Win32 Release"

!ELSEIF  "$(CFG)" == "DaoCont - Win32 Debug"

!ELSEIF  "$(CFG)" == "DaoCont - Win32 Unicode Release"

!ELSEIF  "$(CFG)" == "DaoCont - Win32 Unicode Debug"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\stdafx.cpp
DEP_CPP_STDAF=\
	".\stdafx.h"\
	".\ctrlext.h"\
	".\ctrlext.inl"\
	

!IF  "$(CFG)" == "DaoCont - Win32 Release"

# ADD CPP /Yc"stdafx.h"

"$(INTDIR)\stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(CPP) /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/daocont.pch" /Yc"stdafx.h" /Fo"$(INTDIR)/"\
 /c $(SOURCE)


!ELSEIF  "$(CFG)" == "DaoCont - Win32 Debug"

# ADD CPP /Yc"stdafx.h"

"$(INTDIR)\stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(CPP) /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/daocont.pch" /Yc"stdafx.h"\
 /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c $(SOURCE)


!ELSEIF  "$(CFG)" == "DaoCont - Win32 Unicode Release"

# ADD CPP /Yc"stdafx.h"

"$(INTDIR)\stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(CPP) /nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /D "_UNICODE" /Fp"$(INTDIR)/daocont.pch" /Yc"stdafx.h"\
 /Fo"$(INTDIR)/" /c $(SOURCE)


!ELSEIF  "$(CFG)" == "DaoCont - Win32 Unicode Debug"

# ADD CPP /Yc"stdafx.h"

"$(INTDIR)\stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(CPP) /nologo /MDd /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS"\
 /D "_AFXDLL" /D "_MBCS" /D "_UNICODE" /Fp"$(INTDIR)/daocont.pch" /Yc"stdafx.h"\
 /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\daocndlg.cpp
DEP_CPP_DAOCN=\
	".\daocndlg.h"\
	".\accpict.h"\
	".\daoedit.h"\
	".\daocont.h"\
	".\stdafx.h"\
	".\ctrlext.h"\
	".\ctrlext.inl"\
	

!IF  "$(CFG)" == "DaoCont - Win32 Release"


"$(INTDIR)\daocndlg.obj" : $(SOURCE) $(DEP_CPP_DAOCN) "$(INTDIR)"\
 "$(INTDIR)\daocont.pch"


!ELSEIF  "$(CFG)" == "DaoCont - Win32 Debug"


"$(INTDIR)\daocndlg.obj" : $(SOURCE) $(DEP_CPP_DAOCN) "$(INTDIR)"\
 "$(INTDIR)\daocont.pch"


!ELSEIF  "$(CFG)" == "DaoCont - Win32 Unicode Release"


"$(INTDIR)\daocndlg.obj" : $(SOURCE) $(DEP_CPP_DAOCN) "$(INTDIR)"\
 "$(INTDIR)\daocont.pch"


!ELSEIF  "$(CFG)" == "DaoCont - Win32 Unicode Debug"


"$(INTDIR)\daocndlg.obj" : $(SOURCE) $(DEP_CPP_DAOCN) "$(INTDIR)"\
 "$(INTDIR)\daocont.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\daocont.cpp
DEP_CPP_DAOCO=\
	".\daocndlg.h"\
	".\accpict.h"\
	".\daoedit.h"\
	".\daocont.h"\
	".\stdafx.h"\
	".\ctrlext.h"\
	".\ctrlext.inl"\
	

!IF  "$(CFG)" == "DaoCont - Win32 Release"


"$(INTDIR)\daocont.obj" : $(SOURCE) $(DEP_CPP_DAOCO) "$(INTDIR)"\
 "$(INTDIR)\daocont.pch"


!ELSEIF  "$(CFG)" == "DaoCont - Win32 Debug"


"$(INTDIR)\daocont.obj" : $(SOURCE) $(DEP_CPP_DAOCO) "$(INTDIR)"\
 "$(INTDIR)\daocont.pch"


!ELSEIF  "$(CFG)" == "DaoCont - Win32 Unicode Release"


"$(INTDIR)\daocont.obj" : $(SOURCE) $(DEP_CPP_DAOCO) "$(INTDIR)"\
 "$(INTDIR)\daocont.pch"


!ELSEIF  "$(CFG)" == "DaoCont - Win32 Unicode Debug"


"$(INTDIR)\daocont.obj" : $(SOURCE) $(DEP_CPP_DAOCO) "$(INTDIR)"\
 "$(INTDIR)\daocont.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\daoedit.cpp
DEP_CPP_DAOED=\
	".\daoedit.h"\
	".\stdafx.h"\
	".\ctrlext.h"\
	".\ctrlext.inl"\
	

!IF  "$(CFG)" == "DaoCont - Win32 Release"


"$(INTDIR)\daoedit.obj" : $(SOURCE) $(DEP_CPP_DAOED) "$(INTDIR)"\
 "$(INTDIR)\daocont.pch"


!ELSEIF  "$(CFG)" == "DaoCont - Win32 Debug"


"$(INTDIR)\daoedit.obj" : $(SOURCE) $(DEP_CPP_DAOED) "$(INTDIR)"\
 "$(INTDIR)\daocont.pch"


!ELSEIF  "$(CFG)" == "DaoCont - Win32 Unicode Release"


"$(INTDIR)\daoedit.obj" : $(SOURCE) $(DEP_CPP_DAOED) "$(INTDIR)"\
 "$(INTDIR)\daocont.pch"


!ELSEIF  "$(CFG)" == "DaoCont - Win32 Unicode Debug"


"$(INTDIR)\daoedit.obj" : $(SOURCE) $(DEP_CPP_DAOED) "$(INTDIR)"\
 "$(INTDIR)\daocont.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\ctrlext.cpp
DEP_CPP_CTRLE=\
	".\ctrlext.h"\
	".\ctrlext.inl"\
	".\stdafx.h"\
	

!IF  "$(CFG)" == "DaoCont - Win32 Release"


"$(INTDIR)\ctrlext.obj" : $(SOURCE) $(DEP_CPP_CTRLE) "$(INTDIR)"\
 "$(INTDIR)\daocont.pch"


!ELSEIF  "$(CFG)" == "DaoCont - Win32 Debug"


"$(INTDIR)\ctrlext.obj" : $(SOURCE) $(DEP_CPP_CTRLE) "$(INTDIR)"\
 "$(INTDIR)\daocont.pch"


!ELSEIF  "$(CFG)" == "DaoCont - Win32 Unicode Release"


"$(INTDIR)\ctrlext.obj" : $(SOURCE) $(DEP_CPP_CTRLE) "$(INTDIR)"\
 "$(INTDIR)\daocont.pch"


!ELSEIF  "$(CFG)" == "DaoCont - Win32 Unicode Debug"


"$(INTDIR)\ctrlext.obj" : $(SOURCE) $(DEP_CPP_CTRLE) "$(INTDIR)"\
 "$(INTDIR)\daocont.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\daocont.rc
DEP_RSC_DAOCON=\
	".\res\daocont.ico"\
	".\res\treeimag.bmp"\
	".\res\bitmap1.bmp"\
	".\res\daocont.rc2"\
	

!IF  "$(CFG)" == "DaoCont - Win32 Release"


"$(INTDIR)\daocont.res" : $(SOURCE) $(DEP_RSC_DAOCON) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "DaoCont - Win32 Debug"


"$(INTDIR)\daocont.res" : $(SOURCE) $(DEP_RSC_DAOCON) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "DaoCont - Win32 Unicode Release"


"$(INTDIR)\daocont.res" : $(SOURCE) $(DEP_RSC_DAOCON) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "DaoCont - Win32 Unicode Debug"


"$(INTDIR)\daocont.res" : $(SOURCE) $(DEP_RSC_DAOCON) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\accpict.cpp
DEP_CPP_ACCPI=\
	".\accpict.h"\
	".\stdafx.h"\
	".\ctrlext.h"\
	".\ctrlext.inl"\
	

!IF  "$(CFG)" == "DaoCont - Win32 Release"


"$(INTDIR)\accpict.obj" : $(SOURCE) $(DEP_CPP_ACCPI) "$(INTDIR)"\
 "$(INTDIR)\daocont.pch"


!ELSEIF  "$(CFG)" == "DaoCont - Win32 Debug"


"$(INTDIR)\accpict.obj" : $(SOURCE) $(DEP_CPP_ACCPI) "$(INTDIR)"\
 "$(INTDIR)\daocont.pch"


!ELSEIF  "$(CFG)" == "DaoCont - Win32 Unicode Release"


"$(INTDIR)\accpict.obj" : $(SOURCE) $(DEP_CPP_ACCPI) "$(INTDIR)"\
 "$(INTDIR)\daocont.pch"


!ELSEIF  "$(CFG)" == "DaoCont - Win32 Unicode Debug"


"$(INTDIR)\accpict.obj" : $(SOURCE) $(DEP_CPP_ACCPI) "$(INTDIR)"\
 "$(INTDIR)\daocont.pch"


!ENDIF 

# End Source File
# End Target
# End Project
################################################################################
################################################################################
# Section DaoCont : {1F4E7C23-C38F-11CE-9611-00AA004A75CF}
# 	0:9:Picture.h:C:\daoctl\Picture.h
# 	0:8:Font.cpp:C:\samples\mfc\database\DaoCtl\containr\Font.cpp
# 	0:6:Font.h:C:\samples\mfc\database\DaoCtl\containr\Font.h
# 	0:11:Picture.cpp:C:\daoctl\Picture.cpp
# 	0:12:AccessPict.h:C:\daoctl\AccessPict.h
# 	0:14:AccessPict.cpp:C:\daoctl\AccessPict.cpp
# 	2:21:DefaultSinkHeaderFile:accpict.h
# 	2:16:DefaultSinkClass:CAccessPict
# End Section
################################################################################
################################################################################
# Section DaoCont : {1F4E7C21-C38F-11CE-9611-00AA004A75CF}
# 	2:5:Class:CAccessPict
# 	2:10:HeaderFile:accpict.h
# 	2:8:ImplFile:accpict.cpp
# End Section
################################################################################
################################################################################
# Section OLE Controls
# 	{1F4E7C23-C38F-11CE-9611-00AA004A75CF}
# End Section
################################################################################
