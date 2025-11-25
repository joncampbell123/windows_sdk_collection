# Microsoft Visual C++ Generated NMAKE File, Format Version 40000
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

!IF "$(CFG)" == ""
CFG=oleview - Win32 Debug
!MESSAGE No configuration specified.  Defaulting to oleview - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "oleview - Win32 Debug" && "$(CFG)" !=\
 "oleview - Win32 Release"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "oleview.mak" CFG="oleview - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "oleview - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE "oleview - Win32 Release" (based on "Win32 (x86) Application")
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
# PROP Target_Last_Scanned "oleview - Win32 Release"
CPP=cl.exe
RSC=rc.exe
MTL=mktyplib.exe

!IF  "$(CFG)" == "oleview - Win32 Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir Debug
# PROP BASE Intermediate_Dir Debug
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir Debug
# PROP Intermediate_Dir Debug
OUTDIR=.\Debug
INTDIR=.\Debug

ALL : "$(OUTDIR)\oleview.exe" "$(OUTDIR)\oleview.bsc"

CLEAN : 
	-@erase ".\Debug\vc40.pdb"
	-@erase ".\Debug\oleview.pch"
	-@erase ".\Debug\oleview.bsc"
	-@erase ".\Debug\obj_vw.sbr"
	-@erase ".\Debug\mainfrm.sbr"
	-@erase ".\Debug\shadow.sbr"
	-@erase ".\Debug\stdafx.sbr"
	-@erase ".\Debug\view.sbr"
	-@erase ".\Debug\regview.sbr"
	-@erase ".\Debug\doc.sbr"
	-@erase ".\Debug\util.sbr"
	-@erase ".\Debug\oleview.sbr"
	-@erase ".\Debug\oleview.exe"
	-@erase ".\Debug\obj_vw.obj"
	-@erase ".\Debug\mainfrm.obj"
	-@erase ".\Debug\shadow.obj"
	-@erase ".\Debug\stdafx.obj"
	-@erase ".\Debug\view.obj"
	-@erase ".\Debug\regview.obj"
	-@erase ".\Debug\doc.obj"
	-@erase ".\Debug\util.obj"
	-@erase ".\Debug\oleview.obj"
	-@erase ".\Debug\oleview.res"
	-@erase ".\Debug\oleview.ilk"
	-@erase ".\Debug\oleview.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MD /W3 /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_AFXDLL" /FR /Yu"stdafx.h" /c
# ADD CPP /nologo /MDd /W4 /Gm /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_AFXDLL" /FR /Yu"stdafx.h" /c
# SUBTRACT CPP /WX
CPP_PROJ=/nologo /MDd /W4 /Gm /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS"\
 /D "_MBCS" /D "_AFXDLL" /FR"$(INTDIR)/" /Fp"$(INTDIR)/oleview.pch"\
 /Yu"stdafx.h" /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\Debug/
CPP_SBRS=.\Debug/
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL" /d "WIN32"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/oleview.res" /d "_DEBUG" /d "_AFXDLL" /d\
 "WIN32" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/oleview.bsc" 
BSC32_SBRS= \
	"$(INTDIR)/obj_vw.sbr" \
	"$(INTDIR)/mainfrm.sbr" \
	"$(INTDIR)/shadow.sbr" \
	"$(INTDIR)/stdafx.sbr" \
	"$(INTDIR)/view.sbr" \
	"$(INTDIR)/regview.sbr" \
	"$(INTDIR)/doc.sbr" \
	"$(INTDIR)/util.sbr" \
	"$(INTDIR)/oleview.sbr"

"$(OUTDIR)\oleview.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386
# SUBTRACT BASE LINK32 /pdb:none
# ADD LINK32 version.lib /nologo /subsystem:windows /debug /machine:I386
# SUBTRACT LINK32 /pdb:none
LINK32_FLAGS=version.lib /nologo /subsystem:windows /incremental:yes\
 /pdb:"$(OUTDIR)/oleview.pdb" /debug /machine:I386\
 /out:"$(OUTDIR)/oleview.exe" 
LINK32_OBJS= \
	"$(INTDIR)/obj_vw.obj" \
	"$(INTDIR)/mainfrm.obj" \
	"$(INTDIR)/shadow.obj" \
	"$(INTDIR)/stdafx.obj" \
	"$(INTDIR)/view.obj" \
	"$(INTDIR)/regview.obj" \
	"$(INTDIR)/doc.obj" \
	"$(INTDIR)/util.obj" \
	"$(INTDIR)/oleview.obj" \
	"$(INTDIR)/oleview.res"

"$(OUTDIR)\oleview.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "oleview - Win32 Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir Release
# PROP BASE Intermediate_Dir Release
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir Release
# PROP Intermediate_Dir Release
OUTDIR=.\Release
INTDIR=.\Release

ALL : "$(OUTDIR)\oleview.exe"

CLEAN : 
	-@erase ".\Release\oleview.exe"
	-@erase ".\Release\view.obj"
	-@erase ".\Release\oleview.pch"
	-@erase ".\Release\util.obj"
	-@erase ".\Release\mainfrm.obj"
	-@erase ".\Release\regview.obj"
	-@erase ".\Release\oleview.obj"
	-@erase ".\Release\obj_vw.obj"
	-@erase ".\Release\shadow.obj"
	-@erase ".\Release\doc.obj"
	-@erase ".\Release\stdafx.obj"
	-@erase ".\Release\oleview.res"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_AFXDLL" /FR /Yu"stdafx.h" /c
# ADD CPP /nologo /MD /W4 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_AFXDLL" /Yu"stdafx.h" /c
# SUBTRACT CPP /WX /Fr
CPP_PROJ=/nologo /MD /W4 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D\
 "_MBCS" /D "_AFXDLL" /Fp"$(INTDIR)/oleview.pch" /Yu"stdafx.h" /Fo"$(INTDIR)/"\
 /c 
CPP_OBJS=.\Release/
CPP_SBRS=
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL" /d "WIN32"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/oleview.res" /d "NDEBUG" /d "_AFXDLL" /d\
 "WIN32" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/oleview.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# SUBTRACT BASE LINK32 /pdb:none
# ADD LINK32 version.lib /nologo /subsystem:windows /machine:I386
# SUBTRACT LINK32 /pdb:none
LINK32_FLAGS=version.lib /nologo /subsystem:windows /incremental:no\
 /pdb:"$(OUTDIR)/oleview.pdb" /machine:I386 /out:"$(OUTDIR)/oleview.exe" 
LINK32_OBJS= \
	"$(INTDIR)/view.obj" \
	"$(INTDIR)/util.obj" \
	"$(INTDIR)/mainfrm.obj" \
	"$(INTDIR)/regview.obj" \
	"$(INTDIR)/oleview.obj" \
	"$(INTDIR)/obj_vw.obj" \
	"$(INTDIR)/shadow.obj" \
	"$(INTDIR)/doc.obj" \
	"$(INTDIR)/stdafx.obj" \
	"$(INTDIR)/oleview.res"

"$(OUTDIR)\oleview.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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

MTL_PROJ=
################################################################################
# Begin Target

# Name "oleview - Win32 Debug"
# Name "oleview - Win32 Release"

!IF  "$(CFG)" == "oleview - Win32 Debug"

!ELSEIF  "$(CFG)" == "oleview - Win32 Release"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\stdafx.cpp
DEP_CPP_STDAF=\
	".\stdafx.h"\
	

!IF  "$(CFG)" == "oleview - Win32 Debug"

# ADD BASE CPP /Yc"stdafx.h"
# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MDd /W4 /Gm /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS"\
 /D "_MBCS" /D "_AFXDLL" /FR"$(INTDIR)/" /Fp"$(INTDIR)/oleview.pch"\
 /Yc"stdafx.h" /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\stdafx.sbr" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\oleview.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "oleview - Win32 Release"

# ADD BASE CPP /Yc"stdafx.h"
# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MD /W4 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS"\
 /D "_AFXDLL" /Fp"$(INTDIR)/oleview.pch" /Yc"stdafx.h" /Fo"$(INTDIR)/" /c\
 $(SOURCE) \
	

"$(INTDIR)\stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\oleview.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\mainfrm.cpp
DEP_CPP_MAINF=\
	".\stdafx.h"\
	".\oleview.h"\
	".\mainfrm.h"\
	".\shadow.h"\
	".\obj_vw.h"\
	".\iviewers\iviewers.h"\
	".\util.h"\
	".\doc.h"\
	".\regview.h"\
	".\view.h"\
	

!IF  "$(CFG)" == "oleview - Win32 Debug"


"$(INTDIR)\mainfrm.obj" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\oleview.pch"

"$(INTDIR)\mainfrm.sbr" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\oleview.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\oleview.pch"


!ELSEIF  "$(CFG)" == "oleview - Win32 Release"


"$(INTDIR)\mainfrm.obj" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\oleview.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\oleview.rc
DEP_RSC_OLE2V=\
	".\res\oleview.ico"\
	".\res\automati.ico"\
	".\res\dragdrop.ico"\
	".\res\noicon.ico"\
	".\res\icon1.ico"\
	".\res\ole2_1.ico"\
	".\res\ico00001.ico"\
	".\res\ico00002.ico"\
	".\res\toolbar.bmp"\
	".\res\automati.bmp"\
	".\res\containe.bmp"\
	".\res\bmp00002.bmp"\
	".\res\object.bmp"\
	".\res\object32.bmp"\
	".\res\control.bmp"\
	".\res\obj_c32.bmp"\
	".\res\object_i.bmp"\
	".\res\bmp00001.bmp"\
	".\res\insrt132.bmp"\
	".\res\insrt_32.bmp"\
	".\res\question.bmp"\
	".\res\questn32.bmp"\
	".\res\imagelis.bmp"\
	".\res\bitmap1.bmp"\
	".\res\oleview.rc2"\
	

"$(INTDIR)\oleview.res" : $(SOURCE) $(DEP_RSC_OLE2V) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\obj_vw.cpp
DEP_CPP_OBJ_V=\
	".\stdafx.h"\
	".\oleview.h"\
	".\shadow.h"\
	".\obj_vw.h"\
	".\iviewers\iviewers.h"\
	".\util.h"\
	".\mainfrm.h"\
	".\doc.h"\
	".\regview.h"\
	".\view.h"\
	

!IF  "$(CFG)" == "oleview - Win32 Debug"


"$(INTDIR)\obj_vw.obj" : $(SOURCE) $(DEP_CPP_OBJ_V) "$(INTDIR)"\
 "$(INTDIR)\oleview.pch"

"$(INTDIR)\obj_vw.sbr" : $(SOURCE) $(DEP_CPP_OBJ_V) "$(INTDIR)"\
 "$(INTDIR)\oleview.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_OBJ_V) "$(INTDIR)"\
 "$(INTDIR)\oleview.pch"


!ELSEIF  "$(CFG)" == "oleview - Win32 Release"


"$(INTDIR)\obj_vw.obj" : $(SOURCE) $(DEP_CPP_OBJ_V) "$(INTDIR)"\
 "$(INTDIR)\oleview.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\view.cpp
DEP_CPP_VIEW_=\
	".\stdafx.h"\
	".\oleview.h"\
	".\iviewers\iviewers.h"\
	".\util.h"\
	".\mainfrm.h"\
	".\doc.h"\
	".\regview.h"\
	".\obj_vw.h"\
	".\view.h"\
	

!IF  "$(CFG)" == "oleview - Win32 Debug"


"$(INTDIR)\view.obj" : $(SOURCE) $(DEP_CPP_VIEW_) "$(INTDIR)"\
 "$(INTDIR)\oleview.pch"

"$(INTDIR)\view.sbr" : $(SOURCE) $(DEP_CPP_VIEW_) "$(INTDIR)"\
 "$(INTDIR)\oleview.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_VIEW_) "$(INTDIR)"\
 "$(INTDIR)\oleview.pch"


!ELSEIF  "$(CFG)" == "oleview - Win32 Release"


"$(INTDIR)\view.obj" : $(SOURCE) $(DEP_CPP_VIEW_) "$(INTDIR)"\
 "$(INTDIR)\oleview.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\util.cpp
DEP_CPP_UTIL_=\
	".\stdafx.h"\
	".\util.h"\
	".\shadow.h"\
	

!IF  "$(CFG)" == "oleview - Win32 Debug"


"$(INTDIR)\util.obj" : $(SOURCE) $(DEP_CPP_UTIL_) "$(INTDIR)"\
 "$(INTDIR)\oleview.pch"

"$(INTDIR)\util.sbr" : $(SOURCE) $(DEP_CPP_UTIL_) "$(INTDIR)"\
 "$(INTDIR)\oleview.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_UTIL_) "$(INTDIR)"\
 "$(INTDIR)\oleview.pch"


!ELSEIF  "$(CFG)" == "oleview - Win32 Release"


"$(INTDIR)\util.obj" : $(SOURCE) $(DEP_CPP_UTIL_) "$(INTDIR)"\
 "$(INTDIR)\oleview.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\shadow.cpp
DEP_CPP_SHADO=\
	".\stdafx.h"\
	".\shadow.h"\
	

!IF  "$(CFG)" == "oleview - Win32 Debug"


"$(INTDIR)\shadow.obj" : $(SOURCE) $(DEP_CPP_SHADO) "$(INTDIR)"\
 "$(INTDIR)\oleview.pch"

"$(INTDIR)\shadow.sbr" : $(SOURCE) $(DEP_CPP_SHADO) "$(INTDIR)"\
 "$(INTDIR)\oleview.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_SHADO) "$(INTDIR)"\
 "$(INTDIR)\oleview.pch"


!ELSEIF  "$(CFG)" == "oleview - Win32 Release"


"$(INTDIR)\shadow.obj" : $(SOURCE) $(DEP_CPP_SHADO) "$(INTDIR)"\
 "$(INTDIR)\oleview.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\doc.cpp
DEP_CPP_DOC_C=\
	".\stdafx.h"\
	".\oleview.h"\
	".\doc.h"\
	".\iviewers\iviewers.h"\
	".\util.h"\
	".\mainfrm.h"\
	".\regview.h"\
	".\obj_vw.h"\
	".\view.h"\
	

!IF  "$(CFG)" == "oleview - Win32 Debug"


"$(INTDIR)\doc.obj" : $(SOURCE) $(DEP_CPP_DOC_C) "$(INTDIR)"\
 "$(INTDIR)\oleview.pch"

"$(INTDIR)\doc.sbr" : $(SOURCE) $(DEP_CPP_DOC_C) "$(INTDIR)"\
 "$(INTDIR)\oleview.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_DOC_C) "$(INTDIR)"\
 "$(INTDIR)\oleview.pch"


!ELSEIF  "$(CFG)" == "oleview - Win32 Release"


"$(INTDIR)\doc.obj" : $(SOURCE) $(DEP_CPP_DOC_C) "$(INTDIR)"\
 "$(INTDIR)\oleview.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\oleview.cpp
DEP_CPP_OLE2VI=\
	".\stdafx.h"\
	".\iviewers\iviewers.h"\
	".\oleview.h"\
	".\mainfrm.h"\
	".\doc.h"\
	".\obj_vw.h"\
	".\regview.h"\
	".\shadow.h"\
	".\util.h"\
	".\view.h"\
	

!IF  "$(CFG)" == "oleview - Win32 Debug"


"$(INTDIR)\oleview.obj" : $(SOURCE) $(DEP_CPP_OLE2VI) "$(INTDIR)"\
 "$(INTDIR)\oleview.pch"

"$(INTDIR)\oleview.sbr" : $(SOURCE) $(DEP_CPP_OLE2VI) "$(INTDIR)"\
 "$(INTDIR)\oleview.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_OLE2VI) "$(INTDIR)"\
 "$(INTDIR)\oleview.pch"


!ELSEIF  "$(CFG)" == "oleview - Win32 Release"


"$(INTDIR)\oleview.obj" : $(SOURCE) $(DEP_CPP_OLE2VI) "$(INTDIR)"\
 "$(INTDIR)\oleview.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\regview.cpp
DEP_CPP_REGVI=\
	".\stdafx.h"\
	".\oleview.h"\
	".\regview.h"\
	".\iviewers\iviewers.h"\
	".\util.h"\
	".\mainfrm.h"\
	".\doc.h"\
	".\obj_vw.h"\
	".\view.h"\
	

!IF  "$(CFG)" == "oleview - Win32 Debug"


"$(INTDIR)\regview.obj" : $(SOURCE) $(DEP_CPP_REGVI) "$(INTDIR)"\
 "$(INTDIR)\oleview.pch"

"$(INTDIR)\regview.sbr" : $(SOURCE) $(DEP_CPP_REGVI) "$(INTDIR)"\
 "$(INTDIR)\oleview.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_REGVI) "$(INTDIR)"\
 "$(INTDIR)\oleview.pch"


!ELSEIF  "$(CFG)" == "oleview - Win32 Release"


"$(INTDIR)\regview.obj" : $(SOURCE) $(DEP_CPP_REGVI) "$(INTDIR)"\
 "$(INTDIR)\oleview.pch"


!ENDIF 

# End Source File
# End Target
# End Project
################################################################################
