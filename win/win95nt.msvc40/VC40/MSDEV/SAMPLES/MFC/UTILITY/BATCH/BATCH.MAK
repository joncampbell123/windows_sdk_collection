# Microsoft Visual C++ Generated NMAKE File, Format Version 40000
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

!IF "$(CFG)" == ""
CFG=Batch - Win32 Debug
!MESSAGE No configuration specified.  Defaulting to Batch - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "Batch - Win32 Release" && "$(CFG)" != "Batch - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "Batch.mak" CFG="Batch - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Batch - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "Batch - Win32 Debug" (based on "Win32 (x86) Application")
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
# PROP Target_Last_Scanned "Batch - Win32 Debug"
CPP=cl.exe
MTL=mktyplib.exe
RSC=rc.exe

!IF  "$(CFG)" == "Batch - Win32 Release"

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

ALL : "$(OUTDIR)\Batch.exe" "$(OUTDIR)\Batch.bsc"

CLEAN : 
	-@erase ".\Release\Batch.idb"
	-@erase ".\Release\Batch.pch"
	-@erase ".\Release\Batch.bsc"
	-@erase ".\Release\Interlea.sbr"
	-@erase ".\Release\Batch.sbr"
	-@erase ".\Release\Batchdoc.sbr"
	-@erase ".\Release\Batchvw.sbr"
	-@erase ".\Release\Stdafx.sbr"
	-@erase ".\Release\Destdird.sbr"
	-@erase ".\Release\Mainfrm.sbr"
	-@erase ".\Release\Compress.sbr"
	-@erase ".\Release\Batch.exe"
	-@erase ".\Release\Compress.obj"
	-@erase ".\Release\Interlea.obj"
	-@erase ".\Release\Batch.obj"
	-@erase ".\Release\Batchdoc.obj"
	-@erase ".\Release\Batchvw.obj"
	-@erase ".\Release\Stdafx.obj"
	-@erase ".\Release\Destdird.obj"
	-@erase ".\Release\Mainfrm.obj"
	-@erase ".\Release\Batch.res"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /Yu"Stdafx.h" /c
CPP_PROJ=/nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /FR"$(INTDIR)/" /Fp"$(INTDIR)/Batch.pch" /Yu"Stdafx.h"\
 /Fo"$(INTDIR)/" /Fd"$(OUTDIR)/Batch.pdb" /c 
CPP_OBJS=.\Release/
CPP_SBRS=.\Release/
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/Batch.res" /d "NDEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/Batch.bsc" 
BSC32_SBRS= \
	"$(INTDIR)/Interlea.sbr" \
	"$(INTDIR)/Batch.sbr" \
	"$(INTDIR)/Batchdoc.sbr" \
	"$(INTDIR)/Batchvw.sbr" \
	"$(INTDIR)/Stdafx.sbr" \
	"$(INTDIR)/Destdird.sbr" \
	"$(INTDIR)/Mainfrm.sbr" \
	"$(INTDIR)/Compress.sbr"

"$(OUTDIR)\Batch.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 vfw32.lib msacm32.lib /nologo /subsystem:windows /machine:I386
LINK32_FLAGS=vfw32.lib msacm32.lib /nologo /subsystem:windows /incremental:no\
 /pdb:"$(OUTDIR)/Batch.pdb" /machine:I386 /out:"$(OUTDIR)/Batch.exe" 
LINK32_OBJS= \
	"$(INTDIR)/Compress.obj" \
	"$(INTDIR)/Interlea.obj" \
	"$(INTDIR)/Batch.obj" \
	"$(INTDIR)/Batchdoc.obj" \
	"$(INTDIR)/Batchvw.obj" \
	"$(INTDIR)/Stdafx.obj" \
	"$(INTDIR)/Destdird.obj" \
	"$(INTDIR)/Mainfrm.obj" \
	"$(INTDIR)/Batch.res"

"$(OUTDIR)\Batch.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "Batch - Win32 Debug"

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

ALL : "$(OUTDIR)\Batch.exe" "$(OUTDIR)\Batch.bsc"

CLEAN : 
	-@erase ".\Debug\Batch.pdb"
	-@erase ".\Debug\Batch.pch"
	-@erase ".\Debug\Stdafx.obj"
	-@erase ".\Debug\Interlea.obj"
	-@erase ".\Debug\Batch.obj"
	-@erase ".\Debug\Batchdoc.obj"
	-@erase ".\Debug\Destdird.obj"
	-@erase ".\Debug\Batchvw.obj"
	-@erase ".\Debug\Compress.obj"
	-@erase ".\Debug\Mainfrm.obj"
	-@erase ".\Debug\Batch.res"
	-@erase ".\Debug\Batch.idb"
	-@erase ".\Debug\Batch.bsc"
	-@erase ".\Debug\Interlea.sbr"
	-@erase ".\Debug\Batch.sbr"
	-@erase ".\Debug\Batchdoc.sbr"
	-@erase ".\Debug\Destdird.sbr"
	-@erase ".\Debug\Batchvw.sbr"
	-@erase ".\Debug\Compress.sbr"
	-@erase ".\Debug\Mainfrm.sbr"
	-@erase ".\Debug\Stdafx.sbr"
	-@erase ".\Debug\Batch.exe"
	-@erase ".\Debug\Batch.ilk"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /Yu"Stdafx.h" /c
CPP_PROJ=/nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /D "_AFXDLL" /D "_MBCS" /FR"$(INTDIR)/" /Fp"$(INTDIR)/Batch.pch" /Yu"Stdafx.h"\
 /Fo"$(INTDIR)/" /Fd"$(OUTDIR)/Batch.pdb" /c 
CPP_OBJS=.\Debug/
CPP_SBRS=.\Debug/
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/Batch.res" /d "_DEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/Batch.bsc" 
BSC32_SBRS= \
	"$(INTDIR)/Interlea.sbr" \
	"$(INTDIR)/Batch.sbr" \
	"$(INTDIR)/Batchdoc.sbr" \
	"$(INTDIR)/Destdird.sbr" \
	"$(INTDIR)/Batchvw.sbr" \
	"$(INTDIR)/Compress.sbr" \
	"$(INTDIR)/Mainfrm.sbr" \
	"$(INTDIR)/Stdafx.sbr"

"$(OUTDIR)\Batch.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386
# ADD LINK32 vfw32.lib msacm32.lib /nologo /subsystem:windows /debug /machine:I386
LINK32_FLAGS=vfw32.lib msacm32.lib /nologo /subsystem:windows /incremental:yes\
 /pdb:"$(OUTDIR)/Batch.pdb" /debug /machine:I386 /out:"$(OUTDIR)/Batch.exe" 
LINK32_OBJS= \
	"$(INTDIR)/Stdafx.obj" \
	"$(INTDIR)/Interlea.obj" \
	"$(INTDIR)/Batch.obj" \
	"$(INTDIR)/Batchdoc.obj" \
	"$(INTDIR)/Destdird.obj" \
	"$(INTDIR)/Batchvw.obj" \
	"$(INTDIR)/Compress.obj" \
	"$(INTDIR)/Mainfrm.obj" \
	"$(INTDIR)/Batch.res"

"$(OUTDIR)\Batch.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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

# Name "Batch - Win32 Release"
# Name "Batch - Win32 Debug"

!IF  "$(CFG)" == "Batch - Win32 Release"

!ELSEIF  "$(CFG)" == "Batch - Win32 Debug"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\Batch.rc
DEP_RSC_BATCH=\
	".\Batch.ico"\
	".\Batchdoc.ico"\
	".\Toolbar.bmp"\
	".\Batch.rc2"\
	

"$(INTDIR)\Batch.res" : $(SOURCE) $(DEP_RSC_BATCH) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\Batch.cpp
DEP_CPP_BATCH_=\
	".\Batch.h"\
	".\Resource.h"\
	".\Batchdoc.h"\
	".\Batchvw.h"\
	".\Mainfrm.h"\
	".\Stdafx.h"\
	

!IF  "$(CFG)" == "Batch - Win32 Release"


"$(INTDIR)\Batch.obj" : $(SOURCE) $(DEP_CPP_BATCH_) "$(INTDIR)"\
 "$(INTDIR)\Batch.pch"

"$(INTDIR)\Batch.sbr" : $(SOURCE) $(DEP_CPP_BATCH_) "$(INTDIR)"\
 "$(INTDIR)\Batch.pch"

"$(INTDIR)\Batch.idb" : $(SOURCE) $(DEP_CPP_BATCH_) "$(INTDIR)"\
 "$(INTDIR)\Batch.pch"


!ELSEIF  "$(CFG)" == "Batch - Win32 Debug"


"$(INTDIR)\Batch.obj" : $(SOURCE) $(DEP_CPP_BATCH_) "$(INTDIR)"\
 "$(INTDIR)\Batch.pch"

"$(INTDIR)\Batch.sbr" : $(SOURCE) $(DEP_CPP_BATCH_) "$(INTDIR)"\
 "$(INTDIR)\Batch.pch"

"$(INTDIR)\Batch.pdb" : $(SOURCE) $(DEP_CPP_BATCH_) "$(INTDIR)"\
 "$(INTDIR)\Batch.pch"

"$(INTDIR)\Batch.idb" : $(SOURCE) $(DEP_CPP_BATCH_) "$(INTDIR)"\
 "$(INTDIR)\Batch.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Batchdoc.cpp
DEP_CPP_BATCHD=\
	".\Batch.h"\
	".\Resource.h"\
	".\Batchdoc.h"\
	".\Destdird.h"\
	".\Interlea.h"\
	".\Mainfrm.h"\
	".\Stdafx.h"\
	

!IF  "$(CFG)" == "Batch - Win32 Release"


"$(INTDIR)\Batchdoc.obj" : $(SOURCE) $(DEP_CPP_BATCHD) "$(INTDIR)"\
 "$(INTDIR)\Batch.pch"

"$(INTDIR)\Batchdoc.sbr" : $(SOURCE) $(DEP_CPP_BATCHD) "$(INTDIR)"\
 "$(INTDIR)\Batch.pch"

"$(INTDIR)\Batch.idb" : $(SOURCE) $(DEP_CPP_BATCHD) "$(INTDIR)"\
 "$(INTDIR)\Batch.pch"


!ELSEIF  "$(CFG)" == "Batch - Win32 Debug"


"$(INTDIR)\Batchdoc.obj" : $(SOURCE) $(DEP_CPP_BATCHD) "$(INTDIR)"\
 "$(INTDIR)\Batch.pch"

"$(INTDIR)\Batchdoc.sbr" : $(SOURCE) $(DEP_CPP_BATCHD) "$(INTDIR)"\
 "$(INTDIR)\Batch.pch"

"$(INTDIR)\Batch.pdb" : $(SOURCE) $(DEP_CPP_BATCHD) "$(INTDIR)"\
 "$(INTDIR)\Batch.pch"

"$(INTDIR)\Batch.idb" : $(SOURCE) $(DEP_CPP_BATCHD) "$(INTDIR)"\
 "$(INTDIR)\Batch.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Batchvw.cpp
DEP_CPP_BATCHV=\
	".\Batch.h"\
	".\Resource.h"\
	".\Batchdoc.h"\
	".\Batchvw.h"\
	".\Stdafx.h"\
	

!IF  "$(CFG)" == "Batch - Win32 Release"


"$(INTDIR)\Batchvw.obj" : $(SOURCE) $(DEP_CPP_BATCHV) "$(INTDIR)"\
 "$(INTDIR)\Batch.pch"

"$(INTDIR)\Batchvw.sbr" : $(SOURCE) $(DEP_CPP_BATCHV) "$(INTDIR)"\
 "$(INTDIR)\Batch.pch"

"$(INTDIR)\Batch.idb" : $(SOURCE) $(DEP_CPP_BATCHV) "$(INTDIR)"\
 "$(INTDIR)\Batch.pch"


!ELSEIF  "$(CFG)" == "Batch - Win32 Debug"


"$(INTDIR)\Batchvw.obj" : $(SOURCE) $(DEP_CPP_BATCHV) "$(INTDIR)"\
 "$(INTDIR)\Batch.pch"

"$(INTDIR)\Batchvw.sbr" : $(SOURCE) $(DEP_CPP_BATCHV) "$(INTDIR)"\
 "$(INTDIR)\Batch.pch"

"$(INTDIR)\Batch.pdb" : $(SOURCE) $(DEP_CPP_BATCHV) "$(INTDIR)"\
 "$(INTDIR)\Batch.pch"

"$(INTDIR)\Batch.idb" : $(SOURCE) $(DEP_CPP_BATCHV) "$(INTDIR)"\
 "$(INTDIR)\Batch.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Compress.cpp
DEP_CPP_COMPR=\
	".\Batch.h"\
	".\Resource.h"\
	".\Compress.h"\
	".\Stdafx.h"\
	

!IF  "$(CFG)" == "Batch - Win32 Release"


"$(INTDIR)\Compress.obj" : $(SOURCE) $(DEP_CPP_COMPR) "$(INTDIR)"\
 "$(INTDIR)\Batch.pch"

"$(INTDIR)\Compress.sbr" : $(SOURCE) $(DEP_CPP_COMPR) "$(INTDIR)"\
 "$(INTDIR)\Batch.pch"

"$(INTDIR)\Batch.idb" : $(SOURCE) $(DEP_CPP_COMPR) "$(INTDIR)"\
 "$(INTDIR)\Batch.pch"


!ELSEIF  "$(CFG)" == "Batch - Win32 Debug"


"$(INTDIR)\Compress.obj" : $(SOURCE) $(DEP_CPP_COMPR) "$(INTDIR)"\
 "$(INTDIR)\Batch.pch"

"$(INTDIR)\Compress.sbr" : $(SOURCE) $(DEP_CPP_COMPR) "$(INTDIR)"\
 "$(INTDIR)\Batch.pch"

"$(INTDIR)\Batch.pdb" : $(SOURCE) $(DEP_CPP_COMPR) "$(INTDIR)"\
 "$(INTDIR)\Batch.pch"

"$(INTDIR)\Batch.idb" : $(SOURCE) $(DEP_CPP_COMPR) "$(INTDIR)"\
 "$(INTDIR)\Batch.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Destdird.cpp
DEP_CPP_DESTD=\
	".\Batch.h"\
	".\Resource.h"\
	".\Destdird.h"\
	".\Stdafx.h"\
	

!IF  "$(CFG)" == "Batch - Win32 Release"


"$(INTDIR)\Destdird.obj" : $(SOURCE) $(DEP_CPP_DESTD) "$(INTDIR)"\
 "$(INTDIR)\Batch.pch"

"$(INTDIR)\Destdird.sbr" : $(SOURCE) $(DEP_CPP_DESTD) "$(INTDIR)"\
 "$(INTDIR)\Batch.pch"

"$(INTDIR)\Batch.idb" : $(SOURCE) $(DEP_CPP_DESTD) "$(INTDIR)"\
 "$(INTDIR)\Batch.pch"


!ELSEIF  "$(CFG)" == "Batch - Win32 Debug"


"$(INTDIR)\Destdird.obj" : $(SOURCE) $(DEP_CPP_DESTD) "$(INTDIR)"\
 "$(INTDIR)\Batch.pch"

"$(INTDIR)\Destdird.sbr" : $(SOURCE) $(DEP_CPP_DESTD) "$(INTDIR)"\
 "$(INTDIR)\Batch.pch"

"$(INTDIR)\Batch.pdb" : $(SOURCE) $(DEP_CPP_DESTD) "$(INTDIR)"\
 "$(INTDIR)\Batch.pch"

"$(INTDIR)\Batch.idb" : $(SOURCE) $(DEP_CPP_DESTD) "$(INTDIR)"\
 "$(INTDIR)\Batch.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Interlea.cpp
DEP_CPP_INTER=\
	".\Batch.h"\
	".\Resource.h"\
	".\Interlea.h"\
	".\Stdafx.h"\
	

!IF  "$(CFG)" == "Batch - Win32 Release"


"$(INTDIR)\Interlea.obj" : $(SOURCE) $(DEP_CPP_INTER) "$(INTDIR)"\
 "$(INTDIR)\Batch.pch"

"$(INTDIR)\Interlea.sbr" : $(SOURCE) $(DEP_CPP_INTER) "$(INTDIR)"\
 "$(INTDIR)\Batch.pch"

"$(INTDIR)\Batch.idb" : $(SOURCE) $(DEP_CPP_INTER) "$(INTDIR)"\
 "$(INTDIR)\Batch.pch"


!ELSEIF  "$(CFG)" == "Batch - Win32 Debug"


"$(INTDIR)\Interlea.obj" : $(SOURCE) $(DEP_CPP_INTER) "$(INTDIR)"\
 "$(INTDIR)\Batch.pch"

"$(INTDIR)\Interlea.sbr" : $(SOURCE) $(DEP_CPP_INTER) "$(INTDIR)"\
 "$(INTDIR)\Batch.pch"

"$(INTDIR)\Batch.pdb" : $(SOURCE) $(DEP_CPP_INTER) "$(INTDIR)"\
 "$(INTDIR)\Batch.pch"

"$(INTDIR)\Batch.idb" : $(SOURCE) $(DEP_CPP_INTER) "$(INTDIR)"\
 "$(INTDIR)\Batch.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Mainfrm.cpp
DEP_CPP_MAINF=\
	".\Batch.h"\
	".\Resource.h"\
	".\Mainfrm.h"\
	".\Stdafx.h"\
	

!IF  "$(CFG)" == "Batch - Win32 Release"


"$(INTDIR)\Mainfrm.obj" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\Batch.pch"

"$(INTDIR)\Mainfrm.sbr" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\Batch.pch"

"$(INTDIR)\Batch.idb" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\Batch.pch"


!ELSEIF  "$(CFG)" == "Batch - Win32 Debug"


"$(INTDIR)\Mainfrm.obj" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\Batch.pch"

"$(INTDIR)\Mainfrm.sbr" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\Batch.pch"

"$(INTDIR)\Batch.pdb" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\Batch.pch"

"$(INTDIR)\Batch.idb" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\Batch.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Stdafx.cpp
DEP_CPP_STDAF=\
	".\Stdafx.h"\
	

!IF  "$(CFG)" == "Batch - Win32 Release"

# ADD CPP /Yc

BuildCmds= \
	$(CPP) /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /FR"$(INTDIR)/" /Fp"$(INTDIR)/Batch.pch" /Yc\
 /Fo"$(INTDIR)/" /Fd"$(OUTDIR)/Batch.pdb" /c $(SOURCE) \
	

"$(INTDIR)\Stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\Stdafx.sbr" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\Batch.idb" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\Batch.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "Batch - Win32 Debug"

# ADD CPP /Yc"Stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /D "_AFXDLL" /D "_MBCS" /FR"$(INTDIR)/" /Fp"$(INTDIR)/Batch.pch" /Yc"Stdafx.h"\
 /Fo"$(INTDIR)/" /Fd"$(OUTDIR)/Batch.pdb" /c $(SOURCE) \
	

"$(INTDIR)\Stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\Stdafx.sbr" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\Batch.pdb" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\Batch.idb" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\Batch.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
# End Target
# End Project
################################################################################
