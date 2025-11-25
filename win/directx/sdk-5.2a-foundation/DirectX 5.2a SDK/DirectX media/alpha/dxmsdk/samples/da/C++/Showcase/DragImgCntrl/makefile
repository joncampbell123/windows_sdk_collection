# Microsoft Developer Studio Generated NMAKE File, Based on DragImg.dsp
!IF "$(CFG)" == ""
CFG=DragImg - Win32 Release
!MESSAGE No configuration specified. Defaulting to DragImg - Win32 Release.
!ENDIF 

!IF "$(CFG)" != "DragImg - Win32 Release" && "$(CFG)" !=\
 "DragImg - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "DragImg.mak" CFG="DragImg - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "DragImg - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "DragImg - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "DragImg - Win32 Release"

OUTDIR=.
INTDIR=.
# Begin Custom Macros
OutDir=.
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\DragImgCntrl.exe"

!ELSE 

ALL : "$(OUTDIR)\DragImgCntrl.exe"

!ENDIF 

CLEAN :
	-@erase "$(INTDIR)\ctlhost.obj"
	-@erase "$(INTDIR)\ctlhost.res"
	-@erase "$(INTDIR)\dactl.obj"
	-@erase "$(INTDIR)\dadrag.obj"
	-@erase "$(INTDIR)\dragimg.obj"
	-@erase "$(INTDIR)\vc50.idb"
	-@erase "$(OUTDIR)\DragImgCntrl.exe"

CPP_PROJ=/nologo /ML /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS"\
 /Fp"$(INTDIR)\DragImg.pch" /YX /FD /c 
CPP_OBJS=.
CPP_SBRS=.
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /o NUL /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\ctlhost.res" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\DragImg.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=daguid.lib kernel32.lib user32.lib gdi32.lib winspool.lib\
 comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib\
 odbc32.lib odbccp32.lib /nologo /subsystem:windows /incremental:no\
 /pdb:"$(OUTDIR)\DragImgCntrl.pdb"\
 /out:"$(OUTDIR)\DragImgCntrl.exe" 
LINK32_OBJS= \
	"$(INTDIR)\ctlhost.obj" \
	"$(INTDIR)\ctlhost.res" \
	"$(INTDIR)\dactl.obj" \
	"$(INTDIR)\dadrag.obj" \
	"$(INTDIR)\dragimg.obj"

"$(OUTDIR)\DragImgCntrl.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "DragImg - Win32 Debug"

OUTDIR=.
INTDIR=.
# Begin Custom Macros
OutDir=.
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\DragImgCntrl.exe"

!ELSE 

ALL : "$(OUTDIR)\DragImgCntrl.exe"

!ENDIF 

CLEAN :
	-@erase "$(INTDIR)\ctlhost.obj"
	-@erase "$(INTDIR)\ctlhost.res"
	-@erase "$(INTDIR)\dactl.obj"
	-@erase "$(INTDIR)\dadrag.obj"
	-@erase "$(INTDIR)\dragimg.obj"
	-@erase "$(INTDIR)\vc50.idb"
	-@erase "$(INTDIR)\vc50.pdb"
	-@erase "$(OUTDIR)\DragImgCntrl.exe"
	-@erase "$(OUTDIR)\DragImgCntrl.ilk"
	-@erase "$(OUTDIR)\DragImgCntrl.pdb"

CPP_PROJ=/nologo /MLd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /Fp"$(INTDIR)\DragImg.pch" /YX /FD /c 
CPP_OBJS=.
CPP_SBRS=.
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /o NUL /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\ctlhost.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\DragImg.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=daguid.lib kernel32.lib user32.lib gdi32.lib winspool.lib\
 comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib\
 odbc32.lib odbccp32.lib /nologo /subsystem:windows /incremental:yes\
 /pdb:"$(OUTDIR)\DragImgCntrl.pdb" /debug\
 /out:"$(OUTDIR)\DragImgCntrl.exe" /pdbtype:sept 
LINK32_OBJS= \
	"$(INTDIR)\ctlhost.obj" \
	"$(INTDIR)\ctlhost.res" \
	"$(INTDIR)\dactl.obj" \
	"$(INTDIR)\dadrag.obj" \
	"$(INTDIR)\dragimg.obj"

"$(OUTDIR)\DragImgCntrl.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 

.c{$(CPP_OBJS)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(CPP_OBJS)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(CPP_OBJS)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(CPP_SBRS)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(CPP_SBRS)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(CPP_SBRS)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<


!IF "$(CFG)" == "DragImg - Win32 Release" || "$(CFG)" ==\
 "DragImg - Win32 Debug"
SOURCE=.\ctlhost.cpp
DEP_CPP_CTLHO=\
	".\ctlhost.h"\
	".\dactl.h"\
	".\dadrag.h"\
	

"$(INTDIR)\ctlhost.obj" : $(SOURCE) $(DEP_CPP_CTLHO) "$(INTDIR)"


SOURCE=.\ctlhost.rc
DEP_RSC_CTLHOS=\
	".\Dxm.ico"\
	

"$(INTDIR)\ctlhost.res" : $(SOURCE) $(DEP_RSC_CTLHOS) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)


SOURCE=.\dactl.cpp
DEP_CPP_DACTL=\
	".\dactl.h"\
	".\dadrag.h"\
	

"$(INTDIR)\dactl.obj" : $(SOURCE) $(DEP_CPP_DACTL) "$(INTDIR)"


SOURCE=.\dadrag.cpp
DEP_CPP_DADRA=\
	".\dadrag.h"\
	

"$(INTDIR)\dadrag.obj" : $(SOURCE) $(DEP_CPP_DADRA) "$(INTDIR)"


SOURCE=.\dragimg.cpp
DEP_CPP_DRAGI=\
	".\ctlhost.h"\
	".\dactl.h"\
	".\dadrag.h"\
	

"$(INTDIR)\dragimg.obj" : $(SOURCE) $(DEP_CPP_DRAGI) "$(INTDIR)"



!ENDIF 

