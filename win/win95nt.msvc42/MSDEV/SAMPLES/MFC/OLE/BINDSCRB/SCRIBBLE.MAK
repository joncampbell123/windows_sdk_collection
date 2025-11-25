# Microsoft Developer Studio Generated NMAKE File, Format Version 42001
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

!IF "$(CFG)" == ""
CFG=SCRIBBLE - Win32 (80x86) Debug
!MESSAGE No configuration specified.  Defaulting to SCRIBBLE - Win32 (80x86)\
 Debug.
!ENDIF 

!IF "$(CFG)" != "SCRIBBLE - Win32 (80x86) Debug" && "$(CFG)" !=\
 "SCRIBBLE - Win32 (80x86) Release"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "Scribble.mak" CFG="SCRIBBLE - Win32 (80x86) Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "SCRIBBLE - Win32 (80x86) Debug" (based on "Win32 (x86) Application")
!MESSAGE "SCRIBBLE - Win32 (80x86) Release" (based on\
 "Win32 (x86) Application")
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
# PROP Target_Last_Scanned "SCRIBBLE - Win32 (80x86) Release"
CPP=cl.exe
RSC=rc.exe
MTL=mktyplib.exe

!IF  "$(CFG)" == "SCRIBBLE - Win32 (80x86) Debug"

# PROP BASE Use_MFC 5
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "WinDebug"
# PROP BASE Intermediate_Dir "WinDebug"
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "WinDebug"
# PROP Intermediate_Dir "WinDebug"
OUTDIR=.\WinDebug
INTDIR=.\WinDebug

ALL : "$(OUTDIR)\Scribble.exe"

CLEAN : 
	-@erase "$(INTDIR)\IPFRAME.OBJ"
	-@erase "$(INTDIR)\MAINFRM.OBJ"
	-@erase "$(INTDIR)\PENDLG.OBJ"
	-@erase "$(INTDIR)\SCRIBBLE.OBJ"
	-@erase "$(INTDIR)\Scribble.pch"
	-@erase "$(INTDIR)\SCRIBBLE.res"
	-@erase "$(INTDIR)\SCRIBDOC.OBJ"
	-@erase "$(INTDIR)\SCRIBFRM.OBJ"
	-@erase "$(INTDIR)\SCRIBITM.OBJ"
	-@erase "$(INTDIR)\SCRIBVW.OBJ"
	-@erase "$(INTDIR)\STDAFX.OBJ"
	-@erase "$(INTDIR)\vc40.idb"
	-@erase "$(INTDIR)\vc40.pdb"
	-@erase "$(OUTDIR)\Scribble.exe"
	-@erase "$(OUTDIR)\Scribble.ilk"
	-@erase "$(OUTDIR)\Scribble.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

.\WinDebug\SCRIBBLE.bsc : $(OUTDIR)  $(BSC32_SBRS)
# ADD BASE CPP /nologo /MT /W3 /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /FR /Yu"stdafx.h" /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
# SUBTRACT CPP /Fr
CPP_PROJ=/nologo /MDd /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS"\
 /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/Scribble.pch" /Yu"stdafx.h"\
 /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\WinDebug/
CPP_SBRS=.\.
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/SCRIBBLE.res" /d "_DEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# SUBTRACT BASE BSC32 /Iu
# ADD BSC32 /nologo
# SUBTRACT BSC32 /Iu
BSC32_FLAGS=/nologo /o"$(OUTDIR)/Scribble.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib nafxcwd.lib ctl3d32.lib /nologo /subsystem:windows /debug /machine:I386
# ADD LINK32 uuid3.lib /nologo /subsystem:windows /debug /machine:I386
LINK32_FLAGS=uuid3.lib /nologo /subsystem:windows /incremental:yes\
 /pdb:"$(OUTDIR)/Scribble.pdb" /debug /machine:I386\
 /out:"$(OUTDIR)/Scribble.exe" 
LINK32_OBJS= \
	"$(INTDIR)\IPFRAME.OBJ" \
	"$(INTDIR)\MAINFRM.OBJ" \
	"$(INTDIR)\PENDLG.OBJ" \
	"$(INTDIR)\SCRIBBLE.OBJ" \
	"$(INTDIR)\SCRIBBLE.res" \
	"$(INTDIR)\SCRIBDOC.OBJ" \
	"$(INTDIR)\SCRIBFRM.OBJ" \
	"$(INTDIR)\SCRIBITM.OBJ" \
	"$(INTDIR)\SCRIBVW.OBJ" \
	"$(INTDIR)\STDAFX.OBJ"

"$(OUTDIR)\Scribble.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "SCRIBBLE - Win32 (80x86) Release"

# PROP BASE Use_MFC 5
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "WinRel"
# PROP BASE Intermediate_Dir "WinRel"
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "WinRel"
# PROP Intermediate_Dir "WinRel"
OUTDIR=.\WinRel
INTDIR=.\WinRel

ALL : "$(OUTDIR)\Scribble.exe"

CLEAN : 
	-@erase "$(INTDIR)\IPFRAME.OBJ"
	-@erase "$(INTDIR)\MAINFRM.OBJ"
	-@erase "$(INTDIR)\PENDLG.OBJ"
	-@erase "$(INTDIR)\SCRIBBLE.OBJ"
	-@erase "$(INTDIR)\Scribble.pch"
	-@erase "$(INTDIR)\SCRIBBLE.res"
	-@erase "$(INTDIR)\SCRIBDOC.OBJ"
	-@erase "$(INTDIR)\SCRIBFRM.OBJ"
	-@erase "$(INTDIR)\SCRIBITM.OBJ"
	-@erase "$(INTDIR)\SCRIBVW.OBJ"
	-@erase "$(INTDIR)\STDAFX.OBJ"
	-@erase "$(OUTDIR)\Scribble.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

.\WinRel\SCRIBBLE.bsc : $(OUTDIR)  $(BSC32_SBRS)
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /FR /Yu"stdafx.h" /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
# SUBTRACT CPP /Fr
CPP_PROJ=/nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/Scribble.pch" /Yu"stdafx.h" /Fo"$(INTDIR)/"\
 /c 
CPP_OBJS=.\WinRel/
CPP_SBRS=.\.
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/SCRIBBLE.res" /d "NDEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# SUBTRACT BASE BSC32 /Iu
# ADD BSC32 /nologo
# SUBTRACT BSC32 /Iu
BSC32_FLAGS=/nologo /o"$(OUTDIR)/Scribble.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib nafxcw.lib ctl3d32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 uuid3.lib /nologo /subsystem:windows /machine:I386
LINK32_FLAGS=uuid3.lib /nologo /subsystem:windows /incremental:no\
 /pdb:"$(OUTDIR)/Scribble.pdb" /machine:I386 /out:"$(OUTDIR)/Scribble.exe" 
LINK32_OBJS= \
	"$(INTDIR)\IPFRAME.OBJ" \
	"$(INTDIR)\MAINFRM.OBJ" \
	"$(INTDIR)\PENDLG.OBJ" \
	"$(INTDIR)\SCRIBBLE.OBJ" \
	"$(INTDIR)\SCRIBBLE.res" \
	"$(INTDIR)\SCRIBDOC.OBJ" \
	"$(INTDIR)\SCRIBFRM.OBJ" \
	"$(INTDIR)\SCRIBITM.OBJ" \
	"$(INTDIR)\SCRIBVW.OBJ" \
	"$(INTDIR)\STDAFX.OBJ"

"$(OUTDIR)\Scribble.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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

# Name "SCRIBBLE - Win32 (80x86) Debug"
# Name "SCRIBBLE - Win32 (80x86) Release"

!IF  "$(CFG)" == "SCRIBBLE - Win32 (80x86) Debug"

!ELSEIF  "$(CFG)" == "SCRIBBLE - Win32 (80x86) Release"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\STDAFX.CPP

!IF  "$(CFG)" == "SCRIBBLE - Win32 (80x86) Debug"

DEP_CPP_STDAF=\
	".\Stdafx.h"\
	{$(INCLUDE)}"\Afxdocob.h"\
	{$(INCLUDE)}"\DocObj.h"\
	
# ADD BASE CPP /Yc"stdafx.h"
# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MDd /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS"\
 /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/Scribble.pch" /Yc"stdafx.h"\
 /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\STDAFX.OBJ" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\Scribble.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "SCRIBBLE - Win32 (80x86) Release"

DEP_CPP_STDAF=\
	"..\..\..\INETSDK\INCLUDE\servprov.h"\
	".\Stdafx.h"\
	{$(INCLUDE)}"\Afxdocob.h"\
	{$(INCLUDE)}"\DocObj.h"\
	
# ADD BASE CPP /Yc"stdafx.h"
# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/Scribble.pch" /Yc"stdafx.h" /Fo"$(INTDIR)/"\
 /c $(SOURCE) \
	

"$(INTDIR)\STDAFX.OBJ" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\Scribble.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\SCRIBBLE.CPP

!IF  "$(CFG)" == "SCRIBBLE - Win32 (80x86) Debug"

DEP_CPP_SCRIB=\
	".\Ipframe.h"\
	".\Mainfrm.h"\
	".\Scribble.h"\
	".\Scribdoc.h"\
	".\Scribfrm.h"\
	".\Scribvw.h"\
	".\Stdafx.h"\
	{$(INCLUDE)}"\Afxdocob.h"\
	{$(INCLUDE)}"\DocObj.h"\
	

"$(INTDIR)\SCRIBBLE.OBJ" : $(SOURCE) $(DEP_CPP_SCRIB) "$(INTDIR)"\
 "$(INTDIR)\Scribble.pch"


!ELSEIF  "$(CFG)" == "SCRIBBLE - Win32 (80x86) Release"

DEP_CPP_SCRIB=\
	"..\..\..\INETSDK\INCLUDE\servprov.h"\
	".\Ipframe.h"\
	".\Mainfrm.h"\
	".\Scribble.h"\
	".\Scribdoc.h"\
	".\Scribfrm.h"\
	".\Scribvw.h"\
	".\Stdafx.h"\
	{$(INCLUDE)}"\Afxdocob.h"\
	{$(INCLUDE)}"\DocObj.h"\
	

"$(INTDIR)\SCRIBBLE.OBJ" : $(SOURCE) $(DEP_CPP_SCRIB) "$(INTDIR)"\
 "$(INTDIR)\Scribble.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\MAINFRM.CPP

!IF  "$(CFG)" == "SCRIBBLE - Win32 (80x86) Debug"

DEP_CPP_MAINF=\
	".\Mainfrm.h"\
	".\Scribble.h"\
	".\Stdafx.h"\
	{$(INCLUDE)}"\Afxdocob.h"\
	{$(INCLUDE)}"\DocObj.h"\
	

"$(INTDIR)\MAINFRM.OBJ" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\Scribble.pch"


!ELSEIF  "$(CFG)" == "SCRIBBLE - Win32 (80x86) Release"

DEP_CPP_MAINF=\
	"..\..\..\INETSDK\INCLUDE\servprov.h"\
	".\Mainfrm.h"\
	".\Scribble.h"\
	".\Stdafx.h"\
	{$(INCLUDE)}"\Afxdocob.h"\
	{$(INCLUDE)}"\DocObj.h"\
	

"$(INTDIR)\MAINFRM.OBJ" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\Scribble.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\SCRIBDOC.CPP

!IF  "$(CFG)" == "SCRIBBLE - Win32 (80x86) Debug"

DEP_CPP_SCRIBD=\
	".\PENDLG.H"\
	".\Scribble.h"\
	".\Scribdoc.h"\
	".\SCRIBITM.H"\
	".\Scribvw.h"\
	".\Stdafx.h"\
	

"$(INTDIR)\SCRIBDOC.OBJ" : $(SOURCE) $(DEP_CPP_SCRIBD) "$(INTDIR)"\
 "$(INTDIR)\Scribble.pch"


!ELSEIF  "$(CFG)" == "SCRIBBLE - Win32 (80x86) Release"

DEP_CPP_SCRIBD=\
	"..\..\..\INETSDK\INCLUDE\servprov.h"\
	".\PENDLG.H"\
	".\Scribble.h"\
	".\Scribdoc.h"\
	".\SCRIBITM.H"\
	".\Scribvw.h"\
	".\Stdafx.h"\
	{$(INCLUDE)}"\Afxdocob.h"\
	{$(INCLUDE)}"\DocObj.h"\
	

"$(INTDIR)\SCRIBDOC.OBJ" : $(SOURCE) $(DEP_CPP_SCRIBD) "$(INTDIR)"\
 "$(INTDIR)\Scribble.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\SCRIBVW.CPP

!IF  "$(CFG)" == "SCRIBBLE - Win32 (80x86) Debug"

DEP_CPP_SCRIBV=\
	".\Scribble.h"\
	".\Scribdoc.h"\
	".\Scribvw.h"\
	".\Stdafx.h"\
	

"$(INTDIR)\SCRIBVW.OBJ" : $(SOURCE) $(DEP_CPP_SCRIBV) "$(INTDIR)"\
 "$(INTDIR)\Scribble.pch"


!ELSEIF  "$(CFG)" == "SCRIBBLE - Win32 (80x86) Release"

DEP_CPP_SCRIBV=\
	"..\..\..\INETSDK\INCLUDE\servprov.h"\
	".\Scribble.h"\
	".\Scribdoc.h"\
	".\Scribvw.h"\
	".\Stdafx.h"\
	{$(INCLUDE)}"\Afxdocob.h"\
	{$(INCLUDE)}"\DocObj.h"\
	

"$(INTDIR)\SCRIBVW.OBJ" : $(SOURCE) $(DEP_CPP_SCRIBV) "$(INTDIR)"\
 "$(INTDIR)\Scribble.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\SCRIBBLE.RC
DEP_RSC_SCRIBB=\
	".\RES\ITOOLBAR.BMP"\
	".\RES\SCRIBBLE.ICO"\
	".\RES\SCRIBBLE.RC2"\
	".\RES\SCRIBDOC.ICO"\
	".\RES\TOOLBAR.BMP"\
	

"$(INTDIR)\SCRIBBLE.res" : $(SOURCE) $(DEP_RSC_SCRIBB) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\PENDLG.CPP
DEP_CPP_PENDL=\
	"..\..\..\INETSDK\INCLUDE\servprov.h"\
	".\PENDLG.H"\
	".\Scribble.h"\
	".\Stdafx.h"\
	{$(INCLUDE)}"\Afxdocob.h"\
	{$(INCLUDE)}"\DocObj.h"\
	

"$(INTDIR)\PENDLG.OBJ" : $(SOURCE) $(DEP_CPP_PENDL) "$(INTDIR)"\
 "$(INTDIR)\Scribble.pch"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\SCRIBFRM.CPP
DEP_CPP_SCRIBF=\
	"..\..\..\INETSDK\INCLUDE\servprov.h"\
	".\Scribble.h"\
	".\Scribfrm.h"\
	".\Stdafx.h"\
	{$(INCLUDE)}"\Afxdocob.h"\
	{$(INCLUDE)}"\DocObj.h"\
	

"$(INTDIR)\SCRIBFRM.OBJ" : $(SOURCE) $(DEP_CPP_SCRIBF) "$(INTDIR)"\
 "$(INTDIR)\Scribble.pch"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\IPFRAME.CPP

!IF  "$(CFG)" == "SCRIBBLE - Win32 (80x86) Debug"

DEP_CPP_IPFRA=\
	".\Ipframe.h"\
	".\Scribble.h"\
	".\Stdafx.h"\
	

"$(INTDIR)\IPFRAME.OBJ" : $(SOURCE) $(DEP_CPP_IPFRA) "$(INTDIR)"\
 "$(INTDIR)\Scribble.pch"


!ELSEIF  "$(CFG)" == "SCRIBBLE - Win32 (80x86) Release"

DEP_CPP_IPFRA=\
	"..\..\..\INETSDK\INCLUDE\servprov.h"\
	".\Ipframe.h"\
	".\Scribble.h"\
	".\Stdafx.h"\
	{$(INCLUDE)}"\Afxdocob.h"\
	{$(INCLUDE)}"\DocObj.h"\
	

"$(INTDIR)\IPFRAME.OBJ" : $(SOURCE) $(DEP_CPP_IPFRA) "$(INTDIR)"\
 "$(INTDIR)\Scribble.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\SCRIBITM.CPP

!IF  "$(CFG)" == "SCRIBBLE - Win32 (80x86) Debug"

DEP_CPP_SCRIBI=\
	".\Scribble.h"\
	".\Scribdoc.h"\
	".\SCRIBITM.H"\
	".\Stdafx.h"\
	

"$(INTDIR)\SCRIBITM.OBJ" : $(SOURCE) $(DEP_CPP_SCRIBI) "$(INTDIR)"\
 "$(INTDIR)\Scribble.pch"


!ELSEIF  "$(CFG)" == "SCRIBBLE - Win32 (80x86) Release"

DEP_CPP_SCRIBI=\
	"..\..\..\INETSDK\INCLUDE\servprov.h"\
	".\Scribble.h"\
	".\Scribdoc.h"\
	".\SCRIBITM.H"\
	".\Stdafx.h"\
	{$(INCLUDE)}"\Afxdocob.h"\
	{$(INCLUDE)}"\DocObj.h"\
	

"$(INTDIR)\SCRIBITM.OBJ" : $(SOURCE) $(DEP_CPP_SCRIBI) "$(INTDIR)"\
 "$(INTDIR)\Scribble.pch"


!ENDIF 

# End Source File
# End Target
# End Project
################################################################################
