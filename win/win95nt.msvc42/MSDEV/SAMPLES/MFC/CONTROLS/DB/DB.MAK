# Microsoft Visual C++ Generated NMAKE File, Format Version 40000
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

!IF "$(CFG)" == ""
CFG=DB - Win32 Debug
!MESSAGE No configuration specified.  Defaulting to DB - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "DB - Win32 Release" && "$(CFG)" != "DB - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "db.mak" CFG="DB - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "DB - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "DB - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
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
RSC=rc.exe
MTL=mktyplib.exe
CPP=cl.exe

!IF  "$(CFG)" == "DB - Win32 Release"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir Release
# PROP BASE Intermediate_Dir Release
# PROP BASE Target_Dir
# PROP BASE Target_Ext ocx
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 0
# PROP Output_Dir Release
# PROP Intermediate_Dir Release
# PROP Target_Dir
# PROP Target_Ext ocx
OUTDIR=.\Release
INTDIR=.\Release

ALL : "$(OUTDIR)\db.ocx" "$(OUTDIR)\regsvr32.trg"

CLEAN : 
	-@erase ".\Release\db.lib"
	-@erase ".\Release\dbppg.obj"
	-@erase ".\Release\db.pch"
	-@erase ".\Release\stdafx.obj"
	-@erase ".\Release\dbctl.obj"
	-@erase ".\Release\db.obj"
	-@erase ".\Release\db.res"
	-@erase ".\Release\db.tlb"
	-@erase ".\Release\db.exp"
	-@erase ".\Release\regsvr32.trg"
	-@erase ".\Release\db.ocx"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
CPP_PROJ=/nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/db.pch" /Yu"stdafx.h" /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\Release/
CPP_SBRS=
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/db.res" /d "NDEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/db.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 /nologo /subsystem:windows /dll /machine:I386
LINK32_FLAGS=/nologo /subsystem:windows /dll /incremental:no\
 /pdb:"$(OUTDIR)/db.pdb" /machine:I386 /def:".\db.def" /out:"$(OUTDIR)/db.ocx"\
 /implib:"$(OUTDIR)/db.lib" 
DEF_FILE= \
	".\db.def"
LINK32_OBJS= \
	"$(INTDIR)/dbppg.obj" \
	"$(INTDIR)/stdafx.obj" \
	"$(INTDIR)/dbctl.obj" \
	"$(INTDIR)/db.obj" \
	"$(INTDIR)/db.res"

"$(OUTDIR)\db.ocx" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

# Begin Custom Build - Registering OLE control...
OutDir=.\Release
TargetPath=.\Release\db.ocx
InputPath=.\Release\db.ocx
SOURCE=$(InputPath)

"$(OutDir)\regsvr32.trg" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   regsvr32 /s /c "$(TargetPath)"
   echo regsvr32 exec. time > "$(OutDir)\regsvr32.trg"

# End Custom Build

!ELSEIF  "$(CFG)" == "DB - Win32 Debug"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir Debug
# PROP BASE Intermediate_Dir Debug
# PROP BASE Target_Dir
# PROP BASE Target_Ext ocx
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 1
# PROP Output_Dir Debug
# PROP Intermediate_Dir Debug
# PROP Target_Dir
# PROP Target_Ext ocx
OUTDIR=.\Debug
INTDIR=.\Debug

ALL : "$(OUTDIR)\db.ocx" "$(OUTDIR)\regsvr32.trg"

CLEAN : 
	-@erase ".\Debug\vc40.pdb"
	-@erase ".\Debug\db.pch"
	-@erase ".\Debug\db.ilk"
	-@erase ".\Debug\dbctl.obj"
	-@erase ".\Debug\stdafx.obj"
	-@erase ".\Debug\db.obj"
	-@erase ".\Debug\dbppg.obj"
	-@erase ".\Debug\db.res"
	-@erase ".\Debug\Db.tlb"
	-@erase ".\Debug\db.lib"
	-@erase ".\Debug\db.exp"
	-@erase ".\Debug\db.pdb"
	-@erase ".\Debug\regsvr32.trg"
	-@erase ".\Debug\db.ocx"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
CPP_PROJ=/nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/db.pch" /Yu"stdafx.h" /Fo"$(INTDIR)/"\
 /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\Debug/
CPP_SBRS=
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/db.res" /d "_DEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/db.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /debug /machine:I386
# ADD LINK32 /nologo /subsystem:windows /dll /debug /machine:I386
LINK32_FLAGS=/nologo /subsystem:windows /dll /incremental:yes\
 /pdb:"$(OUTDIR)/db.pdb" /debug /machine:I386 /def:".\db.def"\
 /out:"$(OUTDIR)/db.ocx" /implib:"$(OUTDIR)/db.lib" 
DEF_FILE= \
	".\db.def"
LINK32_OBJS= \
	"$(INTDIR)/dbctl.obj" \
	"$(INTDIR)/stdafx.obj" \
	"$(INTDIR)/db.obj" \
	"$(INTDIR)/dbppg.obj" \
	"$(INTDIR)/db.res"

"$(OUTDIR)\db.ocx" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

# Begin Custom Build - Registering OLE control...
OutDir=.\Debug
TargetPath=.\Debug\db.ocx
InputPath=.\Debug\db.ocx
SOURCE=$(InputPath)

"$(OutDir)\regsvr32.trg" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   regsvr32 /s /c "$(TargetPath)"
   echo regsvr32 exec. time > "$(OutDir)\regsvr32.trg"

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

################################################################################
# Begin Target

# Name "DB - Win32 Release"
# Name "DB - Win32 Debug"

!IF  "$(CFG)" == "DB - Win32 Release"

!ELSEIF  "$(CFG)" == "DB - Win32 Debug"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\readme.txt

!IF  "$(CFG)" == "DB - Win32 Release"

!ELSEIF  "$(CFG)" == "DB - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\stdafx.cpp
DEP_CPP_STDAF=\
	".\stdafx.h"\
	

!IF  "$(CFG)" == "DB - Win32 Release"

# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/db.pch" /Yc"stdafx.h" /Fo"$(INTDIR)/" /c\
 $(SOURCE) \
	

"$(INTDIR)\stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\db.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "DB - Win32 Debug"

# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/db.pch" /Yc"stdafx.h" /Fo"$(INTDIR)/"\
 /Fd"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\db.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\db.cpp
DEP_CPP_DB_CP=\
	".\db.h"\
	".\stdafx.h"\
	

!IF  "$(CFG)" == "DB - Win32 Release"


"$(INTDIR)\db.obj" : $(SOURCE) $(DEP_CPP_DB_CP) "$(INTDIR)" "$(INTDIR)\db.pch"


!ELSEIF  "$(CFG)" == "DB - Win32 Debug"


"$(INTDIR)\db.obj" : $(SOURCE) $(DEP_CPP_DB_CP) "$(INTDIR)" "$(INTDIR)\db.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_DB_CP) "$(INTDIR)"\
 "$(INTDIR)\db.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\db.def

!IF  "$(CFG)" == "DB - Win32 Release"

!ELSEIF  "$(CFG)" == "DB - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\db.rc

!IF  "$(CFG)" == "DB - Win32 Release"

DEP_RSC_DB_RC=\
	".\db.ico"\
	".\dbctl.bmp"\
	".\resource.h"\
	".\db.rc2"\
	".\Release\db.tlb"\
	

"$(INTDIR)\db.res" : $(SOURCE) $(DEP_RSC_DB_RC) "$(INTDIR)" "$(INTDIR)\db.tlb"
   $(RSC) /l 0x409 /fo"$(INTDIR)/db.res" /i "Release" /d "NDEBUG" /d "_AFXDLL"\
 $(SOURCE)


!ELSEIF  "$(CFG)" == "DB - Win32 Debug"

DEP_RSC_DB_RC=\
	".\db.ico"\
	".\dbctl.bmp"\
	".\resource.h"\
	".\db.rc2"\
	".\Debug\Db.tlb"\
	

"$(INTDIR)\db.res" : $(SOURCE) $(DEP_RSC_DB_RC) "$(INTDIR)" "$(INTDIR)\Db.tlb"
   $(RSC) /l 0x409 /fo"$(INTDIR)/db.res" /i "Debug" /d "_DEBUG" /d "_AFXDLL"\
 $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\db.odl

!IF  "$(CFG)" == "DB - Win32 Release"


"$(OUTDIR)\db.tlb" : $(SOURCE) "$(OUTDIR)"
   $(MTL) /nologo /D "NDEBUG" /tlb "$(OUTDIR)/db.tlb" /win32 $(SOURCE)


!ELSEIF  "$(CFG)" == "DB - Win32 Debug"


"$(OUTDIR)\Db.tlb" : $(SOURCE) "$(OUTDIR)"
   $(MTL) /nologo /D "_DEBUG" /tlb "$(OUTDIR)/db.tlb" /win32 $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\dbctl.cpp
DEP_CPP_DBCTL=\
	".\db.h"\
	".\dbctl.h"\
	".\dbppg.h"\
	".\stdafx.h"\
	

!IF  "$(CFG)" == "DB - Win32 Release"


"$(INTDIR)\dbctl.obj" : $(SOURCE) $(DEP_CPP_DBCTL) "$(INTDIR)"\
 "$(INTDIR)\db.pch"


!ELSEIF  "$(CFG)" == "DB - Win32 Debug"


"$(INTDIR)\dbctl.obj" : $(SOURCE) $(DEP_CPP_DBCTL) "$(INTDIR)"\
 "$(INTDIR)\db.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_DBCTL) "$(INTDIR)"\
 "$(INTDIR)\db.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\dbppg.cpp
DEP_CPP_DBPPG=\
	".\db.h"\
	".\dbppg.h"\
	".\stdafx.h"\
	

!IF  "$(CFG)" == "DB - Win32 Release"


"$(INTDIR)\dbppg.obj" : $(SOURCE) $(DEP_CPP_DBPPG) "$(INTDIR)"\
 "$(INTDIR)\db.pch"


!ELSEIF  "$(CFG)" == "DB - Win32 Debug"


"$(INTDIR)\dbppg.obj" : $(SOURCE) $(DEP_CPP_DBPPG) "$(INTDIR)"\
 "$(INTDIR)\db.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_DBPPG) "$(INTDIR)"\
 "$(INTDIR)\db.pch"


!ENDIF 

# End Source File
# End Target
# End Project
################################################################################
