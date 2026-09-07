# Microsoft Developer Studio Generated NMAKE File, Based on Data.dsp
!IF "$(CFG)" == ""
CFG=Data - Win32 Debug
!MESSAGE No configuration specified. Defaulting to Data - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "Data - Win32 Release MinSize" && "$(CFG)" != "Data - Win32 Release MinDependency" && "$(CFG)" != "Data - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Data.mak" CFG="Data - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Data - Win32 Release MinSize" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "Data - Win32 Release MinDependency" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "Data - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "Data - Win32 Release MinSize"

OUTDIR=.\ReleaseMinSize
INTDIR=.\ReleaseMinSize
# Begin Custom Macros
OutDir=.\ReleaseMinSize
# End Custom Macros

ALL : ".\Data.h" "$(OUTDIR)\Data.dll" "$(OUTDIR)\Data.bsc" ".\ReleaseMinSize\regsvr32.trg"


CLEAN :
	-@erase "$(INTDIR)\Data.obj"
	-@erase "$(INTDIR)\Data.pch"
	-@erase "$(INTDIR)\Data.res"
	-@erase "$(INTDIR)\Data.sbr"
	-@erase "$(INTDIR)\Dynamic.obj"
	-@erase "$(INTDIR)\Dynamic.sbr"
	-@erase "$(INTDIR)\Entity.obj"
	-@erase "$(INTDIR)\Entity.sbr"
	-@erase "$(INTDIR)\entity_except.obj"
	-@erase "$(INTDIR)\entity_except.sbr"
	-@erase "$(INTDIR)\heap.obj"
	-@erase "$(INTDIR)\heap.sbr"
	-@erase "$(INTDIR)\ODBCData.obj"
	-@erase "$(INTDIR)\ODBCData.sbr"
	-@erase "$(INTDIR)\pooleddata.obj"
	-@erase "$(INTDIR)\pooleddata.sbr"
	-@erase "$(INTDIR)\Proxy.obj"
	-@erase "$(INTDIR)\Proxy.sbr"
	-@erase "$(INTDIR)\Proxy_except.obj"
	-@erase "$(INTDIR)\Proxy_except.sbr"
	-@erase "$(INTDIR)\Resultset.obj"
	-@erase "$(INTDIR)\Resultset.sbr"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\StdAfx.sbr"
	-@erase "$(INTDIR)\string.obj"
	-@erase "$(INTDIR)\string.sbr"
	-@erase "$(INTDIR)\string_fast.obj"
	-@erase "$(INTDIR)\string_fast.sbr"
	-@erase "$(INTDIR)\Utils.obj"
	-@erase "$(INTDIR)\Utils.sbr"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\Data.bsc"
	-@erase "$(OUTDIR)\Data.dll"
	-@erase "$(OUTDIR)\Data.exp"
	-@erase "$(OUTDIR)\Data.lib"
	-@erase ".\Data.h"
	-@erase ".\Data.tlb"
	-@erase ".\Data_i.c"
	-@erase ".\ReleaseMinSize\regsvr32.trg"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /G6 /MT /W3 /GX /Ox /D "NDEBUG" /D "_MBCS" /D "_ATL_DLL" /D "WIN32" /D "_WINDOWS" /D "_USRDLL" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\Data.pch" /Yu"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

.c{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

MTL=midl.exe
MTL_PROJ=
RSC=rc.exe
RSC_PROJ=/l 0x809 /fo"$(INTDIR)\Data.res" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\Data.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\Data.sbr" \
	"$(INTDIR)\Dynamic.sbr" \
	"$(INTDIR)\Entity.sbr" \
	"$(INTDIR)\entity_except.sbr" \
	"$(INTDIR)\heap.sbr" \
	"$(INTDIR)\ODBCData.sbr" \
	"$(INTDIR)\pooleddata.sbr" \
	"$(INTDIR)\Proxy.sbr" \
	"$(INTDIR)\Proxy_except.sbr" \
	"$(INTDIR)\Resultset.sbr" \
	"$(INTDIR)\StdAfx.sbr" \
	"$(INTDIR)\string.sbr" \
	"$(INTDIR)\string_fast.sbr" \
	"$(INTDIR)\Utils.sbr"

"$(OUTDIR)\Data.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib mtx.lib mtxguid.lib delayimp.lib /nologo /subsystem:windows /dll /incremental:no /pdb:"$(OUTDIR)\Data.pdb" /machine:I386 /def:".\Data.def" /out:"$(OUTDIR)\Data.dll" /implib:"$(OUTDIR)\Data.lib" /delayload:mtxex.dll 
DEF_FILE= \
	".\Data.def"
LINK32_OBJS= \
	"$(INTDIR)\Data.obj" \
	"$(INTDIR)\Dynamic.obj" \
	"$(INTDIR)\Entity.obj" \
	"$(INTDIR)\entity_except.obj" \
	"$(INTDIR)\heap.obj" \
	"$(INTDIR)\ODBCData.obj" \
	"$(INTDIR)\pooleddata.obj" \
	"$(INTDIR)\Proxy.obj" \
	"$(INTDIR)\Proxy_except.obj" \
	"$(INTDIR)\Resultset.obj" \
	"$(INTDIR)\StdAfx.obj" \
	"$(INTDIR)\string.obj" \
	"$(INTDIR)\string_fast.obj" \
	"$(INTDIR)\Utils.obj" \
	"$(INTDIR)\Data.res"

"$(OUTDIR)\Data.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

OutDir=.\ReleaseMinSize
TargetPath=.\ReleaseMinSize\Data.dll
InputPath=.\ReleaseMinSize\Data.dll
SOURCE="$(InputPath)"

"$(OUTDIR)\regsvr32.trg" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	regsvr32 /s /c "$(TargetPath)" 
	echo regsvr32 exec. time > "$(OutDir)\regsvr32.trg" 
	echo Execute mtxrereg.exe before using MTS components in MTS 
<< 
	

!ELSEIF  "$(CFG)" == "Data - Win32 Release MinDependency"

OUTDIR=.\ReleaseMinDependency
INTDIR=.\ReleaseMinDependency
# Begin Custom Macros
OutDir=.\ReleaseMinDependency
# End Custom Macros

ALL : ".\Data.tlb" ".\Data.h" ".\Data_i.c" "$(OUTDIR)\Data.dll" ".\ReleaseMinDependency\regsvr32.trg"


CLEAN :
	-@erase "$(INTDIR)\Data.obj"
	-@erase "$(INTDIR)\Data.pch"
	-@erase "$(INTDIR)\Data.res"
	-@erase "$(INTDIR)\Dynamic.obj"
	-@erase "$(INTDIR)\Entity.obj"
	-@erase "$(INTDIR)\entity_except.obj"
	-@erase "$(INTDIR)\heap.obj"
	-@erase "$(INTDIR)\ODBCData.obj"
	-@erase "$(INTDIR)\pooleddata.obj"
	-@erase "$(INTDIR)\Proxy.obj"
	-@erase "$(INTDIR)\Proxy_except.obj"
	-@erase "$(INTDIR)\Resultset.obj"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\string.obj"
	-@erase "$(INTDIR)\string_fast.obj"
	-@erase "$(INTDIR)\Utils.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\Data.dll"
	-@erase "$(OUTDIR)\Data.exp"
	-@erase "$(OUTDIR)\Data.lib"
	-@erase ".\Data.h"
	-@erase ".\Data.tlb"
	-@erase ".\Data_i.c"
	-@erase ".\ReleaseMinDependency\regsvr32.trg"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /G6 /MT /W3 /GX /Ox /D "NDEBUG" /D "_MBCS" /D "_ATL_STATIC_REGISTRY" /D "WIN32" /D "_WINDOWS" /D "_USRDLL" /Fp"$(INTDIR)\Data.pch" /Yu"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

.c{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

MTL=midl.exe
MTL_PROJ=
RSC=rc.exe
RSC_PROJ=/l 0x809 /fo"$(INTDIR)\Data.res" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\Data.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib mtx.lib mtxguid.lib delayimp.lib /nologo /subsystem:windows /dll /incremental:no /pdb:"$(OUTDIR)\Data.pdb" /machine:I386 /def:".\Data.def" /out:"$(OUTDIR)\Data.dll" /implib:"$(OUTDIR)\Data.lib" /delayload:mtxex.dll 
DEF_FILE= \
	".\Data.def"
LINK32_OBJS= \
	"$(INTDIR)\Data.obj" \
	"$(INTDIR)\Dynamic.obj" \
	"$(INTDIR)\Entity.obj" \
	"$(INTDIR)\entity_except.obj" \
	"$(INTDIR)\heap.obj" \
	"$(INTDIR)\ODBCData.obj" \
	"$(INTDIR)\pooleddata.obj" \
	"$(INTDIR)\Proxy.obj" \
	"$(INTDIR)\Proxy_except.obj" \
	"$(INTDIR)\Resultset.obj" \
	"$(INTDIR)\StdAfx.obj" \
	"$(INTDIR)\string.obj" \
	"$(INTDIR)\string_fast.obj" \
	"$(INTDIR)\Utils.obj" \
	"$(INTDIR)\Data.res"

"$(OUTDIR)\Data.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

OutDir=.\ReleaseMinDependency
TargetPath=.\ReleaseMinDependency\Data.dll
InputPath=.\ReleaseMinDependency\Data.dll
SOURCE="$(InputPath)"

"$(OUTDIR)\regsvr32.trg" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	regsvr32 /s /c "$(TargetPath)" 
	echo regsvr32 exec. time > "$(OutDir)\regsvr32.trg" 
	echo Execute mtxrereg.exe before using MTS components in MTS 
<< 
	

!ELSEIF  "$(CFG)" == "Data - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

ALL : ".\Data.tlb" ".\Data.h" ".\Data_i.c" "$(OUTDIR)\Data.dll" "$(OUTDIR)\Data.bsc" ".\Debug\regsvr32.trg"


CLEAN :
	-@erase "$(INTDIR)\Data.obj"
	-@erase "$(INTDIR)\Data.pch"
	-@erase "$(INTDIR)\Data.res"
	-@erase "$(INTDIR)\Data.sbr"
	-@erase "$(INTDIR)\Dynamic.obj"
	-@erase "$(INTDIR)\Dynamic.sbr"
	-@erase "$(INTDIR)\Entity.obj"
	-@erase "$(INTDIR)\Entity.sbr"
	-@erase "$(INTDIR)\entity_except.obj"
	-@erase "$(INTDIR)\entity_except.sbr"
	-@erase "$(INTDIR)\heap.obj"
	-@erase "$(INTDIR)\heap.sbr"
	-@erase "$(INTDIR)\ODBCData.obj"
	-@erase "$(INTDIR)\ODBCData.sbr"
	-@erase "$(INTDIR)\pooleddata.obj"
	-@erase "$(INTDIR)\pooleddata.sbr"
	-@erase "$(INTDIR)\Proxy.obj"
	-@erase "$(INTDIR)\Proxy.sbr"
	-@erase "$(INTDIR)\Proxy_except.obj"
	-@erase "$(INTDIR)\Proxy_except.sbr"
	-@erase "$(INTDIR)\Resultset.obj"
	-@erase "$(INTDIR)\Resultset.sbr"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\StdAfx.sbr"
	-@erase "$(INTDIR)\string.obj"
	-@erase "$(INTDIR)\string.sbr"
	-@erase "$(INTDIR)\string_fast.obj"
	-@erase "$(INTDIR)\string_fast.sbr"
	-@erase "$(INTDIR)\Utils.obj"
	-@erase "$(INTDIR)\Utils.sbr"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\Data.bsc"
	-@erase "$(OUTDIR)\Data.dll"
	-@erase "$(OUTDIR)\Data.exp"
	-@erase "$(OUTDIR)\Data.ilk"
	-@erase "$(OUTDIR)\Data.lib"
	-@erase "$(OUTDIR)\Data.pdb"
	-@erase ".\Data.h"
	-@erase ".\Data.tlb"
	-@erase ".\Data_i.c"
	-@erase ".\Debug\regsvr32.trg"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /G6 /MTd /W3 /Gm /GX /ZI /Od /D "_DEBUG" /D "_MBCS" /D "WIN32" /D "_WINDOWS" /D "_USRDLL" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\Data.pch" /Yu"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

.c{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

MTL=midl.exe
MTL_PROJ=
RSC=rc.exe
RSC_PROJ=/l 0x809 /fo"$(INTDIR)\Data.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\Data.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\Data.sbr" \
	"$(INTDIR)\Dynamic.sbr" \
	"$(INTDIR)\Entity.sbr" \
	"$(INTDIR)\entity_except.sbr" \
	"$(INTDIR)\heap.sbr" \
	"$(INTDIR)\ODBCData.sbr" \
	"$(INTDIR)\pooleddata.sbr" \
	"$(INTDIR)\Proxy.sbr" \
	"$(INTDIR)\Proxy_except.sbr" \
	"$(INTDIR)\Resultset.sbr" \
	"$(INTDIR)\StdAfx.sbr" \
	"$(INTDIR)\string.sbr" \
	"$(INTDIR)\string_fast.sbr" \
	"$(INTDIR)\Utils.sbr"

"$(OUTDIR)\Data.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib mtx.lib mtxguid.lib delayimp.lib /nologo /subsystem:windows /dll /incremental:yes /pdb:"$(OUTDIR)\Data.pdb" /debug /machine:I386 /def:".\Data.def" /out:"$(OUTDIR)\Data.dll" /implib:"$(OUTDIR)\Data.lib" /pdbtype:sept /delayload:mtxex.dll 
DEF_FILE= \
	".\Data.def"
LINK32_OBJS= \
	"$(INTDIR)\Data.obj" \
	"$(INTDIR)\Dynamic.obj" \
	"$(INTDIR)\Entity.obj" \
	"$(INTDIR)\entity_except.obj" \
	"$(INTDIR)\heap.obj" \
	"$(INTDIR)\ODBCData.obj" \
	"$(INTDIR)\pooleddata.obj" \
	"$(INTDIR)\Proxy.obj" \
	"$(INTDIR)\Proxy_except.obj" \
	"$(INTDIR)\Resultset.obj" \
	"$(INTDIR)\StdAfx.obj" \
	"$(INTDIR)\string.obj" \
	"$(INTDIR)\string_fast.obj" \
	"$(INTDIR)\Utils.obj" \
	"$(INTDIR)\Data.res"

"$(OUTDIR)\Data.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

OutDir=.\Debug
TargetPath=.\Debug\Data.dll
InputPath=.\Debug\Data.dll
SOURCE="$(InputPath)"

"$(OUTDIR)\regsvr32.trg" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	regsvr32 /s /c "$(TargetPath)" 
	echo regsvr32 exec. time > "$(OutDir)\regsvr32.trg" 
	echo Execute mtxrereg.exe before using MTS components in MTS 
<< 
	

!ENDIF 


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("Data.dep")
!INCLUDE "Data.dep"
!ELSE 
!MESSAGE Warning: cannot find "Data.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "Data - Win32 Release MinSize" || "$(CFG)" == "Data - Win32 Release MinDependency" || "$(CFG)" == "Data - Win32 Debug"
SOURCE=.\Data.cpp

!IF  "$(CFG)" == "Data - Win32 Release MinSize"


"$(INTDIR)\Data.obj"	"$(INTDIR)\Data.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Data.pch" ".\Data_i.c"


!ELSEIF  "$(CFG)" == "Data - Win32 Release MinDependency"


"$(INTDIR)\Data.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Data.pch"


!ELSEIF  "$(CFG)" == "Data - Win32 Debug"


"$(INTDIR)\Data.obj"	"$(INTDIR)\Data.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Data.pch"


!ENDIF 

SOURCE=.\Data.idl

!IF  "$(CFG)" == "Data - Win32 Release MinSize"

MTL_SWITCHES=/tlb ".\Data.tlb" /h "Data.h" /iid "Data_i.c" /Oicf 

".\Data.tlb"	".\Data.h"	".\Data_i.c" : $(SOURCE) "$(INTDIR)"
	$(MTL) @<<
  $(MTL_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Data - Win32 Release MinDependency"

MTL_SWITCHES=/tlb ".\Data.tlb" /h "Data.h" /iid "Data_i.c" /Oicf 

".\Data.tlb"	".\Data.h"	".\Data_i.c" : $(SOURCE) "$(INTDIR)"
	$(MTL) @<<
  $(MTL_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Data - Win32 Debug"

MTL_SWITCHES=/tlb ".\Data.tlb" /h "Data.h" /iid "Data_i.c" /Oicf 

".\Data.tlb"	".\Data.h"	".\Data_i.c" : $(SOURCE) "$(INTDIR)"
	$(MTL) @<<
  $(MTL_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\Data.rc

"$(INTDIR)\Data.res" : $(SOURCE) "$(INTDIR)" ".\Data.tlb"
	$(RSC) $(RSC_PROJ) $(SOURCE)


SOURCE=.\Dynamic.cpp

!IF  "$(CFG)" == "Data - Win32 Release MinSize"


"$(INTDIR)\Dynamic.obj"	"$(INTDIR)\Dynamic.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Data.pch"


!ELSEIF  "$(CFG)" == "Data - Win32 Release MinDependency"


"$(INTDIR)\Dynamic.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Data.pch"


!ELSEIF  "$(CFG)" == "Data - Win32 Debug"


"$(INTDIR)\Dynamic.obj"	"$(INTDIR)\Dynamic.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Data.pch"


!ENDIF 

SOURCE=.\Entity.cpp

!IF  "$(CFG)" == "Data - Win32 Release MinSize"


"$(INTDIR)\Entity.obj"	"$(INTDIR)\Entity.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Data.pch"


!ELSEIF  "$(CFG)" == "Data - Win32 Release MinDependency"


"$(INTDIR)\Entity.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Data.pch"


!ELSEIF  "$(CFG)" == "Data - Win32 Debug"


"$(INTDIR)\Entity.obj"	"$(INTDIR)\Entity.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Data.pch"


!ENDIF 

SOURCE=.\entity_except.cpp

!IF  "$(CFG)" == "Data - Win32 Release MinSize"

CPP_SWITCHES=/nologo /G6 /MT /W3 /GX /Ox /D "NDEBUG" /D "_MBCS" /D "_ATL_DLL" /D "WIN32" /D "_WINDOWS" /D "_USRDLL" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\Data.pch" /Yu"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\entity_except.obj"	"$(INTDIR)\entity_except.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Data.pch"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Data - Win32 Release MinDependency"

CPP_SWITCHES=/nologo /G6 /MT /W3 /GX /Ox /D "NDEBUG" /D "_MBCS" /D "_ATL_STATIC_REGISTRY" /D "WIN32" /D "_WINDOWS" /D "_USRDLL" /Fp"$(INTDIR)\Data.pch" /Yu"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\entity_except.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Data.pch"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Data - Win32 Debug"

CPP_SWITCHES=/nologo /G6 /MTd /W3 /Gm /GX /ZI /Od /D "_DEBUG" /D "_MBCS" /D "WIN32" /D "_WINDOWS" /D "_USRDLL" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\Data.pch" /Yu"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\entity_except.obj"	"$(INTDIR)\entity_except.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Data.pch"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\heap.cpp

!IF  "$(CFG)" == "Data - Win32 Release MinSize"


"$(INTDIR)\heap.obj"	"$(INTDIR)\heap.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Data.pch"


!ELSEIF  "$(CFG)" == "Data - Win32 Release MinDependency"


"$(INTDIR)\heap.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Data.pch"


!ELSEIF  "$(CFG)" == "Data - Win32 Debug"


"$(INTDIR)\heap.obj"	"$(INTDIR)\heap.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Data.pch"


!ENDIF 

SOURCE=.\ODBCData.cpp

!IF  "$(CFG)" == "Data - Win32 Release MinSize"


"$(INTDIR)\ODBCData.obj"	"$(INTDIR)\ODBCData.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Data.pch"


!ELSEIF  "$(CFG)" == "Data - Win32 Release MinDependency"


"$(INTDIR)\ODBCData.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Data.pch"


!ELSEIF  "$(CFG)" == "Data - Win32 Debug"


"$(INTDIR)\ODBCData.obj"	"$(INTDIR)\ODBCData.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Data.pch"


!ENDIF 

SOURCE=.\pooleddata.cpp

!IF  "$(CFG)" == "Data - Win32 Release MinSize"


"$(INTDIR)\pooleddata.obj"	"$(INTDIR)\pooleddata.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Data.pch"


!ELSEIF  "$(CFG)" == "Data - Win32 Release MinDependency"


"$(INTDIR)\pooleddata.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Data.pch"


!ELSEIF  "$(CFG)" == "Data - Win32 Debug"


"$(INTDIR)\pooleddata.obj"	"$(INTDIR)\pooleddata.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Data.pch"


!ENDIF 

SOURCE=.\Proxy.cpp

!IF  "$(CFG)" == "Data - Win32 Release MinSize"


"$(INTDIR)\Proxy.obj"	"$(INTDIR)\Proxy.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Data.pch"


!ELSEIF  "$(CFG)" == "Data - Win32 Release MinDependency"


"$(INTDIR)\Proxy.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Data.pch"


!ELSEIF  "$(CFG)" == "Data - Win32 Debug"


"$(INTDIR)\Proxy.obj"	"$(INTDIR)\Proxy.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Data.pch"


!ENDIF 

SOURCE=.\Proxy_except.cpp

!IF  "$(CFG)" == "Data - Win32 Release MinSize"

CPP_SWITCHES=/nologo /G6 /MT /W3 /GX /Ox /D "NDEBUG" /D "_MBCS" /D "_ATL_DLL" /D "WIN32" /D "_WINDOWS" /D "_USRDLL" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\Data.pch" /Yu"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\Proxy_except.obj"	"$(INTDIR)\Proxy_except.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Data.pch"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Data - Win32 Release MinDependency"

CPP_SWITCHES=/nologo /G6 /MT /W3 /GX /Ox /D "NDEBUG" /D "_MBCS" /D "_ATL_STATIC_REGISTRY" /D "WIN32" /D "_WINDOWS" /D "_USRDLL" /Fp"$(INTDIR)\Data.pch" /Yu"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\Proxy_except.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Data.pch"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Data - Win32 Debug"

CPP_SWITCHES=/nologo /G6 /MTd /W3 /Gm /GX /ZI /Od /D "_DEBUG" /D "_MBCS" /D "WIN32" /D "_WINDOWS" /D "_USRDLL" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\Data.pch" /Yu"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\Proxy_except.obj"	"$(INTDIR)\Proxy_except.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Data.pch"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\Resultset.cpp

!IF  "$(CFG)" == "Data - Win32 Release MinSize"


"$(INTDIR)\Resultset.obj"	"$(INTDIR)\Resultset.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Data.pch"


!ELSEIF  "$(CFG)" == "Data - Win32 Release MinDependency"


"$(INTDIR)\Resultset.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Data.pch"


!ELSEIF  "$(CFG)" == "Data - Win32 Debug"


"$(INTDIR)\Resultset.obj"	"$(INTDIR)\Resultset.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Data.pch"


!ENDIF 

SOURCE=.\StdAfx.cpp

!IF  "$(CFG)" == "Data - Win32 Release MinSize"

CPP_SWITCHES=/nologo /G6 /MT /W3 /GX /Ox /D "NDEBUG" /D "_MBCS" /D "_ATL_DLL" /D "WIN32" /D "_WINDOWS" /D "_USRDLL" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\Data.pch" /Yc"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\StdAfx.obj"	"$(INTDIR)\StdAfx.sbr"	"$(INTDIR)\Data.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Data - Win32 Release MinDependency"

CPP_SWITCHES=/nologo /G6 /MT /W3 /GX /Ox /D "NDEBUG" /D "_MBCS" /D "_ATL_STATIC_REGISTRY" /D "WIN32" /D "_WINDOWS" /D "_USRDLL" /Fp"$(INTDIR)\Data.pch" /Yc"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\StdAfx.obj"	"$(INTDIR)\Data.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Data - Win32 Debug"

CPP_SWITCHES=/nologo /G6 /MTd /W3 /Gm /GX /ZI /Od /D "_DEBUG" /D "_MBCS" /D "WIN32" /D "_WINDOWS" /D "_USRDLL" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\Data.pch" /Yc"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\StdAfx.obj"	"$(INTDIR)\StdAfx.sbr"	"$(INTDIR)\Data.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\string.CPP

!IF  "$(CFG)" == "Data - Win32 Release MinSize"


"$(INTDIR)\string.obj"	"$(INTDIR)\string.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Data.pch"


!ELSEIF  "$(CFG)" == "Data - Win32 Release MinDependency"


"$(INTDIR)\string.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Data.pch"


!ELSEIF  "$(CFG)" == "Data - Win32 Debug"


"$(INTDIR)\string.obj"	"$(INTDIR)\string.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Data.pch"


!ENDIF 

SOURCE=.\string_fast.cpp

!IF  "$(CFG)" == "Data - Win32 Release MinSize"


"$(INTDIR)\string_fast.obj"	"$(INTDIR)\string_fast.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Data.pch"


!ELSEIF  "$(CFG)" == "Data - Win32 Release MinDependency"


"$(INTDIR)\string_fast.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Data.pch"


!ELSEIF  "$(CFG)" == "Data - Win32 Debug"


"$(INTDIR)\string_fast.obj"	"$(INTDIR)\string_fast.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Data.pch"


!ENDIF 

SOURCE=.\Utils.cpp

!IF  "$(CFG)" == "Data - Win32 Release MinSize"


"$(INTDIR)\Utils.obj"	"$(INTDIR)\Utils.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Data.pch"


!ELSEIF  "$(CFG)" == "Data - Win32 Release MinDependency"


"$(INTDIR)\Utils.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Data.pch"


!ELSEIF  "$(CFG)" == "Data - Win32 Debug"


"$(INTDIR)\Utils.obj"	"$(INTDIR)\Utils.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Data.pch"


!ENDIF 


!ENDIF 

