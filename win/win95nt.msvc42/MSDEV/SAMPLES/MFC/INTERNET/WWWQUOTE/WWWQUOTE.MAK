# Microsoft Developer Studio Generated NMAKE File, Format Version 4.10
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

!IF "$(CFG)" == ""
CFG=WWWQuote - Win32 Static Debug
!MESSAGE No configuration specified.  Defaulting to WWWQuote - Win32 Static\
 Debug.
!ENDIF 

!IF "$(CFG)" != "WWWQuote - Win32 Release" && "$(CFG)" !=\
 "WWWQuote - Win32 Debug" && "$(CFG)" != "WWWQuote - Win32 Static Release" &&\
 "$(CFG)" != "WWWQuote - Win32 Static Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "wwwquote.mak" CFG="WWWQuote - Win32 Static Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "WWWQuote - Win32 Release" (based on\
 "Win32 (x86) Dynamic-Link Library")
!MESSAGE "WWWQuote - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "WWWQuote - Win32 Static Release" (based on\
 "Win32 (x86) Dynamic-Link Library")
!MESSAGE "WWWQuote - Win32 Static Debug" (based on\
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
# PROP Target_Last_Scanned "WWWQuote - Win32 Static Debug"
CPP=cl.exe
RSC=rc.exe
MTL=mktyplib.exe

!IF  "$(CFG)" == "WWWQuote - Win32 Release"

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

ALL : "$(OUTDIR)\wwwquote.dll"

CLEAN : 
	-@erase "$(INTDIR)\queries.obj"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\WWWQuote.obj"
	-@erase "$(INTDIR)\wwwquote.pch"
	-@erase "$(INTDIR)\WWWQuote.res"
	-@erase "$(OUTDIR)\wwwquote.dll"
	-@erase "$(OUTDIR)\wwwquote.exp"
	-@erase "$(OUTDIR)\wwwquote.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_USRDLL" /Yu"stdafx.h" /c
CPP_PROJ=/nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_USRDLL" /Fp"$(INTDIR)/wwwquote.pch"\
 /Yu"stdafx.h" /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\Release/
CPP_SBRS=.\.
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/WWWQuote.res" /d "NDEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/wwwquote.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 /nologo /subsystem:windows /dll /machine:I386
LINK32_FLAGS=/nologo /subsystem:windows /dll /incremental:no\
 /pdb:"$(OUTDIR)/wwwquote.pdb" /machine:I386 /def:".\WWWQuote.def"\
 /out:"$(OUTDIR)/wwwquote.dll" /implib:"$(OUTDIR)/wwwquote.lib" 
DEF_FILE= \
	".\WWWQuote.def"
LINK32_OBJS= \
	"$(INTDIR)\queries.obj" \
	"$(INTDIR)\StdAfx.obj" \
	"$(INTDIR)\WWWQuote.obj" \
	"$(INTDIR)\WWWQuote.res"

"$(OUTDIR)\wwwquote.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "WWWQuote - Win32 Debug"

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

ALL : "$(OUTDIR)\wwwquote.dll"

CLEAN : 
	-@erase "$(INTDIR)\queries.obj"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\vc40.idb"
	-@erase "$(INTDIR)\vc40.pdb"
	-@erase "$(INTDIR)\WWWQuote.obj"
	-@erase "$(INTDIR)\wwwquote.pch"
	-@erase "$(INTDIR)\WWWQuote.res"
	-@erase "$(OUTDIR)\wwwquote.dll"
	-@erase "$(OUTDIR)\wwwquote.exp"
	-@erase "$(OUTDIR)\wwwquote.ilk"
	-@erase "$(OUTDIR)\wwwquote.lib"
	-@erase "$(OUTDIR)\wwwquote.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_USRDLL" /Yu"stdafx.h" /c
CPP_PROJ=/nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_USRDLL" /Fp"$(INTDIR)/wwwquote.pch"\
 /Yu"stdafx.h" /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\Debug/
CPP_SBRS=.\.
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/WWWQuote.res" /d "_DEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/wwwquote.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /debug /machine:I386
# ADD LINK32 /nologo /subsystem:windows /dll /debug /machine:I386
LINK32_FLAGS=/nologo /subsystem:windows /dll /incremental:yes\
 /pdb:"$(OUTDIR)/wwwquote.pdb" /debug /machine:I386 /def:".\WWWQuote.def"\
 /out:"$(OUTDIR)/wwwquote.dll" /implib:"$(OUTDIR)/wwwquote.lib" 
DEF_FILE= \
	".\WWWQuote.def"
LINK32_OBJS= \
	"$(INTDIR)\queries.obj" \
	"$(INTDIR)\StdAfx.obj" \
	"$(INTDIR)\WWWQuote.obj" \
	"$(INTDIR)\WWWQuote.res"

"$(OUTDIR)\wwwquote.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "WWWQuote - Win32 Static Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "WWWQuote"
# PROP BASE Intermediate_Dir "WWWQuote"
# PROP BASE Target_Dir ""
# PROP Use_MFC 5
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "SRelease"
# PROP Intermediate_Dir "SRelease"
# PROP Target_Dir ""
OUTDIR=.\SRelease
INTDIR=.\SRelease

ALL : "$(OUTDIR)\wwwquote.dll"

CLEAN : 
	-@erase "$(INTDIR)\queries.obj"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\WWWQuote.obj"
	-@erase "$(INTDIR)\wwwquote.pch"
	-@erase "$(INTDIR)\WWWQuote.res"
	-@erase "$(OUTDIR)\wwwquote.dll"
	-@erase "$(OUTDIR)\wwwquote.exp"
	-@erase "$(OUTDIR)\wwwquote.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_USRDLL" /Yu"stdafx.h" /c
# ADD CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_USRDLL" /D "_WINDLL" /D "_MBCS" /Yu"stdafx.h" /c
CPP_PROJ=/nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_USRDLL" /D "_WINDLL" /D "_MBCS" /Fp"$(INTDIR)/wwwquote.pch" /Yu"stdafx.h"\
 /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\SRelease/
CPP_SBRS=.\.
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/WWWQuote.res" /d "NDEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/wwwquote.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 /nologo /subsystem:windows /dll /machine:I386
LINK32_FLAGS=/nologo /subsystem:windows /dll /incremental:no\
 /pdb:"$(OUTDIR)/wwwquote.pdb" /machine:I386 /def:".\WWWQuote.def"\
 /out:"$(OUTDIR)/wwwquote.dll" /implib:"$(OUTDIR)/wwwquote.lib" 
DEF_FILE= \
	".\WWWQuote.def"
LINK32_OBJS= \
	"$(INTDIR)\queries.obj" \
	"$(INTDIR)\StdAfx.obj" \
	"$(INTDIR)\WWWQuote.obj" \
	"$(INTDIR)\WWWQuote.res"

"$(OUTDIR)\wwwquote.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "WWWQuote - Win32 Static Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "WWWQuot0"
# PROP BASE Intermediate_Dir "WWWQuot0"
# PROP BASE Target_Dir ""
# PROP Use_MFC 5
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "SDebug"
# PROP Intermediate_Dir "SDebug"
# PROP Target_Dir ""
OUTDIR=.\SDebug
INTDIR=.\SDebug

ALL : "$(OUTDIR)\wwwquote.dll"

CLEAN : 
	-@erase "$(INTDIR)\queries.obj"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\vc40.idb"
	-@erase "$(INTDIR)\vc40.pdb"
	-@erase "$(INTDIR)\WWWQuote.obj"
	-@erase "$(INTDIR)\wwwquote.pch"
	-@erase "$(INTDIR)\WWWQuote.res"
	-@erase "$(OUTDIR)\wwwquote.dll"
	-@erase "$(OUTDIR)\wwwquote.exp"
	-@erase "$(OUTDIR)\wwwquote.ilk"
	-@erase "$(OUTDIR)\wwwquote.lib"
	-@erase "$(OUTDIR)\wwwquote.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_USRDLL" /Yu"stdafx.h" /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_USRDLL" /D "_WINDLL" /D "_MBCS" /Yu"stdafx.h" /c
CPP_PROJ=/nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /D "_USRDLL" /D "_WINDLL" /D "_MBCS" /Fp"$(INTDIR)/wwwquote.pch" /Yu"stdafx.h"\
 /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\SDebug/
CPP_SBRS=.\.
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/WWWQuote.res" /d "_DEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/wwwquote.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /debug /machine:I386
# ADD LINK32 /nologo /subsystem:windows /dll /debug /machine:I386
LINK32_FLAGS=/nologo /subsystem:windows /dll /incremental:yes\
 /pdb:"$(OUTDIR)/wwwquote.pdb" /debug /machine:I386 /def:".\WWWQuote.def"\
 /out:"$(OUTDIR)/wwwquote.dll" /implib:"$(OUTDIR)/wwwquote.lib" 
DEF_FILE= \
	".\WWWQuote.def"
LINK32_OBJS= \
	"$(INTDIR)\queries.obj" \
	"$(INTDIR)\StdAfx.obj" \
	"$(INTDIR)\WWWQuote.obj" \
	"$(INTDIR)\WWWQuote.res"

"$(OUTDIR)\wwwquote.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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

# Name "WWWQuote - Win32 Release"
# Name "WWWQuote - Win32 Debug"
# Name "WWWQuote - Win32 Static Release"
# Name "WWWQuote - Win32 Static Debug"

!IF  "$(CFG)" == "WWWQuote - Win32 Release"

!ELSEIF  "$(CFG)" == "WWWQuote - Win32 Debug"

!ELSEIF  "$(CFG)" == "WWWQuote - Win32 Static Release"

!ELSEIF  "$(CFG)" == "WWWQuote - Win32 Static Debug"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\WWWQuote.cpp
DEP_CPP_WWWQU=\
	".\Queries.h"\
	".\StdAfx.h"\
	".\WWWQuote.h"\
	

!IF  "$(CFG)" == "WWWQuote - Win32 Release"


"$(INTDIR)\WWWQuote.obj" : $(SOURCE) $(DEP_CPP_WWWQU) "$(INTDIR)"\
 "$(INTDIR)\wwwquote.pch"


!ELSEIF  "$(CFG)" == "WWWQuote - Win32 Debug"


"$(INTDIR)\WWWQuote.obj" : $(SOURCE) $(DEP_CPP_WWWQU) "$(INTDIR)"\
 "$(INTDIR)\wwwquote.pch"


!ELSEIF  "$(CFG)" == "WWWQuote - Win32 Static Release"


"$(INTDIR)\WWWQuote.obj" : $(SOURCE) $(DEP_CPP_WWWQU) "$(INTDIR)"\
 "$(INTDIR)\wwwquote.pch"


!ELSEIF  "$(CFG)" == "WWWQuote - Win32 Static Debug"


"$(INTDIR)\WWWQuote.obj" : $(SOURCE) $(DEP_CPP_WWWQU) "$(INTDIR)"\
 "$(INTDIR)\wwwquote.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\WWWQuote.def

!IF  "$(CFG)" == "WWWQuote - Win32 Release"

!ELSEIF  "$(CFG)" == "WWWQuote - Win32 Debug"

!ELSEIF  "$(CFG)" == "WWWQuote - Win32 Static Release"

!ELSEIF  "$(CFG)" == "WWWQuote - Win32 Static Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\StdAfx.cpp
DEP_CPP_STDAF=\
	".\StdAfx.h"\
	

!IF  "$(CFG)" == "WWWQuote - Win32 Release"

# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_USRDLL" /Fp"$(INTDIR)/wwwquote.pch"\
 /Yc"stdafx.h" /Fo"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\StdAfx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\wwwquote.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "WWWQuote - Win32 Debug"

# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_USRDLL" /Fp"$(INTDIR)/wwwquote.pch"\
 /Yc"stdafx.h" /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\StdAfx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\wwwquote.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "WWWQuote - Win32 Static Release"

# ADD BASE CPP /Yc"stdafx.h"
# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_USRDLL" /D "_WINDLL" /D "_MBCS" /Fp"$(INTDIR)/wwwquote.pch" /Yc"stdafx.h"\
 /Fo"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\StdAfx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\wwwquote.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "WWWQuote - Win32 Static Debug"

# ADD BASE CPP /Yc"stdafx.h"
# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /D "_USRDLL" /D "_WINDLL" /D "_MBCS" /Fp"$(INTDIR)/wwwquote.pch" /Yc"stdafx.h"\
 /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\StdAfx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\wwwquote.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\WWWQuote.rc
DEP_RSC_WWWQUO=\
	".\HTML\issueerr.htm"\
	".\HTML\Next.htm"\
	".\HTML\Prev.htm"\
	".\HTML\quoteerr.htm"\
	".\HTML\quothead.htm"\
	".\HTML\Selhead.htm"\
	".\HTML\Seltail.htm"\
	".\HTML\welcome.htm"\
	".\res\WWWQuote.rc2"\
	

!IF  "$(CFG)" == "WWWQuote - Win32 Release"


"$(INTDIR)\WWWQuote.res" : $(SOURCE) $(DEP_RSC_WWWQUO) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "WWWQuote - Win32 Debug"


"$(INTDIR)\WWWQuote.res" : $(SOURCE) $(DEP_RSC_WWWQUO) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "WWWQuote - Win32 Static Release"


"$(INTDIR)\WWWQuote.res" : $(SOURCE) $(DEP_RSC_WWWQUO) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "WWWQuote - Win32 Static Debug"


"$(INTDIR)\WWWQuote.res" : $(SOURCE) $(DEP_RSC_WWWQUO) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\queries.cpp
DEP_CPP_QUERI=\
	".\Queries.h"\
	".\StdAfx.h"\
	".\WWWQuote.h"\
	

!IF  "$(CFG)" == "WWWQuote - Win32 Release"


"$(INTDIR)\queries.obj" : $(SOURCE) $(DEP_CPP_QUERI) "$(INTDIR)"\
 "$(INTDIR)\wwwquote.pch"


!ELSEIF  "$(CFG)" == "WWWQuote - Win32 Debug"


"$(INTDIR)\queries.obj" : $(SOURCE) $(DEP_CPP_QUERI) "$(INTDIR)"\
 "$(INTDIR)\wwwquote.pch"


!ELSEIF  "$(CFG)" == "WWWQuote - Win32 Static Release"


"$(INTDIR)\queries.obj" : $(SOURCE) $(DEP_CPP_QUERI) "$(INTDIR)"\
 "$(INTDIR)\wwwquote.pch"


!ELSEIF  "$(CFG)" == "WWWQuote - Win32 Static Debug"


"$(INTDIR)\queries.obj" : $(SOURCE) $(DEP_CPP_QUERI) "$(INTDIR)"\
 "$(INTDIR)\wwwquote.pch"


!ENDIF 

# End Source File
# End Target
# End Project
################################################################################
