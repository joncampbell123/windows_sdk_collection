# Microsoft Visual C++ Generated NMAKE File, Format Version 40000
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

!IF "$(CFG)" == ""
CFG=WordPad - Win32 Debug
!MESSAGE No configuration specified.  Defaulting to WordPad - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "WordPad - Win32 Release" && "$(CFG)" !=\
 "WordPad - Win32 Debug" && "$(CFG)" != "WordPad - Win32 Unicode Release" &&\
 "$(CFG)" != "WordPad - Win32 Unicode Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "wordpad.mak" CFG="WordPad - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "WordPad - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "WordPad - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE "WordPad - Win32 Unicode Release" (based on "Win32 (x86) Application")
!MESSAGE "WordPad - Win32 Unicode Debug" (based on "Win32 (x86) Application")
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
# PROP Target_Last_Scanned "WordPad - Win32 Debug"
RSC=rc.exe
CPP=cl.exe
MTL=mktyplib.exe

!IF  "$(CFG)" == "WordPad - Win32 Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir Release
# PROP BASE Intermediate_Dir Release
# PROP BASE Target_Dir
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir Release
# PROP Intermediate_Dir Release
# PROP Target_Dir
OUTDIR=.\Release
INTDIR=.\Release

ALL : "$(OUTDIR)\wordpad.exe" "$(OUTDIR)\wordpad.bsc"

CLEAN : 
	-@erase ".\Release\wordpad.idb"
	-@erase ".\Release\wordpad.pch"
	-@erase ".\Release\wordpad.bsc"
	-@erase ".\Release\mainfrm.sbr"
	-@erase ".\Release\colorlis.sbr"
	-@erase ".\Release\cntritem.sbr"
	-@erase ".\Release\wordpvw.sbr"
	-@erase ".\Release\ddxm.sbr"
	-@erase ".\Release\datedial.sbr"
	-@erase ".\Release\wordpad.sbr"
	-@erase ".\Release\strings.sbr"
	-@erase ".\Release\wordpdoc.sbr"
	-@erase ".\Release\ipframe.sbr"
	-@erase ".\Release\optionsh.sbr"
	-@erase ".\Release\filenewd.sbr"
	-@erase ".\Release\ruler.sbr"
	-@erase ".\Release\multconv.sbr"
	-@erase ".\Release\formatta.sbr"
	-@erase ".\Release\options.sbr"
	-@erase ".\Release\listdlg.sbr"
	-@erase ".\Release\formatba.sbr"
	-@erase ".\Release\key.sbr"
	-@erase ".\Release\formatpa.sbr"
	-@erase ".\Release\doctype.sbr"
	-@erase ".\Release\chicdial.sbr"
	-@erase ".\Release\unitspag.sbr"
	-@erase ".\Release\splash.sbr"
	-@erase ".\Release\pageset.sbr"
	-@erase ".\Release\buttondi.sbr"
	-@erase ".\Release\stdafx.sbr"
	-@erase ".\Release\srvritem.sbr"
	-@erase ".\Release\docopt.sbr"
	-@erase ".\Release\wordpad.exe"
	-@erase ".\Release\listdlg.obj"
	-@erase ".\Release\formatba.obj"
	-@erase ".\Release\key.obj"
	-@erase ".\Release\formatpa.obj"
	-@erase ".\Release\doctype.obj"
	-@erase ".\Release\chicdial.obj"
	-@erase ".\Release\unitspag.obj"
	-@erase ".\Release\splash.obj"
	-@erase ".\Release\pageset.obj"
	-@erase ".\Release\buttondi.obj"
	-@erase ".\Release\stdafx.obj"
	-@erase ".\Release\srvritem.obj"
	-@erase ".\Release\docopt.obj"
	-@erase ".\Release\mainfrm.obj"
	-@erase ".\Release\colorlis.obj"
	-@erase ".\Release\cntritem.obj"
	-@erase ".\Release\wordpvw.obj"
	-@erase ".\Release\ddxm.obj"
	-@erase ".\Release\datedial.obj"
	-@erase ".\Release\wordpad.obj"
	-@erase ".\Release\strings.obj"
	-@erase ".\Release\wordpdoc.obj"
	-@erase ".\Release\ipframe.obj"
	-@erase ".\Release\optionsh.obj"
	-@erase ".\Release\filenewd.obj"
	-@erase ".\Release\ruler.obj"
	-@erase ".\Release\multconv.obj"
	-@erase ".\Release\formatta.obj"
	-@erase ".\Release\options.obj"
	-@erase ".\Release\wordpad.res"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(OUTDIR)\WordPad.bsc" : "$(OUTDIR)" $(BSC32_SBRS)

"$(OUTDIR)\WordPad.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)

# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /Yu"stdafx.h" /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /Yu"stdafx.h" /c
CPP_PROJ=/nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /FR"$(INTDIR)/" /Fp"$(INTDIR)/wordpad.pch" /Yu"stdafx.h"\
 /Fo"$(INTDIR)/" /Fd"$(OUTDIR)/wordpad.pdb" /c 
CPP_OBJS=.\Release/
CPP_SBRS=.\Release/
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/wordpad.res" /d "NDEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/wordpad.bsc" 
BSC32_SBRS= \
	"$(INTDIR)/mainfrm.sbr" \
	"$(INTDIR)/colorlis.sbr" \
	"$(INTDIR)/cntritem.sbr" \
	"$(INTDIR)/wordpvw.sbr" \
	"$(INTDIR)/ddxm.sbr" \
	"$(INTDIR)/datedial.sbr" \
	"$(INTDIR)/wordpad.sbr" \
	"$(INTDIR)/strings.sbr" \
	"$(INTDIR)/wordpdoc.sbr" \
	"$(INTDIR)/ipframe.sbr" \
	"$(INTDIR)/optionsh.sbr" \
	"$(INTDIR)/filenewd.sbr" \
	"$(INTDIR)/ruler.sbr" \
	"$(INTDIR)/multconv.sbr" \
	"$(INTDIR)/formatta.sbr" \
	"$(INTDIR)/options.sbr" \
	"$(INTDIR)/listdlg.sbr" \
	"$(INTDIR)/formatba.sbr" \
	"$(INTDIR)/key.sbr" \
	"$(INTDIR)/formatpa.sbr" \
	"$(INTDIR)/doctype.sbr" \
	"$(INTDIR)/chicdial.sbr" \
	"$(INTDIR)/unitspag.sbr" \
	"$(INTDIR)/splash.sbr" \
	"$(INTDIR)/pageset.sbr" \
	"$(INTDIR)/buttondi.sbr" \
	"$(INTDIR)/stdafx.sbr" \
	"$(INTDIR)/srvritem.sbr" \
	"$(INTDIR)/docopt.sbr"

"$(OUTDIR)\wordpad.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 /nologo /subsystem:windows /machine:I386
LINK32_FLAGS=/nologo /subsystem:windows /incremental:no\
 /pdb:"$(OUTDIR)/wordpad.pdb" /machine:I386 /out:"$(OUTDIR)/wordpad.exe" 
LINK32_OBJS= \
	"$(INTDIR)/listdlg.obj" \
	"$(INTDIR)/formatba.obj" \
	"$(INTDIR)/key.obj" \
	"$(INTDIR)/formatpa.obj" \
	"$(INTDIR)/doctype.obj" \
	"$(INTDIR)/chicdial.obj" \
	"$(INTDIR)/unitspag.obj" \
	"$(INTDIR)/splash.obj" \
	"$(INTDIR)/pageset.obj" \
	"$(INTDIR)/buttondi.obj" \
	"$(INTDIR)/stdafx.obj" \
	"$(INTDIR)/srvritem.obj" \
	"$(INTDIR)/docopt.obj" \
	"$(INTDIR)/mainfrm.obj" \
	"$(INTDIR)/colorlis.obj" \
	"$(INTDIR)/cntritem.obj" \
	"$(INTDIR)/wordpvw.obj" \
	"$(INTDIR)/ddxm.obj" \
	"$(INTDIR)/datedial.obj" \
	"$(INTDIR)/wordpad.obj" \
	"$(INTDIR)/strings.obj" \
	"$(INTDIR)/wordpdoc.obj" \
	"$(INTDIR)/ipframe.obj" \
	"$(INTDIR)/optionsh.obj" \
	"$(INTDIR)/filenewd.obj" \
	"$(INTDIR)/ruler.obj" \
	"$(INTDIR)/multconv.obj" \
	"$(INTDIR)/formatta.obj" \
	"$(INTDIR)/options.obj" \
	"$(INTDIR)/wordpad.res"

"$(OUTDIR)\wordpad.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "WordPad - Win32 Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir Debug
# PROP BASE Intermediate_Dir Debug
# PROP BASE Target_Dir
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir Debug
# PROP Intermediate_Dir Debug
# PROP Target_Dir
OUTDIR=.\Debug
INTDIR=.\Debug

ALL : "$(OUTDIR)\wordpad.exe" "$(OUTDIR)\wordpad.bsc"

CLEAN : 
	-@erase ".\Debug\wordpad.pdb"
	-@erase ".\Debug\wordpad.pch"
	-@erase ".\Debug\srvritem.obj"
	-@erase ".\Debug\options.obj"
	-@erase ".\Debug\listdlg.obj"
	-@erase ".\Debug\colorlis.obj"
	-@erase ".\Debug\stdafx.obj"
	-@erase ".\Debug\splash.obj"
	-@erase ".\Debug\doctype.obj"
	-@erase ".\Debug\cntritem.obj"
	-@erase ".\Debug\docopt.obj"
	-@erase ".\Debug\pageset.obj"
	-@erase ".\Debug\datedial.obj"
	-@erase ".\Debug\mainfrm.obj"
	-@erase ".\Debug\key.obj"
	-@erase ".\Debug\wordpdoc.obj"
	-@erase ".\Debug\optionsh.obj"
	-@erase ".\Debug\filenewd.obj"
	-@erase ".\Debug\wordpvw.obj"
	-@erase ".\Debug\ruler.obj"
	-@erase ".\Debug\multconv.obj"
	-@erase ".\Debug\wordpad.obj"
	-@erase ".\Debug\strings.obj"
	-@erase ".\Debug\ipframe.obj"
	-@erase ".\Debug\formatta.obj"
	-@erase ".\Debug\formatba.obj"
	-@erase ".\Debug\ddxm.obj"
	-@erase ".\Debug\chicdial.obj"
	-@erase ".\Debug\unitspag.obj"
	-@erase ".\Debug\formatpa.obj"
	-@erase ".\Debug\buttondi.obj"
	-@erase ".\Debug\wordpad.res"
	-@erase ".\Debug\wordpad.idb"
	-@erase ".\Debug\wordpad.bsc"
	-@erase ".\Debug\pageset.sbr"
	-@erase ".\Debug\datedial.sbr"
	-@erase ".\Debug\mainfrm.sbr"
	-@erase ".\Debug\key.sbr"
	-@erase ".\Debug\wordpdoc.sbr"
	-@erase ".\Debug\optionsh.sbr"
	-@erase ".\Debug\filenewd.sbr"
	-@erase ".\Debug\wordpvw.sbr"
	-@erase ".\Debug\ruler.sbr"
	-@erase ".\Debug\multconv.sbr"
	-@erase ".\Debug\wordpad.sbr"
	-@erase ".\Debug\strings.sbr"
	-@erase ".\Debug\ipframe.sbr"
	-@erase ".\Debug\formatta.sbr"
	-@erase ".\Debug\formatba.sbr"
	-@erase ".\Debug\ddxm.sbr"
	-@erase ".\Debug\chicdial.sbr"
	-@erase ".\Debug\unitspag.sbr"
	-@erase ".\Debug\formatpa.sbr"
	-@erase ".\Debug\buttondi.sbr"
	-@erase ".\Debug\srvritem.sbr"
	-@erase ".\Debug\options.sbr"
	-@erase ".\Debug\listdlg.sbr"
	-@erase ".\Debug\colorlis.sbr"
	-@erase ".\Debug\stdafx.sbr"
	-@erase ".\Debug\splash.sbr"
	-@erase ".\Debug\doctype.sbr"
	-@erase ".\Debug\cntritem.sbr"
	-@erase ".\Debug\docopt.sbr"
	-@erase ".\Debug\wordpad.exe"
	-@erase ".\Debug\wordpad.ilk"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(OUTDIR)\WordPad.bsc" : "$(OUTDIR)" $(BSC32_SBRS)

"$(OUTDIR)\WordPad.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)

# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /Yu"stdafx.h" /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /Yu"stdafx.h" /c
CPP_PROJ=/nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /D "_AFXDLL" /D "_MBCS" /FR"$(INTDIR)/" /Fp"$(INTDIR)/wordpad.pch"\
 /Yu"stdafx.h" /Fo"$(INTDIR)/" /Fd"$(OUTDIR)/wordpad.pdb" /c 
CPP_OBJS=.\Debug/
CPP_SBRS=.\Debug/
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/wordpad.res" /d "_DEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/wordpad.bsc" 
BSC32_SBRS= \
	"$(INTDIR)/pageset.sbr" \
	"$(INTDIR)/datedial.sbr" \
	"$(INTDIR)/mainfrm.sbr" \
	"$(INTDIR)/key.sbr" \
	"$(INTDIR)/wordpdoc.sbr" \
	"$(INTDIR)/optionsh.sbr" \
	"$(INTDIR)/filenewd.sbr" \
	"$(INTDIR)/wordpvw.sbr" \
	"$(INTDIR)/ruler.sbr" \
	"$(INTDIR)/multconv.sbr" \
	"$(INTDIR)/wordpad.sbr" \
	"$(INTDIR)/strings.sbr" \
	"$(INTDIR)/ipframe.sbr" \
	"$(INTDIR)/formatta.sbr" \
	"$(INTDIR)/formatba.sbr" \
	"$(INTDIR)/ddxm.sbr" \
	"$(INTDIR)/chicdial.sbr" \
	"$(INTDIR)/unitspag.sbr" \
	"$(INTDIR)/formatpa.sbr" \
	"$(INTDIR)/buttondi.sbr" \
	"$(INTDIR)/srvritem.sbr" \
	"$(INTDIR)/options.sbr" \
	"$(INTDIR)/listdlg.sbr" \
	"$(INTDIR)/colorlis.sbr" \
	"$(INTDIR)/stdafx.sbr" \
	"$(INTDIR)/splash.sbr" \
	"$(INTDIR)/doctype.sbr" \
	"$(INTDIR)/cntritem.sbr" \
	"$(INTDIR)/docopt.sbr"

"$(OUTDIR)\wordpad.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386
# ADD LINK32 /nologo /subsystem:windows /debug /machine:I386
LINK32_FLAGS=/nologo /subsystem:windows /incremental:yes\
 /pdb:"$(OUTDIR)/wordpad.pdb" /debug /machine:I386 /out:"$(OUTDIR)/wordpad.exe" 
LINK32_OBJS= \
	"$(INTDIR)/srvritem.obj" \
	"$(INTDIR)/options.obj" \
	"$(INTDIR)/listdlg.obj" \
	"$(INTDIR)/colorlis.obj" \
	"$(INTDIR)/stdafx.obj" \
	"$(INTDIR)/splash.obj" \
	"$(INTDIR)/doctype.obj" \
	"$(INTDIR)/cntritem.obj" \
	"$(INTDIR)/docopt.obj" \
	"$(INTDIR)/pageset.obj" \
	"$(INTDIR)/datedial.obj" \
	"$(INTDIR)/mainfrm.obj" \
	"$(INTDIR)/key.obj" \
	"$(INTDIR)/wordpdoc.obj" \
	"$(INTDIR)/optionsh.obj" \
	"$(INTDIR)/filenewd.obj" \
	"$(INTDIR)/wordpvw.obj" \
	"$(INTDIR)/ruler.obj" \
	"$(INTDIR)/multconv.obj" \
	"$(INTDIR)/wordpad.obj" \
	"$(INTDIR)/strings.obj" \
	"$(INTDIR)/ipframe.obj" \
	"$(INTDIR)/formatta.obj" \
	"$(INTDIR)/formatba.obj" \
	"$(INTDIR)/ddxm.obj" \
	"$(INTDIR)/chicdial.obj" \
	"$(INTDIR)/unitspag.obj" \
	"$(INTDIR)/formatpa.obj" \
	"$(INTDIR)/buttondi.obj" \
	"$(INTDIR)/wordpad.res"

"$(OUTDIR)\wordpad.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "WordPad - Win32 Unicode Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir WordPad_
# PROP BASE Intermediate_Dir WordPad_
# PROP BASE Target_Dir
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir UniRelease
# PROP Intermediate_Dir UniRelease
# PROP Target_Dir
OUTDIR=.\UniRelease
INTDIR=.\UniRelease

ALL : "$(OUTDIR)\wordpad.exe" "$(OUTDIR)\wordpad.bsc"

CLEAN : 
	-@erase ".\UniRelease\wordpad.idb"
	-@erase ".\UniRelease\wordpad.pch"
	-@erase ".\UniRelease\wordpad.bsc"
	-@erase ".\UniRelease\optionsh.sbr"
	-@erase ".\UniRelease\key.sbr"
	-@erase ".\UniRelease\filenewd.sbr"
	-@erase ".\UniRelease\multconv.sbr"
	-@erase ".\UniRelease\formatta.sbr"
	-@erase ".\UniRelease\options.sbr"
	-@erase ".\UniRelease\formatba.sbr"
	-@erase ".\UniRelease\listdlg.sbr"
	-@erase ".\UniRelease\unitspag.sbr"
	-@erase ".\UniRelease\stdafx.sbr"
	-@erase ".\UniRelease\splash.sbr"
	-@erase ".\UniRelease\ruler.sbr"
	-@erase ".\UniRelease\formatpa.sbr"
	-@erase ".\UniRelease\doctype.sbr"
	-@erase ".\UniRelease\chicdial.sbr"
	-@erase ".\UniRelease\buttondi.sbr"
	-@erase ".\UniRelease\srvritem.sbr"
	-@erase ".\UniRelease\docopt.sbr"
	-@erase ".\UniRelease\pageset.sbr"
	-@erase ".\UniRelease\mainfrm.sbr"
	-@erase ".\UniRelease\colorlis.sbr"
	-@erase ".\UniRelease\cntritem.sbr"
	-@erase ".\UniRelease\wordpvw.sbr"
	-@erase ".\UniRelease\datedial.sbr"
	-@erase ".\UniRelease\ddxm.sbr"
	-@erase ".\UniRelease\wordpdoc.sbr"
	-@erase ".\UniRelease\wordpad.sbr"
	-@erase ".\UniRelease\strings.sbr"
	-@erase ".\UniRelease\ipframe.sbr"
	-@erase ".\UniRelease\wordpad.exe"
	-@erase ".\UniRelease\wordpvw.obj"
	-@erase ".\UniRelease\datedial.obj"
	-@erase ".\UniRelease\ddxm.obj"
	-@erase ".\UniRelease\wordpdoc.obj"
	-@erase ".\UniRelease\wordpad.obj"
	-@erase ".\UniRelease\strings.obj"
	-@erase ".\UniRelease\ipframe.obj"
	-@erase ".\UniRelease\optionsh.obj"
	-@erase ".\UniRelease\key.obj"
	-@erase ".\UniRelease\filenewd.obj"
	-@erase ".\UniRelease\multconv.obj"
	-@erase ".\UniRelease\formatta.obj"
	-@erase ".\UniRelease\options.obj"
	-@erase ".\UniRelease\formatba.obj"
	-@erase ".\UniRelease\listdlg.obj"
	-@erase ".\UniRelease\unitspag.obj"
	-@erase ".\UniRelease\stdafx.obj"
	-@erase ".\UniRelease\splash.obj"
	-@erase ".\UniRelease\ruler.obj"
	-@erase ".\UniRelease\formatpa.obj"
	-@erase ".\UniRelease\doctype.obj"
	-@erase ".\UniRelease\chicdial.obj"
	-@erase ".\UniRelease\buttondi.obj"
	-@erase ".\UniRelease\srvritem.obj"
	-@erase ".\UniRelease\docopt.obj"
	-@erase ".\UniRelease\pageset.obj"
	-@erase ".\UniRelease\mainfrm.obj"
	-@erase ".\UniRelease\colorlis.obj"
	-@erase ".\UniRelease\cntritem.obj"
	-@erase ".\UniRelease\wordpad.res"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /Yu"stdafx.h" /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /D "_UNICODE" /FR /Yu"stdafx.h" /c
CPP_PROJ=/nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /D "_UNICODE" /FR"$(INTDIR)/" /Fp"$(INTDIR)/wordpad.pch"\
 /Yu"stdafx.h" /Fo"$(INTDIR)/" /Fd"$(OUTDIR)/wordpad.pdb" /c 
CPP_OBJS=.\UniRelease/
CPP_SBRS=.\UniRelease/
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/wordpad.res" /d "NDEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/wordpad.bsc" 
BSC32_SBRS= \
	"$(INTDIR)/optionsh.sbr" \
	"$(INTDIR)/key.sbr" \
	"$(INTDIR)/filenewd.sbr" \
	"$(INTDIR)/multconv.sbr" \
	"$(INTDIR)/formatta.sbr" \
	"$(INTDIR)/options.sbr" \
	"$(INTDIR)/formatba.sbr" \
	"$(INTDIR)/listdlg.sbr" \
	"$(INTDIR)/unitspag.sbr" \
	"$(INTDIR)/stdafx.sbr" \
	"$(INTDIR)/splash.sbr" \
	"$(INTDIR)/ruler.sbr" \
	"$(INTDIR)/formatpa.sbr" \
	"$(INTDIR)/doctype.sbr" \
	"$(INTDIR)/chicdial.sbr" \
	"$(INTDIR)/buttondi.sbr" \
	"$(INTDIR)/srvritem.sbr" \
	"$(INTDIR)/docopt.sbr" \
	"$(INTDIR)/pageset.sbr" \
	"$(INTDIR)/mainfrm.sbr" \
	"$(INTDIR)/colorlis.sbr" \
	"$(INTDIR)/cntritem.sbr" \
	"$(INTDIR)/wordpvw.sbr" \
	"$(INTDIR)/datedial.sbr" \
	"$(INTDIR)/ddxm.sbr" \
	"$(INTDIR)/wordpdoc.sbr" \
	"$(INTDIR)/wordpad.sbr" \
	"$(INTDIR)/strings.sbr" \
	"$(INTDIR)/ipframe.sbr"

"$(OUTDIR)\wordpad.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 /nologo /entry:"wWinMainCRTStartup" /subsystem:windows /machine:I386
LINK32_FLAGS=/nologo /entry:"wWinMainCRTStartup" /subsystem:windows\
 /incremental:no /pdb:"$(OUTDIR)/wordpad.pdb" /machine:I386\
 /out:"$(OUTDIR)/wordpad.exe" 
LINK32_OBJS= \
	"$(INTDIR)/wordpvw.obj" \
	"$(INTDIR)/datedial.obj" \
	"$(INTDIR)/ddxm.obj" \
	"$(INTDIR)/wordpdoc.obj" \
	"$(INTDIR)/wordpad.obj" \
	"$(INTDIR)/strings.obj" \
	"$(INTDIR)/ipframe.obj" \
	"$(INTDIR)/optionsh.obj" \
	"$(INTDIR)/key.obj" \
	"$(INTDIR)/filenewd.obj" \
	"$(INTDIR)/multconv.obj" \
	"$(INTDIR)/formatta.obj" \
	"$(INTDIR)/options.obj" \
	"$(INTDIR)/formatba.obj" \
	"$(INTDIR)/listdlg.obj" \
	"$(INTDIR)/unitspag.obj" \
	"$(INTDIR)/stdafx.obj" \
	"$(INTDIR)/splash.obj" \
	"$(INTDIR)/ruler.obj" \
	"$(INTDIR)/formatpa.obj" \
	"$(INTDIR)/doctype.obj" \
	"$(INTDIR)/chicdial.obj" \
	"$(INTDIR)/buttondi.obj" \
	"$(INTDIR)/srvritem.obj" \
	"$(INTDIR)/docopt.obj" \
	"$(INTDIR)/pageset.obj" \
	"$(INTDIR)/mainfrm.obj" \
	"$(INTDIR)/colorlis.obj" \
	"$(INTDIR)/cntritem.obj" \
	"$(INTDIR)/wordpad.res"

"$(OUTDIR)\wordpad.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "WordPad - Win32 Unicode Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir WordPad0
# PROP BASE Intermediate_Dir WordPad0
# PROP BASE Target_Dir
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir UniDebug
# PROP Intermediate_Dir UniDebug
# PROP Target_Dir
OUTDIR=.\UniDebug
INTDIR=.\UniDebug

ALL : "$(OUTDIR)\wordpad.exe" "$(OUTDIR)\wordpad.bsc"

CLEAN : 
	-@erase ".\UniDebug\wordpad.pdb"
	-@erase ".\UniDebug\wordpad.pch"
	-@erase ".\UniDebug\doctype.obj"
	-@erase ".\UniDebug\optionsh.obj"
	-@erase ".\UniDebug\filenewd.obj"
	-@erase ".\UniDebug\pageset.obj"
	-@erase ".\UniDebug\multconv.obj"
	-@erase ".\UniDebug\mainfrm.obj"
	-@erase ".\UniDebug\formatta.obj"
	-@erase ".\UniDebug\formatba.obj"
	-@erase ".\UniDebug\wordpvw.obj"
	-@erase ".\UniDebug\key.obj"
	-@erase ".\UniDebug\formatpa.obj"
	-@erase ".\UniDebug\chicdial.obj"
	-@erase ".\UniDebug\unitspag.obj"
	-@erase ".\UniDebug\buttondi.obj"
	-@erase ".\UniDebug\wordpad.obj"
	-@erase ".\UniDebug\strings.obj"
	-@erase ".\UniDebug\srvritem.obj"
	-@erase ".\UniDebug\ipframe.obj"
	-@erase ".\UniDebug\colorlis.obj"
	-@erase ".\UniDebug\cntritem.obj"
	-@erase ".\UniDebug\ruler.obj"
	-@erase ".\UniDebug\datedial.obj"
	-@erase ".\UniDebug\splash.obj"
	-@erase ".\UniDebug\options.obj"
	-@erase ".\UniDebug\ddxm.obj"
	-@erase ".\UniDebug\stdafx.obj"
	-@erase ".\UniDebug\listdlg.obj"
	-@erase ".\UniDebug\docopt.obj"
	-@erase ".\UniDebug\wordpdoc.obj"
	-@erase ".\UniDebug\wordpad.res"
	-@erase ".\UniDebug\wordpad.idb"
	-@erase ".\UniDebug\wordpad.bsc"
	-@erase ".\UniDebug\wordpvw.sbr"
	-@erase ".\UniDebug\key.sbr"
	-@erase ".\UniDebug\formatpa.sbr"
	-@erase ".\UniDebug\chicdial.sbr"
	-@erase ".\UniDebug\unitspag.sbr"
	-@erase ".\UniDebug\buttondi.sbr"
	-@erase ".\UniDebug\wordpad.sbr"
	-@erase ".\UniDebug\strings.sbr"
	-@erase ".\UniDebug\srvritem.sbr"
	-@erase ".\UniDebug\ipframe.sbr"
	-@erase ".\UniDebug\colorlis.sbr"
	-@erase ".\UniDebug\cntritem.sbr"
	-@erase ".\UniDebug\ruler.sbr"
	-@erase ".\UniDebug\datedial.sbr"
	-@erase ".\UniDebug\splash.sbr"
	-@erase ".\UniDebug\options.sbr"
	-@erase ".\UniDebug\ddxm.sbr"
	-@erase ".\UniDebug\stdafx.sbr"
	-@erase ".\UniDebug\listdlg.sbr"
	-@erase ".\UniDebug\docopt.sbr"
	-@erase ".\UniDebug\wordpdoc.sbr"
	-@erase ".\UniDebug\doctype.sbr"
	-@erase ".\UniDebug\optionsh.sbr"
	-@erase ".\UniDebug\filenewd.sbr"
	-@erase ".\UniDebug\pageset.sbr"
	-@erase ".\UniDebug\multconv.sbr"
	-@erase ".\UniDebug\mainfrm.sbr"
	-@erase ".\UniDebug\formatta.sbr"
	-@erase ".\UniDebug\formatba.sbr"
	-@erase ".\UniDebug\wordpad.exe"
	-@erase ".\UniDebug\wordpad.ilk"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /Yu"stdafx.h" /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /D "_UNICODE" /FR /Yu"stdafx.h" /c
CPP_PROJ=/nologo /MDd /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS"\
 /D "_AFXDLL" /D "_MBCS" /D "_UNICODE" /FR"$(INTDIR)/"\
 /Fp"$(INTDIR)/wordpad.pch" /Yu"stdafx.h" /Fo"$(INTDIR)/"\
 /Fd"$(OUTDIR)/wordpad.pdb" /c 
CPP_OBJS=.\UniDebug/
CPP_SBRS=.\UniDebug/
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/wordpad.res" /d "_DEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/wordpad.bsc" 
BSC32_SBRS= \
	"$(INTDIR)/wordpvw.sbr" \
	"$(INTDIR)/key.sbr" \
	"$(INTDIR)/formatpa.sbr" \
	"$(INTDIR)/chicdial.sbr" \
	"$(INTDIR)/unitspag.sbr" \
	"$(INTDIR)/buttondi.sbr" \
	"$(INTDIR)/wordpad.sbr" \
	"$(INTDIR)/strings.sbr" \
	"$(INTDIR)/srvritem.sbr" \
	"$(INTDIR)/ipframe.sbr" \
	"$(INTDIR)/colorlis.sbr" \
	"$(INTDIR)/cntritem.sbr" \
	"$(INTDIR)/ruler.sbr" \
	"$(INTDIR)/datedial.sbr" \
	"$(INTDIR)/splash.sbr" \
	"$(INTDIR)/options.sbr" \
	"$(INTDIR)/ddxm.sbr" \
	"$(INTDIR)/stdafx.sbr" \
	"$(INTDIR)/listdlg.sbr" \
	"$(INTDIR)/docopt.sbr" \
	"$(INTDIR)/wordpdoc.sbr" \
	"$(INTDIR)/doctype.sbr" \
	"$(INTDIR)/optionsh.sbr" \
	"$(INTDIR)/filenewd.sbr" \
	"$(INTDIR)/pageset.sbr" \
	"$(INTDIR)/multconv.sbr" \
	"$(INTDIR)/mainfrm.sbr" \
	"$(INTDIR)/formatta.sbr" \
	"$(INTDIR)/formatba.sbr"

"$(OUTDIR)\wordpad.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386
# ADD LINK32 /nologo /entry:"wWinMainCRTStartup" /subsystem:windows /debug /machine:I386
LINK32_FLAGS=/nologo /entry:"wWinMainCRTStartup" /subsystem:windows\
 /incremental:yes /pdb:"$(OUTDIR)/wordpad.pdb" /debug /machine:I386\
 /out:"$(OUTDIR)/wordpad.exe" 
LINK32_OBJS= \
	"$(INTDIR)/doctype.obj" \
	"$(INTDIR)/optionsh.obj" \
	"$(INTDIR)/filenewd.obj" \
	"$(INTDIR)/pageset.obj" \
	"$(INTDIR)/multconv.obj" \
	"$(INTDIR)/mainfrm.obj" \
	"$(INTDIR)/formatta.obj" \
	"$(INTDIR)/formatba.obj" \
	"$(INTDIR)/wordpvw.obj" \
	"$(INTDIR)/key.obj" \
	"$(INTDIR)/formatpa.obj" \
	"$(INTDIR)/chicdial.obj" \
	"$(INTDIR)/unitspag.obj" \
	"$(INTDIR)/buttondi.obj" \
	"$(INTDIR)/wordpad.obj" \
	"$(INTDIR)/strings.obj" \
	"$(INTDIR)/srvritem.obj" \
	"$(INTDIR)/ipframe.obj" \
	"$(INTDIR)/colorlis.obj" \
	"$(INTDIR)/cntritem.obj" \
	"$(INTDIR)/ruler.obj" \
	"$(INTDIR)/datedial.obj" \
	"$(INTDIR)/splash.obj" \
	"$(INTDIR)/options.obj" \
	"$(INTDIR)/ddxm.obj" \
	"$(INTDIR)/stdafx.obj" \
	"$(INTDIR)/listdlg.obj" \
	"$(INTDIR)/docopt.obj" \
	"$(INTDIR)/wordpdoc.obj" \
	"$(INTDIR)/wordpad.res"

"$(OUTDIR)\wordpad.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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

# Name "WordPad - Win32 Release"
# Name "WordPad - Win32 Debug"
# Name "WordPad - Win32 Unicode Release"
# Name "WordPad - Win32 Unicode Debug"

!IF  "$(CFG)" == "WordPad - Win32 Release"

!ELSEIF  "$(CFG)" == "WordPad - Win32 Debug"

!ELSEIF  "$(CFG)" == "WordPad - Win32 Unicode Release"

!ELSEIF  "$(CFG)" == "WordPad - Win32 Unicode Debug"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\buttondi.cpp
DEP_CPP_BUTTO=\
	".\buttondi.h"\
	".\stdafx.h"\
	".\chicdial.h"\
	".\doctype.h"\
	".\strings.h"\
	".\wordpad.h"\
	".\options.h"\
	".\splash.h"\
	

!IF  "$(CFG)" == "WordPad - Win32 Release"


"$(INTDIR)\buttondi.obj" : $(SOURCE) $(DEP_CPP_BUTTO) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"

"$(INTDIR)\buttondi.sbr" : $(SOURCE) $(DEP_CPP_BUTTO) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"

"$(INTDIR)\wordpad.idb" : $(SOURCE) $(DEP_CPP_BUTTO) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"


!ELSEIF  "$(CFG)" == "WordPad - Win32 Debug"


"$(INTDIR)\buttondi.obj" : $(SOURCE) $(DEP_CPP_BUTTO) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"

"$(INTDIR)\buttondi.sbr" : $(SOURCE) $(DEP_CPP_BUTTO) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"

"$(INTDIR)\wordpad.pdb" : $(SOURCE) $(DEP_CPP_BUTTO) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"

"$(INTDIR)\wordpad.idb" : $(SOURCE) $(DEP_CPP_BUTTO) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"


!ELSEIF  "$(CFG)" == "WordPad - Win32 Unicode Release"


"$(INTDIR)\buttondi.obj" : $(SOURCE) $(DEP_CPP_BUTTO) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"

"$(INTDIR)\buttondi.sbr" : $(SOURCE) $(DEP_CPP_BUTTO) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"

"$(INTDIR)\wordpad.idb" : $(SOURCE) $(DEP_CPP_BUTTO) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"


!ELSEIF  "$(CFG)" == "WordPad - Win32 Unicode Debug"


"$(INTDIR)\buttondi.obj" : $(SOURCE) $(DEP_CPP_BUTTO) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"

"$(INTDIR)\buttondi.sbr" : $(SOURCE) $(DEP_CPP_BUTTO) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"

"$(INTDIR)\wordpad.pdb" : $(SOURCE) $(DEP_CPP_BUTTO) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"

"$(INTDIR)\wordpad.idb" : $(SOURCE) $(DEP_CPP_BUTTO) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\chicdial.cpp
DEP_CPP_CHICD=\
	".\stdafx.h"\
	".\chicdial.h"\
	".\doctype.h"\
	

!IF  "$(CFG)" == "WordPad - Win32 Release"


"$(INTDIR)\chicdial.obj" : $(SOURCE) $(DEP_CPP_CHICD) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"

"$(INTDIR)\chicdial.sbr" : $(SOURCE) $(DEP_CPP_CHICD) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"

"$(INTDIR)\wordpad.idb" : $(SOURCE) $(DEP_CPP_CHICD) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"


!ELSEIF  "$(CFG)" == "WordPad - Win32 Debug"


"$(INTDIR)\chicdial.obj" : $(SOURCE) $(DEP_CPP_CHICD) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"

"$(INTDIR)\chicdial.sbr" : $(SOURCE) $(DEP_CPP_CHICD) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"

"$(INTDIR)\wordpad.pdb" : $(SOURCE) $(DEP_CPP_CHICD) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"

"$(INTDIR)\wordpad.idb" : $(SOURCE) $(DEP_CPP_CHICD) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"


!ELSEIF  "$(CFG)" == "WordPad - Win32 Unicode Release"


"$(INTDIR)\chicdial.obj" : $(SOURCE) $(DEP_CPP_CHICD) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"

"$(INTDIR)\chicdial.sbr" : $(SOURCE) $(DEP_CPP_CHICD) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"

"$(INTDIR)\wordpad.idb" : $(SOURCE) $(DEP_CPP_CHICD) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"


!ELSEIF  "$(CFG)" == "WordPad - Win32 Unicode Debug"


"$(INTDIR)\chicdial.obj" : $(SOURCE) $(DEP_CPP_CHICD) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"

"$(INTDIR)\chicdial.sbr" : $(SOURCE) $(DEP_CPP_CHICD) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"

"$(INTDIR)\wordpad.pdb" : $(SOURCE) $(DEP_CPP_CHICD) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"

"$(INTDIR)\wordpad.idb" : $(SOURCE) $(DEP_CPP_CHICD) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\cntritem.cpp
DEP_CPP_CNTRI=\
	".\cntritem.h"\
	".\stdafx.h"\
	".\chicdial.h"\
	".\doctype.h"\
	".\wordpad.h"\
	".\options.h"\
	".\splash.h"\
	".\wordpdoc.h"\
	".\wordpvw.h"\
	

!IF  "$(CFG)" == "WordPad - Win32 Release"


"$(INTDIR)\cntritem.obj" : $(SOURCE) $(DEP_CPP_CNTRI) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"

"$(INTDIR)\cntritem.sbr" : $(SOURCE) $(DEP_CPP_CNTRI) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"

"$(INTDIR)\wordpad.idb" : $(SOURCE) $(DEP_CPP_CNTRI) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"


!ELSEIF  "$(CFG)" == "WordPad - Win32 Debug"


"$(INTDIR)\cntritem.obj" : $(SOURCE) $(DEP_CPP_CNTRI) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"

"$(INTDIR)\cntritem.sbr" : $(SOURCE) $(DEP_CPP_CNTRI) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"

"$(INTDIR)\wordpad.pdb" : $(SOURCE) $(DEP_CPP_CNTRI) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"

"$(INTDIR)\wordpad.idb" : $(SOURCE) $(DEP_CPP_CNTRI) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"


!ELSEIF  "$(CFG)" == "WordPad - Win32 Unicode Release"


"$(INTDIR)\cntritem.obj" : $(SOURCE) $(DEP_CPP_CNTRI) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"

"$(INTDIR)\cntritem.sbr" : $(SOURCE) $(DEP_CPP_CNTRI) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"

"$(INTDIR)\wordpad.idb" : $(SOURCE) $(DEP_CPP_CNTRI) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"


!ELSEIF  "$(CFG)" == "WordPad - Win32 Unicode Debug"


"$(INTDIR)\cntritem.obj" : $(SOURCE) $(DEP_CPP_CNTRI) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"

"$(INTDIR)\cntritem.sbr" : $(SOURCE) $(DEP_CPP_CNTRI) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"

"$(INTDIR)\wordpad.pdb" : $(SOURCE) $(DEP_CPP_CNTRI) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"

"$(INTDIR)\wordpad.idb" : $(SOURCE) $(DEP_CPP_CNTRI) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\colorlis.cpp
DEP_CPP_COLOR=\
	".\colorlis.h"\
	".\stdafx.h"\
	".\chicdial.h"\
	".\doctype.h"\
	".\wordpad.h"\
	".\options.h"\
	".\splash.h"\
	

!IF  "$(CFG)" == "WordPad - Win32 Release"


"$(INTDIR)\colorlis.obj" : $(SOURCE) $(DEP_CPP_COLOR) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"

"$(INTDIR)\colorlis.sbr" : $(SOURCE) $(DEP_CPP_COLOR) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"

"$(INTDIR)\wordpad.idb" : $(SOURCE) $(DEP_CPP_COLOR) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"


!ELSEIF  "$(CFG)" == "WordPad - Win32 Debug"


"$(INTDIR)\colorlis.obj" : $(SOURCE) $(DEP_CPP_COLOR) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"

"$(INTDIR)\colorlis.sbr" : $(SOURCE) $(DEP_CPP_COLOR) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"

"$(INTDIR)\wordpad.pdb" : $(SOURCE) $(DEP_CPP_COLOR) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"

"$(INTDIR)\wordpad.idb" : $(SOURCE) $(DEP_CPP_COLOR) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"


!ELSEIF  "$(CFG)" == "WordPad - Win32 Unicode Release"


"$(INTDIR)\colorlis.obj" : $(SOURCE) $(DEP_CPP_COLOR) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"

"$(INTDIR)\colorlis.sbr" : $(SOURCE) $(DEP_CPP_COLOR) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"

"$(INTDIR)\wordpad.idb" : $(SOURCE) $(DEP_CPP_COLOR) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"


!ELSEIF  "$(CFG)" == "WordPad - Win32 Unicode Debug"


"$(INTDIR)\colorlis.obj" : $(SOURCE) $(DEP_CPP_COLOR) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"

"$(INTDIR)\colorlis.sbr" : $(SOURCE) $(DEP_CPP_COLOR) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"

"$(INTDIR)\wordpad.pdb" : $(SOURCE) $(DEP_CPP_COLOR) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"

"$(INTDIR)\wordpad.idb" : $(SOURCE) $(DEP_CPP_COLOR) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\datedial.cpp
DEP_CPP_DATED=\
	".\datedial.h"\
	".\helpids.h"\
	".\stdafx.h"\
	".\chicdial.h"\
	".\doctype.h"\
	".\wordpad.h"\
	".\options.h"\
	".\splash.h"\
	

!IF  "$(CFG)" == "WordPad - Win32 Release"


"$(INTDIR)\datedial.obj" : $(SOURCE) $(DEP_CPP_DATED) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"

"$(INTDIR)\datedial.sbr" : $(SOURCE) $(DEP_CPP_DATED) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"

"$(INTDIR)\wordpad.idb" : $(SOURCE) $(DEP_CPP_DATED) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"


!ELSEIF  "$(CFG)" == "WordPad - Win32 Debug"


"$(INTDIR)\datedial.obj" : $(SOURCE) $(DEP_CPP_DATED) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"

"$(INTDIR)\datedial.sbr" : $(SOURCE) $(DEP_CPP_DATED) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"

"$(INTDIR)\wordpad.pdb" : $(SOURCE) $(DEP_CPP_DATED) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"

"$(INTDIR)\wordpad.idb" : $(SOURCE) $(DEP_CPP_DATED) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"


!ELSEIF  "$(CFG)" == "WordPad - Win32 Unicode Release"


"$(INTDIR)\datedial.obj" : $(SOURCE) $(DEP_CPP_DATED) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"

"$(INTDIR)\datedial.sbr" : $(SOURCE) $(DEP_CPP_DATED) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"

"$(INTDIR)\wordpad.idb" : $(SOURCE) $(DEP_CPP_DATED) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"


!ELSEIF  "$(CFG)" == "WordPad - Win32 Unicode Debug"


"$(INTDIR)\datedial.obj" : $(SOURCE) $(DEP_CPP_DATED) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"

"$(INTDIR)\datedial.sbr" : $(SOURCE) $(DEP_CPP_DATED) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"

"$(INTDIR)\wordpad.pdb" : $(SOURCE) $(DEP_CPP_DATED) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"

"$(INTDIR)\wordpad.idb" : $(SOURCE) $(DEP_CPP_DATED) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\ddxm.cpp
DEP_CPP_DDXM_=\
	".\ddxm.h"\
	".\stdafx.h"\
	".\chicdial.h"\
	".\doctype.h"\
	".\wordpad.h"\
	".\options.h"\
	".\splash.h"\
	

!IF  "$(CFG)" == "WordPad - Win32 Release"


"$(INTDIR)\ddxm.obj" : $(SOURCE) $(DEP_CPP_DDXM_) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"

"$(INTDIR)\ddxm.sbr" : $(SOURCE) $(DEP_CPP_DDXM_) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"

"$(INTDIR)\wordpad.idb" : $(SOURCE) $(DEP_CPP_DDXM_) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"


!ELSEIF  "$(CFG)" == "WordPad - Win32 Debug"


"$(INTDIR)\ddxm.obj" : $(SOURCE) $(DEP_CPP_DDXM_) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"

"$(INTDIR)\ddxm.sbr" : $(SOURCE) $(DEP_CPP_DDXM_) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"

"$(INTDIR)\wordpad.pdb" : $(SOURCE) $(DEP_CPP_DDXM_) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"

"$(INTDIR)\wordpad.idb" : $(SOURCE) $(DEP_CPP_DDXM_) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"


!ELSEIF  "$(CFG)" == "WordPad - Win32 Unicode Release"


"$(INTDIR)\ddxm.obj" : $(SOURCE) $(DEP_CPP_DDXM_) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"

"$(INTDIR)\ddxm.sbr" : $(SOURCE) $(DEP_CPP_DDXM_) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"

"$(INTDIR)\wordpad.idb" : $(SOURCE) $(DEP_CPP_DDXM_) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"


!ELSEIF  "$(CFG)" == "WordPad - Win32 Unicode Debug"


"$(INTDIR)\ddxm.obj" : $(SOURCE) $(DEP_CPP_DDXM_) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"

"$(INTDIR)\ddxm.sbr" : $(SOURCE) $(DEP_CPP_DDXM_) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"

"$(INTDIR)\wordpad.pdb" : $(SOURCE) $(DEP_CPP_DDXM_) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"

"$(INTDIR)\wordpad.idb" : $(SOURCE) $(DEP_CPP_DDXM_) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\docopt.cpp
DEP_CPP_DOCOP=\
	".\docopt.h"\
	".\helpids.h"\
	".\stdafx.h"\
	".\chicdial.h"\
	".\doctype.h"\
	".\wordpad.h"\
	".\options.h"\
	".\splash.h"\
	

!IF  "$(CFG)" == "WordPad - Win32 Release"


"$(INTDIR)\docopt.obj" : $(SOURCE) $(DEP_CPP_DOCOP) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"

"$(INTDIR)\docopt.sbr" : $(SOURCE) $(DEP_CPP_DOCOP) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"

"$(INTDIR)\wordpad.idb" : $(SOURCE) $(DEP_CPP_DOCOP) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"


!ELSEIF  "$(CFG)" == "WordPad - Win32 Debug"


"$(INTDIR)\docopt.obj" : $(SOURCE) $(DEP_CPP_DOCOP) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"

"$(INTDIR)\docopt.sbr" : $(SOURCE) $(DEP_CPP_DOCOP) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"

"$(INTDIR)\wordpad.pdb" : $(SOURCE) $(DEP_CPP_DOCOP) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"

"$(INTDIR)\wordpad.idb" : $(SOURCE) $(DEP_CPP_DOCOP) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"


!ELSEIF  "$(CFG)" == "WordPad - Win32 Unicode Release"


"$(INTDIR)\docopt.obj" : $(SOURCE) $(DEP_CPP_DOCOP) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"

"$(INTDIR)\docopt.sbr" : $(SOURCE) $(DEP_CPP_DOCOP) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"

"$(INTDIR)\wordpad.idb" : $(SOURCE) $(DEP_CPP_DOCOP) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"


!ELSEIF  "$(CFG)" == "WordPad - Win32 Unicode Debug"


"$(INTDIR)\docopt.obj" : $(SOURCE) $(DEP_CPP_DOCOP) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"

"$(INTDIR)\docopt.sbr" : $(SOURCE) $(DEP_CPP_DOCOP) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"

"$(INTDIR)\wordpad.pdb" : $(SOURCE) $(DEP_CPP_DOCOP) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"

"$(INTDIR)\wordpad.idb" : $(SOURCE) $(DEP_CPP_DOCOP) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\doctype.cpp
DEP_CPP_DOCTY=\
	".\multconv.h"\
	".\stdafx.h"\
	".\chicdial.h"\
	".\doctype.h"\
	".\strings.h"\
	

!IF  "$(CFG)" == "WordPad - Win32 Release"


"$(INTDIR)\doctype.obj" : $(SOURCE) $(DEP_CPP_DOCTY) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"

"$(INTDIR)\doctype.sbr" : $(SOURCE) $(DEP_CPP_DOCTY) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"

"$(INTDIR)\wordpad.idb" : $(SOURCE) $(DEP_CPP_DOCTY) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"


!ELSEIF  "$(CFG)" == "WordPad - Win32 Debug"


"$(INTDIR)\doctype.obj" : $(SOURCE) $(DEP_CPP_DOCTY) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"

"$(INTDIR)\doctype.sbr" : $(SOURCE) $(DEP_CPP_DOCTY) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"

"$(INTDIR)\wordpad.pdb" : $(SOURCE) $(DEP_CPP_DOCTY) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"

"$(INTDIR)\wordpad.idb" : $(SOURCE) $(DEP_CPP_DOCTY) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"


!ELSEIF  "$(CFG)" == "WordPad - Win32 Unicode Release"


"$(INTDIR)\doctype.obj" : $(SOURCE) $(DEP_CPP_DOCTY) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"

"$(INTDIR)\doctype.sbr" : $(SOURCE) $(DEP_CPP_DOCTY) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"

"$(INTDIR)\wordpad.idb" : $(SOURCE) $(DEP_CPP_DOCTY) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"


!ELSEIF  "$(CFG)" == "WordPad - Win32 Unicode Debug"


"$(INTDIR)\doctype.obj" : $(SOURCE) $(DEP_CPP_DOCTY) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"

"$(INTDIR)\doctype.sbr" : $(SOURCE) $(DEP_CPP_DOCTY) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"

"$(INTDIR)\wordpad.pdb" : $(SOURCE) $(DEP_CPP_DOCTY) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"

"$(INTDIR)\wordpad.idb" : $(SOURCE) $(DEP_CPP_DOCTY) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\filenewd.cpp
DEP_CPP_FILEN=\
	".\filenewd.h"\
	".\helpids.h"\
	".\stdafx.h"\
	".\chicdial.h"\
	".\doctype.h"\
	".\wordpad.h"\
	".\options.h"\
	".\splash.h"\
	

!IF  "$(CFG)" == "WordPad - Win32 Release"


"$(INTDIR)\filenewd.obj" : $(SOURCE) $(DEP_CPP_FILEN) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"

"$(INTDIR)\filenewd.sbr" : $(SOURCE) $(DEP_CPP_FILEN) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"

"$(INTDIR)\wordpad.idb" : $(SOURCE) $(DEP_CPP_FILEN) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"


!ELSEIF  "$(CFG)" == "WordPad - Win32 Debug"


"$(INTDIR)\filenewd.obj" : $(SOURCE) $(DEP_CPP_FILEN) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"

"$(INTDIR)\filenewd.sbr" : $(SOURCE) $(DEP_CPP_FILEN) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"

"$(INTDIR)\wordpad.pdb" : $(SOURCE) $(DEP_CPP_FILEN) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"

"$(INTDIR)\wordpad.idb" : $(SOURCE) $(DEP_CPP_FILEN) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"


!ELSEIF  "$(CFG)" == "WordPad - Win32 Unicode Release"


"$(INTDIR)\filenewd.obj" : $(SOURCE) $(DEP_CPP_FILEN) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"

"$(INTDIR)\filenewd.sbr" : $(SOURCE) $(DEP_CPP_FILEN) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"

"$(INTDIR)\wordpad.idb" : $(SOURCE) $(DEP_CPP_FILEN) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"


!ELSEIF  "$(CFG)" == "WordPad - Win32 Unicode Debug"


"$(INTDIR)\filenewd.obj" : $(SOURCE) $(DEP_CPP_FILEN) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"

"$(INTDIR)\filenewd.sbr" : $(SOURCE) $(DEP_CPP_FILEN) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"

"$(INTDIR)\wordpad.pdb" : $(SOURCE) $(DEP_CPP_FILEN) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"

"$(INTDIR)\wordpad.idb" : $(SOURCE) $(DEP_CPP_FILEN) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\formatba.cpp
DEP_CPP_FORMA=\
	".\formatba.h"\
	".\stdafx.h"\
	".\chicdial.h"\
	".\doctype.h"\
	".\strings.h"\
	".\wordpad.h"\
	".\options.h"\
	".\splash.h"\
	".\wordpdoc.h"\
	".\wordpvw.h"\
	

!IF  "$(CFG)" == "WordPad - Win32 Release"


"$(INTDIR)\formatba.obj" : $(SOURCE) $(DEP_CPP_FORMA) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"

"$(INTDIR)\formatba.sbr" : $(SOURCE) $(DEP_CPP_FORMA) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"

"$(INTDIR)\wordpad.idb" : $(SOURCE) $(DEP_CPP_FORMA) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"


!ELSEIF  "$(CFG)" == "WordPad - Win32 Debug"


"$(INTDIR)\formatba.obj" : $(SOURCE) $(DEP_CPP_FORMA) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"

"$(INTDIR)\formatba.sbr" : $(SOURCE) $(DEP_CPP_FORMA) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"

"$(INTDIR)\wordpad.pdb" : $(SOURCE) $(DEP_CPP_FORMA) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"

"$(INTDIR)\wordpad.idb" : $(SOURCE) $(DEP_CPP_FORMA) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"


!ELSEIF  "$(CFG)" == "WordPad - Win32 Unicode Release"


"$(INTDIR)\formatba.obj" : $(SOURCE) $(DEP_CPP_FORMA) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"

"$(INTDIR)\formatba.sbr" : $(SOURCE) $(DEP_CPP_FORMA) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"

"$(INTDIR)\wordpad.idb" : $(SOURCE) $(DEP_CPP_FORMA) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"


!ELSEIF  "$(CFG)" == "WordPad - Win32 Unicode Debug"


"$(INTDIR)\formatba.obj" : $(SOURCE) $(DEP_CPP_FORMA) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"

"$(INTDIR)\formatba.sbr" : $(SOURCE) $(DEP_CPP_FORMA) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"

"$(INTDIR)\wordpad.pdb" : $(SOURCE) $(DEP_CPP_FORMA) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"

"$(INTDIR)\wordpad.idb" : $(SOURCE) $(DEP_CPP_FORMA) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\formatpa.cpp
DEP_CPP_FORMAT=\
	".\ddxm.h"\
	".\formatpa.h"\
	".\helpids.h"\
	".\stdafx.h"\
	".\chicdial.h"\
	".\doctype.h"\
	".\wordpad.h"\
	".\options.h"\
	".\splash.h"\
	

!IF  "$(CFG)" == "WordPad - Win32 Release"


"$(INTDIR)\formatpa.obj" : $(SOURCE) $(DEP_CPP_FORMAT) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"

"$(INTDIR)\formatpa.sbr" : $(SOURCE) $(DEP_CPP_FORMAT) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"

"$(INTDIR)\wordpad.idb" : $(SOURCE) $(DEP_CPP_FORMAT) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"


!ELSEIF  "$(CFG)" == "WordPad - Win32 Debug"


"$(INTDIR)\formatpa.obj" : $(SOURCE) $(DEP_CPP_FORMAT) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"

"$(INTDIR)\formatpa.sbr" : $(SOURCE) $(DEP_CPP_FORMAT) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"

"$(INTDIR)\wordpad.pdb" : $(SOURCE) $(DEP_CPP_FORMAT) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"

"$(INTDIR)\wordpad.idb" : $(SOURCE) $(DEP_CPP_FORMAT) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"


!ELSEIF  "$(CFG)" == "WordPad - Win32 Unicode Release"


"$(INTDIR)\formatpa.obj" : $(SOURCE) $(DEP_CPP_FORMAT) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"

"$(INTDIR)\formatpa.sbr" : $(SOURCE) $(DEP_CPP_FORMAT) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"

"$(INTDIR)\wordpad.idb" : $(SOURCE) $(DEP_CPP_FORMAT) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"


!ELSEIF  "$(CFG)" == "WordPad - Win32 Unicode Debug"


"$(INTDIR)\formatpa.obj" : $(SOURCE) $(DEP_CPP_FORMAT) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"

"$(INTDIR)\formatpa.sbr" : $(SOURCE) $(DEP_CPP_FORMAT) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"

"$(INTDIR)\wordpad.pdb" : $(SOURCE) $(DEP_CPP_FORMAT) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"

"$(INTDIR)\wordpad.idb" : $(SOURCE) $(DEP_CPP_FORMAT) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\formatta.cpp
DEP_CPP_FORMATT=\
	".\ddxm.h"\
	".\formatta.h"\
	".\helpids.h"\
	".\stdafx.h"\
	".\chicdial.h"\
	".\doctype.h"\
	".\wordpad.h"\
	".\options.h"\
	".\splash.h"\
	

!IF  "$(CFG)" == "WordPad - Win32 Release"


"$(INTDIR)\formatta.obj" : $(SOURCE) $(DEP_CPP_FORMATT) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"

"$(INTDIR)\formatta.sbr" : $(SOURCE) $(DEP_CPP_FORMATT) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"

"$(INTDIR)\wordpad.idb" : $(SOURCE) $(DEP_CPP_FORMATT) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"


!ELSEIF  "$(CFG)" == "WordPad - Win32 Debug"


"$(INTDIR)\formatta.obj" : $(SOURCE) $(DEP_CPP_FORMATT) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"

"$(INTDIR)\formatta.sbr" : $(SOURCE) $(DEP_CPP_FORMATT) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"

"$(INTDIR)\wordpad.pdb" : $(SOURCE) $(DEP_CPP_FORMATT) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"

"$(INTDIR)\wordpad.idb" : $(SOURCE) $(DEP_CPP_FORMATT) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"


!ELSEIF  "$(CFG)" == "WordPad - Win32 Unicode Release"


"$(INTDIR)\formatta.obj" : $(SOURCE) $(DEP_CPP_FORMATT) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"

"$(INTDIR)\formatta.sbr" : $(SOURCE) $(DEP_CPP_FORMATT) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"

"$(INTDIR)\wordpad.idb" : $(SOURCE) $(DEP_CPP_FORMATT) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"


!ELSEIF  "$(CFG)" == "WordPad - Win32 Unicode Debug"


"$(INTDIR)\formatta.obj" : $(SOURCE) $(DEP_CPP_FORMATT) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"

"$(INTDIR)\formatta.sbr" : $(SOURCE) $(DEP_CPP_FORMATT) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"

"$(INTDIR)\wordpad.pdb" : $(SOURCE) $(DEP_CPP_FORMATT) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"

"$(INTDIR)\wordpad.idb" : $(SOURCE) $(DEP_CPP_FORMATT) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\ipframe.cpp
DEP_CPP_IPFRA=\
	".\colorlis.h"\
	".\formatba.h"\
	".\ipframe.h"\
	".\ruler.h"\
	".\stdafx.h"\
	".\chicdial.h"\
	".\doctype.h"\
	".\wordpad.h"\
	".\options.h"\
	".\splash.h"\
	".\wordpdoc.h"\
	".\wordpvw.h"\
	

!IF  "$(CFG)" == "WordPad - Win32 Release"


"$(INTDIR)\ipframe.obj" : $(SOURCE) $(DEP_CPP_IPFRA) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"

"$(INTDIR)\ipframe.sbr" : $(SOURCE) $(DEP_CPP_IPFRA) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"

"$(INTDIR)\wordpad.idb" : $(SOURCE) $(DEP_CPP_IPFRA) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"


!ELSEIF  "$(CFG)" == "WordPad - Win32 Debug"


"$(INTDIR)\ipframe.obj" : $(SOURCE) $(DEP_CPP_IPFRA) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"

"$(INTDIR)\ipframe.sbr" : $(SOURCE) $(DEP_CPP_IPFRA) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"

"$(INTDIR)\wordpad.pdb" : $(SOURCE) $(DEP_CPP_IPFRA) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"

"$(INTDIR)\wordpad.idb" : $(SOURCE) $(DEP_CPP_IPFRA) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"


!ELSEIF  "$(CFG)" == "WordPad - Win32 Unicode Release"


"$(INTDIR)\ipframe.obj" : $(SOURCE) $(DEP_CPP_IPFRA) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"

"$(INTDIR)\ipframe.sbr" : $(SOURCE) $(DEP_CPP_IPFRA) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"

"$(INTDIR)\wordpad.idb" : $(SOURCE) $(DEP_CPP_IPFRA) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"


!ELSEIF  "$(CFG)" == "WordPad - Win32 Unicode Debug"


"$(INTDIR)\ipframe.obj" : $(SOURCE) $(DEP_CPP_IPFRA) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"

"$(INTDIR)\ipframe.sbr" : $(SOURCE) $(DEP_CPP_IPFRA) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"

"$(INTDIR)\wordpad.pdb" : $(SOURCE) $(DEP_CPP_IPFRA) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"

"$(INTDIR)\wordpad.idb" : $(SOURCE) $(DEP_CPP_IPFRA) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\key.cpp
DEP_CPP_KEY_C=\
	".\key.h"\
	".\stdafx.h"\
	".\chicdial.h"\
	".\doctype.h"\
	

!IF  "$(CFG)" == "WordPad - Win32 Release"


"$(INTDIR)\key.obj" : $(SOURCE) $(DEP_CPP_KEY_C) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"

"$(INTDIR)\key.sbr" : $(SOURCE) $(DEP_CPP_KEY_C) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"

"$(INTDIR)\wordpad.idb" : $(SOURCE) $(DEP_CPP_KEY_C) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"


!ELSEIF  "$(CFG)" == "WordPad - Win32 Debug"


"$(INTDIR)\key.obj" : $(SOURCE) $(DEP_CPP_KEY_C) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"

"$(INTDIR)\key.sbr" : $(SOURCE) $(DEP_CPP_KEY_C) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"

"$(INTDIR)\wordpad.pdb" : $(SOURCE) $(DEP_CPP_KEY_C) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"

"$(INTDIR)\wordpad.idb" : $(SOURCE) $(DEP_CPP_KEY_C) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"


!ELSEIF  "$(CFG)" == "WordPad - Win32 Unicode Release"


"$(INTDIR)\key.obj" : $(SOURCE) $(DEP_CPP_KEY_C) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"

"$(INTDIR)\key.sbr" : $(SOURCE) $(DEP_CPP_KEY_C) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"

"$(INTDIR)\wordpad.idb" : $(SOURCE) $(DEP_CPP_KEY_C) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"


!ELSEIF  "$(CFG)" == "WordPad - Win32 Unicode Debug"


"$(INTDIR)\key.obj" : $(SOURCE) $(DEP_CPP_KEY_C) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"

"$(INTDIR)\key.sbr" : $(SOURCE) $(DEP_CPP_KEY_C) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"

"$(INTDIR)\wordpad.pdb" : $(SOURCE) $(DEP_CPP_KEY_C) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"

"$(INTDIR)\wordpad.idb" : $(SOURCE) $(DEP_CPP_KEY_C) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\listdlg.cpp
DEP_CPP_LISTD=\
	".\listdlg.h"\
	".\stdafx.h"\
	".\chicdial.h"\
	".\doctype.h"\
	

!IF  "$(CFG)" == "WordPad - Win32 Release"


"$(INTDIR)\listdlg.obj" : $(SOURCE) $(DEP_CPP_LISTD) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"

"$(INTDIR)\listdlg.sbr" : $(SOURCE) $(DEP_CPP_LISTD) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"

"$(INTDIR)\wordpad.idb" : $(SOURCE) $(DEP_CPP_LISTD) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"


!ELSEIF  "$(CFG)" == "WordPad - Win32 Debug"


"$(INTDIR)\listdlg.obj" : $(SOURCE) $(DEP_CPP_LISTD) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"

"$(INTDIR)\listdlg.sbr" : $(SOURCE) $(DEP_CPP_LISTD) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"

"$(INTDIR)\wordpad.pdb" : $(SOURCE) $(DEP_CPP_LISTD) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"

"$(INTDIR)\wordpad.idb" : $(SOURCE) $(DEP_CPP_LISTD) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"


!ELSEIF  "$(CFG)" == "WordPad - Win32 Unicode Release"


"$(INTDIR)\listdlg.obj" : $(SOURCE) $(DEP_CPP_LISTD) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"

"$(INTDIR)\listdlg.sbr" : $(SOURCE) $(DEP_CPP_LISTD) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"

"$(INTDIR)\wordpad.idb" : $(SOURCE) $(DEP_CPP_LISTD) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"


!ELSEIF  "$(CFG)" == "WordPad - Win32 Unicode Debug"


"$(INTDIR)\listdlg.obj" : $(SOURCE) $(DEP_CPP_LISTD) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"

"$(INTDIR)\listdlg.sbr" : $(SOURCE) $(DEP_CPP_LISTD) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"

"$(INTDIR)\wordpad.pdb" : $(SOURCE) $(DEP_CPP_LISTD) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"

"$(INTDIR)\wordpad.idb" : $(SOURCE) $(DEP_CPP_LISTD) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\mainfrm.cpp
DEP_CPP_MAINF=\
	".\colorlis.h"\
	".\mainfrm.h"\
	".\formatba.h"\
	".\ruler.h"\
	".\stdafx.h"\
	".\chicdial.h"\
	".\doctype.h"\
	".\strings.h"\
	".\wordpad.h"\
	".\options.h"\
	".\splash.h"\
	".\wordpdoc.h"\
	".\wordpvw.h"\
	

!IF  "$(CFG)" == "WordPad - Win32 Release"


"$(INTDIR)\mainfrm.obj" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"

"$(INTDIR)\mainfrm.sbr" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"

"$(INTDIR)\wordpad.idb" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"


!ELSEIF  "$(CFG)" == "WordPad - Win32 Debug"


"$(INTDIR)\mainfrm.obj" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"

"$(INTDIR)\mainfrm.sbr" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"

"$(INTDIR)\wordpad.pdb" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"

"$(INTDIR)\wordpad.idb" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"


!ELSEIF  "$(CFG)" == "WordPad - Win32 Unicode Release"


"$(INTDIR)\mainfrm.obj" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"

"$(INTDIR)\mainfrm.sbr" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"

"$(INTDIR)\wordpad.idb" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"


!ELSEIF  "$(CFG)" == "WordPad - Win32 Unicode Debug"


"$(INTDIR)\mainfrm.obj" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"

"$(INTDIR)\mainfrm.sbr" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"

"$(INTDIR)\wordpad.pdb" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"

"$(INTDIR)\wordpad.idb" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\multconv.cpp
DEP_CPP_MULTC=\
	".\mswd6_32.h"\
	".\multconv.h"\
	".\stdafx.h"\
	".\chicdial.h"\
	".\doctype.h"\
	".\wordpad.h"\
	".\options.h"\
	".\splash.h"\
	

!IF  "$(CFG)" == "WordPad - Win32 Release"


"$(INTDIR)\multconv.obj" : $(SOURCE) $(DEP_CPP_MULTC) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"

"$(INTDIR)\multconv.sbr" : $(SOURCE) $(DEP_CPP_MULTC) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"

"$(INTDIR)\wordpad.idb" : $(SOURCE) $(DEP_CPP_MULTC) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"


!ELSEIF  "$(CFG)" == "WordPad - Win32 Debug"


"$(INTDIR)\multconv.obj" : $(SOURCE) $(DEP_CPP_MULTC) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"

"$(INTDIR)\multconv.sbr" : $(SOURCE) $(DEP_CPP_MULTC) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"

"$(INTDIR)\wordpad.pdb" : $(SOURCE) $(DEP_CPP_MULTC) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"

"$(INTDIR)\wordpad.idb" : $(SOURCE) $(DEP_CPP_MULTC) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"


!ELSEIF  "$(CFG)" == "WordPad - Win32 Unicode Release"


"$(INTDIR)\multconv.obj" : $(SOURCE) $(DEP_CPP_MULTC) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"

"$(INTDIR)\multconv.sbr" : $(SOURCE) $(DEP_CPP_MULTC) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"

"$(INTDIR)\wordpad.idb" : $(SOURCE) $(DEP_CPP_MULTC) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"


!ELSEIF  "$(CFG)" == "WordPad - Win32 Unicode Debug"


"$(INTDIR)\multconv.obj" : $(SOURCE) $(DEP_CPP_MULTC) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"

"$(INTDIR)\multconv.sbr" : $(SOURCE) $(DEP_CPP_MULTC) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"

"$(INTDIR)\wordpad.pdb" : $(SOURCE) $(DEP_CPP_MULTC) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"

"$(INTDIR)\wordpad.idb" : $(SOURCE) $(DEP_CPP_MULTC) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\options.cpp
DEP_CPP_OPTIO=\
	".\stdafx.h"\
	".\chicdial.h"\
	".\doctype.h"\
	".\strings.h"\
	".\wordpad.h"\
	".\options.h"\
	".\splash.h"\
	

!IF  "$(CFG)" == "WordPad - Win32 Release"


"$(INTDIR)\options.obj" : $(SOURCE) $(DEP_CPP_OPTIO) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"

"$(INTDIR)\options.sbr" : $(SOURCE) $(DEP_CPP_OPTIO) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"

"$(INTDIR)\wordpad.idb" : $(SOURCE) $(DEP_CPP_OPTIO) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"


!ELSEIF  "$(CFG)" == "WordPad - Win32 Debug"


"$(INTDIR)\options.obj" : $(SOURCE) $(DEP_CPP_OPTIO) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"

"$(INTDIR)\options.sbr" : $(SOURCE) $(DEP_CPP_OPTIO) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"

"$(INTDIR)\wordpad.pdb" : $(SOURCE) $(DEP_CPP_OPTIO) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"

"$(INTDIR)\wordpad.idb" : $(SOURCE) $(DEP_CPP_OPTIO) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"


!ELSEIF  "$(CFG)" == "WordPad - Win32 Unicode Release"


"$(INTDIR)\options.obj" : $(SOURCE) $(DEP_CPP_OPTIO) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"

"$(INTDIR)\options.sbr" : $(SOURCE) $(DEP_CPP_OPTIO) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"

"$(INTDIR)\wordpad.idb" : $(SOURCE) $(DEP_CPP_OPTIO) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"


!ELSEIF  "$(CFG)" == "WordPad - Win32 Unicode Debug"


"$(INTDIR)\options.obj" : $(SOURCE) $(DEP_CPP_OPTIO) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"

"$(INTDIR)\options.sbr" : $(SOURCE) $(DEP_CPP_OPTIO) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"

"$(INTDIR)\wordpad.pdb" : $(SOURCE) $(DEP_CPP_OPTIO) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"

"$(INTDIR)\wordpad.idb" : $(SOURCE) $(DEP_CPP_OPTIO) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\optionsh.cpp
DEP_CPP_OPTION=\
	".\docopt.h"\
	".\optionsh.h"\
	".\stdafx.h"\
	".\chicdial.h"\
	".\doctype.h"\
	".\unitspag.h"\
	".\wordpad.h"\
	".\options.h"\
	".\splash.h"\
	

!IF  "$(CFG)" == "WordPad - Win32 Release"


"$(INTDIR)\optionsh.obj" : $(SOURCE) $(DEP_CPP_OPTION) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"

"$(INTDIR)\optionsh.sbr" : $(SOURCE) $(DEP_CPP_OPTION) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"

"$(INTDIR)\wordpad.idb" : $(SOURCE) $(DEP_CPP_OPTION) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"


!ELSEIF  "$(CFG)" == "WordPad - Win32 Debug"


"$(INTDIR)\optionsh.obj" : $(SOURCE) $(DEP_CPP_OPTION) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"

"$(INTDIR)\optionsh.sbr" : $(SOURCE) $(DEP_CPP_OPTION) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"

"$(INTDIR)\wordpad.pdb" : $(SOURCE) $(DEP_CPP_OPTION) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"

"$(INTDIR)\wordpad.idb" : $(SOURCE) $(DEP_CPP_OPTION) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"


!ELSEIF  "$(CFG)" == "WordPad - Win32 Unicode Release"


"$(INTDIR)\optionsh.obj" : $(SOURCE) $(DEP_CPP_OPTION) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"

"$(INTDIR)\optionsh.sbr" : $(SOURCE) $(DEP_CPP_OPTION) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"

"$(INTDIR)\wordpad.idb" : $(SOURCE) $(DEP_CPP_OPTION) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"


!ELSEIF  "$(CFG)" == "WordPad - Win32 Unicode Debug"


"$(INTDIR)\optionsh.obj" : $(SOURCE) $(DEP_CPP_OPTION) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"

"$(INTDIR)\optionsh.sbr" : $(SOURCE) $(DEP_CPP_OPTION) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"

"$(INTDIR)\wordpad.pdb" : $(SOURCE) $(DEP_CPP_OPTION) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"

"$(INTDIR)\wordpad.idb" : $(SOURCE) $(DEP_CPP_OPTION) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\pageset.cpp
DEP_CPP_PAGES=\
	".\ddxm.h"\
	".\helpids.h"\
	".\pageset.h"\
	".\ruler.h"\
	".\stdafx.h"\
	".\chicdial.h"\
	".\doctype.h"\
	".\wordpad.h"\
	".\options.h"\
	".\splash.h"\
	

!IF  "$(CFG)" == "WordPad - Win32 Release"


"$(INTDIR)\pageset.obj" : $(SOURCE) $(DEP_CPP_PAGES) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"

"$(INTDIR)\pageset.sbr" : $(SOURCE) $(DEP_CPP_PAGES) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"

"$(INTDIR)\wordpad.idb" : $(SOURCE) $(DEP_CPP_PAGES) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"


!ELSEIF  "$(CFG)" == "WordPad - Win32 Debug"


"$(INTDIR)\pageset.obj" : $(SOURCE) $(DEP_CPP_PAGES) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"

"$(INTDIR)\pageset.sbr" : $(SOURCE) $(DEP_CPP_PAGES) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"

"$(INTDIR)\wordpad.pdb" : $(SOURCE) $(DEP_CPP_PAGES) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"

"$(INTDIR)\wordpad.idb" : $(SOURCE) $(DEP_CPP_PAGES) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"


!ELSEIF  "$(CFG)" == "WordPad - Win32 Unicode Release"


"$(INTDIR)\pageset.obj" : $(SOURCE) $(DEP_CPP_PAGES) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"

"$(INTDIR)\pageset.sbr" : $(SOURCE) $(DEP_CPP_PAGES) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"

"$(INTDIR)\wordpad.idb" : $(SOURCE) $(DEP_CPP_PAGES) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"


!ELSEIF  "$(CFG)" == "WordPad - Win32 Unicode Debug"


"$(INTDIR)\pageset.obj" : $(SOURCE) $(DEP_CPP_PAGES) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"

"$(INTDIR)\pageset.sbr" : $(SOURCE) $(DEP_CPP_PAGES) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"

"$(INTDIR)\wordpad.pdb" : $(SOURCE) $(DEP_CPP_PAGES) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"

"$(INTDIR)\wordpad.idb" : $(SOURCE) $(DEP_CPP_PAGES) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\ruler.cpp
DEP_CPP_RULER=\
	".\ruler.h"\
	".\stdafx.h"\
	".\chicdial.h"\
	".\doctype.h"\
	".\strings.h"\
	".\wordpad.h"\
	".\options.h"\
	".\splash.h"\
	".\wordpdoc.h"\
	".\wordpvw.h"\
	

!IF  "$(CFG)" == "WordPad - Win32 Release"


"$(INTDIR)\ruler.obj" : $(SOURCE) $(DEP_CPP_RULER) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"

"$(INTDIR)\ruler.sbr" : $(SOURCE) $(DEP_CPP_RULER) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"

"$(INTDIR)\wordpad.idb" : $(SOURCE) $(DEP_CPP_RULER) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"


!ELSEIF  "$(CFG)" == "WordPad - Win32 Debug"


"$(INTDIR)\ruler.obj" : $(SOURCE) $(DEP_CPP_RULER) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"

"$(INTDIR)\ruler.sbr" : $(SOURCE) $(DEP_CPP_RULER) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"

"$(INTDIR)\wordpad.pdb" : $(SOURCE) $(DEP_CPP_RULER) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"

"$(INTDIR)\wordpad.idb" : $(SOURCE) $(DEP_CPP_RULER) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"


!ELSEIF  "$(CFG)" == "WordPad - Win32 Unicode Release"


"$(INTDIR)\ruler.obj" : $(SOURCE) $(DEP_CPP_RULER) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"

"$(INTDIR)\ruler.sbr" : $(SOURCE) $(DEP_CPP_RULER) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"

"$(INTDIR)\wordpad.idb" : $(SOURCE) $(DEP_CPP_RULER) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"


!ELSEIF  "$(CFG)" == "WordPad - Win32 Unicode Debug"


"$(INTDIR)\ruler.obj" : $(SOURCE) $(DEP_CPP_RULER) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"

"$(INTDIR)\ruler.sbr" : $(SOURCE) $(DEP_CPP_RULER) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"

"$(INTDIR)\wordpad.pdb" : $(SOURCE) $(DEP_CPP_RULER) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"

"$(INTDIR)\wordpad.idb" : $(SOURCE) $(DEP_CPP_RULER) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\splash.cpp
DEP_CPP_SPLAS=\
	".\stdafx.h"\
	".\chicdial.h"\
	".\doctype.h"\
	".\wordpad.h"\
	".\options.h"\
	".\splash.h"\
	

!IF  "$(CFG)" == "WordPad - Win32 Release"


"$(INTDIR)\splash.obj" : $(SOURCE) $(DEP_CPP_SPLAS) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"

"$(INTDIR)\splash.sbr" : $(SOURCE) $(DEP_CPP_SPLAS) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"

"$(INTDIR)\wordpad.idb" : $(SOURCE) $(DEP_CPP_SPLAS) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"


!ELSEIF  "$(CFG)" == "WordPad - Win32 Debug"


"$(INTDIR)\splash.obj" : $(SOURCE) $(DEP_CPP_SPLAS) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"

"$(INTDIR)\splash.sbr" : $(SOURCE) $(DEP_CPP_SPLAS) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"

"$(INTDIR)\wordpad.pdb" : $(SOURCE) $(DEP_CPP_SPLAS) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"

"$(INTDIR)\wordpad.idb" : $(SOURCE) $(DEP_CPP_SPLAS) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"


!ELSEIF  "$(CFG)" == "WordPad - Win32 Unicode Release"


"$(INTDIR)\splash.obj" : $(SOURCE) $(DEP_CPP_SPLAS) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"

"$(INTDIR)\splash.sbr" : $(SOURCE) $(DEP_CPP_SPLAS) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"

"$(INTDIR)\wordpad.idb" : $(SOURCE) $(DEP_CPP_SPLAS) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"


!ELSEIF  "$(CFG)" == "WordPad - Win32 Unicode Debug"


"$(INTDIR)\splash.obj" : $(SOURCE) $(DEP_CPP_SPLAS) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"

"$(INTDIR)\splash.sbr" : $(SOURCE) $(DEP_CPP_SPLAS) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"

"$(INTDIR)\wordpad.pdb" : $(SOURCE) $(DEP_CPP_SPLAS) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"

"$(INTDIR)\wordpad.idb" : $(SOURCE) $(DEP_CPP_SPLAS) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\srvritem.cpp
DEP_CPP_SRVRI=\
	".\srvritem.h"\
	".\stdafx.h"\
	".\chicdial.h"\
	".\doctype.h"\
	".\wordpad.h"\
	".\options.h"\
	".\splash.h"\
	".\wordpdoc.h"\
	".\wordpvw.h"\
	

!IF  "$(CFG)" == "WordPad - Win32 Release"


"$(INTDIR)\srvritem.obj" : $(SOURCE) $(DEP_CPP_SRVRI) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"

"$(INTDIR)\srvritem.sbr" : $(SOURCE) $(DEP_CPP_SRVRI) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"

"$(INTDIR)\wordpad.idb" : $(SOURCE) $(DEP_CPP_SRVRI) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"


!ELSEIF  "$(CFG)" == "WordPad - Win32 Debug"


"$(INTDIR)\srvritem.obj" : $(SOURCE) $(DEP_CPP_SRVRI) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"

"$(INTDIR)\srvritem.sbr" : $(SOURCE) $(DEP_CPP_SRVRI) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"

"$(INTDIR)\wordpad.pdb" : $(SOURCE) $(DEP_CPP_SRVRI) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"

"$(INTDIR)\wordpad.idb" : $(SOURCE) $(DEP_CPP_SRVRI) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"


!ELSEIF  "$(CFG)" == "WordPad - Win32 Unicode Release"


"$(INTDIR)\srvritem.obj" : $(SOURCE) $(DEP_CPP_SRVRI) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"

"$(INTDIR)\srvritem.sbr" : $(SOURCE) $(DEP_CPP_SRVRI) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"

"$(INTDIR)\wordpad.idb" : $(SOURCE) $(DEP_CPP_SRVRI) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"


!ELSEIF  "$(CFG)" == "WordPad - Win32 Unicode Debug"


"$(INTDIR)\srvritem.obj" : $(SOURCE) $(DEP_CPP_SRVRI) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"

"$(INTDIR)\srvritem.sbr" : $(SOURCE) $(DEP_CPP_SRVRI) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"

"$(INTDIR)\wordpad.pdb" : $(SOURCE) $(DEP_CPP_SRVRI) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"

"$(INTDIR)\wordpad.idb" : $(SOURCE) $(DEP_CPP_SRVRI) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\stdafx.cpp
DEP_CPP_STDAF=\
	".\stdafx.h"\
	".\chicdial.h"\
	".\doctype.h"\
	

!IF  "$(CFG)" == "WordPad - Win32 Release"

# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /FR"$(INTDIR)/" /Fp"$(INTDIR)/wordpad.pch" /Yc"stdafx.h"\
 /Fo"$(INTDIR)/" /Fd"$(OUTDIR)/wordpad.pdb" /c $(SOURCE) \
	

"$(INTDIR)\stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\stdafx.sbr" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\wordpad.idb" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\wordpad.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "WordPad - Win32 Debug"

# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /D "_AFXDLL" /D "_MBCS" /FR"$(INTDIR)/" /Fp"$(INTDIR)/wordpad.pch"\
 /Yc"stdafx.h" /Fo"$(INTDIR)/" /Fd"$(OUTDIR)/wordpad.pdb" /c $(SOURCE) \
	

"$(INTDIR)\stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\stdafx.sbr" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\wordpad.pdb" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\wordpad.idb" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\wordpad.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "WordPad - Win32 Unicode Release"

# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /D "_UNICODE" /FR"$(INTDIR)/" /Fp"$(INTDIR)/wordpad.pch"\
 /Yc"stdafx.h" /Fo"$(INTDIR)/" /Fd"$(OUTDIR)/wordpad.pdb" /c $(SOURCE) \
	

"$(INTDIR)\stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\stdafx.sbr" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\wordpad.idb" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\wordpad.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "WordPad - Win32 Unicode Debug"

# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MDd /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS"\
 /D "_AFXDLL" /D "_MBCS" /D "_UNICODE" /FR"$(INTDIR)/"\
 /Fp"$(INTDIR)/wordpad.pch" /Yc"stdafx.h" /Fo"$(INTDIR)/"\
 /Fd"$(OUTDIR)/wordpad.pdb" /c $(SOURCE) \
	

"$(INTDIR)\stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\stdafx.sbr" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\wordpad.pdb" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\wordpad.idb" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\wordpad.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\strings.cpp
DEP_CPP_STRIN=\
	".\stdafx.h"\
	".\chicdial.h"\
	".\doctype.h"\
	".\strings.h"\
	

!IF  "$(CFG)" == "WordPad - Win32 Release"


"$(INTDIR)\strings.obj" : $(SOURCE) $(DEP_CPP_STRIN) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"

"$(INTDIR)\strings.sbr" : $(SOURCE) $(DEP_CPP_STRIN) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"

"$(INTDIR)\wordpad.idb" : $(SOURCE) $(DEP_CPP_STRIN) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"


!ELSEIF  "$(CFG)" == "WordPad - Win32 Debug"


"$(INTDIR)\strings.obj" : $(SOURCE) $(DEP_CPP_STRIN) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"

"$(INTDIR)\strings.sbr" : $(SOURCE) $(DEP_CPP_STRIN) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"

"$(INTDIR)\wordpad.pdb" : $(SOURCE) $(DEP_CPP_STRIN) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"

"$(INTDIR)\wordpad.idb" : $(SOURCE) $(DEP_CPP_STRIN) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"


!ELSEIF  "$(CFG)" == "WordPad - Win32 Unicode Release"


"$(INTDIR)\strings.obj" : $(SOURCE) $(DEP_CPP_STRIN) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"

"$(INTDIR)\strings.sbr" : $(SOURCE) $(DEP_CPP_STRIN) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"

"$(INTDIR)\wordpad.idb" : $(SOURCE) $(DEP_CPP_STRIN) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"


!ELSEIF  "$(CFG)" == "WordPad - Win32 Unicode Debug"


"$(INTDIR)\strings.obj" : $(SOURCE) $(DEP_CPP_STRIN) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"

"$(INTDIR)\strings.sbr" : $(SOURCE) $(DEP_CPP_STRIN) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"

"$(INTDIR)\wordpad.pdb" : $(SOURCE) $(DEP_CPP_STRIN) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"

"$(INTDIR)\wordpad.idb" : $(SOURCE) $(DEP_CPP_STRIN) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\unitspag.cpp
DEP_CPP_UNITS=\
	".\helpids.h"\
	".\stdafx.h"\
	".\chicdial.h"\
	".\doctype.h"\
	".\unitspag.h"\
	".\wordpad.h"\
	".\options.h"\
	".\splash.h"\
	

!IF  "$(CFG)" == "WordPad - Win32 Release"


"$(INTDIR)\unitspag.obj" : $(SOURCE) $(DEP_CPP_UNITS) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"

"$(INTDIR)\unitspag.sbr" : $(SOURCE) $(DEP_CPP_UNITS) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"

"$(INTDIR)\wordpad.idb" : $(SOURCE) $(DEP_CPP_UNITS) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"


!ELSEIF  "$(CFG)" == "WordPad - Win32 Debug"


"$(INTDIR)\unitspag.obj" : $(SOURCE) $(DEP_CPP_UNITS) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"

"$(INTDIR)\unitspag.sbr" : $(SOURCE) $(DEP_CPP_UNITS) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"

"$(INTDIR)\wordpad.pdb" : $(SOURCE) $(DEP_CPP_UNITS) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"

"$(INTDIR)\wordpad.idb" : $(SOURCE) $(DEP_CPP_UNITS) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"


!ELSEIF  "$(CFG)" == "WordPad - Win32 Unicode Release"


"$(INTDIR)\unitspag.obj" : $(SOURCE) $(DEP_CPP_UNITS) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"

"$(INTDIR)\unitspag.sbr" : $(SOURCE) $(DEP_CPP_UNITS) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"

"$(INTDIR)\wordpad.idb" : $(SOURCE) $(DEP_CPP_UNITS) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"


!ELSEIF  "$(CFG)" == "WordPad - Win32 Unicode Debug"


"$(INTDIR)\unitspag.obj" : $(SOURCE) $(DEP_CPP_UNITS) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"

"$(INTDIR)\unitspag.sbr" : $(SOURCE) $(DEP_CPP_UNITS) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"

"$(INTDIR)\wordpad.pdb" : $(SOURCE) $(DEP_CPP_UNITS) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"

"$(INTDIR)\wordpad.idb" : $(SOURCE) $(DEP_CPP_UNITS) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\wordpad.cpp
DEP_CPP_WORDP=\
	".\filenewd.h"\
	".\ipframe.h"\
	".\key.h"\
	".\mainfrm.h"\
	".\formatba.h"\
	".\ruler.h"\
	".\stdafx.h"\
	".\chicdial.h"\
	".\doctype.h"\
	".\strings.h"\
	".\wordpad.h"\
	".\options.h"\
	".\splash.h"\
	".\wordpdoc.h"\
	".\wordpvw.h"\
	

!IF  "$(CFG)" == "WordPad - Win32 Release"


"$(INTDIR)\wordpad.obj" : $(SOURCE) $(DEP_CPP_WORDP) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"

"$(INTDIR)\wordpad.sbr" : $(SOURCE) $(DEP_CPP_WORDP) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"

"$(INTDIR)\wordpad.idb" : $(SOURCE) $(DEP_CPP_WORDP) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"


!ELSEIF  "$(CFG)" == "WordPad - Win32 Debug"


"$(INTDIR)\wordpad.obj" : $(SOURCE) $(DEP_CPP_WORDP) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"

"$(INTDIR)\wordpad.sbr" : $(SOURCE) $(DEP_CPP_WORDP) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"

"$(INTDIR)\wordpad.pdb" : $(SOURCE) $(DEP_CPP_WORDP) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"

"$(INTDIR)\wordpad.idb" : $(SOURCE) $(DEP_CPP_WORDP) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"


!ELSEIF  "$(CFG)" == "WordPad - Win32 Unicode Release"


"$(INTDIR)\wordpad.obj" : $(SOURCE) $(DEP_CPP_WORDP) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"

"$(INTDIR)\wordpad.sbr" : $(SOURCE) $(DEP_CPP_WORDP) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"

"$(INTDIR)\wordpad.idb" : $(SOURCE) $(DEP_CPP_WORDP) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"


!ELSEIF  "$(CFG)" == "WordPad - Win32 Unicode Debug"


"$(INTDIR)\wordpad.obj" : $(SOURCE) $(DEP_CPP_WORDP) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"

"$(INTDIR)\wordpad.sbr" : $(SOURCE) $(DEP_CPP_WORDP) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"

"$(INTDIR)\wordpad.pdb" : $(SOURCE) $(DEP_CPP_WORDP) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"

"$(INTDIR)\wordpad.idb" : $(SOURCE) $(DEP_CPP_WORDP) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\wordpdoc.cpp
DEP_CPP_WORDPD=\
	".\buttondi.h"\
	".\cntritem.h"\
	".\docopt.h"\
	".\formatba.h"\
	".\helpids.h"\
	".\ipframe.h"\
	".\mainfrm.h"\
	".\ruler.h"\
	".\multconv.h"\
	".\optionsh.h"\
	".\srvritem.h"\
	".\stdafx.h"\
	".\chicdial.h"\
	".\doctype.h"\
	".\strings.h"\
	".\unitspag.h"\
	".\wordpad.h"\
	".\options.h"\
	".\splash.h"\
	".\wordpdoc.h"\
	".\wordpvw.h"\
	

!IF  "$(CFG)" == "WordPad - Win32 Release"


"$(INTDIR)\wordpdoc.obj" : $(SOURCE) $(DEP_CPP_WORDPD) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"

"$(INTDIR)\wordpdoc.sbr" : $(SOURCE) $(DEP_CPP_WORDPD) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"

"$(INTDIR)\wordpad.idb" : $(SOURCE) $(DEP_CPP_WORDPD) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"


!ELSEIF  "$(CFG)" == "WordPad - Win32 Debug"


"$(INTDIR)\wordpdoc.obj" : $(SOURCE) $(DEP_CPP_WORDPD) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"

"$(INTDIR)\wordpdoc.sbr" : $(SOURCE) $(DEP_CPP_WORDPD) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"

"$(INTDIR)\wordpad.pdb" : $(SOURCE) $(DEP_CPP_WORDPD) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"

"$(INTDIR)\wordpad.idb" : $(SOURCE) $(DEP_CPP_WORDPD) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"


!ELSEIF  "$(CFG)" == "WordPad - Win32 Unicode Release"


"$(INTDIR)\wordpdoc.obj" : $(SOURCE) $(DEP_CPP_WORDPD) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"

"$(INTDIR)\wordpdoc.sbr" : $(SOURCE) $(DEP_CPP_WORDPD) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"

"$(INTDIR)\wordpad.idb" : $(SOURCE) $(DEP_CPP_WORDPD) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"


!ELSEIF  "$(CFG)" == "WordPad - Win32 Unicode Debug"


"$(INTDIR)\wordpdoc.obj" : $(SOURCE) $(DEP_CPP_WORDPD) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"

"$(INTDIR)\wordpdoc.sbr" : $(SOURCE) $(DEP_CPP_WORDPD) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"

"$(INTDIR)\wordpad.pdb" : $(SOURCE) $(DEP_CPP_WORDPD) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"

"$(INTDIR)\wordpad.idb" : $(SOURCE) $(DEP_CPP_WORDPD) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\wordpvw.cpp
DEP_CPP_WORDPV=\
	".\cntritem.h"\
	".\colorlis.h"\
	".\datedial.h"\
	".\formatba.h"\
	".\formatpa.h"\
	".\formatta.h"\
	".\pageset.h"\
	".\ruler.h"\
	".\srvritem.h"\
	".\stdafx.h"\
	".\chicdial.h"\
	".\doctype.h"\
	".\strings.h"\
	".\wordpad.h"\
	".\options.h"\
	".\splash.h"\
	".\wordpdoc.h"\
	".\wordpvw.h"\
	

!IF  "$(CFG)" == "WordPad - Win32 Release"


"$(INTDIR)\wordpvw.obj" : $(SOURCE) $(DEP_CPP_WORDPV) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"

"$(INTDIR)\wordpvw.sbr" : $(SOURCE) $(DEP_CPP_WORDPV) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"

"$(INTDIR)\wordpad.idb" : $(SOURCE) $(DEP_CPP_WORDPV) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"


!ELSEIF  "$(CFG)" == "WordPad - Win32 Debug"


"$(INTDIR)\wordpvw.obj" : $(SOURCE) $(DEP_CPP_WORDPV) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"

"$(INTDIR)\wordpvw.sbr" : $(SOURCE) $(DEP_CPP_WORDPV) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"

"$(INTDIR)\wordpad.pdb" : $(SOURCE) $(DEP_CPP_WORDPV) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"

"$(INTDIR)\wordpad.idb" : $(SOURCE) $(DEP_CPP_WORDPV) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"


!ELSEIF  "$(CFG)" == "WordPad - Win32 Unicode Release"


"$(INTDIR)\wordpvw.obj" : $(SOURCE) $(DEP_CPP_WORDPV) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"

"$(INTDIR)\wordpvw.sbr" : $(SOURCE) $(DEP_CPP_WORDPV) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"

"$(INTDIR)\wordpad.idb" : $(SOURCE) $(DEP_CPP_WORDPV) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"


!ELSEIF  "$(CFG)" == "WordPad - Win32 Unicode Debug"


"$(INTDIR)\wordpvw.obj" : $(SOURCE) $(DEP_CPP_WORDPV) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"

"$(INTDIR)\wordpvw.sbr" : $(SOURCE) $(DEP_CPP_WORDPV) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"

"$(INTDIR)\wordpad.pdb" : $(SOURCE) $(DEP_CPP_WORDPV) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"

"$(INTDIR)\wordpad.idb" : $(SOURCE) $(DEP_CPP_WORDPV) "$(INTDIR)"\
 "$(INTDIR)\wordpad.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\wordpad.rc
DEP_RSC_WORDPA=\
	".\res\rulerbl.bmp"\
	".\res\rulerdo.bmp"\
	".\res\rulerta.bmp"\
	".\res\rulerup.bmp"\
	".\res\formatba.bmp"\
	".\res\srvr.bmp"\
	".\res\main1.bmp"\
	".\res\formatbg.bmp"\
	".\res\main1b.bmp"\
	".\res\srvrbig.bmp"\
	".\res\wordp48.bmp"\
	".\res\font.bmp"\
	".\res\rulerblm.bmp"\
	".\res\rulerdom.bmp"\
	".\res\rulertam.bmp"\
	".\res\rulerupm.bmp"\
	".\res\wordpad.ico"\
	".\res\rtfdoc.ico"\
	".\res\write.ico"\
	".\res\textdoc.ico"\
	".\res\wordpad.rc2"\
	

!IF  "$(CFG)" == "WordPad - Win32 Release"


"$(INTDIR)\wordpad.res" : $(SOURCE) $(DEP_RSC_WORDPA) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "WordPad - Win32 Debug"


"$(INTDIR)\wordpad.res" : $(SOURCE) $(DEP_RSC_WORDPA) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "WordPad - Win32 Unicode Release"


"$(INTDIR)\wordpad.res" : $(SOURCE) $(DEP_RSC_WORDPA) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "WordPad - Win32 Unicode Debug"


"$(INTDIR)\wordpad.res" : $(SOURCE) $(DEP_RSC_WORDPA) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ENDIF 

# End Source File
# End Target
# End Project
################################################################################
