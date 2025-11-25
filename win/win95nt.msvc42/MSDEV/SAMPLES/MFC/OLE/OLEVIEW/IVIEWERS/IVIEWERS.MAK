# Microsoft Visual C++ Generated NMAKE File, Format Version 40000
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

!IF "$(CFG)" == ""
CFG=iviewers - Win32 DLL Debug
!MESSAGE No configuration specified.  Defaulting to iviewers - Win32 DLL\
 Debug.
!ENDIF 

!IF "$(CFG)" != "iviewers - Win32 DLL Release" && "$(CFG)" !=\
 "iviewers - Win32 DLL Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "iviewers.mak" CFG="iviewers - Win32 DLL Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "iviewers - Win32 DLL Release" (based on\
 "Win32 (x86) Dynamic-Link Library")
!MESSAGE "iviewers - Win32 DLL Debug" (based on\
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
# PROP Target_Last_Scanned "iviewers - Win32 DLL Debug"
CPP=cl.exe
MTL=mktyplib.exe
RSC=rc.exe

!IF  "$(CFG)" == "iviewers - Win32 DLL Release"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir Win32_D0
# PROP BASE Intermediate_Dir Win32_D0
# PROP Use_MFC 1
# PROP Use_Debug_Libraries 0
# PROP Output_Dir dllrel
# PROP Intermediate_Dir dllrel
OUTDIR=.\dllrel
INTDIR=.\dllrel

ALL : "$(OUTDIR)\iviewers.dll" "$(OUTDIR)\iviewers.bsc"

CLEAN : 
	-@erase ".\dllrel\iviewers.bsc"
	-@erase ".\dllrel\tlblist.sbr"
	-@erase ".\dllrel\iviewers.pch"
	-@erase ".\dllrel\dataobj.sbr"
	-@erase ".\dllrel\tree.sbr"
	-@erase ".\dllrel\iadvsink.sbr"
	-@erase ".\dllrel\typelib.sbr"
	-@erase ".\dllrel\tlbtree.sbr"
	-@erase ".\dllrel\idataobj.sbr"
	-@erase ".\dllrel\dispatch.sbr"
	-@erase ".\dllrel\guid.sbr"
	-@erase ".\dllrel\shadow.sbr"
	-@erase ".\dllrel\stdafx.sbr"
	-@erase ".\dllrel\tlbodl.sbr"
	-@erase ".\dllrel\iviewers.sbr"
	-@erase ".\dllrel\util.sbr"
	-@erase ".\dllrel\iviewer.sbr"
	-@erase ".\dllrel\iviewers.dll"
	-@erase ".\dllrel\tlbodl.obj"
	-@erase ".\dllrel\iviewers.obj"
	-@erase ".\dllrel\util.obj"
	-@erase ".\dllrel\iviewer.obj"
	-@erase ".\dllrel\tlblist.obj"
	-@erase ".\dllrel\dataobj.obj"
	-@erase ".\dllrel\tree.obj"
	-@erase ".\dllrel\iadvsink.obj"
	-@erase ".\dllrel\typelib.obj"
	-@erase ".\dllrel\tlbtree.obj"
	-@erase ".\dllrel\idataobj.obj"
	-@erase ".\dllrel\dispatch.obj"
	-@erase ".\dllrel\guid.obj"
	-@erase ".\dllrel\shadow.obj"
	-@erase ".\dllrel\stdafx.obj"
	-@erase ".\dllrel\iviewers.res"
	-@erase ".\dllrel\iviewers.lib"
	-@erase ".\dllrel\iviewers.exp"
	-@erase ".\dllrel\iviewers.pdb"
	-@erase ".\dllrel\iviewers.map"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /YX /c
# ADD CPP /nologo /MT /W4 /WX /GX /O2 /D "NDEBUG" /D "_USRDLL" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_WINDLL" /FR /Yu"STDAFX.H" /c
CPP_PROJ=/nologo /MT /W4 /WX /GX /O2 /D "NDEBUG" /D "_USRDLL" /D "WIN32" /D\
 "_WINDOWS" /D "_MBCS" /D "_WINDLL" /FR"$(INTDIR)/" /Fp"$(INTDIR)/iviewers.pch"\
 /Yu"STDAFX.H" /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\dllrel/
CPP_SBRS=.\dllrel/
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/iviewers.res" /d "NDEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/iviewers.bsc" 
BSC32_SBRS= \
	"$(INTDIR)/tlblist.sbr" \
	"$(INTDIR)/dataobj.sbr" \
	"$(INTDIR)/tree.sbr" \
	"$(INTDIR)/iadvsink.sbr" \
	"$(INTDIR)/typelib.sbr" \
	"$(INTDIR)/tlbtree.sbr" \
	"$(INTDIR)/idataobj.sbr" \
	"$(INTDIR)/dispatch.sbr" \
	"$(INTDIR)/guid.sbr" \
	"$(INTDIR)/shadow.sbr" \
	"$(INTDIR)/stdafx.sbr" \
	"$(INTDIR)/tlbodl.sbr" \
	"$(INTDIR)/iviewers.sbr" \
	"$(INTDIR)/util.sbr" \
	"$(INTDIR)/iviewer.sbr"

"$(OUTDIR)\iviewers.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /machine:I386
# SUBTRACT BASE LINK32 /pdb:none
# ADD LINK32 /nologo /subsystem:windows /dll /map /debug /machine:I386
# SUBTRACT LINK32 /verbose /pdb:none /incremental:yes
LINK32_FLAGS=/nologo /subsystem:windows /dll /incremental:no\
 /pdb:"$(OUTDIR)/iviewers.pdb" /map:"$(INTDIR)/iviewers.map" /debug\
 /machine:I386 /def:".\iviewers.def" /out:"$(OUTDIR)/iviewers.dll"\
 /implib:"$(OUTDIR)/iviewers.lib" 
DEF_FILE= \
	".\iviewers.def"
LINK32_OBJS= \
	"$(INTDIR)/tlbodl.obj" \
	"$(INTDIR)/iviewers.obj" \
	"$(INTDIR)/util.obj" \
	"$(INTDIR)/iviewer.obj" \
	"$(INTDIR)/tlblist.obj" \
	"$(INTDIR)/dataobj.obj" \
	"$(INTDIR)/tree.obj" \
	"$(INTDIR)/iadvsink.obj" \
	"$(INTDIR)/typelib.obj" \
	"$(INTDIR)/tlbtree.obj" \
	"$(INTDIR)/idataobj.obj" \
	"$(INTDIR)/dispatch.obj" \
	"$(INTDIR)/guid.obj" \
	"$(INTDIR)/shadow.obj" \
	"$(INTDIR)/stdafx.obj" \
	"$(INTDIR)/iviewers.res"

"$(OUTDIR)\iviewers.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "iviewers - Win32 DLL Debug"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir Win32_D0
# PROP BASE Intermediate_Dir Win32_D0
# PROP Use_MFC 1
# PROP Use_Debug_Libraries 1
# PROP Output_Dir dlldebug
# PROP Intermediate_Dir dlldebug
OUTDIR=.\dlldebug
INTDIR=.\dlldebug

ALL : "$(OUTDIR)\iviewers.dll"

CLEAN : 
	-@erase ".\dlldebug\vc40.pdb"
	-@erase ".\dlldebug\iviewers.pch"
	-@erase ".\dlldebug\iviewers.dll"
	-@erase ".\dlldebug\guid.obj"
	-@erase ".\dlldebug\iadvsink.obj"
	-@erase ".\dlldebug\idataobj.obj"
	-@erase ".\dlldebug\dispatch.obj"
	-@erase ".\dlldebug\shadow.obj"
	-@erase ".\dlldebug\util.obj"
	-@erase ".\dlldebug\stdafx.obj"
	-@erase ".\dlldebug\iviewer.obj"
	-@erase ".\dlldebug\tlbodl.obj"
	-@erase ".\dlldebug\tlblist.obj"
	-@erase ".\dlldebug\dataobj.obj"
	-@erase ".\dlldebug\tree.obj"
	-@erase ".\dlldebug\iviewers.obj"
	-@erase ".\dlldebug\typelib.obj"
	-@erase ".\dlldebug\tlbtree.obj"
	-@erase ".\dlldebug\iviewers.res"
	-@erase ".\dlldebug\iviewers.ilk"
	-@erase ".\dlldebug\iviewers.lib"
	-@erase ".\dlldebug\iviewers.exp"
	-@erase ".\dlldebug\iviewers.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MD /W3 /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /YX /c
# ADD CPP /nologo /MTd /W4 /WX /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_USRDLL" /D "_MBCS" /Yu"STDAFX.H" /c
CPP_PROJ=/nologo /MTd /W4 /WX /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D\
 "_WINDOWS" /D "_WINDLL" /D "_USRDLL" /D "_MBCS" /Fp"$(INTDIR)/iviewers.pch"\
 /Yu"STDAFX.H" /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\dlldebug/
CPP_SBRS=
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/iviewers.res" /d "_DEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/iviewers.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /debug /machine:I386
# SUBTRACT BASE LINK32 /pdb:none
# ADD LINK32 /nologo /subsystem:windows /dll /debug /machine:I386
# SUBTRACT LINK32 /profile /incremental:no
LINK32_FLAGS=/nologo /subsystem:windows /dll /incremental:yes\
 /pdb:"$(OUTDIR)/iviewers.pdb" /debug /machine:I386 /def:".\iviewers.def"\
 /out:"$(OUTDIR)/iviewers.dll" /implib:"$(OUTDIR)/iviewers.lib" 
DEF_FILE= \
	".\iviewers.def"
LINK32_OBJS= \
	"$(INTDIR)/guid.obj" \
	"$(INTDIR)/iadvsink.obj" \
	"$(INTDIR)/idataobj.obj" \
	"$(INTDIR)/dispatch.obj" \
	"$(INTDIR)/shadow.obj" \
	"$(INTDIR)/util.obj" \
	"$(INTDIR)/stdafx.obj" \
	"$(INTDIR)/iviewer.obj" \
	"$(INTDIR)/tlbodl.obj" \
	"$(INTDIR)/tlblist.obj" \
	"$(INTDIR)/dataobj.obj" \
	"$(INTDIR)/tree.obj" \
	"$(INTDIR)/iviewers.obj" \
	"$(INTDIR)/typelib.obj" \
	"$(INTDIR)/tlbtree.obj" \
	"$(INTDIR)/iviewers.res"

"$(OUTDIR)\iviewers.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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

# Name "iviewers - Win32 DLL Release"
# Name "iviewers - Win32 DLL Debug"

!IF  "$(CFG)" == "iviewers - Win32 DLL Release"

!ELSEIF  "$(CFG)" == "iviewers - Win32 DLL Debug"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\stdafx.cpp
DEP_CPP_STDAF=\
	".\stdafx.h"\
	

!IF  "$(CFG)" == "iviewers - Win32 DLL Release"

# ADD CPP /Yc"STDAFX.H"

BuildCmds= \
	$(CPP) /nologo /MT /W4 /WX /GX /O2 /D "NDEBUG" /D "_USRDLL" /D "WIN32" /D\
 "_WINDOWS" /D "_MBCS" /D "_WINDLL" /FR"$(INTDIR)/" /Fp"$(INTDIR)/iviewers.pch"\
 /Yc"STDAFX.H" /Fo"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\stdafx.sbr" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\iviewers.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "iviewers - Win32 DLL Debug"

# ADD CPP /Yc"STDAFX.H"

BuildCmds= \
	$(CPP) /nologo /MTd /W4 /WX /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D\
 "_WINDOWS" /D "_WINDLL" /D "_USRDLL" /D "_MBCS" /Fp"$(INTDIR)/iviewers.pch"\
 /Yc"STDAFX.H" /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\iviewers.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\iviewers.cpp
DEP_CPP_IVIEW=\
	".\stdafx.h"\
	".\iviewers.h"\
	".\typelib.h"\
	".\iview.h"\
	".\util.h"\
	".\iviewer.h"\
	

!IF  "$(CFG)" == "iviewers - Win32 DLL Release"


"$(INTDIR)\iviewers.obj" : $(SOURCE) $(DEP_CPP_IVIEW) "$(INTDIR)"\
 "$(INTDIR)\iviewers.pch"

"$(INTDIR)\iviewers.sbr" : $(SOURCE) $(DEP_CPP_IVIEW) "$(INTDIR)"\
 "$(INTDIR)\iviewers.pch"


!ELSEIF  "$(CFG)" == "iviewers - Win32 DLL Debug"


"$(INTDIR)\iviewers.obj" : $(SOURCE) $(DEP_CPP_IVIEW) "$(INTDIR)"\
 "$(INTDIR)\iviewers.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_IVIEW) "$(INTDIR)"\
 "$(INTDIR)\iviewers.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\iviewers.rc
DEP_RSC_IVIEWE=\
	".\res\idr_type.ico"\
	".\res\imagelis.bmp"\
	".\res\toolbar.bmp"\
	".\res\iviewers.rc2"\
	

"$(INTDIR)\iviewers.res" : $(SOURCE) $(DEP_RSC_IVIEWE) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\iviewers.def

!IF  "$(CFG)" == "iviewers - Win32 DLL Release"

!ELSEIF  "$(CFG)" == "iviewers - Win32 DLL Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\typelib.cpp
DEP_CPP_TYPEL=\
	".\stdafx.h"\
	".\iviewers.h"\
	".\typelib.h"\
	".\iview.h"\
	".\iviewer.h"\
	".\util.h"\
	".\tlbtree.h"\
	".\tlblist.h"\
	".\tlbodl.h"\
	".\tree.h"\
	

!IF  "$(CFG)" == "iviewers - Win32 DLL Release"


"$(INTDIR)\typelib.obj" : $(SOURCE) $(DEP_CPP_TYPEL) "$(INTDIR)"\
 "$(INTDIR)\iviewers.pch"

"$(INTDIR)\typelib.sbr" : $(SOURCE) $(DEP_CPP_TYPEL) "$(INTDIR)"\
 "$(INTDIR)\iviewers.pch"


!ELSEIF  "$(CFG)" == "iviewers - Win32 DLL Debug"


"$(INTDIR)\typelib.obj" : $(SOURCE) $(DEP_CPP_TYPEL) "$(INTDIR)"\
 "$(INTDIR)\iviewers.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_TYPEL) "$(INTDIR)"\
 "$(INTDIR)\iviewers.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\dataobj.cpp
DEP_CPP_DATAO=\
	".\stdafx.h"\
	".\iviewers.h"\
	".\iview.h"\
	".\dataobj.h"\
	".\util.h"\
	".\idataobj.h"\
	".\iviewer.h"\
	

!IF  "$(CFG)" == "iviewers - Win32 DLL Release"


"$(INTDIR)\dataobj.obj" : $(SOURCE) $(DEP_CPP_DATAO) "$(INTDIR)"\
 "$(INTDIR)\iviewers.pch"

"$(INTDIR)\dataobj.sbr" : $(SOURCE) $(DEP_CPP_DATAO) "$(INTDIR)"\
 "$(INTDIR)\iviewers.pch"


!ELSEIF  "$(CFG)" == "iviewers - Win32 DLL Debug"


"$(INTDIR)\dataobj.obj" : $(SOURCE) $(DEP_CPP_DATAO) "$(INTDIR)"\
 "$(INTDIR)\iviewers.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_DATAO) "$(INTDIR)"\
 "$(INTDIR)\iviewers.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\iviewer.cpp
DEP_CPP_IVIEWER=\
	".\stdafx.h"\
	".\iviewers.h"\
	".\iviewer.h"\
	

!IF  "$(CFG)" == "iviewers - Win32 DLL Release"


"$(INTDIR)\iviewer.obj" : $(SOURCE) $(DEP_CPP_IVIEWER) "$(INTDIR)"\
 "$(INTDIR)\iviewers.pch"

"$(INTDIR)\iviewer.sbr" : $(SOURCE) $(DEP_CPP_IVIEWER) "$(INTDIR)"\
 "$(INTDIR)\iviewers.pch"


!ELSEIF  "$(CFG)" == "iviewers - Win32 DLL Debug"


"$(INTDIR)\iviewer.obj" : $(SOURCE) $(DEP_CPP_IVIEWER) "$(INTDIR)"\
 "$(INTDIR)\iviewers.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_IVIEWER) "$(INTDIR)"\
 "$(INTDIR)\iviewers.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\dispatch.cpp
DEP_CPP_DISPA=\
	".\dispatch.h"\
	".\iview.h"\
	".\iviewer.h"\
	".\iviewers.h"\
	".\stdafx.h"\
	".\util.h"\
	

!IF  "$(CFG)" == "iviewers - Win32 DLL Release"


"$(INTDIR)\dispatch.obj" : $(SOURCE) $(DEP_CPP_DISPA) "$(INTDIR)"\
 "$(INTDIR)\iviewers.pch"

"$(INTDIR)\dispatch.sbr" : $(SOURCE) $(DEP_CPP_DISPA) "$(INTDIR)"\
 "$(INTDIR)\iviewers.pch"


!ELSEIF  "$(CFG)" == "iviewers - Win32 DLL Debug"


"$(INTDIR)\dispatch.obj" : $(SOURCE) $(DEP_CPP_DISPA) "$(INTDIR)"\
 "$(INTDIR)\iviewers.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_DISPA) "$(INTDIR)"\
 "$(INTDIR)\iviewers.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\idataobj.cpp
DEP_CPP_IDATA=\
	".\stdafx.h"\
	".\iview.h"\
	".\util.h"\
	".\idataobj.h"\
	".\iadvsink.h"\
	

!IF  "$(CFG)" == "iviewers - Win32 DLL Release"


"$(INTDIR)\idataobj.obj" : $(SOURCE) $(DEP_CPP_IDATA) "$(INTDIR)"\
 "$(INTDIR)\iviewers.pch"

"$(INTDIR)\idataobj.sbr" : $(SOURCE) $(DEP_CPP_IDATA) "$(INTDIR)"\
 "$(INTDIR)\iviewers.pch"


!ELSEIF  "$(CFG)" == "iviewers - Win32 DLL Debug"


"$(INTDIR)\idataobj.obj" : $(SOURCE) $(DEP_CPP_IDATA) "$(INTDIR)"\
 "$(INTDIR)\iviewers.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_IDATA) "$(INTDIR)"\
 "$(INTDIR)\iviewers.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\iadvsink.cpp
DEP_CPP_IADVS=\
	".\stdafx.h"\
	".\iview.h"\
	".\util.h"\
	".\idataobj.h"\
	".\iadvsink.h"\
	

!IF  "$(CFG)" == "iviewers - Win32 DLL Release"


"$(INTDIR)\iadvsink.obj" : $(SOURCE) $(DEP_CPP_IADVS) "$(INTDIR)"\
 "$(INTDIR)\iviewers.pch"

"$(INTDIR)\iadvsink.sbr" : $(SOURCE) $(DEP_CPP_IADVS) "$(INTDIR)"\
 "$(INTDIR)\iviewers.pch"


!ELSEIF  "$(CFG)" == "iviewers - Win32 DLL Debug"


"$(INTDIR)\iadvsink.obj" : $(SOURCE) $(DEP_CPP_IADVS) "$(INTDIR)"\
 "$(INTDIR)\iviewers.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_IADVS) "$(INTDIR)"\
 "$(INTDIR)\iviewers.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\util.cpp
DEP_CPP_UTIL_=\
	".\stdafx.h"\
	".\util.h"\
	".\shadow.h"\
	

!IF  "$(CFG)" == "iviewers - Win32 DLL Release"


"$(INTDIR)\util.obj" : $(SOURCE) $(DEP_CPP_UTIL_) "$(INTDIR)"\
 "$(INTDIR)\iviewers.pch"

"$(INTDIR)\util.sbr" : $(SOURCE) $(DEP_CPP_UTIL_) "$(INTDIR)"\
 "$(INTDIR)\iviewers.pch"


!ELSEIF  "$(CFG)" == "iviewers - Win32 DLL Debug"


"$(INTDIR)\util.obj" : $(SOURCE) $(DEP_CPP_UTIL_) "$(INTDIR)"\
 "$(INTDIR)\iviewers.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_UTIL_) "$(INTDIR)"\
 "$(INTDIR)\iviewers.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\shadow.cpp
DEP_CPP_SHADO=\
	".\stdafx.h"\
	".\shadow.h"\
	

!IF  "$(CFG)" == "iviewers - Win32 DLL Release"


"$(INTDIR)\shadow.obj" : $(SOURCE) $(DEP_CPP_SHADO) "$(INTDIR)"\
 "$(INTDIR)\iviewers.pch"

"$(INTDIR)\shadow.sbr" : $(SOURCE) $(DEP_CPP_SHADO) "$(INTDIR)"\
 "$(INTDIR)\iviewers.pch"


!ELSEIF  "$(CFG)" == "iviewers - Win32 DLL Debug"


"$(INTDIR)\shadow.obj" : $(SOURCE) $(DEP_CPP_SHADO) "$(INTDIR)"\
 "$(INTDIR)\iviewers.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_SHADO) "$(INTDIR)"\
 "$(INTDIR)\iviewers.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\tlbtree.cpp
DEP_CPP_TLBTR=\
	".\stdafx.h"\
	".\iview.h"\
	".\util.h"\
	".\iviewers.h"\
	".\iviewer.h"\
	".\typelib.h"\
	".\tlbtree.h"\
	".\tree.h"\
	

!IF  "$(CFG)" == "iviewers - Win32 DLL Release"


"$(INTDIR)\tlbtree.obj" : $(SOURCE) $(DEP_CPP_TLBTR) "$(INTDIR)"\
 "$(INTDIR)\iviewers.pch"

"$(INTDIR)\tlbtree.sbr" : $(SOURCE) $(DEP_CPP_TLBTR) "$(INTDIR)"\
 "$(INTDIR)\iviewers.pch"


!ELSEIF  "$(CFG)" == "iviewers - Win32 DLL Debug"


"$(INTDIR)\tlbtree.obj" : $(SOURCE) $(DEP_CPP_TLBTR) "$(INTDIR)"\
 "$(INTDIR)\iviewers.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_TLBTR) "$(INTDIR)"\
 "$(INTDIR)\iviewers.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\tlblist.cpp
DEP_CPP_TLBLI=\
	".\stdafx.h"\
	".\iview.h"\
	".\util.h"\
	".\iviewers.h"\
	".\iviewer.h"\
	".\typelib.h"\
	".\tlblist.h"\
	

!IF  "$(CFG)" == "iviewers - Win32 DLL Release"


"$(INTDIR)\tlblist.obj" : $(SOURCE) $(DEP_CPP_TLBLI) "$(INTDIR)"\
 "$(INTDIR)\iviewers.pch"

"$(INTDIR)\tlblist.sbr" : $(SOURCE) $(DEP_CPP_TLBLI) "$(INTDIR)"\
 "$(INTDIR)\iviewers.pch"


!ELSEIF  "$(CFG)" == "iviewers - Win32 DLL Debug"


"$(INTDIR)\tlblist.obj" : $(SOURCE) $(DEP_CPP_TLBLI) "$(INTDIR)"\
 "$(INTDIR)\iviewers.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_TLBLI) "$(INTDIR)"\
 "$(INTDIR)\iviewers.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\guid.cpp
DEP_CPP_GUID_=\
	".\stdafx.h"\
	".\iviewers.h"\
	

!IF  "$(CFG)" == "iviewers - Win32 DLL Release"


"$(INTDIR)\guid.obj" : $(SOURCE) $(DEP_CPP_GUID_) "$(INTDIR)"\
 "$(INTDIR)\iviewers.pch"

"$(INTDIR)\guid.sbr" : $(SOURCE) $(DEP_CPP_GUID_) "$(INTDIR)"\
 "$(INTDIR)\iviewers.pch"


!ELSEIF  "$(CFG)" == "iviewers - Win32 DLL Debug"


"$(INTDIR)\guid.obj" : $(SOURCE) $(DEP_CPP_GUID_) "$(INTDIR)"\
 "$(INTDIR)\iviewers.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_GUID_) "$(INTDIR)"\
 "$(INTDIR)\iviewers.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\tree.cpp
DEP_CPP_TREE_=\
	".\stdafx.h"\
	".\tree.h"\
	".\util.h"\
	

!IF  "$(CFG)" == "iviewers - Win32 DLL Release"


"$(INTDIR)\tree.obj" : $(SOURCE) $(DEP_CPP_TREE_) "$(INTDIR)"\
 "$(INTDIR)\iviewers.pch"

"$(INTDIR)\tree.sbr" : $(SOURCE) $(DEP_CPP_TREE_) "$(INTDIR)"\
 "$(INTDIR)\iviewers.pch"


!ELSEIF  "$(CFG)" == "iviewers - Win32 DLL Debug"


"$(INTDIR)\tree.obj" : $(SOURCE) $(DEP_CPP_TREE_) "$(INTDIR)"\
 "$(INTDIR)\iviewers.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_TREE_) "$(INTDIR)"\
 "$(INTDIR)\iviewers.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\tlbodl.cpp
DEP_CPP_TLBOD=\
	".\stdafx.h"\
	".\iviewers.h"\
	".\iview.h"\
	".\util.h"\
	".\iviewer.h"\
	".\typelib.h"\
	".\tree.h"\
	".\tlbodl.h"\
	

!IF  "$(CFG)" == "iviewers - Win32 DLL Release"


"$(INTDIR)\tlbodl.obj" : $(SOURCE) $(DEP_CPP_TLBOD) "$(INTDIR)"\
 "$(INTDIR)\iviewers.pch"

"$(INTDIR)\tlbodl.sbr" : $(SOURCE) $(DEP_CPP_TLBOD) "$(INTDIR)"\
 "$(INTDIR)\iviewers.pch"


!ELSEIF  "$(CFG)" == "iviewers - Win32 DLL Debug"


"$(INTDIR)\tlbodl.obj" : $(SOURCE) $(DEP_CPP_TLBOD) "$(INTDIR)"\
 "$(INTDIR)\iviewers.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_TLBOD) "$(INTDIR)"\
 "$(INTDIR)\iviewers.pch"


!ENDIF 

# End Source File
# End Target
# End Project
################################################################################
