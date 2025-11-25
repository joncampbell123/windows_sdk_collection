# Microsoft Developer Studio Generated NMAKE File, Format Version 4.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

!IF "$(CFG)" == ""
CFG=ACDual - Win32 Debug
!MESSAGE No configuration specified.  Defaulting to ACDual - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "ACDual - Win32 Release" && "$(CFG)" != "ACDual - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "ACDual.mak" CFG="ACDual - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "ACDual - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "ACDual - Win32 Debug" (based on "Win32 (x86) Application")
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
# PROP Target_Last_Scanned "ACDual - Win32 Debug"
MTL=mktyplib.exe
RSC=rc.exe
CPP=cl.exe

!IF  "$(CFG)" == "ACDual - Win32 Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
OUTDIR=.\Release
INTDIR=.\Release

ALL : "$(OUTDIR)\ACDual.exe" "$(OUTDIR)\Autoclik.tlb"

CLEAN : 
	-@erase ".\Release\Autoclik.tlb"
	-@erase ".\Release\ACDual.exe"
	-@erase ".\Release\Dialogs.obj"
	-@erase ".\Release\ACDual.pch"
	-@erase ".\ACDual.h"
	-@erase ".\Release\Aclikdoc.obj"
	-@erase ".\Release\MainFrm.obj"
	-@erase ".\Release\StdAfx.obj"
	-@erase ".\Release\ChildFrm.obj"
	-@erase ".\Release\Autoclik.obj"
	-@erase ".\Release\Clikpnt.obj"
	-@erase ".\Release\initIIDs.obj"
	-@erase ".\Release\MFCDual.obj"
	-@erase ".\Release\Aclikvw.obj"
	-@erase ".\Release\Autoclik.res"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
CPP_PROJ=/nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/ACDual.pch" /Yu"stdafx.h" /Fo"$(INTDIR)/" /c\
 
CPP_OBJS=.\Release/
CPP_SBRS=
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/Autoclik.res" /d "NDEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/ACDual.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 /nologo /subsystem:windows /machine:I386
LINK32_FLAGS=/nologo /subsystem:windows /incremental:no\
 /pdb:"$(OUTDIR)/ACDual.pdb" /machine:I386 /out:"$(OUTDIR)/ACDual.exe" 
LINK32_OBJS= \
	"$(INTDIR)/Dialogs.obj" \
	"$(INTDIR)/Aclikdoc.obj" \
	"$(INTDIR)/MainFrm.obj" \
	"$(INTDIR)/StdAfx.obj" \
	"$(INTDIR)/ChildFrm.obj" \
	"$(INTDIR)/Autoclik.obj" \
	"$(INTDIR)/Clikpnt.obj" \
	"$(INTDIR)/initIIDs.obj" \
	"$(INTDIR)/MFCDual.obj" \
	"$(INTDIR)/Aclikvw.obj" \
	"$(INTDIR)/Autoclik.res"

"$(OUTDIR)\ACDual.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "ACDual - Win32 Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
OUTDIR=.\Debug
INTDIR=.\Debug

ALL : "$(OUTDIR)\ACDual.exe" "$(OUTDIR)\Autoclik.tlb"

CLEAN : 
	-@erase ".\Debug\vc40.pdb"
	-@erase ".\Debug\ACDual.pch"
	-@erase ".\ACDual.h"
	-@erase ".\Debug\vc40.idb"
	-@erase ".\Debug\Autoclik.tlb"
	-@erase ".\Debug\ACDual.exe"
	-@erase ".\Debug\ChildFrm.obj"
	-@erase ".\Debug\StdAfx.obj"
	-@erase ".\Debug\Aclikvw.obj"
	-@erase ".\Debug\Autoclik.obj"
	-@erase ".\Debug\initIIDs.obj"
	-@erase ".\Debug\Dialogs.obj"
	-@erase ".\Debug\MFCDual.obj"
	-@erase ".\Debug\MainFrm.obj"
	-@erase ".\Debug\Aclikdoc.obj"
	-@erase ".\Debug\Clikpnt.obj"
	-@erase ".\Debug\Autoclik.res"
	-@erase ".\Debug\ACDual.ilk"
	-@erase ".\Debug\ACDual.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
CPP_PROJ=/nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/ACDual.pch" /Yu"stdafx.h" /Fo"$(INTDIR)/"\
 /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\Debug/
CPP_SBRS=
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/Autoclik.res" /d "_DEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/ACDual.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386
# ADD LINK32 /nologo /subsystem:windows /debug /machine:I386
LINK32_FLAGS=/nologo /subsystem:windows /incremental:yes\
 /pdb:"$(OUTDIR)/ACDual.pdb" /debug /machine:I386 /out:"$(OUTDIR)/ACDual.exe" 
LINK32_OBJS= \
	"$(INTDIR)/ChildFrm.obj" \
	"$(INTDIR)/StdAfx.obj" \
	"$(INTDIR)/Aclikvw.obj" \
	"$(INTDIR)/Autoclik.obj" \
	"$(INTDIR)/initIIDs.obj" \
	"$(INTDIR)/Dialogs.obj" \
	"$(INTDIR)/MFCDual.obj" \
	"$(INTDIR)/MainFrm.obj" \
	"$(INTDIR)/Aclikdoc.obj" \
	"$(INTDIR)/Clikpnt.obj" \
	"$(INTDIR)/Autoclik.res"

"$(OUTDIR)\ACDual.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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

# Name "ACDual - Win32 Release"
# Name "ACDual - Win32 Debug"

!IF  "$(CFG)" == "ACDual - Win32 Release"

!ELSEIF  "$(CFG)" == "ACDual - Win32 Debug"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\ReadMe.txt

!IF  "$(CFG)" == "ACDual - Win32 Release"

!ELSEIF  "$(CFG)" == "ACDual - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\StdAfx.cpp
DEP_CPP_STDAF=\
	".\StdAfx.h"\
	".\ACDual.h"\
	".\mfcdual.h"\
	

!IF  "$(CFG)" == "ACDual - Win32 Release"

# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/ACDual.pch" /Yc"stdafx.h" /Fo"$(INTDIR)/" /c\
 $(SOURCE) \
	

"$(INTDIR)\StdAfx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)" ".\ACDual.h"
   $(BuildCmds)

"$(INTDIR)\ACDual.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)" ".\ACDual.h"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "ACDual - Win32 Debug"

# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/ACDual.pch" /Yc"stdafx.h" /Fo"$(INTDIR)/"\
 /Fd"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\StdAfx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)" ".\ACDual.h"
   $(BuildCmds)

"$(INTDIR)\ACDual.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)" ".\ACDual.h"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\MainFrm.cpp
DEP_CPP_MAINF=\
	".\StdAfx.h"\
	".\AutoClik.h"\
	".\MainFrm.h"\
	".\ACDual.h"\
	".\mfcdual.h"\
	

"$(INTDIR)\MainFrm.obj" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\ACDual.pch" ".\ACDual.h"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\ChildFrm.cpp
DEP_CPP_CHILD=\
	".\StdAfx.h"\
	".\AutoClik.h"\
	".\ChildFrm.h"\
	".\ACDual.h"\
	".\mfcdual.h"\
	

"$(INTDIR)\ChildFrm.obj" : $(SOURCE) $(DEP_CPP_CHILD) "$(INTDIR)"\
 "$(INTDIR)\ACDual.pch" ".\ACDual.h"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\Autoclik.cpp
DEP_CPP_AUTOC=\
	".\StdAfx.h"\
	".\AutoClik.h"\
	".\MainFrm.h"\
	".\ChildFrm.h"\
	".\AClikDoc.h"\
	".\AClikVw.h"\
	".\ACDual.h"\
	".\mfcdual.h"\
	

"$(INTDIR)\Autoclik.obj" : $(SOURCE) $(DEP_CPP_AUTOC) "$(INTDIR)"\
 "$(INTDIR)\ACDual.pch" ".\ACDual.h"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\Autoclik.odl

!IF  "$(CFG)" == "ACDual - Win32 Release"

# ADD MTL /h "ACDual.h"

BuildCmds= \
	$(MTL) /nologo /D "NDEBUG" /tlb "$(OUTDIR)/Autoclik.tlb" /h "ACDual.h" /win32\
 $(SOURCE) \
	

"$(OUTDIR)\Autoclik.tlb" : $(SOURCE) "$(OUTDIR)"
   $(BuildCmds)

".\ACDual.h" : $(SOURCE) "$(OUTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "ACDual - Win32 Debug"

# ADD MTL /h "ACDual.h"

BuildCmds= \
	$(MTL) /nologo /D "_DEBUG" /tlb "$(OUTDIR)/Autoclik.tlb" /h "ACDual.h" /win32\
 $(SOURCE) \
	

"$(OUTDIR)\Autoclik.tlb" : $(SOURCE) "$(OUTDIR)"
   $(BuildCmds)

".\ACDual.h" : $(SOURCE) "$(OUTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Autoclik.rc
DEP_RSC_AUTOCL=\
	".\res\AutoClik.ico"\
	".\res\AClikDoc.ico"\
	".\res\Toolbar.bmp"\
	".\res\AutoClik.rc2"\
	

!IF  "$(CFG)" == "ACDual - Win32 Release"


"$(INTDIR)\Autoclik.res" : $(SOURCE) $(DEP_RSC_AUTOCL) "$(INTDIR)"
   $(RSC) /l 0x409 /fo"$(INTDIR)/Autoclik.res" /i "Release" /d "NDEBUG" /d\
 "_AFXDLL" $(SOURCE)


!ELSEIF  "$(CFG)" == "ACDual - Win32 Debug"


"$(INTDIR)\Autoclik.res" : $(SOURCE) $(DEP_RSC_AUTOCL) "$(INTDIR)"
   $(RSC) /l 0x409 /fo"$(INTDIR)/Autoclik.res" /i "Debug" /d "_DEBUG" /d\
 "_AFXDLL" $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Aclikdoc.cpp
DEP_CPP_ACLIK=\
	".\StdAfx.h"\
	".\AutoClik.h"\
	".\AClikDoc.h"\
	".\Dialogs.h"\
	".\ClikPnt.h"\
	".\ACDual.h"\
	".\mfcdual.h"\
	

"$(INTDIR)\Aclikdoc.obj" : $(SOURCE) $(DEP_CPP_ACLIK) "$(INTDIR)"\
 "$(INTDIR)\ACDual.pch" ".\ACDual.h"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\Aclikvw.cpp
DEP_CPP_ACLIKV=\
	".\StdAfx.h"\
	".\AutoClik.h"\
	".\AClikDoc.h"\
	".\AClikVw.h"\
	".\ACDual.h"\
	".\mfcdual.h"\
	

"$(INTDIR)\Aclikvw.obj" : $(SOURCE) $(DEP_CPP_ACLIKV) "$(INTDIR)"\
 "$(INTDIR)\ACDual.pch" ".\ACDual.h"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\Dialogs.cpp
DEP_CPP_DIALO=\
	".\StdAfx.h"\
	".\AutoClik.h"\
	".\Dialogs.h"\
	".\ACDual.h"\
	".\mfcdual.h"\
	

"$(INTDIR)\Dialogs.obj" : $(SOURCE) $(DEP_CPP_DIALO) "$(INTDIR)"\
 "$(INTDIR)\ACDual.pch" ".\ACDual.h"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\Clikpnt.cpp
DEP_CPP_CLIKP=\
	".\StdAfx.h"\
	".\AutoClik.h"\
	".\ClikPnt.h"\
	".\ACDual.h"\
	".\mfcdual.h"\
	

"$(INTDIR)\Clikpnt.obj" : $(SOURCE) $(DEP_CPP_CLIKP) "$(INTDIR)"\
 "$(INTDIR)\ACDual.pch" ".\ACDual.h"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\initIIDs.cpp
DEP_CPP_INITI=\
	".\ACDual.h"\
	

!IF  "$(CFG)" == "ACDual - Win32 Release"

# SUBTRACT CPP /YX /Yc /Yu

"$(INTDIR)\initIIDs.obj" : $(SOURCE) $(DEP_CPP_INITI) "$(INTDIR)" ".\ACDual.h"
   $(CPP) /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /Fo"$(INTDIR)/" /c $(SOURCE)


!ELSEIF  "$(CFG)" == "ACDual - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

"$(INTDIR)\initIIDs.obj" : $(SOURCE) $(DEP_CPP_INITI) "$(INTDIR)" ".\ACDual.h"
   $(CPP) /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /D "_AFXDLL" /D "_MBCS" /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\MFCDual.cpp
DEP_CPP_MFCDU=\
	".\StdAfx.h"\
	".\AutoClik.h"\
	".\ACDual.h"\
	".\mfcdual.h"\
	

"$(INTDIR)\MFCDual.obj" : $(SOURCE) $(DEP_CPP_MFCDU) "$(INTDIR)"\
 "$(INTDIR)\ACDual.pch" ".\ACDual.h"


# End Source File
# End Target
# End Project
################################################################################
