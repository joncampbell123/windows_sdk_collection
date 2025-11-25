# Microsoft Developer Studio Generated NMAKE File, Format Version 4.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

!IF "$(CFG)" == ""
CFG=OdbcInfo - Win32 Debug
!MESSAGE No configuration specified.  Defaulting to OdbcInfo - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "OdbcInfo - Win32 Release" && "$(CFG)" !=\
 "OdbcInfo - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "odbcinfo.mak" CFG="OdbcInfo - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "OdbcInfo - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "OdbcInfo - Win32 Debug" (based on "Win32 (x86) Application")
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
# PROP Target_Last_Scanned "OdbcInfo - Win32 Debug"
CPP=cl.exe
MTL=mktyplib.exe
RSC=rc.exe

!IF  "$(CFG)" == "OdbcInfo - Win32 Release"

# PROP BASE Use_MFC 5
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 5
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
OUTDIR=.\Release
INTDIR=.\Release

ALL : "$(OUTDIR)\odbcinfo.exe"

CLEAN : 
	-@erase ".\Release\odbcinfo.exe"
	-@erase ".\Release\catsets.obj"
	-@erase ".\Release\odbcinfo.pch"
	-@erase ".\Release\MySheet.obj"
	-@erase ".\Release\DrvInfo.obj"
	-@erase ".\Release\AboutDlg.obj"
	-@erase ".\Release\OdbcInfo.obj"
	-@erase ".\Release\StdAfx.obj"
	-@erase ".\Release\OdbcInfo.res"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Yu"stdafx.h" /c
# ADD CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Yu"stdafx.h" /c
CPP_PROJ=/nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_MBCS" /Fp"$(INTDIR)/odbcinfo.pch" /Yu"stdafx.h" /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\Release/
CPP_SBRS=
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/OdbcInfo.res" /d "NDEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/odbcinfo.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 /nologo /subsystem:windows /machine:I386
LINK32_FLAGS=/nologo /subsystem:windows /incremental:no\
 /pdb:"$(OUTDIR)/odbcinfo.pdb" /machine:I386 /out:"$(OUTDIR)/odbcinfo.exe" 
LINK32_OBJS= \
	"$(INTDIR)/catsets.obj" \
	"$(INTDIR)/MySheet.obj" \
	"$(INTDIR)/DrvInfo.obj" \
	"$(INTDIR)/AboutDlg.obj" \
	"$(INTDIR)/OdbcInfo.obj" \
	"$(INTDIR)/StdAfx.obj" \
	"$(INTDIR)/OdbcInfo.res"

"$(OUTDIR)\odbcinfo.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "OdbcInfo - Win32 Debug"

# PROP BASE Use_MFC 5
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 5
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
OUTDIR=.\Debug
INTDIR=.\Debug

ALL : "$(OUTDIR)\odbcinfo.exe" "$(OUTDIR)\odbcinfo.bsc"

CLEAN : 
	-@erase ".\Debug\vc40.pdb"
	-@erase ".\Debug\odbcinfo.pch"
	-@erase ".\Debug\vc40.idb"
	-@erase ".\Debug\odbcinfo.bsc"
	-@erase ".\Debug\OdbcInfo.sbr"
	-@erase ".\Debug\catsets.sbr"
	-@erase ".\Debug\MySheet.sbr"
	-@erase ".\Debug\DrvInfo.sbr"
	-@erase ".\Debug\StdAfx.sbr"
	-@erase ".\Debug\AboutDlg.sbr"
	-@erase ".\Debug\odbcinfo.exe"
	-@erase ".\Debug\AboutDlg.obj"
	-@erase ".\Debug\OdbcInfo.obj"
	-@erase ".\Debug\catsets.obj"
	-@erase ".\Debug\MySheet.obj"
	-@erase ".\Debug\DrvInfo.obj"
	-@erase ".\Debug\StdAfx.obj"
	-@erase ".\Debug\OdbcInfo.res"
	-@erase ".\Debug\odbcinfo.ilk"
	-@erase ".\Debug\odbcinfo.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /Yu"stdafx.h" /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /FR /Yu"stdafx.h" /c
CPP_PROJ=/nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /D "_MBCS" /FR"$(INTDIR)/" /Fp"$(INTDIR)/odbcinfo.pch" /Yu"stdafx.h"\
 /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\Debug/
CPP_SBRS=.\Debug/
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/OdbcInfo.res" /d "_DEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/odbcinfo.bsc" 
BSC32_SBRS= \
	"$(INTDIR)/OdbcInfo.sbr" \
	"$(INTDIR)/catsets.sbr" \
	"$(INTDIR)/MySheet.sbr" \
	"$(INTDIR)/DrvInfo.sbr" \
	"$(INTDIR)/StdAfx.sbr" \
	"$(INTDIR)/AboutDlg.sbr"

"$(OUTDIR)\odbcinfo.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386
# ADD LINK32 /nologo /subsystem:windows /debug /machine:I386
LINK32_FLAGS=/nologo /subsystem:windows /incremental:yes\
 /pdb:"$(OUTDIR)/odbcinfo.pdb" /debug /machine:I386\
 /out:"$(OUTDIR)/odbcinfo.exe" 
LINK32_OBJS= \
	"$(INTDIR)/AboutDlg.obj" \
	"$(INTDIR)/OdbcInfo.obj" \
	"$(INTDIR)/catsets.obj" \
	"$(INTDIR)/MySheet.obj" \
	"$(INTDIR)/DrvInfo.obj" \
	"$(INTDIR)/StdAfx.obj" \
	"$(INTDIR)/OdbcInfo.res"

"$(OUTDIR)\odbcinfo.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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

# Name "OdbcInfo - Win32 Release"
# Name "OdbcInfo - Win32 Debug"

!IF  "$(CFG)" == "OdbcInfo - Win32 Release"

!ELSEIF  "$(CFG)" == "OdbcInfo - Win32 Debug"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\ReadMe.txt

!IF  "$(CFG)" == "OdbcInfo - Win32 Release"

!ELSEIF  "$(CFG)" == "OdbcInfo - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\OdbcInfo.cpp
DEP_CPP_ODBCI=\
	".\StdAfx.h"\
	".\OdbcInfo.h"\
	".\MySheet.h"\
	".\catsets.h"\
	".\DrvInfo.h"\
	

!IF  "$(CFG)" == "OdbcInfo - Win32 Release"


"$(INTDIR)\OdbcInfo.obj" : $(SOURCE) $(DEP_CPP_ODBCI) "$(INTDIR)"\
 "$(INTDIR)\odbcinfo.pch"


!ELSEIF  "$(CFG)" == "OdbcInfo - Win32 Debug"


"$(INTDIR)\OdbcInfo.obj" : $(SOURCE) $(DEP_CPP_ODBCI) "$(INTDIR)"\
 "$(INTDIR)\odbcinfo.pch"

"$(INTDIR)\OdbcInfo.sbr" : $(SOURCE) $(DEP_CPP_ODBCI) "$(INTDIR)"\
 "$(INTDIR)\odbcinfo.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\StdAfx.cpp
DEP_CPP_STDAF=\
	".\StdAfx.h"\
	

!IF  "$(CFG)" == "OdbcInfo - Win32 Release"

# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS"\
 /Fp"$(INTDIR)/odbcinfo.pch" /Yc"stdafx.h" /Fo"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\StdAfx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\odbcinfo.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "OdbcInfo - Win32 Debug"

# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /D "_MBCS" /FR"$(INTDIR)/" /Fp"$(INTDIR)/odbcinfo.pch" /Yc"stdafx.h"\
 /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\StdAfx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\StdAfx.sbr" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\odbcinfo.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\OdbcInfo.rc
DEP_RSC_ODBCIN=\
	".\res\OdbcInfo.ico"\
	".\res\bmp00002.bmp"\
	".\res\bitmap1.bmp"\
	".\res\OdbcInfo.rc2"\
	

"$(INTDIR)\OdbcInfo.res" : $(SOURCE) $(DEP_RSC_ODBCIN) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\DrvInfo.cpp
DEP_CPP_DRVIN=\
	".\StdAfx.h"\
	".\OdbcInfo.h"\
	".\catsets.h"\
	".\DrvInfo.h"\
	

!IF  "$(CFG)" == "OdbcInfo - Win32 Release"


"$(INTDIR)\DrvInfo.obj" : $(SOURCE) $(DEP_CPP_DRVIN) "$(INTDIR)"\
 "$(INTDIR)\odbcinfo.pch"


!ELSEIF  "$(CFG)" == "OdbcInfo - Win32 Debug"


"$(INTDIR)\DrvInfo.obj" : $(SOURCE) $(DEP_CPP_DRVIN) "$(INTDIR)"\
 "$(INTDIR)\odbcinfo.pch"

"$(INTDIR)\DrvInfo.sbr" : $(SOURCE) $(DEP_CPP_DRVIN) "$(INTDIR)"\
 "$(INTDIR)\odbcinfo.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\MySheet.cpp
DEP_CPP_MYSHE=\
	".\StdAfx.h"\
	".\OdbcInfo.h"\
	".\MySheet.h"\
	".\catsets.h"\
	".\DrvInfo.h"\
	".\AboutDlg.h"\
	

!IF  "$(CFG)" == "OdbcInfo - Win32 Release"


"$(INTDIR)\MySheet.obj" : $(SOURCE) $(DEP_CPP_MYSHE) "$(INTDIR)"\
 "$(INTDIR)\odbcinfo.pch"


!ELSEIF  "$(CFG)" == "OdbcInfo - Win32 Debug"


"$(INTDIR)\MySheet.obj" : $(SOURCE) $(DEP_CPP_MYSHE) "$(INTDIR)"\
 "$(INTDIR)\odbcinfo.pch"

"$(INTDIR)\MySheet.sbr" : $(SOURCE) $(DEP_CPP_MYSHE) "$(INTDIR)"\
 "$(INTDIR)\odbcinfo.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\catsets.cpp
DEP_CPP_CATSE=\
	".\catsets.h"\
	".\StdAfx.h"\
	

!IF  "$(CFG)" == "OdbcInfo - Win32 Release"


"$(INTDIR)\catsets.obj" : $(SOURCE) $(DEP_CPP_CATSE) "$(INTDIR)"\
 "$(INTDIR)\odbcinfo.pch"


!ELSEIF  "$(CFG)" == "OdbcInfo - Win32 Debug"


"$(INTDIR)\catsets.obj" : $(SOURCE) $(DEP_CPP_CATSE) "$(INTDIR)"\
 "$(INTDIR)\odbcinfo.pch"

"$(INTDIR)\catsets.sbr" : $(SOURCE) $(DEP_CPP_CATSE) "$(INTDIR)"\
 "$(INTDIR)\odbcinfo.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\AboutDlg.cpp
DEP_CPP_ABOUT=\
	".\StdAfx.h"\
	".\OdbcInfo.h"\
	".\AboutDlg.h"\
	

!IF  "$(CFG)" == "OdbcInfo - Win32 Release"


"$(INTDIR)\AboutDlg.obj" : $(SOURCE) $(DEP_CPP_ABOUT) "$(INTDIR)"\
 "$(INTDIR)\odbcinfo.pch"


!ELSEIF  "$(CFG)" == "OdbcInfo - Win32 Debug"


"$(INTDIR)\AboutDlg.obj" : $(SOURCE) $(DEP_CPP_ABOUT) "$(INTDIR)"\
 "$(INTDIR)\odbcinfo.pch"

"$(INTDIR)\AboutDlg.sbr" : $(SOURCE) $(DEP_CPP_ABOUT) "$(INTDIR)"\
 "$(INTDIR)\odbcinfo.pch"


!ENDIF 

# End Source File
# End Target
# End Project
################################################################################
