# Microsoft Visual C++ Generated NMAKE File, Format Version 40000
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101
# TARGTYPE "Power Macintosh Application" 0x0401
# TARGTYPE "Macintosh Application" 0x0301

!IF "$(CFG)" == ""
CFG=PropDlg - Win32 Debug
!MESSAGE No configuration specified.  Defaulting to PropDlg - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "PropDlg - Win32 Release" && "$(CFG)" !=\
 "PropDlg - Win32 Debug" && "$(CFG)" != "PropDlg - Macintosh Release" &&\
 "$(CFG)" != "PropDlg - Macintosh Debug" && "$(CFG)" !=\
 "PropDlg - Power Macintosh Release" && "$(CFG)" !=\
 "PropDlg - Power Macintosh Debug" && "$(CFG)" !=\
 "PropDlg - Win32 Unicode Release" && "$(CFG)" !=\
 "PropDlg - Win32 Unicode Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "Propdlg.mak" CFG="PropDlg - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "PropDlg - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "PropDlg - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE "PropDlg - Macintosh Release" (based on "Macintosh Application")
!MESSAGE "PropDlg - Macintosh Debug" (based on "Macintosh Application")
!MESSAGE "PropDlg - Power Macintosh Release" (based on\
 "Power Macintosh Application")
!MESSAGE "PropDlg - Power Macintosh Debug" (based on\
 "Power Macintosh Application")
!MESSAGE "PropDlg - Win32 Unicode Release" (based on "Win32 (x86) Application")
!MESSAGE "PropDlg - Win32 Unicode Debug" (based on "Win32 (x86) Application")
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
# PROP Target_Last_Scanned "PropDlg - Win32 Debug"

!IF  "$(CFG)" == "PropDlg - Win32 Release"

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

ALL : "$(OUTDIR)\Propdlg.exe"

CLEAN : 
	-@erase ".\Release\vc40.idb"
	-@erase ".\Release\Propdlg.pch"
	-@erase ".\Release\Propdlg.exe"
	-@erase ".\Release\Colorpge.obj"
	-@erase ".\Release\Mainfrm.obj"
	-@erase ".\Release\Shapesvw.obj"
	-@erase ".\Release\Propsht.obj"
	-@erase ".\Release\Stdafx.obj"
	-@erase ".\Release\Preview.obj"
	-@erase ".\Release\Shapeobj.obj"
	-@erase ".\Release\Propdlg.obj"
	-@erase ".\Release\Propsht2.obj"
	-@erase ".\Release\Minifrm.obj"
	-@erase ".\Release\Shapedoc.obj"
	-@erase ".\Release\Stylepge.obj"
	-@erase ".\Release\Propdlg.res"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"Stdafx.h" /c
# SUBTRACT CPP /Fr
CPP_PROJ=/nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/Propdlg.pch" /Yu"Stdafx.h" /Fo"$(INTDIR)/"\
 /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\Release/
CPP_SBRS=

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

MTL=mktyplib.exe
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
RSC=rc.exe
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/Propdlg.res" /d "NDEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/Propdlg.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 /nologo /subsystem:windows /machine:I386
LINK32_FLAGS=/nologo /subsystem:windows /incremental:no\
 /pdb:"$(OUTDIR)/Propdlg.pdb" /machine:I386 /out:"$(OUTDIR)/Propdlg.exe" 
LINK32_OBJS= \
	"$(INTDIR)/Colorpge.obj" \
	"$(INTDIR)/Mainfrm.obj" \
	"$(INTDIR)/Shapesvw.obj" \
	"$(INTDIR)/Propsht.obj" \
	"$(INTDIR)/Stdafx.obj" \
	"$(INTDIR)/Preview.obj" \
	"$(INTDIR)/Shapeobj.obj" \
	"$(INTDIR)/Propdlg.obj" \
	"$(INTDIR)/Propsht2.obj" \
	"$(INTDIR)/Minifrm.obj" \
	"$(INTDIR)/Shapedoc.obj" \
	"$(INTDIR)/Stylepge.obj" \
	"$(INTDIR)/Propdlg.res"

"$(OUTDIR)\Propdlg.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "PropDlg - Win32 Debug"

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

ALL : "$(OUTDIR)\Propdlg.exe"

CLEAN : 
	-@erase ".\Debug\vc40.pdb"
	-@erase ".\Debug\Propdlg.pch"
	-@erase ".\Debug\vc40.idb"
	-@erase ".\Debug\Propdlg.exe"
	-@erase ".\Debug\Propsht2.obj"
	-@erase ".\Debug\Mainfrm.obj"
	-@erase ".\Debug\Stylepge.obj"
	-@erase ".\Debug\Shapedoc.obj"
	-@erase ".\Debug\Propsht.obj"
	-@erase ".\Debug\Preview.obj"
	-@erase ".\Debug\Shapesvw.obj"
	-@erase ".\Debug\Colorpge.obj"
	-@erase ".\Debug\Propdlg.obj"
	-@erase ".\Debug\Minifrm.obj"
	-@erase ".\Debug\Stdafx.obj"
	-@erase ".\Debug\Shapeobj.obj"
	-@erase ".\Debug\Propdlg.res"
	-@erase ".\Debug\Propdlg.ilk"
	-@erase ".\Debug\Propdlg.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"Stdafx.h" /c
# SUBTRACT CPP /Fr
CPP_PROJ=/nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/Propdlg.pch" /Yu"Stdafx.h"\
 /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\Debug/
CPP_SBRS=

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

MTL=mktyplib.exe
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
RSC=rc.exe
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/Propdlg.res" /d "_DEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/Propdlg.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386
# ADD LINK32 /nologo /subsystem:windows /debug /machine:I386
LINK32_FLAGS=/nologo /subsystem:windows /incremental:yes\
 /pdb:"$(OUTDIR)/Propdlg.pdb" /debug /machine:I386 /out:"$(OUTDIR)/Propdlg.exe" 
LINK32_OBJS= \
	"$(INTDIR)/Propsht2.obj" \
	"$(INTDIR)/Mainfrm.obj" \
	"$(INTDIR)/Stylepge.obj" \
	"$(INTDIR)/Shapedoc.obj" \
	"$(INTDIR)/Propsht.obj" \
	"$(INTDIR)/Preview.obj" \
	"$(INTDIR)/Shapesvw.obj" \
	"$(INTDIR)/Colorpge.obj" \
	"$(INTDIR)/Propdlg.obj" \
	"$(INTDIR)/Minifrm.obj" \
	"$(INTDIR)/Stdafx.obj" \
	"$(INTDIR)/Shapeobj.obj" \
	"$(INTDIR)/Propdlg.res"

"$(OUTDIR)\Propdlg.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "PropDlg - Macintosh Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir MacRel
# PROP BASE Intermediate_Dir MacRel
# PROP BASE Target_Dir
# PROP Use_MFC 1
# PROP Use_Debug_Libraries 0
# PROP Output_Dir MacRel
# PROP Intermediate_Dir MacRel
# PROP Target_Dir
OUTDIR=.\MacRel
INTDIR=.\MacRel

ALL : "$(OUTDIR)\Propdlg.exe" "$(OUTDIR)\Propdlg.trg"

CLEAN : 
	-@erase ".\MacRel\Propdlg.trg"
	-@erase ".\MacRel\Propdlg.exe"
	-@erase ".\MacRel\Minifrm.obj"
	-@erase ".\MacRel\Propdlg.pch"
	-@erase ".\MacRel\Stdafx.obj"
	-@erase ".\MacRel\Shapesvw.obj"
	-@erase ".\MacRel\Colorpge.obj"
	-@erase ".\MacRel\Shapeobj.obj"
	-@erase ".\MacRel\Mainfrm.obj"
	-@erase ".\MacRel\Propsht.obj"
	-@erase ".\MacRel\Propsht2.obj"
	-@erase ".\MacRel\Preview.obj"
	-@erase ".\MacRel\Stylepge.obj"
	-@erase ".\MacRel\Shapedoc.obj"
	-@erase ".\MacRel\Propdlg.obj"
	-@erase ".\MacRel\propmac.rsc"
	-@erase ".\MacRel\Propdlg.rsc"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

MTL=mktyplib.exe
# ADD BASE MTL /nologo /D "NDEBUG" /mac
# ADD MTL /nologo /D "NDEBUG" /mac
MTL_PROJ=/nologo /D "NDEBUG" /mac 
CPP=cl.exe
# ADD BASE CPP /nologo /AL /Q68s /W3 /GX /O2 /D "_WINDOWS" /D "_MAC" /D "_68K_" /D "WIN32" /D "NDEBUG" /YX /c
# ADD CPP /nologo /AL /Q68s /W3 /GX /O2 /D "_WINDOWS" /D "_MAC" /D "_68K_" /D "WIN32" /D "NDEBUG" /D "_MBCS" /Yu"Stdafx.h" /c
# SUBTRACT CPP /Fr
CPP_PROJ=/nologo /AL /Q68s /W3 /GX /O2 /D "_WINDOWS" /D "_MAC" /D "_68K_" /D\
 "WIN32" /D "NDEBUG" /D "_MBCS" /Fp"$(INTDIR)/Propdlg.pch" /Yu"Stdafx.h"\
 /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\MacRel/
CPP_SBRS=

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

RSC=rc.exe
# ADD BASE RSC /l 0x409 /r /d "_MAC" /d "_68K_" /d "NDEBUG"
# ADD RSC /l 0x409 /r /d "_MAC" /d "_68K_" /d "NDEBUG"
RSC_PROJ=/l 0x409 /r /m /fo"$(INTDIR)/Propdlg.rsc" /d "_MAC" /d "_68K_" /d\
 "NDEBUG" 
MRC=mrc.exe
# ADD BASE MRC /D "_68K_" /D "_MAC" /D "NDEBUG" /NOLOGO
# ADD MRC /D "_68K_" /D "_MAC" /D "NDEBUG" /NOLOGO
MRC_PROJ=/D "_68K_" /D "_MAC" /D "NDEBUG" /l 0x409 /NOLOGO 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/Propdlg.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 wlm.lib /nologo /MAC:bundle /machine:M68K
# ADD LINK32 /nologo /MAC:bundle /MAC:creator="PROP" /machine:M68K
LINK32_FLAGS=/nologo /MAC:bundle /MAC:type="APPL" /MAC:creator="PROP"\
 /pdb:"$(OUTDIR)/Propdlg.pdb" /machine:M68K /out:"$(OUTDIR)/Propdlg.exe" 
LINK32_OBJS= \
	"$(INTDIR)/Minifrm.obj" \
	"$(INTDIR)/Stdafx.obj" \
	"$(INTDIR)/Shapesvw.obj" \
	"$(INTDIR)/Colorpge.obj" \
	"$(INTDIR)/Shapeobj.obj" \
	"$(INTDIR)/Mainfrm.obj" \
	"$(INTDIR)/Propsht.obj" \
	"$(INTDIR)/Propsht2.obj" \
	"$(INTDIR)/Preview.obj" \
	"$(INTDIR)/Stylepge.obj" \
	"$(INTDIR)/Shapedoc.obj" \
	"$(INTDIR)/Propdlg.obj" \
	"$(INTDIR)/propmac.rsc" \
	"$(INTDIR)/Propdlg.rsc"

"$(OUTDIR)\Propdlg.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

MFILE32=mfile.exe
# ADD BASE MFILE32 COPY /NOLOGO
# ADD MFILE32 COPY /NOLOGO
MFILE32_FLAGS=COPY /NOLOGO 
MFILE32_FILES= \
	"$(OUTDIR)/Propdlg.exe"

"$(OUTDIR)\Propdlg.trg" : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\MacRel\Propdlg.exe "$(MFILE32_DEST)"

DOWNLOAD : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\MacRel\Propdlg.exe "$(MFILE32_DEST)"

!ELSEIF  "$(CFG)" == "PropDlg - Macintosh Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir MacDbg
# PROP BASE Intermediate_Dir MacDbg
# PROP BASE Target_Dir
# PROP Use_MFC 1
# PROP Use_Debug_Libraries 1
# PROP Output_Dir MacDbg
# PROP Intermediate_Dir MacDbg
# PROP Target_Dir
OUTDIR=.\MacDbg
INTDIR=.\MacDbg

ALL : "$(OUTDIR)\Propdlg.exe" "$(OUTDIR)\Propdlg.trg"

CLEAN : 
	-@erase ".\MacDbg\vc40.pdb"
	-@erase ".\MacDbg\Propdlg.pch"
	-@erase ".\MacDbg\Propdlg.trg"
	-@erase ".\MacDbg\Propdlg.exe"
	-@erase ".\MacDbg\Stdafx.obj"
	-@erase ".\MacDbg\Shapesvw.obj"
	-@erase ".\MacDbg\Colorpge.obj"
	-@erase ".\MacDbg\Propdlg.obj"
	-@erase ".\MacDbg\Minifrm.obj"
	-@erase ".\MacDbg\Shapeobj.obj"
	-@erase ".\MacDbg\Propsht2.obj"
	-@erase ".\MacDbg\Mainfrm.obj"
	-@erase ".\MacDbg\Stylepge.obj"
	-@erase ".\MacDbg\Shapedoc.obj"
	-@erase ".\MacDbg\Propsht.obj"
	-@erase ".\MacDbg\Preview.obj"
	-@erase ".\MacDbg\propmac.rsc"
	-@erase ".\MacDbg\Propdlg.rsc"
	-@erase ".\MacDbg\Propdlg.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

MTL=mktyplib.exe
# ADD BASE MTL /nologo /D "_DEBUG" /mac
# ADD MTL /nologo /D "_DEBUG" /mac
MTL_PROJ=/nologo /D "_DEBUG" /mac 
CPP=cl.exe
# ADD BASE CPP /nologo /AL /Q68s /Q68m /W3 /GX /Zi /Od /D "_WINDOWS" /D "_MAC" /D "_68K_" /D "WIN32" /D "_DEBUG" /YX /c
# ADD CPP /nologo /AL /Q68s /Q68m /W3 /GX /Zi /Od /D "_WINDOWS" /D "_MAC" /D "_68K_" /D "WIN32" /D "_DEBUG" /D "_MBCS" /Yu"Stdafx.h" /c
# SUBTRACT CPP /Fr
CPP_PROJ=/nologo /AL /Q68s /Q68m /W3 /GX /Zi /Od /D "_WINDOWS" /D "_MAC" /D\
 "_68K_" /D "WIN32" /D "_DEBUG" /D "_MBCS" /Fp"$(INTDIR)/Propdlg.pch"\
 /Yu"Stdafx.h" /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\MacDbg/
CPP_SBRS=

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

RSC=rc.exe
# ADD BASE RSC /l 0x409 /r /d "_MAC" /d "_68K_" /d "_DEBUG"
# ADD RSC /l 0x409 /r /d "_MAC" /d "_68K_" /d "_DEBUG"
RSC_PROJ=/l 0x409 /r /m /fo"$(INTDIR)/Propdlg.rsc" /d "_MAC" /d "_68K_" /d\
 "_DEBUG" 
MRC=mrc.exe
# ADD BASE MRC /D "_68K_" /D "_MAC" /D "_DEBUG" /NOLOGO
# ADD MRC /D "_68K_" /D "_MAC" /D "_DEBUG" /NOLOGO
MRC_PROJ=/D "_68K_" /D "_MAC" /D "_DEBUG" /l 0x409 /NOLOGO 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/Propdlg.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 wlm.lib /nologo /MAC:bundle /debug /machine:M68K
# ADD LINK32 /nologo /MAC:bundle /MAC:creator="PROP" /debug /machine:M68K
LINK32_FLAGS=/nologo /MAC:bundle /MAC:type="APPL" /MAC:creator="PROP"\
 /pdb:"$(OUTDIR)/Propdlg.pdb" /debug /machine:M68K /out:"$(OUTDIR)/Propdlg.exe" 
LINK32_OBJS= \
	"$(INTDIR)/Stdafx.obj" \
	"$(INTDIR)/Shapesvw.obj" \
	"$(INTDIR)/Colorpge.obj" \
	"$(INTDIR)/Propdlg.obj" \
	"$(INTDIR)/Minifrm.obj" \
	"$(INTDIR)/Shapeobj.obj" \
	"$(INTDIR)/Propsht2.obj" \
	"$(INTDIR)/Mainfrm.obj" \
	"$(INTDIR)/Stylepge.obj" \
	"$(INTDIR)/Shapedoc.obj" \
	"$(INTDIR)/Propsht.obj" \
	"$(INTDIR)/Preview.obj" \
	"$(INTDIR)/propmac.rsc" \
	"$(INTDIR)/Propdlg.rsc"

"$(OUTDIR)\Propdlg.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

MFILE32=mfile.exe
# ADD BASE MFILE32 COPY /NOLOGO
# ADD MFILE32 COPY /NOLOGO
MFILE32_FLAGS=COPY /NOLOGO 
MFILE32_FILES= \
	"$(OUTDIR)/Propdlg.exe"

"$(OUTDIR)\Propdlg.trg" : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\MacDbg\Propdlg.exe "$(MFILE32_DEST)"

DOWNLOAD : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\MacDbg\Propdlg.exe "$(MFILE32_DEST)"

!ELSEIF  "$(CFG)" == "PropDlg - Power Macintosh Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir PMacRel
# PROP BASE Intermediate_Dir PMacRel
# PROP BASE Target_Dir
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 0
# PROP Output_Dir PMacRel
# PROP Intermediate_Dir PMacRel
# PROP Target_Dir
OUTDIR=.\PMacRel
INTDIR=.\PMacRel

ALL : "$(OUTDIR)\Propdlg.exe" "$(OUTDIR)\Propdlg.trg"

CLEAN : 
	-@erase ".\PMacRel\Propdlg.trg"
	-@erase ".\PMacRel\Propdlg.exe"
	-@erase ".\PMacRel\Shapedoc.obj"
	-@erase ".\PMacRel\Propdlg.pch"
	-@erase ".\PMacRel\Stylepge.obj"
	-@erase ".\PMacRel\Propdlg.obj"
	-@erase ".\PMacRel\Minifrm.obj"
	-@erase ".\PMacRel\Shapesvw.obj"
	-@erase ".\PMacRel\Colorpge.obj"
	-@erase ".\PMacRel\Stdafx.obj"
	-@erase ".\PMacRel\Shapeobj.obj"
	-@erase ".\PMacRel\Propsht.obj"
	-@erase ".\PMacRel\Mainfrm.obj"
	-@erase ".\PMacRel\Propsht2.obj"
	-@erase ".\PMacRel\Preview.obj"
	-@erase ".\PMacRel\propmac.rsc"
	-@erase ".\PMacRel\Propdlg.rsc"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

MTL=mktyplib.exe
# ADD BASE MTL /nologo /D "NDEBUG"
# ADD MTL /nologo /D "NDEBUG"
MTL_PROJ=/nologo /D "NDEBUG" 
CPP=cl.exe
# ADD BASE CPP /nologo /ML /W3 /GX /O2 /D "_MAC" /D "_MPPC_" /D "_WINDOWS" /D "WIN32" /D "NDEBUG" /YX /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "_MAC" /D "_MPPC_" /D "_WINDOWS" /D "WIN32" /D "_MBCS" /D "_AFXDLL" /Yu"Stdafx.h" /c
# SUBTRACT CPP /Fr
CPP_PROJ=/nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "_MAC" /D "_MPPC_" /D\
 "_WINDOWS" /D "WIN32" /D "_MBCS" /D "_AFXDLL" /Fp"$(INTDIR)/Propdlg.pch"\
 /Yu"Stdafx.h" /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\PMacRel/
CPP_SBRS=

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

RSC=rc.exe
# ADD BASE RSC /l 0x409 /r /d "_MAC" /d "_MPPC_" /d "NDEBUG"
# ADD RSC /l 0x409 /r /d "_MAC" /d "_MPPC_" /d "NDEBUG"
RSC_PROJ=/l 0x409 /r /m /fo"$(INTDIR)/Propdlg.rsc" /d "_MAC" /d "_MPPC_" /d\
 "NDEBUG" 
MRC=mrc.exe
# ADD BASE MRC /D "_MPPC_" /D "_MAC" /D "NDEBUG" /NOLOGO
# ADD MRC /D "_MPPC_" /D "_MAC" /D "NDEBUG" /NOLOGO
MRC_PROJ=/D "_MPPC_" /D "_MAC" /D "NDEBUG" /l 0x409 /NOLOGO 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/Propdlg.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 cfrg.rsc /nologo /mac:bundle /machine:MPPC
# ADD LINK32 /nologo /mac:bundle /mac:creator="PROP" /machine:MPPC
LINK32_FLAGS=/nologo /mac:bundle /mac:type="APPL" /mac:creator="PROP"\
 /incremental:no /pdb:"$(OUTDIR)/Propdlg.pdb" /machine:MPPC\
 /out:"$(OUTDIR)/Propdlg.exe" 
LINK32_OBJS= \
	"$(INTDIR)/Shapedoc.obj" \
	"$(INTDIR)/Stylepge.obj" \
	"$(INTDIR)/Propdlg.obj" \
	"$(INTDIR)/Minifrm.obj" \
	"$(INTDIR)/Shapesvw.obj" \
	"$(INTDIR)/Colorpge.obj" \
	"$(INTDIR)/Stdafx.obj" \
	"$(INTDIR)/Shapeobj.obj" \
	"$(INTDIR)/Propsht.obj" \
	"$(INTDIR)/Mainfrm.obj" \
	"$(INTDIR)/Propsht2.obj" \
	"$(INTDIR)/Preview.obj" \
	"$(INTDIR)/propmac.rsc" \
	"$(INTDIR)/Propdlg.rsc"

"$(OUTDIR)\Propdlg.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

MFILE32=mfile.exe
# ADD BASE MFILE32 COPY /NOLOGO
# ADD MFILE32 COPY /NOLOGO
MFILE32_FLAGS=COPY /NOLOGO 
MFILE32_FILES= \
	"$(OUTDIR)/Propdlg.exe"

"$(OUTDIR)\Propdlg.trg" : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\PMacRel\Propdlg.exe "$(MFILE32_DEST)"

DOWNLOAD : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\PMacRel\Propdlg.exe "$(MFILE32_DEST)"

!ELSEIF  "$(CFG)" == "PropDlg - Power Macintosh Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir PMacDbg
# PROP BASE Intermediate_Dir PMacDbg
# PROP BASE Target_Dir
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 1
# PROP Output_Dir PMacDbg
# PROP Intermediate_Dir PMacDbg
# PROP Target_Dir
OUTDIR=.\PMacDbg
INTDIR=.\PMacDbg

ALL : "$(OUTDIR)\Propdlg.exe" "$(OUTDIR)\Propdlg.trg"

CLEAN : 
	-@erase ".\PMacDbg\vc40.pdb"
	-@erase ".\PMacDbg\Propdlg.pch"
	-@erase ".\PMacDbg\Propdlg.trg"
	-@erase ".\PMacDbg\Propdlg.exe"
	-@erase ".\PMacDbg\Propsht2.obj"
	-@erase ".\PMacDbg\Mainfrm.obj"
	-@erase ".\PMacDbg\Stylepge.obj"
	-@erase ".\PMacDbg\Shapedoc.obj"
	-@erase ".\PMacDbg\Propsht.obj"
	-@erase ".\PMacDbg\Preview.obj"
	-@erase ".\PMacDbg\Shapesvw.obj"
	-@erase ".\PMacDbg\Colorpge.obj"
	-@erase ".\PMacDbg\Propdlg.obj"
	-@erase ".\PMacDbg\Minifrm.obj"
	-@erase ".\PMacDbg\Stdafx.obj"
	-@erase ".\PMacDbg\Shapeobj.obj"
	-@erase ".\PMacDbg\propmac.rsc"
	-@erase ".\PMacDbg\Propdlg.rsc"
	-@erase ".\PMacDbg\Propdlg.ilk"
	-@erase ".\PMacDbg\Propdlg.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

MTL=mktyplib.exe
# ADD BASE MTL /nologo /D "_DEBUG"
# ADD MTL /nologo /D "_DEBUG"
MTL_PROJ=/nologo /D "_DEBUG" 
CPP=cl.exe
# ADD BASE CPP /nologo /W3 /GX /Zi /Od /D "_MAC" /D "_MPPC_" /D "_WINDOWS" /D "WIN32" /D "_DEBUG" /YX /c
# ADD CPP /nologo /MDd /W3 /GX /Zi /Od /D "_DEBUG" /D "_MAC" /D "_MPPC_" /D "_WINDOWS" /D "WIN32" /D "_MBCS" /D "_AFXDLL" /Yu"Stdafx.h" /c
# SUBTRACT CPP /Fr
CPP_PROJ=/nologo /MDd /W3 /GX /Zi /Od /D "_DEBUG" /D "_MAC" /D "_MPPC_" /D\
 "_WINDOWS" /D "WIN32" /D "_MBCS" /D "_AFXDLL" /Fp"$(INTDIR)/Propdlg.pch"\
 /Yu"Stdafx.h" /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\PMacDbg/
CPP_SBRS=

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

RSC=rc.exe
# ADD BASE RSC /l 0x409 /r /d "_MAC" /d "_MPPC_" /d "_DEBUG"
# ADD RSC /l 0x409 /r /d "_MAC" /d "_MPPC_" /d "_DEBUG"
RSC_PROJ=/l 0x409 /r /m /fo"$(INTDIR)/Propdlg.rsc" /d "_MAC" /d "_MPPC_" /d\
 "_DEBUG" 
MRC=mrc.exe
# ADD BASE MRC /D "_MPPC_" /D "_MAC" /D "_DEBUG" /NOLOGO
# ADD MRC /D "_MPPC_" /D "_MAC" /D "_DEBUG" /NOLOGO
MRC_PROJ=/D "_MPPC_" /D "_MAC" /D "_DEBUG" /l 0x409 /NOLOGO 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/Propdlg.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 cfrg.rsc /nologo /mac:bundle /debug /machine:MPPC
# ADD LINK32 /nologo /mac:bundle /mac:creator="PROP" /debug /machine:MPPC
LINK32_FLAGS=/nologo /mac:bundle /mac:type="APPL" /mac:creator="PROP"\
 /incremental:yes /pdb:"$(OUTDIR)/Propdlg.pdb" /debug /machine:MPPC\
 /out:"$(OUTDIR)/Propdlg.exe" 
LINK32_OBJS= \
	"$(INTDIR)/Propsht2.obj" \
	"$(INTDIR)/Mainfrm.obj" \
	"$(INTDIR)/Stylepge.obj" \
	"$(INTDIR)/Shapedoc.obj" \
	"$(INTDIR)/Propsht.obj" \
	"$(INTDIR)/Preview.obj" \
	"$(INTDIR)/Shapesvw.obj" \
	"$(INTDIR)/Colorpge.obj" \
	"$(INTDIR)/Propdlg.obj" \
	"$(INTDIR)/Minifrm.obj" \
	"$(INTDIR)/Stdafx.obj" \
	"$(INTDIR)/Shapeobj.obj" \
	"$(INTDIR)/propmac.rsc" \
	"$(INTDIR)/Propdlg.rsc"

"$(OUTDIR)\Propdlg.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

MFILE32=mfile.exe
# ADD BASE MFILE32 COPY /NOLOGO
# ADD MFILE32 COPY /NOLOGO
MFILE32_FLAGS=COPY /NOLOGO 
MFILE32_FILES= \
	"$(OUTDIR)/Propdlg.exe"

"$(OUTDIR)\Propdlg.trg" : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\PMacDbg\Propdlg.exe "$(MFILE32_DEST)"

DOWNLOAD : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\PMacDbg\Propdlg.exe "$(MFILE32_DEST)"

!ELSEIF  "$(CFG)" == "PropDlg - Win32 Unicode Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir Release
# PROP BASE Intermediate_Dir Release
# PROP BASE Target_Dir
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 0
# PROP Output_Dir UniRelease
# PROP Intermediate_Dir UniRelease
# PROP Target_Dir
OUTDIR=.\UniRelease
INTDIR=.\UniRelease

ALL : "$(OUTDIR)\Propdlg.exe"

CLEAN : 
	-@erase ".\UniRelease\vc40.idb"
	-@erase ".\UniRelease\Propdlg.pch"
	-@erase ".\UniRelease\Propdlg.exe"
	-@erase ".\UniRelease\Propdlg.obj"
	-@erase ".\UniRelease\Shapeobj.obj"
	-@erase ".\UniRelease\Minifrm.obj"
	-@erase ".\UniRelease\Propsht2.obj"
	-@erase ".\UniRelease\Stylepge.obj"
	-@erase ".\UniRelease\Stdafx.obj"
	-@erase ".\UniRelease\Shapedoc.obj"
	-@erase ".\UniRelease\Mainfrm.obj"
	-@erase ".\UniRelease\Propsht.obj"
	-@erase ".\UniRelease\Preview.obj"
	-@erase ".\UniRelease\Shapesvw.obj"
	-@erase ".\UniRelease\Colorpge.obj"
	-@erase ".\UniRelease\Propdlg.res"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_UNICODE" /D "_AFXDLL" /D "_MBCS" /Yu"Stdafx.h" /c
# SUBTRACT CPP /Fr
CPP_PROJ=/nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D\
 "_UNICODE" /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/Propdlg.pch" /Yu"Stdafx.h"\
 /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\UniRelease/
CPP_SBRS=

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

MTL=mktyplib.exe
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
RSC=rc.exe
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/Propdlg.res" /d "NDEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/Propdlg.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 /nologo /entry:"wWinMainCRTStartup" /subsystem:windows /machine:I386
LINK32_FLAGS=/nologo /entry:"wWinMainCRTStartup" /subsystem:windows\
 /incremental:no /pdb:"$(OUTDIR)/Propdlg.pdb" /machine:I386\
 /out:"$(OUTDIR)/Propdlg.exe" 
LINK32_OBJS= \
	"$(INTDIR)/Propdlg.obj" \
	"$(INTDIR)/Shapeobj.obj" \
	"$(INTDIR)/Minifrm.obj" \
	"$(INTDIR)/Propsht2.obj" \
	"$(INTDIR)/Stylepge.obj" \
	"$(INTDIR)/Stdafx.obj" \
	"$(INTDIR)/Shapedoc.obj" \
	"$(INTDIR)/Mainfrm.obj" \
	"$(INTDIR)/Propsht.obj" \
	"$(INTDIR)/Preview.obj" \
	"$(INTDIR)/Shapesvw.obj" \
	"$(INTDIR)/Colorpge.obj" \
	"$(INTDIR)/Propdlg.res"

"$(OUTDIR)\Propdlg.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "PropDlg - Win32 Unicode Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir Debug
# PROP BASE Intermediate_Dir Debug
# PROP BASE Target_Dir
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 1
# PROP Output_Dir UniDebug
# PROP Intermediate_Dir UniDebug
# PROP Target_Dir
OUTDIR=.\UniDebug
INTDIR=.\UniDebug

ALL : "$(OUTDIR)\Propdlg.exe"

CLEAN : 
	-@erase ".\UniDebug\vc40.pdb"
	-@erase ".\UniDebug\Propdlg.pch"
	-@erase ".\UniDebug\vc40.idb"
	-@erase ".\UniDebug\Propdlg.exe"
	-@erase ".\UniDebug\Shapeobj.obj"
	-@erase ".\UniDebug\Mainfrm.obj"
	-@erase ".\UniDebug\Propsht2.obj"
	-@erase ".\UniDebug\Propsht.obj"
	-@erase ".\UniDebug\Shapedoc.obj"
	-@erase ".\UniDebug\Preview.obj"
	-@erase ".\UniDebug\Stylepge.obj"
	-@erase ".\UniDebug\Stdafx.obj"
	-@erase ".\UniDebug\Propdlg.obj"
	-@erase ".\UniDebug\Colorpge.obj"
	-@erase ".\UniDebug\Minifrm.obj"
	-@erase ".\UniDebug\Shapesvw.obj"
	-@erase ".\UniDebug\Propdlg.res"
	-@erase ".\UniDebug\Propdlg.ilk"
	-@erase ".\UniDebug\Propdlg.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_UNICODE" /D "_AFXDLL" /D "_MBCS" /Yu"Stdafx.h" /c
# SUBTRACT CPP /Fr
CPP_PROJ=/nologo /MDd /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS"\
 /D "_UNICODE" /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/Propdlg.pch" /Yu"Stdafx.h"\
 /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\UniDebug/
CPP_SBRS=

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

MTL=mktyplib.exe
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
RSC=rc.exe
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/Propdlg.res" /d "_DEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/Propdlg.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386
# ADD LINK32 /nologo /entry:"wWinMainCRTStartup" /subsystem:windows /debug /machine:I386
LINK32_FLAGS=/nologo /entry:"wWinMainCRTStartup" /subsystem:windows\
 /incremental:yes /pdb:"$(OUTDIR)/Propdlg.pdb" /debug /machine:I386\
 /out:"$(OUTDIR)/Propdlg.exe" 
LINK32_OBJS= \
	"$(INTDIR)/Shapeobj.obj" \
	"$(INTDIR)/Mainfrm.obj" \
	"$(INTDIR)/Propsht2.obj" \
	"$(INTDIR)/Propsht.obj" \
	"$(INTDIR)/Shapedoc.obj" \
	"$(INTDIR)/Preview.obj" \
	"$(INTDIR)/Stylepge.obj" \
	"$(INTDIR)/Stdafx.obj" \
	"$(INTDIR)/Propdlg.obj" \
	"$(INTDIR)/Colorpge.obj" \
	"$(INTDIR)/Minifrm.obj" \
	"$(INTDIR)/Shapesvw.obj" \
	"$(INTDIR)/Propdlg.res"

"$(OUTDIR)\Propdlg.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 

################################################################################
# Begin Target

# Name "PropDlg - Win32 Release"
# Name "PropDlg - Win32 Debug"
# Name "PropDlg - Macintosh Release"
# Name "PropDlg - Macintosh Debug"
# Name "PropDlg - Power Macintosh Release"
# Name "PropDlg - Power Macintosh Debug"
# Name "PropDlg - Win32 Unicode Release"
# Name "PropDlg - Win32 Unicode Debug"

!IF  "$(CFG)" == "PropDlg - Win32 Release"

!ELSEIF  "$(CFG)" == "PropDlg - Win32 Debug"

!ELSEIF  "$(CFG)" == "PropDlg - Macintosh Release"

!ELSEIF  "$(CFG)" == "PropDlg - Macintosh Debug"

!ELSEIF  "$(CFG)" == "PropDlg - Power Macintosh Release"

!ELSEIF  "$(CFG)" == "PropDlg - Power Macintosh Debug"

!ELSEIF  "$(CFG)" == "PropDlg - Win32 Unicode Release"

!ELSEIF  "$(CFG)" == "PropDlg - Win32 Unicode Debug"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\Propdlg.rc
DEP_RSC_PROPD=\
	".\Res\Propdlg.ico"\
	".\Res\Shapedoc.ico"\
	".\Res\Toolbar.bmp"\
	".\Res\Propdlg.rc2"\
	

!IF  "$(CFG)" == "PropDlg - Win32 Release"


"$(INTDIR)\Propdlg.res" : $(SOURCE) $(DEP_RSC_PROPD) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "PropDlg - Win32 Debug"


"$(INTDIR)\Propdlg.res" : $(SOURCE) $(DEP_RSC_PROPD) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "PropDlg - Macintosh Release"


"$(INTDIR)\Propdlg.rsc" : $(SOURCE) $(DEP_RSC_PROPD) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "PropDlg - Macintosh Debug"


"$(INTDIR)\Propdlg.rsc" : $(SOURCE) $(DEP_RSC_PROPD) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "PropDlg - Power Macintosh Release"


"$(INTDIR)\Propdlg.rsc" : $(SOURCE) $(DEP_RSC_PROPD) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "PropDlg - Power Macintosh Debug"


"$(INTDIR)\Propdlg.rsc" : $(SOURCE) $(DEP_RSC_PROPD) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "PropDlg - Win32 Unicode Release"


"$(INTDIR)\Propdlg.res" : $(SOURCE) $(DEP_RSC_PROPD) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "PropDlg - Win32 Unicode Debug"


"$(INTDIR)\Propdlg.res" : $(SOURCE) $(DEP_RSC_PROPD) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Mainfrm.cpp

!IF  "$(CFG)" == "PropDlg - Win32 Release"

DEP_CPP_MAINF=\
	".\Colorpge.h"\
	".\Mainfrm.h"\
	".\Minifrm.h"\
	".\Preview.h"\
	".\Propdlg.h"\
	".\Resource.h"\
	".\Propsht.h"\
	".\Propsht2.h"\
	".\Shapedoc.h"\
	".\Shapeobj.h"\
	".\Shapesvw.h"\
	".\Stdafx.h"\
	".\Stylepge.h"\
	

"$(INTDIR)\Mainfrm.obj" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\Propdlg.pch"

"$(INTDIR)\vc40.idb" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\Propdlg.pch"


!ELSEIF  "$(CFG)" == "PropDlg - Win32 Debug"

DEP_CPP_MAINF=\
	".\Colorpge.h"\
	".\Mainfrm.h"\
	".\Minifrm.h"\
	".\Preview.h"\
	".\Propdlg.h"\
	".\Resource.h"\
	".\Propsht.h"\
	".\Propsht2.h"\
	".\Shapedoc.h"\
	".\Shapeobj.h"\
	".\Shapesvw.h"\
	".\Stdafx.h"\
	".\Stylepge.h"\
	

"$(INTDIR)\Mainfrm.obj" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\Propdlg.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\Propdlg.pch"

"$(INTDIR)\vc40.idb" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\Propdlg.pch"


!ELSEIF  "$(CFG)" == "PropDlg - Macintosh Release"

DEP_CPP_MAINF=\
	".\Colorpge.h"\
	".\Mainfrm.h"\
	".\Minifrm.h"\
	".\Preview.h"\
	".\Propdlg.h"\
	".\Resource.h"\
	".\Propsht.h"\
	".\Propsht2.h"\
	".\Shapedoc.h"\
	".\Shapeobj.h"\
	".\Shapesvw.h"\
	".\Stdafx.h"\
	".\Stylepge.h"\
	

"$(INTDIR)\Mainfrm.obj" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\Propdlg.pch"


!ELSEIF  "$(CFG)" == "PropDlg - Macintosh Debug"

DEP_CPP_MAINF=\
	".\Colorpge.h"\
	".\Mainfrm.h"\
	".\Minifrm.h"\
	".\Preview.h"\
	".\Propdlg.h"\
	".\Resource.h"\
	".\Propsht.h"\
	".\Propsht2.h"\
	".\Shapedoc.h"\
	".\Shapeobj.h"\
	".\Shapesvw.h"\
	".\Stdafx.h"\
	".\Stylepge.h"\
	

"$(INTDIR)\Mainfrm.obj" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\Propdlg.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\Propdlg.pch"


!ELSEIF  "$(CFG)" == "PropDlg - Power Macintosh Release"

DEP_CPP_MAINF=\
	".\Colorpge.h"\
	".\Mainfrm.h"\
	".\Minifrm.h"\
	".\Preview.h"\
	".\Propdlg.h"\
	".\Propsht.h"\
	".\Propsht2.h"\
	".\Shapedoc.h"\
	".\Shapeobj.h"\
	".\Shapesvw.h"\
	".\Stdafx.h"\
	".\Stylepge.h"\
	

"$(INTDIR)\Mainfrm.obj" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\Propdlg.pch"


!ELSEIF  "$(CFG)" == "PropDlg - Power Macintosh Debug"

DEP_CPP_MAINF=\
	".\Colorpge.h"\
	".\Mainfrm.h"\
	".\Minifrm.h"\
	".\Preview.h"\
	".\Propdlg.h"\
	".\Propsht.h"\
	".\Propsht2.h"\
	".\Shapedoc.h"\
	".\Shapeobj.h"\
	".\Shapesvw.h"\
	".\Stdafx.h"\
	".\Stylepge.h"\
	

"$(INTDIR)\Mainfrm.obj" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\Propdlg.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\Propdlg.pch"


!ELSEIF  "$(CFG)" == "PropDlg - Win32 Unicode Release"

DEP_CPP_MAINF=\
	".\Stdafx.h"\
	".\Propdlg.h"\
	".\Colorpge.h"\
	".\Stylepge.h"\
	".\Shapeobj.h"\
	".\Preview.h"\
	".\Propsht.h"\
	".\Propsht2.h"\
	".\Minifrm.h"\
	".\Mainfrm.h"\
	".\Shapedoc.h"\
	".\Shapesvw.h"\
	

"$(INTDIR)\Mainfrm.obj" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\Propdlg.pch"

"$(INTDIR)\vc40.idb" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\Propdlg.pch"


!ELSEIF  "$(CFG)" == "PropDlg - Win32 Unicode Debug"

DEP_CPP_MAINF=\
	".\Stdafx.h"\
	".\Propdlg.h"\
	".\Colorpge.h"\
	".\Stylepge.h"\
	".\Shapeobj.h"\
	".\Preview.h"\
	".\Propsht.h"\
	".\Propsht2.h"\
	".\Minifrm.h"\
	".\Mainfrm.h"\
	".\Shapedoc.h"\
	".\Shapesvw.h"\
	

"$(INTDIR)\Mainfrm.obj" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\Propdlg.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\Propdlg.pch"

"$(INTDIR)\vc40.idb" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\Propdlg.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Minifrm.cpp

!IF  "$(CFG)" == "PropDlg - Win32 Release"

DEP_CPP_MINIF=\
	".\Colorpge.h"\
	".\Mainfrm.h"\
	".\Minifrm.h"\
	".\Propsht2.h"\
	".\Resource.h"\
	".\Shapeobj.h"\
	".\Stdafx.h"\
	".\Stylepge.h"\
	

"$(INTDIR)\Minifrm.obj" : $(SOURCE) $(DEP_CPP_MINIF) "$(INTDIR)"\
 "$(INTDIR)\Propdlg.pch"

"$(INTDIR)\vc40.idb" : $(SOURCE) $(DEP_CPP_MINIF) "$(INTDIR)"\
 "$(INTDIR)\Propdlg.pch"


!ELSEIF  "$(CFG)" == "PropDlg - Win32 Debug"

DEP_CPP_MINIF=\
	".\Colorpge.h"\
	".\Mainfrm.h"\
	".\Minifrm.h"\
	".\Propsht2.h"\
	".\Resource.h"\
	".\Shapeobj.h"\
	".\Stdafx.h"\
	".\Stylepge.h"\
	

"$(INTDIR)\Minifrm.obj" : $(SOURCE) $(DEP_CPP_MINIF) "$(INTDIR)"\
 "$(INTDIR)\Propdlg.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_MINIF) "$(INTDIR)"\
 "$(INTDIR)\Propdlg.pch"

"$(INTDIR)\vc40.idb" : $(SOURCE) $(DEP_CPP_MINIF) "$(INTDIR)"\
 "$(INTDIR)\Propdlg.pch"


!ELSEIF  "$(CFG)" == "PropDlg - Macintosh Release"


"$(INTDIR)\Minifrm.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Propdlg.pch"


!ELSEIF  "$(CFG)" == "PropDlg - Macintosh Debug"


"$(INTDIR)\Minifrm.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Propdlg.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Propdlg.pch"


!ELSEIF  "$(CFG)" == "PropDlg - Power Macintosh Release"

DEP_CPP_MINIF=\
	".\Colorpge.h"\
	".\Mainfrm.h"\
	".\Minifrm.h"\
	".\Propsht2.h"\
	".\Shapeobj.h"\
	".\Stdafx.h"\
	".\Stylepge.h"\
	

"$(INTDIR)\Minifrm.obj" : $(SOURCE) $(DEP_CPP_MINIF) "$(INTDIR)"\
 "$(INTDIR)\Propdlg.pch"


!ELSEIF  "$(CFG)" == "PropDlg - Power Macintosh Debug"

DEP_CPP_MINIF=\
	".\Colorpge.h"\
	".\Mainfrm.h"\
	".\Minifrm.h"\
	".\Propsht2.h"\
	".\Shapeobj.h"\
	".\Stdafx.h"\
	".\Stylepge.h"\
	

"$(INTDIR)\Minifrm.obj" : $(SOURCE) $(DEP_CPP_MINIF) "$(INTDIR)"\
 "$(INTDIR)\Propdlg.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_MINIF) "$(INTDIR)"\
 "$(INTDIR)\Propdlg.pch"


!ELSEIF  "$(CFG)" == "PropDlg - Win32 Unicode Release"

DEP_CPP_MINIF=\
	".\Stdafx.h"\
	".\Shapeobj.h"\
	".\Colorpge.h"\
	".\Stylepge.h"\
	".\Propsht2.h"\
	".\Minifrm.h"\
	".\Mainfrm.h"\
	

"$(INTDIR)\Minifrm.obj" : $(SOURCE) $(DEP_CPP_MINIF) "$(INTDIR)"\
 "$(INTDIR)\Propdlg.pch"

"$(INTDIR)\vc40.idb" : $(SOURCE) $(DEP_CPP_MINIF) "$(INTDIR)"\
 "$(INTDIR)\Propdlg.pch"


!ELSEIF  "$(CFG)" == "PropDlg - Win32 Unicode Debug"

DEP_CPP_MINIF=\
	".\Stdafx.h"\
	".\Shapeobj.h"\
	".\Colorpge.h"\
	".\Stylepge.h"\
	".\Propsht2.h"\
	".\Minifrm.h"\
	".\Mainfrm.h"\
	

"$(INTDIR)\Minifrm.obj" : $(SOURCE) $(DEP_CPP_MINIF) "$(INTDIR)"\
 "$(INTDIR)\Propdlg.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_MINIF) "$(INTDIR)"\
 "$(INTDIR)\Propdlg.pch"

"$(INTDIR)\vc40.idb" : $(SOURCE) $(DEP_CPP_MINIF) "$(INTDIR)"\
 "$(INTDIR)\Propdlg.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Preview.cpp

!IF  "$(CFG)" == "PropDlg - Win32 Release"


"$(INTDIR)\Preview.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Propdlg.pch"

"$(INTDIR)\vc40.idb" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Propdlg.pch"


!ELSEIF  "$(CFG)" == "PropDlg - Win32 Debug"


"$(INTDIR)\Preview.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Propdlg.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Propdlg.pch"

"$(INTDIR)\vc40.idb" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Propdlg.pch"


!ELSEIF  "$(CFG)" == "PropDlg - Macintosh Release"


"$(INTDIR)\Preview.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Propdlg.pch"


!ELSEIF  "$(CFG)" == "PropDlg - Macintosh Debug"


"$(INTDIR)\Preview.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Propdlg.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Propdlg.pch"


!ELSEIF  "$(CFG)" == "PropDlg - Power Macintosh Release"

DEP_CPP_PREVI=\
	".\Colorpge.h"\
	".\Preview.h"\
	".\Propsht.h"\
	".\Shapeobj.h"\
	".\Stdafx.h"\
	".\Stylepge.h"\
	

"$(INTDIR)\Preview.obj" : $(SOURCE) $(DEP_CPP_PREVI) "$(INTDIR)"\
 "$(INTDIR)\Propdlg.pch"


!ELSEIF  "$(CFG)" == "PropDlg - Power Macintosh Debug"

DEP_CPP_PREVI=\
	".\Colorpge.h"\
	".\Preview.h"\
	".\Propsht.h"\
	".\Shapeobj.h"\
	".\Stdafx.h"\
	".\Stylepge.h"\
	

"$(INTDIR)\Preview.obj" : $(SOURCE) $(DEP_CPP_PREVI) "$(INTDIR)"\
 "$(INTDIR)\Propdlg.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_PREVI) "$(INTDIR)"\
 "$(INTDIR)\Propdlg.pch"


!ELSEIF  "$(CFG)" == "PropDlg - Win32 Unicode Release"

DEP_CPP_PREVI=\
	".\Stdafx.h"\
	".\Preview.h"\
	".\Colorpge.h"\
	".\Stylepge.h"\
	".\Shapeobj.h"\
	".\Propsht.h"\
	

"$(INTDIR)\Preview.obj" : $(SOURCE) $(DEP_CPP_PREVI) "$(INTDIR)"\
 "$(INTDIR)\Propdlg.pch"

"$(INTDIR)\vc40.idb" : $(SOURCE) $(DEP_CPP_PREVI) "$(INTDIR)"\
 "$(INTDIR)\Propdlg.pch"


!ELSEIF  "$(CFG)" == "PropDlg - Win32 Unicode Debug"

DEP_CPP_PREVI=\
	".\Stdafx.h"\
	".\Preview.h"\
	".\Colorpge.h"\
	".\Stylepge.h"\
	".\Shapeobj.h"\
	".\Propsht.h"\
	

"$(INTDIR)\Preview.obj" : $(SOURCE) $(DEP_CPP_PREVI) "$(INTDIR)"\
 "$(INTDIR)\Propdlg.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_PREVI) "$(INTDIR)"\
 "$(INTDIR)\Propdlg.pch"

"$(INTDIR)\vc40.idb" : $(SOURCE) $(DEP_CPP_PREVI) "$(INTDIR)"\
 "$(INTDIR)\Propdlg.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Propdlg.cpp

!IF  "$(CFG)" == "PropDlg - Win32 Release"


"$(INTDIR)\Propdlg.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Propdlg.pch"

"$(INTDIR)\vc40.idb" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Propdlg.pch"


!ELSEIF  "$(CFG)" == "PropDlg - Win32 Debug"


"$(INTDIR)\Propdlg.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Propdlg.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Propdlg.pch"

"$(INTDIR)\vc40.idb" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Propdlg.pch"


!ELSEIF  "$(CFG)" == "PropDlg - Macintosh Release"


"$(INTDIR)\Propdlg.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Propdlg.pch"


!ELSEIF  "$(CFG)" == "PropDlg - Macintosh Debug"


"$(INTDIR)\Propdlg.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Propdlg.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Propdlg.pch"


!ELSEIF  "$(CFG)" == "PropDlg - Power Macintosh Release"

DEP_CPP_PROPDL=\
	".\Colorpge.h"\
	".\Mainfrm.h"\
	".\Minifrm.h"\
	".\Preview.h"\
	".\Propdlg.h"\
	".\Propsht.h"\
	".\Propsht2.h"\
	".\Shapedoc.h"\
	".\Shapeobj.h"\
	".\Shapesvw.h"\
	".\Stdafx.h"\
	".\Stylepge.h"\
	

"$(INTDIR)\Propdlg.obj" : $(SOURCE) $(DEP_CPP_PROPDL) "$(INTDIR)"\
 "$(INTDIR)\Propdlg.pch"


!ELSEIF  "$(CFG)" == "PropDlg - Power Macintosh Debug"

DEP_CPP_PROPDL=\
	".\Colorpge.h"\
	".\Mainfrm.h"\
	".\Minifrm.h"\
	".\Preview.h"\
	".\Propdlg.h"\
	".\Propsht.h"\
	".\Propsht2.h"\
	".\Shapedoc.h"\
	".\Shapeobj.h"\
	".\Shapesvw.h"\
	".\Stdafx.h"\
	".\Stylepge.h"\
	

"$(INTDIR)\Propdlg.obj" : $(SOURCE) $(DEP_CPP_PROPDL) "$(INTDIR)"\
 "$(INTDIR)\Propdlg.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_PROPDL) "$(INTDIR)"\
 "$(INTDIR)\Propdlg.pch"


!ELSEIF  "$(CFG)" == "PropDlg - Win32 Unicode Release"

DEP_CPP_PROPDL=\
	".\Stdafx.h"\
	".\Propdlg.h"\
	".\Shapeobj.h"\
	".\Colorpge.h"\
	".\Stylepge.h"\
	".\Preview.h"\
	".\Propsht.h"\
	".\Propsht2.h"\
	".\Minifrm.h"\
	".\Mainfrm.h"\
	".\Shapedoc.h"\
	".\Shapesvw.h"\
	

"$(INTDIR)\Propdlg.obj" : $(SOURCE) $(DEP_CPP_PROPDL) "$(INTDIR)"\
 "$(INTDIR)\Propdlg.pch"

"$(INTDIR)\vc40.idb" : $(SOURCE) $(DEP_CPP_PROPDL) "$(INTDIR)"\
 "$(INTDIR)\Propdlg.pch"


!ELSEIF  "$(CFG)" == "PropDlg - Win32 Unicode Debug"

DEP_CPP_PROPDL=\
	".\Stdafx.h"\
	".\Propdlg.h"\
	".\Shapeobj.h"\
	".\Colorpge.h"\
	".\Stylepge.h"\
	".\Preview.h"\
	".\Propsht.h"\
	".\Propsht2.h"\
	".\Minifrm.h"\
	".\Mainfrm.h"\
	".\Shapedoc.h"\
	".\Shapesvw.h"\
	

"$(INTDIR)\Propdlg.obj" : $(SOURCE) $(DEP_CPP_PROPDL) "$(INTDIR)"\
 "$(INTDIR)\Propdlg.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_PROPDL) "$(INTDIR)"\
 "$(INTDIR)\Propdlg.pch"

"$(INTDIR)\vc40.idb" : $(SOURCE) $(DEP_CPP_PROPDL) "$(INTDIR)"\
 "$(INTDIR)\Propdlg.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Colorpge.cpp

!IF  "$(CFG)" == "PropDlg - Win32 Release"


"$(INTDIR)\Colorpge.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Propdlg.pch"

"$(INTDIR)\vc40.idb" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Propdlg.pch"


!ELSEIF  "$(CFG)" == "PropDlg - Win32 Debug"


"$(INTDIR)\Colorpge.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Propdlg.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Propdlg.pch"

"$(INTDIR)\vc40.idb" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Propdlg.pch"


!ELSEIF  "$(CFG)" == "PropDlg - Macintosh Release"


"$(INTDIR)\Colorpge.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Propdlg.pch"


!ELSEIF  "$(CFG)" == "PropDlg - Macintosh Debug"


"$(INTDIR)\Colorpge.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Propdlg.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Propdlg.pch"


!ELSEIF  "$(CFG)" == "PropDlg - Power Macintosh Release"

DEP_CPP_COLOR=\
	".\Colorpge.h"\
	".\Preview.h"\
	".\Propdlg.h"\
	".\Propsht.h"\
	".\Propsht2.h"\
	".\Shapeobj.h"\
	".\Stdafx.h"\
	".\Stylepge.h"\
	

"$(INTDIR)\Colorpge.obj" : $(SOURCE) $(DEP_CPP_COLOR) "$(INTDIR)"\
 "$(INTDIR)\Propdlg.pch"


!ELSEIF  "$(CFG)" == "PropDlg - Power Macintosh Debug"

DEP_CPP_COLOR=\
	".\Colorpge.h"\
	".\Preview.h"\
	".\Propdlg.h"\
	".\Propsht.h"\
	".\Propsht2.h"\
	".\Shapeobj.h"\
	".\Stdafx.h"\
	".\Stylepge.h"\
	

"$(INTDIR)\Colorpge.obj" : $(SOURCE) $(DEP_CPP_COLOR) "$(INTDIR)"\
 "$(INTDIR)\Propdlg.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_COLOR) "$(INTDIR)"\
 "$(INTDIR)\Propdlg.pch"


!ELSEIF  "$(CFG)" == "PropDlg - Win32 Unicode Release"

DEP_CPP_COLOR=\
	".\Stdafx.h"\
	".\Propdlg.h"\
	".\Stylepge.h"\
	".\Colorpge.h"\
	".\Shapeobj.h"\
	".\Preview.h"\
	".\Propsht.h"\
	".\Propsht2.h"\
	

"$(INTDIR)\Colorpge.obj" : $(SOURCE) $(DEP_CPP_COLOR) "$(INTDIR)"\
 "$(INTDIR)\Propdlg.pch"

"$(INTDIR)\vc40.idb" : $(SOURCE) $(DEP_CPP_COLOR) "$(INTDIR)"\
 "$(INTDIR)\Propdlg.pch"


!ELSEIF  "$(CFG)" == "PropDlg - Win32 Unicode Debug"

DEP_CPP_COLOR=\
	".\Stdafx.h"\
	".\Propdlg.h"\
	".\Stylepge.h"\
	".\Colorpge.h"\
	".\Shapeobj.h"\
	".\Preview.h"\
	".\Propsht.h"\
	".\Propsht2.h"\
	

"$(INTDIR)\Colorpge.obj" : $(SOURCE) $(DEP_CPP_COLOR) "$(INTDIR)"\
 "$(INTDIR)\Propdlg.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_COLOR) "$(INTDIR)"\
 "$(INTDIR)\Propdlg.pch"

"$(INTDIR)\vc40.idb" : $(SOURCE) $(DEP_CPP_COLOR) "$(INTDIR)"\
 "$(INTDIR)\Propdlg.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Propsht.cpp

!IF  "$(CFG)" == "PropDlg - Win32 Release"


"$(INTDIR)\Propsht.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Propdlg.pch"

"$(INTDIR)\vc40.idb" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Propdlg.pch"


!ELSEIF  "$(CFG)" == "PropDlg - Win32 Debug"


"$(INTDIR)\Propsht.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Propdlg.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Propdlg.pch"

"$(INTDIR)\vc40.idb" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Propdlg.pch"


!ELSEIF  "$(CFG)" == "PropDlg - Macintosh Release"


"$(INTDIR)\Propsht.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Propdlg.pch"


!ELSEIF  "$(CFG)" == "PropDlg - Macintosh Debug"


"$(INTDIR)\Propsht.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Propdlg.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Propdlg.pch"


!ELSEIF  "$(CFG)" == "PropDlg - Power Macintosh Release"

DEP_CPP_PROPS=\
	".\Colorpge.h"\
	".\Preview.h"\
	".\Propdlg.h"\
	".\Propsht.h"\
	".\Shapeobj.h"\
	".\Stdafx.h"\
	".\Stylepge.h"\
	

"$(INTDIR)\Propsht.obj" : $(SOURCE) $(DEP_CPP_PROPS) "$(INTDIR)"\
 "$(INTDIR)\Propdlg.pch"


!ELSEIF  "$(CFG)" == "PropDlg - Power Macintosh Debug"

DEP_CPP_PROPS=\
	".\Colorpge.h"\
	".\Preview.h"\
	".\Propdlg.h"\
	".\Propsht.h"\
	".\Shapeobj.h"\
	".\Stdafx.h"\
	".\Stylepge.h"\
	

"$(INTDIR)\Propsht.obj" : $(SOURCE) $(DEP_CPP_PROPS) "$(INTDIR)"\
 "$(INTDIR)\Propdlg.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_PROPS) "$(INTDIR)"\
 "$(INTDIR)\Propdlg.pch"


!ELSEIF  "$(CFG)" == "PropDlg - Win32 Unicode Release"

DEP_CPP_PROPS=\
	".\Stdafx.h"\
	".\Propdlg.h"\
	".\Shapeobj.h"\
	".\Colorpge.h"\
	".\Stylepge.h"\
	".\Preview.h"\
	".\Propsht.h"\
	

"$(INTDIR)\Propsht.obj" : $(SOURCE) $(DEP_CPP_PROPS) "$(INTDIR)"\
 "$(INTDIR)\Propdlg.pch"

"$(INTDIR)\vc40.idb" : $(SOURCE) $(DEP_CPP_PROPS) "$(INTDIR)"\
 "$(INTDIR)\Propdlg.pch"


!ELSEIF  "$(CFG)" == "PropDlg - Win32 Unicode Debug"

DEP_CPP_PROPS=\
	".\Stdafx.h"\
	".\Propdlg.h"\
	".\Shapeobj.h"\
	".\Colorpge.h"\
	".\Stylepge.h"\
	".\Preview.h"\
	".\Propsht.h"\
	

"$(INTDIR)\Propsht.obj" : $(SOURCE) $(DEP_CPP_PROPS) "$(INTDIR)"\
 "$(INTDIR)\Propdlg.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_PROPS) "$(INTDIR)"\
 "$(INTDIR)\Propdlg.pch"

"$(INTDIR)\vc40.idb" : $(SOURCE) $(DEP_CPP_PROPS) "$(INTDIR)"\
 "$(INTDIR)\Propdlg.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Propsht2.cpp

!IF  "$(CFG)" == "PropDlg - Win32 Release"


"$(INTDIR)\Propsht2.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Propdlg.pch"

"$(INTDIR)\vc40.idb" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Propdlg.pch"


!ELSEIF  "$(CFG)" == "PropDlg - Win32 Debug"


"$(INTDIR)\Propsht2.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Propdlg.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Propdlg.pch"

"$(INTDIR)\vc40.idb" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Propdlg.pch"


!ELSEIF  "$(CFG)" == "PropDlg - Macintosh Release"


"$(INTDIR)\Propsht2.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Propdlg.pch"


!ELSEIF  "$(CFG)" == "PropDlg - Macintosh Debug"


"$(INTDIR)\Propsht2.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Propdlg.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Propdlg.pch"


!ELSEIF  "$(CFG)" == "PropDlg - Power Macintosh Release"

DEP_CPP_PROPSH=\
	".\Colorpge.h"\
	".\Propdlg.h"\
	".\Propsht2.h"\
	".\Shapeobj.h"\
	".\Stdafx.h"\
	".\Stylepge.h"\
	

"$(INTDIR)\Propsht2.obj" : $(SOURCE) $(DEP_CPP_PROPSH) "$(INTDIR)"\
 "$(INTDIR)\Propdlg.pch"


!ELSEIF  "$(CFG)" == "PropDlg - Power Macintosh Debug"

DEP_CPP_PROPSH=\
	".\Colorpge.h"\
	".\Propdlg.h"\
	".\Propsht2.h"\
	".\Shapeobj.h"\
	".\Stdafx.h"\
	".\Stylepge.h"\
	

"$(INTDIR)\Propsht2.obj" : $(SOURCE) $(DEP_CPP_PROPSH) "$(INTDIR)"\
 "$(INTDIR)\Propdlg.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_PROPSH) "$(INTDIR)"\
 "$(INTDIR)\Propdlg.pch"


!ELSEIF  "$(CFG)" == "PropDlg - Win32 Unicode Release"

DEP_CPP_PROPSH=\
	".\Stdafx.h"\
	".\Propdlg.h"\
	".\Shapeobj.h"\
	".\Colorpge.h"\
	".\Stylepge.h"\
	".\Propsht2.h"\
	

"$(INTDIR)\Propsht2.obj" : $(SOURCE) $(DEP_CPP_PROPSH) "$(INTDIR)"\
 "$(INTDIR)\Propdlg.pch"

"$(INTDIR)\vc40.idb" : $(SOURCE) $(DEP_CPP_PROPSH) "$(INTDIR)"\
 "$(INTDIR)\Propdlg.pch"


!ELSEIF  "$(CFG)" == "PropDlg - Win32 Unicode Debug"

DEP_CPP_PROPSH=\
	".\Stdafx.h"\
	".\Propdlg.h"\
	".\Shapeobj.h"\
	".\Colorpge.h"\
	".\Stylepge.h"\
	".\Propsht2.h"\
	

"$(INTDIR)\Propsht2.obj" : $(SOURCE) $(DEP_CPP_PROPSH) "$(INTDIR)"\
 "$(INTDIR)\Propdlg.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_PROPSH) "$(INTDIR)"\
 "$(INTDIR)\Propdlg.pch"

"$(INTDIR)\vc40.idb" : $(SOURCE) $(DEP_CPP_PROPSH) "$(INTDIR)"\
 "$(INTDIR)\Propdlg.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Shapedoc.cpp

!IF  "$(CFG)" == "PropDlg - Win32 Release"


"$(INTDIR)\Shapedoc.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Propdlg.pch"

"$(INTDIR)\vc40.idb" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Propdlg.pch"


!ELSEIF  "$(CFG)" == "PropDlg - Win32 Debug"


"$(INTDIR)\Shapedoc.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Propdlg.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Propdlg.pch"

"$(INTDIR)\vc40.idb" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Propdlg.pch"


!ELSEIF  "$(CFG)" == "PropDlg - Macintosh Release"


"$(INTDIR)\Shapedoc.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Propdlg.pch"


!ELSEIF  "$(CFG)" == "PropDlg - Macintosh Debug"


"$(INTDIR)\Shapedoc.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Propdlg.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Propdlg.pch"


!ELSEIF  "$(CFG)" == "PropDlg - Power Macintosh Release"

DEP_CPP_SHAPE=\
	".\Propdlg.h"\
	".\Shapedoc.h"\
	".\Shapeobj.h"\
	".\Stdafx.h"\
	

"$(INTDIR)\Shapedoc.obj" : $(SOURCE) $(DEP_CPP_SHAPE) "$(INTDIR)"\
 "$(INTDIR)\Propdlg.pch"


!ELSEIF  "$(CFG)" == "PropDlg - Power Macintosh Debug"

DEP_CPP_SHAPE=\
	".\Propdlg.h"\
	".\Shapedoc.h"\
	".\Shapeobj.h"\
	".\Stdafx.h"\
	

"$(INTDIR)\Shapedoc.obj" : $(SOURCE) $(DEP_CPP_SHAPE) "$(INTDIR)"\
 "$(INTDIR)\Propdlg.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_SHAPE) "$(INTDIR)"\
 "$(INTDIR)\Propdlg.pch"


!ELSEIF  "$(CFG)" == "PropDlg - Win32 Unicode Release"

DEP_CPP_SHAPE=\
	".\Stdafx.h"\
	".\Propdlg.h"\
	".\Shapeobj.h"\
	".\Shapedoc.h"\
	

"$(INTDIR)\Shapedoc.obj" : $(SOURCE) $(DEP_CPP_SHAPE) "$(INTDIR)"\
 "$(INTDIR)\Propdlg.pch"

"$(INTDIR)\vc40.idb" : $(SOURCE) $(DEP_CPP_SHAPE) "$(INTDIR)"\
 "$(INTDIR)\Propdlg.pch"


!ELSEIF  "$(CFG)" == "PropDlg - Win32 Unicode Debug"

DEP_CPP_SHAPE=\
	".\Stdafx.h"\
	".\Propdlg.h"\
	".\Shapeobj.h"\
	".\Shapedoc.h"\
	

"$(INTDIR)\Shapedoc.obj" : $(SOURCE) $(DEP_CPP_SHAPE) "$(INTDIR)"\
 "$(INTDIR)\Propdlg.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_SHAPE) "$(INTDIR)"\
 "$(INTDIR)\Propdlg.pch"

"$(INTDIR)\vc40.idb" : $(SOURCE) $(DEP_CPP_SHAPE) "$(INTDIR)"\
 "$(INTDIR)\Propdlg.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Shapeobj.cpp

!IF  "$(CFG)" == "PropDlg - Win32 Release"


"$(INTDIR)\Shapeobj.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Propdlg.pch"

"$(INTDIR)\vc40.idb" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Propdlg.pch"


!ELSEIF  "$(CFG)" == "PropDlg - Win32 Debug"


"$(INTDIR)\Shapeobj.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Propdlg.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Propdlg.pch"

"$(INTDIR)\vc40.idb" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Propdlg.pch"


!ELSEIF  "$(CFG)" == "PropDlg - Macintosh Release"


"$(INTDIR)\Shapeobj.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Propdlg.pch"


!ELSEIF  "$(CFG)" == "PropDlg - Macintosh Debug"


"$(INTDIR)\Shapeobj.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Propdlg.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Propdlg.pch"


!ELSEIF  "$(CFG)" == "PropDlg - Power Macintosh Release"

DEP_CPP_SHAPEO=\
	".\Propdlg.h"\
	".\Shapeobj.h"\
	".\Stdafx.h"\
	

"$(INTDIR)\Shapeobj.obj" : $(SOURCE) $(DEP_CPP_SHAPEO) "$(INTDIR)"\
 "$(INTDIR)\Propdlg.pch"


!ELSEIF  "$(CFG)" == "PropDlg - Power Macintosh Debug"

DEP_CPP_SHAPEO=\
	".\Propdlg.h"\
	".\Shapeobj.h"\
	".\Stdafx.h"\
	

"$(INTDIR)\Shapeobj.obj" : $(SOURCE) $(DEP_CPP_SHAPEO) "$(INTDIR)"\
 "$(INTDIR)\Propdlg.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_SHAPEO) "$(INTDIR)"\
 "$(INTDIR)\Propdlg.pch"


!ELSEIF  "$(CFG)" == "PropDlg - Win32 Unicode Release"

DEP_CPP_SHAPEO=\
	".\Stdafx.h"\
	".\Propdlg.h"\
	".\Shapeobj.h"\
	

"$(INTDIR)\Shapeobj.obj" : $(SOURCE) $(DEP_CPP_SHAPEO) "$(INTDIR)"\
 "$(INTDIR)\Propdlg.pch"

"$(INTDIR)\vc40.idb" : $(SOURCE) $(DEP_CPP_SHAPEO) "$(INTDIR)"\
 "$(INTDIR)\Propdlg.pch"


!ELSEIF  "$(CFG)" == "PropDlg - Win32 Unicode Debug"

DEP_CPP_SHAPEO=\
	".\Stdafx.h"\
	".\Propdlg.h"\
	".\Shapeobj.h"\
	

"$(INTDIR)\Shapeobj.obj" : $(SOURCE) $(DEP_CPP_SHAPEO) "$(INTDIR)"\
 "$(INTDIR)\Propdlg.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_SHAPEO) "$(INTDIR)"\
 "$(INTDIR)\Propdlg.pch"

"$(INTDIR)\vc40.idb" : $(SOURCE) $(DEP_CPP_SHAPEO) "$(INTDIR)"\
 "$(INTDIR)\Propdlg.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Shapesvw.cpp

!IF  "$(CFG)" == "PropDlg - Win32 Release"


"$(INTDIR)\Shapesvw.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Propdlg.pch"

"$(INTDIR)\vc40.idb" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Propdlg.pch"


!ELSEIF  "$(CFG)" == "PropDlg - Win32 Debug"


"$(INTDIR)\Shapesvw.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Propdlg.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Propdlg.pch"

"$(INTDIR)\vc40.idb" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Propdlg.pch"


!ELSEIF  "$(CFG)" == "PropDlg - Macintosh Release"


"$(INTDIR)\Shapesvw.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Propdlg.pch"


!ELSEIF  "$(CFG)" == "PropDlg - Macintosh Debug"


"$(INTDIR)\Shapesvw.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Propdlg.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Propdlg.pch"


!ELSEIF  "$(CFG)" == "PropDlg - Power Macintosh Release"

DEP_CPP_SHAPES=\
	".\Colorpge.h"\
	".\Mainfrm.h"\
	".\Minifrm.h"\
	".\Preview.h"\
	".\Propdlg.h"\
	".\Propsht.h"\
	".\Propsht2.h"\
	".\Shapedoc.h"\
	".\Shapeobj.h"\
	".\Shapesvw.h"\
	".\Stdafx.h"\
	".\Stylepge.h"\
	

"$(INTDIR)\Shapesvw.obj" : $(SOURCE) $(DEP_CPP_SHAPES) "$(INTDIR)"\
 "$(INTDIR)\Propdlg.pch"


!ELSEIF  "$(CFG)" == "PropDlg - Power Macintosh Debug"

DEP_CPP_SHAPES=\
	".\Colorpge.h"\
	".\Mainfrm.h"\
	".\Minifrm.h"\
	".\Preview.h"\
	".\Propdlg.h"\
	".\Propsht.h"\
	".\Propsht2.h"\
	".\Shapedoc.h"\
	".\Shapeobj.h"\
	".\Shapesvw.h"\
	".\Stdafx.h"\
	".\Stylepge.h"\
	

"$(INTDIR)\Shapesvw.obj" : $(SOURCE) $(DEP_CPP_SHAPES) "$(INTDIR)"\
 "$(INTDIR)\Propdlg.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_SHAPES) "$(INTDIR)"\
 "$(INTDIR)\Propdlg.pch"


!ELSEIF  "$(CFG)" == "PropDlg - Win32 Unicode Release"

DEP_CPP_SHAPES=\
	".\Stdafx.h"\
	".\Propdlg.h"\
	".\Shapeobj.h"\
	".\Colorpge.h"\
	".\Stylepge.h"\
	".\Preview.h"\
	".\Propsht.h"\
	".\Propsht2.h"\
	".\Shapedoc.h"\
	".\Shapesvw.h"\
	".\Minifrm.h"\
	".\Mainfrm.h"\
	

"$(INTDIR)\Shapesvw.obj" : $(SOURCE) $(DEP_CPP_SHAPES) "$(INTDIR)"\
 "$(INTDIR)\Propdlg.pch"

"$(INTDIR)\vc40.idb" : $(SOURCE) $(DEP_CPP_SHAPES) "$(INTDIR)"\
 "$(INTDIR)\Propdlg.pch"


!ELSEIF  "$(CFG)" == "PropDlg - Win32 Unicode Debug"

DEP_CPP_SHAPES=\
	".\Stdafx.h"\
	".\Propdlg.h"\
	".\Shapeobj.h"\
	".\Colorpge.h"\
	".\Stylepge.h"\
	".\Preview.h"\
	".\Propsht.h"\
	".\Propsht2.h"\
	".\Shapedoc.h"\
	".\Shapesvw.h"\
	".\Minifrm.h"\
	".\Mainfrm.h"\
	

"$(INTDIR)\Shapesvw.obj" : $(SOURCE) $(DEP_CPP_SHAPES) "$(INTDIR)"\
 "$(INTDIR)\Propdlg.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_SHAPES) "$(INTDIR)"\
 "$(INTDIR)\Propdlg.pch"

"$(INTDIR)\vc40.idb" : $(SOURCE) $(DEP_CPP_SHAPES) "$(INTDIR)"\
 "$(INTDIR)\Propdlg.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Stdafx.cpp

!IF  "$(CFG)" == "PropDlg - Win32 Release"

# ADD CPP /Yc

BuildCmds= \
	$(CPP) /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/Propdlg.pch" /Yc /Fo"$(INTDIR)/"\
 /Fd"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\Stdafx.obj" : $(SOURCE) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\vc40.idb" : $(SOURCE) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\Propdlg.pch" : $(SOURCE) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "PropDlg - Win32 Debug"

# ADD CPP /Yc

BuildCmds= \
	$(CPP) /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/Propdlg.pch" /Yc /Fo"$(INTDIR)/"\
 /Fd"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\Stdafx.obj" : $(SOURCE) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\vc40.pdb" : $(SOURCE) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\vc40.idb" : $(SOURCE) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\Propdlg.pch" : $(SOURCE) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "PropDlg - Macintosh Release"

# ADD CPP /Yc

BuildCmds= \
	$(CPP) /nologo /AL /Q68s /W3 /GX /O2 /D "_WINDOWS" /D "_MAC" /D "_68K_" /D\
 "WIN32" /D "NDEBUG" /D "_MBCS" /Fp"$(INTDIR)/Propdlg.pch" /Yc /Fo"$(INTDIR)/"\
 /Fd"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\Stdafx.obj" : $(SOURCE) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\Propdlg.pch" : $(SOURCE) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "PropDlg - Macintosh Debug"

# ADD CPP /Yc

BuildCmds= \
	$(CPP) /nologo /AL /Q68s /Q68m /W3 /GX /Zi /Od /D "_WINDOWS" /D "_MAC" /D\
 "_68K_" /D "WIN32" /D "_DEBUG" /D "_MBCS" /Fp"$(INTDIR)/Propdlg.pch" /Yc\
 /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\Stdafx.obj" : $(SOURCE) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\vc40.pdb" : $(SOURCE) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\Propdlg.pch" : $(SOURCE) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "PropDlg - Power Macintosh Release"

# ADD CPP /Yc"Stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "_MAC" /D "_MPPC_" /D "_WINDOWS"\
 /D "WIN32" /D "_MBCS" /D "_AFXDLL" /Fp"$(INTDIR)/Propdlg.pch" /Yc"Stdafx.h"\
 /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\Stdafx.obj" : $(SOURCE) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\Propdlg.pch" : $(SOURCE) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "PropDlg - Power Macintosh Debug"

DEP_CPP_STDAF=\
	".\Stdafx.h"\
	
# ADD CPP /Yc

BuildCmds= \
	$(CPP) /nologo /MDd /W3 /GX /Zi /Od /D "_DEBUG" /D "_MAC" /D "_MPPC_" /D\
 "_WINDOWS" /D "WIN32" /D "_MBCS" /D "_AFXDLL" /Fp"$(INTDIR)/Propdlg.pch" /Yc\
 /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\Stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\Propdlg.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "PropDlg - Win32 Unicode Release"

DEP_CPP_STDAF=\
	".\Stdafx.h"\
	
# ADD CPP /Yc

BuildCmds= \
	$(CPP) /nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D\
 "_UNICODE" /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/Propdlg.pch" /Yc\
 /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\Stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\vc40.idb" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\Propdlg.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "PropDlg - Win32 Unicode Debug"

DEP_CPP_STDAF=\
	".\Stdafx.h"\
	
# ADD CPP /Yc

BuildCmds= \
	$(CPP) /nologo /MDd /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS"\
 /D "_UNICODE" /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/Propdlg.pch" /Yc\
 /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\Stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\vc40.idb" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\Propdlg.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Stylepge.cpp

!IF  "$(CFG)" == "PropDlg - Win32 Release"


"$(INTDIR)\Stylepge.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Propdlg.pch"

"$(INTDIR)\vc40.idb" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Propdlg.pch"


!ELSEIF  "$(CFG)" == "PropDlg - Win32 Debug"


"$(INTDIR)\Stylepge.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Propdlg.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Propdlg.pch"

"$(INTDIR)\vc40.idb" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Propdlg.pch"


!ELSEIF  "$(CFG)" == "PropDlg - Macintosh Release"


"$(INTDIR)\Stylepge.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Propdlg.pch"


!ELSEIF  "$(CFG)" == "PropDlg - Macintosh Debug"


"$(INTDIR)\Stylepge.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Propdlg.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Propdlg.pch"


!ELSEIF  "$(CFG)" == "PropDlg - Power Macintosh Release"


"$(INTDIR)\Stylepge.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Propdlg.pch"


!ELSEIF  "$(CFG)" == "PropDlg - Power Macintosh Debug"

DEP_CPP_STYLE=\
	".\Colorpge.h"\
	".\Preview.h"\
	".\Propdlg.h"\
	".\Propsht.h"\
	".\Propsht2.h"\
	".\Shapeobj.h"\
	".\Stdafx.h"\
	".\Stylepge.h"\
	

"$(INTDIR)\Stylepge.obj" : $(SOURCE) $(DEP_CPP_STYLE) "$(INTDIR)"\
 "$(INTDIR)\Propdlg.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_STYLE) "$(INTDIR)"\
 "$(INTDIR)\Propdlg.pch"


!ELSEIF  "$(CFG)" == "PropDlg - Win32 Unicode Release"

DEP_CPP_STYLE=\
	".\Stdafx.h"\
	".\Propdlg.h"\
	".\Stylepge.h"\
	".\Colorpge.h"\
	".\Shapeobj.h"\
	".\Preview.h"\
	".\Propsht.h"\
	".\Propsht2.h"\
	

"$(INTDIR)\Stylepge.obj" : $(SOURCE) $(DEP_CPP_STYLE) "$(INTDIR)"\
 "$(INTDIR)\Propdlg.pch"

"$(INTDIR)\vc40.idb" : $(SOURCE) $(DEP_CPP_STYLE) "$(INTDIR)"\
 "$(INTDIR)\Propdlg.pch"


!ELSEIF  "$(CFG)" == "PropDlg - Win32 Unicode Debug"

DEP_CPP_STYLE=\
	".\Stdafx.h"\
	".\Propdlg.h"\
	".\Stylepge.h"\
	".\Colorpge.h"\
	".\Shapeobj.h"\
	".\Preview.h"\
	".\Propsht.h"\
	".\Propsht2.h"\
	

"$(INTDIR)\Stylepge.obj" : $(SOURCE) $(DEP_CPP_STYLE) "$(INTDIR)"\
 "$(INTDIR)\Propdlg.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_STYLE) "$(INTDIR)"\
 "$(INTDIR)\Propdlg.pch"

"$(INTDIR)\vc40.idb" : $(SOURCE) $(DEP_CPP_STYLE) "$(INTDIR)"\
 "$(INTDIR)\Propdlg.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\propmac.r

!IF  "$(CFG)" == "PropDlg - Win32 Release"

!ELSEIF  "$(CFG)" == "PropDlg - Win32 Debug"

!ELSEIF  "$(CFG)" == "PropDlg - Macintosh Release"


"$(INTDIR)\propmac.rsc" : $(SOURCE) "$(INTDIR)"
   $(MRC) /o"$(INTDIR)/propmac.rsc" /D "_68K_" /D "_MAC" /D "NDEBUG" /l 0x409\
 /NOLOGO $(SOURCE)


!ELSEIF  "$(CFG)" == "PropDlg - Macintosh Debug"


"$(INTDIR)\propmac.rsc" : $(SOURCE) "$(INTDIR)"
   $(MRC) /o"$(INTDIR)/propmac.rsc" /D "_68K_" /D "_MAC" /D "_DEBUG" /l 0x409\
 /NOLOGO $(SOURCE)


!ELSEIF  "$(CFG)" == "PropDlg - Power Macintosh Release"


"$(INTDIR)\propmac.rsc" : $(SOURCE) "$(INTDIR)"
   $(MRC) /o"$(INTDIR)/propmac.rsc" /D "_MPPC_" /D "_MAC" /D "NDEBUG" /l 0x409\
 /NOLOGO $(SOURCE)


!ELSEIF  "$(CFG)" == "PropDlg - Power Macintosh Debug"


"$(INTDIR)\propmac.rsc" : $(SOURCE) "$(INTDIR)"
   $(MRC) /o"$(INTDIR)/propmac.rsc" /D "_MPPC_" /D "_MAC" /D "_DEBUG" /l 0x409\
 /NOLOGO $(SOURCE)


!ELSEIF  "$(CFG)" == "PropDlg - Win32 Unicode Release"

!ELSEIF  "$(CFG)" == "PropDlg - Win32 Unicode Debug"

!ENDIF 

# End Source File
# End Target
# End Project
################################################################################
