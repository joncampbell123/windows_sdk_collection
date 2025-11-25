# Microsoft Developer Studio Generated NMAKE File, Based on Pick3.dsp
!IF "$(CFG)" == ""
CFG=Pick3 - Win32 Release
!MESSAGE No configuration specified. Defaulting to Pick3 - Win32 Release.
!ENDIF 

!IF "$(CFG)" != "Pick3 - Win32 Release" && "$(CFG)" != "Pick3 - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Pick3.mak" CFG="Pick3 - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Pick3 - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "Pick3 - Win32 Debug" (based on "Win32 (x86) Application")
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

!IF  "$(CFG)" == "Pick3 - Win32 Release"

OUTDIR=.
INTDIR=.
# Begin Custom Macros
OutDir=.
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\Pick3Cntrl.exe"

!ELSE 

ALL : "$(OUTDIR)\Pick3Cntrl.exe"

!ENDIF 

CLEAN :
	-@erase "$(INTDIR)\ctlhost.obj"
	-@erase "$(INTDIR)\ctlhost.res"
	-@erase "$(INTDIR)\dactl.obj"
	-@erase "$(INTDIR)\pick3.obj"
	-@erase "$(INTDIR)\vc50.idb"
	-@erase "$(OUTDIR)\Pick3Cntrl.exe"

CPP_PROJ=/nologo /ML /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS"\
 /Fp"$(INTDIR)\Pick3.pch" /YX /FD /c 
CPP_OBJS=.
CPP_SBRS=.
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /o NUL /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\ctlhost.res" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\Pick3.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib /nologo /subsystem:windows /incremental:no\
 /pdb:"$(OUTDIR)\Pick3Cntrl.pdb" /out:"$(OUTDIR)\Pick3Cntrl.exe" 
LINK32_OBJS= \
	"$(INTDIR)\ctlhost.obj" \
	"$(INTDIR)\ctlhost.res" \
	"$(INTDIR)\dactl.obj" \
	"$(INTDIR)\pick3.obj"

"$(OUTDIR)\Pick3Cntrl.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "Pick3 - Win32 Debug"

OUTDIR=.
INTDIR=.
# Begin Custom Macros
OutDir=.
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\Pick3Cntrl.exe"

!ELSE 

ALL : "$(OUTDIR)\Pick3Cntrl.exe"

!ENDIF 

CLEAN :
	-@erase "$(INTDIR)\ctlhost.obj"
	-@erase "$(INTDIR)\ctlhost.res"
	-@erase "$(INTDIR)\dactl.obj"
	-@erase "$(INTDIR)\pick3.obj"
	-@erase "$(INTDIR)\vc50.idb"
	-@erase "$(INTDIR)\vc50.pdb"
	-@erase "$(OUTDIR)\Pick3Cntrl.exe"
	-@erase "$(OUTDIR)\Pick3Cntrl.ilk"
	-@erase "$(OUTDIR)\Pick3Cntrl.pdb"

CPP_PROJ=/nologo /MLd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /Fp"$(INTDIR)\Pick3.pch" /YX /FD /c 
CPP_OBJS=.
CPP_SBRS=.
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /o NUL /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\ctlhost.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\Pick3.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib /nologo /subsystem:windows /incremental:yes\
 /pdb:"$(OUTDIR)\Pick3Cntrl.pdb" /debug\
 /out:"$(OUTDIR)\Pick3Cntrl.exe" /pdbtype:sept 
LINK32_OBJS= \
	"$(INTDIR)\ctlhost.obj" \
	"$(INTDIR)\ctlhost.res" \
	"$(INTDIR)\dactl.obj" \
	"$(INTDIR)\pick3.obj"

"$(OUTDIR)\Pick3Cntrl.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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


!IF "$(CFG)" == "Pick3 - Win32 Release" || "$(CFG)" == "Pick3 - Win32 Debug"
SOURCE=.\ctlhost.cpp
DEP_CPP_CTLHO=\
	".\ctlhost.h"\
	".\dactl.h"\
	

"$(INTDIR)\ctlhost.obj" : $(SOURCE) $(DEP_CPP_CTLHO) "$(INTDIR)"


SOURCE=.\ctlhost.rc
DEP_RSC_CTLHOS=\
	".\dxm.ico"\
	

"$(INTDIR)\ctlhost.res" : $(SOURCE) $(DEP_RSC_CTLHOS) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)


SOURCE=.\dactl.cpp
DEP_CPP_DACTL=\
	".\dactl.h"\
	

"$(INTDIR)\dactl.obj" : $(SOURCE) $(DEP_CPP_DACTL) "$(INTDIR)"


SOURCE=.\pick3.cpp
DEP_CPP_PICK3=\
	".\ctlhost.h"\
	".\dactl.h"\
	

"$(INTDIR)\pick3.obj" : $(SOURCE) $(DEP_CPP_PICK3) "$(INTDIR)"



!ENDIF 

