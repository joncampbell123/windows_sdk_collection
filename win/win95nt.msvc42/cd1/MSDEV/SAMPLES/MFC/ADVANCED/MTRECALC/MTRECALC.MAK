# Microsoft Visual C++ Generated NMAKE File, Format Version 40000
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

!IF "$(CFG)" == ""
CFG=MtRecalc - Win32 Debug
!MESSAGE No configuration specified.  Defaulting to MtRecalc - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "MtRecalc - Win32 Release" && "$(CFG)" !=\
 "MtRecalc - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "MtRecalc.mak" CFG="MtRecalc - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "MtRecalc - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "MtRecalc - Win32 Debug" (based on "Win32 (x86) Application")
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
# PROP Target_Last_Scanned "MtRecalc - Win32 Debug"
MTL=mktyplib.exe
RSC=rc.exe
CPP=cl.exe

!IF  "$(CFG)" == "MtRecalc - Win32 Release"

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

ALL : "$(OUTDIR)\MtRecalc.exe"

CLEAN : 
	-@erase ".\Release\MtRecalc.exe"
	-@erase ".\Release\Stdafx.obj"
	-@erase ".\Release\Mainfrm.obj"
	-@erase ".\Release\MtRecalc.pch"
	-@erase ".\Release\Mtrecalc.obj"
	-@erase ".\Release\Mtrecalc.res"
	-@erase ".\Release\Recalcvw.obj"
	-@erase ".\Release\Recaldoc.obj"
	-@erase ".\Release\Slowcalc.obj"
	-@erase ".\Release\Speeddlg.obj"
	-@erase ".\Release\Calcthrd.obj"
	-@erase ".\Release\MtRecalc.idb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"Stdafx.h" /c
CPP_PROJ=/nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/MtRecalc.pch" /Yu"Stdafx.h" /Fo"$(INTDIR)/"\
 /Fd"$(OUTDIR)/MtRecalc.pdb" /c 
CPP_OBJS=.\Release/
CPP_SBRS=
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/Mtrecalc.res" /d "NDEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/MtRecalc.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 /nologo /subsystem:windows /machine:I386
LINK32_FLAGS=/nologo /subsystem:windows /incremental:no\
 /pdb:"$(OUTDIR)/MtRecalc.pdb" /machine:I386 /out:"$(OUTDIR)/MtRecalc.exe" 
LINK32_OBJS= \
	"$(INTDIR)/Stdafx.obj" \
	"$(INTDIR)/Mainfrm.obj" \
	"$(INTDIR)/Mtrecalc.obj" \
	"$(INTDIR)/Recalcvw.obj" \
	"$(INTDIR)/Recaldoc.obj" \
	"$(INTDIR)/Slowcalc.obj" \
	"$(INTDIR)/Speeddlg.obj" \
	"$(INTDIR)/Calcthrd.obj" \
	"$(INTDIR)/Mtrecalc.res"

"$(OUTDIR)\MtRecalc.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "MtRecalc - Win32 Debug"

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

ALL : "$(OUTDIR)\MtRecalc.exe"

CLEAN : 
	-@erase ".\Debug\MtRecalc.exe"
	-@erase ".\Debug\Stdafx.obj"
	-@erase ".\Debug\Mainfrm.obj"
	-@erase ".\Debug\MtRecalc.pch"
	-@erase ".\Debug\Mtrecalc.obj"
	-@erase ".\Debug\Mtrecalc.res"
	-@erase ".\Debug\Recalcvw.obj"
	-@erase ".\Debug\Recaldoc.obj"
	-@erase ".\Debug\Slowcalc.obj"
	-@erase ".\Debug\Speeddlg.obj"
	-@erase ".\Debug\Calcthrd.obj"
	-@erase ".\Debug\MtRecalc.ilk"
	-@erase ".\Debug\MtRecalc.pdb"
	-@erase ".\Debug\MtRecalc.idb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"Stdafx.h" /c
CPP_PROJ=/nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/MtRecalc.pch" /Yu"Stdafx.h"\
 /Fo"$(INTDIR)/" /Fd"$(OUTDIR)/MtRecalc.pdb" /c 
CPP_OBJS=.\Debug/
CPP_SBRS=
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/Mtrecalc.res" /d "_DEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/MtRecalc.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386
# ADD LINK32 /nologo /subsystem:windows /debug /machine:I386
LINK32_FLAGS=/nologo /subsystem:windows /incremental:yes\
 /pdb:"$(OUTDIR)/MtRecalc.pdb" /debug /machine:I386\
 /out:"$(OUTDIR)/MtRecalc.exe" 
LINK32_OBJS= \
	"$(INTDIR)/Stdafx.obj" \
	"$(INTDIR)/Mainfrm.obj" \
	"$(INTDIR)/Mtrecalc.obj" \
	"$(INTDIR)/Recalcvw.obj" \
	"$(INTDIR)/Recaldoc.obj" \
	"$(INTDIR)/Slowcalc.obj" \
	"$(INTDIR)/Speeddlg.obj" \
	"$(INTDIR)/Calcthrd.obj" \
	"$(INTDIR)/Mtrecalc.res"

"$(OUTDIR)\MtRecalc.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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

# Name "MtRecalc - Win32 Release"
# Name "MtRecalc - Win32 Debug"

!IF  "$(CFG)" == "MtRecalc - Win32 Release"

!ELSEIF  "$(CFG)" == "MtRecalc - Win32 Debug"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\Stdafx.cpp
DEP_CPP_STDAF=\
	".\stdafx.h"\
	

!IF  "$(CFG)" == "MtRecalc - Win32 Release"

# ADD CPP /Yc"Stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/MtRecalc.pch" /Yc"Stdafx.h" /Fo"$(INTDIR)/"\
 /Fd"$(OUTDIR)/MtRecalc.pdb" /c $(SOURCE) \
	

"$(INTDIR)\Stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\MtRecalc.idb" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\MtRecalc.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "MtRecalc - Win32 Debug"

# ADD CPP /Yc"Stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/MtRecalc.pch" /Yc"Stdafx.h"\
 /Fo"$(INTDIR)/" /Fd"$(OUTDIR)/MtRecalc.pdb" /c $(SOURCE) \
	

"$(INTDIR)\Stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\MtRecalc.pdb" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\MtRecalc.idb" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\MtRecalc.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Mainfrm.cpp
DEP_CPP_MAINF=\
	".\mainfrm.h"\
	".\mtrecalc.h"\
	".\resource.h"\
	".\stdafx.h"\
	

!IF  "$(CFG)" == "MtRecalc - Win32 Release"


"$(INTDIR)\Mainfrm.obj" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\MtRecalc.pch"

"$(INTDIR)\MtRecalc.idb" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\MtRecalc.pch"


!ELSEIF  "$(CFG)" == "MtRecalc - Win32 Debug"


"$(INTDIR)\Mainfrm.obj" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\MtRecalc.pch"

"$(INTDIR)\MtRecalc.pdb" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\MtRecalc.pch"

"$(INTDIR)\MtRecalc.idb" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\MtRecalc.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Mtrecalc.cpp
DEP_CPP_MTREC=\
	".\calcthrd.h"\
	".\mainfrm.h"\
	".\mtrecalc.h"\
	".\resource.h"\
	".\recalcvw.h"\
	".\recaldoc.h"\
	".\stdafx.h"\
	

!IF  "$(CFG)" == "MtRecalc - Win32 Release"


"$(INTDIR)\Mtrecalc.obj" : $(SOURCE) $(DEP_CPP_MTREC) "$(INTDIR)"\
 "$(INTDIR)\MtRecalc.pch"

"$(INTDIR)\MtRecalc.idb" : $(SOURCE) $(DEP_CPP_MTREC) "$(INTDIR)"\
 "$(INTDIR)\MtRecalc.pch"


!ELSEIF  "$(CFG)" == "MtRecalc - Win32 Debug"


"$(INTDIR)\Mtrecalc.obj" : $(SOURCE) $(DEP_CPP_MTREC) "$(INTDIR)"\
 "$(INTDIR)\MtRecalc.pch"

"$(INTDIR)\MtRecalc.pdb" : $(SOURCE) $(DEP_CPP_MTREC) "$(INTDIR)"\
 "$(INTDIR)\MtRecalc.pch"

"$(INTDIR)\MtRecalc.idb" : $(SOURCE) $(DEP_CPP_MTREC) "$(INTDIR)"\
 "$(INTDIR)\MtRecalc.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Mtrecalc.rc
DEP_RSC_MTRECA=\
	".\RES\MTRECALC.ICO"\
	".\RES\RECALDOC.ICO"\
	".\RES\TOOLBAR.BMP"\
	".\res\mtrecalc.rc2"\
	

"$(INTDIR)\Mtrecalc.res" : $(SOURCE) $(DEP_RSC_MTRECA) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\Recalcvw.cpp
DEP_CPP_RECAL=\
	".\calcthrd.h"\
	".\mtrecalc.h"\
	".\resource.h"\
	".\recalcvw.h"\
	".\recaldoc.h"\
	".\slowcalc.h"\
	".\stdafx.h"\
	

!IF  "$(CFG)" == "MtRecalc - Win32 Release"


"$(INTDIR)\Recalcvw.obj" : $(SOURCE) $(DEP_CPP_RECAL) "$(INTDIR)"\
 "$(INTDIR)\MtRecalc.pch"

"$(INTDIR)\MtRecalc.idb" : $(SOURCE) $(DEP_CPP_RECAL) "$(INTDIR)"\
 "$(INTDIR)\MtRecalc.pch"


!ELSEIF  "$(CFG)" == "MtRecalc - Win32 Debug"


"$(INTDIR)\Recalcvw.obj" : $(SOURCE) $(DEP_CPP_RECAL) "$(INTDIR)"\
 "$(INTDIR)\MtRecalc.pch"

"$(INTDIR)\MtRecalc.pdb" : $(SOURCE) $(DEP_CPP_RECAL) "$(INTDIR)"\
 "$(INTDIR)\MtRecalc.pch"

"$(INTDIR)\MtRecalc.idb" : $(SOURCE) $(DEP_CPP_RECAL) "$(INTDIR)"\
 "$(INTDIR)\MtRecalc.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Recaldoc.cpp
DEP_CPP_RECALD=\
	".\calcthrd.h"\
	".\mtrecalc.h"\
	".\resource.h"\
	".\recalcvw.h"\
	".\recaldoc.h"\
	".\slowcalc.h"\
	".\speeddlg.h"\
	".\stdafx.h"\
	

!IF  "$(CFG)" == "MtRecalc - Win32 Release"


"$(INTDIR)\Recaldoc.obj" : $(SOURCE) $(DEP_CPP_RECALD) "$(INTDIR)"\
 "$(INTDIR)\MtRecalc.pch"

"$(INTDIR)\MtRecalc.idb" : $(SOURCE) $(DEP_CPP_RECALD) "$(INTDIR)"\
 "$(INTDIR)\MtRecalc.pch"


!ELSEIF  "$(CFG)" == "MtRecalc - Win32 Debug"


"$(INTDIR)\Recaldoc.obj" : $(SOURCE) $(DEP_CPP_RECALD) "$(INTDIR)"\
 "$(INTDIR)\MtRecalc.pch"

"$(INTDIR)\MtRecalc.pdb" : $(SOURCE) $(DEP_CPP_RECALD) "$(INTDIR)"\
 "$(INTDIR)\MtRecalc.pch"

"$(INTDIR)\MtRecalc.idb" : $(SOURCE) $(DEP_CPP_RECALD) "$(INTDIR)"\
 "$(INTDIR)\MtRecalc.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Slowcalc.cpp
DEP_CPP_SLOWC=\
	".\calcthrd.h"\
	".\mtrecalc.h"\
	".\resource.h"\
	".\recaldoc.h"\
	".\slowcalc.h"\
	".\stdafx.h"\
	

!IF  "$(CFG)" == "MtRecalc - Win32 Release"


"$(INTDIR)\Slowcalc.obj" : $(SOURCE) $(DEP_CPP_SLOWC) "$(INTDIR)"\
 "$(INTDIR)\MtRecalc.pch"

"$(INTDIR)\MtRecalc.idb" : $(SOURCE) $(DEP_CPP_SLOWC) "$(INTDIR)"\
 "$(INTDIR)\MtRecalc.pch"


!ELSEIF  "$(CFG)" == "MtRecalc - Win32 Debug"


"$(INTDIR)\Slowcalc.obj" : $(SOURCE) $(DEP_CPP_SLOWC) "$(INTDIR)"\
 "$(INTDIR)\MtRecalc.pch"

"$(INTDIR)\MtRecalc.pdb" : $(SOURCE) $(DEP_CPP_SLOWC) "$(INTDIR)"\
 "$(INTDIR)\MtRecalc.pch"

"$(INTDIR)\MtRecalc.idb" : $(SOURCE) $(DEP_CPP_SLOWC) "$(INTDIR)"\
 "$(INTDIR)\MtRecalc.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Speeddlg.cpp
DEP_CPP_SPEED=\
	".\mtrecalc.h"\
	".\resource.h"\
	".\speeddlg.h"\
	".\stdafx.h"\
	

!IF  "$(CFG)" == "MtRecalc - Win32 Release"


"$(INTDIR)\Speeddlg.obj" : $(SOURCE) $(DEP_CPP_SPEED) "$(INTDIR)"\
 "$(INTDIR)\MtRecalc.pch"

"$(INTDIR)\MtRecalc.idb" : $(SOURCE) $(DEP_CPP_SPEED) "$(INTDIR)"\
 "$(INTDIR)\MtRecalc.pch"


!ELSEIF  "$(CFG)" == "MtRecalc - Win32 Debug"


"$(INTDIR)\Speeddlg.obj" : $(SOURCE) $(DEP_CPP_SPEED) "$(INTDIR)"\
 "$(INTDIR)\MtRecalc.pch"

"$(INTDIR)\MtRecalc.pdb" : $(SOURCE) $(DEP_CPP_SPEED) "$(INTDIR)"\
 "$(INTDIR)\MtRecalc.pch"

"$(INTDIR)\MtRecalc.idb" : $(SOURCE) $(DEP_CPP_SPEED) "$(INTDIR)"\
 "$(INTDIR)\MtRecalc.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Calcthrd.cpp
DEP_CPP_CALCT=\
	".\calcthrd.h"\
	".\mtrecalc.h"\
	".\resource.h"\
	".\recaldoc.h"\
	".\slowcalc.h"\
	".\stdafx.h"\
	

!IF  "$(CFG)" == "MtRecalc - Win32 Release"


"$(INTDIR)\Calcthrd.obj" : $(SOURCE) $(DEP_CPP_CALCT) "$(INTDIR)"\
 "$(INTDIR)\MtRecalc.pch"

"$(INTDIR)\MtRecalc.idb" : $(SOURCE) $(DEP_CPP_CALCT) "$(INTDIR)"\
 "$(INTDIR)\MtRecalc.pch"


!ELSEIF  "$(CFG)" == "MtRecalc - Win32 Debug"


"$(INTDIR)\Calcthrd.obj" : $(SOURCE) $(DEP_CPP_CALCT) "$(INTDIR)"\
 "$(INTDIR)\MtRecalc.pch"

"$(INTDIR)\MtRecalc.pdb" : $(SOURCE) $(DEP_CPP_CALCT) "$(INTDIR)"\
 "$(INTDIR)\MtRecalc.pch"

"$(INTDIR)\MtRecalc.idb" : $(SOURCE) $(DEP_CPP_CALCT) "$(INTDIR)"\
 "$(INTDIR)\MtRecalc.pch"


!ENDIF 

# End Source File
# End Target
# End Project
################################################################################
