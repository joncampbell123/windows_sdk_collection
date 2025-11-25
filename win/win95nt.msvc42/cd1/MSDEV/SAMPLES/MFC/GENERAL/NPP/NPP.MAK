# Microsoft Visual C++ Generated NMAKE File, Format Version 40000
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

!IF "$(CFG)" == ""
CFG=NPP - Win32 Debug
!MESSAGE No configuration specified.  Defaulting to NPP - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "NPP - Win32 Release" && "$(CFG)" != "NPP - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "NPP.MAK" CFG="NPP - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "NPP - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "NPP - Win32 Debug" (based on "Win32 (x86) Application")
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
MTL=mktyplib.exe
RSC=rc.exe

!IF  "$(CFG)" == "NPP - Win32 Release"

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

ALL : "$(OUTDIR)\NPP.exe" "$(OUTDIR)\NPP.bsc"

CLEAN : 
	-@erase ".\Release\NPP.exe"
	-@erase ".\Release\Npp.res"
	-@erase ".\Release\Finddlg.obj"
	-@erase ".\Release\NPP.pch"
	-@erase ".\Release\Gotodlg.obj"
	-@erase ".\Release\History.obj"
	-@erase ".\Release\Mainfrm.obj"
	-@erase ".\Release\Np.obj"
	-@erase ".\Release\Npdoc.obj"
	-@erase ".\Release\Combobar.obj"
	-@erase ".\Release\Npview.obj"
	-@erase ".\Release\Stdafx.obj"
	-@erase ".\Release\NPP.idb"
	-@erase ".\Release\NPP.bsc"
	-@erase ".\Release\Finddlg.sbr"
	-@erase ".\Release\Gotodlg.sbr"
	-@erase ".\Release\History.sbr"
	-@erase ".\Release\Mainfrm.sbr"
	-@erase ".\Release\Np.sbr"
	-@erase ".\Release\Npdoc.sbr"
	-@erase ".\Release\Combobar.sbr"
	-@erase ".\Release\Npview.sbr"
	-@erase ".\Release\Stdafx.sbr"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /Yu"Stdafx.h" /c
CPP_PROJ=/nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /FR"$(INTDIR)/" /Fp"$(INTDIR)/NPP.pch" /Yu"Stdafx.h"\
 /Fo"$(INTDIR)/" /Fd"$(OUTDIR)/NPP.pdb" /c 
CPP_OBJS=.\Release/
CPP_SBRS=.\Release/
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/Npp.res" /d "NDEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/NPP.bsc" 
BSC32_SBRS= \
	"$(INTDIR)/Finddlg.sbr" \
	"$(INTDIR)/Gotodlg.sbr" \
	"$(INTDIR)/History.sbr" \
	"$(INTDIR)/Mainfrm.sbr" \
	"$(INTDIR)/Np.sbr" \
	"$(INTDIR)/Npdoc.sbr" \
	"$(INTDIR)/Combobar.sbr" \
	"$(INTDIR)/Npview.sbr" \
	"$(INTDIR)/Stdafx.sbr"

"$(OUTDIR)\NPP.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 /nologo /subsystem:windows /machine:I386
LINK32_FLAGS=/nologo /subsystem:windows /incremental:no\
 /pdb:"$(OUTDIR)/NPP.pdb" /machine:I386 /out:"$(OUTDIR)/NPP.exe" 
LINK32_OBJS= \
	"$(INTDIR)/Finddlg.obj" \
	"$(INTDIR)/Gotodlg.obj" \
	"$(INTDIR)/History.obj" \
	"$(INTDIR)/Mainfrm.obj" \
	"$(INTDIR)/Np.obj" \
	"$(INTDIR)/Npdoc.obj" \
	"$(INTDIR)/Combobar.obj" \
	"$(INTDIR)/Npview.obj" \
	"$(INTDIR)/Stdafx.obj" \
	"$(INTDIR)/Npp.res"

"$(OUTDIR)\NPP.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "NPP - Win32 Debug"

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

ALL : "$(OUTDIR)\NPP.exe" "$(OUTDIR)\NPP.bsc"

CLEAN : 
	-@erase ".\Debug\NPP.exe"
	-@erase ".\Debug\Npp.res"
	-@erase ".\Debug\Finddlg.obj"
	-@erase ".\Debug\NPP.pch"
	-@erase ".\Debug\Gotodlg.obj"
	-@erase ".\Debug\History.obj"
	-@erase ".\Debug\Mainfrm.obj"
	-@erase ".\Debug\Np.obj"
	-@erase ".\Debug\Npdoc.obj"
	-@erase ".\Debug\Combobar.obj"
	-@erase ".\Debug\Npview.obj"
	-@erase ".\Debug\Stdafx.obj"
	-@erase ".\Debug\NPP.ilk"
	-@erase ".\Debug\NPP.pdb"
	-@erase ".\Debug\NPP.idb"
	-@erase ".\Debug\NPP.bsc"
	-@erase ".\Debug\Finddlg.sbr"
	-@erase ".\Debug\Gotodlg.sbr"
	-@erase ".\Debug\History.sbr"
	-@erase ".\Debug\Mainfrm.sbr"
	-@erase ".\Debug\Np.sbr"
	-@erase ".\Debug\Npdoc.sbr"
	-@erase ".\Debug\Combobar.sbr"
	-@erase ".\Debug\Npview.sbr"
	-@erase ".\Debug\Stdafx.sbr"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /Yu"Stdafx.h" /c
CPP_PROJ=/nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /D "_AFXDLL" /D "_MBCS" /FR"$(INTDIR)/" /Fp"$(INTDIR)/NPP.pch" /Yu"Stdafx.h"\
 /Fo"$(INTDIR)/" /Fd"$(OUTDIR)/NPP.pdb" /c 
CPP_OBJS=.\Debug/
CPP_SBRS=.\Debug/
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/Npp.res" /d "_DEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/NPP.bsc" 
BSC32_SBRS= \
	"$(INTDIR)/Finddlg.sbr" \
	"$(INTDIR)/Gotodlg.sbr" \
	"$(INTDIR)/History.sbr" \
	"$(INTDIR)/Mainfrm.sbr" \
	"$(INTDIR)/Np.sbr" \
	"$(INTDIR)/Npdoc.sbr" \
	"$(INTDIR)/Combobar.sbr" \
	"$(INTDIR)/Npview.sbr" \
	"$(INTDIR)/Stdafx.sbr"

"$(OUTDIR)\NPP.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386
# ADD LINK32 /nologo /subsystem:windows /debug /machine:I386
LINK32_FLAGS=/nologo /subsystem:windows /incremental:yes\
 /pdb:"$(OUTDIR)/NPP.pdb" /debug /machine:I386 /out:"$(OUTDIR)/NPP.exe" 
LINK32_OBJS= \
	"$(INTDIR)/Finddlg.obj" \
	"$(INTDIR)/Gotodlg.obj" \
	"$(INTDIR)/History.obj" \
	"$(INTDIR)/Mainfrm.obj" \
	"$(INTDIR)/Np.obj" \
	"$(INTDIR)/Npdoc.obj" \
	"$(INTDIR)/Combobar.obj" \
	"$(INTDIR)/Npview.obj" \
	"$(INTDIR)/Stdafx.obj" \
	"$(INTDIR)/Npp.res"

"$(OUTDIR)\NPP.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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

# Name "NPP - Win32 Release"
# Name "NPP - Win32 Debug"

!IF  "$(CFG)" == "NPP - Win32 Release"

!ELSEIF  "$(CFG)" == "NPP - Win32 Debug"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\Npp.rc
DEP_RSC_NPP_R=\
	".\res\np.ico"\
	".\res\npdoc.ico"\
	".\res\toolbar.bmp"\
	".\res\np.rc2"\
	

"$(INTDIR)\Npp.res" : $(SOURCE) $(DEP_RSC_NPP_R) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\Finddlg.cpp
DEP_CPP_FINDD=\
	".\combobar.h"\
	".\finddlg.h"\
	".\mainfrm.h"\
	".\np.h"\
	".\resource.h"\
	".\npdoc.h"\
	".\npview.h"\
	".\history.h"\
	".\stdafx.h"\
	

!IF  "$(CFG)" == "NPP - Win32 Release"


"$(INTDIR)\Finddlg.obj" : $(SOURCE) $(DEP_CPP_FINDD) "$(INTDIR)"\
 "$(INTDIR)\NPP.pch"

"$(INTDIR)\Finddlg.sbr" : $(SOURCE) $(DEP_CPP_FINDD) "$(INTDIR)"\
 "$(INTDIR)\NPP.pch"

"$(INTDIR)\NPP.idb" : $(SOURCE) $(DEP_CPP_FINDD) "$(INTDIR)"\
 "$(INTDIR)\NPP.pch"


!ELSEIF  "$(CFG)" == "NPP - Win32 Debug"


"$(INTDIR)\Finddlg.obj" : $(SOURCE) $(DEP_CPP_FINDD) "$(INTDIR)"\
 "$(INTDIR)\NPP.pch"

"$(INTDIR)\Finddlg.sbr" : $(SOURCE) $(DEP_CPP_FINDD) "$(INTDIR)"\
 "$(INTDIR)\NPP.pch"

"$(INTDIR)\NPP.pdb" : $(SOURCE) $(DEP_CPP_FINDD) "$(INTDIR)"\
 "$(INTDIR)\NPP.pch"

"$(INTDIR)\NPP.idb" : $(SOURCE) $(DEP_CPP_FINDD) "$(INTDIR)"\
 "$(INTDIR)\NPP.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Gotodlg.cpp
DEP_CPP_GOTOD=\
	".\gotodlg.h"\
	".\np.h"\
	".\resource.h"\
	".\stdafx.h"\
	

!IF  "$(CFG)" == "NPP - Win32 Release"


"$(INTDIR)\Gotodlg.obj" : $(SOURCE) $(DEP_CPP_GOTOD) "$(INTDIR)"\
 "$(INTDIR)\NPP.pch"

"$(INTDIR)\Gotodlg.sbr" : $(SOURCE) $(DEP_CPP_GOTOD) "$(INTDIR)"\
 "$(INTDIR)\NPP.pch"

"$(INTDIR)\NPP.idb" : $(SOURCE) $(DEP_CPP_GOTOD) "$(INTDIR)"\
 "$(INTDIR)\NPP.pch"


!ELSEIF  "$(CFG)" == "NPP - Win32 Debug"


"$(INTDIR)\Gotodlg.obj" : $(SOURCE) $(DEP_CPP_GOTOD) "$(INTDIR)"\
 "$(INTDIR)\NPP.pch"

"$(INTDIR)\Gotodlg.sbr" : $(SOURCE) $(DEP_CPP_GOTOD) "$(INTDIR)"\
 "$(INTDIR)\NPP.pch"

"$(INTDIR)\NPP.pdb" : $(SOURCE) $(DEP_CPP_GOTOD) "$(INTDIR)"\
 "$(INTDIR)\NPP.pch"

"$(INTDIR)\NPP.idb" : $(SOURCE) $(DEP_CPP_GOTOD) "$(INTDIR)"\
 "$(INTDIR)\NPP.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\History.cpp
DEP_CPP_HISTO=\
	".\combobar.h"\
	".\finddlg.h"\
	".\mainfrm.h"\
	".\np.h"\
	".\resource.h"\
	".\npdoc.h"\
	".\npview.h"\
	".\history.h"\
	".\stdafx.h"\
	

!IF  "$(CFG)" == "NPP - Win32 Release"


"$(INTDIR)\History.obj" : $(SOURCE) $(DEP_CPP_HISTO) "$(INTDIR)"\
 "$(INTDIR)\NPP.pch"

"$(INTDIR)\History.sbr" : $(SOURCE) $(DEP_CPP_HISTO) "$(INTDIR)"\
 "$(INTDIR)\NPP.pch"

"$(INTDIR)\NPP.idb" : $(SOURCE) $(DEP_CPP_HISTO) "$(INTDIR)"\
 "$(INTDIR)\NPP.pch"


!ELSEIF  "$(CFG)" == "NPP - Win32 Debug"


"$(INTDIR)\History.obj" : $(SOURCE) $(DEP_CPP_HISTO) "$(INTDIR)"\
 "$(INTDIR)\NPP.pch"

"$(INTDIR)\History.sbr" : $(SOURCE) $(DEP_CPP_HISTO) "$(INTDIR)"\
 "$(INTDIR)\NPP.pch"

"$(INTDIR)\NPP.pdb" : $(SOURCE) $(DEP_CPP_HISTO) "$(INTDIR)"\
 "$(INTDIR)\NPP.pch"

"$(INTDIR)\NPP.idb" : $(SOURCE) $(DEP_CPP_HISTO) "$(INTDIR)"\
 "$(INTDIR)\NPP.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Mainfrm.cpp
DEP_CPP_MAINF=\
	".\combobar.h"\
	".\mainfrm.h"\
	".\np.h"\
	".\resource.h"\
	".\npdoc.h"\
	".\npview.h"\
	".\history.h"\
	".\stdafx.h"\
	

!IF  "$(CFG)" == "NPP - Win32 Release"


"$(INTDIR)\Mainfrm.obj" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\NPP.pch"

"$(INTDIR)\Mainfrm.sbr" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\NPP.pch"

"$(INTDIR)\NPP.idb" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\NPP.pch"


!ELSEIF  "$(CFG)" == "NPP - Win32 Debug"


"$(INTDIR)\Mainfrm.obj" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\NPP.pch"

"$(INTDIR)\Mainfrm.sbr" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\NPP.pch"

"$(INTDIR)\NPP.pdb" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\NPP.pch"

"$(INTDIR)\NPP.idb" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\NPP.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Np.cpp
DEP_CPP_NP_CP=\
	".\combobar.h"\
	".\mainfrm.h"\
	".\np.h"\
	".\resource.h"\
	".\npdoc.h"\
	".\npview.h"\
	".\history.h"\
	".\stdafx.h"\
	

!IF  "$(CFG)" == "NPP - Win32 Release"


"$(INTDIR)\Np.obj" : $(SOURCE) $(DEP_CPP_NP_CP) "$(INTDIR)" "$(INTDIR)\NPP.pch"

"$(INTDIR)\Np.sbr" : $(SOURCE) $(DEP_CPP_NP_CP) "$(INTDIR)" "$(INTDIR)\NPP.pch"

"$(INTDIR)\NPP.idb" : $(SOURCE) $(DEP_CPP_NP_CP) "$(INTDIR)"\
 "$(INTDIR)\NPP.pch"


!ELSEIF  "$(CFG)" == "NPP - Win32 Debug"


"$(INTDIR)\Np.obj" : $(SOURCE) $(DEP_CPP_NP_CP) "$(INTDIR)" "$(INTDIR)\NPP.pch"

"$(INTDIR)\Np.sbr" : $(SOURCE) $(DEP_CPP_NP_CP) "$(INTDIR)" "$(INTDIR)\NPP.pch"

"$(INTDIR)\NPP.pdb" : $(SOURCE) $(DEP_CPP_NP_CP) "$(INTDIR)"\
 "$(INTDIR)\NPP.pch"

"$(INTDIR)\NPP.idb" : $(SOURCE) $(DEP_CPP_NP_CP) "$(INTDIR)"\
 "$(INTDIR)\NPP.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Npdoc.cpp
DEP_CPP_NPDOC=\
	".\np.h"\
	".\resource.h"\
	".\npdoc.h"\
	".\stdafx.h"\
	

!IF  "$(CFG)" == "NPP - Win32 Release"


"$(INTDIR)\Npdoc.obj" : $(SOURCE) $(DEP_CPP_NPDOC) "$(INTDIR)"\
 "$(INTDIR)\NPP.pch"

"$(INTDIR)\Npdoc.sbr" : $(SOURCE) $(DEP_CPP_NPDOC) "$(INTDIR)"\
 "$(INTDIR)\NPP.pch"

"$(INTDIR)\NPP.idb" : $(SOURCE) $(DEP_CPP_NPDOC) "$(INTDIR)"\
 "$(INTDIR)\NPP.pch"


!ELSEIF  "$(CFG)" == "NPP - Win32 Debug"


"$(INTDIR)\Npdoc.obj" : $(SOURCE) $(DEP_CPP_NPDOC) "$(INTDIR)"\
 "$(INTDIR)\NPP.pch"

"$(INTDIR)\Npdoc.sbr" : $(SOURCE) $(DEP_CPP_NPDOC) "$(INTDIR)"\
 "$(INTDIR)\NPP.pch"

"$(INTDIR)\NPP.pdb" : $(SOURCE) $(DEP_CPP_NPDOC) "$(INTDIR)"\
 "$(INTDIR)\NPP.pch"

"$(INTDIR)\NPP.idb" : $(SOURCE) $(DEP_CPP_NPDOC) "$(INTDIR)"\
 "$(INTDIR)\NPP.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Combobar.cpp
DEP_CPP_COMBO=\
	".\combobar.h"\
	".\finddlg.h"\
	".\mainfrm.h"\
	".\np.h"\
	".\resource.h"\
	".\npdoc.h"\
	".\npview.h"\
	".\history.h"\
	".\stdafx.h"\
	

!IF  "$(CFG)" == "NPP - Win32 Release"


"$(INTDIR)\Combobar.obj" : $(SOURCE) $(DEP_CPP_COMBO) "$(INTDIR)"\
 "$(INTDIR)\NPP.pch"

"$(INTDIR)\Combobar.sbr" : $(SOURCE) $(DEP_CPP_COMBO) "$(INTDIR)"\
 "$(INTDIR)\NPP.pch"

"$(INTDIR)\NPP.idb" : $(SOURCE) $(DEP_CPP_COMBO) "$(INTDIR)"\
 "$(INTDIR)\NPP.pch"


!ELSEIF  "$(CFG)" == "NPP - Win32 Debug"


"$(INTDIR)\Combobar.obj" : $(SOURCE) $(DEP_CPP_COMBO) "$(INTDIR)"\
 "$(INTDIR)\NPP.pch"

"$(INTDIR)\Combobar.sbr" : $(SOURCE) $(DEP_CPP_COMBO) "$(INTDIR)"\
 "$(INTDIR)\NPP.pch"

"$(INTDIR)\NPP.pdb" : $(SOURCE) $(DEP_CPP_COMBO) "$(INTDIR)"\
 "$(INTDIR)\NPP.pch"

"$(INTDIR)\NPP.idb" : $(SOURCE) $(DEP_CPP_COMBO) "$(INTDIR)"\
 "$(INTDIR)\NPP.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Npview.cpp
DEP_CPP_NPVIE=\
	".\combobar.h"\
	".\finddlg.h"\
	".\gotodlg.h"\
	".\mainfrm.h"\
	".\np.h"\
	".\resource.h"\
	".\npdoc.h"\
	".\npview.h"\
	".\history.h"\
	".\stdafx.h"\
	

!IF  "$(CFG)" == "NPP - Win32 Release"


"$(INTDIR)\Npview.obj" : $(SOURCE) $(DEP_CPP_NPVIE) "$(INTDIR)"\
 "$(INTDIR)\NPP.pch"

"$(INTDIR)\Npview.sbr" : $(SOURCE) $(DEP_CPP_NPVIE) "$(INTDIR)"\
 "$(INTDIR)\NPP.pch"

"$(INTDIR)\NPP.idb" : $(SOURCE) $(DEP_CPP_NPVIE) "$(INTDIR)"\
 "$(INTDIR)\NPP.pch"


!ELSEIF  "$(CFG)" == "NPP - Win32 Debug"


"$(INTDIR)\Npview.obj" : $(SOURCE) $(DEP_CPP_NPVIE) "$(INTDIR)"\
 "$(INTDIR)\NPP.pch"

"$(INTDIR)\Npview.sbr" : $(SOURCE) $(DEP_CPP_NPVIE) "$(INTDIR)"\
 "$(INTDIR)\NPP.pch"

"$(INTDIR)\NPP.pdb" : $(SOURCE) $(DEP_CPP_NPVIE) "$(INTDIR)"\
 "$(INTDIR)\NPP.pch"

"$(INTDIR)\NPP.idb" : $(SOURCE) $(DEP_CPP_NPVIE) "$(INTDIR)"\
 "$(INTDIR)\NPP.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Stdafx.cpp
DEP_CPP_STDAF=\
	".\stdafx.h"\
	

!IF  "$(CFG)" == "NPP - Win32 Release"

# ADD CPP /Yc

BuildCmds= \
	$(CPP) /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /FR"$(INTDIR)/" /Fp"$(INTDIR)/NPP.pch" /Yc"Stdafx.h"\
 /Fo"$(INTDIR)/" /Fd"$(OUTDIR)/NPP.pdb" /c $(SOURCE) \
	

"$(INTDIR)\Stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\Stdafx.sbr" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\NPP.idb" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\NPP.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "NPP - Win32 Debug"

# ADD CPP /Yc"Stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /D "_AFXDLL" /D "_MBCS" /FR"$(INTDIR)/" /Fp"$(INTDIR)/NPP.pch" /Yc"Stdafx.h"\
 /Fo"$(INTDIR)/" /Fd"$(OUTDIR)/NPP.pdb" /c $(SOURCE) \
	

"$(INTDIR)\Stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\Stdafx.sbr" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\NPP.pdb" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\NPP.idb" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\NPP.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
# End Target
# End Project
################################################################################
